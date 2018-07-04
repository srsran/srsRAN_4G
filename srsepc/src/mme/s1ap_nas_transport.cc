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
s1ap_nas_transport::init(hss_interface_s1ap * hss_)
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

  //Get info from initial UE message
  uint32_t enb_ue_s1ap_id = init_ue->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID;

  //Log unhandled Initial UE message IEs
  log_unhandled_initial_ue_message_ies(init_ue);

  /*Check whether NAS Attach Request or Service Request*/
  bool mac_valid = false;
  uint8_t pd, msg_type, sec_hdr_type;
  srslte::byte_buffer_t *nas_msg = m_pool->allocate();
  memcpy(nas_msg->msg, &init_ue->NAS_PDU.buffer, init_ue->NAS_PDU.n_octets);
  nas_msg->N_bytes = init_ue->NAS_PDU.n_octets;

  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &pd, &msg_type);
  if(msg_type == LIBLTE_MME_MSG_TYPE_ATTACH_REQUEST)
  {
    m_s1ap_log->info("Received Attach Request \n");
    m_s1ap_log->console("Received Attach Request \n");
    handle_nas_attach_request(enb_ue_s1ap_id, nas_msg, reply_buffer,reply_flag, enb_sri);
  }
  else if(msg_type == LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST)
  {
    m_s1ap_log->info("Received Service Request \n");
    m_s1ap_log->console("Received Service Request \n");
    if(!init_ue->S_TMSI_present)
    {
      m_s1ap_log->error("Service request -- S-TMSI  not present\n");
      m_s1ap_log->console("Service request -- S-TMSI not present\n" );
      return false;
    }
    uint32_t *m_tmsi = (uint32_t*) &init_ue->S_TMSI.m_TMSI.buffer;
    uint32_t enb_ue_s1ap_id = init_ue->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID;
    m_s1ap_log->info("Service request -- S-TMSI 0x%x\n", ntohl(*m_tmsi));
    m_s1ap_log->console("Service request -- S-TMSI 0x%x\n", ntohl(*m_tmsi) );
    m_s1ap_log->info("Service request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id);
    m_s1ap_log->console("Service request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id); 
    handle_nas_service_request(ntohl(*m_tmsi), enb_ue_s1ap_id, nas_msg, reply_buffer,reply_flag, enb_sri);
    return true;
  }
  else if(msg_type == LIBLTE_MME_MSG_TYPE_DETACH_REQUEST)
  {
    m_s1ap_log->console("Received Initial UE message -- Detach Request\n");
    m_s1ap_log->info("Received Initial UE message -- Detach Request\n");
    if(!init_ue->S_TMSI_present)
    {
      m_s1ap_log->error("Detach request -- S-TMSI  not present\n");
      m_s1ap_log->console("Detach request -- S-TMSI not present\n" );
      return false;
    }
    uint32_t *m_tmsi = (uint32_t*) &init_ue->S_TMSI.m_TMSI.buffer;
    uint32_t enb_ue_s1ap_id = init_ue->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID;
    m_s1ap_log->info("Detach Request -- S-TMSI 0x%x\n", ntohl(*m_tmsi));
    m_s1ap_log->console("Detach Request -- S-TMSI 0x%x\n", ntohl(*m_tmsi) );
    m_s1ap_log->info("Detach Request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id);
    m_s1ap_log->console("Detach Request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id);

    handle_nas_detach_request(ntohl(*m_tmsi), enb_ue_s1ap_id, nas_msg, reply_buffer,reply_flag, enb_sri);
    return true;
  }
  else if(msg_type == LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_REQUEST)
  {
      m_s1ap_log->console("Received Initial UE message -- Tracking Area Update Request\n");
      m_s1ap_log->info("Received Initial UE message -- Tracking Area Update Request\n");
      if(!init_ue->S_TMSI_present)
      {
        m_s1ap_log->error("Tracking Area Update Request -- S-TMSI  not present\n");
        m_s1ap_log->console("Tracking Area Update Request -- S-TMSI not present\n" );
        return false;
      }
      uint32_t *m_tmsi = (uint32_t*) &init_ue->S_TMSI.m_TMSI.buffer;
      uint32_t enb_ue_s1ap_id = init_ue->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID;
      m_s1ap_log->info("Tracking Area Update Request -- S-TMSI 0x%x\n", ntohl(*m_tmsi));
      m_s1ap_log->console("Tracking Area Update Request -- S-TMSI 0x%x\n", ntohl(*m_tmsi) );
      m_s1ap_log->info("Tracking Area Update Request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id);
      m_s1ap_log->console("Tracking Area Update Request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id); 

      handle_nas_tracking_area_update_request(ntohl(*m_tmsi), enb_ue_s1ap_id, nas_msg, reply_buffer,reply_flag, enb_sri);
      return true;
  }
  else
  {
    m_s1ap_log->info("Unhandled Initial UE Message 0x%x\n",msg_type);
    m_s1ap_log->console("Unhandled Initial UE Message 0x%x \n", msg_type);
  }
  m_pool->deallocate(nas_msg);

  return true;
}

bool
s1ap_nas_transport::handle_uplink_nas_transport(LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT *ul_xport, struct sctp_sndrcvinfo *enb_sri, srslte::byte_buffer_t *reply_buffer, bool *reply_flag)
{
  uint8_t pd, msg_type, sec_hdr_type;
  uint32_t enb_ue_s1ap_id = ul_xport->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID;
  uint32_t mme_ue_s1ap_id = ul_xport->MME_UE_S1AP_ID.MME_UE_S1AP_ID;
  bool mac_valid = false;

  //Get UE ECM context
  ue_ctx_t *ue_ctx = m_s1ap->find_ue_ctx_from_mme_ue_s1ap_id(mme_ue_s1ap_id);
  if(ue_ctx == NULL)
  {
    m_s1ap_log->warning("Received uplink NAS, but could not find UE ECM context. MME-UE S1AP id: %d\n",mme_ue_s1ap_id);
    return false;
  }

  m_s1ap_log->debug("Received uplink NAS and found UE ECM context. MME-UE S1AP id: %d\n",mme_ue_s1ap_id);
  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;

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
      (msg_type == LIBLTE_MME_MSG_TYPE_AUTHENTICATION_FAILURE && sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY))
  {
    //Only identity response and authentication response are valid as plain NAS.
    //Sometimes authentication response/failure and identity response are sent as integrity protected,
    //but these messages are sent when the securty context is not setup yet, so we cannot integrity check it.
    switch(msg_type)
    {
    case LIBLTE_MME_MSG_TYPE_IDENTITY_RESPONSE:
      m_s1ap_log->info("Uplink NAS: Received Identity Response\n");
      m_s1ap_log->console("Uplink NAS: Received Identity Response\n");
      handle_identity_response(nas_msg, ue_ctx, reply_buffer, reply_flag);
      break;
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_RESPONSE:
      m_s1ap_log->info("Uplink NAS: Received Authentication Response\n");
      m_s1ap_log->console("Uplink NAS: Received Authentication Response\n");
      handle_nas_authentication_response(nas_msg, ue_ctx, reply_buffer, reply_flag);
      break;
    // Authentication failure with the option sync failure can be sent not integrity protected
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_FAILURE:
      m_s1ap_log->info("Plain UL NAS: Authentication Failure\n");
      m_s1ap_log->console("Plain UL NAS: Authentication Failure\n");
      handle_authentication_failure(nas_msg, ue_ctx, reply_buffer, reply_flag);
      break;
    // Detach request can be sent not integrity protected when "power off" option is used
    case LIBLTE_MME_MSG_TYPE_DETACH_REQUEST:
      m_s1ap_log->info("Plain Protected UL NAS: Detach Request\n");
      m_s1ap_log->console("Plain Protected UL NAS: Detach Request\n");
      handle_nas_detach_request(nas_msg, ue_ctx, reply_buffer, reply_flag);
      break;
    default:
      m_s1ap_log->warning("Unhandled Plain NAS message 0x%x\n", msg_type );
      m_s1ap_log->console("Unhandled Plain NAS message 0x%x\n", msg_type );
      m_pool->deallocate(nas_msg);
      return false;
    }
    //Increment UL NAS count.
    emm_ctx->security_ctxt.ul_nas_count++;
  }
  else if(sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_WITH_NEW_EPS_SECURITY_CONTEXT || sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED_WITH_NEW_EPS_SECURITY_CONTEXT)
  {
    switch (msg_type) {
      case  LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMPLETE:
      m_s1ap_log->info("Uplink NAS: Received Security Mode Complete\n");
      m_s1ap_log->console("Uplink NAS: Received Security Mode Complete\n");
      emm_ctx->security_ctxt.ul_nas_count = 0;
      emm_ctx->security_ctxt.dl_nas_count = 0;
      mac_valid = integrity_check(emm_ctx,nas_msg);
      if(mac_valid){
        handle_nas_security_mode_complete(nas_msg, ue_ctx, reply_buffer, reply_flag);
      } else {
        m_s1ap_log->warning("Invalid MAC in Security Mode Command Complete message.\n" );
      }
      break;
    default:
      m_s1ap_log->warning("Unhandled NAS message with new EPS security context 0x%x\n", msg_type );
      m_s1ap_log->warning("Unhandled NAS message with new EPS security context 0x%x\n", msg_type );
    }
  }
  else if(sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY || sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED)
  {
    //Integrity protected NAS message, possibly ciphered.
    emm_ctx->security_ctxt.ul_nas_count++;
    mac_valid = integrity_check(emm_ctx,nas_msg);
    if(!mac_valid){
      m_s1ap_log->warning("Invalid MAC in NAS message type 0x%x.\n", msg_type);
      m_pool->deallocate(nas_msg);
      return false;
    }
    switch (msg_type) {
    case  LIBLTE_MME_MSG_TYPE_ATTACH_COMPLETE:
      m_s1ap_log->info("Integrity Protected UL NAS: Received Attach Complete\n");
      m_s1ap_log->console("Integrity Protected UL NAS: Received Attach Complete\n");
      handle_nas_attach_complete(nas_msg, ue_ctx, reply_buffer, reply_flag);
      break;
    case LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_RESPONSE:
      m_s1ap_log->info("Integrity Protected UL NAS: Received ESM Information Response\n");
      m_s1ap_log->console("Integrity Protected UL NAS: Received ESM Information Response\n");
      handle_esm_information_response(nas_msg, ue_ctx, reply_buffer, reply_flag);
      break;
    case LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_REQUEST:
      m_s1ap_log->info("Integrity Protected UL NAS: Tracking Area Update Request\n");
      m_s1ap_log->console("Integrity Protected UL NAS: Tracking Area Update Request\n");
      handle_tracking_area_update_request(nas_msg, ue_ctx, reply_buffer, reply_flag);
      break;
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_FAILURE:
      m_s1ap_log->info("Integrity Protected UL NAS: Authentication Failure\n");
      m_s1ap_log->console("Integrity Protected UL NAS: Authentication Failure\n");
      handle_authentication_failure(nas_msg, ue_ctx, reply_buffer, reply_flag);
      break;
    case LIBLTE_MME_MSG_TYPE_DETACH_REQUEST:
      m_s1ap_log->info("Integrity Protected UL NAS: Detach Request\n");
      m_s1ap_log->console("Integrity Protected UL NAS: Detach Request\n");
      handle_nas_detach_request(nas_msg, ue_ctx, reply_buffer, reply_flag);
      break;
    default:
      m_s1ap_log->warning("Unhandled NAS integrity protected message 0x%x\n", msg_type );
      m_s1ap_log->console("Unhandled NAS integrity protected message 0x%x\n", msg_type );
      m_pool->deallocate(nas_msg);
      return false;
    }
  }
  else
  {
    m_s1ap_log->error("Unhandled security header type in Uplink NAS Transport: %d\n", sec_hdr_type);
    m_pool->deallocate(nas_msg);
    return false;
  }


  if(*reply_flag == true)
  {
    m_s1ap_log->console("DL NAS: Sent Downlink NAS Message. DL NAS Count=%d, UL NAS count=%d\n",emm_ctx->security_ctxt.dl_nas_count,emm_ctx->security_ctxt.ul_nas_count );
    m_s1ap_log->info("DL NAS: Sent Downlink NAS message. DL NAS Count=%d, UL NAS count=%d\n",emm_ctx->security_ctxt.dl_nas_count, emm_ctx->security_ctxt.ul_nas_count);
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
  if(attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI)
  {
    m_s1ap_log->console("Attach Request -- IMSI-style attach request\n");
    m_s1ap_log->info("Attach Request -- IMSI-style attach request\n");
    handle_nas_imsi_attach_request(enb_ue_s1ap_id, attach_req, pdn_con_req, reply_buffer, reply_flag, enb_sri);
  }
  else if(attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI)
  {
    m_s1ap_log->console("Attach Request -- GUTI-style attach request\n");
    m_s1ap_log->info("Attach Request -- GUTI-style attach request\n");
    handle_nas_guti_attach_request(enb_ue_s1ap_id, attach_req, pdn_con_req, nas_msg, reply_buffer, reply_flag, enb_sri);
  }
  else
  {
    m_s1ap_log->error("Unhandle Mobile Id type in attach request\n");
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

  ue_ctx_t ue_ctx;
  ue_emm_ctx_t *emm_ctx = &ue_ctx.emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx.ecm_ctx;

  //Set UE's EMM context
  uint64_t imsi = 0;
  for(int i=0;i<=14;i++){
    imsi  += attach_req.eps_mobile_id.imsi[i]*std::pow(10,14-i);
  }

  //Check if UE is 
  ue_ctx_t *old_ctx = m_s1ap->find_ue_ctx_from_imsi(imsi);
  if(old_ctx!=NULL)
  {
    m_s1ap_log->console("Attach Request -- UE is already attached.");
    m_s1ap_log->info("Attach Request -- UE is already attached.");
    //Detaching previoulsy attached UE.
    m_mme_gtpc->send_delete_session_request(imsi);
    if(old_ctx->ecm_ctx.mme_ue_s1ap_id!=0)
    {
        m_s1ap->m_s1ap_ctx_mngmt_proc->send_ue_context_release_command(&old_ctx->ecm_ctx, reply_buffer);
    }
    m_s1ap->delete_ue_ctx(imsi);
  }

  emm_ctx->imsi = imsi;
  emm_ctx->mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();
  emm_ctx->state = EMM_STATE_DEREGISTERED;
  //Save UE network capabilities
  memcpy(&emm_ctx->security_ctxt.ue_network_cap, &attach_req.ue_network_cap, sizeof(LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT));
  emm_ctx->security_ctxt.ms_network_cap_present =  attach_req.ms_network_cap_present;
  if(attach_req.ms_network_cap_present)
  {
    memcpy(&emm_ctx->security_ctxt.ms_network_cap, &attach_req.ms_network_cap, sizeof(LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT));
  }
  uint8_t eps_bearer_id = pdn_con_req.eps_bearer_id;             //TODO: Unused
  emm_ctx->procedure_transaction_id = pdn_con_req.proc_transaction_id; 

  //Initialize NAS count
  emm_ctx->security_ctxt.ul_nas_count = 0;
  emm_ctx->security_ctxt.dl_nas_count = 0;

  //Set UE ECM context
  ecm_ctx->imsi = emm_ctx->imsi;
  ecm_ctx->mme_ue_s1ap_id = emm_ctx->mme_ue_s1ap_id;

  //Set eNB information
  ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
  memcpy(&ecm_ctx->enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));

  //Save whether secure ESM information transfer is necessary
  ecm_ctx->eit = pdn_con_req.esm_info_transfer_flag_present;

  //Initialize E-RABs
  for(uint i = 0 ; i< MAX_ERABS_PER_UE; i++)
  {
    ecm_ctx->erabs_ctx[i].state = ERAB_DEACTIVATED;
    ecm_ctx->erabs_ctx[i].erab_id = i;
  }

  //Log Attach Request information
  m_s1ap_log->console("Attach request -- IMSI: %015lu\n", emm_ctx->imsi);
  m_s1ap_log->info("Attach request -- IMSI: %015lu\n", emm_ctx->imsi);
  m_s1ap_log->console("Attach request -- eNB-UE S1AP Id: %d, MME-UE S1AP Id: %d\n", ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id);
  m_s1ap_log->info("Attach request -- eNB-UE S1AP Id: %d, MME-UE S1AP Id: %d\n", ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id);
  m_s1ap_log->console("Attach request -- Attach type: %d\n", attach_req.eps_attach_type);
  m_s1ap_log->info("Attach request -- Attach type: %d\n", attach_req.eps_attach_type);

  m_s1ap_log->console("Attach Request -- UE Network Capabilities EEA: %d%d%d%d%d%d%d%d\n",
                      attach_req.ue_network_cap.eea[0],
                      attach_req.ue_network_cap.eea[1],
                      attach_req.ue_network_cap.eea[2],
                      attach_req.ue_network_cap.eea[3],
                      attach_req.ue_network_cap.eea[4],
                      attach_req.ue_network_cap.eea[5],
                      attach_req.ue_network_cap.eea[6],
                      attach_req.ue_network_cap.eea[7]);
  m_s1ap_log->console("Attach Request -- UE Network Capabilities EIA: %d%d%d%d%d%d%d%d\n",
                      attach_req.ue_network_cap.eia[0],
                      attach_req.ue_network_cap.eia[1],
                      attach_req.ue_network_cap.eia[2],
                      attach_req.ue_network_cap.eia[3],
                      attach_req.ue_network_cap.eia[4],
                      attach_req.ue_network_cap.eia[5],
                      attach_req.ue_network_cap.eia[6],
                      attach_req.ue_network_cap.eia[7]);
  m_s1ap_log->console("Attach Request -- MS Network Capabilities Present: %s\n", attach_req.ms_network_cap_present ? "true" : "false");
  m_s1ap_log->console("PDN Connectivity Request -- EPS Bearer Identity requested: %d\n", pdn_con_req.eps_bearer_id);
  m_s1ap_log->console("PDN Connectivity Request -- Procedure Transaction Id: %d\n", pdn_con_req.proc_transaction_id);
  m_s1ap_log->console("PDN Connectivity Request -- ESM Information Transfer requested: %s\n", pdn_con_req.esm_info_transfer_flag_present ? "true" : "false");

  //Save attach request type
  emm_ctx->attach_type = attach_req.eps_attach_type;

  //Get Authentication Vectors from HSS
  if(!m_hss->gen_auth_info_answer(emm_ctx->imsi, emm_ctx->security_ctxt.k_asme, autn, rand, emm_ctx->security_ctxt.xres))
  {
    m_s1ap_log->console("User not found. IMSI %015lu\n",emm_ctx->imsi);
    m_s1ap_log->info("User not found. IMSI %015lu\n",emm_ctx->imsi);
    return false;
  }
  //Allocate eKSI for this authentication vector
  //Here we assume a new security context thus a new eKSI
  emm_ctx->security_ctxt.eksi=0;

  //Save the UE context
  ue_ctx_t *new_ctx = new ue_ctx_t;
  memcpy(new_ctx,&ue_ctx,sizeof(ue_ctx_t));
  m_s1ap->add_ue_ctx_to_imsi_map(new_ctx);
  m_s1ap->add_ue_ctx_to_mme_ue_s1ap_id_map(new_ctx);
  m_s1ap->add_ue_to_enb_set(enb_sri->sinfo_assoc_id,ecm_ctx->mme_ue_s1ap_id);

  //Pack NAS Authentication Request in Downlink NAS Transport msg
  pack_authentication_request(reply_buffer, ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id, emm_ctx->security_ctxt.eksi, autn, rand);

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
  if(sec_hdr_type != LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY)
  {
    m_s1ap_log->info("Attach request -- GUTI-stlye attach request is not integrity protected\n");
    m_s1ap_log->console("Attach request -- GUTI-stlye attach request is not integrity protected\n");
  }
  else{
    m_s1ap_log->info("Attach request -- GUTI-stlye attach request is integrity protected\n");
    m_s1ap_log->console("Attach request -- GUTI-stlye attach request is integrity protected\n");
  }


  //GUTI style attach
  uint32_t m_tmsi = attach_req.eps_mobile_id.guti.m_tmsi;
  std::map<uint32_t,uint64_t>::iterator it = m_s1ap->m_tmsi_to_imsi.find(m_tmsi);
  if(it == m_s1ap->m_tmsi_to_imsi.end())
  {

    m_s1ap_log->console("Attach Request -- Could not find M-TMSI 0x%x\n", m_tmsi);
    m_s1ap_log->info("Attach Request -- Could not find M-TMSI 0x%x\n", m_tmsi);

    //Could not find IMSI from M-TMSI, send Id request
    ue_ctx_t ue_ctx;
    ue_emm_ctx_t *emm_ctx = &ue_ctx.emm_ctx;
    ue_ecm_ctx_t *ecm_ctx = &ue_ctx.ecm_ctx;

    //We do not know the IMSI of the UE yet
    //The IMSI will be set when the identity response is received
    //Set EMM ctx
    emm_ctx->imsi = 0;
    emm_ctx->state = EMM_STATE_DEREGISTERED;
    emm_ctx->mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();

    //Save UE network capabilities
    memcpy(&emm_ctx->security_ctxt.ue_network_cap, &attach_req.ue_network_cap, sizeof(LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT));
    emm_ctx->security_ctxt.ms_network_cap_present =  attach_req.ms_network_cap_present;
    if(attach_req.ms_network_cap_present)
    {
        memcpy(&emm_ctx->security_ctxt.ms_network_cap, &attach_req.ms_network_cap, sizeof(LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT));
    }
    //Initialize NAS count
    emm_ctx->security_ctxt.ul_nas_count = 0;
    emm_ctx->security_ctxt.dl_nas_count = 0;
    emm_ctx->procedure_transaction_id = pdn_con_req.proc_transaction_id;

    //Set ECM context
    ecm_ctx->imsi = 0;
    ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
    ecm_ctx->mme_ue_s1ap_id = emm_ctx->mme_ue_s1ap_id;

    uint8_t eps_bearer_id = pdn_con_req.eps_bearer_id;             //TODO: Unused

    //Save attach request type
    emm_ctx->attach_type = attach_req.eps_attach_type;

    //Save whether ESM information transfer is necessary
    ecm_ctx->eit = pdn_con_req.esm_info_transfer_flag_present;

    //Add eNB info to UE ctxt
    memcpy(&ecm_ctx->enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));
    //Initialize E-RABs
    for(uint i = 0 ; i< MAX_ERABS_PER_UE; i++)
    {
      ecm_ctx->erabs_ctx[i].state = ERAB_DEACTIVATED;
      ecm_ctx->erabs_ctx[i].erab_id = i;
    }
    m_s1ap_log->console("Attach request -- IMSI: %015lu\n", ecm_ctx->imsi);
    m_s1ap_log->info("Attach request -- IMSI: %015lu\n", ecm_ctx->imsi);
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
    ue_ctx_t *new_ctx = new ue_ctx_t;
    memcpy(new_ctx,&ue_ctx,sizeof(ue_ctx_t));
    m_s1ap->add_ue_ctx_to_mme_ue_s1ap_id_map(new_ctx);
    m_s1ap->add_ue_to_enb_set(enb_sri->sinfo_assoc_id,ecm_ctx->mme_ue_s1ap_id);

    pack_identity_request(reply_buffer, ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id);
    *reply_flag = true;
    return true;
  }
  else{

    m_s1ap_log->console("Attach Request -- Found M-TMSI: %d\n",m_tmsi);
    m_s1ap_log->console("Attach Request -- IMSI: %015lu\n",it->second);
    //Get UE EMM context
    ue_ctx_t *ue_ctx = m_s1ap->find_ue_ctx_from_imsi(it->second);
    if(ue_ctx!=NULL)
    {
      ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
      ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;
      m_s1ap_log->console("Found UE context. IMSI: %015lu, old eNB UE S1ap Id %d, old MME UE S1AP Id %d\n",emm_ctx->imsi, ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id);
      //Check NAS integrity
      bool msg_valid = false;
      emm_ctx->security_ctxt.ul_nas_count++;
      msg_valid = integrity_check(emm_ctx,nas_msg);
      if(msg_valid == true && emm_ctx->state == EMM_STATE_DEREGISTERED)
      {
        m_s1ap_log->console("GUTI Attach Integrity valid. UL count %d, DL count %d\n",emm_ctx->security_ctxt.ul_nas_count, emm_ctx->security_ctxt.dl_nas_count);

        //Create new MME UE S1AP Identity
        emm_ctx->mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();
        ecm_ctx->mme_ue_s1ap_id = emm_ctx->mme_ue_s1ap_id;
        ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
        ecm_ctx->imsi = ecm_ctx->imsi;

        emm_ctx->procedure_transaction_id = pdn_con_req.proc_transaction_id;
        //Save Attach type
        emm_ctx->attach_type = attach_req.eps_attach_type;

        //Set eNB information
        ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
        memcpy(&ecm_ctx->enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));
        //Save whether secure ESM information transfer is necessary
        ecm_ctx->eit = pdn_con_req.esm_info_transfer_flag_present;

        //Initialize E-RABs
        for(uint i = 0 ; i< MAX_ERABS_PER_UE; i++)
        {
            ecm_ctx->erabs_ctx[i].state = ERAB_DEACTIVATED;
            ecm_ctx->erabs_ctx[i].erab_id = i;
        }

        //Store context based on MME UE S1AP id
        m_s1ap->add_ue_ctx_to_mme_ue_s1ap_id_map(ue_ctx);
        m_s1ap->add_ue_to_enb_set(enb_sri->sinfo_assoc_id,ecm_ctx->mme_ue_s1ap_id);

        //Re-generate K_eNB
        srslte::security_generate_k_enb(emm_ctx->security_ctxt.k_asme, emm_ctx->security_ctxt.ul_nas_count, emm_ctx->security_ctxt.k_enb);
        m_s1ap_log->info("Generating KeNB with UL NAS COUNT: %d\n",emm_ctx->security_ctxt.ul_nas_count);
        m_s1ap_log->console("Generating KeNB with UL NAS COUNT: %d\n",emm_ctx->security_ctxt.ul_nas_count);
        m_s1ap_log->info_hex(emm_ctx->security_ctxt.k_enb, 32, "Key eNodeB (k_enb)\n");

        m_s1ap_log->console("Attach request -- IMSI: %015lu\n", ecm_ctx->imsi);
        m_s1ap_log->info("Attach request -- IMSI: %015lu\n", ecm_ctx->imsi);
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
        if(ecm_ctx->eit)
        {
          m_s1ap_log->console("Secure ESM information transfer requested.\n");
          m_s1ap_log->info("Secure ESM information transfer requested.\n");
          pack_esm_information_request(reply_buffer, emm_ctx, ecm_ctx);
          *reply_flag = true;
        }
        else
        {
          //Get subscriber info from HSS
          uint8_t default_bearer=5;
          m_hss->gen_update_loc_answer(emm_ctx->imsi,&ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);
          m_s1ap_log->debug("Getting subscription information -- QCI %d\n", ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);
          m_s1ap_log->console("Getting subscription information -- QCI %d\n", ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);
          m_mme_gtpc->send_create_session_request(emm_ctx->imsi);
          *reply_flag = false; //No reply needed
        }
        return true;
      }
      else
      {
        if(emm_ctx->state != EMM_STATE_DEREGISTERED)
        {
          m_s1ap_log->error("Received GUTI-Attach Request from attached user.\n");
          m_s1ap_log->console("Received GUTI-Attach Request from attached user.\n");

          //Delete previous Ctx, restart authentication
          //Detaching previoulsy attached UE.
          m_mme_gtpc->send_delete_session_request(emm_ctx->imsi);
          if(ecm_ctx->mme_ue_s1ap_id!=0)
          {
            m_s1ap->m_s1ap_ctx_mngmt_proc->send_ue_context_release_command(ecm_ctx, reply_buffer);
          }
        }
        emm_ctx->security_ctxt.ul_nas_count = 0;
        emm_ctx->security_ctxt.dl_nas_count = 0;

        //Create new MME UE S1AP Identity
        uint32_t new_mme_ue_s1ap_id =  m_s1ap->get_next_mme_ue_s1ap_id();

        //Make sure context from previous NAS connections is not present
        if(ecm_ctx->mme_ue_s1ap_id!=0)
        {
          m_s1ap->release_ue_ecm_ctx(ecm_ctx->mme_ue_s1ap_id);
        }
        emm_ctx->mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();
        ecm_ctx->mme_ue_s1ap_id = emm_ctx->mme_ue_s1ap_id;
        //Set EMM as de-registered
        emm_ctx->state = EMM_STATE_DEREGISTERED;
        //Save Attach type
        emm_ctx->attach_type = attach_req.eps_attach_type;

        //Set UE ECM context
        ecm_ctx->imsi = ecm_ctx->imsi;
        ecm_ctx->mme_ue_s1ap_id = ecm_ctx->mme_ue_s1ap_id;

        //Set eNB information
        ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
        memcpy(&ecm_ctx->enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));
        //Save whether secure ESM information transfer is necessary
        ecm_ctx->eit = pdn_con_req.esm_info_transfer_flag_present;

        //Initialize E-RABs
        for(uint i = 0 ; i< MAX_ERABS_PER_UE; i++)
        {
            ecm_ctx->erabs_ctx[i].state = ERAB_DEACTIVATED;
            ecm_ctx->erabs_ctx[i].erab_id = i;
        }
        //Store context based on MME UE S1AP id
        m_s1ap->add_ue_ctx_to_mme_ue_s1ap_id_map(ue_ctx);
        m_s1ap->add_ue_to_enb_set(enb_sri->sinfo_assoc_id,ecm_ctx->mme_ue_s1ap_id);

        //NAS integrity failed. Re-start authentication process.
        m_s1ap_log->console("GUTI Attach request NAS integrity failed.\n");
        m_s1ap_log->console("RE-starting authentication procedure.\n");
        uint8_t     autn[16];
        uint8_t     rand[16];
        //Get Authentication Vectors from HSS
        if(!m_hss->gen_auth_info_answer(emm_ctx->imsi, emm_ctx->security_ctxt.k_asme, autn, rand, emm_ctx->security_ctxt.xres))
        {
            m_s1ap_log->console("User not found. IMSI %015lu\n",emm_ctx->imsi);
            m_s1ap_log->info("User not found. IMSI %015lu\n",emm_ctx->imsi);
            return false;
        }
        //Restarting security context. Reseting eKSI to 0.
        emm_ctx->security_ctxt.eksi=0;
        pack_authentication_request(reply_buffer, ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id, emm_ctx->security_ctxt.eksi, autn, rand);

        //Send reply to eNB
        *reply_flag = true;
        m_s1ap_log->info("Downlink NAS: Sent Authentication Request\n");
        m_s1ap_log->console("Downlink NAS: Sent Authentication Request\n");
        return true;
      }
    }
    else
    {
      m_s1ap_log->error("Found M-TMSI but could not find UE context\n");
      m_s1ap_log->console("Error: Found M-TMSI but could not find UE context\n");
      return false;
    }
  }
  return true;
}

bool
s1ap_nas_transport::handle_nas_service_request(uint32_t m_tmsi,
                                               uint32_t enb_ue_s1ap_id,
                                                srslte::byte_buffer_t *nas_msg,
                                                srslte::byte_buffer_t *reply_buffer,
                                                bool* reply_flag,
                                                struct sctp_sndrcvinfo *enb_sri)
{

  bool mac_valid = false;
  LIBLTE_MME_SERVICE_REQUEST_MSG_STRUCT service_req;

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_service_request_msg((LIBLTE_BYTE_MSG_STRUCT*) nas_msg, &service_req);
  if(err !=LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Could not unpack service request\n");
    return false;
  }

  std::map<uint32_t,uint64_t>::iterator it = m_s1ap->m_tmsi_to_imsi.find(m_tmsi);
  if(it == m_s1ap->m_tmsi_to_imsi.end())
  {
    m_s1ap_log->console("Could not find IMSI from M-TMSI. M-TMSI 0x%x\n", m_tmsi);
    m_s1ap_log->error("Could not find IMSI from M-TMSI. M-TMSI 0x%x\n", m_tmsi);
    pack_service_reject(reply_buffer, LIBLTE_MME_EMM_CAUSE_IMPLICITLY_DETACHED, enb_ue_s1ap_id);
    *reply_flag = true;
    return true;
  }

  ue_ctx_t *ue_ctx = m_s1ap->find_ue_ctx_from_imsi(it->second);
  if(ue_ctx == NULL || ue_ctx->emm_ctx.state != EMM_STATE_REGISTERED)
  {
    m_s1ap_log->console("UE is not EMM-Registered.\n");
    m_s1ap_log->error("UE is not EMM-Registered.\n");
    pack_service_reject(reply_buffer, LIBLTE_MME_EMM_CAUSE_IMPLICITLY_DETACHED, enb_ue_s1ap_id);
    *reply_flag = true;
    return true;
  }
  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;

  emm_ctx->security_ctxt.ul_nas_count++;
  mac_valid = short_integrity_check(emm_ctx,nas_msg);
  if(mac_valid)
  {
    m_s1ap_log->console("Service Request -- Short MAC valid\n");
    m_s1ap_log->info("Service Request -- Short MAC valid\n");
    if(ecm_ctx->state == ECM_STATE_CONNECTED)
    {
      m_s1ap_log->error("Service Request -- User is ECM CONNECTED\n");

      //Release previous context
      m_s1ap_log->info("Service Request -- Releasing previouse ECM context. eNB S1AP Id %d, MME UE S1AP Id %d\n", ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id);
      m_s1ap->m_s1ap_ctx_mngmt_proc->send_ue_context_release_command(ecm_ctx,reply_buffer);
      m_s1ap->release_ue_ecm_ctx(ecm_ctx->mme_ue_s1ap_id);
    }

    ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;

    //UE not connect. Connect normally.
    m_s1ap_log->console("Service Request -- User is ECM DISCONNECTED\n");
    m_s1ap_log->info("Service Request -- User is ECM DISCONNECTED\n");
    //Create ECM context
    ecm_ctx->imsi = emm_ctx->imsi;
    ecm_ctx->mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();
    emm_ctx->mme_ue_s1ap_id = ecm_ctx->mme_ue_s1ap_id;
    //Set eNB information
    ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
    memcpy(&ecm_ctx->enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));

    //Save whether secure ESM information transfer is necessary
    ecm_ctx->eit = false;

    //Get UE IP, and uplink F-TEID
    if(emm_ctx->ue_ip.s_addr == 0 )
    {
      m_s1ap_log->error("UE has no valid IP assigned upon reception of service request");
    }

    m_s1ap_log->console("UE previously assigned IP: %s",inet_ntoa(emm_ctx->ue_ip));

    //Re-generate K_eNB
    srslte::security_generate_k_enb(emm_ctx->security_ctxt.k_asme, emm_ctx->security_ctxt.ul_nas_count, emm_ctx->security_ctxt.k_enb);
    m_s1ap_log->info("Generating KeNB with UL NAS COUNT: %d\n",emm_ctx->security_ctxt.ul_nas_count);
    m_s1ap_log->console("Generating KeNB with UL NAS COUNT: %d\n",emm_ctx->security_ctxt.ul_nas_count);
    m_s1ap_log->info_hex(emm_ctx->security_ctxt.k_enb, 32, "Key eNodeB (k_enb)\n");
    m_s1ap_log->console("UE Ctr TEID %d\n", emm_ctx->sgw_ctrl_fteid.teid);

    //Save UE ctx to MME UE S1AP id
    m_s1ap->add_ue_ctx_to_mme_ue_s1ap_id_map(ue_ctx);
    m_s1ap->m_s1ap_ctx_mngmt_proc->send_initial_context_setup_request(emm_ctx, ecm_ctx,&ecm_ctx->erabs_ctx[5]);
  }
  else
  {
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
  bool mac_valid = false;
  LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT detach_req;

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_detach_request_msg((LIBLTE_BYTE_MSG_STRUCT*) nas_msg, &detach_req);
  if(err !=LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Could not unpack detach request\n");
    return false;
  }

  std::map<uint32_t,uint64_t>::iterator it = m_s1ap->m_tmsi_to_imsi.find(m_tmsi);
  if(it == m_s1ap->m_tmsi_to_imsi.end())
  {
    m_s1ap_log->console("Could not find IMSI from M-TMSI. M-TMSI 0x%x\n", m_tmsi);
    m_s1ap_log->error("Could not find IMSI from M-TMSI. M-TMSI 0x%x\n", m_tmsi);
    return true;
  }
  ue_ctx_t *ue_ctx = m_s1ap->find_ue_ctx_from_imsi(it->second);
  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;

  m_mme_gtpc->send_delete_session_request(emm_ctx->imsi);
  emm_ctx->state = EMM_STATE_DEREGISTERED;
  emm_ctx->security_ctxt.ul_nas_count++;

  m_s1ap_log->console("Received. M-TMSI 0x%x\n", m_tmsi);
  //Received detach request as an initial UE message
  //eNB created new ECM context to send the detach request; this needs to be cleared.
  ecm_ctx->mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();
  ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
  m_s1ap->m_s1ap_ctx_mngmt_proc->send_ue_context_release_command(ecm_ctx, reply_buffer); 
  return true;
}

//FIXME re-factor to reduce code duplication
bool
s1ap_nas_transport::handle_nas_detach_request(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{

  m_s1ap_log->console("Detach request -- IMSI %015lu\n", ue_ctx->emm_ctx.imsi);
  m_s1ap_log->info("Detach request -- IMSI %015lu\n", ue_ctx->emm_ctx.imsi);
  LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT detach_req;

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_detach_request_msg((LIBLTE_BYTE_MSG_STRUCT*) nas_msg, &detach_req);
  if(err !=LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Could not unpack detach request\n");
    return false;
  }

  m_mme_gtpc->send_delete_session_request(ue_ctx->emm_ctx.imsi);
  ue_ctx->emm_ctx.state = EMM_STATE_DEREGISTERED;
  if(ue_ctx->ecm_ctx.mme_ue_s1ap_id!=0)
  {
    m_s1ap->m_s1ap_ctx_mngmt_proc->send_ue_context_release_command(&ue_ctx->ecm_ctx, reply_msg);
  }
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
  m_s1ap_log->console("Warning: Tracking area update requests are not handled yet.\n");
  m_s1ap_log->warning("Tracking area update requests are not handled yet.\n");

  std::map<uint32_t,uint64_t>::iterator it = m_s1ap->m_tmsi_to_imsi.find(m_tmsi);
  if(it == m_s1ap->m_tmsi_to_imsi.end())
  {
    m_s1ap_log->console("Could not find IMSI from M-TMSI. M-TMSI 0x%x\n", m_tmsi);
    m_s1ap_log->error("Could not find IMSI from M-TMSI. M-TMSI 0x%x\n", m_tmsi);
    return true;
  }
  ue_ctx_t *ue_ctx = m_s1ap->find_ue_ctx_from_imsi(it->second);
  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;

  emm_ctx->security_ctxt.ul_nas_count++;//Increment the NAS count, not to break the security ctx
  return true;
}
bool
s1ap_nas_transport::handle_nas_authentication_response(srslte::byte_buffer_t *nas_msg, ue_ctx_t *ue_ctx, srslte::byte_buffer_t *reply_buffer, bool* reply_flag)
{

  LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT auth_resp;
  bool ue_valid=true;

  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;
  m_s1ap_log->console("Authentication Response -- IMSI %015lu\n", emm_ctx->imsi);

  //Get NAS authentication response
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_authentication_response_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &auth_resp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }
  m_s1ap_log->console("Authentication Response -- RES 0x%x%x%x%x%x%x%x%x\n",
                      auth_resp.res[0], auth_resp.res[1], auth_resp.res[2], auth_resp.res[3],
                      auth_resp.res[4], auth_resp.res[5], auth_resp.res[6], auth_resp.res[7]);
  m_s1ap_log->info("Authentication Response -- RES 0x%x%x%x%x%x%x%x%x\n",
                      auth_resp.res[0], auth_resp.res[1], auth_resp.res[2], auth_resp.res[3],
                      auth_resp.res[4], auth_resp.res[5], auth_resp.res[6], auth_resp.res[7]);

  for(int i=0; i<8;i++)
  {
    if(auth_resp.res[i] != emm_ctx->security_ctxt.xres[i])
    {
      ue_valid = false;
    }
  }
  if(!ue_valid)
  {
    m_s1ap_log->info_hex(emm_ctx->security_ctxt.xres,8, "XRES");
    m_s1ap_log->console("UE Authentication Rejected.\n");
    m_s1ap_log->warning("UE Authentication Rejected.\n");

    //Send back Athentication Reject
    pack_authentication_reject(reply_buffer, ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id);
    *reply_flag = true;
    m_s1ap_log->console("Downlink NAS: Sending Authentication Reject.\n");
    return false;
  }
  else
  {
    m_s1ap_log->console("UE Authentication Accepted.\n");
    m_s1ap_log->info("UE Authentication Accepted.\n");

    //Send Security Mode Command
    emm_ctx->security_ctxt.ul_nas_count = 0; // Reset the NAS uplink counter for the right key k_enb derivation
    pack_security_mode_command(reply_buffer, emm_ctx, ecm_ctx);
    *reply_flag = true;
    m_s1ap_log->console("Downlink NAS: Sending NAS Security Mode Command.\n");
  }
  return true;
}

bool
s1ap_nas_transport::handle_nas_security_mode_complete(srslte::byte_buffer_t *nas_msg, ue_ctx_t *ue_ctx, srslte::byte_buffer_t *reply_buffer, bool *reply_flag)
{

  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;

  LIBLTE_MME_SECURITY_MODE_COMPLETE_MSG_STRUCT sm_comp;

  //Get NAS authentication response
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_security_mode_complete_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &sm_comp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  //TODO Handle imeisv
  if(sm_comp.imeisv_present)
  {
    m_s1ap_log->warning("IMEI-SV present but not handled");
  }

  m_s1ap_log->info("Security Mode Command Complete -- IMSI: %lu\n", emm_ctx->imsi);
  m_s1ap_log->console("Security Mode Command Complete -- IMSI: %lu\n", emm_ctx->imsi);
  if(ecm_ctx->eit == true)
  {
    pack_esm_information_request(reply_buffer, emm_ctx, ecm_ctx);
    m_s1ap_log->console("Sending ESM information request\n");
    m_s1ap_log->info_hex(reply_buffer->msg, reply_buffer->N_bytes, "Sending ESM information request\n");
    *reply_flag = true;
  }
  else
  {
    //Get subscriber info from HSS
    uint8_t default_bearer=5;
    m_hss->gen_update_loc_answer(emm_ctx->imsi,&ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);
    m_s1ap_log->debug("Getting subscription information -- QCI %d\n", ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);
    m_s1ap_log->console("Getting subscription information -- QCI %d\n", ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);
    //FIXME The packging of GTP-C messages is not ready.
    //This means that GTP-U tunnels are created with function calls, as opposed to GTP-C.
    m_mme_gtpc->send_create_session_request(emm_ctx->imsi);
    *reply_flag = false; //No reply needed
  }
  return true;
}


bool
s1ap_nas_transport::handle_nas_attach_complete(srslte::byte_buffer_t *nas_msg, ue_ctx_t *ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{

  LIBLTE_MME_ATTACH_COMPLETE_MSG_STRUCT attach_comp;
  uint8_t pd, msg_type;
  srslte::byte_buffer_t *esm_msg = m_pool->allocate();
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT act_bearer;

  m_s1ap_log->info_hex(nas_msg->msg, nas_msg->N_bytes, "NAS Attach complete");

  //Get NAS authentication response
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_attach_complete_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &attach_comp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  err = liblte_mme_unpack_activate_default_eps_bearer_context_accept_msg( (LIBLTE_BYTE_MSG_STRUCT *) &attach_comp.esm_msg, &act_bearer);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking Activate EPS Bearer Context Accept Msg. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;

  m_s1ap_log->console("Unpacked Attached Complete Message. IMSI %" PRIu64 "\n", emm_ctx->imsi);
  m_s1ap_log->console("Unpacked Activate Default EPS Bearer message. EPS Bearer id %d\n",act_bearer.eps_bearer_id);
  //ue_ctx->erabs_ctx[act_bearer->eps_bearer_id].enb_fteid;
  if(act_bearer.eps_bearer_id < 5 || act_bearer.eps_bearer_id > 15)
  {
    m_s1ap_log->error("EPS Bearer ID out of range\n");
    return false;
  }
  if(emm_ctx->state == EMM_STATE_DEREGISTERED)
  {
    //Attach requested from attach request
    m_mme_gtpc->send_modify_bearer_request(emm_ctx->imsi, &ecm_ctx->erabs_ctx[act_bearer.eps_bearer_id]);
    //Send reply to eNB
    m_s1ap_log->console("Packing EMM Information\n");
    *reply_flag = pack_emm_information(ue_ctx, reply_msg);
    m_s1ap_log->console("Sending EMM Information, bytes %d\n",reply_msg->N_bytes);
    m_s1ap_log->info("Sending EMM Information\n");
  }
  emm_ctx->state = EMM_STATE_REGISTERED;
  return true;
}

bool
s1ap_nas_transport::handle_esm_information_response(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{
  LIBLTE_MME_ESM_INFORMATION_RESPONSE_MSG_STRUCT esm_info_resp;

  //Get NAS authentication response
  LIBLTE_ERROR_ENUM err = srslte_mme_unpack_esm_information_response_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &esm_info_resp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }
  m_s1ap_log->info("ESM Info: EPS bearer id %d\n",esm_info_resp.eps_bearer_id);
  if(esm_info_resp.apn_present)
  {
    m_s1ap_log->info("ESM Info: APN %s\n",esm_info_resp.apn.apn);
    m_s1ap_log->console("ESM Info: APN %s\n",esm_info_resp.apn.apn);
  }
  if(esm_info_resp.protocol_cnfg_opts_present)
  {
    m_s1ap_log->info("ESM Info: %d Protocol Configuration Options\n",esm_info_resp.protocol_cnfg_opts.N_opts);
    m_s1ap_log->console("ESM Info: %d Protocol Configuration Options\n",esm_info_resp.protocol_cnfg_opts.N_opts);
  }

  //Get subscriber info from HSS
  uint8_t default_bearer=5;
  m_hss->gen_update_loc_answer(ue_ctx->emm_ctx.imsi,&ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);
  m_s1ap_log->debug("Getting subscription information -- QCI %d\n", ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);
  m_s1ap_log->console("Getting subscription information -- QCI %d\n", ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);

  //FIXME The packging of GTP-C messages is not ready.
  //This means that GTP-U tunnels are created with function calls, as opposed to GTP-C.
  m_mme_gtpc->send_create_session_request(ue_ctx->emm_ctx.imsi);
  return true;
}

bool
s1ap_nas_transport::handle_identity_response(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{
  uint8_t     autn[16]; 
  uint8_t     rand[16];
  uint8_t     xres[8];

  LIBLTE_MME_ID_RESPONSE_MSG_STRUCT id_resp;
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_identity_response_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &id_resp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS identity response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  uint64_t imsi = 0;
  for(int i=0;i<=14;i++){
    imsi  += id_resp.mobile_id.imsi[i]*std::pow(10,14-i);
  }

  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;

  m_s1ap_log->info("ID response -- IMSI: %015lu\n", imsi);
  m_s1ap_log->console("ID Response -- IMSI: %015lu\n", imsi);

  //Set UE's context IMSI
  emm_ctx->imsi=imsi;
  ecm_ctx->imsi = imsi;

  //Get Authentication Vectors from HSS
  if(!m_hss->gen_auth_info_answer(imsi, emm_ctx->security_ctxt.k_asme, autn, rand, emm_ctx->security_ctxt.xres))
  {
    m_s1ap_log->console("User not found. IMSI %015lu\n",imsi);
    m_s1ap_log->info("User not found. IMSI %015lu\n",imsi);
    return false;
  }
  //Identity reponse from unknown GUTI atach. Assigning new eKSI.
  emm_ctx->security_ctxt.eksi=0;

  //Store UE context im IMSI map
  m_s1ap->add_ue_ctx_to_imsi_map(ue_ctx);

  //Pack NAS Authentication Request in Downlink NAS Transport msg
  pack_authentication_request(reply_msg, ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id, emm_ctx->security_ctxt.eksi, autn, rand);

  //Send reply to eNB
  *reply_flag = true;
   m_s1ap_log->info("Downlink NAS: Sent Authentication Request\n");
   m_s1ap_log->console("Downlink NAS: Sent Authentication Request\n");
  //TODO Start T3460 Timer! 

  return true;
}


bool
s1ap_nas_transport::handle_tracking_area_update_request(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{

  m_s1ap_log->console("Warning: Tracking Area Update Request messages not handled yet.\n");
  m_s1ap_log->warning("Warning: Tracking Area Update Request messages not handled yet.\n");
  //Setup initiating message
  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  bzero(&tx_pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));

  tx_pdu.ext          = false;
  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &tx_pdu.choice.initiatingMessage;
  init->procedureCode = LIBLTE_S1AP_PROC_ID_DOWNLINKNASTRANSPORT;
  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_DOWNLINKNASTRANSPORT;

  //Setup Dw NAS structure
  LIBLTE_S1AP_MESSAGE_DOWNLINKNASTRANSPORT_STRUCT *dw_nas = &init->choice.DownlinkNASTransport;
  dw_nas->ext=false;
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = ue_ctx->ecm_ctx.mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ue_ctx->ecm_ctx.enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;
  //m_s1ap_log->console("Tracking area accept to MME-UE S1AP Id %d\n", ue_ctx->mme_ue_s1ap_id);
  LIBLTE_MME_TRACKING_AREA_UPDATE_ACCEPT_MSG_STRUCT tau_acc;

  //Get decimal MCC and MNC
  uint32_t mcc = 0;
  mcc += 0x000F & m_s1ap->m_s1ap_args.mcc;
  mcc += 10*( (0x00F0 & m_s1ap->m_s1ap_args.mcc) >> 4);
  mcc += 100*( (0x0F00 & m_s1ap->m_s1ap_args.mcc) >> 8);

  uint32_t mnc = 0;
  if( 0xFF00 == (m_s1ap->m_s1ap_args.mnc & 0xFF00 ))
  {
    //Two digit MNC
    mnc += 0x000F & m_s1ap->m_s1ap_args.mnc;
    mnc += 10*((0x00F0 & m_s1ap->m_s1ap_args.mnc) >> 4);
  }
  else
  {
    //Three digit MNC
    mnc += 0x000F & m_s1ap->m_s1ap_args.mnc;
    mnc += 10*((0x00F0 & m_s1ap->m_s1ap_args.mnc) >> 4);
    mnc += 100*((0x0F00 & m_s1ap->m_s1ap_args.mnc) >> 8);
  }

  //T3412 Timer
  tau_acc.t3412_present = true;
  tau_acc.t3412.unit = LIBLTE_MME_GPRS_TIMER_UNIT_1_MINUTE;   // GPRS 1 minute unit
  tau_acc.t3412.value = 30;                                   // 30 minute periodic timer

  //GUTI
  tau_acc.guti_present=true;
  tau_acc.guti.type_of_id = 6; //110 -> GUTI
  tau_acc.guti.guti.mcc = mcc;
  tau_acc.guti.guti.mnc = mnc;
  tau_acc.guti.guti.mme_group_id = m_s1ap->m_s1ap_args.mme_group;
  tau_acc.guti.guti.mme_code = m_s1ap->m_s1ap_args.mme_code;
  tau_acc.guti.guti.m_tmsi = 0xF000;
  m_s1ap_log->debug("Allocated GUTI: MCC %d, MNC %d, MME Group Id %d, MME Code 0x%x, M-TMSI 0x%x\n",
                    tau_acc.guti.guti.mcc,
                    tau_acc.guti.guti.mnc,
                    tau_acc.guti.guti.mme_group_id,
                    tau_acc.guti.guti.mme_code,
                    tau_acc.guti.guti.m_tmsi);

  //Unused Options
  tau_acc.t3402_present = false;
  tau_acc.t3423_present = false;
  tau_acc.equivalent_plmns_present = false;
  tau_acc.emerg_num_list_present = false;
  tau_acc.eps_network_feature_support_present = false;
  tau_acc.additional_update_result_present = false;
  tau_acc.t3412_ext_present = false;

  return true;
}

bool
s1ap_nas_transport::short_integrity_check(ue_emm_ctx_t *emm_ctx, srslte::byte_buffer_t *pdu)
{
  uint8_t exp_mac[4];
  uint8_t *mac = &pdu->msg[2];
  int i;

  srslte::security_128_eia1(&emm_ctx->security_ctxt.k_nas_int[16],
                     emm_ctx->security_ctxt.ul_nas_count,
                     0,
                     SECURITY_DIRECTION_UPLINK,
                     &pdu->msg[0],
                     2,
                     &exp_mac[0]);

  // Check if expected mac equals the sent mac
  for(i=0; i<2; i++){
    if(exp_mac[i+2] != mac[i]){
      m_s1ap_log->warning("Short integrity check failure. Local: count=%d, [%02x %02x %02x %02x], "
                       "Received: count=%d, [%02x %02x]\n",
                          emm_ctx->security_ctxt.ul_nas_count, exp_mac[0], exp_mac[1], exp_mac[2], exp_mac[3],
                          pdu->msg[1] & 0x1F, mac[0], mac[1]);
      return false;
    }
  }
  m_s1ap_log->info("Integrity check ok. Local: count=%d, Received: count=%d\n",
                emm_ctx->security_ctxt.ul_nas_count, pdu->msg[1] & 0x1F);
    return true;
}


bool
s1ap_nas_transport::integrity_check(ue_emm_ctx_t *emm_ctx, srslte::byte_buffer_t *pdu)
{
  uint8_t exp_mac[4];
  uint8_t *mac = &pdu->msg[1];
  int i;

  srslte::security_128_eia1(&emm_ctx->security_ctxt.k_nas_int[16],
                     emm_ctx->security_ctxt.ul_nas_count,
                     0,
                     SECURITY_DIRECTION_UPLINK,
                     &pdu->msg[5],
                     pdu->N_bytes-5,
                     &exp_mac[0]);

  // Check if expected mac equals the sent mac
  for(i=0; i<4; i++){
    if(exp_mac[i] != mac[i]){
      m_s1ap_log->warning("Integrity check failure. UL Local: count=%d, [%02x %02x %02x %02x], "
                       "Received: UL count=%d, [%02x %02x %02x %02x]\n",
                       emm_ctx->security_ctxt.ul_nas_count, exp_mac[0], exp_mac[1], exp_mac[2], exp_mac[3],
                       pdu->msg[5], mac[0], mac[1], mac[2], mac[3]);
      return false;
    }
  }
  m_s1ap_log->info("Integrity check ok. Local: count=%d, Received: count=%d\n",
                emm_ctx->security_ctxt.ul_nas_count, pdu->msg[5]);
    return true;
}


bool 
s1ap_nas_transport::handle_authentication_failure(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{
  uint8_t     autn[16]; 
  uint8_t     rand[16];
  uint8_t     xres[8];

  LIBLTE_MME_AUTHENTICATION_FAILURE_MSG_STRUCT auth_fail;
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_authentication_failure_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &auth_fail);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication failure. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;

  switch(auth_fail.emm_cause){
    case 20:
    m_s1ap_log->console("MAC code failure\n");
    m_s1ap_log->info("MAC code failure\n");
    break; 
    case 26:
    m_s1ap_log->console("Non-EPS authentication unacceptable\n");
    m_s1ap_log->info("Non-EPS authentication unacceptable\n");
    break;
    case 21:
    m_s1ap_log->console("Authentication Failure -- Synchronization Failure\n");
    m_s1ap_log->info("Authentication Failure -- Synchronization Failure\n");
    if(auth_fail.auth_fail_param_present == false){
      m_s1ap_log->error("Missing fail parameter\n");
      return false;
    }
    if(!m_hss->resync_sqn(emm_ctx->imsi, auth_fail.auth_fail_param))
    {
      m_s1ap_log->console("Resynchronization failed. IMSI %015lu\n", emm_ctx->imsi);
      m_s1ap_log->info("Resynchronization failed. IMSI %015lu\n", emm_ctx->imsi);
      return false;
    }
    //Get Authentication Vectors from HSS
    if(!m_hss->gen_auth_info_answer(emm_ctx->imsi, emm_ctx->security_ctxt.k_asme, autn, rand, emm_ctx->security_ctxt.xres))
    {
      m_s1ap_log->console("User not found. IMSI %015lu\n", emm_ctx->imsi);
      m_s1ap_log->info("User not found. IMSI %015lu\n", emm_ctx->imsi);
      return false;
    }
    //Making sure eKSI is different from previous eKSI.
    emm_ctx->security_ctxt.eksi = (emm_ctx->security_ctxt.eksi+1)%6;

    //Pack NAS Authentication Request in Downlink NAS Transport msg
    pack_authentication_request(reply_msg, ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id, emm_ctx->security_ctxt.eksi, autn, rand);

    //Send reply to eNB
    *reply_flag = true;
    m_s1ap_log->info("Downlink NAS: Sent Authentication Request\n");
    m_s1ap_log->console("Downlink NAS: Sent Authentication Request\n");
    //TODO Start T3460 Timer! 

    break;
  }
  return true;
}

/*Packing/Unpacking helper functions*/
bool
s1ap_nas_transport::pack_authentication_request(srslte::byte_buffer_t *reply_msg, uint32_t enb_ue_s1ap_id, uint32_t next_mme_ue_s1ap_id, uint8_t eksi, uint8_t *autn, uint8_t *rand)
{
  srslte::byte_buffer_t *nas_buffer = m_pool->allocate();

  //Setup initiating message
  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  bzero(&tx_pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));

  tx_pdu.ext          = false;
  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &tx_pdu.choice.initiatingMessage;
  init->procedureCode = LIBLTE_S1AP_PROC_ID_DOWNLINKNASTRANSPORT;
  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_DOWNLINKNASTRANSPORT;

  //Setup Dw NAS structure
  LIBLTE_S1AP_MESSAGE_DOWNLINKNASTRANSPORT_STRUCT *dw_nas = &init->choice.DownlinkNASTransport;
  dw_nas->ext=false;
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = next_mme_ue_s1ap_id;//FIXME Change name
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  //Pack NAS PDU 
  LIBLTE_MME_AUTHENTICATION_REQUEST_MSG_STRUCT auth_req;
  memcpy(auth_req.autn , autn, 16);
  memcpy(auth_req.rand, rand, 16);
  auth_req.nas_ksi.tsc_flag=LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
  auth_req.nas_ksi.nas_ksi = eksi;

  LIBLTE_ERROR_ENUM err = liblte_mme_pack_authentication_request_msg(&auth_req, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Authentication Request\n");
    m_s1ap_log->console("Error packing Authentication Request\n");
    return false;
  }

  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Authentication Request\n");
    m_s1ap_log->console("Error packing Authentication Request\n");
    return false;
  }
   
  m_pool->deallocate(nas_buffer);

  return true;
}

bool
s1ap_nas_transport::pack_authentication_reject(srslte::byte_buffer_t *reply_msg, uint32_t enb_ue_s1ap_id, uint32_t mme_ue_s1ap_id)
{
  srslte::byte_buffer_t *nas_buffer = m_pool->allocate();

  //Setup initiating message
  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  bzero(&tx_pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));

  tx_pdu.ext          = false;
  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &tx_pdu.choice.initiatingMessage;
  init->procedureCode = LIBLTE_S1AP_PROC_ID_DOWNLINKNASTRANSPORT;
  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_DOWNLINKNASTRANSPORT;

  //Setup Dw NAS structure
  LIBLTE_S1AP_MESSAGE_DOWNLINKNASTRANSPORT_STRUCT *dw_nas = &init->choice.DownlinkNASTransport;
  dw_nas->ext=false;
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = mme_ue_s1ap_id;//FIXME Change name
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  LIBLTE_MME_AUTHENTICATION_REJECT_MSG_STRUCT auth_rej;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_authentication_reject_msg(&auth_rej, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Authentication Reject\n");
    m_s1ap_log->console("Error packing Authentication Reject\n");
    return false;
  }

  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Dw NAS Transport: Authentication Reject\n");
    m_s1ap_log->console("Error packing Downlink NAS Transport: Authentication Reject\n");
    return false;
  } 

  m_pool->deallocate(nas_buffer);
  return true;
}

