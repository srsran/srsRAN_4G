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

#include "srsran/asn1/rrc/ul_dcch_msg.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// TMGI-r9 ::= SEQUENCE
SRSASN_CODE tmgi_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id_r9.pack(bref));
  HANDLE_CODE(service_id_r9.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tmgi_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id_r9.unpack(bref));
  HANDLE_CODE(service_id_r9.unpack(bref));

  return SRSASN_SUCCESS;
}
void tmgi_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Id-r9");
  plmn_id_r9.to_json(j);
  j.write_str("serviceId-r9", service_id_r9.to_string());
  j.end_obj();
}

void tmgi_r9_s::plmn_id_r9_c_::destroy_()
{
  switch (type_) {
    case types::explicit_value_r9:
      c.destroy<plmn_id_s>();
      break;
    default:
      break;
  }
}
void tmgi_r9_s::plmn_id_r9_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::plmn_idx_r9:
      break;
    case types::explicit_value_r9:
      c.init<plmn_id_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r9_s::plmn_id_r9_c_");
  }
}
tmgi_r9_s::plmn_id_r9_c_::plmn_id_r9_c_(const tmgi_r9_s::plmn_id_r9_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::plmn_idx_r9:
      c.init(other.c.get<uint8_t>());
      break;
    case types::explicit_value_r9:
      c.init(other.c.get<plmn_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r9_s::plmn_id_r9_c_");
  }
}
tmgi_r9_s::plmn_id_r9_c_& tmgi_r9_s::plmn_id_r9_c_::operator=(const tmgi_r9_s::plmn_id_r9_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::plmn_idx_r9:
      c.set(other.c.get<uint8_t>());
      break;
    case types::explicit_value_r9:
      c.set(other.c.get<plmn_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r9_s::plmn_id_r9_c_");
  }

  return *this;
}
uint8_t& tmgi_r9_s::plmn_id_r9_c_::set_plmn_idx_r9()
{
  set(types::plmn_idx_r9);
  return c.get<uint8_t>();
}
plmn_id_s& tmgi_r9_s::plmn_id_r9_c_::set_explicit_value_r9()
{
  set(types::explicit_value_r9);
  return c.get<plmn_id_s>();
}
void tmgi_r9_s::plmn_id_r9_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::plmn_idx_r9:
      j.write_int("plmn-Index-r9", c.get<uint8_t>());
      break;
    case types::explicit_value_r9:
      j.write_fieldname("explicitValue-r9");
      c.get<plmn_id_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r9_s::plmn_id_r9_c_");
  }
  j.end_obj();
}
SRSASN_CODE tmgi_r9_s::plmn_id_r9_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::plmn_idx_r9:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)6u));
      break;
    case types::explicit_value_r9:
      HANDLE_CODE(c.get<plmn_id_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r9_s::plmn_id_r9_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE tmgi_r9_s::plmn_id_r9_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::plmn_idx_r9:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)6u));
      break;
    case types::explicit_value_r9:
      HANDLE_CODE(c.get<plmn_id_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r9_s::plmn_id_r9_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MeasResultIdleEUTRA-r15 ::= SEQUENCE
SRSASN_CODE meas_result_idle_eutra_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, carrier_freq_r15, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(pack_integer(bref, pci_r15, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(pack_integer(bref, meas_result_r15.rsrp_result_r15, (uint8_t)0u, (uint8_t)97u));
  HANDLE_CODE(pack_integer(bref, meas_result_r15.rsrq_result_r15, (int8_t)-30, (int8_t)46));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_idle_eutra_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(carrier_freq_r15, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(unpack_integer(pci_r15, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(unpack_integer(meas_result_r15.rsrp_result_r15, bref, (uint8_t)0u, (uint8_t)97u));
  HANDLE_CODE(unpack_integer(meas_result_r15.rsrq_result_r15, bref, (int8_t)-30, (int8_t)46));

  return SRSASN_SUCCESS;
}
void meas_result_idle_eutra_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r15", carrier_freq_r15);
  j.write_int("physCellId-r15", pci_r15);
  j.write_fieldname("measResult-r15");
  j.start_obj();
  j.write_int("rsrpResult-r15", meas_result_r15.rsrp_result_r15);
  j.write_int("rsrqResult-r15", meas_result_r15.rsrq_result_r15);
  j.end_obj();
  j.end_obj();
}

// RRCConnectionSetupComplete-v1540-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gummei_type_v1540_present, 1));
  HANDLE_CODE(bref.pack(guami_type_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (guami_type_r15_present) {
    HANDLE_CODE(guami_type_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_v1540_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gummei_type_v1540_present, 1));
  HANDLE_CODE(bref.unpack(guami_type_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (guami_type_r15_present) {
    HANDLE_CODE(guami_type_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_complete_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gummei_type_v1540_present) {
    j.write_str("gummei-Type-v1540", "mappedFrom5G-v1540");
  }
  if (guami_type_r15_present) {
    j.write_str("guami-Type-r15", guami_type_r15.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* rrc_conn_setup_complete_v1540_ies_s::guami_type_r15_opts::to_string() const
{
  static const char* options[] = {"native", "mapped"};
  return convert_enum_idx(options, 2, value, "rrc_conn_setup_complete_v1540_ies_s::guami_type_r15_e_");
}

// RegisteredAMF-r15 ::= SEQUENCE
SRSASN_CODE registered_amf_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(plmn_id_r15_present, 1));

  if (plmn_id_r15_present) {
    HANDLE_CODE(plmn_id_r15.pack(bref));
  }
  HANDLE_CODE(amf_id_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE registered_amf_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(plmn_id_r15_present, 1));

  if (plmn_id_r15_present) {
    HANDLE_CODE(plmn_id_r15.unpack(bref));
  }
  HANDLE_CODE(amf_id_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void registered_amf_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_r15_present) {
    j.write_fieldname("plmn-Identity-r15");
    plmn_id_r15.to_json(j);
  }
  j.write_str("amf-Identifier-r15", amf_id_r15.to_string());
  j.end_obj();
}

// S-NSSAI-r15 ::= CHOICE
void s_nssai_r15_c::destroy_()
{
  switch (type_) {
    case types::sst:
      c.destroy<fixed_bitstring<8> >();
      break;
    case types::sst_sd:
      c.destroy<fixed_bitstring<32> >();
      break;
    default:
      break;
  }
}
void s_nssai_r15_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sst:
      c.init<fixed_bitstring<8> >();
      break;
    case types::sst_sd:
      c.init<fixed_bitstring<32> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_r15_c");
  }
}
s_nssai_r15_c::s_nssai_r15_c(const s_nssai_r15_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sst:
      c.init(other.c.get<fixed_bitstring<8> >());
      break;
    case types::sst_sd:
      c.init(other.c.get<fixed_bitstring<32> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_r15_c");
  }
}
s_nssai_r15_c& s_nssai_r15_c::operator=(const s_nssai_r15_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sst:
      c.set(other.c.get<fixed_bitstring<8> >());
      break;
    case types::sst_sd:
      c.set(other.c.get<fixed_bitstring<32> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_r15_c");
  }

  return *this;
}
fixed_bitstring<8>& s_nssai_r15_c::set_sst()
{
  set(types::sst);
  return c.get<fixed_bitstring<8> >();
}
fixed_bitstring<32>& s_nssai_r15_c::set_sst_sd()
{
  set(types::sst_sd);
  return c.get<fixed_bitstring<32> >();
}
void s_nssai_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sst:
      j.write_str("sst", c.get<fixed_bitstring<8> >().to_string());
      break;
    case types::sst_sd:
      j.write_str("sst-SD", c.get<fixed_bitstring<32> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE s_nssai_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sst:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().pack(bref));
      break;
    case types::sst_sd:
      HANDLE_CODE(c.get<fixed_bitstring<32> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE s_nssai_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sst:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().unpack(bref));
      break;
    case types::sst_sd:
      HANDLE_CODE(c.get<fixed_bitstring<32> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionSetupComplete-v1530-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(log_meas_available_bt_r15_present, 1));
  HANDLE_CODE(bref.pack(log_meas_available_wlan_r15_present, 1));
  HANDLE_CODE(bref.pack(idle_meas_available_r15_present, 1));
  HANDLE_CODE(bref.pack(flight_path_info_available_r15_present, 1));
  HANDLE_CODE(bref.pack(connect_to5_gc_r15_present, 1));
  HANDLE_CODE(bref.pack(registered_amf_r15_present, 1));
  HANDLE_CODE(bref.pack(s_nssai_list_r15_present, 1));
  HANDLE_CODE(bref.pack(ng_minus5_g_s_tmsi_bits_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (registered_amf_r15_present) {
    HANDLE_CODE(registered_amf_r15.pack(bref));
  }
  if (s_nssai_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, s_nssai_list_r15, 1, 8));
  }
  if (ng_minus5_g_s_tmsi_bits_r15_present) {
    HANDLE_CODE(ng_minus5_g_s_tmsi_bits_r15.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(log_meas_available_bt_r15_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_available_wlan_r15_present, 1));
  HANDLE_CODE(bref.unpack(idle_meas_available_r15_present, 1));
  HANDLE_CODE(bref.unpack(flight_path_info_available_r15_present, 1));
  HANDLE_CODE(bref.unpack(connect_to5_gc_r15_present, 1));
  HANDLE_CODE(bref.unpack(registered_amf_r15_present, 1));
  HANDLE_CODE(bref.unpack(s_nssai_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(ng_minus5_g_s_tmsi_bits_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (registered_amf_r15_present) {
    HANDLE_CODE(registered_amf_r15.unpack(bref));
  }
  if (s_nssai_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(s_nssai_list_r15, bref, 1, 8));
  }
  if (ng_minus5_g_s_tmsi_bits_r15_present) {
    HANDLE_CODE(ng_minus5_g_s_tmsi_bits_r15.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_complete_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (log_meas_available_bt_r15_present) {
    j.write_str("logMeasAvailableBT-r15", "true");
  }
  if (log_meas_available_wlan_r15_present) {
    j.write_str("logMeasAvailableWLAN-r15", "true");
  }
  if (idle_meas_available_r15_present) {
    j.write_str("idleMeasAvailable-r15", "true");
  }
  if (flight_path_info_available_r15_present) {
    j.write_str("flightPathInfoAvailable-r15", "true");
  }
  if (connect_to5_gc_r15_present) {
    j.write_str("connectTo5GC-r15", "true");
  }
  if (registered_amf_r15_present) {
    j.write_fieldname("registeredAMF-r15");
    registered_amf_r15.to_json(j);
  }
  if (s_nssai_list_r15_present) {
    j.start_array("s-NSSAI-list-r15");
    for (const auto& e1 : s_nssai_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ng_minus5_g_s_tmsi_bits_r15_present) {
    j.write_fieldname("ng-5G-S-TMSI-Bits-r15");
    ng_minus5_g_s_tmsi_bits_r15.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

void rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::destroy_()
{
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_r15:
      c.destroy<fixed_bitstring<48> >();
      break;
    case types::ng_minus5_g_s_tmsi_part2_r15:
      c.destroy<fixed_bitstring<8> >();
      break;
    default:
      break;
  }
}
void rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_r15:
      c.init<fixed_bitstring<48> >();
      break;
    case types::ng_minus5_g_s_tmsi_part2_r15:
      c.init<fixed_bitstring<8> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_");
  }
}
rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::ng_minus5_g_s_tmsi_bits_r15_c_(
    const rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_r15:
      c.init(other.c.get<fixed_bitstring<48> >());
      break;
    case types::ng_minus5_g_s_tmsi_part2_r15:
      c.init(other.c.get<fixed_bitstring<8> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_");
  }
}
rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_&
rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::operator=(
    const rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_r15:
      c.set(other.c.get<fixed_bitstring<48> >());
      break;
    case types::ng_minus5_g_s_tmsi_part2_r15:
      c.set(other.c.get<fixed_bitstring<8> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_");
  }

  return *this;
}
fixed_bitstring<48>& rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::set_ng_minus5_g_s_tmsi_r15()
{
  set(types::ng_minus5_g_s_tmsi_r15);
  return c.get<fixed_bitstring<48> >();
}
fixed_bitstring<8>&
rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::set_ng_minus5_g_s_tmsi_part2_r15()
{
  set(types::ng_minus5_g_s_tmsi_part2_r15);
  return c.get<fixed_bitstring<8> >();
}
void rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_r15:
      j.write_str("ng-5G-S-TMSI-r15", c.get<fixed_bitstring<48> >().to_string());
      break;
    case types::ng_minus5_g_s_tmsi_part2_r15:
      j.write_str("ng-5G-S-TMSI-Part2-r15", c.get<fixed_bitstring<8> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_r15:
      HANDLE_CODE(c.get<fixed_bitstring<48> >().pack(bref));
      break;
    case types::ng_minus5_g_s_tmsi_part2_r15:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_r15:
      HANDLE_CODE(c.get<fixed_bitstring<48> >().unpack(bref));
      break;
    case types::ng_minus5_g_s_tmsi_part2_r15:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MeasResultIdle-r15 ::= SEQUENCE
SRSASN_CODE meas_result_idle_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, meas_result_serving_cell_r15.rsrp_result_r15, (uint8_t)0u, (uint8_t)97u));
  HANDLE_CODE(pack_integer(bref, meas_result_serving_cell_r15.rsrq_result_r15, (int8_t)-30, (int8_t)46));
  if (meas_result_neigh_cells_r15_present) {
    HANDLE_CODE(meas_result_neigh_cells_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_idle_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r15_present, 1));

  HANDLE_CODE(unpack_integer(meas_result_serving_cell_r15.rsrp_result_r15, bref, (uint8_t)0u, (uint8_t)97u));
  HANDLE_CODE(unpack_integer(meas_result_serving_cell_r15.rsrq_result_r15, bref, (int8_t)-30, (int8_t)46));
  if (meas_result_neigh_cells_r15_present) {
    HANDLE_CODE(meas_result_neigh_cells_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_result_idle_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measResultServingCell-r15");
  j.start_obj();
  j.write_int("rsrpResult-r15", meas_result_serving_cell_r15.rsrp_result_r15);
  j.write_int("rsrqResult-r15", meas_result_serving_cell_r15.rsrq_result_r15);
  j.end_obj();
  if (meas_result_neigh_cells_r15_present) {
    j.write_fieldname("measResultNeighCells-r15");
    meas_result_neigh_cells_r15.to_json(j);
  }
  j.end_obj();
}

void meas_result_idle_r15_s::meas_result_neigh_cells_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measResultIdleListEUTRA-r15");
  for (const auto& e1 : c) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
SRSASN_CODE meas_result_idle_r15_s::meas_result_neigh_cells_r15_c_::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 8));
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_idle_r15_s::meas_result_neigh_cells_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "meas_result_idle_r15_s::meas_result_neigh_cells_r15_c_");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 8));
  return SRSASN_SUCCESS;
}

// PerCC-GapIndication-r14 ::= SEQUENCE
SRSASN_CODE per_cc_gap_ind_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, serv_cell_id_r14, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(gap_ind_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE per_cc_gap_ind_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(serv_cell_id_r14, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(gap_ind_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void per_cc_gap_ind_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellId-r14", serv_cell_id_r14);
  j.write_str("gapIndication-r14", gap_ind_r14.to_string());
  j.end_obj();
}

const char* per_cc_gap_ind_r14_s::gap_ind_r14_opts::to_string() const
{
  static const char* options[] = {"gap", "ncsg", "nogap-noNcsg"};
  return convert_enum_idx(options, 3, value, "per_cc_gap_ind_r14_s::gap_ind_r14_e_");
}

// RRCConnectionReconfigurationComplete-v1530-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_complete_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(log_meas_available_bt_r15_present, 1));
  HANDLE_CODE(bref.pack(log_meas_available_wlan_r15_present, 1));
  HANDLE_CODE(bref.pack(flight_path_info_available_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_complete_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(log_meas_available_bt_r15_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_available_wlan_r15_present, 1));
  HANDLE_CODE(bref.unpack(flight_path_info_available_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_complete_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (log_meas_available_bt_r15_present) {
    j.write_str("logMeasAvailableBT-r15", "true");
  }
  if (log_meas_available_wlan_r15_present) {
    j.write_str("logMeasAvailableWLAN-r15", "true");
  }
  if (flight_path_info_available_r15_present) {
    j.write_str("flightPathInfoAvailable-r15", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RRCConnectionSetupComplete-v1430-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_v1430_ies_s::pack(bit_ref& bref) const
{
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
SRSASN_CODE rrc_conn_setup_complete_v1430_ies_s::unpack(cbit_ref& bref)
{
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
void rrc_conn_setup_complete_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dcn_id_r14_present) {
    j.write_int("dcn-ID-r14", dcn_id_r14);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// VictimSystemType-r11 ::= SEQUENCE
SRSASN_CODE victim_sys_type_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gps_r11_present, 1));
  HANDLE_CODE(bref.pack(glonass_r11_present, 1));
  HANDLE_CODE(bref.pack(bds_r11_present, 1));
  HANDLE_CODE(bref.pack(galileo_r11_present, 1));
  HANDLE_CODE(bref.pack(wlan_r11_present, 1));
  HANDLE_CODE(bref.pack(bluetooth_r11_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE victim_sys_type_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gps_r11_present, 1));
  HANDLE_CODE(bref.unpack(glonass_r11_present, 1));
  HANDLE_CODE(bref.unpack(bds_r11_present, 1));
  HANDLE_CODE(bref.unpack(galileo_r11_present, 1));
  HANDLE_CODE(bref.unpack(wlan_r11_present, 1));
  HANDLE_CODE(bref.unpack(bluetooth_r11_present, 1));

  return SRSASN_SUCCESS;
}
void victim_sys_type_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gps_r11_present) {
    j.write_str("gps-r11", "true");
  }
  if (glonass_r11_present) {
    j.write_str("glonass-r11", "true");
  }
  if (bds_r11_present) {
    j.write_str("bds-r11", "true");
  }
  if (galileo_r11_present) {
    j.write_str("galileo-r11", "true");
  }
  if (wlan_r11_present) {
    j.write_str("wlan-r11", "true");
  }
  if (bluetooth_r11_present) {
    j.write_str("bluetooth-r11", "true");
  }
  j.end_obj();
}

// VisitedCellInfo-r12 ::= SEQUENCE
SRSASN_CODE visited_cell_info_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(visited_cell_id_r12_present, 1));

  if (visited_cell_id_r12_present) {
    HANDLE_CODE(visited_cell_id_r12.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, time_spent_r12, (uint16_t)0u, (uint16_t)4095u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE visited_cell_info_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(visited_cell_id_r12_present, 1));

  if (visited_cell_id_r12_present) {
    HANDLE_CODE(visited_cell_id_r12.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(time_spent_r12, bref, (uint16_t)0u, (uint16_t)4095u));

  return SRSASN_SUCCESS;
}
void visited_cell_info_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (visited_cell_id_r12_present) {
    j.write_fieldname("visitedCellId-r12");
    visited_cell_id_r12.to_json(j);
  }
  j.write_int("timeSpent-r12", time_spent_r12);
  j.end_obj();
}

void visited_cell_info_r12_s::visited_cell_id_r12_c_::destroy_()
{
  switch (type_) {
    case types::cell_global_id_r12:
      c.destroy<cell_global_id_eutra_s>();
      break;
    case types::pci_arfcn_r12:
      c.destroy<pci_arfcn_r12_s_>();
      break;
    default:
      break;
  }
}
void visited_cell_info_r12_s::visited_cell_id_r12_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_global_id_r12:
      c.init<cell_global_id_eutra_s>();
      break;
    case types::pci_arfcn_r12:
      c.init<pci_arfcn_r12_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r12_s::visited_cell_id_r12_c_");
  }
}
visited_cell_info_r12_s::visited_cell_id_r12_c_::visited_cell_id_r12_c_(
    const visited_cell_info_r12_s::visited_cell_id_r12_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_global_id_r12:
      c.init(other.c.get<cell_global_id_eutra_s>());
      break;
    case types::pci_arfcn_r12:
      c.init(other.c.get<pci_arfcn_r12_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r12_s::visited_cell_id_r12_c_");
  }
}
visited_cell_info_r12_s::visited_cell_id_r12_c_&
visited_cell_info_r12_s::visited_cell_id_r12_c_::operator=(const visited_cell_info_r12_s::visited_cell_id_r12_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_global_id_r12:
      c.set(other.c.get<cell_global_id_eutra_s>());
      break;
    case types::pci_arfcn_r12:
      c.set(other.c.get<pci_arfcn_r12_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r12_s::visited_cell_id_r12_c_");
  }

  return *this;
}
cell_global_id_eutra_s& visited_cell_info_r12_s::visited_cell_id_r12_c_::set_cell_global_id_r12()
{
  set(types::cell_global_id_r12);
  return c.get<cell_global_id_eutra_s>();
}
visited_cell_info_r12_s::visited_cell_id_r12_c_::pci_arfcn_r12_s_&
visited_cell_info_r12_s::visited_cell_id_r12_c_::set_pci_arfcn_r12()
{
  set(types::pci_arfcn_r12);
  return c.get<pci_arfcn_r12_s_>();
}
void visited_cell_info_r12_s::visited_cell_id_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_global_id_r12:
      j.write_fieldname("cellGlobalId-r12");
      c.get<cell_global_id_eutra_s>().to_json(j);
      break;
    case types::pci_arfcn_r12:
      j.write_fieldname("pci-arfcn-r12");
      j.start_obj();
      j.write_int("physCellId-r12", c.get<pci_arfcn_r12_s_>().pci_r12);
      j.write_int("carrierFreq-r12", c.get<pci_arfcn_r12_s_>().carrier_freq_r12);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r12_s::visited_cell_id_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE visited_cell_info_r12_s::visited_cell_id_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_global_id_r12:
      HANDLE_CODE(c.get<cell_global_id_eutra_s>().pack(bref));
      break;
    case types::pci_arfcn_r12:
      HANDLE_CODE(pack_integer(bref, c.get<pci_arfcn_r12_s_>().pci_r12, (uint16_t)0u, (uint16_t)503u));
      HANDLE_CODE(pack_integer(bref, c.get<pci_arfcn_r12_s_>().carrier_freq_r12, (uint32_t)0u, (uint32_t)262143u));
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r12_s::visited_cell_id_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE visited_cell_info_r12_s::visited_cell_id_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_global_id_r12:
      HANDLE_CODE(c.get<cell_global_id_eutra_s>().unpack(bref));
      break;
    case types::pci_arfcn_r12:
      HANDLE_CODE(unpack_integer(c.get<pci_arfcn_r12_s_>().pci_r12, bref, (uint16_t)0u, (uint16_t)503u));
      HANDLE_CODE(unpack_integer(c.get<pci_arfcn_r12_s_>().carrier_freq_r12, bref, (uint32_t)0u, (uint32_t)262143u));
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r12_s::visited_cell_id_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// WayPointLocation-r15 ::= SEQUENCE
SRSASN_CODE way_point_location_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(time_stamp_r15_present, 1));

  HANDLE_CODE(way_point_location_r15.pack(bref));
  if (time_stamp_r15_present) {
    HANDLE_CODE(time_stamp_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE way_point_location_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(time_stamp_r15_present, 1));

  HANDLE_CODE(way_point_location_r15.unpack(bref));
  if (time_stamp_r15_present) {
    HANDLE_CODE(time_stamp_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void way_point_location_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("wayPointLocation-r15");
  way_point_location_r15.to_json(j);
  if (time_stamp_r15_present) {
    j.write_str("timeStamp-r15", time_stamp_r15.to_string());
  }
  j.end_obj();
}

// AffectedCarrierFreqCombInfoMRDC-r15 ::= SEQUENCE
SRSASN_CODE affected_carrier_freq_comb_info_mrdc_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(affected_carrier_freq_comb_mrdc_r15_present, 1));

  HANDLE_CODE(victim_sys_type_r15.pack(bref));
  HANDLE_CODE(interference_direction_mrdc_r15.pack(bref));
  if (affected_carrier_freq_comb_mrdc_r15_present) {
    HANDLE_CODE(bref.pack(affected_carrier_freq_comb_mrdc_r15.affected_carrier_freq_comb_eutra_r15_present, 1));
    if (affected_carrier_freq_comb_mrdc_r15.affected_carrier_freq_comb_eutra_r15_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  affected_carrier_freq_comb_mrdc_r15.affected_carrier_freq_comb_eutra_r15,
                                  1,
                                  32,
                                  integer_packer<uint8_t>(1, 64)));
    }
    HANDLE_CODE(pack_dyn_seq_of(bref,
                                affected_carrier_freq_comb_mrdc_r15.affected_carrier_freq_comb_nr_r15,
                                1,
                                16,
                                integer_packer<uint32_t>(0, 3279165)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE affected_carrier_freq_comb_info_mrdc_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(affected_carrier_freq_comb_mrdc_r15_present, 1));

  HANDLE_CODE(victim_sys_type_r15.unpack(bref));
  HANDLE_CODE(interference_direction_mrdc_r15.unpack(bref));
  if (affected_carrier_freq_comb_mrdc_r15_present) {
    HANDLE_CODE(bref.unpack(affected_carrier_freq_comb_mrdc_r15.affected_carrier_freq_comb_eutra_r15_present, 1));
    if (affected_carrier_freq_comb_mrdc_r15.affected_carrier_freq_comb_eutra_r15_present) {
      HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_mrdc_r15.affected_carrier_freq_comb_eutra_r15,
                                    bref,
                                    1,
                                    32,
                                    integer_packer<uint8_t>(1, 64)));
    }
    HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_mrdc_r15.affected_carrier_freq_comb_nr_r15,
                                  bref,
                                  1,
                                  16,
                                  integer_packer<uint32_t>(0, 3279165)));
  }

  return SRSASN_SUCCESS;
}
void affected_carrier_freq_comb_info_mrdc_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("victimSystemType-r15");
  victim_sys_type_r15.to_json(j);
  j.write_str("interferenceDirectionMRDC-r15", interference_direction_mrdc_r15.to_string());
  if (affected_carrier_freq_comb_mrdc_r15_present) {
    j.write_fieldname("affectedCarrierFreqCombMRDC-r15");
    j.start_obj();
    if (affected_carrier_freq_comb_mrdc_r15.affected_carrier_freq_comb_eutra_r15_present) {
      j.start_array("affectedCarrierFreqCombEUTRA-r15");
      for (const auto& e1 : affected_carrier_freq_comb_mrdc_r15.affected_carrier_freq_comb_eutra_r15) {
        j.write_int(e1);
      }
      j.end_array();
    }
    j.start_array("affectedCarrierFreqCombNR-r15");
    for (const auto& e1 : affected_carrier_freq_comb_mrdc_r15.affected_carrier_freq_comb_nr_r15) {
      j.write_int(e1);
    }
    j.end_array();
    j.end_obj();
  }
  j.end_obj();
}

const char* affected_carrier_freq_comb_info_mrdc_r15_s::interference_direction_mrdc_r15_opts::to_string() const
{
  static const char* options[] = {
      "eutra-nr", "nr", "other", "eutra-nr-other", "nr-other", "spare3", "spare2", "spare1"};
  return convert_enum_idx(
      options, 8, value, "affected_carrier_freq_comb_info_mrdc_r15_s::interference_direction_mrdc_r15_e_");
}

