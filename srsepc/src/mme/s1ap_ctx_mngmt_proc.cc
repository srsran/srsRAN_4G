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

#include "srsepc/hdr/mme/s1ap_ctx_mngmt_proc.h"
#include "srsepc/hdr/mme/s1ap.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/int_helpers.h"
#include "srslte/common/liblte_security.h"
#include <endian.h>

namespace srsepc {

s1ap_ctx_mngmt_proc* s1ap_ctx_mngmt_proc::m_instance    = NULL;
pthread_mutex_t      s1ap_ctx_mngmt_proc_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

s1ap_ctx_mngmt_proc::s1ap_ctx_mngmt_proc()
{
  return;
}

s1ap_ctx_mngmt_proc::~s1ap_ctx_mngmt_proc()
{
  return;
}

s1ap_ctx_mngmt_proc* s1ap_ctx_mngmt_proc::get_instance()
{
  pthread_mutex_lock(&s1ap_ctx_mngmt_proc_instance_mutex);
  if (NULL == m_instance) {
    m_instance = new s1ap_ctx_mngmt_proc();
  }
  pthread_mutex_unlock(&s1ap_ctx_mngmt_proc_instance_mutex);
  return (m_instance);
}

void s1ap_ctx_mngmt_proc::cleanup()
{
  pthread_mutex_lock(&s1ap_ctx_mngmt_proc_instance_mutex);
  if (NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&s1ap_ctx_mngmt_proc_instance_mutex);
}

void s1ap_ctx_mngmt_proc::init()
{
  m_s1ap      = s1ap::get_instance();
  m_mme_gtpc  = mme_gtpc::get_instance();
  m_s1ap_log  = m_s1ap->m_s1ap_log;
  m_s1ap_args = m_s1ap->m_s1ap_args;
  m_pool      = srslte::byte_buffer_pool::get_instance();
}

bool s1ap_ctx_mngmt_proc::send_initial_context_setup_request(nas* nas_ctx, uint16_t erab_to_setup)
{
  m_s1ap_log->info("Preparing to send Initial Context Setup request\n");

  // Get UE Context/E-RAB Context to setup
  emm_ctx_t* emm_ctx = &nas_ctx->m_emm_ctx;
  ecm_ctx_t* ecm_ctx = &nas_ctx->m_ecm_ctx;
  esm_ctx_t* esm_ctx = &nas_ctx->m_esm_ctx[erab_to_setup];
  sec_ctx_t* sec_ctx = &nas_ctx->m_sec_ctx;

  // Prepare reply PDU
  s1ap_pdu_t tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_INIT_CONTEXT_SETUP);

  asn1::s1ap::init_context_setup_request_ies_container& in_ctx_req =
      tx_pdu.init_msg().value.init_context_setup_request().protocol_ies;

  // Add MME and eNB S1AP Ids
  in_ctx_req.mme_ue_s1ap_id.value = ecm_ctx->mme_ue_s1ap_id;
  in_ctx_req.enb_ue_s1ap_id.value = ecm_ctx->enb_ue_s1ap_id;

  // UE-AMBR
  in_ctx_req.ueaggregate_maximum_bitrate.value.ueaggregate_maximum_bit_rate_dl = 1000000000;
  in_ctx_req.ueaggregate_maximum_bitrate.value.ueaggregate_maximum_bit_rate_ul = 1000000000;

  // Number of E-RABs to be setup
  in_ctx_req.erab_to_be_setup_list_ctxt_su_req.value.resize(1);
  in_ctx_req.erab_to_be_setup_list_ctxt_su_req.value[0].load_info_obj(ASN1_S1AP_ID_ERAB_TO_BE_SETUP_ITEM_CTXT_SU_REQ);

  // Setup eRAB context
  asn1::s1ap::erab_to_be_setup_item_ctxt_su_req_s& erab_ctx_req =
      in_ctx_req.erab_to_be_setup_list_ctxt_su_req.value[0].value.erab_to_be_setup_item_ctxt_su_req();
  erab_ctx_req.erab_id = esm_ctx->erab_id;

  // Setup E-RAB QoS parameters
  erab_ctx_req.erab_level_qos_params.qci                             = esm_ctx->qci;
  erab_ctx_req.erab_level_qos_params.alloc_retention_prio.prio_level = 15; // lowest
  erab_ctx_req.erab_level_qos_params.alloc_retention_prio.pre_emption_cap =
      asn1::s1ap::pre_emption_cap_opts::shall_not_trigger_pre_emption;
  erab_ctx_req.erab_level_qos_params.alloc_retention_prio.pre_emption_vulnerability =
      asn1::s1ap::pre_emption_vulnerability_opts::not_pre_emptable;
  erab_ctx_req.erab_level_qos_params.gbr_qos_info_present = false;

