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

#include <strings.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/sctp.h>
#include <unistd.h>

//#include "srslte/upper/s1ap_common.h"
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

  m_s1ap_log = s1ap_log;

  m_s1mme = enb_listen();
  
  m_s1ap_log->console("Initialized S1-APP\n"); 
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

  //Set timeout
  struct timeval timeout;      
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  if (setsockopt (sock_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
    m_s1ap_log->console("Set socket timeout failed\n");
    return -1; 
  }

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
  //s1mme_addr.sin_addr.s_addr = htonl(INADDR_ANY); //TODO this should use the bindx information
  s1mme_addr.sin_port = htons(S1MME_PORT);
  err = bind(sock_fd, (struct sockaddr*) &s1mme_addr, sizeof (s1mme_addr));
  if (err != 0){
    std::cout << "Error binding SCTP socket" << std::endl;
    return -1;
  }

  //Listen for connections
  err = listen(sock_fd,SOMAXCONN);
  if (err != 0){
    std::cout << "Error in SCTP socket listen" << std::endl;
    return -1;
  }

  return sock_fd;
}

bool
s1ap::handle_s1ap_rx_pdu(srslte::byte_buffer_t *pdu, struct sctp_sndrcvinfo *enb_sri) 
{
  LIBLTE_S1AP_S1AP_PDU_STRUCT rx_pdu;

  if(liblte_s1ap_unpack_s1ap_pdu((LIBLTE_BYTE_MSG_STRUCT*)pdu, &rx_pdu) != LIBLTE_SUCCESS) {
    m_s1ap_log->console("Failed to unpack received PDU\n");
    return false;
  }

  switch(rx_pdu.choice_type) {
  case LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE:
    m_s1ap_log->console("Received initiating PDU\n");
    return handle_initiatingmessage(&rx_pdu.choice.initiatingMessage, enb_sri);
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
s1ap::handle_initiatingmessage(LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *msg,  struct sctp_sndrcvinfo *enb_sri)
{
  switch(msg->choice_type) {
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_S1SETUPREQUEST:
    std::cout << "Received S1 Setup Request." << std::endl;
    return handle_s1setuprequest(&msg->choice.S1SetupRequest, enb_sri);
  default:
    std::cout << "Unhandled intiating message" << std::cout;
    //s1ap_log->error("Unhandled intiating message: %s\n", liblte_s1ap_initiatingmessage_choice_text[msg->choice_type]);
  }
  return true;
}

bool 
s1ap::handle_s1setuprequest(LIBLTE_S1AP_MESSAGE_S1SETUPREQUEST_STRUCT *msg, struct sctp_sndrcvinfo *enb_sri)
{
  
  uint8_t enb_name[150];
  uint8_t enb_id[20];
  uint32_t plmn = 0;
  std::string mnc_str, mcc_str;
  uint16_t mcc, mnc;  
  uint16_t tac, bplmn;
  uint32_t bplmns[32];

  //eNB Name
  if(msg->eNBname_present)
  {
    bzero(enb_name,sizeof(enb_name));
    memcpy(enb_name,&msg->eNBname.buffer,msg->eNBname.n_octets);
    std::cout <<"eNB Name: " << enb_name <<std::endl;
  }
  //eNB Id 9.2.1.37
  memcpy(&enb_id, msg->Global_ENB_ID.eNB_ID.choice.macroENB_ID.buffer, LIBLTE_S1AP_MACROENB_ID_BIT_STRING_LEN);
  std::cout << "eNB ID: ";
  for (int i=0;i < 20;i++) {
    std::cout<< (uint16_t)enb_id[i];
  }
  std::cout << std::endl;
  //PLMN Id
  ((uint8_t*)&plmn)[1] = msg->Global_ENB_ID.pLMNidentity.buffer[0];
  ((uint8_t*)&plmn)[2] = msg->Global_ENB_ID.pLMNidentity.buffer[1];
  ((uint8_t*)&plmn)[3] = msg->Global_ENB_ID.pLMNidentity.buffer[2];

  plmn = ntohl(plmn);
  srslte::s1ap_plmn_to_mccmnc(plmn, &mcc, &mnc);
  srslte::mnc_to_string(mnc, &mnc_str);
  srslte::mnc_to_string(mcc, &mcc_str);
  std::cout << "MCC: "<< mcc_str << " MNC: " << mnc_str << std::endl;

  //SupportedTAs
  for(uint16_t i=0; i<msg->SupportedTAs.len; i++)
  {
    //tac = msg->SupportedTAs.buffer[i].tAC.buffer[]; //broadcastPLMNs
    ((uint8_t*)&tac)[0] = msg->SupportedTAs.buffer[i].tAC.buffer[0];
    ((uint8_t*)&tac)[1] = msg->SupportedTAs.buffer[i].tAC.buffer[1];
    std::cout << "TAC: " << ntohs(tac) << std::endl;
    for (uint16_t j=0; j<msg->SupportedTAs.buffer[i].broadcastPLMNs.len; j++)
    {
      ((uint8_t*)&bplmns[j])[1] = msg->SupportedTAs.buffer[i].broadcastPLMNs.buffer[j].buffer[0];
      ((uint8_t*)&bplmns[j])[2] = msg->SupportedTAs.buffer[i].broadcastPLMNs.buffer[j].buffer[1];
      ((uint8_t*)&bplmns[j])[3] = msg->SupportedTAs.buffer[i].broadcastPLMNs.buffer[j].buffer[2];

      bplmns[j] = ntohl(bplmns[j]);
      srslte::mnc_to_string(mnc, &mnc_str);
      srslte::mnc_to_string(mcc, &mcc_str);
      std::cout << "B_MCC: "<< mcc_str << " B_MNC: " << mnc_str << std::endl;
    }
  }

  //Default Paging DRX
  LIBLTE_S1AP_PAGINGDRX_ENUM drx = msg->DefaultPagingDRX.e;
  std::cout << "Default Paging DRX" << drx << std::endl;

  send_s1setupfailure(enb_sri);
  
  return true;
}

bool
s1ap::send_s1setupfailure(struct sctp_sndrcvinfo *enb_sri)
{
  srslte::byte_buffer_t       msg;
  LIBLTE_S1AP_S1AP_PDU_STRUCT pdu;
  bzero(&pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));

  pdu.choice_type = LIBLTE_S1AP_S1AP_PDU_CHOICE_UNSUCCESSFULOUTCOME;

  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_STRUCT *unsucc = &pdu.choice.unsuccessfulOutcome;
  unsucc->procedureCode = LIBLTE_S1AP_PROC_ID_S1SETUP;
  unsucc->criticality = LIBLTE_S1AP_CRITICALITY_REJECT;
  unsucc->choice_type = LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_S1SETUPFAILURE;
 
  LIBLTE_S1AP_MESSAGE_S1SETUPFAILURE_STRUCT* s1_fail=(LIBLTE_S1AP_MESSAGE_S1SETUPFAILURE_STRUCT*)&unsucc->choice;

  s1_fail->TimeToWait_present=false;
  s1_fail->CriticalityDiagnostics_present=false;
  s1_fail->Cause.ext=false;
  s1_fail->Cause.choice_type = LIBLTE_S1AP_CAUSE_CHOICE_MISC;
  s1_fail->Cause.choice.misc.ext=false;
  s1_fail->Cause.choice.misc.e=LIBLTE_S1AP_CAUSEMISC_UNKNOWN_PLMN;
  
  liblte_s1ap_pack_s1ap_pdu(&pdu, (LIBLTE_BYTE_MSG_STRUCT*)&msg);
  
  ssize_t n_sent = sctp_send(m_s1mme,msg.msg, msg.N_bytes, enb_sri, 0);
  
  if(n_sent == -1)
  {
    m_s1ap_log->console("Failed to send S1 Setup Failure");
    return false;
  }
  return true;
}


bool
s1ap::send_s1setupresponse(struct sctp_sndrcvinfo *enb_sri)
{
  srslte::byte_buffer_t       msg;
  LIBLTE_S1AP_S1AP_PDU_STRUCT pdu;
  bzero(&pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));

  pdu.choice_type = LIBLTE_S1AP_S1AP_PDU_CHOICE_SUCCESSFULOUTCOME;

  LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT *succ = &pdu.choice.successfulOutcome;
  succ->procedureCode = LIBLTE_S1AP_PROC_ID_S1SETUP;
  succ->criticality = LIBLTE_S1AP_CRITICALITY_REJECT;
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


/*
bool
s1ap::setup_enb_ctx()
{
  return false;
}*/

} //namespace srsepc
