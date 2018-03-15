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
  m_next_mme_ue_s1ap_id(1),
  m_mme_gtpc(NULL),
  m_pool(NULL)
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
s1ap::init(s1ap_args_t s1ap_args, srslte::log_filter *s1ap_log, hss_interface_s1ap * hss_)
{
  m_pool = srslte::byte_buffer_pool::get_instance();

  m_s1ap_args = s1ap_args;
  srslte::s1ap_mccmnc_to_plmn(s1ap_args.mcc, s1ap_args.mnc, &m_plmn);
  m_next_m_tmsi = rand();
  //Init log
  m_s1ap_log = s1ap_log;

  //Get pointer to the HSS
  m_hss = hss_;

  //Init message handlers
  m_s1ap_mngmt_proc = s1ap_mngmt_proc::get_instance(); //Managment procedures
  m_s1ap_mngmt_proc->init();
  m_s1ap_nas_transport = s1ap_nas_transport::get_instance(); //NAS Transport procedures
  m_s1ap_nas_transport->init(m_hss);
  m_s1ap_ctx_mngmt_proc = s1ap_ctx_mngmt_proc::get_instance(); //Context Management Procedures
  m_s1ap_ctx_mngmt_proc->init();


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
  s1ap_ctx_mngmt_proc::cleanup();
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
    close(sock_fd);
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
    close(sock_fd);
    m_s1ap_log->error("Error binding SCTP socket\n");
    m_s1ap_log->console("Error binding SCTP socket\n");
    return -1;
  }

  //Listen for connections
  err = listen(sock_fd,SOMAXCONN);
  if (err != 0){
    close(sock_fd);
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
  bool reply_flag = false;
  srslte::byte_buffer_t * reply_buffer = m_pool->allocate();

  switch(msg->choice_type) {
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_S1SETUPREQUEST:
    m_s1ap_log->info("Received S1 Setup Request.\n");
    m_s1ap_mngmt_proc->handle_s1_setup_request(&msg->choice.S1SetupRequest, enb_sri, reply_buffer, &reply_flag);
    break;
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_INITIALUEMESSAGE:
    m_s1ap_log->info("Received Initial UE Message.\n");
    m_s1ap_nas_transport->handle_initial_ue_message(&msg->choice.InitialUEMessage, enb_sri, reply_buffer, &reply_flag);
    break;
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UPLINKNASTRANSPORT:
    m_s1ap_log->info("Received Uplink NAS Transport Message.\n");
    m_s1ap_nas_transport->handle_uplink_nas_transport(&msg->choice.UplinkNASTransport, enb_sri, reply_buffer, &reply_flag);
    break;
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UECONTEXTRELEASEREQUEST:
    m_s1ap_log->info("Received UE Context Release Request Message.\n");
    m_s1ap_ctx_mngmt_proc->handle_ue_context_release_request(&msg->choice.UEContextReleaseRequest, enb_sri, reply_buffer, &reply_flag);
    break;
  default:
    m_s1ap_log->error("Unhandled intiating message: %s\n", liblte_s1ap_initiatingmessage_choice_text[msg->choice_type]);
  }
  //Send Reply to eNB
  if(reply_flag == true)
  {
    ssize_t n_sent = sctp_send(m_s1mme,reply_buffer->msg, reply_buffer->N_bytes, enb_sri, 0);
    if(n_sent == -1)
    {
      m_s1ap_log->console("Failed to send S1 Setup Setup Reply\n");
      m_pool->deallocate(reply_buffer);
      return false;
    }
  }
  m_pool->deallocate(reply_buffer);
  return true;
}

bool 
s1ap::handle_successful_outcome(LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT *msg)
{
  switch(msg->choice_type) {
  case LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_INITIALCONTEXTSETUPRESPONSE:
    m_s1ap_log->info("Received Initial Context Setup Response.\n");
    return m_s1ap_ctx_mngmt_proc->handle_initial_context_setup_response(&msg->choice.InitialContextSetupResponse);
  default:
    m_s1ap_log->error("Unhandled successful outcome message: %s\n", liblte_s1ap_successfuloutcome_choice_text[msg->choice_type]);
  }
  return true;
}


