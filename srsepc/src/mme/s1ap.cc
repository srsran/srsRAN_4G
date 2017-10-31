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

#include "srslte/common/bcd_helpers.h"
#include "mme/s1ap.h"

namespace srsepc{

s1ap::s1ap():
  m_s1mme(-1)
{
}

s1ap::~s1ap()
{
}

int
s1ap::init(s1ap_args_t s1ap_args, srslte::log *s1ap_log)
{
  
  m_s1ap_args = s1ap_args;

  srslte::s1ap_mccmnc_to_plmn(s1ap_args.mcc, s1ap_args.mnc, &m_plmn);

  m_s1ap_log = s1ap_log;

  m_s1mme = enb_listen();

  m_hss = hss::get_instance(); 

  m_pool = srslte::byte_buffer_pool::get_instance();
 
  return 0;
}

void
s1ap::stop()
{
  if (m_s1mme != -1){
    close(m_s1mme);
  }
  std::map<uint16_t,enb_ctx_t*>::iterator it = m_active_enbs.begin();
  while(it!=m_active_enbs.end())
  {
    print_enb_ctx_info(*it->second);
    delete it->second;
    m_active_enbs.erase(it++);
  }
  return;
}

int
s1ap::get_s1_mme()
{
  return m_s1mme;
}

int
s1ap::enb_listen()
{
  /*This function sets up the SCTP socket for eNBs to connect to*/
  int sock_fd, err;
  struct sockaddr_in s1mme_addr;
  struct sctp_event_subscribe evnts;

  m_s1ap_log->console("Initializing S1-MME\n");
  sock_fd = socket (AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  if (sock_fd == -1){
    m_s1ap_log->console("Could not create SCTP socket\n"); 
    return -1;
  }

  //Sets the data_io_event to be able to use sendrecv_info
  //Subscribes to the SCTP_SHUTDOWN event, to handle graceful shutdown
  bzero (&evnts, sizeof (evnts)) ;
  evnts.sctp_data_io_event = 1;
  evnts.sctp_shutdown_event=1;
  if(setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof (evnts))){
    m_s1ap_log->console("Subscribing to sctp_data_io_events failed\n");
    return -1;
  }

  //S1-MME bind
  bzero(&s1mme_addr, sizeof(s1mme_addr));
  s1mme_addr.sin_family = AF_INET; 
  inet_pton(AF_INET, m_s1ap_args.mme_bind_addr.c_str(), &(s1mme_addr.sin_addr) );
  s1mme_addr.sin_port = htons(S1MME_PORT);
  err = bind(sock_fd, (struct sockaddr*) &s1mme_addr, sizeof (s1mme_addr));
  if (err != 0){
    m_s1ap_log->error("Error binding SCTP socket\n");
    m_s1ap_log->console("Error binding SCTP socket\n");
    return -1;
  }

  //Listen for connections
  err = listen(sock_fd,SOMAXCONN);
  if (err != 0){
    m_s1ap_log->error("Error in SCTP socket listen\n");
    m_s1ap_log->console("Error in SCTP socket listen\n");
    return -1;
  }

  return sock_fd;
}

bool
s1ap::handle_s1ap_rx_pdu(srslte::byte_buffer_t *pdu, struct sctp_sndrcvinfo *enb_sri) 
{
  LIBLTE_S1AP_S1AP_PDU_STRUCT rx_pdu;

  if(liblte_s1ap_unpack_s1ap_pdu((LIBLTE_BYTE_MSG_STRUCT*)pdu, &rx_pdu) != LIBLTE_SUCCESS) {
    m_s1ap_log->error("Failed to unpack received PDU\n");
    return false;
  }

  switch(rx_pdu.choice_type) {
  case LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE:
    m_s1ap_log->console("Received initiating PDU\n");
    return handle_initiating_message(&rx_pdu.choice.initiatingMessage, enb_sri);
    break;
  case LIBLTE_S1AP_S1AP_PDU_CHOICE_SUCCESSFULOUTCOME:
    m_s1ap_log->console("Received Succeseful Outcome PDU\n");
    return true;//handle_successfuloutcome(&rx_pdu.choice.successfulOutcome);
    break;
  case LIBLTE_S1AP_S1AP_PDU_CHOICE_UNSUCCESSFULOUTCOME:
    m_s1ap_log->console("Received Unsucceseful Outcome PDU\n");
    return true;//handle_unsuccessfuloutcome(&rx_pdu.choice.unsuccessfulOutcome);
    break;
  default:
    m_s1ap_log->console("Unhandled PDU type %d\n", rx_pdu.choice_type);
    return false;
  }

  return true;

}

bool 
s1ap::handle_initiating_message(LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *msg,  struct sctp_sndrcvinfo *enb_sri)
{
  switch(msg->choice_type) {
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_S1SETUPREQUEST:
    m_s1ap_log->info("Received S1 Setup Request.\n");
    return handle_s1_setup_request(&msg->choice.S1SetupRequest, enb_sri);
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_INITIALUEMESSAGE:
    m_s1ap_log->info("Received Initial UE Message.\n");
    return handle_initial_ue_message(&msg->choice.InitialUEMessage, enb_sri);
  default:
    m_s1ap_log->error("Unhandled intiating message: %s\n", liblte_s1ap_initiatingmessage_choice_text[msg->choice_type]);
  }
  return true;
}

bool 
s1ap::handle_s1_setup_request(LIBLTE_S1AP_MESSAGE_S1SETUPREQUEST_STRUCT *msg, struct sctp_sndrcvinfo *enb_sri)
{
  
  std::string mnc_str, mcc_str;
  enb_ctx_t enb_ctx;
  srslte::byte_buffer_t reply_msg;
  LIBLTE_S1AP_S1AP_PDU_STRUCT reply_pdu;

  if(!m_s1ap_mngmt_proc.unpack_s1_setup_request(msg, &enb_ctx))
  {
    m_s1ap_log->error("Malformed S1 Setup Request\n");
    return false;
  }

  //Log S1 Setup Request Info
  print_enb_ctx_info(enb_ctx);
  
  //Check matching PLMNs  
  if(enb_ctx.plmn!=m_plmn){
    m_s1ap_log->console("S1 Setup Failure - Unkown PLMN\n");
    m_s1ap_log->info("S1 Setup Failure - Unkown PLMN\n");
    m_s1ap_mngmt_proc.pack_s1_setup_failure(LIBLTE_S1AP_CAUSEMISC_UNKNOWN_PLMN,&reply_msg);
  }
  else{
    std::map<uint16_t,enb_ctx_t*>::iterator it = m_active_enbs.find(enb_ctx.enb_id);
    if(it != m_active_enbs.end())
    {
      //eNB already registered
      //TODO replace enb_ctx
    }
    else
    {
      //new eNB
      enb_ctx_t *enb_ptr = new enb_ctx_t;//TODO use buffer pool here?
      memcpy(enb_ptr,&enb_ctx,sizeof(enb_ctx));
      m_active_enbs.insert(std::pair<uint16_t,enb_ctx_t*>(enb_ptr->enb_id,enb_ptr));
    }
        
    //m_active_enbs.insert(std::pair<uint16_t,enb_ctx_t>(enb_ctx.enb_id,enb_ctx));
    m_s1ap_mngmt_proc.pack_s1_setup_response(m_s1ap_args, &reply_msg);
    m_s1ap_log->console("S1 Setup Response\n");
    m_s1ap_log->info("S1 Setup Response\n");
  }
  
  //Send Reply to eNB
  ssize_t n_sent = sctp_send(m_s1mme,reply_msg.msg, reply_msg.N_bytes, enb_sri, 0);
  if(n_sent == -1)
  {
    m_s1ap_log->console("Failed to send S1 Setup Setup Reply");
    return false;
  }
  
  return true;
  
}

bool 
s1ap::handle_initial_ue_message(LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT *msg, struct sctp_sndrcvinfo *enb_sri)
{
  m_s1ap_log->console("Received Initial UE Message\n");
  m_s1ap_log->info("Received Initial UE Message\n");
  
  uint8_t     amf[2];  // 3GPP 33.102 v10.0.0 Annex H
  uint8_t     op[16];
  uint8_t     k[16];
  uint64_t    imsi;

  LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT attach_req;
  LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT pdn_con_req;

  /*Get NAS Attach Request Message*/
  uint8_t pd, msg_type;

  srslte::byte_buffer_t *nas_msg = m_pool->allocate();
  memcpy(nas_msg->msg, &msg->NAS_PDU.buffer, msg->NAS_PDU.n_octets);
  nas_msg->N_bytes = msg->NAS_PDU.n_octets;
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &pd, &msg_type);
  
