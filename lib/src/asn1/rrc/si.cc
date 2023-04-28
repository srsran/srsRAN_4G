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

#include "srsran/asn1/rrc/si.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// MasterInformationBlock ::= SEQUENCE
SRSASN_CODE mib_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(dl_bw.pack(bref));
  HANDLE_CODE(phich_cfg.pack(bref));
  HANDLE_CODE(sys_frame_num.pack(bref));
  HANDLE_CODE(pack_integer(bref, sched_info_sib1_br_r13, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(bref.pack(sys_info_unchanged_br_r15, 1));
  HANDLE_CODE(part_earfcn_minus17.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mib_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(dl_bw.unpack(bref));
  HANDLE_CODE(phich_cfg.unpack(bref));
  HANDLE_CODE(sys_frame_num.unpack(bref));
  HANDLE_CODE(unpack_integer(sched_info_sib1_br_r13, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(bref.unpack(sys_info_unchanged_br_r15, 1));
  HANDLE_CODE(part_earfcn_minus17.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void mib_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("dl-Bandwidth", dl_bw.to_string());
  j.write_fieldname("phich-Config");
  phich_cfg.to_json(j);
  j.write_str("systemFrameNumber", sys_frame_num.to_string());
  j.write_int("schedulingInfoSIB1-BR-r13", sched_info_sib1_br_r13);
  j.write_bool("systemInfoUnchanged-BR-r15", sys_info_unchanged_br_r15);
  j.write_fieldname("partEARFCN-17");
  part_earfcn_minus17.to_json(j);
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

const char* mib_s::dl_bw_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "mib_s::dl_bw_e_");
}
uint8_t mib_s::dl_bw_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "mib_s::dl_bw_e_");
}

void mib_s::part_earfcn_minus17_c_::destroy_()
{
  switch (type_) {
    case types::spare:
      c.destroy<fixed_bitstring<2> >();
      break;
    case types::earfcn_lsb:
      c.destroy<fixed_bitstring<2> >();
      break;
    default:
      break;
  }
}
void mib_s::part_earfcn_minus17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::spare:
      c.init<fixed_bitstring<2> >();
      break;
    case types::earfcn_lsb:
      c.init<fixed_bitstring<2> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mib_s::part_earfcn_minus17_c_");
  }
}
mib_s::part_earfcn_minus17_c_::part_earfcn_minus17_c_(const mib_s::part_earfcn_minus17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::spare:
      c.init(other.c.get<fixed_bitstring<2> >());
      break;
    case types::earfcn_lsb:
      c.init(other.c.get<fixed_bitstring<2> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mib_s::part_earfcn_minus17_c_");
  }
}
mib_s::part_earfcn_minus17_c_& mib_s::part_earfcn_minus17_c_::operator=(const mib_s::part_earfcn_minus17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::spare:
      c.set(other.c.get<fixed_bitstring<2> >());
      break;
    case types::earfcn_lsb:
      c.set(other.c.get<fixed_bitstring<2> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mib_s::part_earfcn_minus17_c_");
  }

  return *this;
}
fixed_bitstring<2>& mib_s::part_earfcn_minus17_c_::set_spare()
{
  set(types::spare);
  return c.get<fixed_bitstring<2> >();
}
fixed_bitstring<2>& mib_s::part_earfcn_minus17_c_::set_earfcn_lsb()
{
  set(types::earfcn_lsb);
  return c.get<fixed_bitstring<2> >();
}
void mib_s::part_earfcn_minus17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::spare:
      j.write_str("spare", c.get<fixed_bitstring<2> >().to_string());
      break;
    case types::earfcn_lsb:
      j.write_str("earfcn-LSB", c.get<fixed_bitstring<2> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "mib_s::part_earfcn_minus17_c_");
  }
  j.end_obj();
}
SRSASN_CODE mib_s::part_earfcn_minus17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::spare:
      HANDLE_CODE(c.get<fixed_bitstring<2> >().pack(bref));
      break;
    case types::earfcn_lsb:
      HANDLE_CODE(c.get<fixed_bitstring<2> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mib_s::part_earfcn_minus17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mib_s::part_earfcn_minus17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::spare:
      HANDLE_CODE(c.get<fixed_bitstring<2> >().unpack(bref));
      break;
    case types::earfcn_lsb:
      HANDLE_CODE(c.get<fixed_bitstring<2> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mib_s::part_earfcn_minus17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// GNSS-ID-r15 ::= SEQUENCE
SRSASN_CODE gnss_id_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(gnss_id_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnss_id_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(gnss_id_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void gnss_id_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("gnss-id-r15", gnss_id_r15.to_string());
  j.end_obj();
}

const char* gnss_id_r15_s::gnss_id_r15_opts::to_string() const
{
  static const char* options[] = {"gps", "sbas", "qzss", "galileo", "glonass", "bds", "navic-v1610"};
  return convert_enum_idx(options, 7, value, "gnss_id_r15_s::gnss_id_r15_e_");
}

// PLMN-IdentityInfo-v1700 ::= SEQUENCE
SRSASN_CODE plmn_id_info_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tracking_area_list_r17_present, 1));

  if (tracking_area_list_r17_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, tracking_area_list_r17, 1, 12));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_info_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tracking_area_list_r17_present, 1));

  if (tracking_area_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(tracking_area_list_r17, bref, 1, 12));
  }

  return SRSASN_SUCCESS;
}
void plmn_id_info_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tracking_area_list_r17_present) {
    j.start_array("trackingAreaList-r17");
    for (const auto& e1 : tracking_area_list_r17) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  j.end_obj();
}

// SBAS-ID-r15 ::= SEQUENCE
SRSASN_CODE sbas_id_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(sbas_id_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sbas_id_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(sbas_id_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void sbas_id_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sbas-id-r15", sbas_id_r15.to_string());
  j.end_obj();
}

const char* sbas_id_r15_s::sbas_id_r15_opts::to_string() const
{
  static const char* options[] = {"waas", "egnos", "msas", "gagan"};
  return convert_enum_idx(options, 4, value, "sbas_id_r15_s::sbas_id_r15_e_");
}

// PLMN-IdentityInfo-v1610 ::= SEQUENCE
SRSASN_CODE plmn_id_info_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cp_cio_t_minus5_gs_optim_r16_present, 1));
  HANDLE_CODE(bref.pack(up_cio_t_minus5_gs_optim_r16_present, 1));
  HANDLE_CODE(bref.pack(iab_support_r16_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_info_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cp_cio_t_minus5_gs_optim_r16_present, 1));
  HANDLE_CODE(bref.unpack(up_cio_t_minus5_gs_optim_r16_present, 1));
  HANDLE_CODE(bref.unpack(iab_support_r16_present, 1));

  return SRSASN_SUCCESS;
}
void plmn_id_info_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cp_cio_t_minus5_gs_optim_r16_present) {
    j.write_str("cp-CIoT-5GS-Optimisation-r16", "true");
  }
  if (up_cio_t_minus5_gs_optim_r16_present) {
    j.write_str("up-CIoT-5GS-Optimisation-r16", "true");
  }
  if (iab_support_r16_present) {
    j.write_str("iab-Support-r16", "true");
  }
  j.end_obj();
}

// PosSIB-Type-r15 ::= SEQUENCE
SRSASN_CODE pos_sib_type_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(encrypted_r15_present, 1));
  HANDLE_CODE(bref.pack(gnss_id_r15_present, 1));
  HANDLE_CODE(bref.pack(sbas_id_r15_present, 1));

  if (gnss_id_r15_present) {
    HANDLE_CODE(gnss_id_r15.pack(bref));
  }
  if (sbas_id_r15_present) {
    HANDLE_CODE(sbas_id_r15.pack(bref));
  }
  HANDLE_CODE(pos_sib_type_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_sib_type_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(encrypted_r15_present, 1));
  HANDLE_CODE(bref.unpack(gnss_id_r15_present, 1));
  HANDLE_CODE(bref.unpack(sbas_id_r15_present, 1));

  if (gnss_id_r15_present) {
    HANDLE_CODE(gnss_id_r15.unpack(bref));
  }
  if (sbas_id_r15_present) {
    HANDLE_CODE(sbas_id_r15.unpack(bref));
  }
  HANDLE_CODE(pos_sib_type_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void pos_sib_type_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (encrypted_r15_present) {
    j.write_str("encrypted-r15", "true");
  }
  if (gnss_id_r15_present) {
    j.write_fieldname("gnss-id-r15");
    gnss_id_r15.to_json(j);
  }
  if (sbas_id_r15_present) {
    j.write_fieldname("sbas-id-r15");
    sbas_id_r15.to_json(j);
  }
  j.write_str("posSibType-r15", pos_sib_type_r15.to_string());
  j.end_obj();
}

const char* pos_sib_type_r15_s::pos_sib_type_r15_opts::to_string() const
{
  static const char* options[] = {
      "posSibType1-1",        "posSibType1-2",        "posSibType1-3",        "posSibType1-4",
      "posSibType1-5",        "posSibType1-6",        "posSibType1-7",        "posSibType2-1",
      "posSibType2-2",        "posSibType2-3",        "posSibType2-4",        "posSibType2-5",
      "posSibType2-6",        "posSibType2-7",        "posSibType2-8",        "posSibType2-9",
      "posSibType2-10",       "posSibType2-11",       "posSibType2-12",       "posSibType2-13",
      "posSibType2-14",       "posSibType2-15",       "posSibType2-16",       "posSibType2-17",
      "posSibType2-18",       "posSibType2-19",       "posSibType3-1",        "posSibType1-8-v1610",
      "posSibType2-20-v1610", "posSibType2-21-v1610", "posSibType2-22-v1610", "posSibType2-23-v1610",
      "posSibType2-24-v1610", "posSibType2-25-v1610", "posSibType4-1-v1610",  "posSibType5-1-v1610",
      "posSibType1-9-v1700",  "posSibType1-10-v1700"};
  return convert_enum_idx(options, 38, value, "pos_sib_type_r15_s::pos_sib_type_r15_e_");
}

// SystemInformationBlockType1-v1700-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_access_related_info_ntn_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (cell_access_related_info_ntn_r17_present) {
    HANDLE_CODE(bref.pack(cell_access_related_info_ntn_r17.plmn_id_list_v1700_present, 1));
    HANDLE_CODE(cell_access_related_info_ntn_r17.cell_barred_ntn_r17.pack(bref));
    if (cell_access_related_info_ntn_r17.plmn_id_list_v1700_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, cell_access_related_info_ntn_r17.plmn_id_list_v1700, 1, 6));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_access_related_info_ntn_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cell_access_related_info_ntn_r17_present) {
    HANDLE_CODE(bref.unpack(cell_access_related_info_ntn_r17.plmn_id_list_v1700_present, 1));
    HANDLE_CODE(cell_access_related_info_ntn_r17.cell_barred_ntn_r17.unpack(bref));
    if (cell_access_related_info_ntn_r17.plmn_id_list_v1700_present) {
      HANDLE_CODE(unpack_dyn_seq_of(cell_access_related_info_ntn_r17.plmn_id_list_v1700, bref, 1, 6));
    }
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_access_related_info_ntn_r17_present) {
    j.write_fieldname("cellAccessRelatedInfo-NTN-r17");
    j.start_obj();
    j.write_str("cellBarred-NTN-r17", cell_access_related_info_ntn_r17.cell_barred_ntn_r17.to_string());
    if (cell_access_related_info_ntn_r17.plmn_id_list_v1700_present) {
      j.start_array("plmn-IdentityList-v1700");
      for (const auto& e1 : cell_access_related_info_ntn_r17.plmn_id_list_v1700) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* sib_type1_v1700_ies_s::cell_access_related_info_ntn_r17_s_::cell_barred_ntn_r17_opts::to_string() const
{
  static const char* options[] = {"barred", "notBarred"};
  return convert_enum_idx(
      options, 2, value, "sib_type1_v1700_ies_s::cell_access_related_info_ntn_r17_s_::cell_barred_ntn_r17_e_");
}

// PLMN-IdentityInfo-v1530 ::= SEQUENCE
SRSASN_CODE plmn_id_info_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cell_reserved_for_oper_crs_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_info_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cell_reserved_for_oper_crs_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void plmn_id_info_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cellReservedForOperatorUse-CRS-r15", cell_reserved_for_oper_crs_r15.to_string());
  j.end_obj();
}

const char* plmn_id_info_v1530_s::cell_reserved_for_oper_crs_r15_opts::to_string() const
{
  static const char* options[] = {"reserved", "notReserved"};
  return convert_enum_idx(options, 2, value, "plmn_id_info_v1530_s::cell_reserved_for_oper_crs_r15_e_");
}

// PosSchedulingInfo-r15 ::= SEQUENCE
SRSASN_CODE pos_sched_info_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pos_si_periodicity_r15.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, pos_sib_map_info_r15, 1, 32));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_sched_info_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(pos_si_periodicity_r15.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(pos_sib_map_info_r15, bref, 1, 32));

  return SRSASN_SUCCESS;
}
void pos_sched_info_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("posSI-Periodicity-r15", pos_si_periodicity_r15.to_string());
  j.start_array("posSIB-MappingInfo-r15");
  for (const auto& e1 : pos_sib_map_info_r15) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

