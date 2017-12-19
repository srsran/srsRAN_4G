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
#include "srslte/common/bcd_helpers.h"
#include "mme/s1ap.h"
#include "srslte/asn1/gtpc.h"
#include "srslte/common/liblte_security.h"

namespace srsepc{

s1ap*          s1ap::m_instance = NULL;
boost::mutex   s1ap_instance_mutex;

s1ap::s1ap():
  m_s1mme(-1),
  m_next_mme_ue_s1ap_id(1)
{
}

s1ap::~s1ap()
{
}

s1ap*
s1ap::get_instance(void)
{
  boost::mutex::scoped_lock lock(s1ap_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new s1ap();
  }
  return(m_instance);
}

void
s1ap::cleanup(void)
{
  boost::mutex::scoped_lock lock(s1ap_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
}

int
s1ap::init(s1ap_args_t s1ap_args, srslte::log_filter *s1ap_log)
{
  m_pool = srslte::byte_buffer_pool::get_instance();

  m_s1ap_args = s1ap_args;
  srslte::s1ap_mccmnc_to_plmn(s1ap_args.mcc, s1ap_args.mnc, &m_plmn);

  //Init log
  m_s1ap_log = s1ap_log;

  //Init message handlers
  m_s1ap_mngmt_proc = s1ap_mngmt_proc::get_instance(); //Managment procedures (TS ) 
  m_s1ap_mngmt_proc->init();
  m_s1ap_nas_transport = s1ap_nas_transport::get_instance(); //NAS Transport procedures
  m_s1ap_nas_transport->init();

  //Get pointer to the HSS
  m_hss = hss::get_instance();

  //Get pointer to GTP-C class
  m_mme_gtpc = mme_gtpc::get_instance();

  //Initialize S1-MME
  m_s1mme = enb_listen();

  m_s1ap_log->info("S1AP Initialized\n");
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
    m_s1ap_log->info("Deleting eNB context. eNB Id: 0x%x\n", it->second->enb_id);
    m_s1ap_log->console("Deleting eNB context. eNB Id: 0x%x\n", it->second->enb_id);
    delete_ues_in_enb(it->second->enb_id);
    delete it->second;
    m_active_enbs.erase(it++);
  }

  //Cleanup message handlers
  s1ap_mngmt_proc::cleanup();
  s1ap_nas_transport::cleanup();
  return;
}

void
s1ap::delete_enb_ctx(int32_t assoc_id)
{
  std::map<int32_t,uint16_t>::iterator it_assoc = m_sctp_to_enb_id.find(assoc_id);
  uint16_t enb_id = it_assoc->second;

  std::map<uint16_t,enb_ctx_t*>::iterator it_ctx = m_active_enbs.find(enb_id);
  if(it_ctx == m_active_enbs.end() || it_assoc == m_sctp_to_enb_id.end())
  {
    m_s1ap_log->error("Could not find eNB to delete. Association: %d\n",assoc_id);
    return;
  }

  m_s1ap_log->info("Deleting eNB context. eNB Id: 0x%x\n", enb_id);
  m_s1ap_log->console("Deleting eNB context. eNB Id: 0x%x\n", enb_id);

  //Delete connected UEs ctx
  delete_ues_in_enb(enb_id);

  //Delete eNB
  delete it_ctx->second;
  m_active_enbs.erase(it_ctx);
  m_sctp_to_enb_id.erase(it_assoc);
 return;
}

void
s1ap::delete_ues_in_enb(uint16_t enb_id)
{
  //delete UEs ctx
  std::map<uint16_t,std::set<uint32_t> >::iterator ues_in_enb = m_enb_id_to_ue_ids.find(enb_id);
  std::set<uint32_t>::iterator ue_id = ues_in_enb->second.begin();
  while(ue_id != ues_in_enb->second.end() )
  {
    std::map<uint32_t, ue_ctx_t*>::iterator ue_ctx = m_active_ues.find(*ue_id);
    m_s1ap_log->info("Deleting UE context. UE IMSI: %lu\n", ue_ctx->second->imsi);
    m_s1ap_log->console("Deleting UE context. UE IMSI: %lu\n", ue_ctx->second->imsi);
    delete ue_ctx->second;             //delete UE context
    m_active_ues.erase(ue_ctx);        //remove from general MME map 
    ues_in_enb->second.erase(ue_id++); //erase from the eNB's UE set
  }

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

  m_s1ap_log->info("S1-MME Initializing\n");
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
    m_s1ap_log->info("Received initiating PDU\n");
    return handle_initiating_message(&rx_pdu.choice.initiatingMessage, enb_sri);
    break;
  case LIBLTE_S1AP_S1AP_PDU_CHOICE_SUCCESSFULOUTCOME:
    m_s1ap_log->info("Received Succeseful Outcome PDU\n");
    return handle_successful_outcome(&rx_pdu.choice.successfulOutcome);
    break;
  case LIBLTE_S1AP_S1AP_PDU_CHOICE_UNSUCCESSFULOUTCOME:
    m_s1ap_log->info("Received Unsucceseful Outcome PDU\n");
    return true;//TODO handle_unsuccessfuloutcome(&rx_pdu.choice.unsuccessfulOutcome);
    break;
  default:
    m_s1ap_log->error("Unhandled PDU type %d\n", rx_pdu.choice_type);
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
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UPLINKNASTRANSPORT:
    m_s1ap_log->info("Received Uplink NAS Transport Message.\n");
    return handle_uplink_nas_transport(&msg->choice.UplinkNASTransport, enb_sri);
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UECONTEXTRELEASEREQUEST:
    m_s1ap_log->info("Received UE Context Release Request Message.\n");
    return handle_ue_context_release_request(&msg->choice.UEContextReleaseRequest, enb_sri);
  default:
    m_s1ap_log->error("Unhandled intiating message: %s\n", liblte_s1ap_initiatingmessage_choice_text[msg->choice_type]);
  }
  return true;
}