bool
s1ap_nas_transport::unpack_authentication_response(LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT *ul_xport,
                                              LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT *auth_resp )
{

  /*Get NAS Authentiation Response Message*/
  uint8_t pd, msg_type;
  srslte::byte_buffer_t *nas_msg = m_pool->allocate();

  memcpy(nas_msg->msg, &ul_xport->NAS_PDU.buffer, ul_xport->NAS_PDU.n_octets);
  nas_msg->N_bytes = ul_xport->NAS_PDU.n_octets;
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &pd, &msg_type);

  if(msg_type!=LIBLTE_MME_MSG_TYPE_AUTHENTICATION_RESPONSE){
    m_s1ap_log->error("Error unpacking NAS authentication response\n");
    return false;
  }

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_authentication_response_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, auth_resp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  m_pool->deallocate(nas_msg);
  return true;
}

bool
s1ap_nas_transport::pack_security_mode_command(srslte::byte_buffer_t *reply_msg, ue_emm_ctx_t *ue_emm_ctx, ue_ecm_ctx_t *ue_ecm_ctx)
{
  srslte::byte_buffer_t *nas_buffer = m_pool->allocate();

  //Setup initiating message
  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  bzero(&tx_pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));

  tx_pdu.ext          = false;
  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &tx_pdu.choice.initiatingMessage;
  init->procedureCode = LIBLTE_S1AP_PROC_ID_DOWNLINKNASTRANSPORT;
  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_DOWNLINKNASTRANSPORT;

  //Setup Dw NAS structure
  LIBLTE_S1AP_MESSAGE_DOWNLINKNASTRANSPORT_STRUCT *dw_nas = &init->choice.DownlinkNASTransport;
  dw_nas->ext=false;
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = ue_ecm_ctx->mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ue_ecm_ctx->enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  //Pack NAS PDU 
  LIBLTE_MME_SECURITY_MODE_COMMAND_MSG_STRUCT sm_cmd;
 
  sm_cmd.selected_nas_sec_algs.type_of_eea = LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_EEA0;
  sm_cmd.selected_nas_sec_algs.type_of_eia = LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_128_EIA1;

  sm_cmd.nas_ksi.tsc_flag=LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
  sm_cmd.nas_ksi.nas_ksi=ue_emm_ctx->security_ctxt.eksi; 

  //Replay UE security cap
  memcpy(sm_cmd.ue_security_cap.eea,ue_emm_ctx->security_ctxt.ue_network_cap.eea,8*sizeof(bool));
  memcpy(sm_cmd.ue_security_cap.eia,ue_emm_ctx->security_ctxt.ue_network_cap.eia,8*sizeof(bool));
  sm_cmd.ue_security_cap.uea_present = ue_emm_ctx->security_ctxt.ue_network_cap.uea_present;
  memcpy(sm_cmd.ue_security_cap.uea,ue_emm_ctx->security_ctxt.ue_network_cap.uea,8*sizeof(bool));
  sm_cmd.ue_security_cap.uia_present = ue_emm_ctx->security_ctxt.ue_network_cap.uia_present;
  memcpy(sm_cmd.ue_security_cap.uia,ue_emm_ctx->security_ctxt.ue_network_cap.uia,8*sizeof(bool));
  sm_cmd.ue_security_cap.gea_present = ue_emm_ctx->security_ctxt.ms_network_cap_present;
  memcpy(sm_cmd.ue_security_cap.gea,ue_emm_ctx->security_ctxt.ms_network_cap.gea,8*sizeof(bool));

  sm_cmd.imeisv_req_present=false;
  sm_cmd.nonce_ue_present=false;
  sm_cmd.nonce_mme_present=false;

  uint8_t  sec_hdr_type=3;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_security_mode_command_msg(&sm_cmd,sec_hdr_type, ue_emm_ctx->security_ctxt.dl_nas_count,(LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->console("Error packing Authentication Request\n");
    return false;
  }

  //Generate EPS security context
  uint8_t mac[4];
  srslte::security_generate_k_nas( ue_emm_ctx->security_ctxt.k_asme,
                           srslte::CIPHERING_ALGORITHM_ID_EEA0,
                           srslte::INTEGRITY_ALGORITHM_ID_128_EIA1,
                           ue_emm_ctx->security_ctxt.k_nas_enc,
                           ue_emm_ctx->security_ctxt.k_nas_int
                         );

  m_s1ap_log->info_hex(ue_emm_ctx->security_ctxt.k_nas_enc, 32, "Key NAS Encryption (k_nas_enc)\n");
  m_s1ap_log->info_hex(ue_emm_ctx->security_ctxt.k_nas_int, 32, "Key NAS Integrity (k_nas_int)\n");

  uint8_t key_enb[32];
  srslte::security_generate_k_enb(ue_emm_ctx->security_ctxt.k_asme, ue_emm_ctx->security_ctxt.ul_nas_count, ue_emm_ctx->security_ctxt.k_enb);
  m_s1ap_log->info("Generating KeNB with UL NAS COUNT: %d\n", ue_emm_ctx->security_ctxt.ul_nas_count);
  m_s1ap_log->console("Generating KeNB with UL NAS COUNT: %d\n", ue_emm_ctx->security_ctxt.ul_nas_count);
  m_s1ap_log->info_hex(ue_emm_ctx->security_ctxt.k_enb, 32, "Key eNodeB (k_enb)\n");
  //Generate MAC for integrity protection
  //FIXME Write wrapper to support EIA1, EIA2, etc.
  srslte::security_128_eia1 (&ue_emm_ctx->security_ctxt.k_nas_int[16],
                     ue_emm_ctx->security_ctxt.dl_nas_count,
                     0,
                     SECURITY_DIRECTION_DOWNLINK,
                     &nas_buffer->msg[5],
                     nas_buffer->N_bytes - 5,
                     mac
  ); 

  memcpy(&nas_buffer->msg[1],mac,4);
  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->console("Error packing Authentication Request\n");
    return false;
  }
  m_s1ap_log->debug_hex(reply_msg->msg, reply_msg->N_bytes, "Security Mode Command: ");
  m_pool->deallocate(nas_buffer);
  return true;
}

