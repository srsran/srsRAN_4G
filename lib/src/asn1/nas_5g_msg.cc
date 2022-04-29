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
#include "srsran/asn1/nas_5g_msg.h"
#include "srsran/asn1/nas_5g_ies.h"
#include "srsran/asn1/nas_5g_utils.h"

#include "srsran/asn1/asn1_utils.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/config.h"

#include <array>
#include <stdint.h>
#include <vector>

namespace srsran {
namespace nas_5g {

SRSASN_CODE registration_request_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(ng_ksi.pack(bref));
  HANDLE_CODE(registration_type_5gs.pack(bref));
  HANDLE_CODE(mobile_identity_5gs.pack(bref));

  // Optional fields
  if (non_current_native_nas_key_set_identifier_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_non_current_native_nas_key_set_identifier, 4));
    HANDLE_CODE(non_current_native_nas_key_set_identifier.pack(bref));
  }
  if (capability_5gmm_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_capability_5gmm, 8));
    HANDLE_CODE(capability_5gmm.pack(bref));
  }
  if (ue_security_capability_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ue_security_capability, 8));
    HANDLE_CODE(ue_security_capability.pack(bref));
  }
  if (requested_nssai_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_requested_nssai, 8));
    HANDLE_CODE(requested_nssai.pack(bref));
  }
  if (last_visited_registered_tai_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_last_visited_registered_tai, 8));
    HANDLE_CODE(last_visited_registered_tai.pack(bref));
  }
  if (s1_ue_network_capability_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_s1_ue_network_capability, 8));
    HANDLE_CODE(s1_ue_network_capability.pack(bref));
  }
  if (uplink_data_status_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_uplink_data_status, 8));
    HANDLE_CODE(uplink_data_status.pack(bref));
  }
  if (pdu_session_status_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pdu_session_status, 8));
    HANDLE_CODE(pdu_session_status.pack(bref));
  }
  if (mico_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_mico_indication, 4));
    HANDLE_CODE(mico_indication.pack(bref));
  }
  if (ue_status_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ue_status, 8));
    HANDLE_CODE(ue_status.pack(bref));
  }
  if (additional_guti_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_additional_guti, 8));
    HANDLE_CODE(additional_guti.pack(bref));
  }
  if (allowed_pdu_session_status_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_allowed_pdu_session_status, 8));
    HANDLE_CODE(allowed_pdu_session_status.pack(bref));
  }
  if (ue_usage_setting_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ue_usage_setting, 8));
    HANDLE_CODE(ue_usage_setting.pack(bref));
  }
  if (requested_drx_parameters_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_requested_drx_parameters, 8));
    HANDLE_CODE(requested_drx_parameters.pack(bref));
  }
  if (eps_nas_message_container_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eps_nas_message_container, 8));
    HANDLE_CODE(eps_nas_message_container.pack(bref));
  }
  if (ladn_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ladn_indication, 8));
    HANDLE_CODE(ladn_indication.pack(bref));
  }
  if (payload_container_type_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_payload_container_type, 4));
    HANDLE_CODE(payload_container_type.pack(bref));
  }
  if (payload_container_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_payload_container, 8));
    HANDLE_CODE(payload_container.pack(bref));
  }
  if (network_slicing_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_network_slicing_indication, 4));
    HANDLE_CODE(network_slicing_indication.pack(bref));
  }
  if (update_type_5gs_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_update_type_5gs, 8));
    HANDLE_CODE(update_type_5gs.pack(bref));
  }
  if (mobile_station_classmark_2_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_mobile_station_classmark_2, 8));
    HANDLE_CODE(mobile_station_classmark_2.pack(bref));
  }
  if (supported_codecs_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_supported_codecs, 8));
    HANDLE_CODE(supported_codecs.pack(bref));
  }
  if (nas_message_container_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_nas_message_container, 8));
    HANDLE_CODE(nas_message_container.pack(bref));
  }
  if (eps_bearer_context_status_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eps_bearer_context_status, 8));
    HANDLE_CODE(eps_bearer_context_status.pack(bref));
  }
  if (requested_extended_drx_parameters_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_requested_extended_drx_parameters, 8));
    HANDLE_CODE(requested_extended_drx_parameters.pack(bref));
  }
  if (t3324_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_t3324_value, 8));
    HANDLE_CODE(t3324_value.pack(bref));
  }
  if (ue_radio_capability_id_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ue_radio_capability_id, 8));
    HANDLE_CODE(ue_radio_capability_id.pack(bref));
  }
  if (requested_mapped_nssai_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_requested_mapped_nssai, 8));
    HANDLE_CODE(requested_mapped_nssai.pack(bref));
  }
  if (additional_information_requested_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_additional_information_requested, 8));
    HANDLE_CODE(additional_information_requested.pack(bref));
  }
  if (requested_wus_assistance_information_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_requested_wus_assistance_information, 8));
    HANDLE_CODE(requested_wus_assistance_information.pack(bref));
  }
  if (n5gc_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_n5gc_indication, 4));
    HANDLE_CODE(n5gc_indication.pack(bref));
  }
  if (requested_nb_n1_mode_drx_parameters_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_requested_nb_n1_mode_drx_parameters, 8));
    HANDLE_CODE(requested_nb_n1_mode_drx_parameters.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE registration_request_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(ng_ksi.unpack(bref));
  HANDLE_CODE(registration_type_5gs.unpack(bref));
  HANDLE_CODE(mobile_identity_5gs.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_non_current_native_nas_key_set_identifier:
        non_current_native_nas_key_set_identifier_present = true;
        HANDLE_CODE(non_current_native_nas_key_set_identifier.unpack(bref));
        break;
      case ie_iei_capability_5gmm:
        capability_5gmm_present = true;
        HANDLE_CODE(capability_5gmm.unpack(bref));
        break;
      case ie_iei_ue_security_capability:
        ue_security_capability_present = true;
        HANDLE_CODE(ue_security_capability.unpack(bref));
        break;
      case ie_iei_requested_nssai:
        requested_nssai_present = true;
        HANDLE_CODE(requested_nssai.unpack(bref));
        break;
      case ie_iei_last_visited_registered_tai:
        last_visited_registered_tai_present = true;
        HANDLE_CODE(last_visited_registered_tai.unpack(bref));
        break;
      case ie_iei_s1_ue_network_capability:
        s1_ue_network_capability_present = true;
        HANDLE_CODE(s1_ue_network_capability.unpack(bref));
        break;
      case ie_iei_uplink_data_status:
        uplink_data_status_present = true;
        HANDLE_CODE(uplink_data_status.unpack(bref));
        break;
      case ie_iei_pdu_session_status:
        pdu_session_status_present = true;
        HANDLE_CODE(pdu_session_status.unpack(bref));
        break;
      case ie_iei_mico_indication:
        mico_indication_present = true;
        HANDLE_CODE(mico_indication.unpack(bref));
        break;
      case ie_iei_ue_status:
        ue_status_present = true;
        HANDLE_CODE(ue_status.unpack(bref));
        break;
      case ie_iei_additional_guti:
        additional_guti_present = true;
        HANDLE_CODE(additional_guti.unpack(bref));
        break;
      case ie_iei_allowed_pdu_session_status:
        allowed_pdu_session_status_present = true;
        HANDLE_CODE(allowed_pdu_session_status.unpack(bref));
        break;
      case ie_iei_ue_usage_setting:
        ue_usage_setting_present = true;
        HANDLE_CODE(ue_usage_setting.unpack(bref));
        break;
      case ie_iei_requested_drx_parameters:
        requested_drx_parameters_present = true;
        HANDLE_CODE(requested_drx_parameters.unpack(bref));
        break;
      case ie_iei_eps_nas_message_container:
        eps_nas_message_container_present = true;
        HANDLE_CODE(eps_nas_message_container.unpack(bref));
        break;
      case ie_iei_ladn_indication:
        ladn_indication_present = true;
        HANDLE_CODE(ladn_indication.unpack(bref));
        break;
      case ie_iei_payload_container_type:
        payload_container_type_present = true;
        HANDLE_CODE(payload_container_type.unpack(bref));
        break;
      case ie_iei_payload_container:
        payload_container_present = true;
        HANDLE_CODE(payload_container.unpack(bref));
        break;
      case ie_iei_network_slicing_indication:
        network_slicing_indication_present = true;
        HANDLE_CODE(network_slicing_indication.unpack(bref));
        break;
      case ie_iei_update_type_5gs:
        update_type_5gs_present = true;
        HANDLE_CODE(update_type_5gs.unpack(bref));
        break;
      case ie_iei_mobile_station_classmark_2:
        mobile_station_classmark_2_present = true;
        HANDLE_CODE(mobile_station_classmark_2.unpack(bref));
        break;
      case ie_iei_supported_codecs:
        supported_codecs_present = true;
        HANDLE_CODE(supported_codecs.unpack(bref));
        break;
      case ie_iei_nas_message_container:
        nas_message_container_present = true;
        HANDLE_CODE(nas_message_container.unpack(bref));
        break;
      case ie_iei_eps_bearer_context_status:
        eps_bearer_context_status_present = true;
        HANDLE_CODE(eps_bearer_context_status.unpack(bref));
        break;
      case ie_iei_requested_extended_drx_parameters:
        requested_extended_drx_parameters_present = true;
        HANDLE_CODE(requested_extended_drx_parameters.unpack(bref));
        break;
      case ie_iei_t3324_value:
        t3324_value_present = true;
        HANDLE_CODE(t3324_value.unpack(bref));
        break;
      case ie_iei_ue_radio_capability_id:
        ue_radio_capability_id_present = true;
        HANDLE_CODE(ue_radio_capability_id.unpack(bref));
        break;
      case ie_iei_requested_mapped_nssai:
        requested_mapped_nssai_present = true;
        HANDLE_CODE(requested_mapped_nssai.unpack(bref));
        break;
      case ie_iei_additional_information_requested:
        additional_information_requested_present = true;
        HANDLE_CODE(additional_information_requested.unpack(bref));
        break;
      case ie_iei_requested_wus_assistance_information:
        requested_wus_assistance_information_present = true;
        HANDLE_CODE(requested_wus_assistance_information.unpack(bref));
        break;
      case ie_iei_n5gc_indication:
        n5gc_indication_present = true;
        HANDLE_CODE(n5gc_indication.unpack(bref));
        break;
      case ie_iei_requested_nb_n1_mode_drx_parameters:
        requested_nb_n1_mode_drx_parameters_present = true;
        HANDLE_CODE(requested_nb_n1_mode_drx_parameters.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE registration_accept_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(registration_result_5gs.pack(bref));

  // Optional fields
  if (guti_5g_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_guti_5g, 8));
    HANDLE_CODE(guti_5g.pack(bref));
  }
  if (equivalent_plm_ns_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_equivalent_plm_ns, 8));
    HANDLE_CODE(equivalent_plm_ns.pack(bref));
  }
  if (tai_list_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_tai_list, 8));
    HANDLE_CODE(tai_list.pack(bref));
  }
  if (allowed_nssai_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_allowed_nssai, 8));
    HANDLE_CODE(allowed_nssai.pack(bref));
  }
  if (rejected_nssai_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_rejected_nssai, 8));
    HANDLE_CODE(rejected_nssai.pack(bref));
  }
  if (configured_nssai_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_configured_nssai, 8));
    HANDLE_CODE(configured_nssai.pack(bref));
  }
  if (network_feature_support_5gs_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_network_feature_support_5gs, 8));
    HANDLE_CODE(network_feature_support_5gs.pack(bref));
  }
  if (pdu_session_status_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pdu_session_status, 8));
    HANDLE_CODE(pdu_session_status.pack(bref));
  }
  if (pdu_session_reactivation_result_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pdu_session_reactivation_result, 8));
    HANDLE_CODE(pdu_session_reactivation_result.pack(bref));
  }
  if (pdu_session_reactivation_result_error_cause_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pdu_session_reactivation_result_error_cause, 8));
    HANDLE_CODE(pdu_session_reactivation_result_error_cause.pack(bref));
  }
  if (ladn_information_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ladn_information, 8));
    HANDLE_CODE(ladn_information.pack(bref));
  }
  if (mico_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_mico_indication, 4));
    HANDLE_CODE(mico_indication.pack(bref));
  }
  if (network_slicing_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_network_slicing_indication, 4));
    HANDLE_CODE(network_slicing_indication.pack(bref));
  }
  if (service_area_list_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_service_area_list, 8));
    HANDLE_CODE(service_area_list.pack(bref));
  }
  if (t3512_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_t3512_value, 8));
    HANDLE_CODE(t3512_value.pack(bref));
  }
  if (non_3_gpp_de_registration_timer_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_non_3_gpp_de_registration_timer_value, 8));
    HANDLE_CODE(non_3_gpp_de_registration_timer_value.pack(bref));
  }
  if (t3502_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_t3502_value, 8));
    HANDLE_CODE(t3502_value.pack(bref));
  }
  if (emergency_number_list_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_emergency_number_list, 8));
    HANDLE_CODE(emergency_number_list.pack(bref));
  }
  if (extended_emergency_number_list_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_emergency_number_list, 8));
    HANDLE_CODE(extended_emergency_number_list.pack(bref));
  }
  if (sor_transparent_container_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_sor_transparent_container, 8));
    HANDLE_CODE(sor_transparent_container.pack(bref));
  }
  if (eap_message_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eap_message, 8));
    HANDLE_CODE(eap_message.pack(bref));
  }
  if (nssai_inclusion_mode_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_nssai_inclusion_mode, 4));
    HANDLE_CODE(nssai_inclusion_mode.pack(bref));
  }
  if (operator_defined_access_category_definitions_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_operator_defined_access_category_definitions, 8));
    HANDLE_CODE(operator_defined_access_category_definitions.pack(bref));
  }
  if (negotiated_drx_parameters_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_negotiated_drx_parameters, 8));
    HANDLE_CODE(negotiated_drx_parameters.pack(bref));
  }
  if (non_3_gpp_nw_policies_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_non_3_gpp_nw_policies, 4));
    HANDLE_CODE(non_3_gpp_nw_policies.pack(bref));
  }
  if (eps_bearer_context_status_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eps_bearer_context_status, 8));
    HANDLE_CODE(eps_bearer_context_status.pack(bref));
  }
  if (negotiated_extended_drx_parameters_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_negotiated_extended_drx_parameters, 8));
    HANDLE_CODE(negotiated_extended_drx_parameters.pack(bref));
  }
  if (t3447_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_t3447_value, 8));
    HANDLE_CODE(t3447_value.pack(bref));
  }
  if (t3448_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_t3448_value, 8));
    HANDLE_CODE(t3448_value.pack(bref));
  }
  if (t3324_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_t3324_value, 8));
    HANDLE_CODE(t3324_value.pack(bref));
  }
  if (ue_radio_capability_id_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ue_radio_capability_id, 8));
    HANDLE_CODE(ue_radio_capability_id.pack(bref));
  }
  if (ue_radio_capability_id_deletion_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ue_radio_capability_id_deletion_indication, 4));
    HANDLE_CODE(ue_radio_capability_id_deletion_indication.pack(bref));
  }
  if (pending_nssai_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pending_nssai, 8));
    HANDLE_CODE(pending_nssai.pack(bref));
  }
  if (ciphering_key_data_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ciphering_key_data, 8));
    HANDLE_CODE(ciphering_key_data.pack(bref));
  }
  if (cag_information_list_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_cag_information_list, 8));
    HANDLE_CODE(cag_information_list.pack(bref));
  }
  if (truncated_5g_s_tmsi_configuration_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_truncated_5g_s_tmsi_configuration, 8));
    HANDLE_CODE(truncated_5g_s_tmsi_configuration.pack(bref));
  }
  if (negotiated_wus_assistance_information_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_negotiated_wus_assistance_information, 8));
    HANDLE_CODE(negotiated_wus_assistance_information.pack(bref));
  }
  if (negotiated_nb_n1_mode_drx_parameters_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_negotiated_nb_n1_mode_drx_parameters, 8));
    HANDLE_CODE(negotiated_nb_n1_mode_drx_parameters.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE registration_accept_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(registration_result_5gs.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_guti_5g:
        guti_5g_present = true;
        HANDLE_CODE(guti_5g.unpack(bref));
        break;
      case ie_iei_equivalent_plm_ns:
        equivalent_plm_ns_present = true;
        HANDLE_CODE(equivalent_plm_ns.unpack(bref));
        break;
      case ie_iei_tai_list:
        tai_list_present = true;
        HANDLE_CODE(tai_list.unpack(bref));
        break;
      case ie_iei_allowed_nssai:
        allowed_nssai_present = true;
        HANDLE_CODE(allowed_nssai.unpack(bref));
        break;
      case ie_iei_rejected_nssai:
        rejected_nssai_present = true;
        HANDLE_CODE(rejected_nssai.unpack(bref));
        break;
      case ie_iei_configured_nssai:
        configured_nssai_present = true;
        HANDLE_CODE(configured_nssai.unpack(bref));
        break;
      case ie_iei_network_feature_support_5gs:
        network_feature_support_5gs_present = true;
        HANDLE_CODE(network_feature_support_5gs.unpack(bref));
        break;
      case ie_iei_pdu_session_status:
        pdu_session_status_present = true;
        HANDLE_CODE(pdu_session_status.unpack(bref));
        break;
      case ie_iei_pdu_session_reactivation_result:
        pdu_session_reactivation_result_present = true;
        HANDLE_CODE(pdu_session_reactivation_result.unpack(bref));
        break;
      case ie_iei_pdu_session_reactivation_result_error_cause:
        pdu_session_reactivation_result_error_cause_present = true;
        HANDLE_CODE(pdu_session_reactivation_result_error_cause.unpack(bref));
        break;
      case ie_iei_ladn_information:
        ladn_information_present = true;
        HANDLE_CODE(ladn_information.unpack(bref));
        break;
      case ie_iei_mico_indication:
        mico_indication_present = true;
        HANDLE_CODE(mico_indication.unpack(bref));
        break;
      case ie_iei_network_slicing_indication:
        network_slicing_indication_present = true;
        HANDLE_CODE(network_slicing_indication.unpack(bref));
        break;
      case ie_iei_service_area_list:
        service_area_list_present = true;
        HANDLE_CODE(service_area_list.unpack(bref));
        break;
      case ie_iei_t3512_value:
        t3512_value_present = true;
        HANDLE_CODE(t3512_value.unpack(bref));
        break;
      case ie_iei_non_3_gpp_de_registration_timer_value:
        non_3_gpp_de_registration_timer_value_present = true;
        HANDLE_CODE(non_3_gpp_de_registration_timer_value.unpack(bref));
        break;
      case ie_iei_t3502_value:
        t3502_value_present = true;
        HANDLE_CODE(t3502_value.unpack(bref));
        break;
      case ie_iei_emergency_number_list:
        emergency_number_list_present = true;
        HANDLE_CODE(emergency_number_list.unpack(bref));
        break;
      case ie_iei_extended_emergency_number_list:
        extended_emergency_number_list_present = true;
        HANDLE_CODE(extended_emergency_number_list.unpack(bref));
        break;
      case ie_iei_sor_transparent_container:
        sor_transparent_container_present = true;
        HANDLE_CODE(sor_transparent_container.unpack(bref));
        break;
      case ie_iei_eap_message:
        eap_message_present = true;
        HANDLE_CODE(eap_message.unpack(bref));
        break;
      case ie_iei_nssai_inclusion_mode:
        nssai_inclusion_mode_present = true;
        HANDLE_CODE(nssai_inclusion_mode.unpack(bref));
        break;
      case ie_iei_operator_defined_access_category_definitions:
        operator_defined_access_category_definitions_present = true;
        HANDLE_CODE(operator_defined_access_category_definitions.unpack(bref));
        break;
      case ie_iei_negotiated_drx_parameters:
        negotiated_drx_parameters_present = true;
        HANDLE_CODE(negotiated_drx_parameters.unpack(bref));
        break;
      case ie_iei_non_3_gpp_nw_policies:
        non_3_gpp_nw_policies_present = true;
        HANDLE_CODE(non_3_gpp_nw_policies.unpack(bref));
        break;
      case ie_iei_eps_bearer_context_status:
        eps_bearer_context_status_present = true;
        HANDLE_CODE(eps_bearer_context_status.unpack(bref));
        break;
      case ie_iei_negotiated_extended_drx_parameters:
        negotiated_extended_drx_parameters_present = true;
        HANDLE_CODE(negotiated_extended_drx_parameters.unpack(bref));
        break;
      case ie_iei_t3447_value:
        t3447_value_present = true;
        HANDLE_CODE(t3447_value.unpack(bref));
        break;
      case ie_iei_t3448_value:
        t3448_value_present = true;
        HANDLE_CODE(t3448_value.unpack(bref));
        break;
      case ie_iei_t3324_value:
        t3324_value_present = true;
        HANDLE_CODE(t3324_value.unpack(bref));
        break;
      case ie_iei_ue_radio_capability_id:
        ue_radio_capability_id_present = true;
        HANDLE_CODE(ue_radio_capability_id.unpack(bref));
        break;
      case ie_iei_ue_radio_capability_id_deletion_indication:
        ue_radio_capability_id_deletion_indication_present = true;
        HANDLE_CODE(ue_radio_capability_id_deletion_indication.unpack(bref));
        break;
      case ie_iei_pending_nssai:
        pending_nssai_present = true;
        HANDLE_CODE(pending_nssai.unpack(bref));
        break;
      case ie_iei_ciphering_key_data:
        ciphering_key_data_present = true;
        HANDLE_CODE(ciphering_key_data.unpack(bref));
        break;
      case ie_iei_cag_information_list:
        cag_information_list_present = true;
        HANDLE_CODE(cag_information_list.unpack(bref));
        break;
      case ie_iei_truncated_5g_s_tmsi_configuration:
        truncated_5g_s_tmsi_configuration_present = true;
        HANDLE_CODE(truncated_5g_s_tmsi_configuration.unpack(bref));
        break;
      case ie_iei_negotiated_wus_assistance_information:
        negotiated_wus_assistance_information_present = true;
        HANDLE_CODE(negotiated_wus_assistance_information.unpack(bref));
        break;
      case ie_iei_negotiated_nb_n1_mode_drx_parameters:
        negotiated_nb_n1_mode_drx_parameters_present = true;
        HANDLE_CODE(negotiated_nb_n1_mode_drx_parameters.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE registration_complete_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields
  if (sor_transparent_container_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_sor_transparent_container, 8));
    HANDLE_CODE(sor_transparent_container.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE registration_complete_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_sor_transparent_container:
        sor_transparent_container_present = true;
        HANDLE_CODE(sor_transparent_container.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE registration_reject_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gmm.pack(bref));

  // Optional fields
  if (t3346_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_t3346_value, 8));
    HANDLE_CODE(t3346_value.pack(bref));
  }
  if (t3502_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_t3502_value, 8));
    HANDLE_CODE(t3502_value.pack(bref));
  }
  if (eap_message_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eap_message, 8));
    HANDLE_CODE(eap_message.pack(bref));
  }
  if (rejected_nssai_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_rejected_nssai, 8));
    HANDLE_CODE(rejected_nssai.pack(bref));
  }
  if (cag_information_list_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_cag_information_list, 8));
    HANDLE_CODE(cag_information_list.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE registration_reject_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gmm.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_t3346_value:
        t3346_value_present = true;
        HANDLE_CODE(t3346_value.unpack(bref));
        break;
      case ie_iei_t3502_value:
        t3502_value_present = true;
        HANDLE_CODE(t3502_value.unpack(bref));
        break;
      case ie_iei_eap_message:
        eap_message_present = true;
        HANDLE_CODE(eap_message.unpack(bref));
        break;
      case ie_iei_rejected_nssai:
        rejected_nssai_present = true;
        HANDLE_CODE(rejected_nssai.unpack(bref));
        break;
      case ie_iei_cag_information_list:
        cag_information_list_present = true;
        HANDLE_CODE(cag_information_list.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE deregistration_request_ue_originating_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(ng_ksi.pack(bref));
  HANDLE_CODE(de_registration_type.pack(bref));
  HANDLE_CODE(mobile_identity_5gs.pack(bref));

  // Optional fields

  return SRSASN_SUCCESS;
}
SRSASN_CODE deregistration_request_ue_originating_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(ng_ksi.unpack(bref));
  HANDLE_CODE(de_registration_type.unpack(bref));
  HANDLE_CODE(mobile_identity_5gs.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE deregistration_accept_ue_originating_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  return SRSASN_SUCCESS;
}
SRSASN_CODE deregistration_accept_ue_originating_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE deregistration_request_ue_terminated_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(spare_half_octet.pack(bref));
  HANDLE_CODE(de_registration_type.pack(bref));

  // Optional fields
  if (cause_5gmm_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_cause_5gmm, 8));
    HANDLE_CODE(cause_5gmm.pack(bref));
  }
  if (t3346_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_t3346_value, 8));
    HANDLE_CODE(t3346_value.pack(bref));
  }
  if (rejected_nssai_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_rejected_nssai, 8));
    HANDLE_CODE(rejected_nssai.pack(bref));
  }
  if (cag_information_list_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_cag_information_list, 8));
    HANDLE_CODE(cag_information_list.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE deregistration_request_ue_terminated_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(spare_half_octet.unpack(bref));
  HANDLE_CODE(de_registration_type.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_cause_5gmm:
        cause_5gmm_present = true;
        HANDLE_CODE(cause_5gmm.unpack(bref));
        break;
      case ie_iei_t3346_value:
        t3346_value_present = true;
        HANDLE_CODE(t3346_value.unpack(bref));
        break;
      case ie_iei_rejected_nssai:
        rejected_nssai_present = true;
        HANDLE_CODE(rejected_nssai.unpack(bref));
        break;
      case ie_iei_cag_information_list:
        cag_information_list_present = true;
        HANDLE_CODE(cag_information_list.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE deregistration_accept_ue_terminated_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  return SRSASN_SUCCESS;
}
SRSASN_CODE deregistration_accept_ue_terminated_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE service_request_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(service_type.pack(bref));
  HANDLE_CODE(ng_ksi.pack(bref));
  HANDLE_CODE(s_tmsi_5g.pack(bref));

  // Optional fields
  if (uplink_data_status_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_uplink_data_status, 8));
    HANDLE_CODE(uplink_data_status.pack(bref));
  }
  if (pdu_session_status_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pdu_session_status, 8));
    HANDLE_CODE(pdu_session_status.pack(bref));
  }
  if (allowed_pdu_session_status_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_allowed_pdu_session_status, 8));
    HANDLE_CODE(allowed_pdu_session_status.pack(bref));
  }
  if (nas_message_container_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_nas_message_container, 8));
    HANDLE_CODE(nas_message_container.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE service_request_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(service_type.unpack(bref));
  HANDLE_CODE(ng_ksi.unpack(bref));
  HANDLE_CODE(s_tmsi_5g.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_uplink_data_status:
        uplink_data_status_present = true;
        HANDLE_CODE(uplink_data_status.unpack(bref));
        break;
      case ie_iei_pdu_session_status:
        pdu_session_status_present = true;
        HANDLE_CODE(pdu_session_status.unpack(bref));
        break;
      case ie_iei_allowed_pdu_session_status:
        allowed_pdu_session_status_present = true;
        HANDLE_CODE(allowed_pdu_session_status.unpack(bref));
        break;
      case ie_iei_nas_message_container:
        nas_message_container_present = true;
        HANDLE_CODE(nas_message_container.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE service_reject_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields
  if (pdu_session_status_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pdu_session_status, 8));
    HANDLE_CODE(pdu_session_status.pack(bref));
  }
  if (pdu_session_reactivation_result_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pdu_session_reactivation_result, 8));
    HANDLE_CODE(pdu_session_reactivation_result.pack(bref));
  }
  if (pdu_session_reactivation_result_error_cause_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pdu_session_reactivation_result_error_cause, 8));
    HANDLE_CODE(pdu_session_reactivation_result_error_cause.pack(bref));
  }
  if (eap_message_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eap_message, 8));
    HANDLE_CODE(eap_message.pack(bref));
  }
  if (t3448_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_t3448_value, 8));
    HANDLE_CODE(t3448_value.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE service_reject_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_pdu_session_status:
        pdu_session_status_present = true;
        HANDLE_CODE(pdu_session_status.unpack(bref));
        break;
      case ie_iei_pdu_session_reactivation_result:
        pdu_session_reactivation_result_present = true;
        HANDLE_CODE(pdu_session_reactivation_result.unpack(bref));
        break;
      case ie_iei_pdu_session_reactivation_result_error_cause:
        pdu_session_reactivation_result_error_cause_present = true;
        HANDLE_CODE(pdu_session_reactivation_result_error_cause.unpack(bref));
        break;
      case ie_iei_eap_message:
        eap_message_present = true;
        HANDLE_CODE(eap_message.unpack(bref));
        break;
      case ie_iei_t3448_value:
        t3448_value_present = true;
        HANDLE_CODE(t3448_value.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE service_accept_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gmm.pack(bref));

  // Optional fields
  if (pdu_session_status_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pdu_session_status, 8));
    HANDLE_CODE(pdu_session_status.pack(bref));
  }
  if (t3346_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_t3346_value, 8));
    HANDLE_CODE(t3346_value.pack(bref));
  }
  if (eap_message_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eap_message, 8));
    HANDLE_CODE(eap_message.pack(bref));
  }
  if (t3448_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_t3448_value, 8));
    HANDLE_CODE(t3448_value.pack(bref));
  }
  if (cag_information_list_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_cag_information_list, 8));
    HANDLE_CODE(cag_information_list.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE service_accept_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gmm.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_pdu_session_status:
        pdu_session_status_present = true;
        HANDLE_CODE(pdu_session_status.unpack(bref));
        break;
      case ie_iei_t3346_value:
        t3346_value_present = true;
        HANDLE_CODE(t3346_value.unpack(bref));
        break;
      case ie_iei_eap_message:
        eap_message_present = true;
        HANDLE_CODE(eap_message.unpack(bref));
        break;
      case ie_iei_t3448_value:
        t3448_value_present = true;
        HANDLE_CODE(t3448_value.unpack(bref));
        break;
      case ie_iei_cag_information_list:
        cag_information_list_present = true;
        HANDLE_CODE(cag_information_list.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE configuration_update_command_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields
  if (configuration_update_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_configuration_update_indication, 4));
    HANDLE_CODE(configuration_update_indication.pack(bref));
  }
  if (guti_5g_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_guti_5g, 8));
    HANDLE_CODE(guti_5g.pack(bref));
  }
  if (tai_list_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_tai_list, 8));
    HANDLE_CODE(tai_list.pack(bref));
  }
  if (allowed_nssai_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_allowed_nssai, 8));
    HANDLE_CODE(allowed_nssai.pack(bref));
  }
  if (service_area_list_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_service_area_list, 8));
    HANDLE_CODE(service_area_list.pack(bref));
  }
  if (full_name_for_network_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_full_name_for_network, 8));
    HANDLE_CODE(full_name_for_network.pack(bref));
  }
  if (short_name_for_network_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_short_name_for_network, 8));
    HANDLE_CODE(short_name_for_network.pack(bref));
  }
  if (local_time_zone_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_local_time_zone, 8));
    HANDLE_CODE(local_time_zone.pack(bref));
  }
  if (universal_time_and_local_time_zone_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_universal_time_and_local_time_zone, 8));
    HANDLE_CODE(universal_time_and_local_time_zone.pack(bref));
  }
  if (network_daylight_saving_time_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_network_daylight_saving_time, 8));
    HANDLE_CODE(network_daylight_saving_time.pack(bref));
  }
  if (ladn_information_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ladn_information, 8));
    HANDLE_CODE(ladn_information.pack(bref));
  }
  if (mico_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_mico_indication, 4));
    HANDLE_CODE(mico_indication.pack(bref));
  }
  if (network_slicing_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_network_slicing_indication, 4));
    HANDLE_CODE(network_slicing_indication.pack(bref));
  }
  if (configured_nssai_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_configured_nssai, 8));
    HANDLE_CODE(configured_nssai.pack(bref));
  }
  if (rejected_nssai_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_rejected_nssai, 8));
    HANDLE_CODE(rejected_nssai.pack(bref));
  }
  if (operator_defined_access_category_definitions_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_operator_defined_access_category_definitions, 8));
    HANDLE_CODE(operator_defined_access_category_definitions.pack(bref));
  }
  if (sms_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_sms_indication, 4));
    HANDLE_CODE(sms_indication.pack(bref));
  }
  if (t3447_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_t3447_value, 8));
    HANDLE_CODE(t3447_value.pack(bref));
  }
  if (cag_information_list_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_cag_information_list, 8));
    HANDLE_CODE(cag_information_list.pack(bref));
  }
  if (ue_radio_capability_id_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ue_radio_capability_id, 8));
    HANDLE_CODE(ue_radio_capability_id.pack(bref));
  }
  if (ue_radio_capability_id_deletion_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ue_radio_capability_id_deletion_indication, 4));
    HANDLE_CODE(ue_radio_capability_id_deletion_indication.pack(bref));
  }
  if (registration_result_5gs_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_registration_result_5gs, 8));
    HANDLE_CODE(registration_result_5gs.pack(bref));
  }
  if (truncated_5g_s_tmsi_configuration_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_truncated_5g_s_tmsi_configuration, 8));
    HANDLE_CODE(truncated_5g_s_tmsi_configuration.pack(bref));
  }
  if (additional_configuration_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_additional_configuration_indication, 4));
    HANDLE_CODE(additional_configuration_indication.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE configuration_update_command_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_configuration_update_indication:
        configuration_update_indication_present = true;
        HANDLE_CODE(configuration_update_indication.unpack(bref));
        break;
      case ie_iei_guti_5g:
        guti_5g_present = true;
        HANDLE_CODE(guti_5g.unpack(bref));
        break;
      case ie_iei_tai_list:
        tai_list_present = true;
        HANDLE_CODE(tai_list.unpack(bref));
        break;
      case ie_iei_allowed_nssai:
        allowed_nssai_present = true;
        HANDLE_CODE(allowed_nssai.unpack(bref));
        break;
      case ie_iei_service_area_list:
        service_area_list_present = true;
        HANDLE_CODE(service_area_list.unpack(bref));
        break;
      case ie_iei_full_name_for_network:
        full_name_for_network_present = true;
        HANDLE_CODE(full_name_for_network.unpack(bref));
        break;
      case ie_iei_short_name_for_network:
        short_name_for_network_present = true;
        HANDLE_CODE(short_name_for_network.unpack(bref));
        break;
      case ie_iei_local_time_zone:
        local_time_zone_present = true;
        HANDLE_CODE(local_time_zone.unpack(bref));
        break;
      case ie_iei_universal_time_and_local_time_zone:
        universal_time_and_local_time_zone_present = true;
        HANDLE_CODE(universal_time_and_local_time_zone.unpack(bref));
        break;
      case ie_iei_network_daylight_saving_time:
        network_daylight_saving_time_present = true;
        HANDLE_CODE(network_daylight_saving_time.unpack(bref));
        break;
      case ie_iei_ladn_information:
        ladn_information_present = true;
        HANDLE_CODE(ladn_information.unpack(bref));
        break;
      case ie_iei_mico_indication:
        mico_indication_present = true;
        HANDLE_CODE(mico_indication.unpack(bref));
        break;
      case ie_iei_network_slicing_indication:
        network_slicing_indication_present = true;
        HANDLE_CODE(network_slicing_indication.unpack(bref));
        break;
      case ie_iei_configured_nssai:
        configured_nssai_present = true;
        HANDLE_CODE(configured_nssai.unpack(bref));
        break;
      case ie_iei_rejected_nssai:
        rejected_nssai_present = true;
        HANDLE_CODE(rejected_nssai.unpack(bref));
        break;
      case ie_iei_operator_defined_access_category_definitions:
        operator_defined_access_category_definitions_present = true;
        HANDLE_CODE(operator_defined_access_category_definitions.unpack(bref));
        break;
      case ie_iei_sms_indication:
        sms_indication_present = true;
        HANDLE_CODE(sms_indication.unpack(bref));
        break;
      case ie_iei_t3447_value:
        t3447_value_present = true;
        HANDLE_CODE(t3447_value.unpack(bref));
        break;
      case ie_iei_cag_information_list:
        cag_information_list_present = true;
        HANDLE_CODE(cag_information_list.unpack(bref));
        break;
      case ie_iei_ue_radio_capability_id:
        ue_radio_capability_id_present = true;
        HANDLE_CODE(ue_radio_capability_id.unpack(bref));
        break;
      case ie_iei_ue_radio_capability_id_deletion_indication:
        ue_radio_capability_id_deletion_indication_present = true;
        HANDLE_CODE(ue_radio_capability_id_deletion_indication.unpack(bref));
        break;
      case ie_iei_registration_result_5gs:
        registration_result_5gs_present = true;
        HANDLE_CODE(registration_result_5gs.unpack(bref));
        break;
      case ie_iei_truncated_5g_s_tmsi_configuration:
        truncated_5g_s_tmsi_configuration_present = true;
        HANDLE_CODE(truncated_5g_s_tmsi_configuration.unpack(bref));
        break;
      case ie_iei_additional_configuration_indication:
        additional_configuration_indication_present = true;
        HANDLE_CODE(additional_configuration_indication.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE configuration_update_complete_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  return SRSASN_SUCCESS;
}
SRSASN_CODE configuration_update_complete_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE authentication_request_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(spare_half_octet.pack(bref));
  HANDLE_CODE(ng_ksi.pack(bref));
  HANDLE_CODE(abba.pack(bref));

  // Optional fields
  if (authentication_parameter_rand_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_authentication_parameter_rand, 8));
    HANDLE_CODE(authentication_parameter_rand.pack(bref));
  }
  if (authentication_parameter_autn_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_authentication_parameter_autn, 8));
    HANDLE_CODE(authentication_parameter_autn.pack(bref));
  }
  if (eap_message_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eap_message, 8));
    HANDLE_CODE(eap_message.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE authentication_request_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(spare_half_octet.unpack(bref));
  HANDLE_CODE(ng_ksi.unpack(bref));
  HANDLE_CODE(abba.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_authentication_parameter_rand:
        authentication_parameter_rand_present = true;
        HANDLE_CODE(authentication_parameter_rand.unpack(bref));
        break;
      case ie_iei_authentication_parameter_autn:
        authentication_parameter_autn_present = true;
        HANDLE_CODE(authentication_parameter_autn.unpack(bref));
        break;
      case ie_iei_eap_message:
        eap_message_present = true;
        HANDLE_CODE(eap_message.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE authentication_response_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields
  if (authentication_response_parameter_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_authentication_response_parameter, 8));
    HANDLE_CODE(authentication_response_parameter.pack(bref));
  }
  if (eap_message_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eap_message, 8));
    HANDLE_CODE(eap_message.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE authentication_response_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_authentication_response_parameter:
        authentication_response_parameter_present = true;
        HANDLE_CODE(authentication_response_parameter.unpack(bref));
        break;
      case ie_iei_eap_message:
        eap_message_present = true;
        HANDLE_CODE(eap_message.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE authentication_reject_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields
  if (eap_message_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eap_message, 8));
    HANDLE_CODE(eap_message.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE authentication_reject_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_eap_message:
        eap_message_present = true;
        HANDLE_CODE(eap_message.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE authentication_failure_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gmm.pack(bref));

  // Optional fields
  if (authentication_failure_parameter_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_authentication_failure_parameter, 8));
    HANDLE_CODE(authentication_failure_parameter.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE authentication_failure_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gmm.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_authentication_failure_parameter:
        authentication_failure_parameter_present = true;
        HANDLE_CODE(authentication_failure_parameter.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE authentication_result_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(spare_half_octet.pack(bref));
  HANDLE_CODE(ng_ksi.pack(bref));
  HANDLE_CODE(eap_message.pack(bref));

  // Optional fields
  if (abba_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_abba, 8));
    HANDLE_CODE(abba.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE authentication_result_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(spare_half_octet.unpack(bref));
  HANDLE_CODE(ng_ksi.unpack(bref));
  HANDLE_CODE(eap_message.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_abba:
        abba_present = true;
        HANDLE_CODE(abba.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE identity_request_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(spare_half_octet.pack(bref));
  HANDLE_CODE(identity_type.pack(bref));

  // Optional fields

  return SRSASN_SUCCESS;
}
SRSASN_CODE identity_request_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(spare_half_octet.unpack(bref));
  HANDLE_CODE(identity_type.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE identity_response_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(mobile_identity.pack(bref));

  // Optional fields

  return SRSASN_SUCCESS;
}
SRSASN_CODE identity_response_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(mobile_identity.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE security_mode_command_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(selected_nas_security_algorithms.pack(bref));
  HANDLE_CODE(spare_half_octet.pack(bref));
  HANDLE_CODE(ng_ksi.pack(bref));
  HANDLE_CODE(replayed_ue_security_capabilities.pack(bref));

  // Optional fields
  if (imeisv_request_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_imeisv_request, 4));
    HANDLE_CODE(imeisv_request.pack(bref));
  }
  if (selected_eps_nas_security_algorithms_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_selected_eps_nas_security_algorithms, 8));
    HANDLE_CODE(selected_eps_nas_security_algorithms.pack(bref));
  }
  if (additional_5g_security_information_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_additional_5g_security_information, 8));
    HANDLE_CODE(additional_5g_security_information.pack(bref));
  }
  if (eap_message_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eap_message, 8));
    HANDLE_CODE(eap_message.pack(bref));
  }
  if (abba_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_abba, 8));
    HANDLE_CODE(abba.pack(bref));
  }
  if (replayed_s1_ue_security_capabilities_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_replayed_s1_ue_security_capabilities, 8));
    HANDLE_CODE(replayed_s1_ue_security_capabilities.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_command_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(selected_nas_security_algorithms.unpack(bref));
  HANDLE_CODE(spare_half_octet.unpack(bref));
  HANDLE_CODE(ng_ksi.unpack(bref));
  HANDLE_CODE(replayed_ue_security_capabilities.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_imeisv_request:
        imeisv_request_present = true;
        HANDLE_CODE(imeisv_request.unpack(bref));
        break;
      case ie_iei_selected_eps_nas_security_algorithms:
        selected_eps_nas_security_algorithms_present = true;
        HANDLE_CODE(selected_eps_nas_security_algorithms.unpack(bref));
        break;
      case ie_iei_additional_5g_security_information:
        additional_5g_security_information_present = true;
        HANDLE_CODE(additional_5g_security_information.unpack(bref));
        break;
      case ie_iei_eap_message:
        eap_message_present = true;
        HANDLE_CODE(eap_message.unpack(bref));
        break;
      case ie_iei_abba:
        abba_present = true;
        HANDLE_CODE(abba.unpack(bref));
        break;
      case ie_iei_replayed_s1_ue_security_capabilities:
        replayed_s1_ue_security_capabilities_present = true;
        HANDLE_CODE(replayed_s1_ue_security_capabilities.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE security_mode_complete_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields
  if (imeisv_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_imeisv, 8));
    HANDLE_CODE(imeisv.pack(bref));
  }
  if (nas_message_container_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_nas_message_container, 8));
    HANDLE_CODE(nas_message_container.pack(bref));
  }
  if (non_imeisv_pei_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_non_imeisv_pei, 8));
    HANDLE_CODE(non_imeisv_pei.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_complete_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_imeisv:
        imeisv_present = true;
        HANDLE_CODE(imeisv.unpack(bref));
        break;
      case ie_iei_nas_message_container:
        nas_message_container_present = true;
        HANDLE_CODE(nas_message_container.unpack(bref));
        break;
      case ie_iei_non_imeisv_pei:
        non_imeisv_pei_present = true;
        HANDLE_CODE(non_imeisv_pei.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE security_mode_reject_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gmm.pack(bref));

  // Optional fields

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_reject_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gmm.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE status_5gmm_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gmm.pack(bref));

  // Optional fields

  return SRSASN_SUCCESS;
}
SRSASN_CODE status_5gmm_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gmm.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE notification_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(spare_half_octet.pack(bref));
  HANDLE_CODE(access_type.pack(bref));

  // Optional fields

  return SRSASN_SUCCESS;
}
SRSASN_CODE notification_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(spare_half_octet.unpack(bref));
  HANDLE_CODE(access_type.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE notification_response_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields
  if (pdu_session_status_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pdu_session_status, 8));
    HANDLE_CODE(pdu_session_status.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE notification_response_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_pdu_session_status:
        pdu_session_status_present = true;
        HANDLE_CODE(pdu_session_status.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE ul_nas_transport_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(spare_half_octet.pack(bref));
  HANDLE_CODE(payload_container_type.pack(bref));
  HANDLE_CODE(payload_container.pack(bref));

  // Optional fields
  if (pdu_session_id_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pdu_session_id, 8));
    HANDLE_CODE(pdu_session_id.pack(bref));
  }
  if (old_pdu_session_id_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_old_pdu_session_id, 8));
    HANDLE_CODE(old_pdu_session_id.pack(bref));
  }
  if (request_type_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_request_type, 4));
    HANDLE_CODE(request_type.pack(bref));
  }
  if (s_nssai_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_s_nssai, 8));
    HANDLE_CODE(s_nssai.pack(bref));
  }
  if (dnn_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_dnn, 8));
    HANDLE_CODE(dnn.pack(bref));
  }
  if (additional_information_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_additional_information, 8));
    HANDLE_CODE(additional_information.pack(bref));
  }
  if (ma_pdu_session_information_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ma_pdu_session_information, 4));
    HANDLE_CODE(ma_pdu_session_information.pack(bref));
  }
  if (release_assistance_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_release_assistance_indication, 4));
    HANDLE_CODE(release_assistance_indication.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_nas_transport_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(spare_half_octet.unpack(bref));
  HANDLE_CODE(payload_container_type.unpack(bref));
  HANDLE_CODE(payload_container.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_pdu_session_id:
        pdu_session_id_present = true;
        HANDLE_CODE(pdu_session_id.unpack(bref));
        break;
      case ie_iei_old_pdu_session_id:
        old_pdu_session_id_present = true;
        HANDLE_CODE(old_pdu_session_id.unpack(bref));
        break;
      case ie_iei_request_type:
        request_type_present = true;
        HANDLE_CODE(request_type.unpack(bref));
        break;
      case ie_iei_s_nssai:
        s_nssai_present = true;
        HANDLE_CODE(s_nssai.unpack(bref));
        break;
      case ie_iei_dnn:
        dnn_present = true;
        HANDLE_CODE(dnn.unpack(bref));
        break;
      case ie_iei_additional_information:
        additional_information_present = true;
        HANDLE_CODE(additional_information.unpack(bref));
        break;
      case ie_iei_ma_pdu_session_information:
        ma_pdu_session_information_present = true;
        HANDLE_CODE(ma_pdu_session_information.unpack(bref));
        break;
      case ie_iei_release_assistance_indication:
        release_assistance_indication_present = true;
        HANDLE_CODE(release_assistance_indication.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE dl_nas_transport_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(spare_half_octet.pack(bref));
  HANDLE_CODE(payload_container_type.pack(bref));
  HANDLE_CODE(payload_container.pack(bref));

  // Optional fields
  if (pdu_session_id_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pdu_session_id, 8));
    HANDLE_CODE(pdu_session_id.pack(bref));
  }
  if (additional_information_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_additional_information, 8));
    HANDLE_CODE(additional_information.pack(bref));
  }
  if (cause_5gmm_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_cause_5gmm, 8));
    HANDLE_CODE(cause_5gmm.pack(bref));
  }
  if (back_off_timer_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_back_off_timer_value, 8));
    HANDLE_CODE(back_off_timer_value.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_nas_transport_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(spare_half_octet.unpack(bref));
  HANDLE_CODE(payload_container_type.unpack(bref));
  HANDLE_CODE(payload_container.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_pdu_session_id:
        pdu_session_id_present = true;
        HANDLE_CODE(pdu_session_id.unpack(bref));
        break;
      case ie_iei_additional_information:
        additional_information_present = true;
        HANDLE_CODE(additional_information.unpack(bref));
        break;
      case ie_iei_cause_5gmm:
        cause_5gmm_present = true;
        HANDLE_CODE(cause_5gmm.unpack(bref));
        break;
      case ie_iei_back_off_timer_value:
        back_off_timer_value_present = true;
        HANDLE_CODE(back_off_timer_value.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_establishment_request_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(integrity_protection_maximum_data_rate.pack(bref));

  // Optional fields
  if (pdu_session_type_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pdu_session_type, 4));
    HANDLE_CODE(pdu_session_type.pack(bref));
  }
  if (ssc_mode_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ssc_mode, 4));
    HANDLE_CODE(ssc_mode.pack(bref));
  }
  if (capability_5gsm_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_capability_5gsm, 8));
    HANDLE_CODE(capability_5gsm.pack(bref));
  }
  if (maximum_number_of_supported_packet_filters_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_maximum_number_of_supported_packet_filters, 8));
    HANDLE_CODE(maximum_number_of_supported_packet_filters.pack(bref));
  }
  if (always_on_pdu_session_requested_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_always_on_pdu_session_requested, 4));
    HANDLE_CODE(always_on_pdu_session_requested.pack(bref));
  }
  if (sm_pdu_dn_request_container_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_sm_pdu_dn_request_container, 8));
    HANDLE_CODE(sm_pdu_dn_request_container.pack(bref));
  }
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }
  if (ip_header_compression_configuration_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ip_header_compression_configuration, 8));
    HANDLE_CODE(ip_header_compression_configuration.pack(bref));
  }
  if (ds_tt__ethernet_port_mac_address_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ds_tt__ethernet_port_mac_address, 8));
    HANDLE_CODE(ds_tt__ethernet_port_mac_address.pack(bref));
  }
  if (ue_ds_tt_residence_time_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ue_ds_tt_residence_time, 8));
    HANDLE_CODE(ue_ds_tt_residence_time.pack(bref));
  }
  if (port_management_information_container_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_port_management_information_container, 8));
    HANDLE_CODE(port_management_information_container.pack(bref));
  }
  if (ethernet_header_compression_configuration_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ethernet_header_compression_configuration, 8));
    HANDLE_CODE(ethernet_header_compression_configuration.pack(bref));
  }
  if (suggested_interface_identifier_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_suggested_interface_identifier, 8));
    HANDLE_CODE(suggested_interface_identifier.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_establishment_request_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(integrity_protection_maximum_data_rate.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_pdu_session_type:
        pdu_session_type_present = true;
        HANDLE_CODE(pdu_session_type.unpack(bref));
        break;
      case ie_iei_ssc_mode:
        ssc_mode_present = true;
        HANDLE_CODE(ssc_mode.unpack(bref));
        break;
      case ie_iei_capability_5gsm:
        capability_5gsm_present = true;
        HANDLE_CODE(capability_5gsm.unpack(bref));
        break;
      case ie_iei_maximum_number_of_supported_packet_filters:
        maximum_number_of_supported_packet_filters_present = true;
        HANDLE_CODE(maximum_number_of_supported_packet_filters.unpack(bref));
        break;
      case ie_iei_always_on_pdu_session_requested:
        always_on_pdu_session_requested_present = true;
        HANDLE_CODE(always_on_pdu_session_requested.unpack(bref));
        break;
      case ie_iei_sm_pdu_dn_request_container:
        sm_pdu_dn_request_container_present = true;
        HANDLE_CODE(sm_pdu_dn_request_container.unpack(bref));
        break;
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      case ie_iei_ip_header_compression_configuration:
        ip_header_compression_configuration_present = true;
        HANDLE_CODE(ip_header_compression_configuration.unpack(bref));
        break;
      case ie_iei_ds_tt__ethernet_port_mac_address:
        ds_tt__ethernet_port_mac_address_present = true;
        HANDLE_CODE(ds_tt__ethernet_port_mac_address.unpack(bref));
        break;
      case ie_iei_ue_ds_tt_residence_time:
        ue_ds_tt_residence_time_present = true;
        HANDLE_CODE(ue_ds_tt_residence_time.unpack(bref));
        break;
      case ie_iei_port_management_information_container:
        port_management_information_container_present = true;
        HANDLE_CODE(port_management_information_container.unpack(bref));
        break;
      case ie_iei_ethernet_header_compression_configuration:
        ethernet_header_compression_configuration_present = true;
        HANDLE_CODE(ethernet_header_compression_configuration.unpack(bref));
        break;
      case ie_iei_suggested_interface_identifier:
        suggested_interface_identifier_present = true;
        HANDLE_CODE(suggested_interface_identifier.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_establishment_accept_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(selected_ssc_mode.pack(bref));
  HANDLE_CODE(selected_pdu_session_type.pack(bref));
  HANDLE_CODE(authorized__qo_s_rules.pack(bref));
  HANDLE_CODE(session_ambr.pack(bref));

  // Optional fields
  if (cause_5gsm_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_cause_5gsm, 8));
    HANDLE_CODE(cause_5gsm.pack(bref));
  }
  if (pdu_address_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_pdu_address, 8));
    HANDLE_CODE(pdu_address.pack(bref));
  }
  if (rq_timer_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_rq_timer_value, 8));
    HANDLE_CODE(rq_timer_value.pack(bref));
  }
  if (s_nssai_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_s_nssai, 8));
    HANDLE_CODE(s_nssai.pack(bref));
  }
  if (always_on_pdu_session_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_always_on_pdu_session_indication, 4));
    HANDLE_CODE(always_on_pdu_session_indication.pack(bref));
  }
  if (mapped_eps_bearer_contexts_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_mapped_eps_bearer_contexts, 8));
    HANDLE_CODE(mapped_eps_bearer_contexts.pack(bref));
  }
  if (eap_message_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eap_message, 8));
    HANDLE_CODE(eap_message.pack(bref));
  }
  if (authorized__qo_s_flow_descriptions_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_authorized__qo_s_flow_descriptions, 8));
    HANDLE_CODE(authorized__qo_s_flow_descriptions.pack(bref));
  }
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }
  if (dnn_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_dnn, 8));
    HANDLE_CODE(dnn.pack(bref));
  }
  if (network_feature_support_5gsm_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_network_feature_support_5gsm, 8));
    HANDLE_CODE(network_feature_support_5gsm.pack(bref));
  }
  if (serving_plmn_rate_control_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_serving_plmn_rate_control, 8));
    HANDLE_CODE(serving_plmn_rate_control.pack(bref));
  }
  if (atsss_container_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_atsss_container, 8));
    HANDLE_CODE(atsss_container.pack(bref));
  }
  if (control_plane_only_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_control_plane_only_indication, 4));
    HANDLE_CODE(control_plane_only_indication.pack(bref));
  }
  if (ip_header_compression_configuration_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ip_header_compression_configuration, 8));
    HANDLE_CODE(ip_header_compression_configuration.pack(bref));
  }
  if (ethernet_header_compression_configuration_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ethernet_header_compression_configuration, 8));
    HANDLE_CODE(ethernet_header_compression_configuration.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_establishment_accept_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(selected_ssc_mode.unpack(bref));
  HANDLE_CODE(selected_pdu_session_type.unpack(bref));
  HANDLE_CODE(authorized__qo_s_rules.unpack(bref));
  HANDLE_CODE(session_ambr.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_cause_5gsm:
        cause_5gsm_present = true;
        HANDLE_CODE(cause_5gsm.unpack(bref));
        break;
      case ie_iei_pdu_address:
        pdu_address_present = true;
        HANDLE_CODE(pdu_address.unpack(bref));
        break;
      case ie_iei_rq_timer_value:
        rq_timer_value_present = true;
        HANDLE_CODE(rq_timer_value.unpack(bref));
        break;
      case ie_iei_s_nssai:
        s_nssai_present = true;
        HANDLE_CODE(s_nssai.unpack(bref));
        break;
      case ie_iei_always_on_pdu_session_indication:
        always_on_pdu_session_indication_present = true;
        HANDLE_CODE(always_on_pdu_session_indication.unpack(bref));
        break;
      case ie_iei_mapped_eps_bearer_contexts:
        mapped_eps_bearer_contexts_present = true;
        HANDLE_CODE(mapped_eps_bearer_contexts.unpack(bref));
        break;
      case ie_iei_eap_message:
        eap_message_present = true;
        HANDLE_CODE(eap_message.unpack(bref));
        break;
      case ie_iei_authorized__qo_s_flow_descriptions:
        authorized__qo_s_flow_descriptions_present = true;
        HANDLE_CODE(authorized__qo_s_flow_descriptions.unpack(bref));
        break;
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      case ie_iei_dnn:
        dnn_present = true;
        HANDLE_CODE(dnn.unpack(bref));
        break;
      case ie_iei_network_feature_support_5gsm:
        network_feature_support_5gsm_present = true;
        HANDLE_CODE(network_feature_support_5gsm.unpack(bref));
        break;
      case ie_iei_serving_plmn_rate_control:
        serving_plmn_rate_control_present = true;
        HANDLE_CODE(serving_plmn_rate_control.unpack(bref));
        break;
      case ie_iei_atsss_container:
        atsss_container_present = true;
        HANDLE_CODE(atsss_container.unpack(bref));
        break;
      case ie_iei_control_plane_only_indication:
        control_plane_only_indication_present = true;
        HANDLE_CODE(control_plane_only_indication.unpack(bref));
        break;
      case ie_iei_ip_header_compression_configuration:
        ip_header_compression_configuration_present = true;
        HANDLE_CODE(ip_header_compression_configuration.unpack(bref));
        break;
      case ie_iei_ethernet_header_compression_configuration:
        ethernet_header_compression_configuration_present = true;
        HANDLE_CODE(ethernet_header_compression_configuration.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_establishment_reject_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gsm.pack(bref));

  // Optional fields
  if (back_off_timer_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_back_off_timer_value, 8));
    HANDLE_CODE(back_off_timer_value.pack(bref));
  }
  if (allowed_ssc_mode_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_allowed_ssc_mode, 4));
    HANDLE_CODE(allowed_ssc_mode.pack(bref));
  }
  if (eap_message_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eap_message, 8));
    HANDLE_CODE(eap_message.pack(bref));
  }
  if (congestion_re_attempt_indicator_5gsm_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_congestion_re_attempt_indicator_5gsm, 8));
    HANDLE_CODE(congestion_re_attempt_indicator_5gsm.pack(bref));
  }
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }
  if (re_attempt_indicator_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_re_attempt_indicator, 8));
    HANDLE_CODE(re_attempt_indicator.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_establishment_reject_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gsm.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_back_off_timer_value:
        back_off_timer_value_present = true;
        HANDLE_CODE(back_off_timer_value.unpack(bref));
        break;
      case ie_iei_allowed_ssc_mode:
        allowed_ssc_mode_present = true;
        HANDLE_CODE(allowed_ssc_mode.unpack(bref));
        break;
      case ie_iei_eap_message:
        eap_message_present = true;
        HANDLE_CODE(eap_message.unpack(bref));
        break;
      case ie_iei_congestion_re_attempt_indicator_5gsm:
        congestion_re_attempt_indicator_5gsm_present = true;
        HANDLE_CODE(congestion_re_attempt_indicator_5gsm.unpack(bref));
        break;
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      case ie_iei_re_attempt_indicator:
        re_attempt_indicator_present = true;
        HANDLE_CODE(re_attempt_indicator.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_authentication_command_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(eap_message.pack(bref));

  // Optional fields
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_authentication_command_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(eap_message.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_authentication_complete_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(eap_message.pack(bref));

  // Optional fields
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_authentication_complete_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(eap_message.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_authentication_result_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields
  if (eap_message_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eap_message, 8));
    HANDLE_CODE(eap_message.pack(bref));
  }
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_authentication_result_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_eap_message:
        eap_message_present = true;
        HANDLE_CODE(eap_message.unpack(bref));
        break;
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_modification_request_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields
  if (capability_5gsm_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_capability_5gsm, 8));
    HANDLE_CODE(capability_5gsm.pack(bref));
  }
  if (cause_5gsm_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_cause_5gsm, 8));
    HANDLE_CODE(cause_5gsm.pack(bref));
  }
  if (maximum_number_of_supported_packet_filters_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_maximum_number_of_supported_packet_filters, 8));
    HANDLE_CODE(maximum_number_of_supported_packet_filters.pack(bref));
  }
  if (always_on_pdu_session_requested_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_always_on_pdu_session_requested, 4));
    HANDLE_CODE(always_on_pdu_session_requested.pack(bref));
  }
  if (integrity_protection_maximum_data_rate_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_integrity_protection_maximum_data_rate, 8));
    HANDLE_CODE(integrity_protection_maximum_data_rate.pack(bref));
  }
  if (requested__qo_s_rules_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_requested__qo_s_rules, 8));
    HANDLE_CODE(requested__qo_s_rules.pack(bref));
  }
  if (requested__qo_s_flow_descriptions_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_requested__qo_s_flow_descriptions, 8));
    HANDLE_CODE(requested__qo_s_flow_descriptions.pack(bref));
  }
  if (mapped_eps_bearer_contexts_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_mapped_eps_bearer_contexts, 8));
    HANDLE_CODE(mapped_eps_bearer_contexts.pack(bref));
  }
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }
  if (port_management_information_container_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_port_management_information_container, 8));
    HANDLE_CODE(port_management_information_container.pack(bref));
  }
  if (ip_header_compression_configuration_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ip_header_compression_configuration, 8));
    HANDLE_CODE(ip_header_compression_configuration.pack(bref));
  }
  if (ethernet_header_compression_configuration_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ethernet_header_compression_configuration, 8));
    HANDLE_CODE(ethernet_header_compression_configuration.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_modification_request_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_capability_5gsm:
        capability_5gsm_present = true;
        HANDLE_CODE(capability_5gsm.unpack(bref));
        break;
      case ie_iei_cause_5gsm:
        cause_5gsm_present = true;
        HANDLE_CODE(cause_5gsm.unpack(bref));
        break;
      case ie_iei_maximum_number_of_supported_packet_filters:
        maximum_number_of_supported_packet_filters_present = true;
        HANDLE_CODE(maximum_number_of_supported_packet_filters.unpack(bref));
        break;
      case ie_iei_always_on_pdu_session_requested:
        always_on_pdu_session_requested_present = true;
        HANDLE_CODE(always_on_pdu_session_requested.unpack(bref));
        break;
      case ie_iei_integrity_protection_maximum_data_rate:
        integrity_protection_maximum_data_rate_present = true;
        HANDLE_CODE(integrity_protection_maximum_data_rate.unpack(bref));
        break;
      case ie_iei_requested__qo_s_rules:
        requested__qo_s_rules_present = true;
        HANDLE_CODE(requested__qo_s_rules.unpack(bref));
        break;
      case ie_iei_requested__qo_s_flow_descriptions:
        requested__qo_s_flow_descriptions_present = true;
        HANDLE_CODE(requested__qo_s_flow_descriptions.unpack(bref));
        break;
      case ie_iei_mapped_eps_bearer_contexts:
        mapped_eps_bearer_contexts_present = true;
        HANDLE_CODE(mapped_eps_bearer_contexts.unpack(bref));
        break;
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      case ie_iei_port_management_information_container:
        port_management_information_container_present = true;
        HANDLE_CODE(port_management_information_container.unpack(bref));
        break;
      case ie_iei_ip_header_compression_configuration:
        ip_header_compression_configuration_present = true;
        HANDLE_CODE(ip_header_compression_configuration.unpack(bref));
        break;
      case ie_iei_ethernet_header_compression_configuration:
        ethernet_header_compression_configuration_present = true;
        HANDLE_CODE(ethernet_header_compression_configuration.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_modification_reject_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gsm.pack(bref));

  // Optional fields
  if (back_off_timer_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_back_off_timer_value, 8));
    HANDLE_CODE(back_off_timer_value.pack(bref));
  }
  if (congestion_re_attempt_indicator_5gsm_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_congestion_re_attempt_indicator_5gsm, 8));
    HANDLE_CODE(congestion_re_attempt_indicator_5gsm.pack(bref));
  }
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }
  if (re_attempt_indicator_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_re_attempt_indicator, 8));
    HANDLE_CODE(re_attempt_indicator.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_modification_reject_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gsm.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_back_off_timer_value:
        back_off_timer_value_present = true;
        HANDLE_CODE(back_off_timer_value.unpack(bref));
        break;
      case ie_iei_congestion_re_attempt_indicator_5gsm:
        congestion_re_attempt_indicator_5gsm_present = true;
        HANDLE_CODE(congestion_re_attempt_indicator_5gsm.unpack(bref));
        break;
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      case ie_iei_re_attempt_indicator:
        re_attempt_indicator_present = true;
        HANDLE_CODE(re_attempt_indicator.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_modification_command_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields
  if (cause_5gsm_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_cause_5gsm, 8));
    HANDLE_CODE(cause_5gsm.pack(bref));
  }
  if (session_ambr_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_session_ambr, 8));
    HANDLE_CODE(session_ambr.pack(bref));
  }
  if (rq_timer_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_rq_timer_value, 8));
    HANDLE_CODE(rq_timer_value.pack(bref));
  }
  if (always_on_pdu_session_indication_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_always_on_pdu_session_indication, 4));
    HANDLE_CODE(always_on_pdu_session_indication.pack(bref));
  }
  if (authorized__qo_s_rules_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_authorized__qo_s_rules, 8));
    HANDLE_CODE(authorized__qo_s_rules.pack(bref));
  }
  if (mapped_eps_bearer_contexts_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_mapped_eps_bearer_contexts, 8));
    HANDLE_CODE(mapped_eps_bearer_contexts.pack(bref));
  }
  if (authorized__qo_s_flow_descriptions_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_authorized__qo_s_flow_descriptions, 8));
    HANDLE_CODE(authorized__qo_s_flow_descriptions.pack(bref));
  }
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }
  if (atsss_container_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_atsss_container, 8));
    HANDLE_CODE(atsss_container.pack(bref));
  }
  if (ip_header_compression_configuration_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ip_header_compression_configuration, 8));
    HANDLE_CODE(ip_header_compression_configuration.pack(bref));
  }
  if (port_management_information_container_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_port_management_information_container, 8));
    HANDLE_CODE(port_management_information_container.pack(bref));
  }
  if (serving_plmn_rate_control_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_serving_plmn_rate_control, 8));
    HANDLE_CODE(serving_plmn_rate_control.pack(bref));
  }
  if (ethernet_header_compression_configuration_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_ethernet_header_compression_configuration, 8));
    HANDLE_CODE(ethernet_header_compression_configuration.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_modification_command_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_cause_5gsm:
        cause_5gsm_present = true;
        HANDLE_CODE(cause_5gsm.unpack(bref));
        break;
      case ie_iei_session_ambr:
        session_ambr_present = true;
        HANDLE_CODE(session_ambr.unpack(bref));
        break;
      case ie_iei_rq_timer_value:
        rq_timer_value_present = true;
        HANDLE_CODE(rq_timer_value.unpack(bref));
        break;
      case ie_iei_always_on_pdu_session_indication:
        always_on_pdu_session_indication_present = true;
        HANDLE_CODE(always_on_pdu_session_indication.unpack(bref));
        break;
      case ie_iei_authorized__qo_s_rules:
        authorized__qo_s_rules_present = true;
        HANDLE_CODE(authorized__qo_s_rules.unpack(bref));
        break;
      case ie_iei_mapped_eps_bearer_contexts:
        mapped_eps_bearer_contexts_present = true;
        HANDLE_CODE(mapped_eps_bearer_contexts.unpack(bref));
        break;
      case ie_iei_authorized__qo_s_flow_descriptions:
        authorized__qo_s_flow_descriptions_present = true;
        HANDLE_CODE(authorized__qo_s_flow_descriptions.unpack(bref));
        break;
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      case ie_iei_atsss_container:
        atsss_container_present = true;
        HANDLE_CODE(atsss_container.unpack(bref));
        break;
      case ie_iei_ip_header_compression_configuration:
        ip_header_compression_configuration_present = true;
        HANDLE_CODE(ip_header_compression_configuration.unpack(bref));
        break;
      case ie_iei_port_management_information_container:
        port_management_information_container_present = true;
        HANDLE_CODE(port_management_information_container.unpack(bref));
        break;
      case ie_iei_serving_plmn_rate_control:
        serving_plmn_rate_control_present = true;
        HANDLE_CODE(serving_plmn_rate_control.unpack(bref));
        break;
      case ie_iei_ethernet_header_compression_configuration:
        ethernet_header_compression_configuration_present = true;
        HANDLE_CODE(ethernet_header_compression_configuration.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_modification_complete_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }
  if (port_management_information_container_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_port_management_information_container, 8));
    HANDLE_CODE(port_management_information_container.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_modification_complete_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      case ie_iei_port_management_information_container:
        port_management_information_container_present = true;
        HANDLE_CODE(port_management_information_container.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_modification_command_reject_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gsm.pack(bref));

  // Optional fields
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_modification_command_reject_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gsm.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_release_request_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields
  if (cause_5gsm_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_cause_5gsm, 8));
    HANDLE_CODE(cause_5gsm.pack(bref));
  }
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_release_request_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_cause_5gsm:
        cause_5gsm_present = true;
        HANDLE_CODE(cause_5gsm.unpack(bref));
        break;
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_release_reject_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gsm.pack(bref));

  // Optional fields
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_release_reject_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gsm.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_release_command_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gsm.pack(bref));

  // Optional fields
  if (back_off_timer_value_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_back_off_timer_value, 8));
    HANDLE_CODE(back_off_timer_value.pack(bref));
  }
  if (eap_message_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_eap_message, 8));
    HANDLE_CODE(eap_message.pack(bref));
  }
  if (congestion_re_attempt_indicator_5gsm_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_congestion_re_attempt_indicator_5gsm, 8));
    HANDLE_CODE(congestion_re_attempt_indicator_5gsm.pack(bref));
  }
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }
  if (access_type_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_access_type, 4));
    HANDLE_CODE(access_type.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_release_command_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gsm.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_back_off_timer_value:
        back_off_timer_value_present = true;
        HANDLE_CODE(back_off_timer_value.unpack(bref));
        break;
      case ie_iei_eap_message:
        eap_message_present = true;
        HANDLE_CODE(eap_message.unpack(bref));
        break;
      case ie_iei_congestion_re_attempt_indicator_5gsm:
        congestion_re_attempt_indicator_5gsm_present = true;
        HANDLE_CODE(congestion_re_attempt_indicator_5gsm.unpack(bref));
        break;
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      case ie_iei_access_type:
        access_type_present = true;
        HANDLE_CODE(access_type.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE pdu_session_release_complete_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields

  // Optional fields
  if (cause_5gsm_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_cause_5gsm, 8));
    HANDLE_CODE(cause_5gsm.pack(bref));
  }
  if (extended_protocol_configuration_options_present == true) {
    HANDLE_CODE(bref.pack(ie_iei_extended_protocol_configuration_options, 8));
    HANDLE_CODE(extended_protocol_configuration_options.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_release_complete_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      case ie_iei_cause_5gsm:
        cause_5gsm_present = true;
        HANDLE_CODE(cause_5gsm.unpack(bref));
        break;
      case ie_iei_extended_protocol_configuration_options:
        extended_protocol_configuration_options_present = true;
        HANDLE_CODE(extended_protocol_configuration_options.unpack(bref));
        break;
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

SRSASN_CODE status_5gsm_t::pack(asn1::bit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gsm.pack(bref));

  // Optional fields

  return SRSASN_SUCCESS;
}
SRSASN_CODE status_5gsm_t::unpack(asn1::cbit_ref& bref)
{
  // Mandatory fields
  HANDLE_CODE(cause_5gsm.unpack(bref));

  // Optional fields

  while (bref.distance_bytes_end() > 0) {
    // some iei are only 1/2 byte long which are > 8
    // otherwise a complete byte
    uint8_t iei;
    HANDLE_CODE(bref.unpack(iei, 4));
    if (iei < 8) {
      uint8_t iei_tmp;
      HANDLE_CODE(bref.unpack(iei_tmp, 4));
      iei = iei << 4 | iei_tmp;
    }

    switch (iei) {
      default:
        asn1::log_error("Invalid IE %x", iei);
        break;
    }
  }

  return SRSASN_SUCCESS;
}

// Include from nas5g/infiles/nas_5g_msg.cc.in

SRSASN_CODE nas_5gs_hdr::unpack_outer(asn1::cbit_ref& bref)
{
  unpack_enum<extended_protocol_discriminator_opts, 8>(bref, &extended_protocol_discriminator);
  // Security header type associated with a spare half octet; or PDU session identity
  switch (extended_protocol_discriminator) {
    case extended_protocol_discriminator_5gmm:
      HANDLE_CODE(bref.advance_bits(4)); // spare
      unpack_enum<security_header_type_opts, 4>(bref, &security_header_type);
      if (security_header_type == plain_5gs_nas_message) {
        HANDLE_CODE(message_type.unpack(bref));
      } else {
        HANDLE_CODE(bref.unpack(message_authentication_code, 32));
        HANDLE_CODE(bref.unpack(sequence_number, 8));
      }
      break;
    case extended_protocol_discriminator_5gsm:
      // The PDU session identity and the procedure transaction identity are only used in messages with extended
      // protocol discriminator 5GS session management. Octet 2a with the procedure transaction identity shall only be
      // included in these messages.
      HANDLE_CODE(bref.unpack(pdu_session_identity, 8));
      HANDLE_CODE(bref.unpack(procedure_transaction_identity, 8));
      HANDLE_CODE(message_type.unpack(bref));
      break;
    default:
      asn1::log_error("Unsupported extended protocol discriminator %x\n", extended_protocol_discriminator);
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE nas_5gs_hdr::unpack(asn1::cbit_ref& bref)
{
  unpack_outer(bref);
  if (security_header_type != plain_5gs_nas_message) {
    unpack_enum<extended_protocol_discriminator_opts, 8>(bref, &inner_extended_protocol_discriminator);
    // Security header type associated with a spare half octet; or PDU session identity
    switch (inner_extended_protocol_discriminator) {
      case extended_protocol_discriminator_5gmm:
        HANDLE_CODE(bref.advance_bits(4)); // spare
        unpack_enum<security_header_type_opts, 4>(bref, &inner_security_header_type);
        if (inner_security_header_type == plain_5gs_nas_message) {
          HANDLE_CODE(message_type.unpack(bref));
        } else {
          asn1::log_error("Expected inner security type to be plain\n");
          return SRSASN_ERROR_DECODE_FAIL;
        }
        break;
      case extended_protocol_discriminator_5gsm:
        // The PDU session identity and the procedure transaction identity are only used in messages with extended
        // protocol discriminator 5GS session management. Octet 2a with the procedure transaction identity shall only be
        // included in these messages.
        HANDLE_CODE(bref.unpack(pdu_session_identity, 8));
        HANDLE_CODE(bref.unpack(procedure_transaction_identity, 8));
        HANDLE_CODE(message_type.unpack(bref));
        break;
      default:
        asn1::log_error("Unsupported extended protocol discriminator %x\n", inner_extended_protocol_discriminator);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE nas_5gs_hdr::pack_outer(asn1::bit_ref& bref)
{
  pack_enum<extended_protocol_discriminator_opts, 8>(bref, extended_protocol_discriminator);
  // Security header type associated with a spare half octet; or PDU session identity
  switch (extended_protocol_discriminator) {
    case extended_protocol_discriminator_5gmm:
      HANDLE_CODE(bref.pack(0x0, 4)); // spare
      pack_enum<security_header_type_opts, 4>(bref, security_header_type);
      if (security_header_type == plain_5gs_nas_message) {
        HANDLE_CODE(message_type.pack(bref));
      } else {
        HANDLE_CODE(bref.pack(message_authentication_code, 32));
        HANDLE_CODE(bref.pack(sequence_number, 8));
      }
      break;
    case extended_protocol_discriminator_5gsm:
      // The PDU session identity and the procedure transaction identity are only used in messages with extended
      // protocol discriminator 5GS session management. Octet 2a with the procedure transaction identity shall only be
      // included in these messages.
      HANDLE_CODE(bref.pack(pdu_session_identity, 8));
      HANDLE_CODE(bref.pack(procedure_transaction_identity, 8));
      HANDLE_CODE(message_type.pack(bref));
      break;
    default:
      asn1::log_error("Unsupported extended protocol discriminator %x\n", extended_protocol_discriminator);
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE nas_5gs_hdr::pack(asn1::bit_ref& bref)
{
  pack_outer(bref);
  if (security_header_type != plain_5gs_nas_message) {
    pack_enum<extended_protocol_discriminator_opts, 8>(bref, inner_extended_protocol_discriminator);
    if (inner_extended_protocol_discriminator == extended_protocol_discriminator_5gsm) {
      HANDLE_CODE(bref.pack(pdu_session_identity, 4));
    } else {
      HANDLE_CODE(bref.pack(0x0, 4));
    }
    pack_enum<security_header_type_opts, 4>(bref, inner_security_header_type);
    if (inner_extended_protocol_discriminator == extended_protocol_discriminator_5gsm) {
      HANDLE_CODE(bref.pack(procedure_transaction_identity, 8));
    }
    HANDLE_CODE(message_type.pack(bref));
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE nas_5gs_msg::pack(unique_byte_buffer_t& buf)
{
  asn1::bit_ref msg_bref(buf->msg, buf->get_tailroom());
  HANDLE_CODE(pack(msg_bref));
  buf->N_bytes = msg_bref.distance_bytes();
  return SRSASN_SUCCESS;
}

SRSASN_CODE nas_5gs_msg::pack(std::vector<uint8_t>& buf)
{
  buf.resize(SRSRAN_MAX_BUFFER_SIZE_BYTES);
  asn1::bit_ref msg_bref(buf.data(), buf.size());
  HANDLE_CODE(pack(msg_bref));
  buf.resize(msg_bref.distance_bytes());
  return SRSASN_SUCCESS;
}

SRSASN_CODE nas_5gs_msg::pack(asn1::bit_ref& msg_bref)
{
  HANDLE_CODE(hdr.pack(msg_bref));
  switch (hdr.message_type) {
    case msg_types::options::registration_request: {
      registration_request_t* msg = srslog::detail::any_cast<registration_request_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::registration_accept: {
      registration_accept_t* msg = srslog::detail::any_cast<registration_accept_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::registration_complete: {
      registration_complete_t* msg = srslog::detail::any_cast<registration_complete_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::registration_reject: {
      registration_reject_t* msg = srslog::detail::any_cast<registration_reject_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::deregistration_request_ue_originating: {
      deregistration_request_ue_originating_t* msg =
          srslog::detail::any_cast<deregistration_request_ue_originating_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::deregistration_accept_ue_originating: {
      deregistration_accept_ue_originating_t* msg =
          srslog::detail::any_cast<deregistration_accept_ue_originating_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::deregistration_request_ue_terminated: {
      deregistration_request_ue_terminated_t* msg =
          srslog::detail::any_cast<deregistration_request_ue_terminated_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::deregistration_accept_ue_terminated: {
      deregistration_accept_ue_terminated_t* msg =
          srslog::detail::any_cast<deregistration_accept_ue_terminated_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::service_request: {
      service_request_t* msg = srslog::detail::any_cast<service_request_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::service_reject: {
      service_reject_t* msg = srslog::detail::any_cast<service_reject_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::service_accept: {
      service_accept_t* msg = srslog::detail::any_cast<service_accept_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::configuration_update_command: {
      configuration_update_command_t* msg = srslog::detail::any_cast<configuration_update_command_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::configuration_update_complete: {
      configuration_update_complete_t* msg = srslog::detail::any_cast<configuration_update_complete_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::authentication_request: {
      authentication_request_t* msg = srslog::detail::any_cast<authentication_request_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::authentication_response: {
      authentication_response_t* msg = srslog::detail::any_cast<authentication_response_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::authentication_reject: {
      authentication_reject_t* msg = srslog::detail::any_cast<authentication_reject_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::authentication_failure: {
      authentication_failure_t* msg = srslog::detail::any_cast<authentication_failure_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::authentication_result: {
      authentication_result_t* msg = srslog::detail::any_cast<authentication_result_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::identity_request: {
      identity_request_t* msg = srslog::detail::any_cast<identity_request_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::identity_response: {
      identity_response_t* msg = srslog::detail::any_cast<identity_response_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::security_mode_command: {
      security_mode_command_t* msg = srslog::detail::any_cast<security_mode_command_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::security_mode_complete: {
      security_mode_complete_t* msg = srslog::detail::any_cast<security_mode_complete_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::security_mode_reject: {
      security_mode_reject_t* msg = srslog::detail::any_cast<security_mode_reject_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::status_5gmm: {
      status_5gmm_t* msg = srslog::detail::any_cast<status_5gmm_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::notification: {
      notification_t* msg = srslog::detail::any_cast<notification_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::notification_response: {
      notification_response_t* msg = srslog::detail::any_cast<notification_response_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::ul_nas_transport: {
      ul_nas_transport_t* msg = srslog::detail::any_cast<ul_nas_transport_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::dl_nas_transport: {
      dl_nas_transport_t* msg = srslog::detail::any_cast<dl_nas_transport_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_establishment_request: {
      pdu_session_establishment_request_t* msg =
          srslog::detail::any_cast<pdu_session_establishment_request_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_establishment_accept: {
      pdu_session_establishment_accept_t* msg =
          srslog::detail::any_cast<pdu_session_establishment_accept_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_establishment_reject: {
      pdu_session_establishment_reject_t* msg =
          srslog::detail::any_cast<pdu_session_establishment_reject_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_authentication_command: {
      pdu_session_authentication_command_t* msg =
          srslog::detail::any_cast<pdu_session_authentication_command_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_authentication_complete: {
      pdu_session_authentication_complete_t* msg =
          srslog::detail::any_cast<pdu_session_authentication_complete_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_authentication_result: {
      pdu_session_authentication_result_t* msg =
          srslog::detail::any_cast<pdu_session_authentication_result_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_modification_request: {
      pdu_session_modification_request_t* msg =
          srslog::detail::any_cast<pdu_session_modification_request_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_modification_reject: {
      pdu_session_modification_reject_t* msg =
          srslog::detail::any_cast<pdu_session_modification_reject_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_modification_command: {
      pdu_session_modification_command_t* msg =
          srslog::detail::any_cast<pdu_session_modification_command_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_modification_complete: {
      pdu_session_modification_complete_t* msg =
          srslog::detail::any_cast<pdu_session_modification_complete_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_modification_command_reject: {
      pdu_session_modification_command_reject_t* msg =
          srslog::detail::any_cast<pdu_session_modification_command_reject_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_release_request: {
      pdu_session_release_request_t* msg = srslog::detail::any_cast<pdu_session_release_request_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_release_reject: {
      pdu_session_release_reject_t* msg = srslog::detail::any_cast<pdu_session_release_reject_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_release_command: {
      pdu_session_release_command_t* msg = srslog::detail::any_cast<pdu_session_release_command_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_release_complete: {
      pdu_session_release_complete_t* msg = srslog::detail::any_cast<pdu_session_release_complete_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }
    case msg_types::options::status_5gsm: {
      status_5gsm_t* msg = srslog::detail::any_cast<status_5gsm_t>(&msg_container);
      HANDLE_CODE(msg->pack(msg_bref));
      break;
    }

    default:
      break;
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE nas_5gs_msg::unpack_outer_hdr(const unique_byte_buffer_t& buf)
{
  asn1::cbit_ref msg_bref(buf->msg, buf->N_bytes);
  HANDLE_CODE(hdr.unpack_outer(msg_bref));
  return SRSASN_SUCCESS;
}

SRSASN_CODE nas_5gs_msg::unpack_outer_hdr(const std::vector<uint8_t>& buf)
{
  asn1::cbit_ref msg_bref(buf.data(), buf.size());
  HANDLE_CODE(hdr.unpack_outer(msg_bref));
  return SRSASN_SUCCESS;
}

SRSASN_CODE nas_5gs_msg::unpack(const unique_byte_buffer_t& buf)
{
  asn1::cbit_ref msg_bref(buf->msg, buf->N_bytes);
  HANDLE_CODE(unpack(msg_bref));
  return SRSASN_SUCCESS;
}

SRSASN_CODE nas_5gs_msg::unpack(const std::vector<uint8_t>& buf)
{
  asn1::cbit_ref msg_bref(buf.data(), buf.size());
  HANDLE_CODE(unpack(msg_bref));
  return SRSASN_SUCCESS;
}

SRSASN_CODE nas_5gs_msg::unpack(asn1::cbit_ref& msg_bref)
{
  HANDLE_CODE(hdr.unpack(msg_bref));
  switch (hdr.message_type) {
    case msg_types::options::registration_request: {
      msg_container               = srslog::detail::any{registration_request_t()};
      registration_request_t* msg = srslog::detail::any_cast<registration_request_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::registration_accept: {
      msg_container              = srslog::detail::any{registration_accept_t()};
      registration_accept_t* msg = srslog::detail::any_cast<registration_accept_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::registration_complete: {
      msg_container                = srslog::detail::any{registration_complete_t()};
      registration_complete_t* msg = srslog::detail::any_cast<registration_complete_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::registration_reject: {
      msg_container              = srslog::detail::any{registration_reject_t()};
      registration_reject_t* msg = srslog::detail::any_cast<registration_reject_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::deregistration_request_ue_originating: {
      msg_container = srslog::detail::any{deregistration_request_ue_originating_t()};
      deregistration_request_ue_originating_t* msg =
          srslog::detail::any_cast<deregistration_request_ue_originating_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::deregistration_accept_ue_originating: {
      msg_container = srslog::detail::any{deregistration_accept_ue_originating_t()};
      deregistration_accept_ue_originating_t* msg =
          srslog::detail::any_cast<deregistration_accept_ue_originating_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::deregistration_request_ue_terminated: {
      msg_container = srslog::detail::any{deregistration_request_ue_terminated_t()};
      deregistration_request_ue_terminated_t* msg =
          srslog::detail::any_cast<deregistration_request_ue_terminated_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::deregistration_accept_ue_terminated: {
      msg_container = srslog::detail::any{deregistration_accept_ue_terminated_t()};
      deregistration_accept_ue_terminated_t* msg =
          srslog::detail::any_cast<deregistration_accept_ue_terminated_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::service_request: {
      msg_container          = srslog::detail::any{service_request_t()};
      service_request_t* msg = srslog::detail::any_cast<service_request_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::service_reject: {
      msg_container         = srslog::detail::any{service_reject_t()};
      service_reject_t* msg = srslog::detail::any_cast<service_reject_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::service_accept: {
      msg_container         = srslog::detail::any{service_accept_t()};
      service_accept_t* msg = srslog::detail::any_cast<service_accept_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::configuration_update_command: {
      msg_container                       = srslog::detail::any{configuration_update_command_t()};
      configuration_update_command_t* msg = srslog::detail::any_cast<configuration_update_command_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::configuration_update_complete: {
      msg_container                        = srslog::detail::any{configuration_update_complete_t()};
      configuration_update_complete_t* msg = srslog::detail::any_cast<configuration_update_complete_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::authentication_request: {
      msg_container                 = srslog::detail::any{authentication_request_t()};
      authentication_request_t* msg = srslog::detail::any_cast<authentication_request_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::authentication_response: {
      msg_container                  = srslog::detail::any{authentication_response_t()};
      authentication_response_t* msg = srslog::detail::any_cast<authentication_response_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::authentication_reject: {
      msg_container                = srslog::detail::any{authentication_reject_t()};
      authentication_reject_t* msg = srslog::detail::any_cast<authentication_reject_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::authentication_failure: {
      msg_container                 = srslog::detail::any{authentication_failure_t()};
      authentication_failure_t* msg = srslog::detail::any_cast<authentication_failure_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::authentication_result: {
      msg_container                = srslog::detail::any{authentication_result_t()};
      authentication_result_t* msg = srslog::detail::any_cast<authentication_result_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::identity_request: {
      msg_container           = srslog::detail::any{identity_request_t()};
      identity_request_t* msg = srslog::detail::any_cast<identity_request_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::identity_response: {
      msg_container            = srslog::detail::any{identity_response_t()};
      identity_response_t* msg = srslog::detail::any_cast<identity_response_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::security_mode_command: {
      msg_container                = srslog::detail::any{security_mode_command_t()};
      security_mode_command_t* msg = srslog::detail::any_cast<security_mode_command_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::security_mode_complete: {
      msg_container                 = srslog::detail::any{security_mode_complete_t()};
      security_mode_complete_t* msg = srslog::detail::any_cast<security_mode_complete_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::security_mode_reject: {
      msg_container               = srslog::detail::any{security_mode_reject_t()};
      security_mode_reject_t* msg = srslog::detail::any_cast<security_mode_reject_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::status_5gmm: {
      msg_container      = srslog::detail::any{status_5gmm_t()};
      status_5gmm_t* msg = srslog::detail::any_cast<status_5gmm_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::notification: {
      msg_container       = srslog::detail::any{notification_t()};
      notification_t* msg = srslog::detail::any_cast<notification_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::notification_response: {
      msg_container                = srslog::detail::any{notification_response_t()};
      notification_response_t* msg = srslog::detail::any_cast<notification_response_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::ul_nas_transport: {
      msg_container           = srslog::detail::any{ul_nas_transport_t()};
      ul_nas_transport_t* msg = srslog::detail::any_cast<ul_nas_transport_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::dl_nas_transport: {
      msg_container           = srslog::detail::any{dl_nas_transport_t()};
      dl_nas_transport_t* msg = srslog::detail::any_cast<dl_nas_transport_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_establishment_request: {
      msg_container = srslog::detail::any{pdu_session_establishment_request_t()};
      pdu_session_establishment_request_t* msg =
          srslog::detail::any_cast<pdu_session_establishment_request_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_establishment_accept: {
      msg_container = srslog::detail::any{pdu_session_establishment_accept_t()};
      pdu_session_establishment_accept_t* msg =
          srslog::detail::any_cast<pdu_session_establishment_accept_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_establishment_reject: {
      msg_container = srslog::detail::any{pdu_session_establishment_reject_t()};
      pdu_session_establishment_reject_t* msg =
          srslog::detail::any_cast<pdu_session_establishment_reject_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_authentication_command: {
      msg_container = srslog::detail::any{pdu_session_authentication_command_t()};
      pdu_session_authentication_command_t* msg =
          srslog::detail::any_cast<pdu_session_authentication_command_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_authentication_complete: {
      msg_container = srslog::detail::any{pdu_session_authentication_complete_t()};
      pdu_session_authentication_complete_t* msg =
          srslog::detail::any_cast<pdu_session_authentication_complete_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_authentication_result: {
      msg_container = srslog::detail::any{pdu_session_authentication_result_t()};
      pdu_session_authentication_result_t* msg =
          srslog::detail::any_cast<pdu_session_authentication_result_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_modification_request: {
      msg_container = srslog::detail::any{pdu_session_modification_request_t()};
      pdu_session_modification_request_t* msg =
          srslog::detail::any_cast<pdu_session_modification_request_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_modification_reject: {
      msg_container = srslog::detail::any{pdu_session_modification_reject_t()};
      pdu_session_modification_reject_t* msg =
          srslog::detail::any_cast<pdu_session_modification_reject_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_modification_command: {
      msg_container = srslog::detail::any{pdu_session_modification_command_t()};
      pdu_session_modification_command_t* msg =
          srslog::detail::any_cast<pdu_session_modification_command_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_modification_complete: {
      msg_container = srslog::detail::any{pdu_session_modification_complete_t()};
      pdu_session_modification_complete_t* msg =
          srslog::detail::any_cast<pdu_session_modification_complete_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_modification_command_reject: {
      msg_container = srslog::detail::any{pdu_session_modification_command_reject_t()};
      pdu_session_modification_command_reject_t* msg =
          srslog::detail::any_cast<pdu_session_modification_command_reject_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_release_request: {
      msg_container                      = srslog::detail::any{pdu_session_release_request_t()};
      pdu_session_release_request_t* msg = srslog::detail::any_cast<pdu_session_release_request_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_release_reject: {
      msg_container                     = srslog::detail::any{pdu_session_release_reject_t()};
      pdu_session_release_reject_t* msg = srslog::detail::any_cast<pdu_session_release_reject_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_release_command: {
      msg_container                      = srslog::detail::any{pdu_session_release_command_t()};
      pdu_session_release_command_t* msg = srslog::detail::any_cast<pdu_session_release_command_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::pdu_session_release_complete: {
      msg_container                       = srslog::detail::any{pdu_session_release_complete_t()};
      pdu_session_release_complete_t* msg = srslog::detail::any_cast<pdu_session_release_complete_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }
    case msg_types::options::status_5gsm: {
      msg_container      = srslog::detail::any{status_5gsm_t()};
      status_5gsm_t* msg = srslog::detail::any_cast<status_5gsm_t>(&msg_container);
      HANDLE_CODE(msg->unpack(msg_bref));
      break;
    }

    default:
      break;
  }
  return SRSASN_SUCCESS;
}

} // namespace nas_5g
} // namespace srsran