bool 
s1ap::handle_successful_outcome(LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT *msg)
{
  switch(msg->choice_type) {
  case LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_INITIALCONTEXTSETUPRESPONSE:
    m_s1ap_log->info("Received Initial Context Setup Response.\n");
    return handle_initial_context_setup_response(&msg->choice.InitialContextSetupResponse);
  default:
    m_s1ap_log->error("Unhandled successful outcome message: %s\n", liblte_s1ap_successfuloutcome_choice_text[msg->choice_type]);
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

  if(!m_s1ap_mngmt_proc->unpack_s1_setup_request(msg, &enb_ctx))
  {
    m_s1ap_log->error("Malformed S1 Setup Request\n");
    return false;
  }

  //Log S1 Setup Request Info
  m_s1ap_log->console("Received S1 Setup Request. Association: %d\n",enb_sri->sinfo_assoc_id);
  print_enb_ctx_info(enb_ctx);

  //Check matching PLMNs
  if(enb_ctx.plmn!=m_plmn){
    m_s1ap_log->console("Sending S1 Setup Failure - Unkown PLMN\n");
    m_s1ap_log->info("Sending S1 Setup Failure - Unkown PLMN\n");
    m_s1ap_mngmt_proc->pack_s1_setup_failure(LIBLTE_S1AP_CAUSEMISC_UNKNOWN_PLMN,&reply_msg);
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
      std::set<uint32_t> ue_set;
      enb_ctx_t *enb_ptr = new enb_ctx_t;
      memcpy(enb_ptr,&enb_ctx,sizeof(enb_ctx));
      m_active_enbs.insert(std::pair<uint16_t,enb_ctx_t*>(enb_ptr->enb_id,enb_ptr));
      m_sctp_to_enb_id.insert(std::pair<int32_t,uint16_t>(enb_sri->sinfo_assoc_id, enb_ptr->enb_id));
      m_enb_id_to_ue_ids.insert(std::pair<uint16_t,std::set<uint32_t> >(enb_ptr->enb_id,ue_set));
    }

    m_s1ap_mngmt_proc->pack_s1_setup_response(m_s1ap_args, &reply_msg);
    m_s1ap_log->console("Sending S1 Setup Response\n");
    m_s1ap_log->info("Sending S1 Setup Response\n");
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
s1ap::handle_initial_ue_message(LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT *init_ue, struct sctp_sndrcvinfo *enb_sri)
{
  LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT attach_req;
  LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT pdn_con_req;

  uint64_t    imsi;
  uint8_t     k_asme[32];
  uint8_t     autn[16]; 
  uint8_t     rand[6];
  uint8_t     xres[8];

  ue_ctx_t ue_ctx;

 /*Get info from initial UE message*/ 
  ue_ctx.enb_ue_s1ap_id = init_ue->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID;
  m_s1ap_log->console("Received Initial UE Message. eNB-UE S1AP Id: %d\n", ue_ctx.enb_ue_s1ap_id);
  m_s1ap_log->info("Received Initial UE Message. eNB-UE S1AP Id: %d\n", ue_ctx.enb_ue_s1ap_id);

  /*Log unhandled Initial UE message IEs*/
  m_s1ap_nas_transport->log_unhandled_initial_ue_message_ies(init_ue);

  /*Get NAS Attach Request and PDN connectivity request messages*/
  if(!m_s1ap_nas_transport->unpack_initial_ue_message(init_ue, &attach_req,&pdn_con_req))
  {
    //Could not decode the attach request and the PDN connectivity request.
    m_s1ap_log->error("Could not unpack NAS Attach Request and PDN connectivity request.\n");
    return false;
  }

  //Get IMSI
  imsi = 0;
  for(int i=0;i<=14;i++){
    imsi  += attach_req.eps_mobile_id.imsi[i]*std::pow(10,14-i);
  }
  m_s1ap_log->console("Attach request from IMSI: %015lu\n", imsi);
  m_s1ap_log->info("Attach request from IMSI: %015lu\n", imsi);  

  //Get UE network capabilities
  memcpy(&ue_ctx.ue_network_cap, &attach_req.ue_network_cap, sizeof(LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT));
  ue_ctx.ms_network_cap_present =  attach_req.ms_network_cap_present;
  if(attach_req.ms_network_cap_present)
  {
    memcpy(&ue_ctx.ms_network_cap, &attach_req.ms_network_cap, sizeof(LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT));
  }
  //FIXME use this info
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

  //Get Authentication Vectors from HSS
  if(!m_hss->gen_auth_info_answer(imsi, ue_ctx.security_ctxt.k_asme, autn, rand, ue_ctx.security_ctxt.xres))
  {
    m_s1ap_log->console("User not found. IMSI %015lu\n",imsi);
    m_s1ap_log->info("User not found. IMSI %015lu\n",imsi);
    return false;
  }

  //Save UE context
  ue_ctx.imsi = imsi;
  ue_ctx.mme_ue_s1ap_id = m_next_mme_ue_s1ap_id++; 
  for(uint i = 0 ; i< MAX_ERABS_PER_UE; i++)
  {
    ue_ctx.erabs_ctx[i].state = ERAB_DEACTIVATED;
    ue_ctx.erabs_ctx[i].erab_id = i;
  }

  printf("UL NAS count %d\n", ue_ctx.security_ctxt.ul_nas_count);
  ue_ctx_t *ue_ptr = new ue_ctx_t;
  memcpy(ue_ptr,&ue_ctx,sizeof(ue_ctx));
  printf("UL NAS count %d\n",(int) ue_ptr->security_ctxt.ul_nas_count);
  m_active_ues.insert(std::pair<uint32_t,ue_ctx_t*>(ue_ptr->mme_ue_s1ap_id,ue_ptr));
 
  std::map<int32_t,uint16_t>::iterator it_enb = m_sctp_to_enb_id.find(enb_sri->sinfo_assoc_id);
  uint16_t enb_id = it_enb->second;
  std::map<uint16_t,std::set<uint32_t> >::iterator it_ue_id = m_enb_id_to_ue_ids.find(enb_id);
  it_ue_id->second.insert(ue_ptr->mme_ue_s1ap_id);

  //Pack NAS Authentication Request in Downlink NAS Transport msg
  srslte::byte_buffer_t *reply_msg = m_pool->allocate();
  m_s1ap_nas_transport->pack_authentication_request(reply_msg, ue_ctx.enb_ue_s1ap_id, ue_ctx.mme_ue_s1ap_id, autn, rand);
  
  //Send Reply to eNB
  ssize_t n_sent = sctp_send(m_s1mme,reply_msg->msg, reply_msg->N_bytes, enb_sri, 0);
  if(n_sent == -1)
  {
    m_s1ap_log->error("Failed to send NAS Attach Request");
    return false;
  }
  m_s1ap_log->info("DL NAS: Sent Athentication Request\n");
  m_pool->deallocate(reply_msg);
  //TODO Start T3460 Timer!
  return true;
}

bool
s1ap::handle_uplink_nas_transport(LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT *ul_xport, struct sctp_sndrcvinfo *enb_sri)
{

  bool     ue_valid = true;
  uint32_t enb_ue_s1ap_id = ul_xport->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID;
  uint32_t mme_ue_s1ap_id = ul_xport->MME_UE_S1AP_ID.MME_UE_S1AP_ID;
  ue_ctx_t *ue_ctx;

  srslte::byte_buffer_t *reply_msg = m_pool->allocate();

  m_s1ap_log->console("Received Uplink NAS Transport message. MME-UE S1AP Id: %d\n",mme_ue_s1ap_id);
  m_s1ap_log->info("Received Uplink NAS Transport message. MME-UE S1AP Id: %d\n",mme_ue_s1ap_id);

  std::map<uint32_t, ue_ctx_t*>::iterator it = m_active_ues.find(mme_ue_s1ap_id);
  ue_ctx = it->second;
  if(it == m_active_ues.end())
  {
    //TODO UE not registered, send error message.
    m_s1ap_log->warning("Could not find UE. MME-UE S1AP id: %lu\n",mme_ue_s1ap_id);
    return false;
  }
  m_s1ap_log->debug("Found UE. MME-UE S1AP id: %lu\n",mme_ue_s1ap_id);

  printf("UL NAS count %d\n", ue_ctx->security_ctxt.ul_nas_count);
  //Get NAS message type
  uint8_t pd, msg_type;
  srslte::byte_buffer_t *nas_msg = m_pool->allocate();

  memcpy(nas_msg->msg, &ul_xport->NAS_PDU.buffer, ul_xport->NAS_PDU.n_octets);
  nas_msg->N_bytes = ul_xport->NAS_PDU.n_octets;
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &pd, &msg_type);

  switch (msg_type) {
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_RESPONSE:
      handle_nas_authentication_response(nas_msg, reply_msg, ue_ctx);
      m_s1ap_log->info("UL NAS: Received Authentication Response\n");
      break;
    case  LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMPLETE:
      m_s1ap_log->info("UL NAS: Received Security Mode Complete\n");
      handle_nas_security_mode_complete(nas_msg, reply_msg, ue_ctx);
      //ue_ctx->security_ctxt.ul_nas_count++;
      return true; //no need for reply. FIXME this should be better structured...
      break;
    case  LIBLTE_MME_MSG_TYPE_ATTACH_COMPLETE:
      m_s1ap_log->info("UL NAS: Received Attach Complete\n");
      handle_nas_attach_complete(nas_msg, reply_msg, ue_ctx);
      ue_ctx->security_ctxt.ul_nas_count++;
      return true; //no need for reply. FIXME this should be better structured...
    break;
    case LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_RESPONSE:
      m_s1ap_log->info("UL NAS: Received ESM Information Response\n");
      handle_esm_information_response(nas_msg,reply_msg,ue_ctx);
      ue_ctx->security_ctxt.ul_nas_count++;
      return true;
    default:
      m_s1ap_log->warning("Unhandled NAS message 0x%x\n", msg_type );
      m_s1ap_log->console("Unhandled NAS message 0x%x\n", msg_type );
      return false; //FIXME (nas_msg deallocate needs to be called)
  }


  //Send Reply to eNB
  ssize_t n_sent = sctp_send(m_s1mme,reply_msg->msg, reply_msg->N_bytes, enb_sri, 0);
  if(n_sent == -1)
  {
    m_s1ap_log->error("Failed to send NAS Attach Request");
    return false;
  }
  m_s1ap_log->info("DL NAS: Sent Downlink NAS message\n");
  m_s1ap_log->console("DL NAS: Sent Downlink NAs Message\n");
  m_pool->deallocate(nas_msg);
  m_pool->deallocate(reply_msg);

  return true;
}

bool
s1ap::handle_nas_authentication_response(srslte::byte_buffer_t *nas_msg, srslte::byte_buffer_t *reply_msg, ue_ctx_t *ue_ctx)
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
    m_s1ap_nas_transport->pack_authentication_reject(reply_msg, ue_ctx->enb_ue_s1ap_id, ue_ctx->mme_ue_s1ap_id);
    return false;
  }
  else
  {
    m_s1ap_log->console("UE Authentication Accepted. IMSI: %lu\n", ue_ctx->imsi);
    m_s1ap_log->info("UE Authentication Accepted. IMSI: %lu\n", ue_ctx->imsi);
    //Send Security Mode Command
    m_s1ap_nas_transport->pack_security_mode_command(reply_msg, ue_ctx);

  }
  return true;
}