  // Set E-RAB S-GW F-TEID
  erab_ctx_req.transport_layer_address.resize(32); // IPv4
  asn1::bitstring_utils::from_number(
      erab_ctx_req.transport_layer_address.data(), ntohl(esm_ctx->sgw_s1u_fteid.ipv4), 32);
  erab_ctx_req.gtp_teid.from_number(esm_ctx->sgw_s1u_fteid.teid);

  // Set UE security capabilities and k_enb
  for (int i = 0; i < 3; i++) {
    if (sec_ctx->ue_network_cap.eea[i + 1] == true) {
      in_ctx_req.ue_security_cap.value.encryption_algorithms.set(16 - i, true); // EEA supported
    } else {
      in_ctx_req.ue_security_cap.value.encryption_algorithms.set(16 - i, false); // EEA not supported
    }
    if (sec_ctx->ue_network_cap.eia[i + 1] == true) {
      in_ctx_req.ue_security_cap.value.integrity_protection_algorithms.set(16 - i, true); // EIA supported
    } else {
      in_ctx_req.ue_security_cap.value.integrity_protection_algorithms.set(16 - i, false); // EIA not supported
    }
  }

  // Get K eNB
  // memcpy(in_ctx_req.security_key.value.data(),sec_ctx->k_enb, 32);
  for (uint8_t i = 0; i < 32; ++i) {
    in_ctx_req.security_key.value.data()[31 - i] = sec_ctx->k_enb[i];
  }
  m_s1ap_log->info_hex(sec_ctx->k_enb, 32, "Initial Context Setup Request -- Key eNB (k_enb)\n");

  srslte::unique_byte_buffer_t nas_buffer = allocate_unique_buffer(*m_pool);
  if (emm_ctx->state == EMM_STATE_DEREGISTERED) {
    // Attach procedure initiated from an attach request
    srslte::console("Adding attach accept to Initial Context Setup Request\n");
    m_s1ap_log->info("Adding attach accept to Initial Context Setup Request\n");
    nas_ctx->pack_attach_accept(nas_buffer.get());

    // Add nas message to context setup request
    erab_ctx_req.nas_pdu_present = true;
    erab_ctx_req.nas_pdu.resize(nas_buffer->N_bytes);
    memcpy(erab_ctx_req.nas_pdu.data(), nas_buffer->msg, nas_buffer->N_bytes);
  }

  if (!m_s1ap->s1ap_tx_pdu(tx_pdu, &ecm_ctx->enb_sri)) {
    m_s1ap_log->error("Error sending Initial Context Setup Request.\n");
    return false;
  }

  // Change E-RAB state to Context Setup Requested and save S-GW control F-TEID
  esm_ctx->state = ERAB_CTX_REQUESTED;

  struct in_addr addr;
  addr.s_addr = htonl(erab_ctx_req.transport_layer_address.to_number());
  srslte::console("Sent Initial Context Setup Request. E-RAB id %d \n", erab_ctx_req.erab_id);
  m_s1ap_log->info(
      "Initial Context -- S1-U TEID 0x%" PRIx64 ". IP %s \n", erab_ctx_req.gtp_teid.to_number(), inet_ntoa(addr));
  m_s1ap_log->info("Initial Context Setup Request -- eNB UE S1AP Id %d, MME UE S1AP Id %" PRIu64 "\n",
                   in_ctx_req.enb_ue_s1ap_id.value.value,
                   in_ctx_req.mme_ue_s1ap_id.value.value);
  m_s1ap_log->info("Initial Context Setup Request -- E-RAB id %d\n", erab_ctx_req.erab_id);
  m_s1ap_log->info("Initial Context Setup Request -- S1-U TEID 0x%" PRIu64 ". IP %s \n",
                   erab_ctx_req.gtp_teid.to_number(),
                   inet_ntoa(addr));
  m_s1ap_log->info("Initial Context Setup Request -- S1-U TEID 0x%" PRIu64 ". IP %s \n",
                   erab_ctx_req.gtp_teid.to_number(),
                   inet_ntoa(addr));
  m_s1ap_log->info("Initial Context Setup Request -- QCI %d\n", erab_ctx_req.erab_level_qos_params.qci);
  return true;
}

