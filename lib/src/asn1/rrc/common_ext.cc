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

#include "srsran/asn1/rrc/common_ext.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// MCS-PSSCH-Range-r15 ::= SEQUENCE
SRSASN_CODE mcs_pssch_range_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, min_mcs_pssch_r15, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, max_mcs_pssch_r15, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mcs_pssch_range_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(min_mcs_pssch_r15, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(max_mcs_pssch_r15, bref, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
void mcs_pssch_range_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("minMCS-PSSCH-r15", min_mcs_pssch_r15);
  j.write_int("maxMCS-PSSCH-r15", max_mcs_pssch_r15);
  j.end_obj();
}

// SL-TxPower-r14 ::= CHOICE
void sl_tx_pwr_r14_c::set(types::options e)
{
  type_ = e;
}
void sl_tx_pwr_r14_c::set_minusinfinity_r14()
{
  set(types::minusinfinity_r14);
}
int8_t& sl_tx_pwr_r14_c::set_tx_pwr_r14()
{
  set(types::tx_pwr_r14);
  return c;
}
void sl_tx_pwr_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::minusinfinity_r14:
      break;
    case types::tx_pwr_r14:
      j.write_int("txPower-r14", c);
      break;
    default:
      log_invalid_choice_id(type_, "sl_tx_pwr_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE sl_tx_pwr_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::minusinfinity_r14:
      break;
    case types::tx_pwr_r14:
      HANDLE_CODE(pack_integer(bref, c, (int8_t)-41, (int8_t)31));
      break;
    default:
      log_invalid_choice_id(type_, "sl_tx_pwr_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tx_pwr_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::minusinfinity_r14:
      break;
    case types::tx_pwr_r14:
      HANDLE_CODE(unpack_integer(c, bref, (int8_t)-41, (int8_t)31));
      break;
    default:
      log_invalid_choice_id(type_, "sl_tx_pwr_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-MinT2Value-r15 ::= SEQUENCE
SRSASN_CODE sl_min_t2_value_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, prio_list_r15, 1, 8, integer_packer<uint8_t>(1, 8)));
  HANDLE_CODE(pack_integer(bref, min_t2_value_r15, (uint8_t)10u, (uint8_t)20u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_min_t2_value_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(prio_list_r15, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  HANDLE_CODE(unpack_integer(min_t2_value_r15, bref, (uint8_t)10u, (uint8_t)20u));

  return SRSASN_SUCCESS;
}
void sl_min_t2_value_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("priorityList-r15");
  for (const auto& e1 : prio_list_r15) {
    j.write_int(e1);
  }
  j.end_array();
  j.write_int("minT2Value-r15", min_t2_value_r15);
  j.end_obj();
}

// SL-PPPP-TxConfigIndex-r14 ::= SEQUENCE
SRSASN_CODE sl_pppp_tx_cfg_idx_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, prio_thres_r14, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pack_integer(bref, default_tx_cfg_idx_r14, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, cbr_cfg_idx_r14, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(pack_dyn_seq_of(bref, tx_cfg_idx_list_r14, 1, 16, integer_packer<uint8_t>(0, 63)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pppp_tx_cfg_idx_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(prio_thres_r14, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(unpack_integer(default_tx_cfg_idx_r14, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(cbr_cfg_idx_r14, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(unpack_dyn_seq_of(tx_cfg_idx_list_r14, bref, 1, 16, integer_packer<uint8_t>(0, 63)));

  return SRSASN_SUCCESS;
}
void sl_pppp_tx_cfg_idx_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("priorityThreshold-r14", prio_thres_r14);
  j.write_int("defaultTxConfigIndex-r14", default_tx_cfg_idx_r14);
  j.write_int("cbr-ConfigIndex-r14", cbr_cfg_idx_r14);
  j.start_array("tx-ConfigIndexList-r14");
  for (const auto& e1 : tx_cfg_idx_list_r14) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// SL-PPPP-TxConfigIndex-v1530 ::= SEQUENCE
SRSASN_CODE sl_pppp_tx_cfg_idx_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mcs_pssch_range_list_r15_present, 1));

  if (mcs_pssch_range_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mcs_pssch_range_list_r15, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pppp_tx_cfg_idx_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mcs_pssch_range_list_r15_present, 1));

  if (mcs_pssch_range_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mcs_pssch_range_list_r15, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void sl_pppp_tx_cfg_idx_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mcs_pssch_range_list_r15_present) {
    j.start_array("mcs-PSSCH-RangeList-r15");
    for (const auto& e1 : mcs_pssch_range_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-PSSCH-TxParameters-r14 ::= SEQUENCE
SRSASN_CODE sl_pssch_tx_params_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(max_tx_pwr_r14_present, 1));

  HANDLE_CODE(pack_integer(bref, min_mcs_pssch_r14, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, max_mcs_pssch_r14, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, min_sub_ch_num_pssch_r14, (uint8_t)1u, (uint8_t)20u));
  HANDLE_CODE(pack_integer(bref, max_subch_num_pssch_r14, (uint8_t)1u, (uint8_t)20u));
  HANDLE_CODE(allowed_retx_num_pssch_r14.pack(bref));
  if (max_tx_pwr_r14_present) {
    HANDLE_CODE(max_tx_pwr_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pssch_tx_params_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(max_tx_pwr_r14_present, 1));

  HANDLE_CODE(unpack_integer(min_mcs_pssch_r14, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(max_mcs_pssch_r14, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(min_sub_ch_num_pssch_r14, bref, (uint8_t)1u, (uint8_t)20u));
  HANDLE_CODE(unpack_integer(max_subch_num_pssch_r14, bref, (uint8_t)1u, (uint8_t)20u));
  HANDLE_CODE(allowed_retx_num_pssch_r14.unpack(bref));
  if (max_tx_pwr_r14_present) {
    HANDLE_CODE(max_tx_pwr_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_pssch_tx_params_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("minMCS-PSSCH-r14", min_mcs_pssch_r14);
  j.write_int("maxMCS-PSSCH-r14", max_mcs_pssch_r14);
  j.write_int("minSubChannel-NumberPSSCH-r14", min_sub_ch_num_pssch_r14);
  j.write_int("maxSubchannel-NumberPSSCH-r14", max_subch_num_pssch_r14);
  j.write_str("allowedRetxNumberPSSCH-r14", allowed_retx_num_pssch_r14.to_string());
  if (max_tx_pwr_r14_present) {
    j.write_fieldname("maxTxPower-r14");
    max_tx_pwr_r14.to_json(j);
  }
  j.end_obj();
}

const char* sl_pssch_tx_params_r14_s::allowed_retx_num_pssch_r14_opts::to_string() const
{
  static const char* options[] = {"n0", "n1", "both", "spare1"};
  return convert_enum_idx(options, 4, value, "sl_pssch_tx_params_r14_s::allowed_retx_num_pssch_r14_e_");
}
uint8_t sl_pssch_tx_params_r14_s::allowed_retx_num_pssch_r14_opts::to_number() const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(options, 2, value, "sl_pssch_tx_params_r14_s::allowed_retx_num_pssch_r14_e_");
}

// SL-PSSCH-TxParameters-v1530 ::= SEQUENCE
SRSASN_CODE sl_pssch_tx_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, min_mcs_pssch_r15, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, max_mcs_pssch_r15, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pssch_tx_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(min_mcs_pssch_r15, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(max_mcs_pssch_r15, bref, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
void sl_pssch_tx_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("minMCS-PSSCH-r15", min_mcs_pssch_r15);
  j.write_int("maxMCS-PSSCH-r15", max_mcs_pssch_r15);
  j.end_obj();
}

// SL-RestrictResourceReservationPeriod-r14 ::= ENUMERATED
const char* sl_restrict_res_reserv_period_r14_opts::to_string() const
{
  static const char* options[] = {"v0dot2",
                                  "v0dot5",
                                  "v1",
                                  "v2",
                                  "v3",
                                  "v4",
                                  "v5",
                                  "v6",
                                  "v7",
                                  "v8",
                                  "v9",
                                  "v10",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sl_restrict_res_reserv_period_r14_e");
}
float sl_restrict_res_reserv_period_r14_opts::to_number() const
{
  static const float options[] = {0.2, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  return map_enum_number(options, 12, value, "sl_restrict_res_reserv_period_r14_e");
}
const char* sl_restrict_res_reserv_period_r14_opts::to_number_string() const
{
  static const char* options[] = {"0.2", "0.5", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
  return convert_enum_idx(options, 16, value, "sl_restrict_res_reserv_period_r14_e");
}

// SL-TypeTxSync-r14 ::= ENUMERATED
const char* sl_type_tx_sync_r14_opts::to_string() const
{
  static const char* options[] = {"gnss", "enb", "ue"};
  return convert_enum_idx(options, 3, value, "sl_type_tx_sync_r14_e");
}

// SL-CP-Len-r12 ::= ENUMERATED
const char* sl_cp_len_r12_opts::to_string() const
{
  static const char* options[] = {"normal", "extended"};
  return convert_enum_idx(options, 2, value, "sl_cp_len_r12_e");
}

// SL-OffsetIndicator-r12 ::= CHOICE
void sl_offset_ind_r12_c::destroy_() {}
void sl_offset_ind_r12_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
sl_offset_ind_r12_c::sl_offset_ind_r12_c(const sl_offset_ind_r12_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::small_r12:
      c.init(other.c.get<uint16_t>());
      break;
    case types::large_r12:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_offset_ind_r12_c");
  }
}
sl_offset_ind_r12_c& sl_offset_ind_r12_c::operator=(const sl_offset_ind_r12_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::small_r12:
      c.set(other.c.get<uint16_t>());
      break;
    case types::large_r12:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_offset_ind_r12_c");
  }

  return *this;
}
uint16_t& sl_offset_ind_r12_c::set_small_r12()
{
  set(types::small_r12);
  return c.get<uint16_t>();
}
uint16_t& sl_offset_ind_r12_c::set_large_r12()
{
  set(types::large_r12);
  return c.get<uint16_t>();
}
void sl_offset_ind_r12_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::small_r12:
      j.write_int("small-r12", c.get<uint16_t>());
      break;
    case types::large_r12:
      j.write_int("large-r12", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sl_offset_ind_r12_c");
  }
  j.end_obj();
}
SRSASN_CODE sl_offset_ind_r12_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::small_r12:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::large_r12:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)10239u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_offset_ind_r12_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_offset_ind_r12_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::small_r12:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::large_r12:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)10239u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_offset_ind_r12_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-P2X-ResourceSelectionConfig-r14 ::= SEQUENCE
SRSASN_CODE sl_p2_x_res_sel_cfg_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(partial_sensing_r14_present, 1));
  HANDLE_CODE(bref.pack(random_sel_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_p2_x_res_sel_cfg_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(partial_sensing_r14_present, 1));
  HANDLE_CODE(bref.unpack(random_sel_r14_present, 1));

  return SRSASN_SUCCESS;
}
void sl_p2_x_res_sel_cfg_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (partial_sensing_r14_present) {
    j.write_str("partialSensing-r14", "true");
  }
  if (random_sel_r14_present) {
    j.write_str("randomSelection-r14", "true");
  }
  j.end_obj();
}

// SL-PSSCH-TxConfig-r14 ::= SEQUENCE
SRSASN_CODE sl_pssch_tx_cfg_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(type_tx_sync_r14_present, 1));

  if (type_tx_sync_r14_present) {
    HANDLE_CODE(type_tx_sync_r14.pack(bref));
  }
  HANDLE_CODE(thres_ue_speed_r14.pack(bref));
  HANDLE_CODE(params_above_thres_r14.pack(bref));
  HANDLE_CODE(params_below_thres_r14.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= params_above_thres_v1530.is_present();
    group_flags[0] |= params_below_thres_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(params_above_thres_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(params_below_thres_v1530.is_present(), 1));
      if (params_above_thres_v1530.is_present()) {
        HANDLE_CODE(params_above_thres_v1530->pack(bref));
      }
      if (params_below_thres_v1530.is_present()) {
        HANDLE_CODE(params_below_thres_v1530->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pssch_tx_cfg_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(type_tx_sync_r14_present, 1));

  if (type_tx_sync_r14_present) {
    HANDLE_CODE(type_tx_sync_r14.unpack(bref));
  }
  HANDLE_CODE(thres_ue_speed_r14.unpack(bref));
  HANDLE_CODE(params_above_thres_r14.unpack(bref));
  HANDLE_CODE(params_below_thres_r14.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool params_above_thres_v1530_present;
      HANDLE_CODE(bref.unpack(params_above_thres_v1530_present, 1));
      params_above_thres_v1530.set_present(params_above_thres_v1530_present);
      bool params_below_thres_v1530_present;
      HANDLE_CODE(bref.unpack(params_below_thres_v1530_present, 1));
      params_below_thres_v1530.set_present(params_below_thres_v1530_present);
      if (params_above_thres_v1530.is_present()) {
        HANDLE_CODE(params_above_thres_v1530->unpack(bref));
      }
      if (params_below_thres_v1530.is_present()) {
        HANDLE_CODE(params_below_thres_v1530->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_pssch_tx_cfg_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (type_tx_sync_r14_present) {
    j.write_str("typeTxSync-r14", type_tx_sync_r14.to_string());
  }
  j.write_str("thresUE-Speed-r14", thres_ue_speed_r14.to_string());
  j.write_fieldname("parametersAboveThres-r14");
  params_above_thres_r14.to_json(j);
  j.write_fieldname("parametersBelowThres-r14");
  params_below_thres_r14.to_json(j);
  if (ext) {
    if (params_above_thres_v1530.is_present()) {
      j.write_fieldname("parametersAboveThres-v1530");
      params_above_thres_v1530->to_json(j);
    }
    if (params_below_thres_v1530.is_present()) {
      j.write_fieldname("parametersBelowThres-v1530");
      params_below_thres_v1530->to_json(j);
    }
  }
  j.end_obj();
}

const char* sl_pssch_tx_cfg_r14_s::thres_ue_speed_r14_opts::to_string() const
{
  static const char* options[] = {"kmph60", "kmph80", "kmph100", "kmph120", "kmph140", "kmph160", "kmph180", "kmph200"};
  return convert_enum_idx(options, 8, value, "sl_pssch_tx_cfg_r14_s::thres_ue_speed_r14_e_");
}
uint8_t sl_pssch_tx_cfg_r14_s::thres_ue_speed_r14_opts::to_number() const
{
  static const uint8_t options[] = {60, 80, 100, 120, 140, 160, 180, 200};
  return map_enum_number(options, 8, value, "sl_pssch_tx_cfg_r14_s::thres_ue_speed_r14_e_");
}

// SL-SyncAllowed-r14 ::= SEQUENCE
SRSASN_CODE sl_sync_allowed_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gnss_sync_r14_present, 1));
  HANDLE_CODE(bref.pack(enb_sync_r14_present, 1));
  HANDLE_CODE(bref.pack(ue_sync_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_sync_allowed_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gnss_sync_r14_present, 1));
  HANDLE_CODE(bref.unpack(enb_sync_r14_present, 1));
  HANDLE_CODE(bref.unpack(ue_sync_r14_present, 1));

  return SRSASN_SUCCESS;
}
void sl_sync_allowed_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gnss_sync_r14_present) {
    j.write_str("gnss-Sync-r14", "true");
  }
  if (enb_sync_r14_present) {
    j.write_str("enb-Sync-r14", "true");
  }
  if (ue_sync_r14_present) {
    j.write_str("ue-Sync-r14", "true");
  }
  j.end_obj();
}

// SL-TxParameters-r12 ::= SEQUENCE
SRSASN_CODE sl_tx_params_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(alpha_r12.pack(bref));
  HANDLE_CODE(pack_integer(bref, p0_r12, (int8_t)-126, (int8_t)31));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tx_params_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(alpha_r12.unpack(bref));
  HANDLE_CODE(unpack_integer(p0_r12, bref, (int8_t)-126, (int8_t)31));

  return SRSASN_SUCCESS;
}
void sl_tx_params_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("alpha-r12", alpha_r12.to_string());
  j.write_int("p0-r12", p0_r12);
  j.end_obj();
}

// SubframeBitmapSL-r12 ::= CHOICE
void sf_bitmap_sl_r12_c::destroy_()
{
  switch (type_) {
    case types::bs4_r12:
      c.destroy<fixed_bitstring<4> >();
      break;
    case types::bs8_r12:
      c.destroy<fixed_bitstring<8> >();
      break;
    case types::bs12_r12:
      c.destroy<fixed_bitstring<12> >();
      break;
    case types::bs16_r12:
      c.destroy<fixed_bitstring<16> >();
      break;
    case types::bs30_r12:
      c.destroy<fixed_bitstring<30> >();
      break;
    case types::bs40_r12:
      c.destroy<fixed_bitstring<40> >();
      break;
    case types::bs42_r12:
      c.destroy<fixed_bitstring<42> >();
      break;
    default:
      break;
  }
}
void sf_bitmap_sl_r12_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::bs4_r12:
      c.init<fixed_bitstring<4> >();
      break;
    case types::bs8_r12:
      c.init<fixed_bitstring<8> >();
      break;
    case types::bs12_r12:
      c.init<fixed_bitstring<12> >();
      break;
    case types::bs16_r12:
      c.init<fixed_bitstring<16> >();
      break;
    case types::bs30_r12:
      c.init<fixed_bitstring<30> >();
      break;
    case types::bs40_r12:
      c.init<fixed_bitstring<40> >();
      break;
    case types::bs42_r12:
      c.init<fixed_bitstring<42> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sf_bitmap_sl_r12_c");
  }
}
sf_bitmap_sl_r12_c::sf_bitmap_sl_r12_c(const sf_bitmap_sl_r12_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::bs4_r12:
      c.init(other.c.get<fixed_bitstring<4> >());
      break;
    case types::bs8_r12:
      c.init(other.c.get<fixed_bitstring<8> >());
      break;
    case types::bs12_r12:
      c.init(other.c.get<fixed_bitstring<12> >());
      break;
    case types::bs16_r12:
      c.init(other.c.get<fixed_bitstring<16> >());
      break;
    case types::bs30_r12:
      c.init(other.c.get<fixed_bitstring<30> >());
      break;
    case types::bs40_r12:
      c.init(other.c.get<fixed_bitstring<40> >());
      break;
    case types::bs42_r12:
      c.init(other.c.get<fixed_bitstring<42> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sf_bitmap_sl_r12_c");
  }
}
sf_bitmap_sl_r12_c& sf_bitmap_sl_r12_c::operator=(const sf_bitmap_sl_r12_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::bs4_r12:
      c.set(other.c.get<fixed_bitstring<4> >());
      break;
    case types::bs8_r12:
      c.set(other.c.get<fixed_bitstring<8> >());
      break;
    case types::bs12_r12:
      c.set(other.c.get<fixed_bitstring<12> >());
      break;
    case types::bs16_r12:
      c.set(other.c.get<fixed_bitstring<16> >());
      break;
    case types::bs30_r12:
      c.set(other.c.get<fixed_bitstring<30> >());
      break;
    case types::bs40_r12:
      c.set(other.c.get<fixed_bitstring<40> >());
      break;
    case types::bs42_r12:
      c.set(other.c.get<fixed_bitstring<42> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sf_bitmap_sl_r12_c");
  }

  return *this;
}
fixed_bitstring<4>& sf_bitmap_sl_r12_c::set_bs4_r12()
{
  set(types::bs4_r12);
  return c.get<fixed_bitstring<4> >();
}
fixed_bitstring<8>& sf_bitmap_sl_r12_c::set_bs8_r12()
{
  set(types::bs8_r12);
  return c.get<fixed_bitstring<8> >();
}
fixed_bitstring<12>& sf_bitmap_sl_r12_c::set_bs12_r12()
{
  set(types::bs12_r12);
  return c.get<fixed_bitstring<12> >();
}
fixed_bitstring<16>& sf_bitmap_sl_r12_c::set_bs16_r12()
{
  set(types::bs16_r12);
  return c.get<fixed_bitstring<16> >();
}
fixed_bitstring<30>& sf_bitmap_sl_r12_c::set_bs30_r12()
{
  set(types::bs30_r12);
  return c.get<fixed_bitstring<30> >();
}
fixed_bitstring<40>& sf_bitmap_sl_r12_c::set_bs40_r12()
{
  set(types::bs40_r12);
  return c.get<fixed_bitstring<40> >();
}
fixed_bitstring<42>& sf_bitmap_sl_r12_c::set_bs42_r12()
{
  set(types::bs42_r12);
  return c.get<fixed_bitstring<42> >();
}
void sf_bitmap_sl_r12_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::bs4_r12:
      j.write_str("bs4-r12", c.get<fixed_bitstring<4> >().to_string());
      break;
    case types::bs8_r12:
      j.write_str("bs8-r12", c.get<fixed_bitstring<8> >().to_string());
      break;
    case types::bs12_r12:
      j.write_str("bs12-r12", c.get<fixed_bitstring<12> >().to_string());
      break;
    case types::bs16_r12:
      j.write_str("bs16-r12", c.get<fixed_bitstring<16> >().to_string());
      break;
    case types::bs30_r12:
      j.write_str("bs30-r12", c.get<fixed_bitstring<30> >().to_string());
      break;
    case types::bs40_r12:
      j.write_str("bs40-r12", c.get<fixed_bitstring<40> >().to_string());
      break;
    case types::bs42_r12:
      j.write_str("bs42-r12", c.get<fixed_bitstring<42> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "sf_bitmap_sl_r12_c");
  }
  j.end_obj();
}
SRSASN_CODE sf_bitmap_sl_r12_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::bs4_r12:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().pack(bref));
      break;
    case types::bs8_r12:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().pack(bref));
      break;
    case types::bs12_r12:
      HANDLE_CODE(c.get<fixed_bitstring<12> >().pack(bref));
      break;
    case types::bs16_r12:
      HANDLE_CODE(c.get<fixed_bitstring<16> >().pack(bref));
      break;
    case types::bs30_r12:
      HANDLE_CODE(c.get<fixed_bitstring<30> >().pack(bref));
      break;
    case types::bs40_r12:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().pack(bref));
      break;
    case types::bs42_r12:
      HANDLE_CODE(c.get<fixed_bitstring<42> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sf_bitmap_sl_r12_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sf_bitmap_sl_r12_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::bs4_r12:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().unpack(bref));
      break;
    case types::bs8_r12:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().unpack(bref));
      break;
    case types::bs12_r12:
      HANDLE_CODE(c.get<fixed_bitstring<12> >().unpack(bref));
      break;
    case types::bs16_r12:
      HANDLE_CODE(c.get<fixed_bitstring<16> >().unpack(bref));
      break;
    case types::bs30_r12:
      HANDLE_CODE(c.get<fixed_bitstring<30> >().unpack(bref));
      break;
    case types::bs40_r12:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().unpack(bref));
      break;
    case types::bs42_r12:
      HANDLE_CODE(c.get<fixed_bitstring<42> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sf_bitmap_sl_r12_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SubframeBitmapSL-r14 ::= CHOICE
void sf_bitmap_sl_r14_c::destroy_()
{
  switch (type_) {
    case types::bs10_r14:
      c.destroy<fixed_bitstring<10> >();
      break;
    case types::bs16_r14:
      c.destroy<fixed_bitstring<16> >();
      break;
    case types::bs20_r14:
      c.destroy<fixed_bitstring<20> >();
      break;
    case types::bs30_r14:
      c.destroy<fixed_bitstring<30> >();
      break;
    case types::bs40_r14:
      c.destroy<fixed_bitstring<40> >();
      break;
    case types::bs50_r14:
      c.destroy<fixed_bitstring<50> >();
      break;
    case types::bs60_r14:
      c.destroy<fixed_bitstring<60> >();
      break;
    case types::bs100_r14:
      c.destroy<fixed_bitstring<100> >();
      break;
    default:
      break;
  }
}
void sf_bitmap_sl_r14_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::bs10_r14:
      c.init<fixed_bitstring<10> >();
      break;
    case types::bs16_r14:
      c.init<fixed_bitstring<16> >();
      break;
    case types::bs20_r14:
      c.init<fixed_bitstring<20> >();
      break;
    case types::bs30_r14:
      c.init<fixed_bitstring<30> >();
      break;
    case types::bs40_r14:
      c.init<fixed_bitstring<40> >();
      break;
    case types::bs50_r14:
      c.init<fixed_bitstring<50> >();
      break;
    case types::bs60_r14:
      c.init<fixed_bitstring<60> >();
      break;
    case types::bs100_r14:
      c.init<fixed_bitstring<100> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sf_bitmap_sl_r14_c");
  }
}
sf_bitmap_sl_r14_c::sf_bitmap_sl_r14_c(const sf_bitmap_sl_r14_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::bs10_r14:
      c.init(other.c.get<fixed_bitstring<10> >());
      break;
    case types::bs16_r14:
      c.init(other.c.get<fixed_bitstring<16> >());
      break;
    case types::bs20_r14:
      c.init(other.c.get<fixed_bitstring<20> >());
      break;
    case types::bs30_r14:
      c.init(other.c.get<fixed_bitstring<30> >());
      break;
    case types::bs40_r14:
      c.init(other.c.get<fixed_bitstring<40> >());
      break;
    case types::bs50_r14:
      c.init(other.c.get<fixed_bitstring<50> >());
      break;
    case types::bs60_r14:
      c.init(other.c.get<fixed_bitstring<60> >());
      break;
    case types::bs100_r14:
      c.init(other.c.get<fixed_bitstring<100> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sf_bitmap_sl_r14_c");
  }
}
sf_bitmap_sl_r14_c& sf_bitmap_sl_r14_c::operator=(const sf_bitmap_sl_r14_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::bs10_r14:
      c.set(other.c.get<fixed_bitstring<10> >());
      break;
    case types::bs16_r14:
      c.set(other.c.get<fixed_bitstring<16> >());
      break;
    case types::bs20_r14:
      c.set(other.c.get<fixed_bitstring<20> >());
      break;
    case types::bs30_r14:
      c.set(other.c.get<fixed_bitstring<30> >());
      break;
    case types::bs40_r14:
      c.set(other.c.get<fixed_bitstring<40> >());
      break;
    case types::bs50_r14:
      c.set(other.c.get<fixed_bitstring<50> >());
      break;
    case types::bs60_r14:
      c.set(other.c.get<fixed_bitstring<60> >());
      break;
    case types::bs100_r14:
      c.set(other.c.get<fixed_bitstring<100> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sf_bitmap_sl_r14_c");
  }

  return *this;
}
fixed_bitstring<10>& sf_bitmap_sl_r14_c::set_bs10_r14()
{
  set(types::bs10_r14);
  return c.get<fixed_bitstring<10> >();
}
fixed_bitstring<16>& sf_bitmap_sl_r14_c::set_bs16_r14()
{
  set(types::bs16_r14);
  return c.get<fixed_bitstring<16> >();
}
fixed_bitstring<20>& sf_bitmap_sl_r14_c::set_bs20_r14()
{
  set(types::bs20_r14);
  return c.get<fixed_bitstring<20> >();
}
fixed_bitstring<30>& sf_bitmap_sl_r14_c::set_bs30_r14()
{
  set(types::bs30_r14);
  return c.get<fixed_bitstring<30> >();
}
fixed_bitstring<40>& sf_bitmap_sl_r14_c::set_bs40_r14()
{
  set(types::bs40_r14);
  return c.get<fixed_bitstring<40> >();
}
fixed_bitstring<50>& sf_bitmap_sl_r14_c::set_bs50_r14()
{
  set(types::bs50_r14);
  return c.get<fixed_bitstring<50> >();
}
fixed_bitstring<60>& sf_bitmap_sl_r14_c::set_bs60_r14()
{
  set(types::bs60_r14);
  return c.get<fixed_bitstring<60> >();
}
fixed_bitstring<100>& sf_bitmap_sl_r14_c::set_bs100_r14()
{
  set(types::bs100_r14);
  return c.get<fixed_bitstring<100> >();
}
void sf_bitmap_sl_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::bs10_r14:
      j.write_str("bs10-r14", c.get<fixed_bitstring<10> >().to_string());
      break;
    case types::bs16_r14:
      j.write_str("bs16-r14", c.get<fixed_bitstring<16> >().to_string());
      break;
    case types::bs20_r14:
      j.write_str("bs20-r14", c.get<fixed_bitstring<20> >().to_string());
      break;
    case types::bs30_r14:
      j.write_str("bs30-r14", c.get<fixed_bitstring<30> >().to_string());
      break;
    case types::bs40_r14:
      j.write_str("bs40-r14", c.get<fixed_bitstring<40> >().to_string());
      break;
    case types::bs50_r14:
      j.write_str("bs50-r14", c.get<fixed_bitstring<50> >().to_string());
      break;
    case types::bs60_r14:
      j.write_str("bs60-r14", c.get<fixed_bitstring<60> >().to_string());
      break;
    case types::bs100_r14:
      j.write_str("bs100-r14", c.get<fixed_bitstring<100> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "sf_bitmap_sl_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE sf_bitmap_sl_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::bs10_r14:
      HANDLE_CODE(c.get<fixed_bitstring<10> >().pack(bref));
      break;
    case types::bs16_r14:
      HANDLE_CODE(c.get<fixed_bitstring<16> >().pack(bref));
      break;
    case types::bs20_r14:
      HANDLE_CODE(c.get<fixed_bitstring<20> >().pack(bref));
      break;
    case types::bs30_r14:
      HANDLE_CODE(c.get<fixed_bitstring<30> >().pack(bref));
      break;
    case types::bs40_r14:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().pack(bref));
      break;
    case types::bs50_r14:
      HANDLE_CODE(c.get<fixed_bitstring<50> >().pack(bref));
      break;
    case types::bs60_r14:
      HANDLE_CODE(c.get<fixed_bitstring<60> >().pack(bref));
      break;
    case types::bs100_r14:
      HANDLE_CODE(c.get<fixed_bitstring<100> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sf_bitmap_sl_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sf_bitmap_sl_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::bs10_r14:
      HANDLE_CODE(c.get<fixed_bitstring<10> >().unpack(bref));
      break;
    case types::bs16_r14:
      HANDLE_CODE(c.get<fixed_bitstring<16> >().unpack(bref));
      break;
    case types::bs20_r14:
      HANDLE_CODE(c.get<fixed_bitstring<20> >().unpack(bref));
      break;
    case types::bs30_r14:
      HANDLE_CODE(c.get<fixed_bitstring<30> >().unpack(bref));
      break;
    case types::bs40_r14:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().unpack(bref));
      break;
    case types::bs50_r14:
      HANDLE_CODE(c.get<fixed_bitstring<50> >().unpack(bref));
      break;
    case types::bs60_r14:
      HANDLE_CODE(c.get<fixed_bitstring<60> >().unpack(bref));
      break;
    case types::bs100_r14:
      HANDLE_CODE(c.get<fixed_bitstring<100> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sf_bitmap_sl_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-CommResourcePoolV2X-r14 ::= SEQUENCE
SRSASN_CODE sl_comm_res_pool_v2x_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_offset_ind_r14_present, 1));
  HANDLE_CODE(bref.pack(start_rb_pscch_pool_r14_present, 1));
  HANDLE_CODE(bref.pack(rx_params_ncell_r14_present, 1));
  HANDLE_CODE(bref.pack(data_tx_params_r14_present, 1));
  HANDLE_CODE(bref.pack(zone_id_r14_present, 1));
  HANDLE_CODE(bref.pack(thresh_s_rssi_cbr_r14_present, 1));
  HANDLE_CODE(bref.pack(pool_report_id_r14_present, 1));
  HANDLE_CODE(bref.pack(cbr_pssch_tx_cfg_list_r14_present, 1));
  HANDLE_CODE(bref.pack(res_sel_cfg_p2_x_r14_present, 1));
  HANDLE_CODE(bref.pack(sync_allowed_r14_present, 1));
  HANDLE_CODE(bref.pack(restrict_res_reserv_period_r14_present, 1));

  if (sl_offset_ind_r14_present) {
    HANDLE_CODE(sl_offset_ind_r14.pack(bref));
  }
  HANDLE_CODE(sl_sf_r14.pack(bref));
  HANDLE_CODE(bref.pack(adjacency_pscch_pssch_r14, 1));
  HANDLE_CODE(size_subch_r14.pack(bref));
  HANDLE_CODE(num_subch_r14.pack(bref));
  HANDLE_CODE(pack_integer(bref, start_rb_subch_r14, (uint8_t)0u, (uint8_t)99u));
  if (start_rb_pscch_pool_r14_present) {
    HANDLE_CODE(pack_integer(bref, start_rb_pscch_pool_r14, (uint8_t)0u, (uint8_t)99u));
  }
  if (rx_params_ncell_r14_present) {
    HANDLE_CODE(bref.pack(rx_params_ncell_r14.tdd_cfg_r14_present, 1));
    if (rx_params_ncell_r14.tdd_cfg_r14_present) {
      HANDLE_CODE(rx_params_ncell_r14.tdd_cfg_r14.pack(bref));
    }
    HANDLE_CODE(pack_integer(bref, rx_params_ncell_r14.sync_cfg_idx_r14, (uint8_t)0u, (uint8_t)15u));
  }
  if (data_tx_params_r14_present) {
    HANDLE_CODE(data_tx_params_r14.pack(bref));
  }
  if (zone_id_r14_present) {
    HANDLE_CODE(pack_integer(bref, zone_id_r14, (uint8_t)0u, (uint8_t)7u));
  }
  if (thresh_s_rssi_cbr_r14_present) {
    HANDLE_CODE(pack_integer(bref, thresh_s_rssi_cbr_r14, (uint8_t)0u, (uint8_t)45u));
  }
  if (pool_report_id_r14_present) {
    HANDLE_CODE(pack_integer(bref, pool_report_id_r14, (uint8_t)1u, (uint8_t)72u));
  }
  if (cbr_pssch_tx_cfg_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cbr_pssch_tx_cfg_list_r14, 1, 8));
  }
  if (res_sel_cfg_p2_x_r14_present) {
    HANDLE_CODE(res_sel_cfg_p2_x_r14.pack(bref));
  }
  if (sync_allowed_r14_present) {
    HANDLE_CODE(sync_allowed_r14.pack(bref));
  }
  if (restrict_res_reserv_period_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, restrict_res_reserv_period_r14, 1, 16));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_min_t2_value_list_r15.is_present();
    group_flags[0] |= cbr_pssch_tx_cfg_list_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_min_t2_value_list_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(cbr_pssch_tx_cfg_list_v1530.is_present(), 1));
      if (sl_min_t2_value_list_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *sl_min_t2_value_list_r15, 1, 8));
      }
      if (cbr_pssch_tx_cfg_list_v1530.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *cbr_pssch_tx_cfg_list_v1530, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_comm_res_pool_v2x_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_offset_ind_r14_present, 1));
  HANDLE_CODE(bref.unpack(start_rb_pscch_pool_r14_present, 1));
  HANDLE_CODE(bref.unpack(rx_params_ncell_r14_present, 1));
  HANDLE_CODE(bref.unpack(data_tx_params_r14_present, 1));
  HANDLE_CODE(bref.unpack(zone_id_r14_present, 1));
  HANDLE_CODE(bref.unpack(thresh_s_rssi_cbr_r14_present, 1));
  HANDLE_CODE(bref.unpack(pool_report_id_r14_present, 1));
  HANDLE_CODE(bref.unpack(cbr_pssch_tx_cfg_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(res_sel_cfg_p2_x_r14_present, 1));
  HANDLE_CODE(bref.unpack(sync_allowed_r14_present, 1));
  HANDLE_CODE(bref.unpack(restrict_res_reserv_period_r14_present, 1));

  if (sl_offset_ind_r14_present) {
    HANDLE_CODE(sl_offset_ind_r14.unpack(bref));
  }
  HANDLE_CODE(sl_sf_r14.unpack(bref));
  HANDLE_CODE(bref.unpack(adjacency_pscch_pssch_r14, 1));
  HANDLE_CODE(size_subch_r14.unpack(bref));
  HANDLE_CODE(num_subch_r14.unpack(bref));
  HANDLE_CODE(unpack_integer(start_rb_subch_r14, bref, (uint8_t)0u, (uint8_t)99u));
  if (start_rb_pscch_pool_r14_present) {
    HANDLE_CODE(unpack_integer(start_rb_pscch_pool_r14, bref, (uint8_t)0u, (uint8_t)99u));
  }
  if (rx_params_ncell_r14_present) {
    HANDLE_CODE(bref.unpack(rx_params_ncell_r14.tdd_cfg_r14_present, 1));
    if (rx_params_ncell_r14.tdd_cfg_r14_present) {
      HANDLE_CODE(rx_params_ncell_r14.tdd_cfg_r14.unpack(bref));
    }
    HANDLE_CODE(unpack_integer(rx_params_ncell_r14.sync_cfg_idx_r14, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (data_tx_params_r14_present) {
    HANDLE_CODE(data_tx_params_r14.unpack(bref));
  }
  if (zone_id_r14_present) {
    HANDLE_CODE(unpack_integer(zone_id_r14, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (thresh_s_rssi_cbr_r14_present) {
    HANDLE_CODE(unpack_integer(thresh_s_rssi_cbr_r14, bref, (uint8_t)0u, (uint8_t)45u));
  }
  if (pool_report_id_r14_present) {
    HANDLE_CODE(unpack_integer(pool_report_id_r14, bref, (uint8_t)1u, (uint8_t)72u));
  }
  if (cbr_pssch_tx_cfg_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cbr_pssch_tx_cfg_list_r14, bref, 1, 8));
  }
  if (res_sel_cfg_p2_x_r14_present) {
    HANDLE_CODE(res_sel_cfg_p2_x_r14.unpack(bref));
  }
  if (sync_allowed_r14_present) {
    HANDLE_CODE(sync_allowed_r14.unpack(bref));
  }
  if (restrict_res_reserv_period_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(restrict_res_reserv_period_r14, bref, 1, 16));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool sl_min_t2_value_list_r15_present;
      HANDLE_CODE(bref.unpack(sl_min_t2_value_list_r15_present, 1));
      sl_min_t2_value_list_r15.set_present(sl_min_t2_value_list_r15_present);
      bool cbr_pssch_tx_cfg_list_v1530_present;
      HANDLE_CODE(bref.unpack(cbr_pssch_tx_cfg_list_v1530_present, 1));
      cbr_pssch_tx_cfg_list_v1530.set_present(cbr_pssch_tx_cfg_list_v1530_present);
      if (sl_min_t2_value_list_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*sl_min_t2_value_list_r15, bref, 1, 8));
      }
      if (cbr_pssch_tx_cfg_list_v1530.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*cbr_pssch_tx_cfg_list_v1530, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_comm_res_pool_v2x_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_offset_ind_r14_present) {
    j.write_fieldname("sl-OffsetIndicator-r14");
    sl_offset_ind_r14.to_json(j);
  }
  j.write_fieldname("sl-Subframe-r14");
  sl_sf_r14.to_json(j);
  j.write_bool("adjacencyPSCCH-PSSCH-r14", adjacency_pscch_pssch_r14);
  j.write_str("sizeSubchannel-r14", size_subch_r14.to_string());
  j.write_str("numSubchannel-r14", num_subch_r14.to_string());
  j.write_int("startRB-Subchannel-r14", start_rb_subch_r14);
  if (start_rb_pscch_pool_r14_present) {
    j.write_int("startRB-PSCCH-Pool-r14", start_rb_pscch_pool_r14);
  }
  if (rx_params_ncell_r14_present) {
    j.write_fieldname("rxParametersNCell-r14");
    j.start_obj();
    if (rx_params_ncell_r14.tdd_cfg_r14_present) {
      j.write_fieldname("tdd-Config-r14");
      rx_params_ncell_r14.tdd_cfg_r14.to_json(j);
    }
    j.write_int("syncConfigIndex-r14", rx_params_ncell_r14.sync_cfg_idx_r14);
    j.end_obj();
  }
  if (data_tx_params_r14_present) {
    j.write_fieldname("dataTxParameters-r14");
    data_tx_params_r14.to_json(j);
  }
  if (zone_id_r14_present) {
    j.write_int("zoneID-r14", zone_id_r14);
  }
  if (thresh_s_rssi_cbr_r14_present) {
    j.write_int("threshS-RSSI-CBR-r14", thresh_s_rssi_cbr_r14);
  }
  if (pool_report_id_r14_present) {
    j.write_int("poolReportId-r14", pool_report_id_r14);
  }
  if (cbr_pssch_tx_cfg_list_r14_present) {
    j.start_array("cbr-pssch-TxConfigList-r14");
    for (const auto& e1 : cbr_pssch_tx_cfg_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (res_sel_cfg_p2_x_r14_present) {
    j.write_fieldname("resourceSelectionConfigP2X-r14");
    res_sel_cfg_p2_x_r14.to_json(j);
  }
  if (sync_allowed_r14_present) {
    j.write_fieldname("syncAllowed-r14");
    sync_allowed_r14.to_json(j);
  }
  if (restrict_res_reserv_period_r14_present) {
    j.start_array("restrictResourceReservationPeriod-r14");
    for (const auto& e1 : restrict_res_reserv_period_r14) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (ext) {
    if (sl_min_t2_value_list_r15.is_present()) {
      j.start_array("sl-MinT2ValueList-r15");
      for (const auto& e1 : *sl_min_t2_value_list_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (cbr_pssch_tx_cfg_list_v1530.is_present()) {
      j.start_array("cbr-pssch-TxConfigList-v1530");
      for (const auto& e1 : *cbr_pssch_tx_cfg_list_v1530) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* sl_comm_res_pool_v2x_r14_s::size_subch_r14_opts::to_string() const
{
  static const char* options[] = {"n4",     "n5",     "n6",     "n8",      "n9",      "n10",     "n12",     "n15",
                                  "n16",    "n18",    "n20",    "n25",     "n30",     "n48",     "n50",     "n72",
                                  "n75",    "n96",    "n100",   "spare13", "spare12", "spare11", "spare10", "spare9",
                                  "spare8", "spare7", "spare6", "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(options, 32, value, "sl_comm_res_pool_v2x_r14_s::size_subch_r14_e_");
}
uint8_t sl_comm_res_pool_v2x_r14_s::size_subch_r14_opts::to_number() const
{
  static const uint8_t options[] = {4, 5, 6, 8, 9, 10, 12, 15, 16, 18, 20, 25, 30, 48, 50, 72, 75, 96, 100};
  return map_enum_number(options, 19, value, "sl_comm_res_pool_v2x_r14_s::size_subch_r14_e_");
}

const char* sl_comm_res_pool_v2x_r14_s::num_subch_r14_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n5", "n8", "n10", "n15", "n20", "spare1"};
  return convert_enum_idx(options, 8, value, "sl_comm_res_pool_v2x_r14_s::num_subch_r14_e_");
}
uint8_t sl_comm_res_pool_v2x_r14_s::num_subch_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 5, 8, 10, 15, 20};
  return map_enum_number(options, 7, value, "sl_comm_res_pool_v2x_r14_s::num_subch_r14_e_");
}

// SL-PoolSelectionConfig-r12 ::= SEQUENCE
SRSASN_CODE sl_pool_sel_cfg_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, thresh_low_r12, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(pack_integer(bref, thresh_high_r12, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pool_sel_cfg_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(thresh_low_r12, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(unpack_integer(thresh_high_r12, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void sl_pool_sel_cfg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("threshLow-r12", thresh_low_r12);
  j.write_int("threshHigh-r12", thresh_high_r12);
  j.end_obj();
}

// SL-SyncConfigNFreq-r13 ::= SEQUENCE
SRSASN_CODE sl_sync_cfg_nfreq_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(async_params_r13_present, 1));
  HANDLE_CODE(bref.pack(tx_params_r13_present, 1));
  HANDLE_CODE(bref.pack(rx_params_r13_present, 1));

  if (async_params_r13_present) {
    HANDLE_CODE(async_params_r13.sync_cp_len_r13.pack(bref));
    HANDLE_CODE(pack_integer(bref, async_params_r13.sync_offset_ind_r13, (uint8_t)0u, (uint8_t)39u));
    HANDLE_CODE(pack_integer(bref, async_params_r13.slssid_r13, (uint8_t)0u, (uint8_t)167u));
  }
  if (tx_params_r13_present) {
    HANDLE_CODE(bref.pack(tx_params_r13.sync_info_reserved_r13_present, 1));
    HANDLE_CODE(bref.pack(tx_params_r13.sync_tx_periodic_r13_present, 1));
    HANDLE_CODE(tx_params_r13.sync_tx_params_r13.pack(bref));
    HANDLE_CODE(pack_integer(bref, tx_params_r13.sync_tx_thresh_ic_r13, (uint8_t)0u, (uint8_t)13u));
    if (tx_params_r13.sync_info_reserved_r13_present) {
      HANDLE_CODE(tx_params_r13.sync_info_reserved_r13.pack(bref));
    }
  }
  if (rx_params_r13_present) {
    HANDLE_CODE(rx_params_r13.disc_sync_win_r13.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sync_offset_ind_v1430_present;
    group_flags[0] |= gnss_sync_r14_present;
    group_flags[1] |= sync_offset_ind2_r14_present;
    group_flags[1] |= sync_offset_ind3_r14_present;
    group_flags[2] |= slss_tx_disabled_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sync_offset_ind_v1430_present, 1));
      HANDLE_CODE(bref.pack(gnss_sync_r14_present, 1));
      if (sync_offset_ind_v1430_present) {
        HANDLE_CODE(pack_integer(bref, sync_offset_ind_v1430, (uint8_t)40u, (uint8_t)159u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sync_offset_ind2_r14_present, 1));
      HANDLE_CODE(bref.pack(sync_offset_ind3_r14_present, 1));
      if (sync_offset_ind2_r14_present) {
        HANDLE_CODE(pack_integer(bref, sync_offset_ind2_r14, (uint8_t)0u, (uint8_t)159u));
      }
      if (sync_offset_ind3_r14_present) {
        HANDLE_CODE(pack_integer(bref, sync_offset_ind3_r14, (uint8_t)0u, (uint8_t)159u));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(slss_tx_disabled_r15_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_sync_cfg_nfreq_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(async_params_r13_present, 1));
  HANDLE_CODE(bref.unpack(tx_params_r13_present, 1));
  HANDLE_CODE(bref.unpack(rx_params_r13_present, 1));

  if (async_params_r13_present) {
    HANDLE_CODE(async_params_r13.sync_cp_len_r13.unpack(bref));
    HANDLE_CODE(unpack_integer(async_params_r13.sync_offset_ind_r13, bref, (uint8_t)0u, (uint8_t)39u));
    HANDLE_CODE(unpack_integer(async_params_r13.slssid_r13, bref, (uint8_t)0u, (uint8_t)167u));
  }
  if (tx_params_r13_present) {
    HANDLE_CODE(bref.unpack(tx_params_r13.sync_info_reserved_r13_present, 1));
    HANDLE_CODE(bref.unpack(tx_params_r13.sync_tx_periodic_r13_present, 1));
    HANDLE_CODE(tx_params_r13.sync_tx_params_r13.unpack(bref));
    HANDLE_CODE(unpack_integer(tx_params_r13.sync_tx_thresh_ic_r13, bref, (uint8_t)0u, (uint8_t)13u));
    if (tx_params_r13.sync_info_reserved_r13_present) {
      HANDLE_CODE(tx_params_r13.sync_info_reserved_r13.unpack(bref));
    }
  }
  if (rx_params_r13_present) {
    HANDLE_CODE(rx_params_r13.disc_sync_win_r13.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sync_offset_ind_v1430_present, 1));
      HANDLE_CODE(bref.unpack(gnss_sync_r14_present, 1));
      if (sync_offset_ind_v1430_present) {
        HANDLE_CODE(unpack_integer(sync_offset_ind_v1430, bref, (uint8_t)40u, (uint8_t)159u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sync_offset_ind2_r14_present, 1));
      HANDLE_CODE(bref.unpack(sync_offset_ind3_r14_present, 1));
      if (sync_offset_ind2_r14_present) {
        HANDLE_CODE(unpack_integer(sync_offset_ind2_r14, bref, (uint8_t)0u, (uint8_t)159u));
      }
      if (sync_offset_ind3_r14_present) {
        HANDLE_CODE(unpack_integer(sync_offset_ind3_r14, bref, (uint8_t)0u, (uint8_t)159u));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(slss_tx_disabled_r15_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void sl_sync_cfg_nfreq_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (async_params_r13_present) {
    j.write_fieldname("asyncParameters-r13");
    j.start_obj();
    j.write_str("syncCP-Len-r13", async_params_r13.sync_cp_len_r13.to_string());
    j.write_int("syncOffsetIndicator-r13", async_params_r13.sync_offset_ind_r13);
    j.write_int("slssid-r13", async_params_r13.slssid_r13);
    j.end_obj();
  }
  if (tx_params_r13_present) {
    j.write_fieldname("txParameters-r13");
    j.start_obj();
    j.write_fieldname("syncTxParameters-r13");
    tx_params_r13.sync_tx_params_r13.to_json(j);
    j.write_int("syncTxThreshIC-r13", tx_params_r13.sync_tx_thresh_ic_r13);
    if (tx_params_r13.sync_info_reserved_r13_present) {
      j.write_str("syncInfoReserved-r13", tx_params_r13.sync_info_reserved_r13.to_string());
    }
    if (tx_params_r13.sync_tx_periodic_r13_present) {
      j.write_str("syncTxPeriodic-r13", "true");
    }
    j.end_obj();
  }
  if (rx_params_r13_present) {
    j.write_fieldname("rxParameters-r13");
    j.start_obj();
    j.write_str("discSyncWindow-r13", rx_params_r13.disc_sync_win_r13.to_string());
    j.end_obj();
  }
  if (ext) {
    if (sync_offset_ind_v1430_present) {
      j.write_int("syncOffsetIndicator-v1430", sync_offset_ind_v1430);
    }
    if (gnss_sync_r14_present) {
      j.write_str("gnss-Sync-r14", "true");
    }
    if (sync_offset_ind2_r14_present) {
      j.write_int("syncOffsetIndicator2-r14", sync_offset_ind2_r14);
    }
    if (sync_offset_ind3_r14_present) {
      j.write_int("syncOffsetIndicator3-r14", sync_offset_ind3_r14);
    }
    if (slss_tx_disabled_r15_present) {
      j.write_str("slss-TxDisabled-r15", "true");
    }
  }
  j.end_obj();
}

const char* sl_sync_cfg_nfreq_r13_s::rx_params_r13_s_::disc_sync_win_r13_opts::to_string() const
{
  static const char* options[] = {"w1", "w2"};
  return convert_enum_idx(options, 2, value, "sl_sync_cfg_nfreq_r13_s::rx_params_r13_s_::disc_sync_win_r13_e_");
}
uint8_t sl_sync_cfg_nfreq_r13_s::rx_params_r13_s_::disc_sync_win_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "sl_sync_cfg_nfreq_r13_s::rx_params_r13_s_::disc_sync_win_r13_e_");
}

// SL-TF-ResourceConfig-r12 ::= SEQUENCE
SRSASN_CODE sl_tf_res_cfg_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, prb_num_r12, (uint8_t)1u, (uint8_t)100u));
  HANDLE_CODE(pack_integer(bref, prb_start_r12, (uint8_t)0u, (uint8_t)99u));
  HANDLE_CODE(pack_integer(bref, prb_end_r12, (uint8_t)0u, (uint8_t)99u));
  HANDLE_CODE(offset_ind_r12.pack(bref));
  HANDLE_CODE(sf_bitmap_r12.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tf_res_cfg_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(prb_num_r12, bref, (uint8_t)1u, (uint8_t)100u));
  HANDLE_CODE(unpack_integer(prb_start_r12, bref, (uint8_t)0u, (uint8_t)99u));
  HANDLE_CODE(unpack_integer(prb_end_r12, bref, (uint8_t)0u, (uint8_t)99u));
  HANDLE_CODE(offset_ind_r12.unpack(bref));
  HANDLE_CODE(sf_bitmap_r12.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_tf_res_cfg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("prb-Num-r12", prb_num_r12);
  j.write_int("prb-Start-r12", prb_start_r12);
  j.write_int("prb-End-r12", prb_end_r12);
  j.write_fieldname("offsetIndicator-r12");
  offset_ind_r12.to_json(j);
  j.write_fieldname("subframeBitmap-r12");
  sf_bitmap_r12.to_json(j);
  j.end_obj();
}

// SL-CommTxPoolSensingConfig-r14 ::= SEQUENCE
SRSASN_CODE sl_comm_tx_pool_sensing_cfg_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(restrict_res_reserv_period_r14_present, 1));
  HANDLE_CODE(bref.pack(p2x_sensing_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(sl_reselect_after_r14_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, pssch_tx_cfg_list_r14, 1, 16));
  HANDLE_CODE(pack_fixed_seq_of(
      bref, &(thres_pssch_rsrp_list_r14)[0], thres_pssch_rsrp_list_r14.size(), integer_packer<uint8_t>(0, 66)));
  if (restrict_res_reserv_period_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, restrict_res_reserv_period_r14, 1, 16));
  }
  HANDLE_CODE(prob_res_keep_r14.pack(bref));
  if (p2x_sensing_cfg_r14_present) {
    HANDLE_CODE(pack_integer(bref, p2x_sensing_cfg_r14.min_num_candidate_sf_r14, (uint8_t)1u, (uint8_t)13u));
    HANDLE_CODE(p2x_sensing_cfg_r14.gap_candidate_sensing_r14.pack(bref));
  }
  if (sl_reselect_after_r14_present) {
    HANDLE_CODE(sl_reselect_after_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_comm_tx_pool_sensing_cfg_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(restrict_res_reserv_period_r14_present, 1));
  HANDLE_CODE(bref.unpack(p2x_sensing_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(sl_reselect_after_r14_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(pssch_tx_cfg_list_r14, bref, 1, 16));
  HANDLE_CODE(unpack_fixed_seq_of(
      &(thres_pssch_rsrp_list_r14)[0], bref, thres_pssch_rsrp_list_r14.size(), integer_packer<uint8_t>(0, 66)));
  if (restrict_res_reserv_period_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(restrict_res_reserv_period_r14, bref, 1, 16));
  }
  HANDLE_CODE(prob_res_keep_r14.unpack(bref));
  if (p2x_sensing_cfg_r14_present) {
    HANDLE_CODE(unpack_integer(p2x_sensing_cfg_r14.min_num_candidate_sf_r14, bref, (uint8_t)1u, (uint8_t)13u));
    HANDLE_CODE(p2x_sensing_cfg_r14.gap_candidate_sensing_r14.unpack(bref));
  }
  if (sl_reselect_after_r14_present) {
    HANDLE_CODE(sl_reselect_after_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_comm_tx_pool_sensing_cfg_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("pssch-TxConfigList-r14");
  for (const auto& e1 : pssch_tx_cfg_list_r14) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("thresPSSCH-RSRP-List-r14");
  for (const auto& e1 : thres_pssch_rsrp_list_r14) {
    j.write_int(e1);
  }
  j.end_array();
  if (restrict_res_reserv_period_r14_present) {
    j.start_array("restrictResourceReservationPeriod-r14");
    for (const auto& e1 : restrict_res_reserv_period_r14) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  j.write_str("probResourceKeep-r14", prob_res_keep_r14.to_string());
  if (p2x_sensing_cfg_r14_present) {
    j.write_fieldname("p2x-SensingConfig-r14");
    j.start_obj();
    j.write_int("minNumCandidateSF-r14", p2x_sensing_cfg_r14.min_num_candidate_sf_r14);
    j.write_str("gapCandidateSensing-r14", p2x_sensing_cfg_r14.gap_candidate_sensing_r14.to_string());
    j.end_obj();
  }
  if (sl_reselect_after_r14_present) {
    j.write_str("sl-ReselectAfter-r14", sl_reselect_after_r14.to_string());
  }
  j.end_obj();
}

const char* sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_opts::to_string() const
{
  static const char* options[] = {"v0", "v0dot2", "v0dot4", "v0dot6", "v0dot8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_e_");
}
float sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.2, 0.4, 0.6, 0.8};
  return map_enum_number(options, 5, value, "sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_e_");
}
const char* sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.2", "0.4", "0.6", "0.8"};
  return convert_enum_idx(options, 8, value, "sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_e_");
}

const char* sl_comm_tx_pool_sensing_cfg_r14_s::sl_reselect_after_r14_opts::to_string() const
{
  static const char* options[] = {"n1",
                                  "n2",
                                  "n3",
                                  "n4",
                                  "n5",
                                  "n6",
                                  "n7",
                                  "n8",
                                  "n9",
                                  "spare7",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sl_comm_tx_pool_sensing_cfg_r14_s::sl_reselect_after_r14_e_");
}
uint8_t sl_comm_tx_pool_sensing_cfg_r14_s::sl_reselect_after_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  return map_enum_number(options, 9, value, "sl_comm_tx_pool_sensing_cfg_r14_s::sl_reselect_after_r14_e_");
}

// SL-DiscResourcePool-r12 ::= SEQUENCE
SRSASN_CODE sl_disc_res_pool_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(tx_params_r12_present, 1));
  HANDLE_CODE(bref.pack(rx_params_r12_present, 1));

  HANDLE_CODE(cp_len_r12.pack(bref));
  HANDLE_CODE(disc_period_r12.pack(bref));
  HANDLE_CODE(pack_integer(bref, num_retx_r12, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(pack_integer(bref, num_repeat_r12, (uint8_t)1u, (uint8_t)50u));
  HANDLE_CODE(tf_res_cfg_r12.pack(bref));
  if (tx_params_r12_present) {
    HANDLE_CODE(bref.pack(tx_params_r12.ue_sel_res_cfg_r12_present, 1));
    HANDLE_CODE(tx_params_r12.tx_params_general_r12.pack(bref));
    if (tx_params_r12.ue_sel_res_cfg_r12_present) {
      HANDLE_CODE(tx_params_r12.ue_sel_res_cfg_r12.pool_sel_r12.pack(bref));
      HANDLE_CODE(tx_params_r12.ue_sel_res_cfg_r12.tx_probability_r12.pack(bref));
    }
  }
  if (rx_params_r12_present) {
    HANDLE_CODE(bref.pack(rx_params_r12.tdd_cfg_r12_present, 1));
    if (rx_params_r12.tdd_cfg_r12_present) {
      HANDLE_CODE(rx_params_r12.tdd_cfg_r12.pack(bref));
    }
    HANDLE_CODE(pack_integer(bref, rx_params_r12.sync_cfg_idx_r12, (uint8_t)0u, (uint8_t)15u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= disc_period_v1310.is_present();
    group_flags[0] |= rx_params_add_neigh_freq_r13.is_present();
    group_flags[0] |= tx_params_add_neigh_freq_r13.is_present();
    group_flags[1] |= tx_params_add_neigh_freq_v1370.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(disc_period_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(rx_params_add_neigh_freq_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(tx_params_add_neigh_freq_r13.is_present(), 1));
      if (disc_period_v1310.is_present()) {
        HANDLE_CODE(disc_period_v1310->pack(bref));
      }
      if (rx_params_add_neigh_freq_r13.is_present()) {
        HANDLE_CODE(rx_params_add_neigh_freq_r13->pack(bref));
      }
      if (tx_params_add_neigh_freq_r13.is_present()) {
        HANDLE_CODE(tx_params_add_neigh_freq_r13->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(tx_params_add_neigh_freq_v1370.is_present(), 1));
      if (tx_params_add_neigh_freq_v1370.is_present()) {
        HANDLE_CODE(tx_params_add_neigh_freq_v1370->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_res_pool_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(tx_params_r12_present, 1));
  HANDLE_CODE(bref.unpack(rx_params_r12_present, 1));

  HANDLE_CODE(cp_len_r12.unpack(bref));
  HANDLE_CODE(disc_period_r12.unpack(bref));
  HANDLE_CODE(unpack_integer(num_retx_r12, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(unpack_integer(num_repeat_r12, bref, (uint8_t)1u, (uint8_t)50u));
  HANDLE_CODE(tf_res_cfg_r12.unpack(bref));
  if (tx_params_r12_present) {
    HANDLE_CODE(bref.unpack(tx_params_r12.ue_sel_res_cfg_r12_present, 1));
    HANDLE_CODE(tx_params_r12.tx_params_general_r12.unpack(bref));
    if (tx_params_r12.ue_sel_res_cfg_r12_present) {
      HANDLE_CODE(tx_params_r12.ue_sel_res_cfg_r12.pool_sel_r12.unpack(bref));
      HANDLE_CODE(tx_params_r12.ue_sel_res_cfg_r12.tx_probability_r12.unpack(bref));
    }
  }
  if (rx_params_r12_present) {
    HANDLE_CODE(bref.unpack(rx_params_r12.tdd_cfg_r12_present, 1));
    if (rx_params_r12.tdd_cfg_r12_present) {
      HANDLE_CODE(rx_params_r12.tdd_cfg_r12.unpack(bref));
    }
    HANDLE_CODE(unpack_integer(rx_params_r12.sync_cfg_idx_r12, bref, (uint8_t)0u, (uint8_t)15u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool disc_period_v1310_present;
      HANDLE_CODE(bref.unpack(disc_period_v1310_present, 1));
      disc_period_v1310.set_present(disc_period_v1310_present);
      bool rx_params_add_neigh_freq_r13_present;
      HANDLE_CODE(bref.unpack(rx_params_add_neigh_freq_r13_present, 1));
      rx_params_add_neigh_freq_r13.set_present(rx_params_add_neigh_freq_r13_present);
      bool tx_params_add_neigh_freq_r13_present;
      HANDLE_CODE(bref.unpack(tx_params_add_neigh_freq_r13_present, 1));
      tx_params_add_neigh_freq_r13.set_present(tx_params_add_neigh_freq_r13_present);
      if (disc_period_v1310.is_present()) {
        HANDLE_CODE(disc_period_v1310->unpack(bref));
      }
      if (rx_params_add_neigh_freq_r13.is_present()) {
        HANDLE_CODE(rx_params_add_neigh_freq_r13->unpack(bref));
      }
      if (tx_params_add_neigh_freq_r13.is_present()) {
        HANDLE_CODE(tx_params_add_neigh_freq_r13->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool tx_params_add_neigh_freq_v1370_present;
      HANDLE_CODE(bref.unpack(tx_params_add_neigh_freq_v1370_present, 1));
      tx_params_add_neigh_freq_v1370.set_present(tx_params_add_neigh_freq_v1370_present);
      if (tx_params_add_neigh_freq_v1370.is_present()) {
        HANDLE_CODE(tx_params_add_neigh_freq_v1370->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_disc_res_pool_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cp-Len-r12", cp_len_r12.to_string());
  j.write_str("discPeriod-r12", disc_period_r12.to_string());
  j.write_int("numRetx-r12", num_retx_r12);
  j.write_int("numRepetition-r12", num_repeat_r12);
  j.write_fieldname("tf-ResourceConfig-r12");
  tf_res_cfg_r12.to_json(j);
  if (tx_params_r12_present) {
    j.write_fieldname("txParameters-r12");
    j.start_obj();
    j.write_fieldname("txParametersGeneral-r12");
    tx_params_r12.tx_params_general_r12.to_json(j);
    if (tx_params_r12.ue_sel_res_cfg_r12_present) {
      j.write_fieldname("ue-SelectedResourceConfig-r12");
      j.start_obj();
      j.write_fieldname("poolSelection-r12");
      tx_params_r12.ue_sel_res_cfg_r12.pool_sel_r12.to_json(j);
      j.write_str("txProbability-r12", tx_params_r12.ue_sel_res_cfg_r12.tx_probability_r12.to_string());
      j.end_obj();
    }
    j.end_obj();
  }
  if (rx_params_r12_present) {
    j.write_fieldname("rxParameters-r12");
    j.start_obj();
    if (rx_params_r12.tdd_cfg_r12_present) {
      j.write_fieldname("tdd-Config-r12");
      rx_params_r12.tdd_cfg_r12.to_json(j);
    }
    j.write_int("syncConfigIndex-r12", rx_params_r12.sync_cfg_idx_r12);
    j.end_obj();
  }
  if (ext) {
    if (disc_period_v1310.is_present()) {
      j.write_fieldname("discPeriod-v1310");
      disc_period_v1310->to_json(j);
    }
    if (rx_params_add_neigh_freq_r13.is_present()) {
      j.write_fieldname("rxParamsAddNeighFreq-r13");
      rx_params_add_neigh_freq_r13->to_json(j);
    }
    if (tx_params_add_neigh_freq_r13.is_present()) {
      j.write_fieldname("txParamsAddNeighFreq-r13");
      tx_params_add_neigh_freq_r13->to_json(j);
    }
    if (tx_params_add_neigh_freq_v1370.is_present()) {
      j.write_fieldname("txParamsAddNeighFreq-v1370");
      tx_params_add_neigh_freq_v1370->to_json(j);
    }
  }
  j.end_obj();
}

const char* sl_disc_res_pool_r12_s::disc_period_r12_opts::to_string() const
{
  static const char* options[] = {"rf32", "rf64", "rf128", "rf256", "rf512", "rf1024", "rf16-v1310", "spare"};
  return convert_enum_idx(options, 8, value, "sl_disc_res_pool_r12_s::disc_period_r12_e_");
}
uint16_t sl_disc_res_pool_r12_s::disc_period_r12_opts::to_number() const
{
  static const uint16_t options[] = {32, 64, 128, 256, 512, 1024, 16};
  return map_enum_number(options, 7, value, "sl_disc_res_pool_r12_s::disc_period_r12_e_");
}

void sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_::set(types::options e)
{
  type_ = e;
}
sl_pool_sel_cfg_r12_s&
sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_::set_rsrp_based_r12()
{
  set(types::rsrp_based_r12);
  return c;
}
void sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_::set_random_r12()
{
  set(types::random_r12);
}
void sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rsrp_based_r12:
      j.write_fieldname("rsrpBased-r12");
      c.to_json(j);
      break;
    case types::random_r12:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rsrp_based_r12:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::random_r12:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rsrp_based_r12:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::random_r12:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::tx_probability_r12_opts::to_string() const
{
  static const char* options[] = {"p25", "p50", "p75", "p100"};
  return convert_enum_idx(
      options, 4, value, "sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::tx_probability_r12_e_");
}
uint8_t sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::tx_probability_r12_opts::to_number() const
{
  static const uint8_t options[] = {25, 50, 75, 100};
  return map_enum_number(
      options, 4, value, "sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::tx_probability_r12_e_");
}

void sl_disc_res_pool_r12_s::disc_period_v1310_c_::set(types::options e)
{
  type_ = e;
}
void sl_disc_res_pool_r12_s::disc_period_v1310_c_::set_release()
{
  set(types::release);
}
sl_disc_res_pool_r12_s::disc_period_v1310_c_::setup_e_& sl_disc_res_pool_r12_s::disc_period_v1310_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_disc_res_pool_r12_s::disc_period_v1310_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_str("setup", c.to_string());
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::disc_period_v1310_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_res_pool_r12_s::disc_period_v1310_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::disc_period_v1310_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_res_pool_r12_s::disc_period_v1310_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::disc_period_v1310_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sl_disc_res_pool_r12_s::disc_period_v1310_c_::setup_opts::to_string() const
{
  static const char* options[] = {"rf4", "rf6", "rf7", "rf8", "rf12", "rf14", "rf24", "rf28"};
  return convert_enum_idx(options, 8, value, "sl_disc_res_pool_r12_s::disc_period_v1310_c_::setup_e_");
}
uint8_t sl_disc_res_pool_r12_s::disc_period_v1310_c_::setup_opts::to_number() const
{
  static const uint8_t options[] = {4, 6, 7, 8, 12, 14, 24, 28};
  return map_enum_number(options, 8, value, "sl_disc_res_pool_r12_s::disc_period_v1310_c_::setup_e_");
}

void sl_disc_res_pool_r12_s::rx_params_add_neigh_freq_r13_c_::set(types::options e)
{
  type_ = e;
}
void sl_disc_res_pool_r12_s::rx_params_add_neigh_freq_r13_c_::set_release()
{
  set(types::release);
}
sl_disc_res_pool_r12_s::rx_params_add_neigh_freq_r13_c_::setup_s_&
sl_disc_res_pool_r12_s::rx_params_add_neigh_freq_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_disc_res_pool_r12_s::rx_params_add_neigh_freq_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("physCellId-r13");
      for (const auto& e1 : c.pci_r13) {
        j.write_int(e1);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::rx_params_add_neigh_freq_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_res_pool_r12_s::rx_params_add_neigh_freq_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.pci_r13, 1, 16, integer_packer<uint16_t>(0, 503)));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::rx_params_add_neigh_freq_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_res_pool_r12_s::rx_params_add_neigh_freq_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.pci_r13, bref, 1, 16, integer_packer<uint16_t>(0, 503)));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::rx_params_add_neigh_freq_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::set(types::options e)
{
  type_ = e;
}
void sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::set_release()
{
  set(types::release);
}
sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::setup_s_&
sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("physCellId-r13");
      for (const auto& e1 : c.pci_r13) {
        j.write_int(e1);
      }
      j.end_array();
      if (c.p_max_present) {
        j.write_int("p-Max", c.p_max);
      }
      if (c.tdd_cfg_r13_present) {
        j.write_fieldname("tdd-Config-r13");
        c.tdd_cfg_r13.to_json(j);
      }
      if (c.tdd_cfg_v1130_present) {
        j.write_fieldname("tdd-Config-v1130");
        c.tdd_cfg_v1130.to_json(j);
      }
      j.write_fieldname("freqInfo");
      j.start_obj();
      if (c.freq_info.ul_carrier_freq_present) {
        j.write_int("ul-CarrierFreq", c.freq_info.ul_carrier_freq);
      }
      if (c.freq_info.ul_bw_present) {
        j.write_str("ul-Bandwidth", c.freq_info.ul_bw.to_string());
      }
      j.write_int("additionalSpectrumEmission", c.freq_info.add_spec_emission);
      j.end_obj();
      j.write_int("referenceSignalPower", c.ref_sig_pwr);
      if (c.sync_cfg_idx_r13_present) {
        j.write_int("syncConfigIndex-r13", c.sync_cfg_idx_r13);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.p_max_present, 1));
      HANDLE_CODE(bref.pack(c.tdd_cfg_r13_present, 1));
      HANDLE_CODE(bref.pack(c.tdd_cfg_v1130_present, 1));
      HANDLE_CODE(bref.pack(c.sync_cfg_idx_r13_present, 1));
      HANDLE_CODE(pack_dyn_seq_of(bref, c.pci_r13, 1, 16, integer_packer<uint16_t>(0, 503)));
      if (c.p_max_present) {
        HANDLE_CODE(pack_integer(bref, c.p_max, (int8_t)-30, (int8_t)33));
      }
      if (c.tdd_cfg_r13_present) {
        HANDLE_CODE(c.tdd_cfg_r13.pack(bref));
      }
      if (c.tdd_cfg_v1130_present) {
        HANDLE_CODE(c.tdd_cfg_v1130.pack(bref));
      }
      HANDLE_CODE(bref.pack(c.freq_info.ul_carrier_freq_present, 1));
      HANDLE_CODE(bref.pack(c.freq_info.ul_bw_present, 1));
      if (c.freq_info.ul_carrier_freq_present) {
        HANDLE_CODE(pack_integer(bref, c.freq_info.ul_carrier_freq, (uint32_t)0u, (uint32_t)65535u));
      }
      if (c.freq_info.ul_bw_present) {
        HANDLE_CODE(c.freq_info.ul_bw.pack(bref));
      }
      HANDLE_CODE(pack_integer(bref, c.freq_info.add_spec_emission, (uint8_t)1u, (uint8_t)32u));
      HANDLE_CODE(pack_integer(bref, c.ref_sig_pwr, (int8_t)-60, (int8_t)50));
      if (c.sync_cfg_idx_r13_present) {
        HANDLE_CODE(pack_integer(bref, c.sync_cfg_idx_r13, (uint8_t)0u, (uint8_t)15u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.p_max_present, 1));
      HANDLE_CODE(bref.unpack(c.tdd_cfg_r13_present, 1));
      HANDLE_CODE(bref.unpack(c.tdd_cfg_v1130_present, 1));
      HANDLE_CODE(bref.unpack(c.sync_cfg_idx_r13_present, 1));
      HANDLE_CODE(unpack_dyn_seq_of(c.pci_r13, bref, 1, 16, integer_packer<uint16_t>(0, 503)));
      if (c.p_max_present) {
        HANDLE_CODE(unpack_integer(c.p_max, bref, (int8_t)-30, (int8_t)33));
      }
      if (c.tdd_cfg_r13_present) {
        HANDLE_CODE(c.tdd_cfg_r13.unpack(bref));
      }
      if (c.tdd_cfg_v1130_present) {
        HANDLE_CODE(c.tdd_cfg_v1130.unpack(bref));
      }
      HANDLE_CODE(bref.unpack(c.freq_info.ul_carrier_freq_present, 1));
      HANDLE_CODE(bref.unpack(c.freq_info.ul_bw_present, 1));
      if (c.freq_info.ul_carrier_freq_present) {
        HANDLE_CODE(unpack_integer(c.freq_info.ul_carrier_freq, bref, (uint32_t)0u, (uint32_t)65535u));
      }
      if (c.freq_info.ul_bw_present) {
        HANDLE_CODE(c.freq_info.ul_bw.unpack(bref));
      }
      HANDLE_CODE(unpack_integer(c.freq_info.add_spec_emission, bref, (uint8_t)1u, (uint8_t)32u));
      HANDLE_CODE(unpack_integer(c.ref_sig_pwr, bref, (int8_t)-60, (int8_t)50));
      if (c.sync_cfg_idx_r13_present) {
        HANDLE_CODE(unpack_integer(c.sync_cfg_idx_r13, bref, (uint8_t)0u, (uint8_t)15u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char*
sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::setup_s_::freq_info_s_::ul_bw_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(
      options, 6, value, "sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::setup_s_::freq_info_s_::ul_bw_e_");
}
uint8_t sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::setup_s_::freq_info_s_::ul_bw_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(
      options, 6, value, "sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::setup_s_::freq_info_s_::ul_bw_e_");
}

void sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_v1370_c_::set(types::options e)
{
  type_ = e;
}
void sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_v1370_c_::set_release()
{
  set(types::release);
}
sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_v1370_c_::setup_s_&
sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_v1370_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_v1370_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("freqInfo-v1370");
      j.start_obj();
      j.write_int("additionalSpectrumEmission-v1370", c.freq_info_v1370.add_spec_emission_v1370);
      j.end_obj();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_v1370_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_v1370_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c.freq_info_v1370.add_spec_emission_v1370, (uint16_t)33u, (uint16_t)288u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_v1370_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_v1370_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c.freq_info_v1370.add_spec_emission_v1370, bref, (uint16_t)33u, (uint16_t)288u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_v1370_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-ZoneConfig-r14 ::= SEQUENCE
SRSASN_CODE sl_zone_cfg_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(zone_len_r14.pack(bref));
  HANDLE_CODE(zone_width_r14.pack(bref));
  HANDLE_CODE(pack_integer(bref, zone_id_longi_mod_r14, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, zone_id_lati_mod_r14, (uint8_t)1u, (uint8_t)4u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_zone_cfg_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(zone_len_r14.unpack(bref));
  HANDLE_CODE(zone_width_r14.unpack(bref));
  HANDLE_CODE(unpack_integer(zone_id_longi_mod_r14, bref, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(zone_id_lati_mod_r14, bref, (uint8_t)1u, (uint8_t)4u));

  return SRSASN_SUCCESS;
}
void sl_zone_cfg_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("zoneLength-r14", zone_len_r14.to_string());
  j.write_str("zoneWidth-r14", zone_width_r14.to_string());
  j.write_int("zoneIdLongiMod-r14", zone_id_longi_mod_r14);
  j.write_int("zoneIdLatiMod-r14", zone_id_lati_mod_r14);
  j.end_obj();
}

const char* sl_zone_cfg_r14_s::zone_len_r14_opts::to_string() const
{
  static const char* options[] = {"m5", "m10", "m20", "m50", "m100", "m200", "m500", "spare1"};
  return convert_enum_idx(options, 8, value, "sl_zone_cfg_r14_s::zone_len_r14_e_");
}
uint16_t sl_zone_cfg_r14_s::zone_len_r14_opts::to_number() const
{
  static const uint16_t options[] = {5, 10, 20, 50, 100, 200, 500};
  return map_enum_number(options, 7, value, "sl_zone_cfg_r14_s::zone_len_r14_e_");
}

const char* sl_zone_cfg_r14_s::zone_width_r14_opts::to_string() const
{
  static const char* options[] = {"m5", "m10", "m20", "m50", "m100", "m200", "m500", "spare1"};
  return convert_enum_idx(options, 8, value, "sl_zone_cfg_r14_s::zone_width_r14_e_");
}
uint16_t sl_zone_cfg_r14_s::zone_width_r14_opts::to_number() const
{
  static const uint16_t options[] = {5, 10, 20, 50, 100, 200, 500};
  return map_enum_number(options, 7, value, "sl_zone_cfg_r14_s::zone_width_r14_e_");
}

// PhysCellIdRange ::= SEQUENCE
SRSASN_CODE pci_range_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(range_present, 1));

  HANDLE_CODE(pack_integer(bref, start, (uint16_t)0u, (uint16_t)503u));
  if (range_present) {
    HANDLE_CODE(range.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pci_range_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(range_present, 1));

  HANDLE_CODE(unpack_integer(start, bref, (uint16_t)0u, (uint16_t)503u));
  if (range_present) {
    HANDLE_CODE(range.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pci_range_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("start", start);
  if (range_present) {
    j.write_str("range", range.to_string());
  }
  j.end_obj();
}
bool pci_range_s::operator==(const pci_range_s& other) const
{
  return start == other.start and range_present == other.range_present and (not range_present or range == other.range);
}

const char* pci_range_s::range_opts::to_string() const
{
  static const char* options[] = {"n4",
                                  "n8",
                                  "n12",
                                  "n16",
                                  "n24",
                                  "n32",
                                  "n48",
                                  "n64",
                                  "n84",
                                  "n96",
                                  "n128",
                                  "n168",
                                  "n252",
                                  "n504",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pci_range_s::range_e_");
}
uint16_t pci_range_s::range_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 12, 16, 24, 32, 48, 64, 84, 96, 128, 168, 252, 504};
  return map_enum_number(options, 14, value, "pci_range_s::range_e_");
}

// SL-DiscTxPowerInfo-r12 ::= SEQUENCE
SRSASN_CODE sl_disc_tx_pwr_info_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, disc_max_tx_pwr_r12, (int8_t)-30, (int8_t)33));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_tx_pwr_info_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(disc_max_tx_pwr_r12, bref, (int8_t)-30, (int8_t)33));

  return SRSASN_SUCCESS;
}
void sl_disc_tx_pwr_info_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("discMaxTxPower-r12", disc_max_tx_pwr_r12);
  j.end_obj();
}

// SL-V2X-FreqSelectionConfig-r15 ::= SEQUENCE
SRSASN_CODE sl_v2x_freq_sel_cfg_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(thresh_cbr_freq_resel_r15_present, 1));
  HANDLE_CODE(bref.pack(thresh_cbr_freq_keeping_r15_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, prio_list_r15, 1, 8, integer_packer<uint8_t>(1, 8)));
  if (thresh_cbr_freq_resel_r15_present) {
    HANDLE_CODE(pack_integer(bref, thresh_cbr_freq_resel_r15, (uint8_t)0u, (uint8_t)100u));
  }
  if (thresh_cbr_freq_keeping_r15_present) {
    HANDLE_CODE(pack_integer(bref, thresh_cbr_freq_keeping_r15, (uint8_t)0u, (uint8_t)100u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_v2x_freq_sel_cfg_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(thresh_cbr_freq_resel_r15_present, 1));
  HANDLE_CODE(bref.unpack(thresh_cbr_freq_keeping_r15_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(prio_list_r15, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  if (thresh_cbr_freq_resel_r15_present) {
    HANDLE_CODE(unpack_integer(thresh_cbr_freq_resel_r15, bref, (uint8_t)0u, (uint8_t)100u));
  }
  if (thresh_cbr_freq_keeping_r15_present) {
    HANDLE_CODE(unpack_integer(thresh_cbr_freq_keeping_r15, bref, (uint8_t)0u, (uint8_t)100u));
  }

  return SRSASN_SUCCESS;
}
void sl_v2x_freq_sel_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("priorityList-r15");
  for (const auto& e1 : prio_list_r15) {
    j.write_int(e1);
  }
  j.end_array();
  if (thresh_cbr_freq_resel_r15_present) {
    j.write_int("threshCBR-FreqReselection-r15", thresh_cbr_freq_resel_r15);
  }
  if (thresh_cbr_freq_keeping_r15_present) {
    j.write_int("threshCBR-FreqKeeping-r15", thresh_cbr_freq_keeping_r15);
  }
  j.end_obj();
}

// SL-V2X-InterFreqUE-Config-r14 ::= SEQUENCE
SRSASN_CODE sl_v2x_inter_freq_ue_cfg_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pci_list_r14_present, 1));
  HANDLE_CODE(bref.pack(type_tx_sync_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_sync_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_comm_rx_pool_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_comm_tx_pool_normal_r14_present, 1));
  HANDLE_CODE(bref.pack(p2x_comm_tx_pool_normal_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_comm_tx_pool_exceptional_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_res_sel_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(zone_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(offset_dfn_r14_present, 1));

  if (pci_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pci_list_r14, 1, 16, integer_packer<uint16_t>(0, 503)));
  }
  if (type_tx_sync_r14_present) {
    HANDLE_CODE(type_tx_sync_r14.pack(bref));
  }
  if (v2x_sync_cfg_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_sync_cfg_r14, 1, 16));
  }
  if (v2x_comm_rx_pool_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_comm_rx_pool_r14, 1, 16));
  }
  if (v2x_comm_tx_pool_normal_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_comm_tx_pool_normal_r14, 1, 8));
  }
  if (p2x_comm_tx_pool_normal_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, p2x_comm_tx_pool_normal_r14, 1, 8));
  }
  if (v2x_comm_tx_pool_exceptional_r14_present) {
    HANDLE_CODE(v2x_comm_tx_pool_exceptional_r14.pack(bref));
  }
  if (v2x_res_sel_cfg_r14_present) {
    HANDLE_CODE(v2x_res_sel_cfg_r14.pack(bref));
  }
  if (zone_cfg_r14_present) {
    HANDLE_CODE(zone_cfg_r14.pack(bref));
  }
  if (offset_dfn_r14_present) {
    HANDLE_CODE(pack_integer(bref, offset_dfn_r14, (uint16_t)0u, (uint16_t)1000u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_v2x_inter_freq_ue_cfg_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pci_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(type_tx_sync_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_sync_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_comm_rx_pool_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_comm_tx_pool_normal_r14_present, 1));
  HANDLE_CODE(bref.unpack(p2x_comm_tx_pool_normal_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_comm_tx_pool_exceptional_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_res_sel_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(zone_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(offset_dfn_r14_present, 1));

  if (pci_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pci_list_r14, bref, 1, 16, integer_packer<uint16_t>(0, 503)));
  }
  if (type_tx_sync_r14_present) {
    HANDLE_CODE(type_tx_sync_r14.unpack(bref));
  }
  if (v2x_sync_cfg_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_sync_cfg_r14, bref, 1, 16));
  }
  if (v2x_comm_rx_pool_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_comm_rx_pool_r14, bref, 1, 16));
  }
  if (v2x_comm_tx_pool_normal_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_comm_tx_pool_normal_r14, bref, 1, 8));
  }
  if (p2x_comm_tx_pool_normal_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(p2x_comm_tx_pool_normal_r14, bref, 1, 8));
  }
  if (v2x_comm_tx_pool_exceptional_r14_present) {
    HANDLE_CODE(v2x_comm_tx_pool_exceptional_r14.unpack(bref));
  }
  if (v2x_res_sel_cfg_r14_present) {
    HANDLE_CODE(v2x_res_sel_cfg_r14.unpack(bref));
  }
  if (zone_cfg_r14_present) {
    HANDLE_CODE(zone_cfg_r14.unpack(bref));
  }
  if (offset_dfn_r14_present) {
    HANDLE_CODE(unpack_integer(offset_dfn_r14, bref, (uint16_t)0u, (uint16_t)1000u));
  }

  return SRSASN_SUCCESS;
}
void sl_v2x_inter_freq_ue_cfg_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pci_list_r14_present) {
    j.start_array("physCellIdList-r14");
    for (const auto& e1 : pci_list_r14) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (type_tx_sync_r14_present) {
    j.write_str("typeTxSync-r14", type_tx_sync_r14.to_string());
  }
  if (v2x_sync_cfg_r14_present) {
    j.start_array("v2x-SyncConfig-r14");
    for (const auto& e1 : v2x_sync_cfg_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (v2x_comm_rx_pool_r14_present) {
    j.start_array("v2x-CommRxPool-r14");
    for (const auto& e1 : v2x_comm_rx_pool_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (v2x_comm_tx_pool_normal_r14_present) {
    j.start_array("v2x-CommTxPoolNormal-r14");
    for (const auto& e1 : v2x_comm_tx_pool_normal_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (p2x_comm_tx_pool_normal_r14_present) {
    j.start_array("p2x-CommTxPoolNormal-r14");
    for (const auto& e1 : p2x_comm_tx_pool_normal_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (v2x_comm_tx_pool_exceptional_r14_present) {
    j.write_fieldname("v2x-CommTxPoolExceptional-r14");
    v2x_comm_tx_pool_exceptional_r14.to_json(j);
  }
  if (v2x_res_sel_cfg_r14_present) {
    j.write_fieldname("v2x-ResourceSelectionConfig-r14");
    v2x_res_sel_cfg_r14.to_json(j);
  }
  if (zone_cfg_r14_present) {
    j.write_fieldname("zoneConfig-r14");
    zone_cfg_r14.to_json(j);
  }
  if (offset_dfn_r14_present) {
    j.write_int("offsetDFN-r14", offset_dfn_r14);
  }
  j.end_obj();
}

// AllowedMeasBandwidth ::= ENUMERATED
const char* allowed_meas_bw_opts::to_string() const
{
  static const char* options[] = {"mbw6", "mbw15", "mbw25", "mbw50", "mbw75", "mbw100"};
  return convert_enum_idx(options, 6, value, "allowed_meas_bw_e");
}
uint8_t allowed_meas_bw_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "allowed_meas_bw_e");
}

// CellSelectionInfoNFreq-r13 ::= SEQUENCE
SRSASN_CODE cell_sel_info_nfreq_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(q_rx_lev_min_offset_present, 1));

  HANDLE_CODE(pack_integer(bref, q_rx_lev_min_r13, (int8_t)-70, (int8_t)-22));
  if (q_rx_lev_min_offset_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_offset, (uint8_t)1u, (uint8_t)8u));
  }
  HANDLE_CODE(q_hyst_r13.pack(bref));
  HANDLE_CODE(pack_integer(bref, q_rx_lev_min_resel_r13, (int8_t)-70, (int8_t)-22));
  HANDLE_CODE(pack_integer(bref, t_resel_eutra_r13, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_sel_info_nfreq_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(q_rx_lev_min_offset_present, 1));

  HANDLE_CODE(unpack_integer(q_rx_lev_min_r13, bref, (int8_t)-70, (int8_t)-22));
  if (q_rx_lev_min_offset_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_offset, bref, (uint8_t)1u, (uint8_t)8u));
  }
  HANDLE_CODE(q_hyst_r13.unpack(bref));
  HANDLE_CODE(unpack_integer(q_rx_lev_min_resel_r13, bref, (int8_t)-70, (int8_t)-22));
  HANDLE_CODE(unpack_integer(t_resel_eutra_r13, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void cell_sel_info_nfreq_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("q-RxLevMin-r13", q_rx_lev_min_r13);
  if (q_rx_lev_min_offset_present) {
    j.write_int("q-RxLevMinOffset", q_rx_lev_min_offset);
  }
  j.write_str("q-Hyst-r13", q_hyst_r13.to_string());
  j.write_int("q-RxLevMinReselection-r13", q_rx_lev_min_resel_r13);
  j.write_int("t-ReselectionEUTRA-r13", t_resel_eutra_r13);
  j.end_obj();
}

const char* cell_sel_info_nfreq_r13_s::q_hyst_r13_opts::to_string() const
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
  return convert_enum_idx(options, 16, value, "cell_sel_info_nfreq_r13_s::q_hyst_r13_e_");
}
uint8_t cell_sel_info_nfreq_r13_s::q_hyst_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(options, 16, value, "cell_sel_info_nfreq_r13_s::q_hyst_r13_e_");
}

// SL-AllowedCarrierFreqList-r15 ::= SEQUENCE
SRSASN_CODE sl_allowed_carrier_freq_list_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, allowed_carrier_freq_set1, 1, 8, integer_packer<uint32_t>(0, 262143)));
  HANDLE_CODE(pack_dyn_seq_of(bref, allowed_carrier_freq_set2, 1, 8, integer_packer<uint32_t>(0, 262143)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_allowed_carrier_freq_list_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(allowed_carrier_freq_set1, bref, 1, 8, integer_packer<uint32_t>(0, 262143)));
  HANDLE_CODE(unpack_dyn_seq_of(allowed_carrier_freq_set2, bref, 1, 8, integer_packer<uint32_t>(0, 262143)));

  return SRSASN_SUCCESS;
}
void sl_allowed_carrier_freq_list_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("allowedCarrierFreqSet1");
  for (const auto& e1 : allowed_carrier_freq_set1) {
    j.write_int(e1);
  }
  j.end_array();
  j.start_array("allowedCarrierFreqSet2");
  for (const auto& e1 : allowed_carrier_freq_set2) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// SL-DiscTxResourcesInterFreq-r13 ::= CHOICE
void sl_disc_tx_res_inter_freq_r13_c::set(types::options e)
{
  type_ = e;
}
void sl_disc_tx_res_inter_freq_r13_c::set_acquire_si_from_carrier_r13()
{
  set(types::acquire_si_from_carrier_r13);
}
sl_disc_tx_pool_list_r12_l& sl_disc_tx_res_inter_freq_r13_c::set_disc_tx_pool_common_r13()
{
  set(types::disc_tx_pool_common_r13);
  return c;
}
void sl_disc_tx_res_inter_freq_r13_c::set_request_ded_r13()
{
  set(types::request_ded_r13);
}
void sl_disc_tx_res_inter_freq_r13_c::set_no_tx_on_carrier_r13()
{
  set(types::no_tx_on_carrier_r13);
}
void sl_disc_tx_res_inter_freq_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::acquire_si_from_carrier_r13:
      break;
    case types::disc_tx_pool_common_r13:
      j.start_array("discTxPoolCommon-r13");
      for (const auto& e1 : c) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::request_ded_r13:
      break;
    case types::no_tx_on_carrier_r13:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_tx_res_inter_freq_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_tx_res_inter_freq_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::acquire_si_from_carrier_r13:
      break;
    case types::disc_tx_pool_common_r13:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 4));
      break;
    case types::request_ded_r13:
      break;
    case types::no_tx_on_carrier_r13:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_tx_res_inter_freq_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_tx_res_inter_freq_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::acquire_si_from_carrier_r13:
      break;
    case types::disc_tx_pool_common_r13:
      HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 4));
      break;
    case types::request_ded_r13:
      break;
    case types::no_tx_on_carrier_r13:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_tx_res_inter_freq_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MeasIdleCarrierEUTRA-r15 ::= SEQUENCE
