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
#include <inttypes.h> // for printing uint64_t
#include "srslte/asn1/gtpc.h"
#include "srsepc/hdr/mme/mme_gtpc.h"
#include "srsepc/hdr/mme/s1ap.h"
#include "srsepc/hdr/spgw/spgw.h"

namespace srsepc{

mme_gtpc*          mme_gtpc::m_instance = NULL;
pthread_mutex_t mme_gtpc_instance_mutex = PTHREAD_MUTEX_INITIALIZER;


mme_gtpc::mme_gtpc()
{
}

mme_gtpc::~mme_gtpc()
{
}

mme_gtpc*
mme_gtpc::get_instance(void)
{
  pthread_mutex_lock(&mme_gtpc_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new mme_gtpc();
  }
  pthread_mutex_unlock(&mme_gtpc_instance_mutex);
  return(m_instance);
}

void
mme_gtpc::cleanup(void)
{
  pthread_mutex_lock(&mme_gtpc_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&mme_gtpc_instance_mutex);
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
mme_gtpc::send_create_session_request(uint64_t imsi)
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

  m_mme_gtpc_log->info("Next MME control TEID: %d\n", m_next_ctrl_teid);
  m_mme_gtpc_log->info("Allocated MME control TEID: %d\n", cs_req->sender_f_teid.teid);
  m_mme_gtpc_log->console("Creating Session Response -- IMSI: %" PRIu64 "\n", imsi);
  m_mme_gtpc_log->console("Creating Session Response -- MME control TEID: %d\n", cs_req->sender_f_teid.teid);

  // APN
  strncpy(cs_req->apn, m_s1ap->m_s1ap_args.mme_apn.c_str(), sizeof(cs_req->apn)-1);
  cs_req->apn[sizeof(cs_req->apn)-1] = 0;
  // RAT Type
  //cs_req->rat_type = srslte::GTPC_RAT_TYPE::EUTRAN;

  //Check whether this UE is already registed
  std::map<uint64_t, struct gtpc_ctx>::iterator it = m_imsi_to_gtpc_ctx.find(imsi);
  if(it != m_imsi_to_gtpc_ctx.end())
  {
    m_mme_gtpc_log->warning("Create Session Request being called for an UE with an active GTP-C connection.\n");
    m_mme_gtpc_log->warning("Deleting previous GTP-C connection.\n");
    std::map<uint32_t, uint64_t>::iterator jt = m_mme_ctr_teid_to_imsi.find(it->second.mme_ctr_fteid.teid);
    if(jt == m_mme_ctr_teid_to_imsi.end())
    {
      m_mme_gtpc_log->error("Could not find IMSI from MME Ctrl TEID. MME Ctr TEID: %d\n", it->second.mme_ctr_fteid.teid);
    }
    else
    {
      m_mme_ctr_teid_to_imsi.erase(jt);
    }
    m_imsi_to_gtpc_ctx.erase(it);
    //No need to send delete session request to the SPGW.
    //The create session request will be interpreted as a new request and SPGW will delete locally in existing context.
  }

  //Save RX Control TEID
  m_mme_ctr_teid_to_imsi.insert(std::pair<uint32_t,uint64_t>(cs_req->sender_f_teid.teid, imsi));

  //Save GTP-C context
  gtpc_ctx_t gtpc_ctx;
  bzero(&gtpc_ctx,sizeof(gtpc_ctx_t));
  gtpc_ctx.mme_ctr_fteid = cs_req->sender_f_teid;
  m_imsi_to_gtpc_ctx.insert(std::pair<uint64_t,gtpc_ctx_t>(imsi,gtpc_ctx));
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
     //TODO Handle error
     return;
  }
  if (cs_resp->cause.cause_value != srslte::GTPC_CAUSE_VALUE_REQUEST_ACCEPTED){
    m_mme_gtpc_log->warning("Could not create GTPC session. Create Session Request not accepted\n");
    //TODO Handle error
    return;
  }

  //Get IMSI from the control TEID
  std::map<uint32_t,uint64_t>::iterator id_it = m_mme_ctr_teid_to_imsi.find(cs_resp_pdu->header.teid);
  if(id_it == m_mme_ctr_teid_to_imsi.end())
  {
    m_mme_gtpc_log->warning("Could not find IMSI from Ctrl TEID.\n");
    return;
  }
  uint64_t imsi = id_it->second;

  m_mme_gtpc_log->info("MME GTPC Ctrl TEID %" PRIu64 ", IMSI %" PRIu64 "\n", cs_resp_pdu->header.teid, imsi);

  //Get S-GW Control F-TEID
  srslte::gtp_fteid_t sgw_ctr_fteid;
  sgw_ctr_fteid.teid = cs_resp_pdu->header.teid;
  sgw_ctr_fteid.ipv4 = 0; //FIXME This is not used for now. In the future it will be obtained from the socket addr_info