bool s1ap_ctx_mngmt_proc::handle_initial_context_setup_response(
    const asn1::s1ap::init_context_setup_resp_s& in_ctxt_resp)
{
  uint32_t mme_ue_s1ap_id = in_ctxt_resp.protocol_ies.mme_ue_s1ap_id.value.value;
  nas*     nas_ctx        = m_s1ap->find_nas_ctx_from_mme_ue_s1ap_id(mme_ue_s1ap_id);
  if (nas_ctx == nullptr) {
    m_s1ap_log->error("Could not find UE's context in active UE's map\n");
    return false;
  }

  emm_ctx_t* emm_ctx = &nas_ctx->m_emm_ctx;
  ecm_ctx_t* ecm_ctx = &nas_ctx->m_ecm_ctx;

  srslte::console("Received Initial Context Setup Response\n");

  // Setup E-RABs
  for (const asn1::s1ap::protocol_ie_single_container_s<asn1::s1ap::erab_setup_item_ctxt_su_res_ies_o>& ie_container :
       in_ctxt_resp.protocol_ies.erab_setup_list_ctxt_su_res.value) {

    // Get E-RAB setup context item and E-RAB Id
    const asn1::s1ap::erab_setup_item_ctxt_su_res_s& erab_setup_item_ctxt =
        ie_container.value.erab_setup_item_ctxt_su_res();
    uint8_t erab_id = erab_setup_item_ctxt.erab_id;

    // Make sure we requested the context setup
    esm_ctx_t* esm_ctx = &nas_ctx->m_esm_ctx[erab_id];
    if (esm_ctx->state != ERAB_CTX_REQUESTED) {
      m_s1ap_log->error("E-RAB requested was not previously requested %d\n", erab_id);
      return false;
    }

    // Mark E-RAB with context setup
    esm_ctx->state = ERAB_CTX_SETUP;

    // Set the GTP information
    esm_ctx->enb_fteid.teid = erab_setup_item_ctxt.gtp_teid.to_number();
    esm_ctx->enb_fteid.ipv4 = ntohl(erab_setup_item_ctxt.transport_layer_address.to_number());

    char           enb_addr_str[INET_ADDRSTRLEN + 1] = {};
    struct in_addr tmp_addr                          = {};
    tmp_addr.s_addr                                  = esm_ctx->enb_fteid.ipv4;
    const char* err                                  = inet_ntop(AF_INET, &tmp_addr, enb_addr_str, INET_ADDRSTRLEN);
    if (err == nullptr) {
      m_s1ap_log->error("Error converting IP to string\n");
    }

    m_s1ap_log->info("E-RAB Context Setup. E-RAB id %d\n", esm_ctx->erab_id);
    m_s1ap_log->info("E-RAB Context -- eNB TEID 0x%x, eNB Address %s\n", esm_ctx->enb_fteid.teid, enb_addr_str);
    srslte::console("E-RAB Context Setup. E-RAB id %d\n", esm_ctx->erab_id);
    srslte::console("E-RAB Context -- eNB TEID 0x%x; eNB GTP-U Address %s\n", esm_ctx->enb_fteid.teid, enb_addr_str);
  }

  if (emm_ctx->state == EMM_STATE_REGISTERED) {
    srslte::console("Initial Context Setup Response triggered from Service Request.\n");
    srslte::console("Sending Modify Bearer Request.\n");
    m_mme_gtpc->send_modify_bearer_request(emm_ctx->imsi, 5, &nas_ctx->m_esm_ctx[5].enb_fteid);
  }
  return true;
}

