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
#ifndef SRSRAN_NAS_5G_MSG_H
#define SRSRAN_NAS_5G_MSG_H
#include "nas_5g_ies.h"
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

/*
 * Message: Registration request.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class registration_request_t
{
public:
  // Mandatory fields
  registration_type_5gs_t registration_type_5gs;
  key_set_identifier_t    ng_ksi;
  mobile_identity_5gs_t   mobile_identity_5gs;

  // Optional fields
  bool non_current_native_nas_key_set_identifier_present = false;
  bool capability_5gmm_present                           = false;
  bool ue_security_capability_present                    = false;
  bool requested_nssai_present                           = false;
  bool last_visited_registered_tai_present               = false;
  bool s1_ue_network_capability_present                  = false;
  bool uplink_data_status_present                        = false;
  bool pdu_session_status_present                        = false;
  bool mico_indication_present                           = false;
  bool ue_status_present                                 = false;
  bool additional_guti_present                           = false;
  bool allowed_pdu_session_status_present                = false;
  bool ue_usage_setting_present                          = false;
  bool requested_drx_parameters_present                  = false;
  bool eps_nas_message_container_present                 = false;
  bool ladn_indication_present                           = false;
  bool payload_container_type_present                    = false;
  bool payload_container_present                         = false;
  bool network_slicing_indication_present                = false;
  bool update_type_5gs_present                           = false;
  bool mobile_station_classmark_2_present                = false;
  bool supported_codecs_present                          = false;
  bool nas_message_container_present                     = false;
  bool eps_bearer_context_status_present                 = false;
  bool requested_extended_drx_parameters_present         = false;
  bool t3324_value_present                               = false;
  bool ue_radio_capability_id_present                    = false;
  bool requested_mapped_nssai_present                    = false;
  bool additional_information_requested_present          = false;
  bool requested_wus_assistance_information_present      = false;
  bool n5gc_indication_present                           = false;
  bool requested_nb_n1_mode_drx_parameters_present       = false;

  key_set_identifier_t               non_current_native_nas_key_set_identifier;
  capability_5gmm_t                  capability_5gmm;
  ue_security_capability_t           ue_security_capability;
  nssai_t                            requested_nssai;
  tracking_area_identity_5gs_t       last_visited_registered_tai;
  s1_ue_network_capability_t         s1_ue_network_capability;
  uplink_data_status_t               uplink_data_status;
  pdu_session_status_t               pdu_session_status;
  mico_indication_t                  mico_indication;
  ue_status_t                        ue_status;
  mobile_identity_5gs_t              additional_guti;
  allowed_pdu_session_status_t       allowed_pdu_session_status;
  ue_usage_setting_t                 ue_usage_setting;
  drx_parameters_5gs_t               requested_drx_parameters;
  eps_nas_message_container_t        eps_nas_message_container;
  ladn_indication_t                  ladn_indication;
  payload_container_type_t           payload_container_type;
  payload_container_t                payload_container;
  network_slicing_indication_t       network_slicing_indication;
  update_type_5gs_t                  update_type_5gs;
  mobile_station_classmark_2_t       mobile_station_classmark_2;
  supported_codec_list_t             supported_codecs;
  message_container_t                nas_message_container;
  eps_bearer_context_status_t        eps_bearer_context_status;
  extended_drx_parameters_t          requested_extended_drx_parameters;
  gprs_timer_3_t                     t3324_value;
  ue_radio_capability_id_t           ue_radio_capability_id;
  mapped_nssai_t                     requested_mapped_nssai;
  additional_information_requested_t additional_information_requested;
  wus_assistance_information_t       requested_wus_assistance_information;
  n5gc_indication_t                  n5gc_indication;
  nb_n1_mode_drx_parameters_t        requested_nb_n1_mode_drx_parameters;

  const static uint8_t ie_iei_non_current_native_nas_key_set_identifier = 0xC;
  const static uint8_t ie_iei_capability_5gmm                           = 0x10;
  const static uint8_t ie_iei_ue_security_capability                    = 0x2E;
  const static uint8_t ie_iei_requested_nssai                           = 0x2F;
  const static uint8_t ie_iei_last_visited_registered_tai               = 0x52;
  const static uint8_t ie_iei_s1_ue_network_capability                  = 0x17;
  const static uint8_t ie_iei_uplink_data_status                        = 0x40;
  const static uint8_t ie_iei_pdu_session_status                        = 0x50;
  const static uint8_t ie_iei_mico_indication                           = 0xB;
  const static uint8_t ie_iei_ue_status                                 = 0x2B;
  const static uint8_t ie_iei_additional_guti                           = 0x77;
  const static uint8_t ie_iei_allowed_pdu_session_status                = 0x25;
  const static uint8_t ie_iei_ue_usage_setting                          = 0x18;
  const static uint8_t ie_iei_requested_drx_parameters                  = 0x51;
  const static uint8_t ie_iei_eps_nas_message_container                 = 0x70;
  const static uint8_t ie_iei_ladn_indication                           = 0x74;
  const static uint8_t ie_iei_payload_container_type                    = 0x8;
  const static uint8_t ie_iei_payload_container                         = 0x7B;
  const static uint8_t ie_iei_network_slicing_indication                = 0x9;
  const static uint8_t ie_iei_update_type_5gs                           = 0x53;
  const static uint8_t ie_iei_mobile_station_classmark_2                = 0x41;
  const static uint8_t ie_iei_supported_codecs                          = 0x42;
  const static uint8_t ie_iei_nas_message_container                     = 0x71;
  const static uint8_t ie_iei_eps_bearer_context_status                 = 0x60;
  const static uint8_t ie_iei_requested_extended_drx_parameters         = 0x6E;
  const static uint8_t ie_iei_t3324_value                               = 0x6A;
  const static uint8_t ie_iei_ue_radio_capability_id                    = 0x67;
  const static uint8_t ie_iei_requested_mapped_nssai                    = 0x35;
  const static uint8_t ie_iei_additional_information_requested          = 0x48;
  const static uint8_t ie_iei_requested_wus_assistance_information      = 0x1A;
  const static uint8_t ie_iei_n5gc_indication                           = 0xA;
  const static uint8_t ie_iei_requested_nb_n1_mode_drx_parameters       = 0x30;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // registration_request_t

/*
 * Message: Registration accept.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class registration_accept_t
{
public:
  // Mandatory fields
  registration_result_5gs_t registration_result_5gs;

  // Optional fields
  bool guti_5g_present                                      = false;
  bool equivalent_plm_ns_present                            = false;
  bool tai_list_present                                     = false;
  bool allowed_nssai_present                                = false;
  bool rejected_nssai_present                               = false;
  bool configured_nssai_present                             = false;
  bool network_feature_support_5gs_present                  = false;
  bool pdu_session_status_present                           = false;
  bool pdu_session_reactivation_result_present              = false;
  bool pdu_session_reactivation_result_error_cause_present  = false;
  bool ladn_information_present                             = false;
  bool mico_indication_present                              = false;
  bool network_slicing_indication_present                   = false;
  bool service_area_list_present                            = false;
  bool t3512_value_present                                  = false;
  bool non_3_gpp_de_registration_timer_value_present        = false;
  bool t3502_value_present                                  = false;
  bool emergency_number_list_present                        = false;
  bool extended_emergency_number_list_present               = false;
  bool sor_transparent_container_present                    = false;
  bool eap_message_present                                  = false;
  bool nssai_inclusion_mode_present                         = false;
  bool operator_defined_access_category_definitions_present = false;
  bool negotiated_drx_parameters_present                    = false;
  bool non_3_gpp_nw_policies_present                        = false;
  bool eps_bearer_context_status_present                    = false;
  bool negotiated_extended_drx_parameters_present           = false;
  bool t3447_value_present                                  = false;
  bool t3448_value_present                                  = false;
  bool t3324_value_present                                  = false;
  bool ue_radio_capability_id_present                       = false;
  bool ue_radio_capability_id_deletion_indication_present   = false;
  bool pending_nssai_present                                = false;
  bool ciphering_key_data_present                           = false;
  bool cag_information_list_present                         = false;
  bool truncated_5g_s_tmsi_configuration_present            = false;
  bool negotiated_wus_assistance_information_present        = false;
  bool negotiated_nb_n1_mode_drx_parameters_present         = false;

  mobile_identity_5gs_t                          guti_5g;
  plmn_list_t                                    equivalent_plm_ns;
  tracking_area_identity_list_5gs_t              tai_list;
  nssai_t                                        allowed_nssai;
  rejected_nssai_t                               rejected_nssai;
  nssai_t                                        configured_nssai;
  network_feature_support_5gs_t                  network_feature_support_5gs;
  pdu_session_status_t                           pdu_session_status;
  pdu_session_reactivation_result_t              pdu_session_reactivation_result;
  pdu_session_reactivation_result_error_cause_t  pdu_session_reactivation_result_error_cause;
  ladn_information_t                             ladn_information;
  mico_indication_t                              mico_indication;
  network_slicing_indication_t                   network_slicing_indication;
  service_area_list_t                            service_area_list;
  gprs_timer_3_t                                 t3512_value;
  gprs_timer_2_t                                 non_3_gpp_de_registration_timer_value;
  gprs_timer_2_t                                 t3502_value;
  emergency_number_list_t                        emergency_number_list;
  extended_emergency_number_list_t               extended_emergency_number_list;
  sor_transparent_container_t                    sor_transparent_container;
  eap_message_t                                  eap_message;
  nssai_inclusion_mode_t                         nssai_inclusion_mode;
  operator_defined_access_category_definitions_t operator_defined_access_category_definitions;
  drx_parameters_5gs_t                           negotiated_drx_parameters;
  non_3_gpp_nw_provided_policies_t               non_3_gpp_nw_policies;
  eps_bearer_context_status_t                    eps_bearer_context_status;
  extended_drx_parameters_t                      negotiated_extended_drx_parameters;
  gprs_timer_3_t                                 t3447_value;
  gprs_timer_2_t                                 t3448_value;
  gprs_timer_3_t                                 t3324_value;
  ue_radio_capability_id_t                       ue_radio_capability_id;
  ue_radio_capability_id_deletion_indication_t   ue_radio_capability_id_deletion_indication;
  nssai_t                                        pending_nssai;
  ciphering_key_data_t                           ciphering_key_data;
  cag_information_list_t                         cag_information_list;
  truncated_5g_s_tmsi_configuration_t            truncated_5g_s_tmsi_configuration;
  wus_assistance_information_t                   negotiated_wus_assistance_information;
  nb_n1_mode_drx_parameters_t                    negotiated_nb_n1_mode_drx_parameters;

  const static uint8_t ie_iei_guti_5g                                      = 0x77;
  const static uint8_t ie_iei_equivalent_plm_ns                            = 0x4A;
  const static uint8_t ie_iei_tai_list                                     = 0x54;
  const static uint8_t ie_iei_allowed_nssai                                = 0x15;
  const static uint8_t ie_iei_rejected_nssai                               = 0x11;
  const static uint8_t ie_iei_configured_nssai                             = 0x31;
  const static uint8_t ie_iei_network_feature_support_5gs                  = 0x21;
  const static uint8_t ie_iei_pdu_session_status                           = 0x50;
  const static uint8_t ie_iei_pdu_session_reactivation_result              = 0x26;
  const static uint8_t ie_iei_pdu_session_reactivation_result_error_cause  = 0x72;
  const static uint8_t ie_iei_ladn_information                             = 0x79;
  const static uint8_t ie_iei_mico_indication                              = 0xB;
  const static uint8_t ie_iei_network_slicing_indication                   = 0x9;
  const static uint8_t ie_iei_service_area_list                            = 0x27;
  const static uint8_t ie_iei_t3512_value                                  = 0x5E;
  const static uint8_t ie_iei_non_3_gpp_de_registration_timer_value        = 0x5D;
  const static uint8_t ie_iei_t3502_value                                  = 0x16;
  const static uint8_t ie_iei_emergency_number_list                        = 0x34;
  const static uint8_t ie_iei_extended_emergency_number_list               = 0x7A;
  const static uint8_t ie_iei_sor_transparent_container                    = 0x73;
  const static uint8_t ie_iei_eap_message                                  = 0x78;
  const static uint8_t ie_iei_nssai_inclusion_mode                         = 0xA;
  const static uint8_t ie_iei_operator_defined_access_category_definitions = 0x76;
  const static uint8_t ie_iei_negotiated_drx_parameters                    = 0x51;
  const static uint8_t ie_iei_non_3_gpp_nw_policies                        = 0xD;
  const static uint8_t ie_iei_eps_bearer_context_status                    = 0x60;
  const static uint8_t ie_iei_negotiated_extended_drx_parameters           = 0x6E;
  const static uint8_t ie_iei_t3447_value                                  = 0x6C;
  const static uint8_t ie_iei_t3448_value                                  = 0x6B;
  const static uint8_t ie_iei_t3324_value                                  = 0x6A;
  const static uint8_t ie_iei_ue_radio_capability_id                       = 0x67;
  const static uint8_t ie_iei_ue_radio_capability_id_deletion_indication   = 0xE;
  const static uint8_t ie_iei_pending_nssai                                = 0x39;
  const static uint8_t ie_iei_ciphering_key_data                           = 0x74;
  const static uint8_t ie_iei_cag_information_list                         = 0x75;
  const static uint8_t ie_iei_truncated_5g_s_tmsi_configuration            = 0x1B;
  const static uint8_t ie_iei_negotiated_wus_assistance_information        = 0x1C;
  const static uint8_t ie_iei_negotiated_nb_n1_mode_drx_parameters         = 0x29;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // registration_accept_t

/*
 * Message: Registration complete.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class registration_complete_t
{
public:
  // Mandatory fields

  // Optional fields
  bool sor_transparent_container_present = false;

  sor_transparent_container_t sor_transparent_container;

  const static uint8_t ie_iei_sor_transparent_container = 0x73;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // registration_complete_t

/*
 * Message: Registration reject.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class registration_reject_t
{
public:
  // Mandatory fields
  cause_5gmm_t cause_5gmm;

  // Optional fields
  bool t3346_value_present          = false;
  bool t3502_value_present          = false;
  bool eap_message_present          = false;
  bool rejected_nssai_present       = false;
  bool cag_information_list_present = false;

  gprs_timer_2_t         t3346_value;
  gprs_timer_2_t         t3502_value;
  eap_message_t          eap_message;
  rejected_nssai_t       rejected_nssai;
  cag_information_list_t cag_information_list;

  const static uint8_t ie_iei_t3346_value          = 0x5F;
  const static uint8_t ie_iei_t3502_value          = 0x16;
  const static uint8_t ie_iei_eap_message          = 0x78;
  const static uint8_t ie_iei_rejected_nssai       = 0x69;
  const static uint8_t ie_iei_cag_information_list = 0x75;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // registration_reject_t

/*
 * Message: Deregistration request UE originating.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class deregistration_request_ue_originating_t
{
public:
  // Mandatory fields
  de_registration_type_t de_registration_type;
  key_set_identifier_t   ng_ksi;
  mobile_identity_5gs_t  mobile_identity_5gs;

  // Optional fields

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // deregistration_request_ue_originating_t

/*
 * Message: Deregistration accept UE originating.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class deregistration_accept_ue_originating_t
{
public:
  // Mandatory fields

  // Optional fields

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // deregistration_accept_ue_originating_t

/*
 * Message: Deregistration request UE terminated.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class deregistration_request_ue_terminated_t
{
public:
  // Mandatory fields
  de_registration_type_t de_registration_type;
  spare_half_octet_t     spare_half_octet;

  // Optional fields
  bool cause_5gmm_present           = false;
  bool t3346_value_present          = false;
  bool rejected_nssai_present       = false;
  bool cag_information_list_present = false;

  cause_5gmm_t           cause_5gmm;
  gprs_timer_2_t         t3346_value;
  rejected_nssai_t       rejected_nssai;
  cag_information_list_t cag_information_list;

  const static uint8_t ie_iei_cause_5gmm           = 0x58;
  const static uint8_t ie_iei_t3346_value          = 0x5F;
  const static uint8_t ie_iei_rejected_nssai       = 0x6D;
  const static uint8_t ie_iei_cag_information_list = 0x75;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // deregistration_request_ue_terminated_t

/*
 * Message: Deregistration accept UE terminated.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class deregistration_accept_ue_terminated_t
{
public:
  // Mandatory fields

  // Optional fields

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // deregistration_accept_ue_terminated_t

/*
 * Message: Service request.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class service_request_t
{
public:
  // Mandatory fields
  key_set_identifier_t  ng_ksi;
  service_type_t        service_type;
  mobile_identity_5gs_t s_tmsi_5g;

  // Optional fields
  bool uplink_data_status_present         = false;
  bool pdu_session_status_present         = false;
  bool allowed_pdu_session_status_present = false;
  bool nas_message_container_present      = false;

  uplink_data_status_t         uplink_data_status;
  pdu_session_status_t         pdu_session_status;
  allowed_pdu_session_status_t allowed_pdu_session_status;
  message_container_t          nas_message_container;

  const static uint8_t ie_iei_uplink_data_status         = 0x40;
  const static uint8_t ie_iei_pdu_session_status         = 0x50;
  const static uint8_t ie_iei_allowed_pdu_session_status = 0x25;
  const static uint8_t ie_iei_nas_message_container      = 0x71;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // service_request_t

/*
 * Message: Service reject.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class service_reject_t
{
public:
  // Mandatory fields

  // Optional fields
  bool pdu_session_status_present                          = false;
  bool pdu_session_reactivation_result_present             = false;
  bool pdu_session_reactivation_result_error_cause_present = false;
  bool eap_message_present                                 = false;
  bool t3448_value_present                                 = false;

  pdu_session_status_t                          pdu_session_status;
  pdu_session_reactivation_result_t             pdu_session_reactivation_result;
  pdu_session_reactivation_result_error_cause_t pdu_session_reactivation_result_error_cause;
  eap_message_t                                 eap_message;
  gprs_timer_2_t                                t3448_value;

  const static uint8_t ie_iei_pdu_session_status                          = 0x50;
  const static uint8_t ie_iei_pdu_session_reactivation_result             = 0x26;
  const static uint8_t ie_iei_pdu_session_reactivation_result_error_cause = 0x72;
  const static uint8_t ie_iei_eap_message                                 = 0x78;
  const static uint8_t ie_iei_t3448_value                                 = 0x6B;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // service_reject_t

/*
 * Message: Service accept.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class service_accept_t
{
public:
  // Mandatory fields
  cause_5gmm_t cause_5gmm;

  // Optional fields
  bool pdu_session_status_present   = false;
  bool t3346_value_present          = false;
  bool eap_message_present          = false;
  bool t3448_value_present          = false;
  bool cag_information_list_present = false;

  pdu_session_status_t   pdu_session_status;
  gprs_timer_2_t         t3346_value;
  eap_message_t          eap_message;
  gprs_timer_2_t         t3448_value;
  cag_information_list_t cag_information_list;

  const static uint8_t ie_iei_pdu_session_status   = 0x50;
  const static uint8_t ie_iei_t3346_value          = 0x5F;
  const static uint8_t ie_iei_eap_message          = 0x78;
  const static uint8_t ie_iei_t3448_value          = 0x6B;
  const static uint8_t ie_iei_cag_information_list = 0x75;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // service_accept_t

/*
 * Message: Configuration update command.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class configuration_update_command_t
{
public:
  // Mandatory fields

  // Optional fields
  bool configuration_update_indication_present              = false;
  bool guti_5g_present                                      = false;
  bool tai_list_present                                     = false;
  bool allowed_nssai_present                                = false;
  bool service_area_list_present                            = false;
  bool full_name_for_network_present                        = false;
  bool short_name_for_network_present                       = false;
  bool local_time_zone_present                              = false;
  bool universal_time_and_local_time_zone_present           = false;
  bool network_daylight_saving_time_present                 = false;
  bool ladn_information_present                             = false;
  bool mico_indication_present                              = false;
  bool network_slicing_indication_present                   = false;
  bool configured_nssai_present                             = false;
  bool rejected_nssai_present                               = false;
  bool operator_defined_access_category_definitions_present = false;
  bool sms_indication_present                               = false;
  bool t3447_value_present                                  = false;
  bool cag_information_list_present                         = false;
  bool ue_radio_capability_id_present                       = false;
  bool ue_radio_capability_id_deletion_indication_present   = false;
  bool registration_result_5gs_present                      = false;
  bool truncated_5g_s_tmsi_configuration_present            = false;
  bool additional_configuration_indication_present          = false;

  configuration_update_indication_t              configuration_update_indication;
  mobile_identity_5gs_t                          guti_5g;
  tracking_area_identity_list_5gs_t              tai_list;
  nssai_t                                        allowed_nssai;
  service_area_list_t                            service_area_list;
  network_name_t                                 full_name_for_network;
  network_name_t                                 short_name_for_network;
  time_zone_t                                    local_time_zone;
  time_zone_and_time_t                           universal_time_and_local_time_zone;
  daylight_saving_time_t                         network_daylight_saving_time;
  ladn_information_t                             ladn_information;
  mico_indication_t                              mico_indication;
  network_slicing_indication_t                   network_slicing_indication;
  nssai_t                                        configured_nssai;
  rejected_nssai_t                               rejected_nssai;
  operator_defined_access_category_definitions_t operator_defined_access_category_definitions;
  sms_indication_t                               sms_indication;
  gprs_timer_3_t                                 t3447_value;
  cag_information_list_t                         cag_information_list;
  ue_radio_capability_id_t                       ue_radio_capability_id;
  ue_radio_capability_id_deletion_indication_t   ue_radio_capability_id_deletion_indication;
  registration_result_5gs_t                      registration_result_5gs;
  truncated_5g_s_tmsi_configuration_t            truncated_5g_s_tmsi_configuration;
  additional_configuration_indication_t          additional_configuration_indication;

  const static uint8_t ie_iei_configuration_update_indication              = 0xD;
  const static uint8_t ie_iei_guti_5g                                      = 0x77;
  const static uint8_t ie_iei_tai_list                                     = 0x54;
  const static uint8_t ie_iei_allowed_nssai                                = 0x15;
  const static uint8_t ie_iei_service_area_list                            = 0x27;
  const static uint8_t ie_iei_full_name_for_network                        = 0x43;
  const static uint8_t ie_iei_short_name_for_network                       = 0x45;
  const static uint8_t ie_iei_local_time_zone                              = 0x46;
  const static uint8_t ie_iei_universal_time_and_local_time_zone           = 0x47;
  const static uint8_t ie_iei_network_daylight_saving_time                 = 0x49;
  const static uint8_t ie_iei_ladn_information                             = 0x79;
  const static uint8_t ie_iei_mico_indication                              = 0xB;
  const static uint8_t ie_iei_network_slicing_indication                   = 0x9;
  const static uint8_t ie_iei_configured_nssai                             = 0x31;
  const static uint8_t ie_iei_rejected_nssai                               = 0x11;
  const static uint8_t ie_iei_operator_defined_access_category_definitions = 0x76;
  const static uint8_t ie_iei_sms_indication                               = 0xF;
  const static uint8_t ie_iei_t3447_value                                  = 0x6C;
  const static uint8_t ie_iei_cag_information_list                         = 0x75;
  const static uint8_t ie_iei_ue_radio_capability_id                       = 0x67;
  const static uint8_t ie_iei_ue_radio_capability_id_deletion_indication   = 0xA;
  const static uint8_t ie_iei_registration_result_5gs                      = 0x44;
  const static uint8_t ie_iei_truncated_5g_s_tmsi_configuration            = 0x1B;
  const static uint8_t ie_iei_additional_configuration_indication          = 0xC;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // configuration_update_command_t

/*
 * Message: Configuration update complete.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class configuration_update_complete_t
{
public:
  // Mandatory fields

  // Optional fields

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // configuration_update_complete_t

/*
 * Message: Authentication request.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class authentication_request_t
{
public:
  // Mandatory fields
  key_set_identifier_t ng_ksi;
  spare_half_octet_t   spare_half_octet;
  abba_t               abba;

  // Optional fields
  bool authentication_parameter_rand_present = false;
  bool authentication_parameter_autn_present = false;
  bool eap_message_present                   = false;

  authentication_parameter_rand_t authentication_parameter_rand;
  authentication_parameter_autn_t authentication_parameter_autn;
  eap_message_t                   eap_message;

  const static uint8_t ie_iei_authentication_parameter_rand = 0x21;
  const static uint8_t ie_iei_authentication_parameter_autn = 0x20;
  const static uint8_t ie_iei_eap_message                   = 0x78;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // authentication_request_t

/*
 * Message: Authentication response.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class authentication_response_t
{
public:
  // Mandatory fields

  // Optional fields
  bool authentication_response_parameter_present = false;
  bool eap_message_present                       = false;

  authentication_response_parameter_t authentication_response_parameter;
  eap_message_t                       eap_message;

  const static uint8_t ie_iei_authentication_response_parameter = 0x2D;
  const static uint8_t ie_iei_eap_message                       = 0x78;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // authentication_response_t

/*
 * Message: Authentication reject.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class authentication_reject_t
{
public:
  // Mandatory fields

  // Optional fields
  bool eap_message_present = false;

  eap_message_t eap_message;

  const static uint8_t ie_iei_eap_message = 0x78;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // authentication_reject_t

/*
 * Message: Authentication failure.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class authentication_failure_t
{
public:
  // Mandatory fields
  cause_5gmm_t cause_5gmm;

  // Optional fields
  bool authentication_failure_parameter_present = false;

  authentication_failure_parameter_t authentication_failure_parameter;

  const static uint8_t ie_iei_authentication_failure_parameter = 0x30;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // authentication_failure_t

/*
 * Message: Authentication result.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class authentication_result_t
{
public:
  // Mandatory fields
  key_set_identifier_t ng_ksi;
  spare_half_octet_t   spare_half_octet;
  eap_message_t        eap_message;

  // Optional fields
  bool abba_present = false;

  abba_t abba;

  const static uint8_t ie_iei_abba = 0x38;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // authentication_result_t

/*
 * Message: Identity request.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class identity_request_t
{
public:
  // Mandatory fields
  identity_type_5gs_t identity_type;
  spare_half_octet_t  spare_half_octet;

  // Optional fields

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // identity_request_t

/*
 * Message: Identity response.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class identity_response_t
{
public:
  // Mandatory fields
  mobile_identity_5gs_t mobile_identity;

  // Optional fields

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // identity_response_t

/*
 * Message: Security mode command.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class security_mode_command_t
{
public:
  // Mandatory fields
  security_algorithms_t    selected_nas_security_algorithms;
  key_set_identifier_t     ng_ksi;
  spare_half_octet_t       spare_half_octet;
  ue_security_capability_t replayed_ue_security_capabilities;

  // Optional fields
  bool imeisv_request_present                       = false;
  bool selected_eps_nas_security_algorithms_present = false;
  bool additional_5g_security_information_present   = false;
  bool eap_message_present                          = false;
  bool abba_present                                 = false;
  bool replayed_s1_ue_security_capabilities_present = false;

  imeisv_request_t                     imeisv_request;
  eps_nas_security_algorithms_t        selected_eps_nas_security_algorithms;
  additional_5g_security_information_t additional_5g_security_information;
  eap_message_t                        eap_message;
  abba_t                               abba;
  s1_ue_security_capability_t          replayed_s1_ue_security_capabilities;

  const static uint8_t ie_iei_imeisv_request                       = 0xE;
  const static uint8_t ie_iei_selected_eps_nas_security_algorithms = 0x57;
  const static uint8_t ie_iei_additional_5g_security_information   = 0x36;
  const static uint8_t ie_iei_eap_message                          = 0x78;
  const static uint8_t ie_iei_abba                                 = 0x38;
  const static uint8_t ie_iei_replayed_s1_ue_security_capabilities = 0x19;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // security_mode_command_t

/*
 * Message: Security mode complete.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class security_mode_complete_t
{
public:
  // Mandatory fields

  // Optional fields
  bool imeisv_present                = false;
  bool nas_message_container_present = false;
  bool non_imeisv_pei_present        = false;

  mobile_identity_5gs_t imeisv;
  message_container_t   nas_message_container;
  mobile_identity_5gs_t non_imeisv_pei;

  const static uint8_t ie_iei_imeisv                = 0x77;
  const static uint8_t ie_iei_nas_message_container = 0x71;
  const static uint8_t ie_iei_non_imeisv_pei        = 0x78;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // security_mode_complete_t

/*
 * Message: Security mode reject.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class security_mode_reject_t
{
public:
  // Mandatory fields
  cause_5gmm_t cause_5gmm;

  // Optional fields

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // security_mode_reject_t

/*
 * Message: Status 5GMM.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class status_5gmm_t
{
public:
  // Mandatory fields
  cause_5gmm_t cause_5gmm;

  // Optional fields

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // status_5gmm_t

/*
 * Message: Notification.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class notification_t
{
public:
  // Mandatory fields
  access_type_t      access_type;
  spare_half_octet_t spare_half_octet;

  // Optional fields

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // notification_t

/*
 * Message: Notification response.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class notification_response_t
{
public:
  // Mandatory fields

  // Optional fields
  bool pdu_session_status_present = false;

  pdu_session_status_t pdu_session_status;

  const static uint8_t ie_iei_pdu_session_status = 0x50;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // notification_response_t

/*
 * Message: UL NAS transport.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class ul_nas_transport_t
{
public:
  // Mandatory fields
  payload_container_type_t payload_container_type;
  spare_half_octet_t       spare_half_octet;
  payload_container_t      payload_container;

  // Optional fields
  bool pdu_session_id_present                = false;
  bool old_pdu_session_id_present            = false;
  bool request_type_present                  = false;
  bool s_nssai_present                       = false;
  bool dnn_present                           = false;
  bool additional_information_present        = false;
  bool ma_pdu_session_information_present    = false;
  bool release_assistance_indication_present = false;

  pdu_session_identity_2_t        pdu_session_id;
  pdu_session_identity_2_t        old_pdu_session_id;
  request_type_t                  request_type;
  s_nssai_t                       s_nssai;
  dnn_t                           dnn;
  additional_information_t        additional_information;
  ma_pdu_session_information_t    ma_pdu_session_information;
  release_assistance_indication_t release_assistance_indication;

  const static uint8_t ie_iei_pdu_session_id                = 0x12;
  const static uint8_t ie_iei_old_pdu_session_id            = 0x59;
  const static uint8_t ie_iei_request_type                  = 0x8;
  const static uint8_t ie_iei_s_nssai                       = 0x22;
  const static uint8_t ie_iei_dnn                           = 0x25;
  const static uint8_t ie_iei_additional_information        = 0x24;
  const static uint8_t ie_iei_ma_pdu_session_information    = 0xA;
  const static uint8_t ie_iei_release_assistance_indication = 0xF;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // ul_nas_transport_t

/*
 * Message: DL NAS transport .
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class dl_nas_transport_t
{
public:
  // Mandatory fields
  payload_container_type_t payload_container_type;
  spare_half_octet_t       spare_half_octet;
  payload_container_t      payload_container;

  // Optional fields
  bool pdu_session_id_present         = false;
  bool additional_information_present = false;
  bool cause_5gmm_present             = false;
  bool back_off_timer_value_present   = false;

  pdu_session_identity_2_t pdu_session_id;
  additional_information_t additional_information;
  cause_5gmm_t             cause_5gmm;
  gprs_timer_3_t           back_off_timer_value;

  const static uint8_t ie_iei_pdu_session_id         = 0x12;
  const static uint8_t ie_iei_additional_information = 0x24;
  const static uint8_t ie_iei_cause_5gmm             = 0x58;
  const static uint8_t ie_iei_back_off_timer_value   = 0x37;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // dl_nas_transport_t

/*
 * Message: PDU session establishment request.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_establishment_request_t
{
public:
  // Mandatory fields
  integrity_protection_maximum_data_rate_t integrity_protection_maximum_data_rate;

  // Optional fields
  bool pdu_session_type_present                           = false;
  bool ssc_mode_present                                   = false;
  bool capability_5gsm_present                            = false;
  bool maximum_number_of_supported_packet_filters_present = false;
  bool always_on_pdu_session_requested_present            = false;
  bool sm_pdu_dn_request_container_present                = false;
  bool extended_protocol_configuration_options_present    = false;
  bool ip_header_compression_configuration_present        = false;
  bool ds_tt__ethernet_port_mac_address_present           = false;
  bool ue_ds_tt_residence_time_present                    = false;
  bool port_management_information_container_present      = false;
  bool ethernet_header_compression_configuration_present  = false;
  bool suggested_interface_identifier_present             = false;

  pdu_session_type_t                           pdu_session_type;
  ssc_mode_t                                   ssc_mode;
  capability_5gsm_t                            capability_5gsm;
  maximum_number_of_supported_packet_filters_t maximum_number_of_supported_packet_filters;
  always_on_pdu_session_requested_t            always_on_pdu_session_requested;
  sm_pdu_dn_request_container_t                sm_pdu_dn_request_container;
  extended_protocol_configuration_options_t    extended_protocol_configuration_options;
  ip_header_compression_configuration_t        ip_header_compression_configuration;
  ds_tt__ethernet_port_mac_address_t           ds_tt__ethernet_port_mac_address;
  ue_ds_tt_residence_time_t                    ue_ds_tt_residence_time;
  port_management_information_container_t      port_management_information_container;
  ethernet_header_compression_configuration_t  ethernet_header_compression_configuration;
  pdu_address_t                                suggested_interface_identifier;

  const static uint8_t ie_iei_pdu_session_type                           = 0x9;
  const static uint8_t ie_iei_ssc_mode                                   = 0xA;
  const static uint8_t ie_iei_capability_5gsm                            = 0x28;
  const static uint8_t ie_iei_maximum_number_of_supported_packet_filters = 0x55;
  const static uint8_t ie_iei_always_on_pdu_session_requested            = 0xB;
  const static uint8_t ie_iei_sm_pdu_dn_request_container                = 0x39;
  const static uint8_t ie_iei_extended_protocol_configuration_options    = 0x7B;
  const static uint8_t ie_iei_ip_header_compression_configuration        = 0x66;
  const static uint8_t ie_iei_ds_tt__ethernet_port_mac_address           = 0x6E;
  const static uint8_t ie_iei_ue_ds_tt_residence_time                    = 0x6F;
  const static uint8_t ie_iei_port_management_information_container      = 0x74;
  const static uint8_t ie_iei_ethernet_header_compression_configuration  = 0x1F;
  const static uint8_t ie_iei_suggested_interface_identifier             = 0x29;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_establishment_request_t

/*
 * Message: PDU session establishment accept.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_establishment_accept_t
{
public:
  // Mandatory fields
  pdu_session_type_t selected_pdu_session_type;
  ssc_mode_t         selected_ssc_mode;
  qo_s_rules_t       authorized__qo_s_rules;
  session_ambr_t     session_ambr;

  // Optional fields
  bool cause_5gsm_present                                = false;
  bool pdu_address_present                               = false;
  bool rq_timer_value_present                            = false;
  bool s_nssai_present                                   = false;
  bool always_on_pdu_session_indication_present          = false;
  bool mapped_eps_bearer_contexts_present                = false;
  bool eap_message_present                               = false;
  bool authorized__qo_s_flow_descriptions_present        = false;
  bool extended_protocol_configuration_options_present   = false;
  bool dnn_present                                       = false;
  bool network_feature_support_5gsm_present              = false;
  bool serving_plmn_rate_control_present                 = false;
  bool atsss_container_present                           = false;
  bool control_plane_only_indication_present             = false;
  bool ip_header_compression_configuration_present       = false;
  bool ethernet_header_compression_configuration_present = false;

  cause_5gsm_t                                cause_5gsm;
  pdu_address_t                               pdu_address;
  gprs_timer_t                                rq_timer_value;
  s_nssai_t                                   s_nssai;
  always_on_pdu_session_indication_t          always_on_pdu_session_indication;
  mapped_eps_bearer_contexts_t                mapped_eps_bearer_contexts;
  eap_message_t                               eap_message;
  qo_s_flow_descriptions_t                    authorized__qo_s_flow_descriptions;
  extended_protocol_configuration_options_t   extended_protocol_configuration_options;
  dnn_t                                       dnn;
  network_feature_support_5gsm_t              network_feature_support_5gsm;
  serving_plmn_rate_control_t                 serving_plmn_rate_control;
  atsss_container_t                           atsss_container;
  control_plane_only_indication_t             control_plane_only_indication;
  ip_header_compression_configuration_t       ip_header_compression_configuration;
  ethernet_header_compression_configuration_t ethernet_header_compression_configuration;

  const static uint8_t ie_iei_cause_5gsm                                = 0x59;
  const static uint8_t ie_iei_pdu_address                               = 0x29;
  const static uint8_t ie_iei_rq_timer_value                            = 0x56;
  const static uint8_t ie_iei_s_nssai                                   = 0x22;
  const static uint8_t ie_iei_always_on_pdu_session_indication          = 0x8;
  const static uint8_t ie_iei_mapped_eps_bearer_contexts                = 0x75;
  const static uint8_t ie_iei_eap_message                               = 0x78;
  const static uint8_t ie_iei_authorized__qo_s_flow_descriptions        = 0x79;
  const static uint8_t ie_iei_extended_protocol_configuration_options   = 0x7B;
  const static uint8_t ie_iei_dnn                                       = 0x25;
  const static uint8_t ie_iei_network_feature_support_5gsm              = 0x17;
  const static uint8_t ie_iei_serving_plmn_rate_control                 = 0x18;
  const static uint8_t ie_iei_atsss_container                           = 0x77;
  const static uint8_t ie_iei_control_plane_only_indication             = 0xC;
  const static uint8_t ie_iei_ip_header_compression_configuration       = 0x66;
  const static uint8_t ie_iei_ethernet_header_compression_configuration = 0x1F;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_establishment_accept_t

/*
 * Message: PDU session establishment reject.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_establishment_reject_t
{
public:
  // Mandatory fields
  cause_5gsm_t cause_5gsm;

  // Optional fields
  bool back_off_timer_value_present                    = false;
  bool allowed_ssc_mode_present                        = false;
  bool eap_message_present                             = false;
  bool congestion_re_attempt_indicator_5gsm_present    = false;
  bool extended_protocol_configuration_options_present = false;
  bool re_attempt_indicator_present                    = false;

  gprs_timer_3_t                            back_off_timer_value;
  allowed_ssc_mode_t                        allowed_ssc_mode;
  eap_message_t                             eap_message;
  congestion_re_attempt_indicator_5gsm_t    congestion_re_attempt_indicator_5gsm;
  extended_protocol_configuration_options_t extended_protocol_configuration_options;
  re_attempt_indicator_t                    re_attempt_indicator;

  const static uint8_t ie_iei_back_off_timer_value                    = 0x37;
  const static uint8_t ie_iei_allowed_ssc_mode                        = 0xF;
  const static uint8_t ie_iei_eap_message                             = 0x78;
  const static uint8_t ie_iei_congestion_re_attempt_indicator_5gsm    = 0x61;
  const static uint8_t ie_iei_extended_protocol_configuration_options = 0x7B;
  const static uint8_t ie_iei_re_attempt_indicator                    = 0x1D;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_establishment_reject_t

/*
 * Message: PDU session authentication command.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_authentication_command_t
{
public:
  // Mandatory fields
  eap_message_t eap_message;

  // Optional fields
  bool extended_protocol_configuration_options_present = false;

  extended_protocol_configuration_options_t extended_protocol_configuration_options;

  const static uint8_t ie_iei_extended_protocol_configuration_options = 0x7B;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_authentication_command_t

/*
 * Message: PDU session authentication complete.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_authentication_complete_t
{
public:
  // Mandatory fields
  eap_message_t eap_message;

  // Optional fields
  bool extended_protocol_configuration_options_present = false;

  extended_protocol_configuration_options_t extended_protocol_configuration_options;

  const static uint8_t ie_iei_extended_protocol_configuration_options = 0x7B;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_authentication_complete_t

/*
 * Message: PDU session authentication result.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_authentication_result_t
{
public:
  // Mandatory fields

  // Optional fields
  bool eap_message_present                             = false;
  bool extended_protocol_configuration_options_present = false;

  eap_message_t                             eap_message;
  extended_protocol_configuration_options_t extended_protocol_configuration_options;

  const static uint8_t ie_iei_eap_message                             = 0x78;
  const static uint8_t ie_iei_extended_protocol_configuration_options = 0x7B;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_authentication_result_t

/*
 * Message: PDU session modification request.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_modification_request_t
{
public:
  // Mandatory fields

  // Optional fields
  bool capability_5gsm_present                            = false;
  bool cause_5gsm_present                                 = false;
  bool maximum_number_of_supported_packet_filters_present = false;
  bool always_on_pdu_session_requested_present            = false;
  bool integrity_protection_maximum_data_rate_present     = false;
  bool requested__qo_s_rules_present                      = false;
  bool requested__qo_s_flow_descriptions_present          = false;
  bool mapped_eps_bearer_contexts_present                 = false;
  bool extended_protocol_configuration_options_present    = false;
  bool port_management_information_container_present      = false;
  bool ip_header_compression_configuration_present        = false;
  bool ethernet_header_compression_configuration_present  = false;

  capability_5gsm_t                            capability_5gsm;
  cause_5gsm_t                                 cause_5gsm;
  maximum_number_of_supported_packet_filters_t maximum_number_of_supported_packet_filters;
  always_on_pdu_session_requested_t            always_on_pdu_session_requested;
  integrity_protection_maximum_data_rate_t     integrity_protection_maximum_data_rate;
  qo_s_rules_t                                 requested__qo_s_rules;
  qo_s_flow_descriptions_t                     requested__qo_s_flow_descriptions;
  mapped_eps_bearer_contexts_t                 mapped_eps_bearer_contexts;
  extended_protocol_configuration_options_t    extended_protocol_configuration_options;
  port_management_information_container_t      port_management_information_container;
  ip_header_compression_configuration_t        ip_header_compression_configuration;
  ethernet_header_compression_configuration_t  ethernet_header_compression_configuration;

  const static uint8_t ie_iei_capability_5gsm                            = 0x28;
  const static uint8_t ie_iei_cause_5gsm                                 = 0x59;
  const static uint8_t ie_iei_maximum_number_of_supported_packet_filters = 0x55;
  const static uint8_t ie_iei_always_on_pdu_session_requested            = 0xB;
  const static uint8_t ie_iei_integrity_protection_maximum_data_rate     = 0x13;
  const static uint8_t ie_iei_requested__qo_s_rules                      = 0x7A;
  const static uint8_t ie_iei_requested__qo_s_flow_descriptions          = 0x79;
  const static uint8_t ie_iei_mapped_eps_bearer_contexts                 = 0x75;
  const static uint8_t ie_iei_extended_protocol_configuration_options    = 0x7B;
  const static uint8_t ie_iei_port_management_information_container      = 0x74;
  const static uint8_t ie_iei_ip_header_compression_configuration        = 0x66;
  const static uint8_t ie_iei_ethernet_header_compression_configuration  = 0x1F;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_modification_request_t

/*
 * Message: PDU session modification reject.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_modification_reject_t
{
public:
  // Mandatory fields
  cause_5gsm_t cause_5gsm;

  // Optional fields
  bool back_off_timer_value_present                    = false;
  bool congestion_re_attempt_indicator_5gsm_present    = false;
  bool extended_protocol_configuration_options_present = false;
  bool re_attempt_indicator_present                    = false;

  gprs_timer_3_t                            back_off_timer_value;
  congestion_re_attempt_indicator_5gsm_t    congestion_re_attempt_indicator_5gsm;
  extended_protocol_configuration_options_t extended_protocol_configuration_options;
  re_attempt_indicator_t                    re_attempt_indicator;

  const static uint8_t ie_iei_back_off_timer_value                    = 0x37;
  const static uint8_t ie_iei_congestion_re_attempt_indicator_5gsm    = 0x61;
  const static uint8_t ie_iei_extended_protocol_configuration_options = 0x7B;
  const static uint8_t ie_iei_re_attempt_indicator                    = 0x1D;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_modification_reject_t

/*
 * Message: PDU session modification command.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_modification_command_t
{
public:
  // Mandatory fields

  // Optional fields
  bool cause_5gsm_present                                = false;
  bool session_ambr_present                              = false;
  bool rq_timer_value_present                            = false;
  bool always_on_pdu_session_indication_present          = false;
  bool authorized__qo_s_rules_present                    = false;
  bool mapped_eps_bearer_contexts_present                = false;
  bool authorized__qo_s_flow_descriptions_present        = false;
  bool extended_protocol_configuration_options_present   = false;
  bool atsss_container_present                           = false;
  bool ip_header_compression_configuration_present       = false;
  bool port_management_information_container_present     = false;
  bool serving_plmn_rate_control_present                 = false;
  bool ethernet_header_compression_configuration_present = false;

  cause_5gsm_t                                cause_5gsm;
  session_ambr_t                              session_ambr;
  gprs_timer_t                                rq_timer_value;
  always_on_pdu_session_indication_t          always_on_pdu_session_indication;
  qo_s_rules_t                                authorized__qo_s_rules;
  mapped_eps_bearer_contexts_t                mapped_eps_bearer_contexts;
  qo_s_flow_descriptions_t                    authorized__qo_s_flow_descriptions;
  extended_protocol_configuration_options_t   extended_protocol_configuration_options;
  atsss_container_t                           atsss_container;
  ip_header_compression_configuration_t       ip_header_compression_configuration;
  port_management_information_container_t     port_management_information_container;
  serving_plmn_rate_control_t                 serving_plmn_rate_control;
  ethernet_header_compression_configuration_t ethernet_header_compression_configuration;

  const static uint8_t ie_iei_cause_5gsm                                = 0x59;
  const static uint8_t ie_iei_session_ambr                              = 0x2A;
  const static uint8_t ie_iei_rq_timer_value                            = 0x56;
  const static uint8_t ie_iei_always_on_pdu_session_indication          = 0x8;
  const static uint8_t ie_iei_authorized__qo_s_rules                    = 0x7A;
  const static uint8_t ie_iei_mapped_eps_bearer_contexts                = 0x75;
  const static uint8_t ie_iei_authorized__qo_s_flow_descriptions        = 0x79;
  const static uint8_t ie_iei_extended_protocol_configuration_options   = 0x7B;
  const static uint8_t ie_iei_atsss_container                           = 0x77;
  const static uint8_t ie_iei_ip_header_compression_configuration       = 0x66;
  const static uint8_t ie_iei_port_management_information_container     = 0x74;
  const static uint8_t ie_iei_serving_plmn_rate_control                 = 0x1E;
  const static uint8_t ie_iei_ethernet_header_compression_configuration = 0x1F;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_modification_command_t

/*
 * Message: PDU session modification complete.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_modification_complete_t
{
public:
  // Mandatory fields

  // Optional fields
  bool extended_protocol_configuration_options_present = false;
  bool port_management_information_container_present   = false;

  extended_protocol_configuration_options_t extended_protocol_configuration_options;
  port_management_information_container_t   port_management_information_container;

  const static uint8_t ie_iei_extended_protocol_configuration_options = 0x7B;
  const static uint8_t ie_iei_port_management_information_container   = 0x74;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_modification_complete_t

/*
 * Message: PDU session modification command reject.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_modification_command_reject_t
{
public:
  // Mandatory fields
  cause_5gsm_t cause_5gsm;

  // Optional fields
  bool extended_protocol_configuration_options_present = false;

  extended_protocol_configuration_options_t extended_protocol_configuration_options;

  const static uint8_t ie_iei_extended_protocol_configuration_options = 0x7B;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_modification_command_reject_t

/*
 * Message: PDU session release request.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_release_request_t
{
public:
  // Mandatory fields

  // Optional fields
  bool cause_5gsm_present                              = false;
  bool extended_protocol_configuration_options_present = false;

  cause_5gsm_t                              cause_5gsm;
  extended_protocol_configuration_options_t extended_protocol_configuration_options;

  const static uint8_t ie_iei_cause_5gsm                              = 0x59;
  const static uint8_t ie_iei_extended_protocol_configuration_options = 0x7B;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_release_request_t

/*
 * Message: PDU session release reject.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_release_reject_t
{
public:
  // Mandatory fields
  cause_5gsm_t cause_5gsm;

  // Optional fields
  bool extended_protocol_configuration_options_present = false;

  extended_protocol_configuration_options_t extended_protocol_configuration_options;

  const static uint8_t ie_iei_extended_protocol_configuration_options = 0x7B;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_release_reject_t

/*
 * Message: PDU session release command.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_release_command_t
{
public:
  // Mandatory fields
  cause_5gsm_t cause_5gsm;

  // Optional fields
  bool back_off_timer_value_present                    = false;
  bool eap_message_present                             = false;
  bool congestion_re_attempt_indicator_5gsm_present    = false;
  bool extended_protocol_configuration_options_present = false;
  bool access_type_present                             = false;

  gprs_timer_3_t                            back_off_timer_value;
  eap_message_t                             eap_message;
  congestion_re_attempt_indicator_5gsm_t    congestion_re_attempt_indicator_5gsm;
  extended_protocol_configuration_options_t extended_protocol_configuration_options;
  access_type_t                             access_type;

  const static uint8_t ie_iei_back_off_timer_value                    = 0x37;
  const static uint8_t ie_iei_eap_message                             = 0x78;
  const static uint8_t ie_iei_congestion_re_attempt_indicator_5gsm    = 0x61;
  const static uint8_t ie_iei_extended_protocol_configuration_options = 0x7B;
  const static uint8_t ie_iei_access_type                             = 0xD;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_release_command_t

/*
 * Message: PDU session release complete.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class pdu_session_release_complete_t
{
public:
  // Mandatory fields

  // Optional fields
  bool cause_5gsm_present                              = false;
  bool extended_protocol_configuration_options_present = false;

  cause_5gsm_t                              cause_5gsm;
  extended_protocol_configuration_options_t extended_protocol_configuration_options;

  const static uint8_t ie_iei_cause_5gsm                              = 0x59;
  const static uint8_t ie_iei_extended_protocol_configuration_options = 0x7B;

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // pdu_session_release_complete_t

/*
 * Message: Status 5GSM.
 *          Based on 3GPP TS 24.501 v16.7.0
 */

