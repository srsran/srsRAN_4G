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

#include "srsran/asn1/rrc/phy_ded.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// PeriodicityStartPos-r16 ::= CHOICE
void periodicity_start_pos_r16_c::destroy_() {}
void periodicity_start_pos_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
periodicity_start_pos_r16_c::periodicity_start_pos_r16_c(const periodicity_start_pos_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::periodicity10ms:
      break;
    case types::periodicity20ms:
      c.init(other.c.get<uint8_t>());
      break;
    case types::periodicity40ms:
      c.init(other.c.get<uint8_t>());
      break;
    case types::periodicity80ms:
      c.init(other.c.get<uint8_t>());
      break;
    case types::periodicity160ms:
      c.init(other.c.get<uint8_t>());
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "periodicity_start_pos_r16_c");
  }
}
periodicity_start_pos_r16_c& periodicity_start_pos_r16_c::operator=(const periodicity_start_pos_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::periodicity10ms:
      break;
    case types::periodicity20ms:
      c.set(other.c.get<uint8_t>());
      break;
    case types::periodicity40ms:
      c.set(other.c.get<uint8_t>());
      break;
    case types::periodicity80ms:
      c.set(other.c.get<uint8_t>());
      break;
    case types::periodicity160ms:
      c.set(other.c.get<uint8_t>());
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "periodicity_start_pos_r16_c");
  }

  return *this;
}
void periodicity_start_pos_r16_c::set_periodicity10ms()
{
  set(types::periodicity10ms);
}
uint8_t& periodicity_start_pos_r16_c::set_periodicity20ms()
{
  set(types::periodicity20ms);
  return c.get<uint8_t>();
}
uint8_t& periodicity_start_pos_r16_c::set_periodicity40ms()
{
  set(types::periodicity40ms);
  return c.get<uint8_t>();
}
uint8_t& periodicity_start_pos_r16_c::set_periodicity80ms()
{
  set(types::periodicity80ms);
  return c.get<uint8_t>();
}
uint8_t& periodicity_start_pos_r16_c::set_periodicity160ms()
{
  set(types::periodicity160ms);
  return c.get<uint8_t>();
}
void periodicity_start_pos_r16_c::set_spare3()
{
  set(types::spare3);
}
void periodicity_start_pos_r16_c::set_spare2()
{
  set(types::spare2);
}
void periodicity_start_pos_r16_c::set_spare1()
{
  set(types::spare1);
}
void periodicity_start_pos_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::periodicity10ms:
      break;
    case types::periodicity20ms:
      j.write_int("periodicity20ms", c.get<uint8_t>());
      break;
    case types::periodicity40ms:
      j.write_int("periodicity40ms", c.get<uint8_t>());
      break;
    case types::periodicity80ms:
      j.write_int("periodicity80ms", c.get<uint8_t>());
      break;
    case types::periodicity160ms:
      j.write_int("periodicity160ms", c.get<uint8_t>());
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "periodicity_start_pos_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE periodicity_start_pos_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::periodicity10ms:
      break;
    case types::periodicity20ms:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)1u));
      break;
    case types::periodicity40ms:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    case types::periodicity80ms:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)7u));
      break;
    case types::periodicity160ms:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "periodicity_start_pos_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE periodicity_start_pos_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::periodicity10ms:
      break;
    case types::periodicity20ms:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)1u));
      break;
    case types::periodicity40ms:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    case types::periodicity80ms:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::periodicity160ms:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "periodicity_start_pos_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ResourceReservationConfigDL-r16 ::= SEQUENCE
SRSASN_CODE res_reserv_cfg_dl_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(res_reserv_freq_r16_present, 1));
  HANDLE_CODE(bref.pack(symbol_bitmap1_r16_present, 1));
  HANDLE_CODE(bref.pack(symbol_bitmap2_r16_present, 1));

  HANDLE_CODE(periodicity_start_pos_r16.pack(bref));
  if (res_reserv_freq_r16_present) {
    HANDLE_CODE(res_reserv_freq_r16.pack(bref));
  }
  HANDLE_CODE(slot_bitmap_r16.pack(bref));
  if (symbol_bitmap1_r16_present) {
    HANDLE_CODE(symbol_bitmap1_r16.pack(bref));
  }
  if (symbol_bitmap2_r16_present) {
    HANDLE_CODE(symbol_bitmap2_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE res_reserv_cfg_dl_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(res_reserv_freq_r16_present, 1));
  HANDLE_CODE(bref.unpack(symbol_bitmap1_r16_present, 1));
  HANDLE_CODE(bref.unpack(symbol_bitmap2_r16_present, 1));

  HANDLE_CODE(periodicity_start_pos_r16.unpack(bref));
  if (res_reserv_freq_r16_present) {
    HANDLE_CODE(res_reserv_freq_r16.unpack(bref));
  }
  HANDLE_CODE(slot_bitmap_r16.unpack(bref));
  if (symbol_bitmap1_r16_present) {
    HANDLE_CODE(symbol_bitmap1_r16.unpack(bref));
  }
  if (symbol_bitmap2_r16_present) {
    HANDLE_CODE(symbol_bitmap2_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void res_reserv_cfg_dl_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("periodicityStartPos-r16");
  periodicity_start_pos_r16.to_json(j);
  if (res_reserv_freq_r16_present) {
    j.write_fieldname("resourceReservationFreq-r16");
    res_reserv_freq_r16.to_json(j);
  }
  j.write_fieldname("slotBitmap-r16");
  slot_bitmap_r16.to_json(j);
  if (symbol_bitmap1_r16_present) {
    j.write_str("symbolBitmap1-r16", symbol_bitmap1_r16.to_string());
  }
  if (symbol_bitmap2_r16_present) {
    j.write_str("symbolBitmap2-r16", symbol_bitmap2_r16.to_string());
  }
  j.end_obj();
}

void res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_::destroy_()
{
  switch (type_) {
    case types::rbg_bitmap1dot4:
      c.destroy<fixed_bitstring<6> >();
      break;
    case types::rbg_bitmap3:
      c.destroy<fixed_bitstring<8> >();
      break;
    case types::rbg_bitmap5:
      c.destroy<fixed_bitstring<13> >();
      break;
    case types::rbg_bitmap10:
      c.destroy<fixed_bitstring<17> >();
      break;
    case types::rbg_bitmap15:
      c.destroy<fixed_bitstring<19> >();
      break;
    case types::rbg_bitmap20:
      c.destroy<fixed_bitstring<25> >();
      break;
    default:
      break;
  }
}
void res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rbg_bitmap1dot4:
      c.init<fixed_bitstring<6> >();
      break;
    case types::rbg_bitmap3:
      c.init<fixed_bitstring<8> >();
      break;
    case types::rbg_bitmap5:
      c.init<fixed_bitstring<13> >();
      break;
    case types::rbg_bitmap10:
      c.init<fixed_bitstring<17> >();
      break;
    case types::rbg_bitmap15:
      c.init<fixed_bitstring<19> >();
      break;
    case types::rbg_bitmap20:
      c.init<fixed_bitstring<25> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_");
  }
}
res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_::res_reserv_freq_r16_c_(
    const res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rbg_bitmap1dot4:
      c.init(other.c.get<fixed_bitstring<6> >());
      break;
    case types::rbg_bitmap3:
      c.init(other.c.get<fixed_bitstring<8> >());
      break;
    case types::rbg_bitmap5:
      c.init(other.c.get<fixed_bitstring<13> >());
      break;
    case types::rbg_bitmap10:
      c.init(other.c.get<fixed_bitstring<17> >());
      break;
    case types::rbg_bitmap15:
      c.init(other.c.get<fixed_bitstring<19> >());
      break;
    case types::rbg_bitmap20:
      c.init(other.c.get<fixed_bitstring<25> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_");
  }
}
res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_&
res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_::operator=(const res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rbg_bitmap1dot4:
      c.set(other.c.get<fixed_bitstring<6> >());
      break;
    case types::rbg_bitmap3:
      c.set(other.c.get<fixed_bitstring<8> >());
      break;
    case types::rbg_bitmap5:
      c.set(other.c.get<fixed_bitstring<13> >());
      break;
    case types::rbg_bitmap10:
      c.set(other.c.get<fixed_bitstring<17> >());
      break;
    case types::rbg_bitmap15:
      c.set(other.c.get<fixed_bitstring<19> >());
      break;
    case types::rbg_bitmap20:
      c.set(other.c.get<fixed_bitstring<25> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_");
  }

  return *this;
}
fixed_bitstring<6>& res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_::set_rbg_bitmap1dot4()
{
  set(types::rbg_bitmap1dot4);
  return c.get<fixed_bitstring<6> >();
}
fixed_bitstring<8>& res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_::set_rbg_bitmap3()
{
  set(types::rbg_bitmap3);
  return c.get<fixed_bitstring<8> >();
}
fixed_bitstring<13>& res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_::set_rbg_bitmap5()
{
  set(types::rbg_bitmap5);
  return c.get<fixed_bitstring<13> >();
}
fixed_bitstring<17>& res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_::set_rbg_bitmap10()
{
  set(types::rbg_bitmap10);
  return c.get<fixed_bitstring<17> >();
}
fixed_bitstring<19>& res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_::set_rbg_bitmap15()
{
  set(types::rbg_bitmap15);
  return c.get<fixed_bitstring<19> >();
}
fixed_bitstring<25>& res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_::set_rbg_bitmap20()
{
  set(types::rbg_bitmap20);
  return c.get<fixed_bitstring<25> >();
}
void res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rbg_bitmap1dot4:
      j.write_str("rbg-Bitmap1dot4", c.get<fixed_bitstring<6> >().to_string());
      break;
    case types::rbg_bitmap3:
      j.write_str("rbg-Bitmap3", c.get<fixed_bitstring<8> >().to_string());
      break;
    case types::rbg_bitmap5:
      j.write_str("rbg-Bitmap5", c.get<fixed_bitstring<13> >().to_string());
      break;
    case types::rbg_bitmap10:
      j.write_str("rbg-Bitmap10", c.get<fixed_bitstring<17> >().to_string());
      break;
    case types::rbg_bitmap15:
      j.write_str("rbg-Bitmap15", c.get<fixed_bitstring<19> >().to_string());
      break;
    case types::rbg_bitmap20:
      j.write_str("rbg-Bitmap20", c.get<fixed_bitstring<25> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rbg_bitmap1dot4:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().pack(bref));
      break;
    case types::rbg_bitmap3:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().pack(bref));
      break;
    case types::rbg_bitmap5:
      HANDLE_CODE(c.get<fixed_bitstring<13> >().pack(bref));
      break;
    case types::rbg_bitmap10:
      HANDLE_CODE(c.get<fixed_bitstring<17> >().pack(bref));
      break;
    case types::rbg_bitmap15:
      HANDLE_CODE(c.get<fixed_bitstring<19> >().pack(bref));
      break;
    case types::rbg_bitmap20:
      HANDLE_CODE(c.get<fixed_bitstring<25> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rbg_bitmap1dot4:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().unpack(bref));
      break;
    case types::rbg_bitmap3:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().unpack(bref));
      break;
    case types::rbg_bitmap5:
      HANDLE_CODE(c.get<fixed_bitstring<13> >().unpack(bref));
      break;
    case types::rbg_bitmap10:
      HANDLE_CODE(c.get<fixed_bitstring<17> >().unpack(bref));
      break;
    case types::rbg_bitmap15:
      HANDLE_CODE(c.get<fixed_bitstring<19> >().unpack(bref));
      break;
    case types::rbg_bitmap20:
      HANDLE_CODE(c.get<fixed_bitstring<25> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_dl_r16_s::res_reserv_freq_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_::destroy_()
{
  switch (type_) {
    case types::slot_pattern10ms:
      c.destroy<fixed_bitstring<20> >();
      break;
    case types::slot_pattern40ms:
      c.destroy<fixed_bitstring<80> >();
      break;
    default:
      break;
  }
}
void res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::slot_pattern10ms:
      c.init<fixed_bitstring<20> >();
      break;
    case types::slot_pattern40ms:
      c.init<fixed_bitstring<80> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_");
  }
}
res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_::slot_bitmap_r16_c_(
    const res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::slot_pattern10ms:
      c.init(other.c.get<fixed_bitstring<20> >());
      break;
    case types::slot_pattern40ms:
      c.init(other.c.get<fixed_bitstring<80> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_");
  }
}
res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_&
res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_::operator=(const res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::slot_pattern10ms:
      c.set(other.c.get<fixed_bitstring<20> >());
      break;
    case types::slot_pattern40ms:
      c.set(other.c.get<fixed_bitstring<80> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_");
  }

  return *this;
}
fixed_bitstring<20>& res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_::set_slot_pattern10ms()
{
  set(types::slot_pattern10ms);
  return c.get<fixed_bitstring<20> >();
}
fixed_bitstring<80>& res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_::set_slot_pattern40ms()
{
  set(types::slot_pattern40ms);
  return c.get<fixed_bitstring<80> >();
}
void res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::slot_pattern10ms:
      j.write_str("slotPattern10ms", c.get<fixed_bitstring<20> >().to_string());
      break;
    case types::slot_pattern40ms:
      j.write_str("slotPattern40ms", c.get<fixed_bitstring<80> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::slot_pattern10ms:
      HANDLE_CODE(c.get<fixed_bitstring<20> >().pack(bref));
      break;
    case types::slot_pattern40ms:
      HANDLE_CODE(c.get<fixed_bitstring<80> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::slot_pattern10ms:
      HANDLE_CODE(c.get<fixed_bitstring<20> >().unpack(bref));
      break;
    case types::slot_pattern40ms:
      HANDLE_CODE(c.get<fixed_bitstring<80> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_dl_r16_s::slot_bitmap_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ResourceReservationConfigUL-r16 ::= SEQUENCE
SRSASN_CODE res_reserv_cfg_ul_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(slot_bitmap_r16_present, 1));
  HANDLE_CODE(bref.pack(symbol_bitmap1_r16_present, 1));
  HANDLE_CODE(bref.pack(symbol_bitmap2_r16_present, 1));

  HANDLE_CODE(periodicity_start_pos_r16.pack(bref));
  if (slot_bitmap_r16_present) {
    HANDLE_CODE(slot_bitmap_r16.pack(bref));
  }
  if (symbol_bitmap1_r16_present) {
    HANDLE_CODE(symbol_bitmap1_r16.pack(bref));
  }
  if (symbol_bitmap2_r16_present) {
    HANDLE_CODE(symbol_bitmap2_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE res_reserv_cfg_ul_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(slot_bitmap_r16_present, 1));
  HANDLE_CODE(bref.unpack(symbol_bitmap1_r16_present, 1));
  HANDLE_CODE(bref.unpack(symbol_bitmap2_r16_present, 1));

  HANDLE_CODE(periodicity_start_pos_r16.unpack(bref));
  if (slot_bitmap_r16_present) {
    HANDLE_CODE(slot_bitmap_r16.unpack(bref));
  }
  if (symbol_bitmap1_r16_present) {
    HANDLE_CODE(symbol_bitmap1_r16.unpack(bref));
  }
  if (symbol_bitmap2_r16_present) {
    HANDLE_CODE(symbol_bitmap2_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void res_reserv_cfg_ul_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("periodicityStartPos-r16");
  periodicity_start_pos_r16.to_json(j);
  if (slot_bitmap_r16_present) {
    j.write_fieldname("slotBitmap-r16");
    slot_bitmap_r16.to_json(j);
  }
  if (symbol_bitmap1_r16_present) {
    j.write_str("symbolBitmap1-r16", symbol_bitmap1_r16.to_string());
  }
  if (symbol_bitmap2_r16_present) {
    j.write_str("symbolBitmap2-r16", symbol_bitmap2_r16.to_string());
  }
  j.end_obj();
}

void res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_::destroy_()
{
  switch (type_) {
    case types::slot_pattern10ms:
      c.destroy<fixed_bitstring<20> >();
      break;
    case types::slot_pattern40ms:
      c.destroy<fixed_bitstring<80> >();
      break;
    default:
      break;
  }
}
void res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::slot_pattern10ms:
      c.init<fixed_bitstring<20> >();
      break;
    case types::slot_pattern40ms:
      c.init<fixed_bitstring<80> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_");
  }
}
res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_::slot_bitmap_r16_c_(
    const res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::slot_pattern10ms:
      c.init(other.c.get<fixed_bitstring<20> >());
      break;
    case types::slot_pattern40ms:
      c.init(other.c.get<fixed_bitstring<80> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_");
  }
}
res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_&
res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_::operator=(const res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::slot_pattern10ms:
      c.set(other.c.get<fixed_bitstring<20> >());
      break;
    case types::slot_pattern40ms:
      c.set(other.c.get<fixed_bitstring<80> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_");
  }

  return *this;
}
fixed_bitstring<20>& res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_::set_slot_pattern10ms()
{
  set(types::slot_pattern10ms);
  return c.get<fixed_bitstring<20> >();
}
fixed_bitstring<80>& res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_::set_slot_pattern40ms()
{
  set(types::slot_pattern40ms);
  return c.get<fixed_bitstring<80> >();
}
void res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::slot_pattern10ms:
      j.write_str("slotPattern10ms", c.get<fixed_bitstring<20> >().to_string());
      break;
    case types::slot_pattern40ms:
      j.write_str("slotPattern40ms", c.get<fixed_bitstring<80> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::slot_pattern10ms:
      HANDLE_CODE(c.get<fixed_bitstring<20> >().pack(bref));
      break;
    case types::slot_pattern40ms:
      HANDLE_CODE(c.get<fixed_bitstring<80> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::slot_pattern10ms:
      HANDLE_CODE(c.get<fixed_bitstring<20> >().unpack(bref));
      break;
    case types::slot_pattern40ms:
      HANDLE_CODE(c.get<fixed_bitstring<80> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "res_reserv_cfg_ul_r16_s::slot_bitmap_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// NZP-FrequencyDensity-r14 ::= ENUMERATED
const char* nzp_freq_density_r14_opts::to_string() const
{
  static const char* options[] = {"d1", "d2", "d3"};
  return convert_enum_idx(options, 3, value, "nzp_freq_density_r14_e");
}
uint8_t nzp_freq_density_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(options, 3, value, "nzp_freq_density_r14_e");
}

// P-C-AndCBSR-r13 ::= SEQUENCE
SRSASN_CODE p_c_and_cbsr_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, p_c_r13, (int8_t)-8, (int8_t)15));
  HANDLE_CODE(cbsr_sel_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE p_c_and_cbsr_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(p_c_r13, bref, (int8_t)-8, (int8_t)15));
  HANDLE_CODE(cbsr_sel_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void p_c_and_cbsr_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p-C-r13", p_c_r13);
  j.write_fieldname("cbsr-Selection-r13");
  cbsr_sel_r13.to_json(j);
  j.end_obj();
}
bool p_c_and_cbsr_r13_s::operator==(const p_c_and_cbsr_r13_s& other) const
{
  return ext == other.ext and p_c_r13 == other.p_c_r13 and cbsr_sel_r13 == other.cbsr_sel_r13;
}

void p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::destroy_()
{
  switch (type_) {
    case types::non_precoded_r13:
      c.destroy<non_precoded_r13_s_>();
      break;
    case types::bf_k1a_r13:
      c.destroy<bf_k1a_r13_s_>();
      break;
    case types::bf_kn_r13:
      c.destroy<bf_kn_r13_s_>();
      break;
    default:
      break;
  }
}
void p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::non_precoded_r13:
      c.init<non_precoded_r13_s_>();
      break;
    case types::bf_k1a_r13:
      c.init<bf_k1a_r13_s_>();
      break;
    case types::bf_kn_r13:
      c.init<bf_kn_r13_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "p_c_and_cbsr_r13_s::cbsr_sel_r13_c_");
  }
}
p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::cbsr_sel_r13_c_(const p_c_and_cbsr_r13_s::cbsr_sel_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::non_precoded_r13:
      c.init(other.c.get<non_precoded_r13_s_>());
      break;
    case types::bf_k1a_r13:
      c.init(other.c.get<bf_k1a_r13_s_>());
      break;
    case types::bf_kn_r13:
      c.init(other.c.get<bf_kn_r13_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "p_c_and_cbsr_r13_s::cbsr_sel_r13_c_");
  }
}
p_c_and_cbsr_r13_s::cbsr_sel_r13_c_&
p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::operator=(const p_c_and_cbsr_r13_s::cbsr_sel_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::non_precoded_r13:
      c.set(other.c.get<non_precoded_r13_s_>());
      break;
    case types::bf_k1a_r13:
      c.set(other.c.get<bf_k1a_r13_s_>());
      break;
    case types::bf_kn_r13:
      c.set(other.c.get<bf_kn_r13_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "p_c_and_cbsr_r13_s::cbsr_sel_r13_c_");
  }

  return *this;
}
p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::non_precoded_r13_s_& p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::set_non_precoded_r13()
{
  set(types::non_precoded_r13);
  return c.get<non_precoded_r13_s_>();
}
p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::bf_k1a_r13_s_& p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::set_bf_k1a_r13()
{
  set(types::bf_k1a_r13);
  return c.get<bf_k1a_r13_s_>();
}
p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::bf_kn_r13_s_& p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::set_bf_kn_r13()
{
  set(types::bf_kn_r13);
  return c.get<bf_kn_r13_s_>();
}
void p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::non_precoded_r13:
      j.write_fieldname("nonPrecoded-r13");
      j.start_obj();
      j.write_str("codebookSubsetRestriction1-r13",
                  c.get<non_precoded_r13_s_>().codebook_subset_restrict1_r13.to_string());
      j.write_str("codebookSubsetRestriction2-r13",
                  c.get<non_precoded_r13_s_>().codebook_subset_restrict2_r13.to_string());
      j.end_obj();
      break;
    case types::bf_k1a_r13:
      j.write_fieldname("beamformedK1a-r13");
      j.start_obj();
      j.write_str("codebookSubsetRestriction3-r13", c.get<bf_k1a_r13_s_>().codebook_subset_restrict3_r13.to_string());
      j.end_obj();
      break;
    case types::bf_kn_r13:
      j.write_fieldname("beamformedKN-r13");
      j.start_obj();
      j.write_str("codebookSubsetRestriction-r13", c.get<bf_kn_r13_s_>().codebook_subset_restrict_r13.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "p_c_and_cbsr_r13_s::cbsr_sel_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::non_precoded_r13:
      HANDLE_CODE(c.get<non_precoded_r13_s_>().codebook_subset_restrict1_r13.pack(bref));
      HANDLE_CODE(c.get<non_precoded_r13_s_>().codebook_subset_restrict2_r13.pack(bref));
      break;
    case types::bf_k1a_r13:
      HANDLE_CODE(c.get<bf_k1a_r13_s_>().codebook_subset_restrict3_r13.pack(bref));
      break;
    case types::bf_kn_r13:
      HANDLE_CODE(c.get<bf_kn_r13_s_>().codebook_subset_restrict_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "p_c_and_cbsr_r13_s::cbsr_sel_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::non_precoded_r13:
      HANDLE_CODE(c.get<non_precoded_r13_s_>().codebook_subset_restrict1_r13.unpack(bref));
      HANDLE_CODE(c.get<non_precoded_r13_s_>().codebook_subset_restrict2_r13.unpack(bref));
      break;
    case types::bf_k1a_r13:
      HANDLE_CODE(c.get<bf_k1a_r13_s_>().codebook_subset_restrict3_r13.unpack(bref));
      break;
    case types::bf_kn_r13:
      HANDLE_CODE(c.get<bf_kn_r13_s_>().codebook_subset_restrict_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "p_c_and_cbsr_r13_s::cbsr_sel_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::operator==(const cbsr_sel_r13_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::non_precoded_r13:
      return c.get<non_precoded_r13_s_>().codebook_subset_restrict1_r13 ==
                 other.c.get<non_precoded_r13_s_>().codebook_subset_restrict1_r13 and
             c.get<non_precoded_r13_s_>().codebook_subset_restrict2_r13 ==
                 other.c.get<non_precoded_r13_s_>().codebook_subset_restrict2_r13;
    case types::bf_k1a_r13:
      return c.get<bf_k1a_r13_s_>().codebook_subset_restrict3_r13 ==
             other.c.get<bf_k1a_r13_s_>().codebook_subset_restrict3_r13;
    case types::bf_kn_r13:
      return c.get<bf_kn_r13_s_>().codebook_subset_restrict_r13 ==
             other.c.get<bf_kn_r13_s_>().codebook_subset_restrict_r13;
    default:
      return true;
  }
  return true;
}

// CSI-RS-Config-NZP-v1430 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_nzp_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tx_comb_r14_present, 1));
  HANDLE_CODE(bref.pack(freq_density_r14_present, 1));

  if (tx_comb_r14_present) {
    HANDLE_CODE(pack_integer(bref, tx_comb_r14, (uint8_t)0u, (uint8_t)2u));
  }
  if (freq_density_r14_present) {
    HANDLE_CODE(freq_density_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_nzp_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tx_comb_r14_present, 1));
  HANDLE_CODE(bref.unpack(freq_density_r14_present, 1));

  if (tx_comb_r14_present) {
    HANDLE_CODE(unpack_integer(tx_comb_r14, bref, (uint8_t)0u, (uint8_t)2u));
  }
  if (freq_density_r14_present) {
    HANDLE_CODE(freq_density_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_nzp_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tx_comb_r14_present) {
    j.write_int("transmissionComb-r14", tx_comb_r14);
  }
  if (freq_density_r14_present) {
    j.write_str("frequencyDensity-r14", freq_density_r14.to_string());
  }
  j.end_obj();
}
bool csi_rs_cfg_nzp_v1430_s::operator==(const csi_rs_cfg_nzp_v1430_s& other) const
{
  return tx_comb_r14_present == other.tx_comb_r14_present and
         (not tx_comb_r14_present or tx_comb_r14 == other.tx_comb_r14) and
         freq_density_r14_present == other.freq_density_r14_present and
         (not freq_density_r14_present or freq_density_r14 == other.freq_density_r14);
}

// CSI-RS-ConfigNZP-Activation-r14 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_nzp_activation_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(csi_rs_nzp_mode_r14.pack(bref));
  HANDLE_CODE(pack_integer(bref, activ_res_r14, (uint8_t)0u, (uint8_t)4u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_nzp_activation_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(csi_rs_nzp_mode_r14.unpack(bref));
  HANDLE_CODE(unpack_integer(activ_res_r14, bref, (uint8_t)0u, (uint8_t)4u));

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_nzp_activation_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("csi-RS-NZP-mode-r14", csi_rs_nzp_mode_r14.to_string());
  j.write_int("activatedResources-r14", activ_res_r14);
  j.end_obj();
}
bool csi_rs_cfg_nzp_activation_r14_s::operator==(const csi_rs_cfg_nzp_activation_r14_s& other) const
{
  return csi_rs_nzp_mode_r14 == other.csi_rs_nzp_mode_r14 and activ_res_r14 == other.activ_res_r14;
}

const char* csi_rs_cfg_nzp_activation_r14_s::csi_rs_nzp_mode_r14_opts::to_string() const
{
  static const char* options[] = {"semiPersistent", "aperiodic"};
  return convert_enum_idx(options, 2, value, "csi_rs_cfg_nzp_activation_r14_s::csi_rs_nzp_mode_r14_e_");
}

// CSI-RS-ConfigNZP-r11 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_nzp_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(qcl_crs_info_r11_present, 1));

  HANDLE_CODE(pack_integer(bref, csi_rs_cfg_nzp_id_r11, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(ant_ports_count_r11.pack(bref));
  HANDLE_CODE(pack_integer(bref, res_cfg_r11, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, sf_cfg_r11, (uint8_t)0u, (uint8_t)154u));
  HANDLE_CODE(pack_integer(bref, scrambling_id_r11, (uint16_t)0u, (uint16_t)503u));
  if (qcl_crs_info_r11_present) {
    HANDLE_CODE(bref.pack(qcl_crs_info_r11.mbsfn_sf_cfg_list_r11_present, 1));
    HANDLE_CODE(pack_integer(bref, qcl_crs_info_r11.qcl_scrambling_id_r11, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(qcl_crs_info_r11.crs_ports_count_r11.pack(bref));
    if (qcl_crs_info_r11.mbsfn_sf_cfg_list_r11_present) {
      HANDLE_CODE(qcl_crs_info_r11.mbsfn_sf_cfg_list_r11.pack(bref));
    }
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= csi_rs_cfg_nzp_id_v1310_present;
    group_flags[1] |= tx_comb_r14_present;
    group_flags[1] |= freq_density_r14_present;
    group_flags[2] |= mbsfn_sf_cfg_list_v1430.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_id_v1310_present, 1));
      if (csi_rs_cfg_nzp_id_v1310_present) {
        HANDLE_CODE(pack_integer(bref, csi_rs_cfg_nzp_id_v1310, (uint8_t)4u, (uint8_t)24u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(tx_comb_r14_present, 1));
      HANDLE_CODE(bref.pack(freq_density_r14_present, 1));
      if (tx_comb_r14_present) {
        HANDLE_CODE(pack_integer(bref, tx_comb_r14, (uint8_t)0u, (uint8_t)2u));
      }
      if (freq_density_r14_present) {
        HANDLE_CODE(freq_density_r14.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mbsfn_sf_cfg_list_v1430.is_present(), 1));
      if (mbsfn_sf_cfg_list_v1430.is_present()) {
        HANDLE_CODE(mbsfn_sf_cfg_list_v1430->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_nzp_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(qcl_crs_info_r11_present, 1));

  HANDLE_CODE(unpack_integer(csi_rs_cfg_nzp_id_r11, bref, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(ant_ports_count_r11.unpack(bref));
  HANDLE_CODE(unpack_integer(res_cfg_r11, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(sf_cfg_r11, bref, (uint8_t)0u, (uint8_t)154u));
  HANDLE_CODE(unpack_integer(scrambling_id_r11, bref, (uint16_t)0u, (uint16_t)503u));
  if (qcl_crs_info_r11_present) {
    HANDLE_CODE(bref.unpack(qcl_crs_info_r11.mbsfn_sf_cfg_list_r11_present, 1));
    HANDLE_CODE(unpack_integer(qcl_crs_info_r11.qcl_scrambling_id_r11, bref, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(qcl_crs_info_r11.crs_ports_count_r11.unpack(bref));
    if (qcl_crs_info_r11.mbsfn_sf_cfg_list_r11_present) {
      HANDLE_CODE(qcl_crs_info_r11.mbsfn_sf_cfg_list_r11.unpack(bref));
    }
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_id_v1310_present, 1));
      if (csi_rs_cfg_nzp_id_v1310_present) {
        HANDLE_CODE(unpack_integer(csi_rs_cfg_nzp_id_v1310, bref, (uint8_t)4u, (uint8_t)24u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(tx_comb_r14_present, 1));
      HANDLE_CODE(bref.unpack(freq_density_r14_present, 1));
      if (tx_comb_r14_present) {
        HANDLE_CODE(unpack_integer(tx_comb_r14, bref, (uint8_t)0u, (uint8_t)2u));
      }
      if (freq_density_r14_present) {
        HANDLE_CODE(freq_density_r14.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mbsfn_sf_cfg_list_v1430_present;
      HANDLE_CODE(bref.unpack(mbsfn_sf_cfg_list_v1430_present, 1));
      mbsfn_sf_cfg_list_v1430.set_present(mbsfn_sf_cfg_list_v1430_present);
      if (mbsfn_sf_cfg_list_v1430.is_present()) {
        HANDLE_CODE(mbsfn_sf_cfg_list_v1430->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void csi_rs_cfg_nzp_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-RS-ConfigNZPId-r11", csi_rs_cfg_nzp_id_r11);
  j.write_str("antennaPortsCount-r11", ant_ports_count_r11.to_string());
  j.write_int("resourceConfig-r11", res_cfg_r11);
  j.write_int("subframeConfig-r11", sf_cfg_r11);
  j.write_int("scramblingIdentity-r11", scrambling_id_r11);
  if (qcl_crs_info_r11_present) {
    j.write_fieldname("qcl-CRS-Info-r11");
    j.start_obj();
    j.write_int("qcl-ScramblingIdentity-r11", qcl_crs_info_r11.qcl_scrambling_id_r11);
    j.write_str("crs-PortsCount-r11", qcl_crs_info_r11.crs_ports_count_r11.to_string());
    if (qcl_crs_info_r11.mbsfn_sf_cfg_list_r11_present) {
      j.write_fieldname("mbsfn-SubframeConfigList-r11");
      qcl_crs_info_r11.mbsfn_sf_cfg_list_r11.to_json(j);
    }
    j.end_obj();
  }
  if (ext) {
    if (csi_rs_cfg_nzp_id_v1310_present) {
      j.write_int("csi-RS-ConfigNZPId-v1310", csi_rs_cfg_nzp_id_v1310);
    }
    if (tx_comb_r14_present) {
      j.write_int("transmissionComb-r14", tx_comb_r14);
    }
    if (freq_density_r14_present) {
      j.write_str("frequencyDensity-r14", freq_density_r14.to_string());
    }
    if (mbsfn_sf_cfg_list_v1430.is_present()) {
      j.write_fieldname("mbsfn-SubframeConfigList-v1430");
      mbsfn_sf_cfg_list_v1430->to_json(j);
    }
  }
  j.end_obj();
}
bool csi_rs_cfg_nzp_r11_s::operator==(const csi_rs_cfg_nzp_r11_s& other) const
{
  return ext == other.ext and csi_rs_cfg_nzp_id_r11 == other.csi_rs_cfg_nzp_id_r11 and
         ant_ports_count_r11 == other.ant_ports_count_r11 and res_cfg_r11 == other.res_cfg_r11 and
         sf_cfg_r11 == other.sf_cfg_r11 and scrambling_id_r11 == other.scrambling_id_r11 and
         qcl_crs_info_r11.qcl_scrambling_id_r11 == other.qcl_crs_info_r11.qcl_scrambling_id_r11 and
         qcl_crs_info_r11.crs_ports_count_r11 == other.qcl_crs_info_r11.crs_ports_count_r11 and
         qcl_crs_info_r11.mbsfn_sf_cfg_list_r11_present == other.qcl_crs_info_r11.mbsfn_sf_cfg_list_r11_present and
         (not qcl_crs_info_r11.mbsfn_sf_cfg_list_r11_present or
          qcl_crs_info_r11.mbsfn_sf_cfg_list_r11 == other.qcl_crs_info_r11.mbsfn_sf_cfg_list_r11) and
         (not ext or
          (csi_rs_cfg_nzp_id_v1310_present == other.csi_rs_cfg_nzp_id_v1310_present and
           (not csi_rs_cfg_nzp_id_v1310_present or csi_rs_cfg_nzp_id_v1310 == other.csi_rs_cfg_nzp_id_v1310) and
           tx_comb_r14_present == other.tx_comb_r14_present and
           (not tx_comb_r14_present or tx_comb_r14 == other.tx_comb_r14) and
           freq_density_r14_present == other.freq_density_r14_present and
           (not freq_density_r14_present or freq_density_r14 == other.freq_density_r14) and
           mbsfn_sf_cfg_list_v1430.is_present() == other.mbsfn_sf_cfg_list_v1430.is_present() and
           (not mbsfn_sf_cfg_list_v1430.is_present() or *mbsfn_sf_cfg_list_v1430 == *other.mbsfn_sf_cfg_list_v1430)));
}

const char* csi_rs_cfg_nzp_r11_s::ant_ports_count_r11_opts::to_string() const
{
  static const char* options[] = {"an1", "an2", "an4", "an8"};
  return convert_enum_idx(options, 4, value, "csi_rs_cfg_nzp_r11_s::ant_ports_count_r11_e_");
}
uint8_t csi_rs_cfg_nzp_r11_s::ant_ports_count_r11_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(options, 4, value, "csi_rs_cfg_nzp_r11_s::ant_ports_count_r11_e_");
}

const char* csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::crs_ports_count_r11_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "spare1"};
  return convert_enum_idx(options, 4, value, "csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::crs_ports_count_r11_e_");
}
uint8_t csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::crs_ports_count_r11_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::crs_ports_count_r11_e_");
}

void csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::mbsfn_sf_cfg_list_r11_c_::set(types::options e)
{
  type_ = e;
}
void csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::mbsfn_sf_cfg_list_r11_c_::set_release()
{
  set(types::release);
}
csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::mbsfn_sf_cfg_list_r11_c_::setup_s_&
csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::mbsfn_sf_cfg_list_r11_c_::set_setup()
{
  set(types::setup);
  return c;
}
void csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::mbsfn_sf_cfg_list_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("subframeConfigList");
      for (const auto& e1 : c.sf_cfg_list) {
        e1.to_json(j);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::mbsfn_sf_cfg_list_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::mbsfn_sf_cfg_list_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.sf_cfg_list, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::mbsfn_sf_cfg_list_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::mbsfn_sf_cfg_list_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.sf_cfg_list, bref, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::mbsfn_sf_cfg_list_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::mbsfn_sf_cfg_list_r11_c_::operator==(
    const mbsfn_sf_cfg_list_r11_c_& other) const
{
  return type() == other.type() and c.sf_cfg_list == other.c.sf_cfg_list;
}

void csi_rs_cfg_nzp_r11_s::mbsfn_sf_cfg_list_v1430_c_::set(types::options e)
{
  type_ = e;
}
void csi_rs_cfg_nzp_r11_s::mbsfn_sf_cfg_list_v1430_c_::set_release()
{
  set(types::release);
}
csi_rs_cfg_nzp_r11_s::mbsfn_sf_cfg_list_v1430_c_::setup_s_&
csi_rs_cfg_nzp_r11_s::mbsfn_sf_cfg_list_v1430_c_::set_setup()
{
  set(types::setup);
  return c;
}
void csi_rs_cfg_nzp_r11_s::mbsfn_sf_cfg_list_v1430_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("subframeConfigList-v1430");
      for (const auto& e1 : c.sf_cfg_list_v1430) {
        e1.to_json(j);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_nzp_r11_s::mbsfn_sf_cfg_list_v1430_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_nzp_r11_s::mbsfn_sf_cfg_list_v1430_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.sf_cfg_list_v1430, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_nzp_r11_s::mbsfn_sf_cfg_list_v1430_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_nzp_r11_s::mbsfn_sf_cfg_list_v1430_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.sf_cfg_list_v1430, bref, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_nzp_r11_s::mbsfn_sf_cfg_list_v1430_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_nzp_r11_s::mbsfn_sf_cfg_list_v1430_c_::operator==(const mbsfn_sf_cfg_list_v1430_c_& other) const
{
  return type() == other.type() and c.sf_cfg_list_v1430 == other.c.sf_cfg_list_v1430;
}

// NZP-ResourceConfig-r13 ::= SEQUENCE
SRSASN_CODE nzp_res_cfg_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, res_cfg_r13, (uint8_t)0u, (uint8_t)31u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= tx_comb_r14_present;
    group_flags[0] |= freq_density_r14_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(tx_comb_r14_present, 1));
      HANDLE_CODE(bref.pack(freq_density_r14_present, 1));
      if (tx_comb_r14_present) {
        HANDLE_CODE(pack_integer(bref, tx_comb_r14, (uint8_t)0u, (uint8_t)2u));
      }
      if (freq_density_r14_present) {
        HANDLE_CODE(freq_density_r14.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE nzp_res_cfg_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(res_cfg_r13, bref, (uint8_t)0u, (uint8_t)31u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(tx_comb_r14_present, 1));
      HANDLE_CODE(bref.unpack(freq_density_r14_present, 1));
      if (tx_comb_r14_present) {
        HANDLE_CODE(unpack_integer(tx_comb_r14, bref, (uint8_t)0u, (uint8_t)2u));
      }
      if (freq_density_r14_present) {
        HANDLE_CODE(freq_density_r14.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void nzp_res_cfg_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("resourceConfig-r13", res_cfg_r13);
  if (ext) {
    if (tx_comb_r14_present) {
      j.write_int("transmissionComb-r14", tx_comb_r14);
    }
    if (freq_density_r14_present) {
      j.write_str("frequencyDensity-r14", freq_density_r14.to_string());
    }
  }
  j.end_obj();
}
bool nzp_res_cfg_r13_s::operator==(const nzp_res_cfg_r13_s& other) const
{
  return ext == other.ext and res_cfg_r13 == other.res_cfg_r13 and
         (not ext or (tx_comb_r14_present == other.tx_comb_r14_present and
                      (not tx_comb_r14_present or tx_comb_r14 == other.tx_comb_r14) and
                      freq_density_r14_present == other.freq_density_r14_present and
                      (not freq_density_r14_present or freq_density_r14 == other.freq_density_r14)));
}

// P-C-AndCBSR-r15 ::= SEQUENCE
SRSASN_CODE p_c_and_cbsr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, p_c_r15, (int8_t)-8, (int8_t)15));
  HANDLE_CODE(codebook_subset_restrict4_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE p_c_and_cbsr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(p_c_r15, bref, (int8_t)-8, (int8_t)15));
  HANDLE_CODE(codebook_subset_restrict4_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void p_c_and_cbsr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p-C-r15", p_c_r15);
  j.write_str("codebookSubsetRestriction4-r15", codebook_subset_restrict4_r15.to_string());
  j.end_obj();
}
bool p_c_and_cbsr_r15_s::operator==(const p_c_and_cbsr_r15_s& other) const
{
  return p_c_r15 == other.p_c_r15 and codebook_subset_restrict4_r15 == other.codebook_subset_restrict4_r15;
}

// CSI-RS-ConfigBeamformed-r14 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_bf_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_id_list_ext_r14_present, 1));
  HANDLE_CODE(bref.pack(csi_im_cfg_id_list_r14_present, 1));
  HANDLE_CODE(bref.pack(p_c_and_cbsr_per_res_cfg_list_r14_present, 1));
  HANDLE_CODE(bref.pack(ace_for4_tx_per_res_cfg_list_r14_present, 1));
  HANDLE_CODE(bref.pack(alt_codebook_enabled_bf_r14_present, 1));
  HANDLE_CODE(bref.pack(ch_meas_restrict_r14_present, 1));
  HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_ap_list_r14_present, 1));
  HANDLE_CODE(bref.pack(nzp_res_cfg_original_v1430_present, 1));
  HANDLE_CODE(bref.pack(csi_rs_nzp_activation_r14_present, 1));

  if (csi_rs_cfg_nzp_id_list_ext_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_rs_cfg_nzp_id_list_ext_r14, 1, 7, integer_packer<uint8_t>(1, 24)));
  }
  if (csi_im_cfg_id_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_im_cfg_id_list_r14, 1, 8, integer_packer<uint8_t>(1, 24)));
  }
  if (p_c_and_cbsr_per_res_cfg_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, p_c_and_cbsr_per_res_cfg_list_r14, 1, 8, SeqOfPacker<Packer>(1, 2, Packer())));
  }
  if (ace_for4_tx_per_res_cfg_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ace_for4_tx_per_res_cfg_list_r14, 1, 7, BitPacker(1)));
  }
  if (csi_rs_cfg_nzp_ap_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_rs_cfg_nzp_ap_list_r14, 1, 8));
  }
  if (nzp_res_cfg_original_v1430_present) {
    HANDLE_CODE(nzp_res_cfg_original_v1430.pack(bref));
  }
  if (csi_rs_nzp_activation_r14_present) {
    HANDLE_CODE(csi_rs_nzp_activation_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_bf_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_id_list_ext_r14_present, 1));
  HANDLE_CODE(bref.unpack(csi_im_cfg_id_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(p_c_and_cbsr_per_res_cfg_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(ace_for4_tx_per_res_cfg_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(alt_codebook_enabled_bf_r14_present, 1));
  HANDLE_CODE(bref.unpack(ch_meas_restrict_r14_present, 1));
  HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_ap_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(nzp_res_cfg_original_v1430_present, 1));
  HANDLE_CODE(bref.unpack(csi_rs_nzp_activation_r14_present, 1));

  if (csi_rs_cfg_nzp_id_list_ext_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_rs_cfg_nzp_id_list_ext_r14, bref, 1, 7, integer_packer<uint8_t>(1, 24)));
  }
  if (csi_im_cfg_id_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_im_cfg_id_list_r14, bref, 1, 8, integer_packer<uint8_t>(1, 24)));
  }
  if (p_c_and_cbsr_per_res_cfg_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(p_c_and_cbsr_per_res_cfg_list_r14, bref, 1, 8, SeqOfPacker<Packer>(1, 2, Packer())));
  }
  if (ace_for4_tx_per_res_cfg_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ace_for4_tx_per_res_cfg_list_r14, bref, 1, 7, BitPacker(1)));
  }
  if (csi_rs_cfg_nzp_ap_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_rs_cfg_nzp_ap_list_r14, bref, 1, 8));
  }
  if (nzp_res_cfg_original_v1430_present) {
    HANDLE_CODE(nzp_res_cfg_original_v1430.unpack(bref));
  }
  if (csi_rs_nzp_activation_r14_present) {
    HANDLE_CODE(csi_rs_nzp_activation_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_bf_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_rs_cfg_nzp_id_list_ext_r14_present) {
    j.start_array("csi-RS-ConfigNZPIdListExt-r14");
    for (const auto& e1 : csi_rs_cfg_nzp_id_list_ext_r14) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (csi_im_cfg_id_list_r14_present) {
    j.start_array("csi-IM-ConfigIdList-r14");
    for (const auto& e1 : csi_im_cfg_id_list_r14) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (p_c_and_cbsr_per_res_cfg_list_r14_present) {
    j.start_array("p-C-AndCBSR-PerResourceConfigList-r14");
    for (const auto& e1 : p_c_and_cbsr_per_res_cfg_list_r14) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
      }
      j.end_array();
    }
    j.end_array();
  }
  if (ace_for4_tx_per_res_cfg_list_r14_present) {
    j.start_array("ace-For4Tx-PerResourceConfigList-r14");
    for (const auto& e1 : ace_for4_tx_per_res_cfg_list_r14) {
      j.write_bool(e1);
    }
    j.end_array();
  }
  if (alt_codebook_enabled_bf_r14_present) {
    j.write_str("alternativeCodebookEnabledBeamformed-r14", "true");
  }
  if (ch_meas_restrict_r14_present) {
    j.write_str("channelMeasRestriction-r14", "on");
  }
  if (csi_rs_cfg_nzp_ap_list_r14_present) {
    j.start_array("csi-RS-ConfigNZP-ApList-r14");
    for (const auto& e1 : csi_rs_cfg_nzp_ap_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (nzp_res_cfg_original_v1430_present) {
    j.write_fieldname("nzp-ResourceConfigOriginal-v1430");
    nzp_res_cfg_original_v1430.to_json(j);
  }
  if (csi_rs_nzp_activation_r14_present) {
    j.write_fieldname("csi-RS-NZP-Activation-r14");
    csi_rs_nzp_activation_r14.to_json(j);
  }
  j.end_obj();
}
bool csi_rs_cfg_bf_r14_s::operator==(const csi_rs_cfg_bf_r14_s& other) const
{
  return csi_rs_cfg_nzp_id_list_ext_r14_present == other.csi_rs_cfg_nzp_id_list_ext_r14_present and
         (not csi_rs_cfg_nzp_id_list_ext_r14_present or
          csi_rs_cfg_nzp_id_list_ext_r14 == other.csi_rs_cfg_nzp_id_list_ext_r14) and
         csi_im_cfg_id_list_r14_present == other.csi_im_cfg_id_list_r14_present and
         (not csi_im_cfg_id_list_r14_present or csi_im_cfg_id_list_r14 == other.csi_im_cfg_id_list_r14) and
         p_c_and_cbsr_per_res_cfg_list_r14_present == other.p_c_and_cbsr_per_res_cfg_list_r14_present and
         (not p_c_and_cbsr_per_res_cfg_list_r14_present or
          p_c_and_cbsr_per_res_cfg_list_r14 == other.p_c_and_cbsr_per_res_cfg_list_r14) and
         ace_for4_tx_per_res_cfg_list_r14_present == other.ace_for4_tx_per_res_cfg_list_r14_present and
         (not ace_for4_tx_per_res_cfg_list_r14_present or
          ace_for4_tx_per_res_cfg_list_r14 == other.ace_for4_tx_per_res_cfg_list_r14) and
         alt_codebook_enabled_bf_r14_present == other.alt_codebook_enabled_bf_r14_present and
         ch_meas_restrict_r14_present == other.ch_meas_restrict_r14_present and
         csi_rs_cfg_nzp_ap_list_r14_present == other.csi_rs_cfg_nzp_ap_list_r14_present and
         (not csi_rs_cfg_nzp_ap_list_r14_present or csi_rs_cfg_nzp_ap_list_r14 == other.csi_rs_cfg_nzp_ap_list_r14) and
         nzp_res_cfg_original_v1430_present == other.nzp_res_cfg_original_v1430_present and
         (not nzp_res_cfg_original_v1430_present or nzp_res_cfg_original_v1430 == other.nzp_res_cfg_original_v1430) and
         csi_rs_nzp_activation_r14_present == other.csi_rs_nzp_activation_r14_present and
         (not csi_rs_nzp_activation_r14_present or csi_rs_nzp_activation_r14 == other.csi_rs_nzp_activation_r14);
}

// CSI-RS-ConfigNZP-EMIMO-r13 ::= CHOICE
void csi_rs_cfg_nzp_emimo_r13_c::set(types::options e)
{
  type_ = e;
}
void csi_rs_cfg_nzp_emimo_r13_c::set_release()
{
  set(types::release);
}
csi_rs_cfg_nzp_emimo_r13_c::setup_s_& csi_rs_cfg_nzp_emimo_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void csi_rs_cfg_nzp_emimo_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("nzp-resourceConfigList-r13");
      for (const auto& e1 : c.nzp_res_cfg_list_r13) {
        e1.to_json(j);
      }
      j.end_array();
      if (c.cdm_type_r13_present) {
        j.write_str("cdmType-r13", c.cdm_type_r13.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_nzp_emimo_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_nzp_emimo_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.cdm_type_r13_present, 1));
      HANDLE_CODE(pack_dyn_seq_of(bref, c.nzp_res_cfg_list_r13, 1, 2));
      if (c.cdm_type_r13_present) {
        HANDLE_CODE(c.cdm_type_r13.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_nzp_emimo_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_nzp_emimo_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.cdm_type_r13_present, 1));
      HANDLE_CODE(unpack_dyn_seq_of(c.nzp_res_cfg_list_r13, bref, 1, 2));
      if (c.cdm_type_r13_present) {
        HANDLE_CODE(c.cdm_type_r13.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_nzp_emimo_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_nzp_emimo_r13_c::operator==(const csi_rs_cfg_nzp_emimo_r13_c& other) const
{
  return type() == other.type() and c.nzp_res_cfg_list_r13 == other.c.nzp_res_cfg_list_r13 and
         c.cdm_type_r13_present == other.c.cdm_type_r13_present and
         (not c.cdm_type_r13_present or c.cdm_type_r13 == other.c.cdm_type_r13);
}

const char* csi_rs_cfg_nzp_emimo_r13_c::setup_s_::cdm_type_r13_opts::to_string() const
{
  static const char* options[] = {"cdm2", "cdm4"};
  return convert_enum_idx(options, 2, value, "csi_rs_cfg_nzp_emimo_r13_c::setup_s_::cdm_type_r13_e_");
}
uint8_t csi_rs_cfg_nzp_emimo_r13_c::setup_s_::cdm_type_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "csi_rs_cfg_nzp_emimo_r13_c::setup_s_::cdm_type_r13_e_");
}

// CSI-RS-ConfigNZP-EMIMO-v1430 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_nzp_emimo_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cdm_type_v1430_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, nzp_res_cfg_list_ext_r14, 0, 4));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_nzp_emimo_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cdm_type_v1430_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(nzp_res_cfg_list_ext_r14, bref, 0, 4));

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_nzp_emimo_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("nzp-resourceConfigListExt-r14");
  for (const auto& e1 : nzp_res_cfg_list_ext_r14) {
    e1.to_json(j);
  }
  j.end_array();
  if (cdm_type_v1430_present) {
    j.write_str("cdmType-v1430", "cdm8");
  }
  j.end_obj();
}
bool csi_rs_cfg_nzp_emimo_v1430_s::operator==(const csi_rs_cfg_nzp_emimo_v1430_s& other) const
{
  return nzp_res_cfg_list_ext_r14 == other.nzp_res_cfg_list_ext_r14 and
         cdm_type_v1430_present == other.cdm_type_v1430_present;
}

// CQI-ReportModeAperiodic ::= ENUMERATED
const char* cqi_report_mode_aperiodic_opts::to_string() const
{
  static const char* options[] = {"rm12", "rm20", "rm22", "rm30", "rm31", "rm32-v1250", "rm10-v1310", "rm11-v1310"};
  return convert_enum_idx(options, 8, value, "cqi_report_mode_aperiodic_e");
}
uint8_t cqi_report_mode_aperiodic_opts::to_number() const
{
  static const uint8_t options[] = {12, 20, 22, 30, 31, 32, 10, 11};
  return map_enum_number(options, 8, value, "cqi_report_mode_aperiodic_e");
}

// CSI-RS-ConfigBeamformed-r13 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_bf_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_id_list_ext_r13_present, 1));
  HANDLE_CODE(bref.pack(csi_im_cfg_id_list_r13_present, 1));
  HANDLE_CODE(bref.pack(p_c_and_cbsr_per_res_cfg_list_r13_present, 1));
  HANDLE_CODE(bref.pack(ace_for4_tx_per_res_cfg_list_r13_present, 1));
  HANDLE_CODE(bref.pack(alt_codebook_enabled_bf_r13_present, 1));
  HANDLE_CODE(bref.pack(ch_meas_restrict_r13_present, 1));

  if (csi_rs_cfg_nzp_id_list_ext_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_rs_cfg_nzp_id_list_ext_r13, 1, 7, integer_packer<uint8_t>(1, 24)));
  }
  if (csi_im_cfg_id_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_im_cfg_id_list_r13, 1, 8, integer_packer<uint8_t>(1, 24)));
  }
  if (p_c_and_cbsr_per_res_cfg_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, p_c_and_cbsr_per_res_cfg_list_r13, 1, 8, SeqOfPacker<Packer>(1, 2, Packer())));
  }
  if (ace_for4_tx_per_res_cfg_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ace_for4_tx_per_res_cfg_list_r13, 1, 7, BitPacker(1)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_bf_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_id_list_ext_r13_present, 1));
  HANDLE_CODE(bref.unpack(csi_im_cfg_id_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(p_c_and_cbsr_per_res_cfg_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(ace_for4_tx_per_res_cfg_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(alt_codebook_enabled_bf_r13_present, 1));
  HANDLE_CODE(bref.unpack(ch_meas_restrict_r13_present, 1));

  if (csi_rs_cfg_nzp_id_list_ext_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_rs_cfg_nzp_id_list_ext_r13, bref, 1, 7, integer_packer<uint8_t>(1, 24)));
  }
  if (csi_im_cfg_id_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_im_cfg_id_list_r13, bref, 1, 8, integer_packer<uint8_t>(1, 24)));
  }
  if (p_c_and_cbsr_per_res_cfg_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(p_c_and_cbsr_per_res_cfg_list_r13, bref, 1, 8, SeqOfPacker<Packer>(1, 2, Packer())));
  }
  if (ace_for4_tx_per_res_cfg_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ace_for4_tx_per_res_cfg_list_r13, bref, 1, 7, BitPacker(1)));
  }

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_bf_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_rs_cfg_nzp_id_list_ext_r13_present) {
    j.start_array("csi-RS-ConfigNZPIdListExt-r13");
    for (const auto& e1 : csi_rs_cfg_nzp_id_list_ext_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (csi_im_cfg_id_list_r13_present) {
    j.start_array("csi-IM-ConfigIdList-r13");
    for (const auto& e1 : csi_im_cfg_id_list_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (p_c_and_cbsr_per_res_cfg_list_r13_present) {
    j.start_array("p-C-AndCBSR-PerResourceConfigList-r13");
    for (const auto& e1 : p_c_and_cbsr_per_res_cfg_list_r13) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
      }
      j.end_array();
    }
    j.end_array();
  }
  if (ace_for4_tx_per_res_cfg_list_r13_present) {
    j.start_array("ace-For4Tx-PerResourceConfigList-r13");
    for (const auto& e1 : ace_for4_tx_per_res_cfg_list_r13) {
      j.write_bool(e1);
    }
    j.end_array();
  }
  if (alt_codebook_enabled_bf_r13_present) {
    j.write_str("alternativeCodebookEnabledBeamformed-r13", "true");
  }
  if (ch_meas_restrict_r13_present) {
    j.write_str("channelMeasRestriction-r13", "on");
  }
  j.end_obj();
}
bool csi_rs_cfg_bf_r13_s::operator==(const csi_rs_cfg_bf_r13_s& other) const
{
  return csi_rs_cfg_nzp_id_list_ext_r13_present == other.csi_rs_cfg_nzp_id_list_ext_r13_present and
         (not csi_rs_cfg_nzp_id_list_ext_r13_present or
          csi_rs_cfg_nzp_id_list_ext_r13 == other.csi_rs_cfg_nzp_id_list_ext_r13) and
         csi_im_cfg_id_list_r13_present == other.csi_im_cfg_id_list_r13_present and
         (not csi_im_cfg_id_list_r13_present or csi_im_cfg_id_list_r13 == other.csi_im_cfg_id_list_r13) and
         p_c_and_cbsr_per_res_cfg_list_r13_present == other.p_c_and_cbsr_per_res_cfg_list_r13_present and
         (not p_c_and_cbsr_per_res_cfg_list_r13_present or
          p_c_and_cbsr_per_res_cfg_list_r13 == other.p_c_and_cbsr_per_res_cfg_list_r13) and
         ace_for4_tx_per_res_cfg_list_r13_present == other.ace_for4_tx_per_res_cfg_list_r13_present and
         (not ace_for4_tx_per_res_cfg_list_r13_present or
          ace_for4_tx_per_res_cfg_list_r13 == other.ace_for4_tx_per_res_cfg_list_r13) and
         alt_codebook_enabled_bf_r13_present == other.alt_codebook_enabled_bf_r13_present and
         ch_meas_restrict_r13_present == other.ch_meas_restrict_r13_present;
}

// CSI-RS-ConfigBeamformed-v1430 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_bf_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_ap_list_r14_present, 1));
  HANDLE_CODE(bref.pack(nzp_res_cfg_original_v1430_present, 1));
  HANDLE_CODE(bref.pack(csi_rs_nzp_activation_r14_present, 1));

  if (csi_rs_cfg_nzp_ap_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_rs_cfg_nzp_ap_list_r14, 1, 8));
  }
  if (nzp_res_cfg_original_v1430_present) {
    HANDLE_CODE(nzp_res_cfg_original_v1430.pack(bref));
  }
  if (csi_rs_nzp_activation_r14_present) {
    HANDLE_CODE(csi_rs_nzp_activation_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_bf_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_ap_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(nzp_res_cfg_original_v1430_present, 1));
  HANDLE_CODE(bref.unpack(csi_rs_nzp_activation_r14_present, 1));

  if (csi_rs_cfg_nzp_ap_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_rs_cfg_nzp_ap_list_r14, bref, 1, 8));
  }
  if (nzp_res_cfg_original_v1430_present) {
    HANDLE_CODE(nzp_res_cfg_original_v1430.unpack(bref));
  }
  if (csi_rs_nzp_activation_r14_present) {
    HANDLE_CODE(csi_rs_nzp_activation_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_bf_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_rs_cfg_nzp_ap_list_r14_present) {
    j.start_array("csi-RS-ConfigNZP-ApList-r14");
    for (const auto& e1 : csi_rs_cfg_nzp_ap_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (nzp_res_cfg_original_v1430_present) {
    j.write_fieldname("nzp-ResourceConfigOriginal-v1430");
    nzp_res_cfg_original_v1430.to_json(j);
  }
  if (csi_rs_nzp_activation_r14_present) {
    j.write_fieldname("csi-RS-NZP-Activation-r14");
    csi_rs_nzp_activation_r14.to_json(j);
  }
  j.end_obj();
}
bool csi_rs_cfg_bf_v1430_s::operator==(const csi_rs_cfg_bf_v1430_s& other) const
{
  return csi_rs_cfg_nzp_ap_list_r14_present == other.csi_rs_cfg_nzp_ap_list_r14_present and
         (not csi_rs_cfg_nzp_ap_list_r14_present or csi_rs_cfg_nzp_ap_list_r14 == other.csi_rs_cfg_nzp_ap_list_r14) and
         nzp_res_cfg_original_v1430_present == other.nzp_res_cfg_original_v1430_present and
         (not nzp_res_cfg_original_v1430_present or nzp_res_cfg_original_v1430 == other.nzp_res_cfg_original_v1430) and
         csi_rs_nzp_activation_r14_present == other.csi_rs_nzp_activation_r14_present and
         (not csi_rs_nzp_activation_r14_present or csi_rs_nzp_activation_r14 == other.csi_rs_nzp_activation_r14);
}

// CSI-RS-ConfigEMIMO2-r14 ::= CHOICE
void csi_rs_cfg_emimo2_r14_c::set(types::options e)
{
  type_ = e;
}
void csi_rs_cfg_emimo2_r14_c::set_release()
{
  set(types::release);
}
csi_rs_cfg_bf_r14_s& csi_rs_cfg_emimo2_r14_c::set_setup()
{
  set(types::setup);
  return c;
}
void csi_rs_cfg_emimo2_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo2_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_emimo2_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo2_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_emimo2_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo2_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_emimo2_r14_c::operator==(const csi_rs_cfg_emimo2_r14_c& other) const
{
  return type() == other.type() and c == other.c;
}

// CSI-RS-ConfigNonPrecoded-r13 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_non_precoded_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(p_c_and_cbsr_list_r13_present, 1));
  HANDLE_CODE(bref.pack(codebook_over_sampling_rate_cfg_o1_r13_present, 1));
  HANDLE_CODE(bref.pack(codebook_over_sampling_rate_cfg_o2_r13_present, 1));
  HANDLE_CODE(bref.pack(csi_im_cfg_id_list_r13_present, 1));
  HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_emimo_r13_present, 1));

  if (p_c_and_cbsr_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, p_c_and_cbsr_list_r13, 1, 2));
  }
  HANDLE_CODE(codebook_cfg_n1_r13.pack(bref));
  HANDLE_CODE(codebook_cfg_n2_r13.pack(bref));
  if (codebook_over_sampling_rate_cfg_o1_r13_present) {
    HANDLE_CODE(codebook_over_sampling_rate_cfg_o1_r13.pack(bref));
  }
  if (codebook_over_sampling_rate_cfg_o2_r13_present) {
    HANDLE_CODE(codebook_over_sampling_rate_cfg_o2_r13.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, codebook_cfg_r13, (uint8_t)1u, (uint8_t)4u));
  if (csi_im_cfg_id_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_im_cfg_id_list_r13, 1, 2, integer_packer<uint8_t>(1, 24)));
  }
  if (csi_rs_cfg_nzp_emimo_r13_present) {
    HANDLE_CODE(csi_rs_cfg_nzp_emimo_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_non_precoded_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(p_c_and_cbsr_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(codebook_over_sampling_rate_cfg_o1_r13_present, 1));
  HANDLE_CODE(bref.unpack(codebook_over_sampling_rate_cfg_o2_r13_present, 1));
  HANDLE_CODE(bref.unpack(csi_im_cfg_id_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_emimo_r13_present, 1));

  if (p_c_and_cbsr_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(p_c_and_cbsr_list_r13, bref, 1, 2));
  }
  HANDLE_CODE(codebook_cfg_n1_r13.unpack(bref));
  HANDLE_CODE(codebook_cfg_n2_r13.unpack(bref));
  if (codebook_over_sampling_rate_cfg_o1_r13_present) {
    HANDLE_CODE(codebook_over_sampling_rate_cfg_o1_r13.unpack(bref));
  }
  if (codebook_over_sampling_rate_cfg_o2_r13_present) {
    HANDLE_CODE(codebook_over_sampling_rate_cfg_o2_r13.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(codebook_cfg_r13, bref, (uint8_t)1u, (uint8_t)4u));
  if (csi_im_cfg_id_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_im_cfg_id_list_r13, bref, 1, 2, integer_packer<uint8_t>(1, 24)));
  }
  if (csi_rs_cfg_nzp_emimo_r13_present) {
    HANDLE_CODE(csi_rs_cfg_nzp_emimo_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_non_precoded_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (p_c_and_cbsr_list_r13_present) {
    j.start_array("p-C-AndCBSRList-r13");
    for (const auto& e1 : p_c_and_cbsr_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("codebookConfigN1-r13", codebook_cfg_n1_r13.to_string());
  j.write_str("codebookConfigN2-r13", codebook_cfg_n2_r13.to_string());
  if (codebook_over_sampling_rate_cfg_o1_r13_present) {
    j.write_str("codebookOverSamplingRateConfig-O1-r13", codebook_over_sampling_rate_cfg_o1_r13.to_string());
  }
  if (codebook_over_sampling_rate_cfg_o2_r13_present) {
    j.write_str("codebookOverSamplingRateConfig-O2-r13", codebook_over_sampling_rate_cfg_o2_r13.to_string());
  }
  j.write_int("codebookConfig-r13", codebook_cfg_r13);
  if (csi_im_cfg_id_list_r13_present) {
    j.start_array("csi-IM-ConfigIdList-r13");
    for (const auto& e1 : csi_im_cfg_id_list_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (csi_rs_cfg_nzp_emimo_r13_present) {
    j.write_fieldname("csi-RS-ConfigNZP-EMIMO-r13");
    csi_rs_cfg_nzp_emimo_r13.to_json(j);
  }
  j.end_obj();
}
bool csi_rs_cfg_non_precoded_r13_s::operator==(const csi_rs_cfg_non_precoded_r13_s& other) const
{
  return p_c_and_cbsr_list_r13_present == other.p_c_and_cbsr_list_r13_present and
         (not p_c_and_cbsr_list_r13_present or p_c_and_cbsr_list_r13 == other.p_c_and_cbsr_list_r13) and
         codebook_cfg_n1_r13 == other.codebook_cfg_n1_r13 and codebook_cfg_n2_r13 == other.codebook_cfg_n2_r13 and
         codebook_over_sampling_rate_cfg_o1_r13_present == other.codebook_over_sampling_rate_cfg_o1_r13_present and
         (not codebook_over_sampling_rate_cfg_o1_r13_present or
          codebook_over_sampling_rate_cfg_o1_r13 == other.codebook_over_sampling_rate_cfg_o1_r13) and
         codebook_over_sampling_rate_cfg_o2_r13_present == other.codebook_over_sampling_rate_cfg_o2_r13_present and
         (not codebook_over_sampling_rate_cfg_o2_r13_present or
          codebook_over_sampling_rate_cfg_o2_r13 == other.codebook_over_sampling_rate_cfg_o2_r13) and
         codebook_cfg_r13 == other.codebook_cfg_r13 and
         csi_im_cfg_id_list_r13_present == other.csi_im_cfg_id_list_r13_present and
         (not csi_im_cfg_id_list_r13_present or csi_im_cfg_id_list_r13 == other.csi_im_cfg_id_list_r13) and
         csi_rs_cfg_nzp_emimo_r13_present == other.csi_rs_cfg_nzp_emimo_r13_present and
         (not csi_rs_cfg_nzp_emimo_r13_present or csi_rs_cfg_nzp_emimo_r13 == other.csi_rs_cfg_nzp_emimo_r13);
}

const char* csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n1_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n8"};
  return convert_enum_idx(options, 5, value, "csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n1_r13_e_");
}
uint8_t csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n1_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 8};
  return map_enum_number(options, 5, value, "csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n1_r13_e_");
}

const char* csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n2_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n8"};
  return convert_enum_idx(options, 5, value, "csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n2_r13_e_");
}
uint8_t csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n2_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 8};
  return map_enum_number(options, 5, value, "csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n2_r13_e_");
}

const char* csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o1_r13_opts::to_string() const
{
  static const char* options[] = {"n4", "n8"};
  return convert_enum_idx(
      options, 2, value, "csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o1_r13_e_");
}
uint8_t csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o1_r13_opts::to_number() const
{
  static const uint8_t options[] = {4, 8};
  return map_enum_number(options, 2, value, "csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o1_r13_e_");
}

const char* csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o2_r13_opts::to_string() const
{
  static const char* options[] = {"n4", "n8"};
  return convert_enum_idx(
      options, 2, value, "csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o2_r13_e_");
}
uint8_t csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o2_r13_opts::to_number() const
{
  static const uint8_t options[] = {4, 8};
  return map_enum_number(options, 2, value, "csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o2_r13_e_");
}

// CSI-RS-ConfigNonPrecoded-v1430 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_non_precoded_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_emimo_v1430_present, 1));

  if (csi_rs_cfg_nzp_emimo_v1430_present) {
    HANDLE_CODE(csi_rs_cfg_nzp_emimo_v1430.pack(bref));
  }
  HANDLE_CODE(codebook_cfg_n1_v1430.pack(bref));
  HANDLE_CODE(codebook_cfg_n2_v1430.pack(bref));
  HANDLE_CODE(nzp_res_cfg_tm9_original_v1430.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_non_precoded_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_emimo_v1430_present, 1));

  if (csi_rs_cfg_nzp_emimo_v1430_present) {
    HANDLE_CODE(csi_rs_cfg_nzp_emimo_v1430.unpack(bref));
  }
  HANDLE_CODE(codebook_cfg_n1_v1430.unpack(bref));
  HANDLE_CODE(codebook_cfg_n2_v1430.unpack(bref));
  HANDLE_CODE(nzp_res_cfg_tm9_original_v1430.unpack(bref));

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_non_precoded_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_rs_cfg_nzp_emimo_v1430_present) {
    j.write_fieldname("csi-RS-ConfigNZP-EMIMO-v1430");
    csi_rs_cfg_nzp_emimo_v1430.to_json(j);
  }
  j.write_str("codebookConfigN1-v1430", codebook_cfg_n1_v1430.to_string());
  j.write_str("codebookConfigN2-v1430", codebook_cfg_n2_v1430.to_string());
  j.write_fieldname("nzp-ResourceConfigTM9-Original-v1430");
  nzp_res_cfg_tm9_original_v1430.to_json(j);
  j.end_obj();
}
bool csi_rs_cfg_non_precoded_v1430_s::operator==(const csi_rs_cfg_non_precoded_v1430_s& other) const
{
  return csi_rs_cfg_nzp_emimo_v1430_present == other.csi_rs_cfg_nzp_emimo_v1430_present and
         (not csi_rs_cfg_nzp_emimo_v1430_present or csi_rs_cfg_nzp_emimo_v1430 == other.csi_rs_cfg_nzp_emimo_v1430) and
         codebook_cfg_n1_v1430 == other.codebook_cfg_n1_v1430 and
         codebook_cfg_n2_v1430 == other.codebook_cfg_n2_v1430 and
         nzp_res_cfg_tm9_original_v1430 == other.nzp_res_cfg_tm9_original_v1430;
}

const char* csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n1_v1430_opts::to_string() const
{
  static const char* options[] = {"n5", "n6", "n7", "n10", "n12", "n14", "n16"};
  return convert_enum_idx(options, 7, value, "csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n1_v1430_e_");
}
uint8_t csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n1_v1430_opts::to_number() const
{
  static const uint8_t options[] = {5, 6, 7, 10, 12, 14, 16};
  return map_enum_number(options, 7, value, "csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n1_v1430_e_");
}

const char* csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n2_v1430_opts::to_string() const
{
  static const char* options[] = {"n5", "n6", "n7"};
  return convert_enum_idx(options, 3, value, "csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n2_v1430_e_");
}
uint8_t csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n2_v1430_opts::to_number() const
{
  static const uint8_t options[] = {5, 6, 7};
  return map_enum_number(options, 3, value, "csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n2_v1430_e_");
}

// CSI-RS-ConfigNonPrecoded-v1480 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_non_precoded_v1480_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_emimo_v1480_present, 1));
  HANDLE_CODE(bref.pack(codebook_cfg_n1_v1480_present, 1));
  HANDLE_CODE(bref.pack(codebook_cfg_n2_r1480_present, 1));

  if (csi_rs_cfg_nzp_emimo_v1480_present) {
    HANDLE_CODE(csi_rs_cfg_nzp_emimo_v1480.pack(bref));
  }
  if (codebook_cfg_n1_v1480_present) {
    HANDLE_CODE(codebook_cfg_n1_v1480.pack(bref));
  }
  if (codebook_cfg_n2_r1480_present) {
    HANDLE_CODE(codebook_cfg_n2_r1480.pack(bref));
  }
  HANDLE_CODE(nzp_res_cfg_tm9_original_v1480.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_non_precoded_v1480_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_emimo_v1480_present, 1));
  HANDLE_CODE(bref.unpack(codebook_cfg_n1_v1480_present, 1));
  HANDLE_CODE(bref.unpack(codebook_cfg_n2_r1480_present, 1));

  if (csi_rs_cfg_nzp_emimo_v1480_present) {
    HANDLE_CODE(csi_rs_cfg_nzp_emimo_v1480.unpack(bref));
  }
  if (codebook_cfg_n1_v1480_present) {
    HANDLE_CODE(codebook_cfg_n1_v1480.unpack(bref));
  }
  if (codebook_cfg_n2_r1480_present) {
    HANDLE_CODE(codebook_cfg_n2_r1480.unpack(bref));
  }
  HANDLE_CODE(nzp_res_cfg_tm9_original_v1480.unpack(bref));

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_non_precoded_v1480_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_rs_cfg_nzp_emimo_v1480_present) {
    j.write_fieldname("csi-RS-ConfigNZP-EMIMO-v1480");
    csi_rs_cfg_nzp_emimo_v1480.to_json(j);
  }
  if (codebook_cfg_n1_v1480_present) {
    j.write_str("codebookConfigN1-v1480", codebook_cfg_n1_v1480.to_string());
  }
  if (codebook_cfg_n2_r1480_present) {
    j.write_str("codebookConfigN2-r1480", codebook_cfg_n2_r1480.to_string());
  }
  j.write_fieldname("nzp-ResourceConfigTM9-Original-v1480");
  nzp_res_cfg_tm9_original_v1480.to_json(j);
  j.end_obj();
}
bool csi_rs_cfg_non_precoded_v1480_s::operator==(const csi_rs_cfg_non_precoded_v1480_s& other) const
{
  return csi_rs_cfg_nzp_emimo_v1480_present == other.csi_rs_cfg_nzp_emimo_v1480_present and
         (not csi_rs_cfg_nzp_emimo_v1480_present or csi_rs_cfg_nzp_emimo_v1480 == other.csi_rs_cfg_nzp_emimo_v1480) and
         codebook_cfg_n1_v1480_present == other.codebook_cfg_n1_v1480_present and
         (not codebook_cfg_n1_v1480_present or codebook_cfg_n1_v1480 == other.codebook_cfg_n1_v1480) and
         codebook_cfg_n2_r1480_present == other.codebook_cfg_n2_r1480_present and
         (not codebook_cfg_n2_r1480_present or codebook_cfg_n2_r1480 == other.codebook_cfg_n2_r1480) and
         nzp_res_cfg_tm9_original_v1480 == other.nzp_res_cfg_tm9_original_v1480;
}

const char* csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n1_v1480_opts::to_string() const
{
  static const char* options[] = {"n5", "n6", "n7", "n10", "n12", "n14", "n16"};
  return convert_enum_idx(options, 7, value, "csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n1_v1480_e_");
}
uint8_t csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n1_v1480_opts::to_number() const
{
  static const uint8_t options[] = {5, 6, 7, 10, 12, 14, 16};
  return map_enum_number(options, 7, value, "csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n1_v1480_e_");
}

const char* csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n2_r1480_opts::to_string() const
{
  static const char* options[] = {"n5", "n6", "n7"};
  return convert_enum_idx(options, 3, value, "csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n2_r1480_e_");
}
uint8_t csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n2_r1480_opts::to_number() const
{
  static const uint8_t options[] = {5, 6, 7};
  return map_enum_number(options, 3, value, "csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n2_r1480_e_");
}

// CSI-RS-ConfigNonPrecoded-v1530 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_non_precoded_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(p_c_and_cbsr_list_r15_present, 1));

  if (p_c_and_cbsr_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, p_c_and_cbsr_list_r15, 1, 2));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_non_precoded_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(p_c_and_cbsr_list_r15_present, 1));

  if (p_c_and_cbsr_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(p_c_and_cbsr_list_r15, bref, 1, 2));
  }

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_non_precoded_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (p_c_and_cbsr_list_r15_present) {
    j.start_array("p-C-AndCBSRList-r15");
    for (const auto& e1 : p_c_and_cbsr_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
bool csi_rs_cfg_non_precoded_v1530_s::operator==(const csi_rs_cfg_non_precoded_v1530_s& other) const
{
  return p_c_and_cbsr_list_r15_present == other.p_c_and_cbsr_list_r15_present and
         (not p_c_and_cbsr_list_r15_present or p_c_and_cbsr_list_r15 == other.p_c_and_cbsr_list_r15);
}

// P-C-AndCBSR-r11 ::= SEQUENCE
SRSASN_CODE p_c_and_cbsr_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, p_c_r11, (int8_t)-8, (int8_t)15));
  HANDLE_CODE(codebook_subset_restrict_r11.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE p_c_and_cbsr_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(p_c_r11, bref, (int8_t)-8, (int8_t)15));
  HANDLE_CODE(codebook_subset_restrict_r11.unpack(bref));

  return SRSASN_SUCCESS;
}
void p_c_and_cbsr_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p-C-r11", p_c_r11);
  j.write_str("codebookSubsetRestriction-r11", codebook_subset_restrict_r11.to_string());
  j.end_obj();
}
bool p_c_and_cbsr_r11_s::operator==(const p_c_and_cbsr_r11_s& other) const
{
  return p_c_r11 == other.p_c_r11 and codebook_subset_restrict_r11 == other.codebook_subset_restrict_r11;
}

// CQI-ReportAperiodicProc-r11 ::= SEQUENCE
SRSASN_CODE cqi_report_aperiodic_proc_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cqi_report_mode_aperiodic_r11.pack(bref));
  HANDLE_CODE(bref.pack(trigger01_r11, 1));
  HANDLE_CODE(bref.pack(trigger10_r11, 1));
  HANDLE_CODE(bref.pack(trigger11_r11, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_aperiodic_proc_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cqi_report_mode_aperiodic_r11.unpack(bref));
  HANDLE_CODE(bref.unpack(trigger01_r11, 1));
  HANDLE_CODE(bref.unpack(trigger10_r11, 1));
  HANDLE_CODE(bref.unpack(trigger11_r11, 1));

  return SRSASN_SUCCESS;
}
void cqi_report_aperiodic_proc_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cqi-ReportModeAperiodic-r11", cqi_report_mode_aperiodic_r11.to_string());
  j.write_bool("trigger01-r11", trigger01_r11);
  j.write_bool("trigger10-r11", trigger10_r11);
  j.write_bool("trigger11-r11", trigger11_r11);
  j.end_obj();
}
bool cqi_report_aperiodic_proc_r11_s::operator==(const cqi_report_aperiodic_proc_r11_s& other) const
{
  return cqi_report_mode_aperiodic_r11 == other.cqi_report_mode_aperiodic_r11 and
         trigger01_r11 == other.trigger01_r11 and trigger10_r11 == other.trigger10_r11 and
         trigger11_r11 == other.trigger11_r11;
}

// CQI-ReportAperiodicProc-v1310 ::= SEQUENCE
SRSASN_CODE cqi_report_aperiodic_proc_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(trigger001_r13, 1));
  HANDLE_CODE(bref.pack(trigger010_r13, 1));
  HANDLE_CODE(bref.pack(trigger011_r13, 1));
  HANDLE_CODE(bref.pack(trigger100_r13, 1));
  HANDLE_CODE(bref.pack(trigger101_r13, 1));
  HANDLE_CODE(bref.pack(trigger110_r13, 1));
  HANDLE_CODE(bref.pack(trigger111_r13, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_aperiodic_proc_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(trigger001_r13, 1));
  HANDLE_CODE(bref.unpack(trigger010_r13, 1));
  HANDLE_CODE(bref.unpack(trigger011_r13, 1));
  HANDLE_CODE(bref.unpack(trigger100_r13, 1));
  HANDLE_CODE(bref.unpack(trigger101_r13, 1));
  HANDLE_CODE(bref.unpack(trigger110_r13, 1));
  HANDLE_CODE(bref.unpack(trigger111_r13, 1));

  return SRSASN_SUCCESS;
}
void cqi_report_aperiodic_proc_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("trigger001-r13", trigger001_r13);
  j.write_bool("trigger010-r13", trigger010_r13);
  j.write_bool("trigger011-r13", trigger011_r13);
  j.write_bool("trigger100-r13", trigger100_r13);
  j.write_bool("trigger101-r13", trigger101_r13);
  j.write_bool("trigger110-r13", trigger110_r13);
  j.write_bool("trigger111-r13", trigger111_r13);
  j.end_obj();
}
bool cqi_report_aperiodic_proc_v1310_s::operator==(const cqi_report_aperiodic_proc_v1310_s& other) const
{
  return trigger001_r13 == other.trigger001_r13 and trigger010_r13 == other.trigger010_r13 and
         trigger011_r13 == other.trigger011_r13 and trigger100_r13 == other.trigger100_r13 and
         trigger101_r13 == other.trigger101_r13 and trigger110_r13 == other.trigger110_r13 and
         trigger111_r13 == other.trigger111_r13;
}

// CQI-ReportBothProc-r11 ::= SEQUENCE
SRSASN_CODE cqi_report_both_proc_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ri_ref_csi_process_id_r11_present, 1));
  HANDLE_CODE(bref.pack(pmi_ri_report_r11_present, 1));

  if (ri_ref_csi_process_id_r11_present) {
    HANDLE_CODE(pack_integer(bref, ri_ref_csi_process_id_r11, (uint8_t)1u, (uint8_t)4u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_both_proc_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ri_ref_csi_process_id_r11_present, 1));
  HANDLE_CODE(bref.unpack(pmi_ri_report_r11_present, 1));

  if (ri_ref_csi_process_id_r11_present) {
    HANDLE_CODE(unpack_integer(ri_ref_csi_process_id_r11, bref, (uint8_t)1u, (uint8_t)4u));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_both_proc_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ri_ref_csi_process_id_r11_present) {
    j.write_int("ri-Ref-CSI-ProcessId-r11", ri_ref_csi_process_id_r11);
  }
  if (pmi_ri_report_r11_present) {
    j.write_str("pmi-RI-Report-r11", "setup");
  }
  j.end_obj();
}
bool cqi_report_both_proc_r11_s::operator==(const cqi_report_both_proc_r11_s& other) const
{
  return ri_ref_csi_process_id_r11_present == other.ri_ref_csi_process_id_r11_present and
         (not ri_ref_csi_process_id_r11_present or ri_ref_csi_process_id_r11 == other.ri_ref_csi_process_id_r11) and
         pmi_ri_report_r11_present == other.pmi_ri_report_r11_present;
}

// CRI-ReportConfig-r13 ::= CHOICE
void cri_report_cfg_r13_c::set(types::options e)
{
  type_ = e;
}
void cri_report_cfg_r13_c::set_release()
{
  set(types::release);
}
cri_report_cfg_r13_c::setup_s_& cri_report_cfg_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void cri_report_cfg_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("cri-ConfigIndex-r13", c.cri_cfg_idx_r13);
      if (c.cri_cfg_idx2_r13_present) {
        j.write_int("cri-ConfigIndex2-r13", c.cri_cfg_idx2_r13);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cri_report_cfg_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE cri_report_cfg_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.cri_cfg_idx2_r13_present, 1));
      HANDLE_CODE(pack_integer(bref, c.cri_cfg_idx_r13, (uint16_t)0u, (uint16_t)1023u));
      if (c.cri_cfg_idx2_r13_present) {
        HANDLE_CODE(pack_integer(bref, c.cri_cfg_idx2_r13, (uint16_t)0u, (uint16_t)1023u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cri_report_cfg_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cri_report_cfg_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.cri_cfg_idx2_r13_present, 1));
      HANDLE_CODE(unpack_integer(c.cri_cfg_idx_r13, bref, (uint16_t)0u, (uint16_t)1023u));
      if (c.cri_cfg_idx2_r13_present) {
        HANDLE_CODE(unpack_integer(c.cri_cfg_idx2_r13, bref, (uint16_t)0u, (uint16_t)1023u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cri_report_cfg_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cri_report_cfg_r13_c::operator==(const cri_report_cfg_r13_c& other) const
{
  return type() == other.type() and c.cri_cfg_idx_r13 == other.c.cri_cfg_idx_r13 and
         c.cri_cfg_idx2_r13_present == other.c.cri_cfg_idx2_r13_present and
         (not c.cri_cfg_idx2_r13_present or c.cri_cfg_idx2_r13 == other.c.cri_cfg_idx2_r13);
}

// CSI-RS-ConfigEMIMO-Hybrid-r14 ::= CHOICE
void csi_rs_cfg_emimo_hybrid_r14_c::set(types::options e)
{
  type_ = e;
}
void csi_rs_cfg_emimo_hybrid_r14_c::set_release()
{
  set(types::release);
}
csi_rs_cfg_emimo_hybrid_r14_c::setup_s_& csi_rs_cfg_emimo_hybrid_r14_c::set_setup()
{
  set(types::setup);
  return c;
}
void csi_rs_cfg_emimo_hybrid_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.periodicity_offset_idx_r14_present) {
        j.write_int("periodicityOffsetIndex-r14", c.periodicity_offset_idx_r14);
      }
      if (c.emimo_type2_r14_present) {
        j.write_fieldname("eMIMO-Type2-r14");
        c.emimo_type2_r14.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_hybrid_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_emimo_hybrid_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.periodicity_offset_idx_r14_present, 1));
      HANDLE_CODE(bref.pack(c.emimo_type2_r14_present, 1));
      if (c.periodicity_offset_idx_r14_present) {
        HANDLE_CODE(pack_integer(bref, c.periodicity_offset_idx_r14, (uint16_t)0u, (uint16_t)1023u));
      }
      if (c.emimo_type2_r14_present) {
        HANDLE_CODE(c.emimo_type2_r14.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_hybrid_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_emimo_hybrid_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.periodicity_offset_idx_r14_present, 1));
      HANDLE_CODE(bref.unpack(c.emimo_type2_r14_present, 1));
      if (c.periodicity_offset_idx_r14_present) {
        HANDLE_CODE(unpack_integer(c.periodicity_offset_idx_r14, bref, (uint16_t)0u, (uint16_t)1023u));
      }
      if (c.emimo_type2_r14_present) {
        HANDLE_CODE(c.emimo_type2_r14.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_hybrid_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_emimo_hybrid_r14_c::operator==(const csi_rs_cfg_emimo_hybrid_r14_c& other) const
{
  return type() == other.type() and
         c.periodicity_offset_idx_r14_present == other.c.periodicity_offset_idx_r14_present and
         (not c.periodicity_offset_idx_r14_present or
          c.periodicity_offset_idx_r14 == other.c.periodicity_offset_idx_r14) and
         c.emimo_type2_r14_present == other.c.emimo_type2_r14_present and
         (not c.emimo_type2_r14_present or c.emimo_type2_r14 == other.c.emimo_type2_r14);
}

// CSI-RS-ConfigEMIMO-r13 ::= CHOICE
void csi_rs_cfg_emimo_r13_c::set(types::options e)
{
  type_ = e;
}
void csi_rs_cfg_emimo_r13_c::set_release()
{
  set(types::release);
}
csi_rs_cfg_emimo_r13_c::setup_c_& csi_rs_cfg_emimo_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void csi_rs_cfg_emimo_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_emimo_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_emimo_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_emimo_r13_c::operator==(const csi_rs_cfg_emimo_r13_c& other) const
{
  return type() == other.type() and c == other.c;
}

void csi_rs_cfg_emimo_r13_c::setup_c_::destroy_()
{
  switch (type_) {
    case types::non_precoded_r13:
      c.destroy<csi_rs_cfg_non_precoded_r13_s>();
      break;
    case types::bf_r13:
      c.destroy<csi_rs_cfg_bf_r13_s>();
      break;
    default:
      break;
  }
}
void csi_rs_cfg_emimo_r13_c::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::non_precoded_r13:
      c.init<csi_rs_cfg_non_precoded_r13_s>();
      break;
    case types::bf_r13:
      c.init<csi_rs_cfg_bf_r13_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_r13_c::setup_c_");
  }
}
csi_rs_cfg_emimo_r13_c::setup_c_::setup_c_(const csi_rs_cfg_emimo_r13_c::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::non_precoded_r13:
      c.init(other.c.get<csi_rs_cfg_non_precoded_r13_s>());
      break;
    case types::bf_r13:
      c.init(other.c.get<csi_rs_cfg_bf_r13_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_r13_c::setup_c_");
  }
}
csi_rs_cfg_emimo_r13_c::setup_c_&
csi_rs_cfg_emimo_r13_c::setup_c_::operator=(const csi_rs_cfg_emimo_r13_c::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::non_precoded_r13:
      c.set(other.c.get<csi_rs_cfg_non_precoded_r13_s>());
      break;
    case types::bf_r13:
      c.set(other.c.get<csi_rs_cfg_bf_r13_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_r13_c::setup_c_");
  }

  return *this;
}
csi_rs_cfg_non_precoded_r13_s& csi_rs_cfg_emimo_r13_c::setup_c_::set_non_precoded_r13()
{
  set(types::non_precoded_r13);
  return c.get<csi_rs_cfg_non_precoded_r13_s>();
}
csi_rs_cfg_bf_r13_s& csi_rs_cfg_emimo_r13_c::setup_c_::set_bf_r13()
{
  set(types::bf_r13);
  return c.get<csi_rs_cfg_bf_r13_s>();
}
void csi_rs_cfg_emimo_r13_c::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::non_precoded_r13:
      j.write_fieldname("nonPrecoded-r13");
      c.get<csi_rs_cfg_non_precoded_r13_s>().to_json(j);
      break;
    case types::bf_r13:
      j.write_fieldname("beamformed-r13");
      c.get<csi_rs_cfg_bf_r13_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_r13_c::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_emimo_r13_c::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::non_precoded_r13:
      HANDLE_CODE(c.get<csi_rs_cfg_non_precoded_r13_s>().pack(bref));
      break;
    case types::bf_r13:
      HANDLE_CODE(c.get<csi_rs_cfg_bf_r13_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_r13_c::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_emimo_r13_c::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::non_precoded_r13:
      HANDLE_CODE(c.get<csi_rs_cfg_non_precoded_r13_s>().unpack(bref));
      break;
    case types::bf_r13:
      HANDLE_CODE(c.get<csi_rs_cfg_bf_r13_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_r13_c::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_emimo_r13_c::setup_c_::operator==(const setup_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::non_precoded_r13:
      return c.get<csi_rs_cfg_non_precoded_r13_s>() == other.c.get<csi_rs_cfg_non_precoded_r13_s>();
    case types::bf_r13:
      return c.get<csi_rs_cfg_bf_r13_s>() == other.c.get<csi_rs_cfg_bf_r13_s>();
    default:
      return true;
  }
  return true;
}

// CSI-RS-ConfigEMIMO-v1430 ::= CHOICE
void csi_rs_cfg_emimo_v1430_c::set(types::options e)
{
  type_ = e;
}
void csi_rs_cfg_emimo_v1430_c::set_release()
{
  set(types::release);
}
csi_rs_cfg_emimo_v1430_c::setup_c_& csi_rs_cfg_emimo_v1430_c::set_setup()
{
  set(types::setup);
  return c;
}
void csi_rs_cfg_emimo_v1430_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1430_c");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_emimo_v1430_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1430_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_emimo_v1430_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1430_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_emimo_v1430_c::operator==(const csi_rs_cfg_emimo_v1430_c& other) const
{
  return type() == other.type() and c == other.c;
}

void csi_rs_cfg_emimo_v1430_c::setup_c_::destroy_()
{
  switch (type_) {
    case types::non_precoded_v1430:
      c.destroy<csi_rs_cfg_non_precoded_v1430_s>();
      break;
    case types::bf_v1430:
      c.destroy<csi_rs_cfg_bf_v1430_s>();
      break;
    default:
      break;
  }
}
void csi_rs_cfg_emimo_v1430_c::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::non_precoded_v1430:
      c.init<csi_rs_cfg_non_precoded_v1430_s>();
      break;
    case types::bf_v1430:
      c.init<csi_rs_cfg_bf_v1430_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1430_c::setup_c_");
  }
}
csi_rs_cfg_emimo_v1430_c::setup_c_::setup_c_(const csi_rs_cfg_emimo_v1430_c::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::non_precoded_v1430:
      c.init(other.c.get<csi_rs_cfg_non_precoded_v1430_s>());
      break;
    case types::bf_v1430:
      c.init(other.c.get<csi_rs_cfg_bf_v1430_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1430_c::setup_c_");
  }
}
csi_rs_cfg_emimo_v1430_c::setup_c_&
csi_rs_cfg_emimo_v1430_c::setup_c_::operator=(const csi_rs_cfg_emimo_v1430_c::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::non_precoded_v1430:
      c.set(other.c.get<csi_rs_cfg_non_precoded_v1430_s>());
      break;
    case types::bf_v1430:
      c.set(other.c.get<csi_rs_cfg_bf_v1430_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1430_c::setup_c_");
  }

  return *this;
}
csi_rs_cfg_non_precoded_v1430_s& csi_rs_cfg_emimo_v1430_c::setup_c_::set_non_precoded_v1430()
{
  set(types::non_precoded_v1430);
  return c.get<csi_rs_cfg_non_precoded_v1430_s>();
}
csi_rs_cfg_bf_v1430_s& csi_rs_cfg_emimo_v1430_c::setup_c_::set_bf_v1430()
{
  set(types::bf_v1430);
  return c.get<csi_rs_cfg_bf_v1430_s>();
}
void csi_rs_cfg_emimo_v1430_c::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::non_precoded_v1430:
      j.write_fieldname("nonPrecoded-v1430");
      c.get<csi_rs_cfg_non_precoded_v1430_s>().to_json(j);
      break;
    case types::bf_v1430:
      j.write_fieldname("beamformed-v1430");
      c.get<csi_rs_cfg_bf_v1430_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1430_c::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_emimo_v1430_c::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::non_precoded_v1430:
      HANDLE_CODE(c.get<csi_rs_cfg_non_precoded_v1430_s>().pack(bref));
      break;
    case types::bf_v1430:
      HANDLE_CODE(c.get<csi_rs_cfg_bf_v1430_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1430_c::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_emimo_v1430_c::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::non_precoded_v1430:
      HANDLE_CODE(c.get<csi_rs_cfg_non_precoded_v1430_s>().unpack(bref));
      break;
    case types::bf_v1430:
      HANDLE_CODE(c.get<csi_rs_cfg_bf_v1430_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1430_c::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_emimo_v1430_c::setup_c_::operator==(const setup_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::non_precoded_v1430:
      return c.get<csi_rs_cfg_non_precoded_v1430_s>() == other.c.get<csi_rs_cfg_non_precoded_v1430_s>();
    case types::bf_v1430:
      return c.get<csi_rs_cfg_bf_v1430_s>() == other.c.get<csi_rs_cfg_bf_v1430_s>();
    default:
      return true;
  }
  return true;
}

// CSI-RS-ConfigEMIMO-v1480 ::= CHOICE
void csi_rs_cfg_emimo_v1480_c::set(types::options e)
{
  type_ = e;
}
void csi_rs_cfg_emimo_v1480_c::set_release()
{
  set(types::release);
}
csi_rs_cfg_emimo_v1480_c::setup_c_& csi_rs_cfg_emimo_v1480_c::set_setup()
{
  set(types::setup);
  return c;
}
void csi_rs_cfg_emimo_v1480_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1480_c");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_emimo_v1480_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1480_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_emimo_v1480_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1480_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_emimo_v1480_c::operator==(const csi_rs_cfg_emimo_v1480_c& other) const
{
  return type() == other.type() and c == other.c;
}

void csi_rs_cfg_emimo_v1480_c::setup_c_::destroy_()
{
  switch (type_) {
    case types::non_precoded_v1480:
      c.destroy<csi_rs_cfg_non_precoded_v1480_s>();
      break;
    case types::bf_v1480:
      c.destroy<csi_rs_cfg_bf_v1430_s>();
      break;
    default:
      break;
  }
}
void csi_rs_cfg_emimo_v1480_c::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::non_precoded_v1480:
      c.init<csi_rs_cfg_non_precoded_v1480_s>();
      break;
    case types::bf_v1480:
      c.init<csi_rs_cfg_bf_v1430_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1480_c::setup_c_");
  }
}
csi_rs_cfg_emimo_v1480_c::setup_c_::setup_c_(const csi_rs_cfg_emimo_v1480_c::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::non_precoded_v1480:
      c.init(other.c.get<csi_rs_cfg_non_precoded_v1480_s>());
      break;
    case types::bf_v1480:
      c.init(other.c.get<csi_rs_cfg_bf_v1430_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1480_c::setup_c_");
  }
}
csi_rs_cfg_emimo_v1480_c::setup_c_&
csi_rs_cfg_emimo_v1480_c::setup_c_::operator=(const csi_rs_cfg_emimo_v1480_c::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::non_precoded_v1480:
      c.set(other.c.get<csi_rs_cfg_non_precoded_v1480_s>());
      break;
    case types::bf_v1480:
      c.set(other.c.get<csi_rs_cfg_bf_v1430_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1480_c::setup_c_");
  }

  return *this;
}
csi_rs_cfg_non_precoded_v1480_s& csi_rs_cfg_emimo_v1480_c::setup_c_::set_non_precoded_v1480()
{
  set(types::non_precoded_v1480);
  return c.get<csi_rs_cfg_non_precoded_v1480_s>();
}
csi_rs_cfg_bf_v1430_s& csi_rs_cfg_emimo_v1480_c::setup_c_::set_bf_v1480()
{
  set(types::bf_v1480);
  return c.get<csi_rs_cfg_bf_v1430_s>();
}
void csi_rs_cfg_emimo_v1480_c::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::non_precoded_v1480:
      j.write_fieldname("nonPrecoded-v1480");
      c.get<csi_rs_cfg_non_precoded_v1480_s>().to_json(j);
      break;
    case types::bf_v1480:
      j.write_fieldname("beamformed-v1480");
      c.get<csi_rs_cfg_bf_v1430_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1480_c::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_emimo_v1480_c::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::non_precoded_v1480:
      HANDLE_CODE(c.get<csi_rs_cfg_non_precoded_v1480_s>().pack(bref));
      break;
    case types::bf_v1480:
      HANDLE_CODE(c.get<csi_rs_cfg_bf_v1430_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1480_c::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_emimo_v1480_c::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::non_precoded_v1480:
      HANDLE_CODE(c.get<csi_rs_cfg_non_precoded_v1480_s>().unpack(bref));
      break;
    case types::bf_v1480:
      HANDLE_CODE(c.get<csi_rs_cfg_bf_v1430_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1480_c::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_emimo_v1480_c::setup_c_::operator==(const setup_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::non_precoded_v1480:
      return c.get<csi_rs_cfg_non_precoded_v1480_s>() == other.c.get<csi_rs_cfg_non_precoded_v1480_s>();
    case types::bf_v1480:
      return c.get<csi_rs_cfg_bf_v1430_s>() == other.c.get<csi_rs_cfg_bf_v1430_s>();
    default:
      return true;
  }
  return true;
}

// CSI-RS-ConfigEMIMO-v1530 ::= CHOICE
void csi_rs_cfg_emimo_v1530_c::set(types::options e)
{
  type_ = e;
}
void csi_rs_cfg_emimo_v1530_c::set_release()
{
  set(types::release);
}
csi_rs_cfg_emimo_v1530_c::setup_c_& csi_rs_cfg_emimo_v1530_c::set_setup()
{
  set(types::setup);
  return c;
}
void csi_rs_cfg_emimo_v1530_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1530_c");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_emimo_v1530_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1530_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_emimo_v1530_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_emimo_v1530_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_emimo_v1530_c::operator==(const csi_rs_cfg_emimo_v1530_c& other) const
{
  return type() == other.type() and c == other.c;
}

void csi_rs_cfg_emimo_v1530_c::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("nonPrecoded-v1530");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_emimo_v1530_c::setup_c_::pack(bit_ref& bref) const
{
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_emimo_v1530_c::setup_c_::unpack(cbit_ref& bref)
{
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_emimo_v1530_c::setup_c_::operator==(const setup_c_& other) const
{
  return type() == other.type() and c == other.c;
}

// CQI-ReportPeriodicProcExt-r11 ::= SEQUENCE
SRSASN_CODE cqi_report_periodic_proc_ext_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ri_cfg_idx_r11_present, 1));
  HANDLE_CODE(bref.pack(csi_cfg_idx_r11_present, 1));

  HANDLE_CODE(pack_integer(bref, cqi_report_periodic_proc_ext_id_r11, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(pack_integer(bref, cqi_pmi_cfg_idx_r11, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(cqi_format_ind_periodic_r11.pack(bref));
  if (ri_cfg_idx_r11_present) {
    HANDLE_CODE(pack_integer(bref, ri_cfg_idx_r11, (uint16_t)0u, (uint16_t)1023u));
  }
  if (csi_cfg_idx_r11_present) {
    HANDLE_CODE(csi_cfg_idx_r11.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= cri_report_cfg_r13.is_present();
    group_flags[1] |= periodicity_factor_wb_r13_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cri_report_cfg_r13.is_present(), 1));
      if (cri_report_cfg_r13.is_present()) {
        HANDLE_CODE(cri_report_cfg_r13->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(periodicity_factor_wb_r13_present, 1));
      if (periodicity_factor_wb_r13_present) {
        HANDLE_CODE(periodicity_factor_wb_r13.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_proc_ext_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ri_cfg_idx_r11_present, 1));
  HANDLE_CODE(bref.unpack(csi_cfg_idx_r11_present, 1));

  HANDLE_CODE(unpack_integer(cqi_report_periodic_proc_ext_id_r11, bref, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(unpack_integer(cqi_pmi_cfg_idx_r11, bref, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(cqi_format_ind_periodic_r11.unpack(bref));
  if (ri_cfg_idx_r11_present) {
    HANDLE_CODE(unpack_integer(ri_cfg_idx_r11, bref, (uint16_t)0u, (uint16_t)1023u));
  }
  if (csi_cfg_idx_r11_present) {
    HANDLE_CODE(csi_cfg_idx_r11.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cri_report_cfg_r13_present;
      HANDLE_CODE(bref.unpack(cri_report_cfg_r13_present, 1));
      cri_report_cfg_r13.set_present(cri_report_cfg_r13_present);
      if (cri_report_cfg_r13.is_present()) {
        HANDLE_CODE(cri_report_cfg_r13->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(periodicity_factor_wb_r13_present, 1));
      if (periodicity_factor_wb_r13_present) {
        HANDLE_CODE(periodicity_factor_wb_r13.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void cqi_report_periodic_proc_ext_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cqi-ReportPeriodicProcExtId-r11", cqi_report_periodic_proc_ext_id_r11);
  j.write_int("cqi-pmi-ConfigIndex-r11", cqi_pmi_cfg_idx_r11);
  j.write_fieldname("cqi-FormatIndicatorPeriodic-r11");
  cqi_format_ind_periodic_r11.to_json(j);
  if (ri_cfg_idx_r11_present) {
    j.write_int("ri-ConfigIndex-r11", ri_cfg_idx_r11);
  }
  if (csi_cfg_idx_r11_present) {
    j.write_fieldname("csi-ConfigIndex-r11");
    csi_cfg_idx_r11.to_json(j);
  }
  if (ext) {
    if (cri_report_cfg_r13.is_present()) {
      j.write_fieldname("cri-ReportConfig-r13");
      cri_report_cfg_r13->to_json(j);
    }
    if (periodicity_factor_wb_r13_present) {
      j.write_str("periodicityFactorWB-r13", periodicity_factor_wb_r13.to_string());
    }
  }
  j.end_obj();
}
bool cqi_report_periodic_proc_ext_r11_s::operator==(const cqi_report_periodic_proc_ext_r11_s& other) const
{
  return ext == other.ext and cqi_report_periodic_proc_ext_id_r11 == other.cqi_report_periodic_proc_ext_id_r11 and
         cqi_pmi_cfg_idx_r11 == other.cqi_pmi_cfg_idx_r11 and
         cqi_format_ind_periodic_r11 == other.cqi_format_ind_periodic_r11 and
         ri_cfg_idx_r11_present == other.ri_cfg_idx_r11_present and
         (not ri_cfg_idx_r11_present or ri_cfg_idx_r11 == other.ri_cfg_idx_r11) and
         csi_cfg_idx_r11_present == other.csi_cfg_idx_r11_present and
         (not csi_cfg_idx_r11_present or csi_cfg_idx_r11 == other.csi_cfg_idx_r11) and
         (not ext or
          (cri_report_cfg_r13.is_present() == other.cri_report_cfg_r13.is_present() and
           (not cri_report_cfg_r13.is_present() or *cri_report_cfg_r13 == *other.cri_report_cfg_r13) and
           periodicity_factor_wb_r13_present == other.periodicity_factor_wb_r13_present and
           (not periodicity_factor_wb_r13_present or periodicity_factor_wb_r13 == other.periodicity_factor_wb_r13)));
}

void cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::destroy_()
{
  switch (type_) {
    case types::wideband_cqi_r11:
      c.destroy<wideband_cqi_r11_s_>();
      break;
    case types::subband_cqi_r11:
      c.destroy<subband_cqi_r11_s_>();
      break;
    default:
      break;
  }
}
void cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::wideband_cqi_r11:
      c.init<wideband_cqi_r11_s_>();
      break;
    case types::subband_cqi_r11:
      c.init<subband_cqi_r11_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_");
  }
}
cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::cqi_format_ind_periodic_r11_c_(
    const cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::wideband_cqi_r11:
      c.init(other.c.get<wideband_cqi_r11_s_>());
      break;
    case types::subband_cqi_r11:
      c.init(other.c.get<subband_cqi_r11_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_");
  }
}
cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_&
cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::operator=(
    const cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::wideband_cqi_r11:
      c.set(other.c.get<wideband_cqi_r11_s_>());
      break;
    case types::subband_cqi_r11:
      c.set(other.c.get<subband_cqi_r11_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_");
  }

  return *this;
}
cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::wideband_cqi_r11_s_&
cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::set_wideband_cqi_r11()
{
  set(types::wideband_cqi_r11);
  return c.get<wideband_cqi_r11_s_>();
}
cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::subband_cqi_r11_s_&
cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::set_subband_cqi_r11()
{
  set(types::subband_cqi_r11);
  return c.get<subband_cqi_r11_s_>();
}
void cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::wideband_cqi_r11:
      j.write_fieldname("widebandCQI-r11");
      j.start_obj();
      if (c.get<wideband_cqi_r11_s_>().csi_report_mode_r11_present) {
        j.write_str("csi-ReportMode-r11", c.get<wideband_cqi_r11_s_>().csi_report_mode_r11.to_string());
      }
      j.end_obj();
      break;
    case types::subband_cqi_r11:
      j.write_fieldname("subbandCQI-r11");
      j.start_obj();
      j.write_int("k", c.get<subband_cqi_r11_s_>().k);
      j.write_str("periodicityFactor-r11", c.get<subband_cqi_r11_s_>().periodicity_factor_r11.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::wideband_cqi_r11:
      HANDLE_CODE(bref.pack(c.get<wideband_cqi_r11_s_>().csi_report_mode_r11_present, 1));
      if (c.get<wideband_cqi_r11_s_>().csi_report_mode_r11_present) {
        HANDLE_CODE(c.get<wideband_cqi_r11_s_>().csi_report_mode_r11.pack(bref));
      }
      break;
    case types::subband_cqi_r11:
      HANDLE_CODE(pack_integer(bref, c.get<subband_cqi_r11_s_>().k, (uint8_t)1u, (uint8_t)4u));
      HANDLE_CODE(c.get<subband_cqi_r11_s_>().periodicity_factor_r11.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::wideband_cqi_r11:
      HANDLE_CODE(bref.unpack(c.get<wideband_cqi_r11_s_>().csi_report_mode_r11_present, 1));
      if (c.get<wideband_cqi_r11_s_>().csi_report_mode_r11_present) {
        HANDLE_CODE(c.get<wideband_cqi_r11_s_>().csi_report_mode_r11.unpack(bref));
      }
      break;
    case types::subband_cqi_r11:
      HANDLE_CODE(unpack_integer(c.get<subband_cqi_r11_s_>().k, bref, (uint8_t)1u, (uint8_t)4u));
      HANDLE_CODE(c.get<subband_cqi_r11_s_>().periodicity_factor_r11.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::operator==(
    const cqi_format_ind_periodic_r11_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::wideband_cqi_r11:
      return c.get<wideband_cqi_r11_s_>().csi_report_mode_r11_present ==
                 other.c.get<wideband_cqi_r11_s_>().csi_report_mode_r11_present and
             (not c.get<wideband_cqi_r11_s_>().csi_report_mode_r11_present or
              c.get<wideband_cqi_r11_s_>().csi_report_mode_r11 ==
                  other.c.get<wideband_cqi_r11_s_>().csi_report_mode_r11);
    case types::subband_cqi_r11:
      return c.get<subband_cqi_r11_s_>().k == other.c.get<subband_cqi_r11_s_>().k and
             c.get<subband_cqi_r11_s_>().periodicity_factor_r11 ==
                 other.c.get<subband_cqi_r11_s_>().periodicity_factor_r11;
    default:
      return true;
  }
  return true;
}

const char* cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::wideband_cqi_r11_s_::
    csi_report_mode_r11_opts::to_string() const
{
  static const char* options[] = {"submode1", "submode2"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::wideband_cqi_r11_s_::"
                          "csi_report_mode_r11_e_");
}
uint8_t cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::wideband_cqi_r11_s_::
    csi_report_mode_r11_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::wideband_cqi_r11_s_::csi_"
                         "report_mode_r11_e_");
}

const char* cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::subband_cqi_r11_s_::
    periodicity_factor_r11_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::subband_cqi_r11_s_::"
                          "periodicity_factor_r11_e_");
}
uint8_t cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::subband_cqi_r11_s_::
    periodicity_factor_r11_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::subband_cqi_r11_s_::"
                         "periodicity_factor_r11_e_");
}

void cqi_report_periodic_proc_ext_r11_s::csi_cfg_idx_r11_c_::set(types::options e)
{
  type_ = e;
}
void cqi_report_periodic_proc_ext_r11_s::csi_cfg_idx_r11_c_::set_release()
{
  set(types::release);
}
cqi_report_periodic_proc_ext_r11_s::csi_cfg_idx_r11_c_::setup_s_&
cqi_report_periodic_proc_ext_r11_s::csi_cfg_idx_r11_c_::set_setup()
{
  set(types::setup);
  return c;
}
void cqi_report_periodic_proc_ext_r11_s::csi_cfg_idx_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("cqi-pmi-ConfigIndex2-r11", c.cqi_pmi_cfg_idx2_r11);
      if (c.ri_cfg_idx2_r11_present) {
        j.write_int("ri-ConfigIndex2-r11", c.ri_cfg_idx2_r11);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_proc_ext_r11_s::csi_cfg_idx_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_periodic_proc_ext_r11_s::csi_cfg_idx_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.ri_cfg_idx2_r11_present, 1));
      HANDLE_CODE(pack_integer(bref, c.cqi_pmi_cfg_idx2_r11, (uint16_t)0u, (uint16_t)1023u));
      if (c.ri_cfg_idx2_r11_present) {
        HANDLE_CODE(pack_integer(bref, c.ri_cfg_idx2_r11, (uint16_t)0u, (uint16_t)1023u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_proc_ext_r11_s::csi_cfg_idx_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_proc_ext_r11_s::csi_cfg_idx_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.ri_cfg_idx2_r11_present, 1));
      HANDLE_CODE(unpack_integer(c.cqi_pmi_cfg_idx2_r11, bref, (uint16_t)0u, (uint16_t)1023u));
      if (c.ri_cfg_idx2_r11_present) {
        HANDLE_CODE(unpack_integer(c.ri_cfg_idx2_r11, bref, (uint16_t)0u, (uint16_t)1023u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_proc_ext_r11_s::csi_cfg_idx_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_periodic_proc_ext_r11_s::csi_cfg_idx_r11_c_::operator==(const csi_cfg_idx_r11_c_& other) const
{
  return type() == other.type() and c.cqi_pmi_cfg_idx2_r11 == other.c.cqi_pmi_cfg_idx2_r11 and
         c.ri_cfg_idx2_r11_present == other.c.ri_cfg_idx2_r11_present and
         (not c.ri_cfg_idx2_r11_present or c.ri_cfg_idx2_r11 == other.c.ri_cfg_idx2_r11);
}

const char* cqi_report_periodic_proc_ext_r11_s::periodicity_factor_wb_r13_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "cqi_report_periodic_proc_ext_r11_s::periodicity_factor_wb_r13_e_");
}
uint8_t cqi_report_periodic_proc_ext_r11_s::periodicity_factor_wb_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "cqi_report_periodic_proc_ext_r11_s::periodicity_factor_wb_r13_e_");
}

// CSI-IM-Config-r11 ::= SEQUENCE
SRSASN_CODE csi_im_cfg_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, csi_im_cfg_id_r11, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(pack_integer(bref, res_cfg_r11, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, sf_cfg_r11, (uint8_t)0u, (uint8_t)154u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= interference_meas_restrict_r13_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(interference_meas_restrict_r13_present, 1));
      if (interference_meas_restrict_r13_present) {
        HANDLE_CODE(bref.pack(interference_meas_restrict_r13, 1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_im_cfg_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(csi_im_cfg_id_r11, bref, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(unpack_integer(res_cfg_r11, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(sf_cfg_r11, bref, (uint8_t)0u, (uint8_t)154u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(interference_meas_restrict_r13_present, 1));
      if (interference_meas_restrict_r13_present) {
        HANDLE_CODE(bref.unpack(interference_meas_restrict_r13, 1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void csi_im_cfg_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-IM-ConfigId-r11", csi_im_cfg_id_r11);
  j.write_int("resourceConfig-r11", res_cfg_r11);
  j.write_int("subframeConfig-r11", sf_cfg_r11);
  if (ext) {
    if (interference_meas_restrict_r13_present) {
      j.write_bool("interferenceMeasRestriction-r13", interference_meas_restrict_r13);
    }
  }
  j.end_obj();
}
bool csi_im_cfg_r11_s::operator==(const csi_im_cfg_r11_s& other) const
{
  return ext == other.ext and csi_im_cfg_id_r11 == other.csi_im_cfg_id_r11 and res_cfg_r11 == other.res_cfg_r11 and
         sf_cfg_r11 == other.sf_cfg_r11 and
         (not ext or (interference_meas_restrict_r13_present == other.interference_meas_restrict_r13_present and
                      (not interference_meas_restrict_r13_present or
                       interference_meas_restrict_r13 == other.interference_meas_restrict_r13)));
}

// CSI-IM-ConfigExt-r12 ::= SEQUENCE
SRSASN_CODE csi_im_cfg_ext_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, csi_im_cfg_id_v1250, (uint8_t)4u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, res_cfg_r12, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, sf_cfg_r12, (uint8_t)0u, (uint8_t)154u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= interference_meas_restrict_r13_present;
    group_flags[0] |= csi_im_cfg_id_v1310_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(interference_meas_restrict_r13_present, 1));
      HANDLE_CODE(bref.pack(csi_im_cfg_id_v1310_present, 1));
      if (interference_meas_restrict_r13_present) {
        HANDLE_CODE(bref.pack(interference_meas_restrict_r13, 1));
      }
      if (csi_im_cfg_id_v1310_present) {
        HANDLE_CODE(pack_integer(bref, csi_im_cfg_id_v1310, (uint8_t)5u, (uint8_t)24u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_im_cfg_ext_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(csi_im_cfg_id_v1250, bref, (uint8_t)4u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(res_cfg_r12, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(sf_cfg_r12, bref, (uint8_t)0u, (uint8_t)154u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(interference_meas_restrict_r13_present, 1));
      HANDLE_CODE(bref.unpack(csi_im_cfg_id_v1310_present, 1));
      if (interference_meas_restrict_r13_present) {
        HANDLE_CODE(bref.unpack(interference_meas_restrict_r13, 1));
      }
      if (csi_im_cfg_id_v1310_present) {
        HANDLE_CODE(unpack_integer(csi_im_cfg_id_v1310, bref, (uint8_t)5u, (uint8_t)24u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void csi_im_cfg_ext_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-IM-ConfigId-v1250", csi_im_cfg_id_v1250);
  j.write_int("resourceConfig-r12", res_cfg_r12);
  j.write_int("subframeConfig-r12", sf_cfg_r12);
  if (ext) {
    if (interference_meas_restrict_r13_present) {
      j.write_bool("interferenceMeasRestriction-r13", interference_meas_restrict_r13);
    }
    if (csi_im_cfg_id_v1310_present) {
      j.write_int("csi-IM-ConfigId-v1310", csi_im_cfg_id_v1310);
    }
  }
  j.end_obj();
}
bool csi_im_cfg_ext_r12_s::operator==(const csi_im_cfg_ext_r12_s& other) const
{
  return ext == other.ext and csi_im_cfg_id_v1250 == other.csi_im_cfg_id_v1250 and res_cfg_r12 == other.res_cfg_r12 and
         sf_cfg_r12 == other.sf_cfg_r12 and
         (not ext or (interference_meas_restrict_r13_present == other.interference_meas_restrict_r13_present and
                      (not interference_meas_restrict_r13_present or
                       interference_meas_restrict_r13 == other.interference_meas_restrict_r13) and
                      csi_im_cfg_id_v1310_present == other.csi_im_cfg_id_v1310_present and
                      (not csi_im_cfg_id_v1310_present or csi_im_cfg_id_v1310 == other.csi_im_cfg_id_v1310)));
}

// CSI-Process-r11 ::= SEQUENCE
SRSASN_CODE csi_process_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cqi_report_both_proc_r11_present, 1));
  HANDLE_CODE(bref.pack(cqi_report_periodic_proc_id_r11_present, 1));
  HANDLE_CODE(bref.pack(cqi_report_aperiodic_proc_r11_present, 1));

  HANDLE_CODE(pack_integer(bref, csi_process_id_r11, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, csi_rs_cfg_nzp_id_r11, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(pack_integer(bref, csi_im_cfg_id_r11, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(pack_dyn_seq_of(bref, p_c_and_cbsr_list_r11, 1, 2));
  if (cqi_report_both_proc_r11_present) {
    HANDLE_CODE(cqi_report_both_proc_r11.pack(bref));
  }
  if (cqi_report_periodic_proc_id_r11_present) {
    HANDLE_CODE(pack_integer(bref, cqi_report_periodic_proc_id_r11, (uint8_t)0u, (uint8_t)3u));
  }
  if (cqi_report_aperiodic_proc_r11_present) {
    HANDLE_CODE(cqi_report_aperiodic_proc_r11.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= alt_codebook_enabled_for4_tx_proc_r12_present;
    group_flags[0] |= csi_im_cfg_id_list_r12.is_present();
    group_flags[0] |= cqi_report_aperiodic_proc2_r12.is_present();
    group_flags[1] |= cqi_report_aperiodic_proc_v1310.is_present();
    group_flags[1] |= cqi_report_aperiodic_proc2_v1310.is_present();
    group_flags[1] |= emimo_type_r13.is_present();
    group_flags[2] |= dummy.is_present();
    group_flags[2] |= emimo_hybrid_r14.is_present();
    group_flags[2] |= advanced_codebook_enabled_r14_present;
    group_flags[3] |= emimo_type_v1480.is_present();
    group_flags[4] |= fe_comp_csi_enabled_v1530_present;
    group_flags[4] |= emimo_type_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(alt_codebook_enabled_for4_tx_proc_r12_present, 1));
      HANDLE_CODE(bref.pack(csi_im_cfg_id_list_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(cqi_report_aperiodic_proc2_r12.is_present(), 1));
      if (csi_im_cfg_id_list_r12.is_present()) {
        HANDLE_CODE(csi_im_cfg_id_list_r12->pack(bref));
      }
      if (cqi_report_aperiodic_proc2_r12.is_present()) {
        HANDLE_CODE(cqi_report_aperiodic_proc2_r12->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cqi_report_aperiodic_proc_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(cqi_report_aperiodic_proc2_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(emimo_type_r13.is_present(), 1));
      if (cqi_report_aperiodic_proc_v1310.is_present()) {
        HANDLE_CODE(cqi_report_aperiodic_proc_v1310->pack(bref));
      }
      if (cqi_report_aperiodic_proc2_v1310.is_present()) {
        HANDLE_CODE(cqi_report_aperiodic_proc2_v1310->pack(bref));
      }
      if (emimo_type_r13.is_present()) {
        HANDLE_CODE(emimo_type_r13->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(dummy.is_present(), 1));
      HANDLE_CODE(bref.pack(emimo_hybrid_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(advanced_codebook_enabled_r14_present, 1));
      if (dummy.is_present()) {
        HANDLE_CODE(dummy->pack(bref));
      }
      if (emimo_hybrid_r14.is_present()) {
        HANDLE_CODE(emimo_hybrid_r14->pack(bref));
      }
      if (advanced_codebook_enabled_r14_present) {
        HANDLE_CODE(bref.pack(advanced_codebook_enabled_r14, 1));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(emimo_type_v1480.is_present(), 1));
      if (emimo_type_v1480.is_present()) {
        HANDLE_CODE(emimo_type_v1480->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(fe_comp_csi_enabled_v1530_present, 1));
      HANDLE_CODE(bref.pack(emimo_type_v1530.is_present(), 1));
      if (fe_comp_csi_enabled_v1530_present) {
        HANDLE_CODE(bref.pack(fe_comp_csi_enabled_v1530, 1));
      }
      if (emimo_type_v1530.is_present()) {
        HANDLE_CODE(emimo_type_v1530->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_process_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cqi_report_both_proc_r11_present, 1));
  HANDLE_CODE(bref.unpack(cqi_report_periodic_proc_id_r11_present, 1));
  HANDLE_CODE(bref.unpack(cqi_report_aperiodic_proc_r11_present, 1));

  HANDLE_CODE(unpack_integer(csi_process_id_r11, bref, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(csi_rs_cfg_nzp_id_r11, bref, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(unpack_integer(csi_im_cfg_id_r11, bref, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(unpack_dyn_seq_of(p_c_and_cbsr_list_r11, bref, 1, 2));
  if (cqi_report_both_proc_r11_present) {
    HANDLE_CODE(cqi_report_both_proc_r11.unpack(bref));
  }
  if (cqi_report_periodic_proc_id_r11_present) {
    HANDLE_CODE(unpack_integer(cqi_report_periodic_proc_id_r11, bref, (uint8_t)0u, (uint8_t)3u));
  }
  if (cqi_report_aperiodic_proc_r11_present) {
    HANDLE_CODE(cqi_report_aperiodic_proc_r11.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(5);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(alt_codebook_enabled_for4_tx_proc_r12_present, 1));
      bool csi_im_cfg_id_list_r12_present;
      HANDLE_CODE(bref.unpack(csi_im_cfg_id_list_r12_present, 1));
      csi_im_cfg_id_list_r12.set_present(csi_im_cfg_id_list_r12_present);
      bool cqi_report_aperiodic_proc2_r12_present;
      HANDLE_CODE(bref.unpack(cqi_report_aperiodic_proc2_r12_present, 1));
      cqi_report_aperiodic_proc2_r12.set_present(cqi_report_aperiodic_proc2_r12_present);
      if (csi_im_cfg_id_list_r12.is_present()) {
        HANDLE_CODE(csi_im_cfg_id_list_r12->unpack(bref));
      }
      if (cqi_report_aperiodic_proc2_r12.is_present()) {
        HANDLE_CODE(cqi_report_aperiodic_proc2_r12->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cqi_report_aperiodic_proc_v1310_present;
      HANDLE_CODE(bref.unpack(cqi_report_aperiodic_proc_v1310_present, 1));
      cqi_report_aperiodic_proc_v1310.set_present(cqi_report_aperiodic_proc_v1310_present);
      bool cqi_report_aperiodic_proc2_v1310_present;
      HANDLE_CODE(bref.unpack(cqi_report_aperiodic_proc2_v1310_present, 1));
      cqi_report_aperiodic_proc2_v1310.set_present(cqi_report_aperiodic_proc2_v1310_present);
      bool emimo_type_r13_present;
      HANDLE_CODE(bref.unpack(emimo_type_r13_present, 1));
      emimo_type_r13.set_present(emimo_type_r13_present);
      if (cqi_report_aperiodic_proc_v1310.is_present()) {
        HANDLE_CODE(cqi_report_aperiodic_proc_v1310->unpack(bref));
      }
      if (cqi_report_aperiodic_proc2_v1310.is_present()) {
        HANDLE_CODE(cqi_report_aperiodic_proc2_v1310->unpack(bref));
      }
      if (emimo_type_r13.is_present()) {
        HANDLE_CODE(emimo_type_r13->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool dummy_present;
      HANDLE_CODE(bref.unpack(dummy_present, 1));
      dummy.set_present(dummy_present);
      bool emimo_hybrid_r14_present;
      HANDLE_CODE(bref.unpack(emimo_hybrid_r14_present, 1));
      emimo_hybrid_r14.set_present(emimo_hybrid_r14_present);
      HANDLE_CODE(bref.unpack(advanced_codebook_enabled_r14_present, 1));
      if (dummy.is_present()) {
        HANDLE_CODE(dummy->unpack(bref));
      }
      if (emimo_hybrid_r14.is_present()) {
        HANDLE_CODE(emimo_hybrid_r14->unpack(bref));
      }
      if (advanced_codebook_enabled_r14_present) {
        HANDLE_CODE(bref.unpack(advanced_codebook_enabled_r14, 1));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool emimo_type_v1480_present;
      HANDLE_CODE(bref.unpack(emimo_type_v1480_present, 1));
      emimo_type_v1480.set_present(emimo_type_v1480_present);
      if (emimo_type_v1480.is_present()) {
        HANDLE_CODE(emimo_type_v1480->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(fe_comp_csi_enabled_v1530_present, 1));
      bool emimo_type_v1530_present;
      HANDLE_CODE(bref.unpack(emimo_type_v1530_present, 1));
      emimo_type_v1530.set_present(emimo_type_v1530_present);
      if (fe_comp_csi_enabled_v1530_present) {
        HANDLE_CODE(bref.unpack(fe_comp_csi_enabled_v1530, 1));
      }
      if (emimo_type_v1530.is_present()) {
        HANDLE_CODE(emimo_type_v1530->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void csi_process_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-ProcessId-r11", csi_process_id_r11);
  j.write_int("csi-RS-ConfigNZPId-r11", csi_rs_cfg_nzp_id_r11);
  j.write_int("csi-IM-ConfigId-r11", csi_im_cfg_id_r11);
  j.start_array("p-C-AndCBSRList-r11");
  for (const auto& e1 : p_c_and_cbsr_list_r11) {
    e1.to_json(j);
  }
  j.end_array();
  if (cqi_report_both_proc_r11_present) {
    j.write_fieldname("cqi-ReportBothProc-r11");
    cqi_report_both_proc_r11.to_json(j);
  }
  if (cqi_report_periodic_proc_id_r11_present) {
    j.write_int("cqi-ReportPeriodicProcId-r11", cqi_report_periodic_proc_id_r11);
  }
  if (cqi_report_aperiodic_proc_r11_present) {
    j.write_fieldname("cqi-ReportAperiodicProc-r11");
    cqi_report_aperiodic_proc_r11.to_json(j);
  }
  if (ext) {
    if (alt_codebook_enabled_for4_tx_proc_r12_present) {
      j.write_str("alternativeCodebookEnabledFor4TXProc-r12", "true");
    }
    if (csi_im_cfg_id_list_r12.is_present()) {
      j.write_fieldname("csi-IM-ConfigIdList-r12");
      csi_im_cfg_id_list_r12->to_json(j);
    }
    if (cqi_report_aperiodic_proc2_r12.is_present()) {
      j.write_fieldname("cqi-ReportAperiodicProc2-r12");
      cqi_report_aperiodic_proc2_r12->to_json(j);
    }
    if (cqi_report_aperiodic_proc_v1310.is_present()) {
      j.write_fieldname("cqi-ReportAperiodicProc-v1310");
      cqi_report_aperiodic_proc_v1310->to_json(j);
    }
    if (cqi_report_aperiodic_proc2_v1310.is_present()) {
      j.write_fieldname("cqi-ReportAperiodicProc2-v1310");
      cqi_report_aperiodic_proc2_v1310->to_json(j);
    }
    if (emimo_type_r13.is_present()) {
      j.write_fieldname("eMIMO-Type-r13");
      emimo_type_r13->to_json(j);
    }
    if (dummy.is_present()) {
      j.write_fieldname("dummy");
      dummy->to_json(j);
    }
    if (emimo_hybrid_r14.is_present()) {
      j.write_fieldname("eMIMO-Hybrid-r14");
      emimo_hybrid_r14->to_json(j);
    }
    if (advanced_codebook_enabled_r14_present) {
      j.write_bool("advancedCodebookEnabled-r14", advanced_codebook_enabled_r14);
    }
    if (emimo_type_v1480.is_present()) {
      j.write_fieldname("eMIMO-Type-v1480");
      emimo_type_v1480->to_json(j);
    }
    if (fe_comp_csi_enabled_v1530_present) {
      j.write_bool("feCOMP-CSI-Enabled-v1530", fe_comp_csi_enabled_v1530);
    }
    if (emimo_type_v1530.is_present()) {
      j.write_fieldname("eMIMO-Type-v1530");
      emimo_type_v1530->to_json(j);
    }
  }
  j.end_obj();
}
bool csi_process_r11_s::operator==(const csi_process_r11_s& other) const
{
  return ext == other.ext and csi_process_id_r11 == other.csi_process_id_r11 and
         csi_rs_cfg_nzp_id_r11 == other.csi_rs_cfg_nzp_id_r11 and csi_im_cfg_id_r11 == other.csi_im_cfg_id_r11 and
         p_c_and_cbsr_list_r11 == other.p_c_and_cbsr_list_r11 and
         cqi_report_both_proc_r11_present == other.cqi_report_both_proc_r11_present and
         (not cqi_report_both_proc_r11_present or cqi_report_both_proc_r11 == other.cqi_report_both_proc_r11) and
         cqi_report_periodic_proc_id_r11_present == other.cqi_report_periodic_proc_id_r11_present and
         (not cqi_report_periodic_proc_id_r11_present or
          cqi_report_periodic_proc_id_r11 == other.cqi_report_periodic_proc_id_r11) and
         cqi_report_aperiodic_proc_r11_present == other.cqi_report_aperiodic_proc_r11_present and
         (not cqi_report_aperiodic_proc_r11_present or
          cqi_report_aperiodic_proc_r11 == other.cqi_report_aperiodic_proc_r11) and
         (not ext or
          (alt_codebook_enabled_for4_tx_proc_r12_present == other.alt_codebook_enabled_for4_tx_proc_r12_present and
           csi_im_cfg_id_list_r12.is_present() == other.csi_im_cfg_id_list_r12.is_present() and
           (not csi_im_cfg_id_list_r12.is_present() or *csi_im_cfg_id_list_r12 == *other.csi_im_cfg_id_list_r12) and
           cqi_report_aperiodic_proc2_r12.is_present() == other.cqi_report_aperiodic_proc2_r12.is_present() and
           (not cqi_report_aperiodic_proc2_r12.is_present() or
            *cqi_report_aperiodic_proc2_r12 == *other.cqi_report_aperiodic_proc2_r12) and
           cqi_report_aperiodic_proc_v1310.is_present() == other.cqi_report_aperiodic_proc_v1310.is_present() and
           (not cqi_report_aperiodic_proc_v1310.is_present() or
            *cqi_report_aperiodic_proc_v1310 == *other.cqi_report_aperiodic_proc_v1310) and
           cqi_report_aperiodic_proc2_v1310.is_present() == other.cqi_report_aperiodic_proc2_v1310.is_present() and
           (not cqi_report_aperiodic_proc2_v1310.is_present() or
            *cqi_report_aperiodic_proc2_v1310 == *other.cqi_report_aperiodic_proc2_v1310) and
           emimo_type_r13.is_present() == other.emimo_type_r13.is_present() and
           (not emimo_type_r13.is_present() or *emimo_type_r13 == *other.emimo_type_r13) and
           dummy.is_present() == other.dummy.is_present() and (not dummy.is_present() or *dummy == *other.dummy) and
           emimo_hybrid_r14.is_present() == other.emimo_hybrid_r14.is_present() and
           (not emimo_hybrid_r14.is_present() or *emimo_hybrid_r14 == *other.emimo_hybrid_r14) and
           advanced_codebook_enabled_r14_present == other.advanced_codebook_enabled_r14_present and
           (not advanced_codebook_enabled_r14_present or
            advanced_codebook_enabled_r14 == other.advanced_codebook_enabled_r14) and
           emimo_type_v1480.is_present() == other.emimo_type_v1480.is_present() and
           (not emimo_type_v1480.is_present() or *emimo_type_v1480 == *other.emimo_type_v1480) and
           fe_comp_csi_enabled_v1530_present == other.fe_comp_csi_enabled_v1530_present and
           (not fe_comp_csi_enabled_v1530_present or fe_comp_csi_enabled_v1530 == other.fe_comp_csi_enabled_v1530) and
           emimo_type_v1530.is_present() == other.emimo_type_v1530.is_present() and
           (not emimo_type_v1530.is_present() or *emimo_type_v1530 == *other.emimo_type_v1530)));
}

void csi_process_r11_s::csi_im_cfg_id_list_r12_c_::set(types::options e)
{
  type_ = e;
}
void csi_process_r11_s::csi_im_cfg_id_list_r12_c_::set_release()
{
  set(types::release);
}
csi_process_r11_s::csi_im_cfg_id_list_r12_c_::setup_l_& csi_process_r11_s::csi_im_cfg_id_list_r12_c_::set_setup()
{
  set(types::setup);
  return c;
}
void csi_process_r11_s::csi_im_cfg_id_list_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.start_array("setup");
      for (const auto& e1 : c) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "csi_process_r11_s::csi_im_cfg_id_list_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_process_r11_s::csi_im_cfg_id_list_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 2, integer_packer<uint8_t>(1, 4)));
      break;
    default:
      log_invalid_choice_id(type_, "csi_process_r11_s::csi_im_cfg_id_list_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_process_r11_s::csi_im_cfg_id_list_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 2, integer_packer<uint8_t>(1, 4)));
      break;
    default:
      log_invalid_choice_id(type_, "csi_process_r11_s::csi_im_cfg_id_list_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_process_r11_s::csi_im_cfg_id_list_r12_c_::operator==(const csi_im_cfg_id_list_r12_c_& other) const
{
  return type() == other.type() and c == other.c;
}

void csi_process_r11_s::cqi_report_aperiodic_proc2_r12_c_::set(types::options e)
{
  type_ = e;
}
void csi_process_r11_s::cqi_report_aperiodic_proc2_r12_c_::set_release()
{
  set(types::release);
}
cqi_report_aperiodic_proc_r11_s& csi_process_r11_s::cqi_report_aperiodic_proc2_r12_c_::set_setup()
{
  set(types::setup);
  return c;
}
void csi_process_r11_s::cqi_report_aperiodic_proc2_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "csi_process_r11_s::cqi_report_aperiodic_proc2_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_process_r11_s::cqi_report_aperiodic_proc2_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_process_r11_s::cqi_report_aperiodic_proc2_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_process_r11_s::cqi_report_aperiodic_proc2_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_process_r11_s::cqi_report_aperiodic_proc2_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_process_r11_s::cqi_report_aperiodic_proc2_r12_c_::operator==(
    const cqi_report_aperiodic_proc2_r12_c_& other) const
{
  return type() == other.type() and c == other.c;
}

void csi_process_r11_s::cqi_report_aperiodic_proc_v1310_c_::set(types::options e)
{
  type_ = e;
}
void csi_process_r11_s::cqi_report_aperiodic_proc_v1310_c_::set_release()
{
  set(types::release);
}
cqi_report_aperiodic_proc_v1310_s& csi_process_r11_s::cqi_report_aperiodic_proc_v1310_c_::set_setup()
{
  set(types::setup);
  return c;
}
void csi_process_r11_s::cqi_report_aperiodic_proc_v1310_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "csi_process_r11_s::cqi_report_aperiodic_proc_v1310_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_process_r11_s::cqi_report_aperiodic_proc_v1310_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_process_r11_s::cqi_report_aperiodic_proc_v1310_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_process_r11_s::cqi_report_aperiodic_proc_v1310_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_process_r11_s::cqi_report_aperiodic_proc_v1310_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_process_r11_s::cqi_report_aperiodic_proc_v1310_c_::operator==(
    const cqi_report_aperiodic_proc_v1310_c_& other) const
{
  return type() == other.type() and c == other.c;
}

void csi_process_r11_s::cqi_report_aperiodic_proc2_v1310_c_::set(types::options e)
{
  type_ = e;
}
void csi_process_r11_s::cqi_report_aperiodic_proc2_v1310_c_::set_release()
{
  set(types::release);
}
cqi_report_aperiodic_proc_v1310_s& csi_process_r11_s::cqi_report_aperiodic_proc2_v1310_c_::set_setup()
{
  set(types::setup);
  return c;
}
void csi_process_r11_s::cqi_report_aperiodic_proc2_v1310_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "csi_process_r11_s::cqi_report_aperiodic_proc2_v1310_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_process_r11_s::cqi_report_aperiodic_proc2_v1310_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_process_r11_s::cqi_report_aperiodic_proc2_v1310_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_process_r11_s::cqi_report_aperiodic_proc2_v1310_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_process_r11_s::cqi_report_aperiodic_proc2_v1310_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_process_r11_s::cqi_report_aperiodic_proc2_v1310_c_::operator==(
    const cqi_report_aperiodic_proc2_v1310_c_& other) const
{
  return type() == other.type() and c == other.c;
}

// N4SPUCCH-Resource-r15 ::= SEQUENCE
SRSASN_CODE n4_spucch_res_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, n4start_prb_r15, (uint8_t)0u, (uint8_t)109u));
  HANDLE_CODE(pack_integer(bref, n4nof_prb_r15, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE n4_spucch_res_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(n4start_prb_r15, bref, (uint8_t)0u, (uint8_t)109u));
  HANDLE_CODE(unpack_integer(n4nof_prb_r15, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void n4_spucch_res_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("n4startingPRB-r15", n4start_prb_r15);
  j.write_int("n4numberOfPRB-r15", n4nof_prb_r15);
  j.end_obj();
}
bool n4_spucch_res_r15_s::operator==(const n4_spucch_res_r15_s& other) const
{
  return n4start_prb_r15 == other.n4start_prb_r15 and n4nof_prb_r15 == other.n4nof_prb_r15;
}

// ZeroTxPowerCSI-RS-r12 ::= SEQUENCE
SRSASN_CODE zero_tx_pwr_csi_rs_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(zero_tx_pwr_res_cfg_list_r12.pack(bref));
  HANDLE_CODE(pack_integer(bref, zero_tx_pwr_sf_cfg_r12, (uint8_t)0u, (uint8_t)154u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE zero_tx_pwr_csi_rs_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(zero_tx_pwr_res_cfg_list_r12.unpack(bref));
  HANDLE_CODE(unpack_integer(zero_tx_pwr_sf_cfg_r12, bref, (uint8_t)0u, (uint8_t)154u));

  return SRSASN_SUCCESS;
}
void zero_tx_pwr_csi_rs_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("zeroTxPowerResourceConfigList-r12", zero_tx_pwr_res_cfg_list_r12.to_string());
  j.write_int("zeroTxPowerSubframeConfig-r12", zero_tx_pwr_sf_cfg_r12);
  j.end_obj();
}
bool zero_tx_pwr_csi_rs_r12_s::operator==(const zero_tx_pwr_csi_rs_r12_s& other) const
{
  return zero_tx_pwr_res_cfg_list_r12 == other.zero_tx_pwr_res_cfg_list_r12 and
         zero_tx_pwr_sf_cfg_r12 == other.zero_tx_pwr_sf_cfg_r12;
}

// CQI-ReportAperiodic-r10 ::= CHOICE
void cqi_report_aperiodic_r10_c::set(types::options e)
{
  type_ = e;
}
void cqi_report_aperiodic_r10_c::set_release()
{
  set(types::release);
}
cqi_report_aperiodic_r10_c::setup_s_& cqi_report_aperiodic_r10_c::set_setup()
{
  set(types::setup);
  return c;
}
void cqi_report_aperiodic_r10_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("cqi-ReportModeAperiodic-r10", c.cqi_report_mode_aperiodic_r10.to_string());
      if (c.aperiodic_csi_trigger_r10_present) {
        j.write_fieldname("aperiodicCSI-Trigger-r10");
        j.start_obj();
        j.write_str("trigger1-r10", c.aperiodic_csi_trigger_r10.trigger1_r10.to_string());
        j.write_str("trigger2-r10", c.aperiodic_csi_trigger_r10.trigger2_r10.to_string());
        j.end_obj();
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_r10_c");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_aperiodic_r10_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.aperiodic_csi_trigger_r10_present, 1));
      HANDLE_CODE(c.cqi_report_mode_aperiodic_r10.pack(bref));
      if (c.aperiodic_csi_trigger_r10_present) {
        HANDLE_CODE(c.aperiodic_csi_trigger_r10.trigger1_r10.pack(bref));
        HANDLE_CODE(c.aperiodic_csi_trigger_r10.trigger2_r10.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_r10_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_aperiodic_r10_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.aperiodic_csi_trigger_r10_present, 1));
      HANDLE_CODE(c.cqi_report_mode_aperiodic_r10.unpack(bref));
      if (c.aperiodic_csi_trigger_r10_present) {
        HANDLE_CODE(c.aperiodic_csi_trigger_r10.trigger1_r10.unpack(bref));
        HANDLE_CODE(c.aperiodic_csi_trigger_r10.trigger2_r10.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_r10_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_aperiodic_r10_c::operator==(const cqi_report_aperiodic_r10_c& other) const
{
  return type() == other.type() and c.cqi_report_mode_aperiodic_r10 == other.c.cqi_report_mode_aperiodic_r10 and
         c.aperiodic_csi_trigger_r10.trigger1_r10 == other.c.aperiodic_csi_trigger_r10.trigger1_r10 and
         c.aperiodic_csi_trigger_r10.trigger2_r10 == other.c.aperiodic_csi_trigger_r10.trigger2_r10;
}

// CQI-ReportAperiodic-v1250 ::= CHOICE
void cqi_report_aperiodic_v1250_c::set(types::options e)
{
  type_ = e;
}
void cqi_report_aperiodic_v1250_c::set_release()
{
  set(types::release);
}
cqi_report_aperiodic_v1250_c::setup_s_& cqi_report_aperiodic_v1250_c::set_setup()
{
  set(types::setup);
  return c;
}
void cqi_report_aperiodic_v1250_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("aperiodicCSI-Trigger-v1250");
      j.start_obj();
      j.write_str("trigger-SubframeSetIndicator-r12", c.aperiodic_csi_trigger_v1250.trigger_sf_set_ind_r12.to_string());
      j.write_str("trigger1-SubframeSetIndicator-r12",
                  c.aperiodic_csi_trigger_v1250.trigger1_sf_set_ind_r12.to_string());
      j.write_str("trigger2-SubframeSetIndicator-r12",
                  c.aperiodic_csi_trigger_v1250.trigger2_sf_set_ind_r12.to_string());
      j.end_obj();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_v1250_c");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_aperiodic_v1250_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.aperiodic_csi_trigger_v1250.trigger_sf_set_ind_r12.pack(bref));
      HANDLE_CODE(c.aperiodic_csi_trigger_v1250.trigger1_sf_set_ind_r12.pack(bref));
      HANDLE_CODE(c.aperiodic_csi_trigger_v1250.trigger2_sf_set_ind_r12.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_v1250_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_aperiodic_v1250_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.aperiodic_csi_trigger_v1250.trigger_sf_set_ind_r12.unpack(bref));
      HANDLE_CODE(c.aperiodic_csi_trigger_v1250.trigger1_sf_set_ind_r12.unpack(bref));
      HANDLE_CODE(c.aperiodic_csi_trigger_v1250.trigger2_sf_set_ind_r12.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_v1250_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_aperiodic_v1250_c::operator==(const cqi_report_aperiodic_v1250_c& other) const
{
  return type() == other.type() and
         c.aperiodic_csi_trigger_v1250.trigger_sf_set_ind_r12 ==
             other.c.aperiodic_csi_trigger_v1250.trigger_sf_set_ind_r12 and
         c.aperiodic_csi_trigger_v1250.trigger1_sf_set_ind_r12 ==
             other.c.aperiodic_csi_trigger_v1250.trigger1_sf_set_ind_r12 and
         c.aperiodic_csi_trigger_v1250.trigger2_sf_set_ind_r12 ==
             other.c.aperiodic_csi_trigger_v1250.trigger2_sf_set_ind_r12;
}

const char*
cqi_report_aperiodic_v1250_c::setup_s_::aperiodic_csi_trigger_v1250_s_::trigger_sf_set_ind_r12_opts::to_string() const
{
  static const char* options[] = {"s1", "s2"};
  return convert_enum_idx(
      options,
      2,
      value,
      "cqi_report_aperiodic_v1250_c::setup_s_::aperiodic_csi_trigger_v1250_s_::trigger_sf_set_ind_r12_e_");
}
uint8_t
cqi_report_aperiodic_v1250_c::setup_s_::aperiodic_csi_trigger_v1250_s_::trigger_sf_set_ind_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(
      options,
      2,
      value,
      "cqi_report_aperiodic_v1250_c::setup_s_::aperiodic_csi_trigger_v1250_s_::trigger_sf_set_ind_r12_e_");
}

// CQI-ReportAperiodic-v1310 ::= CHOICE
void cqi_report_aperiodic_v1310_c::set(types::options e)
{
  type_ = e;
}
void cqi_report_aperiodic_v1310_c::set_release()
{
  set(types::release);
}
cqi_report_aperiodic_v1310_c::setup_s_& cqi_report_aperiodic_v1310_c::set_setup()
{
  set(types::setup);
  return c;
}
void cqi_report_aperiodic_v1310_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.aperiodic_csi_trigger_v1310_present) {
        j.write_fieldname("aperiodicCSI-Trigger-v1310");
        j.start_obj();
        j.write_str("trigger1-r13", c.aperiodic_csi_trigger_v1310.trigger1_r13.to_string());
        j.write_str("trigger2-r13", c.aperiodic_csi_trigger_v1310.trigger2_r13.to_string());
        j.write_str("trigger3-r13", c.aperiodic_csi_trigger_v1310.trigger3_r13.to_string());
        j.write_str("trigger4-r13", c.aperiodic_csi_trigger_v1310.trigger4_r13.to_string());
        j.write_str("trigger5-r13", c.aperiodic_csi_trigger_v1310.trigger5_r13.to_string());
        j.write_str("trigger6-r13", c.aperiodic_csi_trigger_v1310.trigger6_r13.to_string());
        j.end_obj();
      }
      if (c.aperiodic_csi_trigger2_r13_present) {
        j.write_fieldname("aperiodicCSI-Trigger2-r13");
        c.aperiodic_csi_trigger2_r13.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_v1310_c");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_aperiodic_v1310_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.aperiodic_csi_trigger_v1310_present, 1));
      HANDLE_CODE(bref.pack(c.aperiodic_csi_trigger2_r13_present, 1));
      if (c.aperiodic_csi_trigger_v1310_present) {
        HANDLE_CODE(c.aperiodic_csi_trigger_v1310.trigger1_r13.pack(bref));
        HANDLE_CODE(c.aperiodic_csi_trigger_v1310.trigger2_r13.pack(bref));
        HANDLE_CODE(c.aperiodic_csi_trigger_v1310.trigger3_r13.pack(bref));
        HANDLE_CODE(c.aperiodic_csi_trigger_v1310.trigger4_r13.pack(bref));
        HANDLE_CODE(c.aperiodic_csi_trigger_v1310.trigger5_r13.pack(bref));
        HANDLE_CODE(c.aperiodic_csi_trigger_v1310.trigger6_r13.pack(bref));
      }
      if (c.aperiodic_csi_trigger2_r13_present) {
        HANDLE_CODE(c.aperiodic_csi_trigger2_r13.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_v1310_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_aperiodic_v1310_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.aperiodic_csi_trigger_v1310_present, 1));
      HANDLE_CODE(bref.unpack(c.aperiodic_csi_trigger2_r13_present, 1));
      if (c.aperiodic_csi_trigger_v1310_present) {
        HANDLE_CODE(c.aperiodic_csi_trigger_v1310.trigger1_r13.unpack(bref));
        HANDLE_CODE(c.aperiodic_csi_trigger_v1310.trigger2_r13.unpack(bref));
        HANDLE_CODE(c.aperiodic_csi_trigger_v1310.trigger3_r13.unpack(bref));
        HANDLE_CODE(c.aperiodic_csi_trigger_v1310.trigger4_r13.unpack(bref));
        HANDLE_CODE(c.aperiodic_csi_trigger_v1310.trigger5_r13.unpack(bref));
        HANDLE_CODE(c.aperiodic_csi_trigger_v1310.trigger6_r13.unpack(bref));
      }
      if (c.aperiodic_csi_trigger2_r13_present) {
        HANDLE_CODE(c.aperiodic_csi_trigger2_r13.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_v1310_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_aperiodic_v1310_c::operator==(const cqi_report_aperiodic_v1310_c& other) const
{
  return type() == other.type() and
         c.aperiodic_csi_trigger_v1310.trigger1_r13 == other.c.aperiodic_csi_trigger_v1310.trigger1_r13 and
         c.aperiodic_csi_trigger_v1310.trigger2_r13 == other.c.aperiodic_csi_trigger_v1310.trigger2_r13 and
         c.aperiodic_csi_trigger_v1310.trigger3_r13 == other.c.aperiodic_csi_trigger_v1310.trigger3_r13 and
         c.aperiodic_csi_trigger_v1310.trigger4_r13 == other.c.aperiodic_csi_trigger_v1310.trigger4_r13 and
         c.aperiodic_csi_trigger_v1310.trigger5_r13 == other.c.aperiodic_csi_trigger_v1310.trigger5_r13 and
         c.aperiodic_csi_trigger_v1310.trigger6_r13 == other.c.aperiodic_csi_trigger_v1310.trigger6_r13 and
         c.aperiodic_csi_trigger2_r13_present == other.c.aperiodic_csi_trigger2_r13_present and
         (not c.aperiodic_csi_trigger2_r13_present or
          c.aperiodic_csi_trigger2_r13 == other.c.aperiodic_csi_trigger2_r13);
}

void cqi_report_aperiodic_v1310_c::setup_s_::aperiodic_csi_trigger2_r13_c_::set(types::options e)
{
  type_ = e;
}
void cqi_report_aperiodic_v1310_c::setup_s_::aperiodic_csi_trigger2_r13_c_::set_release()
{
  set(types::release);
}
cqi_report_aperiodic_v1310_c::setup_s_::aperiodic_csi_trigger2_r13_c_::setup_s_&
cqi_report_aperiodic_v1310_c::setup_s_::aperiodic_csi_trigger2_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void cqi_report_aperiodic_v1310_c::setup_s_::aperiodic_csi_trigger2_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("trigger1-SubframeSetIndicator-r13", c.trigger1_sf_set_ind_r13.to_string());
      j.write_str("trigger2-SubframeSetIndicator-r13", c.trigger2_sf_set_ind_r13.to_string());
      j.write_str("trigger3-SubframeSetIndicator-r13", c.trigger3_sf_set_ind_r13.to_string());
      j.write_str("trigger4-SubframeSetIndicator-r13", c.trigger4_sf_set_ind_r13.to_string());
      j.write_str("trigger5-SubframeSetIndicator-r13", c.trigger5_sf_set_ind_r13.to_string());
      j.write_str("trigger6-SubframeSetIndicator-r13", c.trigger6_sf_set_ind_r13.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_v1310_c::setup_s_::aperiodic_csi_trigger2_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_aperiodic_v1310_c::setup_s_::aperiodic_csi_trigger2_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.trigger1_sf_set_ind_r13.pack(bref));
      HANDLE_CODE(c.trigger2_sf_set_ind_r13.pack(bref));
      HANDLE_CODE(c.trigger3_sf_set_ind_r13.pack(bref));
      HANDLE_CODE(c.trigger4_sf_set_ind_r13.pack(bref));
      HANDLE_CODE(c.trigger5_sf_set_ind_r13.pack(bref));
      HANDLE_CODE(c.trigger6_sf_set_ind_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_v1310_c::setup_s_::aperiodic_csi_trigger2_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_aperiodic_v1310_c::setup_s_::aperiodic_csi_trigger2_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.trigger1_sf_set_ind_r13.unpack(bref));
      HANDLE_CODE(c.trigger2_sf_set_ind_r13.unpack(bref));
      HANDLE_CODE(c.trigger3_sf_set_ind_r13.unpack(bref));
      HANDLE_CODE(c.trigger4_sf_set_ind_r13.unpack(bref));
      HANDLE_CODE(c.trigger5_sf_set_ind_r13.unpack(bref));
      HANDLE_CODE(c.trigger6_sf_set_ind_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_v1310_c::setup_s_::aperiodic_csi_trigger2_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_aperiodic_v1310_c::setup_s_::aperiodic_csi_trigger2_r13_c_::operator==(
    const aperiodic_csi_trigger2_r13_c_& other) const
{
  return type() == other.type() and c.trigger1_sf_set_ind_r13 == other.c.trigger1_sf_set_ind_r13 and
         c.trigger2_sf_set_ind_r13 == other.c.trigger2_sf_set_ind_r13 and
         c.trigger3_sf_set_ind_r13 == other.c.trigger3_sf_set_ind_r13 and
         c.trigger4_sf_set_ind_r13 == other.c.trigger4_sf_set_ind_r13 and
         c.trigger5_sf_set_ind_r13 == other.c.trigger5_sf_set_ind_r13 and
         c.trigger6_sf_set_ind_r13 == other.c.trigger6_sf_set_ind_r13;
}

// CQI-ReportAperiodicHybrid-r14 ::= SEQUENCE
SRSASN_CODE cqi_report_aperiodic_hybrid_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(triggers_r14_present, 1));

  if (triggers_r14_present) {
    HANDLE_CODE(triggers_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_aperiodic_hybrid_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(triggers_r14_present, 1));

  if (triggers_r14_present) {
    HANDLE_CODE(triggers_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_aperiodic_hybrid_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (triggers_r14_present) {
    j.write_fieldname("triggers-r14");
    triggers_r14.to_json(j);
  }
  j.end_obj();
}
bool cqi_report_aperiodic_hybrid_r14_s::operator==(const cqi_report_aperiodic_hybrid_r14_s& other) const
{
  return triggers_r14_present == other.triggers_r14_present and
         (not triggers_r14_present or triggers_r14 == other.triggers_r14);
}

void cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::destroy_()
{
  switch (type_) {
    case types::one_bit_r14:
      c.destroy<one_bit_r14_s_>();
      break;
    case types::two_bit_r14:
      c.destroy<two_bit_r14_s_>();
      break;
    case types::three_bit_r14:
      c.destroy<three_bit_r14_s_>();
      break;
    default:
      break;
  }
}
void cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::one_bit_r14:
      c.init<one_bit_r14_s_>();
      break;
    case types::two_bit_r14:
      c.init<two_bit_r14_s_>();
      break;
    case types::three_bit_r14:
      c.init<three_bit_r14_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_");
  }
}
cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::triggers_r14_c_(
    const cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::one_bit_r14:
      c.init(other.c.get<one_bit_r14_s_>());
      break;
    case types::two_bit_r14:
      c.init(other.c.get<two_bit_r14_s_>());
      break;
    case types::three_bit_r14:
      c.init(other.c.get<three_bit_r14_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_");
  }
}
cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_& cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::operator=(
    const cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::one_bit_r14:
      c.set(other.c.get<one_bit_r14_s_>());
      break;
    case types::two_bit_r14:
      c.set(other.c.get<two_bit_r14_s_>());
      break;
    case types::three_bit_r14:
      c.set(other.c.get<three_bit_r14_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_");
  }

  return *this;
}
cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::one_bit_r14_s_&
cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::set_one_bit_r14()
{
  set(types::one_bit_r14);
  return c.get<one_bit_r14_s_>();
}
cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::two_bit_r14_s_&
cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::set_two_bit_r14()
{
  set(types::two_bit_r14);
  return c.get<two_bit_r14_s_>();
}
cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::three_bit_r14_s_&
cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::set_three_bit_r14()
{
  set(types::three_bit_r14);
  return c.get<three_bit_r14_s_>();
}
void cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::one_bit_r14:
      j.write_fieldname("oneBit-r14");
      j.start_obj();
      j.write_str("trigger1-Indicator-r14", c.get<one_bit_r14_s_>().trigger1_ind_r14.to_string());
      j.end_obj();
      break;
    case types::two_bit_r14:
      j.write_fieldname("twoBit-r14");
      j.start_obj();
      j.write_str("trigger01-Indicator-r14", c.get<two_bit_r14_s_>().trigger01_ind_r14.to_string());
      j.write_str("trigger10-Indicator-r14", c.get<two_bit_r14_s_>().trigger10_ind_r14.to_string());
      j.write_str("trigger11-Indicator-r14", c.get<two_bit_r14_s_>().trigger11_ind_r14.to_string());
      j.end_obj();
      break;
    case types::three_bit_r14:
      j.write_fieldname("threeBit-r14");
      j.start_obj();
      j.write_str("trigger001-Indicator-r14", c.get<three_bit_r14_s_>().trigger001_ind_r14.to_string());
      j.write_str("trigger010-Indicator-r14", c.get<three_bit_r14_s_>().trigger010_ind_r14.to_string());
      j.write_str("trigger011-Indicator-r14", c.get<three_bit_r14_s_>().trigger011_ind_r14.to_string());
      j.write_str("trigger100-Indicator-r14", c.get<three_bit_r14_s_>().trigger100_ind_r14.to_string());
      j.write_str("trigger101-Indicator-r14", c.get<three_bit_r14_s_>().trigger101_ind_r14.to_string());
      j.write_str("trigger110-Indicator-r14", c.get<three_bit_r14_s_>().trigger110_ind_r14.to_string());
      j.write_str("trigger111-Indicator-r14", c.get<three_bit_r14_s_>().trigger111_ind_r14.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::one_bit_r14:
      HANDLE_CODE(c.get<one_bit_r14_s_>().trigger1_ind_r14.pack(bref));
      break;
    case types::two_bit_r14:
      HANDLE_CODE(c.get<two_bit_r14_s_>().trigger01_ind_r14.pack(bref));
      HANDLE_CODE(c.get<two_bit_r14_s_>().trigger10_ind_r14.pack(bref));
      HANDLE_CODE(c.get<two_bit_r14_s_>().trigger11_ind_r14.pack(bref));
      break;
    case types::three_bit_r14:
      HANDLE_CODE(c.get<three_bit_r14_s_>().trigger001_ind_r14.pack(bref));
      HANDLE_CODE(c.get<three_bit_r14_s_>().trigger010_ind_r14.pack(bref));
      HANDLE_CODE(c.get<three_bit_r14_s_>().trigger011_ind_r14.pack(bref));
      HANDLE_CODE(c.get<three_bit_r14_s_>().trigger100_ind_r14.pack(bref));
      HANDLE_CODE(c.get<three_bit_r14_s_>().trigger101_ind_r14.pack(bref));
      HANDLE_CODE(c.get<three_bit_r14_s_>().trigger110_ind_r14.pack(bref));
      HANDLE_CODE(c.get<three_bit_r14_s_>().trigger111_ind_r14.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::one_bit_r14:
      HANDLE_CODE(c.get<one_bit_r14_s_>().trigger1_ind_r14.unpack(bref));
      break;
    case types::two_bit_r14:
      HANDLE_CODE(c.get<two_bit_r14_s_>().trigger01_ind_r14.unpack(bref));
      HANDLE_CODE(c.get<two_bit_r14_s_>().trigger10_ind_r14.unpack(bref));
      HANDLE_CODE(c.get<two_bit_r14_s_>().trigger11_ind_r14.unpack(bref));
      break;
    case types::three_bit_r14:
      HANDLE_CODE(c.get<three_bit_r14_s_>().trigger001_ind_r14.unpack(bref));
      HANDLE_CODE(c.get<three_bit_r14_s_>().trigger010_ind_r14.unpack(bref));
      HANDLE_CODE(c.get<three_bit_r14_s_>().trigger011_ind_r14.unpack(bref));
      HANDLE_CODE(c.get<three_bit_r14_s_>().trigger100_ind_r14.unpack(bref));
      HANDLE_CODE(c.get<three_bit_r14_s_>().trigger101_ind_r14.unpack(bref));
      HANDLE_CODE(c.get<three_bit_r14_s_>().trigger110_ind_r14.unpack(bref));
      HANDLE_CODE(c.get<three_bit_r14_s_>().trigger111_ind_r14.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::operator==(const triggers_r14_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::one_bit_r14:
      return c.get<one_bit_r14_s_>().trigger1_ind_r14 == other.c.get<one_bit_r14_s_>().trigger1_ind_r14;
    case types::two_bit_r14:
      return c.get<two_bit_r14_s_>().trigger01_ind_r14 == other.c.get<two_bit_r14_s_>().trigger01_ind_r14 and
             c.get<two_bit_r14_s_>().trigger10_ind_r14 == other.c.get<two_bit_r14_s_>().trigger10_ind_r14 and
             c.get<two_bit_r14_s_>().trigger11_ind_r14 == other.c.get<two_bit_r14_s_>().trigger11_ind_r14;
    case types::three_bit_r14:
      return c.get<three_bit_r14_s_>().trigger001_ind_r14 == other.c.get<three_bit_r14_s_>().trigger001_ind_r14 and
             c.get<three_bit_r14_s_>().trigger010_ind_r14 == other.c.get<three_bit_r14_s_>().trigger010_ind_r14 and
             c.get<three_bit_r14_s_>().trigger011_ind_r14 == other.c.get<three_bit_r14_s_>().trigger011_ind_r14 and
             c.get<three_bit_r14_s_>().trigger100_ind_r14 == other.c.get<three_bit_r14_s_>().trigger100_ind_r14 and
             c.get<three_bit_r14_s_>().trigger101_ind_r14 == other.c.get<three_bit_r14_s_>().trigger101_ind_r14 and
             c.get<three_bit_r14_s_>().trigger110_ind_r14 == other.c.get<three_bit_r14_s_>().trigger110_ind_r14 and
             c.get<three_bit_r14_s_>().trigger111_ind_r14 == other.c.get<three_bit_r14_s_>().trigger111_ind_r14;
    default:
      return true;
  }
  return true;
}

// CQI-ReportBoth-r11 ::= SEQUENCE
SRSASN_CODE cqi_report_both_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_im_cfg_to_release_list_r11_present, 1));
  HANDLE_CODE(bref.pack(csi_im_cfg_to_add_mod_list_r11_present, 1));
  HANDLE_CODE(bref.pack(csi_process_to_release_list_r11_present, 1));
  HANDLE_CODE(bref.pack(csi_process_to_add_mod_list_r11_present, 1));

  if (csi_im_cfg_to_release_list_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_im_cfg_to_release_list_r11, 1, 3, integer_packer<uint8_t>(1, 3)));
  }
  if (csi_im_cfg_to_add_mod_list_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_im_cfg_to_add_mod_list_r11, 1, 3));
  }
  if (csi_process_to_release_list_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_process_to_release_list_r11, 1, 4, integer_packer<uint8_t>(1, 4)));
  }
  if (csi_process_to_add_mod_list_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_process_to_add_mod_list_r11, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_both_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_im_cfg_to_release_list_r11_present, 1));
  HANDLE_CODE(bref.unpack(csi_im_cfg_to_add_mod_list_r11_present, 1));
  HANDLE_CODE(bref.unpack(csi_process_to_release_list_r11_present, 1));
  HANDLE_CODE(bref.unpack(csi_process_to_add_mod_list_r11_present, 1));

  if (csi_im_cfg_to_release_list_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_im_cfg_to_release_list_r11, bref, 1, 3, integer_packer<uint8_t>(1, 3)));
  }
  if (csi_im_cfg_to_add_mod_list_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_im_cfg_to_add_mod_list_r11, bref, 1, 3));
  }
  if (csi_process_to_release_list_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_process_to_release_list_r11, bref, 1, 4, integer_packer<uint8_t>(1, 4)));
  }
  if (csi_process_to_add_mod_list_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_process_to_add_mod_list_r11, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_both_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_im_cfg_to_release_list_r11_present) {
    j.start_array("csi-IM-ConfigToReleaseList-r11");
    for (const auto& e1 : csi_im_cfg_to_release_list_r11) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (csi_im_cfg_to_add_mod_list_r11_present) {
    j.start_array("csi-IM-ConfigToAddModList-r11");
    for (const auto& e1 : csi_im_cfg_to_add_mod_list_r11) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (csi_process_to_release_list_r11_present) {
    j.start_array("csi-ProcessToReleaseList-r11");
    for (const auto& e1 : csi_process_to_release_list_r11) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (csi_process_to_add_mod_list_r11_present) {
    j.start_array("csi-ProcessToAddModList-r11");
    for (const auto& e1 : csi_process_to_add_mod_list_r11) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
bool cqi_report_both_r11_s::operator==(const cqi_report_both_r11_s& other) const
{
  return csi_im_cfg_to_release_list_r11_present == other.csi_im_cfg_to_release_list_r11_present and
         (not csi_im_cfg_to_release_list_r11_present or
          csi_im_cfg_to_release_list_r11 == other.csi_im_cfg_to_release_list_r11) and
         csi_im_cfg_to_add_mod_list_r11_present == other.csi_im_cfg_to_add_mod_list_r11_present and
         (not csi_im_cfg_to_add_mod_list_r11_present or
          csi_im_cfg_to_add_mod_list_r11 == other.csi_im_cfg_to_add_mod_list_r11) and
         csi_process_to_release_list_r11_present == other.csi_process_to_release_list_r11_present and
         (not csi_process_to_release_list_r11_present or
          csi_process_to_release_list_r11 == other.csi_process_to_release_list_r11) and
         csi_process_to_add_mod_list_r11_present == other.csi_process_to_add_mod_list_r11_present and
         (not csi_process_to_add_mod_list_r11_present or
          csi_process_to_add_mod_list_r11 == other.csi_process_to_add_mod_list_r11);
}

// CQI-ReportBoth-v1250 ::= SEQUENCE
SRSASN_CODE cqi_report_both_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_im_cfg_to_release_list_ext_r12_present, 1));
  HANDLE_CODE(bref.pack(csi_im_cfg_to_add_mod_list_ext_r12_present, 1));

  if (csi_im_cfg_to_release_list_ext_r12_present) {
    HANDLE_CODE(pack_integer(bref, csi_im_cfg_to_release_list_ext_r12, (uint8_t)4u, (uint8_t)4u));
  }
  if (csi_im_cfg_to_add_mod_list_ext_r12_present) {
    HANDLE_CODE(csi_im_cfg_to_add_mod_list_ext_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_both_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_im_cfg_to_release_list_ext_r12_present, 1));
  HANDLE_CODE(bref.unpack(csi_im_cfg_to_add_mod_list_ext_r12_present, 1));

  if (csi_im_cfg_to_release_list_ext_r12_present) {
    HANDLE_CODE(unpack_integer(csi_im_cfg_to_release_list_ext_r12, bref, (uint8_t)4u, (uint8_t)4u));
  }
  if (csi_im_cfg_to_add_mod_list_ext_r12_present) {
    HANDLE_CODE(csi_im_cfg_to_add_mod_list_ext_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_both_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_im_cfg_to_release_list_ext_r12_present) {
    j.write_int("csi-IM-ConfigToReleaseListExt-r12", csi_im_cfg_to_release_list_ext_r12);
  }
  if (csi_im_cfg_to_add_mod_list_ext_r12_present) {
    j.write_fieldname("csi-IM-ConfigToAddModListExt-r12");
    csi_im_cfg_to_add_mod_list_ext_r12.to_json(j);
  }
  j.end_obj();
}
bool cqi_report_both_v1250_s::operator==(const cqi_report_both_v1250_s& other) const
{
  return csi_im_cfg_to_release_list_ext_r12_present == other.csi_im_cfg_to_release_list_ext_r12_present and
         (not csi_im_cfg_to_release_list_ext_r12_present or
          csi_im_cfg_to_release_list_ext_r12 == other.csi_im_cfg_to_release_list_ext_r12) and
         csi_im_cfg_to_add_mod_list_ext_r12_present == other.csi_im_cfg_to_add_mod_list_ext_r12_present and
         (not csi_im_cfg_to_add_mod_list_ext_r12_present or
          csi_im_cfg_to_add_mod_list_ext_r12 == other.csi_im_cfg_to_add_mod_list_ext_r12);
}

// CQI-ReportBoth-v1310 ::= SEQUENCE
SRSASN_CODE cqi_report_both_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_im_cfg_to_release_list_ext_r13_present, 1));
  HANDLE_CODE(bref.pack(csi_im_cfg_to_add_mod_list_ext_r13_present, 1));

  if (csi_im_cfg_to_release_list_ext_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_im_cfg_to_release_list_ext_r13, 1, 20, integer_packer<uint8_t>(5, 24)));
  }
  if (csi_im_cfg_to_add_mod_list_ext_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_im_cfg_to_add_mod_list_ext_r13, 1, 20));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_both_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_im_cfg_to_release_list_ext_r13_present, 1));
  HANDLE_CODE(bref.unpack(csi_im_cfg_to_add_mod_list_ext_r13_present, 1));

  if (csi_im_cfg_to_release_list_ext_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_im_cfg_to_release_list_ext_r13, bref, 1, 20, integer_packer<uint8_t>(5, 24)));
  }
  if (csi_im_cfg_to_add_mod_list_ext_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_im_cfg_to_add_mod_list_ext_r13, bref, 1, 20));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_both_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_im_cfg_to_release_list_ext_r13_present) {
    j.start_array("csi-IM-ConfigToReleaseListExt-r13");
    for (const auto& e1 : csi_im_cfg_to_release_list_ext_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (csi_im_cfg_to_add_mod_list_ext_r13_present) {
    j.start_array("csi-IM-ConfigToAddModListExt-r13");
    for (const auto& e1 : csi_im_cfg_to_add_mod_list_ext_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
bool cqi_report_both_v1310_s::operator==(const cqi_report_both_v1310_s& other) const
{
  return csi_im_cfg_to_release_list_ext_r13_present == other.csi_im_cfg_to_release_list_ext_r13_present and
         (not csi_im_cfg_to_release_list_ext_r13_present or
          csi_im_cfg_to_release_list_ext_r13 == other.csi_im_cfg_to_release_list_ext_r13) and
         csi_im_cfg_to_add_mod_list_ext_r13_present == other.csi_im_cfg_to_add_mod_list_ext_r13_present and
         (not csi_im_cfg_to_add_mod_list_ext_r13_present or
          csi_im_cfg_to_add_mod_list_ext_r13 == other.csi_im_cfg_to_add_mod_list_ext_r13);
}

// CQI-ReportPeriodic-r10 ::= CHOICE
void cqi_report_periodic_r10_c::set(types::options e)
{
  type_ = e;
}
void cqi_report_periodic_r10_c::set_release()
{
  set(types::release);
}
cqi_report_periodic_r10_c::setup_s_& cqi_report_periodic_r10_c::set_setup()
{
  set(types::setup);
  return c;
}
void cqi_report_periodic_r10_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("cqi-PUCCH-ResourceIndex-r10", c.cqi_pucch_res_idx_r10);
      if (c.cqi_pucch_res_idx_p1_r10_present) {
        j.write_int("cqi-PUCCH-ResourceIndexP1-r10", c.cqi_pucch_res_idx_p1_r10);
      }
      j.write_int("cqi-pmi-ConfigIndex", c.cqi_pmi_cfg_idx);
      j.write_fieldname("cqi-FormatIndicatorPeriodic-r10");
      c.cqi_format_ind_periodic_r10.to_json(j);
      if (c.ri_cfg_idx_present) {
        j.write_int("ri-ConfigIndex", c.ri_cfg_idx);
      }
      j.write_bool("simultaneousAckNackAndCQI", c.simul_ack_nack_and_cqi);
      if (c.cqi_mask_r9_present) {
        j.write_str("cqi-Mask-r9", "setup");
      }
      if (c.csi_cfg_idx_r10_present) {
        j.write_fieldname("csi-ConfigIndex-r10");
        c.csi_cfg_idx_r10.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_r10_c");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_periodic_r10_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.cqi_pucch_res_idx_p1_r10_present, 1));
      HANDLE_CODE(bref.pack(c.ri_cfg_idx_present, 1));
      HANDLE_CODE(bref.pack(c.cqi_mask_r9_present, 1));
      HANDLE_CODE(bref.pack(c.csi_cfg_idx_r10_present, 1));
      HANDLE_CODE(pack_integer(bref, c.cqi_pucch_res_idx_r10, (uint16_t)0u, (uint16_t)1184u));
      if (c.cqi_pucch_res_idx_p1_r10_present) {
        HANDLE_CODE(pack_integer(bref, c.cqi_pucch_res_idx_p1_r10, (uint16_t)0u, (uint16_t)1184u));
      }
      HANDLE_CODE(pack_integer(bref, c.cqi_pmi_cfg_idx, (uint16_t)0u, (uint16_t)1023u));
      HANDLE_CODE(c.cqi_format_ind_periodic_r10.pack(bref));
      if (c.ri_cfg_idx_present) {
        HANDLE_CODE(pack_integer(bref, c.ri_cfg_idx, (uint16_t)0u, (uint16_t)1023u));
      }
      HANDLE_CODE(bref.pack(c.simul_ack_nack_and_cqi, 1));
      if (c.csi_cfg_idx_r10_present) {
        HANDLE_CODE(c.csi_cfg_idx_r10.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_r10_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_r10_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.cqi_pucch_res_idx_p1_r10_present, 1));
      HANDLE_CODE(bref.unpack(c.ri_cfg_idx_present, 1));
      HANDLE_CODE(bref.unpack(c.cqi_mask_r9_present, 1));
      HANDLE_CODE(bref.unpack(c.csi_cfg_idx_r10_present, 1));
      HANDLE_CODE(unpack_integer(c.cqi_pucch_res_idx_r10, bref, (uint16_t)0u, (uint16_t)1184u));
      if (c.cqi_pucch_res_idx_p1_r10_present) {
        HANDLE_CODE(unpack_integer(c.cqi_pucch_res_idx_p1_r10, bref, (uint16_t)0u, (uint16_t)1184u));
      }
      HANDLE_CODE(unpack_integer(c.cqi_pmi_cfg_idx, bref, (uint16_t)0u, (uint16_t)1023u));
      HANDLE_CODE(c.cqi_format_ind_periodic_r10.unpack(bref));
      if (c.ri_cfg_idx_present) {
        HANDLE_CODE(unpack_integer(c.ri_cfg_idx, bref, (uint16_t)0u, (uint16_t)1023u));
      }
      HANDLE_CODE(bref.unpack(c.simul_ack_nack_and_cqi, 1));
      if (c.csi_cfg_idx_r10_present) {
        HANDLE_CODE(c.csi_cfg_idx_r10.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_r10_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_periodic_r10_c::operator==(const cqi_report_periodic_r10_c& other) const
{
  return type() == other.type() and c.cqi_pucch_res_idx_r10 == other.c.cqi_pucch_res_idx_r10 and
         c.cqi_pucch_res_idx_p1_r10_present == other.c.cqi_pucch_res_idx_p1_r10_present and
         (not c.cqi_pucch_res_idx_p1_r10_present or c.cqi_pucch_res_idx_p1_r10 == other.c.cqi_pucch_res_idx_p1_r10) and
         c.cqi_pmi_cfg_idx == other.c.cqi_pmi_cfg_idx and
         c.cqi_format_ind_periodic_r10 == other.c.cqi_format_ind_periodic_r10 and
         c.ri_cfg_idx_present == other.c.ri_cfg_idx_present and
         (not c.ri_cfg_idx_present or c.ri_cfg_idx == other.c.ri_cfg_idx) and
         c.simul_ack_nack_and_cqi == other.c.simul_ack_nack_and_cqi and
         c.cqi_mask_r9_present == other.c.cqi_mask_r9_present and
         c.csi_cfg_idx_r10_present == other.c.csi_cfg_idx_r10_present and
         (not c.csi_cfg_idx_r10_present or c.csi_cfg_idx_r10 == other.c.csi_cfg_idx_r10);
}

void cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::destroy_()
{
  switch (type_) {
    case types::wideband_cqi_r10:
      c.destroy<wideband_cqi_r10_s_>();
      break;
    case types::subband_cqi_r10:
      c.destroy<subband_cqi_r10_s_>();
      break;
    default:
      break;
  }
}
void cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::wideband_cqi_r10:
      c.init<wideband_cqi_r10_s_>();
      break;
    case types::subband_cqi_r10:
      c.init<subband_cqi_r10_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_");
  }
}
cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::cqi_format_ind_periodic_r10_c_(
    const cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::wideband_cqi_r10:
      c.init(other.c.get<wideband_cqi_r10_s_>());
      break;
    case types::subband_cqi_r10:
      c.init(other.c.get<subband_cqi_r10_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_");
  }
}
cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_&
cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::operator=(
    const cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::wideband_cqi_r10:
      c.set(other.c.get<wideband_cqi_r10_s_>());
      break;
    case types::subband_cqi_r10:
      c.set(other.c.get<subband_cqi_r10_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_");
  }

  return *this;
}
cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::wideband_cqi_r10_s_&
cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::set_wideband_cqi_r10()
{
  set(types::wideband_cqi_r10);
  return c.get<wideband_cqi_r10_s_>();
}
cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::subband_cqi_r10_s_&
cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::set_subband_cqi_r10()
{
  set(types::subband_cqi_r10);
  return c.get<subband_cqi_r10_s_>();
}
void cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::wideband_cqi_r10:
      j.write_fieldname("widebandCQI-r10");
      j.start_obj();
      if (c.get<wideband_cqi_r10_s_>().csi_report_mode_r10_present) {
        j.write_str("csi-ReportMode-r10", c.get<wideband_cqi_r10_s_>().csi_report_mode_r10.to_string());
      }
      j.end_obj();
      break;
    case types::subband_cqi_r10:
      j.write_fieldname("subbandCQI-r10");
      j.start_obj();
      j.write_int("k", c.get<subband_cqi_r10_s_>().k);
      j.write_str("periodicityFactor-r10", c.get<subband_cqi_r10_s_>().periodicity_factor_r10.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::wideband_cqi_r10:
      HANDLE_CODE(bref.pack(c.get<wideband_cqi_r10_s_>().csi_report_mode_r10_present, 1));
      if (c.get<wideband_cqi_r10_s_>().csi_report_mode_r10_present) {
        HANDLE_CODE(c.get<wideband_cqi_r10_s_>().csi_report_mode_r10.pack(bref));
      }
      break;
    case types::subband_cqi_r10:
      HANDLE_CODE(pack_integer(bref, c.get<subband_cqi_r10_s_>().k, (uint8_t)1u, (uint8_t)4u));
      HANDLE_CODE(c.get<subband_cqi_r10_s_>().periodicity_factor_r10.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::wideband_cqi_r10:
      HANDLE_CODE(bref.unpack(c.get<wideband_cqi_r10_s_>().csi_report_mode_r10_present, 1));
      if (c.get<wideband_cqi_r10_s_>().csi_report_mode_r10_present) {
        HANDLE_CODE(c.get<wideband_cqi_r10_s_>().csi_report_mode_r10.unpack(bref));
      }
      break;
    case types::subband_cqi_r10:
      HANDLE_CODE(unpack_integer(c.get<subband_cqi_r10_s_>().k, bref, (uint8_t)1u, (uint8_t)4u));
      HANDLE_CODE(c.get<subband_cqi_r10_s_>().periodicity_factor_r10.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::operator==(
    const cqi_format_ind_periodic_r10_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::wideband_cqi_r10:
      return c.get<wideband_cqi_r10_s_>().csi_report_mode_r10_present ==
                 other.c.get<wideband_cqi_r10_s_>().csi_report_mode_r10_present and
             (not c.get<wideband_cqi_r10_s_>().csi_report_mode_r10_present or
              c.get<wideband_cqi_r10_s_>().csi_report_mode_r10 ==
                  other.c.get<wideband_cqi_r10_s_>().csi_report_mode_r10);
    case types::subband_cqi_r10:
      return c.get<subband_cqi_r10_s_>().k == other.c.get<subband_cqi_r10_s_>().k and
             c.get<subband_cqi_r10_s_>().periodicity_factor_r10 ==
                 other.c.get<subband_cqi_r10_s_>().periodicity_factor_r10;
    default:
      return true;
  }
  return true;
}

const char* cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::wideband_cqi_r10_s_::
    csi_report_mode_r10_opts::to_string() const
{
  static const char* options[] = {"submode1", "submode2"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::wideband_cqi_r10_s_::"
                          "csi_report_mode_r10_e_");
}
uint8_t cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::wideband_cqi_r10_s_::
    csi_report_mode_r10_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::wideband_cqi_r10_s_::"
                         "csi_report_mode_r10_e_");
}

const char* cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::subband_cqi_r10_s_::
    periodicity_factor_r10_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::subband_cqi_r10_s_::"
                          "periodicity_factor_r10_e_");
}
uint8_t cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::subband_cqi_r10_s_::
    periodicity_factor_r10_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::subband_cqi_r10_s_::"
                         "periodicity_factor_r10_e_");
}

void cqi_report_periodic_r10_c::setup_s_::csi_cfg_idx_r10_c_::set(types::options e)
{
  type_ = e;
}
void cqi_report_periodic_r10_c::setup_s_::csi_cfg_idx_r10_c_::set_release()
{
  set(types::release);
}
cqi_report_periodic_r10_c::setup_s_::csi_cfg_idx_r10_c_::setup_s_&
cqi_report_periodic_r10_c::setup_s_::csi_cfg_idx_r10_c_::set_setup()
{
  set(types::setup);
  return c;
}
void cqi_report_periodic_r10_c::setup_s_::csi_cfg_idx_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("cqi-pmi-ConfigIndex2-r10", c.cqi_pmi_cfg_idx2_r10);
      if (c.ri_cfg_idx2_r10_present) {
        j.write_int("ri-ConfigIndex2-r10", c.ri_cfg_idx2_r10);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_r10_c::setup_s_::csi_cfg_idx_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_periodic_r10_c::setup_s_::csi_cfg_idx_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.ri_cfg_idx2_r10_present, 1));
      HANDLE_CODE(pack_integer(bref, c.cqi_pmi_cfg_idx2_r10, (uint16_t)0u, (uint16_t)1023u));
      if (c.ri_cfg_idx2_r10_present) {
        HANDLE_CODE(pack_integer(bref, c.ri_cfg_idx2_r10, (uint16_t)0u, (uint16_t)1023u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_r10_c::setup_s_::csi_cfg_idx_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_r10_c::setup_s_::csi_cfg_idx_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.ri_cfg_idx2_r10_present, 1));
      HANDLE_CODE(unpack_integer(c.cqi_pmi_cfg_idx2_r10, bref, (uint16_t)0u, (uint16_t)1023u));
      if (c.ri_cfg_idx2_r10_present) {
        HANDLE_CODE(unpack_integer(c.ri_cfg_idx2_r10, bref, (uint16_t)0u, (uint16_t)1023u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_r10_c::setup_s_::csi_cfg_idx_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_periodic_r10_c::setup_s_::csi_cfg_idx_r10_c_::operator==(const csi_cfg_idx_r10_c_& other) const
{
  return type() == other.type() and c.cqi_pmi_cfg_idx2_r10 == other.c.cqi_pmi_cfg_idx2_r10 and
         c.ri_cfg_idx2_r10_present == other.c.ri_cfg_idx2_r10_present and
         (not c.ri_cfg_idx2_r10_present or c.ri_cfg_idx2_r10 == other.c.ri_cfg_idx2_r10);
}

// CQI-ReportPeriodic-v1130 ::= SEQUENCE
SRSASN_CODE cqi_report_periodic_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(simul_ack_nack_and_cqi_format3_r11_present, 1));
  HANDLE_CODE(bref.pack(cqi_report_periodic_proc_ext_to_release_list_r11_present, 1));
  HANDLE_CODE(bref.pack(cqi_report_periodic_proc_ext_to_add_mod_list_r11_present, 1));

  if (cqi_report_periodic_proc_ext_to_release_list_r11_present) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, cqi_report_periodic_proc_ext_to_release_list_r11, 1, 3, integer_packer<uint8_t>(1, 3)));
  }
  if (cqi_report_periodic_proc_ext_to_add_mod_list_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cqi_report_periodic_proc_ext_to_add_mod_list_r11, 1, 3));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(simul_ack_nack_and_cqi_format3_r11_present, 1));
  HANDLE_CODE(bref.unpack(cqi_report_periodic_proc_ext_to_release_list_r11_present, 1));
  HANDLE_CODE(bref.unpack(cqi_report_periodic_proc_ext_to_add_mod_list_r11_present, 1));

  if (cqi_report_periodic_proc_ext_to_release_list_r11_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(cqi_report_periodic_proc_ext_to_release_list_r11, bref, 1, 3, integer_packer<uint8_t>(1, 3)));
  }
  if (cqi_report_periodic_proc_ext_to_add_mod_list_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cqi_report_periodic_proc_ext_to_add_mod_list_r11, bref, 1, 3));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_periodic_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (simul_ack_nack_and_cqi_format3_r11_present) {
    j.write_str("simultaneousAckNackAndCQI-Format3-r11", "setup");
  }
  if (cqi_report_periodic_proc_ext_to_release_list_r11_present) {
    j.start_array("cqi-ReportPeriodicProcExtToReleaseList-r11");
    for (const auto& e1 : cqi_report_periodic_proc_ext_to_release_list_r11) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (cqi_report_periodic_proc_ext_to_add_mod_list_r11_present) {
    j.start_array("cqi-ReportPeriodicProcExtToAddModList-r11");
    for (const auto& e1 : cqi_report_periodic_proc_ext_to_add_mod_list_r11) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
bool cqi_report_periodic_v1130_s::operator==(const cqi_report_periodic_v1130_s& other) const
{
  return simul_ack_nack_and_cqi_format3_r11_present == other.simul_ack_nack_and_cqi_format3_r11_present and
         cqi_report_periodic_proc_ext_to_release_list_r11_present ==
             other.cqi_report_periodic_proc_ext_to_release_list_r11_present and
         (not cqi_report_periodic_proc_ext_to_release_list_r11_present or
          cqi_report_periodic_proc_ext_to_release_list_r11 ==
              other.cqi_report_periodic_proc_ext_to_release_list_r11) and
         cqi_report_periodic_proc_ext_to_add_mod_list_r11_present ==
             other.cqi_report_periodic_proc_ext_to_add_mod_list_r11_present and
         (not cqi_report_periodic_proc_ext_to_add_mod_list_r11_present or
          cqi_report_periodic_proc_ext_to_add_mod_list_r11 == other.cqi_report_periodic_proc_ext_to_add_mod_list_r11);
}

// CQI-ReportPeriodic-v1310 ::= SEQUENCE
SRSASN_CODE cqi_report_periodic_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cri_report_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(simul_ack_nack_and_cqi_format4_format5_r13_present, 1));

  if (cri_report_cfg_r13_present) {
    HANDLE_CODE(cri_report_cfg_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cri_report_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(simul_ack_nack_and_cqi_format4_format5_r13_present, 1));

  if (cri_report_cfg_r13_present) {
    HANDLE_CODE(cri_report_cfg_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_periodic_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cri_report_cfg_r13_present) {
    j.write_fieldname("cri-ReportConfig-r13");
    cri_report_cfg_r13.to_json(j);
  }
  if (simul_ack_nack_and_cqi_format4_format5_r13_present) {
    j.write_str("simultaneousAckNackAndCQI-Format4-Format5-r13", "setup");
  }
  j.end_obj();
}
bool cqi_report_periodic_v1310_s::operator==(const cqi_report_periodic_v1310_s& other) const
{
  return cri_report_cfg_r13_present == other.cri_report_cfg_r13_present and
         (not cri_report_cfg_r13_present or cri_report_cfg_r13 == other.cri_report_cfg_r13) and
         simul_ack_nack_and_cqi_format4_format5_r13_present == other.simul_ack_nack_and_cqi_format4_format5_r13_present;
}

// CQI-ReportPeriodic-v1320 ::= SEQUENCE
SRSASN_CODE cqi_report_periodic_v1320_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(periodicity_factor_wb_r13_present, 1));

  if (periodicity_factor_wb_r13_present) {
    HANDLE_CODE(periodicity_factor_wb_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_v1320_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(periodicity_factor_wb_r13_present, 1));

  if (periodicity_factor_wb_r13_present) {
    HANDLE_CODE(periodicity_factor_wb_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_periodic_v1320_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (periodicity_factor_wb_r13_present) {
    j.write_str("periodicityFactorWB-r13", periodicity_factor_wb_r13.to_string());
  }
  j.end_obj();
}
bool cqi_report_periodic_v1320_s::operator==(const cqi_report_periodic_v1320_s& other) const
{
  return periodicity_factor_wb_r13_present == other.periodicity_factor_wb_r13_present and
         (not periodicity_factor_wb_r13_present or periodicity_factor_wb_r13 == other.periodicity_factor_wb_r13);
}

const char* cqi_report_periodic_v1320_s::periodicity_factor_wb_r13_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "cqi_report_periodic_v1320_s::periodicity_factor_wb_r13_e_");
}
uint8_t cqi_report_periodic_v1320_s::periodicity_factor_wb_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "cqi_report_periodic_v1320_s::periodicity_factor_wb_r13_e_");
}

// SPDCCH-Elements-r15 ::= CHOICE
void spdcch_elems_r15_c::set(types::options e)
{
  type_ = e;
}
void spdcch_elems_r15_c::set_release()
{
  set(types::release);
}
spdcch_elems_r15_c::setup_s_& spdcch_elems_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void spdcch_elems_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.spdcch_set_cfg_id_r15_present) {
        j.write_int("spdcch-SetConfigId-r15", c.spdcch_set_cfg_id_r15);
      }
      if (c.spdcch_set_ref_sig_r15_present) {
        j.write_str("spdcch-SetReferenceSig-r15", c.spdcch_set_ref_sig_r15.to_string());
      }
      if (c.tx_type_r15_present) {
        j.write_str("transmissionType-r15", c.tx_type_r15.to_string());
      }
      if (c.spdcch_no_of_symbols_r15_present) {
        j.write_int("spdcch-NoOfSymbols-r15", c.spdcch_no_of_symbols_r15);
      }
      if (c.dmrs_scrambling_seq_int_r15_present) {
        j.write_int("dmrs-ScramblingSequenceInt-r15", c.dmrs_scrambling_seq_int_r15);
      }
      if (c.dci7_candidates_per_al_pdcch_r15_present) {
        j.start_array("dci7-CandidatesPerAL-PDCCH-r15");
        for (const auto& e1 : c.dci7_candidates_per_al_pdcch_r15) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.dci7_candidate_sets_per_al_spdcch_r15_present) {
        j.start_array("dci7-CandidateSetsPerAL-SPDCCH-r15");
        for (const auto& e1 : c.dci7_candidate_sets_per_al_spdcch_r15) {
          j.start_array();
          for (const auto& e2 : e1) {
            j.write_int(e2);
          }
          j.end_array();
        }
        j.end_array();
      }
      if (c.res_block_assign_r15_present) {
        j.write_fieldname("resourceBlockAssignment-r15");
        j.start_obj();
        j.write_int("numberRB-InFreq-domain-r15", c.res_block_assign_r15.num_rb_in_freq_domain_r15);
        j.write_str("resourceBlockAssignment-r15", c.res_block_assign_r15.res_block_assign_r15.to_string());
        j.end_obj();
      }
      if (c.subslot_applicability_r15_present) {
        j.write_str("subslotApplicability-r15", c.subslot_applicability_r15.to_string());
      }
      if (c.al_start_point_spdcch_r15_present) {
        j.start_array("al-StartingPointSPDCCH-r15");
        for (const auto& e1 : c.al_start_point_spdcch_r15) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.sf_type_r15_present) {
        j.write_str("subframeType-r15", c.sf_type_r15.to_string());
      }
      if (c.rate_matching_mode_r15_present) {
        j.write_str("rateMatchingMode-r15", c.rate_matching_mode_r15.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "spdcch_elems_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE spdcch_elems_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.pack(c.ext, 1);
      HANDLE_CODE(bref.pack(c.spdcch_set_cfg_id_r15_present, 1));
      HANDLE_CODE(bref.pack(c.spdcch_set_ref_sig_r15_present, 1));
      HANDLE_CODE(bref.pack(c.tx_type_r15_present, 1));
      HANDLE_CODE(bref.pack(c.spdcch_no_of_symbols_r15_present, 1));
      HANDLE_CODE(bref.pack(c.dmrs_scrambling_seq_int_r15_present, 1));
      HANDLE_CODE(bref.pack(c.dci7_candidates_per_al_pdcch_r15_present, 1));
      HANDLE_CODE(bref.pack(c.dci7_candidate_sets_per_al_spdcch_r15_present, 1));
      HANDLE_CODE(bref.pack(c.res_block_assign_r15_present, 1));
      HANDLE_CODE(bref.pack(c.subslot_applicability_r15_present, 1));
      HANDLE_CODE(bref.pack(c.al_start_point_spdcch_r15_present, 1));
      HANDLE_CODE(bref.pack(c.sf_type_r15_present, 1));
      HANDLE_CODE(bref.pack(c.rate_matching_mode_r15_present, 1));
      if (c.spdcch_set_cfg_id_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.spdcch_set_cfg_id_r15, (uint8_t)0u, (uint8_t)3u));
      }
      if (c.spdcch_set_ref_sig_r15_present) {
        HANDLE_CODE(c.spdcch_set_ref_sig_r15.pack(bref));
      }
      if (c.tx_type_r15_present) {
        HANDLE_CODE(c.tx_type_r15.pack(bref));
      }
      if (c.spdcch_no_of_symbols_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.spdcch_no_of_symbols_r15, (uint8_t)1u, (uint8_t)2u));
      }
      if (c.dmrs_scrambling_seq_int_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.dmrs_scrambling_seq_int_r15, (uint16_t)0u, (uint16_t)503u));
      }
      if (c.dci7_candidates_per_al_pdcch_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.dci7_candidates_per_al_pdcch_r15, 1, 4, integer_packer<uint8_t>(0, 6)));
      }
      if (c.dci7_candidate_sets_per_al_spdcch_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref,
                                    c.dci7_candidate_sets_per_al_spdcch_r15,
                                    1,
                                    2,
                                    SeqOfPacker<integer_packer<uint8_t> >(1, 4, integer_packer<uint8_t>(0, 6))));
      }
      if (c.res_block_assign_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.res_block_assign_r15.num_rb_in_freq_domain_r15, (uint8_t)2u, (uint8_t)100u));
        HANDLE_CODE(c.res_block_assign_r15.res_block_assign_r15.pack(bref));
      }
      if (c.subslot_applicability_r15_present) {
        HANDLE_CODE(c.subslot_applicability_r15.pack(bref));
      }
      if (c.al_start_point_spdcch_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.al_start_point_spdcch_r15, 1, 4, integer_packer<uint8_t>(0, 49)));
      }
      if (c.sf_type_r15_present) {
        HANDLE_CODE(c.sf_type_r15.pack(bref));
      }
      if (c.rate_matching_mode_r15_present) {
        HANDLE_CODE(c.rate_matching_mode_r15.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "spdcch_elems_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE spdcch_elems_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(bref.unpack(c.spdcch_set_cfg_id_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.spdcch_set_ref_sig_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.tx_type_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.spdcch_no_of_symbols_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.dmrs_scrambling_seq_int_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.dci7_candidates_per_al_pdcch_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.dci7_candidate_sets_per_al_spdcch_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.res_block_assign_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.subslot_applicability_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.al_start_point_spdcch_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.sf_type_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.rate_matching_mode_r15_present, 1));
      if (c.spdcch_set_cfg_id_r15_present) {
        HANDLE_CODE(unpack_integer(c.spdcch_set_cfg_id_r15, bref, (uint8_t)0u, (uint8_t)3u));
      }
      if (c.spdcch_set_ref_sig_r15_present) {
        HANDLE_CODE(c.spdcch_set_ref_sig_r15.unpack(bref));
      }
      if (c.tx_type_r15_present) {
        HANDLE_CODE(c.tx_type_r15.unpack(bref));
      }
      if (c.spdcch_no_of_symbols_r15_present) {
        HANDLE_CODE(unpack_integer(c.spdcch_no_of_symbols_r15, bref, (uint8_t)1u, (uint8_t)2u));
      }
      if (c.dmrs_scrambling_seq_int_r15_present) {
        HANDLE_CODE(unpack_integer(c.dmrs_scrambling_seq_int_r15, bref, (uint16_t)0u, (uint16_t)503u));
      }
      if (c.dci7_candidates_per_al_pdcch_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.dci7_candidates_per_al_pdcch_r15, bref, 1, 4, integer_packer<uint8_t>(0, 6)));
      }
      if (c.dci7_candidate_sets_per_al_spdcch_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.dci7_candidate_sets_per_al_spdcch_r15,
                                      bref,
                                      1,
                                      2,
                                      SeqOfPacker<integer_packer<uint8_t> >(1, 4, integer_packer<uint8_t>(0, 6))));
      }
      if (c.res_block_assign_r15_present) {
        HANDLE_CODE(unpack_integer(c.res_block_assign_r15.num_rb_in_freq_domain_r15, bref, (uint8_t)2u, (uint8_t)100u));
        HANDLE_CODE(c.res_block_assign_r15.res_block_assign_r15.unpack(bref));
      }
      if (c.subslot_applicability_r15_present) {
        HANDLE_CODE(c.subslot_applicability_r15.unpack(bref));
      }
      if (c.al_start_point_spdcch_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.al_start_point_spdcch_r15, bref, 1, 4, integer_packer<uint8_t>(0, 49)));
      }
      if (c.sf_type_r15_present) {
        HANDLE_CODE(c.sf_type_r15.unpack(bref));
      }
      if (c.rate_matching_mode_r15_present) {
        HANDLE_CODE(c.rate_matching_mode_r15.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "spdcch_elems_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool spdcch_elems_r15_c::operator==(const spdcch_elems_r15_c& other) const
{
  return type() == other.type() and c.ext == other.c.ext and
         c.spdcch_set_cfg_id_r15_present == other.c.spdcch_set_cfg_id_r15_present and
         (not c.spdcch_set_cfg_id_r15_present or c.spdcch_set_cfg_id_r15 == other.c.spdcch_set_cfg_id_r15) and
         c.spdcch_set_ref_sig_r15_present == other.c.spdcch_set_ref_sig_r15_present and
         (not c.spdcch_set_ref_sig_r15_present or c.spdcch_set_ref_sig_r15 == other.c.spdcch_set_ref_sig_r15) and
         c.tx_type_r15_present == other.c.tx_type_r15_present and
         (not c.tx_type_r15_present or c.tx_type_r15 == other.c.tx_type_r15) and
         c.spdcch_no_of_symbols_r15_present == other.c.spdcch_no_of_symbols_r15_present and
         (not c.spdcch_no_of_symbols_r15_present or c.spdcch_no_of_symbols_r15 == other.c.spdcch_no_of_symbols_r15) and
         c.dmrs_scrambling_seq_int_r15_present == other.c.dmrs_scrambling_seq_int_r15_present and
         (not c.dmrs_scrambling_seq_int_r15_present or
          c.dmrs_scrambling_seq_int_r15 == other.c.dmrs_scrambling_seq_int_r15) and
         c.dci7_candidates_per_al_pdcch_r15_present == other.c.dci7_candidates_per_al_pdcch_r15_present and
         (not c.dci7_candidates_per_al_pdcch_r15_present or
          c.dci7_candidates_per_al_pdcch_r15 == other.c.dci7_candidates_per_al_pdcch_r15) and
         c.dci7_candidate_sets_per_al_spdcch_r15_present == other.c.dci7_candidate_sets_per_al_spdcch_r15_present and
         (not c.dci7_candidate_sets_per_al_spdcch_r15_present or
          c.dci7_candidate_sets_per_al_spdcch_r15 == other.c.dci7_candidate_sets_per_al_spdcch_r15) and
         c.res_block_assign_r15.num_rb_in_freq_domain_r15 == other.c.res_block_assign_r15.num_rb_in_freq_domain_r15 and
         c.res_block_assign_r15.res_block_assign_r15 == other.c.res_block_assign_r15.res_block_assign_r15 and
         c.subslot_applicability_r15_present == other.c.subslot_applicability_r15_present and
         (not c.subslot_applicability_r15_present or
          c.subslot_applicability_r15 == other.c.subslot_applicability_r15) and
         c.al_start_point_spdcch_r15_present == other.c.al_start_point_spdcch_r15_present and
         (not c.al_start_point_spdcch_r15_present or
          c.al_start_point_spdcch_r15 == other.c.al_start_point_spdcch_r15) and
         c.sf_type_r15_present == other.c.sf_type_r15_present and
         (not c.sf_type_r15_present or c.sf_type_r15 == other.c.sf_type_r15) and
         c.rate_matching_mode_r15_present == other.c.rate_matching_mode_r15_present and
         (not c.rate_matching_mode_r15_present or c.rate_matching_mode_r15 == other.c.rate_matching_mode_r15);
}

const char* spdcch_elems_r15_c::setup_s_::spdcch_set_ref_sig_r15_opts::to_string() const
{
  static const char* options[] = {"crs", "dmrs"};
  return convert_enum_idx(options, 2, value, "spdcch_elems_r15_c::setup_s_::spdcch_set_ref_sig_r15_e_");
}

const char* spdcch_elems_r15_c::setup_s_::tx_type_r15_opts::to_string() const
{
  static const char* options[] = {"localised", "distributed"};
  return convert_enum_idx(options, 2, value, "spdcch_elems_r15_c::setup_s_::tx_type_r15_e_");
}

const char* spdcch_elems_r15_c::setup_s_::sf_type_r15_opts::to_string() const
{
  static const char* options[] = {"mbsfn", "nonmbsfn", "all"};
  return convert_enum_idx(options, 3, value, "spdcch_elems_r15_c::setup_s_::sf_type_r15_e_");
}

const char* spdcch_elems_r15_c::setup_s_::rate_matching_mode_r15_opts::to_string() const
{
  static const char* options[] = {"m1", "m2", "m3", "m4"};
  return convert_enum_idx(options, 4, value, "spdcch_elems_r15_c::setup_s_::rate_matching_mode_r15_e_");
}
uint8_t spdcch_elems_r15_c::setup_s_::rate_matching_mode_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4};
  return map_enum_number(options, 4, value, "spdcch_elems_r15_c::setup_s_::rate_matching_mode_r15_e_");
}

// SPUCCH-Elements-r15 ::= CHOICE
void spucch_elems_r15_c::set(types::options e)
{
  type_ = e;
}
void spucch_elems_r15_c::set_release()
{
  set(types::release);
}
spucch_elems_r15_c::setup_s_& spucch_elems_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void spucch_elems_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.n1_subslot_spucch_an_list_r15_present) {
        j.start_array("n1SubslotSPUCCH-AN-List-r15");
        for (const auto& e1 : c.n1_subslot_spucch_an_list_r15) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.n1_slot_spucch_fh_an_list_r15_present) {
        j.write_int("n1SlotSPUCCH-FH-AN-List-r15", c.n1_slot_spucch_fh_an_list_r15);
      }
      if (c.n1_slot_spucch_no_fh_an_list_r15_present) {
        j.write_int("n1SlotSPUCCH-NoFH-AN-List-r15", c.n1_slot_spucch_no_fh_an_list_r15);
      }
      if (c.n3_spucch_an_list_r15_present) {
        j.write_int("n3SPUCCH-AN-List-r15", c.n3_spucch_an_list_r15);
      }
      if (c.n4_spucch_slot_res_r15_present) {
        j.start_array("n4SPUCCHSlot-Resource-r15");
        for (const auto& e1 : c.n4_spucch_slot_res_r15) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (c.n4_spucch_subslot_res_r15_present) {
        j.start_array("n4SPUCCHSubslot-Resource-r15");
        for (const auto& e1 : c.n4_spucch_subslot_res_r15) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (c.n4max_coderate_slot_pucch_r15_present) {
        j.write_int("n4maxCoderateSlotPUCCH-r15", c.n4max_coderate_slot_pucch_r15);
      }
      if (c.n4max_coderate_subslot_pucch_r15_present) {
        j.write_int("n4maxCoderateSubslotPUCCH-r15", c.n4max_coderate_subslot_pucch_r15);
      }
      if (c.n4max_coderate_multi_res_slot_pucch_r15_present) {
        j.write_int("n4maxCoderateMultiResourceSlotPUCCH-r15", c.n4max_coderate_multi_res_slot_pucch_r15);
      }
      if (c.n4max_coderate_multi_res_subslot_pucch_r15_present) {
        j.write_int("n4maxCoderateMultiResourceSubslotPUCCH-r15", c.n4max_coderate_multi_res_subslot_pucch_r15);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "spucch_elems_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE spucch_elems_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.n1_subslot_spucch_an_list_r15_present, 1));
      HANDLE_CODE(bref.pack(c.n1_slot_spucch_fh_an_list_r15_present, 1));
      HANDLE_CODE(bref.pack(c.n1_slot_spucch_no_fh_an_list_r15_present, 1));
      HANDLE_CODE(bref.pack(c.n3_spucch_an_list_r15_present, 1));
      HANDLE_CODE(bref.pack(c.n4_spucch_slot_res_r15_present, 1));
      HANDLE_CODE(bref.pack(c.n4_spucch_subslot_res_r15_present, 1));
      HANDLE_CODE(bref.pack(c.n4max_coderate_slot_pucch_r15_present, 1));
      HANDLE_CODE(bref.pack(c.n4max_coderate_subslot_pucch_r15_present, 1));
      HANDLE_CODE(bref.pack(c.n4max_coderate_multi_res_slot_pucch_r15_present, 1));
      HANDLE_CODE(bref.pack(c.n4max_coderate_multi_res_subslot_pucch_r15_present, 1));
      if (c.n1_subslot_spucch_an_list_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.n1_subslot_spucch_an_list_r15, 1, 4, integer_packer<uint16_t>(0, 1319)));
      }
      if (c.n1_slot_spucch_fh_an_list_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.n1_slot_spucch_fh_an_list_r15, (uint16_t)0u, (uint16_t)1319u));
      }
      if (c.n1_slot_spucch_no_fh_an_list_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.n1_slot_spucch_no_fh_an_list_r15, (uint16_t)0u, (uint16_t)3959u));
      }
      if (c.n3_spucch_an_list_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.n3_spucch_an_list_r15, (uint16_t)0u, (uint16_t)549u));
      }
      if (c.n4_spucch_slot_res_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.n4_spucch_slot_res_r15, 1, 2));
      }
      if (c.n4_spucch_subslot_res_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.n4_spucch_subslot_res_r15, 1, 2));
      }
      if (c.n4max_coderate_slot_pucch_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.n4max_coderate_slot_pucch_r15, (uint8_t)0u, (uint8_t)7u));
      }
      if (c.n4max_coderate_subslot_pucch_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.n4max_coderate_subslot_pucch_r15, (uint8_t)0u, (uint8_t)7u));
      }
      if (c.n4max_coderate_multi_res_slot_pucch_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.n4max_coderate_multi_res_slot_pucch_r15, (uint8_t)0u, (uint8_t)7u));
      }
      if (c.n4max_coderate_multi_res_subslot_pucch_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.n4max_coderate_multi_res_subslot_pucch_r15, (uint8_t)0u, (uint8_t)7u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "spucch_elems_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE spucch_elems_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.n1_subslot_spucch_an_list_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.n1_slot_spucch_fh_an_list_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.n1_slot_spucch_no_fh_an_list_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.n3_spucch_an_list_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.n4_spucch_slot_res_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.n4_spucch_subslot_res_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.n4max_coderate_slot_pucch_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.n4max_coderate_subslot_pucch_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.n4max_coderate_multi_res_slot_pucch_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.n4max_coderate_multi_res_subslot_pucch_r15_present, 1));
      if (c.n1_subslot_spucch_an_list_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.n1_subslot_spucch_an_list_r15, bref, 1, 4, integer_packer<uint16_t>(0, 1319)));
      }
      if (c.n1_slot_spucch_fh_an_list_r15_present) {
        HANDLE_CODE(unpack_integer(c.n1_slot_spucch_fh_an_list_r15, bref, (uint16_t)0u, (uint16_t)1319u));
      }
      if (c.n1_slot_spucch_no_fh_an_list_r15_present) {
        HANDLE_CODE(unpack_integer(c.n1_slot_spucch_no_fh_an_list_r15, bref, (uint16_t)0u, (uint16_t)3959u));
      }
      if (c.n3_spucch_an_list_r15_present) {
        HANDLE_CODE(unpack_integer(c.n3_spucch_an_list_r15, bref, (uint16_t)0u, (uint16_t)549u));
      }
      if (c.n4_spucch_slot_res_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.n4_spucch_slot_res_r15, bref, 1, 2));
      }
      if (c.n4_spucch_subslot_res_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.n4_spucch_subslot_res_r15, bref, 1, 2));
      }
      if (c.n4max_coderate_slot_pucch_r15_present) {
        HANDLE_CODE(unpack_integer(c.n4max_coderate_slot_pucch_r15, bref, (uint8_t)0u, (uint8_t)7u));
      }
      if (c.n4max_coderate_subslot_pucch_r15_present) {
        HANDLE_CODE(unpack_integer(c.n4max_coderate_subslot_pucch_r15, bref, (uint8_t)0u, (uint8_t)7u));
      }
      if (c.n4max_coderate_multi_res_slot_pucch_r15_present) {
        HANDLE_CODE(unpack_integer(c.n4max_coderate_multi_res_slot_pucch_r15, bref, (uint8_t)0u, (uint8_t)7u));
      }
      if (c.n4max_coderate_multi_res_subslot_pucch_r15_present) {
        HANDLE_CODE(unpack_integer(c.n4max_coderate_multi_res_subslot_pucch_r15, bref, (uint8_t)0u, (uint8_t)7u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "spucch_elems_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool spucch_elems_r15_c::operator==(const spucch_elems_r15_c& other) const
{
  return type() == other.type() and
         c.n1_subslot_spucch_an_list_r15_present == other.c.n1_subslot_spucch_an_list_r15_present and
         (not c.n1_subslot_spucch_an_list_r15_present or
          c.n1_subslot_spucch_an_list_r15 == other.c.n1_subslot_spucch_an_list_r15) and
         c.n1_slot_spucch_fh_an_list_r15_present == other.c.n1_slot_spucch_fh_an_list_r15_present and
         (not c.n1_slot_spucch_fh_an_list_r15_present or
          c.n1_slot_spucch_fh_an_list_r15 == other.c.n1_slot_spucch_fh_an_list_r15) and
         c.n1_slot_spucch_no_fh_an_list_r15_present == other.c.n1_slot_spucch_no_fh_an_list_r15_present and
         (not c.n1_slot_spucch_no_fh_an_list_r15_present or
          c.n1_slot_spucch_no_fh_an_list_r15 == other.c.n1_slot_spucch_no_fh_an_list_r15) and
         c.n3_spucch_an_list_r15_present == other.c.n3_spucch_an_list_r15_present and
         (not c.n3_spucch_an_list_r15_present or c.n3_spucch_an_list_r15 == other.c.n3_spucch_an_list_r15) and
         c.n4_spucch_slot_res_r15_present == other.c.n4_spucch_slot_res_r15_present and
         (not c.n4_spucch_slot_res_r15_present or c.n4_spucch_slot_res_r15 == other.c.n4_spucch_slot_res_r15) and
         c.n4_spucch_subslot_res_r15_present == other.c.n4_spucch_subslot_res_r15_present and
         (not c.n4_spucch_subslot_res_r15_present or
          c.n4_spucch_subslot_res_r15 == other.c.n4_spucch_subslot_res_r15) and
         c.n4max_coderate_slot_pucch_r15_present == other.c.n4max_coderate_slot_pucch_r15_present and
         (not c.n4max_coderate_slot_pucch_r15_present or
          c.n4max_coderate_slot_pucch_r15 == other.c.n4max_coderate_slot_pucch_r15) and
         c.n4max_coderate_subslot_pucch_r15_present == other.c.n4max_coderate_subslot_pucch_r15_present and
         (not c.n4max_coderate_subslot_pucch_r15_present or
          c.n4max_coderate_subslot_pucch_r15 == other.c.n4max_coderate_subslot_pucch_r15) and
         c.n4max_coderate_multi_res_slot_pucch_r15_present ==
             other.c.n4max_coderate_multi_res_slot_pucch_r15_present and
         (not c.n4max_coderate_multi_res_slot_pucch_r15_present or
          c.n4max_coderate_multi_res_slot_pucch_r15 == other.c.n4max_coderate_multi_res_slot_pucch_r15) and
         c.n4max_coderate_multi_res_subslot_pucch_r15_present ==
             other.c.n4max_coderate_multi_res_subslot_pucch_r15_present and
         (not c.n4max_coderate_multi_res_subslot_pucch_r15_present or
          c.n4max_coderate_multi_res_subslot_pucch_r15 == other.c.n4max_coderate_multi_res_subslot_pucch_r15);
}

// TPC-Index ::= CHOICE
void tpc_idx_c::destroy_() {}
void tpc_idx_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
tpc_idx_c::tpc_idx_c(const tpc_idx_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::idx_of_format3:
      c.init(other.c.get<uint8_t>());
      break;
    case types::idx_of_format3_a:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tpc_idx_c");
  }
}
tpc_idx_c& tpc_idx_c::operator=(const tpc_idx_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::idx_of_format3:
      c.set(other.c.get<uint8_t>());
      break;
    case types::idx_of_format3_a:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tpc_idx_c");
  }

  return *this;
}
uint8_t& tpc_idx_c::set_idx_of_format3()
{
  set(types::idx_of_format3);
  return c.get<uint8_t>();
}
uint8_t& tpc_idx_c::set_idx_of_format3_a()
{
  set(types::idx_of_format3_a);
  return c.get<uint8_t>();
}
void tpc_idx_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::idx_of_format3:
      j.write_int("indexOfFormat3", c.get<uint8_t>());
      break;
    case types::idx_of_format3_a:
      j.write_int("indexOfFormat3A", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "tpc_idx_c");
  }
  j.end_obj();
}
SRSASN_CODE tpc_idx_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::idx_of_format3:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)15u));
      break;
    case types::idx_of_format3_a:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)31u));
      break;
    default:
      log_invalid_choice_id(type_, "tpc_idx_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE tpc_idx_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::idx_of_format3:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)15u));
      break;
    case types::idx_of_format3_a:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)31u));
      break;
    default:
      log_invalid_choice_id(type_, "tpc_idx_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool tpc_idx_c::operator==(const tpc_idx_c& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::idx_of_format3:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::idx_of_format3_a:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    default:
      return true;
  }
  return true;
}

// ZeroTxPowerCSI-RS-Conf-r12 ::= CHOICE
void zero_tx_pwr_csi_rs_conf_r12_c::set(types::options e)
{
  type_ = e;
}
void zero_tx_pwr_csi_rs_conf_r12_c::set_release()
{
  set(types::release);
}
zero_tx_pwr_csi_rs_r12_s& zero_tx_pwr_csi_rs_conf_r12_c::set_setup()
{
  set(types::setup);
  return c;
}
void zero_tx_pwr_csi_rs_conf_r12_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "zero_tx_pwr_csi_rs_conf_r12_c");
  }
  j.end_obj();
}
SRSASN_CODE zero_tx_pwr_csi_rs_conf_r12_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "zero_tx_pwr_csi_rs_conf_r12_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE zero_tx_pwr_csi_rs_conf_r12_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "zero_tx_pwr_csi_rs_conf_r12_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool zero_tx_pwr_csi_rs_conf_r12_c::operator==(const zero_tx_pwr_csi_rs_conf_r12_c& other) const
{
  return type() == other.type() and c == other.c;
}

// CQI-ReportConfig-r10 ::= SEQUENCE
SRSASN_CODE cqi_report_cfg_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cqi_report_aperiodic_r10_present, 1));
  HANDLE_CODE(bref.pack(cqi_report_periodic_r10_present, 1));
  HANDLE_CODE(bref.pack(pmi_ri_report_r9_present, 1));
  HANDLE_CODE(bref.pack(csi_sf_pattern_cfg_r10_present, 1));

  if (cqi_report_aperiodic_r10_present) {
    HANDLE_CODE(cqi_report_aperiodic_r10.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, nom_pdsch_rs_epre_offset, (int8_t)-1, (int8_t)6));
  if (cqi_report_periodic_r10_present) {
    HANDLE_CODE(cqi_report_periodic_r10.pack(bref));
  }
  if (csi_sf_pattern_cfg_r10_present) {
    HANDLE_CODE(csi_sf_pattern_cfg_r10.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_cfg_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cqi_report_aperiodic_r10_present, 1));
  HANDLE_CODE(bref.unpack(cqi_report_periodic_r10_present, 1));
  HANDLE_CODE(bref.unpack(pmi_ri_report_r9_present, 1));
  HANDLE_CODE(bref.unpack(csi_sf_pattern_cfg_r10_present, 1));

  if (cqi_report_aperiodic_r10_present) {
    HANDLE_CODE(cqi_report_aperiodic_r10.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(nom_pdsch_rs_epre_offset, bref, (int8_t)-1, (int8_t)6));
  if (cqi_report_periodic_r10_present) {
    HANDLE_CODE(cqi_report_periodic_r10.unpack(bref));
  }
  if (csi_sf_pattern_cfg_r10_present) {
    HANDLE_CODE(csi_sf_pattern_cfg_r10.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_cfg_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cqi_report_aperiodic_r10_present) {
    j.write_fieldname("cqi-ReportAperiodic-r10");
    cqi_report_aperiodic_r10.to_json(j);
  }
  j.write_int("nomPDSCH-RS-EPRE-Offset", nom_pdsch_rs_epre_offset);
  if (cqi_report_periodic_r10_present) {
    j.write_fieldname("cqi-ReportPeriodic-r10");
    cqi_report_periodic_r10.to_json(j);
  }
  if (pmi_ri_report_r9_present) {
    j.write_str("pmi-RI-Report-r9", "setup");
  }
  if (csi_sf_pattern_cfg_r10_present) {
    j.write_fieldname("csi-SubframePatternConfig-r10");
    csi_sf_pattern_cfg_r10.to_json(j);
  }
  j.end_obj();
}
bool cqi_report_cfg_r10_s::operator==(const cqi_report_cfg_r10_s& other) const
{
  return cqi_report_aperiodic_r10_present == other.cqi_report_aperiodic_r10_present and
         (not cqi_report_aperiodic_r10_present or cqi_report_aperiodic_r10 == other.cqi_report_aperiodic_r10) and
         nom_pdsch_rs_epre_offset == other.nom_pdsch_rs_epre_offset and
         cqi_report_periodic_r10_present == other.cqi_report_periodic_r10_present and
         (not cqi_report_periodic_r10_present or cqi_report_periodic_r10 == other.cqi_report_periodic_r10) and
         pmi_ri_report_r9_present == other.pmi_ri_report_r9_present and
         csi_sf_pattern_cfg_r10_present == other.csi_sf_pattern_cfg_r10_present and
         (not csi_sf_pattern_cfg_r10_present or csi_sf_pattern_cfg_r10 == other.csi_sf_pattern_cfg_r10);
}

void cqi_report_cfg_r10_s::csi_sf_pattern_cfg_r10_c_::set(types::options e)
{
  type_ = e;
}
void cqi_report_cfg_r10_s::csi_sf_pattern_cfg_r10_c_::set_release()
{
  set(types::release);
}
cqi_report_cfg_r10_s::csi_sf_pattern_cfg_r10_c_::setup_s_& cqi_report_cfg_r10_s::csi_sf_pattern_cfg_r10_c_::set_setup()
{
  set(types::setup);
  return c;
}
void cqi_report_cfg_r10_s::csi_sf_pattern_cfg_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("csi-MeasSubframeSet1-r10");
      c.csi_meas_sf_set1_r10.to_json(j);
      j.write_fieldname("csi-MeasSubframeSet2-r10");
      c.csi_meas_sf_set2_r10.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_cfg_r10_s::csi_sf_pattern_cfg_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_cfg_r10_s::csi_sf_pattern_cfg_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.csi_meas_sf_set1_r10.pack(bref));
      HANDLE_CODE(c.csi_meas_sf_set2_r10.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_cfg_r10_s::csi_sf_pattern_cfg_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_cfg_r10_s::csi_sf_pattern_cfg_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.csi_meas_sf_set1_r10.unpack(bref));
      HANDLE_CODE(c.csi_meas_sf_set2_r10.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_cfg_r10_s::csi_sf_pattern_cfg_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_cfg_r10_s::csi_sf_pattern_cfg_r10_c_::operator==(const csi_sf_pattern_cfg_r10_c_& other) const
{
  return type() == other.type() and c.csi_meas_sf_set1_r10 == other.c.csi_meas_sf_set1_r10 and
         c.csi_meas_sf_set2_r10 == other.c.csi_meas_sf_set2_r10;
}

// CQI-ReportConfig-v1130 ::= SEQUENCE
SRSASN_CODE cqi_report_cfg_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cqi_report_periodic_v1130.pack(bref));
  HANDLE_CODE(cqi_report_both_r11.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_cfg_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cqi_report_periodic_v1130.unpack(bref));
  HANDLE_CODE(cqi_report_both_r11.unpack(bref));

  return SRSASN_SUCCESS;
}
void cqi_report_cfg_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cqi-ReportPeriodic-v1130");
  cqi_report_periodic_v1130.to_json(j);
  j.write_fieldname("cqi-ReportBoth-r11");
  cqi_report_both_r11.to_json(j);
  j.end_obj();
}
bool cqi_report_cfg_v1130_s::operator==(const cqi_report_cfg_v1130_s& other) const
{
  return cqi_report_periodic_v1130 == other.cqi_report_periodic_v1130 and
         cqi_report_both_r11 == other.cqi_report_both_r11;
}

// CQI-ReportConfig-v1250 ::= SEQUENCE
SRSASN_CODE cqi_report_cfg_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_sf_pattern_cfg_r12_present, 1));
  HANDLE_CODE(bref.pack(cqi_report_both_v1250_present, 1));
  HANDLE_CODE(bref.pack(cqi_report_aperiodic_v1250_present, 1));
  HANDLE_CODE(bref.pack(alt_cqi_table_r12_present, 1));

  if (csi_sf_pattern_cfg_r12_present) {
    HANDLE_CODE(csi_sf_pattern_cfg_r12.pack(bref));
  }
  if (cqi_report_both_v1250_present) {
    HANDLE_CODE(cqi_report_both_v1250.pack(bref));
  }
  if (cqi_report_aperiodic_v1250_present) {
    HANDLE_CODE(cqi_report_aperiodic_v1250.pack(bref));
  }
  if (alt_cqi_table_r12_present) {
    HANDLE_CODE(alt_cqi_table_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_cfg_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_sf_pattern_cfg_r12_present, 1));
  HANDLE_CODE(bref.unpack(cqi_report_both_v1250_present, 1));
  HANDLE_CODE(bref.unpack(cqi_report_aperiodic_v1250_present, 1));
  HANDLE_CODE(bref.unpack(alt_cqi_table_r12_present, 1));

  if (csi_sf_pattern_cfg_r12_present) {
    HANDLE_CODE(csi_sf_pattern_cfg_r12.unpack(bref));
  }
  if (cqi_report_both_v1250_present) {
    HANDLE_CODE(cqi_report_both_v1250.unpack(bref));
  }
  if (cqi_report_aperiodic_v1250_present) {
    HANDLE_CODE(cqi_report_aperiodic_v1250.unpack(bref));
  }
  if (alt_cqi_table_r12_present) {
    HANDLE_CODE(alt_cqi_table_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_cfg_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_sf_pattern_cfg_r12_present) {
    j.write_fieldname("csi-SubframePatternConfig-r12");
    csi_sf_pattern_cfg_r12.to_json(j);
  }
  if (cqi_report_both_v1250_present) {
    j.write_fieldname("cqi-ReportBoth-v1250");
    cqi_report_both_v1250.to_json(j);
  }
  if (cqi_report_aperiodic_v1250_present) {
    j.write_fieldname("cqi-ReportAperiodic-v1250");
    cqi_report_aperiodic_v1250.to_json(j);
  }
  if (alt_cqi_table_r12_present) {
    j.write_str("altCQI-Table-r12", alt_cqi_table_r12.to_string());
  }
  j.end_obj();
}
bool cqi_report_cfg_v1250_s::operator==(const cqi_report_cfg_v1250_s& other) const
{
  return csi_sf_pattern_cfg_r12_present == other.csi_sf_pattern_cfg_r12_present and
         (not csi_sf_pattern_cfg_r12_present or csi_sf_pattern_cfg_r12 == other.csi_sf_pattern_cfg_r12) and
         cqi_report_both_v1250_present == other.cqi_report_both_v1250_present and
         (not cqi_report_both_v1250_present or cqi_report_both_v1250 == other.cqi_report_both_v1250) and
         cqi_report_aperiodic_v1250_present == other.cqi_report_aperiodic_v1250_present and
         (not cqi_report_aperiodic_v1250_present or cqi_report_aperiodic_v1250 == other.cqi_report_aperiodic_v1250) and
         alt_cqi_table_r12_present == other.alt_cqi_table_r12_present and
         (not alt_cqi_table_r12_present or alt_cqi_table_r12 == other.alt_cqi_table_r12);
}

void cqi_report_cfg_v1250_s::csi_sf_pattern_cfg_r12_c_::set(types::options e)
{
  type_ = e;
}
void cqi_report_cfg_v1250_s::csi_sf_pattern_cfg_r12_c_::set_release()
{
  set(types::release);
}
cqi_report_cfg_v1250_s::csi_sf_pattern_cfg_r12_c_::setup_s_&
cqi_report_cfg_v1250_s::csi_sf_pattern_cfg_r12_c_::set_setup()
{
  set(types::setup);
  return c;
}
void cqi_report_cfg_v1250_s::csi_sf_pattern_cfg_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("csi-MeasSubframeSets-r12", c.csi_meas_sf_sets_r12.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_cfg_v1250_s::csi_sf_pattern_cfg_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_cfg_v1250_s::csi_sf_pattern_cfg_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.csi_meas_sf_sets_r12.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_cfg_v1250_s::csi_sf_pattern_cfg_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_cfg_v1250_s::csi_sf_pattern_cfg_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.csi_meas_sf_sets_r12.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_cfg_v1250_s::csi_sf_pattern_cfg_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_cfg_v1250_s::csi_sf_pattern_cfg_r12_c_::operator==(const csi_sf_pattern_cfg_r12_c_& other) const
{
  return type() == other.type() and c.csi_meas_sf_sets_r12 == other.c.csi_meas_sf_sets_r12;
}

const char* cqi_report_cfg_v1250_s::alt_cqi_table_r12_opts::to_string() const
{
  static const char* options[] = {"allSubframes", "csi-SubframeSet1", "csi-SubframeSet2", "spare1"};
  return convert_enum_idx(options, 4, value, "cqi_report_cfg_v1250_s::alt_cqi_table_r12_e_");
}
uint8_t cqi_report_cfg_v1250_s::alt_cqi_table_r12_opts::to_number() const
{
  switch (value) {
    case csi_sf_set1:
      return 1;
    case csi_sf_set2:
      return 2;
    default:
      invalid_enum_number(value, "cqi_report_cfg_v1250_s::alt_cqi_table_r12_e_");
  }
  return 0;
}

// CQI-ReportConfig-v1310 ::= SEQUENCE
SRSASN_CODE cqi_report_cfg_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cqi_report_both_v1310_present, 1));
  HANDLE_CODE(bref.pack(cqi_report_aperiodic_v1310_present, 1));
  HANDLE_CODE(bref.pack(cqi_report_periodic_v1310_present, 1));

  if (cqi_report_both_v1310_present) {
    HANDLE_CODE(cqi_report_both_v1310.pack(bref));
  }
  if (cqi_report_aperiodic_v1310_present) {
    HANDLE_CODE(cqi_report_aperiodic_v1310.pack(bref));
  }
  if (cqi_report_periodic_v1310_present) {
    HANDLE_CODE(cqi_report_periodic_v1310.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_cfg_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cqi_report_both_v1310_present, 1));
  HANDLE_CODE(bref.unpack(cqi_report_aperiodic_v1310_present, 1));
  HANDLE_CODE(bref.unpack(cqi_report_periodic_v1310_present, 1));

  if (cqi_report_both_v1310_present) {
    HANDLE_CODE(cqi_report_both_v1310.unpack(bref));
  }
  if (cqi_report_aperiodic_v1310_present) {
    HANDLE_CODE(cqi_report_aperiodic_v1310.unpack(bref));
  }
  if (cqi_report_periodic_v1310_present) {
    HANDLE_CODE(cqi_report_periodic_v1310.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_cfg_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cqi_report_both_v1310_present) {
    j.write_fieldname("cqi-ReportBoth-v1310");
    cqi_report_both_v1310.to_json(j);
  }
  if (cqi_report_aperiodic_v1310_present) {
    j.write_fieldname("cqi-ReportAperiodic-v1310");
    cqi_report_aperiodic_v1310.to_json(j);
  }
  if (cqi_report_periodic_v1310_present) {
    j.write_fieldname("cqi-ReportPeriodic-v1310");
    cqi_report_periodic_v1310.to_json(j);
  }
  j.end_obj();
}
bool cqi_report_cfg_v1310_s::operator==(const cqi_report_cfg_v1310_s& other) const
{
  return cqi_report_both_v1310_present == other.cqi_report_both_v1310_present and
         (not cqi_report_both_v1310_present or cqi_report_both_v1310 == other.cqi_report_both_v1310) and
         cqi_report_aperiodic_v1310_present == other.cqi_report_aperiodic_v1310_present and
         (not cqi_report_aperiodic_v1310_present or cqi_report_aperiodic_v1310 == other.cqi_report_aperiodic_v1310) and
         cqi_report_periodic_v1310_present == other.cqi_report_periodic_v1310_present and
         (not cqi_report_periodic_v1310_present or cqi_report_periodic_v1310 == other.cqi_report_periodic_v1310);
}

// CQI-ReportConfig-v1320 ::= SEQUENCE
SRSASN_CODE cqi_report_cfg_v1320_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cqi_report_periodic_v1320_present, 1));

  if (cqi_report_periodic_v1320_present) {
    HANDLE_CODE(cqi_report_periodic_v1320.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_cfg_v1320_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cqi_report_periodic_v1320_present, 1));

  if (cqi_report_periodic_v1320_present) {
    HANDLE_CODE(cqi_report_periodic_v1320.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_cfg_v1320_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cqi_report_periodic_v1320_present) {
    j.write_fieldname("cqi-ReportPeriodic-v1320");
    cqi_report_periodic_v1320.to_json(j);
  }
  j.end_obj();
}
bool cqi_report_cfg_v1320_s::operator==(const cqi_report_cfg_v1320_s& other) const
{
  return cqi_report_periodic_v1320_present == other.cqi_report_periodic_v1320_present and
         (not cqi_report_periodic_v1320_present or cqi_report_periodic_v1320 == other.cqi_report_periodic_v1320);
}

// CQI-ReportConfig-v1430 ::= SEQUENCE
SRSASN_CODE cqi_report_cfg_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cqi_report_aperiodic_hybrid_r14_present, 1));

  if (cqi_report_aperiodic_hybrid_r14_present) {
    HANDLE_CODE(cqi_report_aperiodic_hybrid_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_cfg_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cqi_report_aperiodic_hybrid_r14_present, 1));

  if (cqi_report_aperiodic_hybrid_r14_present) {
    HANDLE_CODE(cqi_report_aperiodic_hybrid_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_cfg_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cqi_report_aperiodic_hybrid_r14_present) {
    j.write_fieldname("cqi-ReportAperiodicHybrid-r14");
    cqi_report_aperiodic_hybrid_r14.to_json(j);
  }
  j.end_obj();
}
bool cqi_report_cfg_v1430_s::operator==(const cqi_report_cfg_v1430_s& other) const
{
  return cqi_report_aperiodic_hybrid_r14_present == other.cqi_report_aperiodic_hybrid_r14_present and
         (not cqi_report_aperiodic_hybrid_r14_present or
          cqi_report_aperiodic_hybrid_r14 == other.cqi_report_aperiodic_hybrid_r14);
}

// CSI-RS-Config-r10 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_rs_r10_present, 1));
  HANDLE_CODE(bref.pack(zero_tx_pwr_csi_rs_r10_present, 1));

  if (csi_rs_r10_present) {
    HANDLE_CODE(csi_rs_r10.pack(bref));
  }
  if (zero_tx_pwr_csi_rs_r10_present) {
    HANDLE_CODE(zero_tx_pwr_csi_rs_r10.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_rs_r10_present, 1));
  HANDLE_CODE(bref.unpack(zero_tx_pwr_csi_rs_r10_present, 1));

  if (csi_rs_r10_present) {
    HANDLE_CODE(csi_rs_r10.unpack(bref));
  }
  if (zero_tx_pwr_csi_rs_r10_present) {
    HANDLE_CODE(zero_tx_pwr_csi_rs_r10.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_rs_r10_present) {
    j.write_fieldname("csi-RS-r10");
    csi_rs_r10.to_json(j);
  }
  if (zero_tx_pwr_csi_rs_r10_present) {
    j.write_fieldname("zeroTxPowerCSI-RS-r10");
    zero_tx_pwr_csi_rs_r10.to_json(j);
  }
  j.end_obj();
}
bool csi_rs_cfg_r10_s::operator==(const csi_rs_cfg_r10_s& other) const
{
  return csi_rs_r10_present == other.csi_rs_r10_present and
         (not csi_rs_r10_present or csi_rs_r10 == other.csi_rs_r10) and
         zero_tx_pwr_csi_rs_r10_present == other.zero_tx_pwr_csi_rs_r10_present and
         (not zero_tx_pwr_csi_rs_r10_present or zero_tx_pwr_csi_rs_r10 == other.zero_tx_pwr_csi_rs_r10);
}

void csi_rs_cfg_r10_s::csi_rs_r10_c_::set(types::options e)
{
  type_ = e;
}
void csi_rs_cfg_r10_s::csi_rs_r10_c_::set_release()
{
  set(types::release);
}
csi_rs_cfg_r10_s::csi_rs_r10_c_::setup_s_& csi_rs_cfg_r10_s::csi_rs_r10_c_::set_setup()
{
  set(types::setup);
  return c;
}
void csi_rs_cfg_r10_s::csi_rs_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("antennaPortsCount-r10", c.ant_ports_count_r10.to_string());
      j.write_int("resourceConfig-r10", c.res_cfg_r10);
      j.write_int("subframeConfig-r10", c.sf_cfg_r10);
      j.write_int("p-C-r10", c.p_c_r10);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_r10_s::csi_rs_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_r10_s::csi_rs_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.ant_ports_count_r10.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.res_cfg_r10, (uint8_t)0u, (uint8_t)31u));
      HANDLE_CODE(pack_integer(bref, c.sf_cfg_r10, (uint8_t)0u, (uint8_t)154u));
      HANDLE_CODE(pack_integer(bref, c.p_c_r10, (int8_t)-8, (int8_t)15));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_r10_s::csi_rs_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_r10_s::csi_rs_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.ant_ports_count_r10.unpack(bref));
      HANDLE_CODE(unpack_integer(c.res_cfg_r10, bref, (uint8_t)0u, (uint8_t)31u));
      HANDLE_CODE(unpack_integer(c.sf_cfg_r10, bref, (uint8_t)0u, (uint8_t)154u));
      HANDLE_CODE(unpack_integer(c.p_c_r10, bref, (int8_t)-8, (int8_t)15));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_r10_s::csi_rs_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_r10_s::csi_rs_r10_c_::operator==(const csi_rs_r10_c_& other) const
{
  return type() == other.type() and c.ant_ports_count_r10 == other.c.ant_ports_count_r10 and
         c.res_cfg_r10 == other.c.res_cfg_r10 and c.sf_cfg_r10 == other.c.sf_cfg_r10 and c.p_c_r10 == other.c.p_c_r10;
}

const char* csi_rs_cfg_r10_s::csi_rs_r10_c_::setup_s_::ant_ports_count_r10_opts::to_string() const
{
  static const char* options[] = {"an1", "an2", "an4", "an8"};
  return convert_enum_idx(options, 4, value, "csi_rs_cfg_r10_s::csi_rs_r10_c_::setup_s_::ant_ports_count_r10_e_");
}
uint8_t csi_rs_cfg_r10_s::csi_rs_r10_c_::setup_s_::ant_ports_count_r10_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(options, 4, value, "csi_rs_cfg_r10_s::csi_rs_r10_c_::setup_s_::ant_ports_count_r10_e_");
}

// CSI-RS-Config-v1250 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(zero_tx_pwr_csi_rs2_r12_present, 1));
  HANDLE_CODE(bref.pack(ds_zero_tx_pwr_csi_rs_r12_present, 1));

  if (zero_tx_pwr_csi_rs2_r12_present) {
    HANDLE_CODE(zero_tx_pwr_csi_rs2_r12.pack(bref));
  }
  if (ds_zero_tx_pwr_csi_rs_r12_present) {
    HANDLE_CODE(ds_zero_tx_pwr_csi_rs_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(zero_tx_pwr_csi_rs2_r12_present, 1));
  HANDLE_CODE(bref.unpack(ds_zero_tx_pwr_csi_rs_r12_present, 1));

  if (zero_tx_pwr_csi_rs2_r12_present) {
    HANDLE_CODE(zero_tx_pwr_csi_rs2_r12.unpack(bref));
  }
  if (ds_zero_tx_pwr_csi_rs_r12_present) {
    HANDLE_CODE(ds_zero_tx_pwr_csi_rs_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (zero_tx_pwr_csi_rs2_r12_present) {
    j.write_fieldname("zeroTxPowerCSI-RS2-r12");
    zero_tx_pwr_csi_rs2_r12.to_json(j);
  }
  if (ds_zero_tx_pwr_csi_rs_r12_present) {
    j.write_fieldname("ds-ZeroTxPowerCSI-RS-r12");
    ds_zero_tx_pwr_csi_rs_r12.to_json(j);
  }
  j.end_obj();
}
bool csi_rs_cfg_v1250_s::operator==(const csi_rs_cfg_v1250_s& other) const
{
  return zero_tx_pwr_csi_rs2_r12_present == other.zero_tx_pwr_csi_rs2_r12_present and
         (not zero_tx_pwr_csi_rs2_r12_present or zero_tx_pwr_csi_rs2_r12 == other.zero_tx_pwr_csi_rs2_r12) and
         ds_zero_tx_pwr_csi_rs_r12_present == other.ds_zero_tx_pwr_csi_rs_r12_present and
         (not ds_zero_tx_pwr_csi_rs_r12_present or ds_zero_tx_pwr_csi_rs_r12 == other.ds_zero_tx_pwr_csi_rs_r12);
}

void csi_rs_cfg_v1250_s::ds_zero_tx_pwr_csi_rs_r12_c_::set(types::options e)
{
  type_ = e;
}
void csi_rs_cfg_v1250_s::ds_zero_tx_pwr_csi_rs_r12_c_::set_release()
{
  set(types::release);
}
csi_rs_cfg_v1250_s::ds_zero_tx_pwr_csi_rs_r12_c_::setup_s_&
csi_rs_cfg_v1250_s::ds_zero_tx_pwr_csi_rs_r12_c_::set_setup()
{
  set(types::setup);
  return c;
}
void csi_rs_cfg_v1250_s::ds_zero_tx_pwr_csi_rs_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("zeroTxPowerCSI-RS-List-r12");
      for (const auto& e1 : c.zero_tx_pwr_csi_rs_list_r12) {
        e1.to_json(j);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_v1250_s::ds_zero_tx_pwr_csi_rs_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_v1250_s::ds_zero_tx_pwr_csi_rs_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.zero_tx_pwr_csi_rs_list_r12, 1, 5));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_v1250_s::ds_zero_tx_pwr_csi_rs_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_v1250_s::ds_zero_tx_pwr_csi_rs_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.zero_tx_pwr_csi_rs_list_r12, bref, 1, 5));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_v1250_s::ds_zero_tx_pwr_csi_rs_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_v1250_s::ds_zero_tx_pwr_csi_rs_r12_c_::operator==(const ds_zero_tx_pwr_csi_rs_r12_c_& other) const
{
  return type() == other.type() and c.zero_tx_pwr_csi_rs_list_r12 == other.c.zero_tx_pwr_csi_rs_list_r12;
}

// CSI-RS-Config-v1310 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(emimo_type_r13_present, 1));

  if (emimo_type_r13_present) {
    HANDLE_CODE(emimo_type_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(emimo_type_r13_present, 1));

  if (emimo_type_r13_present) {
    HANDLE_CODE(emimo_type_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (emimo_type_r13_present) {
    j.write_fieldname("eMIMO-Type-r13");
    emimo_type_r13.to_json(j);
  }
  j.end_obj();
}
bool csi_rs_cfg_v1310_s::operator==(const csi_rs_cfg_v1310_s& other) const
{
  return emimo_type_r13_present == other.emimo_type_r13_present and
         (not emimo_type_r13_present or emimo_type_r13 == other.emimo_type_r13);
}

// CSI-RS-Config-v1430 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dummy_present, 1));
  HANDLE_CODE(bref.pack(emimo_hybrid_r14_present, 1));
  HANDLE_CODE(bref.pack(advanced_codebook_enabled_r14_present, 1));

  if (dummy_present) {
    HANDLE_CODE(dummy.pack(bref));
  }
  if (emimo_hybrid_r14_present) {
    HANDLE_CODE(emimo_hybrid_r14.pack(bref));
  }
  if (advanced_codebook_enabled_r14_present) {
    HANDLE_CODE(bref.pack(advanced_codebook_enabled_r14, 1));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dummy_present, 1));
  HANDLE_CODE(bref.unpack(emimo_hybrid_r14_present, 1));
  HANDLE_CODE(bref.unpack(advanced_codebook_enabled_r14_present, 1));

  if (dummy_present) {
    HANDLE_CODE(dummy.unpack(bref));
  }
  if (emimo_hybrid_r14_present) {
    HANDLE_CODE(emimo_hybrid_r14.unpack(bref));
  }
  if (advanced_codebook_enabled_r14_present) {
    HANDLE_CODE(bref.unpack(advanced_codebook_enabled_r14, 1));
  }

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dummy_present) {
    j.write_fieldname("dummy");
    dummy.to_json(j);
  }
  if (emimo_hybrid_r14_present) {
    j.write_fieldname("eMIMO-Hybrid-r14");
    emimo_hybrid_r14.to_json(j);
  }
  if (advanced_codebook_enabled_r14_present) {
    j.write_bool("advancedCodebookEnabled-r14", advanced_codebook_enabled_r14);
  }
  j.end_obj();
}
bool csi_rs_cfg_v1430_s::operator==(const csi_rs_cfg_v1430_s& other) const
{
  return dummy_present == other.dummy_present and (not dummy_present or dummy == other.dummy) and
         emimo_hybrid_r14_present == other.emimo_hybrid_r14_present and
         (not emimo_hybrid_r14_present or emimo_hybrid_r14 == other.emimo_hybrid_r14) and
         advanced_codebook_enabled_r14_present == other.advanced_codebook_enabled_r14_present and
         (not advanced_codebook_enabled_r14_present or
          advanced_codebook_enabled_r14 == other.advanced_codebook_enabled_r14);
}

// CSI-RS-ConfigZP-r11 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_zp_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, csi_rs_cfg_zp_id_r11, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(res_cfg_list_r11.pack(bref));
  HANDLE_CODE(pack_integer(bref, sf_cfg_r11, (uint8_t)0u, (uint8_t)154u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_zp_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(csi_rs_cfg_zp_id_r11, bref, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(res_cfg_list_r11.unpack(bref));
  HANDLE_CODE(unpack_integer(sf_cfg_r11, bref, (uint8_t)0u, (uint8_t)154u));

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_zp_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-RS-ConfigZPId-r11", csi_rs_cfg_zp_id_r11);
  j.write_str("resourceConfigList-r11", res_cfg_list_r11.to_string());
  j.write_int("subframeConfig-r11", sf_cfg_r11);
  j.end_obj();
}
bool csi_rs_cfg_zp_r11_s::operator==(const csi_rs_cfg_zp_r11_s& other) const
{
  return ext == other.ext and csi_rs_cfg_zp_id_r11 == other.csi_rs_cfg_zp_id_r11 and
         res_cfg_list_r11 == other.res_cfg_list_r11 and sf_cfg_r11 == other.sf_cfg_r11;
}

// DeltaTxD-OffsetListSPUCCH-r15 ::= SEQUENCE
SRSASN_CODE delta_tx_d_offset_list_spucch_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(delta_tx_d_offset_spucch_format1_r15.pack(bref));
  HANDLE_CODE(delta_tx_d_offset_spucch_format1a_r15.pack(bref));
  HANDLE_CODE(delta_tx_d_offset_spucch_format1b_r15.pack(bref));
  HANDLE_CODE(delta_tx_d_offset_spucch_format3_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE delta_tx_d_offset_list_spucch_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(delta_tx_d_offset_spucch_format1_r15.unpack(bref));
  HANDLE_CODE(delta_tx_d_offset_spucch_format1a_r15.unpack(bref));
  HANDLE_CODE(delta_tx_d_offset_spucch_format1b_r15.unpack(bref));
  HANDLE_CODE(delta_tx_d_offset_spucch_format3_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void delta_tx_d_offset_list_spucch_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("deltaTxD-OffsetSPUCCH-Format1-r15", delta_tx_d_offset_spucch_format1_r15.to_string());
  j.write_str("deltaTxD-OffsetSPUCCH-Format1a-r15", delta_tx_d_offset_spucch_format1a_r15.to_string());
  j.write_str("deltaTxD-OffsetSPUCCH-Format1b-r15", delta_tx_d_offset_spucch_format1b_r15.to_string());
  j.write_str("deltaTxD-OffsetSPUCCH-Format3-r15", delta_tx_d_offset_spucch_format3_r15.to_string());
  j.end_obj();
}
bool delta_tx_d_offset_list_spucch_r15_s::operator==(const delta_tx_d_offset_list_spucch_r15_s& other) const
{
  return ext == other.ext and delta_tx_d_offset_spucch_format1_r15 == other.delta_tx_d_offset_spucch_format1_r15 and
         delta_tx_d_offset_spucch_format1a_r15 == other.delta_tx_d_offset_spucch_format1a_r15 and
         delta_tx_d_offset_spucch_format1b_r15 == other.delta_tx_d_offset_spucch_format1b_r15 and
         delta_tx_d_offset_spucch_format3_r15 == other.delta_tx_d_offset_spucch_format3_r15;
}

const char* delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1_r15_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1_r15_e_");
}
int8_t delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1_r15_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1_r15_e_");
}

const char* delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1a_r15_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1a_r15_e_");
}
int8_t delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1a_r15_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1a_r15_e_");
}

const char* delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1b_r15_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1b_r15_e_");
}
int8_t delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1b_r15_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1b_r15_e_");
}

const char* delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format3_r15_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format3_r15_e_");
}
int8_t delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format3_r15_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format3_r15_e_");
}

// EPDCCH-SetConfig-r11 ::= SEQUENCE
SRSASN_CODE epdcch_set_cfg_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(re_map_qcl_cfg_id_r11_present, 1));

  HANDLE_CODE(pack_integer(bref, set_cfg_id_r11, (uint8_t)0u, (uint8_t)1u));
  HANDLE_CODE(tx_type_r11.pack(bref));
  HANDLE_CODE(res_block_assign_r11.num_prb_pairs_r11.pack(bref));
  HANDLE_CODE(res_block_assign_r11.res_block_assign_r11.pack(bref));
  HANDLE_CODE(pack_integer(bref, dmrs_scrambling_seq_int_r11, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(pack_integer(bref, pucch_res_start_offset_r11, (uint16_t)0u, (uint16_t)2047u));
  if (re_map_qcl_cfg_id_r11_present) {
    HANDLE_CODE(pack_integer(bref, re_map_qcl_cfg_id_r11, (uint8_t)1u, (uint8_t)4u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= csi_rs_cfg_zp_id2_r12.is_present();
    group_flags[1] |= num_prb_pairs_v1310.is_present();
    group_flags[1] |= mpdcch_cfg_r13.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(csi_rs_cfg_zp_id2_r12.is_present(), 1));
      if (csi_rs_cfg_zp_id2_r12.is_present()) {
        HANDLE_CODE(csi_rs_cfg_zp_id2_r12->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(num_prb_pairs_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(mpdcch_cfg_r13.is_present(), 1));
      if (num_prb_pairs_v1310.is_present()) {
        HANDLE_CODE(num_prb_pairs_v1310->pack(bref));
      }
      if (mpdcch_cfg_r13.is_present()) {
        HANDLE_CODE(mpdcch_cfg_r13->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE epdcch_set_cfg_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(re_map_qcl_cfg_id_r11_present, 1));

  HANDLE_CODE(unpack_integer(set_cfg_id_r11, bref, (uint8_t)0u, (uint8_t)1u));
  HANDLE_CODE(tx_type_r11.unpack(bref));
  HANDLE_CODE(res_block_assign_r11.num_prb_pairs_r11.unpack(bref));
  HANDLE_CODE(res_block_assign_r11.res_block_assign_r11.unpack(bref));
  HANDLE_CODE(unpack_integer(dmrs_scrambling_seq_int_r11, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(unpack_integer(pucch_res_start_offset_r11, bref, (uint16_t)0u, (uint16_t)2047u));
  if (re_map_qcl_cfg_id_r11_present) {
    HANDLE_CODE(unpack_integer(re_map_qcl_cfg_id_r11, bref, (uint8_t)1u, (uint8_t)4u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool csi_rs_cfg_zp_id2_r12_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_zp_id2_r12_present, 1));
      csi_rs_cfg_zp_id2_r12.set_present(csi_rs_cfg_zp_id2_r12_present);
      if (csi_rs_cfg_zp_id2_r12.is_present()) {
        HANDLE_CODE(csi_rs_cfg_zp_id2_r12->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool num_prb_pairs_v1310_present;
      HANDLE_CODE(bref.unpack(num_prb_pairs_v1310_present, 1));
      num_prb_pairs_v1310.set_present(num_prb_pairs_v1310_present);
      bool mpdcch_cfg_r13_present;
      HANDLE_CODE(bref.unpack(mpdcch_cfg_r13_present, 1));
      mpdcch_cfg_r13.set_present(mpdcch_cfg_r13_present);
      if (num_prb_pairs_v1310.is_present()) {
        HANDLE_CODE(num_prb_pairs_v1310->unpack(bref));
      }
      if (mpdcch_cfg_r13.is_present()) {
        HANDLE_CODE(mpdcch_cfg_r13->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void epdcch_set_cfg_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("setConfigId-r11", set_cfg_id_r11);
  j.write_str("transmissionType-r11", tx_type_r11.to_string());
  j.write_fieldname("resourceBlockAssignment-r11");
  j.start_obj();
  j.write_str("numberPRB-Pairs-r11", res_block_assign_r11.num_prb_pairs_r11.to_string());
  j.write_str("resourceBlockAssignment-r11", res_block_assign_r11.res_block_assign_r11.to_string());
  j.end_obj();
  j.write_int("dmrs-ScramblingSequenceInt-r11", dmrs_scrambling_seq_int_r11);
  j.write_int("pucch-ResourceStartOffset-r11", pucch_res_start_offset_r11);
  if (re_map_qcl_cfg_id_r11_present) {
    j.write_int("re-MappingQCL-ConfigId-r11", re_map_qcl_cfg_id_r11);
  }
  if (ext) {
    if (csi_rs_cfg_zp_id2_r12.is_present()) {
      j.write_fieldname("csi-RS-ConfigZPId2-r12");
      csi_rs_cfg_zp_id2_r12->to_json(j);
    }
    if (num_prb_pairs_v1310.is_present()) {
      j.write_fieldname("numberPRB-Pairs-v1310");
      num_prb_pairs_v1310->to_json(j);
    }
    if (mpdcch_cfg_r13.is_present()) {
      j.write_fieldname("mpdcch-config-r13");
      mpdcch_cfg_r13->to_json(j);
    }
  }
  j.end_obj();
}
bool epdcch_set_cfg_r11_s::operator==(const epdcch_set_cfg_r11_s& other) const
{
  return ext == other.ext and set_cfg_id_r11 == other.set_cfg_id_r11 and tx_type_r11 == other.tx_type_r11 and
         res_block_assign_r11.num_prb_pairs_r11 == other.res_block_assign_r11.num_prb_pairs_r11 and
         res_block_assign_r11.res_block_assign_r11 == other.res_block_assign_r11.res_block_assign_r11 and
         dmrs_scrambling_seq_int_r11 == other.dmrs_scrambling_seq_int_r11 and
         pucch_res_start_offset_r11 == other.pucch_res_start_offset_r11 and
         re_map_qcl_cfg_id_r11_present == other.re_map_qcl_cfg_id_r11_present and
         (not re_map_qcl_cfg_id_r11_present or re_map_qcl_cfg_id_r11 == other.re_map_qcl_cfg_id_r11) and
         (not ext or
          (csi_rs_cfg_zp_id2_r12.is_present() == other.csi_rs_cfg_zp_id2_r12.is_present() and
           (not csi_rs_cfg_zp_id2_r12.is_present() or *csi_rs_cfg_zp_id2_r12 == *other.csi_rs_cfg_zp_id2_r12) and
           num_prb_pairs_v1310.is_present() == other.num_prb_pairs_v1310.is_present() and
           (not num_prb_pairs_v1310.is_present() or *num_prb_pairs_v1310 == *other.num_prb_pairs_v1310) and
           mpdcch_cfg_r13.is_present() == other.mpdcch_cfg_r13.is_present() and
           (not mpdcch_cfg_r13.is_present() or *mpdcch_cfg_r13 == *other.mpdcch_cfg_r13)));
}

const char* epdcch_set_cfg_r11_s::tx_type_r11_opts::to_string() const
{
  static const char* options[] = {"localised", "distributed"};
  return convert_enum_idx(options, 2, value, "epdcch_set_cfg_r11_s::tx_type_r11_e_");
}

const char* epdcch_set_cfg_r11_s::res_block_assign_r11_s_::num_prb_pairs_r11_opts::to_string() const
{
  static const char* options[] = {"n2", "n4", "n8"};
  return convert_enum_idx(options, 3, value, "epdcch_set_cfg_r11_s::res_block_assign_r11_s_::num_prb_pairs_r11_e_");
}
uint8_t epdcch_set_cfg_r11_s::res_block_assign_r11_s_::num_prb_pairs_r11_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8};
  return map_enum_number(options, 3, value, "epdcch_set_cfg_r11_s::res_block_assign_r11_s_::num_prb_pairs_r11_e_");
}

void epdcch_set_cfg_r11_s::csi_rs_cfg_zp_id2_r12_c_::set(types::options e)
{
  type_ = e;
}
void epdcch_set_cfg_r11_s::csi_rs_cfg_zp_id2_r12_c_::set_release()
{
  set(types::release);
}
uint8_t& epdcch_set_cfg_r11_s::csi_rs_cfg_zp_id2_r12_c_::set_setup()
{
  set(types::setup);
  return c;
}
void epdcch_set_cfg_r11_s::csi_rs_cfg_zp_id2_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_int("setup", c);
      break;
    default:
      log_invalid_choice_id(type_, "epdcch_set_cfg_r11_s::csi_rs_cfg_zp_id2_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE epdcch_set_cfg_r11_s::csi_rs_cfg_zp_id2_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c, (uint8_t)1u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "epdcch_set_cfg_r11_s::csi_rs_cfg_zp_id2_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE epdcch_set_cfg_r11_s::csi_rs_cfg_zp_id2_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c, bref, (uint8_t)1u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "epdcch_set_cfg_r11_s::csi_rs_cfg_zp_id2_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool epdcch_set_cfg_r11_s::csi_rs_cfg_zp_id2_r12_c_::operator==(const csi_rs_cfg_zp_id2_r12_c_& other) const
{
  return type() == other.type() and c == other.c;
}

void epdcch_set_cfg_r11_s::num_prb_pairs_v1310_c_::set(types::options e)
{
  type_ = e;
}
void epdcch_set_cfg_r11_s::num_prb_pairs_v1310_c_::set_release()
{
  set(types::release);
}
void epdcch_set_cfg_r11_s::num_prb_pairs_v1310_c_::set_setup()
{
  set(types::setup);
}
void epdcch_set_cfg_r11_s::num_prb_pairs_v1310_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.end_obj();
}
SRSASN_CODE epdcch_set_cfg_r11_s::num_prb_pairs_v1310_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  return SRSASN_SUCCESS;
}
SRSASN_CODE epdcch_set_cfg_r11_s::num_prb_pairs_v1310_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  return SRSASN_SUCCESS;
}
bool epdcch_set_cfg_r11_s::num_prb_pairs_v1310_c_::operator==(const num_prb_pairs_v1310_c_& other) const
{
  return type() == other.type();
}

void epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::set(types::options e)
{
  type_ = e;
}
void epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::set_release()
{
  set(types::release);
}
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_& epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("csi-NumRepetitionCE-r13", c.csi_num_repeat_ce_r13.to_string());
      j.write_str("mpdcch-pdsch-HoppingConfig-r13", c.mpdcch_pdsch_hop_cfg_r13.to_string());
      j.write_fieldname("mpdcch-StartSF-UESS-r13");
      c.mpdcch_start_sf_uess_r13.to_json(j);
      j.write_str("mpdcch-NumRepetition-r13", c.mpdcch_num_repeat_r13.to_string());
      j.write_int("mpdcch-Narrowband-r13", c.mpdcch_nb_r13);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.csi_num_repeat_ce_r13.pack(bref));
      HANDLE_CODE(c.mpdcch_pdsch_hop_cfg_r13.pack(bref));
      HANDLE_CODE(c.mpdcch_start_sf_uess_r13.pack(bref));
      HANDLE_CODE(c.mpdcch_num_repeat_r13.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.mpdcch_nb_r13, (uint8_t)1u, (uint8_t)16u));
      break;
    default:
      log_invalid_choice_id(type_, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.csi_num_repeat_ce_r13.unpack(bref));
      HANDLE_CODE(c.mpdcch_pdsch_hop_cfg_r13.unpack(bref));
      HANDLE_CODE(c.mpdcch_start_sf_uess_r13.unpack(bref));
      HANDLE_CODE(c.mpdcch_num_repeat_r13.unpack(bref));
      HANDLE_CODE(unpack_integer(c.mpdcch_nb_r13, bref, (uint8_t)1u, (uint8_t)16u));
      break;
    default:
      log_invalid_choice_id(type_, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::operator==(const mpdcch_cfg_r13_c_& other) const
{
  return type() == other.type() and c.csi_num_repeat_ce_r13 == other.c.csi_num_repeat_ce_r13 and
         c.mpdcch_pdsch_hop_cfg_r13 == other.c.mpdcch_pdsch_hop_cfg_r13 and
         c.mpdcch_start_sf_uess_r13 == other.c.mpdcch_start_sf_uess_r13 and
         c.mpdcch_num_repeat_r13 == other.c.mpdcch_num_repeat_r13 and c.mpdcch_nb_r13 == other.c.mpdcch_nb_r13;
}

const char* epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::csi_num_repeat_ce_r13_opts::to_string() const
{
  static const char* options[] = {"sf1", "sf2", "sf4", "sf8", "sf16", "sf32"};
  return convert_enum_idx(
      options, 6, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::csi_num_repeat_ce_r13_e_");
}
uint8_t epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::csi_num_repeat_ce_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8, 16, 32};
  return map_enum_number(
      options, 6, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::csi_num_repeat_ce_r13_e_");
}

const char* epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_pdsch_hop_cfg_r13_opts::to_string() const
{
  static const char* options[] = {"on", "off"};
  return convert_enum_idx(
      options, 2, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_pdsch_hop_cfg_r13_e_");
}

void epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::destroy_() {}
void epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::mpdcch_start_sf_uess_r13_c_(
    const epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_& other)
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
      log_invalid_choice_id(type_, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_");
  }
}
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_&
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::operator=(
    const epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_& other)
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
      log_invalid_choice_id(type_, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_");
  }

  return *this;
}
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_e_&
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::set_fdd_r13()
{
  set(types::fdd_r13);
  return c.get<fdd_r13_e_>();
}
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::tdd_r13_e_&
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::set_tdd_r13()
{
  set(types::tdd_r13);
  return c.get<tdd_r13_e_>();
}
void epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::operator==(
    const mpdcch_start_sf_uess_r13_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::fdd_r13:
      return c.get<fdd_r13_e_>() == other.c.get<fdd_r13_e_>();
    case types::tdd_r13:
      return c.get<tdd_r13_e_>() == other.c.get<tdd_r13_e_>();
    default:
      return true;
  }
  return true;
}

const char*
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v1dot5", "v2", "v2dot5", "v4", "v5", "v8", "v10"};
  return convert_enum_idx(
      options, 8, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_e_");
}
float epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_opts::to_number() const
{
  static const float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return map_enum_number(
      options, 8, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_e_");
}
const char*
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_opts::to_number_string() const
{
  static const char* options[] = {"1", "1.5", "2", "2.5", "4", "5", "8", "10"};
  return convert_enum_idx(
      options, 8, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_e_");
}

const char*
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::tdd_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v2", "v4", "v5", "v8", "v10", "v20", "spare1"};
  return convert_enum_idx(
      options, 8, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::tdd_r13_e_");
}
uint8_t epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::tdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return map_enum_number(
      options, 7, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::tdd_r13_e_");
}

const char* epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_num_repeat_r13_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "r128", "r256"};
  return convert_enum_idx(
      options, 9, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_num_repeat_r13_e_");
}
uint16_t epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_num_repeat_r13_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(
      options, 9, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_num_repeat_r13_e_");
}

// Enable256QAM-r14 ::= CHOICE
void enable256_qam_r14_c::set(types::options e)
{
  type_ = e;
}
void enable256_qam_r14_c::set_release()
{
  set(types::release);
}
enable256_qam_r14_c::setup_c_& enable256_qam_r14_c::set_setup()
{
  set(types::setup);
  return c;
}
void enable256_qam_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "enable256_qam_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE enable256_qam_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "enable256_qam_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE enable256_qam_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "enable256_qam_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool enable256_qam_r14_c::operator==(const enable256_qam_r14_c& other) const
{
  return type() == other.type() and c == other.c;
}

void enable256_qam_r14_c::setup_c_::destroy_()
{
  switch (type_) {
    case types::tpc_sf_set_cfgured_r14:
      c.destroy<tpc_sf_set_cfgured_r14_s_>();
      break;
    case types::tpc_sf_set_not_cfgured_r14:
      c.destroy<tpc_sf_set_not_cfgured_r14_s_>();
      break;
    default:
      break;
  }
}
void enable256_qam_r14_c::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::tpc_sf_set_cfgured_r14:
      c.init<tpc_sf_set_cfgured_r14_s_>();
      break;
    case types::tpc_sf_set_not_cfgured_r14:
      c.init<tpc_sf_set_not_cfgured_r14_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enable256_qam_r14_c::setup_c_");
  }
}
enable256_qam_r14_c::setup_c_::setup_c_(const enable256_qam_r14_c::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::tpc_sf_set_cfgured_r14:
      c.init(other.c.get<tpc_sf_set_cfgured_r14_s_>());
      break;
    case types::tpc_sf_set_not_cfgured_r14:
      c.init(other.c.get<tpc_sf_set_not_cfgured_r14_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enable256_qam_r14_c::setup_c_");
  }
}
enable256_qam_r14_c::setup_c_& enable256_qam_r14_c::setup_c_::operator=(const enable256_qam_r14_c::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::tpc_sf_set_cfgured_r14:
      c.set(other.c.get<tpc_sf_set_cfgured_r14_s_>());
      break;
    case types::tpc_sf_set_not_cfgured_r14:
      c.set(other.c.get<tpc_sf_set_not_cfgured_r14_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enable256_qam_r14_c::setup_c_");
  }

  return *this;
}
enable256_qam_r14_c::setup_c_::tpc_sf_set_cfgured_r14_s_& enable256_qam_r14_c::setup_c_::set_tpc_sf_set_cfgured_r14()
{
  set(types::tpc_sf_set_cfgured_r14);
  return c.get<tpc_sf_set_cfgured_r14_s_>();
}
enable256_qam_r14_c::setup_c_::tpc_sf_set_not_cfgured_r14_s_&
enable256_qam_r14_c::setup_c_::set_tpc_sf_set_not_cfgured_r14()
{
  set(types::tpc_sf_set_not_cfgured_r14);
  return c.get<tpc_sf_set_not_cfgured_r14_s_>();
}
void enable256_qam_r14_c::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::tpc_sf_set_cfgured_r14:
      j.write_fieldname("tpc-SubframeSet-Configured-r14");
      j.start_obj();
      j.write_bool("subframeSet1-DCI-Format0-r14", c.get<tpc_sf_set_cfgured_r14_s_>().sf_set1_dci_format0_r14);
      j.write_bool("subframeSet1-DCI-Format4-r14", c.get<tpc_sf_set_cfgured_r14_s_>().sf_set1_dci_format4_r14);
      j.write_bool("subframeSet2-DCI-Format0-r14", c.get<tpc_sf_set_cfgured_r14_s_>().sf_set2_dci_format0_r14);
      j.write_bool("subframeSet2-DCI-Format4-r14", c.get<tpc_sf_set_cfgured_r14_s_>().sf_set2_dci_format4_r14);
      j.end_obj();
      break;
    case types::tpc_sf_set_not_cfgured_r14:
      j.write_fieldname("tpc-SubframeSet-NotConfigured-r14");
      j.start_obj();
      j.write_bool("dci-Format0-r14", c.get<tpc_sf_set_not_cfgured_r14_s_>().dci_format0_r14);
      j.write_bool("dci-Format4-r14", c.get<tpc_sf_set_not_cfgured_r14_s_>().dci_format4_r14);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "enable256_qam_r14_c::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE enable256_qam_r14_c::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::tpc_sf_set_cfgured_r14:
      HANDLE_CODE(bref.pack(c.get<tpc_sf_set_cfgured_r14_s_>().sf_set1_dci_format0_r14, 1));
      HANDLE_CODE(bref.pack(c.get<tpc_sf_set_cfgured_r14_s_>().sf_set1_dci_format4_r14, 1));
      HANDLE_CODE(bref.pack(c.get<tpc_sf_set_cfgured_r14_s_>().sf_set2_dci_format0_r14, 1));
      HANDLE_CODE(bref.pack(c.get<tpc_sf_set_cfgured_r14_s_>().sf_set2_dci_format4_r14, 1));
      break;
    case types::tpc_sf_set_not_cfgured_r14:
      HANDLE_CODE(bref.pack(c.get<tpc_sf_set_not_cfgured_r14_s_>().dci_format0_r14, 1));
      HANDLE_CODE(bref.pack(c.get<tpc_sf_set_not_cfgured_r14_s_>().dci_format4_r14, 1));
      break;
    default:
      log_invalid_choice_id(type_, "enable256_qam_r14_c::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE enable256_qam_r14_c::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::tpc_sf_set_cfgured_r14:
      HANDLE_CODE(bref.unpack(c.get<tpc_sf_set_cfgured_r14_s_>().sf_set1_dci_format0_r14, 1));
      HANDLE_CODE(bref.unpack(c.get<tpc_sf_set_cfgured_r14_s_>().sf_set1_dci_format4_r14, 1));
      HANDLE_CODE(bref.unpack(c.get<tpc_sf_set_cfgured_r14_s_>().sf_set2_dci_format0_r14, 1));
      HANDLE_CODE(bref.unpack(c.get<tpc_sf_set_cfgured_r14_s_>().sf_set2_dci_format4_r14, 1));
      break;
    case types::tpc_sf_set_not_cfgured_r14:
      HANDLE_CODE(bref.unpack(c.get<tpc_sf_set_not_cfgured_r14_s_>().dci_format0_r14, 1));
      HANDLE_CODE(bref.unpack(c.get<tpc_sf_set_not_cfgured_r14_s_>().dci_format4_r14, 1));
      break;
    default:
      log_invalid_choice_id(type_, "enable256_qam_r14_c::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool enable256_qam_r14_c::setup_c_::operator==(const setup_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::tpc_sf_set_cfgured_r14:
      return c.get<tpc_sf_set_cfgured_r14_s_>().sf_set1_dci_format0_r14 ==
                 other.c.get<tpc_sf_set_cfgured_r14_s_>().sf_set1_dci_format0_r14 and
             c.get<tpc_sf_set_cfgured_r14_s_>().sf_set1_dci_format4_r14 ==
                 other.c.get<tpc_sf_set_cfgured_r14_s_>().sf_set1_dci_format4_r14 and
             c.get<tpc_sf_set_cfgured_r14_s_>().sf_set2_dci_format0_r14 ==
                 other.c.get<tpc_sf_set_cfgured_r14_s_>().sf_set2_dci_format0_r14 and
             c.get<tpc_sf_set_cfgured_r14_s_>().sf_set2_dci_format4_r14 ==
                 other.c.get<tpc_sf_set_cfgured_r14_s_>().sf_set2_dci_format4_r14;
    case types::tpc_sf_set_not_cfgured_r14:
      return c.get<tpc_sf_set_not_cfgured_r14_s_>().dci_format0_r14 ==
                 other.c.get<tpc_sf_set_not_cfgured_r14_s_>().dci_format0_r14 and
             c.get<tpc_sf_set_not_cfgured_r14_s_>().dci_format4_r14 ==
                 other.c.get<tpc_sf_set_not_cfgured_r14_s_>().dci_format4_r14;
    default:
      return true;
  }
  return true;
}

// PDSCH-RE-MappingQCL-Config-r11 ::= SEQUENCE
SRSASN_CODE pdsch_re_map_qcl_cfg_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(optional_set_of_fields_r11_present, 1));
  HANDLE_CODE(bref.pack(qcl_csi_rs_cfg_nzp_id_r11_present, 1));

  HANDLE_CODE(pack_integer(bref, pdsch_re_map_qcl_cfg_id_r11, (uint8_t)1u, (uint8_t)4u));
  if (optional_set_of_fields_r11_present) {
    HANDLE_CODE(bref.pack(optional_set_of_fields_r11.mbsfn_sf_cfg_list_r11_present, 1));
    HANDLE_CODE(optional_set_of_fields_r11.crs_ports_count_r11.pack(bref));
    HANDLE_CODE(pack_integer(bref, optional_set_of_fields_r11.crs_freq_shift_r11, (uint8_t)0u, (uint8_t)5u));
    if (optional_set_of_fields_r11.mbsfn_sf_cfg_list_r11_present) {
      HANDLE_CODE(optional_set_of_fields_r11.mbsfn_sf_cfg_list_r11.pack(bref));
    }
    HANDLE_CODE(optional_set_of_fields_r11.pdsch_start_r11.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, csi_rs_cfg_zp_id_r11, (uint8_t)1u, (uint8_t)4u));
  if (qcl_csi_rs_cfg_nzp_id_r11_present) {
    HANDLE_CODE(pack_integer(bref, qcl_csi_rs_cfg_nzp_id_r11, (uint8_t)1u, (uint8_t)3u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= mbsfn_sf_cfg_list_v1430.is_present();
    group_flags[1] |= codeword_one_cfg_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mbsfn_sf_cfg_list_v1430.is_present(), 1));
      if (mbsfn_sf_cfg_list_v1430.is_present()) {
        HANDLE_CODE(mbsfn_sf_cfg_list_v1430->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(codeword_one_cfg_v1530.is_present(), 1));
      if (codeword_one_cfg_v1530.is_present()) {
        HANDLE_CODE(codeword_one_cfg_v1530->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_re_map_qcl_cfg_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(optional_set_of_fields_r11_present, 1));
  HANDLE_CODE(bref.unpack(qcl_csi_rs_cfg_nzp_id_r11_present, 1));

  HANDLE_CODE(unpack_integer(pdsch_re_map_qcl_cfg_id_r11, bref, (uint8_t)1u, (uint8_t)4u));
  if (optional_set_of_fields_r11_present) {
    HANDLE_CODE(bref.unpack(optional_set_of_fields_r11.mbsfn_sf_cfg_list_r11_present, 1));
    HANDLE_CODE(optional_set_of_fields_r11.crs_ports_count_r11.unpack(bref));
    HANDLE_CODE(unpack_integer(optional_set_of_fields_r11.crs_freq_shift_r11, bref, (uint8_t)0u, (uint8_t)5u));
    if (optional_set_of_fields_r11.mbsfn_sf_cfg_list_r11_present) {
      HANDLE_CODE(optional_set_of_fields_r11.mbsfn_sf_cfg_list_r11.unpack(bref));
    }
    HANDLE_CODE(optional_set_of_fields_r11.pdsch_start_r11.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(csi_rs_cfg_zp_id_r11, bref, (uint8_t)1u, (uint8_t)4u));
  if (qcl_csi_rs_cfg_nzp_id_r11_present) {
    HANDLE_CODE(unpack_integer(qcl_csi_rs_cfg_nzp_id_r11, bref, (uint8_t)1u, (uint8_t)3u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mbsfn_sf_cfg_list_v1430_present;
      HANDLE_CODE(bref.unpack(mbsfn_sf_cfg_list_v1430_present, 1));
      mbsfn_sf_cfg_list_v1430.set_present(mbsfn_sf_cfg_list_v1430_present);
      if (mbsfn_sf_cfg_list_v1430.is_present()) {
        HANDLE_CODE(mbsfn_sf_cfg_list_v1430->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool codeword_one_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(codeword_one_cfg_v1530_present, 1));
      codeword_one_cfg_v1530.set_present(codeword_one_cfg_v1530_present);
      if (codeword_one_cfg_v1530.is_present()) {
        HANDLE_CODE(codeword_one_cfg_v1530->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void pdsch_re_map_qcl_cfg_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pdsch-RE-MappingQCL-ConfigId-r11", pdsch_re_map_qcl_cfg_id_r11);
  if (optional_set_of_fields_r11_present) {
    j.write_fieldname("optionalSetOfFields-r11");
    j.start_obj();
    j.write_str("crs-PortsCount-r11", optional_set_of_fields_r11.crs_ports_count_r11.to_string());
    j.write_int("crs-FreqShift-r11", optional_set_of_fields_r11.crs_freq_shift_r11);
    if (optional_set_of_fields_r11.mbsfn_sf_cfg_list_r11_present) {
      j.write_fieldname("mbsfn-SubframeConfigList-r11");
      optional_set_of_fields_r11.mbsfn_sf_cfg_list_r11.to_json(j);
    }
    j.write_str("pdsch-Start-r11", optional_set_of_fields_r11.pdsch_start_r11.to_string());
    j.end_obj();
  }
  j.write_int("csi-RS-ConfigZPId-r11", csi_rs_cfg_zp_id_r11);
  if (qcl_csi_rs_cfg_nzp_id_r11_present) {
    j.write_int("qcl-CSI-RS-ConfigNZPId-r11", qcl_csi_rs_cfg_nzp_id_r11);
  }
  if (ext) {
    if (mbsfn_sf_cfg_list_v1430.is_present()) {
      j.write_fieldname("mbsfn-SubframeConfigList-v1430");
      mbsfn_sf_cfg_list_v1430->to_json(j);
    }
    if (codeword_one_cfg_v1530.is_present()) {
      j.write_fieldname("codewordOneConfig-v1530");
      codeword_one_cfg_v1530->to_json(j);
    }
  }
  j.end_obj();
}
bool pdsch_re_map_qcl_cfg_r11_s::operator==(const pdsch_re_map_qcl_cfg_r11_s& other) const
{
  return ext == other.ext and pdsch_re_map_qcl_cfg_id_r11 == other.pdsch_re_map_qcl_cfg_id_r11 and
         optional_set_of_fields_r11.crs_ports_count_r11 == other.optional_set_of_fields_r11.crs_ports_count_r11 and
         optional_set_of_fields_r11.crs_freq_shift_r11 == other.optional_set_of_fields_r11.crs_freq_shift_r11 and
         optional_set_of_fields_r11.mbsfn_sf_cfg_list_r11_present ==
             other.optional_set_of_fields_r11.mbsfn_sf_cfg_list_r11_present and
         (not optional_set_of_fields_r11.mbsfn_sf_cfg_list_r11_present or
          optional_set_of_fields_r11.mbsfn_sf_cfg_list_r11 ==
              other.optional_set_of_fields_r11.mbsfn_sf_cfg_list_r11) and
         optional_set_of_fields_r11.pdsch_start_r11 == other.optional_set_of_fields_r11.pdsch_start_r11 and
         csi_rs_cfg_zp_id_r11 == other.csi_rs_cfg_zp_id_r11 and
         qcl_csi_rs_cfg_nzp_id_r11_present == other.qcl_csi_rs_cfg_nzp_id_r11_present and
         (not qcl_csi_rs_cfg_nzp_id_r11_present or qcl_csi_rs_cfg_nzp_id_r11 == other.qcl_csi_rs_cfg_nzp_id_r11) and
         (not ext or
          (mbsfn_sf_cfg_list_v1430.is_present() == other.mbsfn_sf_cfg_list_v1430.is_present() and
           (not mbsfn_sf_cfg_list_v1430.is_present() or *mbsfn_sf_cfg_list_v1430 == *other.mbsfn_sf_cfg_list_v1430) and
           codeword_one_cfg_v1530.is_present() == other.codeword_one_cfg_v1530.is_present() and
           (not codeword_one_cfg_v1530.is_present() or *codeword_one_cfg_v1530 == *other.codeword_one_cfg_v1530)));
}

const char* pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::crs_ports_count_r11_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "spare1"};
  return convert_enum_idx(
      options, 4, value, "pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::crs_ports_count_r11_e_");
}
uint8_t pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::crs_ports_count_r11_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(
      options, 3, value, "pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::crs_ports_count_r11_e_");
}

void pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::mbsfn_sf_cfg_list_r11_c_::set(types::options e)
{
  type_ = e;
}
void pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::mbsfn_sf_cfg_list_r11_c_::set_release()
{
  set(types::release);
}
pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::mbsfn_sf_cfg_list_r11_c_::setup_s_&
pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::mbsfn_sf_cfg_list_r11_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::mbsfn_sf_cfg_list_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("subframeConfigList");
      for (const auto& e1 : c.sf_cfg_list) {
        e1.to_json(j);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_,
                            "pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::mbsfn_sf_cfg_list_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE
pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::mbsfn_sf_cfg_list_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.sf_cfg_list, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_,
                            "pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::mbsfn_sf_cfg_list_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::mbsfn_sf_cfg_list_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.sf_cfg_list, bref, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_,
                            "pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::mbsfn_sf_cfg_list_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::mbsfn_sf_cfg_list_r11_c_::operator==(
    const mbsfn_sf_cfg_list_r11_c_& other) const
{
  return type() == other.type() and c.sf_cfg_list == other.c.sf_cfg_list;
}

const char* pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::pdsch_start_r11_opts::to_string() const
{
  static const char* options[] = {"reserved", "n1", "n2", "n3", "n4", "assigned"};
  return convert_enum_idx(
      options, 6, value, "pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::pdsch_start_r11_e_");
}
uint8_t pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::pdsch_start_r11_opts::to_number() const
{
  switch (value) {
    case n1:
      return 1;
    case n2:
      return 2;
    case n3:
      return 3;
    case n4:
      return 4;
    default:
      invalid_enum_number(value, "pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::pdsch_start_r11_e_");
  }
  return 0;
}

void pdsch_re_map_qcl_cfg_r11_s::mbsfn_sf_cfg_list_v1430_c_::set(types::options e)
{
  type_ = e;
}
void pdsch_re_map_qcl_cfg_r11_s::mbsfn_sf_cfg_list_v1430_c_::set_release()
{
  set(types::release);
}
pdsch_re_map_qcl_cfg_r11_s::mbsfn_sf_cfg_list_v1430_c_::setup_s_&
pdsch_re_map_qcl_cfg_r11_s::mbsfn_sf_cfg_list_v1430_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pdsch_re_map_qcl_cfg_r11_s::mbsfn_sf_cfg_list_v1430_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("subframeConfigList-v1430");
      for (const auto& e1 : c.sf_cfg_list_v1430) {
        e1.to_json(j);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_re_map_qcl_cfg_r11_s::mbsfn_sf_cfg_list_v1430_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdsch_re_map_qcl_cfg_r11_s::mbsfn_sf_cfg_list_v1430_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.sf_cfg_list_v1430, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_re_map_qcl_cfg_r11_s::mbsfn_sf_cfg_list_v1430_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_re_map_qcl_cfg_r11_s::mbsfn_sf_cfg_list_v1430_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.sf_cfg_list_v1430, bref, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_re_map_qcl_cfg_r11_s::mbsfn_sf_cfg_list_v1430_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pdsch_re_map_qcl_cfg_r11_s::mbsfn_sf_cfg_list_v1430_c_::operator==(const mbsfn_sf_cfg_list_v1430_c_& other) const
{
  return type() == other.type() and c.sf_cfg_list_v1430 == other.c.sf_cfg_list_v1430;
}

void pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::set(types::options e)
{
  type_ = e;
}
void pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::set_release()
{
  set(types::release);
}
pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_&
pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("crs-PortsCount-v1530", c.crs_ports_count_v1530.to_string());
      j.write_int("crs-FreqShift-v1530", c.crs_freq_shift_v1530);
      if (c.mbsfn_sf_cfg_list_v1530_present) {
        j.start_array("mbsfn-SubframeConfigList-v1530");
        for (const auto& e1 : c.mbsfn_sf_cfg_list_v1530) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (c.mbsfn_sf_cfg_list_ext_v1530_present) {
        j.start_array("mbsfn-SubframeConfigListExt-v1530");
        for (const auto& e1 : c.mbsfn_sf_cfg_list_ext_v1530) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.write_str("pdsch-Start-v1530", c.pdsch_start_v1530.to_string());
      j.write_int("csi-RS-ConfigZPId-v1530", c.csi_rs_cfg_zp_id_v1530);
      if (c.qcl_csi_rs_cfg_nzp_id_v1530_present) {
        j.write_int("qcl-CSI-RS-ConfigNZPId-v1530", c.qcl_csi_rs_cfg_nzp_id_v1530);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.mbsfn_sf_cfg_list_v1530_present, 1));
      HANDLE_CODE(bref.pack(c.mbsfn_sf_cfg_list_ext_v1530_present, 1));
      HANDLE_CODE(bref.pack(c.qcl_csi_rs_cfg_nzp_id_v1530_present, 1));
      HANDLE_CODE(c.crs_ports_count_v1530.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.crs_freq_shift_v1530, (uint8_t)0u, (uint8_t)5u));
      if (c.mbsfn_sf_cfg_list_v1530_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.mbsfn_sf_cfg_list_v1530, 1, 8));
      }
      if (c.mbsfn_sf_cfg_list_ext_v1530_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.mbsfn_sf_cfg_list_ext_v1530, 1, 8));
      }
      HANDLE_CODE(c.pdsch_start_v1530.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.csi_rs_cfg_zp_id_v1530, (uint8_t)1u, (uint8_t)4u));
      if (c.qcl_csi_rs_cfg_nzp_id_v1530_present) {
        HANDLE_CODE(pack_integer(bref, c.qcl_csi_rs_cfg_nzp_id_v1530, (uint8_t)1u, (uint8_t)3u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.mbsfn_sf_cfg_list_v1530_present, 1));
      HANDLE_CODE(bref.unpack(c.mbsfn_sf_cfg_list_ext_v1530_present, 1));
      HANDLE_CODE(bref.unpack(c.qcl_csi_rs_cfg_nzp_id_v1530_present, 1));
      HANDLE_CODE(c.crs_ports_count_v1530.unpack(bref));
      HANDLE_CODE(unpack_integer(c.crs_freq_shift_v1530, bref, (uint8_t)0u, (uint8_t)5u));
      if (c.mbsfn_sf_cfg_list_v1530_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.mbsfn_sf_cfg_list_v1530, bref, 1, 8));
      }
      if (c.mbsfn_sf_cfg_list_ext_v1530_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.mbsfn_sf_cfg_list_ext_v1530, bref, 1, 8));
      }
      HANDLE_CODE(c.pdsch_start_v1530.unpack(bref));
      HANDLE_CODE(unpack_integer(c.csi_rs_cfg_zp_id_v1530, bref, (uint8_t)1u, (uint8_t)4u));
      if (c.qcl_csi_rs_cfg_nzp_id_v1530_present) {
        HANDLE_CODE(unpack_integer(c.qcl_csi_rs_cfg_nzp_id_v1530, bref, (uint8_t)1u, (uint8_t)3u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::operator==(const codeword_one_cfg_v1530_c_& other) const
{
  return type() == other.type() and c.crs_ports_count_v1530 == other.c.crs_ports_count_v1530 and
         c.crs_freq_shift_v1530 == other.c.crs_freq_shift_v1530 and
         c.mbsfn_sf_cfg_list_v1530_present == other.c.mbsfn_sf_cfg_list_v1530_present and
         (not c.mbsfn_sf_cfg_list_v1530_present or c.mbsfn_sf_cfg_list_v1530 == other.c.mbsfn_sf_cfg_list_v1530) and
         c.mbsfn_sf_cfg_list_ext_v1530_present == other.c.mbsfn_sf_cfg_list_ext_v1530_present and
         (not c.mbsfn_sf_cfg_list_ext_v1530_present or
          c.mbsfn_sf_cfg_list_ext_v1530 == other.c.mbsfn_sf_cfg_list_ext_v1530) and
         c.pdsch_start_v1530 == other.c.pdsch_start_v1530 and
         c.csi_rs_cfg_zp_id_v1530 == other.c.csi_rs_cfg_zp_id_v1530 and
         c.qcl_csi_rs_cfg_nzp_id_v1530_present == other.c.qcl_csi_rs_cfg_nzp_id_v1530_present and
         (not c.qcl_csi_rs_cfg_nzp_id_v1530_present or
          c.qcl_csi_rs_cfg_nzp_id_v1530 == other.c.qcl_csi_rs_cfg_nzp_id_v1530);
}

const char*
pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::crs_ports_count_v1530_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "spare1"};
  return convert_enum_idx(
      options, 4, value, "pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::crs_ports_count_v1530_e_");
}
uint8_t pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::crs_ports_count_v1530_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(
      options, 3, value, "pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::crs_ports_count_v1530_e_");
}

const char* pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::pdsch_start_v1530_opts::to_string() const
{
  static const char* options[] = {"reserved", "n1", "n2", "n3", "n4", "assigned"};
  return convert_enum_idx(
      options, 6, value, "pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::pdsch_start_v1530_e_");
}
uint8_t pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::pdsch_start_v1530_opts::to_number() const
{
  switch (value) {
    case n1:
      return 1;
    case n2:
      return 2;
    case n3:
      return 3;
    case n4:
      return 4;
    default:
      invalid_enum_number(value,
                          "pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::pdsch_start_v1530_e_");
  }
  return 0;
}

// SRS-AntennaPort ::= ENUMERATED
const char* srs_ant_port_opts::to_string() const
{
  static const char* options[] = {"an1", "an2", "an4", "spare1"};
  return convert_enum_idx(options, 4, value, "srs_ant_port_e");
}
uint8_t srs_ant_port_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "srs_ant_port_e");
}

// ShortTTI-Length-r15 ::= ENUMERATED
const char* short_tti_len_r15_opts::to_string() const
{
  static const char* options[] = {"slot", "subslot"};
  return convert_enum_idx(options, 2, value, "short_tti_len_r15_e");
}

// TPC-PDCCH-Config ::= CHOICE
void tpc_pdcch_cfg_c::set(types::options e)
{
  type_ = e;
}
void tpc_pdcch_cfg_c::set_release()
{
  set(types::release);
}
tpc_pdcch_cfg_c::setup_s_& tpc_pdcch_cfg_c::set_setup()
{
  set(types::setup);
  return c;
}
void tpc_pdcch_cfg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("tpc-RNTI", c.tpc_rnti.to_string());
      j.write_fieldname("tpc-Index");
      c.tpc_idx.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "tpc_pdcch_cfg_c");
  }
  j.end_obj();
}
SRSASN_CODE tpc_pdcch_cfg_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.tpc_rnti.pack(bref));
      HANDLE_CODE(c.tpc_idx.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "tpc_pdcch_cfg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE tpc_pdcch_cfg_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.tpc_rnti.unpack(bref));
      HANDLE_CODE(c.tpc_idx.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "tpc_pdcch_cfg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool tpc_pdcch_cfg_c::operator==(const tpc_pdcch_cfg_c& other) const
{
  return type() == other.type() and c.tpc_rnti == other.c.tpc_rnti and c.tpc_idx == other.c.tpc_idx;
}

// AntennaInfoDedicatedSTTI-r15 ::= CHOICE
void ant_info_ded_stti_r15_c::set(types::options e)
{
  type_ = e;
}
void ant_info_ded_stti_r15_c::set_release()
{
  set(types::release);
}
ant_info_ded_stti_r15_c::setup_s_& ant_info_ded_stti_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void ant_info_ded_stti_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.tx_mode_dl_mbsfn_r15_present) {
        j.write_str("transmissionModeDL-MBSFN-r15", c.tx_mode_dl_mbsfn_r15.to_string());
      }
      if (c.tx_mode_dl_non_mbsfn_r15_present) {
        j.write_str("transmissionModeDL-nonMBSFN-r15", c.tx_mode_dl_non_mbsfn_r15.to_string());
      }
      if (c.codebook_subset_restrict_present) {
        j.write_fieldname("codebookSubsetRestriction");
        c.codebook_subset_restrict.to_json(j);
      }
      if (c.max_layers_mimo_stti_r15_present) {
        j.write_str("maxLayersMIMO-STTI-r15", c.max_layers_mimo_stti_r15.to_string());
      }
      j.write_bool("slotSubslotPDSCH-TxDiv-2Layer-r15", c.slot_subslot_pdsch_tx_div_minus2_layer_r15);
      j.write_bool("slotSubslotPDSCH-TxDiv-4Layer-r15", c.slot_subslot_pdsch_tx_div_minus4_layer_r15);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_stti_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE ant_info_ded_stti_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.tx_mode_dl_mbsfn_r15_present, 1));
      HANDLE_CODE(bref.pack(c.tx_mode_dl_non_mbsfn_r15_present, 1));
      HANDLE_CODE(bref.pack(c.codebook_subset_restrict_present, 1));
      HANDLE_CODE(bref.pack(c.max_layers_mimo_stti_r15_present, 1));
      if (c.tx_mode_dl_mbsfn_r15_present) {
        HANDLE_CODE(c.tx_mode_dl_mbsfn_r15.pack(bref));
      }
      if (c.tx_mode_dl_non_mbsfn_r15_present) {
        HANDLE_CODE(c.tx_mode_dl_non_mbsfn_r15.pack(bref));
      }
      if (c.codebook_subset_restrict_present) {
        HANDLE_CODE(c.codebook_subset_restrict.pack(bref));
      }
      if (c.max_layers_mimo_stti_r15_present) {
        HANDLE_CODE(c.max_layers_mimo_stti_r15.pack(bref));
      }
      HANDLE_CODE(bref.pack(c.slot_subslot_pdsch_tx_div_minus2_layer_r15, 1));
      HANDLE_CODE(bref.pack(c.slot_subslot_pdsch_tx_div_minus4_layer_r15, 1));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_stti_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ded_stti_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.tx_mode_dl_mbsfn_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.tx_mode_dl_non_mbsfn_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.codebook_subset_restrict_present, 1));
      HANDLE_CODE(bref.unpack(c.max_layers_mimo_stti_r15_present, 1));
      if (c.tx_mode_dl_mbsfn_r15_present) {
        HANDLE_CODE(c.tx_mode_dl_mbsfn_r15.unpack(bref));
      }
      if (c.tx_mode_dl_non_mbsfn_r15_present) {
        HANDLE_CODE(c.tx_mode_dl_non_mbsfn_r15.unpack(bref));
      }
      if (c.codebook_subset_restrict_present) {
        HANDLE_CODE(c.codebook_subset_restrict.unpack(bref));
      }
      if (c.max_layers_mimo_stti_r15_present) {
        HANDLE_CODE(c.max_layers_mimo_stti_r15.unpack(bref));
      }
      HANDLE_CODE(bref.unpack(c.slot_subslot_pdsch_tx_div_minus2_layer_r15, 1));
      HANDLE_CODE(bref.unpack(c.slot_subslot_pdsch_tx_div_minus4_layer_r15, 1));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_stti_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool ant_info_ded_stti_r15_c::operator==(const ant_info_ded_stti_r15_c& other) const
{
  return type() == other.type() and c.tx_mode_dl_mbsfn_r15_present == other.c.tx_mode_dl_mbsfn_r15_present and
         (not c.tx_mode_dl_mbsfn_r15_present or c.tx_mode_dl_mbsfn_r15 == other.c.tx_mode_dl_mbsfn_r15) and
         c.tx_mode_dl_non_mbsfn_r15_present == other.c.tx_mode_dl_non_mbsfn_r15_present and
         (not c.tx_mode_dl_non_mbsfn_r15_present or c.tx_mode_dl_non_mbsfn_r15 == other.c.tx_mode_dl_non_mbsfn_r15) and
         c.codebook_subset_restrict_present == other.c.codebook_subset_restrict_present and
         (not c.codebook_subset_restrict_present or c.codebook_subset_restrict == other.c.codebook_subset_restrict) and
         c.max_layers_mimo_stti_r15_present == other.c.max_layers_mimo_stti_r15_present and
         (not c.max_layers_mimo_stti_r15_present or c.max_layers_mimo_stti_r15 == other.c.max_layers_mimo_stti_r15) and
         c.slot_subslot_pdsch_tx_div_minus2_layer_r15 == other.c.slot_subslot_pdsch_tx_div_minus2_layer_r15 and
         c.slot_subslot_pdsch_tx_div_minus4_layer_r15 == other.c.slot_subslot_pdsch_tx_div_minus4_layer_r15;
}

const char* ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_mbsfn_r15_opts::to_string() const
{
  static const char* options[] = {"tm9", "tm10"};
  return convert_enum_idx(options, 2, value, "ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_mbsfn_r15_e_");
}
uint8_t ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_mbsfn_r15_opts::to_number() const
{
  static const uint8_t options[] = {9, 10};
  return map_enum_number(options, 2, value, "ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_mbsfn_r15_e_");
}

const char* ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_non_mbsfn_r15_opts::to_string() const
{
  static const char* options[] = {"tm1", "tm2", "tm3", "tm4", "tm6", "tm8", "tm9", "tm10"};
  return convert_enum_idx(options, 8, value, "ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_non_mbsfn_r15_e_");
}
uint8_t ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_non_mbsfn_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 9, 10};
  return map_enum_number(options, 8, value, "ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_non_mbsfn_r15_e_");
}

void ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::destroy_()
{
  switch (type_) {
    case types::n2_tx_ant_tm3_r15:
      c.destroy<fixed_bitstring<2> >();
      break;
    case types::n4_tx_ant_tm3_r15:
      c.destroy<fixed_bitstring<4> >();
      break;
    case types::n2_tx_ant_tm4_r15:
      c.destroy<fixed_bitstring<6> >();
      break;
    case types::n4_tx_ant_tm4_r15:
      c.destroy<fixed_bitstring<64> >();
      break;
    case types::n2_tx_ant_tm5_r15:
      c.destroy<fixed_bitstring<4> >();
      break;
    case types::n4_tx_ant_tm5_r15:
      c.destroy<fixed_bitstring<16> >();
      break;
    case types::n2_tx_ant_tm6_r15:
      c.destroy<fixed_bitstring<4> >();
      break;
    case types::n4_tx_ant_tm6_r15:
      c.destroy<fixed_bitstring<16> >();
      break;
    case types::n2_tx_ant_tm8_r15:
      c.destroy<fixed_bitstring<6> >();
      break;
    case types::n4_tx_ant_tm8_r15:
      c.destroy<fixed_bitstring<64> >();
      break;
    case types::n2_tx_ant_tm9and10_r15:
      c.destroy<fixed_bitstring<6> >();
      break;
    case types::n4_tx_ant_tm9and10_r15:
      c.destroy<fixed_bitstring<96> >();
      break;
    case types::n8_tx_ant_tm9and10_r15:
      c.destroy<fixed_bitstring<109> >();
      break;
    default:
      break;
  }
}
void ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::n2_tx_ant_tm3_r15:
      c.init<fixed_bitstring<2> >();
      break;
    case types::n4_tx_ant_tm3_r15:
      c.init<fixed_bitstring<4> >();
      break;
    case types::n2_tx_ant_tm4_r15:
      c.init<fixed_bitstring<6> >();
      break;
    case types::n4_tx_ant_tm4_r15:
      c.init<fixed_bitstring<64> >();
      break;
    case types::n2_tx_ant_tm5_r15:
      c.init<fixed_bitstring<4> >();
      break;
    case types::n4_tx_ant_tm5_r15:
      c.init<fixed_bitstring<16> >();
      break;
    case types::n2_tx_ant_tm6_r15:
      c.init<fixed_bitstring<4> >();
      break;
    case types::n4_tx_ant_tm6_r15:
      c.init<fixed_bitstring<16> >();
      break;
    case types::n2_tx_ant_tm8_r15:
      c.init<fixed_bitstring<6> >();
      break;
    case types::n4_tx_ant_tm8_r15:
      c.init<fixed_bitstring<64> >();
      break;
    case types::n2_tx_ant_tm9and10_r15:
      c.init<fixed_bitstring<6> >();
      break;
    case types::n4_tx_ant_tm9and10_r15:
      c.init<fixed_bitstring<96> >();
      break;
    case types::n8_tx_ant_tm9and10_r15:
      c.init<fixed_bitstring<109> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_");
  }
}
ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::codebook_subset_restrict_c_(
    const ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::n2_tx_ant_tm3_r15:
      c.init(other.c.get<fixed_bitstring<2> >());
      break;
    case types::n4_tx_ant_tm3_r15:
      c.init(other.c.get<fixed_bitstring<4> >());
      break;
    case types::n2_tx_ant_tm4_r15:
      c.init(other.c.get<fixed_bitstring<6> >());
      break;
    case types::n4_tx_ant_tm4_r15:
      c.init(other.c.get<fixed_bitstring<64> >());
      break;
    case types::n2_tx_ant_tm5_r15:
      c.init(other.c.get<fixed_bitstring<4> >());
      break;
    case types::n4_tx_ant_tm5_r15:
      c.init(other.c.get<fixed_bitstring<16> >());
      break;
    case types::n2_tx_ant_tm6_r15:
      c.init(other.c.get<fixed_bitstring<4> >());
      break;
    case types::n4_tx_ant_tm6_r15:
      c.init(other.c.get<fixed_bitstring<16> >());
      break;
    case types::n2_tx_ant_tm8_r15:
      c.init(other.c.get<fixed_bitstring<6> >());
      break;
    case types::n4_tx_ant_tm8_r15:
      c.init(other.c.get<fixed_bitstring<64> >());
      break;
    case types::n2_tx_ant_tm9and10_r15:
      c.init(other.c.get<fixed_bitstring<6> >());
      break;
    case types::n4_tx_ant_tm9and10_r15:
      c.init(other.c.get<fixed_bitstring<96> >());
      break;
    case types::n8_tx_ant_tm9and10_r15:
      c.init(other.c.get<fixed_bitstring<109> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_");
  }
}
ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_&
ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::operator=(
    const ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::n2_tx_ant_tm3_r15:
      c.set(other.c.get<fixed_bitstring<2> >());
      break;
    case types::n4_tx_ant_tm3_r15:
      c.set(other.c.get<fixed_bitstring<4> >());
      break;
    case types::n2_tx_ant_tm4_r15:
      c.set(other.c.get<fixed_bitstring<6> >());
      break;
    case types::n4_tx_ant_tm4_r15:
      c.set(other.c.get<fixed_bitstring<64> >());
      break;
    case types::n2_tx_ant_tm5_r15:
      c.set(other.c.get<fixed_bitstring<4> >());
      break;
    case types::n4_tx_ant_tm5_r15:
      c.set(other.c.get<fixed_bitstring<16> >());
      break;
    case types::n2_tx_ant_tm6_r15:
      c.set(other.c.get<fixed_bitstring<4> >());
      break;
    case types::n4_tx_ant_tm6_r15:
      c.set(other.c.get<fixed_bitstring<16> >());
      break;
    case types::n2_tx_ant_tm8_r15:
      c.set(other.c.get<fixed_bitstring<6> >());
      break;
    case types::n4_tx_ant_tm8_r15:
      c.set(other.c.get<fixed_bitstring<64> >());
      break;
    case types::n2_tx_ant_tm9and10_r15:
      c.set(other.c.get<fixed_bitstring<6> >());
      break;
    case types::n4_tx_ant_tm9and10_r15:
      c.set(other.c.get<fixed_bitstring<96> >());
      break;
    case types::n8_tx_ant_tm9and10_r15:
      c.set(other.c.get<fixed_bitstring<109> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_");
  }

  return *this;
}
fixed_bitstring<2>& ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::set_n2_tx_ant_tm3_r15()
{
  set(types::n2_tx_ant_tm3_r15);
  return c.get<fixed_bitstring<2> >();
}
fixed_bitstring<4>& ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::set_n4_tx_ant_tm3_r15()
{
  set(types::n4_tx_ant_tm3_r15);
  return c.get<fixed_bitstring<4> >();
}
fixed_bitstring<6>& ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::set_n2_tx_ant_tm4_r15()
{
  set(types::n2_tx_ant_tm4_r15);
  return c.get<fixed_bitstring<6> >();
}
fixed_bitstring<64>& ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::set_n4_tx_ant_tm4_r15()
{
  set(types::n4_tx_ant_tm4_r15);
  return c.get<fixed_bitstring<64> >();
}
fixed_bitstring<4>& ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::set_n2_tx_ant_tm5_r15()
{
  set(types::n2_tx_ant_tm5_r15);
  return c.get<fixed_bitstring<4> >();
}
fixed_bitstring<16>& ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::set_n4_tx_ant_tm5_r15()
{
  set(types::n4_tx_ant_tm5_r15);
  return c.get<fixed_bitstring<16> >();
}
fixed_bitstring<4>& ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::set_n2_tx_ant_tm6_r15()
{
  set(types::n2_tx_ant_tm6_r15);
  return c.get<fixed_bitstring<4> >();
}
fixed_bitstring<16>& ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::set_n4_tx_ant_tm6_r15()
{
  set(types::n4_tx_ant_tm6_r15);
  return c.get<fixed_bitstring<16> >();
}
fixed_bitstring<6>& ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::set_n2_tx_ant_tm8_r15()
{
  set(types::n2_tx_ant_tm8_r15);
  return c.get<fixed_bitstring<6> >();
}
fixed_bitstring<64>& ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::set_n4_tx_ant_tm8_r15()
{
  set(types::n4_tx_ant_tm8_r15);
  return c.get<fixed_bitstring<64> >();
}
fixed_bitstring<6>& ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::set_n2_tx_ant_tm9and10_r15()
{
  set(types::n2_tx_ant_tm9and10_r15);
  return c.get<fixed_bitstring<6> >();
}
fixed_bitstring<96>& ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::set_n4_tx_ant_tm9and10_r15()
{
  set(types::n4_tx_ant_tm9and10_r15);
  return c.get<fixed_bitstring<96> >();
}
fixed_bitstring<109>& ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::set_n8_tx_ant_tm9and10_r15()
{
  set(types::n8_tx_ant_tm9and10_r15);
  return c.get<fixed_bitstring<109> >();
}
void ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::n2_tx_ant_tm3_r15:
      j.write_str("n2TxAntenna-tm3-r15", c.get<fixed_bitstring<2> >().to_string());
      break;
    case types::n4_tx_ant_tm3_r15:
      j.write_str("n4TxAntenna-tm3-r15", c.get<fixed_bitstring<4> >().to_string());
      break;
    case types::n2_tx_ant_tm4_r15:
      j.write_str("n2TxAntenna-tm4-r15", c.get<fixed_bitstring<6> >().to_string());
      break;
    case types::n4_tx_ant_tm4_r15:
      j.write_str("n4TxAntenna-tm4-r15", c.get<fixed_bitstring<64> >().to_string());
      break;
    case types::n2_tx_ant_tm5_r15:
      j.write_str("n2TxAntenna-tm5-r15", c.get<fixed_bitstring<4> >().to_string());
      break;
    case types::n4_tx_ant_tm5_r15:
      j.write_str("n4TxAntenna-tm5-r15", c.get<fixed_bitstring<16> >().to_string());
      break;
    case types::n2_tx_ant_tm6_r15:
      j.write_str("n2TxAntenna-tm6-r15", c.get<fixed_bitstring<4> >().to_string());
      break;
    case types::n4_tx_ant_tm6_r15:
      j.write_str("n4TxAntenna-tm6-r15", c.get<fixed_bitstring<16> >().to_string());
      break;
    case types::n2_tx_ant_tm8_r15:
      j.write_str("n2TxAntenna-tm8-r15", c.get<fixed_bitstring<6> >().to_string());
      break;
    case types::n4_tx_ant_tm8_r15:
      j.write_str("n4TxAntenna-tm8-r15", c.get<fixed_bitstring<64> >().to_string());
      break;
    case types::n2_tx_ant_tm9and10_r15:
      j.write_str("n2TxAntenna-tm9and10-r15", c.get<fixed_bitstring<6> >().to_string());
      break;
    case types::n4_tx_ant_tm9and10_r15:
      j.write_str("n4TxAntenna-tm9and10-r15", c.get<fixed_bitstring<96> >().to_string());
      break;
    case types::n8_tx_ant_tm9and10_r15:
      j.write_str("n8TxAntenna-tm9and10-r15", c.get<fixed_bitstring<109> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_");
  }
  j.end_obj();
}
SRSASN_CODE ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::n2_tx_ant_tm3_r15:
      HANDLE_CODE(c.get<fixed_bitstring<2> >().pack(bref));
      break;
    case types::n4_tx_ant_tm3_r15:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().pack(bref));
      break;
    case types::n2_tx_ant_tm4_r15:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().pack(bref));
      break;
    case types::n4_tx_ant_tm4_r15:
      HANDLE_CODE(c.get<fixed_bitstring<64> >().pack(bref));
      break;
    case types::n2_tx_ant_tm5_r15:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().pack(bref));
      break;
    case types::n4_tx_ant_tm5_r15:
      HANDLE_CODE(c.get<fixed_bitstring<16> >().pack(bref));
      break;
    case types::n2_tx_ant_tm6_r15:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().pack(bref));
      break;
    case types::n4_tx_ant_tm6_r15:
      HANDLE_CODE(c.get<fixed_bitstring<16> >().pack(bref));
      break;
    case types::n2_tx_ant_tm8_r15:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().pack(bref));
      break;
    case types::n4_tx_ant_tm8_r15:
      HANDLE_CODE(c.get<fixed_bitstring<64> >().pack(bref));
      break;
    case types::n2_tx_ant_tm9and10_r15:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().pack(bref));
      break;
    case types::n4_tx_ant_tm9and10_r15:
      HANDLE_CODE(c.get<fixed_bitstring<96> >().pack(bref));
      break;
    case types::n8_tx_ant_tm9and10_r15:
      HANDLE_CODE(c.get<fixed_bitstring<109> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::n2_tx_ant_tm3_r15:
      HANDLE_CODE(c.get<fixed_bitstring<2> >().unpack(bref));
      break;
    case types::n4_tx_ant_tm3_r15:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().unpack(bref));
      break;
    case types::n2_tx_ant_tm4_r15:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().unpack(bref));
      break;
    case types::n4_tx_ant_tm4_r15:
      HANDLE_CODE(c.get<fixed_bitstring<64> >().unpack(bref));
      break;
    case types::n2_tx_ant_tm5_r15:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().unpack(bref));
      break;
    case types::n4_tx_ant_tm5_r15:
      HANDLE_CODE(c.get<fixed_bitstring<16> >().unpack(bref));
      break;
    case types::n2_tx_ant_tm6_r15:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().unpack(bref));
      break;
    case types::n4_tx_ant_tm6_r15:
      HANDLE_CODE(c.get<fixed_bitstring<16> >().unpack(bref));
      break;
    case types::n2_tx_ant_tm8_r15:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().unpack(bref));
      break;
    case types::n4_tx_ant_tm8_r15:
      HANDLE_CODE(c.get<fixed_bitstring<64> >().unpack(bref));
      break;
    case types::n2_tx_ant_tm9and10_r15:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().unpack(bref));
      break;
    case types::n4_tx_ant_tm9and10_r15:
      HANDLE_CODE(c.get<fixed_bitstring<96> >().unpack(bref));
      break;
    case types::n8_tx_ant_tm9and10_r15:
      HANDLE_CODE(c.get<fixed_bitstring<109> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::operator==(
    const codebook_subset_restrict_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::n2_tx_ant_tm3_r15:
      return c.get<fixed_bitstring<2> >() == other.c.get<fixed_bitstring<2> >();
    case types::n4_tx_ant_tm3_r15:
      return c.get<fixed_bitstring<4> >() == other.c.get<fixed_bitstring<4> >();
    case types::n2_tx_ant_tm4_r15:
      return c.get<fixed_bitstring<6> >() == other.c.get<fixed_bitstring<6> >();
    case types::n4_tx_ant_tm4_r15:
      return c.get<fixed_bitstring<64> >() == other.c.get<fixed_bitstring<64> >();
    case types::n2_tx_ant_tm5_r15:
      return c.get<fixed_bitstring<4> >() == other.c.get<fixed_bitstring<4> >();
    case types::n4_tx_ant_tm5_r15:
      return c.get<fixed_bitstring<16> >() == other.c.get<fixed_bitstring<16> >();
    case types::n2_tx_ant_tm6_r15:
      return c.get<fixed_bitstring<4> >() == other.c.get<fixed_bitstring<4> >();
    case types::n4_tx_ant_tm6_r15:
      return c.get<fixed_bitstring<16> >() == other.c.get<fixed_bitstring<16> >();
    case types::n2_tx_ant_tm8_r15:
      return c.get<fixed_bitstring<6> >() == other.c.get<fixed_bitstring<6> >();
    case types::n4_tx_ant_tm8_r15:
      return c.get<fixed_bitstring<64> >() == other.c.get<fixed_bitstring<64> >();
    case types::n2_tx_ant_tm9and10_r15:
      return c.get<fixed_bitstring<6> >() == other.c.get<fixed_bitstring<6> >();
    case types::n4_tx_ant_tm9and10_r15:
      return c.get<fixed_bitstring<96> >() == other.c.get<fixed_bitstring<96> >();
    case types::n8_tx_ant_tm9and10_r15:
      return c.get<fixed_bitstring<109> >() == other.c.get<fixed_bitstring<109> >();
    default:
      return true;
  }
  return true;
}

const char* ant_info_ded_stti_r15_c::setup_s_::max_layers_mimo_stti_r15_opts::to_string() const
{
  static const char* options[] = {"twoLayers", "fourLayers"};
  return convert_enum_idx(options, 2, value, "ant_info_ded_stti_r15_c::setup_s_::max_layers_mimo_stti_r15_e_");
}
uint8_t ant_info_ded_stti_r15_c::setup_s_::max_layers_mimo_stti_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "ant_info_ded_stti_r15_c::setup_s_::max_layers_mimo_stti_r15_e_");
}

// AntennaInfoUL-STTI-r15 ::= SEQUENCE
SRSASN_CODE ant_info_ul_stti_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tx_mode_ul_stti_r15_present, 1));

  if (tx_mode_ul_stti_r15_present) {
    HANDLE_CODE(tx_mode_ul_stti_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ul_stti_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tx_mode_ul_stti_r15_present, 1));

  if (tx_mode_ul_stti_r15_present) {
    HANDLE_CODE(tx_mode_ul_stti_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ant_info_ul_stti_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tx_mode_ul_stti_r15_present) {
    j.write_str("transmissionModeUL-STTI-r15", tx_mode_ul_stti_r15.to_string());
  }
  j.end_obj();
}
bool ant_info_ul_stti_r15_s::operator==(const ant_info_ul_stti_r15_s& other) const
{
  return tx_mode_ul_stti_r15_present == other.tx_mode_ul_stti_r15_present and
         (not tx_mode_ul_stti_r15_present or tx_mode_ul_stti_r15 == other.tx_mode_ul_stti_r15);
}

const char* ant_info_ul_stti_r15_s::tx_mode_ul_stti_r15_opts::to_string() const
{
  static const char* options[] = {"tm1", "tm2"};
  return convert_enum_idx(options, 2, value, "ant_info_ul_stti_r15_s::tx_mode_ul_stti_r15_e_");
}
uint8_t ant_info_ul_stti_r15_s::tx_mode_ul_stti_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "ant_info_ul_stti_r15_s::tx_mode_ul_stti_r15_e_");
}

// CE-PDSCH-14HARQ-Config-r17 ::= SEQUENCE
SRSASN_CODE ce_pdsch_minus14_harq_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ce_harq_ack_delay_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ce_pdsch_minus14_harq_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ce_harq_ack_delay_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void ce_pdsch_minus14_harq_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ce-HARQ-AckDelay-r17", ce_harq_ack_delay_r17.to_string());
  j.end_obj();
}

const char* ce_pdsch_minus14_harq_cfg_r17_s::ce_harq_ack_delay_r17_opts::to_string() const
{
  static const char* options[] = {"alt-1", "alt-2e"};
  return convert_enum_idx(options, 2, value, "ce_pdsch_minus14_harq_cfg_r17_s::ce_harq_ack_delay_r17_e_");
}
int8_t ce_pdsch_minus14_harq_cfg_r17_s::ce_harq_ack_delay_r17_opts::to_number() const
{
  static const int8_t options[] = {-1, -2};
  return map_enum_number(options, 2, value, "ce_pdsch_minus14_harq_cfg_r17_s::ce_harq_ack_delay_r17_e_");
}

// CE-PDSCH-MultiTB-Config-r16 ::= SEQUENCE
SRSASN_CODE ce_pdsch_multi_tb_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(interleaving_r16_present, 1));
  HANDLE_CODE(bref.pack(harq_ack_bundling_r16_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ce_pdsch_multi_tb_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(interleaving_r16_present, 1));
  HANDLE_CODE(bref.unpack(harq_ack_bundling_r16_present, 1));

  return SRSASN_SUCCESS;
}
void ce_pdsch_multi_tb_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (interleaving_r16_present) {
    j.write_str("interleaving-r16", "on");
  }
  if (harq_ack_bundling_r16_present) {
    j.write_str("harq-AckBundling-r16", "on");
  }
  j.end_obj();
}

// CE-PUSCH-MultiTB-Config-r16 ::= SEQUENCE
SRSASN_CODE ce_pusch_multi_tb_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(interleaving_r16_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ce_pusch_multi_tb_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(interleaving_r16_present, 1));

  return SRSASN_SUCCESS;
}
void ce_pusch_multi_tb_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (interleaving_r16_present) {
    j.write_str("interleaving-r16", "on");
  }
  j.end_obj();
}

// CQI-ReportConfig-r15 ::= CHOICE
void cqi_report_cfg_r15_c::set(types::options e)
{
  type_ = e;
}
void cqi_report_cfg_r15_c::set_release()
{
  set(types::release);
}
cqi_report_cfg_r15_c::setup_s_& cqi_report_cfg_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void cqi_report_cfg_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.cqi_report_cfg_r10_present) {
        j.write_fieldname("cqi-ReportConfig-r10");
        c.cqi_report_cfg_r10.to_json(j);
      }
      if (c.cqi_report_cfg_v1130_present) {
        j.write_fieldname("cqi-ReportConfig-v1130");
        c.cqi_report_cfg_v1130.to_json(j);
      }
      if (c.cqi_report_cfg_pcell_v1250_present) {
        j.write_fieldname("cqi-ReportConfigPCell-v1250");
        c.cqi_report_cfg_pcell_v1250.to_json(j);
      }
      if (c.cqi_report_cfg_v1310_present) {
        j.write_fieldname("cqi-ReportConfig-v1310");
        c.cqi_report_cfg_v1310.to_json(j);
      }
      if (c.cqi_report_cfg_v1320_present) {
        j.write_fieldname("cqi-ReportConfig-v1320");
        c.cqi_report_cfg_v1320.to_json(j);
      }
      if (c.cqi_report_cfg_v1430_present) {
        j.write_fieldname("cqi-ReportConfig-v1430");
        c.cqi_report_cfg_v1430.to_json(j);
      }
      if (c.alt_cqi_table_minus1024_qam_r15_present) {
        j.write_str("altCQI-Table-1024QAM-r15", c.alt_cqi_table_minus1024_qam_r15.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_cfg_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_cfg_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.cqi_report_cfg_r10_present, 1));
      HANDLE_CODE(bref.pack(c.cqi_report_cfg_v1130_present, 1));
      HANDLE_CODE(bref.pack(c.cqi_report_cfg_pcell_v1250_present, 1));
      HANDLE_CODE(bref.pack(c.cqi_report_cfg_v1310_present, 1));
      HANDLE_CODE(bref.pack(c.cqi_report_cfg_v1320_present, 1));
      HANDLE_CODE(bref.pack(c.cqi_report_cfg_v1430_present, 1));
      HANDLE_CODE(bref.pack(c.alt_cqi_table_minus1024_qam_r15_present, 1));
      if (c.cqi_report_cfg_r10_present) {
        HANDLE_CODE(c.cqi_report_cfg_r10.pack(bref));
      }
      if (c.cqi_report_cfg_v1130_present) {
        HANDLE_CODE(c.cqi_report_cfg_v1130.pack(bref));
      }
      if (c.cqi_report_cfg_pcell_v1250_present) {
        HANDLE_CODE(c.cqi_report_cfg_pcell_v1250.pack(bref));
      }
      if (c.cqi_report_cfg_v1310_present) {
        HANDLE_CODE(c.cqi_report_cfg_v1310.pack(bref));
      }
      if (c.cqi_report_cfg_v1320_present) {
        HANDLE_CODE(c.cqi_report_cfg_v1320.pack(bref));
      }
      if (c.cqi_report_cfg_v1430_present) {
        HANDLE_CODE(c.cqi_report_cfg_v1430.pack(bref));
      }
      if (c.alt_cqi_table_minus1024_qam_r15_present) {
        HANDLE_CODE(c.alt_cqi_table_minus1024_qam_r15.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_cfg_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_cfg_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.cqi_report_cfg_r10_present, 1));
      HANDLE_CODE(bref.unpack(c.cqi_report_cfg_v1130_present, 1));
      HANDLE_CODE(bref.unpack(c.cqi_report_cfg_pcell_v1250_present, 1));
      HANDLE_CODE(bref.unpack(c.cqi_report_cfg_v1310_present, 1));
      HANDLE_CODE(bref.unpack(c.cqi_report_cfg_v1320_present, 1));
      HANDLE_CODE(bref.unpack(c.cqi_report_cfg_v1430_present, 1));
      HANDLE_CODE(bref.unpack(c.alt_cqi_table_minus1024_qam_r15_present, 1));
      if (c.cqi_report_cfg_r10_present) {
        HANDLE_CODE(c.cqi_report_cfg_r10.unpack(bref));
      }
      if (c.cqi_report_cfg_v1130_present) {
        HANDLE_CODE(c.cqi_report_cfg_v1130.unpack(bref));
      }
      if (c.cqi_report_cfg_pcell_v1250_present) {
        HANDLE_CODE(c.cqi_report_cfg_pcell_v1250.unpack(bref));
      }
      if (c.cqi_report_cfg_v1310_present) {
        HANDLE_CODE(c.cqi_report_cfg_v1310.unpack(bref));
      }
      if (c.cqi_report_cfg_v1320_present) {
        HANDLE_CODE(c.cqi_report_cfg_v1320.unpack(bref));
      }
      if (c.cqi_report_cfg_v1430_present) {
        HANDLE_CODE(c.cqi_report_cfg_v1430.unpack(bref));
      }
      if (c.alt_cqi_table_minus1024_qam_r15_present) {
        HANDLE_CODE(c.alt_cqi_table_minus1024_qam_r15.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_cfg_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_cfg_r15_c::operator==(const cqi_report_cfg_r15_c& other) const
{
  return type() == other.type() and c.cqi_report_cfg_r10_present == other.c.cqi_report_cfg_r10_present and
         (not c.cqi_report_cfg_r10_present or c.cqi_report_cfg_r10 == other.c.cqi_report_cfg_r10) and
         c.cqi_report_cfg_v1130_present == other.c.cqi_report_cfg_v1130_present and
         (not c.cqi_report_cfg_v1130_present or c.cqi_report_cfg_v1130 == other.c.cqi_report_cfg_v1130) and
         c.cqi_report_cfg_pcell_v1250_present == other.c.cqi_report_cfg_pcell_v1250_present and
         (not c.cqi_report_cfg_pcell_v1250_present or
          c.cqi_report_cfg_pcell_v1250 == other.c.cqi_report_cfg_pcell_v1250) and
         c.cqi_report_cfg_v1310_present == other.c.cqi_report_cfg_v1310_present and
         (not c.cqi_report_cfg_v1310_present or c.cqi_report_cfg_v1310 == other.c.cqi_report_cfg_v1310) and
         c.cqi_report_cfg_v1320_present == other.c.cqi_report_cfg_v1320_present and
         (not c.cqi_report_cfg_v1320_present or c.cqi_report_cfg_v1320 == other.c.cqi_report_cfg_v1320) and
         c.cqi_report_cfg_v1430_present == other.c.cqi_report_cfg_v1430_present and
         (not c.cqi_report_cfg_v1430_present or c.cqi_report_cfg_v1430 == other.c.cqi_report_cfg_v1430) and
         c.alt_cqi_table_minus1024_qam_r15_present == other.c.alt_cqi_table_minus1024_qam_r15_present and
         (not c.alt_cqi_table_minus1024_qam_r15_present or
          c.alt_cqi_table_minus1024_qam_r15 == other.c.alt_cqi_table_minus1024_qam_r15);
}

const char* cqi_report_cfg_r15_c::setup_s_::alt_cqi_table_minus1024_qam_r15_opts::to_string() const
{
  static const char* options[] = {"allSubframes", "csi-SubframeSet1", "csi-SubframeSet2", "spare1"};
  return convert_enum_idx(options, 4, value, "cqi_report_cfg_r15_c::setup_s_::alt_cqi_table_minus1024_qam_r15_e_");
}
uint8_t cqi_report_cfg_r15_c::setup_s_::alt_cqi_table_minus1024_qam_r15_opts::to_number() const
{
  switch (value) {
    case csi_sf_set1:
      return 1;
    case csi_sf_set2:
      return 2;
    default:
      invalid_enum_number(value, "cqi_report_cfg_r15_c::setup_s_::alt_cqi_table_minus1024_qam_r15_e_");
  }
  return 0;
}

// CQI-ReportPeriodic ::= CHOICE
void cqi_report_periodic_c::set(types::options e)
{
  type_ = e;
}
void cqi_report_periodic_c::set_release()
{
  set(types::release);
}
cqi_report_periodic_c::setup_s_& cqi_report_periodic_c::set_setup()
{
  set(types::setup);
  return c;
}
void cqi_report_periodic_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("cqi-PUCCH-ResourceIndex", c.cqi_pucch_res_idx);
      j.write_int("cqi-pmi-ConfigIndex", c.cqi_pmi_cfg_idx);
      j.write_fieldname("cqi-FormatIndicatorPeriodic");
      c.cqi_format_ind_periodic.to_json(j);
      if (c.ri_cfg_idx_present) {
        j.write_int("ri-ConfigIndex", c.ri_cfg_idx);
      }
      j.write_bool("simultaneousAckNackAndCQI", c.simul_ack_nack_and_cqi);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_c");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_periodic_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.ri_cfg_idx_present, 1));
      HANDLE_CODE(pack_integer(bref, c.cqi_pucch_res_idx, (uint16_t)0u, (uint16_t)1185u));
      HANDLE_CODE(pack_integer(bref, c.cqi_pmi_cfg_idx, (uint16_t)0u, (uint16_t)1023u));
      HANDLE_CODE(c.cqi_format_ind_periodic.pack(bref));
      if (c.ri_cfg_idx_present) {
        HANDLE_CODE(pack_integer(bref, c.ri_cfg_idx, (uint16_t)0u, (uint16_t)1023u));
      }
      HANDLE_CODE(bref.pack(c.simul_ack_nack_and_cqi, 1));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.ri_cfg_idx_present, 1));
      HANDLE_CODE(unpack_integer(c.cqi_pucch_res_idx, bref, (uint16_t)0u, (uint16_t)1185u));
      HANDLE_CODE(unpack_integer(c.cqi_pmi_cfg_idx, bref, (uint16_t)0u, (uint16_t)1023u));
      HANDLE_CODE(c.cqi_format_ind_periodic.unpack(bref));
      if (c.ri_cfg_idx_present) {
        HANDLE_CODE(unpack_integer(c.ri_cfg_idx, bref, (uint16_t)0u, (uint16_t)1023u));
      }
      HANDLE_CODE(bref.unpack(c.simul_ack_nack_and_cqi, 1));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::set(types::options e)
{
  type_ = e;
}
void cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::set_wideband_cqi()
{
  set(types::wideband_cqi);
}
cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::subband_cqi_s_&
cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::set_subband_cqi()
{
  set(types::subband_cqi);
  return c;
}
void cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::wideband_cqi:
      break;
    case types::subband_cqi:
      j.write_fieldname("subbandCQI");
      j.start_obj();
      j.write_int("k", c.k);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::wideband_cqi:
      break;
    case types::subband_cqi:
      HANDLE_CODE(pack_integer(bref, c.k, (uint8_t)1u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::wideband_cqi:
      break;
    case types::subband_cqi:
      HANDLE_CODE(unpack_integer(c.k, bref, (uint8_t)1u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CSI-RS-Config-r15 ::= CHOICE
void csi_rs_cfg_r15_c::set(types::options e)
{
  type_ = e;
}
void csi_rs_cfg_r15_c::set_release()
{
  set(types::release);
}
csi_rs_cfg_r15_c::setup_s_& csi_rs_cfg_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void csi_rs_cfg_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.csi_rs_cfg_r10_present) {
        j.write_fieldname("csi-RS-Config-r10");
        c.csi_rs_cfg_r10.to_json(j);
      }
      if (c.csi_rs_cfg_v1250_present) {
        j.write_fieldname("csi-RS-Config-v1250");
        c.csi_rs_cfg_v1250.to_json(j);
      }
      if (c.csi_rs_cfg_v1310_present) {
        j.write_fieldname("csi-RS-Config-v1310");
        c.csi_rs_cfg_v1310.to_json(j);
      }
      if (c.csi_rs_cfg_v1430_present) {
        j.write_fieldname("csi-RS-Config-v1430");
        c.csi_rs_cfg_v1430.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.csi_rs_cfg_r10_present, 1));
      HANDLE_CODE(bref.pack(c.csi_rs_cfg_v1250_present, 1));
      HANDLE_CODE(bref.pack(c.csi_rs_cfg_v1310_present, 1));
      HANDLE_CODE(bref.pack(c.csi_rs_cfg_v1430_present, 1));
      if (c.csi_rs_cfg_r10_present) {
        HANDLE_CODE(c.csi_rs_cfg_r10.pack(bref));
      }
      if (c.csi_rs_cfg_v1250_present) {
        HANDLE_CODE(c.csi_rs_cfg_v1250.pack(bref));
      }
      if (c.csi_rs_cfg_v1310_present) {
        HANDLE_CODE(c.csi_rs_cfg_v1310.pack(bref));
      }
      if (c.csi_rs_cfg_v1430_present) {
        HANDLE_CODE(c.csi_rs_cfg_v1430.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.csi_rs_cfg_r10_present, 1));
      HANDLE_CODE(bref.unpack(c.csi_rs_cfg_v1250_present, 1));
      HANDLE_CODE(bref.unpack(c.csi_rs_cfg_v1310_present, 1));
      HANDLE_CODE(bref.unpack(c.csi_rs_cfg_v1430_present, 1));
      if (c.csi_rs_cfg_r10_present) {
        HANDLE_CODE(c.csi_rs_cfg_r10.unpack(bref));
      }
      if (c.csi_rs_cfg_v1250_present) {
        HANDLE_CODE(c.csi_rs_cfg_v1250.unpack(bref));
      }
      if (c.csi_rs_cfg_v1310_present) {
        HANDLE_CODE(c.csi_rs_cfg_v1310.unpack(bref));
      }
      if (c.csi_rs_cfg_v1430_present) {
        HANDLE_CODE(c.csi_rs_cfg_v1430.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_r15_c::operator==(const csi_rs_cfg_r15_c& other) const
{
  return type() == other.type() and c.csi_rs_cfg_r10_present == other.c.csi_rs_cfg_r10_present and
         (not c.csi_rs_cfg_r10_present or c.csi_rs_cfg_r10 == other.c.csi_rs_cfg_r10) and
         c.csi_rs_cfg_v1250_present == other.c.csi_rs_cfg_v1250_present and
         (not c.csi_rs_cfg_v1250_present or c.csi_rs_cfg_v1250 == other.c.csi_rs_cfg_v1250) and
         c.csi_rs_cfg_v1310_present == other.c.csi_rs_cfg_v1310_present and
         (not c.csi_rs_cfg_v1310_present or c.csi_rs_cfg_v1310 == other.c.csi_rs_cfg_v1310) and
         c.csi_rs_cfg_v1430_present == other.c.csi_rs_cfg_v1430_present and
         (not c.csi_rs_cfg_v1430_present or c.csi_rs_cfg_v1430 == other.c.csi_rs_cfg_v1430);
}

// CSI-RS-ConfigZP-ApList-r14 ::= CHOICE
void csi_rs_cfg_zp_ap_list_r14_c::set(types::options e)
{
  type_ = e;
}
void csi_rs_cfg_zp_ap_list_r14_c::set_release()
{
  set(types::release);
}
csi_rs_cfg_zp_ap_list_r14_c::setup_l_& csi_rs_cfg_zp_ap_list_r14_c::set_setup()
{
  set(types::setup);
  return c;
}
void csi_rs_cfg_zp_ap_list_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.start_array("setup");
      for (const auto& e1 : c) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_zp_ap_list_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_cfg_zp_ap_list_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 4));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_zp_ap_list_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_zp_ap_list_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 4));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_cfg_zp_ap_list_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool csi_rs_cfg_zp_ap_list_r14_c::operator==(const csi_rs_cfg_zp_ap_list_r14_c& other) const
{
  return type() == other.type() and c == other.c;
}

// DMRS-Config-r11 ::= CHOICE
void dmrs_cfg_r11_c::set(types::options e)
{
  type_ = e;
}
void dmrs_cfg_r11_c::set_release()
{
  set(types::release);
}
dmrs_cfg_r11_c::setup_s_& dmrs_cfg_r11_c::set_setup()
{
  set(types::setup);
  return c;
}
void dmrs_cfg_r11_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("scramblingIdentity-r11", c.scrambling_id_r11);
      j.write_int("scramblingIdentity2-r11", c.scrambling_id2_r11);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "dmrs_cfg_r11_c");
  }
  j.end_obj();
}
SRSASN_CODE dmrs_cfg_r11_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c.scrambling_id_r11, (uint16_t)0u, (uint16_t)503u));
      HANDLE_CODE(pack_integer(bref, c.scrambling_id2_r11, (uint16_t)0u, (uint16_t)503u));
      break;
    default:
      log_invalid_choice_id(type_, "dmrs_cfg_r11_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dmrs_cfg_r11_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c.scrambling_id_r11, bref, (uint16_t)0u, (uint16_t)503u));
      HANDLE_CODE(unpack_integer(c.scrambling_id2_r11, bref, (uint16_t)0u, (uint16_t)503u));
      break;
    default:
      log_invalid_choice_id(type_, "dmrs_cfg_r11_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool dmrs_cfg_r11_c::operator==(const dmrs_cfg_r11_c& other) const
{
  return type() == other.type() and c.scrambling_id_r11 == other.c.scrambling_id_r11 and
         c.scrambling_id2_r11 == other.c.scrambling_id2_r11;
}

// DMRS-Config-v1310 ::= SEQUENCE
SRSASN_CODE dmrs_cfg_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dmrs_table_alt_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dmrs_cfg_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dmrs_table_alt_r13_present, 1));

  return SRSASN_SUCCESS;
}
void dmrs_cfg_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dmrs_table_alt_r13_present) {
    j.write_str("dmrs-tableAlt-r13", "true");
  }
  j.end_obj();
}
bool dmrs_cfg_v1310_s::operator==(const dmrs_cfg_v1310_s& other) const
{
  return dmrs_table_alt_r13_present == other.dmrs_table_alt_r13_present;
}

// DeltaTxD-OffsetListPUCCH-r10 ::= SEQUENCE
SRSASN_CODE delta_tx_d_offset_list_pucch_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(delta_tx_d_offset_pucch_format1_r10.pack(bref));
  HANDLE_CODE(delta_tx_d_offset_pucch_format1a1b_r10.pack(bref));
  HANDLE_CODE(delta_tx_d_offset_pucch_format22a2b_r10.pack(bref));
  HANDLE_CODE(delta_tx_d_offset_pucch_format3_r10.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE delta_tx_d_offset_list_pucch_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(delta_tx_d_offset_pucch_format1_r10.unpack(bref));
  HANDLE_CODE(delta_tx_d_offset_pucch_format1a1b_r10.unpack(bref));
  HANDLE_CODE(delta_tx_d_offset_pucch_format22a2b_r10.unpack(bref));
  HANDLE_CODE(delta_tx_d_offset_pucch_format3_r10.unpack(bref));

  return SRSASN_SUCCESS;
}
void delta_tx_d_offset_list_pucch_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("deltaTxD-OffsetPUCCH-Format1-r10", delta_tx_d_offset_pucch_format1_r10.to_string());
  j.write_str("deltaTxD-OffsetPUCCH-Format1a1b-r10", delta_tx_d_offset_pucch_format1a1b_r10.to_string());
  j.write_str("deltaTxD-OffsetPUCCH-Format22a2b-r10", delta_tx_d_offset_pucch_format22a2b_r10.to_string());
  j.write_str("deltaTxD-OffsetPUCCH-Format3-r10", delta_tx_d_offset_pucch_format3_r10.to_string());
  j.end_obj();
}
bool delta_tx_d_offset_list_pucch_r10_s::operator==(const delta_tx_d_offset_list_pucch_r10_s& other) const
{
  return ext == other.ext and delta_tx_d_offset_pucch_format1_r10 == other.delta_tx_d_offset_pucch_format1_r10 and
         delta_tx_d_offset_pucch_format1a1b_r10 == other.delta_tx_d_offset_pucch_format1a1b_r10 and
         delta_tx_d_offset_pucch_format22a2b_r10 == other.delta_tx_d_offset_pucch_format22a2b_r10 and
         delta_tx_d_offset_pucch_format3_r10 == other.delta_tx_d_offset_pucch_format3_r10;
}

const char* delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1_r10_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1_r10_e_");
}
int8_t delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1_r10_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1_r10_e_");
}

const char* delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1a1b_r10_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1a1b_r10_e_");
}
int8_t delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1a1b_r10_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1a1b_r10_e_");
}

const char* delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format22a2b_r10_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format22a2b_r10_e_");
}
int8_t delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format22a2b_r10_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format22a2b_r10_e_");
}

const char* delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format3_r10_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format3_r10_e_");
}
int8_t delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format3_r10_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format3_r10_e_");
}

// DeltaTxD-OffsetListPUCCH-v1130 ::= SEQUENCE
SRSASN_CODE delta_tx_d_offset_list_pucch_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(delta_tx_d_offset_pucch_format1b_cs_r11.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE delta_tx_d_offset_list_pucch_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(delta_tx_d_offset_pucch_format1b_cs_r11.unpack(bref));

  return SRSASN_SUCCESS;
}
void delta_tx_d_offset_list_pucch_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("deltaTxD-OffsetPUCCH-Format1bCS-r11", delta_tx_d_offset_pucch_format1b_cs_r11.to_string());
  j.end_obj();
}
bool delta_tx_d_offset_list_pucch_v1130_s::operator==(const delta_tx_d_offset_list_pucch_v1130_s& other) const
{
  return delta_tx_d_offset_pucch_format1b_cs_r11 == other.delta_tx_d_offset_pucch_format1b_cs_r11;
}

const char* delta_tx_d_offset_list_pucch_v1130_s::delta_tx_d_offset_pucch_format1b_cs_r11_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-1"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_pucch_v1130_s::delta_tx_d_offset_pucch_format1b_cs_r11_e_");
}
int8_t delta_tx_d_offset_list_pucch_v1130_s::delta_tx_d_offset_pucch_format1b_cs_r11_opts::to_number() const
{
  static const int8_t options[] = {0, -1};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_pucch_v1130_s::delta_tx_d_offset_pucch_format1b_cs_r11_e_");
}

// EIMTA-MainConfig-r12 ::= CHOICE
void eimta_main_cfg_r12_c::set(types::options e)
{
  type_ = e;
}
void eimta_main_cfg_r12_c::set_release()
{
  set(types::release);
}
eimta_main_cfg_r12_c::setup_s_& eimta_main_cfg_r12_c::set_setup()
{
  set(types::setup);
  return c;
}
void eimta_main_cfg_r12_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("eimta-RNTI-r12", c.eimta_rnti_r12.to_string());
      j.write_str("eimta-CommandPeriodicity-r12", c.eimta_cmd_periodicity_r12.to_string());
      j.write_str("eimta-CommandSubframeSet-r12", c.eimta_cmd_sf_set_r12.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "eimta_main_cfg_r12_c");
  }
  j.end_obj();
}
SRSASN_CODE eimta_main_cfg_r12_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.eimta_rnti_r12.pack(bref));
      HANDLE_CODE(c.eimta_cmd_periodicity_r12.pack(bref));
      HANDLE_CODE(c.eimta_cmd_sf_set_r12.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "eimta_main_cfg_r12_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE eimta_main_cfg_r12_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.eimta_rnti_r12.unpack(bref));
      HANDLE_CODE(c.eimta_cmd_periodicity_r12.unpack(bref));
      HANDLE_CODE(c.eimta_cmd_sf_set_r12.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "eimta_main_cfg_r12_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool eimta_main_cfg_r12_c::operator==(const eimta_main_cfg_r12_c& other) const
{
  return type() == other.type() and c.eimta_rnti_r12 == other.c.eimta_rnti_r12 and
         c.eimta_cmd_periodicity_r12 == other.c.eimta_cmd_periodicity_r12 and
         c.eimta_cmd_sf_set_r12 == other.c.eimta_cmd_sf_set_r12;
}

const char* eimta_main_cfg_r12_c::setup_s_::eimta_cmd_periodicity_r12_opts::to_string() const
{
  static const char* options[] = {"sf10", "sf20", "sf40", "sf80"};
  return convert_enum_idx(options, 4, value, "eimta_main_cfg_r12_c::setup_s_::eimta_cmd_periodicity_r12_e_");
}
uint8_t eimta_main_cfg_r12_c::setup_s_::eimta_cmd_periodicity_r12_opts::to_number() const
{
  static const uint8_t options[] = {10, 20, 40, 80};
  return map_enum_number(options, 4, value, "eimta_main_cfg_r12_c::setup_s_::eimta_cmd_periodicity_r12_e_");
}

// EIMTA-MainConfigServCell-r12 ::= CHOICE
void eimta_main_cfg_serv_cell_r12_c::set(types::options e)
{
  type_ = e;
}
void eimta_main_cfg_serv_cell_r12_c::set_release()
{
  set(types::release);
}
eimta_main_cfg_serv_cell_r12_c::setup_s_& eimta_main_cfg_serv_cell_r12_c::set_setup()
{
  set(types::setup);
  return c;
}
void eimta_main_cfg_serv_cell_r12_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("eimta-UL-DL-ConfigIndex-r12", c.eimta_ul_dl_cfg_idx_r12);
      j.write_str("eimta-HARQ-ReferenceConfig-r12", c.eimta_harq_ref_cfg_r12.to_string());
      j.write_fieldname("mbsfn-SubframeConfigList-v1250");
      c.mbsfn_sf_cfg_list_v1250.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "eimta_main_cfg_serv_cell_r12_c");
  }
  j.end_obj();
}
SRSASN_CODE eimta_main_cfg_serv_cell_r12_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c.eimta_ul_dl_cfg_idx_r12, (uint8_t)1u, (uint8_t)5u));
      HANDLE_CODE(c.eimta_harq_ref_cfg_r12.pack(bref));
      HANDLE_CODE(c.mbsfn_sf_cfg_list_v1250.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "eimta_main_cfg_serv_cell_r12_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE eimta_main_cfg_serv_cell_r12_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c.eimta_ul_dl_cfg_idx_r12, bref, (uint8_t)1u, (uint8_t)5u));
      HANDLE_CODE(c.eimta_harq_ref_cfg_r12.unpack(bref));
      HANDLE_CODE(c.mbsfn_sf_cfg_list_v1250.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "eimta_main_cfg_serv_cell_r12_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool eimta_main_cfg_serv_cell_r12_c::operator==(const eimta_main_cfg_serv_cell_r12_c& other) const
{
  return type() == other.type() and c.eimta_ul_dl_cfg_idx_r12 == other.c.eimta_ul_dl_cfg_idx_r12 and
         c.eimta_harq_ref_cfg_r12 == other.c.eimta_harq_ref_cfg_r12 and
         c.mbsfn_sf_cfg_list_v1250 == other.c.mbsfn_sf_cfg_list_v1250;
}

const char* eimta_main_cfg_serv_cell_r12_c::setup_s_::eimta_harq_ref_cfg_r12_opts::to_string() const
{
  static const char* options[] = {"sa2", "sa4", "sa5"};
  return convert_enum_idx(options, 3, value, "eimta_main_cfg_serv_cell_r12_c::setup_s_::eimta_harq_ref_cfg_r12_e_");
}
uint8_t eimta_main_cfg_serv_cell_r12_c::setup_s_::eimta_harq_ref_cfg_r12_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 5};
  return map_enum_number(options, 3, value, "eimta_main_cfg_serv_cell_r12_c::setup_s_::eimta_harq_ref_cfg_r12_e_");
}

void eimta_main_cfg_serv_cell_r12_c::setup_s_::mbsfn_sf_cfg_list_v1250_c_::set(types::options e)
{
  type_ = e;
}
void eimta_main_cfg_serv_cell_r12_c::setup_s_::mbsfn_sf_cfg_list_v1250_c_::set_release()
{
  set(types::release);
}
eimta_main_cfg_serv_cell_r12_c::setup_s_::mbsfn_sf_cfg_list_v1250_c_::setup_s_&
eimta_main_cfg_serv_cell_r12_c::setup_s_::mbsfn_sf_cfg_list_v1250_c_::set_setup()
{
  set(types::setup);
  return c;
}
void eimta_main_cfg_serv_cell_r12_c::setup_s_::mbsfn_sf_cfg_list_v1250_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("subframeConfigList-r12");
      for (const auto& e1 : c.sf_cfg_list_r12) {
        e1.to_json(j);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "eimta_main_cfg_serv_cell_r12_c::setup_s_::mbsfn_sf_cfg_list_v1250_c_");
  }
  j.end_obj();
}
SRSASN_CODE eimta_main_cfg_serv_cell_r12_c::setup_s_::mbsfn_sf_cfg_list_v1250_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.sf_cfg_list_r12, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "eimta_main_cfg_serv_cell_r12_c::setup_s_::mbsfn_sf_cfg_list_v1250_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE eimta_main_cfg_serv_cell_r12_c::setup_s_::mbsfn_sf_cfg_list_v1250_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.sf_cfg_list_r12, bref, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "eimta_main_cfg_serv_cell_r12_c::setup_s_::mbsfn_sf_cfg_list_v1250_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool eimta_main_cfg_serv_cell_r12_c::setup_s_::mbsfn_sf_cfg_list_v1250_c_::operator==(
    const mbsfn_sf_cfg_list_v1250_c_& other) const
{
  return type() == other.type() and c.sf_cfg_list_r12 == other.c.sf_cfg_list_r12;
}

// Format4-resource-r13 ::= SEQUENCE
SRSASN_CODE format4_res_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, start_prb_format4_r13, (uint8_t)0u, (uint8_t)109u));
  HANDLE_CODE(pack_integer(bref, nof_prb_format4_r13, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE format4_res_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(start_prb_format4_r13, bref, (uint8_t)0u, (uint8_t)109u));
  HANDLE_CODE(unpack_integer(nof_prb_format4_r13, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void format4_res_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("startingPRB-format4-r13", start_prb_format4_r13);
  j.write_int("numberOfPRB-format4-r13", nof_prb_format4_r13);
  j.end_obj();
}
bool format4_res_r13_s::operator==(const format4_res_r13_s& other) const
{
  return start_prb_format4_r13 == other.start_prb_format4_r13 and nof_prb_format4_r13 == other.nof_prb_format4_r13;
}

// Format5-resource-r13 ::= SEQUENCE
SRSASN_CODE format5_res_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, start_prb_format5_r13, (uint8_t)0u, (uint8_t)109u));
  HANDLE_CODE(pack_integer(bref, cdm_idx_format5_r13, (uint8_t)0u, (uint8_t)1u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE format5_res_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(start_prb_format5_r13, bref, (uint8_t)0u, (uint8_t)109u));
  HANDLE_CODE(unpack_integer(cdm_idx_format5_r13, bref, (uint8_t)0u, (uint8_t)1u));

  return SRSASN_SUCCESS;
}
void format5_res_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("startingPRB-format5-r13", start_prb_format5_r13);
  j.write_int("cdm-index-format5-r13", cdm_idx_format5_r13);
  j.end_obj();
}
bool format5_res_r13_s::operator==(const format5_res_r13_s& other) const
{
  return start_prb_format5_r13 == other.start_prb_format5_r13 and cdm_idx_format5_r13 == other.cdm_idx_format5_r13;
}

// PDCCH-CandidateReductionValue-r13 ::= ENUMERATED
const char* pdcch_candidate_reduction_value_r13_opts::to_string() const
{
  static const char* options[] = {"n0", "n33", "n66", "n100"};
  return convert_enum_idx(options, 4, value, "pdcch_candidate_reduction_value_r13_e");
}
uint8_t pdcch_candidate_reduction_value_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 33, 66, 100};
  return map_enum_number(options, 4, value, "pdcch_candidate_reduction_value_r13_e");
}

// PUCCH-ConfigDedicated-v1530 ::= SEQUENCE
SRSASN_CODE pucch_cfg_ded_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(n1_pucch_an_spt_r15_present, 1));
  HANDLE_CODE(bref.pack(codebooksize_determination_stti_r15_present, 1));

  if (n1_pucch_an_spt_r15_present) {
    HANDLE_CODE(pack_integer(bref, n1_pucch_an_spt_r15, (uint16_t)0u, (uint16_t)2047u));
  }
  if (codebooksize_determination_stti_r15_present) {
    HANDLE_CODE(codebooksize_determination_stti_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(n1_pucch_an_spt_r15_present, 1));
  HANDLE_CODE(bref.unpack(codebooksize_determination_stti_r15_present, 1));

  if (n1_pucch_an_spt_r15_present) {
    HANDLE_CODE(unpack_integer(n1_pucch_an_spt_r15, bref, (uint16_t)0u, (uint16_t)2047u));
  }
  if (codebooksize_determination_stti_r15_present) {
    HANDLE_CODE(codebooksize_determination_stti_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pucch_cfg_ded_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (n1_pucch_an_spt_r15_present) {
    j.write_int("n1PUCCH-AN-SPT-r15", n1_pucch_an_spt_r15);
  }
  if (codebooksize_determination_stti_r15_present) {
    j.write_str("codebooksizeDeterminationSTTI-r15", codebooksize_determination_stti_r15.to_string());
  }
  j.end_obj();
}
bool pucch_cfg_ded_v1530_s::operator==(const pucch_cfg_ded_v1530_s& other) const
{
  return n1_pucch_an_spt_r15_present == other.n1_pucch_an_spt_r15_present and
         (not n1_pucch_an_spt_r15_present or n1_pucch_an_spt_r15 == other.n1_pucch_an_spt_r15) and
         codebooksize_determination_stti_r15_present == other.codebooksize_determination_stti_r15_present and
         (not codebooksize_determination_stti_r15_present or
          codebooksize_determination_stti_r15 == other.codebooksize_determination_stti_r15);
}

const char* pucch_cfg_ded_v1530_s::codebooksize_determination_stti_r15_opts::to_string() const
{
  static const char* options[] = {"dai", "cc"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_ded_v1530_s::codebooksize_determination_stti_r15_e_");
}

// PUCCH-Format3-Conf-r13 ::= SEQUENCE
SRSASN_CODE pucch_format3_conf_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(n3_pucch_an_list_r13_present, 1));
  HANDLE_CODE(bref.pack(two_ant_port_activ_pucch_format3_r13_present, 1));

  if (n3_pucch_an_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, n3_pucch_an_list_r13, 1, 4, integer_packer<uint16_t>(0, 549)));
  }
  if (two_ant_port_activ_pucch_format3_r13_present) {
    HANDLE_CODE(two_ant_port_activ_pucch_format3_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_format3_conf_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(n3_pucch_an_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(two_ant_port_activ_pucch_format3_r13_present, 1));

  if (n3_pucch_an_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(n3_pucch_an_list_r13, bref, 1, 4, integer_packer<uint16_t>(0, 549)));
  }
  if (two_ant_port_activ_pucch_format3_r13_present) {
    HANDLE_CODE(two_ant_port_activ_pucch_format3_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pucch_format3_conf_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (n3_pucch_an_list_r13_present) {
    j.start_array("n3PUCCH-AN-List-r13");
    for (const auto& e1 : n3_pucch_an_list_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (two_ant_port_activ_pucch_format3_r13_present) {
    j.write_fieldname("twoAntennaPortActivatedPUCCH-Format3-r13");
    two_ant_port_activ_pucch_format3_r13.to_json(j);
  }
  j.end_obj();
}
bool pucch_format3_conf_r13_s::operator==(const pucch_format3_conf_r13_s& other) const
{
  return n3_pucch_an_list_r13_present == other.n3_pucch_an_list_r13_present and
         (not n3_pucch_an_list_r13_present or n3_pucch_an_list_r13 == other.n3_pucch_an_list_r13) and
         two_ant_port_activ_pucch_format3_r13_present == other.two_ant_port_activ_pucch_format3_r13_present and
         (not two_ant_port_activ_pucch_format3_r13_present or
          two_ant_port_activ_pucch_format3_r13 == other.two_ant_port_activ_pucch_format3_r13);
}

void pucch_format3_conf_r13_s::two_ant_port_activ_pucch_format3_r13_c_::set(types::options e)
{
  type_ = e;
}
void pucch_format3_conf_r13_s::two_ant_port_activ_pucch_format3_r13_c_::set_release()
{
  set(types::release);
}
pucch_format3_conf_r13_s::two_ant_port_activ_pucch_format3_r13_c_::setup_s_&
pucch_format3_conf_r13_s::two_ant_port_activ_pucch_format3_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pucch_format3_conf_r13_s::two_ant_port_activ_pucch_format3_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("n3PUCCH-AN-ListP1-r13");
      for (const auto& e1 : c.n3_pucch_an_list_p1_r13) {
        j.write_int(e1);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_format3_conf_r13_s::two_ant_port_activ_pucch_format3_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_format3_conf_r13_s::two_ant_port_activ_pucch_format3_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.n3_pucch_an_list_p1_r13, 1, 4, integer_packer<uint16_t>(0, 549)));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_format3_conf_r13_s::two_ant_port_activ_pucch_format3_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_format3_conf_r13_s::two_ant_port_activ_pucch_format3_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.n3_pucch_an_list_p1_r13, bref, 1, 4, integer_packer<uint16_t>(0, 549)));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_format3_conf_r13_s::two_ant_port_activ_pucch_format3_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pucch_format3_conf_r13_s::two_ant_port_activ_pucch_format3_r13_c_::operator==(
    const two_ant_port_activ_pucch_format3_r13_c_& other) const
{
  return type() == other.type() and c.n3_pucch_an_list_p1_r13 == other.c.n3_pucch_an_list_p1_r13;
}

// SPDCCH-Config-r15 ::= CHOICE
void spdcch_cfg_r15_c::set(types::options e)
{
  type_ = e;
}
void spdcch_cfg_r15_c::set_release()
{
  set(types::release);
}
spdcch_cfg_r15_c::setup_s_& spdcch_cfg_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void spdcch_cfg_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.spdcch_l1_reuse_ind_r15_present) {
        j.write_str("spdcch-L1-ReuseIndication-r15", c.spdcch_l1_reuse_ind_r15.to_string());
      }
      if (c.spdcch_set_cfg_r15_present) {
        j.start_array("spdcch-SetConfig-r15");
        for (const auto& e1 : c.spdcch_set_cfg_r15) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "spdcch_cfg_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE spdcch_cfg_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.spdcch_l1_reuse_ind_r15_present, 1));
      HANDLE_CODE(bref.pack(c.spdcch_set_cfg_r15_present, 1));
      if (c.spdcch_l1_reuse_ind_r15_present) {
        HANDLE_CODE(c.spdcch_l1_reuse_ind_r15.pack(bref));
      }
      if (c.spdcch_set_cfg_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.spdcch_set_cfg_r15, 1, 4));
      }
      break;
    default:
      log_invalid_choice_id(type_, "spdcch_cfg_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE spdcch_cfg_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.spdcch_l1_reuse_ind_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.spdcch_set_cfg_r15_present, 1));
      if (c.spdcch_l1_reuse_ind_r15_present) {
        HANDLE_CODE(c.spdcch_l1_reuse_ind_r15.unpack(bref));
      }
      if (c.spdcch_set_cfg_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.spdcch_set_cfg_r15, bref, 1, 4));
      }
      break;
    default:
      log_invalid_choice_id(type_, "spdcch_cfg_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool spdcch_cfg_r15_c::operator==(const spdcch_cfg_r15_c& other) const
{
  return type() == other.type() and c.spdcch_l1_reuse_ind_r15_present == other.c.spdcch_l1_reuse_ind_r15_present and
         (not c.spdcch_l1_reuse_ind_r15_present or c.spdcch_l1_reuse_ind_r15 == other.c.spdcch_l1_reuse_ind_r15) and
         c.spdcch_set_cfg_r15_present == other.c.spdcch_set_cfg_r15_present and
         (not c.spdcch_set_cfg_r15_present or c.spdcch_set_cfg_r15 == other.c.spdcch_set_cfg_r15);
}

const char* spdcch_cfg_r15_c::setup_s_::spdcch_l1_reuse_ind_r15_opts::to_string() const
{
  static const char* options[] = {"n0", "n1", "n2"};
  return convert_enum_idx(options, 3, value, "spdcch_cfg_r15_c::setup_s_::spdcch_l1_reuse_ind_r15_e_");
}
uint8_t spdcch_cfg_r15_c::setup_s_::spdcch_l1_reuse_ind_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2};
  return map_enum_number(options, 3, value, "spdcch_cfg_r15_c::setup_s_::spdcch_l1_reuse_ind_r15_e_");
}

// SPUCCH-Config-r15 ::= CHOICE
void spucch_cfg_r15_c::set(types::options e)
{
  type_ = e;
}
void spucch_cfg_r15_c::set_release()
{
  set(types::release);
}
spucch_cfg_r15_c::setup_s_& spucch_cfg_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void spucch_cfg_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.spucch_set_r15_present) {
        j.start_array("spucch-Set-r15");
        for (const auto& e1 : c.spucch_set_r15) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (c.two_ant_port_activ_spucch_format1a1b_r15_present) {
        j.write_str("twoAntennaPortActivatedSPUCCH-Format1a1b-r15", "true");
      }
      j.write_fieldname("dummy");
      j.start_obj();
      j.start_array("n3SPUCCH-AN-List-r15");
      for (const auto& e1 : c.dummy.n3_spucch_an_list_r15) {
        j.write_int(e1);
      }
      j.end_array();
      j.end_obj();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "spucch_cfg_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE spucch_cfg_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.spucch_set_r15_present, 1));
      HANDLE_CODE(bref.pack(c.two_ant_port_activ_spucch_format1a1b_r15_present, 1));
      if (c.spucch_set_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.spucch_set_r15, 1, 4));
      }
      HANDLE_CODE(pack_dyn_seq_of(bref, c.dummy.n3_spucch_an_list_r15, 1, 4, integer_packer<uint16_t>(0, 549)));
      break;
    default:
      log_invalid_choice_id(type_, "spucch_cfg_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE spucch_cfg_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.spucch_set_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.two_ant_port_activ_spucch_format1a1b_r15_present, 1));
      if (c.spucch_set_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.spucch_set_r15, bref, 1, 4));
      }
      HANDLE_CODE(unpack_dyn_seq_of(c.dummy.n3_spucch_an_list_r15, bref, 1, 4, integer_packer<uint16_t>(0, 549)));
      break;
    default:
      log_invalid_choice_id(type_, "spucch_cfg_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool spucch_cfg_r15_c::operator==(const spucch_cfg_r15_c& other) const
{
  return type() == other.type() and c.spucch_set_r15_present == other.c.spucch_set_r15_present and
         (not c.spucch_set_r15_present or c.spucch_set_r15 == other.c.spucch_set_r15) and
         c.two_ant_port_activ_spucch_format1a1b_r15_present ==
             other.c.two_ant_port_activ_spucch_format1a1b_r15_present and
         c.dummy.n3_spucch_an_list_r15 == other.c.dummy.n3_spucch_an_list_r15;
}

// SRS-CC-SetIndex-r14 ::= SEQUENCE
SRSASN_CODE srs_cc_set_idx_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cc_set_idx_r14, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(pack_integer(bref, cc_idx_in_one_cc_set_r14, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_cc_set_idx_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cc_set_idx_r14, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(unpack_integer(cc_idx_in_one_cc_set_r14, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void srs_cc_set_idx_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cc-SetIndex-r14", cc_set_idx_r14);
  j.write_int("cc-IndexInOneCC-Set-r14", cc_idx_in_one_cc_set_r14);
  j.end_obj();
}
bool srs_cc_set_idx_r14_s::operator==(const srs_cc_set_idx_r14_s& other) const
{
  return cc_set_idx_r14 == other.cc_set_idx_r14 and cc_idx_in_one_cc_set_r14 == other.cc_idx_in_one_cc_set_r14;
}

// SRS-ConfigAdd-r16 ::= SEQUENCE
SRSASN_CODE srs_cfg_add_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_guard_symbol_as_add_r16_present, 1));
  HANDLE_CODE(bref.pack(srs_guard_symbol_fh_add_r16_present, 1));

  HANDLE_CODE(srs_rep_num_add_r16.pack(bref));
  HANDLE_CODE(srs_bw_add_r16.pack(bref));
  HANDLE_CODE(srs_hop_bw_add_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, srs_freq_domain_pos_add_r16, (uint8_t)0u, (uint8_t)23u));
  HANDLE_CODE(srs_ant_port_add_r16.pack(bref));
  HANDLE_CODE(srs_cyclic_shift_add_r16.pack(bref));
  HANDLE_CODE(srs_tx_comb_num_add_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, srs_tx_comb_add_r16, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(pack_integer(bref, srs_start_pos_add_r16, (uint8_t)1u, (uint8_t)13u));
  HANDLE_CODE(pack_integer(bref, srs_dur_add_r16, (uint8_t)1u, (uint8_t)13u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_cfg_add_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_guard_symbol_as_add_r16_present, 1));
  HANDLE_CODE(bref.unpack(srs_guard_symbol_fh_add_r16_present, 1));

  HANDLE_CODE(srs_rep_num_add_r16.unpack(bref));
  HANDLE_CODE(srs_bw_add_r16.unpack(bref));
  HANDLE_CODE(srs_hop_bw_add_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(srs_freq_domain_pos_add_r16, bref, (uint8_t)0u, (uint8_t)23u));
  HANDLE_CODE(srs_ant_port_add_r16.unpack(bref));
  HANDLE_CODE(srs_cyclic_shift_add_r16.unpack(bref));
  HANDLE_CODE(srs_tx_comb_num_add_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(srs_tx_comb_add_r16, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(unpack_integer(srs_start_pos_add_r16, bref, (uint8_t)1u, (uint8_t)13u));
  HANDLE_CODE(unpack_integer(srs_dur_add_r16, bref, (uint8_t)1u, (uint8_t)13u));

  return SRSASN_SUCCESS;
}
void srs_cfg_add_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("srs-RepNumAdd-r16", srs_rep_num_add_r16.to_string());
  j.write_str("srs-BandwidthAdd-r16", srs_bw_add_r16.to_string());
  j.write_str("srs-HoppingBandwidthAdd-r16", srs_hop_bw_add_r16.to_string());
  j.write_int("srs-FreqDomainPosAdd-r16", srs_freq_domain_pos_add_r16);
  j.write_str("srs-AntennaPortAdd-r16", srs_ant_port_add_r16.to_string());
  j.write_str("srs-CyclicShiftAdd-r16", srs_cyclic_shift_add_r16.to_string());
  j.write_str("srs-TransmissionCombNumAdd-r16", srs_tx_comb_num_add_r16.to_string());
  j.write_int("srs-TransmissionCombAdd-r16", srs_tx_comb_add_r16);
  j.write_int("srs-StartPosAdd-r16", srs_start_pos_add_r16);
  j.write_int("srs-DurationAdd-r16", srs_dur_add_r16);
  if (srs_guard_symbol_as_add_r16_present) {
    j.write_str("srs-GuardSymbolAS-Add-r16", "enabled");
  }
  if (srs_guard_symbol_fh_add_r16_present) {
    j.write_str("srs-GuardSymbolFH-Add-r16", "enabled");
  }
  j.end_obj();
}
bool srs_cfg_add_r16_s::operator==(const srs_cfg_add_r16_s& other) const
{
  return srs_rep_num_add_r16 == other.srs_rep_num_add_r16 and srs_bw_add_r16 == other.srs_bw_add_r16 and
         srs_hop_bw_add_r16 == other.srs_hop_bw_add_r16 and
         srs_freq_domain_pos_add_r16 == other.srs_freq_domain_pos_add_r16 and
         srs_ant_port_add_r16 == other.srs_ant_port_add_r16 and
         srs_cyclic_shift_add_r16 == other.srs_cyclic_shift_add_r16 and
         srs_tx_comb_num_add_r16 == other.srs_tx_comb_num_add_r16 and
         srs_tx_comb_add_r16 == other.srs_tx_comb_add_r16 and srs_start_pos_add_r16 == other.srs_start_pos_add_r16 and
         srs_dur_add_r16 == other.srs_dur_add_r16 and
         srs_guard_symbol_as_add_r16_present == other.srs_guard_symbol_as_add_r16_present and
         srs_guard_symbol_fh_add_r16_present == other.srs_guard_symbol_fh_add_r16_present;
}

const char* srs_cfg_add_r16_s::srs_rep_num_add_r16_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n6", "n7", "n8", "n9", "n12", "n13"};
  return convert_enum_idx(options, 10, value, "srs_cfg_add_r16_s::srs_rep_num_add_r16_e_");
}
uint8_t srs_cfg_add_r16_s::srs_rep_num_add_r16_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 7, 8, 9, 12, 13};
  return map_enum_number(options, 10, value, "srs_cfg_add_r16_s::srs_rep_num_add_r16_e_");
}

const char* srs_cfg_add_r16_s::srs_bw_add_r16_opts::to_string() const
{
  static const char* options[] = {"bw0", "bw1", "bw2", "bw3"};
  return convert_enum_idx(options, 4, value, "srs_cfg_add_r16_s::srs_bw_add_r16_e_");
}
uint8_t srs_cfg_add_r16_s::srs_bw_add_r16_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "srs_cfg_add_r16_s::srs_bw_add_r16_e_");
}

const char* srs_cfg_add_r16_s::srs_hop_bw_add_r16_opts::to_string() const
{
  static const char* options[] = {"hbw0", "hbw1", "hbw2", "hbw3"};
  return convert_enum_idx(options, 4, value, "srs_cfg_add_r16_s::srs_hop_bw_add_r16_e_");
}
uint8_t srs_cfg_add_r16_s::srs_hop_bw_add_r16_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "srs_cfg_add_r16_s::srs_hop_bw_add_r16_e_");
}

const char* srs_cfg_add_r16_s::srs_cyclic_shift_add_r16_opts::to_string() const
{
  static const char* options[] = {"cs0", "cs1", "cs2", "cs3", "cs4", "cs5", "cs6", "cs7", "cs8", "cs9", "cs10", "cs11"};
  return convert_enum_idx(options, 12, value, "srs_cfg_add_r16_s::srs_cyclic_shift_add_r16_e_");
}
uint8_t srs_cfg_add_r16_s::srs_cyclic_shift_add_r16_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  return map_enum_number(options, 12, value, "srs_cfg_add_r16_s::srs_cyclic_shift_add_r16_e_");
}

const char* srs_cfg_add_r16_s::srs_tx_comb_num_add_r16_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "srs_cfg_add_r16_s::srs_tx_comb_num_add_r16_e_");
}
uint8_t srs_cfg_add_r16_s::srs_tx_comb_num_add_r16_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "srs_cfg_add_r16_s::srs_tx_comb_num_add_r16_e_");
}

// SRS-ConfigAp-r10 ::= SEQUENCE
SRSASN_CODE srs_cfg_ap_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(srs_ant_port_ap_r10.pack(bref));
  HANDLE_CODE(srs_bw_ap_r10.pack(bref));
  HANDLE_CODE(pack_integer(bref, freq_domain_position_ap_r10, (uint8_t)0u, (uint8_t)23u));
  HANDLE_CODE(pack_integer(bref, tx_comb_ap_r10, (uint8_t)0u, (uint8_t)1u));
  HANDLE_CODE(cyclic_shift_ap_r10.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_cfg_ap_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(srs_ant_port_ap_r10.unpack(bref));
  HANDLE_CODE(srs_bw_ap_r10.unpack(bref));
  HANDLE_CODE(unpack_integer(freq_domain_position_ap_r10, bref, (uint8_t)0u, (uint8_t)23u));
  HANDLE_CODE(unpack_integer(tx_comb_ap_r10, bref, (uint8_t)0u, (uint8_t)1u));
  HANDLE_CODE(cyclic_shift_ap_r10.unpack(bref));

  return SRSASN_SUCCESS;
}
void srs_cfg_ap_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("srs-AntennaPortAp-r10", srs_ant_port_ap_r10.to_string());
  j.write_str("srs-BandwidthAp-r10", srs_bw_ap_r10.to_string());
  j.write_int("freqDomainPositionAp-r10", freq_domain_position_ap_r10);
  j.write_int("transmissionCombAp-r10", tx_comb_ap_r10);
  j.write_str("cyclicShiftAp-r10", cyclic_shift_ap_r10.to_string());
  j.end_obj();
}
bool srs_cfg_ap_r10_s::operator==(const srs_cfg_ap_r10_s& other) const
{
  return srs_ant_port_ap_r10 == other.srs_ant_port_ap_r10 and srs_bw_ap_r10 == other.srs_bw_ap_r10 and
         freq_domain_position_ap_r10 == other.freq_domain_position_ap_r10 and tx_comb_ap_r10 == other.tx_comb_ap_r10 and
         cyclic_shift_ap_r10 == other.cyclic_shift_ap_r10;
}

const char* srs_cfg_ap_r10_s::srs_bw_ap_r10_opts::to_string() const
{
  static const char* options[] = {"bw0", "bw1", "bw2", "bw3"};
  return convert_enum_idx(options, 4, value, "srs_cfg_ap_r10_s::srs_bw_ap_r10_e_");
}
uint8_t srs_cfg_ap_r10_s::srs_bw_ap_r10_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "srs_cfg_ap_r10_s::srs_bw_ap_r10_e_");
}

const char* srs_cfg_ap_r10_s::cyclic_shift_ap_r10_opts::to_string() const
{
  static const char* options[] = {"cs0", "cs1", "cs2", "cs3", "cs4", "cs5", "cs6", "cs7"};
  return convert_enum_idx(options, 8, value, "srs_cfg_ap_r10_s::cyclic_shift_ap_r10_e_");
}
uint8_t srs_cfg_ap_r10_s::cyclic_shift_ap_r10_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return map_enum_number(options, 8, value, "srs_cfg_ap_r10_s::cyclic_shift_ap_r10_e_");
}

// SRS-ConfigAp-r13 ::= SEQUENCE
SRSASN_CODE srs_cfg_ap_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(srs_ant_port_ap_r13.pack(bref));
  HANDLE_CODE(srs_bw_ap_r13.pack(bref));
  HANDLE_CODE(pack_integer(bref, freq_domain_position_ap_r13, (uint8_t)0u, (uint8_t)23u));
  HANDLE_CODE(pack_integer(bref, tx_comb_ap_r13, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(cyclic_shift_ap_r13.pack(bref));
  HANDLE_CODE(tx_comb_num_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_cfg_ap_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(srs_ant_port_ap_r13.unpack(bref));
  HANDLE_CODE(srs_bw_ap_r13.unpack(bref));
  HANDLE_CODE(unpack_integer(freq_domain_position_ap_r13, bref, (uint8_t)0u, (uint8_t)23u));
  HANDLE_CODE(unpack_integer(tx_comb_ap_r13, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(cyclic_shift_ap_r13.unpack(bref));
  HANDLE_CODE(tx_comb_num_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void srs_cfg_ap_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("srs-AntennaPortAp-r13", srs_ant_port_ap_r13.to_string());
  j.write_str("srs-BandwidthAp-r13", srs_bw_ap_r13.to_string());
  j.write_int("freqDomainPositionAp-r13", freq_domain_position_ap_r13);
  j.write_int("transmissionCombAp-r13", tx_comb_ap_r13);
  j.write_str("cyclicShiftAp-r13", cyclic_shift_ap_r13.to_string());
  j.write_str("transmissionCombNum-r13", tx_comb_num_r13.to_string());
  j.end_obj();
}
bool srs_cfg_ap_r13_s::operator==(const srs_cfg_ap_r13_s& other) const
{
  return srs_ant_port_ap_r13 == other.srs_ant_port_ap_r13 and srs_bw_ap_r13 == other.srs_bw_ap_r13 and
         freq_domain_position_ap_r13 == other.freq_domain_position_ap_r13 and tx_comb_ap_r13 == other.tx_comb_ap_r13 and
         cyclic_shift_ap_r13 == other.cyclic_shift_ap_r13 and tx_comb_num_r13 == other.tx_comb_num_r13;
}

const char* srs_cfg_ap_r13_s::srs_bw_ap_r13_opts::to_string() const
{
  static const char* options[] = {"bw0", "bw1", "bw2", "bw3"};
  return convert_enum_idx(options, 4, value, "srs_cfg_ap_r13_s::srs_bw_ap_r13_e_");
}
uint8_t srs_cfg_ap_r13_s::srs_bw_ap_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "srs_cfg_ap_r13_s::srs_bw_ap_r13_e_");
}

const char* srs_cfg_ap_r13_s::cyclic_shift_ap_r13_opts::to_string() const
{
  static const char* options[] = {"cs0", "cs1", "cs2", "cs3", "cs4", "cs5", "cs6", "cs7", "cs8", "cs9", "cs10", "cs11"};
  return convert_enum_idx(options, 12, value, "srs_cfg_ap_r13_s::cyclic_shift_ap_r13_e_");
}
uint8_t srs_cfg_ap_r13_s::cyclic_shift_ap_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  return map_enum_number(options, 12, value, "srs_cfg_ap_r13_s::cyclic_shift_ap_r13_e_");
}

const char* srs_cfg_ap_r13_s::tx_comb_num_r13_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "srs_cfg_ap_r13_s::tx_comb_num_r13_e_");
}
uint8_t srs_cfg_ap_r13_s::tx_comb_num_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "srs_cfg_ap_r13_s::tx_comb_num_r13_e_");
}

// SRS-ConfigAp-v1310 ::= SEQUENCE
SRSASN_CODE srs_cfg_ap_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tx_comb_ap_v1310_present, 1));
  HANDLE_CODE(bref.pack(cyclic_shift_ap_v1310_present, 1));
  HANDLE_CODE(bref.pack(tx_comb_num_r13_present, 1));

  if (tx_comb_ap_v1310_present) {
    HANDLE_CODE(pack_integer(bref, tx_comb_ap_v1310, (uint8_t)2u, (uint8_t)3u));
  }
  if (cyclic_shift_ap_v1310_present) {
    HANDLE_CODE(cyclic_shift_ap_v1310.pack(bref));
  }
  if (tx_comb_num_r13_present) {
    HANDLE_CODE(tx_comb_num_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_cfg_ap_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tx_comb_ap_v1310_present, 1));
  HANDLE_CODE(bref.unpack(cyclic_shift_ap_v1310_present, 1));
  HANDLE_CODE(bref.unpack(tx_comb_num_r13_present, 1));

  if (tx_comb_ap_v1310_present) {
    HANDLE_CODE(unpack_integer(tx_comb_ap_v1310, bref, (uint8_t)2u, (uint8_t)3u));
  }
  if (cyclic_shift_ap_v1310_present) {
    HANDLE_CODE(cyclic_shift_ap_v1310.unpack(bref));
  }
  if (tx_comb_num_r13_present) {
    HANDLE_CODE(tx_comb_num_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void srs_cfg_ap_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tx_comb_ap_v1310_present) {
    j.write_int("transmissionCombAp-v1310", tx_comb_ap_v1310);
  }
  if (cyclic_shift_ap_v1310_present) {
    j.write_str("cyclicShiftAp-v1310", cyclic_shift_ap_v1310.to_string());
  }
  if (tx_comb_num_r13_present) {
    j.write_str("transmissionCombNum-r13", tx_comb_num_r13.to_string());
  }
  j.end_obj();
}
bool srs_cfg_ap_v1310_s::operator==(const srs_cfg_ap_v1310_s& other) const
{
  return tx_comb_ap_v1310_present == other.tx_comb_ap_v1310_present and
         (not tx_comb_ap_v1310_present or tx_comb_ap_v1310 == other.tx_comb_ap_v1310) and
         cyclic_shift_ap_v1310_present == other.cyclic_shift_ap_v1310_present and
         (not cyclic_shift_ap_v1310_present or cyclic_shift_ap_v1310 == other.cyclic_shift_ap_v1310) and
         tx_comb_num_r13_present == other.tx_comb_num_r13_present and
         (not tx_comb_num_r13_present or tx_comb_num_r13 == other.tx_comb_num_r13);
}

const char* srs_cfg_ap_v1310_s::cyclic_shift_ap_v1310_opts::to_string() const
{
  static const char* options[] = {"cs8", "cs9", "cs10", "cs11"};
  return convert_enum_idx(options, 4, value, "srs_cfg_ap_v1310_s::cyclic_shift_ap_v1310_e_");
}
uint8_t srs_cfg_ap_v1310_s::cyclic_shift_ap_v1310_opts::to_number() const
{
  static const uint8_t options[] = {8, 9, 10, 11};
  return map_enum_number(options, 4, value, "srs_cfg_ap_v1310_s::cyclic_shift_ap_v1310_e_");
}

const char* srs_cfg_ap_v1310_s::tx_comb_num_r13_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "srs_cfg_ap_v1310_s::tx_comb_num_r13_e_");
}
uint8_t srs_cfg_ap_v1310_s::tx_comb_num_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "srs_cfg_ap_v1310_s::tx_comb_num_r13_e_");
}

// SchedulingRequestConfig-v1530 ::= CHOICE
void sched_request_cfg_v1530_c::set(types::options e)
{
  type_ = e;
}
void sched_request_cfg_v1530_c::set_release()
{
  set(types::release);
}
sched_request_cfg_v1530_c::setup_s_& sched_request_cfg_v1530_c::set_setup()
{
  set(types::setup);
  return c;
}
void sched_request_cfg_v1530_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.sr_slot_spucch_idx_fh_r15_present) {
        j.write_int("sr-SlotSPUCCH-IndexFH-r15", c.sr_slot_spucch_idx_fh_r15);
      }
      if (c.sr_slot_spucch_idx_no_fh_r15_present) {
        j.write_int("sr-SlotSPUCCH-IndexNoFH-r15", c.sr_slot_spucch_idx_no_fh_r15);
      }
      if (c.sr_subslot_spucch_res_list_r15_present) {
        j.start_array("sr-SubslotSPUCCH-ResourceList-r15");
        for (const auto& e1 : c.sr_subslot_spucch_res_list_r15) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.sr_cfg_idx_slot_r15_present) {
        j.write_int("sr-ConfigIndexSlot-r15", c.sr_cfg_idx_slot_r15);
      }
      if (c.sr_cfg_idx_subslot_r15_present) {
        j.write_int("sr-ConfigIndexSubslot-r15", c.sr_cfg_idx_subslot_r15);
      }
      j.write_str("dssr-TransMax-r15", c.dssr_trans_max_r15.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sched_request_cfg_v1530_c");
  }
  j.end_obj();
}
SRSASN_CODE sched_request_cfg_v1530_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.sr_slot_spucch_idx_fh_r15_present, 1));
      HANDLE_CODE(bref.pack(c.sr_slot_spucch_idx_no_fh_r15_present, 1));
      HANDLE_CODE(bref.pack(c.sr_subslot_spucch_res_list_r15_present, 1));
      HANDLE_CODE(bref.pack(c.sr_cfg_idx_slot_r15_present, 1));
      HANDLE_CODE(bref.pack(c.sr_cfg_idx_subslot_r15_present, 1));
      if (c.sr_slot_spucch_idx_fh_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.sr_slot_spucch_idx_fh_r15, (uint16_t)0u, (uint16_t)1319u));
      }
      if (c.sr_slot_spucch_idx_no_fh_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.sr_slot_spucch_idx_no_fh_r15, (uint16_t)0u, (uint16_t)3959u));
      }
      if (c.sr_subslot_spucch_res_list_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.sr_subslot_spucch_res_list_r15, 1, 4, integer_packer<uint16_t>(0, 1319)));
      }
      if (c.sr_cfg_idx_slot_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.sr_cfg_idx_slot_r15, (uint8_t)0u, (uint8_t)36u));
      }
      if (c.sr_cfg_idx_subslot_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.sr_cfg_idx_subslot_r15, (uint8_t)0u, (uint8_t)122u));
      }
      HANDLE_CODE(c.dssr_trans_max_r15.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sched_request_cfg_v1530_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_request_cfg_v1530_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.sr_slot_spucch_idx_fh_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.sr_slot_spucch_idx_no_fh_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.sr_subslot_spucch_res_list_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.sr_cfg_idx_slot_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.sr_cfg_idx_subslot_r15_present, 1));
      if (c.sr_slot_spucch_idx_fh_r15_present) {
        HANDLE_CODE(unpack_integer(c.sr_slot_spucch_idx_fh_r15, bref, (uint16_t)0u, (uint16_t)1319u));
      }
      if (c.sr_slot_spucch_idx_no_fh_r15_present) {
        HANDLE_CODE(unpack_integer(c.sr_slot_spucch_idx_no_fh_r15, bref, (uint16_t)0u, (uint16_t)3959u));
      }
      if (c.sr_subslot_spucch_res_list_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.sr_subslot_spucch_res_list_r15, bref, 1, 4, integer_packer<uint16_t>(0, 1319)));
      }
      if (c.sr_cfg_idx_slot_r15_present) {
        HANDLE_CODE(unpack_integer(c.sr_cfg_idx_slot_r15, bref, (uint8_t)0u, (uint8_t)36u));
      }
      if (c.sr_cfg_idx_subslot_r15_present) {
        HANDLE_CODE(unpack_integer(c.sr_cfg_idx_subslot_r15, bref, (uint8_t)0u, (uint8_t)122u));
      }
      HANDLE_CODE(c.dssr_trans_max_r15.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sched_request_cfg_v1530_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool sched_request_cfg_v1530_c::operator==(const sched_request_cfg_v1530_c& other) const
{
  return type() == other.type() and c.sr_slot_spucch_idx_fh_r15_present == other.c.sr_slot_spucch_idx_fh_r15_present and
         (not c.sr_slot_spucch_idx_fh_r15_present or
          c.sr_slot_spucch_idx_fh_r15 == other.c.sr_slot_spucch_idx_fh_r15) and
         c.sr_slot_spucch_idx_no_fh_r15_present == other.c.sr_slot_spucch_idx_no_fh_r15_present and
         (not c.sr_slot_spucch_idx_no_fh_r15_present or
          c.sr_slot_spucch_idx_no_fh_r15 == other.c.sr_slot_spucch_idx_no_fh_r15) and
         c.sr_subslot_spucch_res_list_r15_present == other.c.sr_subslot_spucch_res_list_r15_present and
         (not c.sr_subslot_spucch_res_list_r15_present or
          c.sr_subslot_spucch_res_list_r15 == other.c.sr_subslot_spucch_res_list_r15) and
         c.sr_cfg_idx_slot_r15_present == other.c.sr_cfg_idx_slot_r15_present and
         (not c.sr_cfg_idx_slot_r15_present or c.sr_cfg_idx_slot_r15 == other.c.sr_cfg_idx_slot_r15) and
         c.sr_cfg_idx_subslot_r15_present == other.c.sr_cfg_idx_subslot_r15_present and
         (not c.sr_cfg_idx_subslot_r15_present or c.sr_cfg_idx_subslot_r15 == other.c.sr_cfg_idx_subslot_r15) and
         c.dssr_trans_max_r15 == other.c.dssr_trans_max_r15;
}

const char* sched_request_cfg_v1530_c::setup_s_::dssr_trans_max_r15_opts::to_string() const
{
  static const char* options[] = {"n4", "n8", "n16", "n32", "n64", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "sched_request_cfg_v1530_c::setup_s_::dssr_trans_max_r15_e_");
}
uint8_t sched_request_cfg_v1530_c::setup_s_::dssr_trans_max_r15_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 16, 32, 64};
  return map_enum_number(options, 5, value, "sched_request_cfg_v1530_c::setup_s_::dssr_trans_max_r15_e_");
}

// ShortTTI-r15 ::= SEQUENCE
SRSASN_CODE short_tti_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dl_stti_len_r15_present, 1));
  HANDLE_CODE(bref.pack(ul_stti_len_r15_present, 1));

  if (dl_stti_len_r15_present) {
    HANDLE_CODE(dl_stti_len_r15.pack(bref));
  }
  if (ul_stti_len_r15_present) {
    HANDLE_CODE(ul_stti_len_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE short_tti_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dl_stti_len_r15_present, 1));
  HANDLE_CODE(bref.unpack(ul_stti_len_r15_present, 1));

  if (dl_stti_len_r15_present) {
    HANDLE_CODE(dl_stti_len_r15.unpack(bref));
  }
  if (ul_stti_len_r15_present) {
    HANDLE_CODE(ul_stti_len_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void short_tti_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_stti_len_r15_present) {
    j.write_str("dl-STTI-Length-r15", dl_stti_len_r15.to_string());
  }
  if (ul_stti_len_r15_present) {
    j.write_str("ul-STTI-Length-r15", ul_stti_len_r15.to_string());
  }
  j.end_obj();
}
bool short_tti_r15_s::operator==(const short_tti_r15_s& other) const
{
  return dl_stti_len_r15_present == other.dl_stti_len_r15_present and
         (not dl_stti_len_r15_present or dl_stti_len_r15 == other.dl_stti_len_r15) and
         ul_stti_len_r15_present == other.ul_stti_len_r15_present and
         (not ul_stti_len_r15_present or ul_stti_len_r15 == other.ul_stti_len_r15);
}

// SlotOrSubslotPDSCH-Config-r15 ::= CHOICE
void slot_or_subslot_pdsch_cfg_r15_c::set(types::options e)
{
  type_ = e;
}
void slot_or_subslot_pdsch_cfg_r15_c::set_release()
{
  set(types::release);
}
slot_or_subslot_pdsch_cfg_r15_c::setup_s_& slot_or_subslot_pdsch_cfg_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void slot_or_subslot_pdsch_cfg_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.alt_cqi_table_stti_r15_present) {
        j.write_str("altCQI-TableSTTI-r15", c.alt_cqi_table_stti_r15.to_string());
      }
      if (c.alt_cqi_table1024_qam_stti_r15_present) {
        j.write_str("altCQI-Table1024QAM-STTI-r15", c.alt_cqi_table1024_qam_stti_r15.to_string());
      }
      if (c.res_alloc_r15_present) {
        j.write_str("resourceAllocation-r15", c.res_alloc_r15.to_string());
      }
      if (c.tbs_idx_alt_stti_r15_present) {
        j.write_str("tbsIndexAlt-STTI-r15", "a33");
      }
      if (c.tbs_idx_alt2_stti_r15_present) {
        j.write_str("tbsIndexAlt2-STTI-r15", "b33");
      }
      if (c.tbs_idx_alt3_stti_r15_present) {
        j.write_str("tbsIndexAlt3-STTI-r15", "a37");
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "slot_or_subslot_pdsch_cfg_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE slot_or_subslot_pdsch_cfg_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.pack(c.ext, 1);
      HANDLE_CODE(bref.pack(c.alt_cqi_table_stti_r15_present, 1));
      HANDLE_CODE(bref.pack(c.alt_cqi_table1024_qam_stti_r15_present, 1));
      HANDLE_CODE(bref.pack(c.res_alloc_r15_present, 1));
      HANDLE_CODE(bref.pack(c.tbs_idx_alt_stti_r15_present, 1));
      HANDLE_CODE(bref.pack(c.tbs_idx_alt2_stti_r15_present, 1));
      HANDLE_CODE(bref.pack(c.tbs_idx_alt3_stti_r15_present, 1));
      if (c.alt_cqi_table_stti_r15_present) {
        HANDLE_CODE(c.alt_cqi_table_stti_r15.pack(bref));
      }
      if (c.alt_cqi_table1024_qam_stti_r15_present) {
        HANDLE_CODE(c.alt_cqi_table1024_qam_stti_r15.pack(bref));
      }
      if (c.res_alloc_r15_present) {
        HANDLE_CODE(c.res_alloc_r15.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "slot_or_subslot_pdsch_cfg_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE slot_or_subslot_pdsch_cfg_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(bref.unpack(c.alt_cqi_table_stti_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.alt_cqi_table1024_qam_stti_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.res_alloc_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.tbs_idx_alt_stti_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.tbs_idx_alt2_stti_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.tbs_idx_alt3_stti_r15_present, 1));
      if (c.alt_cqi_table_stti_r15_present) {
        HANDLE_CODE(c.alt_cqi_table_stti_r15.unpack(bref));
      }
      if (c.alt_cqi_table1024_qam_stti_r15_present) {
        HANDLE_CODE(c.alt_cqi_table1024_qam_stti_r15.unpack(bref));
      }
      if (c.res_alloc_r15_present) {
        HANDLE_CODE(c.res_alloc_r15.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "slot_or_subslot_pdsch_cfg_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool slot_or_subslot_pdsch_cfg_r15_c::operator==(const slot_or_subslot_pdsch_cfg_r15_c& other) const
{
  return type() == other.type() and c.ext == other.c.ext and
         c.alt_cqi_table_stti_r15_present == other.c.alt_cqi_table_stti_r15_present and
         (not c.alt_cqi_table_stti_r15_present or c.alt_cqi_table_stti_r15 == other.c.alt_cqi_table_stti_r15) and
         c.alt_cqi_table1024_qam_stti_r15_present == other.c.alt_cqi_table1024_qam_stti_r15_present and
         (not c.alt_cqi_table1024_qam_stti_r15_present or
          c.alt_cqi_table1024_qam_stti_r15 == other.c.alt_cqi_table1024_qam_stti_r15) and
         c.res_alloc_r15_present == other.c.res_alloc_r15_present and
         (not c.res_alloc_r15_present or c.res_alloc_r15 == other.c.res_alloc_r15) and
         c.tbs_idx_alt_stti_r15_present == other.c.tbs_idx_alt_stti_r15_present and
         c.tbs_idx_alt2_stti_r15_present == other.c.tbs_idx_alt2_stti_r15_present and
         c.tbs_idx_alt3_stti_r15_present == other.c.tbs_idx_alt3_stti_r15_present;
}

const char* slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table_stti_r15_opts::to_string() const
{
  static const char* options[] = {"allSubframes", "csi-SubframeSet1", "csi-SubframeSet2", "spare1"};
  return convert_enum_idx(options, 4, value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table_stti_r15_e_");
}
uint8_t slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table_stti_r15_opts::to_number() const
{
  switch (value) {
    case csi_sf_set1:
      return 1;
    case csi_sf_set2:
      return 2;
    default:
      invalid_enum_number(value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table_stti_r15_e_");
  }
  return 0;
}

const char* slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table1024_qam_stti_r15_opts::to_string() const
{
  static const char* options[] = {"allSubframes", "csi-SubframeSet1", "csi-SubframeSet2", "spare1"};
  return convert_enum_idx(
      options, 4, value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table1024_qam_stti_r15_e_");
}
uint8_t slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table1024_qam_stti_r15_opts::to_number() const
{
  switch (value) {
    case csi_sf_set1:
      return 1;
    case csi_sf_set2:
      return 2;
    default:
      invalid_enum_number(value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table1024_qam_stti_r15_e_");
  }
  return 0;
}

const char* slot_or_subslot_pdsch_cfg_r15_c::setup_s_::res_alloc_r15_opts::to_string() const
{
  static const char* options[] = {"resourceAllocationType0", "resourceAllocationType2"};
  return convert_enum_idx(options, 2, value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::res_alloc_r15_e_");
}
uint8_t slot_or_subslot_pdsch_cfg_r15_c::setup_s_::res_alloc_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 2};
  return map_enum_number(options, 2, value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::res_alloc_r15_e_");
}

// SlotOrSubslotPUSCH-Config-r15 ::= CHOICE
void slot_or_subslot_pusch_cfg_r15_c::set(types::options e)
{
  type_ = e;
}
void slot_or_subslot_pusch_cfg_r15_c::set_release()
{
  set(types::release);
}
slot_or_subslot_pusch_cfg_r15_c::setup_s_& slot_or_subslot_pusch_cfg_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void slot_or_subslot_pusch_cfg_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.beta_offset_slot_ack_idx_r15_present) {
        j.write_int("betaOffsetSlot-ACK-Index-r15", c.beta_offset_slot_ack_idx_r15);
      }
      if (c.beta_offset2_slot_ack_idx_r15_present) {
        j.write_int("betaOffset2Slot-ACK-Index-r15", c.beta_offset2_slot_ack_idx_r15);
      }
      if (c.beta_offset_subslot_ack_idx_r15_present) {
        j.start_array("betaOffsetSubslot-ACK-Index-r15");
        for (const auto& e1 : c.beta_offset_subslot_ack_idx_r15) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.beta_offset2_subslot_ack_idx_r15_present) {
        j.start_array("betaOffset2Subslot-ACK-Index-r15");
        for (const auto& e1 : c.beta_offset2_subslot_ack_idx_r15) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.beta_offset_slot_ri_idx_r15_present) {
        j.write_int("betaOffsetSlot-RI-Index-r15", c.beta_offset_slot_ri_idx_r15);
      }
      if (c.beta_offset_subslot_ri_idx_r15_present) {
        j.start_array("betaOffsetSubslot-RI-Index-r15");
        for (const auto& e1 : c.beta_offset_subslot_ri_idx_r15) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.beta_offset_slot_cqi_idx_r15_present) {
        j.write_int("betaOffsetSlot-CQI-Index-r15", c.beta_offset_slot_cqi_idx_r15);
      }
      if (c.beta_offset_subslot_cqi_idx_r15_present) {
        j.write_int("betaOffsetSubslot-CQI-Index-r15", c.beta_offset_subslot_cqi_idx_r15);
      }
      if (c.enable256_qam_slot_or_subslot_r15_present) {
        j.write_fieldname("enable256QAM-SlotOrSubslot-r15");
        c.enable256_qam_slot_or_subslot_r15.to_json(j);
      }
      if (c.res_alloc_offset_r15_present) {
        j.write_int("resourceAllocationOffset-r15", c.res_alloc_offset_r15);
      }
      j.write_bool("ul-DMRS-IFDMA-SlotOrSubslot-r15", c.ul_dmrs_ifdma_slot_or_subslot_r15);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "slot_or_subslot_pusch_cfg_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE slot_or_subslot_pusch_cfg_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.pack(c.ext, 1);
      HANDLE_CODE(bref.pack(c.beta_offset_slot_ack_idx_r15_present, 1));
      HANDLE_CODE(bref.pack(c.beta_offset2_slot_ack_idx_r15_present, 1));
      HANDLE_CODE(bref.pack(c.beta_offset_subslot_ack_idx_r15_present, 1));
      HANDLE_CODE(bref.pack(c.beta_offset2_subslot_ack_idx_r15_present, 1));
      HANDLE_CODE(bref.pack(c.beta_offset_slot_ri_idx_r15_present, 1));
      HANDLE_CODE(bref.pack(c.beta_offset_subslot_ri_idx_r15_present, 1));
      HANDLE_CODE(bref.pack(c.beta_offset_slot_cqi_idx_r15_present, 1));
      HANDLE_CODE(bref.pack(c.beta_offset_subslot_cqi_idx_r15_present, 1));
      HANDLE_CODE(bref.pack(c.enable256_qam_slot_or_subslot_r15_present, 1));
      HANDLE_CODE(bref.pack(c.res_alloc_offset_r15_present, 1));
      if (c.beta_offset_slot_ack_idx_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.beta_offset_slot_ack_idx_r15, (uint8_t)0u, (uint8_t)15u));
      }
      if (c.beta_offset2_slot_ack_idx_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.beta_offset2_slot_ack_idx_r15, (uint8_t)0u, (uint8_t)15u));
      }
      if (c.beta_offset_subslot_ack_idx_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.beta_offset_subslot_ack_idx_r15, 1, 2, integer_packer<uint8_t>(0, 15)));
      }
      if (c.beta_offset2_subslot_ack_idx_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.beta_offset2_subslot_ack_idx_r15, 1, 2, integer_packer<uint8_t>(0, 15)));
      }
      if (c.beta_offset_slot_ri_idx_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.beta_offset_slot_ri_idx_r15, (uint8_t)0u, (uint8_t)15u));
      }
      if (c.beta_offset_subslot_ri_idx_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.beta_offset_subslot_ri_idx_r15, 1, 2, integer_packer<uint8_t>(0, 15)));
      }
      if (c.beta_offset_slot_cqi_idx_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.beta_offset_slot_cqi_idx_r15, (uint8_t)0u, (uint8_t)15u));
      }
      if (c.beta_offset_subslot_cqi_idx_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.beta_offset_subslot_cqi_idx_r15, (uint8_t)0u, (uint8_t)15u));
      }
      if (c.enable256_qam_slot_or_subslot_r15_present) {
        HANDLE_CODE(c.enable256_qam_slot_or_subslot_r15.pack(bref));
      }
      if (c.res_alloc_offset_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.res_alloc_offset_r15, (uint8_t)1u, (uint8_t)2u));
      }
      HANDLE_CODE(bref.pack(c.ul_dmrs_ifdma_slot_or_subslot_r15, 1));
      break;
    default:
      log_invalid_choice_id(type_, "slot_or_subslot_pusch_cfg_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE slot_or_subslot_pusch_cfg_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(bref.unpack(c.beta_offset_slot_ack_idx_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.beta_offset2_slot_ack_idx_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.beta_offset_subslot_ack_idx_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.beta_offset2_subslot_ack_idx_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.beta_offset_slot_ri_idx_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.beta_offset_subslot_ri_idx_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.beta_offset_slot_cqi_idx_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.beta_offset_subslot_cqi_idx_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.enable256_qam_slot_or_subslot_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.res_alloc_offset_r15_present, 1));
      if (c.beta_offset_slot_ack_idx_r15_present) {
        HANDLE_CODE(unpack_integer(c.beta_offset_slot_ack_idx_r15, bref, (uint8_t)0u, (uint8_t)15u));
      }
      if (c.beta_offset2_slot_ack_idx_r15_present) {
        HANDLE_CODE(unpack_integer(c.beta_offset2_slot_ack_idx_r15, bref, (uint8_t)0u, (uint8_t)15u));
      }
      if (c.beta_offset_subslot_ack_idx_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.beta_offset_subslot_ack_idx_r15, bref, 1, 2, integer_packer<uint8_t>(0, 15)));
      }
      if (c.beta_offset2_subslot_ack_idx_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.beta_offset2_subslot_ack_idx_r15, bref, 1, 2, integer_packer<uint8_t>(0, 15)));
      }
      if (c.beta_offset_slot_ri_idx_r15_present) {
        HANDLE_CODE(unpack_integer(c.beta_offset_slot_ri_idx_r15, bref, (uint8_t)0u, (uint8_t)15u));
      }
      if (c.beta_offset_subslot_ri_idx_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.beta_offset_subslot_ri_idx_r15, bref, 1, 2, integer_packer<uint8_t>(0, 15)));
      }
      if (c.beta_offset_slot_cqi_idx_r15_present) {
        HANDLE_CODE(unpack_integer(c.beta_offset_slot_cqi_idx_r15, bref, (uint8_t)0u, (uint8_t)15u));
      }
      if (c.beta_offset_subslot_cqi_idx_r15_present) {
        HANDLE_CODE(unpack_integer(c.beta_offset_subslot_cqi_idx_r15, bref, (uint8_t)0u, (uint8_t)15u));
      }
      if (c.enable256_qam_slot_or_subslot_r15_present) {
        HANDLE_CODE(c.enable256_qam_slot_or_subslot_r15.unpack(bref));
      }
      if (c.res_alloc_offset_r15_present) {
        HANDLE_CODE(unpack_integer(c.res_alloc_offset_r15, bref, (uint8_t)1u, (uint8_t)2u));
      }
      HANDLE_CODE(bref.unpack(c.ul_dmrs_ifdma_slot_or_subslot_r15, 1));
      break;
    default:
      log_invalid_choice_id(type_, "slot_or_subslot_pusch_cfg_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool slot_or_subslot_pusch_cfg_r15_c::operator==(const slot_or_subslot_pusch_cfg_r15_c& other) const
{
  return type() == other.type() and c.ext == other.c.ext and
         c.beta_offset_slot_ack_idx_r15_present == other.c.beta_offset_slot_ack_idx_r15_present and
         (not c.beta_offset_slot_ack_idx_r15_present or
          c.beta_offset_slot_ack_idx_r15 == other.c.beta_offset_slot_ack_idx_r15) and
         c.beta_offset2_slot_ack_idx_r15_present == other.c.beta_offset2_slot_ack_idx_r15_present and
         (not c.beta_offset2_slot_ack_idx_r15_present or
          c.beta_offset2_slot_ack_idx_r15 == other.c.beta_offset2_slot_ack_idx_r15) and
         c.beta_offset_subslot_ack_idx_r15_present == other.c.beta_offset_subslot_ack_idx_r15_present and
         (not c.beta_offset_subslot_ack_idx_r15_present or
          c.beta_offset_subslot_ack_idx_r15 == other.c.beta_offset_subslot_ack_idx_r15) and
         c.beta_offset2_subslot_ack_idx_r15_present == other.c.beta_offset2_subslot_ack_idx_r15_present and
         (not c.beta_offset2_subslot_ack_idx_r15_present or
          c.beta_offset2_subslot_ack_idx_r15 == other.c.beta_offset2_subslot_ack_idx_r15) and
         c.beta_offset_slot_ri_idx_r15_present == other.c.beta_offset_slot_ri_idx_r15_present and
         (not c.beta_offset_slot_ri_idx_r15_present or
          c.beta_offset_slot_ri_idx_r15 == other.c.beta_offset_slot_ri_idx_r15) and
         c.beta_offset_subslot_ri_idx_r15_present == other.c.beta_offset_subslot_ri_idx_r15_present and
         (not c.beta_offset_subslot_ri_idx_r15_present or
          c.beta_offset_subslot_ri_idx_r15 == other.c.beta_offset_subslot_ri_idx_r15) and
         c.beta_offset_slot_cqi_idx_r15_present == other.c.beta_offset_slot_cqi_idx_r15_present and
         (not c.beta_offset_slot_cqi_idx_r15_present or
          c.beta_offset_slot_cqi_idx_r15 == other.c.beta_offset_slot_cqi_idx_r15) and
         c.beta_offset_subslot_cqi_idx_r15_present == other.c.beta_offset_subslot_cqi_idx_r15_present and
         (not c.beta_offset_subslot_cqi_idx_r15_present or
          c.beta_offset_subslot_cqi_idx_r15 == other.c.beta_offset_subslot_cqi_idx_r15) and
         c.enable256_qam_slot_or_subslot_r15_present == other.c.enable256_qam_slot_or_subslot_r15_present and
         (not c.enable256_qam_slot_or_subslot_r15_present or
          c.enable256_qam_slot_or_subslot_r15 == other.c.enable256_qam_slot_or_subslot_r15) and
         c.res_alloc_offset_r15_present == other.c.res_alloc_offset_r15_present and
         (not c.res_alloc_offset_r15_present or c.res_alloc_offset_r15 == other.c.res_alloc_offset_r15) and
         c.ul_dmrs_ifdma_slot_or_subslot_r15 == other.c.ul_dmrs_ifdma_slot_or_subslot_r15;
}

// TDD-PUSCH-UpPTS-r14 ::= CHOICE
void tdd_pusch_up_pts_r14_c::set(types::options e)
{
  type_ = e;
}
void tdd_pusch_up_pts_r14_c::set_release()
{
  set(types::release);
}
tdd_pusch_up_pts_r14_c::setup_s_& tdd_pusch_up_pts_r14_c::set_setup()
{
  set(types::setup);
  return c;
}
void tdd_pusch_up_pts_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.sym_pusch_up_pts_r14_present) {
        j.write_str("symPUSCH-UpPTS-r14", c.sym_pusch_up_pts_r14.to_string());
      }
      if (c.dmrs_less_up_pts_cfg_r14_present) {
        j.write_str("dmrs-LessUpPTS-Config-r14", "true");
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "tdd_pusch_up_pts_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE tdd_pusch_up_pts_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.sym_pusch_up_pts_r14_present, 1));
      HANDLE_CODE(bref.pack(c.dmrs_less_up_pts_cfg_r14_present, 1));
      if (c.sym_pusch_up_pts_r14_present) {
        HANDLE_CODE(c.sym_pusch_up_pts_r14.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "tdd_pusch_up_pts_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_pusch_up_pts_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.sym_pusch_up_pts_r14_present, 1));
      HANDLE_CODE(bref.unpack(c.dmrs_less_up_pts_cfg_r14_present, 1));
      if (c.sym_pusch_up_pts_r14_present) {
        HANDLE_CODE(c.sym_pusch_up_pts_r14.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "tdd_pusch_up_pts_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* tdd_pusch_up_pts_r14_c::setup_s_::sym_pusch_up_pts_r14_opts::to_string() const
{
  static const char* options[] = {"sym1", "sym2", "sym3", "sym4", "sym5", "sym6"};
  return convert_enum_idx(options, 6, value, "tdd_pusch_up_pts_r14_c::setup_s_::sym_pusch_up_pts_r14_e_");
}
uint8_t tdd_pusch_up_pts_r14_c::setup_s_::sym_pusch_up_pts_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 6, value, "tdd_pusch_up_pts_r14_c::setup_s_::sym_pusch_up_pts_r14_e_");
}

// UplinkPowerControlDedicatedSTTI-r15 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_ded_stti_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(delta_tx_d_offset_list_spucch_r15_present, 1));

  HANDLE_CODE(bref.pack(accumulation_enabled_stti_r15, 1));
  if (delta_tx_d_offset_list_spucch_r15_present) {
    HANDLE_CODE(delta_tx_d_offset_list_spucch_r15.pack(bref));
  }
  HANDLE_CODE(bref.pack(ul_pwr_csi_payload, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_ded_stti_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(delta_tx_d_offset_list_spucch_r15_present, 1));

  HANDLE_CODE(bref.unpack(accumulation_enabled_stti_r15, 1));
  if (delta_tx_d_offset_list_spucch_r15_present) {
    HANDLE_CODE(delta_tx_d_offset_list_spucch_r15.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(ul_pwr_csi_payload, 1));

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_ded_stti_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("accumulationEnabledSTTI-r15", accumulation_enabled_stti_r15);
  if (delta_tx_d_offset_list_spucch_r15_present) {
    j.write_fieldname("deltaTxD-OffsetListSPUCCH-r15");
    delta_tx_d_offset_list_spucch_r15.to_json(j);
  }
  j.write_bool("uplinkPower-CSIPayload", ul_pwr_csi_payload);
  j.end_obj();
}
bool ul_pwr_ctrl_ded_stti_r15_s::operator==(const ul_pwr_ctrl_ded_stti_r15_s& other) const
{
  return accumulation_enabled_stti_r15 == other.accumulation_enabled_stti_r15 and
         delta_tx_d_offset_list_spucch_r15_present == other.delta_tx_d_offset_list_spucch_r15_present and
         (not delta_tx_d_offset_list_spucch_r15_present or
          delta_tx_d_offset_list_spucch_r15 == other.delta_tx_d_offset_list_spucch_r15) and
         ul_pwr_csi_payload == other.ul_pwr_csi_payload;
}

// AntennaInfoDedicated ::= SEQUENCE
SRSASN_CODE ant_info_ded_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(codebook_subset_restrict_present, 1));

  HANDLE_CODE(tx_mode.pack(bref));
  if (codebook_subset_restrict_present) {
    HANDLE_CODE(codebook_subset_restrict.pack(bref));
  }
  HANDLE_CODE(ue_tx_ant_sel.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ded_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(codebook_subset_restrict_present, 1));

  HANDLE_CODE(tx_mode.unpack(bref));
  if (codebook_subset_restrict_present) {
    HANDLE_CODE(codebook_subset_restrict.unpack(bref));
  }
  HANDLE_CODE(ue_tx_ant_sel.unpack(bref));

  return SRSASN_SUCCESS;
}
void ant_info_ded_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("transmissionMode", tx_mode.to_string());
  if (codebook_subset_restrict_present) {
    j.write_fieldname("codebookSubsetRestriction");
    codebook_subset_restrict.to_json(j);
  }
  j.write_fieldname("ue-TransmitAntennaSelection");
  ue_tx_ant_sel.to_json(j);
  j.end_obj();
}

const char* ant_info_ded_s::tx_mode_opts::to_string() const
{
  static const char* options[] = {"tm1", "tm2", "tm3", "tm4", "tm5", "tm6", "tm7", "tm8-v920"};
  return convert_enum_idx(options, 8, value, "ant_info_ded_s::tx_mode_e_");
}
uint8_t ant_info_ded_s::tx_mode_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8};
  return map_enum_number(options, 8, value, "ant_info_ded_s::tx_mode_e_");
}

void ant_info_ded_s::codebook_subset_restrict_c_::destroy_()
{
  switch (type_) {
    case types::n2_tx_ant_tm3:
      c.destroy<fixed_bitstring<2> >();
      break;
    case types::n4_tx_ant_tm3:
      c.destroy<fixed_bitstring<4> >();
      break;
    case types::n2_tx_ant_tm4:
      c.destroy<fixed_bitstring<6> >();
      break;
    case types::n4_tx_ant_tm4:
      c.destroy<fixed_bitstring<64> >();
      break;
    case types::n2_tx_ant_tm5:
      c.destroy<fixed_bitstring<4> >();
      break;
    case types::n4_tx_ant_tm5:
      c.destroy<fixed_bitstring<16> >();
      break;
    case types::n2_tx_ant_tm6:
      c.destroy<fixed_bitstring<4> >();
      break;
    case types::n4_tx_ant_tm6:
      c.destroy<fixed_bitstring<16> >();
      break;
    default:
      break;
  }
}
void ant_info_ded_s::codebook_subset_restrict_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::n2_tx_ant_tm3:
      c.init<fixed_bitstring<2> >();
      break;
    case types::n4_tx_ant_tm3:
      c.init<fixed_bitstring<4> >();
      break;
    case types::n2_tx_ant_tm4:
      c.init<fixed_bitstring<6> >();
      break;
    case types::n4_tx_ant_tm4:
      c.init<fixed_bitstring<64> >();
      break;
    case types::n2_tx_ant_tm5:
      c.init<fixed_bitstring<4> >();
      break;
    case types::n4_tx_ant_tm5:
      c.init<fixed_bitstring<16> >();
      break;
    case types::n2_tx_ant_tm6:
      c.init<fixed_bitstring<4> >();
      break;
    case types::n4_tx_ant_tm6:
      c.init<fixed_bitstring<16> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_s::codebook_subset_restrict_c_");
  }
}
ant_info_ded_s::codebook_subset_restrict_c_::codebook_subset_restrict_c_(
    const ant_info_ded_s::codebook_subset_restrict_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::n2_tx_ant_tm3:
      c.init(other.c.get<fixed_bitstring<2> >());
      break;
    case types::n4_tx_ant_tm3:
      c.init(other.c.get<fixed_bitstring<4> >());
      break;
    case types::n2_tx_ant_tm4:
      c.init(other.c.get<fixed_bitstring<6> >());
      break;
    case types::n4_tx_ant_tm4:
      c.init(other.c.get<fixed_bitstring<64> >());
      break;
    case types::n2_tx_ant_tm5:
      c.init(other.c.get<fixed_bitstring<4> >());
      break;
    case types::n4_tx_ant_tm5:
      c.init(other.c.get<fixed_bitstring<16> >());
      break;
    case types::n2_tx_ant_tm6:
      c.init(other.c.get<fixed_bitstring<4> >());
      break;
    case types::n4_tx_ant_tm6:
      c.init(other.c.get<fixed_bitstring<16> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_s::codebook_subset_restrict_c_");
  }
}
ant_info_ded_s::codebook_subset_restrict_c_&
ant_info_ded_s::codebook_subset_restrict_c_::operator=(const ant_info_ded_s::codebook_subset_restrict_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::n2_tx_ant_tm3:
      c.set(other.c.get<fixed_bitstring<2> >());
      break;
    case types::n4_tx_ant_tm3:
      c.set(other.c.get<fixed_bitstring<4> >());
      break;
    case types::n2_tx_ant_tm4:
      c.set(other.c.get<fixed_bitstring<6> >());
      break;
    case types::n4_tx_ant_tm4:
      c.set(other.c.get<fixed_bitstring<64> >());
      break;
    case types::n2_tx_ant_tm5:
      c.set(other.c.get<fixed_bitstring<4> >());
      break;
    case types::n4_tx_ant_tm5:
      c.set(other.c.get<fixed_bitstring<16> >());
      break;
    case types::n2_tx_ant_tm6:
      c.set(other.c.get<fixed_bitstring<4> >());
      break;
    case types::n4_tx_ant_tm6:
      c.set(other.c.get<fixed_bitstring<16> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_s::codebook_subset_restrict_c_");
  }

  return *this;
}
fixed_bitstring<2>& ant_info_ded_s::codebook_subset_restrict_c_::set_n2_tx_ant_tm3()
{
  set(types::n2_tx_ant_tm3);
  return c.get<fixed_bitstring<2> >();
}
fixed_bitstring<4>& ant_info_ded_s::codebook_subset_restrict_c_::set_n4_tx_ant_tm3()
{
  set(types::n4_tx_ant_tm3);
  return c.get<fixed_bitstring<4> >();
}
fixed_bitstring<6>& ant_info_ded_s::codebook_subset_restrict_c_::set_n2_tx_ant_tm4()
{
  set(types::n2_tx_ant_tm4);
  return c.get<fixed_bitstring<6> >();
}
fixed_bitstring<64>& ant_info_ded_s::codebook_subset_restrict_c_::set_n4_tx_ant_tm4()
{
  set(types::n4_tx_ant_tm4);
  return c.get<fixed_bitstring<64> >();
}
fixed_bitstring<4>& ant_info_ded_s::codebook_subset_restrict_c_::set_n2_tx_ant_tm5()
{
  set(types::n2_tx_ant_tm5);
  return c.get<fixed_bitstring<4> >();
}
fixed_bitstring<16>& ant_info_ded_s::codebook_subset_restrict_c_::set_n4_tx_ant_tm5()
{
  set(types::n4_tx_ant_tm5);
  return c.get<fixed_bitstring<16> >();
}
fixed_bitstring<4>& ant_info_ded_s::codebook_subset_restrict_c_::set_n2_tx_ant_tm6()
{
  set(types::n2_tx_ant_tm6);
  return c.get<fixed_bitstring<4> >();
}
fixed_bitstring<16>& ant_info_ded_s::codebook_subset_restrict_c_::set_n4_tx_ant_tm6()
{
  set(types::n4_tx_ant_tm6);
  return c.get<fixed_bitstring<16> >();
}
void ant_info_ded_s::codebook_subset_restrict_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::n2_tx_ant_tm3:
      j.write_str("n2TxAntenna-tm3", c.get<fixed_bitstring<2> >().to_string());
      break;
    case types::n4_tx_ant_tm3:
      j.write_str("n4TxAntenna-tm3", c.get<fixed_bitstring<4> >().to_string());
      break;
    case types::n2_tx_ant_tm4:
      j.write_str("n2TxAntenna-tm4", c.get<fixed_bitstring<6> >().to_string());
      break;
    case types::n4_tx_ant_tm4:
      j.write_str("n4TxAntenna-tm4", c.get<fixed_bitstring<64> >().to_string());
      break;
    case types::n2_tx_ant_tm5:
      j.write_str("n2TxAntenna-tm5", c.get<fixed_bitstring<4> >().to_string());
      break;
    case types::n4_tx_ant_tm5:
      j.write_str("n4TxAntenna-tm5", c.get<fixed_bitstring<16> >().to_string());
      break;
    case types::n2_tx_ant_tm6:
      j.write_str("n2TxAntenna-tm6", c.get<fixed_bitstring<4> >().to_string());
      break;
    case types::n4_tx_ant_tm6:
      j.write_str("n4TxAntenna-tm6", c.get<fixed_bitstring<16> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_s::codebook_subset_restrict_c_");
  }
  j.end_obj();
}
SRSASN_CODE ant_info_ded_s::codebook_subset_restrict_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::n2_tx_ant_tm3:
      HANDLE_CODE(c.get<fixed_bitstring<2> >().pack(bref));
      break;
    case types::n4_tx_ant_tm3:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().pack(bref));
      break;
    case types::n2_tx_ant_tm4:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().pack(bref));
      break;
    case types::n4_tx_ant_tm4:
      HANDLE_CODE(c.get<fixed_bitstring<64> >().pack(bref));
      break;
    case types::n2_tx_ant_tm5:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().pack(bref));
      break;
    case types::n4_tx_ant_tm5:
      HANDLE_CODE(c.get<fixed_bitstring<16> >().pack(bref));
      break;
    case types::n2_tx_ant_tm6:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().pack(bref));
      break;
    case types::n4_tx_ant_tm6:
      HANDLE_CODE(c.get<fixed_bitstring<16> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_s::codebook_subset_restrict_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ded_s::codebook_subset_restrict_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::n2_tx_ant_tm3:
      HANDLE_CODE(c.get<fixed_bitstring<2> >().unpack(bref));
      break;
    case types::n4_tx_ant_tm3:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().unpack(bref));
      break;
    case types::n2_tx_ant_tm4:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().unpack(bref));
      break;
    case types::n4_tx_ant_tm4:
      HANDLE_CODE(c.get<fixed_bitstring<64> >().unpack(bref));
      break;
    case types::n2_tx_ant_tm5:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().unpack(bref));
      break;
    case types::n4_tx_ant_tm5:
      HANDLE_CODE(c.get<fixed_bitstring<16> >().unpack(bref));
      break;
    case types::n2_tx_ant_tm6:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().unpack(bref));
      break;
    case types::n4_tx_ant_tm6:
      HANDLE_CODE(c.get<fixed_bitstring<16> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_s::codebook_subset_restrict_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ant_info_ded_s::ue_tx_ant_sel_c_::set(types::options e)
{
  type_ = e;
}
void ant_info_ded_s::ue_tx_ant_sel_c_::set_release()
{
  set(types::release);
}
ant_info_ded_s::ue_tx_ant_sel_c_::setup_e_& ant_info_ded_s::ue_tx_ant_sel_c_::set_setup()
{
  set(types::setup);
  return c;
}
void ant_info_ded_s::ue_tx_ant_sel_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_str("setup", c.to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_s::ue_tx_ant_sel_c_");
  }
  j.end_obj();
}
SRSASN_CODE ant_info_ded_s::ue_tx_ant_sel_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_s::ue_tx_ant_sel_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ded_s::ue_tx_ant_sel_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_s::ue_tx_ant_sel_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ant_info_ded_s::ue_tx_ant_sel_c_::setup_opts::to_string() const
{
  static const char* options[] = {"closedLoop", "openLoop"};
  return convert_enum_idx(options, 2, value, "ant_info_ded_s::ue_tx_ant_sel_c_::setup_e_");
}

// AntennaInfoDedicated-r10 ::= SEQUENCE
SRSASN_CODE ant_info_ded_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(codebook_subset_restrict_r10_present, 1));

  HANDLE_CODE(tx_mode_r10.pack(bref));
  if (codebook_subset_restrict_r10_present) {
    HANDLE_CODE(codebook_subset_restrict_r10.pack(bref));
  }
  HANDLE_CODE(ue_tx_ant_sel.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ded_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(codebook_subset_restrict_r10_present, 1));

  HANDLE_CODE(tx_mode_r10.unpack(bref));
  if (codebook_subset_restrict_r10_present) {
    HANDLE_CODE(codebook_subset_restrict_r10.unpack(bref));
  }
  HANDLE_CODE(ue_tx_ant_sel.unpack(bref));

  return SRSASN_SUCCESS;
}
void ant_info_ded_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("transmissionMode-r10", tx_mode_r10.to_string());
  if (codebook_subset_restrict_r10_present) {
    j.write_str("codebookSubsetRestriction-r10", codebook_subset_restrict_r10.to_string());
  }
  j.write_fieldname("ue-TransmitAntennaSelection");
  ue_tx_ant_sel.to_json(j);
  j.end_obj();
}
bool ant_info_ded_r10_s::operator==(const ant_info_ded_r10_s& other) const
{
  return tx_mode_r10 == other.tx_mode_r10 and
         codebook_subset_restrict_r10_present == other.codebook_subset_restrict_r10_present and
         (not codebook_subset_restrict_r10_present or
          codebook_subset_restrict_r10 == other.codebook_subset_restrict_r10) and
         ue_tx_ant_sel == other.ue_tx_ant_sel;
}

const char* ant_info_ded_r10_s::tx_mode_r10_opts::to_string() const
{
  static const char* options[] = {"tm1",
                                  "tm2",
                                  "tm3",
                                  "tm4",
                                  "tm5",
                                  "tm6",
                                  "tm7",
                                  "tm8-v920",
                                  "tm9-v1020",
                                  "tm10-v1130",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "ant_info_ded_r10_s::tx_mode_r10_e_");
}
uint8_t ant_info_ded_r10_s::tx_mode_r10_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  return map_enum_number(options, 10, value, "ant_info_ded_r10_s::tx_mode_r10_e_");
}

void ant_info_ded_r10_s::ue_tx_ant_sel_c_::set(types::options e)
{
  type_ = e;
}
void ant_info_ded_r10_s::ue_tx_ant_sel_c_::set_release()
{
  set(types::release);
}
ant_info_ded_r10_s::ue_tx_ant_sel_c_::setup_e_& ant_info_ded_r10_s::ue_tx_ant_sel_c_::set_setup()
{
  set(types::setup);
  return c;
}
void ant_info_ded_r10_s::ue_tx_ant_sel_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_str("setup", c.to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_r10_s::ue_tx_ant_sel_c_");
  }
  j.end_obj();
}
SRSASN_CODE ant_info_ded_r10_s::ue_tx_ant_sel_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_r10_s::ue_tx_ant_sel_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ded_r10_s::ue_tx_ant_sel_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_r10_s::ue_tx_ant_sel_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool ant_info_ded_r10_s::ue_tx_ant_sel_c_::operator==(const ue_tx_ant_sel_c_& other) const
{
  return type() == other.type() and c == other.c;
}

const char* ant_info_ded_r10_s::ue_tx_ant_sel_c_::setup_opts::to_string() const
{
  static const char* options[] = {"closedLoop", "openLoop"};
  return convert_enum_idx(options, 2, value, "ant_info_ded_r10_s::ue_tx_ant_sel_c_::setup_e_");
}

// AntennaInfoDedicated-v1250 ::= SEQUENCE
SRSASN_CODE ant_info_ded_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(alt_codebook_enabled_for4_tx_r12, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ded_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(alt_codebook_enabled_for4_tx_r12, 1));

  return SRSASN_SUCCESS;
}
void ant_info_ded_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("alternativeCodebookEnabledFor4TX-r12", alt_codebook_enabled_for4_tx_r12);
  j.end_obj();
}
bool ant_info_ded_v1250_s::operator==(const ant_info_ded_v1250_s& other) const
{
  return alt_codebook_enabled_for4_tx_r12 == other.alt_codebook_enabled_for4_tx_r12;
}

// AntennaInfoDedicated-v1430 ::= SEQUENCE
SRSASN_CODE ant_info_ded_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ce_ue_tx_ant_sel_cfg_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ded_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ce_ue_tx_ant_sel_cfg_r14_present, 1));

  return SRSASN_SUCCESS;
}
void ant_info_ded_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ce_ue_tx_ant_sel_cfg_r14_present) {
    j.write_str("ce-UE-TxAntennaSelection-config-r14", "on");
  }
  j.end_obj();
}

// AntennaInfoDedicated-v1530 ::= CHOICE
void ant_info_ded_v1530_c::set(types::options e)
{
  type_ = e;
}
void ant_info_ded_v1530_c::set_release()
{
  set(types::release);
}
ant_info_ded_v1530_c::setup_c_& ant_info_ded_v1530_c::set_setup()
{
  set(types::setup);
  return c;
}
void ant_info_ded_v1530_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_v1530_c");
  }
  j.end_obj();
}
SRSASN_CODE ant_info_ded_v1530_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_v1530_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ded_v1530_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_v1530_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ant_info_ded_v1530_c::setup_c_::set(types::options e)
{
  type_ = e;
}
void ant_info_ded_v1530_c::setup_c_::set_ue_tx_ant_sel_srs_minus1_t4_r_cfg_r15()
{
  set(types::ue_tx_ant_sel_srs_minus1_t4_r_cfg_r15);
}
ant_info_ded_v1530_c::setup_c_::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_e_&
ant_info_ded_v1530_c::setup_c_::set_ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15()
{
  set(types::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15);
  return c;
}
void ant_info_ded_v1530_c::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_tx_ant_sel_srs_minus1_t4_r_cfg_r15:
      break;
    case types::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15:
      j.write_str("ue-TxAntennaSelection-SRS-2T4R-NrOfPairs-r15", c.to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_v1530_c::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE ant_info_ded_v1530_c::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_tx_ant_sel_srs_minus1_t4_r_cfg_r15:
      break;
    case types::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_v1530_c::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ded_v1530_c::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_tx_ant_sel_srs_minus1_t4_r_cfg_r15:
      break;
    case types::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_v1530_c::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ant_info_ded_v1530_c::setup_c_::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_opts::to_string() const
{
  static const char* options[] = {"two", "three"};
  return convert_enum_idx(
      options, 2, value, "ant_info_ded_v1530_c::setup_c_::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_e_");
}
uint8_t ant_info_ded_v1530_c::setup_c_::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 3};
  return map_enum_number(
      options, 2, value, "ant_info_ded_v1530_c::setup_c_::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_e_");
}

// AntennaInfoDedicated-v920 ::= SEQUENCE
SRSASN_CODE ant_info_ded_v920_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(codebook_subset_restrict_v920_present, 1));

  if (codebook_subset_restrict_v920_present) {
    HANDLE_CODE(codebook_subset_restrict_v920.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ded_v920_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(codebook_subset_restrict_v920_present, 1));

  if (codebook_subset_restrict_v920_present) {
    HANDLE_CODE(codebook_subset_restrict_v920.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ant_info_ded_v920_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (codebook_subset_restrict_v920_present) {
    j.write_fieldname("codebookSubsetRestriction-v920");
    codebook_subset_restrict_v920.to_json(j);
  }
  j.end_obj();
}

void ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::destroy_()
{
  switch (type_) {
    case types::n2_tx_ant_tm8_r9:
      c.destroy<fixed_bitstring<6> >();
      break;
    case types::n4_tx_ant_tm8_r9:
      c.destroy<fixed_bitstring<32> >();
      break;
    default:
      break;
  }
}
void ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::n2_tx_ant_tm8_r9:
      c.init<fixed_bitstring<6> >();
      break;
    case types::n4_tx_ant_tm8_r9:
      c.init<fixed_bitstring<32> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_v920_s::codebook_subset_restrict_v920_c_");
  }
}
ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::codebook_subset_restrict_v920_c_(
    const ant_info_ded_v920_s::codebook_subset_restrict_v920_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::n2_tx_ant_tm8_r9:
      c.init(other.c.get<fixed_bitstring<6> >());
      break;
    case types::n4_tx_ant_tm8_r9:
      c.init(other.c.get<fixed_bitstring<32> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_v920_s::codebook_subset_restrict_v920_c_");
  }
}
ant_info_ded_v920_s::codebook_subset_restrict_v920_c_& ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::operator=(
    const ant_info_ded_v920_s::codebook_subset_restrict_v920_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::n2_tx_ant_tm8_r9:
      c.set(other.c.get<fixed_bitstring<6> >());
      break;
    case types::n4_tx_ant_tm8_r9:
      c.set(other.c.get<fixed_bitstring<32> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_v920_s::codebook_subset_restrict_v920_c_");
  }

  return *this;
}
fixed_bitstring<6>& ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::set_n2_tx_ant_tm8_r9()
{
  set(types::n2_tx_ant_tm8_r9);
  return c.get<fixed_bitstring<6> >();
}
fixed_bitstring<32>& ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::set_n4_tx_ant_tm8_r9()
{
  set(types::n4_tx_ant_tm8_r9);
  return c.get<fixed_bitstring<32> >();
}
void ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::n2_tx_ant_tm8_r9:
      j.write_str("n2TxAntenna-tm8-r9", c.get<fixed_bitstring<6> >().to_string());
      break;
    case types::n4_tx_ant_tm8_r9:
      j.write_str("n4TxAntenna-tm8-r9", c.get<fixed_bitstring<32> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_v920_s::codebook_subset_restrict_v920_c_");
  }
  j.end_obj();
}
SRSASN_CODE ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::n2_tx_ant_tm8_r9:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().pack(bref));
      break;
    case types::n4_tx_ant_tm8_r9:
      HANDLE_CODE(c.get<fixed_bitstring<32> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_v920_s::codebook_subset_restrict_v920_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::n2_tx_ant_tm8_r9:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().unpack(bref));
      break;
    case types::n4_tx_ant_tm8_r9:
      HANDLE_CODE(c.get<fixed_bitstring<32> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ant_info_ded_v920_s::codebook_subset_restrict_v920_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// AntennaInfoUL-r10 ::= SEQUENCE
SRSASN_CODE ant_info_ul_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tx_mode_ul_r10_present, 1));
  HANDLE_CODE(bref.pack(four_ant_port_activ_r10_present, 1));

  if (tx_mode_ul_r10_present) {
    HANDLE_CODE(tx_mode_ul_r10.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ul_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tx_mode_ul_r10_present, 1));
  HANDLE_CODE(bref.unpack(four_ant_port_activ_r10_present, 1));

  if (tx_mode_ul_r10_present) {
    HANDLE_CODE(tx_mode_ul_r10.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ant_info_ul_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tx_mode_ul_r10_present) {
    j.write_str("transmissionModeUL-r10", tx_mode_ul_r10.to_string());
  }
  if (four_ant_port_activ_r10_present) {
    j.write_str("fourAntennaPortActivated-r10", "setup");
  }
  j.end_obj();
}
bool ant_info_ul_r10_s::operator==(const ant_info_ul_r10_s& other) const
{
  return tx_mode_ul_r10_present == other.tx_mode_ul_r10_present and
         (not tx_mode_ul_r10_present or tx_mode_ul_r10 == other.tx_mode_ul_r10) and
         four_ant_port_activ_r10_present == other.four_ant_port_activ_r10_present;
}

const char* ant_info_ul_r10_s::tx_mode_ul_r10_opts::to_string() const
{
  static const char* options[] = {"tm1", "tm2", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ant_info_ul_r10_s::tx_mode_ul_r10_e_");
}
uint8_t ant_info_ul_r10_s::tx_mode_ul_r10_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "ant_info_ul_r10_s::tx_mode_ul_r10_e_");
}

// CFI-Config-r15 ::= SEQUENCE
SRSASN_CODE cfi_cfg_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cfi_sf_non_mbsfn_r15_present, 1));
  HANDLE_CODE(bref.pack(cfi_slot_subslot_non_mbsfn_r15_present, 1));
  HANDLE_CODE(bref.pack(cfi_sf_mbsfn_r15_present, 1));
  HANDLE_CODE(bref.pack(cfi_slot_subslot_mbsfn_r15_present, 1));

  if (cfi_sf_non_mbsfn_r15_present) {
    HANDLE_CODE(pack_integer(bref, cfi_sf_non_mbsfn_r15, (uint8_t)1u, (uint8_t)4u));
  }
  if (cfi_slot_subslot_non_mbsfn_r15_present) {
    HANDLE_CODE(pack_integer(bref, cfi_slot_subslot_non_mbsfn_r15, (uint8_t)1u, (uint8_t)3u));
  }
  if (cfi_sf_mbsfn_r15_present) {
    HANDLE_CODE(pack_integer(bref, cfi_sf_mbsfn_r15, (uint8_t)1u, (uint8_t)2u));
  }
  if (cfi_slot_subslot_mbsfn_r15_present) {
    HANDLE_CODE(pack_integer(bref, cfi_slot_subslot_mbsfn_r15, (uint8_t)1u, (uint8_t)2u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfi_cfg_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cfi_sf_non_mbsfn_r15_present, 1));
  HANDLE_CODE(bref.unpack(cfi_slot_subslot_non_mbsfn_r15_present, 1));
  HANDLE_CODE(bref.unpack(cfi_sf_mbsfn_r15_present, 1));
  HANDLE_CODE(bref.unpack(cfi_slot_subslot_mbsfn_r15_present, 1));

  if (cfi_sf_non_mbsfn_r15_present) {
    HANDLE_CODE(unpack_integer(cfi_sf_non_mbsfn_r15, bref, (uint8_t)1u, (uint8_t)4u));
  }
  if (cfi_slot_subslot_non_mbsfn_r15_present) {
    HANDLE_CODE(unpack_integer(cfi_slot_subslot_non_mbsfn_r15, bref, (uint8_t)1u, (uint8_t)3u));
  }
  if (cfi_sf_mbsfn_r15_present) {
    HANDLE_CODE(unpack_integer(cfi_sf_mbsfn_r15, bref, (uint8_t)1u, (uint8_t)2u));
  }
  if (cfi_slot_subslot_mbsfn_r15_present) {
    HANDLE_CODE(unpack_integer(cfi_slot_subslot_mbsfn_r15, bref, (uint8_t)1u, (uint8_t)2u));
  }

  return SRSASN_SUCCESS;
}
void cfi_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cfi_sf_non_mbsfn_r15_present) {
    j.write_int("cfi-SubframeNonMBSFN-r15", cfi_sf_non_mbsfn_r15);
  }
  if (cfi_slot_subslot_non_mbsfn_r15_present) {
    j.write_int("cfi-SlotSubslotNonMBSFN-r15", cfi_slot_subslot_non_mbsfn_r15);
  }
  if (cfi_sf_mbsfn_r15_present) {
    j.write_int("cfi-SubframeMBSFN-r15", cfi_sf_mbsfn_r15);
  }
  if (cfi_slot_subslot_mbsfn_r15_present) {
    j.write_int("cfi-SlotSubslotMBSFN-r15", cfi_slot_subslot_mbsfn_r15);
  }
  j.end_obj();
}
bool cfi_cfg_r15_s::operator==(const cfi_cfg_r15_s& other) const
{
  return cfi_sf_non_mbsfn_r15_present == other.cfi_sf_non_mbsfn_r15_present and
         (not cfi_sf_non_mbsfn_r15_present or cfi_sf_non_mbsfn_r15 == other.cfi_sf_non_mbsfn_r15) and
         cfi_slot_subslot_non_mbsfn_r15_present == other.cfi_slot_subslot_non_mbsfn_r15_present and
         (not cfi_slot_subslot_non_mbsfn_r15_present or
          cfi_slot_subslot_non_mbsfn_r15 == other.cfi_slot_subslot_non_mbsfn_r15) and
         cfi_sf_mbsfn_r15_present == other.cfi_sf_mbsfn_r15_present and
         (not cfi_sf_mbsfn_r15_present or cfi_sf_mbsfn_r15 == other.cfi_sf_mbsfn_r15) and
         cfi_slot_subslot_mbsfn_r15_present == other.cfi_slot_subslot_mbsfn_r15_present and
         (not cfi_slot_subslot_mbsfn_r15_present or cfi_slot_subslot_mbsfn_r15 == other.cfi_slot_subslot_mbsfn_r15);
}

// CFI-PatternConfig-r15 ::= SEQUENCE
SRSASN_CODE cfi_pattern_cfg_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cfi_pattern_sf_r15_present, 1));
  HANDLE_CODE(bref.pack(cfi_pattern_slot_subslot_r15_present, 1));

  if (cfi_pattern_sf_r15_present) {
    HANDLE_CODE(
        pack_fixed_seq_of(bref, &(cfi_pattern_sf_r15)[0], cfi_pattern_sf_r15.size(), integer_packer<uint8_t>(1, 4)));
  }
  if (cfi_pattern_slot_subslot_r15_present) {
    HANDLE_CODE(pack_fixed_seq_of(
        bref, &(cfi_pattern_slot_subslot_r15)[0], cfi_pattern_slot_subslot_r15.size(), integer_packer<uint8_t>(1, 3)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfi_pattern_cfg_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cfi_pattern_sf_r15_present, 1));
  HANDLE_CODE(bref.unpack(cfi_pattern_slot_subslot_r15_present, 1));

  if (cfi_pattern_sf_r15_present) {
    HANDLE_CODE(
        unpack_fixed_seq_of(&(cfi_pattern_sf_r15)[0], bref, cfi_pattern_sf_r15.size(), integer_packer<uint8_t>(1, 4)));
  }
  if (cfi_pattern_slot_subslot_r15_present) {
    HANDLE_CODE(unpack_fixed_seq_of(
        &(cfi_pattern_slot_subslot_r15)[0], bref, cfi_pattern_slot_subslot_r15.size(), integer_packer<uint8_t>(1, 3)));
  }

  return SRSASN_SUCCESS;
}
void cfi_pattern_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cfi_pattern_sf_r15_present) {
    j.start_array("cfi-PatternSubframe-r15");
    for (const auto& e1 : cfi_pattern_sf_r15) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (cfi_pattern_slot_subslot_r15_present) {
    j.start_array("cfi-PatternSlotSubslot-r15");
    for (const auto& e1 : cfi_pattern_slot_subslot_r15) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}
bool cfi_pattern_cfg_r15_s::operator==(const cfi_pattern_cfg_r15_s& other) const
{
  return cfi_pattern_sf_r15_present == other.cfi_pattern_sf_r15_present and
         (not cfi_pattern_sf_r15_present or cfi_pattern_sf_r15 == other.cfi_pattern_sf_r15) and
         cfi_pattern_slot_subslot_r15_present == other.cfi_pattern_slot_subslot_r15_present and
         (not cfi_pattern_slot_subslot_r15_present or
          cfi_pattern_slot_subslot_r15 == other.cfi_pattern_slot_subslot_r15);
}

// CQI-ReportConfig ::= SEQUENCE
SRSASN_CODE cqi_report_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cqi_report_mode_aperiodic_present, 1));
  HANDLE_CODE(bref.pack(cqi_report_periodic_present, 1));

  if (cqi_report_mode_aperiodic_present) {
    HANDLE_CODE(cqi_report_mode_aperiodic.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, nom_pdsch_rs_epre_offset, (int8_t)-1, (int8_t)6));
  if (cqi_report_periodic_present) {
    HANDLE_CODE(cqi_report_periodic.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cqi_report_mode_aperiodic_present, 1));
  HANDLE_CODE(bref.unpack(cqi_report_periodic_present, 1));

  if (cqi_report_mode_aperiodic_present) {
    HANDLE_CODE(cqi_report_mode_aperiodic.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(nom_pdsch_rs_epre_offset, bref, (int8_t)-1, (int8_t)6));
  if (cqi_report_periodic_present) {
    HANDLE_CODE(cqi_report_periodic.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cqi_report_mode_aperiodic_present) {
    j.write_str("cqi-ReportModeAperiodic", cqi_report_mode_aperiodic.to_string());
  }
  j.write_int("nomPDSCH-RS-EPRE-Offset", nom_pdsch_rs_epre_offset);
  if (cqi_report_periodic_present) {
    j.write_fieldname("cqi-ReportPeriodic");
    cqi_report_periodic.to_json(j);
  }
  j.end_obj();
}

// CQI-ReportConfig-v1530 ::= SEQUENCE
SRSASN_CODE cqi_report_cfg_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(alt_cqi_table_minus1024_qam_r15_present, 1));

  if (alt_cqi_table_minus1024_qam_r15_present) {
    HANDLE_CODE(alt_cqi_table_minus1024_qam_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_cfg_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(alt_cqi_table_minus1024_qam_r15_present, 1));

  if (alt_cqi_table_minus1024_qam_r15_present) {
    HANDLE_CODE(alt_cqi_table_minus1024_qam_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_cfg_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (alt_cqi_table_minus1024_qam_r15_present) {
    j.write_str("altCQI-Table-1024QAM-r15", alt_cqi_table_minus1024_qam_r15.to_string());
  }
  j.end_obj();
}
bool cqi_report_cfg_v1530_s::operator==(const cqi_report_cfg_v1530_s& other) const
{
  return alt_cqi_table_minus1024_qam_r15_present == other.alt_cqi_table_minus1024_qam_r15_present and
         (not alt_cqi_table_minus1024_qam_r15_present or
          alt_cqi_table_minus1024_qam_r15 == other.alt_cqi_table_minus1024_qam_r15);
}

const char* cqi_report_cfg_v1530_s::alt_cqi_table_minus1024_qam_r15_opts::to_string() const
{
  static const char* options[] = {"allSubframes", "csi-SubframeSet1", "csi-SubframeSet2", "spare1"};
  return convert_enum_idx(options, 4, value, "cqi_report_cfg_v1530_s::alt_cqi_table_minus1024_qam_r15_e_");
}
uint8_t cqi_report_cfg_v1530_s::alt_cqi_table_minus1024_qam_r15_opts::to_number() const
{
  switch (value) {
    case csi_sf_set1:
      return 1;
    case csi_sf_set2:
      return 2;
    default:
      invalid_enum_number(value, "cqi_report_cfg_v1530_s::alt_cqi_table_minus1024_qam_r15_e_");
  }
  return 0;
}

// CQI-ReportConfig-v920 ::= SEQUENCE
SRSASN_CODE cqi_report_cfg_v920_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cqi_mask_r9_present, 1));
  HANDLE_CODE(bref.pack(pmi_ri_report_r9_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_cfg_v920_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cqi_mask_r9_present, 1));
  HANDLE_CODE(bref.unpack(pmi_ri_report_r9_present, 1));

  return SRSASN_SUCCESS;
}
void cqi_report_cfg_v920_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cqi_mask_r9_present) {
    j.write_str("cqi-Mask-r9", "setup");
  }
  if (pmi_ri_report_r9_present) {
    j.write_str("pmi-RI-Report-r9", "setup");
  }
  j.end_obj();
}

// CSI-RS-Config-v1480 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_v1480_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(emimo_type_v1480_present, 1));

  if (emimo_type_v1480_present) {
    HANDLE_CODE(emimo_type_v1480.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_v1480_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(emimo_type_v1480_present, 1));

  if (emimo_type_v1480_present) {
    HANDLE_CODE(emimo_type_v1480.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_v1480_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (emimo_type_v1480_present) {
    j.write_fieldname("eMIMO-Type-v1480");
    emimo_type_v1480.to_json(j);
  }
  j.end_obj();
}
bool csi_rs_cfg_v1480_s::operator==(const csi_rs_cfg_v1480_s& other) const
{
  return emimo_type_v1480_present == other.emimo_type_v1480_present and
         (not emimo_type_v1480_present or emimo_type_v1480 == other.emimo_type_v1480);
}

// CSI-RS-Config-v1530 ::= SEQUENCE
SRSASN_CODE csi_rs_cfg_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(emimo_type_v1530_present, 1));

  if (emimo_type_v1530_present) {
    HANDLE_CODE(emimo_type_v1530.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cfg_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(emimo_type_v1530_present, 1));

  if (emimo_type_v1530_present) {
    HANDLE_CODE(emimo_type_v1530.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csi_rs_cfg_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (emimo_type_v1530_present) {
    j.write_fieldname("eMIMO-Type-v1530");
    emimo_type_v1530.to_json(j);
  }
  j.end_obj();
}
bool csi_rs_cfg_v1530_s::operator==(const csi_rs_cfg_v1530_s& other) const
{
  return emimo_type_v1530_present == other.emimo_type_v1530_present and
         (not emimo_type_v1530_present or emimo_type_v1530 == other.emimo_type_v1530);
}

// EPDCCH-Config-r11 ::= SEQUENCE
SRSASN_CODE epdcch_cfg_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cfg_r11.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE epdcch_cfg_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cfg_r11.unpack(bref));

  return SRSASN_SUCCESS;
}
void epdcch_cfg_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("config-r11");
  cfg_r11.to_json(j);
  j.end_obj();
}
bool epdcch_cfg_r11_s::operator==(const epdcch_cfg_r11_s& other) const
{
  return cfg_r11 == other.cfg_r11;
}

void epdcch_cfg_r11_s::cfg_r11_c_::set(types::options e)
{
  type_ = e;
}
void epdcch_cfg_r11_s::cfg_r11_c_::set_release()
{
  set(types::release);
}
epdcch_cfg_r11_s::cfg_r11_c_::setup_s_& epdcch_cfg_r11_s::cfg_r11_c_::set_setup()
{
  set(types::setup);
  return c;
}
void epdcch_cfg_r11_s::cfg_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.sf_pattern_cfg_r11_present) {
        j.write_fieldname("subframePatternConfig-r11");
        c.sf_pattern_cfg_r11.to_json(j);
      }
      if (c.start_symbol_r11_present) {
        j.write_int("startSymbol-r11", c.start_symbol_r11);
      }
      if (c.set_cfg_to_release_list_r11_present) {
        j.start_array("setConfigToReleaseList-r11");
        for (const auto& e1 : c.set_cfg_to_release_list_r11) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.set_cfg_to_add_mod_list_r11_present) {
        j.start_array("setConfigToAddModList-r11");
        for (const auto& e1 : c.set_cfg_to_add_mod_list_r11) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "epdcch_cfg_r11_s::cfg_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE epdcch_cfg_r11_s::cfg_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.sf_pattern_cfg_r11_present, 1));
      HANDLE_CODE(bref.pack(c.start_symbol_r11_present, 1));
      HANDLE_CODE(bref.pack(c.set_cfg_to_release_list_r11_present, 1));
      HANDLE_CODE(bref.pack(c.set_cfg_to_add_mod_list_r11_present, 1));
      if (c.sf_pattern_cfg_r11_present) {
        HANDLE_CODE(c.sf_pattern_cfg_r11.pack(bref));
      }
      if (c.start_symbol_r11_present) {
        HANDLE_CODE(pack_integer(bref, c.start_symbol_r11, (uint8_t)1u, (uint8_t)4u));
      }
      if (c.set_cfg_to_release_list_r11_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.set_cfg_to_release_list_r11, 1, 2, integer_packer<uint8_t>(0, 1)));
      }
      if (c.set_cfg_to_add_mod_list_r11_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.set_cfg_to_add_mod_list_r11, 1, 2));
      }
      break;
    default:
      log_invalid_choice_id(type_, "epdcch_cfg_r11_s::cfg_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE epdcch_cfg_r11_s::cfg_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.sf_pattern_cfg_r11_present, 1));
      HANDLE_CODE(bref.unpack(c.start_symbol_r11_present, 1));
      HANDLE_CODE(bref.unpack(c.set_cfg_to_release_list_r11_present, 1));
      HANDLE_CODE(bref.unpack(c.set_cfg_to_add_mod_list_r11_present, 1));
      if (c.sf_pattern_cfg_r11_present) {
        HANDLE_CODE(c.sf_pattern_cfg_r11.unpack(bref));
      }
      if (c.start_symbol_r11_present) {
        HANDLE_CODE(unpack_integer(c.start_symbol_r11, bref, (uint8_t)1u, (uint8_t)4u));
      }
      if (c.set_cfg_to_release_list_r11_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.set_cfg_to_release_list_r11, bref, 1, 2, integer_packer<uint8_t>(0, 1)));
      }
      if (c.set_cfg_to_add_mod_list_r11_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.set_cfg_to_add_mod_list_r11, bref, 1, 2));
      }
      break;
    default:
      log_invalid_choice_id(type_, "epdcch_cfg_r11_s::cfg_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool epdcch_cfg_r11_s::cfg_r11_c_::operator==(const cfg_r11_c_& other) const
{
  return type() == other.type() and c.sf_pattern_cfg_r11_present == other.c.sf_pattern_cfg_r11_present and
         (not c.sf_pattern_cfg_r11_present or c.sf_pattern_cfg_r11 == other.c.sf_pattern_cfg_r11) and
         c.start_symbol_r11_present == other.c.start_symbol_r11_present and
         (not c.start_symbol_r11_present or c.start_symbol_r11 == other.c.start_symbol_r11) and
         c.set_cfg_to_release_list_r11_present == other.c.set_cfg_to_release_list_r11_present and
         (not c.set_cfg_to_release_list_r11_present or
          c.set_cfg_to_release_list_r11 == other.c.set_cfg_to_release_list_r11) and
         c.set_cfg_to_add_mod_list_r11_present == other.c.set_cfg_to_add_mod_list_r11_present and
         (not c.set_cfg_to_add_mod_list_r11_present or
          c.set_cfg_to_add_mod_list_r11 == other.c.set_cfg_to_add_mod_list_r11);
}

void epdcch_cfg_r11_s::cfg_r11_c_::setup_s_::sf_pattern_cfg_r11_c_::set(types::options e)
{
  type_ = e;
}
void epdcch_cfg_r11_s::cfg_r11_c_::setup_s_::sf_pattern_cfg_r11_c_::set_release()
{
  set(types::release);
}
epdcch_cfg_r11_s::cfg_r11_c_::setup_s_::sf_pattern_cfg_r11_c_::setup_s_&
epdcch_cfg_r11_s::cfg_r11_c_::setup_s_::sf_pattern_cfg_r11_c_::set_setup()
{
  set(types::setup);
  return c;
}
void epdcch_cfg_r11_s::cfg_r11_c_::setup_s_::sf_pattern_cfg_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("subframePattern-r11");
      c.sf_pattern_r11.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "epdcch_cfg_r11_s::cfg_r11_c_::setup_s_::sf_pattern_cfg_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE epdcch_cfg_r11_s::cfg_r11_c_::setup_s_::sf_pattern_cfg_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.sf_pattern_r11.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "epdcch_cfg_r11_s::cfg_r11_c_::setup_s_::sf_pattern_cfg_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE epdcch_cfg_r11_s::cfg_r11_c_::setup_s_::sf_pattern_cfg_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.sf_pattern_r11.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "epdcch_cfg_r11_s::cfg_r11_c_::setup_s_::sf_pattern_cfg_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool epdcch_cfg_r11_s::cfg_r11_c_::setup_s_::sf_pattern_cfg_r11_c_::operator==(const sf_pattern_cfg_r11_c_& other) const
{
  return type() == other.type() and c.sf_pattern_r11 == other.c.sf_pattern_r11;
}

// PDCCH-CandidateReductions-r13 ::= CHOICE
void pdcch_candidate_reductions_r13_c::set(types::options e)
{
  type_ = e;
}
void pdcch_candidate_reductions_r13_c::set_release()
{
  set(types::release);
}
pdcch_candidate_reductions_r13_c::setup_s_& pdcch_candidate_reductions_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void pdcch_candidate_reductions_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("pdcch-candidateReductionAL1-r13", c.pdcch_candidate_reduction_al1_r13.to_string());
      j.write_str("pdcch-candidateReductionAL2-r13", c.pdcch_candidate_reduction_al2_r13.to_string());
      j.write_str("pdcch-candidateReductionAL3-r13", c.pdcch_candidate_reduction_al3_r13.to_string());
      j.write_str("pdcch-candidateReductionAL4-r13", c.pdcch_candidate_reduction_al4_r13.to_string());
      j.write_str("pdcch-candidateReductionAL5-r13", c.pdcch_candidate_reduction_al5_r13.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_candidate_reductions_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE pdcch_candidate_reductions_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pdcch_candidate_reduction_al1_r13.pack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al2_r13.pack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al3_r13.pack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al4_r13.pack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al5_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_candidate_reductions_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_candidate_reductions_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pdcch_candidate_reduction_al1_r13.unpack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al2_r13.unpack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al3_r13.unpack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al4_r13.unpack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al5_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_candidate_reductions_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pdcch_candidate_reductions_r13_c::operator==(const pdcch_candidate_reductions_r13_c& other) const
{
  return type() == other.type() and c.pdcch_candidate_reduction_al1_r13 == other.c.pdcch_candidate_reduction_al1_r13 and
         c.pdcch_candidate_reduction_al2_r13 == other.c.pdcch_candidate_reduction_al2_r13 and
         c.pdcch_candidate_reduction_al3_r13 == other.c.pdcch_candidate_reduction_al3_r13 and
         c.pdcch_candidate_reduction_al4_r13 == other.c.pdcch_candidate_reduction_al4_r13 and
         c.pdcch_candidate_reduction_al5_r13 == other.c.pdcch_candidate_reduction_al5_r13;
}

// PDSCH-ConfigDedicated ::= SEQUENCE
SRSASN_CODE pdsch_cfg_ded_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(p_a.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_ded_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(p_a.unpack(bref));

  return SRSASN_SUCCESS;
}
void pdsch_cfg_ded_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("p-a", p_a.to_string());
  j.end_obj();
}
bool pdsch_cfg_ded_s::operator==(const pdsch_cfg_ded_s& other) const
{
  return p_a == other.p_a;
}

const char* pdsch_cfg_ded_s::p_a_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4dot77", "dB-3", "dB-1dot77", "dB0", "dB1", "dB2", "dB3"};
  return convert_enum_idx(options, 8, value, "pdsch_cfg_ded_s::p_a_e_");
}
float pdsch_cfg_ded_s::p_a_opts::to_number() const
{
  static const float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return map_enum_number(options, 8, value, "pdsch_cfg_ded_s::p_a_e_");
}
const char* pdsch_cfg_ded_s::p_a_opts::to_number_string() const
{
  static const char* options[] = {"-6", "-4.77", "-3", "-1.77", "0", "1", "2", "3"};
  return convert_enum_idx(options, 8, value, "pdsch_cfg_ded_s::p_a_e_");
}

// PDSCH-ConfigDedicated-v1130 ::= SEQUENCE
SRSASN_CODE pdsch_cfg_ded_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dmrs_cfg_pdsch_r11_present, 1));
  HANDLE_CODE(bref.pack(qcl_operation_present, 1));
  HANDLE_CODE(bref.pack(re_map_qcl_cfg_to_release_list_r11_present, 1));
  HANDLE_CODE(bref.pack(re_map_qcl_cfg_to_add_mod_list_r11_present, 1));

  if (dmrs_cfg_pdsch_r11_present) {
    HANDLE_CODE(dmrs_cfg_pdsch_r11.pack(bref));
  }
  if (qcl_operation_present) {
    HANDLE_CODE(qcl_operation.pack(bref));
  }
  if (re_map_qcl_cfg_to_release_list_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, re_map_qcl_cfg_to_release_list_r11, 1, 4, integer_packer<uint8_t>(1, 4)));
  }
  if (re_map_qcl_cfg_to_add_mod_list_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, re_map_qcl_cfg_to_add_mod_list_r11, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_ded_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dmrs_cfg_pdsch_r11_present, 1));
  HANDLE_CODE(bref.unpack(qcl_operation_present, 1));
  HANDLE_CODE(bref.unpack(re_map_qcl_cfg_to_release_list_r11_present, 1));
  HANDLE_CODE(bref.unpack(re_map_qcl_cfg_to_add_mod_list_r11_present, 1));

  if (dmrs_cfg_pdsch_r11_present) {
    HANDLE_CODE(dmrs_cfg_pdsch_r11.unpack(bref));
  }
  if (qcl_operation_present) {
    HANDLE_CODE(qcl_operation.unpack(bref));
  }
  if (re_map_qcl_cfg_to_release_list_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(re_map_qcl_cfg_to_release_list_r11, bref, 1, 4, integer_packer<uint8_t>(1, 4)));
  }
  if (re_map_qcl_cfg_to_add_mod_list_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(re_map_qcl_cfg_to_add_mod_list_r11, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void pdsch_cfg_ded_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dmrs_cfg_pdsch_r11_present) {
    j.write_fieldname("dmrs-ConfigPDSCH-r11");
    dmrs_cfg_pdsch_r11.to_json(j);
  }
  if (qcl_operation_present) {
    j.write_str("qcl-Operation", qcl_operation.to_string());
  }
  if (re_map_qcl_cfg_to_release_list_r11_present) {
    j.start_array("re-MappingQCLConfigToReleaseList-r11");
    for (const auto& e1 : re_map_qcl_cfg_to_release_list_r11) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (re_map_qcl_cfg_to_add_mod_list_r11_present) {
    j.start_array("re-MappingQCLConfigToAddModList-r11");
    for (const auto& e1 : re_map_qcl_cfg_to_add_mod_list_r11) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
bool pdsch_cfg_ded_v1130_s::operator==(const pdsch_cfg_ded_v1130_s& other) const
{
  return dmrs_cfg_pdsch_r11_present == other.dmrs_cfg_pdsch_r11_present and
         (not dmrs_cfg_pdsch_r11_present or dmrs_cfg_pdsch_r11 == other.dmrs_cfg_pdsch_r11) and
         qcl_operation_present == other.qcl_operation_present and
         (not qcl_operation_present or qcl_operation == other.qcl_operation) and
         re_map_qcl_cfg_to_release_list_r11_present == other.re_map_qcl_cfg_to_release_list_r11_present and
         (not re_map_qcl_cfg_to_release_list_r11_present or
          re_map_qcl_cfg_to_release_list_r11 == other.re_map_qcl_cfg_to_release_list_r11) and
         re_map_qcl_cfg_to_add_mod_list_r11_present == other.re_map_qcl_cfg_to_add_mod_list_r11_present and
         (not re_map_qcl_cfg_to_add_mod_list_r11_present or
          re_map_qcl_cfg_to_add_mod_list_r11 == other.re_map_qcl_cfg_to_add_mod_list_r11);
}

const char* pdsch_cfg_ded_v1130_s::qcl_operation_opts::to_string() const
{
  static const char* options[] = {"typeA", "typeB"};
  return convert_enum_idx(options, 2, value, "pdsch_cfg_ded_v1130_s::qcl_operation_e_");
}

// PDSCH-ConfigDedicated-v1280 ::= SEQUENCE
SRSASN_CODE pdsch_cfg_ded_v1280_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tbs_idx_alt_r12_present, 1));

  if (tbs_idx_alt_r12_present) {
    HANDLE_CODE(tbs_idx_alt_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_ded_v1280_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tbs_idx_alt_r12_present, 1));

  if (tbs_idx_alt_r12_present) {
    HANDLE_CODE(tbs_idx_alt_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdsch_cfg_ded_v1280_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tbs_idx_alt_r12_present) {
    j.write_str("tbsIndexAlt-r12", tbs_idx_alt_r12.to_string());
  }
  j.end_obj();
}
bool pdsch_cfg_ded_v1280_s::operator==(const pdsch_cfg_ded_v1280_s& other) const
{
  return tbs_idx_alt_r12_present == other.tbs_idx_alt_r12_present and
         (not tbs_idx_alt_r12_present or tbs_idx_alt_r12 == other.tbs_idx_alt_r12);
}

const char* pdsch_cfg_ded_v1280_s::tbs_idx_alt_r12_opts::to_string() const
{
  static const char* options[] = {"a26", "a33"};
  return convert_enum_idx(options, 2, value, "pdsch_cfg_ded_v1280_s::tbs_idx_alt_r12_e_");
}
uint8_t pdsch_cfg_ded_v1280_s::tbs_idx_alt_r12_opts::to_number() const
{
  static const uint8_t options[] = {26, 33};
  return map_enum_number(options, 2, value, "pdsch_cfg_ded_v1280_s::tbs_idx_alt_r12_e_");
}

// PDSCH-ConfigDedicated-v1310 ::= SEQUENCE
SRSASN_CODE pdsch_cfg_ded_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dmrs_cfg_pdsch_v1310_present, 1));

  if (dmrs_cfg_pdsch_v1310_present) {
    HANDLE_CODE(dmrs_cfg_pdsch_v1310.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_ded_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dmrs_cfg_pdsch_v1310_present, 1));

  if (dmrs_cfg_pdsch_v1310_present) {
    HANDLE_CODE(dmrs_cfg_pdsch_v1310.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdsch_cfg_ded_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dmrs_cfg_pdsch_v1310_present) {
    j.write_fieldname("dmrs-ConfigPDSCH-v1310");
    dmrs_cfg_pdsch_v1310.to_json(j);
  }
  j.end_obj();
}
bool pdsch_cfg_ded_v1310_s::operator==(const pdsch_cfg_ded_v1310_s& other) const
{
  return dmrs_cfg_pdsch_v1310_present == other.dmrs_cfg_pdsch_v1310_present and
         (not dmrs_cfg_pdsch_v1310_present or dmrs_cfg_pdsch_v1310 == other.dmrs_cfg_pdsch_v1310);
}

// PDSCH-ConfigDedicated-v1430 ::= SEQUENCE
SRSASN_CODE pdsch_cfg_ded_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ce_pdsch_max_bw_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_pdsch_ten_processes_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_harq_ack_bundling_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_sched_enhancement_r14_present, 1));
  HANDLE_CODE(bref.pack(tbs_idx_alt2_r14_present, 1));

  if (ce_pdsch_max_bw_r14_present) {
    HANDLE_CODE(ce_pdsch_max_bw_r14.pack(bref));
  }
  if (ce_sched_enhancement_r14_present) {
    HANDLE_CODE(ce_sched_enhancement_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_ded_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ce_pdsch_max_bw_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_pdsch_ten_processes_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_harq_ack_bundling_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_sched_enhancement_r14_present, 1));
  HANDLE_CODE(bref.unpack(tbs_idx_alt2_r14_present, 1));

  if (ce_pdsch_max_bw_r14_present) {
    HANDLE_CODE(ce_pdsch_max_bw_r14.unpack(bref));
  }
  if (ce_sched_enhancement_r14_present) {
    HANDLE_CODE(ce_sched_enhancement_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdsch_cfg_ded_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ce_pdsch_max_bw_r14_present) {
    j.write_str("ce-PDSCH-MaxBandwidth-r14", ce_pdsch_max_bw_r14.to_string());
  }
  if (ce_pdsch_ten_processes_r14_present) {
    j.write_str("ce-PDSCH-TenProcesses-r14", "on");
  }
  if (ce_harq_ack_bundling_r14_present) {
    j.write_str("ce-HARQ-AckBundling-r14", "on");
  }
  if (ce_sched_enhancement_r14_present) {
    j.write_str("ce-SchedulingEnhancement-r14", ce_sched_enhancement_r14.to_string());
  }
  if (tbs_idx_alt2_r14_present) {
    j.write_str("tbsIndexAlt2-r14", "b33");
  }
  j.end_obj();
}

const char* pdsch_cfg_ded_v1430_s::ce_pdsch_max_bw_r14_opts::to_string() const
{
  static const char* options[] = {"bw5", "bw20"};
  return convert_enum_idx(options, 2, value, "pdsch_cfg_ded_v1430_s::ce_pdsch_max_bw_r14_e_");
}
uint8_t pdsch_cfg_ded_v1430_s::ce_pdsch_max_bw_r14_opts::to_number() const
{
  static const uint8_t options[] = {5, 20};
  return map_enum_number(options, 2, value, "pdsch_cfg_ded_v1430_s::ce_pdsch_max_bw_r14_e_");
}

const char* pdsch_cfg_ded_v1430_s::ce_sched_enhancement_r14_opts::to_string() const
{
  static const char* options[] = {"range1", "range2"};
  return convert_enum_idx(options, 2, value, "pdsch_cfg_ded_v1430_s::ce_sched_enhancement_r14_e_");
}
uint8_t pdsch_cfg_ded_v1430_s::ce_sched_enhancement_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "pdsch_cfg_ded_v1430_s::ce_sched_enhancement_r14_e_");
}

// PDSCH-ConfigDedicated-v1530 ::= SEQUENCE
SRSASN_CODE pdsch_cfg_ded_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(qcl_operation_v1530_present, 1));
  HANDLE_CODE(bref.pack(tbs_idx_alt3_r15_present, 1));
  HANDLE_CODE(bref.pack(ce_cqi_alt_table_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(ce_pdsch_minus64_qam_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(ce_pdsch_flex_start_prb_alloc_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(alt_mcs_table_scaling_cfg_r15_present, 1));

  if (alt_mcs_table_scaling_cfg_r15_present) {
    HANDLE_CODE(alt_mcs_table_scaling_cfg_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_ded_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(qcl_operation_v1530_present, 1));
  HANDLE_CODE(bref.unpack(tbs_idx_alt3_r15_present, 1));
  HANDLE_CODE(bref.unpack(ce_cqi_alt_table_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(ce_pdsch_minus64_qam_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(ce_pdsch_flex_start_prb_alloc_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(alt_mcs_table_scaling_cfg_r15_present, 1));

  if (alt_mcs_table_scaling_cfg_r15_present) {
    HANDLE_CODE(alt_mcs_table_scaling_cfg_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdsch_cfg_ded_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (qcl_operation_v1530_present) {
    j.write_str("qcl-Operation-v1530", "typeC");
  }
  if (tbs_idx_alt3_r15_present) {
    j.write_str("tbs-IndexAlt3-r15", "a37");
  }
  if (ce_cqi_alt_table_cfg_r15_present) {
    j.write_str("ce-CQI-AlternativeTableConfig-r15", "on");
  }
  if (ce_pdsch_minus64_qam_cfg_r15_present) {
    j.write_str("ce-PDSCH-64QAM-Config-r15", "on");
  }
  if (ce_pdsch_flex_start_prb_alloc_cfg_r15_present) {
    j.write_str("ce-PDSCH-FlexibleStartPRB-AllocConfig-r15", "on");
  }
  if (alt_mcs_table_scaling_cfg_r15_present) {
    j.write_str("altMCS-TableScalingConfig-r15", alt_mcs_table_scaling_cfg_r15.to_string());
  }
  j.end_obj();
}
bool pdsch_cfg_ded_v1530_s::operator==(const pdsch_cfg_ded_v1530_s& other) const
{
  return qcl_operation_v1530_present == other.qcl_operation_v1530_present and
         tbs_idx_alt3_r15_present == other.tbs_idx_alt3_r15_present and
         ce_cqi_alt_table_cfg_r15_present == other.ce_cqi_alt_table_cfg_r15_present and
         ce_pdsch_minus64_qam_cfg_r15_present == other.ce_pdsch_minus64_qam_cfg_r15_present and
         ce_pdsch_flex_start_prb_alloc_cfg_r15_present == other.ce_pdsch_flex_start_prb_alloc_cfg_r15_present and
         alt_mcs_table_scaling_cfg_r15_present == other.alt_mcs_table_scaling_cfg_r15_present and
         (not alt_mcs_table_scaling_cfg_r15_present or
          alt_mcs_table_scaling_cfg_r15 == other.alt_mcs_table_scaling_cfg_r15);
}

const char* pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_opts::to_string() const
{
  static const char* options[] = {"oDot5", "oDot625", "oDot75", "oDot875"};
  return convert_enum_idx(options, 4, value, "pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_e_");
}
float pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_opts::to_number() const
{
  static const float options[] = {0.5, 0.625, 0.75, 0.875};
  return map_enum_number(options, 4, value, "pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_e_");
}
const char* pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_opts::to_number_string() const
{
  static const char* options[] = {"0.5", "0.625", "0.75", "0.875"};
  return convert_enum_idx(options, 4, value, "pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_e_");
}

// PDSCH-ConfigDedicated-v1610 ::= SEQUENCE
SRSASN_CODE pdsch_cfg_ded_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ce_pdsch_multi_tb_cfg_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_ded_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ce_pdsch_multi_tb_cfg_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void pdsch_cfg_ded_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ce-PDSCH-MultiTB-Config-r16");
  ce_pdsch_multi_tb_cfg_r16.to_json(j);
  j.end_obj();
}

// PDSCH-ConfigDedicated-v1700 ::= SEQUENCE
SRSASN_CODE pdsch_cfg_ded_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ce_pdsch_minus14_harq_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(ce_pdsch_max_tbs_r17_present, 1));

  if (ce_pdsch_minus14_harq_cfg_r17_present) {
    HANDLE_CODE(ce_pdsch_minus14_harq_cfg_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_ded_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ce_pdsch_minus14_harq_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(ce_pdsch_max_tbs_r17_present, 1));

  if (ce_pdsch_minus14_harq_cfg_r17_present) {
    HANDLE_CODE(ce_pdsch_minus14_harq_cfg_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdsch_cfg_ded_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ce_pdsch_minus14_harq_cfg_r17_present) {
    j.write_fieldname("ce-PDSCH-14HARQ-Config-r17");
    ce_pdsch_minus14_harq_cfg_r17.to_json(j);
  }
  if (ce_pdsch_max_tbs_r17_present) {
    j.write_str("ce-PDSCH-maxTBS-r17", "enabled");
  }
  j.end_obj();
}

// PUCCH-ConfigDedicated ::= SEQUENCE
SRSASN_CODE pucch_cfg_ded_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tdd_ack_nack_feedback_mode_present, 1));

  HANDLE_CODE(ack_nack_repeat.pack(bref));
  if (tdd_ack_nack_feedback_mode_present) {
    HANDLE_CODE(tdd_ack_nack_feedback_mode.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tdd_ack_nack_feedback_mode_present, 1));

  HANDLE_CODE(ack_nack_repeat.unpack(bref));
  if (tdd_ack_nack_feedback_mode_present) {
    HANDLE_CODE(tdd_ack_nack_feedback_mode.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pucch_cfg_ded_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ackNackRepetition");
  ack_nack_repeat.to_json(j);
  if (tdd_ack_nack_feedback_mode_present) {
    j.write_str("tdd-AckNackFeedbackMode", tdd_ack_nack_feedback_mode.to_string());
  }
  j.end_obj();
}

void pucch_cfg_ded_s::ack_nack_repeat_c_::set(types::options e)
{
  type_ = e;
}
void pucch_cfg_ded_s::ack_nack_repeat_c_::set_release()
{
  set(types::release);
}
pucch_cfg_ded_s::ack_nack_repeat_c_::setup_s_& pucch_cfg_ded_s::ack_nack_repeat_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pucch_cfg_ded_s::ack_nack_repeat_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("repetitionFactor", c.repeat_factor.to_string());
      j.write_int("n1PUCCH-AN-Rep", c.n1_pucch_an_rep);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_s::ack_nack_repeat_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_s::ack_nack_repeat_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.repeat_factor.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.n1_pucch_an_rep, (uint16_t)0u, (uint16_t)2047u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_s::ack_nack_repeat_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_s::ack_nack_repeat_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.repeat_factor.unpack(bref));
      HANDLE_CODE(unpack_integer(c.n1_pucch_an_rep, bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_s::ack_nack_repeat_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pucch_cfg_ded_s::ack_nack_repeat_c_::setup_s_::repeat_factor_opts::to_string() const
{
  static const char* options[] = {"n2", "n4", "n6", "spare1"};
  return convert_enum_idx(options, 4, value, "pucch_cfg_ded_s::ack_nack_repeat_c_::setup_s_::repeat_factor_e_");
}
uint8_t pucch_cfg_ded_s::ack_nack_repeat_c_::setup_s_::repeat_factor_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 6};
  return map_enum_number(options, 3, value, "pucch_cfg_ded_s::ack_nack_repeat_c_::setup_s_::repeat_factor_e_");
}

const char* pucch_cfg_ded_s::tdd_ack_nack_feedback_mode_opts::to_string() const
{
  static const char* options[] = {"bundling", "multiplexing"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_ded_s::tdd_ack_nack_feedback_mode_e_");
}

// PUCCH-ConfigDedicated-r13 ::= SEQUENCE
SRSASN_CODE pucch_cfg_ded_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tdd_ack_nack_feedback_mode_r13_present, 1));
  HANDLE_CODE(bref.pack(pucch_format_r13_present, 1));
  HANDLE_CODE(bref.pack(two_ant_port_activ_pucch_format1a1b_r13_present, 1));
  HANDLE_CODE(bref.pack(simul_pucch_pusch_r13_present, 1));
  HANDLE_CODE(bref.pack(n1_pucch_an_rep_p1_r13_present, 1));
  HANDLE_CODE(bref.pack(npucch_param_r13_present, 1));
  HANDLE_CODE(bref.pack(nka_pucch_param_r13_present, 1));
  HANDLE_CODE(bref.pack(codebooksize_determination_r13_present, 1));
  HANDLE_CODE(bref.pack(maximum_payload_coderate_r13_present, 1));
  HANDLE_CODE(bref.pack(pucch_num_repeat_ce_r13_present, 1));

  HANDLE_CODE(ack_nack_repeat_r13.pack(bref));
  if (tdd_ack_nack_feedback_mode_r13_present) {
    HANDLE_CODE(tdd_ack_nack_feedback_mode_r13.pack(bref));
  }
  if (pucch_format_r13_present) {
    HANDLE_CODE(pucch_format_r13.pack(bref));
  }
  if (n1_pucch_an_rep_p1_r13_present) {
    HANDLE_CODE(pack_integer(bref, n1_pucch_an_rep_p1_r13, (uint16_t)0u, (uint16_t)2047u));
  }
  if (npucch_param_r13_present) {
    HANDLE_CODE(npucch_param_r13.pack(bref));
  }
  if (nka_pucch_param_r13_present) {
    HANDLE_CODE(nka_pucch_param_r13.pack(bref));
  }
  HANDLE_CODE(bref.pack(spatial_bundling_pucch_r13, 1));
  HANDLE_CODE(bref.pack(spatial_bundling_pusch_r13, 1));
  HANDLE_CODE(bref.pack(harq_timing_tdd_r13, 1));
  if (codebooksize_determination_r13_present) {
    HANDLE_CODE(codebooksize_determination_r13.pack(bref));
  }
  if (maximum_payload_coderate_r13_present) {
    HANDLE_CODE(pack_integer(bref, maximum_payload_coderate_r13, (uint8_t)0u, (uint8_t)7u));
  }
  if (pucch_num_repeat_ce_r13_present) {
    HANDLE_CODE(pucch_num_repeat_ce_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tdd_ack_nack_feedback_mode_r13_present, 1));
  HANDLE_CODE(bref.unpack(pucch_format_r13_present, 1));
  HANDLE_CODE(bref.unpack(two_ant_port_activ_pucch_format1a1b_r13_present, 1));
  HANDLE_CODE(bref.unpack(simul_pucch_pusch_r13_present, 1));
  HANDLE_CODE(bref.unpack(n1_pucch_an_rep_p1_r13_present, 1));
  HANDLE_CODE(bref.unpack(npucch_param_r13_present, 1));
  HANDLE_CODE(bref.unpack(nka_pucch_param_r13_present, 1));
  HANDLE_CODE(bref.unpack(codebooksize_determination_r13_present, 1));
  HANDLE_CODE(bref.unpack(maximum_payload_coderate_r13_present, 1));
  HANDLE_CODE(bref.unpack(pucch_num_repeat_ce_r13_present, 1));

  HANDLE_CODE(ack_nack_repeat_r13.unpack(bref));
  if (tdd_ack_nack_feedback_mode_r13_present) {
    HANDLE_CODE(tdd_ack_nack_feedback_mode_r13.unpack(bref));
  }
  if (pucch_format_r13_present) {
    HANDLE_CODE(pucch_format_r13.unpack(bref));
  }
  if (n1_pucch_an_rep_p1_r13_present) {
    HANDLE_CODE(unpack_integer(n1_pucch_an_rep_p1_r13, bref, (uint16_t)0u, (uint16_t)2047u));
  }
  if (npucch_param_r13_present) {
    HANDLE_CODE(npucch_param_r13.unpack(bref));
  }
  if (nka_pucch_param_r13_present) {
    HANDLE_CODE(nka_pucch_param_r13.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(spatial_bundling_pucch_r13, 1));
  HANDLE_CODE(bref.unpack(spatial_bundling_pusch_r13, 1));
  HANDLE_CODE(bref.unpack(harq_timing_tdd_r13, 1));
  if (codebooksize_determination_r13_present) {
    HANDLE_CODE(codebooksize_determination_r13.unpack(bref));
  }
  if (maximum_payload_coderate_r13_present) {
    HANDLE_CODE(unpack_integer(maximum_payload_coderate_r13, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (pucch_num_repeat_ce_r13_present) {
    HANDLE_CODE(pucch_num_repeat_ce_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pucch_cfg_ded_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ackNackRepetition-r13");
  ack_nack_repeat_r13.to_json(j);
  if (tdd_ack_nack_feedback_mode_r13_present) {
    j.write_str("tdd-AckNackFeedbackMode-r13", tdd_ack_nack_feedback_mode_r13.to_string());
  }
  if (pucch_format_r13_present) {
    j.write_fieldname("pucch-Format-r13");
    pucch_format_r13.to_json(j);
  }
  if (two_ant_port_activ_pucch_format1a1b_r13_present) {
    j.write_str("twoAntennaPortActivatedPUCCH-Format1a1b-r13", "true");
  }
  if (simul_pucch_pusch_r13_present) {
    j.write_str("simultaneousPUCCH-PUSCH-r13", "true");
  }
  if (n1_pucch_an_rep_p1_r13_present) {
    j.write_int("n1PUCCH-AN-RepP1-r13", n1_pucch_an_rep_p1_r13);
  }
  if (npucch_param_r13_present) {
    j.write_fieldname("nPUCCH-Param-r13");
    npucch_param_r13.to_json(j);
  }
  if (nka_pucch_param_r13_present) {
    j.write_fieldname("nkaPUCCH-Param-r13");
    nka_pucch_param_r13.to_json(j);
  }
  j.write_bool("spatialBundlingPUCCH-r13", spatial_bundling_pucch_r13);
  j.write_bool("spatialBundlingPUSCH-r13", spatial_bundling_pusch_r13);
  j.write_bool("harq-TimingTDD-r13", harq_timing_tdd_r13);
  if (codebooksize_determination_r13_present) {
    j.write_str("codebooksizeDetermination-r13", codebooksize_determination_r13.to_string());
  }
  if (maximum_payload_coderate_r13_present) {
    j.write_int("maximumPayloadCoderate-r13", maximum_payload_coderate_r13);
  }
  if (pucch_num_repeat_ce_r13_present) {
    j.write_fieldname("pucch-NumRepetitionCE-r13");
    pucch_num_repeat_ce_r13.to_json(j);
  }
  j.end_obj();
}
bool pucch_cfg_ded_r13_s::operator==(const pucch_cfg_ded_r13_s& other) const
{
  return ack_nack_repeat_r13 == other.ack_nack_repeat_r13 and
         tdd_ack_nack_feedback_mode_r13_present == other.tdd_ack_nack_feedback_mode_r13_present and
         (not tdd_ack_nack_feedback_mode_r13_present or
          tdd_ack_nack_feedback_mode_r13 == other.tdd_ack_nack_feedback_mode_r13) and
         pucch_format_r13_present == other.pucch_format_r13_present and
         (not pucch_format_r13_present or pucch_format_r13 == other.pucch_format_r13) and
         two_ant_port_activ_pucch_format1a1b_r13_present == other.two_ant_port_activ_pucch_format1a1b_r13_present and
         simul_pucch_pusch_r13_present == other.simul_pucch_pusch_r13_present and
         n1_pucch_an_rep_p1_r13_present == other.n1_pucch_an_rep_p1_r13_present and
         (not n1_pucch_an_rep_p1_r13_present or n1_pucch_an_rep_p1_r13 == other.n1_pucch_an_rep_p1_r13) and
         npucch_param_r13_present == other.npucch_param_r13_present and
         (not npucch_param_r13_present or npucch_param_r13 == other.npucch_param_r13) and
         nka_pucch_param_r13_present == other.nka_pucch_param_r13_present and
         (not nka_pucch_param_r13_present or nka_pucch_param_r13 == other.nka_pucch_param_r13) and
         spatial_bundling_pucch_r13 == other.spatial_bundling_pucch_r13 and
         spatial_bundling_pusch_r13 == other.spatial_bundling_pusch_r13 and
         harq_timing_tdd_r13 == other.harq_timing_tdd_r13 and
         codebooksize_determination_r13_present == other.codebooksize_determination_r13_present and
         (not codebooksize_determination_r13_present or
          codebooksize_determination_r13 == other.codebooksize_determination_r13) and
         maximum_payload_coderate_r13_present == other.maximum_payload_coderate_r13_present and
         (not maximum_payload_coderate_r13_present or
          maximum_payload_coderate_r13 == other.maximum_payload_coderate_r13) and
         pucch_num_repeat_ce_r13_present == other.pucch_num_repeat_ce_r13_present and
         (not pucch_num_repeat_ce_r13_present or pucch_num_repeat_ce_r13 == other.pucch_num_repeat_ce_r13);
}

void pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::set(types::options e)
{
  type_ = e;
}
void pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::set_release()
{
  set(types::release);
}
pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::setup_s_& pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("repetitionFactor-r13", c.repeat_factor_r13.to_string());
      j.write_int("n1PUCCH-AN-Rep-r13", c.n1_pucch_an_rep_r13);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.repeat_factor_r13.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.n1_pucch_an_rep_r13, (uint16_t)0u, (uint16_t)2047u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.repeat_factor_r13.unpack(bref));
      HANDLE_CODE(unpack_integer(c.n1_pucch_an_rep_r13, bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::operator==(const ack_nack_repeat_r13_c_& other) const
{
  return type() == other.type() and c.repeat_factor_r13 == other.c.repeat_factor_r13 and
         c.n1_pucch_an_rep_r13 == other.c.n1_pucch_an_rep_r13;
}

const char* pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::setup_s_::repeat_factor_r13_opts::to_string() const
{
  static const char* options[] = {"n2", "n4", "n6", "spare1"};
  return convert_enum_idx(
      options, 4, value, "pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::setup_s_::repeat_factor_r13_e_");
}
uint8_t pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::setup_s_::repeat_factor_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 6};
  return map_enum_number(
      options, 3, value, "pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::setup_s_::repeat_factor_r13_e_");
}

const char* pucch_cfg_ded_r13_s::tdd_ack_nack_feedback_mode_r13_opts::to_string() const
{
  static const char* options[] = {"bundling", "multiplexing"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_ded_r13_s::tdd_ack_nack_feedback_mode_r13_e_");
}

void pucch_cfg_ded_r13_s::pucch_format_r13_c_::destroy_()
{
  switch (type_) {
    case types::format3_r13:
      c.destroy<format3_r13_s_>();
      break;
    case types::ch_sel_r13:
      c.destroy<ch_sel_r13_s_>();
      break;
    case types::format4_r13:
      c.destroy<format4_r13_s_>();
      break;
    case types::format5_r13:
      c.destroy<format5_r13_s_>();
      break;
    default:
      break;
  }
}
void pucch_cfg_ded_r13_s::pucch_format_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::format3_r13:
      c.init<format3_r13_s_>();
      break;
    case types::ch_sel_r13:
      c.init<ch_sel_r13_s_>();
      break;
    case types::format4_r13:
      c.init<format4_r13_s_>();
      break;
    case types::format5_r13:
      c.init<format5_r13_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_format_r13_c_");
  }
}
pucch_cfg_ded_r13_s::pucch_format_r13_c_::pucch_format_r13_c_(const pucch_cfg_ded_r13_s::pucch_format_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::format3_r13:
      c.init(other.c.get<format3_r13_s_>());
      break;
    case types::ch_sel_r13:
      c.init(other.c.get<ch_sel_r13_s_>());
      break;
    case types::format4_r13:
      c.init(other.c.get<format4_r13_s_>());
      break;
    case types::format5_r13:
      c.init(other.c.get<format5_r13_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_format_r13_c_");
  }
}
pucch_cfg_ded_r13_s::pucch_format_r13_c_&
pucch_cfg_ded_r13_s::pucch_format_r13_c_::operator=(const pucch_cfg_ded_r13_s::pucch_format_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::format3_r13:
      c.set(other.c.get<format3_r13_s_>());
      break;
    case types::ch_sel_r13:
      c.set(other.c.get<ch_sel_r13_s_>());
      break;
    case types::format4_r13:
      c.set(other.c.get<format4_r13_s_>());
      break;
    case types::format5_r13:
      c.set(other.c.get<format5_r13_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_format_r13_c_");
  }

  return *this;
}
pucch_cfg_ded_r13_s::pucch_format_r13_c_::format3_r13_s_& pucch_cfg_ded_r13_s::pucch_format_r13_c_::set_format3_r13()
{
  set(types::format3_r13);
  return c.get<format3_r13_s_>();
}
pucch_cfg_ded_r13_s::pucch_format_r13_c_::ch_sel_r13_s_& pucch_cfg_ded_r13_s::pucch_format_r13_c_::set_ch_sel_r13()
{
  set(types::ch_sel_r13);
  return c.get<ch_sel_r13_s_>();
}
pucch_cfg_ded_r13_s::pucch_format_r13_c_::format4_r13_s_& pucch_cfg_ded_r13_s::pucch_format_r13_c_::set_format4_r13()
{
  set(types::format4_r13);
  return c.get<format4_r13_s_>();
}
pucch_cfg_ded_r13_s::pucch_format_r13_c_::format5_r13_s_& pucch_cfg_ded_r13_s::pucch_format_r13_c_::set_format5_r13()
{
  set(types::format5_r13);
  return c.get<format5_r13_s_>();
}
void pucch_cfg_ded_r13_s::pucch_format_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::format3_r13:
      j.write_fieldname("format3-r13");
      j.start_obj();
      if (c.get<format3_r13_s_>().n3_pucch_an_list_r13_present) {
        j.start_array("n3PUCCH-AN-List-r13");
        for (const auto& e1 : c.get<format3_r13_s_>().n3_pucch_an_list_r13) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.get<format3_r13_s_>().two_ant_port_activ_pucch_format3_r13_present) {
        j.write_fieldname("twoAntennaPortActivatedPUCCH-Format3-r13");
        c.get<format3_r13_s_>().two_ant_port_activ_pucch_format3_r13.to_json(j);
      }
      j.end_obj();
      break;
    case types::ch_sel_r13:
      j.write_fieldname("channelSelection-r13");
      j.start_obj();
      if (c.get<ch_sel_r13_s_>().n1_pucch_an_cs_r13_present) {
        j.write_fieldname("n1PUCCH-AN-CS-r13");
        c.get<ch_sel_r13_s_>().n1_pucch_an_cs_r13.to_json(j);
      }
      j.end_obj();
      break;
    case types::format4_r13:
      j.write_fieldname("format4-r13");
      j.start_obj();
      j.start_array("format4-resourceConfiguration-r13");
      for (const auto& e1 : c.get<format4_r13_s_>().format4_res_cfg_r13) {
        e1.to_json(j);
      }
      j.end_array();
      if (c.get<format4_r13_s_>().format4_multi_csi_res_cfg_r13_present) {
        j.start_array("format4-MultiCSI-resourceConfiguration-r13");
        for (const auto& e1 : c.get<format4_r13_s_>().format4_multi_csi_res_cfg_r13) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    case types::format5_r13:
      j.write_fieldname("format5-r13");
      j.start_obj();
      j.start_array("format5-resourceConfiguration-r13");
      for (const auto& e1 : c.get<format5_r13_s_>().format5_res_cfg_r13) {
        e1.to_json(j);
      }
      j.end_array();
      if (c.get<format5_r13_s_>().format5_multi_csi_res_cfg_r13_present) {
        j.write_fieldname("format5-MultiCSI-resourceConfiguration-r13");
        c.get<format5_r13_s_>().format5_multi_csi_res_cfg_r13.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_format_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_r13_s::pucch_format_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::format3_r13:
      HANDLE_CODE(bref.pack(c.get<format3_r13_s_>().n3_pucch_an_list_r13_present, 1));
      HANDLE_CODE(bref.pack(c.get<format3_r13_s_>().two_ant_port_activ_pucch_format3_r13_present, 1));
      if (c.get<format3_r13_s_>().n3_pucch_an_list_r13_present) {
        HANDLE_CODE(pack_dyn_seq_of(
            bref, c.get<format3_r13_s_>().n3_pucch_an_list_r13, 1, 4, integer_packer<uint16_t>(0, 549)));
      }
      if (c.get<format3_r13_s_>().two_ant_port_activ_pucch_format3_r13_present) {
        HANDLE_CODE(c.get<format3_r13_s_>().two_ant_port_activ_pucch_format3_r13.pack(bref));
      }
      break;
    case types::ch_sel_r13:
      HANDLE_CODE(bref.pack(c.get<ch_sel_r13_s_>().n1_pucch_an_cs_r13_present, 1));
      if (c.get<ch_sel_r13_s_>().n1_pucch_an_cs_r13_present) {
        HANDLE_CODE(c.get<ch_sel_r13_s_>().n1_pucch_an_cs_r13.pack(bref));
      }
      break;
    case types::format4_r13:
      HANDLE_CODE(bref.pack(c.get<format4_r13_s_>().format4_multi_csi_res_cfg_r13_present, 1));
      HANDLE_CODE(pack_fixed_seq_of(
          bref, &(c.get<format4_r13_s_>().format4_res_cfg_r13)[0], c.get<format4_r13_s_>().format4_res_cfg_r13.size()));
      if (c.get<format4_r13_s_>().format4_multi_csi_res_cfg_r13_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.get<format4_r13_s_>().format4_multi_csi_res_cfg_r13, 1, 2));
      }
      break;
    case types::format5_r13:
      HANDLE_CODE(bref.pack(c.get<format5_r13_s_>().format5_multi_csi_res_cfg_r13_present, 1));
      HANDLE_CODE(pack_fixed_seq_of(
          bref, &(c.get<format5_r13_s_>().format5_res_cfg_r13)[0], c.get<format5_r13_s_>().format5_res_cfg_r13.size()));
      if (c.get<format5_r13_s_>().format5_multi_csi_res_cfg_r13_present) {
        HANDLE_CODE(c.get<format5_r13_s_>().format5_multi_csi_res_cfg_r13.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_format_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_r13_s::pucch_format_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::format3_r13:
      HANDLE_CODE(bref.unpack(c.get<format3_r13_s_>().n3_pucch_an_list_r13_present, 1));
      HANDLE_CODE(bref.unpack(c.get<format3_r13_s_>().two_ant_port_activ_pucch_format3_r13_present, 1));
      if (c.get<format3_r13_s_>().n3_pucch_an_list_r13_present) {
        HANDLE_CODE(unpack_dyn_seq_of(
            c.get<format3_r13_s_>().n3_pucch_an_list_r13, bref, 1, 4, integer_packer<uint16_t>(0, 549)));
      }
      if (c.get<format3_r13_s_>().two_ant_port_activ_pucch_format3_r13_present) {
        HANDLE_CODE(c.get<format3_r13_s_>().two_ant_port_activ_pucch_format3_r13.unpack(bref));
      }
      break;
    case types::ch_sel_r13:
      HANDLE_CODE(bref.unpack(c.get<ch_sel_r13_s_>().n1_pucch_an_cs_r13_present, 1));
      if (c.get<ch_sel_r13_s_>().n1_pucch_an_cs_r13_present) {
        HANDLE_CODE(c.get<ch_sel_r13_s_>().n1_pucch_an_cs_r13.unpack(bref));
      }
      break;
    case types::format4_r13:
      HANDLE_CODE(bref.unpack(c.get<format4_r13_s_>().format4_multi_csi_res_cfg_r13_present, 1));
      HANDLE_CODE(unpack_fixed_seq_of(
          &(c.get<format4_r13_s_>().format4_res_cfg_r13)[0], bref, c.get<format4_r13_s_>().format4_res_cfg_r13.size()));
      if (c.get<format4_r13_s_>().format4_multi_csi_res_cfg_r13_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.get<format4_r13_s_>().format4_multi_csi_res_cfg_r13, bref, 1, 2));
      }
      break;
    case types::format5_r13:
      HANDLE_CODE(bref.unpack(c.get<format5_r13_s_>().format5_multi_csi_res_cfg_r13_present, 1));
      HANDLE_CODE(unpack_fixed_seq_of(
          &(c.get<format5_r13_s_>().format5_res_cfg_r13)[0], bref, c.get<format5_r13_s_>().format5_res_cfg_r13.size()));
      if (c.get<format5_r13_s_>().format5_multi_csi_res_cfg_r13_present) {
        HANDLE_CODE(c.get<format5_r13_s_>().format5_multi_csi_res_cfg_r13.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_format_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pucch_cfg_ded_r13_s::pucch_format_r13_c_::operator==(const pucch_format_r13_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::format3_r13:
      return c.get<format3_r13_s_>().n3_pucch_an_list_r13_present ==
                 other.c.get<format3_r13_s_>().n3_pucch_an_list_r13_present and
             (not c.get<format3_r13_s_>().n3_pucch_an_list_r13_present or
              c.get<format3_r13_s_>().n3_pucch_an_list_r13 == other.c.get<format3_r13_s_>().n3_pucch_an_list_r13) and
             c.get<format3_r13_s_>().two_ant_port_activ_pucch_format3_r13_present ==
                 other.c.get<format3_r13_s_>().two_ant_port_activ_pucch_format3_r13_present and
             (not c.get<format3_r13_s_>().two_ant_port_activ_pucch_format3_r13_present or
              c.get<format3_r13_s_>().two_ant_port_activ_pucch_format3_r13 ==
                  other.c.get<format3_r13_s_>().two_ant_port_activ_pucch_format3_r13);
    case types::ch_sel_r13:
      return c.get<ch_sel_r13_s_>().n1_pucch_an_cs_r13_present ==
                 other.c.get<ch_sel_r13_s_>().n1_pucch_an_cs_r13_present and
             (not c.get<ch_sel_r13_s_>().n1_pucch_an_cs_r13_present or
              c.get<ch_sel_r13_s_>().n1_pucch_an_cs_r13 == other.c.get<ch_sel_r13_s_>().n1_pucch_an_cs_r13);
    case types::format4_r13:
      return c.get<format4_r13_s_>().format4_res_cfg_r13 == other.c.get<format4_r13_s_>().format4_res_cfg_r13 and
             c.get<format4_r13_s_>().format4_multi_csi_res_cfg_r13_present ==
                 other.c.get<format4_r13_s_>().format4_multi_csi_res_cfg_r13_present and
             (not c.get<format4_r13_s_>().format4_multi_csi_res_cfg_r13_present or
              c.get<format4_r13_s_>().format4_multi_csi_res_cfg_r13 ==
                  other.c.get<format4_r13_s_>().format4_multi_csi_res_cfg_r13);
    case types::format5_r13:
      return c.get<format5_r13_s_>().format5_res_cfg_r13 == other.c.get<format5_r13_s_>().format5_res_cfg_r13 and
             c.get<format5_r13_s_>().format5_multi_csi_res_cfg_r13_present ==
                 other.c.get<format5_r13_s_>().format5_multi_csi_res_cfg_r13_present and
             (not c.get<format5_r13_s_>().format5_multi_csi_res_cfg_r13_present or
              c.get<format5_r13_s_>().format5_multi_csi_res_cfg_r13 ==
                  other.c.get<format5_r13_s_>().format5_multi_csi_res_cfg_r13);
    default:
      return true;
  }
  return true;
}

void pucch_cfg_ded_r13_s::pucch_format_r13_c_::format3_r13_s_::two_ant_port_activ_pucch_format3_r13_c_::set(
    types::options e)
{
  type_ = e;
}
void pucch_cfg_ded_r13_s::pucch_format_r13_c_::format3_r13_s_::two_ant_port_activ_pucch_format3_r13_c_::set_release()
{
  set(types::release);
}
pucch_cfg_ded_r13_s::pucch_format_r13_c_::format3_r13_s_::two_ant_port_activ_pucch_format3_r13_c_::setup_s_&
pucch_cfg_ded_r13_s::pucch_format_r13_c_::format3_r13_s_::two_ant_port_activ_pucch_format3_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pucch_cfg_ded_r13_s::pucch_format_r13_c_::format3_r13_s_::two_ant_port_activ_pucch_format3_r13_c_::to_json(
    json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("n3PUCCH-AN-ListP1-r13");
      for (const auto& e1 : c.n3_pucch_an_list_p1_r13) {
        j.write_int(e1);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(
          type_, "pucch_cfg_ded_r13_s::pucch_format_r13_c_::format3_r13_s_::two_ant_port_activ_pucch_format3_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_r13_s::pucch_format_r13_c_::format3_r13_s_::two_ant_port_activ_pucch_format3_r13_c_::pack(
    bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.n3_pucch_an_list_p1_r13, 1, 4, integer_packer<uint16_t>(0, 549)));
      break;
    default:
      log_invalid_choice_id(
          type_, "pucch_cfg_ded_r13_s::pucch_format_r13_c_::format3_r13_s_::two_ant_port_activ_pucch_format3_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_r13_s::pucch_format_r13_c_::format3_r13_s_::two_ant_port_activ_pucch_format3_r13_c_::unpack(
    cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.n3_pucch_an_list_p1_r13, bref, 1, 4, integer_packer<uint16_t>(0, 549)));
      break;
    default:
      log_invalid_choice_id(
          type_, "pucch_cfg_ded_r13_s::pucch_format_r13_c_::format3_r13_s_::two_ant_port_activ_pucch_format3_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pucch_cfg_ded_r13_s::pucch_format_r13_c_::format3_r13_s_::two_ant_port_activ_pucch_format3_r13_c_::operator==(
    const two_ant_port_activ_pucch_format3_r13_c_& other) const
{
  return type() == other.type() and c.n3_pucch_an_list_p1_r13 == other.c.n3_pucch_an_list_p1_r13;
}

void pucch_cfg_ded_r13_s::pucch_format_r13_c_::ch_sel_r13_s_::n1_pucch_an_cs_r13_c_::set(types::options e)
{
  type_ = e;
}
void pucch_cfg_ded_r13_s::pucch_format_r13_c_::ch_sel_r13_s_::n1_pucch_an_cs_r13_c_::set_release()
{
  set(types::release);
}
pucch_cfg_ded_r13_s::pucch_format_r13_c_::ch_sel_r13_s_::n1_pucch_an_cs_r13_c_::setup_s_&
pucch_cfg_ded_r13_s::pucch_format_r13_c_::ch_sel_r13_s_::n1_pucch_an_cs_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pucch_cfg_ded_r13_s::pucch_format_r13_c_::ch_sel_r13_s_::n1_pucch_an_cs_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("n1PUCCH-AN-CS-List-r13");
      for (const auto& e1 : c.n1_pucch_an_cs_list_r13) {
        j.start_array();
        for (const auto& e2 : e1) {
          j.write_int(e2);
        }
        j.end_array();
      }
      j.end_array();
      j.start_array("dummy1");
      for (const auto& e1 : c.dummy1) {
        j.write_int(e1);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_format_r13_c_::ch_sel_r13_s_::n1_pucch_an_cs_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_r13_s::pucch_format_r13_c_::ch_sel_r13_s_::n1_pucch_an_cs_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  c.n1_pucch_an_cs_list_r13,
                                  1,
                                  2,
                                  SeqOfPacker<integer_packer<uint16_t> >(1, 4, integer_packer<uint16_t>(0, 2047))));
      HANDLE_CODE(pack_dyn_seq_of(bref, c.dummy1, 2, 4, integer_packer<uint16_t>(0, 2047)));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_format_r13_c_::ch_sel_r13_s_::n1_pucch_an_cs_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_r13_s::pucch_format_r13_c_::ch_sel_r13_s_::n1_pucch_an_cs_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.n1_pucch_an_cs_list_r13,
                                    bref,
                                    1,
                                    2,
                                    SeqOfPacker<integer_packer<uint16_t> >(1, 4, integer_packer<uint16_t>(0, 2047))));
      HANDLE_CODE(unpack_dyn_seq_of(c.dummy1, bref, 2, 4, integer_packer<uint16_t>(0, 2047)));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_format_r13_c_::ch_sel_r13_s_::n1_pucch_an_cs_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pucch_cfg_ded_r13_s::pucch_format_r13_c_::ch_sel_r13_s_::n1_pucch_an_cs_r13_c_::operator==(
    const n1_pucch_an_cs_r13_c_& other) const
{
  return type() == other.type() and c.n1_pucch_an_cs_list_r13 == other.c.n1_pucch_an_cs_list_r13 and
         c.dummy1 == other.c.dummy1;
}

void pucch_cfg_ded_r13_s::npucch_param_r13_c_::set(types::options e)
{
  type_ = e;
}
void pucch_cfg_ded_r13_s::npucch_param_r13_c_::set_release()
{
  set(types::release);
}
pucch_cfg_ded_r13_s::npucch_param_r13_c_::setup_s_& pucch_cfg_ded_r13_s::npucch_param_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pucch_cfg_ded_r13_s::npucch_param_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("nPUCCH-Identity-r13", c.npucch_id_r13);
      j.write_int("n1PUCCH-AN-r13", c.n1_pucch_an_r13);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::npucch_param_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_r13_s::npucch_param_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c.npucch_id_r13, (uint16_t)0u, (uint16_t)503u));
      HANDLE_CODE(pack_integer(bref, c.n1_pucch_an_r13, (uint16_t)0u, (uint16_t)2047u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::npucch_param_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_r13_s::npucch_param_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c.npucch_id_r13, bref, (uint16_t)0u, (uint16_t)503u));
      HANDLE_CODE(unpack_integer(c.n1_pucch_an_r13, bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::npucch_param_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pucch_cfg_ded_r13_s::npucch_param_r13_c_::operator==(const npucch_param_r13_c_& other) const
{
  return type() == other.type() and c.npucch_id_r13 == other.c.npucch_id_r13 and
         c.n1_pucch_an_r13 == other.c.n1_pucch_an_r13;
}

void pucch_cfg_ded_r13_s::nka_pucch_param_r13_c_::set(types::options e)
{
  type_ = e;
}
void pucch_cfg_ded_r13_s::nka_pucch_param_r13_c_::set_release()
{
  set(types::release);
}
pucch_cfg_ded_r13_s::nka_pucch_param_r13_c_::setup_s_& pucch_cfg_ded_r13_s::nka_pucch_param_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pucch_cfg_ded_r13_s::nka_pucch_param_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("nkaPUCCH-AN-r13", c.nka_pucch_an_r13);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::nka_pucch_param_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_r13_s::nka_pucch_param_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c.nka_pucch_an_r13, (uint16_t)0u, (uint16_t)2047u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::nka_pucch_param_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_r13_s::nka_pucch_param_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c.nka_pucch_an_r13, bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::nka_pucch_param_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pucch_cfg_ded_r13_s::nka_pucch_param_r13_c_::operator==(const nka_pucch_param_r13_c_& other) const
{
  return type() == other.type() and c.nka_pucch_an_r13 == other.c.nka_pucch_an_r13;
}

const char* pucch_cfg_ded_r13_s::codebooksize_determination_r13_opts::to_string() const
{
  static const char* options[] = {"dai", "cc"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_ded_r13_s::codebooksize_determination_r13_e_");
}

void pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::set(types::options e)
{
  type_ = e;
}
void pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::set_release()
{
  set(types::release);
}
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_& pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::operator==(const pucch_num_repeat_ce_r13_c_& other) const
{
  return type() == other.type() and c == other.c;
}

void pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::destroy_()
{
  switch (type_) {
    case types::mode_a:
      c.destroy<mode_a_s_>();
      break;
    case types::mode_b:
      c.destroy<mode_b_s_>();
      break;
    default:
      break;
  }
}
void pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::mode_a:
      c.init<mode_a_s_>();
      break;
    case types::mode_b:
      c.init<mode_b_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_");
  }
}
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::setup_c_(
    const pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::mode_a:
      c.init(other.c.get<mode_a_s_>());
      break;
    case types::mode_b:
      c.init(other.c.get<mode_b_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_");
  }
}
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_&
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::operator=(
    const pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::mode_a:
      c.set(other.c.get<mode_a_s_>());
      break;
    case types::mode_b:
      c.set(other.c.get<mode_b_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_");
  }

  return *this;
}
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_&
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::set_mode_a()
{
  set(types::mode_a);
  return c.get<mode_a_s_>();
}
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_&
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::set_mode_b()
{
  set(types::mode_b);
  return c.get<mode_b_s_>();
}
void pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::mode_a:
      j.write_fieldname("modeA");
      j.start_obj();
      j.write_str("pucch-NumRepetitionCE-format1-r13", c.get<mode_a_s_>().pucch_num_repeat_ce_format1_r13.to_string());
      j.write_str("pucch-NumRepetitionCE-format2-r13", c.get<mode_a_s_>().pucch_num_repeat_ce_format2_r13.to_string());
      j.end_obj();
      break;
    case types::mode_b:
      j.write_fieldname("modeB");
      j.start_obj();
      j.write_str("pucch-NumRepetitionCE-format1-r13", c.get<mode_b_s_>().pucch_num_repeat_ce_format1_r13.to_string());
      j.write_str("pucch-NumRepetitionCE-format2-r13", c.get<mode_b_s_>().pucch_num_repeat_ce_format2_r13.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::mode_a:
      HANDLE_CODE(c.get<mode_a_s_>().pucch_num_repeat_ce_format1_r13.pack(bref));
      HANDLE_CODE(c.get<mode_a_s_>().pucch_num_repeat_ce_format2_r13.pack(bref));
      break;
    case types::mode_b:
      HANDLE_CODE(c.get<mode_b_s_>().pucch_num_repeat_ce_format1_r13.pack(bref));
      HANDLE_CODE(c.get<mode_b_s_>().pucch_num_repeat_ce_format2_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::mode_a:
      HANDLE_CODE(c.get<mode_a_s_>().pucch_num_repeat_ce_format1_r13.unpack(bref));
      HANDLE_CODE(c.get<mode_a_s_>().pucch_num_repeat_ce_format2_r13.unpack(bref));
      break;
    case types::mode_b:
      HANDLE_CODE(c.get<mode_b_s_>().pucch_num_repeat_ce_format1_r13.unpack(bref));
      HANDLE_CODE(c.get<mode_b_s_>().pucch_num_repeat_ce_format2_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::operator==(const setup_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::mode_a:
      return c.get<mode_a_s_>().pucch_num_repeat_ce_format1_r13 ==
                 other.c.get<mode_a_s_>().pucch_num_repeat_ce_format1_r13 and
             c.get<mode_a_s_>().pucch_num_repeat_ce_format2_r13 ==
                 other.c.get<mode_a_s_>().pucch_num_repeat_ce_format2_r13;
    case types::mode_b:
      return c.get<mode_b_s_>().pucch_num_repeat_ce_format1_r13 ==
                 other.c.get<mode_b_s_>().pucch_num_repeat_ce_format1_r13 and
             c.get<mode_b_s_>().pucch_num_repeat_ce_format2_r13 ==
                 other.c.get<mode_b_s_>().pucch_num_repeat_ce_format2_r13;
    default:
      return true;
  }
  return true;
}

const char*
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format1_r13_opts::to_string()
    const
{
  static const char* options[] = {"r1", "r2", "r4", "r8"};
  return convert_enum_idx(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format1_r13_e_");
}
uint8_t
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format1_r13_opts::to_number()
    const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format1_r13_e_");
}

const char*
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format2_r13_opts::to_string()
    const
{
  static const char* options[] = {"r1", "r2", "r4", "r8"};
  return convert_enum_idx(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format2_r13_e_");
}
uint8_t
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format2_r13_opts::to_number()
    const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format2_r13_e_");
}

const char*
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format1_r13_opts::to_string()
    const
{
  static const char* options[] = {"r4", "r8", "r16", "r32"};
  return convert_enum_idx(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format1_r13_e_");
}
uint8_t
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format1_r13_opts::to_number()
    const
{
  static const uint8_t options[] = {4, 8, 16, 32};
  return map_enum_number(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format1_r13_e_");
}

const char*
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format2_r13_opts::to_string()
    const
{
  static const char* options[] = {"r4", "r8", "r16", "r32"};
  return convert_enum_idx(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format2_r13_e_");
}
uint8_t
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format2_r13_opts::to_number()
    const
{
  static const uint8_t options[] = {4, 8, 16, 32};
  return map_enum_number(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format2_r13_e_");
}

// PUCCH-ConfigDedicated-v1020 ::= SEQUENCE
SRSASN_CODE pucch_cfg_ded_v1020_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pucch_format_r10_present, 1));
  HANDLE_CODE(bref.pack(two_ant_port_activ_pucch_format1a1b_r10_present, 1));
  HANDLE_CODE(bref.pack(simul_pucch_pusch_r10_present, 1));
  HANDLE_CODE(bref.pack(n1_pucch_an_rep_p1_r10_present, 1));

  if (pucch_format_r10_present) {
    HANDLE_CODE(pucch_format_r10.pack(bref));
  }
  if (n1_pucch_an_rep_p1_r10_present) {
    HANDLE_CODE(pack_integer(bref, n1_pucch_an_rep_p1_r10, (uint16_t)0u, (uint16_t)2047u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_v1020_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pucch_format_r10_present, 1));
  HANDLE_CODE(bref.unpack(two_ant_port_activ_pucch_format1a1b_r10_present, 1));
  HANDLE_CODE(bref.unpack(simul_pucch_pusch_r10_present, 1));
  HANDLE_CODE(bref.unpack(n1_pucch_an_rep_p1_r10_present, 1));

  if (pucch_format_r10_present) {
    HANDLE_CODE(pucch_format_r10.unpack(bref));
  }
  if (n1_pucch_an_rep_p1_r10_present) {
    HANDLE_CODE(unpack_integer(n1_pucch_an_rep_p1_r10, bref, (uint16_t)0u, (uint16_t)2047u));
  }

  return SRSASN_SUCCESS;
}
void pucch_cfg_ded_v1020_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pucch_format_r10_present) {
    j.write_fieldname("pucch-Format-r10");
    pucch_format_r10.to_json(j);
  }
  if (two_ant_port_activ_pucch_format1a1b_r10_present) {
    j.write_str("twoAntennaPortActivatedPUCCH-Format1a1b-r10", "true");
  }
  if (simul_pucch_pusch_r10_present) {
    j.write_str("simultaneousPUCCH-PUSCH-r10", "true");
  }
  if (n1_pucch_an_rep_p1_r10_present) {
    j.write_int("n1PUCCH-AN-RepP1-r10", n1_pucch_an_rep_p1_r10);
  }
  j.end_obj();
}

void pucch_cfg_ded_v1020_s::pucch_format_r10_c_::destroy_()
{
  switch (type_) {
    case types::format3_r10:
      c.destroy<pucch_format3_conf_r13_s>();
      break;
    case types::ch_sel_r10:
      c.destroy<ch_sel_r10_s_>();
      break;
    default:
      break;
  }
}
void pucch_cfg_ded_v1020_s::pucch_format_r10_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::format3_r10:
      c.init<pucch_format3_conf_r13_s>();
      break;
    case types::ch_sel_r10:
      c.init<ch_sel_r10_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_");
  }
}
pucch_cfg_ded_v1020_s::pucch_format_r10_c_::pucch_format_r10_c_(const pucch_cfg_ded_v1020_s::pucch_format_r10_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::format3_r10:
      c.init(other.c.get<pucch_format3_conf_r13_s>());
      break;
    case types::ch_sel_r10:
      c.init(other.c.get<ch_sel_r10_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_");
  }
}
pucch_cfg_ded_v1020_s::pucch_format_r10_c_&
pucch_cfg_ded_v1020_s::pucch_format_r10_c_::operator=(const pucch_cfg_ded_v1020_s::pucch_format_r10_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::format3_r10:
      c.set(other.c.get<pucch_format3_conf_r13_s>());
      break;
    case types::ch_sel_r10:
      c.set(other.c.get<ch_sel_r10_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_");
  }

  return *this;
}
pucch_format3_conf_r13_s& pucch_cfg_ded_v1020_s::pucch_format_r10_c_::set_format3_r10()
{
  set(types::format3_r10);
  return c.get<pucch_format3_conf_r13_s>();
}
pucch_cfg_ded_v1020_s::pucch_format_r10_c_::ch_sel_r10_s_& pucch_cfg_ded_v1020_s::pucch_format_r10_c_::set_ch_sel_r10()
{
  set(types::ch_sel_r10);
  return c.get<ch_sel_r10_s_>();
}
void pucch_cfg_ded_v1020_s::pucch_format_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::format3_r10:
      j.write_fieldname("format3-r10");
      c.get<pucch_format3_conf_r13_s>().to_json(j);
      break;
    case types::ch_sel_r10:
      j.write_fieldname("channelSelection-r10");
      j.start_obj();
      if (c.get<ch_sel_r10_s_>().n1_pucch_an_cs_r10_present) {
        j.write_fieldname("n1PUCCH-AN-CS-r10");
        c.get<ch_sel_r10_s_>().n1_pucch_an_cs_r10.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_v1020_s::pucch_format_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::format3_r10:
      HANDLE_CODE(c.get<pucch_format3_conf_r13_s>().pack(bref));
      break;
    case types::ch_sel_r10:
      HANDLE_CODE(bref.pack(c.get<ch_sel_r10_s_>().n1_pucch_an_cs_r10_present, 1));
      if (c.get<ch_sel_r10_s_>().n1_pucch_an_cs_r10_present) {
        HANDLE_CODE(c.get<ch_sel_r10_s_>().n1_pucch_an_cs_r10.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_v1020_s::pucch_format_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::format3_r10:
      HANDLE_CODE(c.get<pucch_format3_conf_r13_s>().unpack(bref));
      break;
    case types::ch_sel_r10:
      HANDLE_CODE(bref.unpack(c.get<ch_sel_r10_s_>().n1_pucch_an_cs_r10_present, 1));
      if (c.get<ch_sel_r10_s_>().n1_pucch_an_cs_r10_present) {
        HANDLE_CODE(c.get<ch_sel_r10_s_>().n1_pucch_an_cs_r10.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void pucch_cfg_ded_v1020_s::pucch_format_r10_c_::ch_sel_r10_s_::n1_pucch_an_cs_r10_c_::set(types::options e)
{
  type_ = e;
}
void pucch_cfg_ded_v1020_s::pucch_format_r10_c_::ch_sel_r10_s_::n1_pucch_an_cs_r10_c_::set_release()
{
  set(types::release);
}
pucch_cfg_ded_v1020_s::pucch_format_r10_c_::ch_sel_r10_s_::n1_pucch_an_cs_r10_c_::setup_s_&
pucch_cfg_ded_v1020_s::pucch_format_r10_c_::ch_sel_r10_s_::n1_pucch_an_cs_r10_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pucch_cfg_ded_v1020_s::pucch_format_r10_c_::ch_sel_r10_s_::n1_pucch_an_cs_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("n1PUCCH-AN-CS-List-r10");
      for (const auto& e1 : c.n1_pucch_an_cs_list_r10) {
        j.start_array();
        for (const auto& e2 : e1) {
          j.write_int(e2);
        }
        j.end_array();
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_::ch_sel_r10_s_::n1_pucch_an_cs_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_v1020_s::pucch_format_r10_c_::ch_sel_r10_s_::n1_pucch_an_cs_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  c.n1_pucch_an_cs_list_r10,
                                  1,
                                  2,
                                  SeqOfPacker<integer_packer<uint16_t> >(1, 4, integer_packer<uint16_t>(0, 2047))));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_::ch_sel_r10_s_::n1_pucch_an_cs_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_v1020_s::pucch_format_r10_c_::ch_sel_r10_s_::n1_pucch_an_cs_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.n1_pucch_an_cs_list_r10,
                                    bref,
                                    1,
                                    2,
                                    SeqOfPacker<integer_packer<uint16_t> >(1, 4, integer_packer<uint16_t>(0, 2047))));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_::ch_sel_r10_s_::n1_pucch_an_cs_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// PUCCH-ConfigDedicated-v1130 ::= SEQUENCE
SRSASN_CODE pucch_cfg_ded_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(n1_pucch_an_cs_v1130_present, 1));
  HANDLE_CODE(bref.pack(npucch_param_r11_present, 1));

  if (n1_pucch_an_cs_v1130_present) {
    HANDLE_CODE(n1_pucch_an_cs_v1130.pack(bref));
  }
  if (npucch_param_r11_present) {
    HANDLE_CODE(npucch_param_r11.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(n1_pucch_an_cs_v1130_present, 1));
  HANDLE_CODE(bref.unpack(npucch_param_r11_present, 1));

  if (n1_pucch_an_cs_v1130_present) {
    HANDLE_CODE(n1_pucch_an_cs_v1130.unpack(bref));
  }
  if (npucch_param_r11_present) {
    HANDLE_CODE(npucch_param_r11.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pucch_cfg_ded_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (n1_pucch_an_cs_v1130_present) {
    j.write_fieldname("n1PUCCH-AN-CS-v1130");
    n1_pucch_an_cs_v1130.to_json(j);
  }
  if (npucch_param_r11_present) {
    j.write_fieldname("nPUCCH-Param-r11");
    npucch_param_r11.to_json(j);
  }
  j.end_obj();
}

void pucch_cfg_ded_v1130_s::n1_pucch_an_cs_v1130_c_::set(types::options e)
{
  type_ = e;
}
void pucch_cfg_ded_v1130_s::n1_pucch_an_cs_v1130_c_::set_release()
{
  set(types::release);
}
pucch_cfg_ded_v1130_s::n1_pucch_an_cs_v1130_c_::setup_s_& pucch_cfg_ded_v1130_s::n1_pucch_an_cs_v1130_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pucch_cfg_ded_v1130_s::n1_pucch_an_cs_v1130_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("n1PUCCH-AN-CS-ListP1-r11");
      for (const auto& e1 : c.n1_pucch_an_cs_list_p1_r11) {
        j.write_int(e1);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1130_s::n1_pucch_an_cs_v1130_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_v1130_s::n1_pucch_an_cs_v1130_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.n1_pucch_an_cs_list_p1_r11, 2, 4, integer_packer<uint16_t>(0, 2047)));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1130_s::n1_pucch_an_cs_v1130_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_v1130_s::n1_pucch_an_cs_v1130_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.n1_pucch_an_cs_list_p1_r11, bref, 2, 4, integer_packer<uint16_t>(0, 2047)));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1130_s::n1_pucch_an_cs_v1130_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void pucch_cfg_ded_v1130_s::npucch_param_r11_c_::set(types::options e)
{
  type_ = e;
}
void pucch_cfg_ded_v1130_s::npucch_param_r11_c_::set_release()
{
  set(types::release);
}
pucch_cfg_ded_v1130_s::npucch_param_r11_c_::setup_s_& pucch_cfg_ded_v1130_s::npucch_param_r11_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pucch_cfg_ded_v1130_s::npucch_param_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("nPUCCH-Identity-r11", c.npucch_id_r11);
      j.write_int("n1PUCCH-AN-r11", c.n1_pucch_an_r11);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1130_s::npucch_param_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_v1130_s::npucch_param_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c.npucch_id_r11, (uint16_t)0u, (uint16_t)503u));
      HANDLE_CODE(pack_integer(bref, c.n1_pucch_an_r11, (uint16_t)0u, (uint16_t)2047u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1130_s::npucch_param_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_v1130_s::npucch_param_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c.npucch_id_r11, bref, (uint16_t)0u, (uint16_t)503u));
      HANDLE_CODE(unpack_integer(c.n1_pucch_an_r11, bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1130_s::npucch_param_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// PUCCH-ConfigDedicated-v1250 ::= SEQUENCE
SRSASN_CODE pucch_cfg_ded_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(nka_pucch_param_r12.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(nka_pucch_param_r12.unpack(bref));

  return SRSASN_SUCCESS;
}
void pucch_cfg_ded_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("nkaPUCCH-Param-r12");
  nka_pucch_param_r12.to_json(j);
  j.end_obj();
}

void pucch_cfg_ded_v1250_s::nka_pucch_param_r12_c_::set(types::options e)
{
  type_ = e;
}
void pucch_cfg_ded_v1250_s::nka_pucch_param_r12_c_::set_release()
{
  set(types::release);
}
pucch_cfg_ded_v1250_s::nka_pucch_param_r12_c_::setup_s_& pucch_cfg_ded_v1250_s::nka_pucch_param_r12_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pucch_cfg_ded_v1250_s::nka_pucch_param_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("nkaPUCCH-AN-r12", c.nka_pucch_an_r12);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1250_s::nka_pucch_param_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_v1250_s::nka_pucch_param_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c.nka_pucch_an_r12, (uint16_t)0u, (uint16_t)2047u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1250_s::nka_pucch_param_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_v1250_s::nka_pucch_param_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c.nka_pucch_an_r12, bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1250_s::nka_pucch_param_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// PUCCH-ConfigDedicated-v1430 ::= SEQUENCE
SRSASN_CODE pucch_cfg_ded_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pucch_num_repeat_ce_format1_r14_present, 1));

  if (pucch_num_repeat_ce_format1_r14_present) {
    HANDLE_CODE(pucch_num_repeat_ce_format1_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pucch_num_repeat_ce_format1_r14_present, 1));

  if (pucch_num_repeat_ce_format1_r14_present) {
    HANDLE_CODE(pucch_num_repeat_ce_format1_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pucch_cfg_ded_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pucch_num_repeat_ce_format1_r14_present) {
    j.write_str("pucch-NumRepetitionCE-format1-r14", pucch_num_repeat_ce_format1_r14.to_string());
  }
  j.end_obj();
}

const char* pucch_cfg_ded_v1430_s::pucch_num_repeat_ce_format1_r14_opts::to_string() const
{
  static const char* options[] = {"r64", "r128"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_ded_v1430_s::pucch_num_repeat_ce_format1_r14_e_");
}
uint8_t pucch_cfg_ded_v1430_s::pucch_num_repeat_ce_format1_r14_opts::to_number() const
{
  static const uint8_t options[] = {64, 128};
  return map_enum_number(options, 2, value, "pucch_cfg_ded_v1430_s::pucch_num_repeat_ce_format1_r14_e_");
}

// PUSCH-ConfigDedicated ::= SEQUENCE
SRSASN_CODE pusch_cfg_ded_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, beta_offset_ack_idx, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, beta_offset_ri_idx, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, beta_offset_cqi_idx, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(beta_offset_ack_idx, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(beta_offset_ri_idx, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(beta_offset_cqi_idx, bref, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
void pusch_cfg_ded_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("betaOffset-ACK-Index", beta_offset_ack_idx);
  j.write_int("betaOffset-RI-Index", beta_offset_ri_idx);
  j.write_int("betaOffset-CQI-Index", beta_offset_cqi_idx);
  j.end_obj();
}

// PUSCH-ConfigDedicated-r13 ::= SEQUENCE
SRSASN_CODE pusch_cfg_ded_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(beta_offset2_ack_idx_r13_present, 1));
  HANDLE_CODE(bref.pack(beta_offset_mc_r13_present, 1));
  HANDLE_CODE(bref.pack(group_hop_disabled_r13_present, 1));
  HANDLE_CODE(bref.pack(dmrs_with_occ_activ_r13_present, 1));
  HANDLE_CODE(bref.pack(pusch_dmrs_r11_present, 1));
  HANDLE_CODE(bref.pack(uci_on_pusch_present, 1));
  HANDLE_CODE(bref.pack(pusch_hop_cfg_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, beta_offset_ack_idx_r13, (uint8_t)0u, (uint8_t)15u));
  if (beta_offset2_ack_idx_r13_present) {
    HANDLE_CODE(pack_integer(bref, beta_offset2_ack_idx_r13, (uint8_t)0u, (uint8_t)15u));
  }
  HANDLE_CODE(pack_integer(bref, beta_offset_ri_idx_r13, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, beta_offset_cqi_idx_r13, (uint8_t)0u, (uint8_t)15u));
  if (beta_offset_mc_r13_present) {
    HANDLE_CODE(bref.pack(beta_offset_mc_r13.beta_offset2_ack_idx_mc_r13_present, 1));
    HANDLE_CODE(pack_integer(bref, beta_offset_mc_r13.beta_offset_ack_idx_mc_r13, (uint8_t)0u, (uint8_t)15u));
    if (beta_offset_mc_r13.beta_offset2_ack_idx_mc_r13_present) {
      HANDLE_CODE(pack_integer(bref, beta_offset_mc_r13.beta_offset2_ack_idx_mc_r13, (uint8_t)0u, (uint8_t)15u));
    }
    HANDLE_CODE(pack_integer(bref, beta_offset_mc_r13.beta_offset_ri_idx_mc_r13, (uint8_t)0u, (uint8_t)15u));
    HANDLE_CODE(pack_integer(bref, beta_offset_mc_r13.beta_offset_cqi_idx_mc_r13, (uint8_t)0u, (uint8_t)15u));
  }
  if (pusch_dmrs_r11_present) {
    HANDLE_CODE(pusch_dmrs_r11.pack(bref));
  }
  if (uci_on_pusch_present) {
    HANDLE_CODE(uci_on_pusch.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(beta_offset2_ack_idx_r13_present, 1));
  HANDLE_CODE(bref.unpack(beta_offset_mc_r13_present, 1));
  HANDLE_CODE(bref.unpack(group_hop_disabled_r13_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_with_occ_activ_r13_present, 1));
  HANDLE_CODE(bref.unpack(pusch_dmrs_r11_present, 1));
  HANDLE_CODE(bref.unpack(uci_on_pusch_present, 1));
  HANDLE_CODE(bref.unpack(pusch_hop_cfg_r13_present, 1));

  HANDLE_CODE(unpack_integer(beta_offset_ack_idx_r13, bref, (uint8_t)0u, (uint8_t)15u));
  if (beta_offset2_ack_idx_r13_present) {
    HANDLE_CODE(unpack_integer(beta_offset2_ack_idx_r13, bref, (uint8_t)0u, (uint8_t)15u));
  }
  HANDLE_CODE(unpack_integer(beta_offset_ri_idx_r13, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(beta_offset_cqi_idx_r13, bref, (uint8_t)0u, (uint8_t)15u));
  if (beta_offset_mc_r13_present) {
    HANDLE_CODE(bref.unpack(beta_offset_mc_r13.beta_offset2_ack_idx_mc_r13_present, 1));
    HANDLE_CODE(unpack_integer(beta_offset_mc_r13.beta_offset_ack_idx_mc_r13, bref, (uint8_t)0u, (uint8_t)15u));
    if (beta_offset_mc_r13.beta_offset2_ack_idx_mc_r13_present) {
      HANDLE_CODE(unpack_integer(beta_offset_mc_r13.beta_offset2_ack_idx_mc_r13, bref, (uint8_t)0u, (uint8_t)15u));
    }
    HANDLE_CODE(unpack_integer(beta_offset_mc_r13.beta_offset_ri_idx_mc_r13, bref, (uint8_t)0u, (uint8_t)15u));
    HANDLE_CODE(unpack_integer(beta_offset_mc_r13.beta_offset_cqi_idx_mc_r13, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (pusch_dmrs_r11_present) {
    HANDLE_CODE(pusch_dmrs_r11.unpack(bref));
  }
  if (uci_on_pusch_present) {
    HANDLE_CODE(uci_on_pusch.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pusch_cfg_ded_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("betaOffset-ACK-Index-r13", beta_offset_ack_idx_r13);
  if (beta_offset2_ack_idx_r13_present) {
    j.write_int("betaOffset2-ACK-Index-r13", beta_offset2_ack_idx_r13);
  }
  j.write_int("betaOffset-RI-Index-r13", beta_offset_ri_idx_r13);
  j.write_int("betaOffset-CQI-Index-r13", beta_offset_cqi_idx_r13);
  if (beta_offset_mc_r13_present) {
    j.write_fieldname("betaOffsetMC-r13");
    j.start_obj();
    j.write_int("betaOffset-ACK-Index-MC-r13", beta_offset_mc_r13.beta_offset_ack_idx_mc_r13);
    if (beta_offset_mc_r13.beta_offset2_ack_idx_mc_r13_present) {
      j.write_int("betaOffset2-ACK-Index-MC-r13", beta_offset_mc_r13.beta_offset2_ack_idx_mc_r13);
    }
    j.write_int("betaOffset-RI-Index-MC-r13", beta_offset_mc_r13.beta_offset_ri_idx_mc_r13);
    j.write_int("betaOffset-CQI-Index-MC-r13", beta_offset_mc_r13.beta_offset_cqi_idx_mc_r13);
    j.end_obj();
  }
  if (group_hop_disabled_r13_present) {
    j.write_str("groupHoppingDisabled-r13", "true");
  }
  if (dmrs_with_occ_activ_r13_present) {
    j.write_str("dmrs-WithOCC-Activated-r13", "true");
  }
  if (pusch_dmrs_r11_present) {
    j.write_fieldname("pusch-DMRS-r11");
    pusch_dmrs_r11.to_json(j);
  }
  if (uci_on_pusch_present) {
    j.write_fieldname("uciOnPUSCH");
    uci_on_pusch.to_json(j);
  }
  if (pusch_hop_cfg_r13_present) {
    j.write_str("pusch-HoppingConfig-r13", "on");
  }
  j.end_obj();
}
bool pusch_cfg_ded_r13_s::operator==(const pusch_cfg_ded_r13_s& other) const
{
  return beta_offset_ack_idx_r13 == other.beta_offset_ack_idx_r13 and
         beta_offset2_ack_idx_r13_present == other.beta_offset2_ack_idx_r13_present and
         (not beta_offset2_ack_idx_r13_present or beta_offset2_ack_idx_r13 == other.beta_offset2_ack_idx_r13) and
         beta_offset_ri_idx_r13 == other.beta_offset_ri_idx_r13 and
         beta_offset_cqi_idx_r13 == other.beta_offset_cqi_idx_r13 and
         beta_offset_mc_r13.beta_offset_ack_idx_mc_r13 == other.beta_offset_mc_r13.beta_offset_ack_idx_mc_r13 and
         beta_offset_mc_r13.beta_offset2_ack_idx_mc_r13_present ==
             other.beta_offset_mc_r13.beta_offset2_ack_idx_mc_r13_present and
         (not beta_offset_mc_r13.beta_offset2_ack_idx_mc_r13_present or
          beta_offset_mc_r13.beta_offset2_ack_idx_mc_r13 == other.beta_offset_mc_r13.beta_offset2_ack_idx_mc_r13) and
         beta_offset_mc_r13.beta_offset_ri_idx_mc_r13 == other.beta_offset_mc_r13.beta_offset_ri_idx_mc_r13 and
         beta_offset_mc_r13.beta_offset_cqi_idx_mc_r13 == other.beta_offset_mc_r13.beta_offset_cqi_idx_mc_r13 and
         group_hop_disabled_r13_present == other.group_hop_disabled_r13_present and
         dmrs_with_occ_activ_r13_present == other.dmrs_with_occ_activ_r13_present and
         pusch_dmrs_r11_present == other.pusch_dmrs_r11_present and
         (not pusch_dmrs_r11_present or pusch_dmrs_r11 == other.pusch_dmrs_r11) and
         uci_on_pusch_present == other.uci_on_pusch_present and
         (not uci_on_pusch_present or uci_on_pusch == other.uci_on_pusch) and
         pusch_hop_cfg_r13_present == other.pusch_hop_cfg_r13_present;
}

void pusch_cfg_ded_r13_s::pusch_dmrs_r11_c_::set(types::options e)
{
  type_ = e;
}
void pusch_cfg_ded_r13_s::pusch_dmrs_r11_c_::set_release()
{
  set(types::release);
}
pusch_cfg_ded_r13_s::pusch_dmrs_r11_c_::setup_s_& pusch_cfg_ded_r13_s::pusch_dmrs_r11_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pusch_cfg_ded_r13_s::pusch_dmrs_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("nPUSCH-Identity-r13", c.npusch_id_r13);
      j.write_int("nDMRS-CSH-Identity-r13", c.ndmrs_csh_id_r13);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_r13_s::pusch_dmrs_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE pusch_cfg_ded_r13_s::pusch_dmrs_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c.npusch_id_r13, (uint16_t)0u, (uint16_t)509u));
      HANDLE_CODE(pack_integer(bref, c.ndmrs_csh_id_r13, (uint16_t)0u, (uint16_t)509u));
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_r13_s::pusch_dmrs_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_r13_s::pusch_dmrs_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c.npusch_id_r13, bref, (uint16_t)0u, (uint16_t)509u));
      HANDLE_CODE(unpack_integer(c.ndmrs_csh_id_r13, bref, (uint16_t)0u, (uint16_t)509u));
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_r13_s::pusch_dmrs_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pusch_cfg_ded_r13_s::pusch_dmrs_r11_c_::operator==(const pusch_dmrs_r11_c_& other) const
{
  return type() == other.type() and c.npusch_id_r13 == other.c.npusch_id_r13 and
         c.ndmrs_csh_id_r13 == other.c.ndmrs_csh_id_r13;
}

void pusch_cfg_ded_r13_s::uci_on_pusch_c_::set(types::options e)
{
  type_ = e;
}
void pusch_cfg_ded_r13_s::uci_on_pusch_c_::set_release()
{
  set(types::release);
}
pusch_cfg_ded_r13_s::uci_on_pusch_c_::setup_s_& pusch_cfg_ded_r13_s::uci_on_pusch_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pusch_cfg_ded_r13_s::uci_on_pusch_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("betaOffset-ACK-Index-SubframeSet2-r13", c.beta_offset_ack_idx_sf_set2_r13);
      if (c.beta_offset2_ack_idx_sf_set2_r13_present) {
        j.write_int("betaOffset2-ACK-Index-SubframeSet2-r13", c.beta_offset2_ack_idx_sf_set2_r13);
      }
      j.write_int("betaOffset-RI-Index-SubframeSet2-r13", c.beta_offset_ri_idx_sf_set2_r13);
      j.write_int("betaOffset-CQI-Index-SubframeSet2-r13", c.beta_offset_cqi_idx_sf_set2_r13);
      if (c.beta_offset_mc_r12_present) {
        j.write_fieldname("betaOffsetMC-r12");
        j.start_obj();
        j.write_int("betaOffset-ACK-Index-MC-SubframeSet2-r13",
                    c.beta_offset_mc_r12.beta_offset_ack_idx_mc_sf_set2_r13);
        if (c.beta_offset_mc_r12.beta_offset2_ack_idx_mc_sf_set2_r13_present) {
          j.write_int("betaOffset2-ACK-Index-MC-SubframeSet2-r13",
                      c.beta_offset_mc_r12.beta_offset2_ack_idx_mc_sf_set2_r13);
        }
        j.write_int("betaOffset-RI-Index-MC-SubframeSet2-r13", c.beta_offset_mc_r12.beta_offset_ri_idx_mc_sf_set2_r13);
        j.write_int("betaOffset-CQI-Index-MC-SubframeSet2-r13",
                    c.beta_offset_mc_r12.beta_offset_cqi_idx_mc_sf_set2_r13);
        j.end_obj();
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_r13_s::uci_on_pusch_c_");
  }
  j.end_obj();
}
SRSASN_CODE pusch_cfg_ded_r13_s::uci_on_pusch_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.beta_offset2_ack_idx_sf_set2_r13_present, 1));
      HANDLE_CODE(bref.pack(c.beta_offset_mc_r12_present, 1));
      HANDLE_CODE(pack_integer(bref, c.beta_offset_ack_idx_sf_set2_r13, (uint8_t)0u, (uint8_t)15u));
      if (c.beta_offset2_ack_idx_sf_set2_r13_present) {
        HANDLE_CODE(pack_integer(bref, c.beta_offset2_ack_idx_sf_set2_r13, (uint8_t)0u, (uint8_t)15u));
      }
      HANDLE_CODE(pack_integer(bref, c.beta_offset_ri_idx_sf_set2_r13, (uint8_t)0u, (uint8_t)15u));
      HANDLE_CODE(pack_integer(bref, c.beta_offset_cqi_idx_sf_set2_r13, (uint8_t)0u, (uint8_t)15u));
      if (c.beta_offset_mc_r12_present) {
        HANDLE_CODE(bref.pack(c.beta_offset_mc_r12.beta_offset2_ack_idx_mc_sf_set2_r13_present, 1));
        HANDLE_CODE(
            pack_integer(bref, c.beta_offset_mc_r12.beta_offset_ack_idx_mc_sf_set2_r13, (uint8_t)0u, (uint8_t)15u));
        if (c.beta_offset_mc_r12.beta_offset2_ack_idx_mc_sf_set2_r13_present) {
          HANDLE_CODE(
              pack_integer(bref, c.beta_offset_mc_r12.beta_offset2_ack_idx_mc_sf_set2_r13, (uint8_t)0u, (uint8_t)15u));
        }
        HANDLE_CODE(
            pack_integer(bref, c.beta_offset_mc_r12.beta_offset_ri_idx_mc_sf_set2_r13, (uint8_t)0u, (uint8_t)15u));
        HANDLE_CODE(
            pack_integer(bref, c.beta_offset_mc_r12.beta_offset_cqi_idx_mc_sf_set2_r13, (uint8_t)0u, (uint8_t)15u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_r13_s::uci_on_pusch_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_r13_s::uci_on_pusch_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.beta_offset2_ack_idx_sf_set2_r13_present, 1));
      HANDLE_CODE(bref.unpack(c.beta_offset_mc_r12_present, 1));
      HANDLE_CODE(unpack_integer(c.beta_offset_ack_idx_sf_set2_r13, bref, (uint8_t)0u, (uint8_t)15u));
      if (c.beta_offset2_ack_idx_sf_set2_r13_present) {
        HANDLE_CODE(unpack_integer(c.beta_offset2_ack_idx_sf_set2_r13, bref, (uint8_t)0u, (uint8_t)15u));
      }
      HANDLE_CODE(unpack_integer(c.beta_offset_ri_idx_sf_set2_r13, bref, (uint8_t)0u, (uint8_t)15u));
      HANDLE_CODE(unpack_integer(c.beta_offset_cqi_idx_sf_set2_r13, bref, (uint8_t)0u, (uint8_t)15u));
      if (c.beta_offset_mc_r12_present) {
        HANDLE_CODE(bref.unpack(c.beta_offset_mc_r12.beta_offset2_ack_idx_mc_sf_set2_r13_present, 1));
        HANDLE_CODE(
            unpack_integer(c.beta_offset_mc_r12.beta_offset_ack_idx_mc_sf_set2_r13, bref, (uint8_t)0u, (uint8_t)15u));
        if (c.beta_offset_mc_r12.beta_offset2_ack_idx_mc_sf_set2_r13_present) {
          HANDLE_CODE(unpack_integer(
              c.beta_offset_mc_r12.beta_offset2_ack_idx_mc_sf_set2_r13, bref, (uint8_t)0u, (uint8_t)15u));
        }
        HANDLE_CODE(
            unpack_integer(c.beta_offset_mc_r12.beta_offset_ri_idx_mc_sf_set2_r13, bref, (uint8_t)0u, (uint8_t)15u));
        HANDLE_CODE(
            unpack_integer(c.beta_offset_mc_r12.beta_offset_cqi_idx_mc_sf_set2_r13, bref, (uint8_t)0u, (uint8_t)15u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_r13_s::uci_on_pusch_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pusch_cfg_ded_r13_s::uci_on_pusch_c_::operator==(const uci_on_pusch_c_& other) const
{
  return type() == other.type() and c.beta_offset_ack_idx_sf_set2_r13 == other.c.beta_offset_ack_idx_sf_set2_r13 and
         c.beta_offset2_ack_idx_sf_set2_r13_present == other.c.beta_offset2_ack_idx_sf_set2_r13_present and
         (not c.beta_offset2_ack_idx_sf_set2_r13_present or
          c.beta_offset2_ack_idx_sf_set2_r13 == other.c.beta_offset2_ack_idx_sf_set2_r13) and
         c.beta_offset_ri_idx_sf_set2_r13 == other.c.beta_offset_ri_idx_sf_set2_r13 and
         c.beta_offset_cqi_idx_sf_set2_r13 == other.c.beta_offset_cqi_idx_sf_set2_r13 and
         c.beta_offset_mc_r12.beta_offset_ack_idx_mc_sf_set2_r13 ==
             other.c.beta_offset_mc_r12.beta_offset_ack_idx_mc_sf_set2_r13 and
         c.beta_offset_mc_r12.beta_offset2_ack_idx_mc_sf_set2_r13_present ==
             other.c.beta_offset_mc_r12.beta_offset2_ack_idx_mc_sf_set2_r13_present and
         (not c.beta_offset_mc_r12.beta_offset2_ack_idx_mc_sf_set2_r13_present or
          c.beta_offset_mc_r12.beta_offset2_ack_idx_mc_sf_set2_r13 ==
              other.c.beta_offset_mc_r12.beta_offset2_ack_idx_mc_sf_set2_r13) and
         c.beta_offset_mc_r12.beta_offset_ri_idx_mc_sf_set2_r13 ==
             other.c.beta_offset_mc_r12.beta_offset_ri_idx_mc_sf_set2_r13 and
         c.beta_offset_mc_r12.beta_offset_cqi_idx_mc_sf_set2_r13 ==
             other.c.beta_offset_mc_r12.beta_offset_cqi_idx_mc_sf_set2_r13;
}

// PUSCH-ConfigDedicated-v1020 ::= SEQUENCE
SRSASN_CODE pusch_cfg_ded_v1020_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(beta_offset_mc_r10_present, 1));
  HANDLE_CODE(bref.pack(group_hop_disabled_r10_present, 1));
  HANDLE_CODE(bref.pack(dmrs_with_occ_activ_r10_present, 1));

  if (beta_offset_mc_r10_present) {
    HANDLE_CODE(pack_integer(bref, beta_offset_mc_r10.beta_offset_ack_idx_mc_r10, (uint8_t)0u, (uint8_t)15u));
    HANDLE_CODE(pack_integer(bref, beta_offset_mc_r10.beta_offset_ri_idx_mc_r10, (uint8_t)0u, (uint8_t)15u));
    HANDLE_CODE(pack_integer(bref, beta_offset_mc_r10.beta_offset_cqi_idx_mc_r10, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_v1020_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(beta_offset_mc_r10_present, 1));
  HANDLE_CODE(bref.unpack(group_hop_disabled_r10_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_with_occ_activ_r10_present, 1));

  if (beta_offset_mc_r10_present) {
    HANDLE_CODE(unpack_integer(beta_offset_mc_r10.beta_offset_ack_idx_mc_r10, bref, (uint8_t)0u, (uint8_t)15u));
    HANDLE_CODE(unpack_integer(beta_offset_mc_r10.beta_offset_ri_idx_mc_r10, bref, (uint8_t)0u, (uint8_t)15u));
    HANDLE_CODE(unpack_integer(beta_offset_mc_r10.beta_offset_cqi_idx_mc_r10, bref, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
void pusch_cfg_ded_v1020_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (beta_offset_mc_r10_present) {
    j.write_fieldname("betaOffsetMC-r10");
    j.start_obj();
    j.write_int("betaOffset-ACK-Index-MC-r10", beta_offset_mc_r10.beta_offset_ack_idx_mc_r10);
    j.write_int("betaOffset-RI-Index-MC-r10", beta_offset_mc_r10.beta_offset_ri_idx_mc_r10);
    j.write_int("betaOffset-CQI-Index-MC-r10", beta_offset_mc_r10.beta_offset_cqi_idx_mc_r10);
    j.end_obj();
  }
  if (group_hop_disabled_r10_present) {
    j.write_str("groupHoppingDisabled-r10", "true");
  }
  if (dmrs_with_occ_activ_r10_present) {
    j.write_str("dmrs-WithOCC-Activated-r10", "true");
  }
  j.end_obj();
}

// PUSCH-ConfigDedicated-v1130 ::= SEQUENCE
SRSASN_CODE pusch_cfg_ded_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pusch_dmrs_r11.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(pusch_dmrs_r11.unpack(bref));

  return SRSASN_SUCCESS;
}
void pusch_cfg_ded_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("pusch-DMRS-r11");
  pusch_dmrs_r11.to_json(j);
  j.end_obj();
}
bool pusch_cfg_ded_v1130_s::operator==(const pusch_cfg_ded_v1130_s& other) const
{
  return pusch_dmrs_r11 == other.pusch_dmrs_r11;
}

void pusch_cfg_ded_v1130_s::pusch_dmrs_r11_c_::set(types::options e)
{
  type_ = e;
}
void pusch_cfg_ded_v1130_s::pusch_dmrs_r11_c_::set_release()
{
  set(types::release);
}
pusch_cfg_ded_v1130_s::pusch_dmrs_r11_c_::setup_s_& pusch_cfg_ded_v1130_s::pusch_dmrs_r11_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pusch_cfg_ded_v1130_s::pusch_dmrs_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("nPUSCH-Identity-r11", c.npusch_id_r11);
      j.write_int("nDMRS-CSH-Identity-r11", c.ndmrs_csh_id_r11);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_v1130_s::pusch_dmrs_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE pusch_cfg_ded_v1130_s::pusch_dmrs_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c.npusch_id_r11, (uint16_t)0u, (uint16_t)509u));
      HANDLE_CODE(pack_integer(bref, c.ndmrs_csh_id_r11, (uint16_t)0u, (uint16_t)509u));
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_v1130_s::pusch_dmrs_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_v1130_s::pusch_dmrs_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c.npusch_id_r11, bref, (uint16_t)0u, (uint16_t)509u));
      HANDLE_CODE(unpack_integer(c.ndmrs_csh_id_r11, bref, (uint16_t)0u, (uint16_t)509u));
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_v1130_s::pusch_dmrs_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pusch_cfg_ded_v1130_s::pusch_dmrs_r11_c_::operator==(const pusch_dmrs_r11_c_& other) const
{
  return type() == other.type() and c.npusch_id_r11 == other.c.npusch_id_r11 and
         c.ndmrs_csh_id_r11 == other.c.ndmrs_csh_id_r11;
}

// PUSCH-ConfigDedicated-v1250 ::= SEQUENCE
SRSASN_CODE pusch_cfg_ded_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(uci_on_pusch.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(uci_on_pusch.unpack(bref));

  return SRSASN_SUCCESS;
}
void pusch_cfg_ded_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("uciOnPUSCH");
  uci_on_pusch.to_json(j);
  j.end_obj();
}

void pusch_cfg_ded_v1250_s::uci_on_pusch_c_::set(types::options e)
{
  type_ = e;
}
void pusch_cfg_ded_v1250_s::uci_on_pusch_c_::set_release()
{
  set(types::release);
}
pusch_cfg_ded_v1250_s::uci_on_pusch_c_::setup_s_& pusch_cfg_ded_v1250_s::uci_on_pusch_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pusch_cfg_ded_v1250_s::uci_on_pusch_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("betaOffset-ACK-Index-SubframeSet2-r12", c.beta_offset_ack_idx_sf_set2_r12);
      j.write_int("betaOffset-RI-Index-SubframeSet2-r12", c.beta_offset_ri_idx_sf_set2_r12);
      j.write_int("betaOffset-CQI-Index-SubframeSet2-r12", c.beta_offset_cqi_idx_sf_set2_r12);
      if (c.beta_offset_mc_r12_present) {
        j.write_fieldname("betaOffsetMC-r12");
        j.start_obj();
        j.write_int("betaOffset-ACK-Index-MC-SubframeSet2-r12",
                    c.beta_offset_mc_r12.beta_offset_ack_idx_mc_sf_set2_r12);
        j.write_int("betaOffset-RI-Index-MC-SubframeSet2-r12", c.beta_offset_mc_r12.beta_offset_ri_idx_mc_sf_set2_r12);
        j.write_int("betaOffset-CQI-Index-MC-SubframeSet2-r12",
                    c.beta_offset_mc_r12.beta_offset_cqi_idx_mc_sf_set2_r12);
        j.end_obj();
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_v1250_s::uci_on_pusch_c_");
  }
  j.end_obj();
}
SRSASN_CODE pusch_cfg_ded_v1250_s::uci_on_pusch_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.beta_offset_mc_r12_present, 1));
      HANDLE_CODE(pack_integer(bref, c.beta_offset_ack_idx_sf_set2_r12, (uint8_t)0u, (uint8_t)15u));
      HANDLE_CODE(pack_integer(bref, c.beta_offset_ri_idx_sf_set2_r12, (uint8_t)0u, (uint8_t)15u));
      HANDLE_CODE(pack_integer(bref, c.beta_offset_cqi_idx_sf_set2_r12, (uint8_t)0u, (uint8_t)15u));
      if (c.beta_offset_mc_r12_present) {
        HANDLE_CODE(
            pack_integer(bref, c.beta_offset_mc_r12.beta_offset_ack_idx_mc_sf_set2_r12, (uint8_t)0u, (uint8_t)15u));
        HANDLE_CODE(
            pack_integer(bref, c.beta_offset_mc_r12.beta_offset_ri_idx_mc_sf_set2_r12, (uint8_t)0u, (uint8_t)15u));
        HANDLE_CODE(
            pack_integer(bref, c.beta_offset_mc_r12.beta_offset_cqi_idx_mc_sf_set2_r12, (uint8_t)0u, (uint8_t)15u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_v1250_s::uci_on_pusch_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_v1250_s::uci_on_pusch_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.beta_offset_mc_r12_present, 1));
      HANDLE_CODE(unpack_integer(c.beta_offset_ack_idx_sf_set2_r12, bref, (uint8_t)0u, (uint8_t)15u));
      HANDLE_CODE(unpack_integer(c.beta_offset_ri_idx_sf_set2_r12, bref, (uint8_t)0u, (uint8_t)15u));
      HANDLE_CODE(unpack_integer(c.beta_offset_cqi_idx_sf_set2_r12, bref, (uint8_t)0u, (uint8_t)15u));
      if (c.beta_offset_mc_r12_present) {
        HANDLE_CODE(
            unpack_integer(c.beta_offset_mc_r12.beta_offset_ack_idx_mc_sf_set2_r12, bref, (uint8_t)0u, (uint8_t)15u));
        HANDLE_CODE(
            unpack_integer(c.beta_offset_mc_r12.beta_offset_ri_idx_mc_sf_set2_r12, bref, (uint8_t)0u, (uint8_t)15u));
        HANDLE_CODE(
            unpack_integer(c.beta_offset_mc_r12.beta_offset_cqi_idx_mc_sf_set2_r12, bref, (uint8_t)0u, (uint8_t)15u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_v1250_s::uci_on_pusch_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// PUSCH-ConfigDedicated-v1430 ::= SEQUENCE
SRSASN_CODE pusch_cfg_ded_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ce_pusch_nb_max_tbs_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_pusch_max_bw_r14_present, 1));
  HANDLE_CODE(bref.pack(tdd_pusch_up_pts_r14_present, 1));
  HANDLE_CODE(bref.pack(enable256_qam_r14_present, 1));

  if (tdd_pusch_up_pts_r14_present) {
    HANDLE_CODE(tdd_pusch_up_pts_r14.pack(bref));
  }
  HANDLE_CODE(bref.pack(ul_dmrs_ifdma_r14, 1));
  if (enable256_qam_r14_present) {
    HANDLE_CODE(enable256_qam_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ce_pusch_nb_max_tbs_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_pusch_max_bw_r14_present, 1));
  HANDLE_CODE(bref.unpack(tdd_pusch_up_pts_r14_present, 1));
  HANDLE_CODE(bref.unpack(enable256_qam_r14_present, 1));

  if (tdd_pusch_up_pts_r14_present) {
    HANDLE_CODE(tdd_pusch_up_pts_r14.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(ul_dmrs_ifdma_r14, 1));
  if (enable256_qam_r14_present) {
    HANDLE_CODE(enable256_qam_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pusch_cfg_ded_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ce_pusch_nb_max_tbs_r14_present) {
    j.write_str("ce-PUSCH-NB-MaxTBS-r14", "on");
  }
  if (ce_pusch_max_bw_r14_present) {
    j.write_str("ce-PUSCH-MaxBandwidth-r14", "bw5");
  }
  if (tdd_pusch_up_pts_r14_present) {
    j.write_fieldname("tdd-PUSCH-UpPTS-r14");
    tdd_pusch_up_pts_r14.to_json(j);
  }
  j.write_bool("ul-DMRS-IFDMA-r14", ul_dmrs_ifdma_r14);
  if (enable256_qam_r14_present) {
    j.write_fieldname("enable256QAM-r14");
    enable256_qam_r14.to_json(j);
  }
  j.end_obj();
}

// PUSCH-ConfigDedicated-v1530 ::= SEQUENCE
SRSASN_CODE pusch_cfg_ded_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ce_pusch_sub_prb_cfg_r15_present, 1));

  HANDLE_CODE(ce_pusch_flex_start_prb_alloc_cfg_r15.pack(bref));
  if (ce_pusch_sub_prb_cfg_r15_present) {
    HANDLE_CODE(ce_pusch_sub_prb_cfg_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ce_pusch_sub_prb_cfg_r15_present, 1));

  HANDLE_CODE(ce_pusch_flex_start_prb_alloc_cfg_r15.unpack(bref));
  if (ce_pusch_sub_prb_cfg_r15_present) {
    HANDLE_CODE(ce_pusch_sub_prb_cfg_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pusch_cfg_ded_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ce-PUSCH-FlexibleStartPRB-AllocConfig-r15");
  ce_pusch_flex_start_prb_alloc_cfg_r15.to_json(j);
  if (ce_pusch_sub_prb_cfg_r15_present) {
    j.write_fieldname("ce-PUSCH-SubPRB-Config-r15");
    ce_pusch_sub_prb_cfg_r15.to_json(j);
  }
  j.end_obj();
}

void pusch_cfg_ded_v1530_s::ce_pusch_flex_start_prb_alloc_cfg_r15_c_::set(types::options e)
{
  type_ = e;
}
void pusch_cfg_ded_v1530_s::ce_pusch_flex_start_prb_alloc_cfg_r15_c_::set_release()
{
  set(types::release);
}
pusch_cfg_ded_v1530_s::ce_pusch_flex_start_prb_alloc_cfg_r15_c_::setup_s_&
pusch_cfg_ded_v1530_s::ce_pusch_flex_start_prb_alloc_cfg_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pusch_cfg_ded_v1530_s::ce_pusch_flex_start_prb_alloc_cfg_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.offset_ce_mode_b_r15_present) {
        j.write_int("offsetCE-ModeB-r15", c.offset_ce_mode_b_r15);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_v1530_s::ce_pusch_flex_start_prb_alloc_cfg_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE pusch_cfg_ded_v1530_s::ce_pusch_flex_start_prb_alloc_cfg_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.offset_ce_mode_b_r15_present, 1));
      if (c.offset_ce_mode_b_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.offset_ce_mode_b_r15, (int8_t)-1, (int8_t)3));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_v1530_s::ce_pusch_flex_start_prb_alloc_cfg_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_v1530_s::ce_pusch_flex_start_prb_alloc_cfg_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.offset_ce_mode_b_r15_present, 1));
      if (c.offset_ce_mode_b_r15_present) {
        HANDLE_CODE(unpack_integer(c.offset_ce_mode_b_r15, bref, (int8_t)-1, (int8_t)3));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_v1530_s::ce_pusch_flex_start_prb_alloc_cfg_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void pusch_cfg_ded_v1530_s::ce_pusch_sub_prb_cfg_r15_c_::set(types::options e)
{
  type_ = e;
}
void pusch_cfg_ded_v1530_s::ce_pusch_sub_prb_cfg_r15_c_::set_release()
{
  set(types::release);
}
pusch_cfg_ded_v1530_s::ce_pusch_sub_prb_cfg_r15_c_::setup_s_&
pusch_cfg_ded_v1530_s::ce_pusch_sub_prb_cfg_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pusch_cfg_ded_v1530_s::ce_pusch_sub_prb_cfg_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.location_ce_mode_b_r15_present) {
        j.write_int("locationCE-ModeB-r15", c.location_ce_mode_b_r15);
      }
      j.write_int("sixToneCyclicShift-r15", c.six_tone_cyclic_shift_r15);
      j.write_int("threeToneCyclicShift-r15", c.three_tone_cyclic_shift_r15);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_v1530_s::ce_pusch_sub_prb_cfg_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE pusch_cfg_ded_v1530_s::ce_pusch_sub_prb_cfg_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.location_ce_mode_b_r15_present, 1));
      if (c.location_ce_mode_b_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.location_ce_mode_b_r15, (uint8_t)0u, (uint8_t)5u));
      }
      HANDLE_CODE(pack_integer(bref, c.six_tone_cyclic_shift_r15, (uint8_t)0u, (uint8_t)3u));
      HANDLE_CODE(pack_integer(bref, c.three_tone_cyclic_shift_r15, (uint8_t)0u, (uint8_t)2u));
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_v1530_s::ce_pusch_sub_prb_cfg_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_v1530_s::ce_pusch_sub_prb_cfg_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.location_ce_mode_b_r15_present, 1));
      if (c.location_ce_mode_b_r15_present) {
        HANDLE_CODE(unpack_integer(c.location_ce_mode_b_r15, bref, (uint8_t)0u, (uint8_t)5u));
      }
      HANDLE_CODE(unpack_integer(c.six_tone_cyclic_shift_r15, bref, (uint8_t)0u, (uint8_t)3u));
      HANDLE_CODE(unpack_integer(c.three_tone_cyclic_shift_r15, bref, (uint8_t)0u, (uint8_t)2u));
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_v1530_s::ce_pusch_sub_prb_cfg_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// PUSCH-ConfigDedicated-v1610 ::= SEQUENCE
SRSASN_CODE pusch_cfg_ded_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ce_pusch_multi_tb_cfg_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ce_pusch_multi_tb_cfg_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void pusch_cfg_ded_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ce-PUSCH-MultiTB-Config-r16");
  ce_pusch_multi_tb_cfg_r16.to_json(j);
  j.end_obj();
}

// PUSCH-EnhancementsConfig-r14 ::= CHOICE
void pusch_enhance_cfg_r14_c::set(types::options e)
{
  type_ = e;
}
void pusch_enhance_cfg_r14_c::set_release()
{
  set(types::release);
}
pusch_enhance_cfg_r14_c::setup_s_& pusch_enhance_cfg_r14_c::set_setup()
{
  set(types::setup);
  return c;
}
void pusch_enhance_cfg_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.pusch_hop_offset_pusch_enh_r14_present) {
        j.write_int("pusch-HoppingOffsetPUSCH-Enh-r14", c.pusch_hop_offset_pusch_enh_r14);
      }
      if (c.interv_ul_hop_pusch_enh_r14_present) {
        j.write_fieldname("interval-ULHoppingPUSCH-Enh-r14");
        c.interv_ul_hop_pusch_enh_r14.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pusch_enhance_cfg_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE pusch_enhance_cfg_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.pusch_hop_offset_pusch_enh_r14_present, 1));
      HANDLE_CODE(bref.pack(c.interv_ul_hop_pusch_enh_r14_present, 1));
      if (c.pusch_hop_offset_pusch_enh_r14_present) {
        HANDLE_CODE(pack_integer(bref, c.pusch_hop_offset_pusch_enh_r14, (uint8_t)1u, (uint8_t)100u));
      }
      if (c.interv_ul_hop_pusch_enh_r14_present) {
        HANDLE_CODE(c.interv_ul_hop_pusch_enh_r14.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pusch_enhance_cfg_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_enhance_cfg_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.pusch_hop_offset_pusch_enh_r14_present, 1));
      HANDLE_CODE(bref.unpack(c.interv_ul_hop_pusch_enh_r14_present, 1));
      if (c.pusch_hop_offset_pusch_enh_r14_present) {
        HANDLE_CODE(unpack_integer(c.pusch_hop_offset_pusch_enh_r14, bref, (uint8_t)1u, (uint8_t)100u));
      }
      if (c.interv_ul_hop_pusch_enh_r14_present) {
        HANDLE_CODE(c.interv_ul_hop_pusch_enh_r14.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pusch_enhance_cfg_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::destroy_() {}
void pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_ul_hop_pusch_enh_r14_c_(
    const pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::interv_fdd_pusch_enh_r14:
      c.init(other.c.get<interv_fdd_pusch_enh_r14_e_>());
      break;
    case types::interv_tdd_pusch_enh_r14:
      c.init(other.c.get<interv_tdd_pusch_enh_r14_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_");
  }
}
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_&
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::operator=(
    const pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::interv_fdd_pusch_enh_r14:
      c.set(other.c.get<interv_fdd_pusch_enh_r14_e_>());
      break;
    case types::interv_tdd_pusch_enh_r14:
      c.set(other.c.get<interv_tdd_pusch_enh_r14_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_");
  }

  return *this;
}
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_fdd_pusch_enh_r14_e_&
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::set_interv_fdd_pusch_enh_r14()
{
  set(types::interv_fdd_pusch_enh_r14);
  return c.get<interv_fdd_pusch_enh_r14_e_>();
}
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_tdd_pusch_enh_r14_e_&
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::set_interv_tdd_pusch_enh_r14()
{
  set(types::interv_tdd_pusch_enh_r14);
  return c.get<interv_tdd_pusch_enh_r14_e_>();
}
void pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::interv_fdd_pusch_enh_r14:
      j.write_str("interval-FDD-PUSCH-Enh-r14", c.get<interv_fdd_pusch_enh_r14_e_>().to_string());
      break;
    case types::interv_tdd_pusch_enh_r14:
      j.write_str("interval-TDD-PUSCH-Enh-r14", c.get<interv_tdd_pusch_enh_r14_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::interv_fdd_pusch_enh_r14:
      HANDLE_CODE(c.get<interv_fdd_pusch_enh_r14_e_>().pack(bref));
      break;
    case types::interv_tdd_pusch_enh_r14:
      HANDLE_CODE(c.get<interv_tdd_pusch_enh_r14_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::interv_fdd_pusch_enh_r14:
      HANDLE_CODE(c.get<interv_fdd_pusch_enh_r14_e_>().unpack(bref));
      break;
    case types::interv_tdd_pusch_enh_r14:
      HANDLE_CODE(c.get<interv_tdd_pusch_enh_r14_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char*
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_fdd_pusch_enh_r14_opts::to_string() const
{
  static const char* options[] = {"int1", "int2", "int4", "int8"};
  return convert_enum_idx(
      options,
      4,
      value,
      "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_fdd_pusch_enh_r14_e_");
}
uint8_t
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_fdd_pusch_enh_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(
      options,
      4,
      value,
      "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_fdd_pusch_enh_r14_e_");
}

const char*
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_tdd_pusch_enh_r14_opts::to_string() const
{
  static const char* options[] = {"int1", "int5", "int10", "int20"};
  return convert_enum_idx(
      options,
      4,
      value,
      "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_tdd_pusch_enh_r14_e_");
}
uint8_t
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_tdd_pusch_enh_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 5, 10, 20};
  return map_enum_number(
      options,
      4,
      value,
      "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_tdd_pusch_enh_r14_e_");
}

// PhysicalConfigDedicatedSTTI-r15 ::= CHOICE
void phys_cfg_ded_stti_r15_c::set(types::options e)
{
  type_ = e;
}
void phys_cfg_ded_stti_r15_c::set_release()
{
  set(types::release);
}
phys_cfg_ded_stti_r15_c::setup_s_& phys_cfg_ded_stti_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void phys_cfg_ded_stti_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.ant_info_ded_stti_r15_present) {
        j.write_fieldname("antennaInfoDedicatedSTTI-r15");
        c.ant_info_ded_stti_r15.to_json(j);
      }
      if (c.ant_info_ul_stti_r15_present) {
        j.write_fieldname("antennaInfoUL-STTI-r15");
        c.ant_info_ul_stti_r15.to_json(j);
      }
      if (c.pucch_cfg_ded_v1530_present) {
        j.write_fieldname("pucch-ConfigDedicated-v1530");
        c.pucch_cfg_ded_v1530.to_json(j);
      }
      if (c.sched_request_cfg_v1530_present) {
        j.write_fieldname("schedulingRequestConfig-v1530");
        c.sched_request_cfg_v1530.to_json(j);
      }
      if (c.ul_pwr_ctrl_ded_stti_r15_present) {
        j.write_fieldname("uplinkPowerControlDedicatedSTTI-r15");
        c.ul_pwr_ctrl_ded_stti_r15.to_json(j);
      }
      if (c.cqi_report_cfg_r15_present) {
        j.write_fieldname("cqi-ReportConfig-r15");
        c.cqi_report_cfg_r15.to_json(j);
      }
      if (c.csi_rs_cfg_r15_present) {
        j.write_fieldname("csi-RS-Config-r15");
        c.csi_rs_cfg_r15.to_json(j);
      }
      if (c.csi_rs_cfg_nzp_to_release_list_r15_present) {
        j.start_array("csi-RS-ConfigNZPToReleaseList-r15");
        for (const auto& e1 : c.csi_rs_cfg_nzp_to_release_list_r15) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.csi_rs_cfg_nzp_to_add_mod_list_r15_present) {
        j.start_array("csi-RS-ConfigNZPToAddModList-r15");
        for (const auto& e1 : c.csi_rs_cfg_nzp_to_add_mod_list_r15) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (c.csi_rs_cfg_zp_to_release_list_r15_present) {
        j.start_array("csi-RS-ConfigZPToReleaseList-r15");
        for (const auto& e1 : c.csi_rs_cfg_zp_to_release_list_r15) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.csi_rs_cfg_zp_to_add_mod_list_r11_present) {
        j.start_array("csi-RS-ConfigZPToAddModList-r11");
        for (const auto& e1 : c.csi_rs_cfg_zp_to_add_mod_list_r11) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (c.csi_rs_cfg_zp_ap_list_r15_present) {
        j.write_fieldname("csi-RS-ConfigZP-ApList-r15");
        c.csi_rs_cfg_zp_ap_list_r15.to_json(j);
      }
      if (c.eimta_main_cfg_r12_present) {
        j.write_fieldname("eimta-MainConfig-r12");
        c.eimta_main_cfg_r12.to_json(j);
      }
      if (c.eimta_main_cfg_serv_cell_r15_present) {
        j.write_fieldname("eimta-MainConfigServCell-r15");
        c.eimta_main_cfg_serv_cell_r15.to_json(j);
      }
      j.write_bool("semiOpenLoopSTTI-r15", c.semi_open_loop_stti_r15);
      if (c.slot_or_subslot_pdsch_cfg_r15_present) {
        j.write_fieldname("slotOrSubslotPDSCH-Config-r15");
        c.slot_or_subslot_pdsch_cfg_r15.to_json(j);
      }
      if (c.slot_or_subslot_pusch_cfg_r15_present) {
        j.write_fieldname("slotOrSubslotPUSCH-Config-r15");
        c.slot_or_subslot_pusch_cfg_r15.to_json(j);
      }
      if (c.spdcch_cfg_r15_present) {
        j.write_fieldname("spdcch-Config-r15");
        c.spdcch_cfg_r15.to_json(j);
      }
      if (c.spucch_cfg_r15_present) {
        j.write_fieldname("spucch-Config-r15");
        c.spucch_cfg_r15.to_json(j);
      }
      j.write_bool("srs-DCI7-TriggeringConfig-r15", c.srs_dci7_trigger_cfg_r15);
      j.write_bool("shortProcessingTime-r15", c.short_processing_time_r15);
      if (c.short_tti_r15_present) {
        j.write_fieldname("shortTTI-r15");
        c.short_tti_r15.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_stti_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_stti_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.ant_info_ded_stti_r15_present, 1));
      HANDLE_CODE(bref.pack(c.ant_info_ul_stti_r15_present, 1));
      HANDLE_CODE(bref.pack(c.pucch_cfg_ded_v1530_present, 1));
      HANDLE_CODE(bref.pack(c.sched_request_cfg_v1530_present, 1));
      HANDLE_CODE(bref.pack(c.ul_pwr_ctrl_ded_stti_r15_present, 1));
      HANDLE_CODE(bref.pack(c.cqi_report_cfg_r15_present, 1));
      HANDLE_CODE(bref.pack(c.csi_rs_cfg_r15_present, 1));
      HANDLE_CODE(bref.pack(c.csi_rs_cfg_nzp_to_release_list_r15_present, 1));
      HANDLE_CODE(bref.pack(c.csi_rs_cfg_nzp_to_add_mod_list_r15_present, 1));
      HANDLE_CODE(bref.pack(c.csi_rs_cfg_zp_to_release_list_r15_present, 1));
      HANDLE_CODE(bref.pack(c.csi_rs_cfg_zp_to_add_mod_list_r11_present, 1));
      HANDLE_CODE(bref.pack(c.csi_rs_cfg_zp_ap_list_r15_present, 1));
      HANDLE_CODE(bref.pack(c.eimta_main_cfg_r12_present, 1));
      HANDLE_CODE(bref.pack(c.eimta_main_cfg_serv_cell_r15_present, 1));
      HANDLE_CODE(bref.pack(c.slot_or_subslot_pdsch_cfg_r15_present, 1));
      HANDLE_CODE(bref.pack(c.slot_or_subslot_pusch_cfg_r15_present, 1));
      HANDLE_CODE(bref.pack(c.spdcch_cfg_r15_present, 1));
      HANDLE_CODE(bref.pack(c.spucch_cfg_r15_present, 1));
      HANDLE_CODE(bref.pack(c.short_tti_r15_present, 1));
      if (c.ant_info_ded_stti_r15_present) {
        HANDLE_CODE(c.ant_info_ded_stti_r15.pack(bref));
      }
      if (c.ant_info_ul_stti_r15_present) {
        HANDLE_CODE(c.ant_info_ul_stti_r15.pack(bref));
      }
      if (c.pucch_cfg_ded_v1530_present) {
        HANDLE_CODE(c.pucch_cfg_ded_v1530.pack(bref));
      }
      if (c.sched_request_cfg_v1530_present) {
        HANDLE_CODE(c.sched_request_cfg_v1530.pack(bref));
      }
      if (c.ul_pwr_ctrl_ded_stti_r15_present) {
        HANDLE_CODE(c.ul_pwr_ctrl_ded_stti_r15.pack(bref));
      }
      if (c.cqi_report_cfg_r15_present) {
        HANDLE_CODE(c.cqi_report_cfg_r15.pack(bref));
      }
      if (c.csi_rs_cfg_r15_present) {
        HANDLE_CODE(c.csi_rs_cfg_r15.pack(bref));
      }
      if (c.csi_rs_cfg_nzp_to_release_list_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.csi_rs_cfg_nzp_to_release_list_r15, 1, 24, integer_packer<uint8_t>(1, 24)));
      }
      if (c.csi_rs_cfg_nzp_to_add_mod_list_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.csi_rs_cfg_nzp_to_add_mod_list_r15, 1, 24));
      }
      if (c.csi_rs_cfg_zp_to_release_list_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.csi_rs_cfg_zp_to_release_list_r15, 1, 4, integer_packer<uint8_t>(1, 4)));
      }
      if (c.csi_rs_cfg_zp_to_add_mod_list_r11_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.csi_rs_cfg_zp_to_add_mod_list_r11, 1, 4));
      }
      if (c.csi_rs_cfg_zp_ap_list_r15_present) {
        HANDLE_CODE(c.csi_rs_cfg_zp_ap_list_r15.pack(bref));
      }
      if (c.eimta_main_cfg_r12_present) {
        HANDLE_CODE(c.eimta_main_cfg_r12.pack(bref));
      }
      if (c.eimta_main_cfg_serv_cell_r15_present) {
        HANDLE_CODE(c.eimta_main_cfg_serv_cell_r15.pack(bref));
      }
      HANDLE_CODE(bref.pack(c.semi_open_loop_stti_r15, 1));
      if (c.slot_or_subslot_pdsch_cfg_r15_present) {
        HANDLE_CODE(c.slot_or_subslot_pdsch_cfg_r15.pack(bref));
      }
      if (c.slot_or_subslot_pusch_cfg_r15_present) {
        HANDLE_CODE(c.slot_or_subslot_pusch_cfg_r15.pack(bref));
      }
      if (c.spdcch_cfg_r15_present) {
        HANDLE_CODE(c.spdcch_cfg_r15.pack(bref));
      }
      if (c.spucch_cfg_r15_present) {
        HANDLE_CODE(c.spucch_cfg_r15.pack(bref));
      }
      HANDLE_CODE(bref.pack(c.srs_dci7_trigger_cfg_r15, 1));
      HANDLE_CODE(bref.pack(c.short_processing_time_r15, 1));
      if (c.short_tti_r15_present) {
        HANDLE_CODE(c.short_tti_r15.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_stti_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_stti_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.ant_info_ded_stti_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.ant_info_ul_stti_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.pucch_cfg_ded_v1530_present, 1));
      HANDLE_CODE(bref.unpack(c.sched_request_cfg_v1530_present, 1));
      HANDLE_CODE(bref.unpack(c.ul_pwr_ctrl_ded_stti_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.cqi_report_cfg_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.csi_rs_cfg_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.csi_rs_cfg_nzp_to_release_list_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.csi_rs_cfg_nzp_to_add_mod_list_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.csi_rs_cfg_zp_to_release_list_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.csi_rs_cfg_zp_to_add_mod_list_r11_present, 1));
      HANDLE_CODE(bref.unpack(c.csi_rs_cfg_zp_ap_list_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.eimta_main_cfg_r12_present, 1));
      HANDLE_CODE(bref.unpack(c.eimta_main_cfg_serv_cell_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.slot_or_subslot_pdsch_cfg_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.slot_or_subslot_pusch_cfg_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.spdcch_cfg_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.spucch_cfg_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.short_tti_r15_present, 1));
      if (c.ant_info_ded_stti_r15_present) {
        HANDLE_CODE(c.ant_info_ded_stti_r15.unpack(bref));
      }
      if (c.ant_info_ul_stti_r15_present) {
        HANDLE_CODE(c.ant_info_ul_stti_r15.unpack(bref));
      }
      if (c.pucch_cfg_ded_v1530_present) {
        HANDLE_CODE(c.pucch_cfg_ded_v1530.unpack(bref));
      }
      if (c.sched_request_cfg_v1530_present) {
        HANDLE_CODE(c.sched_request_cfg_v1530.unpack(bref));
      }
      if (c.ul_pwr_ctrl_ded_stti_r15_present) {
        HANDLE_CODE(c.ul_pwr_ctrl_ded_stti_r15.unpack(bref));
      }
      if (c.cqi_report_cfg_r15_present) {
        HANDLE_CODE(c.cqi_report_cfg_r15.unpack(bref));
      }
      if (c.csi_rs_cfg_r15_present) {
        HANDLE_CODE(c.csi_rs_cfg_r15.unpack(bref));
      }
      if (c.csi_rs_cfg_nzp_to_release_list_r15_present) {
        HANDLE_CODE(
            unpack_dyn_seq_of(c.csi_rs_cfg_nzp_to_release_list_r15, bref, 1, 24, integer_packer<uint8_t>(1, 24)));
      }
      if (c.csi_rs_cfg_nzp_to_add_mod_list_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.csi_rs_cfg_nzp_to_add_mod_list_r15, bref, 1, 24));
      }
      if (c.csi_rs_cfg_zp_to_release_list_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.csi_rs_cfg_zp_to_release_list_r15, bref, 1, 4, integer_packer<uint8_t>(1, 4)));
      }
      if (c.csi_rs_cfg_zp_to_add_mod_list_r11_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.csi_rs_cfg_zp_to_add_mod_list_r11, bref, 1, 4));
      }
      if (c.csi_rs_cfg_zp_ap_list_r15_present) {
        HANDLE_CODE(c.csi_rs_cfg_zp_ap_list_r15.unpack(bref));
      }
      if (c.eimta_main_cfg_r12_present) {
        HANDLE_CODE(c.eimta_main_cfg_r12.unpack(bref));
      }
      if (c.eimta_main_cfg_serv_cell_r15_present) {
        HANDLE_CODE(c.eimta_main_cfg_serv_cell_r15.unpack(bref));
      }
      HANDLE_CODE(bref.unpack(c.semi_open_loop_stti_r15, 1));
      if (c.slot_or_subslot_pdsch_cfg_r15_present) {
        HANDLE_CODE(c.slot_or_subslot_pdsch_cfg_r15.unpack(bref));
      }
      if (c.slot_or_subslot_pusch_cfg_r15_present) {
        HANDLE_CODE(c.slot_or_subslot_pusch_cfg_r15.unpack(bref));
      }
      if (c.spdcch_cfg_r15_present) {
        HANDLE_CODE(c.spdcch_cfg_r15.unpack(bref));
      }
      if (c.spucch_cfg_r15_present) {
        HANDLE_CODE(c.spucch_cfg_r15.unpack(bref));
      }
      HANDLE_CODE(bref.unpack(c.srs_dci7_trigger_cfg_r15, 1));
      HANDLE_CODE(bref.unpack(c.short_processing_time_r15, 1));
      if (c.short_tti_r15_present) {
        HANDLE_CODE(c.short_tti_r15.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_stti_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool phys_cfg_ded_stti_r15_c::operator==(const phys_cfg_ded_stti_r15_c& other) const
{
  return type() == other.type() and c.ant_info_ded_stti_r15_present == other.c.ant_info_ded_stti_r15_present and
         (not c.ant_info_ded_stti_r15_present or c.ant_info_ded_stti_r15 == other.c.ant_info_ded_stti_r15) and
         c.ant_info_ul_stti_r15_present == other.c.ant_info_ul_stti_r15_present and
         (not c.ant_info_ul_stti_r15_present or c.ant_info_ul_stti_r15 == other.c.ant_info_ul_stti_r15) and
         c.pucch_cfg_ded_v1530_present == other.c.pucch_cfg_ded_v1530_present and
         (not c.pucch_cfg_ded_v1530_present or c.pucch_cfg_ded_v1530 == other.c.pucch_cfg_ded_v1530) and
         c.sched_request_cfg_v1530_present == other.c.sched_request_cfg_v1530_present and
         (not c.sched_request_cfg_v1530_present or c.sched_request_cfg_v1530 == other.c.sched_request_cfg_v1530) and
         c.ul_pwr_ctrl_ded_stti_r15_present == other.c.ul_pwr_ctrl_ded_stti_r15_present and
         (not c.ul_pwr_ctrl_ded_stti_r15_present or c.ul_pwr_ctrl_ded_stti_r15 == other.c.ul_pwr_ctrl_ded_stti_r15) and
         c.cqi_report_cfg_r15_present == other.c.cqi_report_cfg_r15_present and
         (not c.cqi_report_cfg_r15_present or c.cqi_report_cfg_r15 == other.c.cqi_report_cfg_r15) and
         c.csi_rs_cfg_r15_present == other.c.csi_rs_cfg_r15_present and
         (not c.csi_rs_cfg_r15_present or c.csi_rs_cfg_r15 == other.c.csi_rs_cfg_r15) and
         c.csi_rs_cfg_nzp_to_release_list_r15_present == other.c.csi_rs_cfg_nzp_to_release_list_r15_present and
         (not c.csi_rs_cfg_nzp_to_release_list_r15_present or
          c.csi_rs_cfg_nzp_to_release_list_r15 == other.c.csi_rs_cfg_nzp_to_release_list_r15) and
         c.csi_rs_cfg_nzp_to_add_mod_list_r15_present == other.c.csi_rs_cfg_nzp_to_add_mod_list_r15_present and
         (not c.csi_rs_cfg_nzp_to_add_mod_list_r15_present or
          c.csi_rs_cfg_nzp_to_add_mod_list_r15 == other.c.csi_rs_cfg_nzp_to_add_mod_list_r15) and
         c.csi_rs_cfg_zp_to_release_list_r15_present == other.c.csi_rs_cfg_zp_to_release_list_r15_present and
         (not c.csi_rs_cfg_zp_to_release_list_r15_present or
          c.csi_rs_cfg_zp_to_release_list_r15 == other.c.csi_rs_cfg_zp_to_release_list_r15) and
         c.csi_rs_cfg_zp_to_add_mod_list_r11_present == other.c.csi_rs_cfg_zp_to_add_mod_list_r11_present and
         (not c.csi_rs_cfg_zp_to_add_mod_list_r11_present or
          c.csi_rs_cfg_zp_to_add_mod_list_r11 == other.c.csi_rs_cfg_zp_to_add_mod_list_r11) and
         c.csi_rs_cfg_zp_ap_list_r15_present == other.c.csi_rs_cfg_zp_ap_list_r15_present and
         (not c.csi_rs_cfg_zp_ap_list_r15_present or
          c.csi_rs_cfg_zp_ap_list_r15 == other.c.csi_rs_cfg_zp_ap_list_r15) and
         c.eimta_main_cfg_r12_present == other.c.eimta_main_cfg_r12_present and
         (not c.eimta_main_cfg_r12_present or c.eimta_main_cfg_r12 == other.c.eimta_main_cfg_r12) and
         c.eimta_main_cfg_serv_cell_r15_present == other.c.eimta_main_cfg_serv_cell_r15_present and
         (not c.eimta_main_cfg_serv_cell_r15_present or
          c.eimta_main_cfg_serv_cell_r15 == other.c.eimta_main_cfg_serv_cell_r15) and
         c.semi_open_loop_stti_r15 == other.c.semi_open_loop_stti_r15 and
         c.slot_or_subslot_pdsch_cfg_r15_present == other.c.slot_or_subslot_pdsch_cfg_r15_present and
         (not c.slot_or_subslot_pdsch_cfg_r15_present or
          c.slot_or_subslot_pdsch_cfg_r15 == other.c.slot_or_subslot_pdsch_cfg_r15) and
         c.slot_or_subslot_pusch_cfg_r15_present == other.c.slot_or_subslot_pusch_cfg_r15_present and
         (not c.slot_or_subslot_pusch_cfg_r15_present or
          c.slot_or_subslot_pusch_cfg_r15 == other.c.slot_or_subslot_pusch_cfg_r15) and
         c.spdcch_cfg_r15_present == other.c.spdcch_cfg_r15_present and
         (not c.spdcch_cfg_r15_present or c.spdcch_cfg_r15 == other.c.spdcch_cfg_r15) and
         c.spucch_cfg_r15_present == other.c.spucch_cfg_r15_present and
         (not c.spucch_cfg_r15_present or c.spucch_cfg_r15 == other.c.spucch_cfg_r15) and
         c.srs_dci7_trigger_cfg_r15 == other.c.srs_dci7_trigger_cfg_r15 and
         c.short_processing_time_r15 == other.c.short_processing_time_r15 and
         c.short_tti_r15_present == other.c.short_tti_r15_present and
         (not c.short_tti_r15_present or c.short_tti_r15 == other.c.short_tti_r15);
}

// ResourceReservationConfigDedicatedDL-r16 ::= SEQUENCE
SRSASN_CODE res_reserv_cfg_ded_dl_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(res_reserv_ded_dl_r16_present, 1));

  if (res_reserv_ded_dl_r16_present) {
    HANDLE_CODE(res_reserv_ded_dl_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE res_reserv_cfg_ded_dl_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(res_reserv_ded_dl_r16_present, 1));

  if (res_reserv_ded_dl_r16_present) {
    HANDLE_CODE(res_reserv_ded_dl_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void res_reserv_cfg_ded_dl_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (res_reserv_ded_dl_r16_present) {
    j.write_fieldname("resourceReservationDedicatedDL-r16");
    res_reserv_ded_dl_r16.to_json(j);
  }
  j.end_obj();
}

// ResourceReservationConfigDedicatedUL-r16 ::= SEQUENCE
SRSASN_CODE res_reserv_cfg_ded_ul_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(res_reserv_ded_ul_r16_present, 1));

  if (res_reserv_ded_ul_r16_present) {
    HANDLE_CODE(res_reserv_ded_ul_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE res_reserv_cfg_ded_ul_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(res_reserv_ded_ul_r16_present, 1));

  if (res_reserv_ded_ul_r16_present) {
    HANDLE_CODE(res_reserv_ded_ul_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void res_reserv_cfg_ded_ul_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (res_reserv_ded_ul_r16_present) {
    j.write_fieldname("resourceReservationDedicatedUL-r16");
    res_reserv_ded_ul_r16.to_json(j);
  }
  j.end_obj();
}

// SPUCCH-Config-v1550 ::= CHOICE
void spucch_cfg_v1550_c::set(types::options e)
{
  type_ = e;
}
void spucch_cfg_v1550_c::set_release()
{
  set(types::release);
}
spucch_cfg_v1550_c::setup_s_& spucch_cfg_v1550_c::set_setup()
{
  set(types::setup);
  return c;
}
void spucch_cfg_v1550_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("twoAntennaPortActivatedSPUCCH-Format3-v1550");
      j.start_obj();
      j.start_array("n3SPUCCH-AN-List-v1550");
      for (const auto& e1 : c.two_ant_port_activ_spucch_format3_v1550.n3_spucch_an_list_v1550) {
        j.write_int(e1);
      }
      j.end_array();
      j.end_obj();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "spucch_cfg_v1550_c");
  }
  j.end_obj();
}
SRSASN_CODE spucch_cfg_v1550_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  c.two_ant_port_activ_spucch_format3_v1550.n3_spucch_an_list_v1550,
                                  1,
                                  4,
                                  integer_packer<uint16_t>(0, 549)));
      break;
    default:
      log_invalid_choice_id(type_, "spucch_cfg_v1550_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE spucch_cfg_v1550_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.two_ant_port_activ_spucch_format3_v1550.n3_spucch_an_list_v1550,
                                    bref,
                                    1,
                                    4,
                                    integer_packer<uint16_t>(0, 549)));
      break;
    default:
      log_invalid_choice_id(type_, "spucch_cfg_v1550_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool spucch_cfg_v1550_c::operator==(const spucch_cfg_v1550_c& other) const
{
  return type() == other.type() and c.two_ant_port_activ_spucch_format3_v1550.n3_spucch_an_list_v1550 ==
                                        other.c.two_ant_port_activ_spucch_format3_v1550.n3_spucch_an_list_v1550;
}

// SRS-TPC-PDCCH-Config-r14 ::= CHOICE
void srs_tpc_pdcch_cfg_r14_c::set(types::options e)
{
  type_ = e;
}
void srs_tpc_pdcch_cfg_r14_c::set_release()
{
  set(types::release);
}
srs_tpc_pdcch_cfg_r14_c::setup_s_& srs_tpc_pdcch_cfg_r14_c::set_setup()
{
  set(types::setup);
  return c;
}
void srs_tpc_pdcch_cfg_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("srs-TPC-RNTI-r14", c.srs_tpc_rnti_r14.to_string());
      j.write_int("startingBitOfFormat3B-r14", c.start_bit_of_format3_b_r14);
      j.write_int("fieldTypeFormat3B-r14", c.field_type_format3_b_r14);
      if (c.srs_cc_set_idxlist_r14_present) {
        j.start_array("srs-CC-SetIndexlist-r14");
        for (const auto& e1 : c.srs_cc_set_idxlist_r14) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_tpc_pdcch_cfg_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE srs_tpc_pdcch_cfg_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.srs_cc_set_idxlist_r14_present, 1));
      HANDLE_CODE(c.srs_tpc_rnti_r14.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.start_bit_of_format3_b_r14, (uint8_t)0u, (uint8_t)31u));
      HANDLE_CODE(pack_integer(bref, c.field_type_format3_b_r14, (uint8_t)1u, (uint8_t)4u));
      if (c.srs_cc_set_idxlist_r14_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.srs_cc_set_idxlist_r14, 1, 4));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_tpc_pdcch_cfg_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_tpc_pdcch_cfg_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.srs_cc_set_idxlist_r14_present, 1));
      HANDLE_CODE(c.srs_tpc_rnti_r14.unpack(bref));
      HANDLE_CODE(unpack_integer(c.start_bit_of_format3_b_r14, bref, (uint8_t)0u, (uint8_t)31u));
      HANDLE_CODE(unpack_integer(c.field_type_format3_b_r14, bref, (uint8_t)1u, (uint8_t)4u));
      if (c.srs_cc_set_idxlist_r14_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.srs_cc_set_idxlist_r14, bref, 1, 4));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_tpc_pdcch_cfg_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srs_tpc_pdcch_cfg_r14_c::operator==(const srs_tpc_pdcch_cfg_r14_c& other) const
{
  return type() == other.type() and c.srs_tpc_rnti_r14 == other.c.srs_tpc_rnti_r14 and
         c.start_bit_of_format3_b_r14 == other.c.start_bit_of_format3_b_r14 and
         c.field_type_format3_b_r14 == other.c.field_type_format3_b_r14 and
         c.srs_cc_set_idxlist_r14_present == other.c.srs_cc_set_idxlist_r14_present and
         (not c.srs_cc_set_idxlist_r14_present or c.srs_cc_set_idxlist_r14 == other.c.srs_cc_set_idxlist_r14);
}

// SchedulingRequestConfig ::= CHOICE
void sched_request_cfg_c::set(types::options e)
{
  type_ = e;
}
void sched_request_cfg_c::set_release()
{
  set(types::release);
}
sched_request_cfg_c::setup_s_& sched_request_cfg_c::set_setup()
{
  set(types::setup);
  return c;
}
void sched_request_cfg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("sr-PUCCH-ResourceIndex", c.sr_pucch_res_idx);
      j.write_int("sr-ConfigIndex", c.sr_cfg_idx);
      j.write_str("dsr-TransMax", c.dsr_trans_max.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sched_request_cfg_c");
  }
  j.end_obj();
}
SRSASN_CODE sched_request_cfg_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c.sr_pucch_res_idx, (uint16_t)0u, (uint16_t)2047u));
      HANDLE_CODE(pack_integer(bref, c.sr_cfg_idx, (uint8_t)0u, (uint8_t)157u));
      HANDLE_CODE(c.dsr_trans_max.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sched_request_cfg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_request_cfg_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c.sr_pucch_res_idx, bref, (uint16_t)0u, (uint16_t)2047u));
      HANDLE_CODE(unpack_integer(c.sr_cfg_idx, bref, (uint8_t)0u, (uint8_t)157u));
      HANDLE_CODE(c.dsr_trans_max.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sched_request_cfg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sched_request_cfg_c::setup_s_::dsr_trans_max_opts::to_string() const
{
  static const char* options[] = {"n4", "n8", "n16", "n32", "n64", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "sched_request_cfg_c::setup_s_::dsr_trans_max_e_");
}
uint8_t sched_request_cfg_c::setup_s_::dsr_trans_max_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 16, 32, 64};
  return map_enum_number(options, 5, value, "sched_request_cfg_c::setup_s_::dsr_trans_max_e_");
}

// SchedulingRequestConfig-v1020 ::= SEQUENCE
SRSASN_CODE sched_request_cfg_v1020_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sr_pucch_res_idx_p1_r10_present, 1));

  if (sr_pucch_res_idx_p1_r10_present) {
    HANDLE_CODE(pack_integer(bref, sr_pucch_res_idx_p1_r10, (uint16_t)0u, (uint16_t)2047u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_request_cfg_v1020_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sr_pucch_res_idx_p1_r10_present, 1));

  if (sr_pucch_res_idx_p1_r10_present) {
    HANDLE_CODE(unpack_integer(sr_pucch_res_idx_p1_r10, bref, (uint16_t)0u, (uint16_t)2047u));
  }

  return SRSASN_SUCCESS;
}
void sched_request_cfg_v1020_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sr_pucch_res_idx_p1_r10_present) {
    j.write_int("sr-PUCCH-ResourceIndexP1-r10", sr_pucch_res_idx_p1_r10);
  }
  j.end_obj();
}

// SoundingRS-UL-ConfigDedicated ::= CHOICE
void srs_ul_cfg_ded_c::set(types::options e)
{
  type_ = e;
}
void srs_ul_cfg_ded_c::set_release()
{
  set(types::release);
}
srs_ul_cfg_ded_c::setup_s_& srs_ul_cfg_ded_c::set_setup()
{
  set(types::setup);
  return c;
}
void srs_ul_cfg_ded_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("srs-Bandwidth", c.srs_bw.to_string());
      j.write_str("srs-HoppingBandwidth", c.srs_hop_bw.to_string());
      j.write_int("freqDomainPosition", c.freq_domain_position);
      j.write_bool("duration", c.dur);
      j.write_int("srs-ConfigIndex", c.srs_cfg_idx);
      j.write_int("transmissionComb", c.tx_comb);
      j.write_str("cyclicShift", c.cyclic_shift.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_c");
  }
  j.end_obj();
}
SRSASN_CODE srs_ul_cfg_ded_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.srs_bw.pack(bref));
      HANDLE_CODE(c.srs_hop_bw.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.freq_domain_position, (uint8_t)0u, (uint8_t)23u));
      HANDLE_CODE(bref.pack(c.dur, 1));
      HANDLE_CODE(pack_integer(bref, c.srs_cfg_idx, (uint16_t)0u, (uint16_t)1023u));
      HANDLE_CODE(pack_integer(bref, c.tx_comb, (uint8_t)0u, (uint8_t)1u));
      HANDLE_CODE(c.cyclic_shift.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_ul_cfg_ded_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.srs_bw.unpack(bref));
      HANDLE_CODE(c.srs_hop_bw.unpack(bref));
      HANDLE_CODE(unpack_integer(c.freq_domain_position, bref, (uint8_t)0u, (uint8_t)23u));
      HANDLE_CODE(bref.unpack(c.dur, 1));
      HANDLE_CODE(unpack_integer(c.srs_cfg_idx, bref, (uint16_t)0u, (uint16_t)1023u));
      HANDLE_CODE(unpack_integer(c.tx_comb, bref, (uint8_t)0u, (uint8_t)1u));
      HANDLE_CODE(c.cyclic_shift.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srs_ul_cfg_ded_c::operator==(const srs_ul_cfg_ded_c& other) const
{
  return type() == other.type() and c.srs_bw == other.c.srs_bw and c.srs_hop_bw == other.c.srs_hop_bw and
         c.freq_domain_position == other.c.freq_domain_position and c.dur == other.c.dur and
         c.srs_cfg_idx == other.c.srs_cfg_idx and c.tx_comb == other.c.tx_comb and
         c.cyclic_shift == other.c.cyclic_shift;
}

const char* srs_ul_cfg_ded_c::setup_s_::srs_bw_opts::to_string() const
{
  static const char* options[] = {"bw0", "bw1", "bw2", "bw3"};
  return convert_enum_idx(options, 4, value, "srs_ul_cfg_ded_c::setup_s_::srs_bw_e_");
}
uint8_t srs_ul_cfg_ded_c::setup_s_::srs_bw_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "srs_ul_cfg_ded_c::setup_s_::srs_bw_e_");
}

const char* srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_opts::to_string() const
{
  static const char* options[] = {"hbw0", "hbw1", "hbw2", "hbw3"};
  return convert_enum_idx(options, 4, value, "srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_e_");
}
uint8_t srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_e_");
}

const char* srs_ul_cfg_ded_c::setup_s_::cyclic_shift_opts::to_string() const
{
  static const char* options[] = {"cs0", "cs1", "cs2", "cs3", "cs4", "cs5", "cs6", "cs7"};
  return convert_enum_idx(options, 8, value, "srs_ul_cfg_ded_c::setup_s_::cyclic_shift_e_");
}
uint8_t srs_ul_cfg_ded_c::setup_s_::cyclic_shift_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return map_enum_number(options, 8, value, "srs_ul_cfg_ded_c::setup_s_::cyclic_shift_e_");
}

// SoundingRS-UL-ConfigDedicated-v1020 ::= SEQUENCE
SRSASN_CODE srs_ul_cfg_ded_v1020_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(srs_ant_port_r10.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_ul_cfg_ded_v1020_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(srs_ant_port_r10.unpack(bref));

  return SRSASN_SUCCESS;
}
void srs_ul_cfg_ded_v1020_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("srs-AntennaPort-r10", srs_ant_port_r10.to_string());
  j.end_obj();
}
bool srs_ul_cfg_ded_v1020_s::operator==(const srs_ul_cfg_ded_v1020_s& other) const
{
  return srs_ant_port_r10 == other.srs_ant_port_r10;
}

// SoundingRS-UL-ConfigDedicated-v1310 ::= CHOICE
void srs_ul_cfg_ded_v1310_c::set(types::options e)
{
  type_ = e;
}
void srs_ul_cfg_ded_v1310_c::set_release()
{
  set(types::release);
}
srs_ul_cfg_ded_v1310_c::setup_s_& srs_ul_cfg_ded_v1310_c::set_setup()
{
  set(types::setup);
  return c;
}
void srs_ul_cfg_ded_v1310_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.tx_comb_v1310_present) {
        j.write_int("transmissionComb-v1310", c.tx_comb_v1310);
      }
      if (c.cyclic_shift_v1310_present) {
        j.write_str("cyclicShift-v1310", c.cyclic_shift_v1310.to_string());
      }
      if (c.tx_comb_num_r13_present) {
        j.write_str("transmissionCombNum-r13", c.tx_comb_num_r13.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_v1310_c");
  }
  j.end_obj();
}
SRSASN_CODE srs_ul_cfg_ded_v1310_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.tx_comb_v1310_present, 1));
      HANDLE_CODE(bref.pack(c.cyclic_shift_v1310_present, 1));
      HANDLE_CODE(bref.pack(c.tx_comb_num_r13_present, 1));
      if (c.tx_comb_v1310_present) {
        HANDLE_CODE(pack_integer(bref, c.tx_comb_v1310, (uint8_t)2u, (uint8_t)3u));
      }
      if (c.cyclic_shift_v1310_present) {
        HANDLE_CODE(c.cyclic_shift_v1310.pack(bref));
      }
      if (c.tx_comb_num_r13_present) {
        HANDLE_CODE(c.tx_comb_num_r13.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_v1310_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_ul_cfg_ded_v1310_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.tx_comb_v1310_present, 1));
      HANDLE_CODE(bref.unpack(c.cyclic_shift_v1310_present, 1));
      HANDLE_CODE(bref.unpack(c.tx_comb_num_r13_present, 1));
      if (c.tx_comb_v1310_present) {
        HANDLE_CODE(unpack_integer(c.tx_comb_v1310, bref, (uint8_t)2u, (uint8_t)3u));
      }
      if (c.cyclic_shift_v1310_present) {
        HANDLE_CODE(c.cyclic_shift_v1310.unpack(bref));
      }
      if (c.tx_comb_num_r13_present) {
        HANDLE_CODE(c.tx_comb_num_r13.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_v1310_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srs_ul_cfg_ded_v1310_c::operator==(const srs_ul_cfg_ded_v1310_c& other) const
{
  return type() == other.type() and c.tx_comb_v1310_present == other.c.tx_comb_v1310_present and
         (not c.tx_comb_v1310_present or c.tx_comb_v1310 == other.c.tx_comb_v1310) and
         c.cyclic_shift_v1310_present == other.c.cyclic_shift_v1310_present and
         (not c.cyclic_shift_v1310_present or c.cyclic_shift_v1310 == other.c.cyclic_shift_v1310) and
         c.tx_comb_num_r13_present == other.c.tx_comb_num_r13_present and
         (not c.tx_comb_num_r13_present or c.tx_comb_num_r13 == other.c.tx_comb_num_r13);
}

const char* srs_ul_cfg_ded_v1310_c::setup_s_::cyclic_shift_v1310_opts::to_string() const
{
  static const char* options[] = {"cs8", "cs9", "cs10", "cs11"};
  return convert_enum_idx(options, 4, value, "srs_ul_cfg_ded_v1310_c::setup_s_::cyclic_shift_v1310_e_");
}
uint8_t srs_ul_cfg_ded_v1310_c::setup_s_::cyclic_shift_v1310_opts::to_number() const
{
  static const uint8_t options[] = {8, 9, 10, 11};
  return map_enum_number(options, 4, value, "srs_ul_cfg_ded_v1310_c::setup_s_::cyclic_shift_v1310_e_");
}

const char* srs_ul_cfg_ded_v1310_c::setup_s_::tx_comb_num_r13_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "srs_ul_cfg_ded_v1310_c::setup_s_::tx_comb_num_r13_e_");
}
uint8_t srs_ul_cfg_ded_v1310_c::setup_s_::tx_comb_num_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "srs_ul_cfg_ded_v1310_c::setup_s_::tx_comb_num_r13_e_");
}

// SoundingRS-UL-ConfigDedicatedAdd-r16 ::= SEQUENCE
SRSASN_CODE srs_ul_cfg_ded_add_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_cfg_ap_dci_format4_r16_present, 1));
  HANDLE_CODE(bref.pack(srs_activ_ap_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, srs_cfg_idx_ap_r16, (uint8_t)0u, (uint8_t)31u));
  if (srs_cfg_ap_dci_format4_r16_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_cfg_ap_dci_format4_r16, 1, 3));
  }
  if (srs_activ_ap_r13_present) {
    HANDLE_CODE(srs_activ_ap_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_ul_cfg_ded_add_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_cfg_ap_dci_format4_r16_present, 1));
  HANDLE_CODE(bref.unpack(srs_activ_ap_r13_present, 1));

  HANDLE_CODE(unpack_integer(srs_cfg_idx_ap_r16, bref, (uint8_t)0u, (uint8_t)31u));
  if (srs_cfg_ap_dci_format4_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_cfg_ap_dci_format4_r16, bref, 1, 3));
  }
  if (srs_activ_ap_r13_present) {
    HANDLE_CODE(srs_activ_ap_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void srs_ul_cfg_ded_add_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("srs-ConfigIndexAp-r16", srs_cfg_idx_ap_r16);
  if (srs_cfg_ap_dci_format4_r16_present) {
    j.start_array("srs-ConfigApDCI-Format4-r16");
    for (const auto& e1 : srs_cfg_ap_dci_format4_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srs_activ_ap_r13_present) {
    j.write_fieldname("srs-ActivateAp-r13");
    srs_activ_ap_r13.to_json(j);
  }
  j.end_obj();
}
bool srs_ul_cfg_ded_add_r16_s::operator==(const srs_ul_cfg_ded_add_r16_s& other) const
{
  return srs_cfg_idx_ap_r16 == other.srs_cfg_idx_ap_r16 and
         srs_cfg_ap_dci_format4_r16_present == other.srs_cfg_ap_dci_format4_r16_present and
         (not srs_cfg_ap_dci_format4_r16_present or srs_cfg_ap_dci_format4_r16 == other.srs_cfg_ap_dci_format4_r16) and
         srs_activ_ap_r13_present == other.srs_activ_ap_r13_present and
         (not srs_activ_ap_r13_present or srs_activ_ap_r13 == other.srs_activ_ap_r13);
}

void srs_ul_cfg_ded_add_r16_s::srs_activ_ap_r13_c_::set(types::options e)
{
  type_ = e;
}
void srs_ul_cfg_ded_add_r16_s::srs_activ_ap_r13_c_::set_release()
{
  set(types::release);
}
srs_ul_cfg_ded_add_r16_s::srs_activ_ap_r13_c_::setup_s_& srs_ul_cfg_ded_add_r16_s::srs_activ_ap_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void srs_ul_cfg_ded_add_r16_s::srs_activ_ap_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("srs-ConfigApDCI-Format0-r16");
      c.srs_cfg_ap_dci_format0_r16.to_json(j);
      j.write_fieldname("srs-ConfigApDCI-Format1a2b2c-r16");
      c.srs_cfg_ap_dci_format1a2b2c_r16.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_add_r16_s::srs_activ_ap_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_ul_cfg_ded_add_r16_s::srs_activ_ap_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.srs_cfg_ap_dci_format0_r16.pack(bref));
      HANDLE_CODE(c.srs_cfg_ap_dci_format1a2b2c_r16.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_add_r16_s::srs_activ_ap_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_ul_cfg_ded_add_r16_s::srs_activ_ap_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.srs_cfg_ap_dci_format0_r16.unpack(bref));
      HANDLE_CODE(c.srs_cfg_ap_dci_format1a2b2c_r16.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_add_r16_s::srs_activ_ap_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srs_ul_cfg_ded_add_r16_s::srs_activ_ap_r13_c_::operator==(const srs_activ_ap_r13_c_& other) const
{
  return type() == other.type() and c.srs_cfg_ap_dci_format0_r16 == other.c.srs_cfg_ap_dci_format0_r16 and
         c.srs_cfg_ap_dci_format1a2b2c_r16 == other.c.srs_cfg_ap_dci_format1a2b2c_r16;
}

// SoundingRS-UL-ConfigDedicatedAperiodic-r10 ::= CHOICE
void srs_ul_cfg_ded_aperiodic_r10_c::set(types::options e)
{
  type_ = e;
}
void srs_ul_cfg_ded_aperiodic_r10_c::set_release()
{
  set(types::release);
}
srs_ul_cfg_ded_aperiodic_r10_c::setup_s_& srs_ul_cfg_ded_aperiodic_r10_c::set_setup()
{
  set(types::setup);
  return c;
}
void srs_ul_cfg_ded_aperiodic_r10_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("srs-ConfigIndexAp-r10", c.srs_cfg_idx_ap_r10);
      if (c.srs_cfg_ap_dci_format4_r10_present) {
        j.start_array("srs-ConfigApDCI-Format4-r10");
        for (const auto& e1 : c.srs_cfg_ap_dci_format4_r10) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (c.srs_activ_ap_r10_present) {
        j.write_fieldname("srs-ActivateAp-r10");
        c.srs_activ_ap_r10.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_r10_c");
  }
  j.end_obj();
}
SRSASN_CODE srs_ul_cfg_ded_aperiodic_r10_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.srs_cfg_ap_dci_format4_r10_present, 1));
      HANDLE_CODE(bref.pack(c.srs_activ_ap_r10_present, 1));
      HANDLE_CODE(pack_integer(bref, c.srs_cfg_idx_ap_r10, (uint8_t)0u, (uint8_t)31u));
      if (c.srs_cfg_ap_dci_format4_r10_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.srs_cfg_ap_dci_format4_r10, 1, 3));
      }
      if (c.srs_activ_ap_r10_present) {
        HANDLE_CODE(c.srs_activ_ap_r10.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_r10_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_ul_cfg_ded_aperiodic_r10_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.srs_cfg_ap_dci_format4_r10_present, 1));
      HANDLE_CODE(bref.unpack(c.srs_activ_ap_r10_present, 1));
      HANDLE_CODE(unpack_integer(c.srs_cfg_idx_ap_r10, bref, (uint8_t)0u, (uint8_t)31u));
      if (c.srs_cfg_ap_dci_format4_r10_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.srs_cfg_ap_dci_format4_r10, bref, 1, 3));
      }
      if (c.srs_activ_ap_r10_present) {
        HANDLE_CODE(c.srs_activ_ap_r10.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_r10_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srs_ul_cfg_ded_aperiodic_r10_c::operator==(const srs_ul_cfg_ded_aperiodic_r10_c& other) const
{
  return type() == other.type() and c.srs_cfg_idx_ap_r10 == other.c.srs_cfg_idx_ap_r10 and
         c.srs_cfg_ap_dci_format4_r10_present == other.c.srs_cfg_ap_dci_format4_r10_present and
         (not c.srs_cfg_ap_dci_format4_r10_present or
          c.srs_cfg_ap_dci_format4_r10 == other.c.srs_cfg_ap_dci_format4_r10) and
         c.srs_activ_ap_r10_present == other.c.srs_activ_ap_r10_present and
         (not c.srs_activ_ap_r10_present or c.srs_activ_ap_r10 == other.c.srs_activ_ap_r10);
}

void srs_ul_cfg_ded_aperiodic_r10_c::setup_s_::srs_activ_ap_r10_c_::set(types::options e)
{
  type_ = e;
}
void srs_ul_cfg_ded_aperiodic_r10_c::setup_s_::srs_activ_ap_r10_c_::set_release()
{
  set(types::release);
}
srs_ul_cfg_ded_aperiodic_r10_c::setup_s_::srs_activ_ap_r10_c_::setup_s_&
srs_ul_cfg_ded_aperiodic_r10_c::setup_s_::srs_activ_ap_r10_c_::set_setup()
{
  set(types::setup);
  return c;
}
void srs_ul_cfg_ded_aperiodic_r10_c::setup_s_::srs_activ_ap_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("srs-ConfigApDCI-Format0-r10");
      c.srs_cfg_ap_dci_format0_r10.to_json(j);
      j.write_fieldname("srs-ConfigApDCI-Format1a2b2c-r10");
      c.srs_cfg_ap_dci_format1a2b2c_r10.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_r10_c::setup_s_::srs_activ_ap_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_ul_cfg_ded_aperiodic_r10_c::setup_s_::srs_activ_ap_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.pack(c.ext, 1);
      HANDLE_CODE(c.srs_cfg_ap_dci_format0_r10.pack(bref));
      HANDLE_CODE(c.srs_cfg_ap_dci_format1a2b2c_r10.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_r10_c::setup_s_::srs_activ_ap_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_ul_cfg_ded_aperiodic_r10_c::setup_s_::srs_activ_ap_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(c.srs_cfg_ap_dci_format0_r10.unpack(bref));
      HANDLE_CODE(c.srs_cfg_ap_dci_format1a2b2c_r10.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_r10_c::setup_s_::srs_activ_ap_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srs_ul_cfg_ded_aperiodic_r10_c::setup_s_::srs_activ_ap_r10_c_::operator==(const srs_activ_ap_r10_c_& other) const
{
  return type() == other.type() and c.ext == other.c.ext and
         c.srs_cfg_ap_dci_format0_r10 == other.c.srs_cfg_ap_dci_format0_r10 and
         c.srs_cfg_ap_dci_format1a2b2c_r10 == other.c.srs_cfg_ap_dci_format1a2b2c_r10;
}

// SoundingRS-UL-ConfigDedicatedAperiodic-v1310 ::= CHOICE
void srs_ul_cfg_ded_aperiodic_v1310_c::set(types::options e)
{
  type_ = e;
}
void srs_ul_cfg_ded_aperiodic_v1310_c::set_release()
{
  set(types::release);
}
srs_ul_cfg_ded_aperiodic_v1310_c::setup_s_& srs_ul_cfg_ded_aperiodic_v1310_c::set_setup()
{
  set(types::setup);
  return c;
}
void srs_ul_cfg_ded_aperiodic_v1310_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.srs_cfg_ap_dci_format4_v1310_present) {
        j.start_array("srs-ConfigApDCI-Format4-v1310");
        for (const auto& e1 : c.srs_cfg_ap_dci_format4_v1310) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (c.srs_activ_ap_v1310_present) {
        j.write_fieldname("srs-ActivateAp-v1310");
        c.srs_activ_ap_v1310.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_v1310_c");
  }
  j.end_obj();
}
SRSASN_CODE srs_ul_cfg_ded_aperiodic_v1310_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.srs_cfg_ap_dci_format4_v1310_present, 1));
      HANDLE_CODE(bref.pack(c.srs_activ_ap_v1310_present, 1));
      if (c.srs_cfg_ap_dci_format4_v1310_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.srs_cfg_ap_dci_format4_v1310, 1, 3));
      }
      if (c.srs_activ_ap_v1310_present) {
        HANDLE_CODE(c.srs_activ_ap_v1310.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_v1310_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_ul_cfg_ded_aperiodic_v1310_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.srs_cfg_ap_dci_format4_v1310_present, 1));
      HANDLE_CODE(bref.unpack(c.srs_activ_ap_v1310_present, 1));
      if (c.srs_cfg_ap_dci_format4_v1310_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.srs_cfg_ap_dci_format4_v1310, bref, 1, 3));
      }
      if (c.srs_activ_ap_v1310_present) {
        HANDLE_CODE(c.srs_activ_ap_v1310.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_v1310_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srs_ul_cfg_ded_aperiodic_v1310_c::operator==(const srs_ul_cfg_ded_aperiodic_v1310_c& other) const
{
  return type() == other.type() and
         c.srs_cfg_ap_dci_format4_v1310_present == other.c.srs_cfg_ap_dci_format4_v1310_present and
         (not c.srs_cfg_ap_dci_format4_v1310_present or
          c.srs_cfg_ap_dci_format4_v1310 == other.c.srs_cfg_ap_dci_format4_v1310) and
         c.srs_activ_ap_v1310_present == other.c.srs_activ_ap_v1310_present and
         (not c.srs_activ_ap_v1310_present or c.srs_activ_ap_v1310 == other.c.srs_activ_ap_v1310);
}

void srs_ul_cfg_ded_aperiodic_v1310_c::setup_s_::srs_activ_ap_v1310_c_::set(types::options e)
{
  type_ = e;
}
void srs_ul_cfg_ded_aperiodic_v1310_c::setup_s_::srs_activ_ap_v1310_c_::set_release()
{
  set(types::release);
}
srs_ul_cfg_ded_aperiodic_v1310_c::setup_s_::srs_activ_ap_v1310_c_::setup_s_&
srs_ul_cfg_ded_aperiodic_v1310_c::setup_s_::srs_activ_ap_v1310_c_::set_setup()
{
  set(types::setup);
  return c;
}
void srs_ul_cfg_ded_aperiodic_v1310_c::setup_s_::srs_activ_ap_v1310_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.srs_cfg_ap_dci_format0_v1310_present) {
        j.write_fieldname("srs-ConfigApDCI-Format0-v1310");
        c.srs_cfg_ap_dci_format0_v1310.to_json(j);
      }
      if (c.srs_cfg_ap_dci_format1a2b2c_v1310_present) {
        j.write_fieldname("srs-ConfigApDCI-Format1a2b2c-v1310");
        c.srs_cfg_ap_dci_format1a2b2c_v1310.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_v1310_c::setup_s_::srs_activ_ap_v1310_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_ul_cfg_ded_aperiodic_v1310_c::setup_s_::srs_activ_ap_v1310_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.srs_cfg_ap_dci_format0_v1310_present, 1));
      HANDLE_CODE(bref.pack(c.srs_cfg_ap_dci_format1a2b2c_v1310_present, 1));
      if (c.srs_cfg_ap_dci_format0_v1310_present) {
        HANDLE_CODE(c.srs_cfg_ap_dci_format0_v1310.pack(bref));
      }
      if (c.srs_cfg_ap_dci_format1a2b2c_v1310_present) {
        HANDLE_CODE(c.srs_cfg_ap_dci_format1a2b2c_v1310.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_v1310_c::setup_s_::srs_activ_ap_v1310_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_ul_cfg_ded_aperiodic_v1310_c::setup_s_::srs_activ_ap_v1310_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.srs_cfg_ap_dci_format0_v1310_present, 1));
      HANDLE_CODE(bref.unpack(c.srs_cfg_ap_dci_format1a2b2c_v1310_present, 1));
      if (c.srs_cfg_ap_dci_format0_v1310_present) {
        HANDLE_CODE(c.srs_cfg_ap_dci_format0_v1310.unpack(bref));
      }
      if (c.srs_cfg_ap_dci_format1a2b2c_v1310_present) {
        HANDLE_CODE(c.srs_cfg_ap_dci_format1a2b2c_v1310.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_v1310_c::setup_s_::srs_activ_ap_v1310_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srs_ul_cfg_ded_aperiodic_v1310_c::setup_s_::srs_activ_ap_v1310_c_::operator==(
    const srs_activ_ap_v1310_c_& other) const
{
  return type() == other.type() and
         c.srs_cfg_ap_dci_format0_v1310_present == other.c.srs_cfg_ap_dci_format0_v1310_present and
         (not c.srs_cfg_ap_dci_format0_v1310_present or
          c.srs_cfg_ap_dci_format0_v1310 == other.c.srs_cfg_ap_dci_format0_v1310) and
         c.srs_cfg_ap_dci_format1a2b2c_v1310_present == other.c.srs_cfg_ap_dci_format1a2b2c_v1310_present and
         (not c.srs_cfg_ap_dci_format1a2b2c_v1310_present or
          c.srs_cfg_ap_dci_format1a2b2c_v1310 == other.c.srs_cfg_ap_dci_format1a2b2c_v1310);
}

// SoundingRS-UL-ConfigDedicatedAperiodicUpPTsExt-r13 ::= CHOICE
void srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::set(types::options e)
{
  type_ = e;
}
void srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::set_release()
{
  set(types::release);
}
srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_& srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("srs-UpPtsAdd-r13", c.srs_up_pts_add_r13.to_string());
      j.write_int("srs-ConfigIndexAp-r13", c.srs_cfg_idx_ap_r13);
      if (c.srs_cfg_ap_dci_format4_r13_present) {
        j.start_array("srs-ConfigApDCI-Format4-r13");
        for (const auto& e1 : c.srs_cfg_ap_dci_format4_r13) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (c.srs_activ_ap_r13_present) {
        j.write_fieldname("srs-ActivateAp-r13");
        c.srs_activ_ap_r13.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.srs_cfg_ap_dci_format4_r13_present, 1));
      HANDLE_CODE(bref.pack(c.srs_activ_ap_r13_present, 1));
      HANDLE_CODE(c.srs_up_pts_add_r13.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.srs_cfg_idx_ap_r13, (uint8_t)0u, (uint8_t)31u));
      if (c.srs_cfg_ap_dci_format4_r13_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.srs_cfg_ap_dci_format4_r13, 1, 3));
      }
      if (c.srs_activ_ap_r13_present) {
        HANDLE_CODE(c.srs_activ_ap_r13.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.srs_cfg_ap_dci_format4_r13_present, 1));
      HANDLE_CODE(bref.unpack(c.srs_activ_ap_r13_present, 1));
      HANDLE_CODE(c.srs_up_pts_add_r13.unpack(bref));
      HANDLE_CODE(unpack_integer(c.srs_cfg_idx_ap_r13, bref, (uint8_t)0u, (uint8_t)31u));
      if (c.srs_cfg_ap_dci_format4_r13_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.srs_cfg_ap_dci_format4_r13, bref, 1, 3));
      }
      if (c.srs_activ_ap_r13_present) {
        HANDLE_CODE(c.srs_activ_ap_r13.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::operator==(const srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c& other) const
{
  return type() == other.type() and c.srs_up_pts_add_r13 == other.c.srs_up_pts_add_r13 and
         c.srs_cfg_idx_ap_r13 == other.c.srs_cfg_idx_ap_r13 and
         c.srs_cfg_ap_dci_format4_r13_present == other.c.srs_cfg_ap_dci_format4_r13_present and
         (not c.srs_cfg_ap_dci_format4_r13_present or
          c.srs_cfg_ap_dci_format4_r13 == other.c.srs_cfg_ap_dci_format4_r13) and
         c.srs_activ_ap_r13_present == other.c.srs_activ_ap_r13_present and
         (not c.srs_activ_ap_r13_present or c.srs_activ_ap_r13 == other.c.srs_activ_ap_r13);
}

const char* srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_opts::to_string() const
{
  static const char* options[] = {"sym2", "sym4"};
  return convert_enum_idx(
      options, 2, value, "srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_");
}
uint8_t srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(
      options, 2, value, "srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_");
}

void srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_activ_ap_r13_c_::set(types::options e)
{
  type_ = e;
}
void srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_activ_ap_r13_c_::set_release()
{
  set(types::release);
}
srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_activ_ap_r13_c_::setup_s_&
srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_activ_ap_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_activ_ap_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("srs-ConfigApDCI-Format0-r13");
      c.srs_cfg_ap_dci_format0_r13.to_json(j);
      j.write_fieldname("srs-ConfigApDCI-Format1a2b2c-r13");
      c.srs_cfg_ap_dci_format1a2b2c_r13.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_activ_ap_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_activ_ap_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.srs_cfg_ap_dci_format0_r13.pack(bref));
      HANDLE_CODE(c.srs_cfg_ap_dci_format1a2b2c_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_activ_ap_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_activ_ap_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.srs_cfg_ap_dci_format0_r13.unpack(bref));
      HANDLE_CODE(c.srs_cfg_ap_dci_format1a2b2c_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_activ_ap_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_activ_ap_r13_c_::operator==(
    const srs_activ_ap_r13_c_& other) const
{
  return type() == other.type() and c.srs_cfg_ap_dci_format0_r13 == other.c.srs_cfg_ap_dci_format0_r13 and
         c.srs_cfg_ap_dci_format1a2b2c_r13 == other.c.srs_cfg_ap_dci_format1a2b2c_r13;
}

// SoundingRS-UL-ConfigDedicatedUpPTsExt-r13 ::= CHOICE
void srs_ul_cfg_ded_up_pts_ext_r13_c::set(types::options e)
{
  type_ = e;
}
void srs_ul_cfg_ded_up_pts_ext_r13_c::set_release()
{
  set(types::release);
}
srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_& srs_ul_cfg_ded_up_pts_ext_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void srs_ul_cfg_ded_up_pts_ext_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("srs-UpPtsAdd-r13", c.srs_up_pts_add_r13.to_string());
      j.write_str("srs-Bandwidth-r13", c.srs_bw_r13.to_string());
      j.write_str("srs-HoppingBandwidth-r13", c.srs_hop_bw_r13.to_string());
      j.write_int("freqDomainPosition-r13", c.freq_domain_position_r13);
      j.write_bool("duration-r13", c.dur_r13);
      j.write_int("srs-ConfigIndex-r13", c.srs_cfg_idx_r13);
      j.write_int("transmissionComb-r13", c.tx_comb_r13);
      j.write_str("cyclicShift-r13", c.cyclic_shift_r13.to_string());
      j.write_str("srs-AntennaPort-r13", c.srs_ant_port_r13.to_string());
      j.write_str("transmissionCombNum-r13", c.tx_comb_num_r13.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_up_pts_ext_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE srs_ul_cfg_ded_up_pts_ext_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.srs_up_pts_add_r13.pack(bref));
      HANDLE_CODE(c.srs_bw_r13.pack(bref));
      HANDLE_CODE(c.srs_hop_bw_r13.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.freq_domain_position_r13, (uint8_t)0u, (uint8_t)23u));
      HANDLE_CODE(bref.pack(c.dur_r13, 1));
      HANDLE_CODE(pack_integer(bref, c.srs_cfg_idx_r13, (uint16_t)0u, (uint16_t)1023u));
      HANDLE_CODE(pack_integer(bref, c.tx_comb_r13, (uint8_t)0u, (uint8_t)3u));
      HANDLE_CODE(c.cyclic_shift_r13.pack(bref));
      HANDLE_CODE(c.srs_ant_port_r13.pack(bref));
      HANDLE_CODE(c.tx_comb_num_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_up_pts_ext_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_ul_cfg_ded_up_pts_ext_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.srs_up_pts_add_r13.unpack(bref));
      HANDLE_CODE(c.srs_bw_r13.unpack(bref));
      HANDLE_CODE(c.srs_hop_bw_r13.unpack(bref));
      HANDLE_CODE(unpack_integer(c.freq_domain_position_r13, bref, (uint8_t)0u, (uint8_t)23u));
      HANDLE_CODE(bref.unpack(c.dur_r13, 1));
      HANDLE_CODE(unpack_integer(c.srs_cfg_idx_r13, bref, (uint16_t)0u, (uint16_t)1023u));
      HANDLE_CODE(unpack_integer(c.tx_comb_r13, bref, (uint8_t)0u, (uint8_t)3u));
      HANDLE_CODE(c.cyclic_shift_r13.unpack(bref));
      HANDLE_CODE(c.srs_ant_port_r13.unpack(bref));
      HANDLE_CODE(c.tx_comb_num_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_up_pts_ext_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srs_ul_cfg_ded_up_pts_ext_r13_c::operator==(const srs_ul_cfg_ded_up_pts_ext_r13_c& other) const
{
  return type() == other.type() and c.srs_up_pts_add_r13 == other.c.srs_up_pts_add_r13 and
         c.srs_bw_r13 == other.c.srs_bw_r13 and c.srs_hop_bw_r13 == other.c.srs_hop_bw_r13 and
         c.freq_domain_position_r13 == other.c.freq_domain_position_r13 and c.dur_r13 == other.c.dur_r13 and
         c.srs_cfg_idx_r13 == other.c.srs_cfg_idx_r13 and c.tx_comb_r13 == other.c.tx_comb_r13 and
         c.cyclic_shift_r13 == other.c.cyclic_shift_r13 and c.srs_ant_port_r13 == other.c.srs_ant_port_r13 and
         c.tx_comb_num_r13 == other.c.tx_comb_num_r13;
}

const char* srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_opts::to_string() const
{
  static const char* options[] = {"sym2", "sym4"};
  return convert_enum_idx(options, 2, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_");
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_");
}

const char* srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_bw_r13_opts::to_string() const
{
  static const char* options[] = {"bw0", "bw1", "bw2", "bw3"};
  return convert_enum_idx(options, 4, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_bw_r13_e_");
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_bw_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_bw_r13_e_");
}

const char* srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_hop_bw_r13_opts::to_string() const
{
  static const char* options[] = {"hbw0", "hbw1", "hbw2", "hbw3"};
  return convert_enum_idx(options, 4, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_hop_bw_r13_e_");
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_hop_bw_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_hop_bw_r13_e_");
}

const char* srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::cyclic_shift_r13_opts::to_string() const
{
  static const char* options[] = {"cs0", "cs1", "cs2", "cs3", "cs4", "cs5", "cs6", "cs7", "cs8", "cs9", "cs10", "cs11"};
  return convert_enum_idx(options, 12, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::cyclic_shift_r13_e_");
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::cyclic_shift_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  return map_enum_number(options, 12, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::cyclic_shift_r13_e_");
}

const char* srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::tx_comb_num_r13_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::tx_comb_num_r13_e_");
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::tx_comb_num_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::tx_comb_num_r13_e_");
}

// SoundingRS-VirtualCellID-r16 ::= SEQUENCE
SRSASN_CODE srs_virtual_cell_id_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, srs_virtual_cell_id_r16, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(bref.pack(srs_virtual_cell_id_all_srs_r16, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_virtual_cell_id_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(srs_virtual_cell_id_r16, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(bref.unpack(srs_virtual_cell_id_all_srs_r16, 1));

  return SRSASN_SUCCESS;
}
void srs_virtual_cell_id_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("srs-VirtualCellID-r16", srs_virtual_cell_id_r16);
  j.write_bool("srs-VirtualCellID-AllSRS-r16", srs_virtual_cell_id_all_srs_r16);
  j.end_obj();
}
bool srs_virtual_cell_id_r16_s::operator==(const srs_virtual_cell_id_r16_s& other) const
{
  return srs_virtual_cell_id_r16 == other.srs_virtual_cell_id_r16 and
         srs_virtual_cell_id_all_srs_r16 == other.srs_virtual_cell_id_all_srs_r16;
}

// UplinkPowerControlAddSRS-r16 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_add_srs_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tpc_idx_srs_add_r16_present, 1));
  HANDLE_CODE(bref.pack(start_bit_of_format3_b_srs_add_r16_present, 1));
  HANDLE_CODE(bref.pack(field_type_format3_b_srs_add_r16_present, 1));
  HANDLE_CODE(bref.pack(p0_ue_srs_add_r16_present, 1));

  if (tpc_idx_srs_add_r16_present) {
    HANDLE_CODE(tpc_idx_srs_add_r16.pack(bref));
  }
  if (start_bit_of_format3_b_srs_add_r16_present) {
    HANDLE_CODE(pack_integer(bref, start_bit_of_format3_b_srs_add_r16, (uint8_t)0u, (uint8_t)31u));
  }
  if (field_type_format3_b_srs_add_r16_present) {
    HANDLE_CODE(pack_integer(bref, field_type_format3_b_srs_add_r16, (uint8_t)1u, (uint8_t)2u));
  }
  if (p0_ue_srs_add_r16_present) {
    HANDLE_CODE(pack_integer(bref, p0_ue_srs_add_r16, (int8_t)-16, (int8_t)15));
  }
  HANDLE_CODE(bref.pack(accumulation_enabled_srs_add_r16, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_add_srs_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tpc_idx_srs_add_r16_present, 1));
  HANDLE_CODE(bref.unpack(start_bit_of_format3_b_srs_add_r16_present, 1));
  HANDLE_CODE(bref.unpack(field_type_format3_b_srs_add_r16_present, 1));
  HANDLE_CODE(bref.unpack(p0_ue_srs_add_r16_present, 1));

  if (tpc_idx_srs_add_r16_present) {
    HANDLE_CODE(tpc_idx_srs_add_r16.unpack(bref));
  }
  if (start_bit_of_format3_b_srs_add_r16_present) {
    HANDLE_CODE(unpack_integer(start_bit_of_format3_b_srs_add_r16, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (field_type_format3_b_srs_add_r16_present) {
    HANDLE_CODE(unpack_integer(field_type_format3_b_srs_add_r16, bref, (uint8_t)1u, (uint8_t)2u));
  }
  if (p0_ue_srs_add_r16_present) {
    HANDLE_CODE(unpack_integer(p0_ue_srs_add_r16, bref, (int8_t)-16, (int8_t)15));
  }
  HANDLE_CODE(bref.unpack(accumulation_enabled_srs_add_r16, 1));

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_add_srs_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tpc_idx_srs_add_r16_present) {
    j.write_fieldname("tpc-IndexSRS-Add-r16");
    tpc_idx_srs_add_r16.to_json(j);
  }
  if (start_bit_of_format3_b_srs_add_r16_present) {
    j.write_int("startingBitOfFormat3B-SRS-Add-r16", start_bit_of_format3_b_srs_add_r16);
  }
  if (field_type_format3_b_srs_add_r16_present) {
    j.write_int("fieldTypeFormat3B-SRS-Add-r16", field_type_format3_b_srs_add_r16);
  }
  if (p0_ue_srs_add_r16_present) {
    j.write_int("p0-UE-SRS-Add-r16", p0_ue_srs_add_r16);
  }
  j.write_bool("accumulationEnabledSRS-Add-r16", accumulation_enabled_srs_add_r16);
  j.end_obj();
}
bool ul_pwr_ctrl_add_srs_r16_s::operator==(const ul_pwr_ctrl_add_srs_r16_s& other) const
{
  return tpc_idx_srs_add_r16_present == other.tpc_idx_srs_add_r16_present and
         (not tpc_idx_srs_add_r16_present or tpc_idx_srs_add_r16 == other.tpc_idx_srs_add_r16) and
         start_bit_of_format3_b_srs_add_r16_present == other.start_bit_of_format3_b_srs_add_r16_present and
         (not start_bit_of_format3_b_srs_add_r16_present or
          start_bit_of_format3_b_srs_add_r16 == other.start_bit_of_format3_b_srs_add_r16) and
         field_type_format3_b_srs_add_r16_present == other.field_type_format3_b_srs_add_r16_present and
         (not field_type_format3_b_srs_add_r16_present or
          field_type_format3_b_srs_add_r16 == other.field_type_format3_b_srs_add_r16) and
         p0_ue_srs_add_r16_present == other.p0_ue_srs_add_r16_present and
         (not p0_ue_srs_add_r16_present or p0_ue_srs_add_r16 == other.p0_ue_srs_add_r16) and
         accumulation_enabled_srs_add_r16 == other.accumulation_enabled_srs_add_r16;
}

// UplinkPowerControlDedicated ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_ded_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(filt_coef_present, 1));

  HANDLE_CODE(pack_integer(bref, p0_ue_pusch, (int8_t)-8, (int8_t)7));
  HANDLE_CODE(delta_mcs_enabled.pack(bref));
  HANDLE_CODE(bref.pack(accumulation_enabled, 1));
  HANDLE_CODE(pack_integer(bref, p0_ue_pucch, (int8_t)-8, (int8_t)7));
  HANDLE_CODE(pack_integer(bref, psrs_offset, (uint8_t)0u, (uint8_t)15u));
  if (filt_coef_present) {
    HANDLE_CODE(filt_coef.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_ded_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(filt_coef_present, 1));

  HANDLE_CODE(unpack_integer(p0_ue_pusch, bref, (int8_t)-8, (int8_t)7));
  HANDLE_CODE(delta_mcs_enabled.unpack(bref));
  HANDLE_CODE(bref.unpack(accumulation_enabled, 1));
  HANDLE_CODE(unpack_integer(p0_ue_pucch, bref, (int8_t)-8, (int8_t)7));
  HANDLE_CODE(unpack_integer(psrs_offset, bref, (uint8_t)0u, (uint8_t)15u));
  if (filt_coef_present) {
    HANDLE_CODE(filt_coef.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_ded_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p0-UE-PUSCH", p0_ue_pusch);
  j.write_str("deltaMCS-Enabled", delta_mcs_enabled.to_string());
  j.write_bool("accumulationEnabled", accumulation_enabled);
  j.write_int("p0-UE-PUCCH", p0_ue_pucch);
  j.write_int("pSRS-Offset", psrs_offset);
  if (filt_coef_present) {
    j.write_str("filterCoefficient", filt_coef.to_string());
  }
  j.end_obj();
}

const char* ul_pwr_ctrl_ded_s::delta_mcs_enabled_opts::to_string() const
{
  static const char* options[] = {"en0", "en1"};
  return convert_enum_idx(options, 2, value, "ul_pwr_ctrl_ded_s::delta_mcs_enabled_e_");
}
uint8_t ul_pwr_ctrl_ded_s::delta_mcs_enabled_opts::to_number() const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(options, 2, value, "ul_pwr_ctrl_ded_s::delta_mcs_enabled_e_");
}

// UplinkPowerControlDedicated-v1020 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_ded_v1020_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(delta_tx_d_offset_list_pucch_r10_present, 1));
  HANDLE_CODE(bref.pack(psrs_offset_ap_r10_present, 1));

  if (delta_tx_d_offset_list_pucch_r10_present) {
    HANDLE_CODE(delta_tx_d_offset_list_pucch_r10.pack(bref));
  }
  if (psrs_offset_ap_r10_present) {
    HANDLE_CODE(pack_integer(bref, psrs_offset_ap_r10, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_ded_v1020_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(delta_tx_d_offset_list_pucch_r10_present, 1));
  HANDLE_CODE(bref.unpack(psrs_offset_ap_r10_present, 1));

  if (delta_tx_d_offset_list_pucch_r10_present) {
    HANDLE_CODE(delta_tx_d_offset_list_pucch_r10.unpack(bref));
  }
  if (psrs_offset_ap_r10_present) {
    HANDLE_CODE(unpack_integer(psrs_offset_ap_r10, bref, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_ded_v1020_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (delta_tx_d_offset_list_pucch_r10_present) {
    j.write_fieldname("deltaTxD-OffsetListPUCCH-r10");
    delta_tx_d_offset_list_pucch_r10.to_json(j);
  }
  if (psrs_offset_ap_r10_present) {
    j.write_int("pSRS-OffsetAp-r10", psrs_offset_ap_r10);
  }
  j.end_obj();
}

// UplinkPowerControlDedicated-v1130 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_ded_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(psrs_offset_v1130_present, 1));
  HANDLE_CODE(bref.pack(psrs_offset_ap_v1130_present, 1));
  HANDLE_CODE(bref.pack(delta_tx_d_offset_list_pucch_v1130_present, 1));

  if (psrs_offset_v1130_present) {
    HANDLE_CODE(pack_integer(bref, psrs_offset_v1130, (uint8_t)16u, (uint8_t)31u));
  }
  if (psrs_offset_ap_v1130_present) {
    HANDLE_CODE(pack_integer(bref, psrs_offset_ap_v1130, (uint8_t)16u, (uint8_t)31u));
  }
  if (delta_tx_d_offset_list_pucch_v1130_present) {
    HANDLE_CODE(delta_tx_d_offset_list_pucch_v1130.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_ded_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(psrs_offset_v1130_present, 1));
  HANDLE_CODE(bref.unpack(psrs_offset_ap_v1130_present, 1));
  HANDLE_CODE(bref.unpack(delta_tx_d_offset_list_pucch_v1130_present, 1));

  if (psrs_offset_v1130_present) {
    HANDLE_CODE(unpack_integer(psrs_offset_v1130, bref, (uint8_t)16u, (uint8_t)31u));
  }
  if (psrs_offset_ap_v1130_present) {
    HANDLE_CODE(unpack_integer(psrs_offset_ap_v1130, bref, (uint8_t)16u, (uint8_t)31u));
  }
  if (delta_tx_d_offset_list_pucch_v1130_present) {
    HANDLE_CODE(delta_tx_d_offset_list_pucch_v1130.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_ded_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (psrs_offset_v1130_present) {
    j.write_int("pSRS-Offset-v1130", psrs_offset_v1130);
  }
  if (psrs_offset_ap_v1130_present) {
    j.write_int("pSRS-OffsetAp-v1130", psrs_offset_ap_v1130);
  }
  if (delta_tx_d_offset_list_pucch_v1130_present) {
    j.write_fieldname("deltaTxD-OffsetListPUCCH-v1130");
    delta_tx_d_offset_list_pucch_v1130.to_json(j);
  }
  j.end_obj();
}
bool ul_pwr_ctrl_ded_v1130_s::operator==(const ul_pwr_ctrl_ded_v1130_s& other) const
{
  return psrs_offset_v1130_present == other.psrs_offset_v1130_present and
         (not psrs_offset_v1130_present or psrs_offset_v1130 == other.psrs_offset_v1130) and
         psrs_offset_ap_v1130_present == other.psrs_offset_ap_v1130_present and
         (not psrs_offset_ap_v1130_present or psrs_offset_ap_v1130 == other.psrs_offset_ap_v1130) and
         delta_tx_d_offset_list_pucch_v1130_present == other.delta_tx_d_offset_list_pucch_v1130_present and
         (not delta_tx_d_offset_list_pucch_v1130_present or
          delta_tx_d_offset_list_pucch_v1130 == other.delta_tx_d_offset_list_pucch_v1130);
}

// UplinkPowerControlDedicated-v1250 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_ded_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(set2_pwr_ctrl_param.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_ded_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(set2_pwr_ctrl_param.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_ded_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("set2PowerControlParameter");
  set2_pwr_ctrl_param.to_json(j);
  j.end_obj();
}
bool ul_pwr_ctrl_ded_v1250_s::operator==(const ul_pwr_ctrl_ded_v1250_s& other) const
{
  return set2_pwr_ctrl_param == other.set2_pwr_ctrl_param;
}

void ul_pwr_ctrl_ded_v1250_s::set2_pwr_ctrl_param_c_::set(types::options e)
{
  type_ = e;
}
void ul_pwr_ctrl_ded_v1250_s::set2_pwr_ctrl_param_c_::set_release()
{
  set(types::release);
}
ul_pwr_ctrl_ded_v1250_s::set2_pwr_ctrl_param_c_::setup_s_& ul_pwr_ctrl_ded_v1250_s::set2_pwr_ctrl_param_c_::set_setup()
{
  set(types::setup);
  return c;
}
void ul_pwr_ctrl_ded_v1250_s::set2_pwr_ctrl_param_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("tpc-SubframeSet-r12", c.tpc_sf_set_r12.to_string());
      j.write_int("p0-NominalPUSCH-SubframeSet2-r12", c.p0_nominal_pusch_sf_set2_r12);
      j.write_str("alpha-SubframeSet2-r12", c.alpha_sf_set2_r12.to_string());
      j.write_int("p0-UE-PUSCH-SubframeSet2-r12", c.p0_ue_pusch_sf_set2_r12);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "ul_pwr_ctrl_ded_v1250_s::set2_pwr_ctrl_param_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_pwr_ctrl_ded_v1250_s::set2_pwr_ctrl_param_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.tpc_sf_set_r12.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.p0_nominal_pusch_sf_set2_r12, (int8_t)-126, (int8_t)24));
      HANDLE_CODE(c.alpha_sf_set2_r12.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.p0_ue_pusch_sf_set2_r12, (int8_t)-8, (int8_t)7));
      break;
    default:
      log_invalid_choice_id(type_, "ul_pwr_ctrl_ded_v1250_s::set2_pwr_ctrl_param_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_ded_v1250_s::set2_pwr_ctrl_param_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.tpc_sf_set_r12.unpack(bref));
      HANDLE_CODE(unpack_integer(c.p0_nominal_pusch_sf_set2_r12, bref, (int8_t)-126, (int8_t)24));
      HANDLE_CODE(c.alpha_sf_set2_r12.unpack(bref));
      HANDLE_CODE(unpack_integer(c.p0_ue_pusch_sf_set2_r12, bref, (int8_t)-8, (int8_t)7));
      break;
    default:
      log_invalid_choice_id(type_, "ul_pwr_ctrl_ded_v1250_s::set2_pwr_ctrl_param_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool ul_pwr_ctrl_ded_v1250_s::set2_pwr_ctrl_param_c_::operator==(const set2_pwr_ctrl_param_c_& other) const
{
  return type() == other.type() and c.tpc_sf_set_r12 == other.c.tpc_sf_set_r12 and
         c.p0_nominal_pusch_sf_set2_r12 == other.c.p0_nominal_pusch_sf_set2_r12 and
         c.alpha_sf_set2_r12 == other.c.alpha_sf_set2_r12 and
         c.p0_ue_pusch_sf_set2_r12 == other.c.p0_ue_pusch_sf_set2_r12;
}

// UplinkPowerControlDedicated-v1530 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_ded_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(alpha_ue_r15_present, 1));
  HANDLE_CODE(bref.pack(p0_ue_pusch_r15_present, 1));

  if (alpha_ue_r15_present) {
    HANDLE_CODE(alpha_ue_r15.pack(bref));
  }
  if (p0_ue_pusch_r15_present) {
    HANDLE_CODE(pack_integer(bref, p0_ue_pusch_r15, (int8_t)-16, (int8_t)15));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_ded_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(alpha_ue_r15_present, 1));
  HANDLE_CODE(bref.unpack(p0_ue_pusch_r15_present, 1));

  if (alpha_ue_r15_present) {
    HANDLE_CODE(alpha_ue_r15.unpack(bref));
  }
  if (p0_ue_pusch_r15_present) {
    HANDLE_CODE(unpack_integer(p0_ue_pusch_r15, bref, (int8_t)-16, (int8_t)15));
  }

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_ded_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (alpha_ue_r15_present) {
    j.write_str("alpha-UE-r15", alpha_ue_r15.to_string());
  }
  if (p0_ue_pusch_r15_present) {
    j.write_int("p0-UE-PUSCH-r15", p0_ue_pusch_r15);
  }
  j.end_obj();
}
bool ul_pwr_ctrl_ded_v1530_s::operator==(const ul_pwr_ctrl_ded_v1530_s& other) const
{
  return alpha_ue_r15_present == other.alpha_ue_r15_present and
         (not alpha_ue_r15_present or alpha_ue_r15 == other.alpha_ue_r15) and
         p0_ue_pusch_r15_present == other.p0_ue_pusch_r15_present and
         (not p0_ue_pusch_r15_present or p0_ue_pusch_r15 == other.p0_ue_pusch_r15);
}

// WidebandPRG-r16 ::= SEQUENCE
SRSASN_CODE wideband_prg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(wideband_prg_sf_r16, 1));
  HANDLE_CODE(bref.pack(wideband_prg_slot_subslot_r16, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE wideband_prg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(wideband_prg_sf_r16, 1));
  HANDLE_CODE(bref.unpack(wideband_prg_slot_subslot_r16, 1));

  return SRSASN_SUCCESS;
}
void wideband_prg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("widebandPRG-Subframe-r16", wideband_prg_sf_r16);
  j.write_bool("widebandPRG-SlotSubslot-r16", wideband_prg_slot_subslot_r16);
  j.end_obj();
}
bool wideband_prg_r16_s::operator==(const wideband_prg_r16_s& other) const
{
  return wideband_prg_sf_r16 == other.wideband_prg_sf_r16 and
         wideband_prg_slot_subslot_r16 == other.wideband_prg_slot_subslot_r16;
}

// PhysicalConfigDedicated ::= SEQUENCE
SRSASN_CODE phys_cfg_ded_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pdsch_cfg_ded_present, 1));
  HANDLE_CODE(bref.pack(pucch_cfg_ded_present, 1));
  HANDLE_CODE(bref.pack(pusch_cfg_ded_present, 1));
  HANDLE_CODE(bref.pack(ul_pwr_ctrl_ded_present, 1));
  HANDLE_CODE(bref.pack(tpc_pdcch_cfg_pucch_present, 1));
  HANDLE_CODE(bref.pack(tpc_pdcch_cfg_pusch_present, 1));
  HANDLE_CODE(bref.pack(cqi_report_cfg_present, 1));
  HANDLE_CODE(bref.pack(srs_ul_cfg_ded_present, 1));
  HANDLE_CODE(bref.pack(ant_info_present, 1));
  HANDLE_CODE(bref.pack(sched_request_cfg_present, 1));

  if (pdsch_cfg_ded_present) {
    HANDLE_CODE(pdsch_cfg_ded.pack(bref));
  }
  if (pucch_cfg_ded_present) {
    HANDLE_CODE(pucch_cfg_ded.pack(bref));
  }
  if (pusch_cfg_ded_present) {
    HANDLE_CODE(pusch_cfg_ded.pack(bref));
  }
  if (ul_pwr_ctrl_ded_present) {
    HANDLE_CODE(ul_pwr_ctrl_ded.pack(bref));
  }
  if (tpc_pdcch_cfg_pucch_present) {
    HANDLE_CODE(tpc_pdcch_cfg_pucch.pack(bref));
  }
  if (tpc_pdcch_cfg_pusch_present) {
    HANDLE_CODE(tpc_pdcch_cfg_pusch.pack(bref));
  }
  if (cqi_report_cfg_present) {
    HANDLE_CODE(cqi_report_cfg.pack(bref));
  }
  if (srs_ul_cfg_ded_present) {
    HANDLE_CODE(srs_ul_cfg_ded.pack(bref));
  }
  if (ant_info_present) {
    HANDLE_CODE(ant_info.pack(bref));
  }
  if (sched_request_cfg_present) {
    HANDLE_CODE(sched_request_cfg.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= cqi_report_cfg_v920.is_present();
    group_flags[0] |= ant_info_v920.is_present();
    group_flags[1] |= ant_info_r10.is_present();
    group_flags[1] |= ant_info_ul_r10.is_present();
    group_flags[1] |= cif_presence_r10_present;
    group_flags[1] |= cqi_report_cfg_r10.is_present();
    group_flags[1] |= csi_rs_cfg_r10.is_present();
    group_flags[1] |= pucch_cfg_ded_v1020.is_present();
    group_flags[1] |= pusch_cfg_ded_v1020.is_present();
    group_flags[1] |= sched_request_cfg_v1020.is_present();
    group_flags[1] |= srs_ul_cfg_ded_v1020.is_present();
    group_flags[1] |= srs_ul_cfg_ded_aperiodic_r10.is_present();
    group_flags[1] |= ul_pwr_ctrl_ded_v1020.is_present();
    group_flags[2] |= add_spec_emission_ca_r10.is_present();
    group_flags[3] |= csi_rs_cfg_nzp_to_release_list_r11.is_present();
    group_flags[3] |= csi_rs_cfg_nzp_to_add_mod_list_r11.is_present();
    group_flags[3] |= csi_rs_cfg_zp_to_release_list_r11.is_present();
    group_flags[3] |= csi_rs_cfg_zp_to_add_mod_list_r11.is_present();
    group_flags[3] |= epdcch_cfg_r11.is_present();
    group_flags[3] |= pdsch_cfg_ded_v1130.is_present();
    group_flags[3] |= cqi_report_cfg_v1130.is_present();
    group_flags[3] |= pucch_cfg_ded_v1130.is_present();
    group_flags[3] |= pusch_cfg_ded_v1130.is_present();
    group_flags[3] |= ul_pwr_ctrl_ded_v1130.is_present();
    group_flags[4] |= ant_info_v1250.is_present();
    group_flags[4] |= eimta_main_cfg_r12.is_present();
    group_flags[4] |= eimta_main_cfg_pcell_r12.is_present();
    group_flags[4] |= pucch_cfg_ded_v1250.is_present();
    group_flags[4] |= cqi_report_cfg_pcell_v1250.is_present();
    group_flags[4] |= ul_pwr_ctrl_ded_v1250.is_present();
    group_flags[4] |= pusch_cfg_ded_v1250.is_present();
    group_flags[4] |= csi_rs_cfg_v1250.is_present();
    group_flags[5] |= pdsch_cfg_ded_v1280.is_present();
    group_flags[6] |= pdsch_cfg_ded_v1310.is_present();
    group_flags[6] |= pucch_cfg_ded_r13.is_present();
    group_flags[6] |= pusch_cfg_ded_r13.is_present();
    group_flags[6] |= pdcch_candidate_reductions_r13.is_present();
    group_flags[6] |= cqi_report_cfg_v1310.is_present();
    group_flags[6] |= srs_ul_cfg_ded_v1310.is_present();
    group_flags[6] |= srs_ul_cfg_ded_up_pts_ext_r13.is_present();
    group_flags[6] |= srs_ul_cfg_ded_aperiodic_v1310.is_present();
    group_flags[6] |= srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.is_present();
    group_flags[6] |= csi_rs_cfg_v1310.is_present();
    group_flags[6] |= ce_mode_r13.is_present();
    group_flags[6] |= csi_rs_cfg_nzp_to_add_mod_list_ext_r13.is_present();
    group_flags[6] |= csi_rs_cfg_nzp_to_release_list_ext_r13.is_present();
    group_flags[7] |= cqi_report_cfg_v1320.is_present();
    group_flags[8] |= type_a_srs_tpc_pdcch_group_r14.is_present();
    group_flags[8] |= must_cfg_r14.is_present();
    group_flags[8] |= pusch_enhance_cfg_r14.is_present();
    group_flags[8] |= ce_pdsch_pusch_enhancement_cfg_r14_present;
    group_flags[8] |= ant_info_v1430.is_present();
    group_flags[8] |= pucch_cfg_ded_v1430.is_present();
    group_flags[8] |= pdsch_cfg_ded_v1430.is_present();
    group_flags[8] |= pusch_cfg_ded_v1430.is_present();
    group_flags[8] |= srs_ul_periodic_cfg_ded_list_r14.is_present();
    group_flags[8] |= srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.is_present();
    group_flags[8] |= srs_ul_aperiodic_cfg_ded_list_r14.is_present();
    group_flags[8] |= srs_ul_cfg_ded_ap_up_pts_ext_list_r14.is_present();
    group_flags[8] |= csi_rs_cfg_v1430.is_present();
    group_flags[8] |= csi_rs_cfg_zp_ap_list_r14.is_present();
    group_flags[8] |= cqi_report_cfg_v1430.is_present();
    group_flags[8] |= semi_open_loop_r14_present;
    group_flags[9] |= csi_rs_cfg_v1480.is_present();
    group_flags[10] |= phys_cfg_ded_stti_r15.is_present();
    group_flags[10] |= pdsch_cfg_ded_v1530.is_present();
    group_flags[10] |= pusch_cfg_ded_v1530.is_present();
    group_flags[10] |= cqi_report_cfg_v1530.is_present();
    group_flags[10] |= ant_info_v1530.is_present();
    group_flags[10] |= csi_rs_cfg_v1530.is_present();
    group_flags[10] |= ul_pwr_ctrl_ded_v1530.is_present();
    group_flags[10] |= semi_static_cfi_cfg_r15.is_present();
    group_flags[10] |= blind_pdsch_repeat_cfg_r15.is_present();
    group_flags[11] |= spucch_cfg_v1550.is_present();
    group_flags[12] |= pdsch_cfg_ded_v1610.is_present();
    group_flags[12] |= pusch_cfg_ded_v1610.is_present();
    group_flags[12] |= ce_csi_rs_feedback_r16_present;
    group_flags[12] |= res_reserv_cfg_ded_dl_r16.is_present();
    group_flags[12] |= res_reserv_cfg_ded_ul_r16.is_present();
    group_flags[12] |= srs_ul_cfg_ded_add_r16.is_present();
    group_flags[12] |= ul_pwr_ctrl_add_srs_r16.is_present();
    group_flags[12] |= srs_virtual_cell_id_r16.is_present();
    group_flags[12] |= wideband_prg_r16.is_present();
    group_flags[13] |= pdsch_cfg_ded_v1700.is_present();
    group_flags[13] |= ntn_cfg_ded_r17.is_present();
    group_flags[14] |= ul_segmented_precompensation_gap_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cqi_report_cfg_v920.is_present(), 1));
      HANDLE_CODE(bref.pack(ant_info_v920.is_present(), 1));
      if (cqi_report_cfg_v920.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v920->pack(bref));
      }
      if (ant_info_v920.is_present()) {
        HANDLE_CODE(ant_info_v920->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ant_info_r10.is_present(), 1));
      HANDLE_CODE(bref.pack(ant_info_ul_r10.is_present(), 1));
      HANDLE_CODE(bref.pack(cif_presence_r10_present, 1));
      HANDLE_CODE(bref.pack(cqi_report_cfg_r10.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_r10.is_present(), 1));
      HANDLE_CODE(bref.pack(pucch_cfg_ded_v1020.is_present(), 1));
      HANDLE_CODE(bref.pack(pusch_cfg_ded_v1020.is_present(), 1));
      HANDLE_CODE(bref.pack(sched_request_cfg_v1020.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_cfg_ded_v1020.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_cfg_ded_aperiodic_r10.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pwr_ctrl_ded_v1020.is_present(), 1));
      if (ant_info_r10.is_present()) {
        HANDLE_CODE(ant_info_r10->pack(bref));
      }
      if (ant_info_ul_r10.is_present()) {
        HANDLE_CODE(ant_info_ul_r10->pack(bref));
      }
      if (cif_presence_r10_present) {
        HANDLE_CODE(bref.pack(cif_presence_r10, 1));
      }
      if (cqi_report_cfg_r10.is_present()) {
        HANDLE_CODE(cqi_report_cfg_r10->pack(bref));
      }
      if (csi_rs_cfg_r10.is_present()) {
        HANDLE_CODE(csi_rs_cfg_r10->pack(bref));
      }
      if (pucch_cfg_ded_v1020.is_present()) {
        HANDLE_CODE(pucch_cfg_ded_v1020->pack(bref));
      }
      if (pusch_cfg_ded_v1020.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1020->pack(bref));
      }
      if (sched_request_cfg_v1020.is_present()) {
        HANDLE_CODE(sched_request_cfg_v1020->pack(bref));
      }
      if (srs_ul_cfg_ded_v1020.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_v1020->pack(bref));
      }
      if (srs_ul_cfg_ded_aperiodic_r10.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_aperiodic_r10->pack(bref));
      }
      if (ul_pwr_ctrl_ded_v1020.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_ded_v1020->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(add_spec_emission_ca_r10.is_present(), 1));
      if (add_spec_emission_ca_r10.is_present()) {
        HANDLE_CODE(add_spec_emission_ca_r10->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_to_release_list_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_to_add_mod_list_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_zp_to_release_list_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_zp_to_add_mod_list_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(epdcch_cfg_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(pdsch_cfg_ded_v1130.is_present(), 1));
      HANDLE_CODE(bref.pack(cqi_report_cfg_v1130.is_present(), 1));
      HANDLE_CODE(bref.pack(pucch_cfg_ded_v1130.is_present(), 1));
      HANDLE_CODE(bref.pack(pusch_cfg_ded_v1130.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pwr_ctrl_ded_v1130.is_present(), 1));
      if (csi_rs_cfg_nzp_to_release_list_r11.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *csi_rs_cfg_nzp_to_release_list_r11, 1, 3, integer_packer<uint8_t>(1, 3)));
      }
      if (csi_rs_cfg_nzp_to_add_mod_list_r11.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *csi_rs_cfg_nzp_to_add_mod_list_r11, 1, 3));
      }
      if (csi_rs_cfg_zp_to_release_list_r11.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *csi_rs_cfg_zp_to_release_list_r11, 1, 4, integer_packer<uint8_t>(1, 4)));
      }
      if (csi_rs_cfg_zp_to_add_mod_list_r11.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *csi_rs_cfg_zp_to_add_mod_list_r11, 1, 4));
      }
      if (epdcch_cfg_r11.is_present()) {
        HANDLE_CODE(epdcch_cfg_r11->pack(bref));
      }
      if (pdsch_cfg_ded_v1130.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1130->pack(bref));
      }
      if (cqi_report_cfg_v1130.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1130->pack(bref));
      }
      if (pucch_cfg_ded_v1130.is_present()) {
        HANDLE_CODE(pucch_cfg_ded_v1130->pack(bref));
      }
      if (pusch_cfg_ded_v1130.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1130->pack(bref));
      }
      if (ul_pwr_ctrl_ded_v1130.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_ded_v1130->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ant_info_v1250.is_present(), 1));
      HANDLE_CODE(bref.pack(eimta_main_cfg_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(eimta_main_cfg_pcell_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(pucch_cfg_ded_v1250.is_present(), 1));
      HANDLE_CODE(bref.pack(cqi_report_cfg_pcell_v1250.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pwr_ctrl_ded_v1250.is_present(), 1));
      HANDLE_CODE(bref.pack(pusch_cfg_ded_v1250.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_v1250.is_present(), 1));
      if (ant_info_v1250.is_present()) {
        HANDLE_CODE(ant_info_v1250->pack(bref));
      }
      if (eimta_main_cfg_r12.is_present()) {
        HANDLE_CODE(eimta_main_cfg_r12->pack(bref));
      }
      if (eimta_main_cfg_pcell_r12.is_present()) {
        HANDLE_CODE(eimta_main_cfg_pcell_r12->pack(bref));
      }
      if (pucch_cfg_ded_v1250.is_present()) {
        HANDLE_CODE(pucch_cfg_ded_v1250->pack(bref));
      }
      if (cqi_report_cfg_pcell_v1250.is_present()) {
        HANDLE_CODE(cqi_report_cfg_pcell_v1250->pack(bref));
      }
      if (ul_pwr_ctrl_ded_v1250.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_ded_v1250->pack(bref));
      }
      if (pusch_cfg_ded_v1250.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1250->pack(bref));
      }
      if (csi_rs_cfg_v1250.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1250->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pdsch_cfg_ded_v1280.is_present(), 1));
      if (pdsch_cfg_ded_v1280.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1280->pack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pdsch_cfg_ded_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(pucch_cfg_ded_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(pusch_cfg_ded_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(pdcch_candidate_reductions_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(cqi_report_cfg_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_cfg_ded_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_cfg_ded_up_pts_ext_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_cfg_ded_aperiodic_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(ce_mode_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_to_add_mod_list_ext_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_to_release_list_ext_r13.is_present(), 1));
      if (pdsch_cfg_ded_v1310.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1310->pack(bref));
      }
      if (pucch_cfg_ded_r13.is_present()) {
        HANDLE_CODE(pucch_cfg_ded_r13->pack(bref));
      }
      if (pusch_cfg_ded_r13.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_r13->pack(bref));
      }
      if (pdcch_candidate_reductions_r13.is_present()) {
        HANDLE_CODE(pdcch_candidate_reductions_r13->pack(bref));
      }
      if (cqi_report_cfg_v1310.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1310->pack(bref));
      }
      if (srs_ul_cfg_ded_v1310.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_v1310->pack(bref));
      }
      if (srs_ul_cfg_ded_up_pts_ext_r13.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_up_pts_ext_r13->pack(bref));
      }
      if (srs_ul_cfg_ded_aperiodic_v1310.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_aperiodic_v1310->pack(bref));
      }
      if (srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_aperiodic_up_pts_ext_r13->pack(bref));
      }
      if (csi_rs_cfg_v1310.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1310->pack(bref));
      }
      if (ce_mode_r13.is_present()) {
        HANDLE_CODE(ce_mode_r13->pack(bref));
      }
      if (csi_rs_cfg_nzp_to_add_mod_list_ext_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *csi_rs_cfg_nzp_to_add_mod_list_ext_r13, 1, 21));
      }
      if (csi_rs_cfg_nzp_to_release_list_ext_r13.is_present()) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, *csi_rs_cfg_nzp_to_release_list_ext_r13, 1, 21, integer_packer<uint8_t>(4, 24)));
      }
    }
    if (group_flags[7]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cqi_report_cfg_v1320.is_present(), 1));
      if (cqi_report_cfg_v1320.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1320->pack(bref));
      }
    }
    if (group_flags[8]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(type_a_srs_tpc_pdcch_group_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(must_cfg_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(pusch_enhance_cfg_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(ce_pdsch_pusch_enhancement_cfg_r14_present, 1));
      HANDLE_CODE(bref.pack(ant_info_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(pucch_cfg_ded_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(pdsch_cfg_ded_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(pusch_cfg_ded_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_periodic_cfg_ded_list_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_aperiodic_cfg_ded_list_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_cfg_ded_ap_up_pts_ext_list_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_zp_ap_list_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(cqi_report_cfg_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(semi_open_loop_r14_present, 1));
      if (type_a_srs_tpc_pdcch_group_r14.is_present()) {
        HANDLE_CODE(type_a_srs_tpc_pdcch_group_r14->pack(bref));
      }
      if (must_cfg_r14.is_present()) {
        HANDLE_CODE(must_cfg_r14->pack(bref));
      }
      if (pusch_enhance_cfg_r14.is_present()) {
        HANDLE_CODE(pusch_enhance_cfg_r14->pack(bref));
      }
      if (ant_info_v1430.is_present()) {
        HANDLE_CODE(ant_info_v1430->pack(bref));
      }
      if (pucch_cfg_ded_v1430.is_present()) {
        HANDLE_CODE(pucch_cfg_ded_v1430->pack(bref));
      }
      if (pdsch_cfg_ded_v1430.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1430->pack(bref));
      }
      if (pusch_cfg_ded_v1430.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1430->pack(bref));
      }
      if (srs_ul_periodic_cfg_ded_list_r14.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *srs_ul_periodic_cfg_ded_list_r14, 1, 2));
      }
      if (srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *srs_ul_periodic_cfg_ded_up_pts_ext_list_r14, 1, 4));
      }
      if (srs_ul_aperiodic_cfg_ded_list_r14.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *srs_ul_aperiodic_cfg_ded_list_r14, 1, 2));
      }
      if (srs_ul_cfg_ded_ap_up_pts_ext_list_r14.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *srs_ul_cfg_ded_ap_up_pts_ext_list_r14, 1, 4));
      }
      if (csi_rs_cfg_v1430.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1430->pack(bref));
      }
      if (csi_rs_cfg_zp_ap_list_r14.is_present()) {
        HANDLE_CODE(csi_rs_cfg_zp_ap_list_r14->pack(bref));
      }
      if (cqi_report_cfg_v1430.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1430->pack(bref));
      }
      if (semi_open_loop_r14_present) {
        HANDLE_CODE(bref.pack(semi_open_loop_r14, 1));
      }
    }
    if (group_flags[9]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(csi_rs_cfg_v1480.is_present(), 1));
      if (csi_rs_cfg_v1480.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1480->pack(bref));
      }
    }
    if (group_flags[10]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(phys_cfg_ded_stti_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(pdsch_cfg_ded_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(pusch_cfg_ded_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(cqi_report_cfg_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(ant_info_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pwr_ctrl_ded_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(semi_static_cfi_cfg_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(blind_pdsch_repeat_cfg_r15.is_present(), 1));
      if (phys_cfg_ded_stti_r15.is_present()) {
        HANDLE_CODE(phys_cfg_ded_stti_r15->pack(bref));
      }
      if (pdsch_cfg_ded_v1530.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1530->pack(bref));
      }
      if (pusch_cfg_ded_v1530.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1530->pack(bref));
      }
      if (cqi_report_cfg_v1530.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1530->pack(bref));
      }
      if (ant_info_v1530.is_present()) {
        HANDLE_CODE(ant_info_v1530->pack(bref));
      }
      if (csi_rs_cfg_v1530.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1530->pack(bref));
      }
      if (ul_pwr_ctrl_ded_v1530.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_ded_v1530->pack(bref));
      }
      if (semi_static_cfi_cfg_r15.is_present()) {
        HANDLE_CODE(semi_static_cfi_cfg_r15->pack(bref));
      }
      if (blind_pdsch_repeat_cfg_r15.is_present()) {
        HANDLE_CODE(blind_pdsch_repeat_cfg_r15->pack(bref));
      }
    }
    if (group_flags[11]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(spucch_cfg_v1550.is_present(), 1));
      if (spucch_cfg_v1550.is_present()) {
        HANDLE_CODE(spucch_cfg_v1550->pack(bref));
      }
    }
    if (group_flags[12]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pdsch_cfg_ded_v1610.is_present(), 1));
      HANDLE_CODE(bref.pack(pusch_cfg_ded_v1610.is_present(), 1));
      HANDLE_CODE(bref.pack(ce_csi_rs_feedback_r16_present, 1));
      HANDLE_CODE(bref.pack(res_reserv_cfg_ded_dl_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(res_reserv_cfg_ded_ul_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_cfg_ded_add_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pwr_ctrl_add_srs_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_virtual_cell_id_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(wideband_prg_r16.is_present(), 1));
      if (pdsch_cfg_ded_v1610.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1610->pack(bref));
      }
      if (pusch_cfg_ded_v1610.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1610->pack(bref));
      }
      if (res_reserv_cfg_ded_dl_r16.is_present()) {
        HANDLE_CODE(res_reserv_cfg_ded_dl_r16->pack(bref));
      }
      if (res_reserv_cfg_ded_ul_r16.is_present()) {
        HANDLE_CODE(res_reserv_cfg_ded_ul_r16->pack(bref));
      }
      if (srs_ul_cfg_ded_add_r16.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_add_r16->pack(bref));
      }
      if (ul_pwr_ctrl_add_srs_r16.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_add_srs_r16->pack(bref));
      }
      if (srs_virtual_cell_id_r16.is_present()) {
        HANDLE_CODE(srs_virtual_cell_id_r16->pack(bref));
      }
      if (wideband_prg_r16.is_present()) {
        HANDLE_CODE(wideband_prg_r16->pack(bref));
      }
    }
    if (group_flags[13]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pdsch_cfg_ded_v1700.is_present(), 1));
      HANDLE_CODE(bref.pack(ntn_cfg_ded_r17.is_present(), 1));
      if (pdsch_cfg_ded_v1700.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1700->pack(bref));
      }
      if (ntn_cfg_ded_r17.is_present()) {
        HANDLE_CODE(bref.pack(ntn_cfg_ded_r17->pucch_tx_dur_r17_present, 1));
        HANDLE_CODE(bref.pack(ntn_cfg_ded_r17->pusch_tx_dur_r17_present, 1));
        if (ntn_cfg_ded_r17->pucch_tx_dur_r17_present) {
          HANDLE_CODE(ntn_cfg_ded_r17->pucch_tx_dur_r17.pack(bref));
        }
        if (ntn_cfg_ded_r17->pusch_tx_dur_r17_present) {
          HANDLE_CODE(ntn_cfg_ded_r17->pusch_tx_dur_r17.pack(bref));
        }
      }
    }
    if (group_flags[14]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_segmented_precompensation_gap_r17_present, 1));
      if (ul_segmented_precompensation_gap_r17_present) {
        HANDLE_CODE(ul_segmented_precompensation_gap_r17.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pdsch_cfg_ded_present, 1));
  HANDLE_CODE(bref.unpack(pucch_cfg_ded_present, 1));
  HANDLE_CODE(bref.unpack(pusch_cfg_ded_present, 1));
  HANDLE_CODE(bref.unpack(ul_pwr_ctrl_ded_present, 1));
  HANDLE_CODE(bref.unpack(tpc_pdcch_cfg_pucch_present, 1));
  HANDLE_CODE(bref.unpack(tpc_pdcch_cfg_pusch_present, 1));
  HANDLE_CODE(bref.unpack(cqi_report_cfg_present, 1));
  HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_present, 1));
  HANDLE_CODE(bref.unpack(ant_info_present, 1));
  HANDLE_CODE(bref.unpack(sched_request_cfg_present, 1));

  if (pdsch_cfg_ded_present) {
    HANDLE_CODE(pdsch_cfg_ded.unpack(bref));
  }
  if (pucch_cfg_ded_present) {
    HANDLE_CODE(pucch_cfg_ded.unpack(bref));
  }
  if (pusch_cfg_ded_present) {
    HANDLE_CODE(pusch_cfg_ded.unpack(bref));
  }
  if (ul_pwr_ctrl_ded_present) {
    HANDLE_CODE(ul_pwr_ctrl_ded.unpack(bref));
  }
  if (tpc_pdcch_cfg_pucch_present) {
    HANDLE_CODE(tpc_pdcch_cfg_pucch.unpack(bref));
  }
  if (tpc_pdcch_cfg_pusch_present) {
    HANDLE_CODE(tpc_pdcch_cfg_pusch.unpack(bref));
  }
  if (cqi_report_cfg_present) {
    HANDLE_CODE(cqi_report_cfg.unpack(bref));
  }
  if (srs_ul_cfg_ded_present) {
    HANDLE_CODE(srs_ul_cfg_ded.unpack(bref));
  }
  if (ant_info_present) {
    HANDLE_CODE(ant_info.unpack(bref));
  }
  if (sched_request_cfg_present) {
    HANDLE_CODE(sched_request_cfg.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(15);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cqi_report_cfg_v920_present;
      HANDLE_CODE(bref.unpack(cqi_report_cfg_v920_present, 1));
      cqi_report_cfg_v920.set_present(cqi_report_cfg_v920_present);
      bool ant_info_v920_present;
      HANDLE_CODE(bref.unpack(ant_info_v920_present, 1));
      ant_info_v920.set_present(ant_info_v920_present);
      if (cqi_report_cfg_v920.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v920->unpack(bref));
      }
      if (ant_info_v920.is_present()) {
        HANDLE_CODE(ant_info_v920->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ant_info_r10_present;
      HANDLE_CODE(bref.unpack(ant_info_r10_present, 1));
      ant_info_r10.set_present(ant_info_r10_present);
      bool ant_info_ul_r10_present;
      HANDLE_CODE(bref.unpack(ant_info_ul_r10_present, 1));
      ant_info_ul_r10.set_present(ant_info_ul_r10_present);
      HANDLE_CODE(bref.unpack(cif_presence_r10_present, 1));
      bool cqi_report_cfg_r10_present;
      HANDLE_CODE(bref.unpack(cqi_report_cfg_r10_present, 1));
      cqi_report_cfg_r10.set_present(cqi_report_cfg_r10_present);
      bool csi_rs_cfg_r10_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_r10_present, 1));
      csi_rs_cfg_r10.set_present(csi_rs_cfg_r10_present);
      bool pucch_cfg_ded_v1020_present;
      HANDLE_CODE(bref.unpack(pucch_cfg_ded_v1020_present, 1));
      pucch_cfg_ded_v1020.set_present(pucch_cfg_ded_v1020_present);
      bool pusch_cfg_ded_v1020_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_ded_v1020_present, 1));
      pusch_cfg_ded_v1020.set_present(pusch_cfg_ded_v1020_present);
      bool sched_request_cfg_v1020_present;
      HANDLE_CODE(bref.unpack(sched_request_cfg_v1020_present, 1));
      sched_request_cfg_v1020.set_present(sched_request_cfg_v1020_present);
      bool srs_ul_cfg_ded_v1020_present;
      HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_v1020_present, 1));
      srs_ul_cfg_ded_v1020.set_present(srs_ul_cfg_ded_v1020_present);
      bool srs_ul_cfg_ded_aperiodic_r10_present;
      HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_aperiodic_r10_present, 1));
      srs_ul_cfg_ded_aperiodic_r10.set_present(srs_ul_cfg_ded_aperiodic_r10_present);
      bool ul_pwr_ctrl_ded_v1020_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_ded_v1020_present, 1));
      ul_pwr_ctrl_ded_v1020.set_present(ul_pwr_ctrl_ded_v1020_present);
      if (ant_info_r10.is_present()) {
        HANDLE_CODE(ant_info_r10->unpack(bref));
      }
      if (ant_info_ul_r10.is_present()) {
        HANDLE_CODE(ant_info_ul_r10->unpack(bref));
      }
      if (cif_presence_r10_present) {
        HANDLE_CODE(bref.unpack(cif_presence_r10, 1));
      }
      if (cqi_report_cfg_r10.is_present()) {
        HANDLE_CODE(cqi_report_cfg_r10->unpack(bref));
      }
      if (csi_rs_cfg_r10.is_present()) {
        HANDLE_CODE(csi_rs_cfg_r10->unpack(bref));
      }
      if (pucch_cfg_ded_v1020.is_present()) {
        HANDLE_CODE(pucch_cfg_ded_v1020->unpack(bref));
      }
      if (pusch_cfg_ded_v1020.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1020->unpack(bref));
      }
      if (sched_request_cfg_v1020.is_present()) {
        HANDLE_CODE(sched_request_cfg_v1020->unpack(bref));
      }
      if (srs_ul_cfg_ded_v1020.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_v1020->unpack(bref));
      }
      if (srs_ul_cfg_ded_aperiodic_r10.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_aperiodic_r10->unpack(bref));
      }
      if (ul_pwr_ctrl_ded_v1020.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_ded_v1020->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool add_spec_emission_ca_r10_present;
      HANDLE_CODE(bref.unpack(add_spec_emission_ca_r10_present, 1));
      add_spec_emission_ca_r10.set_present(add_spec_emission_ca_r10_present);
      if (add_spec_emission_ca_r10.is_present()) {
        HANDLE_CODE(add_spec_emission_ca_r10->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool csi_rs_cfg_nzp_to_release_list_r11_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_to_release_list_r11_present, 1));
      csi_rs_cfg_nzp_to_release_list_r11.set_present(csi_rs_cfg_nzp_to_release_list_r11_present);
      bool csi_rs_cfg_nzp_to_add_mod_list_r11_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_to_add_mod_list_r11_present, 1));
      csi_rs_cfg_nzp_to_add_mod_list_r11.set_present(csi_rs_cfg_nzp_to_add_mod_list_r11_present);
      bool csi_rs_cfg_zp_to_release_list_r11_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_zp_to_release_list_r11_present, 1));
      csi_rs_cfg_zp_to_release_list_r11.set_present(csi_rs_cfg_zp_to_release_list_r11_present);
      bool csi_rs_cfg_zp_to_add_mod_list_r11_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_zp_to_add_mod_list_r11_present, 1));
      csi_rs_cfg_zp_to_add_mod_list_r11.set_present(csi_rs_cfg_zp_to_add_mod_list_r11_present);
      bool epdcch_cfg_r11_present;
      HANDLE_CODE(bref.unpack(epdcch_cfg_r11_present, 1));
      epdcch_cfg_r11.set_present(epdcch_cfg_r11_present);
      bool pdsch_cfg_ded_v1130_present;
      HANDLE_CODE(bref.unpack(pdsch_cfg_ded_v1130_present, 1));
      pdsch_cfg_ded_v1130.set_present(pdsch_cfg_ded_v1130_present);
      bool cqi_report_cfg_v1130_present;
      HANDLE_CODE(bref.unpack(cqi_report_cfg_v1130_present, 1));
      cqi_report_cfg_v1130.set_present(cqi_report_cfg_v1130_present);
      bool pucch_cfg_ded_v1130_present;
      HANDLE_CODE(bref.unpack(pucch_cfg_ded_v1130_present, 1));
      pucch_cfg_ded_v1130.set_present(pucch_cfg_ded_v1130_present);
      bool pusch_cfg_ded_v1130_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_ded_v1130_present, 1));
      pusch_cfg_ded_v1130.set_present(pusch_cfg_ded_v1130_present);
      bool ul_pwr_ctrl_ded_v1130_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_ded_v1130_present, 1));
      ul_pwr_ctrl_ded_v1130.set_present(ul_pwr_ctrl_ded_v1130_present);
      if (csi_rs_cfg_nzp_to_release_list_r11.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*csi_rs_cfg_nzp_to_release_list_r11, bref, 1, 3, integer_packer<uint8_t>(1, 3)));
      }
      if (csi_rs_cfg_nzp_to_add_mod_list_r11.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*csi_rs_cfg_nzp_to_add_mod_list_r11, bref, 1, 3));
      }
      if (csi_rs_cfg_zp_to_release_list_r11.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*csi_rs_cfg_zp_to_release_list_r11, bref, 1, 4, integer_packer<uint8_t>(1, 4)));
      }
      if (csi_rs_cfg_zp_to_add_mod_list_r11.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*csi_rs_cfg_zp_to_add_mod_list_r11, bref, 1, 4));
      }
      if (epdcch_cfg_r11.is_present()) {
        HANDLE_CODE(epdcch_cfg_r11->unpack(bref));
      }
      if (pdsch_cfg_ded_v1130.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1130->unpack(bref));
      }
      if (cqi_report_cfg_v1130.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1130->unpack(bref));
      }
      if (pucch_cfg_ded_v1130.is_present()) {
        HANDLE_CODE(pucch_cfg_ded_v1130->unpack(bref));
      }
      if (pusch_cfg_ded_v1130.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1130->unpack(bref));
      }
      if (ul_pwr_ctrl_ded_v1130.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_ded_v1130->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ant_info_v1250_present;
      HANDLE_CODE(bref.unpack(ant_info_v1250_present, 1));
      ant_info_v1250.set_present(ant_info_v1250_present);
      bool eimta_main_cfg_r12_present;
      HANDLE_CODE(bref.unpack(eimta_main_cfg_r12_present, 1));
      eimta_main_cfg_r12.set_present(eimta_main_cfg_r12_present);
      bool eimta_main_cfg_pcell_r12_present;
      HANDLE_CODE(bref.unpack(eimta_main_cfg_pcell_r12_present, 1));
      eimta_main_cfg_pcell_r12.set_present(eimta_main_cfg_pcell_r12_present);
      bool pucch_cfg_ded_v1250_present;
      HANDLE_CODE(bref.unpack(pucch_cfg_ded_v1250_present, 1));
      pucch_cfg_ded_v1250.set_present(pucch_cfg_ded_v1250_present);
      bool cqi_report_cfg_pcell_v1250_present;
      HANDLE_CODE(bref.unpack(cqi_report_cfg_pcell_v1250_present, 1));
      cqi_report_cfg_pcell_v1250.set_present(cqi_report_cfg_pcell_v1250_present);
      bool ul_pwr_ctrl_ded_v1250_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_ded_v1250_present, 1));
      ul_pwr_ctrl_ded_v1250.set_present(ul_pwr_ctrl_ded_v1250_present);
      bool pusch_cfg_ded_v1250_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_ded_v1250_present, 1));
      pusch_cfg_ded_v1250.set_present(pusch_cfg_ded_v1250_present);
      bool csi_rs_cfg_v1250_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_v1250_present, 1));
      csi_rs_cfg_v1250.set_present(csi_rs_cfg_v1250_present);
      if (ant_info_v1250.is_present()) {
        HANDLE_CODE(ant_info_v1250->unpack(bref));
      }
      if (eimta_main_cfg_r12.is_present()) {
        HANDLE_CODE(eimta_main_cfg_r12->unpack(bref));
      }
      if (eimta_main_cfg_pcell_r12.is_present()) {
        HANDLE_CODE(eimta_main_cfg_pcell_r12->unpack(bref));
      }
      if (pucch_cfg_ded_v1250.is_present()) {
        HANDLE_CODE(pucch_cfg_ded_v1250->unpack(bref));
      }
      if (cqi_report_cfg_pcell_v1250.is_present()) {
        HANDLE_CODE(cqi_report_cfg_pcell_v1250->unpack(bref));
      }
      if (ul_pwr_ctrl_ded_v1250.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_ded_v1250->unpack(bref));
      }
      if (pusch_cfg_ded_v1250.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1250->unpack(bref));
      }
      if (csi_rs_cfg_v1250.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1250->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool pdsch_cfg_ded_v1280_present;
      HANDLE_CODE(bref.unpack(pdsch_cfg_ded_v1280_present, 1));
      pdsch_cfg_ded_v1280.set_present(pdsch_cfg_ded_v1280_present);
      if (pdsch_cfg_ded_v1280.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1280->unpack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool pdsch_cfg_ded_v1310_present;
      HANDLE_CODE(bref.unpack(pdsch_cfg_ded_v1310_present, 1));
      pdsch_cfg_ded_v1310.set_present(pdsch_cfg_ded_v1310_present);
      bool pucch_cfg_ded_r13_present;
      HANDLE_CODE(bref.unpack(pucch_cfg_ded_r13_present, 1));
      pucch_cfg_ded_r13.set_present(pucch_cfg_ded_r13_present);
      bool pusch_cfg_ded_r13_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_ded_r13_present, 1));
      pusch_cfg_ded_r13.set_present(pusch_cfg_ded_r13_present);
      bool pdcch_candidate_reductions_r13_present;
      HANDLE_CODE(bref.unpack(pdcch_candidate_reductions_r13_present, 1));
      pdcch_candidate_reductions_r13.set_present(pdcch_candidate_reductions_r13_present);
      bool cqi_report_cfg_v1310_present;
      HANDLE_CODE(bref.unpack(cqi_report_cfg_v1310_present, 1));
      cqi_report_cfg_v1310.set_present(cqi_report_cfg_v1310_present);
      bool srs_ul_cfg_ded_v1310_present;
      HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_v1310_present, 1));
      srs_ul_cfg_ded_v1310.set_present(srs_ul_cfg_ded_v1310_present);
      bool srs_ul_cfg_ded_up_pts_ext_r13_present;
      HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_up_pts_ext_r13_present, 1));
      srs_ul_cfg_ded_up_pts_ext_r13.set_present(srs_ul_cfg_ded_up_pts_ext_r13_present);
      bool srs_ul_cfg_ded_aperiodic_v1310_present;
      HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_aperiodic_v1310_present, 1));
      srs_ul_cfg_ded_aperiodic_v1310.set_present(srs_ul_cfg_ded_aperiodic_v1310_present);
      bool srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_present;
      HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_present, 1));
      srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.set_present(srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_present);
      bool csi_rs_cfg_v1310_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_v1310_present, 1));
      csi_rs_cfg_v1310.set_present(csi_rs_cfg_v1310_present);
      bool ce_mode_r13_present;
      HANDLE_CODE(bref.unpack(ce_mode_r13_present, 1));
      ce_mode_r13.set_present(ce_mode_r13_present);
      bool csi_rs_cfg_nzp_to_add_mod_list_ext_r13_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_to_add_mod_list_ext_r13_present, 1));
      csi_rs_cfg_nzp_to_add_mod_list_ext_r13.set_present(csi_rs_cfg_nzp_to_add_mod_list_ext_r13_present);
      bool csi_rs_cfg_nzp_to_release_list_ext_r13_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_to_release_list_ext_r13_present, 1));
      csi_rs_cfg_nzp_to_release_list_ext_r13.set_present(csi_rs_cfg_nzp_to_release_list_ext_r13_present);
      if (pdsch_cfg_ded_v1310.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1310->unpack(bref));
      }
      if (pucch_cfg_ded_r13.is_present()) {
        HANDLE_CODE(pucch_cfg_ded_r13->unpack(bref));
      }
      if (pusch_cfg_ded_r13.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_r13->unpack(bref));
      }
      if (pdcch_candidate_reductions_r13.is_present()) {
        HANDLE_CODE(pdcch_candidate_reductions_r13->unpack(bref));
      }
      if (cqi_report_cfg_v1310.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1310->unpack(bref));
      }
      if (srs_ul_cfg_ded_v1310.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_v1310->unpack(bref));
      }
      if (srs_ul_cfg_ded_up_pts_ext_r13.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_up_pts_ext_r13->unpack(bref));
      }
      if (srs_ul_cfg_ded_aperiodic_v1310.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_aperiodic_v1310->unpack(bref));
      }
      if (srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_aperiodic_up_pts_ext_r13->unpack(bref));
      }
      if (csi_rs_cfg_v1310.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1310->unpack(bref));
      }
      if (ce_mode_r13.is_present()) {
        HANDLE_CODE(ce_mode_r13->unpack(bref));
      }
      if (csi_rs_cfg_nzp_to_add_mod_list_ext_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*csi_rs_cfg_nzp_to_add_mod_list_ext_r13, bref, 1, 21));
      }
      if (csi_rs_cfg_nzp_to_release_list_ext_r13.is_present()) {
        HANDLE_CODE(
            unpack_dyn_seq_of(*csi_rs_cfg_nzp_to_release_list_ext_r13, bref, 1, 21, integer_packer<uint8_t>(4, 24)));
      }
    }
    if (group_flags[7]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cqi_report_cfg_v1320_present;
      HANDLE_CODE(bref.unpack(cqi_report_cfg_v1320_present, 1));
      cqi_report_cfg_v1320.set_present(cqi_report_cfg_v1320_present);
      if (cqi_report_cfg_v1320.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1320->unpack(bref));
      }
    }
    if (group_flags[8]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool type_a_srs_tpc_pdcch_group_r14_present;
      HANDLE_CODE(bref.unpack(type_a_srs_tpc_pdcch_group_r14_present, 1));
      type_a_srs_tpc_pdcch_group_r14.set_present(type_a_srs_tpc_pdcch_group_r14_present);
      bool must_cfg_r14_present;
      HANDLE_CODE(bref.unpack(must_cfg_r14_present, 1));
      must_cfg_r14.set_present(must_cfg_r14_present);
      bool pusch_enhance_cfg_r14_present;
      HANDLE_CODE(bref.unpack(pusch_enhance_cfg_r14_present, 1));
      pusch_enhance_cfg_r14.set_present(pusch_enhance_cfg_r14_present);
      HANDLE_CODE(bref.unpack(ce_pdsch_pusch_enhancement_cfg_r14_present, 1));
      bool ant_info_v1430_present;
      HANDLE_CODE(bref.unpack(ant_info_v1430_present, 1));
      ant_info_v1430.set_present(ant_info_v1430_present);
      bool pucch_cfg_ded_v1430_present;
      HANDLE_CODE(bref.unpack(pucch_cfg_ded_v1430_present, 1));
      pucch_cfg_ded_v1430.set_present(pucch_cfg_ded_v1430_present);
      bool pdsch_cfg_ded_v1430_present;
      HANDLE_CODE(bref.unpack(pdsch_cfg_ded_v1430_present, 1));
      pdsch_cfg_ded_v1430.set_present(pdsch_cfg_ded_v1430_present);
      bool pusch_cfg_ded_v1430_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_ded_v1430_present, 1));
      pusch_cfg_ded_v1430.set_present(pusch_cfg_ded_v1430_present);
      bool srs_ul_periodic_cfg_ded_list_r14_present;
      HANDLE_CODE(bref.unpack(srs_ul_periodic_cfg_ded_list_r14_present, 1));
      srs_ul_periodic_cfg_ded_list_r14.set_present(srs_ul_periodic_cfg_ded_list_r14_present);
      bool srs_ul_periodic_cfg_ded_up_pts_ext_list_r14_present;
      HANDLE_CODE(bref.unpack(srs_ul_periodic_cfg_ded_up_pts_ext_list_r14_present, 1));
      srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.set_present(srs_ul_periodic_cfg_ded_up_pts_ext_list_r14_present);
      bool srs_ul_aperiodic_cfg_ded_list_r14_present;
      HANDLE_CODE(bref.unpack(srs_ul_aperiodic_cfg_ded_list_r14_present, 1));
      srs_ul_aperiodic_cfg_ded_list_r14.set_present(srs_ul_aperiodic_cfg_ded_list_r14_present);
      bool srs_ul_cfg_ded_ap_up_pts_ext_list_r14_present;
      HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_ap_up_pts_ext_list_r14_present, 1));
      srs_ul_cfg_ded_ap_up_pts_ext_list_r14.set_present(srs_ul_cfg_ded_ap_up_pts_ext_list_r14_present);
      bool csi_rs_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_v1430_present, 1));
      csi_rs_cfg_v1430.set_present(csi_rs_cfg_v1430_present);
      bool csi_rs_cfg_zp_ap_list_r14_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_zp_ap_list_r14_present, 1));
      csi_rs_cfg_zp_ap_list_r14.set_present(csi_rs_cfg_zp_ap_list_r14_present);
      bool cqi_report_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(cqi_report_cfg_v1430_present, 1));
      cqi_report_cfg_v1430.set_present(cqi_report_cfg_v1430_present);
      HANDLE_CODE(bref.unpack(semi_open_loop_r14_present, 1));
      if (type_a_srs_tpc_pdcch_group_r14.is_present()) {
        HANDLE_CODE(type_a_srs_tpc_pdcch_group_r14->unpack(bref));
      }
      if (must_cfg_r14.is_present()) {
        HANDLE_CODE(must_cfg_r14->unpack(bref));
      }
      if (pusch_enhance_cfg_r14.is_present()) {
        HANDLE_CODE(pusch_enhance_cfg_r14->unpack(bref));
      }
      if (ant_info_v1430.is_present()) {
        HANDLE_CODE(ant_info_v1430->unpack(bref));
      }
      if (pucch_cfg_ded_v1430.is_present()) {
        HANDLE_CODE(pucch_cfg_ded_v1430->unpack(bref));
      }
      if (pdsch_cfg_ded_v1430.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1430->unpack(bref));
      }
      if (pusch_cfg_ded_v1430.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1430->unpack(bref));
      }
      if (srs_ul_periodic_cfg_ded_list_r14.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*srs_ul_periodic_cfg_ded_list_r14, bref, 1, 2));
      }
      if (srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*srs_ul_periodic_cfg_ded_up_pts_ext_list_r14, bref, 1, 4));
      }
      if (srs_ul_aperiodic_cfg_ded_list_r14.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*srs_ul_aperiodic_cfg_ded_list_r14, bref, 1, 2));
      }
      if (srs_ul_cfg_ded_ap_up_pts_ext_list_r14.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*srs_ul_cfg_ded_ap_up_pts_ext_list_r14, bref, 1, 4));
      }
      if (csi_rs_cfg_v1430.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1430->unpack(bref));
      }
      if (csi_rs_cfg_zp_ap_list_r14.is_present()) {
        HANDLE_CODE(csi_rs_cfg_zp_ap_list_r14->unpack(bref));
      }
      if (cqi_report_cfg_v1430.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1430->unpack(bref));
      }
      if (semi_open_loop_r14_present) {
        HANDLE_CODE(bref.unpack(semi_open_loop_r14, 1));
      }
    }
    if (group_flags[9]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool csi_rs_cfg_v1480_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_v1480_present, 1));
      csi_rs_cfg_v1480.set_present(csi_rs_cfg_v1480_present);
      if (csi_rs_cfg_v1480.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1480->unpack(bref));
      }
    }
    if (group_flags[10]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool phys_cfg_ded_stti_r15_present;
      HANDLE_CODE(bref.unpack(phys_cfg_ded_stti_r15_present, 1));
      phys_cfg_ded_stti_r15.set_present(phys_cfg_ded_stti_r15_present);
      bool pdsch_cfg_ded_v1530_present;
      HANDLE_CODE(bref.unpack(pdsch_cfg_ded_v1530_present, 1));
      pdsch_cfg_ded_v1530.set_present(pdsch_cfg_ded_v1530_present);
      bool pusch_cfg_ded_v1530_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_ded_v1530_present, 1));
      pusch_cfg_ded_v1530.set_present(pusch_cfg_ded_v1530_present);
      bool cqi_report_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(cqi_report_cfg_v1530_present, 1));
      cqi_report_cfg_v1530.set_present(cqi_report_cfg_v1530_present);
      bool ant_info_v1530_present;
      HANDLE_CODE(bref.unpack(ant_info_v1530_present, 1));
      ant_info_v1530.set_present(ant_info_v1530_present);
      bool csi_rs_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_v1530_present, 1));
      csi_rs_cfg_v1530.set_present(csi_rs_cfg_v1530_present);
      bool ul_pwr_ctrl_ded_v1530_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_ded_v1530_present, 1));
      ul_pwr_ctrl_ded_v1530.set_present(ul_pwr_ctrl_ded_v1530_present);
      bool semi_static_cfi_cfg_r15_present;
      HANDLE_CODE(bref.unpack(semi_static_cfi_cfg_r15_present, 1));
      semi_static_cfi_cfg_r15.set_present(semi_static_cfi_cfg_r15_present);
      bool blind_pdsch_repeat_cfg_r15_present;
      HANDLE_CODE(bref.unpack(blind_pdsch_repeat_cfg_r15_present, 1));
      blind_pdsch_repeat_cfg_r15.set_present(blind_pdsch_repeat_cfg_r15_present);
      if (phys_cfg_ded_stti_r15.is_present()) {
        HANDLE_CODE(phys_cfg_ded_stti_r15->unpack(bref));
      }
      if (pdsch_cfg_ded_v1530.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1530->unpack(bref));
      }
      if (pusch_cfg_ded_v1530.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1530->unpack(bref));
      }
      if (cqi_report_cfg_v1530.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1530->unpack(bref));
      }
      if (ant_info_v1530.is_present()) {
        HANDLE_CODE(ant_info_v1530->unpack(bref));
      }
      if (csi_rs_cfg_v1530.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1530->unpack(bref));
      }
      if (ul_pwr_ctrl_ded_v1530.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_ded_v1530->unpack(bref));
      }
      if (semi_static_cfi_cfg_r15.is_present()) {
        HANDLE_CODE(semi_static_cfi_cfg_r15->unpack(bref));
      }
      if (blind_pdsch_repeat_cfg_r15.is_present()) {
        HANDLE_CODE(blind_pdsch_repeat_cfg_r15->unpack(bref));
      }
    }
    if (group_flags[11]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool spucch_cfg_v1550_present;
      HANDLE_CODE(bref.unpack(spucch_cfg_v1550_present, 1));
      spucch_cfg_v1550.set_present(spucch_cfg_v1550_present);
      if (spucch_cfg_v1550.is_present()) {
        HANDLE_CODE(spucch_cfg_v1550->unpack(bref));
      }
    }
    if (group_flags[12]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool pdsch_cfg_ded_v1610_present;
      HANDLE_CODE(bref.unpack(pdsch_cfg_ded_v1610_present, 1));
      pdsch_cfg_ded_v1610.set_present(pdsch_cfg_ded_v1610_present);
      bool pusch_cfg_ded_v1610_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_ded_v1610_present, 1));
      pusch_cfg_ded_v1610.set_present(pusch_cfg_ded_v1610_present);
      HANDLE_CODE(bref.unpack(ce_csi_rs_feedback_r16_present, 1));
      bool res_reserv_cfg_ded_dl_r16_present;
      HANDLE_CODE(bref.unpack(res_reserv_cfg_ded_dl_r16_present, 1));
      res_reserv_cfg_ded_dl_r16.set_present(res_reserv_cfg_ded_dl_r16_present);
      bool res_reserv_cfg_ded_ul_r16_present;
      HANDLE_CODE(bref.unpack(res_reserv_cfg_ded_ul_r16_present, 1));
      res_reserv_cfg_ded_ul_r16.set_present(res_reserv_cfg_ded_ul_r16_present);
      bool srs_ul_cfg_ded_add_r16_present;
      HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_add_r16_present, 1));
      srs_ul_cfg_ded_add_r16.set_present(srs_ul_cfg_ded_add_r16_present);
      bool ul_pwr_ctrl_add_srs_r16_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_add_srs_r16_present, 1));
      ul_pwr_ctrl_add_srs_r16.set_present(ul_pwr_ctrl_add_srs_r16_present);
      bool srs_virtual_cell_id_r16_present;
      HANDLE_CODE(bref.unpack(srs_virtual_cell_id_r16_present, 1));
      srs_virtual_cell_id_r16.set_present(srs_virtual_cell_id_r16_present);
      bool wideband_prg_r16_present;
      HANDLE_CODE(bref.unpack(wideband_prg_r16_present, 1));
      wideband_prg_r16.set_present(wideband_prg_r16_present);
      if (pdsch_cfg_ded_v1610.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1610->unpack(bref));
      }
      if (pusch_cfg_ded_v1610.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1610->unpack(bref));
      }
      if (res_reserv_cfg_ded_dl_r16.is_present()) {
        HANDLE_CODE(res_reserv_cfg_ded_dl_r16->unpack(bref));
      }
      if (res_reserv_cfg_ded_ul_r16.is_present()) {
        HANDLE_CODE(res_reserv_cfg_ded_ul_r16->unpack(bref));
      }
      if (srs_ul_cfg_ded_add_r16.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_add_r16->unpack(bref));
      }
      if (ul_pwr_ctrl_add_srs_r16.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_add_srs_r16->unpack(bref));
      }
      if (srs_virtual_cell_id_r16.is_present()) {
        HANDLE_CODE(srs_virtual_cell_id_r16->unpack(bref));
      }
      if (wideband_prg_r16.is_present()) {
        HANDLE_CODE(wideband_prg_r16->unpack(bref));
      }
    }
    if (group_flags[13]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool pdsch_cfg_ded_v1700_present;
      HANDLE_CODE(bref.unpack(pdsch_cfg_ded_v1700_present, 1));
      pdsch_cfg_ded_v1700.set_present(pdsch_cfg_ded_v1700_present);
      bool ntn_cfg_ded_r17_present;
      HANDLE_CODE(bref.unpack(ntn_cfg_ded_r17_present, 1));
      ntn_cfg_ded_r17.set_present(ntn_cfg_ded_r17_present);
      if (pdsch_cfg_ded_v1700.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1700->unpack(bref));
      }
      if (ntn_cfg_ded_r17.is_present()) {
        HANDLE_CODE(bref.unpack(ntn_cfg_ded_r17->pucch_tx_dur_r17_present, 1));
        HANDLE_CODE(bref.unpack(ntn_cfg_ded_r17->pusch_tx_dur_r17_present, 1));
        if (ntn_cfg_ded_r17->pucch_tx_dur_r17_present) {
          HANDLE_CODE(ntn_cfg_ded_r17->pucch_tx_dur_r17.unpack(bref));
        }
        if (ntn_cfg_ded_r17->pusch_tx_dur_r17_present) {
          HANDLE_CODE(ntn_cfg_ded_r17->pusch_tx_dur_r17.unpack(bref));
        }
      }
    }
    if (group_flags[14]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ul_segmented_precompensation_gap_r17_present, 1));
      if (ul_segmented_precompensation_gap_r17_present) {
        HANDLE_CODE(ul_segmented_precompensation_gap_r17.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void phys_cfg_ded_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdsch_cfg_ded_present) {
    j.write_fieldname("pdsch-ConfigDedicated");
    pdsch_cfg_ded.to_json(j);
  }
  if (pucch_cfg_ded_present) {
    j.write_fieldname("pucch-ConfigDedicated");
    pucch_cfg_ded.to_json(j);
  }
  if (pusch_cfg_ded_present) {
    j.write_fieldname("pusch-ConfigDedicated");
    pusch_cfg_ded.to_json(j);
  }
  if (ul_pwr_ctrl_ded_present) {
    j.write_fieldname("uplinkPowerControlDedicated");
    ul_pwr_ctrl_ded.to_json(j);
  }
  if (tpc_pdcch_cfg_pucch_present) {
    j.write_fieldname("tpc-PDCCH-ConfigPUCCH");
    tpc_pdcch_cfg_pucch.to_json(j);
  }
  if (tpc_pdcch_cfg_pusch_present) {
    j.write_fieldname("tpc-PDCCH-ConfigPUSCH");
    tpc_pdcch_cfg_pusch.to_json(j);
  }
  if (cqi_report_cfg_present) {
    j.write_fieldname("cqi-ReportConfig");
    cqi_report_cfg.to_json(j);
  }
  if (srs_ul_cfg_ded_present) {
    j.write_fieldname("soundingRS-UL-ConfigDedicated");
    srs_ul_cfg_ded.to_json(j);
  }
  if (ant_info_present) {
    j.write_fieldname("antennaInfo");
    ant_info.to_json(j);
  }
  if (sched_request_cfg_present) {
    j.write_fieldname("schedulingRequestConfig");
    sched_request_cfg.to_json(j);
  }
  if (ext) {
    if (cqi_report_cfg_v920.is_present()) {
      j.write_fieldname("cqi-ReportConfig-v920");
      cqi_report_cfg_v920->to_json(j);
    }
    if (ant_info_v920.is_present()) {
      j.write_fieldname("antennaInfo-v920");
      ant_info_v920->to_json(j);
    }
    if (ant_info_r10.is_present()) {
      j.write_fieldname("antennaInfo-r10");
      ant_info_r10->to_json(j);
    }
    if (ant_info_ul_r10.is_present()) {
      j.write_fieldname("antennaInfoUL-r10");
      ant_info_ul_r10->to_json(j);
    }
    if (cif_presence_r10_present) {
      j.write_bool("cif-Presence-r10", cif_presence_r10);
    }
    if (cqi_report_cfg_r10.is_present()) {
      j.write_fieldname("cqi-ReportConfig-r10");
      cqi_report_cfg_r10->to_json(j);
    }
    if (csi_rs_cfg_r10.is_present()) {
      j.write_fieldname("csi-RS-Config-r10");
      csi_rs_cfg_r10->to_json(j);
    }
    if (pucch_cfg_ded_v1020.is_present()) {
      j.write_fieldname("pucch-ConfigDedicated-v1020");
      pucch_cfg_ded_v1020->to_json(j);
    }
    if (pusch_cfg_ded_v1020.is_present()) {
      j.write_fieldname("pusch-ConfigDedicated-v1020");
      pusch_cfg_ded_v1020->to_json(j);
    }
    if (sched_request_cfg_v1020.is_present()) {
      j.write_fieldname("schedulingRequestConfig-v1020");
      sched_request_cfg_v1020->to_json(j);
    }
    if (srs_ul_cfg_ded_v1020.is_present()) {
      j.write_fieldname("soundingRS-UL-ConfigDedicated-v1020");
      srs_ul_cfg_ded_v1020->to_json(j);
    }
    if (srs_ul_cfg_ded_aperiodic_r10.is_present()) {
      j.write_fieldname("soundingRS-UL-ConfigDedicatedAperiodic-r10");
      srs_ul_cfg_ded_aperiodic_r10->to_json(j);
    }
    if (ul_pwr_ctrl_ded_v1020.is_present()) {
      j.write_fieldname("uplinkPowerControlDedicated-v1020");
      ul_pwr_ctrl_ded_v1020->to_json(j);
    }
    if (add_spec_emission_ca_r10.is_present()) {
      j.write_fieldname("additionalSpectrumEmissionCA-r10");
      add_spec_emission_ca_r10->to_json(j);
    }
    if (csi_rs_cfg_nzp_to_release_list_r11.is_present()) {
      j.start_array("csi-RS-ConfigNZPToReleaseList-r11");
      for (const auto& e1 : *csi_rs_cfg_nzp_to_release_list_r11) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (csi_rs_cfg_nzp_to_add_mod_list_r11.is_present()) {
      j.start_array("csi-RS-ConfigNZPToAddModList-r11");
      for (const auto& e1 : *csi_rs_cfg_nzp_to_add_mod_list_r11) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (csi_rs_cfg_zp_to_release_list_r11.is_present()) {
      j.start_array("csi-RS-ConfigZPToReleaseList-r11");
      for (const auto& e1 : *csi_rs_cfg_zp_to_release_list_r11) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (csi_rs_cfg_zp_to_add_mod_list_r11.is_present()) {
      j.start_array("csi-RS-ConfigZPToAddModList-r11");
      for (const auto& e1 : *csi_rs_cfg_zp_to_add_mod_list_r11) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (epdcch_cfg_r11.is_present()) {
      j.write_fieldname("epdcch-Config-r11");
      epdcch_cfg_r11->to_json(j);
    }
    if (pdsch_cfg_ded_v1130.is_present()) {
      j.write_fieldname("pdsch-ConfigDedicated-v1130");
      pdsch_cfg_ded_v1130->to_json(j);
    }
    if (cqi_report_cfg_v1130.is_present()) {
      j.write_fieldname("cqi-ReportConfig-v1130");
      cqi_report_cfg_v1130->to_json(j);
    }
    if (pucch_cfg_ded_v1130.is_present()) {
      j.write_fieldname("pucch-ConfigDedicated-v1130");
      pucch_cfg_ded_v1130->to_json(j);
    }
    if (pusch_cfg_ded_v1130.is_present()) {
      j.write_fieldname("pusch-ConfigDedicated-v1130");
      pusch_cfg_ded_v1130->to_json(j);
    }
    if (ul_pwr_ctrl_ded_v1130.is_present()) {
      j.write_fieldname("uplinkPowerControlDedicated-v1130");
      ul_pwr_ctrl_ded_v1130->to_json(j);
    }
    if (ant_info_v1250.is_present()) {
      j.write_fieldname("antennaInfo-v1250");
      ant_info_v1250->to_json(j);
    }
    if (eimta_main_cfg_r12.is_present()) {
      j.write_fieldname("eimta-MainConfig-r12");
      eimta_main_cfg_r12->to_json(j);
    }
    if (eimta_main_cfg_pcell_r12.is_present()) {
      j.write_fieldname("eimta-MainConfigPCell-r12");
      eimta_main_cfg_pcell_r12->to_json(j);
    }
    if (pucch_cfg_ded_v1250.is_present()) {
      j.write_fieldname("pucch-ConfigDedicated-v1250");
      pucch_cfg_ded_v1250->to_json(j);
    }
    if (cqi_report_cfg_pcell_v1250.is_present()) {
      j.write_fieldname("cqi-ReportConfigPCell-v1250");
      cqi_report_cfg_pcell_v1250->to_json(j);
    }
    if (ul_pwr_ctrl_ded_v1250.is_present()) {
      j.write_fieldname("uplinkPowerControlDedicated-v1250");
      ul_pwr_ctrl_ded_v1250->to_json(j);
    }
    if (pusch_cfg_ded_v1250.is_present()) {
      j.write_fieldname("pusch-ConfigDedicated-v1250");
      pusch_cfg_ded_v1250->to_json(j);
    }
    if (csi_rs_cfg_v1250.is_present()) {
      j.write_fieldname("csi-RS-Config-v1250");
      csi_rs_cfg_v1250->to_json(j);
    }
    if (pdsch_cfg_ded_v1280.is_present()) {
      j.write_fieldname("pdsch-ConfigDedicated-v1280");
      pdsch_cfg_ded_v1280->to_json(j);
    }
    if (pdsch_cfg_ded_v1310.is_present()) {
      j.write_fieldname("pdsch-ConfigDedicated-v1310");
      pdsch_cfg_ded_v1310->to_json(j);
    }
    if (pucch_cfg_ded_r13.is_present()) {
      j.write_fieldname("pucch-ConfigDedicated-r13");
      pucch_cfg_ded_r13->to_json(j);
    }
    if (pusch_cfg_ded_r13.is_present()) {
      j.write_fieldname("pusch-ConfigDedicated-r13");
      pusch_cfg_ded_r13->to_json(j);
    }
    if (pdcch_candidate_reductions_r13.is_present()) {
      j.write_fieldname("pdcch-CandidateReductions-r13");
      pdcch_candidate_reductions_r13->to_json(j);
    }
    if (cqi_report_cfg_v1310.is_present()) {
      j.write_fieldname("cqi-ReportConfig-v1310");
      cqi_report_cfg_v1310->to_json(j);
    }
    if (srs_ul_cfg_ded_v1310.is_present()) {
      j.write_fieldname("soundingRS-UL-ConfigDedicated-v1310");
      srs_ul_cfg_ded_v1310->to_json(j);
    }
    if (srs_ul_cfg_ded_up_pts_ext_r13.is_present()) {
      j.write_fieldname("soundingRS-UL-ConfigDedicatedUpPTsExt-r13");
      srs_ul_cfg_ded_up_pts_ext_r13->to_json(j);
    }
    if (srs_ul_cfg_ded_aperiodic_v1310.is_present()) {
      j.write_fieldname("soundingRS-UL-ConfigDedicatedAperiodic-v1310");
      srs_ul_cfg_ded_aperiodic_v1310->to_json(j);
    }
    if (srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.is_present()) {
      j.write_fieldname("soundingRS-UL-ConfigDedicatedAperiodicUpPTsExt-r13");
      srs_ul_cfg_ded_aperiodic_up_pts_ext_r13->to_json(j);
    }
    if (csi_rs_cfg_v1310.is_present()) {
      j.write_fieldname("csi-RS-Config-v1310");
      csi_rs_cfg_v1310->to_json(j);
    }
    if (ce_mode_r13.is_present()) {
      j.write_fieldname("ce-Mode-r13");
      ce_mode_r13->to_json(j);
    }
    if (csi_rs_cfg_nzp_to_add_mod_list_ext_r13.is_present()) {
      j.start_array("csi-RS-ConfigNZPToAddModListExt-r13");
      for (const auto& e1 : *csi_rs_cfg_nzp_to_add_mod_list_ext_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (csi_rs_cfg_nzp_to_release_list_ext_r13.is_present()) {
      j.start_array("csi-RS-ConfigNZPToReleaseListExt-r13");
      for (const auto& e1 : *csi_rs_cfg_nzp_to_release_list_ext_r13) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (cqi_report_cfg_v1320.is_present()) {
      j.write_fieldname("cqi-ReportConfig-v1320");
      cqi_report_cfg_v1320->to_json(j);
    }
    if (type_a_srs_tpc_pdcch_group_r14.is_present()) {
      j.write_fieldname("typeA-SRS-TPC-PDCCH-Group-r14");
      type_a_srs_tpc_pdcch_group_r14->to_json(j);
    }
    if (must_cfg_r14.is_present()) {
      j.write_fieldname("must-Config-r14");
      must_cfg_r14->to_json(j);
    }
    if (pusch_enhance_cfg_r14.is_present()) {
      j.write_fieldname("pusch-EnhancementsConfig-r14");
      pusch_enhance_cfg_r14->to_json(j);
    }
    if (ce_pdsch_pusch_enhancement_cfg_r14_present) {
      j.write_str("ce-pdsch-pusch-EnhancementConfig-r14", "on");
    }
    if (ant_info_v1430.is_present()) {
      j.write_fieldname("antennaInfo-v1430");
      ant_info_v1430->to_json(j);
    }
    if (pucch_cfg_ded_v1430.is_present()) {
      j.write_fieldname("pucch-ConfigDedicated-v1430");
      pucch_cfg_ded_v1430->to_json(j);
    }
    if (pdsch_cfg_ded_v1430.is_present()) {
      j.write_fieldname("pdsch-ConfigDedicated-v1430");
      pdsch_cfg_ded_v1430->to_json(j);
    }
    if (pusch_cfg_ded_v1430.is_present()) {
      j.write_fieldname("pusch-ConfigDedicated-v1430");
      pusch_cfg_ded_v1430->to_json(j);
    }
    if (srs_ul_periodic_cfg_ded_list_r14.is_present()) {
      j.start_array("soundingRS-UL-PeriodicConfigDedicatedList-r14");
      for (const auto& e1 : *srs_ul_periodic_cfg_ded_list_r14) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.is_present()) {
      j.start_array("soundingRS-UL-PeriodicConfigDedicatedUpPTsExtList-r14");
      for (const auto& e1 : *srs_ul_periodic_cfg_ded_up_pts_ext_list_r14) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (srs_ul_aperiodic_cfg_ded_list_r14.is_present()) {
      j.start_array("soundingRS-UL-AperiodicConfigDedicatedList-r14");
      for (const auto& e1 : *srs_ul_aperiodic_cfg_ded_list_r14) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (srs_ul_cfg_ded_ap_up_pts_ext_list_r14.is_present()) {
      j.start_array("soundingRS-UL-ConfigDedicatedApUpPTsExtList-r14");
      for (const auto& e1 : *srs_ul_cfg_ded_ap_up_pts_ext_list_r14) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (csi_rs_cfg_v1430.is_present()) {
      j.write_fieldname("csi-RS-Config-v1430");
      csi_rs_cfg_v1430->to_json(j);
    }
    if (csi_rs_cfg_zp_ap_list_r14.is_present()) {
      j.write_fieldname("csi-RS-ConfigZP-ApList-r14");
      csi_rs_cfg_zp_ap_list_r14->to_json(j);
    }
    if (cqi_report_cfg_v1430.is_present()) {
      j.write_fieldname("cqi-ReportConfig-v1430");
      cqi_report_cfg_v1430->to_json(j);
    }
    if (semi_open_loop_r14_present) {
      j.write_bool("semiOpenLoop-r14", semi_open_loop_r14);
    }
    if (csi_rs_cfg_v1480.is_present()) {
      j.write_fieldname("csi-RS-Config-v1480");
      csi_rs_cfg_v1480->to_json(j);
    }
    if (phys_cfg_ded_stti_r15.is_present()) {
      j.write_fieldname("physicalConfigDedicatedSTTI-r15");
      phys_cfg_ded_stti_r15->to_json(j);
    }
    if (pdsch_cfg_ded_v1530.is_present()) {
      j.write_fieldname("pdsch-ConfigDedicated-v1530");
      pdsch_cfg_ded_v1530->to_json(j);
    }
    if (pusch_cfg_ded_v1530.is_present()) {
      j.write_fieldname("pusch-ConfigDedicated-v1530");
      pusch_cfg_ded_v1530->to_json(j);
    }
    if (cqi_report_cfg_v1530.is_present()) {
      j.write_fieldname("cqi-ReportConfig-v1530");
      cqi_report_cfg_v1530->to_json(j);
    }
    if (ant_info_v1530.is_present()) {
      j.write_fieldname("antennaInfo-v1530");
      ant_info_v1530->to_json(j);
    }
    if (csi_rs_cfg_v1530.is_present()) {
      j.write_fieldname("csi-RS-Config-v1530");
      csi_rs_cfg_v1530->to_json(j);
    }
    if (ul_pwr_ctrl_ded_v1530.is_present()) {
      j.write_fieldname("uplinkPowerControlDedicated-v1530");
      ul_pwr_ctrl_ded_v1530->to_json(j);
    }
    if (semi_static_cfi_cfg_r15.is_present()) {
      j.write_fieldname("semiStaticCFI-Config-r15");
      semi_static_cfi_cfg_r15->to_json(j);
    }
    if (blind_pdsch_repeat_cfg_r15.is_present()) {
      j.write_fieldname("blindPDSCH-Repetition-Config-r15");
      blind_pdsch_repeat_cfg_r15->to_json(j);
    }
    if (spucch_cfg_v1550.is_present()) {
      j.write_fieldname("spucch-Config-v1550");
      spucch_cfg_v1550->to_json(j);
    }
    if (pdsch_cfg_ded_v1610.is_present()) {
      j.write_fieldname("pdsch-ConfigDedicated-v1610");
      pdsch_cfg_ded_v1610->to_json(j);
    }
    if (pusch_cfg_ded_v1610.is_present()) {
      j.write_fieldname("pusch-ConfigDedicated-v1610");
      pusch_cfg_ded_v1610->to_json(j);
    }
    if (ce_csi_rs_feedback_r16_present) {
      j.write_str("ce-CSI-RS-Feedback-r16", "enabled");
    }
    if (res_reserv_cfg_ded_dl_r16.is_present()) {
      j.write_fieldname("resourceReservationConfigDedicatedDL-r16");
      res_reserv_cfg_ded_dl_r16->to_json(j);
    }
    if (res_reserv_cfg_ded_ul_r16.is_present()) {
      j.write_fieldname("resourceReservationConfigDedicatedUL-r16");
      res_reserv_cfg_ded_ul_r16->to_json(j);
    }
    if (srs_ul_cfg_ded_add_r16.is_present()) {
      j.write_fieldname("soundingRS-UL-ConfigDedicatedAdd-r16");
      srs_ul_cfg_ded_add_r16->to_json(j);
    }
    if (ul_pwr_ctrl_add_srs_r16.is_present()) {
      j.write_fieldname("uplinkPowerControlAddSRS-r16");
      ul_pwr_ctrl_add_srs_r16->to_json(j);
    }
    if (srs_virtual_cell_id_r16.is_present()) {
      j.write_fieldname("soundingRS-VirtualCellID-r16");
      srs_virtual_cell_id_r16->to_json(j);
    }
    if (wideband_prg_r16.is_present()) {
      j.write_fieldname("widebandPRG-r16");
      wideband_prg_r16->to_json(j);
    }
    if (pdsch_cfg_ded_v1700.is_present()) {
      j.write_fieldname("pdsch-ConfigDedicated-v1700");
      pdsch_cfg_ded_v1700->to_json(j);
    }
    if (ntn_cfg_ded_r17.is_present()) {
      j.write_fieldname("ntn-ConfigDedicated-r17");
      j.start_obj();
      if (ntn_cfg_ded_r17->pucch_tx_dur_r17_present) {
        j.write_fieldname("pucch-TxDuration-r17");
        ntn_cfg_ded_r17->pucch_tx_dur_r17.to_json(j);
      }
      if (ntn_cfg_ded_r17->pusch_tx_dur_r17_present) {
        j.write_fieldname("pusch-TxDuration-r17");
        ntn_cfg_ded_r17->pusch_tx_dur_r17.to_json(j);
      }
      j.end_obj();
    }
    if (ul_segmented_precompensation_gap_r17_present) {
      j.write_str("uplinkSegmentedPrecompensationGap-r17", ul_segmented_precompensation_gap_r17.to_string());
    }
  }
  j.end_obj();
}

void phys_cfg_ded_s::ant_info_c_::set(types::options e)
{
  type_ = e;
}
ant_info_ded_s& phys_cfg_ded_s::ant_info_c_::set_explicit_value()
{
  set(types::explicit_value);
  return c;
}
void phys_cfg_ded_s::ant_info_c_::set_default_value()
{
  set(types::default_value);
}
void phys_cfg_ded_s::ant_info_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::explicit_value:
      j.write_fieldname("explicitValue");
      c.to_json(j);
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::ant_info_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_s::ant_info_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::explicit_value:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::ant_info_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_s::ant_info_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::explicit_value:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::ant_info_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void phys_cfg_ded_s::ant_info_r10_c_::set(types::options e)
{
  type_ = e;
}
ant_info_ded_r10_s& phys_cfg_ded_s::ant_info_r10_c_::set_explicit_value_r10()
{
  set(types::explicit_value_r10);
  return c;
}
void phys_cfg_ded_s::ant_info_r10_c_::set_default_value()
{
  set(types::default_value);
}
void phys_cfg_ded_s::ant_info_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::explicit_value_r10:
      j.write_fieldname("explicitValue-r10");
      c.to_json(j);
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::ant_info_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_s::ant_info_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::explicit_value_r10:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::ant_info_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_s::ant_info_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::explicit_value_r10:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::ant_info_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void phys_cfg_ded_s::add_spec_emission_ca_r10_c_::set(types::options e)
{
  type_ = e;
}
void phys_cfg_ded_s::add_spec_emission_ca_r10_c_::set_release()
{
  set(types::release);
}
phys_cfg_ded_s::add_spec_emission_ca_r10_c_::setup_s_& phys_cfg_ded_s::add_spec_emission_ca_r10_c_::set_setup()
{
  set(types::setup);
  return c;
}
void phys_cfg_ded_s::add_spec_emission_ca_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("additionalSpectrumEmissionPCell-r10", c.add_spec_emission_pcell_r10);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::add_spec_emission_ca_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_s::add_spec_emission_ca_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c.add_spec_emission_pcell_r10, (uint8_t)1u, (uint8_t)32u));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::add_spec_emission_ca_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_s::add_spec_emission_ca_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c.add_spec_emission_pcell_r10, bref, (uint8_t)1u, (uint8_t)32u));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::add_spec_emission_ca_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void phys_cfg_ded_s::ce_mode_r13_c_::set(types::options e)
{
  type_ = e;
}
void phys_cfg_ded_s::ce_mode_r13_c_::set_release()
{
  set(types::release);
}
phys_cfg_ded_s::ce_mode_r13_c_::setup_e_& phys_cfg_ded_s::ce_mode_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void phys_cfg_ded_s::ce_mode_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_str("setup", c.to_string());
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::ce_mode_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_s::ce_mode_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::ce_mode_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_s::ce_mode_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::ce_mode_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* phys_cfg_ded_s::ce_mode_r13_c_::setup_opts::to_string() const
{
  static const char* options[] = {"ce-ModeA", "ce-ModeB"};
  return convert_enum_idx(options, 2, value, "phys_cfg_ded_s::ce_mode_r13_c_::setup_e_");
}

void phys_cfg_ded_s::type_a_srs_tpc_pdcch_group_r14_c_::set(types::options e)
{
  type_ = e;
}
void phys_cfg_ded_s::type_a_srs_tpc_pdcch_group_r14_c_::set_release()
{
  set(types::release);
}
phys_cfg_ded_s::type_a_srs_tpc_pdcch_group_r14_c_::setup_l_&
phys_cfg_ded_s::type_a_srs_tpc_pdcch_group_r14_c_::set_setup()
{
  set(types::setup);
  return c;
}
void phys_cfg_ded_s::type_a_srs_tpc_pdcch_group_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.start_array("setup");
      for (const auto& e1 : c) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::type_a_srs_tpc_pdcch_group_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_s::type_a_srs_tpc_pdcch_group_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 32));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::type_a_srs_tpc_pdcch_group_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_s::type_a_srs_tpc_pdcch_group_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 32));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::type_a_srs_tpc_pdcch_group_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void phys_cfg_ded_s::must_cfg_r14_c_::set(types::options e)
{
  type_ = e;
}
void phys_cfg_ded_s::must_cfg_r14_c_::set_release()
{
  set(types::release);
}
phys_cfg_ded_s::must_cfg_r14_c_::setup_s_& phys_cfg_ded_s::must_cfg_r14_c_::set_setup()
{
  set(types::setup);
  return c;
}
void phys_cfg_ded_s::must_cfg_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("k-max-r14", c.k_max_r14.to_string());
      if (c.p_a_must_r14_present) {
        j.write_str("p-a-must-r14", c.p_a_must_r14.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::must_cfg_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_s::must_cfg_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.p_a_must_r14_present, 1));
      HANDLE_CODE(c.k_max_r14.pack(bref));
      if (c.p_a_must_r14_present) {
        HANDLE_CODE(c.p_a_must_r14.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::must_cfg_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_s::must_cfg_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.p_a_must_r14_present, 1));
      HANDLE_CODE(c.k_max_r14.unpack(bref));
      if (c.p_a_must_r14_present) {
        HANDLE_CODE(c.p_a_must_r14.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::must_cfg_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::k_max_r14_opts::to_string() const
{
  static const char* options[] = {"l1", "l3"};
  return convert_enum_idx(options, 2, value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_");
}
uint8_t phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::k_max_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 3};
  return map_enum_number(options, 2, value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_");
}

const char* phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4dot77", "dB-3", "dB-1dot77", "dB0", "dB1", "dB2", "dB3"};
  return convert_enum_idx(options, 8, value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
}
float phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_opts::to_number() const
{
  static const float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return map_enum_number(options, 8, value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
}
const char* phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_opts::to_number_string() const
{
  static const char* options[] = {"-6", "-4.77", "-3", "-1.77", "0", "1", "2", "3"};
  return convert_enum_idx(options, 8, value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
}

void phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::set(types::options e)
{
  type_ = e;
}
void phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::set_release()
{
  set(types::release);
}
phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_& phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::destroy_()
{
  switch (type_) {
    case types::cfi_cfg_r15:
      c.destroy<cfi_cfg_r15_s>();
      break;
    case types::cfi_pattern_cfg_r15:
      c.destroy<cfi_pattern_cfg_r15_s>();
      break;
    default:
      break;
  }
}
void phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cfi_cfg_r15:
      c.init<cfi_cfg_r15_s>();
      break;
    case types::cfi_pattern_cfg_r15:
      c.init<cfi_pattern_cfg_r15_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_");
  }
}
phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::setup_c_(
    const phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cfi_cfg_r15:
      c.init(other.c.get<cfi_cfg_r15_s>());
      break;
    case types::cfi_pattern_cfg_r15:
      c.init(other.c.get<cfi_pattern_cfg_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_");
  }
}
phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_& phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::operator=(
    const phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cfi_cfg_r15:
      c.set(other.c.get<cfi_cfg_r15_s>());
      break;
    case types::cfi_pattern_cfg_r15:
      c.set(other.c.get<cfi_pattern_cfg_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_");
  }

  return *this;
}
cfi_cfg_r15_s& phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::set_cfi_cfg_r15()
{
  set(types::cfi_cfg_r15);
  return c.get<cfi_cfg_r15_s>();
}
cfi_pattern_cfg_r15_s& phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::set_cfi_pattern_cfg_r15()
{
  set(types::cfi_pattern_cfg_r15);
  return c.get<cfi_pattern_cfg_r15_s>();
}
void phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cfi_cfg_r15:
      j.write_fieldname("cfi-Config-r15");
      c.get<cfi_cfg_r15_s>().to_json(j);
      break;
    case types::cfi_pattern_cfg_r15:
      j.write_fieldname("cfi-PatternConfig-r15");
      c.get<cfi_pattern_cfg_r15_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cfi_cfg_r15:
      HANDLE_CODE(c.get<cfi_cfg_r15_s>().pack(bref));
      break;
    case types::cfi_pattern_cfg_r15:
      HANDLE_CODE(c.get<cfi_pattern_cfg_r15_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cfi_cfg_r15:
      HANDLE_CODE(c.get<cfi_cfg_r15_s>().unpack(bref));
      break;
    case types::cfi_pattern_cfg_r15:
      HANDLE_CODE(c.get<cfi_pattern_cfg_r15_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::set(types::options e)
{
  type_ = e;
}
void phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::set_release()
{
  set(types::release);
}
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_& phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_bool("blindSubframePDSCH-Repetitions-r15", c.blind_sf_pdsch_repeats_r15);
      j.write_bool("blindSlotSubslotPDSCH-Repetitions-r15", c.blind_slot_subslot_pdsch_repeats_r15);
      if (c.max_num_sf_pdsch_repeats_r15_present) {
        j.write_str("maxNumber-SubframePDSCH-Repetitions-r15", c.max_num_sf_pdsch_repeats_r15.to_string());
      }
      if (c.max_num_slot_subslot_pdsch_repeats_r15_present) {
        j.write_str("maxNumber-SlotSubslotPDSCH-Repetitions-r15", c.max_num_slot_subslot_pdsch_repeats_r15.to_string());
      }
      if (c.rv_sf_pdsch_repeats_r15_present) {
        j.write_str("rv-SubframePDSCH-Repetitions-r15", c.rv_sf_pdsch_repeats_r15.to_string());
      }
      if (c.rv_slotsublot_pdsch_repeats_r15_present) {
        j.write_str("rv-SlotsublotPDSCH-Repetitions-r15", c.rv_slotsublot_pdsch_repeats_r15.to_string());
      }
      if (c.nof_processes_sf_pdsch_repeats_r15_present) {
        j.write_int("numberOfProcesses-SubframePDSCH-Repetitions-r15", c.nof_processes_sf_pdsch_repeats_r15);
      }
      if (c.nof_processes_slot_subslot_pdsch_repeats_r15_present) {
        j.write_int("numberOfProcesses-SlotSubslotPDSCH-Repetitions-r15",
                    c.nof_processes_slot_subslot_pdsch_repeats_r15);
      }
      if (c.mcs_restrict_sf_pdsch_repeats_r15_present) {
        j.write_str("mcs-restrictionSubframePDSCH-Repetitions-r15", c.mcs_restrict_sf_pdsch_repeats_r15.to_string());
      }
      if (c.mcs_restrict_slot_subslot_pdsch_repeats_r15_present) {
        j.write_str("mcs-restrictionSlotSubslotPDSCH-Repetitions-r15",
                    c.mcs_restrict_slot_subslot_pdsch_repeats_r15.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.max_num_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.max_num_slot_subslot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.rv_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.rv_slotsublot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.nof_processes_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.nof_processes_slot_subslot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.mcs_restrict_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.mcs_restrict_slot_subslot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.blind_sf_pdsch_repeats_r15, 1));
      HANDLE_CODE(bref.pack(c.blind_slot_subslot_pdsch_repeats_r15, 1));
      if (c.max_num_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.max_num_sf_pdsch_repeats_r15.pack(bref));
      }
      if (c.max_num_slot_subslot_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.max_num_slot_subslot_pdsch_repeats_r15.pack(bref));
      }
      if (c.rv_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.rv_sf_pdsch_repeats_r15.pack(bref));
      }
      if (c.rv_slotsublot_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.rv_slotsublot_pdsch_repeats_r15.pack(bref));
      }
      if (c.nof_processes_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.nof_processes_sf_pdsch_repeats_r15, (uint8_t)1u, (uint8_t)16u));
      }
      if (c.nof_processes_slot_subslot_pdsch_repeats_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.nof_processes_slot_subslot_pdsch_repeats_r15, (uint8_t)1u, (uint8_t)16u));
      }
      if (c.mcs_restrict_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.mcs_restrict_sf_pdsch_repeats_r15.pack(bref));
      }
      if (c.mcs_restrict_slot_subslot_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.mcs_restrict_slot_subslot_pdsch_repeats_r15.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.max_num_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.max_num_slot_subslot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.rv_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.rv_slotsublot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.nof_processes_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.nof_processes_slot_subslot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.mcs_restrict_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.mcs_restrict_slot_subslot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.blind_sf_pdsch_repeats_r15, 1));
      HANDLE_CODE(bref.unpack(c.blind_slot_subslot_pdsch_repeats_r15, 1));
      if (c.max_num_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.max_num_sf_pdsch_repeats_r15.unpack(bref));
      }
      if (c.max_num_slot_subslot_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.max_num_slot_subslot_pdsch_repeats_r15.unpack(bref));
      }
      if (c.rv_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.rv_sf_pdsch_repeats_r15.unpack(bref));
      }
      if (c.rv_slotsublot_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.rv_slotsublot_pdsch_repeats_r15.unpack(bref));
      }
      if (c.nof_processes_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(unpack_integer(c.nof_processes_sf_pdsch_repeats_r15, bref, (uint8_t)1u, (uint8_t)16u));
      }
      if (c.nof_processes_slot_subslot_pdsch_repeats_r15_present) {
        HANDLE_CODE(unpack_integer(c.nof_processes_slot_subslot_pdsch_repeats_r15, bref, (uint8_t)1u, (uint8_t)16u));
      }
      if (c.mcs_restrict_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.mcs_restrict_sf_pdsch_repeats_r15.unpack(bref));
      }
      if (c.mcs_restrict_slot_subslot_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.mcs_restrict_slot_subslot_pdsch_repeats_r15.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char*
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n4", "n6"};
  return convert_enum_idx(
      options, 2, value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_");
}
uint8_t phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {4, 6};
  return map_enum_number(
      options, 2, value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_");
}

const char*
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n4", "n6"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_e_");
}
uint8_t
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {4, 6};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_e_");
}

const char* phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"dlrvseq1", "dlrvseq2"};
  return convert_enum_idx(
      options, 2, value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_");
}
uint8_t phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(
      options, 2, value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_");
}

const char*
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"dlrvseq1", "dlrvseq2"};
  return convert_enum_idx(
      options, 2, value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_");
}
uint8_t phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(
      options, 2, value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_");
}

const char*
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n0", "n1"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_");
}
uint8_t
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_");
}

const char*
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_pdsch_repeats_r15_opts::to_string()
    const
{
  static const char* options[] = {"n0", "n1"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_pdsch_repeats_r15_e_");
}
uint8_t
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_pdsch_repeats_r15_opts::to_number()
    const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_pdsch_repeats_r15_e_");
}

const char* phys_cfg_ded_s::ul_segmented_precompensation_gap_r17_opts::to_string() const
{
  static const char* options[] = {"sym1", "sl1", "sf1"};
  return convert_enum_idx(options, 3, value, "phys_cfg_ded_s::ul_segmented_precompensation_gap_r17_e_");
}
