/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsenb/hdr/stack/ngap/ngap_ue.h"
#include "srsenb/hdr/stack/ngap/ngap.h"
#include "srsenb/hdr/stack/ngap/ngap_ue_proc.h"
#include "srsran/common/int_helpers.h"

using namespace asn1::ngap_nr;

namespace srsenb {

/*******************************************************************************
/*               ngap_ptr::ue Class
********************************************************************************/

ngap::ue::ue(ngap* ngap_ptr_, rrc_interface_ngap_nr* rrc_ptr_, srslog::basic_logger& logger_) :
  logger(logger_),
  ngap_ptr(ngap_ptr_),
  rrc_ptr(rrc_ptr_),
  initial_context_setup_proc(this, rrc_ptr, &ctxt),
  ue_context_release_proc(this, rrc_ptr, &ctxt)
{
  ctxt.ran_ue_ngap_id = ngap_ptr->next_gnb_ue_ngap_id++;
  gettimeofday(&ctxt.init_timestamp, nullptr);
  stream_id = ngap_ptr->next_ue_stream_id;
}

ngap::ue::~ue() {}

/*******************************************************************************
/* NGAP message senders
********************************************************************************/

bool ngap::ue::send_initial_ue_message(asn1::ngap_nr::rrcestablishment_cause_e cause,
                                       srsran::unique_byte_buffer_t            pdu,
                                       bool                                    has_tmsi,
                                       uint32_t                                s_tmsi)
{
  if (not ngap_ptr->amf_connected) {
    logger.warning("AMF not connected");
    return false;
  }

  ngap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_NGAP_NR_ID_INIT_UE_MSG);
  init_ue_msg_ies_container& container = tx_pdu.init_msg().value.init_ue_msg().protocol_ies;

  // 5G-S-TMSI
  if (has_tmsi) {
    container.five_g_s_tmsi_present = true;
    srsran::uint32_to_uint8(s_tmsi, container.five_g_s_tmsi.value.five_g_tmsi.data());
    container.five_g_s_tmsi.value.amf_set_id.from_number(ctxt.amf_set_id);
    container.five_g_s_tmsi.value.amf_pointer.from_number(ctxt.amf_pointer);
  }

  // RAN_UE_NGAP_ID
  container.ran_ue_ngap_id.value = ctxt.ran_ue_ngap_id;

  // NAS_PDU
  container.nas_pdu.value.resize(pdu->N_bytes);
  memcpy(container.nas_pdu.value.data(), pdu->msg, pdu->N_bytes);

  // RRC Establishment Cause
  container.rrcestablishment_cause.value = cause;

  // User Location Info

  // userLocationInformationNR
  container.user_location_info.value.set_user_location_info_nr();
  container.user_location_info.value.user_location_info_nr().nr_cgi.nrcell_id = ngap_ptr->nr_cgi.nrcell_id;
  container.user_location_info.value.user_location_info_nr().nr_cgi.plmn_id   = ngap_ptr->nr_cgi.plmn_id;
  container.user_location_info.value.user_location_info_nr().tai.plmn_id      = ngap_ptr->tai.plmn_id;
  container.user_location_info.value.user_location_info_nr().tai.tac          = ngap_ptr->tai.tac;

  return ngap_ptr->sctp_send_ngap_pdu(tx_pdu, ctxt.rnti, "InitialUEMessage");
}