SRSASN_CODE meas_idle_carrier_eutra_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(validity_area_r15_present, 1));
  HANDLE_CODE(bref.pack(meas_cell_list_r15_present, 1));
  HANDLE_CODE(bref.pack(quality_thres_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r15, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(allowed_meas_bw_r15.pack(bref));
  if (validity_area_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, validity_area_r15, 1, 8));
  }
  if (meas_cell_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_cell_list_r15, 1, 8));
  }
  HANDLE_CODE(report_quantities.pack(bref));
  if (quality_thres_r15_present) {
    HANDLE_CODE(bref.pack(quality_thres_r15.idle_rsrp_thres_r15_present, 1));
    HANDLE_CODE(bref.pack(quality_thres_r15.idle_rsrq_thres_r15_present, 1));
    if (quality_thres_r15.idle_rsrp_thres_r15_present) {
      HANDLE_CODE(pack_integer(bref, quality_thres_r15.idle_rsrp_thres_r15, (uint8_t)0u, (uint8_t)97u));
    }
    if (quality_thres_r15.idle_rsrq_thres_r15_present) {
      HANDLE_CODE(pack_integer(bref, quality_thres_r15.idle_rsrq_thres_r15, (int8_t)-30, (int8_t)46));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_idle_carrier_eutra_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(validity_area_r15_present, 1));
  HANDLE_CODE(bref.unpack(meas_cell_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(quality_thres_r15_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r15, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(allowed_meas_bw_r15.unpack(bref));
  if (validity_area_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(validity_area_r15, bref, 1, 8));
  }
  if (meas_cell_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_cell_list_r15, bref, 1, 8));
  }
  HANDLE_CODE(report_quantities.unpack(bref));
  if (quality_thres_r15_present) {
    HANDLE_CODE(bref.unpack(quality_thres_r15.idle_rsrp_thres_r15_present, 1));
    HANDLE_CODE(bref.unpack(quality_thres_r15.idle_rsrq_thres_r15_present, 1));
    if (quality_thres_r15.idle_rsrp_thres_r15_present) {
      HANDLE_CODE(unpack_integer(quality_thres_r15.idle_rsrp_thres_r15, bref, (uint8_t)0u, (uint8_t)97u));
    }
    if (quality_thres_r15.idle_rsrq_thres_r15_present) {
      HANDLE_CODE(unpack_integer(quality_thres_r15.idle_rsrq_thres_r15, bref, (int8_t)-30, (int8_t)46));
    }
  }

  return SRSASN_SUCCESS;
}
void meas_idle_carrier_eutra_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r15", carrier_freq_r15);
  j.write_str("allowedMeasBandwidth-r15", allowed_meas_bw_r15.to_string());
  if (validity_area_r15_present) {
    j.start_array("validityArea-r15");
    for (const auto& e1 : validity_area_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_cell_list_r15_present) {
    j.start_array("measCellList-r15");
    for (const auto& e1 : meas_cell_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("reportQuantities", report_quantities.to_string());
  if (quality_thres_r15_present) {
    j.write_fieldname("qualityThreshold-r15");
    j.start_obj();
    if (quality_thres_r15.idle_rsrp_thres_r15_present) {
      j.write_int("idleRSRP-Threshold-r15", quality_thres_r15.idle_rsrp_thres_r15);
    }
    if (quality_thres_r15.idle_rsrq_thres_r15_present) {
      j.write_int("idleRSRQ-Threshold-r15", quality_thres_r15.idle_rsrq_thres_r15);
    }
    j.end_obj();
  }
  j.end_obj();
}

const char* meas_idle_carrier_eutra_r15_s::report_quantities_opts::to_string() const
{
  static const char* options[] = {"rsrp", "rsrq", "both"};
  return convert_enum_idx(options, 3, value, "meas_idle_carrier_eutra_r15_s::report_quantities_e_");
}

// SL-CBR-PSSCH-TxConfig-r14 ::= SEQUENCE
SRSASN_CODE sl_cbr_pssch_tx_cfg_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cr_limit_r14, (uint16_t)0u, (uint16_t)10000u));
  HANDLE_CODE(tx_params_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_cbr_pssch_tx_cfg_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cr_limit_r14, bref, (uint16_t)0u, (uint16_t)10000u));
  HANDLE_CODE(tx_params_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_cbr_pssch_tx_cfg_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cr-Limit-r14", cr_limit_r14);
  j.write_fieldname("tx-Parameters-r14");
  tx_params_r14.to_json(j);
  j.end_obj();
}

