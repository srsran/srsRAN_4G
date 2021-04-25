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

#include "srsran/asn1/rrc/uecap.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// CA-BandwidthClass-r10 ::= ENUMERATED
const char* ca_bw_class_r10_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c", "d", "e", "f"};
  return convert_enum_idx(options, 6, value, "ca_bw_class_r10_e");
}

// UECapabilityEnquiry-v1560-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_v1560_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(requested_cap_common_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (requested_cap_common_r15_present) {
    HANDLE_CODE(requested_cap_common_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_v1560_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(requested_cap_common_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (requested_cap_common_r15_present) {
    HANDLE_CODE(requested_cap_common_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_v1560_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (requested_cap_common_r15_present) {
    j.write_str("requestedCapabilityCommon-r15", requested_cap_common_r15.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// BandIndication-r14 ::= SEQUENCE
SRSASN_CODE band_ind_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ca_bw_class_ul_r14_present, 1));

  HANDLE_CODE(pack_integer(bref, band_eutra_r14, (uint16_t)1u, (uint16_t)256u));
  HANDLE_CODE(ca_bw_class_dl_r14.pack(bref));
  if (ca_bw_class_ul_r14_present) {
    HANDLE_CODE(ca_bw_class_ul_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_ind_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ca_bw_class_ul_r14_present, 1));

  HANDLE_CODE(unpack_integer(band_eutra_r14, bref, (uint16_t)1u, (uint16_t)256u));
  HANDLE_CODE(ca_bw_class_dl_r14.unpack(bref));
  if (ca_bw_class_ul_r14_present) {
    HANDLE_CODE(ca_bw_class_ul_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_ind_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandEUTRA-r14", band_eutra_r14);
  j.write_str("ca-BandwidthClassDL-r14", ca_bw_class_dl_r14.to_string());
  if (ca_bw_class_ul_r14_present) {
    j.write_str("ca-BandwidthClassUL-r14", ca_bw_class_ul_r14.to_string());
  }
  j.end_obj();
}

// UECapabilityEnquiry-v1550-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_v1550_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(requested_cap_nr_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (requested_cap_nr_r15_present) {
    HANDLE_CODE(requested_cap_nr_r15.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_v1550_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(requested_cap_nr_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (requested_cap_nr_r15_present) {
    HANDLE_CODE(requested_cap_nr_r15.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_v1550_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (requested_cap_nr_r15_present) {
    j.write_str("requestedCapabilityNR-r15", requested_cap_nr_r15.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UECapabilityEnquiry-v1530-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(request_stti_spt_cap_r15_present, 1));
  HANDLE_CODE(bref.pack(eutra_nr_only_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(request_stti_spt_cap_r15_present, 1));
  HANDLE_CODE(bref.unpack(eutra_nr_only_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (request_stti_spt_cap_r15_present) {
    j.write_str("requestSTTI-SPT-Capability-r15", "true");
  }
  if (eutra_nr_only_r15_present) {
    j.write_str("eutra-nr-only-r15", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UECapabilityEnquiry-v1510-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_v1510_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(requested_freq_bands_nr_mrdc_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (requested_freq_bands_nr_mrdc_r15_present) {
    HANDLE_CODE(requested_freq_bands_nr_mrdc_r15.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_v1510_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(requested_freq_bands_nr_mrdc_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (requested_freq_bands_nr_mrdc_r15_present) {
    HANDLE_CODE(requested_freq_bands_nr_mrdc_r15.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_v1510_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (requested_freq_bands_nr_mrdc_r15_present) {
    j.write_str("requestedFreqBandsNR-MRDC-r15", requested_freq_bands_nr_mrdc_r15.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UECapabilityEnquiry-v1430-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(request_diff_fallback_comb_list_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (request_diff_fallback_comb_list_r14_present) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, request_diff_fallback_comb_list_r14, 1, 384, SeqOfPacker<Packer>(1, 64, Packer())));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(request_diff_fallback_comb_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (request_diff_fallback_comb_list_r14_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(request_diff_fallback_comb_list_r14, bref, 1, 384, SeqOfPacker<Packer>(1, 64, Packer())));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (request_diff_fallback_comb_list_r14_present) {
    j.start_array("requestDiffFallbackCombList-r14");
    for (const auto& e1 : request_diff_fallback_comb_list_r14) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
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

// UECapabilityEnquiry-v1310-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_v1310_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(request_reduced_format_r13_present, 1));
  HANDLE_CODE(bref.pack(request_skip_fallback_comb_r13_present, 1));
  HANDLE_CODE(bref.pack(requested_max_ccs_dl_r13_present, 1));
  HANDLE_CODE(bref.pack(requested_max_ccs_ul_r13_present, 1));
  HANDLE_CODE(bref.pack(request_reduced_int_non_cont_comb_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (requested_max_ccs_dl_r13_present) {
    HANDLE_CODE(pack_integer(bref, requested_max_ccs_dl_r13, (uint8_t)2u, (uint8_t)32u));
  }
  if (requested_max_ccs_ul_r13_present) {
    HANDLE_CODE(pack_integer(bref, requested_max_ccs_ul_r13, (uint8_t)2u, (uint8_t)32u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_v1310_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(request_reduced_format_r13_present, 1));
  HANDLE_CODE(bref.unpack(request_skip_fallback_comb_r13_present, 1));
  HANDLE_CODE(bref.unpack(requested_max_ccs_dl_r13_present, 1));
  HANDLE_CODE(bref.unpack(requested_max_ccs_ul_r13_present, 1));
  HANDLE_CODE(bref.unpack(request_reduced_int_non_cont_comb_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (requested_max_ccs_dl_r13_present) {
    HANDLE_CODE(unpack_integer(requested_max_ccs_dl_r13, bref, (uint8_t)2u, (uint8_t)32u));
  }
  if (requested_max_ccs_ul_r13_present) {
    HANDLE_CODE(unpack_integer(requested_max_ccs_ul_r13, bref, (uint8_t)2u, (uint8_t)32u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_v1310_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (request_reduced_format_r13_present) {
    j.write_str("requestReducedFormat-r13", "true");
  }
  if (request_skip_fallback_comb_r13_present) {
    j.write_str("requestSkipFallbackComb-r13", "true");
  }
  if (requested_max_ccs_dl_r13_present) {
    j.write_int("requestedMaxCCsDL-r13", requested_max_ccs_dl_r13);
  }
  if (requested_max_ccs_ul_r13_present) {
    j.write_int("requestedMaxCCsUL-r13", requested_max_ccs_ul_r13);
  }
  if (request_reduced_int_non_cont_comb_r13_present) {
    j.write_str("requestReducedIntNonContComb-r13", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RAT-Type ::= ENUMERATED
const char* rat_type_opts::to_string() const
{
  static const char* options[] = {
      "eutra", "utra", "geran-cs", "geran-ps", "cdma2000-1XRTT", "nr", "eutra-nr", "spare1"};
  return convert_enum_idx(options, 8, value, "rat_type_e");
}
uint16_t rat_type_opts::to_number() const
{
  if (value == cdma2000_minus1_xrtt) {
    return 2000;
  }
  invalid_enum_number(value, "rat_type_e");
  return 0;
}

// UECapabilityEnquiry-v1180-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_v1180_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(requested_freq_bands_r11_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (requested_freq_bands_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, requested_freq_bands_r11, 1, 16, integer_packer<uint16_t>(1, 256)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_v1180_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(requested_freq_bands_r11_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (requested_freq_bands_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(requested_freq_bands_r11, bref, 1, 16, integer_packer<uint16_t>(1, 256)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_v1180_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (requested_freq_bands_r11_present) {
    j.start_array("requestedFrequencyBands-r11");
    for (const auto& e1 : requested_freq_bands_r11) {
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

// UECapabilityEnquiry-v8a0-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_v8a0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE ue_cap_enquiry_v8a0_ies_s::unpack(cbit_ref& bref)
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
void ue_cap_enquiry_v8a0_ies_s::to_json(json_writer& j) const
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

// UECapabilityEnquiry-r8-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, ue_cap_request, 1, 8));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(ue_cap_request, bref, 1, 8));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("ue-CapabilityRequest");
  for (const auto& e1 : ue_cap_request) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UECapabilityEnquiry ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_cap_enquiry_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_cap_enquiry_s::crit_exts_c_::c1_c_& ue_cap_enquiry_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ue_cap_enquiry_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_cap_enquiry_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ue_cap_enquiry_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_cap_enquiry_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ue_cap_enquiry_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_cap_enquiry_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ue_cap_enquiry_r8_ies_s& ue_cap_enquiry_s::crit_exts_c_::c1_c_::set_ue_cap_enquiry_r8()
{
  set(types::ue_cap_enquiry_r8);
  return c;
}
void ue_cap_enquiry_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ue_cap_enquiry_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ue_cap_enquiry_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ue_cap_enquiry_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_cap_enquiry_r8:
      j.write_fieldname("ueCapabilityEnquiry-r8");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_cap_enquiry_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_cap_enquiry_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_cap_enquiry_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UE-RadioPagingInfo-r12 ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ue_category_v1250_present, 1));

  if (ue_category_v1250_present) {
    HANDLE_CODE(pack_integer(bref, ue_category_v1250, (uint8_t)0u, (uint8_t)0u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ue_category_dl_v1310_present;
    group_flags[0] |= ce_mode_a_r13_present;
    group_flags[0] |= ce_mode_b_r13_present;
    group_flags[1] |= wake_up_signal_r15_present;
    group_flags[1] |= wake_up_signal_tdd_r15_present;
    group_flags[1] |= wake_up_signal_min_gap_e_drx_r15_present;
    group_flags[1] |= wake_up_signal_min_gap_e_drx_tdd_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ue_category_dl_v1310_present, 1));
      HANDLE_CODE(bref.pack(ce_mode_a_r13_present, 1));
      HANDLE_CODE(bref.pack(ce_mode_b_r13_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(wake_up_signal_r15_present, 1));
      HANDLE_CODE(bref.pack(wake_up_signal_tdd_r15_present, 1));
      HANDLE_CODE(bref.pack(wake_up_signal_min_gap_e_drx_r15_present, 1));
      HANDLE_CODE(bref.pack(wake_up_signal_min_gap_e_drx_tdd_r15_present, 1));
      if (wake_up_signal_min_gap_e_drx_r15_present) {
        HANDLE_CODE(wake_up_signal_min_gap_e_drx_r15.pack(bref));
      }
      if (wake_up_signal_min_gap_e_drx_tdd_r15_present) {
        HANDLE_CODE(wake_up_signal_min_gap_e_drx_tdd_r15.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ue_category_v1250_present, 1));

  if (ue_category_v1250_present) {
    HANDLE_CODE(unpack_integer(ue_category_v1250, bref, (uint8_t)0u, (uint8_t)0u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ue_category_dl_v1310_present, 1));
      HANDLE_CODE(bref.unpack(ce_mode_a_r13_present, 1));
      HANDLE_CODE(bref.unpack(ce_mode_b_r13_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(wake_up_signal_r15_present, 1));
      HANDLE_CODE(bref.unpack(wake_up_signal_tdd_r15_present, 1));
      HANDLE_CODE(bref.unpack(wake_up_signal_min_gap_e_drx_r15_present, 1));
      HANDLE_CODE(bref.unpack(wake_up_signal_min_gap_e_drx_tdd_r15_present, 1));
      if (wake_up_signal_min_gap_e_drx_r15_present) {
        HANDLE_CODE(wake_up_signal_min_gap_e_drx_r15.unpack(bref));
      }
      if (wake_up_signal_min_gap_e_drx_tdd_r15_present) {
        HANDLE_CODE(wake_up_signal_min_gap_e_drx_tdd_r15.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_category_v1250_present) {
    j.write_int("ue-Category-v1250", ue_category_v1250);
  }
  if (ext) {
    if (ue_category_dl_v1310_present) {
      j.write_str("ue-CategoryDL-v1310", "m1");
    }
    if (ce_mode_a_r13_present) {
      j.write_str("ce-ModeA-r13", "true");
    }
    if (ce_mode_b_r13_present) {
      j.write_str("ce-ModeB-r13", "true");
    }
    if (wake_up_signal_r15_present) {
      j.write_str("wakeUpSignal-r15", "true");
    }
    if (wake_up_signal_tdd_r15_present) {
      j.write_str("wakeUpSignal-TDD-r15", "true");
    }
    if (wake_up_signal_min_gap_e_drx_r15_present) {
      j.write_str("wakeUpSignalMinGap-eDRX-r15", wake_up_signal_min_gap_e_drx_r15.to_string());
    }
    if (wake_up_signal_min_gap_e_drx_tdd_r15_present) {
      j.write_str("wakeUpSignalMinGap-eDRX-TDD-r15", wake_up_signal_min_gap_e_drx_tdd_r15.to_string());
    }
  }
  j.end_obj();
}

const char* ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_r15_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms240", "ms1000", "ms2000"};
  return convert_enum_idx(options, 4, value, "ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_r15_e_");
}
uint16_t ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_r15_opts::to_number() const
{
  static const uint16_t options[] = {40, 240, 1000, 2000};
  return map_enum_number(options, 4, value, "ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_r15_e_");
}

const char* ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_tdd_r15_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms240", "ms1000", "ms2000"};
  return convert_enum_idx(options, 4, value, "ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_tdd_r15_e_");
}
uint16_t ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_tdd_r15_opts::to_number() const
{
  static const uint16_t options[] = {40, 240, 1000, 2000};
  return map_enum_number(options, 4, value, "ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_tdd_r15_e_");
}

// UE-CapabilityRAT-Container ::= SEQUENCE
SRSASN_CODE ue_cap_rat_container_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rat_type.pack(bref));
  HANDLE_CODE(ue_cap_rat_container.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_rat_container_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rat_type.unpack(bref));
  HANDLE_CODE(ue_cap_rat_container.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_cap_rat_container_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rat-Type", rat_type.to_string());
  j.write_str("ueCapabilityRAT-Container", ue_cap_rat_container.to_string());
  j.end_obj();
}

// UECapabilityInformation-v1250-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_info_v1250_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_radio_paging_info_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_radio_paging_info_r12_present) {
    HANDLE_CODE(ue_radio_paging_info_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_v1250_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_radio_paging_info_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_radio_paging_info_r12_present) {
    HANDLE_CODE(ue_radio_paging_info_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_info_v1250_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_radio_paging_info_r12_present) {
    j.write_fieldname("ue-RadioPagingInfo-r12");
    ue_radio_paging_info_r12.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UECapabilityInformation-v8a0-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_info_v8a0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE ue_cap_info_v8a0_ies_s::unpack(cbit_ref& bref)
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
void ue_cap_info_v8a0_ies_s::to_json(json_writer& j) const
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

// UECapabilityInformation-r8-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_info_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, ue_cap_rat_container_list, 0, 8));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(ue_cap_rat_container_list, bref, 0, 8));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_info_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("ue-CapabilityRAT-ContainerList");
  for (const auto& e1 : ue_cap_rat_container_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UECapabilityInformation ::= SEQUENCE
SRSASN_CODE ue_cap_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_cap_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_cap_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_cap_info_s::crit_exts_c_::c1_c_& ue_cap_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ue_cap_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_cap_info_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ue_cap_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_cap_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ue_cap_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_cap_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ue_cap_info_r8_ies_s& ue_cap_info_s::crit_exts_c_::c1_c_::set_ue_cap_info_r8()
{
  set(types::ue_cap_info_r8);
  return c;
}
void ue_cap_info_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void ue_cap_info_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void ue_cap_info_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void ue_cap_info_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void ue_cap_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ue_cap_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ue_cap_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ue_cap_info_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_cap_info_r8:
      j.write_fieldname("ueCapabilityInformation-r8");
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
      log_invalid_choice_id(type_, "ue_cap_info_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_cap_info_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_cap_info_r8:
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
      log_invalid_choice_id(type_, "ue_cap_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_cap_info_r8:
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
      log_invalid_choice_id(type_, "ue_cap_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// InterFreqBandInfo ::= SEQUENCE
SRSASN_CODE inter_freq_band_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_freq_need_for_gaps, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_band_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(inter_freq_need_for_gaps, 1));

  return SRSASN_SUCCESS;
}
void inter_freq_band_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("interFreqNeedForGaps", inter_freq_need_for_gaps);
  j.end_obj();
}

// InterRAT-BandInfo ::= SEQUENCE
SRSASN_CODE inter_rat_band_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_rat_need_for_gaps, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_rat_band_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(inter_rat_need_for_gaps, 1));

  return SRSASN_SUCCESS;
}
void inter_rat_band_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("interRAT-NeedForGaps", inter_rat_need_for_gaps);
  j.end_obj();
}

// BandInfoEUTRA ::= SEQUENCE
SRSASN_CODE band_info_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_rat_band_list_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_band_list, 1, 64));
  if (inter_rat_band_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_rat_band_list, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_info_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(inter_rat_band_list_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(inter_freq_band_list, bref, 1, 64));
  if (inter_rat_band_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_rat_band_list, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void band_info_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("interFreqBandList");
  for (const auto& e1 : inter_freq_band_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (inter_rat_band_list_present) {
    j.start_array("interRAT-BandList");
    for (const auto& e1 : inter_rat_band_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// MIMO-CapabilityDL-r10 ::= ENUMERATED
const char* mimo_cap_dl_r10_opts::to_string() const
{
  static const char* options[] = {"twoLayers", "fourLayers", "eightLayers"};
  return convert_enum_idx(options, 3, value, "mimo_cap_dl_r10_e");
}
uint8_t mimo_cap_dl_r10_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8};
  return map_enum_number(options, 3, value, "mimo_cap_dl_r10_e");
}

// MIMO-CapabilityUL-r10 ::= ENUMERATED
const char* mimo_cap_ul_r10_opts::to_string() const
{
  static const char* options[] = {"twoLayers", "fourLayers"};
  return convert_enum_idx(options, 2, value, "mimo_cap_ul_r10_e");
}
uint8_t mimo_cap_ul_r10_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "mimo_cap_ul_r10_e");
}

// CA-MIMO-ParametersDL-r10 ::= SEQUENCE
SRSASN_CODE ca_mimo_params_dl_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_mimo_cap_dl_r10_present, 1));

  HANDLE_CODE(ca_bw_class_dl_r10.pack(bref));
  if (supported_mimo_cap_dl_r10_present) {
    HANDLE_CODE(supported_mimo_cap_dl_r10.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_mimo_params_dl_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_mimo_cap_dl_r10_present, 1));

  HANDLE_CODE(ca_bw_class_dl_r10.unpack(bref));
  if (supported_mimo_cap_dl_r10_present) {
    HANDLE_CODE(supported_mimo_cap_dl_r10.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ca_mimo_params_dl_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ca-BandwidthClassDL-r10", ca_bw_class_dl_r10.to_string());
  if (supported_mimo_cap_dl_r10_present) {
    j.write_str("supportedMIMO-CapabilityDL-r10", supported_mimo_cap_dl_r10.to_string());
  }
  j.end_obj();
}

// CA-MIMO-ParametersUL-r10 ::= SEQUENCE
SRSASN_CODE ca_mimo_params_ul_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_mimo_cap_ul_r10_present, 1));

  HANDLE_CODE(ca_bw_class_ul_r10.pack(bref));
  if (supported_mimo_cap_ul_r10_present) {
    HANDLE_CODE(supported_mimo_cap_ul_r10.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_mimo_params_ul_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_mimo_cap_ul_r10_present, 1));

  HANDLE_CODE(ca_bw_class_ul_r10.unpack(bref));
  if (supported_mimo_cap_ul_r10_present) {
    HANDLE_CODE(supported_mimo_cap_ul_r10.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ca_mimo_params_ul_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ca-BandwidthClassUL-r10", ca_bw_class_ul_r10.to_string());
  if (supported_mimo_cap_ul_r10_present) {
    j.write_str("supportedMIMO-CapabilityUL-r10", supported_mimo_cap_ul_r10.to_string());
  }
  j.end_obj();
}

// BandParameters-r10 ::= SEQUENCE
SRSASN_CODE band_params_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_params_ul_r10_present, 1));
  HANDLE_CODE(bref.pack(band_params_dl_r10_present, 1));

  HANDLE_CODE(pack_integer(bref, band_eutra_r10, (uint8_t)1u, (uint8_t)64u));
  if (band_params_ul_r10_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_params_ul_r10, 1, 16));
  }
  if (band_params_dl_r10_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_params_dl_r10, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_params_ul_r10_present, 1));
  HANDLE_CODE(bref.unpack(band_params_dl_r10_present, 1));

  HANDLE_CODE(unpack_integer(band_eutra_r10, bref, (uint8_t)1u, (uint8_t)64u));
  if (band_params_ul_r10_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_params_ul_r10, bref, 1, 16));
  }
  if (band_params_dl_r10_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_params_dl_r10, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void band_params_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandEUTRA-r10", band_eutra_r10);
  if (band_params_ul_r10_present) {
    j.start_array("bandParametersUL-r10");
    for (const auto& e1 : band_params_ul_r10) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (band_params_dl_r10_present) {
    j.start_array("bandParametersDL-r10");
    for (const auto& e1 : band_params_dl_r10) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// BandParameters-r11 ::= SEQUENCE
SRSASN_CODE band_params_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_params_ul_r11_present, 1));
  HANDLE_CODE(bref.pack(band_params_dl_r11_present, 1));
  HANDLE_CODE(bref.pack(supported_csi_proc_r11_present, 1));

  HANDLE_CODE(pack_integer(bref, band_eutra_r11, (uint16_t)1u, (uint16_t)256u));
  if (band_params_ul_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_params_ul_r11, 1, 16));
  }
  if (band_params_dl_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_params_dl_r11, 1, 16));
  }
  if (supported_csi_proc_r11_present) {
    HANDLE_CODE(supported_csi_proc_r11.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_params_ul_r11_present, 1));
  HANDLE_CODE(bref.unpack(band_params_dl_r11_present, 1));
  HANDLE_CODE(bref.unpack(supported_csi_proc_r11_present, 1));

  HANDLE_CODE(unpack_integer(band_eutra_r11, bref, (uint16_t)1u, (uint16_t)256u));
  if (band_params_ul_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_params_ul_r11, bref, 1, 16));
  }
  if (band_params_dl_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_params_dl_r11, bref, 1, 16));
  }
  if (supported_csi_proc_r11_present) {
    HANDLE_CODE(supported_csi_proc_r11.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_params_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandEUTRA-r11", band_eutra_r11);
  if (band_params_ul_r11_present) {
    j.start_array("bandParametersUL-r11");
    for (const auto& e1 : band_params_ul_r11) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (band_params_dl_r11_present) {
    j.start_array("bandParametersDL-r11");
    for (const auto& e1 : band_params_dl_r11) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_csi_proc_r11_present) {
    j.write_str("supportedCSI-Proc-r11", supported_csi_proc_r11.to_string());
  }
  j.end_obj();
}

const char* band_params_r11_s::supported_csi_proc_r11_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n4"};
  return convert_enum_idx(options, 3, value, "band_params_r11_s::supported_csi_proc_r11_e_");
}
uint8_t band_params_r11_s::supported_csi_proc_r11_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 4};
  return map_enum_number(options, 3, value, "band_params_r11_s::supported_csi_proc_r11_e_");
}

// BandCombinationParameters-r11 ::= SEQUENCE
SRSASN_CODE band_combination_params_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(supported_bw_combination_set_r11_present, 1));
  HANDLE_CODE(bref.pack(multiple_timing_advance_r11_present, 1));
  HANDLE_CODE(bref.pack(simul_rx_tx_r11_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, band_param_list_r11, 1, 64));
  if (supported_bw_combination_set_r11_present) {
    HANDLE_CODE(supported_bw_combination_set_r11.pack(bref));
  }
  HANDLE_CODE(band_info_eutra_r11.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(supported_bw_combination_set_r11_present, 1));
  HANDLE_CODE(bref.unpack(multiple_timing_advance_r11_present, 1));
  HANDLE_CODE(bref.unpack(simul_rx_tx_r11_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(band_param_list_r11, bref, 1, 64));
  if (supported_bw_combination_set_r11_present) {
    HANDLE_CODE(supported_bw_combination_set_r11.unpack(bref));
  }
  HANDLE_CODE(band_info_eutra_r11.unpack(bref));

  return SRSASN_SUCCESS;
}
void band_combination_params_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("bandParameterList-r11");
  for (const auto& e1 : band_param_list_r11) {
    e1.to_json(j);
  }
  j.end_array();
  if (supported_bw_combination_set_r11_present) {
    j.write_str("supportedBandwidthCombinationSet-r11", supported_bw_combination_set_r11.to_string());
  }
  if (multiple_timing_advance_r11_present) {
    j.write_str("multipleTimingAdvance-r11", "supported");
  }
  if (simul_rx_tx_r11_present) {
    j.write_str("simultaneousRx-Tx-r11", "supported");
  }
  j.write_fieldname("bandInfoEUTRA-r11");
  band_info_eutra_r11.to_json(j);
  j.end_obj();
}

// IntraBandContiguousCC-Info-r12 ::= SEQUENCE
SRSASN_CODE intra_band_contiguous_cc_info_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(four_layer_tm3_tm4_per_cc_r12_present, 1));
  HANDLE_CODE(bref.pack(supported_mimo_cap_dl_r12_present, 1));
  HANDLE_CODE(bref.pack(supported_csi_proc_r12_present, 1));

  if (supported_mimo_cap_dl_r12_present) {
    HANDLE_CODE(supported_mimo_cap_dl_r12.pack(bref));
  }
  if (supported_csi_proc_r12_present) {
    HANDLE_CODE(supported_csi_proc_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_band_contiguous_cc_info_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(four_layer_tm3_tm4_per_cc_r12_present, 1));
  HANDLE_CODE(bref.unpack(supported_mimo_cap_dl_r12_present, 1));
  HANDLE_CODE(bref.unpack(supported_csi_proc_r12_present, 1));

  if (supported_mimo_cap_dl_r12_present) {
    HANDLE_CODE(supported_mimo_cap_dl_r12.unpack(bref));
  }
  if (supported_csi_proc_r12_present) {
    HANDLE_CODE(supported_csi_proc_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void intra_band_contiguous_cc_info_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (four_layer_tm3_tm4_per_cc_r12_present) {
    j.write_str("fourLayerTM3-TM4-perCC-r12", "supported");
  }
  if (supported_mimo_cap_dl_r12_present) {
    j.write_str("supportedMIMO-CapabilityDL-r12", supported_mimo_cap_dl_r12.to_string());
  }
  if (supported_csi_proc_r12_present) {
    j.write_str("supportedCSI-Proc-r12", supported_csi_proc_r12.to_string());
  }
  j.end_obj();
}

const char* intra_band_contiguous_cc_info_r12_s::supported_csi_proc_r12_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n4"};
  return convert_enum_idx(options, 3, value, "intra_band_contiguous_cc_info_r12_s::supported_csi_proc_r12_e_");
}
uint8_t intra_band_contiguous_cc_info_r12_s::supported_csi_proc_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 4};
  return map_enum_number(options, 3, value, "intra_band_contiguous_cc_info_r12_s::supported_csi_proc_r12_e_");
}

// CA-MIMO-ParametersDL-r13 ::= SEQUENCE
SRSASN_CODE ca_mimo_params_dl_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_mimo_cap_dl_r13_present, 1));
  HANDLE_CODE(bref.pack(four_layer_tm3_tm4_r13_present, 1));

  HANDLE_CODE(ca_bw_class_dl_r13.pack(bref));
  if (supported_mimo_cap_dl_r13_present) {
    HANDLE_CODE(supported_mimo_cap_dl_r13.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, intra_band_contiguous_cc_info_list_r13, 1, 32));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_mimo_params_dl_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_mimo_cap_dl_r13_present, 1));
  HANDLE_CODE(bref.unpack(four_layer_tm3_tm4_r13_present, 1));

  HANDLE_CODE(ca_bw_class_dl_r13.unpack(bref));
  if (supported_mimo_cap_dl_r13_present) {
    HANDLE_CODE(supported_mimo_cap_dl_r13.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(intra_band_contiguous_cc_info_list_r13, bref, 1, 32));

  return SRSASN_SUCCESS;
}
void ca_mimo_params_dl_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ca-BandwidthClassDL-r13", ca_bw_class_dl_r13.to_string());
  if (supported_mimo_cap_dl_r13_present) {
    j.write_str("supportedMIMO-CapabilityDL-r13", supported_mimo_cap_dl_r13.to_string());
  }
  if (four_layer_tm3_tm4_r13_present) {
    j.write_str("fourLayerTM3-TM4-r13", "supported");
  }
  j.start_array("intraBandContiguousCC-InfoList-r13");
  for (const auto& e1 : intra_band_contiguous_cc_info_list_r13) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// BandParameters-r13 ::= SEQUENCE
SRSASN_CODE band_params_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_params_ul_r13_present, 1));
  HANDLE_CODE(bref.pack(band_params_dl_r13_present, 1));
  HANDLE_CODE(bref.pack(supported_csi_proc_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, band_eutra_r13, (uint16_t)1u, (uint16_t)256u));
  if (band_params_ul_r13_present) {
    HANDLE_CODE(band_params_ul_r13.pack(bref));
  }
  if (band_params_dl_r13_present) {
    HANDLE_CODE(band_params_dl_r13.pack(bref));
  }
  if (supported_csi_proc_r13_present) {
    HANDLE_CODE(supported_csi_proc_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_params_ul_r13_present, 1));
  HANDLE_CODE(bref.unpack(band_params_dl_r13_present, 1));
  HANDLE_CODE(bref.unpack(supported_csi_proc_r13_present, 1));

  HANDLE_CODE(unpack_integer(band_eutra_r13, bref, (uint16_t)1u, (uint16_t)256u));
  if (band_params_ul_r13_present) {
    HANDLE_CODE(band_params_ul_r13.unpack(bref));
  }
  if (band_params_dl_r13_present) {
    HANDLE_CODE(band_params_dl_r13.unpack(bref));
  }
  if (supported_csi_proc_r13_present) {
    HANDLE_CODE(supported_csi_proc_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_params_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandEUTRA-r13", band_eutra_r13);
  if (band_params_ul_r13_present) {
    j.write_fieldname("bandParametersUL-r13");
    band_params_ul_r13.to_json(j);
  }
  if (band_params_dl_r13_present) {
    j.write_fieldname("bandParametersDL-r13");
    band_params_dl_r13.to_json(j);
  }
  if (supported_csi_proc_r13_present) {
    j.write_str("supportedCSI-Proc-r13", supported_csi_proc_r13.to_string());
  }
  j.end_obj();
}

const char* band_params_r13_s::supported_csi_proc_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n4"};
  return convert_enum_idx(options, 3, value, "band_params_r13_s::supported_csi_proc_r13_e_");
}
uint8_t band_params_r13_s::supported_csi_proc_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 4};
  return map_enum_number(options, 3, value, "band_params_r13_s::supported_csi_proc_r13_e_");
}

// BandCombinationParameters-r13 ::= SEQUENCE
SRSASN_CODE band_combination_params_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(different_fallback_supported_r13_present, 1));
  HANDLE_CODE(bref.pack(supported_bw_combination_set_r13_present, 1));
  HANDLE_CODE(bref.pack(multiple_timing_advance_r13_present, 1));
  HANDLE_CODE(bref.pack(simul_rx_tx_r13_present, 1));
  HANDLE_CODE(bref.pack(dc_support_r13_present, 1));
  HANDLE_CODE(bref.pack(supported_naics_minus2_crs_ap_r13_present, 1));
  HANDLE_CODE(bref.pack(comm_supported_bands_per_bc_r13_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, band_param_list_r13, 1, 64));
  if (supported_bw_combination_set_r13_present) {
    HANDLE_CODE(supported_bw_combination_set_r13.pack(bref));
  }
  HANDLE_CODE(band_info_eutra_r13.pack(bref));
  if (dc_support_r13_present) {
    HANDLE_CODE(bref.pack(dc_support_r13.async_r13_present, 1));
    HANDLE_CODE(bref.pack(dc_support_r13.supported_cell_grouping_r13_present, 1));
    if (dc_support_r13.supported_cell_grouping_r13_present) {
      HANDLE_CODE(dc_support_r13.supported_cell_grouping_r13.pack(bref));
    }
  }
  if (supported_naics_minus2_crs_ap_r13_present) {
    HANDLE_CODE(supported_naics_minus2_crs_ap_r13.pack(bref));
  }
  if (comm_supported_bands_per_bc_r13_present) {
    HANDLE_CODE(comm_supported_bands_per_bc_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(different_fallback_supported_r13_present, 1));
  HANDLE_CODE(bref.unpack(supported_bw_combination_set_r13_present, 1));
  HANDLE_CODE(bref.unpack(multiple_timing_advance_r13_present, 1));
  HANDLE_CODE(bref.unpack(simul_rx_tx_r13_present, 1));
  HANDLE_CODE(bref.unpack(dc_support_r13_present, 1));
  HANDLE_CODE(bref.unpack(supported_naics_minus2_crs_ap_r13_present, 1));
  HANDLE_CODE(bref.unpack(comm_supported_bands_per_bc_r13_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(band_param_list_r13, bref, 1, 64));
  if (supported_bw_combination_set_r13_present) {
    HANDLE_CODE(supported_bw_combination_set_r13.unpack(bref));
  }
  HANDLE_CODE(band_info_eutra_r13.unpack(bref));
  if (dc_support_r13_present) {
    HANDLE_CODE(bref.unpack(dc_support_r13.async_r13_present, 1));
    HANDLE_CODE(bref.unpack(dc_support_r13.supported_cell_grouping_r13_present, 1));
    if (dc_support_r13.supported_cell_grouping_r13_present) {
      HANDLE_CODE(dc_support_r13.supported_cell_grouping_r13.unpack(bref));
    }
  }
  if (supported_naics_minus2_crs_ap_r13_present) {
    HANDLE_CODE(supported_naics_minus2_crs_ap_r13.unpack(bref));
  }
  if (comm_supported_bands_per_bc_r13_present) {
    HANDLE_CODE(comm_supported_bands_per_bc_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_combination_params_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (different_fallback_supported_r13_present) {
    j.write_str("differentFallbackSupported-r13", "true");
  }
  j.start_array("bandParameterList-r13");
  for (const auto& e1 : band_param_list_r13) {
    e1.to_json(j);
  }
  j.end_array();
  if (supported_bw_combination_set_r13_present) {
    j.write_str("supportedBandwidthCombinationSet-r13", supported_bw_combination_set_r13.to_string());
  }
  if (multiple_timing_advance_r13_present) {
    j.write_str("multipleTimingAdvance-r13", "supported");
  }
  if (simul_rx_tx_r13_present) {
    j.write_str("simultaneousRx-Tx-r13", "supported");
  }
  j.write_fieldname("bandInfoEUTRA-r13");
  band_info_eutra_r13.to_json(j);
  if (dc_support_r13_present) {
    j.write_fieldname("dc-Support-r13");
    j.start_obj();
    if (dc_support_r13.async_r13_present) {
      j.write_str("asynchronous-r13", "supported");
    }
    if (dc_support_r13.supported_cell_grouping_r13_present) {
      j.write_fieldname("supportedCellGrouping-r13");
      dc_support_r13.supported_cell_grouping_r13.to_json(j);
    }
    j.end_obj();
  }
  if (supported_naics_minus2_crs_ap_r13_present) {
    j.write_str("supportedNAICS-2CRS-AP-r13", supported_naics_minus2_crs_ap_r13.to_string());
  }
  if (comm_supported_bands_per_bc_r13_present) {
    j.write_str("commSupportedBandsPerBC-r13", comm_supported_bands_per_bc_r13.to_string());
  }
  j.end_obj();
}

void band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::destroy_()
{
  switch (type_) {
    case types::three_entries_r13:
      c.destroy<fixed_bitstring<3> >();
      break;
    case types::four_entries_r13:
      c.destroy<fixed_bitstring<7> >();
      break;
    case types::five_entries_r13:
      c.destroy<fixed_bitstring<15> >();
      break;
    default:
      break;
  }
}
void band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::three_entries_r13:
      c.init<fixed_bitstring<3> >();
      break;
    case types::four_entries_r13:
      c.init<fixed_bitstring<7> >();
      break;
    case types::five_entries_r13:
      c.init<fixed_bitstring<15> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_");
  }
}
band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::supported_cell_grouping_r13_c_(
    const band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::three_entries_r13:
      c.init(other.c.get<fixed_bitstring<3> >());
      break;
    case types::four_entries_r13:
      c.init(other.c.get<fixed_bitstring<7> >());
      break;
    case types::five_entries_r13:
      c.init(other.c.get<fixed_bitstring<15> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_");
  }
}
band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_&
band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::operator=(
    const band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::three_entries_r13:
      c.set(other.c.get<fixed_bitstring<3> >());
      break;
    case types::four_entries_r13:
      c.set(other.c.get<fixed_bitstring<7> >());
      break;
    case types::five_entries_r13:
      c.set(other.c.get<fixed_bitstring<15> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_");
  }

  return *this;
}
fixed_bitstring<3>&
band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::set_three_entries_r13()
{
  set(types::three_entries_r13);
  return c.get<fixed_bitstring<3> >();
}
fixed_bitstring<7>&
band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::set_four_entries_r13()
{
  set(types::four_entries_r13);
  return c.get<fixed_bitstring<7> >();
}
fixed_bitstring<15>&
band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::set_five_entries_r13()
{
  set(types::five_entries_r13);
  return c.get<fixed_bitstring<15> >();
}
void band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::three_entries_r13:
      j.write_str("threeEntries-r13", c.get<fixed_bitstring<3> >().to_string());
      break;
    case types::four_entries_r13:
      j.write_str("fourEntries-r13", c.get<fixed_bitstring<7> >().to_string());
      break;
    case types::five_entries_r13:
      j.write_str("fiveEntries-r13", c.get<fixed_bitstring<15> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::three_entries_r13:
      HANDLE_CODE(c.get<fixed_bitstring<3> >().pack(bref));
      break;
    case types::four_entries_r13:
      HANDLE_CODE(c.get<fixed_bitstring<7> >().pack(bref));
      break;
    case types::five_entries_r13:
      HANDLE_CODE(c.get<fixed_bitstring<15> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::three_entries_r13:
      HANDLE_CODE(c.get<fixed_bitstring<3> >().unpack(bref));
      break;
    case types::four_entries_r13:
      HANDLE_CODE(c.get<fixed_bitstring<7> >().unpack(bref));
      break;
    case types::five_entries_r13:
      HANDLE_CODE(c.get<fixed_bitstring<15> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// BandParameters-v1090 ::= SEQUENCE
SRSASN_CODE band_params_v1090_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(band_eutra_v1090_present, 1));

  if (band_eutra_v1090_present) {
    HANDLE_CODE(pack_integer(bref, band_eutra_v1090, (uint16_t)65u, (uint16_t)256u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_v1090_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(band_eutra_v1090_present, 1));

  if (band_eutra_v1090_present) {
    HANDLE_CODE(unpack_integer(band_eutra_v1090, bref, (uint16_t)65u, (uint16_t)256u));
  }

  return SRSASN_SUCCESS;
}
void band_params_v1090_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_eutra_v1090_present) {
    j.write_int("bandEUTRA-v1090", band_eutra_v1090);
  }
  j.end_obj();
}

// CA-MIMO-ParametersDL-v10i0 ::= SEQUENCE
SRSASN_CODE ca_mimo_params_dl_v10i0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(four_layer_tm3_tm4_r10_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_mimo_params_dl_v10i0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(four_layer_tm3_tm4_r10_present, 1));

  return SRSASN_SUCCESS;
}
void ca_mimo_params_dl_v10i0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (four_layer_tm3_tm4_r10_present) {
    j.write_str("fourLayerTM3-TM4-r10", "supported");
  }
  j.end_obj();
}

// BandParameters-v10i0 ::= SEQUENCE
SRSASN_CODE band_params_v10i0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, band_params_dl_v10i0, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_v10i0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(band_params_dl_v10i0, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void band_params_v10i0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("bandParametersDL-v10i0");
  for (const auto& e1 : band_params_dl_v10i0) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// BandCombinationParameters-v10i0 ::= SEQUENCE
SRSASN_CODE band_combination_params_v10i0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_param_list_v10i0_present, 1));

  if (band_param_list_v10i0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_param_list_v10i0, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_v10i0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_param_list_v10i0_present, 1));

  if (band_param_list_v10i0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_param_list_v10i0, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void band_combination_params_v10i0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_param_list_v10i0_present) {
    j.start_array("bandParameterList-v10i0");
    for (const auto& e1 : band_param_list_v10i0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// BandParameters-v1130 ::= SEQUENCE
SRSASN_CODE band_params_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(supported_csi_proc_r11.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(supported_csi_proc_r11.unpack(bref));

  return SRSASN_SUCCESS;
}
void band_params_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("supportedCSI-Proc-r11", supported_csi_proc_r11.to_string());
  j.end_obj();
}

const char* band_params_v1130_s::supported_csi_proc_r11_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n4"};
  return convert_enum_idx(options, 3, value, "band_params_v1130_s::supported_csi_proc_r11_e_");
}
uint8_t band_params_v1130_s::supported_csi_proc_r11_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 4};
  return map_enum_number(options, 3, value, "band_params_v1130_s::supported_csi_proc_r11_e_");
}

// BandCombinationParameters-v1130 ::= SEQUENCE
SRSASN_CODE band_combination_params_v1130_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(multiple_timing_advance_r11_present, 1));
  HANDLE_CODE(bref.pack(simul_rx_tx_r11_present, 1));
  HANDLE_CODE(bref.pack(band_param_list_r11_present, 1));

  if (band_param_list_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_param_list_r11, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_v1130_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(multiple_timing_advance_r11_present, 1));
  HANDLE_CODE(bref.unpack(simul_rx_tx_r11_present, 1));
  HANDLE_CODE(bref.unpack(band_param_list_r11_present, 1));

  if (band_param_list_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_param_list_r11, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void band_combination_params_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (multiple_timing_advance_r11_present) {
    j.write_str("multipleTimingAdvance-r11", "supported");
  }
  if (simul_rx_tx_r11_present) {
    j.write_str("simultaneousRx-Tx-r11", "supported");
  }
  if (band_param_list_r11_present) {
    j.start_array("bandParameterList-r11");
    for (const auto& e1 : band_param_list_r11) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// BandCombinationParameters-v1250 ::= SEQUENCE
SRSASN_CODE band_combination_params_v1250_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dc_support_r12_present, 1));
  HANDLE_CODE(bref.pack(supported_naics_minus2_crs_ap_r12_present, 1));
  HANDLE_CODE(bref.pack(comm_supported_bands_per_bc_r12_present, 1));

  if (dc_support_r12_present) {
    HANDLE_CODE(bref.pack(dc_support_r12.async_r12_present, 1));
    HANDLE_CODE(bref.pack(dc_support_r12.supported_cell_grouping_r12_present, 1));
    if (dc_support_r12.supported_cell_grouping_r12_present) {
      HANDLE_CODE(dc_support_r12.supported_cell_grouping_r12.pack(bref));
    }
  }
  if (supported_naics_minus2_crs_ap_r12_present) {
    HANDLE_CODE(supported_naics_minus2_crs_ap_r12.pack(bref));
  }
  if (comm_supported_bands_per_bc_r12_present) {
    HANDLE_CODE(comm_supported_bands_per_bc_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_v1250_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dc_support_r12_present, 1));
  HANDLE_CODE(bref.unpack(supported_naics_minus2_crs_ap_r12_present, 1));
  HANDLE_CODE(bref.unpack(comm_supported_bands_per_bc_r12_present, 1));

  if (dc_support_r12_present) {
    HANDLE_CODE(bref.unpack(dc_support_r12.async_r12_present, 1));
    HANDLE_CODE(bref.unpack(dc_support_r12.supported_cell_grouping_r12_present, 1));
    if (dc_support_r12.supported_cell_grouping_r12_present) {
      HANDLE_CODE(dc_support_r12.supported_cell_grouping_r12.unpack(bref));
    }
  }
  if (supported_naics_minus2_crs_ap_r12_present) {
    HANDLE_CODE(supported_naics_minus2_crs_ap_r12.unpack(bref));
  }
  if (comm_supported_bands_per_bc_r12_present) {
    HANDLE_CODE(comm_supported_bands_per_bc_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_combination_params_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dc_support_r12_present) {
    j.write_fieldname("dc-Support-r12");
    j.start_obj();
    if (dc_support_r12.async_r12_present) {
      j.write_str("asynchronous-r12", "supported");
    }
    if (dc_support_r12.supported_cell_grouping_r12_present) {
      j.write_fieldname("supportedCellGrouping-r12");
      dc_support_r12.supported_cell_grouping_r12.to_json(j);
    }
    j.end_obj();
  }
  if (supported_naics_minus2_crs_ap_r12_present) {
    j.write_str("supportedNAICS-2CRS-AP-r12", supported_naics_minus2_crs_ap_r12.to_string());
  }
  if (comm_supported_bands_per_bc_r12_present) {
    j.write_str("commSupportedBandsPerBC-r12", comm_supported_bands_per_bc_r12.to_string());
  }
  j.end_obj();
}

void band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::destroy_()
{
  switch (type_) {
    case types::three_entries_r12:
      c.destroy<fixed_bitstring<3> >();
      break;
    case types::four_entries_r12:
      c.destroy<fixed_bitstring<7> >();
      break;
    case types::five_entries_r12:
      c.destroy<fixed_bitstring<15> >();
      break;
    default:
      break;
  }
}
void band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::three_entries_r12:
      c.init<fixed_bitstring<3> >();
      break;
    case types::four_entries_r12:
      c.init<fixed_bitstring<7> >();
      break;
    case types::five_entries_r12:
      c.init<fixed_bitstring<15> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_");
  }
}
band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::supported_cell_grouping_r12_c_(
    const band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::three_entries_r12:
      c.init(other.c.get<fixed_bitstring<3> >());
      break;
    case types::four_entries_r12:
      c.init(other.c.get<fixed_bitstring<7> >());
      break;
    case types::five_entries_r12:
      c.init(other.c.get<fixed_bitstring<15> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_");
  }
}
band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_&
band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::operator=(
    const band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::three_entries_r12:
      c.set(other.c.get<fixed_bitstring<3> >());
      break;
    case types::four_entries_r12:
      c.set(other.c.get<fixed_bitstring<7> >());
      break;
    case types::five_entries_r12:
      c.set(other.c.get<fixed_bitstring<15> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_");
  }

  return *this;
}
fixed_bitstring<3>&
band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::set_three_entries_r12()
{
  set(types::three_entries_r12);
  return c.get<fixed_bitstring<3> >();
}
fixed_bitstring<7>&
band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::set_four_entries_r12()
{
  set(types::four_entries_r12);
  return c.get<fixed_bitstring<7> >();
}
fixed_bitstring<15>&
band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::set_five_entries_r12()
{
  set(types::five_entries_r12);
  return c.get<fixed_bitstring<15> >();
}
void band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::three_entries_r12:
      j.write_str("threeEntries-r12", c.get<fixed_bitstring<3> >().to_string());
      break;
    case types::four_entries_r12:
      j.write_str("fourEntries-r12", c.get<fixed_bitstring<7> >().to_string());
      break;
    case types::five_entries_r12:
      j.write_str("fiveEntries-r12", c.get<fixed_bitstring<15> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_,
                            "band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE
band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::three_entries_r12:
      HANDLE_CODE(c.get<fixed_bitstring<3> >().pack(bref));
      break;
    case types::four_entries_r12:
      HANDLE_CODE(c.get<fixed_bitstring<7> >().pack(bref));
      break;
    case types::five_entries_r12:
      HANDLE_CODE(c.get<fixed_bitstring<15> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_,
                            "band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::three_entries_r12:
      HANDLE_CODE(c.get<fixed_bitstring<3> >().unpack(bref));
      break;
    case types::four_entries_r12:
      HANDLE_CODE(c.get<fixed_bitstring<7> >().unpack(bref));
      break;
    case types::five_entries_r12:
      HANDLE_CODE(c.get<fixed_bitstring<15> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_,
                            "band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CA-MIMO-ParametersDL-v1270 ::= SEQUENCE
SRSASN_CODE ca_mimo_params_dl_v1270_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, intra_band_contiguous_cc_info_list_r12, 1, 5));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_mimo_params_dl_v1270_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(intra_band_contiguous_cc_info_list_r12, bref, 1, 5));

  return SRSASN_SUCCESS;
}
void ca_mimo_params_dl_v1270_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("intraBandContiguousCC-InfoList-r12");
  for (const auto& e1 : intra_band_contiguous_cc_info_list_r12) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// BandParameters-v1270 ::= SEQUENCE
SRSASN_CODE band_params_v1270_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, band_params_dl_v1270, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_v1270_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(band_params_dl_v1270, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void band_params_v1270_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("bandParametersDL-v1270");
  for (const auto& e1 : band_params_dl_v1270) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// BandCombinationParameters-v1270 ::= SEQUENCE
SRSASN_CODE band_combination_params_v1270_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_param_list_v1270_present, 1));

  if (band_param_list_v1270_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_param_list_v1270, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_v1270_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_param_list_v1270_present, 1));

  if (band_param_list_v1270_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_param_list_v1270, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void band_combination_params_v1270_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_param_list_v1270_present) {
    j.start_array("bandParameterList-v1270");
    for (const auto& e1 : band_param_list_v1270) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// MIMO-BeamformedCapabilities-r13 ::= SEQUENCE
SRSASN_CODE mimo_bf_cap_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(n_max_list_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, k_max_r13, (uint8_t)1u, (uint8_t)8u));
  if (n_max_list_r13_present) {
    HANDLE_CODE(n_max_list_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_bf_cap_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(n_max_list_r13_present, 1));

  HANDLE_CODE(unpack_integer(k_max_r13, bref, (uint8_t)1u, (uint8_t)8u));
  if (n_max_list_r13_present) {
    HANDLE_CODE(n_max_list_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mimo_bf_cap_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("k-Max-r13", k_max_r13);
  if (n_max_list_r13_present) {
    j.write_str("n-MaxList-r13", n_max_list_r13.to_string());
  }
  j.end_obj();
}

// MIMO-NonPrecodedCapabilities-r13 ::= SEQUENCE
SRSASN_CODE mimo_non_precoded_cap_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cfg1_r13_present, 1));
  HANDLE_CODE(bref.pack(cfg2_r13_present, 1));
  HANDLE_CODE(bref.pack(cfg3_r13_present, 1));
  HANDLE_CODE(bref.pack(cfg4_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_non_precoded_cap_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cfg1_r13_present, 1));
  HANDLE_CODE(bref.unpack(cfg2_r13_present, 1));
  HANDLE_CODE(bref.unpack(cfg3_r13_present, 1));
  HANDLE_CODE(bref.unpack(cfg4_r13_present, 1));

  return SRSASN_SUCCESS;
}
void mimo_non_precoded_cap_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cfg1_r13_present) {
    j.write_str("config1-r13", "supported");
  }
  if (cfg2_r13_present) {
    j.write_str("config2-r13", "supported");
  }
  if (cfg3_r13_present) {
    j.write_str("config3-r13", "supported");
  }
  if (cfg4_r13_present) {
    j.write_str("config4-r13", "supported");
  }
  j.end_obj();
}

// MIMO-CA-ParametersPerBoBCPerTM-r13 ::= SEQUENCE
SRSASN_CODE mimo_ca_params_per_bo_bc_per_tm_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_precoded_r13_present, 1));
  HANDLE_CODE(bref.pack(bf_r13_present, 1));
  HANDLE_CODE(bref.pack(dmrs_enhance_r13_present, 1));

  if (non_precoded_r13_present) {
    HANDLE_CODE(non_precoded_r13.pack(bref));
  }
  if (bf_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, bf_r13, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ca_params_per_bo_bc_per_tm_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_precoded_r13_present, 1));
  HANDLE_CODE(bref.unpack(bf_r13_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_enhance_r13_present, 1));

  if (non_precoded_r13_present) {
    HANDLE_CODE(non_precoded_r13.unpack(bref));
  }
  if (bf_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(bf_r13, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void mimo_ca_params_per_bo_bc_per_tm_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (non_precoded_r13_present) {
    j.write_fieldname("nonPrecoded-r13");
    non_precoded_r13.to_json(j);
  }
  if (bf_r13_present) {
    j.start_array("beamformed-r13");
    for (const auto& e1 : bf_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (dmrs_enhance_r13_present) {
    j.write_str("dmrs-Enhancements-r13", "different");
  }
  j.end_obj();
}

// MIMO-CA-ParametersPerBoBC-r13 ::= SEQUENCE
SRSASN_CODE mimo_ca_params_per_bo_bc_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(params_tm9_r13_present, 1));
  HANDLE_CODE(bref.pack(params_tm10_r13_present, 1));

  if (params_tm9_r13_present) {
    HANDLE_CODE(params_tm9_r13.pack(bref));
  }
  if (params_tm10_r13_present) {
    HANDLE_CODE(params_tm10_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ca_params_per_bo_bc_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(params_tm9_r13_present, 1));
  HANDLE_CODE(bref.unpack(params_tm10_r13_present, 1));

  if (params_tm9_r13_present) {
    HANDLE_CODE(params_tm9_r13.unpack(bref));
  }
  if (params_tm10_r13_present) {
    HANDLE_CODE(params_tm10_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mimo_ca_params_per_bo_bc_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (params_tm9_r13_present) {
    j.write_fieldname("parametersTM9-r13");
    params_tm9_r13.to_json(j);
  }
  if (params_tm10_r13_present) {
    j.write_fieldname("parametersTM10-r13");
    params_tm10_r13.to_json(j);
  }
  j.end_obj();
}

// BandParameters-v1320 ::= SEQUENCE
SRSASN_CODE band_params_v1320_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(band_params_dl_v1320.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_v1320_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(band_params_dl_v1320.unpack(bref));

  return SRSASN_SUCCESS;
}
void band_params_v1320_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("bandParametersDL-v1320");
  band_params_dl_v1320.to_json(j);
  j.end_obj();
}

// BandCombinationParameters-v1320 ::= SEQUENCE
SRSASN_CODE band_combination_params_v1320_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_param_list_v1320_present, 1));
  HANDLE_CODE(bref.pack(add_rx_tx_performance_req_r13_present, 1));

  if (band_param_list_v1320_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_param_list_v1320, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_v1320_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_param_list_v1320_present, 1));
  HANDLE_CODE(bref.unpack(add_rx_tx_performance_req_r13_present, 1));

  if (band_param_list_v1320_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_param_list_v1320, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void band_combination_params_v1320_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_param_list_v1320_present) {
    j.start_array("bandParameterList-v1320");
    for (const auto& e1 : band_param_list_v1320) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (add_rx_tx_performance_req_r13_present) {
    j.write_str("additionalRx-Tx-PerformanceReq-r13", "supported");
  }
  j.end_obj();
}

// BandParameters-v1380 ::= SEQUENCE
SRSASN_CODE band_params_v1380_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tx_ant_switch_dl_r13_present, 1));
  HANDLE_CODE(bref.pack(tx_ant_switch_ul_r13_present, 1));

  if (tx_ant_switch_dl_r13_present) {
    HANDLE_CODE(pack_integer(bref, tx_ant_switch_dl_r13, (uint8_t)1u, (uint8_t)32u));
  }
  if (tx_ant_switch_ul_r13_present) {
    HANDLE_CODE(pack_integer(bref, tx_ant_switch_ul_r13, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_v1380_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tx_ant_switch_dl_r13_present, 1));
  HANDLE_CODE(bref.unpack(tx_ant_switch_ul_r13_present, 1));

  if (tx_ant_switch_dl_r13_present) {
    HANDLE_CODE(unpack_integer(tx_ant_switch_dl_r13, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (tx_ant_switch_ul_r13_present) {
    HANDLE_CODE(unpack_integer(tx_ant_switch_ul_r13, bref, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
void band_params_v1380_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tx_ant_switch_dl_r13_present) {
    j.write_int("txAntennaSwitchDL-r13", tx_ant_switch_dl_r13);
  }
  if (tx_ant_switch_ul_r13_present) {
    j.write_int("txAntennaSwitchUL-r13", tx_ant_switch_ul_r13);
  }
  j.end_obj();
}

// BandCombinationParameters-v1380 ::= SEQUENCE
SRSASN_CODE band_combination_params_v1380_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_param_list_v1380_present, 1));

  if (band_param_list_v1380_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_param_list_v1380, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_v1380_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_param_list_v1380_present, 1));

  if (band_param_list_v1380_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_param_list_v1380, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void band_combination_params_v1380_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_param_list_v1380_present) {
    j.start_array("bandParameterList-v1380");
    for (const auto& e1 : band_param_list_v1380) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// MIMO-CA-ParametersPerBoBCPerTM-v1430 ::= SEQUENCE
SRSASN_CODE mimo_ca_params_per_bo_bc_per_tm_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_report_np_r14_present, 1));
  HANDLE_CODE(bref.pack(csi_report_advanced_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ca_params_per_bo_bc_per_tm_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_report_np_r14_present, 1));
  HANDLE_CODE(bref.unpack(csi_report_advanced_r14_present, 1));

  return SRSASN_SUCCESS;
}
void mimo_ca_params_per_bo_bc_per_tm_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_report_np_r14_present) {
    j.write_str("csi-ReportingNP-r14", "different");
  }
  if (csi_report_advanced_r14_present) {
    j.write_str("csi-ReportingAdvanced-r14", "different");
  }
  j.end_obj();
}

// MIMO-CA-ParametersPerBoBC-v1430 ::= SEQUENCE
SRSASN_CODE mimo_ca_params_per_bo_bc_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(params_tm9_v1430_present, 1));
  HANDLE_CODE(bref.pack(params_tm10_v1430_present, 1));

  if (params_tm9_v1430_present) {
    HANDLE_CODE(params_tm9_v1430.pack(bref));
  }
  if (params_tm10_v1430_present) {
    HANDLE_CODE(params_tm10_v1430.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ca_params_per_bo_bc_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(params_tm9_v1430_present, 1));
  HANDLE_CODE(bref.unpack(params_tm10_v1430_present, 1));

  if (params_tm9_v1430_present) {
    HANDLE_CODE(params_tm9_v1430.unpack(bref));
  }
  if (params_tm10_v1430_present) {
    HANDLE_CODE(params_tm10_v1430.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mimo_ca_params_per_bo_bc_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (params_tm9_v1430_present) {
    j.write_fieldname("parametersTM9-v1430");
    params_tm9_v1430.to_json(j);
  }
  if (params_tm10_v1430_present) {
    j.write_fieldname("parametersTM10-v1430");
    params_tm10_v1430.to_json(j);
  }
  j.end_obj();
}

// SRS-CapabilityPerBandPair-r14 ::= SEQUENCE
SRSASN_CODE srs_cap_per_band_pair_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(retuning_info.rf_retuning_time_dl_r14_present, 1));
  HANDLE_CODE(bref.pack(retuning_info.rf_retuning_time_ul_r14_present, 1));
  if (retuning_info.rf_retuning_time_dl_r14_present) {
    HANDLE_CODE(retuning_info.rf_retuning_time_dl_r14.pack(bref));
  }
  if (retuning_info.rf_retuning_time_ul_r14_present) {
    HANDLE_CODE(retuning_info.rf_retuning_time_ul_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_cap_per_band_pair_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(retuning_info.rf_retuning_time_dl_r14_present, 1));
  HANDLE_CODE(bref.unpack(retuning_info.rf_retuning_time_ul_r14_present, 1));
  if (retuning_info.rf_retuning_time_dl_r14_present) {
    HANDLE_CODE(retuning_info.rf_retuning_time_dl_r14.unpack(bref));
  }
  if (retuning_info.rf_retuning_time_ul_r14_present) {
    HANDLE_CODE(retuning_info.rf_retuning_time_ul_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void srs_cap_per_band_pair_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("retuningInfo");
  j.start_obj();
  if (retuning_info.rf_retuning_time_dl_r14_present) {
    j.write_str("rf-RetuningTimeDL-r14", retuning_info.rf_retuning_time_dl_r14.to_string());
  }
  if (retuning_info.rf_retuning_time_ul_r14_present) {
    j.write_str("rf-RetuningTimeUL-r14", retuning_info.rf_retuning_time_ul_r14.to_string());
  }
  j.end_obj();
  j.end_obj();
}

const char* srs_cap_per_band_pair_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_opts::to_string() const
{
  static const char* options[] = {"n0",
                                  "n0dot5",
                                  "n1",
                                  "n1dot5",
                                  "n2",
                                  "n2dot5",
                                  "n3",
                                  "n3dot5",
                                  "n4",
                                  "n4dot5",
                                  "n5",
                                  "n5dot5",
                                  "n6",
                                  "n6dot5",
                                  "n7",
                                  "spare1"};
  return convert_enum_idx(
      options, 16, value, "srs_cap_per_band_pair_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_e_");
}
float srs_cap_per_band_pair_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0};
  return map_enum_number(
      options, 15, value, "srs_cap_per_band_pair_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_e_");
}
const char* srs_cap_per_band_pair_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_opts::to_number_string() const
{
  static const char* options[] = {
      "0", "0.5", "1", "1.5", "2", "2.5", "3", "3.5", "4", "4.5", "5", "5.5", "6", "6.5", "7"};
  return convert_enum_idx(
      options, 16, value, "srs_cap_per_band_pair_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_e_");
}

const char* srs_cap_per_band_pair_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_opts::to_string() const
{
  static const char* options[] = {"n0",
                                  "n0dot5",
                                  "n1",
                                  "n1dot5",
                                  "n2",
                                  "n2dot5",
                                  "n3",
                                  "n3dot5",
                                  "n4",
                                  "n4dot5",
                                  "n5",
                                  "n5dot5",
                                  "n6",
                                  "n6dot5",
                                  "n7",
                                  "spare1"};
  return convert_enum_idx(
      options, 16, value, "srs_cap_per_band_pair_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_e_");
}
float srs_cap_per_band_pair_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0};
  return map_enum_number(
      options, 15, value, "srs_cap_per_band_pair_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_e_");
}
const char* srs_cap_per_band_pair_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_opts::to_number_string() const
{
  static const char* options[] = {
      "0", "0.5", "1", "1.5", "2", "2.5", "3", "3.5", "4", "4.5", "5", "5.5", "6", "6.5", "7"};
  return convert_enum_idx(
      options, 16, value, "srs_cap_per_band_pair_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_e_");
}

// UL-256QAM-perCC-Info-r14 ::= SEQUENCE
SRSASN_CODE ul_minus256_qam_per_cc_info_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_minus256_qam_per_cc_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_minus256_qam_per_cc_info_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_minus256_qam_per_cc_r14_present, 1));

  return SRSASN_SUCCESS;
}
void ul_minus256_qam_per_cc_info_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_minus256_qam_per_cc_r14_present) {
    j.write_str("ul-256QAM-perCC-r14", "supported");
  }
  j.end_obj();
}

// BandParameters-v1430 ::= SEQUENCE
SRSASN_CODE band_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_params_dl_v1430_present, 1));
  HANDLE_CODE(bref.pack(ul_minus256_qam_r14_present, 1));
  HANDLE_CODE(bref.pack(ul_minus256_qam_per_cc_info_list_r14_present, 1));
  HANDLE_CODE(bref.pack(srs_cap_per_band_pair_list_r14_present, 1));

  if (band_params_dl_v1430_present) {
    HANDLE_CODE(band_params_dl_v1430.pack(bref));
  }
  if (ul_minus256_qam_per_cc_info_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_minus256_qam_per_cc_info_list_r14, 2, 32));
  }
  if (srs_cap_per_band_pair_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_cap_per_band_pair_list_r14, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_params_dl_v1430_present, 1));
  HANDLE_CODE(bref.unpack(ul_minus256_qam_r14_present, 1));
  HANDLE_CODE(bref.unpack(ul_minus256_qam_per_cc_info_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(srs_cap_per_band_pair_list_r14_present, 1));

  if (band_params_dl_v1430_present) {
    HANDLE_CODE(band_params_dl_v1430.unpack(bref));
  }
  if (ul_minus256_qam_per_cc_info_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_minus256_qam_per_cc_info_list_r14, bref, 2, 32));
  }
  if (srs_cap_per_band_pair_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_cap_per_band_pair_list_r14, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void band_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_params_dl_v1430_present) {
    j.write_fieldname("bandParametersDL-v1430");
    band_params_dl_v1430.to_json(j);
  }
  if (ul_minus256_qam_r14_present) {
    j.write_str("ul-256QAM-r14", "supported");
  }
  if (ul_minus256_qam_per_cc_info_list_r14_present) {
    j.start_array("ul-256QAM-perCC-InfoList-r14");
    for (const auto& e1 : ul_minus256_qam_per_cc_info_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srs_cap_per_band_pair_list_r14_present) {
    j.start_array("srs-CapabilityPerBandPairList-r14");
    for (const auto& e1 : srs_cap_per_band_pair_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// BandCombinationParameters-v1430 ::= SEQUENCE
SRSASN_CODE band_combination_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_param_list_v1430_present, 1));
  HANDLE_CODE(bref.pack(v2x_supported_tx_band_comb_list_per_bc_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_supported_rx_band_comb_list_per_bc_r14_present, 1));

  if (band_param_list_v1430_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_param_list_v1430, 1, 64));
  }
  if (v2x_supported_tx_band_comb_list_per_bc_r14_present) {
    HANDLE_CODE(v2x_supported_tx_band_comb_list_per_bc_r14.pack(bref));
  }
  if (v2x_supported_rx_band_comb_list_per_bc_r14_present) {
    HANDLE_CODE(v2x_supported_rx_band_comb_list_per_bc_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_param_list_v1430_present, 1));
  HANDLE_CODE(bref.unpack(v2x_supported_tx_band_comb_list_per_bc_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_supported_rx_band_comb_list_per_bc_r14_present, 1));

  if (band_param_list_v1430_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_param_list_v1430, bref, 1, 64));
  }
  if (v2x_supported_tx_band_comb_list_per_bc_r14_present) {
    HANDLE_CODE(v2x_supported_tx_band_comb_list_per_bc_r14.unpack(bref));
  }
  if (v2x_supported_rx_band_comb_list_per_bc_r14_present) {
    HANDLE_CODE(v2x_supported_rx_band_comb_list_per_bc_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_combination_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_param_list_v1430_present) {
    j.start_array("bandParameterList-v1430");
    for (const auto& e1 : band_param_list_v1430) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (v2x_supported_tx_band_comb_list_per_bc_r14_present) {
    j.write_str("v2x-SupportedTxBandCombListPerBC-r14", v2x_supported_tx_band_comb_list_per_bc_r14.to_string());
  }
  if (v2x_supported_rx_band_comb_list_per_bc_r14_present) {
    j.write_str("v2x-SupportedRxBandCombListPerBC-r14", v2x_supported_rx_band_comb_list_per_bc_r14.to_string());
  }
  j.end_obj();
}

// MUST-Parameters-r14 ::= SEQUENCE
SRSASN_CODE must_params_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(must_tm234_up_to2_tx_r14_present, 1));
  HANDLE_CODE(bref.pack(must_tm89_up_to_one_interfering_layer_r14_present, 1));
  HANDLE_CODE(bref.pack(must_tm10_up_to_one_interfering_layer_r14_present, 1));
  HANDLE_CODE(bref.pack(must_tm89_up_to_three_interfering_layers_r14_present, 1));
  HANDLE_CODE(bref.pack(must_tm10_up_to_three_interfering_layers_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE must_params_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(must_tm234_up_to2_tx_r14_present, 1));
  HANDLE_CODE(bref.unpack(must_tm89_up_to_one_interfering_layer_r14_present, 1));
  HANDLE_CODE(bref.unpack(must_tm10_up_to_one_interfering_layer_r14_present, 1));
  HANDLE_CODE(bref.unpack(must_tm89_up_to_three_interfering_layers_r14_present, 1));
  HANDLE_CODE(bref.unpack(must_tm10_up_to_three_interfering_layers_r14_present, 1));

  return SRSASN_SUCCESS;
}
void must_params_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (must_tm234_up_to2_tx_r14_present) {
    j.write_str("must-TM234-UpTo2Tx-r14", "supported");
  }
  if (must_tm89_up_to_one_interfering_layer_r14_present) {
    j.write_str("must-TM89-UpToOneInterferingLayer-r14", "supported");
  }
  if (must_tm10_up_to_one_interfering_layer_r14_present) {
    j.write_str("must-TM10-UpToOneInterferingLayer-r14", "supported");
  }
  if (must_tm89_up_to_three_interfering_layers_r14_present) {
    j.write_str("must-TM89-UpToThreeInterferingLayers-r14", "supported");
  }
  if (must_tm10_up_to_three_interfering_layers_r14_present) {
    j.write_str("must-TM10-UpToThreeInterferingLayers-r14", "supported");
  }
  j.end_obj();
}

// BandParameters-v1450 ::= SEQUENCE
SRSASN_CODE band_params_v1450_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(must_cap_per_band_r14_present, 1));

  if (must_cap_per_band_r14_present) {
    HANDLE_CODE(must_cap_per_band_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_v1450_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(must_cap_per_band_r14_present, 1));

  if (must_cap_per_band_r14_present) {
    HANDLE_CODE(must_cap_per_band_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_params_v1450_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (must_cap_per_band_r14_present) {
    j.write_fieldname("must-CapabilityPerBand-r14");
    must_cap_per_band_r14.to_json(j);
  }
  j.end_obj();
}

// BandCombinationParameters-v1450 ::= SEQUENCE
SRSASN_CODE band_combination_params_v1450_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_param_list_v1450_present, 1));

  if (band_param_list_v1450_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_param_list_v1450, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_v1450_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_param_list_v1450_present, 1));

  if (band_param_list_v1450_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_param_list_v1450, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void band_combination_params_v1450_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_param_list_v1450_present) {
    j.start_array("bandParameterList-v1450");
    for (const auto& e1 : band_param_list_v1450) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// MIMO-CA-ParametersPerBoBCPerTM-v1470 ::= SEQUENCE
SRSASN_CODE mimo_ca_params_per_bo_bc_per_tm_v1470_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_report_advanced_max_ports_r14_present, 1));

  if (csi_report_advanced_max_ports_r14_present) {
    HANDLE_CODE(csi_report_advanced_max_ports_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ca_params_per_bo_bc_per_tm_v1470_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_report_advanced_max_ports_r14_present, 1));

  if (csi_report_advanced_max_ports_r14_present) {
    HANDLE_CODE(csi_report_advanced_max_ports_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mimo_ca_params_per_bo_bc_per_tm_v1470_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_report_advanced_max_ports_r14_present) {
    j.write_str("csi-ReportingAdvancedMaxPorts-r14", csi_report_advanced_max_ports_r14.to_string());
  }
  j.end_obj();
}

const char* mimo_ca_params_per_bo_bc_per_tm_v1470_s::csi_report_advanced_max_ports_r14_opts::to_string() const
{
  static const char* options[] = {"n8", "n12", "n16", "n20", "n24", "n28"};
  return convert_enum_idx(
      options, 6, value, "mimo_ca_params_per_bo_bc_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_");
}
uint8_t mimo_ca_params_per_bo_bc_per_tm_v1470_s::csi_report_advanced_max_ports_r14_opts::to_number() const
{
  static const uint8_t options[] = {8, 12, 16, 20, 24, 28};
  return map_enum_number(
      options, 6, value, "mimo_ca_params_per_bo_bc_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_");
}

// MIMO-CA-ParametersPerBoBC-v1470 ::= SEQUENCE
SRSASN_CODE mimo_ca_params_per_bo_bc_v1470_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(params_tm9_v1470.pack(bref));
  HANDLE_CODE(params_tm10_v1470.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ca_params_per_bo_bc_v1470_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(params_tm9_v1470.unpack(bref));
  HANDLE_CODE(params_tm10_v1470.unpack(bref));

  return SRSASN_SUCCESS;
}
void mimo_ca_params_per_bo_bc_v1470_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("parametersTM9-v1470");
  params_tm9_v1470.to_json(j);
  j.write_fieldname("parametersTM10-v1470");
  params_tm10_v1470.to_json(j);
  j.end_obj();
}

// BandParameters-v1470 ::= SEQUENCE
SRSASN_CODE band_params_v1470_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_params_dl_v1470_present, 1));

  if (band_params_dl_v1470_present) {
    HANDLE_CODE(band_params_dl_v1470.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_v1470_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_params_dl_v1470_present, 1));

  if (band_params_dl_v1470_present) {
    HANDLE_CODE(band_params_dl_v1470.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_params_v1470_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_params_dl_v1470_present) {
    j.write_fieldname("bandParametersDL-v1470");
    band_params_dl_v1470.to_json(j);
  }
  j.end_obj();
}

// BandCombinationParameters-v1470 ::= SEQUENCE
SRSASN_CODE band_combination_params_v1470_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_param_list_v1470_present, 1));
  HANDLE_CODE(bref.pack(srs_max_simul_ccs_r14_present, 1));

  if (band_param_list_v1470_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_param_list_v1470, 1, 64));
  }
  if (srs_max_simul_ccs_r14_present) {
    HANDLE_CODE(pack_integer(bref, srs_max_simul_ccs_r14, (uint8_t)1u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_v1470_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_param_list_v1470_present, 1));
  HANDLE_CODE(bref.unpack(srs_max_simul_ccs_r14_present, 1));

  if (band_param_list_v1470_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_param_list_v1470, bref, 1, 64));
  }
  if (srs_max_simul_ccs_r14_present) {
    HANDLE_CODE(unpack_integer(srs_max_simul_ccs_r14, bref, (uint8_t)1u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
void band_combination_params_v1470_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_param_list_v1470_present) {
    j.start_array("bandParameterList-v1470");
    for (const auto& e1 : band_param_list_v1470) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srs_max_simul_ccs_r14_present) {
    j.write_int("srs-MaxSimultaneousCCs-r14", srs_max_simul_ccs_r14);
  }
  j.end_obj();
}

// SRS-CapabilityPerBandPair-v14b0 ::= SEQUENCE
SRSASN_CODE srs_cap_per_band_pair_v14b0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_flex_timing_r14_present, 1));
  HANDLE_CODE(bref.pack(srs_harq_ref_cfg_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_cap_per_band_pair_v14b0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_flex_timing_r14_present, 1));
  HANDLE_CODE(bref.unpack(srs_harq_ref_cfg_r14_present, 1));

  return SRSASN_SUCCESS;
}
void srs_cap_per_band_pair_v14b0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_flex_timing_r14_present) {
    j.write_str("srs-FlexibleTiming-r14", "supported");
  }
  if (srs_harq_ref_cfg_r14_present) {
    j.write_str("srs-HARQ-ReferenceConfig-r14", "supported");
  }
  j.end_obj();
}

// BandParameters-v14b0 ::= SEQUENCE
SRSASN_CODE band_params_v14b0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_cap_per_band_pair_list_v14b0_present, 1));

  if (srs_cap_per_band_pair_list_v14b0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_cap_per_band_pair_list_v14b0, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_v14b0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_cap_per_band_pair_list_v14b0_present, 1));

  if (srs_cap_per_band_pair_list_v14b0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_cap_per_band_pair_list_v14b0, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void band_params_v14b0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_cap_per_band_pair_list_v14b0_present) {
    j.start_array("srs-CapabilityPerBandPairList-v14b0");
    for (const auto& e1 : srs_cap_per_band_pair_list_v14b0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// BandCombinationParameters-v14b0 ::= SEQUENCE
SRSASN_CODE band_combination_params_v14b0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_param_list_v14b0_present, 1));

  if (band_param_list_v14b0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_param_list_v14b0, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_v14b0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_param_list_v14b0_present, 1));

  if (band_param_list_v14b0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_param_list_v14b0, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void band_combination_params_v14b0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_param_list_v14b0_present) {
    j.start_array("bandParameterList-v14b0");
    for (const auto& e1 : band_param_list_v14b0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// DL-UL-CCs-r15 ::= SEQUENCE
SRSASN_CODE dl_ul_ccs_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(max_num_dl_ccs_r15_present, 1));
  HANDLE_CODE(bref.pack(max_num_ul_ccs_r15_present, 1));

  if (max_num_dl_ccs_r15_present) {
    HANDLE_CODE(pack_integer(bref, max_num_dl_ccs_r15, (uint8_t)1u, (uint8_t)32u));
  }
  if (max_num_ul_ccs_r15_present) {
    HANDLE_CODE(pack_integer(bref, max_num_ul_ccs_r15, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ul_ccs_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(max_num_dl_ccs_r15_present, 1));
  HANDLE_CODE(bref.unpack(max_num_ul_ccs_r15_present, 1));

  if (max_num_dl_ccs_r15_present) {
    HANDLE_CODE(unpack_integer(max_num_dl_ccs_r15, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (max_num_ul_ccs_r15_present) {
    HANDLE_CODE(unpack_integer(max_num_ul_ccs_r15, bref, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
void dl_ul_ccs_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (max_num_dl_ccs_r15_present) {
    j.write_int("maxNumberDL-CCs-r15", max_num_dl_ccs_r15);
  }
  if (max_num_ul_ccs_r15_present) {
    j.write_int("maxNumberUL-CCs-r15", max_num_ul_ccs_r15);
  }
  j.end_obj();
}

// CA-MIMO-ParametersDL-r15 ::= SEQUENCE
SRSASN_CODE ca_mimo_params_dl_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_mimo_cap_dl_r15_present, 1));
  HANDLE_CODE(bref.pack(four_layer_tm3_tm4_r15_present, 1));
  HANDLE_CODE(bref.pack(intra_band_contiguous_cc_info_list_r15_present, 1));

  if (supported_mimo_cap_dl_r15_present) {
    HANDLE_CODE(supported_mimo_cap_dl_r15.pack(bref));
  }
  if (intra_band_contiguous_cc_info_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, intra_band_contiguous_cc_info_list_r15, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_mimo_params_dl_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_mimo_cap_dl_r15_present, 1));
  HANDLE_CODE(bref.unpack(four_layer_tm3_tm4_r15_present, 1));
  HANDLE_CODE(bref.unpack(intra_band_contiguous_cc_info_list_r15_present, 1));

  if (supported_mimo_cap_dl_r15_present) {
    HANDLE_CODE(supported_mimo_cap_dl_r15.unpack(bref));
  }
  if (intra_band_contiguous_cc_info_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(intra_band_contiguous_cc_info_list_r15, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void ca_mimo_params_dl_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_mimo_cap_dl_r15_present) {
    j.write_str("supportedMIMO-CapabilityDL-r15", supported_mimo_cap_dl_r15.to_string());
  }
  if (four_layer_tm3_tm4_r15_present) {
    j.write_str("fourLayerTM3-TM4-r15", "supported");
  }
  if (intra_band_contiguous_cc_info_list_r15_present) {
    j.start_array("intraBandContiguousCC-InfoList-r15");
    for (const auto& e1 : intra_band_contiguous_cc_info_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// CA-MIMO-ParametersUL-r15 ::= SEQUENCE
SRSASN_CODE ca_mimo_params_ul_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_mimo_cap_ul_r15_present, 1));

  if (supported_mimo_cap_ul_r15_present) {
    HANDLE_CODE(supported_mimo_cap_ul_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_mimo_params_ul_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_mimo_cap_ul_r15_present, 1));

  if (supported_mimo_cap_ul_r15_present) {
    HANDLE_CODE(supported_mimo_cap_ul_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ca_mimo_params_ul_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_mimo_cap_ul_r15_present) {
    j.write_str("supportedMIMO-CapabilityUL-r15", supported_mimo_cap_ul_r15.to_string());
  }
  j.end_obj();
}

// STTI-SupportedCombinations-r15 ::= SEQUENCE
SRSASN_CODE stti_supported_combinations_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(combination_minus22_r15_present, 1));
  HANDLE_CODE(bref.pack(combination_minus77_r15_present, 1));
  HANDLE_CODE(bref.pack(combination_minus27_r15_present, 1));
  HANDLE_CODE(bref.pack(combination_minus22_minus27_r15_present, 1));
  HANDLE_CODE(bref.pack(combination_minus77_minus22_r15_present, 1));
  HANDLE_CODE(bref.pack(combination_minus77_minus27_r15_present, 1));

  if (combination_minus22_r15_present) {
    HANDLE_CODE(combination_minus22_r15.pack(bref));
  }
  if (combination_minus77_r15_present) {
    HANDLE_CODE(combination_minus77_r15.pack(bref));
  }
  if (combination_minus27_r15_present) {
    HANDLE_CODE(combination_minus27_r15.pack(bref));
  }
  if (combination_minus22_minus27_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, combination_minus22_minus27_r15, 1, 2));
  }
  if (combination_minus77_minus22_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, combination_minus77_minus22_r15, 1, 2));
  }
  if (combination_minus77_minus27_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, combination_minus77_minus27_r15, 1, 2));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE stti_supported_combinations_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(combination_minus22_r15_present, 1));
  HANDLE_CODE(bref.unpack(combination_minus77_r15_present, 1));
  HANDLE_CODE(bref.unpack(combination_minus27_r15_present, 1));
  HANDLE_CODE(bref.unpack(combination_minus22_minus27_r15_present, 1));
  HANDLE_CODE(bref.unpack(combination_minus77_minus22_r15_present, 1));
  HANDLE_CODE(bref.unpack(combination_minus77_minus27_r15_present, 1));

  if (combination_minus22_r15_present) {
    HANDLE_CODE(combination_minus22_r15.unpack(bref));
  }
  if (combination_minus77_r15_present) {
    HANDLE_CODE(combination_minus77_r15.unpack(bref));
  }
  if (combination_minus27_r15_present) {
    HANDLE_CODE(combination_minus27_r15.unpack(bref));
  }
  if (combination_minus22_minus27_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(combination_minus22_minus27_r15, bref, 1, 2));
  }
  if (combination_minus77_minus22_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(combination_minus77_minus22_r15, bref, 1, 2));
  }
  if (combination_minus77_minus27_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(combination_minus77_minus27_r15, bref, 1, 2));
  }

  return SRSASN_SUCCESS;
}
void stti_supported_combinations_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (combination_minus22_r15_present) {
    j.write_fieldname("combination-22-r15");
    combination_minus22_r15.to_json(j);
  }
  if (combination_minus77_r15_present) {
    j.write_fieldname("combination-77-r15");
    combination_minus77_r15.to_json(j);
  }
  if (combination_minus27_r15_present) {
    j.write_fieldname("combination-27-r15");
    combination_minus27_r15.to_json(j);
  }
  if (combination_minus22_minus27_r15_present) {
    j.start_array("combination-22-27-r15");
    for (const auto& e1 : combination_minus22_minus27_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (combination_minus77_minus22_r15_present) {
    j.start_array("combination-77-22-r15");
    for (const auto& e1 : combination_minus77_minus22_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (combination_minus77_minus27_r15_present) {
    j.start_array("combination-77-27-r15");
    for (const auto& e1 : combination_minus77_minus27_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// STTI-SPT-BandParameters-r15 ::= SEQUENCE
SRSASN_CODE stti_spt_band_params_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dl_minus1024_qam_slot_r15_present, 1));
  HANDLE_CODE(bref.pack(dl_minus1024_qam_subslot_ta_minus1_r15_present, 1));
  HANDLE_CODE(bref.pack(dl_minus1024_qam_subslot_ta_minus2_r15_present, 1));
  HANDLE_CODE(bref.pack(simul_tx_different_tx_dur_r15_present, 1));
  HANDLE_CODE(bref.pack(stti_ca_mimo_params_dl_r15_present, 1));
  HANDLE_CODE(bref.pack(stti_fd_mimo_coexistence_present, 1));
  HANDLE_CODE(bref.pack(stti_mimo_ca_params_per_bo_bcs_r15_present, 1));
  HANDLE_CODE(bref.pack(stti_mimo_ca_params_per_bo_bcs_v1530_present, 1));
  HANDLE_CODE(bref.pack(stti_supported_combinations_r15_present, 1));
  HANDLE_CODE(bref.pack(stti_supported_csi_proc_r15_present, 1));
  HANDLE_CODE(bref.pack(ul_minus256_qam_slot_r15_present, 1));
  HANDLE_CODE(bref.pack(ul_minus256_qam_subslot_r15_present, 1));

  if (stti_ca_mimo_params_dl_r15_present) {
    HANDLE_CODE(stti_ca_mimo_params_dl_r15.pack(bref));
  }
  HANDLE_CODE(stti_ca_mimo_params_ul_r15.pack(bref));
  if (stti_mimo_ca_params_per_bo_bcs_r15_present) {
    HANDLE_CODE(stti_mimo_ca_params_per_bo_bcs_r15.pack(bref));
  }
  if (stti_mimo_ca_params_per_bo_bcs_v1530_present) {
    HANDLE_CODE(stti_mimo_ca_params_per_bo_bcs_v1530.pack(bref));
  }
  if (stti_supported_combinations_r15_present) {
    HANDLE_CODE(stti_supported_combinations_r15.pack(bref));
  }
  if (stti_supported_csi_proc_r15_present) {
    HANDLE_CODE(stti_supported_csi_proc_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE stti_spt_band_params_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dl_minus1024_qam_slot_r15_present, 1));
  HANDLE_CODE(bref.unpack(dl_minus1024_qam_subslot_ta_minus1_r15_present, 1));
  HANDLE_CODE(bref.unpack(dl_minus1024_qam_subslot_ta_minus2_r15_present, 1));
  HANDLE_CODE(bref.unpack(simul_tx_different_tx_dur_r15_present, 1));
  HANDLE_CODE(bref.unpack(stti_ca_mimo_params_dl_r15_present, 1));
  HANDLE_CODE(bref.unpack(stti_fd_mimo_coexistence_present, 1));
  HANDLE_CODE(bref.unpack(stti_mimo_ca_params_per_bo_bcs_r15_present, 1));
  HANDLE_CODE(bref.unpack(stti_mimo_ca_params_per_bo_bcs_v1530_present, 1));
  HANDLE_CODE(bref.unpack(stti_supported_combinations_r15_present, 1));
  HANDLE_CODE(bref.unpack(stti_supported_csi_proc_r15_present, 1));
  HANDLE_CODE(bref.unpack(ul_minus256_qam_slot_r15_present, 1));
  HANDLE_CODE(bref.unpack(ul_minus256_qam_subslot_r15_present, 1));

  if (stti_ca_mimo_params_dl_r15_present) {
    HANDLE_CODE(stti_ca_mimo_params_dl_r15.unpack(bref));
  }
  HANDLE_CODE(stti_ca_mimo_params_ul_r15.unpack(bref));
  if (stti_mimo_ca_params_per_bo_bcs_r15_present) {
    HANDLE_CODE(stti_mimo_ca_params_per_bo_bcs_r15.unpack(bref));
  }
  if (stti_mimo_ca_params_per_bo_bcs_v1530_present) {
    HANDLE_CODE(stti_mimo_ca_params_per_bo_bcs_v1530.unpack(bref));
  }
  if (stti_supported_combinations_r15_present) {
    HANDLE_CODE(stti_supported_combinations_r15.unpack(bref));
  }
  if (stti_supported_csi_proc_r15_present) {
    HANDLE_CODE(stti_supported_csi_proc_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void stti_spt_band_params_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_minus1024_qam_slot_r15_present) {
    j.write_str("dl-1024QAM-Slot-r15", "supported");
  }
  if (dl_minus1024_qam_subslot_ta_minus1_r15_present) {
    j.write_str("dl-1024QAM-SubslotTA-1-r15", "supported");
  }
  if (dl_minus1024_qam_subslot_ta_minus2_r15_present) {
    j.write_str("dl-1024QAM-SubslotTA-2-r15", "supported");
  }
  if (simul_tx_different_tx_dur_r15_present) {
    j.write_str("simultaneousTx-differentTx-duration-r15", "supported");
  }
  if (stti_ca_mimo_params_dl_r15_present) {
    j.write_fieldname("sTTI-CA-MIMO-ParametersDL-r15");
    stti_ca_mimo_params_dl_r15.to_json(j);
  }
  j.write_fieldname("sTTI-CA-MIMO-ParametersUL-r15");
  stti_ca_mimo_params_ul_r15.to_json(j);
  if (stti_fd_mimo_coexistence_present) {
    j.write_str("sTTI-FD-MIMO-Coexistence", "supported");
  }
  if (stti_mimo_ca_params_per_bo_bcs_r15_present) {
    j.write_fieldname("sTTI-MIMO-CA-ParametersPerBoBCs-r15");
    stti_mimo_ca_params_per_bo_bcs_r15.to_json(j);
  }
  if (stti_mimo_ca_params_per_bo_bcs_v1530_present) {
    j.write_fieldname("sTTI-MIMO-CA-ParametersPerBoBCs-v1530");
    stti_mimo_ca_params_per_bo_bcs_v1530.to_json(j);
  }
  if (stti_supported_combinations_r15_present) {
    j.write_fieldname("sTTI-SupportedCombinations-r15");
    stti_supported_combinations_r15.to_json(j);
  }
  if (stti_supported_csi_proc_r15_present) {
    j.write_str("sTTI-SupportedCSI-Proc-r15", stti_supported_csi_proc_r15.to_string());
  }
  if (ul_minus256_qam_slot_r15_present) {
    j.write_str("ul-256QAM-Slot-r15", "supported");
  }
  if (ul_minus256_qam_subslot_r15_present) {
    j.write_str("ul-256QAM-Subslot-r15", "supported");
  }
  j.end_obj();
}

const char* stti_spt_band_params_r15_s::stti_supported_csi_proc_r15_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n4"};
  return convert_enum_idx(options, 3, value, "stti_spt_band_params_r15_s::stti_supported_csi_proc_r15_e_");
}
uint8_t stti_spt_band_params_r15_s::stti_supported_csi_proc_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 4};
  return map_enum_number(options, 3, value, "stti_spt_band_params_r15_s::stti_supported_csi_proc_r15_e_");
}

// BandParameters-v1530 ::= SEQUENCE
SRSASN_CODE band_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_tx_ant_sel_srs_minus1_t4_r_r15_present, 1));
  HANDLE_CODE(bref.pack(ue_tx_ant_sel_srs_minus2_t4_r_minus2_pairs_r15_present, 1));
  HANDLE_CODE(bref.pack(ue_tx_ant_sel_srs_minus2_t4_r_minus3_pairs_r15_present, 1));
  HANDLE_CODE(bref.pack(dl_minus1024_qam_r15_present, 1));
  HANDLE_CODE(bref.pack(qcl_type_c_operation_r15_present, 1));
  HANDLE_CODE(bref.pack(qcl_cri_based_csi_report_r15_present, 1));
  HANDLE_CODE(bref.pack(stti_spt_band_params_r15_present, 1));

  if (stti_spt_band_params_r15_present) {
    HANDLE_CODE(stti_spt_band_params_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_tx_ant_sel_srs_minus1_t4_r_r15_present, 1));
  HANDLE_CODE(bref.unpack(ue_tx_ant_sel_srs_minus2_t4_r_minus2_pairs_r15_present, 1));
  HANDLE_CODE(bref.unpack(ue_tx_ant_sel_srs_minus2_t4_r_minus3_pairs_r15_present, 1));
  HANDLE_CODE(bref.unpack(dl_minus1024_qam_r15_present, 1));
  HANDLE_CODE(bref.unpack(qcl_type_c_operation_r15_present, 1));
  HANDLE_CODE(bref.unpack(qcl_cri_based_csi_report_r15_present, 1));
  HANDLE_CODE(bref.unpack(stti_spt_band_params_r15_present, 1));

  if (stti_spt_band_params_r15_present) {
    HANDLE_CODE(stti_spt_band_params_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_tx_ant_sel_srs_minus1_t4_r_r15_present) {
    j.write_str("ue-TxAntennaSelection-SRS-1T4R-r15", "supported");
  }
  if (ue_tx_ant_sel_srs_minus2_t4_r_minus2_pairs_r15_present) {
    j.write_str("ue-TxAntennaSelection-SRS-2T4R-2Pairs-r15", "supported");
  }
  if (ue_tx_ant_sel_srs_minus2_t4_r_minus3_pairs_r15_present) {
    j.write_str("ue-TxAntennaSelection-SRS-2T4R-3Pairs-r15", "supported");
  }
  if (dl_minus1024_qam_r15_present) {
    j.write_str("dl-1024QAM-r15", "supported");
  }
  if (qcl_type_c_operation_r15_present) {
    j.write_str("qcl-TypeC-Operation-r15", "supported");
  }
  if (qcl_cri_based_csi_report_r15_present) {
    j.write_str("qcl-CRI-BasedCSI-Reporting-r15", "supported");
  }
  if (stti_spt_band_params_r15_present) {
    j.write_fieldname("stti-SPT-BandParameters-r15");
    stti_spt_band_params_r15.to_json(j);
  }
  j.end_obj();
}

// SPT-Parameters-r15 ::= SEQUENCE
SRSASN_CODE spt_params_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(frame_structure_type_spt_r15_present, 1));
  HANDLE_CODE(bref.pack(max_num_ccs_spt_r15_present, 1));

  if (frame_structure_type_spt_r15_present) {
    HANDLE_CODE(frame_structure_type_spt_r15.pack(bref));
  }
  if (max_num_ccs_spt_r15_present) {
    HANDLE_CODE(pack_integer(bref, max_num_ccs_spt_r15, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE spt_params_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(frame_structure_type_spt_r15_present, 1));
  HANDLE_CODE(bref.unpack(max_num_ccs_spt_r15_present, 1));

  if (frame_structure_type_spt_r15_present) {
    HANDLE_CODE(frame_structure_type_spt_r15.unpack(bref));
  }
  if (max_num_ccs_spt_r15_present) {
    HANDLE_CODE(unpack_integer(max_num_ccs_spt_r15, bref, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
void spt_params_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (frame_structure_type_spt_r15_present) {
    j.write_str("frameStructureType-SPT-r15", frame_structure_type_spt_r15.to_string());
  }
  if (max_num_ccs_spt_r15_present) {
    j.write_int("maxNumberCCs-SPT-r15", max_num_ccs_spt_r15);
  }
  j.end_obj();
}

// BandCombinationParameters-v1530 ::= SEQUENCE
SRSASN_CODE band_combination_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_param_list_v1530_present, 1));
  HANDLE_CODE(bref.pack(spt_params_r15_present, 1));

  if (band_param_list_v1530_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_param_list_v1530, 1, 64));
  }
  if (spt_params_r15_present) {
    HANDLE_CODE(spt_params_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_param_list_v1530_present, 1));
  HANDLE_CODE(bref.unpack(spt_params_r15_present, 1));

  if (band_param_list_v1530_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_param_list_v1530, bref, 1, 64));
  }
  if (spt_params_r15_present) {
    HANDLE_CODE(spt_params_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_combination_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_param_list_v1530_present) {
    j.start_array("bandParameterList-v1530");
    for (const auto& e1 : band_param_list_v1530) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (spt_params_r15_present) {
    j.write_fieldname("spt-Parameters-r15");
    spt_params_r15.to_json(j);
  }
  j.end_obj();
}

// BandCombinationParametersExt-r10 ::= SEQUENCE
SRSASN_CODE band_combination_params_ext_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_bw_combination_set_r10_present, 1));

  if (supported_bw_combination_set_r10_present) {
    HANDLE_CODE(supported_bw_combination_set_r10.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_ext_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_bw_combination_set_r10_present, 1));

  if (supported_bw_combination_set_r10_present) {
    HANDLE_CODE(supported_bw_combination_set_r10.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_combination_params_ext_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_bw_combination_set_r10_present) {
    j.write_str("supportedBandwidthCombinationSet-r10", supported_bw_combination_set_r10.to_string());
  }
  j.end_obj();
}

// V2X-BandwidthClass-r14 ::= ENUMERATED
const char* v2x_bw_class_r14_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c", "d", "e", "f", "c1-v1530"};
  return convert_enum_idx(options, 7, value, "v2x_bw_class_r14_e");
}
uint8_t v2x_bw_class_r14_opts::to_number() const
{
  if (value == c1_v1530) {
    return 1;
  }
  invalid_enum_number(value, "v2x_bw_class_r14_e");
  return 0;
}

// BandParametersRxSL-r14 ::= SEQUENCE
SRSASN_CODE band_params_rx_sl_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(v2x_high_reception_r14_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, v2x_bw_class_rx_sl_r14, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_rx_sl_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(v2x_high_reception_r14_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(v2x_bw_class_rx_sl_r14, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void band_params_rx_sl_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("v2x-BandwidthClassRxSL-r14");
  for (const auto& e1 : v2x_bw_class_rx_sl_r14) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  if (v2x_high_reception_r14_present) {
    j.write_str("v2x-HighReception-r14", "supported");
  }
  j.end_obj();
}

// BandParametersTxSL-r14 ::= SEQUENCE
SRSASN_CODE band_params_tx_sl_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(v2x_enb_sched_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_high_pwr_r14_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, v2x_bw_class_tx_sl_r14, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_tx_sl_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(v2x_enb_sched_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_high_pwr_r14_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(v2x_bw_class_tx_sl_r14, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void band_params_tx_sl_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("v2x-BandwidthClassTxSL-r14");
  for (const auto& e1 : v2x_bw_class_tx_sl_r14) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  if (v2x_enb_sched_r14_present) {
    j.write_str("v2x-eNB-Scheduled-r14", "supported");
  }
  if (v2x_high_pwr_r14_present) {
    j.write_str("v2x-HighPower-r14", "supported");
  }
  j.end_obj();
}

// FeatureSetDL-PerCC-r15 ::= SEQUENCE
SRSASN_CODE feature_set_dl_per_cc_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(four_layer_tm3_tm4_r15_present, 1));
  HANDLE_CODE(bref.pack(supported_mimo_cap_dl_mrdc_r15_present, 1));
  HANDLE_CODE(bref.pack(supported_csi_proc_r15_present, 1));

  if (supported_mimo_cap_dl_mrdc_r15_present) {
    HANDLE_CODE(supported_mimo_cap_dl_mrdc_r15.pack(bref));
  }
  if (supported_csi_proc_r15_present) {
    HANDLE_CODE(supported_csi_proc_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_set_dl_per_cc_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(four_layer_tm3_tm4_r15_present, 1));
  HANDLE_CODE(bref.unpack(supported_mimo_cap_dl_mrdc_r15_present, 1));
  HANDLE_CODE(bref.unpack(supported_csi_proc_r15_present, 1));

  if (supported_mimo_cap_dl_mrdc_r15_present) {
    HANDLE_CODE(supported_mimo_cap_dl_mrdc_r15.unpack(bref));
  }
  if (supported_csi_proc_r15_present) {
    HANDLE_CODE(supported_csi_proc_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void feature_set_dl_per_cc_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (four_layer_tm3_tm4_r15_present) {
    j.write_str("fourLayerTM3-TM4-r15", "supported");
  }
  if (supported_mimo_cap_dl_mrdc_r15_present) {
    j.write_str("supportedMIMO-CapabilityDL-MRDC-r15", supported_mimo_cap_dl_mrdc_r15.to_string());
  }
  if (supported_csi_proc_r15_present) {
    j.write_str("supportedCSI-Proc-r15", supported_csi_proc_r15.to_string());
  }
  j.end_obj();
}

const char* feature_set_dl_per_cc_r15_s::supported_csi_proc_r15_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n4"};
  return convert_enum_idx(options, 3, value, "feature_set_dl_per_cc_r15_s::supported_csi_proc_r15_e_");
}
uint8_t feature_set_dl_per_cc_r15_s::supported_csi_proc_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 4};
  return map_enum_number(options, 3, value, "feature_set_dl_per_cc_r15_s::supported_csi_proc_r15_e_");
}

// MIMO-CA-ParametersPerBoBCPerTM-r15 ::= SEQUENCE
SRSASN_CODE mimo_ca_params_per_bo_bc_per_tm_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_precoded_r13_present, 1));
  HANDLE_CODE(bref.pack(bf_r13_present, 1));
  HANDLE_CODE(bref.pack(dmrs_enhance_r13_present, 1));
  HANDLE_CODE(bref.pack(csi_report_np_r14_present, 1));
  HANDLE_CODE(bref.pack(csi_report_advanced_r14_present, 1));

  if (non_precoded_r13_present) {
    HANDLE_CODE(non_precoded_r13.pack(bref));
  }
  if (bf_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, bf_r13, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ca_params_per_bo_bc_per_tm_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_precoded_r13_present, 1));
  HANDLE_CODE(bref.unpack(bf_r13_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_enhance_r13_present, 1));
  HANDLE_CODE(bref.unpack(csi_report_np_r14_present, 1));
  HANDLE_CODE(bref.unpack(csi_report_advanced_r14_present, 1));

  if (non_precoded_r13_present) {
    HANDLE_CODE(non_precoded_r13.unpack(bref));
  }
  if (bf_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(bf_r13, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void mimo_ca_params_per_bo_bc_per_tm_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (non_precoded_r13_present) {
    j.write_fieldname("nonPrecoded-r13");
    non_precoded_r13.to_json(j);
  }
  if (bf_r13_present) {
    j.start_array("beamformed-r13");
    for (const auto& e1 : bf_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (dmrs_enhance_r13_present) {
    j.write_str("dmrs-Enhancements-r13", "different");
  }
  if (csi_report_np_r14_present) {
    j.write_str("csi-ReportingNP-r14", "different");
  }
  if (csi_report_advanced_r14_present) {
    j.write_str("csi-ReportingAdvanced-r14", "different");
  }
  j.end_obj();
}

// MIMO-CA-ParametersPerBoBC-r15 ::= SEQUENCE
SRSASN_CODE mimo_ca_params_per_bo_bc_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(params_tm9_r15_present, 1));
  HANDLE_CODE(bref.pack(params_tm10_r15_present, 1));

  if (params_tm9_r15_present) {
    HANDLE_CODE(params_tm9_r15.pack(bref));
  }
  if (params_tm10_r15_present) {
    HANDLE_CODE(params_tm10_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ca_params_per_bo_bc_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(params_tm9_r15_present, 1));
  HANDLE_CODE(bref.unpack(params_tm10_r15_present, 1));

  if (params_tm9_r15_present) {
    HANDLE_CODE(params_tm9_r15.unpack(bref));
  }
  if (params_tm10_r15_present) {
    HANDLE_CODE(params_tm10_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mimo_ca_params_per_bo_bc_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (params_tm9_r15_present) {
    j.write_fieldname("parametersTM9-r15");
    params_tm9_r15.to_json(j);
  }
  if (params_tm10_r15_present) {
    j.write_fieldname("parametersTM10-r15");
    params_tm10_r15.to_json(j);
  }
  j.end_obj();
}

// FeatureSetDL-r15 ::= SEQUENCE
SRSASN_CODE feature_set_dl_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mimo_ca_params_per_bo_bc_r15_present, 1));

  if (mimo_ca_params_per_bo_bc_r15_present) {
    HANDLE_CODE(mimo_ca_params_per_bo_bc_r15.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, feature_set_per_cc_list_dl_r15, 1, 32, integer_packer<uint8_t>(0, 32)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_set_dl_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mimo_ca_params_per_bo_bc_r15_present, 1));

  if (mimo_ca_params_per_bo_bc_r15_present) {
    HANDLE_CODE(mimo_ca_params_per_bo_bc_r15.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(feature_set_per_cc_list_dl_r15, bref, 1, 32, integer_packer<uint8_t>(0, 32)));

  return SRSASN_SUCCESS;
}
void feature_set_dl_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mimo_ca_params_per_bo_bc_r15_present) {
    j.write_fieldname("mimo-CA-ParametersPerBoBC-r15");
    mimo_ca_params_per_bo_bc_r15.to_json(j);
  }
  j.start_array("featureSetPerCC-ListDL-r15");
  for (const auto& e1 : feature_set_per_cc_list_dl_r15) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// FeatureSetUL-PerCC-r15 ::= SEQUENCE
SRSASN_CODE feature_set_ul_per_cc_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_mimo_cap_ul_r15_present, 1));
  HANDLE_CODE(bref.pack(ul_minus256_qam_r15_present, 1));

  if (supported_mimo_cap_ul_r15_present) {
    HANDLE_CODE(supported_mimo_cap_ul_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_set_ul_per_cc_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_mimo_cap_ul_r15_present, 1));
  HANDLE_CODE(bref.unpack(ul_minus256_qam_r15_present, 1));

  if (supported_mimo_cap_ul_r15_present) {
    HANDLE_CODE(supported_mimo_cap_ul_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void feature_set_ul_per_cc_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_mimo_cap_ul_r15_present) {
    j.write_str("supportedMIMO-CapabilityUL-r15", supported_mimo_cap_ul_r15.to_string());
  }
  if (ul_minus256_qam_r15_present) {
    j.write_str("ul-256QAM-r15", "supported");
  }
  j.end_obj();
}

// FeatureSetUL-r15 ::= SEQUENCE
SRSASN_CODE feature_set_ul_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, feature_set_per_cc_list_ul_r15, 1, 32, integer_packer<uint8_t>(0, 32)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_set_ul_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(feature_set_per_cc_list_ul_r15, bref, 1, 32, integer_packer<uint8_t>(0, 32)));

  return SRSASN_SUCCESS;
}
void feature_set_ul_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("featureSetPerCC-ListUL-r15");
  for (const auto& e1 : feature_set_per_cc_list_ul_r15) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// FeatureSetDL-v1550 ::= SEQUENCE
SRSASN_CODE feature_set_dl_v1550_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dl_minus1024_qam_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_set_dl_v1550_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dl_minus1024_qam_r15_present, 1));

  return SRSASN_SUCCESS;
}
void feature_set_dl_v1550_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_minus1024_qam_r15_present) {
    j.write_str("dl-1024QAM-r15", "supported");
  }
  j.end_obj();
}

// FeatureSetsEUTRA-r15 ::= SEQUENCE
SRSASN_CODE feature_sets_eutra_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(feature_sets_dl_r15_present, 1));
  HANDLE_CODE(bref.pack(feature_sets_dl_per_cc_r15_present, 1));
  HANDLE_CODE(bref.pack(feature_sets_ul_r15_present, 1));
  HANDLE_CODE(bref.pack(feature_sets_ul_per_cc_r15_present, 1));

  if (feature_sets_dl_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, feature_sets_dl_r15, 1, 256));
  }
  if (feature_sets_dl_per_cc_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, feature_sets_dl_per_cc_r15, 1, 32));
  }
  if (feature_sets_ul_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, feature_sets_ul_r15, 1, 256));
  }
  if (feature_sets_ul_per_cc_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, feature_sets_ul_per_cc_r15, 1, 32));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= feature_sets_dl_v1550.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(feature_sets_dl_v1550.is_present(), 1));
      if (feature_sets_dl_v1550.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *feature_sets_dl_v1550, 1, 256));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_sets_eutra_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(feature_sets_dl_r15_present, 1));
  HANDLE_CODE(bref.unpack(feature_sets_dl_per_cc_r15_present, 1));
  HANDLE_CODE(bref.unpack(feature_sets_ul_r15_present, 1));
  HANDLE_CODE(bref.unpack(feature_sets_ul_per_cc_r15_present, 1));

  if (feature_sets_dl_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(feature_sets_dl_r15, bref, 1, 256));
  }
  if (feature_sets_dl_per_cc_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(feature_sets_dl_per_cc_r15, bref, 1, 32));
  }
  if (feature_sets_ul_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(feature_sets_ul_r15, bref, 1, 256));
  }
  if (feature_sets_ul_per_cc_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(feature_sets_ul_per_cc_r15, bref, 1, 32));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool feature_sets_dl_v1550_present;
      HANDLE_CODE(bref.unpack(feature_sets_dl_v1550_present, 1));
      feature_sets_dl_v1550.set_present(feature_sets_dl_v1550_present);
      if (feature_sets_dl_v1550.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*feature_sets_dl_v1550, bref, 1, 256));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void feature_sets_eutra_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (feature_sets_dl_r15_present) {
    j.start_array("featureSetsDL-r15");
    for (const auto& e1 : feature_sets_dl_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (feature_sets_dl_per_cc_r15_present) {
    j.start_array("featureSetsDL-PerCC-r15");
    for (const auto& e1 : feature_sets_dl_per_cc_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (feature_sets_ul_r15_present) {
    j.start_array("featureSetsUL-r15");
    for (const auto& e1 : feature_sets_ul_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (feature_sets_ul_per_cc_r15_present) {
    j.start_array("featureSetsUL-PerCC-r15");
    for (const auto& e1 : feature_sets_ul_per_cc_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (feature_sets_dl_v1550.is_present()) {
      j.start_array("featureSetsDL-v1550");
      for (const auto& e1 : *feature_sets_dl_v1550) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// IRAT-ParametersCDMA2000-1XRTT ::= SEQUENCE
SRSASN_CODE irat_params_cdma2000_minus1_xrtt_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list1_xrtt, 1, 32));
  HANDLE_CODE(tx_cfg1_xrtt.pack(bref));
  HANDLE_CODE(rx_cfg1_xrtt.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_cdma2000_minus1_xrtt_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(supported_band_list1_xrtt, bref, 1, 32));
  HANDLE_CODE(tx_cfg1_xrtt.unpack(bref));
  HANDLE_CODE(rx_cfg1_xrtt.unpack(bref));

  return SRSASN_SUCCESS;
}
void irat_params_cdma2000_minus1_xrtt_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandList1XRTT");
  for (const auto& e1 : supported_band_list1_xrtt) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.write_str("tx-Config1XRTT", tx_cfg1_xrtt.to_string());
  j.write_str("rx-Config1XRTT", rx_cfg1_xrtt.to_string());
  j.end_obj();
}

const char* irat_params_cdma2000_minus1_xrtt_s::tx_cfg1_xrtt_opts::to_string() const
{
  static const char* options[] = {"single", "dual"};
  return convert_enum_idx(options, 2, value, "irat_params_cdma2000_minus1_xrtt_s::tx_cfg1_xrtt_e_");
}

const char* irat_params_cdma2000_minus1_xrtt_s::rx_cfg1_xrtt_opts::to_string() const
{
  static const char* options[] = {"single", "dual"};
  return convert_enum_idx(options, 2, value, "irat_params_cdma2000_minus1_xrtt_s::rx_cfg1_xrtt_e_");
}

// IRAT-ParametersCDMA2000-HRPD ::= SEQUENCE
SRSASN_CODE irat_params_cdma2000_hrpd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_hrpd, 1, 32));
  HANDLE_CODE(tx_cfg_hrpd.pack(bref));
  HANDLE_CODE(rx_cfg_hrpd.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_cdma2000_hrpd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_hrpd, bref, 1, 32));
  HANDLE_CODE(tx_cfg_hrpd.unpack(bref));
  HANDLE_CODE(rx_cfg_hrpd.unpack(bref));

  return SRSASN_SUCCESS;
}
void irat_params_cdma2000_hrpd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandListHRPD");
  for (const auto& e1 : supported_band_list_hrpd) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.write_str("tx-ConfigHRPD", tx_cfg_hrpd.to_string());
  j.write_str("rx-ConfigHRPD", rx_cfg_hrpd.to_string());
  j.end_obj();
}

const char* irat_params_cdma2000_hrpd_s::tx_cfg_hrpd_opts::to_string() const
{
  static const char* options[] = {"single", "dual"};
  return convert_enum_idx(options, 2, value, "irat_params_cdma2000_hrpd_s::tx_cfg_hrpd_e_");
}

const char* irat_params_cdma2000_hrpd_s::rx_cfg_hrpd_opts::to_string() const
{
  static const char* options[] = {"single", "dual"};
  return convert_enum_idx(options, 2, value, "irat_params_cdma2000_hrpd_s::rx_cfg_hrpd_e_");
}

// SupportedBandGERAN ::= ENUMERATED
const char* supported_band_geran_opts::to_string() const
{
  static const char* options[] = {"gsm450",
                                  "gsm480",
                                  "gsm710",
                                  "gsm750",
                                  "gsm810",
                                  "gsm850",
                                  "gsm900P",
                                  "gsm900E",
                                  "gsm900R",
                                  "gsm1800",
                                  "gsm1900",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "supported_band_geran_e");
}

// IRAT-ParametersGERAN ::= SEQUENCE
SRSASN_CODE irat_params_geran_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_geran, 1, 64));
  HANDLE_CODE(bref.pack(inter_rat_ps_ho_to_geran, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_geran_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_geran, bref, 1, 64));
  HANDLE_CODE(bref.unpack(inter_rat_ps_ho_to_geran, 1));

  return SRSASN_SUCCESS;
}
void irat_params_geran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandListGERAN");
  for (const auto& e1 : supported_band_list_geran) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.write_bool("interRAT-PS-HO-ToGERAN", inter_rat_ps_ho_to_geran);
  j.end_obj();
}

// SupportedBandNR-r15 ::= SEQUENCE
SRSASN_CODE supported_band_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, band_nr_r15, (uint16_t)1u, (uint16_t)1024u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_band_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(band_nr_r15, bref, (uint16_t)1u, (uint16_t)1024u));

  return SRSASN_SUCCESS;
}
void supported_band_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandNR-r15", band_nr_r15);
  j.end_obj();
}

// IRAT-ParametersNR-r15 ::= SEQUENCE
SRSASN_CODE irat_params_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(en_dc_r15_present, 1));
  HANDLE_CODE(bref.pack(event_b2_r15_present, 1));
  HANDLE_CODE(bref.pack(supported_band_list_en_dc_r15_present, 1));

  if (supported_band_list_en_dc_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_en_dc_r15, 1, 1024));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(en_dc_r15_present, 1));
  HANDLE_CODE(bref.unpack(event_b2_r15_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_list_en_dc_r15_present, 1));

  if (supported_band_list_en_dc_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_en_dc_r15, bref, 1, 1024));
  }

  return SRSASN_SUCCESS;
}
void irat_params_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (en_dc_r15_present) {
    j.write_str("en-DC-r15", "supported");
  }
  if (event_b2_r15_present) {
    j.write_str("eventB2-r15", "supported");
  }
  if (supported_band_list_en_dc_r15_present) {
    j.start_array("supportedBandListEN-DC-r15");
    for (const auto& e1 : supported_band_list_en_dc_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// IRAT-ParametersNR-v1540 ::= SEQUENCE
SRSASN_CODE irat_params_nr_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eutra_minus5_gc_ho_to_nr_fdd_fr1_r15_present, 1));
  HANDLE_CODE(bref.pack(eutra_minus5_gc_ho_to_nr_tdd_fr1_r15_present, 1));
  HANDLE_CODE(bref.pack(eutra_minus5_gc_ho_to_nr_fdd_fr2_r15_present, 1));
  HANDLE_CODE(bref.pack(eutra_minus5_gc_ho_to_nr_tdd_fr2_r15_present, 1));
  HANDLE_CODE(bref.pack(eutra_epc_ho_to_nr_fdd_fr1_r15_present, 1));
  HANDLE_CODE(bref.pack(eutra_epc_ho_to_nr_tdd_fr1_r15_present, 1));
  HANDLE_CODE(bref.pack(eutra_epc_ho_to_nr_fdd_fr2_r15_present, 1));
  HANDLE_CODE(bref.pack(eutra_epc_ho_to_nr_tdd_fr2_r15_present, 1));
  HANDLE_CODE(bref.pack(ims_voice_over_nr_fr1_r15_present, 1));
  HANDLE_CODE(bref.pack(ims_voice_over_nr_fr2_r15_present, 1));
  HANDLE_CODE(bref.pack(sa_nr_r15_present, 1));
  HANDLE_CODE(bref.pack(supported_band_list_nr_sa_r15_present, 1));

  if (supported_band_list_nr_sa_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_nr_sa_r15, 1, 1024));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_nr_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(eutra_minus5_gc_ho_to_nr_fdd_fr1_r15_present, 1));
  HANDLE_CODE(bref.unpack(eutra_minus5_gc_ho_to_nr_tdd_fr1_r15_present, 1));
  HANDLE_CODE(bref.unpack(eutra_minus5_gc_ho_to_nr_fdd_fr2_r15_present, 1));
  HANDLE_CODE(bref.unpack(eutra_minus5_gc_ho_to_nr_tdd_fr2_r15_present, 1));
  HANDLE_CODE(bref.unpack(eutra_epc_ho_to_nr_fdd_fr1_r15_present, 1));
  HANDLE_CODE(bref.unpack(eutra_epc_ho_to_nr_tdd_fr1_r15_present, 1));
  HANDLE_CODE(bref.unpack(eutra_epc_ho_to_nr_fdd_fr2_r15_present, 1));
  HANDLE_CODE(bref.unpack(eutra_epc_ho_to_nr_tdd_fr2_r15_present, 1));
  HANDLE_CODE(bref.unpack(ims_voice_over_nr_fr1_r15_present, 1));
  HANDLE_CODE(bref.unpack(ims_voice_over_nr_fr2_r15_present, 1));
  HANDLE_CODE(bref.unpack(sa_nr_r15_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_list_nr_sa_r15_present, 1));

  if (supported_band_list_nr_sa_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_nr_sa_r15, bref, 1, 1024));
  }

  return SRSASN_SUCCESS;
}
void irat_params_nr_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eutra_minus5_gc_ho_to_nr_fdd_fr1_r15_present) {
    j.write_str("eutra-5GC-HO-ToNR-FDD-FR1-r15", "supported");
  }
  if (eutra_minus5_gc_ho_to_nr_tdd_fr1_r15_present) {
    j.write_str("eutra-5GC-HO-ToNR-TDD-FR1-r15", "supported");
  }
  if (eutra_minus5_gc_ho_to_nr_fdd_fr2_r15_present) {
    j.write_str("eutra-5GC-HO-ToNR-FDD-FR2-r15", "supported");
  }
  if (eutra_minus5_gc_ho_to_nr_tdd_fr2_r15_present) {
    j.write_str("eutra-5GC-HO-ToNR-TDD-FR2-r15", "supported");
  }
  if (eutra_epc_ho_to_nr_fdd_fr1_r15_present) {
    j.write_str("eutra-EPC-HO-ToNR-FDD-FR1-r15", "supported");
  }
  if (eutra_epc_ho_to_nr_tdd_fr1_r15_present) {
    j.write_str("eutra-EPC-HO-ToNR-TDD-FR1-r15", "supported");
  }
  if (eutra_epc_ho_to_nr_fdd_fr2_r15_present) {
    j.write_str("eutra-EPC-HO-ToNR-FDD-FR2-r15", "supported");
  }
  if (eutra_epc_ho_to_nr_tdd_fr2_r15_present) {
    j.write_str("eutra-EPC-HO-ToNR-TDD-FR2-r15", "supported");
  }
  if (ims_voice_over_nr_fr1_r15_present) {
    j.write_str("ims-VoiceOverNR-FR1-r15", "supported");
  }
  if (ims_voice_over_nr_fr2_r15_present) {
    j.write_str("ims-VoiceOverNR-FR2-r15", "supported");
  }
  if (sa_nr_r15_present) {
    j.write_str("sa-NR-r15", "supported");
  }
  if (supported_band_list_nr_sa_r15_present) {
    j.start_array("supportedBandListNR-SA-r15");
    for (const auto& e1 : supported_band_list_nr_sa_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SupportedBandUTRA-FDD ::= ENUMERATED
const char* supported_band_utra_fdd_opts::to_string() const
{
  static const char* options[] = {"bandI",        "bandII",        "bandIII",        "bandIV",       "bandV",
                                  "bandVI",       "bandVII",       "bandVIII",       "bandIX",       "bandX",
                                  "bandXI",       "bandXII",       "bandXIII",       "bandXIV",      "bandXV",
                                  "bandXVI",      "bandXVII-8a0",  "bandXVIII-8a0",  "bandXIX-8a0",  "bandXX-8a0",
                                  "bandXXI-8a0",  "bandXXII-8a0",  "bandXXIII-8a0",  "bandXXIV-8a0", "bandXXV-8a0",
                                  "bandXXVI-8a0", "bandXXVII-8a0", "bandXXVIII-8a0", "bandXXIX-8a0", "bandXXX-8a0",
                                  "bandXXXI-8a0", "bandXXXII-8a0"};
  return convert_enum_idx(options, 32, value, "supported_band_utra_fdd_e");
}

// IRAT-ParametersUTRA-FDD ::= SEQUENCE
SRSASN_CODE irat_params_utra_fdd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_utra_fdd, 1, 64));

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_utra_fdd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_utra_fdd, bref, 1, 64));

  return SRSASN_SUCCESS;
}
void irat_params_utra_fdd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandListUTRA-FDD");
  for (const auto& e1 : supported_band_list_utra_fdd) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

// SupportedBandUTRA-TDD128 ::= ENUMERATED
const char* supported_band_utra_tdd128_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p"};
  return convert_enum_idx(options, 16, value, "supported_band_utra_tdd128_e");
}

// IRAT-ParametersUTRA-TDD128 ::= SEQUENCE
SRSASN_CODE irat_params_utra_tdd128_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_utra_tdd128, 1, 64));

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_utra_tdd128_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_utra_tdd128, bref, 1, 64));

  return SRSASN_SUCCESS;
}
void irat_params_utra_tdd128_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandListUTRA-TDD128");
  for (const auto& e1 : supported_band_list_utra_tdd128) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

// SupportedBandUTRA-TDD384 ::= ENUMERATED
const char* supported_band_utra_tdd384_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p"};
  return convert_enum_idx(options, 16, value, "supported_band_utra_tdd384_e");
}

// IRAT-ParametersUTRA-TDD384 ::= SEQUENCE
SRSASN_CODE irat_params_utra_tdd384_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_utra_tdd384, 1, 64));

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_utra_tdd384_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_utra_tdd384, bref, 1, 64));

  return SRSASN_SUCCESS;
}
void irat_params_utra_tdd384_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandListUTRA-TDD384");
  for (const auto& e1 : supported_band_list_utra_tdd384) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

// SupportedBandUTRA-TDD768 ::= ENUMERATED
const char* supported_band_utra_tdd768_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p"};
  return convert_enum_idx(options, 16, value, "supported_band_utra_tdd768_e");
}

// IRAT-ParametersUTRA-TDD768 ::= SEQUENCE
SRSASN_CODE irat_params_utra_tdd768_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_utra_tdd768, 1, 64));

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_utra_tdd768_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_utra_tdd768, bref, 1, 64));

  return SRSASN_SUCCESS;
}
void irat_params_utra_tdd768_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandListUTRA-TDD768");
  for (const auto& e1 : supported_band_list_utra_tdd768) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

// IRAT-ParametersWLAN-r13 ::= SEQUENCE
SRSASN_CODE irat_params_wlan_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_list_wlan_r13_present, 1));

  if (supported_band_list_wlan_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_wlan_r13, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_wlan_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_list_wlan_r13_present, 1));

  if (supported_band_list_wlan_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_wlan_r13, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void irat_params_wlan_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_list_wlan_r13_present) {
    j.start_array("supportedBandListWLAN-r13");
    for (const auto& e1 : supported_band_list_wlan_r13) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  j.end_obj();
}

// ProcessingTimelineSet-r15 ::= ENUMERATED
const char* processing_timeline_set_r15_opts::to_string() const
{
  static const char* options[] = {"set1", "set2"};
  return convert_enum_idx(options, 2, value, "processing_timeline_set_r15_e");
}
uint8_t processing_timeline_set_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "processing_timeline_set_r15_e");
}

// SkipSubframeProcessing-r15 ::= SEQUENCE
SRSASN_CODE skip_sf_processing_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(skip_processing_dl_slot_r15_present, 1));
  HANDLE_CODE(bref.pack(skip_processing_dl_sub_slot_r15_present, 1));
  HANDLE_CODE(bref.pack(skip_processing_ul_slot_r15_present, 1));
  HANDLE_CODE(bref.pack(skip_processing_ul_sub_slot_r15_present, 1));

  if (skip_processing_dl_slot_r15_present) {
    HANDLE_CODE(pack_integer(bref, skip_processing_dl_slot_r15, (uint8_t)0u, (uint8_t)3u));
  }
  if (skip_processing_dl_sub_slot_r15_present) {
    HANDLE_CODE(pack_integer(bref, skip_processing_dl_sub_slot_r15, (uint8_t)0u, (uint8_t)3u));
  }
  if (skip_processing_ul_slot_r15_present) {
    HANDLE_CODE(pack_integer(bref, skip_processing_ul_slot_r15, (uint8_t)0u, (uint8_t)3u));
  }
  if (skip_processing_ul_sub_slot_r15_present) {
    HANDLE_CODE(pack_integer(bref, skip_processing_ul_sub_slot_r15, (uint8_t)0u, (uint8_t)3u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE skip_sf_processing_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(skip_processing_dl_slot_r15_present, 1));
  HANDLE_CODE(bref.unpack(skip_processing_dl_sub_slot_r15_present, 1));
  HANDLE_CODE(bref.unpack(skip_processing_ul_slot_r15_present, 1));
  HANDLE_CODE(bref.unpack(skip_processing_ul_sub_slot_r15_present, 1));

  if (skip_processing_dl_slot_r15_present) {
    HANDLE_CODE(unpack_integer(skip_processing_dl_slot_r15, bref, (uint8_t)0u, (uint8_t)3u));
  }
  if (skip_processing_dl_sub_slot_r15_present) {
    HANDLE_CODE(unpack_integer(skip_processing_dl_sub_slot_r15, bref, (uint8_t)0u, (uint8_t)3u));
  }
  if (skip_processing_ul_slot_r15_present) {
    HANDLE_CODE(unpack_integer(skip_processing_ul_slot_r15, bref, (uint8_t)0u, (uint8_t)3u));
  }
  if (skip_processing_ul_sub_slot_r15_present) {
    HANDLE_CODE(unpack_integer(skip_processing_ul_sub_slot_r15, bref, (uint8_t)0u, (uint8_t)3u));
  }

  return SRSASN_SUCCESS;
}
void skip_sf_processing_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (skip_processing_dl_slot_r15_present) {
    j.write_int("skipProcessingDL-Slot-r15", skip_processing_dl_slot_r15);
  }
  if (skip_processing_dl_sub_slot_r15_present) {
    j.write_int("skipProcessingDL-SubSlot-r15", skip_processing_dl_sub_slot_r15);
  }
  if (skip_processing_ul_slot_r15_present) {
    j.write_int("skipProcessingUL-Slot-r15", skip_processing_ul_slot_r15);
  }
  if (skip_processing_ul_sub_slot_r15_present) {
    j.write_int("skipProcessingUL-SubSlot-r15", skip_processing_ul_sub_slot_r15);
  }
  j.end_obj();
}

// MAC-Parameters-v1530 ::= SEQUENCE
SRSASN_CODE mac_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(min_proc_timeline_subslot_r15_present, 1));
  HANDLE_CODE(bref.pack(skip_sf_processing_r15_present, 1));
  HANDLE_CODE(bref.pack(early_data_up_r15_present, 1));
  HANDLE_CODE(bref.pack(dormant_scell_state_r15_present, 1));
  HANDLE_CODE(bref.pack(direct_scell_activation_r15_present, 1));
  HANDLE_CODE(bref.pack(direct_scell_hibernation_r15_present, 1));
  HANDLE_CODE(bref.pack(extended_lcid_dupl_r15_present, 1));
  HANDLE_CODE(bref.pack(sps_serving_cell_r15_present, 1));

  if (min_proc_timeline_subslot_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, min_proc_timeline_subslot_r15, 1, 3));
  }
  if (skip_sf_processing_r15_present) {
    HANDLE_CODE(skip_sf_processing_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(min_proc_timeline_subslot_r15_present, 1));
  HANDLE_CODE(bref.unpack(skip_sf_processing_r15_present, 1));
  HANDLE_CODE(bref.unpack(early_data_up_r15_present, 1));
  HANDLE_CODE(bref.unpack(dormant_scell_state_r15_present, 1));
  HANDLE_CODE(bref.unpack(direct_scell_activation_r15_present, 1));
  HANDLE_CODE(bref.unpack(direct_scell_hibernation_r15_present, 1));
  HANDLE_CODE(bref.unpack(extended_lcid_dupl_r15_present, 1));
  HANDLE_CODE(bref.unpack(sps_serving_cell_r15_present, 1));

  if (min_proc_timeline_subslot_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(min_proc_timeline_subslot_r15, bref, 1, 3));
  }
  if (skip_sf_processing_r15_present) {
    HANDLE_CODE(skip_sf_processing_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mac_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (min_proc_timeline_subslot_r15_present) {
    j.start_array("min-Proc-TimelineSubslot-r15");
    for (const auto& e1 : min_proc_timeline_subslot_r15) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (skip_sf_processing_r15_present) {
    j.write_fieldname("skipSubframeProcessing-r15");
    skip_sf_processing_r15.to_json(j);
  }
  if (early_data_up_r15_present) {
    j.write_str("earlyData-UP-r15", "supported");
  }
  if (dormant_scell_state_r15_present) {
    j.write_str("dormantSCellState-r15", "supported");
  }
  if (direct_scell_activation_r15_present) {
    j.write_str("directSCellActivation-r15", "supported");
  }
  if (direct_scell_hibernation_r15_present) {
    j.write_str("directSCellHibernation-r15", "supported");
  }
  if (extended_lcid_dupl_r15_present) {
    j.write_str("extendedLCID-Duplication-r15", "supported");
  }
  if (sps_serving_cell_r15_present) {
    j.write_str("sps-ServingCell-r15", "supported");
  }
  j.end_obj();
}

// MIMO-UE-BeamformedCapabilities-r13 ::= SEQUENCE
SRSASN_CODE mimo_ue_bf_cap_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(alt_codebook_r13_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, mimo_bf_cap_r13, 1, 4));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ue_bf_cap_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(alt_codebook_r13_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(mimo_bf_cap_r13, bref, 1, 4));

  return SRSASN_SUCCESS;
}
void mimo_ue_bf_cap_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (alt_codebook_r13_present) {
    j.write_str("altCodebook-r13", "supported");
  }
  j.start_array("mimo-BeamformedCapabilities-r13");
  for (const auto& e1 : mimo_bf_cap_r13) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MIMO-UE-ParametersPerTM-r13 ::= SEQUENCE
SRSASN_CODE mimo_ue_params_per_tm_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_precoded_r13_present, 1));
  HANDLE_CODE(bref.pack(bf_r13_present, 1));
  HANDLE_CODE(bref.pack(ch_meas_restrict_r13_present, 1));
  HANDLE_CODE(bref.pack(dmrs_enhance_r13_present, 1));
  HANDLE_CODE(bref.pack(csi_rs_enhance_tdd_r13_present, 1));

  if (non_precoded_r13_present) {
    HANDLE_CODE(non_precoded_r13.pack(bref));
  }
  if (bf_r13_present) {
    HANDLE_CODE(bf_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ue_params_per_tm_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_precoded_r13_present, 1));
  HANDLE_CODE(bref.unpack(bf_r13_present, 1));
  HANDLE_CODE(bref.unpack(ch_meas_restrict_r13_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_enhance_r13_present, 1));
  HANDLE_CODE(bref.unpack(csi_rs_enhance_tdd_r13_present, 1));

  if (non_precoded_r13_present) {
    HANDLE_CODE(non_precoded_r13.unpack(bref));
  }
  if (bf_r13_present) {
    HANDLE_CODE(bf_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mimo_ue_params_per_tm_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (non_precoded_r13_present) {
    j.write_fieldname("nonPrecoded-r13");
    non_precoded_r13.to_json(j);
  }
  if (bf_r13_present) {
    j.write_fieldname("beamformed-r13");
    bf_r13.to_json(j);
  }
  if (ch_meas_restrict_r13_present) {
    j.write_str("channelMeasRestriction-r13", "supported");
  }
  if (dmrs_enhance_r13_present) {
    j.write_str("dmrs-Enhancements-r13", "supported");
  }
  if (csi_rs_enhance_tdd_r13_present) {
    j.write_str("csi-RS-EnhancementsTDD-r13", "supported");
  }
  j.end_obj();
}

// MIMO-UE-Parameters-r13 ::= SEQUENCE
SRSASN_CODE mimo_ue_params_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(params_tm9_r13_present, 1));
  HANDLE_CODE(bref.pack(params_tm10_r13_present, 1));
  HANDLE_CODE(bref.pack(srs_enhance_tdd_r13_present, 1));
  HANDLE_CODE(bref.pack(srs_enhance_r13_present, 1));
  HANDLE_CODE(bref.pack(interference_meas_restrict_r13_present, 1));

  if (params_tm9_r13_present) {
    HANDLE_CODE(params_tm9_r13.pack(bref));
  }
  if (params_tm10_r13_present) {
    HANDLE_CODE(params_tm10_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ue_params_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(params_tm9_r13_present, 1));
  HANDLE_CODE(bref.unpack(params_tm10_r13_present, 1));
  HANDLE_CODE(bref.unpack(srs_enhance_tdd_r13_present, 1));
  HANDLE_CODE(bref.unpack(srs_enhance_r13_present, 1));
  HANDLE_CODE(bref.unpack(interference_meas_restrict_r13_present, 1));

  if (params_tm9_r13_present) {
    HANDLE_CODE(params_tm9_r13.unpack(bref));
  }
  if (params_tm10_r13_present) {
    HANDLE_CODE(params_tm10_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mimo_ue_params_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (params_tm9_r13_present) {
    j.write_fieldname("parametersTM9-r13");
    params_tm9_r13.to_json(j);
  }
  if (params_tm10_r13_present) {
    j.write_fieldname("parametersTM10-r13");
    params_tm10_r13.to_json(j);
  }
  if (srs_enhance_tdd_r13_present) {
    j.write_str("srs-EnhancementsTDD-r13", "supported");
  }
  if (srs_enhance_r13_present) {
    j.write_str("srs-Enhancements-r13", "supported");
  }
  if (interference_meas_restrict_r13_present) {
    j.write_str("interferenceMeasRestriction-r13", "supported");
  }
  j.end_obj();
}

// MIMO-UE-ParametersPerTM-v1430 ::= SEQUENCE
SRSASN_CODE mimo_ue_params_per_tm_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nzp_csi_rs_aperiodic_info_r14_present, 1));
  HANDLE_CODE(bref.pack(nzp_csi_rs_periodic_info_r14_present, 1));
  HANDLE_CODE(bref.pack(zp_csi_rs_aperiodic_info_r14_present, 1));
  HANDLE_CODE(bref.pack(ul_dmrs_enhance_r14_present, 1));
  HANDLE_CODE(bref.pack(density_reduction_np_r14_present, 1));
  HANDLE_CODE(bref.pack(density_reduction_bf_r14_present, 1));
  HANDLE_CODE(bref.pack(hybrid_csi_r14_present, 1));
  HANDLE_CODE(bref.pack(semi_ol_r14_present, 1));
  HANDLE_CODE(bref.pack(csi_report_np_r14_present, 1));
  HANDLE_CODE(bref.pack(csi_report_advanced_r14_present, 1));

  if (nzp_csi_rs_aperiodic_info_r14_present) {
    HANDLE_CODE(pack_integer(bref, nzp_csi_rs_aperiodic_info_r14.nmax_proc_r14, (uint8_t)5u, (uint8_t)32u));
    HANDLE_CODE(nzp_csi_rs_aperiodic_info_r14.nmax_res_r14.pack(bref));
  }
  if (nzp_csi_rs_periodic_info_r14_present) {
    HANDLE_CODE(nzp_csi_rs_periodic_info_r14.nmax_res_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ue_params_per_tm_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nzp_csi_rs_aperiodic_info_r14_present, 1));
  HANDLE_CODE(bref.unpack(nzp_csi_rs_periodic_info_r14_present, 1));
  HANDLE_CODE(bref.unpack(zp_csi_rs_aperiodic_info_r14_present, 1));
  HANDLE_CODE(bref.unpack(ul_dmrs_enhance_r14_present, 1));
  HANDLE_CODE(bref.unpack(density_reduction_np_r14_present, 1));
  HANDLE_CODE(bref.unpack(density_reduction_bf_r14_present, 1));
  HANDLE_CODE(bref.unpack(hybrid_csi_r14_present, 1));
  HANDLE_CODE(bref.unpack(semi_ol_r14_present, 1));
  HANDLE_CODE(bref.unpack(csi_report_np_r14_present, 1));
  HANDLE_CODE(bref.unpack(csi_report_advanced_r14_present, 1));

  if (nzp_csi_rs_aperiodic_info_r14_present) {
    HANDLE_CODE(unpack_integer(nzp_csi_rs_aperiodic_info_r14.nmax_proc_r14, bref, (uint8_t)5u, (uint8_t)32u));
    HANDLE_CODE(nzp_csi_rs_aperiodic_info_r14.nmax_res_r14.unpack(bref));
  }
  if (nzp_csi_rs_periodic_info_r14_present) {
    HANDLE_CODE(nzp_csi_rs_periodic_info_r14.nmax_res_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mimo_ue_params_per_tm_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nzp_csi_rs_aperiodic_info_r14_present) {
    j.write_fieldname("nzp-CSI-RS-AperiodicInfo-r14");
    j.start_obj();
    j.write_int("nMaxProc-r14", nzp_csi_rs_aperiodic_info_r14.nmax_proc_r14);
    j.write_str("nMaxResource-r14", nzp_csi_rs_aperiodic_info_r14.nmax_res_r14.to_string());
    j.end_obj();
  }
  if (nzp_csi_rs_periodic_info_r14_present) {
    j.write_fieldname("nzp-CSI-RS-PeriodicInfo-r14");
    j.start_obj();
    j.write_str("nMaxResource-r14", nzp_csi_rs_periodic_info_r14.nmax_res_r14.to_string());
    j.end_obj();
  }
  if (zp_csi_rs_aperiodic_info_r14_present) {
    j.write_str("zp-CSI-RS-AperiodicInfo-r14", "supported");
  }
  if (ul_dmrs_enhance_r14_present) {
    j.write_str("ul-dmrs-Enhancements-r14", "supported");
  }
  if (density_reduction_np_r14_present) {
    j.write_str("densityReductionNP-r14", "supported");
  }
  if (density_reduction_bf_r14_present) {
    j.write_str("densityReductionBF-r14", "supported");
  }
  if (hybrid_csi_r14_present) {
    j.write_str("hybridCSI-r14", "supported");
  }
  if (semi_ol_r14_present) {
    j.write_str("semiOL-r14", "supported");
  }
  if (csi_report_np_r14_present) {
    j.write_str("csi-ReportingNP-r14", "supported");
  }
  if (csi_report_advanced_r14_present) {
    j.write_str("csi-ReportingAdvanced-r14", "supported");
  }
  j.end_obj();
}

const char* mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_aperiodic_info_r14_s_::nmax_res_r14_opts::to_string() const
{
  static const char* options[] = {"ffs1", "ffs2", "ffs3", "ffs4"};
  return convert_enum_idx(
      options, 4, value, "mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_aperiodic_info_r14_s_::nmax_res_r14_e_");
}
uint8_t mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_aperiodic_info_r14_s_::nmax_res_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4};
  return map_enum_number(
      options, 4, value, "mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_aperiodic_info_r14_s_::nmax_res_r14_e_");
}

const char* mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_periodic_info_r14_s_::nmax_res_r14_opts::to_string() const
{
  static const char* options[] = {"ffs1", "ffs2", "ffs3", "ffs4"};
  return convert_enum_idx(
      options, 4, value, "mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_periodic_info_r14_s_::nmax_res_r14_e_");
}
uint8_t mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_periodic_info_r14_s_::nmax_res_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4};
  return map_enum_number(
      options, 4, value, "mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_periodic_info_r14_s_::nmax_res_r14_e_");
}

// MIMO-UE-Parameters-v1430 ::= SEQUENCE
SRSASN_CODE mimo_ue_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(params_tm9_v1430_present, 1));
  HANDLE_CODE(bref.pack(params_tm10_v1430_present, 1));

  if (params_tm9_v1430_present) {
    HANDLE_CODE(params_tm9_v1430.pack(bref));
  }
  if (params_tm10_v1430_present) {
    HANDLE_CODE(params_tm10_v1430.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ue_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(params_tm9_v1430_present, 1));
  HANDLE_CODE(bref.unpack(params_tm10_v1430_present, 1));

  if (params_tm9_v1430_present) {
    HANDLE_CODE(params_tm9_v1430.unpack(bref));
  }
  if (params_tm10_v1430_present) {
    HANDLE_CODE(params_tm10_v1430.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mimo_ue_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (params_tm9_v1430_present) {
    j.write_fieldname("parametersTM9-v1430");
    params_tm9_v1430.to_json(j);
  }
  if (params_tm10_v1430_present) {
    j.write_fieldname("parametersTM10-v1430");
    params_tm10_v1430.to_json(j);
  }
  j.end_obj();
}

// MIMO-UE-ParametersPerTM-v1470 ::= SEQUENCE
SRSASN_CODE mimo_ue_params_per_tm_v1470_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_report_advanced_max_ports_r14_present, 1));

  if (csi_report_advanced_max_ports_r14_present) {
    HANDLE_CODE(csi_report_advanced_max_ports_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ue_params_per_tm_v1470_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_report_advanced_max_ports_r14_present, 1));

  if (csi_report_advanced_max_ports_r14_present) {
    HANDLE_CODE(csi_report_advanced_max_ports_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mimo_ue_params_per_tm_v1470_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_report_advanced_max_ports_r14_present) {
    j.write_str("csi-ReportingAdvancedMaxPorts-r14", csi_report_advanced_max_ports_r14.to_string());
  }
  j.end_obj();
}

const char* mimo_ue_params_per_tm_v1470_s::csi_report_advanced_max_ports_r14_opts::to_string() const
{
  static const char* options[] = {"n8", "n12", "n16", "n20", "n24", "n28"};
  return convert_enum_idx(options, 6, value, "mimo_ue_params_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_");
}
uint8_t mimo_ue_params_per_tm_v1470_s::csi_report_advanced_max_ports_r14_opts::to_number() const
{
  static const uint8_t options[] = {8, 12, 16, 20, 24, 28};
  return map_enum_number(options, 6, value, "mimo_ue_params_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_");
}

// MIMO-UE-Parameters-v1470 ::= SEQUENCE
SRSASN_CODE mimo_ue_params_v1470_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(params_tm9_v1470.pack(bref));
  HANDLE_CODE(params_tm10_v1470.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ue_params_v1470_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(params_tm9_v1470.unpack(bref));
  HANDLE_CODE(params_tm10_v1470.unpack(bref));

  return SRSASN_SUCCESS;
}
void mimo_ue_params_v1470_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("parametersTM9-v1470");
  params_tm9_v1470.to_json(j);
  j.write_fieldname("parametersTM10-v1470");
  params_tm10_v1470.to_json(j);
  j.end_obj();
}

// MeasParameters ::= SEQUENCE
SRSASN_CODE meas_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, band_list_eutra, 1, 64));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(band_list_eutra, bref, 1, 64));

  return SRSASN_SUCCESS;
}
void meas_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("bandListEUTRA");
  for (const auto& e1 : band_list_eutra) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MeasParameters-v1020 ::= SEQUENCE
SRSASN_CODE meas_params_v1020_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, band_combination_list_eutra_r10, 1, 128));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_params_v1020_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(band_combination_list_eutra_r10, bref, 1, 128));

  return SRSASN_SUCCESS;
}
void meas_params_v1020_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("bandCombinationListEUTRA-r10");
  for (const auto& e1 : band_combination_list_eutra_r10) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// NAICS-Capability-Entry-r12 ::= SEQUENCE
SRSASN_CODE naics_cap_entry_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, nof_naics_capable_cc_r12, (uint8_t)1u, (uint8_t)5u));
  HANDLE_CODE(nof_aggregated_prb_r12.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE naics_cap_entry_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(nof_naics_capable_cc_r12, bref, (uint8_t)1u, (uint8_t)5u));
  HANDLE_CODE(nof_aggregated_prb_r12.unpack(bref));

  return SRSASN_SUCCESS;
}
void naics_cap_entry_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("numberOfNAICS-CapableCC-r12", nof_naics_capable_cc_r12);
  j.write_str("numberOfAggregatedPRB-r12", nof_aggregated_prb_r12.to_string());
  j.end_obj();
}

const char* naics_cap_entry_r12_s::nof_aggregated_prb_r12_opts::to_string() const
{
  static const char* options[] = {"n50",
                                  "n75",
                                  "n100",
                                  "n125",
                                  "n150",
                                  "n175",
                                  "n200",
                                  "n225",
                                  "n250",
                                  "n275",
                                  "n300",
                                  "n350",
                                  "n400",
                                  "n450",
                                  "n500",
                                  "spare"};
  return convert_enum_idx(options, 16, value, "naics_cap_entry_r12_s::nof_aggregated_prb_r12_e_");
}
uint16_t naics_cap_entry_r12_s::nof_aggregated_prb_r12_opts::to_number() const
{
  static const uint16_t options[] = {50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 350, 400, 450, 500};
  return map_enum_number(options, 15, value, "naics_cap_entry_r12_s::nof_aggregated_prb_r12_e_");
}

// NonContiguousUL-RA-WithinCC-r10 ::= SEQUENCE
SRSASN_CODE non_contiguous_ul_ra_within_cc_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_contiguous_ul_ra_within_cc_info_r10_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE non_contiguous_ul_ra_within_cc_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_contiguous_ul_ra_within_cc_info_r10_present, 1));

  return SRSASN_SUCCESS;
}
void non_contiguous_ul_ra_within_cc_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (non_contiguous_ul_ra_within_cc_info_r10_present) {
    j.write_str("nonContiguousUL-RA-WithinCC-Info-r10", "supported");
  }
  j.end_obj();
}

// ROHC-ProfileSupportList-r15 ::= SEQUENCE
SRSASN_CODE rohc_profile_support_list_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(profile0x0001_r15, 1));
  HANDLE_CODE(bref.pack(profile0x0002_r15, 1));
  HANDLE_CODE(bref.pack(profile0x0003_r15, 1));
  HANDLE_CODE(bref.pack(profile0x0004_r15, 1));
  HANDLE_CODE(bref.pack(profile0x0006_r15, 1));
  HANDLE_CODE(bref.pack(profile0x0101_r15, 1));
  HANDLE_CODE(bref.pack(profile0x0102_r15, 1));
  HANDLE_CODE(bref.pack(profile0x0103_r15, 1));
  HANDLE_CODE(bref.pack(profile0x0104_r15, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rohc_profile_support_list_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(profile0x0001_r15, 1));
  HANDLE_CODE(bref.unpack(profile0x0002_r15, 1));
  HANDLE_CODE(bref.unpack(profile0x0003_r15, 1));
  HANDLE_CODE(bref.unpack(profile0x0004_r15, 1));
  HANDLE_CODE(bref.unpack(profile0x0006_r15, 1));
  HANDLE_CODE(bref.unpack(profile0x0101_r15, 1));
  HANDLE_CODE(bref.unpack(profile0x0102_r15, 1));
  HANDLE_CODE(bref.unpack(profile0x0103_r15, 1));
  HANDLE_CODE(bref.unpack(profile0x0104_r15, 1));

  return SRSASN_SUCCESS;
}
void rohc_profile_support_list_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("profile0x0001-r15", profile0x0001_r15);
  j.write_bool("profile0x0002-r15", profile0x0002_r15);
  j.write_bool("profile0x0003-r15", profile0x0003_r15);
  j.write_bool("profile0x0004-r15", profile0x0004_r15);
  j.write_bool("profile0x0006-r15", profile0x0006_r15);
  j.write_bool("profile0x0101-r15", profile0x0101_r15);
  j.write_bool("profile0x0102-r15", profile0x0102_r15);
  j.write_bool("profile0x0103-r15", profile0x0103_r15);
  j.write_bool("profile0x0104-r15", profile0x0104_r15);
  j.end_obj();
}

// PDCP-Parameters ::= SEQUENCE
SRSASN_CODE pdcp_params_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(max_num_rohc_context_sessions_present, 1));

  HANDLE_CODE(supported_rohc_profiles.pack(bref));
  if (max_num_rohc_context_sessions_present) {
    HANDLE_CODE(max_num_rohc_context_sessions.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_params_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(max_num_rohc_context_sessions_present, 1));

  HANDLE_CODE(supported_rohc_profiles.unpack(bref));
  if (max_num_rohc_context_sessions_present) {
    HANDLE_CODE(max_num_rohc_context_sessions.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdcp_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("supportedROHC-Profiles");
  supported_rohc_profiles.to_json(j);
  if (max_num_rohc_context_sessions_present) {
    j.write_str("maxNumberROHC-ContextSessions", max_num_rohc_context_sessions.to_string());
  }
  j.end_obj();
}

const char* pdcp_params_s::max_num_rohc_context_sessions_opts::to_string() const
{
  static const char* options[] = {"cs2",
                                  "cs4",
                                  "cs8",
                                  "cs12",
                                  "cs16",
                                  "cs24",
                                  "cs32",
                                  "cs48",
                                  "cs64",
                                  "cs128",
                                  "cs256",
                                  "cs512",
                                  "cs1024",
                                  "cs16384",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pdcp_params_s::max_num_rohc_context_sessions_e_");
}
uint16_t pdcp_params_s::max_num_rohc_context_sessions_opts::to_number() const
{
  static const uint16_t options[] = {2, 4, 8, 12, 16, 24, 32, 48, 64, 128, 256, 512, 1024, 16384};
  return map_enum_number(options, 14, value, "pdcp_params_s::max_num_rohc_context_sessions_e_");
}

// SupportedOperatorDic-r15 ::= SEQUENCE
SRSASN_CODE supported_operator_dic_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, version_of_dictionary_r15, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(associated_plmn_id_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_operator_dic_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(version_of_dictionary_r15, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(associated_plmn_id_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void supported_operator_dic_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("versionOfDictionary-r15", version_of_dictionary_r15);
  j.write_fieldname("associatedPLMN-ID-r15");
  associated_plmn_id_r15.to_json(j);
  j.end_obj();
}

// SupportedUDC-r15 ::= SEQUENCE
SRSASN_CODE supported_udc_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_standard_dic_r15_present, 1));
  HANDLE_CODE(bref.pack(supported_operator_dic_r15_present, 1));

  if (supported_operator_dic_r15_present) {
    HANDLE_CODE(supported_operator_dic_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_udc_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_standard_dic_r15_present, 1));
  HANDLE_CODE(bref.unpack(supported_operator_dic_r15_present, 1));

  if (supported_operator_dic_r15_present) {
    HANDLE_CODE(supported_operator_dic_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void supported_udc_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_standard_dic_r15_present) {
    j.write_str("supportedStandardDic-r15", "supported");
  }
  if (supported_operator_dic_r15_present) {
    j.write_fieldname("supportedOperatorDic-r15");
    supported_operator_dic_r15.to_json(j);
  }
  j.end_obj();
}

// PDCP-Parameters-v1530 ::= SEQUENCE
SRSASN_CODE pdcp_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_udc_r15_present, 1));
  HANDLE_CODE(bref.pack(pdcp_dupl_r15_present, 1));

  if (supported_udc_r15_present) {
    HANDLE_CODE(supported_udc_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_udc_r15_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_dupl_r15_present, 1));

  if (supported_udc_r15_present) {
    HANDLE_CODE(supported_udc_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdcp_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_udc_r15_present) {
    j.write_fieldname("supportedUDC-r15");
    supported_udc_r15.to_json(j);
  }
  if (pdcp_dupl_r15_present) {
    j.write_str("pdcp-Duplication-r15", "supported");
  }
  j.end_obj();
}

// PDCP-ParametersNR-r15 ::= SEQUENCE
SRSASN_CODE pdcp_params_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rohc_context_max_sessions_r15_present, 1));
  HANDLE_CODE(bref.pack(rohc_context_continue_r15_present, 1));
  HANDLE_CODE(bref.pack(out_of_order_delivery_r15_present, 1));
  HANDLE_CODE(bref.pack(sn_size_lo_r15_present, 1));
  HANDLE_CODE(bref.pack(ims_voice_over_nr_pdcp_mcg_bearer_r15_present, 1));
  HANDLE_CODE(bref.pack(ims_voice_over_nr_pdcp_scg_bearer_r15_present, 1));

  HANDLE_CODE(rohc_profiles_r15.pack(bref));
  if (rohc_context_max_sessions_r15_present) {
    HANDLE_CODE(rohc_context_max_sessions_r15.pack(bref));
  }
  HANDLE_CODE(bref.pack(rohc_profiles_ul_only_r15.profile0x0006_r15, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_params_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rohc_context_max_sessions_r15_present, 1));
  HANDLE_CODE(bref.unpack(rohc_context_continue_r15_present, 1));
  HANDLE_CODE(bref.unpack(out_of_order_delivery_r15_present, 1));
  HANDLE_CODE(bref.unpack(sn_size_lo_r15_present, 1));
  HANDLE_CODE(bref.unpack(ims_voice_over_nr_pdcp_mcg_bearer_r15_present, 1));
  HANDLE_CODE(bref.unpack(ims_voice_over_nr_pdcp_scg_bearer_r15_present, 1));

  HANDLE_CODE(rohc_profiles_r15.unpack(bref));
  if (rohc_context_max_sessions_r15_present) {
    HANDLE_CODE(rohc_context_max_sessions_r15.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(rohc_profiles_ul_only_r15.profile0x0006_r15, 1));

  return SRSASN_SUCCESS;
}
void pdcp_params_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rohc-Profiles-r15");
  rohc_profiles_r15.to_json(j);
  if (rohc_context_max_sessions_r15_present) {
    j.write_str("rohc-ContextMaxSessions-r15", rohc_context_max_sessions_r15.to_string());
  }
  j.write_fieldname("rohc-ProfilesUL-Only-r15");
  j.start_obj();
  j.write_bool("profile0x0006-r15", rohc_profiles_ul_only_r15.profile0x0006_r15);
  j.end_obj();
  if (rohc_context_continue_r15_present) {
    j.write_str("rohc-ContextContinue-r15", "supported");
  }
  if (out_of_order_delivery_r15_present) {
    j.write_str("outOfOrderDelivery-r15", "supported");
  }
  if (sn_size_lo_r15_present) {
    j.write_str("sn-SizeLo-r15", "supported");
  }
  if (ims_voice_over_nr_pdcp_mcg_bearer_r15_present) {
    j.write_str("ims-VoiceOverNR-PDCP-MCG-Bearer-r15", "supported");
  }
  if (ims_voice_over_nr_pdcp_scg_bearer_r15_present) {
    j.write_str("ims-VoiceOverNR-PDCP-SCG-Bearer-r15", "supported");
  }
  j.end_obj();
}

const char* pdcp_params_nr_r15_s::rohc_context_max_sessions_r15_opts::to_string() const
{
  static const char* options[] = {"cs2",
                                  "cs4",
                                  "cs8",
                                  "cs12",
                                  "cs16",
                                  "cs24",
                                  "cs32",
                                  "cs48",
                                  "cs64",
                                  "cs128",
                                  "cs256",
                                  "cs512",
                                  "cs1024",
                                  "cs16384",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pdcp_params_nr_r15_s::rohc_context_max_sessions_r15_e_");
}
uint16_t pdcp_params_nr_r15_s::rohc_context_max_sessions_r15_opts::to_number() const
{
  static const uint16_t options[] = {2, 4, 8, 12, 16, 24, 32, 48, 64, 128, 256, 512, 1024, 16384};
  return map_enum_number(options, 14, value, "pdcp_params_nr_r15_s::rohc_context_max_sessions_r15_e_");
}

// PhyLayerParameters-v1020 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v1020_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(two_ant_ports_for_pucch_r10_present, 1));
  HANDLE_CODE(bref.pack(tm9_with_minus8_tx_fdd_r10_present, 1));
  HANDLE_CODE(bref.pack(pmi_disabling_r10_present, 1));
  HANDLE_CODE(bref.pack(cross_carrier_sched_r10_present, 1));
  HANDLE_CODE(bref.pack(simul_pucch_pusch_r10_present, 1));
  HANDLE_CODE(bref.pack(multi_cluster_pusch_within_cc_r10_present, 1));
  HANDLE_CODE(bref.pack(non_contiguous_ul_ra_within_cc_list_r10_present, 1));

  if (non_contiguous_ul_ra_within_cc_list_r10_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, non_contiguous_ul_ra_within_cc_list_r10, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v1020_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(two_ant_ports_for_pucch_r10_present, 1));
  HANDLE_CODE(bref.unpack(tm9_with_minus8_tx_fdd_r10_present, 1));
  HANDLE_CODE(bref.unpack(pmi_disabling_r10_present, 1));
  HANDLE_CODE(bref.unpack(cross_carrier_sched_r10_present, 1));
  HANDLE_CODE(bref.unpack(simul_pucch_pusch_r10_present, 1));
  HANDLE_CODE(bref.unpack(multi_cluster_pusch_within_cc_r10_present, 1));
  HANDLE_CODE(bref.unpack(non_contiguous_ul_ra_within_cc_list_r10_present, 1));

  if (non_contiguous_ul_ra_within_cc_list_r10_present) {
    HANDLE_CODE(unpack_dyn_seq_of(non_contiguous_ul_ra_within_cc_list_r10, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void phy_layer_params_v1020_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (two_ant_ports_for_pucch_r10_present) {
    j.write_str("twoAntennaPortsForPUCCH-r10", "supported");
  }
  if (tm9_with_minus8_tx_fdd_r10_present) {
    j.write_str("tm9-With-8Tx-FDD-r10", "supported");
  }
  if (pmi_disabling_r10_present) {
    j.write_str("pmi-Disabling-r10", "supported");
  }
  if (cross_carrier_sched_r10_present) {
    j.write_str("crossCarrierScheduling-r10", "supported");
  }
  if (simul_pucch_pusch_r10_present) {
    j.write_str("simultaneousPUCCH-PUSCH-r10", "supported");
  }
  if (multi_cluster_pusch_within_cc_r10_present) {
    j.write_str("multiClusterPUSCH-WithinCC-r10", "supported");
  }
  if (non_contiguous_ul_ra_within_cc_list_r10_present) {
    j.start_array("nonContiguousUL-RA-WithinCC-List-r10");
    for (const auto& e1 : non_contiguous_ul_ra_within_cc_list_r10) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// PhyLayerParameters-v1250 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(e_harq_pattern_fdd_r12_present, 1));
  HANDLE_CODE(bref.pack(enhanced_minus4_tx_codebook_r12_present, 1));
  HANDLE_CODE(bref.pack(tdd_fdd_ca_pcell_duplex_r12_present, 1));
  HANDLE_CODE(bref.pack(phy_tdd_re_cfg_tdd_pcell_r12_present, 1));
  HANDLE_CODE(bref.pack(phy_tdd_re_cfg_fdd_pcell_r12_present, 1));
  HANDLE_CODE(bref.pack(pusch_feedback_mode_r12_present, 1));
  HANDLE_CODE(bref.pack(pusch_srs_pwr_ctrl_sf_set_r12_present, 1));
  HANDLE_CODE(bref.pack(csi_sf_set_r12_present, 1));
  HANDLE_CODE(bref.pack(no_res_restrict_for_tti_bundling_r12_present, 1));
  HANDLE_CODE(bref.pack(discovery_signals_in_deact_scell_r12_present, 1));
  HANDLE_CODE(bref.pack(naics_cap_list_r12_present, 1));

  if (tdd_fdd_ca_pcell_duplex_r12_present) {
    HANDLE_CODE(tdd_fdd_ca_pcell_duplex_r12.pack(bref));
  }
  if (naics_cap_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, naics_cap_list_r12, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(e_harq_pattern_fdd_r12_present, 1));
  HANDLE_CODE(bref.unpack(enhanced_minus4_tx_codebook_r12_present, 1));
  HANDLE_CODE(bref.unpack(tdd_fdd_ca_pcell_duplex_r12_present, 1));
  HANDLE_CODE(bref.unpack(phy_tdd_re_cfg_tdd_pcell_r12_present, 1));
  HANDLE_CODE(bref.unpack(phy_tdd_re_cfg_fdd_pcell_r12_present, 1));
  HANDLE_CODE(bref.unpack(pusch_feedback_mode_r12_present, 1));
  HANDLE_CODE(bref.unpack(pusch_srs_pwr_ctrl_sf_set_r12_present, 1));
  HANDLE_CODE(bref.unpack(csi_sf_set_r12_present, 1));
  HANDLE_CODE(bref.unpack(no_res_restrict_for_tti_bundling_r12_present, 1));
  HANDLE_CODE(bref.unpack(discovery_signals_in_deact_scell_r12_present, 1));
  HANDLE_CODE(bref.unpack(naics_cap_list_r12_present, 1));

  if (tdd_fdd_ca_pcell_duplex_r12_present) {
    HANDLE_CODE(tdd_fdd_ca_pcell_duplex_r12.unpack(bref));
  }
  if (naics_cap_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(naics_cap_list_r12, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void phy_layer_params_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (e_harq_pattern_fdd_r12_present) {
    j.write_str("e-HARQ-Pattern-FDD-r12", "supported");
  }
  if (enhanced_minus4_tx_codebook_r12_present) {
    j.write_str("enhanced-4TxCodebook-r12", "supported");
  }
  if (tdd_fdd_ca_pcell_duplex_r12_present) {
    j.write_str("tdd-FDD-CA-PCellDuplex-r12", tdd_fdd_ca_pcell_duplex_r12.to_string());
  }
  if (phy_tdd_re_cfg_tdd_pcell_r12_present) {
    j.write_str("phy-TDD-ReConfig-TDD-PCell-r12", "supported");
  }
  if (phy_tdd_re_cfg_fdd_pcell_r12_present) {
    j.write_str("phy-TDD-ReConfig-FDD-PCell-r12", "supported");
  }
  if (pusch_feedback_mode_r12_present) {
    j.write_str("pusch-FeedbackMode-r12", "supported");
  }
  if (pusch_srs_pwr_ctrl_sf_set_r12_present) {
    j.write_str("pusch-SRS-PowerControl-SubframeSet-r12", "supported");
  }
  if (csi_sf_set_r12_present) {
    j.write_str("csi-SubframeSet-r12", "supported");
  }
  if (no_res_restrict_for_tti_bundling_r12_present) {
    j.write_str("noResourceRestrictionForTTIBundling-r12", "supported");
  }
  if (discovery_signals_in_deact_scell_r12_present) {
    j.write_str("discoverySignalsInDeactSCell-r12", "supported");
  }
  if (naics_cap_list_r12_present) {
    j.start_array("naics-Capability-List-r12");
    for (const auto& e1 : naics_cap_list_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// PhyLayerParameters-v1320 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v1320_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mimo_ue_params_r13_present, 1));

  if (mimo_ue_params_r13_present) {
    HANDLE_CODE(mimo_ue_params_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v1320_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mimo_ue_params_r13_present, 1));

  if (mimo_ue_params_r13_present) {
    HANDLE_CODE(mimo_ue_params_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void phy_layer_params_v1320_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mimo_ue_params_r13_present) {
    j.write_fieldname("mimo-UE-Parameters-r13");
    mimo_ue_params_r13.to_json(j);
  }
  j.end_obj();
}

// PhyLayerParameters-v1330 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v1330_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cch_interf_mitigation_ref_rec_type_a_r13_present, 1));
  HANDLE_CODE(bref.pack(cch_interf_mitigation_ref_rec_type_b_r13_present, 1));
  HANDLE_CODE(bref.pack(cch_interf_mitigation_max_num_ccs_r13_present, 1));
  HANDLE_CODE(bref.pack(crs_interf_mitigation_tm1to_tm9_r13_present, 1));

  if (cch_interf_mitigation_max_num_ccs_r13_present) {
    HANDLE_CODE(pack_integer(bref, cch_interf_mitigation_max_num_ccs_r13, (uint8_t)1u, (uint8_t)32u));
  }
  if (crs_interf_mitigation_tm1to_tm9_r13_present) {
    HANDLE_CODE(pack_integer(bref, crs_interf_mitigation_tm1to_tm9_r13, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v1330_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cch_interf_mitigation_ref_rec_type_a_r13_present, 1));
  HANDLE_CODE(bref.unpack(cch_interf_mitigation_ref_rec_type_b_r13_present, 1));
  HANDLE_CODE(bref.unpack(cch_interf_mitigation_max_num_ccs_r13_present, 1));
  HANDLE_CODE(bref.unpack(crs_interf_mitigation_tm1to_tm9_r13_present, 1));

  if (cch_interf_mitigation_max_num_ccs_r13_present) {
    HANDLE_CODE(unpack_integer(cch_interf_mitigation_max_num_ccs_r13, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (crs_interf_mitigation_tm1to_tm9_r13_present) {
    HANDLE_CODE(unpack_integer(crs_interf_mitigation_tm1to_tm9_r13, bref, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
void phy_layer_params_v1330_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cch_interf_mitigation_ref_rec_type_a_r13_present) {
    j.write_str("cch-InterfMitigation-RefRecTypeA-r13", "supported");
  }
  if (cch_interf_mitigation_ref_rec_type_b_r13_present) {
    j.write_str("cch-InterfMitigation-RefRecTypeB-r13", "supported");
  }
  if (cch_interf_mitigation_max_num_ccs_r13_present) {
    j.write_int("cch-InterfMitigation-MaxNumCCs-r13", cch_interf_mitigation_max_num_ccs_r13);
  }
  if (crs_interf_mitigation_tm1to_tm9_r13_present) {
    j.write_int("crs-InterfMitigationTM1toTM9-r13", crs_interf_mitigation_tm1to_tm9_r13);
  }
  j.end_obj();
}

// FeMBMS-Unicast-Parameters-r14 ::= SEQUENCE
SRSASN_CODE fe_mbms_unicast_params_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(unicast_fembms_mixed_scell_r14_present, 1));
  HANDLE_CODE(bref.pack(empty_unicast_region_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE fe_mbms_unicast_params_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(unicast_fembms_mixed_scell_r14_present, 1));
  HANDLE_CODE(bref.unpack(empty_unicast_region_r14_present, 1));

  return SRSASN_SUCCESS;
}
void fe_mbms_unicast_params_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (unicast_fembms_mixed_scell_r14_present) {
    j.write_str("unicast-fembmsMixedSCell-r14", "supported");
  }
  if (empty_unicast_region_r14_present) {
    j.write_str("emptyUnicastRegion-r14", "supported");
  }
  j.end_obj();
}

// PhyLayerParameters-v1430 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ce_pusch_nb_max_tbs_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_pdsch_pusch_max_bw_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_harq_ack_bundling_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_pdsch_ten_processes_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_retuning_symbols_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_pdsch_pusch_enhancement_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_sched_enhancement_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_srs_enhancement_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_pucch_enhancement_r14_present, 1));
  HANDLE_CODE(bref.pack(ce_closed_loop_tx_ant_sel_r14_present, 1));
  HANDLE_CODE(bref.pack(tdd_special_sf_r14_present, 1));
  HANDLE_CODE(bref.pack(tdd_tti_bundling_r14_present, 1));
  HANDLE_CODE(bref.pack(dmrs_less_up_pts_r14_present, 1));
  HANDLE_CODE(bref.pack(mimo_ue_params_v1430_present, 1));
  HANDLE_CODE(bref.pack(alt_tbs_idx_r14_present, 1));
  HANDLE_CODE(bref.pack(fe_mbms_unicast_params_r14_present, 1));

  if (ce_pdsch_pusch_max_bw_r14_present) {
    HANDLE_CODE(ce_pdsch_pusch_max_bw_r14.pack(bref));
  }
  if (ce_retuning_symbols_r14_present) {
    HANDLE_CODE(ce_retuning_symbols_r14.pack(bref));
  }
  if (mimo_ue_params_v1430_present) {
    HANDLE_CODE(mimo_ue_params_v1430.pack(bref));
  }
  if (fe_mbms_unicast_params_r14_present) {
    HANDLE_CODE(fe_mbms_unicast_params_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ce_pusch_nb_max_tbs_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_pdsch_pusch_max_bw_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_harq_ack_bundling_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_pdsch_ten_processes_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_retuning_symbols_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_pdsch_pusch_enhancement_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_sched_enhancement_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_srs_enhancement_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_pucch_enhancement_r14_present, 1));
  HANDLE_CODE(bref.unpack(ce_closed_loop_tx_ant_sel_r14_present, 1));
  HANDLE_CODE(bref.unpack(tdd_special_sf_r14_present, 1));
  HANDLE_CODE(bref.unpack(tdd_tti_bundling_r14_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_less_up_pts_r14_present, 1));
  HANDLE_CODE(bref.unpack(mimo_ue_params_v1430_present, 1));
  HANDLE_CODE(bref.unpack(alt_tbs_idx_r14_present, 1));
  HANDLE_CODE(bref.unpack(fe_mbms_unicast_params_r14_present, 1));

  if (ce_pdsch_pusch_max_bw_r14_present) {
    HANDLE_CODE(ce_pdsch_pusch_max_bw_r14.unpack(bref));
  }
  if (ce_retuning_symbols_r14_present) {
    HANDLE_CODE(ce_retuning_symbols_r14.unpack(bref));
  }
  if (mimo_ue_params_v1430_present) {
    HANDLE_CODE(mimo_ue_params_v1430.unpack(bref));
  }
  if (fe_mbms_unicast_params_r14_present) {
    HANDLE_CODE(fe_mbms_unicast_params_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void phy_layer_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ce_pusch_nb_max_tbs_r14_present) {
    j.write_str("ce-PUSCH-NB-MaxTBS-r14", "supported");
  }
  if (ce_pdsch_pusch_max_bw_r14_present) {
    j.write_str("ce-PDSCH-PUSCH-MaxBandwidth-r14", ce_pdsch_pusch_max_bw_r14.to_string());
  }
  if (ce_harq_ack_bundling_r14_present) {
    j.write_str("ce-HARQ-AckBundling-r14", "supported");
  }
  if (ce_pdsch_ten_processes_r14_present) {
    j.write_str("ce-PDSCH-TenProcesses-r14", "supported");
  }
  if (ce_retuning_symbols_r14_present) {
    j.write_str("ce-RetuningSymbols-r14", ce_retuning_symbols_r14.to_string());
  }
  if (ce_pdsch_pusch_enhancement_r14_present) {
    j.write_str("ce-PDSCH-PUSCH-Enhancement-r14", "supported");
  }
  if (ce_sched_enhancement_r14_present) {
    j.write_str("ce-SchedulingEnhancement-r14", "supported");
  }
  if (ce_srs_enhancement_r14_present) {
    j.write_str("ce-SRS-Enhancement-r14", "supported");
  }
  if (ce_pucch_enhancement_r14_present) {
    j.write_str("ce-PUCCH-Enhancement-r14", "supported");
  }
  if (ce_closed_loop_tx_ant_sel_r14_present) {
    j.write_str("ce-ClosedLoopTxAntennaSelection-r14", "supported");
  }
  if (tdd_special_sf_r14_present) {
    j.write_str("tdd-SpecialSubframe-r14", "supported");
  }
  if (tdd_tti_bundling_r14_present) {
    j.write_str("tdd-TTI-Bundling-r14", "supported");
  }
  if (dmrs_less_up_pts_r14_present) {
    j.write_str("dmrs-LessUpPTS-r14", "supported");
  }
  if (mimo_ue_params_v1430_present) {
    j.write_fieldname("mimo-UE-Parameters-v1430");
    mimo_ue_params_v1430.to_json(j);
  }
  if (alt_tbs_idx_r14_present) {
    j.write_str("alternativeTBS-Index-r14", "supported");
  }
  if (fe_mbms_unicast_params_r14_present) {
    j.write_fieldname("feMBMS-Unicast-Parameters-r14");
    fe_mbms_unicast_params_r14.to_json(j);
  }
  j.end_obj();
}

const char* phy_layer_params_v1430_s::ce_pdsch_pusch_max_bw_r14_opts::to_string() const
{
  static const char* options[] = {"bw5", "bw20"};
  return convert_enum_idx(options, 2, value, "phy_layer_params_v1430_s::ce_pdsch_pusch_max_bw_r14_e_");
}
uint8_t phy_layer_params_v1430_s::ce_pdsch_pusch_max_bw_r14_opts::to_number() const
{
  static const uint8_t options[] = {5, 20};
  return map_enum_number(options, 2, value, "phy_layer_params_v1430_s::ce_pdsch_pusch_max_bw_r14_e_");
}

const char* phy_layer_params_v1430_s::ce_retuning_symbols_r14_opts::to_string() const
{
  static const char* options[] = {"n0", "n1"};
  return convert_enum_idx(options, 2, value, "phy_layer_params_v1430_s::ce_retuning_symbols_r14_e_");
}
uint8_t phy_layer_params_v1430_s::ce_retuning_symbols_r14_opts::to_number() const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(options, 2, value, "phy_layer_params_v1430_s::ce_retuning_symbols_r14_e_");
}

// PhyLayerParameters-v1470 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v1470_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mimo_ue_params_v1470_present, 1));
  HANDLE_CODE(bref.pack(srs_up_pts_minus6sym_r14_present, 1));

  if (mimo_ue_params_v1470_present) {
    HANDLE_CODE(mimo_ue_params_v1470.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v1470_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mimo_ue_params_v1470_present, 1));
  HANDLE_CODE(bref.unpack(srs_up_pts_minus6sym_r14_present, 1));

  if (mimo_ue_params_v1470_present) {
    HANDLE_CODE(mimo_ue_params_v1470.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void phy_layer_params_v1470_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mimo_ue_params_v1470_present) {
    j.write_fieldname("mimo-UE-Parameters-v1470");
    mimo_ue_params_v1470.to_json(j);
  }
  if (srs_up_pts_minus6sym_r14_present) {
    j.write_str("srs-UpPTS-6sym-r14", "supported");
  }
  j.end_obj();
}

// PhyLayerParameters-v1530 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(stti_spt_cap_r15_present, 1));
  HANDLE_CODE(bref.pack(ce_cap_r15_present, 1));
  HANDLE_CODE(bref.pack(short_cqi_for_scell_activation_r15_present, 1));
  HANDLE_CODE(bref.pack(mimo_cbsr_advanced_csi_r15_present, 1));
  HANDLE_CODE(bref.pack(crs_intf_mitig_r15_present, 1));
  HANDLE_CODE(bref.pack(ul_pwr_ctrl_enhance_r15_present, 1));
  HANDLE_CODE(bref.pack(urllc_cap_r15_present, 1));
  HANDLE_CODE(bref.pack(alt_mcs_table_r15_present, 1));

  if (stti_spt_cap_r15_present) {
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.aperiodic_csi_report_stti_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.dmrs_based_spdcch_mbsfn_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.dmrs_based_spdcch_non_mbsfn_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.dmrs_position_pattern_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.dmrs_sharing_subslot_pdsch_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.dmrs_repeat_subslot_pdsch_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.epdcch_spt_different_cells_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.epdcch_stti_different_cells_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.max_layers_slot_or_subslot_pusch_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.max_num_upd_csi_proc_spt_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb77_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb27_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set1_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set2_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.mimo_ue_params_stti_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.mimo_ue_params_stti_v1530_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.nof_blind_decodes_uss_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.pdsch_slot_subslot_pdsch_decoding_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.pwr_uci_slot_pusch_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.pwr_uci_subslot_pusch_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.slot_pdsch_tx_div_tm9and10_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.subslot_pdsch_tx_div_tm9and10_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.spdcch_different_rs_types_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.srs_dci7_trigger_fs2_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.sps_cyclic_shift_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.spdcch_reuse_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.sps_stti_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.tm8_slot_pdsch_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.tm9_slot_subslot_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.tm9_slot_subslot_mbsfn_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.tm10_slot_subslot_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.tm10_slot_subslot_mbsfn_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.tx_div_spucch_r15_present, 1));
    HANDLE_CODE(bref.pack(stti_spt_cap_r15.ul_async_harq_sharing_diff_tti_lens_r15_present, 1));
    if (stti_spt_cap_r15.max_layers_slot_or_subslot_pusch_r15_present) {
      HANDLE_CODE(stti_spt_cap_r15.max_layers_slot_or_subslot_pusch_r15.pack(bref));
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_spt_r15_present) {
      HANDLE_CODE(pack_integer(bref, stti_spt_cap_r15.max_num_upd_csi_proc_spt_r15, (uint8_t)5u, (uint8_t)32u));
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb77_r15_present) {
      HANDLE_CODE(pack_integer(bref, stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb77_r15, (uint8_t)1u, (uint8_t)32u));
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb27_r15_present) {
      HANDLE_CODE(pack_integer(bref, stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb27_r15, (uint8_t)1u, (uint8_t)32u));
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set1_r15_present) {
      HANDLE_CODE(
          pack_integer(bref, stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set1_r15, (uint8_t)1u, (uint8_t)32u));
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set2_r15_present) {
      HANDLE_CODE(
          pack_integer(bref, stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set2_r15, (uint8_t)1u, (uint8_t)32u));
    }
    if (stti_spt_cap_r15.mimo_ue_params_stti_r15_present) {
      HANDLE_CODE(stti_spt_cap_r15.mimo_ue_params_stti_r15.pack(bref));
    }
    if (stti_spt_cap_r15.mimo_ue_params_stti_v1530_present) {
      HANDLE_CODE(stti_spt_cap_r15.mimo_ue_params_stti_v1530.pack(bref));
    }
    if (stti_spt_cap_r15.nof_blind_decodes_uss_r15_present) {
      HANDLE_CODE(pack_integer(bref, stti_spt_cap_r15.nof_blind_decodes_uss_r15, (uint8_t)4u, (uint8_t)32u));
    }
    if (stti_spt_cap_r15.sps_stti_r15_present) {
      HANDLE_CODE(stti_spt_cap_r15.sps_stti_r15.pack(bref));
    }
  }
  if (ce_cap_r15_present) {
    HANDLE_CODE(bref.pack(ce_cap_r15.ce_crs_intf_mitig_r15_present, 1));
    HANDLE_CODE(bref.pack(ce_cap_r15.ce_cqi_alt_table_r15_present, 1));
    HANDLE_CODE(bref.pack(ce_cap_r15.ce_pdsch_flex_start_prb_ce_mode_a_r15_present, 1));
    HANDLE_CODE(bref.pack(ce_cap_r15.ce_pdsch_flex_start_prb_ce_mode_b_r15_present, 1));
    HANDLE_CODE(bref.pack(ce_cap_r15.ce_pdsch_minus64_qam_r15_present, 1));
    HANDLE_CODE(bref.pack(ce_cap_r15.ce_pusch_flex_start_prb_ce_mode_a_r15_present, 1));
    HANDLE_CODE(bref.pack(ce_cap_r15.ce_pusch_flex_start_prb_ce_mode_b_r15_present, 1));
    HANDLE_CODE(bref.pack(ce_cap_r15.ce_pusch_sub_prb_alloc_r15_present, 1));
    HANDLE_CODE(bref.pack(ce_cap_r15.ce_ul_harq_ack_feedback_r15_present, 1));
  }
  if (urllc_cap_r15_present) {
    HANDLE_CODE(bref.pack(urllc_cap_r15.pdsch_rep_sf_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pdsch_rep_slot_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pdsch_rep_subslot_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_multi_cfg_sf_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_max_cfg_sf_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_multi_cfg_slot_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_max_cfg_slot_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_multi_cfg_subslot_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_max_cfg_subslot_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_slot_rep_pcell_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_slot_rep_ps_cell_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_slot_rep_scell_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_sf_rep_pcell_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_sf_rep_ps_cell_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_sf_rep_scell_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_subslot_rep_pcell_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_subslot_rep_ps_cell_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.pusch_sps_subslot_rep_scell_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.semi_static_cfi_r15_present, 1));
    HANDLE_CODE(bref.pack(urllc_cap_r15.semi_static_cfi_pattern_r15_present, 1));
    if (urllc_cap_r15.pusch_sps_multi_cfg_sf_r15_present) {
      HANDLE_CODE(pack_integer(bref, urllc_cap_r15.pusch_sps_multi_cfg_sf_r15, (uint8_t)0u, (uint8_t)6u));
    }
    if (urllc_cap_r15.pusch_sps_max_cfg_sf_r15_present) {
      HANDLE_CODE(pack_integer(bref, urllc_cap_r15.pusch_sps_max_cfg_sf_r15, (uint8_t)0u, (uint8_t)31u));
    }
    if (urllc_cap_r15.pusch_sps_multi_cfg_slot_r15_present) {
      HANDLE_CODE(pack_integer(bref, urllc_cap_r15.pusch_sps_multi_cfg_slot_r15, (uint8_t)0u, (uint8_t)6u));
    }
    if (urllc_cap_r15.pusch_sps_max_cfg_slot_r15_present) {
      HANDLE_CODE(pack_integer(bref, urllc_cap_r15.pusch_sps_max_cfg_slot_r15, (uint8_t)0u, (uint8_t)31u));
    }
    if (urllc_cap_r15.pusch_sps_multi_cfg_subslot_r15_present) {
      HANDLE_CODE(pack_integer(bref, urllc_cap_r15.pusch_sps_multi_cfg_subslot_r15, (uint8_t)0u, (uint8_t)6u));
    }
    if (urllc_cap_r15.pusch_sps_max_cfg_subslot_r15_present) {
      HANDLE_CODE(pack_integer(bref, urllc_cap_r15.pusch_sps_max_cfg_subslot_r15, (uint8_t)0u, (uint8_t)31u));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(stti_spt_cap_r15_present, 1));
  HANDLE_CODE(bref.unpack(ce_cap_r15_present, 1));
  HANDLE_CODE(bref.unpack(short_cqi_for_scell_activation_r15_present, 1));
  HANDLE_CODE(bref.unpack(mimo_cbsr_advanced_csi_r15_present, 1));
  HANDLE_CODE(bref.unpack(crs_intf_mitig_r15_present, 1));
  HANDLE_CODE(bref.unpack(ul_pwr_ctrl_enhance_r15_present, 1));
  HANDLE_CODE(bref.unpack(urllc_cap_r15_present, 1));
  HANDLE_CODE(bref.unpack(alt_mcs_table_r15_present, 1));

  if (stti_spt_cap_r15_present) {
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.aperiodic_csi_report_stti_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.dmrs_based_spdcch_mbsfn_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.dmrs_based_spdcch_non_mbsfn_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.dmrs_position_pattern_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.dmrs_sharing_subslot_pdsch_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.dmrs_repeat_subslot_pdsch_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.epdcch_spt_different_cells_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.epdcch_stti_different_cells_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.max_layers_slot_or_subslot_pusch_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.max_num_upd_csi_proc_spt_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb77_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb27_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set1_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set2_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.mimo_ue_params_stti_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.mimo_ue_params_stti_v1530_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.nof_blind_decodes_uss_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.pdsch_slot_subslot_pdsch_decoding_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.pwr_uci_slot_pusch_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.pwr_uci_subslot_pusch_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.slot_pdsch_tx_div_tm9and10_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.subslot_pdsch_tx_div_tm9and10_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.spdcch_different_rs_types_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.srs_dci7_trigger_fs2_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.sps_cyclic_shift_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.spdcch_reuse_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.sps_stti_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.tm8_slot_pdsch_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.tm9_slot_subslot_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.tm9_slot_subslot_mbsfn_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.tm10_slot_subslot_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.tm10_slot_subslot_mbsfn_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.tx_div_spucch_r15_present, 1));
    HANDLE_CODE(bref.unpack(stti_spt_cap_r15.ul_async_harq_sharing_diff_tti_lens_r15_present, 1));
    if (stti_spt_cap_r15.max_layers_slot_or_subslot_pusch_r15_present) {
      HANDLE_CODE(stti_spt_cap_r15.max_layers_slot_or_subslot_pusch_r15.unpack(bref));
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_spt_r15_present) {
      HANDLE_CODE(unpack_integer(stti_spt_cap_r15.max_num_upd_csi_proc_spt_r15, bref, (uint8_t)5u, (uint8_t)32u));
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb77_r15_present) {
      HANDLE_CODE(
          unpack_integer(stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb77_r15, bref, (uint8_t)1u, (uint8_t)32u));
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb27_r15_present) {
      HANDLE_CODE(
          unpack_integer(stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb27_r15, bref, (uint8_t)1u, (uint8_t)32u));
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set1_r15_present) {
      HANDLE_CODE(
          unpack_integer(stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set1_r15, bref, (uint8_t)1u, (uint8_t)32u));
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set2_r15_present) {
      HANDLE_CODE(
          unpack_integer(stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set2_r15, bref, (uint8_t)1u, (uint8_t)32u));
    }
    if (stti_spt_cap_r15.mimo_ue_params_stti_r15_present) {
      HANDLE_CODE(stti_spt_cap_r15.mimo_ue_params_stti_r15.unpack(bref));
    }
    if (stti_spt_cap_r15.mimo_ue_params_stti_v1530_present) {
      HANDLE_CODE(stti_spt_cap_r15.mimo_ue_params_stti_v1530.unpack(bref));
    }
    if (stti_spt_cap_r15.nof_blind_decodes_uss_r15_present) {
      HANDLE_CODE(unpack_integer(stti_spt_cap_r15.nof_blind_decodes_uss_r15, bref, (uint8_t)4u, (uint8_t)32u));
    }
    if (stti_spt_cap_r15.sps_stti_r15_present) {
      HANDLE_CODE(stti_spt_cap_r15.sps_stti_r15.unpack(bref));
    }
  }
  if (ce_cap_r15_present) {
    HANDLE_CODE(bref.unpack(ce_cap_r15.ce_crs_intf_mitig_r15_present, 1));
    HANDLE_CODE(bref.unpack(ce_cap_r15.ce_cqi_alt_table_r15_present, 1));
    HANDLE_CODE(bref.unpack(ce_cap_r15.ce_pdsch_flex_start_prb_ce_mode_a_r15_present, 1));
    HANDLE_CODE(bref.unpack(ce_cap_r15.ce_pdsch_flex_start_prb_ce_mode_b_r15_present, 1));
    HANDLE_CODE(bref.unpack(ce_cap_r15.ce_pdsch_minus64_qam_r15_present, 1));
    HANDLE_CODE(bref.unpack(ce_cap_r15.ce_pusch_flex_start_prb_ce_mode_a_r15_present, 1));
    HANDLE_CODE(bref.unpack(ce_cap_r15.ce_pusch_flex_start_prb_ce_mode_b_r15_present, 1));
    HANDLE_CODE(bref.unpack(ce_cap_r15.ce_pusch_sub_prb_alloc_r15_present, 1));
    HANDLE_CODE(bref.unpack(ce_cap_r15.ce_ul_harq_ack_feedback_r15_present, 1));
  }
  if (urllc_cap_r15_present) {
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pdsch_rep_sf_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pdsch_rep_slot_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pdsch_rep_subslot_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_multi_cfg_sf_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_max_cfg_sf_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_multi_cfg_slot_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_max_cfg_slot_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_multi_cfg_subslot_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_max_cfg_subslot_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_slot_rep_pcell_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_slot_rep_ps_cell_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_slot_rep_scell_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_sf_rep_pcell_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_sf_rep_ps_cell_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_sf_rep_scell_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_subslot_rep_pcell_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_subslot_rep_ps_cell_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.pusch_sps_subslot_rep_scell_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.semi_static_cfi_r15_present, 1));
    HANDLE_CODE(bref.unpack(urllc_cap_r15.semi_static_cfi_pattern_r15_present, 1));
    if (urllc_cap_r15.pusch_sps_multi_cfg_sf_r15_present) {
      HANDLE_CODE(unpack_integer(urllc_cap_r15.pusch_sps_multi_cfg_sf_r15, bref, (uint8_t)0u, (uint8_t)6u));
    }
    if (urllc_cap_r15.pusch_sps_max_cfg_sf_r15_present) {
      HANDLE_CODE(unpack_integer(urllc_cap_r15.pusch_sps_max_cfg_sf_r15, bref, (uint8_t)0u, (uint8_t)31u));
    }
    if (urllc_cap_r15.pusch_sps_multi_cfg_slot_r15_present) {
      HANDLE_CODE(unpack_integer(urllc_cap_r15.pusch_sps_multi_cfg_slot_r15, bref, (uint8_t)0u, (uint8_t)6u));
    }
    if (urllc_cap_r15.pusch_sps_max_cfg_slot_r15_present) {
      HANDLE_CODE(unpack_integer(urllc_cap_r15.pusch_sps_max_cfg_slot_r15, bref, (uint8_t)0u, (uint8_t)31u));
    }
    if (urllc_cap_r15.pusch_sps_multi_cfg_subslot_r15_present) {
      HANDLE_CODE(unpack_integer(urllc_cap_r15.pusch_sps_multi_cfg_subslot_r15, bref, (uint8_t)0u, (uint8_t)6u));
    }
    if (urllc_cap_r15.pusch_sps_max_cfg_subslot_r15_present) {
      HANDLE_CODE(unpack_integer(urllc_cap_r15.pusch_sps_max_cfg_subslot_r15, bref, (uint8_t)0u, (uint8_t)31u));
    }
  }

  return SRSASN_SUCCESS;
}
void phy_layer_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (stti_spt_cap_r15_present) {
    j.write_fieldname("stti-SPT-Capabilities-r15");
    j.start_obj();
    if (stti_spt_cap_r15.aperiodic_csi_report_stti_r15_present) {
      j.write_str("aperiodicCsi-ReportingSTTI-r15", "supported");
    }
    if (stti_spt_cap_r15.dmrs_based_spdcch_mbsfn_r15_present) {
      j.write_str("dmrs-BasedSPDCCH-MBSFN-r15", "supported");
    }
    if (stti_spt_cap_r15.dmrs_based_spdcch_non_mbsfn_r15_present) {
      j.write_str("dmrs-BasedSPDCCH-nonMBSFN-r15", "supported");
    }
    if (stti_spt_cap_r15.dmrs_position_pattern_r15_present) {
      j.write_str("dmrs-PositionPattern-r15", "supported");
    }
    if (stti_spt_cap_r15.dmrs_sharing_subslot_pdsch_r15_present) {
      j.write_str("dmrs-SharingSubslotPDSCH-r15", "supported");
    }
    if (stti_spt_cap_r15.dmrs_repeat_subslot_pdsch_r15_present) {
      j.write_str("dmrs-RepetitionSubslotPDSCH-r15", "supported");
    }
    if (stti_spt_cap_r15.epdcch_spt_different_cells_r15_present) {
      j.write_str("epdcch-SPT-differentCells-r15", "supported");
    }
    if (stti_spt_cap_r15.epdcch_stti_different_cells_r15_present) {
      j.write_str("epdcch-STTI-differentCells-r15", "supported");
    }
    if (stti_spt_cap_r15.max_layers_slot_or_subslot_pusch_r15_present) {
      j.write_str("maxLayersSlotOrSubslotPUSCH-r15", stti_spt_cap_r15.max_layers_slot_or_subslot_pusch_r15.to_string());
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_spt_r15_present) {
      j.write_int("maxNumberUpdatedCSI-Proc-SPT-r15", stti_spt_cap_r15.max_num_upd_csi_proc_spt_r15);
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb77_r15_present) {
      j.write_int("maxNumberUpdatedCSI-Proc-STTI-Comb77-r15", stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb77_r15);
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb27_r15_present) {
      j.write_int("maxNumberUpdatedCSI-Proc-STTI-Comb27-r15", stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb27_r15);
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set1_r15_present) {
      j.write_int("maxNumberUpdatedCSI-Proc-STTI-Comb22-Set1-r15",
                  stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set1_r15);
    }
    if (stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set2_r15_present) {
      j.write_int("maxNumberUpdatedCSI-Proc-STTI-Comb22-Set2-r15",
                  stti_spt_cap_r15.max_num_upd_csi_proc_stti_comb22_set2_r15);
    }
    if (stti_spt_cap_r15.mimo_ue_params_stti_r15_present) {
      j.write_fieldname("mimo-UE-ParametersSTTI-r15");
      stti_spt_cap_r15.mimo_ue_params_stti_r15.to_json(j);
    }
    if (stti_spt_cap_r15.mimo_ue_params_stti_v1530_present) {
      j.write_fieldname("mimo-UE-ParametersSTTI-v1530");
      stti_spt_cap_r15.mimo_ue_params_stti_v1530.to_json(j);
    }
    if (stti_spt_cap_r15.nof_blind_decodes_uss_r15_present) {
      j.write_int("numberOfBlindDecodesUSS-r15", stti_spt_cap_r15.nof_blind_decodes_uss_r15);
    }
    if (stti_spt_cap_r15.pdsch_slot_subslot_pdsch_decoding_r15_present) {
      j.write_str("pdsch-SlotSubslotPDSCH-Decoding-r15", "supported");
    }
    if (stti_spt_cap_r15.pwr_uci_slot_pusch_present) {
      j.write_str("powerUCI-SlotPUSCH", "supported");
    }
    if (stti_spt_cap_r15.pwr_uci_subslot_pusch_present) {
      j.write_str("powerUCI-SubslotPUSCH", "supported");
    }
    if (stti_spt_cap_r15.slot_pdsch_tx_div_tm9and10_present) {
      j.write_str("slotPDSCH-TxDiv-TM9and10", "supported");
    }
    if (stti_spt_cap_r15.subslot_pdsch_tx_div_tm9and10_present) {
      j.write_str("subslotPDSCH-TxDiv-TM9and10", "supported");
    }
    if (stti_spt_cap_r15.spdcch_different_rs_types_r15_present) {
      j.write_str("spdcch-differentRS-types-r15", "supported");
    }
    if (stti_spt_cap_r15.srs_dci7_trigger_fs2_r15_present) {
      j.write_str("srs-DCI7-TriggeringFS2-r15", "supported");
    }
    if (stti_spt_cap_r15.sps_cyclic_shift_r15_present) {
      j.write_str("sps-cyclicShift-r15", "supported");
    }
    if (stti_spt_cap_r15.spdcch_reuse_r15_present) {
      j.write_str("spdcch-Reuse-r15", "supported");
    }
    if (stti_spt_cap_r15.sps_stti_r15_present) {
      j.write_str("sps-STTI-r15", stti_spt_cap_r15.sps_stti_r15.to_string());
    }
    if (stti_spt_cap_r15.tm8_slot_pdsch_r15_present) {
      j.write_str("tm8-slotPDSCH-r15", "supported");
    }
    if (stti_spt_cap_r15.tm9_slot_subslot_r15_present) {
      j.write_str("tm9-slotSubslot-r15", "supported");
    }
    if (stti_spt_cap_r15.tm9_slot_subslot_mbsfn_r15_present) {
      j.write_str("tm9-slotSubslotMBSFN-r15", "supported");
    }
    if (stti_spt_cap_r15.tm10_slot_subslot_r15_present) {
      j.write_str("tm10-slotSubslot-r15", "supported");
    }
    if (stti_spt_cap_r15.tm10_slot_subslot_mbsfn_r15_present) {
      j.write_str("tm10-slotSubslotMBSFN-r15", "supported");
    }
    if (stti_spt_cap_r15.tx_div_spucch_r15_present) {
      j.write_str("txDiv-SPUCCH-r15", "supported");
    }
    if (stti_spt_cap_r15.ul_async_harq_sharing_diff_tti_lens_r15_present) {
      j.write_str("ul-AsyncHarqSharingDiff-TTI-Lengths-r15", "supported");
    }
    j.end_obj();
  }
  if (ce_cap_r15_present) {
    j.write_fieldname("ce-Capabilities-r15");
    j.start_obj();
    if (ce_cap_r15.ce_crs_intf_mitig_r15_present) {
      j.write_str("ce-CRS-IntfMitig-r15", "supported");
    }
    if (ce_cap_r15.ce_cqi_alt_table_r15_present) {
      j.write_str("ce-CQI-AlternativeTable-r15", "supported");
    }
    if (ce_cap_r15.ce_pdsch_flex_start_prb_ce_mode_a_r15_present) {
      j.write_str("ce-PDSCH-FlexibleStartPRB-CE-ModeA-r15", "supported");
    }
    if (ce_cap_r15.ce_pdsch_flex_start_prb_ce_mode_b_r15_present) {
      j.write_str("ce-PDSCH-FlexibleStartPRB-CE-ModeB-r15", "supported");
    }
    if (ce_cap_r15.ce_pdsch_minus64_qam_r15_present) {
      j.write_str("ce-PDSCH-64QAM-r15", "supported");
    }
    if (ce_cap_r15.ce_pusch_flex_start_prb_ce_mode_a_r15_present) {
      j.write_str("ce-PUSCH-FlexibleStartPRB-CE-ModeA-r15", "supported");
    }
    if (ce_cap_r15.ce_pusch_flex_start_prb_ce_mode_b_r15_present) {
      j.write_str("ce-PUSCH-FlexibleStartPRB-CE-ModeB-r15", "supported");
    }
    if (ce_cap_r15.ce_pusch_sub_prb_alloc_r15_present) {
      j.write_str("ce-PUSCH-SubPRB-Allocation-r15", "supported");
    }
    if (ce_cap_r15.ce_ul_harq_ack_feedback_r15_present) {
      j.write_str("ce-UL-HARQ-ACK-Feedback-r15", "supported");
    }
    j.end_obj();
  }
  if (short_cqi_for_scell_activation_r15_present) {
    j.write_str("shortCQI-ForSCellActivation-r15", "supported");
  }
  if (mimo_cbsr_advanced_csi_r15_present) {
    j.write_str("mimo-CBSR-AdvancedCSI-r15", "supported");
  }
  if (crs_intf_mitig_r15_present) {
    j.write_str("crs-IntfMitig-r15", "supported");
  }
  if (ul_pwr_ctrl_enhance_r15_present) {
    j.write_str("ul-PowerControlEnhancements-r15", "supported");
  }
  if (urllc_cap_r15_present) {
    j.write_fieldname("urllc-Capabilities-r15");
    j.start_obj();
    if (urllc_cap_r15.pdsch_rep_sf_r15_present) {
      j.write_str("pdsch-RepSubframe-r15", "supported");
    }
    if (urllc_cap_r15.pdsch_rep_slot_r15_present) {
      j.write_str("pdsch-RepSlot-r15", "supported");
    }
    if (urllc_cap_r15.pdsch_rep_subslot_r15_present) {
      j.write_str("pdsch-RepSubslot-r15", "supported");
    }
    if (urllc_cap_r15.pusch_sps_multi_cfg_sf_r15_present) {
      j.write_int("pusch-SPS-MultiConfigSubframe-r15", urllc_cap_r15.pusch_sps_multi_cfg_sf_r15);
    }
    if (urllc_cap_r15.pusch_sps_max_cfg_sf_r15_present) {
      j.write_int("pusch-SPS-MaxConfigSubframe-r15", urllc_cap_r15.pusch_sps_max_cfg_sf_r15);
    }
    if (urllc_cap_r15.pusch_sps_multi_cfg_slot_r15_present) {
      j.write_int("pusch-SPS-MultiConfigSlot-r15", urllc_cap_r15.pusch_sps_multi_cfg_slot_r15);
    }
    if (urllc_cap_r15.pusch_sps_max_cfg_slot_r15_present) {
      j.write_int("pusch-SPS-MaxConfigSlot-r15", urllc_cap_r15.pusch_sps_max_cfg_slot_r15);
    }
    if (urllc_cap_r15.pusch_sps_multi_cfg_subslot_r15_present) {
      j.write_int("pusch-SPS-MultiConfigSubslot-r15", urllc_cap_r15.pusch_sps_multi_cfg_subslot_r15);
    }
    if (urllc_cap_r15.pusch_sps_max_cfg_subslot_r15_present) {
      j.write_int("pusch-SPS-MaxConfigSubslot-r15", urllc_cap_r15.pusch_sps_max_cfg_subslot_r15);
    }
    if (urllc_cap_r15.pusch_sps_slot_rep_pcell_r15_present) {
      j.write_str("pusch-SPS-SlotRepPCell-r15", "supported");
    }
    if (urllc_cap_r15.pusch_sps_slot_rep_ps_cell_r15_present) {
      j.write_str("pusch-SPS-SlotRepPSCell-r15", "supported");
    }
    if (urllc_cap_r15.pusch_sps_slot_rep_scell_r15_present) {
      j.write_str("pusch-SPS-SlotRepSCell-r15", "supported");
    }
    if (urllc_cap_r15.pusch_sps_sf_rep_pcell_r15_present) {
      j.write_str("pusch-SPS-SubframeRepPCell-r15", "supported");
    }
    if (urllc_cap_r15.pusch_sps_sf_rep_ps_cell_r15_present) {
      j.write_str("pusch-SPS-SubframeRepPSCell-r15", "supported");
    }
    if (urllc_cap_r15.pusch_sps_sf_rep_scell_r15_present) {
      j.write_str("pusch-SPS-SubframeRepSCell-r15", "supported");
    }
    if (urllc_cap_r15.pusch_sps_subslot_rep_pcell_r15_present) {
      j.write_str("pusch-SPS-SubslotRepPCell-r15", "supported");
    }
    if (urllc_cap_r15.pusch_sps_subslot_rep_ps_cell_r15_present) {
      j.write_str("pusch-SPS-SubslotRepPSCell-r15", "supported");
    }
    if (urllc_cap_r15.pusch_sps_subslot_rep_scell_r15_present) {
      j.write_str("pusch-SPS-SubslotRepSCell-r15", "supported");
    }
    if (urllc_cap_r15.semi_static_cfi_r15_present) {
      j.write_str("semiStaticCFI-r15", "supported");
    }
    if (urllc_cap_r15.semi_static_cfi_pattern_r15_present) {
      j.write_str("semiStaticCFI-Pattern-r15", "supported");
    }
    j.end_obj();
  }
  if (alt_mcs_table_r15_present) {
    j.write_str("altMCS-Table-r15", "supported");
  }
  j.end_obj();
}

const char* phy_layer_params_v1530_s::stti_spt_cap_r15_s_::max_layers_slot_or_subslot_pusch_r15_opts::to_string() const
{
  static const char* options[] = {"oneLayer", "twoLayers", "fourLayers"};
  return convert_enum_idx(
      options, 3, value, "phy_layer_params_v1530_s::stti_spt_cap_r15_s_::max_layers_slot_or_subslot_pusch_r15_e_");
}
uint8_t phy_layer_params_v1530_s::stti_spt_cap_r15_s_::max_layers_slot_or_subslot_pusch_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(
      options, 3, value, "phy_layer_params_v1530_s::stti_spt_cap_r15_s_::max_layers_slot_or_subslot_pusch_r15_e_");
}

const char* phy_layer_params_v1530_s::stti_spt_cap_r15_s_::sps_stti_r15_opts::to_string() const
{
  static const char* options[] = {"slot", "subslot", "slotAndSubslot"};
  return convert_enum_idx(options, 3, value, "phy_layer_params_v1530_s::stti_spt_cap_r15_s_::sps_stti_r15_e_");
}

// SupportedBandEUTRA ::= SEQUENCE
SRSASN_CODE supported_band_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, band_eutra, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(bref.pack(half_duplex, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_band_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(band_eutra, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(bref.unpack(half_duplex, 1));

  return SRSASN_SUCCESS;
}
void supported_band_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandEUTRA", band_eutra);
  j.write_bool("halfDuplex", half_duplex);
  j.end_obj();
}

// RF-Parameters ::= SEQUENCE
SRSASN_CODE rf_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_eutra, 1, 64));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_eutra, bref, 1, 64));

  return SRSASN_SUCCESS;
}
void rf_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandListEUTRA");
  for (const auto& e1 : supported_band_list_eutra) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// RF-Parameters-v1020 ::= SEQUENCE
SRSASN_CODE rf_params_v1020_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_r10, 1, 128, SeqOfPacker<Packer>(1, 64, Packer())));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1020_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_r10, bref, 1, 128, SeqOfPacker<Packer>(1, 64, Packer())));

  return SRSASN_SUCCESS;
}
void rf_params_v1020_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandCombination-r10");
  for (const auto& e1 : supported_band_combination_r10) {
    j.start_array();
    for (const auto& e2 : e1) {
      e2.to_json(j);
    }
    j.end_array();
  }
  j.end_array();
  j.end_obj();
}

// RF-Parameters-v1060 ::= SEQUENCE
SRSASN_CODE rf_params_v1060_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_ext_r10, 1, 128));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1060_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_ext_r10, bref, 1, 128));

  return SRSASN_SUCCESS;
}
void rf_params_v1060_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandCombinationExt-r10");
  for (const auto& e1 : supported_band_combination_ext_r10) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// RF-Parameters-v1090 ::= SEQUENCE
SRSASN_CODE rf_params_v1090_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_v1090_present, 1));

  if (supported_band_combination_v1090_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_v1090, 1, 128, SeqOfPacker<Packer>(1, 64, Packer())));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1090_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_combination_v1090_present, 1));

  if (supported_band_combination_v1090_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(supported_band_combination_v1090, bref, 1, 128, SeqOfPacker<Packer>(1, 64, Packer())));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v1090_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_v1090_present) {
    j.start_array("supportedBandCombination-v1090");
    for (const auto& e1 : supported_band_combination_v1090) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
      }
      j.end_array();
    }
    j.end_array();
  }
  j.end_obj();
}

// RF-Parameters-v10i0 ::= SEQUENCE
SRSASN_CODE rf_params_v10i0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_v10i0_present, 1));

  if (supported_band_combination_v10i0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_v10i0, 1, 128));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v10i0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_combination_v10i0_present, 1));

  if (supported_band_combination_v10i0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_v10i0, bref, 1, 128));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v10i0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_v10i0_present) {
    j.start_array("supportedBandCombination-v10i0");
    for (const auto& e1 : supported_band_combination_v10i0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// RF-Parameters-v1130 ::= SEQUENCE
SRSASN_CODE rf_params_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_v1130_present, 1));

  if (supported_band_combination_v1130_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_v1130, 1, 128));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_combination_v1130_present, 1));

  if (supported_band_combination_v1130_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_v1130, bref, 1, 128));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_v1130_present) {
    j.start_array("supportedBandCombination-v1130");
    for (const auto& e1 : supported_band_combination_v1130) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// RF-Parameters-v1180 ::= SEQUENCE
SRSASN_CODE rf_params_v1180_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_band_retrieval_r11_present, 1));
  HANDLE_CODE(bref.pack(requested_bands_r11_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_add_r11_present, 1));

  if (requested_bands_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, requested_bands_r11, 1, 64, integer_packer<uint16_t>(1, 256)));
  }
  if (supported_band_combination_add_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_add_r11, 1, 256));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1180_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(freq_band_retrieval_r11_present, 1));
  HANDLE_CODE(bref.unpack(requested_bands_r11_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_add_r11_present, 1));

  if (requested_bands_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(requested_bands_r11, bref, 1, 64, integer_packer<uint16_t>(1, 256)));
  }
  if (supported_band_combination_add_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_add_r11, bref, 1, 256));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v1180_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_retrieval_r11_present) {
    j.write_str("freqBandRetrieval-r11", "supported");
  }
  if (requested_bands_r11_present) {
    j.start_array("requestedBands-r11");
    for (const auto& e1 : requested_bands_r11) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (supported_band_combination_add_r11_present) {
    j.start_array("supportedBandCombinationAdd-r11");
    for (const auto& e1 : supported_band_combination_add_r11) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// RF-Parameters-v11d0 ::= SEQUENCE
SRSASN_CODE rf_params_v11d0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_add_v11d0_present, 1));

  if (supported_band_combination_add_v11d0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_add_v11d0, 1, 256));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v11d0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_combination_add_v11d0_present, 1));

  if (supported_band_combination_add_v11d0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_add_v11d0, bref, 1, 256));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v11d0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_add_v11d0_present) {
    j.start_array("supportedBandCombinationAdd-v11d0");
    for (const auto& e1 : supported_band_combination_add_v11d0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SupportedBandEUTRA-v1250 ::= SEQUENCE
SRSASN_CODE supported_band_eutra_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dl_minus256_qam_r12_present, 1));
  HANDLE_CODE(bref.pack(ul_minus64_qam_r12_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_band_eutra_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dl_minus256_qam_r12_present, 1));
  HANDLE_CODE(bref.unpack(ul_minus64_qam_r12_present, 1));

  return SRSASN_SUCCESS;
}
void supported_band_eutra_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_minus256_qam_r12_present) {
    j.write_str("dl-256QAM-r12", "supported");
  }
  if (ul_minus64_qam_r12_present) {
    j.write_str("ul-64QAM-r12", "supported");
  }
  j.end_obj();
}

// RF-Parameters-v1250 ::= SEQUENCE
SRSASN_CODE rf_params_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_list_eutra_v1250_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_v1250_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_add_v1250_present, 1));
  HANDLE_CODE(bref.pack(freq_band_prio_adjustment_r12_present, 1));

  if (supported_band_list_eutra_v1250_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_eutra_v1250, 1, 64));
  }
  if (supported_band_combination_v1250_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_v1250, 1, 128));
  }
  if (supported_band_combination_add_v1250_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_add_v1250, 1, 256));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_list_eutra_v1250_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_v1250_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_add_v1250_present, 1));
  HANDLE_CODE(bref.unpack(freq_band_prio_adjustment_r12_present, 1));

  if (supported_band_list_eutra_v1250_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_eutra_v1250, bref, 1, 64));
  }
  if (supported_band_combination_v1250_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_v1250, bref, 1, 128));
  }
  if (supported_band_combination_add_v1250_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_add_v1250, bref, 1, 256));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_list_eutra_v1250_present) {
    j.start_array("supportedBandListEUTRA-v1250");
    for (const auto& e1 : supported_band_list_eutra_v1250) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_v1250_present) {
    j.start_array("supportedBandCombination-v1250");
    for (const auto& e1 : supported_band_combination_v1250) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_add_v1250_present) {
    j.start_array("supportedBandCombinationAdd-v1250");
    for (const auto& e1 : supported_band_combination_add_v1250) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (freq_band_prio_adjustment_r12_present) {
    j.write_str("freqBandPriorityAdjustment-r12", "supported");
  }
  j.end_obj();
}

// RF-Parameters-v1270 ::= SEQUENCE
SRSASN_CODE rf_params_v1270_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_v1270_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_add_v1270_present, 1));

  if (supported_band_combination_v1270_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_v1270, 1, 128));
  }
  if (supported_band_combination_add_v1270_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_add_v1270, 1, 256));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1270_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_combination_v1270_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_add_v1270_present, 1));

  if (supported_band_combination_v1270_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_v1270, bref, 1, 128));
  }
  if (supported_band_combination_add_v1270_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_add_v1270, bref, 1, 256));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v1270_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_v1270_present) {
    j.start_array("supportedBandCombination-v1270");
    for (const auto& e1 : supported_band_combination_v1270) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_add_v1270_present) {
    j.start_array("supportedBandCombinationAdd-v1270");
    for (const auto& e1 : supported_band_combination_add_v1270) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SupportedBandEUTRA-v1310 ::= SEQUENCE
SRSASN_CODE supported_band_eutra_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_pwr_class_minus5_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_band_eutra_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_pwr_class_minus5_r13_present, 1));

  return SRSASN_SUCCESS;
}
void supported_band_eutra_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_pwr_class_minus5_r13_present) {
    j.write_str("ue-PowerClass-5-r13", "supported");
  }
  j.end_obj();
}

// RF-Parameters-v1310 ::= SEQUENCE
SRSASN_CODE rf_params_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(enb_requested_params_r13_present, 1));
  HANDLE_CODE(bref.pack(maximum_ccs_retrieval_r13_present, 1));
  HANDLE_CODE(bref.pack(skip_fallback_combinations_r13_present, 1));
  HANDLE_CODE(bref.pack(reduced_int_non_cont_comb_r13_present, 1));
  HANDLE_CODE(bref.pack(supported_band_list_eutra_v1310_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_reduced_r13_present, 1));

  if (enb_requested_params_r13_present) {
    HANDLE_CODE(bref.pack(enb_requested_params_r13.reduced_int_non_cont_comb_requested_r13_present, 1));
    HANDLE_CODE(bref.pack(enb_requested_params_r13.requested_ccs_dl_r13_present, 1));
    HANDLE_CODE(bref.pack(enb_requested_params_r13.requested_ccs_ul_r13_present, 1));
    HANDLE_CODE(bref.pack(enb_requested_params_r13.skip_fallback_comb_requested_r13_present, 1));
    if (enb_requested_params_r13.requested_ccs_dl_r13_present) {
      HANDLE_CODE(pack_integer(bref, enb_requested_params_r13.requested_ccs_dl_r13, (uint8_t)2u, (uint8_t)32u));
    }
    if (enb_requested_params_r13.requested_ccs_ul_r13_present) {
      HANDLE_CODE(pack_integer(bref, enb_requested_params_r13.requested_ccs_ul_r13, (uint8_t)2u, (uint8_t)32u));
    }
  }
  if (supported_band_list_eutra_v1310_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_eutra_v1310, 1, 64));
  }
  if (supported_band_combination_reduced_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_reduced_r13, 1, 384));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(enb_requested_params_r13_present, 1));
  HANDLE_CODE(bref.unpack(maximum_ccs_retrieval_r13_present, 1));
  HANDLE_CODE(bref.unpack(skip_fallback_combinations_r13_present, 1));
  HANDLE_CODE(bref.unpack(reduced_int_non_cont_comb_r13_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_list_eutra_v1310_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_reduced_r13_present, 1));

  if (enb_requested_params_r13_present) {
    HANDLE_CODE(bref.unpack(enb_requested_params_r13.reduced_int_non_cont_comb_requested_r13_present, 1));
    HANDLE_CODE(bref.unpack(enb_requested_params_r13.requested_ccs_dl_r13_present, 1));
    HANDLE_CODE(bref.unpack(enb_requested_params_r13.requested_ccs_ul_r13_present, 1));
    HANDLE_CODE(bref.unpack(enb_requested_params_r13.skip_fallback_comb_requested_r13_present, 1));
    if (enb_requested_params_r13.requested_ccs_dl_r13_present) {
      HANDLE_CODE(unpack_integer(enb_requested_params_r13.requested_ccs_dl_r13, bref, (uint8_t)2u, (uint8_t)32u));
    }
    if (enb_requested_params_r13.requested_ccs_ul_r13_present) {
      HANDLE_CODE(unpack_integer(enb_requested_params_r13.requested_ccs_ul_r13, bref, (uint8_t)2u, (uint8_t)32u));
    }
  }
  if (supported_band_list_eutra_v1310_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_eutra_v1310, bref, 1, 64));
  }
  if (supported_band_combination_reduced_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_reduced_r13, bref, 1, 384));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (enb_requested_params_r13_present) {
    j.write_fieldname("eNB-RequestedParameters-r13");
    j.start_obj();
    if (enb_requested_params_r13.reduced_int_non_cont_comb_requested_r13_present) {
      j.write_str("reducedIntNonContCombRequested-r13", "true");
    }
    if (enb_requested_params_r13.requested_ccs_dl_r13_present) {
      j.write_int("requestedCCsDL-r13", enb_requested_params_r13.requested_ccs_dl_r13);
    }
    if (enb_requested_params_r13.requested_ccs_ul_r13_present) {
      j.write_int("requestedCCsUL-r13", enb_requested_params_r13.requested_ccs_ul_r13);
    }
    if (enb_requested_params_r13.skip_fallback_comb_requested_r13_present) {
      j.write_str("skipFallbackCombRequested-r13", "true");
    }
    j.end_obj();
  }
  if (maximum_ccs_retrieval_r13_present) {
    j.write_str("maximumCCsRetrieval-r13", "supported");
  }
  if (skip_fallback_combinations_r13_present) {
    j.write_str("skipFallbackCombinations-r13", "supported");
  }
  if (reduced_int_non_cont_comb_r13_present) {
    j.write_str("reducedIntNonContComb-r13", "supported");
  }
  if (supported_band_list_eutra_v1310_present) {
    j.start_array("supportedBandListEUTRA-v1310");
    for (const auto& e1 : supported_band_list_eutra_v1310) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_reduced_r13_present) {
    j.start_array("supportedBandCombinationReduced-r13");
    for (const auto& e1 : supported_band_combination_reduced_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SupportedBandEUTRA-v1320 ::= SEQUENCE
SRSASN_CODE supported_band_eutra_v1320_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(intra_freq_ce_need_for_gaps_r13_present, 1));
  HANDLE_CODE(bref.pack(ue_pwr_class_n_r13_present, 1));

  if (ue_pwr_class_n_r13_present) {
    HANDLE_CODE(ue_pwr_class_n_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_band_eutra_v1320_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(intra_freq_ce_need_for_gaps_r13_present, 1));
  HANDLE_CODE(bref.unpack(ue_pwr_class_n_r13_present, 1));

  if (ue_pwr_class_n_r13_present) {
    HANDLE_CODE(ue_pwr_class_n_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void supported_band_eutra_v1320_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (intra_freq_ce_need_for_gaps_r13_present) {
    j.write_str("intraFreq-CE-NeedForGaps-r13", "supported");
  }
  if (ue_pwr_class_n_r13_present) {
    j.write_str("ue-PowerClass-N-r13", ue_pwr_class_n_r13.to_string());
  }
  j.end_obj();
}

const char* supported_band_eutra_v1320_s::ue_pwr_class_n_r13_opts::to_string() const
{
  static const char* options[] = {"class1", "class2", "class4"};
  return convert_enum_idx(options, 3, value, "supported_band_eutra_v1320_s::ue_pwr_class_n_r13_e_");
}
uint8_t supported_band_eutra_v1320_s::ue_pwr_class_n_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "supported_band_eutra_v1320_s::ue_pwr_class_n_r13_e_");
}

// RF-Parameters-v1320 ::= SEQUENCE
SRSASN_CODE rf_params_v1320_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_list_eutra_v1320_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_v1320_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_add_v1320_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_reduced_v1320_present, 1));

  if (supported_band_list_eutra_v1320_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_eutra_v1320, 1, 64));
  }
  if (supported_band_combination_v1320_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_v1320, 1, 128));
  }
  if (supported_band_combination_add_v1320_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_add_v1320, 1, 256));
  }
  if (supported_band_combination_reduced_v1320_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_reduced_v1320, 1, 384));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1320_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_list_eutra_v1320_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_v1320_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_add_v1320_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_reduced_v1320_present, 1));

  if (supported_band_list_eutra_v1320_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_eutra_v1320, bref, 1, 64));
  }
  if (supported_band_combination_v1320_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_v1320, bref, 1, 128));
  }
  if (supported_band_combination_add_v1320_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_add_v1320, bref, 1, 256));
  }
  if (supported_band_combination_reduced_v1320_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_reduced_v1320, bref, 1, 384));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v1320_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_list_eutra_v1320_present) {
    j.start_array("supportedBandListEUTRA-v1320");
    for (const auto& e1 : supported_band_list_eutra_v1320) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_v1320_present) {
    j.start_array("supportedBandCombination-v1320");
    for (const auto& e1 : supported_band_combination_v1320) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_add_v1320_present) {
    j.start_array("supportedBandCombinationAdd-v1320");
    for (const auto& e1 : supported_band_combination_add_v1320) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_reduced_v1320_present) {
    j.start_array("supportedBandCombinationReduced-v1320");
    for (const auto& e1 : supported_band_combination_reduced_v1320) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// RF-Parameters-v1380 ::= SEQUENCE
SRSASN_CODE rf_params_v1380_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_v1380_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_add_v1380_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_reduced_v1380_present, 1));

  if (supported_band_combination_v1380_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_v1380, 1, 128));
  }
  if (supported_band_combination_add_v1380_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_add_v1380, 1, 256));
  }
  if (supported_band_combination_reduced_v1380_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_reduced_v1380, 1, 384));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1380_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_combination_v1380_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_add_v1380_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_reduced_v1380_present, 1));

  if (supported_band_combination_v1380_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_v1380, bref, 1, 128));
  }
  if (supported_band_combination_add_v1380_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_add_v1380, bref, 1, 256));
  }
  if (supported_band_combination_reduced_v1380_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_reduced_v1380, bref, 1, 384));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v1380_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_v1380_present) {
    j.start_array("supportedBandCombination-v1380");
    for (const auto& e1 : supported_band_combination_v1380) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_add_v1380_present) {
    j.start_array("supportedBandCombinationAdd-v1380");
    for (const auto& e1 : supported_band_combination_add_v1380) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_reduced_v1380_present) {
    j.start_array("supportedBandCombinationReduced-v1380");
    for (const auto& e1 : supported_band_combination_reduced_v1380) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// BandCombinationParameters-v1390 ::= SEQUENCE
SRSASN_CODE band_combination_params_v1390_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_ca_pwr_class_n_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_params_v1390_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_ca_pwr_class_n_r13_present, 1));

  return SRSASN_SUCCESS;
}
void band_combination_params_v1390_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_ca_pwr_class_n_r13_present) {
    j.write_str("ue-CA-PowerClass-N-r13", "class2");
  }
  j.end_obj();
}

// RF-Parameters-v1390 ::= SEQUENCE
SRSASN_CODE rf_params_v1390_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_v1390_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_add_v1390_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_reduced_v1390_present, 1));

  if (supported_band_combination_v1390_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_v1390, 1, 128));
  }
  if (supported_band_combination_add_v1390_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_add_v1390, 1, 256));
  }
  if (supported_band_combination_reduced_v1390_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_reduced_v1390, 1, 384));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1390_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_combination_v1390_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_add_v1390_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_reduced_v1390_present, 1));

  if (supported_band_combination_v1390_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_v1390, bref, 1, 128));
  }
  if (supported_band_combination_add_v1390_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_add_v1390, bref, 1, 256));
  }
  if (supported_band_combination_reduced_v1390_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_reduced_v1390, bref, 1, 384));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v1390_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_v1390_present) {
    j.start_array("supportedBandCombination-v1390");
    for (const auto& e1 : supported_band_combination_v1390) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_add_v1390_present) {
    j.start_array("supportedBandCombinationAdd-v1390");
    for (const auto& e1 : supported_band_combination_add_v1390) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_reduced_v1390_present) {
    j.start_array("supportedBandCombinationReduced-v1390");
    for (const auto& e1 : supported_band_combination_reduced_v1390) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// RF-Parameters-v1430 ::= SEQUENCE
SRSASN_CODE rf_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_v1430_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_add_v1430_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_reduced_v1430_present, 1));
  HANDLE_CODE(bref.pack(enb_requested_params_v1430_present, 1));
  HANDLE_CODE(bref.pack(diff_fallback_comb_report_r14_present, 1));

  if (supported_band_combination_v1430_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_v1430, 1, 128));
  }
  if (supported_band_combination_add_v1430_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_add_v1430, 1, 256));
  }
  if (supported_band_combination_reduced_v1430_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_reduced_v1430, 1, 384));
  }
  if (enb_requested_params_v1430_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref,
                                enb_requested_params_v1430.requested_diff_fallback_comb_list_r14,
                                1,
                                384,
                                SeqOfPacker<Packer>(1, 64, Packer())));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_combination_v1430_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_add_v1430_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_reduced_v1430_present, 1));
  HANDLE_CODE(bref.unpack(enb_requested_params_v1430_present, 1));
  HANDLE_CODE(bref.unpack(diff_fallback_comb_report_r14_present, 1));

  if (supported_band_combination_v1430_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_v1430, bref, 1, 128));
  }
  if (supported_band_combination_add_v1430_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_add_v1430, bref, 1, 256));
  }
  if (supported_band_combination_reduced_v1430_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_reduced_v1430, bref, 1, 384));
  }
  if (enb_requested_params_v1430_present) {
    HANDLE_CODE(unpack_dyn_seq_of(enb_requested_params_v1430.requested_diff_fallback_comb_list_r14,
                                  bref,
                                  1,
                                  384,
                                  SeqOfPacker<Packer>(1, 64, Packer())));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_v1430_present) {
    j.start_array("supportedBandCombination-v1430");
    for (const auto& e1 : supported_band_combination_v1430) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_add_v1430_present) {
    j.start_array("supportedBandCombinationAdd-v1430");
    for (const auto& e1 : supported_band_combination_add_v1430) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_reduced_v1430_present) {
    j.start_array("supportedBandCombinationReduced-v1430");
    for (const auto& e1 : supported_band_combination_reduced_v1430) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (enb_requested_params_v1430_present) {
    j.write_fieldname("eNB-RequestedParameters-v1430");
    j.start_obj();
    j.start_array("requestedDiffFallbackCombList-r14");
    for (const auto& e1 : enb_requested_params_v1430.requested_diff_fallback_comb_list_r14) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
      }
      j.end_array();
    }
    j.end_array();
    j.end_obj();
  }
  if (diff_fallback_comb_report_r14_present) {
    j.write_str("diffFallbackCombReport-r14", "supported");
  }
  j.end_obj();
}

// RF-Parameters-v1450 ::= SEQUENCE
SRSASN_CODE rf_params_v1450_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_v1450_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_add_v1450_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_reduced_v1450_present, 1));

  if (supported_band_combination_v1450_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_v1450, 1, 128));
  }
  if (supported_band_combination_add_v1450_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_add_v1450, 1, 256));
  }
  if (supported_band_combination_reduced_v1450_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_reduced_v1450, 1, 384));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1450_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_combination_v1450_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_add_v1450_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_reduced_v1450_present, 1));

  if (supported_band_combination_v1450_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_v1450, bref, 1, 128));
  }
  if (supported_band_combination_add_v1450_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_add_v1450, bref, 1, 256));
  }
  if (supported_band_combination_reduced_v1450_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_reduced_v1450, bref, 1, 384));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v1450_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_v1450_present) {
    j.start_array("supportedBandCombination-v1450");
    for (const auto& e1 : supported_band_combination_v1450) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_add_v1450_present) {
    j.start_array("supportedBandCombinationAdd-v1450");
    for (const auto& e1 : supported_band_combination_add_v1450) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_reduced_v1450_present) {
    j.start_array("supportedBandCombinationReduced-v1450");
    for (const auto& e1 : supported_band_combination_reduced_v1450) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// RF-Parameters-v1470 ::= SEQUENCE
SRSASN_CODE rf_params_v1470_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_v1470_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_add_v1470_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_reduced_v1470_present, 1));

  if (supported_band_combination_v1470_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_v1470, 1, 128));
  }
  if (supported_band_combination_add_v1470_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_add_v1470, 1, 256));
  }
  if (supported_band_combination_reduced_v1470_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_reduced_v1470, 1, 384));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1470_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_combination_v1470_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_add_v1470_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_reduced_v1470_present, 1));

  if (supported_band_combination_v1470_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_v1470, bref, 1, 128));
  }
  if (supported_band_combination_add_v1470_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_add_v1470, bref, 1, 256));
  }
  if (supported_band_combination_reduced_v1470_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_reduced_v1470, bref, 1, 384));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v1470_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_v1470_present) {
    j.start_array("supportedBandCombination-v1470");
    for (const auto& e1 : supported_band_combination_v1470) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_add_v1470_present) {
    j.start_array("supportedBandCombinationAdd-v1470");
    for (const auto& e1 : supported_band_combination_add_v1470) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_reduced_v1470_present) {
    j.start_array("supportedBandCombinationReduced-v1470");
    for (const auto& e1 : supported_band_combination_reduced_v1470) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// RF-Parameters-v14b0 ::= SEQUENCE
SRSASN_CODE rf_params_v14b0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_v14b0_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_add_v14b0_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_reduced_v14b0_present, 1));

  if (supported_band_combination_v14b0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_v14b0, 1, 128));
  }
  if (supported_band_combination_add_v14b0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_add_v14b0, 1, 256));
  }
  if (supported_band_combination_reduced_v14b0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_reduced_v14b0, 1, 384));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v14b0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_combination_v14b0_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_add_v14b0_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_reduced_v14b0_present, 1));

  if (supported_band_combination_v14b0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_v14b0, bref, 1, 128));
  }
  if (supported_band_combination_add_v14b0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_add_v14b0, bref, 1, 256));
  }
  if (supported_band_combination_reduced_v14b0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_reduced_v14b0, bref, 1, 384));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v14b0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_v14b0_present) {
    j.start_array("supportedBandCombination-v14b0");
    for (const auto& e1 : supported_band_combination_v14b0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_add_v14b0_present) {
    j.start_array("supportedBandCombinationAdd-v14b0");
    for (const auto& e1 : supported_band_combination_add_v14b0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_reduced_v14b0_present) {
    j.start_array("supportedBandCombinationReduced-v14b0");
    for (const auto& e1 : supported_band_combination_reduced_v14b0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// RF-Parameters-v1530 ::= SEQUENCE
SRSASN_CODE rf_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(stti_spt_supported_r15_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_v1530_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_add_v1530_present, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_reduced_v1530_present, 1));
  HANDLE_CODE(bref.pack(pwr_class_minus14dbm_r15_present, 1));

  if (supported_band_combination_v1530_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_v1530, 1, 128));
  }
  if (supported_band_combination_add_v1530_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_add_v1530, 1, 256));
  }
  if (supported_band_combination_reduced_v1530_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_reduced_v1530, 1, 384));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(stti_spt_supported_r15_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_v1530_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_add_v1530_present, 1));
  HANDLE_CODE(bref.unpack(supported_band_combination_reduced_v1530_present, 1));
  HANDLE_CODE(bref.unpack(pwr_class_minus14dbm_r15_present, 1));

  if (supported_band_combination_v1530_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_v1530, bref, 1, 128));
  }
  if (supported_band_combination_add_v1530_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_add_v1530, bref, 1, 256));
  }
  if (supported_band_combination_reduced_v1530_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_reduced_v1530, bref, 1, 384));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (stti_spt_supported_r15_present) {
    j.write_str("sTTI-SPT-Supported-r15", "supported");
  }
  if (supported_band_combination_v1530_present) {
    j.start_array("supportedBandCombination-v1530");
    for (const auto& e1 : supported_band_combination_v1530) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_add_v1530_present) {
    j.start_array("supportedBandCombinationAdd-v1530");
    for (const auto& e1 : supported_band_combination_add_v1530) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_reduced_v1530_present) {
    j.start_array("supportedBandCombinationReduced-v1530");
    for (const auto& e1 : supported_band_combination_reduced_v1530) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pwr_class_minus14dbm_r15_present) {
    j.write_str("powerClass-14dBm-r15", "supported");
  }
  j.end_obj();
}

// SupportedBandEUTRA-v9e0 ::= SEQUENCE
SRSASN_CODE supported_band_eutra_v9e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_eutra_v9e0_present, 1));

  if (band_eutra_v9e0_present) {
    HANDLE_CODE(pack_integer(bref, band_eutra_v9e0, (uint16_t)65u, (uint16_t)256u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_band_eutra_v9e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_eutra_v9e0_present, 1));

  if (band_eutra_v9e0_present) {
    HANDLE_CODE(unpack_integer(band_eutra_v9e0, bref, (uint16_t)65u, (uint16_t)256u));
  }

  return SRSASN_SUCCESS;
}
void supported_band_eutra_v9e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_eutra_v9e0_present) {
    j.write_int("bandEUTRA-v9e0", band_eutra_v9e0);
  }
  j.end_obj();
}

// RF-Parameters-v9e0 ::= SEQUENCE
SRSASN_CODE rf_params_v9e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_list_eutra_v9e0_present, 1));

  if (supported_band_list_eutra_v9e0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_eutra_v9e0, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v9e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_band_list_eutra_v9e0_present, 1));

  if (supported_band_list_eutra_v9e0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_eutra_v9e0, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v9e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_list_eutra_v9e0_present) {
    j.start_array("supportedBandListEUTRA-v9e0");
    for (const auto& e1 : supported_band_list_eutra_v9e0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SupportedBandInfo-r12 ::= SEQUENCE
SRSASN_CODE supported_band_info_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(support_r12_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_band_info_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(support_r12_present, 1));

  return SRSASN_SUCCESS;
}
void supported_band_info_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (support_r12_present) {
    j.write_str("support-r12", "supported");
  }
  j.end_obj();
}

// SL-Parameters-r12 ::= SEQUENCE
SRSASN_CODE sl_params_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(comm_simul_tx_r12_present, 1));
  HANDLE_CODE(bref.pack(comm_supported_bands_r12_present, 1));
  HANDLE_CODE(bref.pack(disc_supported_bands_r12_present, 1));
  HANDLE_CODE(bref.pack(disc_sched_res_alloc_r12_present, 1));
  HANDLE_CODE(bref.pack(disc_ue_sel_res_alloc_r12_present, 1));
  HANDLE_CODE(bref.pack(disc_slss_r12_present, 1));
  HANDLE_CODE(bref.pack(disc_supported_proc_r12_present, 1));

  if (comm_supported_bands_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, comm_supported_bands_r12, 1, 64, integer_packer<uint16_t>(1, 256)));
  }
  if (disc_supported_bands_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, disc_supported_bands_r12, 1, 64));
  }
  if (disc_supported_proc_r12_present) {
    HANDLE_CODE(disc_supported_proc_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_params_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(comm_simul_tx_r12_present, 1));
  HANDLE_CODE(bref.unpack(comm_supported_bands_r12_present, 1));
  HANDLE_CODE(bref.unpack(disc_supported_bands_r12_present, 1));
  HANDLE_CODE(bref.unpack(disc_sched_res_alloc_r12_present, 1));
  HANDLE_CODE(bref.unpack(disc_ue_sel_res_alloc_r12_present, 1));
  HANDLE_CODE(bref.unpack(disc_slss_r12_present, 1));
  HANDLE_CODE(bref.unpack(disc_supported_proc_r12_present, 1));

  if (comm_supported_bands_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(comm_supported_bands_r12, bref, 1, 64, integer_packer<uint16_t>(1, 256)));
  }
  if (disc_supported_bands_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(disc_supported_bands_r12, bref, 1, 64));
  }
  if (disc_supported_proc_r12_present) {
    HANDLE_CODE(disc_supported_proc_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_params_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (comm_simul_tx_r12_present) {
    j.write_str("commSimultaneousTx-r12", "supported");
  }
  if (comm_supported_bands_r12_present) {
    j.start_array("commSupportedBands-r12");
    for (const auto& e1 : comm_supported_bands_r12) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (disc_supported_bands_r12_present) {
    j.start_array("discSupportedBands-r12");
    for (const auto& e1 : disc_supported_bands_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (disc_sched_res_alloc_r12_present) {
    j.write_str("discScheduledResourceAlloc-r12", "supported");
  }
  if (disc_ue_sel_res_alloc_r12_present) {
    j.write_str("disc-UE-SelectedResourceAlloc-r12", "supported");
  }
  if (disc_slss_r12_present) {
    j.write_str("disc-SLSS-r12", "supported");
  }
  if (disc_supported_proc_r12_present) {
    j.write_str("discSupportedProc-r12", disc_supported_proc_r12.to_string());
  }
  j.end_obj();
}

const char* sl_params_r12_s::disc_supported_proc_r12_opts::to_string() const
{
  static const char* options[] = {"n50", "n400"};
  return convert_enum_idx(options, 2, value, "sl_params_r12_s::disc_supported_proc_r12_e_");
}
uint16_t sl_params_r12_s::disc_supported_proc_r12_opts::to_number() const
{
  static const uint16_t options[] = {50, 400};
  return map_enum_number(options, 2, value, "sl_params_r12_s::disc_supported_proc_r12_e_");
}

// V2X-BandParameters-r14 ::= SEQUENCE
SRSASN_CODE v2x_band_params_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_params_tx_sl_r14_present, 1));
  HANDLE_CODE(bref.pack(band_params_rx_sl_r14_present, 1));

  HANDLE_CODE(pack_integer(bref, v2x_freq_band_eutra_r14, (uint16_t)1u, (uint16_t)256u));
  if (band_params_tx_sl_r14_present) {
    HANDLE_CODE(band_params_tx_sl_r14.pack(bref));
  }
  if (band_params_rx_sl_r14_present) {
    HANDLE_CODE(band_params_rx_sl_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE v2x_band_params_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_params_tx_sl_r14_present, 1));
  HANDLE_CODE(bref.unpack(band_params_rx_sl_r14_present, 1));

  HANDLE_CODE(unpack_integer(v2x_freq_band_eutra_r14, bref, (uint16_t)1u, (uint16_t)256u));
  if (band_params_tx_sl_r14_present) {
    HANDLE_CODE(band_params_tx_sl_r14.unpack(bref));
  }
  if (band_params_rx_sl_r14_present) {
    HANDLE_CODE(band_params_rx_sl_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void v2x_band_params_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("v2x-FreqBandEUTRA-r14", v2x_freq_band_eutra_r14);
  if (band_params_tx_sl_r14_present) {
    j.write_fieldname("bandParametersTxSL-r14");
    band_params_tx_sl_r14.to_json(j);
  }
  if (band_params_rx_sl_r14_present) {
    j.write_fieldname("bandParametersRxSL-r14");
    band_params_rx_sl_r14.to_json(j);
  }
  j.end_obj();
}

// SL-Parameters-v1430 ::= SEQUENCE
SRSASN_CODE sl_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(zone_based_pool_sel_r14_present, 1));
  HANDLE_CODE(bref.pack(ue_autonomous_with_full_sensing_r14_present, 1));
  HANDLE_CODE(bref.pack(ue_autonomous_with_partial_sensing_r14_present, 1));
  HANDLE_CODE(bref.pack(sl_congestion_ctrl_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_tx_with_short_resv_interv_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_num_tx_rx_timing_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_non_adjacent_pscch_pssch_r14_present, 1));
  HANDLE_CODE(bref.pack(slss_tx_rx_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_supported_band_combination_list_r14_present, 1));

  if (v2x_num_tx_rx_timing_r14_present) {
    HANDLE_CODE(pack_integer(bref, v2x_num_tx_rx_timing_r14, (uint8_t)1u, (uint8_t)16u));
  }
  if (v2x_supported_band_combination_list_r14_present) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, v2x_supported_band_combination_list_r14, 1, 384, SeqOfPacker<Packer>(1, 64, Packer())));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(zone_based_pool_sel_r14_present, 1));
  HANDLE_CODE(bref.unpack(ue_autonomous_with_full_sensing_r14_present, 1));
  HANDLE_CODE(bref.unpack(ue_autonomous_with_partial_sensing_r14_present, 1));
  HANDLE_CODE(bref.unpack(sl_congestion_ctrl_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_tx_with_short_resv_interv_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_num_tx_rx_timing_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_non_adjacent_pscch_pssch_r14_present, 1));
  HANDLE_CODE(bref.unpack(slss_tx_rx_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_supported_band_combination_list_r14_present, 1));

  if (v2x_num_tx_rx_timing_r14_present) {
    HANDLE_CODE(unpack_integer(v2x_num_tx_rx_timing_r14, bref, (uint8_t)1u, (uint8_t)16u));
  }
  if (v2x_supported_band_combination_list_r14_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(v2x_supported_band_combination_list_r14, bref, 1, 384, SeqOfPacker<Packer>(1, 64, Packer())));
  }

  return SRSASN_SUCCESS;
}
void sl_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (zone_based_pool_sel_r14_present) {
    j.write_str("zoneBasedPoolSelection-r14", "supported");
  }
  if (ue_autonomous_with_full_sensing_r14_present) {
    j.write_str("ue-AutonomousWithFullSensing-r14", "supported");
  }
  if (ue_autonomous_with_partial_sensing_r14_present) {
    j.write_str("ue-AutonomousWithPartialSensing-r14", "supported");
  }
  if (sl_congestion_ctrl_r14_present) {
    j.write_str("sl-CongestionControl-r14", "supported");
  }
  if (v2x_tx_with_short_resv_interv_r14_present) {
    j.write_str("v2x-TxWithShortResvInterval-r14", "supported");
  }
  if (v2x_num_tx_rx_timing_r14_present) {
    j.write_int("v2x-numberTxRxTiming-r14", v2x_num_tx_rx_timing_r14);
  }
  if (v2x_non_adjacent_pscch_pssch_r14_present) {
    j.write_str("v2x-nonAdjacentPSCCH-PSSCH-r14", "supported");
  }
  if (slss_tx_rx_r14_present) {
    j.write_str("slss-TxRx-r14", "supported");
  }
  if (v2x_supported_band_combination_list_r14_present) {
    j.start_array("v2x-SupportedBandCombinationList-r14");
    for (const auto& e1 : v2x_supported_band_combination_list_r14) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
      }
      j.end_array();
    }
    j.end_array();
  }
  j.end_obj();
}

// V2X-BandParameters-v1530 ::= SEQUENCE
SRSASN_CODE v2x_band_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(v2x_enhanced_high_reception_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE v2x_band_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(v2x_enhanced_high_reception_r15_present, 1));

  return SRSASN_SUCCESS;
}
void v2x_band_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (v2x_enhanced_high_reception_r15_present) {
    j.write_str("v2x-EnhancedHighReception-r15", "supported");
  }
  j.end_obj();
}

// UE-CategorySL-r15 ::= SEQUENCE
SRSASN_CODE ue_category_sl_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ue_category_sl_c_tx_r15, (uint8_t)1u, (uint8_t)5u));
  HANDLE_CODE(pack_integer(bref, ue_category_sl_c_rx_r15, (uint8_t)1u, (uint8_t)4u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_category_sl_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ue_category_sl_c_tx_r15, bref, (uint8_t)1u, (uint8_t)5u));
  HANDLE_CODE(unpack_integer(ue_category_sl_c_rx_r15, bref, (uint8_t)1u, (uint8_t)4u));

  return SRSASN_SUCCESS;
}
void ue_category_sl_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ue-CategorySL-C-TX-r15", ue_category_sl_c_tx_r15);
  j.write_int("ue-CategorySL-C-RX-r15", ue_category_sl_c_rx_r15);
  j.end_obj();
}

// SL-Parameters-v1530 ::= SEQUENCE
SRSASN_CODE sl_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(slss_supported_tx_freq_r15_present, 1));
  HANDLE_CODE(bref.pack(sl_minus64_qam_tx_r15_present, 1));
  HANDLE_CODE(bref.pack(sl_tx_diversity_r15_present, 1));
  HANDLE_CODE(bref.pack(ue_category_sl_r15_present, 1));
  HANDLE_CODE(bref.pack(v2x_supported_band_combination_list_v1530_present, 1));

  if (slss_supported_tx_freq_r15_present) {
    HANDLE_CODE(slss_supported_tx_freq_r15.pack(bref));
  }
  if (ue_category_sl_r15_present) {
    HANDLE_CODE(ue_category_sl_r15.pack(bref));
  }
  if (v2x_supported_band_combination_list_v1530_present) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, v2x_supported_band_combination_list_v1530, 1, 384, SeqOfPacker<Packer>(1, 64, Packer())));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(slss_supported_tx_freq_r15_present, 1));
  HANDLE_CODE(bref.unpack(sl_minus64_qam_tx_r15_present, 1));
  HANDLE_CODE(bref.unpack(sl_tx_diversity_r15_present, 1));
  HANDLE_CODE(bref.unpack(ue_category_sl_r15_present, 1));
  HANDLE_CODE(bref.unpack(v2x_supported_band_combination_list_v1530_present, 1));

  if (slss_supported_tx_freq_r15_present) {
    HANDLE_CODE(slss_supported_tx_freq_r15.unpack(bref));
  }
  if (ue_category_sl_r15_present) {
    HANDLE_CODE(ue_category_sl_r15.unpack(bref));
  }
  if (v2x_supported_band_combination_list_v1530_present) {
    HANDLE_CODE(unpack_dyn_seq_of(
        v2x_supported_band_combination_list_v1530, bref, 1, 384, SeqOfPacker<Packer>(1, 64, Packer())));
  }

  return SRSASN_SUCCESS;
}
void sl_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (slss_supported_tx_freq_r15_present) {
    j.write_str("slss-SupportedTxFreq-r15", slss_supported_tx_freq_r15.to_string());
  }
  if (sl_minus64_qam_tx_r15_present) {
    j.write_str("sl-64QAM-Tx-r15", "supported");
  }
  if (sl_tx_diversity_r15_present) {
    j.write_str("sl-TxDiversity-r15", "supported");
  }
  if (ue_category_sl_r15_present) {
    j.write_fieldname("ue-CategorySL-r15");
    ue_category_sl_r15.to_json(j);
  }
  if (v2x_supported_band_combination_list_v1530_present) {
    j.start_array("v2x-SupportedBandCombinationList-v1530");
    for (const auto& e1 : v2x_supported_band_combination_list_v1530) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
      }
      j.end_array();
    }
    j.end_array();
  }
  j.end_obj();
}

const char* sl_params_v1530_s::slss_supported_tx_freq_r15_opts::to_string() const
{
  static const char* options[] = {"single", "multiple"};
  return convert_enum_idx(options, 2, value, "sl_params_v1530_s::slss_supported_tx_freq_r15_e_");
}

// NeighCellSI-AcquisitionParameters-v15a0 ::= SEQUENCE
SRSASN_CODE neigh_cell_si_acquisition_params_v15a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eutra_cgi_report_nedc_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE neigh_cell_si_acquisition_params_v15a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(eutra_cgi_report_nedc_r15_present, 1));

  return SRSASN_SUCCESS;
}
void neigh_cell_si_acquisition_params_v15a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eutra_cgi_report_nedc_r15_present) {
    j.write_str("eutra-CGI-Reporting-NEDC-r15", "supported");
  }
  j.end_obj();
}

// PhyLayerParameters-v1540 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(stti_spt_cap_v1540_present, 1));
  HANDLE_CODE(bref.pack(crs_im_tm1_to_tm9_one_rx_port_v1540_present, 1));
  HANDLE_CODE(bref.pack(cch_im_ref_rec_type_a_one_rx_port_v1540_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(stti_spt_cap_v1540_present, 1));
  HANDLE_CODE(bref.unpack(crs_im_tm1_to_tm9_one_rx_port_v1540_present, 1));
  HANDLE_CODE(bref.unpack(cch_im_ref_rec_type_a_one_rx_port_v1540_present, 1));

  return SRSASN_SUCCESS;
}
void phy_layer_params_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (stti_spt_cap_v1540_present) {
    j.write_fieldname("stti-SPT-Capabilities-v1540");
    j.start_obj();
    j.write_str("slotPDSCH-TxDiv-TM8-r15", "supported");
    j.end_obj();
  }
  if (crs_im_tm1_to_tm9_one_rx_port_v1540_present) {
    j.write_str("crs-IM-TM1-toTM9-OneRX-Port-v1540", "supported");
  }
  if (cch_im_ref_rec_type_a_one_rx_port_v1540_present) {
    j.write_str("cch-IM-RefRecTypeA-OneRX-Port-v1540", "supported");
  }
  j.end_obj();
}

// PhyLayerParameters-v1550 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v1550_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dmrs_overhead_reduction_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v1550_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dmrs_overhead_reduction_r15_present, 1));

  return SRSASN_SUCCESS;
}
void phy_layer_params_v1550_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dmrs_overhead_reduction_r15_present) {
    j.write_str("dmrs-OverheadReduction-r15", "supported");
  }
  j.end_obj();
}

// EUTRA-5GC-Parameters-r15 ::= SEQUENCE
SRSASN_CODE eutra_minus5_gc_params_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eutra_minus5_gc_r15_present, 1));
  HANDLE_CODE(bref.pack(eutra_epc_ho_eutra_minus5_gc_r15_present, 1));
  HANDLE_CODE(bref.pack(ho_eutra_minus5_gc_fdd_tdd_r15_present, 1));
  HANDLE_CODE(bref.pack(ho_interfreq_eutra_minus5_gc_r15_present, 1));
  HANDLE_CODE(bref.pack(ims_voice_over_mcg_bearer_eutra_minus5_gc_r15_present, 1));
  HANDLE_CODE(bref.pack(inactive_state_r15_present, 1));
  HANDLE_CODE(bref.pack(reflective_qos_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_minus5_gc_params_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(eutra_minus5_gc_r15_present, 1));
  HANDLE_CODE(bref.unpack(eutra_epc_ho_eutra_minus5_gc_r15_present, 1));
  HANDLE_CODE(bref.unpack(ho_eutra_minus5_gc_fdd_tdd_r15_present, 1));
  HANDLE_CODE(bref.unpack(ho_interfreq_eutra_minus5_gc_r15_present, 1));
  HANDLE_CODE(bref.unpack(ims_voice_over_mcg_bearer_eutra_minus5_gc_r15_present, 1));
  HANDLE_CODE(bref.unpack(inactive_state_r15_present, 1));
  HANDLE_CODE(bref.unpack(reflective_qos_r15_present, 1));

  return SRSASN_SUCCESS;
}
void eutra_minus5_gc_params_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eutra_minus5_gc_r15_present) {
    j.write_str("eutra-5GC-r15", "supported");
  }
  if (eutra_epc_ho_eutra_minus5_gc_r15_present) {
    j.write_str("eutra-EPC-HO-EUTRA-5GC-r15", "supported");
  }
  if (ho_eutra_minus5_gc_fdd_tdd_r15_present) {
    j.write_str("ho-EUTRA-5GC-FDD-TDD-r15", "supported");
  }
  if (ho_interfreq_eutra_minus5_gc_r15_present) {
    j.write_str("ho-InterfreqEUTRA-5GC-r15", "supported");
  }
  if (ims_voice_over_mcg_bearer_eutra_minus5_gc_r15_present) {
    j.write_str("ims-VoiceOverMCG-BearerEUTRA-5GC-r15", "supported");
  }
  if (inactive_state_r15_present) {
    j.write_str("inactiveState-r15", "supported");
  }
  if (reflective_qos_r15_present) {
    j.write_str("reflectiveQoS-r15", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v15a0 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v15a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_layer_params_v1530_present, 1));
  HANDLE_CODE(bref.pack(phy_layer_params_v1540_present, 1));
  HANDLE_CODE(bref.pack(phy_layer_params_v1550_present, 1));

  if (phy_layer_params_v1530_present) {
    HANDLE_CODE(phy_layer_params_v1530.pack(bref));
  }
  if (phy_layer_params_v1540_present) {
    HANDLE_CODE(phy_layer_params_v1540.pack(bref));
  }
  if (phy_layer_params_v1550_present) {
    HANDLE_CODE(phy_layer_params_v1550.pack(bref));
  }
  HANDLE_CODE(neigh_cell_si_acquisition_params_v15a0.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v15a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_layer_params_v1530_present, 1));
  HANDLE_CODE(bref.unpack(phy_layer_params_v1540_present, 1));
  HANDLE_CODE(bref.unpack(phy_layer_params_v1550_present, 1));

  if (phy_layer_params_v1530_present) {
    HANDLE_CODE(phy_layer_params_v1530.unpack(bref));
  }
  if (phy_layer_params_v1540_present) {
    HANDLE_CODE(phy_layer_params_v1540.unpack(bref));
  }
  if (phy_layer_params_v1550_present) {
    HANDLE_CODE(phy_layer_params_v1550.unpack(bref));
  }
  HANDLE_CODE(neigh_cell_si_acquisition_params_v15a0.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v15a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_v1530_present) {
    j.write_fieldname("phyLayerParameters-v1530");
    phy_layer_params_v1530.to_json(j);
  }
  if (phy_layer_params_v1540_present) {
    j.write_fieldname("phyLayerParameters-v1540");
    phy_layer_params_v1540.to_json(j);
  }
  if (phy_layer_params_v1550_present) {
    j.write_fieldname("phyLayerParameters-v1550");
    phy_layer_params_v1550.to_json(j);
  }
  j.write_fieldname("neighCellSI-AcquisitionParameters-v15a0");
  neigh_cell_si_acquisition_params_v15a0.to_json(j);
  j.end_obj();
}

// IRAT-ParametersNR-v1570 ::= SEQUENCE
SRSASN_CODE irat_params_nr_v1570_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ss_sinr_meas_nr_fr1_r15_present, 1));
  HANDLE_CODE(bref.pack(ss_sinr_meas_nr_fr2_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_nr_v1570_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ss_sinr_meas_nr_fr1_r15_present, 1));
  HANDLE_CODE(bref.unpack(ss_sinr_meas_nr_fr2_r15_present, 1));

  return SRSASN_SUCCESS;
}
void irat_params_nr_v1570_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ss_sinr_meas_nr_fr1_r15_present) {
    j.write_str("ss-SINR-Meas-NR-FR1-r15", "supported");
  }
  if (ss_sinr_meas_nr_fr2_r15_present) {
    j.write_str("ss-SINR-Meas-NR-FR2-r15", "supported");
  }
  j.end_obj();
}

// PDCP-ParametersNR-v1560 ::= SEQUENCE
SRSASN_CODE pdcp_params_nr_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ims_vo_nr_pdcp_scg_ngendc_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_params_nr_v1560_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ims_vo_nr_pdcp_scg_ngendc_r15_present, 1));

  return SRSASN_SUCCESS;
}
void pdcp_params_nr_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ims_vo_nr_pdcp_scg_ngendc_r15_present) {
    j.write_str("ims-VoNR-PDCP-SCG-NGENDC-r15", "supported");
  }
  j.end_obj();
}

// RF-Parameters-v1570 ::= SEQUENCE
SRSASN_CODE rf_params_v1570_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(dl_minus1024_qam_scaling_factor_r15.pack(bref));
  HANDLE_CODE(pack_integer(bref, dl_minus1024_qam_total_weighted_layers_r15, (uint8_t)0u, (uint8_t)10u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v1570_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(dl_minus1024_qam_scaling_factor_r15.unpack(bref));
  HANDLE_CODE(unpack_integer(dl_minus1024_qam_total_weighted_layers_r15, bref, (uint8_t)0u, (uint8_t)10u));

  return SRSASN_SUCCESS;
}
void rf_params_v1570_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("dl-1024QAM-ScalingFactor-r15", dl_minus1024_qam_scaling_factor_r15.to_string());
  j.write_int("dl-1024QAM-TotalWeightedLayers-r15", dl_minus1024_qam_total_weighted_layers_r15);
  j.end_obj();
}

const char* rf_params_v1570_s::dl_minus1024_qam_scaling_factor_r15_opts::to_string() const
{
  static const char* options[] = {"v1", "v1dot2", "v1dot25"};
  return convert_enum_idx(options, 3, value, "rf_params_v1570_s::dl_minus1024_qam_scaling_factor_r15_e_");
}
float rf_params_v1570_s::dl_minus1024_qam_scaling_factor_r15_opts::to_number() const
{
  static const float options[] = {1.0, 1.2, 1.25};
  return map_enum_number(options, 3, value, "rf_params_v1570_s::dl_minus1024_qam_scaling_factor_r15_e_");
}
const char* rf_params_v1570_s::dl_minus1024_qam_scaling_factor_r15_opts::to_number_string() const
{
  static const char* options[] = {"1", "1.2", "1.25"};
  return convert_enum_idx(options, 3, value, "rf_params_v1570_s::dl_minus1024_qam_scaling_factor_r15_e_");
}

// UE-EUTRA-Capability-v15a0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v15a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eutra_minus5_gc_params_r15_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_eutra_cap_v15a0_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_eutra_cap_v15a0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(neigh_cell_si_acquisition_params_v15a0.pack(bref));
  if (eutra_minus5_gc_params_r15_present) {
    HANDLE_CODE(eutra_minus5_gc_params_r15.pack(bref));
  }
  if (fdd_add_ue_eutra_cap_v15a0_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v15a0.pack(bref));
  }
  if (tdd_add_ue_eutra_cap_v15a0_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v15a0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v15a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(eutra_minus5_gc_params_r15_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_eutra_cap_v15a0_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_eutra_cap_v15a0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(neigh_cell_si_acquisition_params_v15a0.unpack(bref));
  if (eutra_minus5_gc_params_r15_present) {
    HANDLE_CODE(eutra_minus5_gc_params_r15.unpack(bref));
  }
  if (fdd_add_ue_eutra_cap_v15a0_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v15a0.unpack(bref));
  }
  if (tdd_add_ue_eutra_cap_v15a0_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v15a0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v15a0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("neighCellSI-AcquisitionParameters-v15a0");
  neigh_cell_si_acquisition_params_v15a0.to_json(j);
  if (eutra_minus5_gc_params_r15_present) {
    j.write_fieldname("eutra-5GC-Parameters-r15");
    eutra_minus5_gc_params_r15.to_json(j);
  }
  if (fdd_add_ue_eutra_cap_v15a0_present) {
    j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v15a0");
    fdd_add_ue_eutra_cap_v15a0.to_json(j);
  }
  if (tdd_add_ue_eutra_cap_v15a0_present) {
    j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v15a0");
    tdd_add_ue_eutra_cap_v15a0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// IRAT-ParametersNR-v1560 ::= SEQUENCE
SRSASN_CODE irat_params_nr_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ng_en_dc_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_nr_v1560_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ng_en_dc_r15_present, 1));

  return SRSASN_SUCCESS;
}
void irat_params_nr_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ng_en_dc_r15_present) {
    j.write_str("ng-EN-DC-r15", "supported");
  }
  j.end_obj();
}

// NeighCellSI-AcquisitionParameters-v1550 ::= SEQUENCE
SRSASN_CODE neigh_cell_si_acquisition_params_v1550_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eutra_cgi_report_endc_r15_present, 1));
  HANDLE_CODE(bref.pack(utra_geran_cgi_report_endc_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE neigh_cell_si_acquisition_params_v1550_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(eutra_cgi_report_endc_r15_present, 1));
  HANDLE_CODE(bref.unpack(utra_geran_cgi_report_endc_r15_present, 1));

  return SRSASN_SUCCESS;
}
void neigh_cell_si_acquisition_params_v1550_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eutra_cgi_report_endc_r15_present) {
    j.write_str("eutra-CGI-Reporting-ENDC-r15", "supported");
  }
  if (utra_geran_cgi_report_endc_r15_present) {
    j.write_str("utra-GERAN-CGI-Reporting-ENDC-r15", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1570-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1570_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v1570_present, 1));
  HANDLE_CODE(bref.pack(irat_params_nr_v1570_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v1570_present) {
    HANDLE_CODE(rf_params_v1570.pack(bref));
  }
  if (irat_params_nr_v1570_present) {
    HANDLE_CODE(irat_params_nr_v1570.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1570_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v1570_present, 1));
  HANDLE_CODE(bref.unpack(irat_params_nr_v1570_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v1570_present) {
    HANDLE_CODE(rf_params_v1570.unpack(bref));
  }
  if (irat_params_nr_v1570_present) {
    HANDLE_CODE(irat_params_nr_v1570.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1570_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v1570_present) {
    j.write_fieldname("rf-Parameters-v1570");
    rf_params_v1570.to_json(j);
  }
  if (irat_params_nr_v1570_present) {
    j.write_fieldname("irat-ParametersNR-v1570");
    irat_params_nr_v1570.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v1560 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pdcp_params_nr_v1560.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1560_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(pdcp_params_nr_v1560.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("pdcp-ParametersNR-v1560");
  pdcp_params_nr_v1560.to_json(j);
  j.end_obj();
}

// MAC-Parameters-v1550 ::= SEQUENCE
SRSASN_CODE mac_params_v1550_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(elcid_support_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_params_v1550_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(elcid_support_r15_present, 1));

  return SRSASN_SUCCESS;
}
void mac_params_v1550_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (elcid_support_r15_present) {
    j.write_str("eLCID-Support-r15", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1560-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1560_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(applied_cap_filt_common_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pdcp_params_nr_v1560.pack(bref));
  HANDLE_CODE(irat_params_nr_v1560.pack(bref));
  if (applied_cap_filt_common_r15_present) {
    HANDLE_CODE(applied_cap_filt_common_r15.pack(bref));
  }
  HANDLE_CODE(fdd_add_ue_eutra_cap_v1560.pack(bref));
  HANDLE_CODE(tdd_add_ue_eutra_cap_v1560.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1560_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(applied_cap_filt_common_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(pdcp_params_nr_v1560.unpack(bref));
  HANDLE_CODE(irat_params_nr_v1560.unpack(bref));
  if (applied_cap_filt_common_r15_present) {
    HANDLE_CODE(applied_cap_filt_common_r15.unpack(bref));
  }
  HANDLE_CODE(fdd_add_ue_eutra_cap_v1560.unpack(bref));
  HANDLE_CODE(tdd_add_ue_eutra_cap_v1560.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1560_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("pdcp-ParametersNR-v1560");
  pdcp_params_nr_v1560.to_json(j);
  j.write_fieldname("irat-ParametersNR-v1560");
  irat_params_nr_v1560.to_json(j);
  if (applied_cap_filt_common_r15_present) {
    j.write_str("appliedCapabilityFilterCommon-r15", applied_cap_filt_common_r15.to_string());
  }
  j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v1560");
  fdd_add_ue_eutra_cap_v1560.to_json(j);
  j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v1560");
  tdd_add_ue_eutra_cap_v1560.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v1550 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1550_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(neigh_cell_si_acquisition_params_v1550_present, 1));

  if (neigh_cell_si_acquisition_params_v1550_present) {
    HANDLE_CODE(neigh_cell_si_acquisition_params_v1550.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1550_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(neigh_cell_si_acquisition_params_v1550_present, 1));

  if (neigh_cell_si_acquisition_params_v1550_present) {
    HANDLE_CODE(neigh_cell_si_acquisition_params_v1550.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v1550_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (neigh_cell_si_acquisition_params_v1550_present) {
    j.write_fieldname("neighCellSI-AcquisitionParameters-v1550");
    neigh_cell_si_acquisition_params_v1550.to_json(j);
  }
  j.end_obj();
}

// NeighCellSI-AcquisitionParameters-v1530 ::= SEQUENCE
SRSASN_CODE neigh_cell_si_acquisition_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(report_cgi_nr_en_dc_r15_present, 1));
  HANDLE_CODE(bref.pack(report_cgi_nr_no_en_dc_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE neigh_cell_si_acquisition_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(report_cgi_nr_en_dc_r15_present, 1));
  HANDLE_CODE(bref.unpack(report_cgi_nr_no_en_dc_r15_present, 1));

  return SRSASN_SUCCESS;
}
void neigh_cell_si_acquisition_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (report_cgi_nr_en_dc_r15_present) {
    j.write_str("reportCGI-NR-EN-DC-r15", "supported");
  }
  if (report_cgi_nr_no_en_dc_r15_present) {
    j.write_str("reportCGI-NR-NoEN-DC-r15", "supported");
  }
  j.end_obj();
}

// Other-Parameters-v1540 ::= SEQUENCE
SRSASN_CODE other_params_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(in_dev_coex_ind_endc_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE other_params_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(in_dev_coex_ind_endc_r15_present, 1));

  return SRSASN_SUCCESS;
}
void other_params_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (in_dev_coex_ind_endc_r15_present) {
    j.write_str("inDeviceCoexInd-ENDC-r15", "supported");
  }
  j.end_obj();
}

// SL-Parameters-v1540 ::= SEQUENCE
SRSASN_CODE sl_params_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_minus64_qam_rx_r15_present, 1));
  HANDLE_CODE(bref.pack(sl_rate_matching_tbs_scaling_r15_present, 1));
  HANDLE_CODE(bref.pack(sl_low_t2min_r15_present, 1));
  HANDLE_CODE(bref.pack(v2x_sensing_report_mode3_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_params_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_minus64_qam_rx_r15_present, 1));
  HANDLE_CODE(bref.unpack(sl_rate_matching_tbs_scaling_r15_present, 1));
  HANDLE_CODE(bref.unpack(sl_low_t2min_r15_present, 1));
  HANDLE_CODE(bref.unpack(v2x_sensing_report_mode3_r15_present, 1));

  return SRSASN_SUCCESS;
}
void sl_params_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_minus64_qam_rx_r15_present) {
    j.write_str("sl-64QAM-Rx-r15", "supported");
  }
  if (sl_rate_matching_tbs_scaling_r15_present) {
    j.write_str("sl-RateMatchingTBSScaling-r15", "supported");
  }
  if (sl_low_t2min_r15_present) {
    j.write_str("sl-LowT2min-r15", "supported");
  }
  if (v2x_sensing_report_mode3_r15_present) {
    j.write_str("v2x-SensingReportingMode3-r15", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1550-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1550_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(neigh_cell_si_acquisition_params_v1550_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (neigh_cell_si_acquisition_params_v1550_present) {
    HANDLE_CODE(neigh_cell_si_acquisition_params_v1550.pack(bref));
  }
  HANDLE_CODE(phy_layer_params_v1550.pack(bref));
  HANDLE_CODE(mac_params_v1550.pack(bref));
  HANDLE_CODE(fdd_add_ue_eutra_cap_v1550.pack(bref));
  HANDLE_CODE(tdd_add_ue_eutra_cap_v1550.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1550_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(neigh_cell_si_acquisition_params_v1550_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (neigh_cell_si_acquisition_params_v1550_present) {
    HANDLE_CODE(neigh_cell_si_acquisition_params_v1550.unpack(bref));
  }
  HANDLE_CODE(phy_layer_params_v1550.unpack(bref));
  HANDLE_CODE(mac_params_v1550.unpack(bref));
  HANDLE_CODE(fdd_add_ue_eutra_cap_v1550.unpack(bref));
  HANDLE_CODE(tdd_add_ue_eutra_cap_v1550.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1550_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (neigh_cell_si_acquisition_params_v1550_present) {
    j.write_fieldname("neighCellSI-AcquisitionParameters-v1550");
    neigh_cell_si_acquisition_params_v1550.to_json(j);
  }
  j.write_fieldname("phyLayerParameters-v1550");
  phy_layer_params_v1550.to_json(j);
  j.write_fieldname("mac-Parameters-v1550");
  mac_params_v1550.to_json(j);
  j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v1550");
  fdd_add_ue_eutra_cap_v1550.to_json(j);
  j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v1550");
  tdd_add_ue_eutra_cap_v1550.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v1540 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eutra_minus5_gc_params_r15_present, 1));
  HANDLE_CODE(bref.pack(irat_params_nr_v1540_present, 1));

  if (eutra_minus5_gc_params_r15_present) {
    HANDLE_CODE(eutra_minus5_gc_params_r15.pack(bref));
  }
  if (irat_params_nr_v1540_present) {
    HANDLE_CODE(irat_params_nr_v1540.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(eutra_minus5_gc_params_r15_present, 1));
  HANDLE_CODE(bref.unpack(irat_params_nr_v1540_present, 1));

  if (eutra_minus5_gc_params_r15_present) {
    HANDLE_CODE(eutra_minus5_gc_params_r15.unpack(bref));
  }
  if (irat_params_nr_v1540_present) {
    HANDLE_CODE(irat_params_nr_v1540.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eutra_minus5_gc_params_r15_present) {
    j.write_fieldname("eutra-5GC-Parameters-r15");
    eutra_minus5_gc_params_r15.to_json(j);
  }
  if (irat_params_nr_v1540_present) {
    j.write_fieldname("irat-ParametersNR-v1540");
    irat_params_nr_v1540.to_json(j);
  }
  j.end_obj();
}

// LAA-Parameters-v1530 ::= SEQUENCE
SRSASN_CODE laa_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(aul_r15_present, 1));
  HANDLE_CODE(bref.pack(laa_pusch_mode1_r15_present, 1));
  HANDLE_CODE(bref.pack(laa_pusch_mode2_r15_present, 1));
  HANDLE_CODE(bref.pack(laa_pusch_mode3_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE laa_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(aul_r15_present, 1));
  HANDLE_CODE(bref.unpack(laa_pusch_mode1_r15_present, 1));
  HANDLE_CODE(bref.unpack(laa_pusch_mode2_r15_present, 1));
  HANDLE_CODE(bref.unpack(laa_pusch_mode3_r15_present, 1));

  return SRSASN_SUCCESS;
}
void laa_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (aul_r15_present) {
    j.write_str("aul-r15", "supported");
  }
  if (laa_pusch_mode1_r15_present) {
    j.write_str("laa-PUSCH-Mode1-r15", "supported");
  }
  if (laa_pusch_mode2_r15_present) {
    j.write_str("laa-PUSCH-Mode2-r15", "supported");
  }
  if (laa_pusch_mode3_r15_present) {
    j.write_str("laa-PUSCH-Mode3-r15", "supported");
  }
  j.end_obj();
}

// MeasParameters-v1530 ::= SEQUENCE
SRSASN_CODE meas_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(qoe_meas_report_r15_present, 1));
  HANDLE_CODE(bref.pack(qoe_mtsi_meas_report_r15_present, 1));
  HANDLE_CODE(bref.pack(ca_idle_mode_meass_r15_present, 1));
  HANDLE_CODE(bref.pack(ca_idle_mode_validity_area_r15_present, 1));
  HANDLE_CODE(bref.pack(height_meas_r15_present, 1));
  HANDLE_CODE(bref.pack(multiple_cells_meas_ext_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(qoe_meas_report_r15_present, 1));
  HANDLE_CODE(bref.unpack(qoe_mtsi_meas_report_r15_present, 1));
  HANDLE_CODE(bref.unpack(ca_idle_mode_meass_r15_present, 1));
  HANDLE_CODE(bref.unpack(ca_idle_mode_validity_area_r15_present, 1));
  HANDLE_CODE(bref.unpack(height_meas_r15_present, 1));
  HANDLE_CODE(bref.unpack(multiple_cells_meas_ext_r15_present, 1));

  return SRSASN_SUCCESS;
}
void meas_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (qoe_meas_report_r15_present) {
    j.write_str("qoe-MeasReport-r15", "supported");
  }
  if (qoe_mtsi_meas_report_r15_present) {
    j.write_str("qoe-MTSI-MeasReport-r15", "supported");
  }
  if (ca_idle_mode_meass_r15_present) {
    j.write_str("ca-IdleModeMeasurements-r15", "supported");
  }
  if (ca_idle_mode_validity_area_r15_present) {
    j.write_str("ca-IdleModeValidityArea-r15", "supported");
  }
  if (height_meas_r15_present) {
    j.write_str("heightMeas-r15", "supported");
  }
  if (multiple_cells_meas_ext_r15_present) {
    j.write_str("multipleCellsMeasExtension-r15", "supported");
  }
  j.end_obj();
}

// Other-Parameters-v1530 ::= SEQUENCE
SRSASN_CODE other_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(assist_info_bit_for_lc_r15_present, 1));
  HANDLE_CODE(bref.pack(time_ref_provision_r15_present, 1));
  HANDLE_CODE(bref.pack(flight_path_plan_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE other_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(assist_info_bit_for_lc_r15_present, 1));
  HANDLE_CODE(bref.unpack(time_ref_provision_r15_present, 1));
  HANDLE_CODE(bref.unpack(flight_path_plan_r15_present, 1));

  return SRSASN_SUCCESS;
}
void other_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (assist_info_bit_for_lc_r15_present) {
    j.write_str("assistInfoBitForLC-r15", "supported");
  }
  if (time_ref_provision_r15_present) {
    j.write_str("timeReferenceProvision-r15", "supported");
  }
  if (flight_path_plan_r15_present) {
    j.write_str("flightPathPlan-r15", "supported");
  }
  j.end_obj();
}

// RLC-Parameters-v1530 ::= SEQUENCE
SRSASN_CODE rlc_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(flex_um_am_combinations_r15_present, 1));
  HANDLE_CODE(bref.pack(rlc_am_ooo_delivery_r15_present, 1));
  HANDLE_CODE(bref.pack(rlc_um_ooo_delivery_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(flex_um_am_combinations_r15_present, 1));
  HANDLE_CODE(bref.unpack(rlc_am_ooo_delivery_r15_present, 1));
  HANDLE_CODE(bref.unpack(rlc_um_ooo_delivery_r15_present, 1));

  return SRSASN_SUCCESS;
}
void rlc_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (flex_um_am_combinations_r15_present) {
    j.write_str("flexibleUM-AM-Combinations-r15", "supported");
  }
  if (rlc_am_ooo_delivery_r15_present) {
    j.write_str("rlc-AM-Ooo-Delivery-r15", "supported");
  }
  if (rlc_um_ooo_delivery_r15_present) {
    j.write_str("rlc-UM-Ooo-Delivery-r15", "supported");
  }
  j.end_obj();
}

// UE-BasedNetwPerfMeasParameters-v1530 ::= SEQUENCE
SRSASN_CODE ue_based_netw_perf_meas_params_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(logged_meas_bt_r15_present, 1));
  HANDLE_CODE(bref.pack(logged_meas_wlan_r15_present, 1));
  HANDLE_CODE(bref.pack(imm_meas_bt_r15_present, 1));
  HANDLE_CODE(bref.pack(imm_meas_wlan_r15_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_based_netw_perf_meas_params_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(logged_meas_bt_r15_present, 1));
  HANDLE_CODE(bref.unpack(logged_meas_wlan_r15_present, 1));
  HANDLE_CODE(bref.unpack(imm_meas_bt_r15_present, 1));
  HANDLE_CODE(bref.unpack(imm_meas_wlan_r15_present, 1));

  return SRSASN_SUCCESS;
}
void ue_based_netw_perf_meas_params_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (logged_meas_bt_r15_present) {
    j.write_str("loggedMeasBT-r15", "supported");
  }
  if (logged_meas_wlan_r15_present) {
    j.write_str("loggedMeasWLAN-r15", "supported");
  }
  if (imm_meas_bt_r15_present) {
    j.write_str("immMeasBT-r15", "supported");
  }
  if (imm_meas_wlan_r15_present) {
    j.write_str("immMeasWLAN-r15", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1540-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_layer_params_v1540_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_eutra_cap_v1540_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_eutra_cap_v1540_present, 1));
  HANDLE_CODE(bref.pack(sl_params_v1540_present, 1));
  HANDLE_CODE(bref.pack(irat_params_nr_v1540_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (phy_layer_params_v1540_present) {
    HANDLE_CODE(phy_layer_params_v1540.pack(bref));
  }
  HANDLE_CODE(other_params_v1540.pack(bref));
  if (fdd_add_ue_eutra_cap_v1540_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1540.pack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1540_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1540.pack(bref));
  }
  if (sl_params_v1540_present) {
    HANDLE_CODE(sl_params_v1540.pack(bref));
  }
  if (irat_params_nr_v1540_present) {
    HANDLE_CODE(irat_params_nr_v1540.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1540_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_layer_params_v1540_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_eutra_cap_v1540_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_eutra_cap_v1540_present, 1));
  HANDLE_CODE(bref.unpack(sl_params_v1540_present, 1));
  HANDLE_CODE(bref.unpack(irat_params_nr_v1540_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (phy_layer_params_v1540_present) {
    HANDLE_CODE(phy_layer_params_v1540.unpack(bref));
  }
  HANDLE_CODE(other_params_v1540.unpack(bref));
  if (fdd_add_ue_eutra_cap_v1540_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1540.unpack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1540_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1540.unpack(bref));
  }
  if (sl_params_v1540_present) {
    HANDLE_CODE(sl_params_v1540.unpack(bref));
  }
  if (irat_params_nr_v1540_present) {
    HANDLE_CODE(irat_params_nr_v1540.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_v1540_present) {
    j.write_fieldname("phyLayerParameters-v1540");
    phy_layer_params_v1540.to_json(j);
  }
  j.write_fieldname("otherParameters-v1540");
  other_params_v1540.to_json(j);
  if (fdd_add_ue_eutra_cap_v1540_present) {
    j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v1540");
    fdd_add_ue_eutra_cap_v1540.to_json(j);
  }
  if (tdd_add_ue_eutra_cap_v1540_present) {
    j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v1540");
    tdd_add_ue_eutra_cap_v1540.to_json(j);
  }
  if (sl_params_v1540_present) {
    j.write_fieldname("sl-Parameters-v1540");
    sl_params_v1540.to_json(j);
  }
  if (irat_params_nr_v1540_present) {
    j.write_fieldname("irat-ParametersNR-v1540");
    irat_params_nr_v1540.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v1530 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(neigh_cell_si_acquisition_params_v1530_present, 1));
  HANDLE_CODE(bref.pack(reduced_cp_latency_r15_present, 1));

  if (neigh_cell_si_acquisition_params_v1530_present) {
    HANDLE_CODE(neigh_cell_si_acquisition_params_v1530.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(neigh_cell_si_acquisition_params_v1530_present, 1));
  HANDLE_CODE(bref.unpack(reduced_cp_latency_r15_present, 1));

  if (neigh_cell_si_acquisition_params_v1530_present) {
    HANDLE_CODE(neigh_cell_si_acquisition_params_v1530.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (neigh_cell_si_acquisition_params_v1530_present) {
    j.write_fieldname("neighCellSI-AcquisitionParameters-v1530");
    neigh_cell_si_acquisition_params_v1530.to_json(j);
  }
  if (reduced_cp_latency_r15_present) {
    j.write_str("reducedCP-Latency-r15", "supported");
  }
  j.end_obj();
}

// MeasParameters-v1520 ::= SEQUENCE
SRSASN_CODE meas_params_v1520_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_gap_patterns_r15_present, 1));

  if (meas_gap_patterns_r15_present) {
    HANDLE_CODE(meas_gap_patterns_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_params_v1520_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_gap_patterns_r15_present, 1));

  if (meas_gap_patterns_r15_present) {
    HANDLE_CODE(meas_gap_patterns_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_params_v1520_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_gap_patterns_r15_present) {
    j.write_str("measGapPatterns-r15", meas_gap_patterns_r15.to_string());
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1530-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_params_v1530_present, 1));
  HANDLE_CODE(bref.pack(other_params_v1530_present, 1));
  HANDLE_CODE(bref.pack(neigh_cell_si_acquisition_params_v1530_present, 1));
  HANDLE_CODE(bref.pack(mac_params_v1530_present, 1));
  HANDLE_CODE(bref.pack(phy_layer_params_v1530_present, 1));
  HANDLE_CODE(bref.pack(rf_params_v1530_present, 1));
  HANDLE_CODE(bref.pack(pdcp_params_v1530_present, 1));
  HANDLE_CODE(bref.pack(ue_category_dl_v1530_present, 1));
  HANDLE_CODE(bref.pack(ue_based_netw_perf_meas_params_v1530_present, 1));
  HANDLE_CODE(bref.pack(rlc_params_v1530_present, 1));
  HANDLE_CODE(bref.pack(sl_params_v1530_present, 1));
  HANDLE_CODE(bref.pack(extended_nof_drbs_r15_present, 1));
  HANDLE_CODE(bref.pack(reduced_cp_latency_r15_present, 1));
  HANDLE_CODE(bref.pack(laa_params_v1530_present, 1));
  HANDLE_CODE(bref.pack(ue_category_ul_v1530_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_eutra_cap_v1530_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_eutra_cap_v1530_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_params_v1530_present) {
    HANDLE_CODE(meas_params_v1530.pack(bref));
  }
  if (other_params_v1530_present) {
    HANDLE_CODE(other_params_v1530.pack(bref));
  }
  if (neigh_cell_si_acquisition_params_v1530_present) {
    HANDLE_CODE(neigh_cell_si_acquisition_params_v1530.pack(bref));
  }
  if (mac_params_v1530_present) {
    HANDLE_CODE(mac_params_v1530.pack(bref));
  }
  if (phy_layer_params_v1530_present) {
    HANDLE_CODE(phy_layer_params_v1530.pack(bref));
  }
  if (rf_params_v1530_present) {
    HANDLE_CODE(rf_params_v1530.pack(bref));
  }
  if (pdcp_params_v1530_present) {
    HANDLE_CODE(pdcp_params_v1530.pack(bref));
  }
  if (ue_category_dl_v1530_present) {
    HANDLE_CODE(pack_integer(bref, ue_category_dl_v1530, (uint8_t)22u, (uint8_t)26u));
  }
  if (ue_based_netw_perf_meas_params_v1530_present) {
    HANDLE_CODE(ue_based_netw_perf_meas_params_v1530.pack(bref));
  }
  if (rlc_params_v1530_present) {
    HANDLE_CODE(rlc_params_v1530.pack(bref));
  }
  if (sl_params_v1530_present) {
    HANDLE_CODE(sl_params_v1530.pack(bref));
  }
  if (laa_params_v1530_present) {
    HANDLE_CODE(laa_params_v1530.pack(bref));
  }
  if (ue_category_ul_v1530_present) {
    HANDLE_CODE(pack_integer(bref, ue_category_ul_v1530, (uint8_t)22u, (uint8_t)26u));
  }
  if (fdd_add_ue_eutra_cap_v1530_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1530.pack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1530_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1530.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_params_v1530_present, 1));
  HANDLE_CODE(bref.unpack(other_params_v1530_present, 1));
  HANDLE_CODE(bref.unpack(neigh_cell_si_acquisition_params_v1530_present, 1));
  HANDLE_CODE(bref.unpack(mac_params_v1530_present, 1));
  HANDLE_CODE(bref.unpack(phy_layer_params_v1530_present, 1));
  HANDLE_CODE(bref.unpack(rf_params_v1530_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_params_v1530_present, 1));
  HANDLE_CODE(bref.unpack(ue_category_dl_v1530_present, 1));
  HANDLE_CODE(bref.unpack(ue_based_netw_perf_meas_params_v1530_present, 1));
  HANDLE_CODE(bref.unpack(rlc_params_v1530_present, 1));
  HANDLE_CODE(bref.unpack(sl_params_v1530_present, 1));
  HANDLE_CODE(bref.unpack(extended_nof_drbs_r15_present, 1));
  HANDLE_CODE(bref.unpack(reduced_cp_latency_r15_present, 1));
  HANDLE_CODE(bref.unpack(laa_params_v1530_present, 1));
  HANDLE_CODE(bref.unpack(ue_category_ul_v1530_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_eutra_cap_v1530_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_eutra_cap_v1530_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_params_v1530_present) {
    HANDLE_CODE(meas_params_v1530.unpack(bref));
  }
  if (other_params_v1530_present) {
    HANDLE_CODE(other_params_v1530.unpack(bref));
  }
  if (neigh_cell_si_acquisition_params_v1530_present) {
    HANDLE_CODE(neigh_cell_si_acquisition_params_v1530.unpack(bref));
  }
  if (mac_params_v1530_present) {
    HANDLE_CODE(mac_params_v1530.unpack(bref));
  }
  if (phy_layer_params_v1530_present) {
    HANDLE_CODE(phy_layer_params_v1530.unpack(bref));
  }
  if (rf_params_v1530_present) {
    HANDLE_CODE(rf_params_v1530.unpack(bref));
  }
  if (pdcp_params_v1530_present) {
    HANDLE_CODE(pdcp_params_v1530.unpack(bref));
  }
  if (ue_category_dl_v1530_present) {
    HANDLE_CODE(unpack_integer(ue_category_dl_v1530, bref, (uint8_t)22u, (uint8_t)26u));
  }
  if (ue_based_netw_perf_meas_params_v1530_present) {
    HANDLE_CODE(ue_based_netw_perf_meas_params_v1530.unpack(bref));
  }
  if (rlc_params_v1530_present) {
    HANDLE_CODE(rlc_params_v1530.unpack(bref));
  }
  if (sl_params_v1530_present) {
    HANDLE_CODE(sl_params_v1530.unpack(bref));
  }
  if (laa_params_v1530_present) {
    HANDLE_CODE(laa_params_v1530.unpack(bref));
  }
  if (ue_category_ul_v1530_present) {
    HANDLE_CODE(unpack_integer(ue_category_ul_v1530, bref, (uint8_t)22u, (uint8_t)26u));
  }
  if (fdd_add_ue_eutra_cap_v1530_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1530.unpack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1530_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1530.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_params_v1530_present) {
    j.write_fieldname("measParameters-v1530");
    meas_params_v1530.to_json(j);
  }
  if (other_params_v1530_present) {
    j.write_fieldname("otherParameters-v1530");
    other_params_v1530.to_json(j);
  }
  if (neigh_cell_si_acquisition_params_v1530_present) {
    j.write_fieldname("neighCellSI-AcquisitionParameters-v1530");
    neigh_cell_si_acquisition_params_v1530.to_json(j);
  }
  if (mac_params_v1530_present) {
    j.write_fieldname("mac-Parameters-v1530");
    mac_params_v1530.to_json(j);
  }
  if (phy_layer_params_v1530_present) {
    j.write_fieldname("phyLayerParameters-v1530");
    phy_layer_params_v1530.to_json(j);
  }
  if (rf_params_v1530_present) {
    j.write_fieldname("rf-Parameters-v1530");
    rf_params_v1530.to_json(j);
  }
  if (pdcp_params_v1530_present) {
    j.write_fieldname("pdcp-Parameters-v1530");
    pdcp_params_v1530.to_json(j);
  }
  if (ue_category_dl_v1530_present) {
    j.write_int("ue-CategoryDL-v1530", ue_category_dl_v1530);
  }
  if (ue_based_netw_perf_meas_params_v1530_present) {
    j.write_fieldname("ue-BasedNetwPerfMeasParameters-v1530");
    ue_based_netw_perf_meas_params_v1530.to_json(j);
  }
  if (rlc_params_v1530_present) {
    j.write_fieldname("rlc-Parameters-v1530");
    rlc_params_v1530.to_json(j);
  }
  if (sl_params_v1530_present) {
    j.write_fieldname("sl-Parameters-v1530");
    sl_params_v1530.to_json(j);
  }
  if (extended_nof_drbs_r15_present) {
    j.write_str("extendedNumberOfDRBs-r15", "supported");
  }
  if (reduced_cp_latency_r15_present) {
    j.write_str("reducedCP-Latency-r15", "supported");
  }
  if (laa_params_v1530_present) {
    j.write_fieldname("laa-Parameters-v1530");
    laa_params_v1530.to_json(j);
  }
  if (ue_category_ul_v1530_present) {
    j.write_int("ue-CategoryUL-v1530", ue_category_ul_v1530);
  }
  if (fdd_add_ue_eutra_cap_v1530_present) {
    j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v1530");
    fdd_add_ue_eutra_cap_v1530.to_json(j);
  }
  if (tdd_add_ue_eutra_cap_v1530_present) {
    j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v1530");
    tdd_add_ue_eutra_cap_v1530.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1520-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1520_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(meas_params_v1520.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1520_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(meas_params_v1520.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1520_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measParameters-v1520");
  meas_params_v1520.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v1510 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1510_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdcp_params_nr_r15_present, 1));

  if (pdcp_params_nr_r15_present) {
    HANDLE_CODE(pdcp_params_nr_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1510_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdcp_params_nr_r15_present, 1));

  if (pdcp_params_nr_r15_present) {
    HANDLE_CODE(pdcp_params_nr_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v1510_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcp_params_nr_r15_present) {
    j.write_fieldname("pdcp-ParametersNR-r15");
    pdcp_params_nr_r15.to_json(j);
  }
  j.end_obj();
}

// Other-Parameters-v1460 ::= SEQUENCE
SRSASN_CODE other_params_v1460_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_csg_si_report_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE other_params_v1460_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_csg_si_report_r14_present, 1));

  return SRSASN_SUCCESS;
}
void other_params_v1460_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (non_csg_si_report_r14_present) {
    j.write_str("nonCSG-SI-Reporting-r14", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1510-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1510_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(irat_params_nr_r15_present, 1));
  HANDLE_CODE(bref.pack(feature_sets_eutra_r15_present, 1));
  HANDLE_CODE(bref.pack(pdcp_params_nr_r15_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_eutra_cap_v1510_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_eutra_cap_v1510_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (irat_params_nr_r15_present) {
    HANDLE_CODE(irat_params_nr_r15.pack(bref));
  }
  if (feature_sets_eutra_r15_present) {
    HANDLE_CODE(feature_sets_eutra_r15.pack(bref));
  }
  if (pdcp_params_nr_r15_present) {
    HANDLE_CODE(pdcp_params_nr_r15.pack(bref));
  }
  if (fdd_add_ue_eutra_cap_v1510_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1510.pack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1510_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1510.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1510_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(irat_params_nr_r15_present, 1));
  HANDLE_CODE(bref.unpack(feature_sets_eutra_r15_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_params_nr_r15_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_eutra_cap_v1510_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_eutra_cap_v1510_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (irat_params_nr_r15_present) {
    HANDLE_CODE(irat_params_nr_r15.unpack(bref));
  }
  if (feature_sets_eutra_r15_present) {
    HANDLE_CODE(feature_sets_eutra_r15.unpack(bref));
  }
  if (pdcp_params_nr_r15_present) {
    HANDLE_CODE(pdcp_params_nr_r15.unpack(bref));
  }
  if (fdd_add_ue_eutra_cap_v1510_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1510.unpack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1510_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1510.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1510_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (irat_params_nr_r15_present) {
    j.write_fieldname("irat-ParametersNR-r15");
    irat_params_nr_r15.to_json(j);
  }
  if (feature_sets_eutra_r15_present) {
    j.write_fieldname("featureSetsEUTRA-r15");
    feature_sets_eutra_r15.to_json(j);
  }
  if (pdcp_params_nr_r15_present) {
    j.write_fieldname("pdcp-ParametersNR-r15");
    pdcp_params_nr_r15.to_json(j);
  }
  if (fdd_add_ue_eutra_cap_v1510_present) {
    j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v1510");
    fdd_add_ue_eutra_cap_v1510.to_json(j);
  }
  if (tdd_add_ue_eutra_cap_v1510_present) {
    j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v1510");
    tdd_add_ue_eutra_cap_v1510.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// OtherParameters-v1450 ::= SEQUENCE
SRSASN_CODE other_params_v1450_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(overheat_ind_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE other_params_v1450_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(overheat_ind_r14_present, 1));

  return SRSASN_SUCCESS;
}
void other_params_v1450_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (overheat_ind_r14_present) {
    j.write_str("overheatingInd-r14", "supported");
  }
  j.end_obj();
}

// PhyLayerParameters-v1450 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v1450_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ce_srs_enhancement_without_comb4_r14_present, 1));
  HANDLE_CODE(bref.pack(crs_less_dw_pts_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v1450_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ce_srs_enhancement_without_comb4_r14_present, 1));
  HANDLE_CODE(bref.unpack(crs_less_dw_pts_r14_present, 1));

  return SRSASN_SUCCESS;
}
void phy_layer_params_v1450_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ce_srs_enhancement_without_comb4_r14_present) {
    j.write_str("ce-SRS-EnhancementWithoutComb4-r14", "supported");
  }
  if (crs_less_dw_pts_r14_present) {
    j.write_str("crs-LessDwPTS-r14", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1460-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1460_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_category_dl_v1460_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_category_dl_v1460_present) {
    HANDLE_CODE(pack_integer(bref, ue_category_dl_v1460, (uint8_t)21u, (uint8_t)21u));
  }
  HANDLE_CODE(other_params_v1460.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1460_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_category_dl_v1460_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_category_dl_v1460_present) {
    HANDLE_CODE(unpack_integer(ue_category_dl_v1460, bref, (uint8_t)21u, (uint8_t)21u));
  }
  HANDLE_CODE(other_params_v1460.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1460_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_category_dl_v1460_present) {
    j.write_int("ue-CategoryDL-v1460", ue_category_dl_v1460);
  }
  j.write_fieldname("otherParameters-v1460");
  other_params_v1460.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// LWA-Parameters-v1440 ::= SEQUENCE
SRSASN_CODE lwa_params_v1440_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(lwa_rlc_um_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE lwa_params_v1440_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(lwa_rlc_um_r14_present, 1));

  return SRSASN_SUCCESS;
}
void lwa_params_v1440_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (lwa_rlc_um_r14_present) {
    j.write_str("lwa-RLC-UM-r14", "supported");
  }
  j.end_obj();
}

// MAC-Parameters-v1440 ::= SEQUENCE
SRSASN_CODE mac_params_v1440_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rai_support_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_params_v1440_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rai_support_r14_present, 1));

  return SRSASN_SUCCESS;
}
void mac_params_v1440_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rai_support_r14_present) {
    j.write_str("rai-Support-r14", "supported");
  }
  j.end_obj();
}

// MMTEL-Parameters-r14 ::= SEQUENCE
SRSASN_CODE mmtel_params_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(delay_budget_report_r14_present, 1));
  HANDLE_CODE(bref.pack(pusch_enhance_r14_present, 1));
  HANDLE_CODE(bref.pack(recommended_bit_rate_r14_present, 1));
  HANDLE_CODE(bref.pack(recommended_bit_rate_query_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mmtel_params_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(delay_budget_report_r14_present, 1));
  HANDLE_CODE(bref.unpack(pusch_enhance_r14_present, 1));
  HANDLE_CODE(bref.unpack(recommended_bit_rate_r14_present, 1));
  HANDLE_CODE(bref.unpack(recommended_bit_rate_query_r14_present, 1));

  return SRSASN_SUCCESS;
}
void mmtel_params_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (delay_budget_report_r14_present) {
    j.write_str("delayBudgetReporting-r14", "supported");
  }
  if (pusch_enhance_r14_present) {
    j.write_str("pusch-Enhancements-r14", "supported");
  }
  if (recommended_bit_rate_r14_present) {
    j.write_str("recommendedBitRate-r14", "supported");
  }
  if (recommended_bit_rate_query_r14_present) {
    j.write_str("recommendedBitRateQuery-r14", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1450-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1450_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_layer_params_v1450_present, 1));
  HANDLE_CODE(bref.pack(rf_params_v1450_present, 1));
  HANDLE_CODE(bref.pack(ue_category_dl_v1450_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (phy_layer_params_v1450_present) {
    HANDLE_CODE(phy_layer_params_v1450.pack(bref));
  }
  if (rf_params_v1450_present) {
    HANDLE_CODE(rf_params_v1450.pack(bref));
  }
  HANDLE_CODE(other_params_v1450.pack(bref));
  if (ue_category_dl_v1450_present) {
    HANDLE_CODE(pack_integer(bref, ue_category_dl_v1450, (uint8_t)20u, (uint8_t)20u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1450_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_layer_params_v1450_present, 1));
  HANDLE_CODE(bref.unpack(rf_params_v1450_present, 1));
  HANDLE_CODE(bref.unpack(ue_category_dl_v1450_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (phy_layer_params_v1450_present) {
    HANDLE_CODE(phy_layer_params_v1450.unpack(bref));
  }
  if (rf_params_v1450_present) {
    HANDLE_CODE(rf_params_v1450.unpack(bref));
  }
  HANDLE_CODE(other_params_v1450.unpack(bref));
  if (ue_category_dl_v1450_present) {
    HANDLE_CODE(unpack_integer(ue_category_dl_v1450, bref, (uint8_t)20u, (uint8_t)20u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1450_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_v1450_present) {
    j.write_fieldname("phyLayerParameters-v1450");
    phy_layer_params_v1450.to_json(j);
  }
  if (rf_params_v1450_present) {
    j.write_fieldname("rf-Parameters-v1450");
    rf_params_v1450.to_json(j);
  }
  j.write_fieldname("otherParameters-v1450");
  other_params_v1450.to_json(j);
  if (ue_category_dl_v1450_present) {
    j.write_int("ue-CategoryDL-v1450", ue_category_dl_v1450);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CE-Parameters-v1430 ::= SEQUENCE
SRSASN_CODE ce_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ce_switch_without_ho_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ce_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ce_switch_without_ho_r14_present, 1));

  return SRSASN_SUCCESS;
}
void ce_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ce_switch_without_ho_r14_present) {
    j.write_str("ce-SwitchWithoutHO-r14", "supported");
  }
  j.end_obj();
}

// HighSpeedEnhParameters-r14 ::= SEQUENCE
SRSASN_CODE high_speed_enh_params_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_enhance_r14_present, 1));
  HANDLE_CODE(bref.pack(demod_enhance_r14_present, 1));
  HANDLE_CODE(bref.pack(prach_enhance_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE high_speed_enh_params_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_enhance_r14_present, 1));
  HANDLE_CODE(bref.unpack(demod_enhance_r14_present, 1));
  HANDLE_CODE(bref.unpack(prach_enhance_r14_present, 1));

  return SRSASN_SUCCESS;
}
void high_speed_enh_params_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_enhance_r14_present) {
    j.write_str("measurementEnhancements-r14", "supported");
  }
  if (demod_enhance_r14_present) {
    j.write_str("demodulationEnhancements-r14", "supported");
  }
  if (prach_enhance_r14_present) {
    j.write_str("prach-Enhancements-r14", "supported");
  }
  j.end_obj();
}

// LAA-Parameters-v1430 ::= SEQUENCE
SRSASN_CODE laa_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cross_carrier_sched_laa_ul_r14_present, 1));
  HANDLE_CODE(bref.pack(ul_laa_r14_present, 1));
  HANDLE_CODE(bref.pack(two_step_sched_timing_info_r14_present, 1));
  HANDLE_CODE(bref.pack(uss_blind_decoding_adjustment_r14_present, 1));
  HANDLE_CODE(bref.pack(uss_blind_decoding_reduction_r14_present, 1));
  HANDLE_CODE(bref.pack(out_of_seq_grant_handling_r14_present, 1));

  if (two_step_sched_timing_info_r14_present) {
    HANDLE_CODE(two_step_sched_timing_info_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE laa_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cross_carrier_sched_laa_ul_r14_present, 1));
  HANDLE_CODE(bref.unpack(ul_laa_r14_present, 1));
  HANDLE_CODE(bref.unpack(two_step_sched_timing_info_r14_present, 1));
  HANDLE_CODE(bref.unpack(uss_blind_decoding_adjustment_r14_present, 1));
  HANDLE_CODE(bref.unpack(uss_blind_decoding_reduction_r14_present, 1));
  HANDLE_CODE(bref.unpack(out_of_seq_grant_handling_r14_present, 1));

  if (two_step_sched_timing_info_r14_present) {
    HANDLE_CODE(two_step_sched_timing_info_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void laa_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cross_carrier_sched_laa_ul_r14_present) {
    j.write_str("crossCarrierSchedulingLAA-UL-r14", "supported");
  }
  if (ul_laa_r14_present) {
    j.write_str("uplinkLAA-r14", "supported");
  }
  if (two_step_sched_timing_info_r14_present) {
    j.write_str("twoStepSchedulingTimingInfo-r14", two_step_sched_timing_info_r14.to_string());
  }
  if (uss_blind_decoding_adjustment_r14_present) {
    j.write_str("uss-BlindDecodingAdjustment-r14", "supported");
  }
  if (uss_blind_decoding_reduction_r14_present) {
    j.write_str("uss-BlindDecodingReduction-r14", "supported");
  }
  if (out_of_seq_grant_handling_r14_present) {
    j.write_str("outOfSequenceGrantHandling-r14", "supported");
  }
  j.end_obj();
}

const char* laa_params_v1430_s::two_step_sched_timing_info_r14_opts::to_string() const
{
  static const char* options[] = {"nPlus1", "nPlus2", "nPlus3"};
  return convert_enum_idx(options, 3, value, "laa_params_v1430_s::two_step_sched_timing_info_r14_e_");
}
uint8_t laa_params_v1430_s::two_step_sched_timing_info_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(options, 3, value, "laa_params_v1430_s::two_step_sched_timing_info_r14_e_");
}

// LWA-Parameters-v1430 ::= SEQUENCE
SRSASN_CODE lwa_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(lwa_ho_without_wt_change_r14_present, 1));
  HANDLE_CODE(bref.pack(lwa_ul_r14_present, 1));
  HANDLE_CODE(bref.pack(wlan_periodic_meas_r14_present, 1));
  HANDLE_CODE(bref.pack(wlan_report_any_wlan_r14_present, 1));
  HANDLE_CODE(bref.pack(wlan_supported_data_rate_r14_present, 1));

  if (wlan_supported_data_rate_r14_present) {
    HANDLE_CODE(pack_integer(bref, wlan_supported_data_rate_r14, (uint16_t)1u, (uint16_t)2048u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE lwa_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(lwa_ho_without_wt_change_r14_present, 1));
  HANDLE_CODE(bref.unpack(lwa_ul_r14_present, 1));
  HANDLE_CODE(bref.unpack(wlan_periodic_meas_r14_present, 1));
  HANDLE_CODE(bref.unpack(wlan_report_any_wlan_r14_present, 1));
  HANDLE_CODE(bref.unpack(wlan_supported_data_rate_r14_present, 1));

  if (wlan_supported_data_rate_r14_present) {
    HANDLE_CODE(unpack_integer(wlan_supported_data_rate_r14, bref, (uint16_t)1u, (uint16_t)2048u));
  }

  return SRSASN_SUCCESS;
}
void lwa_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (lwa_ho_without_wt_change_r14_present) {
    j.write_str("lwa-HO-WithoutWT-Change-r14", "supported");
  }
  if (lwa_ul_r14_present) {
    j.write_str("lwa-UL-r14", "supported");
  }
  if (wlan_periodic_meas_r14_present) {
    j.write_str("wlan-PeriodicMeas-r14", "supported");
  }
  if (wlan_report_any_wlan_r14_present) {
    j.write_str("wlan-ReportAnyWLAN-r14", "supported");
  }
  if (wlan_supported_data_rate_r14_present) {
    j.write_int("wlan-SupportedDataRate-r14", wlan_supported_data_rate_r14);
  }
  j.end_obj();
}

// LWIP-Parameters-v1430 ::= SEQUENCE
SRSASN_CODE lwip_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(lwip_aggregation_dl_r14_present, 1));
  HANDLE_CODE(bref.pack(lwip_aggregation_ul_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE lwip_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(lwip_aggregation_dl_r14_present, 1));
  HANDLE_CODE(bref.unpack(lwip_aggregation_ul_r14_present, 1));

  return SRSASN_SUCCESS;
}
void lwip_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (lwip_aggregation_dl_r14_present) {
    j.write_str("lwip-Aggregation-DL-r14", "supported");
  }
  if (lwip_aggregation_ul_r14_present) {
    j.write_str("lwip-Aggregation-UL-r14", "supported");
  }
  j.end_obj();
}

// MAC-Parameters-v1430 ::= SEQUENCE
SRSASN_CODE mac_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(short_sps_interv_fdd_r14_present, 1));
  HANDLE_CODE(bref.pack(short_sps_interv_tdd_r14_present, 1));
  HANDLE_CODE(bref.pack(skip_ul_dynamic_r14_present, 1));
  HANDLE_CODE(bref.pack(skip_ul_sps_r14_present, 1));
  HANDLE_CODE(bref.pack(multiple_ul_sps_r14_present, 1));
  HANDLE_CODE(bref.pack(data_inact_mon_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(short_sps_interv_fdd_r14_present, 1));
  HANDLE_CODE(bref.unpack(short_sps_interv_tdd_r14_present, 1));
  HANDLE_CODE(bref.unpack(skip_ul_dynamic_r14_present, 1));
  HANDLE_CODE(bref.unpack(skip_ul_sps_r14_present, 1));
  HANDLE_CODE(bref.unpack(multiple_ul_sps_r14_present, 1));
  HANDLE_CODE(bref.unpack(data_inact_mon_r14_present, 1));

  return SRSASN_SUCCESS;
}
void mac_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (short_sps_interv_fdd_r14_present) {
    j.write_str("shortSPS-IntervalFDD-r14", "supported");
  }
  if (short_sps_interv_tdd_r14_present) {
    j.write_str("shortSPS-IntervalTDD-r14", "supported");
  }
  if (skip_ul_dynamic_r14_present) {
    j.write_str("skipUplinkDynamic-r14", "supported");
  }
  if (skip_ul_sps_r14_present) {
    j.write_str("skipUplinkSPS-r14", "supported");
  }
  if (multiple_ul_sps_r14_present) {
    j.write_str("multipleUplinkSPS-r14", "supported");
  }
  if (data_inact_mon_r14_present) {
    j.write_str("dataInactMon-r14", "supported");
  }
  j.end_obj();
}

// MBMS-Parameters-v1430 ::= SEQUENCE
SRSASN_CODE mbms_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fembms_ded_cell_r14_present, 1));
  HANDLE_CODE(bref.pack(fembms_mixed_cell_r14_present, 1));
  HANDLE_CODE(bref.pack(subcarrier_spacing_mbms_khz7dot5_r14_present, 1));
  HANDLE_CODE(bref.pack(subcarrier_spacing_mbms_khz1dot25_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fembms_ded_cell_r14_present, 1));
  HANDLE_CODE(bref.unpack(fembms_mixed_cell_r14_present, 1));
  HANDLE_CODE(bref.unpack(subcarrier_spacing_mbms_khz7dot5_r14_present, 1));
  HANDLE_CODE(bref.unpack(subcarrier_spacing_mbms_khz1dot25_r14_present, 1));

  return SRSASN_SUCCESS;
}
void mbms_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fembms_ded_cell_r14_present) {
    j.write_str("fembmsDedicatedCell-r14", "supported");
  }
  if (fembms_mixed_cell_r14_present) {
    j.write_str("fembmsMixedCell-r14", "supported");
  }
  if (subcarrier_spacing_mbms_khz7dot5_r14_present) {
    j.write_str("subcarrierSpacingMBMS-khz7dot5-r14", "supported");
  }
  if (subcarrier_spacing_mbms_khz1dot25_r14_present) {
    j.write_str("subcarrierSpacingMBMS-khz1dot25-r14", "supported");
  }
  j.end_obj();
}

// MeasParameters-v1430 ::= SEQUENCE
SRSASN_CODE meas_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ce_meass_r14_present, 1));
  HANDLE_CODE(bref.pack(ncsg_r14_present, 1));
  HANDLE_CODE(bref.pack(short_meas_gap_r14_present, 1));
  HANDLE_CODE(bref.pack(per_serving_cell_meas_gap_r14_present, 1));
  HANDLE_CODE(bref.pack(non_uniform_gap_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ce_meass_r14_present, 1));
  HANDLE_CODE(bref.unpack(ncsg_r14_present, 1));
  HANDLE_CODE(bref.unpack(short_meas_gap_r14_present, 1));
  HANDLE_CODE(bref.unpack(per_serving_cell_meas_gap_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_uniform_gap_r14_present, 1));

  return SRSASN_SUCCESS;
}
void meas_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ce_meass_r14_present) {
    j.write_str("ceMeasurements-r14", "supported");
  }
  if (ncsg_r14_present) {
    j.write_str("ncsg-r14", "supported");
  }
  if (short_meas_gap_r14_present) {
    j.write_str("shortMeasurementGap-r14", "supported");
  }
  if (per_serving_cell_meas_gap_r14_present) {
    j.write_str("perServingCellMeasurementGap-r14", "supported");
  }
  if (non_uniform_gap_r14_present) {
    j.write_str("nonUniformGap-r14", "supported");
  }
  j.end_obj();
}

// MobilityParameters-r14 ::= SEQUENCE
SRSASN_CODE mob_params_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(make_before_break_r14_present, 1));
  HANDLE_CODE(bref.pack(rach_less_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_params_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(make_before_break_r14_present, 1));
  HANDLE_CODE(bref.unpack(rach_less_r14_present, 1));

  return SRSASN_SUCCESS;
}
void mob_params_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (make_before_break_r14_present) {
    j.write_str("makeBeforeBreak-r14", "supported");
  }
  if (rach_less_r14_present) {
    j.write_str("rach-Less-r14", "supported");
  }
  j.end_obj();
}

// Other-Parameters-v1430 ::= SEQUENCE
SRSASN_CODE other_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(bw_pref_ind_r14_present, 1));
  HANDLE_CODE(bref.pack(rlm_report_support_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE other_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(bw_pref_ind_r14_present, 1));
  HANDLE_CODE(bref.unpack(rlm_report_support_r14_present, 1));

  return SRSASN_SUCCESS;
}
void other_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (bw_pref_ind_r14_present) {
    j.write_str("bwPrefInd-r14", "supported");
  }
  if (rlm_report_support_r14_present) {
    j.write_str("rlm-ReportSupport-r14", "supported");
  }
  j.end_obj();
}

// PDCP-Parameters-v1430 ::= SEQUENCE
SRSASN_CODE pdcp_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(max_num_rohc_context_sessions_r14_present, 1));

  HANDLE_CODE(bref.pack(supported_ul_only_rohc_profiles_r14.profile0x0006_r14, 1));
  if (max_num_rohc_context_sessions_r14_present) {
    HANDLE_CODE(max_num_rohc_context_sessions_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(max_num_rohc_context_sessions_r14_present, 1));

  HANDLE_CODE(bref.unpack(supported_ul_only_rohc_profiles_r14.profile0x0006_r14, 1));
  if (max_num_rohc_context_sessions_r14_present) {
    HANDLE_CODE(max_num_rohc_context_sessions_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdcp_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("supportedUplinkOnlyROHC-Profiles-r14");
  j.start_obj();
  j.write_bool("profile0x0006-r14", supported_ul_only_rohc_profiles_r14.profile0x0006_r14);
  j.end_obj();
  if (max_num_rohc_context_sessions_r14_present) {
    j.write_str("maxNumberROHC-ContextSessions-r14", max_num_rohc_context_sessions_r14.to_string());
  }
  j.end_obj();
}

const char* pdcp_params_v1430_s::max_num_rohc_context_sessions_r14_opts::to_string() const
{
  static const char* options[] = {"cs2",
                                  "cs4",
                                  "cs8",
                                  "cs12",
                                  "cs16",
                                  "cs24",
                                  "cs32",
                                  "cs48",
                                  "cs64",
                                  "cs128",
                                  "cs256",
                                  "cs512",
                                  "cs1024",
                                  "cs16384",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pdcp_params_v1430_s::max_num_rohc_context_sessions_r14_e_");
}
uint16_t pdcp_params_v1430_s::max_num_rohc_context_sessions_r14_opts::to_number() const
{
  static const uint16_t options[] = {2, 4, 8, 12, 16, 24, 32, 48, 64, 128, 256, 512, 1024, 16384};
  return map_enum_number(options, 14, value, "pdcp_params_v1430_s::max_num_rohc_context_sessions_r14_e_");
}

// RLC-Parameters-v1430 ::= SEQUENCE
SRSASN_CODE rlc_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(extended_poll_byte_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(extended_poll_byte_r14_present, 1));

  return SRSASN_SUCCESS;
}
void rlc_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (extended_poll_byte_r14_present) {
    j.write_str("extendedPollByte-r14", "supported");
  }
  j.end_obj();
}

// UE-BasedNetwPerfMeasParameters-v1430 ::= SEQUENCE
SRSASN_CODE ue_based_netw_perf_meas_params_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(location_report_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_based_netw_perf_meas_params_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(location_report_r14_present, 1));

  return SRSASN_SUCCESS;
}
void ue_based_netw_perf_meas_params_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (location_report_r14_present) {
    j.write_str("locationReport-r14", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1440-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1440_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(lwa_params_v1440.pack(bref));
  HANDLE_CODE(mac_params_v1440.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1440_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(lwa_params_v1440.unpack(bref));
  HANDLE_CODE(mac_params_v1440.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1440_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("lwa-Parameters-v1440");
  lwa_params_v1440.to_json(j);
  j.write_fieldname("mac-Parameters-v1440");
  mac_params_v1440.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v1430 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_layer_params_v1430_present, 1));
  HANDLE_CODE(bref.pack(mmtel_params_r14_present, 1));

  if (phy_layer_params_v1430_present) {
    HANDLE_CODE(phy_layer_params_v1430.pack(bref));
  }
  if (mmtel_params_r14_present) {
    HANDLE_CODE(mmtel_params_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_layer_params_v1430_present, 1));
  HANDLE_CODE(bref.unpack(mmtel_params_r14_present, 1));

  if (phy_layer_params_v1430_present) {
    HANDLE_CODE(phy_layer_params_v1430.unpack(bref));
  }
  if (mmtel_params_r14_present) {
    HANDLE_CODE(mmtel_params_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_v1430_present) {
    j.write_fieldname("phyLayerParameters-v1430");
    phy_layer_params_v1430.to_json(j);
  }
  if (mmtel_params_r14_present) {
    j.write_fieldname("mmtel-Parameters-r14");
    mmtel_params_r14.to_json(j);
  }
  j.end_obj();
}

// Other-Parameters-v1360 ::= SEQUENCE
SRSASN_CODE other_params_v1360_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(in_dev_coex_ind_hardware_sharing_ind_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE other_params_v1360_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(in_dev_coex_ind_hardware_sharing_ind_r13_present, 1));

  return SRSASN_SUCCESS;
}
void other_params_v1360_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (in_dev_coex_ind_hardware_sharing_ind_r13_present) {
    j.write_str("inDeviceCoexInd-HardwareSharingInd-r13", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1430-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_category_dl_v1430_present, 1));
  HANDLE_CODE(bref.pack(ue_category_ul_v1430_present, 1));
  HANDLE_CODE(bref.pack(ue_category_ul_v1430b_present, 1));
  HANDLE_CODE(bref.pack(mac_params_v1430_present, 1));
  HANDLE_CODE(bref.pack(meas_params_v1430_present, 1));
  HANDLE_CODE(bref.pack(pdcp_params_v1430_present, 1));
  HANDLE_CODE(bref.pack(rf_params_v1430_present, 1));
  HANDLE_CODE(bref.pack(laa_params_v1430_present, 1));
  HANDLE_CODE(bref.pack(lwa_params_v1430_present, 1));
  HANDLE_CODE(bref.pack(lwip_params_v1430_present, 1));
  HANDLE_CODE(bref.pack(mmtel_params_r14_present, 1));
  HANDLE_CODE(bref.pack(mob_params_r14_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_eutra_cap_v1430_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_eutra_cap_v1430_present, 1));
  HANDLE_CODE(bref.pack(mbms_params_v1430_present, 1));
  HANDLE_CODE(bref.pack(sl_params_v1430_present, 1));
  HANDLE_CODE(bref.pack(ue_based_netw_perf_meas_params_v1430_present, 1));
  HANDLE_CODE(bref.pack(high_speed_enh_params_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(phy_layer_params_v1430.pack(bref));
  if (ue_category_ul_v1430_present) {
    HANDLE_CODE(ue_category_ul_v1430.pack(bref));
  }
  if (mac_params_v1430_present) {
    HANDLE_CODE(mac_params_v1430.pack(bref));
  }
  if (meas_params_v1430_present) {
    HANDLE_CODE(meas_params_v1430.pack(bref));
  }
  if (pdcp_params_v1430_present) {
    HANDLE_CODE(pdcp_params_v1430.pack(bref));
  }
  HANDLE_CODE(rlc_params_v1430.pack(bref));
  if (rf_params_v1430_present) {
    HANDLE_CODE(rf_params_v1430.pack(bref));
  }
  if (laa_params_v1430_present) {
    HANDLE_CODE(laa_params_v1430.pack(bref));
  }
  if (lwa_params_v1430_present) {
    HANDLE_CODE(lwa_params_v1430.pack(bref));
  }
  if (lwip_params_v1430_present) {
    HANDLE_CODE(lwip_params_v1430.pack(bref));
  }
  HANDLE_CODE(other_params_v1430.pack(bref));
  if (mmtel_params_r14_present) {
    HANDLE_CODE(mmtel_params_r14.pack(bref));
  }
  if (mob_params_r14_present) {
    HANDLE_CODE(mob_params_r14.pack(bref));
  }
  HANDLE_CODE(ce_params_v1430.pack(bref));
  if (fdd_add_ue_eutra_cap_v1430_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1430.pack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1430_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1430.pack(bref));
  }
  if (mbms_params_v1430_present) {
    HANDLE_CODE(mbms_params_v1430.pack(bref));
  }
  if (sl_params_v1430_present) {
    HANDLE_CODE(sl_params_v1430.pack(bref));
  }
  if (ue_based_netw_perf_meas_params_v1430_present) {
    HANDLE_CODE(ue_based_netw_perf_meas_params_v1430.pack(bref));
  }
  if (high_speed_enh_params_r14_present) {
    HANDLE_CODE(high_speed_enh_params_r14.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_category_dl_v1430_present, 1));
  HANDLE_CODE(bref.unpack(ue_category_ul_v1430_present, 1));
  HANDLE_CODE(bref.unpack(ue_category_ul_v1430b_present, 1));
  HANDLE_CODE(bref.unpack(mac_params_v1430_present, 1));
  HANDLE_CODE(bref.unpack(meas_params_v1430_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_params_v1430_present, 1));
  HANDLE_CODE(bref.unpack(rf_params_v1430_present, 1));
  HANDLE_CODE(bref.unpack(laa_params_v1430_present, 1));
  HANDLE_CODE(bref.unpack(lwa_params_v1430_present, 1));
  HANDLE_CODE(bref.unpack(lwip_params_v1430_present, 1));
  HANDLE_CODE(bref.unpack(mmtel_params_r14_present, 1));
  HANDLE_CODE(bref.unpack(mob_params_r14_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_eutra_cap_v1430_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_eutra_cap_v1430_present, 1));
  HANDLE_CODE(bref.unpack(mbms_params_v1430_present, 1));
  HANDLE_CODE(bref.unpack(sl_params_v1430_present, 1));
  HANDLE_CODE(bref.unpack(ue_based_netw_perf_meas_params_v1430_present, 1));
  HANDLE_CODE(bref.unpack(high_speed_enh_params_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(phy_layer_params_v1430.unpack(bref));
  if (ue_category_ul_v1430_present) {
    HANDLE_CODE(ue_category_ul_v1430.unpack(bref));
  }
  if (mac_params_v1430_present) {
    HANDLE_CODE(mac_params_v1430.unpack(bref));
  }
  if (meas_params_v1430_present) {
    HANDLE_CODE(meas_params_v1430.unpack(bref));
  }
  if (pdcp_params_v1430_present) {
    HANDLE_CODE(pdcp_params_v1430.unpack(bref));
  }
  HANDLE_CODE(rlc_params_v1430.unpack(bref));
  if (rf_params_v1430_present) {
    HANDLE_CODE(rf_params_v1430.unpack(bref));
  }
  if (laa_params_v1430_present) {
    HANDLE_CODE(laa_params_v1430.unpack(bref));
  }
  if (lwa_params_v1430_present) {
    HANDLE_CODE(lwa_params_v1430.unpack(bref));
  }
  if (lwip_params_v1430_present) {
    HANDLE_CODE(lwip_params_v1430.unpack(bref));
  }
  HANDLE_CODE(other_params_v1430.unpack(bref));
  if (mmtel_params_r14_present) {
    HANDLE_CODE(mmtel_params_r14.unpack(bref));
  }
  if (mob_params_r14_present) {
    HANDLE_CODE(mob_params_r14.unpack(bref));
  }
  HANDLE_CODE(ce_params_v1430.unpack(bref));
  if (fdd_add_ue_eutra_cap_v1430_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1430.unpack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1430_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1430.unpack(bref));
  }
  if (mbms_params_v1430_present) {
    HANDLE_CODE(mbms_params_v1430.unpack(bref));
  }
  if (sl_params_v1430_present) {
    HANDLE_CODE(sl_params_v1430.unpack(bref));
  }
  if (ue_based_netw_perf_meas_params_v1430_present) {
    HANDLE_CODE(ue_based_netw_perf_meas_params_v1430.unpack(bref));
  }
  if (high_speed_enh_params_r14_present) {
    HANDLE_CODE(high_speed_enh_params_r14.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("phyLayerParameters-v1430");
  phy_layer_params_v1430.to_json(j);
  if (ue_category_dl_v1430_present) {
    j.write_str("ue-CategoryDL-v1430", "m2");
  }
  if (ue_category_ul_v1430_present) {
    j.write_str("ue-CategoryUL-v1430", ue_category_ul_v1430.to_string());
  }
  if (ue_category_ul_v1430b_present) {
    j.write_str("ue-CategoryUL-v1430b", "n21");
  }
  if (mac_params_v1430_present) {
    j.write_fieldname("mac-Parameters-v1430");
    mac_params_v1430.to_json(j);
  }
  if (meas_params_v1430_present) {
    j.write_fieldname("measParameters-v1430");
    meas_params_v1430.to_json(j);
  }
  if (pdcp_params_v1430_present) {
    j.write_fieldname("pdcp-Parameters-v1430");
    pdcp_params_v1430.to_json(j);
  }
  j.write_fieldname("rlc-Parameters-v1430");
  rlc_params_v1430.to_json(j);
  if (rf_params_v1430_present) {
    j.write_fieldname("rf-Parameters-v1430");
    rf_params_v1430.to_json(j);
  }
  if (laa_params_v1430_present) {
    j.write_fieldname("laa-Parameters-v1430");
    laa_params_v1430.to_json(j);
  }
  if (lwa_params_v1430_present) {
    j.write_fieldname("lwa-Parameters-v1430");
    lwa_params_v1430.to_json(j);
  }
  if (lwip_params_v1430_present) {
    j.write_fieldname("lwip-Parameters-v1430");
    lwip_params_v1430.to_json(j);
  }
  j.write_fieldname("otherParameters-v1430");
  other_params_v1430.to_json(j);
  if (mmtel_params_r14_present) {
    j.write_fieldname("mmtel-Parameters-r14");
    mmtel_params_r14.to_json(j);
  }
  if (mob_params_r14_present) {
    j.write_fieldname("mobilityParameters-r14");
    mob_params_r14.to_json(j);
  }
  j.write_fieldname("ce-Parameters-v1430");
  ce_params_v1430.to_json(j);
  if (fdd_add_ue_eutra_cap_v1430_present) {
    j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v1430");
    fdd_add_ue_eutra_cap_v1430.to_json(j);
  }
  if (tdd_add_ue_eutra_cap_v1430_present) {
    j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v1430");
    tdd_add_ue_eutra_cap_v1430.to_json(j);
  }
  if (mbms_params_v1430_present) {
    j.write_fieldname("mbms-Parameters-v1430");
    mbms_params_v1430.to_json(j);
  }
  if (sl_params_v1430_present) {
    j.write_fieldname("sl-Parameters-v1430");
    sl_params_v1430.to_json(j);
  }
  if (ue_based_netw_perf_meas_params_v1430_present) {
    j.write_fieldname("ue-BasedNetwPerfMeasParameters-v1430");
    ue_based_netw_perf_meas_params_v1430.to_json(j);
  }
  if (high_speed_enh_params_r14_present) {
    j.write_fieldname("highSpeedEnhParameters-r14");
    high_speed_enh_params_r14.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* ue_eutra_cap_v1430_ies_s::ue_category_ul_v1430_opts::to_string() const
{
  static const char* options[] = {"n16", "n17", "n18", "n19", "n20", "m2"};
  return convert_enum_idx(options, 6, value, "ue_eutra_cap_v1430_ies_s::ue_category_ul_v1430_e_");
}
uint8_t ue_eutra_cap_v1430_ies_s::ue_category_ul_v1430_opts::to_number() const
{
  static const uint8_t options[] = {16, 17, 18, 19, 20, 2};
  return map_enum_number(options, 6, value, "ue_eutra_cap_v1430_ies_s::ue_category_ul_v1430_e_");
}

// CE-Parameters-v1350 ::= SEQUENCE
SRSASN_CODE ce_params_v1350_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(unicast_freq_hop_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ce_params_v1350_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(unicast_freq_hop_r13_present, 1));

  return SRSASN_SUCCESS;
}
void ce_params_v1350_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (unicast_freq_hop_r13_present) {
    j.write_str("unicastFrequencyHopping-r13", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1360-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1360_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(other_params_v1360_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (other_params_v1360_present) {
    HANDLE_CODE(other_params_v1360.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1360_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(other_params_v1360_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (other_params_v1360_present) {
    HANDLE_CODE(other_params_v1360.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1360_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (other_params_v1360_present) {
    j.write_fieldname("other-Parameters-v1360");
    other_params_v1360.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1350-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1350_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_category_dl_v1350_present, 1));
  HANDLE_CODE(bref.pack(ue_category_ul_v1350_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ce_params_v1350.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1350_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_category_dl_v1350_present, 1));
  HANDLE_CODE(bref.unpack(ue_category_ul_v1350_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ce_params_v1350.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1350_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_category_dl_v1350_present) {
    j.write_str("ue-CategoryDL-v1350", "oneBis");
  }
  if (ue_category_ul_v1350_present) {
    j.write_str("ue-CategoryUL-v1350", "oneBis");
  }
  j.write_fieldname("ce-Parameters-v1350");
  ce_params_v1350.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SCPTM-Parameters-r13 ::= SEQUENCE
SRSASN_CODE scptm_params_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scptm_parallel_reception_r13_present, 1));
  HANDLE_CODE(bref.pack(scptm_scell_r13_present, 1));
  HANDLE_CODE(bref.pack(scptm_non_serving_cell_r13_present, 1));
  HANDLE_CODE(bref.pack(scptm_async_dc_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE scptm_params_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scptm_parallel_reception_r13_present, 1));
  HANDLE_CODE(bref.unpack(scptm_scell_r13_present, 1));
  HANDLE_CODE(bref.unpack(scptm_non_serving_cell_r13_present, 1));
  HANDLE_CODE(bref.unpack(scptm_async_dc_r13_present, 1));

  return SRSASN_SUCCESS;
}
void scptm_params_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scptm_parallel_reception_r13_present) {
    j.write_str("scptm-ParallelReception-r13", "supported");
  }
  if (scptm_scell_r13_present) {
    j.write_str("scptm-SCell-r13", "supported");
  }
  if (scptm_non_serving_cell_r13_present) {
    j.write_str("scptm-NonServingCell-r13", "supported");
  }
  if (scptm_async_dc_r13_present) {
    j.write_str("scptm-AsyncDC-r13", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1340-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1340_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_category_ul_v1340_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_category_ul_v1340_present) {
    HANDLE_CODE(pack_integer(bref, ue_category_ul_v1340, (uint8_t)15u, (uint8_t)15u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1340_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_category_ul_v1340_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_category_ul_v1340_present) {
    HANDLE_CODE(unpack_integer(ue_category_ul_v1340, bref, (uint8_t)15u, (uint8_t)15u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1340_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_category_ul_v1340_present) {
    j.write_int("ue-CategoryUL-v1340", ue_category_ul_v1340);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CE-Parameters-v1320 ::= SEQUENCE
SRSASN_CODE ce_params_v1320_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(intra_freq_a3_ce_mode_a_r13_present, 1));
  HANDLE_CODE(bref.pack(intra_freq_a3_ce_mode_b_r13_present, 1));
  HANDLE_CODE(bref.pack(intra_freq_ho_ce_mode_a_r13_present, 1));
  HANDLE_CODE(bref.pack(intra_freq_ho_ce_mode_b_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ce_params_v1320_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(intra_freq_a3_ce_mode_a_r13_present, 1));
  HANDLE_CODE(bref.unpack(intra_freq_a3_ce_mode_b_r13_present, 1));
  HANDLE_CODE(bref.unpack(intra_freq_ho_ce_mode_a_r13_present, 1));
  HANDLE_CODE(bref.unpack(intra_freq_ho_ce_mode_b_r13_present, 1));

  return SRSASN_SUCCESS;
}
void ce_params_v1320_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (intra_freq_a3_ce_mode_a_r13_present) {
    j.write_str("intraFreqA3-CE-ModeA-r13", "supported");
  }
  if (intra_freq_a3_ce_mode_b_r13_present) {
    j.write_str("intraFreqA3-CE-ModeB-r13", "supported");
  }
  if (intra_freq_ho_ce_mode_a_r13_present) {
    j.write_str("intraFreqHO-CE-ModeA-r13", "supported");
  }
  if (intra_freq_ho_ce_mode_b_r13_present) {
    j.write_str("intraFreqHO-CE-ModeB-r13", "supported");
  }
  j.end_obj();
}

// PhyLayerParameters-v1310 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(aperiodic_csi_report_r13_present, 1));
  HANDLE_CODE(bref.pack(codebook_harq_ack_r13_present, 1));
  HANDLE_CODE(bref.pack(cross_carrier_sched_b5_c_r13_present, 1));
  HANDLE_CODE(bref.pack(fdd_harq_timing_tdd_r13_present, 1));
  HANDLE_CODE(bref.pack(max_num_upd_csi_proc_r13_present, 1));
  HANDLE_CODE(bref.pack(pucch_format4_r13_present, 1));
  HANDLE_CODE(bref.pack(pucch_format5_r13_present, 1));
  HANDLE_CODE(bref.pack(pucch_scell_r13_present, 1));
  HANDLE_CODE(bref.pack(spatial_bundling_harq_ack_r13_present, 1));
  HANDLE_CODE(bref.pack(supported_blind_decoding_r13_present, 1));
  HANDLE_CODE(bref.pack(uci_pusch_ext_r13_present, 1));
  HANDLE_CODE(bref.pack(crs_interf_mitigation_tm10_r13_present, 1));
  HANDLE_CODE(bref.pack(pdsch_collision_handling_r13_present, 1));

  if (aperiodic_csi_report_r13_present) {
    HANDLE_CODE(aperiodic_csi_report_r13.pack(bref));
  }
  if (codebook_harq_ack_r13_present) {
    HANDLE_CODE(codebook_harq_ack_r13.pack(bref));
  }
  if (max_num_upd_csi_proc_r13_present) {
    HANDLE_CODE(pack_integer(bref, max_num_upd_csi_proc_r13, (uint8_t)5u, (uint8_t)32u));
  }
  if (supported_blind_decoding_r13_present) {
    HANDLE_CODE(bref.pack(supported_blind_decoding_r13.max_num_decoding_r13_present, 1));
    HANDLE_CODE(bref.pack(supported_blind_decoding_r13.pdcch_candidate_reductions_r13_present, 1));
    HANDLE_CODE(bref.pack(supported_blind_decoding_r13.skip_monitoring_dci_format0_minus1_a_r13_present, 1));
    if (supported_blind_decoding_r13.max_num_decoding_r13_present) {
      HANDLE_CODE(pack_integer(bref, supported_blind_decoding_r13.max_num_decoding_r13, (uint8_t)1u, (uint8_t)32u));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(aperiodic_csi_report_r13_present, 1));
  HANDLE_CODE(bref.unpack(codebook_harq_ack_r13_present, 1));
  HANDLE_CODE(bref.unpack(cross_carrier_sched_b5_c_r13_present, 1));
  HANDLE_CODE(bref.unpack(fdd_harq_timing_tdd_r13_present, 1));
  HANDLE_CODE(bref.unpack(max_num_upd_csi_proc_r13_present, 1));
  HANDLE_CODE(bref.unpack(pucch_format4_r13_present, 1));
  HANDLE_CODE(bref.unpack(pucch_format5_r13_present, 1));
  HANDLE_CODE(bref.unpack(pucch_scell_r13_present, 1));
  HANDLE_CODE(bref.unpack(spatial_bundling_harq_ack_r13_present, 1));
  HANDLE_CODE(bref.unpack(supported_blind_decoding_r13_present, 1));
  HANDLE_CODE(bref.unpack(uci_pusch_ext_r13_present, 1));
  HANDLE_CODE(bref.unpack(crs_interf_mitigation_tm10_r13_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_collision_handling_r13_present, 1));

  if (aperiodic_csi_report_r13_present) {
    HANDLE_CODE(aperiodic_csi_report_r13.unpack(bref));
  }
  if (codebook_harq_ack_r13_present) {
    HANDLE_CODE(codebook_harq_ack_r13.unpack(bref));
  }
  if (max_num_upd_csi_proc_r13_present) {
    HANDLE_CODE(unpack_integer(max_num_upd_csi_proc_r13, bref, (uint8_t)5u, (uint8_t)32u));
  }
  if (supported_blind_decoding_r13_present) {
    HANDLE_CODE(bref.unpack(supported_blind_decoding_r13.max_num_decoding_r13_present, 1));
    HANDLE_CODE(bref.unpack(supported_blind_decoding_r13.pdcch_candidate_reductions_r13_present, 1));
    HANDLE_CODE(bref.unpack(supported_blind_decoding_r13.skip_monitoring_dci_format0_minus1_a_r13_present, 1));
    if (supported_blind_decoding_r13.max_num_decoding_r13_present) {
      HANDLE_CODE(unpack_integer(supported_blind_decoding_r13.max_num_decoding_r13, bref, (uint8_t)1u, (uint8_t)32u));
    }
  }

  return SRSASN_SUCCESS;
}
void phy_layer_params_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (aperiodic_csi_report_r13_present) {
    j.write_str("aperiodicCSI-Reporting-r13", aperiodic_csi_report_r13.to_string());
  }
  if (codebook_harq_ack_r13_present) {
    j.write_str("codebook-HARQ-ACK-r13", codebook_harq_ack_r13.to_string());
  }
  if (cross_carrier_sched_b5_c_r13_present) {
    j.write_str("crossCarrierScheduling-B5C-r13", "supported");
  }
  if (fdd_harq_timing_tdd_r13_present) {
    j.write_str("fdd-HARQ-TimingTDD-r13", "supported");
  }
  if (max_num_upd_csi_proc_r13_present) {
    j.write_int("maxNumberUpdatedCSI-Proc-r13", max_num_upd_csi_proc_r13);
  }
  if (pucch_format4_r13_present) {
    j.write_str("pucch-Format4-r13", "supported");
  }
  if (pucch_format5_r13_present) {
    j.write_str("pucch-Format5-r13", "supported");
  }
  if (pucch_scell_r13_present) {
    j.write_str("pucch-SCell-r13", "supported");
  }
  if (spatial_bundling_harq_ack_r13_present) {
    j.write_str("spatialBundling-HARQ-ACK-r13", "supported");
  }
  if (supported_blind_decoding_r13_present) {
    j.write_fieldname("supportedBlindDecoding-r13");
    j.start_obj();
    if (supported_blind_decoding_r13.max_num_decoding_r13_present) {
      j.write_int("maxNumberDecoding-r13", supported_blind_decoding_r13.max_num_decoding_r13);
    }
    if (supported_blind_decoding_r13.pdcch_candidate_reductions_r13_present) {
      j.write_str("pdcch-CandidateReductions-r13", "supported");
    }
    if (supported_blind_decoding_r13.skip_monitoring_dci_format0_minus1_a_r13_present) {
      j.write_str("skipMonitoringDCI-Format0-1A-r13", "supported");
    }
    j.end_obj();
  }
  if (uci_pusch_ext_r13_present) {
    j.write_str("uci-PUSCH-Ext-r13", "supported");
  }
  if (crs_interf_mitigation_tm10_r13_present) {
    j.write_str("crs-InterfMitigationTM10-r13", "supported");
  }
  if (pdsch_collision_handling_r13_present) {
    j.write_str("pdsch-CollisionHandling-r13", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1330-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1330_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_category_dl_v1330_present, 1));
  HANDLE_CODE(bref.pack(phy_layer_params_v1330_present, 1));
  HANDLE_CODE(bref.pack(ue_ce_need_ul_gaps_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_category_dl_v1330_present) {
    HANDLE_CODE(pack_integer(bref, ue_category_dl_v1330, (uint8_t)18u, (uint8_t)19u));
  }
  if (phy_layer_params_v1330_present) {
    HANDLE_CODE(phy_layer_params_v1330.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1330_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_category_dl_v1330_present, 1));
  HANDLE_CODE(bref.unpack(phy_layer_params_v1330_present, 1));
  HANDLE_CODE(bref.unpack(ue_ce_need_ul_gaps_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_category_dl_v1330_present) {
    HANDLE_CODE(unpack_integer(ue_category_dl_v1330, bref, (uint8_t)18u, (uint8_t)19u));
  }
  if (phy_layer_params_v1330_present) {
    HANDLE_CODE(phy_layer_params_v1330.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1330_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_category_dl_v1330_present) {
    j.write_int("ue-CategoryDL-v1330", ue_category_dl_v1330);
  }
  if (phy_layer_params_v1330_present) {
    j.write_fieldname("phyLayerParameters-v1330");
    phy_layer_params_v1330.to_json(j);
  }
  if (ue_ce_need_ul_gaps_r13_present) {
    j.write_str("ue-CE-NeedULGaps-r13", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v1320 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1320_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_layer_params_v1320_present, 1));
  HANDLE_CODE(bref.pack(scptm_params_r13_present, 1));

  if (phy_layer_params_v1320_present) {
    HANDLE_CODE(phy_layer_params_v1320.pack(bref));
  }
  if (scptm_params_r13_present) {
    HANDLE_CODE(scptm_params_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1320_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_layer_params_v1320_present, 1));
  HANDLE_CODE(bref.unpack(scptm_params_r13_present, 1));

  if (phy_layer_params_v1320_present) {
    HANDLE_CODE(phy_layer_params_v1320.unpack(bref));
  }
  if (scptm_params_r13_present) {
    HANDLE_CODE(scptm_params_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v1320_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_v1320_present) {
    j.write_fieldname("phyLayerParameters-v1320");
    phy_layer_params_v1320.to_json(j);
  }
  if (scptm_params_r13_present) {
    j.write_fieldname("scptm-Parameters-r13");
    scptm_params_r13.to_json(j);
  }
  j.end_obj();
}

// CE-Parameters-r13 ::= SEQUENCE
SRSASN_CODE ce_params_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ce_mode_a_r13_present, 1));
  HANDLE_CODE(bref.pack(ce_mode_b_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ce_params_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ce_mode_a_r13_present, 1));
  HANDLE_CODE(bref.unpack(ce_mode_b_r13_present, 1));

  return SRSASN_SUCCESS;
}
void ce_params_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ce_mode_a_r13_present) {
    j.write_str("ce-ModeA-r13", "supported");
  }
  if (ce_mode_b_r13_present) {
    j.write_str("ce-ModeB-r13", "supported");
  }
  j.end_obj();
}

// DC-Parameters-v1310 ::= SEQUENCE
SRSASN_CODE dc_params_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdcp_transfer_split_ul_r13_present, 1));
  HANDLE_CODE(bref.pack(ue_sstd_meas_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dc_params_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdcp_transfer_split_ul_r13_present, 1));
  HANDLE_CODE(bref.unpack(ue_sstd_meas_r13_present, 1));

  return SRSASN_SUCCESS;
}
void dc_params_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcp_transfer_split_ul_r13_present) {
    j.write_str("pdcp-TransferSplitUL-r13", "supported");
  }
  if (ue_sstd_meas_r13_present) {
    j.write_str("ue-SSTD-Meas-r13", "supported");
  }
  j.end_obj();
}

// LAA-Parameters-r13 ::= SEQUENCE
SRSASN_CODE laa_params_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cross_carrier_sched_laa_dl_r13_present, 1));
  HANDLE_CODE(bref.pack(csi_rs_drs_rrm_meass_laa_r13_present, 1));
  HANDLE_CODE(bref.pack(dl_laa_r13_present, 1));
  HANDLE_CODE(bref.pack(ending_dw_pts_r13_present, 1));
  HANDLE_CODE(bref.pack(second_slot_start_position_r13_present, 1));
  HANDLE_CODE(bref.pack(tm9_laa_r13_present, 1));
  HANDLE_CODE(bref.pack(tm10_laa_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE laa_params_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cross_carrier_sched_laa_dl_r13_present, 1));
  HANDLE_CODE(bref.unpack(csi_rs_drs_rrm_meass_laa_r13_present, 1));
  HANDLE_CODE(bref.unpack(dl_laa_r13_present, 1));
  HANDLE_CODE(bref.unpack(ending_dw_pts_r13_present, 1));
  HANDLE_CODE(bref.unpack(second_slot_start_position_r13_present, 1));
  HANDLE_CODE(bref.unpack(tm9_laa_r13_present, 1));
  HANDLE_CODE(bref.unpack(tm10_laa_r13_present, 1));

  return SRSASN_SUCCESS;
}
void laa_params_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cross_carrier_sched_laa_dl_r13_present) {
    j.write_str("crossCarrierSchedulingLAA-DL-r13", "supported");
  }
  if (csi_rs_drs_rrm_meass_laa_r13_present) {
    j.write_str("csi-RS-DRS-RRM-MeasurementsLAA-r13", "supported");
  }
  if (dl_laa_r13_present) {
    j.write_str("downlinkLAA-r13", "supported");
  }
  if (ending_dw_pts_r13_present) {
    j.write_str("endingDwPTS-r13", "supported");
  }
  if (second_slot_start_position_r13_present) {
    j.write_str("secondSlotStartingPosition-r13", "supported");
  }
  if (tm9_laa_r13_present) {
    j.write_str("tm9-LAA-r13", "supported");
  }
  if (tm10_laa_r13_present) {
    j.write_str("tm10-LAA-r13", "supported");
  }
  j.end_obj();
}

// LWA-Parameters-r13 ::= SEQUENCE
SRSASN_CODE lwa_params_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(lwa_r13_present, 1));
  HANDLE_CODE(bref.pack(lwa_split_bearer_r13_present, 1));
  HANDLE_CODE(bref.pack(wlan_mac_address_r13_present, 1));
  HANDLE_CODE(bref.pack(lwa_buffer_size_r13_present, 1));

  if (wlan_mac_address_r13_present) {
    HANDLE_CODE(wlan_mac_address_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE lwa_params_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(lwa_r13_present, 1));
  HANDLE_CODE(bref.unpack(lwa_split_bearer_r13_present, 1));
  HANDLE_CODE(bref.unpack(wlan_mac_address_r13_present, 1));
  HANDLE_CODE(bref.unpack(lwa_buffer_size_r13_present, 1));

  if (wlan_mac_address_r13_present) {
    HANDLE_CODE(wlan_mac_address_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void lwa_params_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (lwa_r13_present) {
    j.write_str("lwa-r13", "supported");
  }
  if (lwa_split_bearer_r13_present) {
    j.write_str("lwa-SplitBearer-r13", "supported");
  }
  if (wlan_mac_address_r13_present) {
    j.write_str("wlan-MAC-Address-r13", wlan_mac_address_r13.to_string());
  }
  if (lwa_buffer_size_r13_present) {
    j.write_str("lwa-BufferSize-r13", "supported");
  }
  j.end_obj();
}

// LWIP-Parameters-r13 ::= SEQUENCE
SRSASN_CODE lwip_params_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(lwip_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE lwip_params_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(lwip_r13_present, 1));

  return SRSASN_SUCCESS;
}
void lwip_params_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (lwip_r13_present) {
    j.write_str("lwip-r13", "supported");
  }
  j.end_obj();
}

// MAC-Parameters-v1310 ::= SEQUENCE
SRSASN_CODE mac_params_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(extended_mac_len_field_r13_present, 1));
  HANDLE_CODE(bref.pack(extended_long_drx_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_params_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(extended_mac_len_field_r13_present, 1));
  HANDLE_CODE(bref.unpack(extended_long_drx_r13_present, 1));

  return SRSASN_SUCCESS;
}
void mac_params_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (extended_mac_len_field_r13_present) {
    j.write_str("extendedMAC-LengthField-r13", "supported");
  }
  if (extended_long_drx_r13_present) {
    j.write_str("extendedLongDRX-r13", "supported");
  }
  j.end_obj();
}

// MeasParameters-v1310 ::= SEQUENCE
SRSASN_CODE meas_params_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rs_sinr_meas_r13_present, 1));
  HANDLE_CODE(bref.pack(white_cell_list_r13_present, 1));
  HANDLE_CODE(bref.pack(extended_max_obj_id_r13_present, 1));
  HANDLE_CODE(bref.pack(ul_pdcp_delay_r13_present, 1));
  HANDLE_CODE(bref.pack(extended_freq_priorities_r13_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_report_r13_present, 1));
  HANDLE_CODE(bref.pack(rssi_and_ch_occupancy_report_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_params_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rs_sinr_meas_r13_present, 1));
  HANDLE_CODE(bref.unpack(white_cell_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(extended_max_obj_id_r13_present, 1));
  HANDLE_CODE(bref.unpack(ul_pdcp_delay_r13_present, 1));
  HANDLE_CODE(bref.unpack(extended_freq_priorities_r13_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_report_r13_present, 1));
  HANDLE_CODE(bref.unpack(rssi_and_ch_occupancy_report_r13_present, 1));

  return SRSASN_SUCCESS;
}
void meas_params_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rs_sinr_meas_r13_present) {
    j.write_str("rs-SINR-Meas-r13", "supported");
  }
  if (white_cell_list_r13_present) {
    j.write_str("whiteCellList-r13", "supported");
  }
  if (extended_max_obj_id_r13_present) {
    j.write_str("extendedMaxObjectId-r13", "supported");
  }
  if (ul_pdcp_delay_r13_present) {
    j.write_str("ul-PDCP-Delay-r13", "supported");
  }
  if (extended_freq_priorities_r13_present) {
    j.write_str("extendedFreqPriorities-r13", "supported");
  }
  if (multi_band_info_report_r13_present) {
    j.write_str("multiBandInfoReport-r13", "supported");
  }
  if (rssi_and_ch_occupancy_report_r13_present) {
    j.write_str("rssi-AndChannelOccupancyReporting-r13", "supported");
  }
  j.end_obj();
}

// PDCP-Parameters-v1310 ::= SEQUENCE
SRSASN_CODE pdcp_params_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdcp_sn_ext_minus18bits_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_params_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdcp_sn_ext_minus18bits_r13_present, 1));

  return SRSASN_SUCCESS;
}
void pdcp_params_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcp_sn_ext_minus18bits_r13_present) {
    j.write_str("pdcp-SN-Extension-18bits-r13", "supported");
  }
  j.end_obj();
}

// RLC-Parameters-v1310 ::= SEQUENCE
SRSASN_CODE rlc_params_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(extended_rlc_sn_so_field_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_params_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(extended_rlc_sn_so_field_r13_present, 1));

  return SRSASN_SUCCESS;
}
void rlc_params_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (extended_rlc_sn_so_field_r13_present) {
    j.write_str("extendedRLC-SN-SO-Field-r13", "supported");
  }
  j.end_obj();
}

// SL-Parameters-v1310 ::= SEQUENCE
SRSASN_CODE sl_params_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(disc_sys_info_report_r13_present, 1));
  HANDLE_CODE(bref.pack(comm_multiple_tx_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_inter_freq_tx_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_periodic_slss_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_params_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(disc_sys_info_report_r13_present, 1));
  HANDLE_CODE(bref.unpack(comm_multiple_tx_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_inter_freq_tx_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_periodic_slss_r13_present, 1));

  return SRSASN_SUCCESS;
}
void sl_params_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (disc_sys_info_report_r13_present) {
    j.write_str("discSysInfoReporting-r13", "supported");
  }
  if (comm_multiple_tx_r13_present) {
    j.write_str("commMultipleTx-r13", "supported");
  }
  if (disc_inter_freq_tx_r13_present) {
    j.write_str("discInterFreqTx-r13", "supported");
  }
  if (disc_periodic_slss_r13_present) {
    j.write_str("discPeriodicSLSS-r13", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1320-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1320_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ce_params_v1320_present, 1));
  HANDLE_CODE(bref.pack(phy_layer_params_v1320_present, 1));
  HANDLE_CODE(bref.pack(rf_params_v1320_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_eutra_cap_v1320_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_eutra_cap_v1320_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ce_params_v1320_present) {
    HANDLE_CODE(ce_params_v1320.pack(bref));
  }
  if (phy_layer_params_v1320_present) {
    HANDLE_CODE(phy_layer_params_v1320.pack(bref));
  }
  if (rf_params_v1320_present) {
    HANDLE_CODE(rf_params_v1320.pack(bref));
  }
  if (fdd_add_ue_eutra_cap_v1320_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1320.pack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1320_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1320.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1320_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ce_params_v1320_present, 1));
  HANDLE_CODE(bref.unpack(phy_layer_params_v1320_present, 1));
  HANDLE_CODE(bref.unpack(rf_params_v1320_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_eutra_cap_v1320_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_eutra_cap_v1320_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ce_params_v1320_present) {
    HANDLE_CODE(ce_params_v1320.unpack(bref));
  }
  if (phy_layer_params_v1320_present) {
    HANDLE_CODE(phy_layer_params_v1320.unpack(bref));
  }
  if (rf_params_v1320_present) {
    HANDLE_CODE(rf_params_v1320.unpack(bref));
  }
  if (fdd_add_ue_eutra_cap_v1320_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1320.unpack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1320_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1320.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1320_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ce_params_v1320_present) {
    j.write_fieldname("ce-Parameters-v1320");
    ce_params_v1320.to_json(j);
  }
  if (phy_layer_params_v1320_present) {
    j.write_fieldname("phyLayerParameters-v1320");
    phy_layer_params_v1320.to_json(j);
  }
  if (rf_params_v1320_present) {
    j.write_fieldname("rf-Parameters-v1320");
    rf_params_v1320.to_json(j);
  }
  if (fdd_add_ue_eutra_cap_v1320_present) {
    j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v1320");
    fdd_add_ue_eutra_cap_v1320.to_json(j);
  }
  if (tdd_add_ue_eutra_cap_v1320_present) {
    j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v1320");
    tdd_add_ue_eutra_cap_v1320.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v1310 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_layer_params_v1310_present, 1));

  if (phy_layer_params_v1310_present) {
    HANDLE_CODE(phy_layer_params_v1310.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_layer_params_v1310_present, 1));

  if (phy_layer_params_v1310_present) {
    HANDLE_CODE(phy_layer_params_v1310.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_v1310_present) {
    j.write_fieldname("phyLayerParameters-v1310");
    phy_layer_params_v1310.to_json(j);
  }
  j.end_obj();
}

// WLAN-IW-Parameters-v1310 ::= SEQUENCE
SRSASN_CODE wlan_iw_params_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rclwi_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_iw_params_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rclwi_r13_present, 1));

  return SRSASN_SUCCESS;
}
void wlan_iw_params_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rclwi_r13_present) {
    j.write_str("rclwi-r13", "supported");
  }
  j.end_obj();
}

// PhyLayerParameters-v1280 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v1280_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(alt_tbs_indices_r12_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v1280_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(alt_tbs_indices_r12_present, 1));

  return SRSASN_SUCCESS;
}
void phy_layer_params_v1280_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (alt_tbs_indices_r12_present) {
    j.write_str("alternativeTBS-Indices-r12", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1310-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1310_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_category_dl_v1310_present, 1));
  HANDLE_CODE(bref.pack(ue_category_ul_v1310_present, 1));
  HANDLE_CODE(bref.pack(mac_params_v1310_present, 1));
  HANDLE_CODE(bref.pack(phy_layer_params_v1310_present, 1));
  HANDLE_CODE(bref.pack(rf_params_v1310_present, 1));
  HANDLE_CODE(bref.pack(meas_params_v1310_present, 1));
  HANDLE_CODE(bref.pack(dc_params_v1310_present, 1));
  HANDLE_CODE(bref.pack(sl_params_v1310_present, 1));
  HANDLE_CODE(bref.pack(scptm_params_r13_present, 1));
  HANDLE_CODE(bref.pack(ce_params_r13_present, 1));
  HANDLE_CODE(bref.pack(laa_params_r13_present, 1));
  HANDLE_CODE(bref.pack(lwa_params_r13_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_eutra_cap_v1310_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_eutra_cap_v1310_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_category_dl_v1310_present) {
    HANDLE_CODE(ue_category_dl_v1310.pack(bref));
  }
  if (ue_category_ul_v1310_present) {
    HANDLE_CODE(ue_category_ul_v1310.pack(bref));
  }
  HANDLE_CODE(pdcp_params_v1310.pack(bref));
  HANDLE_CODE(rlc_params_v1310.pack(bref));
  if (mac_params_v1310_present) {
    HANDLE_CODE(mac_params_v1310.pack(bref));
  }
  if (phy_layer_params_v1310_present) {
    HANDLE_CODE(phy_layer_params_v1310.pack(bref));
  }
  if (rf_params_v1310_present) {
    HANDLE_CODE(rf_params_v1310.pack(bref));
  }
  if (meas_params_v1310_present) {
    HANDLE_CODE(meas_params_v1310.pack(bref));
  }
  if (dc_params_v1310_present) {
    HANDLE_CODE(dc_params_v1310.pack(bref));
  }
  if (sl_params_v1310_present) {
    HANDLE_CODE(sl_params_v1310.pack(bref));
  }
  if (scptm_params_r13_present) {
    HANDLE_CODE(scptm_params_r13.pack(bref));
  }
  if (ce_params_r13_present) {
    HANDLE_CODE(ce_params_r13.pack(bref));
  }
  HANDLE_CODE(inter_rat_params_wlan_r13.pack(bref));
  if (laa_params_r13_present) {
    HANDLE_CODE(laa_params_r13.pack(bref));
  }
  if (lwa_params_r13_present) {
    HANDLE_CODE(lwa_params_r13.pack(bref));
  }
  HANDLE_CODE(wlan_iw_params_v1310.pack(bref));
  HANDLE_CODE(lwip_params_r13.pack(bref));
  if (fdd_add_ue_eutra_cap_v1310_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1310.pack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1310_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1310.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1310_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_category_dl_v1310_present, 1));
  HANDLE_CODE(bref.unpack(ue_category_ul_v1310_present, 1));
  HANDLE_CODE(bref.unpack(mac_params_v1310_present, 1));
  HANDLE_CODE(bref.unpack(phy_layer_params_v1310_present, 1));
  HANDLE_CODE(bref.unpack(rf_params_v1310_present, 1));
  HANDLE_CODE(bref.unpack(meas_params_v1310_present, 1));
  HANDLE_CODE(bref.unpack(dc_params_v1310_present, 1));
  HANDLE_CODE(bref.unpack(sl_params_v1310_present, 1));
  HANDLE_CODE(bref.unpack(scptm_params_r13_present, 1));
  HANDLE_CODE(bref.unpack(ce_params_r13_present, 1));
  HANDLE_CODE(bref.unpack(laa_params_r13_present, 1));
  HANDLE_CODE(bref.unpack(lwa_params_r13_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_eutra_cap_v1310_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_eutra_cap_v1310_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_category_dl_v1310_present) {
    HANDLE_CODE(ue_category_dl_v1310.unpack(bref));
  }
  if (ue_category_ul_v1310_present) {
    HANDLE_CODE(ue_category_ul_v1310.unpack(bref));
  }
  HANDLE_CODE(pdcp_params_v1310.unpack(bref));
  HANDLE_CODE(rlc_params_v1310.unpack(bref));
  if (mac_params_v1310_present) {
    HANDLE_CODE(mac_params_v1310.unpack(bref));
  }
  if (phy_layer_params_v1310_present) {
    HANDLE_CODE(phy_layer_params_v1310.unpack(bref));
  }
  if (rf_params_v1310_present) {
    HANDLE_CODE(rf_params_v1310.unpack(bref));
  }
  if (meas_params_v1310_present) {
    HANDLE_CODE(meas_params_v1310.unpack(bref));
  }
  if (dc_params_v1310_present) {
    HANDLE_CODE(dc_params_v1310.unpack(bref));
  }
  if (sl_params_v1310_present) {
    HANDLE_CODE(sl_params_v1310.unpack(bref));
  }
  if (scptm_params_r13_present) {
    HANDLE_CODE(scptm_params_r13.unpack(bref));
  }
  if (ce_params_r13_present) {
    HANDLE_CODE(ce_params_r13.unpack(bref));
  }
  HANDLE_CODE(inter_rat_params_wlan_r13.unpack(bref));
  if (laa_params_r13_present) {
    HANDLE_CODE(laa_params_r13.unpack(bref));
  }
  if (lwa_params_r13_present) {
    HANDLE_CODE(lwa_params_r13.unpack(bref));
  }
  HANDLE_CODE(wlan_iw_params_v1310.unpack(bref));
  HANDLE_CODE(lwip_params_r13.unpack(bref));
  if (fdd_add_ue_eutra_cap_v1310_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1310.unpack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1310_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1310.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1310_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_category_dl_v1310_present) {
    j.write_str("ue-CategoryDL-v1310", ue_category_dl_v1310.to_string());
  }
  if (ue_category_ul_v1310_present) {
    j.write_str("ue-CategoryUL-v1310", ue_category_ul_v1310.to_string());
  }
  j.write_fieldname("pdcp-Parameters-v1310");
  pdcp_params_v1310.to_json(j);
  j.write_fieldname("rlc-Parameters-v1310");
  rlc_params_v1310.to_json(j);
  if (mac_params_v1310_present) {
    j.write_fieldname("mac-Parameters-v1310");
    mac_params_v1310.to_json(j);
  }
  if (phy_layer_params_v1310_present) {
    j.write_fieldname("phyLayerParameters-v1310");
    phy_layer_params_v1310.to_json(j);
  }
  if (rf_params_v1310_present) {
    j.write_fieldname("rf-Parameters-v1310");
    rf_params_v1310.to_json(j);
  }
  if (meas_params_v1310_present) {
    j.write_fieldname("measParameters-v1310");
    meas_params_v1310.to_json(j);
  }
  if (dc_params_v1310_present) {
    j.write_fieldname("dc-Parameters-v1310");
    dc_params_v1310.to_json(j);
  }
  if (sl_params_v1310_present) {
    j.write_fieldname("sl-Parameters-v1310");
    sl_params_v1310.to_json(j);
  }
  if (scptm_params_r13_present) {
    j.write_fieldname("scptm-Parameters-r13");
    scptm_params_r13.to_json(j);
  }
  if (ce_params_r13_present) {
    j.write_fieldname("ce-Parameters-r13");
    ce_params_r13.to_json(j);
  }
  j.write_fieldname("interRAT-ParametersWLAN-r13");
  inter_rat_params_wlan_r13.to_json(j);
  if (laa_params_r13_present) {
    j.write_fieldname("laa-Parameters-r13");
    laa_params_r13.to_json(j);
  }
  if (lwa_params_r13_present) {
    j.write_fieldname("lwa-Parameters-r13");
    lwa_params_r13.to_json(j);
  }
  j.write_fieldname("wlan-IW-Parameters-v1310");
  wlan_iw_params_v1310.to_json(j);
  j.write_fieldname("lwip-Parameters-r13");
  lwip_params_r13.to_json(j);
  if (fdd_add_ue_eutra_cap_v1310_present) {
    j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v1310");
    fdd_add_ue_eutra_cap_v1310.to_json(j);
  }
  if (tdd_add_ue_eutra_cap_v1310_present) {
    j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v1310");
    tdd_add_ue_eutra_cap_v1310.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* ue_eutra_cap_v1310_ies_s::ue_category_dl_v1310_opts::to_string() const
{
  static const char* options[] = {"n17", "m1"};
  return convert_enum_idx(options, 2, value, "ue_eutra_cap_v1310_ies_s::ue_category_dl_v1310_e_");
}
uint8_t ue_eutra_cap_v1310_ies_s::ue_category_dl_v1310_opts::to_number() const
{
  static const uint8_t options[] = {17, 1};
  return map_enum_number(options, 2, value, "ue_eutra_cap_v1310_ies_s::ue_category_dl_v1310_e_");
}

const char* ue_eutra_cap_v1310_ies_s::ue_category_ul_v1310_opts::to_string() const
{
  static const char* options[] = {"n14", "m1"};
  return convert_enum_idx(options, 2, value, "ue_eutra_cap_v1310_ies_s::ue_category_ul_v1310_e_");
}
uint8_t ue_eutra_cap_v1310_ies_s::ue_category_ul_v1310_opts::to_number() const
{
  static const uint8_t options[] = {14, 1};
  return map_enum_number(options, 2, value, "ue_eutra_cap_v1310_ies_s::ue_category_ul_v1310_e_");
}

// UE-EUTRA-Capability-v1280-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1280_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_layer_params_v1280_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (phy_layer_params_v1280_present) {
    HANDLE_CODE(phy_layer_params_v1280.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1280_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_layer_params_v1280_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (phy_layer_params_v1280_present) {
    HANDLE_CODE(phy_layer_params_v1280.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1280_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_v1280_present) {
    j.write_fieldname("phyLayerParameters-v1280");
    phy_layer_params_v1280.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MeasParameters-v1250 ::= SEQUENCE
SRSASN_CODE meas_params_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(timer_t312_r12_present, 1));
  HANDLE_CODE(bref.pack(alt_time_to_trigger_r12_present, 1));
  HANDLE_CODE(bref.pack(inc_mon_eutra_r12_present, 1));
  HANDLE_CODE(bref.pack(inc_mon_utra_r12_present, 1));
  HANDLE_CODE(bref.pack(extended_max_meas_id_r12_present, 1));
  HANDLE_CODE(bref.pack(extended_rsrq_lower_range_r12_present, 1));
  HANDLE_CODE(bref.pack(rsrq_on_all_symbols_r12_present, 1));
  HANDLE_CODE(bref.pack(crs_discovery_signals_meas_r12_present, 1));
  HANDLE_CODE(bref.pack(csi_rs_discovery_signals_meas_r12_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_params_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(timer_t312_r12_present, 1));
  HANDLE_CODE(bref.unpack(alt_time_to_trigger_r12_present, 1));
  HANDLE_CODE(bref.unpack(inc_mon_eutra_r12_present, 1));
  HANDLE_CODE(bref.unpack(inc_mon_utra_r12_present, 1));
  HANDLE_CODE(bref.unpack(extended_max_meas_id_r12_present, 1));
  HANDLE_CODE(bref.unpack(extended_rsrq_lower_range_r12_present, 1));
  HANDLE_CODE(bref.unpack(rsrq_on_all_symbols_r12_present, 1));
  HANDLE_CODE(bref.unpack(crs_discovery_signals_meas_r12_present, 1));
  HANDLE_CODE(bref.unpack(csi_rs_discovery_signals_meas_r12_present, 1));

  return SRSASN_SUCCESS;
}
void meas_params_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (timer_t312_r12_present) {
    j.write_str("timerT312-r12", "supported");
  }
  if (alt_time_to_trigger_r12_present) {
    j.write_str("alternativeTimeToTrigger-r12", "supported");
  }
  if (inc_mon_eutra_r12_present) {
    j.write_str("incMonEUTRA-r12", "supported");
  }
  if (inc_mon_utra_r12_present) {
    j.write_str("incMonUTRA-r12", "supported");
  }
  if (extended_max_meas_id_r12_present) {
    j.write_str("extendedMaxMeasId-r12", "supported");
  }
  if (extended_rsrq_lower_range_r12_present) {
    j.write_str("extendedRSRQ-LowerRange-r12", "supported");
  }
  if (rsrq_on_all_symbols_r12_present) {
    j.write_str("rsrq-OnAllSymbols-r12", "supported");
  }
  if (crs_discovery_signals_meas_r12_present) {
    j.write_str("crs-DiscoverySignalsMeas-r12", "supported");
  }
  if (csi_rs_discovery_signals_meas_r12_present) {
    j.write_str("csi-RS-DiscoverySignalsMeas-r12", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1270-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1270_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v1270_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v1270_present) {
    HANDLE_CODE(rf_params_v1270.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1270_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v1270_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v1270_present) {
    HANDLE_CODE(rf_params_v1270.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1270_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v1270_present) {
    j.write_fieldname("rf-Parameters-v1270");
    rf_params_v1270.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// DC-Parameters-r12 ::= SEQUENCE
SRSASN_CODE dc_params_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(drb_type_split_r12_present, 1));
  HANDLE_CODE(bref.pack(drb_type_scg_r12_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dc_params_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(drb_type_split_r12_present, 1));
  HANDLE_CODE(bref.unpack(drb_type_scg_r12_present, 1));

  return SRSASN_SUCCESS;
}
void dc_params_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drb_type_split_r12_present) {
    j.write_str("drb-TypeSplit-r12", "supported");
  }
  if (drb_type_scg_r12_present) {
    j.write_str("drb-TypeSCG-r12", "supported");
  }
  j.end_obj();
}

// MAC-Parameters-r12 ::= SEQUENCE
SRSASN_CODE mac_params_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(lc_ch_sr_prohibit_timer_r12_present, 1));
  HANDLE_CODE(bref.pack(long_drx_cmd_r12_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_params_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(lc_ch_sr_prohibit_timer_r12_present, 1));
  HANDLE_CODE(bref.unpack(long_drx_cmd_r12_present, 1));

  return SRSASN_SUCCESS;
}
void mac_params_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (lc_ch_sr_prohibit_timer_r12_present) {
    j.write_str("logicalChannelSR-ProhibitTimer-r12", "supported");
  }
  if (long_drx_cmd_r12_present) {
    j.write_str("longDRX-Command-r12", "supported");
  }
  j.end_obj();
}

// MBMS-Parameters-v1250 ::= SEQUENCE
SRSASN_CODE mbms_params_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mbms_async_dc_r12_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_params_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mbms_async_dc_r12_present, 1));

  return SRSASN_SUCCESS;
}
void mbms_params_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbms_async_dc_r12_present) {
    j.write_str("mbms-AsyncDC-r12", "supported");
  }
  j.end_obj();
}

// RLC-Parameters-r12 ::= SEQUENCE
SRSASN_CODE rlc_params_r12_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_params_r12_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void rlc_params_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("extended-RLC-LI-Field-r12", "supported");
  j.end_obj();
}

// UE-BasedNetwPerfMeasParameters-v1250 ::= SEQUENCE
SRSASN_CODE ue_based_netw_perf_meas_params_v1250_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_based_netw_perf_meas_params_v1250_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void ue_based_netw_perf_meas_params_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("loggedMBSFNMeasurements-r12", "supported");
  j.end_obj();
}

// UE-EUTRA-Capability-v1260-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1260_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_category_dl_v1260_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_category_dl_v1260_present) {
    HANDLE_CODE(pack_integer(bref, ue_category_dl_v1260, (uint8_t)15u, (uint8_t)16u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1260_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_category_dl_v1260_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_category_dl_v1260_present) {
    HANDLE_CODE(unpack_integer(ue_category_dl_v1260, bref, (uint8_t)15u, (uint8_t)16u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1260_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_category_dl_v1260_present) {
    j.write_int("ue-CategoryDL-v1260", ue_category_dl_v1260);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v1250 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_layer_params_v1250_present, 1));
  HANDLE_CODE(bref.pack(meas_params_v1250_present, 1));

  if (phy_layer_params_v1250_present) {
    HANDLE_CODE(phy_layer_params_v1250.pack(bref));
  }
  if (meas_params_v1250_present) {
    HANDLE_CODE(meas_params_v1250.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_layer_params_v1250_present, 1));
  HANDLE_CODE(bref.unpack(meas_params_v1250_present, 1));

  if (phy_layer_params_v1250_present) {
    HANDLE_CODE(phy_layer_params_v1250.unpack(bref));
  }
  if (meas_params_v1250_present) {
    HANDLE_CODE(meas_params_v1250.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_v1250_present) {
    j.write_fieldname("phyLayerParameters-v1250");
    phy_layer_params_v1250.to_json(j);
  }
  if (meas_params_v1250_present) {
    j.write_fieldname("measParameters-v1250");
    meas_params_v1250.to_json(j);
  }
  j.end_obj();
}

// WLAN-IW-Parameters-r12 ::= SEQUENCE
SRSASN_CODE wlan_iw_params_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(wlan_iw_ran_rules_r12_present, 1));
  HANDLE_CODE(bref.pack(wlan_iw_andsf_policies_r12_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_iw_params_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(wlan_iw_ran_rules_r12_present, 1));
  HANDLE_CODE(bref.unpack(wlan_iw_andsf_policies_r12_present, 1));

  return SRSASN_SUCCESS;
}
void wlan_iw_params_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (wlan_iw_ran_rules_r12_present) {
    j.write_str("wlan-IW-RAN-Rules-r12", "supported");
  }
  if (wlan_iw_andsf_policies_r12_present) {
    j.write_str("wlan-IW-ANDSF-Policies-r12", "supported");
  }
  j.end_obj();
}

// MBMS-Parameters-r11 ::= SEQUENCE
SRSASN_CODE mbms_params_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mbms_scell_r11_present, 1));
  HANDLE_CODE(bref.pack(mbms_non_serving_cell_r11_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_params_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mbms_scell_r11_present, 1));
  HANDLE_CODE(bref.unpack(mbms_non_serving_cell_r11_present, 1));

  return SRSASN_SUCCESS;
}
void mbms_params_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbms_scell_r11_present) {
    j.write_str("mbms-SCell-r11", "supported");
  }
  if (mbms_non_serving_cell_r11_present) {
    j.write_str("mbms-NonServingCell-r11", "supported");
  }
  j.end_obj();
}

// MeasParameters-v11a0 ::= SEQUENCE
SRSASN_CODE meas_params_v11a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(benefits_from_interruption_r11_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_params_v11a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(benefits_from_interruption_r11_present, 1));

  return SRSASN_SUCCESS;
}
void meas_params_v11a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (benefits_from_interruption_r11_present) {
    j.write_str("benefitsFromInterruption-r11", "true");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1250-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1250_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_layer_params_v1250_present, 1));
  HANDLE_CODE(bref.pack(rf_params_v1250_present, 1));
  HANDLE_CODE(bref.pack(rlc_params_r12_present, 1));
  HANDLE_CODE(bref.pack(ue_based_netw_perf_meas_params_v1250_present, 1));
  HANDLE_CODE(bref.pack(ue_category_dl_r12_present, 1));
  HANDLE_CODE(bref.pack(ue_category_ul_r12_present, 1));
  HANDLE_CODE(bref.pack(wlan_iw_params_r12_present, 1));
  HANDLE_CODE(bref.pack(meas_params_v1250_present, 1));
  HANDLE_CODE(bref.pack(dc_params_r12_present, 1));
  HANDLE_CODE(bref.pack(mbms_params_v1250_present, 1));
  HANDLE_CODE(bref.pack(mac_params_r12_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_eutra_cap_v1250_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_eutra_cap_v1250_present, 1));
  HANDLE_CODE(bref.pack(sl_params_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (phy_layer_params_v1250_present) {
    HANDLE_CODE(phy_layer_params_v1250.pack(bref));
  }
  if (rf_params_v1250_present) {
    HANDLE_CODE(rf_params_v1250.pack(bref));
  }
  if (rlc_params_r12_present) {
    HANDLE_CODE(rlc_params_r12.pack(bref));
  }
  if (ue_based_netw_perf_meas_params_v1250_present) {
    HANDLE_CODE(ue_based_netw_perf_meas_params_v1250.pack(bref));
  }
  if (ue_category_dl_r12_present) {
    HANDLE_CODE(pack_integer(bref, ue_category_dl_r12, (uint8_t)0u, (uint8_t)14u));
  }
  if (ue_category_ul_r12_present) {
    HANDLE_CODE(pack_integer(bref, ue_category_ul_r12, (uint8_t)0u, (uint8_t)13u));
  }
  if (wlan_iw_params_r12_present) {
    HANDLE_CODE(wlan_iw_params_r12.pack(bref));
  }
  if (meas_params_v1250_present) {
    HANDLE_CODE(meas_params_v1250.pack(bref));
  }
  if (dc_params_r12_present) {
    HANDLE_CODE(dc_params_r12.pack(bref));
  }
  if (mbms_params_v1250_present) {
    HANDLE_CODE(mbms_params_v1250.pack(bref));
  }
  if (mac_params_r12_present) {
    HANDLE_CODE(mac_params_r12.pack(bref));
  }
  if (fdd_add_ue_eutra_cap_v1250_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1250.pack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1250_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1250.pack(bref));
  }
  if (sl_params_r12_present) {
    HANDLE_CODE(sl_params_r12.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1250_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_layer_params_v1250_present, 1));
  HANDLE_CODE(bref.unpack(rf_params_v1250_present, 1));
  HANDLE_CODE(bref.unpack(rlc_params_r12_present, 1));
  HANDLE_CODE(bref.unpack(ue_based_netw_perf_meas_params_v1250_present, 1));
  HANDLE_CODE(bref.unpack(ue_category_dl_r12_present, 1));
  HANDLE_CODE(bref.unpack(ue_category_ul_r12_present, 1));
  HANDLE_CODE(bref.unpack(wlan_iw_params_r12_present, 1));
  HANDLE_CODE(bref.unpack(meas_params_v1250_present, 1));
  HANDLE_CODE(bref.unpack(dc_params_r12_present, 1));
  HANDLE_CODE(bref.unpack(mbms_params_v1250_present, 1));
  HANDLE_CODE(bref.unpack(mac_params_r12_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_eutra_cap_v1250_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_eutra_cap_v1250_present, 1));
  HANDLE_CODE(bref.unpack(sl_params_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (phy_layer_params_v1250_present) {
    HANDLE_CODE(phy_layer_params_v1250.unpack(bref));
  }
  if (rf_params_v1250_present) {
    HANDLE_CODE(rf_params_v1250.unpack(bref));
  }
  if (rlc_params_r12_present) {
    HANDLE_CODE(rlc_params_r12.unpack(bref));
  }
  if (ue_based_netw_perf_meas_params_v1250_present) {
    HANDLE_CODE(ue_based_netw_perf_meas_params_v1250.unpack(bref));
  }
  if (ue_category_dl_r12_present) {
    HANDLE_CODE(unpack_integer(ue_category_dl_r12, bref, (uint8_t)0u, (uint8_t)14u));
  }
  if (ue_category_ul_r12_present) {
    HANDLE_CODE(unpack_integer(ue_category_ul_r12, bref, (uint8_t)0u, (uint8_t)13u));
  }
  if (wlan_iw_params_r12_present) {
    HANDLE_CODE(wlan_iw_params_r12.unpack(bref));
  }
  if (meas_params_v1250_present) {
    HANDLE_CODE(meas_params_v1250.unpack(bref));
  }
  if (dc_params_r12_present) {
    HANDLE_CODE(dc_params_r12.unpack(bref));
  }
  if (mbms_params_v1250_present) {
    HANDLE_CODE(mbms_params_v1250.unpack(bref));
  }
  if (mac_params_r12_present) {
    HANDLE_CODE(mac_params_r12.unpack(bref));
  }
  if (fdd_add_ue_eutra_cap_v1250_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1250.unpack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1250_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1250.unpack(bref));
  }
  if (sl_params_r12_present) {
    HANDLE_CODE(sl_params_r12.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1250_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_v1250_present) {
    j.write_fieldname("phyLayerParameters-v1250");
    phy_layer_params_v1250.to_json(j);
  }
  if (rf_params_v1250_present) {
    j.write_fieldname("rf-Parameters-v1250");
    rf_params_v1250.to_json(j);
  }
  if (rlc_params_r12_present) {
    j.write_fieldname("rlc-Parameters-r12");
    rlc_params_r12.to_json(j);
  }
  if (ue_based_netw_perf_meas_params_v1250_present) {
    j.write_fieldname("ue-BasedNetwPerfMeasParameters-v1250");
    ue_based_netw_perf_meas_params_v1250.to_json(j);
  }
  if (ue_category_dl_r12_present) {
    j.write_int("ue-CategoryDL-r12", ue_category_dl_r12);
  }
  if (ue_category_ul_r12_present) {
    j.write_int("ue-CategoryUL-r12", ue_category_ul_r12);
  }
  if (wlan_iw_params_r12_present) {
    j.write_fieldname("wlan-IW-Parameters-r12");
    wlan_iw_params_r12.to_json(j);
  }
  if (meas_params_v1250_present) {
    j.write_fieldname("measParameters-v1250");
    meas_params_v1250.to_json(j);
  }
  if (dc_params_r12_present) {
    j.write_fieldname("dc-Parameters-r12");
    dc_params_r12.to_json(j);
  }
  if (mbms_params_v1250_present) {
    j.write_fieldname("mbms-Parameters-v1250");
    mbms_params_v1250.to_json(j);
  }
  if (mac_params_r12_present) {
    j.write_fieldname("mac-Parameters-r12");
    mac_params_r12.to_json(j);
  }
  if (fdd_add_ue_eutra_cap_v1250_present) {
    j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v1250");
    fdd_add_ue_eutra_cap_v1250.to_json(j);
  }
  if (tdd_add_ue_eutra_cap_v1250_present) {
    j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v1250");
    tdd_add_ue_eutra_cap_v1250.to_json(j);
  }
  if (sl_params_r12_present) {
    j.write_fieldname("sl-Parameters-r12");
    sl_params_r12.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v11a0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v11a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_category_v11a0_present, 1));
  HANDLE_CODE(bref.pack(meas_params_v11a0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_category_v11a0_present) {
    HANDLE_CODE(pack_integer(bref, ue_category_v11a0, (uint8_t)11u, (uint8_t)12u));
  }
  if (meas_params_v11a0_present) {
    HANDLE_CODE(meas_params_v11a0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v11a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_category_v11a0_present, 1));
  HANDLE_CODE(bref.unpack(meas_params_v11a0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_category_v11a0_present) {
    HANDLE_CODE(unpack_integer(ue_category_v11a0, bref, (uint8_t)11u, (uint8_t)12u));
  }
  if (meas_params_v11a0_present) {
    HANDLE_CODE(meas_params_v11a0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v11a0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_category_v11a0_present) {
    j.write_int("ue-Category-v11a0", ue_category_v11a0);
  }
  if (meas_params_v11a0_present) {
    j.write_fieldname("measParameters-v11a0");
    meas_params_v11a0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v1180 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1180_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(mbms_params_r11.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1180_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(mbms_params_r11.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v1180_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mbms-Parameters-r11");
  mbms_params_r11.to_json(j);
  j.end_obj();
}

// MeasParameters-v1130 ::= SEQUENCE
SRSASN_CODE meas_params_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrq_meas_wideband_r11_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_params_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrq_meas_wideband_r11_present, 1));

  return SRSASN_SUCCESS;
}
void meas_params_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rsrq_meas_wideband_r11_present) {
    j.write_str("rsrqMeasWideband-r11", "supported");
  }
  j.end_obj();
}

// Other-Parameters-r11 ::= SEQUENCE
SRSASN_CODE other_params_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(in_dev_coex_ind_r11_present, 1));
  HANDLE_CODE(bref.pack(pwr_pref_ind_r11_present, 1));
  HANDLE_CODE(bref.pack(ue_rx_tx_time_diff_meass_r11_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE other_params_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(in_dev_coex_ind_r11_present, 1));
  HANDLE_CODE(bref.unpack(pwr_pref_ind_r11_present, 1));
  HANDLE_CODE(bref.unpack(ue_rx_tx_time_diff_meass_r11_present, 1));

  return SRSASN_SUCCESS;
}
void other_params_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (in_dev_coex_ind_r11_present) {
    j.write_str("inDeviceCoexInd-r11", "supported");
  }
  if (pwr_pref_ind_r11_present) {
    j.write_str("powerPrefInd-r11", "supported");
  }
  if (ue_rx_tx_time_diff_meass_r11_present) {
    j.write_str("ue-Rx-TxTimeDiffMeasurements-r11", "supported");
  }
  j.end_obj();
}

// PhyLayerParameters-v1130 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(crs_interf_handl_r11_present, 1));
  HANDLE_CODE(bref.pack(epdcch_r11_present, 1));
  HANDLE_CODE(bref.pack(multi_ack_csi_report_r11_present, 1));
  HANDLE_CODE(bref.pack(ss_cch_interf_handl_r11_present, 1));
  HANDLE_CODE(bref.pack(tdd_special_sf_r11_present, 1));
  HANDLE_CODE(bref.pack(tx_div_pucch1b_ch_select_r11_present, 1));
  HANDLE_CODE(bref.pack(ul_co_mp_r11_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(crs_interf_handl_r11_present, 1));
  HANDLE_CODE(bref.unpack(epdcch_r11_present, 1));
  HANDLE_CODE(bref.unpack(multi_ack_csi_report_r11_present, 1));
  HANDLE_CODE(bref.unpack(ss_cch_interf_handl_r11_present, 1));
  HANDLE_CODE(bref.unpack(tdd_special_sf_r11_present, 1));
  HANDLE_CODE(bref.unpack(tx_div_pucch1b_ch_select_r11_present, 1));
  HANDLE_CODE(bref.unpack(ul_co_mp_r11_present, 1));

  return SRSASN_SUCCESS;
}
void phy_layer_params_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (crs_interf_handl_r11_present) {
    j.write_str("crs-InterfHandl-r11", "supported");
  }
  if (epdcch_r11_present) {
    j.write_str("ePDCCH-r11", "supported");
  }
  if (multi_ack_csi_report_r11_present) {
    j.write_str("multiACK-CSI-Reporting-r11", "supported");
  }
  if (ss_cch_interf_handl_r11_present) {
    j.write_str("ss-CCH-InterfHandl-r11", "supported");
  }
  if (tdd_special_sf_r11_present) {
    j.write_str("tdd-SpecialSubframe-r11", "supported");
  }
  if (tx_div_pucch1b_ch_select_r11_present) {
    j.write_str("txDiv-PUCCH1b-ChSelect-r11", "supported");
  }
  if (ul_co_mp_r11_present) {
    j.write_str("ul-CoMP-r11", "supported");
  }
  j.end_obj();
}

// PhyLayerParameters-v1170 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v1170_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_band_tdd_ca_with_different_cfg_r11_present, 1));

  if (inter_band_tdd_ca_with_different_cfg_r11_present) {
    HANDLE_CODE(inter_band_tdd_ca_with_different_cfg_r11.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v1170_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(inter_band_tdd_ca_with_different_cfg_r11_present, 1));

  if (inter_band_tdd_ca_with_different_cfg_r11_present) {
    HANDLE_CODE(inter_band_tdd_ca_with_different_cfg_r11.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void phy_layer_params_v1170_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (inter_band_tdd_ca_with_different_cfg_r11_present) {
    j.write_str("interBandTDD-CA-WithDifferentConfig-r11", inter_band_tdd_ca_with_different_cfg_r11.to_string());
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1180-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1180_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v1180_present, 1));
  HANDLE_CODE(bref.pack(mbms_params_r11_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_eutra_cap_v1180_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_eutra_cap_v1180_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v1180_present) {
    HANDLE_CODE(rf_params_v1180.pack(bref));
  }
  if (mbms_params_r11_present) {
    HANDLE_CODE(mbms_params_r11.pack(bref));
  }
  if (fdd_add_ue_eutra_cap_v1180_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1180.pack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1180_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1180.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1180_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v1180_present, 1));
  HANDLE_CODE(bref.unpack(mbms_params_r11_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_eutra_cap_v1180_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_eutra_cap_v1180_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v1180_present) {
    HANDLE_CODE(rf_params_v1180.unpack(bref));
  }
  if (mbms_params_r11_present) {
    HANDLE_CODE(mbms_params_r11.unpack(bref));
  }
  if (fdd_add_ue_eutra_cap_v1180_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1180.unpack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1180_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1180.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1180_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v1180_present) {
    j.write_fieldname("rf-Parameters-v1180");
    rf_params_v1180.to_json(j);
  }
  if (mbms_params_r11_present) {
    j.write_fieldname("mbms-Parameters-r11");
    mbms_params_r11.to_json(j);
  }
  if (fdd_add_ue_eutra_cap_v1180_present) {
    j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v1180");
    fdd_add_ue_eutra_cap_v1180.to_json(j);
  }
  if (tdd_add_ue_eutra_cap_v1180_present) {
    j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v1180");
    tdd_add_ue_eutra_cap_v1180.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// IRAT-ParametersCDMA2000-v1130 ::= SEQUENCE
SRSASN_CODE irat_params_cdma2000_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cdma2000_nw_sharing_r11_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_cdma2000_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cdma2000_nw_sharing_r11_present, 1));

  return SRSASN_SUCCESS;
}
void irat_params_cdma2000_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cdma2000_nw_sharing_r11_present) {
    j.write_str("cdma2000-NW-Sharing-r11", "supported");
  }
  j.end_obj();
}

// PDCP-Parameters-v1130 ::= SEQUENCE
SRSASN_CODE pdcp_params_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdcp_sn_ext_r11_present, 1));
  HANDLE_CODE(bref.pack(support_rohc_context_continue_r11_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_params_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdcp_sn_ext_r11_present, 1));
  HANDLE_CODE(bref.unpack(support_rohc_context_continue_r11_present, 1));

  return SRSASN_SUCCESS;
}
void pdcp_params_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcp_sn_ext_r11_present) {
    j.write_str("pdcp-SN-Extension-r11", "supported");
  }
  if (support_rohc_context_continue_r11_present) {
    j.write_str("supportRohcContextContinue-r11", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1170-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1170_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_layer_params_v1170_present, 1));
  HANDLE_CODE(bref.pack(ue_category_v1170_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (phy_layer_params_v1170_present) {
    HANDLE_CODE(phy_layer_params_v1170.pack(bref));
  }
  if (ue_category_v1170_present) {
    HANDLE_CODE(pack_integer(bref, ue_category_v1170, (uint8_t)9u, (uint8_t)10u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1170_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_layer_params_v1170_present, 1));
  HANDLE_CODE(bref.unpack(ue_category_v1170_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (phy_layer_params_v1170_present) {
    HANDLE_CODE(phy_layer_params_v1170.unpack(bref));
  }
  if (ue_category_v1170_present) {
    HANDLE_CODE(unpack_integer(ue_category_v1170, bref, (uint8_t)9u, (uint8_t)10u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1170_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_v1170_present) {
    j.write_fieldname("phyLayerParameters-v1170");
    phy_layer_params_v1170.to_json(j);
  }
  if (ue_category_v1170_present) {
    j.write_int("ue-Category-v1170", ue_category_v1170);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v1130 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1130_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(phy_layer_params_v1130_present, 1));
  HANDLE_CODE(bref.pack(meas_params_v1130_present, 1));
  HANDLE_CODE(bref.pack(other_params_r11_present, 1));

  if (phy_layer_params_v1130_present) {
    HANDLE_CODE(phy_layer_params_v1130.pack(bref));
  }
  if (meas_params_v1130_present) {
    HANDLE_CODE(meas_params_v1130.pack(bref));
  }
  if (other_params_r11_present) {
    HANDLE_CODE(other_params_r11.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1130_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(phy_layer_params_v1130_present, 1));
  HANDLE_CODE(bref.unpack(meas_params_v1130_present, 1));
  HANDLE_CODE(bref.unpack(other_params_r11_present, 1));

  if (phy_layer_params_v1130_present) {
    HANDLE_CODE(phy_layer_params_v1130.unpack(bref));
  }
  if (meas_params_v1130_present) {
    HANDLE_CODE(meas_params_v1130.unpack(bref));
  }
  if (other_params_r11_present) {
    HANDLE_CODE(other_params_r11.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_v1130_present) {
    j.write_fieldname("phyLayerParameters-v1130");
    phy_layer_params_v1130.to_json(j);
  }
  if (meas_params_v1130_present) {
    j.write_fieldname("measParameters-v1130");
    meas_params_v1130.to_json(j);
  }
  if (other_params_r11_present) {
    j.write_fieldname("otherParameters-r11");
    other_params_r11.to_json(j);
  }
  j.end_obj();
}

// IRAT-ParametersCDMA2000-1XRTT-v1020 ::= SEQUENCE
SRSASN_CODE irat_params_cdma2000_minus1_xrtt_v1020_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_cdma2000_minus1_xrtt_v1020_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void irat_params_cdma2000_minus1_xrtt_v1020_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e-CSFB-dual-1XRTT-r10", "supported");
  j.end_obj();
}

// IRAT-ParametersUTRA-TDD-v1020 ::= SEQUENCE
SRSASN_CODE irat_params_utra_tdd_v1020_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_utra_tdd_v1020_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void irat_params_utra_tdd_v1020_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e-RedirectionUTRA-TDD-r10", "supported");
  j.end_obj();
}

// OTDOA-PositioningCapabilities-r10 ::= SEQUENCE
SRSASN_CODE otdoa_positioning_cap_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_freq_rstd_meas_r10_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE otdoa_positioning_cap_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(inter_freq_rstd_meas_r10_present, 1));

  return SRSASN_SUCCESS;
}
void otdoa_positioning_cap_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("otdoa-UE-Assisted-r10", "supported");
  if (inter_freq_rstd_meas_r10_present) {
    j.write_str("interFreqRSTD-Measurement-r10", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1130-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1130_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_layer_params_v1130_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_eutra_cap_v1130_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_eutra_cap_v1130_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pdcp_params_v1130.pack(bref));
  if (phy_layer_params_v1130_present) {
    HANDLE_CODE(phy_layer_params_v1130.pack(bref));
  }
  HANDLE_CODE(rf_params_v1130.pack(bref));
  HANDLE_CODE(meas_params_v1130.pack(bref));
  HANDLE_CODE(inter_rat_params_cdma2000_v1130.pack(bref));
  HANDLE_CODE(other_params_r11.pack(bref));
  if (fdd_add_ue_eutra_cap_v1130_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1130.pack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1130_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1130.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1130_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_layer_params_v1130_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_eutra_cap_v1130_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_eutra_cap_v1130_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(pdcp_params_v1130.unpack(bref));
  if (phy_layer_params_v1130_present) {
    HANDLE_CODE(phy_layer_params_v1130.unpack(bref));
  }
  HANDLE_CODE(rf_params_v1130.unpack(bref));
  HANDLE_CODE(meas_params_v1130.unpack(bref));
  HANDLE_CODE(inter_rat_params_cdma2000_v1130.unpack(bref));
  HANDLE_CODE(other_params_r11.unpack(bref));
  if (fdd_add_ue_eutra_cap_v1130_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1130.unpack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1130_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1130.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1130_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("pdcp-Parameters-v1130");
  pdcp_params_v1130.to_json(j);
  if (phy_layer_params_v1130_present) {
    j.write_fieldname("phyLayerParameters-v1130");
    phy_layer_params_v1130.to_json(j);
  }
  j.write_fieldname("rf-Parameters-v1130");
  rf_params_v1130.to_json(j);
  j.write_fieldname("measParameters-v1130");
  meas_params_v1130.to_json(j);
  j.write_fieldname("interRAT-ParametersCDMA2000-v1130");
  inter_rat_params_cdma2000_v1130.to_json(j);
  j.write_fieldname("otherParameters-r11");
  other_params_r11.to_json(j);
  if (fdd_add_ue_eutra_cap_v1130_present) {
    j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v1130");
    fdd_add_ue_eutra_cap_v1130.to_json(j);
  }
  if (tdd_add_ue_eutra_cap_v1130_present) {
    j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v1130");
    tdd_add_ue_eutra_cap_v1130.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1090-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1090_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v1090_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v1090_present) {
    HANDLE_CODE(rf_params_v1090.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1090_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v1090_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v1090_present) {
    HANDLE_CODE(rf_params_v1090.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1090_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v1090_present) {
    j.write_fieldname("rf-Parameters-v1090");
    rf_params_v1090.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v1060 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1060_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(phy_layer_params_v1060_present, 1));
  HANDLE_CODE(bref.pack(feature_group_ind_rel10_v1060_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params_cdma2000_v1060_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params_utra_tdd_v1060_present, 1));

  if (phy_layer_params_v1060_present) {
    HANDLE_CODE(phy_layer_params_v1060.pack(bref));
  }
  if (feature_group_ind_rel10_v1060_present) {
    HANDLE_CODE(feature_group_ind_rel10_v1060.pack(bref));
  }
  if (inter_rat_params_cdma2000_v1060_present) {
    HANDLE_CODE(inter_rat_params_cdma2000_v1060.pack(bref));
  }
  if (inter_rat_params_utra_tdd_v1060_present) {
    HANDLE_CODE(inter_rat_params_utra_tdd_v1060.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= otdoa_positioning_cap_r10.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(otdoa_positioning_cap_r10.is_present(), 1));
      if (otdoa_positioning_cap_r10.is_present()) {
        HANDLE_CODE(otdoa_positioning_cap_r10->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1060_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(phy_layer_params_v1060_present, 1));
  HANDLE_CODE(bref.unpack(feature_group_ind_rel10_v1060_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params_cdma2000_v1060_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params_utra_tdd_v1060_present, 1));

  if (phy_layer_params_v1060_present) {
    HANDLE_CODE(phy_layer_params_v1060.unpack(bref));
  }
  if (feature_group_ind_rel10_v1060_present) {
    HANDLE_CODE(feature_group_ind_rel10_v1060.unpack(bref));
  }
  if (inter_rat_params_cdma2000_v1060_present) {
    HANDLE_CODE(inter_rat_params_cdma2000_v1060.unpack(bref));
  }
  if (inter_rat_params_utra_tdd_v1060_present) {
    HANDLE_CODE(inter_rat_params_utra_tdd_v1060.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool otdoa_positioning_cap_r10_present;
      HANDLE_CODE(bref.unpack(otdoa_positioning_cap_r10_present, 1));
      otdoa_positioning_cap_r10.set_present(otdoa_positioning_cap_r10_present);
      if (otdoa_positioning_cap_r10.is_present()) {
        HANDLE_CODE(otdoa_positioning_cap_r10->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v1060_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_v1060_present) {
    j.write_fieldname("phyLayerParameters-v1060");
    phy_layer_params_v1060.to_json(j);
  }
  if (feature_group_ind_rel10_v1060_present) {
    j.write_str("featureGroupIndRel10-v1060", feature_group_ind_rel10_v1060.to_string());
  }
  if (inter_rat_params_cdma2000_v1060_present) {
    j.write_fieldname("interRAT-ParametersCDMA2000-v1060");
    inter_rat_params_cdma2000_v1060.to_json(j);
  }
  if (inter_rat_params_utra_tdd_v1060_present) {
    j.write_fieldname("interRAT-ParametersUTRA-TDD-v1060");
    inter_rat_params_utra_tdd_v1060.to_json(j);
  }
  if (ext) {
    if (otdoa_positioning_cap_r10.is_present()) {
      j.write_fieldname("otdoa-PositioningCapabilities-r10");
      otdoa_positioning_cap_r10->to_json(j);
    }
  }
  j.end_obj();
}

// UE-BasedNetwPerfMeasParameters-r10 ::= SEQUENCE
SRSASN_CODE ue_based_netw_perf_meas_params_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(logged_meass_idle_r10_present, 1));
  HANDLE_CODE(bref.pack(standalone_gnss_location_r10_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_based_netw_perf_meas_params_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(logged_meass_idle_r10_present, 1));
  HANDLE_CODE(bref.unpack(standalone_gnss_location_r10_present, 1));

  return SRSASN_SUCCESS;
}
void ue_based_netw_perf_meas_params_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (logged_meass_idle_r10_present) {
    j.write_str("loggedMeasurementsIdle-r10", "supported");
  }
  if (standalone_gnss_location_r10_present) {
    j.write_str("standaloneGNSS-Location-r10", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1060-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1060_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fdd_add_ue_eutra_cap_v1060_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_eutra_cap_v1060_present, 1));
  HANDLE_CODE(bref.pack(rf_params_v1060_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fdd_add_ue_eutra_cap_v1060_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1060.pack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1060_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1060.pack(bref));
  }
  if (rf_params_v1060_present) {
    HANDLE_CODE(rf_params_v1060.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1060_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fdd_add_ue_eutra_cap_v1060_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_eutra_cap_v1060_present, 1));
  HANDLE_CODE(bref.unpack(rf_params_v1060_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fdd_add_ue_eutra_cap_v1060_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1060.unpack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1060_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1060.unpack(bref));
  }
  if (rf_params_v1060_present) {
    HANDLE_CODE(rf_params_v1060.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1060_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fdd_add_ue_eutra_cap_v1060_present) {
    j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v1060");
    fdd_add_ue_eutra_cap_v1060.to_json(j);
  }
  if (tdd_add_ue_eutra_cap_v1060_present) {
    j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v1060");
    tdd_add_ue_eutra_cap_v1060.to_json(j);
  }
  if (rf_params_v1060_present) {
    j.write_fieldname("rf-Parameters-v1060");
    rf_params_v1060.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1020-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1020_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_category_v1020_present, 1));
  HANDLE_CODE(bref.pack(phy_layer_params_v1020_present, 1));
  HANDLE_CODE(bref.pack(rf_params_v1020_present, 1));
  HANDLE_CODE(bref.pack(meas_params_v1020_present, 1));
  HANDLE_CODE(bref.pack(feature_group_ind_rel10_r10_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params_cdma2000_v1020_present, 1));
  HANDLE_CODE(bref.pack(ue_based_netw_perf_meas_params_r10_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params_utra_tdd_v1020_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_category_v1020_present) {
    HANDLE_CODE(pack_integer(bref, ue_category_v1020, (uint8_t)6u, (uint8_t)8u));
  }
  if (phy_layer_params_v1020_present) {
    HANDLE_CODE(phy_layer_params_v1020.pack(bref));
  }
  if (rf_params_v1020_present) {
    HANDLE_CODE(rf_params_v1020.pack(bref));
  }
  if (meas_params_v1020_present) {
    HANDLE_CODE(meas_params_v1020.pack(bref));
  }
  if (feature_group_ind_rel10_r10_present) {
    HANDLE_CODE(feature_group_ind_rel10_r10.pack(bref));
  }
  if (inter_rat_params_cdma2000_v1020_present) {
    HANDLE_CODE(inter_rat_params_cdma2000_v1020.pack(bref));
  }
  if (ue_based_netw_perf_meas_params_r10_present) {
    HANDLE_CODE(ue_based_netw_perf_meas_params_r10.pack(bref));
  }
  if (inter_rat_params_utra_tdd_v1020_present) {
    HANDLE_CODE(inter_rat_params_utra_tdd_v1020.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1020_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_category_v1020_present, 1));
  HANDLE_CODE(bref.unpack(phy_layer_params_v1020_present, 1));
  HANDLE_CODE(bref.unpack(rf_params_v1020_present, 1));
  HANDLE_CODE(bref.unpack(meas_params_v1020_present, 1));
  HANDLE_CODE(bref.unpack(feature_group_ind_rel10_r10_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params_cdma2000_v1020_present, 1));
  HANDLE_CODE(bref.unpack(ue_based_netw_perf_meas_params_r10_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params_utra_tdd_v1020_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_category_v1020_present) {
    HANDLE_CODE(unpack_integer(ue_category_v1020, bref, (uint8_t)6u, (uint8_t)8u));
  }
  if (phy_layer_params_v1020_present) {
    HANDLE_CODE(phy_layer_params_v1020.unpack(bref));
  }
  if (rf_params_v1020_present) {
    HANDLE_CODE(rf_params_v1020.unpack(bref));
  }
  if (meas_params_v1020_present) {
    HANDLE_CODE(meas_params_v1020.unpack(bref));
  }
  if (feature_group_ind_rel10_r10_present) {
    HANDLE_CODE(feature_group_ind_rel10_r10.unpack(bref));
  }
  if (inter_rat_params_cdma2000_v1020_present) {
    HANDLE_CODE(inter_rat_params_cdma2000_v1020.unpack(bref));
  }
  if (ue_based_netw_perf_meas_params_r10_present) {
    HANDLE_CODE(ue_based_netw_perf_meas_params_r10.unpack(bref));
  }
  if (inter_rat_params_utra_tdd_v1020_present) {
    HANDLE_CODE(inter_rat_params_utra_tdd_v1020.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1020_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_category_v1020_present) {
    j.write_int("ue-Category-v1020", ue_category_v1020);
  }
  if (phy_layer_params_v1020_present) {
    j.write_fieldname("phyLayerParameters-v1020");
    phy_layer_params_v1020.to_json(j);
  }
  if (rf_params_v1020_present) {
    j.write_fieldname("rf-Parameters-v1020");
    rf_params_v1020.to_json(j);
  }
  if (meas_params_v1020_present) {
    j.write_fieldname("measParameters-v1020");
    meas_params_v1020.to_json(j);
  }
  if (feature_group_ind_rel10_r10_present) {
    j.write_str("featureGroupIndRel10-r10", feature_group_ind_rel10_r10.to_string());
  }
  if (inter_rat_params_cdma2000_v1020_present) {
    j.write_fieldname("interRAT-ParametersCDMA2000-v1020");
    inter_rat_params_cdma2000_v1020.to_json(j);
  }
  if (ue_based_netw_perf_meas_params_r10_present) {
    j.write_fieldname("ue-BasedNetwPerfMeasParameters-r10");
    ue_based_netw_perf_meas_params_r10.to_json(j);
  }
  if (inter_rat_params_utra_tdd_v1020_present) {
    j.write_fieldname("interRAT-ParametersUTRA-TDD-v1020");
    inter_rat_params_utra_tdd_v1020.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CSG-ProximityIndicationParameters-r9 ::= SEQUENCE
SRSASN_CODE csg_proximity_ind_params_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(intra_freq_proximity_ind_r9_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_proximity_ind_r9_present, 1));
  HANDLE_CODE(bref.pack(utran_proximity_ind_r9_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csg_proximity_ind_params_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(intra_freq_proximity_ind_r9_present, 1));
  HANDLE_CODE(bref.unpack(inter_freq_proximity_ind_r9_present, 1));
  HANDLE_CODE(bref.unpack(utran_proximity_ind_r9_present, 1));

  return SRSASN_SUCCESS;
}
void csg_proximity_ind_params_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (intra_freq_proximity_ind_r9_present) {
    j.write_str("intraFreqProximityIndication-r9", "supported");
  }
  if (inter_freq_proximity_ind_r9_present) {
    j.write_str("interFreqProximityIndication-r9", "supported");
  }
  if (utran_proximity_ind_r9_present) {
    j.write_str("utran-ProximityIndication-r9", "supported");
  }
  j.end_obj();
}

// IRAT-ParametersCDMA2000-1XRTT-v920 ::= SEQUENCE
SRSASN_CODE irat_params_cdma2000_minus1_xrtt_v920_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(e_csfb_conc_ps_mob1_xrtt_r9_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_cdma2000_minus1_xrtt_v920_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(e_csfb_conc_ps_mob1_xrtt_r9_present, 1));

  return SRSASN_SUCCESS;
}
void irat_params_cdma2000_minus1_xrtt_v920_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e-CSFB-1XRTT-r9", "supported");
  if (e_csfb_conc_ps_mob1_xrtt_r9_present) {
    j.write_str("e-CSFB-ConcPS-Mob1XRTT-r9", "supported");
  }
  j.end_obj();
}

// IRAT-ParametersGERAN-v920 ::= SEQUENCE
SRSASN_CODE irat_params_geran_v920_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dtm_r9_present, 1));
  HANDLE_CODE(bref.pack(e_redirection_geran_r9_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_geran_v920_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dtm_r9_present, 1));
  HANDLE_CODE(bref.unpack(e_redirection_geran_r9_present, 1));

  return SRSASN_SUCCESS;
}
void irat_params_geran_v920_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dtm_r9_present) {
    j.write_str("dtm-r9", "supported");
  }
  if (e_redirection_geran_r9_present) {
    j.write_str("e-RedirectionGERAN-r9", "supported");
  }
  j.end_obj();
}

// IRAT-ParametersUTRA-v920 ::= SEQUENCE
SRSASN_CODE irat_params_utra_v920_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_utra_v920_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void irat_params_utra_v920_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e-RedirectionUTRA-r9", "supported");
  j.end_obj();
}

// NeighCellSI-AcquisitionParameters-r9 ::= SEQUENCE
SRSASN_CODE neigh_cell_si_acquisition_params_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(intra_freq_si_acquisition_for_ho_r9_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_si_acquisition_for_ho_r9_present, 1));
  HANDLE_CODE(bref.pack(utran_si_acquisition_for_ho_r9_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE neigh_cell_si_acquisition_params_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(intra_freq_si_acquisition_for_ho_r9_present, 1));
  HANDLE_CODE(bref.unpack(inter_freq_si_acquisition_for_ho_r9_present, 1));
  HANDLE_CODE(bref.unpack(utran_si_acquisition_for_ho_r9_present, 1));

  return SRSASN_SUCCESS;
}
void neigh_cell_si_acquisition_params_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (intra_freq_si_acquisition_for_ho_r9_present) {
    j.write_str("intraFreqSI-AcquisitionForHO-r9", "supported");
  }
  if (inter_freq_si_acquisition_for_ho_r9_present) {
    j.write_str("interFreqSI-AcquisitionForHO-r9", "supported");
  }
  if (utran_si_acquisition_for_ho_r9_present) {
    j.write_str("utran-SI-AcquisitionForHO-r9", "supported");
  }
  j.end_obj();
}

// PhyLayerParameters-v920 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v920_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(enhanced_dual_layer_fdd_r9_present, 1));
  HANDLE_CODE(bref.pack(enhanced_dual_layer_tdd_r9_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v920_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(enhanced_dual_layer_fdd_r9_present, 1));
  HANDLE_CODE(bref.unpack(enhanced_dual_layer_tdd_r9_present, 1));

  return SRSASN_SUCCESS;
}
void phy_layer_params_v920_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (enhanced_dual_layer_fdd_r9_present) {
    j.write_str("enhancedDualLayerFDD-r9", "supported");
  }
  if (enhanced_dual_layer_tdd_r9_present) {
    j.write_str("enhancedDualLayerTDD-r9", "supported");
  }
  j.end_obj();
}

// SON-Parameters-r9 ::= SEQUENCE
SRSASN_CODE son_params_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rach_report_r9_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE son_params_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rach_report_r9_present, 1));

  return SRSASN_SUCCESS;
}
void son_params_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rach_report_r9_present) {
    j.write_str("rach-Report-r9", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v940-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v940_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE ue_eutra_cap_v940_ies_s::unpack(cbit_ref& bref)
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
void ue_eutra_cap_v940_ies_s::to_json(json_writer& j) const
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

// AccessStratumRelease ::= ENUMERATED
const char* access_stratum_release_opts::to_string() const
{
  static const char* options[] = {"rel8", "rel9", "rel10", "rel11", "rel12", "rel13", "rel14", "rel15"};
  return convert_enum_idx(options, 8, value, "access_stratum_release_e");
}
uint8_t access_stratum_release_opts::to_number() const
{
  static const uint8_t options[] = {8, 9, 10, 11, 12, 13, 14, 15};
  return map_enum_number(options, 8, value, "access_stratum_release_e");
}

// PhyLayerParameters ::= SEQUENCE
SRSASN_CODE phy_layer_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_tx_ant_sel_supported, 1));
  HANDLE_CODE(bref.pack(ue_specific_ref_sigs_supported, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_tx_ant_sel_supported, 1));
  HANDLE_CODE(bref.unpack(ue_specific_ref_sigs_supported, 1));

  return SRSASN_SUCCESS;
}
void phy_layer_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("ue-TxAntennaSelectionSupported", ue_tx_ant_sel_supported);
  j.write_bool("ue-SpecificRefSigsSupported", ue_specific_ref_sigs_supported);
  j.end_obj();
}

// UE-EUTRA-Capability-v920-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v920_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_rat_params_utra_v920_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params_cdma2000_v920_present, 1));
  HANDLE_CODE(bref.pack(dev_type_r9_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(phy_layer_params_v920.pack(bref));
  HANDLE_CODE(inter_rat_params_geran_v920.pack(bref));
  if (inter_rat_params_utra_v920_present) {
    HANDLE_CODE(inter_rat_params_utra_v920.pack(bref));
  }
  if (inter_rat_params_cdma2000_v920_present) {
    HANDLE_CODE(inter_rat_params_cdma2000_v920.pack(bref));
  }
  HANDLE_CODE(csg_proximity_ind_params_r9.pack(bref));
  HANDLE_CODE(neigh_cell_si_acquisition_params_r9.pack(bref));
  HANDLE_CODE(son_params_r9.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v920_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(inter_rat_params_utra_v920_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params_cdma2000_v920_present, 1));
  HANDLE_CODE(bref.unpack(dev_type_r9_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(phy_layer_params_v920.unpack(bref));
  HANDLE_CODE(inter_rat_params_geran_v920.unpack(bref));
  if (inter_rat_params_utra_v920_present) {
    HANDLE_CODE(inter_rat_params_utra_v920.unpack(bref));
  }
  if (inter_rat_params_cdma2000_v920_present) {
    HANDLE_CODE(inter_rat_params_cdma2000_v920.unpack(bref));
  }
  HANDLE_CODE(csg_proximity_ind_params_r9.unpack(bref));
  HANDLE_CODE(neigh_cell_si_acquisition_params_r9.unpack(bref));
  HANDLE_CODE(son_params_r9.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v920_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("phyLayerParameters-v920");
  phy_layer_params_v920.to_json(j);
  j.write_fieldname("interRAT-ParametersGERAN-v920");
  inter_rat_params_geran_v920.to_json(j);
  if (inter_rat_params_utra_v920_present) {
    j.write_fieldname("interRAT-ParametersUTRA-v920");
    inter_rat_params_utra_v920.to_json(j);
  }
  if (inter_rat_params_cdma2000_v920_present) {
    j.write_fieldname("interRAT-ParametersCDMA2000-v920");
    inter_rat_params_cdma2000_v920.to_json(j);
  }
  if (dev_type_r9_present) {
    j.write_str("deviceType-r9", "noBenFromBatConsumpOpt");
  }
  j.write_fieldname("csg-ProximityIndicationParameters-r9");
  csg_proximity_ind_params_r9.to_json(j);
  j.write_fieldname("neighCellSI-AcquisitionParameters-r9");
  neigh_cell_si_acquisition_params_r9.to_json(j);
  j.write_fieldname("son-Parameters-r9");
  son_params_r9.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-Capability ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(feature_group_inds_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(access_stratum_release.pack(bref));
  HANDLE_CODE(pack_integer(bref, ue_category, (uint8_t)1u, (uint8_t)5u));
  HANDLE_CODE(pdcp_params.pack(bref));
  HANDLE_CODE(phy_layer_params.pack(bref));
  HANDLE_CODE(rf_params.pack(bref));
  HANDLE_CODE(meas_params.pack(bref));
  if (feature_group_inds_present) {
    HANDLE_CODE(feature_group_inds.pack(bref));
  }
  HANDLE_CODE(bref.pack(inter_rat_params.utra_fdd_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params.utra_tdd128_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params.utra_tdd384_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params.utra_tdd768_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params.geran_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params.cdma2000_hrpd_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params.cdma2000_minus1x_rtt_present, 1));
  if (inter_rat_params.utra_fdd_present) {
    HANDLE_CODE(inter_rat_params.utra_fdd.pack(bref));
  }
  if (inter_rat_params.utra_tdd128_present) {
    HANDLE_CODE(inter_rat_params.utra_tdd128.pack(bref));
  }
  if (inter_rat_params.utra_tdd384_present) {
    HANDLE_CODE(inter_rat_params.utra_tdd384.pack(bref));
  }
  if (inter_rat_params.utra_tdd768_present) {
    HANDLE_CODE(inter_rat_params.utra_tdd768.pack(bref));
  }
  if (inter_rat_params.geran_present) {
    HANDLE_CODE(inter_rat_params.geran.pack(bref));
  }
  if (inter_rat_params.cdma2000_hrpd_present) {
    HANDLE_CODE(inter_rat_params.cdma2000_hrpd.pack(bref));
  }
  if (inter_rat_params.cdma2000_minus1x_rtt_present) {
    HANDLE_CODE(inter_rat_params.cdma2000_minus1x_rtt.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(feature_group_inds_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(access_stratum_release.unpack(bref));
  HANDLE_CODE(unpack_integer(ue_category, bref, (uint8_t)1u, (uint8_t)5u));
  HANDLE_CODE(pdcp_params.unpack(bref));
  HANDLE_CODE(phy_layer_params.unpack(bref));
  HANDLE_CODE(rf_params.unpack(bref));
  HANDLE_CODE(meas_params.unpack(bref));
  if (feature_group_inds_present) {
    HANDLE_CODE(feature_group_inds.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(inter_rat_params.utra_fdd_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params.utra_tdd128_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params.utra_tdd384_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params.utra_tdd768_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params.geran_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params.cdma2000_hrpd_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params.cdma2000_minus1x_rtt_present, 1));
  if (inter_rat_params.utra_fdd_present) {
    HANDLE_CODE(inter_rat_params.utra_fdd.unpack(bref));
  }
  if (inter_rat_params.utra_tdd128_present) {
    HANDLE_CODE(inter_rat_params.utra_tdd128.unpack(bref));
  }
  if (inter_rat_params.utra_tdd384_present) {
    HANDLE_CODE(inter_rat_params.utra_tdd384.unpack(bref));
  }
  if (inter_rat_params.utra_tdd768_present) {
    HANDLE_CODE(inter_rat_params.utra_tdd768.unpack(bref));
  }
  if (inter_rat_params.geran_present) {
    HANDLE_CODE(inter_rat_params.geran.unpack(bref));
  }
  if (inter_rat_params.cdma2000_hrpd_present) {
    HANDLE_CODE(inter_rat_params.cdma2000_hrpd.unpack(bref));
  }
  if (inter_rat_params.cdma2000_minus1x_rtt_present) {
    HANDLE_CODE(inter_rat_params.cdma2000_minus1x_rtt.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("accessStratumRelease", access_stratum_release.to_string());
  j.write_int("ue-Category", ue_category);
  j.write_fieldname("pdcp-Parameters");
  pdcp_params.to_json(j);
  j.write_fieldname("phyLayerParameters");
  phy_layer_params.to_json(j);
  j.write_fieldname("rf-Parameters");
  rf_params.to_json(j);
  j.write_fieldname("measParameters");
  meas_params.to_json(j);
  if (feature_group_inds_present) {
    j.write_str("featureGroupIndicators", feature_group_inds.to_string());
  }
  j.write_fieldname("interRAT-Parameters");
  j.start_obj();
  if (inter_rat_params.utra_fdd_present) {
    j.write_fieldname("utraFDD");
    inter_rat_params.utra_fdd.to_json(j);
  }
  if (inter_rat_params.utra_tdd128_present) {
    j.write_fieldname("utraTDD128");
    inter_rat_params.utra_tdd128.to_json(j);
  }
  if (inter_rat_params.utra_tdd384_present) {
    j.write_fieldname("utraTDD384");
    inter_rat_params.utra_tdd384.to_json(j);
  }
  if (inter_rat_params.utra_tdd768_present) {
    j.write_fieldname("utraTDD768");
    inter_rat_params.utra_tdd768.to_json(j);
  }
  if (inter_rat_params.geran_present) {
    j.write_fieldname("geran");
    inter_rat_params.geran.to_json(j);
  }
  if (inter_rat_params.cdma2000_hrpd_present) {
    j.write_fieldname("cdma2000-HRPD");
    inter_rat_params.cdma2000_hrpd.to_json(j);
  }
  if (inter_rat_params.cdma2000_minus1x_rtt_present) {
    j.write_fieldname("cdma2000-1xRTT");
    inter_rat_params.cdma2000_minus1x_rtt.to_json(j);
  }
  j.end_obj();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// PhyLayerParameters-v14a0 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v14a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssp10_tdd_only_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v14a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssp10_tdd_only_r14_present, 1));

  return SRSASN_SUCCESS;
}
void phy_layer_params_v14a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssp10_tdd_only_r14_present) {
    j.write_str("ssp10-TDD-Only-r14", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v14b0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v14b0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v14b0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v14b0_present) {
    HANDLE_CODE(rf_params_v14b0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v14b0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v14b0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v14b0_present) {
    HANDLE_CODE(rf_params_v14b0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v14b0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v14b0_present) {
    j.write_fieldname("rf-Parameters-v14b0");
    rf_params_v14b0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// MBMS-Parameters-v1470 ::= SEQUENCE
SRSASN_CODE mbms_params_v1470_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mbms_scaling_factor1dot25_r14_present, 1));
  HANDLE_CODE(bref.pack(mbms_scaling_factor7dot5_r14_present, 1));

  HANDLE_CODE(mbms_max_bw_r14.pack(bref));
  if (mbms_scaling_factor1dot25_r14_present) {
    HANDLE_CODE(mbms_scaling_factor1dot25_r14.pack(bref));
  }
  if (mbms_scaling_factor7dot5_r14_present) {
    HANDLE_CODE(mbms_scaling_factor7dot5_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_params_v1470_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mbms_scaling_factor1dot25_r14_present, 1));
  HANDLE_CODE(bref.unpack(mbms_scaling_factor7dot5_r14_present, 1));

  HANDLE_CODE(mbms_max_bw_r14.unpack(bref));
  if (mbms_scaling_factor1dot25_r14_present) {
    HANDLE_CODE(mbms_scaling_factor1dot25_r14.unpack(bref));
  }
  if (mbms_scaling_factor7dot5_r14_present) {
    HANDLE_CODE(mbms_scaling_factor7dot5_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mbms_params_v1470_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mbms-MaxBW-r14");
  mbms_max_bw_r14.to_json(j);
  if (mbms_scaling_factor1dot25_r14_present) {
    j.write_str("mbms-ScalingFactor1dot25-r14", mbms_scaling_factor1dot25_r14.to_string());
  }
  if (mbms_scaling_factor7dot5_r14_present) {
    j.write_str("mbms-ScalingFactor7dot5-r14", mbms_scaling_factor7dot5_r14.to_string());
  }
  j.end_obj();
}

void mbms_params_v1470_s::mbms_max_bw_r14_c_::set(types::options e)
{
  type_ = e;
}
void mbms_params_v1470_s::mbms_max_bw_r14_c_::set_implicit_value()
{
  set(types::implicit_value);
}
uint8_t& mbms_params_v1470_s::mbms_max_bw_r14_c_::set_explicit_value()
{
  set(types::explicit_value);
  return c;
}
void mbms_params_v1470_s::mbms_max_bw_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::implicit_value:
      break;
    case types::explicit_value:
      j.write_int("explicitValue", c);
      break;
    default:
      log_invalid_choice_id(type_, "mbms_params_v1470_s::mbms_max_bw_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE mbms_params_v1470_s::mbms_max_bw_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::implicit_value:
      break;
    case types::explicit_value:
      HANDLE_CODE(pack_integer(bref, c, (uint8_t)2u, (uint8_t)20u));
      break;
    default:
      log_invalid_choice_id(type_, "mbms_params_v1470_s::mbms_max_bw_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_params_v1470_s::mbms_max_bw_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::implicit_value:
      break;
    case types::explicit_value:
      HANDLE_CODE(unpack_integer(c, bref, (uint8_t)2u, (uint8_t)20u));
      break;
    default:
      log_invalid_choice_id(type_, "mbms_params_v1470_s::mbms_max_bw_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mbms_params_v1470_s::mbms_scaling_factor1dot25_r14_opts::to_string() const
{
  static const char* options[] = {"n3", "n6", "n9", "n12"};
  return convert_enum_idx(options, 4, value, "mbms_params_v1470_s::mbms_scaling_factor1dot25_r14_e_");
}
uint8_t mbms_params_v1470_s::mbms_scaling_factor1dot25_r14_opts::to_number() const
{
  static const uint8_t options[] = {3, 6, 9, 12};
  return map_enum_number(options, 4, value, "mbms_params_v1470_s::mbms_scaling_factor1dot25_r14_e_");
}

const char* mbms_params_v1470_s::mbms_scaling_factor7dot5_r14_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4"};
  return convert_enum_idx(options, 4, value, "mbms_params_v1470_s::mbms_scaling_factor7dot5_r14_e_");
}
uint8_t mbms_params_v1470_s::mbms_scaling_factor7dot5_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4};
  return map_enum_number(options, 4, value, "mbms_params_v1470_s::mbms_scaling_factor7dot5_r14_e_");
}

// UE-EUTRA-Capability-v14a0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v14a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(phy_layer_params_v14a0.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v14a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(phy_layer_params_v14a0.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v14a0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("phyLayerParameters-v14a0");
  phy_layer_params_v14a0.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1470-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1470_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mbms_params_v1470_present, 1));
  HANDLE_CODE(bref.pack(phy_layer_params_v1470_present, 1));
  HANDLE_CODE(bref.pack(rf_params_v1470_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mbms_params_v1470_present) {
    HANDLE_CODE(mbms_params_v1470.pack(bref));
  }
  if (phy_layer_params_v1470_present) {
    HANDLE_CODE(phy_layer_params_v1470.pack(bref));
  }
  if (rf_params_v1470_present) {
    HANDLE_CODE(rf_params_v1470.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1470_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mbms_params_v1470_present, 1));
  HANDLE_CODE(bref.unpack(phy_layer_params_v1470_present, 1));
  HANDLE_CODE(bref.unpack(rf_params_v1470_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mbms_params_v1470_present) {
    HANDLE_CODE(mbms_params_v1470.unpack(bref));
  }
  if (phy_layer_params_v1470_present) {
    HANDLE_CODE(phy_layer_params_v1470.unpack(bref));
  }
  if (rf_params_v1470_present) {
    HANDLE_CODE(rf_params_v1470.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1470_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbms_params_v1470_present) {
    j.write_fieldname("mbms-Parameters-v1470");
    mbms_params_v1470.to_json(j);
  }
  if (phy_layer_params_v1470_present) {
    j.write_fieldname("phyLayerParameters-v1470");
    phy_layer_params_v1470.to_json(j);
  }
  if (rf_params_v1470_present) {
    j.write_fieldname("rf-Parameters-v1470");
    rf_params_v1470.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CE-Parameters-v1380 ::= SEQUENCE
SRSASN_CODE ce_params_v1380_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tm6_ce_mode_a_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ce_params_v1380_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tm6_ce_mode_a_r13_present, 1));

  return SRSASN_SUCCESS;
}
void ce_params_v1380_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tm6_ce_mode_a_r13_present) {
    j.write_str("tm6-CE-ModeA-r13", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v13e0a-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v13e0a_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE ue_eutra_cap_v13e0a_ies_s::unpack(cbit_ref& bref)
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
void ue_eutra_cap_v13e0a_ies_s::to_json(json_writer& j) const
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

// CE-Parameters-v1370 ::= SEQUENCE
SRSASN_CODE ce_params_v1370_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tm9_ce_mode_a_r13_present, 1));
  HANDLE_CODE(bref.pack(tm9_ce_mode_b_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ce_params_v1370_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tm9_ce_mode_a_r13_present, 1));
  HANDLE_CODE(bref.unpack(tm9_ce_mode_b_r13_present, 1));

  return SRSASN_SUCCESS;
}
void ce_params_v1370_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tm9_ce_mode_a_r13_present) {
    j.write_str("tm9-CE-ModeA-r13", "supported");
  }
  if (tm9_ce_mode_b_r13_present) {
    j.write_str("tm9-CE-ModeB-r13", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1390-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1390_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v1390_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v1390_present) {
    HANDLE_CODE(rf_params_v1390.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1390_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v1390_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v1390_present) {
    HANDLE_CODE(rf_params_v1390.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1390_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v1390_present) {
    j.write_fieldname("rf-Parameters-v1390");
    rf_params_v1390.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v1380 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1380_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ce_params_v1380.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1380_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ce_params_v1380.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v1380_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ce-Parameters-v1380");
  ce_params_v1380.to_json(j);
  j.end_obj();
}

// UE-EUTRA-Capability-v1380-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1380_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v1380_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v1380_present) {
    HANDLE_CODE(rf_params_v1380.pack(bref));
  }
  HANDLE_CODE(ce_params_v1380.pack(bref));
  HANDLE_CODE(fdd_add_ue_eutra_cap_v1380.pack(bref));
  HANDLE_CODE(tdd_add_ue_eutra_cap_v1380.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1380_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v1380_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v1380_present) {
    HANDLE_CODE(rf_params_v1380.unpack(bref));
  }
  HANDLE_CODE(ce_params_v1380.unpack(bref));
  HANDLE_CODE(fdd_add_ue_eutra_cap_v1380.unpack(bref));
  HANDLE_CODE(tdd_add_ue_eutra_cap_v1380.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1380_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v1380_present) {
    j.write_fieldname("rf-Parameters-v1380");
    rf_params_v1380.to_json(j);
  }
  j.write_fieldname("ce-Parameters-v1380");
  ce_params_v1380.to_json(j);
  j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v1380");
  fdd_add_ue_eutra_cap_v1380.to_json(j);
  j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v1380");
  tdd_add_ue_eutra_cap_v1380.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-v1370 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1370_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ce_params_v1370_present, 1));

  if (ce_params_v1370_present) {
    HANDLE_CODE(ce_params_v1370.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_v1370_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ce_params_v1370_present, 1));

  if (ce_params_v1370_present) {
    HANDLE_CODE(ce_params_v1370.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_v1370_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ce_params_v1370_present) {
    j.write_fieldname("ce-Parameters-v1370");
    ce_params_v1370.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v1370-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v1370_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ce_params_v1370_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_eutra_cap_v1370_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_eutra_cap_v1370_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ce_params_v1370_present) {
    HANDLE_CODE(ce_params_v1370.pack(bref));
  }
  if (fdd_add_ue_eutra_cap_v1370_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1370.pack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1370_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1370.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v1370_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ce_params_v1370_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_eutra_cap_v1370_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_eutra_cap_v1370_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ce_params_v1370_present) {
    HANDLE_CODE(ce_params_v1370.unpack(bref));
  }
  if (fdd_add_ue_eutra_cap_v1370_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_v1370.unpack(bref));
  }
  if (tdd_add_ue_eutra_cap_v1370_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_v1370.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v1370_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ce_params_v1370_present) {
    j.write_fieldname("ce-Parameters-v1370");
    ce_params_v1370.to_json(j);
  }
  if (fdd_add_ue_eutra_cap_v1370_present) {
    j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-v1370");
    fdd_add_ue_eutra_cap_v1370.to_json(j);
  }
  if (tdd_add_ue_eutra_cap_v1370_present) {
    j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-v1370");
    tdd_add_ue_eutra_cap_v1370.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RF-Parameters-v12b0 ::= SEQUENCE
SRSASN_CODE rf_params_v12b0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(max_layers_mimo_ind_r12_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v12b0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(max_layers_mimo_ind_r12_present, 1));

  return SRSASN_SUCCESS;
}
void rf_params_v12b0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (max_layers_mimo_ind_r12_present) {
    j.write_str("maxLayersMIMO-Indication-r12", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v12x0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v12x0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE ue_eutra_cap_v12x0_ies_s::unpack(cbit_ref& bref)
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
void ue_eutra_cap_v12x0_ies_s::to_json(json_writer& j) const
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

// UE-EUTRA-Capability-v12b0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v12b0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v12b0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v12b0_present) {
    HANDLE_CODE(rf_params_v12b0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v12b0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v12b0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v12b0_present) {
    HANDLE_CODE(rf_params_v12b0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v12b0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v12b0_present) {
    j.write_fieldname("rf-Parameters-v12b0");
    rf_params_v12b0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// Other-Parameters-v11d0 ::= SEQUENCE
SRSASN_CODE other_params_v11d0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(in_dev_coex_ind_ul_ca_r11_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE other_params_v11d0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(in_dev_coex_ind_ul_ca_r11_present, 1));

  return SRSASN_SUCCESS;
}
void other_params_v11d0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (in_dev_coex_ind_ul_ca_r11_present) {
    j.write_str("inDeviceCoexInd-UL-CA-r11", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v11x0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v11x0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE ue_eutra_cap_v11x0_ies_s::unpack(cbit_ref& bref)
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
void ue_eutra_cap_v11x0_ies_s::to_json(json_writer& j) const
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

// UE-EUTRA-Capability-v11d0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v11d0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v11d0_present, 1));
  HANDLE_CODE(bref.pack(other_params_v11d0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v11d0_present) {
    HANDLE_CODE(rf_params_v11d0.pack(bref));
  }
  if (other_params_v11d0_present) {
    HANDLE_CODE(other_params_v11d0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v11d0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v11d0_present, 1));
  HANDLE_CODE(bref.unpack(other_params_v11d0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v11d0_present) {
    HANDLE_CODE(rf_params_v11d0.unpack(bref));
  }
  if (other_params_v11d0_present) {
    HANDLE_CODE(other_params_v11d0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v11d0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v11d0_present) {
    j.write_fieldname("rf-Parameters-v11d0");
    rf_params_v11d0.to_json(j);
  }
  if (other_params_v11d0_present) {
    j.write_fieldname("otherParameters-v11d0");
    other_params_v11d0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RF-Parameters-v10f0 ::= SEQUENCE
SRSASN_CODE rf_params_v10f0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(modified_mpr_behavior_r10_present, 1));

  if (modified_mpr_behavior_r10_present) {
    HANDLE_CODE(modified_mpr_behavior_r10.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v10f0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(modified_mpr_behavior_r10_present, 1));

  if (modified_mpr_behavior_r10_present) {
    HANDLE_CODE(modified_mpr_behavior_r10.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v10f0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (modified_mpr_behavior_r10_present) {
    j.write_str("modifiedMPR-Behavior-r10", modified_mpr_behavior_r10.to_string());
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v10i0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v10i0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v10i0_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v10i0_present) {
    HANDLE_CODE(rf_params_v10i0.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v10i0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v10i0_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v10i0_present) {
    HANDLE_CODE(rf_params_v10i0.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v10i0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v10i0_present) {
    j.write_fieldname("rf-Parameters-v10i0");
    rf_params_v10i0.to_json(j);
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

// UE-EUTRA-Capability-v10f0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v10f0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v10f0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v10f0_present) {
    HANDLE_CODE(rf_params_v10f0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v10f0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v10f0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v10f0_present) {
    HANDLE_CODE(rf_params_v10f0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v10f0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v10f0_present) {
    j.write_fieldname("rf-Parameters-v10f0");
    rf_params_v10f0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v10c0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v10c0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(otdoa_positioning_cap_r10_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (otdoa_positioning_cap_r10_present) {
    HANDLE_CODE(otdoa_positioning_cap_r10.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v10c0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(otdoa_positioning_cap_r10_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (otdoa_positioning_cap_r10_present) {
    HANDLE_CODE(otdoa_positioning_cap_r10.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v10c0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (otdoa_positioning_cap_r10_present) {
    j.write_fieldname("otdoa-PositioningCapabilities-r10");
    otdoa_positioning_cap_r10.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RF-Parameters-v10j0 ::= SEQUENCE
SRSASN_CODE rf_params_v10j0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(multi_ns_pmax_r10_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v10j0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(multi_ns_pmax_r10_present, 1));

  return SRSASN_SUCCESS;
}
void rf_params_v10j0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (multi_ns_pmax_r10_present) {
    j.write_str("multiNS-Pmax-r10", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v10j0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v10j0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v10j0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v10j0_present) {
    HANDLE_CODE(rf_params_v10j0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v10j0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v10j0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v10j0_present) {
    HANDLE_CODE(rf_params_v10j0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v10j0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v10j0_present) {
    j.write_fieldname("rf-Parameters-v10j0");
    rf_params_v10j0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// IRAT-ParametersUTRA-v9h0 ::= SEQUENCE
SRSASN_CODE irat_params_utra_v9h0_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_utra_v9h0_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void irat_params_utra_v9h0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("mfbi-UTRA-r9", "supported");
  j.end_obj();
}

// UE-EUTRA-Capability-v9h0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v9h0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_rat_params_utra_v9h0_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (inter_rat_params_utra_v9h0_present) {
    HANDLE_CODE(inter_rat_params_utra_v9h0.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v9h0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(inter_rat_params_utra_v9h0_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (inter_rat_params_utra_v9h0_present) {
    HANDLE_CODE(inter_rat_params_utra_v9h0.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v9h0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (inter_rat_params_utra_v9h0_present) {
    j.write_fieldname("interRAT-ParametersUTRA-v9h0");
    inter_rat_params_utra_v9h0.to_json(j);
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

// PhyLayerParameters-v9d0 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v9d0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tm5_fdd_r9_present, 1));
  HANDLE_CODE(bref.pack(tm5_tdd_r9_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v9d0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tm5_fdd_r9_present, 1));
  HANDLE_CODE(bref.unpack(tm5_tdd_r9_present, 1));

  return SRSASN_SUCCESS;
}
void phy_layer_params_v9d0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tm5_fdd_r9_present) {
    j.write_str("tm5-FDD-r9", "supported");
  }
  if (tm5_tdd_r9_present) {
    j.write_str("tm5-TDD-r9", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v9e0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v9e0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v9e0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v9e0_present) {
    HANDLE_CODE(rf_params_v9e0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v9e0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v9e0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v9e0_present) {
    HANDLE_CODE(rf_params_v9e0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v9e0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v9e0_present) {
    j.write_fieldname("rf-Parameters-v9e0");
    rf_params_v9e0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// IRAT-ParametersUTRA-v9c0 ::= SEQUENCE
SRSASN_CODE irat_params_utra_v9c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(voice_over_ps_hs_utra_fdd_r9_present, 1));
  HANDLE_CODE(bref.pack(voice_over_ps_hs_utra_tdd128_r9_present, 1));
  HANDLE_CODE(bref.pack(srvcc_from_utra_fdd_to_utra_fdd_r9_present, 1));
  HANDLE_CODE(bref.pack(srvcc_from_utra_fdd_to_geran_r9_present, 1));
  HANDLE_CODE(bref.pack(srvcc_from_utra_tdd128_to_utra_tdd128_r9_present, 1));
  HANDLE_CODE(bref.pack(srvcc_from_utra_tdd128_to_geran_r9_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE irat_params_utra_v9c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(voice_over_ps_hs_utra_fdd_r9_present, 1));
  HANDLE_CODE(bref.unpack(voice_over_ps_hs_utra_tdd128_r9_present, 1));
  HANDLE_CODE(bref.unpack(srvcc_from_utra_fdd_to_utra_fdd_r9_present, 1));
  HANDLE_CODE(bref.unpack(srvcc_from_utra_fdd_to_geran_r9_present, 1));
  HANDLE_CODE(bref.unpack(srvcc_from_utra_tdd128_to_utra_tdd128_r9_present, 1));
  HANDLE_CODE(bref.unpack(srvcc_from_utra_tdd128_to_geran_r9_present, 1));

  return SRSASN_SUCCESS;
}
void irat_params_utra_v9c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (voice_over_ps_hs_utra_fdd_r9_present) {
    j.write_str("voiceOverPS-HS-UTRA-FDD-r9", "supported");
  }
  if (voice_over_ps_hs_utra_tdd128_r9_present) {
    j.write_str("voiceOverPS-HS-UTRA-TDD128-r9", "supported");
  }
  if (srvcc_from_utra_fdd_to_utra_fdd_r9_present) {
    j.write_str("srvcc-FromUTRA-FDD-ToUTRA-FDD-r9", "supported");
  }
  if (srvcc_from_utra_fdd_to_geran_r9_present) {
    j.write_str("srvcc-FromUTRA-FDD-ToGERAN-r9", "supported");
  }
  if (srvcc_from_utra_tdd128_to_utra_tdd128_r9_present) {
    j.write_str("srvcc-FromUTRA-TDD128-ToUTRA-TDD128-r9", "supported");
  }
  if (srvcc_from_utra_tdd128_to_geran_r9_present) {
    j.write_str("srvcc-FromUTRA-TDD128-ToGERAN-r9", "supported");
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v9d0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v9d0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_layer_params_v9d0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (phy_layer_params_v9d0_present) {
    HANDLE_CODE(phy_layer_params_v9d0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v9d0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_layer_params_v9d0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (phy_layer_params_v9d0_present) {
    HANDLE_CODE(phy_layer_params_v9d0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v9d0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_v9d0_present) {
    j.write_fieldname("phyLayerParameters-v9d0");
    phy_layer_params_v9d0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v9c0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v9c0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_rat_params_utra_v9c0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (inter_rat_params_utra_v9c0_present) {
    HANDLE_CODE(inter_rat_params_utra_v9c0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v9c0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(inter_rat_params_utra_v9c0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (inter_rat_params_utra_v9c0_present) {
    HANDLE_CODE(inter_rat_params_utra_v9c0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v9c0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (inter_rat_params_utra_v9c0_present) {
    j.write_fieldname("interRAT-ParametersUTRA-v9c0");
    inter_rat_params_utra_v9c0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-CapabilityAddXDD-Mode-r9 ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_add_xdd_mode_r9_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(phy_layer_params_r9_present, 1));
  HANDLE_CODE(bref.pack(feature_group_inds_r9_present, 1));
  HANDLE_CODE(bref.pack(feature_group_ind_rel9_add_r9_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params_geran_r9_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params_utra_r9_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params_cdma2000_r9_present, 1));
  HANDLE_CODE(bref.pack(neigh_cell_si_acquisition_params_r9_present, 1));

  if (phy_layer_params_r9_present) {
    HANDLE_CODE(phy_layer_params_r9.pack(bref));
  }
  if (feature_group_inds_r9_present) {
    HANDLE_CODE(feature_group_inds_r9.pack(bref));
  }
  if (feature_group_ind_rel9_add_r9_present) {
    HANDLE_CODE(feature_group_ind_rel9_add_r9.pack(bref));
  }
  if (inter_rat_params_geran_r9_present) {
    HANDLE_CODE(inter_rat_params_geran_r9.pack(bref));
  }
  if (inter_rat_params_utra_r9_present) {
    HANDLE_CODE(inter_rat_params_utra_r9.pack(bref));
  }
  if (inter_rat_params_cdma2000_r9_present) {
    HANDLE_CODE(inter_rat_params_cdma2000_r9.pack(bref));
  }
  if (neigh_cell_si_acquisition_params_r9_present) {
    HANDLE_CODE(neigh_cell_si_acquisition_params_r9.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_add_xdd_mode_r9_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(phy_layer_params_r9_present, 1));
  HANDLE_CODE(bref.unpack(feature_group_inds_r9_present, 1));
  HANDLE_CODE(bref.unpack(feature_group_ind_rel9_add_r9_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params_geran_r9_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params_utra_r9_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params_cdma2000_r9_present, 1));
  HANDLE_CODE(bref.unpack(neigh_cell_si_acquisition_params_r9_present, 1));

  if (phy_layer_params_r9_present) {
    HANDLE_CODE(phy_layer_params_r9.unpack(bref));
  }
  if (feature_group_inds_r9_present) {
    HANDLE_CODE(feature_group_inds_r9.unpack(bref));
  }
  if (feature_group_ind_rel9_add_r9_present) {
    HANDLE_CODE(feature_group_ind_rel9_add_r9.unpack(bref));
  }
  if (inter_rat_params_geran_r9_present) {
    HANDLE_CODE(inter_rat_params_geran_r9.unpack(bref));
  }
  if (inter_rat_params_utra_r9_present) {
    HANDLE_CODE(inter_rat_params_utra_r9.unpack(bref));
  }
  if (inter_rat_params_cdma2000_r9_present) {
    HANDLE_CODE(inter_rat_params_cdma2000_r9.unpack(bref));
  }
  if (neigh_cell_si_acquisition_params_r9_present) {
    HANDLE_CODE(neigh_cell_si_acquisition_params_r9.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_add_xdd_mode_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_layer_params_r9_present) {
    j.write_fieldname("phyLayerParameters-r9");
    phy_layer_params_r9.to_json(j);
  }
  if (feature_group_inds_r9_present) {
    j.write_str("featureGroupIndicators-r9", feature_group_inds_r9.to_string());
  }
  if (feature_group_ind_rel9_add_r9_present) {
    j.write_str("featureGroupIndRel9Add-r9", feature_group_ind_rel9_add_r9.to_string());
  }
  if (inter_rat_params_geran_r9_present) {
    j.write_fieldname("interRAT-ParametersGERAN-r9");
    inter_rat_params_geran_r9.to_json(j);
  }
  if (inter_rat_params_utra_r9_present) {
    j.write_fieldname("interRAT-ParametersUTRA-r9");
    inter_rat_params_utra_r9.to_json(j);
  }
  if (inter_rat_params_cdma2000_r9_present) {
    j.write_fieldname("interRAT-ParametersCDMA2000-r9");
    inter_rat_params_cdma2000_r9.to_json(j);
  }
  if (neigh_cell_si_acquisition_params_r9_present) {
    j.write_fieldname("neighCellSI-AcquisitionParameters-r9");
    neigh_cell_si_acquisition_params_r9.to_json(j);
  }
  j.end_obj();
}

// UE-EUTRA-Capability-v9a0-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v9a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(feature_group_ind_rel9_add_r9_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_eutra_cap_r9_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_eutra_cap_r9_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (feature_group_ind_rel9_add_r9_present) {
    HANDLE_CODE(feature_group_ind_rel9_add_r9.pack(bref));
  }
  if (fdd_add_ue_eutra_cap_r9_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_r9.pack(bref));
  }
  if (tdd_add_ue_eutra_cap_r9_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_r9.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v9a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(feature_group_ind_rel9_add_r9_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_eutra_cap_r9_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_eutra_cap_r9_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (feature_group_ind_rel9_add_r9_present) {
    HANDLE_CODE(feature_group_ind_rel9_add_r9.unpack(bref));
  }
  if (fdd_add_ue_eutra_cap_r9_present) {
    HANDLE_CODE(fdd_add_ue_eutra_cap_r9.unpack(bref));
  }
  if (tdd_add_ue_eutra_cap_r9_present) {
    HANDLE_CODE(tdd_add_ue_eutra_cap_r9.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v9a0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (feature_group_ind_rel9_add_r9_present) {
    j.write_str("featureGroupIndRel9Add-r9", feature_group_ind_rel9_add_r9.to_string());
  }
  if (fdd_add_ue_eutra_cap_r9_present) {
    j.write_fieldname("fdd-Add-UE-EUTRA-Capabilities-r9");
    fdd_add_ue_eutra_cap_r9.to_json(j);
  }
  if (tdd_add_ue_eutra_cap_r9_present) {
    j.write_fieldname("tdd-Add-UE-EUTRA-Capabilities-r9");
    tdd_add_ue_eutra_cap_r9.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UERadioAccessCapabilityInformation-r8-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_access_cap_info_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_radio_access_cap_info.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_radio_access_cap_info.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_radio_access_cap_info_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ue-RadioAccessCapabilityInfo", ue_radio_access_cap_info.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UERadioAccessCapabilityInformation ::= SEQUENCE
SRSASN_CODE ue_radio_access_cap_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_radio_access_cap_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_radio_access_cap_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_access_cap_info_s::crit_exts_c_::c1_c_& ue_radio_access_cap_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ue_radio_access_cap_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_radio_access_cap_info_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_access_cap_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_access_cap_info_r8_ies_s& ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_ue_radio_access_cap_info_r8()
{
  set(types::ue_radio_access_cap_info_r8);
  return c;
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_radio_access_cap_info_r8:
      j.write_fieldname("ueRadioAccessCapabilityInformation-r8");
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
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_radio_access_cap_info_r8:
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
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_radio_access_cap_info_r8:
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
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