bool
s1ap_nas_transport::pack_esm_information_request(srslte::byte_buffer_t *reply_msg, ue_emm_ctx_t *ue_emm_ctx, ue_ecm_ctx_t *ue_ecm_ctx)
{
  srslte::byte_buffer_t *nas_buffer = m_pool->allocate();

  //Setup initiating message
  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  bzero(&tx_pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));

  tx_pdu.ext          = false;
  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &tx_pdu.choice.initiatingMessage;
  init->procedureCode = LIBLTE_S1AP_PROC_ID_DOWNLINKNASTRANSPORT;
  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_DOWNLINKNASTRANSPORT;

  //Setup Dw NAS structure
  LIBLTE_S1AP_MESSAGE_DOWNLINKNASTRANSPORT_STRUCT *dw_nas = &init->choice.DownlinkNASTransport;
  dw_nas->ext=false;
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = ue_ecm_ctx->mme_ue_s1ap_id;//FIXME Change name
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ue_ecm_ctx->enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  LIBLTE_MME_ESM_INFORMATION_REQUEST_MSG_STRUCT esm_info_req;
  esm_info_req.eps_bearer_id = 0;
  esm_info_req.proc_transaction_id = ue_emm_ctx->procedure_transaction_id;

  uint8_t sec_hdr_type = LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED;

  ue_emm_ctx->security_ctxt.dl_nas_count++;
  LIBLTE_ERROR_ENUM err = srslte_mme_pack_esm_information_request_msg(&esm_info_req, sec_hdr_type,ue_emm_ctx->security_ctxt.dl_nas_count,(LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing ESM information request\n");
    m_s1ap_log->console("Error packing ESM information request\n");
    return false;
  }

  uint8_t mac[4];
  srslte::security_128_eia1 (&ue_emm_ctx->security_ctxt.k_nas_int[16],
                             ue_emm_ctx->security_ctxt.dl_nas_count,
                             0,
                             SECURITY_DIRECTION_DOWNLINK,
                             &nas_buffer->msg[5],
                             nas_buffer->N_bytes - 5,
                             mac
                             );

  memcpy(&nas_buffer->msg[1],mac,4);
  
  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Dw NAS Transport: Authentication Reject\n");
    m_s1ap_log->console("Error packing Downlink NAS Transport: Authentication Reject\n");
    return false;
  }

  m_pool->deallocate(nas_buffer);
  return true;
}

