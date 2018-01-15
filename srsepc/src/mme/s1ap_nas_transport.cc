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
s1ap_nas_transport::init(void)
{
  m_s1ap = s1ap::get_instance();
  m_s1ap_log = m_s1ap->m_s1ap_log;
  m_pool = srslte::byte_buffer_pool::get_instance();

  m_hss = hss::get_instance();
  m_mme_gtpc = mme_gtpc::get_instance();
}


bool 
s1ap_nas_transport::handle_initial_ue_message(LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT *init_ue, struct sctp_sndrcvinfo *enb_sri, srslte::byte_buffer_t *reply_buffer, bool *reply_flag)
{
  LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT attach_req;
  LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT pdn_con_req;

  uint64_t    imsi = 0;
  uint8_t     k_asme[32];
  uint8_t     autn[16]; 
  uint8_t     rand[6];
  uint8_t     xres[8];

  ue_ctx_t ue_ctx;

  m_s1ap_log->console("Received Initial UE Message.\n");
  m_s1ap_log->info("Received Initial UE Message.\n");

  //Get info from initial UE message
  ue_ctx.enb_ue_s1ap_id = init_ue->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID;

  //Log unhandled Initial UE message IEs
  log_unhandled_initial_ue_message_ies(init_ue);

  //Get NAS Attach Request and PDN connectivity request messages
  if(!unpack_initial_ue_message(init_ue, &attach_req,&pdn_con_req))
  {
    //Could not decode the attach request and the PDN connectivity request.
    m_s1ap_log->error("Could not unpack NAS Attach Request and PDN connectivity request.\n");
    return false;
  }

  //Create basic UE Ctx
  ue_ctx_t *ue_ctx_ptr = &ue_ctx;
  ue_ctx.imsi = 0;
  ue_ctx.mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();
  //Get UE network capabilities
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
  m_s1ap_log->console("EPS Bearer id: %d\n", eps_bearer_id);
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


  //Get UE Identity
  if(attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI)
  {
    //IMSI style attach
    imsi = 0;
    for(int i=0;i<=14;i++){
      imsi  += attach_req.eps_mobile_id.imsi[i]*std::pow(10,14-i);
    }

  }
  else if(attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI)
  {
    //GUTI style attach
    m_s1ap_log->console("Received GUTI-style attach request\n");
    uint32_t m_tmsi = attach_req.eps_mobile_id.guti.m_tmsi;
    std::map<uint32_t,uint32_t>::iterator it = m_s1ap->m_tmsi_to_s1ap_id.find(m_tmsi);
    if(it == m_s1ap->m_tmsi_to_s1ap_id.end())
    {
      //Could not find IMSI from M-TMSI, send Id request
      m_s1ap_log->console("Could not find M-TMSI in attach request. Sending ID request\n");
      m_s1ap_log->info("Could not find M-TMSI in attach request. Sending Id Request\n");
      ue_ctx.mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();
      m_s1ap->add_new_ue_ctx(ue_ctx);
      pack_identity_request(reply_buffer, ue_ctx.mme_ue_s1ap_id, ue_ctx.enb_ue_s1ap_id);
      *reply_flag = true;
      return true;
    }
    m_s1ap_log->console("Found M-TMSI: %d\n",m_tmsi);
    ue_ctx_ptr = m_s1ap->find_ue_ctx(it->second);
    if(ue_ctx_ptr!=NULL)
    {
      m_s1ap_log->console("Found UE context. IMSI: %015lu\n",ue_ctx_ptr->imsi);
      imsi = ue_ctx_ptr->imsi;
    }
    else
    {
      m_s1ap_log->error("Found M-TMSI but could not find UE context\n");
      return false;
    }
  }
  m_s1ap_log->console("Attach request from IMSI: %015lu\n", imsi);
  m_s1ap_log->info("Attach request from IMSI: %015lu\n", imsi);

 
  //Get Authentication Vectors from HSS
  if(!m_hss->gen_auth_info_answer(imsi, ue_ctx_ptr->security_ctxt.k_asme, autn, rand, ue_ctx_ptr->security_ctxt.xres))
  {
    m_s1ap_log->console("User not found. IMSI %015lu\n",imsi);
    m_s1ap_log->info("User not found. IMSI %015lu\n",imsi);
    return false;
  }

  //Save UE context
  //ue_ctx.imsi = imsi;
  if(attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI)
  {
    //IMSI attach requeires new UE ctxt
    ue_ctx.mme_ue_s1ap_id = m_s1ap->get_next_mme_ue_s1ap_id();
    m_s1ap->add_new_ue_ctx(ue_ctx);
  }
  //Pack NAS Authentication Request in Downlink NAS Transport msg
  pack_authentication_request(reply_buffer, ue_ctx_ptr->enb_ue_s1ap_id, ue_ctx_ptr->mme_ue_s1ap_id, autn, rand);

  //Send reply to eNB
  *reply_flag = true;
   m_s1ap_log->info("DL NAS: Sent Athentication Request\n");
  //TODO Start T3460 Timer!
  return true;
}

