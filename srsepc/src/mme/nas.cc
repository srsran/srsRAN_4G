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
#include <inttypes.h> // for printing uint64_t
#include "srsepc/hdr/mme/s1ap.h"
#include "srsepc/hdr/mme/s1ap_nas_transport.h"
#include "srslte/common/security.h"
#include "srslte/common/liblte_security.h"

namespace srsepc{


nas::nas() {}

nas::~nas() {}

/*******************************
 *
 * Handle UE Initiating Messages
 *
 ********************************/

//FIXME Move UE Initiating mesages from s1ap_nas_transport

/*
 *
 * Handle Uplink NAS Transport message
 *
 */

//FIXME re-factor to reduce code duplication for messages that can be both initiating messages and uplink NAS messages
bool
nas::handle_nas_detach_request(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{

  m_s1ap_log->console("Detach request -- IMSI %015lu\n", ue_ctx->emm_ctx.imsi);
  m_s1ap_log->info("Detach request -- IMSI %015lu\n", ue_ctx->emm_ctx.imsi);
  LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT detach_req;

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_detach_request_msg((LIBLTE_BYTE_MSG_STRUCT*) nas_msg, &detach_req);
  if(err !=LIBLTE_SUCCESS)
    {
      m_s1ap_log->error("Could not unpack detach request\n");
      return false;
    }

  m_mme_gtpc->send_delete_session_request(ue_ctx->emm_ctx.imsi);
  ue_ctx->emm_ctx.state = EMM_STATE_DEREGISTERED;
  if(ue_ctx->ecm_ctx.mme_ue_s1ap_id!=0)
    {
      m_s1ap->m_s1ap_ctx_mngmt_proc->send_ue_context_release_command(&ue_ctx->ecm_ctx, reply_msg);
    }
  return true;
}

bool
nas::handle_nas_authentication_response(srslte::byte_buffer_t *nas_msg, ue_ctx_t *ue_ctx, srslte::byte_buffer_t *reply_buffer, bool* reply_flag)
{

  LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT auth_resp;
  bool ue_valid=true;

  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;
  m_s1ap_log->console("Authentication Response -- IMSI %015lu\n", emm_ctx->imsi);

  //Get NAS authentication response
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_authentication_response_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &auth_resp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }
  m_s1ap_log->console("Authentication Response -- RES 0x%x%x%x%x%x%x%x%x\n",
                      auth_resp.res[0], auth_resp.res[1], auth_resp.res[2], auth_resp.res[3],
                      auth_resp.res[4], auth_resp.res[5], auth_resp.res[6], auth_resp.res[7]);
  m_s1ap_log->info("Authentication Response -- RES 0x%x%x%x%x%x%x%x%x\n",
                      auth_resp.res[0], auth_resp.res[1], auth_resp.res[2], auth_resp.res[3],
                      auth_resp.res[4], auth_resp.res[5], auth_resp.res[6], auth_resp.res[7]);

  for(int i=0; i<8;i++){
    if( auth_resp.res[i] != emm_ctx->security_ctxt.xres[i] ) {
      ue_valid = false;
    }
  }

  if(!ue_valid) {
    m_s1ap_log->info_hex(emm_ctx->security_ctxt.xres,8, "XRES");
    m_s1ap_log->console("UE Authentication Rejected.\n");
    m_s1ap_log->warning("UE Authentication Rejected.\n");

    //Send back Athentication Reject
    pack_authentication_reject(reply_buffer, ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id);
    *reply_flag = true;
    m_s1ap_log->console("Downlink NAS: Sending Authentication Reject.\n");
    return false;
  } else {
    m_s1ap_log->console("UE Authentication Accepted.\n");
    m_s1ap_log->info("UE Authentication Accepted.\n");

    //Send Security Mode Command
    emm_ctx->security_ctxt.ul_nas_count = 0; // Reset the NAS uplink counter for the right key k_enb derivation
    pack_security_mode_command(reply_buffer, emm_ctx, ecm_ctx);
    *reply_flag = true;
    m_s1ap_log->console("Downlink NAS: Sending NAS Security Mode Command.\n");
  }
  return true;
}

bool
nas::handle_nas_security_mode_complete(srslte::byte_buffer_t *nas_msg, ue_ctx_t *ue_ctx, srslte::byte_buffer_t *reply_buffer, bool *reply_flag)
{

  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;

  LIBLTE_MME_SECURITY_MODE_COMPLETE_MSG_STRUCT sm_comp;

  //Get NAS authentication response
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_security_mode_complete_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &sm_comp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  m_s1ap_log->info("Security Mode Command Complete -- IMSI: %lu\n", emm_ctx->imsi);
  m_s1ap_log->console("Security Mode Command Complete -- IMSI: %lu\n", emm_ctx->imsi);
  if (ecm_ctx->eit == true) {
    pack_esm_information_request(reply_buffer, emm_ctx, ecm_ctx);
    m_s1ap_log->console("Sending ESM information request\n");
    m_s1ap_log->info_hex(reply_buffer->msg, reply_buffer->N_bytes, "Sending ESM information request\n");
    *reply_flag = true;
  } else {
    //Get subscriber info from HSS
    uint8_t default_bearer=5;
    m_hss->gen_update_loc_answer(emm_ctx->imsi,&ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);
    m_s1ap_log->debug("Getting subscription information -- QCI %d\n", ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);
    m_s1ap_log->console("Getting subscription information -- QCI %d\n", ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);
    //FIXME The packging of GTP-C messages is not ready.
    //This means that GTP-U tunnels are created with function calls, as opposed to GTP-C.
    m_mme_gtpc->send_create_session_request(emm_ctx->imsi);
    *reply_flag = false; //No reply needed
  }
  return true;
}


bool
nas::handle_nas_attach_complete(srslte::byte_buffer_t *nas_msg, ue_ctx_t *ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{

  LIBLTE_MME_ATTACH_COMPLETE_MSG_STRUCT attach_comp;
  uint8_t pd, msg_type;
  srslte::byte_buffer_t *esm_msg = m_pool->allocate();
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT act_bearer;

  m_s1ap_log->info_hex(nas_msg->msg, nas_msg->N_bytes, "NAS Attach complete");

  //Get NAS authentication response
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_attach_complete_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &attach_comp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  err = liblte_mme_unpack_activate_default_eps_bearer_context_accept_msg( (LIBLTE_BYTE_MSG_STRUCT *) &attach_comp.esm_msg, &act_bearer);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking Activate EPS Bearer Context Accept Msg. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;


  m_s1ap_log->console("Unpacked Attached Complete Message. IMSI %" PRIu64 "\n", emm_ctx->imsi);
  m_s1ap_log->console("Unpacked Activate Default EPS Bearer message. EPS Bearer id %d\n",act_bearer.eps_bearer_id);
  if (act_bearer.eps_bearer_id < 5 || act_bearer.eps_bearer_id > 15) {
    m_s1ap_log->error("EPS Bearer ID out of range\n");
    return false;
  }
  if(emm_ctx->state == EMM_STATE_DEREGISTERED)
  {
    //Attach requested from attach request
    m_mme_gtpc->send_modify_bearer_request(emm_ctx->imsi, &ecm_ctx->erabs_ctx[act_bearer.eps_bearer_id]);
    //Send reply to eNB
    m_s1ap_log->console("Packing EMM Information\n");
    *reply_flag = pack_emm_information(ue_ctx, reply_msg);
    m_s1ap_log->console("Sending EMM Information, bytes %d\n",reply_msg->N_bytes);
    m_s1ap_log->info("Sending EMM Information\n");
  }
  emm_ctx->state = EMM_STATE_REGISTERED;
  return true;
}

bool
nas::handle_esm_information_response(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{
  LIBLTE_MME_ESM_INFORMATION_RESPONSE_MSG_STRUCT esm_info_resp;

  //Get NAS authentication response
  LIBLTE_ERROR_ENUM err = srslte_mme_unpack_esm_information_response_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &esm_info_resp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  m_s1ap_log->info("ESM Info: EPS bearer id %d\n",esm_info_resp.eps_bearer_id);
  if(esm_info_resp.apn_present){
    m_s1ap_log->info("ESM Info: APN %s\n",esm_info_resp.apn.apn);
    m_s1ap_log->console("ESM Info: APN %s\n",esm_info_resp.apn.apn);
  }
  if(esm_info_resp.protocol_cnfg_opts_present){
    m_s1ap_log->info("ESM Info: %d Protocol Configuration Options\n",esm_info_resp.protocol_cnfg_opts.N_opts);
    m_s1ap_log->console("ESM Info: %d Protocol Configuration Options\n",esm_info_resp.protocol_cnfg_opts.N_opts);
  }

  //Get subscriber info from HSS
  uint8_t default_bearer=5;
  m_hss->gen_update_loc_answer(ue_ctx->emm_ctx.imsi,&ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);
  m_s1ap_log->debug("Getting subscription information -- QCI %d\n", ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);
  m_s1ap_log->console("Getting subscription information -- QCI %d\n", ue_ctx->ecm_ctx.erabs_ctx[default_bearer].qci);

  //FIXME The packging of GTP-C messages is not ready.
  //This means that GTP-U tunnels are created with function calls, as opposed to GTP-C.
  m_mme_gtpc->send_create_session_request(ue_ctx->emm_ctx.imsi);
  return true;
}

bool
nas::handle_identity_response(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{
  uint8_t     autn[16];
  uint8_t     rand[16];
  uint8_t     xres[8];

  LIBLTE_MME_ID_RESPONSE_MSG_STRUCT id_resp;
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_identity_response_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &id_resp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS identity response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  uint64_t imsi = 0;
  for(int i=0;i<=14;i++){
    imsi  += id_resp.mobile_id.imsi[i]*std::pow(10,14-i);
  }

  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;

  m_s1ap_log->info("ID response -- IMSI: %015lu\n", imsi);
  m_s1ap_log->console("ID Response -- IMSI: %015lu\n", imsi);

  //Set UE's context IMSI
  emm_ctx->imsi=imsi;
  ecm_ctx->imsi = imsi;

  //Get Authentication Vectors from HSS
  if (!m_hss->gen_auth_info_answer(imsi, emm_ctx->security_ctxt.k_asme, autn, rand, emm_ctx->security_ctxt.xres)) {
    m_s1ap_log->console("User not found. IMSI %015lu\n",imsi);
    m_s1ap_log->info("User not found. IMSI %015lu\n",imsi);
    return false;
  }
  //Identity reponse from unknown GUTI atach. Assigning new eKSI.
  emm_ctx->security_ctxt.eksi=0;

  //Store UE context im IMSI map
  m_s1ap->add_ue_ctx_to_imsi_map(ue_ctx);

  //Pack NAS Authentication Request in Downlink NAS Transport msg
  pack_authentication_request(reply_msg, ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id, emm_ctx->security_ctxt.eksi, autn, rand);

  //Send reply to eNB
  *reply_flag = true;
   m_s1ap_log->info("Downlink NAS: Sent Authentication Request\n");
   m_s1ap_log->console("Downlink NAS: Sent Authentication Request\n");
  //TODO Start T3460 Timer! 

  return true;
}


bool
nas::handle_tracking_area_update_request(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{

  m_s1ap_log->console("Warning: Tracking Area Update Request messages not handled yet.\n");
  m_s1ap_log->warning("Warning: Tracking Area Update Request messages not handled yet.\n");
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = ue_ctx->ecm_ctx.mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ue_ctx->ecm_ctx.enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;
  //m_s1ap_log->console("Tracking area accept to MME-UE S1AP Id %d\n", ue_ctx->mme_ue_s1ap_id);
  LIBLTE_MME_TRACKING_AREA_UPDATE_ACCEPT_MSG_STRUCT tau_acc;

  //Get decimal MCC and MNC
  uint32_t mcc = 0;
  mcc += 0x000F & m_s1ap->m_s1ap_args.mcc;
  mcc += 10*( (0x00F0 & m_s1ap->m_s1ap_args.mcc) >> 4);
  mcc += 100*( (0x0F00 & m_s1ap->m_s1ap_args.mcc) >> 8);

  uint32_t mnc = 0;
  if( 0xFF00 == (m_s1ap->m_s1ap_args.mnc & 0xFF00 ))
  {
    //Two digit MNC
    mnc += 0x000F & m_s1ap->m_s1ap_args.mnc;
    mnc += 10*((0x00F0 & m_s1ap->m_s1ap_args.mnc) >> 4);
  }
  else
  {
    //Three digit MNC
    mnc += 0x000F & m_s1ap->m_s1ap_args.mnc;
    mnc += 10*((0x00F0 & m_s1ap->m_s1ap_args.mnc) >> 4);
    mnc += 100*((0x0F00 & m_s1ap->m_s1ap_args.mnc) >> 8);
  }

  //T3412 Timer
  tau_acc.t3412_present = true;
  tau_acc.t3412.unit = LIBLTE_MME_GPRS_TIMER_UNIT_1_MINUTE;   // GPRS 1 minute unit
  tau_acc.t3412.value = 30;                                   // 30 minute periodic timer

  //GUTI
  tau_acc.guti_present=true;
  tau_acc.guti.type_of_id = 6; //110 -> GUTI
  tau_acc.guti.guti.mcc = mcc;
  tau_acc.guti.guti.mnc = mnc;
  tau_acc.guti.guti.mme_group_id = m_s1ap->m_s1ap_args.mme_group;
  tau_acc.guti.guti.mme_code = m_s1ap->m_s1ap_args.mme_code;
  tau_acc.guti.guti.m_tmsi = 0xF000;
  m_s1ap_log->debug("Allocated GUTI: MCC %d, MNC %d, MME Group Id %d, MME Code 0x%x, M-TMSI 0x%x\n",
                    tau_acc.guti.guti.mcc,
                    tau_acc.guti.guti.mnc,
                    tau_acc.guti.guti.mme_group_id,
                    tau_acc.guti.guti.mme_code,
                    tau_acc.guti.guti.m_tmsi);

  //Unused Options
  tau_acc.t3402_present = false;
  tau_acc.t3423_present = false;
  tau_acc.equivalent_plmns_present = false;
  tau_acc.emerg_num_list_present = false;
  tau_acc.eps_network_feature_support_present = false;
  tau_acc.additional_update_result_present = false;
  tau_acc.t3412_ext_present = false;

  return true;
}

/************************
 *
 * Security Functions
 *
 ************************/

bool
nas::short_integrity_check(ue_emm_ctx_t *emm_ctx, srslte::byte_buffer_t *pdu)
{
  uint8_t exp_mac[4];
  uint8_t *mac = &pdu->msg[2];
  int i;

  srslte::security_128_eia1(&emm_ctx->security_ctxt.k_nas_int[16],
                     emm_ctx->security_ctxt.ul_nas_count,
                     0,
                     SECURITY_DIRECTION_UPLINK,
                     &pdu->msg[0],
                     2,
                     &exp_mac[0]);

  // Check if expected mac equals the sent mac
  for(i=0; i<2; i++){
    if(exp_mac[i+2] != mac[i]){
      m_s1ap_log->warning("Short integrity check failure. Local: count=%d, [%02x %02x %02x %02x], "
                       "Received: count=%d, [%02x %02x]\n",
                          emm_ctx->security_ctxt.ul_nas_count, exp_mac[0], exp_mac[1], exp_mac[2], exp_mac[3],
                          pdu->msg[1] & 0x1F, mac[0], mac[1]);
      return false;
    }
  }
  m_s1ap_log->info("Integrity check ok. Local: count=%d, Received: count=%d\n",
                emm_ctx->security_ctxt.ul_nas_count, pdu->msg[1] & 0x1F);
  return true;
}


bool
nas::integrity_check(ue_emm_ctx_t *emm_ctx, srslte::byte_buffer_t *pdu)
{
  uint8_t exp_mac[4];
  uint8_t *mac = &pdu->msg[1];
  int i;

  srslte::security_128_eia1(&emm_ctx->security_ctxt.k_nas_int[16],
                     emm_ctx->security_ctxt.ul_nas_count,
                     0,
                     SECURITY_DIRECTION_UPLINK,
                     &pdu->msg[5],
                     pdu->N_bytes-5,
                     &exp_mac[0]);

  // Check if expected mac equals the sent mac
  for(i=0; i<4; i++){
    if(exp_mac[i] != mac[i]){
      m_s1ap_log->warning("Integrity check failure. UL Local: count=%d, [%02x %02x %02x %02x], "
                       "Received: UL count=%d, [%02x %02x %02x %02x]\n",
                       emm_ctx->security_ctxt.ul_nas_count, exp_mac[0], exp_mac[1], exp_mac[2], exp_mac[3],
                       pdu->msg[5], mac[0], mac[1], mac[2], mac[3]);
      return false;
    }
  }
  m_s1ap_log->info("Integrity check ok. Local: count=%d, Received: count=%d\n",
                emm_ctx->security_ctxt.ul_nas_count, pdu->msg[5]);
    return true;
}


bool
nas::handle_authentication_failure(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{
  uint8_t     autn[16];
  uint8_t     rand[16];
  uint8_t     xres[8];

  LIBLTE_MME_AUTHENTICATION_FAILURE_MSG_STRUCT auth_fail;
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_authentication_failure_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &auth_fail);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication failure. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;

  switch(auth_fail.emm_cause){
    case 20:
    m_s1ap_log->console("MAC code failure\n");
    m_s1ap_log->info("MAC code failure\n");
    break;
    case 26:
    m_s1ap_log->console("Non-EPS authentication unacceptable\n");
    m_s1ap_log->info("Non-EPS authentication unacceptable\n");
    break;
    case 21:
    m_s1ap_log->console("Authentication Failure -- Synchronization Failure\n");
    m_s1ap_log->info("Authentication Failure -- Synchronization Failure\n");
    if(auth_fail.auth_fail_param_present == false){
      m_s1ap_log->error("Missing fail parameter\n");
      return false;
    }
    if(!m_hss->resync_sqn(emm_ctx->imsi, auth_fail.auth_fail_param))
    {
      m_s1ap_log->console("Resynchronization failed. IMSI %015lu\n", emm_ctx->imsi);
      m_s1ap_log->info("Resynchronization failed. IMSI %015lu\n", emm_ctx->imsi);
      return false;
    }
    //Get Authentication Vectors from HSS
    if(!m_hss->gen_auth_info_answer(emm_ctx->imsi, emm_ctx->security_ctxt.k_asme, autn, rand, emm_ctx->security_ctxt.xres))
    {
      m_s1ap_log->console("User not found. IMSI %015lu\n", emm_ctx->imsi);
      m_s1ap_log->info("User not found. IMSI %015lu\n", emm_ctx->imsi);
      return false;
    }
    //Making sure eKSI is different from previous eKSI.
    emm_ctx->security_ctxt.eksi = (emm_ctx->security_ctxt.eksi+1)%6;

    //Pack NAS Authentication Request in Downlink NAS Transport msg
    pack_authentication_request(reply_msg, ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id, emm_ctx->security_ctxt.eksi, autn, rand);

    //Send reply to eNB
    *reply_flag = true;
    m_s1ap_log->info("Downlink NAS: Sent Authentication Request\n");
    m_s1ap_log->console("Downlink NAS: Sent Authentication Request\n");
    //TODO Start T3460 Timer! 

    break;
  }
  return true;
}

/*Packing/Unpacking helper functions*/
bool
nas::pack_authentication_request(srslte::byte_buffer_t *reply_msg, uint32_t enb_ue_s1ap_id, uint32_t next_mme_ue_s1ap_id, uint8_t eksi, uint8_t *autn, uint8_t *rand)
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = next_mme_ue_s1ap_id;//FIXME Change name
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  //Pack NAS PDU 
  LIBLTE_MME_AUTHENTICATION_REQUEST_MSG_STRUCT auth_req;
  memcpy(auth_req.autn , autn, 16);
  memcpy(auth_req.rand, rand, 16);
  auth_req.nas_ksi.tsc_flag=LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
  auth_req.nas_ksi.nas_ksi = eksi;

  LIBLTE_ERROR_ENUM err = liblte_mme_pack_authentication_request_msg(&auth_req, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_s1ap_log->error("Error packing Authentication Request\n");
    m_s1ap_log->console("Error packing Authentication Request\n");
    return false;
  }

  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if (err != LIBLTE_SUCCESS) {
    m_s1ap_log->error("Error packing Authentication Request\n");
    m_s1ap_log->console("Error packing Authentication Request\n");
    return false;
  }

  m_pool->deallocate(nas_buffer);

  return true;
}

bool
nas::pack_authentication_reject(srslte::byte_buffer_t *reply_msg, uint32_t enb_ue_s1ap_id, uint32_t mme_ue_s1ap_id)
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = mme_ue_s1ap_id;//FIXME Change name
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  LIBLTE_MME_AUTHENTICATION_REJECT_MSG_STRUCT auth_rej;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_authentication_reject_msg(&auth_rej, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Authentication Reject\n");
    m_s1ap_log->console("Error packing Authentication Reject\n");
    return false;
  }

  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->error("Error packing Dw NAS Transport: Authentication Reject\n");
    m_s1ap_log->console("Error packing Downlink NAS Transport: Authentication Reject\n");
    return false;
  } 

  m_pool->deallocate(nas_buffer);
  return true;
}

bool
nas::unpack_authentication_response( LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT *ul_xport,
                                     LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT *auth_resp )
{

  /*Get NAS Authentiation Response Message*/
  uint8_t pd, msg_type;
  srslte::byte_buffer_t *nas_msg = m_pool->allocate();

  memcpy(nas_msg->msg, &ul_xport->NAS_PDU.buffer, ul_xport->NAS_PDU.n_octets);
  nas_msg->N_bytes = ul_xport->NAS_PDU.n_octets;
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &pd, &msg_type);

  if(msg_type!=LIBLTE_MME_MSG_TYPE_AUTHENTICATION_RESPONSE){
    m_s1ap_log->error("Error unpacking NAS authentication response\n");
    return false;
  }

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_authentication_response_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, auth_resp);
  if(err != LIBLTE_SUCCESS){
    m_s1ap_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  m_pool->deallocate(nas_msg);
  return true;
}

bool
nas::pack_security_mode_command(srslte::byte_buffer_t *reply_msg, ue_emm_ctx_t *ue_emm_ctx, ue_ecm_ctx_t *ue_ecm_ctx)
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = ue_ecm_ctx->mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ue_ecm_ctx->enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  //Pack NAS PDU
  LIBLTE_MME_SECURITY_MODE_COMMAND_MSG_STRUCT sm_cmd;

