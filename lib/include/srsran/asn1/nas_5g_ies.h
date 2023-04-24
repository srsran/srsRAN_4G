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
#ifndef SRSRAN_NAS_5G_IES_H
#define SRSRAN_NAS_5G_IES_H
#include "nas_5g_utils.h"

#include "srsran/asn1/asn1_utils.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/config.h"

#include <array>
#include <stdint.h>
#include <vector>

namespace srsran {
namespace nas_5g {

// IEs

// IE: 5GS registration type
// Reference: 9.11.3.7
class registration_type_5gs_t
{
public:
  struct registration_type_type_ {
    enum options {
      initial_registration           = 0b001,
      mobility_registration_updating = 0b010,
      periodic_registration_updating = 0b011,
      emergency_registration         = 0b100,
      reserved                       = 0b111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<registration_type_type_, 3> registration_type_type;

  struct follow_on_request_bit_type_ {
    enum options {
      no_follow_on_request_pending = 0b0,
      follow_on_request_pending    = 0b1,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<follow_on_request_bit_type_, 1> follow_on_request_bit_type;

  follow_on_request_bit_type follow_on_request_bit = follow_on_request_bit_type_::options::no_follow_on_request_pending;
  registration_type_type     registration_type     = registration_type_type_::options::initial_registration;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // registration_type_5gs_t

// IE: key set identifier
// Reference: 9.11.3.32
class key_set_identifier_t
{
public:
  struct security_context_flag_type_ {
    enum options {
      native_security_context = 0b0,
      mapped_security_context = 0b1,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<security_context_flag_type_, 1> security_context_flag_type;

  struct nas_key_set_identifier_type_ {
    enum options {
      no_key_is_available_or_reserved = 0b111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<nas_key_set_identifier_type_, 3> nas_key_set_identifier_type;

  security_context_flag_type  security_context_flag = security_context_flag_type_::options::native_security_context;
  nas_key_set_identifier_type nas_key_set_identifier =
      nas_key_set_identifier_type_::options::no_key_is_available_or_reserved;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // key_set_identifier_t

// IE: 5GS mobile identity
// Reference: 9.11.3.4
class mobile_identity_5gs_t
{
public:
  struct identity_types_ {
    enum options {
      no_identity = 0b000,
      suci        = 0b001,
      guti_5g     = 0b010,
      imei        = 0b011,
      s_tmsi_5g   = 0b100,
      imeisv      = 0b101,
      mac_address = 0b110,
      eui_64      = 0b111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<identity_types_, 3> identity_types;

  void           set(identity_types::options e = identity_types::no_identity) { type_ = e; };
  identity_types type() const { return type_; }

  class suci_s
  {
  public:
    struct supi_format_type_ {
      enum options {
        imsi                        = 0b000,
        network_specific_identifier = 0b010,
        gci                         = 0b011,
        gli                         = 0b100,

      } value;
      const char* to_string() const;
    };
    typedef nas_enumerated<supi_format_type_, 3> supi_format_type;

    struct protection_scheme_id_type_ {
      enum options {
        null_scheme            = 0b0000,
        ecies_scheme_profile_a = 0b0001,
        ecies_scheme_profile_b = 0b0010,

      } value;
      const char* to_string() const;
    };
    typedef nas_enumerated<protection_scheme_id_type_, 4> protection_scheme_id_type;

    supi_format_type          supi_format = supi_format_type_::options::imsi;
    std::array<uint8_t, 3>    mcc;
    std::array<uint8_t, 3>    mnc;
    std::array<uint8_t, 4>    routing_indicator;
    protection_scheme_id_type protection_scheme_id = protection_scheme_id_type_::options::null_scheme;
    uint8_t                   home_network_public_key_identifier;
    std::vector<uint8_t>      scheme_output;

    SRSASN_CODE pack(asn1::bit_ref& bref, asn1::bit_ref& bref_tmp);
    SRSASN_CODE unpack(asn1::cbit_ref& bref, uint8_t tmp, uint32_t length);

  }; // suci
  class guti_5g_s
  {
  public:
    std::array<uint8_t, 3> mcc;
    std::array<uint8_t, 3> mnc;
    uint8_t                amf_region_id;
    uint16_t               amf_set_id;
    uint8_t                amf_pointer;
    uint32_t               tmsi_5g;

    SRSASN_CODE pack(asn1::bit_ref& bref, asn1::bit_ref& bref_tmp);
    SRSASN_CODE unpack(asn1::cbit_ref& bref, uint8_t tmp, uint32_t length);

  }; // guti_5g
  class imei_s
  {
  public:
    bool                    odd_even_indicator;
    std::array<uint8_t, 15> imei;

    SRSASN_CODE pack(asn1::bit_ref& bref, asn1::bit_ref& bref_tmp);
    SRSASN_CODE unpack(asn1::cbit_ref& bref, uint8_t tmp, uint32_t length);

  }; // imei
  class s_tmsi_5g_s
  {
  public:
    uint16_t amf_set_id;
    uint8_t  amf_pointer;
    uint32_t tmsi_5g;

    SRSASN_CODE pack(asn1::bit_ref& bref, asn1::bit_ref& bref_tmp);
    SRSASN_CODE unpack(asn1::cbit_ref& bref, uint8_t tmp, uint32_t length);

  }; // s_tmsi_5g
  class imeisv_s
  {
  public:
    bool                    odd_even_indicator;
    std::array<uint8_t, 17> imeisv;

    SRSASN_CODE pack(asn1::bit_ref& bref, asn1::bit_ref& bref_tmp);
    SRSASN_CODE unpack(asn1::cbit_ref& bref, uint8_t tmp, uint32_t length);

  }; // imeisv
  class mac_address_s
  {
  public:
    std::array<uint8_t, 6> mac_address;

    SRSASN_CODE pack(asn1::bit_ref& bref, asn1::bit_ref& bref_tmp);
    SRSASN_CODE unpack(asn1::cbit_ref& bref, uint8_t tmp, uint32_t length);

  }; // mac_address
  class eui_64_s
  {
  public:
    std::array<uint8_t, 8> eui_64;

    SRSASN_CODE pack(asn1::bit_ref& bref, asn1::bit_ref& bref_tmp);
    SRSASN_CODE unpack(asn1::cbit_ref& bref, uint8_t tmp, uint32_t length);

  }; // eui_64

  // Getters

  suci_s& suci()
  {
    asn1::assert_choice_type(identity_types::suci, type_, "suci");
    return *srslog::detail::any_cast<suci_s>(&choice_container);
  }

  guti_5g_s& guti_5g()
  {
    asn1::assert_choice_type(identity_types::guti_5g, type_, "guti_5g");
    return *srslog::detail::any_cast<guti_5g_s>(&choice_container);
  }

  imei_s& imei()
  {
    asn1::assert_choice_type(identity_types::imei, type_, "imei");
    return *srslog::detail::any_cast<imei_s>(&choice_container);
  }

  s_tmsi_5g_s& s_tmsi_5g()
  {
    asn1::assert_choice_type(identity_types::s_tmsi_5g, type_, "s_tmsi_5g");
    return *srslog::detail::any_cast<s_tmsi_5g_s>(&choice_container);
  }

  imeisv_s& imeisv()
  {
    asn1::assert_choice_type(identity_types::imeisv, type_, "imeisv");
    return *srslog::detail::any_cast<imeisv_s>(&choice_container);
  }

  mac_address_s& mac_address()
  {
    asn1::assert_choice_type(identity_types::mac_address, type_, "mac_address");
    return *srslog::detail::any_cast<mac_address_s>(&choice_container);
  }

  eui_64_s& eui_64()
  {
    asn1::assert_choice_type(identity_types::eui_64, type_, "eui_64");
    return *srslog::detail::any_cast<eui_64_s>(&choice_container);
  }

  suci_s& set_suci()
  {
    set(identity_types::suci);
    choice_container = srslog::detail::any{suci_s()};
    return *srslog::detail::any_cast<suci_s>(&choice_container);
  }