// FlightPathInfoReport-r15 ::= SEQUENCE
SRSASN_CODE flight_path_info_report_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(flight_path_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (flight_path_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, flight_path_r15, 1, 20));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE flight_path_info_report_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(flight_path_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (flight_path_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(flight_path_r15, bref, 1, 20));
  }

  return SRSASN_SUCCESS;
}
void flight_path_info_report_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (flight_path_r15_present) {
    j.start_array("flightPath-r15");
    for (const auto& e1 : flight_path_r15) {
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

// RRCConnectionReconfigurationComplete-v1510-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_complete_v1510_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scg_cfg_resp_nr_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (scg_cfg_resp_nr_r15_present) {
    HANDLE_CODE(scg_cfg_resp_nr_r15.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_complete_v1510_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scg_cfg_resp_nr_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (scg_cfg_resp_nr_r15_present) {
    HANDLE_CODE(scg_cfg_resp_nr_r15.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_complete_v1510_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scg_cfg_resp_nr_r15_present) {
    j.write_str("scg-ConfigResponseNR-r15", scg_cfg_resp_nr_r15.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReestablishmentComplete-v1530-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_complete_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(log_meas_available_bt_r15_present, 1));
  HANDLE_CODE(bref.pack(log_meas_available_wlan_r15_present, 1));
  HANDLE_CODE(bref.pack(flight_path_info_available_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_complete_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(log_meas_available_bt_r15_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_available_wlan_r15_present, 1));
  HANDLE_CODE(bref.unpack(flight_path_info_available_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_complete_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (log_meas_available_bt_r15_present) {
    j.write_str("logMeasAvailableBT-r15", "true");
  }
  if (log_meas_available_wlan_r15_present) {
    j.write_str("logMeasAvailableWLAN-r15", "true");
  }
  if (flight_path_info_available_r15_present) {
    j.write_str("flightPathInfoAvailable-r15", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RRCConnectionSetupComplete-v1330-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_v1330_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_ce_need_ul_gaps_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_v1330_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_ce_need_ul_gaps_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_complete_v1330_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_ce_need_ul_gaps_r13_present) {
    j.write_str("ue-CE-NeedULGaps-r13", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MRDC-AssistanceInfo-r15 ::= SEQUENCE
SRSASN_CODE mrdc_assist_info_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, affected_carrier_freq_comb_info_list_mrdc_r15, 1, 128));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrdc_assist_info_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_info_list_mrdc_r15, bref, 1, 128));

  return SRSASN_SUCCESS;
}
void mrdc_assist_info_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("affectedCarrierFreqCombInfoListMRDC-r15");
  for (const auto& e1 : affected_carrier_freq_comb_info_list_mrdc_r15) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// RRCConnectionReconfigurationComplete-v1430-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_complete_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(per_cc_gap_ind_list_r14_present, 1));
  HANDLE_CODE(bref.pack(num_freq_effective_r14_present, 1));
  HANDLE_CODE(bref.pack(num_freq_effective_reduced_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (per_cc_gap_ind_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, per_cc_gap_ind_list_r14, 1, 32));
  }
  if (num_freq_effective_r14_present) {
    HANDLE_CODE(pack_integer(bref, num_freq_effective_r14, (uint8_t)1u, (uint8_t)12u));
  }
  if (num_freq_effective_reduced_r14_present) {
    HANDLE_CODE(pack_integer(bref, num_freq_effective_reduced_r14, (uint8_t)1u, (uint8_t)12u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_complete_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(per_cc_gap_ind_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(num_freq_effective_r14_present, 1));
  HANDLE_CODE(bref.unpack(num_freq_effective_reduced_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (per_cc_gap_ind_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(per_cc_gap_ind_list_r14, bref, 1, 32));
  }
  if (num_freq_effective_r14_present) {
    HANDLE_CODE(unpack_integer(num_freq_effective_r14, bref, (uint8_t)1u, (uint8_t)12u));
  }
  if (num_freq_effective_reduced_r14_present) {
    HANDLE_CODE(unpack_integer(num_freq_effective_reduced_r14, bref, (uint8_t)1u, (uint8_t)12u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_complete_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (per_cc_gap_ind_list_r14_present) {
    j.start_array("perCC-GapIndicationList-r14");
    for (const auto& e1 : per_cc_gap_ind_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (num_freq_effective_r14_present) {
    j.write_int("numFreqEffective-r14", num_freq_effective_r14);
  }
  if (num_freq_effective_reduced_r14_present) {
    j.write_int("numFreqEffectiveReduced-r14", num_freq_effective_reduced_r14);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReestablishmentComplete-v1250-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_complete_v1250_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(log_meas_available_mbsfn_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_complete_v1250_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(log_meas_available_mbsfn_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_complete_v1250_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (log_meas_available_mbsfn_r12_present) {
    j.write_str("logMeasAvailableMBSFN-r12", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionSetupComplete-v1320-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_v1320_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ce_mode_b_r13_present, 1));
  HANDLE_CODE(bref.pack(s_tmsi_r13_present, 1));
  HANDLE_CODE(bref.pack(attach_without_pdn_connect_r13_present, 1));
  HANDLE_CODE(bref.pack(up_cio_t_eps_optim_r13_present, 1));
  HANDLE_CODE(bref.pack(cp_cio_t_eps_optim_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (s_tmsi_r13_present) {
    HANDLE_CODE(s_tmsi_r13.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_v1320_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ce_mode_b_r13_present, 1));
  HANDLE_CODE(bref.unpack(s_tmsi_r13_present, 1));
  HANDLE_CODE(bref.unpack(attach_without_pdn_connect_r13_present, 1));
  HANDLE_CODE(bref.unpack(up_cio_t_eps_optim_r13_present, 1));
  HANDLE_CODE(bref.unpack(cp_cio_t_eps_optim_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (s_tmsi_r13_present) {
    HANDLE_CODE(s_tmsi_r13.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_complete_v1320_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ce_mode_b_r13_present) {
    j.write_str("ce-ModeB-r13", "supported");
  }
  if (s_tmsi_r13_present) {
    j.write_fieldname("s-TMSI-r13");
    s_tmsi_r13.to_json(j);
  }
  if (attach_without_pdn_connect_r13_present) {
    j.write_str("attachWithoutPDN-Connectivity-r13", "true");
  }
  if (up_cio_t_eps_optim_r13_present) {
    j.write_str("up-CIoT-EPS-Optimisation-r13", "true");
  }
  if (cp_cio_t_eps_optim_r13_present) {
    j.write_str("cp-CIoT-EPS-Optimisation-r13", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// TrafficPatternInfo-v1530 ::= SEQUENCE
SRSASN_CODE traffic_pattern_info_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(traffic_dest_r15_present, 1));
  HANDLE_CODE(bref.pack(reliability_info_sl_r15_present, 1));

  if (traffic_dest_r15_present) {
    HANDLE_CODE(traffic_dest_r15.pack(bref));
  }
  if (reliability_info_sl_r15_present) {
    HANDLE_CODE(pack_integer(bref, reliability_info_sl_r15, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE traffic_pattern_info_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(traffic_dest_r15_present, 1));
  HANDLE_CODE(bref.unpack(reliability_info_sl_r15_present, 1));

  if (traffic_dest_r15_present) {
    HANDLE_CODE(traffic_dest_r15.unpack(bref));
  }
  if (reliability_info_sl_r15_present) {
    HANDLE_CODE(unpack_integer(reliability_info_sl_r15, bref, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
void traffic_pattern_info_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (traffic_dest_r15_present) {
    j.write_str("trafficDestination-r15", traffic_dest_r15.to_string());
  }
  if (reliability_info_sl_r15_present) {
    j.write_int("reliabilityInfoSL-r15", reliability_info_sl_r15);
  }
  j.end_obj();
}

// UEInformationResponse-v1530-IEs ::= SEQUENCE
SRSASN_CODE ue_info_resp_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_result_list_idle_r15_present, 1));
  HANDLE_CODE(bref.pack(flight_path_info_report_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_result_list_idle_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_list_idle_r15, 1, 3));
  }
  if (flight_path_info_report_r15_present) {
    HANDLE_CODE(flight_path_info_report_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_resp_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_result_list_idle_r15_present, 1));
  HANDLE_CODE(bref.unpack(flight_path_info_report_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_result_list_idle_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_list_idle_r15, bref, 1, 3));
  }
  if (flight_path_info_report_r15_present) {
    HANDLE_CODE(flight_path_info_report_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_info_resp_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_result_list_idle_r15_present) {
    j.start_array("measResultListIdle-r15");
    for (const auto& e1 : meas_result_list_idle_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (flight_path_info_report_r15_present) {
    j.write_fieldname("flightPathInfoReport-r15");
    flight_path_info_report_r15.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// AffectedCarrierFreq-v1310 ::= SEQUENCE
SRSASN_CODE affected_carrier_freq_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(carrier_freq_v1310_present, 1));

  if (carrier_freq_v1310_present) {
    HANDLE_CODE(pack_integer(bref, carrier_freq_v1310, (uint8_t)33u, (uint8_t)64u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE affected_carrier_freq_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(carrier_freq_v1310_present, 1));

  if (carrier_freq_v1310_present) {
    HANDLE_CODE(unpack_integer(carrier_freq_v1310, bref, (uint8_t)33u, (uint8_t)64u));
  }

  return SRSASN_SUCCESS;
}
void affected_carrier_freq_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_freq_v1310_present) {
    j.write_int("carrierFreq-v1310", carrier_freq_v1310);
  }
  j.end_obj();
}

// ConnEstFailReport-r11 ::= SEQUENCE
SRSASN_CODE conn_est_fail_report_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(location_info_r11_present, 1));
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_r11_present, 1));
  HANDLE_CODE(bref.pack(meas_result_list_eutra_v1130_present, 1));

  HANDLE_CODE(failed_cell_id_r11.pack(bref));
  if (location_info_r11_present) {
    HANDLE_CODE(location_info_r11.pack(bref));
  }
  HANDLE_CODE(bref.pack(meas_result_failed_cell_r11.rsrq_result_r11_present, 1));
  HANDLE_CODE(pack_integer(bref, meas_result_failed_cell_r11.rsrp_result_r11, (uint8_t)0u, (uint8_t)97u));
  if (meas_result_failed_cell_r11.rsrq_result_r11_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_failed_cell_r11.rsrq_result_r11, (uint8_t)0u, (uint8_t)34u));
  }
  if (meas_result_neigh_cells_r11_present) {
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r11.meas_result_list_eutra_r11_present, 1));
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r11.meas_result_list_utra_r11_present, 1));
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r11.meas_result_list_geran_r11_present, 1));
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r11.meas_results_cdma2000_r11_present, 1));
    if (meas_result_neigh_cells_r11.meas_result_list_eutra_r11_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r11.meas_result_list_eutra_r11, 1, 8));
    }
    if (meas_result_neigh_cells_r11.meas_result_list_utra_r11_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r11.meas_result_list_utra_r11, 1, 8));
    }
    if (meas_result_neigh_cells_r11.meas_result_list_geran_r11_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r11.meas_result_list_geran_r11, 1, 8));
    }
    if (meas_result_neigh_cells_r11.meas_results_cdma2000_r11_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r11.meas_results_cdma2000_r11, 1, 8));
    }
  }
  HANDLE_CODE(pack_integer(bref, nof_preambs_sent_r11, (uint8_t)1u, (uint8_t)200u));
  HANDLE_CODE(bref.pack(contention_detected_r11, 1));
  HANDLE_CODE(bref.pack(max_tx_pwr_reached_r11, 1));
  HANDLE_CODE(pack_integer(bref, time_since_fail_r11, (uint32_t)0u, (uint32_t)172800u));
  if (meas_result_list_eutra_v1130_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_list_eutra_v1130, 1, 8));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_result_failed_cell_v1250_present;
    group_flags[0] |= failed_cell_rsrq_type_r12.is_present();
    group_flags[0] |= meas_result_list_eutra_v1250.is_present();
    group_flags[1] |= meas_result_failed_cell_v1360_present;
    group_flags[2] |= log_meas_result_list_bt_r15.is_present();
    group_flags[2] |= log_meas_result_list_wlan_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_failed_cell_v1250_present, 1));
      HANDLE_CODE(bref.pack(failed_cell_rsrq_type_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_list_eutra_v1250.is_present(), 1));
      if (meas_result_failed_cell_v1250_present) {
        HANDLE_CODE(pack_integer(bref, meas_result_failed_cell_v1250, (int8_t)-30, (int8_t)46));
      }
      if (failed_cell_rsrq_type_r12.is_present()) {
        HANDLE_CODE(failed_cell_rsrq_type_r12->pack(bref));
      }
      if (meas_result_list_eutra_v1250.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_list_eutra_v1250, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_failed_cell_v1360_present, 1));
      if (meas_result_failed_cell_v1360_present) {
        HANDLE_CODE(pack_integer(bref, meas_result_failed_cell_v1360, (int8_t)-17, (int8_t)-1));
      }
    }
    if (group_flags[2]) {
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
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE conn_est_fail_report_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(location_info_r11_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r11_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_list_eutra_v1130_present, 1));

  HANDLE_CODE(failed_cell_id_r11.unpack(bref));
  if (location_info_r11_present) {
    HANDLE_CODE(location_info_r11.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(meas_result_failed_cell_r11.rsrq_result_r11_present, 1));
  HANDLE_CODE(unpack_integer(meas_result_failed_cell_r11.rsrp_result_r11, bref, (uint8_t)0u, (uint8_t)97u));
  if (meas_result_failed_cell_r11.rsrq_result_r11_present) {
    HANDLE_CODE(unpack_integer(meas_result_failed_cell_r11.rsrq_result_r11, bref, (uint8_t)0u, (uint8_t)34u));
  }
  if (meas_result_neigh_cells_r11_present) {
    HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r11.meas_result_list_eutra_r11_present, 1));
    HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r11.meas_result_list_utra_r11_present, 1));
    HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r11.meas_result_list_geran_r11_present, 1));
    HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r11.meas_results_cdma2000_r11_present, 1));
    if (meas_result_neigh_cells_r11.meas_result_list_eutra_r11_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r11.meas_result_list_eutra_r11, bref, 1, 8));
    }
    if (meas_result_neigh_cells_r11.meas_result_list_utra_r11_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r11.meas_result_list_utra_r11, bref, 1, 8));
    }
    if (meas_result_neigh_cells_r11.meas_result_list_geran_r11_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r11.meas_result_list_geran_r11, bref, 1, 8));
    }
    if (meas_result_neigh_cells_r11.meas_results_cdma2000_r11_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r11.meas_results_cdma2000_r11, bref, 1, 8));
    }
  }
  HANDLE_CODE(unpack_integer(nof_preambs_sent_r11, bref, (uint8_t)1u, (uint8_t)200u));
  HANDLE_CODE(bref.unpack(contention_detected_r11, 1));
  HANDLE_CODE(bref.unpack(max_tx_pwr_reached_r11, 1));
  HANDLE_CODE(unpack_integer(time_since_fail_r11, bref, (uint32_t)0u, (uint32_t)172800u));
  if (meas_result_list_eutra_v1130_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_list_eutra_v1130, bref, 1, 8));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(meas_result_failed_cell_v1250_present, 1));
      bool failed_cell_rsrq_type_r12_present;
      HANDLE_CODE(bref.unpack(failed_cell_rsrq_type_r12_present, 1));
      failed_cell_rsrq_type_r12.set_present(failed_cell_rsrq_type_r12_present);
      bool meas_result_list_eutra_v1250_present;
      HANDLE_CODE(bref.unpack(meas_result_list_eutra_v1250_present, 1));
      meas_result_list_eutra_v1250.set_present(meas_result_list_eutra_v1250_present);
      if (meas_result_failed_cell_v1250_present) {
        HANDLE_CODE(unpack_integer(meas_result_failed_cell_v1250, bref, (int8_t)-30, (int8_t)46));
      }
      if (failed_cell_rsrq_type_r12.is_present()) {
        HANDLE_CODE(failed_cell_rsrq_type_r12->unpack(bref));
      }
      if (meas_result_list_eutra_v1250.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_list_eutra_v1250, bref, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(meas_result_failed_cell_v1360_present, 1));
      if (meas_result_failed_cell_v1360_present) {
        HANDLE_CODE(unpack_integer(meas_result_failed_cell_v1360, bref, (int8_t)-17, (int8_t)-1));
      }
    }
    if (group_flags[2]) {
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
  }
  return SRSASN_SUCCESS;
}
void conn_est_fail_report_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("failedCellId-r11");
  failed_cell_id_r11.to_json(j);
  if (location_info_r11_present) {
    j.write_fieldname("locationInfo-r11");
    location_info_r11.to_json(j);
  }
  j.write_fieldname("measResultFailedCell-r11");
  j.start_obj();
  j.write_int("rsrpResult-r11", meas_result_failed_cell_r11.rsrp_result_r11);
  if (meas_result_failed_cell_r11.rsrq_result_r11_present) {
    j.write_int("rsrqResult-r11", meas_result_failed_cell_r11.rsrq_result_r11);
  }
  j.end_obj();
  if (meas_result_neigh_cells_r11_present) {
    j.write_fieldname("measResultNeighCells-r11");
    j.start_obj();
    if (meas_result_neigh_cells_r11.meas_result_list_eutra_r11_present) {
      j.start_array("measResultListEUTRA-r11");
      for (const auto& e1 : meas_result_neigh_cells_r11.meas_result_list_eutra_r11) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_neigh_cells_r11.meas_result_list_utra_r11_present) {
      j.start_array("measResultListUTRA-r11");
      for (const auto& e1 : meas_result_neigh_cells_r11.meas_result_list_utra_r11) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_neigh_cells_r11.meas_result_list_geran_r11_present) {
      j.start_array("measResultListGERAN-r11");
      for (const auto& e1 : meas_result_neigh_cells_r11.meas_result_list_geran_r11) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_neigh_cells_r11.meas_results_cdma2000_r11_present) {
      j.start_array("measResultsCDMA2000-r11");
      for (const auto& e1 : meas_result_neigh_cells_r11.meas_results_cdma2000_r11) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  j.write_int("numberOfPreamblesSent-r11", nof_preambs_sent_r11);
  j.write_bool("contentionDetected-r11", contention_detected_r11);
  j.write_bool("maxTxPowerReached-r11", max_tx_pwr_reached_r11);
  j.write_int("timeSinceFailure-r11", time_since_fail_r11);
  if (meas_result_list_eutra_v1130_present) {
    j.start_array("measResultListEUTRA-v1130");
    for (const auto& e1 : meas_result_list_eutra_v1130) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (meas_result_failed_cell_v1250_present) {
      j.write_int("measResultFailedCell-v1250", meas_result_failed_cell_v1250);
    }
    if (failed_cell_rsrq_type_r12.is_present()) {
      j.write_fieldname("failedCellRSRQ-Type-r12");
      failed_cell_rsrq_type_r12->to_json(j);
    }
    if (meas_result_list_eutra_v1250.is_present()) {
      j.start_array("measResultListEUTRA-v1250");
      for (const auto& e1 : *meas_result_list_eutra_v1250) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_failed_cell_v1360_present) {
      j.write_int("measResultFailedCell-v1360", meas_result_failed_cell_v1360);
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
  }
  j.end_obj();
}

// DRB-CountInfo ::= SEQUENCE
SRSASN_CODE drb_count_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, count_ul, (uint64_t)0u, (uint64_t)4294967295u));
  HANDLE_CODE(pack_integer(bref, count_dl, (uint64_t)0u, (uint64_t)4294967295u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_count_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(count_ul, bref, (uint64_t)0u, (uint64_t)4294967295u));
  HANDLE_CODE(unpack_integer(count_dl, bref, (uint64_t)0u, (uint64_t)4294967295u));

  return SRSASN_SUCCESS;
}
void drb_count_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("drb-Identity", drb_id);
  j.write_int("count-Uplink", count_ul);
  j.write_int("count-Downlink", count_dl);
  j.end_obj();
}