bool
s1ap::handle_nas_security_mode_complete(srslte::byte_buffer_t *nas_msg, srslte::byte_buffer_t *reply_msg, ue_ctx_t *ue_ctx)
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
    pack_esm_information_request(reply_msg, ue_ctx);
    m_s1ap_log->console("Sending ESM information request\n");
  }
  else
  {
    //FIXME The packging of GTP-C messages is not ready.
    //This means that GTP-U tunnels are created with function calls, as opposed to GTP-C.
    m_mme_gtpc->send_create_session_request(ue_ctx->imsi, ue_ctx->mme_ue_s1ap_id);
  }
  return true;
}

bool
s1ap::pack_esm_information_request(srslte::byte_buffer_t *reply_msg, ue_ctx_t *ue_ctx)
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
  /*typedef struct{
    uint8 eps_bearer_id;
    uint8 proc_transaction_id;
    }LIBLTE_MME_ESM_INFORMATION_REQUEST_MSG_STRUCT;*/
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
  ssize_t n_sent = sctp_send(m_s1mme,reply_msg->msg, reply_msg->N_bytes, &ue_ctx->enb_sri, 0);
  if(n_sent == -1)
  {
      m_s1ap_log->error("Failed to send NAS Attach Request");
      return false;
  }
  return true;
}