bool
s1ap_nas_transport::pack_attach_accept(ue_emm_ctx_t *ue_emm_ctx, ue_ecm_ctx_t *ue_ecm_ctx, LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT *erab_ctxt, struct srslte::gtpc_pdn_address_allocation_ie *paa, srslte::byte_buffer_t *nas_buffer) {
  LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT attach_accept;
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT act_def_eps_bearer_context_req;
  //bzero(&act_def_eps_bearer_context_req,sizeof(LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT));

  m_s1ap_log->info("Packing Attach Accept\n");

  //Get decimal MCC and MNC
  uint32_t mcc = 0;
  mcc += 0x000F & m_s1ap->m_s1ap_args.mcc;
  mcc += 10*( (0x00F0 & m_s1ap->m_s1ap_args.mcc) >> 4);
  mcc += 100*( (0x0F00 & m_s1ap->m_s1ap_args.mcc) >> 8);

  uint32_t mnc = 0;
  if( 0xFF00 == (m_s1ap->m_s1ap_args.mnc & 0xFF00 ))
  {
    //Two digit MNC
    mnc += 0x000F & m_s1ap->m_s1ap_args.mnc;
    mnc += 10*((0x00F0 & m_s1ap->m_s1ap_args.mnc) >> 4);
  }
  else
  {
    //Three digit MNC
    mnc += 0x000F & m_s1ap->m_s1ap_args.mnc;
    mnc += 10*((0x00F0 & m_s1ap->m_s1ap_args.mnc) >> 4);
    mnc += 100*((0x0F00 & m_s1ap->m_s1ap_args.mnc) >> 8);
  }

  //Attach accept
  attach_accept.eps_attach_result = ue_emm_ctx->attach_type;

  //FIXME: Set t3412 from config
  attach_accept.t3412.unit = LIBLTE_MME_GPRS_TIMER_UNIT_1_MINUTE;   // GPRS 1 minute unit
  attach_accept.t3412.value = 30;                                    // 30 minute periodic timer
  //FIXME: Set tai_list from config
  attach_accept.tai_list.N_tais = 1;
  attach_accept.tai_list.tai[0].mcc = mcc;
  attach_accept.tai_list.tai[0].mnc = mnc;
  attach_accept.tai_list.tai[0].tac = m_s1ap->m_s1ap_args.tac;

  //Allocate a GUTI ot the UE
  attach_accept.guti_present=true;
  attach_accept.guti.type_of_id = 6; //110 -> GUTI
  attach_accept.guti.guti.mcc = mcc;
  attach_accept.guti.guti.mnc = mnc;
  attach_accept.guti.guti.mme_group_id = m_s1ap->m_s1ap_args.mme_group;
  attach_accept.guti.guti.mme_code = m_s1ap->m_s1ap_args.mme_code;
  attach_accept.guti.guti.m_tmsi = m_s1ap->allocate_m_tmsi(ue_emm_ctx->imsi);
  m_s1ap_log->debug("Allocated GUTI: MCC %d, MNC %d, MME Group Id %d, MME Code 0x%x, M-TMSI 0x%x\n",
                    attach_accept.guti.guti.mcc,
                    attach_accept.guti.guti.mnc,
                    attach_accept.guti.guti.mme_group_id,
                    attach_accept.guti.guti.mme_code,
                    attach_accept.guti.guti.m_tmsi);

  //Set EMM cause to no CS available
  attach_accept.emm_cause_present=false;
  //attach_accept.emm_cause_present=true;
  //attach_accept.emm_cause=18;

  //Set up LAI for combined EPS/IMSI attach
  //attach_accept.lai_present=false;
  attach_accept.lai_present=true;
  attach_accept.lai.mcc = mcc;
  attach_accept.lai.mnc = mnc;
  attach_accept.lai.lac = 001;

  attach_accept.ms_id_present=true;
  attach_accept.ms_id.type_of_id = LIBLTE_MME_MOBILE_ID_TYPE_TMSI;
  attach_accept.ms_id.tmsi = attach_accept.guti.guti.m_tmsi;

  //Make sure all unused options are set to false
  attach_accept.t3402_present=false;
  attach_accept.t3423_present=false;
  attach_accept.equivalent_plmns_present=false;
  attach_accept.emerg_num_list_present=false;
  attach_accept.eps_network_feature_support_present=false;
  attach_accept.additional_update_result_present=false;
  attach_accept.t3412_ext_present=false;

  //Set activate default eps bearer (esm_ms)
  //Set pdn_addr
  act_def_eps_bearer_context_req.pdn_addr.pdn_type = LIBLTE_MME_PDN_TYPE_IPV4;
  memcpy(act_def_eps_bearer_context_req.pdn_addr.addr, &paa->ipv4, 4);
  //Set eps bearer id
  act_def_eps_bearer_context_req.eps_bearer_id = erab_ctxt->e_RAB_ID.E_RAB_ID;
  act_def_eps_bearer_context_req.transaction_id_present = false;
  //set eps_qos
  act_def_eps_bearer_context_req.eps_qos.qci =  erab_ctxt->e_RABlevelQoSParameters.qCI.QCI;
  act_def_eps_bearer_context_req.eps_qos.mbr_ul = 254; //FIXME
  act_def_eps_bearer_context_req.eps_qos.mbr_dl = 254; //FIXME
  act_def_eps_bearer_context_req.eps_qos.mbr_ul_ext = 250; //FIXME
  act_def_eps_bearer_context_req.eps_qos.mbr_dl_ext = 250; //FIXME check

  //set apn
  strncpy(act_def_eps_bearer_context_req.apn.apn, m_s1ap->m_s1ap_args.mme_apn.c_str(), LIBLTE_STRING_LEN);
  act_def_eps_bearer_context_req.proc_transaction_id = ue_emm_ctx->procedure_transaction_id; //FIXME

  //Set DNS server
  act_def_eps_bearer_context_req.protocol_cnfg_opts_present = true;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.N_opts = 1;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].id = 0x0d;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].len = 4;

  struct sockaddr_in dns_addr;
  inet_pton(AF_INET, m_s1ap->m_s1ap_args.dns_addr.c_str(), &(dns_addr.sin_addr));
  memcpy(act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].contents,&dns_addr.sin_addr.s_addr, 4);

  //Make sure all unused options are set to false
  act_def_eps_bearer_context_req.negotiated_qos_present = false;
  act_def_eps_bearer_context_req.llc_sapi_present = false;
  act_def_eps_bearer_context_req.radio_prio_present = false;
  act_def_eps_bearer_context_req.packet_flow_id_present = false;
  act_def_eps_bearer_context_req.apn_ambr_present = false;
  act_def_eps_bearer_context_req.esm_cause_present = false;
  act_def_eps_bearer_context_req.connectivity_type_present = false;

  uint8_t sec_hdr_type =2;
  ue_emm_ctx->security_ctxt.dl_nas_count++;
  liblte_mme_pack_activate_default_eps_bearer_context_request_msg(&act_def_eps_bearer_context_req, &attach_accept.esm_msg);
  liblte_mme_pack_attach_accept_msg(&attach_accept, sec_hdr_type, ue_emm_ctx->security_ctxt.dl_nas_count, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  //Integrity protect NAS message
  uint8_t mac[4];
  srslte::security_128_eia1 (&ue_emm_ctx->security_ctxt.k_nas_int[16],
                             ue_emm_ctx->security_ctxt.dl_nas_count,
                             0,
                             SECURITY_DIRECTION_DOWNLINK,
                             &nas_buffer->msg[5],
                             nas_buffer->N_bytes - 5,
                             mac
                             );

  memcpy(&nas_buffer->msg[1],mac,4);
  m_s1ap_log->info("Packed Attach Complete\n");
 
  //Add nas message to context setup request
  erab_ctxt->nAS_PDU_present = true;
  memcpy(erab_ctxt->nAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  erab_ctxt->nAS_PDU.n_octets = nas_buffer->N_bytes;

  return true;
}

bool
s1ap_nas_transport::pack_identity_request(srslte::byte_buffer_t *reply_msg, uint32_t enb_ue_s1ap_id, uint32_t mme_ue_s1ap_id)
{
  srslte::byte_buffer_t *nas_buffer = m_pool->allocate();

  //Setup initiating message
  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  bzero(&tx_pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));

  tx_pdu.ext          = false;
  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &tx_pdu.choice.initiatingMessage;
  init->procedureCode = LIBLTE_S1AP_PROC_ID_DOWNLINKNASTRANSPORT;
  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_DOWNLINKNASTRANSPORT;

  //Setup Dw NAS structure
  LIBLTE_S1AP_MESSAGE_DOWNLINKNASTRANSPORT_STRUCT *dw_nas = &init->choice.DownlinkNASTransport;
  dw_nas->ext=false;
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  LIBLTE_MME_ID_REQUEST_MSG_STRUCT id_req;
  id_req.id_type = LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_identity_request_msg(&id_req, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Identity Request\n");
    m_s1ap_log->console("Error packing Identity REquest\n");
    return false;
  }

  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Dw NAS Transport: Authentication Reject\n");
    m_s1ap_log->console("Error packing Downlink NAS Transport: Authentication Reject\n");
    return false;
  }

  m_pool->deallocate(nas_buffer);
  return true;
}

