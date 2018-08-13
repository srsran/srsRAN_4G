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


nas::nas() {
  m_pool = srslte::byte_buffer_pool::get_instance();
}

void
nas::init(s1ap_interface_nas *s1ap,
          gtpc_interface_nas *gtpc,
          hss_interface_nas  *hss,
          srslte::log        *nas_log)
{
  m_s1ap    = s1ap;
  m_gtpc    = gtpc;
  m_hss     = hss;
  m_nas_log = nas_log;
}
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
bool
nas::handle_nas_detach_request(srslte::byte_buffer_t *nas_msg, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{

  m_nas_log->console("Detach request -- IMSI %015lu\n", m_emm_ctx.imsi);
  m_nas_log->info("Detach request -- IMSI %015lu\n", m_emm_ctx.imsi);
  LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT detach_req;

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_detach_request_msg((LIBLTE_BYTE_MSG_STRUCT*) nas_msg, &detach_req);
  if(err !=LIBLTE_SUCCESS) {
      m_nas_log->error("Could not unpack detach request\n");
      return false;
  }

  m_gtpc->send_delete_session_request(m_emm_ctx.imsi);
  m_emm_ctx.state = EMM_STATE_DEREGISTERED;
  if (m_ecm_ctx.mme_ue_s1ap_id!=0) {
      m_s1ap->send_ue_context_release_command(m_ecm_ctx.mme_ue_s1ap_id);
  }
  return true;
}

bool
nas::handle_nas_authentication_response(srslte::byte_buffer_t *nas_msg, srslte::byte_buffer_t *reply_buffer, bool* reply_flag)
{

  LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT auth_resp;
  m_nas_log->console("Authentication Response -- IMSI %015lu\n", m_emm_ctx.imsi);

  //Get NAS authentication response
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_authentication_response_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &auth_resp);
  if(err != LIBLTE_SUCCESS){
    m_nas_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }
  m_nas_log->console("Authentication Response -- RES 0x%x%x%x%x%x%x%x%x\n",
                      auth_resp.res[0], auth_resp.res[1], auth_resp.res[2], auth_resp.res[3],
                      auth_resp.res[4], auth_resp.res[5], auth_resp.res[6], auth_resp.res[7]);
  m_nas_log->info("Authentication Response -- RES 0x%x%x%x%x%x%x%x%x\n",
                      auth_resp.res[0], auth_resp.res[1], auth_resp.res[2], auth_resp.res[3],
                      auth_resp.res[4], auth_resp.res[5], auth_resp.res[6], auth_resp.res[7]);

  bool ue_valid=true;
  for(int i=0; i<8;i++){
    if( auth_resp.res[i] != m_sec_ctx.xres[i] ) {
      ue_valid = false;
    }
  }

  if(!ue_valid) {
    m_nas_log->info_hex(m_sec_ctx.xres,8, "XRES");
    m_nas_log->console("UE Authentication Rejected.\n");
    m_nas_log->warning("UE Authentication Rejected.\n");

    //Send back Athentication Reject
    pack_authentication_reject(reply_buffer);
    *reply_flag = true;
    m_nas_log->console("Downlink NAS: Sending Authentication Reject.\n");
    return false;
  } else {
    m_nas_log->console("UE Authentication Accepted.\n");
    m_nas_log->info("UE Authentication Accepted.\n");

    //Send Security Mode Command
    m_sec_ctx.ul_nas_count = 0; // Reset the NAS uplink counter for the right key k_enb derivation
    pack_security_mode_command(reply_buffer);
    *reply_flag = true;
    m_nas_log->console("Downlink NAS: Sending NAS Security Mode Command.\n");
  }
  return true;
}