  //Get S-GW S1-u F-TEID
  if (cs_resp->eps_bearer_context_created.s1_u_sgw_f_teid_present == false){
    m_mme_gtpc_log->error("Did not receive SGW S1-U F-TEID in create session response\n");
    return;
  }
  m_mme_gtpc_log->console("Create Session Response -- SPGW control TEID %d\n", sgw_ctr_fteid.teid);
  m_mme_gtpc_log->info("Create Session Response -- SPGW control TEID %d\n", sgw_ctr_fteid.teid);
  in_addr s1u_addr;
  s1u_addr.s_addr = cs_resp->eps_bearer_context_created.s1_u_sgw_f_teid.ipv4;
  m_mme_gtpc_log->console("Create Session Response -- SPGW S1-U Address: %s\n", inet_ntoa(s1u_addr));
  m_mme_gtpc_log->info("Create Session Response -- SPGW S1-U Address: %s\n", inet_ntoa(s1u_addr));

  //Check UE Ipv4 address was allocated
  if(cs_resp->paa_present != true)
  {
    m_mme_gtpc_log->error("PDN Adress Allocation not present\n");
    return;
  }
  if(cs_resp->paa.pdn_type != srslte::GTPC_PDN_TYPE_IPV4)
  {
    m_mme_gtpc_log->error("IPv6 not supported yet\n");
    return;
  }

  //Save create session response info to E-RAB context
  ue_ctx_t *ue_ctx = m_s1ap->find_ue_ctx_from_imsi(imsi);
  if(ue_ctx == NULL){
    m_mme_gtpc_log->error("Could not find UE context. IMSI %015lu\n", imsi);
    return;
  }
  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;

  //Save UE IP to nas ctxt
  emm_ctx->ue_ip.s_addr = cs_resp->paa.ipv4;
  m_mme_gtpc_log->console("SPGW Allocated IP %s to ISMI %015lu\n",inet_ntoa(emm_ctx->ue_ip),emm_ctx->imsi);
  //Save SGW ctrl F-TEID in GTP-C context
  std::map<uint64_t,struct gtpc_ctx>::iterator it_g = m_imsi_to_gtpc_ctx.find(imsi);
  if(it_g == m_imsi_to_gtpc_ctx.end())
  {
    //Could not find GTP-C Context
    m_mme_gtpc_log->error("Could not find GTP-C context\n");
    return;
  }
  gtpc_ctx_t *gtpc_ctx = &it_g->second;
  gtpc_ctx->sgw_ctr_fteid = sgw_ctr_fteid;

  //Set EPS bearer context
  //FIXME default EPS bearer is hard-coded
  int default_bearer=5;
  erab_ctx_t *erab_ctx = &ecm_ctx->erabs_ctx[default_bearer]; 
  erab_ctx->pdn_addr_alloc= cs_resp->paa;
  erab_ctx->sgw_s1u_fteid = cs_resp->eps_bearer_context_created.s1_u_sgw_f_teid;

  m_s1ap->m_s1ap_ctx_mngmt_proc->send_initial_context_setup_request(emm_ctx, ecm_ctx, erab_ctx);
  return;
}


void
mme_gtpc::send_modify_bearer_request(uint64_t imsi, erab_ctx_t *erab_ctx)
{
  m_mme_gtpc_log->info("Sending GTP-C Modify bearer request\n");
  srslte::gtpc_pdu mb_req_pdu;
  srslte::gtp_fteid_t *enb_fteid = &erab_ctx->enb_fteid;

  std::map<uint64_t,gtpc_ctx_t>::iterator it = m_imsi_to_gtpc_ctx.find(imsi);
  if(it == m_imsi_to_gtpc_ctx.end())
  {
    m_mme_gtpc_log->error("Modify bearer request for UE without GTP-C connection\n");
    return;
  }
  srslte::gtp_fteid_t sgw_ctr_fteid = it->second.sgw_ctr_fteid; 

  srslte::gtpc_header *header = &mb_req_pdu.header;
  header->teid_present = true;
  header->teid = sgw_ctr_fteid.teid;
  header->type = srslte::GTPC_MSG_TYPE_MODIFY_BEARER_REQUEST;

  srslte::gtpc_modify_bearer_request *mb_req = &mb_req_pdu.choice.modify_bearer_request;
  mb_req->eps_bearer_context_to_modify.ebi = erab_ctx->erab_id;
  mb_req->eps_bearer_context_to_modify.s1_u_enb_f_teid.ipv4 = enb_fteid->ipv4;
  mb_req->eps_bearer_context_to_modify.s1_u_enb_f_teid.teid = enb_fteid->teid;

  m_mme_gtpc_log->info("GTP-C Modify bearer request -- S-GW Control TEID %d\n", sgw_ctr_fteid.teid );
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
  std::map<uint32_t,uint64_t>::iterator imsi_it = m_mme_ctr_teid_to_imsi.find(mme_ctrl_teid);
  if(imsi_it == m_mme_ctr_teid_to_imsi.end())
  {
    m_mme_gtpc_log->error("Could not find IMSI from control TEID\n");
    return;
  }

  uint8_t ebi = mb_resp_pdu->choice.modify_bearer_response.eps_bearer_context_modified.ebi;
  m_mme_gtpc_log->debug("Activating EPS bearer with id %d\n", ebi);
  m_s1ap->activate_eps_bearer(imsi_it->second,ebi);

  return;
}