  sm_cmd.selected_nas_sec_algs.type_of_eea = LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_EEA0;
  sm_cmd.selected_nas_sec_algs.type_of_eia = LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_128_EIA1;

  sm_cmd.nas_ksi.tsc_flag=LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
  sm_cmd.nas_ksi.nas_ksi=ue_emm_ctx->security_ctxt.eksi;

  //Replay UE security cap
  memcpy(sm_cmd.ue_security_cap.eea,ue_emm_ctx->security_ctxt.ue_network_cap.eea,8*sizeof(bool));
  memcpy(sm_cmd.ue_security_cap.eia,ue_emm_ctx->security_ctxt.ue_network_cap.eia,8*sizeof(bool));
  sm_cmd.ue_security_cap.uea_present = ue_emm_ctx->security_ctxt.ue_network_cap.uea_present;
  memcpy(sm_cmd.ue_security_cap.uea,ue_emm_ctx->security_ctxt.ue_network_cap.uea,8*sizeof(bool));
  sm_cmd.ue_security_cap.uia_present = ue_emm_ctx->security_ctxt.ue_network_cap.uia_present;
  memcpy(sm_cmd.ue_security_cap.uia,ue_emm_ctx->security_ctxt.ue_network_cap.uia,8*sizeof(bool));
  sm_cmd.ue_security_cap.gea_present = ue_emm_ctx->security_ctxt.ms_network_cap_present;
  memcpy(sm_cmd.ue_security_cap.gea,ue_emm_ctx->security_ctxt.ms_network_cap.gea,8*sizeof(bool));

