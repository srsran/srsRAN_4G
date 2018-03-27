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
#include "mme/s1ap.h"
#include "mme/s1ap_nas_transport.h"
#include "srslte/common/security.h"

namespace srsepc{

s1ap_nas_transport*          s1ap_nas_transport::m_instance = NULL;
boost::mutex                 s1ap_nas_transport_instance_mutex;

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
  boost::mutex::scoped_lock lock(s1ap_nas_transport_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new s1ap_nas_transport();
  }
  return(m_instance);
}

void
s1ap_nas_transport::cleanup(void)
{
  boost::mutex::scoped_lock lock(s1ap_nas_transport_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
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
  LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT attach_req;
  LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT pdn_con_req;
  LIBLTE_MME_SERVICE_REQUEST_MSG_STRUCT service_req;

  m_s1ap_log->console("Received Initial UE Message.\n");
  m_s1ap_log->info("Received Initial UE Message.\n");

  //Get info from initial UE message
  uint32_t enb_ue_s1ap_id = init_ue->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID;

  //Log unhandled Initial UE message IEs
  log_unhandled_initial_ue_message_ies(init_ue);

  /*Check whether NAS Attach Request or Service Request*/
  uint8_t pd, msg_type;
  srslte::byte_buffer_t *nas_msg = m_pool->allocate();
  memcpy(nas_msg->msg, &init_ue->NAS_PDU.buffer, init_ue->NAS_PDU.n_octets);
  nas_msg->N_bytes = init_ue->NAS_PDU.n_octets;
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &pd, &msg_type);
  if(msg_type == LIBLTE_MME_MSG_TYPE_ATTACH_REQUEST)
  {
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
    handle_nas_attach_request(enb_ue_s1ap_id, attach_req, pdn_con_req, reply_buffer, reply_flag, enb_sri);
  }
  else if(msg_type == LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST)
  {
    m_s1ap_log->info("Received Service Request \n");
    m_s1ap_log->console("Received Service Request \n");
    liblte_mme_unpack_service_request_msg((LIBLTE_BYTE_MSG_STRUCT*) nas_msg, &service_req);
    return false;
  }
  m_pool->deallocate(nas_msg);

  return true;
}