class status_5gsm_t
{
public:
  // Mandatory fields
  cause_5gsm_t cause_5gsm;

  // Optional fields

public:
  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);

}; // status_5gsm_t
// Include from nas5g/infiles/nas_5g_msg.h.in

struct msg_opts {
  enum options {
    registration_request                    = 0x41,
    registration_accept                     = 0x42,
    registration_complete                   = 0x43,
    registration_reject                     = 0x44,
    deregistration_request_ue_originating   = 0x45,
    deregistration_accept_ue_originating    = 0x46,
    deregistration_request_ue_terminated    = 0x47,
    deregistration_accept_ue_terminated     = 0x48,
    service_request                         = 0x4c,
    service_reject                          = 0x4d,
    service_accept                          = 0x4e,
    configuration_update_command            = 0x54,
    configuration_update_complete           = 0x55,
    authentication_request                  = 0x56,
    authentication_response                 = 0x57,
    authentication_reject                   = 0x58,
    authentication_failure                  = 0x59,
    authentication_result                   = 0x5a,
    identity_request                        = 0x5b,
    identity_response                       = 0x5c,
    security_mode_command                   = 0x5d,
    security_mode_complete                  = 0x5e,
    security_mode_reject                    = 0x5f,
    status_5gmm                             = 0x64,
    notification                            = 0x65,
    notification_response                   = 0x66,
    ul_nas_transport                        = 0x67,
    dl_nas_transport                        = 0x68,
    pdu_session_establishment_request       = 0xc1,
    pdu_session_establishment_accept        = 0xc2,
    pdu_session_establishment_reject        = 0xc3,
    pdu_session_authentication_command      = 0xc5,
    pdu_session_authentication_complete     = 0xc6,
    pdu_session_authentication_result       = 0xc7,
    pdu_session_modification_request        = 0xc9,
    pdu_session_modification_reject         = 0xca,
    pdu_session_modification_command        = 0xcb,
    pdu_session_modification_complete       = 0xcc,
    pdu_session_modification_command_reject = 0xcd,
    pdu_session_release_request             = 0xd1,
    pdu_session_release_reject              = 0xd2,
    pdu_session_release_command             = 0xd3,
    pdu_session_release_complete            = 0xd4,
    status_5gsm                             = 0xd6,
    nulltype                                = 0xff,

  } value;
  const char* to_string() const
  {
    switch (value) {
      case registration_request:
        return "Registration request";
      case registration_accept:
        return "Registration accept";
      case registration_complete:
        return "Registration complete";
      case registration_reject:
        return "Registration reject";
      case deregistration_request_ue_originating:
        return "Deregistration request UE originating";
      case deregistration_accept_ue_originating:
        return "Deregistration accept UE originating";
      case deregistration_request_ue_terminated:
        return "Deregistration request UE terminated";
      case deregistration_accept_ue_terminated:
        return "Deregistration accept UE terminated";
      case service_request:
        return "Service request";
      case service_reject:
        return "Service reject";
      case service_accept:
        return "Service accept";
      case configuration_update_command:
        return "Configuration update command";
      case configuration_update_complete:
        return "Configuration update complete";
      case authentication_request:
        return "Authentication request";
      case authentication_response:
        return "Authentication response";
      case authentication_reject:
        return "Authentication reject";
      case authentication_failure:
        return "Authentication failure";
      case authentication_result:
        return "Authentication result";
      case identity_request:
        return "Identity request";
      case identity_response:
        return "Identity response";
      case security_mode_command:
        return "Security mode command";
      case security_mode_complete:
        return "Security mode complete";
      case security_mode_reject:
        return "Security mode reject";
      case status_5gmm:
        return "Status 5GMM";
      case notification:
        return "Notification";
      case notification_response:
        return "Notification response";
      case ul_nas_transport:
        return "UL NAS transport";
      case dl_nas_transport:
        return "DL NAS transport ";
      case pdu_session_establishment_request:
        return "PDU session establishment request";
      case pdu_session_establishment_accept:
        return "PDU session establishment accept";
      case pdu_session_establishment_reject:
        return "PDU session establishment reject";
      case pdu_session_authentication_command:
        return "PDU session authentication command";
      case pdu_session_authentication_complete:
        return "PDU session authentication complete";
      case pdu_session_authentication_result:
        return "PDU session authentication result";
      case pdu_session_modification_request:
        return "PDU session modification request";
      case pdu_session_modification_reject:
        return "PDU session modification reject";
      case pdu_session_modification_command:
        return "PDU session modification command";
      case pdu_session_modification_complete:
        return "PDU session modification complete";
      case pdu_session_modification_command_reject:
        return "PDU session modification command reject";
      case pdu_session_release_request:
        return "PDU session release request";
      case pdu_session_release_reject:
        return "PDU session release reject";
      case pdu_session_release_command:
        return "PDU session release command";
      case pdu_session_release_complete:
        return "PDU session release complete";
      case status_5gsm:
        return "Status 5GSM";
      default:
        return "Error";
    }
  }
};