  sm_cmd.imeisv_req_present=false;
  sm_cmd.nonce_ue_present=false;
  sm_cmd.nonce_mme_present=false;

  uint8_t  sec_hdr_type=3;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_security_mode_command_msg(&sm_cmd,sec_hdr_type, ue_emm_ctx->security_ctxt.dl_nas_count,(LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_s1ap_log->console("Error packing Authentication Request\n");
    return false;
  }

  //Generate EPS security context
  uint8_t mac[4];
  srslte::security_generate_k_nas( ue_emm_ctx->security_ctxt.k_asme,
                           srslte::CIPHERING_ALGORITHM_ID_EEA0,
                           srslte::INTEGRITY_ALGORITHM_ID_128_EIA1,
                           ue_emm_ctx->security_ctxt.k_nas_enc,
                           ue_emm_ctx->security_ctxt.k_nas_int
                         );

  m_s1ap_log->info_hex(ue_emm_ctx->security_ctxt.k_nas_enc, 32, "Key NAS Encryption (k_nas_enc)\n");
  m_s1ap_log->info_hex(ue_emm_ctx->security_ctxt.k_nas_int, 32, "Key NAS Integrity (k_nas_int)\n");

  uint8_t key_enb[32];
  srslte::security_generate_k_enb(ue_emm_ctx->security_ctxt.k_asme, ue_emm_ctx->security_ctxt.ul_nas_count, ue_emm_ctx->security_ctxt.k_enb);
  m_s1ap_log->info("Generating KeNB with UL NAS COUNT: %d\n", ue_emm_ctx->security_ctxt.ul_nas_count);
  m_s1ap_log->console("Generating KeNB with UL NAS COUNT: %d\n", ue_emm_ctx->security_ctxt.ul_nas_count);
  m_s1ap_log->info_hex(ue_emm_ctx->security_ctxt.k_enb, 32, "Key eNodeB (k_enb)\n");
  //Generate MAC for integrity protection
  //FIXME Write wrapper to support EIA1, EIA2, etc.
  srslte::security_128_eia1 (&ue_emm_ctx->security_ctxt.k_nas_int[16],
                     ue_emm_ctx->security_ctxt.dl_nas_count,
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
    m_s1ap_log->console("Error packing Authentication Request\n");
    return false;
  }
  m_s1ap_log->debug_hex(reply_msg->msg, reply_msg->N_bytes, "Security Mode Command: ");
  m_pool->deallocate(nas_buffer);
  return true;
}

bool
nas::pack_esm_information_request(srslte::byte_buffer_t *reply_msg, ue_emm_ctx_t *ue_emm_ctx, ue_ecm_ctx_t *ue_ecm_ctx)
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = ue_ecm_ctx->mme_ue_s1ap_id;//FIXME Change name
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ue_ecm_ctx->enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  LIBLTE_MME_ESM_INFORMATION_REQUEST_MSG_STRUCT esm_info_req;
  esm_info_req.eps_bearer_id = 0;
  esm_info_req.proc_transaction_id = ue_emm_ctx->procedure_transaction_id;

