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

#include "srsran/asn1/rrc/dl_dcch_msg.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// SCellConfigCommon-r15 ::= SEQUENCE
SRSASN_CODE scell_cfg_common_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rr_cfg_common_scell_r15_present, 1));
  HANDLE_CODE(bref.pack(rr_cfg_ded_scell_r15_present, 1));
  HANDLE_CODE(bref.pack(ant_info_ded_scell_r15_present, 1));

  if (rr_cfg_common_scell_r15_present) {
    HANDLE_CODE(rr_cfg_common_scell_r15.pack(bref));
  }
  if (rr_cfg_ded_scell_r15_present) {
    HANDLE_CODE(rr_cfg_ded_scell_r15.pack(bref));
  }
  if (ant_info_ded_scell_r15_present) {
    HANDLE_CODE(ant_info_ded_scell_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_cfg_common_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rr_cfg_common_scell_r15_present, 1));
  HANDLE_CODE(bref.unpack(rr_cfg_ded_scell_r15_present, 1));
  HANDLE_CODE(bref.unpack(ant_info_ded_scell_r15_present, 1));

  if (rr_cfg_common_scell_r15_present) {
    HANDLE_CODE(rr_cfg_common_scell_r15.unpack(bref));
  }
  if (rr_cfg_ded_scell_r15_present) {
    HANDLE_CODE(rr_cfg_ded_scell_r15.unpack(bref));
  }
  if (ant_info_ded_scell_r15_present) {
    HANDLE_CODE(ant_info_ded_scell_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scell_cfg_common_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rr_cfg_common_scell_r15_present) {
    j.write_fieldname("radioResourceConfigCommonSCell-r15");
    rr_cfg_common_scell_r15.to_json(j);
  }
  if (rr_cfg_ded_scell_r15_present) {
    j.write_fieldname("radioResourceConfigDedicatedSCell-r15");
    rr_cfg_ded_scell_r15.to_json(j);
  }
  if (ant_info_ded_scell_r15_present) {
    j.write_fieldname("antennaInfoDedicatedSCell-r15");
    ant_info_ded_scell_r15.to_json(j);
  }
  j.end_obj();
}

// SL-DiscTxPoolToAddMod-r12 ::= SEQUENCE
SRSASN_CODE sl_disc_tx_pool_to_add_mod_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pool_id_r12, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pool_r12.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_tx_pool_to_add_mod_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pool_id_r12, bref, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pool_r12.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_disc_tx_pool_to_add_mod_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("poolIdentity-r12", pool_id_r12);
  j.write_fieldname("pool-r12");
  pool_r12.to_json(j);
  j.end_obj();
}

// SL-TF-IndexPair-r12b ::= SEQUENCE
SRSASN_CODE sl_tf_idx_pair_r12b_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(disc_sf_idx_r12b_present, 1));
  HANDLE_CODE(bref.pack(disc_prb_idx_r12b_present, 1));

  if (disc_sf_idx_r12b_present) {
    HANDLE_CODE(pack_integer(bref, disc_sf_idx_r12b, (uint8_t)0u, (uint8_t)209u));
  }
  if (disc_prb_idx_r12b_present) {
    HANDLE_CODE(pack_integer(bref, disc_prb_idx_r12b, (uint8_t)0u, (uint8_t)49u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tf_idx_pair_r12b_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(disc_sf_idx_r12b_present, 1));
  HANDLE_CODE(bref.unpack(disc_prb_idx_r12b_present, 1));

  if (disc_sf_idx_r12b_present) {
    HANDLE_CODE(unpack_integer(disc_sf_idx_r12b, bref, (uint8_t)0u, (uint8_t)209u));
  }
  if (disc_prb_idx_r12b_present) {
    HANDLE_CODE(unpack_integer(disc_prb_idx_r12b, bref, (uint8_t)0u, (uint8_t)49u));
  }

  return SRSASN_SUCCESS;
}
void sl_tf_idx_pair_r12b_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (disc_sf_idx_r12b_present) {
    j.write_int("discSF-Index-r12b", disc_sf_idx_r12b);
  }
  if (disc_prb_idx_r12b_present) {
    j.write_int("discPRB-Index-r12b", disc_prb_idx_r12b);
  }
  j.end_obj();
}

// SCellGroupToAddMod-r15 ::= SEQUENCE
SRSASN_CODE scell_group_to_add_mod_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scell_cfg_common_r15_present, 1));
  HANDLE_CODE(bref.pack(scell_to_release_list_r15_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, scell_group_idx_r15, (uint8_t)1u, (uint8_t)4u));
  if (scell_cfg_common_r15_present) {
    HANDLE_CODE(scell_cfg_common_r15.pack(bref));
  }
  if (scell_to_release_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_release_list_r15, 1, 31, integer_packer<uint8_t>(1, 31)));
  }
  if (scell_to_add_mod_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_r15, 1, 31));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_group_to_add_mod_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scell_cfg_common_r15_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_release_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_r15_present, 1));

  HANDLE_CODE(unpack_integer(scell_group_idx_r15, bref, (uint8_t)1u, (uint8_t)4u));
  if (scell_cfg_common_r15_present) {
    HANDLE_CODE(scell_cfg_common_r15.unpack(bref));
  }
  if (scell_to_release_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_release_list_r15, bref, 1, 31, integer_packer<uint8_t>(1, 31)));
  }
  if (scell_to_add_mod_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_r15, bref, 1, 31));
  }

  return SRSASN_SUCCESS;
}
void scell_group_to_add_mod_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sCellGroupIndex-r15", scell_group_idx_r15);
  if (scell_cfg_common_r15_present) {
    j.write_fieldname("sCellConfigCommon-r15");
    scell_cfg_common_r15.to_json(j);
  }
  if (scell_to_release_list_r15_present) {
    j.start_array("sCellToReleaseList-r15");
    for (const auto& e1 : scell_to_release_list_r15) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (scell_to_add_mod_list_r15_present) {
    j.start_array("sCellToAddModList-r15");
    for (const auto& e1 : scell_to_add_mod_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-HoppingConfigDisc-r12 ::= SEQUENCE
SRSASN_CODE sl_hop_cfg_disc_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, a_r12, (uint8_t)1u, (uint8_t)200u));
  HANDLE_CODE(pack_integer(bref, b_r12, (uint8_t)1u, (uint8_t)10u));
  HANDLE_CODE(c_r12.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_hop_cfg_disc_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(a_r12, bref, (uint8_t)1u, (uint8_t)200u));
  HANDLE_CODE(unpack_integer(b_r12, bref, (uint8_t)1u, (uint8_t)10u));
  HANDLE_CODE(c_r12.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_hop_cfg_disc_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("a-r12", a_r12);
  j.write_int("b-r12", b_r12);
  j.write_str("c-r12", c_r12.to_string());
  j.end_obj();
}

const char* sl_hop_cfg_disc_r12_s::c_r12_opts::to_string() const
{
  static const char* options[] = {"n1", "n5"};
  return convert_enum_idx(options, 2, value, "sl_hop_cfg_disc_r12_s::c_r12_e_");
}
uint8_t sl_hop_cfg_disc_r12_s::c_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 5};
  return map_enum_number(options, 2, value, "sl_hop_cfg_disc_r12_s::c_r12_e_");
}

// IKE-Identity-r13 ::= SEQUENCE
SRSASN_CODE ike_id_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(id_i_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ike_id_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(id_i_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void ike_id_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("idI-r13", id_i_r13.to_string());
  j.end_obj();
}

// IP-Address-r13 ::= CHOICE
void ip_address_r13_c::destroy_()
{
  switch (type_) {
    case types::ipv4_r13:
      c.destroy<fixed_bitstring<32> >();
      break;
    case types::ipv6_r13:
      c.destroy<fixed_bitstring<128> >();
      break;
    default:
      break;
  }
}
void ip_address_r13_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ipv4_r13:
      c.init<fixed_bitstring<32> >();
      break;
    case types::ipv6_r13:
      c.init<fixed_bitstring<128> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ip_address_r13_c");
  }
}
ip_address_r13_c::ip_address_r13_c(const ip_address_r13_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ipv4_r13:
      c.init(other.c.get<fixed_bitstring<32> >());
      break;
    case types::ipv6_r13:
      c.init(other.c.get<fixed_bitstring<128> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ip_address_r13_c");
  }
}
ip_address_r13_c& ip_address_r13_c::operator=(const ip_address_r13_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ipv4_r13:
      c.set(other.c.get<fixed_bitstring<32> >());
      break;
    case types::ipv6_r13:
      c.set(other.c.get<fixed_bitstring<128> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ip_address_r13_c");
  }

  return *this;
}
fixed_bitstring<32>& ip_address_r13_c::set_ipv4_r13()
{
  set(types::ipv4_r13);
  return c.get<fixed_bitstring<32> >();
}
fixed_bitstring<128>& ip_address_r13_c::set_ipv6_r13()
{
  set(types::ipv6_r13);
  return c.get<fixed_bitstring<128> >();
}
void ip_address_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ipv4_r13:
      j.write_str("ipv4-r13", c.get<fixed_bitstring<32> >().to_string());
      break;
    case types::ipv6_r13:
      j.write_str("ipv6-r13", c.get<fixed_bitstring<128> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ip_address_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE ip_address_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ipv4_r13:
      HANDLE_CODE(c.get<fixed_bitstring<32> >().pack(bref));
      break;
    case types::ipv6_r13:
      HANDLE_CODE(c.get<fixed_bitstring<128> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ip_address_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ip_address_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ipv4_r13:
      HANDLE_CODE(c.get<fixed_bitstring<32> >().unpack(bref));
      break;
    case types::ipv6_r13:
      HANDLE_CODE(c.get<fixed_bitstring<128> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ip_address_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// PhysicalConfigDedicated-v1370 ::= SEQUENCE
SRSASN_CODE phys_cfg_ded_v1370_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pucch_cfg_ded_v1370_present, 1));

  if (pucch_cfg_ded_v1370_present) {
    HANDLE_CODE(pucch_cfg_ded_v1370.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_v1370_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pucch_cfg_ded_v1370_present, 1));

  if (pucch_cfg_ded_v1370_present) {
    HANDLE_CODE(pucch_cfg_ded_v1370.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void phys_cfg_ded_v1370_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pucch_cfg_ded_v1370_present) {
    j.write_fieldname("pucch-ConfigDedicated-v1370");
    pucch_cfg_ded_v1370.to_json(j);
  }
  j.end_obj();
}

// RAN-AreaConfig-r15 ::= SEQUENCE
SRSASN_CODE ran_area_cfg_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ran_area_code_list_r15_present, 1));

  HANDLE_CODE(tac_minus5_gc_r15.pack(bref));
  if (ran_area_code_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_area_code_list_r15, 1, 32, integer_packer<uint16_t>(0, 255)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_area_cfg_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ran_area_code_list_r15_present, 1));

  HANDLE_CODE(tac_minus5_gc_r15.unpack(bref));
  if (ran_area_code_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ran_area_code_list_r15, bref, 1, 32, integer_packer<uint16_t>(0, 255)));
  }

  return SRSASN_SUCCESS;
}
void ran_area_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("trackingAreaCode-5GC-r15", tac_minus5_gc_r15.to_string());
  if (ran_area_code_list_r15_present) {
    j.start_array("ran-AreaCodeList-r15");
    for (const auto& e1 : ran_area_code_list_r15) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// RadioResourceConfigCommonSCell-v1440 ::= SEQUENCE
SRSASN_CODE rr_cfg_common_scell_v1440_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(
      pack_integer(bref, ul_cfg_v1440.ul_freq_info_v1440.add_spec_emission_scell_v1440, (uint16_t)33u, (uint16_t)288u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_common_scell_v1440_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(
      ul_cfg_v1440.ul_freq_info_v1440.add_spec_emission_scell_v1440, bref, (uint16_t)33u, (uint16_t)288u));

  return SRSASN_SUCCESS;
}
void rr_cfg_common_scell_v1440_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ul-Configuration-v1440");
  j.start_obj();
  j.write_fieldname("ul-FreqInfo-v1440");
  j.start_obj();
  j.write_int("additionalSpectrumEmissionSCell-v1440", ul_cfg_v1440.ul_freq_info_v1440.add_spec_emission_scell_v1440);
  j.end_obj();
  j.end_obj();
  j.end_obj();
}

// SL-DiscTxConfigScheduled-r13 ::= SEQUENCE
SRSASN_CODE sl_disc_tx_cfg_sched_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(disc_tx_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_tf_idx_list_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_hop_cfg_r13_present, 1));

  if (disc_tx_cfg_r13_present) {
    HANDLE_CODE(disc_tx_cfg_r13.pack(bref));
  }
  if (disc_tf_idx_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, disc_tf_idx_list_r13, 1, 64));
  }
  if (disc_hop_cfg_r13_present) {
    HANDLE_CODE(disc_hop_cfg_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_tx_cfg_sched_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(disc_tx_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_tf_idx_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_hop_cfg_r13_present, 1));

  if (disc_tx_cfg_r13_present) {
    HANDLE_CODE(disc_tx_cfg_r13.unpack(bref));
  }
  if (disc_tf_idx_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(disc_tf_idx_list_r13, bref, 1, 64));
  }
  if (disc_hop_cfg_r13_present) {
    HANDLE_CODE(disc_hop_cfg_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_disc_tx_cfg_sched_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (disc_tx_cfg_r13_present) {
    j.write_fieldname("discTxConfig-r13");
    disc_tx_cfg_r13.to_json(j);
  }
  if (disc_tf_idx_list_r13_present) {
    j.start_array("discTF-IndexList-r13");
    for (const auto& e1 : disc_tf_idx_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (disc_hop_cfg_r13_present) {
    j.write_fieldname("discHoppingConfig-r13");
    disc_hop_cfg_r13.to_json(j);
  }
  j.end_obj();
}

// SL-DiscTxPoolDedicated-r13 ::= SEQUENCE
SRSASN_CODE sl_disc_tx_pool_ded_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pool_to_release_list_r13_present, 1));
  HANDLE_CODE(bref.pack(pool_to_add_mod_list_r13_present, 1));

  if (pool_to_release_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pool_to_release_list_r13, 1, 4, integer_packer<uint8_t>(1, 4)));
  }
  if (pool_to_add_mod_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pool_to_add_mod_list_r13, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_tx_pool_ded_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pool_to_release_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(pool_to_add_mod_list_r13_present, 1));

  if (pool_to_release_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pool_to_release_list_r13, bref, 1, 4, integer_packer<uint8_t>(1, 4)));
  }
  if (pool_to_add_mod_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pool_to_add_mod_list_r13, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void sl_disc_tx_pool_ded_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pool_to_release_list_r13_present) {
    j.start_array("poolToReleaseList-r13");
    for (const auto& e1 : pool_to_release_list_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (pool_to_add_mod_list_r13_present) {
    j.start_array("poolToAddModList-r13");
    for (const auto& e1 : pool_to_add_mod_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-TxPoolToAddMod-r14 ::= SEQUENCE
SRSASN_CODE sl_tx_pool_to_add_mod_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pool_id_r14, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pool_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tx_pool_to_add_mod_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pool_id_r14, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pool_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_tx_pool_to_add_mod_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("poolIdentity-r14", pool_id_r14);
  j.write_fieldname("pool-r14");
  pool_r14.to_json(j);
  j.end_obj();
}

// SubframeAssignment-r15 ::= ENUMERATED
const char* sf_assign_r15_opts::to_string() const
{
  static const char* options[] = {"sa0", "sa1", "sa2", "sa3", "sa4", "sa5", "sa6"};
  return convert_enum_idx(options, 7, value, "sf_assign_r15_e");
}
uint8_t sf_assign_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 7, value, "sf_assign_r15_e");
}

// UplinkPowerControlCommonPSCell-r12 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_common_ps_cell_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(delta_f_pucch_format3_r12.pack(bref));
  HANDLE_CODE(delta_f_pucch_format1b_cs_r12.pack(bref));
  HANDLE_CODE(pack_integer(bref, p0_nominal_pucch_r12, (int8_t)-127, (int8_t)-96));
  HANDLE_CODE(delta_flist_pucch_r12.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_common_ps_cell_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(delta_f_pucch_format3_r12.unpack(bref));
  HANDLE_CODE(delta_f_pucch_format1b_cs_r12.unpack(bref));
  HANDLE_CODE(unpack_integer(p0_nominal_pucch_r12, bref, (int8_t)-127, (int8_t)-96));
  HANDLE_CODE(delta_flist_pucch_r12.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_common_ps_cell_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("deltaF-PUCCH-Format3-r12", delta_f_pucch_format3_r12.to_string());
  j.write_str("deltaF-PUCCH-Format1bCS-r12", delta_f_pucch_format1b_cs_r12.to_string());
  j.write_int("p0-NominalPUCCH-r12", p0_nominal_pucch_r12);
  j.write_fieldname("deltaFList-PUCCH-r12");
  delta_flist_pucch_r12.to_json(j);
  j.end_obj();
}

const char* ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format3_r12_opts::to_string() const
{
  static const char* options[] = {
      "deltaF-1", "deltaF0", "deltaF1", "deltaF2", "deltaF3", "deltaF4", "deltaF5", "deltaF6"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format3_r12_e_");
}
int8_t ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format3_r12_opts::to_number() const
{
  static const int8_t options[] = {-1, 0, 1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 8, value, "ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format3_r12_e_");
}

const char* ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format1b_cs_r12_opts::to_string() const
{
  static const char* options[] = {"deltaF1", "deltaF2", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format1b_cs_r12_e_");
}
uint8_t ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format1b_cs_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format1b_cs_r12_e_");
}

// WLAN-SuspendConfig-r14 ::= SEQUENCE
SRSASN_CODE wlan_suspend_cfg_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(wlan_suspend_resume_allowed_r14_present, 1));
  HANDLE_CODE(bref.pack(wlan_suspend_triggers_status_report_r14_present, 1));

  if (wlan_suspend_resume_allowed_r14_present) {
    HANDLE_CODE(bref.pack(wlan_suspend_resume_allowed_r14, 1));
  }
  if (wlan_suspend_triggers_status_report_r14_present) {
    HANDLE_CODE(bref.pack(wlan_suspend_triggers_status_report_r14, 1));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_suspend_cfg_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(wlan_suspend_resume_allowed_r14_present, 1));
  HANDLE_CODE(bref.unpack(wlan_suspend_triggers_status_report_r14_present, 1));

  if (wlan_suspend_resume_allowed_r14_present) {
    HANDLE_CODE(bref.unpack(wlan_suspend_resume_allowed_r14, 1));
  }
  if (wlan_suspend_triggers_status_report_r14_present) {
    HANDLE_CODE(bref.unpack(wlan_suspend_triggers_status_report_r14, 1));
  }

  return SRSASN_SUCCESS;
}
void wlan_suspend_cfg_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (wlan_suspend_resume_allowed_r14_present) {
    j.write_bool("wlan-SuspendResumeAllowed-r14", wlan_suspend_resume_allowed_r14);
  }
  if (wlan_suspend_triggers_status_report_r14_present) {
    j.write_bool("wlan-SuspendTriggersStatusReport-r14", wlan_suspend_triggers_status_report_r14);
  }
  j.end_obj();
}

// MAC-MainConfigSL-r12 ::= SEQUENCE
SRSASN_CODE mac_main_cfg_sl_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(periodic_bsr_timer_sl_present, 1));

  if (periodic_bsr_timer_sl_present) {
    HANDLE_CODE(periodic_bsr_timer_sl.pack(bref));
  }
  HANDLE_CODE(retx_bsr_timer_sl.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_sl_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(periodic_bsr_timer_sl_present, 1));

  if (periodic_bsr_timer_sl_present) {
    HANDLE_CODE(periodic_bsr_timer_sl.unpack(bref));
  }
  HANDLE_CODE(retx_bsr_timer_sl.unpack(bref));

  return SRSASN_SUCCESS;
}
void mac_main_cfg_sl_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (periodic_bsr_timer_sl_present) {
    j.write_str("periodic-BSR-TimerSL", periodic_bsr_timer_sl.to_string());
  }
  j.write_str("retx-BSR-TimerSL", retx_bsr_timer_sl.to_string());
  j.end_obj();
}

// PLMN-RAN-AreaCell-r15 ::= SEQUENCE
SRSASN_CODE plmn_ran_area_cell_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(plmn_id_r15_present, 1));

  if (plmn_id_r15_present) {
    HANDLE_CODE(plmn_id_r15.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, ran_area_cells_r15, 1, 32));

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_ran_area_cell_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(plmn_id_r15_present, 1));

  if (plmn_id_r15_present) {
    HANDLE_CODE(plmn_id_r15.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(ran_area_cells_r15, bref, 1, 32));

  return SRSASN_SUCCESS;
}
void plmn_ran_area_cell_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_r15_present) {
    j.write_fieldname("plmn-Identity-r15");
    plmn_id_r15.to_json(j);
  }
  j.start_array("ran-AreaCells-r15");
  for (const auto& e1 : ran_area_cells_r15) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

// PLMN-RAN-AreaConfig-r15 ::= SEQUENCE
SRSASN_CODE plmn_ran_area_cfg_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(plmn_id_r15_present, 1));

  if (plmn_id_r15_present) {
    HANDLE_CODE(plmn_id_r15.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, ran_area_r15, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_ran_area_cfg_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(plmn_id_r15_present, 1));

  if (plmn_id_r15_present) {
    HANDLE_CODE(plmn_id_r15.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(ran_area_r15, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void plmn_ran_area_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_r15_present) {
    j.write_fieldname("plmn-Identity-r15");
    plmn_id_r15.to_json(j);
  }
  j.start_array("ran-Area-r15");
  for (const auto& e1 : ran_area_r15) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// RRCConnectionReconfiguration-v1530-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(security_cfg_ho_v1530_present, 1));
  HANDLE_CODE(bref.pack(scell_group_to_release_list_r15_present, 1));
  HANDLE_CODE(bref.pack(scell_group_to_add_mod_list_r15_present, 1));
  HANDLE_CODE(bref.pack(ded_info_nas_list_r15_present, 1));
  HANDLE_CODE(bref.pack(p_max_ue_fr1_r15_present, 1));
  HANDLE_CODE(bref.pack(smtc_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (security_cfg_ho_v1530_present) {
    HANDLE_CODE(security_cfg_ho_v1530.pack(bref));
  }
  if (scell_group_to_release_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_group_to_release_list_r15, 1, 4, integer_packer<uint8_t>(1, 4)));
  }
  if (scell_group_to_add_mod_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_group_to_add_mod_list_r15, 1, 4));
  }
  if (ded_info_nas_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ded_info_nas_list_r15, 1, 15));
  }
  if (p_max_ue_fr1_r15_present) {
    HANDLE_CODE(pack_integer(bref, p_max_ue_fr1_r15, (int8_t)-30, (int8_t)33));
  }
  if (smtc_r15_present) {
    HANDLE_CODE(smtc_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(security_cfg_ho_v1530_present, 1));
  HANDLE_CODE(bref.unpack(scell_group_to_release_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(scell_group_to_add_mod_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(ded_info_nas_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(p_max_ue_fr1_r15_present, 1));
  HANDLE_CODE(bref.unpack(smtc_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (security_cfg_ho_v1530_present) {
    HANDLE_CODE(security_cfg_ho_v1530.unpack(bref));
  }
  if (scell_group_to_release_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_group_to_release_list_r15, bref, 1, 4, integer_packer<uint8_t>(1, 4)));
  }
  if (scell_group_to_add_mod_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_group_to_add_mod_list_r15, bref, 1, 4));
  }
  if (ded_info_nas_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ded_info_nas_list_r15, bref, 1, 15));
  }
  if (p_max_ue_fr1_r15_present) {
    HANDLE_CODE(unpack_integer(p_max_ue_fr1_r15, bref, (int8_t)-30, (int8_t)33));
  }
  if (smtc_r15_present) {
    HANDLE_CODE(smtc_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (security_cfg_ho_v1530_present) {
    j.write_fieldname("securityConfigHO-v1530");
    security_cfg_ho_v1530.to_json(j);
  }
  if (scell_group_to_release_list_r15_present) {
    j.start_array("sCellGroupToReleaseList-r15");
    for (const auto& e1 : scell_group_to_release_list_r15) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (scell_group_to_add_mod_list_r15_present) {
    j.start_array("sCellGroupToAddModList-r15");
    for (const auto& e1 : scell_group_to_add_mod_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ded_info_nas_list_r15_present) {
    j.start_array("dedicatedInfoNASList-r15");
    for (const auto& e1 : ded_info_nas_list_r15) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (p_max_ue_fr1_r15_present) {
    j.write_int("p-MaxUE-FR1-r15", p_max_ue_fr1_r15);
  }
  if (smtc_r15_present) {
    j.write_fieldname("smtc-r15");
    smtc_r15.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RadioResourceConfigCommonPSCell-r12 ::= SEQUENCE
SRSASN_CODE rr_cfg_common_ps_cell_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(basic_fields_r12.pack(bref));
  HANDLE_CODE(pucch_cfg_common_r12.pack(bref));
  HANDLE_CODE(rach_cfg_common_r12.pack(bref));
  HANDLE_CODE(ul_pwr_ctrl_common_ps_cell_r12.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ul_pwr_ctrl_common_ps_cell_v1310.is_present();
    group_flags[1] |= ul_pwr_ctrl_common_ps_cell_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_pwr_ctrl_common_ps_cell_v1310.is_present(), 1));
      if (ul_pwr_ctrl_common_ps_cell_v1310.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_ps_cell_v1310->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_pwr_ctrl_common_ps_cell_v1530.is_present(), 1));
      if (ul_pwr_ctrl_common_ps_cell_v1530.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_ps_cell_v1530->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_common_ps_cell_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(basic_fields_r12.unpack(bref));
  HANDLE_CODE(pucch_cfg_common_r12.unpack(bref));
  HANDLE_CODE(rach_cfg_common_r12.unpack(bref));
  HANDLE_CODE(ul_pwr_ctrl_common_ps_cell_r12.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ul_pwr_ctrl_common_ps_cell_v1310_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_common_ps_cell_v1310_present, 1));
      ul_pwr_ctrl_common_ps_cell_v1310.set_present(ul_pwr_ctrl_common_ps_cell_v1310_present);
      if (ul_pwr_ctrl_common_ps_cell_v1310.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_ps_cell_v1310->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ul_pwr_ctrl_common_ps_cell_v1530_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_common_ps_cell_v1530_present, 1));
      ul_pwr_ctrl_common_ps_cell_v1530.set_present(ul_pwr_ctrl_common_ps_cell_v1530_present);
      if (ul_pwr_ctrl_common_ps_cell_v1530.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_ps_cell_v1530->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rr_cfg_common_ps_cell_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("basicFields-r12");
  basic_fields_r12.to_json(j);
  j.write_fieldname("pucch-ConfigCommon-r12");
  pucch_cfg_common_r12.to_json(j);
  j.write_fieldname("rach-ConfigCommon-r12");
  rach_cfg_common_r12.to_json(j);
  j.write_fieldname("uplinkPowerControlCommonPSCell-r12");
  ul_pwr_ctrl_common_ps_cell_r12.to_json(j);
  if (ext) {
    if (ul_pwr_ctrl_common_ps_cell_v1310.is_present()) {
      j.write_fieldname("uplinkPowerControlCommonPSCell-v1310");
      ul_pwr_ctrl_common_ps_cell_v1310->to_json(j);
    }
    if (ul_pwr_ctrl_common_ps_cell_v1530.is_present()) {
      j.write_fieldname("uplinkPowerControlCommonPSCell-v1530");
      ul_pwr_ctrl_common_ps_cell_v1530->to_json(j);
    }
  }
  j.end_obj();
}

// RadioResourceConfigCommonPSCell-v1440 ::= SEQUENCE
SRSASN_CODE rr_cfg_common_ps_cell_v1440_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(basic_fields_v1440.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_common_ps_cell_v1440_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(basic_fields_v1440.unpack(bref));

  return SRSASN_SUCCESS;
}
void rr_cfg_common_ps_cell_v1440_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("basicFields-v1440");
  basic_fields_v1440.to_json(j);
  j.end_obj();
}

// RadioResourceConfigDedicatedPSCell-r12 ::= SEQUENCE
SRSASN_CODE rr_cfg_ded_ps_cell_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(phys_cfg_ded_ps_cell_r12_present, 1));
  HANDLE_CODE(bref.pack(sps_cfg_r12_present, 1));
  HANDLE_CODE(bref.pack(naics_info_r12_present, 1));

  if (phys_cfg_ded_ps_cell_r12_present) {
    HANDLE_CODE(phys_cfg_ded_ps_cell_r12.pack(bref));
  }
  if (sps_cfg_r12_present) {
    HANDLE_CODE(sps_cfg_r12.pack(bref));
  }
  if (naics_info_r12_present) {
    HANDLE_CODE(naics_info_r12.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= neigh_cells_crs_info_ps_cell_r13.is_present();
    group_flags[1] |= sps_cfg_v1430.is_present();
    group_flags[2] |= sps_cfg_v1530.is_present();
    group_flags[2] |= crs_intf_mitig_enabled_r15_present;
    group_flags[2] |= neigh_cells_crs_info_r15.is_present();
    group_flags[3] |= sps_cfg_v1540.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(neigh_cells_crs_info_ps_cell_r13.is_present(), 1));
      if (neigh_cells_crs_info_ps_cell_r13.is_present()) {
        HANDLE_CODE(neigh_cells_crs_info_ps_cell_r13->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sps_cfg_v1430.is_present(), 1));
      if (sps_cfg_v1430.is_present()) {
        HANDLE_CODE(sps_cfg_v1430->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sps_cfg_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(crs_intf_mitig_enabled_r15_present, 1));
      HANDLE_CODE(bref.pack(neigh_cells_crs_info_r15.is_present(), 1));
      if (sps_cfg_v1530.is_present()) {
        HANDLE_CODE(sps_cfg_v1530->pack(bref));
      }
      if (crs_intf_mitig_enabled_r15_present) {
        HANDLE_CODE(bref.pack(crs_intf_mitig_enabled_r15, 1));
      }
      if (neigh_cells_crs_info_r15.is_present()) {
        HANDLE_CODE(neigh_cells_crs_info_r15->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sps_cfg_v1540.is_present(), 1));
      if (sps_cfg_v1540.is_present()) {
        HANDLE_CODE(sps_cfg_v1540->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_ded_ps_cell_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(phys_cfg_ded_ps_cell_r12_present, 1));
  HANDLE_CODE(bref.unpack(sps_cfg_r12_present, 1));
  HANDLE_CODE(bref.unpack(naics_info_r12_present, 1));

  if (phys_cfg_ded_ps_cell_r12_present) {
    HANDLE_CODE(phys_cfg_ded_ps_cell_r12.unpack(bref));
  }
  if (sps_cfg_r12_present) {
    HANDLE_CODE(sps_cfg_r12.unpack(bref));
  }
  if (naics_info_r12_present) {
    HANDLE_CODE(naics_info_r12.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool neigh_cells_crs_info_ps_cell_r13_present;
      HANDLE_CODE(bref.unpack(neigh_cells_crs_info_ps_cell_r13_present, 1));
      neigh_cells_crs_info_ps_cell_r13.set_present(neigh_cells_crs_info_ps_cell_r13_present);
      if (neigh_cells_crs_info_ps_cell_r13.is_present()) {
        HANDLE_CODE(neigh_cells_crs_info_ps_cell_r13->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool sps_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(sps_cfg_v1430_present, 1));
      sps_cfg_v1430.set_present(sps_cfg_v1430_present);
      if (sps_cfg_v1430.is_present()) {
        HANDLE_CODE(sps_cfg_v1430->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool sps_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(sps_cfg_v1530_present, 1));
      sps_cfg_v1530.set_present(sps_cfg_v1530_present);
      HANDLE_CODE(bref.unpack(crs_intf_mitig_enabled_r15_present, 1));
      bool neigh_cells_crs_info_r15_present;
      HANDLE_CODE(bref.unpack(neigh_cells_crs_info_r15_present, 1));
      neigh_cells_crs_info_r15.set_present(neigh_cells_crs_info_r15_present);
      if (sps_cfg_v1530.is_present()) {
        HANDLE_CODE(sps_cfg_v1530->unpack(bref));
      }
      if (crs_intf_mitig_enabled_r15_present) {
        HANDLE_CODE(bref.unpack(crs_intf_mitig_enabled_r15, 1));
      }
      if (neigh_cells_crs_info_r15.is_present()) {
        HANDLE_CODE(neigh_cells_crs_info_r15->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool sps_cfg_v1540_present;
      HANDLE_CODE(bref.unpack(sps_cfg_v1540_present, 1));
      sps_cfg_v1540.set_present(sps_cfg_v1540_present);
      if (sps_cfg_v1540.is_present()) {
        HANDLE_CODE(sps_cfg_v1540->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rr_cfg_ded_ps_cell_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phys_cfg_ded_ps_cell_r12_present) {
    j.write_fieldname("physicalConfigDedicatedPSCell-r12");
    phys_cfg_ded_ps_cell_r12.to_json(j);
  }
  if (sps_cfg_r12_present) {
    j.write_fieldname("sps-Config-r12");
    sps_cfg_r12.to_json(j);
  }
  if (naics_info_r12_present) {
    j.write_fieldname("naics-Info-r12");
    naics_info_r12.to_json(j);
  }
  if (ext) {
    if (neigh_cells_crs_info_ps_cell_r13.is_present()) {
      j.write_fieldname("neighCellsCRS-InfoPSCell-r13");
      neigh_cells_crs_info_ps_cell_r13->to_json(j);
    }
    if (sps_cfg_v1430.is_present()) {
      j.write_fieldname("sps-Config-v1430");
      sps_cfg_v1430->to_json(j);
    }
    if (sps_cfg_v1530.is_present()) {
      j.write_fieldname("sps-Config-v1530");
      sps_cfg_v1530->to_json(j);
    }
    if (crs_intf_mitig_enabled_r15_present) {
      j.write_bool("crs-IntfMitigEnabled-r15", crs_intf_mitig_enabled_r15);
    }
    if (neigh_cells_crs_info_r15.is_present()) {
      j.write_fieldname("neighCellsCRS-Info-r15");
      neigh_cells_crs_info_r15->to_json(j);
    }
    if (sps_cfg_v1540.is_present()) {
      j.write_fieldname("sps-Config-v1540");
      sps_cfg_v1540->to_json(j);
    }
  }
  j.end_obj();
}

// RadioResourceConfigDedicatedPSCell-v1370 ::= SEQUENCE
SRSASN_CODE rr_cfg_ded_ps_cell_v1370_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phys_cfg_ded_ps_cell_v1370_present, 1));

  if (phys_cfg_ded_ps_cell_v1370_present) {
    HANDLE_CODE(phys_cfg_ded_ps_cell_v1370.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_ded_ps_cell_v1370_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phys_cfg_ded_ps_cell_v1370_present, 1));

  if (phys_cfg_ded_ps_cell_v1370_present) {
    HANDLE_CODE(phys_cfg_ded_ps_cell_v1370.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rr_cfg_ded_ps_cell_v1370_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phys_cfg_ded_ps_cell_v1370_present) {
    j.write_fieldname("physicalConfigDedicatedPSCell-v1370");
    phys_cfg_ded_ps_cell_v1370.to_json(j);
  }
  j.end_obj();
}

// RadioResourceConfigDedicatedPSCell-v13c0 ::= SEQUENCE
SRSASN_CODE rr_cfg_ded_ps_cell_v13c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(phys_cfg_ded_ps_cell_v13c0.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_ded_ps_cell_v13c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(phys_cfg_ded_ps_cell_v13c0.unpack(bref));

  return SRSASN_SUCCESS;
}
void rr_cfg_ded_ps_cell_v13c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("physicalConfigDedicatedPSCell-v13c0");
  phys_cfg_ded_ps_cell_v13c0.to_json(j);
  j.end_obj();
}

// SL-DiscTxRefCarrierDedicated-r13 ::= CHOICE
void sl_disc_tx_ref_carrier_ded_r13_c::set(types::options e)
{
  type_ = e;
}
void sl_disc_tx_ref_carrier_ded_r13_c::set_pcell()
{
  set(types::pcell);
}
uint8_t& sl_disc_tx_ref_carrier_ded_r13_c::set_scell()
{
  set(types::scell);
  return c;
}
void sl_disc_tx_ref_carrier_ded_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pcell:
      break;
    case types::scell:
      j.write_int("sCell", c);
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_tx_ref_carrier_ded_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_tx_ref_carrier_ded_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::pcell:
      break;
    case types::scell:
      HANDLE_CODE(pack_integer(bref, c, (uint8_t)1u, (uint8_t)7u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_tx_ref_carrier_ded_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_tx_ref_carrier_ded_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::pcell:
      break;
    case types::scell:
      HANDLE_CODE(unpack_integer(c, bref, (uint8_t)1u, (uint8_t)7u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_tx_ref_carrier_ded_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-DiscTxResource-r13 ::= CHOICE
void sl_disc_tx_res_r13_c::set(types::options e)
{
  type_ = e;
}
void sl_disc_tx_res_r13_c::set_release()
{
  set(types::release);
}
sl_disc_tx_res_r13_c::setup_c_& sl_disc_tx_res_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void sl_disc_tx_res_r13_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "sl_disc_tx_res_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_tx_res_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_tx_res_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_tx_res_r13_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "sl_disc_tx_res_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_disc_tx_res_r13_c::setup_c_::destroy_()
{
  switch (type_) {
    case types::sched_r13:
      c.destroy<sl_disc_tx_cfg_sched_r13_s>();
      break;
    case types::ue_sel_r13:
      c.destroy<sl_disc_tx_pool_ded_r13_s>();
      break;
    default:
      break;
  }
}
void sl_disc_tx_res_r13_c::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sched_r13:
      c.init<sl_disc_tx_cfg_sched_r13_s>();
      break;
    case types::ue_sel_r13:
      c.init<sl_disc_tx_pool_ded_r13_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_tx_res_r13_c::setup_c_");
  }
}
sl_disc_tx_res_r13_c::setup_c_::setup_c_(const sl_disc_tx_res_r13_c::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sched_r13:
      c.init(other.c.get<sl_disc_tx_cfg_sched_r13_s>());
      break;
    case types::ue_sel_r13:
      c.init(other.c.get<sl_disc_tx_pool_ded_r13_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_tx_res_r13_c::setup_c_");
  }
}
sl_disc_tx_res_r13_c::setup_c_& sl_disc_tx_res_r13_c::setup_c_::operator=(const sl_disc_tx_res_r13_c::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sched_r13:
      c.set(other.c.get<sl_disc_tx_cfg_sched_r13_s>());
      break;
    case types::ue_sel_r13:
      c.set(other.c.get<sl_disc_tx_pool_ded_r13_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_tx_res_r13_c::setup_c_");
  }

  return *this;
}
sl_disc_tx_cfg_sched_r13_s& sl_disc_tx_res_r13_c::setup_c_::set_sched_r13()
{
  set(types::sched_r13);
  return c.get<sl_disc_tx_cfg_sched_r13_s>();
}
sl_disc_tx_pool_ded_r13_s& sl_disc_tx_res_r13_c::setup_c_::set_ue_sel_r13()
{
  set(types::ue_sel_r13);
  return c.get<sl_disc_tx_pool_ded_r13_s>();
}
void sl_disc_tx_res_r13_c::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sched_r13:
      j.write_fieldname("scheduled-r13");
      c.get<sl_disc_tx_cfg_sched_r13_s>().to_json(j);
      break;
    case types::ue_sel_r13:
      j.write_fieldname("ue-Selected-r13");
      c.get<sl_disc_tx_pool_ded_r13_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_tx_res_r13_c::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_tx_res_r13_c::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sched_r13:
      HANDLE_CODE(c.get<sl_disc_tx_cfg_sched_r13_s>().pack(bref));
      break;
    case types::ue_sel_r13:
      HANDLE_CODE(c.get<sl_disc_tx_pool_ded_r13_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_tx_res_r13_c::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_tx_res_r13_c::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sched_r13:
      HANDLE_CODE(c.get<sl_disc_tx_cfg_sched_r13_s>().unpack(bref));
      break;
    case types::ue_sel_r13:
      HANDLE_CODE(c.get<sl_disc_tx_pool_ded_r13_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_tx_res_r13_c::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// TDM-PatternConfig-r15 ::= CHOICE
void tdm_pattern_cfg_r15_c::set(types::options e)
{
  type_ = e;
}
void tdm_pattern_cfg_r15_c::set_release()
{
  set(types::release);
}
tdm_pattern_cfg_r15_c::setup_s_& tdm_pattern_cfg_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void tdm_pattern_cfg_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("subframeAssignment-r15", c.sf_assign_r15.to_string());
      j.write_int("harq-Offset-r15", c.harq_offset_r15);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "tdm_pattern_cfg_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE tdm_pattern_cfg_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.sf_assign_r15.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.harq_offset_r15, (uint8_t)0u, (uint8_t)9u));
      break;
    default:
      log_invalid_choice_id(type_, "tdm_pattern_cfg_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE tdm_pattern_cfg_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.sf_assign_r15.unpack(bref));
      HANDLE_CODE(unpack_integer(c.harq_offset_r15, bref, (uint8_t)0u, (uint8_t)9u));
      break;
    default:
      log_invalid_choice_id(type_, "tdm_pattern_cfg_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// TunnelConfigLWIP-r13 ::= SEQUENCE
SRSASN_CODE tunnel_cfg_lwip_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ip_address_r13.pack(bref));
  HANDLE_CODE(ike_id_r13.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= lwip_counter_r13_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(lwip_counter_r13_present, 1));
      if (lwip_counter_r13_present) {
        HANDLE_CODE(pack_integer(bref, lwip_counter_r13, (uint32_t)0u, (uint32_t)65535u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE tunnel_cfg_lwip_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ip_address_r13.unpack(bref));
  HANDLE_CODE(ike_id_r13.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(lwip_counter_r13_present, 1));
      if (lwip_counter_r13_present) {
        HANDLE_CODE(unpack_integer(lwip_counter_r13, bref, (uint32_t)0u, (uint32_t)65535u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void tunnel_cfg_lwip_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ip-Address-r13");
  ip_address_r13.to_json(j);
  j.write_fieldname("ike-Identity-r13");
  ike_id_r13.to_json(j);
  if (ext) {
    if (lwip_counter_r13_present) {
      j.write_int("lwip-Counter-r13", lwip_counter_r13);
    }
  }
  j.end_obj();
}

// WLAN-MobilityConfig-r13 ::= SEQUENCE
SRSASN_CODE wlan_mob_cfg_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(wlan_to_release_list_r13_present, 1));
  HANDLE_CODE(bref.pack(wlan_to_add_list_r13_present, 1));
  HANDLE_CODE(bref.pack(assoc_timer_r13_present, 1));
  HANDLE_CODE(bref.pack(success_report_requested_r13_present, 1));

  if (wlan_to_release_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, wlan_to_release_list_r13, 1, 32));
  }
  if (wlan_to_add_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, wlan_to_add_list_r13, 1, 32));
  }
  if (assoc_timer_r13_present) {
    HANDLE_CODE(assoc_timer_r13.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= wlan_suspend_cfg_r14.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(wlan_suspend_cfg_r14.is_present(), 1));
      if (wlan_suspend_cfg_r14.is_present()) {
        HANDLE_CODE(wlan_suspend_cfg_r14->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_mob_cfg_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(wlan_to_release_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(wlan_to_add_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(assoc_timer_r13_present, 1));
  HANDLE_CODE(bref.unpack(success_report_requested_r13_present, 1));

  if (wlan_to_release_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(wlan_to_release_list_r13, bref, 1, 32));
  }
  if (wlan_to_add_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(wlan_to_add_list_r13, bref, 1, 32));
  }
  if (assoc_timer_r13_present) {
    HANDLE_CODE(assoc_timer_r13.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool wlan_suspend_cfg_r14_present;
      HANDLE_CODE(bref.unpack(wlan_suspend_cfg_r14_present, 1));
      wlan_suspend_cfg_r14.set_present(wlan_suspend_cfg_r14_present);
      if (wlan_suspend_cfg_r14.is_present()) {
        HANDLE_CODE(wlan_suspend_cfg_r14->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void wlan_mob_cfg_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (wlan_to_release_list_r13_present) {
    j.start_array("wlan-ToReleaseList-r13");
    for (const auto& e1 : wlan_to_release_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (wlan_to_add_list_r13_present) {
    j.start_array("wlan-ToAddList-r13");
    for (const auto& e1 : wlan_to_add_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (assoc_timer_r13_present) {
    j.write_str("associationTimer-r13", assoc_timer_r13.to_string());
  }
  if (success_report_requested_r13_present) {
    j.write_str("successReportRequested-r13", "true");
  }
  if (ext) {
    if (wlan_suspend_cfg_r14.is_present()) {
      j.write_fieldname("wlan-SuspendConfig-r14");
      wlan_suspend_cfg_r14->to_json(j);
    }
  }
  j.end_obj();
}

const char* wlan_mob_cfg_r13_s::assoc_timer_r13_opts::to_string() const
{
  static const char* options[] = {"s10", "s30", "s60", "s120", "s240"};
  return convert_enum_idx(options, 5, value, "wlan_mob_cfg_r13_s::assoc_timer_r13_e_");
}
uint8_t wlan_mob_cfg_r13_s::assoc_timer_r13_opts::to_number() const
{
  static const uint8_t options[] = {10, 30, 60, 120, 240};
  return map_enum_number(options, 5, value, "wlan_mob_cfg_r13_s::assoc_timer_r13_e_");
}

// LWA-Config-r13 ::= SEQUENCE
SRSASN_CODE lwa_cfg_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(lwa_mob_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(lwa_wt_counter_r13_present, 1));

  if (lwa_mob_cfg_r13_present) {
    HANDLE_CODE(lwa_mob_cfg_r13.pack(bref));
  }
  if (lwa_wt_counter_r13_present) {
    HANDLE_CODE(pack_integer(bref, lwa_wt_counter_r13, (uint32_t)0u, (uint32_t)65535u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= wt_mac_address_r14_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(wt_mac_address_r14_present, 1));
      if (wt_mac_address_r14_present) {
        HANDLE_CODE(wt_mac_address_r14.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE lwa_cfg_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(lwa_mob_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(lwa_wt_counter_r13_present, 1));

  if (lwa_mob_cfg_r13_present) {
    HANDLE_CODE(lwa_mob_cfg_r13.unpack(bref));
  }
  if (lwa_wt_counter_r13_present) {
    HANDLE_CODE(unpack_integer(lwa_wt_counter_r13, bref, (uint32_t)0u, (uint32_t)65535u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(wt_mac_address_r14_present, 1));
      if (wt_mac_address_r14_present) {
        HANDLE_CODE(wt_mac_address_r14.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void lwa_cfg_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (lwa_mob_cfg_r13_present) {
    j.write_fieldname("lwa-MobilityConfig-r13");
    lwa_mob_cfg_r13.to_json(j);
  }
  if (lwa_wt_counter_r13_present) {
    j.write_int("lwa-WT-Counter-r13", lwa_wt_counter_r13);
  }
  if (ext) {
    if (wt_mac_address_r14_present) {
      j.write_str("wt-MAC-Address-r14", wt_mac_address_r14.to_string());
    }
  }
  j.end_obj();
}

// LWIP-Config-r13 ::= SEQUENCE
SRSASN_CODE lwip_cfg_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(lwip_mob_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(tunnel_cfg_lwip_r13_present, 1));

  if (lwip_mob_cfg_r13_present) {
    HANDLE_CODE(lwip_mob_cfg_r13.pack(bref));
  }
  if (tunnel_cfg_lwip_r13_present) {
    HANDLE_CODE(tunnel_cfg_lwip_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE lwip_cfg_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(lwip_mob_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(tunnel_cfg_lwip_r13_present, 1));

  if (lwip_mob_cfg_r13_present) {
    HANDLE_CODE(lwip_mob_cfg_r13.unpack(bref));
  }
  if (tunnel_cfg_lwip_r13_present) {
    HANDLE_CODE(tunnel_cfg_lwip_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void lwip_cfg_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (lwip_mob_cfg_r13_present) {
    j.write_fieldname("lwip-MobilityConfig-r13");
    lwip_mob_cfg_r13.to_json(j);
  }
  if (tunnel_cfg_lwip_r13_present) {
    j.write_fieldname("tunnelConfigLWIP-r13");
    tunnel_cfg_lwip_r13.to_json(j);
  }
  j.end_obj();
}

// MobilityControlInfoSCG-r12 ::= SEQUENCE
SRSASN_CODE mob_ctrl_info_scg_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ue_id_scg_r12_present, 1));
  HANDLE_CODE(bref.pack(rach_cfg_ded_r12_present, 1));
  HANDLE_CODE(bref.pack(ciphering_algorithm_scg_r12_present, 1));

  HANDLE_CODE(t307_r12.pack(bref));
  if (ue_id_scg_r12_present) {
    HANDLE_CODE(ue_id_scg_r12.pack(bref));
  }
  if (rach_cfg_ded_r12_present) {
    HANDLE_CODE(rach_cfg_ded_r12.pack(bref));
  }
  if (ciphering_algorithm_scg_r12_present) {
    HANDLE_CODE(ciphering_algorithm_scg_r12.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= make_before_break_scg_r14_present;
    group_flags[0] |= rach_skip_scg_r14.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(make_before_break_scg_r14_present, 1));
      HANDLE_CODE(bref.pack(rach_skip_scg_r14.is_present(), 1));
      if (rach_skip_scg_r14.is_present()) {
        HANDLE_CODE(rach_skip_scg_r14->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_ctrl_info_scg_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ue_id_scg_r12_present, 1));
  HANDLE_CODE(bref.unpack(rach_cfg_ded_r12_present, 1));
  HANDLE_CODE(bref.unpack(ciphering_algorithm_scg_r12_present, 1));

  HANDLE_CODE(t307_r12.unpack(bref));
  if (ue_id_scg_r12_present) {
    HANDLE_CODE(ue_id_scg_r12.unpack(bref));
  }
  if (rach_cfg_ded_r12_present) {
    HANDLE_CODE(rach_cfg_ded_r12.unpack(bref));
  }
  if (ciphering_algorithm_scg_r12_present) {
    HANDLE_CODE(ciphering_algorithm_scg_r12.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(make_before_break_scg_r14_present, 1));
      bool rach_skip_scg_r14_present;
      HANDLE_CODE(bref.unpack(rach_skip_scg_r14_present, 1));
      rach_skip_scg_r14.set_present(rach_skip_scg_r14_present);
      if (rach_skip_scg_r14.is_present()) {
        HANDLE_CODE(rach_skip_scg_r14->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void mob_ctrl_info_scg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t307-r12", t307_r12.to_string());
  if (ue_id_scg_r12_present) {
    j.write_str("ue-IdentitySCG-r12", ue_id_scg_r12.to_string());
  }
  if (rach_cfg_ded_r12_present) {
    j.write_fieldname("rach-ConfigDedicated-r12");
    rach_cfg_ded_r12.to_json(j);
  }
  if (ciphering_algorithm_scg_r12_present) {
    j.write_str("cipheringAlgorithmSCG-r12", ciphering_algorithm_scg_r12.to_string());
  }
  if (ext) {
    if (make_before_break_scg_r14_present) {
      j.write_str("makeBeforeBreakSCG-r14", "true");
    }
    if (rach_skip_scg_r14.is_present()) {
      j.write_fieldname("rach-SkipSCG-r14");
      rach_skip_scg_r14->to_json(j);
    }
  }
  j.end_obj();
}

const char* mob_ctrl_info_scg_r12_s::t307_r12_opts::to_string() const
{
  static const char* options[] = {"ms50", "ms100", "ms150", "ms200", "ms500", "ms1000", "ms2000", "spare1"};
  return convert_enum_idx(options, 8, value, "mob_ctrl_info_scg_r12_s::t307_r12_e_");
}
uint16_t mob_ctrl_info_scg_r12_s::t307_r12_opts::to_number() const
{
  static const uint16_t options[] = {50, 100, 150, 200, 500, 1000, 2000};
  return map_enum_number(options, 7, value, "mob_ctrl_info_scg_r12_s::t307_r12_e_");
}

// PSCellToAddMod-r12 ::= SEQUENCE
SRSASN_CODE ps_cell_to_add_mod_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_identif_r12_present, 1));
  HANDLE_CODE(bref.pack(rr_cfg_common_ps_cell_r12_present, 1));
  HANDLE_CODE(bref.pack(rr_cfg_ded_ps_cell_r12_present, 1));

  HANDLE_CODE(pack_integer(bref, scell_idx_r12, (uint8_t)1u, (uint8_t)7u));
  if (cell_identif_r12_present) {
    HANDLE_CODE(pack_integer(bref, cell_identif_r12.pci_r12, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(pack_integer(bref, cell_identif_r12.dl_carrier_freq_r12, (uint32_t)0u, (uint32_t)262143u));
  }
  if (rr_cfg_common_ps_cell_r12_present) {
    HANDLE_CODE(rr_cfg_common_ps_cell_r12.pack(bref));
  }
  if (rr_cfg_ded_ps_cell_r12_present) {
    HANDLE_CODE(rr_cfg_ded_ps_cell_r12.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ant_info_ded_ps_cell_v1280.is_present();
    group_flags[1] |= scell_idx_r13_present;
    group_flags[2] |= rr_cfg_ded_ps_cell_v1370.is_present();
    group_flags[3] |= rr_cfg_ded_ps_cell_v13c0.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ant_info_ded_ps_cell_v1280.is_present(), 1));
      if (ant_info_ded_ps_cell_v1280.is_present()) {
        HANDLE_CODE(ant_info_ded_ps_cell_v1280->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(scell_idx_r13_present, 1));
      if (scell_idx_r13_present) {
        HANDLE_CODE(pack_integer(bref, scell_idx_r13, (uint8_t)1u, (uint8_t)31u));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rr_cfg_ded_ps_cell_v1370.is_present(), 1));
      if (rr_cfg_ded_ps_cell_v1370.is_present()) {
        HANDLE_CODE(rr_cfg_ded_ps_cell_v1370->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rr_cfg_ded_ps_cell_v13c0.is_present(), 1));
      if (rr_cfg_ded_ps_cell_v13c0.is_present()) {
        HANDLE_CODE(rr_cfg_ded_ps_cell_v13c0->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ps_cell_to_add_mod_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_identif_r12_present, 1));
  HANDLE_CODE(bref.unpack(rr_cfg_common_ps_cell_r12_present, 1));
  HANDLE_CODE(bref.unpack(rr_cfg_ded_ps_cell_r12_present, 1));

  HANDLE_CODE(unpack_integer(scell_idx_r12, bref, (uint8_t)1u, (uint8_t)7u));
  if (cell_identif_r12_present) {
    HANDLE_CODE(unpack_integer(cell_identif_r12.pci_r12, bref, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(unpack_integer(cell_identif_r12.dl_carrier_freq_r12, bref, (uint32_t)0u, (uint32_t)262143u));
  }
  if (rr_cfg_common_ps_cell_r12_present) {
    HANDLE_CODE(rr_cfg_common_ps_cell_r12.unpack(bref));
  }
  if (rr_cfg_ded_ps_cell_r12_present) {
    HANDLE_CODE(rr_cfg_ded_ps_cell_r12.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ant_info_ded_ps_cell_v1280_present;
      HANDLE_CODE(bref.unpack(ant_info_ded_ps_cell_v1280_present, 1));
      ant_info_ded_ps_cell_v1280.set_present(ant_info_ded_ps_cell_v1280_present);
      if (ant_info_ded_ps_cell_v1280.is_present()) {
        HANDLE_CODE(ant_info_ded_ps_cell_v1280->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(scell_idx_r13_present, 1));
      if (scell_idx_r13_present) {
        HANDLE_CODE(unpack_integer(scell_idx_r13, bref, (uint8_t)1u, (uint8_t)31u));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rr_cfg_ded_ps_cell_v1370_present;
      HANDLE_CODE(bref.unpack(rr_cfg_ded_ps_cell_v1370_present, 1));
      rr_cfg_ded_ps_cell_v1370.set_present(rr_cfg_ded_ps_cell_v1370_present);
      if (rr_cfg_ded_ps_cell_v1370.is_present()) {
        HANDLE_CODE(rr_cfg_ded_ps_cell_v1370->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rr_cfg_ded_ps_cell_v13c0_present;
      HANDLE_CODE(bref.unpack(rr_cfg_ded_ps_cell_v13c0_present, 1));
      rr_cfg_ded_ps_cell_v13c0.set_present(rr_cfg_ded_ps_cell_v13c0_present);
      if (rr_cfg_ded_ps_cell_v13c0.is_present()) {
        HANDLE_CODE(rr_cfg_ded_ps_cell_v13c0->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ps_cell_to_add_mod_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sCellIndex-r12", scell_idx_r12);
  if (cell_identif_r12_present) {
    j.write_fieldname("cellIdentification-r12");
    j.start_obj();
    j.write_int("physCellId-r12", cell_identif_r12.pci_r12);
    j.write_int("dl-CarrierFreq-r12", cell_identif_r12.dl_carrier_freq_r12);
    j.end_obj();
  }
  if (rr_cfg_common_ps_cell_r12_present) {
    j.write_fieldname("radioResourceConfigCommonPSCell-r12");
    rr_cfg_common_ps_cell_r12.to_json(j);
  }
  if (rr_cfg_ded_ps_cell_r12_present) {
    j.write_fieldname("radioResourceConfigDedicatedPSCell-r12");
    rr_cfg_ded_ps_cell_r12.to_json(j);
  }
  if (ext) {
    if (ant_info_ded_ps_cell_v1280.is_present()) {
      j.write_fieldname("antennaInfoDedicatedPSCell-v1280");
      ant_info_ded_ps_cell_v1280->to_json(j);
    }
    if (scell_idx_r13_present) {
      j.write_int("sCellIndex-r13", scell_idx_r13);
    }
    if (rr_cfg_ded_ps_cell_v1370.is_present()) {
      j.write_fieldname("radioResourceConfigDedicatedPSCell-v1370");
      rr_cfg_ded_ps_cell_v1370->to_json(j);
    }
    if (rr_cfg_ded_ps_cell_v13c0.is_present()) {
      j.write_fieldname("radioResourceConfigDedicatedPSCell-v13c0");
      rr_cfg_ded_ps_cell_v13c0->to_json(j);
    }
  }
  j.end_obj();
}

// PSCellToAddMod-v1440 ::= SEQUENCE
SRSASN_CODE ps_cell_to_add_mod_v1440_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rr_cfg_common_ps_cell_r14_present, 1));

  if (rr_cfg_common_ps_cell_r14_present) {
    HANDLE_CODE(rr_cfg_common_ps_cell_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ps_cell_to_add_mod_v1440_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rr_cfg_common_ps_cell_r14_present, 1));

  if (rr_cfg_common_ps_cell_r14_present) {
    HANDLE_CODE(rr_cfg_common_ps_cell_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ps_cell_to_add_mod_v1440_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rr_cfg_common_ps_cell_r14_present) {
    j.write_fieldname("radioResourceConfigCommonPSCell-r14");
    rr_cfg_common_ps_cell_r14.to_json(j);
  }
  j.end_obj();
}

// RCLWI-Config-r13 ::= SEQUENCE
SRSASN_CODE rclwi_cfg_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(cmd.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rclwi_cfg_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(cmd.unpack(bref));

  return SRSASN_SUCCESS;
}
void rclwi_cfg_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("command");
  cmd.to_json(j);
  j.end_obj();
}

void rclwi_cfg_r13_s::cmd_c_::set(types::options e)
{
  type_ = e;
}
rclwi_cfg_r13_s::cmd_c_::steer_to_wlan_r13_s_& rclwi_cfg_r13_s::cmd_c_::set_steer_to_wlan_r13()
{
  set(types::steer_to_wlan_r13);
  return c;
}
void rclwi_cfg_r13_s::cmd_c_::set_steer_to_lte_r13()
{
  set(types::steer_to_lte_r13);
}
void rclwi_cfg_r13_s::cmd_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::steer_to_wlan_r13:
      j.write_fieldname("steerToWLAN-r13");
      j.start_obj();
      j.start_array("mobilityConfig-r13");
      for (const auto& e1 : c.mob_cfg_r13) {
        e1.to_json(j);
      }
      j.end_array();
      j.end_obj();
      break;
    case types::steer_to_lte_r13:
      break;
    default:
      log_invalid_choice_id(type_, "rclwi_cfg_r13_s::cmd_c_");
  }
  j.end_obj();
}
SRSASN_CODE rclwi_cfg_r13_s::cmd_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::steer_to_wlan_r13:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.mob_cfg_r13, 1, 16));
      break;
    case types::steer_to_lte_r13:
      break;
    default:
      log_invalid_choice_id(type_, "rclwi_cfg_r13_s::cmd_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rclwi_cfg_r13_s::cmd_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::steer_to_wlan_r13:
      HANDLE_CODE(unpack_dyn_seq_of(c.mob_cfg_r13, bref, 1, 16));
      break;
    case types::steer_to_lte_r13:
      break;
    default:
      log_invalid_choice_id(type_, "rclwi_cfg_r13_s::cmd_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionReconfiguration-v1510-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v1510_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nr_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(sk_counter_r15_present, 1));
  HANDLE_CODE(bref.pack(nr_radio_bearer_cfg1_r15_present, 1));
  HANDLE_CODE(bref.pack(nr_radio_bearer_cfg2_r15_present, 1));
  HANDLE_CODE(bref.pack(tdm_pattern_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (nr_cfg_r15_present) {
    HANDLE_CODE(nr_cfg_r15.pack(bref));
  }
  if (sk_counter_r15_present) {
    HANDLE_CODE(pack_integer(bref, sk_counter_r15, (uint32_t)0u, (uint32_t)65535u));
  }
  if (nr_radio_bearer_cfg1_r15_present) {
    HANDLE_CODE(nr_radio_bearer_cfg1_r15.pack(bref));
  }
  if (nr_radio_bearer_cfg2_r15_present) {
    HANDLE_CODE(nr_radio_bearer_cfg2_r15.pack(bref));
  }
  if (tdm_pattern_cfg_r15_present) {
    HANDLE_CODE(tdm_pattern_cfg_r15.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v1510_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nr_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(sk_counter_r15_present, 1));
  HANDLE_CODE(bref.unpack(nr_radio_bearer_cfg1_r15_present, 1));
  HANDLE_CODE(bref.unpack(nr_radio_bearer_cfg2_r15_present, 1));
  HANDLE_CODE(bref.unpack(tdm_pattern_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (nr_cfg_r15_present) {
    HANDLE_CODE(nr_cfg_r15.unpack(bref));
  }
  if (sk_counter_r15_present) {
    HANDLE_CODE(unpack_integer(sk_counter_r15, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (nr_radio_bearer_cfg1_r15_present) {
    HANDLE_CODE(nr_radio_bearer_cfg1_r15.unpack(bref));
  }
  if (nr_radio_bearer_cfg2_r15_present) {
    HANDLE_CODE(nr_radio_bearer_cfg2_r15.unpack(bref));
  }
  if (tdm_pattern_cfg_r15_present) {
    HANDLE_CODE(tdm_pattern_cfg_r15.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_v1510_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nr_cfg_r15_present) {
    j.write_fieldname("nr-Config-r15");
    nr_cfg_r15.to_json(j);
  }
  if (sk_counter_r15_present) {
    j.write_int("sk-Counter-r15", sk_counter_r15);
  }
  if (nr_radio_bearer_cfg1_r15_present) {
    j.write_str("nr-RadioBearerConfig1-r15", nr_radio_bearer_cfg1_r15.to_string());
  }
  if (nr_radio_bearer_cfg2_r15_present) {
    j.write_str("nr-RadioBearerConfig2-r15", nr_radio_bearer_cfg2_r15.to_string());
  }
  if (tdm_pattern_cfg_r15_present) {
    j.write_fieldname("tdm-PatternConfig-r15");
    tdm_pattern_cfg_r15.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

void rrc_conn_recfg_v1510_ies_s::nr_cfg_r15_c_::set(types::options e)
{
  type_ = e;
}
void rrc_conn_recfg_v1510_ies_s::nr_cfg_r15_c_::set_release()
{
  set(types::release);
}
rrc_conn_recfg_v1510_ies_s::nr_cfg_r15_c_::setup_s_& rrc_conn_recfg_v1510_ies_s::nr_cfg_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void rrc_conn_recfg_v1510_ies_s::nr_cfg_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_bool("endc-ReleaseAndAdd-r15", c.endc_release_and_add_r15);
      if (c.nr_secondary_cell_group_cfg_r15_present) {
        j.write_str("nr-SecondaryCellGroupConfig-r15", c.nr_secondary_cell_group_cfg_r15.to_string());
      }
      if (c.p_max_eutra_r15_present) {
        j.write_int("p-MaxEUTRA-r15", c.p_max_eutra_r15);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_v1510_ies_s::nr_cfg_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_recfg_v1510_ies_s::nr_cfg_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.nr_secondary_cell_group_cfg_r15_present, 1));
      HANDLE_CODE(bref.pack(c.p_max_eutra_r15_present, 1));
      HANDLE_CODE(bref.pack(c.endc_release_and_add_r15, 1));
      if (c.nr_secondary_cell_group_cfg_r15_present) {
        HANDLE_CODE(c.nr_secondary_cell_group_cfg_r15.pack(bref));
      }
      if (c.p_max_eutra_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.p_max_eutra_r15, (int8_t)-30, (int8_t)33));
      }
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_v1510_ies_s::nr_cfg_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v1510_ies_s::nr_cfg_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.nr_secondary_cell_group_cfg_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.p_max_eutra_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.endc_release_and_add_r15, 1));
      if (c.nr_secondary_cell_group_cfg_r15_present) {
        HANDLE_CODE(c.nr_secondary_cell_group_cfg_r15.unpack(bref));
      }
      if (c.p_max_eutra_r15_present) {
        HANDLE_CODE(unpack_integer(c.p_max_eutra_r15, bref, (int8_t)-30, (int8_t)33));
      }
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_v1510_ies_s::nr_cfg_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-CommTxPoolToAddMod-r12 ::= SEQUENCE
SRSASN_CODE sl_comm_tx_pool_to_add_mod_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pool_id_r12, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pool_r12.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_comm_tx_pool_to_add_mod_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pool_id_r12, bref, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pool_r12.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_comm_tx_pool_to_add_mod_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("poolIdentity-r12", pool_id_r12);
  j.write_fieldname("pool-r12");
  pool_r12.to_json(j);
  j.end_obj();
}

// SL-CommTxPoolToAddModExt-r13 ::= SEQUENCE
SRSASN_CODE sl_comm_tx_pool_to_add_mod_ext_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pool_id_v1310, (uint8_t)5u, (uint8_t)8u));
  HANDLE_CODE(pool_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_comm_tx_pool_to_add_mod_ext_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pool_id_v1310, bref, (uint8_t)5u, (uint8_t)8u));
  HANDLE_CODE(pool_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_comm_tx_pool_to_add_mod_ext_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("poolIdentity-v1310", pool_id_v1310);
  j.write_fieldname("pool-r13");
  pool_r13.to_json(j);
  j.end_obj();
}

// SL-DiscTxResourceInfoPerFreq-r13 ::= SEQUENCE
SRSASN_CODE sl_disc_tx_res_info_per_freq_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(disc_tx_res_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_tx_res_ps_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_tx_ref_carrier_ded_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_cell_sel_info_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, disc_tx_carrier_freq_r13, (uint32_t)0u, (uint32_t)262143u));
  if (disc_tx_res_r13_present) {
    HANDLE_CODE(disc_tx_res_r13.pack(bref));
  }
  if (disc_tx_res_ps_r13_present) {
    HANDLE_CODE(disc_tx_res_ps_r13.pack(bref));
  }
  if (disc_tx_ref_carrier_ded_r13_present) {
    HANDLE_CODE(disc_tx_ref_carrier_ded_r13.pack(bref));
  }
  if (disc_cell_sel_info_r13_present) {
    HANDLE_CODE(disc_cell_sel_info_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_tx_res_info_per_freq_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(disc_tx_res_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_tx_res_ps_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_tx_ref_carrier_ded_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_cell_sel_info_r13_present, 1));

  HANDLE_CODE(unpack_integer(disc_tx_carrier_freq_r13, bref, (uint32_t)0u, (uint32_t)262143u));
  if (disc_tx_res_r13_present) {
    HANDLE_CODE(disc_tx_res_r13.unpack(bref));
  }
  if (disc_tx_res_ps_r13_present) {
    HANDLE_CODE(disc_tx_res_ps_r13.unpack(bref));
  }
  if (disc_tx_ref_carrier_ded_r13_present) {
    HANDLE_CODE(disc_tx_ref_carrier_ded_r13.unpack(bref));
  }
  if (disc_cell_sel_info_r13_present) {
    HANDLE_CODE(disc_cell_sel_info_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_disc_tx_res_info_per_freq_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("discTxCarrierFreq-r13", disc_tx_carrier_freq_r13);
  if (disc_tx_res_r13_present) {
    j.write_fieldname("discTxResources-r13");
    disc_tx_res_r13.to_json(j);
  }
  if (disc_tx_res_ps_r13_present) {
    j.write_fieldname("discTxResourcesPS-r13");
    disc_tx_res_ps_r13.to_json(j);
  }
  if (disc_tx_ref_carrier_ded_r13_present) {
    j.write_fieldname("discTxRefCarrierDedicated-r13");
    disc_tx_ref_carrier_ded_r13.to_json(j);
  }
  if (disc_cell_sel_info_r13_present) {
    j.write_fieldname("discCellSelectionInfo-r13");
    disc_cell_sel_info_r13.to_json(j);
  }
  j.end_obj();
}

// SL-TF-IndexPair-r12 ::= SEQUENCE
SRSASN_CODE sl_tf_idx_pair_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(disc_sf_idx_r12_present, 1));
  HANDLE_CODE(bref.pack(disc_prb_idx_r12_present, 1));

  if (disc_sf_idx_r12_present) {
    HANDLE_CODE(pack_integer(bref, disc_sf_idx_r12, (uint8_t)1u, (uint8_t)200u));
  }
  if (disc_prb_idx_r12_present) {
    HANDLE_CODE(pack_integer(bref, disc_prb_idx_r12, (uint8_t)1u, (uint8_t)50u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tf_idx_pair_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(disc_sf_idx_r12_present, 1));
  HANDLE_CODE(bref.unpack(disc_prb_idx_r12_present, 1));

  if (disc_sf_idx_r12_present) {
    HANDLE_CODE(unpack_integer(disc_sf_idx_r12, bref, (uint8_t)1u, (uint8_t)200u));
  }
  if (disc_prb_idx_r12_present) {
    HANDLE_CODE(unpack_integer(disc_prb_idx_r12, bref, (uint8_t)1u, (uint8_t)50u));
  }

  return SRSASN_SUCCESS;
}
void sl_tf_idx_pair_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (disc_sf_idx_r12_present) {
    j.write_int("discSF-Index-r12", disc_sf_idx_r12);
  }
  if (disc_prb_idx_r12_present) {
    j.write_int("discPRB-Index-r12", disc_prb_idx_r12);
  }
  j.end_obj();
}

// SL-V2X-ConfigDedicated-r14 ::= SEQUENCE
SRSASN_CODE sl_v2x_cfg_ded_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(comm_tx_res_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_inter_freq_info_list_r14_present, 1));
  HANDLE_CODE(bref.pack(thres_sl_tx_prioritization_r14_present, 1));
  HANDLE_CODE(bref.pack(type_tx_sync_r14_present, 1));
  HANDLE_CODE(bref.pack(cbr_ded_tx_cfg_list_r14_present, 1));

  if (comm_tx_res_r14_present) {
    HANDLE_CODE(comm_tx_res_r14.pack(bref));
  }
  if (v2x_inter_freq_info_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_inter_freq_info_list_r14, 0, 7));
  }
  if (thres_sl_tx_prioritization_r14_present) {
    HANDLE_CODE(pack_integer(bref, thres_sl_tx_prioritization_r14, (uint8_t)1u, (uint8_t)8u));
  }
  if (type_tx_sync_r14_present) {
    HANDLE_CODE(type_tx_sync_r14.pack(bref));
  }
  if (cbr_ded_tx_cfg_list_r14_present) {
    HANDLE_CODE(cbr_ded_tx_cfg_list_r14.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= comm_tx_res_v1530.is_present();
    group_flags[0] |= v2x_packet_dupl_cfg_r15.is_present();
    group_flags[0] |= sync_freq_list_r15.is_present();
    group_flags[0] |= slss_tx_multi_freq_r15_present;
    group_flags[1] |= slss_tx_disabled_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(comm_tx_res_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(v2x_packet_dupl_cfg_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(sync_freq_list_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(slss_tx_multi_freq_r15_present, 1));
      if (comm_tx_res_v1530.is_present()) {
        HANDLE_CODE(comm_tx_res_v1530->pack(bref));
      }
      if (v2x_packet_dupl_cfg_r15.is_present()) {
        HANDLE_CODE(v2x_packet_dupl_cfg_r15->pack(bref));
      }
      if (sync_freq_list_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *sync_freq_list_r15, 1, 8, integer_packer<uint32_t>(0, 262143)));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(slss_tx_disabled_r15_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_v2x_cfg_ded_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(comm_tx_res_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_inter_freq_info_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(thres_sl_tx_prioritization_r14_present, 1));
  HANDLE_CODE(bref.unpack(type_tx_sync_r14_present, 1));
  HANDLE_CODE(bref.unpack(cbr_ded_tx_cfg_list_r14_present, 1));

  if (comm_tx_res_r14_present) {
    HANDLE_CODE(comm_tx_res_r14.unpack(bref));
  }
  if (v2x_inter_freq_info_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_inter_freq_info_list_r14, bref, 0, 7));
  }
  if (thres_sl_tx_prioritization_r14_present) {
    HANDLE_CODE(unpack_integer(thres_sl_tx_prioritization_r14, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (type_tx_sync_r14_present) {
    HANDLE_CODE(type_tx_sync_r14.unpack(bref));
  }
  if (cbr_ded_tx_cfg_list_r14_present) {
    HANDLE_CODE(cbr_ded_tx_cfg_list_r14.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool comm_tx_res_v1530_present;
      HANDLE_CODE(bref.unpack(comm_tx_res_v1530_present, 1));
      comm_tx_res_v1530.set_present(comm_tx_res_v1530_present);
      bool v2x_packet_dupl_cfg_r15_present;
      HANDLE_CODE(bref.unpack(v2x_packet_dupl_cfg_r15_present, 1));
      v2x_packet_dupl_cfg_r15.set_present(v2x_packet_dupl_cfg_r15_present);
      bool sync_freq_list_r15_present;
      HANDLE_CODE(bref.unpack(sync_freq_list_r15_present, 1));
      sync_freq_list_r15.set_present(sync_freq_list_r15_present);
      HANDLE_CODE(bref.unpack(slss_tx_multi_freq_r15_present, 1));
      if (comm_tx_res_v1530.is_present()) {
        HANDLE_CODE(comm_tx_res_v1530->unpack(bref));
      }
      if (v2x_packet_dupl_cfg_r15.is_present()) {
        HANDLE_CODE(v2x_packet_dupl_cfg_r15->unpack(bref));
      }
      if (sync_freq_list_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*sync_freq_list_r15, bref, 1, 8, integer_packer<uint32_t>(0, 262143)));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(slss_tx_disabled_r15_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void sl_v2x_cfg_ded_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (comm_tx_res_r14_present) {
    j.write_fieldname("commTxResources-r14");
    comm_tx_res_r14.to_json(j);
  }
  if (v2x_inter_freq_info_list_r14_present) {
    j.start_array("v2x-InterFreqInfoList-r14");
    for (const auto& e1 : v2x_inter_freq_info_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (thres_sl_tx_prioritization_r14_present) {
    j.write_int("thresSL-TxPrioritization-r14", thres_sl_tx_prioritization_r14);
  }
  if (type_tx_sync_r14_present) {
    j.write_str("typeTxSync-r14", type_tx_sync_r14.to_string());
  }
  if (cbr_ded_tx_cfg_list_r14_present) {
    j.write_fieldname("cbr-DedicatedTxConfigList-r14");
    cbr_ded_tx_cfg_list_r14.to_json(j);
  }
  if (ext) {
    if (comm_tx_res_v1530.is_present()) {
      j.write_fieldname("commTxResources-v1530");
      comm_tx_res_v1530->to_json(j);
    }
    if (v2x_packet_dupl_cfg_r15.is_present()) {
      j.write_fieldname("v2x-PacketDuplicationConfig-r15");
      v2x_packet_dupl_cfg_r15->to_json(j);
    }
    if (sync_freq_list_r15.is_present()) {
      j.start_array("syncFreqList-r15");
      for (const auto& e1 : *sync_freq_list_r15) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (slss_tx_multi_freq_r15_present) {
      j.write_str("slss-TxMultiFreq-r15", "true");
    }
    if (slss_tx_disabled_r15_present) {
      j.write_str("slss-TxDisabled-r15", "true");
    }
  }
  j.end_obj();
}

void sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::set(types::options e)
{
  type_ = e;
}
void sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::set_release()
{
  set(types::release);
}
sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_& sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::destroy_()
{
  switch (type_) {
    case types::sched_r14:
      c.destroy<sched_r14_s_>();
      break;
    case types::ue_sel_r14:
      c.destroy<ue_sel_r14_s_>();
      break;
    default:
      break;
  }
}
void sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sched_r14:
      c.init<sched_r14_s_>();
      break;
    case types::ue_sel_r14:
      c.init<ue_sel_r14_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_");
  }
}
sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::setup_c_(
    const sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sched_r14:
      c.init(other.c.get<sched_r14_s_>());
      break;
    case types::ue_sel_r14:
      c.init(other.c.get<ue_sel_r14_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_");
  }
}
sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_& sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::operator=(
    const sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sched_r14:
      c.set(other.c.get<sched_r14_s_>());
      break;
    case types::ue_sel_r14:
      c.set(other.c.get<ue_sel_r14_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_");
  }

  return *this;
}
sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::sched_r14_s_&
sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::set_sched_r14()
{
  set(types::sched_r14);
  return c.get<sched_r14_s_>();
}
sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::ue_sel_r14_s_&
sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::set_ue_sel_r14()
{
  set(types::ue_sel_r14);
  return c.get<ue_sel_r14_s_>();
}
void sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sched_r14:
      j.write_fieldname("scheduled-r14");
      j.start_obj();
      j.write_str("sl-V-RNTI-r14", c.get<sched_r14_s_>().sl_v_rnti_r14.to_string());
      j.write_fieldname("mac-MainConfig-r14");
      c.get<sched_r14_s_>().mac_main_cfg_r14.to_json(j);
      if (c.get<sched_r14_s_>().v2x_sched_pool_r14_present) {
        j.write_fieldname("v2x-SchedulingPool-r14");
        c.get<sched_r14_s_>().v2x_sched_pool_r14.to_json(j);
      }
      if (c.get<sched_r14_s_>().mcs_r14_present) {
        j.write_int("mcs-r14", c.get<sched_r14_s_>().mcs_r14);
      }
      j.start_array("logicalChGroupInfoList-r14");
      for (const auto& e1 : c.get<sched_r14_s_>().lc_ch_group_info_list_r14) {
        j.start_array();
        for (const auto& e2 : e1) {
          j.write_int(e2);
        }
        j.end_array();
      }
      j.end_array();
      j.end_obj();
      break;
    case types::ue_sel_r14:
      j.write_fieldname("ue-Selected-r14");
      j.start_obj();
      j.write_fieldname("v2x-CommTxPoolNormalDedicated-r14");
      j.start_obj();
      if (c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_release_list_r14_present) {
        j.start_array("poolToReleaseList-r14");
        for (const auto& e1 : c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_release_list_r14) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_add_mod_list_r14_present) {
        j.start_array("poolToAddModList-r14");
        for (const auto& e1 : c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_add_mod_list_r14) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.v2x_comm_tx_pool_sensing_cfg_r14_present) {
        j.write_fieldname("v2x-CommTxPoolSensingConfig-r14");
        c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.v2x_comm_tx_pool_sensing_cfg_r14.to_json(j);
      }
      j.end_obj();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sched_r14:
      HANDLE_CODE(bref.pack(c.get<sched_r14_s_>().v2x_sched_pool_r14_present, 1));
      HANDLE_CODE(bref.pack(c.get<sched_r14_s_>().mcs_r14_present, 1));
      HANDLE_CODE(c.get<sched_r14_s_>().sl_v_rnti_r14.pack(bref));
      HANDLE_CODE(c.get<sched_r14_s_>().mac_main_cfg_r14.pack(bref));
      if (c.get<sched_r14_s_>().v2x_sched_pool_r14_present) {
        HANDLE_CODE(c.get<sched_r14_s_>().v2x_sched_pool_r14.pack(bref));
      }
      if (c.get<sched_r14_s_>().mcs_r14_present) {
        HANDLE_CODE(pack_integer(bref, c.get<sched_r14_s_>().mcs_r14, (uint8_t)0u, (uint8_t)31u));
      }
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  c.get<sched_r14_s_>().lc_ch_group_info_list_r14,
                                  1,
                                  4,
                                  SeqOfPacker<integer_packer<uint8_t> >(1, 8, integer_packer<uint8_t>(1, 8))));
      break;
    case types::ue_sel_r14:
      HANDLE_CODE(
          bref.pack(c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_release_list_r14_present, 1));
      HANDLE_CODE(
          bref.pack(c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_add_mod_list_r14_present, 1));
      HANDLE_CODE(bref.pack(
          c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.v2x_comm_tx_pool_sensing_cfg_r14_present, 1));
      if (c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_release_list_r14_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref,
                                    c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_release_list_r14,
                                    1,
                                    8,
                                    integer_packer<uint8_t>(1, 8)));
      }
      if (c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_add_mod_list_r14_present) {
        HANDLE_CODE(pack_dyn_seq_of(
            bref, c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_add_mod_list_r14, 1, 8));
      }
      if (c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.v2x_comm_tx_pool_sensing_cfg_r14_present) {
        HANDLE_CODE(c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.v2x_comm_tx_pool_sensing_cfg_r14.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sched_r14:
      HANDLE_CODE(bref.unpack(c.get<sched_r14_s_>().v2x_sched_pool_r14_present, 1));
      HANDLE_CODE(bref.unpack(c.get<sched_r14_s_>().mcs_r14_present, 1));
      HANDLE_CODE(c.get<sched_r14_s_>().sl_v_rnti_r14.unpack(bref));
      HANDLE_CODE(c.get<sched_r14_s_>().mac_main_cfg_r14.unpack(bref));
      if (c.get<sched_r14_s_>().v2x_sched_pool_r14_present) {
        HANDLE_CODE(c.get<sched_r14_s_>().v2x_sched_pool_r14.unpack(bref));
      }
      if (c.get<sched_r14_s_>().mcs_r14_present) {
        HANDLE_CODE(unpack_integer(c.get<sched_r14_s_>().mcs_r14, bref, (uint8_t)0u, (uint8_t)31u));
      }
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sched_r14_s_>().lc_ch_group_info_list_r14,
                                    bref,
                                    1,
                                    4,
                                    SeqOfPacker<integer_packer<uint8_t> >(1, 8, integer_packer<uint8_t>(1, 8))));
      break;
    case types::ue_sel_r14:
      HANDLE_CODE(
          bref.unpack(c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_release_list_r14_present, 1));
      HANDLE_CODE(
          bref.unpack(c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_add_mod_list_r14_present, 1));
      HANDLE_CODE(bref.unpack(
          c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.v2x_comm_tx_pool_sensing_cfg_r14_present, 1));
      if (c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_release_list_r14_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_release_list_r14,
                                      bref,
                                      1,
                                      8,
                                      integer_packer<uint8_t>(1, 8)));
      }
      if (c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_add_mod_list_r14_present) {
        HANDLE_CODE(unpack_dyn_seq_of(
            c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.pool_to_add_mod_list_r14, bref, 1, 8));
      }
      if (c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.v2x_comm_tx_pool_sensing_cfg_r14_present) {
        HANDLE_CODE(
            c.get<ue_sel_r14_s_>().v2x_comm_tx_pool_normal_ded_r14.v2x_comm_tx_pool_sensing_cfg_r14.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::set(types::options e)
{
  type_ = e;
}
void sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::set_release()
{
  set(types::release);
}
sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_& sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::destroy_()
{
  switch (type_) {
    case types::sched_v1530:
      c.destroy<sched_v1530_s_>();
      break;
    case types::ue_sel_v1530:
      c.destroy<ue_sel_v1530_s_>();
      break;
    default:
      break;
  }
}
void sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sched_v1530:
      c.init<sched_v1530_s_>();
      break;
    case types::ue_sel_v1530:
      c.init<ue_sel_v1530_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_");
  }
}
sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::setup_c_(
    const sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sched_v1530:
      c.init(other.c.get<sched_v1530_s_>());
      break;
    case types::ue_sel_v1530:
      c.init(other.c.get<ue_sel_v1530_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_");
  }
}
sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_& sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::operator=(
    const sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sched_v1530:
      c.set(other.c.get<sched_v1530_s_>());
      break;
    case types::ue_sel_v1530:
      c.set(other.c.get<ue_sel_v1530_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_");
  }

  return *this;
}
sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::sched_v1530_s_&
sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::set_sched_v1530()
{
  set(types::sched_v1530);
  return c.get<sched_v1530_s_>();
}
sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::ue_sel_v1530_s_&
sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::set_ue_sel_v1530()
{
  set(types::ue_sel_v1530);
  return c.get<ue_sel_v1530_s_>();
}
void sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sched_v1530:
      j.write_fieldname("scheduled-v1530");
      j.start_obj();
      if (c.get<sched_v1530_s_>().lc_ch_group_info_list_v1530_present) {
        j.start_array("logicalChGroupInfoList-v1530");
        for (const auto& e1 : c.get<sched_v1530_s_>().lc_ch_group_info_list_v1530) {
          j.start_array();
          for (const auto& e2 : e1) {
            j.write_int(e2);
          }
          j.end_array();
        }
        j.end_array();
      }
      if (c.get<sched_v1530_s_>().mcs_r15_present) {
        j.write_int("mcs-r15", c.get<sched_v1530_s_>().mcs_r15);
      }
      j.end_obj();
      break;
    case types::ue_sel_v1530:
      j.write_fieldname("ue-Selected-v1530");
      j.start_obj();
      if (c.get<ue_sel_v1530_s_>().v2x_freq_sel_cfg_list_r15_present) {
        j.start_array("v2x-FreqSelectionConfigList-r15");
        for (const auto& e1 : c.get<ue_sel_v1530_s_>().v2x_freq_sel_cfg_list_r15) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sched_v1530:
      HANDLE_CODE(bref.pack(c.get<sched_v1530_s_>().lc_ch_group_info_list_v1530_present, 1));
      HANDLE_CODE(bref.pack(c.get<sched_v1530_s_>().mcs_r15_present, 1));
      if (c.get<sched_v1530_s_>().lc_ch_group_info_list_v1530_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref,
                                    c.get<sched_v1530_s_>().lc_ch_group_info_list_v1530,
                                    1,
                                    4,
                                    SeqOfPacker<integer_packer<uint8_t> >(1, 8, integer_packer<uint8_t>(1, 8))));
      }
      if (c.get<sched_v1530_s_>().mcs_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.get<sched_v1530_s_>().mcs_r15, (uint8_t)0u, (uint8_t)31u));
      }
      break;
    case types::ue_sel_v1530:
      HANDLE_CODE(bref.pack(c.get<ue_sel_v1530_s_>().v2x_freq_sel_cfg_list_r15_present, 1));
      if (c.get<ue_sel_v1530_s_>().v2x_freq_sel_cfg_list_r15_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ue_sel_v1530_s_>().v2x_freq_sel_cfg_list_r15, 1, 8));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sched_v1530:
      HANDLE_CODE(bref.unpack(c.get<sched_v1530_s_>().lc_ch_group_info_list_v1530_present, 1));
      HANDLE_CODE(bref.unpack(c.get<sched_v1530_s_>().mcs_r15_present, 1));
      if (c.get<sched_v1530_s_>().lc_ch_group_info_list_v1530_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.get<sched_v1530_s_>().lc_ch_group_info_list_v1530,
                                      bref,
                                      1,
                                      4,
                                      SeqOfPacker<integer_packer<uint8_t> >(1, 8, integer_packer<uint8_t>(1, 8))));
      }
      if (c.get<sched_v1530_s_>().mcs_r15_present) {
        HANDLE_CODE(unpack_integer(c.get<sched_v1530_s_>().mcs_r15, bref, (uint8_t)0u, (uint8_t)31u));
      }
      break;
    case types::ue_sel_v1530:
      HANDLE_CODE(bref.unpack(c.get<ue_sel_v1530_s_>().v2x_freq_sel_cfg_list_r15_present, 1));
      if (c.get<ue_sel_v1530_s_>().v2x_freq_sel_cfg_list_r15_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.get<ue_sel_v1530_s_>().v2x_freq_sel_cfg_list_r15, bref, 1, 8));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// LWA-Configuration-r13 ::= CHOICE
void lwa_cfg_r13_c::set(types::options e)
{
  type_ = e;
}
void lwa_cfg_r13_c::set_release()
{
  set(types::release);
}
lwa_cfg_r13_c::setup_s_& lwa_cfg_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void lwa_cfg_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("lwa-Config-r13");
      c.lwa_cfg_r13.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "lwa_cfg_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE lwa_cfg_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.lwa_cfg_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "lwa_cfg_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE lwa_cfg_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.lwa_cfg_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "lwa_cfg_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// LWIP-Configuration-r13 ::= CHOICE
void lwip_cfg_r13_c::set(types::options e)
{
  type_ = e;
}
void lwip_cfg_r13_c::set_release()
{
  set(types::release);
}
lwip_cfg_r13_c::setup_s_& lwip_cfg_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void lwip_cfg_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("lwip-Config-r13");
      c.lwip_cfg_r13.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "lwip_cfg_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE lwip_cfg_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.lwip_cfg_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "lwip_cfg_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE lwip_cfg_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.lwip_cfg_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "lwip_cfg_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// PowerCoordinationInfo-r12 ::= SEQUENCE
SRSASN_CODE pwr_coordination_info_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, p_menb_r12, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(pack_integer(bref, p_senb_r12, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(pack_integer(bref, pwr_ctrl_mode_r12, (uint8_t)1u, (uint8_t)2u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pwr_coordination_info_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(p_menb_r12, bref, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(unpack_integer(p_senb_r12, bref, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(unpack_integer(pwr_ctrl_mode_r12, bref, (uint8_t)1u, (uint8_t)2u));

  return SRSASN_SUCCESS;
}
void pwr_coordination_info_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p-MeNB-r12", p_menb_r12);
  j.write_int("p-SeNB-r12", p_senb_r12);
  j.write_int("powerControlMode-r12", pwr_ctrl_mode_r12);
  j.end_obj();
}

// RAN-NotificationAreaInfo-r15 ::= CHOICE
void ran_notif_area_info_r15_c::destroy_()
{
  switch (type_) {
    case types::cell_list_r15:
      c.destroy<plmn_ran_area_cell_list_r15_l>();
      break;
    case types::ran_area_cfg_list_r15:
      c.destroy<plmn_ran_area_cfg_list_r15_l>();
      break;
    default:
      break;
  }
}
void ran_notif_area_info_r15_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_list_r15:
      c.init<plmn_ran_area_cell_list_r15_l>();
      break;
    case types::ran_area_cfg_list_r15:
      c.init<plmn_ran_area_cfg_list_r15_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ran_notif_area_info_r15_c");
  }
}
ran_notif_area_info_r15_c::ran_notif_area_info_r15_c(const ran_notif_area_info_r15_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_list_r15:
      c.init(other.c.get<plmn_ran_area_cell_list_r15_l>());
      break;
    case types::ran_area_cfg_list_r15:
      c.init(other.c.get<plmn_ran_area_cfg_list_r15_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ran_notif_area_info_r15_c");
  }
}
ran_notif_area_info_r15_c& ran_notif_area_info_r15_c::operator=(const ran_notif_area_info_r15_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_list_r15:
      c.set(other.c.get<plmn_ran_area_cell_list_r15_l>());
      break;
    case types::ran_area_cfg_list_r15:
      c.set(other.c.get<plmn_ran_area_cfg_list_r15_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ran_notif_area_info_r15_c");
  }

  return *this;
}
plmn_ran_area_cell_list_r15_l& ran_notif_area_info_r15_c::set_cell_list_r15()
{
  set(types::cell_list_r15);
  return c.get<plmn_ran_area_cell_list_r15_l>();
}
plmn_ran_area_cfg_list_r15_l& ran_notif_area_info_r15_c::set_ran_area_cfg_list_r15()
{
  set(types::ran_area_cfg_list_r15);
  return c.get<plmn_ran_area_cfg_list_r15_l>();
}
void ran_notif_area_info_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_list_r15:
      j.start_array("cellList-r15");
      for (const auto& e1 : c.get<plmn_ran_area_cell_list_r15_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ran_area_cfg_list_r15:
      j.start_array("ran-AreaConfigList-r15");
      for (const auto& e1 : c.get<plmn_ran_area_cfg_list_r15_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ran_notif_area_info_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE ran_notif_area_info_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_list_r15:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<plmn_ran_area_cell_list_r15_l>(), 1, 8));
      break;
    case types::ran_area_cfg_list_r15:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<plmn_ran_area_cfg_list_r15_l>(), 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "ran_notif_area_info_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_notif_area_info_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_list_r15:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<plmn_ran_area_cell_list_r15_l>(), bref, 1, 8));
      break;
    case types::ran_area_cfg_list_r15:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<plmn_ran_area_cfg_list_r15_l>(), bref, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "ran_notif_area_info_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RCLWI-Configuration-r13 ::= CHOICE
void rclwi_cfg_r13_c::set(types::options e)
{
  type_ = e;
}
void rclwi_cfg_r13_c::set_release()
{
  set(types::release);
}
rclwi_cfg_r13_c::setup_s_& rclwi_cfg_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void rclwi_cfg_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("rclwi-Config-r13");
      c.rclwi_cfg_r13.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rclwi_cfg_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE rclwi_cfg_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.rclwi_cfg_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rclwi_cfg_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rclwi_cfg_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.rclwi_cfg_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rclwi_cfg_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionReconfiguration-v1430-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_v2x_cfg_ded_r14_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_ext_v1430_present, 1));
  HANDLE_CODE(bref.pack(per_cc_gap_ind_request_r14_present, 1));
  HANDLE_CODE(bref.pack(sib_type2_ded_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sl_v2x_cfg_ded_r14_present) {
    HANDLE_CODE(sl_v2x_cfg_ded_r14.pack(bref));
  }
  if (scell_to_add_mod_list_ext_v1430_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_ext_v1430, 1, 31));
  }
  if (sib_type2_ded_r14_present) {
    HANDLE_CODE(sib_type2_ded_r14.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_v2x_cfg_ded_r14_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_ext_v1430_present, 1));
  HANDLE_CODE(bref.unpack(per_cc_gap_ind_request_r14_present, 1));
  HANDLE_CODE(bref.unpack(sib_type2_ded_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sl_v2x_cfg_ded_r14_present) {
    HANDLE_CODE(sl_v2x_cfg_ded_r14.unpack(bref));
  }
  if (scell_to_add_mod_list_ext_v1430_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_ext_v1430, bref, 1, 31));
  }
  if (sib_type2_ded_r14_present) {
    HANDLE_CODE(sib_type2_ded_r14.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_v2x_cfg_ded_r14_present) {
    j.write_fieldname("sl-V2X-ConfigDedicated-r14");
    sl_v2x_cfg_ded_r14.to_json(j);
  }
  if (scell_to_add_mod_list_ext_v1430_present) {
    j.start_array("sCellToAddModListExt-v1430");
    for (const auto& e1 : scell_to_add_mod_list_ext_v1430) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (per_cc_gap_ind_request_r14_present) {
    j.write_str("perCC-GapIndicationRequest-r14", "true");
  }
  if (sib_type2_ded_r14_present) {
    j.write_str("systemInformationBlockType2Dedicated-r14", sib_type2_ded_r14.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionRelease-v15b0-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_release_v15b0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(no_last_cell_upd_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_v15b0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(no_last_cell_upd_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void rrc_conn_release_v15b0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (no_last_cell_upd_r15_present) {
    j.write_str("noLastCellUpdate-r15", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SCG-ConfigPartSCG-r12 ::= SEQUENCE
SRSASN_CODE scg_cfg_part_scg_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rr_cfg_ded_scg_r12_present, 1));
  HANDLE_CODE(bref.pack(scell_to_release_list_scg_r12_present, 1));
  HANDLE_CODE(bref.pack(pscell_to_add_mod_r12_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_scg_r12_present, 1));
  HANDLE_CODE(bref.pack(mob_ctrl_info_scg_r12_present, 1));

  if (rr_cfg_ded_scg_r12_present) {
    HANDLE_CODE(rr_cfg_ded_scg_r12.pack(bref));
  }
  if (scell_to_release_list_scg_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_release_list_scg_r12, 1, 4, integer_packer<uint8_t>(1, 7)));
  }
  if (pscell_to_add_mod_r12_present) {
    HANDLE_CODE(pscell_to_add_mod_r12.pack(bref));
  }
  if (scell_to_add_mod_list_scg_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_scg_r12, 1, 4));
  }
  if (mob_ctrl_info_scg_r12_present) {
    HANDLE_CODE(mob_ctrl_info_scg_r12.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= scell_to_release_list_scg_ext_r13.is_present();
    group_flags[0] |= scell_to_add_mod_list_scg_ext_r13.is_present();
    group_flags[1] |= scell_to_add_mod_list_scg_ext_v1370.is_present();
    group_flags[2] |= pscell_to_add_mod_v1440.is_present();
    group_flags[3] |= scell_group_to_release_list_scg_r15.is_present();
    group_flags[3] |= scell_group_to_add_mod_list_scg_r15.is_present();
    group_flags[4] |= meas_cfg_sn_r15.is_present();
    group_flags[4] |= tdm_pattern_cfg_ne_dc_r15.is_present();
    group_flags[5] |= p_max_eutra_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(scell_to_release_list_scg_ext_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(scell_to_add_mod_list_scg_ext_r13.is_present(), 1));
      if (scell_to_release_list_scg_ext_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *scell_to_release_list_scg_ext_r13, 1, 31, integer_packer<uint8_t>(1, 31)));
      }
      if (scell_to_add_mod_list_scg_ext_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *scell_to_add_mod_list_scg_ext_r13, 1, 31));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(scell_to_add_mod_list_scg_ext_v1370.is_present(), 1));
      if (scell_to_add_mod_list_scg_ext_v1370.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *scell_to_add_mod_list_scg_ext_v1370, 1, 31));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pscell_to_add_mod_v1440.is_present(), 1));
      if (pscell_to_add_mod_v1440.is_present()) {
        HANDLE_CODE(pscell_to_add_mod_v1440->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(scell_group_to_release_list_scg_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(scell_group_to_add_mod_list_scg_r15.is_present(), 1));
      if (scell_group_to_release_list_scg_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *scell_group_to_release_list_scg_r15, 1, 4, integer_packer<uint8_t>(1, 4)));
      }
      if (scell_group_to_add_mod_list_scg_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *scell_group_to_add_mod_list_scg_r15, 1, 4));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_cfg_sn_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(tdm_pattern_cfg_ne_dc_r15.is_present(), 1));
      if (meas_cfg_sn_r15.is_present()) {
        HANDLE_CODE(meas_cfg_sn_r15->pack(bref));
      }
      if (tdm_pattern_cfg_ne_dc_r15.is_present()) {
        HANDLE_CODE(tdm_pattern_cfg_ne_dc_r15->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(p_max_eutra_r15_present, 1));
      if (p_max_eutra_r15_present) {
        HANDLE_CODE(pack_integer(bref, p_max_eutra_r15, (int8_t)-30, (int8_t)33));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_part_scg_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rr_cfg_ded_scg_r12_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_release_list_scg_r12_present, 1));
  HANDLE_CODE(bref.unpack(pscell_to_add_mod_r12_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_scg_r12_present, 1));
  HANDLE_CODE(bref.unpack(mob_ctrl_info_scg_r12_present, 1));

  if (rr_cfg_ded_scg_r12_present) {
    HANDLE_CODE(rr_cfg_ded_scg_r12.unpack(bref));
  }
  if (scell_to_release_list_scg_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_release_list_scg_r12, bref, 1, 4, integer_packer<uint8_t>(1, 7)));
  }
  if (pscell_to_add_mod_r12_present) {
    HANDLE_CODE(pscell_to_add_mod_r12.unpack(bref));
  }
  if (scell_to_add_mod_list_scg_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_scg_r12, bref, 1, 4));
  }
  if (mob_ctrl_info_scg_r12_present) {
    HANDLE_CODE(mob_ctrl_info_scg_r12.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(6);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool scell_to_release_list_scg_ext_r13_present;
      HANDLE_CODE(bref.unpack(scell_to_release_list_scg_ext_r13_present, 1));
      scell_to_release_list_scg_ext_r13.set_present(scell_to_release_list_scg_ext_r13_present);
      bool scell_to_add_mod_list_scg_ext_r13_present;
      HANDLE_CODE(bref.unpack(scell_to_add_mod_list_scg_ext_r13_present, 1));
      scell_to_add_mod_list_scg_ext_r13.set_present(scell_to_add_mod_list_scg_ext_r13_present);
      if (scell_to_release_list_scg_ext_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*scell_to_release_list_scg_ext_r13, bref, 1, 31, integer_packer<uint8_t>(1, 31)));
      }
      if (scell_to_add_mod_list_scg_ext_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*scell_to_add_mod_list_scg_ext_r13, bref, 1, 31));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool scell_to_add_mod_list_scg_ext_v1370_present;
      HANDLE_CODE(bref.unpack(scell_to_add_mod_list_scg_ext_v1370_present, 1));
      scell_to_add_mod_list_scg_ext_v1370.set_present(scell_to_add_mod_list_scg_ext_v1370_present);
      if (scell_to_add_mod_list_scg_ext_v1370.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*scell_to_add_mod_list_scg_ext_v1370, bref, 1, 31));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool pscell_to_add_mod_v1440_present;
      HANDLE_CODE(bref.unpack(pscell_to_add_mod_v1440_present, 1));
      pscell_to_add_mod_v1440.set_present(pscell_to_add_mod_v1440_present);
      if (pscell_to_add_mod_v1440.is_present()) {
        HANDLE_CODE(pscell_to_add_mod_v1440->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool scell_group_to_release_list_scg_r15_present;
      HANDLE_CODE(bref.unpack(scell_group_to_release_list_scg_r15_present, 1));
      scell_group_to_release_list_scg_r15.set_present(scell_group_to_release_list_scg_r15_present);
      bool scell_group_to_add_mod_list_scg_r15_present;
      HANDLE_CODE(bref.unpack(scell_group_to_add_mod_list_scg_r15_present, 1));
      scell_group_to_add_mod_list_scg_r15.set_present(scell_group_to_add_mod_list_scg_r15_present);
      if (scell_group_to_release_list_scg_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*scell_group_to_release_list_scg_r15, bref, 1, 4, integer_packer<uint8_t>(1, 4)));
      }
      if (scell_group_to_add_mod_list_scg_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*scell_group_to_add_mod_list_scg_r15, bref, 1, 4));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_cfg_sn_r15_present;
      HANDLE_CODE(bref.unpack(meas_cfg_sn_r15_present, 1));
      meas_cfg_sn_r15.set_present(meas_cfg_sn_r15_present);
      bool tdm_pattern_cfg_ne_dc_r15_present;
      HANDLE_CODE(bref.unpack(tdm_pattern_cfg_ne_dc_r15_present, 1));
      tdm_pattern_cfg_ne_dc_r15.set_present(tdm_pattern_cfg_ne_dc_r15_present);
      if (meas_cfg_sn_r15.is_present()) {
        HANDLE_CODE(meas_cfg_sn_r15->unpack(bref));
      }
      if (tdm_pattern_cfg_ne_dc_r15.is_present()) {
        HANDLE_CODE(tdm_pattern_cfg_ne_dc_r15->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(p_max_eutra_r15_present, 1));
      if (p_max_eutra_r15_present) {
        HANDLE_CODE(unpack_integer(p_max_eutra_r15, bref, (int8_t)-30, (int8_t)33));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void scg_cfg_part_scg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rr_cfg_ded_scg_r12_present) {
    j.write_fieldname("radioResourceConfigDedicatedSCG-r12");
    rr_cfg_ded_scg_r12.to_json(j);
  }
  if (scell_to_release_list_scg_r12_present) {
    j.start_array("sCellToReleaseListSCG-r12");
    for (const auto& e1 : scell_to_release_list_scg_r12) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (pscell_to_add_mod_r12_present) {
    j.write_fieldname("pSCellToAddMod-r12");
    pscell_to_add_mod_r12.to_json(j);
  }
  if (scell_to_add_mod_list_scg_r12_present) {
    j.start_array("sCellToAddModListSCG-r12");
    for (const auto& e1 : scell_to_add_mod_list_scg_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (mob_ctrl_info_scg_r12_present) {
    j.write_fieldname("mobilityControlInfoSCG-r12");
    mob_ctrl_info_scg_r12.to_json(j);
  }
  if (ext) {
    if (scell_to_release_list_scg_ext_r13.is_present()) {
      j.start_array("sCellToReleaseListSCG-Ext-r13");
      for (const auto& e1 : *scell_to_release_list_scg_ext_r13) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (scell_to_add_mod_list_scg_ext_r13.is_present()) {
      j.start_array("sCellToAddModListSCG-Ext-r13");
      for (const auto& e1 : *scell_to_add_mod_list_scg_ext_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (scell_to_add_mod_list_scg_ext_v1370.is_present()) {
      j.start_array("sCellToAddModListSCG-Ext-v1370");
      for (const auto& e1 : *scell_to_add_mod_list_scg_ext_v1370) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (pscell_to_add_mod_v1440.is_present()) {
      j.write_fieldname("pSCellToAddMod-v1440");
      pscell_to_add_mod_v1440->to_json(j);
    }
    if (scell_group_to_release_list_scg_r15.is_present()) {
      j.start_array("sCellGroupToReleaseListSCG-r15");
      for (const auto& e1 : *scell_group_to_release_list_scg_r15) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (scell_group_to_add_mod_list_scg_r15.is_present()) {
      j.start_array("sCellGroupToAddModListSCG-r15");
      for (const auto& e1 : *scell_group_to_add_mod_list_scg_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_cfg_sn_r15.is_present()) {
      j.write_fieldname("measConfigSN-r15");
      meas_cfg_sn_r15->to_json(j);
    }
    if (tdm_pattern_cfg_ne_dc_r15.is_present()) {
      j.write_fieldname("tdm-PatternConfigNE-DC-r15");
      tdm_pattern_cfg_ne_dc_r15->to_json(j);
    }
    if (p_max_eutra_r15_present) {
      j.write_int("p-MaxEUTRA-r15", p_max_eutra_r15);
    }
  }
  j.end_obj();
}

// SL-DiscTxInfoInterFreqListAdd-r13 ::= SEQUENCE
SRSASN_CODE sl_disc_tx_info_inter_freq_list_add_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(disc_tx_freq_to_add_mod_list_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_tx_freq_to_release_list_r13_present, 1));

  if (disc_tx_freq_to_add_mod_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, disc_tx_freq_to_add_mod_list_r13, 1, 8));
  }
  if (disc_tx_freq_to_release_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, disc_tx_freq_to_release_list_r13, 1, 8, integer_packer<uint32_t>(0, 262143)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_tx_info_inter_freq_list_add_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(disc_tx_freq_to_add_mod_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_tx_freq_to_release_list_r13_present, 1));

  if (disc_tx_freq_to_add_mod_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(disc_tx_freq_to_add_mod_list_r13, bref, 1, 8));
  }
  if (disc_tx_freq_to_release_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(disc_tx_freq_to_release_list_r13, bref, 1, 8, integer_packer<uint32_t>(0, 262143)));
  }

  return SRSASN_SUCCESS;
}
void sl_disc_tx_info_inter_freq_list_add_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (disc_tx_freq_to_add_mod_list_r13_present) {
    j.start_array("discTxFreqToAddModList-r13");
    for (const auto& e1 : disc_tx_freq_to_add_mod_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (disc_tx_freq_to_release_list_r13_present) {
    j.start_array("discTxFreqToReleaseList-r13");
    for (const auto& e1 : disc_tx_freq_to_release_list_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-GapConfig-r13 ::= SEQUENCE
SRSASN_CODE sl_gap_cfg_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, gap_pattern_list_r13, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_gap_cfg_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(gap_pattern_list_r13, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void sl_gap_cfg_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("gapPatternList-r13");
  for (const auto& e1 : gap_pattern_list_r13) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// FlightPathInfoReportConfig-r15 ::= SEQUENCE
SRSASN_CODE flight_path_info_report_cfg_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(include_time_stamp_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, max_way_point_num_r15, (uint8_t)1u, (uint8_t)20u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE flight_path_info_report_cfg_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(include_time_stamp_r15_present, 1));

  HANDLE_CODE(unpack_integer(max_way_point_num_r15, bref, (uint8_t)1u, (uint8_t)20u));

  return SRSASN_SUCCESS;
}
void flight_path_info_report_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("maxWayPointNumber-r15", max_way_point_num_r15);
  if (include_time_stamp_r15_present) {
    j.write_str("includeTimeStamp-r15", "true");
  }
  j.end_obj();
}

// MeasIdleConfigDedicated-r15 ::= SEQUENCE
SRSASN_CODE meas_idle_cfg_ded_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_idle_carrier_list_eutra_r15_present, 1));

  if (meas_idle_carrier_list_eutra_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_idle_carrier_list_eutra_r15, 1, 8));
  }
  HANDLE_CODE(meas_idle_dur_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_idle_cfg_ded_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_idle_carrier_list_eutra_r15_present, 1));

  if (meas_idle_carrier_list_eutra_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_idle_carrier_list_eutra_r15, bref, 1, 8));
  }
  HANDLE_CODE(meas_idle_dur_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_idle_cfg_ded_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_idle_carrier_list_eutra_r15_present) {
    j.start_array("measIdleCarrierListEUTRA-r15");
    for (const auto& e1 : meas_idle_carrier_list_eutra_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("measIdleDuration-r15", meas_idle_dur_r15.to_string());
  j.end_obj();
}

const char* meas_idle_cfg_ded_r15_s::meas_idle_dur_r15_opts::to_string() const
{
  static const char* options[] = {"sec10", "sec30", "sec60", "sec120", "sec180", "sec240", "sec300", "spare"};
  return convert_enum_idx(options, 8, value, "meas_idle_cfg_ded_r15_s::meas_idle_dur_r15_e_");
}
uint16_t meas_idle_cfg_ded_r15_s::meas_idle_dur_r15_opts::to_number() const
{
  static const uint16_t options[] = {10, 30, 60, 120, 180, 240, 300};
  return map_enum_number(options, 7, value, "meas_idle_cfg_ded_r15_s::meas_idle_dur_r15_e_");
}

// RRC-InactiveConfig-r15 ::= SEQUENCE
SRSASN_CODE rrc_inactive_cfg_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ran_paging_cycle_r15_present, 1));
  HANDLE_CODE(bref.pack(ran_notif_area_info_r15_present, 1));
  HANDLE_CODE(bref.pack(periodic_rnau_timer_r15_present, 1));
  HANDLE_CODE(bref.pack(next_hop_chaining_count_r15_present, 1));
  HANDLE_CODE(bref.pack(dummy_present, 1));

  HANDLE_CODE(full_i_rnti_r15.pack(bref));
  HANDLE_CODE(short_i_rnti_r15.pack(bref));
  if (ran_paging_cycle_r15_present) {
    HANDLE_CODE(ran_paging_cycle_r15.pack(bref));
  }
  if (ran_notif_area_info_r15_present) {
    HANDLE_CODE(ran_notif_area_info_r15.pack(bref));
  }
  if (periodic_rnau_timer_r15_present) {
    HANDLE_CODE(periodic_rnau_timer_r15.pack(bref));
  }
  if (next_hop_chaining_count_r15_present) {
    HANDLE_CODE(pack_integer(bref, next_hop_chaining_count_r15, (uint8_t)0u, (uint8_t)7u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_inactive_cfg_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ran_paging_cycle_r15_present, 1));
  HANDLE_CODE(bref.unpack(ran_notif_area_info_r15_present, 1));
  HANDLE_CODE(bref.unpack(periodic_rnau_timer_r15_present, 1));
  HANDLE_CODE(bref.unpack(next_hop_chaining_count_r15_present, 1));
  HANDLE_CODE(bref.unpack(dummy_present, 1));

  HANDLE_CODE(full_i_rnti_r15.unpack(bref));
  HANDLE_CODE(short_i_rnti_r15.unpack(bref));
  if (ran_paging_cycle_r15_present) {
    HANDLE_CODE(ran_paging_cycle_r15.unpack(bref));
  }
  if (ran_notif_area_info_r15_present) {
    HANDLE_CODE(ran_notif_area_info_r15.unpack(bref));
  }
  if (periodic_rnau_timer_r15_present) {
    HANDLE_CODE(periodic_rnau_timer_r15.unpack(bref));
  }
  if (next_hop_chaining_count_r15_present) {
    HANDLE_CODE(unpack_integer(next_hop_chaining_count_r15, bref, (uint8_t)0u, (uint8_t)7u));
  }

  return SRSASN_SUCCESS;
}
void rrc_inactive_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("fullI-RNTI-r15", full_i_rnti_r15.to_string());
  j.write_str("shortI-RNTI-r15", short_i_rnti_r15.to_string());
  if (ran_paging_cycle_r15_present) {
    j.write_str("ran-PagingCycle-r15", ran_paging_cycle_r15.to_string());
  }
  if (ran_notif_area_info_r15_present) {
    j.write_fieldname("ran-NotificationAreaInfo-r15");
    ran_notif_area_info_r15.to_json(j);
  }
  if (periodic_rnau_timer_r15_present) {
    j.write_str("periodic-RNAU-timer-r15", periodic_rnau_timer_r15.to_string());
  }
  if (next_hop_chaining_count_r15_present) {
    j.write_int("nextHopChainingCount-r15", next_hop_chaining_count_r15);
  }
  if (dummy_present) {
    j.write_fieldname("dummy");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* rrc_inactive_cfg_r15_s::ran_paging_cycle_r15_opts::to_string() const
{
  static const char* options[] = {"rf32", "rf64", "rf128", "rf256"};
  return convert_enum_idx(options, 4, value, "rrc_inactive_cfg_r15_s::ran_paging_cycle_r15_e_");
}
uint16_t rrc_inactive_cfg_r15_s::ran_paging_cycle_r15_opts::to_number() const
{
  static const uint16_t options[] = {32, 64, 128, 256};
  return map_enum_number(options, 4, value, "rrc_inactive_cfg_r15_s::ran_paging_cycle_r15_e_");
}

const char* rrc_inactive_cfg_r15_s::periodic_rnau_timer_r15_opts::to_string() const
{
  static const char* options[] = {"min5", "min10", "min20", "min30", "min60", "min120", "min360", "min720"};
  return convert_enum_idx(options, 8, value, "rrc_inactive_cfg_r15_s::periodic_rnau_timer_r15_e_");
}
uint16_t rrc_inactive_cfg_r15_s::periodic_rnau_timer_r15_opts::to_number() const
{
  static const uint16_t options[] = {5, 10, 20, 30, 60, 120, 360, 720};
  return map_enum_number(options, 8, value, "rrc_inactive_cfg_r15_s::periodic_rnau_timer_r15_e_");
}

// RRCConnectionReconfiguration-v1310-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v1310_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scell_to_release_list_ext_r13_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_ext_r13_present, 1));
  HANDLE_CODE(bref.pack(lwa_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(lwip_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(rclwi_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (scell_to_release_list_ext_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_release_list_ext_r13, 1, 31, integer_packer<uint8_t>(1, 31)));
  }
  if (scell_to_add_mod_list_ext_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_ext_r13, 1, 31));
  }
  if (lwa_cfg_r13_present) {
    HANDLE_CODE(lwa_cfg_r13.pack(bref));
  }
  if (lwip_cfg_r13_present) {
    HANDLE_CODE(lwip_cfg_r13.pack(bref));
  }
  if (rclwi_cfg_r13_present) {
    HANDLE_CODE(rclwi_cfg_r13.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v1310_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scell_to_release_list_ext_r13_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_ext_r13_present, 1));
  HANDLE_CODE(bref.unpack(lwa_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(lwip_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(rclwi_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (scell_to_release_list_ext_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_release_list_ext_r13, bref, 1, 31, integer_packer<uint8_t>(1, 31)));
  }
  if (scell_to_add_mod_list_ext_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_ext_r13, bref, 1, 31));
  }
  if (lwa_cfg_r13_present) {
    HANDLE_CODE(lwa_cfg_r13.unpack(bref));
  }
  if (lwip_cfg_r13_present) {
    HANDLE_CODE(lwip_cfg_r13.unpack(bref));
  }
  if (rclwi_cfg_r13_present) {
    HANDLE_CODE(rclwi_cfg_r13.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_v1310_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scell_to_release_list_ext_r13_present) {
    j.start_array("sCellToReleaseListExt-r13");
    for (const auto& e1 : scell_to_release_list_ext_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (scell_to_add_mod_list_ext_r13_present) {
    j.start_array("sCellToAddModListExt-r13");
    for (const auto& e1 : scell_to_add_mod_list_ext_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (lwa_cfg_r13_present) {
    j.write_fieldname("lwa-Configuration-r13");
    lwa_cfg_r13.to_json(j);
  }
  if (lwip_cfg_r13_present) {
    j.write_fieldname("lwip-Configuration-r13");
    lwip_cfg_r13.to_json(j);
  }
  if (rclwi_cfg_r13_present) {
    j.write_fieldname("rclwi-Configuration-r13");
    rclwi_cfg_r13.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionRelease-v1540-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_release_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(wait_time_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (wait_time_present) {
    HANDLE_CODE(pack_integer(bref, wait_time, (uint8_t)1u, (uint8_t)16u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_v1540_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(wait_time_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (wait_time_present) {
    HANDLE_CODE(unpack_integer(wait_time, bref, (uint8_t)1u, (uint8_t)16u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_release_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (wait_time_present) {
    j.write_int("waitTime", wait_time);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SCG-Configuration-r12 ::= CHOICE
void scg_cfg_r12_c::set(types::options e)
{
  type_ = e;
}
void scg_cfg_r12_c::set_release()
{
  set(types::release);
}
scg_cfg_r12_c::setup_s_& scg_cfg_r12_c::set_setup()
{
  set(types::setup);
  return c;
}
void scg_cfg_r12_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.scg_cfg_part_mcg_r12_present) {
        j.write_fieldname("scg-ConfigPartMCG-r12");
        j.start_obj();
        if (c.scg_cfg_part_mcg_r12.scg_counter_r12_present) {
          j.write_int("scg-Counter-r12", c.scg_cfg_part_mcg_r12.scg_counter_r12);
        }
        if (c.scg_cfg_part_mcg_r12.pwr_coordination_info_r12_present) {
          j.write_fieldname("powerCoordinationInfo-r12");
          c.scg_cfg_part_mcg_r12.pwr_coordination_info_r12.to_json(j);
        }
        j.end_obj();
      }
      if (c.scg_cfg_part_scg_r12_present) {
        j.write_fieldname("scg-ConfigPartSCG-r12");
        c.scg_cfg_part_scg_r12.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_r12_c");
  }
  j.end_obj();
}
SRSASN_CODE scg_cfg_r12_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.scg_cfg_part_mcg_r12_present, 1));
      HANDLE_CODE(bref.pack(c.scg_cfg_part_scg_r12_present, 1));
      if (c.scg_cfg_part_mcg_r12_present) {
        bref.pack(c.scg_cfg_part_mcg_r12.ext, 1);
        HANDLE_CODE(bref.pack(c.scg_cfg_part_mcg_r12.scg_counter_r12_present, 1));
        HANDLE_CODE(bref.pack(c.scg_cfg_part_mcg_r12.pwr_coordination_info_r12_present, 1));
        if (c.scg_cfg_part_mcg_r12.scg_counter_r12_present) {
          HANDLE_CODE(pack_integer(bref, c.scg_cfg_part_mcg_r12.scg_counter_r12, (uint32_t)0u, (uint32_t)65535u));
        }
        if (c.scg_cfg_part_mcg_r12.pwr_coordination_info_r12_present) {
          HANDLE_CODE(c.scg_cfg_part_mcg_r12.pwr_coordination_info_r12.pack(bref));
        }
      }
      if (c.scg_cfg_part_scg_r12_present) {
        HANDLE_CODE(c.scg_cfg_part_scg_r12.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_r12_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_r12_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.scg_cfg_part_mcg_r12_present, 1));
      HANDLE_CODE(bref.unpack(c.scg_cfg_part_scg_r12_present, 1));
      if (c.scg_cfg_part_mcg_r12_present) {
        bref.unpack(c.scg_cfg_part_mcg_r12.ext, 1);
        HANDLE_CODE(bref.unpack(c.scg_cfg_part_mcg_r12.scg_counter_r12_present, 1));
        HANDLE_CODE(bref.unpack(c.scg_cfg_part_mcg_r12.pwr_coordination_info_r12_present, 1));
        if (c.scg_cfg_part_mcg_r12.scg_counter_r12_present) {
          HANDLE_CODE(unpack_integer(c.scg_cfg_part_mcg_r12.scg_counter_r12, bref, (uint32_t)0u, (uint32_t)65535u));
        }
        if (c.scg_cfg_part_mcg_r12.pwr_coordination_info_r12_present) {
          HANDLE_CODE(c.scg_cfg_part_mcg_r12.pwr_coordination_info_r12.unpack(bref));
        }
      }
      if (c.scg_cfg_part_scg_r12_present) {
        HANDLE_CODE(c.scg_cfg_part_scg_r12.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_r12_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-CommConfig-r12 ::= SEQUENCE
SRSASN_CODE sl_comm_cfg_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(comm_tx_res_r12_present, 1));

  if (comm_tx_res_r12_present) {
    HANDLE_CODE(comm_tx_res_r12.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= comm_tx_res_v1310.is_present();
    group_flags[0] |= comm_tx_allow_relay_ded_r13_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(comm_tx_res_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(comm_tx_allow_relay_ded_r13_present, 1));
      if (comm_tx_res_v1310.is_present()) {
        HANDLE_CODE(comm_tx_res_v1310->pack(bref));
      }
      if (comm_tx_allow_relay_ded_r13_present) {
        HANDLE_CODE(bref.pack(comm_tx_allow_relay_ded_r13, 1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_comm_cfg_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(comm_tx_res_r12_present, 1));

  if (comm_tx_res_r12_present) {
    HANDLE_CODE(comm_tx_res_r12.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool comm_tx_res_v1310_present;
      HANDLE_CODE(bref.unpack(comm_tx_res_v1310_present, 1));
      comm_tx_res_v1310.set_present(comm_tx_res_v1310_present);
      HANDLE_CODE(bref.unpack(comm_tx_allow_relay_ded_r13_present, 1));
      if (comm_tx_res_v1310.is_present()) {
        HANDLE_CODE(comm_tx_res_v1310->unpack(bref));
      }
      if (comm_tx_allow_relay_ded_r13_present) {
        HANDLE_CODE(bref.unpack(comm_tx_allow_relay_ded_r13, 1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_comm_cfg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (comm_tx_res_r12_present) {
    j.write_fieldname("commTxResources-r12");
    comm_tx_res_r12.to_json(j);
  }
  if (ext) {
    if (comm_tx_res_v1310.is_present()) {
      j.write_fieldname("commTxResources-v1310");
      comm_tx_res_v1310->to_json(j);
    }
    if (comm_tx_allow_relay_ded_r13_present) {
      j.write_bool("commTxAllowRelayDedicated-r13", comm_tx_allow_relay_ded_r13);
    }
  }
  j.end_obj();
}

void sl_comm_cfg_r12_s::comm_tx_res_r12_c_::set(types::options e)
{
  type_ = e;
}
void sl_comm_cfg_r12_s::comm_tx_res_r12_c_::set_release()
{
  set(types::release);
}
sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_& sl_comm_cfg_r12_s::comm_tx_res_r12_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_comm_cfg_r12_s::comm_tx_res_r12_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_comm_cfg_r12_s::comm_tx_res_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_comm_cfg_r12_s::comm_tx_res_r12_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::destroy_()
{
  switch (type_) {
    case types::sched_r12:
      c.destroy<sched_r12_s_>();
      break;
    case types::ue_sel_r12:
      c.destroy<ue_sel_r12_s_>();
      break;
    default:
      break;
  }
}
void sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sched_r12:
      c.init<sched_r12_s_>();
      break;
    case types::ue_sel_r12:
      c.init<ue_sel_r12_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_");
  }
}
sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::setup_c_(const sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sched_r12:
      c.init(other.c.get<sched_r12_s_>());
      break;
    case types::ue_sel_r12:
      c.init(other.c.get<ue_sel_r12_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_");
  }
}
sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_&
sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::operator=(const sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sched_r12:
      c.set(other.c.get<sched_r12_s_>());
      break;
    case types::ue_sel_r12:
      c.set(other.c.get<ue_sel_r12_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_");
  }

  return *this;
}
sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::sched_r12_s_&
sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::set_sched_r12()
{
  set(types::sched_r12);
  return c.get<sched_r12_s_>();
}
sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::ue_sel_r12_s_&
sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::set_ue_sel_r12()
{
  set(types::ue_sel_r12);
  return c.get<ue_sel_r12_s_>();
}
void sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sched_r12:
      j.write_fieldname("scheduled-r12");
      j.start_obj();
      j.write_str("sl-RNTI-r12", c.get<sched_r12_s_>().sl_rnti_r12.to_string());
      j.write_fieldname("mac-MainConfig-r12");
      c.get<sched_r12_s_>().mac_main_cfg_r12.to_json(j);
      j.write_fieldname("sc-CommTxConfig-r12");
      c.get<sched_r12_s_>().sc_comm_tx_cfg_r12.to_json(j);
      if (c.get<sched_r12_s_>().mcs_r12_present) {
        j.write_int("mcs-r12", c.get<sched_r12_s_>().mcs_r12);
      }
      j.end_obj();
      break;
    case types::ue_sel_r12:
      j.write_fieldname("ue-Selected-r12");
      j.start_obj();
      j.write_fieldname("commTxPoolNormalDedicated-r12");
      j.start_obj();
      if (c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_release_list_r12_present) {
        j.start_array("poolToReleaseList-r12");
        for (const auto& e1 : c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_release_list_r12) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_add_mod_list_r12_present) {
        j.start_array("poolToAddModList-r12");
        for (const auto& e1 : c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_add_mod_list_r12) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sched_r12:
      HANDLE_CODE(bref.pack(c.get<sched_r12_s_>().mcs_r12_present, 1));
      HANDLE_CODE(c.get<sched_r12_s_>().sl_rnti_r12.pack(bref));
      HANDLE_CODE(c.get<sched_r12_s_>().mac_main_cfg_r12.pack(bref));
      HANDLE_CODE(c.get<sched_r12_s_>().sc_comm_tx_cfg_r12.pack(bref));
      if (c.get<sched_r12_s_>().mcs_r12_present) {
        HANDLE_CODE(pack_integer(bref, c.get<sched_r12_s_>().mcs_r12, (uint8_t)0u, (uint8_t)28u));
      }
      break;
    case types::ue_sel_r12:
      HANDLE_CODE(bref.pack(c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_release_list_r12_present, 1));
      HANDLE_CODE(bref.pack(c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_add_mod_list_r12_present, 1));
      if (c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_release_list_r12_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref,
                                    c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_release_list_r12,
                                    1,
                                    4,
                                    integer_packer<uint8_t>(1, 4)));
      }
      if (c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_add_mod_list_r12_present) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_add_mod_list_r12, 1, 4));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sched_r12:
      HANDLE_CODE(bref.unpack(c.get<sched_r12_s_>().mcs_r12_present, 1));
      HANDLE_CODE(c.get<sched_r12_s_>().sl_rnti_r12.unpack(bref));
      HANDLE_CODE(c.get<sched_r12_s_>().mac_main_cfg_r12.unpack(bref));
      HANDLE_CODE(c.get<sched_r12_s_>().sc_comm_tx_cfg_r12.unpack(bref));
      if (c.get<sched_r12_s_>().mcs_r12_present) {
        HANDLE_CODE(unpack_integer(c.get<sched_r12_s_>().mcs_r12, bref, (uint8_t)0u, (uint8_t)28u));
      }
      break;
    case types::ue_sel_r12:
      HANDLE_CODE(bref.unpack(c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_release_list_r12_present, 1));
      HANDLE_CODE(bref.unpack(c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_add_mod_list_r12_present, 1));
      if (c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_release_list_r12_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_release_list_r12,
                                      bref,
                                      1,
                                      4,
                                      integer_packer<uint8_t>(1, 4)));
      }
      if (c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_add_mod_list_r12_present) {
        HANDLE_CODE(
            unpack_dyn_seq_of(c.get<ue_sel_r12_s_>().comm_tx_pool_normal_ded_r12.pool_to_add_mod_list_r12, bref, 1, 4));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::set(types::options e)
{
  type_ = e;
}
void sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::set_release()
{
  set(types::release);
}
sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_& sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_v1310_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_v1310_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_v1310_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::destroy_()
{
  switch (type_) {
    case types::sched_v1310:
      c.destroy<sched_v1310_s_>();
      break;
    case types::ue_sel_v1310:
      c.destroy<ue_sel_v1310_s_>();
      break;
    default:
      break;
  }
}
void sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sched_v1310:
      c.init<sched_v1310_s_>();
      break;
    case types::ue_sel_v1310:
      c.init<ue_sel_v1310_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_");
  }
}
sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::setup_c_(
    const sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sched_v1310:
      c.init(other.c.get<sched_v1310_s_>());
      break;
    case types::ue_sel_v1310:
      c.init(other.c.get<ue_sel_v1310_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_");
  }
}
sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_& sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::operator=(
    const sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sched_v1310:
      c.set(other.c.get<sched_v1310_s_>());
      break;
    case types::ue_sel_v1310:
      c.set(other.c.get<ue_sel_v1310_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_");
  }

  return *this;
}
sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::sched_v1310_s_&
sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::set_sched_v1310()
{
  set(types::sched_v1310);
  return c.get<sched_v1310_s_>();
}
sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::ue_sel_v1310_s_&
sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::set_ue_sel_v1310()
{
  set(types::ue_sel_v1310);
  return c.get<ue_sel_v1310_s_>();
}
void sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sched_v1310:
      j.write_fieldname("scheduled-v1310");
      j.start_obj();
      j.start_array("logicalChGroupInfoList-r13");
      for (const auto& e1 : c.get<sched_v1310_s_>().lc_ch_group_info_list_r13) {
        j.start_array();
        for (const auto& e2 : e1) {
          j.write_int(e2);
        }
        j.end_array();
      }
      j.end_array();
      j.write_bool("multipleTx-r13", c.get<sched_v1310_s_>().multiple_tx_r13);
      j.end_obj();
      break;
    case types::ue_sel_v1310:
      j.write_fieldname("ue-Selected-v1310");
      j.start_obj();
      j.write_fieldname("commTxPoolNormalDedicatedExt-r13");
      j.start_obj();
      if (c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_release_list_ext_r13_present) {
        j.start_array("poolToReleaseListExt-r13");
        for (const auto& e1 : c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_release_list_ext_r13) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_add_mod_list_ext_r13_present) {
        j.start_array("poolToAddModListExt-r13");
        for (const auto& e1 : c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_add_mod_list_ext_r13) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sched_v1310:
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  c.get<sched_v1310_s_>().lc_ch_group_info_list_r13,
                                  1,
                                  4,
                                  SeqOfPacker<integer_packer<uint8_t> >(1, 8, integer_packer<uint8_t>(1, 8))));
      HANDLE_CODE(bref.pack(c.get<sched_v1310_s_>().multiple_tx_r13, 1));
      break;
    case types::ue_sel_v1310:
      HANDLE_CODE(
          bref.pack(c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_release_list_ext_r13_present, 1));
      HANDLE_CODE(
          bref.pack(c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_add_mod_list_ext_r13_present, 1));
      if (c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_release_list_ext_r13_present) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref,
                            c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_release_list_ext_r13,
                            1,
                            4,
                            integer_packer<uint8_t>(5, 8)));
      }
      if (c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_add_mod_list_ext_r13_present) {
        HANDLE_CODE(pack_dyn_seq_of(
            bref, c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_add_mod_list_ext_r13, 1, 4));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sched_v1310:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sched_v1310_s_>().lc_ch_group_info_list_r13,
                                    bref,
                                    1,
                                    4,
                                    SeqOfPacker<integer_packer<uint8_t> >(1, 8, integer_packer<uint8_t>(1, 8))));
      HANDLE_CODE(bref.unpack(c.get<sched_v1310_s_>().multiple_tx_r13, 1));
      break;
    case types::ue_sel_v1310:
      HANDLE_CODE(bref.unpack(
          c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_release_list_ext_r13_present, 1));
      HANDLE_CODE(bref.unpack(
          c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_add_mod_list_ext_r13_present, 1));
      if (c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_release_list_ext_r13_present) {
        HANDLE_CODE(
            unpack_dyn_seq_of(c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_release_list_ext_r13,
                              bref,
                              1,
                              4,
                              integer_packer<uint8_t>(5, 8)));
      }
      if (c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_add_mod_list_ext_r13_present) {
        HANDLE_CODE(unpack_dyn_seq_of(
            c.get<ue_sel_v1310_s_>().comm_tx_pool_normal_ded_ext_r13.pool_to_add_mod_list_ext_r13, bref, 1, 4));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-DiscConfig-r12 ::= SEQUENCE
SRSASN_CODE sl_disc_cfg_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(disc_tx_res_r12_present, 1));

  if (disc_tx_res_r12_present) {
    HANDLE_CODE(disc_tx_res_r12.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= disc_tf_idx_list_v1260.is_present();
    group_flags[1] |= disc_tx_res_ps_r13.is_present();
    group_flags[1] |= disc_tx_inter_freq_info_r13.is_present();
    group_flags[1] |= gap_requests_allowed_ded_r13_present;
    group_flags[1] |= disc_rx_gap_cfg_r13.is_present();
    group_flags[1] |= disc_tx_gap_cfg_r13.is_present();
    group_flags[1] |= disc_sys_info_to_report_cfg_r13.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(disc_tf_idx_list_v1260.is_present(), 1));
      if (disc_tf_idx_list_v1260.is_present()) {
        HANDLE_CODE(disc_tf_idx_list_v1260->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(disc_tx_res_ps_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(disc_tx_inter_freq_info_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(gap_requests_allowed_ded_r13_present, 1));
      HANDLE_CODE(bref.pack(disc_rx_gap_cfg_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(disc_tx_gap_cfg_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(disc_sys_info_to_report_cfg_r13.is_present(), 1));
      if (disc_tx_res_ps_r13.is_present()) {
        HANDLE_CODE(disc_tx_res_ps_r13->pack(bref));
      }
      if (disc_tx_inter_freq_info_r13.is_present()) {
        HANDLE_CODE(disc_tx_inter_freq_info_r13->pack(bref));
      }
      if (gap_requests_allowed_ded_r13_present) {
        HANDLE_CODE(bref.pack(gap_requests_allowed_ded_r13, 1));
      }
      if (disc_rx_gap_cfg_r13.is_present()) {
        HANDLE_CODE(disc_rx_gap_cfg_r13->pack(bref));
      }
      if (disc_tx_gap_cfg_r13.is_present()) {
        HANDLE_CODE(disc_tx_gap_cfg_r13->pack(bref));
      }
      if (disc_sys_info_to_report_cfg_r13.is_present()) {
        HANDLE_CODE(disc_sys_info_to_report_cfg_r13->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(disc_tx_res_r12_present, 1));

  if (disc_tx_res_r12_present) {
    HANDLE_CODE(disc_tx_res_r12.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool disc_tf_idx_list_v1260_present;
      HANDLE_CODE(bref.unpack(disc_tf_idx_list_v1260_present, 1));
      disc_tf_idx_list_v1260.set_present(disc_tf_idx_list_v1260_present);
      if (disc_tf_idx_list_v1260.is_present()) {
        HANDLE_CODE(disc_tf_idx_list_v1260->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool disc_tx_res_ps_r13_present;
      HANDLE_CODE(bref.unpack(disc_tx_res_ps_r13_present, 1));
      disc_tx_res_ps_r13.set_present(disc_tx_res_ps_r13_present);
      bool disc_tx_inter_freq_info_r13_present;
      HANDLE_CODE(bref.unpack(disc_tx_inter_freq_info_r13_present, 1));
      disc_tx_inter_freq_info_r13.set_present(disc_tx_inter_freq_info_r13_present);
      HANDLE_CODE(bref.unpack(gap_requests_allowed_ded_r13_present, 1));
      bool disc_rx_gap_cfg_r13_present;
      HANDLE_CODE(bref.unpack(disc_rx_gap_cfg_r13_present, 1));
      disc_rx_gap_cfg_r13.set_present(disc_rx_gap_cfg_r13_present);
      bool disc_tx_gap_cfg_r13_present;
      HANDLE_CODE(bref.unpack(disc_tx_gap_cfg_r13_present, 1));
      disc_tx_gap_cfg_r13.set_present(disc_tx_gap_cfg_r13_present);
      bool disc_sys_info_to_report_cfg_r13_present;
      HANDLE_CODE(bref.unpack(disc_sys_info_to_report_cfg_r13_present, 1));
      disc_sys_info_to_report_cfg_r13.set_present(disc_sys_info_to_report_cfg_r13_present);
      if (disc_tx_res_ps_r13.is_present()) {
        HANDLE_CODE(disc_tx_res_ps_r13->unpack(bref));
      }
      if (disc_tx_inter_freq_info_r13.is_present()) {
        HANDLE_CODE(disc_tx_inter_freq_info_r13->unpack(bref));
      }
      if (gap_requests_allowed_ded_r13_present) {
        HANDLE_CODE(bref.unpack(gap_requests_allowed_ded_r13, 1));
      }
      if (disc_rx_gap_cfg_r13.is_present()) {
        HANDLE_CODE(disc_rx_gap_cfg_r13->unpack(bref));
      }
      if (disc_tx_gap_cfg_r13.is_present()) {
        HANDLE_CODE(disc_tx_gap_cfg_r13->unpack(bref));
      }
      if (disc_sys_info_to_report_cfg_r13.is_present()) {
        HANDLE_CODE(disc_sys_info_to_report_cfg_r13->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_disc_cfg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (disc_tx_res_r12_present) {
    j.write_fieldname("discTxResources-r12");
    disc_tx_res_r12.to_json(j);
  }
  if (ext) {
    if (disc_tf_idx_list_v1260.is_present()) {
      j.write_fieldname("discTF-IndexList-v1260");
      disc_tf_idx_list_v1260->to_json(j);
    }
    if (disc_tx_res_ps_r13.is_present()) {
      j.write_fieldname("discTxResourcesPS-r13");
      disc_tx_res_ps_r13->to_json(j);
    }
    if (disc_tx_inter_freq_info_r13.is_present()) {
      j.write_fieldname("discTxInterFreqInfo-r13");
      disc_tx_inter_freq_info_r13->to_json(j);
    }
    if (gap_requests_allowed_ded_r13_present) {
      j.write_bool("gapRequestsAllowedDedicated-r13", gap_requests_allowed_ded_r13);
    }
    if (disc_rx_gap_cfg_r13.is_present()) {
      j.write_fieldname("discRxGapConfig-r13");
      disc_rx_gap_cfg_r13->to_json(j);
    }
    if (disc_tx_gap_cfg_r13.is_present()) {
      j.write_fieldname("discTxGapConfig-r13");
      disc_tx_gap_cfg_r13->to_json(j);
    }
    if (disc_sys_info_to_report_cfg_r13.is_present()) {
      j.write_fieldname("discSysInfoToReportConfig-r13");
      disc_sys_info_to_report_cfg_r13->to_json(j);
    }
  }
  j.end_obj();
}

void sl_disc_cfg_r12_s::disc_tx_res_r12_c_::set(types::options e)
{
  type_ = e;
}
void sl_disc_cfg_r12_s::disc_tx_res_r12_c_::set_release()
{
  set(types::release);
}
sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_& sl_disc_cfg_r12_s::disc_tx_res_r12_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_disc_cfg_r12_s::disc_tx_res_r12_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_tx_res_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_tx_res_r12_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::destroy_()
{
  switch (type_) {
    case types::sched_r12:
      c.destroy<sched_r12_s_>();
      break;
    case types::ue_sel_r12:
      c.destroy<ue_sel_r12_s_>();
      break;
    default:
      break;
  }
}
void sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sched_r12:
      c.init<sched_r12_s_>();
      break;
    case types::ue_sel_r12:
      c.init<ue_sel_r12_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_");
  }
}
sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::setup_c_(const sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sched_r12:
      c.init(other.c.get<sched_r12_s_>());
      break;
    case types::ue_sel_r12:
      c.init(other.c.get<ue_sel_r12_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_");
  }
}
sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_&
sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::operator=(const sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sched_r12:
      c.set(other.c.get<sched_r12_s_>());
      break;
    case types::ue_sel_r12:
      c.set(other.c.get<ue_sel_r12_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_");
  }

  return *this;
}
sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::sched_r12_s_&
sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::set_sched_r12()
{
  set(types::sched_r12);
  return c.get<sched_r12_s_>();
}
sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::ue_sel_r12_s_&
sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::set_ue_sel_r12()
{
  set(types::ue_sel_r12);
  return c.get<ue_sel_r12_s_>();
}
void sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sched_r12:
      j.write_fieldname("scheduled-r12");
      j.start_obj();
      if (c.get<sched_r12_s_>().disc_tx_cfg_r12_present) {
        j.write_fieldname("discTxConfig-r12");
        c.get<sched_r12_s_>().disc_tx_cfg_r12.to_json(j);
      }
      if (c.get<sched_r12_s_>().disc_tf_idx_list_r12_present) {
        j.start_array("discTF-IndexList-r12");
        for (const auto& e1 : c.get<sched_r12_s_>().disc_tf_idx_list_r12) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (c.get<sched_r12_s_>().disc_hop_cfg_r12_present) {
        j.write_fieldname("discHoppingConfig-r12");
        c.get<sched_r12_s_>().disc_hop_cfg_r12.to_json(j);
      }
      j.end_obj();
      break;
    case types::ue_sel_r12:
      j.write_fieldname("ue-Selected-r12");
      j.start_obj();
      if (c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12_present) {
        j.write_fieldname("discTxPoolDedicated-r12");
        j.start_obj();
        if (c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_release_list_r12_present) {
          j.start_array("poolToReleaseList-r12");
          for (const auto& e1 : c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_release_list_r12) {
            j.write_int(e1);
          }
          j.end_array();
        }
        if (c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_add_mod_list_r12_present) {
          j.start_array("poolToAddModList-r12");
          for (const auto& e1 : c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_add_mod_list_r12) {
            e1.to_json(j);
          }
          j.end_array();
        }
        j.end_obj();
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sched_r12:
      HANDLE_CODE(bref.pack(c.get<sched_r12_s_>().disc_tx_cfg_r12_present, 1));
      HANDLE_CODE(bref.pack(c.get<sched_r12_s_>().disc_tf_idx_list_r12_present, 1));
      HANDLE_CODE(bref.pack(c.get<sched_r12_s_>().disc_hop_cfg_r12_present, 1));
      if (c.get<sched_r12_s_>().disc_tx_cfg_r12_present) {
        HANDLE_CODE(c.get<sched_r12_s_>().disc_tx_cfg_r12.pack(bref));
      }
      if (c.get<sched_r12_s_>().disc_tf_idx_list_r12_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sched_r12_s_>().disc_tf_idx_list_r12, 1, 64));
      }
      if (c.get<sched_r12_s_>().disc_hop_cfg_r12_present) {
        HANDLE_CODE(c.get<sched_r12_s_>().disc_hop_cfg_r12.pack(bref));
      }
      break;
    case types::ue_sel_r12:
      HANDLE_CODE(bref.pack(c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12_present, 1));
      if (c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12_present) {
        HANDLE_CODE(bref.pack(c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_release_list_r12_present, 1));
        HANDLE_CODE(bref.pack(c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_add_mod_list_r12_present, 1));
        if (c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_release_list_r12_present) {
          HANDLE_CODE(pack_dyn_seq_of(bref,
                                      c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_release_list_r12,
                                      1,
                                      4,
                                      integer_packer<uint8_t>(1, 4)));
        }
        if (c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_add_mod_list_r12_present) {
          HANDLE_CODE(
              pack_dyn_seq_of(bref, c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_add_mod_list_r12, 1, 4));
        }
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sched_r12:
      HANDLE_CODE(bref.unpack(c.get<sched_r12_s_>().disc_tx_cfg_r12_present, 1));
      HANDLE_CODE(bref.unpack(c.get<sched_r12_s_>().disc_tf_idx_list_r12_present, 1));
      HANDLE_CODE(bref.unpack(c.get<sched_r12_s_>().disc_hop_cfg_r12_present, 1));
      if (c.get<sched_r12_s_>().disc_tx_cfg_r12_present) {
        HANDLE_CODE(c.get<sched_r12_s_>().disc_tx_cfg_r12.unpack(bref));
      }
      if (c.get<sched_r12_s_>().disc_tf_idx_list_r12_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.get<sched_r12_s_>().disc_tf_idx_list_r12, bref, 1, 64));
      }
      if (c.get<sched_r12_s_>().disc_hop_cfg_r12_present) {
        HANDLE_CODE(c.get<sched_r12_s_>().disc_hop_cfg_r12.unpack(bref));
      }
      break;
    case types::ue_sel_r12:
      HANDLE_CODE(bref.unpack(c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12_present, 1));
      if (c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12_present) {
        HANDLE_CODE(bref.unpack(c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_release_list_r12_present, 1));
        HANDLE_CODE(bref.unpack(c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_add_mod_list_r12_present, 1));
        if (c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_release_list_r12_present) {
          HANDLE_CODE(unpack_dyn_seq_of(c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_release_list_r12,
                                        bref,
                                        1,
                                        4,
                                        integer_packer<uint8_t>(1, 4)));
        }
        if (c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_add_mod_list_r12_present) {
          HANDLE_CODE(
              unpack_dyn_seq_of(c.get<ue_sel_r12_s_>().disc_tx_pool_ded_r12.pool_to_add_mod_list_r12, bref, 1, 4));
        }
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_disc_cfg_r12_s::disc_tf_idx_list_v1260_c_::set(types::options e)
{
  type_ = e;
}
void sl_disc_cfg_r12_s::disc_tf_idx_list_v1260_c_::set_release()
{
  set(types::release);
}
sl_disc_cfg_r12_s::disc_tf_idx_list_v1260_c_::setup_s_& sl_disc_cfg_r12_s::disc_tf_idx_list_v1260_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_disc_cfg_r12_s::disc_tf_idx_list_v1260_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("discTF-IndexList-r12b");
      for (const auto& e1 : c.disc_tf_idx_list_r12b) {
        e1.to_json(j);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tf_idx_list_v1260_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_tf_idx_list_v1260_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.disc_tf_idx_list_r12b, 1, 64));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tf_idx_list_v1260_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_tf_idx_list_v1260_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.disc_tf_idx_list_r12b, bref, 1, 64));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tf_idx_list_v1260_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::set(types::options e)
{
  type_ = e;
}
void sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::set_release()
{
  set(types::release);
}
sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_& sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::destroy_()
{
  switch (type_) {
    case types::sched_r13:
      c.destroy<sl_disc_tx_cfg_sched_r13_s>();
      break;
    case types::ue_sel_r13:
      c.destroy<ue_sel_r13_s_>();
      break;
    default:
      break;
  }
}
void sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sched_r13:
      c.init<sl_disc_tx_cfg_sched_r13_s>();
      break;
    case types::ue_sel_r13:
      c.init<ue_sel_r13_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_");
  }
}
sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::setup_c_(
    const sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sched_r13:
      c.init(other.c.get<sl_disc_tx_cfg_sched_r13_s>());
      break;
    case types::ue_sel_r13:
      c.init(other.c.get<ue_sel_r13_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_");
  }
}
sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_& sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::operator=(
    const sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sched_r13:
      c.set(other.c.get<sl_disc_tx_cfg_sched_r13_s>());
      break;
    case types::ue_sel_r13:
      c.set(other.c.get<ue_sel_r13_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_");
  }

  return *this;
}
sl_disc_tx_cfg_sched_r13_s& sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::set_sched_r13()
{
  set(types::sched_r13);
  return c.get<sl_disc_tx_cfg_sched_r13_s>();
}
sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::ue_sel_r13_s_&
sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::set_ue_sel_r13()
{
  set(types::ue_sel_r13);
  return c.get<ue_sel_r13_s_>();
}
void sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sched_r13:
      j.write_fieldname("scheduled-r13");
      c.get<sl_disc_tx_cfg_sched_r13_s>().to_json(j);
      break;
    case types::ue_sel_r13:
      j.write_fieldname("ue-Selected-r13");
      j.start_obj();
      j.write_fieldname("discTxPoolPS-Dedicated-r13");
      c.get<ue_sel_r13_s_>().disc_tx_pool_ps_ded_r13.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sched_r13:
      HANDLE_CODE(c.get<sl_disc_tx_cfg_sched_r13_s>().pack(bref));
      break;
    case types::ue_sel_r13:
      HANDLE_CODE(c.get<ue_sel_r13_s_>().disc_tx_pool_ps_ded_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sched_r13:
      HANDLE_CODE(c.get<sl_disc_tx_cfg_sched_r13_s>().unpack(bref));
      break;
    case types::ue_sel_r13:
      HANDLE_CODE(c.get<ue_sel_r13_s_>().disc_tx_pool_ps_ded_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_disc_cfg_r12_s::disc_tx_inter_freq_info_r13_c_::set(types::options e)
{
  type_ = e;
}
void sl_disc_cfg_r12_s::disc_tx_inter_freq_info_r13_c_::set_release()
{
  set(types::release);
}
sl_disc_cfg_r12_s::disc_tx_inter_freq_info_r13_c_::setup_s_&
sl_disc_cfg_r12_s::disc_tx_inter_freq_info_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_disc_cfg_r12_s::disc_tx_inter_freq_info_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.disc_tx_carrier_freq_r13_present) {
        j.write_int("discTxCarrierFreq-r13", c.disc_tx_carrier_freq_r13);
      }
      if (c.disc_tx_ref_carrier_ded_r13_present) {
        j.write_fieldname("discTxRefCarrierDedicated-r13");
        c.disc_tx_ref_carrier_ded_r13.to_json(j);
      }
      if (c.disc_tx_info_inter_freq_list_add_r13_present) {
        j.write_fieldname("discTxInfoInterFreqListAdd-r13");
        c.disc_tx_info_inter_freq_list_add_r13.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_inter_freq_info_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_tx_inter_freq_info_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.disc_tx_carrier_freq_r13_present, 1));
      HANDLE_CODE(bref.pack(c.disc_tx_ref_carrier_ded_r13_present, 1));
      HANDLE_CODE(bref.pack(c.disc_tx_info_inter_freq_list_add_r13_present, 1));
      if (c.disc_tx_carrier_freq_r13_present) {
        HANDLE_CODE(pack_integer(bref, c.disc_tx_carrier_freq_r13, (uint32_t)0u, (uint32_t)262143u));
      }
      if (c.disc_tx_ref_carrier_ded_r13_present) {
        HANDLE_CODE(c.disc_tx_ref_carrier_ded_r13.pack(bref));
      }
      if (c.disc_tx_info_inter_freq_list_add_r13_present) {
        HANDLE_CODE(c.disc_tx_info_inter_freq_list_add_r13.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_inter_freq_info_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_tx_inter_freq_info_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.disc_tx_carrier_freq_r13_present, 1));
      HANDLE_CODE(bref.unpack(c.disc_tx_ref_carrier_ded_r13_present, 1));
      HANDLE_CODE(bref.unpack(c.disc_tx_info_inter_freq_list_add_r13_present, 1));
      if (c.disc_tx_carrier_freq_r13_present) {
        HANDLE_CODE(unpack_integer(c.disc_tx_carrier_freq_r13, bref, (uint32_t)0u, (uint32_t)262143u));
      }
      if (c.disc_tx_ref_carrier_ded_r13_present) {
        HANDLE_CODE(c.disc_tx_ref_carrier_ded_r13.unpack(bref));
      }
      if (c.disc_tx_info_inter_freq_list_add_r13_present) {
        HANDLE_CODE(c.disc_tx_info_inter_freq_list_add_r13.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_inter_freq_info_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_disc_cfg_r12_s::disc_rx_gap_cfg_r13_c_::set(types::options e)
{
  type_ = e;
}
void sl_disc_cfg_r12_s::disc_rx_gap_cfg_r13_c_::set_release()
{
  set(types::release);
}
sl_gap_cfg_r13_s& sl_disc_cfg_r12_s::disc_rx_gap_cfg_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_disc_cfg_r12_s::disc_rx_gap_cfg_r13_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_rx_gap_cfg_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_rx_gap_cfg_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_rx_gap_cfg_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_rx_gap_cfg_r13_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_rx_gap_cfg_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_disc_cfg_r12_s::disc_tx_gap_cfg_r13_c_::set(types::options e)
{
  type_ = e;
}
void sl_disc_cfg_r12_s::disc_tx_gap_cfg_r13_c_::set_release()
{
  set(types::release);
}
sl_gap_cfg_r13_s& sl_disc_cfg_r12_s::disc_tx_gap_cfg_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_disc_cfg_r12_s::disc_tx_gap_cfg_r13_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_gap_cfg_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_tx_gap_cfg_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_gap_cfg_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_tx_gap_cfg_r13_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_tx_gap_cfg_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sl_disc_cfg_r12_s::disc_sys_info_to_report_cfg_r13_c_::set(types::options e)
{
  type_ = e;
}
void sl_disc_cfg_r12_s::disc_sys_info_to_report_cfg_r13_c_::set_release()
{
  set(types::release);
}
sl_disc_sys_info_to_report_freq_list_r13_l& sl_disc_cfg_r12_s::disc_sys_info_to_report_cfg_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sl_disc_cfg_r12_s::disc_sys_info_to_report_cfg_r13_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_sys_info_to_report_cfg_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_sys_info_to_report_cfg_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 8, integer_packer<uint32_t>(0, 262143)));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_sys_info_to_report_cfg_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_r12_s::disc_sys_info_to_report_cfg_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 8, integer_packer<uint32_t>(0, 262143)));
      break;
    default:
      log_invalid_choice_id(type_, "sl_disc_cfg_r12_s::disc_sys_info_to_report_cfg_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-SyncTxControl-r12 ::= SEQUENCE
SRSASN_CODE sl_sync_tx_ctrl_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(network_ctrl_sync_tx_r12_present, 1));

  if (network_ctrl_sync_tx_r12_present) {
    HANDLE_CODE(network_ctrl_sync_tx_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_sync_tx_ctrl_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(network_ctrl_sync_tx_r12_present, 1));

  if (network_ctrl_sync_tx_r12_present) {
    HANDLE_CODE(network_ctrl_sync_tx_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_sync_tx_ctrl_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (network_ctrl_sync_tx_r12_present) {
    j.write_str("networkControlledSyncTx-r12", network_ctrl_sync_tx_r12.to_string());
  }
  j.end_obj();
}

const char* sl_sync_tx_ctrl_r12_s::network_ctrl_sync_tx_r12_opts::to_string() const
{
  static const char* options[] = {"on", "off"};
  return convert_enum_idx(options, 2, value, "sl_sync_tx_ctrl_r12_s::network_ctrl_sync_tx_r12_e_");
}

// RRCConnectionReconfiguration-v1250-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v1250_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(wlan_offload_info_r12_present, 1));
  HANDLE_CODE(bref.pack(scg_cfg_r12_present, 1));
  HANDLE_CODE(bref.pack(sl_sync_tx_ctrl_r12_present, 1));
  HANDLE_CODE(bref.pack(sl_disc_cfg_r12_present, 1));
  HANDLE_CODE(bref.pack(sl_comm_cfg_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (wlan_offload_info_r12_present) {
    HANDLE_CODE(wlan_offload_info_r12.pack(bref));
  }
  if (scg_cfg_r12_present) {
    HANDLE_CODE(scg_cfg_r12.pack(bref));
  }
  if (sl_sync_tx_ctrl_r12_present) {
    HANDLE_CODE(sl_sync_tx_ctrl_r12.pack(bref));
  }
  if (sl_disc_cfg_r12_present) {
    HANDLE_CODE(sl_disc_cfg_r12.pack(bref));
  }
  if (sl_comm_cfg_r12_present) {
    HANDLE_CODE(sl_comm_cfg_r12.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v1250_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(wlan_offload_info_r12_present, 1));
  HANDLE_CODE(bref.unpack(scg_cfg_r12_present, 1));
  HANDLE_CODE(bref.unpack(sl_sync_tx_ctrl_r12_present, 1));
  HANDLE_CODE(bref.unpack(sl_disc_cfg_r12_present, 1));
  HANDLE_CODE(bref.unpack(sl_comm_cfg_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (wlan_offload_info_r12_present) {
    HANDLE_CODE(wlan_offload_info_r12.unpack(bref));
  }
  if (scg_cfg_r12_present) {
    HANDLE_CODE(scg_cfg_r12.unpack(bref));
  }
  if (sl_sync_tx_ctrl_r12_present) {
    HANDLE_CODE(sl_sync_tx_ctrl_r12.unpack(bref));
  }
  if (sl_disc_cfg_r12_present) {
    HANDLE_CODE(sl_disc_cfg_r12.unpack(bref));
  }
  if (sl_comm_cfg_r12_present) {
    HANDLE_CODE(sl_comm_cfg_r12.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_v1250_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (wlan_offload_info_r12_present) {
    j.write_fieldname("wlan-OffloadInfo-r12");
    wlan_offload_info_r12.to_json(j);
  }
  if (scg_cfg_r12_present) {
    j.write_fieldname("scg-Configuration-r12");
    scg_cfg_r12.to_json(j);
  }
  if (sl_sync_tx_ctrl_r12_present) {
    j.write_fieldname("sl-SyncTxControl-r12");
    sl_sync_tx_ctrl_r12.to_json(j);
  }
  if (sl_disc_cfg_r12_present) {
    j.write_fieldname("sl-DiscConfig-r12");
    sl_disc_cfg_r12.to_json(j);
  }
  if (sl_comm_cfg_r12_present) {
    j.write_fieldname("sl-CommConfig-r12");
    sl_comm_cfg_r12.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

void rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::set(types::options e)
{
  type_ = e;
}
void rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::set_release()
{
  set(types::release);
}
rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::setup_s_&
rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::set_setup()
{
  set(types::setup);
  return c;
}
void rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("wlan-OffloadConfigDedicated-r12");
      c.wlan_offload_cfg_ded_r12.to_json(j);
      if (c.t350_r12_present) {
        j.write_str("t350-r12", c.t350_r12.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.t350_r12_present, 1));
      HANDLE_CODE(c.wlan_offload_cfg_ded_r12.pack(bref));
      if (c.t350_r12_present) {
        HANDLE_CODE(c.t350_r12.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.t350_r12_present, 1));
      HANDLE_CODE(c.wlan_offload_cfg_ded_r12.unpack(bref));
      if (c.t350_r12_present) {
        HANDLE_CODE(c.t350_r12.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::setup_s_::t350_r12_opts::to_string() const
{
  static const char* options[] = {"min5", "min10", "min20", "min30", "min60", "min120", "min180", "spare1"};
  return convert_enum_idx(
      options, 8, value, "rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::setup_s_::t350_r12_e_");
}
uint8_t rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::setup_s_::t350_r12_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 30, 60, 120, 180};
  return map_enum_number(
      options, 7, value, "rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::setup_s_::t350_r12_e_");
}

// RRCConnectionRelease-v1530-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_release_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(drb_continue_rohc_r15_present, 1));
  HANDLE_CODE(bref.pack(next_hop_chaining_count_r15_present, 1));
  HANDLE_CODE(bref.pack(meas_idle_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(rrc_inactive_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(cn_type_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (next_hop_chaining_count_r15_present) {
    HANDLE_CODE(pack_integer(bref, next_hop_chaining_count_r15, (uint8_t)0u, (uint8_t)7u));
  }
  if (meas_idle_cfg_r15_present) {
    HANDLE_CODE(meas_idle_cfg_r15.pack(bref));
  }
  if (rrc_inactive_cfg_r15_present) {
    HANDLE_CODE(rrc_inactive_cfg_r15.pack(bref));
  }
  if (cn_type_r15_present) {
    HANDLE_CODE(cn_type_r15.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(drb_continue_rohc_r15_present, 1));
  HANDLE_CODE(bref.unpack(next_hop_chaining_count_r15_present, 1));
  HANDLE_CODE(bref.unpack(meas_idle_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(rrc_inactive_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(cn_type_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (next_hop_chaining_count_r15_present) {
    HANDLE_CODE(unpack_integer(next_hop_chaining_count_r15, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (meas_idle_cfg_r15_present) {
    HANDLE_CODE(meas_idle_cfg_r15.unpack(bref));
  }
  if (rrc_inactive_cfg_r15_present) {
    HANDLE_CODE(rrc_inactive_cfg_r15.unpack(bref));
  }
  if (cn_type_r15_present) {
    HANDLE_CODE(cn_type_r15.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_release_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drb_continue_rohc_r15_present) {
    j.write_str("drb-ContinueROHC-r15", "true");
  }
  if (next_hop_chaining_count_r15_present) {
    j.write_int("nextHopChainingCount-r15", next_hop_chaining_count_r15);
  }
  if (meas_idle_cfg_r15_present) {
    j.write_fieldname("measIdleConfig-r15");
    meas_idle_cfg_r15.to_json(j);
  }
  if (rrc_inactive_cfg_r15_present) {
    j.write_fieldname("rrc-InactiveConfig-r15");
    rrc_inactive_cfg_r15.to_json(j);
  }
  if (cn_type_r15_present) {
    j.write_str("cn-Type-r15", cn_type_r15.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* rrc_conn_release_v1530_ies_s::cn_type_r15_opts::to_string() const
{
  static const char* options[] = {"epc", "fivegc"};
  return convert_enum_idx(options, 2, value, "rrc_conn_release_v1530_ies_s::cn_type_r15_e_");
}
uint8_t rrc_conn_release_v1530_ies_s::cn_type_r15_opts::to_number() const
{
  if (value == fivegc) {
    return 5;
  }
  invalid_enum_number(value, "rrc_conn_release_v1530_ies_s::cn_type_r15_e_");
  return 0;
}

// TargetMBSFN-Area-r12 ::= SEQUENCE
SRSASN_CODE target_mbsfn_area_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mbsfn_area_id_r12_present, 1));

  if (mbsfn_area_id_r12_present) {
    HANDLE_CODE(pack_integer(bref, mbsfn_area_id_r12, (uint16_t)0u, (uint16_t)255u));
  }
  HANDLE_CODE(pack_integer(bref, carrier_freq_r12, (uint32_t)0u, (uint32_t)262143u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE target_mbsfn_area_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(mbsfn_area_id_r12_present, 1));

  if (mbsfn_area_id_r12_present) {
    HANDLE_CODE(unpack_integer(mbsfn_area_id_r12, bref, (uint16_t)0u, (uint16_t)255u));
  }
  HANDLE_CODE(unpack_integer(carrier_freq_r12, bref, (uint32_t)0u, (uint32_t)262143u));

  return SRSASN_SUCCESS;
}
void target_mbsfn_area_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbsfn_area_id_r12_present) {
    j.write_int("mbsfn-AreaId-r12", mbsfn_area_id_r12);
  }
  j.write_int("carrierFreq-r12", carrier_freq_r12);
  j.end_obj();
}

// UEInformationRequest-v1530-IEs ::= SEQUENCE
SRSASN_CODE ue_info_request_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(idle_mode_meas_req_r15_present, 1));
  HANDLE_CODE(bref.pack(flight_path_info_req_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (flight_path_info_req_r15_present) {
    HANDLE_CODE(flight_path_info_req_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_request_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(idle_mode_meas_req_r15_present, 1));
  HANDLE_CODE(bref.unpack(flight_path_info_req_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (flight_path_info_req_r15_present) {
    HANDLE_CODE(flight_path_info_req_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_info_request_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (idle_mode_meas_req_r15_present) {
    j.write_str("idleModeMeasurementReq-r15", "true");
  }
  if (flight_path_info_req_r15_present) {
    j.write_fieldname("flightPathInfoReq-r15");
    flight_path_info_req_r15.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// CellInfoGERAN-r9 ::= SEQUENCE
SRSASN_CODE cell_info_geran_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pci_r9.pack(bref));
  HANDLE_CODE(carrier_freq_r9.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, sys_info_r9, 1, 10));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_info_geran_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(pci_r9.unpack(bref));
  HANDLE_CODE(carrier_freq_r9.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(sys_info_r9, bref, 1, 10));

  return SRSASN_SUCCESS;
}
void cell_info_geran_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("physCellId-r9");
  pci_r9.to_json(j);
  j.write_fieldname("carrierFreq-r9");
  carrier_freq_r9.to_json(j);
  j.start_array("systemInformation-r9");
  for (const auto& e1 : sys_info_r9) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

// CellInfoUTRA-FDD-r9 ::= SEQUENCE
SRSASN_CODE cell_info_utra_fdd_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pci_r9, (uint16_t)0u, (uint16_t)511u));
  HANDLE_CODE(utra_bcch_container_r9.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_info_utra_fdd_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pci_r9, bref, (uint16_t)0u, (uint16_t)511u));
  HANDLE_CODE(utra_bcch_container_r9.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_info_utra_fdd_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r9", pci_r9);
  j.write_str("utra-BCCH-Container-r9", utra_bcch_container_r9.to_string());
  j.end_obj();
}

// CellInfoUTRA-TDD-r10 ::= SEQUENCE
SRSASN_CODE cell_info_utra_tdd_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pci_r10, (uint8_t)0u, (uint8_t)127u));
  HANDLE_CODE(pack_integer(bref, carrier_freq_r10, (uint16_t)0u, (uint16_t)16383u));
  HANDLE_CODE(utra_bcch_container_r10.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_info_utra_tdd_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pci_r10, bref, (uint8_t)0u, (uint8_t)127u));
  HANDLE_CODE(unpack_integer(carrier_freq_r10, bref, (uint16_t)0u, (uint16_t)16383u));
  HANDLE_CODE(utra_bcch_container_r10.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_info_utra_tdd_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r10", pci_r10);
  j.write_int("carrierFreq-r10", carrier_freq_r10);
  j.write_str("utra-BCCH-Container-r10", utra_bcch_container_r10.to_string());
  j.end_obj();
}

// CellInfoUTRA-TDD-r9 ::= SEQUENCE
SRSASN_CODE cell_info_utra_tdd_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pci_r9, (uint8_t)0u, (uint8_t)127u));
  HANDLE_CODE(utra_bcch_container_r9.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_info_utra_tdd_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pci_r9, bref, (uint8_t)0u, (uint8_t)127u));
  HANDLE_CODE(utra_bcch_container_r9.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_info_utra_tdd_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r9", pci_r9);
  j.write_str("utra-BCCH-Container-r9", utra_bcch_container_r9.to_string());
  j.end_obj();
}

// DRB-CountMSB-Info ::= SEQUENCE
SRSASN_CODE drb_count_msb_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, count_msb_ul, (uint32_t)0u, (uint32_t)33554431u));
  HANDLE_CODE(pack_integer(bref, count_msb_dl, (uint32_t)0u, (uint32_t)33554431u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_count_msb_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(count_msb_ul, bref, (uint32_t)0u, (uint32_t)33554431u));
  HANDLE_CODE(unpack_integer(count_msb_dl, bref, (uint32_t)0u, (uint32_t)33554431u));

  return SRSASN_SUCCESS;
}
void drb_count_msb_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("drb-Identity", drb_id);
  j.write_int("countMSB-Uplink", count_msb_ul);
  j.write_int("countMSB-Downlink", count_msb_dl);
  j.end_obj();
}

// IDC-Config-r11 ::= SEQUENCE
SRSASN_CODE idc_cfg_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(idc_ind_r11_present, 1));
  HANDLE_CODE(bref.pack(autonomous_denial_params_r11_present, 1));

  if (autonomous_denial_params_r11_present) {
    HANDLE_CODE(autonomous_denial_params_r11.autonomous_denial_sfs_r11.pack(bref));
    HANDLE_CODE(autonomous_denial_params_r11.autonomous_denial_validity_r11.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= idc_ind_ul_ca_r11_present;
    group_flags[1] |= idc_hardware_sharing_ind_r13_present;
    group_flags[2] |= idc_ind_mrdc_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(idc_ind_ul_ca_r11_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(idc_hardware_sharing_ind_r13_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(idc_ind_mrdc_r15.is_present(), 1));
      if (idc_ind_mrdc_r15.is_present()) {
        HANDLE_CODE(idc_ind_mrdc_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE idc_cfg_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(idc_ind_r11_present, 1));
  HANDLE_CODE(bref.unpack(autonomous_denial_params_r11_present, 1));

  if (autonomous_denial_params_r11_present) {
    HANDLE_CODE(autonomous_denial_params_r11.autonomous_denial_sfs_r11.unpack(bref));
    HANDLE_CODE(autonomous_denial_params_r11.autonomous_denial_validity_r11.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(idc_ind_ul_ca_r11_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(idc_hardware_sharing_ind_r13_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool idc_ind_mrdc_r15_present;
      HANDLE_CODE(bref.unpack(idc_ind_mrdc_r15_present, 1));
      idc_ind_mrdc_r15.set_present(idc_ind_mrdc_r15_present);
      if (idc_ind_mrdc_r15.is_present()) {
        HANDLE_CODE(idc_ind_mrdc_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void idc_cfg_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (idc_ind_r11_present) {
    j.write_str("idc-Indication-r11", "setup");
  }
  if (autonomous_denial_params_r11_present) {
    j.write_fieldname("autonomousDenialParameters-r11");
    j.start_obj();
    j.write_str("autonomousDenialSubframes-r11", autonomous_denial_params_r11.autonomous_denial_sfs_r11.to_string());
    j.write_str("autonomousDenialValidity-r11",
                autonomous_denial_params_r11.autonomous_denial_validity_r11.to_string());
    j.end_obj();
  }
  if (ext) {
    if (idc_ind_ul_ca_r11_present) {
      j.write_str("idc-Indication-UL-CA-r11", "setup");
    }
    if (idc_hardware_sharing_ind_r13_present) {
      j.write_str("idc-HardwareSharingIndication-r13", "setup");
    }
    if (idc_ind_mrdc_r15.is_present()) {
      j.write_fieldname("idc-Indication-MRDC-r15");
      idc_ind_mrdc_r15->to_json(j);
    }
  }
  j.end_obj();
}

const char* idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_sfs_r11_opts::to_string() const
{
  static const char* options[] = {"n2", "n5", "n10", "n15", "n20", "n30", "spare2", "spare1"};
  return convert_enum_idx(
      options, 8, value, "idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_sfs_r11_e_");
}
uint8_t idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_sfs_r11_opts::to_number() const
{
  static const uint8_t options[] = {2, 5, 10, 15, 20, 30};
  return map_enum_number(
      options, 6, value, "idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_sfs_r11_e_");
}

const char* idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_validity_r11_opts::to_string() const
{
  static const char* options[] = {"sf200", "sf500", "sf1000", "sf2000", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(
      options, 8, value, "idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_validity_r11_e_");
}
uint16_t idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_validity_r11_opts::to_number() const
{
  static const uint16_t options[] = {200, 500, 1000, 2000};
  return map_enum_number(
      options, 4, value, "idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_validity_r11_e_");
}

void idc_cfg_r11_s::idc_ind_mrdc_r15_c_::set(types::options e)
{
  type_ = e;
}
void idc_cfg_r11_s::idc_ind_mrdc_r15_c_::set_release()
{
  set(types::release);
}
candidate_serving_freq_list_nr_r15_l& idc_cfg_r11_s::idc_ind_mrdc_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void idc_cfg_r11_s::idc_ind_mrdc_r15_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "idc_cfg_r11_s::idc_ind_mrdc_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE idc_cfg_r11_s::idc_ind_mrdc_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 32, integer_packer<uint32_t>(0, 3279165)));
      break;
    default:
      log_invalid_choice_id(type_, "idc_cfg_r11_s::idc_ind_mrdc_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE idc_cfg_r11_s::idc_ind_mrdc_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 32, integer_packer<uint32_t>(0, 3279165)));
      break;
    default:
      log_invalid_choice_id(type_, "idc_cfg_r11_s::idc_ind_mrdc_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ObtainLocationConfig-r11 ::= SEQUENCE
SRSASN_CODE obtain_location_cfg_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(obtain_location_r11_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE obtain_location_cfg_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(obtain_location_r11_present, 1));

  return SRSASN_SUCCESS;
}
void obtain_location_cfg_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (obtain_location_r11_present) {
    j.write_str("obtainLocation-r11", "setup");
  }
  j.end_obj();
}

// PowerPrefIndicationConfig-r11 ::= CHOICE
void pwr_pref_ind_cfg_r11_c::set(types::options e)
{
  type_ = e;
}
void pwr_pref_ind_cfg_r11_c::set_release()
{
  set(types::release);
}
pwr_pref_ind_cfg_r11_c::setup_s_& pwr_pref_ind_cfg_r11_c::set_setup()
{
  set(types::setup);
  return c;
}
void pwr_pref_ind_cfg_r11_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("powerPrefIndicationTimer-r11", c.pwr_pref_ind_timer_r11.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pwr_pref_ind_cfg_r11_c");
  }
  j.end_obj();
}
SRSASN_CODE pwr_pref_ind_cfg_r11_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pwr_pref_ind_timer_r11.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pwr_pref_ind_cfg_r11_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pwr_pref_ind_cfg_r11_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pwr_pref_ind_timer_r11.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pwr_pref_ind_cfg_r11_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_opts::to_string() const
{
  static const char* options[] = {"s0",
                                  "s0dot5",
                                  "s1",
                                  "s2",
                                  "s5",
                                  "s10",
                                  "s20",
                                  "s30",
                                  "s60",
                                  "s90",
                                  "s120",
                                  "s300",
                                  "s600",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_e_");
}
float pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0};
  return map_enum_number(options, 13, value, "pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_e_");
}
const char* pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.5", "1", "2", "5", "10", "20", "30", "60", "90", "120", "300", "600"};
  return convert_enum_idx(options, 16, value, "pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_e_");
}

// RRCConnectionReconfiguration-v1130-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v1130_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sib_type1_ded_r11_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sib_type1_ded_r11_present) {
    HANDLE_CODE(sib_type1_ded_r11.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v1130_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sib_type1_ded_r11_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sib_type1_ded_r11_present) {
    HANDLE_CODE(sib_type1_ded_r11.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_v1130_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sib_type1_ded_r11_present) {
    j.write_str("systemInformationBlockType1Dedicated-r11", sib_type1_ded_r11.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionRelease-v1320-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_release_v1320_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(resume_id_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (resume_id_r13_present) {
    HANDLE_CODE(resume_id_r13.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_v1320_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(resume_id_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (resume_id_r13_present) {
    HANDLE_CODE(resume_id_r13.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_release_v1320_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (resume_id_r13_present) {
    j.write_str("resumeIdentity-r13", resume_id_r13.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// ReportProximityConfig-r9 ::= SEQUENCE
SRSASN_CODE report_proximity_cfg_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(proximity_ind_eutra_r9_present, 1));
  HANDLE_CODE(bref.pack(proximity_ind_utra_r9_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE report_proximity_cfg_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(proximity_ind_eutra_r9_present, 1));
  HANDLE_CODE(bref.unpack(proximity_ind_utra_r9_present, 1));

  return SRSASN_SUCCESS;
}
void report_proximity_cfg_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (proximity_ind_eutra_r9_present) {
    j.write_str("proximityIndicationEUTRA-r9", "enabled");
  }
  if (proximity_ind_utra_r9_present) {
    j.write_str("proximityIndicationUTRA-r9", "enabled");
  }
  j.end_obj();
}

// TrackingAreaCodeList-v1130 ::= SEQUENCE
SRSASN_CODE tac_list_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_per_tac_list_r11, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tac_list_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_per_tac_list_r11, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void tac_list_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("plmn-Identity-perTAC-List-r11");
  for (const auto& e1 : plmn_id_per_tac_list_r11) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// UEInformationRequest-v1250-IEs ::= SEQUENCE
SRSASN_CODE ue_info_request_v1250_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mob_history_report_req_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_request_v1250_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mob_history_report_req_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_info_request_v1250_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mob_history_report_req_r12_present) {
    j.write_str("mobilityHistoryReportReq-r12", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// AreaConfiguration-v1130 ::= SEQUENCE
SRSASN_CODE area_cfg_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(tac_list_v1130.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE area_cfg_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(tac_list_v1130.unpack(bref));

  return SRSASN_SUCCESS;
}
void area_cfg_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("trackingAreaCodeList-v1130");
  tac_list_v1130.to_json(j);
  j.end_obj();
}

// HandoverFromEUTRAPreparationRequest-v1020-IEs ::= SEQUENCE
SRSASN_CODE ho_from_eutra_prep_request_v1020_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dual_rx_tx_redirect_ind_r10_present, 1));
  HANDLE_CODE(bref.pack(redirect_carrier_cdma2000_minus1_xrtt_r10_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (redirect_carrier_cdma2000_minus1_xrtt_r10_present) {
    HANDLE_CODE(redirect_carrier_cdma2000_minus1_xrtt_r10.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_from_eutra_prep_request_v1020_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dual_rx_tx_redirect_ind_r10_present, 1));
  HANDLE_CODE(bref.unpack(redirect_carrier_cdma2000_minus1_xrtt_r10_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (redirect_carrier_cdma2000_minus1_xrtt_r10_present) {
    HANDLE_CODE(redirect_carrier_cdma2000_minus1_xrtt_r10.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_from_eutra_prep_request_v1020_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dual_rx_tx_redirect_ind_r10_present) {
    j.write_str("dualRxTxRedirectIndicator-r10", "true");
  }
  if (redirect_carrier_cdma2000_minus1_xrtt_r10_present) {
    j.write_fieldname("redirectCarrierCDMA2000-1XRTT-r10");
    redirect_carrier_cdma2000_minus1_xrtt_r10.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// LoggedMeasurementConfiguration-v1250-IEs ::= SEQUENCE
SRSASN_CODE logged_meas_cfg_v1250_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(target_mbsfn_area_list_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (target_mbsfn_area_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, target_mbsfn_area_list_r12, 0, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_meas_cfg_v1250_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(target_mbsfn_area_list_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (target_mbsfn_area_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(target_mbsfn_area_list_r12, bref, 0, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void logged_meas_cfg_v1250_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (target_mbsfn_area_list_r12_present) {
    j.start_array("targetMBSFN-AreaList-r12");
    for (const auto& e1 : target_mbsfn_area_list_r12) {
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

// MobilityFromEUTRACommand-v1530-IEs ::= SEQUENCE
SRSASN_CODE mob_from_eutra_cmd_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(smtc_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (smtc_r15_present) {
    HANDLE_CODE(smtc_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_from_eutra_cmd_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(smtc_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (smtc_r15_present) {
    HANDLE_CODE(smtc_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mob_from_eutra_cmd_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (smtc_r15_present) {
    j.write_fieldname("smtc-r15");
    smtc_r15.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// OtherConfig-r9 ::= SEQUENCE
SRSASN_CODE other_cfg_r9_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(report_proximity_cfg_r9_present, 1));

  if (report_proximity_cfg_r9_present) {
    HANDLE_CODE(report_proximity_cfg_r9.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= idc_cfg_r11.is_present();
    group_flags[0] |= pwr_pref_ind_cfg_r11.is_present();
    group_flags[0] |= obtain_location_cfg_r11.is_present();
    group_flags[1] |= bw_pref_ind_timer_r14_present;
    group_flags[1] |= sps_assist_info_report_r14_present;
    group_flags[1] |= delay_budget_report_cfg_r14.is_present();
    group_flags[1] |= rlm_report_cfg_r14.is_present();
    group_flags[2] |= overheat_assist_cfg_r14.is_present();
    group_flags[3] |= meas_cfg_app_layer_r15.is_present();
    group_flags[3] |= ailc_bit_cfg_r15_present;
    group_flags[3] |= bt_name_list_cfg_r15.is_present();
    group_flags[3] |= wlan_name_list_cfg_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(idc_cfg_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(pwr_pref_ind_cfg_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(obtain_location_cfg_r11.is_present(), 1));
      if (idc_cfg_r11.is_present()) {
        HANDLE_CODE(idc_cfg_r11->pack(bref));
      }
      if (pwr_pref_ind_cfg_r11.is_present()) {
        HANDLE_CODE(pwr_pref_ind_cfg_r11->pack(bref));
      }
      if (obtain_location_cfg_r11.is_present()) {
        HANDLE_CODE(obtain_location_cfg_r11->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(bw_pref_ind_timer_r14_present, 1));
      HANDLE_CODE(bref.pack(sps_assist_info_report_r14_present, 1));
      HANDLE_CODE(bref.pack(delay_budget_report_cfg_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(rlm_report_cfg_r14.is_present(), 1));
      if (bw_pref_ind_timer_r14_present) {
        HANDLE_CODE(bw_pref_ind_timer_r14.pack(bref));
      }
      if (sps_assist_info_report_r14_present) {
        HANDLE_CODE(bref.pack(sps_assist_info_report_r14, 1));
      }
      if (delay_budget_report_cfg_r14.is_present()) {
        HANDLE_CODE(delay_budget_report_cfg_r14->pack(bref));
      }
      if (rlm_report_cfg_r14.is_present()) {
        HANDLE_CODE(rlm_report_cfg_r14->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(overheat_assist_cfg_r14.is_present(), 1));
      if (overheat_assist_cfg_r14.is_present()) {
        HANDLE_CODE(overheat_assist_cfg_r14->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_cfg_app_layer_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(ailc_bit_cfg_r15_present, 1));
      HANDLE_CODE(bref.pack(bt_name_list_cfg_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(wlan_name_list_cfg_r15.is_present(), 1));
      if (meas_cfg_app_layer_r15.is_present()) {
        HANDLE_CODE(meas_cfg_app_layer_r15->pack(bref));
      }
      if (ailc_bit_cfg_r15_present) {
        HANDLE_CODE(bref.pack(ailc_bit_cfg_r15, 1));
      }
      if (bt_name_list_cfg_r15.is_present()) {
        HANDLE_CODE(bt_name_list_cfg_r15->pack(bref));
      }
      if (wlan_name_list_cfg_r15.is_present()) {
        HANDLE_CODE(wlan_name_list_cfg_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE other_cfg_r9_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(report_proximity_cfg_r9_present, 1));

  if (report_proximity_cfg_r9_present) {
    HANDLE_CODE(report_proximity_cfg_r9.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool idc_cfg_r11_present;
      HANDLE_CODE(bref.unpack(idc_cfg_r11_present, 1));
      idc_cfg_r11.set_present(idc_cfg_r11_present);
      bool pwr_pref_ind_cfg_r11_present;
      HANDLE_CODE(bref.unpack(pwr_pref_ind_cfg_r11_present, 1));
      pwr_pref_ind_cfg_r11.set_present(pwr_pref_ind_cfg_r11_present);
      bool obtain_location_cfg_r11_present;
      HANDLE_CODE(bref.unpack(obtain_location_cfg_r11_present, 1));
      obtain_location_cfg_r11.set_present(obtain_location_cfg_r11_present);
      if (idc_cfg_r11.is_present()) {
        HANDLE_CODE(idc_cfg_r11->unpack(bref));
      }
      if (pwr_pref_ind_cfg_r11.is_present()) {
        HANDLE_CODE(pwr_pref_ind_cfg_r11->unpack(bref));
      }
      if (obtain_location_cfg_r11.is_present()) {
        HANDLE_CODE(obtain_location_cfg_r11->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(bw_pref_ind_timer_r14_present, 1));
      HANDLE_CODE(bref.unpack(sps_assist_info_report_r14_present, 1));
      bool delay_budget_report_cfg_r14_present;
      HANDLE_CODE(bref.unpack(delay_budget_report_cfg_r14_present, 1));
      delay_budget_report_cfg_r14.set_present(delay_budget_report_cfg_r14_present);
      bool rlm_report_cfg_r14_present;
      HANDLE_CODE(bref.unpack(rlm_report_cfg_r14_present, 1));
      rlm_report_cfg_r14.set_present(rlm_report_cfg_r14_present);
      if (bw_pref_ind_timer_r14_present) {
        HANDLE_CODE(bw_pref_ind_timer_r14.unpack(bref));
      }
      if (sps_assist_info_report_r14_present) {
        HANDLE_CODE(bref.unpack(sps_assist_info_report_r14, 1));
      }
      if (delay_budget_report_cfg_r14.is_present()) {
        HANDLE_CODE(delay_budget_report_cfg_r14->unpack(bref));
      }
      if (rlm_report_cfg_r14.is_present()) {
        HANDLE_CODE(rlm_report_cfg_r14->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool overheat_assist_cfg_r14_present;
      HANDLE_CODE(bref.unpack(overheat_assist_cfg_r14_present, 1));
      overheat_assist_cfg_r14.set_present(overheat_assist_cfg_r14_present);
      if (overheat_assist_cfg_r14.is_present()) {
        HANDLE_CODE(overheat_assist_cfg_r14->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_cfg_app_layer_r15_present;
      HANDLE_CODE(bref.unpack(meas_cfg_app_layer_r15_present, 1));
      meas_cfg_app_layer_r15.set_present(meas_cfg_app_layer_r15_present);
      HANDLE_CODE(bref.unpack(ailc_bit_cfg_r15_present, 1));
      bool bt_name_list_cfg_r15_present;
      HANDLE_CODE(bref.unpack(bt_name_list_cfg_r15_present, 1));
      bt_name_list_cfg_r15.set_present(bt_name_list_cfg_r15_present);
      bool wlan_name_list_cfg_r15_present;
      HANDLE_CODE(bref.unpack(wlan_name_list_cfg_r15_present, 1));
      wlan_name_list_cfg_r15.set_present(wlan_name_list_cfg_r15_present);
      if (meas_cfg_app_layer_r15.is_present()) {
        HANDLE_CODE(meas_cfg_app_layer_r15->unpack(bref));
      }
      if (ailc_bit_cfg_r15_present) {
        HANDLE_CODE(bref.unpack(ailc_bit_cfg_r15, 1));
      }
      if (bt_name_list_cfg_r15.is_present()) {
        HANDLE_CODE(bt_name_list_cfg_r15->unpack(bref));
      }
      if (wlan_name_list_cfg_r15.is_present()) {
        HANDLE_CODE(wlan_name_list_cfg_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void other_cfg_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (report_proximity_cfg_r9_present) {
    j.write_fieldname("reportProximityConfig-r9");
    report_proximity_cfg_r9.to_json(j);
  }
  if (ext) {
    if (idc_cfg_r11.is_present()) {
      j.write_fieldname("idc-Config-r11");
      idc_cfg_r11->to_json(j);
    }
    if (pwr_pref_ind_cfg_r11.is_present()) {
      j.write_fieldname("powerPrefIndicationConfig-r11");
      pwr_pref_ind_cfg_r11->to_json(j);
    }
    if (obtain_location_cfg_r11.is_present()) {
      j.write_fieldname("obtainLocationConfig-r11");
      obtain_location_cfg_r11->to_json(j);
    }
    if (bw_pref_ind_timer_r14_present) {
      j.write_str("bw-PreferenceIndicationTimer-r14", bw_pref_ind_timer_r14.to_string());
    }
    if (sps_assist_info_report_r14_present) {
      j.write_bool("sps-AssistanceInfoReport-r14", sps_assist_info_report_r14);
    }
    if (delay_budget_report_cfg_r14.is_present()) {
      j.write_fieldname("delayBudgetReportingConfig-r14");
      delay_budget_report_cfg_r14->to_json(j);
    }
    if (rlm_report_cfg_r14.is_present()) {
      j.write_fieldname("rlm-ReportConfig-r14");
      rlm_report_cfg_r14->to_json(j);
    }
    if (overheat_assist_cfg_r14.is_present()) {
      j.write_fieldname("overheatingAssistanceConfig-r14");
      overheat_assist_cfg_r14->to_json(j);
    }
    if (meas_cfg_app_layer_r15.is_present()) {
      j.write_fieldname("measConfigAppLayer-r15");
      meas_cfg_app_layer_r15->to_json(j);
    }
    if (ailc_bit_cfg_r15_present) {
      j.write_bool("ailc-BitConfig-r15", ailc_bit_cfg_r15);
    }
    if (bt_name_list_cfg_r15.is_present()) {
      j.write_fieldname("bt-NameListConfig-r15");
      bt_name_list_cfg_r15->to_json(j);
    }
    if (wlan_name_list_cfg_r15.is_present()) {
      j.write_fieldname("wlan-NameListConfig-r15");
      wlan_name_list_cfg_r15->to_json(j);
    }
  }
  j.end_obj();
}

const char* other_cfg_r9_s::bw_pref_ind_timer_r14_opts::to_string() const
{
  static const char* options[] = {"s0",
                                  "s0dot5",
                                  "s1",
                                  "s2",
                                  "s5",
                                  "s10",
                                  "s20",
                                  "s30",
                                  "s60",
                                  "s90",
                                  "s120",
                                  "s300",
                                  "s600",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "other_cfg_r9_s::bw_pref_ind_timer_r14_e_");
}
float other_cfg_r9_s::bw_pref_ind_timer_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0};
  return map_enum_number(options, 13, value, "other_cfg_r9_s::bw_pref_ind_timer_r14_e_");
}
const char* other_cfg_r9_s::bw_pref_ind_timer_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.5", "1", "2", "5", "10", "20", "30", "60", "90", "120", "300", "600"};
  return convert_enum_idx(options, 16, value, "other_cfg_r9_s::bw_pref_ind_timer_r14_e_");
}

void other_cfg_r9_s::delay_budget_report_cfg_r14_c_::set(types::options e)
{
  type_ = e;
}
void other_cfg_r9_s::delay_budget_report_cfg_r14_c_::set_release()
{
  set(types::release);
}
other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_& other_cfg_r9_s::delay_budget_report_cfg_r14_c_::set_setup()
{
  set(types::setup);
  return c;
}
void other_cfg_r9_s::delay_budget_report_cfg_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("delayBudgetReportingProhibitTimer-r14", c.delay_budget_report_prohibit_timer_r14.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_r9_s::delay_budget_report_cfg_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE other_cfg_r9_s::delay_budget_report_cfg_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.delay_budget_report_prohibit_timer_r14.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_r9_s::delay_budget_report_cfg_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE other_cfg_r9_s::delay_budget_report_cfg_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.delay_budget_report_prohibit_timer_r14.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_r9_s::delay_budget_report_cfg_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char*
other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_opts::to_string() const
{
  static const char* options[] = {"s0", "s0dot4", "s0dot8", "s1dot6", "s3", "s6", "s12", "s30"};
  return convert_enum_idx(
      options,
      8,
      value,
      "other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_e_");
}
float other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_opts::to_number()
    const
{
  static const float options[] = {0.0, 0.4, 0.8, 1.6, 3.0, 6.0, 12.0, 30.0};
  return map_enum_number(
      options,
      8,
      value,
      "other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_e_");
}
const char* other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_opts::
    to_number_string() const
{
  static const char* options[] = {"0", "0.4", "0.8", "1.6", "3", "6", "12", "30"};
  return convert_enum_idx(
      options,
      8,
      value,
      "other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_e_");
}

void other_cfg_r9_s::rlm_report_cfg_r14_c_::set(types::options e)
{
  type_ = e;
}
void other_cfg_r9_s::rlm_report_cfg_r14_c_::set_release()
{
  set(types::release);
}
other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_& other_cfg_r9_s::rlm_report_cfg_r14_c_::set_setup()
{
  set(types::setup);
  return c;
}
void other_cfg_r9_s::rlm_report_cfg_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("rlmReportTimer-r14", c.rlm_report_timer_r14.to_string());
      if (c.rlm_report_rep_mpdcch_r14_present) {
        j.write_str("rlmReportRep-MPDCCH-r14", "setup");
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_r9_s::rlm_report_cfg_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE other_cfg_r9_s::rlm_report_cfg_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.rlm_report_rep_mpdcch_r14_present, 1));
      HANDLE_CODE(c.rlm_report_timer_r14.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_r9_s::rlm_report_cfg_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE other_cfg_r9_s::rlm_report_cfg_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.rlm_report_rep_mpdcch_r14_present, 1));
      HANDLE_CODE(c.rlm_report_timer_r14.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_r9_s::rlm_report_cfg_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_opts::to_string() const
{
  static const char* options[] = {"s0",
                                  "s0dot5",
                                  "s1",
                                  "s2",
                                  "s5",
                                  "s10",
                                  "s20",
                                  "s30",
                                  "s60",
                                  "s90",
                                  "s120",
                                  "s300",
                                  "s600",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(
      options, 16, value, "other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_e_");
}
float other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0};
  return map_enum_number(
      options, 13, value, "other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_e_");
}
const char* other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.5", "1", "2", "5", "10", "20", "30", "60", "90", "120", "300", "600"};
  return convert_enum_idx(
      options, 16, value, "other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_e_");
}

void other_cfg_r9_s::overheat_assist_cfg_r14_c_::set(types::options e)
{
  type_ = e;
}
void other_cfg_r9_s::overheat_assist_cfg_r14_c_::set_release()
{
  set(types::release);
}
other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_& other_cfg_r9_s::overheat_assist_cfg_r14_c_::set_setup()
{
  set(types::setup);
  return c;
}
void other_cfg_r9_s::overheat_assist_cfg_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("overheatingIndicationProhibitTimer-r14", c.overheat_ind_prohibit_timer_r14.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_r9_s::overheat_assist_cfg_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE other_cfg_r9_s::overheat_assist_cfg_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.overheat_ind_prohibit_timer_r14.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_r9_s::overheat_assist_cfg_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE other_cfg_r9_s::overheat_assist_cfg_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.overheat_ind_prohibit_timer_r14.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_r9_s::overheat_assist_cfg_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char*
other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_opts::to_string() const
{
  static const char* options[] = {"s0",
                                  "s0dot5",
                                  "s1",
                                  "s2",
                                  "s5",
                                  "s10",
                                  "s20",
                                  "s30",
                                  "s60",
                                  "s90",
                                  "s120",
                                  "s300",
                                  "s600",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(
      options, 16, value, "other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_e_");
}
float other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0};
  return map_enum_number(
      options, 13, value, "other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_e_");
}
const char*
other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.5", "1", "2", "5", "10", "20", "30", "60", "90", "120", "300", "600"};
  return convert_enum_idx(
      options, 16, value, "other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_e_");
}

void other_cfg_r9_s::meas_cfg_app_layer_r15_c_::set(types::options e)
{
  type_ = e;
}
void other_cfg_r9_s::meas_cfg_app_layer_r15_c_::set_release()
{
  set(types::release);
}
other_cfg_r9_s::meas_cfg_app_layer_r15_c_::setup_s_& other_cfg_r9_s::meas_cfg_app_layer_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void other_cfg_r9_s::meas_cfg_app_layer_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("measConfigAppLayerContainer-r15", c.meas_cfg_app_layer_container_r15.to_string());
      j.write_str("serviceType-r15", c.service_type_r15.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_r9_s::meas_cfg_app_layer_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE other_cfg_r9_s::meas_cfg_app_layer_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.meas_cfg_app_layer_container_r15.pack(bref));
      HANDLE_CODE(c.service_type_r15.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_r9_s::meas_cfg_app_layer_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE other_cfg_r9_s::meas_cfg_app_layer_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.meas_cfg_app_layer_container_r15.unpack(bref));
      HANDLE_CODE(c.service_type_r15.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_r9_s::meas_cfg_app_layer_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* other_cfg_r9_s::meas_cfg_app_layer_r15_c_::setup_s_::service_type_r15_opts::to_string() const
{
  static const char* options[] = {"qoe", "qoemtsi", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(
      options, 8, value, "other_cfg_r9_s::meas_cfg_app_layer_r15_c_::setup_s_::service_type_r15_e_");
}

// RRCConnectionReconfiguration-v1020-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v1020_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scell_to_release_list_r10_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_r10_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (scell_to_release_list_r10_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_release_list_r10, 1, 4, integer_packer<uint8_t>(1, 7)));
  }
  if (scell_to_add_mod_list_r10_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_r10, 1, 4));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v1020_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scell_to_release_list_r10_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_r10_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (scell_to_release_list_r10_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_release_list_r10, bref, 1, 4, integer_packer<uint8_t>(1, 7)));
  }
  if (scell_to_add_mod_list_r10_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_r10, bref, 1, 4));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_v1020_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scell_to_release_list_r10_present) {
    j.start_array("sCellToReleaseList-r10");
    for (const auto& e1 : scell_to_release_list_r10) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (scell_to_add_mod_list_r10_present) {
    j.start_array("sCellToAddModList-r10");
    for (const auto& e1 : scell_to_add_mod_list_r10) {
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

// RRCConnectionRelease-v1020-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_release_v1020_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(extended_wait_time_r10_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (extended_wait_time_r10_present) {
    HANDLE_CODE(pack_integer(bref, extended_wait_time_r10, (uint16_t)1u, (uint16_t)1800u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_v1020_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(extended_wait_time_r10_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (extended_wait_time_r10_present) {
    HANDLE_CODE(unpack_integer(extended_wait_time_r10, bref, (uint16_t)1u, (uint16_t)1800u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_release_v1020_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (extended_wait_time_r10_present) {
    j.write_int("extendedWaitTime-r10", extended_wait_time_r10);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionResume-v1530-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(full_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(full_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (full_cfg_r15_present) {
    j.write_str("fullConfig-r15", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UEInformationRequest-v1130-IEs ::= SEQUENCE
SRSASN_CODE ue_info_request_v1130_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(conn_est_fail_report_req_r11_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_request_v1130_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(conn_est_fail_report_req_r11_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_info_request_v1130_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (conn_est_fail_report_req_r11_present) {
    j.write_str("connEstFailReportReq-r11", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CarrierInfoNR-r15 ::= SEQUENCE
SRSASN_CODE carrier_info_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(smtc_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r15, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(subcarrier_spacing_ssb_r15.pack(bref));
  if (smtc_r15_present) {
    HANDLE_CODE(smtc_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_info_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(smtc_r15_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r15, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(subcarrier_spacing_ssb_r15.unpack(bref));
  if (smtc_r15_present) {
    HANDLE_CODE(smtc_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void carrier_info_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r15", carrier_freq_r15);
  j.write_str("subcarrierSpacingSSB-r15", subcarrier_spacing_ssb_r15.to_string());
  if (smtc_r15_present) {
    j.write_fieldname("smtc-r15");
    smtc_r15.to_json(j);
  }
  j.end_obj();
}

const char* carrier_info_nr_r15_s::subcarrier_spacing_ssb_r15_opts::to_string() const
{
  static const char* options[] = {"kHz15", "kHz30", "kHz120", "kHz240"};
  return convert_enum_idx(options, 4, value, "carrier_info_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}
uint8_t carrier_info_nr_r15_s::subcarrier_spacing_ssb_r15_opts::to_number() const
{
  static const uint8_t options[] = {15, 30, 120, 240};
  return map_enum_number(options, 4, value, "carrier_info_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}

// CounterCheck-v1530-IEs ::= SEQUENCE
SRSASN_CODE counter_check_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(drb_count_msb_info_list_ext_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (drb_count_msb_info_list_ext_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_count_msb_info_list_ext_r15, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(drb_count_msb_info_list_ext_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (drb_count_msb_info_list_ext_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_count_msb_info_list_ext_r15, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void counter_check_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drb_count_msb_info_list_ext_r15_present) {
    j.start_array("drb-CountMSB-InfoListExt-r15");
    for (const auto& e1 : drb_count_msb_info_list_ext_r15) {
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

// HandoverFromEUTRAPreparationRequest-v920-IEs ::= SEQUENCE
SRSASN_CODE ho_from_eutra_prep_request_v920_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(concurr_prep_cdma2000_hrpd_r9_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (concurr_prep_cdma2000_hrpd_r9_present) {
    HANDLE_CODE(bref.pack(concurr_prep_cdma2000_hrpd_r9, 1));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_from_eutra_prep_request_v920_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(concurr_prep_cdma2000_hrpd_r9_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (concurr_prep_cdma2000_hrpd_r9_present) {
    HANDLE_CODE(bref.unpack(concurr_prep_cdma2000_hrpd_r9, 1));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_from_eutra_prep_request_v920_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (concurr_prep_cdma2000_hrpd_r9_present) {
    j.write_bool("concurrPrepCDMA2000-HRPD-r9", concurr_prep_cdma2000_hrpd_r9);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// LoggedMeasurementConfiguration-v1130-IEs ::= SEQUENCE
SRSASN_CODE logged_meas_cfg_v1130_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(plmn_id_list_r11_present, 1));
  HANDLE_CODE(bref.pack(area_cfg_v1130_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (plmn_id_list_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_r11, 1, 16));
  }
  if (area_cfg_v1130_present) {
    HANDLE_CODE(area_cfg_v1130.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_meas_cfg_v1130_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(plmn_id_list_r11_present, 1));
  HANDLE_CODE(bref.unpack(area_cfg_v1130_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (plmn_id_list_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_r11, bref, 1, 16));
  }
  if (area_cfg_v1130_present) {
    HANDLE_CODE(area_cfg_v1130.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void logged_meas_cfg_v1130_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_list_r11_present) {
    j.start_array("plmn-IdentityList-r11");
    for (const auto& e1 : plmn_id_list_r11) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (area_cfg_v1130_present) {
    j.write_fieldname("areaConfiguration-v1130");
    area_cfg_v1130.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MobilityFromEUTRACommand-v8d0-IEs ::= SEQUENCE
SRSASN_CODE mob_from_eutra_cmd_v8d0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_ind_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (band_ind_present) {
    HANDLE_CODE(band_ind.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_from_eutra_cmd_v8d0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_ind_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (band_ind_present) {
    HANDLE_CODE(band_ind.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mob_from_eutra_cmd_v8d0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_ind_present) {
    j.write_str("bandIndicator", band_ind.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// MobilityFromEUTRACommand-v960-IEs ::= SEQUENCE
SRSASN_CODE mob_from_eutra_cmd_v960_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_ind_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (band_ind_present) {
    HANDLE_CODE(band_ind.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_from_eutra_cmd_v960_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_ind_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (band_ind_present) {
    HANDLE_CODE(band_ind.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mob_from_eutra_cmd_v960_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_ind_present) {
    j.write_str("bandIndicator", band_ind.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReconfiguration-v920-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v920_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(other_cfg_r9_present, 1));
  HANDLE_CODE(bref.pack(full_cfg_r9_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (other_cfg_r9_present) {
    HANDLE_CODE(other_cfg_r9.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v920_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(other_cfg_r9_present, 1));
  HANDLE_CODE(bref.unpack(full_cfg_r9_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (other_cfg_r9_present) {
    HANDLE_CODE(other_cfg_r9.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_v920_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (other_cfg_r9_present) {
    j.write_fieldname("otherConfig-r9");
    other_cfg_r9.to_json(j);
  }
  if (full_cfg_r9_present) {
    j.write_str("fullConfig-r9", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionRelease-v920-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_release_v920_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_info_list_r9_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (cell_info_list_r9_present) {
    HANDLE_CODE(cell_info_list_r9.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_v920_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_info_list_r9_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cell_info_list_r9_present) {
    HANDLE_CODE(cell_info_list_r9.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_release_v920_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_info_list_r9_present) {
    j.write_fieldname("cellInfoList-r9");
    cell_info_list_r9.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

void rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::destroy_()
{
  switch (type_) {
    case types::geran_r9:
      c.destroy<cell_info_list_geran_r9_l>();
      break;
    case types::utra_fdd_r9:
      c.destroy<cell_info_list_utra_fdd_r9_l>();
      break;
    case types::utra_tdd_r9:
      c.destroy<cell_info_list_utra_tdd_r9_l>();
      break;
    case types::utra_tdd_r10:
      c.destroy<cell_info_list_utra_tdd_r10_l>();
      break;
    default:
      break;
  }
}
void rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::geran_r9:
      c.init<cell_info_list_geran_r9_l>();
      break;
    case types::utra_fdd_r9:
      c.init<cell_info_list_utra_fdd_r9_l>();
      break;
    case types::utra_tdd_r9:
      c.init<cell_info_list_utra_tdd_r9_l>();
      break;
    case types::utra_tdd_r10:
      c.init<cell_info_list_utra_tdd_r10_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_v920_ies_s::cell_info_list_r9_c_");
  }
}
rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::cell_info_list_r9_c_(
    const rrc_conn_release_v920_ies_s::cell_info_list_r9_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::geran_r9:
      c.init(other.c.get<cell_info_list_geran_r9_l>());
      break;
    case types::utra_fdd_r9:
      c.init(other.c.get<cell_info_list_utra_fdd_r9_l>());
      break;
    case types::utra_tdd_r9:
      c.init(other.c.get<cell_info_list_utra_tdd_r9_l>());
      break;
    case types::utra_tdd_r10:
      c.init(other.c.get<cell_info_list_utra_tdd_r10_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_v920_ies_s::cell_info_list_r9_c_");
  }
}
rrc_conn_release_v920_ies_s::cell_info_list_r9_c_& rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::operator=(
    const rrc_conn_release_v920_ies_s::cell_info_list_r9_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::geran_r9:
      c.set(other.c.get<cell_info_list_geran_r9_l>());
      break;
    case types::utra_fdd_r9:
      c.set(other.c.get<cell_info_list_utra_fdd_r9_l>());
      break;
    case types::utra_tdd_r9:
      c.set(other.c.get<cell_info_list_utra_tdd_r9_l>());
      break;
    case types::utra_tdd_r10:
      c.set(other.c.get<cell_info_list_utra_tdd_r10_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_v920_ies_s::cell_info_list_r9_c_");
  }

  return *this;
}
cell_info_list_geran_r9_l& rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::set_geran_r9()
{
  set(types::geran_r9);
  return c.get<cell_info_list_geran_r9_l>();
}
cell_info_list_utra_fdd_r9_l& rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::set_utra_fdd_r9()
{
  set(types::utra_fdd_r9);
  return c.get<cell_info_list_utra_fdd_r9_l>();
}
cell_info_list_utra_tdd_r9_l& rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::set_utra_tdd_r9()
{
  set(types::utra_tdd_r9);
  return c.get<cell_info_list_utra_tdd_r9_l>();
}
cell_info_list_utra_tdd_r10_l& rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::set_utra_tdd_r10()
{
  set(types::utra_tdd_r10);
  return c.get<cell_info_list_utra_tdd_r10_l>();
}
void rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::geran_r9:
      j.start_array("geran-r9");
      for (const auto& e1 : c.get<cell_info_list_geran_r9_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::utra_fdd_r9:
      j.start_array("utra-FDD-r9");
      for (const auto& e1 : c.get<cell_info_list_utra_fdd_r9_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::utra_tdd_r9:
      j.start_array("utra-TDD-r9");
      for (const auto& e1 : c.get<cell_info_list_utra_tdd_r9_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::utra_tdd_r10:
      j.start_array("utra-TDD-r10");
      for (const auto& e1 : c.get<cell_info_list_utra_tdd_r10_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_v920_ies_s::cell_info_list_r9_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::geran_r9:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<cell_info_list_geran_r9_l>(), 1, 32));
      break;
    case types::utra_fdd_r9:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<cell_info_list_utra_fdd_r9_l>(), 1, 16));
      break;
    case types::utra_tdd_r9:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<cell_info_list_utra_tdd_r9_l>(), 1, 16));
      break;
    case types::utra_tdd_r10: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<cell_info_list_utra_tdd_r10_l>(), 1, 16));
    } break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_v920_ies_s::cell_info_list_r9_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::geran_r9:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<cell_info_list_geran_r9_l>(), bref, 1, 32));
      break;
    case types::utra_fdd_r9:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<cell_info_list_utra_fdd_r9_l>(), bref, 1, 16));
      break;
    case types::utra_tdd_r9:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<cell_info_list_utra_tdd_r9_l>(), bref, 1, 16));
      break;
    case types::utra_tdd_r10: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_dyn_seq_of(c.get<cell_info_list_utra_tdd_r10_l>(), bref, 1, 16));
    } break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_v920_ies_s::cell_info_list_r9_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionResume-v1510-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_v1510_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sk_counter_r15_present, 1));
  HANDLE_CODE(bref.pack(nr_radio_bearer_cfg1_r15_present, 1));
  HANDLE_CODE(bref.pack(nr_radio_bearer_cfg2_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sk_counter_r15_present) {
    HANDLE_CODE(pack_integer(bref, sk_counter_r15, (uint32_t)0u, (uint32_t)65535u));
  }
  if (nr_radio_bearer_cfg1_r15_present) {
    HANDLE_CODE(nr_radio_bearer_cfg1_r15.pack(bref));
  }
  if (nr_radio_bearer_cfg2_r15_present) {
    HANDLE_CODE(nr_radio_bearer_cfg2_r15.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_v1510_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sk_counter_r15_present, 1));
  HANDLE_CODE(bref.unpack(nr_radio_bearer_cfg1_r15_present, 1));
  HANDLE_CODE(bref.unpack(nr_radio_bearer_cfg2_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sk_counter_r15_present) {
    HANDLE_CODE(unpack_integer(sk_counter_r15, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (nr_radio_bearer_cfg1_r15_present) {
    HANDLE_CODE(nr_radio_bearer_cfg1_r15.unpack(bref));
  }
  if (nr_radio_bearer_cfg2_r15_present) {
    HANDLE_CODE(nr_radio_bearer_cfg2_r15.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_v1510_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sk_counter_r15_present) {
    j.write_int("sk-Counter-r15", sk_counter_r15);
  }
  if (nr_radio_bearer_cfg1_r15_present) {
    j.write_str("nr-RadioBearerConfig1-r15", nr_radio_bearer_cfg1_r15.to_string());
  }
  if (nr_radio_bearer_cfg2_r15_present) {
    j.write_str("nr-RadioBearerConfig2-r15", nr_radio_bearer_cfg2_r15.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SI-OrPSI-GERAN ::= CHOICE
void si_or_psi_geran_c::destroy_()
{
  switch (type_) {
    case types::si:
      c.destroy<sys_info_list_geran_l>();
      break;
    case types::psi:
      c.destroy<sys_info_list_geran_l>();
      break;
    default:
      break;
  }
}
void si_or_psi_geran_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::si:
      c.init<sys_info_list_geran_l>();
      break;
    case types::psi:
      c.init<sys_info_list_geran_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "si_or_psi_geran_c");
  }
}
si_or_psi_geran_c::si_or_psi_geran_c(const si_or_psi_geran_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::si:
      c.init(other.c.get<sys_info_list_geran_l>());
      break;
    case types::psi:
      c.init(other.c.get<sys_info_list_geran_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "si_or_psi_geran_c");
  }
}
si_or_psi_geran_c& si_or_psi_geran_c::operator=(const si_or_psi_geran_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::si:
      c.set(other.c.get<sys_info_list_geran_l>());
      break;
    case types::psi:
      c.set(other.c.get<sys_info_list_geran_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "si_or_psi_geran_c");
  }

  return *this;
}
sys_info_list_geran_l& si_or_psi_geran_c::set_si()
{
  set(types::si);
  return c.get<sys_info_list_geran_l>();
}
sys_info_list_geran_l& si_or_psi_geran_c::set_psi()
{
  set(types::psi);
  return c.get<sys_info_list_geran_l>();
}
void si_or_psi_geran_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::si:
      j.start_array("si");
      for (const auto& e1 : c.get<sys_info_list_geran_l>()) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      break;
    case types::psi:
      j.start_array("psi");
      for (const auto& e1 : c.get<sys_info_list_geran_l>()) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "si_or_psi_geran_c");
  }
  j.end_obj();
}
SRSASN_CODE si_or_psi_geran_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::si:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sys_info_list_geran_l>(), 1, 10));
      break;
    case types::psi:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sys_info_list_geran_l>(), 1, 10));
      break;
    default:
      log_invalid_choice_id(type_, "si_or_psi_geran_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE si_or_psi_geran_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::si:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sys_info_list_geran_l>(), bref, 1, 10));
      break;
    case types::psi:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sys_info_list_geran_l>(), bref, 1, 10));
      break;
    default:
      log_invalid_choice_id(type_, "si_or_psi_geran_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEInformationRequest-v1020-IEs ::= SEQUENCE
SRSASN_CODE ue_info_request_v1020_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(log_meas_report_req_r10_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_request_v1020_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(log_meas_report_req_r10_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_info_request_v1020_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (log_meas_report_req_r10_present) {
    j.write_str("logMeasReportReq-r10", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// AreaConfiguration-r10 ::= CHOICE
void area_cfg_r10_c::destroy_()
{
  switch (type_) {
    case types::cell_global_id_list_r10:
      c.destroy<cell_global_id_list_r10_l>();
      break;
    case types::tac_list_r10:
      c.destroy<tac_list_r10_l>();
      break;
    default:
      break;
  }
}
void area_cfg_r10_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_global_id_list_r10:
      c.init<cell_global_id_list_r10_l>();
      break;
    case types::tac_list_r10:
      c.init<tac_list_r10_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "area_cfg_r10_c");
  }
}
area_cfg_r10_c::area_cfg_r10_c(const area_cfg_r10_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_global_id_list_r10:
      c.init(other.c.get<cell_global_id_list_r10_l>());
      break;
    case types::tac_list_r10:
      c.init(other.c.get<tac_list_r10_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "area_cfg_r10_c");
  }
}
area_cfg_r10_c& area_cfg_r10_c::operator=(const area_cfg_r10_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_global_id_list_r10:
      c.set(other.c.get<cell_global_id_list_r10_l>());
      break;
    case types::tac_list_r10:
      c.set(other.c.get<tac_list_r10_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "area_cfg_r10_c");
  }

  return *this;
}
cell_global_id_list_r10_l& area_cfg_r10_c::set_cell_global_id_list_r10()
{
  set(types::cell_global_id_list_r10);
  return c.get<cell_global_id_list_r10_l>();
}
tac_list_r10_l& area_cfg_r10_c::set_tac_list_r10()
{
  set(types::tac_list_r10);
  return c.get<tac_list_r10_l>();
}
void area_cfg_r10_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_global_id_list_r10:
      j.start_array("cellGlobalIdList-r10");
      for (const auto& e1 : c.get<cell_global_id_list_r10_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::tac_list_r10:
      j.start_array("trackingAreaCodeList-r10");
      for (const auto& e1 : c.get<tac_list_r10_l>()) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "area_cfg_r10_c");
  }
  j.end_obj();
}
SRSASN_CODE area_cfg_r10_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_global_id_list_r10:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<cell_global_id_list_r10_l>(), 1, 32));
      break;
    case types::tac_list_r10:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<tac_list_r10_l>(), 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "area_cfg_r10_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE area_cfg_r10_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_global_id_list_r10:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<cell_global_id_list_r10_l>(), bref, 1, 32));
      break;
    case types::tac_list_r10:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<tac_list_r10_l>(), bref, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "area_cfg_r10_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CSFBParametersResponseCDMA2000-v8a0-IEs ::= SEQUENCE
SRSASN_CODE csfb_params_resp_cdma2000_v8a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csfb_params_resp_cdma2000_v8a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csfb_params_resp_cdma2000_v8a0_ies_s::to_json(json_writer& j) const
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

// CellChangeOrder ::= SEQUENCE
SRSASN_CODE cell_change_order_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(t304.pack(bref));
  HANDLE_CODE(target_rat_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_change_order_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(t304.unpack(bref));
  HANDLE_CODE(target_rat_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_change_order_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t304", t304.to_string());
  j.write_fieldname("targetRAT-Type");
  target_rat_type.to_json(j);
  j.end_obj();
}

const char* cell_change_order_s::t304_opts::to_string() const
{
  static const char* options[] = {"ms100", "ms200", "ms500", "ms1000", "ms2000", "ms4000", "ms8000", "ms10000-v1310"};
  return convert_enum_idx(options, 8, value, "cell_change_order_s::t304_e_");
}
uint16_t cell_change_order_s::t304_opts::to_number() const
{
  static const uint16_t options[] = {100, 200, 500, 1000, 2000, 4000, 8000, 10000};
  return map_enum_number(options, 8, value, "cell_change_order_s::t304_e_");
}

void cell_change_order_s::target_rat_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("geran");
  j.start_obj();
  j.write_fieldname("physCellId");
  c.pci.to_json(j);
  j.write_fieldname("carrierFreq");
  c.carrier_freq.to_json(j);
  if (c.network_ctrl_order_present) {
    j.write_str("networkControlOrder", c.network_ctrl_order.to_string());
  }
  if (c.sys_info_present) {
    j.write_fieldname("systemInformation");
    c.sys_info.to_json(j);
  }
  j.end_obj();
  j.end_obj();
}
SRSASN_CODE cell_change_order_s::target_rat_type_c_::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(bref.pack(c.network_ctrl_order_present, 1));
  HANDLE_CODE(bref.pack(c.sys_info_present, 1));
  HANDLE_CODE(c.pci.pack(bref));
  HANDLE_CODE(c.carrier_freq.pack(bref));
  if (c.network_ctrl_order_present) {
    HANDLE_CODE(c.network_ctrl_order.pack(bref));
  }
  if (c.sys_info_present) {
    HANDLE_CODE(c.sys_info.pack(bref));
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_change_order_s::target_rat_type_c_::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "cell_change_order_s::target_rat_type_c_");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack(c.network_ctrl_order_present, 1));
  HANDLE_CODE(bref.unpack(c.sys_info_present, 1));
  HANDLE_CODE(c.pci.unpack(bref));
  HANDLE_CODE(c.carrier_freq.unpack(bref));
  if (c.network_ctrl_order_present) {
    HANDLE_CODE(c.network_ctrl_order.unpack(bref));
  }
  if (c.sys_info_present) {
    HANDLE_CODE(c.sys_info.unpack(bref));
  }
  return SRSASN_SUCCESS;
}

// CounterCheck-v8a0-IEs ::= SEQUENCE
SRSASN_CODE counter_check_v8a0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE counter_check_v8a0_ies_s::unpack(cbit_ref& bref)
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
void counter_check_v8a0_ies_s::to_json(json_writer& j) const
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

// DLInformationTransfer-v8a0-IEs ::= SEQUENCE
SRSASN_CODE dl_info_transfer_v8a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_v8a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dl_info_transfer_v8a0_ies_s::to_json(json_writer& j) const
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

// E-CSFB-r9 ::= SEQUENCE
SRSASN_CODE e_csfb_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(msg_cont_cdma2000_minus1_xrtt_r9_present, 1));
  HANDLE_CODE(bref.pack(mob_cdma2000_hrpd_r9_present, 1));
  HANDLE_CODE(bref.pack(msg_cont_cdma2000_hrpd_r9_present, 1));
  HANDLE_CODE(bref.pack(redirect_carrier_cdma2000_hrpd_r9_present, 1));

  if (msg_cont_cdma2000_minus1_xrtt_r9_present) {
    HANDLE_CODE(msg_cont_cdma2000_minus1_xrtt_r9.pack(bref));
  }
  if (mob_cdma2000_hrpd_r9_present) {
    HANDLE_CODE(mob_cdma2000_hrpd_r9.pack(bref));
  }
  if (msg_cont_cdma2000_hrpd_r9_present) {
    HANDLE_CODE(msg_cont_cdma2000_hrpd_r9.pack(bref));
  }
  if (redirect_carrier_cdma2000_hrpd_r9_present) {
    HANDLE_CODE(redirect_carrier_cdma2000_hrpd_r9.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e_csfb_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(msg_cont_cdma2000_minus1_xrtt_r9_present, 1));
  HANDLE_CODE(bref.unpack(mob_cdma2000_hrpd_r9_present, 1));
  HANDLE_CODE(bref.unpack(msg_cont_cdma2000_hrpd_r9_present, 1));
  HANDLE_CODE(bref.unpack(redirect_carrier_cdma2000_hrpd_r9_present, 1));

  if (msg_cont_cdma2000_minus1_xrtt_r9_present) {
    HANDLE_CODE(msg_cont_cdma2000_minus1_xrtt_r9.unpack(bref));
  }
  if (mob_cdma2000_hrpd_r9_present) {
    HANDLE_CODE(mob_cdma2000_hrpd_r9.unpack(bref));
  }
  if (msg_cont_cdma2000_hrpd_r9_present) {
    HANDLE_CODE(msg_cont_cdma2000_hrpd_r9.unpack(bref));
  }
  if (redirect_carrier_cdma2000_hrpd_r9_present) {
    HANDLE_CODE(redirect_carrier_cdma2000_hrpd_r9.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void e_csfb_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (msg_cont_cdma2000_minus1_xrtt_r9_present) {
    j.write_str("messageContCDMA2000-1XRTT-r9", msg_cont_cdma2000_minus1_xrtt_r9.to_string());
  }
  if (mob_cdma2000_hrpd_r9_present) {
    j.write_str("mobilityCDMA2000-HRPD-r9", mob_cdma2000_hrpd_r9.to_string());
  }
  if (msg_cont_cdma2000_hrpd_r9_present) {
    j.write_str("messageContCDMA2000-HRPD-r9", msg_cont_cdma2000_hrpd_r9.to_string());
  }
  if (redirect_carrier_cdma2000_hrpd_r9_present) {
    j.write_fieldname("redirectCarrierCDMA2000-HRPD-r9");
    redirect_carrier_cdma2000_hrpd_r9.to_json(j);
  }
  j.end_obj();
}

const char* e_csfb_r9_s::mob_cdma2000_hrpd_r9_opts::to_string() const
{
  static const char* options[] = {"handover", "redirection"};
  return convert_enum_idx(options, 2, value, "e_csfb_r9_s::mob_cdma2000_hrpd_r9_e_");
}

// Handover ::= SEQUENCE
SRSASN_CODE ho_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nas_security_param_from_eutra_present, 1));
  HANDLE_CODE(bref.pack(sys_info_present, 1));

  HANDLE_CODE(target_rat_type.pack(bref));
  HANDLE_CODE(target_rat_msg_container.pack(bref));
  if (nas_security_param_from_eutra_present) {
    HANDLE_CODE(nas_security_param_from_eutra.pack(bref));
  }
  if (sys_info_present) {
    HANDLE_CODE(sys_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nas_security_param_from_eutra_present, 1));
  HANDLE_CODE(bref.unpack(sys_info_present, 1));

  HANDLE_CODE(target_rat_type.unpack(bref));
  HANDLE_CODE(target_rat_msg_container.unpack(bref));
  if (nas_security_param_from_eutra_present) {
    HANDLE_CODE(nas_security_param_from_eutra.unpack(bref));
  }
  if (sys_info_present) {
    HANDLE_CODE(sys_info.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("targetRAT-Type", target_rat_type.to_string());
  j.write_str("targetRAT-MessageContainer", target_rat_msg_container.to_string());
  if (nas_security_param_from_eutra_present) {
    j.write_str("nas-SecurityParamFromEUTRA", nas_security_param_from_eutra.to_string());
  }
  if (sys_info_present) {
    j.write_fieldname("systemInformation");
    sys_info.to_json(j);
  }
  j.end_obj();
}

const char* ho_s::target_rat_type_opts::to_string() const
{
  static const char* options[] = {
      "utra", "geran", "cdma2000-1XRTT", "cdma2000-HRPD", "nr", "eutra", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ho_s::target_rat_type_e_");
}

// HandoverFromEUTRAPreparationRequest-v890-IEs ::= SEQUENCE
SRSASN_CODE ho_from_eutra_prep_request_v890_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE ho_from_eutra_prep_request_v890_ies_s::unpack(cbit_ref& bref)
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
void ho_from_eutra_prep_request_v890_ies_s::to_json(json_writer& j) const
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

// LoggedMeasurementConfiguration-v1080-IEs ::= SEQUENCE
SRSASN_CODE logged_meas_cfg_v1080_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_r10_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_r10_present) {
    HANDLE_CODE(late_non_crit_ext_r10.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_meas_cfg_v1080_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_r10_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_r10_present) {
    HANDLE_CODE(late_non_crit_ext_r10.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void logged_meas_cfg_v1080_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext_r10_present) {
    j.write_str("lateNonCriticalExtension-r10", late_non_crit_ext_r10.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// LoggingDuration-r10 ::= ENUMERATED
const char* logging_dur_r10_opts::to_string() const
{
  static const char* options[] = {"min10", "min20", "min40", "min60", "min90", "min120", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "logging_dur_r10_e");
}
uint8_t logging_dur_r10_opts::to_number() const
{
  static const uint8_t options[] = {10, 20, 40, 60, 90, 120};
  return map_enum_number(options, 6, value, "logging_dur_r10_e");
}

// LoggingInterval-r10 ::= ENUMERATED
const char* logging_interv_r10_opts::to_string() const
{
  static const char* options[] = {"ms1280", "ms2560", "ms5120", "ms10240", "ms20480", "ms30720", "ms40960", "ms61440"};
  return convert_enum_idx(options, 8, value, "logging_interv_r10_e");
}
uint16_t logging_interv_r10_opts::to_number() const
{
  static const uint16_t options[] = {1280, 2560, 5120, 10240, 20480, 30720, 40960, 61440};
  return map_enum_number(options, 8, value, "logging_interv_r10_e");
}

// MobilityFromEUTRACommand-v8a0-IEs ::= SEQUENCE
SRSASN_CODE mob_from_eutra_cmd_v8a0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE mob_from_eutra_cmd_v8a0_ies_s::unpack(cbit_ref& bref)
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
void mob_from_eutra_cmd_v8a0_ies_s::to_json(json_writer& j) const
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

// MobilityFromEUTRACommand-v930-IEs ::= SEQUENCE
SRSASN_CODE mob_from_eutra_cmd_v930_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE mob_from_eutra_cmd_v930_ies_s::unpack(cbit_ref& bref)
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
void mob_from_eutra_cmd_v930_ies_s::to_json(json_writer& j) const
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

// RN-SubframeConfig-r10 ::= SEQUENCE
SRSASN_CODE rn_sf_cfg_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sf_cfg_pattern_r10_present, 1));
  HANDLE_CODE(bref.pack(rpdcch_cfg_r10_present, 1));

  if (sf_cfg_pattern_r10_present) {
    HANDLE_CODE(sf_cfg_pattern_r10.pack(bref));
  }
  if (rpdcch_cfg_r10_present) {
    bref.pack(rpdcch_cfg_r10.ext, 1);
    HANDLE_CODE(rpdcch_cfg_r10.res_alloc_type_r10.pack(bref));
    HANDLE_CODE(rpdcch_cfg_r10.res_block_assign_r10.pack(bref));
    HANDLE_CODE(rpdcch_cfg_r10.demod_rs_r10.pack(bref));
    HANDLE_CODE(pack_integer(bref, rpdcch_cfg_r10.pdsch_start_r10, (uint8_t)1u, (uint8_t)3u));
    HANDLE_CODE(rpdcch_cfg_r10.pucch_cfg_r10.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_sf_cfg_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sf_cfg_pattern_r10_present, 1));
  HANDLE_CODE(bref.unpack(rpdcch_cfg_r10_present, 1));

  if (sf_cfg_pattern_r10_present) {
    HANDLE_CODE(sf_cfg_pattern_r10.unpack(bref));
  }
  if (rpdcch_cfg_r10_present) {
    bref.unpack(rpdcch_cfg_r10.ext, 1);
    HANDLE_CODE(rpdcch_cfg_r10.res_alloc_type_r10.unpack(bref));
    HANDLE_CODE(rpdcch_cfg_r10.res_block_assign_r10.unpack(bref));
    HANDLE_CODE(rpdcch_cfg_r10.demod_rs_r10.unpack(bref));
    HANDLE_CODE(unpack_integer(rpdcch_cfg_r10.pdsch_start_r10, bref, (uint8_t)1u, (uint8_t)3u));
    HANDLE_CODE(rpdcch_cfg_r10.pucch_cfg_r10.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rn_sf_cfg_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sf_cfg_pattern_r10_present) {
    j.write_fieldname("subframeConfigPattern-r10");
    sf_cfg_pattern_r10.to_json(j);
  }
  if (rpdcch_cfg_r10_present) {
    j.write_fieldname("rpdcch-Config-r10");
    j.start_obj();
    j.write_str("resourceAllocationType-r10", rpdcch_cfg_r10.res_alloc_type_r10.to_string());
    j.write_fieldname("resourceBlockAssignment-r10");
    rpdcch_cfg_r10.res_block_assign_r10.to_json(j);
    j.write_fieldname("demodulationRS-r10");
    rpdcch_cfg_r10.demod_rs_r10.to_json(j);
    j.write_int("pdsch-Start-r10", rpdcch_cfg_r10.pdsch_start_r10);
    j.write_fieldname("pucch-Config-r10");
    rpdcch_cfg_r10.pucch_cfg_r10.to_json(j);
    j.end_obj();
  }
  j.end_obj();
}

void rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::destroy_()
{
  switch (type_) {
    case types::sf_cfg_pattern_fdd_r10:
      c.destroy<fixed_bitstring<8> >();
      break;
    default:
      break;
  }
}
void rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sf_cfg_pattern_fdd_r10:
      c.init<fixed_bitstring<8> >();
      break;
    case types::sf_cfg_pattern_tdd_r10:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_");
  }
}
rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::sf_cfg_pattern_r10_c_(const rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf_cfg_pattern_fdd_r10:
      c.init(other.c.get<fixed_bitstring<8> >());
      break;
    case types::sf_cfg_pattern_tdd_r10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_");
  }
}
rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_&
rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::operator=(const rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf_cfg_pattern_fdd_r10:
      c.set(other.c.get<fixed_bitstring<8> >());
      break;
    case types::sf_cfg_pattern_tdd_r10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_");
  }

  return *this;
}
fixed_bitstring<8>& rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::set_sf_cfg_pattern_fdd_r10()
{
  set(types::sf_cfg_pattern_fdd_r10);
  return c.get<fixed_bitstring<8> >();
}
uint8_t& rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::set_sf_cfg_pattern_tdd_r10()
{
  set(types::sf_cfg_pattern_tdd_r10);
  return c.get<uint8_t>();
}
void rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf_cfg_pattern_fdd_r10:
      j.write_str("subframeConfigPatternFDD-r10", c.get<fixed_bitstring<8> >().to_string());
      break;
    case types::sf_cfg_pattern_tdd_r10:
      j.write_int("subframeConfigPatternTDD-r10", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf_cfg_pattern_fdd_r10:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().pack(bref));
      break;
    case types::sf_cfg_pattern_tdd_r10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf_cfg_pattern_fdd_r10:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().unpack(bref));
      break;
    case types::sf_cfg_pattern_tdd_r10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_alloc_type_r10_opts::to_string() const
{
  static const char* options[] = {
      "type0", "type1", "type2Localized", "type2Distributed", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_alloc_type_r10_e_");
}

void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::destroy_()
{
  switch (type_) {
    case types::type01_r10:
      c.destroy<type01_r10_c_>();
      break;
    case types::type2_r10:
      c.destroy<type2_r10_c_>();
      break;
    default:
      break;
  }
}
void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::type01_r10:
      c.init<type01_r10_c_>();
      break;
    case types::type2_r10:
      c.init<type2_r10_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_");
  }
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::res_block_assign_r10_c_(
    const rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::type01_r10:
      c.init(other.c.get<type01_r10_c_>());
      break;
    case types::type2_r10:
      c.init(other.c.get<type2_r10_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_");
  }
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_&
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::operator=(
    const rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::type01_r10:
      c.set(other.c.get<type01_r10_c_>());
      break;
    case types::type2_r10:
      c.set(other.c.get<type2_r10_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_");
  }

  return *this;
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_&
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::set_type01_r10()
{
  set(types::type01_r10);
  return c.get<type01_r10_c_>();
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_&
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::set_type2_r10()
{
  set(types::type2_r10);
  return c.get<type2_r10_c_>();
}
void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::type01_r10:
      j.write_fieldname("type01-r10");
      c.get<type01_r10_c_>().to_json(j);
      break;
    case types::type2_r10:
      j.write_fieldname("type2-r10");
      c.get<type2_r10_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::type01_r10:
      HANDLE_CODE(c.get<type01_r10_c_>().pack(bref));
      break;
    case types::type2_r10:
      HANDLE_CODE(c.get<type2_r10_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::type01_r10:
      HANDLE_CODE(c.get<type01_r10_c_>().unpack(bref));
      break;
    case types::type2_r10:
      HANDLE_CODE(c.get<type2_r10_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::destroy_()
{
  switch (type_) {
    case types::nrb6_r10:
      c.destroy<fixed_bitstring<6> >();
      break;
    case types::nrb15_r10:
      c.destroy<fixed_bitstring<8> >();
      break;
    case types::nrb25_r10:
      c.destroy<fixed_bitstring<13> >();
      break;
    case types::nrb50_r10:
      c.destroy<fixed_bitstring<17> >();
      break;
    case types::nrb75_r10:
      c.destroy<fixed_bitstring<19> >();
      break;
    case types::nrb100_r10:
      c.destroy<fixed_bitstring<25> >();
      break;
    default:
      break;
  }
}
void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nrb6_r10:
      c.init<fixed_bitstring<6> >();
      break;
    case types::nrb15_r10:
      c.init<fixed_bitstring<8> >();
      break;
    case types::nrb25_r10:
      c.init<fixed_bitstring<13> >();
      break;
    case types::nrb50_r10:
      c.init<fixed_bitstring<17> >();
      break;
    case types::nrb75_r10:
      c.init<fixed_bitstring<19> >();
      break;
    case types::nrb100_r10:
      c.init<fixed_bitstring<25> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_");
  }
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::type01_r10_c_(
    const rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nrb6_r10:
      c.init(other.c.get<fixed_bitstring<6> >());
      break;
    case types::nrb15_r10:
      c.init(other.c.get<fixed_bitstring<8> >());
      break;
    case types::nrb25_r10:
      c.init(other.c.get<fixed_bitstring<13> >());
      break;
    case types::nrb50_r10:
      c.init(other.c.get<fixed_bitstring<17> >());
      break;
    case types::nrb75_r10:
      c.init(other.c.get<fixed_bitstring<19> >());
      break;
    case types::nrb100_r10:
      c.init(other.c.get<fixed_bitstring<25> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_");
  }
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_&
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::operator=(
    const rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nrb6_r10:
      c.set(other.c.get<fixed_bitstring<6> >());
      break;
    case types::nrb15_r10:
      c.set(other.c.get<fixed_bitstring<8> >());
      break;
    case types::nrb25_r10:
      c.set(other.c.get<fixed_bitstring<13> >());
      break;
    case types::nrb50_r10:
      c.set(other.c.get<fixed_bitstring<17> >());
      break;
    case types::nrb75_r10:
      c.set(other.c.get<fixed_bitstring<19> >());
      break;
    case types::nrb100_r10:
      c.set(other.c.get<fixed_bitstring<25> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_");
  }

  return *this;
}
fixed_bitstring<6>& rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::set_nrb6_r10()
{
  set(types::nrb6_r10);
  return c.get<fixed_bitstring<6> >();
}
fixed_bitstring<8>& rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::set_nrb15_r10()
{
  set(types::nrb15_r10);
  return c.get<fixed_bitstring<8> >();
}
fixed_bitstring<13>& rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::set_nrb25_r10()
{
  set(types::nrb25_r10);
  return c.get<fixed_bitstring<13> >();
}
fixed_bitstring<17>& rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::set_nrb50_r10()
{
  set(types::nrb50_r10);
  return c.get<fixed_bitstring<17> >();
}
fixed_bitstring<19>& rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::set_nrb75_r10()
{
  set(types::nrb75_r10);
  return c.get<fixed_bitstring<19> >();
}
fixed_bitstring<25>& rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::set_nrb100_r10()
{
  set(types::nrb100_r10);
  return c.get<fixed_bitstring<25> >();
}
void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nrb6_r10:
      j.write_str("nrb6-r10", c.get<fixed_bitstring<6> >().to_string());
      break;
    case types::nrb15_r10:
      j.write_str("nrb15-r10", c.get<fixed_bitstring<8> >().to_string());
      break;
    case types::nrb25_r10:
      j.write_str("nrb25-r10", c.get<fixed_bitstring<13> >().to_string());
      break;
    case types::nrb50_r10:
      j.write_str("nrb50-r10", c.get<fixed_bitstring<17> >().to_string());
      break;
    case types::nrb75_r10:
      j.write_str("nrb75-r10", c.get<fixed_bitstring<19> >().to_string());
      break;
    case types::nrb100_r10:
      j.write_str("nrb100-r10", c.get<fixed_bitstring<25> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nrb6_r10:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().pack(bref));
      break;
    case types::nrb15_r10:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().pack(bref));
      break;
    case types::nrb25_r10:
      HANDLE_CODE(c.get<fixed_bitstring<13> >().pack(bref));
      break;
    case types::nrb50_r10:
      HANDLE_CODE(c.get<fixed_bitstring<17> >().pack(bref));
      break;
    case types::nrb75_r10:
      HANDLE_CODE(c.get<fixed_bitstring<19> >().pack(bref));
      break;
    case types::nrb100_r10:
      HANDLE_CODE(c.get<fixed_bitstring<25> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nrb6_r10:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().unpack(bref));
      break;
    case types::nrb15_r10:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().unpack(bref));
      break;
    case types::nrb25_r10:
      HANDLE_CODE(c.get<fixed_bitstring<13> >().unpack(bref));
      break;
    case types::nrb50_r10:
      HANDLE_CODE(c.get<fixed_bitstring<17> >().unpack(bref));
      break;
    case types::nrb75_r10:
      HANDLE_CODE(c.get<fixed_bitstring<19> >().unpack(bref));
      break;
    case types::nrb100_r10:
      HANDLE_CODE(c.get<fixed_bitstring<25> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::destroy_()
{
  switch (type_) {
    case types::nrb6_r10:
      c.destroy<fixed_bitstring<5> >();
      break;
    case types::nrb15_r10:
      c.destroy<fixed_bitstring<7> >();
      break;
    case types::nrb25_r10:
      c.destroy<fixed_bitstring<9> >();
      break;
    case types::nrb50_r10:
      c.destroy<fixed_bitstring<11> >();
      break;
    case types::nrb75_r10:
      c.destroy<fixed_bitstring<12> >();
      break;
    case types::nrb100_r10:
      c.destroy<fixed_bitstring<13> >();
      break;
    default:
      break;
  }
}
void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nrb6_r10:
      c.init<fixed_bitstring<5> >();
      break;
    case types::nrb15_r10:
      c.init<fixed_bitstring<7> >();
      break;
    case types::nrb25_r10:
      c.init<fixed_bitstring<9> >();
      break;
    case types::nrb50_r10:
      c.init<fixed_bitstring<11> >();
      break;
    case types::nrb75_r10:
      c.init<fixed_bitstring<12> >();
      break;
    case types::nrb100_r10:
      c.init<fixed_bitstring<13> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_");
  }
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::type2_r10_c_(
    const rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nrb6_r10:
      c.init(other.c.get<fixed_bitstring<5> >());
      break;
    case types::nrb15_r10:
      c.init(other.c.get<fixed_bitstring<7> >());
      break;
    case types::nrb25_r10:
      c.init(other.c.get<fixed_bitstring<9> >());
      break;
    case types::nrb50_r10:
      c.init(other.c.get<fixed_bitstring<11> >());
      break;
    case types::nrb75_r10:
      c.init(other.c.get<fixed_bitstring<12> >());
      break;
    case types::nrb100_r10:
      c.init(other.c.get<fixed_bitstring<13> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_");
  }
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_&
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::operator=(
    const rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nrb6_r10:
      c.set(other.c.get<fixed_bitstring<5> >());
      break;
    case types::nrb15_r10:
      c.set(other.c.get<fixed_bitstring<7> >());
      break;
    case types::nrb25_r10:
      c.set(other.c.get<fixed_bitstring<9> >());
      break;
    case types::nrb50_r10:
      c.set(other.c.get<fixed_bitstring<11> >());
      break;
    case types::nrb75_r10:
      c.set(other.c.get<fixed_bitstring<12> >());
      break;
    case types::nrb100_r10:
      c.set(other.c.get<fixed_bitstring<13> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_");
  }

  return *this;
}
fixed_bitstring<5>& rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::set_nrb6_r10()
{
  set(types::nrb6_r10);
  return c.get<fixed_bitstring<5> >();
}
fixed_bitstring<7>& rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::set_nrb15_r10()
{
  set(types::nrb15_r10);
  return c.get<fixed_bitstring<7> >();
}
fixed_bitstring<9>& rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::set_nrb25_r10()
{
  set(types::nrb25_r10);
  return c.get<fixed_bitstring<9> >();
}
fixed_bitstring<11>& rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::set_nrb50_r10()
{
  set(types::nrb50_r10);
  return c.get<fixed_bitstring<11> >();
}
fixed_bitstring<12>& rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::set_nrb75_r10()
{
  set(types::nrb75_r10);
  return c.get<fixed_bitstring<12> >();
}
fixed_bitstring<13>& rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::set_nrb100_r10()
{
  set(types::nrb100_r10);
  return c.get<fixed_bitstring<13> >();
}
void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nrb6_r10:
      j.write_str("nrb6-r10", c.get<fixed_bitstring<5> >().to_string());
      break;
    case types::nrb15_r10:
      j.write_str("nrb15-r10", c.get<fixed_bitstring<7> >().to_string());
      break;
    case types::nrb25_r10:
      j.write_str("nrb25-r10", c.get<fixed_bitstring<9> >().to_string());
      break;
    case types::nrb50_r10:
      j.write_str("nrb50-r10", c.get<fixed_bitstring<11> >().to_string());
      break;
    case types::nrb75_r10:
      j.write_str("nrb75-r10", c.get<fixed_bitstring<12> >().to_string());
      break;
    case types::nrb100_r10:
      j.write_str("nrb100-r10", c.get<fixed_bitstring<13> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nrb6_r10:
      HANDLE_CODE(c.get<fixed_bitstring<5> >().pack(bref));
      break;
    case types::nrb15_r10:
      HANDLE_CODE(c.get<fixed_bitstring<7> >().pack(bref));
      break;
    case types::nrb25_r10:
      HANDLE_CODE(c.get<fixed_bitstring<9> >().pack(bref));
      break;
    case types::nrb50_r10:
      HANDLE_CODE(c.get<fixed_bitstring<11> >().pack(bref));
      break;
    case types::nrb75_r10:
      HANDLE_CODE(c.get<fixed_bitstring<12> >().pack(bref));
      break;
    case types::nrb100_r10:
      HANDLE_CODE(c.get<fixed_bitstring<13> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nrb6_r10:
      HANDLE_CODE(c.get<fixed_bitstring<5> >().unpack(bref));
      break;
    case types::nrb15_r10:
      HANDLE_CODE(c.get<fixed_bitstring<7> >().unpack(bref));
      break;
    case types::nrb25_r10:
      HANDLE_CODE(c.get<fixed_bitstring<9> >().unpack(bref));
      break;
    case types::nrb50_r10:
      HANDLE_CODE(c.get<fixed_bitstring<11> >().unpack(bref));
      break;
    case types::nrb75_r10:
      HANDLE_CODE(c.get<fixed_bitstring<12> >().unpack(bref));
      break;
    case types::nrb100_r10:
      HANDLE_CODE(c.get<fixed_bitstring<13> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::set(types::options e)
{
  type_ = e;
}
void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::set_interleaving_r10()
{
  set(types::interleaving_r10);
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::no_interleaving_r10_e_&
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::set_no_interleaving_r10()
{
  set(types::no_interleaving_r10);
  return c;
}
void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::interleaving_r10:
      break;
    case types::no_interleaving_r10:
      j.write_str("noInterleaving-r10", c.to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::interleaving_r10:
      break;
    case types::no_interleaving_r10:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::interleaving_r10:
      break;
    case types::no_interleaving_r10:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::no_interleaving_r10_opts::to_string() const
{
  static const char* options[] = {"crs", "dmrs"};
  return convert_enum_idx(
      options, 2, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::no_interleaving_r10_e_");
}

void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::destroy_()
{
  switch (type_) {
    case types::tdd:
      c.destroy<tdd_c_>();
      break;
    case types::fdd:
      c.destroy<fdd_s_>();
      break;
    default:
      break;
  }
}
void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::tdd:
      c.init<tdd_c_>();
      break;
    case types::fdd:
      c.init<fdd_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_");
  }
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::pucch_cfg_r10_c_(
    const rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::tdd:
      c.init(other.c.get<tdd_c_>());
      break;
    case types::fdd:
      c.init(other.c.get<fdd_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_");
  }
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_& rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::operator=(
    const rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::tdd:
      c.set(other.c.get<tdd_c_>());
      break;
    case types::fdd:
      c.set(other.c.get<fdd_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_");
  }

  return *this;
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_&
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::set_tdd()
{
  set(types::tdd);
  return c.get<tdd_c_>();
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::fdd_s_&
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::set_fdd()
{
  set(types::fdd);
  return c.get<fdd_s_>();
}
void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::tdd:
      j.write_fieldname("tdd");
      c.get<tdd_c_>().to_json(j);
      break;
    case types::fdd:
      j.write_fieldname("fdd");
      j.start_obj();
      j.write_int("n1PUCCH-AN-P0-r10", c.get<fdd_s_>().n1_pucch_an_p0_r10);
      if (c.get<fdd_s_>().n1_pucch_an_p1_r10_present) {
        j.write_int("n1PUCCH-AN-P1-r10", c.get<fdd_s_>().n1_pucch_an_p1_r10);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::tdd:
      HANDLE_CODE(c.get<tdd_c_>().pack(bref));
      break;
    case types::fdd:
      HANDLE_CODE(bref.pack(c.get<fdd_s_>().n1_pucch_an_p1_r10_present, 1));
      HANDLE_CODE(pack_integer(bref, c.get<fdd_s_>().n1_pucch_an_p0_r10, (uint16_t)0u, (uint16_t)2047u));
      if (c.get<fdd_s_>().n1_pucch_an_p1_r10_present) {
        HANDLE_CODE(pack_integer(bref, c.get<fdd_s_>().n1_pucch_an_p1_r10, (uint16_t)0u, (uint16_t)2047u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::tdd:
      HANDLE_CODE(c.get<tdd_c_>().unpack(bref));
      break;
    case types::fdd:
      HANDLE_CODE(bref.unpack(c.get<fdd_s_>().n1_pucch_an_p1_r10_present, 1));
      HANDLE_CODE(unpack_integer(c.get<fdd_s_>().n1_pucch_an_p0_r10, bref, (uint16_t)0u, (uint16_t)2047u));
      if (c.get<fdd_s_>().n1_pucch_an_p1_r10_present) {
        HANDLE_CODE(unpack_integer(c.get<fdd_s_>().n1_pucch_an_p1_r10, bref, (uint16_t)0u, (uint16_t)2047u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::destroy_()
{
  switch (type_) {
    case types::ch_sel_mux_bundling:
      c.destroy<ch_sel_mux_bundling_s_>();
      break;
    case types::fallback_for_format3:
      c.destroy<fallback_for_format3_s_>();
      break;
    default:
      break;
  }
}
void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ch_sel_mux_bundling:
      c.init<ch_sel_mux_bundling_s_>();
      break;
    case types::fallback_for_format3:
      c.init<fallback_for_format3_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_");
  }
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::tdd_c_(
    const rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ch_sel_mux_bundling:
      c.init(other.c.get<ch_sel_mux_bundling_s_>());
      break;
    case types::fallback_for_format3:
      c.init(other.c.get<fallback_for_format3_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_");
  }
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_&
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::operator=(
    const rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ch_sel_mux_bundling:
      c.set(other.c.get<ch_sel_mux_bundling_s_>());
      break;
    case types::fallback_for_format3:
      c.set(other.c.get<fallback_for_format3_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_");
  }

  return *this;
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::ch_sel_mux_bundling_s_&
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::set_ch_sel_mux_bundling()
{
  set(types::ch_sel_mux_bundling);
  return c.get<ch_sel_mux_bundling_s_>();
}
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::fallback_for_format3_s_&
rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::set_fallback_for_format3()
{
  set(types::fallback_for_format3);
  return c.get<fallback_for_format3_s_>();
}
void rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ch_sel_mux_bundling:
      j.write_fieldname("channelSelectionMultiplexingBundling");
      j.start_obj();
      j.start_array("n1PUCCH-AN-List-r10");
      for (const auto& e1 : c.get<ch_sel_mux_bundling_s_>().n1_pucch_an_list_r10) {
        j.write_int(e1);
      }
      j.end_array();
      j.end_obj();
      break;
    case types::fallback_for_format3:
      j.write_fieldname("fallbackForFormat3");
      j.start_obj();
      j.write_int("n1PUCCH-AN-P0-r10", c.get<fallback_for_format3_s_>().n1_pucch_an_p0_r10);
      if (c.get<fallback_for_format3_s_>().n1_pucch_an_p1_r10_present) {
        j.write_int("n1PUCCH-AN-P1-r10", c.get<fallback_for_format3_s_>().n1_pucch_an_p1_r10);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_");
  }
  j.end_obj();
}
SRSASN_CODE rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ch_sel_mux_bundling:
      HANDLE_CODE(pack_dyn_seq_of(
          bref, c.get<ch_sel_mux_bundling_s_>().n1_pucch_an_list_r10, 1, 4, integer_packer<uint16_t>(0, 2047)));
      break;
    case types::fallback_for_format3:
      HANDLE_CODE(bref.pack(c.get<fallback_for_format3_s_>().n1_pucch_an_p1_r10_present, 1));
      HANDLE_CODE(
          pack_integer(bref, c.get<fallback_for_format3_s_>().n1_pucch_an_p0_r10, (uint16_t)0u, (uint16_t)2047u));
      if (c.get<fallback_for_format3_s_>().n1_pucch_an_p1_r10_present) {
        HANDLE_CODE(
            pack_integer(bref, c.get<fallback_for_format3_s_>().n1_pucch_an_p1_r10, (uint16_t)0u, (uint16_t)2047u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ch_sel_mux_bundling:
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<ch_sel_mux_bundling_s_>().n1_pucch_an_list_r10, bref, 1, 4, integer_packer<uint16_t>(0, 2047)));
      break;
    case types::fallback_for_format3:
      HANDLE_CODE(bref.unpack(c.get<fallback_for_format3_s_>().n1_pucch_an_p1_r10_present, 1));
      HANDLE_CODE(
          unpack_integer(c.get<fallback_for_format3_s_>().n1_pucch_an_p0_r10, bref, (uint16_t)0u, (uint16_t)2047u));
      if (c.get<fallback_for_format3_s_>().n1_pucch_an_p1_r10_present) {
        HANDLE_CODE(
            unpack_integer(c.get<fallback_for_format3_s_>().n1_pucch_an_p1_r10, bref, (uint16_t)0u, (uint16_t)2047u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RN-SystemInfo-r10 ::= SEQUENCE
SRSASN_CODE rn_sys_info_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sib_type1_r10_present, 1));
  HANDLE_CODE(bref.pack(sib_type2_r10_present, 1));

  if (sib_type1_r10_present) {
    HANDLE_CODE(sib_type1_r10.pack(bref));
  }
  if (sib_type2_r10_present) {
    HANDLE_CODE(sib_type2_r10.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_sys_info_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sib_type1_r10_present, 1));
  HANDLE_CODE(bref.unpack(sib_type2_r10_present, 1));

  if (sib_type1_r10_present) {
    HANDLE_CODE(sib_type1_r10.unpack(bref));
  }
  if (sib_type2_r10_present) {
    HANDLE_CODE(sib_type2_r10.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rn_sys_info_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sib_type1_r10_present) {
    j.write_str("systemInformationBlockType1-r10", sib_type1_r10.to_string());
  }
  if (sib_type2_r10_present) {
    j.write_fieldname("systemInformationBlockType2-r10");
    sib_type2_r10.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReconfiguration-v890-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v890_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE rrc_conn_recfg_v890_ies_s::unpack(cbit_ref& bref)
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
void rrc_conn_recfg_v890_ies_s::to_json(json_writer& j) const
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

// RRCConnectionRelease-v890-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_release_v890_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE rrc_conn_release_v890_ies_s::unpack(cbit_ref& bref)
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
void rrc_conn_release_v890_ies_s::to_json(json_writer& j) const
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

// RRCConnectionResume-v1430-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(other_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(rrc_conn_resume_v1510_ies_present, 1));

  if (other_cfg_r14_present) {
    HANDLE_CODE(other_cfg_r14.pack(bref));
  }
  if (rrc_conn_resume_v1510_ies_present) {
    HANDLE_CODE(rrc_conn_resume_v1510_ies.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(other_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(rrc_conn_resume_v1510_ies_present, 1));

  if (other_cfg_r14_present) {
    HANDLE_CODE(other_cfg_r14.unpack(bref));
  }
  if (rrc_conn_resume_v1510_ies_present) {
    HANDLE_CODE(rrc_conn_resume_v1510_ies.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (other_cfg_r14_present) {
    j.write_fieldname("otherConfig-r14");
    other_cfg_r14.to_json(j);
  }
  if (rrc_conn_resume_v1510_ies_present) {
    j.write_fieldname("rrcConnectionResume-v1510-IEs");
    rrc_conn_resume_v1510_ies.to_json(j);
  }
  j.end_obj();
}

// RedirectedCarrierInfo ::= CHOICE
void redirected_carrier_info_c::destroy_()
{
  switch (type_) {
    case types::geran:
      c.destroy<carrier_freqs_geran_s>();
      break;
    case types::cdma2000_hrpd:
      c.destroy<carrier_freq_cdma2000_s>();
      break;
    case types::cdma2000_minus1x_rtt:
      c.destroy<carrier_freq_cdma2000_s>();
      break;
    case types::utra_tdd_r10:
      c.destroy<carrier_freq_list_utra_tdd_r10_l>();
      break;
    case types::nr_r15:
      c.destroy<carrier_info_nr_r15_s>();
      break;
    default:
      break;
  }
}
void redirected_carrier_info_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::eutra:
      break;
    case types::geran:
      c.init<carrier_freqs_geran_s>();
      break;
    case types::utra_fdd:
      break;
    case types::utra_tdd:
      break;
    case types::cdma2000_hrpd:
      c.init<carrier_freq_cdma2000_s>();
      break;
    case types::cdma2000_minus1x_rtt:
      c.init<carrier_freq_cdma2000_s>();
      break;
    case types::utra_tdd_r10:
      c.init<carrier_freq_list_utra_tdd_r10_l>();
      break;
    case types::nr_r15:
      c.init<carrier_info_nr_r15_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_c");
  }
}
redirected_carrier_info_c::redirected_carrier_info_c(const redirected_carrier_info_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::eutra:
      c.init(other.c.get<uint32_t>());
      break;
    case types::geran:
      c.init(other.c.get<carrier_freqs_geran_s>());
      break;
    case types::utra_fdd:
      c.init(other.c.get<uint16_t>());
      break;
    case types::utra_tdd:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cdma2000_hrpd:
      c.init(other.c.get<carrier_freq_cdma2000_s>());
      break;
    case types::cdma2000_minus1x_rtt:
      c.init(other.c.get<carrier_freq_cdma2000_s>());
      break;
    case types::utra_tdd_r10:
      c.init(other.c.get<carrier_freq_list_utra_tdd_r10_l>());
      break;
    case types::nr_r15:
      c.init(other.c.get<carrier_info_nr_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_c");
  }
}
redirected_carrier_info_c& redirected_carrier_info_c::operator=(const redirected_carrier_info_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::eutra:
      c.set(other.c.get<uint32_t>());
      break;
    case types::geran:
      c.set(other.c.get<carrier_freqs_geran_s>());
      break;
    case types::utra_fdd:
      c.set(other.c.get<uint16_t>());
      break;
    case types::utra_tdd:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cdma2000_hrpd:
      c.set(other.c.get<carrier_freq_cdma2000_s>());
      break;
    case types::cdma2000_minus1x_rtt:
      c.set(other.c.get<carrier_freq_cdma2000_s>());
      break;
    case types::utra_tdd_r10:
      c.set(other.c.get<carrier_freq_list_utra_tdd_r10_l>());
      break;
    case types::nr_r15:
      c.set(other.c.get<carrier_info_nr_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_c");
  }

  return *this;
}
uint32_t& redirected_carrier_info_c::set_eutra()
{
  set(types::eutra);
  return c.get<uint32_t>();
}
carrier_freqs_geran_s& redirected_carrier_info_c::set_geran()
{
  set(types::geran);
  return c.get<carrier_freqs_geran_s>();
}
uint16_t& redirected_carrier_info_c::set_utra_fdd()
{
  set(types::utra_fdd);
  return c.get<uint16_t>();
}
uint16_t& redirected_carrier_info_c::set_utra_tdd()
{
  set(types::utra_tdd);
  return c.get<uint16_t>();
}
carrier_freq_cdma2000_s& redirected_carrier_info_c::set_cdma2000_hrpd()
{
  set(types::cdma2000_hrpd);
  return c.get<carrier_freq_cdma2000_s>();
}
carrier_freq_cdma2000_s& redirected_carrier_info_c::set_cdma2000_minus1x_rtt()
{
  set(types::cdma2000_minus1x_rtt);
  return c.get<carrier_freq_cdma2000_s>();
}
carrier_freq_list_utra_tdd_r10_l& redirected_carrier_info_c::set_utra_tdd_r10()
{
  set(types::utra_tdd_r10);
  return c.get<carrier_freq_list_utra_tdd_r10_l>();
}
carrier_info_nr_r15_s& redirected_carrier_info_c::set_nr_r15()
{
  set(types::nr_r15);
  return c.get<carrier_info_nr_r15_s>();
}
void redirected_carrier_info_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::eutra:
      j.write_int("eutra", c.get<uint32_t>());
      break;
    case types::geran:
      j.write_fieldname("geran");
      c.get<carrier_freqs_geran_s>().to_json(j);
      break;
    case types::utra_fdd:
      j.write_int("utra-FDD", c.get<uint16_t>());
      break;
    case types::utra_tdd:
      j.write_int("utra-TDD", c.get<uint16_t>());
      break;
    case types::cdma2000_hrpd:
      j.write_fieldname("cdma2000-HRPD");
      c.get<carrier_freq_cdma2000_s>().to_json(j);
      break;
    case types::cdma2000_minus1x_rtt:
      j.write_fieldname("cdma2000-1xRTT");
      c.get<carrier_freq_cdma2000_s>().to_json(j);
      break;
    case types::utra_tdd_r10:
      j.start_array("utra-TDD-r10");
      for (const auto& e1 : c.get<carrier_freq_list_utra_tdd_r10_l>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::nr_r15:
      j.write_fieldname("nr-r15");
      c.get<carrier_info_nr_r15_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_c");
  }
  j.end_obj();
}
SRSASN_CODE redirected_carrier_info_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::eutra:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)0u, (uint32_t)65535u));
      break;
    case types::geran:
      HANDLE_CODE(c.get<carrier_freqs_geran_s>().pack(bref));
      break;
    case types::utra_fdd:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)16383u));
      break;
    case types::utra_tdd:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)16383u));
      break;
    case types::cdma2000_hrpd:
      HANDLE_CODE(c.get<carrier_freq_cdma2000_s>().pack(bref));
      break;
    case types::cdma2000_minus1x_rtt:
      HANDLE_CODE(c.get<carrier_freq_cdma2000_s>().pack(bref));
      break;
    case types::utra_tdd_r10: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(
          pack_dyn_seq_of(bref, c.get<carrier_freq_list_utra_tdd_r10_l>(), 1, 6, integer_packer<uint16_t>(0, 16383)));
    } break;
    case types::nr_r15: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<carrier_info_nr_r15_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE redirected_carrier_info_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::eutra:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)0u, (uint32_t)65535u));
      break;
    case types::geran:
      HANDLE_CODE(c.get<carrier_freqs_geran_s>().unpack(bref));
      break;
    case types::utra_fdd:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)16383u));
      break;
    case types::utra_tdd:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)16383u));
      break;
    case types::cdma2000_hrpd:
      HANDLE_CODE(c.get<carrier_freq_cdma2000_s>().unpack(bref));
      break;
    case types::cdma2000_minus1x_rtt:
      HANDLE_CODE(c.get<carrier_freq_cdma2000_s>().unpack(bref));
      break;
    case types::utra_tdd_r10: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(
          unpack_dyn_seq_of(c.get<carrier_freq_list_utra_tdd_r10_l>(), bref, 1, 6, integer_packer<uint16_t>(0, 16383)));
    } break;
    case types::nr_r15: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<carrier_info_nr_r15_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ReleaseCause ::= ENUMERATED
const char* release_cause_opts::to_string() const
{
  static const char* options[] = {
      "loadBalancingTAUrequired", "other", "cs-FallbackHighPriority-v1020", "rrc-Suspend-v1320"};
  return convert_enum_idx(options, 4, value, "release_cause_e");
}

// UEInformationRequest-v930-IEs ::= SEQUENCE
SRSASN_CODE ue_info_request_v930_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE ue_info_request_v930_ies_s::unpack(cbit_ref& bref)
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
void ue_info_request_v930_ies_s::to_json(json_writer& j) const
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

// CSFBParametersResponseCDMA2000-r8-IEs ::= SEQUENCE
SRSASN_CODE csfb_params_resp_cdma2000_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(rand.pack(bref));
  HANDLE_CODE(mob_params.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csfb_params_resp_cdma2000_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(rand.unpack(bref));
  HANDLE_CODE(mob_params.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csfb_params_resp_cdma2000_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rand", rand.to_string());
  j.write_str("mobilityParameters", mob_params.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CounterCheck-r8-IEs ::= SEQUENCE
SRSASN_CODE counter_check_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, drb_count_msb_info_list, 1, 11));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(drb_count_msb_info_list, bref, 1, 11));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void counter_check_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("drb-CountMSB-InfoList");
  for (const auto& e1 : drb_count_msb_info_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// DLInformationTransfer-r15-IEs ::= SEQUENCE
SRSASN_CODE dl_info_transfer_r15_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ded_info_type_r15_present, 1));
  HANDLE_CODE(bref.pack(time_ref_info_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ded_info_type_r15_present) {
    HANDLE_CODE(ded_info_type_r15.pack(bref));
  }
  if (time_ref_info_r15_present) {
    HANDLE_CODE(time_ref_info_r15.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_r15_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ded_info_type_r15_present, 1));
  HANDLE_CODE(bref.unpack(time_ref_info_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ded_info_type_r15_present) {
    HANDLE_CODE(ded_info_type_r15.unpack(bref));
  }
  if (time_ref_info_r15_present) {
    HANDLE_CODE(time_ref_info_r15.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dl_info_transfer_r15_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ded_info_type_r15_present) {
    j.write_fieldname("dedicatedInfoType-r15");
    ded_info_type_r15.to_json(j);
  }
  if (time_ref_info_r15_present) {
    j.write_fieldname("timeReferenceInfo-r15");
    time_ref_info_r15.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

void dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::destroy_()
{
  switch (type_) {
    case types::ded_info_nas_r15:
      c.destroy<dyn_octstring>();
      break;
    case types::ded_info_cdma2000_minus1_xrtt_r15:
      c.destroy<dyn_octstring>();
      break;
    case types::ded_info_cdma2000_hrpd_r15:
      c.destroy<dyn_octstring>();
      break;
    default:
      break;
  }
}
void dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ded_info_nas_r15:
      c.init<dyn_octstring>();
      break;
    case types::ded_info_cdma2000_minus1_xrtt_r15:
      c.init<dyn_octstring>();
      break;
    case types::ded_info_cdma2000_hrpd_r15:
      c.init<dyn_octstring>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_r15_ies_s::ded_info_type_r15_c_");
  }
}
dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::ded_info_type_r15_c_(
    const dl_info_transfer_r15_ies_s::ded_info_type_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ded_info_nas_r15:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::ded_info_cdma2000_minus1_xrtt_r15:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::ded_info_cdma2000_hrpd_r15:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_r15_ies_s::ded_info_type_r15_c_");
  }
}
dl_info_transfer_r15_ies_s::ded_info_type_r15_c_& dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::operator=(
    const dl_info_transfer_r15_ies_s::ded_info_type_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ded_info_nas_r15:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::ded_info_cdma2000_minus1_xrtt_r15:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::ded_info_cdma2000_hrpd_r15:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_r15_ies_s::ded_info_type_r15_c_");
  }

  return *this;
}
dyn_octstring& dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::set_ded_info_nas_r15()
{
  set(types::ded_info_nas_r15);
  return c.get<dyn_octstring>();
}
dyn_octstring& dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::set_ded_info_cdma2000_minus1_xrtt_r15()
{
  set(types::ded_info_cdma2000_minus1_xrtt_r15);
  return c.get<dyn_octstring>();
}
dyn_octstring& dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::set_ded_info_cdma2000_hrpd_r15()
{
  set(types::ded_info_cdma2000_hrpd_r15);
  return c.get<dyn_octstring>();
}
void dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ded_info_nas_r15:
      j.write_str("dedicatedInfoNAS-r15", c.get<dyn_octstring>().to_string());
      break;
    case types::ded_info_cdma2000_minus1_xrtt_r15:
      j.write_str("dedicatedInfoCDMA2000-1XRTT-r15", c.get<dyn_octstring>().to_string());
      break;
    case types::ded_info_cdma2000_hrpd_r15:
      j.write_str("dedicatedInfoCDMA2000-HRPD-r15", c.get<dyn_octstring>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_r15_ies_s::ded_info_type_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ded_info_nas_r15:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    case types::ded_info_cdma2000_minus1_xrtt_r15:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    case types::ded_info_cdma2000_hrpd_r15:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_r15_ies_s::ded_info_type_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ded_info_nas_r15:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    case types::ded_info_cdma2000_minus1_xrtt_r15:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    case types::ded_info_cdma2000_hrpd_r15:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_r15_ies_s::ded_info_type_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// DLInformationTransfer-r8-IEs ::= SEQUENCE
SRSASN_CODE dl_info_transfer_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ded_info_type.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ded_info_type.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dl_info_transfer_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("dedicatedInfoType");
  ded_info_type.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

void dl_info_transfer_r8_ies_s::ded_info_type_c_::destroy_()
{
  switch (type_) {
    case types::ded_info_nas:
      c.destroy<dyn_octstring>();
      break;
    case types::ded_info_cdma2000_minus1_xrtt:
      c.destroy<dyn_octstring>();
      break;
    case types::ded_info_cdma2000_hrpd:
      c.destroy<dyn_octstring>();
      break;
    default:
      break;
  }
}
void dl_info_transfer_r8_ies_s::ded_info_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ded_info_nas:
      c.init<dyn_octstring>();
      break;
    case types::ded_info_cdma2000_minus1_xrtt:
      c.init<dyn_octstring>();
      break;
    case types::ded_info_cdma2000_hrpd:
      c.init<dyn_octstring>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_r8_ies_s::ded_info_type_c_");
  }
}
dl_info_transfer_r8_ies_s::ded_info_type_c_::ded_info_type_c_(const dl_info_transfer_r8_ies_s::ded_info_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ded_info_nas:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::ded_info_cdma2000_minus1_xrtt:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::ded_info_cdma2000_hrpd:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_r8_ies_s::ded_info_type_c_");
  }
}
dl_info_transfer_r8_ies_s::ded_info_type_c_&
dl_info_transfer_r8_ies_s::ded_info_type_c_::operator=(const dl_info_transfer_r8_ies_s::ded_info_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ded_info_nas:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::ded_info_cdma2000_minus1_xrtt:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::ded_info_cdma2000_hrpd:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_r8_ies_s::ded_info_type_c_");
  }

  return *this;
}
dyn_octstring& dl_info_transfer_r8_ies_s::ded_info_type_c_::set_ded_info_nas()
{
  set(types::ded_info_nas);
  return c.get<dyn_octstring>();
}
dyn_octstring& dl_info_transfer_r8_ies_s::ded_info_type_c_::set_ded_info_cdma2000_minus1_xrtt()
{
  set(types::ded_info_cdma2000_minus1_xrtt);
  return c.get<dyn_octstring>();
}
dyn_octstring& dl_info_transfer_r8_ies_s::ded_info_type_c_::set_ded_info_cdma2000_hrpd()
{
  set(types::ded_info_cdma2000_hrpd);
  return c.get<dyn_octstring>();
}
void dl_info_transfer_r8_ies_s::ded_info_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ded_info_nas:
      j.write_str("dedicatedInfoNAS", c.get<dyn_octstring>().to_string());
      break;
    case types::ded_info_cdma2000_minus1_xrtt:
      j.write_str("dedicatedInfoCDMA2000-1XRTT", c.get<dyn_octstring>().to_string());
      break;
    case types::ded_info_cdma2000_hrpd:
      j.write_str("dedicatedInfoCDMA2000-HRPD", c.get<dyn_octstring>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_r8_ies_s::ded_info_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_info_transfer_r8_ies_s::ded_info_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ded_info_nas:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    case types::ded_info_cdma2000_minus1_xrtt:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    case types::ded_info_cdma2000_hrpd:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_r8_ies_s::ded_info_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_r8_ies_s::ded_info_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ded_info_nas:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    case types::ded_info_cdma2000_minus1_xrtt:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    case types::ded_info_cdma2000_hrpd:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_r8_ies_s::ded_info_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// HandoverFromEUTRAPreparationRequest-r8-IEs ::= SEQUENCE
SRSASN_CODE ho_from_eutra_prep_request_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rand_present, 1));
  HANDLE_CODE(bref.pack(mob_params_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(cdma2000_type.pack(bref));
  if (rand_present) {
    HANDLE_CODE(rand.pack(bref));
  }
  if (mob_params_present) {
    HANDLE_CODE(mob_params.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_from_eutra_prep_request_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rand_present, 1));
  HANDLE_CODE(bref.unpack(mob_params_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(cdma2000_type.unpack(bref));
  if (rand_present) {
    HANDLE_CODE(rand.unpack(bref));
  }
  if (mob_params_present) {
    HANDLE_CODE(mob_params.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_from_eutra_prep_request_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cdma2000-Type", cdma2000_type.to_string());
  if (rand_present) {
    j.write_str("rand", rand.to_string());
  }
  if (mob_params_present) {
    j.write_str("mobilityParameters", mob_params.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// LoggedMeasurementConfiguration-r10-IEs ::= SEQUENCE
SRSASN_CODE logged_meas_cfg_r10_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(area_cfg_r10_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(trace_ref_r10.pack(bref));
  HANDLE_CODE(trace_recording_session_ref_r10.pack(bref));
  HANDLE_CODE(tce_id_r10.pack(bref));
  HANDLE_CODE(absolute_time_info_r10.pack(bref));
  if (area_cfg_r10_present) {
    HANDLE_CODE(area_cfg_r10.pack(bref));
  }
  HANDLE_CODE(logging_dur_r10.pack(bref));
  HANDLE_CODE(logging_interv_r10.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_meas_cfg_r10_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(area_cfg_r10_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(trace_ref_r10.unpack(bref));
  HANDLE_CODE(trace_recording_session_ref_r10.unpack(bref));
  HANDLE_CODE(tce_id_r10.unpack(bref));
  HANDLE_CODE(absolute_time_info_r10.unpack(bref));
  if (area_cfg_r10_present) {
    HANDLE_CODE(area_cfg_r10.unpack(bref));
  }
  HANDLE_CODE(logging_dur_r10.unpack(bref));
  HANDLE_CODE(logging_interv_r10.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void logged_meas_cfg_r10_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("traceReference-r10");
  trace_ref_r10.to_json(j);
  j.write_str("traceRecordingSessionRef-r10", trace_recording_session_ref_r10.to_string());
  j.write_str("tce-Id-r10", tce_id_r10.to_string());
  j.write_str("absoluteTimeInfo-r10", absolute_time_info_r10.to_string());
  if (area_cfg_r10_present) {
    j.write_fieldname("areaConfiguration-r10");
    area_cfg_r10.to_json(j);
  }
  j.write_str("loggingDuration-r10", logging_dur_r10.to_string());
  j.write_str("loggingInterval-r10", logging_interv_r10.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MobilityFromEUTRACommand-r8-IEs ::= SEQUENCE
SRSASN_CODE mob_from_eutra_cmd_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.pack(cs_fallback_ind, 1));
  HANDLE_CODE(purpose.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_from_eutra_cmd_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.unpack(cs_fallback_ind, 1));
  HANDLE_CODE(purpose.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mob_from_eutra_cmd_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("cs-FallbackIndicator", cs_fallback_ind);
  j.write_fieldname("purpose");
  purpose.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

void mob_from_eutra_cmd_r8_ies_s::purpose_c_::destroy_()
{
  switch (type_) {
    case types::ho:
      c.destroy<ho_s>();
      break;
    case types::cell_change_order:
      c.destroy<cell_change_order_s>();
      break;
    default:
      break;
  }
}
void mob_from_eutra_cmd_r8_ies_s::purpose_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ho:
      c.init<ho_s>();
      break;
    case types::cell_change_order:
      c.init<cell_change_order_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_r8_ies_s::purpose_c_");
  }
}
mob_from_eutra_cmd_r8_ies_s::purpose_c_::purpose_c_(const mob_from_eutra_cmd_r8_ies_s::purpose_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ho:
      c.init(other.c.get<ho_s>());
      break;
    case types::cell_change_order:
      c.init(other.c.get<cell_change_order_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_r8_ies_s::purpose_c_");
  }
}
mob_from_eutra_cmd_r8_ies_s::purpose_c_&
mob_from_eutra_cmd_r8_ies_s::purpose_c_::operator=(const mob_from_eutra_cmd_r8_ies_s::purpose_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ho:
      c.set(other.c.get<ho_s>());
      break;
    case types::cell_change_order:
      c.set(other.c.get<cell_change_order_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_r8_ies_s::purpose_c_");
  }

  return *this;
}
ho_s& mob_from_eutra_cmd_r8_ies_s::purpose_c_::set_ho()
{
  set(types::ho);
  return c.get<ho_s>();
}
cell_change_order_s& mob_from_eutra_cmd_r8_ies_s::purpose_c_::set_cell_change_order()
{
  set(types::cell_change_order);
  return c.get<cell_change_order_s>();
}
void mob_from_eutra_cmd_r8_ies_s::purpose_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho:
      j.write_fieldname("handover");
      c.get<ho_s>().to_json(j);
      break;
    case types::cell_change_order:
      j.write_fieldname("cellChangeOrder");
      c.get<cell_change_order_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_r8_ies_s::purpose_c_");
  }
  j.end_obj();
}
SRSASN_CODE mob_from_eutra_cmd_r8_ies_s::purpose_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ho:
      HANDLE_CODE(c.get<ho_s>().pack(bref));
      break;
    case types::cell_change_order:
      HANDLE_CODE(c.get<cell_change_order_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_r8_ies_s::purpose_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_from_eutra_cmd_r8_ies_s::purpose_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ho:
      HANDLE_CODE(c.get<ho_s>().unpack(bref));
      break;
    case types::cell_change_order:
      HANDLE_CODE(c.get<cell_change_order_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_r8_ies_s::purpose_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MobilityFromEUTRACommand-r9-IEs ::= SEQUENCE
SRSASN_CODE mob_from_eutra_cmd_r9_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.pack(cs_fallback_ind, 1));
  HANDLE_CODE(purpose.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_from_eutra_cmd_r9_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.unpack(cs_fallback_ind, 1));
  HANDLE_CODE(purpose.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mob_from_eutra_cmd_r9_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("cs-FallbackIndicator", cs_fallback_ind);
  j.write_fieldname("purpose");
  purpose.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

void mob_from_eutra_cmd_r9_ies_s::purpose_c_::destroy_()
{
  switch (type_) {
    case types::ho:
      c.destroy<ho_s>();
      break;
    case types::cell_change_order:
      c.destroy<cell_change_order_s>();
      break;
    case types::e_csfb_r9:
      c.destroy<e_csfb_r9_s>();
      break;
    default:
      break;
  }
}
void mob_from_eutra_cmd_r9_ies_s::purpose_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ho:
      c.init<ho_s>();
      break;
    case types::cell_change_order:
      c.init<cell_change_order_s>();
      break;
    case types::e_csfb_r9:
      c.init<e_csfb_r9_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_r9_ies_s::purpose_c_");
  }
}
mob_from_eutra_cmd_r9_ies_s::purpose_c_::purpose_c_(const mob_from_eutra_cmd_r9_ies_s::purpose_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ho:
      c.init(other.c.get<ho_s>());
      break;
    case types::cell_change_order:
      c.init(other.c.get<cell_change_order_s>());
      break;
    case types::e_csfb_r9:
      c.init(other.c.get<e_csfb_r9_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_r9_ies_s::purpose_c_");
  }
}
mob_from_eutra_cmd_r9_ies_s::purpose_c_&
mob_from_eutra_cmd_r9_ies_s::purpose_c_::operator=(const mob_from_eutra_cmd_r9_ies_s::purpose_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ho:
      c.set(other.c.get<ho_s>());
      break;
    case types::cell_change_order:
      c.set(other.c.get<cell_change_order_s>());
      break;
    case types::e_csfb_r9:
      c.set(other.c.get<e_csfb_r9_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_r9_ies_s::purpose_c_");
  }

  return *this;
}
ho_s& mob_from_eutra_cmd_r9_ies_s::purpose_c_::set_ho()
{
  set(types::ho);
  return c.get<ho_s>();
}
cell_change_order_s& mob_from_eutra_cmd_r9_ies_s::purpose_c_::set_cell_change_order()
{
  set(types::cell_change_order);
  return c.get<cell_change_order_s>();
}
e_csfb_r9_s& mob_from_eutra_cmd_r9_ies_s::purpose_c_::set_e_csfb_r9()
{
  set(types::e_csfb_r9);
  return c.get<e_csfb_r9_s>();
}
void mob_from_eutra_cmd_r9_ies_s::purpose_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho:
      j.write_fieldname("handover");
      c.get<ho_s>().to_json(j);
      break;
    case types::cell_change_order:
      j.write_fieldname("cellChangeOrder");
      c.get<cell_change_order_s>().to_json(j);
      break;
    case types::e_csfb_r9:
      j.write_fieldname("e-CSFB-r9");
      c.get<e_csfb_r9_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_r9_ies_s::purpose_c_");
  }
  j.end_obj();
}
SRSASN_CODE mob_from_eutra_cmd_r9_ies_s::purpose_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ho:
      HANDLE_CODE(c.get<ho_s>().pack(bref));
      break;
    case types::cell_change_order:
      HANDLE_CODE(c.get<cell_change_order_s>().pack(bref));
      break;
    case types::e_csfb_r9:
      HANDLE_CODE(c.get<e_csfb_r9_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_r9_ies_s::purpose_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_from_eutra_cmd_r9_ies_s::purpose_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ho:
      HANDLE_CODE(c.get<ho_s>().unpack(bref));
      break;
    case types::cell_change_order:
      HANDLE_CODE(c.get<cell_change_order_s>().unpack(bref));
      break;
    case types::e_csfb_r9:
      HANDLE_CODE(c.get<e_csfb_r9_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_r9_ies_s::purpose_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RNReconfiguration-r10-IEs ::= SEQUENCE
SRSASN_CODE rn_recfg_r10_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rn_sys_info_r10_present, 1));
  HANDLE_CODE(bref.pack(rn_sf_cfg_r10_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rn_sys_info_r10_present) {
    HANDLE_CODE(rn_sys_info_r10.pack(bref));
  }
  if (rn_sf_cfg_r10_present) {
    HANDLE_CODE(rn_sf_cfg_r10.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_recfg_r10_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rn_sys_info_r10_present, 1));
  HANDLE_CODE(bref.unpack(rn_sf_cfg_r10_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rn_sys_info_r10_present) {
    HANDLE_CODE(rn_sys_info_r10.unpack(bref));
  }
  if (rn_sf_cfg_r10_present) {
    HANDLE_CODE(rn_sf_cfg_r10.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rn_recfg_r10_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rn_sys_info_r10_present) {
    j.write_fieldname("rn-SystemInfo-r10");
    rn_sys_info_r10.to_json(j);
  }
  if (rn_sf_cfg_r10_present) {
    j.write_fieldname("rn-SubframeConfig-r10");
    rn_sf_cfg_r10.to_json(j);
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

// RRCConnectionReconfiguration-r8-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_cfg_present, 1));
  HANDLE_CODE(bref.pack(mob_ctrl_info_present, 1));
  HANDLE_CODE(bref.pack(ded_info_nas_list_present, 1));
  HANDLE_CODE(bref.pack(rr_cfg_ded_present, 1));
  HANDLE_CODE(bref.pack(security_cfg_ho_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_cfg_present) {
    HANDLE_CODE(meas_cfg.pack(bref));
  }
  if (mob_ctrl_info_present) {
    HANDLE_CODE(mob_ctrl_info.pack(bref));
  }
  if (ded_info_nas_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ded_info_nas_list, 1, 11));
  }
  if (rr_cfg_ded_present) {
    HANDLE_CODE(rr_cfg_ded.pack(bref));
  }
  if (security_cfg_ho_present) {
    HANDLE_CODE(security_cfg_ho.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_cfg_present, 1));
  HANDLE_CODE(bref.unpack(mob_ctrl_info_present, 1));
  HANDLE_CODE(bref.unpack(ded_info_nas_list_present, 1));
  HANDLE_CODE(bref.unpack(rr_cfg_ded_present, 1));
  HANDLE_CODE(bref.unpack(security_cfg_ho_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_cfg_present) {
    HANDLE_CODE(meas_cfg.unpack(bref));
  }
  if (mob_ctrl_info_present) {
    HANDLE_CODE(mob_ctrl_info.unpack(bref));
  }
  if (ded_info_nas_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ded_info_nas_list, bref, 1, 11));
  }
  if (rr_cfg_ded_present) {
    HANDLE_CODE(rr_cfg_ded.unpack(bref));
  }
  if (security_cfg_ho_present) {
    HANDLE_CODE(security_cfg_ho.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_cfg_present) {
    j.write_fieldname("measConfig");
    meas_cfg.to_json(j);
  }
  if (mob_ctrl_info_present) {
    j.write_fieldname("mobilityControlInfo");
    mob_ctrl_info.to_json(j);
  }
  if (ded_info_nas_list_present) {
    j.start_array("dedicatedInfoNASList");
    for (const auto& e1 : ded_info_nas_list) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (rr_cfg_ded_present) {
    j.write_fieldname("radioResourceConfigDedicated");
    rr_cfg_ded.to_json(j);
  }
  if (security_cfg_ho_present) {
    j.write_fieldname("securityConfigHO");
    security_cfg_ho.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionRelease-r8-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_release_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(redirected_carrier_info_present, 1));
  HANDLE_CODE(bref.pack(idle_mode_mob_ctrl_info_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(release_cause.pack(bref));
  if (redirected_carrier_info_present) {
    HANDLE_CODE(redirected_carrier_info.pack(bref));
  }
  if (idle_mode_mob_ctrl_info_present) {
    HANDLE_CODE(idle_mode_mob_ctrl_info.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(redirected_carrier_info_present, 1));
  HANDLE_CODE(bref.unpack(idle_mode_mob_ctrl_info_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(release_cause.unpack(bref));
  if (redirected_carrier_info_present) {
    HANDLE_CODE(redirected_carrier_info.unpack(bref));
  }
  if (idle_mode_mob_ctrl_info_present) {
    HANDLE_CODE(idle_mode_mob_ctrl_info.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_release_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("releaseCause", release_cause.to_string());
  if (redirected_carrier_info_present) {
    j.write_fieldname("redirectedCarrierInfo");
    redirected_carrier_info.to_json(j);
  }
  if (idle_mode_mob_ctrl_info_present) {
    j.write_fieldname("idleModeMobilityControlInfo");
    idle_mode_mob_ctrl_info.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionResume-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rr_cfg_ded_r13_present, 1));
  HANDLE_CODE(bref.pack(meas_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(ant_info_ded_pcell_r13_present, 1));
  HANDLE_CODE(bref.pack(drb_continue_rohc_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(rrc_conn_resume_v1430_ies_present, 1));

  if (rr_cfg_ded_r13_present) {
    HANDLE_CODE(rr_cfg_ded_r13.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, next_hop_chaining_count_r13, (uint8_t)0u, (uint8_t)7u));
  if (meas_cfg_r13_present) {
    HANDLE_CODE(meas_cfg_r13.pack(bref));
  }
  if (ant_info_ded_pcell_r13_present) {
    HANDLE_CODE(ant_info_ded_pcell_r13.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (rrc_conn_resume_v1430_ies_present) {
    HANDLE_CODE(rrc_conn_resume_v1430_ies.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rr_cfg_ded_r13_present, 1));
  HANDLE_CODE(bref.unpack(meas_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(ant_info_ded_pcell_r13_present, 1));
  HANDLE_CODE(bref.unpack(drb_continue_rohc_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(rrc_conn_resume_v1430_ies_present, 1));

  if (rr_cfg_ded_r13_present) {
    HANDLE_CODE(rr_cfg_ded_r13.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(next_hop_chaining_count_r13, bref, (uint8_t)0u, (uint8_t)7u));
  if (meas_cfg_r13_present) {
    HANDLE_CODE(meas_cfg_r13.unpack(bref));
  }
  if (ant_info_ded_pcell_r13_present) {
    HANDLE_CODE(ant_info_ded_pcell_r13.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (rrc_conn_resume_v1430_ies_present) {
    HANDLE_CODE(rrc_conn_resume_v1430_ies.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rr_cfg_ded_r13_present) {
    j.write_fieldname("radioResourceConfigDedicated-r13");
    rr_cfg_ded_r13.to_json(j);
  }
  j.write_int("nextHopChainingCount-r13", next_hop_chaining_count_r13);
  if (meas_cfg_r13_present) {
    j.write_fieldname("measConfig-r13");
    meas_cfg_r13.to_json(j);
  }
  if (ant_info_ded_pcell_r13_present) {
    j.write_fieldname("antennaInfoDedicatedPCell-r13");
    ant_info_ded_pcell_r13.to_json(j);
  }
  if (drb_continue_rohc_r13_present) {
    j.write_str("drb-ContinueROHC-r13", "true");
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (rrc_conn_resume_v1430_ies_present) {
    j.write_fieldname("rrcConnectionResume-v1430-IEs");
    rrc_conn_resume_v1430_ies.to_json(j);
  }
  j.end_obj();
}

// UEInformationRequest-r9-IEs ::= SEQUENCE
SRSASN_CODE ue_info_request_r9_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.pack(rach_report_req_r9, 1));
  HANDLE_CODE(bref.pack(rlf_report_req_r9, 1));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_request_r9_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.unpack(rach_report_req_r9, 1));
  HANDLE_CODE(bref.unpack(rlf_report_req_r9, 1));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_info_request_r9_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("rach-ReportReq-r9", rach_report_req_r9);
  j.write_bool("rlf-ReportReq-r9", rlf_report_req_r9);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CSFBParametersResponseCDMA2000 ::= SEQUENCE
SRSASN_CODE csfb_params_resp_cdma2000_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csfb_params_resp_cdma2000_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void csfb_params_resp_cdma2000_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void csfb_params_resp_cdma2000_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
csfb_params_resp_cdma2000_r8_ies_s& csfb_params_resp_cdma2000_s::crit_exts_c_::set_csfb_params_resp_cdma2000_r8()
{
  set(types::csfb_params_resp_cdma2000_r8);
  return c;
}
void csfb_params_resp_cdma2000_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void csfb_params_resp_cdma2000_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::csfb_params_resp_cdma2000_r8:
      j.write_fieldname("csfbParametersResponseCDMA2000-r8");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "csfb_params_resp_cdma2000_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE csfb_params_resp_cdma2000_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::csfb_params_resp_cdma2000_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "csfb_params_resp_cdma2000_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csfb_params_resp_cdma2000_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::csfb_params_resp_cdma2000_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "csfb_params_resp_cdma2000_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CounterCheck ::= SEQUENCE
SRSASN_CODE counter_check_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void counter_check_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void counter_check_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
counter_check_s::crit_exts_c_::c1_c_& counter_check_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void counter_check_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void counter_check_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "counter_check_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE counter_check_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "counter_check_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void counter_check_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
counter_check_r8_ies_s& counter_check_s::crit_exts_c_::c1_c_::set_counter_check_r8()
{
  set(types::counter_check_r8);
  return c;
}
void counter_check_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void counter_check_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void counter_check_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void counter_check_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::counter_check_r8:
      j.write_fieldname("counterCheck-r8");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE counter_check_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::counter_check_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::counter_check_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// DLInformationTransfer ::= SEQUENCE
SRSASN_CODE dl_info_transfer_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void dl_info_transfer_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void dl_info_transfer_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
dl_info_transfer_s::crit_exts_c_::c1_c_& dl_info_transfer_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void dl_info_transfer_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void dl_info_transfer_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "dl_info_transfer_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_info_transfer_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "dl_info_transfer_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void dl_info_transfer_s::crit_exts_c_::c1_c_::destroy_()
{
  switch (type_) {
    case types::dl_info_transfer_r8:
      c.destroy<dl_info_transfer_r8_ies_s>();
      break;
    case types::dl_info_transfer_r15:
      c.destroy<dl_info_transfer_r15_ies_s>();
      break;
    default:
      break;
  }
}
void dl_info_transfer_s::crit_exts_c_::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::dl_info_transfer_r8:
      c.init<dl_info_transfer_r8_ies_s>();
      break;
    case types::dl_info_transfer_r15:
      c.init<dl_info_transfer_r15_ies_s>();
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_s::crit_exts_c_::c1_c_");
  }
}
dl_info_transfer_s::crit_exts_c_::c1_c_::c1_c_(const dl_info_transfer_s::crit_exts_c_::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::dl_info_transfer_r8:
      c.init(other.c.get<dl_info_transfer_r8_ies_s>());
      break;
    case types::dl_info_transfer_r15:
      c.init(other.c.get<dl_info_transfer_r15_ies_s>());
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_s::crit_exts_c_::c1_c_");
  }
}
dl_info_transfer_s::crit_exts_c_::c1_c_&
dl_info_transfer_s::crit_exts_c_::c1_c_::operator=(const dl_info_transfer_s::crit_exts_c_::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::dl_info_transfer_r8:
      c.set(other.c.get<dl_info_transfer_r8_ies_s>());
      break;
    case types::dl_info_transfer_r15:
      c.set(other.c.get<dl_info_transfer_r15_ies_s>());
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_s::crit_exts_c_::c1_c_");
  }

  return *this;
}
dl_info_transfer_r8_ies_s& dl_info_transfer_s::crit_exts_c_::c1_c_::set_dl_info_transfer_r8()
{
  set(types::dl_info_transfer_r8);
  return c.get<dl_info_transfer_r8_ies_s>();
}
dl_info_transfer_r15_ies_s& dl_info_transfer_s::crit_exts_c_::c1_c_::set_dl_info_transfer_r15()
{
  set(types::dl_info_transfer_r15);
  return c.get<dl_info_transfer_r15_ies_s>();
}
void dl_info_transfer_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void dl_info_transfer_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void dl_info_transfer_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::dl_info_transfer_r8:
      j.write_fieldname("dlInformationTransfer-r8");
      c.get<dl_info_transfer_r8_ies_s>().to_json(j);
      break;
    case types::dl_info_transfer_r15:
      j.write_fieldname("dlInformationTransfer-r15");
      c.get<dl_info_transfer_r15_ies_s>().to_json(j);
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_info_transfer_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::dl_info_transfer_r8:
      HANDLE_CODE(c.get<dl_info_transfer_r8_ies_s>().pack(bref));
      break;
    case types::dl_info_transfer_r15:
      HANDLE_CODE(c.get<dl_info_transfer_r15_ies_s>().pack(bref));
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::dl_info_transfer_r8:
      HANDLE_CODE(c.get<dl_info_transfer_r8_ies_s>().unpack(bref));
      break;
    case types::dl_info_transfer_r15:
      HANDLE_CODE(c.get<dl_info_transfer_r15_ies_s>().unpack(bref));
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// HandoverFromEUTRAPreparationRequest ::= SEQUENCE
SRSASN_CODE ho_from_eutra_prep_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_from_eutra_prep_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ho_from_eutra_prep_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ho_from_eutra_prep_request_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_& ho_from_eutra_prep_request_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ho_from_eutra_prep_request_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ho_from_eutra_prep_request_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ho_from_eutra_prep_request_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_from_eutra_prep_request_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ho_from_eutra_prep_request_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_from_eutra_prep_request_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ho_from_eutra_prep_request_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ho_from_eutra_prep_request_r8_ies_s&
ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_::set_ho_from_eutra_prep_request_r8()
{
  set(types::ho_from_eutra_prep_request_r8);
  return c;
}
void ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho_from_eutra_prep_request_r8:
      j.write_fieldname("handoverFromEUTRAPreparationRequest-r8");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ho_from_eutra_prep_request_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ho_from_eutra_prep_request_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// LoggedMeasurementConfiguration-r10 ::= SEQUENCE
SRSASN_CODE logged_meas_cfg_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_meas_cfg_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void logged_meas_cfg_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void logged_meas_cfg_r10_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
logged_meas_cfg_r10_s::crit_exts_c_::c1_c_& logged_meas_cfg_r10_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void logged_meas_cfg_r10_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void logged_meas_cfg_r10_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "logged_meas_cfg_r10_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE logged_meas_cfg_r10_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "logged_meas_cfg_r10_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_meas_cfg_r10_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "logged_meas_cfg_r10_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void logged_meas_cfg_r10_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
logged_meas_cfg_r10_ies_s& logged_meas_cfg_r10_s::crit_exts_c_::c1_c_::set_logged_meas_cfg_r10()
{
  set(types::logged_meas_cfg_r10);
  return c;
}
void logged_meas_cfg_r10_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void logged_meas_cfg_r10_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void logged_meas_cfg_r10_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void logged_meas_cfg_r10_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::logged_meas_cfg_r10:
      j.write_fieldname("loggedMeasurementConfiguration-r10");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "logged_meas_cfg_r10_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE logged_meas_cfg_r10_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::logged_meas_cfg_r10:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "logged_meas_cfg_r10_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_meas_cfg_r10_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::logged_meas_cfg_r10:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "logged_meas_cfg_r10_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MobilityFromEUTRACommand ::= SEQUENCE
SRSASN_CODE mob_from_eutra_cmd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_from_eutra_cmd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void mob_from_eutra_cmd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void mob_from_eutra_cmd_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
mob_from_eutra_cmd_s::crit_exts_c_::c1_c_& mob_from_eutra_cmd_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void mob_from_eutra_cmd_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void mob_from_eutra_cmd_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE mob_from_eutra_cmd_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_from_eutra_cmd_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::destroy_()
{
  switch (type_) {
    case types::mob_from_eutra_cmd_r8:
      c.destroy<mob_from_eutra_cmd_r8_ies_s>();
      break;
    case types::mob_from_eutra_cmd_r9:
      c.destroy<mob_from_eutra_cmd_r9_ies_s>();
      break;
    default:
      break;
  }
}
void mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::mob_from_eutra_cmd_r8:
      c.init<mob_from_eutra_cmd_r8_ies_s>();
      break;
    case types::mob_from_eutra_cmd_r9:
      c.init<mob_from_eutra_cmd_r9_ies_s>();
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_s::crit_exts_c_::c1_c_");
  }
}
mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::c1_c_(const mob_from_eutra_cmd_s::crit_exts_c_::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::mob_from_eutra_cmd_r8:
      c.init(other.c.get<mob_from_eutra_cmd_r8_ies_s>());
      break;
    case types::mob_from_eutra_cmd_r9:
      c.init(other.c.get<mob_from_eutra_cmd_r9_ies_s>());
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_s::crit_exts_c_::c1_c_");
  }
}
mob_from_eutra_cmd_s::crit_exts_c_::c1_c_&
mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::operator=(const mob_from_eutra_cmd_s::crit_exts_c_::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::mob_from_eutra_cmd_r8:
      c.set(other.c.get<mob_from_eutra_cmd_r8_ies_s>());
      break;
    case types::mob_from_eutra_cmd_r9:
      c.set(other.c.get<mob_from_eutra_cmd_r9_ies_s>());
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_s::crit_exts_c_::c1_c_");
  }

  return *this;
}
mob_from_eutra_cmd_r8_ies_s& mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::set_mob_from_eutra_cmd_r8()
{
  set(types::mob_from_eutra_cmd_r8);
  return c.get<mob_from_eutra_cmd_r8_ies_s>();
}
mob_from_eutra_cmd_r9_ies_s& mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::set_mob_from_eutra_cmd_r9()
{
  set(types::mob_from_eutra_cmd_r9);
  return c.get<mob_from_eutra_cmd_r9_ies_s>();
}
void mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::mob_from_eutra_cmd_r8:
      j.write_fieldname("mobilityFromEUTRACommand-r8");
      c.get<mob_from_eutra_cmd_r8_ies_s>().to_json(j);
      break;
    case types::mob_from_eutra_cmd_r9:
      j.write_fieldname("mobilityFromEUTRACommand-r9");
      c.get<mob_from_eutra_cmd_r9_ies_s>().to_json(j);
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::mob_from_eutra_cmd_r8:
      HANDLE_CODE(c.get<mob_from_eutra_cmd_r8_ies_s>().pack(bref));
      break;
    case types::mob_from_eutra_cmd_r9:
      HANDLE_CODE(c.get<mob_from_eutra_cmd_r9_ies_s>().pack(bref));
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::mob_from_eutra_cmd_r8:
      HANDLE_CODE(c.get<mob_from_eutra_cmd_r8_ies_s>().unpack(bref));
      break;
    case types::mob_from_eutra_cmd_r9:
      HANDLE_CODE(c.get<mob_from_eutra_cmd_r9_ies_s>().unpack(bref));
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_eutra_cmd_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RNReconfiguration-r10 ::= SEQUENCE
SRSASN_CODE rn_recfg_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_recfg_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rn_recfg_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rn_recfg_r10_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rn_recfg_r10_s::crit_exts_c_::c1_c_& rn_recfg_r10_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void rn_recfg_r10_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rn_recfg_r10_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "rn_recfg_r10_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rn_recfg_r10_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rn_recfg_r10_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_recfg_r10_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "rn_recfg_r10_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rn_recfg_r10_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
rn_recfg_r10_ies_s& rn_recfg_r10_s::crit_exts_c_::c1_c_::set_rn_recfg_r10()
{
  set(types::rn_recfg_r10);
  return c;
}
void rn_recfg_r10_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void rn_recfg_r10_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void rn_recfg_r10_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void rn_recfg_r10_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rn_recfg_r10:
      j.write_fieldname("rnReconfiguration-r10");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rn_recfg_r10_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rn_recfg_r10_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rn_recfg_r10:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rn_recfg_r10_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_recfg_r10_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rn_recfg_r10:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rn_recfg_r10_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionReconfiguration ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_recfg_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_recfg_s::crit_exts_c_::c1_c_& rrc_conn_recfg_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void rrc_conn_recfg_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_conn_recfg_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "rrc_conn_recfg_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_recfg_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "rrc_conn_recfg_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_conn_recfg_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_recfg_r8_ies_s& rrc_conn_recfg_s::crit_exts_c_::c1_c_::set_rrc_conn_recfg_r8()
{
  set(types::rrc_conn_recfg_r8);
  return c;
}
void rrc_conn_recfg_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void rrc_conn_recfg_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void rrc_conn_recfg_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void rrc_conn_recfg_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void rrc_conn_recfg_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void rrc_conn_recfg_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void rrc_conn_recfg_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void rrc_conn_recfg_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_recfg_r8:
      j.write_fieldname("rrcConnectionReconfiguration-r8");
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
      log_invalid_choice_id(type_, "rrc_conn_recfg_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_recfg_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_recfg_r8:
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
      log_invalid_choice_id(type_, "rrc_conn_recfg_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_recfg_r8:
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
      log_invalid_choice_id(type_, "rrc_conn_recfg_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionRelease ::= SEQUENCE
SRSASN_CODE rrc_conn_release_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_release_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_release_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_release_s::crit_exts_c_::c1_c_& rrc_conn_release_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void rrc_conn_release_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_conn_release_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "rrc_conn_release_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_release_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "rrc_conn_release_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_conn_release_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_release_r8_ies_s& rrc_conn_release_s::crit_exts_c_::c1_c_::set_rrc_conn_release_r8()
{
  set(types::rrc_conn_release_r8);
  return c;
}
void rrc_conn_release_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void rrc_conn_release_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void rrc_conn_release_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void rrc_conn_release_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_release_r8:
      j.write_fieldname("rrcConnectionRelease-r8");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_release_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_release_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_release_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_release_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionResume-r13 ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_resume_r13_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_resume_r13_s::crit_exts_c_::c1_c_& rrc_conn_resume_r13_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void rrc_conn_resume_r13_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_conn_resume_r13_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "rrc_conn_resume_r13_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_resume_r13_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_r13_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_r13_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "rrc_conn_resume_r13_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_conn_resume_r13_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_resume_r13_ies_s& rrc_conn_resume_r13_s::crit_exts_c_::c1_c_::set_rrc_conn_resume_r13()
{
  set(types::rrc_conn_resume_r13);
  return c;
}
void rrc_conn_resume_r13_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void rrc_conn_resume_r13_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void rrc_conn_resume_r13_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void rrc_conn_resume_r13_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_resume_r13:
      j.write_fieldname("rrcConnectionResume-r13");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_r13_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_resume_r13_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_resume_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_r13_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_r13_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_resume_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_r13_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEInformationRequest-r9 ::= SEQUENCE
SRSASN_CODE ue_info_request_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_request_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_info_request_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_info_request_r9_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_info_request_r9_s::crit_exts_c_::c1_c_& ue_info_request_r9_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ue_info_request_r9_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_info_request_r9_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ue_info_request_r9_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_info_request_r9_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_info_request_r9_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_request_r9_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ue_info_request_r9_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_info_request_r9_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ue_info_request_r9_ies_s& ue_info_request_r9_s::crit_exts_c_::c1_c_::set_ue_info_request_r9()
{
  set(types::ue_info_request_r9);
  return c;
}
void ue_info_request_r9_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ue_info_request_r9_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ue_info_request_r9_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ue_info_request_r9_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_info_request_r9:
      j.write_fieldname("ueInformationRequest-r9");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_info_request_r9_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_info_request_r9_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_info_request_r9:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_info_request_r9_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_request_r9_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_info_request_r9:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_info_request_r9_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// DL-DCCH-MessageType ::= CHOICE
void dl_dcch_msg_type_c::set(types::options e)
{
  type_ = e;
}
dl_dcch_msg_type_c::c1_c_& dl_dcch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void dl_dcch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void dl_dcch_msg_type_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE dl_dcch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_dcch_msg_type_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void dl_dcch_msg_type_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::csfb_params_resp_cdma2000:
      c.destroy<csfb_params_resp_cdma2000_s>();
      break;
    case types::dl_info_transfer:
      c.destroy<dl_info_transfer_s>();
      break;
    case types::ho_from_eutra_prep_request:
      c.destroy<ho_from_eutra_prep_request_s>();
      break;
    case types::mob_from_eutra_cmd:
      c.destroy<mob_from_eutra_cmd_s>();
      break;
    case types::rrc_conn_recfg:
      c.destroy<rrc_conn_recfg_s>();
      break;
    case types::rrc_conn_release:
      c.destroy<rrc_conn_release_s>();
      break;
    case types::security_mode_cmd:
      c.destroy<security_mode_cmd_s>();
      break;
    case types::ue_cap_enquiry:
      c.destroy<ue_cap_enquiry_s>();
      break;
    case types::counter_check:
      c.destroy<counter_check_s>();
      break;
    case types::ue_info_request_r9:
      c.destroy<ue_info_request_r9_s>();
      break;
    case types::logged_meas_cfg_r10:
      c.destroy<logged_meas_cfg_r10_s>();
      break;
    case types::rn_recfg_r10:
      c.destroy<rn_recfg_r10_s>();
      break;
    case types::rrc_conn_resume_r13:
      c.destroy<rrc_conn_resume_r13_s>();
      break;
    default:
      break;
  }
}
void dl_dcch_msg_type_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::csfb_params_resp_cdma2000:
      c.init<csfb_params_resp_cdma2000_s>();
      break;
    case types::dl_info_transfer:
      c.init<dl_info_transfer_s>();
      break;
    case types::ho_from_eutra_prep_request:
      c.init<ho_from_eutra_prep_request_s>();
      break;
    case types::mob_from_eutra_cmd:
      c.init<mob_from_eutra_cmd_s>();
      break;
    case types::rrc_conn_recfg:
      c.init<rrc_conn_recfg_s>();
      break;
    case types::rrc_conn_release:
      c.init<rrc_conn_release_s>();
      break;
    case types::security_mode_cmd:
      c.init<security_mode_cmd_s>();
      break;
    case types::ue_cap_enquiry:
      c.init<ue_cap_enquiry_s>();
      break;
    case types::counter_check:
      c.init<counter_check_s>();
      break;
    case types::ue_info_request_r9:
      c.init<ue_info_request_r9_s>();
      break;
    case types::logged_meas_cfg_r10:
      c.init<logged_meas_cfg_r10_s>();
      break;
    case types::rn_recfg_r10:
      c.init<rn_recfg_r10_s>();
      break;
    case types::rrc_conn_resume_r13:
      c.init<rrc_conn_resume_r13_s>();
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
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c::c1_c_");
  }
}
dl_dcch_msg_type_c::c1_c_::c1_c_(const dl_dcch_msg_type_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::csfb_params_resp_cdma2000:
      c.init(other.c.get<csfb_params_resp_cdma2000_s>());
      break;
    case types::dl_info_transfer:
      c.init(other.c.get<dl_info_transfer_s>());
      break;
    case types::ho_from_eutra_prep_request:
      c.init(other.c.get<ho_from_eutra_prep_request_s>());
      break;
    case types::mob_from_eutra_cmd:
      c.init(other.c.get<mob_from_eutra_cmd_s>());
      break;
    case types::rrc_conn_recfg:
      c.init(other.c.get<rrc_conn_recfg_s>());
      break;
    case types::rrc_conn_release:
      c.init(other.c.get<rrc_conn_release_s>());
      break;
    case types::security_mode_cmd:
      c.init(other.c.get<security_mode_cmd_s>());
      break;
    case types::ue_cap_enquiry:
      c.init(other.c.get<ue_cap_enquiry_s>());
      break;
    case types::counter_check:
      c.init(other.c.get<counter_check_s>());
      break;
    case types::ue_info_request_r9:
      c.init(other.c.get<ue_info_request_r9_s>());
      break;
    case types::logged_meas_cfg_r10:
      c.init(other.c.get<logged_meas_cfg_r10_s>());
      break;
    case types::rn_recfg_r10:
      c.init(other.c.get<rn_recfg_r10_s>());
      break;
    case types::rrc_conn_resume_r13:
      c.init(other.c.get<rrc_conn_resume_r13_s>());
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
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c::c1_c_");
  }
}
dl_dcch_msg_type_c::c1_c_& dl_dcch_msg_type_c::c1_c_::operator=(const dl_dcch_msg_type_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::csfb_params_resp_cdma2000:
      c.set(other.c.get<csfb_params_resp_cdma2000_s>());
      break;
    case types::dl_info_transfer:
      c.set(other.c.get<dl_info_transfer_s>());
      break;
    case types::ho_from_eutra_prep_request:
      c.set(other.c.get<ho_from_eutra_prep_request_s>());
      break;
    case types::mob_from_eutra_cmd:
      c.set(other.c.get<mob_from_eutra_cmd_s>());
      break;
    case types::rrc_conn_recfg:
      c.set(other.c.get<rrc_conn_recfg_s>());
      break;
    case types::rrc_conn_release:
      c.set(other.c.get<rrc_conn_release_s>());
      break;
    case types::security_mode_cmd:
      c.set(other.c.get<security_mode_cmd_s>());
      break;
    case types::ue_cap_enquiry:
      c.set(other.c.get<ue_cap_enquiry_s>());
      break;
    case types::counter_check:
      c.set(other.c.get<counter_check_s>());
      break;
    case types::ue_info_request_r9:
      c.set(other.c.get<ue_info_request_r9_s>());
      break;
    case types::logged_meas_cfg_r10:
      c.set(other.c.get<logged_meas_cfg_r10_s>());
      break;
    case types::rn_recfg_r10:
      c.set(other.c.get<rn_recfg_r10_s>());
      break;
    case types::rrc_conn_resume_r13:
      c.set(other.c.get<rrc_conn_resume_r13_s>());
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
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c::c1_c_");
  }

  return *this;
}
csfb_params_resp_cdma2000_s& dl_dcch_msg_type_c::c1_c_::set_csfb_params_resp_cdma2000()
{
  set(types::csfb_params_resp_cdma2000);
  return c.get<csfb_params_resp_cdma2000_s>();
}
dl_info_transfer_s& dl_dcch_msg_type_c::c1_c_::set_dl_info_transfer()
{
  set(types::dl_info_transfer);
  return c.get<dl_info_transfer_s>();
}
ho_from_eutra_prep_request_s& dl_dcch_msg_type_c::c1_c_::set_ho_from_eutra_prep_request()
{
  set(types::ho_from_eutra_prep_request);
  return c.get<ho_from_eutra_prep_request_s>();
}
mob_from_eutra_cmd_s& dl_dcch_msg_type_c::c1_c_::set_mob_from_eutra_cmd()
{
  set(types::mob_from_eutra_cmd);
  return c.get<mob_from_eutra_cmd_s>();
}
rrc_conn_recfg_s& dl_dcch_msg_type_c::c1_c_::set_rrc_conn_recfg()
{
  set(types::rrc_conn_recfg);
  return c.get<rrc_conn_recfg_s>();
}
rrc_conn_release_s& dl_dcch_msg_type_c::c1_c_::set_rrc_conn_release()
{
  set(types::rrc_conn_release);
  return c.get<rrc_conn_release_s>();
}
security_mode_cmd_s& dl_dcch_msg_type_c::c1_c_::set_security_mode_cmd()
{
  set(types::security_mode_cmd);
  return c.get<security_mode_cmd_s>();
}
ue_cap_enquiry_s& dl_dcch_msg_type_c::c1_c_::set_ue_cap_enquiry()
{
  set(types::ue_cap_enquiry);
  return c.get<ue_cap_enquiry_s>();
}
counter_check_s& dl_dcch_msg_type_c::c1_c_::set_counter_check()
{
  set(types::counter_check);
  return c.get<counter_check_s>();
}
ue_info_request_r9_s& dl_dcch_msg_type_c::c1_c_::set_ue_info_request_r9()
{
  set(types::ue_info_request_r9);
  return c.get<ue_info_request_r9_s>();
}
logged_meas_cfg_r10_s& dl_dcch_msg_type_c::c1_c_::set_logged_meas_cfg_r10()
{
  set(types::logged_meas_cfg_r10);
  return c.get<logged_meas_cfg_r10_s>();
}
rn_recfg_r10_s& dl_dcch_msg_type_c::c1_c_::set_rn_recfg_r10()
{
  set(types::rn_recfg_r10);
  return c.get<rn_recfg_r10_s>();
}
rrc_conn_resume_r13_s& dl_dcch_msg_type_c::c1_c_::set_rrc_conn_resume_r13()
{
  set(types::rrc_conn_resume_r13);
  return c.get<rrc_conn_resume_r13_s>();
}
void dl_dcch_msg_type_c::c1_c_::set_spare3()
{
  set(types::spare3);
}
void dl_dcch_msg_type_c::c1_c_::set_spare2()
{
  set(types::spare2);
}
void dl_dcch_msg_type_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
void dl_dcch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::csfb_params_resp_cdma2000:
      j.write_fieldname("csfbParametersResponseCDMA2000");
      c.get<csfb_params_resp_cdma2000_s>().to_json(j);
      break;
    case types::dl_info_transfer:
      j.write_fieldname("dlInformationTransfer");
      c.get<dl_info_transfer_s>().to_json(j);
      break;
    case types::ho_from_eutra_prep_request:
      j.write_fieldname("handoverFromEUTRAPreparationRequest");
      c.get<ho_from_eutra_prep_request_s>().to_json(j);
      break;
    case types::mob_from_eutra_cmd:
      j.write_fieldname("mobilityFromEUTRACommand");
      c.get<mob_from_eutra_cmd_s>().to_json(j);
      break;
    case types::rrc_conn_recfg:
      j.write_fieldname("rrcConnectionReconfiguration");
      c.get<rrc_conn_recfg_s>().to_json(j);
      break;
    case types::rrc_conn_release:
      j.write_fieldname("rrcConnectionRelease");
      c.get<rrc_conn_release_s>().to_json(j);
      break;
    case types::security_mode_cmd:
      j.write_fieldname("securityModeCommand");
      c.get<security_mode_cmd_s>().to_json(j);
      break;
    case types::ue_cap_enquiry:
      j.write_fieldname("ueCapabilityEnquiry");
      c.get<ue_cap_enquiry_s>().to_json(j);
      break;
    case types::counter_check:
      j.write_fieldname("counterCheck");
      c.get<counter_check_s>().to_json(j);
      break;
    case types::ue_info_request_r9:
      j.write_fieldname("ueInformationRequest-r9");
      c.get<ue_info_request_r9_s>().to_json(j);
      break;
    case types::logged_meas_cfg_r10:
      j.write_fieldname("loggedMeasurementConfiguration-r10");
      c.get<logged_meas_cfg_r10_s>().to_json(j);
      break;
    case types::rn_recfg_r10:
      j.write_fieldname("rnReconfiguration-r10");
      c.get<rn_recfg_r10_s>().to_json(j);
      break;
    case types::rrc_conn_resume_r13:
      j.write_fieldname("rrcConnectionResume-r13");
      c.get<rrc_conn_resume_r13_s>().to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_dcch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::csfb_params_resp_cdma2000:
      HANDLE_CODE(c.get<csfb_params_resp_cdma2000_s>().pack(bref));
      break;
    case types::dl_info_transfer:
      HANDLE_CODE(c.get<dl_info_transfer_s>().pack(bref));
      break;
    case types::ho_from_eutra_prep_request:
      HANDLE_CODE(c.get<ho_from_eutra_prep_request_s>().pack(bref));
      break;
    case types::mob_from_eutra_cmd:
      HANDLE_CODE(c.get<mob_from_eutra_cmd_s>().pack(bref));
      break;
    case types::rrc_conn_recfg:
      HANDLE_CODE(c.get<rrc_conn_recfg_s>().pack(bref));
      break;
    case types::rrc_conn_release:
      HANDLE_CODE(c.get<rrc_conn_release_s>().pack(bref));
      break;
    case types::security_mode_cmd:
      HANDLE_CODE(c.get<security_mode_cmd_s>().pack(bref));
      break;
    case types::ue_cap_enquiry:
      HANDLE_CODE(c.get<ue_cap_enquiry_s>().pack(bref));
      break;
    case types::counter_check:
      HANDLE_CODE(c.get<counter_check_s>().pack(bref));
      break;
    case types::ue_info_request_r9:
      HANDLE_CODE(c.get<ue_info_request_r9_s>().pack(bref));
      break;
    case types::logged_meas_cfg_r10:
      HANDLE_CODE(c.get<logged_meas_cfg_r10_s>().pack(bref));
      break;
    case types::rn_recfg_r10:
      HANDLE_CODE(c.get<rn_recfg_r10_s>().pack(bref));
      break;
    case types::rrc_conn_resume_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_r13_s>().pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_dcch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::csfb_params_resp_cdma2000:
      HANDLE_CODE(c.get<csfb_params_resp_cdma2000_s>().unpack(bref));
      break;
    case types::dl_info_transfer:
      HANDLE_CODE(c.get<dl_info_transfer_s>().unpack(bref));
      break;
    case types::ho_from_eutra_prep_request:
      HANDLE_CODE(c.get<ho_from_eutra_prep_request_s>().unpack(bref));
      break;
    case types::mob_from_eutra_cmd:
      HANDLE_CODE(c.get<mob_from_eutra_cmd_s>().unpack(bref));
      break;
    case types::rrc_conn_recfg:
      HANDLE_CODE(c.get<rrc_conn_recfg_s>().unpack(bref));
      break;
    case types::rrc_conn_release:
      HANDLE_CODE(c.get<rrc_conn_release_s>().unpack(bref));
      break;
    case types::security_mode_cmd:
      HANDLE_CODE(c.get<security_mode_cmd_s>().unpack(bref));
      break;
    case types::ue_cap_enquiry:
      HANDLE_CODE(c.get<ue_cap_enquiry_s>().unpack(bref));
      break;
    case types::counter_check:
      HANDLE_CODE(c.get<counter_check_s>().unpack(bref));
      break;
    case types::ue_info_request_r9:
      HANDLE_CODE(c.get<ue_info_request_r9_s>().unpack(bref));
      break;
    case types::logged_meas_cfg_r10:
      HANDLE_CODE(c.get<logged_meas_cfg_r10_s>().unpack(bref));
      break;
    case types::rn_recfg_r10:
      HANDLE_CODE(c.get<rn_recfg_r10_s>().unpack(bref));
      break;
    case types::rrc_conn_resume_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_r13_s>().unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// DL-DCCH-Message ::= SEQUENCE
SRSASN_CODE dl_dcch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_dcch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void dl_dcch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("DL-DCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// RadioResourceConfigCommonPSCell-v12f0 ::= SEQUENCE
SRSASN_CODE rr_cfg_common_ps_cell_v12f0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(basic_fields_v12f0.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_common_ps_cell_v12f0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(basic_fields_v12f0.unpack(bref));

  return SRSASN_SUCCESS;
}
void rr_cfg_common_ps_cell_v12f0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("basicFields-v12f0");
  basic_fields_v12f0.to_json(j);
  j.end_obj();
}

// PSCellToAddMod-v12f0 ::= SEQUENCE
SRSASN_CODE ps_cell_to_add_mod_v12f0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rr_cfg_common_ps_cell_r12_present, 1));

  if (rr_cfg_common_ps_cell_r12_present) {
    HANDLE_CODE(rr_cfg_common_ps_cell_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ps_cell_to_add_mod_v12f0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rr_cfg_common_ps_cell_r12_present, 1));

  if (rr_cfg_common_ps_cell_r12_present) {
    HANDLE_CODE(rr_cfg_common_ps_cell_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ps_cell_to_add_mod_v12f0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rr_cfg_common_ps_cell_r12_present) {
    j.write_fieldname("radioResourceConfigCommonPSCell-r12");
    rr_cfg_common_ps_cell_r12.to_json(j);
  }
  j.end_obj();
}

// SCG-ConfigPartSCG-v13c0 ::= SEQUENCE
SRSASN_CODE scg_cfg_part_scg_v13c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_scg_v13c0_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_scg_ext_v13c0_present, 1));

  if (scell_to_add_mod_list_scg_v13c0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_scg_v13c0, 1, 4));
  }
  if (scell_to_add_mod_list_scg_ext_v13c0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_scg_ext_v13c0, 1, 31));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_part_scg_v13c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_scg_v13c0_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_scg_ext_v13c0_present, 1));

  if (scell_to_add_mod_list_scg_v13c0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_scg_v13c0, bref, 1, 4));
  }
  if (scell_to_add_mod_list_scg_ext_v13c0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_scg_ext_v13c0, bref, 1, 31));
  }

  return SRSASN_SUCCESS;
}
void scg_cfg_part_scg_v13c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scell_to_add_mod_list_scg_v13c0_present) {
    j.start_array("sCellToAddModListSCG-v13c0");
    for (const auto& e1 : scell_to_add_mod_list_scg_v13c0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (scell_to_add_mod_list_scg_ext_v13c0_present) {
    j.start_array("sCellToAddModListSCG-Ext-v13c0");
    for (const auto& e1 : scell_to_add_mod_list_scg_ext_v13c0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SCG-Configuration-v13c0 ::= CHOICE
void scg_cfg_v13c0_c::set(types::options e)
{
  type_ = e;
}
void scg_cfg_v13c0_c::set_release()
{
  set(types::release);
}
scg_cfg_v13c0_c::setup_s_& scg_cfg_v13c0_c::set_setup()
{
  set(types::setup);
  return c;
}
void scg_cfg_v13c0_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.scg_cfg_part_scg_v13c0_present) {
        j.write_fieldname("scg-ConfigPartSCG-v13c0");
        c.scg_cfg_part_scg_v13c0.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_v13c0_c");
  }
  j.end_obj();
}
SRSASN_CODE scg_cfg_v13c0_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.scg_cfg_part_scg_v13c0_present, 1));
      if (c.scg_cfg_part_scg_v13c0_present) {
        HANDLE_CODE(c.scg_cfg_part_scg_v13c0.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_v13c0_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_v13c0_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.scg_cfg_part_scg_v13c0_present, 1));
      if (c.scg_cfg_part_scg_v13c0_present) {
        HANDLE_CODE(c.scg_cfg_part_scg_v13c0.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_v13c0_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionReconfiguration-v13c0-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v13c0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rr_cfg_ded_v13c0_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_v13c0_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_ext_v13c0_present, 1));
  HANDLE_CODE(bref.pack(scg_cfg_v13c0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rr_cfg_ded_v13c0_present) {
    HANDLE_CODE(rr_cfg_ded_v13c0.pack(bref));
  }
  if (scell_to_add_mod_list_v13c0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_v13c0, 1, 4));
  }
  if (scell_to_add_mod_list_ext_v13c0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_ext_v13c0, 1, 31));
  }
  if (scg_cfg_v13c0_present) {
    HANDLE_CODE(scg_cfg_v13c0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v13c0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rr_cfg_ded_v13c0_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_v13c0_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_ext_v13c0_present, 1));
  HANDLE_CODE(bref.unpack(scg_cfg_v13c0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rr_cfg_ded_v13c0_present) {
    HANDLE_CODE(rr_cfg_ded_v13c0.unpack(bref));
  }
  if (scell_to_add_mod_list_v13c0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_v13c0, bref, 1, 4));
  }
  if (scell_to_add_mod_list_ext_v13c0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_ext_v13c0, bref, 1, 31));
  }
  if (scg_cfg_v13c0_present) {
    HANDLE_CODE(scg_cfg_v13c0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_v13c0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rr_cfg_ded_v13c0_present) {
    j.write_fieldname("radioResourceConfigDedicated-v13c0");
    rr_cfg_ded_v13c0.to_json(j);
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
  if (scg_cfg_v13c0_present) {
    j.write_fieldname("scg-Configuration-v13c0");
    scg_cfg_v13c0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RadioResourceConfigDedicated-v1370 ::= SEQUENCE
SRSASN_CODE rr_cfg_ded_v1370_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phys_cfg_ded_v1370_present, 1));

  if (phys_cfg_ded_v1370_present) {
    HANDLE_CODE(phys_cfg_ded_v1370.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_ded_v1370_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phys_cfg_ded_v1370_present, 1));

  if (phys_cfg_ded_v1370_present) {
    HANDLE_CODE(phys_cfg_ded_v1370.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rr_cfg_ded_v1370_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phys_cfg_ded_v1370_present) {
    j.write_fieldname("physicalConfigDedicated-v1370");
    phys_cfg_ded_v1370.to_json(j);
  }
  j.end_obj();
}

// SCG-ConfigPartSCG-v12f0 ::= SEQUENCE
SRSASN_CODE scg_cfg_part_scg_v12f0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pscell_to_add_mod_v12f0_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_scg_v12f0_present, 1));

  if (pscell_to_add_mod_v12f0_present) {
    HANDLE_CODE(pscell_to_add_mod_v12f0.pack(bref));
  }
  if (scell_to_add_mod_list_scg_v12f0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_scg_v12f0, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_part_scg_v12f0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pscell_to_add_mod_v12f0_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_scg_v12f0_present, 1));

  if (pscell_to_add_mod_v12f0_present) {
    HANDLE_CODE(pscell_to_add_mod_v12f0.unpack(bref));
  }
  if (scell_to_add_mod_list_scg_v12f0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_scg_v12f0, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void scg_cfg_part_scg_v12f0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pscell_to_add_mod_v12f0_present) {
    j.write_fieldname("pSCellToAddMod-v12f0");
    pscell_to_add_mod_v12f0.to_json(j);
  }
  if (scell_to_add_mod_list_scg_v12f0_present) {
    j.start_array("sCellToAddModListSCG-v12f0");
    for (const auto& e1 : scell_to_add_mod_list_scg_v12f0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// RRCConnectionReconfiguration-v1370-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v1370_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rr_cfg_ded_v1370_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_ext_v1370_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rr_cfg_ded_v1370_present) {
    HANDLE_CODE(rr_cfg_ded_v1370.pack(bref));
  }
  if (scell_to_add_mod_list_ext_v1370_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_ext_v1370, 1, 31));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v1370_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rr_cfg_ded_v1370_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_ext_v1370_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rr_cfg_ded_v1370_present) {
    HANDLE_CODE(rr_cfg_ded_v1370.unpack(bref));
  }
  if (scell_to_add_mod_list_ext_v1370_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_ext_v1370, bref, 1, 31));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_v1370_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rr_cfg_ded_v1370_present) {
    j.write_fieldname("radioResourceConfigDedicated-v1370");
    rr_cfg_ded_v1370.to_json(j);
  }
  if (scell_to_add_mod_list_ext_v1370_present) {
    j.start_array("sCellToAddModListExt-v1370");
    for (const auto& e1 : scell_to_add_mod_list_ext_v1370) {
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

// SCG-Configuration-v12f0 ::= CHOICE
void scg_cfg_v12f0_c::set(types::options e)
{
  type_ = e;
}
void scg_cfg_v12f0_c::set_release()
{
  set(types::release);
}
scg_cfg_v12f0_c::setup_s_& scg_cfg_v12f0_c::set_setup()
{
  set(types::setup);
  return c;
}
void scg_cfg_v12f0_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.scg_cfg_part_scg_v12f0_present) {
        j.write_fieldname("scg-ConfigPartSCG-v12f0");
        c.scg_cfg_part_scg_v12f0.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_v12f0_c");
  }
  j.end_obj();
}
SRSASN_CODE scg_cfg_v12f0_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.scg_cfg_part_scg_v12f0_present, 1));
      if (c.scg_cfg_part_scg_v12f0_present) {
        HANDLE_CODE(c.scg_cfg_part_scg_v12f0.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_v12f0_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_v12f0_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.scg_cfg_part_scg_v12f0_present, 1));
      if (c.scg_cfg_part_scg_v12f0_present) {
        HANDLE_CODE(c.scg_cfg_part_scg_v12f0.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_v12f0_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionReconfiguration-v12f0-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v12f0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scg_cfg_v12f0_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (scg_cfg_v12f0_present) {
    HANDLE_CODE(scg_cfg_v12f0.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v12f0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scg_cfg_v12f0_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (scg_cfg_v12f0_present) {
    HANDLE_CODE(scg_cfg_v12f0.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_v12f0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scg_cfg_v12f0_present) {
    j.write_fieldname("scg-Configuration-v12f0");
    scg_cfg_v12f0.to_json(j);
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

// RRCConnectionReconfiguration-v10l0-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v10l0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mob_ctrl_info_v10l0_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_v10l0_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mob_ctrl_info_v10l0_present) {
    HANDLE_CODE(mob_ctrl_info_v10l0.pack(bref));
  }
  if (scell_to_add_mod_list_v10l0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_v10l0, 1, 4));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v10l0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mob_ctrl_info_v10l0_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_v10l0_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mob_ctrl_info_v10l0_present) {
    HANDLE_CODE(mob_ctrl_info_v10l0.unpack(bref));
  }
  if (scell_to_add_mod_list_v10l0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_v10l0, bref, 1, 4));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_v10l0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mob_ctrl_info_v10l0_present) {
    j.write_fieldname("mobilityControlInfo-v10l0");
    mob_ctrl_info_v10l0.to_json(j);
  }
  if (scell_to_add_mod_list_v10l0_present) {
    j.start_array("sCellToAddModList-v10l0");
    for (const auto& e1 : scell_to_add_mod_list_v10l0) {
      e1.to_json(j);
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

// RRCConnectionReconfiguration-v10i0-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v10i0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ant_info_ded_pcell_v10i0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ant_info_ded_pcell_v10i0_present) {
    HANDLE_CODE(ant_info_ded_pcell_v10i0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_v10i0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ant_info_ded_pcell_v10i0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ant_info_ded_pcell_v10i0_present) {
    HANDLE_CODE(ant_info_ded_pcell_v10i0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_v10i0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ant_info_ded_pcell_v10i0_present) {
    j.write_fieldname("antennaInfoDedicatedPCell-v10i0");
    ant_info_ded_pcell_v10i0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReconfiguration-v8m0-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_v8m0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE rrc_conn_recfg_v8m0_ies_s::unpack(cbit_ref& bref)
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
void rrc_conn_recfg_v8m0_ies_s::to_json(json_writer& j) const
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

// RedirectedCarrierInfo-v9e0 ::= SEQUENCE
SRSASN_CODE redirected_carrier_info_v9e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, eutra_v9e0, (uint32_t)65536u, (uint32_t)262143u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE redirected_carrier_info_v9e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(eutra_v9e0, bref, (uint32_t)65536u, (uint32_t)262143u));

  return SRSASN_SUCCESS;
}
void redirected_carrier_info_v9e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("eutra-v9e0", eutra_v9e0);
  j.end_obj();
}

// RRCConnectionRelease-v9e0-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_release_v9e0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(redirected_carrier_info_v9e0_present, 1));
  HANDLE_CODE(bref.pack(idle_mode_mob_ctrl_info_v9e0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (redirected_carrier_info_v9e0_present) {
    HANDLE_CODE(redirected_carrier_info_v9e0.pack(bref));
  }
  if (idle_mode_mob_ctrl_info_v9e0_present) {
    HANDLE_CODE(idle_mode_mob_ctrl_info_v9e0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_release_v9e0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(redirected_carrier_info_v9e0_present, 1));
  HANDLE_CODE(bref.unpack(idle_mode_mob_ctrl_info_v9e0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (redirected_carrier_info_v9e0_present) {
    HANDLE_CODE(redirected_carrier_info_v9e0.unpack(bref));
  }
  if (idle_mode_mob_ctrl_info_v9e0_present) {
    HANDLE_CODE(idle_mode_mob_ctrl_info_v9e0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_release_v9e0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (redirected_carrier_info_v9e0_present) {
    j.write_fieldname("redirectedCarrierInfo-v9e0");
    redirected_carrier_info_v9e0.to_json(j);
  }
  if (idle_mode_mob_ctrl_info_v9e0_present) {
    j.write_fieldname("idleModeMobilityControlInfo-v9e0");
    idle_mode_mob_ctrl_info_v9e0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}
