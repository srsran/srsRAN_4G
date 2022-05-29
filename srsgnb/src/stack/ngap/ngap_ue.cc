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

#include "srsgnb/hdr/stack/ngap/ngap_ue.h"
#include "srsgnb/hdr/stack/ngap/ngap.h"
#include "srsgnb/hdr/stack/ngap/ngap_ue_proc.h"
#include "srsran/common/int_helpers.h"

using namespace asn1::ngap;

namespace srsenb {

/*******************************************************************************
/*               ngap_ptr::ue Class
********************************************************************************/

ngap::ue::ue(ngap*                  ngap_ptr_,
             rrc_interface_ngap_nr* rrc_ptr_,
             gtpu_interface_rrc*    gtpu_ptr_,
             srslog::basic_logger&  logger_) :
  logger(logger_),
  ngap_ptr(ngap_ptr_),
  bearer_manager(gtpu_ptr_, logger_),
  initial_context_setup_proc(this, rrc_ptr_, &ctxt, logger_),
  ue_context_release_proc(this, rrc_ptr_, &ctxt, &bearer_manager, logger_),
  ue_pdu_session_res_setup_proc(this, rrc_ptr_, &ctxt, &bearer_manager, logger_)
{
  ctxt.ran_ue_ngap_id = ngap_ptr->next_gnb_ue_ngap_id++;
  gettimeofday(&ctxt.init_timestamp, nullptr);
  stream_id = ngap_ptr->next_ue_stream_id;
}

ngap::ue::~ue() {}

/*******************************************************************************
/* NGAP message senders
********************************************************************************/

bool ngap::ue::send_initial_ue_message(asn1::ngap::rrcestablishment_cause_e cause,
                                       srsran::const_byte_span              pdu,
                                       bool                                 has_tmsi,
                                       uint32_t                             s_tmsi)
{
  if (not ngap_ptr->amf_connected) {
    logger.warning("AMF not connected");
    return false;
  }

  ngap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_NGAP_ID_INIT_UE_MSG);
  init_ue_msg_s& container = tx_pdu.init_msg().value.init_ue_msg();

  // 5G-S-TMSI
  if (has_tmsi) {
    container->five_g_s_tmsi_present = true;
    srsran::uint32_to_uint8(s_tmsi, container->five_g_s_tmsi.value.five_g_tmsi.data());
    container->five_g_s_tmsi.value.amf_set_id.from_number(ctxt.amf_set_id);
    container->five_g_s_tmsi.value.amf_pointer.from_number(ctxt.amf_pointer);
  }

  // RAN_UE_NGAP_ID
  container->ran_ue_ngap_id.value = ctxt.ran_ue_ngap_id;

  // NAS_PDU
  container->nas_pdu.value.resize(pdu.size());
  memcpy(container->nas_pdu.value.data(), pdu.data(), pdu.size());

  // RRC Establishment Cause
  container->rrcestablishment_cause.value = cause;

  // User Location Info

  // userLocationInformationNR
  container->user_location_info.value.set_user_location_info_nr();
  container->user_location_info.value.user_location_info_nr().nr_cgi.nrcell_id = ngap_ptr->nr_cgi.nrcell_id;
  container->user_location_info.value.user_location_info_nr().nr_cgi.plmn_id   = ngap_ptr->nr_cgi.plmn_id;
  container->user_location_info.value.user_location_info_nr().tai.plmn_id      = ngap_ptr->tai.plmn_id;
  container->user_location_info.value.user_location_info_nr().tai.tac          = ngap_ptr->tai.tac;

  // UE context request for setup in the NAS registration request
  container->ue_context_request_present = true;
  container->ue_context_request.value   = asn1::ngap::ue_context_request_opts::options::requested;

  return ngap_ptr->sctp_send_ngap_pdu(tx_pdu, ctxt.rnti, "InitialUEMessage");
}