typedef asn1::enumerated<msg_opts> msg_types;
struct nas_5gs_hdr {
  enum security_header_type_opts {
    plain_5gs_nas_message,
    integrity_protected,
    integrity_protected_and_ciphered,
    integrity_protected_with_new_5G_nas_context,
    integrity_protected_and_ciphered_with_new_5G_nas_context
  };

  enum extended_protocol_discriminator_opts {
    extended_protocol_discriminator_5gsm = 0x2e,
    extended_protocol_discriminator_5gmm = 0x7e,
  };

  // Outer
  extended_protocol_discriminator_opts extended_protocol_discriminator = extended_protocol_discriminator_5gsm;
  security_header_type_opts            security_header_type            = plain_5gs_nas_message;
  // Only valid if not plain
  uint8_t  sequence_number             = 0xff;
  uint32_t message_authentication_code = 0x00000000;
  // Only valid if 5gsm type
  uint8_t pdu_session_identity           = 0xff;
  uint8_t procedure_transaction_identity = 0xff;
  // Inner
  extended_protocol_discriminator_opts inner_extended_protocol_discriminator = extended_protocol_discriminator_5gsm;
  security_header_type_opts            inner_security_header_type            = plain_5gs_nas_message;

  msg_types message_type = msg_types::options::nulltype;

