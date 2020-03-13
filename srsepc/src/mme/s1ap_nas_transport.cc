/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#include "srsepc/hdr/mme/s1ap_nas_transport.h"
#include "srsepc/hdr/mme/mme.h"
#include "srsepc/hdr/mme/s1ap.h"
#include "srslte/common/int_helpers.h"
#include "srslte/common/liblte_security.h"
#include "srslte/common/security.h"
#include <cmath>
#include <inttypes.h> // for printing uint64_t

namespace srsepc {

s1ap_nas_transport* s1ap_nas_transport::m_instance    = NULL;
pthread_mutex_t     s1ap_nas_transport_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

s1ap_nas_transport::s1ap_nas_transport()
{
  return;
}

s1ap_nas_transport::~s1ap_nas_transport()
{
  return;
}

s1ap_nas_transport* s1ap_nas_transport::get_instance(void)
{
  pthread_mutex_lock(&s1ap_nas_transport_instance_mutex);
  if (NULL == m_instance) {
    m_instance = new s1ap_nas_transport();
  }
  pthread_mutex_unlock(&s1ap_nas_transport_instance_mutex);
  return (m_instance);
}

void s1ap_nas_transport::cleanup(void)
{
  pthread_mutex_lock(&s1ap_nas_transport_instance_mutex);
  if (NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&s1ap_nas_transport_instance_mutex);
}

void s1ap_nas_transport::init()
{
  m_s1ap     = s1ap::get_instance();
  m_s1ap_log = m_s1ap->m_s1ap_log;
  m_pool     = srslte::byte_buffer_pool::get_instance();

  // Init NAS args
  m_nas_init.mcc          = m_s1ap->m_s1ap_args.mcc;
  m_nas_init.mnc          = m_s1ap->m_s1ap_args.mnc;
  m_nas_init.mme_code     = m_s1ap->m_s1ap_args.mme_code;
  m_nas_init.mme_group    = m_s1ap->m_s1ap_args.mme_group;
  m_nas_init.tac          = m_s1ap->m_s1ap_args.tac;
  m_nas_init.apn          = m_s1ap->m_s1ap_args.mme_apn;
  m_nas_init.dns          = m_s1ap->m_s1ap_args.dns_addr;
  m_nas_init.paging_timer = m_s1ap->m_s1ap_args.paging_timer;
  m_nas_init.integ_algo   = m_s1ap->m_s1ap_args.integrity_algo;
  m_nas_init.cipher_algo  = m_s1ap->m_s1ap_args.encryption_algo;

  // Init NAS interface
  m_nas_if.s1ap = s1ap::get_instance();
  m_nas_if.gtpc = mme_gtpc::get_instance();
  m_nas_if.hss  = hss::get_instance();
  m_nas_if.mme  = mme::get_instance();
}

bool s1ap_nas_transport::handle_initial_ue_message(const asn1::s1ap::init_ue_msg_s& init_ue,
                                                   struct sctp_sndrcvinfo*          enb_sri)
{
  bool                   err, mac_valid;
  uint8_t                pd, msg_type, sec_hdr_type;
  srslte::byte_buffer_t* nas_msg = m_pool->allocate();
  memcpy(nas_msg->msg, init_ue.protocol_ies.nas_pdu.value.data(), init_ue.protocol_ies.nas_pdu.value.size());
  nas_msg->N_bytes = init_ue.protocol_ies.nas_pdu.value.size();

