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

#include "srsran/asn1/rrc/meascfg.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// Q-OffsetRange ::= ENUMERATED
const char* q_offset_range_opts::to_string() const
{
  static const char* options[] = {"dB-24", "dB-22", "dB-20", "dB-18", "dB-16", "dB-14", "dB-12", "dB-10",
                                  "dB-8",  "dB-6",  "dB-5",  "dB-4",  "dB-3",  "dB-2",  "dB-1",  "dB0",
                                  "dB1",   "dB2",   "dB3",   "dB4",   "dB5",   "dB6",   "dB8",   "dB10",
                                  "dB12",  "dB14",  "dB16",  "dB18",  "dB20",  "dB22",  "dB24"};
  return convert_enum_idx(options, 31, value, "q_offset_range_e");
}
int8_t q_offset_range_opts::to_number() const
{
  static const int8_t options[] = {-24, -22, -20, -18, -16, -14, -12, -10, -8, -6, -5, -4, -3, -2, -1, 0,
                                   1,   2,   3,   4,   5,   6,   8,   10,  12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(options, 31, value, "q_offset_range_e");
}

// SpeedStateScaleFactors ::= SEQUENCE
SRSASN_CODE speed_state_scale_factors_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sf_medium.pack(bref));
  HANDLE_CODE(sf_high.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE speed_state_scale_factors_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sf_medium.unpack(bref));
  HANDLE_CODE(sf_high.unpack(bref));

  return SRSASN_SUCCESS;
}
void speed_state_scale_factors_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sf-Medium", sf_medium.to_string());
  j.write_str("sf-High", sf_high.to_string());
  j.end_obj();
}

const char* speed_state_scale_factors_s::sf_medium_opts::to_string() const
{
  static const char* options[] = {"oDot25", "oDot5", "oDot75", "lDot0"};
  return convert_enum_idx(options, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}
float speed_state_scale_factors_s::sf_medium_opts::to_number() const
{
  static const float options[] = {0.25, 0.5, 0.75, 1.0};
  return map_enum_number(options, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}
const char* speed_state_scale_factors_s::sf_medium_opts::to_number_string() const
{
  static const char* options[] = {"0.25", "0.5", "0.75", "1.0"};
  return convert_enum_idx(options, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}

const char* speed_state_scale_factors_s::sf_high_opts::to_string() const
{
  static const char* options[] = {"oDot25", "oDot5", "oDot75", "lDot0"};
  return convert_enum_idx(options, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}
float speed_state_scale_factors_s::sf_high_opts::to_number() const
{
  static const float options[] = {0.25, 0.5, 0.75, 1.0};
  return map_enum_number(options, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}
const char* speed_state_scale_factors_s::sf_high_opts::to_number_string() const
{
  static const char* options[] = {"0.25", "0.5", "0.75", "1.0"};
  return convert_enum_idx(options, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}

// BandIndicatorGERAN ::= ENUMERATED
const char* band_ind_geran_opts::to_string() const
{
  static const char* options[] = {"dcs1800", "pcs1900"};
  return convert_enum_idx(options, 2, value, "band_ind_geran_e");
}
uint16_t band_ind_geran_opts::to_number() const
{
  static const uint16_t options[] = {1800, 1900};
  return map_enum_number(options, 2, value, "band_ind_geran_e");
}

// PreRegistrationInfoHRPD ::= SEQUENCE
SRSASN_CODE pre_regist_info_hrpd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pre_regist_zone_id_present, 1));
  HANDLE_CODE(bref.pack(secondary_pre_regist_zone_id_list_present, 1));

  HANDLE_CODE(bref.pack(pre_regist_allowed, 1));
  if (pre_regist_zone_id_present) {
    HANDLE_CODE(pack_integer(bref, pre_regist_zone_id, (uint16_t)0u, (uint16_t)255u));
  }
  if (secondary_pre_regist_zone_id_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, secondary_pre_regist_zone_id_list, 1, 2, integer_packer<uint16_t>(0, 255)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pre_regist_info_hrpd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pre_regist_zone_id_present, 1));
  HANDLE_CODE(bref.unpack(secondary_pre_regist_zone_id_list_present, 1));

  HANDLE_CODE(bref.unpack(pre_regist_allowed, 1));
  if (pre_regist_zone_id_present) {
    HANDLE_CODE(unpack_integer(pre_regist_zone_id, bref, (uint16_t)0u, (uint16_t)255u));
  }
  if (secondary_pre_regist_zone_id_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(secondary_pre_regist_zone_id_list, bref, 1, 2, integer_packer<uint16_t>(0, 255)));
  }

  return SRSASN_SUCCESS;
}
void pre_regist_info_hrpd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("preRegistrationAllowed", pre_regist_allowed);
  if (pre_regist_zone_id_present) {
    j.write_int("preRegistrationZoneId", pre_regist_zone_id);
  }
  if (secondary_pre_regist_zone_id_list_present) {
    j.start_array("secondaryPreRegistrationZoneIdList");
    for (const auto& e1 : secondary_pre_regist_zone_id_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// CarrierFreqsGERAN ::= SEQUENCE
SRSASN_CODE carrier_freqs_geran_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, start_arfcn, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(band_ind.pack(bref));
  HANDLE_CODE(following_arfcns.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freqs_geran_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(start_arfcn, bref, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(band_ind.unpack(bref));
  HANDLE_CODE(following_arfcns.unpack(bref));

  return SRSASN_SUCCESS;
}
void carrier_freqs_geran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("startingARFCN", start_arfcn);
  j.write_str("bandIndicator", band_ind.to_string());
  j.write_fieldname("followingARFCNs");
  following_arfcns.to_json(j);
  j.end_obj();
}
bool carrier_freqs_geran_s::operator==(const carrier_freqs_geran_s& other) const
{
  return start_arfcn == other.start_arfcn and band_ind == other.band_ind and following_arfcns == other.following_arfcns;
}

void carrier_freqs_geran_s::following_arfcns_c_::destroy_()
{
  switch (type_) {
    case types::explicit_list_of_arfcns:
      c.destroy<explicit_list_of_arfcns_l>();
      break;
    case types::equally_spaced_arfcns:
      c.destroy<equally_spaced_arfcns_s_>();
      break;
    case types::variable_bit_map_of_arfcns:
      c.destroy<bounded_octstring<1, 16> >();
      break;
    default:
      break;
  }
}
void carrier_freqs_geran_s::following_arfcns_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::explicit_list_of_arfcns:
      c.init<explicit_list_of_arfcns_l>();
      break;
    case types::equally_spaced_arfcns:
      c.init<equally_spaced_arfcns_s_>();
      break;
    case types::variable_bit_map_of_arfcns:
      c.init<bounded_octstring<1, 16> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "carrier_freqs_geran_s::following_arfcns_c_");
  }
}
carrier_freqs_geran_s::following_arfcns_c_::following_arfcns_c_(const carrier_freqs_geran_s::following_arfcns_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::explicit_list_of_arfcns:
      c.init(other.c.get<explicit_list_of_arfcns_l>());
      break;
    case types::equally_spaced_arfcns:
      c.init(other.c.get<equally_spaced_arfcns_s_>());
      break;
    case types::variable_bit_map_of_arfcns:
      c.init(other.c.get<bounded_octstring<1, 16> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "carrier_freqs_geran_s::following_arfcns_c_");
  }
}
carrier_freqs_geran_s::following_arfcns_c_&
carrier_freqs_geran_s::following_arfcns_c_::operator=(const carrier_freqs_geran_s::following_arfcns_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::explicit_list_of_arfcns:
      c.set(other.c.get<explicit_list_of_arfcns_l>());
      break;
    case types::equally_spaced_arfcns:
      c.set(other.c.get<equally_spaced_arfcns_s_>());
      break;
    case types::variable_bit_map_of_arfcns:
      c.set(other.c.get<bounded_octstring<1, 16> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "carrier_freqs_geran_s::following_arfcns_c_");
  }

  return *this;
}
explicit_list_of_arfcns_l& carrier_freqs_geran_s::following_arfcns_c_::set_explicit_list_of_arfcns()
{
  set(types::explicit_list_of_arfcns);
  return c.get<explicit_list_of_arfcns_l>();
}
carrier_freqs_geran_s::following_arfcns_c_::equally_spaced_arfcns_s_&
carrier_freqs_geran_s::following_arfcns_c_::set_equally_spaced_arfcns()
{
  set(types::equally_spaced_arfcns);
  return c.get<equally_spaced_arfcns_s_>();
}
bounded_octstring<1, 16>& carrier_freqs_geran_s::following_arfcns_c_::set_variable_bit_map_of_arfcns()
{
  set(types::variable_bit_map_of_arfcns);
  return c.get<bounded_octstring<1, 16> >();
}
void carrier_freqs_geran_s::following_arfcns_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::explicit_list_of_arfcns:
      j.start_array("explicitListOfARFCNs");
      for (const auto& e1 : c.get<explicit_list_of_arfcns_l>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::equally_spaced_arfcns:
      j.write_fieldname("equallySpacedARFCNs");
      j.start_obj();
      j.write_int("arfcn-Spacing", c.get<equally_spaced_arfcns_s_>().arfcn_spacing);
      j.write_int("numberOfFollowingARFCNs", c.get<equally_spaced_arfcns_s_>().nof_following_arfcns);
      j.end_obj();
      break;
    case types::variable_bit_map_of_arfcns:
      j.write_str("variableBitMapOfARFCNs", c.get<bounded_octstring<1, 16> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "carrier_freqs_geran_s::following_arfcns_c_");
  }
  j.end_obj();
}
SRSASN_CODE carrier_freqs_geran_s::following_arfcns_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::explicit_list_of_arfcns:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<explicit_list_of_arfcns_l>(), 0, 31, integer_packer<uint16_t>(0, 1023)));
      break;
    case types::equally_spaced_arfcns:
      HANDLE_CODE(pack_integer(bref, c.get<equally_spaced_arfcns_s_>().arfcn_spacing, (uint8_t)1u, (uint8_t)8u));
      HANDLE_CODE(
          pack_integer(bref, c.get<equally_spaced_arfcns_s_>().nof_following_arfcns, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::variable_bit_map_of_arfcns:
      HANDLE_CODE((c.get<bounded_octstring<1, 16> >().pack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "carrier_freqs_geran_s::following_arfcns_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freqs_geran_s::following_arfcns_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::explicit_list_of_arfcns:
      HANDLE_CODE(
          unpack_dyn_seq_of(c.get<explicit_list_of_arfcns_l>(), bref, 0, 31, integer_packer<uint16_t>(0, 1023)));
      break;
    case types::equally_spaced_arfcns:
      HANDLE_CODE(unpack_integer(c.get<equally_spaced_arfcns_s_>().arfcn_spacing, bref, (uint8_t)1u, (uint8_t)8u));
      HANDLE_CODE(
          unpack_integer(c.get<equally_spaced_arfcns_s_>().nof_following_arfcns, bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::variable_bit_map_of_arfcns:
      HANDLE_CODE((c.get<bounded_octstring<1, 16> >().unpack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "carrier_freqs_geran_s::following_arfcns_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool carrier_freqs_geran_s::following_arfcns_c_::operator==(const following_arfcns_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::explicit_list_of_arfcns:
      return c.get<explicit_list_of_arfcns_l>() == other.c.get<explicit_list_of_arfcns_l>();
    case types::equally_spaced_arfcns:
      return c.get<equally_spaced_arfcns_s_>().arfcn_spacing ==
                 other.c.get<equally_spaced_arfcns_s_>().arfcn_spacing and
             c.get<equally_spaced_arfcns_s_>().nof_following_arfcns ==
                 other.c.get<equally_spaced_arfcns_s_>().nof_following_arfcns;
    case types::variable_bit_map_of_arfcns:
      return c.get<bounded_octstring<1, 16> >() == other.c.get<bounded_octstring<1, 16> >();
    default:
      return true;
  }
  return true;
}

// CellReselectionSubPriority-r13 ::= ENUMERATED
const char* cell_resel_sub_prio_r13_opts::to_string() const
{
  static const char* options[] = {"oDot2", "oDot4", "oDot6", "oDot8"};
  return convert_enum_idx(options, 4, value, "cell_resel_sub_prio_r13_e");
}
float cell_resel_sub_prio_r13_opts::to_number() const
{
  static const float options[] = {0.2, 0.4, 0.6, 0.8};
  return map_enum_number(options, 4, value, "cell_resel_sub_prio_r13_e");
}
const char* cell_resel_sub_prio_r13_opts::to_number_string() const
{
  static const char* options[] = {"0.2", "0.4", "0.6", "0.8"};
  return convert_enum_idx(options, 4, value, "cell_resel_sub_prio_r13_e");
}

// MTC-SSB-NR-r15 ::= SEQUENCE
SRSASN_CODE mtc_ssb_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(periodicity_and_offset_r15.pack(bref));
  HANDLE_CODE(ssb_dur_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mtc_ssb_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(periodicity_and_offset_r15.unpack(bref));
  HANDLE_CODE(ssb_dur_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void mtc_ssb_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("periodicityAndOffset-r15");
  periodicity_and_offset_r15.to_json(j);
  j.write_str("ssb-Duration-r15", ssb_dur_r15.to_string());
  j.end_obj();
}
bool mtc_ssb_nr_r15_s::operator==(const mtc_ssb_nr_r15_s& other) const
{
  return periodicity_and_offset_r15 == other.periodicity_and_offset_r15 and ssb_dur_r15 == other.ssb_dur_r15;
}

void mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::destroy_() {}
void mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::periodicity_and_offset_r15_c_(
    const mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf5_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf10_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf20_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf40_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf80_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf160_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_");
  }
}
mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_&
mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::operator=(const mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf5_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf10_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf20_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf40_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf80_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf160_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_");
  }

  return *this;
}
uint8_t& mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::set_sf5_r15()
{
  set(types::sf5_r15);
  return c.get<uint8_t>();
}
uint8_t& mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::set_sf10_r15()
{
  set(types::sf10_r15);
  return c.get<uint8_t>();
}
uint8_t& mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::set_sf20_r15()
{
  set(types::sf20_r15);
  return c.get<uint8_t>();
}
uint8_t& mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::set_sf40_r15()
{
  set(types::sf40_r15);
  return c.get<uint8_t>();
}
uint8_t& mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::set_sf80_r15()
{
  set(types::sf80_r15);
  return c.get<uint8_t>();
}
uint8_t& mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::set_sf160_r15()
{
  set(types::sf160_r15);
  return c.get<uint8_t>();
}
void mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf5_r15:
      j.write_int("sf5-r15", c.get<uint8_t>());
      break;
    case types::sf10_r15:
      j.write_int("sf10-r15", c.get<uint8_t>());
      break;
    case types::sf20_r15:
      j.write_int("sf20-r15", c.get<uint8_t>());
      break;
    case types::sf40_r15:
      j.write_int("sf40-r15", c.get<uint8_t>());
      break;
    case types::sf80_r15:
      j.write_int("sf80-r15", c.get<uint8_t>());
      break;
    case types::sf160_r15:
      j.write_int("sf160-r15", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf5_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)4u));
      break;
    case types::sf10_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf40_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf80_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf160_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    default:
      log_invalid_choice_id(type_, "mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf5_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)4u));
      break;
    case types::sf10_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf40_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf80_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf160_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    default:
      log_invalid_choice_id(type_, "mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::operator==(const periodicity_and_offset_r15_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::sf5_r15:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::sf10_r15:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::sf20_r15:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::sf40_r15:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::sf80_r15:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::sf160_r15:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    default:
      return true;
  }
  return true;
}

const char* mtc_ssb_nr_r15_s::ssb_dur_r15_opts::to_string() const
{
  static const char* options[] = {"sf1", "sf2", "sf3", "sf4", "sf5"};
  return convert_enum_idx(options, 5, value, "mtc_ssb_nr_r15_s::ssb_dur_r15_e_");
}
uint8_t mtc_ssb_nr_r15_s::ssb_dur_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5};
  return map_enum_number(options, 5, value, "mtc_ssb_nr_r15_s::ssb_dur_r15_e_");
}

// SS-RSSI-Measurement-r15 ::= SEQUENCE
SRSASN_CODE ss_rssi_meas_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(meas_slots_r15.pack(bref));
  HANDLE_CODE(pack_integer(bref, end_symbol_r15, (uint8_t)0u, (uint8_t)3u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ss_rssi_meas_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(meas_slots_r15.unpack(bref));
  HANDLE_CODE(unpack_integer(end_symbol_r15, bref, (uint8_t)0u, (uint8_t)3u));

  return SRSASN_SUCCESS;
}
void ss_rssi_meas_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("measurementSlots-r15", meas_slots_r15.to_string());
  j.write_int("endSymbol-r15", end_symbol_r15);
  j.end_obj();
}
bool ss_rssi_meas_r15_s::operator==(const ss_rssi_meas_r15_s& other) const
{
  return meas_slots_r15 == other.meas_slots_r15 and end_symbol_r15 == other.end_symbol_r15;
}

// SSB-ToMeasure-r15 ::= CHOICE
void ssb_to_measure_r15_c::destroy_()
{
  switch (type_) {
    case types::short_bitmap_r15:
      c.destroy<fixed_bitstring<4> >();
      break;
    case types::medium_bitmap_r15:
      c.destroy<fixed_bitstring<8> >();
      break;
    case types::long_bitmap_r15:
      c.destroy<fixed_bitstring<64> >();
      break;
    default:
      break;
  }
}
void ssb_to_measure_r15_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::short_bitmap_r15:
      c.init<fixed_bitstring<4> >();
      break;
    case types::medium_bitmap_r15:
      c.init<fixed_bitstring<8> >();
      break;
    case types::long_bitmap_r15:
      c.init<fixed_bitstring<64> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ssb_to_measure_r15_c");
  }
}
ssb_to_measure_r15_c::ssb_to_measure_r15_c(const ssb_to_measure_r15_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::short_bitmap_r15:
      c.init(other.c.get<fixed_bitstring<4> >());
      break;
    case types::medium_bitmap_r15:
      c.init(other.c.get<fixed_bitstring<8> >());
      break;
    case types::long_bitmap_r15:
      c.init(other.c.get<fixed_bitstring<64> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ssb_to_measure_r15_c");
  }
}
ssb_to_measure_r15_c& ssb_to_measure_r15_c::operator=(const ssb_to_measure_r15_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::short_bitmap_r15:
      c.set(other.c.get<fixed_bitstring<4> >());
      break;
    case types::medium_bitmap_r15:
      c.set(other.c.get<fixed_bitstring<8> >());
      break;
    case types::long_bitmap_r15:
      c.set(other.c.get<fixed_bitstring<64> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ssb_to_measure_r15_c");
  }

  return *this;
}
fixed_bitstring<4>& ssb_to_measure_r15_c::set_short_bitmap_r15()
{
  set(types::short_bitmap_r15);
  return c.get<fixed_bitstring<4> >();
}
fixed_bitstring<8>& ssb_to_measure_r15_c::set_medium_bitmap_r15()
{
  set(types::medium_bitmap_r15);
  return c.get<fixed_bitstring<8> >();
}
fixed_bitstring<64>& ssb_to_measure_r15_c::set_long_bitmap_r15()
{
  set(types::long_bitmap_r15);
  return c.get<fixed_bitstring<64> >();
}
void ssb_to_measure_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::short_bitmap_r15:
      j.write_str("shortBitmap-r15", c.get<fixed_bitstring<4> >().to_string());
      break;
    case types::medium_bitmap_r15:
      j.write_str("mediumBitmap-r15", c.get<fixed_bitstring<8> >().to_string());
      break;
    case types::long_bitmap_r15:
      j.write_str("longBitmap-r15", c.get<fixed_bitstring<64> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ssb_to_measure_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE ssb_to_measure_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::short_bitmap_r15:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().pack(bref));
      break;
    case types::medium_bitmap_r15:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().pack(bref));
      break;
    case types::long_bitmap_r15:
      HANDLE_CODE(c.get<fixed_bitstring<64> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ssb_to_measure_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_to_measure_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::short_bitmap_r15:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().unpack(bref));
      break;
    case types::medium_bitmap_r15:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().unpack(bref));
      break;
    case types::long_bitmap_r15:
      HANDLE_CODE(c.get<fixed_bitstring<64> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ssb_to_measure_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool ssb_to_measure_r15_c::operator==(const ssb_to_measure_r15_c& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::short_bitmap_r15:
      return c.get<fixed_bitstring<4> >() == other.c.get<fixed_bitstring<4> >();
    case types::medium_bitmap_r15:
      return c.get<fixed_bitstring<8> >() == other.c.get<fixed_bitstring<8> >();
    case types::long_bitmap_r15:
      return c.get<fixed_bitstring<64> >() == other.c.get<fixed_bitstring<64> >();
    default:
      return true;
  }
  return true;
}

// ThresholdListNR-r15 ::= SEQUENCE
SRSASN_CODE thres_list_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nr_rsrp_r15_present, 1));
  HANDLE_CODE(bref.pack(nr_rsrq_r15_present, 1));
  HANDLE_CODE(bref.pack(nr_sinr_r15_present, 1));

  if (nr_rsrp_r15_present) {
    HANDLE_CODE(pack_integer(bref, nr_rsrp_r15, (uint8_t)0u, (uint8_t)127u));
  }
  if (nr_rsrq_r15_present) {
    HANDLE_CODE(pack_integer(bref, nr_rsrq_r15, (uint8_t)0u, (uint8_t)127u));
  }
  if (nr_sinr_r15_present) {
    HANDLE_CODE(pack_integer(bref, nr_sinr_r15, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE thres_list_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nr_rsrp_r15_present, 1));
  HANDLE_CODE(bref.unpack(nr_rsrq_r15_present, 1));
  HANDLE_CODE(bref.unpack(nr_sinr_r15_present, 1));

  if (nr_rsrp_r15_present) {
    HANDLE_CODE(unpack_integer(nr_rsrp_r15, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (nr_rsrq_r15_present) {
    HANDLE_CODE(unpack_integer(nr_rsrq_r15, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (nr_sinr_r15_present) {
    HANDLE_CODE(unpack_integer(nr_sinr_r15, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void thres_list_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nr_rsrp_r15_present) {
    j.write_int("nr-RSRP-r15", nr_rsrp_r15);
  }
  if (nr_rsrq_r15_present) {
    j.write_int("nr-RSRQ-r15", nr_rsrq_r15);
  }
  if (nr_sinr_r15_present) {
    j.write_int("nr-SINR-r15", nr_sinr_r15);
  }
  j.end_obj();
}
bool thres_list_nr_r15_s::operator==(const thres_list_nr_r15_s& other) const
{
  return nr_rsrp_r15_present == other.nr_rsrp_r15_present and
         (not nr_rsrp_r15_present or nr_rsrp_r15 == other.nr_rsrp_r15) and
         nr_rsrq_r15_present == other.nr_rsrq_r15_present and
         (not nr_rsrq_r15_present or nr_rsrq_r15 == other.nr_rsrq_r15) and
         nr_sinr_r15_present == other.nr_sinr_r15_present and
         (not nr_sinr_r15_present or nr_sinr_r15 == other.nr_sinr_r15);
}

// MobilityStateParameters ::= SEQUENCE
SRSASN_CODE mob_state_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(t_eval.pack(bref));
  HANDLE_CODE(t_hyst_normal.pack(bref));
  HANDLE_CODE(pack_integer(bref, n_cell_change_medium, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(pack_integer(bref, n_cell_change_high, (uint8_t)1u, (uint8_t)16u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_state_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(t_eval.unpack(bref));
  HANDLE_CODE(t_hyst_normal.unpack(bref));
  HANDLE_CODE(unpack_integer(n_cell_change_medium, bref, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(unpack_integer(n_cell_change_high, bref, (uint8_t)1u, (uint8_t)16u));

  return SRSASN_SUCCESS;
}
void mob_state_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t-Evaluation", t_eval.to_string());
  j.write_str("t-HystNormal", t_hyst_normal.to_string());
  j.write_int("n-CellChangeMedium", n_cell_change_medium);
  j.write_int("n-CellChangeHigh", n_cell_change_high);
  j.end_obj();
}

const char* mob_state_params_s::t_eval_opts::to_string() const
{
  static const char* options[] = {"s30", "s60", "s120", "s180", "s240", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "mob_state_params_s::t_eval_e_");
}
uint8_t mob_state_params_s::t_eval_opts::to_number() const
{
  static const uint8_t options[] = {30, 60, 120, 180, 240};
  return map_enum_number(options, 5, value, "mob_state_params_s::t_eval_e_");
}

const char* mob_state_params_s::t_hyst_normal_opts::to_string() const
{
  static const char* options[] = {"s30", "s60", "s120", "s180", "s240", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "mob_state_params_s::t_hyst_normal_e_");
}
uint8_t mob_state_params_s::t_hyst_normal_opts::to_number() const
{
  static const uint8_t options[] = {30, 60, 120, 180, 240};
  return map_enum_number(options, 5, value, "mob_state_params_s::t_hyst_normal_e_");
}

// CarrierFreqCDMA2000 ::= SEQUENCE
SRSASN_CODE carrier_freq_cdma2000_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(band_class.pack(bref));
  HANDLE_CODE(pack_integer(bref, arfcn, (uint16_t)0u, (uint16_t)2047u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_cdma2000_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(band_class.unpack(bref));
  HANDLE_CODE(unpack_integer(arfcn, bref, (uint16_t)0u, (uint16_t)2047u));

  return SRSASN_SUCCESS;
}
void carrier_freq_cdma2000_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("bandClass", band_class.to_string());
  j.write_int("arfcn", arfcn);
  j.end_obj();
}
bool carrier_freq_cdma2000_s::operator==(const carrier_freq_cdma2000_s& other) const
{
  return band_class == other.band_class and arfcn == other.arfcn;
}

// MeasCSI-RS-Config-r12 ::= SEQUENCE
SRSASN_CODE meas_csi_rs_cfg_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, meas_csi_rs_id_r12, (uint8_t)1u, (uint8_t)96u));
  HANDLE_CODE(pack_integer(bref, pci_r12, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(pack_integer(bref, scrambling_id_r12, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(pack_integer(bref, res_cfg_r12, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, sf_offset_r12, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(csi_rs_individual_offset_r12.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_csi_rs_cfg_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(meas_csi_rs_id_r12, bref, (uint8_t)1u, (uint8_t)96u));
  HANDLE_CODE(unpack_integer(pci_r12, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(unpack_integer(scrambling_id_r12, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(unpack_integer(res_cfg_r12, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(sf_offset_r12, bref, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(csi_rs_individual_offset_r12.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_csi_rs_cfg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measCSI-RS-Id-r12", meas_csi_rs_id_r12);
  j.write_int("physCellId-r12", pci_r12);
  j.write_int("scramblingIdentity-r12", scrambling_id_r12);
  j.write_int("resourceConfig-r12", res_cfg_r12);
  j.write_int("subframeOffset-r12", sf_offset_r12);
  j.write_str("csi-RS-IndividualOffset-r12", csi_rs_individual_offset_r12.to_string());
  j.end_obj();
}
bool meas_csi_rs_cfg_r12_s::operator==(const meas_csi_rs_cfg_r12_s& other) const
{
  return ext == other.ext and meas_csi_rs_id_r12 == other.meas_csi_rs_id_r12 and pci_r12 == other.pci_r12 and
         scrambling_id_r12 == other.scrambling_id_r12 and res_cfg_r12 == other.res_cfg_r12 and
         sf_offset_r12 == other.sf_offset_r12 and csi_rs_individual_offset_r12 == other.csi_rs_individual_offset_r12;
}

// PhysCellIdRangeUTRA-FDD-r9 ::= SEQUENCE
SRSASN_CODE pci_range_utra_fdd_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(range_r9_present, 1));

  HANDLE_CODE(pack_integer(bref, start_r9, (uint16_t)0u, (uint16_t)511u));
  if (range_r9_present) {
    HANDLE_CODE(pack_integer(bref, range_r9, (uint16_t)2u, (uint16_t)512u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pci_range_utra_fdd_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(range_r9_present, 1));

  HANDLE_CODE(unpack_integer(start_r9, bref, (uint16_t)0u, (uint16_t)511u));
  if (range_r9_present) {
    HANDLE_CODE(unpack_integer(range_r9, bref, (uint16_t)2u, (uint16_t)512u));
  }

  return SRSASN_SUCCESS;
}
void pci_range_utra_fdd_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("start-r9", start_r9);
  if (range_r9_present) {
    j.write_int("range-r9", range_r9);
  }
  j.end_obj();
}
bool pci_range_utra_fdd_r9_s::operator==(const pci_range_utra_fdd_r9_s& other) const
{
  return start_r9 == other.start_r9 and range_r9_present == other.range_r9_present and
         (not range_r9_present or range_r9 == other.range_r9);
}

// AltTTT-CellsToAddMod-r12 ::= SEQUENCE
SRSASN_CODE alt_ttt_cells_to_add_mod_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_idx_r12, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pci_range_r12.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE alt_ttt_cells_to_add_mod_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_idx_r12, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pci_range_r12.unpack(bref));

  return SRSASN_SUCCESS;
}
void alt_ttt_cells_to_add_mod_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellIndex-r12", cell_idx_r12);
  j.write_fieldname("physCellIdRange-r12");
  pci_range_r12.to_json(j);
  j.end_obj();
}
bool alt_ttt_cells_to_add_mod_r12_s::operator==(const alt_ttt_cells_to_add_mod_r12_s& other) const
{
  return cell_idx_r12 == other.cell_idx_r12 and pci_range_r12 == other.pci_range_r12;
}

// BlackCellsToAddMod ::= SEQUENCE
SRSASN_CODE black_cells_to_add_mod_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_idx, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pci_range.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE black_cells_to_add_mod_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_idx, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pci_range.unpack(bref));

  return SRSASN_SUCCESS;
}
void black_cells_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellIndex", cell_idx);
  j.write_fieldname("physCellIdRange");
  pci_range.to_json(j);
  j.end_obj();
}
bool black_cells_to_add_mod_s::operator==(const black_cells_to_add_mod_s& other) const
{
  return cell_idx == other.cell_idx and pci_range == other.pci_range;
}

// CellsToAddMod ::= SEQUENCE
SRSASN_CODE cells_to_add_mod_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_idx, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(cell_individual_offset.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_to_add_mod_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_idx, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(cell_individual_offset.unpack(bref));

  return SRSASN_SUCCESS;
}
void cells_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellIndex", cell_idx);
  j.write_int("physCellId", pci);
  j.write_str("cellIndividualOffset", cell_individual_offset.to_string());
  j.end_obj();
}
bool cells_to_add_mod_s::operator==(const cells_to_add_mod_s& other) const
{
  return cell_idx == other.cell_idx and pci == other.pci and cell_individual_offset == other.cell_individual_offset;
}

// CellsToAddModCDMA2000 ::= SEQUENCE
SRSASN_CODE cells_to_add_mod_cdma2000_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_idx, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)511u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_to_add_mod_cdma2000_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_idx, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)511u));

  return SRSASN_SUCCESS;
}
void cells_to_add_mod_cdma2000_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellIndex", cell_idx);
  j.write_int("physCellId", pci);
  j.end_obj();
}
bool cells_to_add_mod_cdma2000_s::operator==(const cells_to_add_mod_cdma2000_s& other) const
{
  return cell_idx == other.cell_idx and pci == other.pci;
}

// CellsToAddModNR-r15 ::= SEQUENCE
SRSASN_CODE cells_to_add_mod_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_idx_r15, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, pci_r15, (uint16_t)0u, (uint16_t)1007u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_to_add_mod_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_idx_r15, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(pci_r15, bref, (uint16_t)0u, (uint16_t)1007u));

  return SRSASN_SUCCESS;
}
void cells_to_add_mod_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellIndex-r15", cell_idx_r15);
  j.write_int("physCellId-r15", pci_r15);
  j.end_obj();
}
bool cells_to_add_mod_nr_r15_s::operator==(const cells_to_add_mod_nr_r15_s& other) const
{
  return cell_idx_r15 == other.cell_idx_r15 and pci_r15 == other.pci_r15;
}

// CellsToAddModUTRA-FDD ::= SEQUENCE
SRSASN_CODE cells_to_add_mod_utra_fdd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_idx, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)511u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_to_add_mod_utra_fdd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_idx, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)511u));

  return SRSASN_SUCCESS;
}
void cells_to_add_mod_utra_fdd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellIndex", cell_idx);
  j.write_int("physCellId", pci);
  j.end_obj();
}
bool cells_to_add_mod_utra_fdd_s::operator==(const cells_to_add_mod_utra_fdd_s& other) const
{
  return cell_idx == other.cell_idx and pci == other.pci;
}

// CellsToAddModUTRA-TDD ::= SEQUENCE
SRSASN_CODE cells_to_add_mod_utra_tdd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_idx, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, pci, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_to_add_mod_utra_tdd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_idx, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(pci, bref, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
void cells_to_add_mod_utra_tdd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellIndex", cell_idx);
  j.write_int("physCellId", pci);
  j.end_obj();
}
bool cells_to_add_mod_utra_tdd_s::operator==(const cells_to_add_mod_utra_tdd_s& other) const
{
  return cell_idx == other.cell_idx and pci == other.pci;
}

// WhiteCellsToAddMod-r13 ::= SEQUENCE
SRSASN_CODE white_cells_to_add_mod_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_idx_r13, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pci_range_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE white_cells_to_add_mod_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_idx_r13, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pci_range_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void white_cells_to_add_mod_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellIndex-r13", cell_idx_r13);
  j.write_fieldname("physCellIdRange-r13");
  pci_range_r13.to_json(j);
  j.end_obj();
}
bool white_cells_to_add_mod_r13_s::operator==(const white_cells_to_add_mod_r13_s& other) const
{
  return cell_idx_r13 == other.cell_idx_r13 and pci_range_r13 == other.pci_range_r13;
}

// BT-NameListConfig-r15 ::= CHOICE
void bt_name_list_cfg_r15_c::set(types::options e)
{
  type_ = e;
}
void bt_name_list_cfg_r15_c::set_release()
{
  set(types::release);
}
bt_name_list_r15_l& bt_name_list_cfg_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void bt_name_list_cfg_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.start_array("setup");
      for (const auto& e1 : c) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "bt_name_list_cfg_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE bt_name_list_cfg_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 4));
      break;
    default:
      log_invalid_choice_id(type_, "bt_name_list_cfg_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bt_name_list_cfg_r15_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "bt_name_list_cfg_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool bt_name_list_cfg_r15_c::operator==(const bt_name_list_cfg_r15_c& other) const
{
  return type() == other.type() and c == other.c;
}

// CDMA2000-Type ::= ENUMERATED
const char* cdma2000_type_opts::to_string() const
{
  static const char* options[] = {"type1XRTT", "typeHRPD"};
  return convert_enum_idx(options, 2, value, "cdma2000_type_e");
}
uint8_t cdma2000_type_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "cdma2000_type_e");
}

// CSG-AllowedReportingCells-r9 ::= SEQUENCE
SRSASN_CODE csg_allowed_report_cells_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pci_range_utra_fdd_list_r9_present, 1));

  if (pci_range_utra_fdd_list_r9_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pci_range_utra_fdd_list_r9, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csg_allowed_report_cells_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pci_range_utra_fdd_list_r9_present, 1));

  if (pci_range_utra_fdd_list_r9_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pci_range_utra_fdd_list_r9, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void csg_allowed_report_cells_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pci_range_utra_fdd_list_r9_present) {
    j.start_array("physCellIdRangeUTRA-FDDList-r9");
    for (const auto& e1 : pci_range_utra_fdd_list_r9) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
bool csg_allowed_report_cells_r9_s::operator==(const csg_allowed_report_cells_r9_s& other) const
{
  return pci_range_utra_fdd_list_r9_present == other.pci_range_utra_fdd_list_r9_present and
         (not pci_range_utra_fdd_list_r9_present or pci_range_utra_fdd_list_r9 == other.pci_range_utra_fdd_list_r9);
}

// MeasCycleSCell-r10 ::= ENUMERATED
const char* meas_cycle_scell_r10_opts::to_string() const
{
  static const char* options[] = {"sf160", "sf256", "sf320", "sf512", "sf640", "sf1024", "sf1280", "spare1"};
  return convert_enum_idx(options, 8, value, "meas_cycle_scell_r10_e");
}
uint16_t meas_cycle_scell_r10_opts::to_number() const
{
  static const uint16_t options[] = {160, 256, 320, 512, 640, 1024, 1280};
  return map_enum_number(options, 7, value, "meas_cycle_scell_r10_e");
}

// MeasDS-Config-r12 ::= CHOICE
void meas_ds_cfg_r12_c::set(types::options e)
{
  type_ = e;
}
void meas_ds_cfg_r12_c::set_release()
{
  set(types::release);
}
meas_ds_cfg_r12_c::setup_s_& meas_ds_cfg_r12_c::set_setup()
{
  set(types::setup);
  return c;
}
void meas_ds_cfg_r12_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("dmtc-PeriodOffset-r12");
      c.dmtc_period_offset_r12.to_json(j);
      j.write_fieldname("ds-OccasionDuration-r12");
      c.ds_occasion_dur_r12.to_json(j);
      if (c.meas_csi_rs_to_rem_list_r12_present) {
        j.start_array("measCSI-RS-ToRemoveList-r12");
        for (const auto& e1 : c.meas_csi_rs_to_rem_list_r12) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.meas_csi_rs_to_add_mod_list_r12_present) {
        j.start_array("measCSI-RS-ToAddModList-r12");
        for (const auto& e1 : c.meas_csi_rs_to_add_mod_list_r12) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "meas_ds_cfg_r12_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_ds_cfg_r12_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.pack(c.ext, 1);
      HANDLE_CODE(bref.pack(c.meas_csi_rs_to_rem_list_r12_present, 1));
      HANDLE_CODE(bref.pack(c.meas_csi_rs_to_add_mod_list_r12_present, 1));
      HANDLE_CODE(c.dmtc_period_offset_r12.pack(bref));
      HANDLE_CODE(c.ds_occasion_dur_r12.pack(bref));
      if (c.meas_csi_rs_to_rem_list_r12_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.meas_csi_rs_to_rem_list_r12, 1, 96, integer_packer<uint8_t>(1, 96)));
      }
      if (c.meas_csi_rs_to_add_mod_list_r12_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.meas_csi_rs_to_add_mod_list_r12, 1, 96));
      }
      break;
    default:
      log_invalid_choice_id(type_, "meas_ds_cfg_r12_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_ds_cfg_r12_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(bref.unpack(c.meas_csi_rs_to_rem_list_r12_present, 1));
      HANDLE_CODE(bref.unpack(c.meas_csi_rs_to_add_mod_list_r12_present, 1));
      HANDLE_CODE(c.dmtc_period_offset_r12.unpack(bref));
      HANDLE_CODE(c.ds_occasion_dur_r12.unpack(bref));
      if (c.meas_csi_rs_to_rem_list_r12_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.meas_csi_rs_to_rem_list_r12, bref, 1, 96, integer_packer<uint8_t>(1, 96)));
      }
      if (c.meas_csi_rs_to_add_mod_list_r12_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.meas_csi_rs_to_add_mod_list_r12, bref, 1, 96));
      }
      break;
    default:
      log_invalid_choice_id(type_, "meas_ds_cfg_r12_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool meas_ds_cfg_r12_c::operator==(const meas_ds_cfg_r12_c& other) const
{
  return type() == other.type() and c.ext == other.c.ext and
         c.dmtc_period_offset_r12 == other.c.dmtc_period_offset_r12 and
         c.ds_occasion_dur_r12 == other.c.ds_occasion_dur_r12 and
         c.meas_csi_rs_to_rem_list_r12_present == other.c.meas_csi_rs_to_rem_list_r12_present and
         (not c.meas_csi_rs_to_rem_list_r12_present or
          c.meas_csi_rs_to_rem_list_r12 == other.c.meas_csi_rs_to_rem_list_r12) and
         c.meas_csi_rs_to_add_mod_list_r12_present == other.c.meas_csi_rs_to_add_mod_list_r12_present and
         (not c.meas_csi_rs_to_add_mod_list_r12_present or
          c.meas_csi_rs_to_add_mod_list_r12 == other.c.meas_csi_rs_to_add_mod_list_r12);
}

void meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::destroy_() {}
void meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::dmtc_period_offset_r12_c_(
    const meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ms40_r12:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms80_r12:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms160_r12:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_");
  }
}
meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_&
meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::operator=(
    const meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ms40_r12:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms80_r12:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms160_r12:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_");
  }

  return *this;
}
uint8_t& meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::set_ms40_r12()
{
  set(types::ms40_r12);
  return c.get<uint8_t>();
}
uint8_t& meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::set_ms80_r12()
{
  set(types::ms80_r12);
  return c.get<uint8_t>();
}
uint8_t& meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::set_ms160_r12()
{
  set(types::ms160_r12);
  return c.get<uint8_t>();
}
void meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ms40_r12:
      j.write_int("ms40-r12", c.get<uint8_t>());
      break;
    case types::ms80_r12:
      j.write_int("ms80-r12", c.get<uint8_t>());
      break;
    case types::ms160_r12:
      j.write_int("ms160-r12", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ms40_r12:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms80_r12:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms160_r12:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ms40_r12:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms80_r12:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms160_r12:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::operator==(const dmtc_period_offset_r12_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::ms40_r12:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::ms80_r12:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::ms160_r12:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    default:
      return true;
  }
  return true;
}

void meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::destroy_() {}
void meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::ds_occasion_dur_r12_c_(
    const meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::dur_fdd_r12:
      c.init(other.c.get<uint8_t>());
      break;
    case types::dur_tdd_r12:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_");
  }
}
meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_& meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::operator=(
    const meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::dur_fdd_r12:
      c.set(other.c.get<uint8_t>());
      break;
    case types::dur_tdd_r12:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_");
  }

  return *this;
}
uint8_t& meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::set_dur_fdd_r12()
{
  set(types::dur_fdd_r12);
  return c.get<uint8_t>();
}
uint8_t& meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::set_dur_tdd_r12()
{
  set(types::dur_tdd_r12);
  return c.get<uint8_t>();
}
void meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::dur_fdd_r12:
      j.write_int("durationFDD-r12", c.get<uint8_t>());
      break;
    case types::dur_tdd_r12:
      j.write_int("durationTDD-r12", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::dur_fdd_r12:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)5u));
      break;
    case types::dur_tdd_r12:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)2u, (uint8_t)5u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::dur_fdd_r12:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)5u));
      break;
    case types::dur_tdd_r12:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)2u, (uint8_t)5u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::operator==(const ds_occasion_dur_r12_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::dur_fdd_r12:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::dur_tdd_r12:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    default:
      return true;
  }
  return true;
}

// MeasGapConfig ::= CHOICE
void meas_gap_cfg_c::set(types::options e)
{
  type_ = e;
}
void meas_gap_cfg_c::set_release()
{
  set(types::release);
}
meas_gap_cfg_c::setup_s_& meas_gap_cfg_c::set_setup()
{
  set(types::setup);
  return c;
}
void meas_gap_cfg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("gapOffset");
      c.gap_offset.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_gap_cfg_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.gap_offset.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_gap_cfg_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.gap_offset.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void meas_gap_cfg_c::setup_s_::gap_offset_c_::destroy_() {}
void meas_gap_cfg_c::setup_s_::gap_offset_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
meas_gap_cfg_c::setup_s_::gap_offset_c_::gap_offset_c_(const meas_gap_cfg_c::setup_s_::gap_offset_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gp0:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp1:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp2_r14:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp3_r14:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp_ncsg0_r14:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp_ncsg1_r14:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp_ncsg2_r14:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp_ncsg3_r14:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp_non_uniform1_r14:
      c.init(other.c.get<uint16_t>());
      break;
    case types::gp_non_uniform2_r14:
      c.init(other.c.get<uint16_t>());
      break;
    case types::gp_non_uniform3_r14:
      c.init(other.c.get<uint16_t>());
      break;
    case types::gp_non_uniform4_r14:
      c.init(other.c.get<uint16_t>());
      break;
    case types::gp4_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp5_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp6_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp7_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp8_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp9_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp10_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::gp11_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_c::setup_s_::gap_offset_c_");
  }
}
meas_gap_cfg_c::setup_s_::gap_offset_c_&
meas_gap_cfg_c::setup_s_::gap_offset_c_::operator=(const meas_gap_cfg_c::setup_s_::gap_offset_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gp0:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp1:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp2_r14:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp3_r14:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp_ncsg0_r14:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp_ncsg1_r14:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp_ncsg2_r14:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp_ncsg3_r14:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp_non_uniform1_r14:
      c.set(other.c.get<uint16_t>());
      break;
    case types::gp_non_uniform2_r14:
      c.set(other.c.get<uint16_t>());
      break;
    case types::gp_non_uniform3_r14:
      c.set(other.c.get<uint16_t>());
      break;
    case types::gp_non_uniform4_r14:
      c.set(other.c.get<uint16_t>());
      break;
    case types::gp4_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp5_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp6_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp7_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp8_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp9_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp10_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::gp11_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_c::setup_s_::gap_offset_c_");
  }

  return *this;
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp0()
{
  set(types::gp0);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp1()
{
  set(types::gp1);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp2_r14()
{
  set(types::gp2_r14);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp3_r14()
{
  set(types::gp3_r14);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp_ncsg0_r14()
{
  set(types::gp_ncsg0_r14);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp_ncsg1_r14()
{
  set(types::gp_ncsg1_r14);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp_ncsg2_r14()
{
  set(types::gp_ncsg2_r14);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp_ncsg3_r14()
{
  set(types::gp_ncsg3_r14);
  return c.get<uint8_t>();
}
uint16_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp_non_uniform1_r14()
{
  set(types::gp_non_uniform1_r14);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp_non_uniform2_r14()
{
  set(types::gp_non_uniform2_r14);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp_non_uniform3_r14()
{
  set(types::gp_non_uniform3_r14);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp_non_uniform4_r14()
{
  set(types::gp_non_uniform4_r14);
  return c.get<uint16_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp4_r15()
{
  set(types::gp4_r15);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp5_r15()
{
  set(types::gp5_r15);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp6_r15()
{
  set(types::gp6_r15);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp7_r15()
{
  set(types::gp7_r15);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp8_r15()
{
  set(types::gp8_r15);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp9_r15()
{
  set(types::gp9_r15);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp10_r15()
{
  set(types::gp10_r15);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_c::setup_s_::gap_offset_c_::set_gp11_r15()
{
  set(types::gp11_r15);
  return c.get<uint8_t>();
}
void meas_gap_cfg_c::setup_s_::gap_offset_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gp0:
      j.write_int("gp0", c.get<uint8_t>());
      break;
    case types::gp1:
      j.write_int("gp1", c.get<uint8_t>());
      break;
    case types::gp2_r14:
      j.write_int("gp2-r14", c.get<uint8_t>());
      break;
    case types::gp3_r14:
      j.write_int("gp3-r14", c.get<uint8_t>());
      break;
    case types::gp_ncsg0_r14:
      j.write_int("gp-ncsg0-r14", c.get<uint8_t>());
      break;
    case types::gp_ncsg1_r14:
      j.write_int("gp-ncsg1-r14", c.get<uint8_t>());
      break;
    case types::gp_ncsg2_r14:
      j.write_int("gp-ncsg2-r14", c.get<uint8_t>());
      break;
    case types::gp_ncsg3_r14:
      j.write_int("gp-ncsg3-r14", c.get<uint8_t>());
      break;
    case types::gp_non_uniform1_r14:
      j.write_int("gp-nonUniform1-r14", c.get<uint16_t>());
      break;
    case types::gp_non_uniform2_r14:
      j.write_int("gp-nonUniform2-r14", c.get<uint16_t>());
      break;
    case types::gp_non_uniform3_r14:
      j.write_int("gp-nonUniform3-r14", c.get<uint16_t>());
      break;
    case types::gp_non_uniform4_r14:
      j.write_int("gp-nonUniform4-r14", c.get<uint16_t>());
      break;
    case types::gp4_r15:
      j.write_int("gp4-r15", c.get<uint8_t>());
      break;
    case types::gp5_r15:
      j.write_int("gp5-r15", c.get<uint8_t>());
      break;
    case types::gp6_r15:
      j.write_int("gp6-r15", c.get<uint8_t>());
      break;
    case types::gp7_r15:
      j.write_int("gp7-r15", c.get<uint8_t>());
      break;
    case types::gp8_r15:
      j.write_int("gp8-r15", c.get<uint8_t>());
      break;
    case types::gp9_r15:
      j.write_int("gp9-r15", c.get<uint8_t>());
      break;
    case types::gp10_r15:
      j.write_int("gp10-r15", c.get<uint8_t>());
      break;
    case types::gp11_r15:
      j.write_int("gp11-r15", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_c::setup_s_::gap_offset_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_gap_cfg_c::setup_s_::gap_offset_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::gp0:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::gp1:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::gp2_r14: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
    } break;
    case types::gp3_r14: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
    } break;
    case types::gp_ncsg0_r14: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
    } break;
    case types::gp_ncsg1_r14: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
    } break;
    case types::gp_ncsg2_r14: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
    } break;
    case types::gp_ncsg3_r14: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
    } break;
    case types::gp_non_uniform1_r14: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
    } break;
    case types::gp_non_uniform2_r14: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2559u));
    } break;
    case types::gp_non_uniform3_r14: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)5119u));
    } break;
    case types::gp_non_uniform4_r14: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)10239u));
    } break;
    case types::gp4_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
    } break;
    case types::gp5_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
    } break;
    case types::gp6_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
    } break;
    case types::gp7_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
    } break;
    case types::gp8_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
    } break;
    case types::gp9_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
    } break;
    case types::gp10_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
    } break;
    case types::gp11_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
    } break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_c::setup_s_::gap_offset_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_gap_cfg_c::setup_s_::gap_offset_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::gp0:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::gp1:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::gp2_r14: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
    } break;
    case types::gp3_r14: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
    } break;
    case types::gp_ncsg0_r14: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
    } break;
    case types::gp_ncsg1_r14: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
    } break;
    case types::gp_ncsg2_r14: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
    } break;
    case types::gp_ncsg3_r14: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
    } break;
    case types::gp_non_uniform1_r14: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
    } break;
    case types::gp_non_uniform2_r14: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2559u));
    } break;
    case types::gp_non_uniform3_r14: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)5119u));
    } break;
    case types::gp_non_uniform4_r14: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)10239u));
    } break;
    case types::gp4_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
    } break;
    case types::gp5_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
    } break;
    case types::gp6_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
    } break;
    case types::gp7_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
    } break;
    case types::gp8_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
    } break;
    case types::gp9_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
    } break;
    case types::gp10_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
    } break;
    case types::gp11_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
    } break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_c::setup_s_::gap_offset_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MeasRSSI-ReportConfig-r13 ::= SEQUENCE
SRSASN_CODE meas_rssi_report_cfg_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ch_occupancy_thres_r13_present, 1));

  if (ch_occupancy_thres_r13_present) {
    HANDLE_CODE(pack_integer(bref, ch_occupancy_thres_r13, (uint8_t)0u, (uint8_t)76u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_rssi_report_cfg_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ch_occupancy_thres_r13_present, 1));

  if (ch_occupancy_thres_r13_present) {
    HANDLE_CODE(unpack_integer(ch_occupancy_thres_r13, bref, (uint8_t)0u, (uint8_t)76u));
  }

  return SRSASN_SUCCESS;
}
void meas_rssi_report_cfg_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ch_occupancy_thres_r13_present) {
    j.write_int("channelOccupancyThreshold-r13", ch_occupancy_thres_r13);
  }
  j.end_obj();
}
bool meas_rssi_report_cfg_r13_s::operator==(const meas_rssi_report_cfg_r13_s& other) const
{
  return ch_occupancy_thres_r13_present == other.ch_occupancy_thres_r13_present and
         (not ch_occupancy_thres_r13_present or ch_occupancy_thres_r13 == other.ch_occupancy_thres_r13);
}

// MeasSensing-Config-r15 ::= SEQUENCE
SRSASN_CODE meas_sensing_cfg_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, sensing_subch_num_r15, (uint8_t)1u, (uint8_t)20u));
  HANDLE_CODE(sensing_periodicity_r15.pack(bref));
  HANDLE_CODE(pack_integer(bref, sensing_resel_counter_r15, (uint8_t)5u, (uint8_t)75u));
  HANDLE_CODE(pack_integer(bref, sensing_prio_r15, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_sensing_cfg_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(sensing_subch_num_r15, bref, (uint8_t)1u, (uint8_t)20u));
  HANDLE_CODE(sensing_periodicity_r15.unpack(bref));
  HANDLE_CODE(unpack_integer(sensing_resel_counter_r15, bref, (uint8_t)5u, (uint8_t)75u));
  HANDLE_CODE(unpack_integer(sensing_prio_r15, bref, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
void meas_sensing_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sensingSubchannelNumber-r15", sensing_subch_num_r15);
  j.write_str("sensingPeriodicity-r15", sensing_periodicity_r15.to_string());
  j.write_int("sensingReselectionCounter-r15", sensing_resel_counter_r15);
  j.write_int("sensingPriority-r15", sensing_prio_r15);
  j.end_obj();
}
bool meas_sensing_cfg_r15_s::operator==(const meas_sensing_cfg_r15_s& other) const
{
  return sensing_subch_num_r15 == other.sensing_subch_num_r15 and
         sensing_periodicity_r15 == other.sensing_periodicity_r15 and
         sensing_resel_counter_r15 == other.sensing_resel_counter_r15 and sensing_prio_r15 == other.sensing_prio_r15;
}

const char* meas_sensing_cfg_r15_s::sensing_periodicity_r15_opts::to_string() const
{
  static const char* options[] = {
      "ms20", "ms50", "ms100", "ms200", "ms300", "ms400", "ms500", "ms600", "ms700", "ms800", "ms900", "ms1000"};
  return convert_enum_idx(options, 12, value, "meas_sensing_cfg_r15_s::sensing_periodicity_r15_e_");
}
uint16_t meas_sensing_cfg_r15_s::sensing_periodicity_r15_opts::to_number() const
{
  static const uint16_t options[] = {20, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return map_enum_number(options, 12, value, "meas_sensing_cfg_r15_s::sensing_periodicity_r15_e_");
}

// MeasSubframePatternConfigNeigh-r10 ::= CHOICE
void meas_sf_pattern_cfg_neigh_r10_c::set(types::options e)
{
  type_ = e;
}
void meas_sf_pattern_cfg_neigh_r10_c::set_release()
{
  set(types::release);
}
meas_sf_pattern_cfg_neigh_r10_c::setup_s_& meas_sf_pattern_cfg_neigh_r10_c::set_setup()
{
  set(types::setup);
  return c;
}
void meas_sf_pattern_cfg_neigh_r10_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("measSubframePatternNeigh-r10");
      c.meas_sf_pattern_neigh_r10.to_json(j);
      if (c.meas_sf_cell_list_r10_present) {
        j.start_array("measSubframeCellList-r10");
        for (const auto& e1 : c.meas_sf_cell_list_r10) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_cfg_neigh_r10_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_sf_pattern_cfg_neigh_r10_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.meas_sf_cell_list_r10_present, 1));
      HANDLE_CODE(c.meas_sf_pattern_neigh_r10.pack(bref));
      if (c.meas_sf_cell_list_r10_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.meas_sf_cell_list_r10, 1, 32));
      }
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_cfg_neigh_r10_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_sf_pattern_cfg_neigh_r10_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.meas_sf_cell_list_r10_present, 1));
      HANDLE_CODE(c.meas_sf_pattern_neigh_r10.unpack(bref));
      if (c.meas_sf_cell_list_r10_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.meas_sf_cell_list_r10, bref, 1, 32));
      }
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_cfg_neigh_r10_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool meas_sf_pattern_cfg_neigh_r10_c::operator==(const meas_sf_pattern_cfg_neigh_r10_c& other) const
{
  return type() == other.type() and c.meas_sf_pattern_neigh_r10 == other.c.meas_sf_pattern_neigh_r10 and
         c.meas_sf_cell_list_r10_present == other.c.meas_sf_cell_list_r10_present and
         (not c.meas_sf_cell_list_r10_present or c.meas_sf_cell_list_r10 == other.c.meas_sf_cell_list_r10);
}

// PhysCellIdGERAN ::= SEQUENCE
SRSASN_CODE pci_geran_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(network_colour_code.pack(bref));
  HANDLE_CODE(base_station_colour_code.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pci_geran_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(network_colour_code.unpack(bref));
  HANDLE_CODE(base_station_colour_code.unpack(bref));

  return SRSASN_SUCCESS;
}
void pci_geran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("networkColourCode", network_colour_code.to_string());
  j.write_str("baseStationColourCode", base_station_colour_code.to_string());
  j.end_obj();
}
bool pci_geran_s::operator==(const pci_geran_s& other) const
{
  return network_colour_code == other.network_colour_code and
         base_station_colour_code == other.base_station_colour_code;
}

// QuantityConfigRS-NR-r15 ::= SEQUENCE
SRSASN_CODE quant_cfg_rs_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(filt_coeff_rsrp_r15_present, 1));
  HANDLE_CODE(bref.pack(filt_coeff_rsrq_r15_present, 1));
  HANDLE_CODE(bref.pack(filt_coef_sinr_r13_present, 1));

  if (filt_coeff_rsrp_r15_present) {
    HANDLE_CODE(filt_coeff_rsrp_r15.pack(bref));
  }
  if (filt_coeff_rsrq_r15_present) {
    HANDLE_CODE(filt_coeff_rsrq_r15.pack(bref));
  }
  if (filt_coef_sinr_r13_present) {
    HANDLE_CODE(filt_coef_sinr_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_rs_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(filt_coeff_rsrp_r15_present, 1));
  HANDLE_CODE(bref.unpack(filt_coeff_rsrq_r15_present, 1));
  HANDLE_CODE(bref.unpack(filt_coef_sinr_r13_present, 1));

  if (filt_coeff_rsrp_r15_present) {
    HANDLE_CODE(filt_coeff_rsrp_r15.unpack(bref));
  }
  if (filt_coeff_rsrq_r15_present) {
    HANDLE_CODE(filt_coeff_rsrq_r15.unpack(bref));
  }
  if (filt_coef_sinr_r13_present) {
    HANDLE_CODE(filt_coef_sinr_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void quant_cfg_rs_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (filt_coeff_rsrp_r15_present) {
    j.write_str("filterCoeff-RSRP-r15", filt_coeff_rsrp_r15.to_string());
  }
  if (filt_coeff_rsrq_r15_present) {
    j.write_str("filterCoeff-RSRQ-r15", filt_coeff_rsrq_r15.to_string());
  }
  if (filt_coef_sinr_r13_present) {
    j.write_str("filterCoefficient-SINR-r13", filt_coef_sinr_r13.to_string());
  }
  j.end_obj();
}
bool quant_cfg_rs_nr_r15_s::operator==(const quant_cfg_rs_nr_r15_s& other) const
{
  return filt_coeff_rsrp_r15_present == other.filt_coeff_rsrp_r15_present and
         (not filt_coeff_rsrp_r15_present or filt_coeff_rsrp_r15 == other.filt_coeff_rsrp_r15) and
         filt_coeff_rsrq_r15_present == other.filt_coeff_rsrq_r15_present and
         (not filt_coeff_rsrq_r15_present or filt_coeff_rsrq_r15 == other.filt_coeff_rsrq_r15) and
         filt_coef_sinr_r13_present == other.filt_coef_sinr_r13_present and
         (not filt_coef_sinr_r13_present or filt_coef_sinr_r13 == other.filt_coef_sinr_r13);
}

// RMTC-Config-r13 ::= CHOICE
void rmtc_cfg_r13_c::set(types::options e)
{
  type_ = e;
}
void rmtc_cfg_r13_c::set_release()
{
  set(types::release);
}
rmtc_cfg_r13_c::setup_s_& rmtc_cfg_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void rmtc_cfg_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("rmtc-Period-r13", c.rmtc_period_r13.to_string());
      if (c.rmtc_sf_offset_r13_present) {
        j.write_int("rmtc-SubframeOffset-r13", c.rmtc_sf_offset_r13);
      }
      j.write_str("measDuration-r13", c.meas_dur_r13.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rmtc_cfg_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE rmtc_cfg_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.pack(c.ext, 1);
      HANDLE_CODE(bref.pack(c.rmtc_sf_offset_r13_present, 1));
      HANDLE_CODE(c.rmtc_period_r13.pack(bref));
      if (c.rmtc_sf_offset_r13_present) {
        HANDLE_CODE(pack_integer(bref, c.rmtc_sf_offset_r13, (uint16_t)0u, (uint16_t)639u));
      }
      HANDLE_CODE(c.meas_dur_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rmtc_cfg_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rmtc_cfg_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(bref.unpack(c.rmtc_sf_offset_r13_present, 1));
      HANDLE_CODE(c.rmtc_period_r13.unpack(bref));
      if (c.rmtc_sf_offset_r13_present) {
        HANDLE_CODE(unpack_integer(c.rmtc_sf_offset_r13, bref, (uint16_t)0u, (uint16_t)639u));
      }
      HANDLE_CODE(c.meas_dur_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rmtc_cfg_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool rmtc_cfg_r13_c::operator==(const rmtc_cfg_r13_c& other) const
{
  return type() == other.type() and c.ext == other.c.ext and c.rmtc_period_r13 == other.c.rmtc_period_r13 and
         c.rmtc_sf_offset_r13_present == other.c.rmtc_sf_offset_r13_present and
         (not c.rmtc_sf_offset_r13_present or c.rmtc_sf_offset_r13 == other.c.rmtc_sf_offset_r13) and
         c.meas_dur_r13 == other.c.meas_dur_r13;
}

const char* rmtc_cfg_r13_c::setup_s_::rmtc_period_r13_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms80", "ms160", "ms320", "ms640"};
  return convert_enum_idx(options, 5, value, "rmtc_cfg_r13_c::setup_s_::rmtc_period_r13_e_");
}
uint16_t rmtc_cfg_r13_c::setup_s_::rmtc_period_r13_opts::to_number() const
{
  static const uint16_t options[] = {40, 80, 160, 320, 640};
  return map_enum_number(options, 5, value, "rmtc_cfg_r13_c::setup_s_::rmtc_period_r13_e_");
}

const char* rmtc_cfg_r13_c::setup_s_::meas_dur_r13_opts::to_string() const
{
  static const char* options[] = {"sym1", "sym14", "sym28", "sym42", "sym70"};
  return convert_enum_idx(options, 5, value, "rmtc_cfg_r13_c::setup_s_::meas_dur_r13_e_");
}
uint8_t rmtc_cfg_r13_c::setup_s_::meas_dur_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 14, 28, 42, 70};
  return map_enum_number(options, 5, value, "rmtc_cfg_r13_c::setup_s_::meas_dur_r13_e_");
}

// RS-ConfigSSB-NR-r15 ::= SEQUENCE
SRSASN_CODE rs_cfg_ssb_nr_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(meas_timing_cfg_r15.pack(bref));
  HANDLE_CODE(subcarrier_spacing_ssb_r15.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ssb_to_measure_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ssb_to_measure_r15.is_present(), 1));
      if (ssb_to_measure_r15.is_present()) {
        HANDLE_CODE(ssb_to_measure_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rs_cfg_ssb_nr_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(meas_timing_cfg_r15.unpack(bref));
  HANDLE_CODE(subcarrier_spacing_ssb_r15.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ssb_to_measure_r15_present;
      HANDLE_CODE(bref.unpack(ssb_to_measure_r15_present, 1));
      ssb_to_measure_r15.set_present(ssb_to_measure_r15_present);
      if (ssb_to_measure_r15.is_present()) {
        HANDLE_CODE(ssb_to_measure_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rs_cfg_ssb_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measTimingConfig-r15");
  meas_timing_cfg_r15.to_json(j);
  j.write_str("subcarrierSpacingSSB-r15", subcarrier_spacing_ssb_r15.to_string());
  if (ext) {
    if (ssb_to_measure_r15.is_present()) {
      j.write_fieldname("ssb-ToMeasure-r15");
      ssb_to_measure_r15->to_json(j);
    }
  }
  j.end_obj();
}
bool rs_cfg_ssb_nr_r15_s::operator==(const rs_cfg_ssb_nr_r15_s& other) const
{
  return ext == other.ext and meas_timing_cfg_r15 == other.meas_timing_cfg_r15 and
         subcarrier_spacing_ssb_r15 == other.subcarrier_spacing_ssb_r15 and
         (not ext or (ssb_to_measure_r15.is_present() == other.ssb_to_measure_r15.is_present() and
                      (not ssb_to_measure_r15.is_present() or *ssb_to_measure_r15 == *other.ssb_to_measure_r15)));
}

const char* rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_opts::to_string() const
{
  static const char* options[] = {"kHz15", "kHz30", "kHz120", "kHz240"};
  return convert_enum_idx(options, 4, value, "rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}
uint8_t rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_opts::to_number() const
{
  static const uint8_t options[] = {15, 30, 120, 240};
  return map_enum_number(options, 4, value, "rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}

void rs_cfg_ssb_nr_r15_s::ssb_to_measure_r15_c_::set(types::options e)
{
  type_ = e;
}
void rs_cfg_ssb_nr_r15_s::ssb_to_measure_r15_c_::set_release()
{
  set(types::release);
}
ssb_to_measure_r15_c& rs_cfg_ssb_nr_r15_s::ssb_to_measure_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void rs_cfg_ssb_nr_r15_s::ssb_to_measure_r15_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "rs_cfg_ssb_nr_r15_s::ssb_to_measure_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE rs_cfg_ssb_nr_r15_s::ssb_to_measure_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rs_cfg_ssb_nr_r15_s::ssb_to_measure_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rs_cfg_ssb_nr_r15_s::ssb_to_measure_r15_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "rs_cfg_ssb_nr_r15_s::ssb_to_measure_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool rs_cfg_ssb_nr_r15_s::ssb_to_measure_r15_c_::operator==(const ssb_to_measure_r15_c_& other) const
{
  return type() == other.type() and c == other.c;
}

// RSRQ-RangeConfig-r12 ::= CHOICE
void rsrq_range_cfg_r12_c::set(types::options e)
{
  type_ = e;
}
void rsrq_range_cfg_r12_c::set_release()
{
  set(types::release);
}
int8_t& rsrq_range_cfg_r12_c::set_setup()
{
  set(types::setup);
  return c;
}
void rsrq_range_cfg_r12_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_int("setup", c);
      break;
    default:
      log_invalid_choice_id(type_, "rsrq_range_cfg_r12_c");
  }
  j.end_obj();
}
SRSASN_CODE rsrq_range_cfg_r12_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c, (int8_t)-30, (int8_t)46));
      break;
    default:
      log_invalid_choice_id(type_, "rsrq_range_cfg_r12_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rsrq_range_cfg_r12_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c, bref, (int8_t)-30, (int8_t)46));
      break;
    default:
      log_invalid_choice_id(type_, "rsrq_range_cfg_r12_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool rsrq_range_cfg_r12_c::operator==(const rsrq_range_cfg_r12_c& other) const
{
  return type() == other.type() and c == other.c;
}

// ReportInterval ::= ENUMERATED
const char* report_interv_opts::to_string() const
{
  static const char* options[] = {"ms120",
                                  "ms240",
                                  "ms480",
                                  "ms640",
                                  "ms1024",
                                  "ms2048",
                                  "ms5120",
                                  "ms10240",
                                  "min1",
                                  "min6",
                                  "min12",
                                  "min30",
                                  "min60",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "report_interv_e");
}
uint16_t report_interv_opts::to_number() const
{
  static const uint16_t options[] = {120, 240, 480, 640, 1024, 2048, 5120, 10240, 1, 6, 12, 30, 60};
  return map_enum_number(options, 13, value, "report_interv_e");
}

// ReportQuantityNR-r15 ::= SEQUENCE
SRSASN_CODE report_quant_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ss_rsrp, 1));
  HANDLE_CODE(bref.pack(ss_rsrq, 1));
  HANDLE_CODE(bref.pack(ss_sinr, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE report_quant_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ss_rsrp, 1));
  HANDLE_CODE(bref.unpack(ss_rsrq, 1));
  HANDLE_CODE(bref.unpack(ss_sinr, 1));

  return SRSASN_SUCCESS;
}
void report_quant_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("ss-rsrp", ss_rsrp);
  j.write_bool("ss-rsrq", ss_rsrq);
  j.write_bool("ss-sinr", ss_sinr);
  j.end_obj();
}
bool report_quant_nr_r15_s::operator==(const report_quant_nr_r15_s& other) const
{
  return ss_rsrp == other.ss_rsrp and ss_rsrq == other.ss_rsrq and ss_sinr == other.ss_sinr;
}

// ReportQuantityWLAN-r13 ::= SEQUENCE
SRSASN_CODE report_quant_wlan_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(band_request_wlan_r13_present, 1));
  HANDLE_CODE(bref.pack(carrier_info_request_wlan_r13_present, 1));
  HANDLE_CODE(bref.pack(available_admission_capacity_request_wlan_r13_present, 1));
  HANDLE_CODE(bref.pack(backhaul_dl_bw_request_wlan_r13_present, 1));
  HANDLE_CODE(bref.pack(backhaul_ul_bw_request_wlan_r13_present, 1));
  HANDLE_CODE(bref.pack(ch_utilization_request_wlan_r13_present, 1));
  HANDLE_CODE(bref.pack(station_count_request_wlan_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE report_quant_wlan_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(band_request_wlan_r13_present, 1));
  HANDLE_CODE(bref.unpack(carrier_info_request_wlan_r13_present, 1));
  HANDLE_CODE(bref.unpack(available_admission_capacity_request_wlan_r13_present, 1));
  HANDLE_CODE(bref.unpack(backhaul_dl_bw_request_wlan_r13_present, 1));
  HANDLE_CODE(bref.unpack(backhaul_ul_bw_request_wlan_r13_present, 1));
  HANDLE_CODE(bref.unpack(ch_utilization_request_wlan_r13_present, 1));
  HANDLE_CODE(bref.unpack(station_count_request_wlan_r13_present, 1));

  return SRSASN_SUCCESS;
}
void report_quant_wlan_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_request_wlan_r13_present) {
    j.write_str("bandRequestWLAN-r13", "true");
  }
  if (carrier_info_request_wlan_r13_present) {
    j.write_str("carrierInfoRequestWLAN-r13", "true");
  }
  if (available_admission_capacity_request_wlan_r13_present) {
    j.write_str("availableAdmissionCapacityRequestWLAN-r13", "true");
  }
  if (backhaul_dl_bw_request_wlan_r13_present) {
    j.write_str("backhaulDL-BandwidthRequestWLAN-r13", "true");
  }
  if (backhaul_ul_bw_request_wlan_r13_present) {
    j.write_str("backhaulUL-BandwidthRequestWLAN-r13", "true");
  }
  if (ch_utilization_request_wlan_r13_present) {
    j.write_str("channelUtilizationRequestWLAN-r13", "true");
  }
  if (station_count_request_wlan_r13_present) {
    j.write_str("stationCountRequestWLAN-r13", "true");
  }
  j.end_obj();
}
bool report_quant_wlan_r13_s::operator==(const report_quant_wlan_r13_s& other) const
{
  return ext == other.ext and band_request_wlan_r13_present == other.band_request_wlan_r13_present and
         carrier_info_request_wlan_r13_present == other.carrier_info_request_wlan_r13_present and
         available_admission_capacity_request_wlan_r13_present ==
             other.available_admission_capacity_request_wlan_r13_present and
         backhaul_dl_bw_request_wlan_r13_present == other.backhaul_dl_bw_request_wlan_r13_present and
         backhaul_ul_bw_request_wlan_r13_present == other.backhaul_ul_bw_request_wlan_r13_present and
         ch_utilization_request_wlan_r13_present == other.ch_utilization_request_wlan_r13_present and
         station_count_request_wlan_r13_present == other.station_count_request_wlan_r13_present;
}

// ThresholdEUTRA ::= CHOICE
void thres_eutra_c::destroy_() {}
void thres_eutra_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
thres_eutra_c::thres_eutra_c(const thres_eutra_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::thres_rsrp:
      c.init(other.c.get<uint8_t>());
      break;
    case types::thres_rsrq:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "thres_eutra_c");
  }
}
thres_eutra_c& thres_eutra_c::operator=(const thres_eutra_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::thres_rsrp:
      c.set(other.c.get<uint8_t>());
      break;
    case types::thres_rsrq:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "thres_eutra_c");
  }

  return *this;
}
uint8_t& thres_eutra_c::set_thres_rsrp()
{
  set(types::thres_rsrp);
  return c.get<uint8_t>();
}
uint8_t& thres_eutra_c::set_thres_rsrq()
{
  set(types::thres_rsrq);
  return c.get<uint8_t>();
}
void thres_eutra_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::thres_rsrp:
      j.write_int("threshold-RSRP", c.get<uint8_t>());
      break;
    case types::thres_rsrq:
      j.write_int("threshold-RSRQ", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "thres_eutra_c");
  }
  j.end_obj();
}
SRSASN_CODE thres_eutra_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::thres_rsrp:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)97u));
      break;
    case types::thres_rsrq:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)34u));
      break;
    default:
      log_invalid_choice_id(type_, "thres_eutra_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE thres_eutra_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::thres_rsrp:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)97u));
      break;
    case types::thres_rsrq:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)34u));
      break;
    default:
      log_invalid_choice_id(type_, "thres_eutra_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool thres_eutra_c::operator==(const thres_eutra_c& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::thres_rsrp:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::thres_rsrq:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    default:
      return true;
  }
  return true;
}

// ThresholdNR-r15 ::= CHOICE
void thres_nr_r15_c::destroy_() {}
void thres_nr_r15_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
thres_nr_r15_c::thres_nr_r15_c(const thres_nr_r15_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr_rsrp_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nr_rsrq_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nr_sinr_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "thres_nr_r15_c");
  }
}
thres_nr_r15_c& thres_nr_r15_c::operator=(const thres_nr_r15_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr_rsrp_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nr_rsrq_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nr_sinr_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "thres_nr_r15_c");
  }

  return *this;
}
uint8_t& thres_nr_r15_c::set_nr_rsrp_r15()
{
  set(types::nr_rsrp_r15);
  return c.get<uint8_t>();
}
uint8_t& thres_nr_r15_c::set_nr_rsrq_r15()
{
  set(types::nr_rsrq_r15);
  return c.get<uint8_t>();
}
uint8_t& thres_nr_r15_c::set_nr_sinr_r15()
{
  set(types::nr_sinr_r15);
  return c.get<uint8_t>();
}
void thres_nr_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr_rsrp_r15:
      j.write_int("nr-RSRP-r15", c.get<uint8_t>());
      break;
    case types::nr_rsrq_r15:
      j.write_int("nr-RSRQ-r15", c.get<uint8_t>());
      break;
    case types::nr_sinr_r15:
      j.write_int("nr-SINR-r15", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "thres_nr_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE thres_nr_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr_rsrp_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::nr_rsrq_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::nr_sinr_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "thres_nr_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE thres_nr_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr_rsrp_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::nr_rsrq_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::nr_sinr_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "thres_nr_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool thres_nr_r15_c::operator==(const thres_nr_r15_c& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::nr_rsrp_r15:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::nr_rsrq_r15:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::nr_sinr_r15:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    default:
      return true;
  }
  return true;
}

// ThresholdUTRA ::= CHOICE
void thres_utra_c::destroy_() {}
void thres_utra_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
thres_utra_c::thres_utra_c(const thres_utra_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::utra_rscp:
      c.init(other.c.get<int8_t>());
      break;
    case types::utra_ec_n0:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "thres_utra_c");
  }
}
thres_utra_c& thres_utra_c::operator=(const thres_utra_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::utra_rscp:
      c.set(other.c.get<int8_t>());
      break;
    case types::utra_ec_n0:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "thres_utra_c");
  }

  return *this;
}
int8_t& thres_utra_c::set_utra_rscp()
{
  set(types::utra_rscp);
  return c.get<int8_t>();
}
uint8_t& thres_utra_c::set_utra_ec_n0()
{
  set(types::utra_ec_n0);
  return c.get<uint8_t>();
}
void thres_utra_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::utra_rscp:
      j.write_int("utra-RSCP", c.get<int8_t>());
      break;
    case types::utra_ec_n0:
      j.write_int("utra-EcN0", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "thres_utra_c");
  }
  j.end_obj();
}
SRSASN_CODE thres_utra_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::utra_rscp:
      HANDLE_CODE(pack_integer(bref, c.get<int8_t>(), (int8_t)-5, (int8_t)91));
      break;
    case types::utra_ec_n0:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)49u));
      break;
    default:
      log_invalid_choice_id(type_, "thres_utra_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE thres_utra_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::utra_rscp:
      HANDLE_CODE(unpack_integer(c.get<int8_t>(), bref, (int8_t)-5, (int8_t)91));
      break;
    case types::utra_ec_n0:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)49u));
      break;
    default:
      log_invalid_choice_id(type_, "thres_utra_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool thres_utra_c::operator==(const thres_utra_c& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::utra_rscp:
      return c.get<int8_t>() == other.c.get<int8_t>();
    case types::utra_ec_n0:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    default:
      return true;
  }
  return true;
}

// TimeToTrigger ::= ENUMERATED
const char* time_to_trigger_opts::to_string() const
{
  static const char* options[] = {"ms0",
                                  "ms40",
                                  "ms64",
                                  "ms80",
                                  "ms100",
                                  "ms128",
                                  "ms160",
                                  "ms256",
                                  "ms320",
                                  "ms480",
                                  "ms512",
                                  "ms640",
                                  "ms1024",
                                  "ms1280",
                                  "ms2560",
                                  "ms5120"};
  return convert_enum_idx(options, 16, value, "time_to_trigger_e");
}
uint16_t time_to_trigger_opts::to_number() const
{
  static const uint16_t options[] = {0, 40, 64, 80, 100, 128, 160, 256, 320, 480, 512, 640, 1024, 1280, 2560, 5120};
  return map_enum_number(options, 16, value, "time_to_trigger_e");
}

// UL-DelayConfig-r13 ::= CHOICE
void ul_delay_cfg_r13_c::set(types::options e)
{
  type_ = e;
}
void ul_delay_cfg_r13_c::set_release()
{
  set(types::release);
}
ul_delay_cfg_r13_c::setup_s_& ul_delay_cfg_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void ul_delay_cfg_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("delayThreshold-r13", c.delay_thres_r13.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "ul_delay_cfg_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_delay_cfg_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.delay_thres_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_delay_cfg_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_delay_cfg_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.delay_thres_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_delay_cfg_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool ul_delay_cfg_r13_c::operator==(const ul_delay_cfg_r13_c& other) const
{
  return type() == other.type() and c.delay_thres_r13 == other.c.delay_thres_r13;
}

const char* ul_delay_cfg_r13_c::setup_s_::delay_thres_r13_opts::to_string() const
{
  static const char* options[] = {"ms30",
                                  "ms40",
                                  "ms50",
                                  "ms60",
                                  "ms70",
                                  "ms80",
                                  "ms90",
                                  "ms100",
                                  "ms150",
                                  "ms300",
                                  "ms500",
                                  "ms750",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "ul_delay_cfg_r13_c::setup_s_::delay_thres_r13_e_");
}
uint16_t ul_delay_cfg_r13_c::setup_s_::delay_thres_r13_opts::to_number() const
{
  static const uint16_t options[] = {30, 40, 50, 60, 70, 80, 90, 100, 150, 300, 500, 750};
  return map_enum_number(options, 12, value, "ul_delay_cfg_r13_c::setup_s_::delay_thres_r13_e_");
}

// WLAN-CarrierInfo-r13 ::= SEQUENCE
SRSASN_CODE wlan_carrier_info_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(operating_class_r13_present, 1));
  HANDLE_CODE(bref.pack(country_code_r13_present, 1));
  HANDLE_CODE(bref.pack(ch_nums_r13_present, 1));

  if (operating_class_r13_present) {
    HANDLE_CODE(pack_integer(bref, operating_class_r13, (uint16_t)0u, (uint16_t)255u));
  }
  if (country_code_r13_present) {
    HANDLE_CODE(country_code_r13.pack(bref));
  }
  if (ch_nums_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ch_nums_r13, 1, 16, integer_packer<uint16_t>(0, 255)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_carrier_info_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(operating_class_r13_present, 1));
  HANDLE_CODE(bref.unpack(country_code_r13_present, 1));
  HANDLE_CODE(bref.unpack(ch_nums_r13_present, 1));

  if (operating_class_r13_present) {
    HANDLE_CODE(unpack_integer(operating_class_r13, bref, (uint16_t)0u, (uint16_t)255u));
  }
  if (country_code_r13_present) {
    HANDLE_CODE(country_code_r13.unpack(bref));
  }
  if (ch_nums_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ch_nums_r13, bref, 1, 16, integer_packer<uint16_t>(0, 255)));
  }

  return SRSASN_SUCCESS;
}
void wlan_carrier_info_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (operating_class_r13_present) {
    j.write_int("operatingClass-r13", operating_class_r13);
  }
  if (country_code_r13_present) {
    j.write_str("countryCode-r13", country_code_r13.to_string());
  }
  if (ch_nums_r13_present) {
    j.start_array("channelNumbers-r13");
    for (const auto& e1 : ch_nums_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}
bool wlan_carrier_info_r13_s::operator==(const wlan_carrier_info_r13_s& other) const
{
  return ext == other.ext and operating_class_r13_present == other.operating_class_r13_present and
         (not operating_class_r13_present or operating_class_r13 == other.operating_class_r13) and
         country_code_r13_present == other.country_code_r13_present and
         (not country_code_r13_present or country_code_r13 == other.country_code_r13) and
         ch_nums_r13_present == other.ch_nums_r13_present and
         (not ch_nums_r13_present or ch_nums_r13 == other.ch_nums_r13);
}

const char* wlan_carrier_info_r13_s::country_code_r13_opts::to_string() const
{
  static const char* options[] = {"unitedStates", "europe", "japan", "global"};
  return convert_enum_idx(options, 4, value, "wlan_carrier_info_r13_s::country_code_r13_e_");
}

// WLAN-NameListConfig-r15 ::= CHOICE
void wlan_name_list_cfg_r15_c::set(types::options e)
{
  type_ = e;
}
void wlan_name_list_cfg_r15_c::set_release()
{
  set(types::release);
}
wlan_name_list_r15_l& wlan_name_list_cfg_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void wlan_name_list_cfg_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.start_array("setup");
      for (const auto& e1 : c) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "wlan_name_list_cfg_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE wlan_name_list_cfg_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 4));
      break;
    default:
      log_invalid_choice_id(type_, "wlan_name_list_cfg_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_name_list_cfg_r15_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "wlan_name_list_cfg_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool wlan_name_list_cfg_r15_c::operator==(const wlan_name_list_cfg_r15_c& other) const
{
  return type() == other.type() and c == other.c;
}

// MeasGapConfigPerCC-r14 ::= SEQUENCE
SRSASN_CODE meas_gap_cfg_per_cc_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, serv_cell_id_r14, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(meas_gap_cfg_cc_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_gap_cfg_per_cc_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(serv_cell_id_r14, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(meas_gap_cfg_cc_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_gap_cfg_per_cc_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellId-r14", serv_cell_id_r14);
  j.write_fieldname("measGapConfigCC-r14");
  meas_gap_cfg_cc_r14.to_json(j);
  j.end_obj();
}

// MeasObjectCDMA2000 ::= SEQUENCE
SRSASN_CODE meas_obj_cdma2000_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(search_win_size_present, 1));
  HANDLE_CODE(bref.pack(offset_freq_present, 1));
  HANDLE_CODE(bref.pack(cells_to_rem_list_present, 1));
  HANDLE_CODE(bref.pack(cells_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.pack(cell_for_which_to_report_cgi_present, 1));

  HANDLE_CODE(cdma2000_type.pack(bref));
  HANDLE_CODE(carrier_freq.pack(bref));
  if (search_win_size_present) {
    HANDLE_CODE(pack_integer(bref, search_win_size, (uint8_t)0u, (uint8_t)15u));
  }
  if (offset_freq_present) {
    HANDLE_CODE(pack_integer(bref, offset_freq, (int8_t)-15, (int8_t)15));
  }
  if (cells_to_rem_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_to_rem_list, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cells_to_add_mod_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_to_add_mod_list, 1, 32));
  }
  if (cell_for_which_to_report_cgi_present) {
    HANDLE_CODE(pack_integer(bref, cell_for_which_to_report_cgi, (uint16_t)0u, (uint16_t)511u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_cdma2000_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(search_win_size_present, 1));
  HANDLE_CODE(bref.unpack(offset_freq_present, 1));
  HANDLE_CODE(bref.unpack(cells_to_rem_list_present, 1));
  HANDLE_CODE(bref.unpack(cells_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(cell_for_which_to_report_cgi_present, 1));

  HANDLE_CODE(cdma2000_type.unpack(bref));
  HANDLE_CODE(carrier_freq.unpack(bref));
  if (search_win_size_present) {
    HANDLE_CODE(unpack_integer(search_win_size, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (offset_freq_present) {
    HANDLE_CODE(unpack_integer(offset_freq, bref, (int8_t)-15, (int8_t)15));
  }
  if (cells_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_to_rem_list, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cells_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_to_add_mod_list, bref, 1, 32));
  }
  if (cell_for_which_to_report_cgi_present) {
    HANDLE_CODE(unpack_integer(cell_for_which_to_report_cgi, bref, (uint16_t)0u, (uint16_t)511u));
  }

  return SRSASN_SUCCESS;
}
void meas_obj_cdma2000_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cdma2000-Type", cdma2000_type.to_string());
  j.write_fieldname("carrierFreq");
  carrier_freq.to_json(j);
  if (search_win_size_present) {
    j.write_int("searchWindowSize", search_win_size);
  }
  if (offset_freq_present) {
    j.write_int("offsetFreq", offset_freq);
  }
  if (cells_to_rem_list_present) {
    j.start_array("cellsToRemoveList");
    for (const auto& e1 : cells_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (cells_to_add_mod_list_present) {
    j.start_array("cellsToAddModList");
    for (const auto& e1 : cells_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cell_for_which_to_report_cgi_present) {
    j.write_int("cellForWhichToReportCGI", cell_for_which_to_report_cgi);
  }
  j.end_obj();
}
bool meas_obj_cdma2000_s::operator==(const meas_obj_cdma2000_s& other) const
{
  return ext == other.ext and cdma2000_type == other.cdma2000_type and carrier_freq == other.carrier_freq and
         search_win_size_present == other.search_win_size_present and
         (not search_win_size_present or search_win_size == other.search_win_size) and
         offset_freq_present == other.offset_freq_present and
         (not offset_freq_present or offset_freq == other.offset_freq) and
         cells_to_rem_list_present == other.cells_to_rem_list_present and
         (not cells_to_rem_list_present or cells_to_rem_list == other.cells_to_rem_list) and
         cells_to_add_mod_list_present == other.cells_to_add_mod_list_present and
         (not cells_to_add_mod_list_present or cells_to_add_mod_list == other.cells_to_add_mod_list) and
         cell_for_which_to_report_cgi_present == other.cell_for_which_to_report_cgi_present and
         (not cell_for_which_to_report_cgi_present or
          cell_for_which_to_report_cgi == other.cell_for_which_to_report_cgi);
}

// MeasObjectEUTRA ::= SEQUENCE
SRSASN_CODE meas_obj_eutra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(offset_freq_present, 1));
  HANDLE_CODE(bref.pack(cells_to_rem_list_present, 1));
  HANDLE_CODE(bref.pack(cells_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.pack(black_cells_to_rem_list_present, 1));
  HANDLE_CODE(bref.pack(black_cells_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.pack(cell_for_which_to_report_cgi_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(allowed_meas_bw.pack(bref));
  HANDLE_CODE(bref.pack(presence_ant_port1, 1));
  HANDLE_CODE(neigh_cell_cfg.pack(bref));
  if (offset_freq_present) {
    HANDLE_CODE(offset_freq.pack(bref));
  }
  if (cells_to_rem_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_to_rem_list, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cells_to_add_mod_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_to_add_mod_list, 1, 32));
  }
  if (black_cells_to_rem_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, black_cells_to_rem_list, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (black_cells_to_add_mod_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, black_cells_to_add_mod_list, 1, 32));
  }
  if (cell_for_which_to_report_cgi_present) {
    HANDLE_CODE(pack_integer(bref, cell_for_which_to_report_cgi, (uint16_t)0u, (uint16_t)503u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_cycle_scell_r10_present;
    group_flags[0] |= meas_sf_pattern_cfg_neigh_r10.is_present();
    group_flags[1] |= wideband_rsrq_meas_r11_present;
    group_flags[2] |= alt_ttt_cells_to_rem_list_r12.is_present();
    group_flags[2] |= alt_ttt_cells_to_add_mod_list_r12.is_present();
    group_flags[2] |= t312_r12.is_present();
    group_flags[2] |= reduced_meas_performance_r12_present;
    group_flags[2] |= meas_ds_cfg_r12.is_present();
    group_flags[3] |= white_cells_to_rem_list_r13.is_present();
    group_flags[3] |= white_cells_to_add_mod_list_r13.is_present();
    group_flags[3] |= rmtc_cfg_r13.is_present();
    group_flags[3] |= carrier_freq_r13_present;
    group_flags[4] |= tx_res_pool_to_rem_list_r14.is_present();
    group_flags[4] |= tx_res_pool_to_add_list_r14.is_present();
    group_flags[4] |= fembms_mixed_carrier_r14_present;
    group_flags[5] |= meas_sensing_cfg_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_cycle_scell_r10_present, 1));
      HANDLE_CODE(bref.pack(meas_sf_pattern_cfg_neigh_r10.is_present(), 1));
      if (meas_cycle_scell_r10_present) {
        HANDLE_CODE(meas_cycle_scell_r10.pack(bref));
      }
      if (meas_sf_pattern_cfg_neigh_r10.is_present()) {
        HANDLE_CODE(meas_sf_pattern_cfg_neigh_r10->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(wideband_rsrq_meas_r11_present, 1));
      if (wideband_rsrq_meas_r11_present) {
        HANDLE_CODE(bref.pack(wideband_rsrq_meas_r11, 1));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(alt_ttt_cells_to_rem_list_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(alt_ttt_cells_to_add_mod_list_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(t312_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(reduced_meas_performance_r12_present, 1));
      HANDLE_CODE(bref.pack(meas_ds_cfg_r12.is_present(), 1));
      if (alt_ttt_cells_to_rem_list_r12.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *alt_ttt_cells_to_rem_list_r12, 1, 32, integer_packer<uint8_t>(1, 32)));
      }
      if (alt_ttt_cells_to_add_mod_list_r12.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *alt_ttt_cells_to_add_mod_list_r12, 1, 32));
      }
      if (t312_r12.is_present()) {
        HANDLE_CODE(t312_r12->pack(bref));
      }
      if (reduced_meas_performance_r12_present) {
        HANDLE_CODE(bref.pack(reduced_meas_performance_r12, 1));
      }
      if (meas_ds_cfg_r12.is_present()) {
        HANDLE_CODE(meas_ds_cfg_r12->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(white_cells_to_rem_list_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(white_cells_to_add_mod_list_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(rmtc_cfg_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(carrier_freq_r13_present, 1));
      if (white_cells_to_rem_list_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *white_cells_to_rem_list_r13, 1, 32, integer_packer<uint8_t>(1, 32)));
      }
      if (white_cells_to_add_mod_list_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *white_cells_to_add_mod_list_r13, 1, 32));
      }
      if (rmtc_cfg_r13.is_present()) {
        HANDLE_CODE(rmtc_cfg_r13->pack(bref));
      }
      if (carrier_freq_r13_present) {
        HANDLE_CODE(pack_integer(bref, carrier_freq_r13, (uint32_t)65536u, (uint32_t)262143u));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(tx_res_pool_to_rem_list_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(tx_res_pool_to_add_list_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(fembms_mixed_carrier_r14_present, 1));
      if (tx_res_pool_to_rem_list_r14.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *tx_res_pool_to_rem_list_r14, 1, 72, integer_packer<uint8_t>(1, 72)));
      }
      if (tx_res_pool_to_add_list_r14.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *tx_res_pool_to_add_list_r14, 1, 72, integer_packer<uint8_t>(1, 72)));
      }
      if (fembms_mixed_carrier_r14_present) {
        HANDLE_CODE(bref.pack(fembms_mixed_carrier_r14, 1));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_sensing_cfg_r15.is_present(), 1));
      if (meas_sensing_cfg_r15.is_present()) {
        HANDLE_CODE(meas_sensing_cfg_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_eutra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(offset_freq_present, 1));
  HANDLE_CODE(bref.unpack(cells_to_rem_list_present, 1));
  HANDLE_CODE(bref.unpack(cells_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(black_cells_to_rem_list_present, 1));
  HANDLE_CODE(bref.unpack(black_cells_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(cell_for_which_to_report_cgi_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(allowed_meas_bw.unpack(bref));
  HANDLE_CODE(bref.unpack(presence_ant_port1, 1));
  HANDLE_CODE(neigh_cell_cfg.unpack(bref));
  if (offset_freq_present) {
    HANDLE_CODE(offset_freq.unpack(bref));
  }
  if (cells_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_to_rem_list, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cells_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_to_add_mod_list, bref, 1, 32));
  }
  if (black_cells_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(black_cells_to_rem_list, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (black_cells_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(black_cells_to_add_mod_list, bref, 1, 32));
  }
  if (cell_for_which_to_report_cgi_present) {
    HANDLE_CODE(unpack_integer(cell_for_which_to_report_cgi, bref, (uint16_t)0u, (uint16_t)503u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(6);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(meas_cycle_scell_r10_present, 1));
      bool meas_sf_pattern_cfg_neigh_r10_present;
      HANDLE_CODE(bref.unpack(meas_sf_pattern_cfg_neigh_r10_present, 1));
      meas_sf_pattern_cfg_neigh_r10.set_present(meas_sf_pattern_cfg_neigh_r10_present);
      if (meas_cycle_scell_r10_present) {
        HANDLE_CODE(meas_cycle_scell_r10.unpack(bref));
      }
      if (meas_sf_pattern_cfg_neigh_r10.is_present()) {
        HANDLE_CODE(meas_sf_pattern_cfg_neigh_r10->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(wideband_rsrq_meas_r11_present, 1));
      if (wideband_rsrq_meas_r11_present) {
        HANDLE_CODE(bref.unpack(wideband_rsrq_meas_r11, 1));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool alt_ttt_cells_to_rem_list_r12_present;
      HANDLE_CODE(bref.unpack(alt_ttt_cells_to_rem_list_r12_present, 1));
      alt_ttt_cells_to_rem_list_r12.set_present(alt_ttt_cells_to_rem_list_r12_present);
      bool alt_ttt_cells_to_add_mod_list_r12_present;
      HANDLE_CODE(bref.unpack(alt_ttt_cells_to_add_mod_list_r12_present, 1));
      alt_ttt_cells_to_add_mod_list_r12.set_present(alt_ttt_cells_to_add_mod_list_r12_present);
      bool t312_r12_present;
      HANDLE_CODE(bref.unpack(t312_r12_present, 1));
      t312_r12.set_present(t312_r12_present);
      HANDLE_CODE(bref.unpack(reduced_meas_performance_r12_present, 1));
      bool meas_ds_cfg_r12_present;
      HANDLE_CODE(bref.unpack(meas_ds_cfg_r12_present, 1));
      meas_ds_cfg_r12.set_present(meas_ds_cfg_r12_present);
      if (alt_ttt_cells_to_rem_list_r12.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*alt_ttt_cells_to_rem_list_r12, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
      }
      if (alt_ttt_cells_to_add_mod_list_r12.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*alt_ttt_cells_to_add_mod_list_r12, bref, 1, 32));
      }
      if (t312_r12.is_present()) {
        HANDLE_CODE(t312_r12->unpack(bref));
      }
      if (reduced_meas_performance_r12_present) {
        HANDLE_CODE(bref.unpack(reduced_meas_performance_r12, 1));
      }
      if (meas_ds_cfg_r12.is_present()) {
        HANDLE_CODE(meas_ds_cfg_r12->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool white_cells_to_rem_list_r13_present;
      HANDLE_CODE(bref.unpack(white_cells_to_rem_list_r13_present, 1));
      white_cells_to_rem_list_r13.set_present(white_cells_to_rem_list_r13_present);
      bool white_cells_to_add_mod_list_r13_present;
      HANDLE_CODE(bref.unpack(white_cells_to_add_mod_list_r13_present, 1));
      white_cells_to_add_mod_list_r13.set_present(white_cells_to_add_mod_list_r13_present);
      bool rmtc_cfg_r13_present;
      HANDLE_CODE(bref.unpack(rmtc_cfg_r13_present, 1));
      rmtc_cfg_r13.set_present(rmtc_cfg_r13_present);
      HANDLE_CODE(bref.unpack(carrier_freq_r13_present, 1));
      if (white_cells_to_rem_list_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*white_cells_to_rem_list_r13, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
      }
      if (white_cells_to_add_mod_list_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*white_cells_to_add_mod_list_r13, bref, 1, 32));
      }
      if (rmtc_cfg_r13.is_present()) {
        HANDLE_CODE(rmtc_cfg_r13->unpack(bref));
      }
      if (carrier_freq_r13_present) {
        HANDLE_CODE(unpack_integer(carrier_freq_r13, bref, (uint32_t)65536u, (uint32_t)262143u));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool tx_res_pool_to_rem_list_r14_present;
      HANDLE_CODE(bref.unpack(tx_res_pool_to_rem_list_r14_present, 1));
      tx_res_pool_to_rem_list_r14.set_present(tx_res_pool_to_rem_list_r14_present);
      bool tx_res_pool_to_add_list_r14_present;
      HANDLE_CODE(bref.unpack(tx_res_pool_to_add_list_r14_present, 1));
      tx_res_pool_to_add_list_r14.set_present(tx_res_pool_to_add_list_r14_present);
      HANDLE_CODE(bref.unpack(fembms_mixed_carrier_r14_present, 1));
      if (tx_res_pool_to_rem_list_r14.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*tx_res_pool_to_rem_list_r14, bref, 1, 72, integer_packer<uint8_t>(1, 72)));
      }
      if (tx_res_pool_to_add_list_r14.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*tx_res_pool_to_add_list_r14, bref, 1, 72, integer_packer<uint8_t>(1, 72)));
      }
      if (fembms_mixed_carrier_r14_present) {
        HANDLE_CODE(bref.unpack(fembms_mixed_carrier_r14, 1));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_sensing_cfg_r15_present;
      HANDLE_CODE(bref.unpack(meas_sensing_cfg_r15_present, 1));
      meas_sensing_cfg_r15.set_present(meas_sensing_cfg_r15_present);
      if (meas_sensing_cfg_r15.is_present()) {
        HANDLE_CODE(meas_sensing_cfg_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_obj_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  j.write_str("allowedMeasBandwidth", allowed_meas_bw.to_string());
  j.write_bool("presenceAntennaPort1", presence_ant_port1);
  j.write_str("neighCellConfig", neigh_cell_cfg.to_string());
  if (offset_freq_present) {
    j.write_str("offsetFreq", offset_freq.to_string());
  }
  if (cells_to_rem_list_present) {
    j.start_array("cellsToRemoveList");
    for (const auto& e1 : cells_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (cells_to_add_mod_list_present) {
    j.start_array("cellsToAddModList");
    for (const auto& e1 : cells_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (black_cells_to_rem_list_present) {
    j.start_array("blackCellsToRemoveList");
    for (const auto& e1 : black_cells_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (black_cells_to_add_mod_list_present) {
    j.start_array("blackCellsToAddModList");
    for (const auto& e1 : black_cells_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cell_for_which_to_report_cgi_present) {
    j.write_int("cellForWhichToReportCGI", cell_for_which_to_report_cgi);
  }
  if (ext) {
    if (meas_cycle_scell_r10_present) {
      j.write_str("measCycleSCell-r10", meas_cycle_scell_r10.to_string());
    }
    if (meas_sf_pattern_cfg_neigh_r10.is_present()) {
      j.write_fieldname("measSubframePatternConfigNeigh-r10");
      meas_sf_pattern_cfg_neigh_r10->to_json(j);
    }
    if (wideband_rsrq_meas_r11_present) {
      j.write_bool("widebandRSRQ-Meas-r11", wideband_rsrq_meas_r11);
    }
    if (alt_ttt_cells_to_rem_list_r12.is_present()) {
      j.start_array("altTTT-CellsToRemoveList-r12");
      for (const auto& e1 : *alt_ttt_cells_to_rem_list_r12) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (alt_ttt_cells_to_add_mod_list_r12.is_present()) {
      j.start_array("altTTT-CellsToAddModList-r12");
      for (const auto& e1 : *alt_ttt_cells_to_add_mod_list_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (t312_r12.is_present()) {
      j.write_fieldname("t312-r12");
      t312_r12->to_json(j);
    }
    if (reduced_meas_performance_r12_present) {
      j.write_bool("reducedMeasPerformance-r12", reduced_meas_performance_r12);
    }
    if (meas_ds_cfg_r12.is_present()) {
      j.write_fieldname("measDS-Config-r12");
      meas_ds_cfg_r12->to_json(j);
    }
    if (white_cells_to_rem_list_r13.is_present()) {
      j.start_array("whiteCellsToRemoveList-r13");
      for (const auto& e1 : *white_cells_to_rem_list_r13) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (white_cells_to_add_mod_list_r13.is_present()) {
      j.start_array("whiteCellsToAddModList-r13");
      for (const auto& e1 : *white_cells_to_add_mod_list_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (rmtc_cfg_r13.is_present()) {
      j.write_fieldname("rmtc-Config-r13");
      rmtc_cfg_r13->to_json(j);
    }
    if (carrier_freq_r13_present) {
      j.write_int("carrierFreq-r13", carrier_freq_r13);
    }
    if (tx_res_pool_to_rem_list_r14.is_present()) {
      j.start_array("tx-ResourcePoolToRemoveList-r14");
      for (const auto& e1 : *tx_res_pool_to_rem_list_r14) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (tx_res_pool_to_add_list_r14.is_present()) {
      j.start_array("tx-ResourcePoolToAddList-r14");
      for (const auto& e1 : *tx_res_pool_to_add_list_r14) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (fembms_mixed_carrier_r14_present) {
      j.write_bool("fembms-MixedCarrier-r14", fembms_mixed_carrier_r14);
    }
    if (meas_sensing_cfg_r15.is_present()) {
      j.write_fieldname("measSensing-Config-r15");
      meas_sensing_cfg_r15->to_json(j);
    }
  }
  j.end_obj();
}
bool meas_obj_eutra_s::operator==(const meas_obj_eutra_s& other) const
{
  return ext == other.ext and carrier_freq == other.carrier_freq and allowed_meas_bw == other.allowed_meas_bw and
         presence_ant_port1 == other.presence_ant_port1 and neigh_cell_cfg == other.neigh_cell_cfg and
         offset_freq_present == other.offset_freq_present and
         (not offset_freq_present or offset_freq == other.offset_freq) and
         cells_to_rem_list_present == other.cells_to_rem_list_present and
         (not cells_to_rem_list_present or cells_to_rem_list == other.cells_to_rem_list) and
         cells_to_add_mod_list_present == other.cells_to_add_mod_list_present and
         (not cells_to_add_mod_list_present or cells_to_add_mod_list == other.cells_to_add_mod_list) and
         black_cells_to_rem_list_present == other.black_cells_to_rem_list_present and
         (not black_cells_to_rem_list_present or black_cells_to_rem_list == other.black_cells_to_rem_list) and
         black_cells_to_add_mod_list_present == other.black_cells_to_add_mod_list_present and
         (not black_cells_to_add_mod_list_present or
          black_cells_to_add_mod_list == other.black_cells_to_add_mod_list) and
         cell_for_which_to_report_cgi_present == other.cell_for_which_to_report_cgi_present and
         (not cell_for_which_to_report_cgi_present or
          cell_for_which_to_report_cgi == other.cell_for_which_to_report_cgi) and
         (not ext or
          (meas_cycle_scell_r10_present == other.meas_cycle_scell_r10_present and
           (not meas_cycle_scell_r10_present or meas_cycle_scell_r10 == other.meas_cycle_scell_r10) and
           meas_sf_pattern_cfg_neigh_r10.is_present() == other.meas_sf_pattern_cfg_neigh_r10.is_present() and
           (not meas_sf_pattern_cfg_neigh_r10.is_present() or
            *meas_sf_pattern_cfg_neigh_r10 == *other.meas_sf_pattern_cfg_neigh_r10) and
           wideband_rsrq_meas_r11_present == other.wideband_rsrq_meas_r11_present and
           (not wideband_rsrq_meas_r11_present or wideband_rsrq_meas_r11 == other.wideband_rsrq_meas_r11) and
           alt_ttt_cells_to_rem_list_r12.is_present() == other.alt_ttt_cells_to_rem_list_r12.is_present() and
           (not alt_ttt_cells_to_rem_list_r12.is_present() or
            *alt_ttt_cells_to_rem_list_r12 == *other.alt_ttt_cells_to_rem_list_r12) and
           alt_ttt_cells_to_add_mod_list_r12.is_present() == other.alt_ttt_cells_to_add_mod_list_r12.is_present() and
           (not alt_ttt_cells_to_add_mod_list_r12.is_present() or
            *alt_ttt_cells_to_add_mod_list_r12 == *other.alt_ttt_cells_to_add_mod_list_r12) and
           t312_r12.is_present() == other.t312_r12.is_present() and
           (not t312_r12.is_present() or *t312_r12 == *other.t312_r12) and
           reduced_meas_performance_r12_present == other.reduced_meas_performance_r12_present and
           (not reduced_meas_performance_r12_present or
            reduced_meas_performance_r12 == other.reduced_meas_performance_r12) and
           meas_ds_cfg_r12.is_present() == other.meas_ds_cfg_r12.is_present() and
           (not meas_ds_cfg_r12.is_present() or *meas_ds_cfg_r12 == *other.meas_ds_cfg_r12) and
           white_cells_to_rem_list_r13.is_present() == other.white_cells_to_rem_list_r13.is_present() and
           (not white_cells_to_rem_list_r13.is_present() or
            *white_cells_to_rem_list_r13 == *other.white_cells_to_rem_list_r13) and
           white_cells_to_add_mod_list_r13.is_present() == other.white_cells_to_add_mod_list_r13.is_present() and
           (not white_cells_to_add_mod_list_r13.is_present() or
            *white_cells_to_add_mod_list_r13 == *other.white_cells_to_add_mod_list_r13) and
           rmtc_cfg_r13.is_present() == other.rmtc_cfg_r13.is_present() and
           (not rmtc_cfg_r13.is_present() or *rmtc_cfg_r13 == *other.rmtc_cfg_r13) and
           carrier_freq_r13_present == other.carrier_freq_r13_present and
           (not carrier_freq_r13_present or carrier_freq_r13 == other.carrier_freq_r13) and
           tx_res_pool_to_rem_list_r14.is_present() == other.tx_res_pool_to_rem_list_r14.is_present() and
           (not tx_res_pool_to_rem_list_r14.is_present() or
            *tx_res_pool_to_rem_list_r14 == *other.tx_res_pool_to_rem_list_r14) and
           tx_res_pool_to_add_list_r14.is_present() == other.tx_res_pool_to_add_list_r14.is_present() and
           (not tx_res_pool_to_add_list_r14.is_present() or
            *tx_res_pool_to_add_list_r14 == *other.tx_res_pool_to_add_list_r14) and
           fembms_mixed_carrier_r14_present == other.fembms_mixed_carrier_r14_present and
           (not fembms_mixed_carrier_r14_present or fembms_mixed_carrier_r14 == other.fembms_mixed_carrier_r14) and
           meas_sensing_cfg_r15.is_present() == other.meas_sensing_cfg_r15.is_present() and
           (not meas_sensing_cfg_r15.is_present() or *meas_sensing_cfg_r15 == *other.meas_sensing_cfg_r15)));
}

void meas_obj_eutra_s::t312_r12_c_::set(types::options e)
{
  type_ = e;
}
void meas_obj_eutra_s::t312_r12_c_::set_release()
{
  set(types::release);
}
meas_obj_eutra_s::t312_r12_c_::setup_e_& meas_obj_eutra_s::t312_r12_c_::set_setup()
{
  set(types::setup);
  return c;
}
void meas_obj_eutra_s::t312_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_str("setup", c.to_string());
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_eutra_s::t312_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_obj_eutra_s::t312_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_eutra_s::t312_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_eutra_s::t312_r12_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "meas_obj_eutra_s::t312_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool meas_obj_eutra_s::t312_r12_c_::operator==(const t312_r12_c_& other) const
{
  return type() == other.type() and c == other.c;
}

const char* meas_obj_eutra_s::t312_r12_c_::setup_opts::to_string() const
{
  static const char* options[] = {"ms0", "ms50", "ms100", "ms200", "ms300", "ms400", "ms500", "ms1000"};
  return convert_enum_idx(options, 8, value, "meas_obj_eutra_s::t312_r12_c_::setup_e_");
}
uint16_t meas_obj_eutra_s::t312_r12_c_::setup_opts::to_number() const
{
  static const uint16_t options[] = {0, 50, 100, 200, 300, 400, 500, 1000};
  return map_enum_number(options, 8, value, "meas_obj_eutra_s::t312_r12_c_::setup_e_");
}

// MeasObjectEUTRA-v9e0 ::= SEQUENCE
SRSASN_CODE meas_obj_eutra_v9e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, carrier_freq_v9e0, (uint32_t)65536u, (uint32_t)262143u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_eutra_v9e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(carrier_freq_v9e0, bref, (uint32_t)65536u, (uint32_t)262143u));

  return SRSASN_SUCCESS;
}
void meas_obj_eutra_v9e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-v9e0", carrier_freq_v9e0);
  j.end_obj();
}

// MeasObjectGERAN ::= SEQUENCE
SRSASN_CODE meas_obj_geran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(offset_freq_present, 1));
  HANDLE_CODE(bref.pack(ncc_permitted_present, 1));
  HANDLE_CODE(bref.pack(cell_for_which_to_report_cgi_present, 1));

  HANDLE_CODE(carrier_freqs.pack(bref));
  if (offset_freq_present) {
    HANDLE_CODE(pack_integer(bref, offset_freq, (int8_t)-15, (int8_t)15));
  }
  if (ncc_permitted_present) {
    HANDLE_CODE(ncc_permitted.pack(bref));
  }
  if (cell_for_which_to_report_cgi_present) {
    HANDLE_CODE(cell_for_which_to_report_cgi.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_geran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(offset_freq_present, 1));
  HANDLE_CODE(bref.unpack(ncc_permitted_present, 1));
  HANDLE_CODE(bref.unpack(cell_for_which_to_report_cgi_present, 1));

  HANDLE_CODE(carrier_freqs.unpack(bref));
  if (offset_freq_present) {
    HANDLE_CODE(unpack_integer(offset_freq, bref, (int8_t)-15, (int8_t)15));
  }
  if (ncc_permitted_present) {
    HANDLE_CODE(ncc_permitted.unpack(bref));
  }
  if (cell_for_which_to_report_cgi_present) {
    HANDLE_CODE(cell_for_which_to_report_cgi.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_obj_geran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("carrierFreqs");
  carrier_freqs.to_json(j);
  if (offset_freq_present) {
    j.write_int("offsetFreq", offset_freq);
  }
  if (ncc_permitted_present) {
    j.write_str("ncc-Permitted", ncc_permitted.to_string());
  }
  if (cell_for_which_to_report_cgi_present) {
    j.write_fieldname("cellForWhichToReportCGI");
    cell_for_which_to_report_cgi.to_json(j);
  }
  j.end_obj();
}
bool meas_obj_geran_s::operator==(const meas_obj_geran_s& other) const
{
  return ext == other.ext and carrier_freqs == other.carrier_freqs and
         offset_freq_present == other.offset_freq_present and
         (not offset_freq_present or offset_freq == other.offset_freq) and
         ncc_permitted_present == other.ncc_permitted_present and
         (not ncc_permitted_present or ncc_permitted == other.ncc_permitted) and
         cell_for_which_to_report_cgi_present == other.cell_for_which_to_report_cgi_present and
         (not cell_for_which_to_report_cgi_present or
          cell_for_which_to_report_cgi == other.cell_for_which_to_report_cgi);
}

// MeasObjectNR-r15 ::= SEQUENCE
SRSASN_CODE meas_obj_nr_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(thresh_rs_idx_r15_present, 1));
  HANDLE_CODE(bref.pack(max_rs_idx_cell_qual_r15_present, 1));
  HANDLE_CODE(bref.pack(offset_freq_r15_present, 1));
  HANDLE_CODE(bref.pack(black_cells_to_rem_list_r15_present, 1));
  HANDLE_CODE(bref.pack(black_cells_to_add_mod_list_r15_present, 1));
  HANDLE_CODE(bref.pack(cells_for_which_to_report_sftd_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r15, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(rs_cfg_ssb_r15.pack(bref));
  if (thresh_rs_idx_r15_present) {
    HANDLE_CODE(thresh_rs_idx_r15.pack(bref));
  }
  if (max_rs_idx_cell_qual_r15_present) {
    HANDLE_CODE(pack_integer(bref, max_rs_idx_cell_qual_r15, (uint8_t)1u, (uint8_t)16u));
  }
  if (offset_freq_r15_present) {
    HANDLE_CODE(pack_integer(bref, offset_freq_r15, (int8_t)-15, (int8_t)15));
  }
  if (black_cells_to_rem_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, black_cells_to_rem_list_r15, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (black_cells_to_add_mod_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, black_cells_to_add_mod_list_r15, 1, 32));
  }
  HANDLE_CODE(pack_integer(bref, quant_cfg_set_r15, (uint8_t)1u, (uint8_t)2u));
  if (cells_for_which_to_report_sftd_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_for_which_to_report_sftd_r15, 1, 3, integer_packer<uint16_t>(0, 1007)));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= cell_for_which_to_report_cgi_r15_present;
    group_flags[0] |= derive_ssb_idx_from_cell_r15_present;
    group_flags[0] |= ss_rssi_meas_r15.is_present();
    group_flags[0] |= band_nr_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cell_for_which_to_report_cgi_r15_present, 1));
      HANDLE_CODE(bref.pack(derive_ssb_idx_from_cell_r15_present, 1));
      HANDLE_CODE(bref.pack(ss_rssi_meas_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(band_nr_r15.is_present(), 1));
      if (cell_for_which_to_report_cgi_r15_present) {
        HANDLE_CODE(pack_integer(bref, cell_for_which_to_report_cgi_r15, (uint16_t)0u, (uint16_t)1007u));
      }
      if (derive_ssb_idx_from_cell_r15_present) {
        HANDLE_CODE(bref.pack(derive_ssb_idx_from_cell_r15, 1));
      }
      if (ss_rssi_meas_r15.is_present()) {
        HANDLE_CODE(ss_rssi_meas_r15->pack(bref));
      }
      if (band_nr_r15.is_present()) {
        HANDLE_CODE(band_nr_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_nr_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(thresh_rs_idx_r15_present, 1));
  HANDLE_CODE(bref.unpack(max_rs_idx_cell_qual_r15_present, 1));
  HANDLE_CODE(bref.unpack(offset_freq_r15_present, 1));
  HANDLE_CODE(bref.unpack(black_cells_to_rem_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(black_cells_to_add_mod_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(cells_for_which_to_report_sftd_r15_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r15, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(rs_cfg_ssb_r15.unpack(bref));
  if (thresh_rs_idx_r15_present) {
    HANDLE_CODE(thresh_rs_idx_r15.unpack(bref));
  }
  if (max_rs_idx_cell_qual_r15_present) {
    HANDLE_CODE(unpack_integer(max_rs_idx_cell_qual_r15, bref, (uint8_t)1u, (uint8_t)16u));
  }
  if (offset_freq_r15_present) {
    HANDLE_CODE(unpack_integer(offset_freq_r15, bref, (int8_t)-15, (int8_t)15));
  }
  if (black_cells_to_rem_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(black_cells_to_rem_list_r15, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (black_cells_to_add_mod_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(black_cells_to_add_mod_list_r15, bref, 1, 32));
  }
  HANDLE_CODE(unpack_integer(quant_cfg_set_r15, bref, (uint8_t)1u, (uint8_t)2u));
  if (cells_for_which_to_report_sftd_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_for_which_to_report_sftd_r15, bref, 1, 3, integer_packer<uint16_t>(0, 1007)));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(cell_for_which_to_report_cgi_r15_present, 1));
      HANDLE_CODE(bref.unpack(derive_ssb_idx_from_cell_r15_present, 1));
      bool ss_rssi_meas_r15_present;
      HANDLE_CODE(bref.unpack(ss_rssi_meas_r15_present, 1));
      ss_rssi_meas_r15.set_present(ss_rssi_meas_r15_present);
      bool band_nr_r15_present;
      HANDLE_CODE(bref.unpack(band_nr_r15_present, 1));
      band_nr_r15.set_present(band_nr_r15_present);
      if (cell_for_which_to_report_cgi_r15_present) {
        HANDLE_CODE(unpack_integer(cell_for_which_to_report_cgi_r15, bref, (uint16_t)0u, (uint16_t)1007u));
      }
      if (derive_ssb_idx_from_cell_r15_present) {
        HANDLE_CODE(bref.unpack(derive_ssb_idx_from_cell_r15, 1));
      }
      if (ss_rssi_meas_r15.is_present()) {
        HANDLE_CODE(ss_rssi_meas_r15->unpack(bref));
      }
      if (band_nr_r15.is_present()) {
        HANDLE_CODE(band_nr_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_obj_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r15", carrier_freq_r15);
  j.write_fieldname("rs-ConfigSSB-r15");
  rs_cfg_ssb_r15.to_json(j);
  if (thresh_rs_idx_r15_present) {
    j.write_fieldname("threshRS-Index-r15");
    thresh_rs_idx_r15.to_json(j);
  }
  if (max_rs_idx_cell_qual_r15_present) {
    j.write_int("maxRS-IndexCellQual-r15", max_rs_idx_cell_qual_r15);
  }
  if (offset_freq_r15_present) {
    j.write_int("offsetFreq-r15", offset_freq_r15);
  }
  if (black_cells_to_rem_list_r15_present) {
    j.start_array("blackCellsToRemoveList-r15");
    for (const auto& e1 : black_cells_to_rem_list_r15) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (black_cells_to_add_mod_list_r15_present) {
    j.start_array("blackCellsToAddModList-r15");
    for (const auto& e1 : black_cells_to_add_mod_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_int("quantityConfigSet-r15", quant_cfg_set_r15);
  if (cells_for_which_to_report_sftd_r15_present) {
    j.start_array("cellsForWhichToReportSFTD-r15");
    for (const auto& e1 : cells_for_which_to_report_sftd_r15) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (ext) {
    if (cell_for_which_to_report_cgi_r15_present) {
      j.write_int("cellForWhichToReportCGI-r15", cell_for_which_to_report_cgi_r15);
    }
    if (derive_ssb_idx_from_cell_r15_present) {
      j.write_bool("deriveSSB-IndexFromCell-r15", derive_ssb_idx_from_cell_r15);
    }
    if (ss_rssi_meas_r15.is_present()) {
      j.write_fieldname("ss-RSSI-Measurement-r15");
      ss_rssi_meas_r15->to_json(j);
    }
    if (band_nr_r15.is_present()) {
      j.write_fieldname("bandNR-r15");
      band_nr_r15->to_json(j);
    }
  }
  j.end_obj();
}
bool meas_obj_nr_r15_s::operator==(const meas_obj_nr_r15_s& other) const
{
  return ext == other.ext and carrier_freq_r15 == other.carrier_freq_r15 and rs_cfg_ssb_r15 == other.rs_cfg_ssb_r15 and
         thresh_rs_idx_r15_present == other.thresh_rs_idx_r15_present and
         (not thresh_rs_idx_r15_present or thresh_rs_idx_r15 == other.thresh_rs_idx_r15) and
         max_rs_idx_cell_qual_r15_present == other.max_rs_idx_cell_qual_r15_present and
         (not max_rs_idx_cell_qual_r15_present or max_rs_idx_cell_qual_r15 == other.max_rs_idx_cell_qual_r15) and
         offset_freq_r15_present == other.offset_freq_r15_present and
         (not offset_freq_r15_present or offset_freq_r15 == other.offset_freq_r15) and
         black_cells_to_rem_list_r15_present == other.black_cells_to_rem_list_r15_present and
         (not black_cells_to_rem_list_r15_present or
          black_cells_to_rem_list_r15 == other.black_cells_to_rem_list_r15) and
         black_cells_to_add_mod_list_r15_present == other.black_cells_to_add_mod_list_r15_present and
         (not black_cells_to_add_mod_list_r15_present or
          black_cells_to_add_mod_list_r15 == other.black_cells_to_add_mod_list_r15) and
         quant_cfg_set_r15 == other.quant_cfg_set_r15 and
         cells_for_which_to_report_sftd_r15_present == other.cells_for_which_to_report_sftd_r15_present and
         (not cells_for_which_to_report_sftd_r15_present or
          cells_for_which_to_report_sftd_r15 == other.cells_for_which_to_report_sftd_r15) and
         (not ext or (cell_for_which_to_report_cgi_r15_present == other.cell_for_which_to_report_cgi_r15_present and
                      (not cell_for_which_to_report_cgi_r15_present or
                       cell_for_which_to_report_cgi_r15 == other.cell_for_which_to_report_cgi_r15) and
                      derive_ssb_idx_from_cell_r15_present == other.derive_ssb_idx_from_cell_r15_present and
                      (not derive_ssb_idx_from_cell_r15_present or
                       derive_ssb_idx_from_cell_r15 == other.derive_ssb_idx_from_cell_r15) and
                      ss_rssi_meas_r15.is_present() == other.ss_rssi_meas_r15.is_present() and
                      (not ss_rssi_meas_r15.is_present() or *ss_rssi_meas_r15 == *other.ss_rssi_meas_r15) and
                      band_nr_r15.is_present() == other.band_nr_r15.is_present() and
                      (not band_nr_r15.is_present() or *band_nr_r15 == *other.band_nr_r15)));
}

void meas_obj_nr_r15_s::band_nr_r15_c_::set(types::options e)
{
  type_ = e;
}
void meas_obj_nr_r15_s::band_nr_r15_c_::set_release()
{
  set(types::release);
}
uint16_t& meas_obj_nr_r15_s::band_nr_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void meas_obj_nr_r15_s::band_nr_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_int("setup", c);
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_nr_r15_s::band_nr_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_obj_nr_r15_s::band_nr_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c, (uint16_t)1u, (uint16_t)1024u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_nr_r15_s::band_nr_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_nr_r15_s::band_nr_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c, bref, (uint16_t)1u, (uint16_t)1024u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_nr_r15_s::band_nr_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool meas_obj_nr_r15_s::band_nr_r15_c_::operator==(const band_nr_r15_c_& other) const
{
  return type() == other.type() and c == other.c;
}

// MeasObjectUTRA ::= SEQUENCE
SRSASN_CODE meas_obj_utra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(offset_freq_present, 1));
  HANDLE_CODE(bref.pack(cells_to_rem_list_present, 1));
  HANDLE_CODE(bref.pack(cells_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.pack(cell_for_which_to_report_cgi_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint16_t)0u, (uint16_t)16383u));
  if (offset_freq_present) {
    HANDLE_CODE(pack_integer(bref, offset_freq, (int8_t)-15, (int8_t)15));
  }
  if (cells_to_rem_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_to_rem_list, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cells_to_add_mod_list_present) {
    HANDLE_CODE(cells_to_add_mod_list.pack(bref));
  }
  if (cell_for_which_to_report_cgi_present) {
    HANDLE_CODE(cell_for_which_to_report_cgi.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= csg_allowed_report_cells_v930.is_present();
    group_flags[1] |= reduced_meas_performance_r12_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(csg_allowed_report_cells_v930.is_present(), 1));
      if (csg_allowed_report_cells_v930.is_present()) {
        HANDLE_CODE(csg_allowed_report_cells_v930->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(reduced_meas_performance_r12_present, 1));
      if (reduced_meas_performance_r12_present) {
        HANDLE_CODE(bref.pack(reduced_meas_performance_r12, 1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_utra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(offset_freq_present, 1));
  HANDLE_CODE(bref.unpack(cells_to_rem_list_present, 1));
  HANDLE_CODE(bref.unpack(cells_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(cell_for_which_to_report_cgi_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint16_t)0u, (uint16_t)16383u));
  if (offset_freq_present) {
    HANDLE_CODE(unpack_integer(offset_freq, bref, (int8_t)-15, (int8_t)15));
  }
  if (cells_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_to_rem_list, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cells_to_add_mod_list_present) {
    HANDLE_CODE(cells_to_add_mod_list.unpack(bref));
  }
  if (cell_for_which_to_report_cgi_present) {
    HANDLE_CODE(cell_for_which_to_report_cgi.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool csg_allowed_report_cells_v930_present;
      HANDLE_CODE(bref.unpack(csg_allowed_report_cells_v930_present, 1));
      csg_allowed_report_cells_v930.set_present(csg_allowed_report_cells_v930_present);
      if (csg_allowed_report_cells_v930.is_present()) {
        HANDLE_CODE(csg_allowed_report_cells_v930->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(reduced_meas_performance_r12_present, 1));
      if (reduced_meas_performance_r12_present) {
        HANDLE_CODE(bref.unpack(reduced_meas_performance_r12, 1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_obj_utra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  if (offset_freq_present) {
    j.write_int("offsetFreq", offset_freq);
  }
  if (cells_to_rem_list_present) {
    j.start_array("cellsToRemoveList");
    for (const auto& e1 : cells_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (cells_to_add_mod_list_present) {
    j.write_fieldname("cellsToAddModList");
    cells_to_add_mod_list.to_json(j);
  }
  if (cell_for_which_to_report_cgi_present) {
    j.write_fieldname("cellForWhichToReportCGI");
    cell_for_which_to_report_cgi.to_json(j);
  }
  if (ext) {
    if (csg_allowed_report_cells_v930.is_present()) {
      j.write_fieldname("csg-allowedReportingCells-v930");
      csg_allowed_report_cells_v930->to_json(j);
    }
    if (reduced_meas_performance_r12_present) {
      j.write_bool("reducedMeasPerformance-r12", reduced_meas_performance_r12);
    }
  }
  j.end_obj();
}
bool meas_obj_utra_s::operator==(const meas_obj_utra_s& other) const
{
  return ext == other.ext and carrier_freq == other.carrier_freq and
         offset_freq_present == other.offset_freq_present and
         (not offset_freq_present or offset_freq == other.offset_freq) and
         cells_to_rem_list_present == other.cells_to_rem_list_present and
         (not cells_to_rem_list_present or cells_to_rem_list == other.cells_to_rem_list) and
         cells_to_add_mod_list_present == other.cells_to_add_mod_list_present and
         (not cells_to_add_mod_list_present or cells_to_add_mod_list == other.cells_to_add_mod_list) and
         cell_for_which_to_report_cgi_present == other.cell_for_which_to_report_cgi_present and
         (not cell_for_which_to_report_cgi_present or
          cell_for_which_to_report_cgi == other.cell_for_which_to_report_cgi) and
         (not ext or (csg_allowed_report_cells_v930.is_present() == other.csg_allowed_report_cells_v930.is_present() and
                      (not csg_allowed_report_cells_v930.is_present() or
                       *csg_allowed_report_cells_v930 == *other.csg_allowed_report_cells_v930) and
                      reduced_meas_performance_r12_present == other.reduced_meas_performance_r12_present and
                      (not reduced_meas_performance_r12_present or
                       reduced_meas_performance_r12 == other.reduced_meas_performance_r12)));
}

void meas_obj_utra_s::cells_to_add_mod_list_c_::destroy_()
{
  switch (type_) {
    case types::cells_to_add_mod_list_utra_fdd:
      c.destroy<cells_to_add_mod_list_utra_fdd_l>();
      break;
    case types::cells_to_add_mod_list_utra_tdd:
      c.destroy<cells_to_add_mod_list_utra_tdd_l>();
      break;
    default:
      break;
  }
}
void meas_obj_utra_s::cells_to_add_mod_list_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cells_to_add_mod_list_utra_fdd:
      c.init<cells_to_add_mod_list_utra_fdd_l>();
      break;
    case types::cells_to_add_mod_list_utra_tdd:
      c.init<cells_to_add_mod_list_utra_tdd_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_utra_s::cells_to_add_mod_list_c_");
  }
}
meas_obj_utra_s::cells_to_add_mod_list_c_::cells_to_add_mod_list_c_(
    const meas_obj_utra_s::cells_to_add_mod_list_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cells_to_add_mod_list_utra_fdd:
      c.init(other.c.get<cells_to_add_mod_list_utra_fdd_l>());
      break;
    case types::cells_to_add_mod_list_utra_tdd:
      c.init(other.c.get<cells_to_add_mod_list_utra_tdd_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_utra_s::cells_to_add_mod_list_c_");
  }
}
meas_obj_utra_s::cells_to_add_mod_list_c_&
meas_obj_utra_s::cells_to_add_mod_list_c_::operator=(const meas_obj_utra_s::cells_to_add_mod_list_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cells_to_add_mod_list_utra_fdd:
      c.set(other.c.get<cells_to_add_mod_list_utra_fdd_l>());
      break;
    case types::cells_to_add_mod_list_utra_tdd:
      c.set(other.c.get<cells_to_add_mod_list_utra_tdd_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_utra_s::cells_to_add_mod_list_c_");
  }

  return *this;
}
cells_to_add_mod_list_utra_fdd_l& meas_obj_utra_s::cells_to_add_mod_list_c_::set_cells_to_add_mod_list_utra_fdd()
{
  set(types::cells_to_add_mod_list_utra_fdd);
  return c.get<cells_to_add_mod_list_utra_fdd_l>();
}
cells_to_add_mod_list_utra_tdd_l& meas_obj_utra_s::cells_to_add_mod_list_c_::set_cells_to_add_mod_list_utra_tdd()
{
  set(types::cells_to_add_mod_list_utra_tdd);
  return c.get<cells_to_add_mod_list_utra_tdd_l>();
}
void meas_obj_utra_s::cells_to_add_mod_list_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cells_to_add_mod_list_utra_fdd:
      j.start_array("cellsToAddModListUTRA-FDD");
      for (const auto& e1 : c.get<cells_to_add_mod_list_utra_fdd_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::cells_to_add_mod_list_utra_tdd:
      j.start_array("cellsToAddModListUTRA-TDD");
      for (const auto& e1 : c.get<cells_to_add_mod_list_utra_tdd_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_utra_s::cells_to_add_mod_list_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_obj_utra_s::cells_to_add_mod_list_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cells_to_add_mod_list_utra_fdd:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<cells_to_add_mod_list_utra_fdd_l>(), 1, 32));
      break;
    case types::cells_to_add_mod_list_utra_tdd:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<cells_to_add_mod_list_utra_tdd_l>(), 1, 32));
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_utra_s::cells_to_add_mod_list_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_utra_s::cells_to_add_mod_list_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cells_to_add_mod_list_utra_fdd:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<cells_to_add_mod_list_utra_fdd_l>(), bref, 1, 32));
      break;
    case types::cells_to_add_mod_list_utra_tdd:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<cells_to_add_mod_list_utra_tdd_l>(), bref, 1, 32));
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_utra_s::cells_to_add_mod_list_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool meas_obj_utra_s::cells_to_add_mod_list_c_::operator==(const cells_to_add_mod_list_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::cells_to_add_mod_list_utra_fdd:
      return c.get<cells_to_add_mod_list_utra_fdd_l>() == other.c.get<cells_to_add_mod_list_utra_fdd_l>();
    case types::cells_to_add_mod_list_utra_tdd:
      return c.get<cells_to_add_mod_list_utra_tdd_l>() == other.c.get<cells_to_add_mod_list_utra_tdd_l>();
    default:
      return true;
  }
  return true;
}

void meas_obj_utra_s::cell_for_which_to_report_cgi_c_::destroy_() {}
void meas_obj_utra_s::cell_for_which_to_report_cgi_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
meas_obj_utra_s::cell_for_which_to_report_cgi_c_::cell_for_which_to_report_cgi_c_(
    const meas_obj_utra_s::cell_for_which_to_report_cgi_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::utra_fdd:
      c.init(other.c.get<uint16_t>());
      break;
    case types::utra_tdd:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_utra_s::cell_for_which_to_report_cgi_c_");
  }
}
meas_obj_utra_s::cell_for_which_to_report_cgi_c_& meas_obj_utra_s::cell_for_which_to_report_cgi_c_::operator=(
    const meas_obj_utra_s::cell_for_which_to_report_cgi_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::utra_fdd:
      c.set(other.c.get<uint16_t>());
      break;
    case types::utra_tdd:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_utra_s::cell_for_which_to_report_cgi_c_");
  }

  return *this;
}
uint16_t& meas_obj_utra_s::cell_for_which_to_report_cgi_c_::set_utra_fdd()
{
  set(types::utra_fdd);
  return c.get<uint16_t>();
}
uint8_t& meas_obj_utra_s::cell_for_which_to_report_cgi_c_::set_utra_tdd()
{
  set(types::utra_tdd);
  return c.get<uint8_t>();
}
void meas_obj_utra_s::cell_for_which_to_report_cgi_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::utra_fdd:
      j.write_int("utra-FDD", c.get<uint16_t>());
      break;
    case types::utra_tdd:
      j.write_int("utra-TDD", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_utra_s::cell_for_which_to_report_cgi_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_obj_utra_s::cell_for_which_to_report_cgi_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::utra_fdd:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::utra_tdd:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_utra_s::cell_for_which_to_report_cgi_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_utra_s::cell_for_which_to_report_cgi_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::utra_fdd:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::utra_tdd:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_utra_s::cell_for_which_to_report_cgi_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool meas_obj_utra_s::cell_for_which_to_report_cgi_c_::operator==(const cell_for_which_to_report_cgi_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::utra_fdd:
      return c.get<uint16_t>() == other.c.get<uint16_t>();
    case types::utra_tdd:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    default:
      return true;
  }
  return true;
}

// MeasObjectWLAN-r13 ::= SEQUENCE
SRSASN_CODE meas_obj_wlan_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(carrier_freq_r13_present, 1));
  HANDLE_CODE(bref.pack(wlan_to_add_mod_list_r13_present, 1));
  HANDLE_CODE(bref.pack(wlan_to_rem_list_r13_present, 1));

  if (carrier_freq_r13_present) {
    HANDLE_CODE(carrier_freq_r13.pack(bref));
  }
  if (wlan_to_add_mod_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, wlan_to_add_mod_list_r13, 1, 32));
  }
  if (wlan_to_rem_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, wlan_to_rem_list_r13, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_wlan_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(carrier_freq_r13_present, 1));
  HANDLE_CODE(bref.unpack(wlan_to_add_mod_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(wlan_to_rem_list_r13_present, 1));

  if (carrier_freq_r13_present) {
    HANDLE_CODE(carrier_freq_r13.unpack(bref));
  }
  if (wlan_to_add_mod_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(wlan_to_add_mod_list_r13, bref, 1, 32));
  }
  if (wlan_to_rem_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(wlan_to_rem_list_r13, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void meas_obj_wlan_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_freq_r13_present) {
    j.write_fieldname("carrierFreq-r13");
    carrier_freq_r13.to_json(j);
  }
  if (wlan_to_add_mod_list_r13_present) {
    j.start_array("wlan-ToAddModList-r13");
    for (const auto& e1 : wlan_to_add_mod_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (wlan_to_rem_list_r13_present) {
    j.start_array("wlan-ToRemoveList-r13");
    for (const auto& e1 : wlan_to_rem_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
bool meas_obj_wlan_r13_s::operator==(const meas_obj_wlan_r13_s& other) const
{
  return ext == other.ext and carrier_freq_r13_present == other.carrier_freq_r13_present and
         (not carrier_freq_r13_present or carrier_freq_r13 == other.carrier_freq_r13) and
         wlan_to_add_mod_list_r13_present == other.wlan_to_add_mod_list_r13_present and
         (not wlan_to_add_mod_list_r13_present or wlan_to_add_mod_list_r13 == other.wlan_to_add_mod_list_r13) and
         wlan_to_rem_list_r13_present == other.wlan_to_rem_list_r13_present and
         (not wlan_to_rem_list_r13_present or wlan_to_rem_list_r13 == other.wlan_to_rem_list_r13);
}

void meas_obj_wlan_r13_s::carrier_freq_r13_c_::destroy_()
{
  switch (type_) {
    case types::band_ind_list_wlan_r13:
      c.destroy<band_ind_list_wlan_r13_l_>();
      break;
    case types::carrier_info_list_wlan_r13:
      c.destroy<carrier_info_list_wlan_r13_l_>();
      break;
    default:
      break;
  }
}
void meas_obj_wlan_r13_s::carrier_freq_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::band_ind_list_wlan_r13:
      c.init<band_ind_list_wlan_r13_l_>();
      break;
    case types::carrier_info_list_wlan_r13:
      c.init<carrier_info_list_wlan_r13_l_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_wlan_r13_s::carrier_freq_r13_c_");
  }
}
meas_obj_wlan_r13_s::carrier_freq_r13_c_::carrier_freq_r13_c_(const meas_obj_wlan_r13_s::carrier_freq_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::band_ind_list_wlan_r13:
      c.init(other.c.get<band_ind_list_wlan_r13_l_>());
      break;
    case types::carrier_info_list_wlan_r13:
      c.init(other.c.get<carrier_info_list_wlan_r13_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_wlan_r13_s::carrier_freq_r13_c_");
  }
}
meas_obj_wlan_r13_s::carrier_freq_r13_c_&
meas_obj_wlan_r13_s::carrier_freq_r13_c_::operator=(const meas_obj_wlan_r13_s::carrier_freq_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::band_ind_list_wlan_r13:
      c.set(other.c.get<band_ind_list_wlan_r13_l_>());
      break;
    case types::carrier_info_list_wlan_r13:
      c.set(other.c.get<carrier_info_list_wlan_r13_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_wlan_r13_s::carrier_freq_r13_c_");
  }

  return *this;
}
meas_obj_wlan_r13_s::carrier_freq_r13_c_::band_ind_list_wlan_r13_l_&
meas_obj_wlan_r13_s::carrier_freq_r13_c_::set_band_ind_list_wlan_r13()
{
  set(types::band_ind_list_wlan_r13);
  return c.get<band_ind_list_wlan_r13_l_>();
}
meas_obj_wlan_r13_s::carrier_freq_r13_c_::carrier_info_list_wlan_r13_l_&
meas_obj_wlan_r13_s::carrier_freq_r13_c_::set_carrier_info_list_wlan_r13()
{
  set(types::carrier_info_list_wlan_r13);
  return c.get<carrier_info_list_wlan_r13_l_>();
}
void meas_obj_wlan_r13_s::carrier_freq_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::band_ind_list_wlan_r13:
      j.start_array("bandIndicatorListWLAN-r13");
      for (const auto& e1 : c.get<band_ind_list_wlan_r13_l_>()) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      break;
    case types::carrier_info_list_wlan_r13:
      j.start_array("carrierInfoListWLAN-r13");
      for (const auto& e1 : c.get<carrier_info_list_wlan_r13_l_>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_wlan_r13_s::carrier_freq_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_obj_wlan_r13_s::carrier_freq_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::band_ind_list_wlan_r13:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<band_ind_list_wlan_r13_l_>(), 1, 8));
      break;
    case types::carrier_info_list_wlan_r13:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<carrier_info_list_wlan_r13_l_>(), 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_wlan_r13_s::carrier_freq_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_wlan_r13_s::carrier_freq_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::band_ind_list_wlan_r13:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<band_ind_list_wlan_r13_l_>(), bref, 1, 8));
      break;
    case types::carrier_info_list_wlan_r13:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<carrier_info_list_wlan_r13_l_>(), bref, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_wlan_r13_s::carrier_freq_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool meas_obj_wlan_r13_s::carrier_freq_r13_c_::operator==(const carrier_freq_r13_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::band_ind_list_wlan_r13:
      return c.get<band_ind_list_wlan_r13_l_>() == other.c.get<band_ind_list_wlan_r13_l_>();
    case types::carrier_info_list_wlan_r13:
      return c.get<carrier_info_list_wlan_r13_l_>() == other.c.get<carrier_info_list_wlan_r13_l_>();
    default:
      return true;
  }
  return true;
}

// QuantityConfigNR-r15 ::= SEQUENCE
SRSASN_CODE quant_cfg_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_quant_rs_idx_nr_r15_present, 1));

  HANDLE_CODE(meas_quant_cell_nr_r15.pack(bref));
  if (meas_quant_rs_idx_nr_r15_present) {
    HANDLE_CODE(meas_quant_rs_idx_nr_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_quant_rs_idx_nr_r15_present, 1));

  HANDLE_CODE(meas_quant_cell_nr_r15.unpack(bref));
  if (meas_quant_rs_idx_nr_r15_present) {
    HANDLE_CODE(meas_quant_rs_idx_nr_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void quant_cfg_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measQuantityCellNR-r15");
  meas_quant_cell_nr_r15.to_json(j);
  if (meas_quant_rs_idx_nr_r15_present) {
    j.write_fieldname("measQuantityRS-IndexNR-r15");
    meas_quant_rs_idx_nr_r15.to_json(j);
  }
  j.end_obj();
}
bool quant_cfg_nr_r15_s::operator==(const quant_cfg_nr_r15_s& other) const
{
  return meas_quant_cell_nr_r15 == other.meas_quant_cell_nr_r15 and
         meas_quant_rs_idx_nr_r15_present == other.meas_quant_rs_idx_nr_r15_present and
         (not meas_quant_rs_idx_nr_r15_present or meas_quant_rs_idx_nr_r15 == other.meas_quant_rs_idx_nr_r15);
}

SRSASN_CODE eutra_event_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(event_id.pack(bref));
  HANDLE_CODE(pack_integer(bref, hysteresis, (uint8_t)0u, (uint8_t)30u));
  HANDLE_CODE(time_to_trigger.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_event_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(event_id.unpack(bref));
  HANDLE_CODE(unpack_integer(hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
  HANDLE_CODE(time_to_trigger.unpack(bref));

  return SRSASN_SUCCESS;
}
void eutra_event_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("eventId");
  event_id.to_json(j);
  j.write_int("hysteresis", hysteresis);
  j.write_str("timeToTrigger", time_to_trigger.to_string());
  j.end_obj();
}
bool eutra_event_s::operator==(const eutra_event_s& other) const
{
  return event_id == other.event_id and hysteresis == other.hysteresis and time_to_trigger == other.time_to_trigger;
}

void eutra_event_s::event_id_c_::destroy_()
{
  switch (type_) {
    case types::event_a1:
      c.destroy<event_a1_s_>();
      break;
    case types::event_a2:
      c.destroy<event_a2_s_>();
      break;
    case types::event_a3:
      c.destroy<event_a3_s_>();
      break;
    case types::event_a4:
      c.destroy<event_a4_s_>();
      break;
    case types::event_a5:
      c.destroy<event_a5_s_>();
      break;
    case types::event_a6_r10:
      c.destroy<event_a6_r10_s_>();
      break;
    case types::event_c1_r12:
      c.destroy<event_c1_r12_s_>();
      break;
    case types::event_c2_r12:
      c.destroy<event_c2_r12_s_>();
      break;
    case types::event_v1_r14:
      c.destroy<event_v1_r14_s_>();
      break;
    case types::event_v2_r14:
      c.destroy<event_v2_r14_s_>();
      break;
    case types::event_h1_r15:
      c.destroy<event_h1_r15_s_>();
      break;
    case types::event_h2_r15:
      c.destroy<event_h2_r15_s_>();
      break;
    default:
      break;
  }
}
void eutra_event_s::event_id_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::event_a1:
      c.init<event_a1_s_>();
      break;
    case types::event_a2:
      c.init<event_a2_s_>();
      break;
    case types::event_a3:
      c.init<event_a3_s_>();
      break;
    case types::event_a4:
      c.init<event_a4_s_>();
      break;
    case types::event_a5:
      c.init<event_a5_s_>();
      break;
    case types::event_a6_r10:
      c.init<event_a6_r10_s_>();
      break;
    case types::event_c1_r12:
      c.init<event_c1_r12_s_>();
      break;
    case types::event_c2_r12:
      c.init<event_c2_r12_s_>();
      break;
    case types::event_v1_r14:
      c.init<event_v1_r14_s_>();
      break;
    case types::event_v2_r14:
      c.init<event_v2_r14_s_>();
      break;
    case types::event_h1_r15:
      c.init<event_h1_r15_s_>();
      break;
    case types::event_h2_r15:
      c.init<event_h2_r15_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutra_event_s::event_id_c_");
  }
}
eutra_event_s::event_id_c_::event_id_c_(const eutra_event_s::event_id_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::event_a1:
      c.init(other.c.get<event_a1_s_>());
      break;
    case types::event_a2:
      c.init(other.c.get<event_a2_s_>());
      break;
    case types::event_a3:
      c.init(other.c.get<event_a3_s_>());
      break;
    case types::event_a4:
      c.init(other.c.get<event_a4_s_>());
      break;
    case types::event_a5:
      c.init(other.c.get<event_a5_s_>());
      break;
    case types::event_a6_r10:
      c.init(other.c.get<event_a6_r10_s_>());
      break;
    case types::event_c1_r12:
      c.init(other.c.get<event_c1_r12_s_>());
      break;
    case types::event_c2_r12:
      c.init(other.c.get<event_c2_r12_s_>());
      break;
    case types::event_v1_r14:
      c.init(other.c.get<event_v1_r14_s_>());
      break;
    case types::event_v2_r14:
      c.init(other.c.get<event_v2_r14_s_>());
      break;
    case types::event_h1_r15:
      c.init(other.c.get<event_h1_r15_s_>());
      break;
    case types::event_h2_r15:
      c.init(other.c.get<event_h2_r15_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutra_event_s::event_id_c_");
  }
}
eutra_event_s::event_id_c_& eutra_event_s::event_id_c_::operator=(const eutra_event_s::event_id_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::event_a1:
      c.set(other.c.get<event_a1_s_>());
      break;
    case types::event_a2:
      c.set(other.c.get<event_a2_s_>());
      break;
    case types::event_a3:
      c.set(other.c.get<event_a3_s_>());
      break;
    case types::event_a4:
      c.set(other.c.get<event_a4_s_>());
      break;
    case types::event_a5:
      c.set(other.c.get<event_a5_s_>());
      break;
    case types::event_a6_r10:
      c.set(other.c.get<event_a6_r10_s_>());
      break;
    case types::event_c1_r12:
      c.set(other.c.get<event_c1_r12_s_>());
      break;
    case types::event_c2_r12:
      c.set(other.c.get<event_c2_r12_s_>());
      break;
    case types::event_v1_r14:
      c.set(other.c.get<event_v1_r14_s_>());
      break;
    case types::event_v2_r14:
      c.set(other.c.get<event_v2_r14_s_>());
      break;
    case types::event_h1_r15:
      c.set(other.c.get<event_h1_r15_s_>());
      break;
    case types::event_h2_r15:
      c.set(other.c.get<event_h2_r15_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutra_event_s::event_id_c_");
  }

  return *this;
}
eutra_event_s::event_id_c_::event_a1_s_& eutra_event_s::event_id_c_::set_event_a1()
{
  set(types::event_a1);
  return c.get<event_a1_s_>();
}
eutra_event_s::event_id_c_::event_a2_s_& eutra_event_s::event_id_c_::set_event_a2()
{
  set(types::event_a2);
  return c.get<event_a2_s_>();
}
eutra_event_s::event_id_c_::event_a3_s_& eutra_event_s::event_id_c_::set_event_a3()
{
  set(types::event_a3);
  return c.get<event_a3_s_>();
}
eutra_event_s::event_id_c_::event_a4_s_& eutra_event_s::event_id_c_::set_event_a4()
{
  set(types::event_a4);
  return c.get<event_a4_s_>();
}
eutra_event_s::event_id_c_::event_a5_s_& eutra_event_s::event_id_c_::set_event_a5()
{
  set(types::event_a5);
  return c.get<event_a5_s_>();
}
eutra_event_s::event_id_c_::event_a6_r10_s_& eutra_event_s::event_id_c_::set_event_a6_r10()
{
  set(types::event_a6_r10);
  return c.get<event_a6_r10_s_>();
}
eutra_event_s::event_id_c_::event_c1_r12_s_& eutra_event_s::event_id_c_::set_event_c1_r12()
{
  set(types::event_c1_r12);
  return c.get<event_c1_r12_s_>();
}
eutra_event_s::event_id_c_::event_c2_r12_s_& eutra_event_s::event_id_c_::set_event_c2_r12()
{
  set(types::event_c2_r12);
  return c.get<event_c2_r12_s_>();
}
eutra_event_s::event_id_c_::event_v1_r14_s_& eutra_event_s::event_id_c_::set_event_v1_r14()
{
  set(types::event_v1_r14);
  return c.get<event_v1_r14_s_>();
}
eutra_event_s::event_id_c_::event_v2_r14_s_& eutra_event_s::event_id_c_::set_event_v2_r14()
{
  set(types::event_v2_r14);
  return c.get<event_v2_r14_s_>();
}
eutra_event_s::event_id_c_::event_h1_r15_s_& eutra_event_s::event_id_c_::set_event_h1_r15()
{
  set(types::event_h1_r15);
  return c.get<event_h1_r15_s_>();
}
eutra_event_s::event_id_c_::event_h2_r15_s_& eutra_event_s::event_id_c_::set_event_h2_r15()
{
  set(types::event_h2_r15);
  return c.get<event_h2_r15_s_>();
}
void eutra_event_s::event_id_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::event_a1:
      j.write_fieldname("eventA1");
      j.start_obj();
      j.write_fieldname("a1-Threshold");
      c.get<event_a1_s_>().a1_thres.to_json(j);
      j.end_obj();
      break;
    case types::event_a2:
      j.write_fieldname("eventA2");
      j.start_obj();
      j.write_fieldname("a2-Threshold");
      c.get<event_a2_s_>().a2_thres.to_json(j);
      j.end_obj();
      break;
    case types::event_a3:
      j.write_fieldname("eventA3");
      j.start_obj();
      j.write_int("a3-Offset", c.get<event_a3_s_>().a3_offset);
      j.write_bool("reportOnLeave", c.get<event_a3_s_>().report_on_leave);
      j.end_obj();
      break;
    case types::event_a4:
      j.write_fieldname("eventA4");
      j.start_obj();
      j.write_fieldname("a4-Threshold");
      c.get<event_a4_s_>().a4_thres.to_json(j);
      j.end_obj();
      break;
    case types::event_a5:
      j.write_fieldname("eventA5");
      j.start_obj();
      j.write_fieldname("a5-Threshold1");
      c.get<event_a5_s_>().a5_thres1.to_json(j);
      j.write_fieldname("a5-Threshold2");
      c.get<event_a5_s_>().a5_thres2.to_json(j);
      j.end_obj();
      break;
    case types::event_a6_r10:
      j.write_fieldname("eventA6-r10");
      j.start_obj();
      j.write_int("a6-Offset-r10", c.get<event_a6_r10_s_>().a6_offset_r10);
      j.write_bool("a6-ReportOnLeave-r10", c.get<event_a6_r10_s_>().a6_report_on_leave_r10);
      j.end_obj();
      break;
    case types::event_c1_r12:
      j.write_fieldname("eventC1-r12");
      j.start_obj();
      j.write_int("c1-Threshold-r12", c.get<event_c1_r12_s_>().c1_thres_r12);
      j.write_bool("c1-ReportOnLeave-r12", c.get<event_c1_r12_s_>().c1_report_on_leave_r12);
      j.end_obj();
      break;
    case types::event_c2_r12:
      j.write_fieldname("eventC2-r12");
      j.start_obj();
      j.write_int("c2-RefCSI-RS-r12", c.get<event_c2_r12_s_>().c2_ref_csi_rs_r12);
      j.write_int("c2-Offset-r12", c.get<event_c2_r12_s_>().c2_offset_r12);
      j.write_bool("c2-ReportOnLeave-r12", c.get<event_c2_r12_s_>().c2_report_on_leave_r12);
      j.end_obj();
      break;
    case types::event_v1_r14:
      j.write_fieldname("eventV1-r14");
      j.start_obj();
      j.write_int("v1-Threshold-r14", c.get<event_v1_r14_s_>().v1_thres_r14);
      j.end_obj();
      break;
    case types::event_v2_r14:
      j.write_fieldname("eventV2-r14");
      j.start_obj();
      j.write_int("v2-Threshold-r14", c.get<event_v2_r14_s_>().v2_thres_r14);
      j.end_obj();
      break;
    case types::event_h1_r15:
      j.write_fieldname("eventH1-r15");
      j.start_obj();
      j.write_int("h1-ThresholdOffset-r15", c.get<event_h1_r15_s_>().h1_thres_offset_r15);
      j.write_int("h1-Hysteresis-r15", c.get<event_h1_r15_s_>().h1_hysteresis_r15);
      j.end_obj();
      break;
    case types::event_h2_r15:
      j.write_fieldname("eventH2-r15");
      j.start_obj();
      j.write_int("h2-ThresholdOffset-r15", c.get<event_h2_r15_s_>().h2_thres_offset_r15);
      j.write_int("h2-Hysteresis-r15", c.get<event_h2_r15_s_>().h2_hysteresis_r15);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "eutra_event_s::event_id_c_");
  }
  j.end_obj();
}
SRSASN_CODE eutra_event_s::event_id_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::event_a1:
      HANDLE_CODE(c.get<event_a1_s_>().a1_thres.pack(bref));
      break;
    case types::event_a2:
      HANDLE_CODE(c.get<event_a2_s_>().a2_thres.pack(bref));
      break;
    case types::event_a3:
      HANDLE_CODE(pack_integer(bref, c.get<event_a3_s_>().a3_offset, (int8_t)-30, (int8_t)30));
      HANDLE_CODE(bref.pack(c.get<event_a3_s_>().report_on_leave, 1));
      break;
    case types::event_a4:
      HANDLE_CODE(c.get<event_a4_s_>().a4_thres.pack(bref));
      break;
    case types::event_a5:
      HANDLE_CODE(c.get<event_a5_s_>().a5_thres1.pack(bref));
      HANDLE_CODE(c.get<event_a5_s_>().a5_thres2.pack(bref));
      break;
    case types::event_a6_r10: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<event_a6_r10_s_>().a6_offset_r10, (int8_t)-30, (int8_t)30));
      HANDLE_CODE(bref.pack(c.get<event_a6_r10_s_>().a6_report_on_leave_r10, 1));
    } break;
    case types::event_c1_r12: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<event_c1_r12_s_>().c1_thres_r12, (uint8_t)0u, (uint8_t)97u));
      HANDLE_CODE(bref.pack(c.get<event_c1_r12_s_>().c1_report_on_leave_r12, 1));
    } break;
    case types::event_c2_r12: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<event_c2_r12_s_>().c2_ref_csi_rs_r12, (uint8_t)1u, (uint8_t)96u));
      HANDLE_CODE(pack_integer(bref, c.get<event_c2_r12_s_>().c2_offset_r12, (int8_t)-30, (int8_t)30));
      HANDLE_CODE(bref.pack(c.get<event_c2_r12_s_>().c2_report_on_leave_r12, 1));
    } break;
    case types::event_v1_r14: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<event_v1_r14_s_>().v1_thres_r14, (uint8_t)0u, (uint8_t)100u));
    } break;
    case types::event_v2_r14: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<event_v2_r14_s_>().v2_thres_r14, (uint8_t)0u, (uint8_t)100u));
    } break;
    case types::event_h1_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<event_h1_r15_s_>().h1_thres_offset_r15, (uint16_t)0u, (uint16_t)300u));
      HANDLE_CODE(pack_integer(bref, c.get<event_h1_r15_s_>().h1_hysteresis_r15, (uint8_t)1u, (uint8_t)16u));
    } break;
    case types::event_h2_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<event_h2_r15_s_>().h2_thres_offset_r15, (uint16_t)0u, (uint16_t)300u));
      HANDLE_CODE(pack_integer(bref, c.get<event_h2_r15_s_>().h2_hysteresis_r15, (uint8_t)1u, (uint8_t)16u));
    } break;
    default:
      log_invalid_choice_id(type_, "eutra_event_s::event_id_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_event_s::event_id_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::event_a1:
      HANDLE_CODE(c.get<event_a1_s_>().a1_thres.unpack(bref));
      break;
    case types::event_a2:
      HANDLE_CODE(c.get<event_a2_s_>().a2_thres.unpack(bref));
      break;
    case types::event_a3:
      HANDLE_CODE(unpack_integer(c.get<event_a3_s_>().a3_offset, bref, (int8_t)-30, (int8_t)30));
      HANDLE_CODE(bref.unpack(c.get<event_a3_s_>().report_on_leave, 1));
      break;
    case types::event_a4:
      HANDLE_CODE(c.get<event_a4_s_>().a4_thres.unpack(bref));
      break;
    case types::event_a5:
      HANDLE_CODE(c.get<event_a5_s_>().a5_thres1.unpack(bref));
      HANDLE_CODE(c.get<event_a5_s_>().a5_thres2.unpack(bref));
      break;
    case types::event_a6_r10: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<event_a6_r10_s_>().a6_offset_r10, bref, (int8_t)-30, (int8_t)30));
      HANDLE_CODE(bref.unpack(c.get<event_a6_r10_s_>().a6_report_on_leave_r10, 1));
    } break;
    case types::event_c1_r12: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<event_c1_r12_s_>().c1_thres_r12, bref, (uint8_t)0u, (uint8_t)97u));
      HANDLE_CODE(bref.unpack(c.get<event_c1_r12_s_>().c1_report_on_leave_r12, 1));
    } break;
    case types::event_c2_r12: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<event_c2_r12_s_>().c2_ref_csi_rs_r12, bref, (uint8_t)1u, (uint8_t)96u));
      HANDLE_CODE(unpack_integer(c.get<event_c2_r12_s_>().c2_offset_r12, bref, (int8_t)-30, (int8_t)30));
      HANDLE_CODE(bref.unpack(c.get<event_c2_r12_s_>().c2_report_on_leave_r12, 1));
    } break;
    case types::event_v1_r14: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<event_v1_r14_s_>().v1_thres_r14, bref, (uint8_t)0u, (uint8_t)100u));
    } break;
    case types::event_v2_r14: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<event_v2_r14_s_>().v2_thres_r14, bref, (uint8_t)0u, (uint8_t)100u));
    } break;
    case types::event_h1_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<event_h1_r15_s_>().h1_thres_offset_r15, bref, (uint16_t)0u, (uint16_t)300u));
      HANDLE_CODE(unpack_integer(c.get<event_h1_r15_s_>().h1_hysteresis_r15, bref, (uint8_t)1u, (uint8_t)16u));
    } break;
    case types::event_h2_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<event_h2_r15_s_>().h2_thres_offset_r15, bref, (uint16_t)0u, (uint16_t)300u));
      HANDLE_CODE(unpack_integer(c.get<event_h2_r15_s_>().h2_hysteresis_r15, bref, (uint8_t)1u, (uint8_t)16u));
    } break;
    default:
      log_invalid_choice_id(type_, "eutra_event_s::event_id_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool eutra_event_s::event_id_c_::operator==(const event_id_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::event_a1:
      return c.get<event_a1_s_>().a1_thres == other.c.get<event_a1_s_>().a1_thres;
    case types::event_a2:
      return c.get<event_a2_s_>().a2_thres == other.c.get<event_a2_s_>().a2_thres;
    case types::event_a3:
      return c.get<event_a3_s_>().a3_offset == other.c.get<event_a3_s_>().a3_offset and
             c.get<event_a3_s_>().report_on_leave == other.c.get<event_a3_s_>().report_on_leave;
    case types::event_a4:
      return c.get<event_a4_s_>().a4_thres == other.c.get<event_a4_s_>().a4_thres;
    case types::event_a5:
      return c.get<event_a5_s_>().a5_thres1 == other.c.get<event_a5_s_>().a5_thres1 and
             c.get<event_a5_s_>().a5_thres2 == other.c.get<event_a5_s_>().a5_thres2;
    case types::event_a6_r10:
      return c.get<event_a6_r10_s_>().a6_offset_r10 == other.c.get<event_a6_r10_s_>().a6_offset_r10 and
             c.get<event_a6_r10_s_>().a6_report_on_leave_r10 == other.c.get<event_a6_r10_s_>().a6_report_on_leave_r10;
    case types::event_c1_r12:
      return c.get<event_c1_r12_s_>().c1_thres_r12 == other.c.get<event_c1_r12_s_>().c1_thres_r12 and
             c.get<event_c1_r12_s_>().c1_report_on_leave_r12 == other.c.get<event_c1_r12_s_>().c1_report_on_leave_r12;
    case types::event_c2_r12:
      return c.get<event_c2_r12_s_>().c2_ref_csi_rs_r12 == other.c.get<event_c2_r12_s_>().c2_ref_csi_rs_r12 and
             c.get<event_c2_r12_s_>().c2_offset_r12 == other.c.get<event_c2_r12_s_>().c2_offset_r12 and
             c.get<event_c2_r12_s_>().c2_report_on_leave_r12 == other.c.get<event_c2_r12_s_>().c2_report_on_leave_r12;
    case types::event_v1_r14:
      return c.get<event_v1_r14_s_>().v1_thres_r14 == other.c.get<event_v1_r14_s_>().v1_thres_r14;
    case types::event_v2_r14:
      return c.get<event_v2_r14_s_>().v2_thres_r14 == other.c.get<event_v2_r14_s_>().v2_thres_r14;
    case types::event_h1_r15:
      return c.get<event_h1_r15_s_>().h1_thres_offset_r15 == other.c.get<event_h1_r15_s_>().h1_thres_offset_r15 and
             c.get<event_h1_r15_s_>().h1_hysteresis_r15 == other.c.get<event_h1_r15_s_>().h1_hysteresis_r15;
    case types::event_h2_r15:
      return c.get<event_h2_r15_s_>().h2_thres_offset_r15 == other.c.get<event_h2_r15_s_>().h2_thres_offset_r15 and
             c.get<event_h2_r15_s_>().h2_hysteresis_r15 == other.c.get<event_h2_r15_s_>().h2_hysteresis_r15;
    default:
      return true;
  }
  return true;
}

// ReportConfigEUTRA ::= SEQUENCE
SRSASN_CODE report_cfg_eutra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(trigger_type.pack(bref));
  HANDLE_CODE(trigger_quant.pack(bref));
  HANDLE_CODE(report_quant.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_report_cells, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(report_interv.pack(bref));
  HANDLE_CODE(report_amount.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= si_request_for_ho_r9_present;
    group_flags[0] |= ue_rx_tx_time_diff_periodical_r9_present;
    group_flags[1] |= include_location_info_r10_present;
    group_flags[1] |= report_add_neigh_meas_r10_present;
    group_flags[2] |= alt_time_to_trigger_r12.is_present();
    group_flags[2] |= use_t312_r12_present;
    group_flags[2] |= use_ps_cell_r12_present;
    group_flags[2] |= an_thres1_v1250.is_present();
    group_flags[2] |= a5_thres2_v1250.is_present();
    group_flags[2] |= report_strongest_csi_rss_r12_present;
    group_flags[2] |= report_crs_meas_r12_present;
    group_flags[2] |= trigger_quant_csi_rs_r12_present;
    group_flags[3] |= report_sstd_meas_r13_present;
    group_flags[3] |= rs_sinr_cfg_r13.is_present();
    group_flags[3] |= use_white_cell_list_r13_present;
    group_flags[3] |= meas_rssi_report_cfg_r13.is_present();
    group_flags[3] |= include_multi_band_info_r13_present;
    group_flags[3] |= ul_delay_cfg_r13.is_present();
    group_flags[4] |= ue_rx_tx_time_diff_periodical_tdd_r13_present;
    group_flags[5] |= purpose_v1430_present;
    group_flags[6] |= max_report_rs_idx_r15_present;
    group_flags[7] |= include_bt_meas_r15.is_present();
    group_flags[7] |= include_wlan_meas_r15.is_present();
    group_flags[7] |= purpose_r15_present;
    group_flags[7] |= nof_trigger_cells_r15_present;
    group_flags[7] |= a4_a5_report_on_leave_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(si_request_for_ho_r9_present, 1));
      HANDLE_CODE(bref.pack(ue_rx_tx_time_diff_periodical_r9_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(include_location_info_r10_present, 1));
      HANDLE_CODE(bref.pack(report_add_neigh_meas_r10_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(alt_time_to_trigger_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(use_t312_r12_present, 1));
      HANDLE_CODE(bref.pack(use_ps_cell_r12_present, 1));
      HANDLE_CODE(bref.pack(an_thres1_v1250.is_present(), 1));
      HANDLE_CODE(bref.pack(a5_thres2_v1250.is_present(), 1));
      HANDLE_CODE(bref.pack(report_strongest_csi_rss_r12_present, 1));
      HANDLE_CODE(bref.pack(report_crs_meas_r12_present, 1));
      HANDLE_CODE(bref.pack(trigger_quant_csi_rs_r12_present, 1));
      if (alt_time_to_trigger_r12.is_present()) {
        HANDLE_CODE(alt_time_to_trigger_r12->pack(bref));
      }
      if (use_t312_r12_present) {
        HANDLE_CODE(bref.pack(use_t312_r12, 1));
      }
      if (use_ps_cell_r12_present) {
        HANDLE_CODE(bref.pack(use_ps_cell_r12, 1));
      }
      if (an_thres1_v1250.is_present()) {
        HANDLE_CODE(an_thres1_v1250->pack(bref));
      }
      if (a5_thres2_v1250.is_present()) {
        HANDLE_CODE(a5_thres2_v1250->pack(bref));
      }
      if (report_strongest_csi_rss_r12_present) {
        HANDLE_CODE(bref.pack(report_strongest_csi_rss_r12, 1));
      }
      if (report_crs_meas_r12_present) {
        HANDLE_CODE(bref.pack(report_crs_meas_r12, 1));
      }
      if (trigger_quant_csi_rs_r12_present) {
        HANDLE_CODE(bref.pack(trigger_quant_csi_rs_r12, 1));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_sstd_meas_r13_present, 1));
      HANDLE_CODE(bref.pack(rs_sinr_cfg_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(use_white_cell_list_r13_present, 1));
      HANDLE_CODE(bref.pack(meas_rssi_report_cfg_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(include_multi_band_info_r13_present, 1));
      HANDLE_CODE(bref.pack(ul_delay_cfg_r13.is_present(), 1));
      if (report_sstd_meas_r13_present) {
        HANDLE_CODE(bref.pack(report_sstd_meas_r13, 1));
      }
      if (rs_sinr_cfg_r13.is_present()) {
        HANDLE_CODE(rs_sinr_cfg_r13->pack(bref));
      }
      if (use_white_cell_list_r13_present) {
        HANDLE_CODE(bref.pack(use_white_cell_list_r13, 1));
      }
      if (meas_rssi_report_cfg_r13.is_present()) {
        HANDLE_CODE(meas_rssi_report_cfg_r13->pack(bref));
      }
      if (ul_delay_cfg_r13.is_present()) {
        HANDLE_CODE(ul_delay_cfg_r13->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ue_rx_tx_time_diff_periodical_tdd_r13_present, 1));
      if (ue_rx_tx_time_diff_periodical_tdd_r13_present) {
        HANDLE_CODE(bref.pack(ue_rx_tx_time_diff_periodical_tdd_r13, 1));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(purpose_v1430_present, 1));
      if (purpose_v1430_present) {
        HANDLE_CODE(purpose_v1430.pack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(max_report_rs_idx_r15_present, 1));
      if (max_report_rs_idx_r15_present) {
        HANDLE_CODE(pack_integer(bref, max_report_rs_idx_r15, (uint8_t)0u, (uint8_t)32u));
      }
    }
    if (group_flags[7]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(include_bt_meas_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(include_wlan_meas_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(purpose_r15_present, 1));
      HANDLE_CODE(bref.pack(nof_trigger_cells_r15_present, 1));
      HANDLE_CODE(bref.pack(a4_a5_report_on_leave_r15_present, 1));
      if (include_bt_meas_r15.is_present()) {
        HANDLE_CODE(include_bt_meas_r15->pack(bref));
      }
      if (include_wlan_meas_r15.is_present()) {
        HANDLE_CODE(include_wlan_meas_r15->pack(bref));
      }
      if (nof_trigger_cells_r15_present) {
        HANDLE_CODE(pack_integer(bref, nof_trigger_cells_r15, (uint8_t)2u, (uint8_t)8u));
      }
      if (a4_a5_report_on_leave_r15_present) {
        HANDLE_CODE(bref.pack(a4_a5_report_on_leave_r15, 1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_eutra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(trigger_type.unpack(bref));
  HANDLE_CODE(trigger_quant.unpack(bref));
  HANDLE_CODE(report_quant.unpack(bref));
  HANDLE_CODE(unpack_integer(max_report_cells, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(report_interv.unpack(bref));
  HANDLE_CODE(report_amount.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(8);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(si_request_for_ho_r9_present, 1));
      HANDLE_CODE(bref.unpack(ue_rx_tx_time_diff_periodical_r9_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(include_location_info_r10_present, 1));
      HANDLE_CODE(bref.unpack(report_add_neigh_meas_r10_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool alt_time_to_trigger_r12_present;
      HANDLE_CODE(bref.unpack(alt_time_to_trigger_r12_present, 1));
      alt_time_to_trigger_r12.set_present(alt_time_to_trigger_r12_present);
      HANDLE_CODE(bref.unpack(use_t312_r12_present, 1));
      HANDLE_CODE(bref.unpack(use_ps_cell_r12_present, 1));
      bool an_thres1_v1250_present;
      HANDLE_CODE(bref.unpack(an_thres1_v1250_present, 1));
      an_thres1_v1250.set_present(an_thres1_v1250_present);
      bool a5_thres2_v1250_present;
      HANDLE_CODE(bref.unpack(a5_thres2_v1250_present, 1));
      a5_thres2_v1250.set_present(a5_thres2_v1250_present);
      HANDLE_CODE(bref.unpack(report_strongest_csi_rss_r12_present, 1));
      HANDLE_CODE(bref.unpack(report_crs_meas_r12_present, 1));
      HANDLE_CODE(bref.unpack(trigger_quant_csi_rs_r12_present, 1));
      if (alt_time_to_trigger_r12.is_present()) {
        HANDLE_CODE(alt_time_to_trigger_r12->unpack(bref));
      }
      if (use_t312_r12_present) {
        HANDLE_CODE(bref.unpack(use_t312_r12, 1));
      }
      if (use_ps_cell_r12_present) {
        HANDLE_CODE(bref.unpack(use_ps_cell_r12, 1));
      }
      if (an_thres1_v1250.is_present()) {
        HANDLE_CODE(an_thres1_v1250->unpack(bref));
      }
      if (a5_thres2_v1250.is_present()) {
        HANDLE_CODE(a5_thres2_v1250->unpack(bref));
      }
      if (report_strongest_csi_rss_r12_present) {
        HANDLE_CODE(bref.unpack(report_strongest_csi_rss_r12, 1));
      }
      if (report_crs_meas_r12_present) {
        HANDLE_CODE(bref.unpack(report_crs_meas_r12, 1));
      }
      if (trigger_quant_csi_rs_r12_present) {
        HANDLE_CODE(bref.unpack(trigger_quant_csi_rs_r12, 1));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(report_sstd_meas_r13_present, 1));
      bool rs_sinr_cfg_r13_present;
      HANDLE_CODE(bref.unpack(rs_sinr_cfg_r13_present, 1));
      rs_sinr_cfg_r13.set_present(rs_sinr_cfg_r13_present);
      HANDLE_CODE(bref.unpack(use_white_cell_list_r13_present, 1));
      bool meas_rssi_report_cfg_r13_present;
      HANDLE_CODE(bref.unpack(meas_rssi_report_cfg_r13_present, 1));
      meas_rssi_report_cfg_r13.set_present(meas_rssi_report_cfg_r13_present);
      HANDLE_CODE(bref.unpack(include_multi_band_info_r13_present, 1));
      bool ul_delay_cfg_r13_present;
      HANDLE_CODE(bref.unpack(ul_delay_cfg_r13_present, 1));
      ul_delay_cfg_r13.set_present(ul_delay_cfg_r13_present);
      if (report_sstd_meas_r13_present) {
        HANDLE_CODE(bref.unpack(report_sstd_meas_r13, 1));
      }
      if (rs_sinr_cfg_r13.is_present()) {
        HANDLE_CODE(rs_sinr_cfg_r13->unpack(bref));
      }
      if (use_white_cell_list_r13_present) {
        HANDLE_CODE(bref.unpack(use_white_cell_list_r13, 1));
      }
      if (meas_rssi_report_cfg_r13.is_present()) {
        HANDLE_CODE(meas_rssi_report_cfg_r13->unpack(bref));
      }
      if (ul_delay_cfg_r13.is_present()) {
        HANDLE_CODE(ul_delay_cfg_r13->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ue_rx_tx_time_diff_periodical_tdd_r13_present, 1));
      if (ue_rx_tx_time_diff_periodical_tdd_r13_present) {
        HANDLE_CODE(bref.unpack(ue_rx_tx_time_diff_periodical_tdd_r13, 1));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(purpose_v1430_present, 1));
      if (purpose_v1430_present) {
        HANDLE_CODE(purpose_v1430.unpack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(max_report_rs_idx_r15_present, 1));
      if (max_report_rs_idx_r15_present) {
        HANDLE_CODE(unpack_integer(max_report_rs_idx_r15, bref, (uint8_t)0u, (uint8_t)32u));
      }
    }
    if (group_flags[7]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool include_bt_meas_r15_present;
      HANDLE_CODE(bref.unpack(include_bt_meas_r15_present, 1));
      include_bt_meas_r15.set_present(include_bt_meas_r15_present);
      bool include_wlan_meas_r15_present;
      HANDLE_CODE(bref.unpack(include_wlan_meas_r15_present, 1));
      include_wlan_meas_r15.set_present(include_wlan_meas_r15_present);
      HANDLE_CODE(bref.unpack(purpose_r15_present, 1));
      HANDLE_CODE(bref.unpack(nof_trigger_cells_r15_present, 1));
      HANDLE_CODE(bref.unpack(a4_a5_report_on_leave_r15_present, 1));
      if (include_bt_meas_r15.is_present()) {
        HANDLE_CODE(include_bt_meas_r15->unpack(bref));
      }
      if (include_wlan_meas_r15.is_present()) {
        HANDLE_CODE(include_wlan_meas_r15->unpack(bref));
      }
      if (nof_trigger_cells_r15_present) {
        HANDLE_CODE(unpack_integer(nof_trigger_cells_r15, bref, (uint8_t)2u, (uint8_t)8u));
      }
      if (a4_a5_report_on_leave_r15_present) {
        HANDLE_CODE(bref.unpack(a4_a5_report_on_leave_r15, 1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void report_cfg_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("triggerType");
  trigger_type.to_json(j);
  j.write_str("triggerQuantity", trigger_quant.to_string());
  j.write_str("reportQuantity", report_quant.to_string());
  j.write_int("maxReportCells", max_report_cells);
  j.write_str("reportInterval", report_interv.to_string());
  j.write_str("reportAmount", report_amount.to_string());
  if (ext) {
    if (si_request_for_ho_r9_present) {
      j.write_str("si-RequestForHO-r9", "setup");
    }
    if (ue_rx_tx_time_diff_periodical_r9_present) {
      j.write_str("ue-RxTxTimeDiffPeriodical-r9", "setup");
    }
    if (include_location_info_r10_present) {
      j.write_str("includeLocationInfo-r10", "true");
    }
    if (report_add_neigh_meas_r10_present) {
      j.write_str("reportAddNeighMeas-r10", "setup");
    }
    if (alt_time_to_trigger_r12.is_present()) {
      j.write_fieldname("alternativeTimeToTrigger-r12");
      alt_time_to_trigger_r12->to_json(j);
    }
    if (use_t312_r12_present) {
      j.write_bool("useT312-r12", use_t312_r12);
    }
    if (use_ps_cell_r12_present) {
      j.write_bool("usePSCell-r12", use_ps_cell_r12);
    }
    if (an_thres1_v1250.is_present()) {
      j.write_fieldname("aN-Threshold1-v1250");
      an_thres1_v1250->to_json(j);
    }
    if (a5_thres2_v1250.is_present()) {
      j.write_fieldname("a5-Threshold2-v1250");
      a5_thres2_v1250->to_json(j);
    }
    if (report_strongest_csi_rss_r12_present) {
      j.write_bool("reportStrongestCSI-RSs-r12", report_strongest_csi_rss_r12);
    }
    if (report_crs_meas_r12_present) {
      j.write_bool("reportCRS-Meas-r12", report_crs_meas_r12);
    }
    if (trigger_quant_csi_rs_r12_present) {
      j.write_bool("triggerQuantityCSI-RS-r12", trigger_quant_csi_rs_r12);
    }
    if (report_sstd_meas_r13_present) {
      j.write_bool("reportSSTD-Meas-r13", report_sstd_meas_r13);
    }
    if (rs_sinr_cfg_r13.is_present()) {
      j.write_fieldname("rs-sinr-Config-r13");
      rs_sinr_cfg_r13->to_json(j);
    }
    if (use_white_cell_list_r13_present) {
      j.write_bool("useWhiteCellList-r13", use_white_cell_list_r13);
    }
    if (meas_rssi_report_cfg_r13.is_present()) {
      j.write_fieldname("measRSSI-ReportConfig-r13");
      meas_rssi_report_cfg_r13->to_json(j);
    }
    if (include_multi_band_info_r13_present) {
      j.write_str("includeMultiBandInfo-r13", "true");
    }
    if (ul_delay_cfg_r13.is_present()) {
      j.write_fieldname("ul-DelayConfig-r13");
      ul_delay_cfg_r13->to_json(j);
    }
    if (ue_rx_tx_time_diff_periodical_tdd_r13_present) {
      j.write_bool("ue-RxTxTimeDiffPeriodicalTDD-r13", ue_rx_tx_time_diff_periodical_tdd_r13);
    }
    if (purpose_v1430_present) {
      j.write_str("purpose-v1430", purpose_v1430.to_string());
    }
    if (max_report_rs_idx_r15_present) {
      j.write_int("maxReportRS-Index-r15", max_report_rs_idx_r15);
    }
    if (include_bt_meas_r15.is_present()) {
      j.write_fieldname("includeBT-Meas-r15");
      include_bt_meas_r15->to_json(j);
    }
    if (include_wlan_meas_r15.is_present()) {
      j.write_fieldname("includeWLAN-Meas-r15");
      include_wlan_meas_r15->to_json(j);
    }
    if (purpose_r15_present) {
      j.write_str("purpose-r15", "sensing");
    }
    if (nof_trigger_cells_r15_present) {
      j.write_int("numberOfTriggeringCells-r15", nof_trigger_cells_r15);
    }
    if (a4_a5_report_on_leave_r15_present) {
      j.write_bool("a4-a5-ReportOnLeave-r15", a4_a5_report_on_leave_r15);
    }
  }
  j.end_obj();
}
bool report_cfg_eutra_s::operator==(const report_cfg_eutra_s& other) const
{
  return ext == other.ext and trigger_type == other.trigger_type and trigger_quant == other.trigger_quant and
         report_quant == other.report_quant and max_report_cells == other.max_report_cells and
         report_interv == other.report_interv and report_amount == other.report_amount and
         (not ext or
          (si_request_for_ho_r9_present == other.si_request_for_ho_r9_present and
           ue_rx_tx_time_diff_periodical_r9_present == other.ue_rx_tx_time_diff_periodical_r9_present and
           include_location_info_r10_present == other.include_location_info_r10_present and
           report_add_neigh_meas_r10_present == other.report_add_neigh_meas_r10_present and
           alt_time_to_trigger_r12.is_present() == other.alt_time_to_trigger_r12.is_present() and
           (not alt_time_to_trigger_r12.is_present() or *alt_time_to_trigger_r12 == *other.alt_time_to_trigger_r12) and
           use_t312_r12_present == other.use_t312_r12_present and
           (not use_t312_r12_present or use_t312_r12 == other.use_t312_r12) and
           use_ps_cell_r12_present == other.use_ps_cell_r12_present and
           (not use_ps_cell_r12_present or use_ps_cell_r12 == other.use_ps_cell_r12) and
           an_thres1_v1250.is_present() == other.an_thres1_v1250.is_present() and
           (not an_thres1_v1250.is_present() or *an_thres1_v1250 == *other.an_thres1_v1250) and
           a5_thres2_v1250.is_present() == other.a5_thres2_v1250.is_present() and
           (not a5_thres2_v1250.is_present() or *a5_thres2_v1250 == *other.a5_thres2_v1250) and
           report_strongest_csi_rss_r12_present == other.report_strongest_csi_rss_r12_present and
           (not report_strongest_csi_rss_r12_present or
            report_strongest_csi_rss_r12 == other.report_strongest_csi_rss_r12) and
           report_crs_meas_r12_present == other.report_crs_meas_r12_present and
           (not report_crs_meas_r12_present or report_crs_meas_r12 == other.report_crs_meas_r12) and
           trigger_quant_csi_rs_r12_present == other.trigger_quant_csi_rs_r12_present and
           (not trigger_quant_csi_rs_r12_present or trigger_quant_csi_rs_r12 == other.trigger_quant_csi_rs_r12) and
           report_sstd_meas_r13_present == other.report_sstd_meas_r13_present and
           (not report_sstd_meas_r13_present or report_sstd_meas_r13 == other.report_sstd_meas_r13) and
           rs_sinr_cfg_r13.is_present() == other.rs_sinr_cfg_r13.is_present() and
           (not rs_sinr_cfg_r13.is_present() or *rs_sinr_cfg_r13 == *other.rs_sinr_cfg_r13) and
           use_white_cell_list_r13_present == other.use_white_cell_list_r13_present and
           (not use_white_cell_list_r13_present or use_white_cell_list_r13 == other.use_white_cell_list_r13) and
           meas_rssi_report_cfg_r13.is_present() == other.meas_rssi_report_cfg_r13.is_present() and
           (not meas_rssi_report_cfg_r13.is_present() or
            *meas_rssi_report_cfg_r13 == *other.meas_rssi_report_cfg_r13) and
           include_multi_band_info_r13_present == other.include_multi_band_info_r13_present and
           ul_delay_cfg_r13.is_present() == other.ul_delay_cfg_r13.is_present() and
           (not ul_delay_cfg_r13.is_present() or *ul_delay_cfg_r13 == *other.ul_delay_cfg_r13) and
           ue_rx_tx_time_diff_periodical_tdd_r13_present == other.ue_rx_tx_time_diff_periodical_tdd_r13_present and
           (not ue_rx_tx_time_diff_periodical_tdd_r13_present or
            ue_rx_tx_time_diff_periodical_tdd_r13 == other.ue_rx_tx_time_diff_periodical_tdd_r13) and
           purpose_v1430_present == other.purpose_v1430_present and
           (not purpose_v1430_present or purpose_v1430 == other.purpose_v1430) and
           max_report_rs_idx_r15_present == other.max_report_rs_idx_r15_present and
           (not max_report_rs_idx_r15_present or max_report_rs_idx_r15 == other.max_report_rs_idx_r15) and
           include_bt_meas_r15.is_present() == other.include_bt_meas_r15.is_present() and
           (not include_bt_meas_r15.is_present() or *include_bt_meas_r15 == *other.include_bt_meas_r15) and
           include_wlan_meas_r15.is_present() == other.include_wlan_meas_r15.is_present() and
           (not include_wlan_meas_r15.is_present() or *include_wlan_meas_r15 == *other.include_wlan_meas_r15) and
           purpose_r15_present == other.purpose_r15_present and
           nof_trigger_cells_r15_present == other.nof_trigger_cells_r15_present and
           (not nof_trigger_cells_r15_present or nof_trigger_cells_r15 == other.nof_trigger_cells_r15) and
           a4_a5_report_on_leave_r15_present == other.a4_a5_report_on_leave_r15_present and
           (not a4_a5_report_on_leave_r15_present or a4_a5_report_on_leave_r15 == other.a4_a5_report_on_leave_r15)));
}

void report_cfg_eutra_s::trigger_type_c_::destroy_()
{
  switch (type_) {
    case types::event:
      c.destroy<event_s_>();
      break;
    case types::periodical:
      c.destroy<periodical_s_>();
      break;
    default:
      break;
  }
}
void report_cfg_eutra_s::trigger_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::event:
      c.init<event_s_>();
      break;
    case types::periodical:
      c.init<periodical_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_eutra_s::trigger_type_c_");
  }
}
report_cfg_eutra_s::trigger_type_c_::trigger_type_c_(const report_cfg_eutra_s::trigger_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::event:
      c.init(other.c.get<event_s_>());
      break;
    case types::periodical:
      c.init(other.c.get<periodical_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_eutra_s::trigger_type_c_");
  }
}
report_cfg_eutra_s::trigger_type_c_&
report_cfg_eutra_s::trigger_type_c_::operator=(const report_cfg_eutra_s::trigger_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::event:
      c.set(other.c.get<event_s_>());
      break;
    case types::periodical:
      c.set(other.c.get<periodical_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_eutra_s::trigger_type_c_");
  }

  return *this;
}
report_cfg_eutra_s::trigger_type_c_::event_s_& report_cfg_eutra_s::trigger_type_c_::set_event()
{
  set(types::event);
  return c.get<event_s_>();
}
report_cfg_eutra_s::trigger_type_c_::periodical_s_& report_cfg_eutra_s::trigger_type_c_::set_periodical()
{
  set(types::periodical);
  return c.get<periodical_s_>();
}
void report_cfg_eutra_s::trigger_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::event:
      j.write_fieldname("event");
      c.get<event_s_>().to_json(j);
      break;
    case types::periodical:
      j.write_fieldname("periodical");
      j.start_obj();
      j.write_str("purpose", c.get<periodical_s_>().purpose.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_eutra_s::trigger_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE report_cfg_eutra_s::trigger_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::event:
      HANDLE_CODE(c.get<event_s_>().pack(bref));
      break;
    case types::periodical:
      HANDLE_CODE(c.get<periodical_s_>().purpose.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_eutra_s::trigger_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_eutra_s::trigger_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::event:
      HANDLE_CODE(c.get<event_s_>().unpack(bref));
      break;
    case types::periodical:
      HANDLE_CODE(c.get<periodical_s_>().purpose.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_eutra_s::trigger_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool report_cfg_eutra_s::trigger_type_c_::operator==(const trigger_type_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::event:
      return c.get<event_s_>() == other.c.get<event_s_>();
    case types::periodical:
      return c.get<periodical_s_>().purpose == other.c.get<periodical_s_>().purpose;
    default:
      return true;
  }
  return true;
}

const char* report_cfg_eutra_s::trigger_type_c_::periodical_s_::purpose_opts::to_string() const
{
  static const char* options[] = {"reportStrongestCells", "reportCGI"};
  return convert_enum_idx(options, 2, value, "report_cfg_eutra_s::trigger_type_c_::periodical_s_::purpose_e_");
}

const char* report_cfg_eutra_s::trigger_quant_opts::to_string() const
{
  static const char* options[] = {"rsrp", "rsrq"};
  return convert_enum_idx(options, 2, value, "report_cfg_eutra_s::trigger_quant_e_");
}

const char* report_cfg_eutra_s::report_quant_opts::to_string() const
{
  static const char* options[] = {"sameAsTriggerQuantity", "both"};
  return convert_enum_idx(options, 2, value, "report_cfg_eutra_s::report_quant_e_");
}

const char* report_cfg_eutra_s::report_amount_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(options, 8, value, "report_cfg_eutra_s::report_amount_e_");
}
int8_t report_cfg_eutra_s::report_amount_opts::to_number() const
{
  static const int8_t options[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(options, 8, value, "report_cfg_eutra_s::report_amount_e_");
}

void report_cfg_eutra_s::alt_time_to_trigger_r12_c_::set(types::options e)
{
  type_ = e;
}
void report_cfg_eutra_s::alt_time_to_trigger_r12_c_::set_release()
{
  set(types::release);
}
time_to_trigger_e& report_cfg_eutra_s::alt_time_to_trigger_r12_c_::set_setup()
{
  set(types::setup);
  return c;
}
void report_cfg_eutra_s::alt_time_to_trigger_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_str("setup", c.to_string());
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_eutra_s::alt_time_to_trigger_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE report_cfg_eutra_s::alt_time_to_trigger_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_eutra_s::alt_time_to_trigger_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_eutra_s::alt_time_to_trigger_r12_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "report_cfg_eutra_s::alt_time_to_trigger_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool report_cfg_eutra_s::alt_time_to_trigger_r12_c_::operator==(const alt_time_to_trigger_r12_c_& other) const
{
  return type() == other.type() and c == other.c;
}

void report_cfg_eutra_s::rs_sinr_cfg_r13_c_::set(types::options e)
{
  type_ = e;
}
void report_cfg_eutra_s::rs_sinr_cfg_r13_c_::set_release()
{
  set(types::release);
}
report_cfg_eutra_s::rs_sinr_cfg_r13_c_::setup_s_& report_cfg_eutra_s::rs_sinr_cfg_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void report_cfg_eutra_s::rs_sinr_cfg_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.trigger_quant_v1310_present) {
        j.write_str("triggerQuantity-v1310", "sinr");
      }
      if (c.an_thres1_r13_present) {
        j.write_int("aN-Threshold1-r13", c.an_thres1_r13);
      }
      if (c.a5_thres2_r13_present) {
        j.write_int("a5-Threshold2-r13", c.a5_thres2_r13);
      }
      j.write_str("reportQuantity-v1310", c.report_quant_v1310.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_eutra_s::rs_sinr_cfg_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE report_cfg_eutra_s::rs_sinr_cfg_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.trigger_quant_v1310_present, 1));
      HANDLE_CODE(bref.pack(c.an_thres1_r13_present, 1));
      HANDLE_CODE(bref.pack(c.a5_thres2_r13_present, 1));
      if (c.an_thres1_r13_present) {
        HANDLE_CODE(pack_integer(bref, c.an_thres1_r13, (uint8_t)0u, (uint8_t)127u));
      }
      if (c.a5_thres2_r13_present) {
        HANDLE_CODE(pack_integer(bref, c.a5_thres2_r13, (uint8_t)0u, (uint8_t)127u));
      }
      HANDLE_CODE(c.report_quant_v1310.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_eutra_s::rs_sinr_cfg_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_eutra_s::rs_sinr_cfg_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.trigger_quant_v1310_present, 1));
      HANDLE_CODE(bref.unpack(c.an_thres1_r13_present, 1));
      HANDLE_CODE(bref.unpack(c.a5_thres2_r13_present, 1));
      if (c.an_thres1_r13_present) {
        HANDLE_CODE(unpack_integer(c.an_thres1_r13, bref, (uint8_t)0u, (uint8_t)127u));
      }
      if (c.a5_thres2_r13_present) {
        HANDLE_CODE(unpack_integer(c.a5_thres2_r13, bref, (uint8_t)0u, (uint8_t)127u));
      }
      HANDLE_CODE(c.report_quant_v1310.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_eutra_s::rs_sinr_cfg_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool report_cfg_eutra_s::rs_sinr_cfg_r13_c_::operator==(const rs_sinr_cfg_r13_c_& other) const
{
  return type() == other.type() and c.trigger_quant_v1310_present == other.c.trigger_quant_v1310_present and
         c.an_thres1_r13_present == other.c.an_thres1_r13_present and
         (not c.an_thres1_r13_present or c.an_thres1_r13 == other.c.an_thres1_r13) and
         c.a5_thres2_r13_present == other.c.a5_thres2_r13_present and
         (not c.a5_thres2_r13_present or c.a5_thres2_r13 == other.c.a5_thres2_r13) and
         c.report_quant_v1310 == other.c.report_quant_v1310;
}

const char* report_cfg_eutra_s::rs_sinr_cfg_r13_c_::setup_s_::report_quant_v1310_opts::to_string() const
{
  static const char* options[] = {"rsrpANDsinr", "rsrqANDsinr", "all"};
  return convert_enum_idx(options, 3, value, "report_cfg_eutra_s::rs_sinr_cfg_r13_c_::setup_s_::report_quant_v1310_e_");
}

const char* report_cfg_eutra_s::purpose_v1430_opts::to_string() const
{
  static const char* options[] = {"reportLocation", "sidelink", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "report_cfg_eutra_s::purpose_v1430_e_");
}

// ReportConfigInterRAT ::= SEQUENCE
SRSASN_CODE report_cfg_inter_rat_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(trigger_type.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_report_cells, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(report_interv.pack(bref));
  HANDLE_CODE(report_amount.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= si_request_for_ho_r9_present;
    group_flags[1] |= report_quant_utra_fdd_r10_present;
    group_flags[2] |= include_location_info_r11_present;
    group_flags[3] |= b2_thres1_v1250.is_present();
    group_flags[4] |= report_quant_wlan_r13.is_present();
    group_flags[5] |= report_any_wlan_r14_present;
    group_flags[6] |= report_quant_cell_nr_r15.is_present();
    group_flags[6] |= max_report_rs_idx_r15_present;
    group_flags[6] |= report_quant_rs_idx_nr_r15.is_present();
    group_flags[6] |= report_rs_idx_results_nr_present;
    group_flags[6] |= report_sftd_meas_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(si_request_for_ho_r9_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_quant_utra_fdd_r10_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(include_location_info_r11_present, 1));
      if (include_location_info_r11_present) {
        HANDLE_CODE(bref.pack(include_location_info_r11, 1));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(b2_thres1_v1250.is_present(), 1));
      if (b2_thres1_v1250.is_present()) {
        HANDLE_CODE(b2_thres1_v1250->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_quant_wlan_r13.is_present(), 1));
      if (report_quant_wlan_r13.is_present()) {
        HANDLE_CODE(report_quant_wlan_r13->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_any_wlan_r14_present, 1));
      if (report_any_wlan_r14_present) {
        HANDLE_CODE(bref.pack(report_any_wlan_r14, 1));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_quant_cell_nr_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(max_report_rs_idx_r15_present, 1));
      HANDLE_CODE(bref.pack(report_quant_rs_idx_nr_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(report_rs_idx_results_nr_present, 1));
      HANDLE_CODE(bref.pack(report_sftd_meas_r15_present, 1));
      if (report_quant_cell_nr_r15.is_present()) {
        HANDLE_CODE(report_quant_cell_nr_r15->pack(bref));
      }
      if (max_report_rs_idx_r15_present) {
        HANDLE_CODE(pack_integer(bref, max_report_rs_idx_r15, (uint8_t)0u, (uint8_t)32u));
      }
      if (report_quant_rs_idx_nr_r15.is_present()) {
        HANDLE_CODE(report_quant_rs_idx_nr_r15->pack(bref));
      }
      if (report_rs_idx_results_nr_present) {
        HANDLE_CODE(bref.pack(report_rs_idx_results_nr, 1));
      }
      if (report_sftd_meas_r15_present) {
        HANDLE_CODE(report_sftd_meas_r15.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_inter_rat_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(trigger_type.unpack(bref));
  HANDLE_CODE(unpack_integer(max_report_cells, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(report_interv.unpack(bref));
  HANDLE_CODE(report_amount.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(7);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(si_request_for_ho_r9_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(report_quant_utra_fdd_r10_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(include_location_info_r11_present, 1));
      if (include_location_info_r11_present) {
        HANDLE_CODE(bref.unpack(include_location_info_r11, 1));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool b2_thres1_v1250_present;
      HANDLE_CODE(bref.unpack(b2_thres1_v1250_present, 1));
      b2_thres1_v1250.set_present(b2_thres1_v1250_present);
      if (b2_thres1_v1250.is_present()) {
        HANDLE_CODE(b2_thres1_v1250->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool report_quant_wlan_r13_present;
      HANDLE_CODE(bref.unpack(report_quant_wlan_r13_present, 1));
      report_quant_wlan_r13.set_present(report_quant_wlan_r13_present);
      if (report_quant_wlan_r13.is_present()) {
        HANDLE_CODE(report_quant_wlan_r13->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(report_any_wlan_r14_present, 1));
      if (report_any_wlan_r14_present) {
        HANDLE_CODE(bref.unpack(report_any_wlan_r14, 1));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool report_quant_cell_nr_r15_present;
      HANDLE_CODE(bref.unpack(report_quant_cell_nr_r15_present, 1));
      report_quant_cell_nr_r15.set_present(report_quant_cell_nr_r15_present);
      HANDLE_CODE(bref.unpack(max_report_rs_idx_r15_present, 1));
      bool report_quant_rs_idx_nr_r15_present;
      HANDLE_CODE(bref.unpack(report_quant_rs_idx_nr_r15_present, 1));
      report_quant_rs_idx_nr_r15.set_present(report_quant_rs_idx_nr_r15_present);
      HANDLE_CODE(bref.unpack(report_rs_idx_results_nr_present, 1));
      HANDLE_CODE(bref.unpack(report_sftd_meas_r15_present, 1));
      if (report_quant_cell_nr_r15.is_present()) {
        HANDLE_CODE(report_quant_cell_nr_r15->unpack(bref));
      }
      if (max_report_rs_idx_r15_present) {
        HANDLE_CODE(unpack_integer(max_report_rs_idx_r15, bref, (uint8_t)0u, (uint8_t)32u));
      }
      if (report_quant_rs_idx_nr_r15.is_present()) {
        HANDLE_CODE(report_quant_rs_idx_nr_r15->unpack(bref));
      }
      if (report_rs_idx_results_nr_present) {
        HANDLE_CODE(bref.unpack(report_rs_idx_results_nr, 1));
      }
      if (report_sftd_meas_r15_present) {
        HANDLE_CODE(report_sftd_meas_r15.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void report_cfg_inter_rat_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("triggerType");
  trigger_type.to_json(j);
  j.write_int("maxReportCells", max_report_cells);
  j.write_str("reportInterval", report_interv.to_string());
  j.write_str("reportAmount", report_amount.to_string());
  if (ext) {
    if (si_request_for_ho_r9_present) {
      j.write_str("si-RequestForHO-r9", "setup");
    }
    if (report_quant_utra_fdd_r10_present) {
      j.write_str("reportQuantityUTRA-FDD-r10", "both");
    }
    if (include_location_info_r11_present) {
      j.write_bool("includeLocationInfo-r11", include_location_info_r11);
    }
    if (b2_thres1_v1250.is_present()) {
      j.write_fieldname("b2-Threshold1-v1250");
      b2_thres1_v1250->to_json(j);
    }
    if (report_quant_wlan_r13.is_present()) {
      j.write_fieldname("reportQuantityWLAN-r13");
      report_quant_wlan_r13->to_json(j);
    }
    if (report_any_wlan_r14_present) {
      j.write_bool("reportAnyWLAN-r14", report_any_wlan_r14);
    }
    if (report_quant_cell_nr_r15.is_present()) {
      j.write_fieldname("reportQuantityCellNR-r15");
      report_quant_cell_nr_r15->to_json(j);
    }
    if (max_report_rs_idx_r15_present) {
      j.write_int("maxReportRS-Index-r15", max_report_rs_idx_r15);
    }
    if (report_quant_rs_idx_nr_r15.is_present()) {
      j.write_fieldname("reportQuantityRS-IndexNR-r15");
      report_quant_rs_idx_nr_r15->to_json(j);
    }
    if (report_rs_idx_results_nr_present) {
      j.write_bool("reportRS-IndexResultsNR", report_rs_idx_results_nr);
    }
    if (report_sftd_meas_r15_present) {
      j.write_str("reportSFTD-Meas-r15", report_sftd_meas_r15.to_string());
    }
  }
  j.end_obj();
}
bool report_cfg_inter_rat_s::operator==(const report_cfg_inter_rat_s& other) const
{
  return ext == other.ext and trigger_type == other.trigger_type and max_report_cells == other.max_report_cells and
         report_interv == other.report_interv and report_amount == other.report_amount and
         (not ext or
          (si_request_for_ho_r9_present == other.si_request_for_ho_r9_present and
           report_quant_utra_fdd_r10_present == other.report_quant_utra_fdd_r10_present and
           include_location_info_r11_present == other.include_location_info_r11_present and
           (not include_location_info_r11_present or include_location_info_r11 == other.include_location_info_r11) and
           b2_thres1_v1250.is_present() == other.b2_thres1_v1250.is_present() and
           (not b2_thres1_v1250.is_present() or *b2_thres1_v1250 == *other.b2_thres1_v1250) and
           report_quant_wlan_r13.is_present() == other.report_quant_wlan_r13.is_present() and
           (not report_quant_wlan_r13.is_present() or *report_quant_wlan_r13 == *other.report_quant_wlan_r13) and
           report_any_wlan_r14_present == other.report_any_wlan_r14_present and
           (not report_any_wlan_r14_present or report_any_wlan_r14 == other.report_any_wlan_r14) and
           report_quant_cell_nr_r15.is_present() == other.report_quant_cell_nr_r15.is_present() and
           (not report_quant_cell_nr_r15.is_present() or
            *report_quant_cell_nr_r15 == *other.report_quant_cell_nr_r15) and
           max_report_rs_idx_r15_present == other.max_report_rs_idx_r15_present and
           (not max_report_rs_idx_r15_present or max_report_rs_idx_r15 == other.max_report_rs_idx_r15) and
           report_quant_rs_idx_nr_r15.is_present() == other.report_quant_rs_idx_nr_r15.is_present() and
           (not report_quant_rs_idx_nr_r15.is_present() or
            *report_quant_rs_idx_nr_r15 == *other.report_quant_rs_idx_nr_r15) and
           report_rs_idx_results_nr_present == other.report_rs_idx_results_nr_present and
           (not report_rs_idx_results_nr_present or report_rs_idx_results_nr == other.report_rs_idx_results_nr) and
           report_sftd_meas_r15_present == other.report_sftd_meas_r15_present and
           (not report_sftd_meas_r15_present or report_sftd_meas_r15 == other.report_sftd_meas_r15)));
}

void report_cfg_inter_rat_s::trigger_type_c_::destroy_()
{
  switch (type_) {
    case types::event:
      c.destroy<event_s_>();
      break;
    case types::periodical:
      c.destroy<periodical_s_>();
      break;
    default:
      break;
  }
}
void report_cfg_inter_rat_s::trigger_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::event:
      c.init<event_s_>();
      break;
    case types::periodical:
      c.init<periodical_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::trigger_type_c_");
  }
}
report_cfg_inter_rat_s::trigger_type_c_::trigger_type_c_(const report_cfg_inter_rat_s::trigger_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::event:
      c.init(other.c.get<event_s_>());
      break;
    case types::periodical:
      c.init(other.c.get<periodical_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::trigger_type_c_");
  }
}
report_cfg_inter_rat_s::trigger_type_c_&
report_cfg_inter_rat_s::trigger_type_c_::operator=(const report_cfg_inter_rat_s::trigger_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::event:
      c.set(other.c.get<event_s_>());
      break;
    case types::periodical:
      c.set(other.c.get<periodical_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::trigger_type_c_");
  }

  return *this;
}
report_cfg_inter_rat_s::trigger_type_c_::event_s_& report_cfg_inter_rat_s::trigger_type_c_::set_event()
{
  set(types::event);
  return c.get<event_s_>();
}
report_cfg_inter_rat_s::trigger_type_c_::periodical_s_& report_cfg_inter_rat_s::trigger_type_c_::set_periodical()
{
  set(types::periodical);
  return c.get<periodical_s_>();
}
void report_cfg_inter_rat_s::trigger_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::event:
      j.write_fieldname("event");
      j.start_obj();
      j.write_fieldname("eventId");
      c.get<event_s_>().event_id.to_json(j);
      j.write_int("hysteresis", c.get<event_s_>().hysteresis);
      j.write_str("timeToTrigger", c.get<event_s_>().time_to_trigger.to_string());
      j.end_obj();
      break;
    case types::periodical:
      j.write_fieldname("periodical");
      j.start_obj();
      j.write_str("purpose", c.get<periodical_s_>().purpose.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::trigger_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE report_cfg_inter_rat_s::trigger_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::event:
      HANDLE_CODE(c.get<event_s_>().event_id.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.get<event_s_>().hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_s_>().time_to_trigger.pack(bref));
      break;
    case types::periodical:
      HANDLE_CODE(c.get<periodical_s_>().purpose.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::trigger_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_inter_rat_s::trigger_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::event:
      HANDLE_CODE(c.get<event_s_>().event_id.unpack(bref));
      HANDLE_CODE(unpack_integer(c.get<event_s_>().hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_s_>().time_to_trigger.unpack(bref));
      break;
    case types::periodical:
      HANDLE_CODE(c.get<periodical_s_>().purpose.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::trigger_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool report_cfg_inter_rat_s::trigger_type_c_::operator==(const trigger_type_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::event:
      return c.get<event_s_>().event_id == other.c.get<event_s_>().event_id and
             c.get<event_s_>().hysteresis == other.c.get<event_s_>().hysteresis and
             c.get<event_s_>().time_to_trigger == other.c.get<event_s_>().time_to_trigger;
    case types::periodical:
      return c.get<periodical_s_>().purpose == other.c.get<periodical_s_>().purpose;
    default:
      return true;
  }
  return true;
}

void report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::destroy_()
{
  switch (type_) {
    case types::event_b1:
      c.destroy<event_b1_s_>();
      break;
    case types::event_b2:
      c.destroy<event_b2_s_>();
      break;
    case types::event_w1_r13:
      c.destroy<event_w1_r13_s_>();
      break;
    case types::event_w2_r13:
      c.destroy<event_w2_r13_s_>();
      break;
    case types::event_w3_r13:
      c.destroy<event_w3_r13_s_>();
      break;
    case types::event_b1_nr_r15:
      c.destroy<event_b1_nr_r15_s_>();
      break;
    case types::event_b2_nr_r15:
      c.destroy<event_b2_nr_r15_s_>();
      break;
    default:
      break;
  }
}
void report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::event_b1:
      c.init<event_b1_s_>();
      break;
    case types::event_b2:
      c.init<event_b2_s_>();
      break;
    case types::event_w1_r13:
      c.init<event_w1_r13_s_>();
      break;
    case types::event_w2_r13:
      c.init<event_w2_r13_s_>();
      break;
    case types::event_w3_r13:
      c.init<event_w3_r13_s_>();
      break;
    case types::event_b1_nr_r15:
      c.init<event_b1_nr_r15_s_>();
      break;
    case types::event_b2_nr_r15:
      c.init<event_b2_nr_r15_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_");
  }
}
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_id_c_(
    const report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::event_b1:
      c.init(other.c.get<event_b1_s_>());
      break;
    case types::event_b2:
      c.init(other.c.get<event_b2_s_>());
      break;
    case types::event_w1_r13:
      c.init(other.c.get<event_w1_r13_s_>());
      break;
    case types::event_w2_r13:
      c.init(other.c.get<event_w2_r13_s_>());
      break;
    case types::event_w3_r13:
      c.init(other.c.get<event_w3_r13_s_>());
      break;
    case types::event_b1_nr_r15:
      c.init(other.c.get<event_b1_nr_r15_s_>());
      break;
    case types::event_b2_nr_r15:
      c.init(other.c.get<event_b2_nr_r15_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_");
  }
}
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::operator=(
    const report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::event_b1:
      c.set(other.c.get<event_b1_s_>());
      break;
    case types::event_b2:
      c.set(other.c.get<event_b2_s_>());
      break;
    case types::event_w1_r13:
      c.set(other.c.get<event_w1_r13_s_>());
      break;
    case types::event_w2_r13:
      c.set(other.c.get<event_w2_r13_s_>());
      break;
    case types::event_w3_r13:
      c.set(other.c.get<event_w3_r13_s_>());
      break;
    case types::event_b1_nr_r15:
      c.set(other.c.get<event_b1_nr_r15_s_>());
      break;
    case types::event_b2_nr_r15:
      c.set(other.c.get<event_b2_nr_r15_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_");
  }

  return *this;
}
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::set_event_b1()
{
  set(types::event_b1);
  return c.get<event_b1_s_>();
}
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::set_event_b2()
{
  set(types::event_b2);
  return c.get<event_b2_s_>();
}
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_w1_r13_s_&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::set_event_w1_r13()
{
  set(types::event_w1_r13);
  return c.get<event_w1_r13_s_>();
}
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_w2_r13_s_&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::set_event_w2_r13()
{
  set(types::event_w2_r13);
  return c.get<event_w2_r13_s_>();
}
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_w3_r13_s_&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::set_event_w3_r13()
{
  set(types::event_w3_r13);
  return c.get<event_w3_r13_s_>();
}
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_nr_r15_s_&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::set_event_b1_nr_r15()
{
  set(types::event_b1_nr_r15);
  return c.get<event_b1_nr_r15_s_>();
}
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_nr_r15_s_&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::set_event_b2_nr_r15()
{
  set(types::event_b2_nr_r15);
  return c.get<event_b2_nr_r15_s_>();
}
void report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::event_b1:
      j.write_fieldname("eventB1");
      j.start_obj();
      j.write_fieldname("b1-Threshold");
      c.get<event_b1_s_>().b1_thres.to_json(j);
      j.end_obj();
      break;
    case types::event_b2:
      j.write_fieldname("eventB2");
      j.start_obj();
      j.write_fieldname("b2-Threshold1");
      c.get<event_b2_s_>().b2_thres1.to_json(j);
      j.write_fieldname("b2-Threshold2");
      c.get<event_b2_s_>().b2_thres2.to_json(j);
      j.end_obj();
      break;
    case types::event_w1_r13:
      j.write_fieldname("eventW1-r13");
      j.start_obj();
      j.write_int("w1-Threshold-r13", c.get<event_w1_r13_s_>().w1_thres_r13);
      j.end_obj();
      break;
    case types::event_w2_r13:
      j.write_fieldname("eventW2-r13");
      j.start_obj();
      j.write_int("w2-Threshold1-r13", c.get<event_w2_r13_s_>().w2_thres1_r13);
      j.write_int("w2-Threshold2-r13", c.get<event_w2_r13_s_>().w2_thres2_r13);
      j.end_obj();
      break;
    case types::event_w3_r13:
      j.write_fieldname("eventW3-r13");
      j.start_obj();
      j.write_int("w3-Threshold-r13", c.get<event_w3_r13_s_>().w3_thres_r13);
      j.end_obj();
      break;
    case types::event_b1_nr_r15:
      j.write_fieldname("eventB1-NR-r15");
      j.start_obj();
      j.write_fieldname("b1-ThresholdNR-r15");
      c.get<event_b1_nr_r15_s_>().b1_thres_nr_r15.to_json(j);
      j.write_bool("reportOnLeave-r15", c.get<event_b1_nr_r15_s_>().report_on_leave_r15);
      j.end_obj();
      break;
    case types::event_b2_nr_r15:
      j.write_fieldname("eventB2-NR-r15");
      j.start_obj();
      j.write_fieldname("b2-Threshold1-r15");
      c.get<event_b2_nr_r15_s_>().b2_thres1_r15.to_json(j);
      j.write_fieldname("b2-Threshold2NR-r15");
      c.get<event_b2_nr_r15_s_>().b2_thres2_nr_r15.to_json(j);
      j.write_bool("reportOnLeave-r15", c.get<event_b2_nr_r15_s_>().report_on_leave_r15);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_");
  }
  j.end_obj();
}
SRSASN_CODE report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::event_b1:
      HANDLE_CODE(c.get<event_b1_s_>().b1_thres.pack(bref));
      break;
    case types::event_b2:
      HANDLE_CODE(c.get<event_b2_s_>().b2_thres1.pack(bref));
      HANDLE_CODE(c.get<event_b2_s_>().b2_thres2.pack(bref));
      break;
    case types::event_w1_r13: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<event_w1_r13_s_>().w1_thres_r13, (uint8_t)0u, (uint8_t)141u));
    } break;
    case types::event_w2_r13: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<event_w2_r13_s_>().w2_thres1_r13, (uint8_t)0u, (uint8_t)141u));
      HANDLE_CODE(pack_integer(bref, c.get<event_w2_r13_s_>().w2_thres2_r13, (uint8_t)0u, (uint8_t)141u));
    } break;
    case types::event_w3_r13: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<event_w3_r13_s_>().w3_thres_r13, (uint8_t)0u, (uint8_t)141u));
    } break;
    case types::event_b1_nr_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<event_b1_nr_r15_s_>().b1_thres_nr_r15.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_b1_nr_r15_s_>().report_on_leave_r15, 1));
    } break;
    case types::event_b2_nr_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<event_b2_nr_r15_s_>().b2_thres1_r15.pack(bref));
      HANDLE_CODE(c.get<event_b2_nr_r15_s_>().b2_thres2_nr_r15.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_b2_nr_r15_s_>().report_on_leave_r15, 1));
    } break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::event_b1:
      HANDLE_CODE(c.get<event_b1_s_>().b1_thres.unpack(bref));
      break;
    case types::event_b2:
      HANDLE_CODE(c.get<event_b2_s_>().b2_thres1.unpack(bref));
      HANDLE_CODE(c.get<event_b2_s_>().b2_thres2.unpack(bref));
      break;
    case types::event_w1_r13: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<event_w1_r13_s_>().w1_thres_r13, bref, (uint8_t)0u, (uint8_t)141u));
    } break;
    case types::event_w2_r13: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<event_w2_r13_s_>().w2_thres1_r13, bref, (uint8_t)0u, (uint8_t)141u));
      HANDLE_CODE(unpack_integer(c.get<event_w2_r13_s_>().w2_thres2_r13, bref, (uint8_t)0u, (uint8_t)141u));
    } break;
    case types::event_w3_r13: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<event_w3_r13_s_>().w3_thres_r13, bref, (uint8_t)0u, (uint8_t)141u));
    } break;
    case types::event_b1_nr_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<event_b1_nr_r15_s_>().b1_thres_nr_r15.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_b1_nr_r15_s_>().report_on_leave_r15, 1));
    } break;
    case types::event_b2_nr_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<event_b2_nr_r15_s_>().b2_thres1_r15.unpack(bref));
      HANDLE_CODE(c.get<event_b2_nr_r15_s_>().b2_thres2_nr_r15.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_b2_nr_r15_s_>().report_on_leave_r15, 1));
    } break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::operator==(const event_id_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::event_b1:
      return c.get<event_b1_s_>().b1_thres == other.c.get<event_b1_s_>().b1_thres;
    case types::event_b2:
      return c.get<event_b2_s_>().b2_thres1 == other.c.get<event_b2_s_>().b2_thres1 and
             c.get<event_b2_s_>().b2_thres2 == other.c.get<event_b2_s_>().b2_thres2;
    case types::event_w1_r13:
      return c.get<event_w1_r13_s_>().w1_thres_r13 == other.c.get<event_w1_r13_s_>().w1_thres_r13;
    case types::event_w2_r13:
      return c.get<event_w2_r13_s_>().w2_thres1_r13 == other.c.get<event_w2_r13_s_>().w2_thres1_r13 and
             c.get<event_w2_r13_s_>().w2_thres2_r13 == other.c.get<event_w2_r13_s_>().w2_thres2_r13;
    case types::event_w3_r13:
      return c.get<event_w3_r13_s_>().w3_thres_r13 == other.c.get<event_w3_r13_s_>().w3_thres_r13;
    case types::event_b1_nr_r15:
      return c.get<event_b1_nr_r15_s_>().b1_thres_nr_r15 == other.c.get<event_b1_nr_r15_s_>().b1_thres_nr_r15 and
             c.get<event_b1_nr_r15_s_>().report_on_leave_r15 == other.c.get<event_b1_nr_r15_s_>().report_on_leave_r15;
    case types::event_b2_nr_r15:
      return c.get<event_b2_nr_r15_s_>().b2_thres1_r15 == other.c.get<event_b2_nr_r15_s_>().b2_thres1_r15 and
             c.get<event_b2_nr_r15_s_>().b2_thres2_nr_r15 == other.c.get<event_b2_nr_r15_s_>().b2_thres2_nr_r15 and
             c.get<event_b2_nr_r15_s_>().report_on_leave_r15 == other.c.get<event_b2_nr_r15_s_>().report_on_leave_r15;
    default:
      return true;
  }
  return true;
}

void report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::destroy_()
{
  switch (type_) {
    case types::b1_thres_utra:
      c.destroy<thres_utra_c>();
      break;
    default:
      break;
  }
}
void report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::b1_thres_utra:
      c.init<thres_utra_c>();
      break;
    case types::b1_thres_geran:
      break;
    case types::b1_thres_cdma2000:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_");
  }
}
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::b1_thres_c_(
    const report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::b1_thres_utra:
      c.init(other.c.get<thres_utra_c>());
      break;
    case types::b1_thres_geran:
      c.init(other.c.get<uint8_t>());
      break;
    case types::b1_thres_cdma2000:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_");
  }
}
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::operator=(
    const report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::b1_thres_utra:
      c.set(other.c.get<thres_utra_c>());
      break;
    case types::b1_thres_geran:
      c.set(other.c.get<uint8_t>());
      break;
    case types::b1_thres_cdma2000:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_");
  }

  return *this;
}
thres_utra_c&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::set_b1_thres_utra()
{
  set(types::b1_thres_utra);
  return c.get<thres_utra_c>();
}
uint8_t& report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::set_b1_thres_geran()
{
  set(types::b1_thres_geran);
  return c.get<uint8_t>();
}
uint8_t&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::set_b1_thres_cdma2000()
{
  set(types::b1_thres_cdma2000);
  return c.get<uint8_t>();
}
void report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::to_json(
    json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::b1_thres_utra:
      j.write_fieldname("b1-ThresholdUTRA");
      c.get<thres_utra_c>().to_json(j);
      break;
    case types::b1_thres_geran:
      j.write_int("b1-ThresholdGERAN", c.get<uint8_t>());
      break;
    case types::b1_thres_cdma2000:
      j.write_int("b1-ThresholdCDMA2000", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_,
                            "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_");
  }
  j.end_obj();
}
SRSASN_CODE
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::b1_thres_utra:
      HANDLE_CODE(c.get<thres_utra_c>().pack(bref));
      break;
    case types::b1_thres_geran:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::b1_thres_cdma2000:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    default:
      log_invalid_choice_id(type_,
                            "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::b1_thres_utra:
      HANDLE_CODE(c.get<thres_utra_c>().unpack(bref));
      break;
    case types::b1_thres_geran:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::b1_thres_cdma2000:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    default:
      log_invalid_choice_id(type_,
                            "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::operator==(
    const b1_thres_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::b1_thres_utra:
      return c.get<thres_utra_c>() == other.c.get<thres_utra_c>();
    case types::b1_thres_geran:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::b1_thres_cdma2000:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    default:
      return true;
  }
  return true;
}

void report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::destroy_()
{
  switch (type_) {
    case types::b2_thres2_utra:
      c.destroy<thres_utra_c>();
      break;
    default:
      break;
  }
}
void report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::b2_thres2_utra:
      c.init<thres_utra_c>();
      break;
    case types::b2_thres2_geran:
      break;
    case types::b2_thres2_cdma2000:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_");
  }
}
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::b2_thres2_c_(
    const report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::b2_thres2_utra:
      c.init(other.c.get<thres_utra_c>());
      break;
    case types::b2_thres2_geran:
      c.init(other.c.get<uint8_t>());
      break;
    case types::b2_thres2_cdma2000:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_");
  }
}
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::operator=(
    const report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::b2_thres2_utra:
      c.set(other.c.get<thres_utra_c>());
      break;
    case types::b2_thres2_geran:
      c.set(other.c.get<uint8_t>());
      break;
    case types::b2_thres2_cdma2000:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_");
  }

  return *this;
}
thres_utra_c&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::set_b2_thres2_utra()
{
  set(types::b2_thres2_utra);
  return c.get<thres_utra_c>();
}
uint8_t&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::set_b2_thres2_geran()
{
  set(types::b2_thres2_geran);
  return c.get<uint8_t>();
}
uint8_t&
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::set_b2_thres2_cdma2000()
{
  set(types::b2_thres2_cdma2000);
  return c.get<uint8_t>();
}
void report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::to_json(
    json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::b2_thres2_utra:
      j.write_fieldname("b2-Threshold2UTRA");
      c.get<thres_utra_c>().to_json(j);
      break;
    case types::b2_thres2_geran:
      j.write_int("b2-Threshold2GERAN", c.get<uint8_t>());
      break;
    case types::b2_thres2_cdma2000:
      j.write_int("b2-Threshold2CDMA2000", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(
          type_, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_");
  }
  j.end_obj();
}
SRSASN_CODE
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::b2_thres2_utra:
      HANDLE_CODE(c.get<thres_utra_c>().pack(bref));
      break;
    case types::b2_thres2_geran:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::b2_thres2_cdma2000:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    default:
      log_invalid_choice_id(
          type_, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::b2_thres2_utra:
      HANDLE_CODE(c.get<thres_utra_c>().unpack(bref));
      break;
    case types::b2_thres2_geran:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::b2_thres2_cdma2000:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    default:
      log_invalid_choice_id(
          type_, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::operator==(
    const b2_thres2_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::b2_thres2_utra:
      return c.get<thres_utra_c>() == other.c.get<thres_utra_c>();
    case types::b2_thres2_geran:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::b2_thres2_cdma2000:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    default:
      return true;
  }
  return true;
}

const char* report_cfg_inter_rat_s::trigger_type_c_::periodical_s_::purpose_opts::to_string() const
{
  static const char* options[] = {"reportStrongestCells", "reportStrongestCellsForSON", "reportCGI"};
  return convert_enum_idx(options, 3, value, "report_cfg_inter_rat_s::trigger_type_c_::periodical_s_::purpose_e_");
}

const char* report_cfg_inter_rat_s::report_amount_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(options, 8, value, "report_cfg_inter_rat_s::report_amount_e_");
}
int8_t report_cfg_inter_rat_s::report_amount_opts::to_number() const
{
  static const int8_t options[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(options, 8, value, "report_cfg_inter_rat_s::report_amount_e_");
}

void report_cfg_inter_rat_s::b2_thres1_v1250_c_::set(types::options e)
{
  type_ = e;
}
void report_cfg_inter_rat_s::b2_thres1_v1250_c_::set_release()
{
  set(types::release);
}
int8_t& report_cfg_inter_rat_s::b2_thres1_v1250_c_::set_setup()
{
  set(types::setup);
  return c;
}
void report_cfg_inter_rat_s::b2_thres1_v1250_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_int("setup", c);
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::b2_thres1_v1250_c_");
  }
  j.end_obj();
}
SRSASN_CODE report_cfg_inter_rat_s::b2_thres1_v1250_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c, (int8_t)-30, (int8_t)46));
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::b2_thres1_v1250_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_inter_rat_s::b2_thres1_v1250_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c, bref, (int8_t)-30, (int8_t)46));
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::b2_thres1_v1250_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool report_cfg_inter_rat_s::b2_thres1_v1250_c_::operator==(const b2_thres1_v1250_c_& other) const
{
  return type() == other.type() and c == other.c;
}

const char* report_cfg_inter_rat_s::report_sftd_meas_r15_opts::to_string() const
{
  static const char* options[] = {"pSCell", "neighborCells"};
  return convert_enum_idx(options, 2, value, "report_cfg_inter_rat_s::report_sftd_meas_r15_e_");
}

// MeasIdToAddMod ::= SEQUENCE
SRSASN_CODE meas_id_to_add_mod_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, meas_id, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, meas_obj_id, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, report_cfg_id, (uint8_t)1u, (uint8_t)32u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_id_to_add_mod_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(meas_id, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(meas_obj_id, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(report_cfg_id, bref, (uint8_t)1u, (uint8_t)32u));

  return SRSASN_SUCCESS;
}
void meas_id_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measId", meas_id);
  j.write_int("measObjectId", meas_obj_id);
  j.write_int("reportConfigId", report_cfg_id);
  j.end_obj();
}
bool meas_id_to_add_mod_s::operator==(const meas_id_to_add_mod_s& other) const
{
  return meas_id == other.meas_id and meas_obj_id == other.meas_obj_id and report_cfg_id == other.report_cfg_id;
}

// MeasIdToAddMod-v1310 ::= SEQUENCE
SRSASN_CODE meas_id_to_add_mod_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_obj_id_v1310_present, 1));

  if (meas_obj_id_v1310_present) {
    HANDLE_CODE(pack_integer(bref, meas_obj_id_v1310, (uint8_t)33u, (uint8_t)64u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_id_to_add_mod_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_obj_id_v1310_present, 1));

  if (meas_obj_id_v1310_present) {
    HANDLE_CODE(unpack_integer(meas_obj_id_v1310, bref, (uint8_t)33u, (uint8_t)64u));
  }

  return SRSASN_SUCCESS;
}
void meas_id_to_add_mod_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_obj_id_v1310_present) {
    j.write_int("measObjectId-v1310", meas_obj_id_v1310);
  }
  j.end_obj();
}

// MeasIdToAddModExt-r12 ::= SEQUENCE
SRSASN_CODE meas_id_to_add_mod_ext_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, meas_id_v1250, (uint8_t)33u, (uint8_t)64u));
  HANDLE_CODE(pack_integer(bref, meas_obj_id_r12, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, report_cfg_id_r12, (uint8_t)1u, (uint8_t)32u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_id_to_add_mod_ext_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(meas_id_v1250, bref, (uint8_t)33u, (uint8_t)64u));
  HANDLE_CODE(unpack_integer(meas_obj_id_r12, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(report_cfg_id_r12, bref, (uint8_t)1u, (uint8_t)32u));

  return SRSASN_SUCCESS;
}
void meas_id_to_add_mod_ext_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measId-v1250", meas_id_v1250);
  j.write_int("measObjectId-r12", meas_obj_id_r12);
  j.write_int("reportConfigId-r12", report_cfg_id_r12);
  j.end_obj();
}

// MeasObjectToAddMod ::= SEQUENCE
SRSASN_CODE meas_obj_to_add_mod_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, meas_obj_id, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(meas_obj.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_to_add_mod_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(meas_obj_id, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(meas_obj.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_obj_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measObjectId", meas_obj_id);
  j.write_fieldname("measObject");
  meas_obj.to_json(j);
  j.end_obj();
}
bool meas_obj_to_add_mod_s::operator==(const meas_obj_to_add_mod_s& other) const
{
  return meas_obj_id == other.meas_obj_id and meas_obj == other.meas_obj;
}

void meas_obj_to_add_mod_s::meas_obj_c_::destroy_()
{
  switch (type_) {
    case types::meas_obj_eutra:
      c.destroy<meas_obj_eutra_s>();
      break;
    case types::meas_obj_utra:
      c.destroy<meas_obj_utra_s>();
      break;
    case types::meas_obj_geran:
      c.destroy<meas_obj_geran_s>();
      break;
    case types::meas_obj_cdma2000:
      c.destroy<meas_obj_cdma2000_s>();
      break;
    case types::meas_obj_wlan_r13:
      c.destroy<meas_obj_wlan_r13_s>();
      break;
    case types::meas_obj_nr_r15:
      c.destroy<meas_obj_nr_r15_s>();
      break;
    default:
      break;
  }
}
void meas_obj_to_add_mod_s::meas_obj_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::meas_obj_eutra:
      c.init<meas_obj_eutra_s>();
      break;
    case types::meas_obj_utra:
      c.init<meas_obj_utra_s>();
      break;
    case types::meas_obj_geran:
      c.init<meas_obj_geran_s>();
      break;
    case types::meas_obj_cdma2000:
      c.init<meas_obj_cdma2000_s>();
      break;
    case types::meas_obj_wlan_r13:
      c.init<meas_obj_wlan_r13_s>();
      break;
    case types::meas_obj_nr_r15:
      c.init<meas_obj_nr_r15_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_s::meas_obj_c_");
  }
}
meas_obj_to_add_mod_s::meas_obj_c_::meas_obj_c_(const meas_obj_to_add_mod_s::meas_obj_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::meas_obj_eutra:
      c.init(other.c.get<meas_obj_eutra_s>());
      break;
    case types::meas_obj_utra:
      c.init(other.c.get<meas_obj_utra_s>());
      break;
    case types::meas_obj_geran:
      c.init(other.c.get<meas_obj_geran_s>());
      break;
    case types::meas_obj_cdma2000:
      c.init(other.c.get<meas_obj_cdma2000_s>());
      break;
    case types::meas_obj_wlan_r13:
      c.init(other.c.get<meas_obj_wlan_r13_s>());
      break;
    case types::meas_obj_nr_r15:
      c.init(other.c.get<meas_obj_nr_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_s::meas_obj_c_");
  }
}
meas_obj_to_add_mod_s::meas_obj_c_&
meas_obj_to_add_mod_s::meas_obj_c_::operator=(const meas_obj_to_add_mod_s::meas_obj_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::meas_obj_eutra:
      c.set(other.c.get<meas_obj_eutra_s>());
      break;
    case types::meas_obj_utra:
      c.set(other.c.get<meas_obj_utra_s>());
      break;
    case types::meas_obj_geran:
      c.set(other.c.get<meas_obj_geran_s>());
      break;
    case types::meas_obj_cdma2000:
      c.set(other.c.get<meas_obj_cdma2000_s>());
      break;
    case types::meas_obj_wlan_r13:
      c.set(other.c.get<meas_obj_wlan_r13_s>());
      break;
    case types::meas_obj_nr_r15:
      c.set(other.c.get<meas_obj_nr_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_s::meas_obj_c_");
  }

  return *this;
}
meas_obj_eutra_s& meas_obj_to_add_mod_s::meas_obj_c_::set_meas_obj_eutra()
{
  set(types::meas_obj_eutra);
  return c.get<meas_obj_eutra_s>();
}
meas_obj_utra_s& meas_obj_to_add_mod_s::meas_obj_c_::set_meas_obj_utra()
{
  set(types::meas_obj_utra);
  return c.get<meas_obj_utra_s>();
}
meas_obj_geran_s& meas_obj_to_add_mod_s::meas_obj_c_::set_meas_obj_geran()
{
  set(types::meas_obj_geran);
  return c.get<meas_obj_geran_s>();
}
meas_obj_cdma2000_s& meas_obj_to_add_mod_s::meas_obj_c_::set_meas_obj_cdma2000()
{
  set(types::meas_obj_cdma2000);
  return c.get<meas_obj_cdma2000_s>();
}
meas_obj_wlan_r13_s& meas_obj_to_add_mod_s::meas_obj_c_::set_meas_obj_wlan_r13()
{
  set(types::meas_obj_wlan_r13);
  return c.get<meas_obj_wlan_r13_s>();
}
meas_obj_nr_r15_s& meas_obj_to_add_mod_s::meas_obj_c_::set_meas_obj_nr_r15()
{
  set(types::meas_obj_nr_r15);
  return c.get<meas_obj_nr_r15_s>();
}
void meas_obj_to_add_mod_s::meas_obj_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_obj_eutra:
      j.write_fieldname("measObjectEUTRA");
      c.get<meas_obj_eutra_s>().to_json(j);
      break;
    case types::meas_obj_utra:
      j.write_fieldname("measObjectUTRA");
      c.get<meas_obj_utra_s>().to_json(j);
      break;
    case types::meas_obj_geran:
      j.write_fieldname("measObjectGERAN");
      c.get<meas_obj_geran_s>().to_json(j);
      break;
    case types::meas_obj_cdma2000:
      j.write_fieldname("measObjectCDMA2000");
      c.get<meas_obj_cdma2000_s>().to_json(j);
      break;
    case types::meas_obj_wlan_r13:
      j.write_fieldname("measObjectWLAN-r13");
      c.get<meas_obj_wlan_r13_s>().to_json(j);
      break;
    case types::meas_obj_nr_r15:
      j.write_fieldname("measObjectNR-r15");
      c.get<meas_obj_nr_r15_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_s::meas_obj_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_obj_to_add_mod_s::meas_obj_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_obj_eutra:
      HANDLE_CODE(c.get<meas_obj_eutra_s>().pack(bref));
      break;
    case types::meas_obj_utra:
      HANDLE_CODE(c.get<meas_obj_utra_s>().pack(bref));
      break;
    case types::meas_obj_geran:
      HANDLE_CODE(c.get<meas_obj_geran_s>().pack(bref));
      break;
    case types::meas_obj_cdma2000:
      HANDLE_CODE(c.get<meas_obj_cdma2000_s>().pack(bref));
      break;
    case types::meas_obj_wlan_r13: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_wlan_r13_s>().pack(bref));
    } break;
    case types::meas_obj_nr_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_nr_r15_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_s::meas_obj_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_to_add_mod_s::meas_obj_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_obj_eutra:
      HANDLE_CODE(c.get<meas_obj_eutra_s>().unpack(bref));
      break;
    case types::meas_obj_utra:
      HANDLE_CODE(c.get<meas_obj_utra_s>().unpack(bref));
      break;
    case types::meas_obj_geran:
      HANDLE_CODE(c.get<meas_obj_geran_s>().unpack(bref));
      break;
    case types::meas_obj_cdma2000:
      HANDLE_CODE(c.get<meas_obj_cdma2000_s>().unpack(bref));
      break;
    case types::meas_obj_wlan_r13: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_wlan_r13_s>().unpack(bref));
    } break;
    case types::meas_obj_nr_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_nr_r15_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_s::meas_obj_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool meas_obj_to_add_mod_s::meas_obj_c_::operator==(const meas_obj_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::meas_obj_eutra:
      return c.get<meas_obj_eutra_s>() == other.c.get<meas_obj_eutra_s>();
    case types::meas_obj_utra:
      return c.get<meas_obj_utra_s>() == other.c.get<meas_obj_utra_s>();
    case types::meas_obj_geran:
      return c.get<meas_obj_geran_s>() == other.c.get<meas_obj_geran_s>();
    case types::meas_obj_cdma2000:
      return c.get<meas_obj_cdma2000_s>() == other.c.get<meas_obj_cdma2000_s>();
    case types::meas_obj_wlan_r13:
      return c.get<meas_obj_wlan_r13_s>() == other.c.get<meas_obj_wlan_r13_s>();
    case types::meas_obj_nr_r15:
      return c.get<meas_obj_nr_r15_s>() == other.c.get<meas_obj_nr_r15_s>();
    default:
      return true;
  }
  return true;
}

// MeasObjectToAddMod-v9e0 ::= SEQUENCE
SRSASN_CODE meas_obj_to_add_mod_v9e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_obj_eutra_v9e0_present, 1));

  if (meas_obj_eutra_v9e0_present) {
    HANDLE_CODE(meas_obj_eutra_v9e0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_to_add_mod_v9e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_obj_eutra_v9e0_present, 1));

  if (meas_obj_eutra_v9e0_present) {
    HANDLE_CODE(meas_obj_eutra_v9e0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_obj_to_add_mod_v9e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_obj_eutra_v9e0_present) {
    j.write_fieldname("measObjectEUTRA-v9e0");
    meas_obj_eutra_v9e0.to_json(j);
  }
  j.end_obj();
}

// MeasObjectToAddModExt-r13 ::= SEQUENCE
SRSASN_CODE meas_obj_to_add_mod_ext_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, meas_obj_id_r13, (uint8_t)33u, (uint8_t)64u));
  HANDLE_CODE(meas_obj_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_to_add_mod_ext_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(meas_obj_id_r13, bref, (uint8_t)33u, (uint8_t)64u));
  HANDLE_CODE(meas_obj_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_obj_to_add_mod_ext_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measObjectId-r13", meas_obj_id_r13);
  j.write_fieldname("measObject-r13");
  meas_obj_r13.to_json(j);
  j.end_obj();
}

void meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::destroy_()
{
  switch (type_) {
    case types::meas_obj_eutra_r13:
      c.destroy<meas_obj_eutra_s>();
      break;
    case types::meas_obj_utra_r13:
      c.destroy<meas_obj_utra_s>();
      break;
    case types::meas_obj_geran_r13:
      c.destroy<meas_obj_geran_s>();
      break;
    case types::meas_obj_cdma2000_r13:
      c.destroy<meas_obj_cdma2000_s>();
      break;
    case types::meas_obj_wlan_v1320:
      c.destroy<meas_obj_wlan_r13_s>();
      break;
    case types::meas_obj_nr_r15:
      c.destroy<meas_obj_nr_r15_s>();
      break;
    default:
      break;
  }
}
void meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::meas_obj_eutra_r13:
      c.init<meas_obj_eutra_s>();
      break;
    case types::meas_obj_utra_r13:
      c.init<meas_obj_utra_s>();
      break;
    case types::meas_obj_geran_r13:
      c.init<meas_obj_geran_s>();
      break;
    case types::meas_obj_cdma2000_r13:
      c.init<meas_obj_cdma2000_s>();
      break;
    case types::meas_obj_wlan_v1320:
      c.init<meas_obj_wlan_r13_s>();
      break;
    case types::meas_obj_nr_r15:
      c.init<meas_obj_nr_r15_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_");
  }
}
meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::meas_obj_r13_c_(
    const meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::meas_obj_eutra_r13:
      c.init(other.c.get<meas_obj_eutra_s>());
      break;
    case types::meas_obj_utra_r13:
      c.init(other.c.get<meas_obj_utra_s>());
      break;
    case types::meas_obj_geran_r13:
      c.init(other.c.get<meas_obj_geran_s>());
      break;
    case types::meas_obj_cdma2000_r13:
      c.init(other.c.get<meas_obj_cdma2000_s>());
      break;
    case types::meas_obj_wlan_v1320:
      c.init(other.c.get<meas_obj_wlan_r13_s>());
      break;
    case types::meas_obj_nr_r15:
      c.init(other.c.get<meas_obj_nr_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_");
  }
}
meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_&
meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::operator=(const meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::meas_obj_eutra_r13:
      c.set(other.c.get<meas_obj_eutra_s>());
      break;
    case types::meas_obj_utra_r13:
      c.set(other.c.get<meas_obj_utra_s>());
      break;
    case types::meas_obj_geran_r13:
      c.set(other.c.get<meas_obj_geran_s>());
      break;
    case types::meas_obj_cdma2000_r13:
      c.set(other.c.get<meas_obj_cdma2000_s>());
      break;
    case types::meas_obj_wlan_v1320:
      c.set(other.c.get<meas_obj_wlan_r13_s>());
      break;
    case types::meas_obj_nr_r15:
      c.set(other.c.get<meas_obj_nr_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_");
  }

  return *this;
}
meas_obj_eutra_s& meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::set_meas_obj_eutra_r13()
{
  set(types::meas_obj_eutra_r13);
  return c.get<meas_obj_eutra_s>();
}
meas_obj_utra_s& meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::set_meas_obj_utra_r13()
{
  set(types::meas_obj_utra_r13);
  return c.get<meas_obj_utra_s>();
}
meas_obj_geran_s& meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::set_meas_obj_geran_r13()
{
  set(types::meas_obj_geran_r13);
  return c.get<meas_obj_geran_s>();
}
meas_obj_cdma2000_s& meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::set_meas_obj_cdma2000_r13()
{
  set(types::meas_obj_cdma2000_r13);
  return c.get<meas_obj_cdma2000_s>();
}
meas_obj_wlan_r13_s& meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::set_meas_obj_wlan_v1320()
{
  set(types::meas_obj_wlan_v1320);
  return c.get<meas_obj_wlan_r13_s>();
}
meas_obj_nr_r15_s& meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::set_meas_obj_nr_r15()
{
  set(types::meas_obj_nr_r15);
  return c.get<meas_obj_nr_r15_s>();
}
void meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_obj_eutra_r13:
      j.write_fieldname("measObjectEUTRA-r13");
      c.get<meas_obj_eutra_s>().to_json(j);
      break;
    case types::meas_obj_utra_r13:
      j.write_fieldname("measObjectUTRA-r13");
      c.get<meas_obj_utra_s>().to_json(j);
      break;
    case types::meas_obj_geran_r13:
      j.write_fieldname("measObjectGERAN-r13");
      c.get<meas_obj_geran_s>().to_json(j);
      break;
    case types::meas_obj_cdma2000_r13:
      j.write_fieldname("measObjectCDMA2000-r13");
      c.get<meas_obj_cdma2000_s>().to_json(j);
      break;
    case types::meas_obj_wlan_v1320:
      j.write_fieldname("measObjectWLAN-v1320");
      c.get<meas_obj_wlan_r13_s>().to_json(j);
      break;
    case types::meas_obj_nr_r15:
      j.write_fieldname("measObjectNR-r15");
      c.get<meas_obj_nr_r15_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_obj_eutra_r13:
      HANDLE_CODE(c.get<meas_obj_eutra_s>().pack(bref));
      break;
    case types::meas_obj_utra_r13:
      HANDLE_CODE(c.get<meas_obj_utra_s>().pack(bref));
      break;
    case types::meas_obj_geran_r13:
      HANDLE_CODE(c.get<meas_obj_geran_s>().pack(bref));
      break;
    case types::meas_obj_cdma2000_r13:
      HANDLE_CODE(c.get<meas_obj_cdma2000_s>().pack(bref));
      break;
    case types::meas_obj_wlan_v1320: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_wlan_r13_s>().pack(bref));
    } break;
    case types::meas_obj_nr_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_nr_r15_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_obj_eutra_r13:
      HANDLE_CODE(c.get<meas_obj_eutra_s>().unpack(bref));
      break;
    case types::meas_obj_utra_r13:
      HANDLE_CODE(c.get<meas_obj_utra_s>().unpack(bref));
      break;
    case types::meas_obj_geran_r13:
      HANDLE_CODE(c.get<meas_obj_geran_s>().unpack(bref));
      break;
    case types::meas_obj_cdma2000_r13:
      HANDLE_CODE(c.get<meas_obj_cdma2000_s>().unpack(bref));
      break;
    case types::meas_obj_wlan_v1320: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_wlan_r13_s>().unpack(bref));
    } break;
    case types::meas_obj_nr_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_nr_r15_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// QuantityConfigCDMA2000 ::= SEQUENCE
SRSASN_CODE quant_cfg_cdma2000_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(meas_quant_cdma2000.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_cdma2000_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(meas_quant_cdma2000.unpack(bref));

  return SRSASN_SUCCESS;
}
void quant_cfg_cdma2000_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("measQuantityCDMA2000", meas_quant_cdma2000.to_string());
  j.end_obj();
}
bool quant_cfg_cdma2000_s::operator==(const quant_cfg_cdma2000_s& other) const
{
  return meas_quant_cdma2000 == other.meas_quant_cdma2000;
}

const char* quant_cfg_cdma2000_s::meas_quant_cdma2000_opts::to_string() const
{
  static const char* options[] = {"pilotStrength", "pilotPnPhaseAndPilotStrength"};
  return convert_enum_idx(options, 2, value, "quant_cfg_cdma2000_s::meas_quant_cdma2000_e_");
}

// QuantityConfigEUTRA ::= SEQUENCE
SRSASN_CODE quant_cfg_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(filt_coef_rsrp_present, 1));
  HANDLE_CODE(bref.pack(filt_coef_rsrq_present, 1));

  if (filt_coef_rsrp_present) {
    HANDLE_CODE(filt_coef_rsrp.pack(bref));
  }
  if (filt_coef_rsrq_present) {
    HANDLE_CODE(filt_coef_rsrq.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(filt_coef_rsrp_present, 1));
  HANDLE_CODE(bref.unpack(filt_coef_rsrq_present, 1));

  if (filt_coef_rsrp_present) {
    HANDLE_CODE(filt_coef_rsrp.unpack(bref));
  }
  if (filt_coef_rsrq_present) {
    HANDLE_CODE(filt_coef_rsrq.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void quant_cfg_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (filt_coef_rsrp_present) {
    j.write_str("filterCoefficientRSRP", filt_coef_rsrp.to_string());
  }
  if (filt_coef_rsrq_present) {
    j.write_str("filterCoefficientRSRQ", filt_coef_rsrq.to_string());
  }
  j.end_obj();
}
bool quant_cfg_eutra_s::operator==(const quant_cfg_eutra_s& other) const
{
  return filt_coef_rsrp_present == other.filt_coef_rsrp_present and
         (not filt_coef_rsrp_present or filt_coef_rsrp == other.filt_coef_rsrp) and
         filt_coef_rsrq_present == other.filt_coef_rsrq_present and
         (not filt_coef_rsrq_present or filt_coef_rsrq == other.filt_coef_rsrq);
}

// QuantityConfigEUTRA-v1250 ::= SEQUENCE
SRSASN_CODE quant_cfg_eutra_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(filt_coef_csi_rsrp_r12_present, 1));

  if (filt_coef_csi_rsrp_r12_present) {
    HANDLE_CODE(filt_coef_csi_rsrp_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_eutra_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(filt_coef_csi_rsrp_r12_present, 1));

  if (filt_coef_csi_rsrp_r12_present) {
    HANDLE_CODE(filt_coef_csi_rsrp_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void quant_cfg_eutra_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (filt_coef_csi_rsrp_r12_present) {
    j.write_str("filterCoefficientCSI-RSRP-r12", filt_coef_csi_rsrp_r12.to_string());
  }
  j.end_obj();
}
bool quant_cfg_eutra_v1250_s::operator==(const quant_cfg_eutra_v1250_s& other) const
{
  return filt_coef_csi_rsrp_r12_present == other.filt_coef_csi_rsrp_r12_present and
         (not filt_coef_csi_rsrp_r12_present or filt_coef_csi_rsrp_r12 == other.filt_coef_csi_rsrp_r12);
}

// QuantityConfigEUTRA-v1310 ::= SEQUENCE
SRSASN_CODE quant_cfg_eutra_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(filt_coef_rs_sinr_r13_present, 1));

  if (filt_coef_rs_sinr_r13_present) {
    HANDLE_CODE(filt_coef_rs_sinr_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_eutra_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(filt_coef_rs_sinr_r13_present, 1));

  if (filt_coef_rs_sinr_r13_present) {
    HANDLE_CODE(filt_coef_rs_sinr_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void quant_cfg_eutra_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (filt_coef_rs_sinr_r13_present) {
    j.write_str("filterCoefficientRS-SINR-r13", filt_coef_rs_sinr_r13.to_string());
  }
  j.end_obj();
}
bool quant_cfg_eutra_v1310_s::operator==(const quant_cfg_eutra_v1310_s& other) const
{
  return filt_coef_rs_sinr_r13_present == other.filt_coef_rs_sinr_r13_present and
         (not filt_coef_rs_sinr_r13_present or filt_coef_rs_sinr_r13 == other.filt_coef_rs_sinr_r13);
}

// QuantityConfigGERAN ::= SEQUENCE
SRSASN_CODE quant_cfg_geran_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(filt_coef_present, 1));

  if (filt_coef_present) {
    HANDLE_CODE(filt_coef.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_geran_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(filt_coef_present, 1));

  if (filt_coef_present) {
    HANDLE_CODE(filt_coef.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void quant_cfg_geran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("measQuantityGERAN", "rssi");
  if (filt_coef_present) {
    j.write_str("filterCoefficient", filt_coef.to_string());
  }
  j.end_obj();
}
bool quant_cfg_geran_s::operator==(const quant_cfg_geran_s& other) const
{
  return filt_coef_present == other.filt_coef_present and (not filt_coef_present or filt_coef == other.filt_coef);
}

// QuantityConfigUTRA ::= SEQUENCE
SRSASN_CODE quant_cfg_utra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(filt_coef_present, 1));

  HANDLE_CODE(meas_quant_utra_fdd.pack(bref));
  if (filt_coef_present) {
    HANDLE_CODE(filt_coef.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_utra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(filt_coef_present, 1));

  HANDLE_CODE(meas_quant_utra_fdd.unpack(bref));
  if (filt_coef_present) {
    HANDLE_CODE(filt_coef.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void quant_cfg_utra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("measQuantityUTRA-FDD", meas_quant_utra_fdd.to_string());
  j.write_str("measQuantityUTRA-TDD", "pccpch-RSCP");
  if (filt_coef_present) {
    j.write_str("filterCoefficient", filt_coef.to_string());
  }
  j.end_obj();
}
bool quant_cfg_utra_s::operator==(const quant_cfg_utra_s& other) const
{
  return meas_quant_utra_fdd == other.meas_quant_utra_fdd and filt_coef_present == other.filt_coef_present and
         (not filt_coef_present or filt_coef == other.filt_coef);
}

const char* quant_cfg_utra_s::meas_quant_utra_fdd_opts::to_string() const
{
  static const char* options[] = {"cpich-RSCP", "cpich-EcN0"};
  return convert_enum_idx(options, 2, value, "quant_cfg_utra_s::meas_quant_utra_fdd_e_");
}
uint8_t quant_cfg_utra_s::meas_quant_utra_fdd_opts::to_number() const
{
  if (value == cpich_ec_n0) {
    return 0;
  }
  invalid_enum_number(value, "quant_cfg_utra_s::meas_quant_utra_fdd_e_");
  return 0;
}

// QuantityConfigUTRA-v1020 ::= SEQUENCE
SRSASN_CODE quant_cfg_utra_v1020_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(filt_coef2_fdd_r10_present, 1));

  if (filt_coef2_fdd_r10_present) {
    HANDLE_CODE(filt_coef2_fdd_r10.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_utra_v1020_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(filt_coef2_fdd_r10_present, 1));

  if (filt_coef2_fdd_r10_present) {
    HANDLE_CODE(filt_coef2_fdd_r10.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void quant_cfg_utra_v1020_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (filt_coef2_fdd_r10_present) {
    j.write_str("filterCoefficient2-FDD-r10", filt_coef2_fdd_r10.to_string());
  }
  j.end_obj();
}
bool quant_cfg_utra_v1020_s::operator==(const quant_cfg_utra_v1020_s& other) const
{
  return filt_coef2_fdd_r10_present == other.filt_coef2_fdd_r10_present and
         (not filt_coef2_fdd_r10_present or filt_coef2_fdd_r10 == other.filt_coef2_fdd_r10);
}

// QuantityConfigWLAN-r13 ::= SEQUENCE
SRSASN_CODE quant_cfg_wlan_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(filt_coef_r13_present, 1));

  if (filt_coef_r13_present) {
    HANDLE_CODE(filt_coef_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_wlan_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(filt_coef_r13_present, 1));

  if (filt_coef_r13_present) {
    HANDLE_CODE(filt_coef_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void quant_cfg_wlan_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("measQuantityWLAN-r13", "rssiWLAN");
  if (filt_coef_r13_present) {
    j.write_str("filterCoefficient-r13", filt_coef_r13.to_string());
  }
  j.end_obj();
}
bool quant_cfg_wlan_r13_s::operator==(const quant_cfg_wlan_r13_s& other) const
{
  return filt_coef_r13_present == other.filt_coef_r13_present and
         (not filt_coef_r13_present or filt_coef_r13 == other.filt_coef_r13);
}

// ReportConfigToAddMod ::= SEQUENCE
SRSASN_CODE report_cfg_to_add_mod_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, report_cfg_id, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(report_cfg.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_to_add_mod_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(report_cfg_id, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(report_cfg.unpack(bref));

  return SRSASN_SUCCESS;
}
void report_cfg_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("reportConfigId", report_cfg_id);
  j.write_fieldname("reportConfig");
  report_cfg.to_json(j);
  j.end_obj();
}
bool report_cfg_to_add_mod_s::operator==(const report_cfg_to_add_mod_s& other) const
{
  return report_cfg_id == other.report_cfg_id and report_cfg == other.report_cfg;
}

void report_cfg_to_add_mod_s::report_cfg_c_::destroy_()
{
  switch (type_) {
    case types::report_cfg_eutra:
      c.destroy<report_cfg_eutra_s>();
      break;
    case types::report_cfg_inter_rat:
      c.destroy<report_cfg_inter_rat_s>();
      break;
    default:
      break;
  }
}
void report_cfg_to_add_mod_s::report_cfg_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::report_cfg_eutra:
      c.init<report_cfg_eutra_s>();
      break;
    case types::report_cfg_inter_rat:
      c.init<report_cfg_inter_rat_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_to_add_mod_s::report_cfg_c_");
  }
}
report_cfg_to_add_mod_s::report_cfg_c_::report_cfg_c_(const report_cfg_to_add_mod_s::report_cfg_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::report_cfg_eutra:
      c.init(other.c.get<report_cfg_eutra_s>());
      break;
    case types::report_cfg_inter_rat:
      c.init(other.c.get<report_cfg_inter_rat_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_to_add_mod_s::report_cfg_c_");
  }
}
report_cfg_to_add_mod_s::report_cfg_c_&
report_cfg_to_add_mod_s::report_cfg_c_::operator=(const report_cfg_to_add_mod_s::report_cfg_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::report_cfg_eutra:
      c.set(other.c.get<report_cfg_eutra_s>());
      break;
    case types::report_cfg_inter_rat:
      c.set(other.c.get<report_cfg_inter_rat_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_to_add_mod_s::report_cfg_c_");
  }

  return *this;
}
report_cfg_eutra_s& report_cfg_to_add_mod_s::report_cfg_c_::set_report_cfg_eutra()
{
  set(types::report_cfg_eutra);
  return c.get<report_cfg_eutra_s>();
}
report_cfg_inter_rat_s& report_cfg_to_add_mod_s::report_cfg_c_::set_report_cfg_inter_rat()
{
  set(types::report_cfg_inter_rat);
  return c.get<report_cfg_inter_rat_s>();
}
void report_cfg_to_add_mod_s::report_cfg_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::report_cfg_eutra:
      j.write_fieldname("reportConfigEUTRA");
      c.get<report_cfg_eutra_s>().to_json(j);
      break;
    case types::report_cfg_inter_rat:
      j.write_fieldname("reportConfigInterRAT");
      c.get<report_cfg_inter_rat_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_to_add_mod_s::report_cfg_c_");
  }
  j.end_obj();
}
SRSASN_CODE report_cfg_to_add_mod_s::report_cfg_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::report_cfg_eutra:
      HANDLE_CODE(c.get<report_cfg_eutra_s>().pack(bref));
      break;
    case types::report_cfg_inter_rat:
      HANDLE_CODE(c.get<report_cfg_inter_rat_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_to_add_mod_s::report_cfg_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_to_add_mod_s::report_cfg_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::report_cfg_eutra:
      HANDLE_CODE(c.get<report_cfg_eutra_s>().unpack(bref));
      break;
    case types::report_cfg_inter_rat:
      HANDLE_CODE(c.get<report_cfg_inter_rat_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_to_add_mod_s::report_cfg_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool report_cfg_to_add_mod_s::report_cfg_c_::operator==(const report_cfg_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::report_cfg_eutra:
      return c.get<report_cfg_eutra_s>() == other.c.get<report_cfg_eutra_s>();
    case types::report_cfg_inter_rat:
      return c.get<report_cfg_inter_rat_s>() == other.c.get<report_cfg_inter_rat_s>();
    default:
      return true;
  }
  return true;
}

// MeasGapConfigDensePRS-r15 ::= CHOICE
void meas_gap_cfg_dense_prs_r15_c::set(types::options e)
{
  type_ = e;
}
void meas_gap_cfg_dense_prs_r15_c::set_release()
{
  set(types::release);
}
meas_gap_cfg_dense_prs_r15_c::setup_s_& meas_gap_cfg_dense_prs_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void meas_gap_cfg_dense_prs_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("gapOffsetDensePRS-r15");
      c.gap_offset_dense_prs_r15.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_dense_prs_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_gap_cfg_dense_prs_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.gap_offset_dense_prs_r15.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_dense_prs_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_gap_cfg_dense_prs_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.gap_offset_dense_prs_r15.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_dense_prs_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::destroy_() {}
void meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::gap_offset_dense_prs_r15_c_(
    const meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rstd0_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rstd1_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rstd2_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd3_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd4_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd5_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rstd6_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd7_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd8_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd9_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd10_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd11_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd12_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd13_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd14_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd15_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd16_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd17_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd18_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd19_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::rstd20_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_");
  }
}
meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_&
meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::operator=(
    const meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rstd0_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rstd1_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rstd2_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd3_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd4_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd5_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rstd6_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd7_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd8_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd9_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd10_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd11_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd12_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd13_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd14_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd15_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd16_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd17_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd18_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd19_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::rstd20_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_");
  }

  return *this;
}
uint8_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd0_r15()
{
  set(types::rstd0_r15);
  return c.get<uint8_t>();
}
uint8_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd1_r15()
{
  set(types::rstd1_r15);
  return c.get<uint8_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd2_r15()
{
  set(types::rstd2_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd3_r15()
{
  set(types::rstd3_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd4_r15()
{
  set(types::rstd4_r15);
  return c.get<uint16_t>();
}
uint8_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd5_r15()
{
  set(types::rstd5_r15);
  return c.get<uint8_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd6_r15()
{
  set(types::rstd6_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd7_r15()
{
  set(types::rstd7_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd8_r15()
{
  set(types::rstd8_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd9_r15()
{
  set(types::rstd9_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd10_r15()
{
  set(types::rstd10_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd11_r15()
{
  set(types::rstd11_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd12_r15()
{
  set(types::rstd12_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd13_r15()
{
  set(types::rstd13_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd14_r15()
{
  set(types::rstd14_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd15_r15()
{
  set(types::rstd15_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd16_r15()
{
  set(types::rstd16_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd17_r15()
{
  set(types::rstd17_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd18_r15()
{
  set(types::rstd18_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd19_r15()
{
  set(types::rstd19_r15);
  return c.get<uint16_t>();
}
uint16_t& meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::set_rstd20_r15()
{
  set(types::rstd20_r15);
  return c.get<uint16_t>();
}
void meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rstd0_r15:
      j.write_int("rstd0-r15", c.get<uint8_t>());
      break;
    case types::rstd1_r15:
      j.write_int("rstd1-r15", c.get<uint8_t>());
      break;
    case types::rstd2_r15:
      j.write_int("rstd2-r15", c.get<uint16_t>());
      break;
    case types::rstd3_r15:
      j.write_int("rstd3-r15", c.get<uint16_t>());
      break;
    case types::rstd4_r15:
      j.write_int("rstd4-r15", c.get<uint16_t>());
      break;
    case types::rstd5_r15:
      j.write_int("rstd5-r15", c.get<uint8_t>());
      break;
    case types::rstd6_r15:
      j.write_int("rstd6-r15", c.get<uint16_t>());
      break;
    case types::rstd7_r15:
      j.write_int("rstd7-r15", c.get<uint16_t>());
      break;
    case types::rstd8_r15:
      j.write_int("rstd8-r15", c.get<uint16_t>());
      break;
    case types::rstd9_r15:
      j.write_int("rstd9-r15", c.get<uint16_t>());
      break;
    case types::rstd10_r15:
      j.write_int("rstd10-r15", c.get<uint16_t>());
      break;
    case types::rstd11_r15:
      j.write_int("rstd11-r15", c.get<uint16_t>());
      break;
    case types::rstd12_r15:
      j.write_int("rstd12-r15", c.get<uint16_t>());
      break;
    case types::rstd13_r15:
      j.write_int("rstd13-r15", c.get<uint16_t>());
      break;
    case types::rstd14_r15:
      j.write_int("rstd14-r15", c.get<uint16_t>());
      break;
    case types::rstd15_r15:
      j.write_int("rstd15-r15", c.get<uint16_t>());
      break;
    case types::rstd16_r15:
      j.write_int("rstd16-r15", c.get<uint16_t>());
      break;
    case types::rstd17_r15:
      j.write_int("rstd17-r15", c.get<uint16_t>());
      break;
    case types::rstd18_r15:
      j.write_int("rstd18-r15", c.get<uint16_t>());
      break;
    case types::rstd19_r15:
      j.write_int("rstd19-r15", c.get<uint16_t>());
      break;
    case types::rstd20_r15:
      j.write_int("rstd20-r15", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rstd0_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::rstd1_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::rstd2_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::rstd3_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::rstd4_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::rstd5_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::rstd6_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::rstd7_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::rstd8_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::rstd9_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::rstd10_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::rstd11_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::rstd12_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::rstd13_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::rstd14_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::rstd15_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::rstd16_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::rstd17_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::rstd18_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::rstd19_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::rstd20_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rstd0_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::rstd1_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::rstd2_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::rstd3_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::rstd4_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::rstd5_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::rstd6_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::rstd7_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::rstd8_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::rstd9_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::rstd10_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::rstd11_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::rstd12_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::rstd13_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::rstd14_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::rstd15_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::rstd16_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::rstd17_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::rstd18_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::rstd19_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::rstd20_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MeasGapConfigPerCC-List-r14 ::= CHOICE
void meas_gap_cfg_per_cc_list_r14_c::set(types::options e)
{
  type_ = e;
}
void meas_gap_cfg_per_cc_list_r14_c::set_release()
{
  set(types::release);
}
meas_gap_cfg_per_cc_list_r14_c::setup_s_& meas_gap_cfg_per_cc_list_r14_c::set_setup()
{
  set(types::setup);
  return c;
}
void meas_gap_cfg_per_cc_list_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.meas_gap_cfg_to_rem_list_r14_present) {
        j.start_array("measGapConfigToRemoveList-r14");
        for (const auto& e1 : c.meas_gap_cfg_to_rem_list_r14) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.meas_gap_cfg_to_add_mod_list_r14_present) {
        j.start_array("measGapConfigToAddModList-r14");
        for (const auto& e1 : c.meas_gap_cfg_to_add_mod_list_r14) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_per_cc_list_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_gap_cfg_per_cc_list_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.meas_gap_cfg_to_rem_list_r14_present, 1));
      HANDLE_CODE(bref.pack(c.meas_gap_cfg_to_add_mod_list_r14_present, 1));
      if (c.meas_gap_cfg_to_rem_list_r14_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.meas_gap_cfg_to_rem_list_r14, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (c.meas_gap_cfg_to_add_mod_list_r14_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.meas_gap_cfg_to_add_mod_list_r14, 1, 32));
      }
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_per_cc_list_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_gap_cfg_per_cc_list_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.meas_gap_cfg_to_rem_list_r14_present, 1));
      HANDLE_CODE(bref.unpack(c.meas_gap_cfg_to_add_mod_list_r14_present, 1));
      if (c.meas_gap_cfg_to_rem_list_r14_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.meas_gap_cfg_to_rem_list_r14, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (c.meas_gap_cfg_to_add_mod_list_r14_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.meas_gap_cfg_to_add_mod_list_r14, bref, 1, 32));
      }
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_cfg_per_cc_list_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MeasGapSharingConfig-r14 ::= CHOICE
void meas_gap_sharing_cfg_r14_c::set(types::options e)
{
  type_ = e;
}
void meas_gap_sharing_cfg_r14_c::set_release()
{
  set(types::release);
}
meas_gap_sharing_cfg_r14_c::setup_s_& meas_gap_sharing_cfg_r14_c::set_setup()
{
  set(types::setup);
  return c;
}
void meas_gap_sharing_cfg_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("measGapSharingScheme-r14", c.meas_gap_sharing_scheme_r14.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_sharing_cfg_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_gap_sharing_cfg_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.meas_gap_sharing_scheme_r14.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_sharing_cfg_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_gap_sharing_cfg_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.meas_gap_sharing_scheme_r14.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_gap_sharing_cfg_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_opts::to_string() const
{
  static const char* options[] = {"scheme00", "scheme01", "scheme10", "scheme11"};
  return convert_enum_idx(options, 4, value, "meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_e_");
}
float meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.1, 1.0, 1.1};
  return map_enum_number(options, 4, value, "meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_e_");
}
const char* meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_opts::to_number_string() const
{
  static const char* options[] = {"0.0", "0.1", "1.0", "1.1"};
  return convert_enum_idx(options, 4, value, "meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_e_");
}

// MeasScaleFactor-r12 ::= ENUMERATED
const char* meas_scale_factor_r12_opts::to_string() const
{
  static const char* options[] = {"sf-EUTRA-cf1", "sf-EUTRA-cf2"};
  return convert_enum_idx(options, 2, value, "meas_scale_factor_r12_e");
}
uint8_t meas_scale_factor_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "meas_scale_factor_r12_e");
}

// QuantityConfig ::= SEQUENCE
SRSASN_CODE quant_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(quant_cfg_eutra_present, 1));
  HANDLE_CODE(bref.pack(quant_cfg_utra_present, 1));
  HANDLE_CODE(bref.pack(quant_cfg_geran_present, 1));
  HANDLE_CODE(bref.pack(quant_cfg_cdma2000_present, 1));

  if (quant_cfg_eutra_present) {
    HANDLE_CODE(quant_cfg_eutra.pack(bref));
  }
  if (quant_cfg_utra_present) {
    HANDLE_CODE(quant_cfg_utra.pack(bref));
  }
  if (quant_cfg_geran_present) {
    HANDLE_CODE(quant_cfg_geran.pack(bref));
  }
  if (quant_cfg_cdma2000_present) {
    HANDLE_CODE(quant_cfg_cdma2000.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= quant_cfg_utra_v1020.is_present();
    group_flags[1] |= quant_cfg_eutra_v1250.is_present();
    group_flags[2] |= quant_cfg_eutra_v1310.is_present();
    group_flags[2] |= quant_cfg_wlan_r13.is_present();
    group_flags[3] |= quant_cfg_nr_list_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(quant_cfg_utra_v1020.is_present(), 1));
      if (quant_cfg_utra_v1020.is_present()) {
        HANDLE_CODE(quant_cfg_utra_v1020->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(quant_cfg_eutra_v1250.is_present(), 1));
      if (quant_cfg_eutra_v1250.is_present()) {
        HANDLE_CODE(quant_cfg_eutra_v1250->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(quant_cfg_eutra_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(quant_cfg_wlan_r13.is_present(), 1));
      if (quant_cfg_eutra_v1310.is_present()) {
        HANDLE_CODE(quant_cfg_eutra_v1310->pack(bref));
      }
      if (quant_cfg_wlan_r13.is_present()) {
        HANDLE_CODE(quant_cfg_wlan_r13->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(quant_cfg_nr_list_r15.is_present(), 1));
      if (quant_cfg_nr_list_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *quant_cfg_nr_list_r15, 1, 2));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(quant_cfg_eutra_present, 1));
  HANDLE_CODE(bref.unpack(quant_cfg_utra_present, 1));
  HANDLE_CODE(bref.unpack(quant_cfg_geran_present, 1));
  HANDLE_CODE(bref.unpack(quant_cfg_cdma2000_present, 1));

  if (quant_cfg_eutra_present) {
    HANDLE_CODE(quant_cfg_eutra.unpack(bref));
  }
  if (quant_cfg_utra_present) {
    HANDLE_CODE(quant_cfg_utra.unpack(bref));
  }
  if (quant_cfg_geran_present) {
    HANDLE_CODE(quant_cfg_geran.unpack(bref));
  }
  if (quant_cfg_cdma2000_present) {
    HANDLE_CODE(quant_cfg_cdma2000.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool quant_cfg_utra_v1020_present;
      HANDLE_CODE(bref.unpack(quant_cfg_utra_v1020_present, 1));
      quant_cfg_utra_v1020.set_present(quant_cfg_utra_v1020_present);
      if (quant_cfg_utra_v1020.is_present()) {
        HANDLE_CODE(quant_cfg_utra_v1020->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool quant_cfg_eutra_v1250_present;
      HANDLE_CODE(bref.unpack(quant_cfg_eutra_v1250_present, 1));
      quant_cfg_eutra_v1250.set_present(quant_cfg_eutra_v1250_present);
      if (quant_cfg_eutra_v1250.is_present()) {
        HANDLE_CODE(quant_cfg_eutra_v1250->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool quant_cfg_eutra_v1310_present;
      HANDLE_CODE(bref.unpack(quant_cfg_eutra_v1310_present, 1));
      quant_cfg_eutra_v1310.set_present(quant_cfg_eutra_v1310_present);
      bool quant_cfg_wlan_r13_present;
      HANDLE_CODE(bref.unpack(quant_cfg_wlan_r13_present, 1));
      quant_cfg_wlan_r13.set_present(quant_cfg_wlan_r13_present);
      if (quant_cfg_eutra_v1310.is_present()) {
        HANDLE_CODE(quant_cfg_eutra_v1310->unpack(bref));
      }
      if (quant_cfg_wlan_r13.is_present()) {
        HANDLE_CODE(quant_cfg_wlan_r13->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool quant_cfg_nr_list_r15_present;
      HANDLE_CODE(bref.unpack(quant_cfg_nr_list_r15_present, 1));
      quant_cfg_nr_list_r15.set_present(quant_cfg_nr_list_r15_present);
      if (quant_cfg_nr_list_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*quant_cfg_nr_list_r15, bref, 1, 2));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void quant_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (quant_cfg_eutra_present) {
    j.write_fieldname("quantityConfigEUTRA");
    quant_cfg_eutra.to_json(j);
  }
  if (quant_cfg_utra_present) {
    j.write_fieldname("quantityConfigUTRA");
    quant_cfg_utra.to_json(j);
  }
  if (quant_cfg_geran_present) {
    j.write_fieldname("quantityConfigGERAN");
    quant_cfg_geran.to_json(j);
  }
  if (quant_cfg_cdma2000_present) {
    j.write_fieldname("quantityConfigCDMA2000");
    quant_cfg_cdma2000.to_json(j);
  }
  if (ext) {
    if (quant_cfg_utra_v1020.is_present()) {
      j.write_fieldname("quantityConfigUTRA-v1020");
      quant_cfg_utra_v1020->to_json(j);
    }
    if (quant_cfg_eutra_v1250.is_present()) {
      j.write_fieldname("quantityConfigEUTRA-v1250");
      quant_cfg_eutra_v1250->to_json(j);
    }
    if (quant_cfg_eutra_v1310.is_present()) {
      j.write_fieldname("quantityConfigEUTRA-v1310");
      quant_cfg_eutra_v1310->to_json(j);
    }
    if (quant_cfg_wlan_r13.is_present()) {
      j.write_fieldname("quantityConfigWLAN-r13");
      quant_cfg_wlan_r13->to_json(j);
    }
    if (quant_cfg_nr_list_r15.is_present()) {
      j.start_array("quantityConfigNRList-r15");
      for (const auto& e1 : *quant_cfg_nr_list_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}
bool quant_cfg_s::operator==(const quant_cfg_s& other) const
{
  return ext == other.ext and quant_cfg_eutra_present == other.quant_cfg_eutra_present and
         (not quant_cfg_eutra_present or quant_cfg_eutra == other.quant_cfg_eutra) and
         quant_cfg_utra_present == other.quant_cfg_utra_present and
         (not quant_cfg_utra_present or quant_cfg_utra == other.quant_cfg_utra) and
         quant_cfg_geran_present == other.quant_cfg_geran_present and
         (not quant_cfg_geran_present or quant_cfg_geran == other.quant_cfg_geran) and
         quant_cfg_cdma2000_present == other.quant_cfg_cdma2000_present and
         (not quant_cfg_cdma2000_present or quant_cfg_cdma2000 == other.quant_cfg_cdma2000) and
         (not ext or
          (quant_cfg_utra_v1020.is_present() == other.quant_cfg_utra_v1020.is_present() and
           (not quant_cfg_utra_v1020.is_present() or *quant_cfg_utra_v1020 == *other.quant_cfg_utra_v1020) and
           quant_cfg_eutra_v1250.is_present() == other.quant_cfg_eutra_v1250.is_present() and
           (not quant_cfg_eutra_v1250.is_present() or *quant_cfg_eutra_v1250 == *other.quant_cfg_eutra_v1250) and
           quant_cfg_eutra_v1310.is_present() == other.quant_cfg_eutra_v1310.is_present() and
           (not quant_cfg_eutra_v1310.is_present() or *quant_cfg_eutra_v1310 == *other.quant_cfg_eutra_v1310) and
           quant_cfg_wlan_r13.is_present() == other.quant_cfg_wlan_r13.is_present() and
           (not quant_cfg_wlan_r13.is_present() or *quant_cfg_wlan_r13 == *other.quant_cfg_wlan_r13) and
           quant_cfg_nr_list_r15.is_present() == other.quant_cfg_nr_list_r15.is_present() and
           (not quant_cfg_nr_list_r15.is_present() or *quant_cfg_nr_list_r15 == *other.quant_cfg_nr_list_r15)));
}

// RACH-ConfigDedicated ::= SEQUENCE
SRSASN_CODE rach_cfg_ded_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ra_preamb_idx, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, ra_prach_mask_idx, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_cfg_ded_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ra_preamb_idx, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(ra_prach_mask_idx, bref, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
void rach_cfg_ded_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ra-PreambleIndex", ra_preamb_idx);
  j.write_int("ra-PRACH-MaskIndex", ra_prach_mask_idx);
  j.end_obj();
}

// RACH-Skip-r14 ::= SEQUENCE
SRSASN_CODE rach_skip_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_cfg_info_r14_present, 1));

  HANDLE_CODE(target_ta_r14.pack(bref));
  if (ul_cfg_info_r14_present) {
    HANDLE_CODE(pack_integer(bref, ul_cfg_info_r14.nof_conf_ul_processes_r14, (uint8_t)1u, (uint8_t)8u));
    HANDLE_CODE(ul_cfg_info_r14.ul_sched_interv_r14.pack(bref));
    HANDLE_CODE(pack_integer(bref, ul_cfg_info_r14.ul_start_sf_r14, (uint8_t)0u, (uint8_t)9u));
    HANDLE_CODE(ul_cfg_info_r14.ul_grant_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_skip_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_cfg_info_r14_present, 1));

  HANDLE_CODE(target_ta_r14.unpack(bref));
  if (ul_cfg_info_r14_present) {
    HANDLE_CODE(unpack_integer(ul_cfg_info_r14.nof_conf_ul_processes_r14, bref, (uint8_t)1u, (uint8_t)8u));
    HANDLE_CODE(ul_cfg_info_r14.ul_sched_interv_r14.unpack(bref));
    HANDLE_CODE(unpack_integer(ul_cfg_info_r14.ul_start_sf_r14, bref, (uint8_t)0u, (uint8_t)9u));
    HANDLE_CODE(ul_cfg_info_r14.ul_grant_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rach_skip_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("targetTA-r14");
  target_ta_r14.to_json(j);
  if (ul_cfg_info_r14_present) {
    j.write_fieldname("ul-ConfigInfo-r14");
    j.start_obj();
    j.write_int("numberOfConfUL-Processes-r14", ul_cfg_info_r14.nof_conf_ul_processes_r14);
    j.write_str("ul-SchedInterval-r14", ul_cfg_info_r14.ul_sched_interv_r14.to_string());
    j.write_int("ul-StartSubframe-r14", ul_cfg_info_r14.ul_start_sf_r14);
    j.write_str("ul-Grant-r14", ul_cfg_info_r14.ul_grant_r14.to_string());
    j.end_obj();
  }
  j.end_obj();
}

void rach_skip_r14_s::target_ta_r14_c_::destroy_() {}
void rach_skip_r14_s::target_ta_r14_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
rach_skip_r14_s::target_ta_r14_c_::target_ta_r14_c_(const rach_skip_r14_s::target_ta_r14_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ta0_r14:
      break;
    case types::mcg_ptag_r14:
      break;
    case types::scg_ptag_r14:
      break;
    case types::mcg_stag_r14:
      c.init(other.c.get<uint8_t>());
      break;
    case types::scg_stag_r14:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rach_skip_r14_s::target_ta_r14_c_");
  }
}
rach_skip_r14_s::target_ta_r14_c_&
rach_skip_r14_s::target_ta_r14_c_::operator=(const rach_skip_r14_s::target_ta_r14_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ta0_r14:
      break;
    case types::mcg_ptag_r14:
      break;
    case types::scg_ptag_r14:
      break;
    case types::mcg_stag_r14:
      c.set(other.c.get<uint8_t>());
      break;
    case types::scg_stag_r14:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rach_skip_r14_s::target_ta_r14_c_");
  }

  return *this;
}
void rach_skip_r14_s::target_ta_r14_c_::set_ta0_r14()
{
  set(types::ta0_r14);
}
void rach_skip_r14_s::target_ta_r14_c_::set_mcg_ptag_r14()
{
  set(types::mcg_ptag_r14);
}
void rach_skip_r14_s::target_ta_r14_c_::set_scg_ptag_r14()
{
  set(types::scg_ptag_r14);
}
uint8_t& rach_skip_r14_s::target_ta_r14_c_::set_mcg_stag_r14()
{
  set(types::mcg_stag_r14);
  return c.get<uint8_t>();
}
uint8_t& rach_skip_r14_s::target_ta_r14_c_::set_scg_stag_r14()
{
  set(types::scg_stag_r14);
  return c.get<uint8_t>();
}
void rach_skip_r14_s::target_ta_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ta0_r14:
      break;
    case types::mcg_ptag_r14:
      break;
    case types::scg_ptag_r14:
      break;
    case types::mcg_stag_r14:
      j.write_int("mcg-STAG-r14", c.get<uint8_t>());
      break;
    case types::scg_stag_r14:
      j.write_int("scg-STAG-r14", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "rach_skip_r14_s::target_ta_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE rach_skip_r14_s::target_ta_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ta0_r14:
      break;
    case types::mcg_ptag_r14:
      break;
    case types::scg_ptag_r14:
      break;
    case types::mcg_stag_r14:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)3u));
      break;
    case types::scg_stag_r14:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)3u));
      break;
    default:
      log_invalid_choice_id(type_, "rach_skip_r14_s::target_ta_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_skip_r14_s::target_ta_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ta0_r14:
      break;
    case types::mcg_ptag_r14:
      break;
    case types::scg_ptag_r14:
      break;
    case types::mcg_stag_r14:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)3u));
      break;
    case types::scg_stag_r14:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)3u));
      break;
    default:
      log_invalid_choice_id(type_, "rach_skip_r14_s::target_ta_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rach_skip_r14_s::ul_cfg_info_r14_s_::ul_sched_interv_r14_opts::to_string() const
{
  static const char* options[] = {"sf2", "sf5", "sf10"};
  return convert_enum_idx(options, 3, value, "rach_skip_r14_s::ul_cfg_info_r14_s_::ul_sched_interv_r14_e_");
}
uint8_t rach_skip_r14_s::ul_cfg_info_r14_s_::ul_sched_interv_r14_opts::to_number() const
{
  static const uint8_t options[] = {2, 5, 10};
  return map_enum_number(options, 3, value, "rach_skip_r14_s::ul_cfg_info_r14_s_::ul_sched_interv_r14_e_");
}

// MeasConfig ::= SEQUENCE
SRSASN_CODE meas_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_obj_to_rem_list_present, 1));
  HANDLE_CODE(bref.pack(meas_obj_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.pack(report_cfg_to_rem_list_present, 1));
  HANDLE_CODE(bref.pack(report_cfg_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.pack(meas_id_to_rem_list_present, 1));
  HANDLE_CODE(bref.pack(meas_id_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.pack(quant_cfg_present, 1));
  HANDLE_CODE(bref.pack(meas_gap_cfg_present, 1));
  HANDLE_CODE(bref.pack(s_measure_present, 1));
  HANDLE_CODE(bref.pack(pre_regist_info_hrpd_present, 1));
  HANDLE_CODE(bref.pack(speed_state_pars_present, 1));

  if (meas_obj_to_rem_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_obj_to_rem_list, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (meas_obj_to_add_mod_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_obj_to_add_mod_list, 1, 32));
  }
  if (report_cfg_to_rem_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, report_cfg_to_rem_list, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (report_cfg_to_add_mod_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, report_cfg_to_add_mod_list, 1, 32));
  }
  if (meas_id_to_rem_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_id_to_rem_list, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (meas_id_to_add_mod_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_id_to_add_mod_list, 1, 32));
  }
  if (quant_cfg_present) {
    HANDLE_CODE(quant_cfg.pack(bref));
  }
  if (meas_gap_cfg_present) {
    HANDLE_CODE(meas_gap_cfg.pack(bref));
  }
  if (s_measure_present) {
    HANDLE_CODE(pack_integer(bref, s_measure, (uint8_t)0u, (uint8_t)97u));
  }
  if (pre_regist_info_hrpd_present) {
    HANDLE_CODE(pre_regist_info_hrpd.pack(bref));
  }
  if (speed_state_pars_present) {
    HANDLE_CODE(speed_state_pars.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_obj_to_add_mod_list_v9e0.is_present();
    group_flags[1] |= allow_interruptions_r11_present;
    group_flags[2] |= meas_scale_factor_r12.is_present();
    group_flags[2] |= meas_id_to_rem_list_ext_r12.is_present();
    group_flags[2] |= meas_id_to_add_mod_list_ext_r12.is_present();
    group_flags[2] |= meas_rsrq_on_all_symbols_r12_present;
    group_flags[3] |= meas_obj_to_rem_list_ext_r13.is_present();
    group_flags[3] |= meas_obj_to_add_mod_list_ext_r13.is_present();
    group_flags[3] |= meas_id_to_add_mod_list_v1310.is_present();
    group_flags[3] |= meas_id_to_add_mod_list_ext_v1310.is_present();
    group_flags[4] |= meas_gap_cfg_per_cc_list_r14.is_present();
    group_flags[4] |= meas_gap_sharing_cfg_r14.is_present();
    group_flags[5] |= fr1_gap_r15_present;
    group_flags[5] |= mgta_r15_present;
    group_flags[6] |= meas_gap_cfg_dense_prs_r15.is_present();
    group_flags[6] |= height_thresh_ref_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_obj_to_add_mod_list_v9e0.is_present(), 1));
      if (meas_obj_to_add_mod_list_v9e0.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_obj_to_add_mod_list_v9e0, 1, 32));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(allow_interruptions_r11_present, 1));
      if (allow_interruptions_r11_present) {
        HANDLE_CODE(bref.pack(allow_interruptions_r11, 1));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_scale_factor_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_id_to_rem_list_ext_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_id_to_add_mod_list_ext_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_rsrq_on_all_symbols_r12_present, 1));
      if (meas_scale_factor_r12.is_present()) {
        HANDLE_CODE(meas_scale_factor_r12->pack(bref));
      }
      if (meas_id_to_rem_list_ext_r12.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_id_to_rem_list_ext_r12, 1, 32, integer_packer<uint8_t>(33, 64)));
      }
      if (meas_id_to_add_mod_list_ext_r12.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_id_to_add_mod_list_ext_r12, 1, 32));
      }
      if (meas_rsrq_on_all_symbols_r12_present) {
        HANDLE_CODE(bref.pack(meas_rsrq_on_all_symbols_r12, 1));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_obj_to_rem_list_ext_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_obj_to_add_mod_list_ext_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_id_to_add_mod_list_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_id_to_add_mod_list_ext_v1310.is_present(), 1));
      if (meas_obj_to_rem_list_ext_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_obj_to_rem_list_ext_r13, 1, 32, integer_packer<uint8_t>(33, 64)));
      }
      if (meas_obj_to_add_mod_list_ext_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_obj_to_add_mod_list_ext_r13, 1, 32));
      }
      if (meas_id_to_add_mod_list_v1310.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_id_to_add_mod_list_v1310, 1, 32));
      }
      if (meas_id_to_add_mod_list_ext_v1310.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_id_to_add_mod_list_ext_v1310, 1, 32));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_gap_cfg_per_cc_list_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_gap_sharing_cfg_r14.is_present(), 1));
      if (meas_gap_cfg_per_cc_list_r14.is_present()) {
        HANDLE_CODE(meas_gap_cfg_per_cc_list_r14->pack(bref));
      }
      if (meas_gap_sharing_cfg_r14.is_present()) {
        HANDLE_CODE(meas_gap_sharing_cfg_r14->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(fr1_gap_r15_present, 1));
      HANDLE_CODE(bref.pack(mgta_r15_present, 1));
      if (fr1_gap_r15_present) {
        HANDLE_CODE(bref.pack(fr1_gap_r15, 1));
      }
      if (mgta_r15_present) {
        HANDLE_CODE(bref.pack(mgta_r15, 1));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_gap_cfg_dense_prs_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(height_thresh_ref_r15.is_present(), 1));
      if (meas_gap_cfg_dense_prs_r15.is_present()) {
        HANDLE_CODE(meas_gap_cfg_dense_prs_r15->pack(bref));
      }
      if (height_thresh_ref_r15.is_present()) {
        HANDLE_CODE(height_thresh_ref_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_obj_to_rem_list_present, 1));
  HANDLE_CODE(bref.unpack(meas_obj_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(report_cfg_to_rem_list_present, 1));
  HANDLE_CODE(bref.unpack(report_cfg_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(meas_id_to_rem_list_present, 1));
  HANDLE_CODE(bref.unpack(meas_id_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(quant_cfg_present, 1));
  HANDLE_CODE(bref.unpack(meas_gap_cfg_present, 1));
  HANDLE_CODE(bref.unpack(s_measure_present, 1));
  HANDLE_CODE(bref.unpack(pre_regist_info_hrpd_present, 1));
  HANDLE_CODE(bref.unpack(speed_state_pars_present, 1));

  if (meas_obj_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_obj_to_rem_list, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (meas_obj_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_obj_to_add_mod_list, bref, 1, 32));
  }
  if (report_cfg_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(report_cfg_to_rem_list, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (report_cfg_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(report_cfg_to_add_mod_list, bref, 1, 32));
  }
  if (meas_id_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_id_to_rem_list, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (meas_id_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_id_to_add_mod_list, bref, 1, 32));
  }
  if (quant_cfg_present) {
    HANDLE_CODE(quant_cfg.unpack(bref));
  }
  if (meas_gap_cfg_present) {
    HANDLE_CODE(meas_gap_cfg.unpack(bref));
  }
  if (s_measure_present) {
    HANDLE_CODE(unpack_integer(s_measure, bref, (uint8_t)0u, (uint8_t)97u));
  }
  if (pre_regist_info_hrpd_present) {
    HANDLE_CODE(pre_regist_info_hrpd.unpack(bref));
  }
  if (speed_state_pars_present) {
    HANDLE_CODE(speed_state_pars.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(7);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_obj_to_add_mod_list_v9e0_present;
      HANDLE_CODE(bref.unpack(meas_obj_to_add_mod_list_v9e0_present, 1));
      meas_obj_to_add_mod_list_v9e0.set_present(meas_obj_to_add_mod_list_v9e0_present);
      if (meas_obj_to_add_mod_list_v9e0.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_obj_to_add_mod_list_v9e0, bref, 1, 32));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(allow_interruptions_r11_present, 1));
      if (allow_interruptions_r11_present) {
        HANDLE_CODE(bref.unpack(allow_interruptions_r11, 1));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_scale_factor_r12_present;
      HANDLE_CODE(bref.unpack(meas_scale_factor_r12_present, 1));
      meas_scale_factor_r12.set_present(meas_scale_factor_r12_present);
      bool meas_id_to_rem_list_ext_r12_present;
      HANDLE_CODE(bref.unpack(meas_id_to_rem_list_ext_r12_present, 1));
      meas_id_to_rem_list_ext_r12.set_present(meas_id_to_rem_list_ext_r12_present);
      bool meas_id_to_add_mod_list_ext_r12_present;
      HANDLE_CODE(bref.unpack(meas_id_to_add_mod_list_ext_r12_present, 1));
      meas_id_to_add_mod_list_ext_r12.set_present(meas_id_to_add_mod_list_ext_r12_present);
      HANDLE_CODE(bref.unpack(meas_rsrq_on_all_symbols_r12_present, 1));
      if (meas_scale_factor_r12.is_present()) {
        HANDLE_CODE(meas_scale_factor_r12->unpack(bref));
      }
      if (meas_id_to_rem_list_ext_r12.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_id_to_rem_list_ext_r12, bref, 1, 32, integer_packer<uint8_t>(33, 64)));
      }
      if (meas_id_to_add_mod_list_ext_r12.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_id_to_add_mod_list_ext_r12, bref, 1, 32));
      }
      if (meas_rsrq_on_all_symbols_r12_present) {
        HANDLE_CODE(bref.unpack(meas_rsrq_on_all_symbols_r12, 1));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_obj_to_rem_list_ext_r13_present;
      HANDLE_CODE(bref.unpack(meas_obj_to_rem_list_ext_r13_present, 1));
      meas_obj_to_rem_list_ext_r13.set_present(meas_obj_to_rem_list_ext_r13_present);
      bool meas_obj_to_add_mod_list_ext_r13_present;
      HANDLE_CODE(bref.unpack(meas_obj_to_add_mod_list_ext_r13_present, 1));
      meas_obj_to_add_mod_list_ext_r13.set_present(meas_obj_to_add_mod_list_ext_r13_present);
      bool meas_id_to_add_mod_list_v1310_present;
      HANDLE_CODE(bref.unpack(meas_id_to_add_mod_list_v1310_present, 1));
      meas_id_to_add_mod_list_v1310.set_present(meas_id_to_add_mod_list_v1310_present);
      bool meas_id_to_add_mod_list_ext_v1310_present;
      HANDLE_CODE(bref.unpack(meas_id_to_add_mod_list_ext_v1310_present, 1));
      meas_id_to_add_mod_list_ext_v1310.set_present(meas_id_to_add_mod_list_ext_v1310_present);
      if (meas_obj_to_rem_list_ext_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_obj_to_rem_list_ext_r13, bref, 1, 32, integer_packer<uint8_t>(33, 64)));
      }
      if (meas_obj_to_add_mod_list_ext_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_obj_to_add_mod_list_ext_r13, bref, 1, 32));
      }
      if (meas_id_to_add_mod_list_v1310.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_id_to_add_mod_list_v1310, bref, 1, 32));
      }
      if (meas_id_to_add_mod_list_ext_v1310.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_id_to_add_mod_list_ext_v1310, bref, 1, 32));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_gap_cfg_per_cc_list_r14_present;
      HANDLE_CODE(bref.unpack(meas_gap_cfg_per_cc_list_r14_present, 1));
      meas_gap_cfg_per_cc_list_r14.set_present(meas_gap_cfg_per_cc_list_r14_present);
      bool meas_gap_sharing_cfg_r14_present;
      HANDLE_CODE(bref.unpack(meas_gap_sharing_cfg_r14_present, 1));
      meas_gap_sharing_cfg_r14.set_present(meas_gap_sharing_cfg_r14_present);
      if (meas_gap_cfg_per_cc_list_r14.is_present()) {
        HANDLE_CODE(meas_gap_cfg_per_cc_list_r14->unpack(bref));
      }
      if (meas_gap_sharing_cfg_r14.is_present()) {
        HANDLE_CODE(meas_gap_sharing_cfg_r14->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(fr1_gap_r15_present, 1));
      HANDLE_CODE(bref.unpack(mgta_r15_present, 1));
      if (fr1_gap_r15_present) {
        HANDLE_CODE(bref.unpack(fr1_gap_r15, 1));
      }
      if (mgta_r15_present) {
        HANDLE_CODE(bref.unpack(mgta_r15, 1));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_gap_cfg_dense_prs_r15_present;
      HANDLE_CODE(bref.unpack(meas_gap_cfg_dense_prs_r15_present, 1));
      meas_gap_cfg_dense_prs_r15.set_present(meas_gap_cfg_dense_prs_r15_present);
      bool height_thresh_ref_r15_present;
      HANDLE_CODE(bref.unpack(height_thresh_ref_r15_present, 1));
      height_thresh_ref_r15.set_present(height_thresh_ref_r15_present);
      if (meas_gap_cfg_dense_prs_r15.is_present()) {
        HANDLE_CODE(meas_gap_cfg_dense_prs_r15->unpack(bref));
      }
      if (height_thresh_ref_r15.is_present()) {
        HANDLE_CODE(height_thresh_ref_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_obj_to_rem_list_present) {
    j.start_array("measObjectToRemoveList");
    for (const auto& e1 : meas_obj_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (meas_obj_to_add_mod_list_present) {
    j.start_array("measObjectToAddModList");
    for (const auto& e1 : meas_obj_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (report_cfg_to_rem_list_present) {
    j.start_array("reportConfigToRemoveList");
    for (const auto& e1 : report_cfg_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (report_cfg_to_add_mod_list_present) {
    j.start_array("reportConfigToAddModList");
    for (const auto& e1 : report_cfg_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_id_to_rem_list_present) {
    j.start_array("measIdToRemoveList");
    for (const auto& e1 : meas_id_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (meas_id_to_add_mod_list_present) {
    j.start_array("measIdToAddModList");
    for (const auto& e1 : meas_id_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (quant_cfg_present) {
    j.write_fieldname("quantityConfig");
    quant_cfg.to_json(j);
  }
  if (meas_gap_cfg_present) {
    j.write_fieldname("measGapConfig");
    meas_gap_cfg.to_json(j);
  }
  if (s_measure_present) {
    j.write_int("s-Measure", s_measure);
  }
  if (pre_regist_info_hrpd_present) {
    j.write_fieldname("preRegistrationInfoHRPD");
    pre_regist_info_hrpd.to_json(j);
  }
  if (speed_state_pars_present) {
    j.write_fieldname("speedStatePars");
    speed_state_pars.to_json(j);
  }
  if (ext) {
    if (meas_obj_to_add_mod_list_v9e0.is_present()) {
      j.start_array("measObjectToAddModList-v9e0");
      for (const auto& e1 : *meas_obj_to_add_mod_list_v9e0) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (allow_interruptions_r11_present) {
      j.write_bool("allowInterruptions-r11", allow_interruptions_r11);
    }
    if (meas_scale_factor_r12.is_present()) {
      j.write_fieldname("measScaleFactor-r12");
      meas_scale_factor_r12->to_json(j);
    }
    if (meas_id_to_rem_list_ext_r12.is_present()) {
      j.start_array("measIdToRemoveListExt-r12");
      for (const auto& e1 : *meas_id_to_rem_list_ext_r12) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (meas_id_to_add_mod_list_ext_r12.is_present()) {
      j.start_array("measIdToAddModListExt-r12");
      for (const auto& e1 : *meas_id_to_add_mod_list_ext_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_rsrq_on_all_symbols_r12_present) {
      j.write_bool("measRSRQ-OnAllSymbols-r12", meas_rsrq_on_all_symbols_r12);
    }
    if (meas_obj_to_rem_list_ext_r13.is_present()) {
      j.start_array("measObjectToRemoveListExt-r13");
      for (const auto& e1 : *meas_obj_to_rem_list_ext_r13) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (meas_obj_to_add_mod_list_ext_r13.is_present()) {
      j.start_array("measObjectToAddModListExt-r13");
      for (const auto& e1 : *meas_obj_to_add_mod_list_ext_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_id_to_add_mod_list_v1310.is_present()) {
      j.start_array("measIdToAddModList-v1310");
      for (const auto& e1 : *meas_id_to_add_mod_list_v1310) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_id_to_add_mod_list_ext_v1310.is_present()) {
      j.start_array("measIdToAddModListExt-v1310");
      for (const auto& e1 : *meas_id_to_add_mod_list_ext_v1310) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_gap_cfg_per_cc_list_r14.is_present()) {
      j.write_fieldname("measGapConfigPerCC-List-r14");
      meas_gap_cfg_per_cc_list_r14->to_json(j);
    }
    if (meas_gap_sharing_cfg_r14.is_present()) {
      j.write_fieldname("measGapSharingConfig-r14");
      meas_gap_sharing_cfg_r14->to_json(j);
    }
    if (fr1_gap_r15_present) {
      j.write_bool("fr1-Gap-r15", fr1_gap_r15);
    }
    if (mgta_r15_present) {
      j.write_bool("mgta-r15", mgta_r15);
    }
    if (meas_gap_cfg_dense_prs_r15.is_present()) {
      j.write_fieldname("measGapConfigDensePRS-r15");
      meas_gap_cfg_dense_prs_r15->to_json(j);
    }
    if (height_thresh_ref_r15.is_present()) {
      j.write_fieldname("heightThreshRef-r15");
      height_thresh_ref_r15->to_json(j);
    }
  }
  j.end_obj();
}

void meas_cfg_s::speed_state_pars_c_::set(types::options e)
{
  type_ = e;
}
void meas_cfg_s::speed_state_pars_c_::set_release()
{
  set(types::release);
}
meas_cfg_s::speed_state_pars_c_::setup_s_& meas_cfg_s::speed_state_pars_c_::set_setup()
{
  set(types::setup);
  return c;
}
void meas_cfg_s::speed_state_pars_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("mobilityStateParameters");
      c.mob_state_params.to_json(j);
      j.write_fieldname("timeToTrigger-SF");
      c.time_to_trigger_sf.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "meas_cfg_s::speed_state_pars_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_cfg_s::speed_state_pars_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.mob_state_params.pack(bref));
      HANDLE_CODE(c.time_to_trigger_sf.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_cfg_s::speed_state_pars_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cfg_s::speed_state_pars_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.mob_state_params.unpack(bref));
      HANDLE_CODE(c.time_to_trigger_sf.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_cfg_s::speed_state_pars_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void meas_cfg_s::meas_scale_factor_r12_c_::set(types::options e)
{
  type_ = e;
}
void meas_cfg_s::meas_scale_factor_r12_c_::set_release()
{
  set(types::release);
}
meas_scale_factor_r12_e& meas_cfg_s::meas_scale_factor_r12_c_::set_setup()
{
  set(types::setup);
  return c;
}
void meas_cfg_s::meas_scale_factor_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_str("setup", c.to_string());
      break;
    default:
      log_invalid_choice_id(type_, "meas_cfg_s::meas_scale_factor_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_cfg_s::meas_scale_factor_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_cfg_s::meas_scale_factor_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cfg_s::meas_scale_factor_r12_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "meas_cfg_s::meas_scale_factor_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void meas_cfg_s::height_thresh_ref_r15_c_::set(types::options e)
{
  type_ = e;
}
void meas_cfg_s::height_thresh_ref_r15_c_::set_release()
{
  set(types::release);
}
uint8_t& meas_cfg_s::height_thresh_ref_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void meas_cfg_s::height_thresh_ref_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_int("setup", c);
      break;
    default:
      log_invalid_choice_id(type_, "meas_cfg_s::height_thresh_ref_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_cfg_s::height_thresh_ref_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c, (uint8_t)0u, (uint8_t)31u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_cfg_s::height_thresh_ref_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cfg_s::height_thresh_ref_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c, bref, (uint8_t)0u, (uint8_t)31u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_cfg_s::height_thresh_ref_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CarrierFreqGERAN ::= SEQUENCE
SRSASN_CODE carrier_freq_geran_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, arfcn, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(band_ind.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_geran_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(arfcn, bref, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(band_ind.unpack(bref));

  return SRSASN_SUCCESS;
}
void carrier_freq_geran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("arfcn", arfcn);
  j.write_str("bandIndicator", band_ind.to_string());
  j.end_obj();
}

// LoggedMeasurementConfiguration-v1530-IEs ::= SEQUENCE
SRSASN_CODE logged_meas_cfg_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(bt_name_list_r15_present, 1));
  HANDLE_CODE(bref.pack(wlan_name_list_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (bt_name_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, bt_name_list_r15, 1, 4));
  }
  if (wlan_name_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, wlan_name_list_r15, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_meas_cfg_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(bt_name_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(wlan_name_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (bt_name_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(bt_name_list_r15, bref, 1, 4));
  }
  if (wlan_name_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(wlan_name_list_r15, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void logged_meas_cfg_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (bt_name_list_r15_present) {
    j.start_array("bt-NameList-r15");
    for (const auto& e1 : bt_name_list_r15) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (wlan_name_list_r15_present) {
    j.start_array("wlan-NameList-r15");
    for (const auto& e1 : wlan_name_list_r15) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// CellGlobalIdEUTRA ::= SEQUENCE
SRSASN_CODE cell_global_id_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(cell_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_global_id_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(cell_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_global_id_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Identity");
  plmn_id.to_json(j);
  j.write_str("cellIdentity", cell_id.to_string());
  j.end_obj();
}

// CarrierBandwidthEUTRA ::= SEQUENCE
SRSASN_CODE carrier_bw_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_bw_present, 1));

  HANDLE_CODE(dl_bw.pack(bref));
  if (ul_bw_present) {
    HANDLE_CODE(ul_bw.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_bw_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_bw_present, 1));

  HANDLE_CODE(dl_bw.unpack(bref));
  if (ul_bw_present) {
    HANDLE_CODE(ul_bw.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void carrier_bw_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("dl-Bandwidth", dl_bw.to_string());
  if (ul_bw_present) {
    j.write_str("ul-Bandwidth", ul_bw.to_string());
  }
  j.end_obj();
}

const char* carrier_bw_eutra_s::dl_bw_opts::to_string() const
{
  static const char* options[] = {"n6",
                                  "n15",
                                  "n25",
                                  "n50",
                                  "n75",
                                  "n100",
                                  "spare10",
                                  "spare9",
                                  "spare8",
                                  "spare7",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "carrier_bw_eutra_s::dl_bw_e_");
}
uint8_t carrier_bw_eutra_s::dl_bw_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "carrier_bw_eutra_s::dl_bw_e_");
}

const char* carrier_bw_eutra_s::ul_bw_opts::to_string() const
{
  static const char* options[] = {"n6",
                                  "n15",
                                  "n25",
                                  "n50",
                                  "n75",
                                  "n100",
                                  "spare10",
                                  "spare9",
                                  "spare8",
                                  "spare7",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "carrier_bw_eutra_s::ul_bw_e_");
}
uint8_t carrier_bw_eutra_s::ul_bw_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "carrier_bw_eutra_s::ul_bw_e_");
}

// CarrierFreqEUTRA ::= SEQUENCE
SRSASN_CODE carrier_freq_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_carrier_freq_present, 1));

  HANDLE_CODE(pack_integer(bref, dl_carrier_freq, (uint32_t)0u, (uint32_t)65535u));
  if (ul_carrier_freq_present) {
    HANDLE_CODE(pack_integer(bref, ul_carrier_freq, (uint32_t)0u, (uint32_t)65535u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_carrier_freq_present, 1));

  HANDLE_CODE(unpack_integer(dl_carrier_freq, bref, (uint32_t)0u, (uint32_t)65535u));
  if (ul_carrier_freq_present) {
    HANDLE_CODE(unpack_integer(ul_carrier_freq, bref, (uint32_t)0u, (uint32_t)65535u));
  }

  return SRSASN_SUCCESS;
}
void carrier_freq_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-CarrierFreq", dl_carrier_freq);
  if (ul_carrier_freq_present) {
    j.write_int("ul-CarrierFreq", ul_carrier_freq);
  }
  j.end_obj();
}

// CarrierFreqEUTRA-v9e0 ::= SEQUENCE
SRSASN_CODE carrier_freq_eutra_v9e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_carrier_freq_v9e0_present, 1));

  HANDLE_CODE(pack_integer(bref, dl_carrier_freq_v9e0, (uint32_t)0u, (uint32_t)262143u));
  if (ul_carrier_freq_v9e0_present) {
    HANDLE_CODE(pack_integer(bref, ul_carrier_freq_v9e0, (uint32_t)0u, (uint32_t)262143u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_eutra_v9e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_carrier_freq_v9e0_present, 1));

  HANDLE_CODE(unpack_integer(dl_carrier_freq_v9e0, bref, (uint32_t)0u, (uint32_t)262143u));
  if (ul_carrier_freq_v9e0_present) {
    HANDLE_CODE(unpack_integer(ul_carrier_freq_v9e0, bref, (uint32_t)0u, (uint32_t)262143u));
  }

  return SRSASN_SUCCESS;
}
void carrier_freq_eutra_v9e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-CarrierFreq-v9e0", dl_carrier_freq_v9e0);
  if (ul_carrier_freq_v9e0_present) {
    j.write_int("ul-CarrierFreq-v9e0", ul_carrier_freq_v9e0);
  }
  j.end_obj();
}

// MobilityControlInfoV2X-r14 ::= SEQUENCE
SRSASN_CODE mob_ctrl_info_v2x_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(v2x_comm_tx_pool_exceptional_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_comm_rx_pool_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_comm_sync_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(cbr_mob_tx_cfg_list_r14_present, 1));

  if (v2x_comm_tx_pool_exceptional_r14_present) {
    HANDLE_CODE(v2x_comm_tx_pool_exceptional_r14.pack(bref));
  }
  if (v2x_comm_rx_pool_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_comm_rx_pool_r14, 1, 16));
  }
  if (v2x_comm_sync_cfg_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_comm_sync_cfg_r14, 1, 16));
  }
  if (cbr_mob_tx_cfg_list_r14_present) {
    HANDLE_CODE(cbr_mob_tx_cfg_list_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_ctrl_info_v2x_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(v2x_comm_tx_pool_exceptional_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_comm_rx_pool_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_comm_sync_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(cbr_mob_tx_cfg_list_r14_present, 1));

  if (v2x_comm_tx_pool_exceptional_r14_present) {
    HANDLE_CODE(v2x_comm_tx_pool_exceptional_r14.unpack(bref));
  }
  if (v2x_comm_rx_pool_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_comm_rx_pool_r14, bref, 1, 16));
  }
  if (v2x_comm_sync_cfg_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_comm_sync_cfg_r14, bref, 1, 16));
  }
  if (cbr_mob_tx_cfg_list_r14_present) {
    HANDLE_CODE(cbr_mob_tx_cfg_list_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mob_ctrl_info_v2x_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (v2x_comm_tx_pool_exceptional_r14_present) {
    j.write_fieldname("v2x-CommTxPoolExceptional-r14");
    v2x_comm_tx_pool_exceptional_r14.to_json(j);
  }
  if (v2x_comm_rx_pool_r14_present) {
    j.start_array("v2x-CommRxPool-r14");
    for (const auto& e1 : v2x_comm_rx_pool_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (v2x_comm_sync_cfg_r14_present) {
    j.start_array("v2x-CommSyncConfig-r14");
    for (const auto& e1 : v2x_comm_sync_cfg_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cbr_mob_tx_cfg_list_r14_present) {
    j.write_fieldname("cbr-MobilityTxConfigList-r14");
    cbr_mob_tx_cfg_list_r14.to_json(j);
  }
  j.end_obj();
}

// MobilityControlInfo ::= SEQUENCE
SRSASN_CODE mob_ctrl_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(carrier_freq_present, 1));
  HANDLE_CODE(bref.pack(carrier_bw_present, 1));
  HANDLE_CODE(bref.pack(add_spec_emission_present, 1));
  HANDLE_CODE(bref.pack(rach_cfg_ded_present, 1));

  HANDLE_CODE(pack_integer(bref, target_pci, (uint16_t)0u, (uint16_t)503u));
  if (carrier_freq_present) {
    HANDLE_CODE(carrier_freq.pack(bref));
  }
  if (carrier_bw_present) {
    HANDLE_CODE(carrier_bw.pack(bref));
  }
  if (add_spec_emission_present) {
    HANDLE_CODE(pack_integer(bref, add_spec_emission, (uint8_t)1u, (uint8_t)32u));
  }
  HANDLE_CODE(t304.pack(bref));
  HANDLE_CODE(new_ue_id.pack(bref));
  HANDLE_CODE(rr_cfg_common.pack(bref));
  if (rach_cfg_ded_present) {
    HANDLE_CODE(rach_cfg_ded.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= carrier_freq_v9e0.is_present();
    group_flags[1] |= drb_continue_rohc_r11_present;
    group_flags[2] |= mob_ctrl_info_v2x_r14.is_present();
    group_flags[2] |= ho_without_wt_change_r14_present;
    group_flags[2] |= make_before_break_r14_present;
    group_flags[2] |= rach_skip_r14.is_present();
    group_flags[2] |= same_sfn_ind_r14_present;
    group_flags[3] |= mib_repeat_status_r14_present;
    group_flags[3] |= sched_info_sib1_br_r14_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(carrier_freq_v9e0.is_present(), 1));
      if (carrier_freq_v9e0.is_present()) {
        HANDLE_CODE(carrier_freq_v9e0->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(drb_continue_rohc_r11_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mob_ctrl_info_v2x_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(ho_without_wt_change_r14_present, 1));
      HANDLE_CODE(bref.pack(make_before_break_r14_present, 1));
      HANDLE_CODE(bref.pack(rach_skip_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(same_sfn_ind_r14_present, 1));
      if (mob_ctrl_info_v2x_r14.is_present()) {
        HANDLE_CODE(mob_ctrl_info_v2x_r14->pack(bref));
      }
      if (ho_without_wt_change_r14_present) {
        HANDLE_CODE(ho_without_wt_change_r14.pack(bref));
      }
      if (rach_skip_r14.is_present()) {
        HANDLE_CODE(rach_skip_r14->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mib_repeat_status_r14_present, 1));
      HANDLE_CODE(bref.pack(sched_info_sib1_br_r14_present, 1));
      if (mib_repeat_status_r14_present) {
        HANDLE_CODE(bref.pack(mib_repeat_status_r14, 1));
      }
      if (sched_info_sib1_br_r14_present) {
        HANDLE_CODE(pack_integer(bref, sched_info_sib1_br_r14, (uint8_t)0u, (uint8_t)31u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_ctrl_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(carrier_freq_present, 1));
  HANDLE_CODE(bref.unpack(carrier_bw_present, 1));
  HANDLE_CODE(bref.unpack(add_spec_emission_present, 1));
  HANDLE_CODE(bref.unpack(rach_cfg_ded_present, 1));

  HANDLE_CODE(unpack_integer(target_pci, bref, (uint16_t)0u, (uint16_t)503u));
  if (carrier_freq_present) {
    HANDLE_CODE(carrier_freq.unpack(bref));
  }
  if (carrier_bw_present) {
    HANDLE_CODE(carrier_bw.unpack(bref));
  }
  if (add_spec_emission_present) {
    HANDLE_CODE(unpack_integer(add_spec_emission, bref, (uint8_t)1u, (uint8_t)32u));
  }
  HANDLE_CODE(t304.unpack(bref));
  HANDLE_CODE(new_ue_id.unpack(bref));
  HANDLE_CODE(rr_cfg_common.unpack(bref));
  if (rach_cfg_ded_present) {
    HANDLE_CODE(rach_cfg_ded.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool carrier_freq_v9e0_present;
      HANDLE_CODE(bref.unpack(carrier_freq_v9e0_present, 1));
      carrier_freq_v9e0.set_present(carrier_freq_v9e0_present);
      if (carrier_freq_v9e0.is_present()) {
        HANDLE_CODE(carrier_freq_v9e0->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(drb_continue_rohc_r11_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mob_ctrl_info_v2x_r14_present;
      HANDLE_CODE(bref.unpack(mob_ctrl_info_v2x_r14_present, 1));
      mob_ctrl_info_v2x_r14.set_present(mob_ctrl_info_v2x_r14_present);
      HANDLE_CODE(bref.unpack(ho_without_wt_change_r14_present, 1));
      HANDLE_CODE(bref.unpack(make_before_break_r14_present, 1));
      bool rach_skip_r14_present;
      HANDLE_CODE(bref.unpack(rach_skip_r14_present, 1));
      rach_skip_r14.set_present(rach_skip_r14_present);
      HANDLE_CODE(bref.unpack(same_sfn_ind_r14_present, 1));
      if (mob_ctrl_info_v2x_r14.is_present()) {
        HANDLE_CODE(mob_ctrl_info_v2x_r14->unpack(bref));
      }
      if (ho_without_wt_change_r14_present) {
        HANDLE_CODE(ho_without_wt_change_r14.unpack(bref));
      }
      if (rach_skip_r14.is_present()) {
        HANDLE_CODE(rach_skip_r14->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(mib_repeat_status_r14_present, 1));
      HANDLE_CODE(bref.unpack(sched_info_sib1_br_r14_present, 1));
      if (mib_repeat_status_r14_present) {
        HANDLE_CODE(bref.unpack(mib_repeat_status_r14, 1));
      }
      if (sched_info_sib1_br_r14_present) {
        HANDLE_CODE(unpack_integer(sched_info_sib1_br_r14, bref, (uint8_t)0u, (uint8_t)31u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void mob_ctrl_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("targetPhysCellId", target_pci);
  if (carrier_freq_present) {
    j.write_fieldname("carrierFreq");
    carrier_freq.to_json(j);
  }
  if (carrier_bw_present) {
    j.write_fieldname("carrierBandwidth");
    carrier_bw.to_json(j);
  }
  if (add_spec_emission_present) {
    j.write_int("additionalSpectrumEmission", add_spec_emission);
  }
  j.write_str("t304", t304.to_string());
  j.write_str("newUE-Identity", new_ue_id.to_string());
  j.write_fieldname("radioResourceConfigCommon");
  rr_cfg_common.to_json(j);
  if (rach_cfg_ded_present) {
    j.write_fieldname("rach-ConfigDedicated");
    rach_cfg_ded.to_json(j);
  }
  if (ext) {
    if (carrier_freq_v9e0.is_present()) {
      j.write_fieldname("carrierFreq-v9e0");
      carrier_freq_v9e0->to_json(j);
    }
    if (drb_continue_rohc_r11_present) {
      j.write_str("drb-ContinueROHC-r11", "true");
    }
    if (mob_ctrl_info_v2x_r14.is_present()) {
      j.write_fieldname("mobilityControlInfoV2X-r14");
      mob_ctrl_info_v2x_r14->to_json(j);
    }
    if (ho_without_wt_change_r14_present) {
      j.write_str("handoverWithoutWT-Change-r14", ho_without_wt_change_r14.to_string());
    }
    if (make_before_break_r14_present) {
      j.write_str("makeBeforeBreak-r14", "true");
    }
    if (rach_skip_r14.is_present()) {
      j.write_fieldname("rach-Skip-r14");
      rach_skip_r14->to_json(j);
    }
    if (same_sfn_ind_r14_present) {
      j.write_str("sameSFN-Indication-r14", "true");
    }
    if (mib_repeat_status_r14_present) {
      j.write_bool("mib-RepetitionStatus-r14", mib_repeat_status_r14);
    }
    if (sched_info_sib1_br_r14_present) {
      j.write_int("schedulingInfoSIB1-BR-r14", sched_info_sib1_br_r14);
    }
  }
  j.end_obj();
}

const char* mob_ctrl_info_s::t304_opts::to_string() const
{
  static const char* options[] = {"ms50", "ms100", "ms150", "ms200", "ms500", "ms1000", "ms2000", "ms10000-v1310"};
  return convert_enum_idx(options, 8, value, "mob_ctrl_info_s::t304_e_");
}
uint16_t mob_ctrl_info_s::t304_opts::to_number() const
{
  static const uint16_t options[] = {50, 100, 150, 200, 500, 1000, 2000, 10000};
  return map_enum_number(options, 8, value, "mob_ctrl_info_s::t304_e_");
}

const char* mob_ctrl_info_s::ho_without_wt_change_r14_opts::to_string() const
{
  static const char* options[] = {"keepLWA-Config", "sendEndMarker"};
  return convert_enum_idx(options, 2, value, "mob_ctrl_info_s::ho_without_wt_change_r14_e_");
}

// TraceReference-r10 ::= SEQUENCE
SRSASN_CODE trace_ref_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id_r10.pack(bref));
  HANDLE_CODE(trace_id_r10.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE trace_ref_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id_r10.unpack(bref));
  HANDLE_CODE(trace_id_r10.unpack(bref));

  return SRSASN_SUCCESS;
}
void trace_ref_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Identity-r10");
  plmn_id_r10.to_json(j);
  j.write_str("traceId-r10", trace_id_r10.to_string());
  j.end_obj();
}

// CellGlobalIdCDMA2000 ::= CHOICE
void cell_global_id_cdma2000_c::destroy_()
{
  switch (type_) {
    case types::cell_global_id1_xrtt:
      c.destroy<fixed_bitstring<47> >();
      break;
    case types::cell_global_id_hrpd:
      c.destroy<fixed_bitstring<128> >();
      break;
    default:
      break;
  }
}
void cell_global_id_cdma2000_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_global_id1_xrtt:
      c.init<fixed_bitstring<47> >();
      break;
    case types::cell_global_id_hrpd:
      c.init<fixed_bitstring<128> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_global_id_cdma2000_c");
  }
}
cell_global_id_cdma2000_c::cell_global_id_cdma2000_c(const cell_global_id_cdma2000_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_global_id1_xrtt:
      c.init(other.c.get<fixed_bitstring<47> >());
      break;
    case types::cell_global_id_hrpd:
      c.init(other.c.get<fixed_bitstring<128> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_global_id_cdma2000_c");
  }
}
cell_global_id_cdma2000_c& cell_global_id_cdma2000_c::operator=(const cell_global_id_cdma2000_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_global_id1_xrtt:
      c.set(other.c.get<fixed_bitstring<47> >());
      break;
    case types::cell_global_id_hrpd:
      c.set(other.c.get<fixed_bitstring<128> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_global_id_cdma2000_c");
  }

  return *this;
}
fixed_bitstring<47>& cell_global_id_cdma2000_c::set_cell_global_id1_xrtt()
{
  set(types::cell_global_id1_xrtt);
  return c.get<fixed_bitstring<47> >();
}
fixed_bitstring<128>& cell_global_id_cdma2000_c::set_cell_global_id_hrpd()
{
  set(types::cell_global_id_hrpd);
  return c.get<fixed_bitstring<128> >();
}
void cell_global_id_cdma2000_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_global_id1_xrtt:
      j.write_str("cellGlobalId1XRTT", c.get<fixed_bitstring<47> >().to_string());
      break;
    case types::cell_global_id_hrpd:
      j.write_str("cellGlobalIdHRPD", c.get<fixed_bitstring<128> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "cell_global_id_cdma2000_c");
  }
  j.end_obj();
}
SRSASN_CODE cell_global_id_cdma2000_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_global_id1_xrtt:
      HANDLE_CODE(c.get<fixed_bitstring<47> >().pack(bref));
      break;
    case types::cell_global_id_hrpd:
      HANDLE_CODE(c.get<fixed_bitstring<128> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cell_global_id_cdma2000_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_global_id_cdma2000_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_global_id1_xrtt:
      HANDLE_CODE(c.get<fixed_bitstring<47> >().unpack(bref));
      break;
    case types::cell_global_id_hrpd:
      HANDLE_CODE(c.get<fixed_bitstring<128> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cell_global_id_cdma2000_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// AdditionalSI-Info-r9 ::= SEQUENCE
SRSASN_CODE add_si_info_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csg_member_status_r9_present, 1));
  HANDLE_CODE(bref.pack(csg_id_r9_present, 1));

  if (csg_id_r9_present) {
    HANDLE_CODE(csg_id_r9.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE add_si_info_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csg_member_status_r9_present, 1));
  HANDLE_CODE(bref.unpack(csg_id_r9_present, 1));

  if (csg_id_r9_present) {
    HANDLE_CODE(csg_id_r9.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void add_si_info_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csg_member_status_r9_present) {
    j.write_str("csg-MemberStatus-r9", "member");
  }
  if (csg_id_r9_present) {
    j.write_str("csg-Identity-r9", csg_id_r9.to_string());
  }
  j.end_obj();
}

// BLER-Result-r12 ::= SEQUENCE
SRSASN_CODE bler_result_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, bler_r12, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(blocks_rx_r12.n_r12.pack(bref));
  HANDLE_CODE(blocks_rx_r12.m_r12.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE bler_result_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(bler_r12, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(blocks_rx_r12.n_r12.unpack(bref));
  HANDLE_CODE(blocks_rx_r12.m_r12.unpack(bref));

  return SRSASN_SUCCESS;
}
void bler_result_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bler-r12", bler_r12);
  j.write_fieldname("blocksReceived-r12");
  j.start_obj();
  j.write_str("n-r12", blocks_rx_r12.n_r12.to_string());
  j.write_str("m-r12", blocks_rx_r12.m_r12.to_string());
  j.end_obj();
  j.end_obj();
}

// CellGlobalIdUTRA ::= SEQUENCE
SRSASN_CODE cell_global_id_utra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(cell_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_global_id_utra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(cell_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_global_id_utra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Identity");
  plmn_id.to_json(j);
  j.write_str("cellIdentity", cell_id.to_string());
  j.end_obj();
}

// MeasResultCDMA2000 ::= SEQUENCE
SRSASN_CODE meas_result_cdma2000_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cgi_info_present, 1));

  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)511u));
  if (cgi_info_present) {
    HANDLE_CODE(cgi_info.pack(bref));
  }
  bref.pack(meas_result.ext, 1);
  HANDLE_CODE(bref.pack(meas_result.pilot_pn_phase_present, 1));
  if (meas_result.pilot_pn_phase_present) {
    HANDLE_CODE(pack_integer(bref, meas_result.pilot_pn_phase, (uint16_t)0u, (uint16_t)32767u));
  }
  HANDLE_CODE(pack_integer(bref, meas_result.pilot_strength, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_cdma2000_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cgi_info_present, 1));

  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)511u));
  if (cgi_info_present) {
    HANDLE_CODE(cgi_info.unpack(bref));
  }
  bref.unpack(meas_result.ext, 1);
  HANDLE_CODE(bref.unpack(meas_result.pilot_pn_phase_present, 1));
  if (meas_result.pilot_pn_phase_present) {
    HANDLE_CODE(unpack_integer(meas_result.pilot_pn_phase, bref, (uint16_t)0u, (uint16_t)32767u));
  }
  HANDLE_CODE(unpack_integer(meas_result.pilot_strength, bref, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
void meas_result_cdma2000_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId", pci);
  if (cgi_info_present) {
    j.write_fieldname("cgi-Info");
    cgi_info.to_json(j);
  }
  j.write_fieldname("measResult");
  j.start_obj();
  if (meas_result.pilot_pn_phase_present) {
    j.write_int("pilotPnPhase", meas_result.pilot_pn_phase);
  }
  j.write_int("pilotStrength", meas_result.pilot_strength);
  j.end_obj();
  j.end_obj();
}

// CellGlobalIdGERAN ::= SEQUENCE
SRSASN_CODE cell_global_id_geran_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(location_area_code.pack(bref));
  HANDLE_CODE(cell_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_global_id_geran_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(location_area_code.unpack(bref));
  HANDLE_CODE(cell_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_global_id_geran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Identity");
  plmn_id.to_json(j);
  j.write_str("locationAreaCode", location_area_code.to_string());
  j.write_str("cellIdentity", cell_id.to_string());
  j.end_obj();
}

// DataBLER-MCH-Result-r12 ::= SEQUENCE
SRSASN_CODE data_bler_mch_result_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, mch_idx_r12, (uint8_t)1u, (uint8_t)15u));
  HANDLE_CODE(data_bler_result_r12.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE data_bler_mch_result_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(mch_idx_r12, bref, (uint8_t)1u, (uint8_t)15u));
  HANDLE_CODE(data_bler_result_r12.unpack(bref));

  return SRSASN_SUCCESS;
}
void data_bler_mch_result_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("mch-Index-r12", mch_idx_r12);
  j.write_fieldname("dataBLER-Result-r12");
  data_bler_result_r12.to_json(j);
  j.end_obj();
}

// MeasResultEUTRA ::= SEQUENCE
SRSASN_CODE meas_result_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cgi_info_present, 1));

  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)503u));
  if (cgi_info_present) {
    HANDLE_CODE(bref.pack(cgi_info.plmn_id_list_present, 1));
    HANDLE_CODE(cgi_info.cell_global_id.pack(bref));
    HANDLE_CODE(cgi_info.tac.pack(bref));
    if (cgi_info.plmn_id_list_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, cgi_info.plmn_id_list, 1, 5));
    }
  }
  HANDLE_CODE(meas_result.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cgi_info_present, 1));

  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)503u));
  if (cgi_info_present) {
    HANDLE_CODE(bref.unpack(cgi_info.plmn_id_list_present, 1));
    HANDLE_CODE(cgi_info.cell_global_id.unpack(bref));
    HANDLE_CODE(cgi_info.tac.unpack(bref));
    if (cgi_info.plmn_id_list_present) {
      HANDLE_CODE(unpack_dyn_seq_of(cgi_info.plmn_id_list, bref, 1, 5));
    }
  }
  HANDLE_CODE(meas_result.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_result_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId", pci);
  if (cgi_info_present) {
    j.write_fieldname("cgi-Info");
    j.start_obj();
    j.write_fieldname("cellGlobalId");
    cgi_info.cell_global_id.to_json(j);
    j.write_str("trackingAreaCode", cgi_info.tac.to_string());
    if (cgi_info.plmn_id_list_present) {
      j.start_array("plmn-IdentityList");
      for (const auto& e1 : cgi_info.plmn_id_list) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  j.write_fieldname("measResult");
  meas_result.to_json(j);
  j.end_obj();
}

SRSASN_CODE meas_result_eutra_s::meas_result_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rsrp_result_present, 1));
  HANDLE_CODE(bref.pack(rsrq_result_present, 1));

  if (rsrp_result_present) {
    HANDLE_CODE(pack_integer(bref, rsrp_result, (uint8_t)0u, (uint8_t)97u));
  }
  if (rsrq_result_present) {
    HANDLE_CODE(pack_integer(bref, rsrq_result, (uint8_t)0u, (uint8_t)34u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= add_si_info_r9.is_present();
    group_flags[1] |= primary_plmn_suitable_r12_present;
    group_flags[1] |= meas_result_v1250_present;
    group_flags[2] |= rs_sinr_result_r13_present;
    group_flags[2] |= cgi_info_v1310.is_present();
    group_flags[3] |= meas_result_v1360_present;
    group_flags[4] |= cgi_info_minus5_gc_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(add_si_info_r9.is_present(), 1));
      if (add_si_info_r9.is_present()) {
        HANDLE_CODE(add_si_info_r9->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(primary_plmn_suitable_r12_present, 1));
      HANDLE_CODE(bref.pack(meas_result_v1250_present, 1));
      if (meas_result_v1250_present) {
        HANDLE_CODE(pack_integer(bref, meas_result_v1250, (int8_t)-30, (int8_t)46));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rs_sinr_result_r13_present, 1));
      HANDLE_CODE(bref.pack(cgi_info_v1310.is_present(), 1));
      if (rs_sinr_result_r13_present) {
        HANDLE_CODE(pack_integer(bref, rs_sinr_result_r13, (uint8_t)0u, (uint8_t)127u));
      }
      if (cgi_info_v1310.is_present()) {
        HANDLE_CODE(bref.pack(cgi_info_v1310->freq_band_ind_r13_present, 1));
        HANDLE_CODE(bref.pack(cgi_info_v1310->multi_band_info_list_r13_present, 1));
        HANDLE_CODE(bref.pack(cgi_info_v1310->freq_band_ind_prio_r13_present, 1));
        if (cgi_info_v1310->freq_band_ind_r13_present) {
          HANDLE_CODE(pack_integer(bref, cgi_info_v1310->freq_band_ind_r13, (uint16_t)1u, (uint16_t)256u));
        }
        if (cgi_info_v1310->multi_band_info_list_r13_present) {
          HANDLE_CODE(
              pack_dyn_seq_of(bref, cgi_info_v1310->multi_band_info_list_r13, 1, 8, integer_packer<uint16_t>(1, 256)));
        }
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_v1360_present, 1));
      if (meas_result_v1360_present) {
        HANDLE_CODE(pack_integer(bref, meas_result_v1360, (int8_t)-17, (int8_t)-1));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cgi_info_minus5_gc_r15.is_present(), 1));
      if (cgi_info_minus5_gc_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *cgi_info_minus5_gc_r15, 1, 6));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_eutra_s::meas_result_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rsrp_result_present, 1));
  HANDLE_CODE(bref.unpack(rsrq_result_present, 1));

  if (rsrp_result_present) {
    HANDLE_CODE(unpack_integer(rsrp_result, bref, (uint8_t)0u, (uint8_t)97u));
  }
  if (rsrq_result_present) {
    HANDLE_CODE(unpack_integer(rsrq_result, bref, (uint8_t)0u, (uint8_t)34u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(5);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool add_si_info_r9_present;
      HANDLE_CODE(bref.unpack(add_si_info_r9_present, 1));
      add_si_info_r9.set_present(add_si_info_r9_present);
      if (add_si_info_r9.is_present()) {
        HANDLE_CODE(add_si_info_r9->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(primary_plmn_suitable_r12_present, 1));
      HANDLE_CODE(bref.unpack(meas_result_v1250_present, 1));
      if (meas_result_v1250_present) {
        HANDLE_CODE(unpack_integer(meas_result_v1250, bref, (int8_t)-30, (int8_t)46));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(rs_sinr_result_r13_present, 1));
      bool cgi_info_v1310_present;
      HANDLE_CODE(bref.unpack(cgi_info_v1310_present, 1));
      cgi_info_v1310.set_present(cgi_info_v1310_present);
      if (rs_sinr_result_r13_present) {
        HANDLE_CODE(unpack_integer(rs_sinr_result_r13, bref, (uint8_t)0u, (uint8_t)127u));
      }
      if (cgi_info_v1310.is_present()) {
        HANDLE_CODE(bref.unpack(cgi_info_v1310->freq_band_ind_r13_present, 1));
        HANDLE_CODE(bref.unpack(cgi_info_v1310->multi_band_info_list_r13_present, 1));
        HANDLE_CODE(bref.unpack(cgi_info_v1310->freq_band_ind_prio_r13_present, 1));
        if (cgi_info_v1310->freq_band_ind_r13_present) {
          HANDLE_CODE(unpack_integer(cgi_info_v1310->freq_band_ind_r13, bref, (uint16_t)1u, (uint16_t)256u));
        }
        if (cgi_info_v1310->multi_band_info_list_r13_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              cgi_info_v1310->multi_band_info_list_r13, bref, 1, 8, integer_packer<uint16_t>(1, 256)));
        }
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(meas_result_v1360_present, 1));
      if (meas_result_v1360_present) {
        HANDLE_CODE(unpack_integer(meas_result_v1360, bref, (int8_t)-17, (int8_t)-1));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cgi_info_minus5_gc_r15_present;
      HANDLE_CODE(bref.unpack(cgi_info_minus5_gc_r15_present, 1));
      cgi_info_minus5_gc_r15.set_present(cgi_info_minus5_gc_r15_present);
      if (cgi_info_minus5_gc_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*cgi_info_minus5_gc_r15, bref, 1, 6));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_result_eutra_s::meas_result_s_::to_json(json_writer& j) const
{
  j.start_obj();
  if (rsrp_result_present) {
    j.write_int("rsrpResult", rsrp_result);
  }
  if (rsrq_result_present) {
    j.write_int("rsrqResult", rsrq_result);
  }
  if (ext) {
    if (add_si_info_r9.is_present()) {
      j.write_fieldname("additionalSI-Info-r9");
      add_si_info_r9->to_json(j);
    }
    if (primary_plmn_suitable_r12_present) {
      j.write_str("primaryPLMN-Suitable-r12", "true");
    }
    if (meas_result_v1250_present) {
      j.write_int("measResult-v1250", meas_result_v1250);
    }
    if (rs_sinr_result_r13_present) {
      j.write_int("rs-sinr-Result-r13", rs_sinr_result_r13);
    }
    if (cgi_info_v1310.is_present()) {
      j.write_fieldname("cgi-Info-v1310");
      j.start_obj();
      if (cgi_info_v1310->freq_band_ind_r13_present) {
        j.write_int("freqBandIndicator-r13", cgi_info_v1310->freq_band_ind_r13);
      }
      if (cgi_info_v1310->multi_band_info_list_r13_present) {
        j.start_array("multiBandInfoList-r13");
        for (const auto& e1 : cgi_info_v1310->multi_band_info_list_r13) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (cgi_info_v1310->freq_band_ind_prio_r13_present) {
        j.write_str("freqBandIndicatorPriority-r13", "true");
      }
      j.end_obj();
    }
    if (meas_result_v1360_present) {
      j.write_int("measResult-v1360", meas_result_v1360);
    }
    if (cgi_info_minus5_gc_r15.is_present()) {
      j.start_array("cgi-Info-5GC-r15");
      for (const auto& e1 : *cgi_info_minus5_gc_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// MeasResultUTRA ::= SEQUENCE
SRSASN_CODE meas_result_utra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cgi_info_present, 1));

  HANDLE_CODE(pci.pack(bref));
  if (cgi_info_present) {
    HANDLE_CODE(bref.pack(cgi_info.location_area_code_present, 1));
    HANDLE_CODE(bref.pack(cgi_info.routing_area_code_present, 1));
    HANDLE_CODE(bref.pack(cgi_info.plmn_id_list_present, 1));
    HANDLE_CODE(cgi_info.cell_global_id.pack(bref));
    if (cgi_info.location_area_code_present) {
      HANDLE_CODE(cgi_info.location_area_code.pack(bref));
    }
    if (cgi_info.routing_area_code_present) {
      HANDLE_CODE(cgi_info.routing_area_code.pack(bref));
    }
    if (cgi_info.plmn_id_list_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, cgi_info.plmn_id_list, 1, 5));
    }
  }
  HANDLE_CODE(meas_result.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_utra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cgi_info_present, 1));

  HANDLE_CODE(pci.unpack(bref));
  if (cgi_info_present) {
    HANDLE_CODE(bref.unpack(cgi_info.location_area_code_present, 1));
    HANDLE_CODE(bref.unpack(cgi_info.routing_area_code_present, 1));
    HANDLE_CODE(bref.unpack(cgi_info.plmn_id_list_present, 1));
    HANDLE_CODE(cgi_info.cell_global_id.unpack(bref));
    if (cgi_info.location_area_code_present) {
      HANDLE_CODE(cgi_info.location_area_code.unpack(bref));
    }
    if (cgi_info.routing_area_code_present) {
      HANDLE_CODE(cgi_info.routing_area_code.unpack(bref));
    }
    if (cgi_info.plmn_id_list_present) {
      HANDLE_CODE(unpack_dyn_seq_of(cgi_info.plmn_id_list, bref, 1, 5));
    }
  }
  HANDLE_CODE(meas_result.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_result_utra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("physCellId");
  pci.to_json(j);
  if (cgi_info_present) {
    j.write_fieldname("cgi-Info");
    j.start_obj();
    j.write_fieldname("cellGlobalId");
    cgi_info.cell_global_id.to_json(j);
    if (cgi_info.location_area_code_present) {
      j.write_str("locationAreaCode", cgi_info.location_area_code.to_string());
    }
    if (cgi_info.routing_area_code_present) {
      j.write_str("routingAreaCode", cgi_info.routing_area_code.to_string());
    }
    if (cgi_info.plmn_id_list_present) {
      j.start_array("plmn-IdentityList");
      for (const auto& e1 : cgi_info.plmn_id_list) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  j.write_fieldname("measResult");
  meas_result.to_json(j);
  j.end_obj();
}

void meas_result_utra_s::pci_c_::destroy_() {}
void meas_result_utra_s::pci_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
meas_result_utra_s::pci_c_::pci_c_(const meas_result_utra_s::pci_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::fdd:
      c.init(other.c.get<uint16_t>());
      break;
    case types::tdd:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_result_utra_s::pci_c_");
  }
}
meas_result_utra_s::pci_c_& meas_result_utra_s::pci_c_::operator=(const meas_result_utra_s::pci_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::fdd:
      c.set(other.c.get<uint16_t>());
      break;
    case types::tdd:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_result_utra_s::pci_c_");
  }

  return *this;
}
uint16_t& meas_result_utra_s::pci_c_::set_fdd()
{
  set(types::fdd);
  return c.get<uint16_t>();
}
uint8_t& meas_result_utra_s::pci_c_::set_tdd()
{
  set(types::tdd);
  return c.get<uint8_t>();
}
void meas_result_utra_s::pci_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fdd:
      j.write_int("fdd", c.get<uint16_t>());
      break;
    case types::tdd:
      j.write_int("tdd", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "meas_result_utra_s::pci_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_result_utra_s::pci_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fdd:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::tdd:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_result_utra_s::pci_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_utra_s::pci_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fdd:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::tdd:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_result_utra_s::pci_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE meas_result_utra_s::meas_result_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(utra_rscp_present, 1));
  HANDLE_CODE(bref.pack(utra_ec_n0_present, 1));

  if (utra_rscp_present) {
    HANDLE_CODE(pack_integer(bref, utra_rscp, (int8_t)-5, (int8_t)91));
  }
  if (utra_ec_n0_present) {
    HANDLE_CODE(pack_integer(bref, utra_ec_n0, (uint8_t)0u, (uint8_t)49u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= add_si_info_r9.is_present();
    group_flags[1] |= primary_plmn_suitable_r12_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(add_si_info_r9.is_present(), 1));
      if (add_si_info_r9.is_present()) {
        HANDLE_CODE(add_si_info_r9->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(primary_plmn_suitable_r12_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_utra_s::meas_result_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(utra_rscp_present, 1));
  HANDLE_CODE(bref.unpack(utra_ec_n0_present, 1));

  if (utra_rscp_present) {
    HANDLE_CODE(unpack_integer(utra_rscp, bref, (int8_t)-5, (int8_t)91));
  }
  if (utra_ec_n0_present) {
    HANDLE_CODE(unpack_integer(utra_ec_n0, bref, (uint8_t)0u, (uint8_t)49u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool add_si_info_r9_present;
      HANDLE_CODE(bref.unpack(add_si_info_r9_present, 1));
      add_si_info_r9.set_present(add_si_info_r9_present);
      if (add_si_info_r9.is_present()) {
        HANDLE_CODE(add_si_info_r9->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(primary_plmn_suitable_r12_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_result_utra_s::meas_result_s_::to_json(json_writer& j) const
{
  j.start_obj();
  if (utra_rscp_present) {
    j.write_int("utra-RSCP", utra_rscp);
  }
  if (utra_ec_n0_present) {
    j.write_int("utra-EcN0", utra_ec_n0);
  }
  if (ext) {
    if (add_si_info_r9.is_present()) {
      j.write_fieldname("additionalSI-Info-r9");
      add_si_info_r9->to_json(j);
    }
    if (primary_plmn_suitable_r12_present) {
      j.write_str("primaryPLMN-Suitable-r12", "true");
    }
  }
  j.end_obj();
}

// LocationInfo-r10 ::= SEQUENCE
SRSASN_CODE location_info_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(horizontal_velocity_r10_present, 1));
  HANDLE_CODE(bref.pack(gnss_tod_msec_r10_present, 1));

  HANDLE_CODE(location_coordinates_r10.pack(bref));
  if (horizontal_velocity_r10_present) {
    HANDLE_CODE(horizontal_velocity_r10.pack(bref));
  }
  if (gnss_tod_msec_r10_present) {
    HANDLE_CODE(gnss_tod_msec_r10.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= vertical_velocity_info_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(vertical_velocity_info_r15.is_present(), 1));
      if (vertical_velocity_info_r15.is_present()) {
        HANDLE_CODE(vertical_velocity_info_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE location_info_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(horizontal_velocity_r10_present, 1));
  HANDLE_CODE(bref.unpack(gnss_tod_msec_r10_present, 1));

  HANDLE_CODE(location_coordinates_r10.unpack(bref));
  if (horizontal_velocity_r10_present) {
    HANDLE_CODE(horizontal_velocity_r10.unpack(bref));
  }
  if (gnss_tod_msec_r10_present) {
    HANDLE_CODE(gnss_tod_msec_r10.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool vertical_velocity_info_r15_present;
      HANDLE_CODE(bref.unpack(vertical_velocity_info_r15_present, 1));
      vertical_velocity_info_r15.set_present(vertical_velocity_info_r15_present);
      if (vertical_velocity_info_r15.is_present()) {
        HANDLE_CODE(vertical_velocity_info_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void location_info_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("locationCoordinates-r10");
  location_coordinates_r10.to_json(j);
  if (horizontal_velocity_r10_present) {
    j.write_str("horizontalVelocity-r10", horizontal_velocity_r10.to_string());
  }
  if (gnss_tod_msec_r10_present) {
    j.write_str("gnss-TOD-msec-r10", gnss_tod_msec_r10.to_string());
  }
  if (ext) {
    if (vertical_velocity_info_r15.is_present()) {
      j.write_fieldname("verticalVelocityInfo-r15");
      vertical_velocity_info_r15->to_json(j);
    }
  }
  j.end_obj();
}

void location_info_r10_s::location_coordinates_r10_c_::destroy_()
{
  switch (type_) {
    case types::ellipsoid_point_r10:
      c.destroy<dyn_octstring>();
      break;
    case types::ellipsoid_point_with_altitude_r10:
      c.destroy<dyn_octstring>();
      break;
    case types::ellipsoid_point_with_uncertainty_circle_r11:
      c.destroy<dyn_octstring>();
      break;
    case types::ellipsoid_point_with_uncertainty_ellipse_r11:
      c.destroy<dyn_octstring>();
      break;
    case types::ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11:
      c.destroy<dyn_octstring>();
      break;
    case types::ellipsoid_arc_r11:
      c.destroy<dyn_octstring>();
      break;
    case types::polygon_r11:
      c.destroy<dyn_octstring>();
      break;
    default:
      break;
  }
}
void location_info_r10_s::location_coordinates_r10_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ellipsoid_point_r10:
      c.init<dyn_octstring>();
      break;
    case types::ellipsoid_point_with_altitude_r10:
      c.init<dyn_octstring>();
      break;
    case types::ellipsoid_point_with_uncertainty_circle_r11:
      c.init<dyn_octstring>();
      break;
    case types::ellipsoid_point_with_uncertainty_ellipse_r11:
      c.init<dyn_octstring>();
      break;
    case types::ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11:
      c.init<dyn_octstring>();
      break;
    case types::ellipsoid_arc_r11:
      c.init<dyn_octstring>();
      break;
    case types::polygon_r11:
      c.init<dyn_octstring>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "location_info_r10_s::location_coordinates_r10_c_");
  }
}
location_info_r10_s::location_coordinates_r10_c_::location_coordinates_r10_c_(
    const location_info_r10_s::location_coordinates_r10_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ellipsoid_point_r10:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::ellipsoid_point_with_altitude_r10:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::ellipsoid_point_with_uncertainty_circle_r11:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::ellipsoid_point_with_uncertainty_ellipse_r11:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::ellipsoid_arc_r11:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::polygon_r11:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "location_info_r10_s::location_coordinates_r10_c_");
  }
}
location_info_r10_s::location_coordinates_r10_c_& location_info_r10_s::location_coordinates_r10_c_::operator=(
    const location_info_r10_s::location_coordinates_r10_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ellipsoid_point_r10:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::ellipsoid_point_with_altitude_r10:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::ellipsoid_point_with_uncertainty_circle_r11:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::ellipsoid_point_with_uncertainty_ellipse_r11:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::ellipsoid_arc_r11:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::polygon_r11:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "location_info_r10_s::location_coordinates_r10_c_");
  }

  return *this;
}
dyn_octstring& location_info_r10_s::location_coordinates_r10_c_::set_ellipsoid_point_r10()
{
  set(types::ellipsoid_point_r10);
  return c.get<dyn_octstring>();
}
dyn_octstring& location_info_r10_s::location_coordinates_r10_c_::set_ellipsoid_point_with_altitude_r10()
{
  set(types::ellipsoid_point_with_altitude_r10);
  return c.get<dyn_octstring>();
}
dyn_octstring& location_info_r10_s::location_coordinates_r10_c_::set_ellipsoid_point_with_uncertainty_circle_r11()
{
  set(types::ellipsoid_point_with_uncertainty_circle_r11);
  return c.get<dyn_octstring>();
}
dyn_octstring& location_info_r10_s::location_coordinates_r10_c_::set_ellipsoid_point_with_uncertainty_ellipse_r11()
{
  set(types::ellipsoid_point_with_uncertainty_ellipse_r11);
  return c.get<dyn_octstring>();
}
dyn_octstring&
location_info_r10_s::location_coordinates_r10_c_::set_ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11()
{
  set(types::ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11);
  return c.get<dyn_octstring>();
}
dyn_octstring& location_info_r10_s::location_coordinates_r10_c_::set_ellipsoid_arc_r11()
{
  set(types::ellipsoid_arc_r11);
  return c.get<dyn_octstring>();
}
dyn_octstring& location_info_r10_s::location_coordinates_r10_c_::set_polygon_r11()
{
  set(types::polygon_r11);
  return c.get<dyn_octstring>();
}
void location_info_r10_s::location_coordinates_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ellipsoid_point_r10:
      j.write_str("ellipsoid-Point-r10", c.get<dyn_octstring>().to_string());
      break;
    case types::ellipsoid_point_with_altitude_r10:
      j.write_str("ellipsoidPointWithAltitude-r10", c.get<dyn_octstring>().to_string());
      break;
    case types::ellipsoid_point_with_uncertainty_circle_r11:
      j.write_str("ellipsoidPointWithUncertaintyCircle-r11", c.get<dyn_octstring>().to_string());
      break;
    case types::ellipsoid_point_with_uncertainty_ellipse_r11:
      j.write_str("ellipsoidPointWithUncertaintyEllipse-r11", c.get<dyn_octstring>().to_string());
      break;
    case types::ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11:
      j.write_str("ellipsoidPointWithAltitudeAndUncertaintyEllipsoid-r11", c.get<dyn_octstring>().to_string());
      break;
    case types::ellipsoid_arc_r11:
      j.write_str("ellipsoidArc-r11", c.get<dyn_octstring>().to_string());
      break;
    case types::polygon_r11:
      j.write_str("polygon-r11", c.get<dyn_octstring>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "location_info_r10_s::location_coordinates_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE location_info_r10_s::location_coordinates_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ellipsoid_point_r10:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    case types::ellipsoid_point_with_altitude_r10:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    case types::ellipsoid_point_with_uncertainty_circle_r11: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
    } break;
    case types::ellipsoid_point_with_uncertainty_ellipse_r11: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
    } break;
    case types::ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
    } break;
    case types::ellipsoid_arc_r11: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
    } break;
    case types::polygon_r11: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "location_info_r10_s::location_coordinates_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE location_info_r10_s::location_coordinates_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ellipsoid_point_r10:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    case types::ellipsoid_point_with_altitude_r10:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    case types::ellipsoid_point_with_uncertainty_circle_r11: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
    } break;
    case types::ellipsoid_point_with_uncertainty_ellipse_r11: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
    } break;
    case types::ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
    } break;
    case types::ellipsoid_arc_r11: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
    } break;
    case types::polygon_r11: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "location_info_r10_s::location_coordinates_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void location_info_r10_s::vertical_velocity_info_r15_c_::destroy_()
{
  switch (type_) {
    case types::vertical_velocity_r15:
      c.destroy<dyn_octstring>();
      break;
    case types::vertical_velocity_and_uncertainty_r15:
      c.destroy<dyn_octstring>();
      break;
    default:
      break;
  }
}
void location_info_r10_s::vertical_velocity_info_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::vertical_velocity_r15:
      c.init<dyn_octstring>();
      break;
    case types::vertical_velocity_and_uncertainty_r15:
      c.init<dyn_octstring>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "location_info_r10_s::vertical_velocity_info_r15_c_");
  }
}
location_info_r10_s::vertical_velocity_info_r15_c_::vertical_velocity_info_r15_c_(
    const location_info_r10_s::vertical_velocity_info_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::vertical_velocity_r15:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::vertical_velocity_and_uncertainty_r15:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "location_info_r10_s::vertical_velocity_info_r15_c_");
  }
}
location_info_r10_s::vertical_velocity_info_r15_c_& location_info_r10_s::vertical_velocity_info_r15_c_::operator=(
    const location_info_r10_s::vertical_velocity_info_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::vertical_velocity_r15:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::vertical_velocity_and_uncertainty_r15:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "location_info_r10_s::vertical_velocity_info_r15_c_");
  }

  return *this;
}
dyn_octstring& location_info_r10_s::vertical_velocity_info_r15_c_::set_vertical_velocity_r15()
{
  set(types::vertical_velocity_r15);
  return c.get<dyn_octstring>();
}
dyn_octstring& location_info_r10_s::vertical_velocity_info_r15_c_::set_vertical_velocity_and_uncertainty_r15()
{
  set(types::vertical_velocity_and_uncertainty_r15);
  return c.get<dyn_octstring>();
}
void location_info_r10_s::vertical_velocity_info_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::vertical_velocity_r15:
      j.write_str("verticalVelocity-r15", c.get<dyn_octstring>().to_string());
      break;
    case types::vertical_velocity_and_uncertainty_r15:
      j.write_str("verticalVelocityAndUncertainty-r15", c.get<dyn_octstring>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "location_info_r10_s::vertical_velocity_info_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE location_info_r10_s::vertical_velocity_info_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::vertical_velocity_r15:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    case types::vertical_velocity_and_uncertainty_r15:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "location_info_r10_s::vertical_velocity_info_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE location_info_r10_s::vertical_velocity_info_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::vertical_velocity_r15:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    case types::vertical_velocity_and_uncertainty_r15:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "location_info_r10_s::vertical_velocity_info_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MeasResultGERAN ::= SEQUENCE
SRSASN_CODE meas_result_geran_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cgi_info_present, 1));

  HANDLE_CODE(carrier_freq.pack(bref));
  HANDLE_CODE(pci.pack(bref));
  if (cgi_info_present) {
    HANDLE_CODE(bref.pack(cgi_info.routing_area_code_present, 1));
    HANDLE_CODE(cgi_info.cell_global_id.pack(bref));
    if (cgi_info.routing_area_code_present) {
      HANDLE_CODE(cgi_info.routing_area_code.pack(bref));
    }
  }
  bref.pack(meas_result.ext, 1);
  HANDLE_CODE(pack_integer(bref, meas_result.rssi, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_geran_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cgi_info_present, 1));

  HANDLE_CODE(carrier_freq.unpack(bref));
  HANDLE_CODE(pci.unpack(bref));
  if (cgi_info_present) {
    HANDLE_CODE(bref.unpack(cgi_info.routing_area_code_present, 1));
    HANDLE_CODE(cgi_info.cell_global_id.unpack(bref));
    if (cgi_info.routing_area_code_present) {
      HANDLE_CODE(cgi_info.routing_area_code.unpack(bref));
    }
  }
  bref.unpack(meas_result.ext, 1);
  HANDLE_CODE(unpack_integer(meas_result.rssi, bref, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
void meas_result_geran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("carrierFreq");
  carrier_freq.to_json(j);
  j.write_fieldname("physCellId");
  pci.to_json(j);
  if (cgi_info_present) {
    j.write_fieldname("cgi-Info");
    j.start_obj();
    j.write_fieldname("cellGlobalId");
    cgi_info.cell_global_id.to_json(j);
    if (cgi_info.routing_area_code_present) {
      j.write_str("routingAreaCode", cgi_info.routing_area_code.to_string());
    }
    j.end_obj();
  }
  j.write_fieldname("measResult");
  j.start_obj();
  j.write_int("rssi", meas_result.rssi);
  j.end_obj();
  j.end_obj();
}

// MeasResultNR-r15 ::= SEQUENCE
SRSASN_CODE meas_result_nr_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rsrp_result_r15_present, 1));
  HANDLE_CODE(bref.pack(rsrq_result_r15_present, 1));
  HANDLE_CODE(bref.pack(rs_sinr_result_r15_present, 1));

  if (rsrp_result_r15_present) {
    HANDLE_CODE(pack_integer(bref, rsrp_result_r15, (uint8_t)0u, (uint8_t)127u));
  }
  if (rsrq_result_r15_present) {
    HANDLE_CODE(pack_integer(bref, rsrq_result_r15, (uint8_t)0u, (uint8_t)127u));
  }
  if (rs_sinr_result_r15_present) {
    HANDLE_CODE(pack_integer(bref, rs_sinr_result_r15, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_nr_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rsrp_result_r15_present, 1));
  HANDLE_CODE(bref.unpack(rsrq_result_r15_present, 1));
  HANDLE_CODE(bref.unpack(rs_sinr_result_r15_present, 1));

  if (rsrp_result_r15_present) {
    HANDLE_CODE(unpack_integer(rsrp_result_r15, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (rsrq_result_r15_present) {
    HANDLE_CODE(unpack_integer(rsrq_result_r15, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (rs_sinr_result_r15_present) {
    HANDLE_CODE(unpack_integer(rs_sinr_result_r15, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void meas_result_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rsrp_result_r15_present) {
    j.write_int("rsrpResult-r15", rsrp_result_r15);
  }
  if (rsrq_result_r15_present) {
    j.write_int("rsrqResult-r15", rsrq_result_r15);
  }
  if (rs_sinr_result_r15_present) {
    j.write_int("rs-sinr-Result-r15", rs_sinr_result_r15);
  }
  j.end_obj();
}

// MeasResultsCDMA2000 ::= SEQUENCE
SRSASN_CODE meas_results_cdma2000_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pre_regist_status_hrpd, 1));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_list_cdma2000, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_results_cdma2000_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pre_regist_status_hrpd, 1));
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_list_cdma2000, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void meas_results_cdma2000_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("preRegistrationStatusHRPD", pre_regist_status_hrpd);
  j.start_array("measResultListCDMA2000");
  for (const auto& e1 : meas_result_list_cdma2000) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// PLMN-IdentityInfoNR-r15 ::= SEQUENCE
SRSASN_CODE plmn_id_info_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tac_r15_present, 1));
  HANDLE_CODE(bref.pack(ran_area_code_r15_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_r15, 1, 12));
  if (tac_r15_present) {
    HANDLE_CODE(tac_r15.pack(bref));
  }
  if (ran_area_code_r15_present) {
    HANDLE_CODE(pack_integer(bref, ran_area_code_r15, (uint16_t)0u, (uint16_t)255u));
  }
  HANDLE_CODE(cell_id_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_info_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tac_r15_present, 1));
  HANDLE_CODE(bref.unpack(ran_area_code_r15_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_r15, bref, 1, 12));
  if (tac_r15_present) {
    HANDLE_CODE(tac_r15.unpack(bref));
  }
  if (ran_area_code_r15_present) {
    HANDLE_CODE(unpack_integer(ran_area_code_r15, bref, (uint16_t)0u, (uint16_t)255u));
  }
  HANDLE_CODE(cell_id_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void plmn_id_info_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("plmn-IdentityList-r15");
  for (const auto& e1 : plmn_id_list_r15) {
    e1.to_json(j);
  }
  j.end_array();
  if (tac_r15_present) {
    j.write_str("trackingAreaCode-r15", tac_r15.to_string());
  }
  if (ran_area_code_r15_present) {
    j.write_int("ran-AreaCode-r15", ran_area_code_r15);
  }
  j.write_str("cellIdentity-r15", cell_id_r15.to_string());
  j.end_obj();
}

// RSRQ-Type-r12 ::= SEQUENCE
SRSASN_CODE rsrq_type_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(all_symbols_r12, 1));
  HANDLE_CODE(bref.pack(wide_band_r12, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rsrq_type_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(all_symbols_r12, 1));
  HANDLE_CODE(bref.unpack(wide_band_r12, 1));

  return SRSASN_SUCCESS;
}
void rsrq_type_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("allSymbols-r12", all_symbols_r12);
  j.write_bool("wideBand-r12", wide_band_r12);
  j.end_obj();
}

// WLAN-RTT-r15 ::= SEQUENCE
SRSASN_CODE wlan_rtt_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rtt_accuracy_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, rtt_value_r15, (uint32_t)0u, (uint32_t)16777215u));
  HANDLE_CODE(rtt_units_r15.pack(bref));
  if (rtt_accuracy_r15_present) {
    HANDLE_CODE(pack_integer(bref, rtt_accuracy_r15, (uint16_t)0u, (uint16_t)255u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_rtt_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rtt_accuracy_r15_present, 1));

  HANDLE_CODE(unpack_integer(rtt_value_r15, bref, (uint32_t)0u, (uint32_t)16777215u));
  HANDLE_CODE(rtt_units_r15.unpack(bref));
  if (rtt_accuracy_r15_present) {
    HANDLE_CODE(unpack_integer(rtt_accuracy_r15, bref, (uint16_t)0u, (uint16_t)255u));
  }

  return SRSASN_SUCCESS;
}
void wlan_rtt_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rttValue-r15", rtt_value_r15);
  j.write_str("rttUnits-r15", rtt_units_r15.to_string());
  if (rtt_accuracy_r15_present) {
    j.write_int("rttAccuracy-r15", rtt_accuracy_r15);
  }
  j.end_obj();
}

const char* wlan_rtt_r15_s::rtt_units_r15_opts::to_string() const
{
  static const char* options[] = {
      "microseconds", "hundredsofnanoseconds", "tensofnanoseconds", "nanoseconds", "tenthsofnanoseconds"};
  return convert_enum_idx(options, 5, value, "wlan_rtt_r15_s::rtt_units_r15_e_");
}

// LogMeasResultBT-r15 ::= SEQUENCE
SRSASN_CODE log_meas_result_bt_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rssi_bt_r15_present, 1));

  HANDLE_CODE(bt_addr_r15.pack(bref));
  if (rssi_bt_r15_present) {
    HANDLE_CODE(pack_integer(bref, rssi_bt_r15, (int16_t)-128, (int16_t)127));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE log_meas_result_bt_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rssi_bt_r15_present, 1));

  HANDLE_CODE(bt_addr_r15.unpack(bref));
  if (rssi_bt_r15_present) {
    HANDLE_CODE(unpack_integer(rssi_bt_r15, bref, (int16_t)-128, (int16_t)127));
  }

  return SRSASN_SUCCESS;
}
void log_meas_result_bt_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("bt-Addr-r15", bt_addr_r15.to_string());
  if (rssi_bt_r15_present) {
    j.write_int("rssi-BT-r15", rssi_bt_r15);
  }
  j.end_obj();
}

// LogMeasResultWLAN-r15 ::= SEQUENCE
SRSASN_CODE log_meas_result_wlan_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rssi_wlan_r15_present, 1));
  HANDLE_CODE(bref.pack(rtt_wlan_r15_present, 1));

  HANDLE_CODE(wlan_ids_r15.pack(bref));
  if (rssi_wlan_r15_present) {
    HANDLE_CODE(pack_integer(bref, rssi_wlan_r15, (uint8_t)0u, (uint8_t)141u));
  }
  if (rtt_wlan_r15_present) {
    HANDLE_CODE(rtt_wlan_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE log_meas_result_wlan_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rssi_wlan_r15_present, 1));
  HANDLE_CODE(bref.unpack(rtt_wlan_r15_present, 1));

  HANDLE_CODE(wlan_ids_r15.unpack(bref));
  if (rssi_wlan_r15_present) {
    HANDLE_CODE(unpack_integer(rssi_wlan_r15, bref, (uint8_t)0u, (uint8_t)141u));
  }
  if (rtt_wlan_r15_present) {
    HANDLE_CODE(rtt_wlan_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void log_meas_result_wlan_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("wlan-Identifiers-r15");
  wlan_ids_r15.to_json(j);
  if (rssi_wlan_r15_present) {
    j.write_int("rssiWLAN-r15", rssi_wlan_r15);
  }
  if (rtt_wlan_r15_present) {
    j.write_fieldname("rtt-WLAN-r15");
    rtt_wlan_r15.to_json(j);
  }
  j.end_obj();
}

// MeasResult2CDMA2000-r9 ::= SEQUENCE
SRSASN_CODE meas_result2_cdma2000_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(carrier_freq_r9.pack(bref));
  HANDLE_CODE(meas_result_list_r9.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result2_cdma2000_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(carrier_freq_r9.unpack(bref));
  HANDLE_CODE(meas_result_list_r9.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_result2_cdma2000_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("carrierFreq-r9");
  carrier_freq_r9.to_json(j);
  j.write_fieldname("measResultList-r9");
  meas_result_list_r9.to_json(j);
  j.end_obj();
}

// MeasResult2EUTRA-r9 ::= SEQUENCE
SRSASN_CODE meas_result2_eutra_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, carrier_freq_r9, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_list_r9, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result2_eutra_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(carrier_freq_r9, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_list_r9, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void meas_result2_eutra_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r9", carrier_freq_r9);
  j.start_array("measResultList-r9");
  for (const auto& e1 : meas_result_list_r9) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MeasResult2EUTRA-v1250 ::= SEQUENCE
SRSASN_CODE meas_result2_eutra_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrq_type_r12_present, 1));

  if (rsrq_type_r12_present) {
    HANDLE_CODE(rsrq_type_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result2_eutra_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrq_type_r12_present, 1));

  if (rsrq_type_r12_present) {
    HANDLE_CODE(rsrq_type_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_result2_eutra_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rsrq_type_r12_present) {
    j.write_fieldname("rsrq-Type-r12");
    rsrq_type_r12.to_json(j);
  }
  j.end_obj();
}

// MeasResult2EUTRA-v9e0 ::= SEQUENCE
SRSASN_CODE meas_result2_eutra_v9e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(carrier_freq_v9e0_present, 1));

  if (carrier_freq_v9e0_present) {
    HANDLE_CODE(pack_integer(bref, carrier_freq_v9e0, (uint32_t)65536u, (uint32_t)262143u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result2_eutra_v9e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(carrier_freq_v9e0_present, 1));

  if (carrier_freq_v9e0_present) {
    HANDLE_CODE(unpack_integer(carrier_freq_v9e0, bref, (uint32_t)65536u, (uint32_t)262143u));
  }

  return SRSASN_SUCCESS;
}
void meas_result2_eutra_v9e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_freq_v9e0_present) {
    j.write_int("carrierFreq-v9e0", carrier_freq_v9e0);
  }
  j.end_obj();
}

// MeasResult2UTRA-r9 ::= SEQUENCE
SRSASN_CODE meas_result2_utra_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, carrier_freq_r9, (uint16_t)0u, (uint16_t)16383u));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_list_r9, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result2_utra_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(carrier_freq_r9, bref, (uint16_t)0u, (uint16_t)16383u));
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_list_r9, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void meas_result2_utra_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r9", carrier_freq_r9);
  j.start_array("measResultList-r9");
  for (const auto& e1 : meas_result_list_r9) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MeasResultMBSFN-r12 ::= SEQUENCE
SRSASN_CODE meas_result_mbsfn_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sig_bler_result_r12_present, 1));
  HANDLE_CODE(bref.pack(data_bler_mch_result_list_r12_present, 1));

  HANDLE_CODE(pack_integer(bref, mbsfn_area_r12.mbsfn_area_id_r12, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(pack_integer(bref, mbsfn_area_r12.carrier_freq_r12, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(pack_integer(bref, rsrp_result_mbsfn_r12, (uint8_t)0u, (uint8_t)97u));
  HANDLE_CODE(pack_integer(bref, rsrq_result_mbsfn_r12, (uint8_t)0u, (uint8_t)31u));
  if (sig_bler_result_r12_present) {
    HANDLE_CODE(sig_bler_result_r12.pack(bref));
  }
  if (data_bler_mch_result_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, data_bler_mch_result_list_r12, 1, 15));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_mbsfn_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sig_bler_result_r12_present, 1));
  HANDLE_CODE(bref.unpack(data_bler_mch_result_list_r12_present, 1));

  HANDLE_CODE(unpack_integer(mbsfn_area_r12.mbsfn_area_id_r12, bref, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(unpack_integer(mbsfn_area_r12.carrier_freq_r12, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(unpack_integer(rsrp_result_mbsfn_r12, bref, (uint8_t)0u, (uint8_t)97u));
  HANDLE_CODE(unpack_integer(rsrq_result_mbsfn_r12, bref, (uint8_t)0u, (uint8_t)31u));
  if (sig_bler_result_r12_present) {
    HANDLE_CODE(sig_bler_result_r12.unpack(bref));
  }
  if (data_bler_mch_result_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(data_bler_mch_result_list_r12, bref, 1, 15));
  }

  return SRSASN_SUCCESS;
}
void meas_result_mbsfn_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mbsfn-Area-r12");
  j.start_obj();
  j.write_int("mbsfn-AreaId-r12", mbsfn_area_r12.mbsfn_area_id_r12);
  j.write_int("carrierFreq-r12", mbsfn_area_r12.carrier_freq_r12);
  j.end_obj();
  j.write_int("rsrpResultMBSFN-r12", rsrp_result_mbsfn_r12);
  j.write_int("rsrqResultMBSFN-r12", rsrq_result_mbsfn_r12);
  if (sig_bler_result_r12_present) {
    j.write_fieldname("signallingBLER-Result-r12");
    sig_bler_result_r12.to_json(j);
  }
  if (data_bler_mch_result_list_r12_present) {
    j.start_array("dataBLER-MCH-ResultList-r12");
    for (const auto& e1 : data_bler_mch_result_list_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// MeasResultSSB-Index-r15 ::= SEQUENCE
SRSASN_CODE meas_result_ssb_idx_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_ssb_idx_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, ssb_idx_r15, (uint8_t)0u, (uint8_t)63u));
  if (meas_result_ssb_idx_r15_present) {
    HANDLE_CODE(meas_result_ssb_idx_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_ssb_idx_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_ssb_idx_r15_present, 1));

  HANDLE_CODE(unpack_integer(ssb_idx_r15, bref, (uint8_t)0u, (uint8_t)63u));
  if (meas_result_ssb_idx_r15_present) {
    HANDLE_CODE(meas_result_ssb_idx_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_result_ssb_idx_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssb-Index-r15", ssb_idx_r15);
  if (meas_result_ssb_idx_r15_present) {
    j.write_fieldname("measResultSSB-Index-r15");
    meas_result_ssb_idx_r15.to_json(j);
  }
  j.end_obj();
}

// CGI-InfoNR-r15 ::= SEQUENCE
SRSASN_CODE cgi_info_nr_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(plmn_id_info_list_r15_present, 1));
  HANDLE_CODE(bref.pack(freq_band_list_r15_present, 1));
  HANDLE_CODE(bref.pack(no_sib1_r15_present, 1));

  if (plmn_id_info_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_info_list_r15, 1, 12));
  }
  if (freq_band_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list_r15, 1, 32, integer_packer<uint16_t>(1, 1024)));
  }
  if (no_sib1_r15_present) {
    HANDLE_CODE(pack_integer(bref, no_sib1_r15.ssb_subcarrier_offset_r15, (uint8_t)0u, (uint8_t)15u));
    HANDLE_CODE(pack_integer(bref, no_sib1_r15.pdcch_cfg_sib1_r15, (uint16_t)0u, (uint16_t)255u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cgi_info_nr_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(plmn_id_info_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(freq_band_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(no_sib1_r15_present, 1));

  if (plmn_id_info_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(plmn_id_info_list_r15, bref, 1, 12));
  }
  if (freq_band_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list_r15, bref, 1, 32, integer_packer<uint16_t>(1, 1024)));
  }
  if (no_sib1_r15_present) {
    HANDLE_CODE(unpack_integer(no_sib1_r15.ssb_subcarrier_offset_r15, bref, (uint8_t)0u, (uint8_t)15u));
    HANDLE_CODE(unpack_integer(no_sib1_r15.pdcch_cfg_sib1_r15, bref, (uint16_t)0u, (uint16_t)255u));
  }

  return SRSASN_SUCCESS;
}
void cgi_info_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_info_list_r15_present) {
    j.start_array("plmn-IdentityInfoList-r15");
    for (const auto& e1 : plmn_id_info_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (freq_band_list_r15_present) {
    j.start_array("frequencyBandList-r15");
    for (const auto& e1 : freq_band_list_r15) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (no_sib1_r15_present) {
    j.write_fieldname("noSIB1-r15");
    j.start_obj();
    j.write_int("ssb-SubcarrierOffset-r15", no_sib1_r15.ssb_subcarrier_offset_r15);
    j.write_int("pdcch-ConfigSIB1-r15", no_sib1_r15.pdcch_cfg_sib1_r15);
    j.end_obj();
  }
  j.end_obj();
}

// LogMeasInfo-r10 ::= SEQUENCE
SRSASN_CODE log_meas_info_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(location_info_r10_present, 1));
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_r10_present, 1));

  if (location_info_r10_present) {
    HANDLE_CODE(location_info_r10.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, relative_time_stamp_r10, (uint16_t)0u, (uint16_t)7200u));
  HANDLE_CODE(serv_cell_id_r10.pack(bref));
  HANDLE_CODE(pack_integer(bref, meas_result_serv_cell_r10.rsrp_result_r10, (uint8_t)0u, (uint8_t)97u));
  HANDLE_CODE(pack_integer(bref, meas_result_serv_cell_r10.rsrq_result_r10, (uint8_t)0u, (uint8_t)34u));
  if (meas_result_neigh_cells_r10_present) {
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r10.meas_result_list_eutra_r10_present, 1));
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r10.meas_result_list_utra_r10_present, 1));
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r10.meas_result_list_geran_r10_present, 1));
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r10.meas_result_list_cdma2000_r10_present, 1));
    if (meas_result_neigh_cells_r10.meas_result_list_eutra_r10_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r10.meas_result_list_eutra_r10, 1, 8));
    }
    if (meas_result_neigh_cells_r10.meas_result_list_utra_r10_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r10.meas_result_list_utra_r10, 1, 8));
    }
    if (meas_result_neigh_cells_r10.meas_result_list_geran_r10_present) {
      HANDLE_CODE(pack_dyn_seq_of(
          bref, meas_result_neigh_cells_r10.meas_result_list_geran_r10, 1, 3, SeqOfPacker<Packer>(1, 8, Packer())));
    }
    if (meas_result_neigh_cells_r10.meas_result_list_cdma2000_r10_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r10.meas_result_list_cdma2000_r10, 1, 8));
    }
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_result_list_eutra_v1090.is_present();
    group_flags[1] |= meas_result_list_mbsfn_r12.is_present();
    group_flags[1] |= meas_result_serv_cell_v1250_present;
    group_flags[1] |= serv_cell_rsrq_type_r12.is_present();
    group_flags[1] |= meas_result_list_eutra_v1250.is_present();
    group_flags[2] |= in_dev_coex_detected_r13_present;
    group_flags[3] |= meas_result_serv_cell_v1360_present;
    group_flags[4] |= log_meas_result_list_bt_r15.is_present();
    group_flags[4] |= log_meas_result_list_wlan_r15.is_present();
    group_flags[5] |= any_cell_sel_detected_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_list_eutra_v1090.is_present(), 1));
      if (meas_result_list_eutra_v1090.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_list_eutra_v1090, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_list_mbsfn_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_serv_cell_v1250_present, 1));
      HANDLE_CODE(bref.pack(serv_cell_rsrq_type_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_list_eutra_v1250.is_present(), 1));
      if (meas_result_list_mbsfn_r12.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_list_mbsfn_r12, 1, 8));
      }
      if (meas_result_serv_cell_v1250_present) {
        HANDLE_CODE(pack_integer(bref, meas_result_serv_cell_v1250, (int8_t)-30, (int8_t)46));
      }
      if (serv_cell_rsrq_type_r12.is_present()) {
        HANDLE_CODE(serv_cell_rsrq_type_r12->pack(bref));
      }
      if (meas_result_list_eutra_v1250.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_list_eutra_v1250, 1, 8));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(in_dev_coex_detected_r13_present, 1));
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_serv_cell_v1360_present, 1));
      if (meas_result_serv_cell_v1360_present) {
        HANDLE_CODE(pack_integer(bref, meas_result_serv_cell_v1360, (int8_t)-17, (int8_t)-1));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(log_meas_result_list_bt_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(log_meas_result_list_wlan_r15.is_present(), 1));
      if (log_meas_result_list_bt_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *log_meas_result_list_bt_r15, 1, 32));
      }
      if (log_meas_result_list_wlan_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *log_meas_result_list_wlan_r15, 1, 32));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(any_cell_sel_detected_r15_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE log_meas_info_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(location_info_r10_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r10_present, 1));

  if (location_info_r10_present) {
    HANDLE_CODE(location_info_r10.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(relative_time_stamp_r10, bref, (uint16_t)0u, (uint16_t)7200u));
  HANDLE_CODE(serv_cell_id_r10.unpack(bref));
  HANDLE_CODE(unpack_integer(meas_result_serv_cell_r10.rsrp_result_r10, bref, (uint8_t)0u, (uint8_t)97u));
  HANDLE_CODE(unpack_integer(meas_result_serv_cell_r10.rsrq_result_r10, bref, (uint8_t)0u, (uint8_t)34u));
  if (meas_result_neigh_cells_r10_present) {
    HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r10.meas_result_list_eutra_r10_present, 1));
    HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r10.meas_result_list_utra_r10_present, 1));
    HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r10.meas_result_list_geran_r10_present, 1));
    HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r10.meas_result_list_cdma2000_r10_present, 1));
    if (meas_result_neigh_cells_r10.meas_result_list_eutra_r10_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r10.meas_result_list_eutra_r10, bref, 1, 8));
    }
    if (meas_result_neigh_cells_r10.meas_result_list_utra_r10_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r10.meas_result_list_utra_r10, bref, 1, 8));
    }
    if (meas_result_neigh_cells_r10.meas_result_list_geran_r10_present) {
      HANDLE_CODE(unpack_dyn_seq_of(
          meas_result_neigh_cells_r10.meas_result_list_geran_r10, bref, 1, 3, SeqOfPacker<Packer>(1, 8, Packer())));
    }
    if (meas_result_neigh_cells_r10.meas_result_list_cdma2000_r10_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r10.meas_result_list_cdma2000_r10, bref, 1, 8));
    }
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(6);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_result_list_eutra_v1090_present;
      HANDLE_CODE(bref.unpack(meas_result_list_eutra_v1090_present, 1));
      meas_result_list_eutra_v1090.set_present(meas_result_list_eutra_v1090_present);
      if (meas_result_list_eutra_v1090.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_list_eutra_v1090, bref, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_result_list_mbsfn_r12_present;
      HANDLE_CODE(bref.unpack(meas_result_list_mbsfn_r12_present, 1));
      meas_result_list_mbsfn_r12.set_present(meas_result_list_mbsfn_r12_present);
      HANDLE_CODE(bref.unpack(meas_result_serv_cell_v1250_present, 1));
      bool serv_cell_rsrq_type_r12_present;
      HANDLE_CODE(bref.unpack(serv_cell_rsrq_type_r12_present, 1));
      serv_cell_rsrq_type_r12.set_present(serv_cell_rsrq_type_r12_present);
      bool meas_result_list_eutra_v1250_present;
      HANDLE_CODE(bref.unpack(meas_result_list_eutra_v1250_present, 1));
      meas_result_list_eutra_v1250.set_present(meas_result_list_eutra_v1250_present);
      if (meas_result_list_mbsfn_r12.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_list_mbsfn_r12, bref, 1, 8));
      }
      if (meas_result_serv_cell_v1250_present) {
        HANDLE_CODE(unpack_integer(meas_result_serv_cell_v1250, bref, (int8_t)-30, (int8_t)46));
      }
      if (serv_cell_rsrq_type_r12.is_present()) {
        HANDLE_CODE(serv_cell_rsrq_type_r12->unpack(bref));
      }
      if (meas_result_list_eutra_v1250.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_list_eutra_v1250, bref, 1, 8));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(in_dev_coex_detected_r13_present, 1));
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(meas_result_serv_cell_v1360_present, 1));
      if (meas_result_serv_cell_v1360_present) {
        HANDLE_CODE(unpack_integer(meas_result_serv_cell_v1360, bref, (int8_t)-17, (int8_t)-1));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool log_meas_result_list_bt_r15_present;
      HANDLE_CODE(bref.unpack(log_meas_result_list_bt_r15_present, 1));
      log_meas_result_list_bt_r15.set_present(log_meas_result_list_bt_r15_present);
      bool log_meas_result_list_wlan_r15_present;
      HANDLE_CODE(bref.unpack(log_meas_result_list_wlan_r15_present, 1));
      log_meas_result_list_wlan_r15.set_present(log_meas_result_list_wlan_r15_present);
      if (log_meas_result_list_bt_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*log_meas_result_list_bt_r15, bref, 1, 32));
      }
      if (log_meas_result_list_wlan_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*log_meas_result_list_wlan_r15, bref, 1, 32));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(any_cell_sel_detected_r15_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void log_meas_info_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (location_info_r10_present) {
    j.write_fieldname("locationInfo-r10");
    location_info_r10.to_json(j);
  }
  j.write_int("relativeTimeStamp-r10", relative_time_stamp_r10);
  j.write_fieldname("servCellIdentity-r10");
  serv_cell_id_r10.to_json(j);
  j.write_fieldname("measResultServCell-r10");
  j.start_obj();
  j.write_int("rsrpResult-r10", meas_result_serv_cell_r10.rsrp_result_r10);
  j.write_int("rsrqResult-r10", meas_result_serv_cell_r10.rsrq_result_r10);
  j.end_obj();
  if (meas_result_neigh_cells_r10_present) {
    j.write_fieldname("measResultNeighCells-r10");
    j.start_obj();
    if (meas_result_neigh_cells_r10.meas_result_list_eutra_r10_present) {
      j.start_array("measResultListEUTRA-r10");
      for (const auto& e1 : meas_result_neigh_cells_r10.meas_result_list_eutra_r10) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_neigh_cells_r10.meas_result_list_utra_r10_present) {
      j.start_array("measResultListUTRA-r10");
      for (const auto& e1 : meas_result_neigh_cells_r10.meas_result_list_utra_r10) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_neigh_cells_r10.meas_result_list_geran_r10_present) {
      j.start_array("measResultListGERAN-r10");
      for (const auto& e1 : meas_result_neigh_cells_r10.meas_result_list_geran_r10) {
        j.start_array();
        for (const auto& e2 : e1) {
          e2.to_json(j);
        }
        j.end_array();
      }
      j.end_array();
    }
    if (meas_result_neigh_cells_r10.meas_result_list_cdma2000_r10_present) {
      j.start_array("measResultListCDMA2000-r10");
      for (const auto& e1 : meas_result_neigh_cells_r10.meas_result_list_cdma2000_r10) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  if (ext) {
    if (meas_result_list_eutra_v1090.is_present()) {
      j.start_array("measResultListEUTRA-v1090");
      for (const auto& e1 : *meas_result_list_eutra_v1090) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_list_mbsfn_r12.is_present()) {
      j.start_array("measResultListMBSFN-r12");
      for (const auto& e1 : *meas_result_list_mbsfn_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_serv_cell_v1250_present) {
      j.write_int("measResultServCell-v1250", meas_result_serv_cell_v1250);
    }
    if (serv_cell_rsrq_type_r12.is_present()) {
      j.write_fieldname("servCellRSRQ-Type-r12");
      serv_cell_rsrq_type_r12->to_json(j);
    }
    if (meas_result_list_eutra_v1250.is_present()) {
      j.start_array("measResultListEUTRA-v1250");
      for (const auto& e1 : *meas_result_list_eutra_v1250) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (in_dev_coex_detected_r13_present) {
      j.write_str("inDeviceCoexDetected-r13", "true");
    }
    if (meas_result_serv_cell_v1360_present) {
      j.write_int("measResultServCell-v1360", meas_result_serv_cell_v1360);
    }
    if (log_meas_result_list_bt_r15.is_present()) {
      j.start_array("logMeasResultListBT-r15");
      for (const auto& e1 : *log_meas_result_list_bt_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (log_meas_result_list_wlan_r15.is_present()) {
      j.start_array("logMeasResultListWLAN-r15");
      for (const auto& e1 : *log_meas_result_list_wlan_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (any_cell_sel_detected_r15_present) {
      j.write_str("anyCellSelectionDetected-r15", "true");
    }
  }
  j.end_obj();
}

// MeasResultCellNR-r15 ::= SEQUENCE
SRSASN_CODE meas_result_cell_nr_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_rs_idx_list_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, pci_r15, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(meas_result_cell_r15.pack(bref));
  if (meas_result_rs_idx_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_rs_idx_list_r15, 1, 32));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= cgi_info_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cgi_info_r15.is_present(), 1));
      if (cgi_info_r15.is_present()) {
        HANDLE_CODE(cgi_info_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_cell_nr_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_rs_idx_list_r15_present, 1));

  HANDLE_CODE(unpack_integer(pci_r15, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(meas_result_cell_r15.unpack(bref));
  if (meas_result_rs_idx_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_rs_idx_list_r15, bref, 1, 32));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cgi_info_r15_present;
      HANDLE_CODE(bref.unpack(cgi_info_r15_present, 1));
      cgi_info_r15.set_present(cgi_info_r15_present);
      if (cgi_info_r15.is_present()) {
        HANDLE_CODE(cgi_info_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_result_cell_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pci-r15", pci_r15);
  j.write_fieldname("measResultCell-r15");
  meas_result_cell_r15.to_json(j);
  if (meas_result_rs_idx_list_r15_present) {
    j.start_array("measResultRS-IndexList-r15");
    for (const auto& e1 : meas_result_rs_idx_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (cgi_info_r15.is_present()) {
      j.write_fieldname("cgi-Info-r15");
      cgi_info_r15->to_json(j);
    }
  }
  j.end_obj();
}

// LogMeasReport-r10 ::= SEQUENCE
SRSASN_CODE log_meas_report_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(log_meas_available_r10_present, 1));

  HANDLE_CODE(absolute_time_stamp_r10.pack(bref));
  HANDLE_CODE(trace_ref_r10.pack(bref));
  HANDLE_CODE(trace_recording_session_ref_r10.pack(bref));
  HANDLE_CODE(tce_id_r10.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, log_meas_info_list_r10, 1, 520));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= log_meas_available_bt_r15_present;
    group_flags[0] |= log_meas_available_wlan_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(log_meas_available_bt_r15_present, 1));
      HANDLE_CODE(bref.pack(log_meas_available_wlan_r15_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE log_meas_report_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(log_meas_available_r10_present, 1));

  HANDLE_CODE(absolute_time_stamp_r10.unpack(bref));
  HANDLE_CODE(trace_ref_r10.unpack(bref));
  HANDLE_CODE(trace_recording_session_ref_r10.unpack(bref));
  HANDLE_CODE(tce_id_r10.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(log_meas_info_list_r10, bref, 1, 520));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(log_meas_available_bt_r15_present, 1));
      HANDLE_CODE(bref.unpack(log_meas_available_wlan_r15_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void log_meas_report_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("absoluteTimeStamp-r10", absolute_time_stamp_r10.to_string());
  j.write_fieldname("traceReference-r10");
  trace_ref_r10.to_json(j);
  j.write_str("traceRecordingSessionRef-r10", trace_recording_session_ref_r10.to_string());
  j.write_str("tce-Id-r10", tce_id_r10.to_string());
  j.start_array("logMeasInfoList-r10");
  for (const auto& e1 : log_meas_info_list_r10) {
    e1.to_json(j);
  }
  j.end_array();
  if (log_meas_available_r10_present) {
    j.write_str("logMeasAvailable-r10", "true");
  }
  if (ext) {
    if (log_meas_available_bt_r15_present) {
      j.write_str("logMeasAvailableBT-r15", "true");
    }
    if (log_meas_available_wlan_r15_present) {
      j.write_str("logMeasAvailableWLAN-r15", "true");
    }
  }
  j.end_obj();
}

// MeasResultCBR-r14 ::= SEQUENCE
SRSASN_CODE meas_result_cbr_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cbr_pscch_r14_present, 1));

  HANDLE_CODE(pack_integer(bref, pool_id_r14, (uint8_t)1u, (uint8_t)72u));
  HANDLE_CODE(pack_integer(bref, cbr_pssch_r14, (uint8_t)0u, (uint8_t)100u));
  if (cbr_pscch_r14_present) {
    HANDLE_CODE(pack_integer(bref, cbr_pscch_r14, (uint8_t)0u, (uint8_t)100u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_cbr_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cbr_pscch_r14_present, 1));

  HANDLE_CODE(unpack_integer(pool_id_r14, bref, (uint8_t)1u, (uint8_t)72u));
  HANDLE_CODE(unpack_integer(cbr_pssch_r14, bref, (uint8_t)0u, (uint8_t)100u));
  if (cbr_pscch_r14_present) {
    HANDLE_CODE(unpack_integer(cbr_pscch_r14, bref, (uint8_t)0u, (uint8_t)100u));
  }

  return SRSASN_SUCCESS;
}
void meas_result_cbr_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("poolIdentity-r14", pool_id_r14);
  j.write_int("cbr-PSSCH-r14", cbr_pssch_r14);
  if (cbr_pscch_r14_present) {
    j.write_int("cbr-PSCCH-r14", cbr_pscch_r14);
  }
  j.end_obj();
}

// MeasResultCSI-RS-r12 ::= SEQUENCE
SRSASN_CODE meas_result_csi_rs_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, meas_csi_rs_id_r12, (uint8_t)1u, (uint8_t)96u));
  HANDLE_CODE(pack_integer(bref, csi_rsrp_result_r12, (uint8_t)0u, (uint8_t)97u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_csi_rs_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(meas_csi_rs_id_r12, bref, (uint8_t)1u, (uint8_t)96u));
  HANDLE_CODE(unpack_integer(csi_rsrp_result_r12, bref, (uint8_t)0u, (uint8_t)97u));

  return SRSASN_SUCCESS;
}
void meas_result_csi_rs_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measCSI-RS-Id-r12", meas_csi_rs_id_r12);
  j.write_int("csi-RSRP-Result-r12", csi_rsrp_result_r12);
  j.end_obj();
}

// MeasResultCellSFTD-r15 ::= SEQUENCE
SRSASN_CODE meas_result_cell_sftd_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrp_result_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, pci_r15, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(pack_integer(bref, sfn_offset_result_r15, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(pack_integer(bref, frame_boundary_offset_result_r15, (int16_t)-30720, (int16_t)30719));
  if (rsrp_result_r15_present) {
    HANDLE_CODE(pack_integer(bref, rsrp_result_r15, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_cell_sftd_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrp_result_r15_present, 1));

  HANDLE_CODE(unpack_integer(pci_r15, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(unpack_integer(sfn_offset_result_r15, bref, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(unpack_integer(frame_boundary_offset_result_r15, bref, (int16_t)-30720, (int16_t)30719));
  if (rsrp_result_r15_present) {
    HANDLE_CODE(unpack_integer(rsrp_result_r15, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void meas_result_cell_sftd_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r15", pci_r15);
  j.write_int("sfn-OffsetResult-r15", sfn_offset_result_r15);
  j.write_int("frameBoundaryOffsetResult-r15", frame_boundary_offset_result_r15);
  if (rsrp_result_r15_present) {
    j.write_int("rsrpResult-r15", rsrp_result_r15);
  }
  j.end_obj();
}

// MeasResultServFreq-r10 ::= SEQUENCE
SRSASN_CODE meas_result_serv_freq_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_scell_r10_present, 1));
  HANDLE_CODE(bref.pack(meas_result_best_neigh_cell_r10_present, 1));

  HANDLE_CODE(pack_integer(bref, serv_freq_id_r10, (uint8_t)0u, (uint8_t)7u));
  if (meas_result_scell_r10_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_scell_r10.rsrp_result_scell_r10, (uint8_t)0u, (uint8_t)97u));
    HANDLE_CODE(pack_integer(bref, meas_result_scell_r10.rsrq_result_scell_r10, (uint8_t)0u, (uint8_t)34u));
  }
  if (meas_result_best_neigh_cell_r10_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_best_neigh_cell_r10.pci_r10, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(pack_integer(bref, meas_result_best_neigh_cell_r10.rsrp_result_ncell_r10, (uint8_t)0u, (uint8_t)97u));
    HANDLE_CODE(pack_integer(bref, meas_result_best_neigh_cell_r10.rsrq_result_ncell_r10, (uint8_t)0u, (uint8_t)34u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_result_scell_v1250_present;
    group_flags[0] |= meas_result_best_neigh_cell_v1250_present;
    group_flags[1] |= meas_result_scell_v1310.is_present();
    group_flags[1] |= meas_result_best_neigh_cell_v1310.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_scell_v1250_present, 1));
      HANDLE_CODE(bref.pack(meas_result_best_neigh_cell_v1250_present, 1));
      if (meas_result_scell_v1250_present) {
        HANDLE_CODE(pack_integer(bref, meas_result_scell_v1250, (int8_t)-30, (int8_t)46));
      }
      if (meas_result_best_neigh_cell_v1250_present) {
        HANDLE_CODE(pack_integer(bref, meas_result_best_neigh_cell_v1250, (int8_t)-30, (int8_t)46));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_scell_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_best_neigh_cell_v1310.is_present(), 1));
      if (meas_result_scell_v1310.is_present()) {
        HANDLE_CODE(pack_integer(bref, meas_result_scell_v1310->rs_sinr_result_r13, (uint8_t)0u, (uint8_t)127u));
      }
      if (meas_result_best_neigh_cell_v1310.is_present()) {
        HANDLE_CODE(
            pack_integer(bref, meas_result_best_neigh_cell_v1310->rs_sinr_result_r13, (uint8_t)0u, (uint8_t)127u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_serv_freq_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_scell_r10_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_best_neigh_cell_r10_present, 1));

  HANDLE_CODE(unpack_integer(serv_freq_id_r10, bref, (uint8_t)0u, (uint8_t)7u));
  if (meas_result_scell_r10_present) {
    HANDLE_CODE(unpack_integer(meas_result_scell_r10.rsrp_result_scell_r10, bref, (uint8_t)0u, (uint8_t)97u));
    HANDLE_CODE(unpack_integer(meas_result_scell_r10.rsrq_result_scell_r10, bref, (uint8_t)0u, (uint8_t)34u));
  }
  if (meas_result_best_neigh_cell_r10_present) {
    HANDLE_CODE(unpack_integer(meas_result_best_neigh_cell_r10.pci_r10, bref, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(unpack_integer(meas_result_best_neigh_cell_r10.rsrp_result_ncell_r10, bref, (uint8_t)0u, (uint8_t)97u));
    HANDLE_CODE(unpack_integer(meas_result_best_neigh_cell_r10.rsrq_result_ncell_r10, bref, (uint8_t)0u, (uint8_t)34u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(meas_result_scell_v1250_present, 1));
      HANDLE_CODE(bref.unpack(meas_result_best_neigh_cell_v1250_present, 1));
      if (meas_result_scell_v1250_present) {
        HANDLE_CODE(unpack_integer(meas_result_scell_v1250, bref, (int8_t)-30, (int8_t)46));
      }
      if (meas_result_best_neigh_cell_v1250_present) {
        HANDLE_CODE(unpack_integer(meas_result_best_neigh_cell_v1250, bref, (int8_t)-30, (int8_t)46));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_result_scell_v1310_present;
      HANDLE_CODE(bref.unpack(meas_result_scell_v1310_present, 1));
      meas_result_scell_v1310.set_present(meas_result_scell_v1310_present);
      bool meas_result_best_neigh_cell_v1310_present;
      HANDLE_CODE(bref.unpack(meas_result_best_neigh_cell_v1310_present, 1));
      meas_result_best_neigh_cell_v1310.set_present(meas_result_best_neigh_cell_v1310_present);
      if (meas_result_scell_v1310.is_present()) {
        HANDLE_CODE(unpack_integer(meas_result_scell_v1310->rs_sinr_result_r13, bref, (uint8_t)0u, (uint8_t)127u));
      }
      if (meas_result_best_neigh_cell_v1310.is_present()) {
        HANDLE_CODE(
            unpack_integer(meas_result_best_neigh_cell_v1310->rs_sinr_result_r13, bref, (uint8_t)0u, (uint8_t)127u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_result_serv_freq_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servFreqId-r10", serv_freq_id_r10);
  if (meas_result_scell_r10_present) {
    j.write_fieldname("measResultSCell-r10");
    j.start_obj();
    j.write_int("rsrpResultSCell-r10", meas_result_scell_r10.rsrp_result_scell_r10);
    j.write_int("rsrqResultSCell-r10", meas_result_scell_r10.rsrq_result_scell_r10);
    j.end_obj();
  }
  if (meas_result_best_neigh_cell_r10_present) {
    j.write_fieldname("measResultBestNeighCell-r10");
    j.start_obj();
    j.write_int("physCellId-r10", meas_result_best_neigh_cell_r10.pci_r10);
    j.write_int("rsrpResultNCell-r10", meas_result_best_neigh_cell_r10.rsrp_result_ncell_r10);
    j.write_int("rsrqResultNCell-r10", meas_result_best_neigh_cell_r10.rsrq_result_ncell_r10);
    j.end_obj();
  }
  if (ext) {
    if (meas_result_scell_v1250_present) {
      j.write_int("measResultSCell-v1250", meas_result_scell_v1250);
    }
    if (meas_result_best_neigh_cell_v1250_present) {
      j.write_int("measResultBestNeighCell-v1250", meas_result_best_neigh_cell_v1250);
    }
    if (meas_result_scell_v1310.is_present()) {
      j.write_fieldname("measResultSCell-v1310");
      j.start_obj();
      j.write_int("rs-sinr-Result-r13", meas_result_scell_v1310->rs_sinr_result_r13);
      j.end_obj();
    }
    if (meas_result_best_neigh_cell_v1310.is_present()) {
      j.write_fieldname("measResultBestNeighCell-v1310");
      j.start_obj();
      j.write_int("rs-sinr-Result-r13", meas_result_best_neigh_cell_v1310->rs_sinr_result_r13);
      j.end_obj();
    }
  }
  j.end_obj();
}

// MeasResultServFreq-r13 ::= SEQUENCE
SRSASN_CODE meas_result_serv_freq_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_scell_r13_present, 1));
  HANDLE_CODE(bref.pack(meas_result_best_neigh_cell_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, serv_freq_id_r13, (uint8_t)0u, (uint8_t)31u));
  if (meas_result_scell_r13_present) {
    HANDLE_CODE(bref.pack(meas_result_scell_r13.rs_sinr_result_r13_present, 1));
    HANDLE_CODE(pack_integer(bref, meas_result_scell_r13.rsrp_result_scell_r13, (uint8_t)0u, (uint8_t)97u));
    HANDLE_CODE(pack_integer(bref, meas_result_scell_r13.rsrq_result_scell_r13, (int8_t)-30, (int8_t)46));
    if (meas_result_scell_r13.rs_sinr_result_r13_present) {
      HANDLE_CODE(pack_integer(bref, meas_result_scell_r13.rs_sinr_result_r13, (uint8_t)0u, (uint8_t)127u));
    }
  }
  if (meas_result_best_neigh_cell_r13_present) {
    HANDLE_CODE(bref.pack(meas_result_best_neigh_cell_r13.rs_sinr_result_r13_present, 1));
    HANDLE_CODE(pack_integer(bref, meas_result_best_neigh_cell_r13.pci_r13, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(pack_integer(bref, meas_result_best_neigh_cell_r13.rsrp_result_ncell_r13, (uint8_t)0u, (uint8_t)97u));
    HANDLE_CODE(pack_integer(bref, meas_result_best_neigh_cell_r13.rsrq_result_ncell_r13, (int8_t)-30, (int8_t)46));
    if (meas_result_best_neigh_cell_r13.rs_sinr_result_r13_present) {
      HANDLE_CODE(pack_integer(bref, meas_result_best_neigh_cell_r13.rs_sinr_result_r13, (uint8_t)0u, (uint8_t)127u));
    }
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_result_best_neigh_cell_v1360.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_best_neigh_cell_v1360.is_present(), 1));
      if (meas_result_best_neigh_cell_v1360.is_present()) {
        HANDLE_CODE(
            pack_integer(bref, meas_result_best_neigh_cell_v1360->rsrp_result_ncell_v1360, (int8_t)-17, (int8_t)-1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_serv_freq_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_scell_r13_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_best_neigh_cell_r13_present, 1));

  HANDLE_CODE(unpack_integer(serv_freq_id_r13, bref, (uint8_t)0u, (uint8_t)31u));
  if (meas_result_scell_r13_present) {
    HANDLE_CODE(bref.unpack(meas_result_scell_r13.rs_sinr_result_r13_present, 1));
    HANDLE_CODE(unpack_integer(meas_result_scell_r13.rsrp_result_scell_r13, bref, (uint8_t)0u, (uint8_t)97u));
    HANDLE_CODE(unpack_integer(meas_result_scell_r13.rsrq_result_scell_r13, bref, (int8_t)-30, (int8_t)46));
    if (meas_result_scell_r13.rs_sinr_result_r13_present) {
      HANDLE_CODE(unpack_integer(meas_result_scell_r13.rs_sinr_result_r13, bref, (uint8_t)0u, (uint8_t)127u));
    }
  }
  if (meas_result_best_neigh_cell_r13_present) {
    HANDLE_CODE(bref.unpack(meas_result_best_neigh_cell_r13.rs_sinr_result_r13_present, 1));
    HANDLE_CODE(unpack_integer(meas_result_best_neigh_cell_r13.pci_r13, bref, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(unpack_integer(meas_result_best_neigh_cell_r13.rsrp_result_ncell_r13, bref, (uint8_t)0u, (uint8_t)97u));
    HANDLE_CODE(unpack_integer(meas_result_best_neigh_cell_r13.rsrq_result_ncell_r13, bref, (int8_t)-30, (int8_t)46));
    if (meas_result_best_neigh_cell_r13.rs_sinr_result_r13_present) {
      HANDLE_CODE(unpack_integer(meas_result_best_neigh_cell_r13.rs_sinr_result_r13, bref, (uint8_t)0u, (uint8_t)127u));
    }
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_result_best_neigh_cell_v1360_present;
      HANDLE_CODE(bref.unpack(meas_result_best_neigh_cell_v1360_present, 1));
      meas_result_best_neigh_cell_v1360.set_present(meas_result_best_neigh_cell_v1360_present);
      if (meas_result_best_neigh_cell_v1360.is_present()) {
        HANDLE_CODE(
            unpack_integer(meas_result_best_neigh_cell_v1360->rsrp_result_ncell_v1360, bref, (int8_t)-17, (int8_t)-1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_result_serv_freq_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servFreqId-r13", serv_freq_id_r13);
  if (meas_result_scell_r13_present) {
    j.write_fieldname("measResultSCell-r13");
    j.start_obj();
    j.write_int("rsrpResultSCell-r13", meas_result_scell_r13.rsrp_result_scell_r13);
    j.write_int("rsrqResultSCell-r13", meas_result_scell_r13.rsrq_result_scell_r13);
    if (meas_result_scell_r13.rs_sinr_result_r13_present) {
      j.write_int("rs-sinr-Result-r13", meas_result_scell_r13.rs_sinr_result_r13);
    }
    j.end_obj();
  }
  if (meas_result_best_neigh_cell_r13_present) {
    j.write_fieldname("measResultBestNeighCell-r13");
    j.start_obj();
    j.write_int("physCellId-r13", meas_result_best_neigh_cell_r13.pci_r13);
    j.write_int("rsrpResultNCell-r13", meas_result_best_neigh_cell_r13.rsrp_result_ncell_r13);
    j.write_int("rsrqResultNCell-r13", meas_result_best_neigh_cell_r13.rsrq_result_ncell_r13);
    if (meas_result_best_neigh_cell_r13.rs_sinr_result_r13_present) {
      j.write_int("rs-sinr-Result-r13", meas_result_best_neigh_cell_r13.rs_sinr_result_r13);
    }
    j.end_obj();
  }
  if (ext) {
    if (meas_result_best_neigh_cell_v1360.is_present()) {
      j.write_fieldname("measResultBestNeighCell-v1360");
      j.start_obj();
      j.write_int("rsrpResultNCell-v1360", meas_result_best_neigh_cell_v1360->rsrp_result_ncell_v1360);
      j.end_obj();
    }
  }
  j.end_obj();
}

// MeasResultServFreqNR-r15 ::= SEQUENCE
SRSASN_CODE meas_result_serv_freq_nr_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_scell_r15_present, 1));
  HANDLE_CODE(bref.pack(meas_result_best_neigh_cell_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r15, (uint32_t)0u, (uint32_t)3279165u));
  if (meas_result_scell_r15_present) {
    HANDLE_CODE(meas_result_scell_r15.pack(bref));
  }
  if (meas_result_best_neigh_cell_r15_present) {
    HANDLE_CODE(meas_result_best_neigh_cell_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_serv_freq_nr_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_scell_r15_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_best_neigh_cell_r15_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r15, bref, (uint32_t)0u, (uint32_t)3279165u));
  if (meas_result_scell_r15_present) {
    HANDLE_CODE(meas_result_scell_r15.unpack(bref));
  }
  if (meas_result_best_neigh_cell_r15_present) {
    HANDLE_CODE(meas_result_best_neigh_cell_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_result_serv_freq_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r15", carrier_freq_r15);
  if (meas_result_scell_r15_present) {
    j.write_fieldname("measResultSCell-r15");
    meas_result_scell_r15.to_json(j);
  }
  if (meas_result_best_neigh_cell_r15_present) {
    j.write_fieldname("measResultBestNeighCell-r15");
    meas_result_best_neigh_cell_r15.to_json(j);
  }
  j.end_obj();
}

// MeasResultWLAN-r13 ::= SEQUENCE
SRSASN_CODE meas_result_wlan_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(carrier_info_wlan_r13_present, 1));
  HANDLE_CODE(bref.pack(band_wlan_r13_present, 1));
  HANDLE_CODE(bref.pack(available_admission_capacity_wlan_r13_present, 1));
  HANDLE_CODE(bref.pack(backhaul_dl_bw_wlan_r13_present, 1));
  HANDLE_CODE(bref.pack(backhaul_ul_bw_wlan_r13_present, 1));
  HANDLE_CODE(bref.pack(ch_utilization_wlan_r13_present, 1));
  HANDLE_CODE(bref.pack(station_count_wlan_r13_present, 1));
  HANDLE_CODE(bref.pack(connected_wlan_r13_present, 1));

  HANDLE_CODE(wlan_ids_r13.pack(bref));
  if (carrier_info_wlan_r13_present) {
    HANDLE_CODE(carrier_info_wlan_r13.pack(bref));
  }
  if (band_wlan_r13_present) {
    HANDLE_CODE(band_wlan_r13.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, rssi_wlan_r13, (uint8_t)0u, (uint8_t)141u));
  if (available_admission_capacity_wlan_r13_present) {
    HANDLE_CODE(pack_integer(bref, available_admission_capacity_wlan_r13, (uint16_t)0u, (uint16_t)31250u));
  }
  if (backhaul_dl_bw_wlan_r13_present) {
    HANDLE_CODE(backhaul_dl_bw_wlan_r13.pack(bref));
  }
  if (backhaul_ul_bw_wlan_r13_present) {
    HANDLE_CODE(backhaul_ul_bw_wlan_r13.pack(bref));
  }
  if (ch_utilization_wlan_r13_present) {
    HANDLE_CODE(pack_integer(bref, ch_utilization_wlan_r13, (uint16_t)0u, (uint16_t)255u));
  }
  if (station_count_wlan_r13_present) {
    HANDLE_CODE(pack_integer(bref, station_count_wlan_r13, (uint32_t)0u, (uint32_t)65535u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_wlan_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(carrier_info_wlan_r13_present, 1));
  HANDLE_CODE(bref.unpack(band_wlan_r13_present, 1));
  HANDLE_CODE(bref.unpack(available_admission_capacity_wlan_r13_present, 1));
  HANDLE_CODE(bref.unpack(backhaul_dl_bw_wlan_r13_present, 1));
  HANDLE_CODE(bref.unpack(backhaul_ul_bw_wlan_r13_present, 1));
  HANDLE_CODE(bref.unpack(ch_utilization_wlan_r13_present, 1));
  HANDLE_CODE(bref.unpack(station_count_wlan_r13_present, 1));
  HANDLE_CODE(bref.unpack(connected_wlan_r13_present, 1));

  HANDLE_CODE(wlan_ids_r13.unpack(bref));
  if (carrier_info_wlan_r13_present) {
    HANDLE_CODE(carrier_info_wlan_r13.unpack(bref));
  }
  if (band_wlan_r13_present) {
    HANDLE_CODE(band_wlan_r13.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(rssi_wlan_r13, bref, (uint8_t)0u, (uint8_t)141u));
  if (available_admission_capacity_wlan_r13_present) {
    HANDLE_CODE(unpack_integer(available_admission_capacity_wlan_r13, bref, (uint16_t)0u, (uint16_t)31250u));
  }
  if (backhaul_dl_bw_wlan_r13_present) {
    HANDLE_CODE(backhaul_dl_bw_wlan_r13.unpack(bref));
  }
  if (backhaul_ul_bw_wlan_r13_present) {
    HANDLE_CODE(backhaul_ul_bw_wlan_r13.unpack(bref));
  }
  if (ch_utilization_wlan_r13_present) {
    HANDLE_CODE(unpack_integer(ch_utilization_wlan_r13, bref, (uint16_t)0u, (uint16_t)255u));
  }
  if (station_count_wlan_r13_present) {
    HANDLE_CODE(unpack_integer(station_count_wlan_r13, bref, (uint32_t)0u, (uint32_t)65535u));
  }

  return SRSASN_SUCCESS;
}
void meas_result_wlan_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("wlan-Identifiers-r13");
  wlan_ids_r13.to_json(j);
  if (carrier_info_wlan_r13_present) {
    j.write_fieldname("carrierInfoWLAN-r13");
    carrier_info_wlan_r13.to_json(j);
  }
  if (band_wlan_r13_present) {
    j.write_str("bandWLAN-r13", band_wlan_r13.to_string());
  }
  j.write_int("rssiWLAN-r13", rssi_wlan_r13);
  if (available_admission_capacity_wlan_r13_present) {
    j.write_int("availableAdmissionCapacityWLAN-r13", available_admission_capacity_wlan_r13);
  }
  if (backhaul_dl_bw_wlan_r13_present) {
    j.write_str("backhaulDL-BandwidthWLAN-r13", backhaul_dl_bw_wlan_r13.to_string());
  }
  if (backhaul_ul_bw_wlan_r13_present) {
    j.write_str("backhaulUL-BandwidthWLAN-r13", backhaul_ul_bw_wlan_r13.to_string());
  }
  if (ch_utilization_wlan_r13_present) {
    j.write_int("channelUtilizationWLAN-r13", ch_utilization_wlan_r13);
  }
  if (station_count_wlan_r13_present) {
    j.write_int("stationCountWLAN-r13", station_count_wlan_r13);
  }
  if (connected_wlan_r13_present) {
    j.write_str("connectedWLAN-r13", "true");
  }
  j.end_obj();
}

// SensingResult-r15 ::= SEQUENCE
SRSASN_CODE sensing_result_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, res_idx_r15, (uint16_t)1u, (uint16_t)2000u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sensing_result_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(res_idx_r15, bref, (uint16_t)1u, (uint16_t)2000u));

  return SRSASN_SUCCESS;
}
void sensing_result_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("resourceIndex-r15", res_idx_r15);
  j.end_obj();
}

// UL-PDCP-DelayResult-r13 ::= SEQUENCE
SRSASN_CODE ul_pdcp_delay_result_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(qci_id_r13.pack(bref));
  HANDLE_CODE(pack_integer(bref, excess_delay_r13, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pdcp_delay_result_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(qci_id_r13.unpack(bref));
  HANDLE_CODE(unpack_integer(excess_delay_r13, bref, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
void ul_pdcp_delay_result_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("qci-Id-r13", qci_id_r13.to_string());
  j.write_int("excessDelay-r13", excess_delay_r13);
  j.end_obj();
}

const char* ul_pdcp_delay_result_r13_s::qci_id_r13_opts::to_string() const
{
  static const char* options[] = {"qci1", "qci2", "qci3", "qci4", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ul_pdcp_delay_result_r13_s::qci_id_r13_e_");
}
uint8_t ul_pdcp_delay_result_r13_s::qci_id_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4};
  return map_enum_number(options, 4, value, "ul_pdcp_delay_result_r13_s::qci_id_r13_e_");
}

// MeasResultForECID-r9 ::= SEQUENCE
SRSASN_CODE meas_result_for_ecid_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ue_rx_tx_time_diff_result_r9, (uint16_t)0u, (uint16_t)4095u));
  HANDLE_CODE(current_sfn_r9.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_for_ecid_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ue_rx_tx_time_diff_result_r9, bref, (uint16_t)0u, (uint16_t)4095u));
  HANDLE_CODE(current_sfn_r9.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_result_for_ecid_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ue-RxTxTimeDiffResult-r9", ue_rx_tx_time_diff_result_r9);
  j.write_str("currentSFN-r9", current_sfn_r9.to_string());
  j.end_obj();
}

// MeasResultForRSSI-r13 ::= SEQUENCE
SRSASN_CODE meas_result_for_rssi_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, rssi_result_r13, (uint8_t)0u, (uint8_t)76u));
  HANDLE_CODE(pack_integer(bref, ch_occupancy_r13, (uint8_t)0u, (uint8_t)100u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_for_rssi_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(rssi_result_r13, bref, (uint8_t)0u, (uint8_t)76u));
  HANDLE_CODE(unpack_integer(ch_occupancy_r13, bref, (uint8_t)0u, (uint8_t)100u));

  return SRSASN_SUCCESS;
}
void meas_result_for_rssi_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rssi-Result-r13", rssi_result_r13);
  j.write_int("channelOccupancy-r13", ch_occupancy_r13);
  j.end_obj();
}

// MeasResultSSTD-r13 ::= SEQUENCE
SRSASN_CODE meas_result_sstd_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, sfn_offset_result_r13, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(pack_integer(bref, frame_boundary_offset_result_r13, (int8_t)-5, (int8_t)4));
  HANDLE_CODE(pack_integer(bref, sf_boundary_offset_result_r13, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_sstd_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(sfn_offset_result_r13, bref, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(unpack_integer(frame_boundary_offset_result_r13, bref, (int8_t)-5, (int8_t)4));
  HANDLE_CODE(unpack_integer(sf_boundary_offset_result_r13, bref, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
void meas_result_sstd_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sfn-OffsetResult-r13", sfn_offset_result_r13);
  j.write_int("frameBoundaryOffsetResult-r13", frame_boundary_offset_result_r13);
  j.write_int("subframeBoundaryOffsetResult-r13", sf_boundary_offset_result_r13);
  j.end_obj();
}

// MeasResultSensing-r15 ::= SEQUENCE
SRSASN_CODE meas_result_sensing_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, sl_sf_ref_r15, (uint16_t)0u, (uint16_t)10239u));
  HANDLE_CODE(pack_dyn_seq_of(bref, sensing_result_r15, 0, 400));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_sensing_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(sl_sf_ref_r15, bref, (uint16_t)0u, (uint16_t)10239u));
  HANDLE_CODE(unpack_dyn_seq_of(sensing_result_r15, bref, 0, 400));

  return SRSASN_SUCCESS;
}
void meas_result_sensing_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-SubframeRef-r15", sl_sf_ref_r15);
  j.start_array("sensingResult-r15");
  for (const auto& e1 : sensing_result_r15) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MeasResults ::= SEQUENCE
SRSASN_CODE meas_results_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_present, 1));

  HANDLE_CODE(pack_integer(bref, meas_id, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, meas_result_pcell.rsrp_result, (uint8_t)0u, (uint8_t)97u));
  HANDLE_CODE(pack_integer(bref, meas_result_pcell.rsrq_result, (uint8_t)0u, (uint8_t)34u));
  if (meas_result_neigh_cells_present) {
    HANDLE_CODE(meas_result_neigh_cells.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_result_for_ecid_r9.is_present();
    group_flags[1] |= location_info_r10.is_present();
    group_flags[1] |= meas_result_serv_freq_list_r10.is_present();
    group_flags[2] |= meas_id_v1250_present;
    group_flags[2] |= meas_result_pcell_v1250_present;
    group_flags[2] |= meas_result_csi_rs_list_r12.is_present();
    group_flags[3] |= meas_result_for_rssi_r13.is_present();
    group_flags[3] |= meas_result_serv_freq_list_ext_r13.is_present();
    group_flags[3] |= meas_result_sstd_r13.is_present();
    group_flags[3] |= meas_result_pcell_v1310.is_present();
    group_flags[3] |= ul_pdcp_delay_result_list_r13.is_present();
    group_flags[3] |= meas_result_list_wlan_r13.is_present();
    group_flags[4] |= meas_result_pcell_v1360_present;
    group_flags[5] |= meas_result_list_cbr_r14.is_present();
    group_flags[5] |= meas_result_list_wlan_r14.is_present();
    group_flags[6] |= meas_result_serv_freq_list_nr_r15.is_present();
    group_flags[6] |= meas_result_cell_list_sftd_r15.is_present();
    group_flags[7] |= log_meas_result_list_bt_r15.is_present();
    group_flags[7] |= log_meas_result_list_wlan_r15.is_present();
    group_flags[7] |= meas_result_sensing_r15.is_present();
    group_flags[7] |= height_ue_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_for_ecid_r9.is_present(), 1));
      if (meas_result_for_ecid_r9.is_present()) {
        HANDLE_CODE(meas_result_for_ecid_r9->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(location_info_r10.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_serv_freq_list_r10.is_present(), 1));
      if (location_info_r10.is_present()) {
        HANDLE_CODE(location_info_r10->pack(bref));
      }
      if (meas_result_serv_freq_list_r10.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_serv_freq_list_r10, 1, 5));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_id_v1250_present, 1));
      HANDLE_CODE(bref.pack(meas_result_pcell_v1250_present, 1));
      HANDLE_CODE(bref.pack(meas_result_csi_rs_list_r12.is_present(), 1));
      if (meas_id_v1250_present) {
        HANDLE_CODE(pack_integer(bref, meas_id_v1250, (uint8_t)33u, (uint8_t)64u));
      }
      if (meas_result_pcell_v1250_present) {
        HANDLE_CODE(pack_integer(bref, meas_result_pcell_v1250, (int8_t)-30, (int8_t)46));
      }
      if (meas_result_csi_rs_list_r12.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_csi_rs_list_r12, 1, 8));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_for_rssi_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_serv_freq_list_ext_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_sstd_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_pcell_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pdcp_delay_result_list_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_list_wlan_r13.is_present(), 1));
      if (meas_result_for_rssi_r13.is_present()) {
        HANDLE_CODE(meas_result_for_rssi_r13->pack(bref));
      }
      if (meas_result_serv_freq_list_ext_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_serv_freq_list_ext_r13, 1, 32));
      }
      if (meas_result_sstd_r13.is_present()) {
        HANDLE_CODE(meas_result_sstd_r13->pack(bref));
      }
      if (meas_result_pcell_v1310.is_present()) {
        HANDLE_CODE(pack_integer(bref, meas_result_pcell_v1310->rs_sinr_result_r13, (uint8_t)0u, (uint8_t)127u));
      }
      if (ul_pdcp_delay_result_list_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *ul_pdcp_delay_result_list_r13, 1, 6));
      }
      if (meas_result_list_wlan_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_list_wlan_r13, 1, 8));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_pcell_v1360_present, 1));
      if (meas_result_pcell_v1360_present) {
        HANDLE_CODE(pack_integer(bref, meas_result_pcell_v1360, (int8_t)-17, (int8_t)-1));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_list_cbr_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_list_wlan_r14.is_present(), 1));
      if (meas_result_list_cbr_r14.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_list_cbr_r14, 1, 72));
      }
      if (meas_result_list_wlan_r14.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_list_wlan_r14, 1, 32));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_serv_freq_list_nr_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_cell_list_sftd_r15.is_present(), 1));
      if (meas_result_serv_freq_list_nr_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_serv_freq_list_nr_r15, 1, 32));
      }
      if (meas_result_cell_list_sftd_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_cell_list_sftd_r15, 1, 3));
      }
    }
    if (group_flags[7]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(log_meas_result_list_bt_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(log_meas_result_list_wlan_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_sensing_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(height_ue_r15_present, 1));
      if (log_meas_result_list_bt_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *log_meas_result_list_bt_r15, 1, 32));
      }
      if (log_meas_result_list_wlan_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *log_meas_result_list_wlan_r15, 1, 32));
      }
      if (meas_result_sensing_r15.is_present()) {
        HANDLE_CODE(meas_result_sensing_r15->pack(bref));
      }
      if (height_ue_r15_present) {
        HANDLE_CODE(pack_integer(bref, height_ue_r15, (int16_t)-400, (int16_t)8880));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_results_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_neigh_cells_present, 1));

  HANDLE_CODE(unpack_integer(meas_id, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(meas_result_pcell.rsrp_result, bref, (uint8_t)0u, (uint8_t)97u));
  HANDLE_CODE(unpack_integer(meas_result_pcell.rsrq_result, bref, (uint8_t)0u, (uint8_t)34u));
  if (meas_result_neigh_cells_present) {
    HANDLE_CODE(meas_result_neigh_cells.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(8);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_result_for_ecid_r9_present;
      HANDLE_CODE(bref.unpack(meas_result_for_ecid_r9_present, 1));
      meas_result_for_ecid_r9.set_present(meas_result_for_ecid_r9_present);
      if (meas_result_for_ecid_r9.is_present()) {
        HANDLE_CODE(meas_result_for_ecid_r9->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool location_info_r10_present;
      HANDLE_CODE(bref.unpack(location_info_r10_present, 1));
      location_info_r10.set_present(location_info_r10_present);
      bool meas_result_serv_freq_list_r10_present;
      HANDLE_CODE(bref.unpack(meas_result_serv_freq_list_r10_present, 1));
      meas_result_serv_freq_list_r10.set_present(meas_result_serv_freq_list_r10_present);
      if (location_info_r10.is_present()) {
        HANDLE_CODE(location_info_r10->unpack(bref));
      }
      if (meas_result_serv_freq_list_r10.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_serv_freq_list_r10, bref, 1, 5));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(meas_id_v1250_present, 1));
      HANDLE_CODE(bref.unpack(meas_result_pcell_v1250_present, 1));
      bool meas_result_csi_rs_list_r12_present;
      HANDLE_CODE(bref.unpack(meas_result_csi_rs_list_r12_present, 1));
      meas_result_csi_rs_list_r12.set_present(meas_result_csi_rs_list_r12_present);
      if (meas_id_v1250_present) {
        HANDLE_CODE(unpack_integer(meas_id_v1250, bref, (uint8_t)33u, (uint8_t)64u));
      }
      if (meas_result_pcell_v1250_present) {
        HANDLE_CODE(unpack_integer(meas_result_pcell_v1250, bref, (int8_t)-30, (int8_t)46));
      }
      if (meas_result_csi_rs_list_r12.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_csi_rs_list_r12, bref, 1, 8));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_result_for_rssi_r13_present;
      HANDLE_CODE(bref.unpack(meas_result_for_rssi_r13_present, 1));
      meas_result_for_rssi_r13.set_present(meas_result_for_rssi_r13_present);
      bool meas_result_serv_freq_list_ext_r13_present;
      HANDLE_CODE(bref.unpack(meas_result_serv_freq_list_ext_r13_present, 1));
      meas_result_serv_freq_list_ext_r13.set_present(meas_result_serv_freq_list_ext_r13_present);
      bool meas_result_sstd_r13_present;
      HANDLE_CODE(bref.unpack(meas_result_sstd_r13_present, 1));
      meas_result_sstd_r13.set_present(meas_result_sstd_r13_present);
      bool meas_result_pcell_v1310_present;
      HANDLE_CODE(bref.unpack(meas_result_pcell_v1310_present, 1));
      meas_result_pcell_v1310.set_present(meas_result_pcell_v1310_present);
      bool ul_pdcp_delay_result_list_r13_present;
      HANDLE_CODE(bref.unpack(ul_pdcp_delay_result_list_r13_present, 1));
      ul_pdcp_delay_result_list_r13.set_present(ul_pdcp_delay_result_list_r13_present);
      bool meas_result_list_wlan_r13_present;
      HANDLE_CODE(bref.unpack(meas_result_list_wlan_r13_present, 1));
      meas_result_list_wlan_r13.set_present(meas_result_list_wlan_r13_present);
      if (meas_result_for_rssi_r13.is_present()) {
        HANDLE_CODE(meas_result_for_rssi_r13->unpack(bref));
      }
      if (meas_result_serv_freq_list_ext_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_serv_freq_list_ext_r13, bref, 1, 32));
      }
      if (meas_result_sstd_r13.is_present()) {
        HANDLE_CODE(meas_result_sstd_r13->unpack(bref));
      }
      if (meas_result_pcell_v1310.is_present()) {
        HANDLE_CODE(unpack_integer(meas_result_pcell_v1310->rs_sinr_result_r13, bref, (uint8_t)0u, (uint8_t)127u));
      }
      if (ul_pdcp_delay_result_list_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*ul_pdcp_delay_result_list_r13, bref, 1, 6));
      }
      if (meas_result_list_wlan_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_list_wlan_r13, bref, 1, 8));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(meas_result_pcell_v1360_present, 1));
      if (meas_result_pcell_v1360_present) {
        HANDLE_CODE(unpack_integer(meas_result_pcell_v1360, bref, (int8_t)-17, (int8_t)-1));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_result_list_cbr_r14_present;
      HANDLE_CODE(bref.unpack(meas_result_list_cbr_r14_present, 1));
      meas_result_list_cbr_r14.set_present(meas_result_list_cbr_r14_present);
      bool meas_result_list_wlan_r14_present;
      HANDLE_CODE(bref.unpack(meas_result_list_wlan_r14_present, 1));
      meas_result_list_wlan_r14.set_present(meas_result_list_wlan_r14_present);
      if (meas_result_list_cbr_r14.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_list_cbr_r14, bref, 1, 72));
      }
      if (meas_result_list_wlan_r14.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_list_wlan_r14, bref, 1, 32));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_result_serv_freq_list_nr_r15_present;
      HANDLE_CODE(bref.unpack(meas_result_serv_freq_list_nr_r15_present, 1));
      meas_result_serv_freq_list_nr_r15.set_present(meas_result_serv_freq_list_nr_r15_present);
      bool meas_result_cell_list_sftd_r15_present;
      HANDLE_CODE(bref.unpack(meas_result_cell_list_sftd_r15_present, 1));
      meas_result_cell_list_sftd_r15.set_present(meas_result_cell_list_sftd_r15_present);
      if (meas_result_serv_freq_list_nr_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_serv_freq_list_nr_r15, bref, 1, 32));
      }
      if (meas_result_cell_list_sftd_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_cell_list_sftd_r15, bref, 1, 3));
      }
    }
    if (group_flags[7]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool log_meas_result_list_bt_r15_present;
      HANDLE_CODE(bref.unpack(log_meas_result_list_bt_r15_present, 1));
      log_meas_result_list_bt_r15.set_present(log_meas_result_list_bt_r15_present);
      bool log_meas_result_list_wlan_r15_present;
      HANDLE_CODE(bref.unpack(log_meas_result_list_wlan_r15_present, 1));
      log_meas_result_list_wlan_r15.set_present(log_meas_result_list_wlan_r15_present);
      bool meas_result_sensing_r15_present;
      HANDLE_CODE(bref.unpack(meas_result_sensing_r15_present, 1));
      meas_result_sensing_r15.set_present(meas_result_sensing_r15_present);
      HANDLE_CODE(bref.unpack(height_ue_r15_present, 1));
      if (log_meas_result_list_bt_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*log_meas_result_list_bt_r15, bref, 1, 32));
      }
      if (log_meas_result_list_wlan_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*log_meas_result_list_wlan_r15, bref, 1, 32));
      }
      if (meas_result_sensing_r15.is_present()) {
        HANDLE_CODE(meas_result_sensing_r15->unpack(bref));
      }
      if (height_ue_r15_present) {
        HANDLE_CODE(unpack_integer(height_ue_r15, bref, (int16_t)-400, (int16_t)8880));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_results_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measId", meas_id);
  j.write_fieldname("measResultPCell");
  j.start_obj();
  j.write_int("rsrpResult", meas_result_pcell.rsrp_result);
  j.write_int("rsrqResult", meas_result_pcell.rsrq_result);
  j.end_obj();
  if (meas_result_neigh_cells_present) {
    j.write_fieldname("measResultNeighCells");
    meas_result_neigh_cells.to_json(j);
  }
  if (ext) {
    if (meas_result_for_ecid_r9.is_present()) {
      j.write_fieldname("measResultForECID-r9");
      meas_result_for_ecid_r9->to_json(j);
    }
    if (location_info_r10.is_present()) {
      j.write_fieldname("locationInfo-r10");
      location_info_r10->to_json(j);
    }
    if (meas_result_serv_freq_list_r10.is_present()) {
      j.start_array("measResultServFreqList-r10");
      for (const auto& e1 : *meas_result_serv_freq_list_r10) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_id_v1250_present) {
      j.write_int("measId-v1250", meas_id_v1250);
    }
    if (meas_result_pcell_v1250_present) {
      j.write_int("measResultPCell-v1250", meas_result_pcell_v1250);
    }
    if (meas_result_csi_rs_list_r12.is_present()) {
      j.start_array("measResultCSI-RS-List-r12");
      for (const auto& e1 : *meas_result_csi_rs_list_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_for_rssi_r13.is_present()) {
      j.write_fieldname("measResultForRSSI-r13");
      meas_result_for_rssi_r13->to_json(j);
    }
    if (meas_result_serv_freq_list_ext_r13.is_present()) {
      j.start_array("measResultServFreqListExt-r13");
      for (const auto& e1 : *meas_result_serv_freq_list_ext_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_sstd_r13.is_present()) {
      j.write_fieldname("measResultSSTD-r13");
      meas_result_sstd_r13->to_json(j);
    }
    if (meas_result_pcell_v1310.is_present()) {
      j.write_fieldname("measResultPCell-v1310");
      j.start_obj();
      j.write_int("rs-sinr-Result-r13", meas_result_pcell_v1310->rs_sinr_result_r13);
      j.end_obj();
    }
    if (ul_pdcp_delay_result_list_r13.is_present()) {
      j.start_array("ul-PDCP-DelayResultList-r13");
      for (const auto& e1 : *ul_pdcp_delay_result_list_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_list_wlan_r13.is_present()) {
      j.start_array("measResultListWLAN-r13");
      for (const auto& e1 : *meas_result_list_wlan_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_pcell_v1360_present) {
      j.write_int("measResultPCell-v1360", meas_result_pcell_v1360);
    }
    if (meas_result_list_cbr_r14.is_present()) {
      j.start_array("measResultListCBR-r14");
      for (const auto& e1 : *meas_result_list_cbr_r14) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_list_wlan_r14.is_present()) {
      j.start_array("measResultListWLAN-r14");
      for (const auto& e1 : *meas_result_list_wlan_r14) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_serv_freq_list_nr_r15.is_present()) {
      j.start_array("measResultServFreqListNR-r15");
      for (const auto& e1 : *meas_result_serv_freq_list_nr_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_cell_list_sftd_r15.is_present()) {
      j.start_array("measResultCellListSFTD-r15");
      for (const auto& e1 : *meas_result_cell_list_sftd_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (log_meas_result_list_bt_r15.is_present()) {
      j.start_array("logMeasResultListBT-r15");
      for (const auto& e1 : *log_meas_result_list_bt_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (log_meas_result_list_wlan_r15.is_present()) {
      j.start_array("logMeasResultListWLAN-r15");
      for (const auto& e1 : *log_meas_result_list_wlan_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_sensing_r15.is_present()) {
      j.write_fieldname("measResultSensing-r15");
      meas_result_sensing_r15->to_json(j);
    }
    if (height_ue_r15_present) {
      j.write_int("heightUE-r15", height_ue_r15);
    }
  }
  j.end_obj();
}

void meas_results_s::meas_result_neigh_cells_c_::destroy_()
{
  switch (type_) {
    case types::meas_result_list_eutra:
      c.destroy<meas_result_list_eutra_l>();
      break;
    case types::meas_result_list_utra:
      c.destroy<meas_result_list_utra_l>();
      break;
    case types::meas_result_list_geran:
      c.destroy<meas_result_list_geran_l>();
      break;
    case types::meas_results_cdma2000:
      c.destroy<meas_results_cdma2000_s>();
      break;
    case types::meas_result_neigh_cell_list_nr_r15:
      c.destroy<meas_result_cell_list_nr_r15_l>();
      break;
    default:
      break;
  }
}
void meas_results_s::meas_result_neigh_cells_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::meas_result_list_eutra:
      c.init<meas_result_list_eutra_l>();
      break;
    case types::meas_result_list_utra:
      c.init<meas_result_list_utra_l>();
      break;
    case types::meas_result_list_geran:
      c.init<meas_result_list_geran_l>();
      break;
    case types::meas_results_cdma2000:
      c.init<meas_results_cdma2000_s>();
      break;
    case types::meas_result_neigh_cell_list_nr_r15:
      c.init<meas_result_cell_list_nr_r15_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
  }
}
meas_results_s::meas_result_neigh_cells_c_::meas_result_neigh_cells_c_(
    const meas_results_s::meas_result_neigh_cells_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::meas_result_list_eutra:
      c.init(other.c.get<meas_result_list_eutra_l>());
      break;
    case types::meas_result_list_utra:
      c.init(other.c.get<meas_result_list_utra_l>());
      break;
    case types::meas_result_list_geran:
      c.init(other.c.get<meas_result_list_geran_l>());
      break;
    case types::meas_results_cdma2000:
      c.init(other.c.get<meas_results_cdma2000_s>());
      break;
    case types::meas_result_neigh_cell_list_nr_r15:
      c.init(other.c.get<meas_result_cell_list_nr_r15_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
  }
}
meas_results_s::meas_result_neigh_cells_c_&
meas_results_s::meas_result_neigh_cells_c_::operator=(const meas_results_s::meas_result_neigh_cells_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::meas_result_list_eutra:
      c.set(other.c.get<meas_result_list_eutra_l>());
      break;
    case types::meas_result_list_utra:
      c.set(other.c.get<meas_result_list_utra_l>());
      break;
    case types::meas_result_list_geran:
      c.set(other.c.get<meas_result_list_geran_l>());
      break;
    case types::meas_results_cdma2000:
      c.set(other.c.get<meas_results_cdma2000_s>());
      break;
    case types::meas_result_neigh_cell_list_nr_r15:
      c.set(other.c.get<meas_result_cell_list_nr_r15_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
  }

  return *this;
}
meas_result_list_eutra_l& meas_results_s::meas_result_neigh_cells_c_::set_meas_result_list_eutra()
{
  set(types::meas_result_list_eutra);
  return c.get<meas_result_list_eutra_l>();
}
meas_result_list_utra_l& meas_results_s::meas_result_neigh_cells_c_::set_meas_result_list_utra()
{
  set(types::meas_result_list_utra);
  return c.get<meas_result_list_utra_l>();
}
meas_result_list_geran_l& meas_results_s::meas_result_neigh_cells_c_::set_meas_result_list_geran()
{
  set(types::meas_result_list_geran);
  return c.get<meas_result_list_geran_l>();
}
meas_results_cdma2000_s& meas_results_s::meas_result_neigh_cells_c_::set_meas_results_cdma2000()
{
  set(types::meas_results_cdma2000);
  return c.get<meas_results_cdma2000_s>();
}
meas_result_cell_list_nr_r15_l& meas_results_s::meas_result_neigh_cells_c_::set_meas_result_neigh_cell_list_nr_r15()
{
  set(types::meas_result_neigh_cell_list_nr_r15);
  return c.get<meas_result_cell_list_nr_r15_l>();
}
void meas_results_s::meas_result_neigh_cells_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_result_list_eutra:
      j.start_array("measResultListEUTRA");
      for (const auto& e1 : c.get<meas_result_list_eutra_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::meas_result_list_utra:
      j.start_array("measResultListUTRA");
      for (const auto& e1 : c.get<meas_result_list_utra_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::meas_result_list_geran:
      j.start_array("measResultListGERAN");
      for (const auto& e1 : c.get<meas_result_list_geran_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::meas_results_cdma2000:
      j.write_fieldname("measResultsCDMA2000");
      c.get<meas_results_cdma2000_s>().to_json(j);
      break;
    case types::meas_result_neigh_cell_list_nr_r15:
      j.start_array("measResultNeighCellListNR-r15");
      for (const auto& e1 : c.get<meas_result_cell_list_nr_r15_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_results_s::meas_result_neigh_cells_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_result_list_eutra:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<meas_result_list_eutra_l>(), 1, 8));
      break;
    case types::meas_result_list_utra:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<meas_result_list_utra_l>(), 1, 8));
      break;
    case types::meas_result_list_geran:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<meas_result_list_geran_l>(), 1, 8));
      break;
    case types::meas_results_cdma2000:
      HANDLE_CODE(c.get<meas_results_cdma2000_s>().pack(bref));
      break;
    case types::meas_result_neigh_cell_list_nr_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<meas_result_cell_list_nr_r15_l>(), 1, 8));
    } break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_results_s::meas_result_neigh_cells_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_result_list_eutra:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<meas_result_list_eutra_l>(), bref, 1, 8));
      break;
    case types::meas_result_list_utra:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<meas_result_list_utra_l>(), bref, 1, 8));
      break;
    case types::meas_result_list_geran:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<meas_result_list_geran_l>(), bref, 1, 8));
      break;
    case types::meas_results_cdma2000:
      HANDLE_CODE(c.get<meas_results_cdma2000_s>().unpack(bref));
      break;
    case types::meas_result_neigh_cell_list_nr_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_dyn_seq_of(c.get<meas_result_cell_list_nr_r15_l>(), bref, 1, 8));
    } break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MeasurementReport-v8a0-IEs ::= SEQUENCE
SRSASN_CODE meas_report_v8a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_v8a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_report_v8a0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// MeasurementReport-r8-IEs ::= SEQUENCE
SRSASN_CODE meas_report_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(meas_results.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(meas_results.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_report_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measResults");
  meas_results.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MeasurementReport ::= SEQUENCE
SRSASN_CODE meas_report_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_report_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void meas_report_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
meas_report_s::crit_exts_c_::c1_c_& meas_report_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void meas_report_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void meas_report_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_report_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void meas_report_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
meas_report_r8_ies_s& meas_report_s::crit_exts_c_::c1_c_::set_meas_report_r8()
{
  set(types::meas_report_r8);
  return c;
}
void meas_report_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void meas_report_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void meas_report_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void meas_report_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void meas_report_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void meas_report_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void meas_report_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void meas_report_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_report_r8:
      j.write_fieldname("measurementReport-r8");
      c.to_json(j);
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_report_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_report_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_report_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MobilityControlInfo-v10l0 ::= SEQUENCE
SRSASN_CODE mob_ctrl_info_v10l0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(add_spec_emission_v10l0_present, 1));

  if (add_spec_emission_v10l0_present) {
    HANDLE_CODE(pack_integer(bref, add_spec_emission_v10l0, (uint16_t)33u, (uint16_t)288u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_ctrl_info_v10l0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(add_spec_emission_v10l0_present, 1));

  if (add_spec_emission_v10l0_present) {
    HANDLE_CODE(unpack_integer(add_spec_emission_v10l0, bref, (uint16_t)33u, (uint16_t)288u));
  }

  return SRSASN_SUCCESS;
}
void mob_ctrl_info_v10l0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (add_spec_emission_v10l0_present) {
    j.write_int("additionalSpectrumEmission-v10l0", add_spec_emission_v10l0);
  }
  j.end_obj();
}
