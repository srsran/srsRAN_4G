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

#include "srsran/asn1/rrc/ho_cmd.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// SCG-Config-v13c0-IEs ::= SEQUENCE
SRSASN_CODE scg_cfg_v13c0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scg_radio_cfg_v13c0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (scg_radio_cfg_v13c0_present) {
    HANDLE_CODE(scg_radio_cfg_v13c0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_v13c0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scg_radio_cfg_v13c0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (scg_radio_cfg_v13c0_present) {
    HANDLE_CODE(scg_radio_cfg_v13c0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_cfg_v13c0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scg_radio_cfg_v13c0_present) {
    j.write_fieldname("scg-RadioConfig-v13c0");
    scg_radio_cfg_v13c0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SCG-Config-v12i0a-IEs ::= SEQUENCE
SRSASN_CODE scg_cfg_v12i0a_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE scg_cfg_v12i0a_ies_s::unpack(cbit_ref& bref)
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
void scg_cfg_v12i0a_ies_s::to_json(json_writer& j) const
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

// SCG-Config-r12-IEs ::= SEQUENCE
SRSASN_CODE scg_cfg_r12_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scg_radio_cfg_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (scg_radio_cfg_r12_present) {
    HANDLE_CODE(scg_radio_cfg_r12.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_r12_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scg_radio_cfg_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (scg_radio_cfg_r12_present) {
    HANDLE_CODE(scg_radio_cfg_r12.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_cfg_r12_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scg_radio_cfg_r12_present) {
    j.write_fieldname("scg-RadioConfig-r12");
    scg_radio_cfg_r12.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// AS-Config-v1550 ::= SEQUENCE
SRSASN_CODE as_cfg_v1550_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tdm_pattern_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(p_max_eutra_r15_present, 1));

  if (tdm_pattern_cfg_r15_present) {
    HANDLE_CODE(tdm_pattern_cfg_r15.sf_assign_r15.pack(bref));
    HANDLE_CODE(pack_integer(bref, tdm_pattern_cfg_r15.harq_offset_r15, (uint8_t)0u, (uint8_t)9u));
  }
  if (p_max_eutra_r15_present) {
    HANDLE_CODE(pack_integer(bref, p_max_eutra_r15, (int8_t)-30, (int8_t)33));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_v1550_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tdm_pattern_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(p_max_eutra_r15_present, 1));

  if (tdm_pattern_cfg_r15_present) {
    HANDLE_CODE(tdm_pattern_cfg_r15.sf_assign_r15.unpack(bref));
    HANDLE_CODE(unpack_integer(tdm_pattern_cfg_r15.harq_offset_r15, bref, (uint8_t)0u, (uint8_t)9u));
  }
  if (p_max_eutra_r15_present) {
    HANDLE_CODE(unpack_integer(p_max_eutra_r15, bref, (int8_t)-30, (int8_t)33));
  }

  return SRSASN_SUCCESS;
}
void as_cfg_v1550_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tdm_pattern_cfg_r15_present) {
    j.write_fieldname("tdm-PatternConfig-r15");
    j.start_obj();
    j.write_str("subframeAssignment-r15", tdm_pattern_cfg_r15.sf_assign_r15.to_string());
    j.write_int("harq-Offset-r15", tdm_pattern_cfg_r15.harq_offset_r15);
    j.end_obj();
  }
  if (p_max_eutra_r15_present) {
    j.write_int("p-MaxEUTRA-r15", p_max_eutra_r15);
  }
  j.end_obj();
}

// AS-ConfigNR-r15 ::= SEQUENCE
SRSASN_CODE as_cfg_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(source_rb_cfg_nr_r15_present, 1));
  HANDLE_CODE(bref.pack(source_rb_cfg_sn_nr_r15_present, 1));
  HANDLE_CODE(bref.pack(source_other_cfg_sn_nr_r15_present, 1));

  if (source_rb_cfg_nr_r15_present) {
    HANDLE_CODE(source_rb_cfg_nr_r15.pack(bref));
  }
  if (source_rb_cfg_sn_nr_r15_present) {
    HANDLE_CODE(source_rb_cfg_sn_nr_r15.pack(bref));
  }
  if (source_other_cfg_sn_nr_r15_present) {
    HANDLE_CODE(source_other_cfg_sn_nr_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(source_rb_cfg_nr_r15_present, 1));
  HANDLE_CODE(bref.unpack(source_rb_cfg_sn_nr_r15_present, 1));
  HANDLE_CODE(bref.unpack(source_other_cfg_sn_nr_r15_present, 1));

  if (source_rb_cfg_nr_r15_present) {
    HANDLE_CODE(source_rb_cfg_nr_r15.unpack(bref));
  }
  if (source_rb_cfg_sn_nr_r15_present) {
    HANDLE_CODE(source_rb_cfg_sn_nr_r15.unpack(bref));
  }
  if (source_other_cfg_sn_nr_r15_present) {
    HANDLE_CODE(source_other_cfg_sn_nr_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void as_cfg_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (source_rb_cfg_nr_r15_present) {
    j.write_str("sourceRB-ConfigNR-r15", source_rb_cfg_nr_r15.to_string());
  }
  if (source_rb_cfg_sn_nr_r15_present) {
    j.write_str("sourceRB-ConfigSN-NR-r15", source_rb_cfg_sn_nr_r15.to_string());
  }
  if (source_other_cfg_sn_nr_r15_present) {
    j.write_str("sourceOtherConfigSN-NR-r15", source_other_cfg_sn_nr_r15.to_string());
  }
  j.end_obj();
}

// AS-ConfigNR-v1570 ::= SEQUENCE
SRSASN_CODE as_cfg_nr_v1570_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_nr_v1570_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void as_cfg_nr_v1570_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sourceSCG-ConfiguredNR-r15", "true");
  j.end_obj();
}

// AS-ConfigNR-v1620 ::= SEQUENCE
SRSASN_CODE as_cfg_nr_v1620_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(tdm_pattern_cfg2_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_nr_v1620_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(tdm_pattern_cfg2_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void as_cfg_nr_v1620_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tdm-PatternConfig2-r16");
  tdm_pattern_cfg2_r16.to_json(j);
  j.end_obj();
}

// SCG-Config-r12 ::= SEQUENCE
SRSASN_CODE scg_cfg_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void scg_cfg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void scg_cfg_r12_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
scg_cfg_r12_s::crit_exts_c_::c1_c_& scg_cfg_r12_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void scg_cfg_r12_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void scg_cfg_r12_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "scg_cfg_r12_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE scg_cfg_r12_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_r12_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_r12_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "scg_cfg_r12_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void scg_cfg_r12_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
scg_cfg_r12_ies_s& scg_cfg_r12_s::crit_exts_c_::c1_c_::set_scg_cfg_r12()
{
  set(types::scg_cfg_r12);
  return c;
}
void scg_cfg_r12_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void scg_cfg_r12_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void scg_cfg_r12_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void scg_cfg_r12_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void scg_cfg_r12_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void scg_cfg_r12_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void scg_cfg_r12_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void scg_cfg_r12_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scg_cfg_r12:
      j.write_fieldname("scg-Config-r12");
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
      log_invalid_choice_id(type_, "scg_cfg_r12_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE scg_cfg_r12_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scg_cfg_r12:
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
      log_invalid_choice_id(type_, "scg_cfg_r12_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_r12_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scg_cfg_r12:
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
      log_invalid_choice_id(type_, "scg_cfg_r12_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// AS-Config ::= SEQUENCE
SRSASN_CODE as_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(source_meas_cfg.pack(bref));
  HANDLE_CODE(source_rr_cfg.pack(bref));
  HANDLE_CODE(source_security_algorithm_cfg.pack(bref));
  HANDLE_CODE(source_ue_id.pack(bref));
  HANDLE_CODE(source_mib.pack(bref));
  HANDLE_CODE(source_sib_type1.pack(bref));
  HANDLE_CODE(source_sib_type2.pack(bref));
  HANDLE_CODE(ant_info_common.pack(bref));
  HANDLE_CODE(pack_integer(bref, source_dl_carrier_freq, (uint32_t)0u, (uint32_t)65535u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= source_sib_type1_ext_present;
    group_flags[0] |= source_other_cfg_r9.is_present();
    group_flags[1] |= source_scell_cfg_list_r10.is_present();
    group_flags[2] |= source_cfg_scg_r12.is_present();
    group_flags[3] |= as_cfg_nr_r15.is_present();
    group_flags[4] |= as_cfg_v1550.is_present();
    group_flags[5] |= as_cfg_nr_v1570.is_present();
    group_flags[6] |= as_cfg_nr_v1620.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(source_sib_type1_ext_present, 1));
      if (source_sib_type1_ext_present) {
        HANDLE_CODE(source_sib_type1_ext.pack(bref));
      }
      HANDLE_CODE(source_other_cfg_r9->pack(bref));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(source_scell_cfg_list_r10.is_present(), 1));
      if (source_scell_cfg_list_r10.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *source_scell_cfg_list_r10, 1, 4));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(source_cfg_scg_r12.is_present(), 1));
      if (source_cfg_scg_r12.is_present()) {
        HANDLE_CODE(source_cfg_scg_r12->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(as_cfg_nr_r15.is_present(), 1));
      if (as_cfg_nr_r15.is_present()) {
        HANDLE_CODE(as_cfg_nr_r15->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(as_cfg_v1550.is_present(), 1));
      if (as_cfg_v1550.is_present()) {
        HANDLE_CODE(as_cfg_v1550->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(as_cfg_nr_v1570.is_present(), 1));
      if (as_cfg_nr_v1570.is_present()) {
        HANDLE_CODE(as_cfg_nr_v1570->pack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(as_cfg_nr_v1620.is_present(), 1));
      if (as_cfg_nr_v1620.is_present()) {
        HANDLE_CODE(as_cfg_nr_v1620->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(source_meas_cfg.unpack(bref));
  HANDLE_CODE(source_rr_cfg.unpack(bref));
  HANDLE_CODE(source_security_algorithm_cfg.unpack(bref));
  HANDLE_CODE(source_ue_id.unpack(bref));
  HANDLE_CODE(source_mib.unpack(bref));
  HANDLE_CODE(source_sib_type1.unpack(bref));
  HANDLE_CODE(source_sib_type2.unpack(bref));
  HANDLE_CODE(ant_info_common.unpack(bref));
  HANDLE_CODE(unpack_integer(source_dl_carrier_freq, bref, (uint32_t)0u, (uint32_t)65535u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(7);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(source_sib_type1_ext_present, 1));
      if (source_sib_type1_ext_present) {
        HANDLE_CODE(source_sib_type1_ext.unpack(bref));
      }
      HANDLE_CODE(source_other_cfg_r9->unpack(bref));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool source_scell_cfg_list_r10_present;
      HANDLE_CODE(bref.unpack(source_scell_cfg_list_r10_present, 1));
      source_scell_cfg_list_r10.set_present(source_scell_cfg_list_r10_present);
      if (source_scell_cfg_list_r10.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*source_scell_cfg_list_r10, bref, 1, 4));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool source_cfg_scg_r12_present;
      HANDLE_CODE(bref.unpack(source_cfg_scg_r12_present, 1));
      source_cfg_scg_r12.set_present(source_cfg_scg_r12_present);
      if (source_cfg_scg_r12.is_present()) {
        HANDLE_CODE(source_cfg_scg_r12->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool as_cfg_nr_r15_present;
      HANDLE_CODE(bref.unpack(as_cfg_nr_r15_present, 1));
      as_cfg_nr_r15.set_present(as_cfg_nr_r15_present);
      if (as_cfg_nr_r15.is_present()) {
        HANDLE_CODE(as_cfg_nr_r15->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool as_cfg_v1550_present;
      HANDLE_CODE(bref.unpack(as_cfg_v1550_present, 1));
      as_cfg_v1550.set_present(as_cfg_v1550_present);
      if (as_cfg_v1550.is_present()) {
        HANDLE_CODE(as_cfg_v1550->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool as_cfg_nr_v1570_present;
      HANDLE_CODE(bref.unpack(as_cfg_nr_v1570_present, 1));
      as_cfg_nr_v1570.set_present(as_cfg_nr_v1570_present);
      if (as_cfg_nr_v1570.is_present()) {
        HANDLE_CODE(as_cfg_nr_v1570->unpack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool as_cfg_nr_v1620_present;
      HANDLE_CODE(bref.unpack(as_cfg_nr_v1620_present, 1));
      as_cfg_nr_v1620.set_present(as_cfg_nr_v1620_present);
      if (as_cfg_nr_v1620.is_present()) {
        HANDLE_CODE(as_cfg_nr_v1620->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void as_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sourceMeasConfig");
  source_meas_cfg.to_json(j);
  j.write_fieldname("sourceRadioResourceConfig");
  source_rr_cfg.to_json(j);
  j.write_fieldname("sourceSecurityAlgorithmConfig");
  source_security_algorithm_cfg.to_json(j);
  j.write_str("sourceUE-Identity", source_ue_id.to_string());
  j.write_fieldname("sourceMasterInformationBlock");
  source_mib.to_json(j);
  j.write_fieldname("sourceSystemInformationBlockType1");
  source_sib_type1.to_json(j);
  j.write_fieldname("sourceSystemInformationBlockType2");
  source_sib_type2.to_json(j);
  j.write_fieldname("antennaInfoCommon");
  ant_info_common.to_json(j);
  j.write_int("sourceDl-CarrierFreq", source_dl_carrier_freq);
  if (ext) {
    if (source_sib_type1_ext_present) {
      j.write_str("sourceSystemInformationBlockType1Ext", source_sib_type1_ext.to_string());
    }
    j.write_fieldname("sourceOtherConfig-r9");
    source_other_cfg_r9->to_json(j);
    if (source_scell_cfg_list_r10.is_present()) {
      j.start_array("sourceSCellConfigList-r10");
      for (const auto& e1 : *source_scell_cfg_list_r10) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (source_cfg_scg_r12.is_present()) {
      j.write_fieldname("sourceConfigSCG-r12");
      source_cfg_scg_r12->to_json(j);
    }
    if (as_cfg_nr_r15.is_present()) {
      j.write_fieldname("as-ConfigNR-r15");
      as_cfg_nr_r15->to_json(j);
    }
    if (as_cfg_v1550.is_present()) {
      j.write_fieldname("as-Config-v1550");
      as_cfg_v1550->to_json(j);
    }
    if (as_cfg_nr_v1570.is_present()) {
      j.write_fieldname("as-ConfigNR-v1570");
      as_cfg_nr_v1570->to_json(j);
    }
    if (as_cfg_nr_v1620.is_present()) {
      j.write_fieldname("as-ConfigNR-v1620");
      as_cfg_nr_v1620->to_json(j);
    }
  }
  j.end_obj();
}

// AS-Config-v10j0 ::= SEQUENCE
SRSASN_CODE as_cfg_v10j0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ant_info_ded_pcell_v10i0_present, 1));

  if (ant_info_ded_pcell_v10i0_present) {
    HANDLE_CODE(ant_info_ded_pcell_v10i0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_v10j0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ant_info_ded_pcell_v10i0_present, 1));

  if (ant_info_ded_pcell_v10i0_present) {
    HANDLE_CODE(ant_info_ded_pcell_v10i0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void as_cfg_v10j0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ant_info_ded_pcell_v10i0_present) {
    j.write_fieldname("antennaInfoDedicatedPCell-v10i0");
    ant_info_ded_pcell_v10i0.to_json(j);
  }
  j.end_obj();
}

// AS-Config-v1250 ::= SEQUENCE
SRSASN_CODE as_cfg_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(source_wlan_offload_cfg_r12_present, 1));
  HANDLE_CODE(bref.pack(source_sl_comm_cfg_r12_present, 1));
  HANDLE_CODE(bref.pack(source_sl_disc_cfg_r12_present, 1));

  if (source_wlan_offload_cfg_r12_present) {
    HANDLE_CODE(source_wlan_offload_cfg_r12.pack(bref));
  }
  if (source_sl_comm_cfg_r12_present) {
    HANDLE_CODE(source_sl_comm_cfg_r12.pack(bref));
  }
  if (source_sl_disc_cfg_r12_present) {
    HANDLE_CODE(source_sl_disc_cfg_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(source_wlan_offload_cfg_r12_present, 1));
  HANDLE_CODE(bref.unpack(source_sl_comm_cfg_r12_present, 1));
  HANDLE_CODE(bref.unpack(source_sl_disc_cfg_r12_present, 1));

  if (source_wlan_offload_cfg_r12_present) {
    HANDLE_CODE(source_wlan_offload_cfg_r12.unpack(bref));
  }
  if (source_sl_comm_cfg_r12_present) {
    HANDLE_CODE(source_sl_comm_cfg_r12.unpack(bref));
  }
  if (source_sl_disc_cfg_r12_present) {
    HANDLE_CODE(source_sl_disc_cfg_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void as_cfg_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (source_wlan_offload_cfg_r12_present) {
    j.write_fieldname("sourceWlan-OffloadConfig-r12");
    source_wlan_offload_cfg_r12.to_json(j);
  }
  if (source_sl_comm_cfg_r12_present) {
    j.write_fieldname("sourceSL-CommConfig-r12");
    source_sl_comm_cfg_r12.to_json(j);
  }
  if (source_sl_disc_cfg_r12_present) {
    j.write_fieldname("sourceSL-DiscConfig-r12");
    source_sl_disc_cfg_r12.to_json(j);
  }
  j.end_obj();
}

// AS-Config-v1320 ::= SEQUENCE
SRSASN_CODE as_cfg_v1320_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(source_scell_cfg_list_r13_present, 1));
  HANDLE_CODE(bref.pack(source_rclwi_cfg_r13_present, 1));

  if (source_scell_cfg_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, source_scell_cfg_list_r13, 1, 31));
  }
  if (source_rclwi_cfg_r13_present) {
    HANDLE_CODE(source_rclwi_cfg_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_v1320_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(source_scell_cfg_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(source_rclwi_cfg_r13_present, 1));

  if (source_scell_cfg_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(source_scell_cfg_list_r13, bref, 1, 31));
  }
  if (source_rclwi_cfg_r13_present) {
    HANDLE_CODE(source_rclwi_cfg_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void as_cfg_v1320_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (source_scell_cfg_list_r13_present) {
    j.start_array("sourceSCellConfigList-r13");
    for (const auto& e1 : source_scell_cfg_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (source_rclwi_cfg_r13_present) {
    j.write_fieldname("sourceRCLWI-Configuration-r13");
    source_rclwi_cfg_r13.to_json(j);
  }
  j.end_obj();
}

// AS-Config-v13c0 ::= SEQUENCE
SRSASN_CODE as_cfg_v13c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rr_cfg_ded_v13c01_present, 1));
  HANDLE_CODE(bref.pack(rr_cfg_ded_v13c02_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_v13c0_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_ext_v13c0_present, 1));

  if (rr_cfg_ded_v13c01_present) {
    HANDLE_CODE(rr_cfg_ded_v13c01.pack(bref));
  }
  if (rr_cfg_ded_v13c02_present) {
    HANDLE_CODE(rr_cfg_ded_v13c02.pack(bref));
  }
  if (scell_to_add_mod_list_v13c0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_v13c0, 1, 4));
  }
  if (scell_to_add_mod_list_ext_v13c0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_ext_v13c0, 1, 31));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_v13c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rr_cfg_ded_v13c01_present, 1));
  HANDLE_CODE(bref.unpack(rr_cfg_ded_v13c02_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_v13c0_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_ext_v13c0_present, 1));

  if (rr_cfg_ded_v13c01_present) {
    HANDLE_CODE(rr_cfg_ded_v13c01.unpack(bref));
  }
  if (rr_cfg_ded_v13c02_present) {
    HANDLE_CODE(rr_cfg_ded_v13c02.unpack(bref));
  }
  if (scell_to_add_mod_list_v13c0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_v13c0, bref, 1, 4));
  }
  if (scell_to_add_mod_list_ext_v13c0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_ext_v13c0, bref, 1, 31));
  }

  return SRSASN_SUCCESS;
}
void as_cfg_v13c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rr_cfg_ded_v13c01_present) {
    j.write_fieldname("radioResourceConfigDedicated-v13c01");
    rr_cfg_ded_v13c01.to_json(j);
  }
  if (rr_cfg_ded_v13c02_present) {
    j.write_fieldname("radioResourceConfigDedicated-v13c02");
    rr_cfg_ded_v13c02.to_json(j);
  }
  if (scell_to_add_mod_list_v13c0_present) {
    j.start_array("sCellToAddModList-v13c0");
    for (const auto& e1 : scell_to_add_mod_list_v13c0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (scell_to_add_mod_list_ext_v13c0_present) {
    j.start_array("sCellToAddModListExt-v13c0");
    for (const auto& e1 : scell_to_add_mod_list_ext_v13c0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// AS-Config-v1430 ::= SEQUENCE
SRSASN_CODE as_cfg_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(source_sl_v2x_comm_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(source_lwa_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(source_wlan_meas_result_r14_present, 1));

  if (source_sl_v2x_comm_cfg_r14_present) {
    HANDLE_CODE(source_sl_v2x_comm_cfg_r14.pack(bref));
  }
  if (source_lwa_cfg_r14_present) {
    HANDLE_CODE(source_lwa_cfg_r14.pack(bref));
  }
  if (source_wlan_meas_result_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, source_wlan_meas_result_r14, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(source_sl_v2x_comm_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(source_lwa_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(source_wlan_meas_result_r14_present, 1));

  if (source_sl_v2x_comm_cfg_r14_present) {
    HANDLE_CODE(source_sl_v2x_comm_cfg_r14.unpack(bref));
  }
  if (source_lwa_cfg_r14_present) {
    HANDLE_CODE(source_lwa_cfg_r14.unpack(bref));
  }
  if (source_wlan_meas_result_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(source_wlan_meas_result_r14, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void as_cfg_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (source_sl_v2x_comm_cfg_r14_present) {
    j.write_fieldname("sourceSL-V2X-CommConfig-r14");
    source_sl_v2x_comm_cfg_r14.to_json(j);
  }
  if (source_lwa_cfg_r14_present) {
    j.write_fieldname("sourceLWA-Config-r14");
    source_lwa_cfg_r14.to_json(j);
  }
  if (source_wlan_meas_result_r14_present) {
    j.start_array("sourceWLAN-MeasResult-r14");
    for (const auto& e1 : source_wlan_meas_result_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// AS-Config-v9e0 ::= SEQUENCE
SRSASN_CODE as_cfg_v9e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, source_dl_carrier_freq_v9e0, (uint32_t)65536u, (uint32_t)262143u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_v9e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(source_dl_carrier_freq_v9e0, bref, (uint32_t)65536u, (uint32_t)262143u));

  return SRSASN_SUCCESS;
}
void as_cfg_v9e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sourceDl-CarrierFreq-v9e0", source_dl_carrier_freq_v9e0);
  j.end_obj();
}

// AdditionalReestabInfo ::= SEQUENCE
SRSASN_CODE add_reestab_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cell_id.pack(bref));
  HANDLE_CODE(key_e_node_b_star.pack(bref));
  HANDLE_CODE(short_mac_i.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE add_reestab_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cell_id.unpack(bref));
  HANDLE_CODE(key_e_node_b_star.unpack(bref));
  HANDLE_CODE(short_mac_i.unpack(bref));

  return SRSASN_SUCCESS;
}
void add_reestab_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cellIdentity", cell_id.to_string());
  j.write_str("key-eNodeB-Star", key_e_node_b_star.to_string());
  j.write_str("shortMAC-I", short_mac_i.to_string());
  j.end_obj();
}

// ReestablishmentInfo ::= SEQUENCE
SRSASN_CODE reest_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(add_reestab_info_list_present, 1));

  HANDLE_CODE(pack_integer(bref, source_pci, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(target_cell_short_mac_i.pack(bref));
  if (add_reestab_info_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, add_reestab_info_list, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE reest_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(add_reestab_info_list_present, 1));

  HANDLE_CODE(unpack_integer(source_pci, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(target_cell_short_mac_i.unpack(bref));
  if (add_reestab_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(add_reestab_info_list, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void reest_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sourcePhysCellId", source_pci);
  j.write_str("targetCellShortMAC-I", target_cell_short_mac_i.to_string());
  if (add_reestab_info_list_present) {
    j.start_array("additionalReestabInfoList");
    for (const auto& e1 : add_reestab_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// AS-Context ::= SEQUENCE
SRSASN_CODE as_context_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(reest_info_present, 1));

  if (reest_info_present) {
    HANDLE_CODE(reest_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_context_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(reest_info_present, 1));

  if (reest_info_present) {
    HANDLE_CODE(reest_info.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void as_context_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reest_info_present) {
    j.write_fieldname("reestablishmentInfo");
    reest_info.to_json(j);
  }
  j.end_obj();
}

// AS-Context-v1130 ::= SEQUENCE
SRSASN_CODE as_context_v1130_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(idc_ind_r11_present, 1));
  HANDLE_CODE(bref.pack(mbms_interest_ind_r11_present, 1));
  HANDLE_CODE(bref.pack(ue_assist_info_r11_present, 1));

  if (idc_ind_r11_present) {
    HANDLE_CODE(idc_ind_r11.pack(bref));
  }
  if (mbms_interest_ind_r11_present) {
    HANDLE_CODE(mbms_interest_ind_r11.pack(bref));
  }
  if (ue_assist_info_r11_present) {
    HANDLE_CODE(ue_assist_info_r11.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sidelink_ue_info_r12_present;
    group_flags[1] |= source_context_en_dc_r15_present;
    group_flags[2] |= selband_combination_info_en_dc_v1540_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sidelink_ue_info_r12_present, 1));
      if (sidelink_ue_info_r12_present) {
        HANDLE_CODE(sidelink_ue_info_r12.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(source_context_en_dc_r15_present, 1));
      if (source_context_en_dc_r15_present) {
        HANDLE_CODE(source_context_en_dc_r15.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(selband_combination_info_en_dc_v1540_present, 1));
      if (selband_combination_info_en_dc_v1540_present) {
        HANDLE_CODE(selband_combination_info_en_dc_v1540.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE as_context_v1130_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(idc_ind_r11_present, 1));
  HANDLE_CODE(bref.unpack(mbms_interest_ind_r11_present, 1));
  HANDLE_CODE(bref.unpack(ue_assist_info_r11_present, 1));

  if (idc_ind_r11_present) {
    HANDLE_CODE(idc_ind_r11.unpack(bref));
  }
  if (mbms_interest_ind_r11_present) {
    HANDLE_CODE(mbms_interest_ind_r11.unpack(bref));
  }
  if (ue_assist_info_r11_present) {
    HANDLE_CODE(ue_assist_info_r11.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sidelink_ue_info_r12_present, 1));
      if (sidelink_ue_info_r12_present) {
        HANDLE_CODE(sidelink_ue_info_r12.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(source_context_en_dc_r15_present, 1));
      if (source_context_en_dc_r15_present) {
        HANDLE_CODE(source_context_en_dc_r15.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(selband_combination_info_en_dc_v1540_present, 1));
      if (selband_combination_info_en_dc_v1540_present) {
        HANDLE_CODE(selband_combination_info_en_dc_v1540.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void as_context_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (idc_ind_r11_present) {
    j.write_str("idc-Indication-r11", idc_ind_r11.to_string());
  }
  if (mbms_interest_ind_r11_present) {
    j.write_str("mbmsInterestIndication-r11", mbms_interest_ind_r11.to_string());
  }
  if (ue_assist_info_r11_present) {
    j.write_str("ueAssistanceInformation-r11", ue_assist_info_r11.to_string());
  }
  if (ext) {
    if (sidelink_ue_info_r12_present) {
      j.write_str("sidelinkUEInformation-r12", sidelink_ue_info_r12.to_string());
    }
    if (source_context_en_dc_r15_present) {
      j.write_str("sourceContextEN-DC-r15", source_context_en_dc_r15.to_string());
    }
    if (selband_combination_info_en_dc_v1540_present) {
      j.write_str("selectedbandCombinationInfoEN-DC-v1540", selband_combination_info_en_dc_v1540.to_string());
    }
  }
  j.end_obj();
}

// AS-Context-v1320 ::= SEQUENCE
SRSASN_CODE as_context_v1320_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(wlan_conn_status_report_r13_present, 1));

  if (wlan_conn_status_report_r13_present) {
    HANDLE_CODE(wlan_conn_status_report_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_context_v1320_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(wlan_conn_status_report_r13_present, 1));

  if (wlan_conn_status_report_r13_present) {
    HANDLE_CODE(wlan_conn_status_report_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void as_context_v1320_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (wlan_conn_status_report_r13_present) {
    j.write_str("wlanConnectionStatusReport-r13", wlan_conn_status_report_r13.to_string());
  }
  j.end_obj();
}

// ConfigRestrictInfoDAPS-r16 ::= SEQUENCE
SRSASN_CODE cfg_restrict_info_daps_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(max_sch_tb_bits_dl_r16_present, 1));
  HANDLE_CODE(bref.pack(max_sch_tb_bits_ul_r16_present, 1));

  if (max_sch_tb_bits_dl_r16_present) {
    HANDLE_CODE(pack_integer(bref, max_sch_tb_bits_dl_r16, (uint8_t)1u, (uint8_t)100u));
  }
  if (max_sch_tb_bits_ul_r16_present) {
    HANDLE_CODE(pack_integer(bref, max_sch_tb_bits_ul_r16, (uint8_t)1u, (uint8_t)100u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_restrict_info_daps_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(max_sch_tb_bits_dl_r16_present, 1));
  HANDLE_CODE(bref.unpack(max_sch_tb_bits_ul_r16_present, 1));

  if (max_sch_tb_bits_dl_r16_present) {
    HANDLE_CODE(unpack_integer(max_sch_tb_bits_dl_r16, bref, (uint8_t)1u, (uint8_t)100u));
  }
  if (max_sch_tb_bits_ul_r16_present) {
    HANDLE_CODE(unpack_integer(max_sch_tb_bits_ul_r16, bref, (uint8_t)1u, (uint8_t)100u));
  }

  return SRSASN_SUCCESS;
}
void cfg_restrict_info_daps_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (max_sch_tb_bits_dl_r16_present) {
    j.write_int("maxSCH-TB-BitsDL-r16", max_sch_tb_bits_dl_r16);
  }
  if (max_sch_tb_bits_ul_r16_present) {
    j.write_int("maxSCH-TB-BitsUL-r16", max_sch_tb_bits_ul_r16);
  }
  j.end_obj();
}

// AS-Context-v1610 ::= SEQUENCE
SRSASN_CODE as_context_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sidelink_ue_info_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(ue_assist_info_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(cfg_restrict_info_daps_r16_present, 1));

  if (sidelink_ue_info_nr_r16_present) {
    HANDLE_CODE(sidelink_ue_info_nr_r16.pack(bref));
  }
  if (ue_assist_info_nr_r16_present) {
    HANDLE_CODE(ue_assist_info_nr_r16.pack(bref));
  }
  if (cfg_restrict_info_daps_r16_present) {
    HANDLE_CODE(cfg_restrict_info_daps_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_context_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sidelink_ue_info_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(ue_assist_info_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(cfg_restrict_info_daps_r16_present, 1));

  if (sidelink_ue_info_nr_r16_present) {
    HANDLE_CODE(sidelink_ue_info_nr_r16.unpack(bref));
  }
  if (ue_assist_info_nr_r16_present) {
    HANDLE_CODE(ue_assist_info_nr_r16.unpack(bref));
  }
  if (cfg_restrict_info_daps_r16_present) {
    HANDLE_CODE(cfg_restrict_info_daps_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void as_context_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sidelink_ue_info_nr_r16_present) {
    j.write_str("sidelinkUEInformationNR-r16", sidelink_ue_info_nr_r16.to_string());
  }
  if (ue_assist_info_nr_r16_present) {
    j.write_str("ueAssistanceInformationNR-r16", ue_assist_info_nr_r16.to_string());
  }
  if (cfg_restrict_info_daps_r16_present) {
    j.write_fieldname("configRestrictInfoDAPS-r16");
    cfg_restrict_info_daps_r16.to_json(j);
  }
  j.end_obj();
}

// ConfigRestrictInfoDAPS-v1630 ::= SEQUENCE
SRSASN_CODE cfg_restrict_info_daps_v1630_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(daps_pwr_coordination_info_r16_present, 1));

  if (daps_pwr_coordination_info_r16_present) {
    HANDLE_CODE(daps_pwr_coordination_info_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_restrict_info_daps_v1630_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(daps_pwr_coordination_info_r16_present, 1));

  if (daps_pwr_coordination_info_r16_present) {
    HANDLE_CODE(daps_pwr_coordination_info_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cfg_restrict_info_daps_v1630_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (daps_pwr_coordination_info_r16_present) {
    j.write_fieldname("daps-PowerCoordinationInfo-r16");
    daps_pwr_coordination_info_r16.to_json(j);
  }
  j.end_obj();
}

// AS-Context-v1630 ::= SEQUENCE
SRSASN_CODE as_context_v1630_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cfg_restrict_info_daps_v1630_present, 1));

  if (cfg_restrict_info_daps_v1630_present) {
    HANDLE_CODE(cfg_restrict_info_daps_v1630.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_context_v1630_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cfg_restrict_info_daps_v1630_present, 1));

  if (cfg_restrict_info_daps_v1630_present) {
    HANDLE_CODE(cfg_restrict_info_daps_v1630.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void as_context_v1630_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cfg_restrict_info_daps_v1630_present) {
    j.write_fieldname("configRestrictInfoDAPS-v1630");
    cfg_restrict_info_daps_v1630.to_json(j);
  }
  j.end_obj();
}

// CandidateCellInfo-r10 ::= SEQUENCE
SRSASN_CODE candidate_cell_info_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rsrp_result_r10_present, 1));
  HANDLE_CODE(bref.pack(rsrq_result_r10_present, 1));

  HANDLE_CODE(pack_integer(bref, pci_r10, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(pack_integer(bref, dl_carrier_freq_r10, (uint32_t)0u, (uint32_t)65535u));
  if (rsrp_result_r10_present) {
    HANDLE_CODE(pack_integer(bref, rsrp_result_r10, (uint8_t)0u, (uint8_t)97u));
  }
  if (rsrq_result_r10_present) {
    HANDLE_CODE(pack_integer(bref, rsrq_result_r10, (uint8_t)0u, (uint8_t)34u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= dl_carrier_freq_v1090_present;
    group_flags[1] |= rsrq_result_v1250_present;
    group_flags[2] |= rs_sinr_result_r13_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(dl_carrier_freq_v1090_present, 1));
      if (dl_carrier_freq_v1090_present) {
        HANDLE_CODE(pack_integer(bref, dl_carrier_freq_v1090, (uint32_t)65536u, (uint32_t)262143u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rsrq_result_v1250_present, 1));
      if (rsrq_result_v1250_present) {
        HANDLE_CODE(pack_integer(bref, rsrq_result_v1250, (int8_t)-30, (int8_t)46));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rs_sinr_result_r13_present, 1));
      if (rs_sinr_result_r13_present) {
        HANDLE_CODE(pack_integer(bref, rs_sinr_result_r13, (uint8_t)0u, (uint8_t)127u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE candidate_cell_info_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rsrp_result_r10_present, 1));
  HANDLE_CODE(bref.unpack(rsrq_result_r10_present, 1));

  HANDLE_CODE(unpack_integer(pci_r10, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(unpack_integer(dl_carrier_freq_r10, bref, (uint32_t)0u, (uint32_t)65535u));
  if (rsrp_result_r10_present) {
    HANDLE_CODE(unpack_integer(rsrp_result_r10, bref, (uint8_t)0u, (uint8_t)97u));
  }
  if (rsrq_result_r10_present) {
    HANDLE_CODE(unpack_integer(rsrq_result_r10, bref, (uint8_t)0u, (uint8_t)34u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(dl_carrier_freq_v1090_present, 1));
      if (dl_carrier_freq_v1090_present) {
        HANDLE_CODE(unpack_integer(dl_carrier_freq_v1090, bref, (uint32_t)65536u, (uint32_t)262143u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(rsrq_result_v1250_present, 1));
      if (rsrq_result_v1250_present) {
        HANDLE_CODE(unpack_integer(rsrq_result_v1250, bref, (int8_t)-30, (int8_t)46));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(rs_sinr_result_r13_present, 1));
      if (rs_sinr_result_r13_present) {
        HANDLE_CODE(unpack_integer(rs_sinr_result_r13, bref, (uint8_t)0u, (uint8_t)127u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void candidate_cell_info_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r10", pci_r10);
  j.write_int("dl-CarrierFreq-r10", dl_carrier_freq_r10);
  if (rsrp_result_r10_present) {
    j.write_int("rsrpResult-r10", rsrp_result_r10);
  }
  if (rsrq_result_r10_present) {
    j.write_int("rsrqResult-r10", rsrq_result_r10);
  }
  if (ext) {
    if (dl_carrier_freq_v1090_present) {
      j.write_int("dl-CarrierFreq-v1090", dl_carrier_freq_v1090);
    }
    if (rsrq_result_v1250_present) {
      j.write_int("rsrqResult-v1250", rsrq_result_v1250);
    }
    if (rs_sinr_result_r13_present) {
      j.write_int("rs-sinr-Result-r13", rs_sinr_result_r13);
    }
  }
  j.end_obj();
}

// HandoverCommand-r8-IEs ::= SEQUENCE
SRSASN_CODE ho_cmd_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ho_cmd_msg.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_cmd_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ho_cmd_msg.unpack(bref));

  return SRSASN_SUCCESS;
}
void ho_cmd_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("handoverCommandMessage", ho_cmd_msg.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// HandoverCommand ::= SEQUENCE
SRSASN_CODE ho_cmd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_cmd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ho_cmd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ho_cmd_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ho_cmd_s::crit_exts_c_::c1_c_& ho_cmd_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ho_cmd_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ho_cmd_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_cmd_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_cmd_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ho_cmd_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ho_cmd_r8_ies_s& ho_cmd_s::crit_exts_c_::c1_c_::set_ho_cmd_r8()
{
  set(types::ho_cmd_r8);
  return c;
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ho_cmd_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho_cmd_r8:
      j.write_fieldname("handoverCommand-r8");
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
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_cmd_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ho_cmd_r8:
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
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_cmd_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ho_cmd_r8:
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
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// AS-Config-v1700 ::= SEQUENCE
SRSASN_CODE as_cfg_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scg_state_r17_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scg_state_r17_present, 1));

  return SRSASN_SUCCESS;
}
void as_cfg_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scg_state_r17_present) {
    j.write_str("scg-State-r17", "deactivated");
  }
  j.end_obj();
}

// HandoverPreparationInformation-v1700-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(as_cfg_v1700_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (as_cfg_v1700_present) {
    HANDLE_CODE(as_cfg_v1700.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(as_cfg_v1700_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (as_cfg_v1700_present) {
    HANDLE_CODE(as_cfg_v1700.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (as_cfg_v1700_present) {
    j.write_fieldname("as-Config-v1700");
    as_cfg_v1700.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// AS-Context-v1620 ::= SEQUENCE
SRSASN_CODE as_context_v1620_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_assist_info_nr_scg_r16_present, 1));

  if (ue_assist_info_nr_scg_r16_present) {
    HANDLE_CODE(ue_assist_info_nr_scg_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE as_context_v1620_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_assist_info_nr_scg_r16_present, 1));

  if (ue_assist_info_nr_scg_r16_present) {
    HANDLE_CODE(ue_assist_info_nr_scg_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void as_context_v1620_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_assist_info_nr_scg_r16_present) {
    j.write_str("ueAssistanceInformationNR-SCG-r16", ue_assist_info_nr_scg_r16.to_string());
  }
  j.end_obj();
}

// HandoverPreparationInformation-v1630-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v1630_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(as_context_v1630_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (as_context_v1630_present) {
    HANDLE_CODE(as_context_v1630.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_v1630_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(as_context_v1630_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (as_context_v1630_present) {
    HANDLE_CODE(as_context_v1630.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_v1630_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (as_context_v1630_present) {
    j.write_fieldname("as-Context-v1630");
    as_context_v1630.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// HandoverPreparationInformation-v1620-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v1620_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(as_context_v1620_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (as_context_v1620_present) {
    HANDLE_CODE(as_context_v1620.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_v1620_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(as_context_v1620_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (as_context_v1620_present) {
    HANDLE_CODE(as_context_v1620.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_v1620_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (as_context_v1620_present) {
    j.write_fieldname("as-Context-v1620");
    as_context_v1620.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// HandoverPreparationInformation-v1610-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(as_context_v1610_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (as_context_v1610_present) {
    HANDLE_CODE(as_context_v1610.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(as_context_v1610_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (as_context_v1610_present) {
    HANDLE_CODE(as_context_v1610.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (as_context_v1610_present) {
    j.write_fieldname("as-Context-v1610");
    as_context_v1610.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// HandoverPreparationInformation-v1540-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(source_rb_cfg_intra5_gc_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (source_rb_cfg_intra5_gc_r15_present) {
    HANDLE_CODE(source_rb_cfg_intra5_gc_r15.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_v1540_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(source_rb_cfg_intra5_gc_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (source_rb_cfg_intra5_gc_r15_present) {
    HANDLE_CODE(source_rb_cfg_intra5_gc_r15.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (source_rb_cfg_intra5_gc_r15_present) {
    j.write_str("sourceRB-ConfigIntra5GC-r15", source_rb_cfg_intra5_gc_r15.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// HandoverPreparationInformation-v1530-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ran_notif_area_info_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ran_notif_area_info_r15_present) {
    HANDLE_CODE(ran_notif_area_info_r15.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ran_notif_area_info_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ran_notif_area_info_r15_present) {
    HANDLE_CODE(ran_notif_area_info_r15.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ran_notif_area_info_r15_present) {
    j.write_fieldname("ran-NotificationAreaInfo-r15");
    ran_notif_area_info_r15.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// HandoverPreparationInformation-v1430-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(as_cfg_v1430_present, 1));
  HANDLE_CODE(bref.pack(make_before_break_req_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (as_cfg_v1430_present) {
    HANDLE_CODE(as_cfg_v1430.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(as_cfg_v1430_present, 1));
  HANDLE_CODE(bref.unpack(make_before_break_req_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (as_cfg_v1430_present) {
    HANDLE_CODE(as_cfg_v1430.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (as_cfg_v1430_present) {
    j.write_fieldname("as-Config-v1430");
    as_cfg_v1430.to_json(j);
  }
  if (make_before_break_req_r14_present) {
    j.write_str("makeBeforeBreakReq-r14", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// HandoverPreparationInformation-v1320-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v1320_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(as_cfg_v1320_present, 1));
  HANDLE_CODE(bref.pack(as_context_v1320_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (as_cfg_v1320_present) {
    HANDLE_CODE(as_cfg_v1320.pack(bref));
  }
  if (as_context_v1320_present) {
    HANDLE_CODE(as_context_v1320.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_v1320_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(as_cfg_v1320_present, 1));
  HANDLE_CODE(bref.unpack(as_context_v1320_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (as_cfg_v1320_present) {
    HANDLE_CODE(as_cfg_v1320.unpack(bref));
  }
  if (as_context_v1320_present) {
    HANDLE_CODE(as_context_v1320.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_v1320_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (as_cfg_v1320_present) {
    j.write_fieldname("as-Config-v1320");
    as_cfg_v1320.to_json(j);
  }
  if (as_context_v1320_present) {
    j.write_fieldname("as-Context-v1320");
    as_context_v1320.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// HandoverPreparationInformation-v1250-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v1250_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_supported_earfcn_r12_present, 1));
  HANDLE_CODE(bref.pack(as_cfg_v1250_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_supported_earfcn_r12_present) {
    HANDLE_CODE(pack_integer(bref, ue_supported_earfcn_r12, (uint32_t)0u, (uint32_t)262143u));
  }
  if (as_cfg_v1250_present) {
    HANDLE_CODE(as_cfg_v1250.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_v1250_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_supported_earfcn_r12_present, 1));
  HANDLE_CODE(bref.unpack(as_cfg_v1250_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_supported_earfcn_r12_present) {
    HANDLE_CODE(unpack_integer(ue_supported_earfcn_r12, bref, (uint32_t)0u, (uint32_t)262143u));
  }
  if (as_cfg_v1250_present) {
    HANDLE_CODE(as_cfg_v1250.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_v1250_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_supported_earfcn_r12_present) {
    j.write_int("ue-SupportedEARFCN-r12", ue_supported_earfcn_r12);
  }
  if (as_cfg_v1250_present) {
    j.write_fieldname("as-Config-v1250");
    as_cfg_v1250.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// HandoverPreparationInformation-v1130-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v1130_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(as_context_v1130_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (as_context_v1130_present) {
    HANDLE_CODE(as_context_v1130.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_v1130_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(as_context_v1130_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (as_context_v1130_present) {
    HANDLE_CODE(as_context_v1130.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_v1130_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (as_context_v1130_present) {
    j.write_fieldname("as-Context-v1130");
    as_context_v1130.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// HandoverPreparationInformation-v9e0-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v9e0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(as_cfg_v9e0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (as_cfg_v9e0_present) {
    HANDLE_CODE(as_cfg_v9e0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_v9e0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(as_cfg_v9e0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (as_cfg_v9e0_present) {
    HANDLE_CODE(as_cfg_v9e0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_v9e0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (as_cfg_v9e0_present) {
    j.write_fieldname("as-Config-v9e0");
    as_cfg_v9e0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// HandoverPreparationInformation-v9d0-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v9d0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE ho_prep_info_v9d0_ies_s::unpack(cbit_ref& bref)
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
void ho_prep_info_v9d0_ies_s::to_json(json_writer& j) const
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

// HandoverPreparationInformation-v920-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v920_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_cfg_release_r9_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_cfg_release_r9_present) {
    HANDLE_CODE(ue_cfg_release_r9.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_v920_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_cfg_release_r9_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_cfg_release_r9_present) {
    HANDLE_CODE(ue_cfg_release_r9.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_v920_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_cfg_release_r9_present) {
    j.write_str("ue-ConfigRelease-r9", ue_cfg_release_r9.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* ho_prep_info_v920_ies_s::ue_cfg_release_r9_opts::to_string() const
{
  static const char* options[] = {
      "rel9", "rel10", "rel11", "rel12", "v10j0", "v11e0", "v1280", "rel13", "rel14", "rel15", "rel16", "rel17"};
  return convert_enum_idx(options, 12, value, "ho_prep_info_v920_ies_s::ue_cfg_release_r9_e_");
}

// RRM-Config ::= SEQUENCE
SRSASN_CODE rrm_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ue_inactive_time_present, 1));

  if (ue_inactive_time_present) {
    HANDLE_CODE(ue_inactive_time.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= candidate_cell_info_list_r10.is_present();
    group_flags[1] |= candidate_cell_info_list_nr_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(candidate_cell_info_list_r10.is_present(), 1));
      if (candidate_cell_info_list_r10.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *candidate_cell_info_list_r10, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(candidate_cell_info_list_nr_r15.is_present(), 1));
      if (candidate_cell_info_list_nr_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *candidate_cell_info_list_nr_r15, 1, 32));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrm_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ue_inactive_time_present, 1));

  if (ue_inactive_time_present) {
    HANDLE_CODE(ue_inactive_time.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool candidate_cell_info_list_r10_present;
      HANDLE_CODE(bref.unpack(candidate_cell_info_list_r10_present, 1));
      candidate_cell_info_list_r10.set_present(candidate_cell_info_list_r10_present);
      if (candidate_cell_info_list_r10.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*candidate_cell_info_list_r10, bref, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool candidate_cell_info_list_nr_r15_present;
      HANDLE_CODE(bref.unpack(candidate_cell_info_list_nr_r15_present, 1));
      candidate_cell_info_list_nr_r15.set_present(candidate_cell_info_list_nr_r15_present);
      if (candidate_cell_info_list_nr_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*candidate_cell_info_list_nr_r15, bref, 1, 32));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rrm_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_inactive_time_present) {
    j.write_str("ue-InactiveTime", ue_inactive_time.to_string());
  }
  if (ext) {
    if (candidate_cell_info_list_r10.is_present()) {
      j.start_array("candidateCellInfoList-r10");
      for (const auto& e1 : *candidate_cell_info_list_r10) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (candidate_cell_info_list_nr_r15.is_present()) {
      j.start_array("candidateCellInfoListNR-r15");
      for (const auto& e1 : *candidate_cell_info_list_nr_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* rrm_cfg_s::ue_inactive_time_opts::to_string() const
{
  static const char* options[] = {
      "s1",    "s2",       "s3",    "s5",           "s7",      "s10",   "s15",     "s20",      "s25",     "s30",
      "s40",   "s50",      "min1",  "min1s20c",     "min1s40", "min2",  "min2s30", "min3",     "min3s30", "min4",
      "min5",  "min6",     "min7",  "min8",         "min9",    "min10", "min12",   "min14",    "min17",   "min20",
      "min24", "min28",    "min33", "min38",        "min44",   "min50", "hr1",     "hr1min30", "hr2",     "hr2min30",
      "hr3",   "hr3min30", "hr4",   "hr5",          "hr6",     "hr8",   "hr10",    "hr13",     "hr16",    "hr20",
      "day1",  "day1hr12", "day2",  "day2hr12",     "day3",    "day4",  "day5",    "day7",     "day10",   "day14",
      "day19", "day24",    "day30", "dayMoreThan30"};
  return convert_enum_idx(options, 64, value, "rrm_cfg_s::ue_inactive_time_e_");
}

// HandoverPreparationInformation-r8-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(as_cfg_present, 1));
  HANDLE_CODE(bref.pack(rrm_cfg_present, 1));
  HANDLE_CODE(bref.pack(as_context_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, ue_radio_access_cap_info, 0, 8));
  if (as_cfg_present) {
    HANDLE_CODE(as_cfg.pack(bref));
  }
  if (rrm_cfg_present) {
    HANDLE_CODE(rrm_cfg.pack(bref));
  }
  if (as_context_present) {
    HANDLE_CODE(as_context.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(as_cfg_present, 1));
  HANDLE_CODE(bref.unpack(rrm_cfg_present, 1));
  HANDLE_CODE(bref.unpack(as_context_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(ue_radio_access_cap_info, bref, 0, 8));
  if (as_cfg_present) {
    HANDLE_CODE(as_cfg.unpack(bref));
  }
  if (rrm_cfg_present) {
    HANDLE_CODE(rrm_cfg.unpack(bref));
  }
  if (as_context_present) {
    HANDLE_CODE(as_context.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("ue-RadioAccessCapabilityInfo");
  for (const auto& e1 : ue_radio_access_cap_info) {
    e1.to_json(j);
  }
  j.end_array();
  if (as_cfg_present) {
    j.write_fieldname("as-Config");
    as_cfg.to_json(j);
  }
  if (rrm_cfg_present) {
    j.write_fieldname("rrm-Config");
    rrm_cfg.to_json(j);
  }
  if (as_context_present) {
    j.write_fieldname("as-Context");
    as_context.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// HandoverPreparationInformation ::= SEQUENCE
SRSASN_CODE ho_prep_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ho_prep_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ho_prep_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ho_prep_info_s::crit_exts_c_::c1_c_& ho_prep_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ho_prep_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ho_prep_info_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_prep_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ho_prep_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ho_prep_info_r8_ies_s& ho_prep_info_s::crit_exts_c_::c1_c_::set_ho_prep_info_r8()
{
  set(types::ho_prep_info_r8);
  return c;
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho_prep_info_r8:
      j.write_fieldname("handoverPreparationInformation-r8");
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
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_prep_info_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ho_prep_info_r8:
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
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ho_prep_info_r8:
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
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// HandoverPreparationInformation-v13c0-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v13c0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(as_cfg_v13c0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (as_cfg_v13c0_present) {
    HANDLE_CODE(as_cfg_v13c0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_v13c0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(as_cfg_v13c0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (as_cfg_v13c0_present) {
    HANDLE_CODE(as_cfg_v13c0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_v13c0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (as_cfg_v13c0_present) {
    j.write_fieldname("as-Config-v13c0");
    as_cfg_v13c0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// HandoverPreparationInformation-v10x0-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v10x0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE ho_prep_info_v10x0_ies_s::unpack(cbit_ref& bref)
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
void ho_prep_info_v10x0_ies_s::to_json(json_writer& j) const
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

// HandoverPreparationInformation-v10j0-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v10j0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(as_cfg_v10j0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (as_cfg_v10j0_present) {
    HANDLE_CODE(as_cfg_v10j0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_v10j0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(as_cfg_v10j0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (as_cfg_v10j0_present) {
    HANDLE_CODE(as_cfg_v10j0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_v10j0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (as_cfg_v10j0_present) {
    j.write_fieldname("as-Config-v10j0");
    as_cfg_v10j0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// HandoverPreparationInformation-v9j0-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_v9j0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE ho_prep_info_v9j0_ies_s::unpack(cbit_ref& bref)
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
void ho_prep_info_v9j0_ies_s::to_json(json_writer& j) const
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

// VarMeasConfig ::= SEQUENCE
SRSASN_CODE var_meas_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_id_list_present, 1));
  HANDLE_CODE(bref.pack(meas_id_list_ext_r12_present, 1));
  HANDLE_CODE(bref.pack(meas_id_list_v1310_present, 1));
  HANDLE_CODE(bref.pack(meas_id_list_ext_v1310_present, 1));
  HANDLE_CODE(bref.pack(meas_obj_list_present, 1));
  HANDLE_CODE(bref.pack(meas_obj_list_ext_r13_present, 1));
  HANDLE_CODE(bref.pack(meas_obj_list_v9i0_present, 1));
  HANDLE_CODE(bref.pack(report_cfg_list_present, 1));
  HANDLE_CODE(bref.pack(quant_cfg_present, 1));
  HANDLE_CODE(bref.pack(meas_scale_factor_r12_present, 1));
  HANDLE_CODE(bref.pack(s_measure_present, 1));
  HANDLE_CODE(bref.pack(speed_state_pars_present, 1));
  HANDLE_CODE(bref.pack(allow_interruptions_r11_present, 1));

  if (meas_id_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_id_list, 1, 32));
  }
  if (meas_id_list_ext_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_id_list_ext_r12, 1, 32));
  }
  if (meas_id_list_v1310_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_id_list_v1310, 1, 32));
  }
  if (meas_id_list_ext_v1310_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_id_list_ext_v1310, 1, 32));
  }
  if (meas_obj_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_obj_list, 1, 32));
  }
  if (meas_obj_list_ext_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_obj_list_ext_r13, 1, 32));
  }
  if (meas_obj_list_v9i0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_obj_list_v9i0, 1, 32));
  }
  if (report_cfg_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, report_cfg_list, 1, 32));
  }
  if (quant_cfg_present) {
    HANDLE_CODE(quant_cfg.pack(bref));
  }
  if (meas_scale_factor_r12_present) {
    HANDLE_CODE(meas_scale_factor_r12.pack(bref));
  }
  if (s_measure_present) {
    HANDLE_CODE(pack_integer(bref, s_measure, (int16_t)-140, (int16_t)-44));
  }
  if (speed_state_pars_present) {
    HANDLE_CODE(speed_state_pars.pack(bref));
  }
  if (allow_interruptions_r11_present) {
    HANDLE_CODE(bref.pack(allow_interruptions_r11, 1));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_id_list_present, 1));
  HANDLE_CODE(bref.unpack(meas_id_list_ext_r12_present, 1));
  HANDLE_CODE(bref.unpack(meas_id_list_v1310_present, 1));
  HANDLE_CODE(bref.unpack(meas_id_list_ext_v1310_present, 1));
  HANDLE_CODE(bref.unpack(meas_obj_list_present, 1));
  HANDLE_CODE(bref.unpack(meas_obj_list_ext_r13_present, 1));
  HANDLE_CODE(bref.unpack(meas_obj_list_v9i0_present, 1));
  HANDLE_CODE(bref.unpack(report_cfg_list_present, 1));
  HANDLE_CODE(bref.unpack(quant_cfg_present, 1));
  HANDLE_CODE(bref.unpack(meas_scale_factor_r12_present, 1));
  HANDLE_CODE(bref.unpack(s_measure_present, 1));
  HANDLE_CODE(bref.unpack(speed_state_pars_present, 1));
  HANDLE_CODE(bref.unpack(allow_interruptions_r11_present, 1));

  if (meas_id_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_id_list, bref, 1, 32));
  }
  if (meas_id_list_ext_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_id_list_ext_r12, bref, 1, 32));
  }
  if (meas_id_list_v1310_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_id_list_v1310, bref, 1, 32));
  }
  if (meas_id_list_ext_v1310_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_id_list_ext_v1310, bref, 1, 32));
  }
  if (meas_obj_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_obj_list, bref, 1, 32));
  }
  if (meas_obj_list_ext_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_obj_list_ext_r13, bref, 1, 32));
  }
  if (meas_obj_list_v9i0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_obj_list_v9i0, bref, 1, 32));
  }
  if (report_cfg_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(report_cfg_list, bref, 1, 32));
  }
  if (quant_cfg_present) {
    HANDLE_CODE(quant_cfg.unpack(bref));
  }
  if (meas_scale_factor_r12_present) {
    HANDLE_CODE(meas_scale_factor_r12.unpack(bref));
  }
  if (s_measure_present) {
    HANDLE_CODE(unpack_integer(s_measure, bref, (int16_t)-140, (int16_t)-44));
  }
  if (speed_state_pars_present) {
    HANDLE_CODE(speed_state_pars.unpack(bref));
  }
  if (allow_interruptions_r11_present) {
    HANDLE_CODE(bref.unpack(allow_interruptions_r11, 1));
  }

  return SRSASN_SUCCESS;
}
void var_meas_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_id_list_present) {
    j.start_array("measIdList");
    for (const auto& e1 : meas_id_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_id_list_ext_r12_present) {
    j.start_array("measIdListExt-r12");
    for (const auto& e1 : meas_id_list_ext_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_id_list_v1310_present) {
    j.start_array("measIdList-v1310");
    for (const auto& e1 : meas_id_list_v1310) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_id_list_ext_v1310_present) {
    j.start_array("measIdListExt-v1310");
    for (const auto& e1 : meas_id_list_ext_v1310) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_obj_list_present) {
    j.start_array("measObjectList");
    for (const auto& e1 : meas_obj_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_obj_list_ext_r13_present) {
    j.start_array("measObjectListExt-r13");
    for (const auto& e1 : meas_obj_list_ext_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_obj_list_v9i0_present) {
    j.start_array("measObjectList-v9i0");
    for (const auto& e1 : meas_obj_list_v9i0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (report_cfg_list_present) {
    j.start_array("reportConfigList");
    for (const auto& e1 : report_cfg_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (quant_cfg_present) {
    j.write_fieldname("quantityConfig");
    quant_cfg.to_json(j);
  }
  if (meas_scale_factor_r12_present) {
    j.write_str("measScaleFactor-r12", meas_scale_factor_r12.to_string());
  }
  if (s_measure_present) {
    j.write_int("s-Measure", s_measure);
  }
  if (speed_state_pars_present) {
    j.write_fieldname("speedStatePars");
    speed_state_pars.to_json(j);
  }
  if (allow_interruptions_r11_present) {
    j.write_bool("allowInterruptions-r11", allow_interruptions_r11);
  }
  j.end_obj();
}

void var_meas_cfg_s::speed_state_pars_c_::set(types::options e)
{
  type_ = e;
}
void var_meas_cfg_s::speed_state_pars_c_::set_release()
{
  set(types::release);
}
var_meas_cfg_s::speed_state_pars_c_::setup_s_& var_meas_cfg_s::speed_state_pars_c_::set_setup()
{
  set(types::setup);
  return c;
}
void var_meas_cfg_s::speed_state_pars_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "var_meas_cfg_s::speed_state_pars_c_");
  }
  j.end_obj();
}
SRSASN_CODE var_meas_cfg_s::speed_state_pars_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "var_meas_cfg_s::speed_state_pars_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_cfg_s::speed_state_pars_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "var_meas_cfg_s::speed_state_pars_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
