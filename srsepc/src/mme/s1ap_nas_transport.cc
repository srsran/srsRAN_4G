/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <iostream>
#include <cmath>
#include <inttypes.h> // for printing uint64_t
#include "srsepc/hdr/mme/s1ap.h"
#include "srsepc/hdr/mme/s1ap_nas_transport.h"
#include "srslte/common/security.h"
#include "srslte/common/liblte_security.h"

namespace srsepc{

s1ap_nas_transport*          s1ap_nas_transport::m_instance = NULL;
pthread_mutex_t s1ap_nas_transport_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

s1ap_nas_transport::s1ap_nas_transport()
{
  return;
}

s1ap_nas_transport::~s1ap_nas_transport()
{
  return;
}

s1ap_nas_transport*
s1ap_nas_transport::get_instance(void)
{
  pthread_mutex_lock(&s1ap_nas_transport_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new s1ap_nas_transport();
  }
  pthread_mutex_unlock(&s1ap_nas_transport_instance_mutex);
  return(m_instance);
}

void
s1ap_nas_transport::cleanup(void)
{
  pthread_mutex_lock(&s1ap_nas_transport_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&s1ap_nas_transport_instance_mutex);
}

void
s1ap_nas_transport::init(hss_interface_nas * hss_)
{
  m_s1ap = s1ap::get_instance();
  m_s1ap_log = m_s1ap->m_s1ap_log;
  m_pool = srslte::byte_buffer_pool::get_instance();

  m_hss = hss_;
  m_mme_gtpc = mme_gtpc::get_instance();
}


bool
s1ap_nas_transport::handle_initial_ue_message(LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT *init_ue, struct sctp_sndrcvinfo *enb_sri, srslte::byte_buffer_t *reply_buffer, bool *reply_flag)
{
  bool err, mac_valid;
  uint8_t pd, msg_type, sec_hdr_type;
  srslte::byte_buffer_t *nas_msg = m_pool->allocate();
  memcpy(nas_msg->msg, &init_ue->NAS_PDU.buffer, init_ue->NAS_PDU.n_octets);
  nas_msg->N_bytes = init_ue->NAS_PDU.n_octets;

  uint32_t m_tmsi;
  uint32_t enb_ue_s1ap_id = init_ue->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID;
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &pd, &msg_type);

  m_s1ap_log->console("Initial UE message: %s\n", liblte_nas_msg_type_to_string(msg_type));
  m_s1ap_log->info   ("Initial UE message: %s\n", liblte_nas_msg_type_to_string(msg_type));

  //Make sure M-TMSI is present, if mandatory
  if (msg_type != LIBLTE_MME_MSG_TYPE_ATTACH_REQUEST && !init_ue->S_TMSI_present){
    m_s1ap_log->error("Initial UE Message 0x%x -- S-TMSI not present\n", msg_type);
    m_s1ap_log->console("Initial UE message 0x%x -- S-TMSI not present\n", msg_type);
    m_pool->deallocate(nas_msg);
    return false;
  }
  if (init_ue->S_TMSI_present) {
    m_tmsi = ntohl(*((uint32_t*)&init_ue->S_TMSI.m_TMSI.buffer));
  }

  switch (msg_type)
  {
  case LIBLTE_MME_MSG_TYPE_ATTACH_REQUEST:
    m_s1ap_log->console("Received Initial UE message -- Attach Request\n");
    m_s1ap_log->info("Received Initial UE message -- Attach Request\n");
    err = handle_nas_attach_request(enb_ue_s1ap_id, nas_msg, reply_buffer,reply_flag, enb_sri);
    break;
  case LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST:
    m_s1ap_log->console("Received Initial UE message -- Service Request\n");
    m_s1ap_log->info("Received Initial UE message -- Service Request\n");
    err = handle_nas_service_request(m_tmsi, enb_ue_s1ap_id, nas_msg, reply_buffer,reply_flag, enb_sri);
    break;
  case LIBLTE_MME_MSG_TYPE_DETACH_REQUEST:
    m_s1ap_log->console("Received Initial UE message -- Detach Request\n");
    m_s1ap_log->info("Received Initial UE message -- Detach Request\n");
    err = handle_nas_detach_request(m_tmsi, enb_ue_s1ap_id, nas_msg, reply_buffer,reply_flag, enb_sri);
    break;
  case LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_REQUEST:
    m_s1ap_log->console("Received Initial UE message -- Tracking Area Update Request\n");
    m_s1ap_log->info("Received Initial UE message -- Tracking Area Update Request\n");
    err = handle_nas_tracking_area_update_request(m_tmsi, enb_ue_s1ap_id, nas_msg, reply_buffer,reply_flag, enb_sri);
    break;
  default:
    m_s1ap_log->info("Unhandled Initial UE Message 0x%x \n", msg_type);
    m_s1ap_log->console("Unhandled Initial UE Message 0x%x \n", msg_type);
    err = false;
  }
  m_pool->deallocate(nas_msg);
  return err;
}

