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

s1ap_nas_transport::s1ap_nas_transport()
{
  m_pool = srslte::byte_buffer_pool::get_instance();
  return;
}

s1ap_nas_transport::~s1ap_nas_transport()
{
  return;
}

void
s1ap_nas_transport::set_log(srslte::log *s1ap_log)
{
  m_s1ap_log=s1ap_log;
  return;
}

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

  if(pdn_con_req->pdn_type != LIBLTE_MME_PDN_TYPE_IPV4)
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
    m_s1ap_log->error("Unhandled NAS message within UL NAS Transport message\n");
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

  //FIXME UE security cap not used by srsUE.
  //sm_cmd.ue_security_cap;

  sm_cmd.imeisv_req_present=false;
  sm_cmd.nonce_ue_present=false;
  sm_cmd.nonce_mme_present=false;

  uint8_t  sec_hdr_type=3;
  uint32_t count=0;

  LIBLTE_ERROR_ENUM err = liblte_mme_pack_security_mode_command_msg(&sm_cmd,sec_hdr_type, count,(LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
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
                     count,
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
   
  m_pool->deallocate(nas_buffer);

  return true;
}
  


/*Helper functions*/
void
s1ap_nas_transport::log_unhandled_attach_request_ies(const LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT *attach_req)
{
  if(attach_req->old_p_tmsi_signature_present)
  {
    m_s1ap_log->warning("NAS attach request: Old P-TMSI signature present, but not handled.");
  }
  if(attach_req->additional_guti_present)
  {
    m_s1ap_log->warning("NAS attach request: Aditional GUTI present, but not handled.");
  }
  if(attach_req->last_visited_registered_tai_present)
  {
    m_s1ap_log->warning("NAS attach request: Last visited registered TAI present, but not handled.");
  }
  if(attach_req->drx_param_present)
  {
    m_s1ap_log->warning("NAS attach request: DRX Param present, but not handled.");
  }
  if(attach_req->ms_network_cap_present)
  {
    m_s1ap_log->warning("NAS attach request: MS network cap present, but not handled.");
  }
  if(attach_req->old_lai_present)
  {
    m_s1ap_log->warning("NAS attach request: Old LAI present, but not handled.");
  }
  if(attach_req->tmsi_status_present)
  {
    m_s1ap_log->warning("NAS attach request: TSMI status present, but not handled.");
  }
  if(attach_req->ms_cm2_present)
  {
    m_s1ap_log->warning("NAS attach request: MS CM2 present, but not handled.");
  }
  if(attach_req->ms_cm3_present)
  {
    m_s1ap_log->warning("NAS attach request: MS CM3 present, but not handled.");
  }
  if(attach_req->supported_codecs_present)
  {
    m_s1ap_log->warning("NAS attach request: Supported CODECs present, but not handled.");
  }
  if(attach_req->additional_update_type_present)
  {
    m_s1ap_log->warning("NAS attach request: Additional Update Type present, but not handled.");
  }
  if(attach_req->voice_domain_pref_and_ue_usage_setting_present)
  {
    m_s1ap_log->warning("NAS attach request: Voice domain preference and UE usage setting  present, but not handled.");
  }
  if(attach_req->device_properties_present)
  {
    m_s1ap_log->warning("NAS attach request: Device properties present, but not handled.");
  }
  if(attach_req->old_guti_type_present)
  {
    m_s1ap_log->warning("NAS attach request: Old GUTI type present, but not handled.");
  }
  return;
}

void
s1ap_nas_transport::log_unhandled_pdn_con_request_ies(const LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT *pdn_con_req)
{
  //Handle the optional flags
  if(pdn_con_req->esm_info_transfer_flag_present)
  {
    m_s1ap_log->warning("PDN Connectivity request: ESM info transfer flag properties present, but not handled.");
  }
  if(pdn_con_req->apn_present)
  {
    m_s1ap_log->warning("PDN Connectivity request: APN present, but not handled.");
  }
  if(pdn_con_req->protocol_cnfg_opts_present)
  {
    m_s1ap_log->warning("PDN Connectivity request: Protocol Cnfg options present, but not handled.");
  }
  if(pdn_con_req->device_properties_present)
  {
    m_s1ap_log->warning("PDN Connectivity request: Device properties present, but not handled.");
  }
}


void
s1ap_nas_transport::log_unhandled_initial_ue_message_ies(LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT *init_ue)
{
  if(init_ue->S_TMSI_present){
    m_s1ap_log->warning("S-TMSI present, but not handled.");
  }
  if(init_ue->CSG_Id_present){
    m_s1ap_log->warning("S-TMSI present, but not handled.");
  }
  if(init_ue->GUMMEI_ID_present){
    m_s1ap_log->warning("GUMMEI ID present, but not handled.");
  }
  if(init_ue->CellAccessMode_present){
    m_s1ap_log->warning("Cell Access Mode present, but not handled.");
  } 
  if(init_ue->GW_TransportLayerAddress_present){
    m_s1ap_log->warning("GW Transport Layer present, but not handled.");
  }
  if(init_ue->GW_TransportLayerAddress_present){
    m_s1ap_log->warning("GW Transport Layer present, but not handled.");
  }
  if(init_ue->RelayNode_Indicator_present){
    m_s1ap_log->warning("Relay Node Indicator present, but not handled.");
  }
  if(init_ue->GUMMEIType_present){
    m_s1ap_log->warning("GUMMEI Type present, but not handled.");
  }
  if(init_ue->Tunnel_Information_for_BBF_present){
    m_s1ap_log->warning("Tunnel Information for BBF present, but not handled.");
  }
  if(init_ue->SIPTO_L_GW_TransportLayerAddress_present){
    m_s1ap_log->warning("SIPTO GW Transport Layer Address present, but not handled.");
  }
  if(init_ue->LHN_ID_present){
    m_s1ap_log->warning("LHN Id present, but not handled.");
  }
  return;
}


bool
s1ap_nas_transport::pack_attach_accept(ue_ctx_t *ue_ctx, LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT *erab_ctxt, struct srslte::gtpc_pdn_address_allocation_ie *paa, srslte::byte_buffer_t *nas_buffer) {
  LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT attach_accept;
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT act_def_eps_bearer_context_req;

  m_s1ap_log->info("Packing Attach Accept\n");
  bzero(&attach_accept, sizeof(LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT));
  bzero(&act_def_eps_bearer_context_req, sizeof(LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT));

  //Attach accept
  attach_accept.eps_attach_result = LIBLTE_MME_EPS_ATTACH_RESULT_EPS_ONLY;
  //Mandatory
  //FIXME: Set t3412.unit
  //FIXME: Set tai_list
  //FIXME: Set esm_msg
  act_def_eps_bearer_context_req.pdn_addr.pdn_type = LIBLTE_MME_PDN_TYPE_IPV4;
  memcpy(act_def_eps_bearer_context_req.pdn_addr.addr, &paa->ipv4, 4);
  /*
  act_def_eps_bearer_context_req.pdn_addr.addr[0]  |= ip_addr << 24;
  act_def_eps_bearer_context_req.pdn_addr.addr[1]  |= ip_addr << 16;
  act_def_eps_bearer_context_req.pdn_addr.addr[2]  |= ip_addr << 8;
  act_def_eps_bearer_context_req.pdn_addr.addr[3]  |= ip_addr;
  */
  //Activate default EPS bearer context
  act_def_eps_bearer_context_req.eps_bearer_id = erab_ctxt->e_RAB_ID.E_RAB_ID;
  act_def_eps_bearer_context_req.transaction_id_present = false;
  liblte_mme_pack_activate_default_eps_bearer_context_request_msg(&act_def_eps_bearer_context_req, &attach_accept.esm_msg);

    //FIXME: Set the following parameters
//    act_def_eps_bearer_context_req.eps_qos.qci
//    act_def_eps_bearer_context_req.eps_qos.br_present
//    act_def_eps_bearer_context_req.eps_qos.br_ext_present
//    act_def_eps_bearer_context_req.apn.apn
//    act_def_eps_bearer_context_req.negotiated_qos_present
//    act_def_eps_bearer_context_req.llc_sapi_present
//    act_def_eps_bearer_context_req.radio_prio_present
//    act_def_eps_bearer_context_req.packet_flow_id_present
//    act_def_eps_bearer_context_req.apn_ambr_present
//    act_def_eps_bearer_context_req.protocol_cnfg_opts_present
//    act_def_eps_bearer_context_req.connectivity_type_present

    // FIXME: Setup the default EPS bearer context

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
 
  return true;
}

} //namespace srsepc