bool
s1ap_nas_transport::handle_uplink_nas_transport(LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT *ul_xport, struct sctp_sndrcvinfo *enb_sri, srslte::byte_buffer_t *reply_buffer, bool *reply_flag)
{

  bool     ue_valid = true;
  uint32_t enb_ue_s1ap_id = ul_xport->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID;
  uint32_t mme_ue_s1ap_id = ul_xport->MME_UE_S1AP_ID.MME_UE_S1AP_ID;

  ue_ctx_t *ue_ctx = m_s1ap->find_ue_ctx(mme_ue_s1ap_id);
  if(ue_ctx == NULL)
  {
    //TODO UE not registered, send error message.
    m_s1ap_log->warning("Received uplink NAS, but could not find UE context. MME-UE S1AP id: %lu\n",mme_ue_s1ap_id);
    return false;
  }

  m_s1ap_log->debug("Received uplink NAS and found UE. MME-UE S1AP id: %lu\n",mme_ue_s1ap_id);

  //Get NAS message type
  uint8_t pd, msg_type;
  srslte::byte_buffer_t *nas_msg = m_pool->allocate();

  memcpy(nas_msg->msg, &ul_xport->NAS_PDU.buffer, ul_xport->NAS_PDU.n_octets);
  nas_msg->N_bytes = ul_xport->NAS_PDU.n_octets;
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &pd, &msg_type);

  switch (msg_type) {
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_RESPONSE:
      m_s1ap_log->info("Uplink NAS: Received Authentication Response\n");
      m_s1ap_log->console("Uplink NAS: Received Authentication Response\n");
      handle_nas_authentication_response(nas_msg, ue_ctx, reply_buffer, reply_flag);
      break;
    case  LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMPLETE:
      m_s1ap_log->info("Uplink NAS: Received Security Mode Complete\n");
      m_s1ap_log->console("Uplink NAS: Received Security Mode Complete\n");
      handle_nas_security_mode_complete(nas_msg, ue_ctx, reply_buffer, reply_flag);
      break;
    case  LIBLTE_MME_MSG_TYPE_ATTACH_COMPLETE:
      m_s1ap_log->info("Uplink NAS: Received Attach Complete\n");
      m_s1ap_log->console("Uplink NAS: Received Attach Complete\n");
      handle_nas_attach_complete(nas_msg, ue_ctx, reply_buffer, reply_flag);
      ue_ctx->security_ctxt.ul_nas_count++;
      break;
    case LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_RESPONSE:
      m_s1ap_log->info("Uplink NAS: Received ESM Information Response\n");
      m_s1ap_log->console("Uplink NAS: Received ESM Information Response\n");
      handle_esm_information_response(nas_msg, ue_ctx, reply_buffer, reply_flag);
      ue_ctx->security_ctxt.ul_nas_count++;
      break;
    case LIBLTE_MME_MSG_TYPE_IDENTITY_RESPONSE:
      m_s1ap_log->info("Uplink NAS: Received Identity Response\n");
      m_s1ap_log->console("Uplink NAS: Received Identity Response\n");
      handle_identity_response(nas_msg, ue_ctx, reply_buffer, reply_flag);
      //ue_ctx->security_ctxt.ul_nas_count++;
      break;
    case LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_REQUEST:
      m_s1ap_log->info("Uplink NAS: Tracking Area Update Request\n");
      handle_tracking_area_update_request(nas_msg, ue_ctx, reply_buffer, reply_flag);
      break;
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_FAILURE:
      m_s1ap_log->info("Uplink NAS: Authentication Failure\n");
      handle_authentication_failure(nas_msg, ue_ctx, reply_buffer, reply_flag);
      ue_ctx->security_ctxt.ul_nas_count++;
      break;
    default:
      m_s1ap_log->warning("Unhandled NAS message 0x%x\n", msg_type );
      m_s1ap_log->console("Unhandled NAS message 0x%x\n", msg_type );
      return false; //FIXME (nas_msg deallocate needs to be called)
  }

  if(*reply_flag == true)
  {
    m_s1ap_log->info("DL NAS: Sent Downlink NAS message\n");
    m_s1ap_log->console("DL NAS: Sent Downlink NAS Message\n");
  }
  m_pool->deallocate(nas_msg);

  return true;
}

bool
s1ap_nas_transport::handle_nas_attach_request(uint32_t enb_ue_s1ap_id,
                                              const LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT &attach_req,
                                              const LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT &pdn_con_req,
                                              srslte::byte_buffer_t *reply_buffer,
                                              bool* reply_flag,
                                              struct sctp_sndrcvinfo *enb_sri)
{
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
    handle_nas_guti_attach_request(enb_ue_s1ap_id, attach_req, pdn_con_req, reply_buffer, reply_flag, enb_sri);
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
  ue_ctx.imsi = 0;
  ue_ctx.enb_ue_s1ap_id = enb_ue_s1ap_id;
  ue_ctx.mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();

  //Save UE network capabilities
  memcpy(&ue_ctx.ue_network_cap, &attach_req.ue_network_cap, sizeof(LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT));
  ue_ctx.ms_network_cap_present =  attach_req.ms_network_cap_present;
  if(attach_req.ms_network_cap_present)
  {
    memcpy(&ue_ctx.ms_network_cap, &attach_req.ms_network_cap, sizeof(LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT));
  }
  uint8_t eps_bearer_id = pdn_con_req.eps_bearer_id;             //TODO: Unused
  ue_ctx.procedure_transaction_id = pdn_con_req.proc_transaction_id; 

