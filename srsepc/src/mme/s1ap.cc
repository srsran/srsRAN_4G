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

#include <iostream> //TODO Remove

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
  m_mme_code    = s1ap_args.mme_code ;
  m_mme_group   = s1ap_args.mme_group;
  m_tac         = s1ap_args.tac;
  m_mcc         = s1ap_args.mcc;        
  m_mnc         = s1ap_args.mnc;        
  m_mme_bind_addr = s1ap_args.mme_bind_addr;
  m_mme_name = std::string("srsmme0");

  srslte::s1ap_mccmnc_to_plmn(m_mcc, m_mnc, &m_plmn);

  m_s1ap_log = s1ap_log;

  m_s1mme = enb_listen();
  
  return 0;
}

void
s1ap::stop()
{
  if (m_s1mme != -1){
    close(m_s1mme);
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
  bzero (&evnts, sizeof (evnts)) ;
  evnts.sctp_data_io_event = 1;
  if(setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof (evnts))){
    m_s1ap_log->console("Subscribing to sctp_data_io_events failed\n");
    return -1;
  }

  //S1-MME bind
  bzero(&s1mme_addr, sizeof(s1mme_addr));
  s1mme_addr.sin_family = AF_INET; 
  inet_pton(AF_INET, m_mme_bind_addr.c_str(), &(s1mme_addr.sin_addr) );
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
  
  //Check matching PLMNs  
  if(enb_ctx.plmn!=m_plmn){
    m_s1ap_log->console("S1 Setup Failure - Unkown PLMN\n");
    m_s1ap_log->info("S1 Setup Failure - Unkown PLMN\n");
    m_s1ap_mngmt_proc.pack_s1_setup_failure(LIBLTE_S1AP_CAUSEMISC_UNKNOWN_PLMN,&reply_msg);
    ssize_t n_sent = sctp_send(m_s1mme,reply_msg.msg, reply_msg.N_bytes, enb_sri, 0); //FIXME
  }
  else{
    m_s1ap_log->console("S1 Setup Response\n");
    m_s1ap_log->info("S1 Setup Response\n");
    //m_s1ap_mngmt_proc.pack_s1_setup_response(,&reply_msg);
    send_s1_setup_response(enb_sri);
  }
  
  //Send Reply to eNB
  /*
  ssize_t n_sent = sctp_send(m_s1mme,msg.msg, msg.N_bytes, enb_sri, 0);
  if(n_sent == -1)
  {
    m_s1ap_log->console("Failed to send S1 Setup Setup Reply");
    return false;
  }
  */
  return true;
  
}

bool
s1ap::send_s1_setup_response(struct sctp_sndrcvinfo *enb_sri)
{
  srslte::byte_buffer_t       msg;
  LIBLTE_S1AP_S1AP_PDU_STRUCT pdu;
  bzero(&pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));

  pdu.choice_type = LIBLTE_S1AP_S1AP_PDU_CHOICE_SUCCESSFULOUTCOME;

  LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT *succ = &pdu.choice.successfulOutcome;
  succ->procedureCode = LIBLTE_S1AP_PROC_ID_S1SETUP;
  succ->criticality = LIBLTE_S1AP_CRITICALITY_IGNORE;
  succ->choice_type = LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_S1SETUPRESPONSE;
 
  LIBLTE_S1AP_MESSAGE_S1SETUPRESPONSE_STRUCT* s1_resp=(LIBLTE_S1AP_MESSAGE_S1SETUPRESPONSE_STRUCT*)&succ->choice;

  s1_resp->ext=false;
  
  //MME Name
  s1_resp->MMEname_present=true;
  s1_resp->MMEname.ext=false;
  s1_resp->MMEname.n_octets=m_mme_name.length();
  memcpy(s1_resp->MMEname.buffer,m_mme_name.c_str(),m_mme_name.length());

  //Served GUMEIs
  s1_resp->ServedGUMMEIs.len=1;//TODO Only one served GUMMEI supported
  LIBLTE_S1AP_SERVEDGUMMEISITEM_STRUCT *serv_gummei = &s1_resp->ServedGUMMEIs.buffer[0];

  serv_gummei->ext=false;
  //serv_gummei->iE_Extensions=false;

  uint32_t plmn=0;
  srslte::s1ap_mccmnc_to_plmn(m_mcc, m_mnc, &plmn);
  plmn=htonl(plmn);
  serv_gummei->servedPLMNs.len = 1; //Only one PLMN supported
  serv_gummei->servedPLMNs.buffer[0].buffer[0]=((uint8_t*)&plmn)[1];
  serv_gummei->servedPLMNs.buffer[0].buffer[1]=((uint8_t*)&plmn)[2];
  serv_gummei->servedPLMNs.buffer[0].buffer[2]=((uint8_t*)&plmn)[3];

  serv_gummei->servedGroupIDs.len=1; //LIBLTE_S1AP_SERVEDGROUPIDS_STRUCT
  uint16_t tmp=htons(m_mme_group);
  serv_gummei->servedGroupIDs.buffer[0].buffer[0]=((uint8_t*)&tmp)[0];
  serv_gummei->servedGroupIDs.buffer[0].buffer[1]=((uint8_t*)&tmp)[1];
 
  serv_gummei->servedMMECs.len=1; //Only one MMEC served
  serv_gummei->servedMMECs.buffer[0].buffer[0]=m_mme_code;

  //Relative MME Capacity
  s1_resp->RelativeMMECapacity.RelativeMMECapacity=255;

  //Relay Unsupported
  s1_resp->MMERelaySupportIndicator_present=false;
    
  liblte_s1ap_pack_s1ap_pdu(&pdu, (LIBLTE_BYTE_MSG_STRUCT*)&msg);
  
  ssize_t n_sent = sctp_send(m_s1mme,msg.msg, msg.N_bytes, enb_sri, 0);
  
  if(n_sent == -1)
  {
    m_s1ap_log->console("Failed to send S1 Setup Failure");
    return false;
  }
  return true;
}

} //namespace srsepc