bool
s1ap_nas_transport::handle_uplink_nas_transport(LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT *ul_xport, struct sctp_sndrcvinfo *enb_sri, srslte::byte_buffer_t *reply_buffer, bool *reply_flag)
{
  uint8_t pd, msg_type, sec_hdr_type;
  uint32_t enb_ue_s1ap_id = ul_xport->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID;
  uint32_t mme_ue_s1ap_id = ul_xport->MME_UE_S1AP_ID.MME_UE_S1AP_ID;
  bool mac_valid = false;

  //Get UE ECM context
  nas *nas_ctx = m_s1ap->find_nas_ctx_from_mme_ue_s1ap_id(mme_ue_s1ap_id);
  if (nas_ctx == NULL) {
    m_s1ap_log->warning("Received uplink NAS, but could not find UE NAS context. MME-UE S1AP id: %d\n",mme_ue_s1ap_id);
    return false;
  }

  m_s1ap_log->debug("Received uplink NAS and found UE NAS context. MME-UE S1AP id: %d\n",mme_ue_s1ap_id);
  emm_ctx_t *emm_ctx = &nas_ctx->m_emm_ctx;
  ecm_ctx_t *ecm_ctx = &nas_ctx->m_ecm_ctx;
  sec_ctx_t *sec_ctx = &nas_ctx->m_sec_ctx;

  //Parse NAS message header
  srslte::byte_buffer_t *nas_msg = m_pool->allocate();
  memcpy(nas_msg->msg, &ul_xport->NAS_PDU.buffer, ul_xport->NAS_PDU.n_octets);
  nas_msg->N_bytes = ul_xport->NAS_PDU.n_octets;
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &pd, &msg_type);

  // Parse the message security header
  liblte_mme_parse_msg_sec_header((LIBLTE_BYTE_MSG_STRUCT*)nas_msg, &pd, &sec_hdr_type);

  //Find UE EMM context if message is security protected.
  if(sec_hdr_type != LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS)
  {
    //Make sure EMM context is set-up, to do integrity check/de-chiphering
    if(emm_ctx->imsi == 0)
    {
      //No EMM context found.
      //Perhaps a temporary context is being created?
      //This can happen with integrity protected identity reponse messages
      if( !(msg_type == LIBLTE_MME_MSG_TYPE_IDENTITY_RESPONSE && sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY))
      {
        m_s1ap_log->warning("Uplink NAS: could not find security context for integrity protected message. MME-UE S1AP id: %d\n",mme_ue_s1ap_id);
        m_pool->deallocate(nas_msg);
        return false;
      }
    }
  }

  if( sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS ||
      (msg_type == LIBLTE_MME_MSG_TYPE_IDENTITY_RESPONSE && sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY) ||
      (msg_type == LIBLTE_MME_MSG_TYPE_AUTHENTICATION_RESPONSE && sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY) ||
      (msg_type == LIBLTE_MME_MSG_TYPE_AUTHENTICATION_FAILURE && sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY)) {

    //Only identity response and authentication response are valid as plain NAS.
    //Sometimes authentication response/failure and identity response are sent as integrity protected,
    //but these messages are sent when the securty context is not setup yet, so we cannot integrity check it.
    switch(msg_type)
    {
    case LIBLTE_MME_MSG_TYPE_IDENTITY_RESPONSE:
      m_s1ap_log->info("Uplink NAS: Received Identity Response\n");
      m_s1ap_log->console("Uplink NAS: Received Identity Response\n");
      nas_ctx->handle_identity_response(nas_msg, reply_buffer, reply_flag);
      break;
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_RESPONSE:
      m_s1ap_log->info("Uplink NAS: Received Authentication Response\n");
      m_s1ap_log->console("Uplink NAS: Received Authentication Response\n");
      nas_ctx->handle_nas_authentication_response(nas_msg, reply_buffer, reply_flag);
      break;
    // Authentication failure with the option sync failure can be sent not integrity protected
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_FAILURE:
      m_s1ap_log->info("Plain UL NAS: Authentication Failure\n");
      m_s1ap_log->console("Plain UL NAS: Authentication Failure\n");
      nas_ctx->handle_authentication_failure(nas_msg, reply_buffer, reply_flag);
      break;
    // Detach request can be sent not integrity protected when "power off" option is used
    case LIBLTE_MME_MSG_TYPE_DETACH_REQUEST:
      m_s1ap_log->info("Plain Protected UL NAS: Detach Request\n");
      m_s1ap_log->console("Plain Protected UL NAS: Detach Request\n");
      nas_ctx->handle_nas_detach_request(nas_msg, reply_buffer, reply_flag);
      break;
    default:
      m_s1ap_log->warning("Unhandled Plain NAS message 0x%x\n", msg_type );
      m_s1ap_log->console("Unhandled Plain NAS message 0x%x\n", msg_type );
      m_pool->deallocate(nas_msg);
      return false;
    }
    //Increment UL NAS count.
    sec_ctx->ul_nas_count++;
  } else if (sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_WITH_NEW_EPS_SECURITY_CONTEXT ||
             sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED_WITH_NEW_EPS_SECURITY_CONTEXT) {

    //Integrity Protected Messages, possibly chiphered, with new EPS context.
    switch (msg_type) {
      case  LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMPLETE:
      m_s1ap_log->info("Uplink NAS: Received Security Mode Complete\n");
      m_s1ap_log->console("Uplink NAS: Received Security Mode Complete\n");
      sec_ctx->ul_nas_count = 0;
      sec_ctx->dl_nas_count = 0;
      mac_valid = nas_ctx->integrity_check(nas_msg);
      if(mac_valid){
        nas_ctx->handle_nas_security_mode_complete(nas_msg, reply_buffer, reply_flag);
      } else {
        m_s1ap_log->warning("Invalid MAC in Security Mode Command Complete message.\n" );
      }
      break;
    default:
      m_s1ap_log->warning("Unhandled NAS message with new EPS security context 0x%x\n", msg_type );
      m_s1ap_log->warning("Unhandled NAS message with new EPS security context 0x%x\n", msg_type );
    }

  } else if (sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY ||
             sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED) {

    //Integrity protected NAS message, possibly ciphered.
    sec_ctx->ul_nas_count++;
    mac_valid = nas_ctx->integrity_check(nas_msg);
    if(!mac_valid){
      m_s1ap_log->warning("Invalid MAC in NAS message type 0x%x.\n", msg_type);
      m_pool->deallocate(nas_msg);
      return false;
    }
    switch (msg_type) {
    case  LIBLTE_MME_MSG_TYPE_ATTACH_COMPLETE:
      m_s1ap_log->info("Integrity Protected UL NAS: Received Attach Complete\n");
      m_s1ap_log->console("Integrity Protected UL NAS: Received Attach Complete\n");
      nas_ctx->handle_nas_attach_complete(nas_msg, reply_buffer, reply_flag);
      break;
    case LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_RESPONSE:
      m_s1ap_log->info("Integrity Protected UL NAS: Received ESM Information Response\n");
      m_s1ap_log->console("Integrity Protected UL NAS: Received ESM Information Response\n");
      nas_ctx->handle_esm_information_response(nas_msg, reply_buffer, reply_flag);
      break;
    case LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_REQUEST:
      m_s1ap_log->info("Integrity Protected UL NAS: Tracking Area Update Request\n");
      m_s1ap_log->console("Integrity Protected UL NAS: Tracking Area Update Request\n");
      nas_ctx->handle_tracking_area_update_request(nas_msg, reply_buffer, reply_flag);
      break;
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_FAILURE:
      m_s1ap_log->info("Integrity Protected UL NAS: Authentication Failure\n");
      m_s1ap_log->console("Integrity Protected UL NAS: Authentication Failure\n");
      nas_ctx->handle_authentication_failure(nas_msg, reply_buffer, reply_flag);
      break;
    case LIBLTE_MME_MSG_TYPE_DETACH_REQUEST:
      m_s1ap_log->info("Integrity Protected UL NAS: Detach Request\n");
      m_s1ap_log->console("Integrity Protected UL NAS: Detach Request\n");
      nas_ctx->handle_nas_detach_request(nas_msg, reply_buffer, reply_flag);
      break;
    default:
      m_s1ap_log->warning("Unhandled NAS integrity protected message %s\n", liblte_nas_msg_type_to_string(msg_type));
      m_s1ap_log->console("Unhandled NAS integrity protected message %s\n", liblte_nas_msg_type_to_string(msg_type));
      m_pool->deallocate(nas_msg);
      return false;
    }
  } else {
    m_s1ap_log->error("Unhandled security header type in Uplink NAS Transport: %d\n", sec_hdr_type);
    m_pool->deallocate(nas_msg);
    return false;
  }

  if (*reply_flag == true) {
    m_s1ap_log->console("DL NAS: Sent Downlink NAS Message. DL NAS Count=%d, UL NAS count=%d\n", sec_ctx->dl_nas_count, sec_ctx->ul_nas_count);
    m_s1ap_log->info("DL NAS: Sent Downlink NAS message. DL NAS Count=%d, UL NAS count=%d\n", sec_ctx->dl_nas_count, sec_ctx->ul_nas_count);
    m_s1ap_log->info("DL NAS: MME UE S1AP id %d\n",ecm_ctx->mme_ue_s1ap_id);
    m_s1ap_log->console("DL NAS: MME UE S1AP id %d\n",ecm_ctx->mme_ue_s1ap_id);
  }
  m_pool->deallocate(nas_msg);
  return true;
}