  guti_5g_s& set_guti_5g()
  {
    set(identity_types::guti_5g);
    choice_container = srslog::detail::any{guti_5g_s()};
    return *srslog::detail::any_cast<guti_5g_s>(&choice_container);
  }

  imei_s& set_imei()
  {
    set(identity_types::imei);
    choice_container = srslog::detail::any{imei_s()};
    return *srslog::detail::any_cast<imei_s>(&choice_container);
  }

  s_tmsi_5g_s& set_s_tmsi_5g()
  {
    set(identity_types::s_tmsi_5g);
    choice_container = srslog::detail::any{s_tmsi_5g_s()};
    return *srslog::detail::any_cast<s_tmsi_5g_s>(&choice_container);
  }

  imeisv_s& set_imeisv()
  {
    set(identity_types::imeisv);
    choice_container = srslog::detail::any{imeisv_s()};
    return *srslog::detail::any_cast<imeisv_s>(&choice_container);
  }

  mac_address_s& set_mac_address()
  {
    set(identity_types::mac_address);
    choice_container = srslog::detail::any{mac_address_s()};
    return *srslog::detail::any_cast<mac_address_s>(&choice_container);
  }

  eui_64_s& set_eui_64()
  {
    set(identity_types::eui_64);
    choice_container = srslog::detail::any{eui_64_s()};
    return *srslog::detail::any_cast<eui_64_s>(&choice_container);
  }

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

private:
  identity_types      type_ = identity_types_::options::no_identity;
  srslog::detail::any choice_container;

}; // mobile_identity_5gs_t

// IE: 5GMM capability
// Reference: 9.11.3.1
class capability_5gmm_t
{
public:
  bool sgc               = false;
  bool iphc_cp_c_io_t_5g = false;
  bool n3_data           = false;
  bool cp_c_io_t_5g      = false;
  bool restrict_ec       = false;
  bool lpp               = false;
  bool ho_attach         = false;
  bool s1_mode           = false;
  bool racs              = false;
  bool nssaa             = false;
  bool lcs_5g            = false;
  bool v2_xcnpc5         = false;
  bool v2_xcepc5         = false;
  bool v2_x              = false;
  bool up_c_io_t_5g      = false;
  bool srvcc_5g          = false;
  bool ehc_cp_c_io_t_5g  = false;
  bool multiple_up       = false;
  bool wusa              = false;
  bool cag               = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // capability_5gmm_t

// IE: UE security capability
// Reference: 9.11.3.54
class ue_security_capability_t
{
public:
  bool ea0_5g_supported     = false;
  bool ea1_128_5g_supported = false;
  bool ea2_128_5g_supported = false;
  bool ea3_128_5g_supported = false;
  bool ea4_5g_supported     = false;
  bool ea5_5g_supported     = false;
  bool ea6_5g_supported     = false;
  bool ea7_5g_supported     = false;
  bool ia0_5g_supported     = false;
  bool ia1_128_5g_supported = false;
  bool ia2_128_5g_supported = false;
  bool ia3_128_5g_supported = false;
  bool ia4_5g_supported     = false;
  bool ia5_5g_supported     = false;
  bool ia6_5g_supported     = false;
  bool ia7_5g_supported     = false;
  bool eps_caps_present     = false;
  bool eea0_supported       = false;
  bool eea1_128_supported   = false;
  bool eea2_128_supported   = false;
  bool eea3_128_supported   = false;
  bool eea4_supported       = false;
  bool eea5_supported       = false;
  bool eea6_supported       = false;
  bool eea7_supported       = false;
  bool eia0_supported       = false;
  bool eia1_128_supported   = false;
  bool eia2_128_supported   = false;
  bool eia3_128_supported   = false;
  bool eia4_supported       = false;
  bool eia5_supported       = false;
  bool eia6_supported       = false;
  bool eia7_supported       = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ue_security_capability_t

// IE: S-NSSAI
// Reference: 9.11.2.8
// IE: S-NSSAI
// Reference: 9.11.2.8
class s_nssai_t
{
public:
  struct SST_type_ {
    enum options {
      sst                                         = 0b00000001,
      sst_and_mapped_hplmn_sst                    = 0b00000010,
      sst_and_sd                                  = 0b00000100,
      sst_sd_and_mapped_hplmn_sst                 = 0b00000101,
      sst_sd_mapped_hplmn_sst_and_mapped_hplmn_sd = 0b00001000,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<SST_type_, 8> SST_type;

  SST_type type;
  uint8_t  sst;
  uint32_t sd;
  uint8_t  mapped_hplmn_sst;
  uint32_t mapped_hplmn_sd;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // s_nssai_t

// IE: NSSAI
// Reference: 9.11.3.37
class nssai_t
{
public:
  std::vector<s_nssai_t> s_nssai_list;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // nssai_t

// IE: 5GS tracking area identity
// Reference: 9.11.3.8
class tracking_area_identity_5gs_t
{
public:
  std::array<uint8_t, 3> mcc;
  std::array<uint8_t, 3> mnc;
  uint32_t               tac;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // tracking_area_identity_5gs_t

// IE: S1 UE network capability
// Reference: 9.11.3.48
class s1_ue_network_capability_t
{
public:
  bool eea0_supported              = false;
  bool eea1_128_supported          = false;
  bool eea2_128_supported          = false;
  bool eea3_128_supported          = false;
  bool eea4_supported              = false;
  bool eea5_supported              = false;
  bool eea6_supported              = false;
  bool eea7_supported              = false;
  bool eia0_supported              = false;
  bool eia1_128_supported          = false;
  bool eia2_128_supported          = false;
  bool eia3_128_supported          = false;
  bool eia4_supported              = false;
  bool eia5_supported              = false;
  bool eia6_supported              = false;
  bool eia7_supported              = false;
  bool uea0_supported              = false;
  bool uea1_128_supported          = false;
  bool uea2_128_supported          = false;
  bool uea3_128_supported          = false;
  bool uea4_supported              = false;
  bool uea5_supported              = false;
  bool uea6_supported              = false;
  bool uea7_supported              = false;
  bool ucs2_support                = false;
  bool uia1_128_supported          = false;
  bool uia2_128_supported          = false;
  bool uia3_128_supported          = false;
  bool uia4_supported              = false;
  bool uia5_supported              = false;
  bool uia6_supported              = false;
  bool uia7_supported              = false;
  bool pro_se_dd_supported         = false;
  bool pro_se_supported            = false;
  bool h245_ash_supported          = false;
  bool acc_csfb_supported          = false;
  bool llp_supported               = false;
  bool lcs_supported               = false;
  bool srvcc_capability_supported  = false;
  bool nf_capability_supported     = false;
  bool e_pco_supported             = false;
  bool hc_cp_c_io_t_supported      = false;
  bool e_rw_o_pdn_supported        = false;
  bool s1_u_data_supported         = false;
  bool up_c_io_t_supported         = false;
  bool cp_c_io_t_supported         = false;
  bool pro_se_relay_supported      = false;
  bool pro_se_dc_supported         = false;
  bool max_15_eps_bearer_supported = false;
  bool sgc_supported               = false;
  bool n1mode_supported            = false;
  bool dcnr_supported              = false;
  bool cp_backoff_supported        = false;
  bool restrict_ec_supported       = false;
  bool v2_x_pc5_supported          = false;
  bool multiple_drb_supported      = false;
  bool nr_pc5_supported            = false;
  bool up_mt_edt_supported         = false;
  bool cp_mt_edt_supported         = false;
  bool wus_supported               = false;
  bool racs_supported              = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // s1_ue_network_capability_t

// IE: Uplink data status
// Reference: 9.11.3.57
class uplink_data_status_t
{
public:
  bool psi_7  = false;
  bool psi_6  = false;
  bool psi_5  = false;
  bool psi_4  = false;
  bool psi_3  = false;
  bool psi_2  = false;
  bool psi_1  = false;
  bool psi_0  = false;
  bool psi_15 = false;
  bool psi_14 = false;
  bool psi_13 = false;
  bool psi_12 = false;
  bool psi_11 = false;
  bool psi_10 = false;
  bool psi_9  = false;
  bool psi_8  = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // uplink_data_status_t

// IE: PDU session status
// Reference: 9.11.3.44
class pdu_session_status_t
{
public:
  bool psi_7  = false;
  bool psi_6  = false;
  bool psi_5  = false;
  bool psi_4  = false;
  bool psi_3  = false;
  bool psi_2  = false;
  bool psi_1  = false;
  bool psi_0  = false;
  bool psi_15 = false;
  bool psi_14 = false;
  bool psi_13 = false;
  bool psi_12 = false;
  bool psi_11 = false;
  bool psi_10 = false;
  bool psi_9  = false;
  bool psi_8  = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_status_t

// IE: MICO indication
// Reference: 9.11.3.31
class mico_indication_t
{
public:
  bool sprti = false;
  bool aai   = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // mico_indication_t

// IE: UE status
// Reference: 9.11.3.56
class ue_status_t
{
public:
  bool n1_mode_reg = false;
  bool s1_mode_reg = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ue_status_t

// IE: Allowed PDU session status
// Reference: 9.11.3.13
class allowed_pdu_session_status_t
{
public:
  bool psi_7  = false;
  bool psi_6  = false;
  bool psi_5  = false;
  bool psi_4  = false;
  bool psi_3  = false;
  bool psi_2  = false;
  bool psi_1  = false;
  bool psi_0  = false;
  bool psi_15 = false;
  bool psi_14 = false;
  bool psi_13 = false;
  bool psi_12 = false;
  bool psi_11 = false;
  bool psi_10 = false;
  bool psi_9  = false;
  bool psi_8  = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // allowed_pdu_session_status_t

// IE: UE usage setting
// Reference: 9.11.3.55
class ue_usage_setting_t
{
public:
  struct UE_usage_setting_type_ {
    enum options {
      voice_centric = 0b0,
      data_centric  = 0b1,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<UE_usage_setting_type_, 1> UE_usage_setting_type;