bool
s1ap_nas_transport::handle_uplink_nas_transport(LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT *ul_xport, struct sctp_sndrcvinfo *enb_sri, srslte::byte_buffer_t *reply_buffer, bool *reply_flag)
{

  bool     ue_valid = true;
  uint32_t enb_ue_s1ap_id = ul_xport->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID;
  uint32_t mme_ue_s1ap_id = ul_xport->MME_UE_S1AP_ID.MME_UE_S1AP_ID;

  m_s1ap_log->console("Received Uplink NAS Transport message.\n");
  m_s1ap_log->info("Received Uplink NAS Transport message.\n");


  ue_ctx_t *ue_ctx = m_s1ap->find_ue_ctx(mme_ue_s1ap_id);
  if(ue_ctx == NULL)
  {
    //TODO UE not registered, send error message.
    m_s1ap_log->warning("Could not find UE. MME-UE S1AP id: %lu\n",mme_ue_s1ap_id);
    return false;
  }

  m_s1ap_log->debug("Found UE. MME-UE S1AP id: %lu\n",mme_ue_s1ap_id);

  //Get NAS message type
  uint8_t pd, msg_type;
  srslte::byte_buffer_t *nas_msg = m_pool->allocate();

  memcpy(nas_msg->msg, &ul_xport->NAS_PDU.buffer, ul_xport->NAS_PDU.n_octets);
  nas_msg->N_bytes = ul_xport->NAS_PDU.n_octets;
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &pd, &msg_type);

  switch (msg_type) {
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_RESPONSE:
      handle_nas_authentication_response(nas_msg, ue_ctx, reply_buffer, reply_flag);
      m_s1ap_log->info("UL NAS: Received Authentication Response\n");
      break;
    case  LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMPLETE:
      m_s1ap_log->info("UL NAS: Received Security Mode Complete\n");
      handle_nas_security_mode_complete(nas_msg, ue_ctx, reply_buffer, reply_flag);
      return true; //no need for reply. FIXME this should be better structured...
      break;
    case  LIBLTE_MME_MSG_TYPE_ATTACH_COMPLETE:
      m_s1ap_log->info("UL NAS: Received Attach Complete\n");
      handle_nas_attach_complete(nas_msg, ue_ctx, reply_buffer, reply_flag);
      ue_ctx->security_ctxt.ul_nas_count++;
      return true; //no need for reply. FIXME this should be better structured...
    break;
    case LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_RESPONSE:
      m_s1ap_log->info("UL NAS: Received ESM Information Response\n");
      handle_esm_information_response(nas_msg, ue_ctx, reply_buffer, reply_flag);
      ue_ctx->security_ctxt.ul_nas_count++;
      return true;
    case LIBLTE_MME_MSG_TYPE_IDENTITY_RESPONSE:
      m_s1ap_log->info("UL NAS: Received ID Response\n");
      handle_identity_response(nas_msg, ue_ctx, reply_buffer, reply_flag);
      //ue_ctx->security_ctxt.ul_nas_count++;
    return true;
    default:
      m_s1ap_log->warning("Unhandled NAS message 0x%x\n", msg_type );
      m_s1ap_log->console("Unhandled NAS message 0x%x\n", msg_type );
      return false; //FIXME (nas_msg deallocate needs to be called)
  }

  if(*reply_flag == true)
  {
    m_s1ap_log->info("DL NAS: Sent Downlink NAS message\n");
    m_s1ap_log->console("DL NAS: Sent Downlink NAs Message\n");
  }
  m_pool->deallocate(nas_msg);

  return true;
}