bool
s1ap_nas_transport::handle_nas_attach_request(uint32_t enb_ue_s1ap_id,
                                              srslte::byte_buffer_t *nas_msg,
                                              srslte::byte_buffer_t *reply_buffer,
                                              bool* reply_flag,
                                              struct sctp_sndrcvinfo *enb_sri)
{
  LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT attach_req;
  LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT pdn_con_req;

  //Get NAS Attach Request and PDN connectivity request messages
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_attach_request_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &attach_req);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS attach request. Error: %s\n", liblte_error_text[err]);
    m_pool->deallocate(nas_msg);
    return false;
  }
  /*Get PDN Connectivity Request*/
  err = liblte_mme_unpack_pdn_connectivity_request_msg(&attach_req.esm_msg, &pdn_con_req);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS PDN Connectivity Request. Error: %s\n", liblte_error_text[err]);
    m_pool->deallocate(nas_msg);
    return false;
  }

  //Get attach type from attach request
  if (attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI) {
    m_s1ap_log->console("Attach Request -- IMSI-style attach request\n");
    m_s1ap_log->info("Attach Request -- IMSI-style attach request\n");
    handle_nas_imsi_attach_request(enb_ue_s1ap_id, attach_req, pdn_con_req, reply_buffer, reply_flag, enb_sri);
  } else if (attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI) {
    m_s1ap_log->console("Attach Request -- GUTI-style attach request\n");
    m_s1ap_log->info("Attach Request -- GUTI-style attach request\n");
    handle_nas_guti_attach_request(enb_ue_s1ap_id, attach_req, pdn_con_req, nas_msg, reply_buffer, reply_flag, enb_sri);
  } else {
    m_s1ap_log->error("Unhandled Mobile Id type in attach request\n");
    return false;
  }
  return true;
}

bool
s1ap_nas_transport::handle_nas_imsi_attach_request(uint32_t enb_ue_s1ap_id,
                                                   const LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT &attach_req,
                                                   const LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT &pdn_con_req,
                                                   srslte::byte_buffer_t *reply_buffer,
                                                   bool* reply_flag,
                                                   struct sctp_sndrcvinfo *enb_sri)
{
  uint8_t     k_asme[32];
  uint8_t     autn[16];
  uint8_t     rand[16];
  uint8_t     xres[8];

  //Get UE's IMSI
  uint64_t imsi = 0;
  for(int i=0;i<=14;i++){
    imsi  += attach_req.eps_mobile_id.imsi[i]*std::pow(10,14-i);
  }

  //Check if UE is already attached
  nas *old_ctx = m_s1ap->find_nas_ctx_from_imsi(imsi);
  if (old_ctx!=NULL) {
    m_s1ap_log->console("Attach Request -- UE is already attached.");
    m_s1ap_log->info("Attach Request -- UE is already attached.");
    //Detaching previoulsy attached UE.
    m_mme_gtpc->send_delete_session_request(imsi);
    if (old_ctx->m_ecm_ctx.mme_ue_s1ap_id!=0) {
        m_s1ap->m_s1ap_ctx_mngmt_proc->send_ue_context_release_command(old_ctx);
    }
    m_s1ap->delete_ue_ctx(imsi);
  }

  //Set UE's EMM context
  nas *nas_ctx = new nas;
  nas_ctx->init(m_s1ap->m_s1ap_args.mcc,
                m_s1ap->m_s1ap_args.mnc,
                m_s1ap->m_s1ap_args.mme_code,
                m_s1ap->m_s1ap_args.mme_group,
                m_s1ap->m_s1ap_args.mme_apn,
                m_s1ap->m_s1ap_args.dns_addr,
                m_s1ap, m_mme_gtpc, m_hss, m_s1ap->m_nas_log);
  emm_ctx_t *emm_ctx = &nas_ctx->m_emm_ctx;
  ecm_ctx_t *ecm_ctx = &nas_ctx->m_ecm_ctx;
  sec_ctx_t *sec_ctx = &nas_ctx->m_sec_ctx;