  SRSASN_CODE pack(asn1::bit_ref& bref);
  SRSASN_CODE pack_outer(asn1::bit_ref& bref);
  SRSASN_CODE unpack(asn1::cbit_ref& bref);
  SRSASN_CODE unpack_outer(asn1::cbit_ref& bref);
};

class nas_5gs_msg
{
public:
  nas_5gs_hdr hdr;

  SRSASN_CODE pack(unique_byte_buffer_t& buf);
  SRSASN_CODE pack(std::vector<uint8_t>& buf);
  SRSASN_CODE unpack(const unique_byte_buffer_t& buf);
  SRSASN_CODE unpack(const std::vector<uint8_t>& buf);
  SRSASN_CODE unpack_outer_hdr(const unique_byte_buffer_t& buf);
  SRSASN_CODE unpack_outer_hdr(const std::vector<uint8_t>& buf);

  void set(msg_types::options e = msg_types::nulltype) { hdr.message_type = e; };
  // Getters

  registration_request_t& registration_request()
  {
    asn1::assert_choice_type(msg_types::options::registration_request, hdr.message_type, "registration_request");
    return *srslog::detail::any_cast<registration_request_t>(&msg_container);
  }

  registration_accept_t& registration_accept()
  {
    asn1::assert_choice_type(msg_types::options::registration_accept, hdr.message_type, "registration_accept");
    return *srslog::detail::any_cast<registration_accept_t>(&msg_container);
  }