bool s1ap_ctx_mngmt_proc::handle_ue_context_release_request(const asn1::s1ap::ue_context_release_request_s& ue_rel,
                                                            struct sctp_sndrcvinfo*                         enb_sri)
{
  uint32_t mme_ue_s1ap_id = ue_rel.protocol_ies.mme_ue_s1ap_id.value.value;
  m_s1ap_log->info("Received UE Context Release Request. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
  srslte::console("Received UE Context Release Request. MME-UE S1AP Id %d\n", mme_ue_s1ap_id);

  nas* nas_ctx = m_s1ap->find_nas_ctx_from_mme_ue_s1ap_id(mme_ue_s1ap_id);
  if (nas_ctx == nullptr) {
    m_s1ap_log->info("No UE context to release found. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
    srslte::console("No UE context to release found. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
    return false;
  }

  emm_ctx_t* emm_ctx = &nas_ctx->m_emm_ctx;
  ecm_ctx_t* ecm_ctx = &nas_ctx->m_ecm_ctx;

  // Send release context command to eNB, so that it can release it's bearers
  if (ecm_ctx->state == ECM_STATE_CONNECTED) {
    send_ue_context_release_command(nas_ctx);
  } else {
    // No ECM Context to release
    m_s1ap_log->info("UE is not ECM connected. No need to release S1-U. MME UE S1AP Id %d\n", mme_ue_s1ap_id);
    // Make sure E-RABS are marked as DEACTIVATED.
    for (esm_ctx_t& esm_ctx : nas_ctx->m_esm_ctx) {
      esm_ctx.state = ERAB_DEACTIVATED;
    }
  }

  // Set UE context will be cleared upon reception of UE Context Release Complete
  return true;
}

bool s1ap_ctx_mngmt_proc::send_ue_context_release_command(nas* nas_ctx)
{
  emm_ctx_t* emm_ctx = &nas_ctx->m_emm_ctx;
  ecm_ctx_t* ecm_ctx = &nas_ctx->m_ecm_ctx;

  if (ecm_ctx->state != ECM_STATE_CONNECTED) {
    m_s1ap_log->error("UE is not ECM connected. No send context release command. MME UE S1AP Id %d\n",
                      ecm_ctx->mme_ue_s1ap_id);
    return false;
  }

  // Detect weather there are active E-RABs
  bool active_erabs = false;
  for (esm_ctx_t& esm_ctx : nas_ctx->m_esm_ctx) {
    if (esm_ctx.state != ERAB_DEACTIVATED) {
      active_erabs = true;
      break;
    }
  }

  // On some circumstances, such as the NAS Detach, the UE context has already been cleared from
  // the SPGW. In such cases, there is no need to send the GTP-C Release Access Bearers Request.
  if (active_erabs) {
    // There are active E-RABs, send release access mearers request
    srslte::console("There are active E-RABs, send release access bearers request\n");
    m_s1ap_log->info("There are active E-RABs, send release access bearers request\n");

    // The handle_release_access_bearers_response function will make sure to mark E-RABS DEACTIVATED
    // It will release the UEs downstream S1-u and keep the upstream S1-U connection active.
    m_mme_gtpc->send_release_access_bearers_request(emm_ctx->imsi);
  }

  // Mark ECM state as IDLE and de-activate E-RABs
  ecm_ctx->state = ECM_STATE_IDLE;
  for (esm_ctx_t& esm_ctx : nas_ctx->m_esm_ctx) {
    esm_ctx.state = ERAB_DEACTIVATED;
  }

  // Prepare reply PDU
  s1ap_pdu_t tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_UE_CONTEXT_RELEASE);

  asn1::s1ap::ue_context_release_cmd_ies_container& ctx_rel_cmd =
      tx_pdu.init_msg().value.ue_context_release_cmd().protocol_ies;
  ctx_rel_cmd.ue_s1ap_ids.value.set(asn1::s1ap::ue_s1ap_ids_c::types_opts::ue_s1ap_id_pair);
  ctx_rel_cmd.ue_s1ap_ids.value.ue_s1ap_id_pair().mme_ue_s1ap_id = nas_ctx->m_ecm_ctx.mme_ue_s1ap_id;
  ctx_rel_cmd.ue_s1ap_ids.value.ue_s1ap_id_pair().enb_ue_s1ap_id = nas_ctx->m_ecm_ctx.enb_ue_s1ap_id;

  ctx_rel_cmd.cause.value.set(asn1::s1ap::cause_c::types_opts::nas);
  ctx_rel_cmd.cause.value.nas().value = asn1::s1ap::cause_nas_opts::options::normal_release;

  // Send Reply to eNB
  if (!m_s1ap->s1ap_tx_pdu(tx_pdu, &nas_ctx->m_ecm_ctx.enb_sri)) {
    m_s1ap_log->error("Error sending UE Context Release Command.\n");
    return false;
  }

  return true;
}

bool s1ap_ctx_mngmt_proc::handle_ue_context_release_complete(const asn1::s1ap::ue_context_release_complete_s& rel_comp)
{
  uint32_t mme_ue_s1ap_id = rel_comp.protocol_ies.mme_ue_s1ap_id.value.value;
  m_s1ap_log->info("Received UE Context Release Complete. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
  srslte::console("Received UE Context Release Complete. MME-UE S1AP Id %d\n", mme_ue_s1ap_id);

  nas* nas_ctx = m_s1ap->find_nas_ctx_from_mme_ue_s1ap_id(mme_ue_s1ap_id);
  if (nas_ctx == nullptr) {
    m_s1ap_log->info("No UE context to release found. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
    srslte::console("No UE context to release found. MME-UE S1AP Id: %d\n", mme_ue_s1ap_id);
    return false;
  }

  // Delete UE context
  m_s1ap->release_ue_ecm_ctx(nas_ctx->m_ecm_ctx.mme_ue_s1ap_id);
  m_s1ap_log->info("UE Context Release Completed.\n");
  srslte::console("UE Context Release Completed.\n");
  return true;
}
} // namespace srsepc