  //Save whether ESM information transfer is necessary
  ue_ctx.eit = pdn_con_req.esm_info_transfer_flag_present;
  //m_s1ap_log->console("EPS Bearer id: %d\n", eps_bearer_id);
  //Initialize NAS count
  ue_ctx.security_ctxt.ul_nas_count = 0;
  ue_ctx.security_ctxt.dl_nas_count = 0;
  //Add eNB info to UE ctxt
  memcpy(&ue_ctx.enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));
  //Initialize E-RABs
  for(uint i = 0 ; i< MAX_ERABS_PER_UE; i++)
  {
    ue_ctx.erabs_ctx[i].state = ERAB_DEACTIVATED;
    ue_ctx.erabs_ctx[i].erab_id = i;
  }

  //IMSI style attach
  ue_ctx.imsi = 0;
  for(int i=0;i<=14;i++){
    ue_ctx.imsi  += attach_req.eps_mobile_id.imsi[i]*std::pow(10,14-i);
  }

  m_s1ap_log->console("Attach request -- IMSI: %015lu\n", ue_ctx.imsi);
  m_s1ap_log->info("Attach request -- IMSI: %015lu\n", ue_ctx.imsi);
  m_s1ap_log->console("Attach request -- eNB-UE S1AP Id: %d, MME-UE S1AP Id: %d\n", ue_ctx.enb_ue_s1ap_id, ue_ctx.mme_ue_s1ap_id);
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
 
  //Get Authentication Vectors from HSS
  if(!m_hss->gen_auth_info_answer(ue_ctx.imsi, ue_ctx.security_ctxt.k_asme, autn, rand, ue_ctx.security_ctxt.xres))
  {
    m_s1ap_log->console("User not found. IMSI %015lu\n",ue_ctx.imsi);
    m_s1ap_log->info("User not found. IMSI %015lu\n",ue_ctx.imsi);
    return false;
  }

  m_s1ap->add_new_ue_ctx(ue_ctx);
  //Pack NAS Authentication Request in Downlink NAS Transport msg
  pack_authentication_request(reply_buffer, ue_ctx.enb_ue_s1ap_id, ue_ctx.mme_ue_s1ap_id, autn, rand);
  
  //Send reply to eNB
  *reply_flag = true;
  m_s1ap_log->info("Downlink NAS: Sending Authentication Request\n");
  m_s1ap_log->console("Downlink NAS: Sending Authentication Request\n");
  return true;
}

bool
s1ap_nas_transport::handle_nas_guti_attach_request(uint32_t enb_ue_s1ap_id,
                                                     const LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT &attach_req,
                                                     const LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT &pdn_con_req,
                                                     srslte::byte_buffer_t *reply_buffer,
                                                     bool* reply_flag,
                                                     struct sctp_sndrcvinfo *enb_sri)
{
  //GUTI style attach
  uint32_t m_tmsi = attach_req.eps_mobile_id.guti.m_tmsi;
  std::map<uint32_t,uint32_t>::iterator it = m_s1ap->m_tmsi_to_s1ap_id.find(m_tmsi);
  if(it == m_s1ap->m_tmsi_to_s1ap_id.end())
  {
    //Could not find IMSI from M-TMSI, send Id request
    ue_ctx_t ue_ctx;
    ue_ctx.imsi = 0;
    ue_ctx.enb_ue_s1ap_id = enb_ue_s1ap_id;
    ue_ctx.mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();

    //Save UE network capabilities
    memcpy(&ue_ctx.ue_network_cap, &attach_req.ue_network_cap, sizeof(LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT));
    ue_ctx.ms_network_cap_present =  attach_req.ms_network_cap_present;
    if(attach_req.ms_network_cap_present)
    {
      memcpy(&ue_ctx.ms_network_cap, &attach_req.ms_network_cap, sizeof(LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT));
    }
    uint8_t eps_bearer_id = pdn_con_req.eps_bearer_id;             //TODO: Unused
    ue_ctx.procedure_transaction_id = pdn_con_req.proc_transaction_id;

    //Save whether ESM information transfer is necessary
    ue_ctx.eit = pdn_con_req.esm_info_transfer_flag_present;
    //m_s1ap_log->console("EPS Bearer id: %d\n", eps_bearer_id);
    //Initialize NAS count
    ue_ctx.security_ctxt.ul_nas_count = 0;
    ue_ctx.security_ctxt.dl_nas_count = 0;
    //Add eNB info to UE ctxt
    memcpy(&ue_ctx.enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));
    //Initialize E-RABs
    for(uint i = 0 ; i< MAX_ERABS_PER_UE; i++)
    {
      ue_ctx.erabs_ctx[i].state = ERAB_DEACTIVATED;
      ue_ctx.erabs_ctx[i].erab_id = i;
    }
    m_s1ap_log->console("Attach request -- IMSI: %015lu\n", ue_ctx.imsi);
    m_s1ap_log->info("Attach request -- IMSI: %015lu\n", ue_ctx.imsi);
    m_s1ap_log->console("Attach request -- eNB-UE S1AP Id: %d, MME-UE S1AP Id: %d\n", ue_ctx.enb_ue_s1ap_id, ue_ctx.mme_ue_s1ap_id);
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