bool
s1ap_nas_transport::pack_emm_information( ue_ctx_t *ue_ctx, srslte::byte_buffer_t *reply_msg)
{
  srslte::byte_buffer_t *nas_buffer = m_pool->allocate();

  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;
  //Setup initiating message
  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  bzero(&tx_pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));

  tx_pdu.ext          = false;
  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &tx_pdu.choice.initiatingMessage;
  init->procedureCode = LIBLTE_S1AP_PROC_ID_DOWNLINKNASTRANSPORT;
  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_DOWNLINKNASTRANSPORT;

  //Setup Dw NAS structure
  LIBLTE_S1AP_MESSAGE_DOWNLINKNASTRANSPORT_STRUCT *dw_nas = &init->choice.DownlinkNASTransport;
  dw_nas->ext=false;
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = ecm_ctx->mme_ue_s1ap_id;//FIXME Change name
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ecm_ctx->enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT emm_info;
  emm_info.full_net_name_present = true;
  strncpy(emm_info.full_net_name.name, "Software Radio Systems LTE", LIBLTE_STRING_LEN);
  emm_info.full_net_name.add_ci = LIBLTE_MME_ADD_CI_DONT_ADD;
  emm_info.short_net_name_present = true;
  strncpy(emm_info.short_net_name.name, "srsLTE", LIBLTE_STRING_LEN);
  emm_info.short_net_name.add_ci = LIBLTE_MME_ADD_CI_DONT_ADD;

  emm_info.local_time_zone_present = false;
  emm_info.utc_and_local_time_zone_present = false;
  emm_info.net_dst_present = false;

  uint8_t sec_hdr_type =2;
  emm_ctx->security_ctxt.dl_nas_count++;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_emm_information_msg(&emm_info, sec_hdr_type, emm_ctx->security_ctxt.dl_nas_count, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing EMM Information\n");
    m_s1ap_log->console("Error packing EMM Information\n");
    return false;
  }

  uint8_t mac[4];
  srslte::security_128_eia1 (&emm_ctx->security_ctxt.k_nas_int[16],
                             emm_ctx->security_ctxt.dl_nas_count,
                             0,
                             SECURITY_DIRECTION_DOWNLINK,
                             &nas_buffer->msg[5],
                             nas_buffer->N_bytes - 5,
                             mac
                             );

  memcpy(&nas_buffer->msg[1],mac,4);
  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Dw NAS Transport: EMM Info\n");
    m_s1ap_log->console("Error packing Downlink NAS Transport: EMM Info\n");
    return false;
  } 

  m_s1ap_log->info("Packed UE EMM information\n"); 
  return true;
}