// SL-HoppingConfigComm-r12 ::= SEQUENCE
SRSASN_CODE sl_hop_cfg_comm_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, hop_param_r12, (uint16_t)0u, (uint16_t)504u));
  HANDLE_CODE(num_subbands_r12.pack(bref));
  HANDLE_CODE(pack_integer(bref, rb_offset_r12, (uint8_t)0u, (uint8_t)110u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_hop_cfg_comm_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(hop_param_r12, bref, (uint16_t)0u, (uint16_t)504u));
  HANDLE_CODE(num_subbands_r12.unpack(bref));
  HANDLE_CODE(unpack_integer(rb_offset_r12, bref, (uint8_t)0u, (uint8_t)110u));

  return SRSASN_SUCCESS;
}
void sl_hop_cfg_comm_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("hoppingParameter-r12", hop_param_r12);
  j.write_str("numSubbands-r12", num_subbands_r12.to_string());
  j.write_int("rb-Offset-r12", rb_offset_r12);
  j.end_obj();
}

const char* sl_hop_cfg_comm_r12_s::num_subbands_r12_opts::to_string() const
{
  static const char* options[] = {"ns1", "ns2", "ns4"};
  return convert_enum_idx(options, 3, value, "sl_hop_cfg_comm_r12_s::num_subbands_r12_e_");
}
uint8_t sl_hop_cfg_comm_r12_s::num_subbands_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "sl_hop_cfg_comm_r12_s::num_subbands_r12_e_");
}