// InDeviceCoexIndication-v1530-IEs ::= SEQUENCE
SRSASN_CODE in_dev_coex_ind_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mrdc_assist_info_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mrdc_assist_info_r15_present) {
    HANDLE_CODE(mrdc_assist_info_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE in_dev_coex_ind_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mrdc_assist_info_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mrdc_assist_info_r15_present) {
    HANDLE_CODE(mrdc_assist_info_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void in_dev_coex_ind_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mrdc_assist_info_r15_present) {
    j.write_fieldname("mrdc-AssistanceInfo-r15");
    mrdc_assist_info_r15.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RRCConnectionReconfigurationComplete-v1250-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_complete_v1250_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(log_meas_available_mbsfn_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_complete_v1250_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(log_meas_available_mbsfn_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_complete_v1250_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (log_meas_available_mbsfn_r12_present) {
    j.write_str("logMeasAvailableMBSFN-r12", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReestablishmentComplete-v1130-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_complete_v1130_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(conn_est_fail_info_available_r11_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_complete_v1130_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(conn_est_fail_info_available_r11_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_complete_v1130_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (conn_est_fail_info_available_r11_present) {
    j.write_str("connEstFailInfoAvailable-r11", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionSetupComplete-v1250-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_v1250_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mob_state_r12_present, 1));
  HANDLE_CODE(bref.pack(mob_history_avail_r12_present, 1));
  HANDLE_CODE(bref.pack(log_meas_available_mbsfn_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mob_state_r12_present) {
    HANDLE_CODE(mob_state_r12.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_v1250_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mob_state_r12_present, 1));
  HANDLE_CODE(bref.unpack(mob_history_avail_r12_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_available_mbsfn_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mob_state_r12_present) {
    HANDLE_CODE(mob_state_r12.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_complete_v1250_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mob_state_r12_present) {
    j.write_str("mobilityState-r12", mob_state_r12.to_string());
  }
  if (mob_history_avail_r12_present) {
    j.write_str("mobilityHistoryAvail-r12", "true");
  }
  if (log_meas_available_mbsfn_r12_present) {
    j.write_str("logMeasAvailableMBSFN-r12", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* rrc_conn_setup_complete_v1250_ies_s::mob_state_r12_opts::to_string() const
{
  static const char* options[] = {"normal", "medium", "high", "spare"};
  return convert_enum_idx(options, 4, value, "rrc_conn_setup_complete_v1250_ies_s::mob_state_r12_e_");
}

// SL-V2X-CommTxResourceReq-r14 ::= SEQUENCE
SRSASN_CODE sl_v2x_comm_tx_res_req_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(carrier_freq_comm_tx_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_type_tx_sync_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_dest_info_list_r14_present, 1));

  if (carrier_freq_comm_tx_r14_present) {
    HANDLE_CODE(pack_integer(bref, carrier_freq_comm_tx_r14, (uint8_t)0u, (uint8_t)7u));
  }
  if (v2x_type_tx_sync_r14_present) {
    HANDLE_CODE(v2x_type_tx_sync_r14.pack(bref));
  }
  if (v2x_dest_info_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_dest_info_list_r14, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_v2x_comm_tx_res_req_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(carrier_freq_comm_tx_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_type_tx_sync_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_dest_info_list_r14_present, 1));

  if (carrier_freq_comm_tx_r14_present) {
    HANDLE_CODE(unpack_integer(carrier_freq_comm_tx_r14, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (v2x_type_tx_sync_r14_present) {
    HANDLE_CODE(v2x_type_tx_sync_r14.unpack(bref));
  }
  if (v2x_dest_info_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_dest_info_list_r14, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void sl_v2x_comm_tx_res_req_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_freq_comm_tx_r14_present) {
    j.write_int("carrierFreqCommTx-r14", carrier_freq_comm_tx_r14);
  }
  if (v2x_type_tx_sync_r14_present) {
    j.write_str("v2x-TypeTxSync-r14", v2x_type_tx_sync_r14.to_string());
  }
  if (v2x_dest_info_list_r14_present) {
    j.start_array("v2x-DestinationInfoList-r14");
    for (const auto& e1 : v2x_dest_info_list_r14) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  j.end_obj();
}

// UEInformationResponse-v1250-IEs ::= SEQUENCE
SRSASN_CODE ue_info_resp_v1250_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mob_history_report_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mob_history_report_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mob_history_report_r12, 1, 16));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_resp_v1250_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mob_history_report_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mob_history_report_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mob_history_report_r12, bref, 1, 16));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_info_resp_v1250_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mob_history_report_r12_present) {
    j.start_array("mobilityHistoryReport-r12");
    for (const auto& e1 : mob_history_report_r12) {
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

// IDC-SubframePattern-r11 ::= CHOICE
void idc_sf_pattern_r11_c::destroy_()
{
  switch (type_) {
    case types::sf_pattern_fdd_r11:
      c.destroy<fixed_bitstring<4> >();
      break;
    case types::sf_pattern_tdd_r11:
      c.destroy<sf_pattern_tdd_r11_c_>();
      break;
    default:
      break;
  }
}
void idc_sf_pattern_r11_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sf_pattern_fdd_r11:
      c.init<fixed_bitstring<4> >();
      break;
    case types::sf_pattern_tdd_r11:
      c.init<sf_pattern_tdd_r11_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "idc_sf_pattern_r11_c");
  }
}
idc_sf_pattern_r11_c::idc_sf_pattern_r11_c(const idc_sf_pattern_r11_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf_pattern_fdd_r11:
      c.init(other.c.get<fixed_bitstring<4> >());
      break;
    case types::sf_pattern_tdd_r11:
      c.init(other.c.get<sf_pattern_tdd_r11_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "idc_sf_pattern_r11_c");
  }
}
idc_sf_pattern_r11_c& idc_sf_pattern_r11_c::operator=(const idc_sf_pattern_r11_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf_pattern_fdd_r11:
      c.set(other.c.get<fixed_bitstring<4> >());
      break;
    case types::sf_pattern_tdd_r11:
      c.set(other.c.get<sf_pattern_tdd_r11_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "idc_sf_pattern_r11_c");
  }

  return *this;
}
fixed_bitstring<4>& idc_sf_pattern_r11_c::set_sf_pattern_fdd_r11()
{
  set(types::sf_pattern_fdd_r11);
  return c.get<fixed_bitstring<4> >();
}
idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_& idc_sf_pattern_r11_c::set_sf_pattern_tdd_r11()
{
  set(types::sf_pattern_tdd_r11);
  return c.get<sf_pattern_tdd_r11_c_>();
}
void idc_sf_pattern_r11_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf_pattern_fdd_r11:
      j.write_str("subframePatternFDD-r11", c.get<fixed_bitstring<4> >().to_string());
      break;
    case types::sf_pattern_tdd_r11:
      j.write_fieldname("subframePatternTDD-r11");
      c.get<sf_pattern_tdd_r11_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "idc_sf_pattern_r11_c");
  }
  j.end_obj();
}
SRSASN_CODE idc_sf_pattern_r11_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf_pattern_fdd_r11:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().pack(bref));
      break;
    case types::sf_pattern_tdd_r11:
      HANDLE_CODE(c.get<sf_pattern_tdd_r11_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "idc_sf_pattern_r11_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE idc_sf_pattern_r11_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf_pattern_fdd_r11:
      HANDLE_CODE(c.get<fixed_bitstring<4> >().unpack(bref));
      break;
    case types::sf_pattern_tdd_r11:
      HANDLE_CODE(c.get<sf_pattern_tdd_r11_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "idc_sf_pattern_r11_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::destroy_()
{
  switch (type_) {
    case types::sf_cfg0_r11:
      c.destroy<fixed_bitstring<70> >();
      break;
    case types::sf_cfg1_minus5_r11:
      c.destroy<fixed_bitstring<10> >();
      break;
    case types::sf_cfg6_r11:
      c.destroy<fixed_bitstring<60> >();
      break;
    default:
      break;
  }
}
void idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sf_cfg0_r11:
      c.init<fixed_bitstring<70> >();
      break;
    case types::sf_cfg1_minus5_r11:
      c.init<fixed_bitstring<10> >();
      break;
    case types::sf_cfg6_r11:
      c.init<fixed_bitstring<60> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_");
  }
}
idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::sf_pattern_tdd_r11_c_(
    const idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf_cfg0_r11:
      c.init(other.c.get<fixed_bitstring<70> >());
      break;
    case types::sf_cfg1_minus5_r11:
      c.init(other.c.get<fixed_bitstring<10> >());
      break;
    case types::sf_cfg6_r11:
      c.init(other.c.get<fixed_bitstring<60> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_");
  }
}
idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_&
idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::operator=(const idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf_cfg0_r11:
      c.set(other.c.get<fixed_bitstring<70> >());
      break;
    case types::sf_cfg1_minus5_r11:
      c.set(other.c.get<fixed_bitstring<10> >());
      break;
    case types::sf_cfg6_r11:
      c.set(other.c.get<fixed_bitstring<60> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_");
  }

  return *this;
}
fixed_bitstring<70>& idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::set_sf_cfg0_r11()
{
  set(types::sf_cfg0_r11);
  return c.get<fixed_bitstring<70> >();
}
fixed_bitstring<10>& idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::set_sf_cfg1_minus5_r11()
{
  set(types::sf_cfg1_minus5_r11);
  return c.get<fixed_bitstring<10> >();
}
fixed_bitstring<60>& idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::set_sf_cfg6_r11()
{
  set(types::sf_cfg6_r11);
  return c.get<fixed_bitstring<60> >();
}
void idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf_cfg0_r11:
      j.write_str("subframeConfig0-r11", c.get<fixed_bitstring<70> >().to_string());
      break;
    case types::sf_cfg1_minus5_r11:
      j.write_str("subframeConfig1-5-r11", c.get<fixed_bitstring<10> >().to_string());
      break;
    case types::sf_cfg6_r11:
      j.write_str("subframeConfig6-r11", c.get<fixed_bitstring<60> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf_cfg0_r11:
      HANDLE_CODE(c.get<fixed_bitstring<70> >().pack(bref));
      break;
    case types::sf_cfg1_minus5_r11:
      HANDLE_CODE(c.get<fixed_bitstring<10> >().pack(bref));
      break;
    case types::sf_cfg6_r11:
      HANDLE_CODE(c.get<fixed_bitstring<60> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf_cfg0_r11:
      HANDLE_CODE(c.get<fixed_bitstring<70> >().unpack(bref));
      break;
    case types::sf_cfg1_minus5_r11:
      HANDLE_CODE(c.get<fixed_bitstring<10> >().unpack(bref));
      break;
    case types::sf_cfg6_r11:
      HANDLE_CODE(c.get<fixed_bitstring<60> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// InDeviceCoexIndication-v1360-IEs ::= SEQUENCE
SRSASN_CODE in_dev_coex_ind_v1360_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(hardware_sharing_problem_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE in_dev_coex_ind_v1360_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(hardware_sharing_problem_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void in_dev_coex_ind_v1360_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (hardware_sharing_problem_r13_present) {
    j.write_str("hardwareSharingProblem-r13", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MBMS-ROM-Info-r15 ::= SEQUENCE
SRSASN_CODE mbms_rom_info_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, mbms_rom_freq_r15, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(mbms_rom_subcarrier_spacing_r15.pack(bref));
  HANDLE_CODE(mbms_bw_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_rom_info_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(mbms_rom_freq_r15, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(mbms_rom_subcarrier_spacing_r15.unpack(bref));
  HANDLE_CODE(mbms_bw_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void mbms_rom_info_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("mbms-ROM-Freq-r15", mbms_rom_freq_r15);
  j.write_str("mbms-ROM-SubcarrierSpacing-r15", mbms_rom_subcarrier_spacing_r15.to_string());
  j.write_str("mbms-Bandwidth-r15", mbms_bw_r15.to_string());
  j.end_obj();
}

const char* mbms_rom_info_r15_s::mbms_rom_subcarrier_spacing_r15_opts::to_string() const
{
  static const char* options[] = {"kHz15", "kHz7dot5", "kHz1dot25"};
  return convert_enum_idx(options, 3, value, "mbms_rom_info_r15_s::mbms_rom_subcarrier_spacing_r15_e_");
}
float mbms_rom_info_r15_s::mbms_rom_subcarrier_spacing_r15_opts::to_number() const
{
  static const float options[] = {15.0, 7.5, 1.25};
  return map_enum_number(options, 3, value, "mbms_rom_info_r15_s::mbms_rom_subcarrier_spacing_r15_e_");
}
const char* mbms_rom_info_r15_s::mbms_rom_subcarrier_spacing_r15_opts::to_number_string() const
{
  static const char* options[] = {"15", "7.5", "1.25"};
  return convert_enum_idx(options, 3, value, "mbms_rom_info_r15_s::mbms_rom_subcarrier_spacing_r15_e_");
}

const char* mbms_rom_info_r15_s::mbms_bw_r15_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "mbms_rom_info_r15_s::mbms_bw_r15_e_");
}
uint8_t mbms_rom_info_r15_s::mbms_bw_r15_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "mbms_rom_info_r15_s::mbms_bw_r15_e_");
}

// MBMS-ServiceInfo-r13 ::= SEQUENCE
SRSASN_CODE mbms_service_info_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(tmgi_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_service_info_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(tmgi_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void mbms_service_info_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tmgi-r13");
  tmgi_r13.to_json(j);
  j.end_obj();
}

// MeasResultFreqFailNR-r15 ::= SEQUENCE
SRSASN_CODE meas_result_freq_fail_nr_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_cell_list_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r15, (uint32_t)0u, (uint32_t)3279165u));
  if (meas_result_cell_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_cell_list_r15, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_freq_fail_nr_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_cell_list_r15_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r15, bref, (uint32_t)0u, (uint32_t)3279165u));
  if (meas_result_cell_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_cell_list_r15, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void meas_result_freq_fail_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r15", carrier_freq_r15);
  if (meas_result_cell_list_r15_present) {
    j.start_array("measResultCellList-r15");
    for (const auto& e1 : meas_result_cell_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// OverheatingAssistance-r14 ::= SEQUENCE
SRSASN_CODE overheat_assist_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(reduced_ue_category_present, 1));
  HANDLE_CODE(bref.pack(reduced_max_ccs_present, 1));

  if (reduced_ue_category_present) {
    HANDLE_CODE(pack_integer(bref, reduced_ue_category.reduced_ue_category_dl, (uint8_t)0u, (uint8_t)19u));
    HANDLE_CODE(pack_integer(bref, reduced_ue_category.reduced_ue_category_ul, (uint8_t)0u, (uint8_t)21u));
  }
  if (reduced_max_ccs_present) {
    HANDLE_CODE(pack_integer(bref, reduced_max_ccs.reduced_ccs_dl, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(pack_integer(bref, reduced_max_ccs.reduced_ccs_ul, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE overheat_assist_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(reduced_ue_category_present, 1));
  HANDLE_CODE(bref.unpack(reduced_max_ccs_present, 1));

  if (reduced_ue_category_present) {
    HANDLE_CODE(unpack_integer(reduced_ue_category.reduced_ue_category_dl, bref, (uint8_t)0u, (uint8_t)19u));
    HANDLE_CODE(unpack_integer(reduced_ue_category.reduced_ue_category_ul, bref, (uint8_t)0u, (uint8_t)21u));
  }
  if (reduced_max_ccs_present) {
    HANDLE_CODE(unpack_integer(reduced_max_ccs.reduced_ccs_dl, bref, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(unpack_integer(reduced_max_ccs.reduced_ccs_ul, bref, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
void overheat_assist_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reduced_ue_category_present) {
    j.write_fieldname("reducedUE-Category");
    j.start_obj();
    j.write_int("reducedUE-CategoryDL", reduced_ue_category.reduced_ue_category_dl);
    j.write_int("reducedUE-CategoryUL", reduced_ue_category.reduced_ue_category_ul);
    j.end_obj();
  }
  if (reduced_max_ccs_present) {
    j.write_fieldname("reducedMaxCCs");
    j.start_obj();
    j.write_int("reducedCCsDL", reduced_max_ccs.reduced_ccs_dl);
    j.write_int("reducedCCsUL", reduced_max_ccs.reduced_ccs_ul);
    j.end_obj();
  }
  j.end_obj();
}

// RRCConnectionReconfigurationComplete-v1130-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_complete_v1130_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(conn_est_fail_info_available_r11_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_complete_v1130_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(conn_est_fail_info_available_r11_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_complete_v1130_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (conn_est_fail_info_available_r11_present) {
    j.write_str("connEstFailInfoAvailable-r11", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReestablishmentComplete-v1020-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_complete_v1020_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(log_meas_available_r10_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_complete_v1020_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(log_meas_available_r10_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_complete_v1020_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (log_meas_available_r10_present) {
    j.write_str("logMeasAvailable-r10", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionSetupComplete-v1130-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_v1130_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(conn_est_fail_info_available_r11_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_v1130_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(conn_est_fail_info_available_r11_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_complete_v1130_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (conn_est_fail_info_available_r11_present) {
    j.write_str("connEstFailInfoAvailable-r11", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SL-DiscSysInfoReport-r13 ::= SEQUENCE
SRSASN_CODE sl_disc_sys_info_report_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(plmn_id_list_r13_present, 1));
  HANDLE_CODE(bref.pack(cell_id_minus13_present, 1));
  HANDLE_CODE(bref.pack(carrier_freq_info_minus13_present, 1));
  HANDLE_CODE(bref.pack(disc_rx_res_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_tx_pool_common_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_tx_pwr_info_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_sync_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_cell_sel_info_r13_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_info_r13_present, 1));
  HANDLE_CODE(bref.pack(tdd_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(freq_info_r13_present, 1));
  HANDLE_CODE(bref.pack(p_max_r13_present, 1));
  HANDLE_CODE(bref.pack(ref_sig_pwr_r13_present, 1));

  if (plmn_id_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_r13, 1, 6));
  }
  if (cell_id_minus13_present) {
    HANDLE_CODE(cell_id_minus13.pack(bref));
  }
  if (carrier_freq_info_minus13_present) {
    HANDLE_CODE(pack_integer(bref, carrier_freq_info_minus13, (uint32_t)0u, (uint32_t)262143u));
  }
  if (disc_rx_res_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, disc_rx_res_r13, 1, 16));
  }
  if (disc_tx_pool_common_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, disc_tx_pool_common_r13, 1, 4));
  }
  if (disc_tx_pwr_info_r13_present) {
    HANDLE_CODE(pack_fixed_seq_of(bref, &(disc_tx_pwr_info_r13)[0], disc_tx_pwr_info_r13.size()));
  }
  if (disc_sync_cfg_r13_present) {
    HANDLE_CODE(disc_sync_cfg_r13.pack(bref));
  }
  if (disc_cell_sel_info_r13_present) {
    HANDLE_CODE(bref.pack(disc_cell_sel_info_r13.q_rx_lev_min_offset_r13_present, 1));
    HANDLE_CODE(pack_integer(bref, disc_cell_sel_info_r13.q_rx_lev_min_r13, (int8_t)-70, (int8_t)-22));
    if (disc_cell_sel_info_r13.q_rx_lev_min_offset_r13_present) {
      HANDLE_CODE(pack_integer(bref, disc_cell_sel_info_r13.q_rx_lev_min_offset_r13, (uint8_t)1u, (uint8_t)8u));
    }
  }
  if (cell_resel_info_r13_present) {
    HANDLE_CODE(cell_resel_info_r13.q_hyst_r13.pack(bref));
    HANDLE_CODE(pack_integer(bref, cell_resel_info_r13.q_rx_lev_min_r13, (int8_t)-70, (int8_t)-22));
    HANDLE_CODE(pack_integer(bref, cell_resel_info_r13.t_resel_eutra_r13, (uint8_t)0u, (uint8_t)7u));
  }
  if (tdd_cfg_r13_present) {
    HANDLE_CODE(tdd_cfg_r13.pack(bref));
  }
  if (freq_info_r13_present) {
    HANDLE_CODE(bref.pack(freq_info_r13.ul_carrier_freq_r13_present, 1));
    HANDLE_CODE(bref.pack(freq_info_r13.ul_bw_r13_present, 1));
    HANDLE_CODE(bref.pack(freq_info_r13.add_spec_emission_r13_present, 1));
    if (freq_info_r13.ul_carrier_freq_r13_present) {
      HANDLE_CODE(pack_integer(bref, freq_info_r13.ul_carrier_freq_r13, (uint32_t)0u, (uint32_t)65535u));
    }
    if (freq_info_r13.ul_bw_r13_present) {
      HANDLE_CODE(freq_info_r13.ul_bw_r13.pack(bref));
    }
    if (freq_info_r13.add_spec_emission_r13_present) {
      HANDLE_CODE(pack_integer(bref, freq_info_r13.add_spec_emission_r13, (uint8_t)1u, (uint8_t)32u));
    }
  }
  if (p_max_r13_present) {
    HANDLE_CODE(pack_integer(bref, p_max_r13, (int8_t)-30, (int8_t)33));
  }
  if (ref_sig_pwr_r13_present) {
    HANDLE_CODE(pack_integer(bref, ref_sig_pwr_r13, (int8_t)-60, (int8_t)50));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= freq_info_v1370.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(freq_info_v1370.is_present(), 1));
      if (freq_info_v1370.is_present()) {
        HANDLE_CODE(pack_integer(bref, freq_info_v1370->add_spec_emission_v1370, (uint16_t)33u, (uint16_t)288u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_sys_info_report_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(plmn_id_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(cell_id_minus13_present, 1));
  HANDLE_CODE(bref.unpack(carrier_freq_info_minus13_present, 1));
  HANDLE_CODE(bref.unpack(disc_rx_res_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_tx_pool_common_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_tx_pwr_info_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_sync_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_cell_sel_info_r13_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_info_r13_present, 1));
  HANDLE_CODE(bref.unpack(tdd_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(freq_info_r13_present, 1));
  HANDLE_CODE(bref.unpack(p_max_r13_present, 1));
  HANDLE_CODE(bref.unpack(ref_sig_pwr_r13_present, 1));

  if (plmn_id_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_r13, bref, 1, 6));
  }
  if (cell_id_minus13_present) {
    HANDLE_CODE(cell_id_minus13.unpack(bref));
  }
  if (carrier_freq_info_minus13_present) {
    HANDLE_CODE(unpack_integer(carrier_freq_info_minus13, bref, (uint32_t)0u, (uint32_t)262143u));
  }
  if (disc_rx_res_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(disc_rx_res_r13, bref, 1, 16));
  }
  if (disc_tx_pool_common_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(disc_tx_pool_common_r13, bref, 1, 4));
  }
  if (disc_tx_pwr_info_r13_present) {
    HANDLE_CODE(unpack_fixed_seq_of(&(disc_tx_pwr_info_r13)[0], bref, disc_tx_pwr_info_r13.size()));
  }
  if (disc_sync_cfg_r13_present) {
    HANDLE_CODE(disc_sync_cfg_r13.unpack(bref));
  }
  if (disc_cell_sel_info_r13_present) {
    HANDLE_CODE(bref.unpack(disc_cell_sel_info_r13.q_rx_lev_min_offset_r13_present, 1));
    HANDLE_CODE(unpack_integer(disc_cell_sel_info_r13.q_rx_lev_min_r13, bref, (int8_t)-70, (int8_t)-22));
    if (disc_cell_sel_info_r13.q_rx_lev_min_offset_r13_present) {
      HANDLE_CODE(unpack_integer(disc_cell_sel_info_r13.q_rx_lev_min_offset_r13, bref, (uint8_t)1u, (uint8_t)8u));
    }
  }
  if (cell_resel_info_r13_present) {
    HANDLE_CODE(cell_resel_info_r13.q_hyst_r13.unpack(bref));
    HANDLE_CODE(unpack_integer(cell_resel_info_r13.q_rx_lev_min_r13, bref, (int8_t)-70, (int8_t)-22));
    HANDLE_CODE(unpack_integer(cell_resel_info_r13.t_resel_eutra_r13, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (tdd_cfg_r13_present) {
    HANDLE_CODE(tdd_cfg_r13.unpack(bref));
  }
  if (freq_info_r13_present) {
    HANDLE_CODE(bref.unpack(freq_info_r13.ul_carrier_freq_r13_present, 1));
    HANDLE_CODE(bref.unpack(freq_info_r13.ul_bw_r13_present, 1));
    HANDLE_CODE(bref.unpack(freq_info_r13.add_spec_emission_r13_present, 1));
    if (freq_info_r13.ul_carrier_freq_r13_present) {
      HANDLE_CODE(unpack_integer(freq_info_r13.ul_carrier_freq_r13, bref, (uint32_t)0u, (uint32_t)65535u));
    }
    if (freq_info_r13.ul_bw_r13_present) {
      HANDLE_CODE(freq_info_r13.ul_bw_r13.unpack(bref));
    }
    if (freq_info_r13.add_spec_emission_r13_present) {
      HANDLE_CODE(unpack_integer(freq_info_r13.add_spec_emission_r13, bref, (uint8_t)1u, (uint8_t)32u));
    }
  }
  if (p_max_r13_present) {
    HANDLE_CODE(unpack_integer(p_max_r13, bref, (int8_t)-30, (int8_t)33));
  }
  if (ref_sig_pwr_r13_present) {
    HANDLE_CODE(unpack_integer(ref_sig_pwr_r13, bref, (int8_t)-60, (int8_t)50));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool freq_info_v1370_present;
      HANDLE_CODE(bref.unpack(freq_info_v1370_present, 1));
      freq_info_v1370.set_present(freq_info_v1370_present);
      if (freq_info_v1370.is_present()) {
        HANDLE_CODE(unpack_integer(freq_info_v1370->add_spec_emission_v1370, bref, (uint16_t)33u, (uint16_t)288u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_disc_sys_info_report_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_list_r13_present) {
    j.start_array("plmn-IdentityList-r13");
    for (const auto& e1 : plmn_id_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cell_id_minus13_present) {
    j.write_str("cellIdentity-13", cell_id_minus13.to_string());
  }
  if (carrier_freq_info_minus13_present) {
    j.write_int("carrierFreqInfo-13", carrier_freq_info_minus13);
  }
  if (disc_rx_res_r13_present) {
    j.start_array("discRxResources-r13");
    for (const auto& e1 : disc_rx_res_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (disc_tx_pool_common_r13_present) {
    j.start_array("discTxPoolCommon-r13");
    for (const auto& e1 : disc_tx_pool_common_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (disc_tx_pwr_info_r13_present) {
    j.start_array("discTxPowerInfo-r13");
    for (const auto& e1 : disc_tx_pwr_info_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (disc_sync_cfg_r13_present) {
    j.write_fieldname("discSyncConfig-r13");
    disc_sync_cfg_r13.to_json(j);
  }
  if (disc_cell_sel_info_r13_present) {
    j.write_fieldname("discCellSelectionInfo-r13");
    j.start_obj();
    j.write_int("q-RxLevMin-r13", disc_cell_sel_info_r13.q_rx_lev_min_r13);
    if (disc_cell_sel_info_r13.q_rx_lev_min_offset_r13_present) {
      j.write_int("q-RxLevMinOffset-r13", disc_cell_sel_info_r13.q_rx_lev_min_offset_r13);
    }
    j.end_obj();
  }
  if (cell_resel_info_r13_present) {
    j.write_fieldname("cellReselectionInfo-r13");
    j.start_obj();
    j.write_str("q-Hyst-r13", cell_resel_info_r13.q_hyst_r13.to_string());
    j.write_int("q-RxLevMin-r13", cell_resel_info_r13.q_rx_lev_min_r13);
    j.write_int("t-ReselectionEUTRA-r13", cell_resel_info_r13.t_resel_eutra_r13);
    j.end_obj();
  }
  if (tdd_cfg_r13_present) {
    j.write_fieldname("tdd-Config-r13");
    tdd_cfg_r13.to_json(j);
  }
  if (freq_info_r13_present) {
    j.write_fieldname("freqInfo-r13");
    j.start_obj();
    if (freq_info_r13.ul_carrier_freq_r13_present) {
      j.write_int("ul-CarrierFreq-r13", freq_info_r13.ul_carrier_freq_r13);
    }
    if (freq_info_r13.ul_bw_r13_present) {
      j.write_str("ul-Bandwidth-r13", freq_info_r13.ul_bw_r13.to_string());
    }
    if (freq_info_r13.add_spec_emission_r13_present) {
      j.write_int("additionalSpectrumEmission-r13", freq_info_r13.add_spec_emission_r13);
    }
    j.end_obj();
  }
  if (p_max_r13_present) {
    j.write_int("p-Max-r13", p_max_r13);
  }
  if (ref_sig_pwr_r13_present) {
    j.write_int("referenceSignalPower-r13", ref_sig_pwr_r13);
  }
  if (ext) {
    if (freq_info_v1370.is_present()) {
      j.write_fieldname("freqInfo-v1370");
      j.start_obj();
      j.write_int("additionalSpectrumEmission-v1370", freq_info_v1370->add_spec_emission_v1370);
      j.end_obj();
    }
  }
  j.end_obj();
}

const char* sl_disc_sys_info_report_r13_s::cell_resel_info_r13_s_::q_hyst_r13_opts::to_string() const
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
  return convert_enum_idx(options, 16, value, "sl_disc_sys_info_report_r13_s::cell_resel_info_r13_s_::q_hyst_r13_e_");
}
uint8_t sl_disc_sys_info_report_r13_s::cell_resel_info_r13_s_::q_hyst_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(options, 16, value, "sl_disc_sys_info_report_r13_s::cell_resel_info_r13_s_::q_hyst_r13_e_");
}

const char* sl_disc_sys_info_report_r13_s::freq_info_r13_s_::ul_bw_r13_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "sl_disc_sys_info_report_r13_s::freq_info_r13_s_::ul_bw_r13_e_");
}
uint8_t sl_disc_sys_info_report_r13_s::freq_info_r13_s_::ul_bw_r13_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "sl_disc_sys_info_report_r13_s::freq_info_r13_s_::ul_bw_r13_e_");
}

// SL-DiscTxResourceReq-r13 ::= SEQUENCE
SRSASN_CODE sl_disc_tx_res_req_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(carrier_freq_disc_tx_r13_present, 1));

  if (carrier_freq_disc_tx_r13_present) {
    HANDLE_CODE(pack_integer(bref, carrier_freq_disc_tx_r13, (uint8_t)1u, (uint8_t)8u));
  }
  HANDLE_CODE(pack_integer(bref, disc_tx_res_req_r13, (uint8_t)1u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_tx_res_req_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(carrier_freq_disc_tx_r13_present, 1));

  if (carrier_freq_disc_tx_r13_present) {
    HANDLE_CODE(unpack_integer(carrier_freq_disc_tx_r13, bref, (uint8_t)1u, (uint8_t)8u));
  }
  HANDLE_CODE(unpack_integer(disc_tx_res_req_r13, bref, (uint8_t)1u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
void sl_disc_tx_res_req_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_freq_disc_tx_r13_present) {
    j.write_int("carrierFreqDiscTx-r13", carrier_freq_disc_tx_r13);
  }
  j.write_int("discTxResourceReq-r13", disc_tx_res_req_r13);
  j.end_obj();
}

// SL-GapFreqInfo-r13 ::= SEQUENCE
SRSASN_CODE sl_gap_freq_info_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(carrier_freq_r13_present, 1));

  if (carrier_freq_r13_present) {
    HANDLE_CODE(pack_integer(bref, carrier_freq_r13, (uint32_t)0u, (uint32_t)262143u));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, gap_pattern_list_r13, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_gap_freq_info_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(carrier_freq_r13_present, 1));

  if (carrier_freq_r13_present) {
    HANDLE_CODE(unpack_integer(carrier_freq_r13, bref, (uint32_t)0u, (uint32_t)262143u));
  }
  HANDLE_CODE(unpack_dyn_seq_of(gap_pattern_list_r13, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void sl_gap_freq_info_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_freq_r13_present) {
    j.write_int("carrierFreq-r13", carrier_freq_r13);
  }
  j.start_array("gapPatternList-r13");
  for (const auto& e1 : gap_pattern_list_r13) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// SidelinkUEInformation-v1530-IEs ::= SEQUENCE
SRSASN_CODE sidelink_ue_info_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(reliability_info_list_sl_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (reliability_info_list_sl_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, reliability_info_list_sl_r15, 1, 8, integer_packer<uint8_t>(1, 8)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sidelink_ue_info_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(reliability_info_list_sl_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (reliability_info_list_sl_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(reliability_info_list_sl_r15, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  }

  return SRSASN_SUCCESS;
}
void sidelink_ue_info_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reliability_info_list_sl_r15_present) {
    j.start_array("reliabilityInfoListSL-r15");
    for (const auto& e1 : reliability_info_list_sl_r15) {
      j.write_int(e1);
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

// TrafficPatternInfo-r14 ::= SEQUENCE
SRSASN_CODE traffic_pattern_info_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(prio_info_sl_r14_present, 1));
  HANDLE_CODE(bref.pack(lc_ch_id_ul_r14_present, 1));

  HANDLE_CODE(traffic_periodicity_r14.pack(bref));
  HANDLE_CODE(pack_integer(bref, timing_offset_r14, (uint16_t)0u, (uint16_t)10239u));
  if (prio_info_sl_r14_present) {
    HANDLE_CODE(pack_integer(bref, prio_info_sl_r14, (uint8_t)1u, (uint8_t)8u));
  }
  if (lc_ch_id_ul_r14_present) {
    HANDLE_CODE(pack_integer(bref, lc_ch_id_ul_r14, (uint8_t)3u, (uint8_t)10u));
  }
  HANDLE_CODE(msg_size_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE traffic_pattern_info_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(prio_info_sl_r14_present, 1));
  HANDLE_CODE(bref.unpack(lc_ch_id_ul_r14_present, 1));

  HANDLE_CODE(traffic_periodicity_r14.unpack(bref));
  HANDLE_CODE(unpack_integer(timing_offset_r14, bref, (uint16_t)0u, (uint16_t)10239u));
  if (prio_info_sl_r14_present) {
    HANDLE_CODE(unpack_integer(prio_info_sl_r14, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (lc_ch_id_ul_r14_present) {
    HANDLE_CODE(unpack_integer(lc_ch_id_ul_r14, bref, (uint8_t)3u, (uint8_t)10u));
  }
  HANDLE_CODE(msg_size_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void traffic_pattern_info_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("trafficPeriodicity-r14", traffic_periodicity_r14.to_string());
  j.write_int("timingOffset-r14", timing_offset_r14);
  if (prio_info_sl_r14_present) {
    j.write_int("priorityInfoSL-r14", prio_info_sl_r14);
  }
  if (lc_ch_id_ul_r14_present) {
    j.write_int("logicalChannelIdentityUL-r14", lc_ch_id_ul_r14);
  }
  j.write_str("messageSize-r14", msg_size_r14.to_string());
  j.end_obj();
}

const char* traffic_pattern_info_r14_s::traffic_periodicity_r14_opts::to_string() const
{
  static const char* options[] = {
      "sf20", "sf50", "sf100", "sf200", "sf300", "sf400", "sf500", "sf600", "sf700", "sf800", "sf900", "sf1000"};
  return convert_enum_idx(options, 12, value, "traffic_pattern_info_r14_s::traffic_periodicity_r14_e_");
}
uint16_t traffic_pattern_info_r14_s::traffic_periodicity_r14_opts::to_number() const
{
  static const uint16_t options[] = {20, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return map_enum_number(options, 12, value, "traffic_pattern_info_r14_s::traffic_periodicity_r14_e_");
}

// UEAssistanceInformation-v1530-IEs ::= SEQUENCE
SRSASN_CODE ueassist_info_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sps_assist_info_v1530_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sps_assist_info_v1530_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sps_assist_info_v1530.traffic_pattern_info_list_sl_v1530, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueassist_info_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sps_assist_info_v1530_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sps_assist_info_v1530_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sps_assist_info_v1530.traffic_pattern_info_list_sl_v1530, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void ueassist_info_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sps_assist_info_v1530_present) {
    j.write_fieldname("sps-AssistanceInformation-v1530");
    j.start_obj();
    j.start_array("trafficPatternInfoListSL-v1530");
    for (const auto& e1 : sps_assist_info_v1530.traffic_pattern_info_list_sl_v1530) {
      e1.to_json(j);
    }
    j.end_array();
    j.end_obj();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UEInformationResponse-v1130-IEs ::= SEQUENCE
SRSASN_CODE ue_info_resp_v1130_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(conn_est_fail_report_r11_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (conn_est_fail_report_r11_present) {
    HANDLE_CODE(conn_est_fail_report_r11.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_resp_v1130_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(conn_est_fail_report_r11_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (conn_est_fail_report_r11_present) {
    HANDLE_CODE(conn_est_fail_report_r11.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_info_resp_v1130_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (conn_est_fail_report_r11_present) {
    j.write_fieldname("connEstFailReport-r11");
    conn_est_fail_report_r11.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// AffectedCarrierFreq-r11 ::= SEQUENCE
SRSASN_CODE affected_carrier_freq_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, carrier_freq_r11, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(interference_direction_r11.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE affected_carrier_freq_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(carrier_freq_r11, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(interference_direction_r11.unpack(bref));

  return SRSASN_SUCCESS;
}
void affected_carrier_freq_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r11", carrier_freq_r11);
  j.write_str("interferenceDirection-r11", interference_direction_r11.to_string());
  j.end_obj();
}

const char* affected_carrier_freq_r11_s::interference_direction_r11_opts::to_string() const
{
  static const char* options[] = {"eutra", "other", "both", "spare"};
  return convert_enum_idx(options, 4, value, "affected_carrier_freq_r11_s::interference_direction_r11_e_");
}

// BW-Preference-r14 ::= SEQUENCE
SRSASN_CODE bw_pref_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dl_pref_r14_present, 1));
  HANDLE_CODE(bref.pack(ul_pref_r14_present, 1));

  if (dl_pref_r14_present) {
    HANDLE_CODE(dl_pref_r14.pack(bref));
  }
  if (ul_pref_r14_present) {
    HANDLE_CODE(ul_pref_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bw_pref_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dl_pref_r14_present, 1));
  HANDLE_CODE(bref.unpack(ul_pref_r14_present, 1));

  if (dl_pref_r14_present) {
    HANDLE_CODE(dl_pref_r14.unpack(bref));
  }
  if (ul_pref_r14_present) {
    HANDLE_CODE(ul_pref_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void bw_pref_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_pref_r14_present) {
    j.write_str("dl-Preference-r14", dl_pref_r14.to_string());
  }
  if (ul_pref_r14_present) {
    j.write_str("ul-Preference-r14", ul_pref_r14.to_string());
  }
  j.end_obj();
}

const char* bw_pref_r14_s::dl_pref_r14_opts::to_string() const
{
  static const char* options[] = {"mhz1dot4", "mhz5", "mhz20"};
  return convert_enum_idx(options, 3, value, "bw_pref_r14_s::dl_pref_r14_e_");
}
float bw_pref_r14_s::dl_pref_r14_opts::to_number() const
{
  static const float options[] = {1.4, 5.0, 20.0};
  return map_enum_number(options, 3, value, "bw_pref_r14_s::dl_pref_r14_e_");
}
const char* bw_pref_r14_s::dl_pref_r14_opts::to_number_string() const
{
  static const char* options[] = {"1.4", "5", "20"};
  return convert_enum_idx(options, 3, value, "bw_pref_r14_s::dl_pref_r14_e_");
}

const char* bw_pref_r14_s::ul_pref_r14_opts::to_string() const
{
  static const char* options[] = {"mhz1dot4", "mhz5"};
  return convert_enum_idx(options, 2, value, "bw_pref_r14_s::ul_pref_r14_e_");
}
float bw_pref_r14_s::ul_pref_r14_opts::to_number() const
{
  static const float options[] = {1.4, 5.0};
  return map_enum_number(options, 2, value, "bw_pref_r14_s::ul_pref_r14_e_");
}
const char* bw_pref_r14_s::ul_pref_r14_opts::to_number_string() const
{
  static const char* options[] = {"1.4", "5"};
  return convert_enum_idx(options, 2, value, "bw_pref_r14_s::ul_pref_r14_e_");
}

// CounterCheckResponse-v1530-IEs ::= SEQUENCE
SRSASN_CODE counter_check_resp_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(drb_count_info_list_ext_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (drb_count_info_list_ext_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_count_info_list_ext_r15, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_resp_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(drb_count_info_list_ext_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (drb_count_info_list_ext_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_count_info_list_ext_r15, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void counter_check_resp_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drb_count_info_list_ext_r15_present) {
    j.start_array("drb-CountInfoListExt-r15");
    for (const auto& e1 : drb_count_info_list_ext_r15) {
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

// CountingResponseInfo-r10 ::= SEQUENCE
SRSASN_CODE count_resp_info_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, count_resp_service_r10, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE count_resp_info_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(count_resp_service_r10, bref, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
void count_resp_info_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("countingResponseService-r10", count_resp_service_r10);
  j.end_obj();
}

// DelayBudgetReport-r14 ::= CHOICE
void delay_budget_report_r14_c::destroy_() {}
void delay_budget_report_r14_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
delay_budget_report_r14_c::delay_budget_report_r14_c(const delay_budget_report_r14_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::type1:
      c.init(other.c.get<type1_e_>());
      break;
    case types::type2:
      c.init(other.c.get<type2_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "delay_budget_report_r14_c");
  }
}
delay_budget_report_r14_c& delay_budget_report_r14_c::operator=(const delay_budget_report_r14_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::type1:
      c.set(other.c.get<type1_e_>());
      break;
    case types::type2:
      c.set(other.c.get<type2_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "delay_budget_report_r14_c");
  }

  return *this;
}
delay_budget_report_r14_c::type1_e_& delay_budget_report_r14_c::set_type1()
{
  set(types::type1);
  return c.get<type1_e_>();
}
delay_budget_report_r14_c::type2_e_& delay_budget_report_r14_c::set_type2()
{
  set(types::type2);
  return c.get<type2_e_>();
}
void delay_budget_report_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::type1:
      j.write_str("type1", c.get<type1_e_>().to_string());
      break;
    case types::type2:
      j.write_str("type2", c.get<type2_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "delay_budget_report_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE delay_budget_report_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::type1:
      HANDLE_CODE(c.get<type1_e_>().pack(bref));
      break;
    case types::type2:
      HANDLE_CODE(c.get<type2_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "delay_budget_report_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE delay_budget_report_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::type1:
      HANDLE_CODE(c.get<type1_e_>().unpack(bref));
      break;
    case types::type2:
      HANDLE_CODE(c.get<type2_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "delay_budget_report_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* delay_budget_report_r14_c::type1_opts::to_string() const
{
  static const char* options[] = {"msMinus1280",
                                  "msMinus640",
                                  "msMinus320",
                                  "msMinus160",
                                  "msMinus80",
                                  "msMinus60",
                                  "msMinus40",
                                  "msMinus20",
                                  "ms0",
                                  "ms20",
                                  "ms40",
                                  "ms60",
                                  "ms80",
                                  "ms160",
                                  "ms320",
                                  "ms640",
                                  "ms1280"};
  return convert_enum_idx(options, 17, value, "delay_budget_report_r14_c::type1_e_");
}
int16_t delay_budget_report_r14_c::type1_opts::to_number() const
{
  static const int16_t options[] = {
      -1280, -640, -320, -160, -80, -60, -40, -20, 0, 20, 40, 60, 80, 160, 320, 640, 1280};
  return map_enum_number(options, 17, value, "delay_budget_report_r14_c::type1_e_");
}

const char* delay_budget_report_r14_c::type2_opts::to_string() const
{
  static const char* options[] = {"msMinus192",
                                  "msMinus168",
                                  "msMinus144",
                                  "msMinus120",
                                  "msMinus96",
                                  "msMinus72",
                                  "msMinus48",
                                  "msMinus24",
                                  "ms0",
                                  "ms24",
                                  "ms48",
                                  "ms72",
                                  "ms96",
                                  "ms120",
                                  "ms144",
                                  "ms168",
                                  "ms192"};
  return convert_enum_idx(options, 17, value, "delay_budget_report_r14_c::type2_e_");
}
int16_t delay_budget_report_r14_c::type2_opts::to_number() const
{
  static const int16_t options[] = {-192, -168, -144, -120, -96, -72, -48, -24, 0, 24, 48, 72, 96, 120, 144, 168, 192};
  return map_enum_number(options, 17, value, "delay_budget_report_r14_c::type2_e_");
}

// InDeviceCoexIndication-v1310-IEs ::= SEQUENCE
SRSASN_CODE in_dev_coex_ind_v1310_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(affected_carrier_freq_list_v1310_present, 1));
  HANDLE_CODE(bref.pack(affected_carrier_freq_comb_list_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (affected_carrier_freq_list_v1310_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, affected_carrier_freq_list_v1310, 1, 32));
  }
  if (affected_carrier_freq_comb_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref,
                                affected_carrier_freq_comb_list_r13,
                                1,
                                128,
                                SeqOfPacker<integer_packer<uint8_t> >(2, 32, integer_packer<uint8_t>(1, 64))));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE in_dev_coex_ind_v1310_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(affected_carrier_freq_list_v1310_present, 1));
  HANDLE_CODE(bref.unpack(affected_carrier_freq_comb_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (affected_carrier_freq_list_v1310_present) {
    HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_list_v1310, bref, 1, 32));
  }
  if (affected_carrier_freq_comb_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_list_r13,
                                  bref,
                                  1,
                                  128,
                                  SeqOfPacker<integer_packer<uint8_t> >(2, 32, integer_packer<uint8_t>(1, 64))));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void in_dev_coex_ind_v1310_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (affected_carrier_freq_list_v1310_present) {
    j.start_array("affectedCarrierFreqList-v1310");
    for (const auto& e1 : affected_carrier_freq_list_v1310) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (affected_carrier_freq_comb_list_r13_present) {
    j.start_array("affectedCarrierFreqCombList-r13");
    for (const auto& e1 : affected_carrier_freq_comb_list_r13) {
      j.start_array();
      for (const auto& e2 : e1) {
        j.write_int(e2);
      }
      j.end_array();
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MBMSInterestIndication-v1540-IEs ::= SEQUENCE
SRSASN_CODE mbms_interest_ind_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mbms_rom_info_list_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mbms_rom_info_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbms_rom_info_list_r15, 1, 15));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_interest_ind_v1540_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mbms_rom_info_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mbms_rom_info_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbms_rom_info_list_r15, bref, 1, 15));
  }

  return SRSASN_SUCCESS;
}
void mbms_interest_ind_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbms_rom_info_list_r15_present) {
    j.start_array("mbms-ROM-InfoList-r15");
    for (const auto& e1 : mbms_rom_info_list_r15) {
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

// RRCConnectionReconfigurationComplete-v1020-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_complete_v1020_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rlf_info_available_r10_present, 1));
  HANDLE_CODE(bref.pack(log_meas_available_r10_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_complete_v1020_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rlf_info_available_r10_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_available_r10_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_complete_v1020_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rlf_info_available_r10_present) {
    j.write_str("rlf-InfoAvailable-r10", "true");
  }
  if (log_meas_available_r10_present) {
    j.write_str("logMeasAvailable-r10", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReestablishmentComplete-v8a0-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_complete_v8a0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE rrc_conn_reest_complete_v8a0_ies_s::unpack(cbit_ref& bref)
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
void rrc_conn_reest_complete_v8a0_ies_s::to_json(json_writer& j) const
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

// RRCConnectionSetupComplete-v1020-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_v1020_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gummei_type_r10_present, 1));
  HANDLE_CODE(bref.pack(rlf_info_available_r10_present, 1));
  HANDLE_CODE(bref.pack(log_meas_available_r10_present, 1));
  HANDLE_CODE(bref.pack(rn_sf_cfg_req_r10_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (gummei_type_r10_present) {
    HANDLE_CODE(gummei_type_r10.pack(bref));
  }
  if (rn_sf_cfg_req_r10_present) {
    HANDLE_CODE(rn_sf_cfg_req_r10.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_v1020_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gummei_type_r10_present, 1));
  HANDLE_CODE(bref.unpack(rlf_info_available_r10_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_available_r10_present, 1));
  HANDLE_CODE(bref.unpack(rn_sf_cfg_req_r10_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (gummei_type_r10_present) {
    HANDLE_CODE(gummei_type_r10.unpack(bref));
  }
  if (rn_sf_cfg_req_r10_present) {
    HANDLE_CODE(rn_sf_cfg_req_r10.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_complete_v1020_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gummei_type_r10_present) {
    j.write_str("gummei-Type-r10", gummei_type_r10.to_string());
  }
  if (rlf_info_available_r10_present) {
    j.write_str("rlf-InfoAvailable-r10", "true");
  }
  if (log_meas_available_r10_present) {
    j.write_str("logMeasAvailable-r10", "true");
  }
  if (rn_sf_cfg_req_r10_present) {
    j.write_str("rn-SubframeConfigReq-r10", rn_sf_cfg_req_r10.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* rrc_conn_setup_complete_v1020_ies_s::gummei_type_r10_opts::to_string() const
{
  static const char* options[] = {"native", "mapped"};
  return convert_enum_idx(options, 2, value, "rrc_conn_setup_complete_v1020_ies_s::gummei_type_r10_e_");
}

const char* rrc_conn_setup_complete_v1020_ies_s::rn_sf_cfg_req_r10_opts::to_string() const
{
  static const char* options[] = {"required", "notRequired"};
  return convert_enum_idx(options, 2, value, "rrc_conn_setup_complete_v1020_ies_s::rn_sf_cfg_req_r10_e_");
}

// RSTD-InterFreqInfo-r10 ::= SEQUENCE
SRSASN_CODE rstd_inter_freq_info_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, carrier_freq_r10, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(pack_integer(bref, meas_prs_offset_r10, (uint8_t)0u, (uint8_t)39u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= carrier_freq_v1090_present;
    group_flags[1] |= meas_prs_offset_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(carrier_freq_v1090_present, 1));
      if (carrier_freq_v1090_present) {
        HANDLE_CODE(pack_integer(bref, carrier_freq_v1090, (uint32_t)65536u, (uint32_t)262143u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_prs_offset_r15.is_present(), 1));
      if (meas_prs_offset_r15.is_present()) {
        HANDLE_CODE(meas_prs_offset_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rstd_inter_freq_info_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(carrier_freq_r10, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(unpack_integer(meas_prs_offset_r10, bref, (uint8_t)0u, (uint8_t)39u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(carrier_freq_v1090_present, 1));
      if (carrier_freq_v1090_present) {
        HANDLE_CODE(unpack_integer(carrier_freq_v1090, bref, (uint32_t)65536u, (uint32_t)262143u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_prs_offset_r15_present;
      HANDLE_CODE(bref.unpack(meas_prs_offset_r15_present, 1));
      meas_prs_offset_r15.set_present(meas_prs_offset_r15_present);
      if (meas_prs_offset_r15.is_present()) {
        HANDLE_CODE(meas_prs_offset_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rstd_inter_freq_info_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r10", carrier_freq_r10);
  j.write_int("measPRS-Offset-r10", meas_prs_offset_r10);
  if (ext) {
    if (carrier_freq_v1090_present) {
      j.write_int("carrierFreq-v1090", carrier_freq_v1090);
    }
    if (meas_prs_offset_r15.is_present()) {
      j.write_fieldname("measPRS-Offset-r15");
      meas_prs_offset_r15->to_json(j);
    }
  }
  j.end_obj();
}

void rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::destroy_() {}
void rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::meas_prs_offset_r15_c_(
    const rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_& other)
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
      log_invalid_choice_id(type_, "rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_");
  }
}
rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::operator=(
    const rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_& other)
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
      log_invalid_choice_id(type_, "rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_");
  }

  return *this;
}
uint8_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd0_r15()
{
  set(types::rstd0_r15);
  return c.get<uint8_t>();
}
uint8_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd1_r15()
{
  set(types::rstd1_r15);
  return c.get<uint8_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd2_r15()
{
  set(types::rstd2_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd3_r15()
{
  set(types::rstd3_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd4_r15()
{
  set(types::rstd4_r15);
  return c.get<uint16_t>();
}
uint8_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd5_r15()
{
  set(types::rstd5_r15);
  return c.get<uint8_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd6_r15()
{
  set(types::rstd6_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd7_r15()
{
  set(types::rstd7_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd8_r15()
{
  set(types::rstd8_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd9_r15()
{
  set(types::rstd9_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd10_r15()
{
  set(types::rstd10_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd11_r15()
{
  set(types::rstd11_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd12_r15()
{
  set(types::rstd12_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd13_r15()
{
  set(types::rstd13_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd14_r15()
{
  set(types::rstd14_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd15_r15()
{
  set(types::rstd15_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd16_r15()
{
  set(types::rstd16_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd17_r15()
{
  set(types::rstd17_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd18_r15()
{
  set(types::rstd18_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd19_r15()
{
  set(types::rstd19_r15);
  return c.get<uint16_t>();
}
uint16_t& rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::set_rstd20_r15()
{
  set(types::rstd20_r15);
  return c.get<uint16_t>();
}
void rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-CommTxResourceReq-r12 ::= SEQUENCE
SRSASN_CODE sl_comm_tx_res_req_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(carrier_freq_r12_present, 1));

  if (carrier_freq_r12_present) {
    HANDLE_CODE(pack_integer(bref, carrier_freq_r12, (uint32_t)0u, (uint32_t)262143u));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, dest_info_list_r12, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_comm_tx_res_req_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(carrier_freq_r12_present, 1));

  if (carrier_freq_r12_present) {
    HANDLE_CODE(unpack_integer(carrier_freq_r12, bref, (uint32_t)0u, (uint32_t)262143u));
  }
  HANDLE_CODE(unpack_dyn_seq_of(dest_info_list_r12, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void sl_comm_tx_res_req_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_freq_r12_present) {
    j.write_int("carrierFreq-r12", carrier_freq_r12);
  }
  j.start_array("destinationInfoList-r12");
  for (const auto& e1 : dest_info_list_r12) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

// SidelinkUEInformation-v1430-IEs ::= SEQUENCE
SRSASN_CODE sidelink_ue_info_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(v2x_comm_rx_interested_freq_list_r14_present, 1));
  HANDLE_CODE(bref.pack(p2x_comm_tx_type_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_comm_tx_res_req_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (v2x_comm_rx_interested_freq_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_comm_rx_interested_freq_list_r14, 1, 8, integer_packer<uint8_t>(0, 7)));
  }
  if (v2x_comm_tx_res_req_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_comm_tx_res_req_r14, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sidelink_ue_info_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(v2x_comm_rx_interested_freq_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(p2x_comm_tx_type_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_comm_tx_res_req_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (v2x_comm_rx_interested_freq_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_comm_rx_interested_freq_list_r14, bref, 1, 8, integer_packer<uint8_t>(0, 7)));
  }
  if (v2x_comm_tx_res_req_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_comm_tx_res_req_r14, bref, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sidelink_ue_info_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (v2x_comm_rx_interested_freq_list_r14_present) {
    j.start_array("v2x-CommRxInterestedFreqList-r14");
    for (const auto& e1 : v2x_comm_rx_interested_freq_list_r14) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (p2x_comm_tx_type_r14_present) {
    j.write_str("p2x-CommTxType-r14", "true");
  }
  if (v2x_comm_tx_res_req_r14_present) {
    j.start_array("v2x-CommTxResourceReq-r14");
    for (const auto& e1 : v2x_comm_tx_res_req_r14) {
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

// UEAssistanceInformation-v1450-IEs ::= SEQUENCE
SRSASN_CODE ueassist_info_v1450_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(overheat_assist_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (overheat_assist_r14_present) {
    HANDLE_CODE(overheat_assist_r14.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueassist_info_v1450_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(overheat_assist_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (overheat_assist_r14_present) {
    HANDLE_CODE(overheat_assist_r14.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ueassist_info_v1450_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (overheat_assist_r14_present) {
    j.write_fieldname("overheatingAssistance-r14");
    overheat_assist_r14.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UEInformationResponse-v1020-IEs ::= SEQUENCE
SRSASN_CODE ue_info_resp_v1020_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(log_meas_report_r10_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (log_meas_report_r10_present) {
    HANDLE_CODE(log_meas_report_r10.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_resp_v1020_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(log_meas_report_r10_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (log_meas_report_r10_present) {
    HANDLE_CODE(log_meas_report_r10.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_info_resp_v1020_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (log_meas_report_r10_present) {
    j.write_fieldname("logMeasReport-r10");
    log_meas_report_r10.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// WLAN-Status-v1430 ::= ENUMERATED
const char* wlan_status_v1430_opts::to_string() const
{
  static const char* options[] = {"suspended", "resumed"};
  return convert_enum_idx(options, 2, value, "wlan_status_v1430_e");
}

// CSFBParametersRequestCDMA2000-v8a0-IEs ::= SEQUENCE
SRSASN_CODE csfb_params_request_cdma2000_v8a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csfb_params_request_cdma2000_v8a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csfb_params_request_cdma2000_v8a0_ies_s::to_json(json_writer& j) const
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

// CounterCheckResponse-v8a0-IEs ::= SEQUENCE
SRSASN_CODE counter_check_resp_v8a0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE counter_check_resp_v8a0_ies_s::unpack(cbit_ref& bref)
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
void counter_check_resp_v8a0_ies_s::to_json(json_writer& j) const
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

// FailureReportSCG-NR-r15 ::= SEQUENCE
SRSASN_CODE fail_report_scg_nr_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_freq_list_nr_r15_present, 1));
  HANDLE_CODE(bref.pack(meas_result_scg_r15_present, 1));

  HANDLE_CODE(fail_type_r15.pack(bref));
  if (meas_result_freq_list_nr_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_freq_list_nr_r15, 1, 5));
  }
  if (meas_result_scg_r15_present) {
    HANDLE_CODE(meas_result_scg_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_report_scg_nr_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_freq_list_nr_r15_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_scg_r15_present, 1));

  HANDLE_CODE(fail_type_r15.unpack(bref));
  if (meas_result_freq_list_nr_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_freq_list_nr_r15, bref, 1, 5));
  }
  if (meas_result_scg_r15_present) {
    HANDLE_CODE(meas_result_scg_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void fail_report_scg_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("failureType-r15", fail_type_r15.to_string());
  if (meas_result_freq_list_nr_r15_present) {
    j.start_array("measResultFreqListNR-r15");
    for (const auto& e1 : meas_result_freq_list_nr_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_scg_r15_present) {
    j.write_str("measResultSCG-r15", meas_result_scg_r15.to_string());
  }
  j.end_obj();
}

const char* fail_report_scg_nr_r15_s::fail_type_r15_opts::to_string() const
{
  static const char* options[] = {"t310-Expiry",
                                  "randomAccessProblem",
                                  "rlc-MaxNumRetx",
                                  "synchReconfigFailureSCG",
                                  "scg-reconfigFailure",
                                  "srb3-IntegrityFailure"};
  return convert_enum_idx(options, 6, value, "fail_report_scg_nr_r15_s::fail_type_r15_e_");
}
uint16_t fail_report_scg_nr_r15_s::fail_type_r15_opts::to_number() const
{
  switch (value) {
    case t310_expiry:
      return 310;
    case srb3_integrity_fail:
      return 3;
    default:
      invalid_enum_number(value, "fail_report_scg_nr_r15_s::fail_type_r15_e_");
  }
  return 0;
}

// FailureReportSCG-r12 ::= SEQUENCE
SRSASN_CODE fail_report_scg_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_serv_freq_list_r12_present, 1));
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_r12_present, 1));

  HANDLE_CODE(fail_type_r12.pack(bref));
  if (meas_result_serv_freq_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_serv_freq_list_r12, 1, 5));
  }
  if (meas_result_neigh_cells_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r12, 1, 8));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= fail_type_v1290_present;
    group_flags[1] |= meas_result_serv_freq_list_ext_r13.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(fail_type_v1290_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_serv_freq_list_ext_r13.is_present(), 1));
      if (meas_result_serv_freq_list_ext_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_serv_freq_list_ext_r13, 1, 32));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_report_scg_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_serv_freq_list_r12_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r12_present, 1));

  HANDLE_CODE(fail_type_r12.unpack(bref));
  if (meas_result_serv_freq_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_serv_freq_list_r12, bref, 1, 5));
  }
  if (meas_result_neigh_cells_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r12, bref, 1, 8));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(fail_type_v1290_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_result_serv_freq_list_ext_r13_present;
      HANDLE_CODE(bref.unpack(meas_result_serv_freq_list_ext_r13_present, 1));
      meas_result_serv_freq_list_ext_r13.set_present(meas_result_serv_freq_list_ext_r13_present);
      if (meas_result_serv_freq_list_ext_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_serv_freq_list_ext_r13, bref, 1, 32));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void fail_report_scg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("failureType-r12", fail_type_r12.to_string());
  if (meas_result_serv_freq_list_r12_present) {
    j.start_array("measResultServFreqList-r12");
    for (const auto& e1 : meas_result_serv_freq_list_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_neigh_cells_r12_present) {
    j.start_array("measResultNeighCells-r12");
    for (const auto& e1 : meas_result_neigh_cells_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (fail_type_v1290_present) {
      j.write_str("failureType-v1290", "maxUL-TimingDiff-v1290");
    }
    if (meas_result_serv_freq_list_ext_r13.is_present()) {
      j.start_array("measResultServFreqListExt-r13");
      for (const auto& e1 : *meas_result_serv_freq_list_ext_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* fail_report_scg_r12_s::fail_type_r12_opts::to_string() const
{
  static const char* options[] = {"t313-Expiry", "randomAccessProblem", "rlc-MaxNumRetx", "scg-ChangeFailure"};
  return convert_enum_idx(options, 4, value, "fail_report_scg_r12_s::fail_type_r12_e_");
}
uint16_t fail_report_scg_r12_s::fail_type_r12_opts::to_number() const
{
  static const uint16_t options[] = {313};
  return map_enum_number(options, 1, value, "fail_report_scg_r12_s::fail_type_r12_e_");
}

// InDeviceCoexIndication-v11d0-IEs ::= SEQUENCE
SRSASN_CODE in_dev_coex_ind_v11d0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_ca_assist_info_r11_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ul_ca_assist_info_r11_present) {
    HANDLE_CODE(bref.pack(ul_ca_assist_info_r11.affected_carrier_freq_comb_list_r11_present, 1));
    if (ul_ca_assist_info_r11.affected_carrier_freq_comb_list_r11_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  ul_ca_assist_info_r11.affected_carrier_freq_comb_list_r11,
                                  1,
                                  128,
                                  SeqOfPacker<integer_packer<uint8_t> >(2, 5, integer_packer<uint8_t>(1, 32))));
    }
    HANDLE_CODE(ul_ca_assist_info_r11.victim_sys_type_r11.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE in_dev_coex_ind_v11d0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_ca_assist_info_r11_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ul_ca_assist_info_r11_present) {
    HANDLE_CODE(bref.unpack(ul_ca_assist_info_r11.affected_carrier_freq_comb_list_r11_present, 1));
    if (ul_ca_assist_info_r11.affected_carrier_freq_comb_list_r11_present) {
      HANDLE_CODE(unpack_dyn_seq_of(ul_ca_assist_info_r11.affected_carrier_freq_comb_list_r11,
                                    bref,
                                    1,
                                    128,
                                    SeqOfPacker<integer_packer<uint8_t> >(2, 5, integer_packer<uint8_t>(1, 32))));
    }
    HANDLE_CODE(ul_ca_assist_info_r11.victim_sys_type_r11.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void in_dev_coex_ind_v11d0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_ca_assist_info_r11_present) {
    j.write_fieldname("ul-CA-AssistanceInfo-r11");
    j.start_obj();
    if (ul_ca_assist_info_r11.affected_carrier_freq_comb_list_r11_present) {
      j.start_array("affectedCarrierFreqCombList-r11");
      for (const auto& e1 : ul_ca_assist_info_r11.affected_carrier_freq_comb_list_r11) {
        j.start_array();
        for (const auto& e2 : e1) {
          j.write_int(e2);
        }
        j.end_array();
      }
      j.end_array();
    }
    j.write_fieldname("victimSystemType-r11");
    ul_ca_assist_info_r11.victim_sys_type_r11.to_json(j);
    j.end_obj();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MBMSInterestIndication-v1310-IEs ::= SEQUENCE
SRSASN_CODE mbms_interest_ind_v1310_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mbms_services_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mbms_services_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbms_services_r13, 0, 15));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_interest_ind_v1310_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mbms_services_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mbms_services_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbms_services_r13, bref, 0, 15));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mbms_interest_ind_v1310_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbms_services_r13_present) {
    j.start_array("mbms-Services-r13");
    for (const auto& e1 : mbms_services_r13) {
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

// MeasReportAppLayer-v1590-IEs ::= SEQUENCE
SRSASN_CODE meas_report_app_layer_v1590_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_app_layer_v1590_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_report_app_layer_v1590_ies_s::to_json(json_writer& j) const
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

// ProximityIndication-v930-IEs ::= SEQUENCE
SRSASN_CODE proximity_ind_v930_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE proximity_ind_v930_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void proximity_ind_v930_ies_s::to_json(json_writer& j) const
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

// RLF-Report-r9 ::= SEQUENCE
SRSASN_CODE rlf_report_r9_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_r9_present, 1));

  HANDLE_CODE(bref.pack(meas_result_last_serv_cell_r9.rsrq_result_r9_present, 1));
  HANDLE_CODE(pack_integer(bref, meas_result_last_serv_cell_r9.rsrp_result_r9, (uint8_t)0u, (uint8_t)97u));
  if (meas_result_last_serv_cell_r9.rsrq_result_r9_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_last_serv_cell_r9.rsrq_result_r9, (uint8_t)0u, (uint8_t)34u));
  }
  if (meas_result_neigh_cells_r9_present) {
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r9.meas_result_list_eutra_r9_present, 1));
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r9.meas_result_list_utra_r9_present, 1));
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r9.meas_result_list_geran_r9_present, 1));
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r9.meas_results_cdma2000_r9_present, 1));
    if (meas_result_neigh_cells_r9.meas_result_list_eutra_r9_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r9.meas_result_list_eutra_r9, 1, 8));
    }
    if (meas_result_neigh_cells_r9.meas_result_list_utra_r9_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r9.meas_result_list_utra_r9, 1, 8));
    }
    if (meas_result_neigh_cells_r9.meas_result_list_geran_r9_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r9.meas_result_list_geran_r9, 1, 8));
    }
    if (meas_result_neigh_cells_r9.meas_results_cdma2000_r9_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r9.meas_results_cdma2000_r9, 1, 8));
    }
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= location_info_r10.is_present();
    group_flags[0] |= failed_pcell_id_r10.is_present();
    group_flags[0] |= reest_cell_id_r10.is_present();
    group_flags[0] |= time_conn_fail_r10_present;
    group_flags[0] |= conn_fail_type_r10_present;
    group_flags[0] |= prev_pcell_id_r10.is_present();
    group_flags[1] |= failed_pcell_id_v1090.is_present();
    group_flags[2] |= basic_fields_r11.is_present();
    group_flags[2] |= prev_utra_cell_id_r11.is_present();
    group_flags[2] |= sel_utra_cell_id_r11.is_present();
    group_flags[3] |= failed_pcell_id_v1250.is_present();
    group_flags[3] |= meas_result_last_serv_cell_v1250_present;
    group_flags[3] |= last_serv_cell_rsrq_type_r12.is_present();
    group_flags[3] |= meas_result_list_eutra_v1250.is_present();
    group_flags[4] |= drb_established_with_qci_minus1_r13_present;
    group_flags[5] |= meas_result_last_serv_cell_v1360_present;
    group_flags[6] |= log_meas_result_list_bt_r15.is_present();
    group_flags[6] |= log_meas_result_list_wlan_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(location_info_r10.is_present(), 1));
      HANDLE_CODE(bref.pack(failed_pcell_id_r10.is_present(), 1));
      HANDLE_CODE(bref.pack(reest_cell_id_r10.is_present(), 1));
      HANDLE_CODE(bref.pack(time_conn_fail_r10_present, 1));
      HANDLE_CODE(bref.pack(conn_fail_type_r10_present, 1));
      HANDLE_CODE(bref.pack(prev_pcell_id_r10.is_present(), 1));
      if (location_info_r10.is_present()) {
        HANDLE_CODE(location_info_r10->pack(bref));
      }
      if (failed_pcell_id_r10.is_present()) {
        HANDLE_CODE(failed_pcell_id_r10->pack(bref));
      }
      if (reest_cell_id_r10.is_present()) {
        HANDLE_CODE(reest_cell_id_r10->pack(bref));
      }
      if (time_conn_fail_r10_present) {
        HANDLE_CODE(pack_integer(bref, time_conn_fail_r10, (uint16_t)0u, (uint16_t)1023u));
      }
      if (conn_fail_type_r10_present) {
        HANDLE_CODE(conn_fail_type_r10.pack(bref));
      }
      if (prev_pcell_id_r10.is_present()) {
        HANDLE_CODE(prev_pcell_id_r10->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(failed_pcell_id_v1090.is_present(), 1));
      if (failed_pcell_id_v1090.is_present()) {
        HANDLE_CODE(pack_integer(bref, failed_pcell_id_v1090->carrier_freq_v1090, (uint32_t)65536u, (uint32_t)262143u));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(basic_fields_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(prev_utra_cell_id_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(sel_utra_cell_id_r11.is_present(), 1));
      if (basic_fields_r11.is_present()) {
        HANDLE_CODE(basic_fields_r11->c_rnti_r11.pack(bref));
        HANDLE_CODE(basic_fields_r11->rlf_cause_r11.pack(bref));
        HANDLE_CODE(pack_integer(bref, basic_fields_r11->time_since_fail_r11, (uint32_t)0u, (uint32_t)172800u));
      }
      if (prev_utra_cell_id_r11.is_present()) {
        HANDLE_CODE(bref.pack(prev_utra_cell_id_r11->cell_global_id_r11_present, 1));
        HANDLE_CODE(pack_integer(bref, prev_utra_cell_id_r11->carrier_freq_r11, (uint16_t)0u, (uint16_t)16383u));
        HANDLE_CODE(prev_utra_cell_id_r11->pci_r11.pack(bref));
        if (prev_utra_cell_id_r11->cell_global_id_r11_present) {
          HANDLE_CODE(prev_utra_cell_id_r11->cell_global_id_r11.pack(bref));
        }
      }
      if (sel_utra_cell_id_r11.is_present()) {
        HANDLE_CODE(pack_integer(bref, sel_utra_cell_id_r11->carrier_freq_r11, (uint16_t)0u, (uint16_t)16383u));
        HANDLE_CODE(sel_utra_cell_id_r11->pci_r11.pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(failed_pcell_id_v1250.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_last_serv_cell_v1250_present, 1));
      HANDLE_CODE(bref.pack(last_serv_cell_rsrq_type_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_list_eutra_v1250.is_present(), 1));
      if (failed_pcell_id_v1250.is_present()) {
        HANDLE_CODE(failed_pcell_id_v1250->tac_failed_pcell_r12.pack(bref));
      }
      if (meas_result_last_serv_cell_v1250_present) {
        HANDLE_CODE(pack_integer(bref, meas_result_last_serv_cell_v1250, (int8_t)-30, (int8_t)46));
      }
      if (last_serv_cell_rsrq_type_r12.is_present()) {
        HANDLE_CODE(last_serv_cell_rsrq_type_r12->pack(bref));
      }
      if (meas_result_list_eutra_v1250.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_list_eutra_v1250, 1, 8));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(drb_established_with_qci_minus1_r13_present, 1));
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_last_serv_cell_v1360_present, 1));
      if (meas_result_last_serv_cell_v1360_present) {
        HANDLE_CODE(pack_integer(bref, meas_result_last_serv_cell_v1360, (int8_t)-17, (int8_t)-1));
      }
    }
    if (group_flags[6]) {
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
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_report_r9_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r9_present, 1));

  HANDLE_CODE(bref.unpack(meas_result_last_serv_cell_r9.rsrq_result_r9_present, 1));
  HANDLE_CODE(unpack_integer(meas_result_last_serv_cell_r9.rsrp_result_r9, bref, (uint8_t)0u, (uint8_t)97u));
  if (meas_result_last_serv_cell_r9.rsrq_result_r9_present) {
    HANDLE_CODE(unpack_integer(meas_result_last_serv_cell_r9.rsrq_result_r9, bref, (uint8_t)0u, (uint8_t)34u));
  }
  if (meas_result_neigh_cells_r9_present) {
    HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r9.meas_result_list_eutra_r9_present, 1));
    HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r9.meas_result_list_utra_r9_present, 1));
    HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r9.meas_result_list_geran_r9_present, 1));
    HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r9.meas_results_cdma2000_r9_present, 1));
    if (meas_result_neigh_cells_r9.meas_result_list_eutra_r9_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r9.meas_result_list_eutra_r9, bref, 1, 8));
    }
    if (meas_result_neigh_cells_r9.meas_result_list_utra_r9_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r9.meas_result_list_utra_r9, bref, 1, 8));
    }
    if (meas_result_neigh_cells_r9.meas_result_list_geran_r9_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r9.meas_result_list_geran_r9, bref, 1, 8));
    }
    if (meas_result_neigh_cells_r9.meas_results_cdma2000_r9_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r9.meas_results_cdma2000_r9, bref, 1, 8));
    }
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(7);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool location_info_r10_present;
      HANDLE_CODE(bref.unpack(location_info_r10_present, 1));
      location_info_r10.set_present(location_info_r10_present);
      bool failed_pcell_id_r10_present;
      HANDLE_CODE(bref.unpack(failed_pcell_id_r10_present, 1));
      failed_pcell_id_r10.set_present(failed_pcell_id_r10_present);
      bool reest_cell_id_r10_present;
      HANDLE_CODE(bref.unpack(reest_cell_id_r10_present, 1));
      reest_cell_id_r10.set_present(reest_cell_id_r10_present);
      HANDLE_CODE(bref.unpack(time_conn_fail_r10_present, 1));
      HANDLE_CODE(bref.unpack(conn_fail_type_r10_present, 1));
      bool prev_pcell_id_r10_present;
      HANDLE_CODE(bref.unpack(prev_pcell_id_r10_present, 1));
      prev_pcell_id_r10.set_present(prev_pcell_id_r10_present);
      if (location_info_r10.is_present()) {
        HANDLE_CODE(location_info_r10->unpack(bref));
      }
      if (failed_pcell_id_r10.is_present()) {
        HANDLE_CODE(failed_pcell_id_r10->unpack(bref));
      }
      if (reest_cell_id_r10.is_present()) {
        HANDLE_CODE(reest_cell_id_r10->unpack(bref));
      }
      if (time_conn_fail_r10_present) {
        HANDLE_CODE(unpack_integer(time_conn_fail_r10, bref, (uint16_t)0u, (uint16_t)1023u));
      }
      if (conn_fail_type_r10_present) {
        HANDLE_CODE(conn_fail_type_r10.unpack(bref));
      }
      if (prev_pcell_id_r10.is_present()) {
        HANDLE_CODE(prev_pcell_id_r10->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool failed_pcell_id_v1090_present;
      HANDLE_CODE(bref.unpack(failed_pcell_id_v1090_present, 1));
      failed_pcell_id_v1090.set_present(failed_pcell_id_v1090_present);
      if (failed_pcell_id_v1090.is_present()) {
        HANDLE_CODE(
            unpack_integer(failed_pcell_id_v1090->carrier_freq_v1090, bref, (uint32_t)65536u, (uint32_t)262143u));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool basic_fields_r11_present;
      HANDLE_CODE(bref.unpack(basic_fields_r11_present, 1));
      basic_fields_r11.set_present(basic_fields_r11_present);
      bool prev_utra_cell_id_r11_present;
      HANDLE_CODE(bref.unpack(prev_utra_cell_id_r11_present, 1));
      prev_utra_cell_id_r11.set_present(prev_utra_cell_id_r11_present);
      bool sel_utra_cell_id_r11_present;
      HANDLE_CODE(bref.unpack(sel_utra_cell_id_r11_present, 1));
      sel_utra_cell_id_r11.set_present(sel_utra_cell_id_r11_present);
      if (basic_fields_r11.is_present()) {
        HANDLE_CODE(basic_fields_r11->c_rnti_r11.unpack(bref));
        HANDLE_CODE(basic_fields_r11->rlf_cause_r11.unpack(bref));
        HANDLE_CODE(unpack_integer(basic_fields_r11->time_since_fail_r11, bref, (uint32_t)0u, (uint32_t)172800u));
      }
      if (prev_utra_cell_id_r11.is_present()) {
        HANDLE_CODE(bref.unpack(prev_utra_cell_id_r11->cell_global_id_r11_present, 1));
        HANDLE_CODE(unpack_integer(prev_utra_cell_id_r11->carrier_freq_r11, bref, (uint16_t)0u, (uint16_t)16383u));
        HANDLE_CODE(prev_utra_cell_id_r11->pci_r11.unpack(bref));
        if (prev_utra_cell_id_r11->cell_global_id_r11_present) {
          HANDLE_CODE(prev_utra_cell_id_r11->cell_global_id_r11.unpack(bref));
        }
      }
      if (sel_utra_cell_id_r11.is_present()) {
        HANDLE_CODE(unpack_integer(sel_utra_cell_id_r11->carrier_freq_r11, bref, (uint16_t)0u, (uint16_t)16383u));
        HANDLE_CODE(sel_utra_cell_id_r11->pci_r11.unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool failed_pcell_id_v1250_present;
      HANDLE_CODE(bref.unpack(failed_pcell_id_v1250_present, 1));
      failed_pcell_id_v1250.set_present(failed_pcell_id_v1250_present);
      HANDLE_CODE(bref.unpack(meas_result_last_serv_cell_v1250_present, 1));
      bool last_serv_cell_rsrq_type_r12_present;
      HANDLE_CODE(bref.unpack(last_serv_cell_rsrq_type_r12_present, 1));
      last_serv_cell_rsrq_type_r12.set_present(last_serv_cell_rsrq_type_r12_present);
      bool meas_result_list_eutra_v1250_present;
      HANDLE_CODE(bref.unpack(meas_result_list_eutra_v1250_present, 1));
      meas_result_list_eutra_v1250.set_present(meas_result_list_eutra_v1250_present);
      if (failed_pcell_id_v1250.is_present()) {
        HANDLE_CODE(failed_pcell_id_v1250->tac_failed_pcell_r12.unpack(bref));
      }
      if (meas_result_last_serv_cell_v1250_present) {
        HANDLE_CODE(unpack_integer(meas_result_last_serv_cell_v1250, bref, (int8_t)-30, (int8_t)46));
      }
      if (last_serv_cell_rsrq_type_r12.is_present()) {
        HANDLE_CODE(last_serv_cell_rsrq_type_r12->unpack(bref));
      }
      if (meas_result_list_eutra_v1250.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_list_eutra_v1250, bref, 1, 8));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(drb_established_with_qci_minus1_r13_present, 1));
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(meas_result_last_serv_cell_v1360_present, 1));
      if (meas_result_last_serv_cell_v1360_present) {
        HANDLE_CODE(unpack_integer(meas_result_last_serv_cell_v1360, bref, (int8_t)-17, (int8_t)-1));
      }
    }
    if (group_flags[6]) {
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
  }
  return SRSASN_SUCCESS;
}
void rlf_report_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measResultLastServCell-r9");
  j.start_obj();
  j.write_int("rsrpResult-r9", meas_result_last_serv_cell_r9.rsrp_result_r9);
  if (meas_result_last_serv_cell_r9.rsrq_result_r9_present) {
    j.write_int("rsrqResult-r9", meas_result_last_serv_cell_r9.rsrq_result_r9);
  }
  j.end_obj();
  if (meas_result_neigh_cells_r9_present) {
    j.write_fieldname("measResultNeighCells-r9");
    j.start_obj();
    if (meas_result_neigh_cells_r9.meas_result_list_eutra_r9_present) {
      j.start_array("measResultListEUTRA-r9");
      for (const auto& e1 : meas_result_neigh_cells_r9.meas_result_list_eutra_r9) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_neigh_cells_r9.meas_result_list_utra_r9_present) {
      j.start_array("measResultListUTRA-r9");
      for (const auto& e1 : meas_result_neigh_cells_r9.meas_result_list_utra_r9) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_neigh_cells_r9.meas_result_list_geran_r9_present) {
      j.start_array("measResultListGERAN-r9");
      for (const auto& e1 : meas_result_neigh_cells_r9.meas_result_list_geran_r9) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_neigh_cells_r9.meas_results_cdma2000_r9_present) {
      j.start_array("measResultsCDMA2000-r9");
      for (const auto& e1 : meas_result_neigh_cells_r9.meas_results_cdma2000_r9) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  if (ext) {
    if (location_info_r10.is_present()) {
      j.write_fieldname("locationInfo-r10");
      location_info_r10->to_json(j);
    }
    if (failed_pcell_id_r10.is_present()) {
      j.write_fieldname("failedPCellId-r10");
      failed_pcell_id_r10->to_json(j);
    }
    if (reest_cell_id_r10.is_present()) {
      j.write_fieldname("reestablishmentCellId-r10");
      reest_cell_id_r10->to_json(j);
    }
    if (time_conn_fail_r10_present) {
      j.write_int("timeConnFailure-r10", time_conn_fail_r10);
    }
    if (conn_fail_type_r10_present) {
      j.write_str("connectionFailureType-r10", conn_fail_type_r10.to_string());
    }
    if (prev_pcell_id_r10.is_present()) {
      j.write_fieldname("previousPCellId-r10");
      prev_pcell_id_r10->to_json(j);
    }
    if (failed_pcell_id_v1090.is_present()) {
      j.write_fieldname("failedPCellId-v1090");
      j.start_obj();
      j.write_int("carrierFreq-v1090", failed_pcell_id_v1090->carrier_freq_v1090);
      j.end_obj();
    }
    if (basic_fields_r11.is_present()) {
      j.write_fieldname("basicFields-r11");
      j.start_obj();
      j.write_str("c-RNTI-r11", basic_fields_r11->c_rnti_r11.to_string());
      j.write_str("rlf-Cause-r11", basic_fields_r11->rlf_cause_r11.to_string());
      j.write_int("timeSinceFailure-r11", basic_fields_r11->time_since_fail_r11);
      j.end_obj();
    }
    if (prev_utra_cell_id_r11.is_present()) {
      j.write_fieldname("previousUTRA-CellId-r11");
      j.start_obj();
      j.write_int("carrierFreq-r11", prev_utra_cell_id_r11->carrier_freq_r11);
      j.write_fieldname("physCellId-r11");
      prev_utra_cell_id_r11->pci_r11.to_json(j);
      if (prev_utra_cell_id_r11->cell_global_id_r11_present) {
        j.write_fieldname("cellGlobalId-r11");
        prev_utra_cell_id_r11->cell_global_id_r11.to_json(j);
      }
      j.end_obj();
    }
    if (sel_utra_cell_id_r11.is_present()) {
      j.write_fieldname("selectedUTRA-CellId-r11");
      j.start_obj();
      j.write_int("carrierFreq-r11", sel_utra_cell_id_r11->carrier_freq_r11);
      j.write_fieldname("physCellId-r11");
      sel_utra_cell_id_r11->pci_r11.to_json(j);
      j.end_obj();
    }
    if (failed_pcell_id_v1250.is_present()) {
      j.write_fieldname("failedPCellId-v1250");
      j.start_obj();
      j.write_str("tac-FailedPCell-r12", failed_pcell_id_v1250->tac_failed_pcell_r12.to_string());
      j.end_obj();
    }
    if (meas_result_last_serv_cell_v1250_present) {
      j.write_int("measResultLastServCell-v1250", meas_result_last_serv_cell_v1250);
    }
    if (last_serv_cell_rsrq_type_r12.is_present()) {
      j.write_fieldname("lastServCellRSRQ-Type-r12");
      last_serv_cell_rsrq_type_r12->to_json(j);
    }
    if (meas_result_list_eutra_v1250.is_present()) {
      j.start_array("measResultListEUTRA-v1250");
      for (const auto& e1 : *meas_result_list_eutra_v1250) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (drb_established_with_qci_minus1_r13_present) {
      j.write_str("drb-EstablishedWithQCI-1-r13", "qci1");
    }
    if (meas_result_last_serv_cell_v1360_present) {
      j.write_int("measResultLastServCell-v1360", meas_result_last_serv_cell_v1360);
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
  }
  j.end_obj();
}

void rlf_report_r9_s::failed_pcell_id_r10_c_::destroy_()
{
  switch (type_) {
    case types::cell_global_id_r10:
      c.destroy<cell_global_id_eutra_s>();
      break;
    case types::pci_arfcn_r10:
      c.destroy<pci_arfcn_r10_s_>();
      break;
    default:
      break;
  }
}
void rlf_report_r9_s::failed_pcell_id_r10_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_global_id_r10:
      c.init<cell_global_id_eutra_s>();
      break;
    case types::pci_arfcn_r10:
      c.init<pci_arfcn_r10_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::failed_pcell_id_r10_c_");
  }
}
rlf_report_r9_s::failed_pcell_id_r10_c_::failed_pcell_id_r10_c_(const rlf_report_r9_s::failed_pcell_id_r10_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_global_id_r10:
      c.init(other.c.get<cell_global_id_eutra_s>());
      break;
    case types::pci_arfcn_r10:
      c.init(other.c.get<pci_arfcn_r10_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::failed_pcell_id_r10_c_");
  }
}
rlf_report_r9_s::failed_pcell_id_r10_c_&
rlf_report_r9_s::failed_pcell_id_r10_c_::operator=(const rlf_report_r9_s::failed_pcell_id_r10_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_global_id_r10:
      c.set(other.c.get<cell_global_id_eutra_s>());
      break;
    case types::pci_arfcn_r10:
      c.set(other.c.get<pci_arfcn_r10_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::failed_pcell_id_r10_c_");
  }

  return *this;
}
cell_global_id_eutra_s& rlf_report_r9_s::failed_pcell_id_r10_c_::set_cell_global_id_r10()
{
  set(types::cell_global_id_r10);
  return c.get<cell_global_id_eutra_s>();
}
rlf_report_r9_s::failed_pcell_id_r10_c_::pci_arfcn_r10_s_& rlf_report_r9_s::failed_pcell_id_r10_c_::set_pci_arfcn_r10()
{
  set(types::pci_arfcn_r10);
  return c.get<pci_arfcn_r10_s_>();
}
void rlf_report_r9_s::failed_pcell_id_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_global_id_r10:
      j.write_fieldname("cellGlobalId-r10");
      c.get<cell_global_id_eutra_s>().to_json(j);
      break;
    case types::pci_arfcn_r10:
      j.write_fieldname("pci-arfcn-r10");
      j.start_obj();
      j.write_int("physCellId-r10", c.get<pci_arfcn_r10_s_>().pci_r10);
      j.write_int("carrierFreq-r10", c.get<pci_arfcn_r10_s_>().carrier_freq_r10);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::failed_pcell_id_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE rlf_report_r9_s::failed_pcell_id_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_global_id_r10:
      HANDLE_CODE(c.get<cell_global_id_eutra_s>().pack(bref));
      break;
    case types::pci_arfcn_r10:
      HANDLE_CODE(pack_integer(bref, c.get<pci_arfcn_r10_s_>().pci_r10, (uint16_t)0u, (uint16_t)503u));
      HANDLE_CODE(pack_integer(bref, c.get<pci_arfcn_r10_s_>().carrier_freq_r10, (uint32_t)0u, (uint32_t)65535u));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::failed_pcell_id_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_report_r9_s::failed_pcell_id_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_global_id_r10:
      HANDLE_CODE(c.get<cell_global_id_eutra_s>().unpack(bref));
      break;
    case types::pci_arfcn_r10:
      HANDLE_CODE(unpack_integer(c.get<pci_arfcn_r10_s_>().pci_r10, bref, (uint16_t)0u, (uint16_t)503u));
      HANDLE_CODE(unpack_integer(c.get<pci_arfcn_r10_s_>().carrier_freq_r10, bref, (uint32_t)0u, (uint32_t)65535u));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::failed_pcell_id_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rlf_report_r9_s::conn_fail_type_r10_opts::to_string() const
{
  static const char* options[] = {"rlf", "hof"};
  return convert_enum_idx(options, 2, value, "rlf_report_r9_s::conn_fail_type_r10_e_");
}

const char* rlf_report_r9_s::basic_fields_r11_s_::rlf_cause_r11_opts::to_string() const
{
  static const char* options[] = {"t310-Expiry", "randomAccessProblem", "rlc-MaxNumRetx", "t312-Expiry-r12"};
  return convert_enum_idx(options, 4, value, "rlf_report_r9_s::basic_fields_r11_s_::rlf_cause_r11_e_");
}
uint16_t rlf_report_r9_s::basic_fields_r11_s_::rlf_cause_r11_opts::to_number() const
{
  switch (value) {
    case t310_expiry:
      return 310;
    case t312_expiry_r12:
      return 312;
    default:
      invalid_enum_number(value, "rlf_report_r9_s::basic_fields_r11_s_::rlf_cause_r11_e_");
  }
  return 0;
}

void rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::destroy_() {}
void rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::pci_r11_c_(
    const rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::fdd_r11:
      c.init(other.c.get<uint16_t>());
      break;
    case types::tdd_r11:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_");
  }
}
rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_& rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::operator=(
    const rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::fdd_r11:
      c.set(other.c.get<uint16_t>());
      break;
    case types::tdd_r11:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_");
  }

  return *this;
}
uint16_t& rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::set_fdd_r11()
{
  set(types::fdd_r11);
  return c.get<uint16_t>();
}
uint8_t& rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::set_tdd_r11()
{
  set(types::tdd_r11);
  return c.get<uint8_t>();
}
void rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fdd_r11:
      j.write_int("fdd-r11", c.get<uint16_t>());
      break;
    case types::tdd_r11:
      j.write_int("tdd-r11", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fdd_r11:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::tdd_r11:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fdd_r11:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::tdd_r11:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::destroy_() {}
void rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::pci_r11_c_(
    const rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::fdd_r11:
      c.init(other.c.get<uint16_t>());
      break;
    case types::tdd_r11:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_");
  }
}
rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_& rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::operator=(
    const rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::fdd_r11:
      c.set(other.c.get<uint16_t>());
      break;
    case types::tdd_r11:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_");
  }

  return *this;
}
uint16_t& rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::set_fdd_r11()
{
  set(types::fdd_r11);
  return c.get<uint16_t>();
}
uint8_t& rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::set_tdd_r11()
{
  set(types::tdd_r11);
  return c.get<uint8_t>();
}
void rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fdd_r11:
      j.write_int("fdd-r11", c.get<uint16_t>());
      break;
    case types::tdd_r11:
      j.write_int("tdd-r11", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fdd_r11:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::tdd_r11:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fdd_r11:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::tdd_r11:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionReconfigurationComplete-v8a0-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_complete_v8a0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE rrc_conn_recfg_complete_v8a0_ies_s::unpack(cbit_ref& bref)
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
void rrc_conn_recfg_complete_v8a0_ies_s::to_json(json_writer& j) const
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

// RRCConnectionReestablishmentComplete-v920-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_complete_v920_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rlf_info_available_r9_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_complete_v920_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rlf_info_available_r9_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_complete_v920_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rlf_info_available_r9_present) {
    j.write_str("rlf-InfoAvailable-r9", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionResumeComplete-v1530-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_complete_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(log_meas_available_bt_r15_present, 1));
  HANDLE_CODE(bref.pack(log_meas_available_wlan_r15_present, 1));
  HANDLE_CODE(bref.pack(idle_meas_available_r15_present, 1));
  HANDLE_CODE(bref.pack(flight_path_info_available_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_complete_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(log_meas_available_bt_r15_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_available_wlan_r15_present, 1));
  HANDLE_CODE(bref.unpack(idle_meas_available_r15_present, 1));
  HANDLE_CODE(bref.unpack(flight_path_info_available_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_complete_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (log_meas_available_bt_r15_present) {
    j.write_str("logMeasAvailableBT-r15", "true");
  }
  if (log_meas_available_wlan_r15_present) {
    j.write_str("logMeasAvailableWLAN-r15", "true");
  }
  if (idle_meas_available_r15_present) {
    j.write_str("idleMeasAvailable-r15", "true");
  }
  if (flight_path_info_available_r15_present) {
    j.write_str("flightPathInfoAvailable-r15", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RRCConnectionSetupComplete-v8a0-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_v8a0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE rrc_conn_setup_complete_v8a0_ies_s::unpack(cbit_ref& bref)
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
void rrc_conn_setup_complete_v8a0_ies_s::to_json(json_writer& j) const
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

// RegisteredMME ::= SEQUENCE
SRSASN_CODE registered_mme_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(plmn_id_present, 1));

  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.pack(bref));
  }
  HANDLE_CODE(mmegi.pack(bref));
  HANDLE_CODE(mmec.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE registered_mme_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(plmn_id_present, 1));

  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.unpack(bref));
  }
  HANDLE_CODE(mmegi.unpack(bref));
  HANDLE_CODE(mmec.unpack(bref));

  return SRSASN_SUCCESS;
}
void registered_mme_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_present) {
    j.write_fieldname("plmn-Identity");
    plmn_id.to_json(j);
  }
  j.write_str("mmegi", mmegi.to_string());
  j.write_str("mmec", mmec.to_string());
  j.end_obj();
}

// SCGFailureInformation-v12d0a-IEs ::= SEQUENCE
SRSASN_CODE scg_fail_info_v12d0a_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_v12d0a_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_fail_info_v12d0a_ies_s::to_json(json_writer& j) const
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

// SCGFailureInformationNR-v1590-IEs ::= SEQUENCE
SRSASN_CODE scg_fail_info_nr_v1590_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_nr_v1590_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_fail_info_nr_v1590_ies_s::to_json(json_writer& j) const
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

// SidelinkUEInformation-v1310-IEs ::= SEQUENCE
SRSASN_CODE sidelink_ue_info_v1310_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(comm_tx_res_req_uc_r13_present, 1));
  HANDLE_CODE(bref.pack(comm_tx_res_info_req_relay_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_tx_res_req_v1310_present, 1));
  HANDLE_CODE(bref.pack(disc_tx_res_req_ps_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_rx_gap_req_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_tx_gap_req_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_sys_info_report_freq_list_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (comm_tx_res_req_uc_r13_present) {
    HANDLE_CODE(comm_tx_res_req_uc_r13.pack(bref));
  }
  if (comm_tx_res_info_req_relay_r13_present) {
    HANDLE_CODE(bref.pack(comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_r13_present, 1));
    HANDLE_CODE(bref.pack(comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_uc_r13_present, 1));
    if (comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_r13_present) {
      HANDLE_CODE(comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_r13.pack(bref));
    }
    if (comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_uc_r13_present) {
      HANDLE_CODE(comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_uc_r13.pack(bref));
    }
    HANDLE_CODE(comm_tx_res_info_req_relay_r13.ue_type_r13.pack(bref));
  }
  if (disc_tx_res_req_v1310_present) {
    HANDLE_CODE(bref.pack(disc_tx_res_req_v1310.carrier_freq_disc_tx_r13_present, 1));
    HANDLE_CODE(bref.pack(disc_tx_res_req_v1310.disc_tx_res_req_add_freq_r13_present, 1));
    if (disc_tx_res_req_v1310.carrier_freq_disc_tx_r13_present) {
      HANDLE_CODE(pack_integer(bref, disc_tx_res_req_v1310.carrier_freq_disc_tx_r13, (uint8_t)1u, (uint8_t)8u));
    }
    if (disc_tx_res_req_v1310.disc_tx_res_req_add_freq_r13_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, disc_tx_res_req_v1310.disc_tx_res_req_add_freq_r13, 1, 8));
    }
  }
  if (disc_tx_res_req_ps_r13_present) {
    HANDLE_CODE(disc_tx_res_req_ps_r13.pack(bref));
  }
  if (disc_rx_gap_req_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, disc_rx_gap_req_r13, 1, 8));
  }
  if (disc_tx_gap_req_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, disc_tx_gap_req_r13, 1, 8));
  }
  if (disc_sys_info_report_freq_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, disc_sys_info_report_freq_list_r13, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sidelink_ue_info_v1310_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(comm_tx_res_req_uc_r13_present, 1));
  HANDLE_CODE(bref.unpack(comm_tx_res_info_req_relay_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_tx_res_req_v1310_present, 1));
  HANDLE_CODE(bref.unpack(disc_tx_res_req_ps_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_rx_gap_req_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_tx_gap_req_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_sys_info_report_freq_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (comm_tx_res_req_uc_r13_present) {
    HANDLE_CODE(comm_tx_res_req_uc_r13.unpack(bref));
  }
  if (comm_tx_res_info_req_relay_r13_present) {
    HANDLE_CODE(bref.unpack(comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_r13_present, 1));
    HANDLE_CODE(bref.unpack(comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_uc_r13_present, 1));
    if (comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_r13_present) {
      HANDLE_CODE(comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_r13.unpack(bref));
    }
    if (comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_uc_r13_present) {
      HANDLE_CODE(comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_uc_r13.unpack(bref));
    }
    HANDLE_CODE(comm_tx_res_info_req_relay_r13.ue_type_r13.unpack(bref));
  }
  if (disc_tx_res_req_v1310_present) {
    HANDLE_CODE(bref.unpack(disc_tx_res_req_v1310.carrier_freq_disc_tx_r13_present, 1));
    HANDLE_CODE(bref.unpack(disc_tx_res_req_v1310.disc_tx_res_req_add_freq_r13_present, 1));
    if (disc_tx_res_req_v1310.carrier_freq_disc_tx_r13_present) {
      HANDLE_CODE(unpack_integer(disc_tx_res_req_v1310.carrier_freq_disc_tx_r13, bref, (uint8_t)1u, (uint8_t)8u));
    }
    if (disc_tx_res_req_v1310.disc_tx_res_req_add_freq_r13_present) {
      HANDLE_CODE(unpack_dyn_seq_of(disc_tx_res_req_v1310.disc_tx_res_req_add_freq_r13, bref, 1, 8));
    }
  }
  if (disc_tx_res_req_ps_r13_present) {
    HANDLE_CODE(disc_tx_res_req_ps_r13.unpack(bref));
  }
  if (disc_rx_gap_req_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(disc_rx_gap_req_r13, bref, 1, 8));
  }
  if (disc_tx_gap_req_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(disc_tx_gap_req_r13, bref, 1, 8));
  }
  if (disc_sys_info_report_freq_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(disc_sys_info_report_freq_list_r13, bref, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sidelink_ue_info_v1310_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (comm_tx_res_req_uc_r13_present) {
    j.write_fieldname("commTxResourceReqUC-r13");
    comm_tx_res_req_uc_r13.to_json(j);
  }
  if (comm_tx_res_info_req_relay_r13_present) {
    j.write_fieldname("commTxResourceInfoReqRelay-r13");
    j.start_obj();
    if (comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_r13_present) {
      j.write_fieldname("commTxResourceReqRelay-r13");
      comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_r13.to_json(j);
    }
    if (comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_uc_r13_present) {
      j.write_fieldname("commTxResourceReqRelayUC-r13");
      comm_tx_res_info_req_relay_r13.comm_tx_res_req_relay_uc_r13.to_json(j);
    }
    j.write_str("ue-Type-r13", comm_tx_res_info_req_relay_r13.ue_type_r13.to_string());
    j.end_obj();
  }
  if (disc_tx_res_req_v1310_present) {
    j.write_fieldname("discTxResourceReq-v1310");
    j.start_obj();
    if (disc_tx_res_req_v1310.carrier_freq_disc_tx_r13_present) {
      j.write_int("carrierFreqDiscTx-r13", disc_tx_res_req_v1310.carrier_freq_disc_tx_r13);
    }
    if (disc_tx_res_req_v1310.disc_tx_res_req_add_freq_r13_present) {
      j.start_array("discTxResourceReqAddFreq-r13");
      for (const auto& e1 : disc_tx_res_req_v1310.disc_tx_res_req_add_freq_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  if (disc_tx_res_req_ps_r13_present) {
    j.write_fieldname("discTxResourceReqPS-r13");
    disc_tx_res_req_ps_r13.to_json(j);
  }
  if (disc_rx_gap_req_r13_present) {
    j.start_array("discRxGapReq-r13");
    for (const auto& e1 : disc_rx_gap_req_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (disc_tx_gap_req_r13_present) {
    j.start_array("discTxGapReq-r13");
    for (const auto& e1 : disc_tx_gap_req_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (disc_sys_info_report_freq_list_r13_present) {
    j.start_array("discSysInfoReportFreqList-r13");
    for (const auto& e1 : disc_sys_info_report_freq_list_r13) {
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

const char* sidelink_ue_info_v1310_ies_s::comm_tx_res_info_req_relay_r13_s_::ue_type_r13_opts::to_string() const
{
  static const char* options[] = {"relayUE", "remoteUE"};
  return convert_enum_idx(
      options, 2, value, "sidelink_ue_info_v1310_ies_s::comm_tx_res_info_req_relay_r13_s_::ue_type_r13_e_");
}

// TDM-AssistanceInfo-r11 ::= CHOICE
void tdm_assist_info_r11_c::destroy_()
{
  switch (type_) {
    case types::drx_assist_info_r11:
      c.destroy<drx_assist_info_r11_s_>();
      break;
    case types::idc_sf_pattern_list_r11:
      c.destroy<idc_sf_pattern_list_r11_l>();
      break;
    default:
      break;
  }
}
void tdm_assist_info_r11_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::drx_assist_info_r11:
      c.init<drx_assist_info_r11_s_>();
      break;
    case types::idc_sf_pattern_list_r11:
      c.init<idc_sf_pattern_list_r11_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tdm_assist_info_r11_c");
  }
}
tdm_assist_info_r11_c::tdm_assist_info_r11_c(const tdm_assist_info_r11_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::drx_assist_info_r11:
      c.init(other.c.get<drx_assist_info_r11_s_>());
      break;
    case types::idc_sf_pattern_list_r11:
      c.init(other.c.get<idc_sf_pattern_list_r11_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tdm_assist_info_r11_c");
  }
}
tdm_assist_info_r11_c& tdm_assist_info_r11_c::operator=(const tdm_assist_info_r11_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::drx_assist_info_r11:
      c.set(other.c.get<drx_assist_info_r11_s_>());
      break;
    case types::idc_sf_pattern_list_r11:
      c.set(other.c.get<idc_sf_pattern_list_r11_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tdm_assist_info_r11_c");
  }

  return *this;
}
tdm_assist_info_r11_c::drx_assist_info_r11_s_& tdm_assist_info_r11_c::set_drx_assist_info_r11()
{
  set(types::drx_assist_info_r11);
  return c.get<drx_assist_info_r11_s_>();
}
idc_sf_pattern_list_r11_l& tdm_assist_info_r11_c::set_idc_sf_pattern_list_r11()
{
  set(types::idc_sf_pattern_list_r11);
  return c.get<idc_sf_pattern_list_r11_l>();
}
void tdm_assist_info_r11_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::drx_assist_info_r11:
      j.write_fieldname("drx-AssistanceInfo-r11");
      j.start_obj();
      j.write_str("drx-CycleLength-r11", c.get<drx_assist_info_r11_s_>().drx_cycle_len_r11.to_string());
      if (c.get<drx_assist_info_r11_s_>().drx_offset_r11_present) {
        j.write_int("drx-Offset-r11", c.get<drx_assist_info_r11_s_>().drx_offset_r11);
      }
      j.write_str("drx-ActiveTime-r11", c.get<drx_assist_info_r11_s_>().drx_active_time_r11.to_string());
      j.end_obj();
      break;
    case types::idc_sf_pattern_list_r11:
      j.start_array("idc-SubframePatternList-r11");
      for (const auto& e1 : c.get<idc_sf_pattern_list_r11_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "tdm_assist_info_r11_c");
  }
  j.end_obj();
}
SRSASN_CODE tdm_assist_info_r11_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::drx_assist_info_r11:
      HANDLE_CODE(bref.pack(c.get<drx_assist_info_r11_s_>().drx_offset_r11_present, 1));
      HANDLE_CODE(c.get<drx_assist_info_r11_s_>().drx_cycle_len_r11.pack(bref));
      if (c.get<drx_assist_info_r11_s_>().drx_offset_r11_present) {
        HANDLE_CODE(pack_integer(bref, c.get<drx_assist_info_r11_s_>().drx_offset_r11, (uint16_t)0u, (uint16_t)255u));
      }
      HANDLE_CODE(c.get<drx_assist_info_r11_s_>().drx_active_time_r11.pack(bref));
      break;
    case types::idc_sf_pattern_list_r11:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<idc_sf_pattern_list_r11_l>(), 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "tdm_assist_info_r11_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE tdm_assist_info_r11_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::drx_assist_info_r11:
      HANDLE_CODE(bref.unpack(c.get<drx_assist_info_r11_s_>().drx_offset_r11_present, 1));
      HANDLE_CODE(c.get<drx_assist_info_r11_s_>().drx_cycle_len_r11.unpack(bref));
      if (c.get<drx_assist_info_r11_s_>().drx_offset_r11_present) {
        HANDLE_CODE(unpack_integer(c.get<drx_assist_info_r11_s_>().drx_offset_r11, bref, (uint16_t)0u, (uint16_t)255u));
      }
      HANDLE_CODE(c.get<drx_assist_info_r11_s_>().drx_active_time_r11.unpack(bref));
      break;
    case types::idc_sf_pattern_list_r11:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<idc_sf_pattern_list_r11_l>(), bref, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "tdm_assist_info_r11_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_cycle_len_r11_opts::to_string() const
{
  static const char* options[] = {"sf40", "sf64", "sf80", "sf128", "sf160", "sf256", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_cycle_len_r11_e_");
}
uint16_t tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_cycle_len_r11_opts::to_number() const
{
  static const uint16_t options[] = {40, 64, 80, 128, 160, 256};
  return map_enum_number(options, 6, value, "tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_cycle_len_r11_e_");
}

const char* tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_active_time_r11_opts::to_string() const
{
  static const char* options[] = {"sf20", "sf30", "sf40", "sf60", "sf80", "sf100", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_active_time_r11_e_");
}
uint8_t tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_active_time_r11_opts::to_number() const
{
  static const uint8_t options[] = {20, 30, 40, 60, 80, 100};
  return map_enum_number(options, 6, value, "tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_active_time_r11_e_");
}

// UEAssistanceInformation-v1430-IEs ::= SEQUENCE
SRSASN_CODE ueassist_info_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(bw_pref_r14_present, 1));
  HANDLE_CODE(bref.pack(sps_assist_info_r14_present, 1));
  HANDLE_CODE(bref.pack(rlm_report_r14_present, 1));
  HANDLE_CODE(bref.pack(delay_budget_report_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (bw_pref_r14_present) {
    HANDLE_CODE(bw_pref_r14.pack(bref));
  }
  if (sps_assist_info_r14_present) {
    HANDLE_CODE(bref.pack(sps_assist_info_r14.traffic_pattern_info_list_sl_r14_present, 1));
    HANDLE_CODE(bref.pack(sps_assist_info_r14.traffic_pattern_info_list_ul_r14_present, 1));
    if (sps_assist_info_r14.traffic_pattern_info_list_sl_r14_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, sps_assist_info_r14.traffic_pattern_info_list_sl_r14, 1, 8));
    }
    if (sps_assist_info_r14.traffic_pattern_info_list_ul_r14_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, sps_assist_info_r14.traffic_pattern_info_list_ul_r14, 1, 8));
    }
  }
  if (rlm_report_r14_present) {
    HANDLE_CODE(bref.pack(rlm_report_r14.excess_rep_mpdcch_r14_present, 1));
    HANDLE_CODE(rlm_report_r14.rlm_event_r14.pack(bref));
    if (rlm_report_r14.excess_rep_mpdcch_r14_present) {
      HANDLE_CODE(rlm_report_r14.excess_rep_mpdcch_r14.pack(bref));
    }
  }
  if (delay_budget_report_r14_present) {
    HANDLE_CODE(delay_budget_report_r14.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueassist_info_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(bw_pref_r14_present, 1));
  HANDLE_CODE(bref.unpack(sps_assist_info_r14_present, 1));
  HANDLE_CODE(bref.unpack(rlm_report_r14_present, 1));
  HANDLE_CODE(bref.unpack(delay_budget_report_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (bw_pref_r14_present) {
    HANDLE_CODE(bw_pref_r14.unpack(bref));
  }
  if (sps_assist_info_r14_present) {
    HANDLE_CODE(bref.unpack(sps_assist_info_r14.traffic_pattern_info_list_sl_r14_present, 1));
    HANDLE_CODE(bref.unpack(sps_assist_info_r14.traffic_pattern_info_list_ul_r14_present, 1));
    if (sps_assist_info_r14.traffic_pattern_info_list_sl_r14_present) {
      HANDLE_CODE(unpack_dyn_seq_of(sps_assist_info_r14.traffic_pattern_info_list_sl_r14, bref, 1, 8));
    }
    if (sps_assist_info_r14.traffic_pattern_info_list_ul_r14_present) {
      HANDLE_CODE(unpack_dyn_seq_of(sps_assist_info_r14.traffic_pattern_info_list_ul_r14, bref, 1, 8));
    }
  }
  if (rlm_report_r14_present) {
    HANDLE_CODE(bref.unpack(rlm_report_r14.excess_rep_mpdcch_r14_present, 1));
    HANDLE_CODE(rlm_report_r14.rlm_event_r14.unpack(bref));
    if (rlm_report_r14.excess_rep_mpdcch_r14_present) {
      HANDLE_CODE(rlm_report_r14.excess_rep_mpdcch_r14.unpack(bref));
    }
  }
  if (delay_budget_report_r14_present) {
    HANDLE_CODE(delay_budget_report_r14.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ueassist_info_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (bw_pref_r14_present) {
    j.write_fieldname("bw-Preference-r14");
    bw_pref_r14.to_json(j);
  }
  if (sps_assist_info_r14_present) {
    j.write_fieldname("sps-AssistanceInformation-r14");
    j.start_obj();
    if (sps_assist_info_r14.traffic_pattern_info_list_sl_r14_present) {
      j.start_array("trafficPatternInfoListSL-r14");
      for (const auto& e1 : sps_assist_info_r14.traffic_pattern_info_list_sl_r14) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (sps_assist_info_r14.traffic_pattern_info_list_ul_r14_present) {
      j.start_array("trafficPatternInfoListUL-r14");
      for (const auto& e1 : sps_assist_info_r14.traffic_pattern_info_list_ul_r14) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  if (rlm_report_r14_present) {
    j.write_fieldname("rlm-Report-r14");
    j.start_obj();
    j.write_str("rlm-Event-r14", rlm_report_r14.rlm_event_r14.to_string());
    if (rlm_report_r14.excess_rep_mpdcch_r14_present) {
      j.write_str("excessRep-MPDCCH-r14", rlm_report_r14.excess_rep_mpdcch_r14.to_string());
    }
    j.end_obj();
  }
  if (delay_budget_report_r14_present) {
    j.write_fieldname("delayBudgetReport-r14");
    delay_budget_report_r14.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* ueassist_info_v1430_ies_s::rlm_report_r14_s_::rlm_event_r14_opts::to_string() const
{
  static const char* options[] = {"earlyOutOfSync", "earlyInSync"};
  return convert_enum_idx(options, 2, value, "ueassist_info_v1430_ies_s::rlm_report_r14_s_::rlm_event_r14_e_");
}

const char* ueassist_info_v1430_ies_s::rlm_report_r14_s_::excess_rep_mpdcch_r14_opts::to_string() const
{
  static const char* options[] = {"excessRep1", "excessRep2"};
  return convert_enum_idx(options, 2, value, "ueassist_info_v1430_ies_s::rlm_report_r14_s_::excess_rep_mpdcch_r14_e_");
}
uint8_t ueassist_info_v1430_ies_s::rlm_report_r14_s_::excess_rep_mpdcch_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "ueassist_info_v1430_ies_s::rlm_report_r14_s_::excess_rep_mpdcch_r14_e_");
}

// UEInformationResponse-v930-IEs ::= SEQUENCE
SRSASN_CODE ue_info_resp_v930_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE ue_info_resp_v930_ies_s::unpack(cbit_ref& bref)
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
void ue_info_resp_v930_ies_s::to_json(json_writer& j) const
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

// ULHandoverPreparationTransfer-v8a0-IEs ::= SEQUENCE
SRSASN_CODE ul_ho_prep_transfer_v8a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ho_prep_transfer_v8a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_ho_prep_transfer_v8a0_ies_s::to_json(json_writer& j) const
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

// ULInformationTransfer-v8a0-IEs ::= SEQUENCE
SRSASN_CODE ul_info_transfer_v8a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_v8a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_info_transfer_v8a0_ies_s::to_json(json_writer& j) const
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

// WLAN-Status-r13 ::= ENUMERATED
const char* wlan_status_r13_opts::to_string() const
{
  static const char* options[] = {
      "successfulAssociation", "failureWlanRadioLink", "failureWlanUnavailable", "failureTimeout"};
  return convert_enum_idx(options, 4, value, "wlan_status_r13_e");
}

// WLANConnectionStatusReport-v1430-IEs ::= SEQUENCE
SRSASN_CODE wlan_conn_status_report_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(wlan_status_v1430.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_conn_status_report_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(wlan_status_v1430.unpack(bref));

  return SRSASN_SUCCESS;
}
void wlan_conn_status_report_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("wlan-Status-v1430", wlan_status_v1430.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// CSFBParametersRequestCDMA2000-r8-IEs ::= SEQUENCE
SRSASN_CODE csfb_params_request_cdma2000_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csfb_params_request_cdma2000_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csfb_params_request_cdma2000_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CounterCheckResponse-r8-IEs ::= SEQUENCE
SRSASN_CODE counter_check_resp_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, drb_count_info_list, 0, 11));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_resp_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(drb_count_info_list, bref, 0, 11));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void counter_check_resp_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("drb-CountInfoList");
  for (const auto& e1 : drb_count_info_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// FailedLogicalChannelInfo-r15 ::= SEQUENCE
SRSASN_CODE failed_lc_ch_info_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(failed_lc_ch_id_r15.lc_ch_id_r15_present, 1));
  HANDLE_CODE(bref.pack(failed_lc_ch_id_r15.lc_ch_id_ext_r15_present, 1));
  HANDLE_CODE(failed_lc_ch_id_r15.cell_group_ind_r15.pack(bref));
  if (failed_lc_ch_id_r15.lc_ch_id_r15_present) {
    HANDLE_CODE(pack_integer(bref, failed_lc_ch_id_r15.lc_ch_id_r15, (uint8_t)1u, (uint8_t)10u));
  }
  if (failed_lc_ch_id_r15.lc_ch_id_ext_r15_present) {
    HANDLE_CODE(pack_integer(bref, failed_lc_ch_id_r15.lc_ch_id_ext_r15, (uint8_t)32u, (uint8_t)38u));
  }
  HANDLE_CODE(fail_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE failed_lc_ch_info_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(failed_lc_ch_id_r15.lc_ch_id_r15_present, 1));
  HANDLE_CODE(bref.unpack(failed_lc_ch_id_r15.lc_ch_id_ext_r15_present, 1));
  HANDLE_CODE(failed_lc_ch_id_r15.cell_group_ind_r15.unpack(bref));
  if (failed_lc_ch_id_r15.lc_ch_id_r15_present) {
    HANDLE_CODE(unpack_integer(failed_lc_ch_id_r15.lc_ch_id_r15, bref, (uint8_t)1u, (uint8_t)10u));
  }
  if (failed_lc_ch_id_r15.lc_ch_id_ext_r15_present) {
    HANDLE_CODE(unpack_integer(failed_lc_ch_id_r15.lc_ch_id_ext_r15, bref, (uint8_t)32u, (uint8_t)38u));
  }
  HANDLE_CODE(fail_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void failed_lc_ch_info_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("failedLogicalChannelIdentity-r15");
  j.start_obj();
  j.write_str("cellGroupIndication-r15", failed_lc_ch_id_r15.cell_group_ind_r15.to_string());
  if (failed_lc_ch_id_r15.lc_ch_id_r15_present) {
    j.write_int("logicalChannelIdentity-r15", failed_lc_ch_id_r15.lc_ch_id_r15);
  }
  if (failed_lc_ch_id_r15.lc_ch_id_ext_r15_present) {
    j.write_int("logicalChannelIdentityExt-r15", failed_lc_ch_id_r15.lc_ch_id_ext_r15);
  }
  j.end_obj();
  j.write_str("failureType", fail_type.to_string());
  j.end_obj();
}

const char* failed_lc_ch_info_r15_s::failed_lc_ch_id_r15_s_::cell_group_ind_r15_opts::to_string() const
{
  static const char* options[] = {"mn", "sn"};
  return convert_enum_idx(options, 2, value, "failed_lc_ch_info_r15_s::failed_lc_ch_id_r15_s_::cell_group_ind_r15_e_");
}

const char* failed_lc_ch_info_r15_s::fail_type_opts::to_string() const
{
  static const char* options[] = {"duplication", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "failed_lc_ch_info_r15_s::fail_type_e_");
}

// InDeviceCoexIndication-r11-IEs ::= SEQUENCE
SRSASN_CODE in_dev_coex_ind_r11_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(affected_carrier_freq_list_r11_present, 1));
  HANDLE_CODE(bref.pack(tdm_assist_info_r11_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (affected_carrier_freq_list_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, affected_carrier_freq_list_r11, 1, 32));
  }
  if (tdm_assist_info_r11_present) {
    HANDLE_CODE(tdm_assist_info_r11.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE in_dev_coex_ind_r11_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(affected_carrier_freq_list_r11_present, 1));
  HANDLE_CODE(bref.unpack(tdm_assist_info_r11_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (affected_carrier_freq_list_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_list_r11, bref, 1, 32));
  }
  if (tdm_assist_info_r11_present) {
    HANDLE_CODE(tdm_assist_info_r11.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void in_dev_coex_ind_r11_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (affected_carrier_freq_list_r11_present) {
    j.start_array("affectedCarrierFreqList-r11");
    for (const auto& e1 : affected_carrier_freq_list_r11) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (tdm_assist_info_r11_present) {
    j.write_fieldname("tdm-AssistanceInfo-r11");
    tdm_assist_info_r11.to_json(j);
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

// InterFreqRSTDMeasurementIndication-r10-IEs ::= SEQUENCE
SRSASN_CODE inter_freq_rstd_meas_ind_r10_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(rstd_inter_freq_ind_r10.pack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_rstd_meas_ind_r10_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(rstd_inter_freq_ind_r10.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_rstd_meas_ind_r10_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rstd-InterFreqIndication-r10");
  rstd_inter_freq_ind_r10.to_json(j);
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

void inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_::set(types::options e)
{
  type_ = e;
}
inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_::start_s_&
inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_::set_start()
{
  set(types::start);
  return c;
}
void inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_::set_stop()
{
  set(types::stop);
}
void inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::start:
      j.write_fieldname("start");
      j.start_obj();
      j.start_array("rstd-InterFreqInfoList-r10");
      for (const auto& e1 : c.rstd_inter_freq_info_list_r10) {
        e1.to_json(j);
      }
      j.end_array();
      j.end_obj();
      break;
    case types::stop:
      break;
    default:
      log_invalid_choice_id(type_, "inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::start:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.rstd_inter_freq_info_list_r10, 1, 3));
      break;
    case types::stop:
      break;
    default:
      log_invalid_choice_id(type_, "inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::start:
      HANDLE_CODE(unpack_dyn_seq_of(c.rstd_inter_freq_info_list_r10, bref, 1, 3));
      break;
    case types::stop:
      break;
    default:
      log_invalid_choice_id(type_, "inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MBMSCountingResponse-r10-IEs ::= SEQUENCE
SRSASN_CODE mbms_count_resp_r10_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mbsfn_area_idx_r10_present, 1));
  HANDLE_CODE(bref.pack(count_resp_list_r10_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mbsfn_area_idx_r10_present) {
    HANDLE_CODE(pack_integer(bref, mbsfn_area_idx_r10, (uint8_t)0u, (uint8_t)7u));
  }
  if (count_resp_list_r10_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, count_resp_list_r10, 1, 16));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_count_resp_r10_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mbsfn_area_idx_r10_present, 1));
  HANDLE_CODE(bref.unpack(count_resp_list_r10_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mbsfn_area_idx_r10_present) {
    HANDLE_CODE(unpack_integer(mbsfn_area_idx_r10, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (count_resp_list_r10_present) {
    HANDLE_CODE(unpack_dyn_seq_of(count_resp_list_r10, bref, 1, 16));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mbms_count_resp_r10_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbsfn_area_idx_r10_present) {
    j.write_int("mbsfn-AreaIndex-r10", mbsfn_area_idx_r10);
  }
  if (count_resp_list_r10_present) {
    j.start_array("countingResponseList-r10");
    for (const auto& e1 : count_resp_list_r10) {
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

// MBMSInterestIndication-r11-IEs ::= SEQUENCE
SRSASN_CODE mbms_interest_ind_r11_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mbms_freq_list_r11_present, 1));
  HANDLE_CODE(bref.pack(mbms_prio_r11_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mbms_freq_list_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbms_freq_list_r11, 1, 5, integer_packer<uint32_t>(0, 262143)));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_interest_ind_r11_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mbms_freq_list_r11_present, 1));
  HANDLE_CODE(bref.unpack(mbms_prio_r11_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mbms_freq_list_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbms_freq_list_r11, bref, 1, 5, integer_packer<uint32_t>(0, 262143)));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mbms_interest_ind_r11_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbms_freq_list_r11_present) {
    j.start_array("mbms-FreqList-r11");
    for (const auto& e1 : mbms_freq_list_r11) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (mbms_prio_r11_present) {
    j.write_str("mbms-Priority-r11", "true");
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

// MeasReportAppLayer-r15-IEs ::= SEQUENCE
SRSASN_CODE meas_report_app_layer_r15_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_report_app_layer_container_r15_present, 1));
  HANDLE_CODE(bref.pack(service_type_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_report_app_layer_container_r15_present) {
    HANDLE_CODE(meas_report_app_layer_container_r15.pack(bref));
  }
  if (service_type_r15_present) {
    HANDLE_CODE(service_type_r15.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_app_layer_r15_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_report_app_layer_container_r15_present, 1));
  HANDLE_CODE(bref.unpack(service_type_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_report_app_layer_container_r15_present) {
    HANDLE_CODE(meas_report_app_layer_container_r15.unpack(bref));
  }
  if (service_type_r15_present) {
    HANDLE_CODE(service_type_r15.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_report_app_layer_r15_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_report_app_layer_container_r15_present) {
    j.write_str("measReportAppLayerContainer-r15", meas_report_app_layer_container_r15.to_string());
  }
  if (service_type_r15_present) {
    j.write_str("serviceType-r15", service_type_r15.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* meas_report_app_layer_r15_ies_s::service_type_r15_opts::to_string() const
{
  static const char* options[] = {"qoe", "qoemtsi", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "meas_report_app_layer_r15_ies_s::service_type_r15_e_");
}

// ProximityIndication-r9-IEs ::= SEQUENCE
SRSASN_CODE proximity_ind_r9_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(type_r9.pack(bref));
  HANDLE_CODE(carrier_freq_r9.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE proximity_ind_r9_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(type_r9.unpack(bref));
  HANDLE_CODE(carrier_freq_r9.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void proximity_ind_r9_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("type-r9", type_r9.to_string());
  j.write_fieldname("carrierFreq-r9");
  carrier_freq_r9.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* proximity_ind_r9_ies_s::type_r9_opts::to_string() const
{
  static const char* options[] = {"entering", "leaving"};
  return convert_enum_idx(options, 2, value, "proximity_ind_r9_ies_s::type_r9_e_");
}

void proximity_ind_r9_ies_s::carrier_freq_r9_c_::destroy_() {}
void proximity_ind_r9_ies_s::carrier_freq_r9_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
proximity_ind_r9_ies_s::carrier_freq_r9_c_::carrier_freq_r9_c_(const proximity_ind_r9_ies_s::carrier_freq_r9_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::eutra_r9:
      c.init(other.c.get<uint32_t>());
      break;
    case types::utra_r9:
      c.init(other.c.get<uint16_t>());
      break;
    case types::eutra2_v9e0:
      c.init(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "proximity_ind_r9_ies_s::carrier_freq_r9_c_");
  }
}
proximity_ind_r9_ies_s::carrier_freq_r9_c_&
proximity_ind_r9_ies_s::carrier_freq_r9_c_::operator=(const proximity_ind_r9_ies_s::carrier_freq_r9_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::eutra_r9:
      c.set(other.c.get<uint32_t>());
      break;
    case types::utra_r9:
      c.set(other.c.get<uint16_t>());
      break;
    case types::eutra2_v9e0:
      c.set(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "proximity_ind_r9_ies_s::carrier_freq_r9_c_");
  }

  return *this;
}
uint32_t& proximity_ind_r9_ies_s::carrier_freq_r9_c_::set_eutra_r9()
{
  set(types::eutra_r9);
  return c.get<uint32_t>();
}
uint16_t& proximity_ind_r9_ies_s::carrier_freq_r9_c_::set_utra_r9()
{
  set(types::utra_r9);
  return c.get<uint16_t>();
}
uint32_t& proximity_ind_r9_ies_s::carrier_freq_r9_c_::set_eutra2_v9e0()
{
  set(types::eutra2_v9e0);
  return c.get<uint32_t>();
}
void proximity_ind_r9_ies_s::carrier_freq_r9_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::eutra_r9:
      j.write_int("eutra-r9", c.get<uint32_t>());
      break;
    case types::utra_r9:
      j.write_int("utra-r9", c.get<uint16_t>());
      break;
    case types::eutra2_v9e0:
      j.write_int("eutra2-v9e0", c.get<uint32_t>());
      break;
    default:
      log_invalid_choice_id(type_, "proximity_ind_r9_ies_s::carrier_freq_r9_c_");
  }
  j.end_obj();
}
SRSASN_CODE proximity_ind_r9_ies_s::carrier_freq_r9_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::eutra_r9:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)0u, (uint32_t)65535u));
      break;
    case types::utra_r9:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)16383u));
      break;
    case types::eutra2_v9e0: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)65536u, (uint32_t)262143u));
    } break;
    default:
      log_invalid_choice_id(type_, "proximity_ind_r9_ies_s::carrier_freq_r9_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE proximity_ind_r9_ies_s::carrier_freq_r9_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::eutra_r9:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)0u, (uint32_t)65535u));
      break;
    case types::utra_r9:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)16383u));
      break;
    case types::eutra2_v9e0: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)65536u, (uint32_t)262143u));
    } break;
    default:
      log_invalid_choice_id(type_, "proximity_ind_r9_ies_s::carrier_freq_r9_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RNReconfigurationComplete-r10-IEs ::= SEQUENCE
SRSASN_CODE rn_recfg_complete_r10_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_recfg_complete_r10_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rn_recfg_complete_r10_ies_s::to_json(json_writer& j) const
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

// RRCConnectionReconfigurationComplete-r8-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_complete_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_complete_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_complete_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReestablishmentComplete-r8-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_complete_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_complete_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_complete_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionResumeComplete-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_complete_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sel_plmn_id_r13_present, 1));
  HANDLE_CODE(bref.pack(ded_info_nas_r13_present, 1));
  HANDLE_CODE(bref.pack(rlf_info_available_r13_present, 1));
  HANDLE_CODE(bref.pack(log_meas_available_r13_present, 1));
  HANDLE_CODE(bref.pack(conn_est_fail_info_available_r13_present, 1));
  HANDLE_CODE(bref.pack(mob_state_r13_present, 1));
  HANDLE_CODE(bref.pack(mob_history_avail_r13_present, 1));
  HANDLE_CODE(bref.pack(log_meas_available_mbsfn_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sel_plmn_id_r13_present) {
    HANDLE_CODE(pack_integer(bref, sel_plmn_id_r13, (uint8_t)1u, (uint8_t)6u));
  }
  if (ded_info_nas_r13_present) {
    HANDLE_CODE(ded_info_nas_r13.pack(bref));
  }
  if (mob_state_r13_present) {
    HANDLE_CODE(mob_state_r13.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_complete_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sel_plmn_id_r13_present, 1));
  HANDLE_CODE(bref.unpack(ded_info_nas_r13_present, 1));
  HANDLE_CODE(bref.unpack(rlf_info_available_r13_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_available_r13_present, 1));
  HANDLE_CODE(bref.unpack(conn_est_fail_info_available_r13_present, 1));
  HANDLE_CODE(bref.unpack(mob_state_r13_present, 1));
  HANDLE_CODE(bref.unpack(mob_history_avail_r13_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_available_mbsfn_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sel_plmn_id_r13_present) {
    HANDLE_CODE(unpack_integer(sel_plmn_id_r13, bref, (uint8_t)1u, (uint8_t)6u));
  }
  if (ded_info_nas_r13_present) {
    HANDLE_CODE(ded_info_nas_r13.unpack(bref));
  }
  if (mob_state_r13_present) {
    HANDLE_CODE(mob_state_r13.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_complete_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sel_plmn_id_r13_present) {
    j.write_int("selectedPLMN-Identity-r13", sel_plmn_id_r13);
  }
  if (ded_info_nas_r13_present) {
    j.write_str("dedicatedInfoNAS-r13", ded_info_nas_r13.to_string());
  }
  if (rlf_info_available_r13_present) {
    j.write_str("rlf-InfoAvailable-r13", "true");
  }
  if (log_meas_available_r13_present) {
    j.write_str("logMeasAvailable-r13", "true");
  }
  if (conn_est_fail_info_available_r13_present) {
    j.write_str("connEstFailInfoAvailable-r13", "true");
  }
  if (mob_state_r13_present) {
    j.write_str("mobilityState-r13", mob_state_r13.to_string());
  }
  if (mob_history_avail_r13_present) {
    j.write_str("mobilityHistoryAvail-r13", "true");
  }
  if (log_meas_available_mbsfn_r13_present) {
    j.write_str("logMeasAvailableMBSFN-r13", "true");
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

const char* rrc_conn_resume_complete_r13_ies_s::mob_state_r13_opts::to_string() const
{
  static const char* options[] = {"normal", "medium", "high", "spare"};
  return convert_enum_idx(options, 4, value, "rrc_conn_resume_complete_r13_ies_s::mob_state_r13_e_");
}

// RRCConnectionSetupComplete-r8-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(registered_mme_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_integer(bref, sel_plmn_id, (uint8_t)1u, (uint8_t)6u));
  if (registered_mme_present) {
    HANDLE_CODE(registered_mme.pack(bref));
  }
  HANDLE_CODE(ded_info_nas.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(registered_mme_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_integer(sel_plmn_id, bref, (uint8_t)1u, (uint8_t)6u));
  if (registered_mme_present) {
    HANDLE_CODE(registered_mme.unpack(bref));
  }
  HANDLE_CODE(ded_info_nas.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_complete_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("selectedPLMN-Identity", sel_plmn_id);
  if (registered_mme_present) {
    j.write_fieldname("registeredMME");
    registered_mme.to_json(j);
  }
  j.write_str("dedicatedInfoNAS", ded_info_nas.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SCGFailureInformation-r12-IEs ::= SEQUENCE
SRSASN_CODE scg_fail_info_r12_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fail_report_scg_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fail_report_scg_r12_present) {
    HANDLE_CODE(fail_report_scg_r12.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_r12_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fail_report_scg_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fail_report_scg_r12_present) {
    HANDLE_CODE(fail_report_scg_r12.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_fail_info_r12_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fail_report_scg_r12_present) {
    j.write_fieldname("failureReportSCG-r12");
    fail_report_scg_r12.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SCGFailureInformationNR-r15-IEs ::= SEQUENCE
SRSASN_CODE scg_fail_info_nr_r15_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fail_report_scg_nr_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fail_report_scg_nr_r15_present) {
    HANDLE_CODE(fail_report_scg_nr_r15.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_nr_r15_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fail_report_scg_nr_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fail_report_scg_nr_r15_present) {
    HANDLE_CODE(fail_report_scg_nr_r15.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_fail_info_nr_r15_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fail_report_scg_nr_r15_present) {
    j.write_fieldname("failureReportSCG-NR-r15");
    fail_report_scg_nr_r15.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SidelinkUEInformation-r12-IEs ::= SEQUENCE
SRSASN_CODE sidelink_ue_info_r12_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(comm_rx_interested_freq_r12_present, 1));
  HANDLE_CODE(bref.pack(comm_tx_res_req_r12_present, 1));
  HANDLE_CODE(bref.pack(disc_rx_interest_r12_present, 1));
  HANDLE_CODE(bref.pack(disc_tx_res_req_r12_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (comm_rx_interested_freq_r12_present) {
    HANDLE_CODE(pack_integer(bref, comm_rx_interested_freq_r12, (uint32_t)0u, (uint32_t)262143u));
  }
  if (comm_tx_res_req_r12_present) {
    HANDLE_CODE(comm_tx_res_req_r12.pack(bref));
  }
  if (disc_tx_res_req_r12_present) {
    HANDLE_CODE(pack_integer(bref, disc_tx_res_req_r12, (uint8_t)1u, (uint8_t)63u));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sidelink_ue_info_r12_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(comm_rx_interested_freq_r12_present, 1));
  HANDLE_CODE(bref.unpack(comm_tx_res_req_r12_present, 1));
  HANDLE_CODE(bref.unpack(disc_rx_interest_r12_present, 1));
  HANDLE_CODE(bref.unpack(disc_tx_res_req_r12_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (comm_rx_interested_freq_r12_present) {
    HANDLE_CODE(unpack_integer(comm_rx_interested_freq_r12, bref, (uint32_t)0u, (uint32_t)262143u));
  }
  if (comm_tx_res_req_r12_present) {
    HANDLE_CODE(comm_tx_res_req_r12.unpack(bref));
  }
  if (disc_tx_res_req_r12_present) {
    HANDLE_CODE(unpack_integer(disc_tx_res_req_r12, bref, (uint8_t)1u, (uint8_t)63u));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sidelink_ue_info_r12_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (comm_rx_interested_freq_r12_present) {
    j.write_int("commRxInterestedFreq-r12", comm_rx_interested_freq_r12);
  }
  if (comm_tx_res_req_r12_present) {
    j.write_fieldname("commTxResourceReq-r12");
    comm_tx_res_req_r12.to_json(j);
  }
  if (disc_rx_interest_r12_present) {
    j.write_str("discRxInterest-r12", "true");
  }
  if (disc_tx_res_req_r12_present) {
    j.write_int("discTxResourceReq-r12", disc_tx_res_req_r12);
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

// UEAssistanceInformation-r11-IEs ::= SEQUENCE
SRSASN_CODE ueassist_info_r11_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pwr_pref_ind_r11_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (pwr_pref_ind_r11_present) {
    HANDLE_CODE(pwr_pref_ind_r11.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueassist_info_r11_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pwr_pref_ind_r11_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (pwr_pref_ind_r11_present) {
    HANDLE_CODE(pwr_pref_ind_r11.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ueassist_info_r11_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pwr_pref_ind_r11_present) {
    j.write_str("powerPrefIndication-r11", pwr_pref_ind_r11.to_string());
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

const char* ueassist_info_r11_ies_s::pwr_pref_ind_r11_opts::to_string() const
{
  static const char* options[] = {"normal", "lowPowerConsumption"};
  return convert_enum_idx(options, 2, value, "ueassist_info_r11_ies_s::pwr_pref_ind_r11_e_");
}

// UEInformationResponse-r9-IEs ::= SEQUENCE
SRSASN_CODE ue_info_resp_r9_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rach_report_r9_present, 1));
  HANDLE_CODE(bref.pack(rlf_report_r9_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rach_report_r9_present) {
    HANDLE_CODE(pack_integer(bref, rach_report_r9.nof_preambs_sent_r9, (uint8_t)1u, (uint8_t)200u));
    HANDLE_CODE(bref.pack(rach_report_r9.contention_detected_r9, 1));
  }
  if (rlf_report_r9_present) {
    HANDLE_CODE(rlf_report_r9.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_resp_r9_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rach_report_r9_present, 1));
  HANDLE_CODE(bref.unpack(rlf_report_r9_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rach_report_r9_present) {
    HANDLE_CODE(unpack_integer(rach_report_r9.nof_preambs_sent_r9, bref, (uint8_t)1u, (uint8_t)200u));
    HANDLE_CODE(bref.unpack(rach_report_r9.contention_detected_r9, 1));
  }
  if (rlf_report_r9_present) {
    HANDLE_CODE(rlf_report_r9.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_info_resp_r9_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rach_report_r9_present) {
    j.write_fieldname("rach-Report-r9");
    j.start_obj();
    j.write_int("numberOfPreamblesSent-r9", rach_report_r9.nof_preambs_sent_r9);
    j.write_bool("contentionDetected-r9", rach_report_r9.contention_detected_r9);
    j.end_obj();
  }
  if (rlf_report_r9_present) {
    j.write_fieldname("rlf-Report-r9");
    rlf_report_r9.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// ULHandoverPreparationTransfer-r8-IEs ::= SEQUENCE
SRSASN_CODE ul_ho_prep_transfer_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meid_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(cdma2000_type.pack(bref));
  if (meid_present) {
    HANDLE_CODE(meid.pack(bref));
  }
  HANDLE_CODE(ded_info.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ho_prep_transfer_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meid_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(cdma2000_type.unpack(bref));
  if (meid_present) {
    HANDLE_CODE(meid.unpack(bref));
  }
  HANDLE_CODE(ded_info.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_ho_prep_transfer_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cdma2000-Type", cdma2000_type.to_string());
  if (meid_present) {
    j.write_str("meid", meid.to_string());
  }
  j.write_str("dedicatedInfo", ded_info.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// ULInformationTransfer-r8-IEs ::= SEQUENCE
SRSASN_CODE ul_info_transfer_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ded_info_type.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ded_info_type.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_info_transfer_r8_ies_s::to_json(json_writer& j) const
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

void ul_info_transfer_r8_ies_s::ded_info_type_c_::destroy_()
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
void ul_info_transfer_r8_ies_s::ded_info_type_c_::set(types::options e)
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
      log_invalid_choice_id(type_, "ul_info_transfer_r8_ies_s::ded_info_type_c_");
  }
}
ul_info_transfer_r8_ies_s::ded_info_type_c_::ded_info_type_c_(const ul_info_transfer_r8_ies_s::ded_info_type_c_& other)
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
      log_invalid_choice_id(type_, "ul_info_transfer_r8_ies_s::ded_info_type_c_");
  }
}
ul_info_transfer_r8_ies_s::ded_info_type_c_&
ul_info_transfer_r8_ies_s::ded_info_type_c_::operator=(const ul_info_transfer_r8_ies_s::ded_info_type_c_& other)
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
      log_invalid_choice_id(type_, "ul_info_transfer_r8_ies_s::ded_info_type_c_");
  }

  return *this;
}
dyn_octstring& ul_info_transfer_r8_ies_s::ded_info_type_c_::set_ded_info_nas()
{
  set(types::ded_info_nas);
  return c.get<dyn_octstring>();
}
dyn_octstring& ul_info_transfer_r8_ies_s::ded_info_type_c_::set_ded_info_cdma2000_minus1_xrtt()
{
  set(types::ded_info_cdma2000_minus1_xrtt);
  return c.get<dyn_octstring>();
}
dyn_octstring& ul_info_transfer_r8_ies_s::ded_info_type_c_::set_ded_info_cdma2000_hrpd()
{
  set(types::ded_info_cdma2000_hrpd);
  return c.get<dyn_octstring>();
}
void ul_info_transfer_r8_ies_s::ded_info_type_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ul_info_transfer_r8_ies_s::ded_info_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_info_transfer_r8_ies_s::ded_info_type_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "ul_info_transfer_r8_ies_s::ded_info_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_r8_ies_s::ded_info_type_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ul_info_transfer_r8_ies_s::ded_info_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ULInformationTransferMRDC-r15-IEs ::= SEQUENCE
SRSASN_CODE ul_info_transfer_mrdc_r15_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_dcch_msg_nr_r15_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ul_dcch_msg_nr_r15_present) {
    HANDLE_CODE(ul_dcch_msg_nr_r15.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_mrdc_r15_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_dcch_msg_nr_r15_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ul_dcch_msg_nr_r15_present) {
    HANDLE_CODE(ul_dcch_msg_nr_r15.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_info_transfer_mrdc_r15_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_dcch_msg_nr_r15_present) {
    j.write_str("ul-DCCH-MessageNR-r15", ul_dcch_msg_nr_r15.to_string());
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

// WLANConnectionStatusReport-r13-IEs ::= SEQUENCE
SRSASN_CODE wlan_conn_status_report_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(wlan_status_r13.pack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_conn_status_report_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(wlan_status_r13.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void wlan_conn_status_report_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("wlan-Status-r13", wlan_status_r13.to_string());
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CSFBParametersRequestCDMA2000 ::= SEQUENCE
SRSASN_CODE csfb_params_request_cdma2000_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csfb_params_request_cdma2000_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void csfb_params_request_cdma2000_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void csfb_params_request_cdma2000_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
csfb_params_request_cdma2000_r8_ies_s&
csfb_params_request_cdma2000_s::crit_exts_c_::set_csfb_params_request_cdma2000_r8()
{
  set(types::csfb_params_request_cdma2000_r8);
  return c;
}
void csfb_params_request_cdma2000_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void csfb_params_request_cdma2000_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::csfb_params_request_cdma2000_r8:
      j.write_fieldname("csfbParametersRequestCDMA2000-r8");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "csfb_params_request_cdma2000_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE csfb_params_request_cdma2000_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::csfb_params_request_cdma2000_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "csfb_params_request_cdma2000_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csfb_params_request_cdma2000_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::csfb_params_request_cdma2000_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "csfb_params_request_cdma2000_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CounterCheckResponse ::= SEQUENCE
SRSASN_CODE counter_check_resp_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_resp_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void counter_check_resp_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void counter_check_resp_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
counter_check_resp_r8_ies_s& counter_check_resp_s::crit_exts_c_::set_counter_check_resp_r8()
{
  set(types::counter_check_resp_r8);
  return c;
}
void counter_check_resp_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void counter_check_resp_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::counter_check_resp_r8:
      j.write_fieldname("counterCheckResponse-r8");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_resp_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE counter_check_resp_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::counter_check_resp_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_resp_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_resp_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::counter_check_resp_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_resp_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// FailureInformation-r15 ::= SEQUENCE
SRSASN_CODE fail_info_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(failed_lc_ch_info_r15_present, 1));

  if (failed_lc_ch_info_r15_present) {
    HANDLE_CODE(failed_lc_ch_info_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_info_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(failed_lc_ch_info_r15_present, 1));

  if (failed_lc_ch_info_r15_present) {
    HANDLE_CODE(failed_lc_ch_info_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void fail_info_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (failed_lc_ch_info_r15_present) {
    j.write_fieldname("failedLogicalChannelInfo-r15");
    failed_lc_ch_info_r15.to_json(j);
  }
  j.end_obj();
}

// InDeviceCoexIndication-r11 ::= SEQUENCE
SRSASN_CODE in_dev_coex_ind_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE in_dev_coex_ind_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void in_dev_coex_ind_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void in_dev_coex_ind_r11_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_& in_dev_coex_ind_r11_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void in_dev_coex_ind_r11_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void in_dev_coex_ind_r11_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "in_dev_coex_ind_r11_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE in_dev_coex_ind_r11_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "in_dev_coex_ind_r11_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE in_dev_coex_ind_r11_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "in_dev_coex_ind_r11_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
in_dev_coex_ind_r11_ies_s& in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_::set_in_dev_coex_ind_r11()
{
  set(types::in_dev_coex_ind_r11);
  return c;
}
void in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::in_dev_coex_ind_r11:
      j.write_fieldname("inDeviceCoexIndication-r11");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::in_dev_coex_ind_r11:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::in_dev_coex_ind_r11:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// InterFreqRSTDMeasurementIndication-r10 ::= SEQUENCE
SRSASN_CODE inter_freq_rstd_meas_ind_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_rstd_meas_ind_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void inter_freq_rstd_meas_ind_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_& inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "inter_freq_rstd_meas_ind_r10_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "inter_freq_rstd_meas_ind_r10_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "inter_freq_rstd_meas_ind_r10_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
inter_freq_rstd_meas_ind_r10_ies_s&
inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_::set_inter_freq_rstd_meas_ind_r10()
{
  set(types::inter_freq_rstd_meas_ind_r10);
  return c;
}
void inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::inter_freq_rstd_meas_ind_r10:
      j.write_fieldname("interFreqRSTDMeasurementIndication-r10");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::inter_freq_rstd_meas_ind_r10:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::inter_freq_rstd_meas_ind_r10:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MBMSCountingResponse-r10 ::= SEQUENCE
SRSASN_CODE mbms_count_resp_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_count_resp_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void mbms_count_resp_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void mbms_count_resp_r10_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
mbms_count_resp_r10_s::crit_exts_c_::c1_c_& mbms_count_resp_r10_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void mbms_count_resp_r10_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void mbms_count_resp_r10_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "mbms_count_resp_r10_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE mbms_count_resp_r10_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mbms_count_resp_r10_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_count_resp_r10_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "mbms_count_resp_r10_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void mbms_count_resp_r10_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
mbms_count_resp_r10_ies_s& mbms_count_resp_r10_s::crit_exts_c_::c1_c_::set_count_resp_r10()
{
  set(types::count_resp_r10);
  return c;
}
void mbms_count_resp_r10_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void mbms_count_resp_r10_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void mbms_count_resp_r10_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void mbms_count_resp_r10_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::count_resp_r10:
      j.write_fieldname("countingResponse-r10");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mbms_count_resp_r10_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE mbms_count_resp_r10_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::count_resp_r10:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mbms_count_resp_r10_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_count_resp_r10_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::count_resp_r10:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mbms_count_resp_r10_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MBMSInterestIndication-r11 ::= SEQUENCE
SRSASN_CODE mbms_interest_ind_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_interest_ind_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void mbms_interest_ind_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void mbms_interest_ind_r11_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
mbms_interest_ind_r11_s::crit_exts_c_::c1_c_& mbms_interest_ind_r11_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void mbms_interest_ind_r11_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void mbms_interest_ind_r11_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "mbms_interest_ind_r11_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE mbms_interest_ind_r11_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mbms_interest_ind_r11_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_interest_ind_r11_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "mbms_interest_ind_r11_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void mbms_interest_ind_r11_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
mbms_interest_ind_r11_ies_s& mbms_interest_ind_r11_s::crit_exts_c_::c1_c_::set_interest_ind_r11()
{
  set(types::interest_ind_r11);
  return c;
}
void mbms_interest_ind_r11_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void mbms_interest_ind_r11_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void mbms_interest_ind_r11_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void mbms_interest_ind_r11_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::interest_ind_r11:
      j.write_fieldname("interestIndication-r11");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mbms_interest_ind_r11_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE mbms_interest_ind_r11_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::interest_ind_r11:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mbms_interest_ind_r11_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_interest_ind_r11_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::interest_ind_r11:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mbms_interest_ind_r11_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MeasReportAppLayer-r15 ::= SEQUENCE
SRSASN_CODE meas_report_app_layer_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_app_layer_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_report_app_layer_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void meas_report_app_layer_r15_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
meas_report_app_layer_r15_ies_s& meas_report_app_layer_r15_s::crit_exts_c_::set_meas_report_app_layer_r15()
{
  set(types::meas_report_app_layer_r15);
  return c;
}
void meas_report_app_layer_r15_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void meas_report_app_layer_r15_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_report_app_layer_r15:
      j.write_fieldname("measReportAppLayer-r15");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_app_layer_r15_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_report_app_layer_r15_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_report_app_layer_r15:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_app_layer_r15_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_app_layer_r15_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_report_app_layer_r15:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_app_layer_r15_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ProximityIndication-r9 ::= SEQUENCE
SRSASN_CODE proximity_ind_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE proximity_ind_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void proximity_ind_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void proximity_ind_r9_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
proximity_ind_r9_s::crit_exts_c_::c1_c_& proximity_ind_r9_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void proximity_ind_r9_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void proximity_ind_r9_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "proximity_ind_r9_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE proximity_ind_r9_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "proximity_ind_r9_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE proximity_ind_r9_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "proximity_ind_r9_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void proximity_ind_r9_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
proximity_ind_r9_ies_s& proximity_ind_r9_s::crit_exts_c_::c1_c_::set_proximity_ind_r9()
{
  set(types::proximity_ind_r9);
  return c;
}
void proximity_ind_r9_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void proximity_ind_r9_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void proximity_ind_r9_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void proximity_ind_r9_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::proximity_ind_r9:
      j.write_fieldname("proximityIndication-r9");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "proximity_ind_r9_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE proximity_ind_r9_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::proximity_ind_r9:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "proximity_ind_r9_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE proximity_ind_r9_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::proximity_ind_r9:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "proximity_ind_r9_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RNReconfigurationComplete-r10 ::= SEQUENCE
SRSASN_CODE rn_recfg_complete_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_recfg_complete_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rn_recfg_complete_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rn_recfg_complete_r10_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rn_recfg_complete_r10_s::crit_exts_c_::c1_c_& rn_recfg_complete_r10_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void rn_recfg_complete_r10_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rn_recfg_complete_r10_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "rn_recfg_complete_r10_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rn_recfg_complete_r10_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rn_recfg_complete_r10_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_recfg_complete_r10_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "rn_recfg_complete_r10_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rn_recfg_complete_r10_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
rn_recfg_complete_r10_ies_s& rn_recfg_complete_r10_s::crit_exts_c_::c1_c_::set_rn_recfg_complete_r10()
{
  set(types::rn_recfg_complete_r10);
  return c;
}
void rn_recfg_complete_r10_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void rn_recfg_complete_r10_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void rn_recfg_complete_r10_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void rn_recfg_complete_r10_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rn_recfg_complete_r10:
      j.write_fieldname("rnReconfigurationComplete-r10");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rn_recfg_complete_r10_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rn_recfg_complete_r10_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rn_recfg_complete_r10:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rn_recfg_complete_r10_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rn_recfg_complete_r10_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rn_recfg_complete_r10:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rn_recfg_complete_r10_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionReconfigurationComplete ::= SEQUENCE
SRSASN_CODE rrc_conn_recfg_complete_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_complete_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_recfg_complete_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_recfg_complete_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_recfg_complete_r8_ies_s& rrc_conn_recfg_complete_s::crit_exts_c_::set_rrc_conn_recfg_complete_r8()
{
  set(types::rrc_conn_recfg_complete_r8);
  return c;
}
void rrc_conn_recfg_complete_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_conn_recfg_complete_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_recfg_complete_r8:
      j.write_fieldname("rrcConnectionReconfigurationComplete-r8");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_complete_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_recfg_complete_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_recfg_complete_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_complete_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_recfg_complete_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_recfg_complete_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_recfg_complete_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionReestablishmentComplete ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_complete_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_complete_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_complete_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_reest_complete_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_reest_complete_r8_ies_s& rrc_conn_reest_complete_s::crit_exts_c_::set_rrc_conn_reest_complete_r8()
{
  set(types::rrc_conn_reest_complete_r8);
  return c;
}
void rrc_conn_reest_complete_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_conn_reest_complete_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_reest_complete_r8:
      j.write_fieldname("rrcConnectionReestablishmentComplete-r8");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_complete_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_reest_complete_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_reest_complete_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_complete_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_complete_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_reest_complete_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_complete_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionResumeComplete-r13 ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_complete_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_complete_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_complete_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_resume_complete_r13_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_resume_complete_r13_ies_s& rrc_conn_resume_complete_r13_s::crit_exts_c_::set_rrc_conn_resume_complete_r13()
{
  set(types::rrc_conn_resume_complete_r13);
  return c;
}
void rrc_conn_resume_complete_r13_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_conn_resume_complete_r13_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_resume_complete_r13:
      j.write_fieldname("rrcConnectionResumeComplete-r13");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_complete_r13_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_resume_complete_r13_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_resume_complete_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_complete_r13_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_complete_r13_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_resume_complete_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_complete_r13_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionSetupComplete ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_complete_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_complete_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_setup_complete_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_setup_complete_s::crit_exts_c_::c1_c_& rrc_conn_setup_complete_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void rrc_conn_setup_complete_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_conn_setup_complete_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_setup_complete_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_setup_complete_r8_ies_s& rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::set_rrc_conn_setup_complete_r8()
{
  set(types::rrc_conn_setup_complete_r8);
  return c;
}
void rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_setup_complete_r8:
      j.write_fieldname("rrcConnectionSetupComplete-r8");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_setup_complete_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_setup_complete_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_complete_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SCGFailureInformation-r12 ::= SEQUENCE
SRSASN_CODE scg_fail_info_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void scg_fail_info_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void scg_fail_info_r12_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
scg_fail_info_r12_s::crit_exts_c_::c1_c_& scg_fail_info_r12_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void scg_fail_info_r12_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void scg_fail_info_r12_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "scg_fail_info_r12_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE scg_fail_info_r12_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_r12_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_r12_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "scg_fail_info_r12_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void scg_fail_info_r12_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
scg_fail_info_r12_ies_s& scg_fail_info_r12_s::crit_exts_c_::c1_c_::set_scg_fail_info_r12()
{
  set(types::scg_fail_info_r12);
  return c;
}
void scg_fail_info_r12_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void scg_fail_info_r12_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void scg_fail_info_r12_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void scg_fail_info_r12_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scg_fail_info_r12:
      j.write_fieldname("scgFailureInformation-r12");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_r12_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE scg_fail_info_r12_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scg_fail_info_r12:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_r12_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_r12_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scg_fail_info_r12:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_r12_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SCGFailureInformationNR-r15 ::= SEQUENCE
SRSASN_CODE scg_fail_info_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void scg_fail_info_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void scg_fail_info_nr_r15_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_& scg_fail_info_nr_r15_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void scg_fail_info_nr_r15_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void scg_fail_info_nr_r15_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "scg_fail_info_nr_r15_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE scg_fail_info_nr_r15_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_nr_r15_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_nr_r15_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "scg_fail_info_nr_r15_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
scg_fail_info_nr_r15_ies_s& scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_::set_scg_fail_info_nr_r15()
{
  set(types::scg_fail_info_nr_r15);
  return c;
}
void scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scg_fail_info_nr_r15:
      j.write_fieldname("scgFailureInformationNR-r15");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scg_fail_info_nr_r15:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scg_fail_info_nr_r15:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SidelinkUEInformation-r12 ::= SEQUENCE
SRSASN_CODE sidelink_ue_info_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sidelink_ue_info_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void sidelink_ue_info_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void sidelink_ue_info_r12_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
sidelink_ue_info_r12_s::crit_exts_c_::c1_c_& sidelink_ue_info_r12_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void sidelink_ue_info_r12_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void sidelink_ue_info_r12_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "sidelink_ue_info_r12_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE sidelink_ue_info_r12_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sidelink_ue_info_r12_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sidelink_ue_info_r12_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "sidelink_ue_info_r12_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sidelink_ue_info_r12_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
sidelink_ue_info_r12_ies_s& sidelink_ue_info_r12_s::crit_exts_c_::c1_c_::set_sidelink_ue_info_r12()
{
  set(types::sidelink_ue_info_r12);
  return c;
}
void sidelink_ue_info_r12_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void sidelink_ue_info_r12_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void sidelink_ue_info_r12_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void sidelink_ue_info_r12_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sidelink_ue_info_r12:
      j.write_fieldname("sidelinkUEInformation-r12");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "sidelink_ue_info_r12_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE sidelink_ue_info_r12_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sidelink_ue_info_r12:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "sidelink_ue_info_r12_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sidelink_ue_info_r12_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sidelink_ue_info_r12:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "sidelink_ue_info_r12_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEAssistanceInformation-r11 ::= SEQUENCE
SRSASN_CODE ueassist_info_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueassist_info_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ueassist_info_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ueassist_info_r11_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ueassist_info_r11_s::crit_exts_c_::c1_c_& ueassist_info_r11_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ueassist_info_r11_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ueassist_info_r11_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ueassist_info_r11_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ueassist_info_r11_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ueassist_info_r11_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ueassist_info_r11_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ueassist_info_r11_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ueassist_info_r11_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ueassist_info_r11_ies_s& ueassist_info_r11_s::crit_exts_c_::c1_c_::set_ue_assist_info_r11()
{
  set(types::ue_assist_info_r11);
  return c;
}
void ueassist_info_r11_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ueassist_info_r11_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ueassist_info_r11_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ueassist_info_r11_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_assist_info_r11:
      j.write_fieldname("ueAssistanceInformation-r11");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ueassist_info_r11_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ueassist_info_r11_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_assist_info_r11:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ueassist_info_r11_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ueassist_info_r11_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_assist_info_r11:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ueassist_info_r11_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEInformationResponse-r9 ::= SEQUENCE
SRSASN_CODE ue_info_resp_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_resp_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_info_resp_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_info_resp_r9_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_info_resp_r9_s::crit_exts_c_::c1_c_& ue_info_resp_r9_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ue_info_resp_r9_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_info_resp_r9_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ue_info_resp_r9_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_info_resp_r9_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_info_resp_r9_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_resp_r9_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ue_info_resp_r9_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_info_resp_r9_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ue_info_resp_r9_ies_s& ue_info_resp_r9_s::crit_exts_c_::c1_c_::set_ue_info_resp_r9()
{
  set(types::ue_info_resp_r9);
  return c;
}
void ue_info_resp_r9_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ue_info_resp_r9_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ue_info_resp_r9_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ue_info_resp_r9_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_info_resp_r9:
      j.write_fieldname("ueInformationResponse-r9");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_info_resp_r9_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_info_resp_r9_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_info_resp_r9:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_info_resp_r9_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_resp_r9_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_info_resp_r9:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_info_resp_r9_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ULHandoverPreparationTransfer ::= SEQUENCE
SRSASN_CODE ul_ho_prep_transfer_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ho_prep_transfer_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_ho_prep_transfer_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ul_ho_prep_transfer_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ul_ho_prep_transfer_s::crit_exts_c_::c1_c_& ul_ho_prep_transfer_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ul_ho_prep_transfer_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ul_ho_prep_transfer_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ul_ho_prep_transfer_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_ho_prep_transfer_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ho_prep_transfer_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ho_prep_transfer_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ul_ho_prep_transfer_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_ho_prep_transfer_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ul_ho_prep_transfer_r8_ies_s& ul_ho_prep_transfer_s::crit_exts_c_::c1_c_::set_ul_ho_prep_transfer_r8()
{
  set(types::ul_ho_prep_transfer_r8);
  return c;
}
void ul_ho_prep_transfer_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ul_ho_prep_transfer_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ul_ho_prep_transfer_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ul_ho_prep_transfer_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ul_ho_prep_transfer_r8:
      j.write_fieldname("ulHandoverPreparationTransfer-r8");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ho_prep_transfer_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_ho_prep_transfer_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ul_ho_prep_transfer_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ho_prep_transfer_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ho_prep_transfer_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ul_ho_prep_transfer_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ho_prep_transfer_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ULInformationTransfer ::= SEQUENCE
SRSASN_CODE ul_info_transfer_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_info_transfer_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ul_info_transfer_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ul_info_transfer_s::crit_exts_c_::c1_c_& ul_info_transfer_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ul_info_transfer_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ul_info_transfer_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ul_info_transfer_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_info_transfer_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ul_info_transfer_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_info_transfer_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ul_info_transfer_r8_ies_s& ul_info_transfer_s::crit_exts_c_::c1_c_::set_ul_info_transfer_r8()
{
  set(types::ul_info_transfer_r8);
  return c;
}
void ul_info_transfer_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ul_info_transfer_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ul_info_transfer_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ul_info_transfer_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ul_info_transfer_r8:
      j.write_fieldname("ulInformationTransfer-r8");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_info_transfer_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ul_info_transfer_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ul_info_transfer_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ULInformationTransferMRDC-r15 ::= SEQUENCE
SRSASN_CODE ul_info_transfer_mrdc_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_mrdc_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_info_transfer_mrdc_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ul_info_transfer_mrdc_r15_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_& ul_info_transfer_mrdc_r15_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ul_info_transfer_mrdc_r15_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ul_info_transfer_mrdc_r15_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_r15_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_info_transfer_mrdc_r15_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_r15_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_mrdc_r15_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_r15_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ul_info_transfer_mrdc_r15_ies_s& ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_::set_ul_info_transfer_mrdc_r15()
{
  set(types::ul_info_transfer_mrdc_r15);
  return c;
}
void ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ul_info_transfer_mrdc_r15:
      j.write_fieldname("ulInformationTransferMRDC-r15");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ul_info_transfer_mrdc_r15:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ul_info_transfer_mrdc_r15:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// WLANConnectionStatusReport-r13 ::= SEQUENCE
SRSASN_CODE wlan_conn_status_report_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_conn_status_report_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void wlan_conn_status_report_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void wlan_conn_status_report_r13_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_& wlan_conn_status_report_r13_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void wlan_conn_status_report_r13_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void wlan_conn_status_report_r13_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "wlan_conn_status_report_r13_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE wlan_conn_status_report_r13_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "wlan_conn_status_report_r13_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_conn_status_report_r13_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "wlan_conn_status_report_r13_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
wlan_conn_status_report_r13_ies_s& wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_::set_wlan_conn_status_report_r13()
{
  set(types::wlan_conn_status_report_r13);
  return c;
}
void wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::wlan_conn_status_report_r13:
      j.write_fieldname("wlanConnectionStatusReport-r13");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::wlan_conn_status_report_r13:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::wlan_conn_status_report_r13:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UL-DCCH-MessageType ::= CHOICE
void ul_dcch_msg_type_c::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    case types::msg_class_ext:
      c.destroy<msg_class_ext_c_>();
      break;
    default:
      break;
  }
}
void ul_dcch_msg_type_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::msg_class_ext:
      c.init<msg_class_ext_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
  }
}
ul_dcch_msg_type_c::ul_dcch_msg_type_c(const ul_dcch_msg_type_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      c.init(other.c.get<msg_class_ext_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
  }
}
ul_dcch_msg_type_c& ul_dcch_msg_type_c::operator=(const ul_dcch_msg_type_c& other)
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
      c.set(other.c.get<msg_class_ext_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
  }

  return *this;
}
ul_dcch_msg_type_c::c1_c_& ul_dcch_msg_type_c::set_c1()
{
  set(types::c1);
  return c.get<c1_c_>();
}
ul_dcch_msg_type_c::msg_class_ext_c_& ul_dcch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
  return c.get<msg_class_ext_c_>();
}
void ul_dcch_msg_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::msg_class_ext:
      j.write_fieldname("messageClassExtension");
      c.get<msg_class_ext_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_dcch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::msg_class_ext:
      HANDLE_CODE(c.get<msg_class_ext_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::msg_class_ext:
      HANDLE_CODE(c.get<msg_class_ext_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_dcch_msg_type_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::csfb_params_request_cdma2000:
      c.destroy<csfb_params_request_cdma2000_s>();
      break;
    case types::meas_report:
      c.destroy<meas_report_s>();
      break;
    case types::rrc_conn_recfg_complete:
      c.destroy<rrc_conn_recfg_complete_s>();
      break;
    case types::rrc_conn_reest_complete:
      c.destroy<rrc_conn_reest_complete_s>();
      break;
    case types::rrc_conn_setup_complete:
      c.destroy<rrc_conn_setup_complete_s>();
      break;
    case types::security_mode_complete:
      c.destroy<security_mode_complete_s>();
      break;
    case types::security_mode_fail:
      c.destroy<security_mode_fail_s>();
      break;
    case types::ue_cap_info:
      c.destroy<ue_cap_info_s>();
      break;
    case types::ul_ho_prep_transfer:
      c.destroy<ul_ho_prep_transfer_s>();
      break;
    case types::ul_info_transfer:
      c.destroy<ul_info_transfer_s>();
      break;
    case types::counter_check_resp:
      c.destroy<counter_check_resp_s>();
      break;
    case types::ue_info_resp_r9:
      c.destroy<ue_info_resp_r9_s>();
      break;
    case types::proximity_ind_r9:
      c.destroy<proximity_ind_r9_s>();
      break;
    case types::rn_recfg_complete_r10:
      c.destroy<rn_recfg_complete_r10_s>();
      break;
    case types::mbms_count_resp_r10:
      c.destroy<mbms_count_resp_r10_s>();
      break;
    case types::inter_freq_rstd_meas_ind_r10:
      c.destroy<inter_freq_rstd_meas_ind_r10_s>();
      break;
    default:
      break;
  }
}
void ul_dcch_msg_type_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::csfb_params_request_cdma2000:
      c.init<csfb_params_request_cdma2000_s>();
      break;
    case types::meas_report:
      c.init<meas_report_s>();
      break;
    case types::rrc_conn_recfg_complete:
      c.init<rrc_conn_recfg_complete_s>();
      break;
    case types::rrc_conn_reest_complete:
      c.init<rrc_conn_reest_complete_s>();
      break;
    case types::rrc_conn_setup_complete:
      c.init<rrc_conn_setup_complete_s>();
      break;
    case types::security_mode_complete:
      c.init<security_mode_complete_s>();
      break;
    case types::security_mode_fail:
      c.init<security_mode_fail_s>();
      break;
    case types::ue_cap_info:
      c.init<ue_cap_info_s>();
      break;
    case types::ul_ho_prep_transfer:
      c.init<ul_ho_prep_transfer_s>();
      break;
    case types::ul_info_transfer:
      c.init<ul_info_transfer_s>();
      break;
    case types::counter_check_resp:
      c.init<counter_check_resp_s>();
      break;
    case types::ue_info_resp_r9:
      c.init<ue_info_resp_r9_s>();
      break;
    case types::proximity_ind_r9:
      c.init<proximity_ind_r9_s>();
      break;
    case types::rn_recfg_complete_r10:
      c.init<rn_recfg_complete_r10_s>();
      break;
    case types::mbms_count_resp_r10:
      c.init<mbms_count_resp_r10_s>();
      break;
    case types::inter_freq_rstd_meas_ind_r10:
      c.init<inter_freq_rstd_meas_ind_r10_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
  }
}
ul_dcch_msg_type_c::c1_c_::c1_c_(const ul_dcch_msg_type_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::csfb_params_request_cdma2000:
      c.init(other.c.get<csfb_params_request_cdma2000_s>());
      break;
    case types::meas_report:
      c.init(other.c.get<meas_report_s>());
      break;
    case types::rrc_conn_recfg_complete:
      c.init(other.c.get<rrc_conn_recfg_complete_s>());
      break;
    case types::rrc_conn_reest_complete:
      c.init(other.c.get<rrc_conn_reest_complete_s>());
      break;
    case types::rrc_conn_setup_complete:
      c.init(other.c.get<rrc_conn_setup_complete_s>());
      break;
    case types::security_mode_complete:
      c.init(other.c.get<security_mode_complete_s>());
      break;
    case types::security_mode_fail:
      c.init(other.c.get<security_mode_fail_s>());
      break;
    case types::ue_cap_info:
      c.init(other.c.get<ue_cap_info_s>());
      break;
    case types::ul_ho_prep_transfer:
      c.init(other.c.get<ul_ho_prep_transfer_s>());
      break;
    case types::ul_info_transfer:
      c.init(other.c.get<ul_info_transfer_s>());
      break;
    case types::counter_check_resp:
      c.init(other.c.get<counter_check_resp_s>());
      break;
    case types::ue_info_resp_r9:
      c.init(other.c.get<ue_info_resp_r9_s>());
      break;
    case types::proximity_ind_r9:
      c.init(other.c.get<proximity_ind_r9_s>());
      break;
    case types::rn_recfg_complete_r10:
      c.init(other.c.get<rn_recfg_complete_r10_s>());
      break;
    case types::mbms_count_resp_r10:
      c.init(other.c.get<mbms_count_resp_r10_s>());
      break;
    case types::inter_freq_rstd_meas_ind_r10:
      c.init(other.c.get<inter_freq_rstd_meas_ind_r10_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
  }
}
ul_dcch_msg_type_c::c1_c_& ul_dcch_msg_type_c::c1_c_::operator=(const ul_dcch_msg_type_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::csfb_params_request_cdma2000:
      c.set(other.c.get<csfb_params_request_cdma2000_s>());
      break;
    case types::meas_report:
      c.set(other.c.get<meas_report_s>());
      break;
    case types::rrc_conn_recfg_complete:
      c.set(other.c.get<rrc_conn_recfg_complete_s>());
      break;
    case types::rrc_conn_reest_complete:
      c.set(other.c.get<rrc_conn_reest_complete_s>());
      break;
    case types::rrc_conn_setup_complete:
      c.set(other.c.get<rrc_conn_setup_complete_s>());
      break;
    case types::security_mode_complete:
      c.set(other.c.get<security_mode_complete_s>());
      break;
    case types::security_mode_fail:
      c.set(other.c.get<security_mode_fail_s>());
      break;
    case types::ue_cap_info:
      c.set(other.c.get<ue_cap_info_s>());
      break;
    case types::ul_ho_prep_transfer:
      c.set(other.c.get<ul_ho_prep_transfer_s>());
      break;
    case types::ul_info_transfer:
      c.set(other.c.get<ul_info_transfer_s>());
      break;
    case types::counter_check_resp:
      c.set(other.c.get<counter_check_resp_s>());
      break;
    case types::ue_info_resp_r9:
      c.set(other.c.get<ue_info_resp_r9_s>());
      break;
    case types::proximity_ind_r9:
      c.set(other.c.get<proximity_ind_r9_s>());
      break;
    case types::rn_recfg_complete_r10:
      c.set(other.c.get<rn_recfg_complete_r10_s>());
      break;
    case types::mbms_count_resp_r10:
      c.set(other.c.get<mbms_count_resp_r10_s>());
      break;
    case types::inter_freq_rstd_meas_ind_r10:
      c.set(other.c.get<inter_freq_rstd_meas_ind_r10_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
  }

  return *this;
}
csfb_params_request_cdma2000_s& ul_dcch_msg_type_c::c1_c_::set_csfb_params_request_cdma2000()
{
  set(types::csfb_params_request_cdma2000);
  return c.get<csfb_params_request_cdma2000_s>();
}
meas_report_s& ul_dcch_msg_type_c::c1_c_::set_meas_report()
{
  set(types::meas_report);
  return c.get<meas_report_s>();
}
rrc_conn_recfg_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_conn_recfg_complete()
{
  set(types::rrc_conn_recfg_complete);
  return c.get<rrc_conn_recfg_complete_s>();
}
rrc_conn_reest_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_conn_reest_complete()
{
  set(types::rrc_conn_reest_complete);
  return c.get<rrc_conn_reest_complete_s>();
}
rrc_conn_setup_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_conn_setup_complete()
{
  set(types::rrc_conn_setup_complete);
  return c.get<rrc_conn_setup_complete_s>();
}
security_mode_complete_s& ul_dcch_msg_type_c::c1_c_::set_security_mode_complete()
{
  set(types::security_mode_complete);
  return c.get<security_mode_complete_s>();
}
security_mode_fail_s& ul_dcch_msg_type_c::c1_c_::set_security_mode_fail()
{
  set(types::security_mode_fail);
  return c.get<security_mode_fail_s>();
}
ue_cap_info_s& ul_dcch_msg_type_c::c1_c_::set_ue_cap_info()
{
  set(types::ue_cap_info);
  return c.get<ue_cap_info_s>();
}
ul_ho_prep_transfer_s& ul_dcch_msg_type_c::c1_c_::set_ul_ho_prep_transfer()
{
  set(types::ul_ho_prep_transfer);
  return c.get<ul_ho_prep_transfer_s>();
}
ul_info_transfer_s& ul_dcch_msg_type_c::c1_c_::set_ul_info_transfer()
{
  set(types::ul_info_transfer);
  return c.get<ul_info_transfer_s>();
}
counter_check_resp_s& ul_dcch_msg_type_c::c1_c_::set_counter_check_resp()
{
  set(types::counter_check_resp);
  return c.get<counter_check_resp_s>();
}
ue_info_resp_r9_s& ul_dcch_msg_type_c::c1_c_::set_ue_info_resp_r9()
{
  set(types::ue_info_resp_r9);
  return c.get<ue_info_resp_r9_s>();
}
proximity_ind_r9_s& ul_dcch_msg_type_c::c1_c_::set_proximity_ind_r9()
{
  set(types::proximity_ind_r9);
  return c.get<proximity_ind_r9_s>();
}
rn_recfg_complete_r10_s& ul_dcch_msg_type_c::c1_c_::set_rn_recfg_complete_r10()
{
  set(types::rn_recfg_complete_r10);
  return c.get<rn_recfg_complete_r10_s>();
}
mbms_count_resp_r10_s& ul_dcch_msg_type_c::c1_c_::set_mbms_count_resp_r10()
{
  set(types::mbms_count_resp_r10);
  return c.get<mbms_count_resp_r10_s>();
}
inter_freq_rstd_meas_ind_r10_s& ul_dcch_msg_type_c::c1_c_::set_inter_freq_rstd_meas_ind_r10()
{
  set(types::inter_freq_rstd_meas_ind_r10);
  return c.get<inter_freq_rstd_meas_ind_r10_s>();
}
void ul_dcch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::csfb_params_request_cdma2000:
      j.write_fieldname("csfbParametersRequestCDMA2000");
      c.get<csfb_params_request_cdma2000_s>().to_json(j);
      break;
    case types::meas_report:
      j.write_fieldname("measurementReport");
      c.get<meas_report_s>().to_json(j);
      break;
    case types::rrc_conn_recfg_complete:
      j.write_fieldname("rrcConnectionReconfigurationComplete");
      c.get<rrc_conn_recfg_complete_s>().to_json(j);
      break;
    case types::rrc_conn_reest_complete:
      j.write_fieldname("rrcConnectionReestablishmentComplete");
      c.get<rrc_conn_reest_complete_s>().to_json(j);
      break;
    case types::rrc_conn_setup_complete:
      j.write_fieldname("rrcConnectionSetupComplete");
      c.get<rrc_conn_setup_complete_s>().to_json(j);
      break;
    case types::security_mode_complete:
      j.write_fieldname("securityModeComplete");
      c.get<security_mode_complete_s>().to_json(j);
      break;
    case types::security_mode_fail:
      j.write_fieldname("securityModeFailure");
      c.get<security_mode_fail_s>().to_json(j);
      break;
    case types::ue_cap_info:
      j.write_fieldname("ueCapabilityInformation");
      c.get<ue_cap_info_s>().to_json(j);
      break;
    case types::ul_ho_prep_transfer:
      j.write_fieldname("ulHandoverPreparationTransfer");
      c.get<ul_ho_prep_transfer_s>().to_json(j);
      break;
    case types::ul_info_transfer:
      j.write_fieldname("ulInformationTransfer");
      c.get<ul_info_transfer_s>().to_json(j);
      break;
    case types::counter_check_resp:
      j.write_fieldname("counterCheckResponse");
      c.get<counter_check_resp_s>().to_json(j);
      break;
    case types::ue_info_resp_r9:
      j.write_fieldname("ueInformationResponse-r9");
      c.get<ue_info_resp_r9_s>().to_json(j);
      break;
    case types::proximity_ind_r9:
      j.write_fieldname("proximityIndication-r9");
      c.get<proximity_ind_r9_s>().to_json(j);
      break;
    case types::rn_recfg_complete_r10:
      j.write_fieldname("rnReconfigurationComplete-r10");
      c.get<rn_recfg_complete_r10_s>().to_json(j);
      break;
    case types::mbms_count_resp_r10:
      j.write_fieldname("mbmsCountingResponse-r10");
      c.get<mbms_count_resp_r10_s>().to_json(j);
      break;
    case types::inter_freq_rstd_meas_ind_r10:
      j.write_fieldname("interFreqRSTDMeasurementIndication-r10");
      c.get<inter_freq_rstd_meas_ind_r10_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_dcch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::csfb_params_request_cdma2000:
      HANDLE_CODE(c.get<csfb_params_request_cdma2000_s>().pack(bref));
      break;
    case types::meas_report:
      HANDLE_CODE(c.get<meas_report_s>().pack(bref));
      break;
    case types::rrc_conn_recfg_complete:
      HANDLE_CODE(c.get<rrc_conn_recfg_complete_s>().pack(bref));
      break;
    case types::rrc_conn_reest_complete:
      HANDLE_CODE(c.get<rrc_conn_reest_complete_s>().pack(bref));
      break;
    case types::rrc_conn_setup_complete:
      HANDLE_CODE(c.get<rrc_conn_setup_complete_s>().pack(bref));
      break;
    case types::security_mode_complete:
      HANDLE_CODE(c.get<security_mode_complete_s>().pack(bref));
      break;
    case types::security_mode_fail:
      HANDLE_CODE(c.get<security_mode_fail_s>().pack(bref));
      break;
    case types::ue_cap_info:
      HANDLE_CODE(c.get<ue_cap_info_s>().pack(bref));
      break;
    case types::ul_ho_prep_transfer:
      HANDLE_CODE(c.get<ul_ho_prep_transfer_s>().pack(bref));
      break;
    case types::ul_info_transfer:
      HANDLE_CODE(c.get<ul_info_transfer_s>().pack(bref));
      break;
    case types::counter_check_resp:
      HANDLE_CODE(c.get<counter_check_resp_s>().pack(bref));
      break;
    case types::ue_info_resp_r9:
      HANDLE_CODE(c.get<ue_info_resp_r9_s>().pack(bref));
      break;
    case types::proximity_ind_r9:
      HANDLE_CODE(c.get<proximity_ind_r9_s>().pack(bref));
      break;
    case types::rn_recfg_complete_r10:
      HANDLE_CODE(c.get<rn_recfg_complete_r10_s>().pack(bref));
      break;
    case types::mbms_count_resp_r10:
      HANDLE_CODE(c.get<mbms_count_resp_r10_s>().pack(bref));
      break;
    case types::inter_freq_rstd_meas_ind_r10:
      HANDLE_CODE(c.get<inter_freq_rstd_meas_ind_r10_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::csfb_params_request_cdma2000:
      HANDLE_CODE(c.get<csfb_params_request_cdma2000_s>().unpack(bref));
      break;
    case types::meas_report:
      HANDLE_CODE(c.get<meas_report_s>().unpack(bref));
      break;
    case types::rrc_conn_recfg_complete:
      HANDLE_CODE(c.get<rrc_conn_recfg_complete_s>().unpack(bref));
      break;
    case types::rrc_conn_reest_complete:
      HANDLE_CODE(c.get<rrc_conn_reest_complete_s>().unpack(bref));
      break;
    case types::rrc_conn_setup_complete:
      HANDLE_CODE(c.get<rrc_conn_setup_complete_s>().unpack(bref));
      break;
    case types::security_mode_complete:
      HANDLE_CODE(c.get<security_mode_complete_s>().unpack(bref));
      break;
    case types::security_mode_fail:
      HANDLE_CODE(c.get<security_mode_fail_s>().unpack(bref));
      break;
    case types::ue_cap_info:
      HANDLE_CODE(c.get<ue_cap_info_s>().unpack(bref));
      break;
    case types::ul_ho_prep_transfer:
      HANDLE_CODE(c.get<ul_ho_prep_transfer_s>().unpack(bref));
      break;
    case types::ul_info_transfer:
      HANDLE_CODE(c.get<ul_info_transfer_s>().unpack(bref));
      break;
    case types::counter_check_resp:
      HANDLE_CODE(c.get<counter_check_resp_s>().unpack(bref));
      break;
    case types::ue_info_resp_r9:
      HANDLE_CODE(c.get<ue_info_resp_r9_s>().unpack(bref));
      break;
    case types::proximity_ind_r9:
      HANDLE_CODE(c.get<proximity_ind_r9_s>().unpack(bref));
      break;
    case types::rn_recfg_complete_r10:
      HANDLE_CODE(c.get<rn_recfg_complete_r10_s>().unpack(bref));
      break;
    case types::mbms_count_resp_r10:
      HANDLE_CODE(c.get<mbms_count_resp_r10_s>().unpack(bref));
      break;
    case types::inter_freq_rstd_meas_ind_r10:
      HANDLE_CODE(c.get<inter_freq_rstd_meas_ind_r10_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_dcch_msg_type_c::msg_class_ext_c_::set(types::options e)
{
  type_ = e;
}
ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_& ul_dcch_msg_type_c::msg_class_ext_c_::set_c2()
{
  set(types::c2);
  return c;
}
void ul_dcch_msg_type_c::msg_class_ext_c_::set_msg_class_ext_future_r11()
{
  set(types::msg_class_ext_future_r11);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c2:
      j.write_fieldname("c2");
      c.to_json(j);
      break;
    case types::msg_class_ext_future_r11:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_dcch_msg_type_c::msg_class_ext_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c2:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext_future_r11:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_type_c::msg_class_ext_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c2:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext_future_r11:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::destroy_()
{
  switch (type_) {
    case types::ue_assist_info_r11:
      c.destroy<ueassist_info_r11_s>();
      break;
    case types::in_dev_coex_ind_r11:
      c.destroy<in_dev_coex_ind_r11_s>();
      break;
    case types::mbms_interest_ind_r11:
      c.destroy<mbms_interest_ind_r11_s>();
      break;
    case types::scg_fail_info_r12:
      c.destroy<scg_fail_info_r12_s>();
      break;
    case types::sidelink_ue_info_r12:
      c.destroy<sidelink_ue_info_r12_s>();
      break;
    case types::wlan_conn_status_report_r13:
      c.destroy<wlan_conn_status_report_r13_s>();
      break;
    case types::rrc_conn_resume_complete_r13:
      c.destroy<rrc_conn_resume_complete_r13_s>();
      break;
    case types::ul_info_transfer_mrdc_r15:
      c.destroy<ul_info_transfer_mrdc_r15_s>();
      break;
    case types::scg_fail_info_nr_r15:
      c.destroy<scg_fail_info_nr_r15_s>();
      break;
    case types::meas_report_app_layer_r15:
      c.destroy<meas_report_app_layer_r15_s>();
      break;
    case types::fail_info_r15:
      c.destroy<fail_info_r15_s>();
      break;
    default:
      break;
  }
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ue_assist_info_r11:
      c.init<ueassist_info_r11_s>();
      break;
    case types::in_dev_coex_ind_r11:
      c.init<in_dev_coex_ind_r11_s>();
      break;
    case types::mbms_interest_ind_r11:
      c.init<mbms_interest_ind_r11_s>();
      break;
    case types::scg_fail_info_r12:
      c.init<scg_fail_info_r12_s>();
      break;
    case types::sidelink_ue_info_r12:
      c.init<sidelink_ue_info_r12_s>();
      break;
    case types::wlan_conn_status_report_r13:
      c.init<wlan_conn_status_report_r13_s>();
      break;
    case types::rrc_conn_resume_complete_r13:
      c.init<rrc_conn_resume_complete_r13_s>();
      break;
    case types::ul_info_transfer_mrdc_r15:
      c.init<ul_info_transfer_mrdc_r15_s>();
      break;
    case types::scg_fail_info_nr_r15:
      c.init<scg_fail_info_nr_r15_s>();
      break;
    case types::meas_report_app_layer_r15:
      c.init<meas_report_app_layer_r15_s>();
      break;
    case types::fail_info_r15:
      c.init<fail_info_r15_s>();
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
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_");
  }
}
ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::c2_c_(const ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ue_assist_info_r11:
      c.init(other.c.get<ueassist_info_r11_s>());
      break;
    case types::in_dev_coex_ind_r11:
      c.init(other.c.get<in_dev_coex_ind_r11_s>());
      break;
    case types::mbms_interest_ind_r11:
      c.init(other.c.get<mbms_interest_ind_r11_s>());
      break;
    case types::scg_fail_info_r12:
      c.init(other.c.get<scg_fail_info_r12_s>());
      break;
    case types::sidelink_ue_info_r12:
      c.init(other.c.get<sidelink_ue_info_r12_s>());
      break;
    case types::wlan_conn_status_report_r13:
      c.init(other.c.get<wlan_conn_status_report_r13_s>());
      break;
    case types::rrc_conn_resume_complete_r13:
      c.init(other.c.get<rrc_conn_resume_complete_r13_s>());
      break;
    case types::ul_info_transfer_mrdc_r15:
      c.init(other.c.get<ul_info_transfer_mrdc_r15_s>());
      break;
    case types::scg_fail_info_nr_r15:
      c.init(other.c.get<scg_fail_info_nr_r15_s>());
      break;
    case types::meas_report_app_layer_r15:
      c.init(other.c.get<meas_report_app_layer_r15_s>());
      break;
    case types::fail_info_r15:
      c.init(other.c.get<fail_info_r15_s>());
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
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_");
  }
}
ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_&
ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::operator=(const ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ue_assist_info_r11:
      c.set(other.c.get<ueassist_info_r11_s>());
      break;
    case types::in_dev_coex_ind_r11:
      c.set(other.c.get<in_dev_coex_ind_r11_s>());
      break;
    case types::mbms_interest_ind_r11:
      c.set(other.c.get<mbms_interest_ind_r11_s>());
      break;
    case types::scg_fail_info_r12:
      c.set(other.c.get<scg_fail_info_r12_s>());
      break;
    case types::sidelink_ue_info_r12:
      c.set(other.c.get<sidelink_ue_info_r12_s>());
      break;
    case types::wlan_conn_status_report_r13:
      c.set(other.c.get<wlan_conn_status_report_r13_s>());
      break;
    case types::rrc_conn_resume_complete_r13:
      c.set(other.c.get<rrc_conn_resume_complete_r13_s>());
      break;
    case types::ul_info_transfer_mrdc_r15:
      c.set(other.c.get<ul_info_transfer_mrdc_r15_s>());
      break;
    case types::scg_fail_info_nr_r15:
      c.set(other.c.get<scg_fail_info_nr_r15_s>());
      break;
    case types::meas_report_app_layer_r15:
      c.set(other.c.get<meas_report_app_layer_r15_s>());
      break;
    case types::fail_info_r15:
      c.set(other.c.get<fail_info_r15_s>());
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
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_");
  }

  return *this;
}
ueassist_info_r11_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_ue_assist_info_r11()
{
  set(types::ue_assist_info_r11);
  return c.get<ueassist_info_r11_s>();
}
in_dev_coex_ind_r11_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_in_dev_coex_ind_r11()
{
  set(types::in_dev_coex_ind_r11);
  return c.get<in_dev_coex_ind_r11_s>();
}
mbms_interest_ind_r11_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_mbms_interest_ind_r11()
{
  set(types::mbms_interest_ind_r11);
  return c.get<mbms_interest_ind_r11_s>();
}
scg_fail_info_r12_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_scg_fail_info_r12()
{
  set(types::scg_fail_info_r12);
  return c.get<scg_fail_info_r12_s>();
}
sidelink_ue_info_r12_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_sidelink_ue_info_r12()
{
  set(types::sidelink_ue_info_r12);
  return c.get<sidelink_ue_info_r12_s>();
}
wlan_conn_status_report_r13_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_wlan_conn_status_report_r13()
{
  set(types::wlan_conn_status_report_r13);
  return c.get<wlan_conn_status_report_r13_s>();
}
rrc_conn_resume_complete_r13_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_rrc_conn_resume_complete_r13()
{
  set(types::rrc_conn_resume_complete_r13);
  return c.get<rrc_conn_resume_complete_r13_s>();
}
ul_info_transfer_mrdc_r15_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_ul_info_transfer_mrdc_r15()
{
  set(types::ul_info_transfer_mrdc_r15);
  return c.get<ul_info_transfer_mrdc_r15_s>();
}
scg_fail_info_nr_r15_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_scg_fail_info_nr_r15()
{
  set(types::scg_fail_info_nr_r15);
  return c.get<scg_fail_info_nr_r15_s>();
}
meas_report_app_layer_r15_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_meas_report_app_layer_r15()
{
  set(types::meas_report_app_layer_r15);
  return c.get<meas_report_app_layer_r15_s>();
}
fail_info_r15_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_fail_info_r15()
{
  set(types::fail_info_r15);
  return c.get<fail_info_r15_s>();
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare5()
{
  set(types::spare5);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare4()
{
  set(types::spare4);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare3()
{
  set(types::spare3);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare2()
{
  set(types::spare2);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare1()
{
  set(types::spare1);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_assist_info_r11:
      j.write_fieldname("ueAssistanceInformation-r11");
      c.get<ueassist_info_r11_s>().to_json(j);
      break;
    case types::in_dev_coex_ind_r11:
      j.write_fieldname("inDeviceCoexIndication-r11");
      c.get<in_dev_coex_ind_r11_s>().to_json(j);
      break;
    case types::mbms_interest_ind_r11:
      j.write_fieldname("mbmsInterestIndication-r11");
      c.get<mbms_interest_ind_r11_s>().to_json(j);
      break;
    case types::scg_fail_info_r12:
      j.write_fieldname("scgFailureInformation-r12");
      c.get<scg_fail_info_r12_s>().to_json(j);
      break;
    case types::sidelink_ue_info_r12:
      j.write_fieldname("sidelinkUEInformation-r12");
      c.get<sidelink_ue_info_r12_s>().to_json(j);
      break;
    case types::wlan_conn_status_report_r13:
      j.write_fieldname("wlanConnectionStatusReport-r13");
      c.get<wlan_conn_status_report_r13_s>().to_json(j);
      break;
    case types::rrc_conn_resume_complete_r13:
      j.write_fieldname("rrcConnectionResumeComplete-r13");
      c.get<rrc_conn_resume_complete_r13_s>().to_json(j);
      break;
    case types::ul_info_transfer_mrdc_r15:
      j.write_fieldname("ulInformationTransferMRDC-r15");
      c.get<ul_info_transfer_mrdc_r15_s>().to_json(j);
      break;
    case types::scg_fail_info_nr_r15:
      j.write_fieldname("scgFailureInformationNR-r15");
      c.get<scg_fail_info_nr_r15_s>().to_json(j);
      break;
    case types::meas_report_app_layer_r15:
      j.write_fieldname("measReportAppLayer-r15");
      c.get<meas_report_app_layer_r15_s>().to_json(j);
      break;
    case types::fail_info_r15:
      j.write_fieldname("failureInformation-r15");
      c.get<fail_info_r15_s>().to_json(j);
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
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_assist_info_r11:
      HANDLE_CODE(c.get<ueassist_info_r11_s>().pack(bref));
      break;
    case types::in_dev_coex_ind_r11:
      HANDLE_CODE(c.get<in_dev_coex_ind_r11_s>().pack(bref));
      break;
    case types::mbms_interest_ind_r11:
      HANDLE_CODE(c.get<mbms_interest_ind_r11_s>().pack(bref));
      break;
    case types::scg_fail_info_r12:
      HANDLE_CODE(c.get<scg_fail_info_r12_s>().pack(bref));
      break;
    case types::sidelink_ue_info_r12:
      HANDLE_CODE(c.get<sidelink_ue_info_r12_s>().pack(bref));
      break;
    case types::wlan_conn_status_report_r13:
      HANDLE_CODE(c.get<wlan_conn_status_report_r13_s>().pack(bref));
      break;
    case types::rrc_conn_resume_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_complete_r13_s>().pack(bref));
      break;
    case types::ul_info_transfer_mrdc_r15:
      HANDLE_CODE(c.get<ul_info_transfer_mrdc_r15_s>().pack(bref));
      break;
    case types::scg_fail_info_nr_r15:
      HANDLE_CODE(c.get<scg_fail_info_nr_r15_s>().pack(bref));
      break;
    case types::meas_report_app_layer_r15:
      HANDLE_CODE(c.get<meas_report_app_layer_r15_s>().pack(bref));
      break;
    case types::fail_info_r15:
      HANDLE_CODE(c.get<fail_info_r15_s>().pack(bref));
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
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_assist_info_r11:
      HANDLE_CODE(c.get<ueassist_info_r11_s>().unpack(bref));
      break;
    case types::in_dev_coex_ind_r11:
      HANDLE_CODE(c.get<in_dev_coex_ind_r11_s>().unpack(bref));
      break;
    case types::mbms_interest_ind_r11:
      HANDLE_CODE(c.get<mbms_interest_ind_r11_s>().unpack(bref));
      break;
    case types::scg_fail_info_r12:
      HANDLE_CODE(c.get<scg_fail_info_r12_s>().unpack(bref));
      break;
    case types::sidelink_ue_info_r12:
      HANDLE_CODE(c.get<sidelink_ue_info_r12_s>().unpack(bref));
      break;
    case types::wlan_conn_status_report_r13:
      HANDLE_CODE(c.get<wlan_conn_status_report_r13_s>().unpack(bref));
      break;
    case types::rrc_conn_resume_complete_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_complete_r13_s>().unpack(bref));
      break;
    case types::ul_info_transfer_mrdc_r15:
      HANDLE_CODE(c.get<ul_info_transfer_mrdc_r15_s>().unpack(bref));
      break;
    case types::scg_fail_info_nr_r15:
      HANDLE_CODE(c.get<scg_fail_info_nr_r15_s>().unpack(bref));
      break;
    case types::meas_report_app_layer_r15:
      HANDLE_CODE(c.get<meas_report_app_layer_r15_s>().unpack(bref));
      break;
    case types::fail_info_r15:
      HANDLE_CODE(c.get<fail_info_r15_s>().unpack(bref));
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
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UL-DCCH-Message ::= SEQUENCE
SRSASN_CODE ul_dcch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ul_dcch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("UL-DCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// RLF-Report-v9e0 ::= SEQUENCE
SRSASN_CODE rlf_report_v9e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_list_eutra_v9e0, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_report_v9e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_list_eutra_v9e0, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void rlf_report_v9e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measResultListEUTRA-v9e0");
  for (const auto& e1 : meas_result_list_eutra_v9e0) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// UEInformationResponse-v9e0-IEs ::= SEQUENCE
SRSASN_CODE ue_info_resp_v9e0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rlf_report_v9e0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rlf_report_v9e0_present) {
    HANDLE_CODE(rlf_report_v9e0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_resp_v9e0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rlf_report_v9e0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rlf_report_v9e0_present) {
    HANDLE_CODE(rlf_report_v9e0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_info_resp_v9e0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rlf_report_v9e0_present) {
    j.write_fieldname("rlf-Report-v9e0");
    rlf_report_v9e0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}