  //Save IMSI, MME UE S1AP Id and make sure UE is EMM_DEREGISTERED
  emm_ctx->imsi = imsi;
  emm_ctx->state = EMM_STATE_DEREGISTERED;
  ecm_ctx->mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();

  //Save UE network capabilities
  memcpy(&sec_ctx->ue_network_cap, &attach_req.ue_network_cap, sizeof(LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT));
  sec_ctx->ms_network_cap_present = attach_req.ms_network_cap_present;
  if (attach_req.ms_network_cap_present) {
    memcpy(&sec_ctx->ms_network_cap, &attach_req.ms_network_cap, sizeof(LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT));
  }

  uint8_t eps_bearer_id = pdn_con_req.eps_bearer_id;             //TODO: Unused
  emm_ctx->procedure_transaction_id = pdn_con_req.proc_transaction_id;

  //Initialize NAS count
  sec_ctx->ul_nas_count = 0;
  sec_ctx->dl_nas_count = 0;

  //Set eNB information
  ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
  memcpy(&ecm_ctx->enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));

  //Save whether secure ESM information transfer is necessary
  ecm_ctx->eit = pdn_con_req.esm_info_transfer_flag_present;

  //Initialize E-RABs
  for (uint i = 0 ; i< MAX_ERABS_PER_UE; i++) {
    nas_ctx->m_esm_ctx[i].state = ERAB_DEACTIVATED;
    nas_ctx->m_esm_ctx[i].erab_id = i;
  }

  //Log Attach Request information
  m_s1ap_log->console("Attach request -- IMSI: %015lu\n", emm_ctx->imsi);
  m_s1ap_log->info("Attach request -- IMSI: %015lu\n", emm_ctx->imsi);
  m_s1ap_log->console("Attach request -- eNB-UE S1AP Id: %d, MME-UE S1AP Id: %d\n", ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id);
  m_s1ap_log->info("Attach request -- eNB-UE S1AP Id: %d, MME-UE S1AP Id: %d\n", ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id);
  m_s1ap_log->console("Attach request -- Attach type: %d\n", attach_req.eps_attach_type);
  m_s1ap_log->info("Attach request -- Attach type: %d\n", attach_req.eps_attach_type);

  m_s1ap_log->console("Attach Request -- UE Network Capabilities EEA: %d%d%d%d%d%d%d%d\n",
                      attach_req.ue_network_cap.eea[0], attach_req.ue_network_cap.eea[1], attach_req.ue_network_cap.eea[2], attach_req.ue_network_cap.eea[3],
                      attach_req.ue_network_cap.eea[4], attach_req.ue_network_cap.eea[5], attach_req.ue_network_cap.eea[6], attach_req.ue_network_cap.eea[7]);
  m_s1ap_log->console("Attach Request -- UE Network Capabilities EIA: %d%d%d%d%d%d%d%d\n",
                      attach_req.ue_network_cap.eia[0], attach_req.ue_network_cap.eia[1], attach_req.ue_network_cap.eia[2], attach_req.ue_network_cap.eia[3],
                      attach_req.ue_network_cap.eia[4], attach_req.ue_network_cap.eia[5], attach_req.ue_network_cap.eia[6], attach_req.ue_network_cap.eia[7]);

  m_s1ap_log->console("Attach Request -- MS Network Capabilities Present: %s\n", attach_req.ms_network_cap_present ? "true" : "false");
  m_s1ap_log->console("PDN Connectivity Request -- EPS Bearer Identity requested: %d\n", pdn_con_req.eps_bearer_id);
  m_s1ap_log->console("PDN Connectivity Request -- Procedure Transaction Id: %d\n", pdn_con_req.proc_transaction_id);
  m_s1ap_log->console("PDN Connectivity Request -- ESM Information Transfer requested: %s\n", pdn_con_req.esm_info_transfer_flag_present ? "true" : "false");

  //Save attach request type
  emm_ctx->attach_type = attach_req.eps_attach_type;

  //Get Authentication Vectors from HSS
  if (!m_hss->gen_auth_info_answer(emm_ctx->imsi, sec_ctx->k_asme, sec_ctx->autn, sec_ctx->rand, sec_ctx->xres)) {
    m_s1ap_log->console("User not found. IMSI %015lu\n",emm_ctx->imsi);
    m_s1ap_log->info("User not found. IMSI %015lu\n",emm_ctx->imsi);
    delete nas_ctx;
    return false;
  }
  //Allocate eKSI for this authentication vector
  //Here we assume a new security context thus a new eKSI
  sec_ctx->eksi=0;

  //Save the UE context
  m_s1ap->add_nas_ctx_to_imsi_map(nas_ctx);
  m_s1ap->add_nas_ctx_to_mme_ue_s1ap_id_map(nas_ctx);
  m_s1ap->add_ue_to_enb_set(enb_sri->sinfo_assoc_id,ecm_ctx->mme_ue_s1ap_id);

  //Pack NAS Authentication Request in Downlink NAS Transport msg
  nas_ctx->pack_authentication_request(reply_buffer);

  //Send reply to eNB
  *reply_flag = true;
  m_s1ap_log->info("Downlink NAS: Sending Authentication Request\n");
  m_s1ap_log->console("Downlink NAS: Sending Authentication Request\n");
  return true;
}

bool
s1ap_nas_transport::handle_nas_guti_attach_request(  uint32_t enb_ue_s1ap_id,
                                                     const LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT &attach_req,
                                                     const LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT &pdn_con_req,
                                                     srslte::byte_buffer_t *nas_msg,
                                                     srslte::byte_buffer_t *reply_buffer,
                                                     bool* reply_flag,
                                                     struct sctp_sndrcvinfo *enb_sri)
{
  //Parse the message security header
  uint8 pd = 0;
  uint8 sec_hdr_type = 0;
  liblte_mme_parse_msg_sec_header((LIBLTE_BYTE_MSG_STRUCT*)nas_msg, &pd, &sec_hdr_type);

  bool integrity_valid = false;
  if(sec_hdr_type != LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY) {
    m_s1ap_log->info("Attach request -- GUTI-stlye attach request is not integrity protected\n");
    m_s1ap_log->console("Attach request -- GUTI-stlye attach request is not integrity protected\n");
  } else {
    m_s1ap_log->info("Attach request -- GUTI-stlye attach request is integrity protected\n");
    m_s1ap_log->console("Attach request -- GUTI-stlye attach request is integrity protected\n");
  }