bool ngap::ue::send_ul_nas_transport(srsran::unique_byte_buffer_t pdu)
{
  if (not ngap_ptr->amf_connected) {
    logger.warning("AMF not connected");
    return false;
  }

  ngap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_NGAP_NR_ID_UL_NAS_TRANSPORT);
  asn1::ngap_nr::ul_nas_transport_ies_container& container = tx_pdu.init_msg().value.ul_nas_transport().protocol_ies;

  // AMF UE NGAP ID
  container.amf_ue_ngap_id.value = ctxt.amf_ue_ngap_id.value();

  // RAN UE NGAP ID
  container.ran_ue_ngap_id.value = ctxt.ran_ue_ngap_id;

  // NAS PDU
  container.nas_pdu.value.resize(pdu->N_bytes);
  memcpy(container.nas_pdu.value.data(), pdu->msg, pdu->N_bytes);

  // User Location Info
  // userLocationInformationNR
  container.user_location_info.value.set_user_location_info_nr();
  container.user_location_info.value.user_location_info_nr().nr_cgi.nrcell_id = ngap_ptr->nr_cgi.nrcell_id;
  container.user_location_info.value.user_location_info_nr().nr_cgi.plmn_id   = ngap_ptr->nr_cgi.plmn_id;
  container.user_location_info.value.user_location_info_nr().tai.plmn_id      = ngap_ptr->tai.plmn_id;
  container.user_location_info.value.user_location_info_nr().tai.tac          = ngap_ptr->tai.tac;

  return ngap_ptr->sctp_send_ngap_pdu(tx_pdu, ctxt.rnti, "UplinkNASTransport");
}

void ngap::ue::ue_ctxt_setup_complete()
{
  ngap_pdu_c tx_pdu;
  // Handle PDU Session List once RRC interface is ready
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_NGAP_NR_ID_INIT_CONTEXT_SETUP);
  auto& container = tx_pdu.successful_outcome().value.init_context_setup_resp().protocol_ies;
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
  tx_pdu.set_init_msg().load_info_obj(ASN1_NGAP_NR_ID_INIT_CONTEXT_SETUP);
  init_context_setup_resp_s& container = tx_pdu.successful_outcome().value.init_context_setup_resp();

  // AMF UE NGAP ID
  container.protocol_ies.amf_ue_ngap_id.value = ctxt.amf_ue_ngap_id.value();

  // RAN UE NGAP ID
  container.protocol_ies.ran_ue_ngap_id.value = ctxt.ran_ue_ngap_id;

  /* // TODO: PDU Session Resource Setup Response List - Integrate PDU Session and Bearer management into NGAP
  container.protocol_ies.pdu_session_res_setup_list_cxt_res_present = true;

  // Case PDU Session Resource Failed to Setup List
  container.protocol_ies.pdu_session_res_failed_to_setup_list_cxt_res_present = true; */

  return true;
}

bool ngap::ue::send_initial_ctxt_setup_failure(cause_c cause)
{
  if (not ngap_ptr->amf_connected) {
    logger.warning("AMF not connected");
    return false;
  }

  ngap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_NGAP_NR_ID_INIT_CONTEXT_SETUP);
  init_context_setup_fail_s& container = tx_pdu.unsuccessful_outcome().value.init_context_setup_fail();

  // AMF UE NGAP ID
  container.protocol_ies.amf_ue_ngap_id.value = ctxt.amf_ue_ngap_id.value();

  // RAN UE NGAP ID
  container.protocol_ies.ran_ue_ngap_id.value = ctxt.ran_ue_ngap_id;

  /* // TODO: PDU Session Resource Setup Response List - Integrate PDU Session and Bearer management into NGAP
  container.protocol_ies.pdu_session_res_setup_list_cxt_res_present = true;

  // Case PDU Session Resource Failed to Setup List
  container.protocol_ies.pdu_session_res_failed_to_setup_list_cxt_res_present = true; */

  return true;
}

/*******************************************************************************
/* NGAP message handler
********************************************************************************/

bool ngap::ue::handle_initial_ctxt_setup_request(const asn1::ngap_nr::init_context_setup_request_s& msg)
{
  if (not initial_context_setup_proc.launch(msg)) {
    logger.error("Failed to start Initial Context Setup Procedure");
    return false;
  }
  return true;
}

bool ngap::ue::handle_ue_ctxt_release_cmd(const asn1::ngap_nr::ue_context_release_cmd_s& msg)
{
  // TODO: Release UE context
  if (not ue_context_release_proc.launch(msg)) {
    logger.error("Failed to start UE Context Release Procedure");
    return false;
  }
  return true;
}

} // namespace srsenb