bool
s1ap::handle_esm_information_response(srslte::byte_buffer_t *nas_msg, srslte::byte_buffer_t *reply_msg, ue_ctx_t* ue_ctx)
{
  LIBLTE_MME_ESM_INFORMATION_RESPONSE_MSG_STRUCT esm_info_resp;
  /*
  typedef struct{
    LIBLTE_MME_ACCESS_POINT_NAME_STRUCT       apn;
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    bool                                      apn_present;
    bool                                      protocol_cnfg_opts_present;
    }LIBLTE_MME_ESM_INFORMATION_RESPONSE_MSG_STRUCT;*/
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
  /*
  m_pool->deallocate(nas_buffer);
  ssize_t n_sent = sctp_send(m_s1mme,reply_msg->msg, reply_msg->N_bytes, &ue_ctx->enb_sri, 0);
  if(n_sent == -1)
  {
      m_s1ap_log->error("Failed to send NAS Attach Request");
      return false;
  }
  */

  //FIXME The packging of GTP-C messages is not ready.
  //This means that GTP-U tunnels are created with function calls, as opposed to GTP-C.
  m_mme_gtpc->send_create_session_request(ue_ctx->imsi, ue_ctx->mme_ue_s1ap_id);
  return true;
}
bool
s1ap::send_initial_context_setup_request(uint32_t mme_ue_s1ap_id, struct srslte::gtpc_create_session_response *cs_resp, struct srslte::gtpc_f_teid_ie sgw_ctrl_fteid)
{
  ue_ctx_t *ue_ctx;

  //Prepare reply PDU
  LIBLTE_S1AP_S1AP_PDU_STRUCT pdu;
  bzero(&pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));
  pdu.choice_type = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &pdu.choice.initiatingMessage;
  init->procedureCode = LIBLTE_S1AP_PROC_ID_INITIALCONTEXTSETUP;
  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_INITIALCONTEXTSETUPREQUEST;

  LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPREQUEST_STRUCT *in_ctxt_req = &init->choice.InitialContextSetupRequest;
  
  LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT *erab_ctxt = &in_ctxt_req->E_RABToBeSetupListCtxtSUReq.buffer[0]; //FIXME support more than one erab
  srslte::byte_buffer_t *reply_buffer = m_pool->allocate(); 

  m_s1ap_log->info("Preparing to send Initial Context Setup request\n");

  //Find UE Context
  std::map<uint32_t, ue_ctx_t*>::iterator ue_ctx_it = m_active_ues.find(mme_ue_s1ap_id);
  if(ue_ctx_it == m_active_ues.end())
  {
    m_s1ap_log->error("Could not find UE to send Setup Context Request. MME S1AP Id: %d", mme_ue_s1ap_id);
    return false;
  }
  ue_ctx = ue_ctx_it->second;

  //Add MME and eNB S1AP Ids
  in_ctxt_req->MME_UE_S1AP_ID.MME_UE_S1AP_ID = ue_ctx->mme_ue_s1ap_id;
  in_ctxt_req->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ue_ctx->enb_ue_s1ap_id;

  //Set UE-AMBR
  in_ctxt_req->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateDL.BitRate=1000000000;//2^32-1
  in_ctxt_req->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateUL.BitRate=1000000000;//FIXME Get UE-AMBR from HSS

  //Setup eRAB context
  in_ctxt_req->E_RABToBeSetupListCtxtSUReq.len = 1;
  erab_ctxt->e_RAB_ID.E_RAB_ID = cs_resp->eps_bearer_context_created.ebi;
  //Setup E-RAB QoS parameters
  erab_ctxt->e_RABlevelQoSParameters.qCI.QCI = 9;
  erab_ctxt->e_RABlevelQoSParameters.allocationRetentionPriority.priorityLevel.PriorityLevel = 15 ;//Lowest
  erab_ctxt->e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionCapability = LIBLTE_S1AP_PRE_EMPTIONCAPABILITY_SHALL_NOT_TRIGGER_PRE_EMPTION;
  erab_ctxt->e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionVulnerability = LIBLTE_S1AP_PRE_EMPTIONVULNERABILITY_PRE_EMPTABLE;

  erab_ctxt->e_RABlevelQoSParameters.gbrQosInformation_present=false;
  
  //Set E-RAB S-GW F-TEID
  if (cs_resp->eps_bearer_context_created.s1_u_sgw_f_teid_present == false){
    m_s1ap_log->error("Did not receive S1-U TEID in create session response\n");
    return false;
  } 
  erab_ctxt->transportLayerAddress.n_bits = 32; //IPv4
  uint32_t sgw_s1u_ip = htonl(cs_resp->eps_bearer_context_created.s1_u_sgw_f_teid.ipv4);
  //uint32_t sgw_s1u_ip = cs_resp->eps_bearer_context_created.s1_u_sgw_f_teid.ipv4;
  uint8_t *tmp_ptr =  erab_ctxt->transportLayerAddress.buffer;
  liblte_value_2_bits(sgw_s1u_ip, &tmp_ptr, 32);//FIXME consider ipv6

  uint32_t tmp_teid = cs_resp->eps_bearer_context_created.s1_u_sgw_f_teid.teid; 
  memcpy(erab_ctxt->gTP_TEID.buffer, &tmp_teid, sizeof(uint32_t));

  //Set UE security capabilities and k_enb
  bzero(in_ctxt_req->UESecurityCapabilities.encryptionAlgorithms.buffer,sizeof(uint8_t)*16); 
  bzero(in_ctxt_req->UESecurityCapabilities.integrityProtectionAlgorithms.buffer,sizeof(uint8_t)*16); 
  for(int i = 0; i<3; i++)
  {
    if(ue_ctx->ue_network_cap.eea[i+1] == true)
    {
      in_ctxt_req->UESecurityCapabilities.encryptionAlgorithms.buffer[i] = 1;          //EEA supported
    }
    else
    {
      in_ctxt_req->UESecurityCapabilities.encryptionAlgorithms.buffer[i] = 0;          //EEA not supported
    }
    if(ue_ctx->ue_network_cap.eia[i+1] == true)
    {
      in_ctxt_req->UESecurityCapabilities.integrityProtectionAlgorithms.buffer[i] = 1;          //EEA supported
    }
    else
    {
      in_ctxt_req->UESecurityCapabilities.integrityProtectionAlgorithms.buffer[i] = 0;          //EEA not supported
    }
    // in_ctxt_req->UESecurityCapabilities.integrityProtectionAlgorithms.buffer[0] = 1; //EIA1
  }
  uint8_t key_enb[32];
  liblte_security_generate_k_enb(ue_ctx->security_ctxt.k_asme, ue_ctx->security_ctxt.ul_nas_count, key_enb);
  liblte_unpack(key_enb, 32, in_ctxt_req->SecurityKey.buffer);
  m_s1ap_log->info("Generating KeNB with UL NAS COUNT: %d\n",ue_ctx->security_ctxt.ul_nas_count);
  //Set Attach accepted and activat default bearer NAS messages
  if(cs_resp->paa_present != true)
  {
    m_s1ap_log->error("PAA not present\n");
    return false;
  }
  if(cs_resp->paa.pdn_type != srslte::GTPC_PDN_TYPE_IPV4)
  {
    m_s1ap_log->error("IPv6 not supported yet\n");
    return false;
  }
  srslte::byte_buffer_t *nas_buffer = m_pool->allocate();
  m_s1ap_nas_transport->pack_attach_accept(ue_ctx, erab_ctxt, &cs_resp->paa, nas_buffer); 

  
  LIBLTE_ERROR_ENUM err = liblte_s1ap_pack_s1ap_pdu(&pdu, (LIBLTE_BYTE_MSG_STRUCT*)reply_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Could not pack Initial Context Setup Request Message\n");
    return false;
  }
  //Send Reply to eNB 
  ssize_t n_sent = sctp_send(m_s1mme,reply_buffer->msg, reply_buffer->N_bytes, &ue_ctx->enb_sri, 0);
  if(n_sent == -1)
  {
      m_s1ap_log->error("Failed to send Initial Context Setup Request\n");
      return false;
  }

  //Change E-RAB state to Context Setup Requested and save S-GW control F-TEID
  ue_ctx->erabs_ctx[erab_ctxt->e_RAB_ID.E_RAB_ID].state = ERAB_CTX_REQUESTED;
  ue_ctx->erabs_ctx[erab_ctxt->e_RAB_ID.E_RAB_ID].sgw_ctrl_fteid.teid = sgw_ctrl_fteid.teid;
  ue_ctx->erabs_ctx[erab_ctxt->e_RAB_ID.E_RAB_ID].sgw_ctrl_fteid.ipv4 = sgw_ctrl_fteid.ipv4;

  struct in_addr addr;
  addr.s_addr = sgw_s1u_ip;
  m_s1ap_log->info("Sent Intial Context Setup Request. E-RAB id %d \n",erab_ctxt->e_RAB_ID.E_RAB_ID);
  m_s1ap_log->info("Initial Context -- S1-U TEID 0x%x. IP %s \n", tmp_teid,inet_ntoa(addr));
  m_s1ap_log->console("Sent Intial Context Setup Request, E-RAB id %d\n",erab_ctxt->e_RAB_ID.E_RAB_ID);
  m_s1ap_log->console("Initial Context -- S1-U TEID 0x%x. IP %s \n", tmp_teid,inet_ntoa(addr));

  m_pool->deallocate(reply_buffer);
  m_pool->deallocate(nas_buffer);
  return true;
}