bool
nas::handle_nas_security_mode_complete(srslte::byte_buffer_t *nas_msg, srslte::byte_buffer_t *reply_buffer, bool *reply_flag)
{
  LIBLTE_MME_SECURITY_MODE_COMPLETE_MSG_STRUCT sm_comp;

  //Get NAS authentication response
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_security_mode_complete_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &sm_comp);
  if(err != LIBLTE_SUCCESS){
    m_nas_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  m_nas_log->info("Security Mode Command Complete -- IMSI: %lu\n", m_emm_ctx.imsi);
  m_nas_log->console("Security Mode Command Complete -- IMSI: %lu\n", m_emm_ctx.imsi);
  if (m_ecm_ctx.eit == true) {
    pack_esm_information_request(reply_buffer);
    m_nas_log->console("Sending ESM information request\n");
    m_nas_log->info_hex(reply_buffer->msg, reply_buffer->N_bytes, "Sending ESM information request\n");
    *reply_flag = true;
  } else {
    //Get subscriber info from HSS
    uint8_t default_bearer=5;
    m_hss->gen_update_loc_answer(m_emm_ctx.imsi, &m_esm_ctx[default_bearer].qci);
    m_nas_log->debug("Getting subscription information -- QCI %d\n", m_esm_ctx[default_bearer].qci);
    m_nas_log->console("Getting subscription information -- QCI %d\n", m_esm_ctx[default_bearer].qci);
    //FIXME The packging of GTP-C messages is not ready.
    //This means that GTP-U tunnels are created with function calls, as opposed to GTP-C.
    m_gtpc->send_create_session_request(m_emm_ctx.imsi);
    *reply_flag = false; //No reply needed
  }
  return true;
}