bool ngap::ue::send_ul_nas_transport(srsran::const_byte_span pdu)
{
  if (not ngap_ptr->amf_connected) {
    logger.warning("AMF not connected");
    return false;
  }

  ngap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_NGAP_ID_UL_NAS_TRANSPORT);
  ul_nas_transport_s& container = tx_pdu.init_msg().value.ul_nas_transport();

  // AMF UE NGAP ID
  if (ctxt.amf_ue_ngap_id.has_value()) {
    container->amf_ue_ngap_id.value = ctxt.amf_ue_ngap_id.value();
  } else {
    logger.error("Attempting to send UL NAS Transport without AMF context");
    return false;
  }

  // RAN UE NGAP ID
  container->ran_ue_ngap_id.value = ctxt.ran_ue_ngap_id;

  // NAS PDU
  container->nas_pdu.value.resize(pdu.size());
  memcpy(container->nas_pdu.value.data(), pdu.data(), pdu.size());

  // User Location Info
  // userLocationInformationNR
  container->user_location_info.value.set_user_location_info_nr();
  container->user_location_info.value.user_location_info_nr().nr_cgi.nrcell_id = ngap_ptr->nr_cgi.nrcell_id;
  container->user_location_info.value.user_location_info_nr().nr_cgi.plmn_id   = ngap_ptr->nr_cgi.plmn_id;
  container->user_location_info.value.user_location_info_nr().tai.plmn_id      = ngap_ptr->tai.plmn_id;
  container->user_location_info.value.user_location_info_nr().tai.tac          = ngap_ptr->tai.tac;

  return ngap_ptr->sctp_send_ngap_pdu(tx_pdu, ctxt.rnti, "UplinkNASTransport");
}

void ngap::ue::notify_rrc_reconf_complete(const bool outcome)
{
  initial_context_setup_proc.trigger(outcome);
}

bool ngap::ue::send_initial_ctxt_setup_response()
{
  if (not ngap_ptr->amf_connected) {
    logger.warning("AMF not connected");
    return false;
  }

  ngap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_NGAP_ID_INIT_CONTEXT_SETUP);
  init_context_setup_resp_s& container = tx_pdu.successful_outcome().value.init_context_setup_resp();

  // AMF UE NGAP ID
  container->amf_ue_ngap_id.value = ctxt.amf_ue_ngap_id.value();

  // RAN UE NGAP ID
  container->ran_ue_ngap_id.value = ctxt.ran_ue_ngap_id;

  return ngap_ptr->sctp_send_ngap_pdu(tx_pdu, ctxt.rnti, "InitialContextSetupResponse");
}

bool ngap::ue::send_initial_ctxt_setup_failure(cause_c cause)
{
  if (not ngap_ptr->amf_connected) {
    logger.warning("AMF not connected");
    return false;
  }

  ngap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_NGAP_ID_INIT_CONTEXT_SETUP);
  init_context_setup_fail_s& container = tx_pdu.unsuccessful_outcome().value.init_context_setup_fail();

  // AMF UE NGAP ID
  container->amf_ue_ngap_id.value = ctxt.amf_ue_ngap_id.value();

  // RAN UE NGAP ID
  container->ran_ue_ngap_id.value = ctxt.ran_ue_ngap_id;

  /* // TODO: PDU Session Resource Setup Response List - Integrate PDU Session and Bearer management into NGAP
  container->pdu_session_res_setup_list_cxt_res_present = true;

  // Case PDU Session Resource Failed to Setup List
  container->pdu_session_res_failed_to_setup_list_cxt_res_present = true; */

  return true;
}

bool ngap::ue::send_pdu_session_resource_setup_response(uint16_t                                    pdu_session_id,
                                                        uint32_t                                    teid_in,
                                                        asn1::bounded_bitstring<1, 160, true, true> addr_in)
{
  if (not ngap_ptr->amf_connected) {
    logger.warning("AMF not connected");
    return false;
  }
  // TODO: QOS Params
  ngap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_NGAP_ID_PDU_SESSION_RES_SETUP);
  pdu_session_res_setup_resp_s& container              = tx_pdu.successful_outcome().value.pdu_session_res_setup_resp();
  container->amf_ue_ngap_id.value                      = ctxt.amf_ue_ngap_id.value();
  container->ran_ue_ngap_id.value                      = ctxt.ran_ue_ngap_id;
  container->pdu_session_res_setup_list_su_res_present = true;
  pdu_session_res_setup_item_su_res_s su_res;
  su_res.pdu_session_res_setup_resp_transfer.resize(512);
  su_res.pdu_session_id = pdu_session_id;
  pdu_session_res_setup_resp_transfer_s resp_transfer;

  gtp_tunnel_s& gtp_tunnel = resp_transfer.dlqos_flow_per_tnl_info.uptransport_layer_info.set_gtp_tunnel();

  gtp_tunnel.gtp_teid.from_number(teid_in);
  gtp_tunnel.transport_layer_address = addr_in;
  asn1::ngap::associated_qos_flow_list_l qos_flow_list;
  asn1::ngap::associated_qos_flow_item_s qos_flow_item;
  qos_flow_item.qos_flow_id = 1;
  qos_flow_list.push_back(qos_flow_item);
  resp_transfer.dlqos_flow_per_tnl_info.associated_qos_flow_list = qos_flow_list;

  asn1::bit_ref bref(su_res.pdu_session_res_setup_resp_transfer.data(),
                     su_res.pdu_session_res_setup_resp_transfer.size());
  resp_transfer.pack(bref);
  su_res.pdu_session_res_setup_resp_transfer.resize(bref.distance_bytes());

  container->pdu_session_res_setup_list_su_res.value.push_back(su_res);
  return ngap_ptr->sctp_send_ngap_pdu(tx_pdu, ctxt.rnti, "PDUSessionResourceSetupResponse");
}