  //GUTI style attach
  uint32_t m_tmsi = attach_req.eps_mobile_id.guti.m_tmsi;
  std::map<uint32_t,uint64_t>::iterator it = m_s1ap->m_tmsi_to_imsi.find(m_tmsi);
  if (it == m_s1ap->m_tmsi_to_imsi.end()) {
    //Could not find IMSI from M-TMSI, send Id request
    m_s1ap_log->console("Attach Request -- Could not find M-TMSI 0x%x\n", m_tmsi);
    m_s1ap_log->info("Attach Request -- Could not find M-TMSI 0x%x\n", m_tmsi);

    nas *nas_ctx = new nas;
    nas_ctx->init(m_s1ap->m_s1ap_args.mcc,
                  m_s1ap->m_s1ap_args.mnc,
                  m_s1ap->m_s1ap_args.mme_code,
                  m_s1ap->m_s1ap_args.mme_group,
                  m_s1ap->m_s1ap_args.mme_apn,
                  m_s1ap->m_s1ap_args.dns_addr,
                  m_s1ap, m_mme_gtpc, m_hss, m_s1ap->m_nas_log);
    emm_ctx_t *emm_ctx = &nas_ctx->m_emm_ctx;
    ecm_ctx_t *ecm_ctx = &nas_ctx->m_ecm_ctx;
    sec_ctx_t *sec_ctx = &nas_ctx->m_sec_ctx;

    //We do not know the IMSI of the UE yet
    //The IMSI will be set when the identity response is received
    //Set EMM ctx
    emm_ctx->imsi = 0;
    emm_ctx->state = EMM_STATE_DEREGISTERED;

    //Save UE network capabilities
    memcpy(&sec_ctx->ue_network_cap, &attach_req.ue_network_cap, sizeof(LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT));
    sec_ctx->ms_network_cap_present =  attach_req.ms_network_cap_present;
    if (attach_req.ms_network_cap_present) {
        memcpy(&sec_ctx->ms_network_cap, &attach_req.ms_network_cap, sizeof(LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT));
    }
    //Initialize NAS count
    sec_ctx->ul_nas_count = 0;
    sec_ctx->dl_nas_count = 0;
    emm_ctx->procedure_transaction_id = pdn_con_req.proc_transaction_id;

    //Set ECM context
    ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
    ecm_ctx->mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();

    uint8_t eps_bearer_id = pdn_con_req.eps_bearer_id;             //TODO: Unused

    //Save attach request type
    emm_ctx->attach_type = attach_req.eps_attach_type;

    //Save whether ESM information transfer is necessary
    ecm_ctx->eit = pdn_con_req.esm_info_transfer_flag_present;

    //Add eNB info to UE ctxt
    memcpy(&ecm_ctx->enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));
    //Initialize E-RABs
    for (uint i = 0 ; i< MAX_ERABS_PER_UE; i++) {
      nas_ctx->m_esm_ctx[i].state = ERAB_DEACTIVATED;
      nas_ctx->m_esm_ctx[i].erab_id = i;
    }

    m_s1ap_log->console("Attach request -- IMSI: %015lu\n", emm_ctx->imsi);
    m_s1ap_log->info("Attach request -- IMSI: %015lu\n", emm_ctx->imsi);
    m_s1ap_log->console("Attach request -- eNB-UE S1AP Id: %d, MME-UE S1AP Id: %d\n", ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id);
    m_s1ap_log->console("Attach Request -- UE Network Capabilities EEA: %d%d%d%d%d%d%d%d\n",
                      attach_req.ue_network_cap.eea[0], attach_req.ue_network_cap.eea[1], attach_req.ue_network_cap.eea[2], attach_req.ue_network_cap.eea[3],
                      attach_req.ue_network_cap.eea[4], attach_req.ue_network_cap.eea[5], attach_req.ue_network_cap.eea[6], attach_req.ue_network_cap.eea[7]);
    m_s1ap_log->console("Attach Request -- UE Network Capabilities EIA: %d%d%d%d%d%d%d%d\n",
                      attach_req.ue_network_cap.eia[0], attach_req.ue_network_cap.eia[1], attach_req.ue_network_cap.eia[2], attach_req.ue_network_cap.eia[3],
                      attach_req.ue_network_cap.eia[4], attach_req.ue_network_cap.eia[5], attach_req.ue_network_cap.eia[6], attach_req.ue_network_cap.eia[7]);
    m_s1ap_log->console("Attach Request -- MS Network Capabilities Present: %s\n", attach_req.ms_network_cap_present ? "true" : "false");
    m_s1ap_log->console("PDN Connectivity Request -- EPS Bearer Identity requested: %d\n", pdn_con_req.eps_bearer_id);
    m_s1ap_log->console("PDN Connectivity Request -- Procedure Transaction Id: %d\n", pdn_con_req.proc_transaction_id);
    m_s1ap_log->console("PDN Connectivity Request -- ESM Information Transfer requested: %s\n", pdn_con_req.esm_info_transfer_flag_present ? "true" : "false");

    m_s1ap_log->console("Could not find M-TMSI=0x%x. Sending ID request\n",m_tmsi);
    m_s1ap_log->info("Could not find M-TMSI=0x%x. Sending Id Request\n", m_tmsi);

    //Store temporary ue context
    m_s1ap->add_nas_ctx_to_mme_ue_s1ap_id_map(nas_ctx);
    m_s1ap->add_ue_to_enb_set(enb_sri->sinfo_assoc_id,ecm_ctx->mme_ue_s1ap_id);