const char* pos_sched_info_r15_s::pos_si_periodicity_r15_opts::to_string() const
{
  static const char* options[] = {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
  return convert_enum_idx(options, 7, value, "pos_sched_info_r15_s::pos_si_periodicity_r15_e_");
}
uint16_t pos_sched_info_r15_s::pos_si_periodicity_r15_opts::to_number() const
{
  static const uint16_t options[] = {8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 7, value, "pos_sched_info_r15_s::pos_si_periodicity_r15_e_");
}

// SystemInformationBlockType1-v1610-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(edrx_allowed_minus5_gc_r16_present, 1));
  HANDLE_CODE(bref.pack(tx_in_ctrl_ch_region_r16_present, 1));
  HANDLE_CODE(bref.pack(camping_allowed_in_ce_r16_present, 1));
  HANDLE_CODE(bref.pack(plmn_id_list_v1610_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (plmn_id_list_v1610_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_v1610, 1, 6));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(edrx_allowed_minus5_gc_r16_present, 1));
  HANDLE_CODE(bref.unpack(tx_in_ctrl_ch_region_r16_present, 1));
  HANDLE_CODE(bref.unpack(camping_allowed_in_ce_r16_present, 1));
  HANDLE_CODE(bref.unpack(plmn_id_list_v1610_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (plmn_id_list_v1610_present) {
    HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_v1610, bref, 1, 6));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (edrx_allowed_minus5_gc_r16_present) {
    j.write_str("eDRX-Allowed-5GC-r16", "true");
  }
  if (tx_in_ctrl_ch_region_r16_present) {
    j.write_str("transmissionInControlChRegion-r16", "true");
  }
  if (camping_allowed_in_ce_r16_present) {
    j.write_str("campingAllowedInCE-r16", "true");
  }
  if (plmn_id_list_v1610_present) {
    j.start_array("plmn-IdentityList-v1610");
    for (const auto& e1 : plmn_id_list_v1610) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CellSelectionInfoCE-v1530 ::= SEQUENCE
SRSASN_CODE cell_sel_info_ce_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pwr_class14dbm_offset_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_sel_info_ce_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(pwr_class14dbm_offset_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_sel_info_ce_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("powerClass14dBm-Offset-r15", pwr_class14dbm_offset_r15.to_string());
  j.end_obj();
}

const char* cell_sel_info_ce_v1530_s::pwr_class14dbm_offset_r15_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-3", "dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(options, 6, value, "cell_sel_info_ce_v1530_s::pwr_class14dbm_offset_r15_e_");
}
int8_t cell_sel_info_ce_v1530_s::pwr_class14dbm_offset_r15_opts::to_number() const
{
  static const int8_t options[] = {-6, -3, 3, 6, 9, 12};
  return map_enum_number(options, 6, value, "cell_sel_info_ce_v1530_s::pwr_class14dbm_offset_r15_e_");
}

// SystemInformationBlockType1-v1540-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(si_pos_offset_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v1540_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(si_pos_offset_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (si_pos_offset_r15_present) {
    j.write_str("si-posOffset-r15", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType1-v1530-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(hsdn_cell_r15_present, 1));
  HANDLE_CODE(bref.pack(cell_sel_info_ce_v1530_present, 1));
  HANDLE_CODE(bref.pack(crs_intf_mitig_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(plmn_id_list_v1530_present, 1));
  HANDLE_CODE(bref.pack(pos_sched_info_list_r15_present, 1));
  HANDLE_CODE(bref.pack(cell_access_related_info_minus5_gc_r15_present, 1));
  HANDLE_CODE(bref.pack(ims_emergency_support5_gc_r15_present, 1));
  HANDLE_CODE(bref.pack(ecall_over_ims_support5_gc_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (cell_sel_info_ce_v1530_present) {
    HANDLE_CODE(cell_sel_info_ce_v1530.pack(bref));
  }
  if (crs_intf_mitig_cfg_r15_present) {
    HANDLE_CODE(crs_intf_mitig_cfg_r15.pack(bref));
  }
  HANDLE_CODE(cell_barred_crs_r15.pack(bref));
  if (plmn_id_list_v1530_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_v1530, 1, 6));
  }
  if (pos_sched_info_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pos_sched_info_list_r15, 1, 32));
  }
  if (cell_access_related_info_minus5_gc_r15_present) {
    HANDLE_CODE(cell_access_related_info_minus5_gc_r15.cell_barred_minus5_gc_r15.pack(bref));
    HANDLE_CODE(cell_access_related_info_minus5_gc_r15.cell_barred_minus5_gc_crs_r15.pack(bref));
    HANDLE_CODE(pack_dyn_seq_of(
        bref, cell_access_related_info_minus5_gc_r15.cell_access_related_info_list_minus5_gc_r15, 1, 6));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(hsdn_cell_r15_present, 1));
  HANDLE_CODE(bref.unpack(cell_sel_info_ce_v1530_present, 1));
  HANDLE_CODE(bref.unpack(crs_intf_mitig_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(plmn_id_list_v1530_present, 1));
  HANDLE_CODE(bref.unpack(pos_sched_info_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(cell_access_related_info_minus5_gc_r15_present, 1));
  HANDLE_CODE(bref.unpack(ims_emergency_support5_gc_r15_present, 1));
  HANDLE_CODE(bref.unpack(ecall_over_ims_support5_gc_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cell_sel_info_ce_v1530_present) {
    HANDLE_CODE(cell_sel_info_ce_v1530.unpack(bref));
  }
  if (crs_intf_mitig_cfg_r15_present) {
    HANDLE_CODE(crs_intf_mitig_cfg_r15.unpack(bref));
  }
  HANDLE_CODE(cell_barred_crs_r15.unpack(bref));
  if (plmn_id_list_v1530_present) {
    HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_v1530, bref, 1, 6));
  }
  if (pos_sched_info_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pos_sched_info_list_r15, bref, 1, 32));
  }
  if (cell_access_related_info_minus5_gc_r15_present) {
    HANDLE_CODE(cell_access_related_info_minus5_gc_r15.cell_barred_minus5_gc_r15.unpack(bref));
    HANDLE_CODE(cell_access_related_info_minus5_gc_r15.cell_barred_minus5_gc_crs_r15.unpack(bref));
    HANDLE_CODE(unpack_dyn_seq_of(
        cell_access_related_info_minus5_gc_r15.cell_access_related_info_list_minus5_gc_r15, bref, 1, 6));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (hsdn_cell_r15_present) {
    j.write_str("hsdn-Cell-r15", "true");
  }
  if (cell_sel_info_ce_v1530_present) {
    j.write_fieldname("cellSelectionInfoCE-v1530");
    cell_sel_info_ce_v1530.to_json(j);
  }
  if (crs_intf_mitig_cfg_r15_present) {
    j.write_fieldname("crs-IntfMitigConfig-r15");
    crs_intf_mitig_cfg_r15.to_json(j);
  }
  j.write_str("cellBarred-CRS-r15", cell_barred_crs_r15.to_string());
  if (plmn_id_list_v1530_present) {
    j.start_array("plmn-IdentityList-v1530");
    for (const auto& e1 : plmn_id_list_v1530) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pos_sched_info_list_r15_present) {
    j.start_array("posSchedulingInfoList-r15");
    for (const auto& e1 : pos_sched_info_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cell_access_related_info_minus5_gc_r15_present) {
    j.write_fieldname("cellAccessRelatedInfo-5GC-r15");
    j.start_obj();
    j.write_str("cellBarred-5GC-r15", cell_access_related_info_minus5_gc_r15.cell_barred_minus5_gc_r15.to_string());
    j.write_str("cellBarred-5GC-CRS-r15",
                cell_access_related_info_minus5_gc_r15.cell_barred_minus5_gc_crs_r15.to_string());
    j.start_array("cellAccessRelatedInfoList-5GC-r15");
    for (const auto& e1 : cell_access_related_info_minus5_gc_r15.cell_access_related_info_list_minus5_gc_r15) {
      e1.to_json(j);
    }
    j.end_array();
    j.end_obj();
  }
  if (ims_emergency_support5_gc_r15_present) {
    j.write_str("ims-EmergencySupport5GC-r15", "true");
  }
  if (ecall_over_ims_support5_gc_r15_present) {
    j.write_str("eCallOverIMS-Support5GC-r15", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

void sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::set(types::options e)
{
  type_ = e;
}
void sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::set_crs_intf_mitig_enabled()
{
  set(types::crs_intf_mitig_enabled);
}
sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::crs_intf_mitig_num_prbs_e_&
sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::set_crs_intf_mitig_num_prbs()
{
  set(types::crs_intf_mitig_num_prbs);
  return c;
}
void sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::crs_intf_mitig_enabled:
      break;
    case types::crs_intf_mitig_num_prbs:
      j.write_str("crs-IntfMitigNumPRBs", c.to_string());
      break;
    default:
      log_invalid_choice_id(type_, "sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::crs_intf_mitig_enabled:
      break;
    case types::crs_intf_mitig_num_prbs:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::crs_intf_mitig_enabled:
      break;
    case types::crs_intf_mitig_num_prbs:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::crs_intf_mitig_num_prbs_opts::to_string() const
{
  static const char* options[] = {"n6", "n24"};
  return convert_enum_idx(
      options, 2, value, "sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::crs_intf_mitig_num_prbs_e_");
}
uint8_t sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::crs_intf_mitig_num_prbs_opts::to_number() const
{
  static const uint8_t options[] = {6, 24};
  return map_enum_number(
      options, 2, value, "sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::crs_intf_mitig_num_prbs_e_");
}

const char* sib_type1_v1530_ies_s::cell_barred_crs_r15_opts::to_string() const
{
  static const char* options[] = {"barred", "notBarred"};
  return convert_enum_idx(options, 2, value, "sib_type1_v1530_ies_s::cell_barred_crs_r15_e_");
}

const char*
sib_type1_v1530_ies_s::cell_access_related_info_minus5_gc_r15_s_::cell_barred_minus5_gc_r15_opts::to_string() const
{
  static const char* options[] = {"barred", "notBarred"};
  return convert_enum_idx(
      options,
      2,
      value,
      "sib_type1_v1530_ies_s::cell_access_related_info_minus5_gc_r15_s_::cell_barred_minus5_gc_r15_e_");
}

const char*
sib_type1_v1530_ies_s::cell_access_related_info_minus5_gc_r15_s_::cell_barred_minus5_gc_crs_r15_opts::to_string() const
{
  static const char* options[] = {"barred", "notBarred"};
  return convert_enum_idx(
      options,
      2,
      value,
      "sib_type1_v1530_ies_s::cell_access_related_info_minus5_gc_r15_s_::cell_barred_minus5_gc_crs_r15_e_");
}

// CellAccessRelatedInfo-r14 ::= SEQUENCE
SRSASN_CODE cell_access_related_info_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_r14, 1, 6));
  HANDLE_CODE(tac_r14.pack(bref));
  HANDLE_CODE(cell_id_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_access_related_info_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_r14, bref, 1, 6));
  HANDLE_CODE(tac_r14.unpack(bref));
  HANDLE_CODE(cell_id_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_access_related_info_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("plmn-IdentityList-r14");
  for (const auto& e1 : plmn_id_list_r14) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("trackingAreaCode-r14", tac_r14.to_string());
  j.write_str("cellIdentity-r14", cell_id_r14.to_string());
  j.end_obj();
}

// SystemInformationBlockType1-v1450-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v1450_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tdd_cfg_v1450_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (tdd_cfg_v1450_present) {
    HANDLE_CODE(tdd_cfg_v1450.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v1450_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tdd_cfg_v1450_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (tdd_cfg_v1450_present) {
    HANDLE_CODE(tdd_cfg_v1450.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v1450_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tdd_cfg_v1450_present) {
    j.write_fieldname("tdd-Config-v1450");
    tdd_cfg_v1450.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CellSelectionInfoCE1-v1360 ::= SEQUENCE
SRSASN_CODE cell_sel_info_ce1_v1360_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, delta_rx_lev_min_ce1_v1360, (int8_t)-8, (int8_t)-1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_sel_info_ce1_v1360_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(delta_rx_lev_min_ce1_v1360, bref, (int8_t)-8, (int8_t)-1));

  return SRSASN_SUCCESS;
}
void cell_sel_info_ce1_v1360_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("delta-RxLevMinCE1-v1360", delta_rx_lev_min_ce1_v1360);
  j.end_obj();
}

// SystemInformationBlockType1-v1430-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ecall_over_ims_support_r14_present, 1));
  HANDLE_CODE(bref.pack(tdd_cfg_v1430_present, 1));
  HANDLE_CODE(bref.pack(cell_access_related_info_list_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (tdd_cfg_v1430_present) {
    HANDLE_CODE(tdd_cfg_v1430.pack(bref));
  }
  if (cell_access_related_info_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cell_access_related_info_list_r14, 1, 5));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ecall_over_ims_support_r14_present, 1));
  HANDLE_CODE(bref.unpack(tdd_cfg_v1430_present, 1));
  HANDLE_CODE(bref.unpack(cell_access_related_info_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (tdd_cfg_v1430_present) {
    HANDLE_CODE(tdd_cfg_v1430.unpack(bref));
  }
  if (cell_access_related_info_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cell_access_related_info_list_r14, bref, 1, 5));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ecall_over_ims_support_r14_present) {
    j.write_str("eCallOverIMS-Support-r14", "true");
  }
  if (tdd_cfg_v1430_present) {
    j.write_fieldname("tdd-Config-v1430");
    tdd_cfg_v1430.to_json(j);
  }
  if (cell_access_related_info_list_r14_present) {
    j.start_array("cellAccessRelatedInfoList-r14");
    for (const auto& e1 : cell_access_related_info_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CellSelectionInfoCE1-r13 ::= SEQUENCE
SRSASN_CODE cell_sel_info_ce1_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(q_qual_min_rsrq_ce1_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, q_rx_lev_min_ce1_r13, (int8_t)-70, (int8_t)-22));
  if (q_qual_min_rsrq_ce1_r13_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_rsrq_ce1_r13, (int8_t)-34, (int8_t)-3));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_sel_info_ce1_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(q_qual_min_rsrq_ce1_r13_present, 1));

  HANDLE_CODE(unpack_integer(q_rx_lev_min_ce1_r13, bref, (int8_t)-70, (int8_t)-22));
  if (q_qual_min_rsrq_ce1_r13_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_rsrq_ce1_r13, bref, (int8_t)-34, (int8_t)-3));
  }

  return SRSASN_SUCCESS;
}
void cell_sel_info_ce1_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("q-RxLevMinCE1-r13", q_rx_lev_min_ce1_r13);
  if (q_qual_min_rsrq_ce1_r13_present) {
    j.write_int("q-QualMinRSRQ-CE1-r13", q_qual_min_rsrq_ce1_r13);
  }
  j.end_obj();
}