  uint8_t sec_hdr_type = LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED;

  ue_emm_ctx->security_ctxt.dl_nas_count++;
  LIBLTE_ERROR_ENUM err = srslte_mme_pack_esm_information_request_msg(&esm_info_req, sec_hdr_type,ue_emm_ctx->security_ctxt.dl_nas_count,(LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_s1ap_log->error("Error packing ESM information request\n");
    m_s1ap_log->console("Error packing ESM information request\n");
    return false;
  }

  uint8_t mac[4];
  srslte::security_128_eia1 (&ue_emm_ctx->security_ctxt.k_nas_int[16],
                             ue_emm_ctx->security_ctxt.dl_nas_count,
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
    m_s1ap_log->error("Error packing Dw NAS Transport: Authentication Reject\n");
    m_s1ap_log->console("Error packing Downlink NAS Transport: Authentication Reject\n");
    return false;
  }

  m_pool->deallocate(nas_buffer);
  return true;
}

bool
nas::pack_attach_accept(LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT *erab_ctxt, struct srslte::gtpc_pdn_address_allocation_ie *paa, srslte::byte_buffer_t *nas_buffer)
{
  m_nas_log->info("Packing Attach Accept\n");
  LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT attach_accept;
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT act_def_eps_bearer_context_req;

  //Get decimal MCC and MNC
  uint32_t mcc = 0;
  mcc += 0x000F & m_s1ap->m_s1ap_args.mcc;
  mcc += 10*( (0x00F0 & m_s1ap->m_s1ap_args.mcc) >> 4);
  mcc += 100*( (0x0F00 & m_s1ap->m_s1ap_args.mcc) >> 8);

  uint32_t mnc = 0;
  if( 0xFF00 == (m_s1ap->m_s1ap_args.mnc & 0xFF00 ))
  {
    //Two digit MNC
    mnc += 0x000F & m_s1ap->m_s1ap_args.mnc;
    mnc += 10*((0x00F0 & m_s1ap->m_s1ap_args.mnc) >> 4);
  }
  else
  {
    //Three digit MNC
    mnc += 0x000F & m_s1ap->m_s1ap_args.mnc;
    mnc += 10*((0x00F0 & m_s1ap->m_s1ap_args.mnc) >> 4);
    mnc += 100*((0x0F00 & m_s1ap->m_s1ap_args.mnc) >> 8);
  }

  //Attach accept
  attach_accept.eps_attach_result = ue_emm_ctx->attach_type;

  //FIXME: Set t3412 from config
  attach_accept.t3412.unit = LIBLTE_MME_GPRS_TIMER_UNIT_1_MINUTE;   // GPRS 1 minute unit
  attach_accept.t3412.value = 30;                                    // 30 minute periodic timer
  //FIXME: Set tai_list from config
  attach_accept.tai_list.N_tais = 1;
  attach_accept.tai_list.tai[0].mcc = mcc;
  attach_accept.tai_list.tai[0].mnc = mnc;
  attach_accept.tai_list.tai[0].tac = m_s1ap->m_s1ap_args.tac;

  //Allocate a GUTI ot the UE
  attach_accept.guti_present=true;
  attach_accept.guti.type_of_id = 6; //110 -> GUTI
  attach_accept.guti.guti.mcc = mcc;
  attach_accept.guti.guti.mnc = mnc;
  attach_accept.guti.guti.mme_group_id = m_s1ap->m_s1ap_args.mme_group;
  attach_accept.guti.guti.mme_code = m_s1ap->m_s1ap_args.mme_code;
  attach_accept.guti.guti.m_tmsi = m_s1ap->allocate_m_tmsi(ue_emm_ctx->imsi);
  m_s1ap_log->debug("Allocated GUTI: MCC %d, MNC %d, MME Group Id %d, MME Code 0x%x, M-TMSI 0x%x\n",
                    attach_accept.guti.guti.mcc,
                    attach_accept.guti.guti.mnc,
                    attach_accept.guti.guti.mme_group_id,
                    attach_accept.guti.guti.mme_code,
                    attach_accept.guti.guti.m_tmsi);

  //Set up LAI for combined EPS/IMSI attach
  attach_accept.lai_present=true;
  attach_accept.lai.mcc = mcc;
  attach_accept.lai.mnc = mnc;
  attach_accept.lai.lac = 001;

  attach_accept.ms_id_present=true;
  attach_accept.ms_id.type_of_id = LIBLTE_MME_MOBILE_ID_TYPE_TMSI;
  attach_accept.ms_id.tmsi = attach_accept.guti.guti.m_tmsi;

  //Make sure all unused options are set to false
  attach_accept.emm_cause_present=false;
  attach_accept.t3402_present=false;
  attach_accept.t3423_present=false;
  attach_accept.equivalent_plmns_present=false;
  attach_accept.emerg_num_list_present=false;
  attach_accept.eps_network_feature_support_present=false;
  attach_accept.additional_update_result_present=false;
  attach_accept.t3412_ext_present=false;

  //Set activate default eps bearer (esm_ms)
  //Set pdn_addr
  act_def_eps_bearer_context_req.pdn_addr.pdn_type = LIBLTE_MME_PDN_TYPE_IPV4;
  memcpy(act_def_eps_bearer_context_req.pdn_addr.addr, &paa->ipv4, 4);
  //Set eps bearer id
  act_def_eps_bearer_context_req.eps_bearer_id = erab_ctxt->e_RAB_ID.E_RAB_ID;
  act_def_eps_bearer_context_req.transaction_id_present = false;
  //set eps_qos
  act_def_eps_bearer_context_req.eps_qos.qci =  erab_ctxt->e_RABlevelQoSParameters.qCI.QCI;
  act_def_eps_bearer_context_req.eps_qos.mbr_ul = 254; //FIXME
  act_def_eps_bearer_context_req.eps_qos.mbr_dl = 254; //FIXME
  act_def_eps_bearer_context_req.eps_qos.mbr_ul_ext = 250; //FIXME
  act_def_eps_bearer_context_req.eps_qos.mbr_dl_ext = 250; //FIXME check

  //set apn
  strncpy(act_def_eps_bearer_context_req.apn.apn, m_s1ap->m_s1ap_args.mme_apn.c_str(), LIBLTE_STRING_LEN);
  act_def_eps_bearer_context_req.proc_transaction_id = ue_emm_ctx->procedure_transaction_id; //FIXME

  //Set DNS server
  act_def_eps_bearer_context_req.protocol_cnfg_opts_present = true;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.N_opts = 1;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].id = 0x0d;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].len = 4;

  struct sockaddr_in dns_addr;
  inet_pton(AF_INET, m_s1ap->m_s1ap_args.dns_addr.c_str(), &(dns_addr.sin_addr));
  memcpy(act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].contents,&dns_addr.sin_addr.s_addr, 4);

  //Make sure all unused options are set to false
  act_def_eps_bearer_context_req.negotiated_qos_present = false;
  act_def_eps_bearer_context_req.llc_sapi_present = false;
  act_def_eps_bearer_context_req.radio_prio_present = false;
  act_def_eps_bearer_context_req.packet_flow_id_present = false;
  act_def_eps_bearer_context_req.apn_ambr_present = false;
  act_def_eps_bearer_context_req.esm_cause_present = false;
  act_def_eps_bearer_context_req.connectivity_type_present = false;

  uint8_t sec_hdr_type = 2;
  ue_emm_ctx->security_ctxt.dl_nas_count++;
  liblte_mme_pack_activate_default_eps_bearer_context_request_msg(&act_def_eps_bearer_context_req, &attach_accept.esm_msg);
  liblte_mme_pack_attach_accept_msg(&attach_accept, sec_hdr_type, ue_emm_ctx->security_ctxt.dl_nas_count, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  //Integrity protect NAS message
  uint8_t mac[4];
  srslte::security_128_eia1 (&ue_emm_ctx->security_ctxt.k_nas_int[16],
                             ue_emm_ctx->security_ctxt.dl_nas_count,
                             0,
                             SECURITY_DIRECTION_DOWNLINK,
                             &nas_buffer->msg[5],
                             nas_buffer->N_bytes - 5,
                             mac
                             );

  memcpy(&nas_buffer->msg[1],mac,4);
  m_s1ap_log->info("Packed Attach Complete\n");

  //Add nas message to context setup request
  erab_ctxt->nAS_PDU_present = true;
  memcpy(erab_ctxt->nAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  erab_ctxt->nAS_PDU.n_octets = nas_buffer->N_bytes;

  return true;
}

bool
nas::pack_identity_request(srslte::byte_buffer_t *reply_msg, uint32_t enb_ue_s1ap_id, uint32_t mme_ue_s1ap_id)
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  LIBLTE_MME_ID_REQUEST_MSG_STRUCT id_req;
  id_req.id_type = LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_identity_request_msg(&id_req, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_s1ap_log->error("Error packing Identity Request\n");
    m_s1ap_log->console("Error packing Identity REquest\n");
    return false;
  }

  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if (err != LIBLTE_SUCCESS) {
    m_s1ap_log->error("Error packing Dw NAS Transport: Authentication Reject\n");
    m_s1ap_log->console("Error packing Downlink NAS Transport: Authentication Reject\n");
    return false;
  }

  m_pool->deallocate(nas_buffer);
  return true;
}

