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
#include "srsepc/hdr/mme/s1ap_mngmt_proc.h"

namespace srsepc{

s1ap_mngmt_proc*          s1ap_mngmt_proc::m_instance = NULL;
pthread_mutex_t s1ap_mngmt_proc_instance_mutex = PTHREAD_MUTEX_INITIALIZER;


s1ap_mngmt_proc::s1ap_mngmt_proc()
{
}

s1ap_mngmt_proc::~s1ap_mngmt_proc()
{
}

s1ap_mngmt_proc*
s1ap_mngmt_proc::get_instance(void)
{
  pthread_mutex_lock(&s1ap_mngmt_proc_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new s1ap_mngmt_proc();
  }
  pthread_mutex_unlock(&s1ap_mngmt_proc_instance_mutex);
  return(m_instance);
}

void
s1ap_mngmt_proc::cleanup(void)
{
  pthread_mutex_lock(&s1ap_mngmt_proc_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&s1ap_mngmt_proc_instance_mutex);
}

void
s1ap_mngmt_proc::init(void)
{
  m_s1ap     = s1ap::get_instance();
  m_s1ap_log = m_s1ap->m_s1ap_log;
  m_s1mme = m_s1ap->get_s1_mme();
  m_s1ap_args = m_s1ap->m_s1ap_args;
}

bool
s1ap_mngmt_proc::handle_s1_setup_request(LIBLTE_S1AP_MESSAGE_S1SETUPREQUEST_STRUCT *msg, struct sctp_sndrcvinfo *enb_sri, srslte::byte_buffer_t *reply_buffer, bool *reply_flag)
{

  enb_ctx_t enb_ctx;
  LIBLTE_S1AP_S1AP_PDU_STRUCT reply_pdu;

  if(!unpack_s1_setup_request(msg, &enb_ctx))
  {
    m_s1ap_log->error("Malformed S1 Setup Request\n");
    return false;
  }

  //Log S1 Setup Request Info
  m_s1ap_log->console("Received S1 Setup Request.\n");
  m_s1ap->print_enb_ctx_info(std::string("S1 Setup Request"),enb_ctx);

  //Check matching PLMNs
  if(enb_ctx.plmn!=m_s1ap->get_plmn()){

    m_s1ap_log->console("Sending S1 Setup Failure - Unkown PLMN\n");
    m_s1ap_log->warning("Sending S1 Setup Failure - Unkown PLMN\n");
    pack_s1_setup_failure(LIBLTE_S1AP_CAUSEMISC_UNKNOWN_PLMN,reply_buffer);
  }
  else{
    enb_ctx_t *enb_ptr = m_s1ap->find_enb_ctx(enb_ctx.enb_id);
    if(enb_ptr != NULL)
    {
      //eNB already registered
      //TODO replace enb_ctx
      m_s1ap_log->warning("eNB Already registered\n");
    }
    else
    {
      //new eNB
      m_s1ap->add_new_enb_ctx(enb_ctx,enb_sri);
    }

    pack_s1_setup_response(m_s1ap_args, reply_buffer);
    m_s1ap_log->console("Sending S1 Setup Response\n");
    m_s1ap_log->info("Sending S1 Setup Response\n");
  }

  *reply_flag = true;
  return true;
}


/*
 * Packing/Unpacking helper functions.
 */
bool 
s1ap_mngmt_proc::unpack_s1_setup_request(LIBLTE_S1AP_MESSAGE_S1SETUPREQUEST_STRUCT *msg, enb_ctx_t* enb_ctx)
{
  
  uint8_t enb_id_bits[32];
  uint32_t plmn = 0;  
  uint16_t tac, bplmn;

  uint32_t tmp32=0;
  //eNB Name
  enb_ctx->enb_name_present=msg->eNBname_present;
  if(msg->eNBname_present)
  {
    bzero(enb_ctx->enb_name,sizeof(enb_ctx->enb_name));
    memcpy(enb_ctx->enb_name,&msg->eNBname.buffer,msg->eNBname.n_octets);
  }
  //eNB Id
  bzero(enb_id_bits,sizeof(enb_id_bits));
  memcpy(&enb_id_bits[32-LIBLTE_S1AP_MACROENB_ID_BIT_STRING_LEN], msg->Global_ENB_ID.eNB_ID.choice.macroENB_ID.buffer, LIBLTE_S1AP_MACROENB_ID_BIT_STRING_LEN);
  liblte_pack(enb_id_bits, 32, (uint8_t*) &tmp32);
  enb_ctx->enb_id=ntohl(tmp32);
  
  //PLMN Id
  ((uint8_t*)&plmn)[1] = msg->Global_ENB_ID.pLMNidentity.buffer[0];
  ((uint8_t*)&plmn)[2] = msg->Global_ENB_ID.pLMNidentity.buffer[1];
  ((uint8_t*)&plmn)[3] = msg->Global_ENB_ID.pLMNidentity.buffer[2];

  enb_ctx->plmn = ntohl(plmn);
  srslte::s1ap_plmn_to_mccmnc(enb_ctx->plmn, &enb_ctx->mcc, &enb_ctx->mnc);

  //SupportedTAs
  enb_ctx->nof_supported_ta=msg->SupportedTAs.len;
  for(uint16_t i=0; i<msg->SupportedTAs.len; i++)
  {
    //TAC
    ((uint8_t*)&enb_ctx->tac[i])[0] = msg->SupportedTAs.buffer[i].tAC.buffer[0];
    ((uint8_t*)&enb_ctx->tac[i])[1] = msg->SupportedTAs.buffer[i].tAC.buffer[1];
    enb_ctx->tac[i]=ntohs(enb_ctx->tac[i]);
    enb_ctx->nof_supported_bplmns[i]=msg->SupportedTAs.buffer[i].broadcastPLMNs.len;
    for (uint16_t j=0; j<msg->SupportedTAs.buffer[i].broadcastPLMNs.len; j++)
    {
      //BPLMNs
      ((uint8_t*)&enb_ctx->bplmns[i][j])[1] = msg->SupportedTAs.buffer[i].broadcastPLMNs.buffer[j].buffer[0];
      ((uint8_t*)&enb_ctx->bplmns[i][j])[2] = msg->SupportedTAs.buffer[i].broadcastPLMNs.buffer[j].buffer[1];
      ((uint8_t*)&enb_ctx->bplmns[i][j])[3] = msg->SupportedTAs.buffer[i].broadcastPLMNs.buffer[j].buffer[2];

      enb_ctx->bplmns[i][j] = ntohl(enb_ctx->bplmns[i][j]);
    }
  }

  //Default Paging DRX
  enb_ctx->drx = msg->DefaultPagingDRX.e;

  return true;
}

bool
s1ap_mngmt_proc::pack_s1_setup_failure(LIBLTE_S1AP_CAUSEMISC_ENUM cause, srslte::byte_buffer_t *msg)
{
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
  s1_fail->Cause.choice.misc.e=cause;
  
  liblte_s1ap_pack_s1ap_pdu(&pdu, (LIBLTE_BYTE_MSG_STRUCT*)msg);
  return true;
 }


bool
s1ap_mngmt_proc::pack_s1_setup_response(s1ap_args_t s1ap_args, srslte::byte_buffer_t *msg)
{

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
  s1_resp->MMEname.n_octets=s1ap_args.mme_name.length();
  memcpy(s1_resp->MMEname.buffer,s1ap_args.mme_name.c_str(),s1ap_args.mme_name.length());

  //Served GUMEIs
  s1_resp->ServedGUMMEIs.len=1;//TODO Only one served GUMMEI supported
  LIBLTE_S1AP_SERVEDGUMMEISITEM_STRUCT *serv_gummei = &s1_resp->ServedGUMMEIs.buffer[0];

  serv_gummei->ext=false;
  serv_gummei->iE_Extensions_present = false;

  uint32_t plmn=0;
  srslte::s1ap_mccmnc_to_plmn(s1ap_args.mcc, s1ap_args.mnc, &plmn);
  plmn=htonl(plmn);
  serv_gummei->servedPLMNs.len = 1; //Only one PLMN supported
  serv_gummei->servedPLMNs.buffer[0].buffer[0]=((uint8_t*)&plmn)[1];
  serv_gummei->servedPLMNs.buffer[0].buffer[1]=((uint8_t*)&plmn)[2];
  serv_gummei->servedPLMNs.buffer[0].buffer[2]=((uint8_t*)&plmn)[3];

  serv_gummei->servedGroupIDs.len=1; //LIBLTE_S1AP_SERVEDGROUPIDS_STRUCT
  uint16_t tmp=htons(s1ap_args.mme_group);
  serv_gummei->servedGroupIDs.buffer[0].buffer[0]=((uint8_t*)&tmp)[0];
  serv_gummei->servedGroupIDs.buffer[0].buffer[1]=((uint8_t*)&tmp)[1];
 
  serv_gummei->servedMMECs.len=1; //Only one MMEC served
  serv_gummei->servedMMECs.buffer[0].buffer[0]=s1ap_args.mme_code;

  //Relative MME Capacity
  s1_resp->RelativeMMECapacity.RelativeMMECapacity=255;

  //Relay Unsupported
  s1_resp->MMERelaySupportIndicator_present=false;

  s1_resp->CriticalityDiagnostics_present = false;

  liblte_s1ap_pack_s1ap_pdu(&pdu, (LIBLTE_BYTE_MSG_STRUCT*)msg);
  
 return true;
}

} //namespace srsepc