// SL-InterFreqInfoV2X-r14 ::= SEQUENCE
SRSASN_CODE sl_inter_freq_info_v2x_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(plmn_id_list_r14_present, 1));
  HANDLE_CODE(bref.pack(sl_max_tx_pwr_r14_present, 1));
  HANDLE_CODE(bref.pack(sl_bw_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_sched_pool_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_ue_cfg_list_r14_present, 1));

  if (plmn_id_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_r14, 1, 6));
  }
  HANDLE_CODE(pack_integer(bref, v2x_comm_carrier_freq_r14, (uint32_t)0u, (uint32_t)262143u));
  if (sl_max_tx_pwr_r14_present) {
    HANDLE_CODE(pack_integer(bref, sl_max_tx_pwr_r14, (int8_t)-30, (int8_t)33));
  }
  if (sl_bw_r14_present) {
    HANDLE_CODE(sl_bw_r14.pack(bref));
  }
  if (v2x_sched_pool_r14_present) {
    HANDLE_CODE(v2x_sched_pool_r14.pack(bref));
  }
  if (v2x_ue_cfg_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_ue_cfg_list_r14, 1, 16));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= add_spec_emission_v2x_r14.is_present();
    group_flags[1] |= v2x_freq_sel_cfg_list_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(add_spec_emission_v2x_r14.is_present(), 1));
      if (add_spec_emission_v2x_r14.is_present()) {
        HANDLE_CODE(add_spec_emission_v2x_r14->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(v2x_freq_sel_cfg_list_r15.is_present(), 1));
      if (v2x_freq_sel_cfg_list_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *v2x_freq_sel_cfg_list_r15, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_inter_freq_info_v2x_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(plmn_id_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(sl_max_tx_pwr_r14_present, 1));
  HANDLE_CODE(bref.unpack(sl_bw_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_sched_pool_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_ue_cfg_list_r14_present, 1));

  if (plmn_id_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_r14, bref, 1, 6));
  }
  HANDLE_CODE(unpack_integer(v2x_comm_carrier_freq_r14, bref, (uint32_t)0u, (uint32_t)262143u));
  if (sl_max_tx_pwr_r14_present) {
    HANDLE_CODE(unpack_integer(sl_max_tx_pwr_r14, bref, (int8_t)-30, (int8_t)33));
  }
  if (sl_bw_r14_present) {
    HANDLE_CODE(sl_bw_r14.unpack(bref));
  }
  if (v2x_sched_pool_r14_present) {
    HANDLE_CODE(v2x_sched_pool_r14.unpack(bref));
  }
  if (v2x_ue_cfg_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_ue_cfg_list_r14, bref, 1, 16));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool add_spec_emission_v2x_r14_present;
      HANDLE_CODE(bref.unpack(add_spec_emission_v2x_r14_present, 1));
      add_spec_emission_v2x_r14.set_present(add_spec_emission_v2x_r14_present);
      if (add_spec_emission_v2x_r14.is_present()) {
        HANDLE_CODE(add_spec_emission_v2x_r14->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool v2x_freq_sel_cfg_list_r15_present;
      HANDLE_CODE(bref.unpack(v2x_freq_sel_cfg_list_r15_present, 1));
      v2x_freq_sel_cfg_list_r15.set_present(v2x_freq_sel_cfg_list_r15_present);
      if (v2x_freq_sel_cfg_list_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*v2x_freq_sel_cfg_list_r15, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_inter_freq_info_v2x_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_list_r14_present) {
    j.start_array("plmn-IdentityList-r14");
    for (const auto& e1 : plmn_id_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_int("v2x-CommCarrierFreq-r14", v2x_comm_carrier_freq_r14);
  if (sl_max_tx_pwr_r14_present) {
    j.write_int("sl-MaxTxPower-r14", sl_max_tx_pwr_r14);
  }
  if (sl_bw_r14_present) {
    j.write_str("sl-Bandwidth-r14", sl_bw_r14.to_string());
  }
  if (v2x_sched_pool_r14_present) {
    j.write_fieldname("v2x-SchedulingPool-r14");
    v2x_sched_pool_r14.to_json(j);
  }
  if (v2x_ue_cfg_list_r14_present) {
    j.start_array("v2x-UE-ConfigList-r14");
    for (const auto& e1 : v2x_ue_cfg_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (add_spec_emission_v2x_r14.is_present()) {
      j.write_fieldname("additionalSpectrumEmissionV2X-r14");
      add_spec_emission_v2x_r14->to_json(j);
    }
    if (v2x_freq_sel_cfg_list_r15.is_present()) {
      j.start_array("v2x-FreqSelectionConfigList-r15");
      for (const auto& e1 : *v2x_freq_sel_cfg_list_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* sl_inter_freq_info_v2x_r14_s::sl_bw_r14_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "sl_inter_freq_info_v2x_r14_s::sl_bw_r14_e_");
}
uint8_t sl_inter_freq_info_v2x_r14_s::sl_bw_r14_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "sl_inter_freq_info_v2x_r14_s::sl_bw_r14_e_");
}

void sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::destroy_() {}
void sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::add_spec_emission_v2x_r14_c_(
    const sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::add_spec_emission_r14:
      c.init(other.c.get<uint8_t>());
      break;
    case types::add_spec_emission_v1440:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_");
  }
}
sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_&
sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::operator=(
    const sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::add_spec_emission_r14:
      c.set(other.c.get<uint8_t>());
      break;
    case types::add_spec_emission_v1440:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_");
  }

  return *this;
}
uint8_t& sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::set_add_spec_emission_r14()
{
  set(types::add_spec_emission_r14);
  return c.get<uint8_t>();
}
uint16_t& sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::set_add_spec_emission_v1440()
{
  set(types::add_spec_emission_v1440);
  return c.get<uint16_t>();
}
void sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::add_spec_emission_r14:
      j.write_int("additionalSpectrumEmission-r14", c.get<uint8_t>());
      break;
    case types::add_spec_emission_v1440:
      j.write_int("additionalSpectrumEmission-v1440", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::add_spec_emission_r14:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)32u));
      break;
    case types::add_spec_emission_v1440:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)33u, (uint16_t)288u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::add_spec_emission_r14:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)32u));
      break;
    case types::add_spec_emission_v1440:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)33u, (uint16_t)288u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-PPPR-Dest-CarrierFreq ::= SEQUENCE
