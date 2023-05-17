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

#include "srsran/asn1/rrc/security.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// SecurityAlgorithmConfig ::= SEQUENCE
SRSASN_CODE security_algorithm_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ciphering_algorithm.pack(bref));
  HANDLE_CODE(integrity_prot_algorithm.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_algorithm_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ciphering_algorithm.unpack(bref));
  HANDLE_CODE(integrity_prot_algorithm.unpack(bref));

  return SRSASN_SUCCESS;
}
void security_algorithm_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cipheringAlgorithm", ciphering_algorithm.to_string());
  j.write_str("integrityProtAlgorithm", integrity_prot_algorithm.to_string());
  j.end_obj();
}

const char* security_algorithm_cfg_s::integrity_prot_algorithm_opts::to_string() const
{
  static const char* options[] = {"eia0-v920", "eia1", "eia2", "eia3-v1130", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "security_algorithm_cfg_s::integrity_prot_algorithm_e_");
}
uint8_t security_algorithm_cfg_s::integrity_prot_algorithm_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "security_algorithm_cfg_s::integrity_prot_algorithm_e_");
}

// SecurityConfigHO-v1530 ::= SEQUENCE
SRSASN_CODE security_cfg_ho_v1530_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(handov_type_v1530.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_cfg_ho_v1530_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(handov_type_v1530.unpack(bref));

  return SRSASN_SUCCESS;
}
void security_cfg_ho_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("handoverType-v1530");
  handov_type_v1530.to_json(j);
  j.end_obj();
}