bool
s1ap::handle_initial_context_setup_response(LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT *in_ctxt_resp)
{

  uint32_t mme_ue_s1ap_id = in_ctxt_resp->MME_UE_S1AP_ID.MME_UE_S1AP_ID;
  std::map<uint32_t,ue_ctx_t*>::iterator ue_ctx_it = m_active_ues.find(mme_ue_s1ap_id);
  if (ue_ctx_it == m_active_ues.end())
  {
    m_s1ap_log->error("Could not find UE's context in active UE's map\n");
    return false;
  }
  for(uint32_t i=0; i<in_ctxt_resp->E_RABSetupListCtxtSURes.len;i++)
  {
    uint8_t erab_id = in_ctxt_resp->E_RABSetupListCtxtSURes.buffer[i].e_RAB_ID.E_RAB_ID;
    erab_ctx_t *erab_ctx = &ue_ctx_it->second->erabs_ctx[erab_id];
    if (erab_ctx->state != ERAB_CTX_REQUESTED)
    {
      m_s1ap_log->error("E-RAB requested was not active %d\n",erab_id);
      return false;
    }
    //Mark E-RAB with context setup
    erab_ctx->state = ERAB_CTX_SETUP;

    //Set the GTP information
    uint8_t *bit_ptr = in_ctxt_resp->E_RABSetupListCtxtSURes.buffer[i].transportLayerAddress.buffer;
    erab_ctx->enb_fteid.ipv4 = htonl(liblte_bits_2_value(&bit_ptr,32));
    memcpy(&erab_ctx->enb_fteid.teid, in_ctxt_resp->E_RABSetupListCtxtSURes.buffer[i].gTP_TEID.buffer, 4);
    erab_ctx->enb_fteid.teid = ntohl(erab_ctx->enb_fteid.teid);

    char enb_addr_str[INET_ADDRSTRLEN+1];
    const char *err = inet_ntop(AF_INET, &erab_ctx->enb_fteid.ipv4,enb_addr_str,sizeof(enb_addr_str));
    if(err == NULL)
    {
      m_s1ap_log->error("Error converting IP to string\n");
    }

    m_s1ap_log->info("E-RAB Context Setup. E-RAB id %d\n",erab_ctx->erab_id);
    m_s1ap_log->info("E-RAB Context -- eNB TEID 0x%x, eNB Address %s\n", erab_ctx->enb_fteid.teid, enb_addr_str);
    m_s1ap_log->console("E-RAB Context Setup. E-RAB id %d\n",erab_ctx->erab_id);
    m_s1ap_log->console("E-RAB Context -- eNB TEID 0x%x; eNB GTP-U Address %s\n", erab_ctx->enb_fteid.teid, enb_addr_str);

  }
  return true;
}