SRSASN_CODE sl_pppr_dest_carrier_freq_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dest_info_list_r15_present, 1));
  HANDLE_CODE(bref.pack(allowed_carrier_freq_list_r15_present, 1));

  if (dest_info_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, dest_info_list_r15, 1, 16));
  }
  if (allowed_carrier_freq_list_r15_present) {
    HANDLE_CODE(allowed_carrier_freq_list_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pppr_dest_carrier_freq_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dest_info_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(allowed_carrier_freq_list_r15_present, 1));

  if (dest_info_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(dest_info_list_r15, bref, 1, 16));
  }
  if (allowed_carrier_freq_list_r15_present) {
    HANDLE_CODE(allowed_carrier_freq_list_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_pppr_dest_carrier_freq_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dest_info_list_r15_present) {
    j.start_array("destinationInfoList-r15");
    for (const auto& e1 : dest_info_list_r15) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (allowed_carrier_freq_list_r15_present) {
    j.write_fieldname("allowedCarrierFreqList-r15");
    allowed_carrier_freq_list_r15.to_json(j);
  }
  j.end_obj();
}

// SL-PeriodComm-r12 ::= ENUMERATED
const char* sl_period_comm_r12_opts::to_string() const
{
  static const char* options[] = {"sf40",
                                  "sf60",
                                  "sf70",
                                  "sf80",
                                  "sf120",
                                  "sf140",
                                  "sf160",
                                  "sf240",
                                  "sf280",
                                  "sf320",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare"};
  return convert_enum_idx(options, 16, value, "sl_period_comm_r12_e");
}
uint16_t sl_period_comm_r12_opts::to_number() const
{
  static const uint16_t options[] = {40, 60, 70, 80, 120, 140, 160, 240, 280, 320};
  return map_enum_number(options, 10, value, "sl_period_comm_r12_e");
}

// SL-ResourcesInterFreq-r13 ::= SEQUENCE
SRSASN_CODE sl_res_inter_freq_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(disc_rx_res_inter_freq_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_tx_res_inter_freq_r13_present, 1));

  if (disc_rx_res_inter_freq_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, disc_rx_res_inter_freq_r13, 1, 16));
  }
  if (disc_tx_res_inter_freq_r13_present) {
    HANDLE_CODE(disc_tx_res_inter_freq_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_res_inter_freq_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(disc_rx_res_inter_freq_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_tx_res_inter_freq_r13_present, 1));

  if (disc_rx_res_inter_freq_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(disc_rx_res_inter_freq_r13, bref, 1, 16));
  }
  if (disc_tx_res_inter_freq_r13_present) {
    HANDLE_CODE(disc_tx_res_inter_freq_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_res_inter_freq_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (disc_rx_res_inter_freq_r13_present) {
    j.start_array("discRxResourcesInterFreq-r13");
    for (const auto& e1 : disc_rx_res_inter_freq_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (disc_tx_res_inter_freq_r13_present) {
    j.write_fieldname("discTxResourcesInterFreq-r13");
    disc_tx_res_inter_freq_r13.to_json(j);
  }
  j.end_obj();
}

// SL-SyncConfig-r12 ::= SEQUENCE
SRSASN_CODE sl_sync_cfg_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(tx_params_r12_present, 1));
  HANDLE_CODE(bref.pack(rx_params_ncell_r12_present, 1));

  HANDLE_CODE(sync_cp_len_r12.pack(bref));
  HANDLE_CODE(pack_integer(bref, sync_offset_ind_r12, (uint8_t)0u, (uint8_t)39u));
  HANDLE_CODE(pack_integer(bref, slssid_r12, (uint8_t)0u, (uint8_t)167u));
  if (tx_params_r12_present) {
    HANDLE_CODE(bref.pack(tx_params_r12.sync_info_reserved_r12_present, 1));
    HANDLE_CODE(tx_params_r12.sync_tx_params_r12.pack(bref));
    HANDLE_CODE(pack_integer(bref, tx_params_r12.sync_tx_thresh_ic_r12, (uint8_t)0u, (uint8_t)13u));
    if (tx_params_r12.sync_info_reserved_r12_present) {
      HANDLE_CODE(tx_params_r12.sync_info_reserved_r12.pack(bref));
    }
  }
  if (rx_params_ncell_r12_present) {
    HANDLE_CODE(pack_integer(bref, rx_params_ncell_r12.pci_r12, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(rx_params_ncell_r12.disc_sync_win_r12.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sync_tx_periodic_r13_present;
    group_flags[1] |= sync_offset_ind_v1430_present;
    group_flags[1] |= gnss_sync_r14_present;
    group_flags[2] |= sync_offset_ind2_r14_present;
    group_flags[2] |= sync_offset_ind3_r14_present;
    group_flags[3] |= slss_tx_disabled_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sync_tx_periodic_r13_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sync_offset_ind_v1430_present, 1));
      HANDLE_CODE(bref.pack(gnss_sync_r14_present, 1));
      if (sync_offset_ind_v1430_present) {
        HANDLE_CODE(pack_integer(bref, sync_offset_ind_v1430, (uint8_t)40u, (uint8_t)159u));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sync_offset_ind2_r14_present, 1));
      HANDLE_CODE(bref.pack(sync_offset_ind3_r14_present, 1));
      if (sync_offset_ind2_r14_present) {
        HANDLE_CODE(pack_integer(bref, sync_offset_ind2_r14, (uint8_t)0u, (uint8_t)159u));
      }
      if (sync_offset_ind3_r14_present) {
        HANDLE_CODE(pack_integer(bref, sync_offset_ind3_r14, (uint8_t)0u, (uint8_t)159u));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(slss_tx_disabled_r15_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_sync_cfg_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(tx_params_r12_present, 1));
  HANDLE_CODE(bref.unpack(rx_params_ncell_r12_present, 1));

  HANDLE_CODE(sync_cp_len_r12.unpack(bref));
  HANDLE_CODE(unpack_integer(sync_offset_ind_r12, bref, (uint8_t)0u, (uint8_t)39u));
  HANDLE_CODE(unpack_integer(slssid_r12, bref, (uint8_t)0u, (uint8_t)167u));
  if (tx_params_r12_present) {
    HANDLE_CODE(bref.unpack(tx_params_r12.sync_info_reserved_r12_present, 1));
    HANDLE_CODE(tx_params_r12.sync_tx_params_r12.unpack(bref));
    HANDLE_CODE(unpack_integer(tx_params_r12.sync_tx_thresh_ic_r12, bref, (uint8_t)0u, (uint8_t)13u));
    if (tx_params_r12.sync_info_reserved_r12_present) {
      HANDLE_CODE(tx_params_r12.sync_info_reserved_r12.unpack(bref));
    }
  }
  if (rx_params_ncell_r12_present) {
    HANDLE_CODE(unpack_integer(rx_params_ncell_r12.pci_r12, bref, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(rx_params_ncell_r12.disc_sync_win_r12.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sync_tx_periodic_r13_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sync_offset_ind_v1430_present, 1));
      HANDLE_CODE(bref.unpack(gnss_sync_r14_present, 1));
      if (sync_offset_ind_v1430_present) {
        HANDLE_CODE(unpack_integer(sync_offset_ind_v1430, bref, (uint8_t)40u, (uint8_t)159u));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sync_offset_ind2_r14_present, 1));
      HANDLE_CODE(bref.unpack(sync_offset_ind3_r14_present, 1));
      if (sync_offset_ind2_r14_present) {
        HANDLE_CODE(unpack_integer(sync_offset_ind2_r14, bref, (uint8_t)0u, (uint8_t)159u));
      }
      if (sync_offset_ind3_r14_present) {
        HANDLE_CODE(unpack_integer(sync_offset_ind3_r14, bref, (uint8_t)0u, (uint8_t)159u));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(slss_tx_disabled_r15_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void sl_sync_cfg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("syncCP-Len-r12", sync_cp_len_r12.to_string());
  j.write_int("syncOffsetIndicator-r12", sync_offset_ind_r12);
  j.write_int("slssid-r12", slssid_r12);
  if (tx_params_r12_present) {
    j.write_fieldname("txParameters-r12");
    j.start_obj();
    j.write_fieldname("syncTxParameters-r12");
    tx_params_r12.sync_tx_params_r12.to_json(j);
    j.write_int("syncTxThreshIC-r12", tx_params_r12.sync_tx_thresh_ic_r12);
    if (tx_params_r12.sync_info_reserved_r12_present) {
      j.write_str("syncInfoReserved-r12", tx_params_r12.sync_info_reserved_r12.to_string());
    }
    j.end_obj();
  }
  if (rx_params_ncell_r12_present) {
    j.write_fieldname("rxParamsNCell-r12");
    j.start_obj();
    j.write_int("physCellId-r12", rx_params_ncell_r12.pci_r12);
    j.write_str("discSyncWindow-r12", rx_params_ncell_r12.disc_sync_win_r12.to_string());
    j.end_obj();
  }
  if (ext) {
    if (sync_tx_periodic_r13_present) {
      j.write_str("syncTxPeriodic-r13", "true");
    }
    if (sync_offset_ind_v1430_present) {
      j.write_int("syncOffsetIndicator-v1430", sync_offset_ind_v1430);
    }
    if (gnss_sync_r14_present) {
      j.write_str("gnss-Sync-r14", "true");
    }
    if (sync_offset_ind2_r14_present) {
      j.write_int("syncOffsetIndicator2-r14", sync_offset_ind2_r14);
    }
    if (sync_offset_ind3_r14_present) {
      j.write_int("syncOffsetIndicator3-r14", sync_offset_ind3_r14);
    }
    if (slss_tx_disabled_r15_present) {
      j.write_str("slss-TxDisabled-r15", "true");
    }
  }
  j.end_obj();
}

const char* sl_sync_cfg_r12_s::rx_params_ncell_r12_s_::disc_sync_win_r12_opts::to_string() const
{
  static const char* options[] = {"w1", "w2"};
  return convert_enum_idx(options, 2, value, "sl_sync_cfg_r12_s::rx_params_ncell_r12_s_::disc_sync_win_r12_e_");
}
uint8_t sl_sync_cfg_r12_s::rx_params_ncell_r12_s_::disc_sync_win_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "sl_sync_cfg_r12_s::rx_params_ncell_r12_s_::disc_sync_win_r12_e_");
}

// WLAN-Identifiers-r12 ::= SEQUENCE
SRSASN_CODE wlan_ids_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ssid_r12_present, 1));
  HANDLE_CODE(bref.pack(bssid_r12_present, 1));
  HANDLE_CODE(bref.pack(hessid_r12_present, 1));

  if (ssid_r12_present) {
    HANDLE_CODE(ssid_r12.pack(bref));
  }
  if (bssid_r12_present) {
    HANDLE_CODE(bssid_r12.pack(bref));
  }
  if (hessid_r12_present) {
    HANDLE_CODE(hessid_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_ids_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ssid_r12_present, 1));
  HANDLE_CODE(bref.unpack(bssid_r12_present, 1));
  HANDLE_CODE(bref.unpack(hessid_r12_present, 1));

  if (ssid_r12_present) {
    HANDLE_CODE(ssid_r12.unpack(bref));
  }
  if (bssid_r12_present) {
    HANDLE_CODE(bssid_r12.unpack(bref));
  }
  if (hessid_r12_present) {
    HANDLE_CODE(hessid_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void wlan_ids_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssid_r12_present) {
    j.write_str("ssid-r12", ssid_r12.to_string());
  }
  if (bssid_r12_present) {
    j.write_str("bssid-r12", bssid_r12.to_string());
  }
  if (hessid_r12_present) {
    j.write_str("hessid-r12", hessid_r12.to_string());
  }
  j.end_obj();
}
bool wlan_ids_r12_s::operator==(const wlan_ids_r12_s& other) const
{
  return ext == other.ext and ssid_r12_present == other.ssid_r12_present and
         (not ssid_r12_present or ssid_r12 == other.ssid_r12) and bssid_r12_present == other.bssid_r12_present and
         (not bssid_r12_present or bssid_r12 == other.bssid_r12) and hessid_r12_present == other.hessid_r12_present and
         (not hessid_r12_present or hessid_r12 == other.hessid_r12);
}

// WLAN-backhaulRate-r12 ::= ENUMERATED
const char* wlan_backhaul_rate_r12_opts::to_string() const
{
  static const char* options[] = {
      "r0",        "r4",        "r8",         "r16",        "r32",        "r64",         "r128",        "r256",
      "r512",      "r1024",     "r2048",      "r4096",      "r8192",      "r16384",      "r32768",      "r65536",
      "r131072",   "r262144",   "r524288",    "r1048576",   "r2097152",   "r4194304",    "r8388608",    "r16777216",
      "r33554432", "r67108864", "r134217728", "r268435456", "r536870912", "r1073741824", "r2147483648", "r4294967296"};
  return convert_enum_idx(options, 32, value, "wlan_backhaul_rate_r12_e");
}
uint64_t wlan_backhaul_rate_r12_opts::to_number() const
{
  static const uint64_t options[] = {0,         4,          8,          16,        32,       64,        128,
                                     256,       512,        1024,       2048,      4096,     8192,      16384,
                                     32768,     65536,      131072,     262144,    524288,   1048576,   2097152,
                                     4194304,   8388608,    16777216,   33554432,  67108864, 134217728, 268435456,
                                     536870912, 1073741824, 2147483648, 4294967296};
  return map_enum_number(options, 32, value, "wlan_backhaul_rate_r12_e");
}

// ReferenceTime-r15 ::= SEQUENCE
SRSASN_CODE ref_time_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ref_days_r15, (uint32_t)0u, (uint32_t)72999u));
  HANDLE_CODE(pack_integer(bref, ref_seconds_r15, (uint32_t)0u, (uint32_t)86399u));
  HANDLE_CODE(pack_integer(bref, ref_milli_seconds_r15, (uint16_t)0u, (uint16_t)999u));
  HANDLE_CODE(pack_integer(bref, ref_quarter_micro_seconds_r15, (uint16_t)0u, (uint16_t)3999u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ref_time_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ref_days_r15, bref, (uint32_t)0u, (uint32_t)72999u));
  HANDLE_CODE(unpack_integer(ref_seconds_r15, bref, (uint32_t)0u, (uint32_t)86399u));
  HANDLE_CODE(unpack_integer(ref_milli_seconds_r15, bref, (uint16_t)0u, (uint16_t)999u));
  HANDLE_CODE(unpack_integer(ref_quarter_micro_seconds_r15, bref, (uint16_t)0u, (uint16_t)3999u));

  return SRSASN_SUCCESS;
}
void ref_time_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("refDays-r15", ref_days_r15);
  j.write_int("refSeconds-r15", ref_seconds_r15);
  j.write_int("refMilliSeconds-r15", ref_milli_seconds_r15);
  j.write_int("refQuarterMicroSeconds-r15", ref_quarter_micro_seconds_r15);
  j.end_obj();
}