  if(msg_type!=LIBLTE_MME_MSG_TYPE_ATTACH_REQUEST){
    m_s1ap_log->error("Unhandled NAS message within the Initial UE message\n");
    return false;
  }

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_attach_request_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &attach_req);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->console("Error unpacking NAS attach request. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  m_s1ap_log->console("Unpacked NAS attach request.\n");


  if(attach_req.eps_mobile_id.type_of_id!=LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI){
    m_s1ap_log->warning("NAS Attach Request: Unhandle UE Id Type");
  }
  else{
    imsi = 0;
    for(int i=14;i>=0;i--)
    {
      imsi  *=10;
      imsi  += attach_req.eps_mobile_id.imsi[i];
    }
    m_s1ap_log->console("IMSI: %d", imsi);
  }
  
  if(attach_req.old_p_tmsi_signature_present){}
  if(attach_req.additional_guti_present){}
  if(attach_req.last_visited_registered_tai_present){}
  if(attach_req.drx_param_present){}
  if(attach_req.ms_network_cap_present){}
  if(attach_req.old_lai_present){}
  if(attach_req.tmsi_status_present){}
  if(attach_req.ms_cm2_present){}
  if(attach_req.ms_cm3_present){}
  if(attach_req.supported_codecs_present){}
  if(attach_req.additional_update_type_present){}
  if(attach_req.voice_domain_pref_and_ue_usage_setting_present){}
  if(attach_req.device_properties_present){}
  if(attach_req.old_guti_type_present){}
    

  /*Handle PDN Connctivity Request*/
  liblte_mme_unpack_pdn_connectivity_request_msg(&attach_req.esm_msg, &pdn_con_req);
  
  //pdn_con_req.eps_bearer_id
  //pdn_con_req.proc_transaction_id = 0x01; // First transaction ID
  //pdn_con_req.pdn_type = LIBLTE_MME_PDN_TYPE_IPV4;
  //pdn_con_req.request_type = LIBLTE_MME_REQUEST_TYPE_INITIAL_REQUEST;

  // Set the optional flags
  if(pdn_con_req.esm_info_transfer_flag_present){}
  if(pdn_con_req.apn_present){}
  if(pdn_con_req.protocol_cnfg_opts_present){}
  if(pdn_con_req.device_properties_present){}



  /*Log unhandled IEs*/
  if(msg->S_TMSI_present){
    m_s1ap_log->warning("S-TMSI present, but not handled.");
  }
  if(msg->CSG_Id_present){
    m_s1ap_log->warning("S-TMSI present, but not handled.");
  }
  if(msg->GUMMEI_ID_present){
    m_s1ap_log->warning("GUMMEI ID present, but not handled.");
  }
  if(msg->CellAccessMode_present){
    m_s1ap_log->warning("Cell Access Mode present, but not handled.");
  }
  if(msg->GW_TransportLayerAddress_present){
    m_s1ap_log->warning("GW Transport Layer present, but not handled.");
  }
  if(msg->GW_TransportLayerAddress_present){
    m_s1ap_log->warning("GW Transport Layer present, but not handled.");
  }
  if(msg->RelayNode_Indicator_present){
    m_s1ap_log->warning("Relay Node Indicator present, but not handled.");
  }
  if(msg->GUMMEIType_present){
    m_s1ap_log->warning("GUMMEI Type present, but not handled.");
  }
  if(msg->Tunnel_Information_for_BBF_present){
    m_s1ap_log->warning("Tunnel Information for BBF present, but not handled.");
  }
  if(msg->SIPTO_L_GW_TransportLayerAddress_present){
    m_s1ap_log->warning("SIPTO GW Transport Layer Address present, but not handled.");
  }
  if(msg->LHN_ID_present){
    m_s1ap_log->warning("LHN Id present, but not handled.");
  }
 
  if(!m_hss->get_k_amf_op(imsi, k, amf, op))
  {
    m_s1ap_log->info("User %d not found",imsi);
  }

  

  /*
  typedef struct{
    LIBLTE_MME_NAS_KEY_SET_ID_STRUCT                         nas_ksi;
    LIBLTE_MME_EPS_MOBILE_ID_STRUCT                          eps_mobile_id;
    LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT                  ue_network_cap;
    LIBLTE_BYTE_MSG_STRUCT                                   esm_msg;
    LIBLTE_MME_EPS_MOBILE_ID_STRUCT                          additional_guti;
    LIBLTE_MME_TRACKING_AREA_ID_STRUCT                       last_visited_registered_tai;
    LIBLTE_MME_DRX_PARAMETER_STRUCT                          drx_param;
    LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT                  ms_network_cap;
    LIBLTE_MME_LOCATION_AREA_ID_STRUCT                       old_lai;
    LIBLTE_MME_MOBILE_STATION_CLASSMARK_2_STRUCT             ms_cm2;
    LIBLTE_MME_MOBILE_STATION_CLASSMARK_3_STRUCT             ms_cm3;
    LIBLTE_MME_SUPPORTED_CODEC_LIST_STRUCT                   supported_codecs;
    LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_STRUCT voice_domain_pref_and_ue_usage_setting;
    LIBLTE_MME_TMSI_STATUS_ENUM                              tmsi_status;
    LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_ENUM                   additional_update_type;
    LIBLTE_MME_DEVICE_PROPERTIES_ENUM                        device_properties;
    LIBLTE_MME_GUTI_TYPE_ENUM                                old_guti_type;
    uint32                                                   old_p_tmsi_signature;
    uint8                                                    eps_attach_type;
    bool                                                     old_p_tmsi_signature_present;
    bool                                                     additional_guti_present;
    bool                                                     last_visited_registered_tai_present;
    bool                                                     drx_param_present;
    bool                                                     ms_network_cap_present;
    bool                                                     old_lai_present;
    bool                                                     tmsi_status_present;
    bool                                                     ms_cm2_present;
    bool                                                     ms_cm3_present;
    bool                                                     supported_codecs_present;
    bool                                                     additional_update_type_present;
    bool                                                     voice_domain_pref_and_ue_usage_setting_present; 
    bool                                                     device_properties_present;
    bool                                                     old_guti_type_present;
  }LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT;
  */
  /*
  typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_NAS_PDU_STRUCT                                   NAS_PDU;
  LIBLTE_S1AP_TAI_STRUCT                                       TAI;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                EUTRAN_CGI;
  LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM_EXT                 RRC_Establishment_Cause;
  LIBLTE_S1AP_S_TMSI_STRUCT                                    S_TMSI;
  bool                                                         S_TMSI_present;
  LIBLTE_S1AP_CSG_ID_STRUCT                                    CSG_Id;
  bool                                                         CSG_Id_present;
  LIBLTE_S1AP_GUMMEI_STRUCT                                    GUMMEI_ID;
  bool                                                         GUMMEI_ID_present;
  LIBLTE_S1AP_CELLACCESSMODE_ENUM_EXT                          CellAccessMode;
  bool                                                         CellAccessMode_present;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     GW_TransportLayerAddress;
  bool                                                         GW_TransportLayerAddress_present;
  LIBLTE_S1AP_RELAYNODE_INDICATOR_ENUM_EXT                     RelayNode_Indicator;
  bool                                                         RelayNode_Indicator_present;
  LIBLTE_S1AP_GUMMEITYPE_ENUM_EXT                              GUMMEIType;
  bool                                                         GUMMEIType_present;
  LIBLTE_S1AP_TUNNELINFORMATION_STRUCT                         Tunnel_Information_for_BBF;
  bool                                                         Tunnel_Information_for_BBF_present;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     SIPTO_L_GW_TransportLayerAddress;
  bool                                                         SIPTO_L_GW_TransportLayerAddress_present;
  LIBLTE_S1AP_LHN_ID_STRUCT                                    LHN_ID;
  bool                                                         LHN_ID_present;
  }LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT;  
  */ 
  //Send Reply to eNB
  //ssize_t n_sent = sctp_send(m_s1mme,reply_msg.msg, reply_msg.N_bytes, enb_sri, 0);
  //if(n_sent == -1)
  //{
  //  m_s1ap_log->console("Failed to send S1 Setup Setup Reply");
  //  return false;
  //}
  
  return true;
  
}