  UE_usage_setting_type ue_usage_setting = UE_usage_setting_type_::options::voice_centric;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ue_usage_setting_t

// IE: 5GS DRX parameters
// Reference: 9.11.3.2A
class drx_parameters_5gs_t
{
public:
  struct drx_value_type_ {
    enum options {
      drx_value_not_specified   = 0b0000,
      drx_cycle_parameter_t_32  = 0b0001,
      drx_cycle_parameter_t_64  = 0b0010,
      drx_cycle_parameter_t_128 = 0b0011,
      drx_cycle_parameter_t_256 = 0b0100,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<drx_value_type_, 4> drx_value_type;

  drx_value_type drx_value = drx_value_type_::options::drx_value_not_specified;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // drx_parameters_5gs_t

// IE: EPS NAS message container
// Reference: 9.11.3.24
class eps_nas_message_container_t
{
public:
  std::vector<uint8_t> eps_nas_message_container;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // eps_nas_message_container_t

// IE: DNN
// Reference: 9.11.2.1B
class dnn_t
{
public:
  std::vector<uint8_t> dnn_value;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // dnn_t

// IE: LADN indication
// Reference: 9.11.3.29
class ladn_indication_t
{
public:
  std::vector<dnn_t> ladn_dnn_values;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ladn_indication_t

// IE: Payload container type
// Reference: 9.11.3.40
class payload_container_type_t
{
public:
  struct Payload_container_type_type_ {
    enum options {
      n1_sm_information                              = 0b0001,
      sms                                            = 0b0010,
      lte_positioning_protocol_lpp_message_container = 0b0011,
      sor_transparent_container                      = 0b0100,
      ue_policy_container                            = 0b0101,
      ue_parameters_update_transparent_container     = 0b0110,
      location_services_message_container            = 0b0111,
      c_io_t_user_data_container                     = 0b1000,
      multiple_payloads                              = 0b1111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<Payload_container_type_type_, 4> Payload_container_type_type;

  Payload_container_type_type payload_container_type = Payload_container_type_type_::options::n1_sm_information;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // payload_container_type_t

// IE: Payload container
// Reference: 9.11.3.39
class payload_container_t
{
public:
  std::vector<uint8_t> payload_container_contents;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // payload_container_t

// IE: Network slicing indication
// Reference: 9.11.3.36
class network_slicing_indication_t
{
public:
  bool nssci = false;
  bool dcni  = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // network_slicing_indication_t

// IE: 5GS update type
// Reference: 9.11.3.9A
class update_type_5gs_t
{
public:
  struct SMS_requested_type_ {
    enum options {
      sms_over_nas_not_supported = 0b0,
      sms_over_nas_supported     = 0b1,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<SMS_requested_type_, 1> SMS_requested_type;

  struct NG_RAN_RCU_type_ {
    enum options {
      ue_radio_capability_update_not_needed = 0b0,
      ue_radio_capability_update_needed     = 0b1,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<NG_RAN_RCU_type_, 1> NG_RAN_RCU_type;

  struct PNB_5GS_CIoT_type_ {
    enum options {
      no_additional_information             = 0b00,
      control_plane_c_io_t_5gs_optimization = 0b01,
      user_plane_c_io_t_5gs_optimization    = 0b10,
      reserved                              = 0b11,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<PNB_5GS_CIoT_type_, 2> PNB_5GS_CIoT_type;

  struct PNB_EPS_CIoT_type_ {
    enum options {
      no_additional_information             = 0b00,
      control_plane_c_io_t_eps_optimization = 0b01,
      user_plane_c_io_t_eps_optimization    = 0b10,
      reserved                              = 0b11,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<PNB_EPS_CIoT_type_, 2> PNB_EPS_CIoT_type;

  PNB_EPS_CIoT_type  pnb_eps_c_io_t = PNB_EPS_CIoT_type_::options::no_additional_information;
  PNB_5GS_CIoT_type  pnb_5gs_c_io_t = PNB_5GS_CIoT_type_::options::no_additional_information;
  NG_RAN_RCU_type    ng_ran_rcu     = NG_RAN_RCU_type_::options::ue_radio_capability_update_not_needed;
  SMS_requested_type sms_requested  = SMS_requested_type_::options::sms_over_nas_not_supported;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // update_type_5gs_t

// IE: Mobile station classmark 2
// Reference: 9.11.3.31C
class mobile_station_classmark_2_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // mobile_station_classmark_2_t

// IE: Supported codec list
// Reference: 9.11.3.51A
class supported_codec_list_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // supported_codec_list_t

// IE: message container
// Reference: 9.11.3.33
class message_container_t
{
public:
  std::vector<uint8_t> nas_message_container;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // message_container_t

// IE: EPS bearer context status
// Reference: 9.11.3.23A
class eps_bearer_context_status_t
{
public:
  bool ebi_7  = false;
  bool ebi_6  = false;
  bool ebi_5  = false;
  bool ebi_4  = false;
  bool ebi_3  = false;
  bool ebi_2  = false;
  bool ebi_1  = false;
  bool ebi_0  = false;
  bool ebi_15 = false;
  bool ebi_14 = false;
  bool ebi_13 = false;
  bool ebi_12 = false;
  bool ebi_11 = false;
  bool ebi_10 = false;
  bool ebi_9  = false;
  bool ebi_8  = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // eps_bearer_context_status_t

// IE: Extended DRX parameters
// Reference: 9.11.3.26A
class extended_drx_parameters_t
{
public:
  struct Paging_Time_Window_type_ {
    enum options {
      seconds_0  = 0b0000,
      second_1   = 0b0001,
      seconds_2  = 0b0010,
      seconds_3  = 0b0011,
      seconds_4  = 0b0100,
      seconds_5  = 0b0101,
      seconds_6  = 0b0110,
      seconds_7  = 0b0111,
      seconds_8  = 0b1000,
      seconds_9  = 0b1001,
      seconds_10 = 0b1010,
      seconds_12 = 0b1011,
      seconds_14 = 0b1100,
      seconds_16 = 0b1101,
      seconds_18 = 0b1110,
      seconds_20 = 0b1111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<Paging_Time_Window_type_, 4> Paging_Time_Window_type;