bool
s1ap::delete_ue_ctx(ue_ctx_t *ue_ctx)
{
  uint32_t mme_ue_s1ap_id = ue_ctx->mme_ue_s1ap_id;
  std::map<uint32_t, ue_ctx_t*>::iterator ue_ctx_it = m_active_ues.find(mme_ue_s1ap_id);
  if(ue_ctx_it == m_active_ues.end() )
  {
    m_s1ap_log->info("UE not found. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
    return false;
  }

  //Delete UE within eNB UE set
  std::map<int32_t,uint16_t>::iterator it = m_sctp_to_enb_id.find(ue_ctx->enb_sri.sinfo_assoc_id);
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

  //Delete UE context
  delete ue_ctx;
  m_active_ues.erase(ue_ctx_it);
  m_s1ap_log->info("Deleted UE Context.\n");

  return true;
}

enb_ctx_t*
s1ap::find_enb_ctx(uint16_t enb_id)
{
  std::map<uint16_t,enb_ctx_t*>::iterator it = m_active_enbs.find(enb_id);
  if(it == m_active_enbs.end())
  {
    return NULL;
  }
  else
  {
    return it->second;
  }
}

void
s1ap::add_new_enb_ctx(const enb_ctx_t &enb_ctx, const struct sctp_sndrcvinfo *enb_sri)
{
  m_s1ap_log->info("Adding new eNB context. eNB ID %d\n", enb_ctx.enb_id);
  std::set<uint32_t> ue_set;
  enb_ctx_t *enb_ptr = new enb_ctx_t;
  memcpy(enb_ptr,&enb_ctx,sizeof(enb_ctx_t));
  m_active_enbs.insert(std::pair<uint16_t,enb_ctx_t*>(enb_ptr->enb_id,enb_ptr));
  m_sctp_to_enb_id.insert(std::pair<int32_t,uint16_t>(enb_sri->sinfo_assoc_id, enb_ptr->enb_id));
  m_enb_id_to_ue_ids.insert(std::pair<uint16_t,std::set<uint32_t> >(enb_ptr->enb_id,ue_set));

  return;
}

ue_ctx_t*
s1ap::find_ue_ctx(uint32_t mme_ue_s1ap_id)
{
  std::map<uint32_t, ue_ctx_t*>::iterator it = m_active_ues.find(mme_ue_s1ap_id);
  if(it == m_active_ues.end())
    {
      return NULL;
  }
  else
  {
    return it->second;
  }
}

void
s1ap::add_new_ue_ctx(const ue_ctx_t &ue_ctx)
{
  ue_ctx_t *ue_ptr = new ue_ctx_t;
  memcpy(ue_ptr,&ue_ctx,sizeof(ue_ctx));
  m_active_ues.insert(std::pair<uint32_t,ue_ctx_t*>(ue_ptr->mme_ue_s1ap_id,ue_ptr));
 
  std::map<int32_t,uint16_t>::iterator it_enb = m_sctp_to_enb_id.find(ue_ptr->enb_sri.sinfo_assoc_id);
  uint16_t enb_id = it_enb->second;
  std::map<uint16_t,std::set<uint32_t> >::iterator it_ue_id = m_enb_id_to_ue_ids.find(enb_id);
  if(it_ue_id==m_enb_id_to_ue_ids.end())
  {
    m_s1ap_log->error("Could not find eNB's UEs\n");
    return;
  }
  it_ue_id->second.insert(ue_ptr->mme_ue_s1ap_id);
  return;
}

uint32_t 
s1ap::get_next_mme_ue_s1ap_id()
{
  return m_next_mme_ue_s1ap_id++;
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

uint32_t
s1ap::allocate_m_tmsi(uint32_t mme_ue_s1ap_id)
{
  //uint32_t m_tmsi = m_next_m_tmsi++;
  //m_tmsi_to_s1ap_id.insert(std::pair<uint32_t,uint32_t>(m_tmsi,mme_ue_s1ap_id));
  uint32_t m_tmsi = 0x0123;
  return m_tmsi;
}

void
s1ap::print_enb_ctx_info(const std::string &prefix, const enb_ctx_t &enb_ctx)
{
  std::string mnc_str, mcc_str;

  if(enb_ctx.enb_name_present)
  {
    m_s1ap_log->console("%s - eNB Name: %s, eNB id: 0x%x\n",prefix.c_str(), enb_ctx.enb_name, enb_ctx.enb_id);
    m_s1ap_log->info("%s - eNB Name: %s, eNB id: 0x%x\n", prefix.c_str(), enb_ctx.enb_name, enb_ctx.enb_id);
  }
  else
  {
    m_s1ap_log->console("%s - eNB Id 0x%x\n",prefix.c_str(), enb_ctx.enb_id);
    m_s1ap_log->info("%s - eNB Id 0x%x\n", prefix.c_str(), enb_ctx.enb_id);
  }
  srslte::mcc_to_string(enb_ctx.mcc, &mcc_str);
  srslte::mnc_to_string(enb_ctx.mnc, &mnc_str);
  m_s1ap_log->info("%s - MCC:%s, MNC:%s, PLMN: %d\n", prefix.c_str(), mcc_str.c_str(), mnc_str.c_str(), enb_ctx.plmn);
  m_s1ap_log->console("%s - MCC:%s, MNC:%s, PLMN: %d\n", prefix.c_str(), mcc_str.c_str(), mnc_str.c_str(), enb_ctx.plmn);
  for(int i=0;i<enb_ctx.nof_supported_ta;i++)
  {
    for(int j=0;i<enb_ctx.nof_supported_ta;i++)
    {
      m_s1ap_log->info("%s - TAC %d, B-PLMN %d\n",prefix.c_str(), enb_ctx.tac[i],enb_ctx.bplmns[i][j]);
      m_s1ap_log->console("%s - TAC %d, B-PLMN %d\n",prefix.c_str(), enb_ctx.tac[i],enb_ctx.bplmns[i][j]);
    }
  }
  m_s1ap_log->console("%s - Paging DRX %d\n",prefix.c_str(),enb_ctx.drx);
  return;
}

} //namespace srsepc
