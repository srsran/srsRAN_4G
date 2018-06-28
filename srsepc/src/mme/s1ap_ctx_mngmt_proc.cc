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
#include "srsepc/hdr/mme/s1ap.h"
#include "srsepc/hdr/mme/s1ap_ctx_mngmt_proc.h"
#include "srslte/common/liblte_security.h"


namespace srsepc{

s1ap_ctx_mngmt_proc*          s1ap_ctx_mngmt_proc::m_instance = NULL;
pthread_mutex_t s1ap_ctx_mngmt_proc_instance_mutex = PTHREAD_MUTEX_INITIALIZER;


s1ap_ctx_mngmt_proc::s1ap_ctx_mngmt_proc()
{
}

s1ap_ctx_mngmt_proc::~s1ap_ctx_mngmt_proc()
{
}

s1ap_ctx_mngmt_proc*
s1ap_ctx_mngmt_proc::get_instance(void)
{
  pthread_mutex_lock(&s1ap_ctx_mngmt_proc_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new s1ap_ctx_mngmt_proc();
  }
  pthread_mutex_unlock(&s1ap_ctx_mngmt_proc_instance_mutex);
  return(m_instance);
}

void
s1ap_ctx_mngmt_proc::cleanup(void)
{
  pthread_mutex_lock(&s1ap_ctx_mngmt_proc_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&s1ap_ctx_mngmt_proc_instance_mutex);
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
s1ap_ctx_mngmt_proc::send_initial_context_setup_request(ue_emm_ctx_t *emm_ctx,
                                                        ue_ecm_ctx_t *ecm_ctx,
                                                        erab_ctx_t *erab_ctx)
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
  
  LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT *erab_ctx_req = &in_ctxt_req->E_RABToBeSetupListCtxtSUReq.buffer[0]; //FIXME support more than one erab
  srslte::byte_buffer_t *reply_buffer = m_pool->allocate(); 

  m_s1ap_log->info("Preparing to send Initial Context Setup request\n");

  //Add MME and eNB S1AP Ids
  in_ctxt_req->MME_UE_S1AP_ID.MME_UE_S1AP_ID = ecm_ctx->mme_ue_s1ap_id;
  in_ctxt_req->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ecm_ctx->enb_ue_s1ap_id;

  //Set UE-AMBR
  in_ctxt_req->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateDL.BitRate=1000000000;
  in_ctxt_req->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateUL.BitRate=1000000000;//FIXME Get UE-AMBR from HSS

  //Setup eRAB context
  in_ctxt_req->E_RABToBeSetupListCtxtSUReq.len = 1;
  erab_ctx_req->e_RAB_ID.E_RAB_ID = erab_ctx->erab_id;
  //Setup E-RAB QoS parameters
  erab_ctx_req->e_RABlevelQoSParameters.qCI.QCI = erab_ctx->qci;
  erab_ctx_req->e_RABlevelQoSParameters.allocationRetentionPriority.priorityLevel.PriorityLevel = 15 ;//Lowest
  erab_ctx_req->e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionCapability = LIBLTE_S1AP_PRE_EMPTIONCAPABILITY_SHALL_NOT_TRIGGER_PRE_EMPTION;
  erab_ctx_req->e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionVulnerability = LIBLTE_S1AP_PRE_EMPTIONVULNERABILITY_PRE_EMPTABLE;
  erab_ctx_req->e_RABlevelQoSParameters.gbrQosInformation_present=false;


  //Set E-RAB S-GW F-TEID
  //if (cs_resp->eps_bearer_context_created.s1_u_sgw_f_teid_present == false){
  //  m_s1ap_log->error("Did not receive S1-U TEID in create session response\n");
  //  return false;
  //} 
  erab_ctx_req->transportLayerAddress.n_bits = 32; //IPv4
  uint32_t sgw_s1u_ip = htonl(erab_ctx->sgw_s1u_fteid.ipv4);
  //uint32_t sgw_s1u_ip = cs_resp->eps_bearer_context_created.s1_u_sgw_f_teid.ipv4;
  uint8_t *tmp_ptr =  erab_ctx_req->transportLayerAddress.buffer;
  liblte_value_2_bits(sgw_s1u_ip, &tmp_ptr, 32);//FIXME consider ipv6

  uint32_t sgw_s1u_teid = erab_ctx->sgw_s1u_fteid.teid; 
  memcpy(erab_ctx_req->gTP_TEID.buffer, &sgw_s1u_teid, sizeof(uint32_t));

  //Set UE security capabilities and k_enb
  bzero(in_ctxt_req->UESecurityCapabilities.encryptionAlgorithms.buffer,sizeof(uint8_t)*16); 
  bzero(in_ctxt_req->UESecurityCapabilities.integrityProtectionAlgorithms.buffer,sizeof(uint8_t)*16); 
  for(int i = 0; i<3; i++)
  {
    if(emm_ctx->security_ctxt.ue_network_cap.eea[i+1] == true)
    {
      in_ctxt_req->UESecurityCapabilities.encryptionAlgorithms.buffer[i] = 1;          //EEA supported
    }
    else
    {
      in_ctxt_req->UESecurityCapabilities.encryptionAlgorithms.buffer[i] = 0;          //EEA not supported
    }
    if(emm_ctx->security_ctxt.ue_network_cap.eia[i+1] == true)
    {
      in_ctxt_req->UESecurityCapabilities.integrityProtectionAlgorithms.buffer[i] = 1;          //EEA supported
    }
    else
    {
      in_ctxt_req->UESecurityCapabilities.integrityProtectionAlgorithms.buffer[i] = 0;          //EEA not supported
    }
  }
  //Get K eNB
  liblte_unpack(emm_ctx->security_ctxt.k_enb, 32, in_ctxt_req->SecurityKey.buffer);
  m_s1ap_log->info_hex(emm_ctx->security_ctxt.k_enb, 32, "Initial Context Setup Request -- Key eNB (k_enb)\n");

  srslte::byte_buffer_t *nas_buffer = m_pool->allocate();
  if(emm_ctx->state == EMM_STATE_DEREGISTERED)
  {
    //Attach procedure initiated from an attach request
    m_s1ap_log->console("Adding attach accept to Initial Context Setup Request\n");
    m_s1ap_log->info("Adding attach accept to Initial Context Setup Request\n");
    m_s1ap_nas_transport->pack_attach_accept(emm_ctx, ecm_ctx, erab_ctx_req, &erab_ctx->pdn_addr_alloc, nas_buffer); 
  }


  LIBLTE_ERROR_ENUM err = liblte_s1ap_pack_s1ap_pdu(&pdu, (LIBLTE_BYTE_MSG_STRUCT*)reply_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Could not pack Initial Context Setup Request Message\n");
    return false;
  }

  //Send Reply to eNB
  ssize_t n_sent = sctp_send(s1mme,reply_buffer->msg, reply_buffer->N_bytes, &ecm_ctx->enb_sri, 0); 
  if(n_sent == -1)
  {
      m_s1ap_log->error("Failed to send Initial Context Setup Request\n");
      return false;
  }

  //Change E-RAB state to Context Setup Requested and save S-GW control F-TEID
  ecm_ctx->erabs_ctx[erab_ctx_req->e_RAB_ID.E_RAB_ID].state = ERAB_CTX_REQUESTED;

  struct in_addr addr;
  addr.s_addr = htonl(sgw_s1u_ip);
  m_s1ap_log->info("Sent Intial Context Setup Request. E-RAB id %d \n",erab_ctx_req->e_RAB_ID.E_RAB_ID);
  m_s1ap_log->info("Initial Context -- S1-U TEID 0x%x. IP %s \n", sgw_s1u_teid,inet_ntoa(addr));
  m_s1ap_log->console("Initial Context Setup Request -- eNB UE S1AP Id %d, MME UE S1AP Id %d\n",in_ctxt_req->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID, in_ctxt_req->MME_UE_S1AP_ID.MME_UE_S1AP_ID);
  m_s1ap_log->console("Initial Context Setup Request -- E-RAB id %d\n",erab_ctx_req->e_RAB_ID.E_RAB_ID);
  m_s1ap_log->console("Initial Context Setup Request -- S1-U TEID 0x%x. IP %s \n", sgw_s1u_teid,inet_ntoa(addr));
  m_s1ap_log->console("Initial Context Setup Request -- S1-U TEID 0x%x. IP %s \n", sgw_s1u_teid,inet_ntoa(addr));
  m_s1ap_log->console("Initial Context Setup Request -- QCI %d \n", erab_ctx_req->e_RABlevelQoSParameters.qCI.QCI);

  m_pool->deallocate(reply_buffer);
  m_pool->deallocate(nas_buffer);
  return true;
}

bool
s1ap_ctx_mngmt_proc::handle_initial_context_setup_response(LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT *in_ctxt_resp)
{
  uint32_t mme_ue_s1ap_id = in_ctxt_resp->MME_UE_S1AP_ID.MME_UE_S1AP_ID;
  ue_ctx_t *ue_ctx = m_s1ap->find_ue_ctx_from_mme_ue_s1ap_id(mme_ue_s1ap_id);
  if (ue_ctx == NULL)
  {
    m_s1ap_log->error("Could not find UE's context in active UE's map\n");
    return false;
  }
  ue_emm_ctx_t * emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t * ecm_ctx = &ue_ctx->ecm_ctx;

  m_s1ap_log->console("Received Initial Context Setup Response\n");
  //Setup E-RABs
  for(uint32_t i=0; i<in_ctxt_resp->E_RABSetupListCtxtSURes.len;i++)
  {
    uint8_t erab_id = in_ctxt_resp->E_RABSetupListCtxtSURes.buffer[i].e_RAB_ID.E_RAB_ID;
    erab_ctx_t *erab_ctx = &ecm_ctx->erabs_ctx[erab_id];
    if (erab_ctx->state != ERAB_CTX_REQUESTED)
    {
      m_s1ap_log->error("E-RAB requested was not previously requested %d\n",erab_id);
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
  if(emm_ctx->state == EMM_STATE_REGISTERED)
  {
    m_s1ap_log->console("Initial Context Setup Response triggered from Service Request.\n");
    m_s1ap_log->console("Sending Modify Bearer Request.\n");
    m_mme_gtpc->send_modify_bearer_request(emm_ctx->imsi, &ecm_ctx->erabs_ctx[5]);
  }
  return true;
}

bool
s1ap_ctx_mngmt_proc::handle_ue_context_release_request(LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASEREQUEST_STRUCT *ue_rel, struct sctp_sndrcvinfo *enb_sri, srslte::byte_buffer_t *reply_buffer, bool *reply_flag)
{

  LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASEREQUEST_STRUCT ue_rel_req;

  uint32_t mme_ue_s1ap_id = ue_rel->MME_UE_S1AP_ID.MME_UE_S1AP_ID;
  m_s1ap_log->info("Received UE Context Release Request. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
  m_s1ap_log->console("Received UE Context Release Request. MME-UE S1AP Id %d\n", mme_ue_s1ap_id);

  ue_ctx_t * ue_ctx = m_s1ap->find_ue_ctx_from_mme_ue_s1ap_id(mme_ue_s1ap_id);
  if(ue_ctx == NULL)
  {
    m_s1ap_log->info("No UE context to release found. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
    m_s1ap_log->console("No UE context to release found. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
    return false;
  }
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx; 

  //Delete user plane context at the SPGW (but keep GTP-C connection).
  if (ecm_ctx->state == ECM_STATE_CONNECTED)
  {
    //There are active E-RABs, send release access mearers request
    m_s1ap_log->console("There are active E-RABs, send release access mearers request\n");
    m_s1ap_log->info("There are active E-RABs, send release access mearers request\n");

    //The handle_release_access_bearers_response function will make sure to mark E-RABS DEACTIVATED
    //It will release the UEs downstream S1-u and keep the upstream S1-U connection active.
    m_mme_gtpc->send_release_access_bearers_request(ecm_ctx->imsi);

    //Send release context command to enb, so that it can release it's bearers
    send_ue_context_release_command(ecm_ctx,reply_buffer);
  }
  else
  {
    //No ECM Context to release
    m_s1ap_log->info("UE is not ECM connected. No need to release S1-U. MME UE S1AP Id %d\n", mme_ue_s1ap_id);
    m_s1ap_log->console("UE is not ECM connected. No need to release S1-U. MME UE S1AP Id %d\n", mme_ue_s1ap_id);
    //Make sure E-RABS are merked as DEACTIVATED.
    for(int i=0;i<MAX_ERABS_PER_UE;i++)
    {
      ecm_ctx->erabs_ctx[i].state = ERAB_DEACTIVATED;
    }
  }
  
  //Delete UE context
  ecm_ctx->state = ECM_STATE_IDLE;
  ecm_ctx->enb_ue_s1ap_id = 0;
  ecm_ctx->mme_ue_s1ap_id = 0;
  m_s1ap_log->info("UE is ECM IDLE.\n");
  m_s1ap_log->console("UE is ECM IDLE.\n");
  return true;
}

bool
s1ap_ctx_mngmt_proc::send_ue_context_release_command(ue_ecm_ctx_t *ecm_ctx, srslte::byte_buffer_t *reply_buffer)
{

  int s1mme = m_s1ap->get_s1_mme();

  //Prepare reply PDU
  LIBLTE_S1AP_S1AP_PDU_STRUCT pdu;
  bzero(&pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));
  pdu.choice_type = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &pdu.choice.initiatingMessage;
  init->procedureCode = LIBLTE_S1AP_PROC_ID_UECONTEXTRELEASE;
  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UECONTEXTRELEASECOMMAND;

  LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMMAND_STRUCT *ctx_rel_cmd = &init->choice.UEContextReleaseCommand;

  ctx_rel_cmd->UE_S1AP_IDs.choice_type = LIBLTE_S1AP_UE_S1AP_IDS_CHOICE_UE_S1AP_ID_PAIR;
  ctx_rel_cmd->UE_S1AP_IDs.choice.uE_S1AP_ID_pair.mME_UE_S1AP_ID.MME_UE_S1AP_ID = ecm_ctx->mme_ue_s1ap_id;
  ctx_rel_cmd->UE_S1AP_IDs.choice.uE_S1AP_ID_pair.eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ecm_ctx->enb_ue_s1ap_id;

  ctx_rel_cmd->Cause.choice_type = LIBLTE_S1AP_CAUSE_CHOICE_NAS;
  ctx_rel_cmd->Cause.choice.nas.ext = false;
  ctx_rel_cmd->Cause.choice.nas.e =  LIBLTE_S1AP_CAUSENAS_NORMAL_RELEASE;

  LIBLTE_ERROR_ENUM err = liblte_s1ap_pack_s1ap_pdu(&pdu, (LIBLTE_BYTE_MSG_STRUCT*)reply_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Could not pack Initial Context Setup Request Message\n");
    return false;
  }
  //Send Reply to eNB 
  int n_sent = sctp_send(s1mme,reply_buffer->msg, reply_buffer->N_bytes, &ecm_ctx->enb_sri, 0);
  if(n_sent == -1)
  {
    m_s1ap_log->error("Failed to send Initial Context Setup Request\n");
    return false;
  }


  return true;
}

bool
s1ap_ctx_mngmt_proc::handle_ue_context_release_complete(LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMPLETE_STRUCT *rel_comp)
{
  /*
    typedef struct{
    bool                                                         ext;
    LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
    LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
    LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
    bool                                                         CriticalityDiagnostics_present;
    LIBLTE_S1AP_USERLOCATIONINFORMATION_STRUCT                   UserLocationInformation;
    bool                                                         UserLocationInformation_present;
    }LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMPLETE_STRUCT;
   */

  uint32_t mme_ue_s1ap_id = rel_comp->MME_UE_S1AP_ID.MME_UE_S1AP_ID;
  m_s1ap_log->info("Received UE Context Release Complete. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
  m_s1ap_log->console("Received UE Context Release Complete. MME-UE S1AP Id %d\n", mme_ue_s1ap_id);

  ue_ctx_t * ue_ctx = m_s1ap->find_ue_ctx_from_mme_ue_s1ap_id(mme_ue_s1ap_id);
  if(ue_ctx == NULL)
  {
    m_s1ap_log->info("No UE context to release found. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
    m_s1ap_log->console("No UE context to release found. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
    return false;
  }
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx; 

  //Delete user plane context at the SPGW (but keep GTP-C connection).
  if (ecm_ctx->state == ECM_STATE_CONNECTED)
  {
    //There are active E-RABs, send release access mearers request
    m_s1ap_log->console("There are active E-RABs, send release access mearers request");
    m_s1ap_log->info("There are active E-RABs, send release access mearers request");
    m_mme_gtpc->send_release_access_bearers_request(ecm_ctx->imsi);
    //The handle_releease_access_bearers_response function will make sure to mark E-RABS DEACTIVATED
    //It will release the UEs downstream S1-u and keep the upstream S1-U connection active.
  }
  else
  {
    //No ECM Context to release
    m_s1ap_log->info("UE is not ECM connected. No need to release S1-U. MME UE S1AP Id %d\n", mme_ue_s1ap_id);
    m_s1ap_log->console("UE is not ECM connected. No need to release S1-U. MME UE S1AP Id %d\n", mme_ue_s1ap_id);
    //Make sure E-RABS are merked as DEACTIVATED.
    for(int i=0;i<MAX_ERABS_PER_UE;i++)
    {
      ecm_ctx->erabs_ctx[i].state = ERAB_DEACTIVATED;
    }
  }

  //Delete UE context
  m_s1ap->release_ue_ecm_ctx(ue_ctx->ecm_ctx.mme_ue_s1ap_id);
  m_s1ap_log->info("UE Context Release Completed.\n");
  m_s1ap_log->console("UE Context Release Completed.\n");
  return true;
}


} //namespace srsepc