// SL-CBR-CommonTxConfigList-r14 ::= SEQUENCE
SRSASN_CODE sl_cbr_common_tx_cfg_list_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref,
                              cbr_range_common_cfg_list_r14,
                              1,
                              4,
                              SeqOfPacker<integer_packer<uint8_t> >(1, 16, integer_packer<uint8_t>(0, 100))));
  HANDLE_CODE(pack_dyn_seq_of(bref, sl_cbr_pssch_tx_cfg_list_r14, 1, 64));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_cbr_common_tx_cfg_list_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(cbr_range_common_cfg_list_r14,
                                bref,
                                1,
                                4,
                                SeqOfPacker<integer_packer<uint8_t> >(1, 16, integer_packer<uint8_t>(0, 100))));
  HANDLE_CODE(unpack_dyn_seq_of(sl_cbr_pssch_tx_cfg_list_r14, bref, 1, 64));

  return SRSASN_SUCCESS;
}
void sl_cbr_common_tx_cfg_list_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("cbr-RangeCommonConfigList-r14");
  for (const auto& e1 : cbr_range_common_cfg_list_r14) {
    j.start_array();
    for (const auto& e2 : e1) {
      j.write_int(e2);
    }
    j.end_array();
  }
  j.end_array();
  j.start_array("sl-CBR-PSSCH-TxConfigList-r14");
  for (const auto& e1 : sl_cbr_pssch_tx_cfg_list_r14) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// SL-CommResourcePool-r12 ::= SEQUENCE
