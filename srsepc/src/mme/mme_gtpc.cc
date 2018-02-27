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
#include "srslte/asn1/gtpc.h"
#include "mme/mme_gtpc.h"
#include "mme/s1ap.h"
#include "spgw/spgw.h"

namespace srsepc{

mme_gtpc*          mme_gtpc::m_instance = NULL;
boost::mutex  mme_gtpc_instance_mutex;

mme_gtpc::mme_gtpc()
{
}

mme_gtpc::~mme_gtpc()
{
}

mme_gtpc*
mme_gtpc::get_instance(void)
{
  boost::mutex::scoped_lock lock(mme_gtpc_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new mme_gtpc();
  }
  return(m_instance);
}

void
mme_gtpc::cleanup(void)
{
  boost::mutex::scoped_lock lock(mme_gtpc_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
}


bool
mme_gtpc::init(srslte::log_filter *mme_gtpc_log)
{

  /*Init log*/
  m_mme_gtpc_log = mme_gtpc_log;

  m_next_ctrl_teid = 1;

  m_s1ap = s1ap::get_instance();
  m_mme_gtpc_ip = inet_addr("127.0.0.1");//FIXME At the moment, the GTP-C messages are not sent over the wire. So this parameter is not used.
  m_spgw = spgw::get_instance();

  m_mme_gtpc_log->info("MME GTP-C Initialized\n");
  m_mme_gtpc_log->console("MME GTP-C Initialized\n");
  return true;
}

uint32_t
mme_gtpc::get_new_ctrl_teid()
{
  return m_next_ctrl_teid++; //FIXME Use a Id pool?
}
void
mme_gtpc::send_create_session_request(uint64_t imsi, uint32_t mme_ue_s1ap_id)
{
  m_mme_gtpc_log->info("Sending Create Session Request.\n");
  m_mme_gtpc_log->console("Sending Create Session Request.\n");
  struct srslte::gtpc_pdu cs_req_pdu;
  struct srslte::gtpc_create_session_request *cs_req = &cs_req_pdu.choice.create_session_request;

  struct srslte::gtpc_pdu cs_resp_pdu;
 

  //Initialize GTP-C message to zero
  bzero(&cs_req_pdu, sizeof(struct srslte::gtpc_pdu));

  //Setup GTP-C Header. FIXME: Length, sequence and other fields need to be added.
  cs_req_pdu.header.piggyback = false;
  cs_req_pdu.header.teid_present = true;
  cs_req_pdu.header.teid = 0; //Send create session request to the butler TEID
  cs_req_pdu.header.type = srslte::GTPC_MSG_TYPE_CREATE_SESSION_REQUEST;

  //Setup GTP-C Create Session Request IEs
  cs_req->imsi = imsi;
  // Control TEID allocated
  cs_req->sender_f_teid.teid = get_new_ctrl_teid();
  cs_req->sender_f_teid.ipv4 = m_mme_gtpc_ip;

  m_mme_gtpc_log->info("Next control TEID: %lu \n", m_next_ctrl_teid);
  m_mme_gtpc_log->info("Allocated control TEID: %lu \n", cs_req->sender_f_teid.teid);
  m_mme_gtpc_log->console("Creating Session Response -- IMSI: %015lu \n", imsi);
  m_mme_gtpc_log->console("Creating Session Response -- MME control TEID: %lu \n", cs_req->sender_f_teid.teid);
  // APN
  strncpy(cs_req->apn, m_s1ap->m_s1ap_args.mme_apn.c_str(), sizeof(cs_req->apn)-1);
  cs_req->apn[sizeof(cs_req->apn)-1] = 0;
  // RAT Type
  //cs_req->rat_type = srslte::GTPC_RAT_TYPE::EUTRAN;

  //Save RX Control TEID
  m_teid_to_mme_s1ap_id.insert(std::pair<uint32_t,uint32_t>(cs_req->sender_f_teid.teid, mme_ue_s1ap_id));

  m_spgw->handle_create_session_request(cs_req, &cs_resp_pdu);
 
}

void
mme_gtpc::handle_create_session_response(srslte::gtpc_pdu *cs_resp_pdu)
{
  struct srslte::gtpc_create_session_response *cs_resp = & cs_resp_pdu->choice.create_session_response;
  m_mme_gtpc_log->info("Received Create Session Response\n");
  m_mme_gtpc_log->console("Received Create Session Response\n");
  if (cs_resp_pdu->header.type != srslte::GTPC_MSG_TYPE_CREATE_SESSION_RESPONSE)
  {
     m_mme_gtpc_log->warning("Could not create GTPC session. Not a create session response\n");
     //TODO Handle err
     return;
  }
  if (cs_resp->cause.cause_value != srslte::GTPC_CAUSE_VALUE_REQUEST_ACCEPTED){
    m_mme_gtpc_log->warning("Could not create GTPC session. Create Session Request not accepted\n");
    //TODO Handle error
    return;
  }
  
  //Get MME_UE_S1AP_ID from the control TEID
  std::map<uint32_t,uint32_t>::iterator id_it = m_teid_to_mme_s1ap_id.find(cs_resp_pdu->header.teid);
  if(id_it == m_teid_to_mme_s1ap_id.end())
  {
    //Could not find MME UE S1AP TEID
    m_mme_gtpc_log->warning("Could not find MME UE S1AP TEID.\n");
    return;
  }
  uint32_t mme_s1ap_id = id_it->second;

  //Get S-GW Control F-TEID
  srslte::gtpc_f_teid_ie sgw_ctrl_fteid;
  sgw_ctrl_fteid.teid = cs_resp_pdu->header.teid;
  sgw_ctrl_fteid.ipv4 = 0; //FIXME This is not used for now. In the future it will be obtained from the socket addr_info

  m_mme_gtpc_log->console("Create Session Response -- SPGW control TEID %d\n", sgw_ctrl_fteid.teid);
  in_addr s1u_addr;
  s1u_addr.s_addr = cs_resp->eps_bearer_context_created.s1_u_sgw_f_teid.ipv4;
  m_mme_gtpc_log->console("Create Session Response -- SPGW S1-U Address: %s\n", inet_ntoa(s1u_addr));
  m_s1ap->m_s1ap_ctx_mngmt_proc->send_initial_context_setup_request(mme_s1ap_id, cs_resp, sgw_ctrl_fteid);
}


void
mme_gtpc::send_modify_bearer_request(erab_ctx_t *erab_ctx)
{
  m_mme_gtpc_log->info("Sending GTP-C Modify bearer request\n");
  srslte::gtpc_pdu mb_req_pdu;
  srslte::gtpc_f_teid_ie *enb_fteid = &erab_ctx->enb_fteid; 
  srslte::gtpc_f_teid_ie *sgw_ctrl_fteid = &erab_ctx->sgw_ctrl_fteid; 

  srslte::gtpc_header *header = &mb_req_pdu.header;
  header->teid_present = true;
  header->teid = sgw_ctrl_fteid->teid;
  header->type = srslte::GTPC_MSG_TYPE_MODIFY_BEARER_REQUEST;

  srslte::gtpc_modify_bearer_request *mb_req = &mb_req_pdu.choice.modify_bearer_request;
  mb_req->eps_bearer_context_to_modify.ebi = erab_ctx->erab_id;
  mb_req->eps_bearer_context_to_modify.s1_u_enb_f_teid.ipv4 = enb_fteid->ipv4;
  mb_req->eps_bearer_context_to_modify.s1_u_enb_f_teid.teid = enb_fteid->teid;

  m_mme_gtpc_log->info("GTP-C Modify bearer request -- S-GW Control TEID %d\n", sgw_ctrl_fteid->teid );
  struct in_addr addr;
  addr.s_addr = enb_fteid->ipv4;
  m_mme_gtpc_log->info("GTP-C Modify bearer request -- S1-U TEID 0x%x, IP %s\n", enb_fteid->teid, inet_ntoa(addr) );

  //
  srslte::gtpc_pdu mb_resp_pdu;
  m_spgw->handle_modify_bearer_request(&mb_req_pdu,&mb_resp_pdu);
  handle_modify_bearer_response(&mb_resp_pdu);
  return;
}

void
mme_gtpc::handle_modify_bearer_response(srslte::gtpc_pdu *mb_resp_pdu)
{
  uint32_t mme_ctrl_teid = mb_resp_pdu->header.teid;
  std::map<uint32_t,uint32_t>::iterator mme_s1ap_id_it = m_teid_to_mme_s1ap_id.find(mme_ctrl_teid);
  if(mme_s1ap_id_it == m_teid_to_mme_s1ap_id.end())
  {
    m_mme_gtpc_log->error("Could not find MME S1AP Id from control TEID\n");
    return;
  }

  uint8_t ebi = mb_resp_pdu->choice.modify_bearer_response.eps_bearer_context_modified.ebi;
  m_mme_gtpc_log->debug("Activating EPS bearer with id %d\n", ebi);
  m_s1ap->activate_eps_bearer(mme_s1ap_id_it->second,ebi);

  return;
}

void
mme_gtpc::send_delete_session_request(ue_ctx_t *ue_ctx) 
{
  m_mme_gtpc_log->info("Sending GTP-C Delete Session Request request\n");
  srslte::gtpc_pdu del_req_pdu;
  srslte::gtpc_f_teid_ie *sgw_ctrl_fteid = NULL;

  //FIXME the UE control TEID sould be stored in the UE ctxt, not in the E-RAB ctxt
  //Maybe a mme_s1ap_id to ctrl teid map as well?

  for(int i = 0; i<MAX_ERABS_PER_UE; i++)
  {
    if(ue_ctx->erabs_ctx[i].state != ERAB_DEACTIVATED)
    {
      sgw_ctrl_fteid = &ue_ctx->erabs_ctx[i].sgw_ctrl_fteid;
      break;
    }
  }
  //FIXME: add proper error handling
  assert(sgw_ctrl_fteid != NULL);

  srslte::gtpc_header *header = &del_req_pdu.header;
  header->teid_present = true;
  header->teid = sgw_ctrl_fteid->teid;
  header->type = srslte::GTPC_MSG_TYPE_DELETE_SESSION_REQUEST;

  srslte::gtpc_delete_session_request *del_req = &del_req_pdu.choice.delete_session_request;
  del_req->cause.cause_value = srslte::GTPC_CAUSE_VALUE_ISR_DEACTIVATION;
  m_mme_gtpc_log->info("GTP-C Delete Session Request -- S-GW Control TEID %d\n", sgw_ctrl_fteid->teid );

  srslte::gtpc_pdu del_resp_pdu;
  m_spgw->handle_delete_session_request(&del_req_pdu, &del_resp_pdu);

  //TODO Handle delete session response
  return;
}
} //namespace srsepc