  uint64_t imsi           = 0;
  uint32_t m_tmsi         = 0;
  uint32_t enb_ue_s1ap_id = init_ue.protocol_ies.enb_ue_s1ap_id.value.value;
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT*)nas_msg, &pd, &msg_type);

  m_s1ap_log->console("Initial UE message: %s\n", liblte_nas_msg_type_to_string(msg_type));
  m_s1ap_log->info("Initial UE message: %s\n", liblte_nas_msg_type_to_string(msg_type));

  if (init_ue.protocol_ies.s_tmsi_present) {
    srslte::uint8_to_uint32(init_ue.protocol_ies.s_tmsi.value.m_tmsi.data(), &m_tmsi);
  }

  switch (msg_type) {
    case LIBLTE_MME_MSG_TYPE_ATTACH_REQUEST:
      m_s1ap_log->console("Received Initial UE message -- Attach Request\n");
      m_s1ap_log->info("Received Initial UE message -- Attach Request\n");
      err = nas::handle_attach_request(enb_ue_s1ap_id, enb_sri, nas_msg, m_nas_init, m_nas_if, m_s1ap->m_nas_log);
      break;
    case LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST:
      m_s1ap_log->console("Received Initial UE message -- Service Request\n");
      m_s1ap_log->info("Received Initial UE message -- Service Request\n");
      err = nas::handle_service_request(
          m_tmsi, enb_ue_s1ap_id, enb_sri, nas_msg, m_nas_init, m_nas_if, m_s1ap->m_nas_log);
      break;
    case LIBLTE_MME_MSG_TYPE_DETACH_REQUEST:
      m_s1ap_log->console("Received Initial UE message -- Detach Request\n");
      m_s1ap_log->info("Received Initial UE message -- Detach Request\n");
      err =
          nas::handle_detach_request(m_tmsi, enb_ue_s1ap_id, enb_sri, nas_msg, m_nas_init, m_nas_if, m_s1ap->m_nas_log);
      break;
    case LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_REQUEST:
      m_s1ap_log->console("Received Initial UE message -- Tracking Area Update Request\n");
      m_s1ap_log->info("Received Initial UE message -- Tracking Area Update Request\n");
      err = nas::handle_tracking_area_update_request(
          m_tmsi, enb_ue_s1ap_id, enb_sri, nas_msg, m_nas_init, m_nas_if, m_s1ap->m_nas_log);
      break;
    default:
      m_s1ap_log->info("Unhandled Initial UE Message 0x%x \n", msg_type);
      m_s1ap_log->console("Unhandled Initial UE Message 0x%x \n", msg_type);
      err = false;
  }
  m_pool->deallocate(nas_msg);
  return err;
}

bool s1ap_nas_transport::handle_uplink_nas_transport(const asn1::s1ap::ul_nas_transport_s& ul_xport,
                                                     struct sctp_sndrcvinfo*               enb_sri)
{
  uint8_t  pd, msg_type, sec_hdr_type;
  uint32_t enb_ue_s1ap_id      = ul_xport.protocol_ies.enb_ue_s1ap_id.value.value;
  uint32_t mme_ue_s1ap_id      = ul_xport.protocol_ies.mme_ue_s1ap_id.value.value;
  bool     mac_valid           = false;
  bool     increase_ul_nas_cnt = true;

  // Get UE NAS context
  nas* nas_ctx = m_s1ap->find_nas_ctx_from_mme_ue_s1ap_id(mme_ue_s1ap_id);
  if (nas_ctx == nullptr) {
    m_s1ap_log->warning("Received uplink NAS, but could not find UE NAS context. MME-UE S1AP id: %d\n", mme_ue_s1ap_id);
    return false;
  }

  m_s1ap_log->debug("Received uplink NAS and found UE NAS context. MME-UE S1AP id: %d\n", mme_ue_s1ap_id);
  emm_ctx_t* emm_ctx = &nas_ctx->m_emm_ctx;
  ecm_ctx_t* ecm_ctx = &nas_ctx->m_ecm_ctx;
  sec_ctx_t* sec_ctx = &nas_ctx->m_sec_ctx;

  // Parse NAS message header
  srslte::byte_buffer_t* nas_msg = m_pool->allocate();
  memcpy(nas_msg->msg, ul_xport.protocol_ies.nas_pdu.value.data(), ul_xport.protocol_ies.nas_pdu.value.size());
  nas_msg->N_bytes   = ul_xport.protocol_ies.nas_pdu.value.size();
  bool msg_encrypted = false;

  // Parse the message security header
  liblte_mme_parse_msg_sec_header((LIBLTE_BYTE_MSG_STRUCT*)nas_msg, &pd, &sec_hdr_type);

  // Invalid Security Header Type simply return function
  if (!(sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS ||
        sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY ||
        sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED ||
        sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_WITH_NEW_EPS_SECURITY_CONTEXT ||
        sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED_WITH_NEW_EPS_SECURITY_CONTEXT)) {
    m_s1ap_log->error("Unhandled security header type in Uplink NAS Transport: %d\n", sec_hdr_type);
    m_pool->deallocate(nas_msg);
    return false;
  }
  // Todo: Check on count mismatch of uplink count and do resync nas counter...