bool ngap::ue::send_ue_ctxt_release_complete()
{
  if (not ngap_ptr->amf_connected) {
    logger.warning("AMF not connected");
    return false;
  }

  ngap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_RELEASE);
  ue_context_release_complete_s& container = tx_pdu.successful_outcome().value.ue_context_release_complete();

  // userLocationInformationNR
  container->user_location_info.value.set_user_location_info_nr();
  container->user_location_info.value.user_location_info_nr().nr_cgi.nrcell_id = ngap_ptr->nr_cgi.nrcell_id;
  container->user_location_info.value.user_location_info_nr().nr_cgi.plmn_id   = ngap_ptr->nr_cgi.plmn_id;
  container->user_location_info.value.user_location_info_nr().tai.plmn_id      = ngap_ptr->tai.plmn_id;
  container->user_location_info.value.user_location_info_nr().tai.tac          = ngap_ptr->tai.tac;

  container->ran_ue_ngap_id.value = ctxt.ran_ue_ngap_id;
  container->amf_ue_ngap_id.value = ctxt.amf_ue_ngap_id.value();

  return ngap_ptr->sctp_send_ngap_pdu(tx_pdu, ctxt.rnti, "UEContextReleaseComplete");
}

bool ngap::ue::send_ue_context_release_request(asn1::ngap::cause_c cause)
{
  if (not ngap_ptr->amf_connected) {
    logger.warning("AMF not connected");
    return false;
  }

  if (not ctxt.amf_ue_ngap_id.has_value()) {
    logger.warning("Can't send release request. User 0x%x has no AMF UE Id.", ctxt.rnti);
    return false;
  }

  if (was_ue_context_release_requested()) {
    // let timeout auto-remove user.
    return false;
  }
  release_requested = true;

  ngap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_RELEASE_REQUEST);
  ue_context_release_request_s& container = tx_pdu.init_msg().value.ue_context_release_request();

  container->cause.value = cause;

  // PDU Session Resource List
  auto& session_lst = container->pdu_session_res_list_cxt_rel_req.value;
  for (const auto& pdu_pair : bearer_manager.pdu_sessions()) {
    const ngap_ue_bearer_manager::pdu_session_t& session = pdu_pair.second;

    pdu_session_res_item_cxt_rel_req_s obj;
    obj.pdu_session_id = session.id;
    session_lst.push_back(obj);
  }
  container->pdu_session_res_list_cxt_rel_req_present = session_lst.size() > 0;

  container->ran_ue_ngap_id.value = ctxt.ran_ue_ngap_id;
  container->amf_ue_ngap_id.value = ctxt.amf_ue_ngap_id.value();

  // TODO: Implement timeout
  return ngap_ptr->sctp_send_ngap_pdu(tx_pdu, ctxt.rnti, "UEContextReleaseRequest");
}

/*******************************************************************************
/* NGAP message handler
********************************************************************************/

bool ngap::ue::handle_initial_ctxt_setup_request(const asn1::ngap::init_context_setup_request_s& msg)
{
  if (not initial_context_setup_proc.launch(msg)) {
    logger.error("Failed to start Initial Context Setup Procedure");
    return false;
  }
  return true;
}

bool ngap::ue::handle_ue_context_release_cmd(const asn1::ngap::ue_context_release_cmd_s& msg)
{
  // TODO: Release UE context
  if (not ue_context_release_proc.launch(msg)) {
    logger.error("Failed to start UE Context Release Procedure");
    return false;
  }
  return true;
}

bool ngap::ue::handle_pdu_session_res_setup_request(const asn1::ngap::pdu_session_res_setup_request_s& msg)
{
  if (not ue_pdu_session_res_setup_proc.launch(msg)) {
    logger.error("Failed to start UE PDU Session Resource Setup Procedure");
    return false;
  }
  return true;
}

} // namespace srsenb