void security_cfg_ho_v1530_s::handov_type_v1530_c_::destroy_()
{
  switch (type_) {
    case types::intra5_gc:
      c.destroy<intra5_gc_s_>();
      break;
    case types::fivegc_to_epc:
      c.destroy<fivegc_to_epc_s_>();
      break;
    case types::epc_to5_gc:
      c.destroy<epc_to5_gc_s_>();
      break;
    default:
      break;
  }
}
void security_cfg_ho_v1530_s::handov_type_v1530_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::intra5_gc:
      c.init<intra5_gc_s_>();
      break;
    case types::fivegc_to_epc:
      c.init<fivegc_to_epc_s_>();
      break;
    case types::epc_to5_gc:
      c.init<epc_to5_gc_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "security_cfg_ho_v1530_s::handov_type_v1530_c_");
  }
}
security_cfg_ho_v1530_s::handov_type_v1530_c_::handov_type_v1530_c_(
    const security_cfg_ho_v1530_s::handov_type_v1530_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::intra5_gc:
      c.init(other.c.get<intra5_gc_s_>());
      break;
    case types::fivegc_to_epc:
      c.init(other.c.get<fivegc_to_epc_s_>());
      break;
    case types::epc_to5_gc:
      c.init(other.c.get<epc_to5_gc_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "security_cfg_ho_v1530_s::handov_type_v1530_c_");
  }
}
security_cfg_ho_v1530_s::handov_type_v1530_c_&
security_cfg_ho_v1530_s::handov_type_v1530_c_::operator=(const security_cfg_ho_v1530_s::handov_type_v1530_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::intra5_gc:
      c.set(other.c.get<intra5_gc_s_>());
      break;
    case types::fivegc_to_epc:
      c.set(other.c.get<fivegc_to_epc_s_>());
      break;
    case types::epc_to5_gc:
      c.set(other.c.get<epc_to5_gc_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "security_cfg_ho_v1530_s::handov_type_v1530_c_");
  }

  return *this;
}
security_cfg_ho_v1530_s::handov_type_v1530_c_::intra5_gc_s_&
security_cfg_ho_v1530_s::handov_type_v1530_c_::set_intra5_gc()
{
  set(types::intra5_gc);
  return c.get<intra5_gc_s_>();
}
security_cfg_ho_v1530_s::handov_type_v1530_c_::fivegc_to_epc_s_&
security_cfg_ho_v1530_s::handov_type_v1530_c_::set_fivegc_to_epc()
{
  set(types::fivegc_to_epc);
  return c.get<fivegc_to_epc_s_>();
}
security_cfg_ho_v1530_s::handov_type_v1530_c_::epc_to5_gc_s_&
security_cfg_ho_v1530_s::handov_type_v1530_c_::set_epc_to5_gc()
{
  set(types::epc_to5_gc);
  return c.get<epc_to5_gc_s_>();
}
void security_cfg_ho_v1530_s::handov_type_v1530_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::intra5_gc:
      j.write_fieldname("intra5GC");
      j.start_obj();
      if (c.get<intra5_gc_s_>().security_algorithm_cfg_r15_present) {
        j.write_fieldname("securityAlgorithmConfig-r15");
        c.get<intra5_gc_s_>().security_algorithm_cfg_r15.to_json(j);
      }
      j.write_bool("keyChangeIndicator-r15", c.get<intra5_gc_s_>().key_change_ind_r15);
      j.write_int("nextHopChainingCount-r15", c.get<intra5_gc_s_>().next_hop_chaining_count_r15);
      if (c.get<intra5_gc_s_>().nas_container_r15_present) {
        j.write_str("nas-Container-r15", c.get<intra5_gc_s_>().nas_container_r15.to_string());
      }
      j.end_obj();
      break;
    case types::fivegc_to_epc:
      j.write_fieldname("fivegc-ToEPC");
      j.start_obj();
      j.write_fieldname("securityAlgorithmConfig-r15");
      c.get<fivegc_to_epc_s_>().security_algorithm_cfg_r15.to_json(j);
      j.write_int("nextHopChainingCount-r15", c.get<fivegc_to_epc_s_>().next_hop_chaining_count_r15);
      j.end_obj();
      break;
    case types::epc_to5_gc:
      j.write_fieldname("epc-To5GC");
      j.start_obj();
      j.write_fieldname("securityAlgorithmConfig-r15");
      c.get<epc_to5_gc_s_>().security_algorithm_cfg_r15.to_json(j);
      j.write_str("nas-Container-r15", c.get<epc_to5_gc_s_>().nas_container_r15.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "security_cfg_ho_v1530_s::handov_type_v1530_c_");
  }
  j.end_obj();
}
SRSASN_CODE security_cfg_ho_v1530_s::handov_type_v1530_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::intra5_gc:
      HANDLE_CODE(bref.pack(c.get<intra5_gc_s_>().security_algorithm_cfg_r15_present, 1));
      HANDLE_CODE(bref.pack(c.get<intra5_gc_s_>().nas_container_r15_present, 1));
      if (c.get<intra5_gc_s_>().security_algorithm_cfg_r15_present) {
        HANDLE_CODE(c.get<intra5_gc_s_>().security_algorithm_cfg_r15.pack(bref));
      }
      HANDLE_CODE(bref.pack(c.get<intra5_gc_s_>().key_change_ind_r15, 1));
      HANDLE_CODE(pack_integer(bref, c.get<intra5_gc_s_>().next_hop_chaining_count_r15, (uint8_t)0u, (uint8_t)7u));
      if (c.get<intra5_gc_s_>().nas_container_r15_present) {
        HANDLE_CODE(c.get<intra5_gc_s_>().nas_container_r15.pack(bref));
      }
      break;
    case types::fivegc_to_epc:
      HANDLE_CODE(c.get<fivegc_to_epc_s_>().security_algorithm_cfg_r15.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.get<fivegc_to_epc_s_>().next_hop_chaining_count_r15, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::epc_to5_gc:
      HANDLE_CODE(c.get<epc_to5_gc_s_>().security_algorithm_cfg_r15.pack(bref));
      HANDLE_CODE(c.get<epc_to5_gc_s_>().nas_container_r15.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "security_cfg_ho_v1530_s::handov_type_v1530_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE security_cfg_ho_v1530_s::handov_type_v1530_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::intra5_gc:
      HANDLE_CODE(bref.unpack(c.get<intra5_gc_s_>().security_algorithm_cfg_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.get<intra5_gc_s_>().nas_container_r15_present, 1));
      if (c.get<intra5_gc_s_>().security_algorithm_cfg_r15_present) {
        HANDLE_CODE(c.get<intra5_gc_s_>().security_algorithm_cfg_r15.unpack(bref));
      }
      HANDLE_CODE(bref.unpack(c.get<intra5_gc_s_>().key_change_ind_r15, 1));
      HANDLE_CODE(unpack_integer(c.get<intra5_gc_s_>().next_hop_chaining_count_r15, bref, (uint8_t)0u, (uint8_t)7u));
      if (c.get<intra5_gc_s_>().nas_container_r15_present) {
        HANDLE_CODE(c.get<intra5_gc_s_>().nas_container_r15.unpack(bref));
      }
      break;
    case types::fivegc_to_epc:
      HANDLE_CODE(c.get<fivegc_to_epc_s_>().security_algorithm_cfg_r15.unpack(bref));
      HANDLE_CODE(
          unpack_integer(c.get<fivegc_to_epc_s_>().next_hop_chaining_count_r15, bref, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::epc_to5_gc:
      HANDLE_CODE(c.get<epc_to5_gc_s_>().security_algorithm_cfg_r15.unpack(bref));
      HANDLE_CODE(c.get<epc_to5_gc_s_>().nas_container_r15.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "security_cfg_ho_v1530_s::handov_type_v1530_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SecurityConfigHO ::= SEQUENCE
SRSASN_CODE security_cfg_ho_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(handov_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_cfg_ho_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(handov_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void security_cfg_ho_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("handoverType");
  handov_type.to_json(j);
  j.end_obj();
}

void security_cfg_ho_s::handov_type_c_::destroy_()
{
  switch (type_) {
    case types::intra_lte:
      c.destroy<intra_lte_s_>();
      break;
    case types::inter_rat:
      c.destroy<inter_rat_s_>();
      break;
    default:
      break;
  }
}
void security_cfg_ho_s::handov_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::intra_lte:
      c.init<intra_lte_s_>();
      break;
    case types::inter_rat:
      c.init<inter_rat_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "security_cfg_ho_s::handov_type_c_");
  }
}
security_cfg_ho_s::handov_type_c_::handov_type_c_(const security_cfg_ho_s::handov_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::intra_lte:
      c.init(other.c.get<intra_lte_s_>());
      break;
    case types::inter_rat:
      c.init(other.c.get<inter_rat_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "security_cfg_ho_s::handov_type_c_");
  }
}
security_cfg_ho_s::handov_type_c_&
security_cfg_ho_s::handov_type_c_::operator=(const security_cfg_ho_s::handov_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::intra_lte:
      c.set(other.c.get<intra_lte_s_>());
      break;
    case types::inter_rat:
      c.set(other.c.get<inter_rat_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "security_cfg_ho_s::handov_type_c_");
  }

  return *this;
}
security_cfg_ho_s::handov_type_c_::intra_lte_s_& security_cfg_ho_s::handov_type_c_::set_intra_lte()
{
  set(types::intra_lte);
  return c.get<intra_lte_s_>();
}
security_cfg_ho_s::handov_type_c_::inter_rat_s_& security_cfg_ho_s::handov_type_c_::set_inter_rat()
{
  set(types::inter_rat);
  return c.get<inter_rat_s_>();
}
void security_cfg_ho_s::handov_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::intra_lte:
      j.write_fieldname("intraLTE");
      j.start_obj();
      if (c.get<intra_lte_s_>().security_algorithm_cfg_present) {
        j.write_fieldname("securityAlgorithmConfig");
        c.get<intra_lte_s_>().security_algorithm_cfg.to_json(j);
      }
      j.write_bool("keyChangeIndicator", c.get<intra_lte_s_>().key_change_ind);
      j.write_int("nextHopChainingCount", c.get<intra_lte_s_>().next_hop_chaining_count);
      j.end_obj();
      break;
    case types::inter_rat:
      j.write_fieldname("interRAT");
      j.start_obj();
      j.write_fieldname("securityAlgorithmConfig");
      c.get<inter_rat_s_>().security_algorithm_cfg.to_json(j);
      j.write_str("nas-SecurityParamToEUTRA", c.get<inter_rat_s_>().nas_security_param_to_eutra.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "security_cfg_ho_s::handov_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE security_cfg_ho_s::handov_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::intra_lte:
      HANDLE_CODE(bref.pack(c.get<intra_lte_s_>().security_algorithm_cfg_present, 1));
      if (c.get<intra_lte_s_>().security_algorithm_cfg_present) {
        HANDLE_CODE(c.get<intra_lte_s_>().security_algorithm_cfg.pack(bref));
      }
      HANDLE_CODE(bref.pack(c.get<intra_lte_s_>().key_change_ind, 1));
      HANDLE_CODE(pack_integer(bref, c.get<intra_lte_s_>().next_hop_chaining_count, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::inter_rat:
      HANDLE_CODE(c.get<inter_rat_s_>().security_algorithm_cfg.pack(bref));
      HANDLE_CODE(c.get<inter_rat_s_>().nas_security_param_to_eutra.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "security_cfg_ho_s::handov_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE security_cfg_ho_s::handov_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::intra_lte:
      HANDLE_CODE(bref.unpack(c.get<intra_lte_s_>().security_algorithm_cfg_present, 1));
      if (c.get<intra_lte_s_>().security_algorithm_cfg_present) {
        HANDLE_CODE(c.get<intra_lte_s_>().security_algorithm_cfg.unpack(bref));
      }
      HANDLE_CODE(bref.unpack(c.get<intra_lte_s_>().key_change_ind, 1));
      HANDLE_CODE(unpack_integer(c.get<intra_lte_s_>().next_hop_chaining_count, bref, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::inter_rat:
      HANDLE_CODE(c.get<inter_rat_s_>().security_algorithm_cfg.unpack(bref));
      HANDLE_CODE(c.get<inter_rat_s_>().nas_security_param_to_eutra.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "security_cfg_ho_s::handov_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SecurityConfigSMC ::= SEQUENCE
SRSASN_CODE security_cfg_smc_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(security_algorithm_cfg.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_cfg_smc_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(security_algorithm_cfg.unpack(bref));

  return SRSASN_SUCCESS;
}
void security_cfg_smc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("securityAlgorithmConfig");
  security_algorithm_cfg.to_json(j);
  j.end_obj();
}

// SecurityModeCommand-v8a0-IEs ::= SEQUENCE
SRSASN_CODE security_mode_cmd_v8a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_cmd_v8a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_mode_cmd_v8a0_ies_s::to_json(json_writer& j) const
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

// SecurityModeCommand-r8-IEs ::= SEQUENCE
SRSASN_CODE security_mode_cmd_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(security_cfg_smc.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_cmd_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(security_cfg_smc.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_mode_cmd_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("securityConfigSMC");
  security_cfg_smc.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SecurityModeCommand ::= SEQUENCE
SRSASN_CODE security_mode_cmd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_cmd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void security_mode_cmd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void security_mode_cmd_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
security_mode_cmd_s::crit_exts_c_::c1_c_& security_mode_cmd_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void security_mode_cmd_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void security_mode_cmd_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "security_mode_cmd_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE security_mode_cmd_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_cmd_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_cmd_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "security_mode_cmd_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void security_mode_cmd_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
security_mode_cmd_r8_ies_s& security_mode_cmd_s::crit_exts_c_::c1_c_::set_security_mode_cmd_r8()
{
  set(types::security_mode_cmd_r8);
  return c;
}
void security_mode_cmd_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void security_mode_cmd_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void security_mode_cmd_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void security_mode_cmd_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::security_mode_cmd_r8:
      j.write_fieldname("securityModeCommand-r8");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_cmd_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE security_mode_cmd_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::security_mode_cmd_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_cmd_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_cmd_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::security_mode_cmd_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_cmd_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SecurityModeComplete-v8a0-IEs ::= SEQUENCE
SRSASN_CODE security_mode_complete_v8a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_complete_v8a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_mode_complete_v8a0_ies_s::to_json(json_writer& j) const
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

// SecurityModeFailure-v8a0-IEs ::= SEQUENCE
SRSASN_CODE security_mode_fail_v8a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_fail_v8a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_mode_fail_v8a0_ies_s::to_json(json_writer& j) const
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

// SecurityModeComplete-r8-IEs ::= SEQUENCE
SRSASN_CODE security_mode_complete_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_complete_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_mode_complete_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SecurityModeFailure-r8-IEs ::= SEQUENCE
SRSASN_CODE security_mode_fail_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_fail_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_mode_fail_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SecurityModeComplete ::= SEQUENCE
SRSASN_CODE security_mode_complete_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_complete_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void security_mode_complete_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void security_mode_complete_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
security_mode_complete_r8_ies_s& security_mode_complete_s::crit_exts_c_::set_security_mode_complete_r8()
{
  set(types::security_mode_complete_r8);
  return c;
}
void security_mode_complete_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void security_mode_complete_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::security_mode_complete_r8:
      j.write_fieldname("securityModeComplete-r8");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_complete_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE security_mode_complete_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::security_mode_complete_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_complete_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_complete_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::security_mode_complete_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_complete_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SecurityModeFailure ::= SEQUENCE
SRSASN_CODE security_mode_fail_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_fail_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void security_mode_fail_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void security_mode_fail_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
security_mode_fail_r8_ies_s& security_mode_fail_s::crit_exts_c_::set_security_mode_fail_r8()
{
  set(types::security_mode_fail_r8);
  return c;
}
void security_mode_fail_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void security_mode_fail_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::security_mode_fail_r8:
      j.write_fieldname("securityModeFailure-r8");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_fail_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE security_mode_fail_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::security_mode_fail_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_fail_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_fail_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::security_mode_fail_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_fail_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// VarShortMAC-Input ::= SEQUENCE
SRSASN_CODE var_short_mac_input_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cell_id.pack(bref));
  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(c_rnti.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_short_mac_input_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cell_id.unpack(bref));
  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(c_rnti.unpack(bref));

  return SRSASN_SUCCESS;
}
void var_short_mac_input_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cellIdentity", cell_id.to_string());
  j.write_int("physCellId", pci);
  j.write_str("c-RNTI", c_rnti.to_string());
  j.end_obj();
}