  registration_complete_t& registration_complete()
  {
    asn1::assert_choice_type(msg_types::options::registration_complete, hdr.message_type, "registration_complete");
    return *srslog::detail::any_cast<registration_complete_t>(&msg_container);
  }

  registration_reject_t& registration_reject()
  {
    asn1::assert_choice_type(msg_types::options::registration_reject, hdr.message_type, "registration_reject");
    return *srslog::detail::any_cast<registration_reject_t>(&msg_container);
  }

  deregistration_request_ue_originating_t& deregistration_request_ue_originating()
  {
    asn1::assert_choice_type(msg_types::options::deregistration_request_ue_originating,
                             hdr.message_type,
                             "deregistration_request_ue_originating");
    return *srslog::detail::any_cast<deregistration_request_ue_originating_t>(&msg_container);
  }

  deregistration_accept_ue_originating_t& deregistration_accept_ue_originating()
  {
    asn1::assert_choice_type(msg_types::options::deregistration_accept_ue_originating,
                             hdr.message_type,
                             "deregistration_accept_ue_originating");
    return *srslog::detail::any_cast<deregistration_accept_ue_originating_t>(&msg_container);
  }

  deregistration_request_ue_terminated_t& deregistration_request_ue_terminated()
  {
    asn1::assert_choice_type(msg_types::options::deregistration_request_ue_terminated,
                             hdr.message_type,
                             "deregistration_request_ue_terminated");
    return *srslog::detail::any_cast<deregistration_request_ue_terminated_t>(&msg_container);
  }