    m_s1ap_log->console("Could not find M-TMSI=0x%x. Sending ID request\n",m_tmsi);
    m_s1ap_log->info("Could not find M-TMSI=0x%d. Sending Id Request\n", m_tmsi);
    m_s1ap->add_new_ue_ctx(ue_ctx);
    pack_identity_request(reply_buffer, ue_ctx.enb_ue_s1ap_id, ue_ctx.mme_ue_s1ap_id);
    *reply_flag = true;
    return true;
  }
  else{
    m_s1ap_log->console("Attach Request -- Found M-TMSI: %d\n",m_tmsi);
    ue_ctx_t *ue_ctx_ptr = m_s1ap->find_ue_ctx(it->second);
    if(ue_ctx_ptr!=NULL)
    {
      m_s1ap_log->console("Found UE context. IMSI: %015lu\n",ue_ctx_ptr->imsi);
      m_mme_gtpc->send_create_session_request(ue_ctx_ptr->imsi, ue_ctx_ptr->mme_ue_s1ap_id);
      *reply_flag = false; //No reply needed
      return true;
    }
    else
    {
      m_s1ap_log->error("Found M-TMSI but could not find UE context\n");
      return false;
    }
  }
  return true;
}

bool
s1ap_nas_transport::handle_nas_authentication_response(srslte::byte_buffer_t *nas_msg, ue_ctx_t *ue_ctx, srslte::byte_buffer_t *reply_buffer, bool* reply_flag)
{

  LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT auth_resp;
  bool ue_valid=true;

  m_s1ap_log->console("Authentication Response -- IMSI %015lu\n", ue_ctx->imsi);

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
    if(auth_resp.res[i] != ue_ctx->security_ctxt.xres[i])
    {
      ue_valid = false;
    }
  }
  if(!ue_valid)
  {
    std::cout<<std::endl;
    std::cout<<"XRES: ";
    for(int i=0;i<8;i++)
    {
      std::cout << std::hex <<(uint16_t)ue_ctx->security_ctxt.xres[i];
    }
    std::cout<<std::endl;

    m_s1ap_log->console("UE Authentication Rejected.\n");
    m_s1ap_log->warning("UE Authentication Rejected.\n");
    //Send back Authentication Reject
    pack_authentication_reject(reply_buffer, ue_ctx->enb_ue_s1ap_id, ue_ctx->mme_ue_s1ap_id);
    *reply_flag = true;
    m_s1ap_log->console("Downlink NAS: Sending Authentication Reject.\n");
    return false;
  }
  else
  {
    m_s1ap_log->console("UE Authentication Accepted.\n");
    m_s1ap_log->info("UE Authentication Accepted.\n");
    //Send Security Mode Command
    pack_security_mode_command(reply_buffer, ue_ctx);
    *reply_flag = true;
    m_s1ap_log->console("Downlink NAS: Sending NAS Security Mode Command.\n");
  }
  return true;
}