// SystemInformationBlockType1-v1360-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v1360_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_sel_info_ce1_v1360_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (cell_sel_info_ce1_v1360_present) {
    HANDLE_CODE(cell_sel_info_ce1_v1360.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v1360_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_sel_info_ce1_v1360_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cell_sel_info_ce1_v1360_present) {
    HANDLE_CODE(cell_sel_info_ce1_v1360.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v1360_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_sel_info_ce1_v1360_present) {
    j.write_fieldname("cellSelectionInfoCE1-v1360");
    cell_sel_info_ce1_v1360.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SchedulingInfo-BR-r13 ::= SEQUENCE
SRSASN_CODE sched_info_br_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, si_nb_r13, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(si_tbs_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_info_br_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(si_nb_r13, bref, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(si_tbs_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void sched_info_br_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("si-Narrowband-r13", si_nb_r13);
  j.write_str("si-TBS-r13", si_tbs_r13.to_string());
  j.end_obj();
}

const char* sched_info_br_r13_s::si_tbs_r13_opts::to_string() const
{
  static const char* options[] = {"b152", "b208", "b256", "b328", "b408", "b504", "b600", "b712", "b808", "b936"};
  return convert_enum_idx(options, 10, value, "sched_info_br_r13_s::si_tbs_r13_e_");
}
uint16_t sched_info_br_r13_s::si_tbs_r13_opts::to_number() const
{
  static const uint16_t options[] = {152, 208, 256, 328, 408, 504, 600, 712, 808, 936};
  return map_enum_number(options, 10, value, "sched_info_br_r13_s::si_tbs_r13_e_");
}

// SystemInformationBlockType1-v1350-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v1350_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_sel_info_ce1_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (cell_sel_info_ce1_r13_present) {
    HANDLE_CODE(cell_sel_info_ce1_r13.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v1350_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_sel_info_ce1_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cell_sel_info_ce1_r13_present) {
    HANDLE_CODE(cell_sel_info_ce1_r13.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v1350_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_sel_info_ce1_r13_present) {
    j.write_fieldname("cellSelectionInfoCE1-r13");
    cell_sel_info_ce1_r13.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// BarringPerACDC-Category-r13 ::= SEQUENCE
SRSASN_CODE barr_per_acdc_category_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(acdc_barr_cfg_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, acdc_category_r13, (uint8_t)1u, (uint8_t)16u));
  if (acdc_barr_cfg_r13_present) {
    HANDLE_CODE(acdc_barr_cfg_r13.ac_barr_factor_r13.pack(bref));
    HANDLE_CODE(acdc_barr_cfg_r13.ac_barr_time_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE barr_per_acdc_category_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(acdc_barr_cfg_r13_present, 1));

  HANDLE_CODE(unpack_integer(acdc_category_r13, bref, (uint8_t)1u, (uint8_t)16u));
  if (acdc_barr_cfg_r13_present) {
    HANDLE_CODE(acdc_barr_cfg_r13.ac_barr_factor_r13.unpack(bref));
    HANDLE_CODE(acdc_barr_cfg_r13.ac_barr_time_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void barr_per_acdc_category_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("acdc-Category-r13", acdc_category_r13);
  if (acdc_barr_cfg_r13_present) {
    j.write_fieldname("acdc-BarringConfig-r13");
    j.start_obj();
    j.write_str("ac-BarringFactor-r13", acdc_barr_cfg_r13.ac_barr_factor_r13.to_string());
    j.write_str("ac-BarringTime-r13", acdc_barr_cfg_r13.ac_barr_time_r13.to_string());
    j.end_obj();
  }
  j.end_obj();
}

const char* barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_opts::to_string() const
{
  static const char* options[] = {
      "p00", "p05", "p10", "p15", "p20", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p85", "p90", "p95"};
  return convert_enum_idx(
      options, 16, value, "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_e_");
}
float barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return map_enum_number(
      options, 16, value, "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_e_");
}
const char* barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_opts::to_number_string() const
{
  static const char* options[] = {
      "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "4.0", "5.0", "6.0", "7.0", "7.5", "8.0", "8.5", "9.0", "9.5"};
  return convert_enum_idx(
      options, 16, value, "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_e_");
}

const char* barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_time_r13_opts::to_string() const
{
  static const char* options[] = {"s4", "s8", "s16", "s32", "s64", "s128", "s256", "s512"};
  return convert_enum_idx(options, 8, value, "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_time_r13_e_");
}
uint16_t barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_time_r13_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 8, value, "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_time_r13_e_");
}

// CellSelectionInfoCE-r13 ::= SEQUENCE
SRSASN_CODE cell_sel_info_ce_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(q_qual_min_rsrq_ce_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, q_rx_lev_min_ce_r13, (int8_t)-70, (int8_t)-22));
  if (q_qual_min_rsrq_ce_r13_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_rsrq_ce_r13, (int8_t)-34, (int8_t)-3));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_sel_info_ce_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(q_qual_min_rsrq_ce_r13_present, 1));

  HANDLE_CODE(unpack_integer(q_rx_lev_min_ce_r13, bref, (int8_t)-70, (int8_t)-22));
  if (q_qual_min_rsrq_ce_r13_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_rsrq_ce_r13, bref, (int8_t)-34, (int8_t)-3));
  }

  return SRSASN_SUCCESS;
}
void cell_sel_info_ce_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("q-RxLevMinCE-r13", q_rx_lev_min_ce_r13);
  if (q_qual_min_rsrq_ce_r13_present) {
    j.write_int("q-QualMinRSRQ-CE-r13", q_qual_min_rsrq_ce_r13);
  }
  j.end_obj();
}

// SystemInformationBlockType1-v1320-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v1320_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_hop_params_dl_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (freq_hop_params_dl_r13_present) {
    HANDLE_CODE(bref.pack(freq_hop_params_dl_r13.mpdcch_pdsch_hop_nb_r13_present, 1));
    HANDLE_CODE(bref.pack(freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_a_r13_present, 1));
    HANDLE_CODE(bref.pack(freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_b_r13_present, 1));
    HANDLE_CODE(bref.pack(freq_hop_params_dl_r13.mpdcch_pdsch_hop_offset_r13_present, 1));
    if (freq_hop_params_dl_r13.mpdcch_pdsch_hop_nb_r13_present) {
      HANDLE_CODE(freq_hop_params_dl_r13.mpdcch_pdsch_hop_nb_r13.pack(bref));
    }
    if (freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_a_r13_present) {
      HANDLE_CODE(freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_a_r13.pack(bref));
    }
    if (freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_b_r13_present) {
      HANDLE_CODE(freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_b_r13.pack(bref));
    }
    if (freq_hop_params_dl_r13.mpdcch_pdsch_hop_offset_r13_present) {
      HANDLE_CODE(pack_integer(bref, freq_hop_params_dl_r13.mpdcch_pdsch_hop_offset_r13, (uint8_t)1u, (uint8_t)16u));
    }
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v1320_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(freq_hop_params_dl_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (freq_hop_params_dl_r13_present) {
    HANDLE_CODE(bref.unpack(freq_hop_params_dl_r13.mpdcch_pdsch_hop_nb_r13_present, 1));
    HANDLE_CODE(bref.unpack(freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_a_r13_present, 1));
    HANDLE_CODE(bref.unpack(freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_b_r13_present, 1));
    HANDLE_CODE(bref.unpack(freq_hop_params_dl_r13.mpdcch_pdsch_hop_offset_r13_present, 1));
    if (freq_hop_params_dl_r13.mpdcch_pdsch_hop_nb_r13_present) {
      HANDLE_CODE(freq_hop_params_dl_r13.mpdcch_pdsch_hop_nb_r13.unpack(bref));
    }
    if (freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_a_r13_present) {
      HANDLE_CODE(freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_a_r13.unpack(bref));
    }
    if (freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_b_r13_present) {
      HANDLE_CODE(freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_b_r13.unpack(bref));
    }
    if (freq_hop_params_dl_r13.mpdcch_pdsch_hop_offset_r13_present) {
      HANDLE_CODE(unpack_integer(freq_hop_params_dl_r13.mpdcch_pdsch_hop_offset_r13, bref, (uint8_t)1u, (uint8_t)16u));
    }
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v1320_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_hop_params_dl_r13_present) {
    j.write_fieldname("freqHoppingParametersDL-r13");
    j.start_obj();
    if (freq_hop_params_dl_r13.mpdcch_pdsch_hop_nb_r13_present) {
      j.write_str("mpdcch-pdsch-HoppingNB-r13", freq_hop_params_dl_r13.mpdcch_pdsch_hop_nb_r13.to_string());
    }
    if (freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_a_r13_present) {
      j.write_fieldname("interval-DLHoppingConfigCommonModeA-r13");
      freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_a_r13.to_json(j);
    }
    if (freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_b_r13_present) {
      j.write_fieldname("interval-DLHoppingConfigCommonModeB-r13");
      freq_hop_params_dl_r13.interv_dl_hop_cfg_common_mode_b_r13.to_json(j);
    }
    if (freq_hop_params_dl_r13.mpdcch_pdsch_hop_offset_r13_present) {
      j.write_int("mpdcch-pdsch-HoppingOffset-r13", freq_hop_params_dl_r13.mpdcch_pdsch_hop_offset_r13);
    }
    j.end_obj();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::mpdcch_pdsch_hop_nb_r13_opts::to_string() const
{
  static const char* options[] = {"nb2", "nb4"};
  return convert_enum_idx(
      options, 2, value, "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::mpdcch_pdsch_hop_nb_r13_e_");
}
uint8_t sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::mpdcch_pdsch_hop_nb_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(
      options, 2, value, "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::mpdcch_pdsch_hop_nb_r13_e_");
}

void sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::destroy_() {}
void sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::
    interv_dl_hop_cfg_common_mode_a_r13_c_(
        const sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_& other)
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
      log_invalid_choice_id(type_,
                            "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_");
  }
}
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_&
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::operator=(
    const sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_& other)
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
      log_invalid_choice_id(type_,
                            "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_");
  }

  return *this;
}
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_&
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::set_interv_fdd_r13()
{
  set(types::interv_fdd_r13);
  return c.get<interv_fdd_r13_e_>();
}
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_&
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::set_interv_tdd_r13()
{
  set(types::interv_tdd_r13);
  return c.get<interv_tdd_r13_e_>();
}
void sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::to_json(
    json_writer& j) const
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
      log_invalid_choice_id(type_,
                            "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_,
                            "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_,
                            "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::
    interv_fdd_r13_opts::to_string() const
{
  static const char* options[] = {"int1", "int2", "int4", "int8"};
  return convert_enum_idx(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_");
}
uint8_t sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_opts::
    to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_");
}

const char* sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::
    interv_tdd_r13_opts::to_string() const
{
  static const char* options[] = {"int1", "int5", "int10", "int20"};
  return convert_enum_idx(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_");
}
uint8_t sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_opts::
    to_number() const
{
  static const uint8_t options[] = {1, 5, 10, 20};
  return map_enum_number(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_");
}

void sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::destroy_() {}
void sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::
    interv_dl_hop_cfg_common_mode_b_r13_c_(
        const sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_& other)
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
      log_invalid_choice_id(type_,
                            "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_");
  }
}
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_&
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::operator=(
    const sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_& other)
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
      log_invalid_choice_id(type_,
                            "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_");
  }

  return *this;
}
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_&
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::set_interv_fdd_r13()
{
  set(types::interv_fdd_r13);
  return c.get<interv_fdd_r13_e_>();
}
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_&
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::set_interv_tdd_r13()
{
  set(types::interv_tdd_r13);
  return c.get<interv_tdd_r13_e_>();
}
void sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::to_json(
    json_writer& j) const
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
      log_invalid_choice_id(type_,
                            "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_,
                            "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_,
                            "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::
    interv_fdd_r13_opts::to_string() const
{
  static const char* options[] = {"int2", "int4", "int8", "int16"};
  return convert_enum_idx(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_");
}
uint8_t sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_opts::
    to_number() const
{
  static const uint8_t options[] = {2, 4, 8, 16};
  return map_enum_number(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_");
}

const char* sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::
    interv_tdd_r13_opts::to_string() const
{
  static const char* options[] = {"int5", "int10", "int20", "int40"};
  return convert_enum_idx(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_");
}
uint8_t sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_opts::
    to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 40};
  return map_enum_number(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_");
}

// AC-BarringConfig ::= SEQUENCE
SRSASN_CODE ac_barr_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ac_barr_factor.pack(bref));
  HANDLE_CODE(ac_barr_time.pack(bref));
  HANDLE_CODE(ac_barr_for_special_ac.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ac_barr_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ac_barr_factor.unpack(bref));
  HANDLE_CODE(ac_barr_time.unpack(bref));
  HANDLE_CODE(ac_barr_for_special_ac.unpack(bref));

  return SRSASN_SUCCESS;
}
void ac_barr_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ac-BarringFactor", ac_barr_factor.to_string());
  j.write_str("ac-BarringTime", ac_barr_time.to_string());
  j.write_str("ac-BarringForSpecialAC", ac_barr_for_special_ac.to_string());
  j.end_obj();
}