  deregistration_accept_ue_terminated_t& deregistration_accept_ue_terminated()
  {
    asn1::assert_choice_type(msg_types::options::deregistration_accept_ue_terminated,
                             hdr.message_type,
                             "deregistration_accept_ue_terminated");
    return *srslog::detail::any_cast<deregistration_accept_ue_terminated_t>(&msg_container);
  }

  service_request_t& service_request()
  {
    asn1::assert_choice_type(msg_types::options::service_request, hdr.message_type, "service_request");
    return *srslog::detail::any_cast<service_request_t>(&msg_container);
  }

  service_reject_t& service_reject()
  {
    asn1::assert_choice_type(msg_types::options::service_reject, hdr.message_type, "service_reject");
    return *srslog::detail::any_cast<service_reject_t>(&msg_container);
  }

  service_accept_t& service_accept()
  {
    asn1::assert_choice_type(msg_types::options::service_accept, hdr.message_type, "service_accept");
    return *srslog::detail::any_cast<service_accept_t>(&msg_container);
  }

  configuration_update_command_t& configuration_update_command()
  {
    asn1::assert_choice_type(
        msg_types::options::configuration_update_command, hdr.message_type, "configuration_update_command");
    return *srslog::detail::any_cast<configuration_update_command_t>(&msg_container);
  }