void
mme_gtpc::send_delete_session_request(uint64_t imsi) 
{
  m_mme_gtpc_log->info("Sending GTP-C Delete Session Request request. IMSI %" PRIu64 "\n",imsi);
  srslte::gtpc_pdu del_req_pdu;
  srslte::gtp_fteid_t sgw_ctr_fteid;
  srslte::gtp_fteid_t mme_ctr_fteid;
  //Get S-GW Ctr TEID
  std::map<uint64_t,gtpc_ctx_t>::iterator it_ctx = m_imsi_to_gtpc_ctx.find(imsi);
  if(it_ctx == m_imsi_to_gtpc_ctx.end())
  {
      m_mme_gtpc_log->error("Could not find GTP-C context to remove\n");
      return;
  }
  sgw_ctr_fteid = it_ctx->second.sgw_ctr_fteid;
  mme_ctr_fteid = it_ctx->second.mme_ctr_fteid;
  srslte::gtpc_header *header = &del_req_pdu.header;
  header->teid_present = true;
  header->teid = sgw_ctr_fteid.teid;
  header->type = srslte::GTPC_MSG_TYPE_DELETE_SESSION_REQUEST;

  srslte::gtpc_delete_session_request *del_req = &del_req_pdu.choice.delete_session_request;
  del_req->cause.cause_value = srslte::GTPC_CAUSE_VALUE_ISR_DEACTIVATION;
  m_mme_gtpc_log->info("GTP-C Delete Session Request -- S-GW Control TEID %d\n", sgw_ctr_fteid.teid );

  srslte::gtpc_pdu del_resp_pdu;
  m_spgw->handle_delete_session_request(&del_req_pdu, &del_resp_pdu);

  //TODO Handle delete session response

  //Delete GTP-C context
  std::map<uint32_t,uint64_t>::iterator it_imsi = m_mme_ctr_teid_to_imsi.find(mme_ctr_fteid.teid);
  if(it_imsi == m_mme_ctr_teid_to_imsi.end())
  {
    m_mme_gtpc_log->error("Could not find IMSI from MME ctr TEID");
  }
  else
  {
    m_mme_ctr_teid_to_imsi.erase(it_imsi);
  }
  m_imsi_to_gtpc_ctx.erase(it_ctx);
  return;
}

void
mme_gtpc::send_release_access_bearers_request(uint64_t imsi) 
{
  m_mme_gtpc_log->info("Sending GTP-C Delete Access Bearers Request\n");
  srslte::gtpc_pdu rel_req_pdu;
  srslte::gtp_fteid_t sgw_ctr_fteid;

  //Get S-GW Ctr TEID
  std::map<uint64_t,gtpc_ctx_t>::iterator it_ctx = m_imsi_to_gtpc_ctx.find(imsi);
  if(it_ctx == m_imsi_to_gtpc_ctx.end())
  {
    m_mme_gtpc_log->error("Could not find GTP-C context to remove\n");
    return;
  }
  sgw_ctr_fteid = it_ctx->second.sgw_ctr_fteid;

  //Set GTP-C header
  srslte::gtpc_header *header = &rel_req_pdu.header;
  header->teid_present = true;
  header->teid = sgw_ctr_fteid.teid;
  header->type = srslte::GTPC_MSG_TYPE_RELEASE_ACCESS_BEARERS_REQUEST;

  srslte::gtpc_release_access_bearers_request *rel_req = &rel_req_pdu.choice.release_access_bearers_request;
  m_mme_gtpc_log->info("GTP-C Release Access Berarers Request -- S-GW Control TEID %d\n", sgw_ctr_fteid.teid );

  srslte::gtpc_pdu rel_resp_pdu;
  m_spgw->handle_release_access_bearers_request(&rel_req_pdu, &rel_resp_pdu);

  //The GTP-C connection will not be torn down, just the user plane bearers.
  return;
}
} //namespace srsepc