bool
s1ap_nas_transport::handle_nas_authentication_response(srslte::byte_buffer_t *nas_msg, ue_ctx_t *ue_ctx, srslte::byte_buffer_t *reply_buffer, bool* reply_flag)
{

  LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT auth_resp;
  bool ue_valid=true;

  //Get NAS authentication response
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_authentication_response_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &auth_resp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

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

    m_s1ap_log->console("UE Authentication Rejected. IMSI: %lu\n", ue_ctx->imsi);
    m_s1ap_log->warning("UE Authentication Rejected. IMSI: %lu\n", ue_ctx->imsi);
    //Send back Athentication Reject
    pack_authentication_reject(reply_buffer, ue_ctx->enb_ue_s1ap_id, ue_ctx->mme_ue_s1ap_id);
    *reply_flag = true;
    return false;
  }
  else
  {
    m_s1ap_log->console("UE Authentication Accepted. IMSI: %lu\n", ue_ctx->imsi);
    m_s1ap_log->info("UE Authentication Accepted. IMSI: %lu\n", ue_ctx->imsi);
    //Send Security Mode Command
    pack_security_mode_command(reply_buffer, ue_ctx);
    *reply_flag = true;
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

  m_s1ap_log->info("Received Security Mode Command Complete. IMSI: %lu\n", ue_ctx->imsi);
  m_s1ap_log->console("Received Security Mode Command Complete. IMSI: %lu\n", ue_ctx->imsi);
  if(ue_ctx->eit == true)
  {
    pack_esm_information_request(reply_buffer, ue_ctx);
    m_s1ap_log->console("Sending ESM information request\n");
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

  m_s1ap_log->info_hex(nas_msg->msg, nas_msg->N_bytes, "NAS Attach complte");

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
  m_s1ap_log->console("Unpacked Activavate Default EPS Bearer message. EPS Bearer id %d\n",act_bearer.eps_bearer_id);
  //ue_ctx->erabs_ctx[act_bearer->eps_bearer_id].enb_fteid;
  if(act_bearer.eps_bearer_id < 5 || act_bearer.eps_bearer_id > 16)
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

  //FIXME The packging of GTP-C messages is not ready.
  //This means that GTP-U tunnels are created with function calls, as opposed to GTP-C.
  m_mme_gtpc->send_create_session_request(ue_ctx->imsi, ue_ctx->mme_ue_s1ap_id);
  return true;
}

bool
s1ap_nas_transport::handle_identity_response(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{
  uint8_t     autn[16]; 
  uint8_t     rand[6];
  uint8_t     xres[8];

  LIBLTE_MME_ID_RESPONSE_MSG_STRUCT id_resp;
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_identity_response_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &id_resp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  uint64_t imsi = 0;
  for(int i=0;i<=14;i++){
    imsi  += id_resp.mobile_id.imsi[i]*std::pow(10,14-i);
  }

  m_s1ap_log->info("Id Response IMSI: %015lu\n", imsi);
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
   m_s1ap_log->info("DL NAS: Sent Athentication Request\n");
  //TODO Start T3460 Timer! 

  return true;
}
/*Packing/Unpacking helper functions*/
bool 
s1ap_nas_transport::unpack_initial_ue_message(LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT *init_ue,
                                              LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT *attach_req,
                                              LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT *pdn_con_req )
{

  /*Get NAS Attach Request Message*/
  uint8_t pd, msg_type;
  srslte::byte_buffer_t *nas_msg = m_pool->allocate();
  
  memcpy(nas_msg->msg, &init_ue->NAS_PDU.buffer, init_ue->NAS_PDU.n_octets);
  nas_msg->N_bytes = init_ue->NAS_PDU.n_octets;
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &pd, &msg_type);
  
  if(msg_type!=LIBLTE_MME_MSG_TYPE_ATTACH_REQUEST){
    m_s1ap_log->error("Unhandled NAS message within the Initial UE message\n");
    return false;
  }

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_attach_request_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, attach_req);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS attach request. Error: %s\n", liblte_error_text[err]);
    return false;
  }
  /*Log unhandled Attach Request IEs*/
  log_unhandled_attach_request_ies(attach_req);

  /*Get PDN Connectivity Request*/
  err = liblte_mme_unpack_pdn_connectivity_request_msg(&attach_req->esm_msg, pdn_con_req);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS PDN Connectivity Request. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  if(pdn_con_req->pdn_type == LIBLTE_MME_PDN_TYPE_IPV6)
  {
    m_s1ap_log->error("PDN Connectivity Request: Only IPv4 connectivity supported.\n");
    return false;
  }
  if(pdn_con_req->request_type != LIBLTE_MME_REQUEST_TYPE_INITIAL_REQUEST)
  {
    m_s1ap_log->error("PDN Connectivity Request: Only Initial Request supported.\n");
    return false;
  }
  /*Log unhandled PDN connectivity request IEs*/
  log_unhandled_pdn_con_request_ies(pdn_con_req);

  m_pool->deallocate(nas_msg);
  return true;
}

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
    m_s1ap_log->error("Error packing Athentication Request\n");
    m_s1ap_log->console("Error packing Athentication Request\n");
    return false;
  }

  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Athentication Request\n");
    m_s1ap_log->console("Error packing Athentication Request\n");
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
    m_s1ap_log->error("Error packing Athentication Reject\n");
    m_s1ap_log->console("Error packing Athentication Reject\n");
    return false;
  }

  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Dw NAS Transport: Athentication Reject\n");
    m_s1ap_log->console("Error packing Downlink NAS Transport: Athentication Reject\n");
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
  m_s1ap_log->console("Sending Security Mode command to MME-UE S1AP Id %d\n", ue_ctx->mme_ue_s1ap_id);

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
    m_s1ap_log->console("Error packing Athentication Request\n");
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
    m_s1ap_log->console("Error packing Athentication Request\n");
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
  esm_info_req.eps_bearer_id=0;
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
    m_s1ap_log->error("Error packing Dw NAS Transport: Athentication Reject\n");
    m_s1ap_log->console("Error packing Downlink NAS Transport: Athentication Reject\n");
    return false;
  }

  m_pool->deallocate(nas_buffer);
  return true;
}