  configuration_update_complete_t& configuration_update_complete()
  {
    asn1::assert_choice_type(
        msg_types::options::configuration_update_complete, hdr.message_type, "configuration_update_complete");
    return *srslog::detail::any_cast<configuration_update_complete_t>(&msg_container);
  }

  authentication_request_t& authentication_request()
  {
    asn1::assert_choice_type(msg_types::options::authentication_request, hdr.message_type, "authentication_request");
    return *srslog::detail::any_cast<authentication_request_t>(&msg_container);
  }

  authentication_response_t& authentication_response()
  {
    asn1::assert_choice_type(msg_types::options::authentication_response, hdr.message_type, "authentication_response");
    return *srslog::detail::any_cast<authentication_response_t>(&msg_container);
  }

  authentication_reject_t& authentication_reject()
  {
    asn1::assert_choice_type(msg_types::options::authentication_reject, hdr.message_type, "authentication_reject");
    return *srslog::detail::any_cast<authentication_reject_t>(&msg_container);
  }

  authentication_failure_t& authentication_failure()
  {
    asn1::assert_choice_type(msg_types::options::authentication_failure, hdr.message_type, "authentication_failure");
    return *srslog::detail::any_cast<authentication_failure_t>(&msg_container);
  }

  authentication_result_t& authentication_result()
  {
    asn1::assert_choice_type(msg_types::options::authentication_result, hdr.message_type, "authentication_result");
    return *srslog::detail::any_cast<authentication_result_t>(&msg_container);
  }

  identity_request_t& identity_request()
  {
    asn1::assert_choice_type(msg_types::options::identity_request, hdr.message_type, "identity_request");
    return *srslog::detail::any_cast<identity_request_t>(&msg_container);
  }

  identity_response_t& identity_response()
  {
    asn1::assert_choice_type(msg_types::options::identity_response, hdr.message_type, "identity_response");
    return *srslog::detail::any_cast<identity_response_t>(&msg_container);
  }

  security_mode_command_t& security_mode_command()
  {
    asn1::assert_choice_type(msg_types::options::security_mode_command, hdr.message_type, "security_mode_command");
    return *srslog::detail::any_cast<security_mode_command_t>(&msg_container);
  }

  security_mode_complete_t& security_mode_complete()
  {
    asn1::assert_choice_type(msg_types::options::security_mode_complete, hdr.message_type, "security_mode_complete");
    return *srslog::detail::any_cast<security_mode_complete_t>(&msg_container);
  }

  security_mode_reject_t& security_mode_reject()
  {
    asn1::assert_choice_type(msg_types::options::security_mode_reject, hdr.message_type, "security_mode_reject");
    return *srslog::detail::any_cast<security_mode_reject_t>(&msg_container);
  }

  status_5gmm_t& status_5gmm()
  {
    asn1::assert_choice_type(msg_types::options::status_5gmm, hdr.message_type, "status_5gmm");
    return *srslog::detail::any_cast<status_5gmm_t>(&msg_container);
  }

  notification_t& notification()
  {
    asn1::assert_choice_type(msg_types::options::notification, hdr.message_type, "notification");
    return *srslog::detail::any_cast<notification_t>(&msg_container);
  }

  notification_response_t& notification_response()
  {
    asn1::assert_choice_type(msg_types::options::notification_response, hdr.message_type, "notification_response");
    return *srslog::detail::any_cast<notification_response_t>(&msg_container);
  }

  ul_nas_transport_t& ul_nas_transport()
  {
    asn1::assert_choice_type(msg_types::options::ul_nas_transport, hdr.message_type, "ul_nas_transport");
    return *srslog::detail::any_cast<ul_nas_transport_t>(&msg_container);
  }

  dl_nas_transport_t& dl_nas_transport()
  {
    asn1::assert_choice_type(msg_types::options::dl_nas_transport, hdr.message_type, "dl_nas_transport");
    return *srslog::detail::any_cast<dl_nas_transport_t>(&msg_container);
  }

  pdu_session_establishment_request_t& pdu_session_establishment_request()
  {
    asn1::assert_choice_type(
        msg_types::options::pdu_session_establishment_request, hdr.message_type, "pdu_session_establishment_request");
    return *srslog::detail::any_cast<pdu_session_establishment_request_t>(&msg_container);
  }

  pdu_session_establishment_accept_t& pdu_session_establishment_accept()
  {
    asn1::assert_choice_type(
        msg_types::options::pdu_session_establishment_accept, hdr.message_type, "pdu_session_establishment_accept");
    return *srslog::detail::any_cast<pdu_session_establishment_accept_t>(&msg_container);
  }

  pdu_session_establishment_reject_t& pdu_session_establishment_reject()
  {
    asn1::assert_choice_type(
        msg_types::options::pdu_session_establishment_reject, hdr.message_type, "pdu_session_establishment_reject");
    return *srslog::detail::any_cast<pdu_session_establishment_reject_t>(&msg_container);
  }

  pdu_session_authentication_command_t& pdu_session_authentication_command()
  {
    asn1::assert_choice_type(
        msg_types::options::pdu_session_authentication_command, hdr.message_type, "pdu_session_authentication_command");
    return *srslog::detail::any_cast<pdu_session_authentication_command_t>(&msg_container);
  }

  pdu_session_authentication_complete_t& pdu_session_authentication_complete()
  {
    asn1::assert_choice_type(msg_types::options::pdu_session_authentication_complete,
                             hdr.message_type,
                             "pdu_session_authentication_complete");
    return *srslog::detail::any_cast<pdu_session_authentication_complete_t>(&msg_container);
  }

  pdu_session_authentication_result_t& pdu_session_authentication_result()
  {
    asn1::assert_choice_type(
        msg_types::options::pdu_session_authentication_result, hdr.message_type, "pdu_session_authentication_result");
    return *srslog::detail::any_cast<pdu_session_authentication_result_t>(&msg_container);
  }

  pdu_session_modification_request_t& pdu_session_modification_request()
  {
    asn1::assert_choice_type(
        msg_types::options::pdu_session_modification_request, hdr.message_type, "pdu_session_modification_request");
    return *srslog::detail::any_cast<pdu_session_modification_request_t>(&msg_container);
  }

  pdu_session_modification_reject_t& pdu_session_modification_reject()
  {
    asn1::assert_choice_type(
        msg_types::options::pdu_session_modification_reject, hdr.message_type, "pdu_session_modification_reject");
    return *srslog::detail::any_cast<pdu_session_modification_reject_t>(&msg_container);
  }

  pdu_session_modification_command_t& pdu_session_modification_command()
  {
    asn1::assert_choice_type(
        msg_types::options::pdu_session_modification_command, hdr.message_type, "pdu_session_modification_command");
    return *srslog::detail::any_cast<pdu_session_modification_command_t>(&msg_container);
  }

  pdu_session_modification_complete_t& pdu_session_modification_complete()
  {
    asn1::assert_choice_type(
        msg_types::options::pdu_session_modification_complete, hdr.message_type, "pdu_session_modification_complete");
    return *srslog::detail::any_cast<pdu_session_modification_complete_t>(&msg_container);
  }

  pdu_session_modification_command_reject_t& pdu_session_modification_command_reject()
  {
    asn1::assert_choice_type(msg_types::options::pdu_session_modification_command_reject,
                             hdr.message_type,
                             "pdu_session_modification_command_reject");
    return *srslog::detail::any_cast<pdu_session_modification_command_reject_t>(&msg_container);
  }

  pdu_session_release_request_t& pdu_session_release_request()
  {
    asn1::assert_choice_type(
        msg_types::options::pdu_session_release_request, hdr.message_type, "pdu_session_release_request");
    return *srslog::detail::any_cast<pdu_session_release_request_t>(&msg_container);
  }

  pdu_session_release_reject_t& pdu_session_release_reject()
  {
    asn1::assert_choice_type(
        msg_types::options::pdu_session_release_reject, hdr.message_type, "pdu_session_release_reject");
    return *srslog::detail::any_cast<pdu_session_release_reject_t>(&msg_container);
  }

  pdu_session_release_command_t& pdu_session_release_command()
  {
    asn1::assert_choice_type(
        msg_types::options::pdu_session_release_command, hdr.message_type, "pdu_session_release_command");
    return *srslog::detail::any_cast<pdu_session_release_command_t>(&msg_container);
  }

  pdu_session_release_complete_t& pdu_session_release_complete()
  {
    asn1::assert_choice_type(
        msg_types::options::pdu_session_release_complete, hdr.message_type, "pdu_session_release_complete");
    return *srslog::detail::any_cast<pdu_session_release_complete_t>(&msg_container);
  }

  status_5gsm_t& status_5gsm()
  {
    asn1::assert_choice_type(msg_types::options::status_5gsm, hdr.message_type, "status_5gsm");
    return *srslog::detail::any_cast<status_5gsm_t>(&msg_container);
  }

  // Setters