bool
s1ap_nas_transport::handle_nas_security_mode_complete(srslte::byte_buffer_t *nas_msg, ue_ctx_t *ue_ctx, srslte::byte_buffer_t *reply_buffer, bool *reply_flag)
{
  LIBLTE_MME_SECURITY_MODE_COMPLETE_MSG_STRUCT sm_comp;

  //Get NAS authentication response
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_security_mode_complete_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &sm_comp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  //TODO Check integrity

  //TODO Handle imeisv
  if(sm_comp.imeisv_present)
  {
    m_s1ap_log->warning("IMEI-SV present but not handled");
  }

  m_s1ap_log->info("Security Mode Command Complete -- IMSI: %lu\n", ue_ctx->imsi);
  m_s1ap_log->console("Security Mode Command Complete -- IMSI: %lu\n", ue_ctx->imsi);
  if(ue_ctx->eit == true)
  {
    pack_esm_information_request(reply_buffer, ue_ctx);
    m_s1ap_log->console("Sending ESM information request\n");
    m_s1ap_log->info("Sending ESM information request\n");
    *reply_flag = true;
  }
  else
  {
    //FIXME The packging of GTP-C messages is not ready.
    //This means that GTP-U tunnels are created with function calls, as opposed to GTP-C.
    m_mme_gtpc->send_create_session_request(ue_ctx->imsi, ue_ctx->mme_ue_s1ap_id);
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

  m_s1ap_log->console("Unpacked Attached Complete Message\n");
  m_s1ap_log->console("Unpacked Activate Default EPS Bearer message. EPS Bearer id %d\n",act_bearer.eps_bearer_id);
  //ue_ctx->erabs_ctx[act_bearer->eps_bearer_id].enb_fteid;
  if(act_bearer.eps_bearer_id < 5 || act_bearer.eps_bearer_id > 15)
  {
    m_s1ap_log->error("EPS Bearer ID out of range\n");
    return false;
  }
  m_mme_gtpc->send_modify_bearer_request(&ue_ctx->erabs_ctx[act_bearer.eps_bearer_id]);
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
    m_s1ap_log->info("ESM Info: APN %s\n",esm_info_resp.eps_bearer_id);
    m_s1ap_log->console("ESM Info: APN %s\n",esm_info_resp.eps_bearer_id);
  }
  if(esm_info_resp.protocol_cnfg_opts_present)
  {
    m_s1ap_log->info("ESM Info: %d Protocol Configuration Options %s\n",esm_info_resp.protocol_cnfg_opts.N_opts);
    m_s1ap_log->console("ESM Info: %d Protocol Configuration Options %s\n",esm_info_resp.protocol_cnfg_opts.N_opts);
  }

  //FIXME The packging of GTP-C messages is not ready.
  //This means that GTP-U tunnels are created with function calls, as opposed to GTP-C.
  m_mme_gtpc->send_create_session_request(ue_ctx->imsi, ue_ctx->mme_ue_s1ap_id);
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
  m_s1ap_log->info("Id Response -- IMSI: %015lu\n", imsi);
  m_s1ap_log->console("Id Response -- IMSI: %015lu\n", imsi);
  ue_ctx->imsi = imsi;

  //Get Authentication Vectors from HSS
  if(!m_hss->gen_auth_info_answer(imsi, ue_ctx->security_ctxt.k_asme, autn, rand, ue_ctx->security_ctxt.xres))
  {
    m_s1ap_log->console("User not found. IMSI %015lu\n",imsi);
    m_s1ap_log->info("User not found. IMSI %015lu\n",imsi);
    return false;
  }
   
  //Pack NAS Authentication Request in Downlink NAS Transport msg
  pack_authentication_request(reply_msg, ue_ctx->enb_ue_s1ap_id, ue_ctx->mme_ue_s1ap_id, autn, rand);

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

  /*
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_tracking_area_update_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &tau_req);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }
  */
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = ue_ctx->mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ue_ctx->enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;
  //m_s1ap_log->console("Tracking area accept to MME-UE S1AP Id %d\n", ue_ctx->mme_ue_s1ap_id);
 
  LIBLTE_MME_TRACKING_AREA_UPDATE_ACCEPT_MSG_STRUCT tau_acc;
  /*typedef struct{
    LIBLTE_MME_GPRS_TIMER_STRUCT                  t3412;
    LIBLTE_MME_EPS_MOBILE_ID_STRUCT               guti;
    LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_STRUCT tai_list;
    LIBLTE_MME_EPS_BEARER_CONTEXT_STATUS_STRUCT   eps_bearer_context_status;
    LIBLTE_MME_LOCATION_AREA_ID_STRUCT            lai;
    LIBLTE_MME_MOBILE_ID_STRUCT                   ms_id;
    LIBLTE_MME_GPRS_TIMER_STRUCT                  t3402;
    LIBLTE_MME_GPRS_TIMER_STRUCT                  t3423;
    LIBLTE_MME_PLMN_LIST_STRUCT                   equivalent_plmns;
    LIBLTE_MME_EMERGENCY_NUMBER_LIST_STRUCT       emerg_num_list;
    LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_STRUCT eps_network_feature_support;
    LIBLTE_MME_GPRS_TIMER_3_STRUCT                t3412_ext;
    LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_ENUM      additional_update_result;
    uint8                                         eps_update_result;
    uint8                                         emm_cause;
    bool                                          t3412_present;
    bool                                          guti_present;
    bool                                          tai_list_present;
    bool                                          eps_bearer_context_status_present;
    bool                                          lai_present;
    bool                                          ms_id_present;
    bool                                          emm_cause_present;
    bool                                          t3402_present;
    bool                                          t3423_present;
    bool                                          equivalent_plmns_present;
    bool                                          emerg_num_list_present;
    bool                                          eps_network_feature_support_present;
    bool                                          additional_update_result_present;
    bool                                          t3412_ext_present;
}LIBLTE_MME_TRACKING_AREA_UPDATE_ACCEPT_MSG_STRUCT;
*/
  //Send reply to eNB
  //*reply_flag = true;

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
    m_s1ap_log->console("Sequence number synch failure\n");
    m_s1ap_log->info("Sequence number synch failure\n");
    if(auth_fail.auth_fail_param_present == false){
      m_s1ap_log->error("Missing fail parameter\n");
      return false;
    }
    if(!m_hss->resync_sqn(ue_ctx->imsi, auth_fail.auth_fail_param))
    {
      m_s1ap_log->console("Resynchronization failed. IMSI %015lu\n", ue_ctx->imsi);
      m_s1ap_log->info("Resynchronization failed. IMSI %015lu\n", ue_ctx->imsi);
      return false;
    }
    //Get Authentication Vectors from HSS
    if(!m_hss->gen_auth_info_answer(ue_ctx->imsi, ue_ctx->security_ctxt.k_asme, autn, rand, ue_ctx->security_ctxt.xres))
    {
      m_s1ap_log->console("User not found. IMSI %015lu\n", ue_ctx->imsi);
      m_s1ap_log->info("User not found. IMSI %015lu\n", ue_ctx->imsi);
      return false;
    }
    
    //Pack NAS Authentication Request in Downlink NAS Transport msg
    pack_authentication_request(reply_msg, ue_ctx->enb_ue_s1ap_id, ue_ctx->mme_ue_s1ap_id, autn, rand);

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
s1ap_nas_transport::pack_authentication_request(srslte::byte_buffer_t *reply_msg, uint32_t enb_ue_s1ap_id, uint32_t next_mme_ue_s1ap_id, uint8_t *autn, uint8_t *rand)
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
  auth_req.nas_ksi.nas_ksi=0;

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
s1ap_nas_transport::pack_security_mode_command(srslte::byte_buffer_t *reply_msg, ue_ctx_t *ue_ctx)
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = ue_ctx->mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ue_ctx->enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  //Pack NAS PDU 
  LIBLTE_MME_SECURITY_MODE_COMMAND_MSG_STRUCT sm_cmd;
 
  sm_cmd.selected_nas_sec_algs.type_of_eea = LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_EEA0;
  sm_cmd.selected_nas_sec_algs.type_of_eia = LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_128_EIA1;

  sm_cmd.nas_ksi.tsc_flag=LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
  sm_cmd.nas_ksi.nas_ksi=0; 

  //Replay UE security cap
  memcpy(sm_cmd.ue_security_cap.eea,ue_ctx->ue_network_cap.eea,8*sizeof(bool));
  memcpy(sm_cmd.ue_security_cap.eia,ue_ctx->ue_network_cap.eia,8*sizeof(bool));
  sm_cmd.ue_security_cap.uea_present = ue_ctx->ue_network_cap.uea_present;
  memcpy(sm_cmd.ue_security_cap.uea,ue_ctx->ue_network_cap.uea,8*sizeof(bool));
  sm_cmd.ue_security_cap.uia_present = ue_ctx->ue_network_cap.uia_present;
  memcpy(sm_cmd.ue_security_cap.uia,ue_ctx->ue_network_cap.uia,8*sizeof(bool));
  sm_cmd.ue_security_cap.gea_present = ue_ctx->ms_network_cap_present;
  memcpy(sm_cmd.ue_security_cap.gea,ue_ctx->ms_network_cap.gea,8*sizeof(bool));

  sm_cmd.imeisv_req_present=false;
  sm_cmd.nonce_ue_present=false;
  sm_cmd.nonce_mme_present=false;

  uint8_t  sec_hdr_type=3;
  
  ue_ctx->security_ctxt.dl_nas_count = 0;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_security_mode_command_msg(&sm_cmd,sec_hdr_type, ue_ctx->security_ctxt.dl_nas_count,(LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->console("Error packing Authentication Request\n");
    return false;
  }

  //Generate MAC for integrity protection
  //FIXME Write wrapper to support EIA1, EIA2, etc.
  //TODO which is the RB ID? Standard says a constant, but which?
  uint8_t mac[4];

  srslte::security_generate_k_nas( ue_ctx->security_ctxt.k_asme,
                           srslte::CIPHERING_ALGORITHM_ID_EEA0,
                           srslte::INTEGRITY_ALGORITHM_ID_128_EIA1,
                           ue_ctx->security_ctxt.k_nas_enc,
                           ue_ctx->security_ctxt.k_nas_int
                         );

  srslte::security_128_eia1 (&ue_ctx->security_ctxt.k_nas_int[16],
                     ue_ctx->security_ctxt.dl_nas_count,
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
s1ap_nas_transport::pack_esm_information_request(srslte::byte_buffer_t *reply_msg, ue_ctx_t *ue_ctx)
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = ue_ctx->mme_ue_s1ap_id;//FIXME Change name
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ue_ctx->enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  LIBLTE_MME_ESM_INFORMATION_REQUEST_MSG_STRUCT esm_info_req;
  esm_info_req.eps_bearer_id = 0;
  esm_info_req.proc_transaction_id = ue_ctx->procedure_transaction_id;
  uint8_t  sec_hdr_type=2;
  
  ue_ctx->security_ctxt.dl_nas_count++;
 
  LIBLTE_ERROR_ENUM err = srslte_mme_pack_esm_information_request_msg(&esm_info_req, sec_hdr_type,ue_ctx->security_ctxt.dl_nas_count,(LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing ESM information request\n");
    m_s1ap_log->console("Error packing ESM information request\n");
    return false;
  }

  uint8_t mac[4];
  srslte::security_128_eia1 (&ue_ctx->security_ctxt.k_nas_int[16],
                             ue_ctx->security_ctxt.dl_nas_count,
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
s1ap_nas_transport::pack_attach_accept(ue_ctx_t *ue_ctx, LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT *erab_ctxt, struct srslte::gtpc_pdn_address_allocation_ie *paa, srslte::byte_buffer_t *nas_buffer) {
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
  attach_accept.eps_attach_result = LIBLTE_MME_EPS_ATTACH_RESULT_EPS_ONLY;
  //Mandatory
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
  attach_accept.guti.guti.m_tmsi = m_s1ap->allocate_m_tmsi(ue_ctx->mme_ue_s1ap_id);
  m_s1ap_log->debug("Allocated GUTI: MCC %d, MNC %d, MME Group Id %d, MME Code 0x%x, M-TMSI 0x%x\n",
                    attach_accept.guti.guti.mcc,
                    attach_accept.guti.guti.mnc,
                    attach_accept.guti.guti.mme_group_id,
                    attach_accept.guti.guti.mme_code,
                    attach_accept.guti.guti.m_tmsi);

  //Make sure all unused options are set to false
  attach_accept.lai_present=false;
  attach_accept.ms_id_present=false;
  attach_accept.emm_cause_present=false;
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
  //act_def_eps_bearer_context_req.apn
  act_def_eps_bearer_context_req.apn.apn = m_s1ap->m_s1ap_args.mme_apn;
  act_def_eps_bearer_context_req.proc_transaction_id = ue_ctx->procedure_transaction_id; //FIXME

  //Set DNS server
  act_def_eps_bearer_context_req.protocol_cnfg_opts_present = true;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.N_opts = 1;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].id = 0x0d;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].len = 4;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].contents[0] = 8;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].contents[1] = 8;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].contents[2] = 8;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].contents[3] = 8;

  //Make sure all unused options are set to false
  act_def_eps_bearer_context_req.negotiated_qos_present = false;
  act_def_eps_bearer_context_req.llc_sapi_present = false;
  act_def_eps_bearer_context_req.radio_prio_present = false;
  act_def_eps_bearer_context_req.packet_flow_id_present = false;
  act_def_eps_bearer_context_req.apn_ambr_present = false;
  act_def_eps_bearer_context_req.esm_cause_present = false;

  uint8_t sec_hdr_type =2;
  ue_ctx->security_ctxt.dl_nas_count++;
  liblte_mme_pack_activate_default_eps_bearer_context_request_msg(&act_def_eps_bearer_context_req, &attach_accept.esm_msg);
  liblte_mme_pack_attach_accept_msg(&attach_accept, sec_hdr_type, ue_ctx->security_ctxt.dl_nas_count, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  //Integrity protect NAS message
  uint8_t mac[4];
  srslte::security_128_eia1 (&ue_ctx->security_ctxt.k_nas_int[16],
                             ue_ctx->security_ctxt.dl_nas_count,
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = mme_ue_s1ap_id;//FIXME Change name
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
s1ap_nas_transport::pack_emm_information(srslte::byte_buffer_t *reply_msg, uint32_t enb_ue_s1ap_id, uint32_t mme_ue_s1ap_id)
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

  LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT emm_info;
  emm_info.full_net_name_present = true;
  emm_info.full_net_name.name = std::string("srsLTE");
  emm_info.full_net_name.add_ci = LIBLTE_MME_ADD_CI_DONT_ADD;
  emm_info.short_net_name_present = true;
  emm_info.short_net_name.name = std::string("srsLTE");
  emm_info.short_net_name.add_ci = LIBLTE_MME_ADD_CI_DONT_ADD;

  emm_info.local_time_zone_present = false;
  emm_info.utc_and_local_time_zone_present = false;
  emm_info.net_dst_present = false;

  //Integrity check
  ue_ctx_t * ue_ctx = m_s1ap->find_ue_ctx(mme_ue_s1ap_id);
  if(ue_ctx == NULL)
  {
    return false;
  }
  uint8_t sec_hdr_type =2;
  ue_ctx->security_ctxt.dl_nas_count++;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_emm_information_msg(&emm_info, sec_hdr_type, ue_ctx->security_ctxt.dl_nas_count, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Identity Request\n");
    m_s1ap_log->console("Error packing Identity REquest\n");
    return false;
  }

  uint8_t mac[4];
  srslte::security_128_eia1 (&ue_ctx->security_ctxt.k_nas_int[16],
                             ue_ctx->security_ctxt.dl_nas_count,
                             0,
                             SECURITY_DIRECTION_DOWNLINK,
                             &nas_buffer->msg[5],
                             nas_buffer->N_bytes - 5,
                             mac
                             );

  memcpy(&nas_buffer->msg[1],mac,4);


  m_s1ap_log->info("Packed \n"); 
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