bool
s1ap_nas_transport::pack_service_reject(srslte::byte_buffer_t *reply_msg, uint8_t emm_cause, uint32_t enb_ue_s1ap_id)
{
  srslte::byte_buffer_t *nas_buffer = m_pool->allocate();

  //Setup initiating message
  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  bzero(&tx_pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));

  tx_pdu.ext          = false;
  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &tx_pdu.choice.initiatingMessage;
  init->procedureCode = LIBLTE_S1AP_PROC_ID_DOWNLINKNASTRANSPORT;
  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_DOWNLINKNASTRANSPORT;

  //Setup Dw NAS structure
  LIBLTE_S1AP_MESSAGE_DOWNLINKNASTRANSPORT_STRUCT *dw_nas = &init->choice.DownlinkNASTransport;
  dw_nas->ext=false;
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = m_s1ap->get_next_mme_ue_s1ap_id();
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;
  LIBLTE_MME_SERVICE_REJECT_MSG_STRUCT service_rej;
  service_rej.t3442_present = true;
  service_rej.t3442.unit = LIBLTE_MME_GPRS_TIMER_DEACTIVATED;
  service_rej.t3442.value = 0;
  service_rej.t3446_present = true;
  service_rej.t3446 = 0;
  service_rej.emm_cause = emm_cause;

  LIBLTE_ERROR_ENUM err = liblte_mme_pack_service_reject_msg(&service_rej, LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS, 0, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Service Reject\n");
    m_s1ap_log->console("Error packing Service Reject\n");
    return false;
  }

  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Dw NAS Transport: Service Reject\n");
    m_s1ap_log->console("Error packing Downlink NAS Transport: Service Reject\n");
    return false;
  }
  return true;
}
/*Helper functions*/
void
s1ap_nas_transport::log_unhandled_attach_request_ies(const LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT *attach_req)
{
  if(attach_req->old_p_tmsi_signature_present)
  {
    m_s1ap_log->warning("NAS attach request: Old P-TMSI signature present, but not handled.\n");
  }
  if(attach_req->additional_guti_present)
  {
    m_s1ap_log->warning("NAS attach request: Aditional GUTI present, but not handled.\n");
  }
  if(attach_req->last_visited_registered_tai_present)
  {
    m_s1ap_log->warning("NAS attach request: Last visited registered TAI present, but not handled.\n");
  }
  if(attach_req->drx_param_present)
  {
    m_s1ap_log->warning("NAS attach request: DRX Param present, but not handled.\n");
  }
  if(attach_req->ms_network_cap_present)
  {
    m_s1ap_log->warning("NAS attach request: MS network cap present, but not handled.\n");
  }
  if(attach_req->old_lai_present)
  {
    m_s1ap_log->warning("NAS attach request: Old LAI present, but not handled.\n");
  }
  if(attach_req->tmsi_status_present)
  {
    m_s1ap_log->warning("NAS attach request: TSMI status present, but not handled.\n");
  }
  if(attach_req->ms_cm2_present)
  {
    m_s1ap_log->warning("NAS attach request: MS CM2 present, but not handled.\n");
  }
  if(attach_req->ms_cm3_present)
  {
    m_s1ap_log->warning("NAS attach request: MS CM3 present, but not handled.\n");
  }
  if(attach_req->supported_codecs_present)
  {
    m_s1ap_log->warning("NAS attach request: Supported CODECs present, but not handled.\n");
  }
  if(attach_req->additional_update_type_present)
  {
    m_s1ap_log->warning("NAS attach request: Additional Update Type present, but not handled.\n");
  }
  if(attach_req->voice_domain_pref_and_ue_usage_setting_present)
  {
    m_s1ap_log->warning("NAS attach request: Voice domain preference and UE usage setting  present, but not handled.\n");
  }
  if(attach_req->device_properties_present)
  {
    m_s1ap_log->warning("NAS attach request: Device properties present, but not handled.\n");
  }
  if(attach_req->old_guti_type_present)
  {
    m_s1ap_log->warning("NAS attach request: Old GUTI type present, but not handled.\n");
  }
  return;
}