SRSASN_CODE sl_comm_res_pool_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ue_sel_res_cfg_r12_present, 1));
  HANDLE_CODE(bref.pack(rx_params_ncell_r12_present, 1));
  HANDLE_CODE(bref.pack(tx_params_r12_present, 1));

  HANDLE_CODE(sc_cp_len_r12.pack(bref));
  HANDLE_CODE(sc_period_r12.pack(bref));
  HANDLE_CODE(sc_tf_res_cfg_r12.pack(bref));
  HANDLE_CODE(data_cp_len_r12.pack(bref));
  HANDLE_CODE(data_hop_cfg_r12.pack(bref));
  if (ue_sel_res_cfg_r12_present) {
    HANDLE_CODE(bref.pack(ue_sel_res_cfg_r12.trpt_subset_r12_present, 1));
    HANDLE_CODE(ue_sel_res_cfg_r12.data_tf_res_cfg_r12.pack(bref));
    if (ue_sel_res_cfg_r12.trpt_subset_r12_present) {
      HANDLE_CODE(ue_sel_res_cfg_r12.trpt_subset_r12.pack(bref));
    }
  }
  if (rx_params_ncell_r12_present) {
    HANDLE_CODE(bref.pack(rx_params_ncell_r12.tdd_cfg_r12_present, 1));
    if (rx_params_ncell_r12.tdd_cfg_r12_present) {
      HANDLE_CODE(rx_params_ncell_r12.tdd_cfg_r12.pack(bref));
    }
    HANDLE_CODE(pack_integer(bref, rx_params_ncell_r12.sync_cfg_idx_r12, (uint8_t)0u, (uint8_t)15u));
  }
  if (tx_params_r12_present) {
    HANDLE_CODE(tx_params_r12.sc_tx_params_r12.pack(bref));
    HANDLE_CODE(tx_params_r12.data_tx_params_r12.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= prio_list_r13.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(prio_list_r13.is_present(), 1));
      if (prio_list_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *prio_list_r13, 1, 8, integer_packer<uint8_t>(1, 8)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_comm_res_pool_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ue_sel_res_cfg_r12_present, 1));
  HANDLE_CODE(bref.unpack(rx_params_ncell_r12_present, 1));
  HANDLE_CODE(bref.unpack(tx_params_r12_present, 1));

  HANDLE_CODE(sc_cp_len_r12.unpack(bref));
  HANDLE_CODE(sc_period_r12.unpack(bref));
  HANDLE_CODE(sc_tf_res_cfg_r12.unpack(bref));
  HANDLE_CODE(data_cp_len_r12.unpack(bref));
  HANDLE_CODE(data_hop_cfg_r12.unpack(bref));
  if (ue_sel_res_cfg_r12_present) {
    HANDLE_CODE(bref.unpack(ue_sel_res_cfg_r12.trpt_subset_r12_present, 1));
    HANDLE_CODE(ue_sel_res_cfg_r12.data_tf_res_cfg_r12.unpack(bref));
    if (ue_sel_res_cfg_r12.trpt_subset_r12_present) {
      HANDLE_CODE(ue_sel_res_cfg_r12.trpt_subset_r12.unpack(bref));
    }
  }
  if (rx_params_ncell_r12_present) {
    HANDLE_CODE(bref.unpack(rx_params_ncell_r12.tdd_cfg_r12_present, 1));
    if (rx_params_ncell_r12.tdd_cfg_r12_present) {
      HANDLE_CODE(rx_params_ncell_r12.tdd_cfg_r12.unpack(bref));
    }
    HANDLE_CODE(unpack_integer(rx_params_ncell_r12.sync_cfg_idx_r12, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (tx_params_r12_present) {
    HANDLE_CODE(tx_params_r12.sc_tx_params_r12.unpack(bref));
    HANDLE_CODE(tx_params_r12.data_tx_params_r12.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool prio_list_r13_present;
      HANDLE_CODE(bref.unpack(prio_list_r13_present, 1));
      prio_list_r13.set_present(prio_list_r13_present);
      if (prio_list_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*prio_list_r13, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_comm_res_pool_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sc-CP-Len-r12", sc_cp_len_r12.to_string());
  j.write_str("sc-Period-r12", sc_period_r12.to_string());
  j.write_fieldname("sc-TF-ResourceConfig-r12");
  sc_tf_res_cfg_r12.to_json(j);
  j.write_str("data-CP-Len-r12", data_cp_len_r12.to_string());
  j.write_fieldname("dataHoppingConfig-r12");
  data_hop_cfg_r12.to_json(j);
  if (ue_sel_res_cfg_r12_present) {
    j.write_fieldname("ue-SelectedResourceConfig-r12");
    j.start_obj();
    j.write_fieldname("data-TF-ResourceConfig-r12");
    ue_sel_res_cfg_r12.data_tf_res_cfg_r12.to_json(j);
    if (ue_sel_res_cfg_r12.trpt_subset_r12_present) {
      j.write_str("trpt-Subset-r12", ue_sel_res_cfg_r12.trpt_subset_r12.to_string());
    }
    j.end_obj();
  }
  if (rx_params_ncell_r12_present) {
    j.write_fieldname("rxParametersNCell-r12");
    j.start_obj();
    if (rx_params_ncell_r12.tdd_cfg_r12_present) {
      j.write_fieldname("tdd-Config-r12");
      rx_params_ncell_r12.tdd_cfg_r12.to_json(j);
    }
    j.write_int("syncConfigIndex-r12", rx_params_ncell_r12.sync_cfg_idx_r12);
    j.end_obj();
  }
  if (tx_params_r12_present) {
    j.write_fieldname("txParameters-r12");
    j.start_obj();
    j.write_fieldname("sc-TxParameters-r12");
    tx_params_r12.sc_tx_params_r12.to_json(j);
    j.write_fieldname("dataTxParameters-r12");
    tx_params_r12.data_tx_params_r12.to_json(j);
    j.end_obj();
  }
  if (ext) {
    if (prio_list_r13.is_present()) {
      j.start_array("priorityList-r13");
      for (const auto& e1 : *prio_list_r13) {
        j.write_int(e1);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// WLAN-OffloadConfig-r12 ::= SEQUENCE
SRSASN_CODE wlan_offload_cfg_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(thres_rsrp_r12_present, 1));
  HANDLE_CODE(bref.pack(thres_rsrq_r12_present, 1));
  HANDLE_CODE(bref.pack(thres_rsrq_on_all_symbols_with_wb_r12_present, 1));
  HANDLE_CODE(bref.pack(thres_rsrq_on_all_symbols_r12_present, 1));
  HANDLE_CODE(bref.pack(thres_rsrq_wb_r12_present, 1));
  HANDLE_CODE(bref.pack(thres_ch_utilization_r12_present, 1));
  HANDLE_CODE(bref.pack(thres_backhaul_bw_r12_present, 1));
  HANDLE_CODE(bref.pack(thres_wlan_rssi_r12_present, 1));
  HANDLE_CODE(bref.pack(offload_pref_ind_r12_present, 1));
  HANDLE_CODE(bref.pack(t_steering_wlan_r12_present, 1));

  if (thres_rsrp_r12_present) {
    HANDLE_CODE(pack_integer(bref, thres_rsrp_r12.thres_rsrp_low_r12, (uint8_t)0u, (uint8_t)97u));
    HANDLE_CODE(pack_integer(bref, thres_rsrp_r12.thres_rsrp_high_r12, (uint8_t)0u, (uint8_t)97u));
  }
  if (thres_rsrq_r12_present) {
    HANDLE_CODE(pack_integer(bref, thres_rsrq_r12.thres_rsrq_low_r12, (uint8_t)0u, (uint8_t)34u));
    HANDLE_CODE(pack_integer(bref, thres_rsrq_r12.thres_rsrq_high_r12, (uint8_t)0u, (uint8_t)34u));
  }
  if (thres_rsrq_on_all_symbols_with_wb_r12_present) {
    HANDLE_CODE(pack_integer(bref,
                             thres_rsrq_on_all_symbols_with_wb_r12.thres_rsrq_on_all_symbols_with_wb_low_r12,
                             (uint8_t)0u,
                             (uint8_t)34u));
    HANDLE_CODE(pack_integer(bref,
                             thres_rsrq_on_all_symbols_with_wb_r12.thres_rsrq_on_all_symbols_with_wb_high_r12,
                             (uint8_t)0u,
                             (uint8_t)34u));
  }
  if (thres_rsrq_on_all_symbols_r12_present) {
    HANDLE_CODE(
        pack_integer(bref, thres_rsrq_on_all_symbols_r12.thres_rsrq_on_all_symbols_low_r12, (uint8_t)0u, (uint8_t)34u));
    HANDLE_CODE(pack_integer(
        bref, thres_rsrq_on_all_symbols_r12.thres_rsrq_on_all_symbols_high_r12, (uint8_t)0u, (uint8_t)34u));
  }
  if (thres_rsrq_wb_r12_present) {
    HANDLE_CODE(pack_integer(bref, thres_rsrq_wb_r12.thres_rsrq_wb_low_r12, (uint8_t)0u, (uint8_t)34u));
    HANDLE_CODE(pack_integer(bref, thres_rsrq_wb_r12.thres_rsrq_wb_high_r12, (uint8_t)0u, (uint8_t)34u));
  }
  if (thres_ch_utilization_r12_present) {
    HANDLE_CODE(
        pack_integer(bref, thres_ch_utilization_r12.thres_ch_utilization_low_r12, (uint16_t)0u, (uint16_t)255u));
    HANDLE_CODE(
        pack_integer(bref, thres_ch_utilization_r12.thres_ch_utilization_high_r12, (uint16_t)0u, (uint16_t)255u));
  }
  if (thres_backhaul_bw_r12_present) {
    HANDLE_CODE(thres_backhaul_bw_r12.thres_backhaul_dl_bw_low_r12.pack(bref));
    HANDLE_CODE(thres_backhaul_bw_r12.thres_backhaul_dl_bw_high_r12.pack(bref));
    HANDLE_CODE(thres_backhaul_bw_r12.thres_backhaul_ul_bw_low_r12.pack(bref));
    HANDLE_CODE(thres_backhaul_bw_r12.thres_backhaul_ul_bw_high_r12.pack(bref));
  }
  if (thres_wlan_rssi_r12_present) {
    HANDLE_CODE(pack_integer(bref, thres_wlan_rssi_r12.thres_wlan_rssi_low_r12, (uint16_t)0u, (uint16_t)255u));
    HANDLE_CODE(pack_integer(bref, thres_wlan_rssi_r12.thres_wlan_rssi_high_r12, (uint16_t)0u, (uint16_t)255u));
  }
  if (offload_pref_ind_r12_present) {
    HANDLE_CODE(offload_pref_ind_r12.pack(bref));
  }
  if (t_steering_wlan_r12_present) {
    HANDLE_CODE(pack_integer(bref, t_steering_wlan_r12, (uint8_t)0u, (uint8_t)7u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_offload_cfg_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(thres_rsrp_r12_present, 1));
  HANDLE_CODE(bref.unpack(thres_rsrq_r12_present, 1));
  HANDLE_CODE(bref.unpack(thres_rsrq_on_all_symbols_with_wb_r12_present, 1));
  HANDLE_CODE(bref.unpack(thres_rsrq_on_all_symbols_r12_present, 1));
  HANDLE_CODE(bref.unpack(thres_rsrq_wb_r12_present, 1));
  HANDLE_CODE(bref.unpack(thres_ch_utilization_r12_present, 1));
  HANDLE_CODE(bref.unpack(thres_backhaul_bw_r12_present, 1));
  HANDLE_CODE(bref.unpack(thres_wlan_rssi_r12_present, 1));
  HANDLE_CODE(bref.unpack(offload_pref_ind_r12_present, 1));
  HANDLE_CODE(bref.unpack(t_steering_wlan_r12_present, 1));

  if (thres_rsrp_r12_present) {
    HANDLE_CODE(unpack_integer(thres_rsrp_r12.thres_rsrp_low_r12, bref, (uint8_t)0u, (uint8_t)97u));
    HANDLE_CODE(unpack_integer(thres_rsrp_r12.thres_rsrp_high_r12, bref, (uint8_t)0u, (uint8_t)97u));
  }
  if (thres_rsrq_r12_present) {
    HANDLE_CODE(unpack_integer(thres_rsrq_r12.thres_rsrq_low_r12, bref, (uint8_t)0u, (uint8_t)34u));
    HANDLE_CODE(unpack_integer(thres_rsrq_r12.thres_rsrq_high_r12, bref, (uint8_t)0u, (uint8_t)34u));
  }
  if (thres_rsrq_on_all_symbols_with_wb_r12_present) {
    HANDLE_CODE(unpack_integer(thres_rsrq_on_all_symbols_with_wb_r12.thres_rsrq_on_all_symbols_with_wb_low_r12,
                               bref,
                               (uint8_t)0u,
                               (uint8_t)34u));
    HANDLE_CODE(unpack_integer(thres_rsrq_on_all_symbols_with_wb_r12.thres_rsrq_on_all_symbols_with_wb_high_r12,
                               bref,
                               (uint8_t)0u,
                               (uint8_t)34u));
  }
  if (thres_rsrq_on_all_symbols_r12_present) {
    HANDLE_CODE(unpack_integer(
        thres_rsrq_on_all_symbols_r12.thres_rsrq_on_all_symbols_low_r12, bref, (uint8_t)0u, (uint8_t)34u));
    HANDLE_CODE(unpack_integer(
        thres_rsrq_on_all_symbols_r12.thres_rsrq_on_all_symbols_high_r12, bref, (uint8_t)0u, (uint8_t)34u));
  }
  if (thres_rsrq_wb_r12_present) {
    HANDLE_CODE(unpack_integer(thres_rsrq_wb_r12.thres_rsrq_wb_low_r12, bref, (uint8_t)0u, (uint8_t)34u));
    HANDLE_CODE(unpack_integer(thres_rsrq_wb_r12.thres_rsrq_wb_high_r12, bref, (uint8_t)0u, (uint8_t)34u));
  }
  if (thres_ch_utilization_r12_present) {
    HANDLE_CODE(
        unpack_integer(thres_ch_utilization_r12.thres_ch_utilization_low_r12, bref, (uint16_t)0u, (uint16_t)255u));
    HANDLE_CODE(
        unpack_integer(thres_ch_utilization_r12.thres_ch_utilization_high_r12, bref, (uint16_t)0u, (uint16_t)255u));
  }
  if (thres_backhaul_bw_r12_present) {
    HANDLE_CODE(thres_backhaul_bw_r12.thres_backhaul_dl_bw_low_r12.unpack(bref));
    HANDLE_CODE(thres_backhaul_bw_r12.thres_backhaul_dl_bw_high_r12.unpack(bref));
    HANDLE_CODE(thres_backhaul_bw_r12.thres_backhaul_ul_bw_low_r12.unpack(bref));
    HANDLE_CODE(thres_backhaul_bw_r12.thres_backhaul_ul_bw_high_r12.unpack(bref));
  }
  if (thres_wlan_rssi_r12_present) {
    HANDLE_CODE(unpack_integer(thres_wlan_rssi_r12.thres_wlan_rssi_low_r12, bref, (uint16_t)0u, (uint16_t)255u));
    HANDLE_CODE(unpack_integer(thres_wlan_rssi_r12.thres_wlan_rssi_high_r12, bref, (uint16_t)0u, (uint16_t)255u));
  }
  if (offload_pref_ind_r12_present) {
    HANDLE_CODE(offload_pref_ind_r12.unpack(bref));
  }
  if (t_steering_wlan_r12_present) {
    HANDLE_CODE(unpack_integer(t_steering_wlan_r12, bref, (uint8_t)0u, (uint8_t)7u));
  }

  return SRSASN_SUCCESS;
}
void wlan_offload_cfg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (thres_rsrp_r12_present) {
    j.write_fieldname("thresholdRSRP-r12");
    j.start_obj();
    j.write_int("thresholdRSRP-Low-r12", thres_rsrp_r12.thres_rsrp_low_r12);
    j.write_int("thresholdRSRP-High-r12", thres_rsrp_r12.thres_rsrp_high_r12);
    j.end_obj();
  }
  if (thres_rsrq_r12_present) {
    j.write_fieldname("thresholdRSRQ-r12");
    j.start_obj();
    j.write_int("thresholdRSRQ-Low-r12", thres_rsrq_r12.thres_rsrq_low_r12);
    j.write_int("thresholdRSRQ-High-r12", thres_rsrq_r12.thres_rsrq_high_r12);
    j.end_obj();
  }
  if (thres_rsrq_on_all_symbols_with_wb_r12_present) {
    j.write_fieldname("thresholdRSRQ-OnAllSymbolsWithWB-r12");
    j.start_obj();
    j.write_int("thresholdRSRQ-OnAllSymbolsWithWB-Low-r12",
                thres_rsrq_on_all_symbols_with_wb_r12.thres_rsrq_on_all_symbols_with_wb_low_r12);
    j.write_int("thresholdRSRQ-OnAllSymbolsWithWB-High-r12",
                thres_rsrq_on_all_symbols_with_wb_r12.thres_rsrq_on_all_symbols_with_wb_high_r12);
    j.end_obj();
  }
  if (thres_rsrq_on_all_symbols_r12_present) {
    j.write_fieldname("thresholdRSRQ-OnAllSymbols-r12");
    j.start_obj();
    j.write_int("thresholdRSRQ-OnAllSymbolsLow-r12", thres_rsrq_on_all_symbols_r12.thres_rsrq_on_all_symbols_low_r12);
    j.write_int("thresholdRSRQ-OnAllSymbolsHigh-r12", thres_rsrq_on_all_symbols_r12.thres_rsrq_on_all_symbols_high_r12);
    j.end_obj();
  }
  if (thres_rsrq_wb_r12_present) {
    j.write_fieldname("thresholdRSRQ-WB-r12");
    j.start_obj();
    j.write_int("thresholdRSRQ-WB-Low-r12", thres_rsrq_wb_r12.thres_rsrq_wb_low_r12);
    j.write_int("thresholdRSRQ-WB-High-r12", thres_rsrq_wb_r12.thres_rsrq_wb_high_r12);
    j.end_obj();
  }
  if (thres_ch_utilization_r12_present) {
    j.write_fieldname("thresholdChannelUtilization-r12");
    j.start_obj();
    j.write_int("thresholdChannelUtilizationLow-r12", thres_ch_utilization_r12.thres_ch_utilization_low_r12);
    j.write_int("thresholdChannelUtilizationHigh-r12", thres_ch_utilization_r12.thres_ch_utilization_high_r12);
    j.end_obj();
  }
  if (thres_backhaul_bw_r12_present) {
    j.write_fieldname("thresholdBackhaul-Bandwidth-r12");
    j.start_obj();
    j.write_str("thresholdBackhaulDL-BandwidthLow-r12", thres_backhaul_bw_r12.thres_backhaul_dl_bw_low_r12.to_string());
    j.write_str("thresholdBackhaulDL-BandwidthHigh-r12",
                thres_backhaul_bw_r12.thres_backhaul_dl_bw_high_r12.to_string());
    j.write_str("thresholdBackhaulUL-BandwidthLow-r12", thres_backhaul_bw_r12.thres_backhaul_ul_bw_low_r12.to_string());
    j.write_str("thresholdBackhaulUL-BandwidthHigh-r12",
                thres_backhaul_bw_r12.thres_backhaul_ul_bw_high_r12.to_string());
    j.end_obj();
  }
  if (thres_wlan_rssi_r12_present) {
    j.write_fieldname("thresholdWLAN-RSSI-r12");
    j.start_obj();
    j.write_int("thresholdWLAN-RSSI-Low-r12", thres_wlan_rssi_r12.thres_wlan_rssi_low_r12);
    j.write_int("thresholdWLAN-RSSI-High-r12", thres_wlan_rssi_r12.thres_wlan_rssi_high_r12);
    j.end_obj();
  }
  if (offload_pref_ind_r12_present) {
    j.write_str("offloadPreferenceIndicator-r12", offload_pref_ind_r12.to_string());
  }
  if (t_steering_wlan_r12_present) {
    j.write_int("t-SteeringWLAN-r12", t_steering_wlan_r12);
  }
  j.end_obj();
}

// SL-V2X-ConfigCommon-r14 ::= SEQUENCE
SRSASN_CODE sl_v2x_cfg_common_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(v2x_comm_rx_pool_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_comm_tx_pool_normal_common_r14_present, 1));
  HANDLE_CODE(bref.pack(p2x_comm_tx_pool_normal_common_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_comm_tx_pool_exceptional_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_sync_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_inter_freq_info_list_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_res_sel_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(zone_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(type_tx_sync_r14_present, 1));
  HANDLE_CODE(bref.pack(thres_sl_tx_prioritization_r14_present, 1));
  HANDLE_CODE(bref.pack(anchor_carrier_freq_list_r14_present, 1));
  HANDLE_CODE(bref.pack(offset_dfn_r14_present, 1));
  HANDLE_CODE(bref.pack(cbr_common_tx_cfg_list_r14_present, 1));

  if (v2x_comm_rx_pool_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_comm_rx_pool_r14, 1, 16));
  }
  if (v2x_comm_tx_pool_normal_common_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_comm_tx_pool_normal_common_r14, 1, 8));
  }
  if (p2x_comm_tx_pool_normal_common_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, p2x_comm_tx_pool_normal_common_r14, 1, 8));
  }
  if (v2x_comm_tx_pool_exceptional_r14_present) {
    HANDLE_CODE(v2x_comm_tx_pool_exceptional_r14.pack(bref));
  }
  if (v2x_sync_cfg_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_sync_cfg_r14, 1, 16));
  }
  if (v2x_inter_freq_info_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_inter_freq_info_list_r14, 0, 7));
  }
  if (v2x_res_sel_cfg_r14_present) {
    HANDLE_CODE(v2x_res_sel_cfg_r14.pack(bref));
  }
  if (zone_cfg_r14_present) {
    HANDLE_CODE(zone_cfg_r14.pack(bref));
  }
  if (type_tx_sync_r14_present) {
    HANDLE_CODE(type_tx_sync_r14.pack(bref));
  }
  if (thres_sl_tx_prioritization_r14_present) {
    HANDLE_CODE(pack_integer(bref, thres_sl_tx_prioritization_r14, (uint8_t)1u, (uint8_t)8u));
  }
  if (anchor_carrier_freq_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, anchor_carrier_freq_list_r14, 1, 8, integer_packer<uint32_t>(0, 262143)));
  }
  if (offset_dfn_r14_present) {
    HANDLE_CODE(pack_integer(bref, offset_dfn_r14, (uint16_t)0u, (uint16_t)1000u));
  }
  if (cbr_common_tx_cfg_list_r14_present) {
    HANDLE_CODE(cbr_common_tx_cfg_list_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_v2x_cfg_common_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(v2x_comm_rx_pool_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_comm_tx_pool_normal_common_r14_present, 1));
  HANDLE_CODE(bref.unpack(p2x_comm_tx_pool_normal_common_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_comm_tx_pool_exceptional_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_sync_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_inter_freq_info_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_res_sel_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(zone_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(type_tx_sync_r14_present, 1));
  HANDLE_CODE(bref.unpack(thres_sl_tx_prioritization_r14_present, 1));
  HANDLE_CODE(bref.unpack(anchor_carrier_freq_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(offset_dfn_r14_present, 1));
  HANDLE_CODE(bref.unpack(cbr_common_tx_cfg_list_r14_present, 1));

  if (v2x_comm_rx_pool_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_comm_rx_pool_r14, bref, 1, 16));
  }
  if (v2x_comm_tx_pool_normal_common_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_comm_tx_pool_normal_common_r14, bref, 1, 8));
  }
  if (p2x_comm_tx_pool_normal_common_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(p2x_comm_tx_pool_normal_common_r14, bref, 1, 8));
  }
  if (v2x_comm_tx_pool_exceptional_r14_present) {
    HANDLE_CODE(v2x_comm_tx_pool_exceptional_r14.unpack(bref));
  }
  if (v2x_sync_cfg_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_sync_cfg_r14, bref, 1, 16));
  }
  if (v2x_inter_freq_info_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_inter_freq_info_list_r14, bref, 0, 7));
  }
  if (v2x_res_sel_cfg_r14_present) {
    HANDLE_CODE(v2x_res_sel_cfg_r14.unpack(bref));
  }
  if (zone_cfg_r14_present) {
    HANDLE_CODE(zone_cfg_r14.unpack(bref));
  }
  if (type_tx_sync_r14_present) {
    HANDLE_CODE(type_tx_sync_r14.unpack(bref));
  }
  if (thres_sl_tx_prioritization_r14_present) {
    HANDLE_CODE(unpack_integer(thres_sl_tx_prioritization_r14, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (anchor_carrier_freq_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(anchor_carrier_freq_list_r14, bref, 1, 8, integer_packer<uint32_t>(0, 262143)));
  }
  if (offset_dfn_r14_present) {
    HANDLE_CODE(unpack_integer(offset_dfn_r14, bref, (uint16_t)0u, (uint16_t)1000u));
  }
  if (cbr_common_tx_cfg_list_r14_present) {
    HANDLE_CODE(cbr_common_tx_cfg_list_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_v2x_cfg_common_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (v2x_comm_rx_pool_r14_present) {
    j.start_array("v2x-CommRxPool-r14");
    for (const auto& e1 : v2x_comm_rx_pool_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (v2x_comm_tx_pool_normal_common_r14_present) {
    j.start_array("v2x-CommTxPoolNormalCommon-r14");
    for (const auto& e1 : v2x_comm_tx_pool_normal_common_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (p2x_comm_tx_pool_normal_common_r14_present) {
    j.start_array("p2x-CommTxPoolNormalCommon-r14");
    for (const auto& e1 : p2x_comm_tx_pool_normal_common_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (v2x_comm_tx_pool_exceptional_r14_present) {
    j.write_fieldname("v2x-CommTxPoolExceptional-r14");
    v2x_comm_tx_pool_exceptional_r14.to_json(j);
  }
  if (v2x_sync_cfg_r14_present) {
    j.start_array("v2x-SyncConfig-r14");
    for (const auto& e1 : v2x_sync_cfg_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (v2x_inter_freq_info_list_r14_present) {
    j.start_array("v2x-InterFreqInfoList-r14");
    for (const auto& e1 : v2x_inter_freq_info_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (v2x_res_sel_cfg_r14_present) {
    j.write_fieldname("v2x-ResourceSelectionConfig-r14");
    v2x_res_sel_cfg_r14.to_json(j);
  }
  if (zone_cfg_r14_present) {
    j.write_fieldname("zoneConfig-r14");
    zone_cfg_r14.to_json(j);
  }
  if (type_tx_sync_r14_present) {
    j.write_str("typeTxSync-r14", type_tx_sync_r14.to_string());
  }
  if (thres_sl_tx_prioritization_r14_present) {
    j.write_int("thresSL-TxPrioritization-r14", thres_sl_tx_prioritization_r14);
  }
  if (anchor_carrier_freq_list_r14_present) {
    j.start_array("anchorCarrierFreqList-r14");
    for (const auto& e1 : anchor_carrier_freq_list_r14) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (offset_dfn_r14_present) {
    j.write_int("offsetDFN-r14", offset_dfn_r14);
  }
  if (cbr_common_tx_cfg_list_r14_present) {
    j.write_fieldname("cbr-CommonTxConfigList-r14");
    cbr_common_tx_cfg_list_r14.to_json(j);
  }
  j.end_obj();
}

// SL-V2X-PacketDuplicationConfig-r15 ::= SEQUENCE
SRSASN_CODE sl_v2x_packet_dupl_cfg_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(allowed_carrier_freq_cfg_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, thresh_sl_reliability_r15, (uint8_t)1u, (uint8_t)8u));
  if (allowed_carrier_freq_cfg_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, allowed_carrier_freq_cfg_r15, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_v2x_packet_dupl_cfg_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(allowed_carrier_freq_cfg_r15_present, 1));

  HANDLE_CODE(unpack_integer(thresh_sl_reliability_r15, bref, (uint8_t)1u, (uint8_t)8u));
  if (allowed_carrier_freq_cfg_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(allowed_carrier_freq_cfg_r15, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void sl_v2x_packet_dupl_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("threshSL-Reliability-r15", thresh_sl_reliability_r15);
  if (allowed_carrier_freq_cfg_r15_present) {
    j.start_array("allowedCarrierFreqConfig-r15");
    for (const auto& e1 : allowed_carrier_freq_cfg_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// TimeReferenceInfo-r15 ::= SEQUENCE
SRSASN_CODE time_ref_info_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(uncertainty_r15_present, 1));
  HANDLE_CODE(bref.pack(time_info_type_r15_present, 1));
  HANDLE_CODE(bref.pack(ref_sfn_r15_present, 1));

  HANDLE_CODE(time_r15.pack(bref));
  if (uncertainty_r15_present) {
    HANDLE_CODE(pack_integer(bref, uncertainty_r15, (uint8_t)0u, (uint8_t)12u));
  }
  if (ref_sfn_r15_present) {
    HANDLE_CODE(pack_integer(bref, ref_sfn_r15, (uint16_t)0u, (uint16_t)1023u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE time_ref_info_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(uncertainty_r15_present, 1));
  HANDLE_CODE(bref.unpack(time_info_type_r15_present, 1));
  HANDLE_CODE(bref.unpack(ref_sfn_r15_present, 1));

  HANDLE_CODE(time_r15.unpack(bref));
  if (uncertainty_r15_present) {
    HANDLE_CODE(unpack_integer(uncertainty_r15, bref, (uint8_t)0u, (uint8_t)12u));
  }
  if (ref_sfn_r15_present) {
    HANDLE_CODE(unpack_integer(ref_sfn_r15, bref, (uint16_t)0u, (uint16_t)1023u));
  }

  return SRSASN_SUCCESS;
}
void time_ref_info_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("time-r15");
  time_r15.to_json(j);
  if (uncertainty_r15_present) {
    j.write_int("uncertainty-r15", uncertainty_r15);
  }
  if (time_info_type_r15_present) {
    j.write_str("timeInfoType-r15", "localClock");
  }
  if (ref_sfn_r15_present) {
    j.write_int("referenceSFN-r15", ref_sfn_r15);
  }
  j.end_obj();
}

// WLAN-OffloadInfoPerPLMN-r12 ::= SEQUENCE
SRSASN_CODE wlan_offload_info_per_plmn_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(wlan_offload_cfg_common_r12_present, 1));
  HANDLE_CODE(bref.pack(wlan_id_list_r12_present, 1));

  if (wlan_offload_cfg_common_r12_present) {
    HANDLE_CODE(wlan_offload_cfg_common_r12.pack(bref));
  }
  if (wlan_id_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, wlan_id_list_r12, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_offload_info_per_plmn_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(wlan_offload_cfg_common_r12_present, 1));
  HANDLE_CODE(bref.unpack(wlan_id_list_r12_present, 1));

  if (wlan_offload_cfg_common_r12_present) {
    HANDLE_CODE(wlan_offload_cfg_common_r12.unpack(bref));
  }
  if (wlan_id_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(wlan_id_list_r12, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void wlan_offload_info_per_plmn_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (wlan_offload_cfg_common_r12_present) {
    j.write_fieldname("wlan-OffloadConfigCommon-r12");
    wlan_offload_cfg_common_r12.to_json(j);
  }
  if (wlan_id_list_r12_present) {
    j.start_array("wlan-Id-List-r12");
    for (const auto& e1 : wlan_id_list_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-GapPattern-r13 ::= SEQUENCE
SRSASN_CODE sl_gap_pattern_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(gap_period_r13.pack(bref));
  HANDLE_CODE(gap_offset_r12.pack(bref));
  HANDLE_CODE(gap_sf_bitmap_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_gap_pattern_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(gap_period_r13.unpack(bref));
  HANDLE_CODE(gap_offset_r12.unpack(bref));
  HANDLE_CODE(gap_sf_bitmap_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_gap_pattern_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("gapPeriod-r13", gap_period_r13.to_string());
  j.write_fieldname("gapOffset-r12");
  gap_offset_r12.to_json(j);
  j.write_str("gapSubframeBitmap-r13", gap_sf_bitmap_r13.to_string());
  j.end_obj();
}

const char* sl_gap_pattern_r13_s::gap_period_r13_opts::to_string() const
{
  static const char* options[] = {"sf40",
                                  "sf60",
                                  "sf70",
                                  "sf80",
                                  "sf120",
                                  "sf140",
                                  "sf160",
                                  "sf240",
                                  "sf280",
                                  "sf320",
                                  "sf640",
                                  "sf1280",
                                  "sf2560",
                                  "sf5120",
                                  "sf10240"};
  return convert_enum_idx(options, 15, value, "sl_gap_pattern_r13_s::gap_period_r13_e_");
}
uint16_t sl_gap_pattern_r13_s::gap_period_r13_opts::to_number() const
{
  static const uint16_t options[] = {40, 60, 70, 80, 120, 140, 160, 240, 280, 320, 640, 1280, 2560, 5120, 10240};
  return map_enum_number(options, 15, value, "sl_gap_pattern_r13_s::gap_period_r13_e_");
}