bool
nas::handle_nas_attach_complete(srslte::byte_buffer_t *nas_msg, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{

  LIBLTE_MME_ATTACH_COMPLETE_MSG_STRUCT attach_comp;
  uint8_t pd, msg_type;
  srslte::byte_buffer_t *esm_msg = m_pool->allocate();
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT act_bearer;

  m_nas_log->info_hex(nas_msg->msg, nas_msg->N_bytes, "NAS Attach complete");

  //Get NAS authentication response
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_attach_complete_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &attach_comp);
  if(err != LIBLTE_SUCCESS){
    m_nas_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  err = liblte_mme_unpack_activate_default_eps_bearer_context_accept_msg( (LIBLTE_BYTE_MSG_STRUCT *) &attach_comp.esm_msg, &act_bearer);
  if(err != LIBLTE_SUCCESS){
    m_nas_log->error("Error unpacking Activate EPS Bearer Context Accept Msg. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  m_nas_log->console("Unpacked Attached Complete Message. IMSI %" PRIu64 "\n", m_emm_ctx.imsi);
  m_nas_log->console("Unpacked Activate Default EPS Bearer message. EPS Bearer id %d\n",act_bearer.eps_bearer_id);
  if (act_bearer.eps_bearer_id < 5 || act_bearer.eps_bearer_id > 15) {
    m_nas_log->error("EPS Bearer ID out of range\n");
    return false;
  }
  if (m_emm_ctx.state == EMM_STATE_DEREGISTERED) {
    //Attach requested from attach request
    m_gtpc->send_modify_bearer_request(m_emm_ctx.imsi, act_bearer.eps_bearer_id, &m_esm_ctx[act_bearer.eps_bearer_id].enb_fteid);
    //Send reply to eNB
    m_nas_log->console("Packing EMM Information\n");
    *reply_flag = pack_emm_information(reply_msg);
    m_nas_log->console("Sending EMM Information, bytes %d\n",reply_msg->N_bytes);
    m_nas_log->info("Sending EMM Information\n");
  }
  m_emm_ctx.state = EMM_STATE_REGISTERED;
  return true;
}

bool
nas::handle_esm_information_response(srslte::byte_buffer_t *nas_msg, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{
  LIBLTE_MME_ESM_INFORMATION_RESPONSE_MSG_STRUCT esm_info_resp;

  //Get NAS authentication response
  LIBLTE_ERROR_ENUM err = srslte_mme_unpack_esm_information_response_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &esm_info_resp);
  if(err != LIBLTE_SUCCESS){
    m_nas_log->error("Error unpacking NAS authentication response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  m_nas_log->info("ESM Info: EPS bearer id %d\n",esm_info_resp.eps_bearer_id);
  if(esm_info_resp.apn_present){
    m_nas_log->info("ESM Info: APN %s\n",esm_info_resp.apn.apn);
    m_nas_log->console("ESM Info: APN %s\n",esm_info_resp.apn.apn);
  }
  if(esm_info_resp.protocol_cnfg_opts_present){
    m_nas_log->info("ESM Info: %d Protocol Configuration Options\n",esm_info_resp.protocol_cnfg_opts.N_opts);
    m_nas_log->console("ESM Info: %d Protocol Configuration Options\n",esm_info_resp.protocol_cnfg_opts.N_opts);
  }

  //Get subscriber info from HSS
  uint8_t default_bearer=5;
  m_hss->gen_update_loc_answer(m_emm_ctx.imsi, &m_esm_ctx[default_bearer].qci);
  m_nas_log->debug("Getting subscription information -- QCI %d\n", m_esm_ctx[default_bearer].qci);
  m_nas_log->console("Getting subscription information -- QCI %d\n", m_esm_ctx[default_bearer].qci);

  //FIXME The packging of GTP-C messages is not ready.
  //This means that GTP-U tunnels are created with function calls, as opposed to GTP-C.
  m_gtpc->send_create_session_request(m_emm_ctx.imsi);
  return true;
}

bool
nas::handle_identity_response(srslte::byte_buffer_t *nas_msg, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{
  uint8_t     autn[16];
  uint8_t     rand[16];
  uint8_t     xres[8];

  LIBLTE_MME_ID_RESPONSE_MSG_STRUCT id_resp;
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_identity_response_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &id_resp);
  if(err != LIBLTE_SUCCESS){
    m_nas_log->error("Error unpacking NAS identity response. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  uint64_t imsi = 0;
  for(int i=0;i<=14;i++){
    imsi  += id_resp.mobile_id.imsi[i]*std::pow(10,14-i);
  }

  m_nas_log->info("ID response -- IMSI: %015lu\n", imsi);
  m_nas_log->console("ID Response -- IMSI: %015lu\n", imsi);

  //Set UE's IMSI
  m_emm_ctx.imsi=imsi;

  //Get Authentication Vectors from HSS
  if (!m_hss->gen_auth_info_answer(imsi, m_sec_ctx.k_asme, m_sec_ctx.autn, m_sec_ctx.rand, m_sec_ctx.xres)) {
    m_nas_log->console("User not found. IMSI %015lu\n",imsi);
    m_nas_log->info("User not found. IMSI %015lu\n",imsi);
    return false;
  }
  //Identity reponse from unknown GUTI atach. Assigning new eKSI.
  m_sec_ctx.eksi=0;

  //Store UE context im IMSI map
  m_s1ap->add_nas_ctx_to_imsi_map(this);

  //Pack NAS Authentication Request in Downlink NAS Transport msg
  pack_authentication_request(reply_msg);

  //Send reply to eNB
  *reply_flag = true;
   m_nas_log->info("Downlink NAS: Sent Authentication Request\n");
   m_nas_log->console("Downlink NAS: Sent Authentication Request\n");
  //TODO Start T3460 Timer! 

  return true;
}


bool
nas::handle_tracking_area_update_request(srslte::byte_buffer_t *nas_msg, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{

  m_nas_log->console("Warning: Tracking Area Update Request messages not handled yet.\n");
  m_nas_log->warning("Warning: Tracking Area Update Request messages not handled yet.\n");
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = m_ecm_ctx.mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = m_ecm_ctx.enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;
  //m_nas_log->console("Tracking area accept to MME-UE S1AP Id %d\n", ue_ctx->mme_ue_s1ap_id);
  LIBLTE_MME_TRACKING_AREA_UPDATE_ACCEPT_MSG_STRUCT tau_acc;

  //T3412 Timer
  tau_acc.t3412_present = true;
  tau_acc.t3412.unit = LIBLTE_MME_GPRS_TIMER_UNIT_1_MINUTE;   // GPRS 1 minute unit
  tau_acc.t3412.value = 30;                                   // 30 minute periodic timer

  //GUTI
  tau_acc.guti_present=true;
  tau_acc.guti.type_of_id = 6; //110 -> GUTI
  tau_acc.guti.guti.mcc = m_mcc;
  tau_acc.guti.guti.mnc = m_mnc;
  tau_acc.guti.guti.mme_group_id = m_mme_group;
  tau_acc.guti.guti.mme_code = m_mme_code;
  tau_acc.guti.guti.m_tmsi = 0xF000;
  m_nas_log->debug("Allocated GUTI: MCC %d, MNC %d, MME Group Id %d, MME Code 0x%x, M-TMSI 0x%x\n",
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


bool
nas::handle_authentication_failure(srslte::byte_buffer_t *nas_msg, srslte::byte_buffer_t *reply_msg, bool *reply_flag)
{
  uint8_t     autn[16];
  uint8_t     rand[16];
  uint8_t     xres[8];

  LIBLTE_MME_AUTHENTICATION_FAILURE_MSG_STRUCT auth_fail;
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_authentication_failure_msg((LIBLTE_BYTE_MSG_STRUCT *) nas_msg, &auth_fail);
  if(err != LIBLTE_SUCCESS){
    m_nas_log->error("Error unpacking NAS authentication failure. Error: %s\n", liblte_error_text[err]);
    return false;
  }

  switch (auth_fail.emm_cause) {
    case 20:
    m_nas_log->console("MAC code failure\n");
    m_nas_log->info("MAC code failure\n");
    break;
    case 26:
    m_nas_log->console("Non-EPS authentication unacceptable\n");
    m_nas_log->info("Non-EPS authentication unacceptable\n");
    break;
    case 21:
    m_nas_log->console("Authentication Failure -- Synchronization Failure\n");
    m_nas_log->info("Authentication Failure -- Synchronization Failure\n");
    if(auth_fail.auth_fail_param_present == false){
      m_nas_log->error("Missing fail parameter\n");
      return false;
    }
    if (!m_hss->resync_sqn(m_emm_ctx.imsi, auth_fail.auth_fail_param)) {
      m_nas_log->console("Resynchronization failed. IMSI %015lu\n", m_emm_ctx.imsi);
      m_nas_log->info("Resynchronization failed. IMSI %015lu\n", m_emm_ctx.imsi);
      return false;
    }
    //Get Authentication Vectors from HSS
    if (!m_hss->gen_auth_info_answer(m_emm_ctx.imsi, m_sec_ctx.k_asme, autn, rand, m_sec_ctx.xres)) {
      m_nas_log->console("User not found. IMSI %015lu\n", m_emm_ctx.imsi);
      m_nas_log->info("User not found. IMSI %015lu\n", m_emm_ctx.imsi);
      return false;
    }

    //Making sure eKSI is different from previous eKSI.
    m_sec_ctx.eksi = (m_sec_ctx.eksi+1)%6;

    //Pack NAS Authentication Request in Downlink NAS Transport msg
    pack_authentication_request(reply_msg);

    //Send reply to eNB
    *reply_flag = true;
    m_nas_log->info("Downlink NAS: Sent Authentication Request\n");
    m_nas_log->console("Downlink NAS: Sent Authentication Request\n");
    //TODO Start T3460 Timer! 

    break;
  }
  return true;
}

/*Packing/Unpacking helper functions*/
bool
nas::pack_authentication_request(srslte::byte_buffer_t *reply_msg)
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = m_ecm_ctx.mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = m_ecm_ctx.enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  //Pack NAS PDU 
  LIBLTE_MME_AUTHENTICATION_REQUEST_MSG_STRUCT auth_req;
  memcpy(auth_req.autn , m_sec_ctx.autn, 16);
  memcpy(auth_req.rand, m_sec_ctx.rand, 16);
  auth_req.nas_ksi.tsc_flag=LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
  auth_req.nas_ksi.nas_ksi = m_sec_ctx.eksi;

  LIBLTE_ERROR_ENUM err = liblte_mme_pack_authentication_request_msg(&auth_req, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_nas_log->error("Error packing Authentication Request\n");
    m_nas_log->console("Error packing Authentication Request\n");
    return false;
  }

  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if (err != LIBLTE_SUCCESS) {
    m_nas_log->error("Error packing Authentication Request\n");
    m_nas_log->console("Error packing Authentication Request\n");
    return false;
  }

  m_pool->deallocate(nas_buffer);

  return true;
}

bool
nas::pack_authentication_reject(srslte::byte_buffer_t *reply_msg)
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = m_ecm_ctx.mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = m_ecm_ctx.enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  LIBLTE_MME_AUTHENTICATION_REJECT_MSG_STRUCT auth_rej;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_authentication_reject_msg(&auth_rej, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_nas_log->error("Error packing Authentication Reject\n");
    m_nas_log->console("Error packing Authentication Reject\n");
    return false;
  }

  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if (err != LIBLTE_SUCCESS) {
    m_nas_log->error("Error packing Dw NAS Transport: Authentication Reject\n");
    m_nas_log->console("Error packing Downlink NAS Transport: Authentication Reject\n");
    return false;
  }

  m_pool->deallocate(nas_buffer);
  return true;
}

bool
nas::pack_security_mode_command(srslte::byte_buffer_t *reply_msg)
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = m_ecm_ctx.mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = m_ecm_ctx.enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  //Pack NAS PDU
  LIBLTE_MME_SECURITY_MODE_COMMAND_MSG_STRUCT sm_cmd;

  sm_cmd.selected_nas_sec_algs.type_of_eea = LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_EEA0;
  sm_cmd.selected_nas_sec_algs.type_of_eia = LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_128_EIA1;

  sm_cmd.nas_ksi.tsc_flag=LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
  sm_cmd.nas_ksi.nas_ksi=m_sec_ctx.eksi;

  //Replay UE security cap
  memcpy(sm_cmd.ue_security_cap.eea,m_sec_ctx.ue_network_cap.eea,8*sizeof(bool));
  memcpy(sm_cmd.ue_security_cap.eia,m_sec_ctx.ue_network_cap.eia,8*sizeof(bool));

  sm_cmd.ue_security_cap.uea_present = m_sec_ctx.ue_network_cap.uea_present;
  memcpy(sm_cmd.ue_security_cap.uea,m_sec_ctx.ue_network_cap.uea,8*sizeof(bool));

  sm_cmd.ue_security_cap.uia_present = m_sec_ctx.ue_network_cap.uia_present;
  memcpy(sm_cmd.ue_security_cap.uia,m_sec_ctx.ue_network_cap.uia,8*sizeof(bool));

  sm_cmd.ue_security_cap.gea_present = m_sec_ctx.ms_network_cap_present;
  memcpy(sm_cmd.ue_security_cap.gea,m_sec_ctx.ms_network_cap.gea,8*sizeof(bool));

  sm_cmd.imeisv_req_present=false;
  sm_cmd.nonce_ue_present=false;
  sm_cmd.nonce_mme_present=false;

  uint8_t  sec_hdr_type=3;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_security_mode_command_msg(&sm_cmd,sec_hdr_type, m_sec_ctx.dl_nas_count,(LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if(err != LIBLTE_SUCCESS)
  {
    m_nas_log->console("Error packing Authentication Request\n");
    return false;
  }

  //Generate EPS security context
  uint8_t mac[4];
  srslte::security_generate_k_nas( m_sec_ctx.k_asme,
                           srslte::CIPHERING_ALGORITHM_ID_EEA0,
                           srslte::INTEGRITY_ALGORITHM_ID_128_EIA1,
                           m_sec_ctx.k_nas_enc,
                           m_sec_ctx.k_nas_int
                         );

  m_nas_log->info_hex(m_sec_ctx.k_nas_enc, 32, "Key NAS Encryption (k_nas_enc)\n");
  m_nas_log->info_hex(m_sec_ctx.k_nas_int, 32, "Key NAS Integrity (k_nas_int)\n");

  uint8_t key_enb[32];
  srslte::security_generate_k_enb(m_sec_ctx.k_asme, m_sec_ctx.ul_nas_count, m_sec_ctx.k_enb);
  m_nas_log->info("Generating KeNB with UL NAS COUNT: %d\n", m_sec_ctx.ul_nas_count);
  m_nas_log->console("Generating KeNB with UL NAS COUNT: %d\n", m_sec_ctx.ul_nas_count);
  m_nas_log->info_hex(m_sec_ctx.k_enb, 32, "Key eNodeB (k_enb)\n");
  //Generate MAC for integrity protection
  //FIXME Write wrapper to support EIA1, EIA2, etc.
  srslte::security_128_eia1 (&m_sec_ctx.k_nas_int[16],
                     m_sec_ctx.dl_nas_count,
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
    m_nas_log->console("Error packing Authentication Request\n");
    return false;
  }
  m_nas_log->debug_hex(reply_msg->msg, reply_msg->N_bytes, "Security Mode Command: ");
  m_pool->deallocate(nas_buffer);
  return true;
}

bool
nas::pack_esm_information_request(srslte::byte_buffer_t *reply_msg)
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = m_ecm_ctx.mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = m_ecm_ctx.enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  LIBLTE_MME_ESM_INFORMATION_REQUEST_MSG_STRUCT esm_info_req;
  esm_info_req.eps_bearer_id = 0;
  esm_info_req.proc_transaction_id = m_emm_ctx.procedure_transaction_id;

  uint8_t sec_hdr_type = LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED;

  m_sec_ctx.dl_nas_count++;
  LIBLTE_ERROR_ENUM err = srslte_mme_pack_esm_information_request_msg(&esm_info_req, sec_hdr_type, m_sec_ctx.dl_nas_count,(LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_nas_log->error("Error packing ESM information request\n");
    m_nas_log->console("Error packing ESM information request\n");
    return false;
  }

  uint8_t mac[4];
  srslte::security_128_eia1 (&m_sec_ctx.k_nas_int[16],
                             m_sec_ctx.dl_nas_count,
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
  if(err != LIBLTE_SUCCESS) {
    m_nas_log->error("Error packing Dw NAS Transport: Authentication Reject\n");
    m_nas_log->console("Error packing Downlink NAS Transport: Authentication Reject\n");
    return false;
  }

  m_pool->deallocate(nas_buffer);
  return true;
}

bool
nas::pack_attach_accept(srslte::byte_buffer_t *nas_buffer)
{
  m_nas_log->info("Packing Attach Accept\n");
  LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT attach_accept;
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT act_def_eps_bearer_context_req;

  //Attach accept
  attach_accept.eps_attach_result = m_emm_ctx.attach_type;

  //FIXME: Set t3412 from config
  attach_accept.t3412.unit = LIBLTE_MME_GPRS_TIMER_UNIT_1_MINUTE;   // GPRS 1 minute unit
  attach_accept.t3412.value = 30;                                    // 30 minute periodic timer

  //FIXME: Set tai_list from config
  attach_accept.tai_list.N_tais = 1;
  attach_accept.tai_list.tai[0].mcc = m_mcc;
  attach_accept.tai_list.tai[0].mnc = m_mnc;
  attach_accept.tai_list.tai[0].tac = m_tac;

  //Allocate a GUTI ot the UE
  attach_accept.guti_present=true;
  attach_accept.guti.type_of_id = 6; //110 -> GUTI
  attach_accept.guti.guti.mcc = m_mcc;
  attach_accept.guti.guti.mnc = m_mnc;
  attach_accept.guti.guti.mme_group_id = m_mme_group;
  attach_accept.guti.guti.mme_code = m_mme_code;
  attach_accept.guti.guti.m_tmsi = m_s1ap->allocate_m_tmsi(m_emm_ctx.imsi);
  m_nas_log->debug("Allocated GUTI: MCC %d, MNC %d, MME Group Id %d, MME Code 0x%x, M-TMSI 0x%x\n",
                    attach_accept.guti.guti.mcc,
                    attach_accept.guti.guti.mnc,
                    attach_accept.guti.guti.mme_group_id,
                    attach_accept.guti.guti.mme_code,
                    attach_accept.guti.guti.m_tmsi);

  //Set up LAI for combined EPS/IMSI attach
  attach_accept.lai_present=true;
  attach_accept.lai.mcc = m_mcc;
  attach_accept.lai.mnc = m_mnc;
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
  memcpy(act_def_eps_bearer_context_req.pdn_addr.addr, &m_emm_ctx.ue_ip.s_addr, 4);
  //Set eps bearer id
  act_def_eps_bearer_context_req.eps_bearer_id = 5;
  act_def_eps_bearer_context_req.transaction_id_present = false;
  //set eps_qos
  act_def_eps_bearer_context_req.eps_qos.qci =  m_esm_ctx[5].qci;

  //set apn
  strncpy(act_def_eps_bearer_context_req.apn.apn, m_apn.c_str(), LIBLTE_STRING_LEN);
  act_def_eps_bearer_context_req.proc_transaction_id = m_emm_ctx.procedure_transaction_id; //FIXME

  //Set DNS server
  act_def_eps_bearer_context_req.protocol_cnfg_opts_present = true;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.N_opts = 1;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].id = 0x0d;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].len = 4;

  struct sockaddr_in dns_addr;
  inet_pton(AF_INET, m_dns.c_str(), &(dns_addr.sin_addr));
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
  m_sec_ctx.dl_nas_count++;
  liblte_mme_pack_activate_default_eps_bearer_context_request_msg(&act_def_eps_bearer_context_req, &attach_accept.esm_msg);
  liblte_mme_pack_attach_accept_msg(&attach_accept, sec_hdr_type, m_sec_ctx.dl_nas_count, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  //Integrity protect NAS message
  uint8_t mac[4];
  srslte::security_128_eia1 (&m_sec_ctx.k_nas_int[16],
                             m_sec_ctx.dl_nas_count,
                             0,
                             SECURITY_DIRECTION_DOWNLINK,
                             &nas_buffer->msg[5],
                             nas_buffer->N_bytes - 5,
                             mac
                             );

  memcpy(&nas_buffer->msg[1],mac,4);
  m_nas_log->info("Packed Attach Complete\n");

  //Add nas message to context setup request
  //erab_ctxt->nAS_PDU_present = true;
  //memcpy(erab_ctxt->nAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  //erab_ctxt->nAS_PDU.n_octets = nas_buffer->N_bytes;
  return true;
}

bool
nas::pack_identity_request(srslte::byte_buffer_t *reply_msg)
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = m_ecm_ctx.mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = m_ecm_ctx.enb_ue_s1ap_id;
  dw_nas->HandoverRestrictionList_present=false;
  dw_nas->SubscriberProfileIDforRFP_present=false;

  LIBLTE_MME_ID_REQUEST_MSG_STRUCT id_req;
  id_req.id_type = LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_identity_request_msg(&id_req, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_nas_log->error("Error packing Identity Request\n");
    m_nas_log->console("Error packing Identity REquest\n");
    return false;
  }

  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if (err != LIBLTE_SUCCESS) {
    m_nas_log->error("Error packing Dw NAS Transport: Authentication Reject\n");
    m_nas_log->console("Error packing Downlink NAS Transport: Authentication Reject\n");
    return false;
  }

  m_pool->deallocate(nas_buffer);
  return true;
}

bool
nas::pack_emm_information(srslte::byte_buffer_t *reply_msg)
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
  dw_nas->MME_UE_S1AP_ID.MME_UE_S1AP_ID = m_ecm_ctx.mme_ue_s1ap_id;
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = m_ecm_ctx.enb_ue_s1ap_id;
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
  m_sec_ctx.dl_nas_count++;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_emm_information_msg(&emm_info, sec_hdr_type, m_sec_ctx.dl_nas_count, (LIBLTE_BYTE_MSG_STRUCT *) nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_nas_log->error("Error packing EMM Information\n");
    m_nas_log->console("Error packing EMM Information\n");
    return false;
  }

  uint8_t mac[4];
  srslte::security_128_eia1 (&m_sec_ctx.k_nas_int[16],
                             m_sec_ctx.dl_nas_count,
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
    m_nas_log->error("Error packing Dw NAS Transport: EMM Info\n");
    m_nas_log->console("Error packing Downlink NAS Transport: EMM Info\n");
    return false;
  }

  m_nas_log->info("Packed UE EMM information\n"); 
  return true;
}

bool
nas::pack_service_reject(srslte::byte_buffer_t *reply_msg)
{
  uint8_t emm_cause = LIBLTE_MME_EMM_CAUSE_IMPLICITLY_DETACHED;
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
  dw_nas->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = m_ecm_ctx.enb_ue_s1ap_id;
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
    m_nas_log->error("Error packing Service Reject\n");
    m_nas_log->console("Error packing Service Reject\n");
    return false;
  }

  //Copy NAS PDU to Downlink NAS Trasport message buffer
  memcpy(dw_nas->NAS_PDU.buffer, nas_buffer->msg, nas_buffer->N_bytes);
  dw_nas->NAS_PDU.n_octets = nas_buffer->N_bytes;

  //Pack Downlink NAS Transport Message
  err = liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT *) reply_msg);
  if (err != LIBLTE_SUCCESS) {
    m_nas_log->error("Error packing Dw NAS Transport: Service Reject\n");
    m_nas_log->console("Error packing Downlink NAS Transport: Service Reject\n");
    return false;
  }
  return true;
}

/************************
 *
 * Security Functions
 *
 ************************/

bool
nas::short_integrity_check(srslte::byte_buffer_t *pdu)
{
  uint8_t exp_mac[4];
  uint8_t *mac = &pdu->msg[2];
  int i;

  srslte::security_128_eia1(&m_sec_ctx.k_nas_int[16],
                     m_sec_ctx.ul_nas_count,
                     0,
                     SECURITY_DIRECTION_UPLINK,
                     &pdu->msg[0],
                     2,
                     &exp_mac[0]);

  // Check if expected mac equals the sent mac
  for(i=0; i<2; i++){
    if(exp_mac[i+2] != mac[i]){
      m_nas_log->warning("Short integrity check failure. Local: count=%d, [%02x %02x %02x %02x], "
                          "Received: count=%d, [%02x %02x]\n",
                          m_sec_ctx.ul_nas_count, exp_mac[0], exp_mac[1], exp_mac[2], exp_mac[3],
                          pdu->msg[1] & 0x1F, mac[0], mac[1]);
      return false;
    }
  }
  m_nas_log->info("Integrity check ok. Local: count=%d, Received: count=%d\n",
                  m_sec_ctx.ul_nas_count, pdu->msg[1] & 0x1F);
  return true;
}


bool
nas::integrity_check(srslte::byte_buffer_t *pdu)
{
  uint8_t exp_mac[4];
  uint8_t *mac = &pdu->msg[1];
  int i;

  srslte::security_128_eia1(&m_sec_ctx.k_nas_int[16],
                     m_sec_ctx.ul_nas_count,
                     0,
                     SECURITY_DIRECTION_UPLINK,
                     &pdu->msg[5],
                     pdu->N_bytes-5,
                     &exp_mac[0]);

  // Check if expected mac equals the sent mac
  for(i=0; i<4; i++){
    if(exp_mac[i] != mac[i]){
      m_nas_log->warning("Integrity check failure. UL Local: count=%d, [%02x %02x %02x %02x], "
                       "Received: UL count=%d, [%02x %02x %02x %02x]\n",
                       m_sec_ctx.ul_nas_count, exp_mac[0], exp_mac[1], exp_mac[2], exp_mac[3],
                       pdu->msg[5], mac[0], mac[1], mac[2], mac[3]);
      return false;
    }
  }
  m_nas_log->info("Integrity check ok. Local: count=%d, Received: count=%d\n",
                m_sec_ctx.ul_nas_count, pdu->msg[5]);
    return true;
}

} //namespace srsepc