    nas_ctx->pack_identity_request(reply_buffer);
    *reply_flag = true;
    return true;
  } else {
    //Found UE context from M-TMSI
    m_s1ap_log->console("Attach Request -- Found M-TMSI: %d\n",m_tmsi);
    m_s1ap_log->console("Attach Request -- IMSI: %015lu\n",it->second);
    //Get UE EMM context
    nas *nas_ctx = m_s1ap->find_nas_ctx_from_imsi(it->second);
    if (nas_ctx!=NULL) {
      emm_ctx_t *emm_ctx = &nas_ctx->m_emm_ctx;
      ecm_ctx_t *ecm_ctx = &nas_ctx->m_ecm_ctx;
      sec_ctx_t *sec_ctx = &nas_ctx->m_sec_ctx;

      m_s1ap_log->console("Found UE context. IMSI: %015lu, old eNB UE S1ap Id %d, old MME UE S1AP Id %d\n",emm_ctx->imsi, ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id);
      //Check NAS integrity
      bool msg_valid = false;
      sec_ctx->ul_nas_count++;
      msg_valid = nas_ctx->integrity_check(nas_msg);
      if (msg_valid == true && emm_ctx->state == EMM_STATE_DEREGISTERED) {
        m_s1ap_log->console("GUTI Attach Integrity valid. UL count %d, DL count %d\n",sec_ctx->ul_nas_count, sec_ctx->dl_nas_count);

        //Create new MME UE S1AP Identity
        ecm_ctx->mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();
        ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;

        emm_ctx->procedure_transaction_id = pdn_con_req.proc_transaction_id;
        //Save Attach type
        emm_ctx->attach_type = attach_req.eps_attach_type;

        //Set eNB information
        ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
        memcpy(&ecm_ctx->enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));
        //Save whether secure ESM information transfer is necessary
        ecm_ctx->eit = pdn_con_req.esm_info_transfer_flag_present;

        //Initialize E-RABs
        for (uint i = 0 ; i< MAX_ERABS_PER_UE; i++) {
          nas_ctx->m_esm_ctx[i].state = ERAB_DEACTIVATED;
          nas_ctx->m_esm_ctx[i].erab_id = i;
        }

        //Store context based on MME UE S1AP id
        m_s1ap->add_nas_ctx_to_mme_ue_s1ap_id_map(nas_ctx);
        m_s1ap->add_ue_to_enb_set(enb_sri->sinfo_assoc_id,ecm_ctx->mme_ue_s1ap_id);

        //Re-generate K_eNB
        srslte::security_generate_k_enb(sec_ctx->k_asme, sec_ctx->ul_nas_count, sec_ctx->k_enb);
        m_s1ap_log->info("Generating KeNB with UL NAS COUNT: %d\n", sec_ctx->ul_nas_count);
        m_s1ap_log->console("Generating KeNB with UL NAS COUNT: %d\n", sec_ctx->ul_nas_count);
        m_s1ap_log->info_hex(sec_ctx->k_enb, 32, "Key eNodeB (k_enb)\n");

        m_s1ap_log->console("Attach request -- IMSI: %015lu\n", emm_ctx->imsi);
        m_s1ap_log->info("Attach request -- IMSI: %015lu\n", emm_ctx->imsi);
        m_s1ap_log->console("Attach request -- eNB-UE S1AP Id: %d, MME-UE S1AP Id: %d\n", ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id);
        m_s1ap_log->console("Attach Request -- UE Network Capabilities EEA: %d%d%d%d%d%d%d%d\n",
                      attach_req.ue_network_cap.eea[0], attach_req.ue_network_cap.eea[1], attach_req.ue_network_cap.eea[2], attach_req.ue_network_cap.eea[3],
                      attach_req.ue_network_cap.eea[4], attach_req.ue_network_cap.eea[5], attach_req.ue_network_cap.eea[6], attach_req.ue_network_cap.eea[7]);
        m_s1ap_log->console("Attach Request -- UE Network Capabilities EIA: %d%d%d%d%d%d%d%d\n",
                      attach_req.ue_network_cap.eia[0], attach_req.ue_network_cap.eia[1], attach_req.ue_network_cap.eia[2], attach_req.ue_network_cap.eia[3],
                      attach_req.ue_network_cap.eia[4], attach_req.ue_network_cap.eia[5], attach_req.ue_network_cap.eia[6], attach_req.ue_network_cap.eia[7]);
        m_s1ap_log->console("Attach Request -- MS Network Capabilities Present: %s\n", attach_req.ms_network_cap_present ? "true" : "false");
        m_s1ap_log->console("PDN Connectivity Request -- EPS Bearer Identity requested: %d\n", pdn_con_req.eps_bearer_id);
        m_s1ap_log->console("PDN Connectivity Request -- Procedure Transaction Id: %d\n", pdn_con_req.proc_transaction_id);
        m_s1ap_log->console("PDN Connectivity Request -- ESM Information Transfer requested: %s\n", pdn_con_req.esm_info_transfer_flag_present ? "true" : "false");

        //Create session request
        m_s1ap_log->console("GUTI Attach -- NAS Integrity OK.\n");
        if (ecm_ctx->eit) {
          m_s1ap_log->console("Secure ESM information transfer requested.\n");
          m_s1ap_log->info("Secure ESM information transfer requested.\n");
          nas_ctx->pack_esm_information_request(reply_buffer);
          *reply_flag = true;
        } else {
          //Get subscriber info from HSS
          uint8_t default_bearer=5;
          m_hss->gen_update_loc_answer(emm_ctx->imsi,&nas_ctx->m_esm_ctx[default_bearer].qci);
          m_s1ap_log->debug("Getting subscription information -- QCI %d\n", nas_ctx->m_esm_ctx[default_bearer].qci);
          m_s1ap_log->console("Getting subscription information -- QCI %d\n", nas_ctx->m_esm_ctx[default_bearer].qci);
          m_mme_gtpc->send_create_session_request(emm_ctx->imsi);
          *reply_flag = false; //No reply needed
        }
        return true;
      } else {
        if (emm_ctx->state != EMM_STATE_DEREGISTERED) {
          m_s1ap_log->error("Received GUTI-Attach Request from attached user.\n");
          m_s1ap_log->console("Received GUTI-Attach Request from attached user.\n");

          //Delete previous Ctx, restart authentication
          //Detaching previoulsy attached UE.
          m_mme_gtpc->send_delete_session_request(emm_ctx->imsi);
          if (ecm_ctx->mme_ue_s1ap_id!=0) {
            m_s1ap->m_s1ap_ctx_mngmt_proc->send_ue_context_release_command(nas_ctx);
          }
        }
        sec_ctx->ul_nas_count = 0;
        sec_ctx->dl_nas_count = 0;

        //Create new MME UE S1AP Identity
        uint32_t new_mme_ue_s1ap_id =  m_s1ap->get_next_mme_ue_s1ap_id();

        //Make sure context from previous NAS connections is not present
        if (ecm_ctx->mme_ue_s1ap_id!=0) {
          m_s1ap->release_ue_ecm_ctx(ecm_ctx->mme_ue_s1ap_id);
        }
        ecm_ctx->mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();

        //Set EMM as de-registered
        emm_ctx->state = EMM_STATE_DEREGISTERED;
        //Save Attach type
        emm_ctx->attach_type = attach_req.eps_attach_type;

        //Set UE ECM context
        ecm_ctx->mme_ue_s1ap_id = ecm_ctx->mme_ue_s1ap_id;

        //Set eNB information
        ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
        memcpy(&ecm_ctx->enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));
        //Save whether secure ESM information transfer is necessary
        ecm_ctx->eit = pdn_con_req.esm_info_transfer_flag_present;

        //Initialize E-RABs
        for (uint i = 0 ; i< MAX_ERABS_PER_UE; i++) {
            nas_ctx->m_esm_ctx[i].state = ERAB_DEACTIVATED;
            nas_ctx->m_esm_ctx[i].erab_id = i;
        }
        //Store context based on MME UE S1AP id
        m_s1ap->add_nas_ctx_to_mme_ue_s1ap_id_map(nas_ctx);
        m_s1ap->add_ue_to_enb_set(enb_sri->sinfo_assoc_id,ecm_ctx->mme_ue_s1ap_id);

        //NAS integrity failed. Re-start authentication process.
        m_s1ap_log->console("GUTI Attach request NAS integrity failed.\n");
        m_s1ap_log->console("RE-starting authentication procedure.\n");
        uint8_t     autn[16];
        uint8_t     rand[16];

        //Get Authentication Vectors from HSS
        if (!m_hss->gen_auth_info_answer(emm_ctx->imsi, sec_ctx->k_asme, autn, rand, sec_ctx->xres)) {
          m_s1ap_log->console("User not found. IMSI %015lu\n",emm_ctx->imsi);
          m_s1ap_log->info("User not found. IMSI %015lu\n",emm_ctx->imsi);
          return false;
        }

        //Restarting security context. Reseting eKSI to 0.
        sec_ctx->eksi=0;
        nas_ctx->pack_authentication_request(reply_buffer);

        //Send reply to eNB
        *reply_flag = true;
        m_s1ap_log->info("Downlink NAS: Sent Authentication Request\n");
        m_s1ap_log->console("Downlink NAS: Sent Authentication Request\n");
        return true;
      }
    } else {
      m_s1ap_log->error("Found M-TMSI but could not find UE context\n");
      m_s1ap_log->console("Error: Found M-TMSI but could not find UE context\n");
      return false;
    }
  }
  return true;
}