void
s1ap_nas_transport::log_unhandled_pdn_con_request_ies(const LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT *pdn_con_req)
{
  //Handle the optional flags
  if(pdn_con_req->esm_info_transfer_flag_present)
  {
    m_s1ap_log->warning("PDN Connectivity request: ESM info transfer flag properties present, but not handled.\n");
  }
  if(pdn_con_req->apn_present)
  {
    m_s1ap_log->warning("PDN Connectivity request: APN present, but not handled.\n");
  }
  if(pdn_con_req->protocol_cnfg_opts_present)
  {
    m_s1ap_log->warning("PDN Connectivity request: Protocol Cnfg options present, but not handled.\n");
  }
  if(pdn_con_req->device_properties_present)
  {
    m_s1ap_log->warning("PDN Connectivity request: Device properties present, but not handled.\n");
  }
}


void
s1ap_nas_transport::log_unhandled_initial_ue_message_ies(LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT *init_ue)
{
  if(init_ue->S_TMSI_present){
    m_s1ap_log->warning("S-TMSI present, but not handled.\n");
  }
  if(init_ue->CSG_Id_present){
    m_s1ap_log->warning("S-TMSI present, but not handled.\n");
  }
  if(init_ue->GUMMEI_ID_present){
    m_s1ap_log->warning("GUMMEI ID present, but not handled.\n");
  }
  if(init_ue->CellAccessMode_present){
    m_s1ap_log->warning("Cell Access Mode present, but not handled.\n");
  } 
  if(init_ue->GW_TransportLayerAddress_present){
    m_s1ap_log->warning("GW Transport Layer present, but not handled.\n");
  }
  if(init_ue->GW_TransportLayerAddress_present){
    m_s1ap_log->warning("GW Transport Layer present, but not handled.\n");
  }
  if(init_ue->RelayNode_Indicator_present){
    m_s1ap_log->warning("Relay Node Indicator present, but not handled.\n");
  }
  if(init_ue->GUMMEIType_present){
    m_s1ap_log->warning("GUMMEI Type present, but not handled.\n");
  }
  if(init_ue->Tunnel_Information_for_BBF_present){
    m_s1ap_log->warning("Tunnel Information for BBF present, but not handled.\n");
  }
  if(init_ue->SIPTO_L_GW_TransportLayerAddress_present){
    m_s1ap_log->warning("SIPTO GW Transport Layer Address present, but not handled.\n");
  }
  if(init_ue->LHN_ID_present){
    m_s1ap_log->warning("LHN Id present, but not handled.\n");
  }
  return;
}


} //namespace srsepc