  struct eDRX_value_type_ {
    enum options {
      second_1_28  = 0b0000,
      second_2_56  = 0b0001,
      second_3_84  = 0b0010,
      second_5_12  = 0b0011,
      second_6_4   = 0b0100,
      second_7_68  = 0b0101,
      second_8_96  = 0b0110,
      second_10_24 = 0b0111,
      second_11_52 = 0b1000,
      second_12_8  = 0b1001,
      second_14_08 = 0b1010,
      second_15_36 = 0b1011,
      second_16_64 = 0b1100,
      second_17_92 = 0b1101,
      second_19_20 = 0b1110,
      second_20_48 = 0b1111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<eDRX_value_type_, 4> eDRX_value_type;

  Paging_Time_Window_type paging__time__window;
  eDRX_value_type         e_drx_value;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // extended_drx_parameters_t

// IE: GPRS timer 3
// Reference: 9.11.2.5
class gprs_timer_3_t
{
public:
  struct Unit_type_ {
    enum options {
      value_is_incremented_in_multiples_of_10_minutes = 0b000,
      value_is_incremented_in_multiples_of_1_hour     = 0b001,
      value_is_incremented_in_multiples_of_10_hours   = 0b010,
      value_is_incremented_in_multiples_of_2_seconds  = 0b011,
      value_is_incremented_in_multiples_of_30_seconds = 0b100,
      value_is_incremented_in_multiples_of_1_minute   = 0b101,
      value_is_incremented_in_multiples_of_320_hours  = 0b110,
      value_indicates_that_the_timer_is_deactivated   = 0b111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<Unit_type_, 3> Unit_type;

  Unit_type unit;
  uint8_t   timer_value;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // gprs_timer_3_t

// IE: UE radio capability ID
// Reference: 9.11.3.68
class ue_radio_capability_id_t
{
public:
  std::vector<uint8_t> ue_radio_capability_id;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ue_radio_capability_id_t

// IE: Mapped NSSAI
// Reference: 9.11.3.31B
class mapped_nssai_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // mapped_nssai_t

// IE: Additional information requested
// Reference: 9.11.3.12A
class additional_information_requested_t
{
public:
  bool cipher_key = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // additional_information_requested_t

// IE: WUS assistance information
// Reference: 9.11.3.71
class wus_assistance_information_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // wus_assistance_information_t

// IE: N5GC indication
// Reference: 9.11.3.72
class n5gc_indication_t
{
public:
  bool n5gcreg = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // n5gc_indication_t

// IE: NB-N1 mode DRX parameters
// Reference: 9.11.3.73
class nb_n1_mode_drx_parameters_t
{
public:
  struct nb_n1_mode_drx_value_type_ {
    enum options {
      drx_value_not_specified    = 0b0000,
      drx_cycle_parameter_t_32   = 0b0001,
      drx_cycle_parameter_t_64   = 0b0010,
      drx_cycle_parameter_t_128  = 0b0011,
      drx_cycle_parameter_t_256  = 0b0100,
      drx_cycle_parameter_t_512  = 0b0101,
      drx_cycle_parameter_t_1024 = 0b0111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<nb_n1_mode_drx_value_type_, 4> nb_n1_mode_drx_value_type;

  nb_n1_mode_drx_value_type nb_n1_mode_drx_value = nb_n1_mode_drx_value_type_::options::drx_value_not_specified;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // nb_n1_mode_drx_parameters_t

// IE: 5GS registration result
// Reference: 9.11.3.6
class registration_result_5gs_t
{
public:
  struct Emergency_registered_type_ {
    enum options {
      not_registered_for_emergency_services = 0b0,
      registered_for_emergency_services     = 0b1,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<Emergency_registered_type_, 1> Emergency_registered_type;

  struct NSSAA_to_be_performed_type_ {
    enum options {
      nssaa_is_not_to_be_performed = 0b0,
      nssaa_is_to_be_performed     = 0b1,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<NSSAA_to_be_performed_type_, 1> NSSAA_to_be_performed_type;

  struct SMS_allowed_type_ {
    enum options {
      sms_over_nas_not_allowed = 0b0,
      sms_over_nas_allowed     = 0b1,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<SMS_allowed_type_, 1> SMS_allowed_type;

  struct registration_result_type_ {
    enum options {
      access_3_gpp                      = 0b001,
      non_3_gpp_access                  = 0b010,
      access_3_gpp_and_non_3_gpp_access = 0b011,
      reserved                          = 0b111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<registration_result_type_, 3> registration_result_type;

  Emergency_registered_type emergency_registered =
      Emergency_registered_type_::options::not_registered_for_emergency_services;
  NSSAA_to_be_performed_type nssaa_to_be_performed = NSSAA_to_be_performed_type_::options::nssaa_is_to_be_performed;
  SMS_allowed_type           sms_allowed           = SMS_allowed_type_::options::sms_over_nas_not_allowed;
  registration_result_type   registration_result   = registration_result_type_::options::access_3_gpp;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // registration_result_5gs_t

// IE: PLMN list
// Reference: 9.11.3.45
class plmn_list_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // plmn_list_t

// IE: 5GS tracking area identity list
// Reference: 9.11.3.9
class tracking_area_identity_list_5gs_t
{
public:
  struct type_of_list_type_ {
    enum options {
      list_of_ta_cs_belonging_to_one_plmn_or_snpn_with_non_consecutive_tac_values = 0b00,
      list_of_ta_cs_belonging_to_one_plmn_or_snpn_with_consecutive_tac_values     = 0b01,
      list_of_ta_is_belonging_to_different_plm_ns                                 = 0b10,
      reserved                                                                    = 0b11,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<type_of_list_type_, 2> type_of_list_type;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // tracking_area_identity_list_5gs_t

// IE: Rejected NSSAI
// Reference: 9.11.3.46
class rejected_nssai_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // rejected_nssai_t

// IE: 5GS network feature support
// Reference: 9.11.3.5
class network_feature_support_5gs_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // network_feature_support_5gs_t

// IE: PDU session reactivation result
// Reference: 9.11.3.42
class pdu_session_reactivation_result_t
{
public:
  bool psi_7  = false;
  bool psi_6  = false;
  bool psi_5  = false;
  bool psi_4  = false;
  bool psi_3  = false;
  bool psi_2  = false;
  bool psi_1  = false;
  bool psi_0  = false;
  bool psi_15 = false;
  bool psi_14 = false;
  bool psi_13 = false;
  bool psi_12 = false;
  bool psi_11 = false;
  bool psi_10 = false;
  bool psi_9  = false;
  bool psi_8  = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_reactivation_result_t

// IE: PDU session reactivation result error cause
// Reference: 9.11.3.43
class pdu_session_reactivation_result_error_cause_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_reactivation_result_error_cause_t

// IE: LADN information
// Reference: 9.11.3.30
class ladn_information_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ladn_information_t

// IE: Service area list
// Reference: 9.11.3.49
class service_area_list_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // service_area_list_t

// IE: GPRS timer 2
// Reference: 9.11.2.4
class gprs_timer_2_t
{
public:
  uint8_t timer_value;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // gprs_timer_2_t

// IE: Emergency number list
// Reference: 9.11.3.23
class emergency_number_list_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // emergency_number_list_t

// IE: Extended emergency number list
// Reference: 9.11.3.26
class extended_emergency_number_list_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // extended_emergency_number_list_t

// IE: SOR transparent container
// Reference: 9.11.3.51
class sor_transparent_container_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // sor_transparent_container_t

// IE: EAP message
// Reference: 9.11.2.2
class eap_message_t
{
public:
  std::vector<uint8_t> eap_message;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // eap_message_t

// IE: NSSAI inclusion mode
// Reference: 9.11.3.37A
class nssai_inclusion_mode_t
{
public:
  struct NSSAI_inclusion_mode_type_ {
    enum options {
      nssai_inclusion_mode_a = 0b00,
      nssai_inclusion_mode_b = 0b01,
      nssai_inclusion_mode_c = 0b10,
      nssai_inclusion_mode_d = 0b11,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<NSSAI_inclusion_mode_type_, 2> NSSAI_inclusion_mode_type;