  registration_request_t& set_registration_request()
  {
    set(msg_types::options::registration_request);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{registration_request_t()};
    return *srslog::detail::any_cast<registration_request_t>(&msg_container);
  }
  registration_accept_t& set_registration_accept()
  {
    set(msg_types::options::registration_accept);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{registration_accept_t()};
    return *srslog::detail::any_cast<registration_accept_t>(&msg_container);
  }
  registration_complete_t& set_registration_complete()
  {
    set(msg_types::options::registration_complete);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{registration_complete_t()};
    return *srslog::detail::any_cast<registration_complete_t>(&msg_container);
  }
  registration_reject_t& set_registration_reject()
  {
    set(msg_types::options::registration_reject);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{registration_reject_t()};
    return *srslog::detail::any_cast<registration_reject_t>(&msg_container);
  }
  deregistration_request_ue_originating_t& set_deregistration_request_ue_originating()
  {
    set(msg_types::options::deregistration_request_ue_originating);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{deregistration_request_ue_originating_t()};
    return *srslog::detail::any_cast<deregistration_request_ue_originating_t>(&msg_container);
  }
  deregistration_accept_ue_originating_t& set_deregistration_accept_ue_originating()
  {
    set(msg_types::options::deregistration_accept_ue_originating);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{deregistration_accept_ue_originating_t()};
    return *srslog::detail::any_cast<deregistration_accept_ue_originating_t>(&msg_container);
  }
  deregistration_request_ue_terminated_t& set_deregistration_request_ue_terminated()
  {
    set(msg_types::options::deregistration_request_ue_terminated);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{deregistration_request_ue_terminated_t()};
    return *srslog::detail::any_cast<deregistration_request_ue_terminated_t>(&msg_container);
  }
  deregistration_accept_ue_terminated_t& set_deregistration_accept_ue_terminated()
  {
    set(msg_types::options::deregistration_accept_ue_terminated);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{deregistration_accept_ue_terminated_t()};
    return *srslog::detail::any_cast<deregistration_accept_ue_terminated_t>(&msg_container);
  }
  service_request_t& set_service_request()
  {
    set(msg_types::options::service_request);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{service_request_t()};
    return *srslog::detail::any_cast<service_request_t>(&msg_container);
  }
  service_reject_t& set_service_reject()
  {
    set(msg_types::options::service_reject);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{service_reject_t()};
    return *srslog::detail::any_cast<service_reject_t>(&msg_container);
  }
  service_accept_t& set_service_accept()
  {
    set(msg_types::options::service_accept);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{service_accept_t()};
    return *srslog::detail::any_cast<service_accept_t>(&msg_container);
  }
  configuration_update_command_t& set_configuration_update_command()
  {
    set(msg_types::options::configuration_update_command);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{configuration_update_command_t()};
    return *srslog::detail::any_cast<configuration_update_command_t>(&msg_container);
  }
  configuration_update_complete_t& set_configuration_update_complete()
  {
    set(msg_types::options::configuration_update_complete);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{configuration_update_complete_t()};
    return *srslog::detail::any_cast<configuration_update_complete_t>(&msg_container);
  }
  authentication_request_t& set_authentication_request()
  {
    set(msg_types::options::authentication_request);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{authentication_request_t()};
    return *srslog::detail::any_cast<authentication_request_t>(&msg_container);
  }
  authentication_response_t& set_authentication_response()
  {
    set(msg_types::options::authentication_response);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{authentication_response_t()};
    return *srslog::detail::any_cast<authentication_response_t>(&msg_container);
  }
  authentication_reject_t& set_authentication_reject()
  {
    set(msg_types::options::authentication_reject);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{authentication_reject_t()};
    return *srslog::detail::any_cast<authentication_reject_t>(&msg_container);
  }
  authentication_failure_t& set_authentication_failure()
  {
    set(msg_types::options::authentication_failure);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{authentication_failure_t()};
    return *srslog::detail::any_cast<authentication_failure_t>(&msg_container);
  }
  authentication_result_t& set_authentication_result()
  {
    set(msg_types::options::authentication_result);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{authentication_result_t()};
    return *srslog::detail::any_cast<authentication_result_t>(&msg_container);
  }
  identity_request_t& set_identity_request()
  {
    set(msg_types::options::identity_request);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{identity_request_t()};
    return *srslog::detail::any_cast<identity_request_t>(&msg_container);
  }
  identity_response_t& set_identity_response()
  {
    set(msg_types::options::identity_response);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{identity_response_t()};
    return *srslog::detail::any_cast<identity_response_t>(&msg_container);
  }
  security_mode_command_t& set_security_mode_command()
  {
    set(msg_types::options::security_mode_command);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{security_mode_command_t()};
    return *srslog::detail::any_cast<security_mode_command_t>(&msg_container);
  }
  security_mode_complete_t& set_security_mode_complete()
  {
    set(msg_types::options::security_mode_complete);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{security_mode_complete_t()};
    return *srslog::detail::any_cast<security_mode_complete_t>(&msg_container);
  }
  security_mode_reject_t& set_security_mode_reject()
  {
    set(msg_types::options::security_mode_reject);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{security_mode_reject_t()};
    return *srslog::detail::any_cast<security_mode_reject_t>(&msg_container);
  }
  status_5gmm_t& set_status_5gmm()
  {
    set(msg_types::options::status_5gmm);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{status_5gmm_t()};
    return *srslog::detail::any_cast<status_5gmm_t>(&msg_container);
  }
  notification_t& set_notification()
  {
    set(msg_types::options::notification);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{notification_t()};
    return *srslog::detail::any_cast<notification_t>(&msg_container);
  }
  notification_response_t& set_notification_response()
  {
    set(msg_types::options::notification_response);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{notification_response_t()};
    return *srslog::detail::any_cast<notification_response_t>(&msg_container);
  }
  ul_nas_transport_t& set_ul_nas_transport()
  {
    set(msg_types::options::ul_nas_transport);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{ul_nas_transport_t()};
    return *srslog::detail::any_cast<ul_nas_transport_t>(&msg_container);
  }
  dl_nas_transport_t& set_dl_nas_transport()
  {
    set(msg_types::options::dl_nas_transport);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gmm;
    msg_container                             = srslog::detail::any{dl_nas_transport_t()};
    return *srslog::detail::any_cast<dl_nas_transport_t>(&msg_container);
  }
  pdu_session_establishment_request_t& set_pdu_session_establishment_request()
  {
    set(msg_types::options::pdu_session_establishment_request);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_establishment_request_t()};
    return *srslog::detail::any_cast<pdu_session_establishment_request_t>(&msg_container);
  }
  pdu_session_establishment_accept_t& set_pdu_session_establishment_accept()
  {
    set(msg_types::options::pdu_session_establishment_accept);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_establishment_accept_t()};
    return *srslog::detail::any_cast<pdu_session_establishment_accept_t>(&msg_container);
  }
  pdu_session_establishment_reject_t& set_pdu_session_establishment_reject()
  {
    set(msg_types::options::pdu_session_establishment_reject);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_establishment_reject_t()};
    return *srslog::detail::any_cast<pdu_session_establishment_reject_t>(&msg_container);
  }
  pdu_session_authentication_command_t& set_pdu_session_authentication_command()
  {
    set(msg_types::options::pdu_session_authentication_command);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_authentication_command_t()};
    return *srslog::detail::any_cast<pdu_session_authentication_command_t>(&msg_container);
  }
  pdu_session_authentication_complete_t& set_pdu_session_authentication_complete()
  {
    set(msg_types::options::pdu_session_authentication_complete);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_authentication_complete_t()};
    return *srslog::detail::any_cast<pdu_session_authentication_complete_t>(&msg_container);
  }
  pdu_session_authentication_result_t& set_pdu_session_authentication_result()
  {
    set(msg_types::options::pdu_session_authentication_result);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_authentication_result_t()};
    return *srslog::detail::any_cast<pdu_session_authentication_result_t>(&msg_container);
  }
  pdu_session_modification_request_t& set_pdu_session_modification_request()
  {
    set(msg_types::options::pdu_session_modification_request);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_modification_request_t()};
    return *srslog::detail::any_cast<pdu_session_modification_request_t>(&msg_container);
  }
  pdu_session_modification_reject_t& set_pdu_session_modification_reject()
  {
    set(msg_types::options::pdu_session_modification_reject);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_modification_reject_t()};
    return *srslog::detail::any_cast<pdu_session_modification_reject_t>(&msg_container);
  }
  pdu_session_modification_command_t& set_pdu_session_modification_command()
  {
    set(msg_types::options::pdu_session_modification_command);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_modification_command_t()};
    return *srslog::detail::any_cast<pdu_session_modification_command_t>(&msg_container);
  }
  pdu_session_modification_complete_t& set_pdu_session_modification_complete()
  {
    set(msg_types::options::pdu_session_modification_complete);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_modification_complete_t()};
    return *srslog::detail::any_cast<pdu_session_modification_complete_t>(&msg_container);
  }
  pdu_session_modification_command_reject_t& set_pdu_session_modification_command_reject()
  {
    set(msg_types::options::pdu_session_modification_command_reject);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_modification_command_reject_t()};
    return *srslog::detail::any_cast<pdu_session_modification_command_reject_t>(&msg_container);
  }
  pdu_session_release_request_t& set_pdu_session_release_request()
  {
    set(msg_types::options::pdu_session_release_request);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_release_request_t()};
    return *srslog::detail::any_cast<pdu_session_release_request_t>(&msg_container);
  }
  pdu_session_release_reject_t& set_pdu_session_release_reject()
  {
    set(msg_types::options::pdu_session_release_reject);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_release_reject_t()};
    return *srslog::detail::any_cast<pdu_session_release_reject_t>(&msg_container);
  }
  pdu_session_release_command_t& set_pdu_session_release_command()
  {
    set(msg_types::options::pdu_session_release_command);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_release_command_t()};
    return *srslog::detail::any_cast<pdu_session_release_command_t>(&msg_container);
  }
  pdu_session_release_complete_t& set_pdu_session_release_complete()
  {
    set(msg_types::options::pdu_session_release_complete);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{pdu_session_release_complete_t()};
    return *srslog::detail::any_cast<pdu_session_release_complete_t>(&msg_container);
  }
  status_5gsm_t& set_status_5gsm()
  {
    set(msg_types::options::status_5gsm);
    hdr.extended_protocol_discriminator       = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    hdr.inner_extended_protocol_discriminator = nas_5gs_hdr::extended_protocol_discriminator_5gsm;
    msg_container                             = srslog::detail::any{status_5gsm_t()};
    return *srslog::detail::any_cast<status_5gsm_t>(&msg_container);
  }

private:
  SRSASN_CODE         unpack(asn1::cbit_ref& bref);
  SRSASN_CODE         pack(asn1::bit_ref& bref);
  srslog::detail::any msg_container = srslog::detail::any{registration_request_t()};
};
} // namespace nas_5g
} // namespace srsran
#endif
