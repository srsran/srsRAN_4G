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

#include "srsran/asn1/rrc/paging.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// PagingRecord-v1700 ::= SEQUENCE
SRSASN_CODE paging_record_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(paging_cause_r17_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_record_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(paging_cause_r17_present, 1));

  return SRSASN_SUCCESS;
}
void paging_record_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (paging_cause_r17_present) {
    j.write_str("pagingCause-r17", "voice");
  }
  j.end_obj();
}

// PagingRecord-v1610 ::= SEQUENCE
SRSASN_CODE paging_record_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(access_type_r16_present, 1));
  HANDLE_CODE(bref.pack(mt_edt_r16_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_record_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(access_type_r16_present, 1));
  HANDLE_CODE(bref.unpack(mt_edt_r16_present, 1));

  return SRSASN_SUCCESS;
}
void paging_record_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (access_type_r16_present) {
    j.write_str("accessType-r16", "non3GPP");
  }
  if (mt_edt_r16_present) {
    j.write_str("mt-EDT-r16", "true");
  }
  j.end_obj();
}

// Paging-v1700-IEs ::= SEQUENCE
SRSASN_CODE paging_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(paging_record_list_v1700_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (paging_record_list_v1700_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, paging_record_list_v1700, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(paging_record_list_v1700_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (paging_record_list_v1700_present) {
    HANDLE_CODE(unpack_dyn_seq_of(paging_record_list_v1700, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void paging_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (paging_record_list_v1700_present) {
    j.start_array("pagingRecordList-v1700");
    for (const auto& e1 : paging_record_list_v1700) {
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

// Paging-v1610-IEs ::= SEQUENCE
SRSASN_CODE paging_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(paging_record_list_v1610_present, 1));
  HANDLE_CODE(bref.pack(uac_param_mod_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (paging_record_list_v1610_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, paging_record_list_v1610, 1, 16));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(paging_record_list_v1610_present, 1));
  HANDLE_CODE(bref.unpack(uac_param_mod_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (paging_record_list_v1610_present) {
    HANDLE_CODE(unpack_dyn_seq_of(paging_record_list_v1610, bref, 1, 16));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void paging_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (paging_record_list_v1610_present) {
    j.start_array("pagingRecordList-v1610");
    for (const auto& e1 : paging_record_list_v1610) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (uac_param_mod_r16_present) {
    j.write_str("uac-ParamModification-r16", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// Paging-v1530-IEs ::= SEQUENCE
SRSASN_CODE paging_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(access_type_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(access_type_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void paging_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (access_type_present) {
    j.write_str("accessType", "non3GPP");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// Paging-v1310-IEs ::= SEQUENCE
SRSASN_CODE paging_v1310_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(redist_ind_r13_present, 1));
  HANDLE_CODE(bref.pack(sys_info_mod_e_drx_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_v1310_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(redist_ind_r13_present, 1));
  HANDLE_CODE(bref.unpack(sys_info_mod_e_drx_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void paging_v1310_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (redist_ind_r13_present) {
    j.write_str("redistributionIndication-r13", "true");
  }
  if (sys_info_mod_e_drx_r13_present) {
    j.write_str("systemInfoModification-eDRX-r13", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// S-TMSI ::= SEQUENCE
SRSASN_CODE s_tmsi_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(mmec.pack(bref));
  HANDLE_CODE(m_tmsi.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE s_tmsi_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(mmec.unpack(bref));
  HANDLE_CODE(m_tmsi.unpack(bref));

  return SRSASN_SUCCESS;
}
void s_tmsi_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("mmec", mmec.to_string());
  j.write_str("m-TMSI", m_tmsi.to_string());
  j.end_obj();
}

// Paging-v1130-IEs ::= SEQUENCE
SRSASN_CODE paging_v1130_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eab_param_mod_r11_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_v1130_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(eab_param_mod_r11_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void paging_v1130_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eab_param_mod_r11_present) {
    j.write_str("eab-ParamModification-r11", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// PagingUE-Identity ::= CHOICE
void paging_ue_id_c::destroy_()
{
  switch (type_) {
    case types::s_tmsi:
      c.destroy<s_tmsi_s>();
      break;
    case types::imsi:
      c.destroy<imsi_l>();
      break;
    case types::ng_minus5_g_s_tmsi_r15:
      c.destroy<fixed_bitstring<48> >();
      break;
    case types::full_i_rnti_r15:
      c.destroy<fixed_bitstring<40> >();
      break;
    default:
      break;
  }
}
void paging_ue_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::s_tmsi:
      c.init<s_tmsi_s>();
      break;
    case types::imsi:
      c.init<imsi_l>();
      break;
    case types::ng_minus5_g_s_tmsi_r15:
      c.init<fixed_bitstring<48> >();
      break;
    case types::full_i_rnti_r15:
      c.init<fixed_bitstring<40> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
  }
}
paging_ue_id_c::paging_ue_id_c(const paging_ue_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::s_tmsi:
      c.init(other.c.get<s_tmsi_s>());
      break;
    case types::imsi:
      c.init(other.c.get<imsi_l>());
      break;
    case types::ng_minus5_g_s_tmsi_r15:
      c.init(other.c.get<fixed_bitstring<48> >());
      break;
    case types::full_i_rnti_r15:
      c.init(other.c.get<fixed_bitstring<40> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
  }
}
paging_ue_id_c& paging_ue_id_c::operator=(const paging_ue_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::s_tmsi:
      c.set(other.c.get<s_tmsi_s>());
      break;
    case types::imsi:
      c.set(other.c.get<imsi_l>());
      break;
    case types::ng_minus5_g_s_tmsi_r15:
      c.set(other.c.get<fixed_bitstring<48> >());
      break;
    case types::full_i_rnti_r15:
      c.set(other.c.get<fixed_bitstring<40> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
  }

  return *this;
}
s_tmsi_s& paging_ue_id_c::set_s_tmsi()
{
  set(types::s_tmsi);
  return c.get<s_tmsi_s>();
}
imsi_l& paging_ue_id_c::set_imsi()
{
  set(types::imsi);
  return c.get<imsi_l>();
}
fixed_bitstring<48>& paging_ue_id_c::set_ng_minus5_g_s_tmsi_r15()
{
  set(types::ng_minus5_g_s_tmsi_r15);
  return c.get<fixed_bitstring<48> >();
}
fixed_bitstring<40>& paging_ue_id_c::set_full_i_rnti_r15()
{
  set(types::full_i_rnti_r15);
  return c.get<fixed_bitstring<40> >();
}
void paging_ue_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::s_tmsi:
      j.write_fieldname("s-TMSI");
      c.get<s_tmsi_s>().to_json(j);
      break;
    case types::imsi:
      j.start_array("imsi");
      for (const auto& e1 : c.get<imsi_l>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::ng_minus5_g_s_tmsi_r15:
      j.write_str("ng-5G-S-TMSI-r15", c.get<fixed_bitstring<48> >().to_string());
      break;
    case types::full_i_rnti_r15:
      j.write_str("fullI-RNTI-r15", c.get<fixed_bitstring<40> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
  }
  j.end_obj();
}
SRSASN_CODE paging_ue_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::s_tmsi:
      HANDLE_CODE(c.get<s_tmsi_s>().pack(bref));
      break;
    case types::imsi:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<imsi_l>(), 6, 21, integer_packer<uint8_t>(0, 9)));
      break;
    case types::ng_minus5_g_s_tmsi_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<fixed_bitstring<48> >().pack(bref));
    } break;
    case types::full_i_rnti_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<fixed_bitstring<40> >().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_ue_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::s_tmsi:
      HANDLE_CODE(c.get<s_tmsi_s>().unpack(bref));
      break;
    case types::imsi:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<imsi_l>(), bref, 6, 21, integer_packer<uint8_t>(0, 9)));
      break;
    case types::ng_minus5_g_s_tmsi_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<fixed_bitstring<48> >().unpack(bref));
    } break;
    case types::full_i_rnti_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<fixed_bitstring<40> >().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// Paging-v920-IEs ::= SEQUENCE
SRSASN_CODE paging_v920_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cmas_ind_r9_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_v920_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cmas_ind_r9_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void paging_v920_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cmas_ind_r9_present) {
    j.write_str("cmas-Indication-r9", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// PagingRecord ::= SEQUENCE
SRSASN_CODE paging_record_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ue_id.pack(bref));
  HANDLE_CODE(cn_domain.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_record_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ue_id.unpack(bref));
  HANDLE_CODE(cn_domain.unpack(bref));

  return SRSASN_SUCCESS;
}
void paging_record_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity");
  ue_id.to_json(j);
  j.write_str("cn-Domain", cn_domain.to_string());
  j.end_obj();
}

const char* paging_record_s::cn_domain_opts::to_string() const
{
  static const char* options[] = {"ps", "cs"};
  return convert_enum_idx(options, 2, value, "paging_record_s::cn_domain_e_");
}

// Paging-v890-IEs ::= SEQUENCE
SRSASN_CODE paging_v890_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE paging_v890_ies_s::unpack(cbit_ref& bref)
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
void paging_v890_ies_s::to_json(json_writer& j) const
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

// Paging ::= SEQUENCE
SRSASN_CODE paging_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(paging_record_list_present, 1));
  HANDLE_CODE(bref.pack(sys_info_mod_present, 1));
  HANDLE_CODE(bref.pack(etws_ind_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (paging_record_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, paging_record_list, 1, 16));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(paging_record_list_present, 1));
  HANDLE_CODE(bref.unpack(sys_info_mod_present, 1));
  HANDLE_CODE(bref.unpack(etws_ind_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (paging_record_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(paging_record_list, bref, 1, 16));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void paging_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (paging_record_list_present) {
    j.start_array("pagingRecordList");
    for (const auto& e1 : paging_record_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sys_info_mod_present) {
    j.write_str("systemInfoModification", "true");
  }
  if (etws_ind_present) {
    j.write_str("etws-Indication", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// PCCH-MessageType ::= CHOICE
void pcch_msg_type_c::set(types::options e)
{
  type_ = e;
}
pcch_msg_type_c::c1_c_& pcch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void pcch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void pcch_msg_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE pcch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_msg_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void pcch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("paging");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE pcch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// PCCH-Message ::= SEQUENCE
SRSASN_CODE pcch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void pcch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("PCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// UEPagingCoverageInformation-r13-IEs ::= SEQUENCE
SRSASN_CODE ue_paging_coverage_info_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mpdcch_num_repeat_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mpdcch_num_repeat_r13_present) {
    HANDLE_CODE(pack_integer(bref, mpdcch_num_repeat_r13, (uint16_t)1u, (uint16_t)256u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_paging_coverage_info_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mpdcch_num_repeat_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mpdcch_num_repeat_r13_present) {
    HANDLE_CODE(unpack_integer(mpdcch_num_repeat_r13, bref, (uint16_t)1u, (uint16_t)256u));
  }

  return SRSASN_SUCCESS;
}
void ue_paging_coverage_info_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mpdcch_num_repeat_r13_present) {
    j.write_int("mpdcch-NumRepetition-r13", mpdcch_num_repeat_r13);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UEPagingCoverageInformation ::= SEQUENCE
SRSASN_CODE ue_paging_coverage_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_paging_coverage_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_paging_coverage_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_paging_coverage_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_paging_coverage_info_s::crit_exts_c_::c1_c_& ue_paging_coverage_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ue_paging_coverage_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_paging_coverage_info_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ue_paging_coverage_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_paging_coverage_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_paging_coverage_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_paging_coverage_info_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ue_paging_coverage_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_paging_coverage_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ue_paging_coverage_info_r13_ies_s& ue_paging_coverage_info_s::crit_exts_c_::c1_c_::set_ue_paging_coverage_info_r13()
{
  set(types::ue_paging_coverage_info_r13);
  return c;
}
void ue_paging_coverage_info_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void ue_paging_coverage_info_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void ue_paging_coverage_info_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void ue_paging_coverage_info_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void ue_paging_coverage_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ue_paging_coverage_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ue_paging_coverage_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ue_paging_coverage_info_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_paging_coverage_info_r13:
      j.write_fieldname("uePagingCoverageInformation-r13");
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
      log_invalid_choice_id(type_, "ue_paging_coverage_info_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_paging_coverage_info_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_paging_coverage_info_r13:
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
      log_invalid_choice_id(type_, "ue_paging_coverage_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_paging_coverage_info_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_paging_coverage_info_r13:
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
      log_invalid_choice_id(type_, "ue_paging_coverage_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UERadioPagingInformation-v1610-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(access_stratum_release_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(access_stratum_release_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (access_stratum_release_r16_present) {
    j.write_str("accessStratumRelease-r16", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UERadioPagingInformation-v1310-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_v1310_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_list_eutra_for_paging_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (supported_band_list_eutra_for_paging_r13_present) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, supported_band_list_eutra_for_paging_r13, 1, 64, integer_packer<uint16_t>(1, 256)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_v1310_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_list_eutra_for_paging_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (supported_band_list_eutra_for_paging_r13_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(supported_band_list_eutra_for_paging_r13, bref, 1, 64, integer_packer<uint16_t>(1, 256)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_v1310_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_list_eutra_for_paging_r13_present) {
    j.start_array("supportedBandListEUTRAForPaging-r13");
    for (const auto& e1 : supported_band_list_eutra_for_paging_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UERadioPagingInformation-r12-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_r12_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_radio_paging_info_r12.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_r12_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_radio_paging_info_r12.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_r12_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ue-RadioPagingInfo-r12", ue_radio_paging_info_r12.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UERadioPagingInformation ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_radio_paging_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_paging_info_s::crit_exts_c_::c1_c_& ue_radio_paging_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ue_radio_paging_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_radio_paging_info_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_paging_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_paging_info_r12_ies_s& ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_ue_radio_paging_info_r12()
{
  set(types::ue_radio_paging_info_r12);
  return c;
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_radio_paging_info_r12:
      j.write_fieldname("ueRadioPagingInformation-r12");
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
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_paging_info_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_radio_paging_info_r12:
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
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_radio_paging_info_r12:
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
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