  NSSAI_inclusion_mode_type nssai_inclusion_mode = NSSAI_inclusion_mode_type_::options::nssai_inclusion_mode_a;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // nssai_inclusion_mode_t

// IE: Operator-defined access category definitions
// Reference: 9.11.3.38
class operator_defined_access_category_definitions_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // operator_defined_access_category_definitions_t

// IE: Non-3GPP NW provided policies
// Reference: 9.11.3.36A
class non_3_gpp_nw_provided_policies_t
{
public:
  bool n3_en = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // non_3_gpp_nw_provided_policies_t

// IE: UE radio capability ID deletion indication
// Reference: 9.11.3.69
class ue_radio_capability_id_deletion_indication_t
{
public:
  struct Deletion_request_type_ {
    enum options {
      ue_radio_capability_id_deletion_not_requested                = 0b000,
      network_assigned_ue_radio_capability_i_ds_deletion_requested = 0b001,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<Deletion_request_type_, 3> Deletion_request_type;

  Deletion_request_type deletion_request =
      Deletion_request_type_::options::ue_radio_capability_id_deletion_not_requested;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ue_radio_capability_id_deletion_indication_t

// IE: Ciphering key data
// Reference: 9.11.3.18C
class ciphering_key_data_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ciphering_key_data_t

// IE: CAG information list
// Reference: 9.11.3.18A
class cag_information_list_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // cag_information_list_t

// IE: Truncated 5G-S-TMSI configuration
// Reference: 9.11.3.70
class truncated_5g_s_tmsi_configuration_t
{
public:
  uint8_t truncated_amf__set_id_value;
  uint8_t truncated_amf__pointer_value;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // truncated_5g_s_tmsi_configuration_t

// IE: 5GMM cause
// Reference: 9.11.3.2
class cause_5gmm_t
{
public:
  struct cause_5gmm_type_ {
    enum options {
      illegal_ue                                                   = 0b00000011,
      pei_not_accepted                                             = 0b00000101,
      illegal_me                                                   = 0b00000110,
      services_not_allowed_5gs                                     = 0b00000111,
      ue_identity_cannot_be_derived_by_the_network                 = 0b00001001,
      implicitly_de_registered                                     = 0b00001010,
      plmn_not_allowed                                             = 0b00001011,
      tracking_area_not_allowed                                    = 0b00001100,
      roaming_not_allowed_in_this_tracking_area                    = 0b00001101,
      no_suitable_cells_in_tracking_area                           = 0b00001111,
      mac_failure                                                  = 0b00010100,
      synch_failure                                                = 0b00010101,
      congestion                                                   = 0b00010110,
      ue_security_capabilities_mismatch                            = 0b00010111,
      security_mode_rejected_unspecified                           = 0b00011000,
      non_5g_authentication_unacceptable                           = 0b00011010,
      n1_mode_not_allowed                                          = 0b00011011,
      restricted_service_area                                      = 0b00011100,
      redirection_to_epc_required                                  = 0b00011111,
      ladn_not_available                                           = 0b00101011,
      no_network_slices_available                                  = 0b00111110,
      maximum_number_of_pdu_sessions_reached_                      = 0b01000001,
      insufficient_resources_for_specific_slice_and_dnn            = 0b01000011,
      insufficient_resources_for_specific_slice                    = 0b01000101,
      ng_ksi_already_in_use                                        = 0b01000111,
      non_3_gpp_access_to_5gcn_not_allowed                         = 0b01001000,
      serving_network_not_authorized                               = 0b01001001,
      temporarily_not_authorized_for_this_snpn                     = 0b01001010,
      permanently_not_authorized_for_this_snpn                     = 0b01001011,
      not_authorized_for_this_cag_or_authorized_for_cag_cells_only = 0b01001100,
      wireline_access_area_not_allowed                             = 0b01001101,
      payload_was_not_forwarded                                    = 0b01011010,
      dnn_not_supported_or_not_subscribed_in_the_slice             = 0b01011011,
      insufficient_user_plane_resources_for_the_pdu_session        = 0b01011100,
      semantically_incorrect_message                               = 0b01011111,
      invalid_mandatory_information                                = 0b01100000,
      message_type_non_existent_or_not_implemented                 = 0b01100001,
      message_type_not_compatible_with_the_protocol_state          = 0b01100010,
      information_element_non_existent_or_not_implemented          = 0b01100011,
      conditional_ie_error                                         = 0b01100100,
      message_not_compatible_with_the_protocol_state               = 0b01100101,
      protocol_error_unspecified                                   = 0b01101111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<cause_5gmm_type_, 8> cause_5gmm_type;

  cause_5gmm_type cause_5gmm = cause_5gmm_type_::options::illegal_ue;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // cause_5gmm_t

// IE: De-registration type
// Reference: 9.11.3.20
class de_registration_type_t
{
public:
  struct switch_off_type_ {
    enum options {
      normal_de_registration = 0b0,
      switch_off             = 0b1,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<switch_off_type_, 1> switch_off_type;

  struct re_registration_required_type_ {
    enum options {
      re_registration_not_required = 0b0,
      re_registration_required     = 0b1,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<re_registration_required_type_, 1> re_registration_required_type;

  struct access_type_type_ {
    enum options {
      access_3_gpp                      = 0b01,
      non_3_gpp_access                  = 0b10,
      access_3_gpp_and_non_3_gpp_access = 0b11,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<access_type_type_, 2> access_type_type;

  switch_off_type               switch_off = switch_off_type_::options::normal_de_registration;
  re_registration_required_type re_registration_required =
      re_registration_required_type_::options::re_registration_not_required;
  access_type_type access_type = access_type_type_::options::access_3_gpp;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // de_registration_type_t

// IE: Spare half octet
// Reference: 9.5
class spare_half_octet_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // spare_half_octet_t

// IE: Service type
// Reference: 9.11.3.50
class service_type_t
{
public:
  struct Service_type_value_type_ {
    enum options {
      signalling                                  = 0b0000,
      data                                        = 0b0001,
      mobile_terminated_services                  = 0b0010,
      emergency_services                          = 0b0011,
      emergency_services_fallback                 = 0b0100,
      high_priority_access                        = 0b0101,
      elevated_signalling                         = 0b0110,
      unused_shall_be_interpreted_as_signalling   = 0b0111,
      unused_shall_be_interpreted_as_signalling_1 = 0b1000,
      unused_shall_be_interpreted_as_data         = 0b1001,
      unused_shall_be_interpreted_as_data_1       = 0b1010,
      unused_shall_be_interpreted_as_data_2       = 0b1011,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<Service_type_value_type_, 4> Service_type_value_type;

  Service_type_value_type service_type_value = Service_type_value_type_::options::signalling;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // service_type_t

// IE: Configuration update indication
// Reference: 9.11.3.18
class configuration_update_indication_t
{
public:
  struct control_plane_service_type_value_type_ {
    enum options {
      mobile_originating_request  = 0b000,
      mobile_terminating_request  = 0b001,
      emergency_services          = 0b010,
      emergency_services_fallback = 0b100,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<control_plane_service_type_value_type_, 3> control_plane_service_type_value_type;

