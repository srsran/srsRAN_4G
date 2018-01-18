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

//#include "srslte/upper/s1ap_common.h"
#include "srslte/common/bcd_helpers.h"
#include "mme/s1ap.h"
#include "mme/s1ap_ctx_mngmt_proc.h"
#include "srslte/common/liblte_security.h"


namespace srsepc{

s1ap_ctx_mngmt_proc*          s1ap_ctx_mngmt_proc::m_instance = NULL;
boost::mutex   s1ap_ctx_mngmt_proc_instance_mutex;


s1ap_ctx_mngmt_proc::s1ap_ctx_mngmt_proc()
{
}

s1ap_ctx_mngmt_proc::~s1ap_ctx_mngmt_proc()
{
}

s1ap_ctx_mngmt_proc*
s1ap_ctx_mngmt_proc::get_instance(void)
{
  boost::mutex::scoped_lock lock(s1ap_ctx_mngmt_proc_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new s1ap_ctx_mngmt_proc();
  }
  return(m_instance);
}

void
s1ap_ctx_mngmt_proc::cleanup(void)
{
  boost::mutex::scoped_lock lock(s1ap_ctx_mngmt_proc_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
}

void
s1ap_ctx_mngmt_proc::init(void)
{
  m_s1ap     = s1ap::get_instance();
  m_mme_gtpc = mme_gtpc::get_instance();
  m_s1ap_log = m_s1ap->m_s1ap_log;
  m_s1ap_args = m_s1ap->m_s1ap_args;
  m_pool = srslte::byte_buffer_pool::get_instance();
  m_s1ap_nas_transport = s1ap_nas_transport::get_instance();
}

bool
s1ap_ctx_mngmt_proc::send_initial_context_setup_request(uint32_t mme_ue_s1ap_id, struct srslte::gtpc_create_session_response *cs_resp, struct srslte::gtpc_f_teid_ie sgw_ctrl_fteid)
{
  int s1mme = m_s1ap->get_s1_mme();

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
  ue_ctx_t *ue_ctx = m_s1ap->find_ue_ctx(mme_ue_s1ap_id);
  if(ue_ctx == NULL)
  {
    m_s1ap_log->error("Could not find UE to send Setup Context Request. MME S1AP Id: %d", mme_ue_s1ap_id);
    return false;
  }

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
  ssize_t n_sent = sctp_send(s1mme,reply_buffer->msg, reply_buffer->N_bytes, &ue_ctx->enb_sri, 0);
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
  addr.s_addr = htonl(sgw_s1u_ip);
  m_s1ap_log->info("Sent Intial Context Setup Request. E-RAB id %d \n",erab_ctxt->e_RAB_ID.E_RAB_ID);
  m_s1ap_log->info("Initial Context -- S1-U TEID 0x%x. IP %s \n", tmp_teid,inet_ntoa(addr));
  m_s1ap_log->console("Sent Intial Context Setup Request, E-RAB id %d\n",erab_ctxt->e_RAB_ID.E_RAB_ID);
  m_s1ap_log->console("Initial Context -- S1-U TEID 0x%x. IP %s \n", tmp_teid,inet_ntoa(addr));

  m_pool->deallocate(reply_buffer);
  m_pool->deallocate(nas_buffer);
  return true;
}

bool
s1ap_ctx_mngmt_proc::handle_initial_context_setup_response(LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT *in_ctxt_resp)
{

  uint32_t mme_ue_s1ap_id = in_ctxt_resp->MME_UE_S1AP_ID.MME_UE_S1AP_ID;
  ue_ctx_t *ue_ctx = m_s1ap->find_ue_ctx(mme_ue_s1ap_id);
  if (ue_ctx == NULL)
  {
    m_s1ap_log->error("Could not find UE's context in active UE's map\n");
    return false;
  }

  //Setup E-RABs
  for(uint32_t i=0; i<in_ctxt_resp->E_RABSetupListCtxtSURes.len;i++)
  {
    uint8_t erab_id = in_ctxt_resp->E_RABSetupListCtxtSURes.buffer[i].e_RAB_ID.E_RAB_ID;
    erab_ctx_t *erab_ctx = &ue_ctx->erabs_ctx[erab_id];
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
s1ap_ctx_mngmt_proc::handle_ue_context_release_request(LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASEREQUEST_STRUCT *ue_rel, struct sctp_sndrcvinfo *enb_sri, srslte::byte_buffer_t *reply_buffer, bool *reply_flag)
{

  uint32_t mme_ue_s1ap_id = ue_rel->MME_UE_S1AP_ID.MME_UE_S1AP_ID;
  m_s1ap_log->info("Received UE Context Release Request. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
  m_s1ap_log->console("Received UE Context Release Request. MME-UE S1AP Id %d\n", mme_ue_s1ap_id);

  ue_ctx_t *ue_ctx = m_s1ap->find_ue_ctx(mme_ue_s1ap_id);
  if(ue_ctx == NULL)
  {
    m_s1ap_log->info("UE not found. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
    return false;
  }

  //Delete any context at the SPGW
  bool active = false;
  for(int i=0;i<MAX_ERABS_PER_UE;i++)
  {
    if(ue_ctx->erabs_ctx[i].state != ERAB_DEACTIVATED)
    {
      active = true;
      //ue_ctx->erabs_ctx[i].state = ERAB_DEACTIVATED;
      break;
    }
  }
  if(active == true)
  {
    //There are active E-RABs, send delete session request
    m_mme_gtpc->send_delete_session_request(ue_ctx);
  }
  //m_s1ap->delete_ue_ctx(ue_ctx);
  for(int i=0;i<MAX_ERABS_PER_UE;i++)
  {
    ue_ctx->erabs_ctx[i].state = ERAB_DEACTIVATED;
  }
  //Delete UE context
  m_s1ap_log->info("Deleted UE Context.\n");
  return true;
}

} //namespace srsepc