bool
s1ap_nas_transport::pack_attach_accept(ue_ctx_t *ue_ctx, LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT *erab_ctxt, struct srslte::gtpc_pdn_address_allocation_ie *paa, srslte::byte_buffer_t *nas_buffer) {
  LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT attach_accept;
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT act_def_eps_bearer_context_req;
  bzero(&act_def_eps_bearer_context_req,sizeof(LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT));

  m_s1ap_log->info("Packing Attach Accept\n");

  //Attach accept
  attach_accept.eps_attach_result = LIBLTE_MME_EPS_ATTACH_RESULT_EPS_ONLY;
  //Mandatory
  //FIXME: Set t3412 from config
  attach_accept.t3412.unit = LIBLTE_MME_GPRS_TIMER_UNIT_1_MINUTE;   // GPRS 1 minute unit
  attach_accept.t3412.value = 30;                                    // 30 minute periodic timer
  //FIXME: Set tai_list from config
  attach_accept.tai_list.N_tais = 1;
  attach_accept.tai_list.tai[0].mcc = m_s1ap->m_s1ap_args.mcc;
  attach_accept.tai_list.tai[0].mnc = m_s1ap->m_s1ap_args.mnc;
  attach_accept.tai_list.tai[0].tac = m_s1ap->m_s1ap_args.tac;

  //Allocate a GUTI ot the UE
  attach_accept.guti_present=true;
  attach_accept.guti.type_of_id = 6; //110 -> GUTI
  attach_accept.guti.guti.mcc = m_s1ap->m_s1ap_args.mcc;
  attach_accept.guti.guti.mnc = m_s1ap->m_s1ap_args.mnc;
  attach_accept.guti.guti.mme_group_id = 0x0001;
  attach_accept.guti.guti.mme_code = 0x1a;
  attach_accept.guti.guti.m_tmsi = m_s1ap->allocate_m_tmsi(ue_ctx->mme_ue_s1ap_id);

  /*
  typedef struct{
    uint32 m_tmsi;
    uint16 mcc;
    uint16 mnc;
    uint16 mme_group_id;
    uint8  mme_code;
  }LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT;
  typedef struct{
    LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT guti;
    uint8                                type_of_id;
    uint8                                imsi[15];
    uint8                                imei[15];
  }LIBLTE_MME_EPS_MOBILE_ID_STRUCT;*/

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
  printf("%d\n",act_def_eps_bearer_context_req.eps_bearer_id);
  act_def_eps_bearer_context_req.transaction_id_present = false;
  //set eps_qos
  act_def_eps_bearer_context_req.eps_qos.qci =  erab_ctxt->e_RABlevelQoSParameters.qCI.QCI;
  act_def_eps_bearer_context_req.eps_qos.mbr_ul = 254; //FIXME
  act_def_eps_bearer_context_req.eps_qos.mbr_dl = 254; //FIXME
  act_def_eps_bearer_context_req.eps_qos.mbr_ul_ext = 250; //FIXME
  act_def_eps_bearer_context_req.eps_qos.mbr_dl_ext = 250; //FIXME check
  //set apn
  //act_def_eps_bearer_context_req.apn
  std::string apn("test123");
  act_def_eps_bearer_context_req.apn.apn = apn; //FIXME
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
    m_s1ap_log->error("Error packing Dw NAS Transport: Athentication Reject\n");
    m_s1ap_log->console("Error packing Downlink NAS Transport: Athentication Reject\n");
    return false;
  } 

  m_pool->deallocate(nas_buffer);
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