  control_plane_service_type_value_type control_plane_service_type_value =
      control_plane_service_type_value_type_::options::mobile_originating_request;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // configuration_update_indication_t

// IE: Network name
// Reference: 9.11.3.35
class network_name_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // network_name_t

// IE: Time zone
// Reference: 9.11.3.52
class time_zone_t
{
public:
  uint8_t time_zone;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // time_zone_t

// IE: Time zone and time
// Reference: 9.11.3.53
class time_zone_and_time_t
{
public:
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t time_zone;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // time_zone_and_time_t

// IE: Daylight saving time
// Reference: 9.11.3.19
class daylight_saving_time_t
{
public:
  struct value_type_ {
    enum options {
      no_adjustment_for_daylight_saving_time      = 0b00,
      hour_1_adjustment_for_daylight_saving_time  = 0b01,
      hours_2_adjustment_for_daylight_saving_time = 0b10,
      reserved                                    = 0b11,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<value_type_, 3> value_type;

  value_type value = value_type_::options::no_adjustment_for_daylight_saving_time;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // daylight_saving_time_t

// IE: SMS indication
// Reference: 9.11.3.50A
class sms_indication_t
{
public:
  bool sms_availability_indication = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // sms_indication_t

// IE: Additional configuration indication
// Reference: 9.11.3.74
class additional_configuration_indication_t
{
public:
  struct SCMR_type_ {
    enum options {
      no_additional_information                            = 0b0,
      release_of_n1_nas_signalling_connection_not_required = 0b1,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<SCMR_type_, 1> SCMR_type;

  SCMR_type scmr = SCMR_type_::options::no_additional_information;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // additional_configuration_indication_t

// IE: ABBA
// Reference: 9.11.3.10
class abba_t
{
public:
  std::vector<uint8_t> abba_contents;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // abba_t

// IE: Authentication parameter RAND
// Reference: 9.11.3.16
class authentication_parameter_rand_t
{
public:
  std::array<uint8_t, 16> rand;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // authentication_parameter_rand_t

// IE: Authentication parameter AUTN
// Reference: 9.11.3.15
class authentication_parameter_autn_t
{
public:
  std::vector<uint8_t> autn;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // authentication_parameter_autn_t

// IE: Authentication response parameter
// Reference: 9.11.3.17
class authentication_response_parameter_t
{
public:
  std::vector<uint8_t> res;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // authentication_response_parameter_t

// IE: Authentication failure parameter
// Reference: 9.11.3.14
class authentication_failure_parameter_t
{
public:
  std::vector<uint8_t> auth_failure;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // authentication_failure_parameter_t

// IE: 5GS identity type
// Reference: 9.11.3.3
class identity_type_5gs_t
{
public:
  struct identity_types_ {
    enum options {
      suci        = 0b001,
      guti_5g     = 0b010,
      imei        = 0b011,
      s_tmsi_5g   = 0b100,
      imeisv      = 0b101,
      mac_address = 0b110,
      eui_64      = 0b111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<identity_types_, 3> identity_types;

  identity_types type_of_identity = identity_types_::options::suci;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // identity_type_5gs_t

// IE: security algorithms
// Reference: 9.11.3.34
class security_algorithms_t
{
public:
  struct integrity_protection_algorithm_type_ {
    enum options {
      ia0_5g     = 0b0000,
      ia1_128_5g = 0b0001,
      ia2_128_5g = 0b0010,
      ia3_128_5g = 0b0011,
      ia4_5g     = 0b0100,
      ia5_5g     = 0b0101,
      ia6_5g     = 0b0110,
      ia7_5g     = 0b0111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<integrity_protection_algorithm_type_, 4> integrity_protection_algorithm_type;

  struct ciphering_algorithm_type_ {
    enum options {
      ea0_5g     = 0b0000,
      ea1_128_5g = 0b0001,
      ea2_128_5g = 0b0010,
      ea3_128_5g = 0b0011,
      ea4_5g     = 0b0100,
      ea5_5g     = 0b0101,
      ea6_5g     = 0b0110,
      ea7_5g     = 0b0111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<ciphering_algorithm_type_, 4> ciphering_algorithm_type;

  ciphering_algorithm_type            ciphering_algorithm = ciphering_algorithm_type_::options::ea0_5g;
  integrity_protection_algorithm_type integrity_protection_algorithm =
      integrity_protection_algorithm_type_::options::ia0_5g;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // security_algorithms_t

// IE: IMEISV request
// Reference: 9.11.3.28
class imeisv_request_t
{
public:
  struct imeisv_request_type_ {
    enum options {
      imeisv_not_requested = 0b000,
      imeisv_requested     = 0b001,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<imeisv_request_type_, 3> imeisv_request_type;

  imeisv_request_type imeisv_request = imeisv_request_type_::options::imeisv_not_requested;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // imeisv_request_t

// IE: EPS NAS security algorithms
// Reference: 9.11.3.25
class eps_nas_security_algorithms_t
{
public:
  struct integrity_protection_algorithm_type_ {
    enum options {
      eia0     = 0b000,
      eia1_128 = 0b001,
      eia2_128 = 0b010,
      eia3_128 = 0b011,
      eia4     = 0b100,
      eia5     = 0b101,
      eia6     = 0b110,
      eia7     = 0b111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<integrity_protection_algorithm_type_, 3> integrity_protection_algorithm_type;

  struct ciphering_algorithm_type_ {
    enum options {
      eea0     = 0b000,
      eea1_128 = 0b001,
      eea2_128 = 0b010,
      eea3_128 = 0b011,
      eea4     = 0b100,
      eea5     = 0b101,
      eea6     = 0b110,
      eea7     = 0b111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<ciphering_algorithm_type_, 3> ciphering_algorithm_type;

  ciphering_algorithm_type            ciphering_algorithm = ciphering_algorithm_type_::options::eea0;
  integrity_protection_algorithm_type integrity_protection_algorithm =
      integrity_protection_algorithm_type_::options::eia0;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // eps_nas_security_algorithms_t

// IE: Additional 5G security information
// Reference: 9.11.3.12
class additional_5g_security_information_t
{
public:
  bool rinmr = false;
  bool hdp   = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // additional_5g_security_information_t

// IE: S1 UE security capability
// Reference: 9.11.3.48A
class s1_ue_security_capability_t
{
public:
  bool eea0     = false;
  bool eea1_128 = false;
  bool eea2_128 = false;
  bool eea3_128 = false;
  bool eea4     = false;
  bool eea5     = false;
  bool eea6     = false;
  bool eea7     = false;
  bool eia0     = false;
  bool eia1_128 = false;
  bool eia2_128 = false;
  bool eia3_128 = false;
  bool eia4     = false;
  bool eia5     = false;
  bool eia6     = false;
  bool eia7     = false;
  bool uea0     = false;
  bool uea1     = false;
  bool uea2     = false;
  bool uea3     = false;
  bool uea4     = false;
  bool uea5     = false;
  bool uea6     = false;
  bool uea7     = false;
  bool uia1     = false;
  bool uia2     = false;
  bool uia3     = false;
  bool uia4     = false;
  bool uia5     = false;
  bool uia6     = false;
  bool uia7     = false;
  bool gea1     = false;
  bool gea2     = false;
  bool gea3     = false;
  bool gea4     = false;
  bool gea5     = false;
  bool gea6     = false;
  bool gea7     = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // s1_ue_security_capability_t

// IE: Access type
// Reference: 9.11.2.1A
class access_type_t
{
public:
  struct Access_type_value_type_ {
    enum options {
      access_3_gpp     = 0b01,
      non_3_gpp_access = 0b10,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<Access_type_value_type_, 2> Access_type_value_type;

