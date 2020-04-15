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

#include "srslte/asn1/rrc_asn1_nbiot.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                              Logging Utilities
 ******************************************************************************/
static void log_invalid_choice_id(uint32_t val, const char* choice_type)
{
  asn1::log_error("Invalid choice id=%d for choice type %s\n", val, choice_type);
}

static void invalid_enum_number(int value, const char* name)
{
  asn1::log_error("The provided enum value=%d of type %s cannot be translated into a number\n", value, name);
}

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// AB-Config-NB-r13 ::= SEQUENCE
SRSASN_CODE ab_cfg_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ab_barr_for_exception_data_r13_present, 1));

  HANDLE_CODE(ab_category_r13.pack(bref));
  HANDLE_CODE(ab_barr_bitmap_r13.pack(bref));
  HANDLE_CODE(ab_barr_for_special_ac_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ab_cfg_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ab_barr_for_exception_data_r13_present, 1));

  HANDLE_CODE(ab_category_r13.unpack(bref));
  HANDLE_CODE(ab_barr_bitmap_r13.unpack(bref));
  HANDLE_CODE(ab_barr_for_special_ac_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void ab_cfg_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ab-Category-r13", ab_category_r13.to_string());
  j.write_str("ab-BarringBitmap-r13", ab_barr_bitmap_r13.to_string());
  if (ab_barr_for_exception_data_r13_present) {
    j.write_str("ab-BarringForExceptionData-r13", "true");
  }
  j.write_str("ab-BarringForSpecialAC-r13", ab_barr_for_special_ac_r13.to_string());
  j.end_obj();
}

std::string ab_cfg_nb_r13_s::ab_category_r13_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c"};
  return convert_enum_idx(options, 3, value, "ab_cfg_nb_r13_s::ab_category_r13_e_");
}

// AB-ConfigPLMN-NB-r13 ::= SEQUENCE
SRSASN_CODE ab_cfg_plmn_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ab_cfg_r13_present, 1));

  if (ab_cfg_r13_present) {
    HANDLE_CODE(ab_cfg_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ab_cfg_plmn_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ab_cfg_r13_present, 1));

  if (ab_cfg_r13_present) {
    HANDLE_CODE(ab_cfg_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ab_cfg_plmn_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ab_cfg_r13_present) {
    j.write_fieldname("ab-Config-r13");
    ab_cfg_r13.to_json(j);
  }
  j.end_obj();
}

// T-PollRetransmit-NB-r13 ::= ENUMERATED
std::string t_poll_retx_nb_r13_opts::to_string() const
{
  static const char* options[] = {"ms250",
                                  "ms500",
                                  "ms1000",
                                  "ms2000",
                                  "ms3000",
                                  "ms4000",
                                  "ms6000",
                                  "ms10000",
                                  "ms15000",
                                  "ms25000",
                                  "ms40000",
                                  "ms60000",
                                  "ms90000",
                                  "ms120000",
                                  "ms180000",
                                  "ms300000-v1530"};
  return convert_enum_idx(options, 16, value, "t_poll_retx_nb_r13_e");
}
uint32_t t_poll_retx_nb_r13_opts::to_number() const
{
  static const uint32_t options[] = {
      250, 500, 1000, 2000, 3000, 4000, 6000, 10000, 15000, 25000, 40000, 60000, 90000, 120000, 180000, 300000};
  return map_enum_number(options, 16, value, "t_poll_retx_nb_r13_e");
}

// CarrierFreq-NB-r13 ::= SEQUENCE
SRSASN_CODE carrier_freq_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(carrier_freq_offset_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r13, (uint32_t)0u, (uint32_t)262143u));
  if (carrier_freq_offset_r13_present) {
    HANDLE_CODE(carrier_freq_offset_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(carrier_freq_offset_r13_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r13, bref, (uint32_t)0u, (uint32_t)262143u));
  if (carrier_freq_offset_r13_present) {
    HANDLE_CODE(carrier_freq_offset_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void carrier_freq_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r13", carrier_freq_r13);
  if (carrier_freq_offset_r13_present) {
    j.write_str("carrierFreqOffset-r13", carrier_freq_offset_r13.to_string());
  }
  j.end_obj();
}

std::string carrier_freq_nb_r13_s::carrier_freq_offset_r13_opts::to_string() const
{
  static const char* options[] = {"v-10", "v-9", "v-8", "v-7", "v-6", "v-5", "v-4", "v-3", "v-2", "v-1", "v-0dot5",
                                  "v0",   "v1",  "v2",  "v3",  "v4",  "v5",  "v6",  "v7",  "v8",  "v9"};
  return convert_enum_idx(options, 21, value, "carrier_freq_nb_r13_s::carrier_freq_offset_r13_e_");
}
float carrier_freq_nb_r13_s::carrier_freq_offset_r13_opts::to_number() const
{
  static const float options[] = {-10.0, -9.0, -8.0, -7.0, -6.0, -5.0, -4.0, -3.0, -2.0, -1.0, -0.5,
                                  0.0,   1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0};
  return map_enum_number(options, 21, value, "carrier_freq_nb_r13_s::carrier_freq_offset_r13_e_");
}
std::string carrier_freq_nb_r13_s::carrier_freq_offset_r13_opts::to_number_string() const
{
  static const char* options[] = {"-10", "-9", "-8", "-7", "-6", "-5", "-4", "-3", "-2", "-1", "-0.5",
                                  "0",   "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9"};
  return convert_enum_idx(options, 21, value, "carrier_freq_nb_r13_s::carrier_freq_offset_r13_e_");
}

// DL-AM-RLC-NB-r13 ::= SEQUENCE
SRSASN_CODE dl_am_rlc_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(enable_status_report_sn_gap_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_am_rlc_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(enable_status_report_sn_gap_r13_present, 1));

  return SRSASN_SUCCESS;
}
void dl_am_rlc_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (enable_status_report_sn_gap_r13_present) {
    j.write_str("enableStatusReportSN-Gap-r13", "true");
  }
  j.end_obj();
}

// DL-Bitmap-NB-r13 ::= CHOICE
void dl_bitmap_nb_r13_c::destroy_()
{
  switch (type_) {
    case types::sf_pattern10_r13:
      c.destroy<fixed_bitstring<10> >();
      break;
    case types::sf_pattern40_r13:
      c.destroy<fixed_bitstring<40> >();
      break;
    default:
      break;
  }
}
void dl_bitmap_nb_r13_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sf_pattern10_r13:
      c.init<fixed_bitstring<10> >();
      break;
    case types::sf_pattern40_r13:
      c.init<fixed_bitstring<40> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_bitmap_nb_r13_c");
  }
}
dl_bitmap_nb_r13_c::dl_bitmap_nb_r13_c(const dl_bitmap_nb_r13_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf_pattern10_r13:
      c.init(other.c.get<fixed_bitstring<10> >());
      break;
    case types::sf_pattern40_r13:
      c.init(other.c.get<fixed_bitstring<40> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_bitmap_nb_r13_c");
  }
}
dl_bitmap_nb_r13_c& dl_bitmap_nb_r13_c::operator=(const dl_bitmap_nb_r13_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf_pattern10_r13:
      c.set(other.c.get<fixed_bitstring<10> >());
      break;
    case types::sf_pattern40_r13:
      c.set(other.c.get<fixed_bitstring<40> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_bitmap_nb_r13_c");
  }

  return *this;
}
void dl_bitmap_nb_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf_pattern10_r13:
      j.write_str("subframePattern10-r13", c.get<fixed_bitstring<10> >().to_string());
      break;
    case types::sf_pattern40_r13:
      j.write_str("subframePattern40-r13", c.get<fixed_bitstring<40> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "dl_bitmap_nb_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE dl_bitmap_nb_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf_pattern10_r13:
      HANDLE_CODE(c.get<fixed_bitstring<10> >().pack(bref));
      break;
    case types::sf_pattern40_r13:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_bitmap_nb_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_bitmap_nb_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf_pattern10_r13:
      HANDLE_CODE(c.get<fixed_bitstring<10> >().unpack(bref));
      break;
    case types::sf_pattern40_r13:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_bitmap_nb_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string dl_bitmap_nb_r13_c::types_opts::to_string() const
{
  static const char* options[] = {"subframePattern10-r13", "subframePattern40-r13"};
  return convert_enum_idx(options, 2, value, "dl_bitmap_nb_r13_c::types");
}
uint8_t dl_bitmap_nb_r13_c::types_opts::to_number() const
{
  static const uint8_t options[] = {10, 40};
  return map_enum_number(options, 2, value, "dl_bitmap_nb_r13_c::types");
}

// DL-GapConfig-NB-r13 ::= SEQUENCE
SRSASN_CODE dl_gap_cfg_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(dl_gap_thres_r13.pack(bref));
  HANDLE_CODE(dl_gap_periodicity_r13.pack(bref));
  HANDLE_CODE(dl_gap_dur_coeff_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_gap_cfg_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(dl_gap_thres_r13.unpack(bref));
  HANDLE_CODE(dl_gap_periodicity_r13.unpack(bref));
  HANDLE_CODE(dl_gap_dur_coeff_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void dl_gap_cfg_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("dl-GapThreshold-r13", dl_gap_thres_r13.to_string());
  j.write_str("dl-GapPeriodicity-r13", dl_gap_periodicity_r13.to_string());
  j.write_str("dl-GapDurationCoeff-r13", dl_gap_dur_coeff_r13.to_string());
  j.end_obj();
}

std::string dl_gap_cfg_nb_r13_s::dl_gap_thres_r13_opts::to_string() const
{
  static const char* options[] = {"n32", "n64", "n128", "n256"};
  return convert_enum_idx(options, 4, value, "dl_gap_cfg_nb_r13_s::dl_gap_thres_r13_e_");
}
uint16_t dl_gap_cfg_nb_r13_s::dl_gap_thres_r13_opts::to_number() const
{
  static const uint16_t options[] = {32, 64, 128, 256};
  return map_enum_number(options, 4, value, "dl_gap_cfg_nb_r13_s::dl_gap_thres_r13_e_");
}

std::string dl_gap_cfg_nb_r13_s::dl_gap_periodicity_r13_opts::to_string() const
{
  static const char* options[] = {"sf64", "sf128", "sf256", "sf512"};
  return convert_enum_idx(options, 4, value, "dl_gap_cfg_nb_r13_s::dl_gap_periodicity_r13_e_");
}
uint16_t dl_gap_cfg_nb_r13_s::dl_gap_periodicity_r13_opts::to_number() const
{
  static const uint16_t options[] = {64, 128, 256, 512};
  return map_enum_number(options, 4, value, "dl_gap_cfg_nb_r13_s::dl_gap_periodicity_r13_e_");
}

std::string dl_gap_cfg_nb_r13_s::dl_gap_dur_coeff_r13_opts::to_string() const
{
  static const char* options[] = {"oneEighth", "oneFourth", "threeEighth", "oneHalf"};
  return convert_enum_idx(options, 4, value, "dl_gap_cfg_nb_r13_s::dl_gap_dur_coeff_r13_e_");
}
float dl_gap_cfg_nb_r13_s::dl_gap_dur_coeff_r13_opts::to_number() const
{
  static const float options[] = {0.125, 0.25, 0.375, 0.5};
  return map_enum_number(options, 4, value, "dl_gap_cfg_nb_r13_s::dl_gap_dur_coeff_r13_e_");
}
std::string dl_gap_cfg_nb_r13_s::dl_gap_dur_coeff_r13_opts::to_number_string() const
{
  static const char* options[] = {"1/8", "1/4", "3/8", "1/2"};
  return convert_enum_idx(options, 4, value, "dl_gap_cfg_nb_r13_s::dl_gap_dur_coeff_r13_e_");
}

// DL-GapConfig-NB-v1530 ::= SEQUENCE
SRSASN_CODE dl_gap_cfg_nb_v1530_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_gap_cfg_nb_v1530_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void dl_gap_cfg_nb_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("dl-GapPeriodicity-v1530", "sf1024");
  j.end_obj();
}

// TDD-UL-DL-AlignmentOffset-NB-r15 ::= ENUMERATED
std::string tdd_ul_dl_align_offset_nb_r15_opts::to_string() const
{
  static const char* options[] = {"khz-7dot5", "khz0", "khz7dot5"};
  return convert_enum_idx(options, 3, value, "tdd_ul_dl_align_offset_nb_r15_e");
}
float tdd_ul_dl_align_offset_nb_r15_opts::to_number() const
{
  static const float options[] = {-7.5, 0.0, 7.5};
  return map_enum_number(options, 3, value, "tdd_ul_dl_align_offset_nb_r15_e");
}
std::string tdd_ul_dl_align_offset_nb_r15_opts::to_number_string() const
{
  static const char* options[] = {"-7.5", "0", "7.5"};
  return convert_enum_idx(options, 3, value, "tdd_ul_dl_align_offset_nb_r15_e");
}

// UL-AM-RLC-NB-r13 ::= SEQUENCE
SRSASN_CODE ul_am_rlc_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(t_poll_retx_r13.pack(bref));
  HANDLE_CODE(max_retx_thres_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_am_rlc_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(t_poll_retx_r13.unpack(bref));
  HANDLE_CODE(max_retx_thres_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_am_rlc_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t-PollRetransmit-r13", t_poll_retx_r13.to_string());
  j.write_str("maxRetxThreshold-r13", max_retx_thres_r13.to_string());
  j.end_obj();
}

std::string ul_am_rlc_nb_r13_s::max_retx_thres_r13_opts::to_string() const
{
  static const char* options[] = {"t1", "t2", "t3", "t4", "t6", "t8", "t16", "t32"};
  return convert_enum_idx(options, 8, value, "ul_am_rlc_nb_r13_s::max_retx_thres_r13_e_");
}
uint8_t ul_am_rlc_nb_r13_s::max_retx_thres_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(options, 8, value, "ul_am_rlc_nb_r13_s::max_retx_thres_r13_e_");
}

// ACK-NACK-NumRepetitions-NB-r13 ::= ENUMERATED
std::string ack_nack_num_repeats_nb_r13_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "r128"};
  return convert_enum_idx(options, 8, value, "ack_nack_num_repeats_nb_r13_e");
}
uint8_t ack_nack_num_repeats_nb_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8, 16, 32, 64, 128};
  return map_enum_number(options, 8, value, "ack_nack_num_repeats_nb_r13_e");
}

// DL-CarrierConfigDedicated-NB-r13 ::= SEQUENCE
SRSASN_CODE dl_carrier_cfg_ded_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dl_bitmap_non_anchor_r13_present, 1));
  HANDLE_CODE(bref.pack(dl_gap_non_anchor_r13_present, 1));
  HANDLE_CODE(bref.pack(inband_carrier_info_r13_present, 1));

  HANDLE_CODE(dl_carrier_freq_r13.pack(bref));
  if (dl_bitmap_non_anchor_r13_present) {
    HANDLE_CODE(dl_bitmap_non_anchor_r13.pack(bref));
  }
  if (dl_gap_non_anchor_r13_present) {
    HANDLE_CODE(dl_gap_non_anchor_r13.pack(bref));
  }
  if (inband_carrier_info_r13_present) {
    HANDLE_CODE(bref.pack(inband_carrier_info_r13.same_pci_ind_r13_present, 1));
    if (inband_carrier_info_r13.same_pci_ind_r13_present) {
      HANDLE_CODE(inband_carrier_info_r13.same_pci_ind_r13.pack(bref));
    }
    HANDLE_CODE(inband_carrier_info_r13.eutra_ctrl_region_size_r13.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= nrs_pwr_offset_non_anchor_v1330_present;
    group_flags[1] |= dl_gap_non_anchor_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(nrs_pwr_offset_non_anchor_v1330_present, 1));
      if (nrs_pwr_offset_non_anchor_v1330_present) {
        HANDLE_CODE(nrs_pwr_offset_non_anchor_v1330.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(dl_gap_non_anchor_v1530.is_present(), 1));
      if (dl_gap_non_anchor_v1530.is_present()) {
        HANDLE_CODE(dl_gap_non_anchor_v1530->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_carrier_cfg_ded_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dl_bitmap_non_anchor_r13_present, 1));
  HANDLE_CODE(bref.unpack(dl_gap_non_anchor_r13_present, 1));
  HANDLE_CODE(bref.unpack(inband_carrier_info_r13_present, 1));

  HANDLE_CODE(dl_carrier_freq_r13.unpack(bref));
  if (dl_bitmap_non_anchor_r13_present) {
    HANDLE_CODE(dl_bitmap_non_anchor_r13.unpack(bref));
  }
  if (dl_gap_non_anchor_r13_present) {
    HANDLE_CODE(dl_gap_non_anchor_r13.unpack(bref));
  }
  if (inband_carrier_info_r13_present) {
    HANDLE_CODE(bref.unpack(inband_carrier_info_r13.same_pci_ind_r13_present, 1));
    if (inband_carrier_info_r13.same_pci_ind_r13_present) {
      HANDLE_CODE(inband_carrier_info_r13.same_pci_ind_r13.unpack(bref));
    }
    HANDLE_CODE(inband_carrier_info_r13.eutra_ctrl_region_size_r13.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(nrs_pwr_offset_non_anchor_v1330_present, 1));
      if (nrs_pwr_offset_non_anchor_v1330_present) {
        HANDLE_CODE(nrs_pwr_offset_non_anchor_v1330.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool dl_gap_non_anchor_v1530_present;
      HANDLE_CODE(bref.unpack(dl_gap_non_anchor_v1530_present, 1));
      dl_gap_non_anchor_v1530.set_present(dl_gap_non_anchor_v1530_present);
      if (dl_gap_non_anchor_v1530.is_present()) {
        HANDLE_CODE(dl_gap_non_anchor_v1530->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void dl_carrier_cfg_ded_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("dl-CarrierFreq-r13");
  dl_carrier_freq_r13.to_json(j);
  if (dl_bitmap_non_anchor_r13_present) {
    j.write_fieldname("downlinkBitmapNonAnchor-r13");
    dl_bitmap_non_anchor_r13.to_json(j);
  }
  if (dl_gap_non_anchor_r13_present) {
    j.write_fieldname("dl-GapNonAnchor-r13");
    dl_gap_non_anchor_r13.to_json(j);
  }
  if (inband_carrier_info_r13_present) {
    j.write_fieldname("inbandCarrierInfo-r13");
    j.start_obj();
    if (inband_carrier_info_r13.same_pci_ind_r13_present) {
      j.write_fieldname("samePCI-Indicator-r13");
      inband_carrier_info_r13.same_pci_ind_r13.to_json(j);
    }
    j.write_str("eutraControlRegionSize-r13", inband_carrier_info_r13.eutra_ctrl_region_size_r13.to_string());
    j.end_obj();
  }
  if (ext) {
    if (nrs_pwr_offset_non_anchor_v1330_present) {
      j.write_str("nrs-PowerOffsetNonAnchor-v1330", nrs_pwr_offset_non_anchor_v1330.to_string());
    }
    if (dl_gap_non_anchor_v1530.is_present()) {
      j.write_fieldname("dl-GapNonAnchor-v1530");
      dl_gap_non_anchor_v1530->to_json(j);
    }
  }
  j.end_obj();
}

void dl_carrier_cfg_ded_nb_r13_s::dl_bitmap_non_anchor_r13_c_::set(types::options e)
{
  type_ = e;
}
void dl_carrier_cfg_ded_nb_r13_s::dl_bitmap_non_anchor_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::use_no_bitmap_r13:
      break;
    case types::use_anchor_bitmap_r13:
      break;
    case types::explicit_bitmap_cfg_r13:
      j.write_fieldname("explicitBitmapConfiguration-r13");
      c.to_json(j);
      break;
    case types::spare:
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_ded_nb_r13_s::dl_bitmap_non_anchor_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_carrier_cfg_ded_nb_r13_s::dl_bitmap_non_anchor_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::use_no_bitmap_r13:
      break;
    case types::use_anchor_bitmap_r13:
      break;
    case types::explicit_bitmap_cfg_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare:
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_ded_nb_r13_s::dl_bitmap_non_anchor_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_carrier_cfg_ded_nb_r13_s::dl_bitmap_non_anchor_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::use_no_bitmap_r13:
      break;
    case types::use_anchor_bitmap_r13:
      break;
    case types::explicit_bitmap_cfg_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare:
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_ded_nb_r13_s::dl_bitmap_non_anchor_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string dl_carrier_cfg_ded_nb_r13_s::dl_bitmap_non_anchor_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"useNoBitmap-r13", "useAnchorBitmap-r13", "explicitBitmapConfiguration-r13", "spare"};
  return convert_enum_idx(options, 4, value, "dl_carrier_cfg_ded_nb_r13_s::dl_bitmap_non_anchor_r13_c_::types");
}

void dl_carrier_cfg_ded_nb_r13_s::dl_gap_non_anchor_r13_c_::set(types::options e)
{
  type_ = e;
}
void dl_carrier_cfg_ded_nb_r13_s::dl_gap_non_anchor_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::use_no_gap_r13:
      break;
    case types::use_anchor_gap_cfg_r13:
      break;
    case types::explicit_gap_cfg_r13:
      j.write_fieldname("explicitGapConfiguration-r13");
      c.to_json(j);
      break;
    case types::spare:
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_ded_nb_r13_s::dl_gap_non_anchor_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_carrier_cfg_ded_nb_r13_s::dl_gap_non_anchor_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::use_no_gap_r13:
      break;
    case types::use_anchor_gap_cfg_r13:
      break;
    case types::explicit_gap_cfg_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare:
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_ded_nb_r13_s::dl_gap_non_anchor_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_carrier_cfg_ded_nb_r13_s::dl_gap_non_anchor_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::use_no_gap_r13:
      break;
    case types::use_anchor_gap_cfg_r13:
      break;
    case types::explicit_gap_cfg_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare:
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_ded_nb_r13_s::dl_gap_non_anchor_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string dl_carrier_cfg_ded_nb_r13_s::dl_gap_non_anchor_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"useNoGap-r13", "useAnchorGapConfig-r13", "explicitGapConfiguration-r13", "spare"};
  return convert_enum_idx(options, 4, value, "dl_carrier_cfg_ded_nb_r13_s::dl_gap_non_anchor_r13_c_::types");
}

void dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_::destroy_()
{
  switch (type_) {
    case types::same_pci_r13:
      c.destroy<same_pci_r13_s_>();
      break;
    case types::different_pci_r13:
      c.destroy<different_pci_r13_s_>();
      break;
    default:
      break;
  }
}
void dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::same_pci_r13:
      c.init<same_pci_r13_s_>();
      break;
    case types::different_pci_r13:
      c.init<different_pci_r13_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_");
  }
}
dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_::same_pci_ind_r13_c_(
    const dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::same_pci_r13:
      c.init(other.c.get<same_pci_r13_s_>());
      break;
    case types::different_pci_r13:
      c.init(other.c.get<different_pci_r13_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_");
  }
}
dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_&
dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_::
operator=(const dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::same_pci_r13:
      c.set(other.c.get<same_pci_r13_s_>());
      break;
    case types::different_pci_r13:
      c.set(other.c.get<different_pci_r13_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_");
  }

  return *this;
}
void dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::same_pci_r13:
      j.write_fieldname("samePCI-r13");
      j.start_obj();
      j.write_int("indexToMidPRB-r13", c.get<same_pci_r13_s_>().idx_to_mid_prb_r13);
      j.end_obj();
      break;
    case types::different_pci_r13:
      j.write_fieldname("differentPCI-r13");
      j.start_obj();
      j.write_str("eutra-NumCRS-Ports-r13", c.get<different_pci_r13_s_>().eutra_num_crs_ports_r13.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::same_pci_r13:
      HANDLE_CODE(pack_integer(bref, c.get<same_pci_r13_s_>().idx_to_mid_prb_r13, (int8_t)-55, (int8_t)54));
      break;
    case types::different_pci_r13:
      HANDLE_CODE(c.get<different_pci_r13_s_>().eutra_num_crs_ports_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::same_pci_r13:
      HANDLE_CODE(unpack_integer(c.get<same_pci_r13_s_>().idx_to_mid_prb_r13, bref, (int8_t)-55, (int8_t)54));
      break;
    case types::different_pci_r13:
      HANDLE_CODE(c.get<different_pci_r13_s_>().eutra_num_crs_ports_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_::different_pci_r13_s_::
    eutra_num_crs_ports_r13_opts::to_string() const
{
  static const char* options[] = {"same", "four"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_::different_pci_"
                          "r13_s_::eutra_num_crs_ports_r13_e_");
}
uint8_t dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_::different_pci_r13_s_::
    eutra_num_crs_ports_r13_opts::to_number() const
{
  if (value == four) {
    return 4;
  }
  invalid_enum_number(value,
                      "dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_::different_pci_r13_"
                      "s_::eutra_num_crs_ports_r13_e_");
  return 0;
}

std::string dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"samePCI-r13", "differentPCI-r13"};
  return convert_enum_idx(
      options, 2, value, "dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::same_pci_ind_r13_c_::types");
}

std::string dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::eutra_ctrl_region_size_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3"};
  return convert_enum_idx(
      options, 3, value, "dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::eutra_ctrl_region_size_r13_e_");
}
uint8_t dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::eutra_ctrl_region_size_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(
      options, 3, value, "dl_carrier_cfg_ded_nb_r13_s::inband_carrier_info_r13_s_::eutra_ctrl_region_size_r13_e_");
}

std::string dl_carrier_cfg_ded_nb_r13_s::nrs_pwr_offset_non_anchor_v1330_opts::to_string() const
{
  static const char* options[] = {"dB-12", "dB-10", "dB-8", "dB-6", "dB-4", "dB-2", "dB0", "dB3"};
  return convert_enum_idx(options, 8, value, "dl_carrier_cfg_ded_nb_r13_s::nrs_pwr_offset_non_anchor_v1330_e_");
}
int8_t dl_carrier_cfg_ded_nb_r13_s::nrs_pwr_offset_non_anchor_v1330_opts::to_number() const
{
  static const int8_t options[] = {-12, -10, -8, -6, -4, -2, 0, 3};
  return map_enum_number(options, 8, value, "dl_carrier_cfg_ded_nb_r13_s::nrs_pwr_offset_non_anchor_v1330_e_");
}

// LogicalChannelConfig-NB-r13 ::= SEQUENCE
SRSASN_CODE lc_ch_cfg_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(prio_r13_present, 1));
  HANDLE_CODE(bref.pack(lc_ch_sr_prohibit_r13_present, 1));

  if (prio_r13_present) {
    HANDLE_CODE(pack_integer(bref, prio_r13, (uint8_t)1u, (uint8_t)16u));
  }
  if (lc_ch_sr_prohibit_r13_present) {
    HANDLE_CODE(bref.pack(lc_ch_sr_prohibit_r13, 1));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE lc_ch_cfg_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(prio_r13_present, 1));
  HANDLE_CODE(bref.unpack(lc_ch_sr_prohibit_r13_present, 1));

  if (prio_r13_present) {
    HANDLE_CODE(unpack_integer(prio_r13, bref, (uint8_t)1u, (uint8_t)16u));
  }
  if (lc_ch_sr_prohibit_r13_present) {
    HANDLE_CODE(bref.unpack(lc_ch_sr_prohibit_r13, 1));
  }

  return SRSASN_SUCCESS;
}
void lc_ch_cfg_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (prio_r13_present) {
    j.write_int("priority-r13", prio_r13);
  }
  if (lc_ch_sr_prohibit_r13_present) {
    j.write_bool("logicalChannelSR-Prohibit-r13", lc_ch_sr_prohibit_r13);
  }
  j.end_obj();
}

// PDCP-Config-NB-r13 ::= SEQUENCE
SRSASN_CODE pdcp_cfg_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(discard_timer_r13_present, 1));

  if (discard_timer_r13_present) {
    HANDLE_CODE(discard_timer_r13.pack(bref));
  }
  HANDLE_CODE(hdr_compress_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(discard_timer_r13_present, 1));

  if (discard_timer_r13_present) {
    HANDLE_CODE(discard_timer_r13.unpack(bref));
  }
  HANDLE_CODE(hdr_compress_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void pdcp_cfg_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (discard_timer_r13_present) {
    j.write_str("discardTimer-r13", discard_timer_r13.to_string());
  }
  j.write_fieldname("headerCompression-r13");
  hdr_compress_r13.to_json(j);
  j.end_obj();
}

std::string pdcp_cfg_nb_r13_s::discard_timer_r13_opts::to_string() const
{
  static const char* options[] = {"ms5120", "ms10240", "ms20480", "ms40960", "ms81920", "infinity", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "pdcp_cfg_nb_r13_s::discard_timer_r13_e_");
}
int32_t pdcp_cfg_nb_r13_s::discard_timer_r13_opts::to_number() const
{
  static const int32_t options[] = {5120, 10240, 20480, 40960, 81920, -1};
  return map_enum_number(options, 6, value, "pdcp_cfg_nb_r13_s::discard_timer_r13_e_");
}

void pdcp_cfg_nb_r13_s::hdr_compress_r13_c_::set(types::options e)
{
  type_ = e;
}
void pdcp_cfg_nb_r13_s::hdr_compress_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      j.write_fieldname("rohc");
      j.start_obj();
      if (c.max_cid_r13_present) {
        j.write_int("maxCID-r13", c.max_cid_r13);
      }
      j.write_fieldname("profiles-r13");
      j.start_obj();
      j.write_bool("profile0x0002", c.profiles_r13.profile0x0002);
      j.write_bool("profile0x0003", c.profiles_r13.profile0x0003);
      j.write_bool("profile0x0004", c.profiles_r13.profile0x0004);
      j.write_bool("profile0x0006", c.profiles_r13.profile0x0006);
      j.write_bool("profile0x0102", c.profiles_r13.profile0x0102);
      j.write_bool("profile0x0103", c.profiles_r13.profile0x0103);
      j.write_bool("profile0x0104", c.profiles_r13.profile0x0104);
      j.end_obj();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_nb_r13_s::hdr_compress_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdcp_cfg_nb_r13_s::hdr_compress_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      bref.pack(c.ext, 1);
      HANDLE_CODE(bref.pack(c.max_cid_r13_present, 1));
      if (c.max_cid_r13_present) {
        HANDLE_CODE(pack_integer(bref, c.max_cid_r13, (uint16_t)1u, (uint16_t)16383u));
      }
      HANDLE_CODE(bref.pack(c.profiles_r13.profile0x0002, 1));
      HANDLE_CODE(bref.pack(c.profiles_r13.profile0x0003, 1));
      HANDLE_CODE(bref.pack(c.profiles_r13.profile0x0004, 1));
      HANDLE_CODE(bref.pack(c.profiles_r13.profile0x0006, 1));
      HANDLE_CODE(bref.pack(c.profiles_r13.profile0x0102, 1));
      HANDLE_CODE(bref.pack(c.profiles_r13.profile0x0103, 1));
      HANDLE_CODE(bref.pack(c.profiles_r13.profile0x0104, 1));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_nb_r13_s::hdr_compress_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_nb_r13_s::hdr_compress_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(bref.unpack(c.max_cid_r13_present, 1));
      if (c.max_cid_r13_present) {
        HANDLE_CODE(unpack_integer(c.max_cid_r13, bref, (uint16_t)1u, (uint16_t)16383u));
      }
      HANDLE_CODE(bref.unpack(c.profiles_r13.profile0x0002, 1));
      HANDLE_CODE(bref.unpack(c.profiles_r13.profile0x0003, 1));
      HANDLE_CODE(bref.unpack(c.profiles_r13.profile0x0004, 1));
      HANDLE_CODE(bref.unpack(c.profiles_r13.profile0x0006, 1));
      HANDLE_CODE(bref.unpack(c.profiles_r13.profile0x0102, 1));
      HANDLE_CODE(bref.unpack(c.profiles_r13.profile0x0103, 1));
      HANDLE_CODE(bref.unpack(c.profiles_r13.profile0x0104, 1));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_nb_r13_s::hdr_compress_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string pdcp_cfg_nb_r13_s::hdr_compress_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"notUsed", "rohc"};
  return convert_enum_idx(options, 2, value, "pdcp_cfg_nb_r13_s::hdr_compress_r13_c_::types");
}

// RLC-Config-NB-r13 ::= CHOICE
void rlc_cfg_nb_r13_c::set(types::options e)
{
  type_ = e;
}
void rlc_cfg_nb_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::am:
      j.write_fieldname("am");
      j.start_obj();
      j.write_fieldname("ul-AM-RLC-r13");
      c.ul_am_rlc_r13.to_json(j);
      j.write_fieldname("dl-AM-RLC-r13");
      c.dl_am_rlc_r13.to_json(j);
      j.end_obj();
      break;
    case types::um_bi_dir_r15:
      break;
    case types::um_uni_dir_ul_r15:
      break;
    case types::um_uni_dir_dl_r15:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_nb_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE rlc_cfg_nb_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::am:
      HANDLE_CODE(c.ul_am_rlc_r13.pack(bref));
      HANDLE_CODE(c.dl_am_rlc_r13.pack(bref));
      break;
    case types::um_bi_dir_r15:
      break;
    case types::um_uni_dir_ul_r15:
      break;
    case types::um_uni_dir_dl_r15:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_nb_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_cfg_nb_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::am:
      HANDLE_CODE(c.ul_am_rlc_r13.unpack(bref));
      HANDLE_CODE(c.dl_am_rlc_r13.unpack(bref));
      break;
    case types::um_bi_dir_r15:
      break;
    case types::um_uni_dir_ul_r15:
      break;
    case types::um_uni_dir_dl_r15:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_nb_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rlc_cfg_nb_r13_c::types_opts::to_string() const
{
  static const char* options[] = {
      "am", "um-Bi-Directional-r15", "um-Uni-Directional-UL-r15", "um-Uni-Directional-DL-r15"};
  return convert_enum_idx(options, 4, value, "rlc_cfg_nb_r13_c::types");
}

// RLC-Config-NB-v1430 ::= SEQUENCE
SRSASN_CODE rlc_cfg_nb_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(t_reordering_r14_present, 1));

  if (t_reordering_r14_present) {
    HANDLE_CODE(t_reordering_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_cfg_nb_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(t_reordering_r14_present, 1));

  if (t_reordering_r14_present) {
    HANDLE_CODE(t_reordering_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rlc_cfg_nb_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (t_reordering_r14_present) {
    j.write_str("t-Reordering-r14", t_reordering_r14.to_string());
  }
  j.end_obj();
}

// SR-NPRACH-Resource-NB-r15 ::= SEQUENCE
SRSASN_CODE sr_nprach_res_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, nprach_carrier_idx_r15, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, nprach_res_idx_r15, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(nprach_sub_carrier_idx_r15.pack(bref));
  HANDLE_CODE(pack_integer(bref, p0_sr_r15, (int8_t)-126, (int8_t)24));
  HANDLE_CODE(alpha_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sr_nprach_res_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(nprach_carrier_idx_r15, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(nprach_res_idx_r15, bref, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(nprach_sub_carrier_idx_r15.unpack(bref));
  HANDLE_CODE(unpack_integer(p0_sr_r15, bref, (int8_t)-126, (int8_t)24));
  HANDLE_CODE(alpha_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void sr_nprach_res_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("nprach-CarrierIndex-r15", nprach_carrier_idx_r15);
  j.write_int("nprach-ResourceIndex-r15", nprach_res_idx_r15);
  j.write_fieldname("nprach-SubCarrierIndex-r15");
  nprach_sub_carrier_idx_r15.to_json(j);
  j.write_int("p0-SR-r15", p0_sr_r15);
  j.write_str("alpha-r15", alpha_r15.to_string());
  j.end_obj();
}

void sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_::destroy_() {}
void sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_::nprach_sub_carrier_idx_r15_c_(
    const sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nprach_fmt0_fmt1_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nprach_fmt2_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_");
  }
}
sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_& sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_::
                                                       operator=(const sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nprach_fmt0_fmt1_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nprach_fmt2_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_");
  }

  return *this;
}
void sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nprach_fmt0_fmt1_r15:
      j.write_int("nprach-Fmt0Fmt1-r15", c.get<uint8_t>());
      break;
    case types::nprach_fmt2_r15:
      j.write_int("nprach-Fmt2-r15", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nprach_fmt0_fmt1_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)47u));
      break;
    case types::nprach_fmt2_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)143u));
      break;
    default:
      log_invalid_choice_id(type_, "sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nprach_fmt0_fmt1_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)47u));
      break;
    case types::nprach_fmt2_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)143u));
      break;
    default:
      log_invalid_choice_id(type_, "sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"nprach-Fmt0Fmt1-r15", "nprach-Fmt2-r15"};
  return convert_enum_idx(options, 2, value, "sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_::types");
}
uint8_t sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {0, 2};
  return map_enum_number(options, 2, value, "sr_nprach_res_nb_r15_s::nprach_sub_carrier_idx_r15_c_::types");
}

std::string sr_nprach_res_nb_r15_s::alpha_r15_opts::to_string() const
{
  static const char* options[] = {"al0", "al04", "al05", "al06", "al07", "al08", "al09", "al1"};
  return convert_enum_idx(options, 8, value, "sr_nprach_res_nb_r15_s::alpha_r15_e_");
}
float sr_nprach_res_nb_r15_s::alpha_r15_opts::to_number() const
{
  static const float options[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(options, 8, value, "sr_nprach_res_nb_r15_s::alpha_r15_e_");
}
std::string sr_nprach_res_nb_r15_s::alpha_r15_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(options, 8, value, "sr_nprach_res_nb_r15_s::alpha_r15_e_");
}

// UL-CarrierConfigDedicated-NB-r13 ::= SEQUENCE
SRSASN_CODE ul_carrier_cfg_ded_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_carrier_freq_r13_present, 1));

  if (ul_carrier_freq_r13_present) {
    HANDLE_CODE(ul_carrier_freq_r13.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= tdd_ul_dl_align_offset_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(tdd_ul_dl_align_offset_r15_present, 1));
      if (tdd_ul_dl_align_offset_r15_present) {
        HANDLE_CODE(tdd_ul_dl_align_offset_r15.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_carrier_cfg_ded_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ul_carrier_freq_r13_present, 1));

  if (ul_carrier_freq_r13_present) {
    HANDLE_CODE(ul_carrier_freq_r13.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(tdd_ul_dl_align_offset_r15_present, 1));
      if (tdd_ul_dl_align_offset_r15_present) {
        HANDLE_CODE(tdd_ul_dl_align_offset_r15.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ul_carrier_cfg_ded_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_carrier_freq_r13_present) {
    j.write_fieldname("ul-CarrierFreq-r13");
    ul_carrier_freq_r13.to_json(j);
  }
  if (ext) {
    if (tdd_ul_dl_align_offset_r15_present) {
      j.write_str("tdd-UL-DL-AlignmentOffset-r15", tdd_ul_dl_align_offset_r15.to_string());
    }
  }
  j.end_obj();
}

// CarrierConfigDedicated-NB-r13 ::= SEQUENCE
SRSASN_CODE carrier_cfg_ded_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(dl_carrier_cfg_r13.pack(bref));
  HANDLE_CODE(ul_carrier_cfg_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_cfg_ded_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(dl_carrier_cfg_r13.unpack(bref));
  HANDLE_CODE(ul_carrier_cfg_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void carrier_cfg_ded_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("dl-CarrierConfig-r13");
  dl_carrier_cfg_r13.to_json(j);
  j.write_fieldname("ul-CarrierConfig-r13");
  ul_carrier_cfg_r13.to_json(j);
  j.end_obj();
}

// DRB-ToAddMod-NB-r13 ::= SEQUENCE
SRSASN_CODE drb_to_add_mod_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(eps_bearer_id_r13_present, 1));
  HANDLE_CODE(bref.pack(pdcp_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(rlc_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(lc_ch_id_r13_present, 1));
  HANDLE_CODE(bref.pack(lc_ch_cfg_r13_present, 1));

  if (eps_bearer_id_r13_present) {
    HANDLE_CODE(pack_integer(bref, eps_bearer_id_r13, (uint8_t)0u, (uint8_t)15u));
  }
  HANDLE_CODE(pack_integer(bref, drb_id_r13, (uint8_t)1u, (uint8_t)32u));
  if (pdcp_cfg_r13_present) {
    HANDLE_CODE(pdcp_cfg_r13.pack(bref));
  }
  if (rlc_cfg_r13_present) {
    HANDLE_CODE(rlc_cfg_r13.pack(bref));
  }
  if (lc_ch_id_r13_present) {
    HANDLE_CODE(pack_integer(bref, lc_ch_id_r13, (uint8_t)3u, (uint8_t)10u));
  }
  if (lc_ch_cfg_r13_present) {
    HANDLE_CODE(lc_ch_cfg_r13.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= rlc_cfg_v1430.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlc_cfg_v1430.is_present(), 1));
      if (rlc_cfg_v1430.is_present()) {
        HANDLE_CODE(rlc_cfg_v1430->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_add_mod_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(eps_bearer_id_r13_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(rlc_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(lc_ch_id_r13_present, 1));
  HANDLE_CODE(bref.unpack(lc_ch_cfg_r13_present, 1));

  if (eps_bearer_id_r13_present) {
    HANDLE_CODE(unpack_integer(eps_bearer_id_r13, bref, (uint8_t)0u, (uint8_t)15u));
  }
  HANDLE_CODE(unpack_integer(drb_id_r13, bref, (uint8_t)1u, (uint8_t)32u));
  if (pdcp_cfg_r13_present) {
    HANDLE_CODE(pdcp_cfg_r13.unpack(bref));
  }
  if (rlc_cfg_r13_present) {
    HANDLE_CODE(rlc_cfg_r13.unpack(bref));
  }
  if (lc_ch_id_r13_present) {
    HANDLE_CODE(unpack_integer(lc_ch_id_r13, bref, (uint8_t)3u, (uint8_t)10u));
  }
  if (lc_ch_cfg_r13_present) {
    HANDLE_CODE(lc_ch_cfg_r13.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rlc_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(rlc_cfg_v1430_present, 1));
      rlc_cfg_v1430.set_present(rlc_cfg_v1430_present);
      if (rlc_cfg_v1430.is_present()) {
        HANDLE_CODE(rlc_cfg_v1430->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void drb_to_add_mod_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eps_bearer_id_r13_present) {
    j.write_int("eps-BearerIdentity-r13", eps_bearer_id_r13);
  }
  j.write_int("drb-Identity-r13", drb_id_r13);
  if (pdcp_cfg_r13_present) {
    j.write_fieldname("pdcp-Config-r13");
    pdcp_cfg_r13.to_json(j);
  }
  if (rlc_cfg_r13_present) {
    j.write_fieldname("rlc-Config-r13");
    rlc_cfg_r13.to_json(j);
  }
  if (lc_ch_id_r13_present) {
    j.write_int("logicalChannelIdentity-r13", lc_ch_id_r13);
  }
  if (lc_ch_cfg_r13_present) {
    j.write_fieldname("logicalChannelConfig-r13");
    lc_ch_cfg_r13.to_json(j);
  }
  if (ext) {
    if (rlc_cfg_v1430.is_present()) {
      j.write_fieldname("rlc-Config-v1430");
      rlc_cfg_v1430->to_json(j);
    }
  }
  j.end_obj();
}

// DRX-Config-NB-r13 ::= CHOICE
void drx_cfg_nb_r13_c::set(types::options e)
{
  type_ = e;
}
void drx_cfg_nb_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("onDurationTimer-r13", c.on_dur_timer_r13.to_string());
      j.write_str("drx-InactivityTimer-r13", c.drx_inactivity_timer_r13.to_string());
      j.write_str("drx-RetransmissionTimer-r13", c.drx_retx_timer_r13.to_string());
      j.write_str("drx-Cycle-r13", c.drx_cycle_r13.to_string());
      j.write_int("drx-StartOffset-r13", c.drx_start_offset_r13);
      j.write_str("drx-ULRetransmissionTimer-r13", c.drx_ul_retx_timer_r13.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_nb_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE drx_cfg_nb_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.on_dur_timer_r13.pack(bref));
      HANDLE_CODE(c.drx_inactivity_timer_r13.pack(bref));
      HANDLE_CODE(c.drx_retx_timer_r13.pack(bref));
      HANDLE_CODE(c.drx_cycle_r13.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.drx_start_offset_r13, (uint16_t)0u, (uint16_t)255u));
      HANDLE_CODE(c.drx_ul_retx_timer_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_nb_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_nb_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.on_dur_timer_r13.unpack(bref));
      HANDLE_CODE(c.drx_inactivity_timer_r13.unpack(bref));
      HANDLE_CODE(c.drx_retx_timer_r13.unpack(bref));
      HANDLE_CODE(c.drx_cycle_r13.unpack(bref));
      HANDLE_CODE(unpack_integer(c.drx_start_offset_r13, bref, (uint16_t)0u, (uint16_t)255u));
      HANDLE_CODE(c.drx_ul_retx_timer_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_nb_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string drx_cfg_nb_r13_c::setup_s_::on_dur_timer_r13_opts::to_string() const
{
  static const char* options[] = {"pp1", "pp2", "pp3", "pp4", "pp8", "pp16", "pp32", "spare"};
  return convert_enum_idx(options, 8, value, "drx_cfg_nb_r13_c::setup_s_::on_dur_timer_r13_e_");
}
uint8_t drx_cfg_nb_r13_c::setup_s_::on_dur_timer_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 8, 16, 32};
  return map_enum_number(options, 7, value, "drx_cfg_nb_r13_c::setup_s_::on_dur_timer_r13_e_");
}

std::string drx_cfg_nb_r13_c::setup_s_::drx_inactivity_timer_r13_opts::to_string() const
{
  static const char* options[] = {"pp0", "pp1", "pp2", "pp3", "pp4", "pp8", "pp16", "pp32"};
  return convert_enum_idx(options, 8, value, "drx_cfg_nb_r13_c::setup_s_::drx_inactivity_timer_r13_e_");
}
uint8_t drx_cfg_nb_r13_c::setup_s_::drx_inactivity_timer_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 8, 16, 32};
  return map_enum_number(options, 8, value, "drx_cfg_nb_r13_c::setup_s_::drx_inactivity_timer_r13_e_");
}

std::string drx_cfg_nb_r13_c::setup_s_::drx_retx_timer_r13_opts::to_string() const
{
  static const char* options[] = {"pp0",
                                  "pp1",
                                  "pp2",
                                  "pp4",
                                  "pp6",
                                  "pp8",
                                  "pp16",
                                  "pp24",
                                  "pp33",
                                  "spare7",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "drx_cfg_nb_r13_c::setup_s_::drx_retx_timer_r13_e_");
}
uint8_t drx_cfg_nb_r13_c::setup_s_::drx_retx_timer_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 4, 6, 8, 16, 24, 33};
  return map_enum_number(options, 9, value, "drx_cfg_nb_r13_c::setup_s_::drx_retx_timer_r13_e_");
}

std::string drx_cfg_nb_r13_c::setup_s_::drx_cycle_r13_opts::to_string() const
{
  static const char* options[] = {"sf256",
                                  "sf512",
                                  "sf1024",
                                  "sf1536",
                                  "sf2048",
                                  "sf3072",
                                  "sf4096",
                                  "sf4608",
                                  "sf6144",
                                  "sf7680",
                                  "sf8192",
                                  "sf9216",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "drx_cfg_nb_r13_c::setup_s_::drx_cycle_r13_e_");
}
uint16_t drx_cfg_nb_r13_c::setup_s_::drx_cycle_r13_opts::to_number() const
{
  static const uint16_t options[] = {256, 512, 1024, 1536, 2048, 3072, 4096, 4608, 6144, 7680, 8192, 9216};
  return map_enum_number(options, 12, value, "drx_cfg_nb_r13_c::setup_s_::drx_cycle_r13_e_");
}

std::string drx_cfg_nb_r13_c::setup_s_::drx_ul_retx_timer_r13_opts::to_string() const
{
  static const char* options[] = {"pp0",
                                  "pp1",
                                  "pp2",
                                  "pp4",
                                  "pp6",
                                  "pp8",
                                  "pp16",
                                  "pp24",
                                  "pp33",
                                  "pp40",
                                  "pp64",
                                  "pp80",
                                  "pp96",
                                  "pp112",
                                  "pp128",
                                  "pp160",
                                  "pp320"};
  return convert_enum_idx(options, 17, value, "drx_cfg_nb_r13_c::setup_s_::drx_ul_retx_timer_r13_e_");
}
uint16_t drx_cfg_nb_r13_c::setup_s_::drx_ul_retx_timer_r13_opts::to_number() const
{
  static const uint16_t options[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(options, 17, value, "drx_cfg_nb_r13_c::setup_s_::drx_ul_retx_timer_r13_e_");
}

// NPDCCH-ConfigDedicated-NB-r13 ::= SEQUENCE
SRSASN_CODE npdcch_cfg_ded_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(npdcch_num_repeats_r13.pack(bref));
  HANDLE_CODE(npdcch_start_sf_uss_r13.pack(bref));
  HANDLE_CODE(npdcch_offset_uss_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE npdcch_cfg_ded_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(npdcch_num_repeats_r13.unpack(bref));
  HANDLE_CODE(npdcch_start_sf_uss_r13.unpack(bref));
  HANDLE_CODE(npdcch_offset_uss_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void npdcch_cfg_ded_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("npdcch-NumRepetitions-r13", npdcch_num_repeats_r13.to_string());
  j.write_str("npdcch-StartSF-USS-r13", npdcch_start_sf_uss_r13.to_string());
  j.write_str("npdcch-Offset-USS-r13", npdcch_offset_uss_r13.to_string());
  j.end_obj();
}

std::string npdcch_cfg_ded_nb_r13_s::npdcch_num_repeats_r13_opts::to_string() const
{
  static const char* options[] = {"r1",
                                  "r2",
                                  "r4",
                                  "r8",
                                  "r16",
                                  "r32",
                                  "r64",
                                  "r128",
                                  "r256",
                                  "r512",
                                  "r1024",
                                  "r2048",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "npdcch_cfg_ded_nb_r13_s::npdcch_num_repeats_r13_e_");
}
uint16_t npdcch_cfg_ded_nb_r13_s::npdcch_num_repeats_r13_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
  return map_enum_number(options, 12, value, "npdcch_cfg_ded_nb_r13_s::npdcch_num_repeats_r13_e_");
}

std::string npdcch_cfg_ded_nb_r13_s::npdcch_start_sf_uss_r13_opts::to_string() const
{
  static const char* options[] = {"v1dot5", "v2", "v4", "v8", "v16", "v32", "v48", "v64"};
  return convert_enum_idx(options, 8, value, "npdcch_cfg_ded_nb_r13_s::npdcch_start_sf_uss_r13_e_");
}
float npdcch_cfg_ded_nb_r13_s::npdcch_start_sf_uss_r13_opts::to_number() const
{
  static const float options[] = {1.5, 2.0, 4.0, 8.0, 16.0, 32.0, 48.0, 64.0};
  return map_enum_number(options, 8, value, "npdcch_cfg_ded_nb_r13_s::npdcch_start_sf_uss_r13_e_");
}
std::string npdcch_cfg_ded_nb_r13_s::npdcch_start_sf_uss_r13_opts::to_number_string() const
{
  static const char* options[] = {"1.5", "2", "4", "8", "16", "32", "48", "64"};
  return convert_enum_idx(options, 8, value, "npdcch_cfg_ded_nb_r13_s::npdcch_start_sf_uss_r13_e_");
}

std::string npdcch_cfg_ded_nb_r13_s::npdcch_offset_uss_r13_opts::to_string() const
{
  static const char* options[] = {"zero", "oneEighth", "oneFourth", "threeEighth"};
  return convert_enum_idx(options, 4, value, "npdcch_cfg_ded_nb_r13_s::npdcch_offset_uss_r13_e_");
}
float npdcch_cfg_ded_nb_r13_s::npdcch_offset_uss_r13_opts::to_number() const
{
  static const float options[] = {0.0, 0.125, 0.25, 0.375};
  return map_enum_number(options, 4, value, "npdcch_cfg_ded_nb_r13_s::npdcch_offset_uss_r13_e_");
}
std::string npdcch_cfg_ded_nb_r13_s::npdcch_offset_uss_r13_opts::to_number_string() const
{
  static const char* options[] = {"0", "1/8", "1/4", "3/8"};
  return convert_enum_idx(options, 4, value, "npdcch_cfg_ded_nb_r13_s::npdcch_offset_uss_r13_e_");
}

// NPDCCH-ConfigDedicated-NB-v1530 ::= SEQUENCE
SRSASN_CODE npdcch_cfg_ded_nb_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(npdcch_start_sf_uss_v1530.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE npdcch_cfg_ded_nb_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(npdcch_start_sf_uss_v1530.unpack(bref));

  return SRSASN_SUCCESS;
}
void npdcch_cfg_ded_nb_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("npdcch-StartSF-USS-v1530", npdcch_start_sf_uss_v1530.to_string());
  j.end_obj();
}

std::string npdcch_cfg_ded_nb_v1530_s::npdcch_start_sf_uss_v1530_opts::to_string() const
{
  static const char* options[] = {"v96", "v128"};
  return convert_enum_idx(options, 2, value, "npdcch_cfg_ded_nb_v1530_s::npdcch_start_sf_uss_v1530_e_");
}
uint8_t npdcch_cfg_ded_nb_v1530_s::npdcch_start_sf_uss_v1530_opts::to_number() const
{
  static const uint8_t options[] = {96, 128};
  return map_enum_number(options, 2, value, "npdcch_cfg_ded_nb_v1530_s::npdcch_start_sf_uss_v1530_e_");
}

// NPUSCH-ConfigDedicated-NB-r13 ::= SEQUENCE
SRSASN_CODE npusch_cfg_ded_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ack_nack_num_repeats_r13_present, 1));
  HANDLE_CODE(bref.pack(npusch_all_symbols_r13_present, 1));
  HANDLE_CODE(bref.pack(group_hop_disabled_r13_present, 1));

  if (ack_nack_num_repeats_r13_present) {
    HANDLE_CODE(ack_nack_num_repeats_r13.pack(bref));
  }
  if (npusch_all_symbols_r13_present) {
    HANDLE_CODE(bref.pack(npusch_all_symbols_r13, 1));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE npusch_cfg_ded_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ack_nack_num_repeats_r13_present, 1));
  HANDLE_CODE(bref.unpack(npusch_all_symbols_r13_present, 1));
  HANDLE_CODE(bref.unpack(group_hop_disabled_r13_present, 1));

  if (ack_nack_num_repeats_r13_present) {
    HANDLE_CODE(ack_nack_num_repeats_r13.unpack(bref));
  }
  if (npusch_all_symbols_r13_present) {
    HANDLE_CODE(bref.unpack(npusch_all_symbols_r13, 1));
  }

  return SRSASN_SUCCESS;
}
void npusch_cfg_ded_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ack_nack_num_repeats_r13_present) {
    j.write_str("ack-NACK-NumRepetitions-r13", ack_nack_num_repeats_r13.to_string());
  }
  if (npusch_all_symbols_r13_present) {
    j.write_bool("npusch-AllSymbols-r13", npusch_all_symbols_r13);
  }
  if (group_hop_disabled_r13_present) {
    j.write_str("groupHoppingDisabled-r13", "true");
  }
  j.end_obj();
}

// PeriodicBSR-Timer-NB-r13 ::= ENUMERATED
std::string periodic_bsr_timer_nb_r13_opts::to_string() const
{
  static const char* options[] = {"pp2", "pp4", "pp8", "pp16", "pp64", "pp128", "infinity", "spare"};
  return convert_enum_idx(options, 8, value, "periodic_bsr_timer_nb_r13_e");
}
int16_t periodic_bsr_timer_nb_r13_opts::to_number() const
{
  static const int16_t options[] = {2, 4, 8, 16, 64, 128, -1};
  return map_enum_number(options, 7, value, "periodic_bsr_timer_nb_r13_e");
}

// RetxBSR-Timer-NB-r13 ::= ENUMERATED
std::string retx_bsr_timer_nb_r13_opts::to_string() const
{
  static const char* options[] = {"pp4", "pp16", "pp64", "pp128", "pp256", "pp512", "infinity", "spare"};
  return convert_enum_idx(options, 8, value, "retx_bsr_timer_nb_r13_e");
}
int16_t retx_bsr_timer_nb_r13_opts::to_number() const
{
  static const int16_t options[] = {4, 16, 64, 128, 256, 512, -1};
  return map_enum_number(options, 7, value, "retx_bsr_timer_nb_r13_e");
}

// SR-SPS-BSR-Config-NB-r15 ::= CHOICE
void sr_sps_bsr_cfg_nb_r15_c::set(types::options e)
{
  type_ = e;
}
void sr_sps_bsr_cfg_nb_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("semiPersistSchedC-RNTI-r15", c.semi_persist_sched_c_rnti_r15.to_string());
      j.write_str("semiPersistSchedIntervalUL-r15", c.semi_persist_sched_interv_ul_r15.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sr_sps_bsr_cfg_nb_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE sr_sps_bsr_cfg_nb_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.semi_persist_sched_c_rnti_r15.pack(bref));
      HANDLE_CODE(c.semi_persist_sched_interv_ul_r15.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sr_sps_bsr_cfg_nb_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sr_sps_bsr_cfg_nb_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.semi_persist_sched_c_rnti_r15.unpack(bref));
      HANDLE_CODE(c.semi_persist_sched_interv_ul_r15.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sr_sps_bsr_cfg_nb_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string sr_sps_bsr_cfg_nb_r15_c::setup_s_::semi_persist_sched_interv_ul_r15_opts::to_string() const
{
  static const char* options[] = {"sf128", "sf256", "sf512", "sf1024", "sf1280", "sf2048", "sf2560", "sf5120"};
  return convert_enum_idx(options, 8, value, "sr_sps_bsr_cfg_nb_r15_c::setup_s_::semi_persist_sched_interv_ul_r15_e_");
}
uint16_t sr_sps_bsr_cfg_nb_r15_c::setup_s_::semi_persist_sched_interv_ul_r15_opts::to_number() const
{
  static const uint16_t options[] = {128, 256, 512, 1024, 1280, 2048, 2560, 5120};
  return map_enum_number(options, 8, value, "sr_sps_bsr_cfg_nb_r15_c::setup_s_::semi_persist_sched_interv_ul_r15_e_");
}

// SR-WithoutHARQ-ACK-Config-NB-r15 ::= CHOICE
void sr_without_harq_ack_cfg_nb_r15_c::set(types::options e)
{
  type_ = e;
}
void sr_without_harq_ack_cfg_nb_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.sr_prohibit_timer_r15_present) {
        j.write_int("sr-ProhibitTimer-r15", c.sr_prohibit_timer_r15);
      }
      if (c.sr_nprach_res_r15_present) {
        j.write_fieldname("sr-NPRACH-Resource-r15");
        c.sr_nprach_res_r15.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sr_without_harq_ack_cfg_nb_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE sr_without_harq_ack_cfg_nb_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.sr_prohibit_timer_r15_present, 1));
      HANDLE_CODE(bref.pack(c.sr_nprach_res_r15_present, 1));
      if (c.sr_prohibit_timer_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.sr_prohibit_timer_r15, (uint8_t)0u, (uint8_t)7u));
      }
      if (c.sr_nprach_res_r15_present) {
        HANDLE_CODE(c.sr_nprach_res_r15.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sr_without_harq_ack_cfg_nb_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sr_without_harq_ack_cfg_nb_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.sr_prohibit_timer_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.sr_nprach_res_r15_present, 1));
      if (c.sr_prohibit_timer_r15_present) {
        HANDLE_CODE(unpack_integer(c.sr_prohibit_timer_r15, bref, (uint8_t)0u, (uint8_t)7u));
      }
      if (c.sr_nprach_res_r15_present) {
        HANDLE_CODE(c.sr_nprach_res_r15.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sr_without_harq_ack_cfg_nb_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SRB-ToAddMod-NB-r13 ::= SEQUENCE
SRSASN_CODE srb_to_add_mod_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rlc_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(lc_ch_cfg_r13_present, 1));

  if (rlc_cfg_r13_present) {
    HANDLE_CODE(rlc_cfg_r13.pack(bref));
  }
  if (lc_ch_cfg_r13_present) {
    HANDLE_CODE(lc_ch_cfg_r13.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= rlc_cfg_v1430.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlc_cfg_v1430.is_present(), 1));
      if (rlc_cfg_v1430.is_present()) {
        HANDLE_CODE(rlc_cfg_v1430->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srb_to_add_mod_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rlc_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(lc_ch_cfg_r13_present, 1));

  if (rlc_cfg_r13_present) {
    HANDLE_CODE(rlc_cfg_r13.unpack(bref));
  }
  if (lc_ch_cfg_r13_present) {
    HANDLE_CODE(lc_ch_cfg_r13.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rlc_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(rlc_cfg_v1430_present, 1));
      rlc_cfg_v1430.set_present(rlc_cfg_v1430_present);
      if (rlc_cfg_v1430.is_present()) {
        HANDLE_CODE(rlc_cfg_v1430->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void srb_to_add_mod_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rlc_cfg_r13_present) {
    j.write_fieldname("rlc-Config-r13");
    rlc_cfg_r13.to_json(j);
  }
  if (lc_ch_cfg_r13_present) {
    j.write_fieldname("logicalChannelConfig-r13");
    lc_ch_cfg_r13.to_json(j);
  }
  if (ext) {
    if (rlc_cfg_v1430.is_present()) {
      j.write_fieldname("rlc-Config-v1430");
      rlc_cfg_v1430->to_json(j);
    }
  }
  j.end_obj();
}

void srb_to_add_mod_nb_r13_s::rlc_cfg_r13_c_::set(types::options e)
{
  type_ = e;
}
void srb_to_add_mod_nb_r13_s::rlc_cfg_r13_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "srb_to_add_mod_nb_r13_s::rlc_cfg_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE srb_to_add_mod_nb_r13_s::rlc_cfg_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::explicit_value:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "srb_to_add_mod_nb_r13_s::rlc_cfg_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srb_to_add_mod_nb_r13_s::rlc_cfg_r13_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "srb_to_add_mod_nb_r13_s::rlc_cfg_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string srb_to_add_mod_nb_r13_s::rlc_cfg_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue", "defaultValue"};
  return convert_enum_idx(options, 2, value, "srb_to_add_mod_nb_r13_s::rlc_cfg_r13_c_::types");
}

void srb_to_add_mod_nb_r13_s::lc_ch_cfg_r13_c_::set(types::options e)
{
  type_ = e;
}
void srb_to_add_mod_nb_r13_s::lc_ch_cfg_r13_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "srb_to_add_mod_nb_r13_s::lc_ch_cfg_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE srb_to_add_mod_nb_r13_s::lc_ch_cfg_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::explicit_value:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "srb_to_add_mod_nb_r13_s::lc_ch_cfg_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srb_to_add_mod_nb_r13_s::lc_ch_cfg_r13_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "srb_to_add_mod_nb_r13_s::lc_ch_cfg_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string srb_to_add_mod_nb_r13_s::lc_ch_cfg_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue", "defaultValue"};
  return convert_enum_idx(options, 2, value, "srb_to_add_mod_nb_r13_s::lc_ch_cfg_r13_c_::types");
}

// UplinkPowerControlDedicated-NB-r13 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_ded_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, p0_ue_npusch_r13, (int8_t)-8, (int8_t)7));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_ded_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(p0_ue_npusch_r13, bref, (int8_t)-8, (int8_t)7));

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_ded_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p0-UE-NPUSCH-r13", p0_ue_npusch_r13);
  j.end_obj();
}

// MAC-MainConfig-NB-r13 ::= SEQUENCE
SRSASN_CODE mac_main_cfg_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_sch_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(drx_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(lc_ch_sr_cfg_r13_present, 1));

  if (ul_sch_cfg_r13_present) {
    HANDLE_CODE(bref.pack(ul_sch_cfg_r13.periodic_bsr_timer_r13_present, 1));
    if (ul_sch_cfg_r13.periodic_bsr_timer_r13_present) {
      HANDLE_CODE(ul_sch_cfg_r13.periodic_bsr_timer_r13.pack(bref));
    }
    HANDLE_CODE(ul_sch_cfg_r13.retx_bsr_timer_r13.pack(bref));
  }
  if (drx_cfg_r13_present) {
    HANDLE_CODE(drx_cfg_r13.pack(bref));
  }
  HANDLE_CODE(time_align_timer_ded_r13.pack(bref));
  if (lc_ch_sr_cfg_r13_present) {
    HANDLE_CODE(lc_ch_sr_cfg_r13.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= rai_activation_r14_present;
    group_flags[0] |= data_inactivity_timer_cfg_r14.is_present();
    group_flags[1] |= drx_cycle_v1430_present;
    group_flags[2] |= ra_cfra_cfg_r14_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rai_activation_r14_present, 1));
      HANDLE_CODE(bref.pack(data_inactivity_timer_cfg_r14.is_present(), 1));
      if (data_inactivity_timer_cfg_r14.is_present()) {
        HANDLE_CODE(data_inactivity_timer_cfg_r14->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(drx_cycle_v1430_present, 1));
      if (drx_cycle_v1430_present) {
        HANDLE_CODE(drx_cycle_v1430.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ra_cfra_cfg_r14_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ul_sch_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(drx_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(lc_ch_sr_cfg_r13_present, 1));

  if (ul_sch_cfg_r13_present) {
    HANDLE_CODE(bref.unpack(ul_sch_cfg_r13.periodic_bsr_timer_r13_present, 1));
    if (ul_sch_cfg_r13.periodic_bsr_timer_r13_present) {
      HANDLE_CODE(ul_sch_cfg_r13.periodic_bsr_timer_r13.unpack(bref));
    }
    HANDLE_CODE(ul_sch_cfg_r13.retx_bsr_timer_r13.unpack(bref));
  }
  if (drx_cfg_r13_present) {
    HANDLE_CODE(drx_cfg_r13.unpack(bref));
  }
  HANDLE_CODE(time_align_timer_ded_r13.unpack(bref));
  if (lc_ch_sr_cfg_r13_present) {
    HANDLE_CODE(lc_ch_sr_cfg_r13.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(rai_activation_r14_present, 1));
      bool data_inactivity_timer_cfg_r14_present;
      HANDLE_CODE(bref.unpack(data_inactivity_timer_cfg_r14_present, 1));
      data_inactivity_timer_cfg_r14.set_present(data_inactivity_timer_cfg_r14_present);
      if (data_inactivity_timer_cfg_r14.is_present()) {
        HANDLE_CODE(data_inactivity_timer_cfg_r14->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(drx_cycle_v1430_present, 1));
      if (drx_cycle_v1430_present) {
        HANDLE_CODE(drx_cycle_v1430.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ra_cfra_cfg_r14_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void mac_main_cfg_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_sch_cfg_r13_present) {
    j.write_fieldname("ul-SCH-Config-r13");
    j.start_obj();
    if (ul_sch_cfg_r13.periodic_bsr_timer_r13_present) {
      j.write_str("periodicBSR-Timer-r13", ul_sch_cfg_r13.periodic_bsr_timer_r13.to_string());
    }
    j.write_str("retxBSR-Timer-r13", ul_sch_cfg_r13.retx_bsr_timer_r13.to_string());
    j.end_obj();
  }
  if (drx_cfg_r13_present) {
    j.write_fieldname("drx-Config-r13");
    drx_cfg_r13.to_json(j);
  }
  j.write_str("timeAlignmentTimerDedicated-r13", time_align_timer_ded_r13.to_string());
  if (lc_ch_sr_cfg_r13_present) {
    j.write_fieldname("logicalChannelSR-Config-r13");
    lc_ch_sr_cfg_r13.to_json(j);
  }
  if (ext) {
    if (rai_activation_r14_present) {
      j.write_str("rai-Activation-r14", "true");
    }
    if (data_inactivity_timer_cfg_r14.is_present()) {
      j.write_fieldname("dataInactivityTimerConfig-r14");
      data_inactivity_timer_cfg_r14->to_json(j);
    }
    if (drx_cycle_v1430_present) {
      j.write_str("drx-Cycle-v1430", drx_cycle_v1430.to_string());
    }
    if (ra_cfra_cfg_r14_present) {
      j.write_str("ra-CFRA-Config-r14", "true");
    }
  }
  j.end_obj();
}

void mac_main_cfg_nb_r13_s::lc_ch_sr_cfg_r13_c_::set(types::options e)
{
  type_ = e;
}
void mac_main_cfg_nb_r13_s::lc_ch_sr_cfg_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("logicalChannelSR-ProhibitTimer-r13", c.lc_ch_sr_prohibit_timer_r13.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_nb_r13_s::lc_ch_sr_cfg_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE mac_main_cfg_nb_r13_s::lc_ch_sr_cfg_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.lc_ch_sr_prohibit_timer_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_nb_r13_s::lc_ch_sr_cfg_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_nb_r13_s::lc_ch_sr_cfg_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.lc_ch_sr_prohibit_timer_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_nb_r13_s::lc_ch_sr_cfg_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string mac_main_cfg_nb_r13_s::lc_ch_sr_cfg_r13_c_::setup_s_::lc_ch_sr_prohibit_timer_r13_opts::to_string() const
{
  static const char* options[] = {"pp2", "pp8", "pp32", "pp128", "pp512", "pp1024", "pp2048", "spare"};
  return convert_enum_idx(
      options, 8, value, "mac_main_cfg_nb_r13_s::lc_ch_sr_cfg_r13_c_::setup_s_::lc_ch_sr_prohibit_timer_r13_e_");
}
uint16_t mac_main_cfg_nb_r13_s::lc_ch_sr_cfg_r13_c_::setup_s_::lc_ch_sr_prohibit_timer_r13_opts::to_number() const
{
  static const uint16_t options[] = {2, 8, 32, 128, 512, 1024, 2048};
  return map_enum_number(
      options, 7, value, "mac_main_cfg_nb_r13_s::lc_ch_sr_cfg_r13_c_::setup_s_::lc_ch_sr_prohibit_timer_r13_e_");
}

void mac_main_cfg_nb_r13_s::data_inactivity_timer_cfg_r14_c_::set(types::options e)
{
  type_ = e;
}
void mac_main_cfg_nb_r13_s::data_inactivity_timer_cfg_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("dataInactivityTimer-r14", c.data_inactivity_timer_r14.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_nb_r13_s::data_inactivity_timer_cfg_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE mac_main_cfg_nb_r13_s::data_inactivity_timer_cfg_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.data_inactivity_timer_r14.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_nb_r13_s::data_inactivity_timer_cfg_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_nb_r13_s::data_inactivity_timer_cfg_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.data_inactivity_timer_r14.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_nb_r13_s::data_inactivity_timer_cfg_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string mac_main_cfg_nb_r13_s::drx_cycle_v1430_opts::to_string() const
{
  static const char* options[] = {"sf1280", "sf2560", "sf5120", "sf10240"};
  return convert_enum_idx(options, 4, value, "mac_main_cfg_nb_r13_s::drx_cycle_v1430_e_");
}
uint16_t mac_main_cfg_nb_r13_s::drx_cycle_v1430_opts::to_number() const
{
  static const uint16_t options[] = {1280, 2560, 5120, 10240};
  return map_enum_number(options, 4, value, "mac_main_cfg_nb_r13_s::drx_cycle_v1430_e_");
}

// PhysicalConfigDedicated-NB-r13 ::= SEQUENCE
SRSASN_CODE phys_cfg_ded_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(carrier_cfg_ded_r13_present, 1));
  HANDLE_CODE(bref.pack(npdcch_cfg_ded_r13_present, 1));
  HANDLE_CODE(bref.pack(npusch_cfg_ded_r13_present, 1));
  HANDLE_CODE(bref.pack(ul_pwr_ctrl_ded_r13_present, 1));

  if (carrier_cfg_ded_r13_present) {
    HANDLE_CODE(carrier_cfg_ded_r13.pack(bref));
  }
  if (npdcch_cfg_ded_r13_present) {
    HANDLE_CODE(npdcch_cfg_ded_r13.pack(bref));
  }
  if (npusch_cfg_ded_r13_present) {
    HANDLE_CODE(npusch_cfg_ded_r13.pack(bref));
  }
  if (ul_pwr_ctrl_ded_r13_present) {
    HANDLE_CODE(ul_pwr_ctrl_ded_r13.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= two_harq_processes_cfg_r14_present;
    group_flags[1] |= interference_randomisation_cfg_r14_present;
    group_flags[2] |= npdcch_cfg_ded_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(two_harq_processes_cfg_r14_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(interference_randomisation_cfg_r14_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(npdcch_cfg_ded_v1530.is_present(), 1));
      if (npdcch_cfg_ded_v1530.is_present()) {
        HANDLE_CODE(npdcch_cfg_ded_v1530->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(carrier_cfg_ded_r13_present, 1));
  HANDLE_CODE(bref.unpack(npdcch_cfg_ded_r13_present, 1));
  HANDLE_CODE(bref.unpack(npusch_cfg_ded_r13_present, 1));
  HANDLE_CODE(bref.unpack(ul_pwr_ctrl_ded_r13_present, 1));

  if (carrier_cfg_ded_r13_present) {
    HANDLE_CODE(carrier_cfg_ded_r13.unpack(bref));
  }
  if (npdcch_cfg_ded_r13_present) {
    HANDLE_CODE(npdcch_cfg_ded_r13.unpack(bref));
  }
  if (npusch_cfg_ded_r13_present) {
    HANDLE_CODE(npusch_cfg_ded_r13.unpack(bref));
  }
  if (ul_pwr_ctrl_ded_r13_present) {
    HANDLE_CODE(ul_pwr_ctrl_ded_r13.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(two_harq_processes_cfg_r14_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(interference_randomisation_cfg_r14_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool npdcch_cfg_ded_v1530_present;
      HANDLE_CODE(bref.unpack(npdcch_cfg_ded_v1530_present, 1));
      npdcch_cfg_ded_v1530.set_present(npdcch_cfg_ded_v1530_present);
      if (npdcch_cfg_ded_v1530.is_present()) {
        HANDLE_CODE(npdcch_cfg_ded_v1530->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void phys_cfg_ded_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_cfg_ded_r13_present) {
    j.write_fieldname("carrierConfigDedicated-r13");
    carrier_cfg_ded_r13.to_json(j);
  }
  if (npdcch_cfg_ded_r13_present) {
    j.write_fieldname("npdcch-ConfigDedicated-r13");
    npdcch_cfg_ded_r13.to_json(j);
  }
  if (npusch_cfg_ded_r13_present) {
    j.write_fieldname("npusch-ConfigDedicated-r13");
    npusch_cfg_ded_r13.to_json(j);
  }
  if (ul_pwr_ctrl_ded_r13_present) {
    j.write_fieldname("uplinkPowerControlDedicated-r13");
    ul_pwr_ctrl_ded_r13.to_json(j);
  }
  if (ext) {
    if (two_harq_processes_cfg_r14_present) {
      j.write_str("twoHARQ-ProcessesConfig-r14", "true");
    }
    if (interference_randomisation_cfg_r14_present) {
      j.write_str("interferenceRandomisationConfig-r14", "true");
    }
    if (npdcch_cfg_ded_v1530.is_present()) {
      j.write_fieldname("npdcch-ConfigDedicated-v1530");
      npdcch_cfg_ded_v1530->to_json(j);
    }
  }
  j.end_obj();
}

// RLF-TimersAndConstants-NB-r13 ::= CHOICE
void rlf_timers_and_consts_nb_r13_c::set(types::options e)
{
  type_ = e;
}
void rlf_timers_and_consts_nb_r13_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "rlf_timers_and_consts_nb_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE rlf_timers_and_consts_nb_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_timers_and_consts_nb_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_timers_and_consts_nb_r13_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "rlf_timers_and_consts_nb_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE rlf_timers_and_consts_nb_r13_c::setup_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(t301_r13.pack(bref));
  HANDLE_CODE(t310_r13.pack(bref));
  HANDLE_CODE(n310_r13.pack(bref));
  HANDLE_CODE(t311_r13.pack(bref));
  HANDLE_CODE(n311_r13.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= t311_v1350_present;
    group_flags[1] |= t301_v1530_present;
    group_flags[1] |= t311_v1530_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(t311_v1350_present, 1));
      if (t311_v1350_present) {
        HANDLE_CODE(t311_v1350.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(t301_v1530_present, 1));
      HANDLE_CODE(bref.pack(t311_v1530_present, 1));
      if (t301_v1530_present) {
        HANDLE_CODE(t301_v1530.pack(bref));
      }
      if (t311_v1530_present) {
        HANDLE_CODE(t311_v1530.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_timers_and_consts_nb_r13_c::setup_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(t301_r13.unpack(bref));
  HANDLE_CODE(t310_r13.unpack(bref));
  HANDLE_CODE(n310_r13.unpack(bref));
  HANDLE_CODE(t311_r13.unpack(bref));
  HANDLE_CODE(n311_r13.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(t311_v1350_present, 1));
      if (t311_v1350_present) {
        HANDLE_CODE(t311_v1350.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(t301_v1530_present, 1));
      HANDLE_CODE(bref.unpack(t311_v1530_present, 1));
      if (t301_v1530_present) {
        HANDLE_CODE(t301_v1530.unpack(bref));
      }
      if (t311_v1530_present) {
        HANDLE_CODE(t311_v1530.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rlf_timers_and_consts_nb_r13_c::setup_s_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t301-r13", t301_r13.to_string());
  j.write_str("t310-r13", t310_r13.to_string());
  j.write_str("n310-r13", n310_r13.to_string());
  j.write_str("t311-r13", t311_r13.to_string());
  j.write_str("n311-r13", n311_r13.to_string());
  if (ext) {
    if (t311_v1350_present) {
      j.write_str("t311-v1350", t311_v1350.to_string());
    }
    if (t301_v1530_present) {
      j.write_str("t301-v1530", t301_v1530.to_string());
    }
    if (t311_v1530_present) {
      j.write_str("t311-v1530", t311_v1530.to_string());
    }
  }
  j.end_obj();
}

std::string rlf_timers_and_consts_nb_r13_c::setup_s_::t301_r13_opts::to_string() const
{
  static const char* options[] = {"ms2500", "ms4000", "ms6000", "ms10000", "ms15000", "ms25000", "ms40000", "ms60000"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::t301_r13_e_");
}
uint16_t rlf_timers_and_consts_nb_r13_c::setup_s_::t301_r13_opts::to_number() const
{
  static const uint16_t options[] = {2500, 4000, 6000, 10000, 15000, 25000, 40000, 60000};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::t301_r13_e_");
}

std::string rlf_timers_and_consts_nb_r13_c::setup_s_::t310_r13_opts::to_string() const
{
  static const char* options[] = {"ms0", "ms200", "ms500", "ms1000", "ms2000", "ms4000", "ms8000"};
  return convert_enum_idx(options, 7, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::t310_r13_e_");
}
uint16_t rlf_timers_and_consts_nb_r13_c::setup_s_::t310_r13_opts::to_number() const
{
  static const uint16_t options[] = {0, 200, 500, 1000, 2000, 4000, 8000};
  return map_enum_number(options, 7, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::t310_r13_e_");
}

std::string rlf_timers_and_consts_nb_r13_c::setup_s_::n310_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::n310_r13_e_");
}
uint8_t rlf_timers_and_consts_nb_r13_c::setup_s_::n310_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::n310_r13_e_");
}

std::string rlf_timers_and_consts_nb_r13_c::setup_s_::t311_r13_opts::to_string() const
{
  static const char* options[] = {"ms1000", "ms3000", "ms5000", "ms10000", "ms15000", "ms20000", "ms30000"};
  return convert_enum_idx(options, 7, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::t311_r13_e_");
}
uint16_t rlf_timers_and_consts_nb_r13_c::setup_s_::t311_r13_opts::to_number() const
{
  static const uint16_t options[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return map_enum_number(options, 7, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::t311_r13_e_");
}

std::string rlf_timers_and_consts_nb_r13_c::setup_s_::n311_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::n311_r13_e_");
}
uint8_t rlf_timers_and_consts_nb_r13_c::setup_s_::n311_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::n311_r13_e_");
}

std::string rlf_timers_and_consts_nb_r13_c::setup_s_::t311_v1350_opts::to_string() const
{
  static const char* options[] = {"ms40000", "ms60000", "ms90000", "ms120000"};
  return convert_enum_idx(options, 4, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::t311_v1350_e_");
}
uint32_t rlf_timers_and_consts_nb_r13_c::setup_s_::t311_v1350_opts::to_number() const
{
  static const uint32_t options[] = {40000, 60000, 90000, 120000};
  return map_enum_number(options, 4, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::t311_v1350_e_");
}

std::string rlf_timers_and_consts_nb_r13_c::setup_s_::t301_v1530_opts::to_string() const
{
  static const char* options[] = {"ms80000", "ms100000", "ms120000"};
  return convert_enum_idx(options, 3, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::t301_v1530_e_");
}
uint32_t rlf_timers_and_consts_nb_r13_c::setup_s_::t301_v1530_opts::to_number() const
{
  static const uint32_t options[] = {80000, 100000, 120000};
  return map_enum_number(options, 3, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::t301_v1530_e_");
}

std::string rlf_timers_and_consts_nb_r13_c::setup_s_::t311_v1530_opts::to_string() const
{
  static const char* options[] = {"ms160000", "ms200000"};
  return convert_enum_idx(options, 2, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::t311_v1530_e_");
}
uint32_t rlf_timers_and_consts_nb_r13_c::setup_s_::t311_v1530_opts::to_number() const
{
  static const uint32_t options[] = {160000, 200000};
  return map_enum_number(options, 2, value, "rlf_timers_and_consts_nb_r13_c::setup_s_::t311_v1530_e_");
}

// SchedulingRequestConfig-NB-r15 ::= SEQUENCE
SRSASN_CODE sched_request_cfg_nb_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sr_with_harq_ack_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(sr_without_harq_ack_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(sr_sps_bsr_cfg_r15_present, 1));

  if (sr_without_harq_ack_cfg_r15_present) {
    HANDLE_CODE(sr_without_harq_ack_cfg_r15.pack(bref));
  }
  if (sr_sps_bsr_cfg_r15_present) {
    HANDLE_CODE(sr_sps_bsr_cfg_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_request_cfg_nb_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sr_with_harq_ack_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(sr_without_harq_ack_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(sr_sps_bsr_cfg_r15_present, 1));

  if (sr_without_harq_ack_cfg_r15_present) {
    HANDLE_CODE(sr_without_harq_ack_cfg_r15.unpack(bref));
  }
  if (sr_sps_bsr_cfg_r15_present) {
    HANDLE_CODE(sr_sps_bsr_cfg_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sched_request_cfg_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sr_with_harq_ack_cfg_r15_present) {
    j.write_str("sr-WithHARQ-ACK-Config-r15", "true");
  }
  if (sr_without_harq_ack_cfg_r15_present) {
    j.write_fieldname("sr-WithoutHARQ-ACK-Config-r15");
    sr_without_harq_ack_cfg_r15.to_json(j);
  }
  if (sr_sps_bsr_cfg_r15_present) {
    j.write_fieldname("sr-SPS-BSR-Config-r15");
    sr_sps_bsr_cfg_r15.to_json(j);
  }
  j.end_obj();
}

// RadioResourceConfigDedicated-NB-r13 ::= SEQUENCE
SRSASN_CODE rr_cfg_ded_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(srb_to_add_mod_list_r13_present, 1));
  HANDLE_CODE(bref.pack(drb_to_add_mod_list_r13_present, 1));
  HANDLE_CODE(bref.pack(drb_to_release_list_r13_present, 1));
  HANDLE_CODE(bref.pack(mac_main_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(phys_cfg_ded_r13_present, 1));
  HANDLE_CODE(bref.pack(rlf_timers_and_consts_r13_present, 1));

  if (srb_to_add_mod_list_r13_present) {
    HANDLE_CODE(pack_fixed_seq_of(bref, &(srb_to_add_mod_list_r13)[0], srb_to_add_mod_list_r13.size()));
  }
  if (drb_to_add_mod_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_add_mod_list_r13, 1, 2));
  }
  if (drb_to_release_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_release_list_r13, 1, 2, integer_packer<uint8_t>(1, 32)));
  }
  if (mac_main_cfg_r13_present) {
    HANDLE_CODE(mac_main_cfg_r13.pack(bref));
  }
  if (phys_cfg_ded_r13_present) {
    HANDLE_CODE(phys_cfg_ded_r13.pack(bref));
  }
  if (rlf_timers_and_consts_r13_present) {
    HANDLE_CODE(rlf_timers_and_consts_r13.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sched_request_cfg_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sched_request_cfg_r15.is_present(), 1));
      if (sched_request_cfg_r15.is_present()) {
        HANDLE_CODE(sched_request_cfg_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_ded_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(srb_to_add_mod_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(drb_to_add_mod_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(drb_to_release_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(mac_main_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(phys_cfg_ded_r13_present, 1));
  HANDLE_CODE(bref.unpack(rlf_timers_and_consts_r13_present, 1));

  if (srb_to_add_mod_list_r13_present) {
    HANDLE_CODE(unpack_fixed_seq_of(&(srb_to_add_mod_list_r13)[0], bref, srb_to_add_mod_list_r13.size()));
  }
  if (drb_to_add_mod_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_add_mod_list_r13, bref, 1, 2));
  }
  if (drb_to_release_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_release_list_r13, bref, 1, 2, integer_packer<uint8_t>(1, 32)));
  }
  if (mac_main_cfg_r13_present) {
    HANDLE_CODE(mac_main_cfg_r13.unpack(bref));
  }
  if (phys_cfg_ded_r13_present) {
    HANDLE_CODE(phys_cfg_ded_r13.unpack(bref));
  }
  if (rlf_timers_and_consts_r13_present) {
    HANDLE_CODE(rlf_timers_and_consts_r13.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool sched_request_cfg_r15_present;
      HANDLE_CODE(bref.unpack(sched_request_cfg_r15_present, 1));
      sched_request_cfg_r15.set_present(sched_request_cfg_r15_present);
      if (sched_request_cfg_r15.is_present()) {
        HANDLE_CODE(sched_request_cfg_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rr_cfg_ded_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srb_to_add_mod_list_r13_present) {
    j.start_array("srb-ToAddModList-r13");
    for (const auto& e1 : srb_to_add_mod_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_to_add_mod_list_r13_present) {
    j.start_array("drb-ToAddModList-r13");
    for (const auto& e1 : drb_to_add_mod_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_to_release_list_r13_present) {
    j.start_array("drb-ToReleaseList-r13");
    for (const auto& e1 : drb_to_release_list_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (mac_main_cfg_r13_present) {
    j.write_fieldname("mac-MainConfig-r13");
    mac_main_cfg_r13.to_json(j);
  }
  if (phys_cfg_ded_r13_present) {
    j.write_fieldname("physicalConfigDedicated-r13");
    phys_cfg_ded_r13.to_json(j);
  }
  if (rlf_timers_and_consts_r13_present) {
    j.write_fieldname("rlf-TimersAndConstants-r13");
    rlf_timers_and_consts_r13.to_json(j);
  }
  if (ext) {
    if (sched_request_cfg_r15.is_present()) {
      j.write_fieldname("schedulingRequestConfig-r15");
      sched_request_cfg_r15->to_json(j);
    }
  }
  j.end_obj();
}

void rr_cfg_ded_nb_r13_s::mac_main_cfg_r13_c_::set(types::options e)
{
  type_ = e;
}
void rr_cfg_ded_nb_r13_s::mac_main_cfg_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::explicit_value_r13:
      j.write_fieldname("explicitValue-r13");
      c.to_json(j);
      break;
    case types::default_value_r13:
      break;
    default:
      log_invalid_choice_id(type_, "rr_cfg_ded_nb_r13_s::mac_main_cfg_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE rr_cfg_ded_nb_r13_s::mac_main_cfg_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::explicit_value_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::default_value_r13:
      break;
    default:
      log_invalid_choice_id(type_, "rr_cfg_ded_nb_r13_s::mac_main_cfg_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_ded_nb_r13_s::mac_main_cfg_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::explicit_value_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::default_value_r13:
      break;
    default:
      log_invalid_choice_id(type_, "rr_cfg_ded_nb_r13_s::mac_main_cfg_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rr_cfg_ded_nb_r13_s::mac_main_cfg_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue-r13", "defaultValue-r13"};
  return convert_enum_idx(options, 2, value, "rr_cfg_ded_nb_r13_s::mac_main_cfg_r13_c_::types");
}

// AS-Config-NB ::= SEQUENCE
SRSASN_CODE as_cfg_nb_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(source_rr_cfg_r13.pack(bref));
  HANDLE_CODE(source_security_algorithm_cfg_r13.pack(bref));
  HANDLE_CODE(source_ue_id_r13.pack(bref));
  HANDLE_CODE(source_dl_carrier_freq_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_nb_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(source_rr_cfg_r13.unpack(bref));
  HANDLE_CODE(source_security_algorithm_cfg_r13.unpack(bref));
  HANDLE_CODE(source_ue_id_r13.unpack(bref));
  HANDLE_CODE(source_dl_carrier_freq_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void as_cfg_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sourceRadioResourceConfig-r13");
  source_rr_cfg_r13.to_json(j);
  j.write_fieldname("sourceSecurityAlgorithmConfig-r13");
  source_security_algorithm_cfg_r13.to_json(j);
  j.write_str("sourceUE-Identity-r13", source_ue_id_r13.to_string());
  j.write_fieldname("sourceDl-CarrierFreq-r13");
  source_dl_carrier_freq_r13.to_json(j);
  j.end_obj();
}

// ReestablishmentInfo-NB ::= SEQUENCE
SRSASN_CODE reest_info_nb_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(add_reestab_info_list_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, source_pci_r13, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(target_cell_short_mac_i_r13.pack(bref));
  if (add_reestab_info_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, add_reestab_info_list_r13, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE reest_info_nb_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(add_reestab_info_list_r13_present, 1));

  HANDLE_CODE(unpack_integer(source_pci_r13, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(target_cell_short_mac_i_r13.unpack(bref));
  if (add_reestab_info_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(add_reestab_info_list_r13, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void reest_info_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sourcePhysCellId-r13", source_pci_r13);
  j.write_str("targetCellShortMAC-I-r13", target_cell_short_mac_i_r13.to_string());
  if (add_reestab_info_list_r13_present) {
    j.start_array("additionalReestabInfoList-r13");
    for (const auto& e1 : add_reestab_info_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// AS-Context-NB ::= SEQUENCE
SRSASN_CODE as_context_nb_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(reest_info_r13_present, 1));

  if (reest_info_r13_present) {
    HANDLE_CODE(reest_info_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_context_nb_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(reest_info_r13_present, 1));

  if (reest_info_r13_present) {
    HANDLE_CODE(reest_info_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void as_context_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reest_info_r13_present) {
    j.write_fieldname("reestablishmentInfo-r13");
    reest_info_r13.to_json(j);
  }
  j.end_obj();
}

// ChannelRasterOffset-NB-r13 ::= ENUMERATED
std::string ch_raster_offset_nb_r13_opts::to_string() const
{
  static const char* options[] = {"khz-7dot5", "khz-2dot5", "khz2dot5", "khz7dot5"};
  return convert_enum_idx(options, 4, value, "ch_raster_offset_nb_r13_e");
}
float ch_raster_offset_nb_r13_opts::to_number() const
{
  static const float options[] = {-7.5, -2.5, 2.5, 7.5};
  return map_enum_number(options, 4, value, "ch_raster_offset_nb_r13_e");
}
std::string ch_raster_offset_nb_r13_opts::to_number_string() const
{
  static const char* options[] = {"-7.5", "-2.5", "2.5", "7.5"};
  return convert_enum_idx(options, 4, value, "ch_raster_offset_nb_r13_e");
}

// Guardband-NB-r13 ::= SEQUENCE
SRSASN_CODE guardband_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(raster_offset_r13.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE guardband_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(raster_offset_r13.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void guardband_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rasterOffset-r13", raster_offset_r13.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// Inband-DifferentPCI-NB-r13 ::= SEQUENCE
SRSASN_CODE inband_different_pci_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(eutra_num_crs_ports_r13.pack(bref));
  HANDLE_CODE(raster_offset_r13.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE inband_different_pci_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(eutra_num_crs_ports_r13.unpack(bref));
  HANDLE_CODE(raster_offset_r13.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void inband_different_pci_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("eutra-NumCRS-Ports-r13", eutra_num_crs_ports_r13.to_string());
  j.write_str("rasterOffset-r13", raster_offset_r13.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

std::string inband_different_pci_nb_r13_s::eutra_num_crs_ports_r13_opts::to_string() const
{
  static const char* options[] = {"same", "four"};
  return convert_enum_idx(options, 2, value, "inband_different_pci_nb_r13_s::eutra_num_crs_ports_r13_e_");
}
uint8_t inband_different_pci_nb_r13_s::eutra_num_crs_ports_r13_opts::to_number() const
{
  if (value == four) {
    return 4;
  }
  invalid_enum_number(value, "inband_different_pci_nb_r13_s::eutra_num_crs_ports_r13_e_");
  return 0;
}

// Inband-SamePCI-NB-r13 ::= SEQUENCE
SRSASN_CODE inband_same_pci_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, eutra_crs_seq_info_r13, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE inband_same_pci_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(eutra_crs_seq_info_r13, bref, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
void inband_same_pci_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("eutra-CRS-SequenceInfo-r13", eutra_crs_seq_info_r13);
  j.end_obj();
}

// Standalone-NB-r13 ::= SEQUENCE
SRSASN_CODE standalone_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE standalone_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void standalone_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// MasterInformationBlock-NB ::= SEQUENCE
SRSASN_CODE mib_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sys_frame_num_msb_r13.pack(bref));
  HANDLE_CODE(hyper_sfn_lsb_r13.pack(bref));
  HANDLE_CODE(pack_integer(bref, sched_info_sib1_r13, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, sys_info_value_tag_r13, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(bref.pack(ab_enabled_r13, 1));
  HANDLE_CODE(operation_mode_info_r13.pack(bref));
  HANDLE_CODE(bref.pack(add_tx_sib1_r15, 1));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mib_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sys_frame_num_msb_r13.unpack(bref));
  HANDLE_CODE(hyper_sfn_lsb_r13.unpack(bref));
  HANDLE_CODE(unpack_integer(sched_info_sib1_r13, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(sys_info_value_tag_r13, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(bref.unpack(ab_enabled_r13, 1));
  HANDLE_CODE(operation_mode_info_r13.unpack(bref));
  HANDLE_CODE(bref.unpack(add_tx_sib1_r15, 1));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void mib_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("systemFrameNumber-MSB-r13", sys_frame_num_msb_r13.to_string());
  j.write_str("hyperSFN-LSB-r13", hyper_sfn_lsb_r13.to_string());
  j.write_int("schedulingInfoSIB1-r13", sched_info_sib1_r13);
  j.write_int("systemInfoValueTag-r13", sys_info_value_tag_r13);
  j.write_bool("ab-Enabled-r13", ab_enabled_r13);
  j.write_fieldname("operationModeInfo-r13");
  operation_mode_info_r13.to_json(j);
  j.write_bool("additionalTransmissionSIB1-r15", add_tx_sib1_r15);
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

void mib_nb_s::operation_mode_info_r13_c_::destroy_()
{
  switch (type_) {
    case types::inband_same_pci_r13:
      c.destroy<inband_same_pci_nb_r13_s>();
      break;
    case types::inband_different_pci_r13:
      c.destroy<inband_different_pci_nb_r13_s>();
      break;
    case types::guardband_r13:
      c.destroy<guardband_nb_r13_s>();
      break;
    case types::standalone_r13:
      c.destroy<standalone_nb_r13_s>();
      break;
    default:
      break;
  }
}
void mib_nb_s::operation_mode_info_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::inband_same_pci_r13:
      c.init<inband_same_pci_nb_r13_s>();
      break;
    case types::inband_different_pci_r13:
      c.init<inband_different_pci_nb_r13_s>();
      break;
    case types::guardband_r13:
      c.init<guardband_nb_r13_s>();
      break;
    case types::standalone_r13:
      c.init<standalone_nb_r13_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mib_nb_s::operation_mode_info_r13_c_");
  }
}
mib_nb_s::operation_mode_info_r13_c_::operation_mode_info_r13_c_(const mib_nb_s::operation_mode_info_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::inband_same_pci_r13:
      c.init(other.c.get<inband_same_pci_nb_r13_s>());
      break;
    case types::inband_different_pci_r13:
      c.init(other.c.get<inband_different_pci_nb_r13_s>());
      break;
    case types::guardband_r13:
      c.init(other.c.get<guardband_nb_r13_s>());
      break;
    case types::standalone_r13:
      c.init(other.c.get<standalone_nb_r13_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mib_nb_s::operation_mode_info_r13_c_");
  }
}
mib_nb_s::operation_mode_info_r13_c_& mib_nb_s::operation_mode_info_r13_c_::
                                      operator=(const mib_nb_s::operation_mode_info_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::inband_same_pci_r13:
      c.set(other.c.get<inband_same_pci_nb_r13_s>());
      break;
    case types::inband_different_pci_r13:
      c.set(other.c.get<inband_different_pci_nb_r13_s>());
      break;
    case types::guardband_r13:
      c.set(other.c.get<guardband_nb_r13_s>());
      break;
    case types::standalone_r13:
      c.set(other.c.get<standalone_nb_r13_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mib_nb_s::operation_mode_info_r13_c_");
  }

  return *this;
}
void mib_nb_s::operation_mode_info_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::inband_same_pci_r13:
      j.write_fieldname("inband-SamePCI-r13");
      c.get<inband_same_pci_nb_r13_s>().to_json(j);
      break;
    case types::inband_different_pci_r13:
      j.write_fieldname("inband-DifferentPCI-r13");
      c.get<inband_different_pci_nb_r13_s>().to_json(j);
      break;
    case types::guardband_r13:
      j.write_fieldname("guardband-r13");
      c.get<guardband_nb_r13_s>().to_json(j);
      break;
    case types::standalone_r13:
      j.write_fieldname("standalone-r13");
      c.get<standalone_nb_r13_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "mib_nb_s::operation_mode_info_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE mib_nb_s::operation_mode_info_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::inband_same_pci_r13:
      HANDLE_CODE(c.get<inband_same_pci_nb_r13_s>().pack(bref));
      break;
    case types::inband_different_pci_r13:
      HANDLE_CODE(c.get<inband_different_pci_nb_r13_s>().pack(bref));
      break;
    case types::guardband_r13:
      HANDLE_CODE(c.get<guardband_nb_r13_s>().pack(bref));
      break;
    case types::standalone_r13:
      HANDLE_CODE(c.get<standalone_nb_r13_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mib_nb_s::operation_mode_info_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mib_nb_s::operation_mode_info_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::inband_same_pci_r13:
      HANDLE_CODE(c.get<inband_same_pci_nb_r13_s>().unpack(bref));
      break;
    case types::inband_different_pci_r13:
      HANDLE_CODE(c.get<inband_different_pci_nb_r13_s>().unpack(bref));
      break;
    case types::guardband_r13:
      HANDLE_CODE(c.get<guardband_nb_r13_s>().unpack(bref));
      break;
    case types::standalone_r13:
      HANDLE_CODE(c.get<standalone_nb_r13_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mib_nb_s::operation_mode_info_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string mib_nb_s::operation_mode_info_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"inband-SamePCI-r13", "inband-DifferentPCI-r13", "guardband-r13", "standalone-r13"};
  return convert_enum_idx(options, 4, value, "mib_nb_s::operation_mode_info_r13_c_::types");
}
uint8_t mib_nb_s::operation_mode_info_r13_c_::types_opts::to_number() const
{
  if (value == standalone_r13) {
    return 1;
  }
  invalid_enum_number(value, "mib_nb_s::operation_mode_info_r13_c_::types");
  return 0;
}

// BCCH-BCH-Message-NB ::= SEQUENCE
SRSASN_CODE bcch_bch_msg_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_bch_msg_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void bcch_bch_msg_nb_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("BCCH-BCH-Message-NB");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// SIB-GuardbandAnchorTDD-NB-r15 ::= SEQUENCE
SRSASN_CODE sib_guardband_anchor_tdd_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_guardband_anchor_tdd_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void sib_guardband_anchor_tdd_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// SIB-GuardbandGuardbandTDD-NB-r15 ::= SEQUENCE
SRSASN_CODE sib_guardband_guardband_tdd_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sib_guardband_guardband_location_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_guardband_guardband_tdd_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sib_guardband_guardband_location_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void sib_guardband_guardband_tdd_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sib-GuardbandGuardbandLocation-r15", sib_guardband_guardband_location_r15.to_string());
  j.end_obj();
}

std::string sib_guardband_guardband_tdd_nb_r15_s::sib_guardband_guardband_location_r15_opts::to_string() const
{
  static const char* options[] = {"same", "opposite"};
  return convert_enum_idx(
      options, 2, value, "sib_guardband_guardband_tdd_nb_r15_s::sib_guardband_guardband_location_r15_e_");
}

// SIB-GuardbandInbandDiffPCI-TDD-NB-r15 ::= SEQUENCE
SRSASN_CODE sib_guardband_inband_diff_pci_tdd_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sib_eutra_num_crs_ports_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_guardband_inband_diff_pci_tdd_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sib_eutra_num_crs_ports_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void sib_guardband_inband_diff_pci_tdd_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sib-EUTRA-NumCRS-Ports-r15", sib_eutra_num_crs_ports_r15.to_string());
  j.end_obj();
}

std::string sib_guardband_inband_diff_pci_tdd_nb_r15_s::sib_eutra_num_crs_ports_r15_opts::to_string() const
{
  static const char* options[] = {"same", "four"};
  return convert_enum_idx(
      options, 2, value, "sib_guardband_inband_diff_pci_tdd_nb_r15_s::sib_eutra_num_crs_ports_r15_e_");
}
uint8_t sib_guardband_inband_diff_pci_tdd_nb_r15_s::sib_eutra_num_crs_ports_r15_opts::to_number() const
{
  if (value == four) {
    return 4;
  }
  invalid_enum_number(value, "sib_guardband_inband_diff_pci_tdd_nb_r15_s::sib_eutra_num_crs_ports_r15_e_");
  return 0;
}

// SIB-GuardbandInbandSamePCI-TDD-NB-r15 ::= SEQUENCE
SRSASN_CODE sib_guardband_inband_same_pci_tdd_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_guardband_inband_same_pci_tdd_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void sib_guardband_inband_same_pci_tdd_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// GuardbandTDD-NB-r15 ::= SEQUENCE
SRSASN_CODE guardband_tdd_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(raster_offset_r15.pack(bref));
  HANDLE_CODE(sib_guardband_info_r15.pack(bref));
  HANDLE_CODE(eutra_bandwitdh_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE guardband_tdd_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(raster_offset_r15.unpack(bref));
  HANDLE_CODE(sib_guardband_info_r15.unpack(bref));
  HANDLE_CODE(eutra_bandwitdh_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void guardband_tdd_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rasterOffset-r15", raster_offset_r15.to_string());
  j.write_fieldname("sib-GuardbandInfo-r15");
  sib_guardband_info_r15.to_json(j);
  j.write_str("eutra-Bandwitdh-r15", eutra_bandwitdh_r15.to_string());
  j.end_obj();
}

void guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_::destroy_()
{
  switch (type_) {
    case types::sib_guardband_anchor_r15:
      c.destroy<sib_guardband_anchor_tdd_nb_r15_s>();
      break;
    case types::sib_guardband_guardband_r15:
      c.destroy<sib_guardband_guardband_tdd_nb_r15_s>();
      break;
    case types::sib_guardband_inband_same_pci_r15:
      c.destroy<sib_guardband_inband_same_pci_tdd_nb_r15_s>();
      break;
    case types::sib_guardbandinband_diff_pci_r15:
      c.destroy<sib_guardband_inband_diff_pci_tdd_nb_r15_s>();
      break;
    default:
      break;
  }
}
void guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sib_guardband_anchor_r15:
      c.init<sib_guardband_anchor_tdd_nb_r15_s>();
      break;
    case types::sib_guardband_guardband_r15:
      c.init<sib_guardband_guardband_tdd_nb_r15_s>();
      break;
    case types::sib_guardband_inband_same_pci_r15:
      c.init<sib_guardband_inband_same_pci_tdd_nb_r15_s>();
      break;
    case types::sib_guardbandinband_diff_pci_r15:
      c.init<sib_guardband_inband_diff_pci_tdd_nb_r15_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_");
  }
}
guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_::sib_guardband_info_r15_c_(
    const guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sib_guardband_anchor_r15:
      c.init(other.c.get<sib_guardband_anchor_tdd_nb_r15_s>());
      break;
    case types::sib_guardband_guardband_r15:
      c.init(other.c.get<sib_guardband_guardband_tdd_nb_r15_s>());
      break;
    case types::sib_guardband_inband_same_pci_r15:
      c.init(other.c.get<sib_guardband_inband_same_pci_tdd_nb_r15_s>());
      break;
    case types::sib_guardbandinband_diff_pci_r15:
      c.init(other.c.get<sib_guardband_inband_diff_pci_tdd_nb_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_");
  }
}
guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_& guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_::
                                                   operator=(const guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sib_guardband_anchor_r15:
      c.set(other.c.get<sib_guardband_anchor_tdd_nb_r15_s>());
      break;
    case types::sib_guardband_guardband_r15:
      c.set(other.c.get<sib_guardband_guardband_tdd_nb_r15_s>());
      break;
    case types::sib_guardband_inband_same_pci_r15:
      c.set(other.c.get<sib_guardband_inband_same_pci_tdd_nb_r15_s>());
      break;
    case types::sib_guardbandinband_diff_pci_r15:
      c.set(other.c.get<sib_guardband_inband_diff_pci_tdd_nb_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_");
  }

  return *this;
}
void guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sib_guardband_anchor_r15:
      j.write_fieldname("sib-GuardbandAnchor-r15");
      c.get<sib_guardband_anchor_tdd_nb_r15_s>().to_json(j);
      break;
    case types::sib_guardband_guardband_r15:
      j.write_fieldname("sib-GuardbandGuardband-r15");
      c.get<sib_guardband_guardband_tdd_nb_r15_s>().to_json(j);
      break;
    case types::sib_guardband_inband_same_pci_r15:
      j.write_fieldname("sib-GuardbandInbandSamePCI-r15");
      c.get<sib_guardband_inband_same_pci_tdd_nb_r15_s>().to_json(j);
      break;
    case types::sib_guardbandinband_diff_pci_r15:
      j.write_fieldname("sib-GuardbandinbandDiffPCI-r15");
      c.get<sib_guardband_inband_diff_pci_tdd_nb_r15_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sib_guardband_anchor_r15:
      HANDLE_CODE(c.get<sib_guardband_anchor_tdd_nb_r15_s>().pack(bref));
      break;
    case types::sib_guardband_guardband_r15:
      HANDLE_CODE(c.get<sib_guardband_guardband_tdd_nb_r15_s>().pack(bref));
      break;
    case types::sib_guardband_inband_same_pci_r15:
      HANDLE_CODE(c.get<sib_guardband_inband_same_pci_tdd_nb_r15_s>().pack(bref));
      break;
    case types::sib_guardbandinband_diff_pci_r15:
      HANDLE_CODE(c.get<sib_guardband_inband_diff_pci_tdd_nb_r15_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sib_guardband_anchor_r15:
      HANDLE_CODE(c.get<sib_guardband_anchor_tdd_nb_r15_s>().unpack(bref));
      break;
    case types::sib_guardband_guardband_r15:
      HANDLE_CODE(c.get<sib_guardband_guardband_tdd_nb_r15_s>().unpack(bref));
      break;
    case types::sib_guardband_inband_same_pci_r15:
      HANDLE_CODE(c.get<sib_guardband_inband_same_pci_tdd_nb_r15_s>().unpack(bref));
      break;
    case types::sib_guardbandinband_diff_pci_r15:
      HANDLE_CODE(c.get<sib_guardband_inband_diff_pci_tdd_nb_r15_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"sib-GuardbandAnchor-r15",
                                  "sib-GuardbandGuardband-r15",
                                  "sib-GuardbandInbandSamePCI-r15",
                                  "sib-GuardbandinbandDiffPCI-r15"};
  return convert_enum_idx(options, 4, value, "guardband_tdd_nb_r15_s::sib_guardband_info_r15_c_::types");
}

std::string guardband_tdd_nb_r15_s::eutra_bandwitdh_r15_opts::to_string() const
{
  static const char* options[] = {"bw5or10", "bw15or20"};
  return convert_enum_idx(options, 2, value, "guardband_tdd_nb_r15_s::eutra_bandwitdh_r15_e_");
}
uint8_t guardband_tdd_nb_r15_s::eutra_bandwitdh_r15_opts::to_number() const
{
  static const uint8_t options[] = {5, 15};
  return map_enum_number(options, 2, value, "guardband_tdd_nb_r15_s::eutra_bandwitdh_r15_e_");
}

// Inband-DifferentPCI-TDD-NB-r15 ::= SEQUENCE
SRSASN_CODE inband_different_pci_tdd_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(eutra_num_crs_ports_r15.pack(bref));
  HANDLE_CODE(raster_offset_r15.pack(bref));
  HANDLE_CODE(sib_inband_location_r15.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE inband_different_pci_tdd_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(eutra_num_crs_ports_r15.unpack(bref));
  HANDLE_CODE(raster_offset_r15.unpack(bref));
  HANDLE_CODE(sib_inband_location_r15.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void inband_different_pci_tdd_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("eutra-NumCRS-Ports-r15", eutra_num_crs_ports_r15.to_string());
  j.write_str("rasterOffset-r15", raster_offset_r15.to_string());
  j.write_str("sib-InbandLocation-r15", sib_inband_location_r15.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

std::string inband_different_pci_tdd_nb_r15_s::eutra_num_crs_ports_r15_opts::to_string() const
{
  static const char* options[] = {"same", "four"};
  return convert_enum_idx(options, 2, value, "inband_different_pci_tdd_nb_r15_s::eutra_num_crs_ports_r15_e_");
}
uint8_t inband_different_pci_tdd_nb_r15_s::eutra_num_crs_ports_r15_opts::to_number() const
{
  if (value == four) {
    return 4;
  }
  invalid_enum_number(value, "inband_different_pci_tdd_nb_r15_s::eutra_num_crs_ports_r15_e_");
  return 0;
}

std::string inband_different_pci_tdd_nb_r15_s::sib_inband_location_r15_opts::to_string() const
{
  static const char* options[] = {"lower", "higher"};
  return convert_enum_idx(options, 2, value, "inband_different_pci_tdd_nb_r15_s::sib_inband_location_r15_e_");
}

// Inband-SamePCI-TDD-NB-r15 ::= SEQUENCE
SRSASN_CODE inband_same_pci_tdd_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, eutra_crs_seq_info_r15, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(sib_inband_location_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE inband_same_pci_tdd_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(eutra_crs_seq_info_r15, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(sib_inband_location_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void inband_same_pci_tdd_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("eutra-CRS-SequenceInfo-r15", eutra_crs_seq_info_r15);
  j.write_str("sib-InbandLocation-r15", sib_inband_location_r15.to_string());
  j.end_obj();
}

std::string inband_same_pci_tdd_nb_r15_s::sib_inband_location_r15_opts::to_string() const
{
  static const char* options[] = {"lower", "higher"};
  return convert_enum_idx(options, 2, value, "inband_same_pci_tdd_nb_r15_s::sib_inband_location_r15_e_");
}

// StandaloneTDD-NB-r15 ::= SEQUENCE
SRSASN_CODE standalone_tdd_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sib_standalone_location_r15.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE standalone_tdd_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sib_standalone_location_r15.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void standalone_tdd_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sib-StandaloneLocation-r15", sib_standalone_location_r15.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

std::string standalone_tdd_nb_r15_s::sib_standalone_location_r15_opts::to_string() const
{
  static const char* options[] = {"lower", "higher"};
  return convert_enum_idx(options, 2, value, "standalone_tdd_nb_r15_s::sib_standalone_location_r15_e_");
}

// MasterInformationBlock-TDD-NB-r15 ::= SEQUENCE
SRSASN_CODE mib_tdd_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sys_frame_num_msb_r15.pack(bref));
  HANDLE_CODE(hyper_sfn_lsb_r15.pack(bref));
  HANDLE_CODE(pack_integer(bref, sched_info_sib1_r15, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, sys_info_value_tag_r15, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(bref.pack(ab_enabled_r15, 1));
  HANDLE_CODE(operation_mode_info_r15.pack(bref));
  HANDLE_CODE(sib1_carrier_info_r15.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mib_tdd_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sys_frame_num_msb_r15.unpack(bref));
  HANDLE_CODE(hyper_sfn_lsb_r15.unpack(bref));
  HANDLE_CODE(unpack_integer(sched_info_sib1_r15, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(sys_info_value_tag_r15, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(bref.unpack(ab_enabled_r15, 1));
  HANDLE_CODE(operation_mode_info_r15.unpack(bref));
  HANDLE_CODE(sib1_carrier_info_r15.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void mib_tdd_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("systemFrameNumber-MSB-r15", sys_frame_num_msb_r15.to_string());
  j.write_str("hyperSFN-LSB-r15", hyper_sfn_lsb_r15.to_string());
  j.write_int("schedulingInfoSIB1-r15", sched_info_sib1_r15);
  j.write_int("systemInfoValueTag-r15", sys_info_value_tag_r15);
  j.write_bool("ab-Enabled-r15", ab_enabled_r15);
  j.write_fieldname("operationModeInfo-r15");
  operation_mode_info_r15.to_json(j);
  j.write_str("sib1-CarrierInfo-r15", sib1_carrier_info_r15.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

void mib_tdd_nb_r15_s::operation_mode_info_r15_c_::destroy_()
{
  switch (type_) {
    case types::inband_same_pci_r15:
      c.destroy<inband_same_pci_tdd_nb_r15_s>();
      break;
    case types::inband_different_pci_r15:
      c.destroy<inband_different_pci_tdd_nb_r15_s>();
      break;
    case types::guardband_r15:
      c.destroy<guardband_tdd_nb_r15_s>();
      break;
    case types::standalone_r15:
      c.destroy<standalone_tdd_nb_r15_s>();
      break;
    default:
      break;
  }
}
void mib_tdd_nb_r15_s::operation_mode_info_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::inband_same_pci_r15:
      c.init<inband_same_pci_tdd_nb_r15_s>();
      break;
    case types::inband_different_pci_r15:
      c.init<inband_different_pci_tdd_nb_r15_s>();
      break;
    case types::guardband_r15:
      c.init<guardband_tdd_nb_r15_s>();
      break;
    case types::standalone_r15:
      c.init<standalone_tdd_nb_r15_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mib_tdd_nb_r15_s::operation_mode_info_r15_c_");
  }
}
mib_tdd_nb_r15_s::operation_mode_info_r15_c_::operation_mode_info_r15_c_(
    const mib_tdd_nb_r15_s::operation_mode_info_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::inband_same_pci_r15:
      c.init(other.c.get<inband_same_pci_tdd_nb_r15_s>());
      break;
    case types::inband_different_pci_r15:
      c.init(other.c.get<inband_different_pci_tdd_nb_r15_s>());
      break;
    case types::guardband_r15:
      c.init(other.c.get<guardband_tdd_nb_r15_s>());
      break;
    case types::standalone_r15:
      c.init(other.c.get<standalone_tdd_nb_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mib_tdd_nb_r15_s::operation_mode_info_r15_c_");
  }
}
mib_tdd_nb_r15_s::operation_mode_info_r15_c_& mib_tdd_nb_r15_s::operation_mode_info_r15_c_::
                                              operator=(const mib_tdd_nb_r15_s::operation_mode_info_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::inband_same_pci_r15:
      c.set(other.c.get<inband_same_pci_tdd_nb_r15_s>());
      break;
    case types::inband_different_pci_r15:
      c.set(other.c.get<inband_different_pci_tdd_nb_r15_s>());
      break;
    case types::guardband_r15:
      c.set(other.c.get<guardband_tdd_nb_r15_s>());
      break;
    case types::standalone_r15:
      c.set(other.c.get<standalone_tdd_nb_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mib_tdd_nb_r15_s::operation_mode_info_r15_c_");
  }

  return *this;
}
void mib_tdd_nb_r15_s::operation_mode_info_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::inband_same_pci_r15:
      j.write_fieldname("inband-SamePCI-r15");
      c.get<inband_same_pci_tdd_nb_r15_s>().to_json(j);
      break;
    case types::inband_different_pci_r15:
      j.write_fieldname("inband-DifferentPCI-r15");
      c.get<inband_different_pci_tdd_nb_r15_s>().to_json(j);
      break;
    case types::guardband_r15:
      j.write_fieldname("guardband-r15");
      c.get<guardband_tdd_nb_r15_s>().to_json(j);
      break;
    case types::standalone_r15:
      j.write_fieldname("standalone-r15");
      c.get<standalone_tdd_nb_r15_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "mib_tdd_nb_r15_s::operation_mode_info_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE mib_tdd_nb_r15_s::operation_mode_info_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::inband_same_pci_r15:
      HANDLE_CODE(c.get<inband_same_pci_tdd_nb_r15_s>().pack(bref));
      break;
    case types::inband_different_pci_r15:
      HANDLE_CODE(c.get<inband_different_pci_tdd_nb_r15_s>().pack(bref));
      break;
    case types::guardband_r15:
      HANDLE_CODE(c.get<guardband_tdd_nb_r15_s>().pack(bref));
      break;
    case types::standalone_r15:
      HANDLE_CODE(c.get<standalone_tdd_nb_r15_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mib_tdd_nb_r15_s::operation_mode_info_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mib_tdd_nb_r15_s::operation_mode_info_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::inband_same_pci_r15:
      HANDLE_CODE(c.get<inband_same_pci_tdd_nb_r15_s>().unpack(bref));
      break;
    case types::inband_different_pci_r15:
      HANDLE_CODE(c.get<inband_different_pci_tdd_nb_r15_s>().unpack(bref));
      break;
    case types::guardband_r15:
      HANDLE_CODE(c.get<guardband_tdd_nb_r15_s>().unpack(bref));
      break;
    case types::standalone_r15:
      HANDLE_CODE(c.get<standalone_tdd_nb_r15_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mib_tdd_nb_r15_s::operation_mode_info_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string mib_tdd_nb_r15_s::operation_mode_info_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"inband-SamePCI-r15", "inband-DifferentPCI-r15", "guardband-r15", "standalone-r15"};
  return convert_enum_idx(options, 4, value, "mib_tdd_nb_r15_s::operation_mode_info_r15_c_::types");
}
uint8_t mib_tdd_nb_r15_s::operation_mode_info_r15_c_::types_opts::to_number() const
{
  if (value == standalone_r15) {
    return 1;
  }
  invalid_enum_number(value, "mib_tdd_nb_r15_s::operation_mode_info_r15_c_::types");
  return 0;
}

std::string mib_tdd_nb_r15_s::sib1_carrier_info_r15_opts::to_string() const
{
  static const char* options[] = {"anchor", "non-anchor"};
  return convert_enum_idx(options, 2, value, "mib_tdd_nb_r15_s::sib1_carrier_info_r15_e_");
}

// BCCH-BCH-Message-TDD-NB ::= SEQUENCE
SRSASN_CODE bcch_bch_msg_tdd_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_bch_msg_tdd_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void bcch_bch_msg_tdd_nb_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("BCCH-BCH-Message-TDD-NB");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// NS-PmaxValue-NB-r13 ::= SEQUENCE
SRSASN_CODE ns_pmax_value_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(add_pmax_r13_present, 1));

  if (add_pmax_r13_present) {
    HANDLE_CODE(pack_integer(bref, add_pmax_r13, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(pack_integer(bref, add_spec_emission_r13, (uint8_t)1u, (uint8_t)32u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ns_pmax_value_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(add_pmax_r13_present, 1));

  if (add_pmax_r13_present) {
    HANDLE_CODE(unpack_integer(add_pmax_r13, bref, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(unpack_integer(add_spec_emission_r13, bref, (uint8_t)1u, (uint8_t)32u));

  return SRSASN_SUCCESS;
}
void ns_pmax_value_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (add_pmax_r13_present) {
    j.write_int("additionalPmax-r13", add_pmax_r13);
  }
  j.write_int("additionalSpectrumEmission-r13", add_spec_emission_r13);
  j.end_obj();
}

// SIB-Type-NB-v1530 ::= ENUMERATED
std::string sib_type_nb_v1530_opts::to_string() const
{
  static const char* options[] = {
      "sibType23-NB-r15", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "sib_type_nb_v1530_e");
}
uint8_t sib_type_nb_v1530_opts::to_number() const
{
  static const uint8_t options[] = {23};
  return map_enum_number(options, 1, value, "sib_type_nb_v1530_e");
}

// NSSS-RRM-Config-NB-r15 ::= SEQUENCE
SRSASN_CODE nsss_rrm_cfg_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nsss_num_occ_diff_precoders_r15_present, 1));

  HANDLE_CODE(nsss_rrm_pwr_offset_r15.pack(bref));
  if (nsss_num_occ_diff_precoders_r15_present) {
    HANDLE_CODE(nsss_num_occ_diff_precoders_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nsss_rrm_cfg_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nsss_num_occ_diff_precoders_r15_present, 1));

  HANDLE_CODE(nsss_rrm_pwr_offset_r15.unpack(bref));
  if (nsss_num_occ_diff_precoders_r15_present) {
    HANDLE_CODE(nsss_num_occ_diff_precoders_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void nsss_rrm_cfg_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("nsss-RRM-PowerOffset-r15", nsss_rrm_pwr_offset_r15.to_string());
  if (nsss_num_occ_diff_precoders_r15_present) {
    j.write_str("nsss-NumOccDiffPrecoders-r15", nsss_num_occ_diff_precoders_r15.to_string());
  }
  j.end_obj();
}

std::string nsss_rrm_cfg_nb_r15_s::nsss_rrm_pwr_offset_r15_opts::to_string() const
{
  static const char* options[] = {"dB-3", "db0", "dB3"};
  return convert_enum_idx(options, 3, value, "nsss_rrm_cfg_nb_r15_s::nsss_rrm_pwr_offset_r15_e_");
}
int8_t nsss_rrm_cfg_nb_r15_s::nsss_rrm_pwr_offset_r15_opts::to_number() const
{
  static const int8_t options[] = {-3, 0, 3};
  return map_enum_number(options, 3, value, "nsss_rrm_cfg_nb_r15_s::nsss_rrm_pwr_offset_r15_e_");
}

std::string nsss_rrm_cfg_nb_r15_s::nsss_num_occ_diff_precoders_r15_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "n8"};
  return convert_enum_idx(options, 4, value, "nsss_rrm_cfg_nb_r15_s::nsss_num_occ_diff_precoders_r15_e_");
}
uint8_t nsss_rrm_cfg_nb_r15_s::nsss_num_occ_diff_precoders_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(options, 4, value, "nsss_rrm_cfg_nb_r15_s::nsss_num_occ_diff_precoders_r15_e_");
}

// EDT-TBS-NB-r15 ::= SEQUENCE
SRSASN_CODE edt_tbs_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(edt_small_tbs_enabled_r15, 1));
  HANDLE_CODE(edt_tbs_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE edt_tbs_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(edt_small_tbs_enabled_r15, 1));
  HANDLE_CODE(edt_tbs_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void edt_tbs_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("edt-SmallTBS-Enabled-r15", edt_small_tbs_enabled_r15);
  j.write_str("edt-TBS-r15", edt_tbs_r15.to_string());
  j.end_obj();
}

std::string edt_tbs_nb_r15_s::edt_tbs_r15_opts::to_string() const
{
  static const char* options[] = {"b328", "b408", "b504", "b584", "b680", "b808", "b936", "b1000"};
  return convert_enum_idx(options, 8, value, "edt_tbs_nb_r15_s::edt_tbs_r15_e_");
}
uint16_t edt_tbs_nb_r15_s::edt_tbs_r15_opts::to_number() const
{
  static const uint16_t options[] = {328, 408, 504, 584, 680, 808, 936, 1000};
  return map_enum_number(options, 8, value, "edt_tbs_nb_r15_s::edt_tbs_r15_e_");
}

// InterFreqNeighCellInfo-NB-v1530 ::= SEQUENCE
SRSASN_CODE inter_freq_neigh_cell_info_nb_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nsss_rrm_cfg_r15_present, 1));

  if (nsss_rrm_cfg_r15_present) {
    HANDLE_CODE(nsss_rrm_cfg_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_neigh_cell_info_nb_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nsss_rrm_cfg_r15_present, 1));

  if (nsss_rrm_cfg_r15_present) {
    HANDLE_CODE(nsss_rrm_cfg_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_neigh_cell_info_nb_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nsss_rrm_cfg_r15_present) {
    j.write_fieldname("nsss-RRM-Config-r15");
    nsss_rrm_cfg_r15.to_json(j);
  }
  j.end_obj();
}

// MultiBandInfo-NB-r13 ::= SEQUENCE
SRSASN_CODE multi_band_info_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_band_ind_r13_present, 1));
  HANDLE_CODE(bref.pack(freq_band_info_r13_present, 1));

  if (freq_band_ind_r13_present) {
    HANDLE_CODE(pack_integer(bref, freq_band_ind_r13, (uint16_t)1u, (uint16_t)256u));
  }
  if (freq_band_info_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_info_r13, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE multi_band_info_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(freq_band_ind_r13_present, 1));
  HANDLE_CODE(bref.unpack(freq_band_info_r13_present, 1));

  if (freq_band_ind_r13_present) {
    HANDLE_CODE(unpack_integer(freq_band_ind_r13, bref, (uint16_t)1u, (uint16_t)256u));
  }
  if (freq_band_info_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_info_r13, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void multi_band_info_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_ind_r13_present) {
    j.write_int("freqBandIndicator-r13", freq_band_ind_r13);
  }
  if (freq_band_info_r13_present) {
    j.start_array("freqBandInfo-r13");
    for (const auto& e1 : freq_band_info_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// NPRACH-Parameters-NB-r13 ::= SEQUENCE
SRSASN_CODE nprach_params_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(nprach_periodicity_r13.pack(bref));
  HANDLE_CODE(nprach_start_time_r13.pack(bref));
  HANDLE_CODE(nprach_subcarrier_offset_r13.pack(bref));
  HANDLE_CODE(nprach_num_subcarriers_r13.pack(bref));
  HANDLE_CODE(nprach_subcarrier_msg3_range_start_r13.pack(bref));
  HANDLE_CODE(max_num_preamb_attempt_ce_r13.pack(bref));
  HANDLE_CODE(num_repeats_per_preamb_attempt_r13.pack(bref));
  HANDLE_CODE(npdcch_num_repeats_ra_r13.pack(bref));
  HANDLE_CODE(npdcch_start_sf_css_ra_r13.pack(bref));
  HANDLE_CODE(npdcch_offset_ra_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nprach_params_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(nprach_periodicity_r13.unpack(bref));
  HANDLE_CODE(nprach_start_time_r13.unpack(bref));
  HANDLE_CODE(nprach_subcarrier_offset_r13.unpack(bref));
  HANDLE_CODE(nprach_num_subcarriers_r13.unpack(bref));
  HANDLE_CODE(nprach_subcarrier_msg3_range_start_r13.unpack(bref));
  HANDLE_CODE(max_num_preamb_attempt_ce_r13.unpack(bref));
  HANDLE_CODE(num_repeats_per_preamb_attempt_r13.unpack(bref));
  HANDLE_CODE(npdcch_num_repeats_ra_r13.unpack(bref));
  HANDLE_CODE(npdcch_start_sf_css_ra_r13.unpack(bref));
  HANDLE_CODE(npdcch_offset_ra_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void nprach_params_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("nprach-Periodicity-r13", nprach_periodicity_r13.to_string());
  j.write_str("nprach-StartTime-r13", nprach_start_time_r13.to_string());
  j.write_str("nprach-SubcarrierOffset-r13", nprach_subcarrier_offset_r13.to_string());
  j.write_str("nprach-NumSubcarriers-r13", nprach_num_subcarriers_r13.to_string());
  j.write_str("nprach-SubcarrierMSG3-RangeStart-r13", nprach_subcarrier_msg3_range_start_r13.to_string());
  j.write_str("maxNumPreambleAttemptCE-r13", max_num_preamb_attempt_ce_r13.to_string());
  j.write_str("numRepetitionsPerPreambleAttempt-r13", num_repeats_per_preamb_attempt_r13.to_string());
  j.write_str("npdcch-NumRepetitions-RA-r13", npdcch_num_repeats_ra_r13.to_string());
  j.write_str("npdcch-StartSF-CSS-RA-r13", npdcch_start_sf_css_ra_r13.to_string());
  j.write_str("npdcch-Offset-RA-r13", npdcch_offset_ra_r13.to_string());
  j.end_obj();
}

std::string nprach_params_nb_r13_s::nprach_periodicity_r13_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms80", "ms160", "ms240", "ms320", "ms640", "ms1280", "ms2560"};
  return convert_enum_idx(options, 8, value, "nprach_params_nb_r13_s::nprach_periodicity_r13_e_");
}
uint16_t nprach_params_nb_r13_s::nprach_periodicity_r13_opts::to_number() const
{
  static const uint16_t options[] = {40, 80, 160, 240, 320, 640, 1280, 2560};
  return map_enum_number(options, 8, value, "nprach_params_nb_r13_s::nprach_periodicity_r13_e_");
}

std::string nprach_params_nb_r13_s::nprach_start_time_r13_opts::to_string() const
{
  static const char* options[] = {"ms8", "ms16", "ms32", "ms64", "ms128", "ms256", "ms512", "ms1024"};
  return convert_enum_idx(options, 8, value, "nprach_params_nb_r13_s::nprach_start_time_r13_e_");
}
uint16_t nprach_params_nb_r13_s::nprach_start_time_r13_opts::to_number() const
{
  static const uint16_t options[] = {8, 16, 32, 64, 128, 256, 512, 1024};
  return map_enum_number(options, 8, value, "nprach_params_nb_r13_s::nprach_start_time_r13_e_");
}

std::string nprach_params_nb_r13_s::nprach_subcarrier_offset_r13_opts::to_string() const
{
  static const char* options[] = {"n0", "n12", "n24", "n36", "n2", "n18", "n34", "spare1"};
  return convert_enum_idx(options, 8, value, "nprach_params_nb_r13_s::nprach_subcarrier_offset_r13_e_");
}
uint8_t nprach_params_nb_r13_s::nprach_subcarrier_offset_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 12, 24, 36, 2, 18, 34};
  return map_enum_number(options, 7, value, "nprach_params_nb_r13_s::nprach_subcarrier_offset_r13_e_");
}

std::string nprach_params_nb_r13_s::nprach_num_subcarriers_r13_opts::to_string() const
{
  static const char* options[] = {"n12", "n24", "n36", "n48"};
  return convert_enum_idx(options, 4, value, "nprach_params_nb_r13_s::nprach_num_subcarriers_r13_e_");
}
uint8_t nprach_params_nb_r13_s::nprach_num_subcarriers_r13_opts::to_number() const
{
  static const uint8_t options[] = {12, 24, 36, 48};
  return map_enum_number(options, 4, value, "nprach_params_nb_r13_s::nprach_num_subcarriers_r13_e_");
}

std::string nprach_params_nb_r13_s::nprach_subcarrier_msg3_range_start_r13_opts::to_string() const
{
  static const char* options[] = {"zero", "oneThird", "twoThird", "one"};
  return convert_enum_idx(options, 4, value, "nprach_params_nb_r13_s::nprach_subcarrier_msg3_range_start_r13_e_");
}
float nprach_params_nb_r13_s::nprach_subcarrier_msg3_range_start_r13_opts::to_number() const
{
  static const float options[] = {0.0, 0.3333333333333333, 0.6666666666666666, 1.0};
  return map_enum_number(options, 4, value, "nprach_params_nb_r13_s::nprach_subcarrier_msg3_range_start_r13_e_");
}
std::string nprach_params_nb_r13_s::nprach_subcarrier_msg3_range_start_r13_opts::to_number_string() const
{
  static const char* options[] = {"0", "1/3", "2/3", "1"};
  return convert_enum_idx(options, 4, value, "nprach_params_nb_r13_s::nprach_subcarrier_msg3_range_start_r13_e_");
}

std::string nprach_params_nb_r13_s::max_num_preamb_attempt_ce_r13_opts::to_string() const
{
  static const char* options[] = {"n3", "n4", "n5", "n6", "n7", "n8", "n10", "spare1"};
  return convert_enum_idx(options, 8, value, "nprach_params_nb_r13_s::max_num_preamb_attempt_ce_r13_e_");
}
uint8_t nprach_params_nb_r13_s::max_num_preamb_attempt_ce_r13_opts::to_number() const
{
  static const uint8_t options[] = {3, 4, 5, 6, 7, 8, 10};
  return map_enum_number(options, 7, value, "nprach_params_nb_r13_s::max_num_preamb_attempt_ce_r13_e_");
}

std::string nprach_params_nb_r13_s::num_repeats_per_preamb_attempt_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "n8", "n16", "n32", "n64", "n128"};
  return convert_enum_idx(options, 8, value, "nprach_params_nb_r13_s::num_repeats_per_preamb_attempt_r13_e_");
}
uint8_t nprach_params_nb_r13_s::num_repeats_per_preamb_attempt_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8, 16, 32, 64, 128};
  return map_enum_number(options, 8, value, "nprach_params_nb_r13_s::num_repeats_per_preamb_attempt_r13_e_");
}

std::string nprach_params_nb_r13_s::npdcch_num_repeats_ra_r13_opts::to_string() const
{
  static const char* options[] = {"r1",
                                  "r2",
                                  "r4",
                                  "r8",
                                  "r16",
                                  "r32",
                                  "r64",
                                  "r128",
                                  "r256",
                                  "r512",
                                  "r1024",
                                  "r2048",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "nprach_params_nb_r13_s::npdcch_num_repeats_ra_r13_e_");
}
uint16_t nprach_params_nb_r13_s::npdcch_num_repeats_ra_r13_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
  return map_enum_number(options, 12, value, "nprach_params_nb_r13_s::npdcch_num_repeats_ra_r13_e_");
}

std::string nprach_params_nb_r13_s::npdcch_start_sf_css_ra_r13_opts::to_string() const
{
  static const char* options[] = {"v1dot5", "v2", "v4", "v8", "v16", "v32", "v48", "v64"};
  return convert_enum_idx(options, 8, value, "nprach_params_nb_r13_s::npdcch_start_sf_css_ra_r13_e_");
}
float nprach_params_nb_r13_s::npdcch_start_sf_css_ra_r13_opts::to_number() const
{
  static const float options[] = {1.5, 2.0, 4.0, 8.0, 16.0, 32.0, 48.0, 64.0};
  return map_enum_number(options, 8, value, "nprach_params_nb_r13_s::npdcch_start_sf_css_ra_r13_e_");
}
std::string nprach_params_nb_r13_s::npdcch_start_sf_css_ra_r13_opts::to_number_string() const
{
  static const char* options[] = {"1.5", "2", "4", "8", "16", "32", "48", "64"};
  return convert_enum_idx(options, 8, value, "nprach_params_nb_r13_s::npdcch_start_sf_css_ra_r13_e_");
}

std::string nprach_params_nb_r13_s::npdcch_offset_ra_r13_opts::to_string() const
{
  static const char* options[] = {"zero", "oneEighth", "oneFourth", "threeEighth"};
  return convert_enum_idx(options, 4, value, "nprach_params_nb_r13_s::npdcch_offset_ra_r13_e_");
}
float nprach_params_nb_r13_s::npdcch_offset_ra_r13_opts::to_number() const
{
  static const float options[] = {0.0, 0.125, 0.25, 0.375};
  return map_enum_number(options, 4, value, "nprach_params_nb_r13_s::npdcch_offset_ra_r13_e_");
}
std::string nprach_params_nb_r13_s::npdcch_offset_ra_r13_opts::to_number_string() const
{
  static const char* options[] = {"0", "1/8", "1/4", "3/8"};
  return convert_enum_idx(options, 4, value, "nprach_params_nb_r13_s::npdcch_offset_ra_r13_e_");
}

// NPRACH-Parameters-NB-r14 ::= SEQUENCE
SRSASN_CODE nprach_params_nb_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nprach_params_r14_present, 1));

  if (nprach_params_r14_present) {
    bref.pack(nprach_params_r14.ext, 1);
    HANDLE_CODE(bref.pack(nprach_params_r14.nprach_periodicity_r14_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r14.nprach_start_time_r14_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r14.nprach_subcarrier_offset_r14_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r14.nprach_num_subcarriers_r14_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r14.nprach_subcarrier_msg3_range_start_r14_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r14.npdcch_num_repeats_ra_r14_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r14.npdcch_start_sf_css_ra_r14_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r14.npdcch_offset_ra_r14_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r14.nprach_num_cbra_start_subcarriers_r14_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r14.npdcch_carrier_idx_r14_present, 1));
    if (nprach_params_r14.nprach_periodicity_r14_present) {
      HANDLE_CODE(nprach_params_r14.nprach_periodicity_r14.pack(bref));
    }
    if (nprach_params_r14.nprach_start_time_r14_present) {
      HANDLE_CODE(nprach_params_r14.nprach_start_time_r14.pack(bref));
    }
    if (nprach_params_r14.nprach_subcarrier_offset_r14_present) {
      HANDLE_CODE(nprach_params_r14.nprach_subcarrier_offset_r14.pack(bref));
    }
    if (nprach_params_r14.nprach_num_subcarriers_r14_present) {
      HANDLE_CODE(nprach_params_r14.nprach_num_subcarriers_r14.pack(bref));
    }
    if (nprach_params_r14.nprach_subcarrier_msg3_range_start_r14_present) {
      HANDLE_CODE(nprach_params_r14.nprach_subcarrier_msg3_range_start_r14.pack(bref));
    }
    if (nprach_params_r14.npdcch_num_repeats_ra_r14_present) {
      HANDLE_CODE(nprach_params_r14.npdcch_num_repeats_ra_r14.pack(bref));
    }
    if (nprach_params_r14.npdcch_start_sf_css_ra_r14_present) {
      HANDLE_CODE(nprach_params_r14.npdcch_start_sf_css_ra_r14.pack(bref));
    }
    if (nprach_params_r14.npdcch_offset_ra_r14_present) {
      HANDLE_CODE(nprach_params_r14.npdcch_offset_ra_r14.pack(bref));
    }
    if (nprach_params_r14.nprach_num_cbra_start_subcarriers_r14_present) {
      HANDLE_CODE(nprach_params_r14.nprach_num_cbra_start_subcarriers_r14.pack(bref));
    }
    if (nprach_params_r14.npdcch_carrier_idx_r14_present) {
      HANDLE_CODE(pack_integer(bref, nprach_params_r14.npdcch_carrier_idx_r14, (uint8_t)1u, (uint8_t)15u));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nprach_params_nb_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nprach_params_r14_present, 1));

  if (nprach_params_r14_present) {
    bref.unpack(nprach_params_r14.ext, 1);
    HANDLE_CODE(bref.unpack(nprach_params_r14.nprach_periodicity_r14_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r14.nprach_start_time_r14_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r14.nprach_subcarrier_offset_r14_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r14.nprach_num_subcarriers_r14_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r14.nprach_subcarrier_msg3_range_start_r14_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r14.npdcch_num_repeats_ra_r14_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r14.npdcch_start_sf_css_ra_r14_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r14.npdcch_offset_ra_r14_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r14.nprach_num_cbra_start_subcarriers_r14_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r14.npdcch_carrier_idx_r14_present, 1));
    if (nprach_params_r14.nprach_periodicity_r14_present) {
      HANDLE_CODE(nprach_params_r14.nprach_periodicity_r14.unpack(bref));
    }
    if (nprach_params_r14.nprach_start_time_r14_present) {
      HANDLE_CODE(nprach_params_r14.nprach_start_time_r14.unpack(bref));
    }
    if (nprach_params_r14.nprach_subcarrier_offset_r14_present) {
      HANDLE_CODE(nprach_params_r14.nprach_subcarrier_offset_r14.unpack(bref));
    }
    if (nprach_params_r14.nprach_num_subcarriers_r14_present) {
      HANDLE_CODE(nprach_params_r14.nprach_num_subcarriers_r14.unpack(bref));
    }
    if (nprach_params_r14.nprach_subcarrier_msg3_range_start_r14_present) {
      HANDLE_CODE(nprach_params_r14.nprach_subcarrier_msg3_range_start_r14.unpack(bref));
    }
    if (nprach_params_r14.npdcch_num_repeats_ra_r14_present) {
      HANDLE_CODE(nprach_params_r14.npdcch_num_repeats_ra_r14.unpack(bref));
    }
    if (nprach_params_r14.npdcch_start_sf_css_ra_r14_present) {
      HANDLE_CODE(nprach_params_r14.npdcch_start_sf_css_ra_r14.unpack(bref));
    }
    if (nprach_params_r14.npdcch_offset_ra_r14_present) {
      HANDLE_CODE(nprach_params_r14.npdcch_offset_ra_r14.unpack(bref));
    }
    if (nprach_params_r14.nprach_num_cbra_start_subcarriers_r14_present) {
      HANDLE_CODE(nprach_params_r14.nprach_num_cbra_start_subcarriers_r14.unpack(bref));
    }
    if (nprach_params_r14.npdcch_carrier_idx_r14_present) {
      HANDLE_CODE(unpack_integer(nprach_params_r14.npdcch_carrier_idx_r14, bref, (uint8_t)1u, (uint8_t)15u));
    }
  }

  return SRSASN_SUCCESS;
}
void nprach_params_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nprach_params_r14_present) {
    j.write_fieldname("nprach-Parameters-r14");
    j.start_obj();
    if (nprach_params_r14.nprach_periodicity_r14_present) {
      j.write_str("nprach-Periodicity-r14", nprach_params_r14.nprach_periodicity_r14.to_string());
    }
    if (nprach_params_r14.nprach_start_time_r14_present) {
      j.write_str("nprach-StartTime-r14", nprach_params_r14.nprach_start_time_r14.to_string());
    }
    if (nprach_params_r14.nprach_subcarrier_offset_r14_present) {
      j.write_str("nprach-SubcarrierOffset-r14", nprach_params_r14.nprach_subcarrier_offset_r14.to_string());
    }
    if (nprach_params_r14.nprach_num_subcarriers_r14_present) {
      j.write_str("nprach-NumSubcarriers-r14", nprach_params_r14.nprach_num_subcarriers_r14.to_string());
    }
    if (nprach_params_r14.nprach_subcarrier_msg3_range_start_r14_present) {
      j.write_str("nprach-SubcarrierMSG3-RangeStart-r14",
                  nprach_params_r14.nprach_subcarrier_msg3_range_start_r14.to_string());
    }
    if (nprach_params_r14.npdcch_num_repeats_ra_r14_present) {
      j.write_str("npdcch-NumRepetitions-RA-r14", nprach_params_r14.npdcch_num_repeats_ra_r14.to_string());
    }
    if (nprach_params_r14.npdcch_start_sf_css_ra_r14_present) {
      j.write_str("npdcch-StartSF-CSS-RA-r14", nprach_params_r14.npdcch_start_sf_css_ra_r14.to_string());
    }
    if (nprach_params_r14.npdcch_offset_ra_r14_present) {
      j.write_str("npdcch-Offset-RA-r14", nprach_params_r14.npdcch_offset_ra_r14.to_string());
    }
    if (nprach_params_r14.nprach_num_cbra_start_subcarriers_r14_present) {
      j.write_str("nprach-NumCBRA-StartSubcarriers-r14",
                  nprach_params_r14.nprach_num_cbra_start_subcarriers_r14.to_string());
    }
    if (nprach_params_r14.npdcch_carrier_idx_r14_present) {
      j.write_int("npdcch-CarrierIndex-r14", nprach_params_r14.npdcch_carrier_idx_r14);
    }
    j.end_obj();
  }
  j.end_obj();
}

std::string nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_periodicity_r14_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms80", "ms160", "ms240", "ms320", "ms640", "ms1280", "ms2560"};
  return convert_enum_idx(options, 8, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_periodicity_r14_e_");
}
uint16_t nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_periodicity_r14_opts::to_number() const
{
  static const uint16_t options[] = {40, 80, 160, 240, 320, 640, 1280, 2560};
  return map_enum_number(options, 8, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_periodicity_r14_e_");
}

std::string nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_start_time_r14_opts::to_string() const
{
  static const char* options[] = {"ms8", "ms16", "ms32", "ms64", "ms128", "ms256", "ms512", "ms1024"};
  return convert_enum_idx(options, 8, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_start_time_r14_e_");
}
uint16_t nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_start_time_r14_opts::to_number() const
{
  static const uint16_t options[] = {8, 16, 32, 64, 128, 256, 512, 1024};
  return map_enum_number(options, 8, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_start_time_r14_e_");
}

std::string nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_subcarrier_offset_r14_opts::to_string() const
{
  static const char* options[] = {"n0", "n12", "n24", "n36", "n2", "n18", "n34", "spare1"};
  return convert_enum_idx(
      options, 8, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_subcarrier_offset_r14_e_");
}
uint8_t nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_subcarrier_offset_r14_opts::to_number() const
{
  static const uint8_t options[] = {0, 12, 24, 36, 2, 18, 34};
  return map_enum_number(
      options, 7, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_subcarrier_offset_r14_e_");
}

std::string nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_num_subcarriers_r14_opts::to_string() const
{
  static const char* options[] = {"n12", "n24", "n36", "n48"};
  return convert_enum_idx(
      options, 4, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_num_subcarriers_r14_e_");
}
uint8_t nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_num_subcarriers_r14_opts::to_number() const
{
  static const uint8_t options[] = {12, 24, 36, 48};
  return map_enum_number(
      options, 4, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_num_subcarriers_r14_e_");
}

std::string nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_subcarrier_msg3_range_start_r14_opts::to_string() const
{
  static const char* options[] = {"zero", "oneThird", "twoThird", "one"};
  return convert_enum_idx(
      options, 4, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_subcarrier_msg3_range_start_r14_e_");
}
float nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_subcarrier_msg3_range_start_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.3333333333333333, 0.6666666666666666, 1.0};
  return map_enum_number(
      options, 4, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_subcarrier_msg3_range_start_r14_e_");
}
std::string
nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_subcarrier_msg3_range_start_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "1/3", "2/3", "1"};
  return convert_enum_idx(
      options, 4, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_subcarrier_msg3_range_start_r14_e_");
}

std::string nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_num_repeats_ra_r14_opts::to_string() const
{
  static const char* options[] = {"r1",
                                  "r2",
                                  "r4",
                                  "r8",
                                  "r16",
                                  "r32",
                                  "r64",
                                  "r128",
                                  "r256",
                                  "r512",
                                  "r1024",
                                  "r2048",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(
      options, 16, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_num_repeats_ra_r14_e_");
}
uint16_t nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_num_repeats_ra_r14_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
  return map_enum_number(
      options, 12, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_num_repeats_ra_r14_e_");
}

std::string nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_start_sf_css_ra_r14_opts::to_string() const
{
  static const char* options[] = {"v1dot5", "v2", "v4", "v8", "v16", "v32", "v48", "v64"};
  return convert_enum_idx(
      options, 8, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_start_sf_css_ra_r14_e_");
}
float nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_start_sf_css_ra_r14_opts::to_number() const
{
  static const float options[] = {1.5, 2.0, 4.0, 8.0, 16.0, 32.0, 48.0, 64.0};
  return map_enum_number(
      options, 8, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_start_sf_css_ra_r14_e_");
}
std::string nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_start_sf_css_ra_r14_opts::to_number_string() const
{
  static const char* options[] = {"1.5", "2", "4", "8", "16", "32", "48", "64"};
  return convert_enum_idx(
      options, 8, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_start_sf_css_ra_r14_e_");
}

std::string nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_offset_ra_r14_opts::to_string() const
{
  static const char* options[] = {"zero", "oneEighth", "oneFourth", "threeEighth"};
  return convert_enum_idx(options, 4, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_offset_ra_r14_e_");
}
float nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_offset_ra_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.125, 0.25, 0.375};
  return map_enum_number(options, 4, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_offset_ra_r14_e_");
}
std::string nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_offset_ra_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "1/8", "1/4", "3/8"};
  return convert_enum_idx(options, 4, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::npdcch_offset_ra_r14_e_");
}

std::string nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_num_cbra_start_subcarriers_r14_opts::to_string() const
{
  static const char* options[] = {
      "n8", "n10", "n11", "n12", "n20", "n22", "n23", "n24", "n32", "n34", "n35", "n36", "n40", "n44", "n46", "n48"};
  return convert_enum_idx(
      options, 16, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_num_cbra_start_subcarriers_r14_e_");
}
uint8_t nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_num_cbra_start_subcarriers_r14_opts::to_number() const
{
  static const uint8_t options[] = {8, 10, 11, 12, 20, 22, 23, 24, 32, 34, 35, 36, 40, 44, 46, 48};
  return map_enum_number(
      options, 16, value, "nprach_params_nb_r14_s::nprach_params_r14_s_::nprach_num_cbra_start_subcarriers_r14_e_");
}

// NPRACH-Parameters-NB-v1330 ::= SEQUENCE
SRSASN_CODE nprach_params_nb_v1330_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(nprach_num_cbra_start_subcarriers_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nprach_params_nb_v1330_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(nprach_num_cbra_start_subcarriers_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void nprach_params_nb_v1330_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("nprach-NumCBRA-StartSubcarriers-r13", nprach_num_cbra_start_subcarriers_r13.to_string());
  j.end_obj();
}

std::string nprach_params_nb_v1330_s::nprach_num_cbra_start_subcarriers_r13_opts::to_string() const
{
  static const char* options[] = {
      "n8", "n10", "n11", "n12", "n20", "n22", "n23", "n24", "n32", "n34", "n35", "n36", "n40", "n44", "n46", "n48"};
  return convert_enum_idx(options, 16, value, "nprach_params_nb_v1330_s::nprach_num_cbra_start_subcarriers_r13_e_");
}
uint8_t nprach_params_nb_v1330_s::nprach_num_cbra_start_subcarriers_r13_opts::to_number() const
{
  static const uint8_t options[] = {8, 10, 11, 12, 20, 22, 23, 24, 32, 34, 35, 36, 40, 44, 46, 48};
  return map_enum_number(options, 16, value, "nprach_params_nb_v1330_s::nprach_num_cbra_start_subcarriers_r13_e_");
}

// NPRACH-ParametersFmt2-NB-r15 ::= SEQUENCE
SRSASN_CODE nprach_params_fmt2_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nprach_params_r15_present, 1));

  if (nprach_params_r15_present) {
    bref.pack(nprach_params_r15.ext, 1);
    HANDLE_CODE(bref.pack(nprach_params_r15.nprach_periodicity_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.nprach_start_time_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.nprach_subcarrier_offset_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.nprach_num_subcarriers_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.nprach_subcarrier_msg3_range_start_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.npdcch_num_repeats_ra_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.npdcch_start_sf_css_ra_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.npdcch_offset_ra_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.nprach_num_cbra_start_subcarriers_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.npdcch_carrier_idx_r15_present, 1));
    if (nprach_params_r15.nprach_periodicity_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_periodicity_r15.pack(bref));
    }
    if (nprach_params_r15.nprach_start_time_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_start_time_r15.pack(bref));
    }
    if (nprach_params_r15.nprach_subcarrier_offset_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_subcarrier_offset_r15.pack(bref));
    }
    if (nprach_params_r15.nprach_num_subcarriers_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_num_subcarriers_r15.pack(bref));
    }
    if (nprach_params_r15.nprach_subcarrier_msg3_range_start_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_subcarrier_msg3_range_start_r15.pack(bref));
    }
    if (nprach_params_r15.npdcch_num_repeats_ra_r15_present) {
      HANDLE_CODE(nprach_params_r15.npdcch_num_repeats_ra_r15.pack(bref));
    }
    if (nprach_params_r15.npdcch_start_sf_css_ra_r15_present) {
      HANDLE_CODE(nprach_params_r15.npdcch_start_sf_css_ra_r15.pack(bref));
    }
    if (nprach_params_r15.npdcch_offset_ra_r15_present) {
      HANDLE_CODE(nprach_params_r15.npdcch_offset_ra_r15.pack(bref));
    }
    if (nprach_params_r15.nprach_num_cbra_start_subcarriers_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_num_cbra_start_subcarriers_r15.pack(bref));
    }
    if (nprach_params_r15.npdcch_carrier_idx_r15_present) {
      HANDLE_CODE(pack_integer(bref, nprach_params_r15.npdcch_carrier_idx_r15, (uint8_t)1u, (uint8_t)15u));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nprach_params_fmt2_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nprach_params_r15_present, 1));

  if (nprach_params_r15_present) {
    bref.unpack(nprach_params_r15.ext, 1);
    HANDLE_CODE(bref.unpack(nprach_params_r15.nprach_periodicity_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.nprach_start_time_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.nprach_subcarrier_offset_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.nprach_num_subcarriers_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.nprach_subcarrier_msg3_range_start_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.npdcch_num_repeats_ra_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.npdcch_start_sf_css_ra_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.npdcch_offset_ra_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.nprach_num_cbra_start_subcarriers_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.npdcch_carrier_idx_r15_present, 1));
    if (nprach_params_r15.nprach_periodicity_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_periodicity_r15.unpack(bref));
    }
    if (nprach_params_r15.nprach_start_time_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_start_time_r15.unpack(bref));
    }
    if (nprach_params_r15.nprach_subcarrier_offset_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_subcarrier_offset_r15.unpack(bref));
    }
    if (nprach_params_r15.nprach_num_subcarriers_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_num_subcarriers_r15.unpack(bref));
    }
    if (nprach_params_r15.nprach_subcarrier_msg3_range_start_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_subcarrier_msg3_range_start_r15.unpack(bref));
    }
    if (nprach_params_r15.npdcch_num_repeats_ra_r15_present) {
      HANDLE_CODE(nprach_params_r15.npdcch_num_repeats_ra_r15.unpack(bref));
    }
    if (nprach_params_r15.npdcch_start_sf_css_ra_r15_present) {
      HANDLE_CODE(nprach_params_r15.npdcch_start_sf_css_ra_r15.unpack(bref));
    }
    if (nprach_params_r15.npdcch_offset_ra_r15_present) {
      HANDLE_CODE(nprach_params_r15.npdcch_offset_ra_r15.unpack(bref));
    }
    if (nprach_params_r15.nprach_num_cbra_start_subcarriers_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_num_cbra_start_subcarriers_r15.unpack(bref));
    }
    if (nprach_params_r15.npdcch_carrier_idx_r15_present) {
      HANDLE_CODE(unpack_integer(nprach_params_r15.npdcch_carrier_idx_r15, bref, (uint8_t)1u, (uint8_t)15u));
    }
  }

  return SRSASN_SUCCESS;
}
void nprach_params_fmt2_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nprach_params_r15_present) {
    j.write_fieldname("nprach-Parameters-r15");
    j.start_obj();
    if (nprach_params_r15.nprach_periodicity_r15_present) {
      j.write_str("nprach-Periodicity-r15", nprach_params_r15.nprach_periodicity_r15.to_string());
    }
    if (nprach_params_r15.nprach_start_time_r15_present) {
      j.write_str("nprach-StartTime-r15", nprach_params_r15.nprach_start_time_r15.to_string());
    }
    if (nprach_params_r15.nprach_subcarrier_offset_r15_present) {
      j.write_str("nprach-SubcarrierOffset-r15", nprach_params_r15.nprach_subcarrier_offset_r15.to_string());
    }
    if (nprach_params_r15.nprach_num_subcarriers_r15_present) {
      j.write_str("nprach-NumSubcarriers-r15", nprach_params_r15.nprach_num_subcarriers_r15.to_string());
    }
    if (nprach_params_r15.nprach_subcarrier_msg3_range_start_r15_present) {
      j.write_str("nprach-SubcarrierMSG3-RangeStart-r15",
                  nprach_params_r15.nprach_subcarrier_msg3_range_start_r15.to_string());
    }
    if (nprach_params_r15.npdcch_num_repeats_ra_r15_present) {
      j.write_str("npdcch-NumRepetitions-RA-r15", nprach_params_r15.npdcch_num_repeats_ra_r15.to_string());
    }
    if (nprach_params_r15.npdcch_start_sf_css_ra_r15_present) {
      j.write_str("npdcch-StartSF-CSS-RA-r15", nprach_params_r15.npdcch_start_sf_css_ra_r15.to_string());
    }
    if (nprach_params_r15.npdcch_offset_ra_r15_present) {
      j.write_str("npdcch-Offset-RA-r15", nprach_params_r15.npdcch_offset_ra_r15.to_string());
    }
    if (nprach_params_r15.nprach_num_cbra_start_subcarriers_r15_present) {
      j.write_str("nprach-NumCBRA-StartSubcarriers-r15",
                  nprach_params_r15.nprach_num_cbra_start_subcarriers_r15.to_string());
    }
    if (nprach_params_r15.npdcch_carrier_idx_r15_present) {
      j.write_int("npdcch-CarrierIndex-r15", nprach_params_r15.npdcch_carrier_idx_r15);
    }
    j.end_obj();
  }
  j.end_obj();
}

std::string nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_periodicity_r15_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms80", "ms160", "ms320", "ms640", "ms1280", "ms2560", "ms5120"};
  return convert_enum_idx(
      options, 8, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_periodicity_r15_e_");
}
uint16_t nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_periodicity_r15_opts::to_number() const
{
  static const uint16_t options[] = {40, 80, 160, 320, 640, 1280, 2560, 5120};
  return map_enum_number(
      options, 8, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_periodicity_r15_e_");
}

std::string nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_start_time_r15_opts::to_string() const
{
  static const char* options[] = {"ms8", "ms16", "ms32", "ms64", "ms128", "ms256", "ms512", "ms1024"};
  return convert_enum_idx(
      options, 8, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_start_time_r15_e_");
}
uint16_t nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_start_time_r15_opts::to_number() const
{
  static const uint16_t options[] = {8, 16, 32, 64, 128, 256, 512, 1024};
  return map_enum_number(
      options, 8, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_start_time_r15_e_");
}

std::string nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_offset_r15_opts::to_string() const
{
  static const char* options[] = {
      "n0", "n36", "n72", "n108", "n6", "n54", "n102", "n42", "n78", "n90", "n12", "n24", "n48", "n84", "n60", "n18"};
  return convert_enum_idx(
      options, 16, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_offset_r15_e_");
}
uint8_t nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_offset_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 36, 72, 108, 6, 54, 102, 42, 78, 90, 12, 24, 48, 84, 60, 18};
  return map_enum_number(
      options, 16, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_offset_r15_e_");
}

std::string nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_num_subcarriers_r15_opts::to_string() const
{
  static const char* options[] = {"n36", "n72", "n108", "n144"};
  return convert_enum_idx(
      options, 4, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_num_subcarriers_r15_e_");
}
uint8_t nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_num_subcarriers_r15_opts::to_number() const
{
  static const uint8_t options[] = {36, 72, 108, 144};
  return map_enum_number(
      options, 4, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_num_subcarriers_r15_e_");
}

std::string
nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_msg3_range_start_r15_opts::to_string() const
{
  static const char* options[] = {"zero", "oneThird", "twoThird", "one"};
  return convert_enum_idx(
      options,
      4,
      value,
      "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_msg3_range_start_r15_e_");
}
float nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_msg3_range_start_r15_opts::to_number() const
{
  static const float options[] = {0.0, 0.3333333333333333, 0.6666666666666666, 1.0};
  return map_enum_number(
      options,
      4,
      value,
      "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_msg3_range_start_r15_e_");
}
std::string
nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_msg3_range_start_r15_opts::to_number_string() const
{
  static const char* options[] = {"0", "1/3", "2/3", "1"};
  return convert_enum_idx(
      options,
      4,
      value,
      "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_msg3_range_start_r15_e_");
}

std::string nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_num_repeats_ra_r15_opts::to_string() const
{
  static const char* options[] = {"r1",
                                  "r2",
                                  "r4",
                                  "r8",
                                  "r16",
                                  "r32",
                                  "r64",
                                  "r128",
                                  "r256",
                                  "r512",
                                  "r1024",
                                  "r2048",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(
      options, 16, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_num_repeats_ra_r15_e_");
}
uint16_t nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_num_repeats_ra_r15_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
  return map_enum_number(
      options, 12, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_num_repeats_ra_r15_e_");
}

std::string nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_start_sf_css_ra_r15_opts::to_string() const
{
  static const char* options[] = {"v1dot5", "v2", "v4", "v8", "v16", "v32", "v48", "v64"};
  return convert_enum_idx(
      options, 8, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_start_sf_css_ra_r15_e_");
}
float nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_start_sf_css_ra_r15_opts::to_number() const
{
  static const float options[] = {1.5, 2.0, 4.0, 8.0, 16.0, 32.0, 48.0, 64.0};
  return map_enum_number(
      options, 8, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_start_sf_css_ra_r15_e_");
}
std::string nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_start_sf_css_ra_r15_opts::to_number_string() const
{
  static const char* options[] = {"1.5", "2", "4", "8", "16", "32", "48", "64"};
  return convert_enum_idx(
      options, 8, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_start_sf_css_ra_r15_e_");
}

std::string nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_offset_ra_r15_opts::to_string() const
{
  static const char* options[] = {"zero", "oneEighth", "oneFourth", "threeEighth"};
  return convert_enum_idx(
      options, 4, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_offset_ra_r15_e_");
}
float nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_offset_ra_r15_opts::to_number() const
{
  static const float options[] = {0.0, 0.125, 0.25, 0.375};
  return map_enum_number(
      options, 4, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_offset_ra_r15_e_");
}
std::string nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_offset_ra_r15_opts::to_number_string() const
{
  static const char* options[] = {"0", "1/8", "1/4", "3/8"};
  return convert_enum_idx(
      options, 4, value, "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::npdcch_offset_ra_r15_e_");
}

std::string
nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_num_cbra_start_subcarriers_r15_opts::to_string() const
{
  static const char* options[] = {"n24",
                                  "n30",
                                  "n33",
                                  "n36",
                                  "n60",
                                  "n66",
                                  "n69",
                                  "n72",
                                  "n96",
                                  "n102",
                                  "n105",
                                  "n108",
                                  "n120",
                                  "n132",
                                  "n138",
                                  "n144"};
  return convert_enum_idx(
      options,
      16,
      value,
      "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_num_cbra_start_subcarriers_r15_e_");
}
uint8_t nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_num_cbra_start_subcarriers_r15_opts::to_number() const
{
  static const uint8_t options[] = {24, 30, 33, 36, 60, 66, 69, 72, 96, 102, 105, 108, 120, 132, 138, 144};
  return map_enum_number(options,
                         16,
                         value,
                         "nprach_params_fmt2_nb_r15_s::nprach_params_r15_s_::nprach_num_cbra_start_subcarriers_r15_e_");
}

// NPRACH-ParametersTDD-NB-r15 ::= SEQUENCE
SRSASN_CODE nprach_params_tdd_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nprach_params_r15_present, 1));

  if (nprach_params_r15_present) {
    bref.pack(nprach_params_r15.ext, 1);
    HANDLE_CODE(bref.pack(nprach_params_r15.nprach_periodicity_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.nprach_start_time_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.nprach_subcarrier_offset_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.nprach_num_subcarriers_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.nprach_subcarrier_msg3_range_start_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.npdcch_num_repeats_ra_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.npdcch_start_sf_css_ra_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.npdcch_offset_ra_r15_present, 1));
    HANDLE_CODE(bref.pack(nprach_params_r15.nprach_num_cbra_start_subcarriers_r15_present, 1));
    if (nprach_params_r15.nprach_periodicity_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_periodicity_r15.pack(bref));
    }
    if (nprach_params_r15.nprach_start_time_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_start_time_r15.pack(bref));
    }
    if (nprach_params_r15.nprach_subcarrier_offset_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_subcarrier_offset_r15.pack(bref));
    }
    if (nprach_params_r15.nprach_num_subcarriers_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_num_subcarriers_r15.pack(bref));
    }
    if (nprach_params_r15.nprach_subcarrier_msg3_range_start_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_subcarrier_msg3_range_start_r15.pack(bref));
    }
    if (nprach_params_r15.npdcch_num_repeats_ra_r15_present) {
      HANDLE_CODE(nprach_params_r15.npdcch_num_repeats_ra_r15.pack(bref));
    }
    if (nprach_params_r15.npdcch_start_sf_css_ra_r15_present) {
      HANDLE_CODE(nprach_params_r15.npdcch_start_sf_css_ra_r15.pack(bref));
    }
    if (nprach_params_r15.npdcch_offset_ra_r15_present) {
      HANDLE_CODE(nprach_params_r15.npdcch_offset_ra_r15.pack(bref));
    }
    if (nprach_params_r15.nprach_num_cbra_start_subcarriers_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_num_cbra_start_subcarriers_r15.pack(bref));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nprach_params_tdd_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nprach_params_r15_present, 1));

  if (nprach_params_r15_present) {
    bref.unpack(nprach_params_r15.ext, 1);
    HANDLE_CODE(bref.unpack(nprach_params_r15.nprach_periodicity_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.nprach_start_time_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.nprach_subcarrier_offset_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.nprach_num_subcarriers_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.nprach_subcarrier_msg3_range_start_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.npdcch_num_repeats_ra_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.npdcch_start_sf_css_ra_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.npdcch_offset_ra_r15_present, 1));
    HANDLE_CODE(bref.unpack(nprach_params_r15.nprach_num_cbra_start_subcarriers_r15_present, 1));
    if (nprach_params_r15.nprach_periodicity_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_periodicity_r15.unpack(bref));
    }
    if (nprach_params_r15.nprach_start_time_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_start_time_r15.unpack(bref));
    }
    if (nprach_params_r15.nprach_subcarrier_offset_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_subcarrier_offset_r15.unpack(bref));
    }
    if (nprach_params_r15.nprach_num_subcarriers_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_num_subcarriers_r15.unpack(bref));
    }
    if (nprach_params_r15.nprach_subcarrier_msg3_range_start_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_subcarrier_msg3_range_start_r15.unpack(bref));
    }
    if (nprach_params_r15.npdcch_num_repeats_ra_r15_present) {
      HANDLE_CODE(nprach_params_r15.npdcch_num_repeats_ra_r15.unpack(bref));
    }
    if (nprach_params_r15.npdcch_start_sf_css_ra_r15_present) {
      HANDLE_CODE(nprach_params_r15.npdcch_start_sf_css_ra_r15.unpack(bref));
    }
    if (nprach_params_r15.npdcch_offset_ra_r15_present) {
      HANDLE_CODE(nprach_params_r15.npdcch_offset_ra_r15.unpack(bref));
    }
    if (nprach_params_r15.nprach_num_cbra_start_subcarriers_r15_present) {
      HANDLE_CODE(nprach_params_r15.nprach_num_cbra_start_subcarriers_r15.unpack(bref));
    }
  }

  return SRSASN_SUCCESS;
}
void nprach_params_tdd_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nprach_params_r15_present) {
    j.write_fieldname("nprach-Parameters-r15");
    j.start_obj();
    if (nprach_params_r15.nprach_periodicity_r15_present) {
      j.write_str("nprach-Periodicity-r15", nprach_params_r15.nprach_periodicity_r15.to_string());
    }
    if (nprach_params_r15.nprach_start_time_r15_present) {
      j.write_str("nprach-StartTime-r15", nprach_params_r15.nprach_start_time_r15.to_string());
    }
    if (nprach_params_r15.nprach_subcarrier_offset_r15_present) {
      j.write_str("nprach-SubcarrierOffset-r15", nprach_params_r15.nprach_subcarrier_offset_r15.to_string());
    }
    if (nprach_params_r15.nprach_num_subcarriers_r15_present) {
      j.write_str("nprach-NumSubcarriers-r15", nprach_params_r15.nprach_num_subcarriers_r15.to_string());
    }
    if (nprach_params_r15.nprach_subcarrier_msg3_range_start_r15_present) {
      j.write_str("nprach-SubcarrierMSG3-RangeStart-r15",
                  nprach_params_r15.nprach_subcarrier_msg3_range_start_r15.to_string());
    }
    if (nprach_params_r15.npdcch_num_repeats_ra_r15_present) {
      j.write_str("npdcch-NumRepetitions-RA-r15", nprach_params_r15.npdcch_num_repeats_ra_r15.to_string());
    }
    if (nprach_params_r15.npdcch_start_sf_css_ra_r15_present) {
      j.write_str("npdcch-StartSF-CSS-RA-r15", nprach_params_r15.npdcch_start_sf_css_ra_r15.to_string());
    }
    if (nprach_params_r15.npdcch_offset_ra_r15_present) {
      j.write_str("npdcch-Offset-RA-r15", nprach_params_r15.npdcch_offset_ra_r15.to_string());
    }
    if (nprach_params_r15.nprach_num_cbra_start_subcarriers_r15_present) {
      j.write_str("nprach-NumCBRA-StartSubcarriers-r15",
                  nprach_params_r15.nprach_num_cbra_start_subcarriers_r15.to_string());
    }
    j.end_obj();
  }
  j.end_obj();
}

std::string nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_periodicity_r15_opts::to_string() const
{
  static const char* options[] = {"ms80", "ms160", "ms320", "ms640", "ms1280", "ms2560", "ms5120", "ms10240"};
  return convert_enum_idx(
      options, 8, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_periodicity_r15_e_");
}
uint16_t nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_periodicity_r15_opts::to_number() const
{
  static const uint16_t options[] = {80, 160, 320, 640, 1280, 2560, 5120, 10240};
  return map_enum_number(
      options, 8, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_periodicity_r15_e_");
}

std::string nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_start_time_r15_opts::to_string() const
{
  static const char* options[] = {"ms10",
                                  "ms20",
                                  "ms40",
                                  "ms80",
                                  "ms160",
                                  "ms320",
                                  "ms640",
                                  "ms1280",
                                  "ms2560",
                                  "ms5120",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(
      options, 16, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_start_time_r15_e_");
}
uint16_t nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_start_time_r15_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5120};
  return map_enum_number(
      options, 10, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_start_time_r15_e_");
}

std::string nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_offset_r15_opts::to_string() const
{
  static const char* options[] = {"n0", "n12", "n24", "n36", "n2", "n18", "n34", "spare1"};
  return convert_enum_idx(
      options, 8, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_offset_r15_e_");
}
uint8_t nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_offset_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 12, 24, 36, 2, 18, 34};
  return map_enum_number(
      options, 7, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_offset_r15_e_");
}

std::string nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_num_subcarriers_r15_opts::to_string() const
{
  static const char* options[] = {"n12", "n24", "n36", "n48"};
  return convert_enum_idx(
      options, 4, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_num_subcarriers_r15_e_");
}
uint8_t nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_num_subcarriers_r15_opts::to_number() const
{
  static const uint8_t options[] = {12, 24, 36, 48};
  return map_enum_number(
      options, 4, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_num_subcarriers_r15_e_");
}

std::string
nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_msg3_range_start_r15_opts::to_string() const
{
  static const char* options[] = {"zero", "oneThird", "twoThird", "one"};
  return convert_enum_idx(
      options, 4, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_msg3_range_start_r15_e_");
}
float nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_msg3_range_start_r15_opts::to_number() const
{
  static const float options[] = {0.0, 0.3333333333333333, 0.6666666666666666, 1.0};
  return map_enum_number(
      options, 4, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_msg3_range_start_r15_e_");
}
std::string
nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_msg3_range_start_r15_opts::to_number_string() const
{
  static const char* options[] = {"0", "1/3", "2/3", "1"};
  return convert_enum_idx(
      options, 4, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_subcarrier_msg3_range_start_r15_e_");
}

std::string nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::npdcch_num_repeats_ra_r15_opts::to_string() const
{
  static const char* options[] = {"r1",
                                  "r2",
                                  "r4",
                                  "r8",
                                  "r16",
                                  "r32",
                                  "r64",
                                  "r128",
                                  "r256",
                                  "r512",
                                  "r1024",
                                  "r2048",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(
      options, 16, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::npdcch_num_repeats_ra_r15_e_");
}
uint16_t nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::npdcch_num_repeats_ra_r15_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
  return map_enum_number(
      options, 12, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::npdcch_num_repeats_ra_r15_e_");
}

std::string nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::npdcch_start_sf_css_ra_r15_opts::to_string() const
{
  static const char* options[] = {"v4", "v8", "v16", "v32", "v48", "v64", "v96", "v128"};
  return convert_enum_idx(
      options, 8, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::npdcch_start_sf_css_ra_r15_e_");
}
uint8_t nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::npdcch_start_sf_css_ra_r15_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 16, 32, 48, 64, 96, 128};
  return map_enum_number(
      options, 8, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::npdcch_start_sf_css_ra_r15_e_");
}

std::string nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::npdcch_offset_ra_r15_opts::to_string() const
{
  static const char* options[] = {"zero", "oneEighth", "oneFourth", "threeEighth"};
  return convert_enum_idx(
      options, 4, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::npdcch_offset_ra_r15_e_");
}
float nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::npdcch_offset_ra_r15_opts::to_number() const
{
  static const float options[] = {0.0, 0.125, 0.25, 0.375};
  return map_enum_number(
      options, 4, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::npdcch_offset_ra_r15_e_");
}
std::string nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::npdcch_offset_ra_r15_opts::to_number_string() const
{
  static const char* options[] = {"0", "1/8", "1/4", "3/8"};
  return convert_enum_idx(
      options, 4, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::npdcch_offset_ra_r15_e_");
}

std::string
nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_num_cbra_start_subcarriers_r15_opts::to_string() const
{
  static const char* options[] = {
      "n8", "n10", "n11", "n12", "n20", "n22", "n23", "n24", "n32", "n34", "n35", "n36", "n40", "n44", "n46", "n48"};
  return convert_enum_idx(
      options, 16, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_num_cbra_start_subcarriers_r15_e_");
}
uint8_t nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_num_cbra_start_subcarriers_r15_opts::to_number() const
{
  static const uint8_t options[] = {8, 10, 11, 12, 20, 22, 23, 24, 32, 34, 35, 36, 40, 44, 46, 48};
  return map_enum_number(
      options, 16, value, "nprach_params_tdd_nb_r15_s::nprach_params_r15_s_::nprach_num_cbra_start_subcarriers_r15_e_");
}

// PagingWeight-NB-r14 ::= ENUMERATED
std::string paging_weight_nb_r14_opts::to_string() const
{
  static const char* options[] = {
      "w1", "w2", "w3", "w4", "w5", "w6", "w7", "w8", "w9", "w10", "w11", "w12", "w13", "w14", "w15", "w16"};
  return convert_enum_idx(options, 16, value, "paging_weight_nb_r14_e");
}
uint8_t paging_weight_nb_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  return map_enum_number(options, 16, value, "paging_weight_nb_r14_e");
}

// RACH-Info-NB-r13 ::= SEQUENCE
SRSASN_CODE rach_info_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ra_resp_win_size_r13.pack(bref));
  HANDLE_CODE(mac_contention_resolution_timer_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_info_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ra_resp_win_size_r13.unpack(bref));
  HANDLE_CODE(mac_contention_resolution_timer_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void rach_info_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ra-ResponseWindowSize-r13", ra_resp_win_size_r13.to_string());
  j.write_str("mac-ContentionResolutionTimer-r13", mac_contention_resolution_timer_r13.to_string());
  j.end_obj();
}

std::string rach_info_nb_r13_s::ra_resp_win_size_r13_opts::to_string() const
{
  static const char* options[] = {"pp2", "pp3", "pp4", "pp5", "pp6", "pp7", "pp8", "pp10"};
  return convert_enum_idx(options, 8, value, "rach_info_nb_r13_s::ra_resp_win_size_r13_e_");
}
uint8_t rach_info_nb_r13_s::ra_resp_win_size_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 5, 6, 7, 8, 10};
  return map_enum_number(options, 8, value, "rach_info_nb_r13_s::ra_resp_win_size_r13_e_");
}

std::string rach_info_nb_r13_s::mac_contention_resolution_timer_r13_opts::to_string() const
{
  static const char* options[] = {"pp1", "pp2", "pp3", "pp4", "pp8", "pp16", "pp32", "pp64"};
  return convert_enum_idx(options, 8, value, "rach_info_nb_r13_s::mac_contention_resolution_timer_r13_e_");
}
uint8_t rach_info_nb_r13_s::mac_contention_resolution_timer_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 8, 16, 32, 64};
  return map_enum_number(options, 8, value, "rach_info_nb_r13_s::mac_contention_resolution_timer_r13_e_");
}

// RACH-Info-NB-v1530 ::= SEQUENCE
SRSASN_CODE rach_info_nb_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(mac_contention_resolution_timer_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_info_nb_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(mac_contention_resolution_timer_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void rach_info_nb_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("mac-ContentionResolutionTimer-r15", mac_contention_resolution_timer_r15.to_string());
  j.end_obj();
}

std::string rach_info_nb_v1530_s::mac_contention_resolution_timer_r15_opts::to_string() const
{
  static const char* options[] = {"pp1", "pp2", "pp3", "pp4", "pp8", "pp16", "pp32", "pp64"};
  return convert_enum_idx(options, 8, value, "rach_info_nb_v1530_s::mac_contention_resolution_timer_r15_e_");
}
uint8_t rach_info_nb_v1530_s::mac_contention_resolution_timer_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 8, 16, 32, 64};
  return map_enum_number(options, 8, value, "rach_info_nb_v1530_s::mac_contention_resolution_timer_r15_e_");
}

// SchedulingInfo-NB-v1530 ::= SEQUENCE
SRSASN_CODE sched_info_nb_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sib_map_info_v1530_present, 1));

  if (sib_map_info_v1530_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sib_map_info_v1530, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_info_nb_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sib_map_info_v1530_present, 1));

  if (sib_map_info_v1530_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sib_map_info_v1530, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void sched_info_nb_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sib_map_info_v1530_present) {
    j.start_array("sib-MappingInfo-v1530");
    for (const auto& e1 : sib_map_info_v1530) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  j.end_obj();
}

// WUS-MaxDurationFactor-NB-r15 ::= ENUMERATED
std::string wus_max_dur_factor_nb_r15_opts::to_string() const
{
  static const char* options[] = {"one128th", "one64th", "one32th", "one16th", "oneEighth", "oneQuarter", "oneHalf"};
  return convert_enum_idx(options, 7, value, "wus_max_dur_factor_nb_r15_e");
}
float wus_max_dur_factor_nb_r15_opts::to_number() const
{
  static const float options[] = {128.0, 64.0, 32.0, 16.0, 0.125, 0.25, 0.5};
  return map_enum_number(options, 7, value, "wus_max_dur_factor_nb_r15_e");
}
std::string wus_max_dur_factor_nb_r15_opts::to_number_string() const
{
  static const char* options[] = {"128", "64", "32", "16", "1/8", "1/4", "1/2"};
  return convert_enum_idx(options, 7, value, "wus_max_dur_factor_nb_r15_e");
}

// DL-CarrierConfigCommon-NB-r14 ::= SEQUENCE
SRSASN_CODE dl_carrier_cfg_common_nb_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(inband_carrier_info_r14_present, 1));
  HANDLE_CODE(bref.pack(nrs_pwr_offset_non_anchor_r14_present, 1));

  HANDLE_CODE(dl_carrier_freq_r14.pack(bref));
  HANDLE_CODE(dl_bitmap_non_anchor_r14.pack(bref));
  HANDLE_CODE(dl_gap_non_anchor_r14.pack(bref));
  if (inband_carrier_info_r14_present) {
    HANDLE_CODE(bref.pack(inband_carrier_info_r14.same_pci_ind_r14_present, 1));
    if (inband_carrier_info_r14.same_pci_ind_r14_present) {
      HANDLE_CODE(inband_carrier_info_r14.same_pci_ind_r14.pack(bref));
    }
    HANDLE_CODE(inband_carrier_info_r14.eutra_ctrl_region_size_r14.pack(bref));
  }
  if (nrs_pwr_offset_non_anchor_r14_present) {
    HANDLE_CODE(nrs_pwr_offset_non_anchor_r14.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= dl_gap_non_anchor_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(dl_gap_non_anchor_v1530.is_present(), 1));
      if (dl_gap_non_anchor_v1530.is_present()) {
        HANDLE_CODE(dl_gap_non_anchor_v1530->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_carrier_cfg_common_nb_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(inband_carrier_info_r14_present, 1));
  HANDLE_CODE(bref.unpack(nrs_pwr_offset_non_anchor_r14_present, 1));

  HANDLE_CODE(dl_carrier_freq_r14.unpack(bref));
  HANDLE_CODE(dl_bitmap_non_anchor_r14.unpack(bref));
  HANDLE_CODE(dl_gap_non_anchor_r14.unpack(bref));
  if (inband_carrier_info_r14_present) {
    HANDLE_CODE(bref.unpack(inband_carrier_info_r14.same_pci_ind_r14_present, 1));
    if (inband_carrier_info_r14.same_pci_ind_r14_present) {
      HANDLE_CODE(inband_carrier_info_r14.same_pci_ind_r14.unpack(bref));
    }
    HANDLE_CODE(inband_carrier_info_r14.eutra_ctrl_region_size_r14.unpack(bref));
  }
  if (nrs_pwr_offset_non_anchor_r14_present) {
    HANDLE_CODE(nrs_pwr_offset_non_anchor_r14.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool dl_gap_non_anchor_v1530_present;
      HANDLE_CODE(bref.unpack(dl_gap_non_anchor_v1530_present, 1));
      dl_gap_non_anchor_v1530.set_present(dl_gap_non_anchor_v1530_present);
      if (dl_gap_non_anchor_v1530.is_present()) {
        HANDLE_CODE(dl_gap_non_anchor_v1530->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void dl_carrier_cfg_common_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("dl-CarrierFreq-r14");
  dl_carrier_freq_r14.to_json(j);
  j.write_fieldname("downlinkBitmapNonAnchor-r14");
  dl_bitmap_non_anchor_r14.to_json(j);
  j.write_fieldname("dl-GapNonAnchor-r14");
  dl_gap_non_anchor_r14.to_json(j);
  if (inband_carrier_info_r14_present) {
    j.write_fieldname("inbandCarrierInfo-r14");
    j.start_obj();
    if (inband_carrier_info_r14.same_pci_ind_r14_present) {
      j.write_fieldname("samePCI-Indicator-r14");
      inband_carrier_info_r14.same_pci_ind_r14.to_json(j);
    }
    j.write_str("eutraControlRegionSize-r14", inband_carrier_info_r14.eutra_ctrl_region_size_r14.to_string());
    j.end_obj();
  }
  if (nrs_pwr_offset_non_anchor_r14_present) {
    j.write_str("nrs-PowerOffsetNonAnchor-r14", nrs_pwr_offset_non_anchor_r14.to_string());
  }
  if (ext) {
    if (dl_gap_non_anchor_v1530.is_present()) {
      j.write_fieldname("dl-GapNonAnchor-v1530");
      dl_gap_non_anchor_v1530->to_json(j);
    }
  }
  j.end_obj();
}

void dl_carrier_cfg_common_nb_r14_s::dl_bitmap_non_anchor_r14_c_::set(types::options e)
{
  type_ = e;
}
void dl_carrier_cfg_common_nb_r14_s::dl_bitmap_non_anchor_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::use_no_bitmap_r14:
      break;
    case types::use_anchor_bitmap_r14:
      break;
    case types::explicit_bitmap_cfg_r14:
      j.write_fieldname("explicitBitmapConfiguration-r14");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_common_nb_r14_s::dl_bitmap_non_anchor_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_carrier_cfg_common_nb_r14_s::dl_bitmap_non_anchor_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::use_no_bitmap_r14:
      break;
    case types::use_anchor_bitmap_r14:
      break;
    case types::explicit_bitmap_cfg_r14:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_common_nb_r14_s::dl_bitmap_non_anchor_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_carrier_cfg_common_nb_r14_s::dl_bitmap_non_anchor_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::use_no_bitmap_r14:
      break;
    case types::use_anchor_bitmap_r14:
      break;
    case types::explicit_bitmap_cfg_r14:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_common_nb_r14_s::dl_bitmap_non_anchor_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string dl_carrier_cfg_common_nb_r14_s::dl_bitmap_non_anchor_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"useNoBitmap-r14", "useAnchorBitmap-r14", "explicitBitmapConfiguration-r14"};
  return convert_enum_idx(options, 3, value, "dl_carrier_cfg_common_nb_r14_s::dl_bitmap_non_anchor_r14_c_::types");
}

void dl_carrier_cfg_common_nb_r14_s::dl_gap_non_anchor_r14_c_::set(types::options e)
{
  type_ = e;
}
void dl_carrier_cfg_common_nb_r14_s::dl_gap_non_anchor_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::use_no_gap_r14:
      break;
    case types::use_anchor_gap_cfg_r14:
      break;
    case types::explicit_gap_cfg_r14:
      j.write_fieldname("explicitGapConfiguration-r14");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_common_nb_r14_s::dl_gap_non_anchor_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_carrier_cfg_common_nb_r14_s::dl_gap_non_anchor_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::use_no_gap_r14:
      break;
    case types::use_anchor_gap_cfg_r14:
      break;
    case types::explicit_gap_cfg_r14:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_common_nb_r14_s::dl_gap_non_anchor_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_carrier_cfg_common_nb_r14_s::dl_gap_non_anchor_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::use_no_gap_r14:
      break;
    case types::use_anchor_gap_cfg_r14:
      break;
    case types::explicit_gap_cfg_r14:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_common_nb_r14_s::dl_gap_non_anchor_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string dl_carrier_cfg_common_nb_r14_s::dl_gap_non_anchor_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"useNoGap-r14", "useAnchorGapConfig-r14", "explicitGapConfiguration-r14"};
  return convert_enum_idx(options, 3, value, "dl_carrier_cfg_common_nb_r14_s::dl_gap_non_anchor_r14_c_::types");
}

void dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_::destroy_()
{
  switch (type_) {
    case types::same_pci_r14:
      c.destroy<same_pci_r14_s_>();
      break;
    case types::different_pci_r14:
      c.destroy<different_pci_r14_s_>();
      break;
    default:
      break;
  }
}
void dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::same_pci_r14:
      c.init<same_pci_r14_s_>();
      break;
    case types::different_pci_r14:
      c.init<different_pci_r14_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_");
  }
}
dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_::same_pci_ind_r14_c_(
    const dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::same_pci_r14:
      c.init(other.c.get<same_pci_r14_s_>());
      break;
    case types::different_pci_r14:
      c.init(other.c.get<different_pci_r14_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_");
  }
}
dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_&
dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_::
operator=(const dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::same_pci_r14:
      c.set(other.c.get<same_pci_r14_s_>());
      break;
    case types::different_pci_r14:
      c.set(other.c.get<different_pci_r14_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_");
  }

  return *this;
}
void dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::same_pci_r14:
      j.write_fieldname("samePCI-r14");
      j.start_obj();
      j.write_int("indexToMidPRB-r14", c.get<same_pci_r14_s_>().idx_to_mid_prb_r14);
      j.end_obj();
      break;
    case types::different_pci_r14:
      j.write_fieldname("differentPCI-r14");
      j.start_obj();
      j.write_str("eutra-NumCRS-Ports-r14", c.get<different_pci_r14_s_>().eutra_num_crs_ports_r14.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::same_pci_r14:
      HANDLE_CODE(pack_integer(bref, c.get<same_pci_r14_s_>().idx_to_mid_prb_r14, (int8_t)-55, (int8_t)54));
      break;
    case types::different_pci_r14:
      HANDLE_CODE(c.get<different_pci_r14_s_>().eutra_num_crs_ports_r14.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::same_pci_r14:
      HANDLE_CODE(unpack_integer(c.get<same_pci_r14_s_>().idx_to_mid_prb_r14, bref, (int8_t)-55, (int8_t)54));
      break;
    case types::different_pci_r14:
      HANDLE_CODE(c.get<different_pci_r14_s_>().eutra_num_crs_ports_r14.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_::different_pci_r14_s_::
    eutra_num_crs_ports_r14_opts::to_string() const
{
  static const char* options[] = {"same", "four"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_::different_"
                          "pci_r14_s_::eutra_num_crs_ports_r14_e_");
}
uint8_t dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_::different_pci_r14_s_::
    eutra_num_crs_ports_r14_opts::to_number() const
{
  if (value == four) {
    return 4;
  }
  invalid_enum_number(value,
                      "dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_::different_pci_"
                      "r14_s_::eutra_num_crs_ports_r14_e_");
  return 0;
}

std::string
dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"samePCI-r14", "differentPCI-r14"};
  return convert_enum_idx(
      options, 2, value, "dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::same_pci_ind_r14_c_::types");
}

std::string
dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::eutra_ctrl_region_size_r14_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3"};
  return convert_enum_idx(
      options, 3, value, "dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::eutra_ctrl_region_size_r14_e_");
}
uint8_t dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::eutra_ctrl_region_size_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(
      options, 3, value, "dl_carrier_cfg_common_nb_r14_s::inband_carrier_info_r14_s_::eutra_ctrl_region_size_r14_e_");
}

std::string dl_carrier_cfg_common_nb_r14_s::nrs_pwr_offset_non_anchor_r14_opts::to_string() const
{
  static const char* options[] = {"dB-12", "dB-10", "dB-8", "dB-6", "dB-4", "dB-2", "dB0", "dB3"};
  return convert_enum_idx(options, 8, value, "dl_carrier_cfg_common_nb_r14_s::nrs_pwr_offset_non_anchor_r14_e_");
}
int8_t dl_carrier_cfg_common_nb_r14_s::nrs_pwr_offset_non_anchor_r14_opts::to_number() const
{
  static const int8_t options[] = {-12, -10, -8, -6, -4, -2, 0, 3};
  return map_enum_number(options, 8, value, "dl_carrier_cfg_common_nb_r14_s::nrs_pwr_offset_non_anchor_r14_e_");
}

// PCCH-Config-NB-r14 ::= SEQUENCE
SRSASN_CODE pcch_cfg_nb_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(npdcch_num_repeat_paging_r14_present, 1));
  HANDLE_CODE(bref.pack(paging_weight_r14_present, 1));

  if (npdcch_num_repeat_paging_r14_present) {
    HANDLE_CODE(npdcch_num_repeat_paging_r14.pack(bref));
  }
  if (paging_weight_r14_present) {
    HANDLE_CODE(paging_weight_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_cfg_nb_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(npdcch_num_repeat_paging_r14_present, 1));
  HANDLE_CODE(bref.unpack(paging_weight_r14_present, 1));

  if (npdcch_num_repeat_paging_r14_present) {
    HANDLE_CODE(npdcch_num_repeat_paging_r14.unpack(bref));
  }
  if (paging_weight_r14_present) {
    HANDLE_CODE(paging_weight_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pcch_cfg_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (npdcch_num_repeat_paging_r14_present) {
    j.write_str("npdcch-NumRepetitionPaging-r14", npdcch_num_repeat_paging_r14.to_string());
  }
  if (paging_weight_r14_present) {
    j.write_str("pagingWeight-r14", paging_weight_r14.to_string());
  }
  j.end_obj();
}

std::string pcch_cfg_nb_r14_s::npdcch_num_repeat_paging_r14_opts::to_string() const
{
  static const char* options[] = {"r1",
                                  "r2",
                                  "r4",
                                  "r8",
                                  "r16",
                                  "r32",
                                  "r64",
                                  "r128",
                                  "r256",
                                  "r512",
                                  "r1024",
                                  "r2048",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pcch_cfg_nb_r14_s::npdcch_num_repeat_paging_r14_e_");
}
uint16_t pcch_cfg_nb_r14_s::npdcch_num_repeat_paging_r14_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
  return map_enum_number(options, 12, value, "pcch_cfg_nb_r14_s::npdcch_num_repeat_paging_r14_e_");
}

// PowerRampingParameters-NB-v1450 ::= SEQUENCE
SRSASN_CODE pwr_ramp_params_nb_v1450_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(preamb_init_rx_target_pwr_v1450_present, 1));
  HANDLE_CODE(bref.pack(pwr_ramp_params_ce1_r14_present, 1));

  if (preamb_init_rx_target_pwr_v1450_present) {
    HANDLE_CODE(preamb_init_rx_target_pwr_v1450.pack(bref));
  }
  if (pwr_ramp_params_ce1_r14_present) {
    HANDLE_CODE(pwr_ramp_params_ce1_r14.pwr_ramp_step_ce1_r14.pack(bref));
    HANDLE_CODE(pwr_ramp_params_ce1_r14.preamb_init_rx_target_pwr_ce1_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pwr_ramp_params_nb_v1450_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(preamb_init_rx_target_pwr_v1450_present, 1));
  HANDLE_CODE(bref.unpack(pwr_ramp_params_ce1_r14_present, 1));

  if (preamb_init_rx_target_pwr_v1450_present) {
    HANDLE_CODE(preamb_init_rx_target_pwr_v1450.unpack(bref));
  }
  if (pwr_ramp_params_ce1_r14_present) {
    HANDLE_CODE(pwr_ramp_params_ce1_r14.pwr_ramp_step_ce1_r14.unpack(bref));
    HANDLE_CODE(pwr_ramp_params_ce1_r14.preamb_init_rx_target_pwr_ce1_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pwr_ramp_params_nb_v1450_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (preamb_init_rx_target_pwr_v1450_present) {
    j.write_str("preambleInitialReceivedTargetPower-v1450", preamb_init_rx_target_pwr_v1450.to_string());
  }
  if (pwr_ramp_params_ce1_r14_present) {
    j.write_fieldname("powerRampingParametersCE1-r14");
    j.start_obj();
    j.write_str("powerRampingStepCE1-r14", pwr_ramp_params_ce1_r14.pwr_ramp_step_ce1_r14.to_string());
    j.write_str("preambleInitialReceivedTargetPowerCE1-r14",
                pwr_ramp_params_ce1_r14.preamb_init_rx_target_pwr_ce1_r14.to_string());
    j.end_obj();
  }
  j.end_obj();
}

std::string pwr_ramp_params_nb_v1450_s::preamb_init_rx_target_pwr_v1450_opts::to_string() const
{
  static const char* options[] = {
      "dBm-130", "dBm-128", "dBm-126", "dBm-124", "dBm-122", "dBm-88", "dBm-86", "dBm-84", "dBm-82", "dBm-80"};
  return convert_enum_idx(options, 10, value, "pwr_ramp_params_nb_v1450_s::preamb_init_rx_target_pwr_v1450_e_");
}
int16_t pwr_ramp_params_nb_v1450_s::preamb_init_rx_target_pwr_v1450_opts::to_number() const
{
  static const int16_t options[] = {-130, -128, -126, -124, -122, -88, -86, -84, -82, -80};
  return map_enum_number(options, 10, value, "pwr_ramp_params_nb_v1450_s::preamb_init_rx_target_pwr_v1450_e_");
}

std::string pwr_ramp_params_nb_v1450_s::pwr_ramp_params_ce1_r14_s_::pwr_ramp_step_ce1_r14_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB2", "dB4", "dB6"};
  return convert_enum_idx(
      options, 4, value, "pwr_ramp_params_nb_v1450_s::pwr_ramp_params_ce1_r14_s_::pwr_ramp_step_ce1_r14_e_");
}
uint8_t pwr_ramp_params_nb_v1450_s::pwr_ramp_params_ce1_r14_s_::pwr_ramp_step_ce1_r14_opts::to_number() const
{
  static const uint8_t options[] = {0, 2, 4, 6};
  return map_enum_number(
      options, 4, value, "pwr_ramp_params_nb_v1450_s::pwr_ramp_params_ce1_r14_s_::pwr_ramp_step_ce1_r14_e_");
}

std::string
pwr_ramp_params_nb_v1450_s::pwr_ramp_params_ce1_r14_s_::preamb_init_rx_target_pwr_ce1_r14_opts::to_string() const
{
  static const char* options[] = {"dBm-130", "dBm-128", "dBm-126", "dBm-124", "dBm-122", "dBm-120", "dBm-118",
                                  "dBm-116", "dBm-114", "dBm-112", "dBm-110", "dBm-108", "dBm-106", "dBm-104",
                                  "dBm-102", "dBm-100", "dBm-98",  "dBm-96",  "dBm-94",  "dBm-92",  "dBm-90",
                                  "dBm-88",  "dBm-86",  "dBm-84",  "dBm-82",  "dBm-80"};
  return convert_enum_idx(
      options,
      26,
      value,
      "pwr_ramp_params_nb_v1450_s::pwr_ramp_params_ce1_r14_s_::preamb_init_rx_target_pwr_ce1_r14_e_");
}
int16_t
pwr_ramp_params_nb_v1450_s::pwr_ramp_params_ce1_r14_s_::preamb_init_rx_target_pwr_ce1_r14_opts::to_number() const
{
  static const int16_t options[] = {-130, -128, -126, -124, -122, -120, -118, -116, -114, -112, -110, -108, -106,
                                    -104, -102, -100, -98,  -96,  -94,  -92,  -90,  -88,  -86,  -84,  -82,  -80};
  return map_enum_number(
      options,
      26,
      value,
      "pwr_ramp_params_nb_v1450_s::pwr_ramp_params_ce1_r14_s_::preamb_init_rx_target_pwr_ce1_r14_e_");
}

// TDD-Config-NB-r15 ::= SEQUENCE
SRSASN_CODE tdd_cfg_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sf_assign_r15.pack(bref));
  HANDLE_CODE(special_sf_patterns_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_cfg_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sf_assign_r15.unpack(bref));
  HANDLE_CODE(special_sf_patterns_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void tdd_cfg_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("subframeAssignment-r15", sf_assign_r15.to_string());
  j.write_str("specialSubframePatterns-r15", special_sf_patterns_r15.to_string());
  j.end_obj();
}

std::string tdd_cfg_nb_r15_s::sf_assign_r15_opts::to_string() const
{
  static const char* options[] = {"sa1", "sa2", "sa3", "sa4", "sa5"};
  return convert_enum_idx(options, 5, value, "tdd_cfg_nb_r15_s::sf_assign_r15_e_");
}
uint8_t tdd_cfg_nb_r15_s::sf_assign_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5};
  return map_enum_number(options, 5, value, "tdd_cfg_nb_r15_s::sf_assign_r15_e_");
}

std::string tdd_cfg_nb_r15_s::special_sf_patterns_r15_opts::to_string() const
{
  static const char* options[] = {
      "ssp0", "ssp1", "ssp2", "ssp3", "ssp4", "ssp5", "ssp6", "ssp7", "ssp8", "ssp9", "ssp10", "ssp10-CRS-LessDwPTS"};
  return convert_enum_idx(options, 12, value, "tdd_cfg_nb_r15_s::special_sf_patterns_r15_e_");
}

// UL-ReferenceSignalsNPUSCH-NB-r13 ::= SEQUENCE
SRSASN_CODE ul_ref_sigs_npusch_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(group_hop_enabled_r13, 1));
  HANDLE_CODE(pack_integer(bref, group_assign_npusch_r13, (uint8_t)0u, (uint8_t)29u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ref_sigs_npusch_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(group_hop_enabled_r13, 1));
  HANDLE_CODE(unpack_integer(group_assign_npusch_r13, bref, (uint8_t)0u, (uint8_t)29u));

  return SRSASN_SUCCESS;
}
void ul_ref_sigs_npusch_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("groupHoppingEnabled-r13", group_hop_enabled_r13);
  j.write_int("groupAssignmentNPUSCH-r13", group_assign_npusch_r13);
  j.end_obj();
}

// WUS-ConfigPerCarrier-NB-r15 ::= SEQUENCE
SRSASN_CODE wus_cfg_per_carrier_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(max_dur_factor_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE wus_cfg_per_carrier_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(max_dur_factor_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void wus_cfg_per_carrier_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxDurationFactor-r15", max_dur_factor_r15.to_string());
  j.end_obj();
}

// BCCH-Config-NB-r13 ::= SEQUENCE
SRSASN_CODE bcch_cfg_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(mod_period_coeff_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_cfg_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(mod_period_coeff_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void bcch_cfg_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("modificationPeriodCoeff-r13", mod_period_coeff_r13.to_string());
  j.end_obj();
}

std::string bcch_cfg_nb_r13_s::mod_period_coeff_r13_opts::to_string() const
{
  static const char* options[] = {"n16", "n32", "n64", "n128"};
  return convert_enum_idx(options, 4, value, "bcch_cfg_nb_r13_s::mod_period_coeff_r13_e_");
}
uint8_t bcch_cfg_nb_r13_s::mod_period_coeff_r13_opts::to_number() const
{
  static const uint8_t options[] = {16, 32, 64, 128};
  return map_enum_number(options, 4, value, "bcch_cfg_nb_r13_s::mod_period_coeff_r13_e_");
}

// DL-ConfigCommon-NB-r14 ::= SEQUENCE
SRSASN_CODE dl_cfg_common_nb_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pcch_cfg_r14_present, 1));

  HANDLE_CODE(dl_carrier_cfg_r14.pack(bref));
  if (pcch_cfg_r14_present) {
    HANDLE_CODE(pcch_cfg_r14.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= wus_cfg_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(wus_cfg_r15.is_present(), 1));
      if (wus_cfg_r15.is_present()) {
        HANDLE_CODE(wus_cfg_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_cfg_common_nb_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pcch_cfg_r14_present, 1));

  HANDLE_CODE(dl_carrier_cfg_r14.unpack(bref));
  if (pcch_cfg_r14_present) {
    HANDLE_CODE(pcch_cfg_r14.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool wus_cfg_r15_present;
      HANDLE_CODE(bref.unpack(wus_cfg_r15_present, 1));
      wus_cfg_r15.set_present(wus_cfg_r15_present);
      if (wus_cfg_r15.is_present()) {
        HANDLE_CODE(wus_cfg_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void dl_cfg_common_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("dl-CarrierConfig-r14");
  dl_carrier_cfg_r14.to_json(j);
  if (pcch_cfg_r14_present) {
    j.write_fieldname("pcch-Config-r14");
    pcch_cfg_r14.to_json(j);
  }
  if (ext) {
    if (wus_cfg_r15.is_present()) {
      j.write_fieldname("wus-Config-r15");
      wus_cfg_r15->to_json(j);
    }
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-NB-r13 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(q_qual_min_r13_present, 1));
  HANDLE_CODE(bref.pack(p_max_r13_present, 1));
  HANDLE_CODE(bref.pack(q_offset_freq_r13_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_neigh_cell_list_r13_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_black_cell_list_r13_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_r13_present, 1));

  HANDLE_CODE(dl_carrier_freq_r13.pack(bref));
  HANDLE_CODE(pack_integer(bref, q_rx_lev_min_r13, (int8_t)-70, (int8_t)-22));
  if (q_qual_min_r13_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_r13, (int8_t)-34, (int8_t)-3));
  }
  if (p_max_r13_present) {
    HANDLE_CODE(pack_integer(bref, p_max_r13, (int8_t)-30, (int8_t)33));
  }
  if (q_offset_freq_r13_present) {
    HANDLE_CODE(q_offset_freq_r13.pack(bref));
  }
  if (inter_freq_neigh_cell_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_neigh_cell_list_r13, 1, 16, integer_packer<uint16_t>(0, 503)));
  }
  if (inter_freq_black_cell_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_black_cell_list_r13, 1, 16, integer_packer<uint16_t>(0, 503)));
  }
  if (multi_band_info_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_r13, 1, 8));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= delta_rx_lev_min_v1350_present;
    group_flags[1] |= pwr_class14dbm_offset_r14_present;
    group_flags[1] |= ce_authorisation_offset_r14_present;
    group_flags[2] |= nsss_rrm_cfg_r15.is_present();
    group_flags[2] |= inter_freq_neigh_cell_list_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(delta_rx_lev_min_v1350_present, 1));
      if (delta_rx_lev_min_v1350_present) {
        HANDLE_CODE(pack_integer(bref, delta_rx_lev_min_v1350, (int8_t)-8, (int8_t)-1));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pwr_class14dbm_offset_r14_present, 1));
      HANDLE_CODE(bref.pack(ce_authorisation_offset_r14_present, 1));
      if (pwr_class14dbm_offset_r14_present) {
        HANDLE_CODE(pwr_class14dbm_offset_r14.pack(bref));
      }
      if (ce_authorisation_offset_r14_present) {
        HANDLE_CODE(ce_authorisation_offset_r14.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(nsss_rrm_cfg_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(inter_freq_neigh_cell_list_v1530.is_present(), 1));
      if (nsss_rrm_cfg_r15.is_present()) {
        HANDLE_CODE(nsss_rrm_cfg_r15->pack(bref));
      }
      if (inter_freq_neigh_cell_list_v1530.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_neigh_cell_list_v1530, 1, 16));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(q_qual_min_r13_present, 1));
  HANDLE_CODE(bref.unpack(p_max_r13_present, 1));
  HANDLE_CODE(bref.unpack(q_offset_freq_r13_present, 1));
  HANDLE_CODE(bref.unpack(inter_freq_neigh_cell_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(inter_freq_black_cell_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_r13_present, 1));

  HANDLE_CODE(dl_carrier_freq_r13.unpack(bref));
  HANDLE_CODE(unpack_integer(q_rx_lev_min_r13, bref, (int8_t)-70, (int8_t)-22));
  if (q_qual_min_r13_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_r13, bref, (int8_t)-34, (int8_t)-3));
  }
  if (p_max_r13_present) {
    HANDLE_CODE(unpack_integer(p_max_r13, bref, (int8_t)-30, (int8_t)33));
  }
  if (q_offset_freq_r13_present) {
    HANDLE_CODE(q_offset_freq_r13.unpack(bref));
  }
  if (inter_freq_neigh_cell_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_neigh_cell_list_r13, bref, 1, 16, integer_packer<uint16_t>(0, 503)));
  }
  if (inter_freq_black_cell_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_black_cell_list_r13, bref, 1, 16, integer_packer<uint16_t>(0, 503)));
  }
  if (multi_band_info_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_r13, bref, 1, 8));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(delta_rx_lev_min_v1350_present, 1));
      if (delta_rx_lev_min_v1350_present) {
        HANDLE_CODE(unpack_integer(delta_rx_lev_min_v1350, bref, (int8_t)-8, (int8_t)-1));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(pwr_class14dbm_offset_r14_present, 1));
      HANDLE_CODE(bref.unpack(ce_authorisation_offset_r14_present, 1));
      if (pwr_class14dbm_offset_r14_present) {
        HANDLE_CODE(pwr_class14dbm_offset_r14.unpack(bref));
      }
      if (ce_authorisation_offset_r14_present) {
        HANDLE_CODE(ce_authorisation_offset_r14.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool nsss_rrm_cfg_r15_present;
      HANDLE_CODE(bref.unpack(nsss_rrm_cfg_r15_present, 1));
      nsss_rrm_cfg_r15.set_present(nsss_rrm_cfg_r15_present);
      bool inter_freq_neigh_cell_list_v1530_present;
      HANDLE_CODE(bref.unpack(inter_freq_neigh_cell_list_v1530_present, 1));
      inter_freq_neigh_cell_list_v1530.set_present(inter_freq_neigh_cell_list_v1530_present);
      if (nsss_rrm_cfg_r15.is_present()) {
        HANDLE_CODE(nsss_rrm_cfg_r15->unpack(bref));
      }
      if (inter_freq_neigh_cell_list_v1530.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_neigh_cell_list_v1530, bref, 1, 16));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("dl-CarrierFreq-r13");
  dl_carrier_freq_r13.to_json(j);
  j.write_int("q-RxLevMin-r13", q_rx_lev_min_r13);
  if (q_qual_min_r13_present) {
    j.write_int("q-QualMin-r13", q_qual_min_r13);
  }
  if (p_max_r13_present) {
    j.write_int("p-Max-r13", p_max_r13);
  }
  if (q_offset_freq_r13_present) {
    j.write_str("q-OffsetFreq-r13", q_offset_freq_r13.to_string());
  }
  if (inter_freq_neigh_cell_list_r13_present) {
    j.start_array("interFreqNeighCellList-r13");
    for (const auto& e1 : inter_freq_neigh_cell_list_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (inter_freq_black_cell_list_r13_present) {
    j.start_array("interFreqBlackCellList-r13");
    for (const auto& e1 : inter_freq_black_cell_list_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (multi_band_info_list_r13_present) {
    j.start_array("multiBandInfoList-r13");
    for (const auto& e1 : multi_band_info_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (delta_rx_lev_min_v1350_present) {
      j.write_int("delta-RxLevMin-v1350", delta_rx_lev_min_v1350);
    }
    if (pwr_class14dbm_offset_r14_present) {
      j.write_str("powerClass14dBm-Offset-r14", pwr_class14dbm_offset_r14.to_string());
    }
    if (ce_authorisation_offset_r14_present) {
      j.write_str("ce-AuthorisationOffset-r14", ce_authorisation_offset_r14.to_string());
    }
    if (nsss_rrm_cfg_r15.is_present()) {
      j.write_fieldname("nsss-RRM-Config-r15");
      nsss_rrm_cfg_r15->to_json(j);
    }
    if (inter_freq_neigh_cell_list_v1530.is_present()) {
      j.start_array("interFreqNeighCellList-v1530");
      for (const auto& e1 : *inter_freq_neigh_cell_list_v1530) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

std::string inter_freq_carrier_freq_info_nb_r13_s::pwr_class14dbm_offset_r14_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-3", "dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(options, 6, value, "inter_freq_carrier_freq_info_nb_r13_s::pwr_class14dbm_offset_r14_e_");
}
int8_t inter_freq_carrier_freq_info_nb_r13_s::pwr_class14dbm_offset_r14_opts::to_number() const
{
  static const int8_t options[] = {-6, -3, 3, 6, 9, 12};
  return map_enum_number(options, 6, value, "inter_freq_carrier_freq_info_nb_r13_s::pwr_class14dbm_offset_r14_e_");
}

std::string inter_freq_carrier_freq_info_nb_r13_s::ce_authorisation_offset_r14_opts::to_string() const
{
  static const char* options[] = {"dB5", "dB10", "dB15", "dB20", "dB25", "dB30", "dB35"};
  return convert_enum_idx(options, 7, value, "inter_freq_carrier_freq_info_nb_r13_s::ce_authorisation_offset_r14_e_");
}
uint8_t inter_freq_carrier_freq_info_nb_r13_s::ce_authorisation_offset_r14_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 15, 20, 25, 30, 35};
  return map_enum_number(options, 7, value, "inter_freq_carrier_freq_info_nb_r13_s::ce_authorisation_offset_r14_e_");
}

// IntraFreqNeighCellInfo-NB-v1530 ::= SEQUENCE
SRSASN_CODE intra_freq_neigh_cell_info_nb_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nsss_rrm_cfg_r15_present, 1));

  if (nsss_rrm_cfg_r15_present) {
    HANDLE_CODE(nsss_rrm_cfg_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_freq_neigh_cell_info_nb_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nsss_rrm_cfg_r15_present, 1));

  if (nsss_rrm_cfg_r15_present) {
    HANDLE_CODE(nsss_rrm_cfg_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void intra_freq_neigh_cell_info_nb_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nsss_rrm_cfg_r15_present) {
    j.write_fieldname("nsss-RRM-Config-r15");
    nsss_rrm_cfg_r15.to_json(j);
  }
  j.end_obj();
}

// MBMS-SAI-InterFreq-NB-r14 ::= SEQUENCE
SRSASN_CODE mbms_sai_inter_freq_nb_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(multi_band_info_list_r14_present, 1));

  HANDLE_CODE(dl_carrier_freq_r14.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, mbms_sai_list_r14, 1, 64, integer_packer<uint32_t>(0, 65535)));
  if (multi_band_info_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_r14, 1, 8, integer_packer<uint16_t>(1, 256)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_sai_inter_freq_nb_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(multi_band_info_list_r14_present, 1));

  HANDLE_CODE(dl_carrier_freq_r14.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(mbms_sai_list_r14, bref, 1, 64, integer_packer<uint32_t>(0, 65535)));
  if (multi_band_info_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_r14, bref, 1, 8, integer_packer<uint16_t>(1, 256)));
  }

  return SRSASN_SUCCESS;
}
void mbms_sai_inter_freq_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("dl-CarrierFreq-r14");
  dl_carrier_freq_r14.to_json(j);
  j.start_array("mbms-SAI-List-r14");
  for (const auto& e1 : mbms_sai_list_r14) {
    j.write_int(e1);
  }
  j.end_array();
  if (multi_band_info_list_r14_present) {
    j.start_array("multiBandInfoList-r14");
    for (const auto& e1 : multi_band_info_list_r14) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// NPDSCH-ConfigCommon-NB-r13 ::= SEQUENCE
SRSASN_CODE npdsch_cfg_common_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, nrs_pwr_r13, (int8_t)-60, (int8_t)50));

  return SRSASN_SUCCESS;
}
SRSASN_CODE npdsch_cfg_common_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(nrs_pwr_r13, bref, (int8_t)-60, (int8_t)50));

  return SRSASN_SUCCESS;
}
void npdsch_cfg_common_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("nrs-Power-r13", nrs_pwr_r13);
  j.end_obj();
}

// NPRACH-ConfigSIB-NB-r13 ::= SEQUENCE
SRSASN_CODE nprach_cfg_sib_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrp_thress_prach_info_list_r13_present, 1));

  HANDLE_CODE(nprach_cp_len_r13.pack(bref));
  if (rsrp_thress_prach_info_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rsrp_thress_prach_info_list_r13, 1, 2, integer_packer<uint8_t>(0, 97)));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, nprach_params_list_r13, 1, 3));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nprach_cfg_sib_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrp_thress_prach_info_list_r13_present, 1));

  HANDLE_CODE(nprach_cp_len_r13.unpack(bref));
  if (rsrp_thress_prach_info_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rsrp_thress_prach_info_list_r13, bref, 1, 2, integer_packer<uint8_t>(0, 97)));
  }
  HANDLE_CODE(unpack_dyn_seq_of(nprach_params_list_r13, bref, 1, 3));

  return SRSASN_SUCCESS;
}
void nprach_cfg_sib_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("nprach-CP-Length-r13", nprach_cp_len_r13.to_string());
  if (rsrp_thress_prach_info_list_r13_present) {
    j.start_array("rsrp-ThresholdsPrachInfoList-r13");
    for (const auto& e1 : rsrp_thress_prach_info_list_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.start_array("nprach-ParametersList-r13");
  for (const auto& e1 : nprach_params_list_r13) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

std::string nprach_cfg_sib_nb_r13_s::nprach_cp_len_r13_opts::to_string() const
{
  static const char* options[] = {"us66dot7", "us266dot7"};
  return convert_enum_idx(options, 2, value, "nprach_cfg_sib_nb_r13_s::nprach_cp_len_r13_e_");
}
float nprach_cfg_sib_nb_r13_s::nprach_cp_len_r13_opts::to_number() const
{
  static const float options[] = {66.7, 266.7};
  return map_enum_number(options, 2, value, "nprach_cfg_sib_nb_r13_s::nprach_cp_len_r13_e_");
}
std::string nprach_cfg_sib_nb_r13_s::nprach_cp_len_r13_opts::to_number_string() const
{
  static const char* options[] = {"66.7", "266.7"};
  return convert_enum_idx(options, 2, value, "nprach_cfg_sib_nb_r13_s::nprach_cp_len_r13_e_");
}

// NPRACH-ConfigSIB-NB-v1330 ::= SEQUENCE
SRSASN_CODE nprach_cfg_sib_nb_v1330_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, nprach_params_list_v1330, 1, 3));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nprach_cfg_sib_nb_v1330_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(nprach_params_list_v1330, bref, 1, 3));

  return SRSASN_SUCCESS;
}
void nprach_cfg_sib_nb_v1330_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("nprach-ParametersList-v1330");
  for (const auto& e1 : nprach_params_list_v1330) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// NPRACH-ConfigSIB-NB-v1450 ::= SEQUENCE
SRSASN_CODE nprach_cfg_sib_nb_v1450_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(max_num_preamb_attempt_ce_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nprach_cfg_sib_nb_v1450_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(max_num_preamb_attempt_ce_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void nprach_cfg_sib_nb_v1450_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxNumPreambleAttemptCE-r14", max_num_preamb_attempt_ce_r14.to_string());
  j.end_obj();
}

std::string nprach_cfg_sib_nb_v1450_s::max_num_preamb_attempt_ce_r14_opts::to_string() const
{
  static const char* options[] = {"n3", "n4", "n5", "n6", "n7", "n8", "n10", "spare1"};
  return convert_enum_idx(options, 8, value, "nprach_cfg_sib_nb_v1450_s::max_num_preamb_attempt_ce_r14_e_");
}
uint8_t nprach_cfg_sib_nb_v1450_s::max_num_preamb_attempt_ce_r14_opts::to_number() const
{
  static const uint8_t options[] = {3, 4, 5, 6, 7, 8, 10};
  return map_enum_number(options, 7, value, "nprach_cfg_sib_nb_v1450_s::max_num_preamb_attempt_ce_r14_e_");
}

// NPRACH-ConfigSIB-NB-v1530 ::= SEQUENCE
SRSASN_CODE nprach_cfg_sib_nb_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tdd_params_r15_present, 1));
  HANDLE_CODE(bref.pack(fmt2_params_r15_present, 1));
  HANDLE_CODE(bref.pack(edt_params_r15_present, 1));

  if (tdd_params_r15_present) {
    HANDLE_CODE(tdd_params_r15.nprach_preamb_format_r15.pack(bref));
    HANDLE_CODE(tdd_params_r15.num_repeats_per_preamb_attempt_r15.pack(bref));
    HANDLE_CODE(pack_dyn_seq_of(bref, tdd_params_r15.nprach_params_list_tdd_r15, 1, 3));
  }
  if (fmt2_params_r15_present) {
    HANDLE_CODE(bref.pack(fmt2_params_r15.nprach_params_list_fmt2_r15_present, 1));
    HANDLE_CODE(bref.pack(fmt2_params_r15.nprach_params_list_fmt2_edt_r15_present, 1));
    if (fmt2_params_r15.nprach_params_list_fmt2_r15_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, fmt2_params_r15.nprach_params_list_fmt2_r15, 1, 3));
    }
    if (fmt2_params_r15.nprach_params_list_fmt2_edt_r15_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, fmt2_params_r15.nprach_params_list_fmt2_edt_r15, 1, 3));
    }
  }
  if (edt_params_r15_present) {
    HANDLE_CODE(bref.pack(edt_params_r15.edt_small_tbs_subset_r15_present, 1));
    HANDLE_CODE(bref.pack(edt_params_r15.nprach_params_list_edt_r15_present, 1));
    HANDLE_CODE(pack_dyn_seq_of(bref, edt_params_r15.edt_tbs_info_list_r15, 1, 3));
    if (edt_params_r15.nprach_params_list_edt_r15_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, edt_params_r15.nprach_params_list_edt_r15, 1, 3));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nprach_cfg_sib_nb_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tdd_params_r15_present, 1));
  HANDLE_CODE(bref.unpack(fmt2_params_r15_present, 1));
  HANDLE_CODE(bref.unpack(edt_params_r15_present, 1));

  if (tdd_params_r15_present) {
    HANDLE_CODE(tdd_params_r15.nprach_preamb_format_r15.unpack(bref));
    HANDLE_CODE(tdd_params_r15.num_repeats_per_preamb_attempt_r15.unpack(bref));
    HANDLE_CODE(unpack_dyn_seq_of(tdd_params_r15.nprach_params_list_tdd_r15, bref, 1, 3));
  }
  if (fmt2_params_r15_present) {
    HANDLE_CODE(bref.unpack(fmt2_params_r15.nprach_params_list_fmt2_r15_present, 1));
    HANDLE_CODE(bref.unpack(fmt2_params_r15.nprach_params_list_fmt2_edt_r15_present, 1));
    if (fmt2_params_r15.nprach_params_list_fmt2_r15_present) {
      HANDLE_CODE(unpack_dyn_seq_of(fmt2_params_r15.nprach_params_list_fmt2_r15, bref, 1, 3));
    }
    if (fmt2_params_r15.nprach_params_list_fmt2_edt_r15_present) {
      HANDLE_CODE(unpack_dyn_seq_of(fmt2_params_r15.nprach_params_list_fmt2_edt_r15, bref, 1, 3));
    }
  }
  if (edt_params_r15_present) {
    HANDLE_CODE(bref.unpack(edt_params_r15.edt_small_tbs_subset_r15_present, 1));
    HANDLE_CODE(bref.unpack(edt_params_r15.nprach_params_list_edt_r15_present, 1));
    HANDLE_CODE(unpack_dyn_seq_of(edt_params_r15.edt_tbs_info_list_r15, bref, 1, 3));
    if (edt_params_r15.nprach_params_list_edt_r15_present) {
      HANDLE_CODE(unpack_dyn_seq_of(edt_params_r15.nprach_params_list_edt_r15, bref, 1, 3));
    }
  }

  return SRSASN_SUCCESS;
}
void nprach_cfg_sib_nb_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tdd_params_r15_present) {
    j.write_fieldname("tdd-Parameters-r15");
    j.start_obj();
    j.write_str("nprach-PreambleFormat-r15", tdd_params_r15.nprach_preamb_format_r15.to_string());
    j.write_str("numRepetitionsPerPreambleAttempt-r15", tdd_params_r15.num_repeats_per_preamb_attempt_r15.to_string());
    j.start_array("nprach-ParametersListTDD-r15");
    for (const auto& e1 : tdd_params_r15.nprach_params_list_tdd_r15) {
      e1.to_json(j);
    }
    j.end_array();
    j.end_obj();
  }
  if (fmt2_params_r15_present) {
    j.write_fieldname("fmt2-Parameters-r15");
    j.start_obj();
    if (fmt2_params_r15.nprach_params_list_fmt2_r15_present) {
      j.start_array("nprach-ParametersListFmt2-r15");
      for (const auto& e1 : fmt2_params_r15.nprach_params_list_fmt2_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (fmt2_params_r15.nprach_params_list_fmt2_edt_r15_present) {
      j.start_array("nprach-ParametersListFmt2EDT-r15");
      for (const auto& e1 : fmt2_params_r15.nprach_params_list_fmt2_edt_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  if (edt_params_r15_present) {
    j.write_fieldname("edt-Parameters-r15");
    j.start_obj();
    if (edt_params_r15.edt_small_tbs_subset_r15_present) {
      j.write_str("edt-SmallTBS-Subset-r15", "true");
    }
    j.start_array("edt-TBS-InfoList-r15");
    for (const auto& e1 : edt_params_r15.edt_tbs_info_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
    if (edt_params_r15.nprach_params_list_edt_r15_present) {
      j.start_array("nprach-ParametersListEDT-r15");
      for (const auto& e1 : edt_params_r15.nprach_params_list_edt_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  j.end_obj();
}

std::string nprach_cfg_sib_nb_v1530_s::tdd_params_r15_s_::nprach_preamb_format_r15_opts::to_string() const
{
  static const char* options[] = {"fmt0", "fmt1", "fmt2", "fmt0-a", "fmt1-a"};
  return convert_enum_idx(
      options, 5, value, "nprach_cfg_sib_nb_v1530_s::tdd_params_r15_s_::nprach_preamb_format_r15_e_");
}

std::string nprach_cfg_sib_nb_v1530_s::tdd_params_r15_s_::num_repeats_per_preamb_attempt_r15_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "n8", "n16", "n32", "n64", "n128", "n256", "n512", "n1024"};
  return convert_enum_idx(
      options, 11, value, "nprach_cfg_sib_nb_v1530_s::tdd_params_r15_s_::num_repeats_per_preamb_attempt_r15_e_");
}
uint16_t nprach_cfg_sib_nb_v1530_s::tdd_params_r15_s_::num_repeats_per_preamb_attempt_r15_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
  return map_enum_number(
      options, 11, value, "nprach_cfg_sib_nb_v1530_s::tdd_params_r15_s_::num_repeats_per_preamb_attempt_r15_e_");
}

// NPRACH-ProbabilityAnchor-NB-r14 ::= SEQUENCE
SRSASN_CODE nprach_probability_anchor_nb_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nprach_probability_anchor_r14_present, 1));

  if (nprach_probability_anchor_r14_present) {
    HANDLE_CODE(nprach_probability_anchor_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nprach_probability_anchor_nb_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nprach_probability_anchor_r14_present, 1));

  if (nprach_probability_anchor_r14_present) {
    HANDLE_CODE(nprach_probability_anchor_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void nprach_probability_anchor_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nprach_probability_anchor_r14_present) {
    j.write_str("nprach-ProbabilityAnchor-r14", nprach_probability_anchor_r14.to_string());
  }
  j.end_obj();
}

std::string nprach_probability_anchor_nb_r14_s::nprach_probability_anchor_r14_opts::to_string() const
{
  static const char* options[] = {"zero",
                                  "oneSixteenth",
                                  "oneFifteenth",
                                  "oneFourteenth",
                                  "oneThirteenth",
                                  "oneTwelfth",
                                  "oneEleventh",
                                  "oneTenth",
                                  "oneNinth",
                                  "oneEighth",
                                  "oneSeventh",
                                  "oneSixth",
                                  "oneFifth",
                                  "oneFourth",
                                  "oneThird",
                                  "oneHalf"};
  return convert_enum_idx(options, 16, value, "nprach_probability_anchor_nb_r14_s::nprach_probability_anchor_r14_e_");
}
float nprach_probability_anchor_nb_r14_s::nprach_probability_anchor_r14_opts::to_number() const
{
  static const float options[] = {0.0,
                                  0.0625,
                                  0.06666666666666667,
                                  0.07142857142857142,
                                  0.07692307692307693,
                                  0.08333333333333333,
                                  0.09090909090909091,
                                  0.1,
                                  0.1111111111111111,
                                  0.125,
                                  0.14285714285714285,
                                  0.16666666666666666,
                                  0.2,
                                  0.25,
                                  0.3333333333333333,
                                  0.5};
  return map_enum_number(options, 16, value, "nprach_probability_anchor_nb_r14_s::nprach_probability_anchor_r14_e_");
}
std::string nprach_probability_anchor_nb_r14_s::nprach_probability_anchor_r14_opts::to_number_string() const
{
  static const char* options[] = {"0",
                                  "1/16",
                                  "1/15",
                                  "1/14",
                                  "1/13",
                                  "1/12",
                                  "1/11",
                                  "1/10",
                                  "1/9",
                                  "1/8",
                                  "1/7",
                                  "1/6",
                                  "1/5",
                                  "1/4",
                                  "1/3",
                                  "1/2"};
  return convert_enum_idx(options, 16, value, "nprach_probability_anchor_nb_r14_s::nprach_probability_anchor_r14_e_");
}

// NPUSCH-ConfigCommon-NB-r13 ::= SEQUENCE
SRSASN_CODE npusch_cfg_common_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_sf_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(dmrs_cfg_r13_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, ack_nack_num_repeats_msg4_r13, 1, 3));
  if (srs_sf_cfg_r13_present) {
    HANDLE_CODE(srs_sf_cfg_r13.pack(bref));
  }
  if (dmrs_cfg_r13_present) {
    HANDLE_CODE(bref.pack(dmrs_cfg_r13.three_tone_base_seq_r13_present, 1));
    HANDLE_CODE(bref.pack(dmrs_cfg_r13.six_tone_base_seq_r13_present, 1));
    HANDLE_CODE(bref.pack(dmrs_cfg_r13.twelve_tone_base_seq_r13_present, 1));
    if (dmrs_cfg_r13.three_tone_base_seq_r13_present) {
      HANDLE_CODE(pack_integer(bref, dmrs_cfg_r13.three_tone_base_seq_r13, (uint8_t)0u, (uint8_t)12u));
    }
    HANDLE_CODE(pack_integer(bref, dmrs_cfg_r13.three_tone_cyclic_shift_r13, (uint8_t)0u, (uint8_t)2u));
    if (dmrs_cfg_r13.six_tone_base_seq_r13_present) {
      HANDLE_CODE(pack_integer(bref, dmrs_cfg_r13.six_tone_base_seq_r13, (uint8_t)0u, (uint8_t)14u));
    }
    HANDLE_CODE(pack_integer(bref, dmrs_cfg_r13.six_tone_cyclic_shift_r13, (uint8_t)0u, (uint8_t)3u));
    if (dmrs_cfg_r13.twelve_tone_base_seq_r13_present) {
      HANDLE_CODE(pack_integer(bref, dmrs_cfg_r13.twelve_tone_base_seq_r13, (uint8_t)0u, (uint8_t)30u));
    }
  }
  HANDLE_CODE(ul_ref_sigs_npusch_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE npusch_cfg_common_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_sf_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_cfg_r13_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(ack_nack_num_repeats_msg4_r13, bref, 1, 3));
  if (srs_sf_cfg_r13_present) {
    HANDLE_CODE(srs_sf_cfg_r13.unpack(bref));
  }
  if (dmrs_cfg_r13_present) {
    HANDLE_CODE(bref.unpack(dmrs_cfg_r13.three_tone_base_seq_r13_present, 1));
    HANDLE_CODE(bref.unpack(dmrs_cfg_r13.six_tone_base_seq_r13_present, 1));
    HANDLE_CODE(bref.unpack(dmrs_cfg_r13.twelve_tone_base_seq_r13_present, 1));
    if (dmrs_cfg_r13.three_tone_base_seq_r13_present) {
      HANDLE_CODE(unpack_integer(dmrs_cfg_r13.three_tone_base_seq_r13, bref, (uint8_t)0u, (uint8_t)12u));
    }
    HANDLE_CODE(unpack_integer(dmrs_cfg_r13.three_tone_cyclic_shift_r13, bref, (uint8_t)0u, (uint8_t)2u));
    if (dmrs_cfg_r13.six_tone_base_seq_r13_present) {
      HANDLE_CODE(unpack_integer(dmrs_cfg_r13.six_tone_base_seq_r13, bref, (uint8_t)0u, (uint8_t)14u));
    }
    HANDLE_CODE(unpack_integer(dmrs_cfg_r13.six_tone_cyclic_shift_r13, bref, (uint8_t)0u, (uint8_t)3u));
    if (dmrs_cfg_r13.twelve_tone_base_seq_r13_present) {
      HANDLE_CODE(unpack_integer(dmrs_cfg_r13.twelve_tone_base_seq_r13, bref, (uint8_t)0u, (uint8_t)30u));
    }
  }
  HANDLE_CODE(ul_ref_sigs_npusch_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void npusch_cfg_common_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("ack-NACK-NumRepetitions-Msg4-r13");
  for (const auto& e1 : ack_nack_num_repeats_msg4_r13) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  if (srs_sf_cfg_r13_present) {
    j.write_str("srs-SubframeConfig-r13", srs_sf_cfg_r13.to_string());
  }
  if (dmrs_cfg_r13_present) {
    j.write_fieldname("dmrs-Config-r13");
    j.start_obj();
    if (dmrs_cfg_r13.three_tone_base_seq_r13_present) {
      j.write_int("threeTone-BaseSequence-r13", dmrs_cfg_r13.three_tone_base_seq_r13);
    }
    j.write_int("threeTone-CyclicShift-r13", dmrs_cfg_r13.three_tone_cyclic_shift_r13);
    if (dmrs_cfg_r13.six_tone_base_seq_r13_present) {
      j.write_int("sixTone-BaseSequence-r13", dmrs_cfg_r13.six_tone_base_seq_r13);
    }
    j.write_int("sixTone-CyclicShift-r13", dmrs_cfg_r13.six_tone_cyclic_shift_r13);
    if (dmrs_cfg_r13.twelve_tone_base_seq_r13_present) {
      j.write_int("twelveTone-BaseSequence-r13", dmrs_cfg_r13.twelve_tone_base_seq_r13);
    }
    j.end_obj();
  }
  j.write_fieldname("ul-ReferenceSignalsNPUSCH-r13");
  ul_ref_sigs_npusch_r13.to_json(j);
  j.end_obj();
}

std::string npusch_cfg_common_nb_r13_s::srs_sf_cfg_r13_opts::to_string() const
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
  return convert_enum_idx(options, 16, value, "npusch_cfg_common_nb_r13_s::srs_sf_cfg_r13_e_");
}
uint8_t npusch_cfg_common_nb_r13_s::srs_sf_cfg_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  return map_enum_number(options, 16, value, "npusch_cfg_common_nb_r13_s::srs_sf_cfg_r13_e_");
}

// PCCH-Config-NB-r13 ::= SEQUENCE
SRSASN_CODE pcch_cfg_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(default_paging_cycle_r13.pack(bref));
  HANDLE_CODE(nb_r13.pack(bref));
  HANDLE_CODE(npdcch_num_repeat_paging_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_cfg_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(default_paging_cycle_r13.unpack(bref));
  HANDLE_CODE(nb_r13.unpack(bref));
  HANDLE_CODE(npdcch_num_repeat_paging_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void pcch_cfg_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("defaultPagingCycle-r13", default_paging_cycle_r13.to_string());
  j.write_str("nB-r13", nb_r13.to_string());
  j.write_str("npdcch-NumRepetitionPaging-r13", npdcch_num_repeat_paging_r13.to_string());
  j.end_obj();
}

std::string pcch_cfg_nb_r13_s::default_paging_cycle_r13_opts::to_string() const
{
  static const char* options[] = {"rf128", "rf256", "rf512", "rf1024"};
  return convert_enum_idx(options, 4, value, "pcch_cfg_nb_r13_s::default_paging_cycle_r13_e_");
}
uint16_t pcch_cfg_nb_r13_s::default_paging_cycle_r13_opts::to_number() const
{
  static const uint16_t options[] = {128, 256, 512, 1024};
  return map_enum_number(options, 4, value, "pcch_cfg_nb_r13_s::default_paging_cycle_r13_e_");
}

std::string pcch_cfg_nb_r13_s::nb_r13_opts::to_string() const
{
  static const char* options[] = {"fourT",
                                  "twoT",
                                  "oneT",
                                  "halfT",
                                  "quarterT",
                                  "one8thT",
                                  "one16thT",
                                  "one32ndT",
                                  "one64thT",
                                  "one128thT",
                                  "one256thT",
                                  "one512thT",
                                  "one1024thT",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pcch_cfg_nb_r13_s::nb_r13_e_");
}
float pcch_cfg_nb_r13_s::nb_r13_opts::to_number() const
{
  static const float options[] = {4.0, 2.0, 1.0, 0.5, 0.25, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0, 512.0, 1024.0};
  return map_enum_number(options, 13, value, "pcch_cfg_nb_r13_s::nb_r13_e_");
}
std::string pcch_cfg_nb_r13_s::nb_r13_opts::to_number_string() const
{
  static const char* options[] = {"4", "2", "1", "0.5", "0.25", "8", "16", "32", "64", "128", "256", "512", "1024"};
  return convert_enum_idx(options, 16, value, "pcch_cfg_nb_r13_s::nb_r13_e_");
}

std::string pcch_cfg_nb_r13_s::npdcch_num_repeat_paging_r13_opts::to_string() const
{
  static const char* options[] = {"r1",
                                  "r2",
                                  "r4",
                                  "r8",
                                  "r16",
                                  "r32",
                                  "r64",
                                  "r128",
                                  "r256",
                                  "r512",
                                  "r1024",
                                  "r2048",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pcch_cfg_nb_r13_s::npdcch_num_repeat_paging_r13_e_");
}
uint16_t pcch_cfg_nb_r13_s::npdcch_num_repeat_paging_r13_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
  return map_enum_number(options, 12, value, "pcch_cfg_nb_r13_s::npdcch_num_repeat_paging_r13_e_");
}

// RACH-ConfigCommon-NB-r13 ::= SEQUENCE
SRSASN_CODE rach_cfg_common_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(conn_est_fail_offset_r13_present, 1));

  HANDLE_CODE(preamb_trans_max_ce_r13.pack(bref));
  HANDLE_CODE(pwr_ramp_params_r13.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, rach_info_list_r13, 1, 3));
  if (conn_est_fail_offset_r13_present) {
    HANDLE_CODE(pack_integer(bref, conn_est_fail_offset_r13, (uint8_t)0u, (uint8_t)15u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= pwr_ramp_params_v1450.is_present();
    group_flags[1] |= rach_info_list_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pwr_ramp_params_v1450.is_present(), 1));
      if (pwr_ramp_params_v1450.is_present()) {
        HANDLE_CODE(pwr_ramp_params_v1450->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rach_info_list_v1530.is_present(), 1));
      if (rach_info_list_v1530.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *rach_info_list_v1530, 1, 3));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_cfg_common_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(conn_est_fail_offset_r13_present, 1));

  HANDLE_CODE(preamb_trans_max_ce_r13.unpack(bref));
  HANDLE_CODE(pwr_ramp_params_r13.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(rach_info_list_r13, bref, 1, 3));
  if (conn_est_fail_offset_r13_present) {
    HANDLE_CODE(unpack_integer(conn_est_fail_offset_r13, bref, (uint8_t)0u, (uint8_t)15u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool pwr_ramp_params_v1450_present;
      HANDLE_CODE(bref.unpack(pwr_ramp_params_v1450_present, 1));
      pwr_ramp_params_v1450.set_present(pwr_ramp_params_v1450_present);
      if (pwr_ramp_params_v1450.is_present()) {
        HANDLE_CODE(pwr_ramp_params_v1450->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rach_info_list_v1530_present;
      HANDLE_CODE(bref.unpack(rach_info_list_v1530_present, 1));
      rach_info_list_v1530.set_present(rach_info_list_v1530_present);
      if (rach_info_list_v1530.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*rach_info_list_v1530, bref, 1, 3));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rach_cfg_common_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("preambleTransMax-CE-r13", preamb_trans_max_ce_r13.to_string());
  j.write_fieldname("powerRampingParameters-r13");
  pwr_ramp_params_r13.to_json(j);
  j.start_array("rach-InfoList-r13");
  for (const auto& e1 : rach_info_list_r13) {
    e1.to_json(j);
  }
  j.end_array();
  if (conn_est_fail_offset_r13_present) {
    j.write_int("connEstFailOffset-r13", conn_est_fail_offset_r13);
  }
  if (ext) {
    if (pwr_ramp_params_v1450.is_present()) {
      j.write_fieldname("powerRampingParameters-v1450");
      pwr_ramp_params_v1450->to_json(j);
    }
    if (rach_info_list_v1530.is_present()) {
      j.start_array("rach-InfoList-v1530");
      for (const auto& e1 : *rach_info_list_v1530) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SIB-Type-NB-r13 ::= ENUMERATED
std::string sib_type_nb_r13_opts::to_string() const
{
  static const char* options[] = {"sibType3-NB-r13",
                                  "sibType4-NB-r13",
                                  "sibType5-NB-r13",
                                  "sibType14-NB-r13",
                                  "sibType16-NB-r13",
                                  "sibType15-NB-r14",
                                  "sibType20-NB-r14",
                                  "sibType22-NB-r14"};
  return convert_enum_idx(options, 8, value, "sib_type_nb_r13_e");
}
uint8_t sib_type_nb_r13_opts::to_number() const
{
  static const uint8_t options[] = {3, 4, 5, 14, 16, 15, 20, 22};
  return map_enum_number(options, 8, value, "sib_type_nb_r13_e");
}

// SystemInformationBlockType1-NB-v1530 ::= SEQUENCE
SRSASN_CODE sib_type1_nb_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tdd_params_r15_present, 1));
  HANDLE_CODE(bref.pack(sched_info_list_v1530_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (tdd_params_r15_present) {
    HANDLE_CODE(bref.pack(tdd_params_r15.tdd_si_sfs_bitmap_r15_present, 1));
    HANDLE_CODE(tdd_params_r15.tdd_cfg_r15.pack(bref));
    HANDLE_CODE(tdd_params_r15.tdd_si_carrier_info_r15.pack(bref));
    if (tdd_params_r15.tdd_si_sfs_bitmap_r15_present) {
      HANDLE_CODE(tdd_params_r15.tdd_si_sfs_bitmap_r15.pack(bref));
    }
  }
  if (sched_info_list_v1530_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sched_info_list_v1530, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_nb_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tdd_params_r15_present, 1));
  HANDLE_CODE(bref.unpack(sched_info_list_v1530_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (tdd_params_r15_present) {
    HANDLE_CODE(bref.unpack(tdd_params_r15.tdd_si_sfs_bitmap_r15_present, 1));
    HANDLE_CODE(tdd_params_r15.tdd_cfg_r15.unpack(bref));
    HANDLE_CODE(tdd_params_r15.tdd_si_carrier_info_r15.unpack(bref));
    if (tdd_params_r15.tdd_si_sfs_bitmap_r15_present) {
      HANDLE_CODE(tdd_params_r15.tdd_si_sfs_bitmap_r15.unpack(bref));
    }
  }
  if (sched_info_list_v1530_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sched_info_list_v1530, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_nb_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tdd_params_r15_present) {
    j.write_fieldname("tdd-Parameters-r15");
    j.start_obj();
    j.write_fieldname("tdd-Config-r15");
    tdd_params_r15.tdd_cfg_r15.to_json(j);
    j.write_str("tdd-SI-CarrierInfo-r15", tdd_params_r15.tdd_si_carrier_info_r15.to_string());
    if (tdd_params_r15.tdd_si_sfs_bitmap_r15_present) {
      j.write_fieldname("tdd-SI-SubframesBitmap-r15");
      tdd_params_r15.tdd_si_sfs_bitmap_r15.to_json(j);
    }
    j.end_obj();
  }
  if (sched_info_list_v1530_present) {
    j.start_array("schedulingInfoList-v1530");
    for (const auto& e1 : sched_info_list_v1530) {
      e1.to_json(j);
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

std::string sib_type1_nb_v1530_s::tdd_params_r15_s_::tdd_si_carrier_info_r15_opts::to_string() const
{
  static const char* options[] = {"anchor", "non-anchor"};
  return convert_enum_idx(options, 2, value, "sib_type1_nb_v1530_s::tdd_params_r15_s_::tdd_si_carrier_info_r15_e_");
}

// UL-ConfigCommon-NB-r14 ::= SEQUENCE
SRSASN_CODE ul_cfg_common_nb_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(nprach_params_list_r14_present, 1));

  HANDLE_CODE(ul_carrier_freq_r14.pack(bref));
  if (nprach_params_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, nprach_params_list_r14, 1, 3));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= nprach_params_list_edt_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(nprach_params_list_edt_r15.is_present(), 1));
      if (nprach_params_list_edt_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *nprach_params_list_edt_r15, 1, 3));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_cfg_common_nb_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(nprach_params_list_r14_present, 1));

  HANDLE_CODE(ul_carrier_freq_r14.unpack(bref));
  if (nprach_params_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(nprach_params_list_r14, bref, 1, 3));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool nprach_params_list_edt_r15_present;
      HANDLE_CODE(bref.unpack(nprach_params_list_edt_r15_present, 1));
      nprach_params_list_edt_r15.set_present(nprach_params_list_edt_r15_present);
      if (nprach_params_list_edt_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*nprach_params_list_edt_r15, bref, 1, 3));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ul_cfg_common_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ul-CarrierFreq-r14");
  ul_carrier_freq_r14.to_json(j);
  if (nprach_params_list_r14_present) {
    j.start_array("nprach-ParametersList-r14");
    for (const auto& e1 : nprach_params_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (nprach_params_list_edt_r15.is_present()) {
      j.start_array("nprach-ParametersListEDT-r15");
      for (const auto& e1 : *nprach_params_list_edt_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// UL-ConfigCommon-NB-v1530 ::= SEQUENCE
SRSASN_CODE ul_cfg_common_nb_v1530_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(nprach_params_list_fmt2_r15_present, 1));
  HANDLE_CODE(bref.pack(nprach_params_list_fmt2_edt_r15_present, 1));

  if (nprach_params_list_fmt2_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, nprach_params_list_fmt2_r15, 1, 3));
  }
  if (nprach_params_list_fmt2_edt_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, nprach_params_list_fmt2_edt_r15, 1, 3));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_cfg_common_nb_v1530_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(nprach_params_list_fmt2_r15_present, 1));
  HANDLE_CODE(bref.unpack(nprach_params_list_fmt2_edt_r15_present, 1));

  if (nprach_params_list_fmt2_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(nprach_params_list_fmt2_r15, bref, 1, 3));
  }
  if (nprach_params_list_fmt2_edt_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(nprach_params_list_fmt2_edt_r15, bref, 1, 3));
  }

  return SRSASN_SUCCESS;
}
void ul_cfg_common_nb_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nprach_params_list_fmt2_r15_present) {
    j.start_array("nprach-ParametersListFmt2-r15");
    for (const auto& e1 : nprach_params_list_fmt2_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (nprach_params_list_fmt2_edt_r15_present) {
    j.start_array("nprach-ParametersListFmt2EDT-r15");
    for (const auto& e1 : nprach_params_list_fmt2_edt_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// UL-ConfigCommonTDD-NB-r15 ::= SEQUENCE
SRSASN_CODE ul_cfg_common_tdd_nb_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(nprach_params_list_tdd_r15_present, 1));

  HANDLE_CODE(tdd_ul_dl_align_offset_r15.pack(bref));
  if (nprach_params_list_tdd_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, nprach_params_list_tdd_r15, 1, 3));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_cfg_common_tdd_nb_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(nprach_params_list_tdd_r15_present, 1));

  HANDLE_CODE(tdd_ul_dl_align_offset_r15.unpack(bref));
  if (nprach_params_list_tdd_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(nprach_params_list_tdd_r15, bref, 1, 3));
  }

  return SRSASN_SUCCESS;
}
void ul_cfg_common_tdd_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("tdd-UL-DL-AlignmentOffset-r15", tdd_ul_dl_align_offset_r15.to_string());
  if (nprach_params_list_tdd_r15_present) {
    j.start_array("nprach-ParametersListTDD-r15");
    for (const auto& e1 : nprach_params_list_tdd_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// UplinkPowerControlCommon-NB-r13 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_common_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, p0_nominal_npusch_r13, (int8_t)-126, (int8_t)24));
  HANDLE_CODE(alpha_r13.pack(bref));
  HANDLE_CODE(pack_integer(bref, delta_preamb_msg3_r13, (int8_t)-1, (int8_t)6));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_common_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(p0_nominal_npusch_r13, bref, (int8_t)-126, (int8_t)24));
  HANDLE_CODE(alpha_r13.unpack(bref));
  HANDLE_CODE(unpack_integer(delta_preamb_msg3_r13, bref, (int8_t)-1, (int8_t)6));

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_common_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p0-NominalNPUSCH-r13", p0_nominal_npusch_r13);
  j.write_str("alpha-r13", alpha_r13.to_string());
  j.write_int("deltaPreambleMsg3-r13", delta_preamb_msg3_r13);
  j.end_obj();
}

std::string ul_pwr_ctrl_common_nb_r13_s::alpha_r13_opts::to_string() const
{
  static const char* options[] = {"al0", "al04", "al05", "al06", "al07", "al08", "al09", "al1"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_nb_r13_s::alpha_r13_e_");
}
float ul_pwr_ctrl_common_nb_r13_s::alpha_r13_opts::to_number() const
{
  static const float options[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(options, 8, value, "ul_pwr_ctrl_common_nb_r13_s::alpha_r13_e_");
}
std::string ul_pwr_ctrl_common_nb_r13_s::alpha_r13_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_nb_r13_s::alpha_r13_e_");
}

// WUS-Config-NB-r15 ::= SEQUENCE
SRSASN_CODE wus_cfg_nb_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(num_pos_r15_present, 1));
  HANDLE_CODE(bref.pack(time_offset_e_drx_long_r15_present, 1));

  HANDLE_CODE(max_dur_factor_r15.pack(bref));
  if (num_pos_r15_present) {
    HANDLE_CODE(num_pos_r15.pack(bref));
  }
  HANDLE_CODE(num_drx_cycles_relaxed_r15.pack(bref));
  HANDLE_CODE(time_offset_drx_r15.pack(bref));
  HANDLE_CODE(time_offset_e_drx_short_r15.pack(bref));
  if (time_offset_e_drx_long_r15_present) {
    HANDLE_CODE(time_offset_e_drx_long_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE wus_cfg_nb_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(num_pos_r15_present, 1));
  HANDLE_CODE(bref.unpack(time_offset_e_drx_long_r15_present, 1));

  HANDLE_CODE(max_dur_factor_r15.unpack(bref));
  if (num_pos_r15_present) {
    HANDLE_CODE(num_pos_r15.unpack(bref));
  }
  HANDLE_CODE(num_drx_cycles_relaxed_r15.unpack(bref));
  HANDLE_CODE(time_offset_drx_r15.unpack(bref));
  HANDLE_CODE(time_offset_e_drx_short_r15.unpack(bref));
  if (time_offset_e_drx_long_r15_present) {
    HANDLE_CODE(time_offset_e_drx_long_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void wus_cfg_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxDurationFactor-r15", max_dur_factor_r15.to_string());
  if (num_pos_r15_present) {
    j.write_str("numPOs-r15", num_pos_r15.to_string());
  }
  j.write_str("numDRX-CyclesRelaxed-r15", num_drx_cycles_relaxed_r15.to_string());
  j.write_str("timeOffsetDRX-r15", time_offset_drx_r15.to_string());
  j.write_str("timeOffset-eDRX-Short-r15", time_offset_e_drx_short_r15.to_string());
  if (time_offset_e_drx_long_r15_present) {
    j.write_str("timeOffset-eDRX-Long-r15", time_offset_e_drx_long_r15.to_string());
  }
  j.end_obj();
}

std::string wus_cfg_nb_r15_s::num_pos_r15_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4"};
  return convert_enum_idx(options, 3, value, "wus_cfg_nb_r15_s::num_pos_r15_e_");
}
uint8_t wus_cfg_nb_r15_s::num_pos_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "wus_cfg_nb_r15_s::num_pos_r15_e_");
}

std::string wus_cfg_nb_r15_s::num_drx_cycles_relaxed_r15_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "n8"};
  return convert_enum_idx(options, 4, value, "wus_cfg_nb_r15_s::num_drx_cycles_relaxed_r15_e_");
}
uint8_t wus_cfg_nb_r15_s::num_drx_cycles_relaxed_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(options, 4, value, "wus_cfg_nb_r15_s::num_drx_cycles_relaxed_r15_e_");
}

std::string wus_cfg_nb_r15_s::time_offset_drx_r15_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms80", "ms160", "ms240"};
  return convert_enum_idx(options, 4, value, "wus_cfg_nb_r15_s::time_offset_drx_r15_e_");
}
uint8_t wus_cfg_nb_r15_s::time_offset_drx_r15_opts::to_number() const
{
  static const uint8_t options[] = {40, 80, 160, 240};
  return map_enum_number(options, 4, value, "wus_cfg_nb_r15_s::time_offset_drx_r15_e_");
}

std::string wus_cfg_nb_r15_s::time_offset_e_drx_short_r15_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms80", "ms160", "ms240"};
  return convert_enum_idx(options, 4, value, "wus_cfg_nb_r15_s::time_offset_e_drx_short_r15_e_");
}
uint8_t wus_cfg_nb_r15_s::time_offset_e_drx_short_r15_opts::to_number() const
{
  static const uint8_t options[] = {40, 80, 160, 240};
  return map_enum_number(options, 4, value, "wus_cfg_nb_r15_s::time_offset_e_drx_short_r15_e_");
}

std::string wus_cfg_nb_r15_s::time_offset_e_drx_long_r15_opts::to_string() const
{
  static const char* options[] = {"ms1000", "ms2000"};
  return convert_enum_idx(options, 2, value, "wus_cfg_nb_r15_s::time_offset_e_drx_long_r15_e_");
}
uint16_t wus_cfg_nb_r15_s::time_offset_e_drx_long_r15_opts::to_number() const
{
  static const uint16_t options[] = {1000, 2000};
  return map_enum_number(options, 2, value, "wus_cfg_nb_r15_s::time_offset_e_drx_long_r15_e_");
}

// CellReselectionInfoCommon-NB-v1450 ::= SEQUENCE
SRSASN_CODE cell_resel_info_common_nb_v1450_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(s_search_delta_p_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_resel_info_common_nb_v1450_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(s_search_delta_p_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_resel_info_common_nb_v1450_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("s-SearchDeltaP-r14", s_search_delta_p_r14.to_string());
  j.end_obj();
}

std::string cell_resel_info_common_nb_v1450_s::s_search_delta_p_r14_opts::to_string() const
{
  static const char* options[] = {"dB6", "dB9", "dB12", "dB15"};
  return convert_enum_idx(options, 4, value, "cell_resel_info_common_nb_v1450_s::s_search_delta_p_r14_e_");
}
uint8_t cell_resel_info_common_nb_v1450_s::s_search_delta_p_r14_opts::to_number() const
{
  static const uint8_t options[] = {6, 9, 12, 15};
  return map_enum_number(options, 4, value, "cell_resel_info_common_nb_v1450_s::s_search_delta_p_r14_e_");
}

// CellSelectionInfo-NB-v1430 ::= SEQUENCE
SRSASN_CODE cell_sel_info_nb_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pwr_class14dbm_offset_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_authorisation_offset_r14_present, 1));

  if (pwr_class14dbm_offset_r14_present) {
    HANDLE_CODE(pwr_class14dbm_offset_r14.pack(bref));
  }
  if (ce_authorisation_offset_r14_present) {
    HANDLE_CODE(ce_authorisation_offset_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_sel_info_nb_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pwr_class14dbm_offset_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_authorisation_offset_r14_present, 1));

  if (pwr_class14dbm_offset_r14_present) {
    HANDLE_CODE(pwr_class14dbm_offset_r14.unpack(bref));
  }
  if (ce_authorisation_offset_r14_present) {
    HANDLE_CODE(ce_authorisation_offset_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cell_sel_info_nb_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pwr_class14dbm_offset_r14_present) {
    j.write_str("powerClass14dBm-Offset-r14", pwr_class14dbm_offset_r14.to_string());
  }
  if (ce_authorisation_offset_r14_present) {
    j.write_str("ce-authorisationOffset-r14", ce_authorisation_offset_r14.to_string());
  }
  j.end_obj();
}

std::string cell_sel_info_nb_v1430_s::pwr_class14dbm_offset_r14_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-3", "dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(options, 6, value, "cell_sel_info_nb_v1430_s::pwr_class14dbm_offset_r14_e_");
}
int8_t cell_sel_info_nb_v1430_s::pwr_class14dbm_offset_r14_opts::to_number() const
{
  static const int8_t options[] = {-6, -3, 3, 6, 9, 12};
  return map_enum_number(options, 6, value, "cell_sel_info_nb_v1430_s::pwr_class14dbm_offset_r14_e_");
}

std::string cell_sel_info_nb_v1430_s::ce_authorisation_offset_r14_opts::to_string() const
{
  static const char* options[] = {"dB5", "dB10", "dB15", "dB20", "dB25", "dB30", "dB35"};
  return convert_enum_idx(options, 7, value, "cell_sel_info_nb_v1430_s::ce_authorisation_offset_r14_e_");
}
uint8_t cell_sel_info_nb_v1430_s::ce_authorisation_offset_r14_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 15, 20, 25, 30, 35};
  return map_enum_number(options, 7, value, "cell_sel_info_nb_v1430_s::ce_authorisation_offset_r14_e_");
}

// IntraFreqCellReselectionInfo-NB-v1350 ::= SEQUENCE
SRSASN_CODE intra_freq_cell_resel_info_nb_v1350_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, delta_rx_lev_min_v1350, (int8_t)-8, (int8_t)-1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_freq_cell_resel_info_nb_v1350_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(delta_rx_lev_min_v1350, bref, (int8_t)-8, (int8_t)-1));

  return SRSASN_SUCCESS;
}
void intra_freq_cell_resel_info_nb_v1350_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("delta-RxLevMin-v1350", delta_rx_lev_min_v1350);
  j.end_obj();
}

// IntraFreqCellReselectionInfo-NB-v1360 ::= SEQUENCE
SRSASN_CODE intra_freq_cell_resel_info_nb_v1360_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, s_intra_search_p_v1360, (uint8_t)32u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_freq_cell_resel_info_nb_v1360_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(s_intra_search_p_v1360, bref, (uint8_t)32u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
void intra_freq_cell_resel_info_nb_v1360_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("s-IntraSearchP-v1360", s_intra_search_p_v1360);
  j.end_obj();
}

// IntraFreqCellReselectionInfo-NB-v1430 ::= SEQUENCE
SRSASN_CODE intra_freq_cell_resel_info_nb_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pwr_class14dbm_offset_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_authorisation_offset_r14_present, 1));

  if (pwr_class14dbm_offset_r14_present) {
    HANDLE_CODE(pwr_class14dbm_offset_r14.pack(bref));
  }
  if (ce_authorisation_offset_r14_present) {
    HANDLE_CODE(ce_authorisation_offset_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_freq_cell_resel_info_nb_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pwr_class14dbm_offset_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_authorisation_offset_r14_present, 1));

  if (pwr_class14dbm_offset_r14_present) {
    HANDLE_CODE(pwr_class14dbm_offset_r14.unpack(bref));
  }
  if (ce_authorisation_offset_r14_present) {
    HANDLE_CODE(ce_authorisation_offset_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void intra_freq_cell_resel_info_nb_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pwr_class14dbm_offset_r14_present) {
    j.write_str("powerClass14dBm-Offset-r14", pwr_class14dbm_offset_r14.to_string());
  }
  if (ce_authorisation_offset_r14_present) {
    j.write_str("ce-AuthorisationOffset-r14", ce_authorisation_offset_r14.to_string());
  }
  j.end_obj();
}

std::string intra_freq_cell_resel_info_nb_v1430_s::pwr_class14dbm_offset_r14_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-3", "dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(options, 6, value, "intra_freq_cell_resel_info_nb_v1430_s::pwr_class14dbm_offset_r14_e_");
}
int8_t intra_freq_cell_resel_info_nb_v1430_s::pwr_class14dbm_offset_r14_opts::to_number() const
{
  static const int8_t options[] = {-6, -3, 3, 6, 9, 12};
  return map_enum_number(options, 6, value, "intra_freq_cell_resel_info_nb_v1430_s::pwr_class14dbm_offset_r14_e_");
}

std::string intra_freq_cell_resel_info_nb_v1430_s::ce_authorisation_offset_r14_opts::to_string() const
{
  static const char* options[] = {"dB5", "dB10", "dB15", "dB20", "dB25", "dB30", "dB35"};
  return convert_enum_idx(options, 7, value, "intra_freq_cell_resel_info_nb_v1430_s::ce_authorisation_offset_r14_e_");
}
uint8_t intra_freq_cell_resel_info_nb_v1430_s::ce_authorisation_offset_r14_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 15, 20, 25, 30, 35};
  return map_enum_number(options, 7, value, "intra_freq_cell_resel_info_nb_v1430_s::ce_authorisation_offset_r14_e_");
}

// NPDCCH-SC-MCCH-Config-NB-r14 ::= SEQUENCE
SRSASN_CODE npdcch_sc_mcch_cfg_nb_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(npdcch_num_repeats_sc_mcch_r14.pack(bref));
  HANDLE_CODE(npdcch_start_sf_sc_mcch_r14.pack(bref));
  HANDLE_CODE(npdcch_offset_sc_mcch_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE npdcch_sc_mcch_cfg_nb_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(npdcch_num_repeats_sc_mcch_r14.unpack(bref));
  HANDLE_CODE(npdcch_start_sf_sc_mcch_r14.unpack(bref));
  HANDLE_CODE(npdcch_offset_sc_mcch_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void npdcch_sc_mcch_cfg_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("npdcch-NumRepetitions-SC-MCCH-r14", npdcch_num_repeats_sc_mcch_r14.to_string());
  j.write_str("npdcch-StartSF-SC-MCCH-r14", npdcch_start_sf_sc_mcch_r14.to_string());
  j.write_str("npdcch-Offset-SC-MCCH-r14", npdcch_offset_sc_mcch_r14.to_string());
  j.end_obj();
}

std::string npdcch_sc_mcch_cfg_nb_r14_s::npdcch_num_repeats_sc_mcch_r14_opts::to_string() const
{
  static const char* options[] = {
      "r1", "r2", "r4", "r8", "r16", "r32", "r64", "r128", "r256", "r512", "r1024", "r2048"};
  return convert_enum_idx(options, 12, value, "npdcch_sc_mcch_cfg_nb_r14_s::npdcch_num_repeats_sc_mcch_r14_e_");
}
uint16_t npdcch_sc_mcch_cfg_nb_r14_s::npdcch_num_repeats_sc_mcch_r14_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
  return map_enum_number(options, 12, value, "npdcch_sc_mcch_cfg_nb_r14_s::npdcch_num_repeats_sc_mcch_r14_e_");
}

std::string npdcch_sc_mcch_cfg_nb_r14_s::npdcch_start_sf_sc_mcch_r14_opts::to_string() const
{
  static const char* options[] = {"v1dot5", "v2", "v4", "v8", "v16", "v32", "v48", "v64"};
  return convert_enum_idx(options, 8, value, "npdcch_sc_mcch_cfg_nb_r14_s::npdcch_start_sf_sc_mcch_r14_e_");
}
float npdcch_sc_mcch_cfg_nb_r14_s::npdcch_start_sf_sc_mcch_r14_opts::to_number() const
{
  static const float options[] = {1.5, 2.0, 4.0, 8.0, 16.0, 32.0, 48.0, 64.0};
  return map_enum_number(options, 8, value, "npdcch_sc_mcch_cfg_nb_r14_s::npdcch_start_sf_sc_mcch_r14_e_");
}
std::string npdcch_sc_mcch_cfg_nb_r14_s::npdcch_start_sf_sc_mcch_r14_opts::to_number_string() const
{
  static const char* options[] = {"1.5", "2", "4", "8", "16", "32", "48", "64"};
  return convert_enum_idx(options, 8, value, "npdcch_sc_mcch_cfg_nb_r14_s::npdcch_start_sf_sc_mcch_r14_e_");
}

std::string npdcch_sc_mcch_cfg_nb_r14_s::npdcch_offset_sc_mcch_r14_opts::to_string() const
{
  static const char* options[] = {
      "zero", "oneEighth", "oneQuarter", "threeEighth", "oneHalf", "fiveEighth", "threeQuarter", "sevenEighth"};
  return convert_enum_idx(options, 8, value, "npdcch_sc_mcch_cfg_nb_r14_s::npdcch_offset_sc_mcch_r14_e_");
}
float npdcch_sc_mcch_cfg_nb_r14_s::npdcch_offset_sc_mcch_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875};
  return map_enum_number(options, 8, value, "npdcch_sc_mcch_cfg_nb_r14_s::npdcch_offset_sc_mcch_r14_e_");
}
std::string npdcch_sc_mcch_cfg_nb_r14_s::npdcch_offset_sc_mcch_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "1/8", "1/4", "3/8", "1/2", "5/8", "3/4", "7/8"};
  return convert_enum_idx(options, 8, value, "npdcch_sc_mcch_cfg_nb_r14_s::npdcch_offset_sc_mcch_r14_e_");
}

// RadioResourceConfigCommonSIB-NB-r13 ::= SEQUENCE
SRSASN_CODE rr_cfg_common_sib_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dl_gap_r13_present, 1));

  HANDLE_CODE(rach_cfg_common_r13.pack(bref));
  HANDLE_CODE(bcch_cfg_r13.pack(bref));
  HANDLE_CODE(pcch_cfg_r13.pack(bref));
  HANDLE_CODE(nprach_cfg_r13.pack(bref));
  HANDLE_CODE(npdsch_cfg_common_r13.pack(bref));
  HANDLE_CODE(npusch_cfg_common_r13.pack(bref));
  if (dl_gap_r13_present) {
    HANDLE_CODE(dl_gap_r13.pack(bref));
  }
  HANDLE_CODE(ul_pwr_ctrl_common_r13.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= nprach_cfg_v1330.is_present();
    group_flags[1] |= nprach_cfg_v1450.is_present();
    group_flags[2] |= nprach_cfg_v1530.is_present();
    group_flags[2] |= dl_gap_v1530.is_present();
    group_flags[2] |= wus_cfg_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(nprach_cfg_v1330.is_present(), 1));
      if (nprach_cfg_v1330.is_present()) {
        HANDLE_CODE(nprach_cfg_v1330->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(nprach_cfg_v1450.is_present(), 1));
      if (nprach_cfg_v1450.is_present()) {
        HANDLE_CODE(nprach_cfg_v1450->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(nprach_cfg_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(dl_gap_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(wus_cfg_r15.is_present(), 1));
      if (nprach_cfg_v1530.is_present()) {
        HANDLE_CODE(nprach_cfg_v1530->pack(bref));
      }
      if (dl_gap_v1530.is_present()) {
        HANDLE_CODE(dl_gap_v1530->pack(bref));
      }
      if (wus_cfg_r15.is_present()) {
        HANDLE_CODE(wus_cfg_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_common_sib_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dl_gap_r13_present, 1));

  HANDLE_CODE(rach_cfg_common_r13.unpack(bref));
  HANDLE_CODE(bcch_cfg_r13.unpack(bref));
  HANDLE_CODE(pcch_cfg_r13.unpack(bref));
  HANDLE_CODE(nprach_cfg_r13.unpack(bref));
  HANDLE_CODE(npdsch_cfg_common_r13.unpack(bref));
  HANDLE_CODE(npusch_cfg_common_r13.unpack(bref));
  if (dl_gap_r13_present) {
    HANDLE_CODE(dl_gap_r13.unpack(bref));
  }
  HANDLE_CODE(ul_pwr_ctrl_common_r13.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool nprach_cfg_v1330_present;
      HANDLE_CODE(bref.unpack(nprach_cfg_v1330_present, 1));
      nprach_cfg_v1330.set_present(nprach_cfg_v1330_present);
      if (nprach_cfg_v1330.is_present()) {
        HANDLE_CODE(nprach_cfg_v1330->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool nprach_cfg_v1450_present;
      HANDLE_CODE(bref.unpack(nprach_cfg_v1450_present, 1));
      nprach_cfg_v1450.set_present(nprach_cfg_v1450_present);
      if (nprach_cfg_v1450.is_present()) {
        HANDLE_CODE(nprach_cfg_v1450->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool nprach_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(nprach_cfg_v1530_present, 1));
      nprach_cfg_v1530.set_present(nprach_cfg_v1530_present);
      bool dl_gap_v1530_present;
      HANDLE_CODE(bref.unpack(dl_gap_v1530_present, 1));
      dl_gap_v1530.set_present(dl_gap_v1530_present);
      bool wus_cfg_r15_present;
      HANDLE_CODE(bref.unpack(wus_cfg_r15_present, 1));
      wus_cfg_r15.set_present(wus_cfg_r15_present);
      if (nprach_cfg_v1530.is_present()) {
        HANDLE_CODE(nprach_cfg_v1530->unpack(bref));
      }
      if (dl_gap_v1530.is_present()) {
        HANDLE_CODE(dl_gap_v1530->unpack(bref));
      }
      if (wus_cfg_r15.is_present()) {
        HANDLE_CODE(wus_cfg_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rr_cfg_common_sib_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rach-ConfigCommon-r13");
  rach_cfg_common_r13.to_json(j);
  j.write_fieldname("bcch-Config-r13");
  bcch_cfg_r13.to_json(j);
  j.write_fieldname("pcch-Config-r13");
  pcch_cfg_r13.to_json(j);
  j.write_fieldname("nprach-Config-r13");
  nprach_cfg_r13.to_json(j);
  j.write_fieldname("npdsch-ConfigCommon-r13");
  npdsch_cfg_common_r13.to_json(j);
  j.write_fieldname("npusch-ConfigCommon-r13");
  npusch_cfg_common_r13.to_json(j);
  if (dl_gap_r13_present) {
    j.write_fieldname("dl-Gap-r13");
    dl_gap_r13.to_json(j);
  }
  j.write_fieldname("uplinkPowerControlCommon-r13");
  ul_pwr_ctrl_common_r13.to_json(j);
  if (ext) {
    if (nprach_cfg_v1330.is_present()) {
      j.write_fieldname("nprach-Config-v1330");
      nprach_cfg_v1330->to_json(j);
    }
    if (nprach_cfg_v1450.is_present()) {
      j.write_fieldname("nprach-Config-v1450");
      nprach_cfg_v1450->to_json(j);
    }
    if (nprach_cfg_v1530.is_present()) {
      j.write_fieldname("nprach-Config-v1530");
      nprach_cfg_v1530->to_json(j);
    }
    if (dl_gap_v1530.is_present()) {
      j.write_fieldname("dl-Gap-v1530");
      dl_gap_v1530->to_json(j);
    }
    if (wus_cfg_r15.is_present()) {
      j.write_fieldname("wus-Config-r15");
      wus_cfg_r15->to_json(j);
    }
  }
  j.end_obj();
}

// SC-MCCH-SchedulingInfo-NB-r14 ::= SEQUENCE
SRSASN_CODE sc_mcch_sched_info_nb_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(on_dur_timer_scptm_r14.pack(bref));
  HANDLE_CODE(drx_inactivity_timer_scptm_r14.pack(bref));
  HANDLE_CODE(sched_period_start_offset_scptm_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mcch_sched_info_nb_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(on_dur_timer_scptm_r14.unpack(bref));
  HANDLE_CODE(drx_inactivity_timer_scptm_r14.unpack(bref));
  HANDLE_CODE(sched_period_start_offset_scptm_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void sc_mcch_sched_info_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("onDurationTimerSCPTM-r14", on_dur_timer_scptm_r14.to_string());
  j.write_str("drx-InactivityTimerSCPTM-r14", drx_inactivity_timer_scptm_r14.to_string());
  j.write_fieldname("schedulingPeriodStartOffsetSCPTM-r14");
  sched_period_start_offset_scptm_r14.to_json(j);
  j.end_obj();
}

std::string sc_mcch_sched_info_nb_r14_s::on_dur_timer_scptm_r14_opts::to_string() const
{
  static const char* options[] = {"pp1", "pp2", "pp3", "pp4", "pp8", "pp16", "pp32", "spare"};
  return convert_enum_idx(options, 8, value, "sc_mcch_sched_info_nb_r14_s::on_dur_timer_scptm_r14_e_");
}
uint8_t sc_mcch_sched_info_nb_r14_s::on_dur_timer_scptm_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 8, 16, 32};
  return map_enum_number(options, 7, value, "sc_mcch_sched_info_nb_r14_s::on_dur_timer_scptm_r14_e_");
}

std::string sc_mcch_sched_info_nb_r14_s::drx_inactivity_timer_scptm_r14_opts::to_string() const
{
  static const char* options[] = {"pp0", "pp1", "pp2", "pp3", "pp4", "pp8", "pp16", "pp32"};
  return convert_enum_idx(options, 8, value, "sc_mcch_sched_info_nb_r14_s::drx_inactivity_timer_scptm_r14_e_");
}
uint8_t sc_mcch_sched_info_nb_r14_s::drx_inactivity_timer_scptm_r14_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 8, 16, 32};
  return map_enum_number(options, 8, value, "sc_mcch_sched_info_nb_r14_s::drx_inactivity_timer_scptm_r14_e_");
}

void sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::destroy_() {}
void sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::sched_period_start_offset_scptm_r14_c_(
    const sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf32:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf64:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf128:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf256:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf512:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf640:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf1024:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf2048:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf4096:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf8192:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_");
  }
}
sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_&
sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::
operator=(const sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf32:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf64:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf128:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf256:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf512:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf640:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf1024:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf2048:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf4096:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf8192:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_");
  }

  return *this;
}
void sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf10:
      j.write_int("sf10", c.get<uint8_t>());
      break;
    case types::sf20:
      j.write_int("sf20", c.get<uint8_t>());
      break;
    case types::sf32:
      j.write_int("sf32", c.get<uint8_t>());
      break;
    case types::sf40:
      j.write_int("sf40", c.get<uint8_t>());
      break;
    case types::sf64:
      j.write_int("sf64", c.get<uint8_t>());
      break;
    case types::sf80:
      j.write_int("sf80", c.get<uint8_t>());
      break;
    case types::sf128:
      j.write_int("sf128", c.get<uint8_t>());
      break;
    case types::sf160:
      j.write_int("sf160", c.get<uint8_t>());
      break;
    case types::sf256:
      j.write_int("sf256", c.get<uint16_t>());
      break;
    case types::sf320:
      j.write_int("sf320", c.get<uint16_t>());
      break;
    case types::sf512:
      j.write_int("sf512", c.get<uint16_t>());
      break;
    case types::sf640:
      j.write_int("sf640", c.get<uint16_t>());
      break;
    case types::sf1024:
      j.write_int("sf1024", c.get<uint16_t>());
      break;
    case types::sf2048:
      j.write_int("sf2048", c.get<uint16_t>());
      break;
    case types::sf4096:
      j.write_int("sf4096", c.get<uint16_t>());
      break;
    case types::sf8192:
      j.write_int("sf8192", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf32:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sf40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::sf80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf128:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sf160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sf256:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sf320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sf512:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::sf640:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sf1024:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::sf2048:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::sf4096:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u));
      break;
    case types::sf8192:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)8191u));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf32:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sf40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::sf80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf128:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sf160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sf256:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sf320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sf512:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::sf640:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sf1024:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::sf2048:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::sf4096:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u));
      break;
    case types::sf8192:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)8191u));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"sf10",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf256",
                                  "sf320",
                                  "sf512",
                                  "sf640",
                                  "sf1024",
                                  "sf2048",
                                  "sf4096",
                                  "sf8192"};
  return convert_enum_idx(
      options, 16, value, "sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::types");
}
uint16_t sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1024, 2048, 4096, 8192};
  return map_enum_number(
      options, 16, value, "sc_mcch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::types");
}

// SystemInformationBlockType1-NB-v1450 ::= SEQUENCE
SRSASN_CODE sib_type1_nb_v1450_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nrs_crs_pwr_offset_v1450_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (nrs_crs_pwr_offset_v1450_present) {
    HANDLE_CODE(nrs_crs_pwr_offset_v1450.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_nb_v1450_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nrs_crs_pwr_offset_v1450_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (nrs_crs_pwr_offset_v1450_present) {
    HANDLE_CODE(nrs_crs_pwr_offset_v1450.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_nb_v1450_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nrs_crs_pwr_offset_v1450_present) {
    j.write_str("nrs-CRS-PowerOffset-v1450", nrs_crs_pwr_offset_v1450.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

std::string sib_type1_nb_v1450_s::nrs_crs_pwr_offset_v1450_opts::to_string() const
{
  static const char* options[] = {"dB-6",
                                  "dB-4dot77",
                                  "dB-3",
                                  "dB-1dot77",
                                  "dB0",
                                  "dB1",
                                  "dB1dot23",
                                  "dB2",
                                  "dB3",
                                  "dB4",
                                  "dB4dot23",
                                  "dB5",
                                  "dB6",
                                  "dB7",
                                  "dB8",
                                  "dB9"};
  return convert_enum_idx(options, 16, value, "sib_type1_nb_v1450_s::nrs_crs_pwr_offset_v1450_e_");
}
float sib_type1_nb_v1450_s::nrs_crs_pwr_offset_v1450_opts::to_number() const
{
  static const float options[] = {
      -6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 1.23, 2.0, 3.0, 4.0, 4.23, 5.0, 6.0, 7.0, 8.0, 9.0};
  return map_enum_number(options, 16, value, "sib_type1_nb_v1450_s::nrs_crs_pwr_offset_v1450_e_");
}
std::string sib_type1_nb_v1450_s::nrs_crs_pwr_offset_v1450_opts::to_number_string() const
{
  static const char* options[] = {
      "-6", "-4.77", "-3", "-1.77", "0", "1", "1.23", "2", "3", "4", "4.23", "5", "6", "7", "8", "9"};
  return convert_enum_idx(options, 16, value, "sib_type1_nb_v1450_s::nrs_crs_pwr_offset_v1450_e_");
}

// T-Reselection-NB-r13 ::= ENUMERATED
std::string t_resel_nb_r13_opts::to_string() const
{
  static const char* options[] = {"s0", "s3", "s6", "s9", "s12", "s15", "s18", "s21"};
  return convert_enum_idx(options, 8, value, "t_resel_nb_r13_e");
}
uint8_t t_resel_nb_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12, 15, 18, 21};
  return map_enum_number(options, 8, value, "t_resel_nb_r13_e");
}

// UE-TimersAndConstants-NB-r13 ::= SEQUENCE
SRSASN_CODE ue_timers_and_consts_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(t300_r13.pack(bref));
  HANDLE_CODE(t301_r13.pack(bref));
  HANDLE_CODE(t310_r13.pack(bref));
  HANDLE_CODE(n310_r13.pack(bref));
  HANDLE_CODE(t311_r13.pack(bref));
  HANDLE_CODE(n311_r13.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= t311_v1350_present;
    group_flags[1] |= t300_v1530_present;
    group_flags[1] |= t301_v1530_present;
    group_flags[1] |= t311_v1530_present;
    group_flags[1] |= t300_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(t311_v1350_present, 1));
      if (t311_v1350_present) {
        HANDLE_CODE(t311_v1350.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(t300_v1530_present, 1));
      HANDLE_CODE(bref.pack(t301_v1530_present, 1));
      HANDLE_CODE(bref.pack(t311_v1530_present, 1));
      HANDLE_CODE(bref.pack(t300_r15_present, 1));
      if (t300_v1530_present) {
        HANDLE_CODE(t300_v1530.pack(bref));
      }
      if (t301_v1530_present) {
        HANDLE_CODE(t301_v1530.pack(bref));
      }
      if (t311_v1530_present) {
        HANDLE_CODE(t311_v1530.pack(bref));
      }
      if (t300_r15_present) {
        HANDLE_CODE(t300_r15.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_timers_and_consts_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(t300_r13.unpack(bref));
  HANDLE_CODE(t301_r13.unpack(bref));
  HANDLE_CODE(t310_r13.unpack(bref));
  HANDLE_CODE(n310_r13.unpack(bref));
  HANDLE_CODE(t311_r13.unpack(bref));
  HANDLE_CODE(n311_r13.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(t311_v1350_present, 1));
      if (t311_v1350_present) {
        HANDLE_CODE(t311_v1350.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(t300_v1530_present, 1));
      HANDLE_CODE(bref.unpack(t301_v1530_present, 1));
      HANDLE_CODE(bref.unpack(t311_v1530_present, 1));
      HANDLE_CODE(bref.unpack(t300_r15_present, 1));
      if (t300_v1530_present) {
        HANDLE_CODE(t300_v1530.unpack(bref));
      }
      if (t301_v1530_present) {
        HANDLE_CODE(t301_v1530.unpack(bref));
      }
      if (t311_v1530_present) {
        HANDLE_CODE(t311_v1530.unpack(bref));
      }
      if (t300_r15_present) {
        HANDLE_CODE(t300_r15.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ue_timers_and_consts_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t300-r13", t300_r13.to_string());
  j.write_str("t301-r13", t301_r13.to_string());
  j.write_str("t310-r13", t310_r13.to_string());
  j.write_str("n310-r13", n310_r13.to_string());
  j.write_str("t311-r13", t311_r13.to_string());
  j.write_str("n311-r13", n311_r13.to_string());
  if (ext) {
    if (t311_v1350_present) {
      j.write_str("t311-v1350", t311_v1350.to_string());
    }
    if (t300_v1530_present) {
      j.write_str("t300-v1530", t300_v1530.to_string());
    }
    if (t301_v1530_present) {
      j.write_str("t301-v1530", t301_v1530.to_string());
    }
    if (t311_v1530_present) {
      j.write_str("t311-v1530", t311_v1530.to_string());
    }
    if (t300_r15_present) {
      j.write_str("t300-r15", t300_r15.to_string());
    }
  }
  j.end_obj();
}

std::string ue_timers_and_consts_nb_r13_s::t300_r13_opts::to_string() const
{
  static const char* options[] = {"ms2500", "ms4000", "ms6000", "ms10000", "ms15000", "ms25000", "ms40000", "ms60000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_nb_r13_s::t300_r13_e_");
}
uint16_t ue_timers_and_consts_nb_r13_s::t300_r13_opts::to_number() const
{
  static const uint16_t options[] = {2500, 4000, 6000, 10000, 15000, 25000, 40000, 60000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_nb_r13_s::t300_r13_e_");
}

std::string ue_timers_and_consts_nb_r13_s::t301_r13_opts::to_string() const
{
  static const char* options[] = {"ms2500", "ms4000", "ms6000", "ms10000", "ms15000", "ms25000", "ms40000", "ms60000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_nb_r13_s::t301_r13_e_");
}
uint16_t ue_timers_and_consts_nb_r13_s::t301_r13_opts::to_number() const
{
  static const uint16_t options[] = {2500, 4000, 6000, 10000, 15000, 25000, 40000, 60000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_nb_r13_s::t301_r13_e_");
}

std::string ue_timers_and_consts_nb_r13_s::t310_r13_opts::to_string() const
{
  static const char* options[] = {"ms0", "ms200", "ms500", "ms1000", "ms2000", "ms4000", "ms8000"};
  return convert_enum_idx(options, 7, value, "ue_timers_and_consts_nb_r13_s::t310_r13_e_");
}
uint16_t ue_timers_and_consts_nb_r13_s::t310_r13_opts::to_number() const
{
  static const uint16_t options[] = {0, 200, 500, 1000, 2000, 4000, 8000};
  return map_enum_number(options, 7, value, "ue_timers_and_consts_nb_r13_s::t310_r13_e_");
}

std::string ue_timers_and_consts_nb_r13_s::n310_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_nb_r13_s::n310_r13_e_");
}
uint8_t ue_timers_and_consts_nb_r13_s::n310_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_nb_r13_s::n310_r13_e_");
}

std::string ue_timers_and_consts_nb_r13_s::t311_r13_opts::to_string() const
{
  static const char* options[] = {"ms1000", "ms3000", "ms5000", "ms10000", "ms15000", "ms20000", "ms30000"};
  return convert_enum_idx(options, 7, value, "ue_timers_and_consts_nb_r13_s::t311_r13_e_");
}
uint16_t ue_timers_and_consts_nb_r13_s::t311_r13_opts::to_number() const
{
  static const uint16_t options[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return map_enum_number(options, 7, value, "ue_timers_and_consts_nb_r13_s::t311_r13_e_");
}

std::string ue_timers_and_consts_nb_r13_s::n311_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_nb_r13_s::n311_r13_e_");
}
uint8_t ue_timers_and_consts_nb_r13_s::n311_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_nb_r13_s::n311_r13_e_");
}

std::string ue_timers_and_consts_nb_r13_s::t311_v1350_opts::to_string() const
{
  static const char* options[] = {"ms40000", "ms60000", "ms90000", "ms120000"};
  return convert_enum_idx(options, 4, value, "ue_timers_and_consts_nb_r13_s::t311_v1350_e_");
}
uint32_t ue_timers_and_consts_nb_r13_s::t311_v1350_opts::to_number() const
{
  static const uint32_t options[] = {40000, 60000, 90000, 120000};
  return map_enum_number(options, 4, value, "ue_timers_and_consts_nb_r13_s::t311_v1350_e_");
}

std::string ue_timers_and_consts_nb_r13_s::t300_v1530_opts::to_string() const
{
  static const char* options[] = {"ms80000", "ms100000", "ms120000"};
  return convert_enum_idx(options, 3, value, "ue_timers_and_consts_nb_r13_s::t300_v1530_e_");
}
uint32_t ue_timers_and_consts_nb_r13_s::t300_v1530_opts::to_number() const
{
  static const uint32_t options[] = {80000, 100000, 120000};
  return map_enum_number(options, 3, value, "ue_timers_and_consts_nb_r13_s::t300_v1530_e_");
}

std::string ue_timers_and_consts_nb_r13_s::t301_v1530_opts::to_string() const
{
  static const char* options[] = {"ms80000", "ms100000", "ms120000"};
  return convert_enum_idx(options, 3, value, "ue_timers_and_consts_nb_r13_s::t301_v1530_e_");
}
uint32_t ue_timers_and_consts_nb_r13_s::t301_v1530_opts::to_number() const
{
  static const uint32_t options[] = {80000, 100000, 120000};
  return map_enum_number(options, 3, value, "ue_timers_and_consts_nb_r13_s::t301_v1530_e_");
}

std::string ue_timers_and_consts_nb_r13_s::t311_v1530_opts::to_string() const
{
  static const char* options[] = {"ms160000", "ms200000"};
  return convert_enum_idx(options, 2, value, "ue_timers_and_consts_nb_r13_s::t311_v1530_e_");
}
uint32_t ue_timers_and_consts_nb_r13_s::t311_v1530_opts::to_number() const
{
  static const uint32_t options[] = {160000, 200000};
  return map_enum_number(options, 2, value, "ue_timers_and_consts_nb_r13_s::t311_v1530_e_");
}

std::string ue_timers_and_consts_nb_r13_s::t300_r15_opts::to_string() const
{
  static const char* options[] = {
      "ms6000", "ms10000", "ms15000", "ms25000", "ms40000", "ms60000", "ms80000", "ms120000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_nb_r13_s::t300_r15_e_");
}
uint32_t ue_timers_and_consts_nb_r13_s::t300_r15_opts::to_number() const
{
  static const uint32_t options[] = {6000, 10000, 15000, 25000, 40000, 60000, 80000, 120000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_nb_r13_s::t300_r15_e_");
}

// CellSelectionInfo-NB-v1350 ::= SEQUENCE
SRSASN_CODE cell_sel_info_nb_v1350_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, delta_rx_lev_min_v1350, (int8_t)-8, (int8_t)-1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_sel_info_nb_v1350_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(delta_rx_lev_min_v1350, bref, (int8_t)-8, (int8_t)-1));

  return SRSASN_SUCCESS;
}
void cell_sel_info_nb_v1350_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("delta-RxLevMin-v1350", delta_rx_lev_min_v1350);
  j.end_obj();
}

// PLMN-IdentityInfo-NB-r13 ::= SEQUENCE
SRSASN_CODE plmn_id_info_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(attach_without_pdn_connect_r13_present, 1));

  HANDLE_CODE(plmn_id_r13.pack(bref));
  HANDLE_CODE(cell_reserved_for_oper_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_info_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(attach_without_pdn_connect_r13_present, 1));

  HANDLE_CODE(plmn_id_r13.unpack(bref));
  HANDLE_CODE(cell_reserved_for_oper_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void plmn_id_info_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Identity-r13");
  plmn_id_r13.to_json(j);
  j.write_str("cellReservedForOperatorUse-r13", cell_reserved_for_oper_r13.to_string());
  if (attach_without_pdn_connect_r13_present) {
    j.write_str("attachWithoutPDN-Connectivity-r13", "true");
  }
  j.end_obj();
}

std::string plmn_id_info_nb_r13_s::cell_reserved_for_oper_r13_opts::to_string() const
{
  static const char* options[] = {"reserved", "notReserved"};
  return convert_enum_idx(options, 2, value, "plmn_id_info_nb_r13_s::cell_reserved_for_oper_r13_e_");
}

// SchedulingInfo-NB-r13 ::= SEQUENCE
SRSASN_CODE sched_info_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(si_periodicity_r13.pack(bref));
  HANDLE_CODE(si_repeat_pattern_r13.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, sib_map_info_r13, 0, 31));
  HANDLE_CODE(si_tb_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_info_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(si_periodicity_r13.unpack(bref));
  HANDLE_CODE(si_repeat_pattern_r13.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(sib_map_info_r13, bref, 0, 31));
  HANDLE_CODE(si_tb_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void sched_info_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("si-Periodicity-r13", si_periodicity_r13.to_string());
  j.write_str("si-RepetitionPattern-r13", si_repeat_pattern_r13.to_string());
  j.start_array("sib-MappingInfo-r13");
  for (const auto& e1 : sib_map_info_r13) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.write_str("si-TB-r13", si_tb_r13.to_string());
  j.end_obj();
}

std::string sched_info_nb_r13_s::si_periodicity_r13_opts::to_string() const
{
  static const char* options[] = {"rf64", "rf128", "rf256", "rf512", "rf1024", "rf2048", "rf4096", "spare"};
  return convert_enum_idx(options, 8, value, "sched_info_nb_r13_s::si_periodicity_r13_e_");
}
uint16_t sched_info_nb_r13_s::si_periodicity_r13_opts::to_number() const
{
  static const uint16_t options[] = {64, 128, 256, 512, 1024, 2048, 4096};
  return map_enum_number(options, 7, value, "sched_info_nb_r13_s::si_periodicity_r13_e_");
}

std::string sched_info_nb_r13_s::si_repeat_pattern_r13_opts::to_string() const
{
  static const char* options[] = {"every2ndRF", "every4thRF", "every8thRF", "every16thRF"};
  return convert_enum_idx(options, 4, value, "sched_info_nb_r13_s::si_repeat_pattern_r13_e_");
}
uint8_t sched_info_nb_r13_s::si_repeat_pattern_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8, 16};
  return map_enum_number(options, 4, value, "sched_info_nb_r13_s::si_repeat_pattern_r13_e_");
}

std::string sched_info_nb_r13_s::si_tb_r13_opts::to_string() const
{
  static const char* options[] = {"b56", "b120", "b208", "b256", "b328", "b440", "b552", "b680"};
  return convert_enum_idx(options, 8, value, "sched_info_nb_r13_s::si_tb_r13_e_");
}
uint16_t sched_info_nb_r13_s::si_tb_r13_opts::to_number() const
{
  static const uint16_t options[] = {56, 120, 208, 256, 328, 440, 552, 680};
  return map_enum_number(options, 8, value, "sched_info_nb_r13_s::si_tb_r13_e_");
}

// SystemInformationBlockType1-NB-v1430 ::= SEQUENCE
SRSASN_CODE sib_type1_nb_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_sel_info_v1430_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (cell_sel_info_v1430_present) {
    HANDLE_CODE(cell_sel_info_v1430.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_nb_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_sel_info_v1430_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cell_sel_info_v1430_present) {
    HANDLE_CODE(cell_sel_info_v1430.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_nb_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_sel_info_v1430_present) {
    j.write_fieldname("cellSelectionInfo-v1430");
    cell_sel_info_v1430.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType14-NB-r13 ::= SEQUENCE
SRSASN_CODE sib_type14_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ab_param_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (ab_param_r13_present) {
    HANDLE_CODE(ab_param_r13.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ab_per_nrsrp_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ab_per_nrsrp_r15_present, 1));
      if (ab_per_nrsrp_r15_present) {
        HANDLE_CODE(ab_per_nrsrp_r15.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type14_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ab_param_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (ab_param_r13_present) {
    HANDLE_CODE(ab_param_r13.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ab_per_nrsrp_r15_present, 1));
      if (ab_per_nrsrp_r15_present) {
        HANDLE_CODE(ab_per_nrsrp_r15.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type14_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ab_param_r13_present) {
    j.write_fieldname("ab-Param-r13");
    ab_param_r13.to_json(j);
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (ab_per_nrsrp_r15_present) {
      j.write_str("ab-PerNRSRP-r15", ab_per_nrsrp_r15.to_string());
    }
  }
  j.end_obj();
}

void sib_type14_nb_r13_s::ab_param_r13_c_::destroy_()
{
  switch (type_) {
    case types::ab_common_r13:
      c.destroy<ab_cfg_nb_r13_s>();
      break;
    case types::ab_per_plmn_list_r13:
      c.destroy<ab_per_plmn_list_r13_l_>();
      break;
    default:
      break;
  }
}
void sib_type14_nb_r13_s::ab_param_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ab_common_r13:
      c.init<ab_cfg_nb_r13_s>();
      break;
    case types::ab_per_plmn_list_r13:
      c.init<ab_per_plmn_list_r13_l_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type14_nb_r13_s::ab_param_r13_c_");
  }
}
sib_type14_nb_r13_s::ab_param_r13_c_::ab_param_r13_c_(const sib_type14_nb_r13_s::ab_param_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ab_common_r13:
      c.init(other.c.get<ab_cfg_nb_r13_s>());
      break;
    case types::ab_per_plmn_list_r13:
      c.init(other.c.get<ab_per_plmn_list_r13_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type14_nb_r13_s::ab_param_r13_c_");
  }
}
sib_type14_nb_r13_s::ab_param_r13_c_& sib_type14_nb_r13_s::ab_param_r13_c_::
                                      operator=(const sib_type14_nb_r13_s::ab_param_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ab_common_r13:
      c.set(other.c.get<ab_cfg_nb_r13_s>());
      break;
    case types::ab_per_plmn_list_r13:
      c.set(other.c.get<ab_per_plmn_list_r13_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type14_nb_r13_s::ab_param_r13_c_");
  }

  return *this;
}
void sib_type14_nb_r13_s::ab_param_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ab_common_r13:
      j.write_fieldname("ab-Common-r13");
      c.get<ab_cfg_nb_r13_s>().to_json(j);
      break;
    case types::ab_per_plmn_list_r13:
      j.start_array("ab-PerPLMN-List-r13");
      for (const auto& e1 : c.get<ab_per_plmn_list_r13_l_>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "sib_type14_nb_r13_s::ab_param_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE sib_type14_nb_r13_s::ab_param_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ab_common_r13:
      HANDLE_CODE(c.get<ab_cfg_nb_r13_s>().pack(bref));
      break;
    case types::ab_per_plmn_list_r13:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ab_per_plmn_list_r13_l_>(), 1, 6));
      break;
    default:
      log_invalid_choice_id(type_, "sib_type14_nb_r13_s::ab_param_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type14_nb_r13_s::ab_param_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ab_common_r13:
      HANDLE_CODE(c.get<ab_cfg_nb_r13_s>().unpack(bref));
      break;
    case types::ab_per_plmn_list_r13:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ab_per_plmn_list_r13_l_>(), bref, 1, 6));
      break;
    default:
      log_invalid_choice_id(type_, "sib_type14_nb_r13_s::ab_param_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string sib_type14_nb_r13_s::ab_param_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"ab-Common-r13", "ab-PerPLMN-List-r13"};
  return convert_enum_idx(options, 2, value, "sib_type14_nb_r13_s::ab_param_r13_c_::types");
}

std::string sib_type14_nb_r13_s::ab_per_nrsrp_r15_opts::to_string() const
{
  static const char* options[] = {"thresh1", "thresh2"};
  return convert_enum_idx(options, 2, value, "sib_type14_nb_r13_s::ab_per_nrsrp_r15_e_");
}
uint8_t sib_type14_nb_r13_s::ab_per_nrsrp_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "sib_type14_nb_r13_s::ab_per_nrsrp_r15_e_");
}

// SystemInformationBlockType15-NB-r14 ::= SEQUENCE
SRSASN_CODE sib_type15_nb_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mbms_sai_intra_freq_r14_present, 1));
  HANDLE_CODE(bref.pack(mbms_sai_inter_freq_list_r14_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (mbms_sai_intra_freq_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbms_sai_intra_freq_r14, 1, 64, integer_packer<uint32_t>(0, 65535)));
  }
  if (mbms_sai_inter_freq_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbms_sai_inter_freq_list_r14, 1, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type15_nb_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(mbms_sai_intra_freq_r14_present, 1));
  HANDLE_CODE(bref.unpack(mbms_sai_inter_freq_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (mbms_sai_intra_freq_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbms_sai_intra_freq_r14, bref, 1, 64, integer_packer<uint32_t>(0, 65535)));
  }
  if (mbms_sai_inter_freq_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbms_sai_inter_freq_list_r14, bref, 1, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type15_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbms_sai_intra_freq_r14_present) {
    j.start_array("mbms-SAI-IntraFreq-r14");
    for (const auto& e1 : mbms_sai_intra_freq_r14) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (mbms_sai_inter_freq_list_r14_present) {
    j.start_array("mbms-SAI-InterFreqList-r14");
    for (const auto& e1 : mbms_sai_inter_freq_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SystemInformationBlockType2-NB-r13 ::= SEQUENCE
SRSASN_CODE sib_type2_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(multi_band_info_list_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  HANDLE_CODE(rr_cfg_common_r13.pack(bref));
  HANDLE_CODE(ue_timers_and_consts_r13.pack(bref));
  HANDLE_CODE(bref.pack(freq_info_r13.ul_carrier_freq_r13_present, 1));
  if (freq_info_r13.ul_carrier_freq_r13_present) {
    HANDLE_CODE(freq_info_r13.ul_carrier_freq_r13.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, freq_info_r13.add_spec_emission_r13, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(time_align_timer_common_r13.pack(bref));
  if (multi_band_info_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_r13, 1, 8, integer_packer<uint8_t>(1, 32)));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= cp_reest_r14_present;
    group_flags[1] |= serving_cell_meas_info_r14_present;
    group_flags[1] |= cqi_report_r14_present;
    group_flags[2] |= enhanced_phr_r15_present;
    group_flags[2] |= freq_info_v1530.is_present();
    group_flags[2] |= cp_edt_r15_present;
    group_flags[2] |= up_edt_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cp_reest_r14_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(serving_cell_meas_info_r14_present, 1));
      HANDLE_CODE(bref.pack(cqi_report_r14_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(enhanced_phr_r15_present, 1));
      HANDLE_CODE(bref.pack(freq_info_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(cp_edt_r15_present, 1));
      HANDLE_CODE(bref.pack(up_edt_r15_present, 1));
      if (freq_info_v1530.is_present()) {
        HANDLE_CODE(freq_info_v1530->tdd_ul_dl_align_offset_r15.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type2_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(multi_band_info_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(rr_cfg_common_r13.unpack(bref));
  HANDLE_CODE(ue_timers_and_consts_r13.unpack(bref));
  HANDLE_CODE(bref.unpack(freq_info_r13.ul_carrier_freq_r13_present, 1));
  if (freq_info_r13.ul_carrier_freq_r13_present) {
    HANDLE_CODE(freq_info_r13.ul_carrier_freq_r13.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(freq_info_r13.add_spec_emission_r13, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(time_align_timer_common_r13.unpack(bref));
  if (multi_band_info_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_r13, bref, 1, 8, integer_packer<uint8_t>(1, 32)));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(cp_reest_r14_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(serving_cell_meas_info_r14_present, 1));
      HANDLE_CODE(bref.unpack(cqi_report_r14_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(enhanced_phr_r15_present, 1));
      bool freq_info_v1530_present;
      HANDLE_CODE(bref.unpack(freq_info_v1530_present, 1));
      freq_info_v1530.set_present(freq_info_v1530_present);
      HANDLE_CODE(bref.unpack(cp_edt_r15_present, 1));
      HANDLE_CODE(bref.unpack(up_edt_r15_present, 1));
      if (freq_info_v1530.is_present()) {
        HANDLE_CODE(freq_info_v1530->tdd_ul_dl_align_offset_r15.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type2_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("radioResourceConfigCommon-r13");
  rr_cfg_common_r13.to_json(j);
  j.write_fieldname("ue-TimersAndConstants-r13");
  ue_timers_and_consts_r13.to_json(j);
  j.write_fieldname("freqInfo-r13");
  j.start_obj();
  if (freq_info_r13.ul_carrier_freq_r13_present) {
    j.write_fieldname("ul-CarrierFreq-r13");
    freq_info_r13.ul_carrier_freq_r13.to_json(j);
  }
  j.write_int("additionalSpectrumEmission-r13", freq_info_r13.add_spec_emission_r13);
  j.end_obj();
  j.write_str("timeAlignmentTimerCommon-r13", time_align_timer_common_r13.to_string());
  if (multi_band_info_list_r13_present) {
    j.start_array("multiBandInfoList-r13");
    for (const auto& e1 : multi_band_info_list_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (cp_reest_r14_present) {
      j.write_str("cp-Reestablishment-r14", "true");
    }
    if (serving_cell_meas_info_r14_present) {
      j.write_str("servingCellMeasInfo-r14", "true");
    }
    if (cqi_report_r14_present) {
      j.write_str("cqi-Reporting-r14", "true");
    }
    if (enhanced_phr_r15_present) {
      j.write_str("enhancedPHR-r15", "true");
    }
    if (freq_info_v1530.is_present()) {
      j.write_fieldname("freqInfo-v1530");
      j.start_obj();
      j.write_str("tdd-UL-DL-AlignmentOffset-r15", freq_info_v1530->tdd_ul_dl_align_offset_r15.to_string());
      j.end_obj();
    }
    if (cp_edt_r15_present) {
      j.write_str("cp-EDT-r15", "true");
    }
    if (up_edt_r15_present) {
      j.write_str("up-EDT-r15", "true");
    }
  }
  j.end_obj();
}

// SystemInformationBlockType20-NB-r14 ::= SEQUENCE
SRSASN_CODE sib_type20_nb_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sc_mcch_sched_info_r14_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  HANDLE_CODE(npdcch_sc_mcch_cfg_r14.pack(bref));
  HANDLE_CODE(sc_mcch_carrier_cfg_r14.pack(bref));
  HANDLE_CODE(sc_mcch_repeat_period_r14.pack(bref));
  HANDLE_CODE(pack_integer(bref, sc_mcch_offset_r14, (uint8_t)0u, (uint8_t)10u));
  HANDLE_CODE(sc_mcch_mod_period_r14.pack(bref));
  if (sc_mcch_sched_info_r14_present) {
    HANDLE_CODE(sc_mcch_sched_info_r14.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type20_nb_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sc_mcch_sched_info_r14_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(npdcch_sc_mcch_cfg_r14.unpack(bref));
  HANDLE_CODE(sc_mcch_carrier_cfg_r14.unpack(bref));
  HANDLE_CODE(sc_mcch_repeat_period_r14.unpack(bref));
  HANDLE_CODE(unpack_integer(sc_mcch_offset_r14, bref, (uint8_t)0u, (uint8_t)10u));
  HANDLE_CODE(sc_mcch_mod_period_r14.unpack(bref));
  if (sc_mcch_sched_info_r14_present) {
    HANDLE_CODE(sc_mcch_sched_info_r14.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type20_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("npdcch-SC-MCCH-Config-r14");
  npdcch_sc_mcch_cfg_r14.to_json(j);
  j.write_fieldname("sc-mcch-CarrierConfig-r14");
  sc_mcch_carrier_cfg_r14.to_json(j);
  j.write_str("sc-mcch-RepetitionPeriod-r14", sc_mcch_repeat_period_r14.to_string());
  j.write_int("sc-mcch-Offset-r14", sc_mcch_offset_r14);
  j.write_str("sc-mcch-ModificationPeriod-r14", sc_mcch_mod_period_r14.to_string());
  if (sc_mcch_sched_info_r14_present) {
    j.write_fieldname("sc-mcch-SchedulingInfo-r14");
    sc_mcch_sched_info_r14.to_json(j);
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

void sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_::destroy_()
{
  switch (type_) {
    case types::dl_carrier_cfg_r14:
      c.destroy<dl_carrier_cfg_common_nb_r14_s>();
      break;
    default:
      break;
  }
}
void sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::dl_carrier_cfg_r14:
      c.init<dl_carrier_cfg_common_nb_r14_s>();
      break;
    case types::dl_carrier_idx_r14:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_");
  }
}
sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_::sc_mcch_carrier_cfg_r14_c_(
    const sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::dl_carrier_cfg_r14:
      c.init(other.c.get<dl_carrier_cfg_common_nb_r14_s>());
      break;
    case types::dl_carrier_idx_r14:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_");
  }
}
sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_& sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_::
                                                 operator=(const sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::dl_carrier_cfg_r14:
      c.set(other.c.get<dl_carrier_cfg_common_nb_r14_s>());
      break;
    case types::dl_carrier_idx_r14:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_");
  }

  return *this;
}
void sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::dl_carrier_cfg_r14:
      j.write_fieldname("dl-CarrierConfig-r14");
      c.get<dl_carrier_cfg_common_nb_r14_s>().to_json(j);
      break;
    case types::dl_carrier_idx_r14:
      j.write_int("dl-CarrierIndex-r14", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::dl_carrier_cfg_r14:
      HANDLE_CODE(c.get<dl_carrier_cfg_common_nb_r14_s>().pack(bref));
      break;
    case types::dl_carrier_idx_r14:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    default:
      log_invalid_choice_id(type_, "sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::dl_carrier_cfg_r14:
      HANDLE_CODE(c.get<dl_carrier_cfg_common_nb_r14_s>().unpack(bref));
      break;
    case types::dl_carrier_idx_r14:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    default:
      log_invalid_choice_id(type_, "sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"dl-CarrierConfig-r14", "dl-CarrierIndex-r14"};
  return convert_enum_idx(options, 2, value, "sib_type20_nb_r14_s::sc_mcch_carrier_cfg_r14_c_::types");
}

std::string sib_type20_nb_r14_s::sc_mcch_repeat_period_r14_opts::to_string() const
{
  static const char* options[] = {"rf32", "rf128", "rf512", "rf1024", "rf2048", "rf4096", "rf8192", "rf16384"};
  return convert_enum_idx(options, 8, value, "sib_type20_nb_r14_s::sc_mcch_repeat_period_r14_e_");
}
uint16_t sib_type20_nb_r14_s::sc_mcch_repeat_period_r14_opts::to_number() const
{
  static const uint16_t options[] = {32, 128, 512, 1024, 2048, 4096, 8192, 16384};
  return map_enum_number(options, 8, value, "sib_type20_nb_r14_s::sc_mcch_repeat_period_r14_e_");
}

std::string sib_type20_nb_r14_s::sc_mcch_mod_period_r14_opts::to_string() const
{
  static const char* options[] = {"rf32",
                                  "rf128",
                                  "rf256",
                                  "rf512",
                                  "rf1024",
                                  "rf2048",
                                  "rf4096",
                                  "rf8192",
                                  "rf16384",
                                  "rf32768",
                                  "rf65536",
                                  "rf131072",
                                  "rf262144",
                                  "rf524288",
                                  "rf1048576",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sib_type20_nb_r14_s::sc_mcch_mod_period_r14_e_");
}
uint32_t sib_type20_nb_r14_s::sc_mcch_mod_period_r14_opts::to_number() const
{
  static const uint32_t options[] = {
      32, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576};
  return map_enum_number(options, 15, value, "sib_type20_nb_r14_s::sc_mcch_mod_period_r14_e_");
}

// SystemInformationBlockType22-NB-r14 ::= SEQUENCE
SRSASN_CODE sib_type22_nb_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dl_cfg_list_r14_present, 1));
  HANDLE_CODE(bref.pack(ul_cfg_list_r14_present, 1));
  HANDLE_CODE(bref.pack(paging_weight_anchor_r14_present, 1));
  HANDLE_CODE(bref.pack(nprach_probability_anchor_list_r14_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (dl_cfg_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, dl_cfg_list_r14, 1, 15));
  }
  if (ul_cfg_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_cfg_list_r14, 1, 15));
  }
  if (paging_weight_anchor_r14_present) {
    HANDLE_CODE(paging_weight_anchor_r14.pack(bref));
  }
  if (nprach_probability_anchor_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, nprach_probability_anchor_list_r14, 1, 3));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= mixed_operation_mode_cfg_r15.is_present();
    group_flags[0] |= ul_cfg_list_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mixed_operation_mode_cfg_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_cfg_list_r15.is_present(), 1));
      if (mixed_operation_mode_cfg_r15.is_present()) {
        HANDLE_CODE(bref.pack(mixed_operation_mode_cfg_r15->dl_cfg_list_mixed_r15_present, 1));
        HANDLE_CODE(bref.pack(mixed_operation_mode_cfg_r15->ul_cfg_list_mixed_r15_present, 1));
        HANDLE_CODE(bref.pack(mixed_operation_mode_cfg_r15->paging_distribution_r15_present, 1));
        HANDLE_CODE(bref.pack(mixed_operation_mode_cfg_r15->nprach_distribution_r15_present, 1));
        if (mixed_operation_mode_cfg_r15->dl_cfg_list_mixed_r15_present) {
          HANDLE_CODE(pack_dyn_seq_of(bref, mixed_operation_mode_cfg_r15->dl_cfg_list_mixed_r15, 1, 15));
        }
        if (mixed_operation_mode_cfg_r15->ul_cfg_list_mixed_r15_present) {
          HANDLE_CODE(pack_dyn_seq_of(bref, mixed_operation_mode_cfg_r15->ul_cfg_list_mixed_r15, 1, 15));
        }
      }
      if (ul_cfg_list_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *ul_cfg_list_r15, 1, 15));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type22_nb_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dl_cfg_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(ul_cfg_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(paging_weight_anchor_r14_present, 1));
  HANDLE_CODE(bref.unpack(nprach_probability_anchor_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (dl_cfg_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(dl_cfg_list_r14, bref, 1, 15));
  }
  if (ul_cfg_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_cfg_list_r14, bref, 1, 15));
  }
  if (paging_weight_anchor_r14_present) {
    HANDLE_CODE(paging_weight_anchor_r14.unpack(bref));
  }
  if (nprach_probability_anchor_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(nprach_probability_anchor_list_r14, bref, 1, 3));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mixed_operation_mode_cfg_r15_present;
      HANDLE_CODE(bref.unpack(mixed_operation_mode_cfg_r15_present, 1));
      mixed_operation_mode_cfg_r15.set_present(mixed_operation_mode_cfg_r15_present);
      bool ul_cfg_list_r15_present;
      HANDLE_CODE(bref.unpack(ul_cfg_list_r15_present, 1));
      ul_cfg_list_r15.set_present(ul_cfg_list_r15_present);
      if (mixed_operation_mode_cfg_r15.is_present()) {
        HANDLE_CODE(bref.unpack(mixed_operation_mode_cfg_r15->dl_cfg_list_mixed_r15_present, 1));
        HANDLE_CODE(bref.unpack(mixed_operation_mode_cfg_r15->ul_cfg_list_mixed_r15_present, 1));
        HANDLE_CODE(bref.unpack(mixed_operation_mode_cfg_r15->paging_distribution_r15_present, 1));
        HANDLE_CODE(bref.unpack(mixed_operation_mode_cfg_r15->nprach_distribution_r15_present, 1));
        if (mixed_operation_mode_cfg_r15->dl_cfg_list_mixed_r15_present) {
          HANDLE_CODE(unpack_dyn_seq_of(mixed_operation_mode_cfg_r15->dl_cfg_list_mixed_r15, bref, 1, 15));
        }
        if (mixed_operation_mode_cfg_r15->ul_cfg_list_mixed_r15_present) {
          HANDLE_CODE(unpack_dyn_seq_of(mixed_operation_mode_cfg_r15->ul_cfg_list_mixed_r15, bref, 1, 15));
        }
      }
      if (ul_cfg_list_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*ul_cfg_list_r15, bref, 1, 15));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type22_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_cfg_list_r14_present) {
    j.start_array("dl-ConfigList-r14");
    for (const auto& e1 : dl_cfg_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ul_cfg_list_r14_present) {
    j.start_array("ul-ConfigList-r14");
    for (const auto& e1 : ul_cfg_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (paging_weight_anchor_r14_present) {
    j.write_str("pagingWeightAnchor-r14", paging_weight_anchor_r14.to_string());
  }
  if (nprach_probability_anchor_list_r14_present) {
    j.start_array("nprach-ProbabilityAnchorList-r14");
    for (const auto& e1 : nprach_probability_anchor_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (mixed_operation_mode_cfg_r15.is_present()) {
      j.write_fieldname("mixedOperationModeConfig-r15");
      j.start_obj();
      if (mixed_operation_mode_cfg_r15->dl_cfg_list_mixed_r15_present) {
        j.start_array("dl-ConfigListMixed-r15");
        for (const auto& e1 : mixed_operation_mode_cfg_r15->dl_cfg_list_mixed_r15) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (mixed_operation_mode_cfg_r15->ul_cfg_list_mixed_r15_present) {
        j.start_array("ul-ConfigListMixed-r15");
        for (const auto& e1 : mixed_operation_mode_cfg_r15->ul_cfg_list_mixed_r15) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (mixed_operation_mode_cfg_r15->paging_distribution_r15_present) {
        j.write_str("pagingDistribution-r15", "true");
      }
      if (mixed_operation_mode_cfg_r15->nprach_distribution_r15_present) {
        j.write_str("nprach-Distribution-r15", "true");
      }
      j.end_obj();
    }
    if (ul_cfg_list_r15.is_present()) {
      j.start_array("ul-ConfigList-r15");
      for (const auto& e1 : *ul_cfg_list_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SystemInformationBlockType23-NB-r15 ::= SEQUENCE
SRSASN_CODE sib_type23_nb_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_cfg_list_v1530_present, 1));
  HANDLE_CODE(bref.pack(ul_cfg_list_mixed_v1530_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (ul_cfg_list_v1530_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_cfg_list_v1530, 1, 15));
  }
  if (ul_cfg_list_mixed_v1530_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_cfg_list_mixed_v1530, 1, 15));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type23_nb_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ul_cfg_list_v1530_present, 1));
  HANDLE_CODE(bref.unpack(ul_cfg_list_mixed_v1530_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (ul_cfg_list_v1530_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_cfg_list_v1530, bref, 1, 15));
  }
  if (ul_cfg_list_mixed_v1530_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_cfg_list_mixed_v1530, bref, 1, 15));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type23_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_cfg_list_v1530_present) {
    j.start_array("ul-ConfigList-v1530");
    for (const auto& e1 : ul_cfg_list_v1530) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ul_cfg_list_mixed_v1530_present) {
    j.start_array("ul-ConfigListMixed-v1530");
    for (const auto& e1 : ul_cfg_list_mixed_v1530) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SystemInformationBlockType3-NB-r13 ::= SEQUENCE
SRSASN_CODE sib_type3_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_band_info_r13_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  HANDLE_CODE(cell_resel_info_common_r13.q_hyst_r13.pack(bref));
  HANDLE_CODE(pack_integer(bref, cell_resel_serving_freq_info_r13.s_non_intra_search_r13, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(bref.pack(intra_freq_cell_resel_info_r13.q_qual_min_r13_present, 1));
  HANDLE_CODE(bref.pack(intra_freq_cell_resel_info_r13.p_max_r13_present, 1));
  HANDLE_CODE(pack_integer(bref, intra_freq_cell_resel_info_r13.q_rx_lev_min_r13, (int8_t)-70, (int8_t)-22));
  if (intra_freq_cell_resel_info_r13.q_qual_min_r13_present) {
    HANDLE_CODE(pack_integer(bref, intra_freq_cell_resel_info_r13.q_qual_min_r13, (int8_t)-34, (int8_t)-3));
  }
  if (intra_freq_cell_resel_info_r13.p_max_r13_present) {
    HANDLE_CODE(pack_integer(bref, intra_freq_cell_resel_info_r13.p_max_r13, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(pack_integer(bref, intra_freq_cell_resel_info_r13.s_intra_search_p_r13, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(intra_freq_cell_resel_info_r13.t_resel_r13.pack(bref));
  if (freq_band_info_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_info_r13, 1, 4));
  }
  if (multi_band_info_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_r13, 1, 8, SeqOfPacker<Packer>(1, 4, Packer())));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= intra_freq_cell_resel_info_v1350.is_present();
    group_flags[1] |= intra_freq_cell_resel_info_v1360.is_present();
    group_flags[2] |= intra_freq_cell_resel_info_v1430.is_present();
    group_flags[3] |= cell_resel_info_common_v1450.is_present();
    group_flags[4] |= nsss_rrm_cfg_r15.is_present();
    group_flags[4] |= npbch_rrm_cfg_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(intra_freq_cell_resel_info_v1350.is_present(), 1));
      if (intra_freq_cell_resel_info_v1350.is_present()) {
        HANDLE_CODE(intra_freq_cell_resel_info_v1350->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(intra_freq_cell_resel_info_v1360.is_present(), 1));
      if (intra_freq_cell_resel_info_v1360.is_present()) {
        HANDLE_CODE(intra_freq_cell_resel_info_v1360->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(intra_freq_cell_resel_info_v1430.is_present(), 1));
      if (intra_freq_cell_resel_info_v1430.is_present()) {
        HANDLE_CODE(intra_freq_cell_resel_info_v1430->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cell_resel_info_common_v1450.is_present(), 1));
      if (cell_resel_info_common_v1450.is_present()) {
        HANDLE_CODE(cell_resel_info_common_v1450->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(nsss_rrm_cfg_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(npbch_rrm_cfg_r15_present, 1));
      if (nsss_rrm_cfg_r15.is_present()) {
        HANDLE_CODE(nsss_rrm_cfg_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type3_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(freq_band_info_r13_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(cell_resel_info_common_r13.q_hyst_r13.unpack(bref));
  HANDLE_CODE(unpack_integer(cell_resel_serving_freq_info_r13.s_non_intra_search_r13, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(bref.unpack(intra_freq_cell_resel_info_r13.q_qual_min_r13_present, 1));
  HANDLE_CODE(bref.unpack(intra_freq_cell_resel_info_r13.p_max_r13_present, 1));
  HANDLE_CODE(unpack_integer(intra_freq_cell_resel_info_r13.q_rx_lev_min_r13, bref, (int8_t)-70, (int8_t)-22));
  if (intra_freq_cell_resel_info_r13.q_qual_min_r13_present) {
    HANDLE_CODE(unpack_integer(intra_freq_cell_resel_info_r13.q_qual_min_r13, bref, (int8_t)-34, (int8_t)-3));
  }
  if (intra_freq_cell_resel_info_r13.p_max_r13_present) {
    HANDLE_CODE(unpack_integer(intra_freq_cell_resel_info_r13.p_max_r13, bref, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(unpack_integer(intra_freq_cell_resel_info_r13.s_intra_search_p_r13, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(intra_freq_cell_resel_info_r13.t_resel_r13.unpack(bref));
  if (freq_band_info_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_info_r13, bref, 1, 4));
  }
  if (multi_band_info_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_r13, bref, 1, 8, SeqOfPacker<Packer>(1, 4, Packer())));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(5);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool intra_freq_cell_resel_info_v1350_present;
      HANDLE_CODE(bref.unpack(intra_freq_cell_resel_info_v1350_present, 1));
      intra_freq_cell_resel_info_v1350.set_present(intra_freq_cell_resel_info_v1350_present);
      if (intra_freq_cell_resel_info_v1350.is_present()) {
        HANDLE_CODE(intra_freq_cell_resel_info_v1350->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool intra_freq_cell_resel_info_v1360_present;
      HANDLE_CODE(bref.unpack(intra_freq_cell_resel_info_v1360_present, 1));
      intra_freq_cell_resel_info_v1360.set_present(intra_freq_cell_resel_info_v1360_present);
      if (intra_freq_cell_resel_info_v1360.is_present()) {
        HANDLE_CODE(intra_freq_cell_resel_info_v1360->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool intra_freq_cell_resel_info_v1430_present;
      HANDLE_CODE(bref.unpack(intra_freq_cell_resel_info_v1430_present, 1));
      intra_freq_cell_resel_info_v1430.set_present(intra_freq_cell_resel_info_v1430_present);
      if (intra_freq_cell_resel_info_v1430.is_present()) {
        HANDLE_CODE(intra_freq_cell_resel_info_v1430->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cell_resel_info_common_v1450_present;
      HANDLE_CODE(bref.unpack(cell_resel_info_common_v1450_present, 1));
      cell_resel_info_common_v1450.set_present(cell_resel_info_common_v1450_present);
      if (cell_resel_info_common_v1450.is_present()) {
        HANDLE_CODE(cell_resel_info_common_v1450->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool nsss_rrm_cfg_r15_present;
      HANDLE_CODE(bref.unpack(nsss_rrm_cfg_r15_present, 1));
      nsss_rrm_cfg_r15.set_present(nsss_rrm_cfg_r15_present);
      HANDLE_CODE(bref.unpack(npbch_rrm_cfg_r15_present, 1));
      if (nsss_rrm_cfg_r15.is_present()) {
        HANDLE_CODE(nsss_rrm_cfg_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type3_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellReselectionInfoCommon-r13");
  j.start_obj();
  j.write_str("q-Hyst-r13", cell_resel_info_common_r13.q_hyst_r13.to_string());
  j.end_obj();
  j.write_fieldname("cellReselectionServingFreqInfo-r13");
  j.start_obj();
  j.write_int("s-NonIntraSearch-r13", cell_resel_serving_freq_info_r13.s_non_intra_search_r13);
  j.end_obj();
  j.write_fieldname("intraFreqCellReselectionInfo-r13");
  j.start_obj();
  j.write_int("q-RxLevMin-r13", intra_freq_cell_resel_info_r13.q_rx_lev_min_r13);
  if (intra_freq_cell_resel_info_r13.q_qual_min_r13_present) {
    j.write_int("q-QualMin-r13", intra_freq_cell_resel_info_r13.q_qual_min_r13);
  }
  if (intra_freq_cell_resel_info_r13.p_max_r13_present) {
    j.write_int("p-Max-r13", intra_freq_cell_resel_info_r13.p_max_r13);
  }
  j.write_int("s-IntraSearchP-r13", intra_freq_cell_resel_info_r13.s_intra_search_p_r13);
  j.write_str("t-Reselection-r13", intra_freq_cell_resel_info_r13.t_resel_r13.to_string());
  j.end_obj();
  if (freq_band_info_r13_present) {
    j.start_array("freqBandInfo-r13");
    for (const auto& e1 : freq_band_info_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (multi_band_info_list_r13_present) {
    j.start_array("multiBandInfoList-r13");
    for (const auto& e1 : multi_band_info_list_r13) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
      }
      j.end_array();
    }
    j.end_array();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (intra_freq_cell_resel_info_v1350.is_present()) {
      j.write_fieldname("intraFreqCellReselectionInfo-v1350");
      intra_freq_cell_resel_info_v1350->to_json(j);
    }
    if (intra_freq_cell_resel_info_v1360.is_present()) {
      j.write_fieldname("intraFreqCellReselectionInfo-v1360");
      intra_freq_cell_resel_info_v1360->to_json(j);
    }
    if (intra_freq_cell_resel_info_v1430.is_present()) {
      j.write_fieldname("intraFreqCellReselectionInfo-v1430");
      intra_freq_cell_resel_info_v1430->to_json(j);
    }
    if (cell_resel_info_common_v1450.is_present()) {
      j.write_fieldname("cellReselectionInfoCommon-v1450");
      cell_resel_info_common_v1450->to_json(j);
    }
    if (nsss_rrm_cfg_r15.is_present()) {
      j.write_fieldname("nsss-RRM-Config-r15");
      nsss_rrm_cfg_r15->to_json(j);
    }
    if (npbch_rrm_cfg_r15_present) {
      j.write_str("npbch-RRM-Config-r15", "enabled");
    }
  }
  j.end_obj();
}

std::string sib_type3_nb_r13_s::cell_resel_info_common_r13_s_::q_hyst_r13_opts::to_string() const
{
  static const char* options[] = {"dB0",
                                  "dB1",
                                  "dB2",
                                  "dB3",
                                  "dB4",
                                  "dB5",
                                  "dB6",
                                  "dB8",
                                  "dB10",
                                  "dB12",
                                  "dB14",
                                  "dB16",
                                  "dB18",
                                  "dB20",
                                  "dB22",
                                  "dB24"};
  return convert_enum_idx(options, 16, value, "sib_type3_nb_r13_s::cell_resel_info_common_r13_s_::q_hyst_r13_e_");
}
uint8_t sib_type3_nb_r13_s::cell_resel_info_common_r13_s_::q_hyst_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(options, 16, value, "sib_type3_nb_r13_s::cell_resel_info_common_r13_s_::q_hyst_r13_e_");
}

// SystemInformationBlockType4-NB-r13 ::= SEQUENCE
SRSASN_CODE sib_type4_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(intra_freq_neigh_cell_list_r13_present, 1));
  HANDLE_CODE(bref.pack(intra_freq_black_cell_list_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (intra_freq_neigh_cell_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, intra_freq_neigh_cell_list_r13, 1, 16));
  }
  if (intra_freq_black_cell_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, intra_freq_black_cell_list_r13, 1, 16));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= nsss_rrm_cfg_r15.is_present();
    group_flags[0] |= intra_freq_neigh_cell_list_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(nsss_rrm_cfg_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(intra_freq_neigh_cell_list_v1530.is_present(), 1));
      if (nsss_rrm_cfg_r15.is_present()) {
        HANDLE_CODE(nsss_rrm_cfg_r15->pack(bref));
      }
      if (intra_freq_neigh_cell_list_v1530.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_freq_neigh_cell_list_v1530, 1, 16));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type4_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(intra_freq_neigh_cell_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(intra_freq_black_cell_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (intra_freq_neigh_cell_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(intra_freq_neigh_cell_list_r13, bref, 1, 16));
  }
  if (intra_freq_black_cell_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(intra_freq_black_cell_list_r13, bref, 1, 16));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool nsss_rrm_cfg_r15_present;
      HANDLE_CODE(bref.unpack(nsss_rrm_cfg_r15_present, 1));
      nsss_rrm_cfg_r15.set_present(nsss_rrm_cfg_r15_present);
      bool intra_freq_neigh_cell_list_v1530_present;
      HANDLE_CODE(bref.unpack(intra_freq_neigh_cell_list_v1530_present, 1));
      intra_freq_neigh_cell_list_v1530.set_present(intra_freq_neigh_cell_list_v1530_present);
      if (nsss_rrm_cfg_r15.is_present()) {
        HANDLE_CODE(nsss_rrm_cfg_r15->unpack(bref));
      }
      if (intra_freq_neigh_cell_list_v1530.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_freq_neigh_cell_list_v1530, bref, 1, 16));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type4_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (intra_freq_neigh_cell_list_r13_present) {
    j.start_array("intraFreqNeighCellList-r13");
    for (const auto& e1 : intra_freq_neigh_cell_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (intra_freq_black_cell_list_r13_present) {
    j.start_array("intraFreqBlackCellList-r13");
    for (const auto& e1 : intra_freq_black_cell_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (nsss_rrm_cfg_r15.is_present()) {
      j.write_fieldname("nsss-RRM-Config-r15");
      nsss_rrm_cfg_r15->to_json(j);
    }
    if (intra_freq_neigh_cell_list_v1530.is_present()) {
      j.start_array("intraFreqNeighCellList-v1530");
      for (const auto& e1 : *intra_freq_neigh_cell_list_v1530) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SystemInformationBlockType5-NB-r13 ::= SEQUENCE
SRSASN_CODE sib_type5_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_carrier_freq_list_r13, 1, 8));
  HANDLE_CODE(t_resel_r13.pack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= scptm_freq_offset_r14_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(scptm_freq_offset_r14_present, 1));
      if (scptm_freq_offset_r14_present) {
        HANDLE_CODE(pack_integer(bref, scptm_freq_offset_r14, (uint8_t)1u, (uint8_t)8u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type5_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(inter_freq_carrier_freq_list_r13, bref, 1, 8));
  HANDLE_CODE(t_resel_r13.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(scptm_freq_offset_r14_present, 1));
      if (scptm_freq_offset_r14_present) {
        HANDLE_CODE(unpack_integer(scptm_freq_offset_r14, bref, (uint8_t)1u, (uint8_t)8u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type5_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("interFreqCarrierFreqList-r13");
  for (const auto& e1 : inter_freq_carrier_freq_list_r13) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("t-Reselection-r13", t_resel_r13.to_string());
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (scptm_freq_offset_r14_present) {
      j.write_int("scptm-FreqOffset-r14", scptm_freq_offset_r14);
    }
  }
  j.end_obj();
}

// SystemInformation-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE sys_info_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, sib_type_and_info_r13, 1, 32));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(sib_type_and_info_r13, bref, 1, 32));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sys_info_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("sib-TypeAndInfo-r13");
  for (const auto& e1 : sib_type_and_info_r13) {
    e1.to_json(j);
  }
  j.end_array();
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

void sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_::destroy_()
{
  switch (type_) {
    case types::sib2_r13:
      c.destroy<sib_type2_nb_r13_s>();
      break;
    case types::sib3_r13:
      c.destroy<sib_type3_nb_r13_s>();
      break;
    case types::sib4_r13:
      c.destroy<sib_type4_nb_r13_s>();
      break;
    case types::sib5_r13:
      c.destroy<sib_type5_nb_r13_s>();
      break;
    case types::sib14_r13:
      c.destroy<sib_type14_nb_r13_s>();
      break;
    case types::sib16_r13:
      c.destroy<sib_type16_nb_r13_s>();
      break;
    case types::sib15_v1430:
      c.destroy<sib_type15_nb_r14_s>();
      break;
    case types::sib20_v1430:
      c.destroy<sib_type20_nb_r14_s>();
      break;
    case types::sib22_v1430:
      c.destroy<sib_type22_nb_r14_s>();
      break;
    case types::sib23_v1530:
      c.destroy<sib_type23_nb_r15_s>();
      break;
    default:
      break;
  }
}
void sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sib2_r13:
      c.init<sib_type2_nb_r13_s>();
      break;
    case types::sib3_r13:
      c.init<sib_type3_nb_r13_s>();
      break;
    case types::sib4_r13:
      c.init<sib_type4_nb_r13_s>();
      break;
    case types::sib5_r13:
      c.init<sib_type5_nb_r13_s>();
      break;
    case types::sib14_r13:
      c.init<sib_type14_nb_r13_s>();
      break;
    case types::sib16_r13:
      c.init<sib_type16_nb_r13_s>();
      break;
    case types::sib15_v1430:
      c.init<sib_type15_nb_r14_s>();
      break;
    case types::sib20_v1430:
      c.init<sib_type20_nb_r14_s>();
      break;
    case types::sib22_v1430:
      c.init<sib_type22_nb_r14_s>();
      break;
    case types::sib23_v1530:
      c.init<sib_type23_nb_r15_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_");
  }
}
sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_::sib_type_and_info_r13_item_c_(
    const sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sib2_r13:
      c.init(other.c.get<sib_type2_nb_r13_s>());
      break;
    case types::sib3_r13:
      c.init(other.c.get<sib_type3_nb_r13_s>());
      break;
    case types::sib4_r13:
      c.init(other.c.get<sib_type4_nb_r13_s>());
      break;
    case types::sib5_r13:
      c.init(other.c.get<sib_type5_nb_r13_s>());
      break;
    case types::sib14_r13:
      c.init(other.c.get<sib_type14_nb_r13_s>());
      break;
    case types::sib16_r13:
      c.init(other.c.get<sib_type16_nb_r13_s>());
      break;
    case types::sib15_v1430:
      c.init(other.c.get<sib_type15_nb_r14_s>());
      break;
    case types::sib20_v1430:
      c.init(other.c.get<sib_type20_nb_r14_s>());
      break;
    case types::sib22_v1430:
      c.init(other.c.get<sib_type22_nb_r14_s>());
      break;
    case types::sib23_v1530:
      c.init(other.c.get<sib_type23_nb_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_");
  }
}
sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_& sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_::
                                                      operator=(const sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sib2_r13:
      c.set(other.c.get<sib_type2_nb_r13_s>());
      break;
    case types::sib3_r13:
      c.set(other.c.get<sib_type3_nb_r13_s>());
      break;
    case types::sib4_r13:
      c.set(other.c.get<sib_type4_nb_r13_s>());
      break;
    case types::sib5_r13:
      c.set(other.c.get<sib_type5_nb_r13_s>());
      break;
    case types::sib14_r13:
      c.set(other.c.get<sib_type14_nb_r13_s>());
      break;
    case types::sib16_r13:
      c.set(other.c.get<sib_type16_nb_r13_s>());
      break;
    case types::sib15_v1430:
      c.set(other.c.get<sib_type15_nb_r14_s>());
      break;
    case types::sib20_v1430:
      c.set(other.c.get<sib_type20_nb_r14_s>());
      break;
    case types::sib22_v1430:
      c.set(other.c.get<sib_type22_nb_r14_s>());
      break;
    case types::sib23_v1530:
      c.set(other.c.get<sib_type23_nb_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_");
  }

  return *this;
}
void sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sib2_r13:
      j.write_fieldname("sib2-r13");
      c.get<sib_type2_nb_r13_s>().to_json(j);
      break;
    case types::sib3_r13:
      j.write_fieldname("sib3-r13");
      c.get<sib_type3_nb_r13_s>().to_json(j);
      break;
    case types::sib4_r13:
      j.write_fieldname("sib4-r13");
      c.get<sib_type4_nb_r13_s>().to_json(j);
      break;
    case types::sib5_r13:
      j.write_fieldname("sib5-r13");
      c.get<sib_type5_nb_r13_s>().to_json(j);
      break;
    case types::sib14_r13:
      j.write_fieldname("sib14-r13");
      c.get<sib_type14_nb_r13_s>().to_json(j);
      break;
    case types::sib16_r13:
      j.write_fieldname("sib16-r13");
      c.get<sib_type16_nb_r13_s>().to_json(j);
      break;
    case types::sib15_v1430:
      j.write_fieldname("sib15-v1430");
      c.get<sib_type15_nb_r14_s>().to_json(j);
      break;
    case types::sib20_v1430:
      j.write_fieldname("sib20-v1430");
      c.get<sib_type20_nb_r14_s>().to_json(j);
      break;
    case types::sib22_v1430:
      j.write_fieldname("sib22-v1430");
      c.get<sib_type22_nb_r14_s>().to_json(j);
      break;
    case types::sib23_v1530:
      j.write_fieldname("sib23-v1530");
      c.get<sib_type23_nb_r15_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_");
  }
  j.end_obj();
}
SRSASN_CODE sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sib2_r13:
      HANDLE_CODE(c.get<sib_type2_nb_r13_s>().pack(bref));
      break;
    case types::sib3_r13:
      HANDLE_CODE(c.get<sib_type3_nb_r13_s>().pack(bref));
      break;
    case types::sib4_r13:
      HANDLE_CODE(c.get<sib_type4_nb_r13_s>().pack(bref));
      break;
    case types::sib5_r13:
      HANDLE_CODE(c.get<sib_type5_nb_r13_s>().pack(bref));
      break;
    case types::sib14_r13:
      HANDLE_CODE(c.get<sib_type14_nb_r13_s>().pack(bref));
      break;
    case types::sib16_r13:
      HANDLE_CODE(c.get<sib_type16_nb_r13_s>().pack(bref));
      break;
    case types::sib15_v1430: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type15_nb_r14_s>().pack(bref));
    } break;
    case types::sib20_v1430: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type20_nb_r14_s>().pack(bref));
    } break;
    case types::sib22_v1430: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type22_nb_r14_s>().pack(bref));
    } break;
    case types::sib23_v1530: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type23_nb_r15_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sib2_r13:
      HANDLE_CODE(c.get<sib_type2_nb_r13_s>().unpack(bref));
      break;
    case types::sib3_r13:
      HANDLE_CODE(c.get<sib_type3_nb_r13_s>().unpack(bref));
      break;
    case types::sib4_r13:
      HANDLE_CODE(c.get<sib_type4_nb_r13_s>().unpack(bref));
      break;
    case types::sib5_r13:
      HANDLE_CODE(c.get<sib_type5_nb_r13_s>().unpack(bref));
      break;
    case types::sib14_r13:
      HANDLE_CODE(c.get<sib_type14_nb_r13_s>().unpack(bref));
      break;
    case types::sib16_r13:
      HANDLE_CODE(c.get<sib_type16_nb_r13_s>().unpack(bref));
      break;
    case types::sib15_v1430: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type15_nb_r14_s>().unpack(bref));
    } break;
    case types::sib20_v1430: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type20_nb_r14_s>().unpack(bref));
    } break;
    case types::sib22_v1430: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type22_nb_r14_s>().unpack(bref));
    } break;
    case types::sib23_v1530: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type23_nb_r15_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_::types_opts::to_string() const
{
  static const char* options[] = {"sib2-r13",
                                  "sib3-r13",
                                  "sib4-r13",
                                  "sib5-r13",
                                  "sib14-r13",
                                  "sib16-r13",
                                  "sib15-v1430",
                                  "sib20-v1430",
                                  "sib22-v1430",
                                  "sib23-v1530"};
  return convert_enum_idx(options, 10, value, "sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_::types");
}
uint8_t sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 5, 14, 16, 15, 20, 22, 23};
  return map_enum_number(options, 10, value, "sys_info_nb_r13_ies_s::sib_type_and_info_r13_item_c_::types");
}

// SystemInformationBlockType1-NB-v1350 ::= SEQUENCE
SRSASN_CODE sib_type1_nb_v1350_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_sel_info_v1350_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (cell_sel_info_v1350_present) {
    HANDLE_CODE(cell_sel_info_v1350.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_nb_v1350_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_sel_info_v1350_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cell_sel_info_v1350_present) {
    HANDLE_CODE(cell_sel_info_v1350.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_nb_v1350_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_sel_info_v1350_present) {
    j.write_fieldname("cellSelectionInfo-v1350");
    cell_sel_info_v1350.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformation-NB ::= SEQUENCE
SRSASN_CODE sys_info_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void sys_info_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void sys_info_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::sys_info_r13:
      c.destroy<sys_info_nb_r13_ies_s>();
      break;
    default:
      break;
  }
}
void sys_info_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sys_info_r13:
      c.init<sys_info_nb_r13_ies_s>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_nb_s::crit_exts_c_");
  }
}
sys_info_nb_s::crit_exts_c_::crit_exts_c_(const sys_info_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sys_info_r13:
      c.init(other.c.get<sys_info_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_nb_s::crit_exts_c_");
  }
}
sys_info_nb_s::crit_exts_c_& sys_info_nb_s::crit_exts_c_::operator=(const sys_info_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sys_info_r13:
      c.set(other.c.get<sys_info_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_nb_s::crit_exts_c_");
  }

  return *this;
}
void sys_info_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sys_info_r13:
      j.write_fieldname("systemInformation-r13");
      c.get<sys_info_nb_r13_ies_s>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE sys_info_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sys_info_r13:
      HANDLE_CODE(c.get<sys_info_nb_r13_ies_s>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sys_info_r13:
      HANDLE_CODE(c.get<sys_info_nb_r13_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string sys_info_nb_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"systemInformation-r13", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "sys_info_nb_s::crit_exts_c_::types");
}

// SystemInformationBlockType1-NB ::= SEQUENCE
SRSASN_CODE sib_type1_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(p_max_r13_present, 1));
  HANDLE_CODE(bref.pack(freq_band_info_r13_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_r13_present, 1));
  HANDLE_CODE(bref.pack(dl_bitmap_r13_present, 1));
  HANDLE_CODE(bref.pack(eutra_ctrl_region_size_r13_present, 1));
  HANDLE_CODE(bref.pack(nrs_crs_pwr_offset_r13_present, 1));
  HANDLE_CODE(bref.pack(si_radio_frame_offset_r13_present, 1));
  HANDLE_CODE(bref.pack(sys_info_value_tag_list_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(hyper_sfn_msb_r13.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, cell_access_related_info_r13.plmn_id_list_r13, 1, 6));
  HANDLE_CODE(cell_access_related_info_r13.tac_r13.pack(bref));
  HANDLE_CODE(cell_access_related_info_r13.cell_id_r13.pack(bref));
  HANDLE_CODE(cell_access_related_info_r13.cell_barred_r13.pack(bref));
  HANDLE_CODE(cell_access_related_info_r13.intra_freq_resel_r13.pack(bref));
  HANDLE_CODE(pack_integer(bref, cell_sel_info_r13.q_rx_lev_min_r13, (int8_t)-70, (int8_t)-22));
  HANDLE_CODE(pack_integer(bref, cell_sel_info_r13.q_qual_min_r13, (int8_t)-34, (int8_t)-3));
  if (p_max_r13_present) {
    HANDLE_CODE(pack_integer(bref, p_max_r13, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(pack_integer(bref, freq_band_ind_r13, (uint16_t)1u, (uint16_t)256u));
  if (freq_band_info_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_info_r13, 1, 4));
  }
  if (multi_band_info_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_r13, 1, 8));
  }
  if (dl_bitmap_r13_present) {
    HANDLE_CODE(dl_bitmap_r13.pack(bref));
  }
  if (eutra_ctrl_region_size_r13_present) {
    HANDLE_CODE(eutra_ctrl_region_size_r13.pack(bref));
  }
  if (nrs_crs_pwr_offset_r13_present) {
    HANDLE_CODE(nrs_crs_pwr_offset_r13.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, sched_info_list_r13, 1, 8));
  HANDLE_CODE(si_win_len_r13.pack(bref));
  if (si_radio_frame_offset_r13_present) {
    HANDLE_CODE(pack_integer(bref, si_radio_frame_offset_r13, (uint8_t)1u, (uint8_t)15u));
  }
  if (sys_info_value_tag_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sys_info_value_tag_list_r13, 1, 8, integer_packer<uint8_t>(0, 3)));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(p_max_r13_present, 1));
  HANDLE_CODE(bref.unpack(freq_band_info_r13_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(dl_bitmap_r13_present, 1));
  HANDLE_CODE(bref.unpack(eutra_ctrl_region_size_r13_present, 1));
  HANDLE_CODE(bref.unpack(nrs_crs_pwr_offset_r13_present, 1));
  HANDLE_CODE(bref.unpack(si_radio_frame_offset_r13_present, 1));
  HANDLE_CODE(bref.unpack(sys_info_value_tag_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(hyper_sfn_msb_r13.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(cell_access_related_info_r13.plmn_id_list_r13, bref, 1, 6));
  HANDLE_CODE(cell_access_related_info_r13.tac_r13.unpack(bref));
  HANDLE_CODE(cell_access_related_info_r13.cell_id_r13.unpack(bref));
  HANDLE_CODE(cell_access_related_info_r13.cell_barred_r13.unpack(bref));
  HANDLE_CODE(cell_access_related_info_r13.intra_freq_resel_r13.unpack(bref));
  HANDLE_CODE(unpack_integer(cell_sel_info_r13.q_rx_lev_min_r13, bref, (int8_t)-70, (int8_t)-22));
  HANDLE_CODE(unpack_integer(cell_sel_info_r13.q_qual_min_r13, bref, (int8_t)-34, (int8_t)-3));
  if (p_max_r13_present) {
    HANDLE_CODE(unpack_integer(p_max_r13, bref, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(unpack_integer(freq_band_ind_r13, bref, (uint16_t)1u, (uint16_t)256u));
  if (freq_band_info_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_info_r13, bref, 1, 4));
  }
  if (multi_band_info_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_r13, bref, 1, 8));
  }
  if (dl_bitmap_r13_present) {
    HANDLE_CODE(dl_bitmap_r13.unpack(bref));
  }
  if (eutra_ctrl_region_size_r13_present) {
    HANDLE_CODE(eutra_ctrl_region_size_r13.unpack(bref));
  }
  if (nrs_crs_pwr_offset_r13_present) {
    HANDLE_CODE(nrs_crs_pwr_offset_r13.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(sched_info_list_r13, bref, 1, 8));
  HANDLE_CODE(si_win_len_r13.unpack(bref));
  if (si_radio_frame_offset_r13_present) {
    HANDLE_CODE(unpack_integer(si_radio_frame_offset_r13, bref, (uint8_t)1u, (uint8_t)15u));
  }
  if (sys_info_value_tag_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sys_info_value_tag_list_r13, bref, 1, 8, integer_packer<uint8_t>(0, 3)));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("hyperSFN-MSB-r13", hyper_sfn_msb_r13.to_string());
  j.write_fieldname("cellAccessRelatedInfo-r13");
  j.start_obj();
  j.start_array("plmn-IdentityList-r13");
  for (const auto& e1 : cell_access_related_info_r13.plmn_id_list_r13) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("trackingAreaCode-r13", cell_access_related_info_r13.tac_r13.to_string());
  j.write_str("cellIdentity-r13", cell_access_related_info_r13.cell_id_r13.to_string());
  j.write_str("cellBarred-r13", cell_access_related_info_r13.cell_barred_r13.to_string());
  j.write_str("intraFreqReselection-r13", cell_access_related_info_r13.intra_freq_resel_r13.to_string());
  j.end_obj();
  j.write_fieldname("cellSelectionInfo-r13");
  j.start_obj();
  j.write_int("q-RxLevMin-r13", cell_sel_info_r13.q_rx_lev_min_r13);
  j.write_int("q-QualMin-r13", cell_sel_info_r13.q_qual_min_r13);
  j.end_obj();
  if (p_max_r13_present) {
    j.write_int("p-Max-r13", p_max_r13);
  }
  j.write_int("freqBandIndicator-r13", freq_band_ind_r13);
  if (freq_band_info_r13_present) {
    j.start_array("freqBandInfo-r13");
    for (const auto& e1 : freq_band_info_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (multi_band_info_list_r13_present) {
    j.start_array("multiBandInfoList-r13");
    for (const auto& e1 : multi_band_info_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (dl_bitmap_r13_present) {
    j.write_fieldname("downlinkBitmap-r13");
    dl_bitmap_r13.to_json(j);
  }
  if (eutra_ctrl_region_size_r13_present) {
    j.write_str("eutraControlRegionSize-r13", eutra_ctrl_region_size_r13.to_string());
  }
  if (nrs_crs_pwr_offset_r13_present) {
    j.write_str("nrs-CRS-PowerOffset-r13", nrs_crs_pwr_offset_r13.to_string());
  }
  j.start_array("schedulingInfoList-r13");
  for (const auto& e1 : sched_info_list_r13) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("si-WindowLength-r13", si_win_len_r13.to_string());
  if (si_radio_frame_offset_r13_present) {
    j.write_int("si-RadioFrameOffset-r13", si_radio_frame_offset_r13);
  }
  if (sys_info_value_tag_list_r13_present) {
    j.start_array("systemInfoValueTagList-r13");
    for (const auto& e1 : sys_info_value_tag_list_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

std::string sib_type1_nb_s::cell_access_related_info_r13_s_::cell_barred_r13_opts::to_string() const
{
  static const char* options[] = {"barred", "notBarred"};
  return convert_enum_idx(options, 2, value, "sib_type1_nb_s::cell_access_related_info_r13_s_::cell_barred_r13_e_");
}

std::string sib_type1_nb_s::cell_access_related_info_r13_s_::intra_freq_resel_r13_opts::to_string() const
{
  static const char* options[] = {"allowed", "notAllowed"};
  return convert_enum_idx(
      options, 2, value, "sib_type1_nb_s::cell_access_related_info_r13_s_::intra_freq_resel_r13_e_");
}

std::string sib_type1_nb_s::eutra_ctrl_region_size_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3"};
  return convert_enum_idx(options, 3, value, "sib_type1_nb_s::eutra_ctrl_region_size_r13_e_");
}
uint8_t sib_type1_nb_s::eutra_ctrl_region_size_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(options, 3, value, "sib_type1_nb_s::eutra_ctrl_region_size_r13_e_");
}

std::string sib_type1_nb_s::nrs_crs_pwr_offset_r13_opts::to_string() const
{
  static const char* options[] = {"dB-6",
                                  "dB-4dot77",
                                  "dB-3",
                                  "dB-1dot77",
                                  "dB0",
                                  "dB1",
                                  "dB1dot23",
                                  "dB2",
                                  "dB3",
                                  "dB4",
                                  "dB4dot23",
                                  "dB5",
                                  "dB6",
                                  "dB7",
                                  "dB8",
                                  "dB9"};
  return convert_enum_idx(options, 16, value, "sib_type1_nb_s::nrs_crs_pwr_offset_r13_e_");
}
float sib_type1_nb_s::nrs_crs_pwr_offset_r13_opts::to_number() const
{
  static const float options[] = {
      -6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 1.23, 2.0, 3.0, 4.0, 4.23, 5.0, 6.0, 7.0, 8.0, 9.0};
  return map_enum_number(options, 16, value, "sib_type1_nb_s::nrs_crs_pwr_offset_r13_e_");
}
std::string sib_type1_nb_s::nrs_crs_pwr_offset_r13_opts::to_number_string() const
{
  static const char* options[] = {
      "-6", "-4.77", "-3", "-1.77", "0", "1", "1.23", "2", "3", "4", "4.23", "5", "6", "7", "8", "9"};
  return convert_enum_idx(options, 16, value, "sib_type1_nb_s::nrs_crs_pwr_offset_r13_e_");
}

std::string sib_type1_nb_s::si_win_len_r13_opts::to_string() const
{
  static const char* options[] = {"ms160", "ms320", "ms480", "ms640", "ms960", "ms1280", "ms1600", "spare1"};
  return convert_enum_idx(options, 8, value, "sib_type1_nb_s::si_win_len_r13_e_");
}
uint16_t sib_type1_nb_s::si_win_len_r13_opts::to_number() const
{
  static const uint16_t options[] = {160, 320, 480, 640, 960, 1280, 1600};
  return map_enum_number(options, 7, value, "sib_type1_nb_s::si_win_len_r13_e_");
}

// BCCH-DL-SCH-MessageType-NB ::= CHOICE
void bcch_dl_sch_msg_type_nb_c::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void bcch_dl_sch_msg_type_nb_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::msg_class_ext:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_nb_c");
  }
}
bcch_dl_sch_msg_type_nb_c::bcch_dl_sch_msg_type_nb_c(const bcch_dl_sch_msg_type_nb_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_nb_c");
  }
}
bcch_dl_sch_msg_type_nb_c& bcch_dl_sch_msg_type_nb_c::operator=(const bcch_dl_sch_msg_type_nb_c& other)
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
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_nb_c");
  }

  return *this;
}
void bcch_dl_sch_msg_type_nb_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_nb_c");
  }
  j.end_obj();
}
SRSASN_CODE bcch_dl_sch_msg_type_nb_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_nb_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_type_nb_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_nb_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void bcch_dl_sch_msg_type_nb_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::sys_info_r13:
      c.destroy<sys_info_nb_s>();
      break;
    case types::sib_type1_r13:
      c.destroy<sib_type1_nb_s>();
      break;
    default:
      break;
  }
}
void bcch_dl_sch_msg_type_nb_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sys_info_r13:
      c.init<sys_info_nb_s>();
      break;
    case types::sib_type1_r13:
      c.init<sib_type1_nb_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_nb_c::c1_c_");
  }
}
bcch_dl_sch_msg_type_nb_c::c1_c_::c1_c_(const bcch_dl_sch_msg_type_nb_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sys_info_r13:
      c.init(other.c.get<sys_info_nb_s>());
      break;
    case types::sib_type1_r13:
      c.init(other.c.get<sib_type1_nb_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_nb_c::c1_c_");
  }
}
bcch_dl_sch_msg_type_nb_c::c1_c_& bcch_dl_sch_msg_type_nb_c::c1_c_::
                                  operator=(const bcch_dl_sch_msg_type_nb_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sys_info_r13:
      c.set(other.c.get<sys_info_nb_s>());
      break;
    case types::sib_type1_r13:
      c.set(other.c.get<sib_type1_nb_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_nb_c::c1_c_");
  }

  return *this;
}
void bcch_dl_sch_msg_type_nb_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sys_info_r13:
      j.write_fieldname("systemInformation-r13");
      c.get<sys_info_nb_s>().to_json(j);
      break;
    case types::sib_type1_r13:
      j.write_fieldname("systemInformationBlockType1-r13");
      c.get<sib_type1_nb_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_nb_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE bcch_dl_sch_msg_type_nb_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sys_info_r13:
      HANDLE_CODE(c.get<sys_info_nb_s>().pack(bref));
      break;
    case types::sib_type1_r13:
      HANDLE_CODE(c.get<sib_type1_nb_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_nb_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_type_nb_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sys_info_r13:
      HANDLE_CODE(c.get<sys_info_nb_s>().unpack(bref));
      break;
    case types::sib_type1_r13:
      HANDLE_CODE(c.get<sib_type1_nb_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_nb_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string bcch_dl_sch_msg_type_nb_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"systemInformation-r13", "systemInformationBlockType1-r13"};
  return convert_enum_idx(options, 2, value, "bcch_dl_sch_msg_type_nb_c::c1_c_::types");
}
uint8_t bcch_dl_sch_msg_type_nb_c::c1_c_::types_opts::to_number() const
{
  if (value == sib_type1_r13) {
    return 1;
  }
  invalid_enum_number(value, "bcch_dl_sch_msg_type_nb_c::c1_c_::types");
  return 0;
}

std::string bcch_dl_sch_msg_type_nb_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "bcch_dl_sch_msg_type_nb_c::types");
}
uint8_t bcch_dl_sch_msg_type_nb_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "bcch_dl_sch_msg_type_nb_c::types");
}

// BCCH-DL-SCH-Message-NB ::= SEQUENCE
SRSASN_CODE bcch_dl_sch_msg_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void bcch_dl_sch_msg_nb_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("BCCH-DL-SCH-Message-NB");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// RRCConnectionReestablishment-NB-v1430-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_nb_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dl_nas_mac_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (dl_nas_mac_present) {
    HANDLE_CODE(dl_nas_mac.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_nb_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dl_nas_mac_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (dl_nas_mac_present) {
    HANDLE_CODE(dl_nas_mac.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_nb_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_nas_mac_present) {
    j.write_str("dl-NAS-MAC", dl_nas_mac.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RedirectedCarrierInfo-NB-v1430 ::= SEQUENCE
SRSASN_CODE redirected_carrier_info_nb_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(redirected_carrier_offset_ded_r14.pack(bref));
  HANDLE_CODE(t322_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE redirected_carrier_info_nb_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(redirected_carrier_offset_ded_r14.unpack(bref));
  HANDLE_CODE(t322_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void redirected_carrier_info_nb_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("redirectedCarrierOffsetDedicated-r14", redirected_carrier_offset_ded_r14.to_string());
  j.write_str("t322-r14", t322_r14.to_string());
  j.end_obj();
}

std::string redirected_carrier_info_nb_v1430_s::redirected_carrier_offset_ded_r14_opts::to_string() const
{
  static const char* options[] = {"dB1",
                                  "dB2",
                                  "dB3",
                                  "dB4",
                                  "dB5",
                                  "dB6",
                                  "dB8",
                                  "dB10",
                                  "dB12",
                                  "dB14",
                                  "dB16",
                                  "dB18",
                                  "dB20",
                                  "dB22",
                                  "dB24",
                                  "dB26"};
  return convert_enum_idx(
      options, 16, value, "redirected_carrier_info_nb_v1430_s::redirected_carrier_offset_ded_r14_e_");
}
uint8_t redirected_carrier_info_nb_v1430_s::redirected_carrier_offset_ded_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26};
  return map_enum_number(
      options, 16, value, "redirected_carrier_info_nb_v1430_s::redirected_carrier_offset_ded_r14_e_");
}

std::string redirected_carrier_info_nb_v1430_s::t322_r14_opts::to_string() const
{
  static const char* options[] = {"min5", "min10", "min20", "min30", "min60", "min120", "min180", "spare1"};
  return convert_enum_idx(options, 8, value, "redirected_carrier_info_nb_v1430_s::t322_r14_e_");
}
uint8_t redirected_carrier_info_nb_v1430_s::t322_r14_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 30, 60, 120, 180};
  return map_enum_number(options, 7, value, "redirected_carrier_info_nb_v1430_s::t322_r14_e_");
}

// RRCConnectionReestablishment-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(rr_cfg_ded_r13.pack(bref));
  HANDLE_CODE(pack_integer(bref, next_hop_chaining_count_r13, (uint8_t)0u, (uint8_t)7u));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(rr_cfg_ded_r13.unpack(bref));
  HANDLE_CODE(unpack_integer(next_hop_chaining_count_r13, bref, (uint8_t)0u, (uint8_t)7u));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("radioResourceConfigDedicated-r13");
  rr_cfg_ded_r13.to_json(j);
  j.write_int("nextHopChainingCount-r13", next_hop_chaining_count_r13);
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReject-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reject_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rrc_suspend_ind_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_integer(bref, extended_wait_time_r13, (uint16_t)1u, (uint16_t)1800u));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reject_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rrc_suspend_ind_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_integer(extended_wait_time_r13, bref, (uint16_t)1u, (uint16_t)1800u));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reject_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("extendedWaitTime-r13", extended_wait_time_r13);
  if (rrc_suspend_ind_r13_present) {
    j.write_str("rrc-SuspendIndication-r13", "true");
  }
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

// RRCConnectionSetup-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(rr_cfg_ded_r13.pack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(rr_cfg_ded_r13.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("radioResourceConfigDedicated-r13");
  rr_cfg_ded_r13.to_json(j);
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

// RRCEarlyDataComplete-NB-r15-IEs ::= SEQUENCE
SRSASN_CODE rrc_early_data_complete_nb_r15_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ded_info_nas_r15_present, 1));
  HANDLE_CODE(bref.pack(extended_wait_time_r15_present, 1));
  HANDLE_CODE(bref.pack(redirected_carrier_info_r15_present, 1));
  HANDLE_CODE(bref.pack(redirected_carrier_info_ext_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ded_info_nas_r15_present) {
    HANDLE_CODE(ded_info_nas_r15.pack(bref));
  }
  if (extended_wait_time_r15_present) {
    HANDLE_CODE(pack_integer(bref, extended_wait_time_r15, (uint16_t)1u, (uint16_t)1800u));
  }
  if (redirected_carrier_info_r15_present) {
    HANDLE_CODE(redirected_carrier_info_r15.pack(bref));
  }
  if (redirected_carrier_info_ext_r15_present) {
    HANDLE_CODE(redirected_carrier_info_ext_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_early_data_complete_nb_r15_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ded_info_nas_r15_present, 1));
  HANDLE_CODE(bref.unpack(extended_wait_time_r15_present, 1));
  HANDLE_CODE(bref.unpack(redirected_carrier_info_r15_present, 1));
  HANDLE_CODE(bref.unpack(redirected_carrier_info_ext_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ded_info_nas_r15_present) {
    HANDLE_CODE(ded_info_nas_r15.unpack(bref));
  }
  if (extended_wait_time_r15_present) {
    HANDLE_CODE(unpack_integer(extended_wait_time_r15, bref, (uint16_t)1u, (uint16_t)1800u));
  }
  if (redirected_carrier_info_r15_present) {
    HANDLE_CODE(redirected_carrier_info_r15.unpack(bref));
  }
  if (redirected_carrier_info_ext_r15_present) {
    HANDLE_CODE(redirected_carrier_info_ext_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_early_data_complete_nb_r15_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ded_info_nas_r15_present) {
    j.write_str("dedicatedInfoNAS-r15", ded_info_nas_r15.to_string());
  }
  if (extended_wait_time_r15_present) {
    j.write_int("extendedWaitTime-r15", extended_wait_time_r15);
  }
  if (redirected_carrier_info_r15_present) {
    j.write_fieldname("redirectedCarrierInfo-r15");
    redirected_carrier_info_r15.to_json(j);
  }
  if (redirected_carrier_info_ext_r15_present) {
    j.write_fieldname("redirectedCarrierInfoExt-r15");
    redirected_carrier_info_ext_r15.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RRCConnectionReestablishment-NB ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_reest_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void rrc_conn_reest_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_nb_s::crit_exts_c_");
  }
}
rrc_conn_reest_nb_s::crit_exts_c_::crit_exts_c_(const rrc_conn_reest_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_nb_s::crit_exts_c_");
  }
}
rrc_conn_reest_nb_s::crit_exts_c_& rrc_conn_reest_nb_s::crit_exts_c_::
                                   operator=(const rrc_conn_reest_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_nb_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_reest_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_reest_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_conn_reest_nb_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
void rrc_conn_reest_nb_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_reest_r13:
      j.write_fieldname("rrcConnectionReestablishment-r13");
      c.to_json(j);
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_nb_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_reest_nb_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_reest_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_nb_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_reest_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_conn_reest_nb_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishment-r13", "spare1"};
  return convert_enum_idx(options, 2, value, "rrc_conn_reest_nb_s::crit_exts_c_::c1_c_::types");
}

// RRCConnectionReject-NB ::= SEQUENCE
SRSASN_CODE rrc_conn_reject_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reject_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_reject_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_reject_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void rrc_conn_reject_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_nb_s::crit_exts_c_");
  }
}
rrc_conn_reject_nb_s::crit_exts_c_::crit_exts_c_(const rrc_conn_reject_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_nb_s::crit_exts_c_");
  }
}
rrc_conn_reject_nb_s::crit_exts_c_& rrc_conn_reject_nb_s::crit_exts_c_::
                                    operator=(const rrc_conn_reject_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_nb_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_reject_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_reject_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reject_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_conn_reject_nb_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
void rrc_conn_reject_nb_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_reject_r13:
      j.write_fieldname("rrcConnectionReject-r13");
      c.to_json(j);
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_nb_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_reject_nb_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_reject_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reject_nb_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_reject_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_conn_reject_nb_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReject-r13", "spare1"};
  return convert_enum_idx(options, 2, value, "rrc_conn_reject_nb_s::crit_exts_c_::c1_c_::types");
}

// RRCConnectionSetup-NB ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_setup_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void rrc_conn_setup_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_nb_s::crit_exts_c_");
  }
}
rrc_conn_setup_nb_s::crit_exts_c_::crit_exts_c_(const rrc_conn_setup_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_nb_s::crit_exts_c_");
  }
}
rrc_conn_setup_nb_s::crit_exts_c_& rrc_conn_setup_nb_s::crit_exts_c_::
                                   operator=(const rrc_conn_setup_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_nb_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_setup_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_setup_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_conn_setup_nb_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
void rrc_conn_setup_nb_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_setup_r13:
      j.write_fieldname("rrcConnectionSetup-r13");
      c.to_json(j);
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_nb_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_setup_nb_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_setup_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_nb_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_setup_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_conn_setup_nb_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionSetup-r13", "spare1"};
  return convert_enum_idx(options, 2, value, "rrc_conn_setup_nb_s::crit_exts_c_::c1_c_::types");
}

// RRCEarlyDataComplete-NB-r15 ::= SEQUENCE
SRSASN_CODE rrc_early_data_complete_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_early_data_complete_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_early_data_complete_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_early_data_complete_nb_r15_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_early_data_complete_r15:
      c.destroy<rrc_early_data_complete_nb_r15_ies_s>();
      break;
    default:
      break;
  }
}
void rrc_early_data_complete_nb_r15_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_early_data_complete_r15:
      c.init<rrc_early_data_complete_nb_r15_ies_s>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_complete_nb_r15_s::crit_exts_c_");
  }
}
rrc_early_data_complete_nb_r15_s::crit_exts_c_::crit_exts_c_(
    const rrc_early_data_complete_nb_r15_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_early_data_complete_r15:
      c.init(other.c.get<rrc_early_data_complete_nb_r15_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_complete_nb_r15_s::crit_exts_c_");
  }
}
rrc_early_data_complete_nb_r15_s::crit_exts_c_& rrc_early_data_complete_nb_r15_s::crit_exts_c_::
                                                operator=(const rrc_early_data_complete_nb_r15_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_early_data_complete_r15:
      c.set(other.c.get<rrc_early_data_complete_nb_r15_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_complete_nb_r15_s::crit_exts_c_");
  }

  return *this;
}
void rrc_early_data_complete_nb_r15_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_early_data_complete_r15:
      j.write_fieldname("rrcEarlyDataComplete-r15");
      c.get<rrc_early_data_complete_nb_r15_ies_s>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_complete_nb_r15_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_early_data_complete_nb_r15_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_early_data_complete_r15:
      HANDLE_CODE(c.get<rrc_early_data_complete_nb_r15_ies_s>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_complete_nb_r15_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_early_data_complete_nb_r15_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_early_data_complete_r15:
      HANDLE_CODE(c.get<rrc_early_data_complete_nb_r15_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_complete_nb_r15_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_early_data_complete_nb_r15_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcEarlyDataComplete-r15", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_early_data_complete_nb_r15_s::crit_exts_c_::types");
}

// DL-CCCH-MessageType-NB ::= CHOICE
void dl_ccch_msg_type_nb_c::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void dl_ccch_msg_type_nb_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::msg_class_ext:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_nb_c");
  }
}
dl_ccch_msg_type_nb_c::dl_ccch_msg_type_nb_c(const dl_ccch_msg_type_nb_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_nb_c");
  }
}
dl_ccch_msg_type_nb_c& dl_ccch_msg_type_nb_c::operator=(const dl_ccch_msg_type_nb_c& other)
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
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_nb_c");
  }

  return *this;
}
void dl_ccch_msg_type_nb_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_nb_c");
  }
  j.end_obj();
}
SRSASN_CODE dl_ccch_msg_type_nb_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_nb_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ccch_msg_type_nb_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_nb_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void dl_ccch_msg_type_nb_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_reest_r13:
      c.destroy<rrc_conn_reest_nb_s>();
      break;
    case types::rrc_conn_reest_reject_r13:
      c.destroy<rrc_conn_reest_reject_s>();
      break;
    case types::rrc_conn_reject_r13:
      c.destroy<rrc_conn_reject_nb_s>();
      break;
    case types::rrc_conn_setup_r13:
      c.destroy<rrc_conn_setup_nb_s>();
      break;
    case types::rrc_early_data_complete_r15:
      c.destroy<rrc_early_data_complete_nb_r15_s>();
      break;
    default:
      break;
  }
}
void dl_ccch_msg_type_nb_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_reest_r13:
      c.init<rrc_conn_reest_nb_s>();
      break;
    case types::rrc_conn_reest_reject_r13:
      c.init<rrc_conn_reest_reject_s>();
      break;
    case types::rrc_conn_reject_r13:
      c.init<rrc_conn_reject_nb_s>();
      break;
    case types::rrc_conn_setup_r13:
      c.init<rrc_conn_setup_nb_s>();
      break;
    case types::rrc_early_data_complete_r15:
      c.init<rrc_early_data_complete_nb_r15_s>();
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
      log_invalid_choice_id(type_, "dl_ccch_msg_type_nb_c::c1_c_");
  }
}
dl_ccch_msg_type_nb_c::c1_c_::c1_c_(const dl_ccch_msg_type_nb_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_reest_r13:
      c.init(other.c.get<rrc_conn_reest_nb_s>());
      break;
    case types::rrc_conn_reest_reject_r13:
      c.init(other.c.get<rrc_conn_reest_reject_s>());
      break;
    case types::rrc_conn_reject_r13:
      c.init(other.c.get<rrc_conn_reject_nb_s>());
      break;
    case types::rrc_conn_setup_r13:
      c.init(other.c.get<rrc_conn_setup_nb_s>());
      break;
    case types::rrc_early_data_complete_r15:
      c.init(other.c.get<rrc_early_data_complete_nb_r15_s>());
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
      log_invalid_choice_id(type_, "dl_ccch_msg_type_nb_c::c1_c_");
  }
}
dl_ccch_msg_type_nb_c::c1_c_& dl_ccch_msg_type_nb_c::c1_c_::operator=(const dl_ccch_msg_type_nb_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_reest_r13:
      c.set(other.c.get<rrc_conn_reest_nb_s>());
      break;
    case types::rrc_conn_reest_reject_r13:
      c.set(other.c.get<rrc_conn_reest_reject_s>());
      break;
    case types::rrc_conn_reject_r13:
      c.set(other.c.get<rrc_conn_reject_nb_s>());
      break;
    case types::rrc_conn_setup_r13:
      c.set(other.c.get<rrc_conn_setup_nb_s>());
      break;
    case types::rrc_early_data_complete_r15:
      c.set(other.c.get<rrc_early_data_complete_nb_r15_s>());
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
      log_invalid_choice_id(type_, "dl_ccch_msg_type_nb_c::c1_c_");
  }

  return *this;
}
void dl_ccch_msg_type_nb_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_reest_r13:
      j.write_fieldname("rrcConnectionReestablishment-r13");
      c.get<rrc_conn_reest_nb_s>().to_json(j);
      break;
    case types::rrc_conn_reest_reject_r13:
      j.write_fieldname("rrcConnectionReestablishmentReject-r13");
      c.get<rrc_conn_reest_reject_s>().to_json(j);
      break;
    case types::rrc_conn_reject_r13:
      j.write_fieldname("rrcConnectionReject-r13");
      c.get<rrc_conn_reject_nb_s>().to_json(j);
      break;
    case types::rrc_conn_setup_r13:
      j.write_fieldname("rrcConnectionSetup-r13");
      c.get<rrc_conn_setup_nb_s>().to_json(j);
      break;
    case types::rrc_early_data_complete_r15:
      j.write_fieldname("rrcEarlyDataComplete-r15");
      c.get<rrc_early_data_complete_nb_r15_s>().to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_nb_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_ccch_msg_type_nb_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_reest_r13:
      HANDLE_CODE(c.get<rrc_conn_reest_nb_s>().pack(bref));
      break;
    case types::rrc_conn_reest_reject_r13:
      HANDLE_CODE(c.get<rrc_conn_reest_reject_s>().pack(bref));
      break;
    case types::rrc_conn_reject_r13:
      HANDLE_CODE(c.get<rrc_conn_reject_nb_s>().pack(bref));
      break;
    case types::rrc_conn_setup_r13:
      HANDLE_CODE(c.get<rrc_conn_setup_nb_s>().pack(bref));
      break;
    case types::rrc_early_data_complete_r15:
      HANDLE_CODE(c.get<rrc_early_data_complete_nb_r15_s>().pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_nb_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ccch_msg_type_nb_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_reest_r13:
      HANDLE_CODE(c.get<rrc_conn_reest_nb_s>().unpack(bref));
      break;
    case types::rrc_conn_reest_reject_r13:
      HANDLE_CODE(c.get<rrc_conn_reest_reject_s>().unpack(bref));
      break;
    case types::rrc_conn_reject_r13:
      HANDLE_CODE(c.get<rrc_conn_reject_nb_s>().unpack(bref));
      break;
    case types::rrc_conn_setup_r13:
      HANDLE_CODE(c.get<rrc_conn_setup_nb_s>().unpack(bref));
      break;
    case types::rrc_early_data_complete_r15:
      HANDLE_CODE(c.get<rrc_early_data_complete_nb_r15_s>().unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_nb_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string dl_ccch_msg_type_nb_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishment-r13",
                                  "rrcConnectionReestablishmentReject-r13",
                                  "rrcConnectionReject-r13",
                                  "rrcConnectionSetup-r13",
                                  "rrcEarlyDataComplete-r15",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 8, value, "dl_ccch_msg_type_nb_c::c1_c_::types");
}

std::string dl_ccch_msg_type_nb_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "dl_ccch_msg_type_nb_c::types");
}
uint8_t dl_ccch_msg_type_nb_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "dl_ccch_msg_type_nb_c::types");
}

// DL-CCCH-Message-NB ::= SEQUENCE
SRSASN_CODE dl_ccch_msg_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ccch_msg_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void dl_ccch_msg_nb_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("DL-CCCH-Message-NB");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// RRCConnectionRelease-NB-v1530-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_release_nb_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(drb_continue_rohc_r15_present, 1));
  HANDLE_CODE(bref.pack(next_hop_chaining_count_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (next_hop_chaining_count_r15_present) {
    HANDLE_CODE(pack_integer(bref, next_hop_chaining_count_r15, (uint8_t)0u, (uint8_t)7u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_nb_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(drb_continue_rohc_r15_present, 1));
  HANDLE_CODE(bref.unpack(next_hop_chaining_count_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (next_hop_chaining_count_r15_present) {
    HANDLE_CODE(unpack_integer(next_hop_chaining_count_r15, bref, (uint8_t)0u, (uint8_t)7u));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_release_nb_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drb_continue_rohc_r15_present) {
    j.write_str("drb-ContinueROHC-r15", "true");
  }
  if (next_hop_chaining_count_r15_present) {
    j.write_int("nextHopChainingCount-r15", next_hop_chaining_count_r15);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RRCConnectionRelease-NB-v1430-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_release_nb_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(redirected_carrier_info_v1430_present, 1));
  HANDLE_CODE(bref.pack(extended_wait_time_cpdata_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (redirected_carrier_info_v1430_present) {
    HANDLE_CODE(redirected_carrier_info_v1430.pack(bref));
  }
  if (extended_wait_time_cpdata_r14_present) {
    HANDLE_CODE(pack_integer(bref, extended_wait_time_cpdata_r14, (uint16_t)1u, (uint16_t)1800u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_nb_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(redirected_carrier_info_v1430_present, 1));
  HANDLE_CODE(bref.unpack(extended_wait_time_cpdata_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (redirected_carrier_info_v1430_present) {
    HANDLE_CODE(redirected_carrier_info_v1430.unpack(bref));
  }
  if (extended_wait_time_cpdata_r14_present) {
    HANDLE_CODE(unpack_integer(extended_wait_time_cpdata_r14, bref, (uint16_t)1u, (uint16_t)1800u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_release_nb_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (redirected_carrier_info_v1430_present) {
    j.write_fieldname("redirectedCarrierInfo-v1430");
    redirected_carrier_info_v1430.to_json(j);
  }
  if (extended_wait_time_cpdata_r14_present) {
    j.write_int("extendedWaitTime-CPdata-r14", extended_wait_time_cpdata_r14);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// ReleaseCause-NB-r13 ::= ENUMERATED
std::string release_cause_nb_r13_opts::to_string() const
{
  static const char* options[] = {"loadBalancingTAUrequired", "other", "rrc-Suspend", "spare1"};
  return convert_enum_idx(options, 4, value, "release_cause_nb_r13_e");
}

// DLInformationTransfer-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE dl_info_transfer_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ded_info_nas_r13.pack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ded_info_nas_r13.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dl_info_transfer_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("dedicatedInfoNAS-r13", ded_info_nas_r13.to_string());
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

// RRCConnectionReconfiguration-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ded_info_nas_list_r13_present, 1));
  HANDLE_CODE(bref.pack(rr_cfg_ded_r13_present, 1));
  HANDLE_CODE(bref.pack(full_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ded_info_nas_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ded_info_nas_list_r13, 1, 2));
  }
  if (rr_cfg_ded_r13_present) {
    HANDLE_CODE(rr_cfg_ded_r13.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ded_info_nas_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(rr_cfg_ded_r13_present, 1));
  HANDLE_CODE(bref.unpack(full_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ded_info_nas_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ded_info_nas_list_r13, bref, 1, 2));
  }
  if (rr_cfg_ded_r13_present) {
    HANDLE_CODE(rr_cfg_ded_r13.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ded_info_nas_list_r13_present) {
    j.start_array("dedicatedInfoNASList-r13");
    for (const auto& e1 : ded_info_nas_list_r13) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (rr_cfg_ded_r13_present) {
    j.write_fieldname("radioResourceConfigDedicated-r13");
    rr_cfg_ded_r13.to_json(j);
  }
  if (full_cfg_r13_present) {
    j.write_str("fullConfig-r13", "true");
  }
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

// RRCConnectionRelease-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_release_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(resume_id_r13_present, 1));
  HANDLE_CODE(bref.pack(extended_wait_time_r13_present, 1));
  HANDLE_CODE(bref.pack(redirected_carrier_info_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(release_cause_r13.pack(bref));
  if (resume_id_r13_present) {
    HANDLE_CODE(resume_id_r13.pack(bref));
  }
  if (extended_wait_time_r13_present) {
    HANDLE_CODE(pack_integer(bref, extended_wait_time_r13, (uint16_t)1u, (uint16_t)1800u));
  }
  if (redirected_carrier_info_r13_present) {
    HANDLE_CODE(redirected_carrier_info_r13.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(resume_id_r13_present, 1));
  HANDLE_CODE(bref.unpack(extended_wait_time_r13_present, 1));
  HANDLE_CODE(bref.unpack(redirected_carrier_info_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(release_cause_r13.unpack(bref));
  if (resume_id_r13_present) {
    HANDLE_CODE(resume_id_r13.unpack(bref));
  }
  if (extended_wait_time_r13_present) {
    HANDLE_CODE(unpack_integer(extended_wait_time_r13, bref, (uint16_t)1u, (uint16_t)1800u));
  }
  if (redirected_carrier_info_r13_present) {
    HANDLE_CODE(redirected_carrier_info_r13.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_release_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("releaseCause-r13", release_cause_r13.to_string());
  if (resume_id_r13_present) {
    j.write_str("resumeIdentity-r13", resume_id_r13.to_string());
  }
  if (extended_wait_time_r13_present) {
    j.write_int("extendedWaitTime-r13", extended_wait_time_r13);
  }
  if (redirected_carrier_info_r13_present) {
    j.write_fieldname("redirectedCarrierInfo-r13");
    redirected_carrier_info_r13.to_json(j);
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionResume-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rr_cfg_ded_r13_present, 1));
  HANDLE_CODE(bref.pack(drb_continue_rohc_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rr_cfg_ded_r13_present) {
    HANDLE_CODE(rr_cfg_ded_r13.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, next_hop_chaining_count_r13, (uint8_t)0u, (uint8_t)7u));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rr_cfg_ded_r13_present, 1));
  HANDLE_CODE(bref.unpack(drb_continue_rohc_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rr_cfg_ded_r13_present) {
    HANDLE_CODE(rr_cfg_ded_r13.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(next_hop_chaining_count_r13, bref, (uint8_t)0u, (uint8_t)7u));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rr_cfg_ded_r13_present) {
    j.write_fieldname("radioResourceConfigDedicated-r13");
    rr_cfg_ded_r13.to_json(j);
  }
  j.write_int("nextHopChainingCount-r13", next_hop_chaining_count_r13);
  if (drb_continue_rohc_r13_present) {
    j.write_str("drb-ContinueROHC-r13", "true");
  }
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

// UECapabilityEnquiry-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_nb_r13_ies_s::to_json(json_writer& j) const
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

// DLInformationTransfer-NB ::= SEQUENCE
SRSASN_CODE dl_info_transfer_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void dl_info_transfer_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void dl_info_transfer_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void dl_info_transfer_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_nb_s::crit_exts_c_");
  }
}
dl_info_transfer_nb_s::crit_exts_c_::crit_exts_c_(const dl_info_transfer_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_nb_s::crit_exts_c_");
  }
}
dl_info_transfer_nb_s::crit_exts_c_& dl_info_transfer_nb_s::crit_exts_c_::
                                     operator=(const dl_info_transfer_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_nb_s::crit_exts_c_");
  }

  return *this;
}
void dl_info_transfer_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_info_transfer_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void dl_info_transfer_nb_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
void dl_info_transfer_nb_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::dl_info_transfer_r13:
      j.write_fieldname("dlInformationTransfer-r13");
      c.to_json(j);
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_nb_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_info_transfer_nb_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::dl_info_transfer_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_nb_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::dl_info_transfer_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string dl_info_transfer_nb_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"dlInformationTransfer-r13", "spare1"};
  return convert_enum_idx(options, 2, value, "dl_info_transfer_nb_s::crit_exts_c_::c1_c_::types");
}

// RRCConnectionReconfiguration-NB ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_recfg_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void rrc_conn_recfg_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_nb_s::crit_exts_c_");
  }
}
rrc_conn_recfg_nb_s::crit_exts_c_::crit_exts_c_(const rrc_conn_recfg_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_nb_s::crit_exts_c_");
  }
}
rrc_conn_recfg_nb_s::crit_exts_c_& rrc_conn_recfg_nb_s::crit_exts_c_::
                                   operator=(const rrc_conn_recfg_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_nb_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_recfg_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_recfg_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_conn_recfg_nb_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
void rrc_conn_recfg_nb_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_recfg_r13:
      j.write_fieldname("rrcConnectionReconfiguration-r13");
      c.to_json(j);
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_nb_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_recfg_nb_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_recfg_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_nb_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_recfg_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_conn_recfg_nb_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReconfiguration-r13", "spare1"};
  return convert_enum_idx(options, 2, value, "rrc_conn_recfg_nb_s::crit_exts_c_::c1_c_::types");
}

// RRCConnectionRelease-NB ::= SEQUENCE
SRSASN_CODE rrc_conn_release_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_release_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_release_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void rrc_conn_release_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_nb_s::crit_exts_c_");
  }
}
rrc_conn_release_nb_s::crit_exts_c_::crit_exts_c_(const rrc_conn_release_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_nb_s::crit_exts_c_");
  }
}
rrc_conn_release_nb_s::crit_exts_c_& rrc_conn_release_nb_s::crit_exts_c_::
                                     operator=(const rrc_conn_release_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_nb_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_release_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_release_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_conn_release_nb_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
void rrc_conn_release_nb_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_release_r13:
      j.write_fieldname("rrcConnectionRelease-r13");
      c.to_json(j);
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_nb_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_release_nb_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_release_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_nb_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_release_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_conn_release_nb_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionRelease-r13", "spare1"};
  return convert_enum_idx(options, 2, value, "rrc_conn_release_nb_s::crit_exts_c_::c1_c_::types");
}

// RRCConnectionResume-NB ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_resume_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void rrc_conn_resume_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_nb_s::crit_exts_c_");
  }
}
rrc_conn_resume_nb_s::crit_exts_c_::crit_exts_c_(const rrc_conn_resume_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_nb_s::crit_exts_c_");
  }
}
rrc_conn_resume_nb_s::crit_exts_c_& rrc_conn_resume_nb_s::crit_exts_c_::
                                    operator=(const rrc_conn_resume_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_nb_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_resume_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_resume_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_conn_resume_nb_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
void rrc_conn_resume_nb_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_resume_r13:
      j.write_fieldname("rrcConnectionResume-r13");
      c.to_json(j);
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_nb_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_resume_nb_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_resume_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_nb_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_resume_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_conn_resume_nb_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionResume-r13", "spare1"};
  return convert_enum_idx(options, 2, value, "rrc_conn_resume_nb_s::crit_exts_c_::c1_c_::types");
}

// UECapabilityEnquiry-NB ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_cap_enquiry_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void ue_cap_enquiry_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_nb_s::crit_exts_c_");
  }
}
ue_cap_enquiry_nb_s::crit_exts_c_::crit_exts_c_(const ue_cap_enquiry_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_nb_s::crit_exts_c_");
  }
}
ue_cap_enquiry_nb_s::crit_exts_c_& ue_cap_enquiry_nb_s::crit_exts_c_::
                                   operator=(const ue_cap_enquiry_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_nb_s::crit_exts_c_");
  }

  return *this;
}
void ue_cap_enquiry_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_cap_enquiry_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_cap_enquiry_nb_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
void ue_cap_enquiry_nb_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_cap_enquiry_r13:
      j.write_fieldname("ueCapabilityEnquiry-r13");
      c.to_json(j);
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_nb_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_cap_enquiry_nb_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_cap_enquiry_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_nb_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_cap_enquiry_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string ue_cap_enquiry_nb_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueCapabilityEnquiry-r13", "spare1"};
  return convert_enum_idx(options, 2, value, "ue_cap_enquiry_nb_s::crit_exts_c_::c1_c_::types");
}

// DL-DCCH-MessageType-NB ::= CHOICE
void dl_dcch_msg_type_nb_c::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void dl_dcch_msg_type_nb_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::msg_class_ext:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_nb_c");
  }
}
dl_dcch_msg_type_nb_c::dl_dcch_msg_type_nb_c(const dl_dcch_msg_type_nb_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_nb_c");
  }
}
dl_dcch_msg_type_nb_c& dl_dcch_msg_type_nb_c::operator=(const dl_dcch_msg_type_nb_c& other)
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
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_nb_c");
  }

  return *this;
}
void dl_dcch_msg_type_nb_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_nb_c");
  }
  j.end_obj();
}
SRSASN_CODE dl_dcch_msg_type_nb_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_nb_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_dcch_msg_type_nb_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_nb_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void dl_dcch_msg_type_nb_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::dl_info_transfer_r13:
      c.destroy<dl_info_transfer_nb_s>();
      break;
    case types::rrc_conn_recfg_r13:
      c.destroy<rrc_conn_recfg_nb_s>();
      break;
    case types::rrc_conn_release_r13:
      c.destroy<rrc_conn_release_nb_s>();
      break;
    case types::security_mode_cmd_r13:
      c.destroy<security_mode_cmd_s>();
      break;
    case types::ue_cap_enquiry_r13:
      c.destroy<ue_cap_enquiry_nb_s>();
      break;
    case types::rrc_conn_resume_r13:
      c.destroy<rrc_conn_resume_nb_s>();
      break;
    default:
      break;
  }
}
void dl_dcch_msg_type_nb_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::dl_info_transfer_r13:
      c.init<dl_info_transfer_nb_s>();
      break;
    case types::rrc_conn_recfg_r13:
      c.init<rrc_conn_recfg_nb_s>();
      break;
    case types::rrc_conn_release_r13:
      c.init<rrc_conn_release_nb_s>();
      break;
    case types::security_mode_cmd_r13:
      c.init<security_mode_cmd_s>();
      break;
    case types::ue_cap_enquiry_r13:
      c.init<ue_cap_enquiry_nb_s>();
      break;
    case types::rrc_conn_resume_r13:
      c.init<rrc_conn_resume_nb_s>();
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_nb_c::c1_c_");
  }
}
dl_dcch_msg_type_nb_c::c1_c_::c1_c_(const dl_dcch_msg_type_nb_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::dl_info_transfer_r13:
      c.init(other.c.get<dl_info_transfer_nb_s>());
      break;
    case types::rrc_conn_recfg_r13:
      c.init(other.c.get<rrc_conn_recfg_nb_s>());
      break;
    case types::rrc_conn_release_r13:
      c.init(other.c.get<rrc_conn_release_nb_s>());
      break;
    case types::security_mode_cmd_r13:
      c.init(other.c.get<security_mode_cmd_s>());
      break;
    case types::ue_cap_enquiry_r13:
      c.init(other.c.get<ue_cap_enquiry_nb_s>());
      break;
    case types::rrc_conn_resume_r13:
      c.init(other.c.get<rrc_conn_resume_nb_s>());
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_nb_c::c1_c_");
  }
}
dl_dcch_msg_type_nb_c::c1_c_& dl_dcch_msg_type_nb_c::c1_c_::operator=(const dl_dcch_msg_type_nb_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::dl_info_transfer_r13:
      c.set(other.c.get<dl_info_transfer_nb_s>());
      break;
    case types::rrc_conn_recfg_r13:
      c.set(other.c.get<rrc_conn_recfg_nb_s>());
      break;
    case types::rrc_conn_release_r13:
      c.set(other.c.get<rrc_conn_release_nb_s>());
      break;
    case types::security_mode_cmd_r13:
      c.set(other.c.get<security_mode_cmd_s>());
      break;
    case types::ue_cap_enquiry_r13:
      c.set(other.c.get<ue_cap_enquiry_nb_s>());
      break;
    case types::rrc_conn_resume_r13:
      c.set(other.c.get<rrc_conn_resume_nb_s>());
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_nb_c::c1_c_");
  }

  return *this;
}
void dl_dcch_msg_type_nb_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::dl_info_transfer_r13:
      j.write_fieldname("dlInformationTransfer-r13");
      c.get<dl_info_transfer_nb_s>().to_json(j);
      break;
    case types::rrc_conn_recfg_r13:
      j.write_fieldname("rrcConnectionReconfiguration-r13");
      c.get<rrc_conn_recfg_nb_s>().to_json(j);
      break;
    case types::rrc_conn_release_r13:
      j.write_fieldname("rrcConnectionRelease-r13");
      c.get<rrc_conn_release_nb_s>().to_json(j);
      break;
    case types::security_mode_cmd_r13:
      j.write_fieldname("securityModeCommand-r13");
      c.get<security_mode_cmd_s>().to_json(j);
      break;
    case types::ue_cap_enquiry_r13:
      j.write_fieldname("ueCapabilityEnquiry-r13");
      c.get<ue_cap_enquiry_nb_s>().to_json(j);
      break;
    case types::rrc_conn_resume_r13:
      j.write_fieldname("rrcConnectionResume-r13");
      c.get<rrc_conn_resume_nb_s>().to_json(j);
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_nb_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_dcch_msg_type_nb_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::dl_info_transfer_r13:
      HANDLE_CODE(c.get<dl_info_transfer_nb_s>().pack(bref));
      break;
    case types::rrc_conn_recfg_r13:
      HANDLE_CODE(c.get<rrc_conn_recfg_nb_s>().pack(bref));
      break;
    case types::rrc_conn_release_r13:
      HANDLE_CODE(c.get<rrc_conn_release_nb_s>().pack(bref));
      break;
    case types::security_mode_cmd_r13:
      HANDLE_CODE(c.get<security_mode_cmd_s>().pack(bref));
      break;
    case types::ue_cap_enquiry_r13:
      HANDLE_CODE(c.get<ue_cap_enquiry_nb_s>().pack(bref));
      break;
    case types::rrc_conn_resume_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_nb_s>().pack(bref));
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_nb_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_dcch_msg_type_nb_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::dl_info_transfer_r13:
      HANDLE_CODE(c.get<dl_info_transfer_nb_s>().unpack(bref));
      break;
    case types::rrc_conn_recfg_r13:
      HANDLE_CODE(c.get<rrc_conn_recfg_nb_s>().unpack(bref));
      break;
    case types::rrc_conn_release_r13:
      HANDLE_CODE(c.get<rrc_conn_release_nb_s>().unpack(bref));
      break;
    case types::security_mode_cmd_r13:
      HANDLE_CODE(c.get<security_mode_cmd_s>().unpack(bref));
      break;
    case types::ue_cap_enquiry_r13:
      HANDLE_CODE(c.get<ue_cap_enquiry_nb_s>().unpack(bref));
      break;
    case types::rrc_conn_resume_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_nb_s>().unpack(bref));
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_nb_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string dl_dcch_msg_type_nb_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"dlInformationTransfer-r13",
                                  "rrcConnectionReconfiguration-r13",
                                  "rrcConnectionRelease-r13",
                                  "securityModeCommand-r13",
                                  "ueCapabilityEnquiry-r13",
                                  "rrcConnectionResume-r13",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 8, value, "dl_dcch_msg_type_nb_c::c1_c_::types");
}

std::string dl_dcch_msg_type_nb_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "dl_dcch_msg_type_nb_c::types");
}
uint8_t dl_dcch_msg_type_nb_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "dl_dcch_msg_type_nb_c::types");
}

// DL-DCCH-Message-NB ::= SEQUENCE
SRSASN_CODE dl_dcch_msg_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_dcch_msg_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void dl_dcch_msg_nb_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("DL-DCCH-Message-NB");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// PhyLayerParameters-NB-r13 ::= SEQUENCE
SRSASN_CODE phy_layer_params_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(multi_tone_r13_present, 1));
  HANDLE_CODE(bref.pack(multi_carrier_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(multi_tone_r13_present, 1));
  HANDLE_CODE(bref.unpack(multi_carrier_r13_present, 1));

  return SRSASN_SUCCESS;
}
void phy_layer_params_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (multi_tone_r13_present) {
    j.write_str("multiTone-r13", "supported");
  }
  if (multi_carrier_r13_present) {
    j.write_str("multiCarrier-r13", "supported");
  }
  j.end_obj();
}

// PhyLayerParameters-NB-v1430 ::= SEQUENCE
SRSASN_CODE phy_layer_params_nb_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(multi_carrier_nprach_r14_present, 1));
  HANDLE_CODE(bref.pack(two_harq_processes_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_nb_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(multi_carrier_nprach_r14_present, 1));
  HANDLE_CODE(bref.unpack(two_harq_processes_r14_present, 1));

  return SRSASN_SUCCESS;
}
void phy_layer_params_nb_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (multi_carrier_nprach_r14_present) {
    j.write_str("multiCarrier-NPRACH-r14", "supported");
  }
  if (two_harq_processes_r14_present) {
    j.write_str("twoHARQ-Processes-r14", "supported");
  }
  j.end_obj();
}

// PhyLayerParameters-NB-v1530 ::= SEQUENCE
SRSASN_CODE phy_layer_params_nb_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mixed_operation_mode_r15_present, 1));
  HANDLE_CODE(bref.pack(sr_with_harq_ack_r15_present, 1));
  HANDLE_CODE(bref.pack(sr_without_harq_ack_r15_present, 1));
  HANDLE_CODE(bref.pack(nprach_format2_r15_present, 1));
  HANDLE_CODE(bref.pack(add_tx_sib1_r15_present, 1));
  HANDLE_CODE(bref.pack(npusch_minus3dot75k_hz_scs_tdd_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_nb_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mixed_operation_mode_r15_present, 1));
  HANDLE_CODE(bref.unpack(sr_with_harq_ack_r15_present, 1));
  HANDLE_CODE(bref.unpack(sr_without_harq_ack_r15_present, 1));
  HANDLE_CODE(bref.unpack(nprach_format2_r15_present, 1));
  HANDLE_CODE(bref.unpack(add_tx_sib1_r15_present, 1));
  HANDLE_CODE(bref.unpack(npusch_minus3dot75k_hz_scs_tdd_r15_present, 1));

  return SRSASN_SUCCESS;
}
void phy_layer_params_nb_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mixed_operation_mode_r15_present) {
    j.write_str("mixedOperationMode-r15", "supported");
  }
  if (sr_with_harq_ack_r15_present) {
    j.write_str("sr-WithHARQ-ACK-r15", "supported");
  }
  if (sr_without_harq_ack_r15_present) {
    j.write_str("sr-WithoutHARQ-ACK-r15", "supported");
  }
  if (nprach_format2_r15_present) {
    j.write_str("nprach-Format2-r15", "supported");
  }
  if (add_tx_sib1_r15_present) {
    j.write_str("additionalTransmissionSIB1-r15", "supported");
  }
  if (npusch_minus3dot75k_hz_scs_tdd_r15_present) {
    j.write_str("npusch-3dot75kHz-SCS-TDD-r15", "supported");
  }
  j.end_obj();
}

// MAC-Parameters-NB-v1530 ::= SEQUENCE
SRSASN_CODE mac_params_nb_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sr_sps_bsr_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_params_nb_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sr_sps_bsr_r15_present, 1));

  return SRSASN_SUCCESS;
}
void mac_params_nb_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sr_sps_bsr_r15_present) {
    j.write_str("sr-SPS-BSR-r15", "supported");
  }
  j.end_obj();
}

// RLC-Parameters-NB-r15 ::= SEQUENCE
SRSASN_CODE rlc_params_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rlc_um_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_params_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rlc_um_r15_present, 1));

  return SRSASN_SUCCESS;
}
void rlc_params_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rlc_um_r15_present) {
    j.write_str("rlc-UM-r15", "supported");
  }
  j.end_obj();
}

// TDD-UE-Capability-NB-r15 ::= SEQUENCE
SRSASN_CODE tdd_ue_cap_nb_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ue_category_nb_r15_present, 1));
  HANDLE_CODE(bref.pack(phy_layer_params_rel13_r15_present, 1));
  HANDLE_CODE(bref.pack(phy_layer_params_rel14_r15_present, 1));
  HANDLE_CODE(bref.pack(phy_layer_params_v1530_present, 1));

  if (phy_layer_params_rel13_r15_present) {
    HANDLE_CODE(phy_layer_params_rel13_r15.pack(bref));
  }
  if (phy_layer_params_rel14_r15_present) {
    HANDLE_CODE(phy_layer_params_rel14_r15.pack(bref));
  }
  if (phy_layer_params_v1530_present) {
    HANDLE_CODE(phy_layer_params_v1530.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_ue_cap_nb_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ue_category_nb_r15_present, 1));
  HANDLE_CODE(bref.unpack(phy_layer_params_rel13_r15_present, 1));
  HANDLE_CODE(bref.unpack(phy_layer_params_rel14_r15_present, 1));
  HANDLE_CODE(bref.unpack(phy_layer_params_v1530_present, 1));

  if (phy_layer_params_rel13_r15_present) {
    HANDLE_CODE(phy_layer_params_rel13_r15.unpack(bref));
  }
  if (phy_layer_params_rel14_r15_present) {
    HANDLE_CODE(phy_layer_params_rel14_r15.unpack(bref));
  }
  if (phy_layer_params_v1530_present) {
    HANDLE_CODE(phy_layer_params_v1530.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void tdd_ue_cap_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_category_nb_r15_present) {
    j.write_str("ue-Category-NB-r15", "nb2");
  }
  if (phy_layer_params_rel13_r15_present) {
    j.write_fieldname("phyLayerParametersRel13-r15");
    phy_layer_params_rel13_r15.to_json(j);
  }
  if (phy_layer_params_rel14_r15_present) {
    j.write_fieldname("phyLayerParametersRel14-r15");
    phy_layer_params_rel14_r15.to_json(j);
  }
  if (phy_layer_params_v1530_present) {
    j.write_fieldname("phyLayerParameters-v1530");
    phy_layer_params_v1530.to_json(j);
  }
  j.end_obj();
}

// UE-Capability-NB-v1530-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_nb_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(early_data_up_r15_present, 1));
  HANDLE_CODE(bref.pack(phy_layer_params_v1530_present, 1));
  HANDLE_CODE(bref.pack(tdd_ue_cap_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(rlc_params_r15.pack(bref));
  HANDLE_CODE(mac_params_v1530.pack(bref));
  if (phy_layer_params_v1530_present) {
    HANDLE_CODE(phy_layer_params_v1530.pack(bref));
  }
  if (tdd_ue_cap_r15_present) {
    HANDLE_CODE(tdd_ue_cap_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_nb_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(early_data_up_r15_present, 1));
  HANDLE_CODE(bref.unpack(phy_layer_params_v1530_present, 1));
  HANDLE_CODE(bref.unpack(tdd_ue_cap_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(rlc_params_r15.unpack(bref));
  HANDLE_CODE(mac_params_v1530.unpack(bref));
  if (phy_layer_params_v1530_present) {
    HANDLE_CODE(phy_layer_params_v1530.unpack(bref));
  }
  if (tdd_ue_cap_r15_present) {
    HANDLE_CODE(tdd_ue_cap_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_nb_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (early_data_up_r15_present) {
    j.write_str("earlyData-UP-r15", "supported");
  }
  j.write_fieldname("rlc-Parameters-r15");
  rlc_params_r15.to_json(j);
  j.write_fieldname("mac-Parameters-v1530");
  mac_params_v1530.to_json(j);
  if (phy_layer_params_v1530_present) {
    j.write_fieldname("phyLayerParameters-v1530");
    phy_layer_params_v1530.to_json(j);
  }
  if (tdd_ue_cap_r15_present) {
    j.write_fieldname("tdd-UE-Capability-r15");
    tdd_ue_cap_r15.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// PhyLayerParameters-NB-v1440 ::= SEQUENCE
SRSASN_CODE phy_layer_params_nb_v1440_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(interference_randomisation_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_nb_v1440_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(interference_randomisation_r14_present, 1));

  return SRSASN_SUCCESS;
}
void phy_layer_params_nb_v1440_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (interference_randomisation_r14_present) {
    j.write_str("interferenceRandomisation-r14", "supported");
  }
  j.end_obj();
}

// UE-Capability-NB-v14x0-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_nb_v14x0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_nb_v14x0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_nb_v14x0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MAC-Parameters-NB-r14 ::= SEQUENCE
SRSASN_CODE mac_params_nb_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(data_inact_mon_r14_present, 1));
  HANDLE_CODE(bref.pack(rai_support_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_params_nb_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(data_inact_mon_r14_present, 1));
  HANDLE_CODE(bref.unpack(rai_support_r14_present, 1));

  return SRSASN_SUCCESS;
}
void mac_params_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (data_inact_mon_r14_present) {
    j.write_str("dataInactMon-r14", "supported");
  }
  if (rai_support_r14_present) {
    j.write_str("rai-Support-r14", "supported");
  }
  j.end_obj();
}

// RF-Parameters-NB-v1430 ::= SEQUENCE
SRSASN_CODE rf_params_nb_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pwr_class_nb_minus14dbm_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_nb_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pwr_class_nb_minus14dbm_r14_present, 1));

  return SRSASN_SUCCESS;
}
void rf_params_nb_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pwr_class_nb_minus14dbm_r14_present) {
    j.write_str("powerClassNB-14dBm-r14", "supported");
  }
  j.end_obj();
}

// SupportedBand-NB-r13 ::= SEQUENCE
SRSASN_CODE supported_band_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pwr_class_nb_minus20dbm_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, band_r13, (uint16_t)1u, (uint16_t)256u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_band_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pwr_class_nb_minus20dbm_r13_present, 1));

  HANDLE_CODE(unpack_integer(band_r13, bref, (uint16_t)1u, (uint16_t)256u));

  return SRSASN_SUCCESS;
}
void supported_band_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("band-r13", band_r13);
  if (pwr_class_nb_minus20dbm_r13_present) {
    j.write_str("powerClassNB-20dBm-r13", "supported");
  }
  j.end_obj();
}

// UE-Capability-NB-v1440-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_nb_v1440_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_layer_params_v1440_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (phy_layer_params_v1440_present) {
    HANDLE_CODE(phy_layer_params_v1440.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_nb_v1440_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_layer_params_v1440_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (phy_layer_params_v1440_present) {
    HANDLE_CODE(phy_layer_params_v1440.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_nb_v1440_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_v1440_present) {
    j.write_fieldname("phyLayerParameters-v1440");
    phy_layer_params_v1440.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-Capability-NB-Ext-r14-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_nb_ext_r14_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_category_nb_r14_present, 1));
  HANDLE_CODE(bref.pack(mac_params_r14_present, 1));
  HANDLE_CODE(bref.pack(phy_layer_params_v1430_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mac_params_r14_present) {
    HANDLE_CODE(mac_params_r14.pack(bref));
  }
  if (phy_layer_params_v1430_present) {
    HANDLE_CODE(phy_layer_params_v1430.pack(bref));
  }
  HANDLE_CODE(rf_params_v1430.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_nb_ext_r14_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_category_nb_r14_present, 1));
  HANDLE_CODE(bref.unpack(mac_params_r14_present, 1));
  HANDLE_CODE(bref.unpack(phy_layer_params_v1430_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mac_params_r14_present) {
    HANDLE_CODE(mac_params_r14.unpack(bref));
  }
  if (phy_layer_params_v1430_present) {
    HANDLE_CODE(phy_layer_params_v1430.unpack(bref));
  }
  HANDLE_CODE(rf_params_v1430.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_nb_ext_r14_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_category_nb_r14_present) {
    j.write_str("ue-Category-NB-r14", "nb2");
  }
  if (mac_params_r14_present) {
    j.write_fieldname("mac-Parameters-r14");
    mac_params_r14.to_json(j);
  }
  if (phy_layer_params_v1430_present) {
    j.write_fieldname("phyLayerParameters-v1430");
    phy_layer_params_v1430.to_json(j);
  }
  j.write_fieldname("rf-Parameters-v1430");
  rf_params_v1430.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// AccessStratumRelease-NB-r13 ::= ENUMERATED
std::string access_stratum_release_nb_r13_opts::to_string() const
{
  static const char* options[] = {"rel13", "rel14", "rel15", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "access_stratum_release_nb_r13_e");
}
uint8_t access_stratum_release_nb_r13_opts::to_number() const
{
  static const uint8_t options[] = {13, 14, 15};
  return map_enum_number(options, 3, value, "access_stratum_release_nb_r13_e");
}

// HandoverPreparationInformation-NB-Ext-r14-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_nb_ext_r14_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_radio_access_cap_info_ext_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_radio_access_cap_info_ext_r14_present) {
    HANDLE_CODE(ue_radio_access_cap_info_ext_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_nb_ext_r14_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_radio_access_cap_info_ext_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_radio_access_cap_info_ext_r14_present) {
    HANDLE_CODE(ue_radio_access_cap_info_ext_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_nb_ext_r14_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_radio_access_cap_info_ext_r14_present) {
    j.write_str("ue-RadioAccessCapabilityInfoExt-r14", ue_radio_access_cap_info_ext_r14.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// PDCP-Parameters-NB-r13 ::= SEQUENCE
SRSASN_CODE pdcp_params_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(max_num_rohc_context_sessions_r13_present, 1));

  HANDLE_CODE(bref.pack(supported_rohc_profiles_r13.profile0x0002, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles_r13.profile0x0003, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles_r13.profile0x0004, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles_r13.profile0x0006, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles_r13.profile0x0102, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles_r13.profile0x0103, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles_r13.profile0x0104, 1));
  if (max_num_rohc_context_sessions_r13_present) {
    HANDLE_CODE(max_num_rohc_context_sessions_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_params_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(max_num_rohc_context_sessions_r13_present, 1));

  HANDLE_CODE(bref.unpack(supported_rohc_profiles_r13.profile0x0002, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles_r13.profile0x0003, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles_r13.profile0x0004, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles_r13.profile0x0006, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles_r13.profile0x0102, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles_r13.profile0x0103, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles_r13.profile0x0104, 1));
  if (max_num_rohc_context_sessions_r13_present) {
    HANDLE_CODE(max_num_rohc_context_sessions_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdcp_params_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("supportedROHC-Profiles-r13");
  j.start_obj();
  j.write_bool("profile0x0002", supported_rohc_profiles_r13.profile0x0002);
  j.write_bool("profile0x0003", supported_rohc_profiles_r13.profile0x0003);
  j.write_bool("profile0x0004", supported_rohc_profiles_r13.profile0x0004);
  j.write_bool("profile0x0006", supported_rohc_profiles_r13.profile0x0006);
  j.write_bool("profile0x0102", supported_rohc_profiles_r13.profile0x0102);
  j.write_bool("profile0x0103", supported_rohc_profiles_r13.profile0x0103);
  j.write_bool("profile0x0104", supported_rohc_profiles_r13.profile0x0104);
  j.end_obj();
  if (max_num_rohc_context_sessions_r13_present) {
    j.write_str("maxNumberROHC-ContextSessions-r13", max_num_rohc_context_sessions_r13.to_string());
  }
  j.end_obj();
}

std::string pdcp_params_nb_r13_s::max_num_rohc_context_sessions_r13_opts::to_string() const
{
  static const char* options[] = {"cs2", "cs4", "cs8", "cs12"};
  return convert_enum_idx(options, 4, value, "pdcp_params_nb_r13_s::max_num_rohc_context_sessions_r13_e_");
}
uint8_t pdcp_params_nb_r13_s::max_num_rohc_context_sessions_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8, 12};
  return map_enum_number(options, 4, value, "pdcp_params_nb_r13_s::max_num_rohc_context_sessions_r13_e_");
}

// RF-Parameters-NB-r13 ::= SEQUENCE
SRSASN_CODE rf_params_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(multi_ns_pmax_r13_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_r13, 1, 64));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(multi_ns_pmax_r13_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_r13, bref, 1, 64));

  return SRSASN_SUCCESS;
}
void rf_params_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandList-r13");
  for (const auto& e1 : supported_band_list_r13) {
    e1.to_json(j);
  }
  j.end_array();
  if (multi_ns_pmax_r13_present) {
    j.write_str("multiNS-Pmax-r13", "supported");
  }
  j.end_obj();
}

// HandoverPreparationInformation-NB-v1380-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_nb_v1380_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_nb_v1380_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_nb_v1380_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRM-Config-NB ::= SEQUENCE
SRSASN_CODE rrm_cfg_nb_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ue_inactive_time_present, 1));

  if (ue_inactive_time_present) {
    HANDLE_CODE(ue_inactive_time.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrm_cfg_nb_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ue_inactive_time_present, 1));

  if (ue_inactive_time_present) {
    HANDLE_CODE(ue_inactive_time.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrm_cfg_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_inactive_time_present) {
    j.write_str("ue-InactiveTime", ue_inactive_time.to_string());
  }
  j.end_obj();
}

std::string rrm_cfg_nb_s::ue_inactive_time_opts::to_string() const
{
  static const char* options[] = {
      "s1",    "s2",       "s3",    "s5",           "s7",      "s10",   "s15",     "s20",      "s25",     "s30",
      "s40",   "s50",      "min1",  "min1s20",      "min1s40", "min2",  "min2s30", "min3",     "min3s30", "min4",
      "min5",  "min6",     "min7",  "min8",         "min9",    "min10", "min12",   "min14",    "min17",   "min20",
      "min24", "min28",    "min33", "min38",        "min44",   "min50", "hr1",     "hr1min30", "hr2",     "hr2min30",
      "hr3",   "hr3min30", "hr4",   "hr5",          "hr6",     "hr8",   "hr10",    "hr13",     "hr16",    "hr20",
      "day1",  "day1hr12", "day2",  "day2hr12",     "day3",    "day4",  "day5",    "day7",     "day10",   "day14",
      "day19", "day24",    "day30", "dayMoreThan30"};
  return convert_enum_idx(options, 64, value, "rrm_cfg_nb_s::ue_inactive_time_e_");
}

// UE-Capability-NB-r13 ::= SEQUENCE
SRSASN_CODE ue_cap_nb_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_category_nb_r13_present, 1));
  HANDLE_CODE(bref.pack(multiple_drb_r13_present, 1));
  HANDLE_CODE(bref.pack(pdcp_params_r13_present, 1));
  HANDLE_CODE(bref.pack(dummy_present, 1));

  HANDLE_CODE(access_stratum_release_r13.pack(bref));
  if (pdcp_params_r13_present) {
    HANDLE_CODE(pdcp_params_r13.pack(bref));
  }
  HANDLE_CODE(phy_layer_params_r13.pack(bref));
  HANDLE_CODE(rf_params_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_nb_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_category_nb_r13_present, 1));
  HANDLE_CODE(bref.unpack(multiple_drb_r13_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_params_r13_present, 1));
  HANDLE_CODE(bref.unpack(dummy_present, 1));

  HANDLE_CODE(access_stratum_release_r13.unpack(bref));
  if (pdcp_params_r13_present) {
    HANDLE_CODE(pdcp_params_r13.unpack(bref));
  }
  HANDLE_CODE(phy_layer_params_r13.unpack(bref));
  HANDLE_CODE(rf_params_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_cap_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("accessStratumRelease-r13", access_stratum_release_r13.to_string());
  if (ue_category_nb_r13_present) {
    j.write_str("ue-Category-NB-r13", "nb1");
  }
  if (multiple_drb_r13_present) {
    j.write_str("multipleDRB-r13", "supported");
  }
  if (pdcp_params_r13_present) {
    j.write_fieldname("pdcp-Parameters-r13");
    pdcp_params_r13.to_json(j);
  }
  j.write_fieldname("phyLayerParameters-r13");
  phy_layer_params_r13.to_json(j);
  j.write_fieldname("rf-Parameters-r13");
  rf_params_r13.to_json(j);
  if (dummy_present) {
    j.write_fieldname("dummy");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// HandoverPreparationInformation-NB-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_nb_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rrm_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(as_context_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_radio_access_cap_info_r13.pack(bref));
  HANDLE_CODE(as_cfg_r13.pack(bref));
  if (rrm_cfg_r13_present) {
    HANDLE_CODE(rrm_cfg_r13.pack(bref));
  }
  if (as_context_r13_present) {
    HANDLE_CODE(as_context_r13.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_nb_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rrm_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(as_context_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_radio_access_cap_info_r13.unpack(bref));
  HANDLE_CODE(as_cfg_r13.unpack(bref));
  if (rrm_cfg_r13_present) {
    HANDLE_CODE(rrm_cfg_r13.unpack(bref));
  }
  if (as_context_r13_present) {
    HANDLE_CODE(as_context_r13.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_nb_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-RadioAccessCapabilityInfo-r13");
  ue_radio_access_cap_info_r13.to_json(j);
  j.write_fieldname("as-Config-r13");
  as_cfg_r13.to_json(j);
  if (rrm_cfg_r13_present) {
    j.write_fieldname("rrm-Config-r13");
    rrm_cfg_r13.to_json(j);
  }
  if (as_context_r13_present) {
    j.write_fieldname("as-Context-r13");
    as_context_r13.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// HandoverPreparationInformation-NB ::= SEQUENCE
SRSASN_CODE ho_prep_info_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ho_prep_info_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ho_prep_info_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void ho_prep_info_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_nb_s::crit_exts_c_");
  }
}
ho_prep_info_nb_s::crit_exts_c_::crit_exts_c_(const ho_prep_info_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_nb_s::crit_exts_c_");
  }
}
ho_prep_info_nb_s::crit_exts_c_& ho_prep_info_nb_s::crit_exts_c_::
                                 operator=(const ho_prep_info_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_nb_s::crit_exts_c_");
  }

  return *this;
}
void ho_prep_info_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_prep_info_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ho_prep_info_nb_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
void ho_prep_info_nb_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho_prep_info_r13:
      j.write_fieldname("handoverPreparationInformation-r13");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_nb_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_prep_info_nb_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ho_prep_info_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_nb_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ho_prep_info_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string ho_prep_info_nb_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"handoverPreparationInformation-r13", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ho_prep_info_nb_s::crit_exts_c_::c1_c_::types");
}

// MeasResultServCell-NB-r14 ::= SEQUENCE
SRSASN_CODE meas_result_serv_cell_nb_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, nrsrp_result_r14, (uint8_t)0u, (uint8_t)113u));
  HANDLE_CODE(pack_integer(bref, nrsrq_result_r14, (int8_t)-30, (int8_t)46));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_serv_cell_nb_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(nrsrp_result_r14, bref, (uint8_t)0u, (uint8_t)113u));
  HANDLE_CODE(unpack_integer(nrsrq_result_r14, bref, (int8_t)-30, (int8_t)46));

  return SRSASN_SUCCESS;
}
void meas_result_serv_cell_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("nrsrpResult-r14", nrsrp_result_r14);
  j.write_int("nrsrqResult-r14", nrsrq_result_r14);
  j.end_obj();
}

// PagingRecord-NB-r13 ::= SEQUENCE
SRSASN_CODE paging_record_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ue_id_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_record_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ue_id_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void paging_record_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity-r13");
  ue_id_r13.to_json(j);
  j.end_obj();
}

// Paging-NB ::= SEQUENCE
SRSASN_CODE paging_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(paging_record_list_r13_present, 1));
  HANDLE_CODE(bref.pack(sys_info_mod_r13_present, 1));
  HANDLE_CODE(bref.pack(sys_info_mod_e_drx_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (paging_record_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, paging_record_list_r13, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(paging_record_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(sys_info_mod_r13_present, 1));
  HANDLE_CODE(bref.unpack(sys_info_mod_e_drx_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (paging_record_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(paging_record_list_r13, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void paging_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (paging_record_list_r13_present) {
    j.start_array("pagingRecordList-r13");
    for (const auto& e1 : paging_record_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sys_info_mod_r13_present) {
    j.write_str("systemInfoModification-r13", "true");
  }
  if (sys_info_mod_e_drx_r13_present) {
    j.write_str("systemInfoModification-eDRX-r13", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// PCCH-MessageType-NB ::= CHOICE
void pcch_msg_type_nb_c::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void pcch_msg_type_nb_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::msg_class_ext:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_nb_c");
  }
}
pcch_msg_type_nb_c::pcch_msg_type_nb_c(const pcch_msg_type_nb_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_nb_c");
  }
}
pcch_msg_type_nb_c& pcch_msg_type_nb_c::operator=(const pcch_msg_type_nb_c& other)
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
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_nb_c");
  }

  return *this;
}
void pcch_msg_type_nb_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_nb_c");
  }
  j.end_obj();
}
SRSASN_CODE pcch_msg_type_nb_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_nb_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_msg_type_nb_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_nb_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void pcch_msg_type_nb_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("paging-r13");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE pcch_msg_type_nb_c::c1_c_::pack(bit_ref& bref) const
{
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_msg_type_nb_c::c1_c_::unpack(cbit_ref& bref)
{
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

std::string pcch_msg_type_nb_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"paging-r13"};
  return convert_enum_idx(options, 1, value, "pcch_msg_type_nb_c::c1_c_::types");
}

std::string pcch_msg_type_nb_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "pcch_msg_type_nb_c::types");
}
uint8_t pcch_msg_type_nb_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "pcch_msg_type_nb_c::types");
}

// PCCH-Message-NB ::= SEQUENCE
SRSASN_CODE pcch_msg_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_msg_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void pcch_msg_nb_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("PCCH-Message-NB");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// PCI-ARFCN-NB-r14 ::= SEQUENCE
SRSASN_CODE pci_arfcn_nb_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(carrier_freq_r14_present, 1));

  HANDLE_CODE(pack_integer(bref, pci_r14, (uint16_t)0u, (uint16_t)503u));
  if (carrier_freq_r14_present) {
    HANDLE_CODE(carrier_freq_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pci_arfcn_nb_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(carrier_freq_r14_present, 1));

  HANDLE_CODE(unpack_integer(pci_r14, bref, (uint16_t)0u, (uint16_t)503u));
  if (carrier_freq_r14_present) {
    HANDLE_CODE(carrier_freq_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pci_arfcn_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r14", pci_r14);
  if (carrier_freq_r14_present) {
    j.write_fieldname("carrierFreq-r14");
    carrier_freq_r14.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReconfigurationComplete-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_complete_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_complete_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_complete_nb_r13_ies_s::to_json(json_writer& j) const
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

// RRCConnectionReconfigurationComplete-NB ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_complete_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_complete_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_complete_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_recfg_complete_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_recfg_complete_r13:
      c.destroy<rrc_conn_recfg_complete_nb_r13_ies_s>();
      break;
    default:
      break;
  }
}
void rrc_conn_recfg_complete_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_recfg_complete_r13:
      c.init<rrc_conn_recfg_complete_nb_r13_ies_s>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_complete_nb_s::crit_exts_c_");
  }
}
rrc_conn_recfg_complete_nb_s::crit_exts_c_::crit_exts_c_(const rrc_conn_recfg_complete_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_recfg_complete_r13:
      c.init(other.c.get<rrc_conn_recfg_complete_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_complete_nb_s::crit_exts_c_");
  }
}
rrc_conn_recfg_complete_nb_s::crit_exts_c_& rrc_conn_recfg_complete_nb_s::crit_exts_c_::
                                            operator=(const rrc_conn_recfg_complete_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_recfg_complete_r13:
      c.set(other.c.get<rrc_conn_recfg_complete_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_complete_nb_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_recfg_complete_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_recfg_complete_r13:
      j.write_fieldname("rrcConnectionReconfigurationComplete-r13");
      c.get<rrc_conn_recfg_complete_nb_r13_ies_s>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_complete_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_recfg_complete_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_recfg_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_recfg_complete_nb_r13_ies_s>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_complete_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_complete_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_recfg_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_recfg_complete_nb_r13_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_complete_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_conn_recfg_complete_nb_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReconfigurationComplete-r13", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_recfg_complete_nb_s::crit_exts_c_::types");
}

// RRCConnectionReestablishmentComplete-NB-v1470-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_complete_nb_v1470_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_result_serv_cell_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_result_serv_cell_r14_present) {
    HANDLE_CODE(meas_result_serv_cell_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_complete_nb_v1470_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_result_serv_cell_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_result_serv_cell_r14_present) {
    HANDLE_CODE(meas_result_serv_cell_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_complete_nb_v1470_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_result_serv_cell_r14_present) {
    j.write_fieldname("measResultServCell-r14");
    meas_result_serv_cell_r14.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RRCConnectionReestablishmentComplete-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_complete_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_complete_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_complete_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReestablishmentComplete-NB ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_complete_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_complete_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_complete_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_reest_complete_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_reest_complete_r13:
      c.destroy<rrc_conn_reest_complete_nb_r13_ies_s>();
      break;
    default:
      break;
  }
}
void rrc_conn_reest_complete_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_reest_complete_r13:
      c.init<rrc_conn_reest_complete_nb_r13_ies_s>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_complete_nb_s::crit_exts_c_");
  }
}
rrc_conn_reest_complete_nb_s::crit_exts_c_::crit_exts_c_(const rrc_conn_reest_complete_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_reest_complete_r13:
      c.init(other.c.get<rrc_conn_reest_complete_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_complete_nb_s::crit_exts_c_");
  }
}
rrc_conn_reest_complete_nb_s::crit_exts_c_& rrc_conn_reest_complete_nb_s::crit_exts_c_::
                                            operator=(const rrc_conn_reest_complete_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_reest_complete_r13:
      c.set(other.c.get<rrc_conn_reest_complete_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_complete_nb_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_reest_complete_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_reest_complete_r13:
      j.write_fieldname("rrcConnectionReestablishmentComplete-r13");
      c.get<rrc_conn_reest_complete_nb_r13_ies_s>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_complete_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_reest_complete_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_reest_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_reest_complete_nb_r13_ies_s>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_complete_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_complete_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_reest_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_reest_complete_nb_r13_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_complete_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_conn_reest_complete_nb_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishmentComplete-r13", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_reest_complete_nb_s::crit_exts_c_::types");
}

// CQI-NPDCCH-NB-r14 ::= ENUMERATED
std::string cqi_npdcch_nb_r14_opts::to_string() const
{
  static const char* options[] = {"noMeasurements",
                                  "candidateRep-A",
                                  "candidateRep-B",
                                  "candidateRep-C",
                                  "candidateRep-D",
                                  "candidateRep-E",
                                  "candidateRep-F",
                                  "candidateRep-G",
                                  "candidateRep-H",
                                  "candidateRep-I",
                                  "candidateRep-J",
                                  "candidateRep-K",
                                  "candidateRep-L"};
  return convert_enum_idx(options, 13, value, "cqi_npdcch_nb_r14_e");
}

// CQI-NPDCCH-Short-NB-r14 ::= ENUMERATED
std::string cqi_npdcch_short_nb_r14_opts::to_string() const
{
  static const char* options[] = {"noMeasurements", "candidateRep-1", "candidateRep-2", "candidateRep-3"};
  return convert_enum_idx(options, 4, value, "cqi_npdcch_short_nb_r14_e");
}
int8_t cqi_npdcch_short_nb_r14_opts::to_number() const
{
  switch (value) {
    case candidate_rep_minus1:
      return -1;
    case candidate_rep_minus2:
      return -2;
    case candidate_rep_minus3:
      return -3;
    default:
      invalid_enum_number(value, "cqi_npdcch_short_nb_r14_e");
  }
  return 0;
}

// ReestabUE-Identity-CP-NB-r14 ::= SEQUENCE
SRSASN_CODE reestab_ue_id_cp_nb_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(s_tmsi_r14.pack(bref));
  HANDLE_CODE(ul_nas_mac_r14.pack(bref));
  HANDLE_CODE(ul_nas_count_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE reestab_ue_id_cp_nb_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(s_tmsi_r14.unpack(bref));
  HANDLE_CODE(ul_nas_mac_r14.unpack(bref));
  HANDLE_CODE(ul_nas_count_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void reestab_ue_id_cp_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("s-TMSI-r14");
  s_tmsi_r14.to_json(j);
  j.write_str("ul-NAS-MAC-r14", ul_nas_mac_r14.to_string());
  j.write_str("ul-NAS-Count-r14", ul_nas_count_r14.to_string());
  j.end_obj();
}

// ReestablishmentCause-NB-r13 ::= ENUMERATED
std::string reest_cause_nb_r13_opts::to_string() const
{
  static const char* options[] = {"reconfigurationFailure", "otherFailure", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "reest_cause_nb_r13_e");
}

// RRCConnectionReestablishmentRequest-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_request_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ue_id_r13.pack(bref));
  HANDLE_CODE(reest_cause_r13.pack(bref));
  HANDLE_CODE(cqi_npdcch_r14.pack(bref));
  HANDLE_CODE(bref.pack(early_contention_resolution_r14, 1));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_request_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ue_id_r13.unpack(bref));
  HANDLE_CODE(reest_cause_r13.unpack(bref));
  HANDLE_CODE(cqi_npdcch_r14.unpack(bref));
  HANDLE_CODE(bref.unpack(early_contention_resolution_r14, 1));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_request_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity-r13");
  ue_id_r13.to_json(j);
  j.write_str("reestablishmentCause-r13", reest_cause_r13.to_string());
  j.write_str("cqi-NPDCCH-r14", cqi_npdcch_r14.to_string());
  j.write_bool("earlyContentionResolution-r14", early_contention_resolution_r14);
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCConnectionReestablishmentRequest-NB-r14-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_request_nb_r14_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ue_id_r14.pack(bref));
  HANDLE_CODE(reest_cause_r14.pack(bref));
  HANDLE_CODE(cqi_npdcch_r14.pack(bref));
  HANDLE_CODE(bref.pack(early_contention_resolution_r14, 1));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_request_nb_r14_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ue_id_r14.unpack(bref));
  HANDLE_CODE(reest_cause_r14.unpack(bref));
  HANDLE_CODE(cqi_npdcch_r14.unpack(bref));
  HANDLE_CODE(bref.unpack(early_contention_resolution_r14, 1));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_request_nb_r14_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity-r14");
  ue_id_r14.to_json(j);
  j.write_str("reestablishmentCause-r14", reest_cause_r14.to_string());
  j.write_str("cqi-NPDCCH-r14", cqi_npdcch_r14.to_string());
  j.write_bool("earlyContentionResolution-r14", early_contention_resolution_r14);
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCConnectionReestablishmentRequest-NB ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_request_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_request_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_request_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_reest_request_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_reest_request_r13:
      c.destroy<rrc_conn_reest_request_nb_r13_ies_s>();
      break;
    case types::later:
      c.destroy<later_c_>();
      break;
    default:
      break;
  }
}
void rrc_conn_reest_request_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_reest_request_r13:
      c.init<rrc_conn_reest_request_nb_r13_ies_s>();
      break;
    case types::later:
      c.init<later_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_nb_s::crit_exts_c_");
  }
}
rrc_conn_reest_request_nb_s::crit_exts_c_::crit_exts_c_(const rrc_conn_reest_request_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_reest_request_r13:
      c.init(other.c.get<rrc_conn_reest_request_nb_r13_ies_s>());
      break;
    case types::later:
      c.init(other.c.get<later_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_nb_s::crit_exts_c_");
  }
}
rrc_conn_reest_request_nb_s::crit_exts_c_& rrc_conn_reest_request_nb_s::crit_exts_c_::
                                           operator=(const rrc_conn_reest_request_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_reest_request_r13:
      c.set(other.c.get<rrc_conn_reest_request_nb_r13_ies_s>());
      break;
    case types::later:
      c.set(other.c.get<later_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_nb_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_reest_request_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_reest_request_r13:
      j.write_fieldname("rrcConnectionReestablishmentRequest-r13");
      c.get<rrc_conn_reest_request_nb_r13_ies_s>().to_json(j);
      break;
    case types::later:
      j.write_fieldname("later");
      c.get<later_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_reest_request_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_reest_request_r13:
      HANDLE_CODE(c.get<rrc_conn_reest_request_nb_r13_ies_s>().pack(bref));
      break;
    case types::later:
      HANDLE_CODE(c.get<later_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_request_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_reest_request_r13:
      HANDLE_CODE(c.get<rrc_conn_reest_request_nb_r13_ies_s>().unpack(bref));
      break;
    case types::later:
      HANDLE_CODE(c.get<later_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_reest_request_r14:
      c.destroy<rrc_conn_reest_request_nb_r14_ies_s>();
      break;
    default:
      break;
  }
}
void rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_reest_request_r14:
      c.init<rrc_conn_reest_request_nb_r14_ies_s>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_");
  }
}
rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_::later_c_(
    const rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_reest_request_r14:
      c.init(other.c.get<rrc_conn_reest_request_nb_r14_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_");
  }
}
rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_& rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_::
                                                     operator=(const rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_reest_request_r14:
      c.set(other.c.get<rrc_conn_reest_request_nb_r14_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_");
  }

  return *this;
}
void rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_reest_request_r14:
      j.write_fieldname("rrcConnectionReestablishmentRequest-r14");
      c.get<rrc_conn_reest_request_nb_r14_ies_s>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_reest_request_r14:
      HANDLE_CODE(c.get<rrc_conn_reest_request_nb_r14_ies_s>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_reest_request_r14:
      HANDLE_CODE(c.get<rrc_conn_reest_request_nb_r14_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishmentRequest-r14", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_reest_request_nb_s::crit_exts_c_::later_c_::types");
}

std::string rrc_conn_reest_request_nb_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishmentRequest-r13", "later"};
  return convert_enum_idx(options, 2, value, "rrc_conn_reest_request_nb_s::crit_exts_c_::types");
}

// EstablishmentCause-NB-r13 ::= ENUMERATED
std::string establishment_cause_nb_r13_opts::to_string() const
{
  static const char* options[] = {"mt-Access",
                                  "mo-Signalling",
                                  "mo-Data",
                                  "mo-ExceptionData",
                                  "delayTolerantAccess-v1330",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 8, value, "establishment_cause_nb_r13_e");
}

// RRCConnectionRequest-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_request_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(multi_tone_support_r13_present, 1));
  HANDLE_CODE(bref.pack(multi_carrier_support_r13_present, 1));

  HANDLE_CODE(ue_id_r13.pack(bref));
  HANDLE_CODE(establishment_cause_r13.pack(bref));
  HANDLE_CODE(bref.pack(early_contention_resolution_r14, 1));
  HANDLE_CODE(cqi_npdcch_r14.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_request_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(multi_tone_support_r13_present, 1));
  HANDLE_CODE(bref.unpack(multi_carrier_support_r13_present, 1));

  HANDLE_CODE(ue_id_r13.unpack(bref));
  HANDLE_CODE(establishment_cause_r13.unpack(bref));
  HANDLE_CODE(bref.unpack(early_contention_resolution_r14, 1));
  HANDLE_CODE(cqi_npdcch_r14.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_request_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity-r13");
  ue_id_r13.to_json(j);
  j.write_str("establishmentCause-r13", establishment_cause_r13.to_string());
  if (multi_tone_support_r13_present) {
    j.write_str("multiToneSupport-r13", "true");
  }
  if (multi_carrier_support_r13_present) {
    j.write_str("multiCarrierSupport-r13", "true");
  }
  j.write_bool("earlyContentionResolution-r14", early_contention_resolution_r14);
  j.write_str("cqi-NPDCCH-r14", cqi_npdcch_r14.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCConnectionRequest-NB ::= SEQUENCE
SRSASN_CODE rrc_conn_request_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_request_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_request_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_request_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_request_r13:
      c.destroy<rrc_conn_request_nb_r13_ies_s>();
      break;
    default:
      break;
  }
}
void rrc_conn_request_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_request_r13:
      c.init<rrc_conn_request_nb_r13_ies_s>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_request_nb_s::crit_exts_c_");
  }
}
rrc_conn_request_nb_s::crit_exts_c_::crit_exts_c_(const rrc_conn_request_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_request_r13:
      c.init(other.c.get<rrc_conn_request_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_request_nb_s::crit_exts_c_");
  }
}
rrc_conn_request_nb_s::crit_exts_c_& rrc_conn_request_nb_s::crit_exts_c_::
                                     operator=(const rrc_conn_request_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_request_r13:
      c.set(other.c.get<rrc_conn_request_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_request_nb_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_request_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_request_r13:
      j.write_fieldname("rrcConnectionRequest-r13");
      c.get<rrc_conn_request_nb_r13_ies_s>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_request_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_request_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_request_r13:
      HANDLE_CODE(c.get<rrc_conn_request_nb_r13_ies_s>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_request_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_request_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_request_r13:
      HANDLE_CODE(c.get<rrc_conn_request_nb_r13_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_request_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_conn_request_nb_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionRequest-r13", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_request_nb_s::crit_exts_c_::types");
}

// RRCConnectionResumeComplete-NB-v1470-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_complete_nb_v1470_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_result_serv_cell_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_result_serv_cell_r14_present) {
    HANDLE_CODE(meas_result_serv_cell_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_complete_nb_v1470_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_result_serv_cell_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_result_serv_cell_r14_present) {
    HANDLE_CODE(meas_result_serv_cell_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_complete_nb_v1470_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_result_serv_cell_r14_present) {
    j.write_fieldname("measResultServCell-r14");
    meas_result_serv_cell_r14.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RRCConnectionResumeComplete-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_complete_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sel_plmn_id_r13_present, 1));
  HANDLE_CODE(bref.pack(ded_info_nas_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sel_plmn_id_r13_present) {
    HANDLE_CODE(pack_integer(bref, sel_plmn_id_r13, (uint8_t)1u, (uint8_t)6u));
  }
  if (ded_info_nas_r13_present) {
    HANDLE_CODE(ded_info_nas_r13.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_complete_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sel_plmn_id_r13_present, 1));
  HANDLE_CODE(bref.unpack(ded_info_nas_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sel_plmn_id_r13_present) {
    HANDLE_CODE(unpack_integer(sel_plmn_id_r13, bref, (uint8_t)1u, (uint8_t)6u));
  }
  if (ded_info_nas_r13_present) {
    HANDLE_CODE(ded_info_nas_r13.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_complete_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sel_plmn_id_r13_present) {
    j.write_int("selectedPLMN-Identity-r13", sel_plmn_id_r13);
  }
  if (ded_info_nas_r13_present) {
    j.write_str("dedicatedInfoNAS-r13", ded_info_nas_r13.to_string());
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionResumeComplete-NB ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_complete_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_complete_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_complete_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_resume_complete_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_resume_complete_r13:
      c.destroy<rrc_conn_resume_complete_nb_r13_ies_s>();
      break;
    default:
      break;
  }
}
void rrc_conn_resume_complete_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_resume_complete_r13:
      c.init<rrc_conn_resume_complete_nb_r13_ies_s>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_complete_nb_s::crit_exts_c_");
  }
}
rrc_conn_resume_complete_nb_s::crit_exts_c_::crit_exts_c_(const rrc_conn_resume_complete_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_resume_complete_r13:
      c.init(other.c.get<rrc_conn_resume_complete_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_complete_nb_s::crit_exts_c_");
  }
}
rrc_conn_resume_complete_nb_s::crit_exts_c_& rrc_conn_resume_complete_nb_s::crit_exts_c_::
                                             operator=(const rrc_conn_resume_complete_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_resume_complete_r13:
      c.set(other.c.get<rrc_conn_resume_complete_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_complete_nb_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_resume_complete_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_resume_complete_r13:
      j.write_fieldname("rrcConnectionResumeComplete-r13");
      c.get<rrc_conn_resume_complete_nb_r13_ies_s>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_complete_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_resume_complete_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_resume_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_complete_nb_r13_ies_s>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_complete_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_complete_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_resume_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_complete_nb_r13_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_complete_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_conn_resume_complete_nb_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionResumeComplete-r13", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_resume_complete_nb_s::crit_exts_c_::types");
}

// RRCConnectionResumeRequest-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_request_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(resume_id_r13.pack(bref));
  HANDLE_CODE(short_resume_mac_i_r13.pack(bref));
  HANDLE_CODE(resume_cause_r13.pack(bref));
  HANDLE_CODE(bref.pack(early_contention_resolution_r14, 1));
  HANDLE_CODE(cqi_npdcch_r14.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_request_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(resume_id_r13.unpack(bref));
  HANDLE_CODE(short_resume_mac_i_r13.unpack(bref));
  HANDLE_CODE(resume_cause_r13.unpack(bref));
  HANDLE_CODE(bref.unpack(early_contention_resolution_r14, 1));
  HANDLE_CODE(cqi_npdcch_r14.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_request_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("resumeID-r13", resume_id_r13.to_string());
  j.write_str("shortResumeMAC-I-r13", short_resume_mac_i_r13.to_string());
  j.write_str("resumeCause-r13", resume_cause_r13.to_string());
  j.write_bool("earlyContentionResolution-r14", early_contention_resolution_r14);
  j.write_str("cqi-NPDCCH-r14", cqi_npdcch_r14.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCConnectionResumeRequest-NB ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_request_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_request_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_request_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_resume_request_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_resume_request_r13:
      c.destroy<rrc_conn_resume_request_nb_r13_ies_s>();
      break;
    default:
      break;
  }
}
void rrc_conn_resume_request_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_resume_request_r13:
      c.init<rrc_conn_resume_request_nb_r13_ies_s>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_nb_s::crit_exts_c_");
  }
}
rrc_conn_resume_request_nb_s::crit_exts_c_::crit_exts_c_(const rrc_conn_resume_request_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_resume_request_r13:
      c.init(other.c.get<rrc_conn_resume_request_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_nb_s::crit_exts_c_");
  }
}
rrc_conn_resume_request_nb_s::crit_exts_c_& rrc_conn_resume_request_nb_s::crit_exts_c_::
                                            operator=(const rrc_conn_resume_request_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_resume_request_r13:
      c.set(other.c.get<rrc_conn_resume_request_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_nb_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_resume_request_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_resume_request_r13:
      j.write_fieldname("rrcConnectionResumeRequest-r13");
      c.get<rrc_conn_resume_request_nb_r13_ies_s>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_resume_request_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_resume_request_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_request_nb_r13_ies_s>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_request_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_resume_request_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_request_nb_r13_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_conn_resume_request_nb_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionResumeRequest-r13", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_resume_request_nb_s::crit_exts_c_::types");
}

// RRCConnectionSetupComplete-NB-v1470-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_nb_v1470_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_result_serv_cell_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_result_serv_cell_r14_present) {
    HANDLE_CODE(meas_result_serv_cell_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_nb_v1470_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_result_serv_cell_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_result_serv_cell_r14_present) {
    HANDLE_CODE(meas_result_serv_cell_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_complete_nb_v1470_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_result_serv_cell_r14_present) {
    j.write_fieldname("measResultServCell-r14");
    meas_result_serv_cell_r14.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RRCConnectionSetupComplete-NB-v1430-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_nb_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gummei_type_r14_present, 1));
  HANDLE_CODE(bref.pack(dcn_id_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (dcn_id_r14_present) {
    HANDLE_CODE(pack_integer(bref, dcn_id_r14, (uint32_t)0u, (uint32_t)65535u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_nb_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gummei_type_r14_present, 1));
  HANDLE_CODE(bref.unpack(dcn_id_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (dcn_id_r14_present) {
    HANDLE_CODE(unpack_integer(dcn_id_r14, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_complete_nb_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gummei_type_r14_present) {
    j.write_str("gummei-Type-r14", "mapped");
  }
  if (dcn_id_r14_present) {
    j.write_int("dcn-ID-r14", dcn_id_r14);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionSetupComplete-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(s_tmsi_r13_present, 1));
  HANDLE_CODE(bref.pack(registered_mme_r13_present, 1));
  HANDLE_CODE(bref.pack(attach_without_pdn_connect_r13_present, 1));
  HANDLE_CODE(bref.pack(up_cio_t_eps_optim_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_integer(bref, sel_plmn_id_r13, (uint8_t)1u, (uint8_t)6u));
  if (s_tmsi_r13_present) {
    HANDLE_CODE(s_tmsi_r13.pack(bref));
  }
  if (registered_mme_r13_present) {
    HANDLE_CODE(registered_mme_r13.pack(bref));
  }
  HANDLE_CODE(ded_info_nas_r13.pack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(s_tmsi_r13_present, 1));
  HANDLE_CODE(bref.unpack(registered_mme_r13_present, 1));
  HANDLE_CODE(bref.unpack(attach_without_pdn_connect_r13_present, 1));
  HANDLE_CODE(bref.unpack(up_cio_t_eps_optim_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_integer(sel_plmn_id_r13, bref, (uint8_t)1u, (uint8_t)6u));
  if (s_tmsi_r13_present) {
    HANDLE_CODE(s_tmsi_r13.unpack(bref));
  }
  if (registered_mme_r13_present) {
    HANDLE_CODE(registered_mme_r13.unpack(bref));
  }
  HANDLE_CODE(ded_info_nas_r13.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_complete_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("selectedPLMN-Identity-r13", sel_plmn_id_r13);
  if (s_tmsi_r13_present) {
    j.write_fieldname("s-TMSI-r13");
    s_tmsi_r13.to_json(j);
  }
  if (registered_mme_r13_present) {
    j.write_fieldname("registeredMME-r13");
    registered_mme_r13.to_json(j);
  }
  j.write_str("dedicatedInfoNAS-r13", ded_info_nas_r13.to_string());
  if (attach_without_pdn_connect_r13_present) {
    j.write_str("attachWithoutPDN-Connectivity-r13", "true");
  }
  if (up_cio_t_eps_optim_r13_present) {
    j.write_str("up-CIoT-EPS-Optimisation-r13", "true");
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionSetupComplete-NB ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_complete_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_setup_complete_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_setup_complete_r13:
      c.destroy<rrc_conn_setup_complete_nb_r13_ies_s>();
      break;
    default:
      break;
  }
}
void rrc_conn_setup_complete_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_setup_complete_r13:
      c.init<rrc_conn_setup_complete_nb_r13_ies_s>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_nb_s::crit_exts_c_");
  }
}
rrc_conn_setup_complete_nb_s::crit_exts_c_::crit_exts_c_(const rrc_conn_setup_complete_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_setup_complete_r13:
      c.init(other.c.get<rrc_conn_setup_complete_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_nb_s::crit_exts_c_");
  }
}
rrc_conn_setup_complete_nb_s::crit_exts_c_& rrc_conn_setup_complete_nb_s::crit_exts_c_::
                                            operator=(const rrc_conn_setup_complete_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_setup_complete_r13:
      c.set(other.c.get<rrc_conn_setup_complete_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_nb_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_setup_complete_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_setup_complete_r13:
      j.write_fieldname("rrcConnectionSetupComplete-r13");
      c.get<rrc_conn_setup_complete_nb_r13_ies_s>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_setup_complete_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_setup_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_setup_complete_nb_r13_ies_s>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_setup_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_setup_complete_nb_r13_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_conn_setup_complete_nb_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionSetupComplete-r13", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_setup_complete_nb_s::crit_exts_c_::types");
}

// RRCEarlyDataRequest-NB-r15-IEs ::= SEQUENCE
SRSASN_CODE rrc_early_data_request_nb_r15_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cqi_npdcch_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(s_tmsi_r15.pack(bref));
  HANDLE_CODE(establishment_cause_r15.pack(bref));
  if (cqi_npdcch_r15_present) {
    HANDLE_CODE(cqi_npdcch_r15.pack(bref));
  }
  HANDLE_CODE(ded_info_nas_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_early_data_request_nb_r15_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cqi_npdcch_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(s_tmsi_r15.unpack(bref));
  HANDLE_CODE(establishment_cause_r15.unpack(bref));
  if (cqi_npdcch_r15_present) {
    HANDLE_CODE(cqi_npdcch_r15.unpack(bref));
  }
  HANDLE_CODE(ded_info_nas_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_early_data_request_nb_r15_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("s-TMSI-r15");
  s_tmsi_r15.to_json(j);
  j.write_str("establishmentCause-r15", establishment_cause_r15.to_string());
  if (cqi_npdcch_r15_present) {
    j.write_str("cqi-NPDCCH-r15", cqi_npdcch_r15.to_string());
  }
  j.write_str("dedicatedInfoNAS-r15", ded_info_nas_r15.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

std::string rrc_early_data_request_nb_r15_ies_s::establishment_cause_r15_opts::to_string() const
{
  static const char* options[] = {"mo-Data-r15", "mo-ExceptionData-r15", "delayTolerantAccess-r15", "spare1"};
  return convert_enum_idx(options, 4, value, "rrc_early_data_request_nb_r15_ies_s::establishment_cause_r15_e_");
}

// RRCEarlyDataRequest-NB-r15 ::= SEQUENCE
SRSASN_CODE rrc_early_data_request_nb_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_early_data_request_nb_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_early_data_request_nb_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_early_data_request_nb_r15_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_early_data_request_r15:
      c.destroy<rrc_early_data_request_nb_r15_ies_s>();
      break;
    default:
      break;
  }
}
void rrc_early_data_request_nb_r15_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_early_data_request_r15:
      c.init<rrc_early_data_request_nb_r15_ies_s>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_request_nb_r15_s::crit_exts_c_");
  }
}
rrc_early_data_request_nb_r15_s::crit_exts_c_::crit_exts_c_(const rrc_early_data_request_nb_r15_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_early_data_request_r15:
      c.init(other.c.get<rrc_early_data_request_nb_r15_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_request_nb_r15_s::crit_exts_c_");
  }
}
rrc_early_data_request_nb_r15_s::crit_exts_c_& rrc_early_data_request_nb_r15_s::crit_exts_c_::
                                               operator=(const rrc_early_data_request_nb_r15_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_early_data_request_r15:
      c.set(other.c.get<rrc_early_data_request_nb_r15_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_request_nb_r15_s::crit_exts_c_");
  }

  return *this;
}
void rrc_early_data_request_nb_r15_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_early_data_request_r15:
      j.write_fieldname("rrcEarlyDataRequest-r15");
      c.get<rrc_early_data_request_nb_r15_ies_s>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_request_nb_r15_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_early_data_request_nb_r15_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_early_data_request_r15:
      HANDLE_CODE(c.get<rrc_early_data_request_nb_r15_ies_s>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_request_nb_r15_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_early_data_request_nb_r15_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_early_data_request_r15:
      HANDLE_CODE(c.get<rrc_early_data_request_nb_r15_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_request_nb_r15_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string rrc_early_data_request_nb_r15_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcEarlyDataRequest-r15", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_early_data_request_nb_r15_s::crit_exts_c_::types");
}

// SC-MTCH-SchedulingInfo-NB-r14 ::= SEQUENCE
SRSASN_CODE sc_mtch_sched_info_nb_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(on_dur_timer_scptm_r14.pack(bref));
  HANDLE_CODE(drx_inactivity_timer_scptm_r14.pack(bref));
  HANDLE_CODE(sched_period_start_offset_scptm_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mtch_sched_info_nb_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(on_dur_timer_scptm_r14.unpack(bref));
  HANDLE_CODE(drx_inactivity_timer_scptm_r14.unpack(bref));
  HANDLE_CODE(sched_period_start_offset_scptm_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void sc_mtch_sched_info_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("onDurationTimerSCPTM-r14", on_dur_timer_scptm_r14.to_string());
  j.write_str("drx-InactivityTimerSCPTM-r14", drx_inactivity_timer_scptm_r14.to_string());
  j.write_fieldname("schedulingPeriodStartOffsetSCPTM-r14");
  sched_period_start_offset_scptm_r14.to_json(j);
  j.end_obj();
}

std::string sc_mtch_sched_info_nb_r14_s::on_dur_timer_scptm_r14_opts::to_string() const
{
  static const char* options[] = {"pp1", "pp2", "pp3", "pp4", "pp8", "pp16", "pp32", "spare"};
  return convert_enum_idx(options, 8, value, "sc_mtch_sched_info_nb_r14_s::on_dur_timer_scptm_r14_e_");
}
uint8_t sc_mtch_sched_info_nb_r14_s::on_dur_timer_scptm_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 8, 16, 32};
  return map_enum_number(options, 7, value, "sc_mtch_sched_info_nb_r14_s::on_dur_timer_scptm_r14_e_");
}

std::string sc_mtch_sched_info_nb_r14_s::drx_inactivity_timer_scptm_r14_opts::to_string() const
{
  static const char* options[] = {"pp0", "pp1", "pp2", "pp3", "pp4", "pp8", "pp16", "pp32"};
  return convert_enum_idx(options, 8, value, "sc_mtch_sched_info_nb_r14_s::drx_inactivity_timer_scptm_r14_e_");
}
uint8_t sc_mtch_sched_info_nb_r14_s::drx_inactivity_timer_scptm_r14_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 8, 16, 32};
  return map_enum_number(options, 8, value, "sc_mtch_sched_info_nb_r14_s::drx_inactivity_timer_scptm_r14_e_");
}

void sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::destroy_() {}
void sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::sched_period_start_offset_scptm_r14_c_(
    const sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf32:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf64:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf128:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf256:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf512:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf640:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf1024:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf2048:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf4096:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf8192:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_");
  }
}
sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_&
sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::
operator=(const sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf32:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf64:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf128:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf256:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf512:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf640:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf1024:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf2048:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf4096:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf8192:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_");
  }

  return *this;
}
void sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf10:
      j.write_int("sf10", c.get<uint8_t>());
      break;
    case types::sf20:
      j.write_int("sf20", c.get<uint8_t>());
      break;
    case types::sf32:
      j.write_int("sf32", c.get<uint8_t>());
      break;
    case types::sf40:
      j.write_int("sf40", c.get<uint8_t>());
      break;
    case types::sf64:
      j.write_int("sf64", c.get<uint8_t>());
      break;
    case types::sf80:
      j.write_int("sf80", c.get<uint8_t>());
      break;
    case types::sf128:
      j.write_int("sf128", c.get<uint8_t>());
      break;
    case types::sf160:
      j.write_int("sf160", c.get<uint8_t>());
      break;
    case types::sf256:
      j.write_int("sf256", c.get<uint16_t>());
      break;
    case types::sf320:
      j.write_int("sf320", c.get<uint16_t>());
      break;
    case types::sf512:
      j.write_int("sf512", c.get<uint16_t>());
      break;
    case types::sf640:
      j.write_int("sf640", c.get<uint16_t>());
      break;
    case types::sf1024:
      j.write_int("sf1024", c.get<uint16_t>());
      break;
    case types::sf2048:
      j.write_int("sf2048", c.get<uint16_t>());
      break;
    case types::sf4096:
      j.write_int("sf4096", c.get<uint16_t>());
      break;
    case types::sf8192:
      j.write_int("sf8192", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf32:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sf40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::sf80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf128:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sf160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sf256:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sf320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sf512:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::sf640:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sf1024:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::sf2048:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::sf4096:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u));
      break;
    case types::sf8192:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)8191u));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf32:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sf40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::sf80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf128:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sf160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sf256:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sf320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sf512:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::sf640:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sf1024:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::sf2048:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::sf4096:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u));
      break;
    case types::sf8192:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)8191u));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"sf10",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf256",
                                  "sf320",
                                  "sf512",
                                  "sf640",
                                  "sf1024",
                                  "sf2048",
                                  "sf4096",
                                  "sf8192"};
  return convert_enum_idx(
      options, 16, value, "sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::types");
}
uint16_t sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1024, 2048, 4096, 8192};
  return map_enum_number(
      options, 16, value, "sc_mtch_sched_info_nb_r14_s::sched_period_start_offset_scptm_r14_c_::types");
}

// SC-MTCH-Info-NB-r14 ::= SEQUENCE
SRSASN_CODE sc_mtch_info_nb_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sc_mtch_sched_info_r14_present, 1));
  HANDLE_CODE(bref.pack(sc_mtch_neighbour_cell_r14_present, 1));

  HANDLE_CODE(sc_mtch_carrier_cfg_r14.pack(bref));
  HANDLE_CODE(mbms_session_info_r14.pack(bref));
  HANDLE_CODE(g_rnti_r14.pack(bref));
  if (sc_mtch_sched_info_r14_present) {
    HANDLE_CODE(sc_mtch_sched_info_r14.pack(bref));
  }
  if (sc_mtch_neighbour_cell_r14_present) {
    HANDLE_CODE(sc_mtch_neighbour_cell_r14.pack(bref));
  }
  HANDLE_CODE(npdcch_npdsch_max_tbs_sc_mtch_r14.pack(bref));
  HANDLE_CODE(npdcch_num_repeats_sc_mtch_r14.pack(bref));
  HANDLE_CODE(npdcch_start_sf_sc_mtch_r14.pack(bref));
  HANDLE_CODE(npdcch_offset_sc_mtch_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mtch_info_nb_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sc_mtch_sched_info_r14_present, 1));
  HANDLE_CODE(bref.unpack(sc_mtch_neighbour_cell_r14_present, 1));

  HANDLE_CODE(sc_mtch_carrier_cfg_r14.unpack(bref));
  HANDLE_CODE(mbms_session_info_r14.unpack(bref));
  HANDLE_CODE(g_rnti_r14.unpack(bref));
  if (sc_mtch_sched_info_r14_present) {
    HANDLE_CODE(sc_mtch_sched_info_r14.unpack(bref));
  }
  if (sc_mtch_neighbour_cell_r14_present) {
    HANDLE_CODE(sc_mtch_neighbour_cell_r14.unpack(bref));
  }
  HANDLE_CODE(npdcch_npdsch_max_tbs_sc_mtch_r14.unpack(bref));
  HANDLE_CODE(npdcch_num_repeats_sc_mtch_r14.unpack(bref));
  HANDLE_CODE(npdcch_start_sf_sc_mtch_r14.unpack(bref));
  HANDLE_CODE(npdcch_offset_sc_mtch_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void sc_mtch_info_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sc-mtch-CarrierConfig-r14");
  sc_mtch_carrier_cfg_r14.to_json(j);
  j.write_fieldname("mbmsSessionInfo-r14");
  mbms_session_info_r14.to_json(j);
  j.write_str("g-RNTI-r14", g_rnti_r14.to_string());
  if (sc_mtch_sched_info_r14_present) {
    j.write_fieldname("sc-mtch-SchedulingInfo-r14");
    sc_mtch_sched_info_r14.to_json(j);
  }
  if (sc_mtch_neighbour_cell_r14_present) {
    j.write_str("sc-mtch-NeighbourCell-r14", sc_mtch_neighbour_cell_r14.to_string());
  }
  j.write_str("npdcch-NPDSCH-MaxTBS-SC-MTCH-r14", npdcch_npdsch_max_tbs_sc_mtch_r14.to_string());
  j.write_str("npdcch-NumRepetitions-SC-MTCH-r14", npdcch_num_repeats_sc_mtch_r14.to_string());
  j.write_str("npdcch-StartSF-SC-MTCH-r14", npdcch_start_sf_sc_mtch_r14.to_string());
  j.write_str("npdcch-Offset-SC-MTCH-r14", npdcch_offset_sc_mtch_r14.to_string());
  j.end_obj();
}

void sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_::destroy_()
{
  switch (type_) {
    case types::dl_carrier_cfg_r14:
      c.destroy<dl_carrier_cfg_common_nb_r14_s>();
      break;
    default:
      break;
  }
}
void sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::dl_carrier_cfg_r14:
      c.init<dl_carrier_cfg_common_nb_r14_s>();
      break;
    case types::dl_carrier_idx_r14:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_");
  }
}
sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_::sc_mtch_carrier_cfg_r14_c_(
    const sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::dl_carrier_cfg_r14:
      c.init(other.c.get<dl_carrier_cfg_common_nb_r14_s>());
      break;
    case types::dl_carrier_idx_r14:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_");
  }
}
sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_& sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_::
                                                   operator=(const sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::dl_carrier_cfg_r14:
      c.set(other.c.get<dl_carrier_cfg_common_nb_r14_s>());
      break;
    case types::dl_carrier_idx_r14:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_");
  }

  return *this;
}
void sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::dl_carrier_cfg_r14:
      j.write_fieldname("dl-CarrierConfig-r14");
      c.get<dl_carrier_cfg_common_nb_r14_s>().to_json(j);
      break;
    case types::dl_carrier_idx_r14:
      j.write_int("dl-CarrierIndex-r14", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::dl_carrier_cfg_r14:
      HANDLE_CODE(c.get<dl_carrier_cfg_common_nb_r14_s>().pack(bref));
      break;
    case types::dl_carrier_idx_r14:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::dl_carrier_cfg_r14:
      HANDLE_CODE(c.get<dl_carrier_cfg_common_nb_r14_s>().unpack(bref));
      break;
    case types::dl_carrier_idx_r14:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"dl-CarrierConfig-r14", "dl-CarrierIndex-r14"};
  return convert_enum_idx(options, 2, value, "sc_mtch_info_nb_r14_s::sc_mtch_carrier_cfg_r14_c_::types");
}

std::string sc_mtch_info_nb_r14_s::npdcch_npdsch_max_tbs_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"n680", "n2536"};
  return convert_enum_idx(options, 2, value, "sc_mtch_info_nb_r14_s::npdcch_npdsch_max_tbs_sc_mtch_r14_e_");
}
uint16_t sc_mtch_info_nb_r14_s::npdcch_npdsch_max_tbs_sc_mtch_r14_opts::to_number() const
{
  static const uint16_t options[] = {680, 2536};
  return map_enum_number(options, 2, value, "sc_mtch_info_nb_r14_s::npdcch_npdsch_max_tbs_sc_mtch_r14_e_");
}

std::string sc_mtch_info_nb_r14_s::npdcch_num_repeats_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"r1",
                                  "r2",
                                  "r4",
                                  "r8",
                                  "r16",
                                  "r32",
                                  "r64",
                                  "r128",
                                  "r256",
                                  "r512",
                                  "r1024",
                                  "r2048",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sc_mtch_info_nb_r14_s::npdcch_num_repeats_sc_mtch_r14_e_");
}
uint16_t sc_mtch_info_nb_r14_s::npdcch_num_repeats_sc_mtch_r14_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
  return map_enum_number(options, 12, value, "sc_mtch_info_nb_r14_s::npdcch_num_repeats_sc_mtch_r14_e_");
}

std::string sc_mtch_info_nb_r14_s::npdcch_start_sf_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"v1dot5", "v2", "v4", "v8", "v16", "v32", "v48", "v64"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_nb_r14_s::npdcch_start_sf_sc_mtch_r14_e_");
}
float sc_mtch_info_nb_r14_s::npdcch_start_sf_sc_mtch_r14_opts::to_number() const
{
  static const float options[] = {1.5, 2.0, 4.0, 8.0, 16.0, 32.0, 48.0, 64.0};
  return map_enum_number(options, 8, value, "sc_mtch_info_nb_r14_s::npdcch_start_sf_sc_mtch_r14_e_");
}
std::string sc_mtch_info_nb_r14_s::npdcch_start_sf_sc_mtch_r14_opts::to_number_string() const
{
  static const char* options[] = {"1.5", "2", "4", "8", "16", "32", "48", "64"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_nb_r14_s::npdcch_start_sf_sc_mtch_r14_e_");
}

std::string sc_mtch_info_nb_r14_s::npdcch_offset_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {
      "zero", "oneEighth", "oneQuarter", "threeEighth", "oneHalf", "fiveEighth", "threeQuarter", "sevenEighth"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_nb_r14_s::npdcch_offset_sc_mtch_r14_e_");
}
float sc_mtch_info_nb_r14_s::npdcch_offset_sc_mtch_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875};
  return map_enum_number(options, 8, value, "sc_mtch_info_nb_r14_s::npdcch_offset_sc_mtch_r14_e_");
}
std::string sc_mtch_info_nb_r14_s::npdcch_offset_sc_mtch_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "1/8", "1/4", "3/8", "1/2", "5/8", "3/4", "7/8"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_nb_r14_s::npdcch_offset_sc_mtch_r14_e_");
}

// SCPTMConfiguration-NB-r14 ::= SEQUENCE
SRSASN_CODE scptm_cfg_nb_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scptm_neighbour_cell_list_r14_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, sc_mtch_info_list_r14, 0, 64));
  if (scptm_neighbour_cell_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scptm_neighbour_cell_list_r14, 1, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scptm_cfg_nb_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scptm_neighbour_cell_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(sc_mtch_info_list_r14, bref, 0, 64));
  if (scptm_neighbour_cell_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scptm_neighbour_cell_list_r14, bref, 1, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scptm_cfg_nb_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("sc-mtch-InfoList-r14");
  for (const auto& e1 : sc_mtch_info_list_r14) {
    e1.to_json(j);
  }
  j.end_array();
  if (scptm_neighbour_cell_list_r14_present) {
    j.start_array("scptm-NeighbourCellList-r14");
    for (const auto& e1 : scptm_neighbour_cell_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
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

// SC-MCCH-MessageType-NB ::= CHOICE
void sc_mcch_msg_type_nb_c::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void sc_mcch_msg_type_nb_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::msg_class_ext:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_nb_c");
  }
}
sc_mcch_msg_type_nb_c::sc_mcch_msg_type_nb_c(const sc_mcch_msg_type_nb_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_nb_c");
  }
}
sc_mcch_msg_type_nb_c& sc_mcch_msg_type_nb_c::operator=(const sc_mcch_msg_type_nb_c& other)
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
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_nb_c");
  }

  return *this;
}
void sc_mcch_msg_type_nb_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_nb_c");
  }
  j.end_obj();
}
SRSASN_CODE sc_mcch_msg_type_nb_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_nb_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mcch_msg_type_nb_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_nb_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sc_mcch_msg_type_nb_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("scptmConfiguration-r14");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sc_mcch_msg_type_nb_c::c1_c_::pack(bit_ref& bref) const
{
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mcch_msg_type_nb_c::c1_c_::unpack(cbit_ref& bref)
{
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

std::string sc_mcch_msg_type_nb_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"scptmConfiguration-r14"};
  return convert_enum_idx(options, 1, value, "sc_mcch_msg_type_nb_c::c1_c_::types");
}

std::string sc_mcch_msg_type_nb_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "sc_mcch_msg_type_nb_c::types");
}
uint8_t sc_mcch_msg_type_nb_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "sc_mcch_msg_type_nb_c::types");
}

// SC-MCCH-Message-NB ::= SEQUENCE
SRSASN_CODE sc_mcch_msg_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mcch_msg_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void sc_mcch_msg_nb_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("SC-MCCH-Message-NB");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// UE-RadioPagingInfo-NB-r13 ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_nb_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ue_category_nb_r13_present, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= multi_carrier_paging_r14_present;
    group_flags[1] |= mixed_operation_mode_r15_present;
    group_flags[1] |= wake_up_signal_r15_present;
    group_flags[1] |= wake_up_signal_min_gap_e_drx_r15_present;
    group_flags[1] |= multi_carrier_paging_tdd_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(multi_carrier_paging_r14_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mixed_operation_mode_r15_present, 1));
      HANDLE_CODE(bref.pack(wake_up_signal_r15_present, 1));
      HANDLE_CODE(bref.pack(wake_up_signal_min_gap_e_drx_r15_present, 1));
      HANDLE_CODE(bref.pack(multi_carrier_paging_tdd_r15_present, 1));
      if (wake_up_signal_min_gap_e_drx_r15_present) {
        HANDLE_CODE(wake_up_signal_min_gap_e_drx_r15.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_nb_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ue_category_nb_r13_present, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(multi_carrier_paging_r14_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(mixed_operation_mode_r15_present, 1));
      HANDLE_CODE(bref.unpack(wake_up_signal_r15_present, 1));
      HANDLE_CODE(bref.unpack(wake_up_signal_min_gap_e_drx_r15_present, 1));
      HANDLE_CODE(bref.unpack(multi_carrier_paging_tdd_r15_present, 1));
      if (wake_up_signal_min_gap_e_drx_r15_present) {
        HANDLE_CODE(wake_up_signal_min_gap_e_drx_r15.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_nb_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_category_nb_r13_present) {
    j.write_str("ue-Category-NB-r13", "nb1");
  }
  if (ext) {
    if (multi_carrier_paging_r14_present) {
      j.write_str("multiCarrierPaging-r14", "true");
    }
    if (mixed_operation_mode_r15_present) {
      j.write_str("mixedOperationMode-r15", "supported");
    }
    if (wake_up_signal_r15_present) {
      j.write_str("wakeUpSignal-r15", "true");
    }
    if (wake_up_signal_min_gap_e_drx_r15_present) {
      j.write_str("wakeUpSignalMinGap-eDRX-r15", wake_up_signal_min_gap_e_drx_r15.to_string());
    }
    if (multi_carrier_paging_tdd_r15_present) {
      j.write_str("multiCarrierPagingTDD-r15", "true");
    }
  }
  j.end_obj();
}

std::string ue_radio_paging_info_nb_r13_s::wake_up_signal_min_gap_e_drx_r15_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms240", "ms1000", "ms2000"};
  return convert_enum_idx(options, 4, value, "ue_radio_paging_info_nb_r13_s::wake_up_signal_min_gap_e_drx_r15_e_");
}
uint16_t ue_radio_paging_info_nb_r13_s::wake_up_signal_min_gap_e_drx_r15_opts::to_number() const
{
  static const uint16_t options[] = {40, 240, 1000, 2000};
  return map_enum_number(options, 4, value, "ue_radio_paging_info_nb_r13_s::wake_up_signal_min_gap_e_drx_r15_e_");
}

// UECapabilityInformation-NB-Ext-r14-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_info_nb_ext_r14_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_cap_container_ext_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_nb_ext_r14_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_cap_container_ext_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_cap_info_nb_ext_r14_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ue-Capability-ContainerExt-r14", ue_cap_container_ext_r14.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UECapabilityInformation-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_info_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_cap_r13.pack(bref));
  HANDLE_CODE(ue_radio_paging_info_r13.pack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_cap_r13.unpack(bref));
  HANDLE_CODE(ue_radio_paging_info_r13.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_info_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Capability-r13");
  ue_cap_r13.to_json(j);
  j.write_fieldname("ue-RadioPagingInfo-r13");
  ue_radio_paging_info_r13.to_json(j);
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UECapabilityInformation-NB ::= SEQUENCE
SRSASN_CODE ue_cap_info_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_cap_info_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_cap_info_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::ue_cap_info_r13:
      c.destroy<ue_cap_info_nb_r13_ies_s>();
      break;
    default:
      break;
  }
}
void ue_cap_info_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ue_cap_info_r13:
      c.init<ue_cap_info_nb_r13_ies_s>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_info_nb_s::crit_exts_c_");
  }
}
ue_cap_info_nb_s::crit_exts_c_::crit_exts_c_(const ue_cap_info_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ue_cap_info_r13:
      c.init(other.c.get<ue_cap_info_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_info_nb_s::crit_exts_c_");
  }
}
ue_cap_info_nb_s::crit_exts_c_& ue_cap_info_nb_s::crit_exts_c_::operator=(const ue_cap_info_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ue_cap_info_r13:
      c.set(other.c.get<ue_cap_info_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_info_nb_s::crit_exts_c_");
  }

  return *this;
}
void ue_cap_info_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_cap_info_r13:
      j.write_fieldname("ueCapabilityInformation-r13");
      c.get<ue_cap_info_nb_r13_ies_s>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_info_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_cap_info_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_cap_info_r13:
      HANDLE_CODE(c.get<ue_cap_info_nb_r13_ies_s>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_info_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_cap_info_r13:
      HANDLE_CODE(c.get<ue_cap_info_nb_r13_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_info_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string ue_cap_info_nb_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueCapabilityInformation-r13", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "ue_cap_info_nb_s::crit_exts_c_::types");
}

// UEPagingCoverageInformation-NB-IEs ::= SEQUENCE
SRSASN_CODE ue_paging_coverage_info_nb_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(npdcch_num_repeat_paging_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (npdcch_num_repeat_paging_r13_present) {
    HANDLE_CODE(pack_integer(bref, npdcch_num_repeat_paging_r13, (uint16_t)1u, (uint16_t)2048u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_paging_coverage_info_nb_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(npdcch_num_repeat_paging_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (npdcch_num_repeat_paging_r13_present) {
    HANDLE_CODE(unpack_integer(npdcch_num_repeat_paging_r13, bref, (uint16_t)1u, (uint16_t)2048u));
  }

  return SRSASN_SUCCESS;
}
void ue_paging_coverage_info_nb_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (npdcch_num_repeat_paging_r13_present) {
    j.write_int("npdcch-NumRepetitionPaging-r13", npdcch_num_repeat_paging_r13);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UEPagingCoverageInformation-NB ::= SEQUENCE
SRSASN_CODE ue_paging_coverage_info_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_paging_coverage_info_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_paging_coverage_info_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_paging_coverage_info_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void ue_paging_coverage_info_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_paging_coverage_info_nb_s::crit_exts_c_");
  }
}
ue_paging_coverage_info_nb_s::crit_exts_c_::crit_exts_c_(const ue_paging_coverage_info_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_paging_coverage_info_nb_s::crit_exts_c_");
  }
}
ue_paging_coverage_info_nb_s::crit_exts_c_& ue_paging_coverage_info_nb_s::crit_exts_c_::
                                            operator=(const ue_paging_coverage_info_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_paging_coverage_info_nb_s::crit_exts_c_");
  }

  return *this;
}
void ue_paging_coverage_info_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_paging_coverage_info_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_paging_coverage_info_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_paging_coverage_info_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_paging_coverage_info_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_paging_coverage_info_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_paging_coverage_info_nb_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
void ue_paging_coverage_info_nb_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_paging_coverage_info_r13:
      j.write_fieldname("uePagingCoverageInformation-r13");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_paging_coverage_info_nb_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_paging_coverage_info_nb_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_paging_coverage_info_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_paging_coverage_info_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_paging_coverage_info_nb_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_paging_coverage_info_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_paging_coverage_info_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string ue_paging_coverage_info_nb_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"uePagingCoverageInformation-r13", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ue_paging_coverage_info_nb_s::crit_exts_c_::c1_c_::types");
}

// UERadioAccessCapabilityInformation-NB-r14-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_access_cap_info_nb_r14_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_radio_access_cap_info_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_radio_access_cap_info_r14_present) {
    HANDLE_CODE(ue_radio_access_cap_info_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_nb_r14_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_radio_access_cap_info_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_radio_access_cap_info_r14_present) {
    HANDLE_CODE(ue_radio_access_cap_info_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_radio_access_cap_info_nb_r14_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_radio_access_cap_info_r14_present) {
    j.write_str("ue-RadioAccessCapabilityInfo-r14", ue_radio_access_cap_info_r14.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UERadioAccessCapabilityInformation-NB-v1380-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_access_cap_info_nb_v1380_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_nb_v1380_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_radio_access_cap_info_nb_v1380_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UERadioAccessCapabilityInformation-NB-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_access_cap_info_nb_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_radio_access_cap_info_r13.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_nb_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_radio_access_cap_info_r13.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_radio_access_cap_info_nb_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ue-RadioAccessCapabilityInfo-r13", ue_radio_access_cap_info_r13.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UERadioAccessCapabilityInformation-NB ::= SEQUENCE
SRSASN_CODE ue_radio_access_cap_info_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_radio_access_cap_info_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_radio_access_cap_info_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void ue_radio_access_cap_info_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_nb_s::crit_exts_c_");
  }
}
ue_radio_access_cap_info_nb_s::crit_exts_c_::crit_exts_c_(const ue_radio_access_cap_info_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_nb_s::crit_exts_c_");
  }
}
ue_radio_access_cap_info_nb_s::crit_exts_c_& ue_radio_access_cap_info_nb_s::crit_exts_c_::
                                             operator=(const ue_radio_access_cap_info_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_nb_s::crit_exts_c_");
  }

  return *this;
}
void ue_radio_access_cap_info_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_access_cap_info_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_radio_access_cap_info_nb_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
void ue_radio_access_cap_info_nb_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_radio_access_cap_info_r13:
      j.write_fieldname("ueRadioAccessCapabilityInformation-r13");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_nb_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_access_cap_info_nb_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_radio_access_cap_info_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_nb_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_radio_access_cap_info_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string ue_radio_access_cap_info_nb_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueRadioAccessCapabilityInformation-r13", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ue_radio_access_cap_info_nb_s::crit_exts_c_::c1_c_::types");
}

// UERadioPagingInformation-NB-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_nb_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_radio_paging_info_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_nb_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_radio_paging_info_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_nb_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ue-RadioPagingInfo-r13", ue_radio_paging_info_r13.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UERadioPagingInformation-NB ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_radio_paging_info_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void ue_radio_paging_info_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_nb_s::crit_exts_c_");
  }
}
ue_radio_paging_info_nb_s::crit_exts_c_::crit_exts_c_(const ue_radio_paging_info_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_nb_s::crit_exts_c_");
  }
}
ue_radio_paging_info_nb_s::crit_exts_c_& ue_radio_paging_info_nb_s::crit_exts_c_::
                                         operator=(const ue_radio_paging_info_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_nb_s::crit_exts_c_");
  }

  return *this;
}
void ue_radio_paging_info_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_paging_info_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_radio_paging_info_nb_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
void ue_radio_paging_info_nb_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_radio_paging_info_r13:
      j.write_fieldname("ueRadioPagingInformation-r13");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_nb_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_paging_info_nb_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_radio_paging_info_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_nb_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_radio_paging_info_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_nb_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string ue_radio_paging_info_nb_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueRadioPagingInformation-r13", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ue_radio_paging_info_nb_s::crit_exts_c_::c1_c_::types");
}

// UL-CCCH-MessageType-NB ::= CHOICE
void ul_ccch_msg_type_nb_c::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void ul_ccch_msg_type_nb_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::msg_class_ext:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_nb_c");
  }
}
ul_ccch_msg_type_nb_c::ul_ccch_msg_type_nb_c(const ul_ccch_msg_type_nb_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_nb_c");
  }
}
ul_ccch_msg_type_nb_c& ul_ccch_msg_type_nb_c::operator=(const ul_ccch_msg_type_nb_c& other)
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
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_nb_c");
  }

  return *this;
}
void ul_ccch_msg_type_nb_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_nb_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch_msg_type_nb_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_nb_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_type_nb_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_nb_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_ccch_msg_type_nb_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_reest_request_r13:
      c.destroy<rrc_conn_reest_request_nb_s>();
      break;
    case types::rrc_conn_request_r13:
      c.destroy<rrc_conn_request_nb_s>();
      break;
    case types::rrc_conn_resume_request_r13:
      c.destroy<rrc_conn_resume_request_nb_s>();
      break;
    case types::rrc_early_data_request_r15:
      c.destroy<rrc_early_data_request_nb_r15_s>();
      break;
    default:
      break;
  }
}
void ul_ccch_msg_type_nb_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_reest_request_r13:
      c.init<rrc_conn_reest_request_nb_s>();
      break;
    case types::rrc_conn_request_r13:
      c.init<rrc_conn_request_nb_s>();
      break;
    case types::rrc_conn_resume_request_r13:
      c.init<rrc_conn_resume_request_nb_s>();
      break;
    case types::rrc_early_data_request_r15:
      c.init<rrc_early_data_request_nb_r15_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_nb_c::c1_c_");
  }
}
ul_ccch_msg_type_nb_c::c1_c_::c1_c_(const ul_ccch_msg_type_nb_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_reest_request_r13:
      c.init(other.c.get<rrc_conn_reest_request_nb_s>());
      break;
    case types::rrc_conn_request_r13:
      c.init(other.c.get<rrc_conn_request_nb_s>());
      break;
    case types::rrc_conn_resume_request_r13:
      c.init(other.c.get<rrc_conn_resume_request_nb_s>());
      break;
    case types::rrc_early_data_request_r15:
      c.init(other.c.get<rrc_early_data_request_nb_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_nb_c::c1_c_");
  }
}
ul_ccch_msg_type_nb_c::c1_c_& ul_ccch_msg_type_nb_c::c1_c_::operator=(const ul_ccch_msg_type_nb_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_reest_request_r13:
      c.set(other.c.get<rrc_conn_reest_request_nb_s>());
      break;
    case types::rrc_conn_request_r13:
      c.set(other.c.get<rrc_conn_request_nb_s>());
      break;
    case types::rrc_conn_resume_request_r13:
      c.set(other.c.get<rrc_conn_resume_request_nb_s>());
      break;
    case types::rrc_early_data_request_r15:
      c.set(other.c.get<rrc_early_data_request_nb_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_nb_c::c1_c_");
  }

  return *this;
}
void ul_ccch_msg_type_nb_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_reest_request_r13:
      j.write_fieldname("rrcConnectionReestablishmentRequest-r13");
      c.get<rrc_conn_reest_request_nb_s>().to_json(j);
      break;
    case types::rrc_conn_request_r13:
      j.write_fieldname("rrcConnectionRequest-r13");
      c.get<rrc_conn_request_nb_s>().to_json(j);
      break;
    case types::rrc_conn_resume_request_r13:
      j.write_fieldname("rrcConnectionResumeRequest-r13");
      c.get<rrc_conn_resume_request_nb_s>().to_json(j);
      break;
    case types::rrc_early_data_request_r15:
      j.write_fieldname("rrcEarlyDataRequest-r15");
      c.get<rrc_early_data_request_nb_r15_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_nb_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch_msg_type_nb_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_reest_request_r13:
      HANDLE_CODE(c.get<rrc_conn_reest_request_nb_s>().pack(bref));
      break;
    case types::rrc_conn_request_r13:
      HANDLE_CODE(c.get<rrc_conn_request_nb_s>().pack(bref));
      break;
    case types::rrc_conn_resume_request_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_request_nb_s>().pack(bref));
      break;
    case types::rrc_early_data_request_r15:
      HANDLE_CODE(c.get<rrc_early_data_request_nb_r15_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_nb_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_type_nb_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_reest_request_r13:
      HANDLE_CODE(c.get<rrc_conn_reest_request_nb_s>().unpack(bref));
      break;
    case types::rrc_conn_request_r13:
      HANDLE_CODE(c.get<rrc_conn_request_nb_s>().unpack(bref));
      break;
    case types::rrc_conn_resume_request_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_request_nb_s>().unpack(bref));
      break;
    case types::rrc_early_data_request_r15:
      HANDLE_CODE(c.get<rrc_early_data_request_nb_r15_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_nb_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string ul_ccch_msg_type_nb_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishmentRequest-r13",
                                  "rrcConnectionRequest-r13",
                                  "rrcConnectionResumeRequest-r13",
                                  "rrcEarlyDataRequest-r15"};
  return convert_enum_idx(options, 4, value, "ul_ccch_msg_type_nb_c::c1_c_::types");
}

std::string ul_ccch_msg_type_nb_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "ul_ccch_msg_type_nb_c::types");
}
uint8_t ul_ccch_msg_type_nb_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ul_ccch_msg_type_nb_c::types");
}

// UL-CCCH-Message-NB ::= SEQUENCE
SRSASN_CODE ul_ccch_msg_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ul_ccch_msg_nb_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("UL-CCCH-Message-NB");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// ULInformationTransfer-NB-r13-IEs ::= SEQUENCE
SRSASN_CODE ul_info_transfer_nb_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ded_info_nas_r13.pack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_nb_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ded_info_nas_r13.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_info_transfer_nb_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("dedicatedInfoNAS-r13", ded_info_nas_r13.to_string());
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

// ULInformationTransfer-NB ::= SEQUENCE
SRSASN_CODE ul_info_transfer_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_info_transfer_nb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ul_info_transfer_nb_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::ul_info_transfer_r13:
      c.destroy<ul_info_transfer_nb_r13_ies_s>();
      break;
    default:
      break;
  }
}
void ul_info_transfer_nb_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ul_info_transfer_r13:
      c.init<ul_info_transfer_nb_r13_ies_s>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_nb_s::crit_exts_c_");
  }
}
ul_info_transfer_nb_s::crit_exts_c_::crit_exts_c_(const ul_info_transfer_nb_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ul_info_transfer_r13:
      c.init(other.c.get<ul_info_transfer_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_nb_s::crit_exts_c_");
  }
}
ul_info_transfer_nb_s::crit_exts_c_& ul_info_transfer_nb_s::crit_exts_c_::
                                     operator=(const ul_info_transfer_nb_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ul_info_transfer_r13:
      c.set(other.c.get<ul_info_transfer_nb_r13_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_nb_s::crit_exts_c_");
  }

  return *this;
}
void ul_info_transfer_nb_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ul_info_transfer_r13:
      j.write_fieldname("ulInformationTransfer-r13");
      c.get<ul_info_transfer_nb_r13_ies_s>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_nb_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_info_transfer_nb_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ul_info_transfer_r13:
      HANDLE_CODE(c.get<ul_info_transfer_nb_r13_ies_s>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_nb_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_nb_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ul_info_transfer_r13:
      HANDLE_CODE(c.get<ul_info_transfer_nb_r13_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_nb_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string ul_info_transfer_nb_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"ulInformationTransfer-r13", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "ul_info_transfer_nb_s::crit_exts_c_::types");
}

// UL-DCCH-MessageType-NB ::= CHOICE
void ul_dcch_msg_type_nb_c::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    default:
      break;
  }
}
void ul_dcch_msg_type_nb_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::msg_class_ext:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_nb_c");
  }
}
ul_dcch_msg_type_nb_c::ul_dcch_msg_type_nb_c(const ul_dcch_msg_type_nb_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_nb_c");
  }
}
ul_dcch_msg_type_nb_c& ul_dcch_msg_type_nb_c::operator=(const ul_dcch_msg_type_nb_c& other)
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
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_nb_c");
  }

  return *this;
}
void ul_dcch_msg_type_nb_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_nb_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_dcch_msg_type_nb_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_nb_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_type_nb_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_nb_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_dcch_msg_type_nb_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_recfg_complete_r13:
      c.destroy<rrc_conn_recfg_complete_nb_s>();
      break;
    case types::rrc_conn_reest_complete_r13:
      c.destroy<rrc_conn_reest_complete_nb_s>();
      break;
    case types::rrc_conn_setup_complete_r13:
      c.destroy<rrc_conn_setup_complete_nb_s>();
      break;
    case types::security_mode_complete_r13:
      c.destroy<security_mode_complete_s>();
      break;
    case types::security_mode_fail_r13:
      c.destroy<security_mode_fail_s>();
      break;
    case types::ue_cap_info_r13:
      c.destroy<ue_cap_info_nb_s>();
      break;
    case types::ul_info_transfer_r13:
      c.destroy<ul_info_transfer_nb_s>();
      break;
    case types::rrc_conn_resume_complete_r13:
      c.destroy<rrc_conn_resume_complete_nb_s>();
      break;
    default:
      break;
  }
}
void ul_dcch_msg_type_nb_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_recfg_complete_r13:
      c.init<rrc_conn_recfg_complete_nb_s>();
      break;
    case types::rrc_conn_reest_complete_r13:
      c.init<rrc_conn_reest_complete_nb_s>();
      break;
    case types::rrc_conn_setup_complete_r13:
      c.init<rrc_conn_setup_complete_nb_s>();
      break;
    case types::security_mode_complete_r13:
      c.init<security_mode_complete_s>();
      break;
    case types::security_mode_fail_r13:
      c.init<security_mode_fail_s>();
      break;
    case types::ue_cap_info_r13:
      c.init<ue_cap_info_nb_s>();
      break;
    case types::ul_info_transfer_r13:
      c.init<ul_info_transfer_nb_s>();
      break;
    case types::rrc_conn_resume_complete_r13:
      c.init<rrc_conn_resume_complete_nb_s>();
      break;
    case types::spare8:
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
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_nb_c::c1_c_");
  }
}
ul_dcch_msg_type_nb_c::c1_c_::c1_c_(const ul_dcch_msg_type_nb_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_recfg_complete_r13:
      c.init(other.c.get<rrc_conn_recfg_complete_nb_s>());
      break;
    case types::rrc_conn_reest_complete_r13:
      c.init(other.c.get<rrc_conn_reest_complete_nb_s>());
      break;
    case types::rrc_conn_setup_complete_r13:
      c.init(other.c.get<rrc_conn_setup_complete_nb_s>());
      break;
    case types::security_mode_complete_r13:
      c.init(other.c.get<security_mode_complete_s>());
      break;
    case types::security_mode_fail_r13:
      c.init(other.c.get<security_mode_fail_s>());
      break;
    case types::ue_cap_info_r13:
      c.init(other.c.get<ue_cap_info_nb_s>());
      break;
    case types::ul_info_transfer_r13:
      c.init(other.c.get<ul_info_transfer_nb_s>());
      break;
    case types::rrc_conn_resume_complete_r13:
      c.init(other.c.get<rrc_conn_resume_complete_nb_s>());
      break;
    case types::spare8:
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
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_nb_c::c1_c_");
  }
}
ul_dcch_msg_type_nb_c::c1_c_& ul_dcch_msg_type_nb_c::c1_c_::operator=(const ul_dcch_msg_type_nb_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_recfg_complete_r13:
      c.set(other.c.get<rrc_conn_recfg_complete_nb_s>());
      break;
    case types::rrc_conn_reest_complete_r13:
      c.set(other.c.get<rrc_conn_reest_complete_nb_s>());
      break;
    case types::rrc_conn_setup_complete_r13:
      c.set(other.c.get<rrc_conn_setup_complete_nb_s>());
      break;
    case types::security_mode_complete_r13:
      c.set(other.c.get<security_mode_complete_s>());
      break;
    case types::security_mode_fail_r13:
      c.set(other.c.get<security_mode_fail_s>());
      break;
    case types::ue_cap_info_r13:
      c.set(other.c.get<ue_cap_info_nb_s>());
      break;
    case types::ul_info_transfer_r13:
      c.set(other.c.get<ul_info_transfer_nb_s>());
      break;
    case types::rrc_conn_resume_complete_r13:
      c.set(other.c.get<rrc_conn_resume_complete_nb_s>());
      break;
    case types::spare8:
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
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_nb_c::c1_c_");
  }

  return *this;
}
void ul_dcch_msg_type_nb_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_recfg_complete_r13:
      j.write_fieldname("rrcConnectionReconfigurationComplete-r13");
      c.get<rrc_conn_recfg_complete_nb_s>().to_json(j);
      break;
    case types::rrc_conn_reest_complete_r13:
      j.write_fieldname("rrcConnectionReestablishmentComplete-r13");
      c.get<rrc_conn_reest_complete_nb_s>().to_json(j);
      break;
    case types::rrc_conn_setup_complete_r13:
      j.write_fieldname("rrcConnectionSetupComplete-r13");
      c.get<rrc_conn_setup_complete_nb_s>().to_json(j);
      break;
    case types::security_mode_complete_r13:
      j.write_fieldname("securityModeComplete-r13");
      c.get<security_mode_complete_s>().to_json(j);
      break;
    case types::security_mode_fail_r13:
      j.write_fieldname("securityModeFailure-r13");
      c.get<security_mode_fail_s>().to_json(j);
      break;
    case types::ue_cap_info_r13:
      j.write_fieldname("ueCapabilityInformation-r13");
      c.get<ue_cap_info_nb_s>().to_json(j);
      break;
    case types::ul_info_transfer_r13:
      j.write_fieldname("ulInformationTransfer-r13");
      c.get<ul_info_transfer_nb_s>().to_json(j);
      break;
    case types::rrc_conn_resume_complete_r13:
      j.write_fieldname("rrcConnectionResumeComplete-r13");
      c.get<rrc_conn_resume_complete_nb_s>().to_json(j);
      break;
    case types::spare8:
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
      log_invalid_choice_id(type_, "ul_dcch_msg_type_nb_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_dcch_msg_type_nb_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_recfg_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_recfg_complete_nb_s>().pack(bref));
      break;
    case types::rrc_conn_reest_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_reest_complete_nb_s>().pack(bref));
      break;
    case types::rrc_conn_setup_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_setup_complete_nb_s>().pack(bref));
      break;
    case types::security_mode_complete_r13:
      HANDLE_CODE(c.get<security_mode_complete_s>().pack(bref));
      break;
    case types::security_mode_fail_r13:
      HANDLE_CODE(c.get<security_mode_fail_s>().pack(bref));
      break;
    case types::ue_cap_info_r13:
      HANDLE_CODE(c.get<ue_cap_info_nb_s>().pack(bref));
      break;
    case types::ul_info_transfer_r13:
      HANDLE_CODE(c.get<ul_info_transfer_nb_s>().pack(bref));
      break;
    case types::rrc_conn_resume_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_complete_nb_s>().pack(bref));
      break;
    case types::spare8:
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
      log_invalid_choice_id(type_, "ul_dcch_msg_type_nb_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_type_nb_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_recfg_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_recfg_complete_nb_s>().unpack(bref));
      break;
    case types::rrc_conn_reest_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_reest_complete_nb_s>().unpack(bref));
      break;
    case types::rrc_conn_setup_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_setup_complete_nb_s>().unpack(bref));
      break;
    case types::security_mode_complete_r13:
      HANDLE_CODE(c.get<security_mode_complete_s>().unpack(bref));
      break;
    case types::security_mode_fail_r13:
      HANDLE_CODE(c.get<security_mode_fail_s>().unpack(bref));
      break;
    case types::ue_cap_info_r13:
      HANDLE_CODE(c.get<ue_cap_info_nb_s>().unpack(bref));
      break;
    case types::ul_info_transfer_r13:
      HANDLE_CODE(c.get<ul_info_transfer_nb_s>().unpack(bref));
      break;
    case types::rrc_conn_resume_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_complete_nb_s>().unpack(bref));
      break;
    case types::spare8:
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
      log_invalid_choice_id(type_, "ul_dcch_msg_type_nb_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

std::string ul_dcch_msg_type_nb_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReconfigurationComplete-r13",
                                  "rrcConnectionReestablishmentComplete-r13",
                                  "rrcConnectionSetupComplete-r13",
                                  "securityModeComplete-r13",
                                  "securityModeFailure-r13",
                                  "ueCapabilityInformation-r13",
                                  "ulInformationTransfer-r13",
                                  "rrcConnectionResumeComplete-r13",
                                  "spare8",
                                  "spare7",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "ul_dcch_msg_type_nb_c::c1_c_::types");
}

std::string ul_dcch_msg_type_nb_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "ul_dcch_msg_type_nb_c::types");
}
uint8_t ul_dcch_msg_type_nb_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ul_dcch_msg_type_nb_c::types");
}

// UL-DCCH-Message-NB ::= SEQUENCE
SRSASN_CODE ul_dcch_msg_nb_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_nb_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ul_dcch_msg_nb_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("UL-DCCH-Message-NB");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}