const char* ac_barr_cfg_s::ac_barr_factor_opts::to_string() const
{
  static const char* options[] = {
      "p00", "p05", "p10", "p15", "p20", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p85", "p90", "p95"};
  return convert_enum_idx(options, 16, value, "ac_barr_cfg_s::ac_barr_factor_e_");
}
float ac_barr_cfg_s::ac_barr_factor_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return map_enum_number(options, 16, value, "ac_barr_cfg_s::ac_barr_factor_e_");
}
const char* ac_barr_cfg_s::ac_barr_factor_opts::to_number_string() const
{
  static const char* options[] = {
      "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "4.0", "5.0", "6.0", "7.0", "7.5", "8.0", "8.5", "9.0", "9.5"};
  return convert_enum_idx(options, 16, value, "ac_barr_cfg_s::ac_barr_factor_e_");
}

const char* ac_barr_cfg_s::ac_barr_time_opts::to_string() const
{
  static const char* options[] = {"s4", "s8", "s16", "s32", "s64", "s128", "s256", "s512"};
  return convert_enum_idx(options, 8, value, "ac_barr_cfg_s::ac_barr_time_e_");
}
uint16_t ac_barr_cfg_s::ac_barr_time_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 8, value, "ac_barr_cfg_s::ac_barr_time_e_");
}

// CellSelectionInfo-v1250 ::= SEQUENCE
SRSASN_CODE cell_sel_info_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, q_qual_min_rsrq_on_all_symbols_r12, (int8_t)-34, (int8_t)-3));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_sel_info_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(q_qual_min_rsrq_on_all_symbols_r12, bref, (int8_t)-34, (int8_t)-3));

  return SRSASN_SUCCESS;
}
void cell_sel_info_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("q-QualMinRSRQ-OnAllSymbols-r12", q_qual_min_rsrq_on_all_symbols_r12);
  j.end_obj();
}

// SystemInformationBlockType1-v1310-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v1310_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(hyper_sfn_r13_present, 1));
  HANDLE_CODE(bref.pack(edrx_allowed_r13_present, 1));
  HANDLE_CODE(bref.pack(cell_sel_info_ce_r13_present, 1));
  HANDLE_CODE(bref.pack(bw_reduced_access_related_info_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (hyper_sfn_r13_present) {
    HANDLE_CODE(hyper_sfn_r13.pack(bref));
  }
  if (cell_sel_info_ce_r13_present) {
    HANDLE_CODE(cell_sel_info_ce_r13.pack(bref));
  }
  if (bw_reduced_access_related_info_r13_present) {
    HANDLE_CODE(bref.pack(bw_reduced_access_related_info_r13.sched_info_list_br_r13_present, 1));
    HANDLE_CODE(bref.pack(bw_reduced_access_related_info_r13.fdd_dl_or_tdd_sf_bitmap_br_r13_present, 1));
    HANDLE_CODE(bref.pack(bw_reduced_access_related_info_r13.fdd_ul_sf_bitmap_br_r13_present, 1));
    HANDLE_CODE(bref.pack(bw_reduced_access_related_info_r13.si_validity_time_r13_present, 1));
    HANDLE_CODE(bref.pack(bw_reduced_access_related_info_r13.sys_info_value_tag_list_r13_present, 1));
    HANDLE_CODE(bw_reduced_access_related_info_r13.si_win_len_br_r13.pack(bref));
    HANDLE_CODE(bw_reduced_access_related_info_r13.si_repeat_pattern_r13.pack(bref));
    if (bw_reduced_access_related_info_r13.sched_info_list_br_r13_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, bw_reduced_access_related_info_r13.sched_info_list_br_r13, 1, 32));
    }
    if (bw_reduced_access_related_info_r13.fdd_dl_or_tdd_sf_bitmap_br_r13_present) {
      HANDLE_CODE(bw_reduced_access_related_info_r13.fdd_dl_or_tdd_sf_bitmap_br_r13.pack(bref));
    }
    if (bw_reduced_access_related_info_r13.fdd_ul_sf_bitmap_br_r13_present) {
      HANDLE_CODE(bw_reduced_access_related_info_r13.fdd_ul_sf_bitmap_br_r13.pack(bref));
    }
    HANDLE_CODE(pack_integer(bref, bw_reduced_access_related_info_r13.start_symbol_br_r13, (uint8_t)1u, (uint8_t)4u));
    HANDLE_CODE(bw_reduced_access_related_info_r13.si_hop_cfg_common_r13.pack(bref));
    if (bw_reduced_access_related_info_r13.sys_info_value_tag_list_r13_present) {
      HANDLE_CODE(pack_dyn_seq_of(
          bref, bw_reduced_access_related_info_r13.sys_info_value_tag_list_r13, 1, 32, integer_packer<uint8_t>(0, 3)));
    }
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v1310_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(hyper_sfn_r13_present, 1));
  HANDLE_CODE(bref.unpack(edrx_allowed_r13_present, 1));
  HANDLE_CODE(bref.unpack(cell_sel_info_ce_r13_present, 1));
  HANDLE_CODE(bref.unpack(bw_reduced_access_related_info_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (hyper_sfn_r13_present) {
    HANDLE_CODE(hyper_sfn_r13.unpack(bref));
  }
  if (cell_sel_info_ce_r13_present) {
    HANDLE_CODE(cell_sel_info_ce_r13.unpack(bref));
  }
  if (bw_reduced_access_related_info_r13_present) {
    HANDLE_CODE(bref.unpack(bw_reduced_access_related_info_r13.sched_info_list_br_r13_present, 1));
    HANDLE_CODE(bref.unpack(bw_reduced_access_related_info_r13.fdd_dl_or_tdd_sf_bitmap_br_r13_present, 1));
    HANDLE_CODE(bref.unpack(bw_reduced_access_related_info_r13.fdd_ul_sf_bitmap_br_r13_present, 1));
    HANDLE_CODE(bref.unpack(bw_reduced_access_related_info_r13.si_validity_time_r13_present, 1));
    HANDLE_CODE(bref.unpack(bw_reduced_access_related_info_r13.sys_info_value_tag_list_r13_present, 1));
    HANDLE_CODE(bw_reduced_access_related_info_r13.si_win_len_br_r13.unpack(bref));
    HANDLE_CODE(bw_reduced_access_related_info_r13.si_repeat_pattern_r13.unpack(bref));
    if (bw_reduced_access_related_info_r13.sched_info_list_br_r13_present) {
      HANDLE_CODE(unpack_dyn_seq_of(bw_reduced_access_related_info_r13.sched_info_list_br_r13, bref, 1, 32));
    }
    if (bw_reduced_access_related_info_r13.fdd_dl_or_tdd_sf_bitmap_br_r13_present) {
      HANDLE_CODE(bw_reduced_access_related_info_r13.fdd_dl_or_tdd_sf_bitmap_br_r13.unpack(bref));
    }
    if (bw_reduced_access_related_info_r13.fdd_ul_sf_bitmap_br_r13_present) {
      HANDLE_CODE(bw_reduced_access_related_info_r13.fdd_ul_sf_bitmap_br_r13.unpack(bref));
    }
    HANDLE_CODE(unpack_integer(bw_reduced_access_related_info_r13.start_symbol_br_r13, bref, (uint8_t)1u, (uint8_t)4u));
    HANDLE_CODE(bw_reduced_access_related_info_r13.si_hop_cfg_common_r13.unpack(bref));
    if (bw_reduced_access_related_info_r13.sys_info_value_tag_list_r13_present) {
      HANDLE_CODE(unpack_dyn_seq_of(
          bw_reduced_access_related_info_r13.sys_info_value_tag_list_r13, bref, 1, 32, integer_packer<uint8_t>(0, 3)));
    }
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v1310_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (hyper_sfn_r13_present) {
    j.write_str("hyperSFN-r13", hyper_sfn_r13.to_string());
  }
  if (edrx_allowed_r13_present) {
    j.write_str("eDRX-Allowed-r13", "true");
  }
  if (cell_sel_info_ce_r13_present) {
    j.write_fieldname("cellSelectionInfoCE-r13");
    cell_sel_info_ce_r13.to_json(j);
  }
  if (bw_reduced_access_related_info_r13_present) {
    j.write_fieldname("bandwidthReducedAccessRelatedInfo-r13");
    j.start_obj();
    j.write_str("si-WindowLength-BR-r13", bw_reduced_access_related_info_r13.si_win_len_br_r13.to_string());
    j.write_str("si-RepetitionPattern-r13", bw_reduced_access_related_info_r13.si_repeat_pattern_r13.to_string());
    if (bw_reduced_access_related_info_r13.sched_info_list_br_r13_present) {
      j.start_array("schedulingInfoList-BR-r13");
      for (const auto& e1 : bw_reduced_access_related_info_r13.sched_info_list_br_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (bw_reduced_access_related_info_r13.fdd_dl_or_tdd_sf_bitmap_br_r13_present) {
      j.write_fieldname("fdd-DownlinkOrTddSubframeBitmapBR-r13");
      bw_reduced_access_related_info_r13.fdd_dl_or_tdd_sf_bitmap_br_r13.to_json(j);
    }
    if (bw_reduced_access_related_info_r13.fdd_ul_sf_bitmap_br_r13_present) {
      j.write_str("fdd-UplinkSubframeBitmapBR-r13",
                  bw_reduced_access_related_info_r13.fdd_ul_sf_bitmap_br_r13.to_string());
    }
    j.write_int("startSymbolBR-r13", bw_reduced_access_related_info_r13.start_symbol_br_r13);
    j.write_str("si-HoppingConfigCommon-r13", bw_reduced_access_related_info_r13.si_hop_cfg_common_r13.to_string());
    if (bw_reduced_access_related_info_r13.si_validity_time_r13_present) {
      j.write_str("si-ValidityTime-r13", "true");
    }
    if (bw_reduced_access_related_info_r13.sys_info_value_tag_list_r13_present) {
      j.start_array("systemInfoValueTagList-r13");
      for (const auto& e1 : bw_reduced_access_related_info_r13.sys_info_value_tag_list_r13) {
        j.write_int(e1);
      }
      j.end_array();
    }
    j.end_obj();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_win_len_br_r13_opts::to_string() const
{
  static const char* options[] = {"ms20", "ms40", "ms60", "ms80", "ms120", "ms160", "ms200", "spare"};
  return convert_enum_idx(
      options, 8, value, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_win_len_br_r13_e_");
}
uint8_t sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_win_len_br_r13_opts::to_number() const
{
  static const uint8_t options[] = {20, 40, 60, 80, 120, 160, 200};
  return map_enum_number(
      options, 7, value, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_win_len_br_r13_e_");
}

const char* sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_repeat_pattern_r13_opts::to_string() const
{
  static const char* options[] = {"everyRF", "every2ndRF", "every4thRF", "every8thRF"};
  return convert_enum_idx(
      options, 4, value, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_repeat_pattern_r13_e_");
}
uint8_t sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_repeat_pattern_r13_opts::to_number() const
{
  switch (value) {
    case every2nd_rf:
      return 2;
    case every4th_rf:
      return 4;
    case every8th_rf:
      return 8;
    default:
      invalid_enum_number(value,
                          "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_repeat_pattern_r13_e_");
  }
  return 0;
}

void sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::destroy_()
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
void sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::set(
    types::options e)
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
      log_invalid_choice_id(
          type_, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_");
  }
}
sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::
    fdd_dl_or_tdd_sf_bitmap_br_r13_c_(
        const sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_& other)
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
      log_invalid_choice_id(
          type_, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_");
  }
}
sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_&
sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::operator=(
    const sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_& other)
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
      log_invalid_choice_id(
          type_, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_");
  }

  return *this;
}
fixed_bitstring<10>&
sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::set_sf_pattern10_r13()
{
  set(types::sf_pattern10_r13);
  return c.get<fixed_bitstring<10> >();
}
fixed_bitstring<40>&
sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::set_sf_pattern40_r13()
{
  set(types::sf_pattern40_r13);
  return c.get<fixed_bitstring<40> >();
}
void sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::to_json(
    json_writer& j) const
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
      log_invalid_choice_id(
          type_, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::pack(
    bit_ref& bref) const
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
      log_invalid_choice_id(
          type_, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE
sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(
          type_, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_hop_cfg_common_r13_opts::to_string() const
{
  static const char* options[] = {"on", "off"};
  return convert_enum_idx(
      options, 2, value, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_hop_cfg_common_r13_e_");
}

// UDT-Restricting-r13 ::= SEQUENCE
SRSASN_CODE udt_restricting_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(udt_restricting_r13_present, 1));
  HANDLE_CODE(bref.pack(udt_restricting_time_r13_present, 1));

  if (udt_restricting_time_r13_present) {
    HANDLE_CODE(udt_restricting_time_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE udt_restricting_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(udt_restricting_r13_present, 1));
  HANDLE_CODE(bref.unpack(udt_restricting_time_r13_present, 1));

  if (udt_restricting_time_r13_present) {
    HANDLE_CODE(udt_restricting_time_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void udt_restricting_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (udt_restricting_r13_present) {
    j.write_str("udt-Restricting-r13", "true");
  }
  if (udt_restricting_time_r13_present) {
    j.write_str("udt-RestrictingTime-r13", udt_restricting_time_r13.to_string());
  }
  j.end_obj();
}

const char* udt_restricting_r13_s::udt_restricting_time_r13_opts::to_string() const
{
  static const char* options[] = {"s4", "s8", "s16", "s32", "s64", "s128", "s256", "s512"};
  return convert_enum_idx(options, 8, value, "udt_restricting_r13_s::udt_restricting_time_r13_e_");
}
uint16_t udt_restricting_r13_s::udt_restricting_time_r13_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 8, value, "udt_restricting_r13_s::udt_restricting_time_r13_e_");
}

// AC-BarringPerPLMN-r12 ::= SEQUENCE
SRSASN_CODE ac_barr_per_plmn_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ac_barr_info_r12_present, 1));
  HANDLE_CODE(bref.pack(ac_barr_skip_for_mmtel_voice_r12_present, 1));
  HANDLE_CODE(bref.pack(ac_barr_skip_for_mmtel_video_r12_present, 1));
  HANDLE_CODE(bref.pack(ac_barr_skip_for_sms_r12_present, 1));
  HANDLE_CODE(bref.pack(ac_barr_for_csfb_r12_present, 1));
  HANDLE_CODE(bref.pack(ssac_barr_for_mmtel_voice_r12_present, 1));
  HANDLE_CODE(bref.pack(ssac_barr_for_mmtel_video_r12_present, 1));

  HANDLE_CODE(pack_integer(bref, plmn_id_idx_r12, (uint8_t)1u, (uint8_t)6u));
  if (ac_barr_info_r12_present) {
    HANDLE_CODE(bref.pack(ac_barr_info_r12.ac_barr_for_mo_sig_r12_present, 1));
    HANDLE_CODE(bref.pack(ac_barr_info_r12.ac_barr_for_mo_data_r12_present, 1));
    HANDLE_CODE(bref.pack(ac_barr_info_r12.ac_barr_for_emergency_r12, 1));
    if (ac_barr_info_r12.ac_barr_for_mo_sig_r12_present) {
      HANDLE_CODE(ac_barr_info_r12.ac_barr_for_mo_sig_r12.pack(bref));
    }
    if (ac_barr_info_r12.ac_barr_for_mo_data_r12_present) {
      HANDLE_CODE(ac_barr_info_r12.ac_barr_for_mo_data_r12.pack(bref));
    }
  }
  if (ac_barr_for_csfb_r12_present) {
    HANDLE_CODE(ac_barr_for_csfb_r12.pack(bref));
  }
  if (ssac_barr_for_mmtel_voice_r12_present) {
    HANDLE_CODE(ssac_barr_for_mmtel_voice_r12.pack(bref));
  }
  if (ssac_barr_for_mmtel_video_r12_present) {
    HANDLE_CODE(ssac_barr_for_mmtel_video_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ac_barr_per_plmn_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ac_barr_info_r12_present, 1));
  HANDLE_CODE(bref.unpack(ac_barr_skip_for_mmtel_voice_r12_present, 1));
  HANDLE_CODE(bref.unpack(ac_barr_skip_for_mmtel_video_r12_present, 1));
  HANDLE_CODE(bref.unpack(ac_barr_skip_for_sms_r12_present, 1));
  HANDLE_CODE(bref.unpack(ac_barr_for_csfb_r12_present, 1));
  HANDLE_CODE(bref.unpack(ssac_barr_for_mmtel_voice_r12_present, 1));
  HANDLE_CODE(bref.unpack(ssac_barr_for_mmtel_video_r12_present, 1));

  HANDLE_CODE(unpack_integer(plmn_id_idx_r12, bref, (uint8_t)1u, (uint8_t)6u));
  if (ac_barr_info_r12_present) {
    HANDLE_CODE(bref.unpack(ac_barr_info_r12.ac_barr_for_mo_sig_r12_present, 1));
    HANDLE_CODE(bref.unpack(ac_barr_info_r12.ac_barr_for_mo_data_r12_present, 1));
    HANDLE_CODE(bref.unpack(ac_barr_info_r12.ac_barr_for_emergency_r12, 1));
    if (ac_barr_info_r12.ac_barr_for_mo_sig_r12_present) {
      HANDLE_CODE(ac_barr_info_r12.ac_barr_for_mo_sig_r12.unpack(bref));
    }
    if (ac_barr_info_r12.ac_barr_for_mo_data_r12_present) {
      HANDLE_CODE(ac_barr_info_r12.ac_barr_for_mo_data_r12.unpack(bref));
    }
  }
  if (ac_barr_for_csfb_r12_present) {
    HANDLE_CODE(ac_barr_for_csfb_r12.unpack(bref));
  }
  if (ssac_barr_for_mmtel_voice_r12_present) {
    HANDLE_CODE(ssac_barr_for_mmtel_voice_r12.unpack(bref));
  }
  if (ssac_barr_for_mmtel_video_r12_present) {
    HANDLE_CODE(ssac_barr_for_mmtel_video_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ac_barr_per_plmn_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("plmn-IdentityIndex-r12", plmn_id_idx_r12);
  if (ac_barr_info_r12_present) {
    j.write_fieldname("ac-BarringInfo-r12");
    j.start_obj();
    j.write_bool("ac-BarringForEmergency-r12", ac_barr_info_r12.ac_barr_for_emergency_r12);
    if (ac_barr_info_r12.ac_barr_for_mo_sig_r12_present) {
      j.write_fieldname("ac-BarringForMO-Signalling-r12");
      ac_barr_info_r12.ac_barr_for_mo_sig_r12.to_json(j);
    }
    if (ac_barr_info_r12.ac_barr_for_mo_data_r12_present) {
      j.write_fieldname("ac-BarringForMO-Data-r12");
      ac_barr_info_r12.ac_barr_for_mo_data_r12.to_json(j);
    }
    j.end_obj();
  }
  if (ac_barr_skip_for_mmtel_voice_r12_present) {
    j.write_str("ac-BarringSkipForMMTELVoice-r12", "true");
  }
  if (ac_barr_skip_for_mmtel_video_r12_present) {
    j.write_str("ac-BarringSkipForMMTELVideo-r12", "true");
  }
  if (ac_barr_skip_for_sms_r12_present) {
    j.write_str("ac-BarringSkipForSMS-r12", "true");
  }
  if (ac_barr_for_csfb_r12_present) {
    j.write_fieldname("ac-BarringForCSFB-r12");
    ac_barr_for_csfb_r12.to_json(j);
  }
  if (ssac_barr_for_mmtel_voice_r12_present) {
    j.write_fieldname("ssac-BarringForMMTEL-Voice-r12");
    ssac_barr_for_mmtel_voice_r12.to_json(j);
  }
  if (ssac_barr_for_mmtel_video_r12_present) {
    j.write_fieldname("ssac-BarringForMMTEL-Video-r12");
    ssac_barr_for_mmtel_video_r12.to_json(j);
  }
  j.end_obj();
}

// ACDC-BarringPerPLMN-r13 ::= SEQUENCE
SRSASN_CODE acdc_barr_per_plmn_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, plmn_id_idx_r13, (uint8_t)1u, (uint8_t)6u));
  HANDLE_CODE(bref.pack(acdc_only_for_hplmn_r13, 1));
  HANDLE_CODE(pack_dyn_seq_of(bref, barr_per_acdc_category_list_r13, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE acdc_barr_per_plmn_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(plmn_id_idx_r13, bref, (uint8_t)1u, (uint8_t)6u));
  HANDLE_CODE(bref.unpack(acdc_only_for_hplmn_r13, 1));
  HANDLE_CODE(unpack_dyn_seq_of(barr_per_acdc_category_list_r13, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void acdc_barr_per_plmn_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("plmn-IdentityIndex-r13", plmn_id_idx_r13);
  j.write_bool("acdc-OnlyForHPLMN-r13", acdc_only_for_hplmn_r13);
  j.start_array("barringPerACDC-CategoryList-r13");
  for (const auto& e1 : barr_per_acdc_category_list_r13) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// CIOT-OptimisationPLMN-r13 ::= SEQUENCE
SRSASN_CODE ciot_optim_plmn_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(up_cio_t_eps_optim_r13_present, 1));
  HANDLE_CODE(bref.pack(cp_cio_t_eps_optim_r13_present, 1));
  HANDLE_CODE(bref.pack(attach_without_pdn_connect_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ciot_optim_plmn_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(up_cio_t_eps_optim_r13_present, 1));
  HANDLE_CODE(bref.unpack(cp_cio_t_eps_optim_r13_present, 1));
  HANDLE_CODE(bref.unpack(attach_without_pdn_connect_r13_present, 1));

  return SRSASN_SUCCESS;
}
void ciot_optim_plmn_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (up_cio_t_eps_optim_r13_present) {
    j.write_str("up-CIoT-EPS-Optimisation-r13", "true");
  }
  if (cp_cio_t_eps_optim_r13_present) {
    j.write_str("cp-CIoT-EPS-Optimisation-r13", "true");
  }
  if (attach_without_pdn_connect_r13_present) {
    j.write_str("attachWithoutPDN-Connectivity-r13", "true");
  }
  j.end_obj();
}

// CellSelectionInfo-v1130 ::= SEQUENCE
SRSASN_CODE cell_sel_info_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, q_qual_min_wb_r11, (int8_t)-34, (int8_t)-3));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_sel_info_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(q_qual_min_wb_r11, bref, (int8_t)-34, (int8_t)-3));

  return SRSASN_SUCCESS;
}
void cell_sel_info_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("q-QualMinWB-r11", q_qual_min_wb_r11);
  j.end_obj();
}

// PLMN-Info-r15 ::= SEQUENCE
SRSASN_CODE plmn_info_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(upper_layer_ind_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_info_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(upper_layer_ind_r15_present, 1));

  return SRSASN_SUCCESS;
}
void plmn_info_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (upper_layer_ind_r15_present) {
    j.write_str("upperLayerIndication-r15", "true");
  }
  j.end_obj();
}