void
s1ap::print_enb_ctx_info(const enb_ctx_t &enb_ctx)
{
  std::string mnc_str, mcc_str;

  if(enb_ctx.enb_name_present)
  {
    m_s1ap_log->console("S1 Setup Request - eNB Name: %s, eNB id: 0x%x\n", enb_ctx.enb_name, enb_ctx.enb_id);
    m_s1ap_log->info("S1 Setup Request - eNB Name: %s, eNB id: 0x%x\n", enb_ctx.enb_name, enb_ctx.enb_id);
  }
  else
  {
    m_s1ap_log->console("S1 Setup Request - eNB Id 0x%x\n", enb_ctx.enb_id);
    m_s1ap_log->info("S1 Setup request - eNB Id 0x%x\n", enb_ctx.enb_id);
  }
  srslte::mcc_to_string(enb_ctx.mcc, &mcc_str);
  srslte::mnc_to_string(enb_ctx.mnc, &mnc_str);
  m_s1ap_log->info("S1 Setup Request - MCC:%s, MNC:%s, PLMN: %d\n", mcc_str.c_str(), mnc_str.c_str(), enb_ctx.plmn);
  m_s1ap_log->console("S1 Setup Request - MCC:%s, MNC:%s, PLMN: %d\n", mcc_str.c_str(), mnc_str.c_str(), enb_ctx.plmn);
  for(int i=0;i<enb_ctx.nof_supported_ta;i++)
  {
    for(int j=0;i<enb_ctx.nof_supported_ta;i++)
    {
      m_s1ap_log->info("S1 Setup Request - TAC %d, B-PLMN %d\n",enb_ctx.tac[i],enb_ctx.bplmns[i][j]);
      m_s1ap_log->console("S1 Setup Request - TAC %d, B-PLMN %d\n",enb_ctx.tac[i],enb_ctx.bplmns[i][j]);
    }
  }
  m_s1ap_log->console("S1 Setup Request - Paging DRX %d\n",enb_ctx.drx);
  return;
}

} //namespace srsepc