  // Check MAC if message is integrity protected
  if (sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY ||
      sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED ||
      sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_WITH_NEW_EPS_SECURITY_CONTEXT ||
      sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED_WITH_NEW_EPS_SECURITY_CONTEXT) {
    mac_valid = nas_ctx->integrity_check(nas_msg);
    if (mac_valid == false) {
      m_s1ap_log->warning("Invalid MAC message. Even if security header indicates integrity protection (Maybe: "
                          "Identity Response or Authentication Response)\n");
    }
  }

  // Decrypt message if indicated
  if (sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED ||
      sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED_WITH_NEW_EPS_SECURITY_CONTEXT) {
    m_s1ap_log->debug_hex(nas_msg->msg, nas_msg->N_bytes, "Encrypted");
    nas_ctx->cipher_decrypt(nas_msg);
    msg_encrypted = true;
    m_s1ap_log->debug_hex(nas_msg->msg, nas_msg->N_bytes, "Decrypted");
  }

  // Now parse message header and handle message
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT*)nas_msg, &pd, &msg_type);

  // Find UE EMM context if message is security protected.
  if (sec_hdr_type != LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS) {
    // Make sure EMM context is set-up, to do integrity check/de-chiphering
    if (emm_ctx->imsi == 0) {
      // No EMM context found. Perhaps a temporary context is being created?
      // This can happen with integrity protected identity reponse messages
      if (!(msg_type == LIBLTE_MME_MSG_TYPE_IDENTITY_RESPONSE &&
            sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY)) {
        m_s1ap_log->warning(
            "Uplink NAS: could not find security context for integrity protected message. MME-UE S1AP id: %d\n",
            mme_ue_s1ap_id);
        m_pool->deallocate(nas_msg);
        return false;
      }
    }
  }

  // Handle message and check if security requirements for messages
  // 4.4.4.3	Integrity checking of NAS signalling messages in the MME
  // Except the messages listed below, no NAS signalling messages shall be processed...
  // - ATTACH REQUEST;
  // - IDENTITY RESPONSE (if requested identification parameter is IMSI);
  // - AUTHENTICATION RESPONSE;
  // - AUTHENTICATION FAILURE;
  // - SECURITY MODE REJECT;
  // - DETACH REQUEST;
  // - DETACH ACCEPT;
  // - TRACKING AREA UPDATE REQUEST.
  m_s1ap_log->info("UL NAS: sec_hdr_type: %s, mac_vaild: %s, msg_encrypted: %s\n",
                   liblte_nas_sec_hdr_type_to_string(sec_hdr_type),
                   mac_valid == true ? "yes" : "no",
                   msg_encrypted == true ? "yes" : "no");

  switch (msg_type) {
    case LIBLTE_MME_MSG_TYPE_ATTACH_REQUEST:
      m_s1ap_log->info("UL NAS: Attach Request\n");
      m_s1ap_log->console("UL NAS: Attach Resquest\n");
      nas_ctx->handle_attach_request(nas_msg);
      break;
    case LIBLTE_MME_MSG_TYPE_IDENTITY_RESPONSE:
      m_s1ap_log->info("UL NAS: Received Identity Response\n");
      m_s1ap_log->console("UL NAS: Received Identity Response\n");
      nas_ctx->handle_identity_response(nas_msg);
      break;
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_RESPONSE:
      m_s1ap_log->info("UL NAS: Received Authentication Response\n");
      m_s1ap_log->console("UL NAS: Received Authentication Response\n");
      nas_ctx->handle_authentication_response(nas_msg);
      // In case of a successful authentication response, security mode command follows.
      // Reset counter for incoming security mode complete
      sec_ctx->ul_nas_count = 0;
      sec_ctx->dl_nas_count = 0;
      increase_ul_nas_cnt   = false;
      break;
    // Authentication failure with the option sync failure can be sent not integrity protected
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_FAILURE:
      m_s1ap_log->info("UL NAS: Authentication Failure\n");
      m_s1ap_log->console("UL NAS: Authentication Failure\n");
      nas_ctx->handle_authentication_failure(nas_msg);
      break;
    // Detach request can be sent not integrity protected when "power off" option is used
    case LIBLTE_MME_MSG_TYPE_DETACH_REQUEST:
      m_s1ap_log->info("UL NAS: Detach Request\n");
      m_s1ap_log->console("UL NAS: Detach Request\n");
      // TODO: check integrity protection in detach request
      nas_ctx->handle_detach_request(nas_msg);
      break;
    case LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMPLETE:
      m_s1ap_log->info("UL NAS: Received Security Mode Complete\n");
      m_s1ap_log->console("UL NAS: Received Security Mode Complete\n");
      if (sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED_WITH_NEW_EPS_SECURITY_CONTEXT &&
          mac_valid == true) {
        nas_ctx->handle_security_mode_complete(nas_msg);
      } else {
        // Security Mode Complete was not integrity protected
        m_s1ap_log->console("Security Mode Complete %s. Discard message.\n",
                            (mac_valid ? "not integrity protected" : "invalid integrity"));
        m_s1ap_log->warning("Security Mode Complete %s. Discard message.\n",
                            (mac_valid ? "not integrity protected" : "invalid integrity"));
        increase_ul_nas_cnt = false;
      }
      break;
    case LIBLTE_MME_MSG_TYPE_ATTACH_COMPLETE:
      m_s1ap_log->info("UL NAS: Received Attach Complete\n");
      m_s1ap_log->console("UL NAS: Received Attach Complete\n");
      if (sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED && mac_valid == true) {
        nas_ctx->handle_attach_complete(nas_msg);
      } else {
        // Attach Complete was not integrity protected
        m_s1ap_log->console("Attach Complete not integrity protected. Discard message.\n");
        m_s1ap_log->warning("Attach Complete not integrity protected. Discard message.\n");
        increase_ul_nas_cnt = false;
      }
      break;
    case LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_RESPONSE:
      m_s1ap_log->info("UL NAS: Received ESM Information Response\n");
      m_s1ap_log->console("UL NAS: Received ESM Information Response\n");
      if (sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED && mac_valid == true) {
        nas_ctx->handle_esm_information_response(nas_msg);
      } else {
        // Attach Complete was not integrity protected
        m_s1ap_log->console("ESM Information Response %s. Discard message.\n",
                            (mac_valid ? "not integrity protected" : "invalid integrity"));
        m_s1ap_log->warning("ESM Information Response %s. Discard message.\n",
                            (mac_valid ? "not integrity protected" : "invalid integrity"));
        increase_ul_nas_cnt = false;
      }
      break;
    case LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_REQUEST:
      m_s1ap_log->info("UL NAS: Tracking Area Update Request\n");
      m_s1ap_log->console("UL NAS: Tracking Area Update Request\n");
      nas_ctx->handle_tracking_area_update_request(nas_msg);
      break;
    default:
      m_s1ap_log->warning("Unhandled NAS integrity protected message %s\n", liblte_nas_msg_type_to_string(msg_type));
      m_s1ap_log->console("Unhandled NAS integrity protected message %s\n", liblte_nas_msg_type_to_string(msg_type));
      m_pool->deallocate(nas_msg);
      return false;
  }

  // Increment UL NAS count. if counter not resetted in function, e.g., DL Security mode command after Authentication
  // response
  if (increase_ul_nas_cnt == true) {
    sec_ctx->ul_nas_count++;
  }
  m_pool->deallocate(nas_msg);
  return true;
}

