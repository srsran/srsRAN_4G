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
#include "srsran/asn1/nas_5g_ies.h"

#include "srsran/asn1/asn1_utils.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/config.h"

#include <array>
#include <stdint.h>
#include <vector>

namespace srsran {
namespace nas_5g {

using namespace asn1;
// IE: 5GS registration type
// Reference: 9.11.3.7
SRSASN_CODE registration_type_5gs_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(follow_on_request_bit.pack(bref));
  HANDLE_CODE(registration_type.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: 5GS registration type
// Reference: 9.11.3.7
SRSASN_CODE registration_type_5gs_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(follow_on_request_bit.unpack(bref));
  HANDLE_CODE(registration_type.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: key set identifier
// Reference: 9.11.3.32
SRSASN_CODE key_set_identifier_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(security_context_flag.pack(bref));
  HANDLE_CODE(nas_key_set_identifier.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: key set identifier
// Reference: 9.11.3.32
SRSASN_CODE key_set_identifier_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(security_context_flag.unpack(bref));
  HANDLE_CODE(nas_key_set_identifier.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: 5GS mobile identity
// Reference: 9.11.3.4
SRSASN_CODE mobile_identity_5gs_t::pack(asn1::bit_ref& bref)
{
  // Length
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));
  asn1::bit_ref bref_tmp = bref;
  switch (type_) {
    case identity_types::no_identity:
      HANDLE_CODE(type_.pack(bref));
      break;
    case identity_types::suci: {
      suci_s* choice = srslog::detail::any_cast<suci_s>(&choice_container);
      HANDLE_CODE(choice->pack(bref, bref_tmp));
      break;
    }
    case identity_types::guti_5g: {
      guti_5g_s* choice = srslog::detail::any_cast<guti_5g_s>(&choice_container);
      HANDLE_CODE(choice->pack(bref, bref_tmp));
      break;
    }
    case identity_types::imei: {
      imei_s* choice = srslog::detail::any_cast<imei_s>(&choice_container);
      HANDLE_CODE(choice->pack(bref, bref_tmp));
      break;
    }
    case identity_types::s_tmsi_5g: {
      s_tmsi_5g_s* choice = srslog::detail::any_cast<s_tmsi_5g_s>(&choice_container);
      HANDLE_CODE(choice->pack(bref, bref_tmp));
      break;
    }
    case identity_types::imeisv: {
      imeisv_s* choice = srslog::detail::any_cast<imeisv_s>(&choice_container);
      HANDLE_CODE(choice->pack(bref, bref_tmp));
      break;
    }
    case identity_types::mac_address: {
      mac_address_s* choice = srslog::detail::any_cast<mac_address_s>(&choice_container);
      HANDLE_CODE(choice->pack(bref, bref_tmp));
      break;
    }
    case identity_types::eui_64: {
      eui_64_s* choice = srslog::detail::any_cast<eui_64_s>(&choice_container);
      HANDLE_CODE(choice->pack(bref, bref_tmp));
      break;
    }
    default:
      log_invalid_choice_id(type_, "5G NAS ID TYPE");
      return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  uint16_t length = (uint16_t)((bref.distance(bref_length) / 8) - 2);
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: 5GS mobile identity
// Reference: 9.11.3.4
SRSASN_CODE mobile_identity_5gs_t::unpack(asn1::cbit_ref& bref)
{
  // Length
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  uint8_t tmp;
  HANDLE_CODE(bref.unpack(tmp, 5));
  identity_types e = identity_types::no_identity;
  HANDLE_CODE(e.unpack(bref));
  set(e);
  switch (type_) {
    case identity_types::no_identity:
      break;
    case identity_types::suci: {
      choice_container = srslog::detail::any{suci_s()};
      suci_s* choice   = srslog::detail::any_cast<suci_s>(&choice_container);
      HANDLE_CODE(choice->unpack(bref, tmp, length));
      break;
    }
    case identity_types::guti_5g: {
      choice_container  = srslog::detail::any{guti_5g_s()};
      guti_5g_s* choice = srslog::detail::any_cast<guti_5g_s>(&choice_container);
      HANDLE_CODE(choice->unpack(bref, tmp, length));
      break;
    }
    case identity_types::imei: {
      choice_container = srslog::detail::any{imei_s()};
      imei_s* choice   = srslog::detail::any_cast<imei_s>(&choice_container);
      HANDLE_CODE(choice->unpack(bref, tmp, length));
      break;
    }
    case identity_types::s_tmsi_5g: {
      choice_container    = srslog::detail::any{s_tmsi_5g_s()};
      s_tmsi_5g_s* choice = srslog::detail::any_cast<s_tmsi_5g_s>(&choice_container);
      HANDLE_CODE(choice->unpack(bref, tmp, length));
      break;
    }
    case identity_types::imeisv: {
      choice_container = srslog::detail::any{imeisv_s()};
      imeisv_s* choice = srslog::detail::any_cast<imeisv_s>(&choice_container);
      HANDLE_CODE(choice->unpack(bref, tmp, length));
      break;
    }
    case identity_types::mac_address: {
      choice_container      = srslog::detail::any{mac_address_s()};
      mac_address_s* choice = srslog::detail::any_cast<mac_address_s>(&choice_container);
      HANDLE_CODE(choice->unpack(bref, tmp, length));
      break;
    }
    case identity_types::eui_64: {
      choice_container = srslog::detail::any{eui_64_s()};
      eui_64_s* choice = srslog::detail::any_cast<eui_64_s>(&choice_container);
      HANDLE_CODE(choice->unpack(bref, tmp, length));
      break;
    }
    default:
      log_invalid_choice_id(type_, "5G NAS ID TYPE");
      return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mobile_identity_5gs_t::identity_types_::to_string()
{
  switch (value) {
    case identity_types_::no_identity:
      return "No identity";
    case identity_types_::suci:
      return "SUCI";
    case identity_types_::guti_5g:
      return "5G-GUTI";
    case identity_types_::imei:
      return "IMEI";
    case identity_types_::s_tmsi_5g:
      return "5G-S-TMSI";
    case identity_types_::imeisv:
      return "IMEISV";
    case identity_types_::mac_address:
      return "MAC address";
    case identity_types_::eui_64:
      return "EUI-64";
    default:
      return "Invalid Choice";
  }
}
SRSASN_CODE mobile_identity_5gs_t::suci_s::pack(asn1::bit_ref& bref, asn1::bit_ref& bref_tmp)
{
  HANDLE_CODE(bref.advance_bits(1));
  HANDLE_CODE(supi_format.pack(bref));
  HANDLE_CODE(bref.advance_bits(1));

  // Pack Type of identity
  uint8_t type = static_cast<uint8_t>(mobile_identity_5gs_t::identity_types_::options::suci);
  HANDLE_CODE(bref.pack(type, 3));

  HANDLE_CODE(pack_mcc_mnc(mcc.data(), mnc.data(), bref));
  HANDLE_CODE(bref.pack(routing_indicator[1], 4));
  HANDLE_CODE(bref.pack(routing_indicator[0], 4));
  HANDLE_CODE(bref.pack(routing_indicator[3], 4));
  HANDLE_CODE(bref.pack(routing_indicator[2], 4));
  // Spare
  HANDLE_CODE(bref.advance_bits(4));
  HANDLE_CODE(protection_scheme_id.pack(bref));
  HANDLE_CODE(bref.pack(home_network_public_key_identifier, 8));
  HANDLE_CODE(bref.pack_bytes(scheme_output.data(), scheme_output.size()));
  return SRSASN_SUCCESS;
}
SRSASN_CODE mobile_identity_5gs_t::suci_s::unpack(asn1::cbit_ref& bref, uint8_t tmp, uint32_t length)
{
  supi_format = static_cast<typename supi_format_type::options>(tmp >> 1 & 0b111);
  HANDLE_CODE(unpack_mcc_mnc(mcc.data(), mnc.data(), bref));
  HANDLE_CODE(bref.unpack(routing_indicator[1], 4));
  HANDLE_CODE(bref.unpack(routing_indicator[0], 4));
  HANDLE_CODE(bref.unpack(routing_indicator[3], 4));
  HANDLE_CODE(bref.unpack(routing_indicator[2], 4));
  // Spare
  HANDLE_CODE(bref.advance_bits(4));
  HANDLE_CODE(protection_scheme_id.unpack(bref));
  HANDLE_CODE(bref.unpack(home_network_public_key_identifier, 8));
  scheme_output.resize(length - 8);
  HANDLE_CODE(bref.unpack_bytes(scheme_output.data(), length - 8));
  return SRSASN_SUCCESS;
}

SRSASN_CODE mobile_identity_5gs_t::guti_5g_s::pack(asn1::bit_ref& bref, asn1::bit_ref& bref_tmp)
{
  HANDLE_CODE(bref.pack(0x0, 5));
  // Pack Type of identity
  uint8_t type = static_cast<uint8_t>(mobile_identity_5gs_t::identity_types_::options::guti_5g);
  HANDLE_CODE(bref.pack(type, 3));

  HANDLE_CODE(pack_mcc_mnc(mcc.data(), mnc.data(), bref));
  HANDLE_CODE(bref.pack(amf_region_id, 8));
  HANDLE_CODE(bref.pack(amf_set_id, 10));
  HANDLE_CODE(bref.pack(amf_pointer, 6));
  HANDLE_CODE(bref.pack(tmsi_5g, 4 * 8));
  return SRSASN_SUCCESS;
}
SRSASN_CODE mobile_identity_5gs_t::guti_5g_s::unpack(asn1::cbit_ref& bref, uint8_t tmp, uint32_t length)
{
  HANDLE_CODE(unpack_mcc_mnc(mcc.data(), mnc.data(), bref));
  HANDLE_CODE(bref.unpack(amf_region_id, 8));
  HANDLE_CODE(bref.unpack(amf_set_id, 10));
  HANDLE_CODE(bref.unpack(amf_pointer, 6));
  HANDLE_CODE(bref.unpack(tmsi_5g, 4 * 8));
  return SRSASN_SUCCESS;
}

SRSASN_CODE mobile_identity_5gs_t::imei_s::pack(asn1::bit_ref& bref, asn1::bit_ref& bref_tmp)
{
  HANDLE_CODE(bref.pack(imei[0], 4));
  HANDLE_CODE(bref.pack(odd_even_indicator, 1));

  // Pack Type of identity
  uint8_t type = static_cast<uint8_t>(mobile_identity_5gs_t::identity_types_::options::imei);
  HANDLE_CODE(bref.pack(type, 3));

  HANDLE_CODE(bref.pack(imei[2], 4));
  HANDLE_CODE(bref.pack(imei[1], 4));
  HANDLE_CODE(bref.pack(imei[4], 4));
  HANDLE_CODE(bref.pack(imei[3], 4));
  HANDLE_CODE(bref.pack(imei[6], 4));
  HANDLE_CODE(bref.pack(imei[5], 4));
  HANDLE_CODE(bref.pack(imei[8], 4));
  HANDLE_CODE(bref.pack(imei[7], 4));
  HANDLE_CODE(bref.pack(imei[10], 4));
  HANDLE_CODE(bref.pack(imei[9], 4));
  HANDLE_CODE(bref.pack(imei[12], 4));
  HANDLE_CODE(bref.pack(imei[11], 4));
  HANDLE_CODE(bref.pack(imei[14], 4));
  HANDLE_CODE(bref.pack(imei[13], 4));
  return SRSASN_SUCCESS;
}
SRSASN_CODE mobile_identity_5gs_t::imei_s::unpack(asn1::cbit_ref& bref, uint8_t tmp, uint32_t length)
{
  imei[0]            = (uint8_t)(tmp >> 1) & 0x0f;
  odd_even_indicator = (bool)(tmp & 0x01); //  true = odd number
  HANDLE_CODE(bref.unpack(imei[2], 4));
  HANDLE_CODE(bref.unpack(imei[1], 4));
  HANDLE_CODE(bref.unpack(imei[4], 4));
  HANDLE_CODE(bref.unpack(imei[3], 4));
  HANDLE_CODE(bref.unpack(imei[6], 4));
  HANDLE_CODE(bref.unpack(imei[5], 4));
  HANDLE_CODE(bref.unpack(imei[8], 4));
  HANDLE_CODE(bref.unpack(imei[7], 4));
  HANDLE_CODE(bref.unpack(imei[10], 4));
  HANDLE_CODE(bref.unpack(imei[9], 4));
  HANDLE_CODE(bref.unpack(imei[12], 4));
  HANDLE_CODE(bref.unpack(imei[11], 4));
  HANDLE_CODE(bref.unpack(imei[14], 4));
  HANDLE_CODE(bref.unpack(imei[13], 4));
  return SRSASN_SUCCESS;
}

SRSASN_CODE mobile_identity_5gs_t::s_tmsi_5g_s::pack(asn1::bit_ref& bref, asn1::bit_ref& bref_tmp)
{
  // Pack Type of identity
  uint8_t type = static_cast<uint8_t>(mobile_identity_5gs_t::identity_types_::options::s_tmsi_5g);
  HANDLE_CODE(bref.pack(type, 3));

  HANDLE_CODE(bref.unpack(amf_set_id, 10));
  HANDLE_CODE(bref.unpack(amf_pointer, 6));
  HANDLE_CODE(bref.unpack(tmsi_5g, 4 * 8));
  return SRSASN_SUCCESS;
}
SRSASN_CODE mobile_identity_5gs_t::s_tmsi_5g_s::unpack(asn1::cbit_ref& bref, uint8_t tmp, uint32_t length)
{
  HANDLE_CODE(bref.unpack(amf_set_id, 10));
  HANDLE_CODE(bref.unpack(amf_pointer, 6));
  HANDLE_CODE(bref.unpack(tmsi_5g, 4 * 8));
  return SRSASN_SUCCESS;
}

SRSASN_CODE mobile_identity_5gs_t::imeisv_s::pack(asn1::bit_ref& bref, asn1::bit_ref& bref_tmp)
{
  HANDLE_CODE(bref.pack(imeisv[0], 4));
  HANDLE_CODE(bref.pack(odd_even_indicator, 1));

  // Pack Type of identity
  uint8_t type = static_cast<uint8_t>(mobile_identity_5gs_t::identity_types_::options::imeisv);
  HANDLE_CODE(bref.pack(type, 3));

  HANDLE_CODE(bref.pack(imeisv[2], 4));
  HANDLE_CODE(bref.pack(imeisv[1], 4));
  HANDLE_CODE(bref.pack(imeisv[4], 4));
  HANDLE_CODE(bref.pack(imeisv[3], 4));
  HANDLE_CODE(bref.pack(imeisv[6], 4));
  HANDLE_CODE(bref.pack(imeisv[5], 4));
  HANDLE_CODE(bref.pack(imeisv[8], 4));
  HANDLE_CODE(bref.pack(imeisv[7], 4));
  HANDLE_CODE(bref.pack(imeisv[10], 4));
  HANDLE_CODE(bref.pack(imeisv[9], 4));
  HANDLE_CODE(bref.pack(imeisv[12], 4));
  HANDLE_CODE(bref.pack(imeisv[11], 4));
  HANDLE_CODE(bref.pack(imeisv[14], 4));
  HANDLE_CODE(bref.pack(imeisv[13], 4));
  HANDLE_CODE(bref.pack(0xf, 4));
  HANDLE_CODE(bref.pack(imeisv[15], 4));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mobile_identity_5gs_t::imeisv_s::unpack(asn1::cbit_ref& bref, uint8_t tmp, uint32_t length)
{
  imeisv[0]          = (uint8_t)(tmp >> 1) & 0x0f;
  odd_even_indicator = (bool)(tmp & 0x01); //  true = odd number
  HANDLE_CODE(bref.unpack(imeisv[2], 4));
  HANDLE_CODE(bref.unpack(imeisv[1], 4));
  HANDLE_CODE(bref.unpack(imeisv[4], 4));
  HANDLE_CODE(bref.unpack(imeisv[3], 4));
  HANDLE_CODE(bref.unpack(imeisv[6], 4));
  HANDLE_CODE(bref.unpack(imeisv[5], 4));
  HANDLE_CODE(bref.unpack(imeisv[8], 4));
  HANDLE_CODE(bref.unpack(imeisv[7], 4));
  HANDLE_CODE(bref.unpack(imeisv[10], 4));
  HANDLE_CODE(bref.unpack(imeisv[9], 4));
  HANDLE_CODE(bref.unpack(imeisv[12], 4));
  HANDLE_CODE(bref.unpack(imeisv[11], 4));
  HANDLE_CODE(bref.unpack(imeisv[14], 4));
  HANDLE_CODE(bref.unpack(imeisv[13], 4));
  HANDLE_CODE(bref.advance_bits(4));
  HANDLE_CODE(bref.unpack(imeisv[15], 4));
  return SRSASN_SUCCESS;
}

SRSASN_CODE mobile_identity_5gs_t::mac_address_s::pack(asn1::bit_ref& bref, asn1::bit_ref& bref_tmp)
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE mobile_identity_5gs_t::mac_address_s::unpack(asn1::cbit_ref& bref, uint8_t tmp, uint32_t length)
{
  return SRSASN_SUCCESS;
}

SRSASN_CODE mobile_identity_5gs_t::eui_64_s::pack(asn1::bit_ref& bref, asn1::bit_ref& bref_tmp)
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE mobile_identity_5gs_t::eui_64_s::unpack(asn1::cbit_ref& bref, uint8_t tmp, uint32_t length)
{
  return SRSASN_SUCCESS;
}

// IE: 5GMM capability
// Reference: 9.11.3.1
SRSASN_CODE capability_5gmm_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack(sgc, 1));
  HANDLE_CODE(bref.pack(iphc_cp_c_io_t_5g, 1));
  HANDLE_CODE(bref.pack(n3_data, 1));
  HANDLE_CODE(bref.pack(cp_c_io_t_5g, 1));
  HANDLE_CODE(bref.pack(restrict_ec, 1));
  HANDLE_CODE(bref.pack(lpp, 1));
  HANDLE_CODE(bref.pack(ho_attach, 1));
  HANDLE_CODE(bref.pack(s1_mode, 1));

  HANDLE_CODE(bref.pack(racs, 1));
  HANDLE_CODE(bref.pack(nssaa, 1));
  HANDLE_CODE(bref.pack(lcs_5g, 1));
  HANDLE_CODE(bref.pack(v2_xcnpc5, 1));
  HANDLE_CODE(bref.pack(v2_xcepc5, 1));
  HANDLE_CODE(bref.pack(v2_x, 1));
  HANDLE_CODE(bref.pack(up_c_io_t_5g, 1));
  HANDLE_CODE(bref.pack(srvcc_5g, 1));

  HANDLE_CODE(bref.advance_bits(4));
  HANDLE_CODE(bref.pack(ehc_cp_c_io_t_5g, 1));
  HANDLE_CODE(bref.pack(multiple_up, 1));
  HANDLE_CODE(bref.pack(wusa, 1));
  HANDLE_CODE(bref.pack(cag, 1));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 1 || length > 13) {
    asn1::log_error("Encoding Failed (5GMM capability): Packed length (%d) is not in range of min: 1 and max 13 bytes",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: 5GMM capability
// Reference: 9.11.3.1
SRSASN_CODE capability_5gmm_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 1 || length > 13) {
    asn1::log_error("Decoding Failed (5GMM capability): Length (%d) is not in range of min: 1 and max 13 bytes",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack(sgc, 1));
  HANDLE_CODE(bref.unpack(iphc_cp_c_io_t_5g, 1));
  HANDLE_CODE(bref.unpack(n3_data, 1));
  HANDLE_CODE(bref.unpack(cp_c_io_t_5g, 1));
  HANDLE_CODE(bref.unpack(restrict_ec, 1));
  HANDLE_CODE(bref.unpack(lpp, 1));
  HANDLE_CODE(bref.unpack(ho_attach, 1));
  HANDLE_CODE(bref.unpack(s1_mode, 1));

  if (length < 2) {
    return SRSASN_SUCCESS;
  }

  HANDLE_CODE(bref.unpack(racs, 1));
  HANDLE_CODE(bref.unpack(nssaa, 1));
  HANDLE_CODE(bref.unpack(lcs_5g, 1));
  HANDLE_CODE(bref.unpack(v2_xcnpc5, 1));
  HANDLE_CODE(bref.unpack(v2_xcepc5, 1));
  HANDLE_CODE(bref.unpack(v2_x, 1));
  HANDLE_CODE(bref.unpack(up_c_io_t_5g, 1));
  HANDLE_CODE(bref.unpack(srvcc_5g, 1));

  if (length < 3) {
    return SRSASN_SUCCESS;
  }

  HANDLE_CODE(bref.advance_bits(4));
  HANDLE_CODE(bref.unpack(ehc_cp_c_io_t_5g, 1));
  HANDLE_CODE(bref.unpack(multiple_up, 1));
  HANDLE_CODE(bref.unpack(wusa, 1));
  HANDLE_CODE(bref.unpack(cag, 1));
  return SRSASN_SUCCESS;
}

// IE: UE security capability
// Reference: 9.11.3.54
SRSASN_CODE ue_security_capability_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack(ea0_5g_supported, 1));
  HANDLE_CODE(bref.pack(ea1_128_5g_supported, 1));
  HANDLE_CODE(bref.pack(ea2_128_5g_supported, 1));
  HANDLE_CODE(bref.pack(ea3_128_5g_supported, 1));
  HANDLE_CODE(bref.pack(ea4_5g_supported, 1));
  HANDLE_CODE(bref.pack(ea5_5g_supported, 1));
  HANDLE_CODE(bref.pack(ea6_5g_supported, 1));
  HANDLE_CODE(bref.pack(ea7_5g_supported, 1));
  HANDLE_CODE(bref.pack(ia0_5g_supported, 1));

  HANDLE_CODE(bref.pack(ia1_128_5g_supported, 1));
  HANDLE_CODE(bref.pack(ia2_128_5g_supported, 1));
  HANDLE_CODE(bref.pack(ia3_128_5g_supported, 1));
  HANDLE_CODE(bref.pack(ia4_5g_supported, 1));
  HANDLE_CODE(bref.pack(ia5_5g_supported, 1));
  HANDLE_CODE(bref.pack(ia6_5g_supported, 1));
  HANDLE_CODE(bref.pack(ia7_5g_supported, 1));

  if (eps_caps_present == true) {
    HANDLE_CODE(bref.pack(eea0_supported, 1));
    HANDLE_CODE(bref.pack(eea1_128_supported, 1));
    HANDLE_CODE(bref.pack(eea2_128_supported, 1));
    HANDLE_CODE(bref.pack(eea3_128_supported, 1));
    HANDLE_CODE(bref.pack(eea4_supported, 1));
    HANDLE_CODE(bref.pack(eea5_supported, 1));
    HANDLE_CODE(bref.pack(eea6_supported, 1));
    HANDLE_CODE(bref.pack(eea7_supported, 1));
    HANDLE_CODE(bref.pack(eia0_supported, 1));
    HANDLE_CODE(bref.pack(eia1_128_supported, 1));
    HANDLE_CODE(bref.pack(eia2_128_supported, 1));
    HANDLE_CODE(bref.pack(eia3_128_supported, 1));
    HANDLE_CODE(bref.pack(eia4_supported, 1));
    HANDLE_CODE(bref.pack(eia5_supported, 1));
    HANDLE_CODE(bref.pack(eia6_supported, 1));
    HANDLE_CODE(bref.pack(eia7_supported, 1));
  }
  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 2 || length > 8) {
    asn1::log_error(
        "Encoding Failed (UE security capability): Packed length (%d) is not in range of min: 2 and max 8 bytes",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: UE security capability
// Reference: 9.11.3.54
SRSASN_CODE ue_security_capability_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 2 || length > 8) {
    asn1::log_error("Decoding Failed (UE security capability): Length (%d) is not in range of min: 2 and max 8 bytes",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack(ea0_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ea1_128_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ea2_128_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ea3_128_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ea4_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ea5_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ea6_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ea7_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ia0_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ia1_128_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ia2_128_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ia3_128_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ia4_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ia5_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ia6_5g_supported, 1));
  HANDLE_CODE(bref.unpack(ia7_5g_supported, 1));
  if (length > 2) {
    eps_caps_present = true;
    HANDLE_CODE(bref.unpack(eea0_supported, 1));
    HANDLE_CODE(bref.unpack(eea1_128_supported, 1));
    HANDLE_CODE(bref.unpack(eea2_128_supported, 1));
    HANDLE_CODE(bref.unpack(eea3_128_supported, 1));
    HANDLE_CODE(bref.unpack(eea4_supported, 1));
    HANDLE_CODE(bref.unpack(eea5_supported, 1));
    HANDLE_CODE(bref.unpack(eea6_supported, 1));
    HANDLE_CODE(bref.unpack(eea7_supported, 1));
    HANDLE_CODE(bref.unpack(eia0_supported, 1));
    HANDLE_CODE(bref.unpack(eia1_128_supported, 1));
    HANDLE_CODE(bref.unpack(eia2_128_supported, 1));
    HANDLE_CODE(bref.unpack(eia3_128_supported, 1));
    HANDLE_CODE(bref.unpack(eia4_supported, 1));
    HANDLE_CODE(bref.unpack(eia5_supported, 1));
    HANDLE_CODE(bref.unpack(eia6_supported, 1));
    HANDLE_CODE(bref.unpack(eia7_supported, 1));
  }
  if (length > 4) {
    HANDLE_CODE(bref.advance_bits((length - 4) * 8));
  }
  return SRSASN_SUCCESS;
}

// IE: NSSAI
// Reference: 9.11.3.37
SRSASN_CODE nssai_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  for (auto& s_nssai : s_nssai_list) {
    HANDLE_CODE(s_nssai.pack(bref));
  }

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 2 || length > 144) {
    asn1::log_error("Encoding Failed (NSSAI): Packed length (%d) is not in range of min: 2 and max 144 bytes", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: NSSAI
// Reference: 9.11.3.37
SRSASN_CODE nssai_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 2 || length > 144) {
    asn1::log_error("Decoding Failed (NSSAI): Length (%d) is not in range of min: 2 and max 144 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  asn1::cbit_ref bref_start = bref;
  while (floorl(bref.distance(bref_start) / 8) < length) {
    s_nssai_t s_nssai;
    HANDLE_CODE(s_nssai.unpack(bref));
    s_nssai_list.push_back(s_nssai);
  }
  return SRSASN_SUCCESS;
}

// IE: 5GS tracking area identity
// Reference: 9.11.3.8
SRSASN_CODE tracking_area_identity_5gs_t::pack(asn1::bit_ref& bref)
{
  pack_mcc_mnc(mcc.data(), mnc.data(), bref);
  HANDLE_CODE(bref.pack(tac, 3 * 8));
  return SRSASN_SUCCESS;
}

// IE: 5GS tracking area identity
// Reference: 9.11.3.8
SRSASN_CODE tracking_area_identity_5gs_t::unpack(asn1::cbit_ref& bref)
{
  unpack_mcc_mnc(mcc.data(), mnc.data(), bref);
  HANDLE_CODE(bref.unpack(tac, 3 * 8));
  return SRSASN_SUCCESS;
}

// IE: S1 UE network capability
// Reference: 9.11.3.48
SRSASN_CODE s1_ue_network_capability_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack(eea0_supported, 1));
  HANDLE_CODE(bref.pack(eea1_128_supported, 1));
  HANDLE_CODE(bref.pack(eea2_128_supported, 1));
  HANDLE_CODE(bref.pack(eea3_128_supported, 1));
  HANDLE_CODE(bref.pack(eea4_supported, 1));
  HANDLE_CODE(bref.pack(eea5_supported, 1));
  HANDLE_CODE(bref.pack(eea6_supported, 1));
  HANDLE_CODE(bref.pack(eea7_supported, 1));
  HANDLE_CODE(bref.pack(eia0_supported, 1));

  HANDLE_CODE(bref.pack(eia1_128_supported, 1));
  HANDLE_CODE(bref.pack(eia2_128_supported, 1));
  HANDLE_CODE(bref.pack(eia3_128_supported, 1));
  HANDLE_CODE(bref.pack(eia4_supported, 1));
  HANDLE_CODE(bref.pack(eia5_supported, 1));
  HANDLE_CODE(bref.pack(eia6_supported, 1));
  HANDLE_CODE(bref.pack(eia7_supported, 1));

  HANDLE_CODE(bref.pack(uea0_supported, 1));
  HANDLE_CODE(bref.pack(uea1_128_supported, 1));
  HANDLE_CODE(bref.pack(uea2_128_supported, 1));
  HANDLE_CODE(bref.pack(uea3_128_supported, 1));
  HANDLE_CODE(bref.pack(uea4_supported, 1));
  HANDLE_CODE(bref.pack(uea5_supported, 1));
  HANDLE_CODE(bref.pack(uea6_supported, 1));
  HANDLE_CODE(bref.pack(uea7_supported, 1));

  HANDLE_CODE(bref.pack(ucs2_support, 1));
  HANDLE_CODE(bref.pack(uia1_128_supported, 1));
  HANDLE_CODE(bref.pack(uia2_128_supported, 1));
  HANDLE_CODE(bref.pack(uia3_128_supported, 1));
  HANDLE_CODE(bref.pack(uia4_supported, 1));
  HANDLE_CODE(bref.pack(uia5_supported, 1));
  HANDLE_CODE(bref.pack(uia6_supported, 1));
  HANDLE_CODE(bref.pack(uia7_supported, 1));

  HANDLE_CODE(bref.pack(pro_se_dd_supported, 1));
  HANDLE_CODE(bref.pack(pro_se_supported, 1));
  HANDLE_CODE(bref.pack(h245_ash_supported, 1));
  HANDLE_CODE(bref.pack(acc_csfb_supported, 1));
  HANDLE_CODE(bref.pack(llp_supported, 1));
  HANDLE_CODE(bref.pack(lcs_supported, 1));
  HANDLE_CODE(bref.pack(srvcc_capability_supported, 1));
  HANDLE_CODE(bref.pack(nf_capability_supported, 1));

  HANDLE_CODE(bref.pack(e_pco_supported, 1));
  HANDLE_CODE(bref.pack(hc_cp_c_io_t_supported, 1));
  HANDLE_CODE(bref.pack(e_rw_o_pdn_supported, 1));
  HANDLE_CODE(bref.pack(s1_u_data_supported, 1));
  HANDLE_CODE(bref.pack(up_c_io_t_supported, 1));
  HANDLE_CODE(bref.pack(cp_c_io_t_supported, 1));
  HANDLE_CODE(bref.pack(pro_se_relay_supported, 1));

  HANDLE_CODE(bref.pack(pro_se_dc_supported, 1));
  HANDLE_CODE(bref.pack(max_15_eps_bearer_supported, 1));
  HANDLE_CODE(bref.pack(sgc_supported, 1));
  HANDLE_CODE(bref.pack(n1mode_supported, 1));
  HANDLE_CODE(bref.pack(dcnr_supported, 1));
  HANDLE_CODE(bref.pack(cp_backoff_supported, 1));
  HANDLE_CODE(bref.pack(restrict_ec_supported, 1));
  HANDLE_CODE(bref.pack(v2_x_pc5_supported, 1));
  HANDLE_CODE(bref.pack(multiple_drb_supported, 1));

  HANDLE_CODE(bref.advance_bits(3));
  HANDLE_CODE(bref.pack(nr_pc5_supported, 1));
  HANDLE_CODE(bref.pack(up_mt_edt_supported, 1));
  HANDLE_CODE(bref.pack(cp_mt_edt_supported, 1));
  HANDLE_CODE(bref.pack(wus_supported, 1));
  HANDLE_CODE(bref.pack(racs_supported, 1));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 2 || length > 13) {
    asn1::log_error(
        "Encoding Failed (S1 UE network capability): Packed length (%d) is not in range of min: 2 and max 13 bytes",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: S1 UE network capability
// Reference: 9.11.3.48
SRSASN_CODE s1_ue_network_capability_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 2 || length > 13) {
    asn1::log_error(
        "Decoding Failed (S1 UE network capability): Length (%d) is not in range of min: 2 and max 13 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack(eea0_supported, 1));
  HANDLE_CODE(bref.unpack(eea1_128_supported, 1));
  HANDLE_CODE(bref.unpack(eea2_128_supported, 1));
  HANDLE_CODE(bref.unpack(eea3_128_supported, 1));
  HANDLE_CODE(bref.unpack(eea4_supported, 1));
  HANDLE_CODE(bref.unpack(eea5_supported, 1));
  HANDLE_CODE(bref.unpack(eea6_supported, 1));
  HANDLE_CODE(bref.unpack(eea7_supported, 1));

  HANDLE_CODE(bref.unpack(eia0_supported, 1));
  HANDLE_CODE(bref.unpack(eia1_128_supported, 1));
  HANDLE_CODE(bref.unpack(eia2_128_supported, 1));
  HANDLE_CODE(bref.unpack(eia3_128_supported, 1));
  HANDLE_CODE(bref.unpack(eia4_supported, 1));
  HANDLE_CODE(bref.unpack(eia5_supported, 1));
  HANDLE_CODE(bref.unpack(eia6_supported, 1));
  HANDLE_CODE(bref.unpack(eia7_supported, 1));

  if (length < 3) {
    return SRSASN_SUCCESS;
  }

  HANDLE_CODE(bref.unpack(uea0_supported, 1));
  HANDLE_CODE(bref.unpack(uea1_128_supported, 1));
  HANDLE_CODE(bref.unpack(uea2_128_supported, 1));
  HANDLE_CODE(bref.unpack(uea3_128_supported, 1));
  HANDLE_CODE(bref.unpack(uea4_supported, 1));
  HANDLE_CODE(bref.unpack(uea5_supported, 1));
  HANDLE_CODE(bref.unpack(uea6_supported, 1));
  HANDLE_CODE(bref.unpack(uea7_supported, 1));

  if (length < 4) {
    return SRSASN_SUCCESS;
  }

  HANDLE_CODE(bref.unpack(ucs2_support, 1));
  HANDLE_CODE(bref.unpack(uia1_128_supported, 1));
  HANDLE_CODE(bref.unpack(uia2_128_supported, 1));
  HANDLE_CODE(bref.unpack(uia3_128_supported, 1));
  HANDLE_CODE(bref.unpack(uia4_supported, 1));
  HANDLE_CODE(bref.unpack(uia5_supported, 1));
  HANDLE_CODE(bref.unpack(uia6_supported, 1));
  HANDLE_CODE(bref.unpack(uia7_supported, 1));

  if (length < 5) {
    return SRSASN_SUCCESS;
  }

  HANDLE_CODE(bref.unpack(pro_se_dd_supported, 1));
  HANDLE_CODE(bref.unpack(pro_se_supported, 1));
  HANDLE_CODE(bref.unpack(h245_ash_supported, 1));
  HANDLE_CODE(bref.unpack(acc_csfb_supported, 1));
  HANDLE_CODE(bref.unpack(llp_supported, 1));
  HANDLE_CODE(bref.unpack(lcs_supported, 1));
  HANDLE_CODE(bref.unpack(srvcc_capability_supported, 1));
  HANDLE_CODE(bref.unpack(nf_capability_supported, 1));

  if (length < 6) {
    return SRSASN_SUCCESS;
  }

  HANDLE_CODE(bref.unpack(e_pco_supported, 1));
  HANDLE_CODE(bref.unpack(hc_cp_c_io_t_supported, 1));
  HANDLE_CODE(bref.unpack(e_rw_o_pdn_supported, 1));
  HANDLE_CODE(bref.unpack(s1_u_data_supported, 1));
  HANDLE_CODE(bref.unpack(up_c_io_t_supported, 1));
  HANDLE_CODE(bref.unpack(cp_c_io_t_supported, 1));
  HANDLE_CODE(bref.unpack(pro_se_relay_supported, 1));
  HANDLE_CODE(bref.unpack(pro_se_dc_supported, 1));

  if (length < 7) {
    return SRSASN_SUCCESS;
  }

  HANDLE_CODE(bref.unpack(max_15_eps_bearer_supported, 1));
  HANDLE_CODE(bref.unpack(sgc_supported, 1));
  HANDLE_CODE(bref.unpack(n1mode_supported, 1));
  HANDLE_CODE(bref.unpack(dcnr_supported, 1));
  HANDLE_CODE(bref.unpack(cp_backoff_supported, 1));
  HANDLE_CODE(bref.unpack(restrict_ec_supported, 1));
  HANDLE_CODE(bref.unpack(v2_x_pc5_supported, 1));
  HANDLE_CODE(bref.unpack(multiple_drb_supported, 1));

  if (length < 8) {
    return SRSASN_SUCCESS;
  }
  // 3 spare bits
  bref.advance_bits(3);

  HANDLE_CODE(bref.unpack(nr_pc5_supported, 1));
  HANDLE_CODE(bref.unpack(up_mt_edt_supported, 1));
  HANDLE_CODE(bref.unpack(cp_mt_edt_supported, 1));
  HANDLE_CODE(bref.unpack(wus_supported, 1));
  HANDLE_CODE(bref.unpack(racs_supported, 1));
  return SRSASN_SUCCESS;
}

// IE: Uplink data status
// Reference: 9.11.3.57
SRSASN_CODE uplink_data_status_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack(psi_7, 1));
  HANDLE_CODE(bref.pack(psi_6, 1));
  HANDLE_CODE(bref.pack(psi_5, 1));
  HANDLE_CODE(bref.pack(psi_4, 1));
  HANDLE_CODE(bref.pack(psi_3, 1));
  HANDLE_CODE(bref.pack(psi_2, 1));
  HANDLE_CODE(bref.pack(psi_1, 1));
  HANDLE_CODE(bref.pack(psi_0, 1));
  HANDLE_CODE(bref.pack(psi_15, 1));
  HANDLE_CODE(bref.pack(psi_14, 1));
  HANDLE_CODE(bref.pack(psi_13, 1));
  HANDLE_CODE(bref.pack(psi_12, 1));
  HANDLE_CODE(bref.pack(psi_11, 1));
  HANDLE_CODE(bref.pack(psi_10, 1));
  HANDLE_CODE(bref.pack(psi_9, 1));
  HANDLE_CODE(bref.pack(psi_8, 1));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 2 || length > 32) {
    asn1::log_error(
        "Encoding Failed (Uplink data status): Packed length (%d) is not in range of min: 2 and max 32 bytes", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Uplink data status
// Reference: 9.11.3.57
SRSASN_CODE uplink_data_status_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 2 || length > 32) {
    asn1::log_error("Decoding Failed (Uplink data status): Length (%d) is not in range of min: 2 and max 32 bytes",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack(psi_7, 1));
  HANDLE_CODE(bref.unpack(psi_6, 1));
  HANDLE_CODE(bref.unpack(psi_5, 1));
  HANDLE_CODE(bref.unpack(psi_4, 1));
  HANDLE_CODE(bref.unpack(psi_3, 1));
  HANDLE_CODE(bref.unpack(psi_2, 1));
  HANDLE_CODE(bref.unpack(psi_1, 1));
  HANDLE_CODE(bref.unpack(psi_0, 1));
  HANDLE_CODE(bref.unpack(psi_15, 1));
  HANDLE_CODE(bref.unpack(psi_14, 1));
  HANDLE_CODE(bref.unpack(psi_13, 1));
  HANDLE_CODE(bref.unpack(psi_12, 1));
  HANDLE_CODE(bref.unpack(psi_11, 1));
  HANDLE_CODE(bref.unpack(psi_10, 1));
  HANDLE_CODE(bref.unpack(psi_9, 1));
  HANDLE_CODE(bref.unpack(psi_8, 1));

  if (length > 2) {
    bref.advance_bits((length - 2) * 8);
  }
  return SRSASN_SUCCESS;
}

// IE: PDU session status
// Reference: 9.11.3.44
SRSASN_CODE pdu_session_status_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack(psi_7, 1));
  HANDLE_CODE(bref.pack(psi_6, 1));
  HANDLE_CODE(bref.pack(psi_5, 1));
  HANDLE_CODE(bref.pack(psi_4, 1));
  HANDLE_CODE(bref.pack(psi_3, 1));
  HANDLE_CODE(bref.pack(psi_2, 1));
  HANDLE_CODE(bref.pack(psi_1, 1));
  HANDLE_CODE(bref.pack(psi_0, 1));
  HANDLE_CODE(bref.pack(psi_15, 1));
  HANDLE_CODE(bref.pack(psi_14, 1));
  HANDLE_CODE(bref.pack(psi_13, 1));
  HANDLE_CODE(bref.pack(psi_12, 1));
  HANDLE_CODE(bref.pack(psi_11, 1));
  HANDLE_CODE(bref.pack(psi_10, 1));
  HANDLE_CODE(bref.pack(psi_9, 1));
  HANDLE_CODE(bref.pack(psi_8, 1));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 2 || length > 32) {
    asn1::log_error(
        "Encoding Failed (PDU session status): Packed length (%d) is not in range of min: 2 and max 32 bytes", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: PDU session status
// Reference: 9.11.3.44
SRSASN_CODE pdu_session_status_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 2 || length > 32) {
    asn1::log_error("Decoding Failed (PDU session status): Length (%d) is not in range of min: 2 and max 32 bytes",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack(psi_7, 1));
  HANDLE_CODE(bref.unpack(psi_6, 1));
  HANDLE_CODE(bref.unpack(psi_5, 1));
  HANDLE_CODE(bref.unpack(psi_4, 1));
  HANDLE_CODE(bref.unpack(psi_3, 1));
  HANDLE_CODE(bref.unpack(psi_2, 1));
  HANDLE_CODE(bref.unpack(psi_1, 1));
  HANDLE_CODE(bref.unpack(psi_0, 1));
  HANDLE_CODE(bref.unpack(psi_15, 1));
  HANDLE_CODE(bref.unpack(psi_14, 1));
  HANDLE_CODE(bref.unpack(psi_13, 1));
  HANDLE_CODE(bref.unpack(psi_12, 1));
  HANDLE_CODE(bref.unpack(psi_11, 1));
  HANDLE_CODE(bref.unpack(psi_10, 1));
  HANDLE_CODE(bref.unpack(psi_9, 1));
  HANDLE_CODE(bref.unpack(psi_8, 1));

  if (length > 2) {
    bref.advance_bits((length - 2) * 8);
  }
  return SRSASN_SUCCESS;
}

// IE: MICO indication
// Reference: 9.11.3.31
SRSASN_CODE mico_indication_t::pack(asn1::bit_ref& bref)
{
  // 2 Spare bits
  HANDLE_CODE(bref.pack(0x0, 2));
  HANDLE_CODE(bref.pack(sprti, 1));
  HANDLE_CODE(bref.pack(aai, 1));
  return SRSASN_SUCCESS;
}

// IE: MICO indication
// Reference: 9.11.3.31
SRSASN_CODE mico_indication_t::unpack(asn1::cbit_ref& bref)
{
  // 2 Spare bits
  bref.advance_bits(2);
  HANDLE_CODE(bref.unpack(sprti, 1));
  HANDLE_CODE(bref.unpack(aai, 1));
  return SRSASN_SUCCESS;
}

// IE: UE status
// Reference: 9.11.3.56
SRSASN_CODE ue_status_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // 6 Spare bits
  HANDLE_CODE(bref.pack(0x0, 6));
  HANDLE_CODE(bref.pack(n1_mode_reg, 1));
  HANDLE_CODE(bref.pack(s1_mode_reg, 1));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 1) {
    asn1::log_error("Encoding Failed (UE status): Packed length (%d) does not equal expected length 1", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: UE status
// Reference: 9.11.3.56
SRSASN_CODE ue_status_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 1) {
    asn1::log_error("Decoding Failed (UE status): Length (%d) does not equal expected length 1", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  // 6 Spare bits
  bref.advance_bits(6);
  HANDLE_CODE(bref.unpack(n1_mode_reg, 1));
  HANDLE_CODE(bref.unpack(s1_mode_reg, 1));
  return SRSASN_SUCCESS;
}

// IE: Allowed PDU session status
// Reference: 9.11.3.13
SRSASN_CODE allowed_pdu_session_status_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack(psi_7, 1));
  HANDLE_CODE(bref.pack(psi_6, 1));
  HANDLE_CODE(bref.pack(psi_5, 1));
  HANDLE_CODE(bref.pack(psi_4, 1));
  HANDLE_CODE(bref.pack(psi_3, 1));
  HANDLE_CODE(bref.pack(psi_2, 1));
  HANDLE_CODE(bref.pack(psi_1, 1));
  HANDLE_CODE(bref.pack(psi_0, 1));
  HANDLE_CODE(bref.pack(psi_15, 1));
  HANDLE_CODE(bref.pack(psi_14, 1));
  HANDLE_CODE(bref.pack(psi_13, 1));
  HANDLE_CODE(bref.pack(psi_12, 1));
  HANDLE_CODE(bref.pack(psi_11, 1));
  HANDLE_CODE(bref.pack(psi_10, 1));
  HANDLE_CODE(bref.pack(psi_9, 1));
  HANDLE_CODE(bref.pack(psi_8, 1));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 2 || length > 32) {
    asn1::log_error(
        "Encoding Failed (Allowed PDU session status): Packed length (%d) is not in range of min: 2 and max 32 bytes",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Allowed PDU session status
// Reference: 9.11.3.13
SRSASN_CODE allowed_pdu_session_status_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 2 || length > 32) {
    asn1::log_error(
        "Decoding Failed (Allowed PDU session status): Length (%d) is not in range of min: 2 and max 32 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack(psi_7, 1));
  HANDLE_CODE(bref.unpack(psi_6, 1));
  HANDLE_CODE(bref.unpack(psi_5, 1));
  HANDLE_CODE(bref.unpack(psi_4, 1));
  HANDLE_CODE(bref.unpack(psi_3, 1));
  HANDLE_CODE(bref.unpack(psi_2, 1));
  HANDLE_CODE(bref.unpack(psi_1, 1));
  HANDLE_CODE(bref.unpack(psi_0, 1));
  HANDLE_CODE(bref.unpack(psi_15, 1));
  HANDLE_CODE(bref.unpack(psi_14, 1));
  HANDLE_CODE(bref.unpack(psi_13, 1));
  HANDLE_CODE(bref.unpack(psi_12, 1));
  HANDLE_CODE(bref.unpack(psi_11, 1));
  HANDLE_CODE(bref.unpack(psi_10, 1));
  HANDLE_CODE(bref.unpack(psi_9, 1));
  HANDLE_CODE(bref.unpack(psi_8, 1));

  if (length > 2) {
    bref.advance_bits((length - 2) * 8);
  }
  return SRSASN_SUCCESS;
}

// IE: UE usage setting
// Reference: 9.11.3.55
SRSASN_CODE ue_usage_setting_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // 7 Spare bits
  HANDLE_CODE(bref.pack(0x0, 7));
  HANDLE_CODE(ue_usage_setting.pack(bref));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 1) {
    asn1::log_error("Encoding Failed (UE usage setting): Packed length (%d) does not equal expected length 1", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: UE usage setting
// Reference: 9.11.3.55
SRSASN_CODE ue_usage_setting_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 1) {
    asn1::log_error("Decoding Failed (UE usage setting): Length (%d) does not equal expected length 1", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  // 7 Spare bits
  bref.advance_bits(7);
  HANDLE_CODE(ue_usage_setting.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: 5GS DRX parameters
// Reference: 9.11.3.2A
SRSASN_CODE drx_parameters_5gs_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // 4 Spare bits
  HANDLE_CODE(bref.pack(0x0, 4));
  HANDLE_CODE(drx_value.pack(bref));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 1) {
    asn1::log_error("Encoding Failed (5GS DRX parameters): Packed length (%d) does not equal expected length 1",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: 5GS DRX parameters
// Reference: 9.11.3.2A
SRSASN_CODE drx_parameters_5gs_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 1) {
    asn1::log_error("Decoding Failed (5GS DRX parameters): Length (%d) does not equal expected length 1", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  // 4 Spare bits
  bref.advance_bits(4);
  HANDLE_CODE(drx_value.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: EPS NAS message container
// Reference: 9.11.3.24
SRSASN_CODE eps_nas_message_container_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  HANDLE_CODE(bref.pack_bytes(eps_nas_message_container.data(), eps_nas_message_container.size()));
  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: EPS NAS message container
// Reference: 9.11.3.24
SRSASN_CODE eps_nas_message_container_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  eps_nas_message_container.resize(length);
  HANDLE_CODE(bref.unpack_bytes(eps_nas_message_container.data(), length));
  return SRSASN_SUCCESS;
}

// IE: LADN indication
// Reference: 9.11.3.29
SRSASN_CODE ladn_indication_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  for (auto& dnn : ladn_dnn_values) {
    HANDLE_CODE(dnn.pack(bref));
  }

  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  // min. length of 0 not checked: uint underflow
  if (length > 808) {
    asn1::log_error("Encoding Failed (LADN indication): Packed length (%d) is not in range of max 808 bytes", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: LADN indication
// Reference: 9.11.3.29
SRSASN_CODE ladn_indication_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  // min. length of 0 not checked: uint underflow
  if (length > 808) {
    asn1::log_error("Decoding Failed (LADN indication): Length (%d) is not in range of max 808 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  asn1::cbit_ref bref_start = bref;
  while (floorl(bref.distance(bref_start) / 8) < length) {
    dnn_t dnn;
    dnn.unpack(bref);
    ladn_dnn_values.push_back(dnn);
  }
  return SRSASN_SUCCESS;
}

// IE: Payload container type
// Reference: 9.11.3.40
SRSASN_CODE payload_container_type_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(payload_container_type.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: Payload container type
// Reference: 9.11.3.40
SRSASN_CODE payload_container_type_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(payload_container_type.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: Payload container
// Reference: 9.11.3.39
SRSASN_CODE payload_container_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  HANDLE_CODE(bref.pack_bytes(payload_container_contents.data(), payload_container_contents.size()));
  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  // max. length of 65535 not checked: uint overflow
  if (length < 1) {
    asn1::log_error("Encoding Failed (Payload container): Packed length (%d) is not in range of min: 1 bytes", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: Payload container
// Reference: 9.11.3.39
SRSASN_CODE payload_container_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  // max. length of 65535 not checked: uint overflow
  if (length < 1) {
    asn1::log_error("Decoding Failed (Payload container): Length (%d) is not in range of min: 1 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  payload_container_contents.resize(length);
  HANDLE_CODE(bref.unpack_bytes(payload_container_contents.data(), length));
  return SRSASN_SUCCESS;
}

// IE: Network slicing indication
// Reference: 9.11.3.36
SRSASN_CODE network_slicing_indication_t::pack(asn1::bit_ref& bref)
{
  // 2 Spare bits
  HANDLE_CODE(bref.pack(0x0, 2));
  HANDLE_CODE(bref.pack(nssci, 1));
  HANDLE_CODE(bref.pack(dcni, 1));
  return SRSASN_SUCCESS;
}

// IE: Network slicing indication
// Reference: 9.11.3.36
SRSASN_CODE network_slicing_indication_t::unpack(asn1::cbit_ref& bref)
{
  // 2 Spare bits
  bref.advance_bits(2);
  HANDLE_CODE(bref.unpack(nssci, 1));
  HANDLE_CODE(bref.unpack(dcni, 1));
  return SRSASN_SUCCESS;
}

// IE: 5GS update type
// Reference: 9.11.3.9A
SRSASN_CODE update_type_5gs_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // 2 Spare bits
  HANDLE_CODE(bref.pack(0x0, 2));
  HANDLE_CODE(pnb_eps_c_io_t.pack(bref));
  HANDLE_CODE(pnb_5gs_c_io_t.pack(bref));
  HANDLE_CODE(ng_ran_rcu.pack(bref));
  HANDLE_CODE(sms_requested.pack(bref));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: 5GS update type
// Reference: 9.11.3.9A
SRSASN_CODE update_type_5gs_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8)); // 2 Spare bits
  bref.advance_bits(2);
  HANDLE_CODE(pnb_eps_c_io_t.unpack(bref));
  HANDLE_CODE(pnb_5gs_c_io_t.unpack(bref));
  HANDLE_CODE(ng_ran_rcu.unpack(bref));
  HANDLE_CODE(sms_requested.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: Mobile station classmark 2
// Reference: 9.11.3.31C
SRSASN_CODE mobile_station_classmark_2_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // TODO proper packing

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 3) {
    asn1::log_error("Encoding Failed (Mobile station classmark 2): Packed length (%d) does not equal expected length 3",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Mobile station classmark 2
// Reference: 9.11.3.31C
SRSASN_CODE mobile_station_classmark_2_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 3) {
    asn1::log_error("Decoding Failed (Mobile station classmark 2): Length (%d) does not equal expected length 3",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: Supported codec list
// Reference: 9.11.3.51A
SRSASN_CODE supported_codec_list_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // TODO proper packing

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 3) {
    asn1::log_error("Encoding Failed (Supported codec list): Packed length (%d) is not in range of min: 3 bytes",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Supported codec list
// Reference: 9.11.3.51A
SRSASN_CODE supported_codec_list_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 3) {
    asn1::log_error("Decoding Failed (Supported codec list): Length (%d) is not in range of min: 3 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: message container
// Reference: 9.11.3.33
SRSASN_CODE message_container_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  HANDLE_CODE(bref.pack_bytes(nas_message_container.data(), nas_message_container.size()));
  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  if (length < 1 || length > 65532) {
    asn1::log_error(
        "Encoding Failed (message container): Packed length (%d) is not in range of min: 1 and max 65532 bytes",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: message container
// Reference: 9.11.3.33
SRSASN_CODE message_container_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  if (length < 1 || length > 65532) {
    asn1::log_error("Decoding Failed (message container): Length (%d) is not in range of min: 1 and max 65532 bytes",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  nas_message_container.resize(length);
  HANDLE_CODE(bref.unpack_bytes(nas_message_container.data(), length));
  return SRSASN_SUCCESS;
}

// IE: EPS bearer context status
// Reference: 9.11.3.23A
SRSASN_CODE eps_bearer_context_status_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack(ebi_7, 1));
  HANDLE_CODE(bref.pack(ebi_6, 1));
  HANDLE_CODE(bref.pack(ebi_5, 1));
  HANDLE_CODE(bref.pack(ebi_4, 1));
  HANDLE_CODE(bref.pack(ebi_3, 1));
  HANDLE_CODE(bref.pack(ebi_2, 1));
  HANDLE_CODE(bref.pack(ebi_1, 1));
  HANDLE_CODE(bref.pack(ebi_0, 1));
  HANDLE_CODE(bref.pack(ebi_15, 1));
  HANDLE_CODE(bref.pack(ebi_14, 1));
  HANDLE_CODE(bref.pack(ebi_13, 1));
  HANDLE_CODE(bref.pack(ebi_12, 1));
  HANDLE_CODE(bref.pack(ebi_11, 1));
  HANDLE_CODE(bref.pack(ebi_10, 1));
  HANDLE_CODE(bref.pack(ebi_9, 1));
  HANDLE_CODE(bref.pack(ebi_8, 1));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 2) {
    asn1::log_error("Encoding Failed (EPS bearer context status): Packed length (%d) does not equal expected length 2",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: EPS bearer context status
// Reference: 9.11.3.23A
SRSASN_CODE eps_bearer_context_status_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 2) {
    asn1::log_error("Decoding Failed (EPS bearer context status): Length (%d) does not equal expected length 2",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack(ebi_7, 1));
  HANDLE_CODE(bref.unpack(ebi_6, 1));
  HANDLE_CODE(bref.unpack(ebi_5, 1));
  HANDLE_CODE(bref.unpack(ebi_4, 1));
  HANDLE_CODE(bref.unpack(ebi_3, 1));
  HANDLE_CODE(bref.unpack(ebi_2, 1));
  HANDLE_CODE(bref.unpack(ebi_1, 1));
  HANDLE_CODE(bref.unpack(ebi_0, 1));
  HANDLE_CODE(bref.unpack(ebi_15, 1));
  HANDLE_CODE(bref.unpack(ebi_14, 1));
  HANDLE_CODE(bref.unpack(ebi_13, 1));
  HANDLE_CODE(bref.unpack(ebi_12, 1));
  HANDLE_CODE(bref.unpack(ebi_11, 1));
  HANDLE_CODE(bref.unpack(ebi_10, 1));
  HANDLE_CODE(bref.unpack(ebi_9, 1));
  HANDLE_CODE(bref.unpack(ebi_8, 1));
  return SRSASN_SUCCESS;
}

// IE: Extended DRX parameters
// Reference: 9.11.3.26A
SRSASN_CODE extended_drx_parameters_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(paging__time__window.pack(bref));
  HANDLE_CODE(e_drx_value.pack(bref));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 1) {
    asn1::log_error("Encoding Failed (Extended DRX parameters): Packed length (%d) does not equal expected length 1",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Extended DRX parameters
// Reference: 9.11.3.26A
SRSASN_CODE extended_drx_parameters_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 1) {
    asn1::log_error("Decoding Failed (Extended DRX parameters): Length (%d) does not equal expected length 1", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(paging__time__window.unpack(bref));
  HANDLE_CODE(e_drx_value.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: GPRS timer 3
// Reference: 9.11.2.5
SRSASN_CODE gprs_timer_3_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(unit.pack(bref));
  HANDLE_CODE(bref.pack(timer_value, 5));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 1) {
    asn1::log_error("Encoding Failed (GPRS timer 3): Packed length (%d) does not equal expected length 1", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: GPRS timer 3
// Reference: 9.11.2.5
SRSASN_CODE gprs_timer_3_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 1) {
    asn1::log_error("Decoding Failed (GPRS timer 3): Length (%d) does not equal expected length 1", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(unit.unpack(bref));
  HANDLE_CODE(bref.unpack(timer_value, 5));
  return SRSASN_SUCCESS;
}

// IE: UE radio capability ID
// Reference: 9.11.3.68
SRSASN_CODE ue_radio_capability_id_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack_bytes(ue_radio_capability_id.data(), ue_radio_capability_id.size()));
  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: UE radio capability ID
// Reference: 9.11.3.68
SRSASN_CODE ue_radio_capability_id_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  ue_radio_capability_id.resize(length);
  HANDLE_CODE(bref.unpack_bytes(ue_radio_capability_id.data(), length));
  return SRSASN_SUCCESS;
}

// IE: Mapped NSSAI
// Reference: 9.11.3.31B
SRSASN_CODE mapped_nssai_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // TODO proper packing

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 2 || length > 40) {
    asn1::log_error("Encoding Failed (Mapped NSSAI): Packed length (%d) is not in range of min: 2 and max 40 bytes",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Mapped NSSAI
// Reference: 9.11.3.31B
SRSASN_CODE mapped_nssai_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 2 || length > 40) {
    asn1::log_error("Decoding Failed (Mapped NSSAI): Length (%d) is not in range of min: 2 and max 40 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: Additional information requested
// Reference: 9.11.3.12A
SRSASN_CODE additional_information_requested_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // 7 Spare bits
  HANDLE_CODE(bref.pack(0x0, 7));
  HANDLE_CODE(bref.pack(cipher_key, 1));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Additional information requested
// Reference: 9.11.3.12A
SRSASN_CODE additional_information_requested_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8)); // 7 Spare bits
  bref.advance_bits(7);
  HANDLE_CODE(bref.unpack(cipher_key, 1));
  return SRSASN_SUCCESS;
}

// IE: WUS assistance information
// Reference: 9.11.3.71
SRSASN_CODE wus_assistance_information_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // TODO proper packing

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 1) {
    asn1::log_error("Encoding Failed (WUS assistance information): Packed length (%d) is not in range of min: 1 bytes",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: WUS assistance information
// Reference: 9.11.3.71
SRSASN_CODE wus_assistance_information_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 1) {
    asn1::log_error("Decoding Failed (WUS assistance information): Length (%d) is not in range of min: 1 bytes",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: N5GC indication
// Reference: 9.11.3.72
SRSASN_CODE n5gc_indication_t::pack(asn1::bit_ref& bref)
{
  // 3 Spare bits
  HANDLE_CODE(bref.pack(0x0, 3));
  HANDLE_CODE(bref.pack(n5gcreg, 1));
  return SRSASN_SUCCESS;
}

// IE: N5GC indication
// Reference: 9.11.3.72
SRSASN_CODE n5gc_indication_t::unpack(asn1::cbit_ref& bref)
{
  // 3 Spare bits
  bref.advance_bits(3);
  HANDLE_CODE(bref.unpack(n5gcreg, 1));
  return SRSASN_SUCCESS;
}

// IE: NB-N1 mode DRX parameters
// Reference: 9.11.3.73
SRSASN_CODE nb_n1_mode_drx_parameters_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // 4 Spare bits
  HANDLE_CODE(bref.pack(0x0, 4));
  HANDLE_CODE(nb_n1_mode_drx_value.pack(bref));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 1) {
    asn1::log_error("Encoding Failed (NB-N1 mode DRX parameters): Packed length (%d) does not equal expected length 1",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: NB-N1 mode DRX parameters
// Reference: 9.11.3.73
SRSASN_CODE nb_n1_mode_drx_parameters_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 1) {
    asn1::log_error("Decoding Failed (NB-N1 mode DRX parameters): Length (%d) does not equal expected length 1",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  // 4 Spare bits
  bref.advance_bits(4);
  HANDLE_CODE(nb_n1_mode_drx_value.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: 5GS registration result
// Reference: 9.11.3.6
SRSASN_CODE registration_result_5gs_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // 2 Spare bits
  HANDLE_CODE(bref.pack(0x0, 2));
  HANDLE_CODE(emergency_registered.pack(bref));
  HANDLE_CODE(nssaa_to_be_performed.pack(bref));
  HANDLE_CODE(sms_allowed.pack(bref));
  HANDLE_CODE(registration_result.pack(bref));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 1) {
    asn1::log_error("Encoding Failed (5GS registration result): Packed length (%d) does not equal expected length 1",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: 5GS registration result
// Reference: 9.11.3.6
SRSASN_CODE registration_result_5gs_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 1) {
    asn1::log_error("Decoding Failed (5GS registration result): Length (%d) does not equal expected length 1", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  // 2 Spare bits
  bref.advance_bits(2);
  HANDLE_CODE(emergency_registered.unpack(bref));
  HANDLE_CODE(nssaa_to_be_performed.unpack(bref));
  HANDLE_CODE(sms_allowed.unpack(bref));
  HANDLE_CODE(registration_result.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: PLMN list
// Reference: 9.11.3.45
SRSASN_CODE plmn_list_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // TODO proper packing

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: PLMN list
// Reference: 9.11.3.45
SRSASN_CODE plmn_list_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: 5GS tracking area identity list
// Reference: 9.11.3.9
SRSASN_CODE tracking_area_identity_list_5gs_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // TODO proper packing

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 7) {
    asn1::log_error(
        "Encoding Failed (5GS tracking area identity list): Packed length (%d) does not equal expected length 7",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: 5GS tracking area identity list
// Reference: 9.11.3.9
SRSASN_CODE tracking_area_identity_list_5gs_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 7) {
    asn1::log_error("Decoding Failed (5GS tracking area identity list): Length (%d) does not equal expected length 7",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: Rejected NSSAI
// Reference: 9.11.3.46
SRSASN_CODE rejected_nssai_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // TODO proper packing

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Rejected NSSAI
// Reference: 9.11.3.46
SRSASN_CODE rejected_nssai_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: 5GS network feature support
// Reference: 9.11.3.5
SRSASN_CODE network_feature_support_5gs_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // TODO proper packing

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 1 || length > 3) {
    asn1::log_error(
        "Encoding Failed (5GS network feature support): Packed length (%d) is not in range of min: 1 and max 3 bytes",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: 5GS network feature support
// Reference: 9.11.3.5
SRSASN_CODE network_feature_support_5gs_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 1 || length > 3) {
    asn1::log_error(
        "Decoding Failed (5GS network feature support): Length (%d) is not in range of min: 1 and max 3 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: PDU session reactivation result
// Reference: 9.11.3.42
SRSASN_CODE pdu_session_reactivation_result_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack(psi_7, 1));
  HANDLE_CODE(bref.pack(psi_6, 1));
  HANDLE_CODE(bref.pack(psi_5, 1));
  HANDLE_CODE(bref.pack(psi_4, 1));
  HANDLE_CODE(bref.pack(psi_3, 1));
  HANDLE_CODE(bref.pack(psi_2, 1));
  HANDLE_CODE(bref.pack(psi_1, 1));
  HANDLE_CODE(bref.pack(psi_0, 1));
  HANDLE_CODE(bref.pack(psi_15, 1));
  HANDLE_CODE(bref.pack(psi_14, 1));
  HANDLE_CODE(bref.pack(psi_13, 1));
  HANDLE_CODE(bref.pack(psi_12, 1));
  HANDLE_CODE(bref.pack(psi_11, 1));
  HANDLE_CODE(bref.pack(psi_10, 1));
  HANDLE_CODE(bref.pack(psi_9, 1));
  HANDLE_CODE(bref.pack(psi_8, 1));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 2 || length > 32) {
    asn1::log_error("Encoding Failed (PDU session reactivation result): Packed length (%d) is not in range of min: 2 "
                    "and max 32 bytes",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: PDU session reactivation result
// Reference: 9.11.3.42
SRSASN_CODE pdu_session_reactivation_result_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 2 || length > 32) {
    asn1::log_error(
        "Decoding Failed (PDU session reactivation result): Length (%d) is not in range of min: 2 and max 32 bytes",
        length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  HANDLE_CODE(bref.unpack(psi_7, 1));
  HANDLE_CODE(bref.unpack(psi_6, 1));
  HANDLE_CODE(bref.unpack(psi_5, 1));
  HANDLE_CODE(bref.unpack(psi_4, 1));
  HANDLE_CODE(bref.unpack(psi_3, 1));
  HANDLE_CODE(bref.unpack(psi_2, 1));
  HANDLE_CODE(bref.unpack(psi_1, 1));
  HANDLE_CODE(bref.unpack(psi_0, 1));

  HANDLE_CODE(bref.unpack(psi_15, 1));
  HANDLE_CODE(bref.unpack(psi_14, 1));
  HANDLE_CODE(bref.unpack(psi_13, 1));
  HANDLE_CODE(bref.unpack(psi_12, 1));
  HANDLE_CODE(bref.unpack(psi_11, 1));
  HANDLE_CODE(bref.unpack(psi_10, 1));
  HANDLE_CODE(bref.unpack(psi_9, 1));
  HANDLE_CODE(bref.unpack(psi_8, 1));

  if (length > 2) {
    HANDLE_CODE(bref.advance_bits((length - 2) * 8));
  }
  return SRSASN_SUCCESS;
}

// IE: PDU session reactivation result error cause
// Reference: 9.11.3.43
SRSASN_CODE pdu_session_reactivation_result_error_cause_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  // TODO proper packing

  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  if (length < 2 || length > 512) {
    asn1::log_error("Encoding Failed (PDU session reactivation result error cause): Packed length (%d) is not in range "
                    "of min: 2 and max 512 bytes",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: PDU session reactivation result error cause
// Reference: 9.11.3.43
SRSASN_CODE pdu_session_reactivation_result_error_cause_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  if (length < 2 || length > 512) {
    asn1::log_error("Decoding Failed (PDU session reactivation result error cause): Length (%d) is not in range of "
                    "min: 2 and max 512 bytes",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: LADN information
// Reference: 9.11.3.30
SRSASN_CODE ladn_information_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  // TODO proper packing

  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  // min. length of 0 not checked: uint underflow
  if (length > 1712) {
    asn1::log_error("Encoding Failed (LADN information): Packed length (%d) is not in range of max 1712 bytes", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: LADN information
// Reference: 9.11.3.30
SRSASN_CODE ladn_information_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  // min. length of 0 not checked: uint underflow
  if (length > 1712) {
    asn1::log_error("Decoding Failed (LADN information): Length (%d) is not in range of max 1712 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: Service area list
// Reference: 9.11.3.49
SRSASN_CODE service_area_list_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // TODO proper packing

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Service area list
// Reference: 9.11.3.49
SRSASN_CODE service_area_list_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: GPRS timer 2
// Reference: 9.11.2.4
SRSASN_CODE gprs_timer_2_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack(timer_value, 8));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 1) {
    asn1::log_error("Encoding Failed (GPRS timer 2): Packed length (%d) does not equal expected length 1", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: GPRS timer 2
// Reference: 9.11.2.4
SRSASN_CODE gprs_timer_2_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 1) {
    asn1::log_error("Decoding Failed (GPRS timer 2): Length (%d) does not equal expected length 1", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack(timer_value, 8));
  return SRSASN_SUCCESS;
}

// IE: Emergency number list
// Reference: 9.11.3.23
SRSASN_CODE emergency_number_list_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // TODO proper packing

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 3 || length > 48) {
    asn1::log_error(
        "Encoding Failed (Emergency number list): Packed length (%d) is not in range of min: 3 and max 48 bytes",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Emergency number list
// Reference: 9.11.3.23
SRSASN_CODE emergency_number_list_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 3 || length > 48) {
    asn1::log_error("Decoding Failed (Emergency number list): Length (%d) is not in range of min: 3 and max 48 bytes",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: Extended emergency number list
// Reference: 9.11.3.26
SRSASN_CODE extended_emergency_number_list_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  // TODO proper packing

  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  // max. length of 65535 not checked: uint overflow
  if (length < 4) {
    asn1::log_error(
        "Encoding Failed (Extended emergency number list): Packed length (%d) is not in range of min: 4 bytes", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: Extended emergency number list
// Reference: 9.11.3.26
SRSASN_CODE extended_emergency_number_list_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  // max. length of 65535 not checked: uint overflow
  if (length < 4) {
    asn1::log_error("Decoding Failed (Extended emergency number list): Length (%d) is not in range of min: 4 bytes",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: SOR transparent container
// Reference: 9.11.3.51
SRSASN_CODE sor_transparent_container_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  // TODO proper packing

  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  if (length < 17) {
    asn1::log_error("Encoding Failed (SOR transparent container): Packed length (%d) is not in range of min: 17 bytes",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: SOR transparent container
// Reference: 9.11.3.51
SRSASN_CODE sor_transparent_container_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  if (length < 17) {
    asn1::log_error("Decoding Failed (SOR transparent container): Length (%d) is not in range of min: 17 bytes",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: EAP message
// Reference: 9.11.2.2
SRSASN_CODE eap_message_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  HANDLE_CODE(bref.pack_bytes(eap_message.data(), eap_message.size()));
  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  if (length < 4 || length > 1500) {
    asn1::log_error("Encoding Failed (EAP message): Packed length (%d) is not in range of min: 4 and max 1500 bytes",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: EAP message
// Reference: 9.11.2.2
SRSASN_CODE eap_message_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  if (length < 4 || length > 1500) {
    asn1::log_error("Decoding Failed (EAP message): Length (%d) is not in range of min: 4 and max 1500 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  eap_message.resize(length);
  HANDLE_CODE(bref.unpack_bytes(eap_message.data(), length));
  return SRSASN_SUCCESS;
}

// IE: NSSAI inclusion mode
// Reference: 9.11.3.37A
SRSASN_CODE nssai_inclusion_mode_t::pack(asn1::bit_ref& bref)
{
  // 2 Spare bits
  HANDLE_CODE(bref.pack(0x0, 2));
  HANDLE_CODE(nssai_inclusion_mode.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: NSSAI inclusion mode
// Reference: 9.11.3.37A
SRSASN_CODE nssai_inclusion_mode_t::unpack(asn1::cbit_ref& bref)
{
  // 2 Spare bits
  bref.advance_bits(2);
  HANDLE_CODE(nssai_inclusion_mode.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: Operator-defined access category definitions
// Reference: 9.11.3.38
SRSASN_CODE operator_defined_access_category_definitions_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  // TODO proper packing

  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: Operator-defined access category definitions
// Reference: 9.11.3.38
SRSASN_CODE operator_defined_access_category_definitions_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: Non-3GPP NW provided policies
// Reference: 9.11.3.36A
SRSASN_CODE non_3_gpp_nw_provided_policies_t::pack(asn1::bit_ref& bref)
{
  // 3 Spare bits
  HANDLE_CODE(bref.pack(0x0, 3));
  HANDLE_CODE(bref.pack(n3_en, 1));
  return SRSASN_SUCCESS;
}

// IE: Non-3GPP NW provided policies
// Reference: 9.11.3.36A
SRSASN_CODE non_3_gpp_nw_provided_policies_t::unpack(asn1::cbit_ref& bref)
{
  // 3 Spare bits
  bref.advance_bits(3);
  HANDLE_CODE(bref.unpack(n3_en, 1));
  return SRSASN_SUCCESS;
}

// IE: UE radio capability ID deletion indication
// Reference: 9.11.3.69
SRSASN_CODE ue_radio_capability_id_deletion_indication_t::pack(asn1::bit_ref& bref)
{
  // 1 Spare bits
  HANDLE_CODE(bref.pack(0x0, 1));
  HANDLE_CODE(deletion_request.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: UE radio capability ID deletion indication
// Reference: 9.11.3.69
SRSASN_CODE ue_radio_capability_id_deletion_indication_t::unpack(asn1::cbit_ref& bref)
{
  // 1 Spare bits
  bref.advance_bits(1);
  HANDLE_CODE(deletion_request.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: Ciphering key data
// Reference: 9.11.3.18C
SRSASN_CODE ciphering_key_data_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  // TODO proper packing

  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  if (length < 31 || length > 2672) {
    asn1::log_error(
        "Encoding Failed (Ciphering key data): Packed length (%d) is not in range of min: 31 and max 2672 bytes",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: Ciphering key data
// Reference: 9.11.3.18C
SRSASN_CODE ciphering_key_data_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  if (length < 31 || length > 2672) {
    asn1::log_error("Decoding Failed (Ciphering key data): Length (%d) is not in range of min: 31 and max 2672 bytes",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: CAG information list
// Reference: 9.11.3.18A
SRSASN_CODE cag_information_list_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  // TODO proper packing

  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: CAG information list
// Reference: 9.11.3.18A
SRSASN_CODE cag_information_list_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: Truncated 5G-S-TMSI configuration
// Reference: 9.11.3.70
SRSASN_CODE truncated_5g_s_tmsi_configuration_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack(truncated_amf__set_id_value, 4));
  HANDLE_CODE(bref.pack(truncated_amf__pointer_value, 4));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 1) {
    asn1::log_error(
        "Encoding Failed (Truncated 5G-S-TMSI configuration): Packed length (%d) does not equal expected length 1",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Truncated 5G-S-TMSI configuration
// Reference: 9.11.3.70
SRSASN_CODE truncated_5g_s_tmsi_configuration_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 1) {
    asn1::log_error("Decoding Failed (Truncated 5G-S-TMSI configuration): Length (%d) does not equal expected length 1",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack(truncated_amf__set_id_value, 4));
  HANDLE_CODE(bref.unpack(truncated_amf__pointer_value, 4));
  return SRSASN_SUCCESS;
}

// IE: 5GMM cause
// Reference: 9.11.3.2
SRSASN_CODE cause_5gmm_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(cause_5gmm.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: 5GMM cause
// Reference: 9.11.3.2
SRSASN_CODE cause_5gmm_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(cause_5gmm.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: De-registration type
// Reference: 9.11.3.20
SRSASN_CODE de_registration_type_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(switch_off.pack(bref));
  HANDLE_CODE(re_registration_required.pack(bref));
  HANDLE_CODE(access_type.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: De-registration type
// Reference: 9.11.3.20
SRSASN_CODE de_registration_type_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(switch_off.unpack(bref));
  HANDLE_CODE(re_registration_required.unpack(bref));
  HANDLE_CODE(access_type.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: Spare half octet
// Reference: 9.5
SRSASN_CODE spare_half_octet_t::pack(asn1::bit_ref& bref)
{
  // 4 Spare bits
  HANDLE_CODE(bref.pack(0x0, 4));
  return SRSASN_SUCCESS;
}

// IE: Spare half octet
// Reference: 9.5
SRSASN_CODE spare_half_octet_t::unpack(asn1::cbit_ref& bref)
{
  // 4 Spare bits
  bref.advance_bits(4);
  return SRSASN_SUCCESS;
}

// IE: Service type
// Reference: 9.11.3.50
SRSASN_CODE service_type_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(service_type_value.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: Service type
// Reference: 9.11.3.50
SRSASN_CODE service_type_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(service_type_value.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: Configuration update indication
// Reference: 9.11.3.18
SRSASN_CODE configuration_update_indication_t::pack(asn1::bit_ref& bref)
{
  // 1 Spare bits
  HANDLE_CODE(bref.pack(0x0, 1));
  HANDLE_CODE(control_plane_service_type_value.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: Configuration update indication
// Reference: 9.11.3.18
SRSASN_CODE configuration_update_indication_t::unpack(asn1::cbit_ref& bref)
{
  // 1 Spare bits
  bref.advance_bits(1);
  HANDLE_CODE(control_plane_service_type_value.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: Network name
// Reference: 9.11.3.35
SRSASN_CODE network_name_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // TODO proper packing

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Network name
// Reference: 9.11.3.35
SRSASN_CODE network_name_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: Time zone
// Reference: 9.11.3.52
SRSASN_CODE time_zone_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(bref.pack(year, 8));
  HANDLE_CODE(bref.pack(month, 8));
  HANDLE_CODE(bref.pack(day, 8));
  HANDLE_CODE(bref.pack(hour, 8));
  HANDLE_CODE(bref.pack(second, 8));
  HANDLE_CODE(bref.pack(time_zone, 8));
  return SRSASN_SUCCESS;
}

// IE: Time zone
// Reference: 9.11.3.52
SRSASN_CODE time_zone_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(year, 8));
  HANDLE_CODE(bref.unpack(month, 8));
  HANDLE_CODE(bref.unpack(day, 8));
  HANDLE_CODE(bref.unpack(hour, 8));
  HANDLE_CODE(bref.unpack(second, 8));
  HANDLE_CODE(bref.unpack(time_zone, 8));
  return SRSASN_SUCCESS;
}

// IE: Time zone and time
// Reference: 9.11.3.53
SRSASN_CODE time_zone_and_time_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(bref.pack(time_zone, 8));
  return SRSASN_SUCCESS;
}

// IE: Time zone and time
// Reference: 9.11.3.53
SRSASN_CODE time_zone_and_time_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(time_zone, 8));
  return SRSASN_SUCCESS;
}

// IE: Daylight saving time
// Reference: 9.11.3.19
SRSASN_CODE daylight_saving_time_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(value.pack(bref));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 1) {
    asn1::log_error("Encoding Failed (Daylight saving time): Packed length (%d) does not equal expected length 1",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Daylight saving time
// Reference: 9.11.3.19
SRSASN_CODE daylight_saving_time_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 1) {
    asn1::log_error("Decoding Failed (Daylight saving time): Length (%d) does not equal expected length 1", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(value.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: SMS indication
// Reference: 9.11.3.50A
SRSASN_CODE sms_indication_t::pack(asn1::bit_ref& bref)
{
  // 3 Spare bits
  HANDLE_CODE(bref.pack(0x0, 3));
  HANDLE_CODE(bref.pack(sms_availability_indication, 1));
  return SRSASN_SUCCESS;
}

// IE: SMS indication
// Reference: 9.11.3.50A
SRSASN_CODE sms_indication_t::unpack(asn1::cbit_ref& bref)
{
  // 3 Spare bits
  bref.advance_bits(3);
  HANDLE_CODE(bref.unpack(sms_availability_indication, 1));
  return SRSASN_SUCCESS;
}

// IE: Additional configuration indication
// Reference: 9.11.3.74
SRSASN_CODE additional_configuration_indication_t::pack(asn1::bit_ref& bref)
{
  // 3 Spare bits
  HANDLE_CODE(bref.pack(0x0, 3));
  HANDLE_CODE(scmr.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: Additional configuration indication
// Reference: 9.11.3.74
SRSASN_CODE additional_configuration_indication_t::unpack(asn1::cbit_ref& bref)
{
  // 3 Spare bits
  bref.advance_bits(3);
  HANDLE_CODE(scmr.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: ABBA
// Reference: 9.11.3.10
SRSASN_CODE abba_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack_bytes(abba_contents.data(), abba_contents.size()));
  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 2) {
    asn1::log_error("Encoding Failed (ABBA): Packed length (%d) is not in range of min: 2 bytes", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: ABBA
// Reference: 9.11.3.10
SRSASN_CODE abba_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 2) {
    asn1::log_error("Decoding Failed (ABBA): Length (%d) is not in range of min: 2 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  abba_contents.resize(length);
  HANDLE_CODE(bref.unpack_bytes(abba_contents.data(), length));
  return SRSASN_SUCCESS;
}

// IE: Authentication parameter RAND
// Reference: 9.11.3.16
SRSASN_CODE authentication_parameter_rand_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(bref.pack_bytes(rand.data(), 16));
  return SRSASN_SUCCESS;
}

// IE: Authentication parameter RAND
// Reference: 9.11.3.16
SRSASN_CODE authentication_parameter_rand_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack_bytes(rand.data(), 16));
  return SRSASN_SUCCESS;
}

// IE: Authentication parameter AUTN
// Reference: 9.11.3.15
SRSASN_CODE authentication_parameter_autn_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack_bytes(autn.data(), autn.size()));
  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 16) {
    asn1::log_error(
        "Encoding Failed (Authentication parameter AUTN): Packed length (%d) does not equal expected length 16",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Authentication parameter AUTN
// Reference: 9.11.3.15
SRSASN_CODE authentication_parameter_autn_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 16) {
    asn1::log_error("Decoding Failed (Authentication parameter AUTN): Length (%d) does not equal expected length 16",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  autn.resize(length);
  HANDLE_CODE(bref.unpack_bytes(autn.data(), length));
  return SRSASN_SUCCESS;
}

// IE: Authentication response parameter
// Reference: 9.11.3.17
SRSASN_CODE authentication_response_parameter_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack_bytes(res.data(), res.size()));
  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 16) {
    asn1::log_error(
        "Encoding Failed (Authentication response parameter): Packed length (%d) does not equal expected length 16",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Authentication response parameter
// Reference: 9.11.3.17
SRSASN_CODE authentication_response_parameter_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 16) {
    asn1::log_error(
        "Decoding Failed (Authentication response parameter): Length (%d) does not equal expected length 16", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  res.resize(length);
  HANDLE_CODE(bref.unpack_bytes(res.data(), length));
  return SRSASN_SUCCESS;
}

// IE: Authentication failure parameter
// Reference: 9.11.3.14
SRSASN_CODE authentication_failure_parameter_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack_bytes(auth_failure.data(), auth_failure.size()));
  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 14) {
    asn1::log_error(
        "Encoding Failed (Authentication failure parameter): Packed length (%d) does not equal expected length 14",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Authentication failure parameter
// Reference: 9.11.3.14
SRSASN_CODE authentication_failure_parameter_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 14) {
    asn1::log_error("Decoding Failed (Authentication failure parameter): Length (%d) does not equal expected length 14",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  auth_failure.resize(length);
  HANDLE_CODE(bref.unpack_bytes(auth_failure.data(), length));
  return SRSASN_SUCCESS;
}

// IE: 5GS identity type
// Reference: 9.11.3.3
SRSASN_CODE identity_type_5gs_t::pack(asn1::bit_ref& bref)
{
  // 1 Spare bits
  HANDLE_CODE(bref.pack(0x0, 1));
  HANDLE_CODE(type_of_identity.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: 5GS identity type
// Reference: 9.11.3.3
SRSASN_CODE identity_type_5gs_t::unpack(asn1::cbit_ref& bref)
{
  // 1 Spare bits
  bref.advance_bits(1);
  HANDLE_CODE(type_of_identity.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: security algorithms
// Reference: 9.11.3.34
SRSASN_CODE security_algorithms_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(ciphering_algorithm.pack(bref));
  HANDLE_CODE(integrity_protection_algorithm.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: security algorithms
// Reference: 9.11.3.34
SRSASN_CODE security_algorithms_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(ciphering_algorithm.unpack(bref));
  HANDLE_CODE(integrity_protection_algorithm.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: IMEISV request
// Reference: 9.11.3.28
SRSASN_CODE imeisv_request_t::pack(asn1::bit_ref& bref)
{
  // 1 Spare bits
  HANDLE_CODE(bref.pack(0x0, 1));
  HANDLE_CODE(imeisv_request.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: IMEISV request
// Reference: 9.11.3.28
SRSASN_CODE imeisv_request_t::unpack(asn1::cbit_ref& bref)
{
  // 1 Spare bits
  bref.advance_bits(1);
  HANDLE_CODE(imeisv_request.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: EPS NAS security algorithms
// Reference: 9.11.3.25
SRSASN_CODE eps_nas_security_algorithms_t::pack(asn1::bit_ref& bref)
{
  // 1 Spare bits
  HANDLE_CODE(bref.pack(0x0, 1));
  HANDLE_CODE(ciphering_algorithm.pack(bref));

  // 1 Spare bits
  HANDLE_CODE(bref.pack(0x0, 1));
  HANDLE_CODE(integrity_protection_algorithm.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: EPS NAS security algorithms
// Reference: 9.11.3.25
SRSASN_CODE eps_nas_security_algorithms_t::unpack(asn1::cbit_ref& bref)
{
  // 1 Spare bits
  bref.advance_bits(1);
  HANDLE_CODE(ciphering_algorithm.unpack(bref));
  // 1 Spare bits
  bref.advance_bits(1);
  HANDLE_CODE(integrity_protection_algorithm.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: Additional 5G security information
// Reference: 9.11.3.12
SRSASN_CODE additional_5g_security_information_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // 6 Spare bits
  HANDLE_CODE(bref.pack(0x0, 6));
  HANDLE_CODE(bref.pack(rinmr, 1));
  HANDLE_CODE(bref.pack(hdp, 1));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 1) {
    asn1::log_error(
        "Encoding Failed (Additional 5G security information): Packed length (%d) does not equal expected length 1",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Additional 5G security information
// Reference: 9.11.3.12
SRSASN_CODE additional_5g_security_information_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 1) {
    asn1::log_error(
        "Decoding Failed (Additional 5G security information): Length (%d) does not equal expected length 1", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  // 6 Spare bits
  bref.advance_bits(6);
  HANDLE_CODE(bref.unpack(rinmr, 1));
  HANDLE_CODE(bref.unpack(hdp, 1));
  return SRSASN_SUCCESS;
}

// IE: S1 UE security capability
// Reference: 9.11.3.48A
SRSASN_CODE s1_ue_security_capability_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack(eea0, 1));
  HANDLE_CODE(bref.pack(eea1_128, 1));
  HANDLE_CODE(bref.pack(eea2_128, 1));
  HANDLE_CODE(bref.pack(eea3_128, 1));
  HANDLE_CODE(bref.pack(eea4, 1));
  HANDLE_CODE(bref.pack(eea5, 1));
  HANDLE_CODE(bref.pack(eea6, 1));
  HANDLE_CODE(bref.pack(eea7, 1));
  HANDLE_CODE(bref.pack(eia0, 1));
  HANDLE_CODE(bref.pack(eia1_128, 1));
  HANDLE_CODE(bref.pack(eia2_128, 1));
  HANDLE_CODE(bref.pack(eia3_128, 1));
  HANDLE_CODE(bref.pack(eia4, 1));
  HANDLE_CODE(bref.pack(eia5, 1));
  HANDLE_CODE(bref.pack(eia6, 1));
  HANDLE_CODE(bref.pack(eia7, 1));
  HANDLE_CODE(bref.pack(uea0, 1));
  HANDLE_CODE(bref.pack(uea1, 1));
  HANDLE_CODE(bref.pack(uea2, 1));
  HANDLE_CODE(bref.pack(uea3, 1));
  HANDLE_CODE(bref.pack(uea4, 1));
  HANDLE_CODE(bref.pack(uea5, 1));
  HANDLE_CODE(bref.pack(uea6, 1));
  HANDLE_CODE(bref.pack(uea7, 1));

  // 1 Spare bits
  HANDLE_CODE(bref.pack(0x0, 1));
  HANDLE_CODE(bref.pack(uia1, 1));
  HANDLE_CODE(bref.pack(uia2, 1));
  HANDLE_CODE(bref.pack(uia3, 1));
  HANDLE_CODE(bref.pack(uia4, 1));
  HANDLE_CODE(bref.pack(uia5, 1));
  HANDLE_CODE(bref.pack(uia6, 1));
  HANDLE_CODE(bref.pack(uia7, 1));

  // 1 Spare bits
  HANDLE_CODE(bref.pack(0x0, 1));
  HANDLE_CODE(bref.pack(gea1, 1));
  HANDLE_CODE(bref.pack(gea2, 1));
  HANDLE_CODE(bref.pack(gea3, 1));
  HANDLE_CODE(bref.pack(gea4, 1));
  HANDLE_CODE(bref.pack(gea5, 1));
  HANDLE_CODE(bref.pack(gea6, 1));
  HANDLE_CODE(bref.pack(gea7, 1));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 2 || length > 5) {
    asn1::log_error(
        "Encoding Failed (S1 UE security capability): Packed length (%d) is not in range of min: 2 and max 5 bytes",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: S1 UE security capability
// Reference: 9.11.3.48A
SRSASN_CODE s1_ue_security_capability_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 2 || length > 5) {
    asn1::log_error(
        "Decoding Failed (S1 UE security capability): Length (%d) is not in range of min: 2 and max 5 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack(eea0, 1));
  HANDLE_CODE(bref.unpack(eea1_128, 1));
  HANDLE_CODE(bref.unpack(eea2_128, 1));
  HANDLE_CODE(bref.unpack(eea3_128, 1));
  HANDLE_CODE(bref.unpack(eea4, 1));
  HANDLE_CODE(bref.unpack(eea5, 1));
  HANDLE_CODE(bref.unpack(eea6, 1));
  HANDLE_CODE(bref.unpack(eea7, 1));

  HANDLE_CODE(bref.unpack(eia0, 1));
  HANDLE_CODE(bref.unpack(eia1_128, 1));
  HANDLE_CODE(bref.unpack(eia2_128, 1));
  HANDLE_CODE(bref.unpack(eia3_128, 1));
  HANDLE_CODE(bref.unpack(eia4, 1));
  HANDLE_CODE(bref.unpack(eia5, 1));
  HANDLE_CODE(bref.unpack(eia6, 1));
  HANDLE_CODE(bref.unpack(eia7, 1));

  if (length < 3) {
    return SRSASN_SUCCESS;
  }

  HANDLE_CODE(bref.unpack(uea0, 1));
  HANDLE_CODE(bref.unpack(uea1, 1));
  HANDLE_CODE(bref.unpack(uea2, 1));
  HANDLE_CODE(bref.unpack(uea3, 1));
  HANDLE_CODE(bref.unpack(uea4, 1));
  HANDLE_CODE(bref.unpack(uea5, 1));
  HANDLE_CODE(bref.unpack(uea6, 1));
  HANDLE_CODE(bref.unpack(uea7, 1));

  if (length < 4) {
    return SRSASN_SUCCESS;
  }
  // 1 Spare bits
  bref.advance_bits(1);
  HANDLE_CODE(bref.unpack(uia1, 1));
  HANDLE_CODE(bref.unpack(uia2, 1));
  HANDLE_CODE(bref.unpack(uia3, 1));
  HANDLE_CODE(bref.unpack(uia4, 1));
  HANDLE_CODE(bref.unpack(uia5, 1));
  HANDLE_CODE(bref.unpack(uia6, 1));
  HANDLE_CODE(bref.unpack(uia7, 1));

  if (length < 5) {
    return SRSASN_SUCCESS;
  }
  // 1 Spare bits
  bref.advance_bits(1);
  HANDLE_CODE(bref.unpack(gea1, 1));
  HANDLE_CODE(bref.unpack(gea2, 1));
  HANDLE_CODE(bref.unpack(gea3, 1));
  HANDLE_CODE(bref.unpack(gea4, 1));
  HANDLE_CODE(bref.unpack(gea5, 1));
  HANDLE_CODE(bref.unpack(gea6, 1));
  HANDLE_CODE(bref.unpack(gea7, 1));
  return SRSASN_SUCCESS;
}

// IE: Access type
// Reference: 9.11.2.1A
SRSASN_CODE access_type_t::pack(asn1::bit_ref& bref)
{
  // 2 Spare bits
  HANDLE_CODE(bref.pack(0x0, 2));
  HANDLE_CODE(access_type_value.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: Access type
// Reference: 9.11.2.1A
SRSASN_CODE access_type_t::unpack(asn1::cbit_ref& bref)
{
  // 2 Spare bits
  bref.advance_bits(2);
  HANDLE_CODE(access_type_value.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: PDU session identity 2
// Reference: 9.11.3.41
SRSASN_CODE pdu_session_identity_2_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(bref.pack(pdu_session_identity_2_value, 8));
  return SRSASN_SUCCESS;
}

// IE: PDU session identity 2
// Reference: 9.11.3.41
SRSASN_CODE pdu_session_identity_2_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdu_session_identity_2_value, 8));
  return SRSASN_SUCCESS;
}

// IE: Request type
// Reference: 9.11.3.47
SRSASN_CODE request_type_t::pack(asn1::bit_ref& bref)
{
  // 1 Spare bits
  HANDLE_CODE(bref.pack(0x0, 1));
  HANDLE_CODE(request_type_value.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: Request type
// Reference: 9.11.3.47
SRSASN_CODE request_type_t::unpack(asn1::cbit_ref& bref)
{
  // 1 Spare bits
  bref.advance_bits(1);
  HANDLE_CODE(request_type_value.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: S-NSSAI
// Reference: 9.11.2.8
SRSASN_CODE s_nssai_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  if (type == s_nssai_t::SST_type_::options::sst) {
    HANDLE_CODE(bref.pack(sst, 8));
  } else if (type == s_nssai_t::SST_type_::options::sst_and_mapped_hplmn_sst) {
    HANDLE_CODE(bref.pack(sst, 8));
    HANDLE_CODE(bref.pack(mapped_hplmn_sst, 8));
  } else if (type == s_nssai_t::SST_type_::options::sst_and_sd) {
    HANDLE_CODE(bref.pack(sst, 8));
    HANDLE_CODE(bref.pack(sd, 24));
  } else if (type == s_nssai_t::SST_type_::options::sst_sd_mapped_hplmn_sst_and_mapped_hplmn_sd) {
    HANDLE_CODE(bref.pack(sst, 8));
    HANDLE_CODE(bref.pack(sd, 24));
    HANDLE_CODE(bref.pack(mapped_hplmn_sst, 8));
    HANDLE_CODE(bref.pack(mapped_hplmn_sd, 24));
  } else {
    asn1::log_error("Not such a length type for s_nssai");
    return SRSASN_ERROR_ENCODE_FAIL;
  }

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 1 || length > 8) {
    asn1::log_error("Encoding Failed (S-NSSAI): Packed length (%d) is not in range of min: 1 and max 8 bytes", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: S-NSSAI
// Reference: 9.11.2.8
SRSASN_CODE s_nssai_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 1 || length > 8) {
    asn1::log_error("Decoding Failed (S-NSSAI): Length (%d) is not in range of min: 1 and max 8 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  if (length == s_nssai_t::SST_type_::options::sst) {
    type = s_nssai_t::SST_type_::options::sst;
    HANDLE_CODE(bref.unpack(sst, 8));
  } else if (length == s_nssai_t::SST_type_::options::sst_and_mapped_hplmn_sst) {
    type = s_nssai_t::SST_type_::options::sst_and_mapped_hplmn_sst;
    HANDLE_CODE(bref.unpack(sst, 8));
    HANDLE_CODE(bref.unpack(mapped_hplmn_sst, 8));
  } else if (length == s_nssai_t::SST_type_::options::sst_and_sd) {
    type = s_nssai_t::SST_type_::options::sst_and_sd;
    HANDLE_CODE(bref.unpack(sst, 8));
    HANDLE_CODE(bref.unpack(sd, 24));
  } else if (length == s_nssai_t::SST_type_::options::sst_sd_mapped_hplmn_sst_and_mapped_hplmn_sd) {
    type = s_nssai_t::SST_type_::options::sst_sd_mapped_hplmn_sst_and_mapped_hplmn_sd;
    HANDLE_CODE(bref.unpack(sst, 8));
    HANDLE_CODE(bref.unpack(sd, 24));
    HANDLE_CODE(bref.unpack(mapped_hplmn_sst, 8));
    HANDLE_CODE(bref.unpack(mapped_hplmn_sd, 24));
  } else {
    asn1::log_error("Not such a type for s_nssai");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// IE: DNN
// Reference: 9.11.2.1B
SRSASN_CODE dnn_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack_bytes(dnn_value.data(), dnn_value.size()));
  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 1 || length > 100) {
    asn1::log_error("Encoding Failed (DNN): Packed length (%d) is not in range of min: 1 and max 100 bytes", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: DNN
// Reference: 9.11.2.1B
SRSASN_CODE dnn_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 1 || length > 100) {
    asn1::log_error("Decoding Failed (DNN): Length (%d) is not in range of min: 1 and max 100 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  dnn_value.resize(length);
  HANDLE_CODE(bref.unpack_bytes(dnn_value.data(), length));
  return SRSASN_SUCCESS;
}

// IE: Additional information
// Reference: 9.11.2.1
SRSASN_CODE additional_information_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack_bytes(additional_information_value.data(), additional_information_value.size()));
  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 1) {
    asn1::log_error("Encoding Failed (Additional information): Packed length (%d) is not in range of min: 1 bytes",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Additional information
// Reference: 9.11.2.1
SRSASN_CODE additional_information_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 1) {
    asn1::log_error("Decoding Failed (Additional information): Length (%d) is not in range of min: 1 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  additional_information_value.resize(length);
  HANDLE_CODE(bref.unpack_bytes(additional_information_value.data(), length));
  return SRSASN_SUCCESS;
}

// IE: MA PDU session information
// Reference: 9.11.3.31A
SRSASN_CODE ma_pdu_session_information_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(ma_pdu_session_information_value.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: MA PDU session information
// Reference: 9.11.3.31A
SRSASN_CODE ma_pdu_session_information_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(ma_pdu_session_information_value.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: Release assistance indication
// Reference: 9.11.3.46A
SRSASN_CODE release_assistance_indication_t::pack(asn1::bit_ref& bref)
{
  // 2 Spare bits
  HANDLE_CODE(bref.pack(0x0, 2));
  HANDLE_CODE(downlink_data_expected.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: Release assistance indication
// Reference: 9.11.3.46A
SRSASN_CODE release_assistance_indication_t::unpack(asn1::cbit_ref& bref)
{
  // 2 Spare bits
  bref.advance_bits(2);
  HANDLE_CODE(downlink_data_expected.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: Integrity protection maximum data rate
// Reference: 9.11.4.7
SRSASN_CODE integrity_protection_maximum_data_rate_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(max_data_rate_upip_uplink.pack(bref));
  HANDLE_CODE(max_data_rate_upip_downlink.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: Integrity protection maximum data rate
// Reference: 9.11.4.7
SRSASN_CODE integrity_protection_maximum_data_rate_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(max_data_rate_upip_uplink.unpack(bref));
  HANDLE_CODE(max_data_rate_upip_downlink.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: PDU session type
// Reference: 9.11.4.11
SRSASN_CODE pdu_session_type_t::pack(asn1::bit_ref& bref)
{
  // 1 Spare bits
  HANDLE_CODE(bref.pack(0x0, 1));
  HANDLE_CODE(pdu_session_type_value.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: PDU session type
// Reference: 9.11.4.11
SRSASN_CODE pdu_session_type_t::unpack(asn1::cbit_ref& bref)
{
  // 1 Spare bits
  bref.advance_bits(1);
  HANDLE_CODE(pdu_session_type_value.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: SSC mode
// Reference: 9.11.4.16
SRSASN_CODE ssc_mode_t::pack(asn1::bit_ref& bref)
{
  // 1 Spare bits
  HANDLE_CODE(bref.pack(0x0, 1));
  HANDLE_CODE(ssc_mode_value.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: SSC mode
// Reference: 9.11.4.16
SRSASN_CODE ssc_mode_t::unpack(asn1::cbit_ref& bref)
{
  // 1 Spare bits
  bref.advance_bits(1);
  HANDLE_CODE(ssc_mode_value.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: 5GSM capability
// Reference: 9.11.4.1
SRSASN_CODE capability_5gsm_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // TODO proper packing

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 1 || length > 13) {
    asn1::log_error("Encoding Failed (5GSM capability): Packed length (%d) is not in range of min: 1 and max 13 bytes",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: 5GSM capability
// Reference: 9.11.4.1
SRSASN_CODE capability_5gsm_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 1 || length > 13) {
    asn1::log_error("Decoding Failed (5GSM capability): Length (%d) is not in range of min: 1 and max 13 bytes",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: Maximum number of supported packet filters
// Reference: 9.11.4.9
SRSASN_CODE maximum_number_of_supported_packet_filters_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(bref.pack(maximum_number_of_supported_packet_filters, 11));

  // 5 Spare bits
  HANDLE_CODE(bref.pack(0x0, 5));
  return SRSASN_SUCCESS;
}

// IE: Maximum number of supported packet filters
// Reference: 9.11.4.9
SRSASN_CODE maximum_number_of_supported_packet_filters_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(maximum_number_of_supported_packet_filters, 11));
  // 5 Spare bits
  bref.advance_bits(5);
  return SRSASN_SUCCESS;
}

// IE: Always-on PDU session requested
// Reference: 9.11.4.4
SRSASN_CODE always_on_pdu_session_requested_t::pack(asn1::bit_ref& bref)
{
  // 3 Spare bits
  HANDLE_CODE(bref.pack(0x0, 3));
  HANDLE_CODE(bref.pack(apsi, 1));
  return SRSASN_SUCCESS;
}

// IE: Always-on PDU session requested
// Reference: 9.11.4.4
SRSASN_CODE always_on_pdu_session_requested_t::unpack(asn1::cbit_ref& bref)
{
  // 3 Spare bits
  bref.advance_bits(3);
  HANDLE_CODE(bref.unpack(apsi, 1));
  return SRSASN_SUCCESS;
}

// IE: SM PDU DN request container
// Reference: 9.11.4.15
SRSASN_CODE sm_pdu_dn_request_container_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack_bytes(dn_specific_identity.data(), dn_specific_identity.size()));
  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 1 || length > 253) {
    asn1::log_error(
        "Encoding Failed (SM PDU DN request container): Packed length (%d) is not in range of min: 1 and max 253 bytes",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: SM PDU DN request container
// Reference: 9.11.4.15
SRSASN_CODE sm_pdu_dn_request_container_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 1 || length > 253) {
    asn1::log_error(
        "Decoding Failed (SM PDU DN request container): Length (%d) is not in range of min: 1 and max 253 bytes",
        length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  dn_specific_identity.resize(length);
  HANDLE_CODE(bref.unpack_bytes(dn_specific_identity.data(), length));
  return SRSASN_SUCCESS;
}

// IE: Extended protocol configuration options
// Reference: 9.11.4.6
SRSASN_CODE extended_protocol_configuration_options_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  // TODO proper packing

  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  // max. length of 65535 not checked: uint overflow
  if (length < 1) {
    asn1::log_error(
        "Encoding Failed (Extended protocol configuration options): Packed length (%d) is not in range of min: 1 bytes",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: Extended protocol configuration options
// Reference: 9.11.4.6
SRSASN_CODE extended_protocol_configuration_options_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  // max. length of 65535 not checked: uint overflow
  if (length < 1) {
    asn1::log_error(
        "Decoding Failed (Extended protocol configuration options): Length (%d) is not in range of min: 1 bytes",
        length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: IP header compression configuration
// Reference: 9.11.4.24
SRSASN_CODE ip_header_compression_configuration_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // TODO proper packing

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  // max. length of 255 not checked: uint overflow
  if (length < 3) {
    asn1::log_error(
        "Encoding Failed (IP header compression configuration): Packed length (%d) is not in range of min: 3 bytes",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: IP header compression configuration
// Reference: 9.11.4.24
SRSASN_CODE ip_header_compression_configuration_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  // max. length of 255 not checked: uint overflow
  if (length < 3) {
    asn1::log_error(
        "Decoding Failed (IP header compression configuration): Length (%d) is not in range of min: 3 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: DS-TT Ethernet port MAC address
// Reference: 9.11.4.25
SRSASN_CODE ds_tt__ethernet_port_mac_address_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack_bytes(ds_tt__ethernet_port_mac_address_contents.data(), 6));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 6) {
    asn1::log_error(
        "Encoding Failed (DS-TT Ethernet port MAC address): Packed length (%d) does not equal expected length 6",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: DS-TT Ethernet port MAC address
// Reference: 9.11.4.25
SRSASN_CODE ds_tt__ethernet_port_mac_address_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 6) {
    asn1::log_error("Decoding Failed (DS-TT Ethernet port MAC address): Length (%d) does not equal expected length 6",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack_bytes(ds_tt__ethernet_port_mac_address_contents.data(), 6));
  return SRSASN_SUCCESS;
}

// IE: UE-DS-TT residence time
// Reference: 9.11.4.26
SRSASN_CODE ue_ds_tt_residence_time_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack_bytes(ue_ds_tt_residence_time_contents.data(), 8));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 8) {
    asn1::log_error("Encoding Failed (UE-DS-TT residence time): Packed length (%d) does not equal expected length 8",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: UE-DS-TT residence time
// Reference: 9.11.4.26
SRSASN_CODE ue_ds_tt_residence_time_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 8) {
    asn1::log_error("Decoding Failed (UE-DS-TT residence time): Length (%d) does not equal expected length 8", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack_bytes(ue_ds_tt_residence_time_contents.data(), 8));
  return SRSASN_SUCCESS;
}

// IE: Port management information container
// Reference: 9.11.4.27
SRSASN_CODE port_management_information_container_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  HANDLE_CODE(
      bref.pack_bytes(port_management_information_container.data(), port_management_information_container.size()));
  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  // max. length of 65535 not checked: uint overflow
  if (length < 1) {
    asn1::log_error(
        "Encoding Failed (Port management information container): Packed length (%d) is not in range of min: 1 bytes",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: Port management information container
// Reference: 9.11.4.27
SRSASN_CODE port_management_information_container_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  // max. length of 65535 not checked: uint overflow
  if (length < 1) {
    asn1::log_error(
        "Decoding Failed (Port management information container): Length (%d) is not in range of min: 1 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  port_management_information_container.resize(length);
  HANDLE_CODE(bref.unpack_bytes(port_management_information_container.data(), length));
  return SRSASN_SUCCESS;
}

// IE: Ethernet header compression configuration
// Reference: 9.11.4.28
SRSASN_CODE ethernet_header_compression_configuration_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // 6 Spare bits
  HANDLE_CODE(bref.pack(0x0, 6));
  HANDLE_CODE(cid__length.pack(bref));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 1) {
    asn1::log_error("Encoding Failed (Ethernet header compression configuration): Packed length (%d) does not equal "
                    "expected length 1",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Ethernet header compression configuration
// Reference: 9.11.4.28
SRSASN_CODE ethernet_header_compression_configuration_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 1) {
    asn1::log_error(
        "Decoding Failed (Ethernet header compression configuration): Length (%d) does not equal expected length 1",
        length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  // 6 Spare bits
  bref.advance_bits(6);
  HANDLE_CODE(cid__length.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: PDU address
// Reference: 9.11.4.10
SRSASN_CODE pdu_address_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // 4 Spare bits
  HANDLE_CODE(bref.pack(0x0, 4));
  HANDLE_CODE(bref.pack(si6_lla, 1));
  HANDLE_CODE(pdu_session_type_value.pack(bref));

  if (pdu_session_type_value == pdu_address_t::PDU_session_type_value_type_::options::ipv4) {
    HANDLE_CODE(bref.pack_bytes(ipv4.data(), 4));
  } else if (pdu_session_type_value == pdu_address_t::PDU_session_type_value_type_::options::ipv6) {
    HANDLE_CODE(bref.pack_bytes(ipv6.data(), 16));
  } else if (pdu_session_type_value == pdu_address_t::PDU_session_type_value_type_::options::ipv4v6) {
    HANDLE_CODE(bref.pack_bytes(ipv6.data(), 16));
    HANDLE_CODE(bref.pack_bytes(ipv4.data(), 4));
  }

  if (si6_lla == true) {
    HANDLE_CODE(bref.pack_bytes(smf_i_pv6_link_local_address.data(), 16));
  }

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 5 || length > 29) {
    asn1::log_error("Encoding Failed (PDU address): Packed length (%d) is not in range of min: 5 and max 29 bytes",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: PDU address
// Reference: 9.11.4.10
SRSASN_CODE pdu_address_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 5 || length > 29) {
    asn1::log_error("Decoding Failed (PDU address): Length (%d) is not in range of min: 5 and max 29 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  // 4 Spare bits
  bref.advance_bits(4);
  HANDLE_CODE(bref.unpack(si6_lla, 1));
  HANDLE_CODE(pdu_session_type_value.unpack(bref));
  if (length == 5 && pdu_session_type_value == pdu_address_t::PDU_session_type_value_type_::options::ipv4 &&
      si6_lla == false) {
    HANDLE_CODE(bref.unpack_bytes(ipv4.data(), 4));
  } else if (length == 9 && pdu_session_type_value == pdu_address_t::PDU_session_type_value_type_::options::ipv6 &&
             si6_lla == false) {
    HANDLE_CODE(bref.unpack_bytes(ipv6.data(), 16));
  } else if (length == 13 && pdu_session_type_value == pdu_address_t::PDU_session_type_value_type_::options::ipv4v6 &&
             si6_lla == false) {
    HANDLE_CODE(bref.unpack_bytes(ipv6.data(), 16));
    HANDLE_CODE(bref.unpack_bytes(ipv4.data(), 4));
  } else if (length == 25 && pdu_session_type_value == pdu_address_t::PDU_session_type_value_type_::options::ipv6 &&
             si6_lla == true) {
    HANDLE_CODE(bref.unpack_bytes(ipv6.data(), 16));
    HANDLE_CODE(bref.unpack_bytes(smf_i_pv6_link_local_address.data(), 16));
  } else if (length == 29 && pdu_session_type_value == pdu_address_t::PDU_session_type_value_type_::options::ipv4v6 &&
             si6_lla == true) {
    HANDLE_CODE(bref.unpack_bytes(ipv6.data(), 16));
    HANDLE_CODE(bref.unpack_bytes(ipv4.data(), 4));
    HANDLE_CODE(bref.unpack_bytes(smf_i_pv6_link_local_address.data(), 16));
  } else {
    asn1::log_error("Not expected combination of length and type field");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// IE: QoS rules
// Reference: 9.11.4.13
SRSASN_CODE qo_s_rules_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  // max. length of 65535 not checked: uint overflow
  if (length < 4) {
    asn1::log_error("Encoding Failed (QoS rules): Packed length (%d) is not in range of min: 4 bytes", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: QoS rules
// Reference: 9.11.4.13
SRSASN_CODE qo_s_rules_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  // max. length of 65535 not checked: uint overflow
  if (length < 4) {
    asn1::log_error("Decoding Failed (QoS rules): Length (%d) is not in range of min: 4 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.advance_bits(length * 8));
  return SRSASN_SUCCESS;
}

// IE: Session-AMBR
// Reference: 9.11.4.14
SRSASN_CODE session_ambr_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(unit_session_ambr_for_downlink.pack(bref));
  HANDLE_CODE(bref.pack(session_ambr_for_downlink, 16));
  HANDLE_CODE(unit_session_ambr_for_uplink.pack(bref));
  HANDLE_CODE(bref.pack(session_ambr_for_uplink, 16));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 6) {
    asn1::log_error("Encoding Failed (Session-AMBR): Packed length (%d) does not equal expected length 6", length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Session-AMBR
// Reference: 9.11.4.14
SRSASN_CODE session_ambr_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 6) {
    asn1::log_error("Decoding Failed (Session-AMBR): Length (%d) does not equal expected length 6", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(unit_session_ambr_for_downlink.unpack(bref));
  HANDLE_CODE(bref.unpack(session_ambr_for_downlink, 16));
  HANDLE_CODE(unit_session_ambr_for_uplink.unpack(bref));
  HANDLE_CODE(bref.unpack(session_ambr_for_uplink, 16));
  return SRSASN_SUCCESS;
}

// IE: 5GSM cause
// Reference: 9.11.4.2
SRSASN_CODE cause_5gsm_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(cause_value.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: 5GSM cause
// Reference: 9.11.4.2
SRSASN_CODE cause_5gsm_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(cause_value.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: GPRS timer
// Reference: 9.11.2.3
SRSASN_CODE gprs_timer_t::pack(asn1::bit_ref& bref)
{
  HANDLE_CODE(unit.pack(bref));
  HANDLE_CODE(bref.pack(timer_value, 5));
  return SRSASN_SUCCESS;
}

// IE: GPRS timer
// Reference: 9.11.2.3
SRSASN_CODE gprs_timer_t::unpack(asn1::cbit_ref& bref)
{
  HANDLE_CODE(unit.unpack(bref));
  HANDLE_CODE(bref.unpack(timer_value, 5));
  return SRSASN_SUCCESS;
}

// IE: Always-on PDU session indication
// Reference: 9.11.4.3
SRSASN_CODE always_on_pdu_session_indication_t::pack(asn1::bit_ref& bref)
{
  // 3 Spare bits
  HANDLE_CODE(bref.pack(0x0, 3));
  HANDLE_CODE(bref.pack(apsr, 1));
  return SRSASN_SUCCESS;
}

// IE: Always-on PDU session indication
// Reference: 9.11.4.3
SRSASN_CODE always_on_pdu_session_indication_t::unpack(asn1::cbit_ref& bref)
{
  // 3 Spare bits
  bref.advance_bits(3);
  HANDLE_CODE(bref.unpack(apsr, 1));
  return SRSASN_SUCCESS;
}

// IE: Mapped EPS bearer contexts
// Reference: 9.11.4.8
SRSASN_CODE mapped_eps_bearer_contexts_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  // TODO proper packing

  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  // max. length of 65535 not checked: uint overflow
  if (length < 4) {
    asn1::log_error("Encoding Failed (Mapped EPS bearer contexts): Packed length (%d) is not in range of min: 4 bytes",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: Mapped EPS bearer contexts
// Reference: 9.11.4.8
SRSASN_CODE mapped_eps_bearer_contexts_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  // max. length of 65535 not checked: uint overflow
  if (length < 4) {
    asn1::log_error("Decoding Failed (Mapped EPS bearer contexts): Length (%d) is not in range of min: 4 bytes",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: QoS flow descriptions
// Reference: 9.11.4.12
SRSASN_CODE qo_s_flow_descriptions_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  // TODO proper packing

  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  // max. length of 65535 not checked: uint overflow
  if (length < 3) {
    asn1::log_error("Encoding Failed (QoS flow descriptions): Packed length (%d) is not in range of min: 3 bytes",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: QoS flow descriptions
// Reference: 9.11.4.12
SRSASN_CODE qo_s_flow_descriptions_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  // max. length of 65535 not checked: uint overflow
  if (length < 3) {
    asn1::log_error("Decoding Failed (QoS flow descriptions): Length (%d) is not in range of min: 3 bytes", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }

  // TODO proper unpacking
  bref.advance_bits(length * 8);
  return SRSASN_SUCCESS;
}

// IE: 5GSM network feature support
// Reference: 9.11.4.18
SRSASN_CODE network_feature_support_5gsm_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // 7 Spare bits
  HANDLE_CODE(bref.pack(0x0, 7));
  HANDLE_CODE(ept_s1.pack(bref));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length < 1 || length > 13) {
    asn1::log_error(
        "Encoding Failed (5GSM network feature support): Packed length (%d) is not in range of min: 1 and max 13 bytes",
        length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: 5GSM network feature support
// Reference: 9.11.4.18
SRSASN_CODE network_feature_support_5gsm_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length < 1 || length > 13) {
    asn1::log_error(
        "Decoding Failed (5GSM network feature support): Length (%d) is not in range of min: 1 and max 13 bytes",
        length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  // 7 Spare bits
  bref.advance_bits(7);
  HANDLE_CODE(ept_s1.unpack(bref));
  if (length > 1) {
    HANDLE_CODE(bref.advance_bits((length - 1) * 8));
  }
  return SRSASN_SUCCESS;
}

// IE: Serving PLMN rate control
// Reference: 9.11.4.20
SRSASN_CODE serving_plmn_rate_control_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  HANDLE_CODE(bref.pack(serving_plmn_rate_control_value, 16));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 2) {
    asn1::log_error("Encoding Failed (Serving PLMN rate control): Packed length (%d) does not equal expected length 2",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Serving PLMN rate control
// Reference: 9.11.4.20
SRSASN_CODE serving_plmn_rate_control_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 2) {
    asn1::log_error("Decoding Failed (Serving PLMN rate control): Length (%d) does not equal expected length 2",
                    length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack(serving_plmn_rate_control_value, 16));
  return SRSASN_SUCCESS;
}

// IE: ATSSS container
// Reference: 9.11.4.22
SRSASN_CODE atsss_container_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(16));

  HANDLE_CODE(bref.pack_bytes(nas_message_container.data(), nas_message_container.size()));
  bref.align_bytes_zero();
  uint16_t length = (uint16_t)(ceilf((float)bref.distance(bref_length) / 8) - 2);

  // min. length of 0 not checked: uint underflow
  // max. length of 65535 not checked: uint overflow
  HANDLE_CODE(bref_length.pack(length, 16));
  return SRSASN_SUCCESS;
}

// IE: ATSSS container
// Reference: 9.11.4.22
SRSASN_CODE atsss_container_t::unpack(asn1::cbit_ref& bref)
{
  uint16_t length = 0;
  HANDLE_CODE(bref.unpack(length, 16));
  // min. length of 0 not checked: uint underflow
  nas_message_container.resize(length);
  HANDLE_CODE(bref.unpack_bytes(nas_message_container.data(), length));
  return SRSASN_SUCCESS;
}

// IE: Control plane only indication
// Reference: 9.11.4.23
SRSASN_CODE control_plane_only_indication_t::pack(asn1::bit_ref& bref)
{
  // 3 Spare bits
  HANDLE_CODE(bref.pack(0x0, 3));
  HANDLE_CODE(bref.pack(cpoi, 1));
  return SRSASN_SUCCESS;
}

// IE: Control plane only indication
// Reference: 9.11.4.23
SRSASN_CODE control_plane_only_indication_t::unpack(asn1::cbit_ref& bref)
{
  // 3 Spare bits
  bref.advance_bits(3);
  HANDLE_CODE(bref.unpack(cpoi, 1));
  return SRSASN_SUCCESS;
}

// IE: Allowed SSC mode
// Reference: 9.11.4.5
SRSASN_CODE allowed_ssc_mode_t::pack(asn1::bit_ref& bref)
{
  // 1 Spare bits
  HANDLE_CODE(bref.pack(0x0, 1));
  HANDLE_CODE(bref.pack(ssc3, 1));
  HANDLE_CODE(bref.pack(ssc2, 1));
  HANDLE_CODE(bref.pack(ssc1, 1));
  return SRSASN_SUCCESS;
}

// IE: Allowed SSC mode
// Reference: 9.11.4.5
SRSASN_CODE allowed_ssc_mode_t::unpack(asn1::cbit_ref& bref)
{
  // 1 Spare bits
  bref.advance_bits(1);
  HANDLE_CODE(bref.unpack(ssc3, 1));
  HANDLE_CODE(bref.unpack(ssc2, 1));
  HANDLE_CODE(bref.unpack(ssc1, 1));
  return SRSASN_SUCCESS;
}

// IE: 5GSM congestion re-attempt indicator
// Reference: 9.11.4.21
SRSASN_CODE congestion_re_attempt_indicator_5gsm_t::pack(asn1::bit_ref& bref)
{
  // 7 Spare bits
  HANDLE_CODE(bref.pack(0x0, 7));
  HANDLE_CODE(abo.pack(bref));
  return SRSASN_SUCCESS;
}

// IE: 5GSM congestion re-attempt indicator
// Reference: 9.11.4.21
SRSASN_CODE congestion_re_attempt_indicator_5gsm_t::unpack(asn1::cbit_ref& bref)
{
  // 7 Spare bits
  bref.advance_bits(7);
  HANDLE_CODE(abo.unpack(bref));
  return SRSASN_SUCCESS;
}

// IE: Re-attempt indicator
// Reference: 9.11.4.17
SRSASN_CODE re_attempt_indicator_t::pack(asn1::bit_ref& bref)
{
  // Save length bref pointer
  asn1::bit_ref bref_length = bref;
  HANDLE_CODE(bref.advance_bits(8));

  // 6 Spare bits
  HANDLE_CODE(bref.pack(0x0, 6));
  HANDLE_CODE(bref.pack(eplmnc, 1));
  HANDLE_CODE(bref.pack(ratc, 1));

  bref.align_bytes_zero();
  uint8_t length = (uint8_t)(ceilf((float)bref.distance(bref_length) / 8) - 1);

  if (length != 1) {
    asn1::log_error("Encoding Failed (Re-attempt indicator): Packed length (%d) does not equal expected length 1",
                    length);
    return asn1::SRSASN_ERROR_ENCODE_FAIL;
  }
  HANDLE_CODE(bref_length.pack(length, 8));
  return SRSASN_SUCCESS;
}

// IE: Re-attempt indicator
// Reference: 9.11.4.17
SRSASN_CODE re_attempt_indicator_t::unpack(asn1::cbit_ref& bref)
{
  uint8_t length = 0;
  HANDLE_CODE(bref.unpack(length, 8));
  if (length != 1) {
    asn1::log_error("Decoding Failed (Re-attempt indicator): Length (%d) does not equal expected length 1", length);
    return asn1::SRSASN_ERROR_DECODE_FAIL;
  }
  // 6 Spare bits
  bref.advance_bits(6);
  HANDLE_CODE(bref.unpack(eplmnc, 1));
  HANDLE_CODE(bref.unpack(ratc, 1));
  return SRSASN_SUCCESS;
}

} // namespace nas_5g
} // namespace srsran