bool
nas::pack_emm_information( ue_ctx_t *ue_ctx, srslte::byte_buffer_t *reply_msg)
{
  srslte::byte_buffer_t *nas_buffer = m_pool->allocate();

  ue_emm_ctx_t *emm_ctx = &ue_ctx->emm_ctx;
  ue_ecm_ctx_t *ecm_ctx = &ue_ctx->ecm_ctx;
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = ecm_ctx->mme_ue_s1ap_id;//FIXME Change name
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ecm_ctx->enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT emm_info;
  emm_info.full_net_name_present = true;
  strncpy(emm_info.full_net_name.name, "Software Radio Systems LTE", LIBLTE_STRING_LEN);
  emm_info.full_net_name.add_ci = LIBLTE_MME_ADD_CI_DONT_ADD;
  emm_info.short_net_name_present = true;
  strncpy(emm_info.short_net_name.name, "srsLTE", LIBLTE_STRING_LEN);
  emm_info.short_net_name.add_ci = LIBLTE_MME_ADD_CI_DONT_ADD;

  emm_info.local_time_zone_present = false;
  emm_info.utc_and_local_time_zone_present = false;
  emm_info.net_dst_present = false;

  uint8_t sec_hdr_type =2;
  emm_ctx->security_ctxt.dl_nas_count++;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_emm_information_msg(&emm_info, sec_hdr_type, emm_ctx->security_ctxt.dl_nas_count, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_s1ap_log->error("Error packing EMM Information\n");
    m_s1ap_log->console("Error packing EMM Information\n");
    return false;
  }

  uint8_t mac[4];
  srslte::security_128_eia1 (&emm_ctx->security_ctxt.k_nas_int[16],
                             emm_ctx->security_ctxt.dl_nas_count,
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
  if (err != LIBLTE_SUCCESS) {
    m_s1ap_log->error("Error packing Dw NAS Transport: EMM Info\n");
    m_s1ap_log->console("Error packing Downlink NAS Transport: EMM Info\n");
    return false;
  }

  m_s1ap_log->info("Packed UE EMM information\n"); 
  return true;
}

bool
s1ap_nas_transport::pack_service_reject(srslte::byte_buffer_t *reply_msg, uint8_t emm_cause, uint32_t enb_ue_s1ap_id)
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = m_s1ap->get_next_mme_ue_s1ap_id();
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;
  LIBLTE_MME_SERVICE_REJECT_MSG_STRUCT service_rej;
  service_rej.t3442_present = true;
  service_rej.t3442.unit = LIBLTE_MME_GPRS_TIMER_DEACTIVATED;
  service_rej.t3442.value = 0;
  service_rej.t3446_present = true;
  service_rej.t3446 = 0;
  service_rej.emm_cause = emm_cause;

  LIBLTE_ERROR_ENUM err = liblte_mme_pack_service_reject_msg(&service_rej, LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS, 0, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_s1ap_log->error("Error packing Service Reject\n");
    m_s1ap_log->console("Error packing Service Reject\n");
    return false;
  }

  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if (err != LIBLTE_SUCCESS) {
    m_s1ap_log->error("Error packing Dw NAS Transport: Service Reject\n");
    m_s1ap_log->console("Error packing Downlink NAS Transport: Service Reject\n");
    return false;
  }
  return true;
}

} //namespace srsepc