bool s1ap_nas_transport::send_downlink_nas_transport(uint32_t               enb_ue_s1ap_id,
                                                     uint32_t               mme_ue_s1ap_id,
                                                     srslte::byte_buffer_t* nas_msg,
                                                     struct sctp_sndrcvinfo enb_sri)
{
  m_s1ap_log->debug("Sending message to eNB with SCTP association %d. MME UE S1AP ID %d, eNB UE S1AP ID %d\n",
                    enb_sri.sinfo_assoc_id,
                    mme_ue_s1ap_id,
                    enb_ue_s1ap_id);

  // Setup initiating message
  s1ap_pdu_t tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_DL_NAS_TRANSPORT);

  // Setup Dw NAS structure
  asn1::s1ap::dl_nas_transport_ies_container& dw_nas = tx_pdu.init_msg().value.dl_nas_transport().protocol_ies;
  dw_nas.enb_ue_s1ap_id.value                        = enb_ue_s1ap_id;
  dw_nas.mme_ue_s1ap_id.value                        = mme_ue_s1ap_id;

  // Copy NAS PDU to Downlink NAS Trasport message buffer
  dw_nas.nas_pdu.value.resize(nas_msg->N_bytes);
  memcpy(dw_nas.nas_pdu.value.data(), nas_msg->msg, nas_msg->N_bytes);

  // Send Downlink NAS Transport Message
  m_s1ap->s1ap_tx_pdu(tx_pdu, &enb_sri);
  return true;
}

} // namespace srsepc