// SIB-Type ::= ENUMERATED
const char* sib_type_opts::to_string() const
{
  static const char* options[] = {
      "sibType3",        "sibType4",        "sibType5",        "sibType6",        "sibType7",         "sibType8",
      "sibType9",        "sibType10",       "sibType11",       "sibType12-v920",  "sibType13-v920",   "sibType14-v1130",
      "sibType15-v1130", "sibType16-v1130", "sibType17-v1250", "sibType18-v1250", "sibType19-v1250",  "sibType20-v1310",
      "sibType21-v1430", "sibType24-v1530", "sibType25-v1530", "sibType26-v1530", "sibType26a-v1610", "sibType27-v1610",
      "sibType28-v1610", "sibType29-v1610"};
  return convert_enum_idx(options, 26, value, "sib_type_e");
}

// SystemInformationBlockType1-v1250-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v1250_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_sel_info_v1250_present, 1));
  HANDLE_CODE(bref.pack(freq_band_ind_prio_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.pack(cell_access_related_info_v1250.category0_allowed_r12_present, 1));
  if (cell_sel_info_v1250_present) {
    HANDLE_CODE(cell_sel_info_v1250.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v1250_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_sel_info_v1250_present, 1));
  HANDLE_CODE(bref.unpack(freq_band_ind_prio_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.unpack(cell_access_related_info_v1250.category0_allowed_r12_present, 1));
  if (cell_sel_info_v1250_present) {
    HANDLE_CODE(cell_sel_info_v1250.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v1250_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellAccessRelatedInfo-v1250");
  j.start_obj();
  if (cell_access_related_info_v1250.category0_allowed_r12_present) {
    j.write_str("category0Allowed-r12", "true");
  }
  j.end_obj();
  if (cell_sel_info_v1250_present) {
    j.write_fieldname("cellSelectionInfo-v1250");
    cell_sel_info_v1250.to_json(j);
  }
  if (freq_band_ind_prio_r12_present) {
    j.write_str("freqBandIndicatorPriority-r12", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UDT-RestrictingPerPLMN-r13 ::= SEQUENCE
SRSASN_CODE udt_restricting_per_plmn_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(udt_restricting_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, plmn_id_idx_r13, (uint8_t)1u, (uint8_t)6u));
  if (udt_restricting_r13_present) {
    HANDLE_CODE(udt_restricting_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE udt_restricting_per_plmn_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(udt_restricting_r13_present, 1));

  HANDLE_CODE(unpack_integer(plmn_id_idx_r13, bref, (uint8_t)1u, (uint8_t)6u));
  if (udt_restricting_r13_present) {
    HANDLE_CODE(udt_restricting_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void udt_restricting_per_plmn_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("plmn-IdentityIndex-r13", plmn_id_idx_r13);
  if (udt_restricting_r13_present) {
    j.write_fieldname("udt-Restricting-r13");
    udt_restricting_r13.to_json(j);
  }
  j.end_obj();
}

// ACDC-BarringForCommon-r13 ::= SEQUENCE
SRSASN_CODE acdc_barr_for_common_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(acdc_hplm_nonly_r13, 1));
  HANDLE_CODE(pack_dyn_seq_of(bref, barr_per_acdc_category_list_r13, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE acdc_barr_for_common_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(acdc_hplm_nonly_r13, 1));
  HANDLE_CODE(unpack_dyn_seq_of(barr_per_acdc_category_list_r13, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void acdc_barr_for_common_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("acdc-HPLMNonly-r13", acdc_hplm_nonly_r13);
  j.start_array("barringPerACDC-CategoryList-r13");
  for (const auto& e1 : barr_per_acdc_category_list_r13) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// CellSelectionInfo-v920 ::= SEQUENCE
SRSASN_CODE cell_sel_info_v920_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(q_qual_min_offset_r9_present, 1));

  HANDLE_CODE(pack_integer(bref, q_qual_min_r9, (int8_t)-34, (int8_t)-3));
  if (q_qual_min_offset_r9_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_offset_r9, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_sel_info_v920_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(q_qual_min_offset_r9_present, 1));

  HANDLE_CODE(unpack_integer(q_qual_min_r9, bref, (int8_t)-34, (int8_t)-3));
  if (q_qual_min_offset_r9_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_offset_r9, bref, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
void cell_sel_info_v920_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("q-QualMin-r9", q_qual_min_r9);
  if (q_qual_min_offset_r9_present) {
    j.write_int("q-QualMinOffset-r9", q_qual_min_offset_r9);
  }
  j.end_obj();
}

// SI-Periodicity-r12 ::= ENUMERATED
const char* si_periodicity_r12_opts::to_string() const
{
  static const char* options[] = {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
  return convert_enum_idx(options, 7, value, "si_periodicity_r12_e");
}
uint16_t si_periodicity_r12_opts::to_number() const
{
  static const uint16_t options[] = {8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 7, value, "si_periodicity_r12_e");
}

// SystemInformationBlockType1-v1130-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v1130_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tdd_cfg_v1130_present, 1));
  HANDLE_CODE(bref.pack(cell_sel_info_v1130_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (tdd_cfg_v1130_present) {
    HANDLE_CODE(tdd_cfg_v1130.pack(bref));
  }
  if (cell_sel_info_v1130_present) {
    HANDLE_CODE(cell_sel_info_v1130.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v1130_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tdd_cfg_v1130_present, 1));
  HANDLE_CODE(bref.unpack(cell_sel_info_v1130_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (tdd_cfg_v1130_present) {
    HANDLE_CODE(tdd_cfg_v1130.unpack(bref));
  }
  if (cell_sel_info_v1130_present) {
    HANDLE_CODE(cell_sel_info_v1130.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v1130_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tdd_cfg_v1130_present) {
    j.write_fieldname("tdd-Config-v1130");
    tdd_cfg_v1130.to_json(j);
  }
  if (cell_sel_info_v1130_present) {
    j.write_fieldname("cellSelectionInfo-v1130");
    cell_sel_info_v1130.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-TimersAndConstants ::= SEQUENCE
SRSASN_CODE ue_timers_and_consts_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(t300.pack(bref));
  HANDLE_CODE(t301.pack(bref));
  HANDLE_CODE(t310.pack(bref));
  HANDLE_CODE(n310.pack(bref));
  HANDLE_CODE(t311.pack(bref));
  HANDLE_CODE(n311.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= t300_v1310_present;
    group_flags[0] |= t301_v1310_present;
    group_flags[1] |= t310_v1330_present;
    group_flags[2] |= t300_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(t300_v1310_present, 1));
      HANDLE_CODE(bref.pack(t301_v1310_present, 1));
      if (t300_v1310_present) {
        HANDLE_CODE(t300_v1310.pack(bref));
      }
      if (t301_v1310_present) {
        HANDLE_CODE(t301_v1310.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(t310_v1330_present, 1));
      if (t310_v1330_present) {
        HANDLE_CODE(t310_v1330.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(t300_r15_present, 1));
      if (t300_r15_present) {
        HANDLE_CODE(t300_r15.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_timers_and_consts_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(t300.unpack(bref));
  HANDLE_CODE(t301.unpack(bref));
  HANDLE_CODE(t310.unpack(bref));
  HANDLE_CODE(n310.unpack(bref));
  HANDLE_CODE(t311.unpack(bref));
  HANDLE_CODE(n311.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(t300_v1310_present, 1));
      HANDLE_CODE(bref.unpack(t301_v1310_present, 1));
      if (t300_v1310_present) {
        HANDLE_CODE(t300_v1310.unpack(bref));
      }
      if (t301_v1310_present) {
        HANDLE_CODE(t301_v1310.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(t310_v1330_present, 1));
      if (t310_v1330_present) {
        HANDLE_CODE(t310_v1330.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(t300_r15_present, 1));
      if (t300_r15_present) {
        HANDLE_CODE(t300_r15.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ue_timers_and_consts_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t300", t300.to_string());
  j.write_str("t301", t301.to_string());
  j.write_str("t310", t310.to_string());
  j.write_str("n310", n310.to_string());
  j.write_str("t311", t311.to_string());
  j.write_str("n311", n311.to_string());
  if (ext) {
    if (t300_v1310_present) {
      j.write_str("t300-v1310", t300_v1310.to_string());
    }
    if (t301_v1310_present) {
      j.write_str("t301-v1310", t301_v1310.to_string());
    }
    if (t310_v1330_present) {
      j.write_str("t310-v1330", t310_v1330.to_string());
    }
    if (t300_r15_present) {
      j.write_str("t300-r15", t300_r15.to_string());
    }
  }
  j.end_obj();
}

const char* ue_timers_and_consts_s::t300_opts::to_string() const
{
  static const char* options[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::t300_e_");
}
uint16_t ue_timers_and_consts_s::t300_opts::to_number() const
{
  static const uint16_t options[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::t300_e_");
}

const char* ue_timers_and_consts_s::t301_opts::to_string() const
{
  static const char* options[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::t301_e_");
}
uint16_t ue_timers_and_consts_s::t301_opts::to_number() const
{
  static const uint16_t options[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::t301_e_");
}

const char* ue_timers_and_consts_s::t310_opts::to_string() const
{
  static const char* options[] = {"ms0", "ms50", "ms100", "ms200", "ms500", "ms1000", "ms2000"};
  return convert_enum_idx(options, 7, value, "ue_timers_and_consts_s::t310_e_");
}
uint16_t ue_timers_and_consts_s::t310_opts::to_number() const
{
  static const uint16_t options[] = {0, 50, 100, 200, 500, 1000, 2000};
  return map_enum_number(options, 7, value, "ue_timers_and_consts_s::t310_e_");
}

const char* ue_timers_and_consts_s::n310_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::n310_e_");
}
uint8_t ue_timers_and_consts_s::n310_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::n310_e_");
}

const char* ue_timers_and_consts_s::t311_opts::to_string() const
{
  static const char* options[] = {"ms1000", "ms3000", "ms5000", "ms10000", "ms15000", "ms20000", "ms30000"};
  return convert_enum_idx(options, 7, value, "ue_timers_and_consts_s::t311_e_");
}
uint16_t ue_timers_and_consts_s::t311_opts::to_number() const
{
  static const uint16_t options[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return map_enum_number(options, 7, value, "ue_timers_and_consts_s::t311_e_");
}

const char* ue_timers_and_consts_s::n311_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::n311_e_");
}
uint8_t ue_timers_and_consts_s::n311_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::n311_e_");
}

const char* ue_timers_and_consts_s::t300_v1310_opts::to_string() const
{
  static const char* options[] = {"ms2500", "ms3000", "ms3500", "ms4000", "ms5000", "ms6000", "ms8000", "ms10000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::t300_v1310_e_");
}
uint16_t ue_timers_and_consts_s::t300_v1310_opts::to_number() const
{
  static const uint16_t options[] = {2500, 3000, 3500, 4000, 5000, 6000, 8000, 10000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::t300_v1310_e_");
}

const char* ue_timers_and_consts_s::t301_v1310_opts::to_string() const
{
  static const char* options[] = {"ms2500", "ms3000", "ms3500", "ms4000", "ms5000", "ms6000", "ms8000", "ms10000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::t301_v1310_e_");
}
uint16_t ue_timers_and_consts_s::t301_v1310_opts::to_number() const
{
  static const uint16_t options[] = {2500, 3000, 3500, 4000, 5000, 6000, 8000, 10000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::t301_v1310_e_");
}

const char* ue_timers_and_consts_s::t310_v1330_opts::to_string() const
{
  static const char* options[] = {"ms4000", "ms6000"};
  return convert_enum_idx(options, 2, value, "ue_timers_and_consts_s::t310_v1330_e_");
}
uint16_t ue_timers_and_consts_s::t310_v1330_opts::to_number() const
{
  static const uint16_t options[] = {4000, 6000};
  return map_enum_number(options, 2, value, "ue_timers_and_consts_s::t310_v1330_e_");
}

const char* ue_timers_and_consts_s::t300_r15_opts::to_string() const
{
  static const char* options[] = {"ms4000", "ms6000", "ms8000", "ms10000", "ms15000", "ms25000", "ms40000", "ms60000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::t300_r15_e_");
}
uint16_t ue_timers_and_consts_s::t300_r15_opts::to_number() const
{
  static const uint16_t options[] = {4000, 6000, 8000, 10000, 15000, 25000, 40000, 60000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::t300_r15_e_");
}

// SchedulingInfo ::= SEQUENCE
SRSASN_CODE sched_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(si_periodicity.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, sib_map_info, 0, 31));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(si_periodicity.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(sib_map_info, bref, 0, 31));

  return SRSASN_SUCCESS;
}
void sched_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("si-Periodicity", si_periodicity.to_string());
  j.start_array("sib-MappingInfo");
  for (const auto& e1 : sib_map_info) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

// SystemInformationBlockType1-v920-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v920_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ims_emergency_support_r9_present, 1));
  HANDLE_CODE(bref.pack(cell_sel_info_v920_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (cell_sel_info_v920_present) {
    HANDLE_CODE(cell_sel_info_v920.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v920_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ims_emergency_support_r9_present, 1));
  HANDLE_CODE(bref.unpack(cell_sel_info_v920_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cell_sel_info_v920_present) {
    HANDLE_CODE(cell_sel_info_v920.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v920_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ims_emergency_support_r9_present) {
    j.write_str("ims-EmergencySupport-r9", "true");
  }
  if (cell_sel_info_v920_present) {
    j.write_fieldname("cellSelectionInfo-v920");
    cell_sel_info_v920.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType2 ::= SEQUENCE
SRSASN_CODE sib_type2_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ac_barr_info_present, 1));
  HANDLE_CODE(bref.pack(mbsfn_sf_cfg_list_present, 1));

  if (ac_barr_info_present) {
    HANDLE_CODE(bref.pack(ac_barr_info.ac_barr_for_mo_sig_present, 1));
    HANDLE_CODE(bref.pack(ac_barr_info.ac_barr_for_mo_data_present, 1));
    HANDLE_CODE(bref.pack(ac_barr_info.ac_barr_for_emergency, 1));
    if (ac_barr_info.ac_barr_for_mo_sig_present) {
      HANDLE_CODE(ac_barr_info.ac_barr_for_mo_sig.pack(bref));
    }
    if (ac_barr_info.ac_barr_for_mo_data_present) {
      HANDLE_CODE(ac_barr_info.ac_barr_for_mo_data.pack(bref));
    }
  }
  HANDLE_CODE(rr_cfg_common.pack(bref));
  HANDLE_CODE(ue_timers_and_consts.pack(bref));
  HANDLE_CODE(bref.pack(freq_info.ul_carrier_freq_present, 1));
  HANDLE_CODE(bref.pack(freq_info.ul_bw_present, 1));
  if (freq_info.ul_carrier_freq_present) {
    HANDLE_CODE(pack_integer(bref, freq_info.ul_carrier_freq, (uint32_t)0u, (uint32_t)65535u));
  }
  if (freq_info.ul_bw_present) {
    HANDLE_CODE(freq_info.ul_bw.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, freq_info.add_spec_emission, (uint8_t)1u, (uint8_t)32u));
  if (mbsfn_sf_cfg_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbsfn_sf_cfg_list, 1, 8));
  }
  HANDLE_CODE(time_align_timer_common.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= late_non_crit_ext_present;
    group_flags[1] |= ssac_barr_for_mmtel_voice_r9.is_present();
    group_flags[1] |= ssac_barr_for_mmtel_video_r9.is_present();
    group_flags[2] |= ac_barr_for_csfb_r10.is_present();
    group_flags[3] |= ac_barr_skip_for_mmtel_voice_r12_present;
    group_flags[3] |= ac_barr_skip_for_mmtel_video_r12_present;
    group_flags[3] |= ac_barr_skip_for_sms_r12_present;
    group_flags[3] |= ac_barr_per_plmn_list_r12.is_present();
    group_flags[4] |= voice_service_cause_ind_r12_present;
    group_flags[5] |= acdc_barr_for_common_r13.is_present();
    group_flags[5] |= acdc_barr_per_plmn_list_r13.is_present();
    group_flags[6] |= udt_restricting_for_common_r13.is_present();
    group_flags[6] |= udt_restricting_per_plmn_list_r13.is_present();
    group_flags[6] |= cio_t_eps_optim_info_r13.is_present();
    group_flags[6] |= use_full_resume_id_r13_present;
    group_flags[7] |= unicast_freq_hop_ind_r13_present;
    group_flags[8] |= mbsfn_sf_cfg_list_v1430.is_present();
    group_flags[8] |= video_service_cause_ind_r14_present;
    group_flags[9] |= plmn_info_list_r15.is_present();
    group_flags[10] |= cp_edt_r15_present;
    group_flags[10] |= up_edt_r15_present;
    group_flags[10] |= idle_mode_meass_r15_present;
    group_flags[10] |= reduced_cp_latency_enabled_r15_present;
    group_flags[11] |= mbms_rom_service_ind_r15_present;
    group_flags[12] |= rlos_enabled_r16_present;
    group_flags[12] |= early_security_reactivation_r16_present;
    group_flags[12] |= cp_edt_minus5_gc_r16_present;
    group_flags[12] |= up_edt_minus5_gc_r16_present;
    group_flags[12] |= cp_pur_epc_r16_present;
    group_flags[12] |= up_pur_epc_r16_present;
    group_flags[12] |= cp_pur_minus5_gc_r16_present;
    group_flags[12] |= up_pur_minus5_gc_r16_present;
    group_flags[12] |= mpdcch_cqi_report_r16_present;
    group_flags[12] |= rai_activation_enh_r16_present;
    group_flags[12] |= idle_mode_meass_nr_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ssac_barr_for_mmtel_voice_r9.is_present(), 1));
      HANDLE_CODE(bref.pack(ssac_barr_for_mmtel_video_r9.is_present(), 1));
      if (ssac_barr_for_mmtel_voice_r9.is_present()) {
        HANDLE_CODE(ssac_barr_for_mmtel_voice_r9->pack(bref));
      }
      if (ssac_barr_for_mmtel_video_r9.is_present()) {
        HANDLE_CODE(ssac_barr_for_mmtel_video_r9->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ac_barr_for_csfb_r10.is_present(), 1));
      if (ac_barr_for_csfb_r10.is_present()) {
        HANDLE_CODE(ac_barr_for_csfb_r10->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ac_barr_skip_for_mmtel_voice_r12_present, 1));
      HANDLE_CODE(bref.pack(ac_barr_skip_for_mmtel_video_r12_present, 1));
      HANDLE_CODE(bref.pack(ac_barr_skip_for_sms_r12_present, 1));
      HANDLE_CODE(bref.pack(ac_barr_per_plmn_list_r12.is_present(), 1));
      if (ac_barr_per_plmn_list_r12.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *ac_barr_per_plmn_list_r12, 1, 6));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(voice_service_cause_ind_r12_present, 1));
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(acdc_barr_for_common_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(acdc_barr_per_plmn_list_r13.is_present(), 1));
      if (acdc_barr_for_common_r13.is_present()) {
        HANDLE_CODE(acdc_barr_for_common_r13->pack(bref));
      }
      if (acdc_barr_per_plmn_list_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *acdc_barr_per_plmn_list_r13, 1, 6));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(udt_restricting_for_common_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(udt_restricting_per_plmn_list_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(cio_t_eps_optim_info_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(use_full_resume_id_r13_present, 1));
      if (udt_restricting_for_common_r13.is_present()) {
        HANDLE_CODE(udt_restricting_for_common_r13->pack(bref));
      }
      if (udt_restricting_per_plmn_list_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *udt_restricting_per_plmn_list_r13, 1, 6));
      }
      if (cio_t_eps_optim_info_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *cio_t_eps_optim_info_r13, 1, 6));
      }
    }
    if (group_flags[7]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(unicast_freq_hop_ind_r13_present, 1));
    }
    if (group_flags[8]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mbsfn_sf_cfg_list_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(video_service_cause_ind_r14_present, 1));
      if (mbsfn_sf_cfg_list_v1430.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *mbsfn_sf_cfg_list_v1430, 1, 8));
      }
    }
    if (group_flags[9]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(plmn_info_list_r15.is_present(), 1));
      if (plmn_info_list_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *plmn_info_list_r15, 1, 6));
      }
    }
    if (group_flags[10]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cp_edt_r15_present, 1));
      HANDLE_CODE(bref.pack(up_edt_r15_present, 1));
      HANDLE_CODE(bref.pack(idle_mode_meass_r15_present, 1));
      HANDLE_CODE(bref.pack(reduced_cp_latency_enabled_r15_present, 1));
    }
    if (group_flags[11]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mbms_rom_service_ind_r15_present, 1));
    }
    if (group_flags[12]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlos_enabled_r16_present, 1));
      HANDLE_CODE(bref.pack(early_security_reactivation_r16_present, 1));
      HANDLE_CODE(bref.pack(cp_edt_minus5_gc_r16_present, 1));
      HANDLE_CODE(bref.pack(up_edt_minus5_gc_r16_present, 1));
      HANDLE_CODE(bref.pack(cp_pur_epc_r16_present, 1));
      HANDLE_CODE(bref.pack(up_pur_epc_r16_present, 1));
      HANDLE_CODE(bref.pack(cp_pur_minus5_gc_r16_present, 1));
      HANDLE_CODE(bref.pack(up_pur_minus5_gc_r16_present, 1));
      HANDLE_CODE(bref.pack(mpdcch_cqi_report_r16_present, 1));
      HANDLE_CODE(bref.pack(rai_activation_enh_r16_present, 1));
      HANDLE_CODE(bref.pack(idle_mode_meass_nr_r16_present, 1));
      if (mpdcch_cqi_report_r16_present) {
        HANDLE_CODE(mpdcch_cqi_report_r16.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type2_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ac_barr_info_present, 1));
  HANDLE_CODE(bref.unpack(mbsfn_sf_cfg_list_present, 1));

  if (ac_barr_info_present) {
    HANDLE_CODE(bref.unpack(ac_barr_info.ac_barr_for_mo_sig_present, 1));
    HANDLE_CODE(bref.unpack(ac_barr_info.ac_barr_for_mo_data_present, 1));
    HANDLE_CODE(bref.unpack(ac_barr_info.ac_barr_for_emergency, 1));
    if (ac_barr_info.ac_barr_for_mo_sig_present) {
      HANDLE_CODE(ac_barr_info.ac_barr_for_mo_sig.unpack(bref));
    }
    if (ac_barr_info.ac_barr_for_mo_data_present) {
      HANDLE_CODE(ac_barr_info.ac_barr_for_mo_data.unpack(bref));
    }
  }
  HANDLE_CODE(rr_cfg_common.unpack(bref));
  HANDLE_CODE(ue_timers_and_consts.unpack(bref));
  HANDLE_CODE(bref.unpack(freq_info.ul_carrier_freq_present, 1));
  HANDLE_CODE(bref.unpack(freq_info.ul_bw_present, 1));
  if (freq_info.ul_carrier_freq_present) {
    HANDLE_CODE(unpack_integer(freq_info.ul_carrier_freq, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (freq_info.ul_bw_present) {
    HANDLE_CODE(freq_info.ul_bw.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(freq_info.add_spec_emission, bref, (uint8_t)1u, (uint8_t)32u));
  if (mbsfn_sf_cfg_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbsfn_sf_cfg_list, bref, 1, 8));
  }
  HANDLE_CODE(time_align_timer_common.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(13);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ssac_barr_for_mmtel_voice_r9_present;
      HANDLE_CODE(bref.unpack(ssac_barr_for_mmtel_voice_r9_present, 1));
      ssac_barr_for_mmtel_voice_r9.set_present(ssac_barr_for_mmtel_voice_r9_present);
      bool ssac_barr_for_mmtel_video_r9_present;
      HANDLE_CODE(bref.unpack(ssac_barr_for_mmtel_video_r9_present, 1));
      ssac_barr_for_mmtel_video_r9.set_present(ssac_barr_for_mmtel_video_r9_present);
      if (ssac_barr_for_mmtel_voice_r9.is_present()) {
        HANDLE_CODE(ssac_barr_for_mmtel_voice_r9->unpack(bref));
      }
      if (ssac_barr_for_mmtel_video_r9.is_present()) {
        HANDLE_CODE(ssac_barr_for_mmtel_video_r9->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ac_barr_for_csfb_r10_present;
      HANDLE_CODE(bref.unpack(ac_barr_for_csfb_r10_present, 1));
      ac_barr_for_csfb_r10.set_present(ac_barr_for_csfb_r10_present);
      if (ac_barr_for_csfb_r10.is_present()) {
        HANDLE_CODE(ac_barr_for_csfb_r10->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ac_barr_skip_for_mmtel_voice_r12_present, 1));
      HANDLE_CODE(bref.unpack(ac_barr_skip_for_mmtel_video_r12_present, 1));
      HANDLE_CODE(bref.unpack(ac_barr_skip_for_sms_r12_present, 1));
      bool ac_barr_per_plmn_list_r12_present;
      HANDLE_CODE(bref.unpack(ac_barr_per_plmn_list_r12_present, 1));
      ac_barr_per_plmn_list_r12.set_present(ac_barr_per_plmn_list_r12_present);
      if (ac_barr_per_plmn_list_r12.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*ac_barr_per_plmn_list_r12, bref, 1, 6));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(voice_service_cause_ind_r12_present, 1));
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool acdc_barr_for_common_r13_present;
      HANDLE_CODE(bref.unpack(acdc_barr_for_common_r13_present, 1));
      acdc_barr_for_common_r13.set_present(acdc_barr_for_common_r13_present);
      bool acdc_barr_per_plmn_list_r13_present;
      HANDLE_CODE(bref.unpack(acdc_barr_per_plmn_list_r13_present, 1));
      acdc_barr_per_plmn_list_r13.set_present(acdc_barr_per_plmn_list_r13_present);
      if (acdc_barr_for_common_r13.is_present()) {
        HANDLE_CODE(acdc_barr_for_common_r13->unpack(bref));
      }
      if (acdc_barr_per_plmn_list_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*acdc_barr_per_plmn_list_r13, bref, 1, 6));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool udt_restricting_for_common_r13_present;
      HANDLE_CODE(bref.unpack(udt_restricting_for_common_r13_present, 1));
      udt_restricting_for_common_r13.set_present(udt_restricting_for_common_r13_present);
      bool udt_restricting_per_plmn_list_r13_present;
      HANDLE_CODE(bref.unpack(udt_restricting_per_plmn_list_r13_present, 1));
      udt_restricting_per_plmn_list_r13.set_present(udt_restricting_per_plmn_list_r13_present);
      bool cio_t_eps_optim_info_r13_present;
      HANDLE_CODE(bref.unpack(cio_t_eps_optim_info_r13_present, 1));
      cio_t_eps_optim_info_r13.set_present(cio_t_eps_optim_info_r13_present);
      HANDLE_CODE(bref.unpack(use_full_resume_id_r13_present, 1));
      if (udt_restricting_for_common_r13.is_present()) {
        HANDLE_CODE(udt_restricting_for_common_r13->unpack(bref));
      }
      if (udt_restricting_per_plmn_list_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*udt_restricting_per_plmn_list_r13, bref, 1, 6));
      }
      if (cio_t_eps_optim_info_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*cio_t_eps_optim_info_r13, bref, 1, 6));
      }
    }
    if (group_flags[7]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(unicast_freq_hop_ind_r13_present, 1));
    }
    if (group_flags[8]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mbsfn_sf_cfg_list_v1430_present;
      HANDLE_CODE(bref.unpack(mbsfn_sf_cfg_list_v1430_present, 1));
      mbsfn_sf_cfg_list_v1430.set_present(mbsfn_sf_cfg_list_v1430_present);
      HANDLE_CODE(bref.unpack(video_service_cause_ind_r14_present, 1));
      if (mbsfn_sf_cfg_list_v1430.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*mbsfn_sf_cfg_list_v1430, bref, 1, 8));
      }
    }
    if (group_flags[9]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool plmn_info_list_r15_present;
      HANDLE_CODE(bref.unpack(plmn_info_list_r15_present, 1));
      plmn_info_list_r15.set_present(plmn_info_list_r15_present);
      if (plmn_info_list_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*plmn_info_list_r15, bref, 1, 6));
      }
    }
    if (group_flags[10]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(cp_edt_r15_present, 1));
      HANDLE_CODE(bref.unpack(up_edt_r15_present, 1));
      HANDLE_CODE(bref.unpack(idle_mode_meass_r15_present, 1));
      HANDLE_CODE(bref.unpack(reduced_cp_latency_enabled_r15_present, 1));
    }
    if (group_flags[11]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(mbms_rom_service_ind_r15_present, 1));
    }
    if (group_flags[12]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(rlos_enabled_r16_present, 1));
      HANDLE_CODE(bref.unpack(early_security_reactivation_r16_present, 1));
      HANDLE_CODE(bref.unpack(cp_edt_minus5_gc_r16_present, 1));
      HANDLE_CODE(bref.unpack(up_edt_minus5_gc_r16_present, 1));
      HANDLE_CODE(bref.unpack(cp_pur_epc_r16_present, 1));
      HANDLE_CODE(bref.unpack(up_pur_epc_r16_present, 1));
      HANDLE_CODE(bref.unpack(cp_pur_minus5_gc_r16_present, 1));
      HANDLE_CODE(bref.unpack(up_pur_minus5_gc_r16_present, 1));
      HANDLE_CODE(bref.unpack(mpdcch_cqi_report_r16_present, 1));
      HANDLE_CODE(bref.unpack(rai_activation_enh_r16_present, 1));
      HANDLE_CODE(bref.unpack(idle_mode_meass_nr_r16_present, 1));
      if (mpdcch_cqi_report_r16_present) {
        HANDLE_CODE(mpdcch_cqi_report_r16.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type2_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ac_barr_info_present) {
    j.write_fieldname("ac-BarringInfo");
    j.start_obj();
    j.write_bool("ac-BarringForEmergency", ac_barr_info.ac_barr_for_emergency);
    if (ac_barr_info.ac_barr_for_mo_sig_present) {
      j.write_fieldname("ac-BarringForMO-Signalling");
      ac_barr_info.ac_barr_for_mo_sig.to_json(j);
    }
    if (ac_barr_info.ac_barr_for_mo_data_present) {
      j.write_fieldname("ac-BarringForMO-Data");
      ac_barr_info.ac_barr_for_mo_data.to_json(j);
    }
    j.end_obj();
  }
  j.write_fieldname("radioResourceConfigCommon");
  rr_cfg_common.to_json(j);
  j.write_fieldname("ue-TimersAndConstants");
  ue_timers_and_consts.to_json(j);
  j.write_fieldname("freqInfo");
  j.start_obj();
  if (freq_info.ul_carrier_freq_present) {
    j.write_int("ul-CarrierFreq", freq_info.ul_carrier_freq);
  }
  if (freq_info.ul_bw_present) {
    j.write_str("ul-Bandwidth", freq_info.ul_bw.to_string());
  }
  j.write_int("additionalSpectrumEmission", freq_info.add_spec_emission);
  j.end_obj();
  if (mbsfn_sf_cfg_list_present) {
    j.start_array("mbsfn-SubframeConfigList");
    for (const auto& e1 : mbsfn_sf_cfg_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("timeAlignmentTimerCommon", time_align_timer_common.to_string());
  if (ext) {
    if (late_non_crit_ext_present) {
      j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
    }
    if (ssac_barr_for_mmtel_voice_r9.is_present()) {
      j.write_fieldname("ssac-BarringForMMTEL-Voice-r9");
      ssac_barr_for_mmtel_voice_r9->to_json(j);
    }
    if (ssac_barr_for_mmtel_video_r9.is_present()) {
      j.write_fieldname("ssac-BarringForMMTEL-Video-r9");
      ssac_barr_for_mmtel_video_r9->to_json(j);
    }
    if (ac_barr_for_csfb_r10.is_present()) {
      j.write_fieldname("ac-BarringForCSFB-r10");
      ac_barr_for_csfb_r10->to_json(j);
    }
    if (ac_barr_skip_for_mmtel_voice_r12_present) {
      j.write_str("ac-BarringSkipForMMTELVoice-r12", "true");
    }
    if (ac_barr_skip_for_mmtel_video_r12_present) {
      j.write_str("ac-BarringSkipForMMTELVideo-r12", "true");
    }
    if (ac_barr_skip_for_sms_r12_present) {
      j.write_str("ac-BarringSkipForSMS-r12", "true");
    }
    if (ac_barr_per_plmn_list_r12.is_present()) {
      j.start_array("ac-BarringPerPLMN-List-r12");
      for (const auto& e1 : *ac_barr_per_plmn_list_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (voice_service_cause_ind_r12_present) {
      j.write_str("voiceServiceCauseIndication-r12", "true");
    }
    if (acdc_barr_for_common_r13.is_present()) {
      j.write_fieldname("acdc-BarringForCommon-r13");
      acdc_barr_for_common_r13->to_json(j);
    }
    if (acdc_barr_per_plmn_list_r13.is_present()) {
      j.start_array("acdc-BarringPerPLMN-List-r13");
      for (const auto& e1 : *acdc_barr_per_plmn_list_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (udt_restricting_for_common_r13.is_present()) {
      j.write_fieldname("udt-RestrictingForCommon-r13");
      udt_restricting_for_common_r13->to_json(j);
    }
    if (udt_restricting_per_plmn_list_r13.is_present()) {
      j.start_array("udt-RestrictingPerPLMN-List-r13");
      for (const auto& e1 : *udt_restricting_per_plmn_list_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (cio_t_eps_optim_info_r13.is_present()) {
      j.start_array("cIoT-EPS-OptimisationInfo-r13");
      for (const auto& e1 : *cio_t_eps_optim_info_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (use_full_resume_id_r13_present) {
      j.write_str("useFullResumeID-r13", "true");
    }
    if (unicast_freq_hop_ind_r13_present) {
      j.write_str("unicastFreqHoppingInd-r13", "true");
    }
    if (mbsfn_sf_cfg_list_v1430.is_present()) {
      j.start_array("mbsfn-SubframeConfigList-v1430");
      for (const auto& e1 : *mbsfn_sf_cfg_list_v1430) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (video_service_cause_ind_r14_present) {
      j.write_str("videoServiceCauseIndication-r14", "true");
    }
    if (plmn_info_list_r15.is_present()) {
      j.start_array("plmn-InfoList-r15");
      for (const auto& e1 : *plmn_info_list_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (cp_edt_r15_present) {
      j.write_str("cp-EDT-r15", "true");
    }
    if (up_edt_r15_present) {
      j.write_str("up-EDT-r15", "true");
    }
    if (idle_mode_meass_r15_present) {
      j.write_str("idleModeMeasurements-r15", "true");
    }
    if (reduced_cp_latency_enabled_r15_present) {
      j.write_str("reducedCP-LatencyEnabled-r15", "true");
    }
    if (mbms_rom_service_ind_r15_present) {
      j.write_str("mbms-ROM-ServiceIndication-r15", "true");
    }
    if (rlos_enabled_r16_present) {
      j.write_str("rlos-Enabled-r16", "true");
    }
    if (early_security_reactivation_r16_present) {
      j.write_str("earlySecurityReactivation-r16", "true");
    }
    if (cp_edt_minus5_gc_r16_present) {
      j.write_str("cp-EDT-5GC-r16", "true");
    }
    if (up_edt_minus5_gc_r16_present) {
      j.write_str("up-EDT-5GC-r16", "true");
    }
    if (cp_pur_epc_r16_present) {
      j.write_str("cp-PUR-EPC-r16", "true");
    }
    if (up_pur_epc_r16_present) {
      j.write_str("up-PUR-EPC-r16", "true");
    }
    if (cp_pur_minus5_gc_r16_present) {
      j.write_str("cp-PUR-5GC-r16", "true");
    }
    if (up_pur_minus5_gc_r16_present) {
      j.write_str("up-PUR-5GC-r16", "true");
    }
    if (mpdcch_cqi_report_r16_present) {
      j.write_str("mpdcch-CQI-Reporting-r16", mpdcch_cqi_report_r16.to_string());
    }
    if (rai_activation_enh_r16_present) {
      j.write_str("rai-ActivationEnh-r16", "true");
    }
    if (idle_mode_meass_nr_r16_present) {
      j.write_str("idleModeMeasurementsNR-r16", "true");
    }
  }
  j.end_obj();
}

const char* sib_type2_s::freq_info_s_::ul_bw_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "sib_type2_s::freq_info_s_::ul_bw_e_");
}
uint8_t sib_type2_s::freq_info_s_::ul_bw_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "sib_type2_s::freq_info_s_::ul_bw_e_");
}

const char* sib_type2_s::mpdcch_cqi_report_r16_opts::to_string() const
{
  static const char* options[] = {"fourBits", "both"};
  return convert_enum_idx(options, 2, value, "sib_type2_s::mpdcch_cqi_report_r16_e_");
}
uint8_t sib_type2_s::mpdcch_cqi_report_r16_opts::to_number() const
{
  static const uint8_t options[] = {4};
  return map_enum_number(options, 1, value, "sib_type2_s::mpdcch_cqi_report_r16_e_");
}

// SystemInformationBlockType1-v890-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v890_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE sib_type1_v890_ies_s::unpack(cbit_ref& bref)
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
void sib_type1_v890_ies_s::to_json(json_writer& j) const
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

// SystemInformationBlockType1 ::= SEQUENCE
SRSASN_CODE sib_type1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(p_max_present, 1));
  HANDLE_CODE(bref.pack(tdd_cfg_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.pack(cell_access_related_info.csg_id_present, 1));
  HANDLE_CODE(pack_dyn_seq_of(bref, cell_access_related_info.plmn_id_list, 1, 6));
  HANDLE_CODE(cell_access_related_info.tac.pack(bref));
  HANDLE_CODE(cell_access_related_info.cell_id.pack(bref));
  HANDLE_CODE(cell_access_related_info.cell_barred.pack(bref));
  HANDLE_CODE(cell_access_related_info.intra_freq_resel.pack(bref));
  HANDLE_CODE(bref.pack(cell_access_related_info.csg_ind, 1));
  if (cell_access_related_info.csg_id_present) {
    HANDLE_CODE(cell_access_related_info.csg_id.pack(bref));
  }
  HANDLE_CODE(bref.pack(cell_sel_info.q_rx_lev_min_offset_present, 1));
  HANDLE_CODE(pack_integer(bref, cell_sel_info.q_rx_lev_min, (int8_t)-70, (int8_t)-22));
  if (cell_sel_info.q_rx_lev_min_offset_present) {
    HANDLE_CODE(pack_integer(bref, cell_sel_info.q_rx_lev_min_offset, (uint8_t)1u, (uint8_t)8u));
  }
  if (p_max_present) {
    HANDLE_CODE(pack_integer(bref, p_max, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(pack_integer(bref, freq_band_ind, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_dyn_seq_of(bref, sched_info_list, 1, 32));
  if (tdd_cfg_present) {
    HANDLE_CODE(tdd_cfg.pack(bref));
  }
  HANDLE_CODE(si_win_len.pack(bref));
  HANDLE_CODE(pack_integer(bref, sys_info_value_tag, (uint8_t)0u, (uint8_t)31u));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(p_max_present, 1));
  HANDLE_CODE(bref.unpack(tdd_cfg_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.unpack(cell_access_related_info.csg_id_present, 1));
  HANDLE_CODE(unpack_dyn_seq_of(cell_access_related_info.plmn_id_list, bref, 1, 6));
  HANDLE_CODE(cell_access_related_info.tac.unpack(bref));
  HANDLE_CODE(cell_access_related_info.cell_id.unpack(bref));
  HANDLE_CODE(cell_access_related_info.cell_barred.unpack(bref));
  HANDLE_CODE(cell_access_related_info.intra_freq_resel.unpack(bref));
  HANDLE_CODE(bref.unpack(cell_access_related_info.csg_ind, 1));
  if (cell_access_related_info.csg_id_present) {
    HANDLE_CODE(cell_access_related_info.csg_id.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(cell_sel_info.q_rx_lev_min_offset_present, 1));
  HANDLE_CODE(unpack_integer(cell_sel_info.q_rx_lev_min, bref, (int8_t)-70, (int8_t)-22));
  if (cell_sel_info.q_rx_lev_min_offset_present) {
    HANDLE_CODE(unpack_integer(cell_sel_info.q_rx_lev_min_offset, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (p_max_present) {
    HANDLE_CODE(unpack_integer(p_max, bref, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(unpack_integer(freq_band_ind, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_dyn_seq_of(sched_info_list, bref, 1, 32));
  if (tdd_cfg_present) {
    HANDLE_CODE(tdd_cfg.unpack(bref));
  }
  HANDLE_CODE(si_win_len.unpack(bref));
  HANDLE_CODE(unpack_integer(sys_info_value_tag, bref, (uint8_t)0u, (uint8_t)31u));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellAccessRelatedInfo");
  j.start_obj();
  j.start_array("plmn-IdentityList");
  for (const auto& e1 : cell_access_related_info.plmn_id_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("trackingAreaCode", cell_access_related_info.tac.to_string());
  j.write_str("cellIdentity", cell_access_related_info.cell_id.to_string());
  j.write_str("cellBarred", cell_access_related_info.cell_barred.to_string());
  j.write_str("intraFreqReselection", cell_access_related_info.intra_freq_resel.to_string());
  j.write_bool("csg-Indication", cell_access_related_info.csg_ind);
  if (cell_access_related_info.csg_id_present) {
    j.write_str("csg-Identity", cell_access_related_info.csg_id.to_string());
  }
  j.end_obj();
  j.write_fieldname("cellSelectionInfo");
  j.start_obj();
  j.write_int("q-RxLevMin", cell_sel_info.q_rx_lev_min);
  if (cell_sel_info.q_rx_lev_min_offset_present) {
    j.write_int("q-RxLevMinOffset", cell_sel_info.q_rx_lev_min_offset);
  }
  j.end_obj();
  if (p_max_present) {
    j.write_int("p-Max", p_max);
  }
  j.write_int("freqBandIndicator", freq_band_ind);
  j.start_array("schedulingInfoList");
  for (const auto& e1 : sched_info_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (tdd_cfg_present) {
    j.write_fieldname("tdd-Config");
    tdd_cfg.to_json(j);
  }
  j.write_str("si-WindowLength", si_win_len.to_string());
  j.write_int("systemInfoValueTag", sys_info_value_tag);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* sib_type1_s::cell_access_related_info_s_::cell_barred_opts::to_string() const
{
  static const char* options[] = {"barred", "notBarred"};
  return convert_enum_idx(options, 2, value, "sib_type1_s::cell_access_related_info_s_::cell_barred_e_");
}

const char* sib_type1_s::cell_access_related_info_s_::intra_freq_resel_opts::to_string() const
{
  static const char* options[] = {"allowed", "notAllowed"};
  return convert_enum_idx(options, 2, value, "sib_type1_s::cell_access_related_info_s_::intra_freq_resel_e_");
}

const char* sib_type1_s::si_win_len_opts::to_string() const
{
  static const char* options[] = {"ms1", "ms2", "ms5", "ms10", "ms15", "ms20", "ms40"};
  return convert_enum_idx(options, 7, value, "sib_type1_s::si_win_len_e_");
}
uint8_t sib_type1_s::si_win_len_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 5, 10, 15, 20, 40};
  return map_enum_number(options, 7, value, "sib_type1_s::si_win_len_e_");
}