bool
s1ap_nas_transport::handle_nas_service_request( uint32_t m_tmsi,
                                                uint32_t enb_ue_s1ap_id,
                                                srslte::byte_buffer_t *nas_msg,
                                                srslte::byte_buffer_t *reply_buffer,
                                                bool* reply_flag,
                                                struct sctp_sndrcvinfo *enb_sri)
{
  m_s1ap_log->info("Service request -- S-TMSI 0x%x\n", m_tmsi);
  m_s1ap_log->console("Service request -- S-TMSI 0x%x\n", m_tmsi);
  m_s1ap_log->info("Service request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id);
  m_s1ap_log->console("Service request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id); 

  bool mac_valid = false;
  LIBLTE_MME_SERVICE_REQUEST_MSG_STRUCT service_req;

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_service_request_msg((LIBLTE_BYTE_MSG_STRUCT*) nas_msg, &service_req);
  if (err !=LIBLTE_SUCCESS) {
    m_s1ap_log->error("Could not unpack service request\n");
    return false;
  }

  std::map<uint32_t,uint64_t>::iterator it = m_s1ap->m_tmsi_to_imsi.find(m_tmsi);
  if (it == m_s1ap->m_tmsi_to_imsi.end()) {
    m_s1ap_log->console("Could not find IMSI from M-TMSI. M-TMSI 0x%x\n", m_tmsi);
    m_s1ap_log->error("Could not find IMSI from M-TMSI. M-TMSI 0x%x\n", m_tmsi);
    nas nas_tmp;
    nas_tmp.m_ecm_ctx.enb_ue_s1ap_id = enb_ue_s1ap_id;
    nas_tmp.pack_service_reject(reply_buffer);
    *reply_flag = true;
    return true;
  }

  nas *nas_ctx = m_s1ap->find_nas_ctx_from_imsi(it->second);
  if (nas_ctx == NULL || nas_ctx->m_emm_ctx.state != EMM_STATE_REGISTERED) {
    m_s1ap_log->console("UE is not EMM-Registered.\n");
    m_s1ap_log->error("UE is not EMM-Registered.\n");
    nas_ctx->pack_service_reject(reply_buffer);
    *reply_flag = true;
    return true;
  }
  emm_ctx_t *emm_ctx = &nas_ctx->m_emm_ctx;
  ecm_ctx_t *ecm_ctx = &nas_ctx->m_ecm_ctx;
  sec_ctx_t *sec_ctx = &nas_ctx->m_sec_ctx;

  sec_ctx->ul_nas_count++;
  mac_valid = nas_ctx->short_integrity_check(nas_msg);
  if (mac_valid) {
    m_s1ap_log->console("Service Request -- Short MAC valid\n");
    m_s1ap_log->info("Service Request -- Short MAC valid\n");
    if(ecm_ctx->state == ECM_STATE_CONNECTED) {
      m_s1ap_log->error("Service Request -- User is ECM CONNECTED\n");

      //Release previous context
      m_s1ap_log->info("Service Request -- Releasing previouse ECM context. eNB S1AP Id %d, MME UE S1AP Id %d\n", ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id);
      m_s1ap->m_s1ap_ctx_mngmt_proc->send_ue_context_release_command(nas_ctx);
      m_s1ap->release_ue_ecm_ctx(ecm_ctx->mme_ue_s1ap_id);
    }

    ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;

    //UE not connect. Connect normally.
    m_s1ap_log->console("Service Request -- User is ECM DISCONNECTED\n");
    m_s1ap_log->info("Service Request -- User is ECM DISCONNECTED\n");

    //Create ECM context
    ecm_ctx->mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();

    //Set eNB information
    ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
    memcpy(&ecm_ctx->enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));

    //Save whether secure ESM information transfer is necessary
    ecm_ctx->eit = false;

    //Get UE IP, and uplink F-TEID
    if(emm_ctx->ue_ip.s_addr == 0 ){
      m_s1ap_log->error("UE has no valid IP assigned upon reception of service request");
    }

    m_s1ap_log->console("UE previously assigned IP: %s",inet_ntoa(emm_ctx->ue_ip));

    //Re-generate K_eNB
    srslte::security_generate_k_enb(sec_ctx->k_asme, sec_ctx->ul_nas_count, sec_ctx->k_enb);
    m_s1ap_log->info("Generating KeNB with UL NAS COUNT: %d\n", sec_ctx->ul_nas_count);
    m_s1ap_log->console("Generating KeNB with UL NAS COUNT: %d\n", sec_ctx->ul_nas_count);
    m_s1ap_log->info_hex(sec_ctx->k_enb, 32, "Key eNodeB (k_enb)\n");
    m_s1ap_log->console("UE Ctr TEID %d\n", emm_ctx->sgw_ctrl_fteid.teid);

    //Save UE ctx to MME UE S1AP id
    m_s1ap->add_nas_ctx_to_mme_ue_s1ap_id_map(nas_ctx);
    m_s1ap->m_s1ap_ctx_mngmt_proc->send_initial_context_setup_request(nas_ctx, 5);
  } else {
    m_s1ap_log->console("Service Request -- Short MAC invalid. Ignoring service request\n");
    m_s1ap_log->console("Service Request -- Short MAC invalid. Ignoring service request\n");
  }
  return true;
}

bool
s1ap_nas_transport::handle_nas_detach_request(uint32_t m_tmsi,
                                              uint32_t enb_ue_s1ap_id,
                                              srslte::byte_buffer_t *nas_msg,
                                              srslte::byte_buffer_t *reply_buffer,
                                              bool* reply_flag,
                                              struct sctp_sndrcvinfo *enb_sri)
{
  m_s1ap_log->info("Detach Request -- S-TMSI 0x%x\n", m_tmsi);
  m_s1ap_log->console("Detach Request -- S-TMSI 0x%x\n", m_tmsi);
  m_s1ap_log->info("Detach Request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id);
  m_s1ap_log->console("Detach Request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id);

  bool mac_valid = false;
  LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT detach_req;

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_detach_request_msg((LIBLTE_BYTE_MSG_STRUCT*) nas_msg, &detach_req);
  if (err !=LIBLTE_SUCCESS) {
    m_s1ap_log->error("Could not unpack detach request\n");
    return false;
  }

  std::map<uint32_t,uint64_t>::iterator it = m_s1ap->m_tmsi_to_imsi.find(m_tmsi);
  if (it == m_s1ap->m_tmsi_to_imsi.end()) {
    m_s1ap_log->console("Could not find IMSI from M-TMSI. M-TMSI 0x%x\n", m_tmsi);
    m_s1ap_log->error("Could not find IMSI from M-TMSI. M-TMSI 0x%x\n", m_tmsi);
    return true;
  }

  nas *nas_ctx = m_s1ap->find_nas_ctx_from_imsi(it->second);
  emm_ctx_t *emm_ctx = &nas_ctx->m_emm_ctx;
  ecm_ctx_t *ecm_ctx = &nas_ctx->m_ecm_ctx;
  sec_ctx_t *sec_ctx = &nas_ctx->m_sec_ctx;

  m_mme_gtpc->send_delete_session_request(emm_ctx->imsi);
  emm_ctx->state = EMM_STATE_DEREGISTERED;
  sec_ctx->ul_nas_count++;

  m_s1ap_log->console("Received. M-TMSI 0x%x\n", m_tmsi);
  //Received detach request as an initial UE message
  //eNB created new ECM context to send the detach request; this needs to be cleared.
  ecm_ctx->mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();
  ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
  m_s1ap->m_s1ap_ctx_mngmt_proc->send_ue_context_release_command(nas_ctx);
  return true;
}

bool
s1ap_nas_transport::handle_nas_tracking_area_update_request(uint32_t m_tmsi,
                                              uint32_t enb_ue_s1ap_id,
                                              srslte::byte_buffer_t *nas_msg,
                                              srslte::byte_buffer_t *reply_buffer,
                                              bool* reply_flag,
                                              struct sctp_sndrcvinfo *enb_sri)
{
  m_s1ap_log->info("Tracking Area Update Request -- S-TMSI 0x%x\n", m_tmsi);
  m_s1ap_log->console("Tracking Area Update Request -- S-TMSI 0x%x\n", m_tmsi);
  m_s1ap_log->info("Tracking Area Update Request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id);
  m_s1ap_log->console("Tracking Area Update Request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id);

  m_s1ap_log->console("Warning: Tracking area update requests are not handled yet.\n");
  m_s1ap_log->warning("Tracking area update requests are not handled yet.\n");

  std::map<uint32_t,uint64_t>::iterator it = m_s1ap->m_tmsi_to_imsi.find(m_tmsi);
  if (it == m_s1ap->m_tmsi_to_imsi.end()) {
    m_s1ap_log->console("Could not find IMSI from M-TMSI. M-TMSI 0x%x\n", m_tmsi);
    m_s1ap_log->error("Could not find IMSI from M-TMSI. M-TMSI 0x%x\n", m_tmsi);
    return true;
  }

  nas *nas_ctx = m_s1ap->find_nas_ctx_from_imsi(it->second);
  emm_ctx_t *emm_ctx = &nas_ctx->m_emm_ctx;
  ecm_ctx_t *ecm_ctx = &nas_ctx->m_ecm_ctx;

  sec_ctx_t *sec_ctx = &nas_ctx->m_sec_ctx;

  sec_ctx->ul_nas_count++; //Increment the NAS count, not to break the security ctx
  return true;
}


} //namespace srsepc