  Access_type_value_type access_type_value = Access_type_value_type_::options::access_3_gpp;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // access_type_t

// IE: PDU session identity 2
// Reference: 9.11.3.41
class pdu_session_identity_2_t
{
public:
  uint8_t pdu_session_identity_2_value;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_identity_2_t

// IE: Request type
// Reference: 9.11.3.47
class request_type_t
{
public:
  struct Request_type_value_type_ {
    enum options {
      initial_request                = 0b001,
      existing_pdu_session           = 0b010,
      initial_emergency_request      = 0b011,
      existing_emergency_pdu_session = 0b100,
      modification_request           = 0b101,
      ma_pdu_request                 = 0b110,
      reserved                       = 0b111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<Request_type_value_type_, 3> Request_type_value_type;

  Request_type_value_type request_type_value = request_type_t::Request_type_value_type_::options::initial_request;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // request_type_t

// IE: Additional information
// Reference: 9.11.2.1
class additional_information_t
{
public:
  std::vector<uint8_t> additional_information_value;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // additional_information_t

// IE: MA PDU session information
// Reference: 9.11.3.31A
class ma_pdu_session_information_t
{
public:
  struct MA_PDU_session_information_value_type_ {
    enum options {
      ma_pdu_session_network_upgrade_is_allowed = 0b0001,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<MA_PDU_session_information_value_type_, 4> MA_PDU_session_information_value_type;

  MA_PDU_session_information_value_type ma_pdu_session_information_value =
      MA_PDU_session_information_value_type_::options::ma_pdu_session_network_upgrade_is_allowed;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ma_pdu_session_information_t

// IE: Release assistance indication
// Reference: 9.11.3.46A
class release_assistance_indication_t
{
public:
  struct Downlink_data_expected_type_ {
    enum options {
      no_information_regarding_ddx_is_conveyed       = 0b00,
      no_further_uplink_and_no_further_downlink_data = 0b01,
      only_a_single_downlink_data_transmission       = 0b10,
      reserved                                       = 0b11,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<Downlink_data_expected_type_, 2> Downlink_data_expected_type;

  Downlink_data_expected_type downlink_data_expected =
      Downlink_data_expected_type_::options::no_information_regarding_ddx_is_conveyed;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // release_assistance_indication_t

// IE: Integrity protection maximum data rate
// Reference: 9.11.4.7
class integrity_protection_maximum_data_rate_t
{
public:
  struct max_data_rate_UPIP_uplink_type_ {
    enum options {
      kbps_64        = 0b00000000,
      null           = 0b00000001,
      full_data_rate = 0b11111111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<max_data_rate_UPIP_uplink_type_, 8> max_data_rate_UPIP_uplink_type;

  struct max_data_rate_UPIP_downlink_type_ {
    enum options {
      kbps_64        = 0b00000000,
      null           = 0b00000001,
      full_data_rate = 0b11111111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<max_data_rate_UPIP_downlink_type_, 8> max_data_rate_UPIP_downlink_type;

  max_data_rate_UPIP_uplink_type   max_data_rate_upip_uplink   = max_data_rate_UPIP_uplink_type_::options::kbps_64;
  max_data_rate_UPIP_downlink_type max_data_rate_upip_downlink = max_data_rate_UPIP_downlink_type_::options::kbps_64;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // integrity_protection_maximum_data_rate_t

// IE: PDU session type
// Reference: 9.11.4.11
class pdu_session_type_t
{
public:
  struct PDU_session_type_value_type_ {
    enum options {
      ipv4         = 0b001,
      ipv6         = 0b010,
      ipv4v6       = 0b011,
      unstructured = 0b100,
      ethernet     = 0b101,
      reserved     = 0b111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<PDU_session_type_value_type_, 3> PDU_session_type_value_type;

  PDU_session_type_value_type pdu_session_type_value = PDU_session_type_value_type_::options::ipv4;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_type_t

// IE: SSC mode
// Reference: 9.11.4.16
class ssc_mode_t
{
public:
  struct SSC_mode_value_type_ {
    enum options {
      ssc_mode_1           = 0b001,
      ssc_mode_2           = 0b010,
      ssc_mode_3           = 0b011,
      unused_or_ssc_mode_1 = 0b100,
      unused_or_ssc_mode_2 = 0b101,
      unused_or_ssc_mode_3 = 0b110,
      reserved             = 0b111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<SSC_mode_value_type_, 3> SSC_mode_value_type;

  SSC_mode_value_type ssc_mode_value = ssc_mode_t::SSC_mode_value_type_::options::ssc_mode_1;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ssc_mode_t

// IE: 5GSM capability
// Reference: 9.11.4.1
class capability_5gsm_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // capability_5gsm_t

// IE: Maximum number of supported packet filters
// Reference: 9.11.4.9
class maximum_number_of_supported_packet_filters_t
{
public:
  uint16_t maximum_number_of_supported_packet_filters;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // maximum_number_of_supported_packet_filters_t

// IE: Always-on PDU session requested
// Reference: 9.11.4.4
class always_on_pdu_session_requested_t
{
public:
  bool apsi = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // always_on_pdu_session_requested_t

// IE: SM PDU DN request container
// Reference: 9.11.4.15
class sm_pdu_dn_request_container_t
{
public:
  std::vector<uint8_t> dn_specific_identity;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // sm_pdu_dn_request_container_t

// IE: Extended protocol configuration options
// Reference: 9.11.4.6
class extended_protocol_configuration_options_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // extended_protocol_configuration_options_t

// IE: IP header compression configuration
// Reference: 9.11.4.24
class ip_header_compression_configuration_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ip_header_compression_configuration_t

// IE: DS-TT Ethernet port MAC address
// Reference: 9.11.4.25
class ds_tt__ethernet_port_mac_address_t
{
public:
  std::array<uint8_t, 6> ds_tt__ethernet_port_mac_address_contents;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ds_tt__ethernet_port_mac_address_t

// IE: UE-DS-TT residence time
// Reference: 9.11.4.26
class ue_ds_tt_residence_time_t
{
public:
  std::array<uint8_t, 8> ue_ds_tt_residence_time_contents;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ue_ds_tt_residence_time_t

// IE: Port management information container
// Reference: 9.11.4.27
class port_management_information_container_t
{
public:
  std::vector<uint8_t> port_management_information_container;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // port_management_information_container_t

// IE: Ethernet header compression configuration
// Reference: 9.11.4.28
class ethernet_header_compression_configuration_t
{
public:
  struct CID_Length_type_ {
    enum options {
      ethernet_header_compression_not_used = 0b00,
      bits_7                               = 0b01,
      bits_15                              = 0b10,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<CID_Length_type_, 2> CID_Length_type;

  CID_Length_type cid__length = CID_Length_type_::options::ethernet_header_compression_not_used;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ethernet_header_compression_configuration_t

// IE: PDU address
// Reference: 9.11.4.10
class pdu_address_t
{
public:
  struct PDU_session_type_value_type_ {
    enum options {
      ipv4   = 0b001,
      ipv6   = 0b010,
      ipv4v6 = 0b011,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<PDU_session_type_value_type_, 3> PDU_session_type_value_type;

  bool                        si6_lla                = false;
  PDU_session_type_value_type pdu_session_type_value = PDU_session_type_value_type_::options::ipv4;
  std::array<uint8_t, 4>      ipv4;
  std::array<uint8_t, 8>      ipv6;
  std::array<uint8_t, 8>      smf_i_pv6_link_local_address;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_address_t

// IE: QoS rules
// Reference: 9.11.4.13
class qo_s_rules_t
{
public:
  struct qos_rule_t {};
  std::vector<qos_rule_t> qos_rules;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // qo_s_rules_t

// IE: Session-AMBR
// Reference: 9.11.4.14
class session_ambr_t
{
public:
  struct unit_session_AMBR_type_ {
    enum options {
      not_used        = 0b00000000,
      inc_by_1_kbps   = 0b00000001,
      inc_by_4_kbps   = 0b00000010,
      inc_by_16_kbps  = 0b00000011,
      inc_by_64_kbps  = 0b00000100,
      inc_by_256_kbps = 0b00000101,
      inc_by_1_mbps   = 0b00000110,
      inc_by_4_mbps   = 0b00000111,
      inc_by_16_mbps  = 0b00001000,
      inc_by_64_mbps  = 0b00001001,
      inc_by_256_mbps = 0b00001010,
      inc_by_1_gbps   = 0b00001011,
      inc_by_4_gbps   = 0b00001100,
      inc_by_16_gbps  = 0b00001101,
      inc_by_64_gbps  = 0b00001110,
      inc_by_256_gbps = 0b00001111,
      inc_by_1_tbps   = 0b00010000,
      inc_by_4_tbps   = 0b00010001,
      inc_by_16_tbps  = 0b00010010,
      inc_by_64_tbps  = 0b00010011,
      inc_by_256_tbps = 0b00010100,
      inc_by_1_pbps   = 0b00010101,
      inc_by_4_pbps   = 0b00010110,
      inc_by_16_pbps  = 0b00010111,
      inc_by_64_pbps  = 0b00011000,
      inc_by_256_pbps = 0b00011001,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<unit_session_AMBR_type_, 8> unit_session_AMBR_type;