bool
s1ap::handle_nas_attach_complete(srslte::byte_buffer_t *nas_msg, srslte::byte_buffer_t *reply_msg, ue_ctx_t *ue_ctx)
{
  /*
  typedef struct{
  LIBLTE_BYTE_MSG_STRUCT  esm_msg;
  }LIBLTE_MME_ATTACH_COMPLETE_MSG_STRUCT;
  */
  /*
  typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    bool                                      protocol_cnfg_opts_present;
  }LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT;
  */
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

  //  memcpy(esm_msg->msg, attach_comp.esm_msg.buffer, attach_comp.esm_msg.N_bytes);
  //esm_msg->N_bytes = attach_comp.esm_msg.N_bytes;

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
s1ap::handle_ue_context_release_request(LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASEREQUEST_STRUCT *ue_rel, struct sctp_sndrcvinfo *enb_sri)
{

  uint32_t mme_ue_s1ap_id = ue_rel->MME_UE_S1AP_ID.MME_UE_S1AP_ID;
  m_s1ap_log->info("Received UE Context Release Request. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
  m_s1ap_log->console("Received UE Context Release Request. MME-UE S1AP Id %d\n", mme_ue_s1ap_id);

  std::map<uint32_t, ue_ctx_t*>::iterator ue_ctx = m_active_ues.find(mme_ue_s1ap_id);
  if(ue_ctx == m_active_ues.end() )
  {
    m_s1ap_log->info("UE not found. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
    return false;
  }
  
  //Delete UE within eNB UE set
  std::map<int32_t,uint16_t>::iterator it = m_sctp_to_enb_id.find(enb_sri->sinfo_assoc_id);
  if(it == m_sctp_to_enb_id.end() )
  {
    m_s1ap_log->error("Could not find eNB for this request.\n");
    return false;
  }  
  uint16_t enb_id = it->second; 
  std::map<uint16_t,std::set<uint32_t> >::iterator ue_set = m_enb_id_to_ue_ids.find(enb_id);
  if(ue_set == m_enb_id_to_ue_ids.end())
  {
    m_s1ap_log->error("Could not find the eNB's UEs.\n");
    return false;
  }
  ue_set->second.erase(mme_ue_s1ap_id);

  //Delete any context at the SPGW
  //m_spgw->delete_session_request(ue_ctx->imsi);

  //Delete UE context
  delete ue_ctx->second;
  m_active_ues.erase(ue_ctx);
  m_s1ap_log->info("Deleted UE Context.\n");
  return true;
}

void
s1ap::activate_eps_bearer(uint32_t mme_s1ap_id, uint8_t ebi)
{
  std::map<uint32_t,ue_ctx_t*>::iterator ue_ctx_it = m_active_ues.find(mme_s1ap_id);
  if(ue_ctx_it == m_active_ues.end())
  {
    m_s1ap_log->error("Could not find UE context\n");
    return;
  }
  ue_ctx_t * ue_ctx = ue_ctx_it->second;
  if (ue_ctx->erabs_ctx[ebi].state != ERAB_CTX_SETUP)
  {
    m_s1ap_log->error("EPS Bearer could not be activated. MME S1AP Id %d, EPS Bearer id %d, state %d\n",mme_s1ap_id,ebi,ue_ctx->erabs_ctx[ebi].state);
    m_s1ap_log->console("EPS Bearer could not be activated. MME S1AP Id %d, EPS Bearer id %d\n",mme_s1ap_id,ebi,ue_ctx->erabs_ctx[ebi].state);
    return;
  }

  ue_ctx->erabs_ctx[ebi].state = ERAB_ACTIVE;
  m_s1ap_log->info("Activated EPS Bearer\n");
  return;
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