  unit_session_AMBR_type unit_session_ambr_for_downlink = unit_session_AMBR_type_::options::not_used;
  uint16_t               session_ambr_for_downlink;
  unit_session_AMBR_type unit_session_ambr_for_uplink = unit_session_AMBR_type_::options::not_used;
  uint16_t               session_ambr_for_uplink;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // session_ambr_t

// IE: 5GSM cause
// Reference: 9.11.4.2
class cause_5gsm_t
{
public:
  struct cause_value_type_ {
    enum options {
      operator_determined_barring                                             = 0b00001000,
      insufficient_resources                                                  = 0b00011010,
      missing_or_unknown_dnn                                                  = 0b00011011,
      unknown_pdu_session_type                                                = 0b00011100,
      user_authentication_or_authorization_failed                             = 0b00011101,
      request_rejected_unspecified                                            = 0b00011111,
      service_option_not_supported                                            = 0b00100000,
      requested_service_option_not_subscribed                                 = 0b00100001,
      pti_already_in_use                                                      = 0b00100011,
      regular_deactivation                                                    = 0b00100100,
      network_failure                                                         = 0b00100110,
      reactivation_requested                                                  = 0b00100111,
      semantic_error_in_the_tft_operation                                     = 0b00101001,
      syntactical_error_in_the_tft_operation                                  = 0b00101010,
      invalid_pdu_session_identity                                            = 0b00101011,
      semantic_errors_in_packet_filter                                        = 0b00101100,
      syntactical_error_in_packet_filter                                      = 0b00101101,
      out_of_ladn_service_area                                                = 0b00101110,
      pti_mismatch                                                            = 0b00101111,
      pdu_session_type_i_pv4_only_allowed                                     = 0b00110010,
      pdu_session_type_i_pv6_only_allowed                                     = 0b00110011,
      pdu_session_does_not_exist                                              = 0b00110110,
      pdu_session_type_i_pv4v6_only_allowed                                   = 0b00111001,
      pdu_session_type_unstructured_only_allowed                              = 0b00111010,
      unsupported_5_qi_value                                                  = 0b00111011,
      pdu_session_type_ethernet_only_allowed                                  = 0b00111101,
      insufficient_resources_for_specific_slice_and_dnn                       = 0b01000011,
      not_supported_ssc_mode                                                  = 0b01000100,
      insufficient_resources_for_specific_slice                               = 0b01000101,
      missing_or_unknown_dnn_in_a_slice                                       = 0b01000110,
      invalid_pti_value                                                       = 0b01010001,
      maximum_data_rate_per_ue_for_user_plane_integrity_protection_is_too_low = 0b01010010,
      semantic_error_in_the_qo_s_operation                                    = 0b01010011,
      syntactical_error_in_the_qo_s_operation                                 = 0b01010100,
      invalid_mapped_eps_bearer_identity                                      = 0b01010101,
      semantically_incorrect_message                                          = 0b01011111,
      invalid_mandatory_information                                           = 0b01100000,
      message_type_non_existent_or_not_implemented                            = 0b01100001,
      message_type_not_compatible_with_the_protocol_state                     = 0b01100010,
      information_element_non_existent_or_not_implemented                     = 0b01100011,
      conditional_ie_error                                                    = 0b01100100,
      message_not_compatible_with_the_protocol_state                          = 0b01100101,
      protocol_error_unspecified                                              = 0b01101111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<cause_value_type_, 8> cause_value_type;

  cause_value_type cause_value = cause_value_type_::options::operator_determined_barring;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // cause_5gsm_t

// IE: GPRS timer
// Reference: 9.11.2.3
class gprs_timer_t
{
public:
  struct Unit_type_ {
    enum options {
      value_is_incremented_in_multiples_of_2_seconds = 0b000,
      value_is_incremented_in_multiples_of_1_minute  = 0b001,
      value_is_incremented_in_multiples_of_decihours = 0b010,
      value_indicates_that_the_timer_is_deactivated  = 0b111,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<Unit_type_, 3> Unit_type;

  Unit_type unit;
  uint8_t   timer_value;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // gprs_timer_t

// IE: Always-on PDU session indication
// Reference: 9.11.4.3
class always_on_pdu_session_indication_t
{
public:
  bool apsr = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // always_on_pdu_session_indication_t

// IE: Mapped EPS bearer contexts
// Reference: 9.11.4.8
class mapped_eps_bearer_contexts_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // mapped_eps_bearer_contexts_t

// IE: QoS flow descriptions
// Reference: 9.11.4.12
class qo_s_flow_descriptions_t
{
public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // qo_s_flow_descriptions_t

// IE: 5GSM network feature support
// Reference: 9.11.4.18
class network_feature_support_5gsm_t
{
public:
  struct EPT_S1_type_ {
    enum options {
      ethernet_pdn_type_in_s1_mode_not_supported = 0b0,
      ethernet_pdn_type_in_s1_mode_supported     = 0b1,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<EPT_S1_type_, 1> EPT_S1_type;

  EPT_S1_type ept_s1 = EPT_S1_type_::options::ethernet_pdn_type_in_s1_mode_not_supported;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // network_feature_support_5gsm_t

// IE: Serving PLMN rate control
// Reference: 9.11.4.20
class serving_plmn_rate_control_t
{
public:
  uint16_t serving_plmn_rate_control_value;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // serving_plmn_rate_control_t

// IE: ATSSS container
// Reference: 9.11.4.22
class atsss_container_t
{
public:
  std::vector<uint8_t> nas_message_container;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // atsss_container_t

// IE: Control plane only indication
// Reference: 9.11.4.23
class control_plane_only_indication_t
{
public:
  bool cpoi = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // control_plane_only_indication_t

// IE: Allowed SSC mode
// Reference: 9.11.4.5
class allowed_ssc_mode_t
{
public:
  bool ssc3 = false;
  bool ssc2 = false;
  bool ssc1 = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // allowed_ssc_mode_t

// IE: 5GSM congestion re-attempt indicator
// Reference: 9.11.4.21
class congestion_re_attempt_indicator_5gsm_t
{
public:
  struct abo_type_ {
    enum options {
      the_back_off_timer_is_applied_in_the_registered_plmn = 0b0,
      the_back_off_timer_is_applied_in_all_plm_ns          = 0b1,

    } value;
    const char* to_string() const;
  };
  typedef nas_enumerated<abo_type_, 1> abo_type;

  abo_type abo =
      congestion_re_attempt_indicator_5gsm_t::abo_type_::options::the_back_off_timer_is_applied_in_the_registered_plmn;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // congestion_re_attempt_indicator_5gsm_t

// IE: Re-attempt indicator
// Reference: 9.11.4.17
class re_attempt_indicator_t
{
public:
  bool eplmnc = false;
  bool ratc   = false;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // re_attempt_indicator_t

} // namespace nas_5g
} // namespace srsran
#endif
