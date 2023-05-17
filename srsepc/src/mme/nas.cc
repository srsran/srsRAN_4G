/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsepc/hdr/mme/s1ap.h"
#include "srsepc/hdr/mme/s1ap_nas_transport.h"
#include "srsran/common/liblte_security.h"
#include "srsran/common/security.h"
#include <cmath>
#include <inttypes.h> // for printing uint64_t
#include <netinet/sctp.h>
#include <sys/timerfd.h>
#include <time.h>

namespace srsepc {

nas::nas(const nas_init_t& args, const nas_if_t& itf) :
  m_gtpc(itf.gtpc),
  m_s1ap(itf.s1ap),
  m_hss(itf.hss),
  m_mme(itf.mme),
  m_mcc(args.mcc),
  m_mnc(args.mnc),
  m_mme_group(args.mme_group),
  m_mme_code(args.mme_code),
  m_tac(args.tac),
  m_apn(args.apn),
  m_dns(args.dns),
  m_full_net_name(args.full_net_name),
  m_short_net_name(args.short_net_name),
  m_t3413(args.paging_timer),
  m_request_imeisv(args.request_imeisv),
  m_lac(args.lac)
{
  m_sec_ctx.integ_algo  = args.integ_algo;
  m_sec_ctx.cipher_algo = args.cipher_algo;
  m_logger.debug("NAS Context Initialized. MCC: 0x%x, MNC 0x%x", m_mcc, m_mnc);
}

void nas::reset()
{
  m_emm_ctx = {};
  m_ecm_ctx = {};
  for (int i = 0; i < MAX_ERABS_PER_UE; ++i) {
    m_esm_ctx[i] = {};
  }

  srsran::INTEGRITY_ALGORITHM_ID_ENUM integ_algo  = m_sec_ctx.integ_algo;
  srsran::CIPHERING_ALGORITHM_ID_ENUM cipher_algo = m_sec_ctx.cipher_algo;
  m_sec_ctx                                       = {};
  m_sec_ctx.integ_algo                            = integ_algo;
  m_sec_ctx.cipher_algo                           = cipher_algo;
}

/**********************************
 *
 * Handle UE Initiating Messages
 *
 ********************************/
bool nas::handle_attach_request(uint32_t                enb_ue_s1ap_id,
                                struct sctp_sndrcvinfo* enb_sri,
                                srsran::byte_buffer_t*  nas_rx,
                                const nas_init_t&       args,
                                const nas_if_t&         itf)
{
  uint32_t                                       m_tmsi      = 0;
  uint64_t                                       imsi        = 0;
  LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT           attach_req  = {};
  LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT pdn_con_req = {};
  auto&                                          nas_logger  = srslog::fetch_basic_logger("NAS");

  // Interfaces
  s1ap_interface_nas* s1ap = itf.s1ap;
  hss_interface_nas*  hss  = itf.hss;
  gtpc_interface_nas* gtpc = itf.gtpc;

  // Get NAS Attach Request and PDN connectivity request messages
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_attach_request_msg((LIBLTE_BYTE_MSG_STRUCT*)nas_rx, &attach_req);
  if (err != LIBLTE_SUCCESS) {
    nas_logger.error("Error unpacking NAS attach request. Error: %s", liblte_error_text[err]);
    return false;
  }
  // Get PDN Connectivity Request*/
  err = liblte_mme_unpack_pdn_connectivity_request_msg(&attach_req.esm_msg, &pdn_con_req);
  if (err != LIBLTE_SUCCESS) {
    nas_logger.error("Error unpacking NAS PDN Connectivity Request. Error: %s", liblte_error_text[err]);
    return false;
  }

  // Get UE IMSI
  if (attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI) {
    for (int i = 0; i <= 14; i++) {
      imsi += attach_req.eps_mobile_id.imsi[i] * std::pow(10, 14 - i);
    }
    srsran::console("Attach request -- IMSI: %015" PRIu64 "\n", imsi);
    nas_logger.info("Attach request -- IMSI: %015" PRIu64 "", imsi);
  } else if (attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI) {
    m_tmsi = attach_req.eps_mobile_id.guti.m_tmsi;
    imsi   = s1ap->find_imsi_from_m_tmsi(m_tmsi);
    srsran::console("Attach request -- M-TMSI: 0x%x\n", m_tmsi);
    nas_logger.info("Attach request -- M-TMSI: 0x%x", m_tmsi);
  } else {
    nas_logger.error("Unhandled Mobile Id type in attach request");
    return false;
  }

  // Log Attach Request Information
  srsran::console("Attach request -- eNB-UE S1AP Id: %d\n", enb_ue_s1ap_id);
  nas_logger.info("Attach request -- eNB-UE S1AP Id: %d", enb_ue_s1ap_id);
  srsran::console("Attach request -- Attach type: %d\n", attach_req.eps_attach_type);
  nas_logger.info("Attach request -- Attach type: %d", attach_req.eps_attach_type);
  srsran::console("Attach Request -- UE Network Capabilities EEA: %d%d%d%d%d%d%d%d\n",
                  attach_req.ue_network_cap.eea[0],
                  attach_req.ue_network_cap.eea[1],
                  attach_req.ue_network_cap.eea[2],
                  attach_req.ue_network_cap.eea[3],
                  attach_req.ue_network_cap.eea[4],
                  attach_req.ue_network_cap.eea[5],
                  attach_req.ue_network_cap.eea[6],
                  attach_req.ue_network_cap.eea[7]);
  nas_logger.info("Attach Request -- UE Network Capabilities EEA: %d%d%d%d%d%d%d%d",
                  attach_req.ue_network_cap.eea[0],
                  attach_req.ue_network_cap.eea[1],
                  attach_req.ue_network_cap.eea[2],
                  attach_req.ue_network_cap.eea[3],
                  attach_req.ue_network_cap.eea[4],
                  attach_req.ue_network_cap.eea[5],
                  attach_req.ue_network_cap.eea[6],
                  attach_req.ue_network_cap.eea[7]);
  srsran::console("Attach Request -- UE Network Capabilities EIA: %d%d%d%d%d%d%d%d\n",
                  attach_req.ue_network_cap.eia[0],
                  attach_req.ue_network_cap.eia[1],
                  attach_req.ue_network_cap.eia[2],
                  attach_req.ue_network_cap.eia[3],
                  attach_req.ue_network_cap.eia[4],
                  attach_req.ue_network_cap.eia[5],
                  attach_req.ue_network_cap.eia[6],
                  attach_req.ue_network_cap.eia[7]);
  nas_logger.info("Attach Request -- UE Network Capabilities EIA: %d%d%d%d%d%d%d%d",
                  attach_req.ue_network_cap.eia[0],
                  attach_req.ue_network_cap.eia[1],
                  attach_req.ue_network_cap.eia[2],
                  attach_req.ue_network_cap.eia[3],
                  attach_req.ue_network_cap.eia[4],
                  attach_req.ue_network_cap.eia[5],
                  attach_req.ue_network_cap.eia[6],
                  attach_req.ue_network_cap.eia[7]);
  srsran::console("Attach Request -- MS Network Capabilities Present: %s\n",
                  attach_req.ms_network_cap_present ? "true" : "false");
  nas_logger.info("Attach Request -- MS Network Capabilities Present: %s",
                  attach_req.ms_network_cap_present ? "true" : "false");
  srsran::console("PDN Connectivity Request -- EPS Bearer Identity requested: %d\n", pdn_con_req.eps_bearer_id);
  nas_logger.info("PDN Connectivity Request -- EPS Bearer Identity requested: %d", pdn_con_req.eps_bearer_id);
  srsran::console("PDN Connectivity Request -- Procedure Transaction Id: %d\n", pdn_con_req.proc_transaction_id);
  nas_logger.info("PDN Connectivity Request -- Procedure Transaction Id: %d", pdn_con_req.proc_transaction_id);
  srsran::console("PDN Connectivity Request -- ESM Information Transfer requested: %s\n",
                  pdn_con_req.esm_info_transfer_flag_present ? "true" : "false");
  nas_logger.info("PDN Connectivity Request -- ESM Information Transfer requested: %s",
                  pdn_con_req.esm_info_transfer_flag_present ? "true" : "false");

  // Get NAS Context if UE is known
  nas* nas_ctx = s1ap->find_nas_ctx_from_imsi(imsi);
  if (nas_ctx == NULL) {
    // Get attach type from attach request
    if (attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI) {
      nas::handle_imsi_attach_request_unknown_ue(enb_ue_s1ap_id, enb_sri, attach_req, pdn_con_req, args, itf);
    } else if (attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI) {
      nas::handle_guti_attach_request_unknown_ue(enb_ue_s1ap_id, enb_sri, attach_req, pdn_con_req, args, itf);
    } else {
      return false;
    }
  } else {
    nas_logger.info("Attach Request -- Found previously attached UE.");
    srsran::console("Attach Request -- Found previously attach UE.\n");
    if (attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI) {
      nas::handle_imsi_attach_request_known_ue(
          nas_ctx, enb_ue_s1ap_id, enb_sri, attach_req, pdn_con_req, nas_rx, args, itf);
    } else if (attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI) {
      nas::handle_guti_attach_request_known_ue(
          nas_ctx, enb_ue_s1ap_id, enb_sri, attach_req, pdn_con_req, nas_rx, args, itf);
    } else {
      return false;
    }
  }
  return true;
}

bool nas::handle_imsi_attach_request_unknown_ue(uint32_t                                              enb_ue_s1ap_id,
                                                struct sctp_sndrcvinfo*                               enb_sri,
                                                const LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT&           attach_req,
                                                const LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT& pdn_con_req,
                                                const nas_init_t&                                     args,
                                                const nas_if_t&                                       itf)
{
  nas*                         nas_ctx;
  srsran::unique_byte_buffer_t nas_tx;
  auto&                        nas_logger = srslog::fetch_basic_logger("NAS");

  // Interfaces
  s1ap_interface_nas* s1ap = itf.s1ap;
  hss_interface_nas*  hss  = itf.hss;
  gtpc_interface_nas* gtpc = itf.gtpc;

  // Get IMSI
  uint64_t imsi = 0;
  for (int i = 0; i <= 14; i++) {
    imsi += attach_req.eps_mobile_id.imsi[i] * std::pow(10, 14 - i);
  }

  // Create UE context
  nas_ctx = new nas(args, itf);

  // Save IMSI, eNB UE S1AP Id, MME UE S1AP Id and make sure UE is EMM_DEREGISTERED
  nas_ctx->m_emm_ctx.imsi           = imsi;
  nas_ctx->m_emm_ctx.state          = EMM_STATE_DEREGISTERED;
  nas_ctx->m_ecm_ctx.enb_ue_s1ap_id = enb_ue_s1ap_id;
  nas_ctx->m_ecm_ctx.mme_ue_s1ap_id = s1ap->get_next_mme_ue_s1ap_id();

  // Save UE network capabilities
  memcpy(
      &nas_ctx->m_sec_ctx.ue_network_cap, &attach_req.ue_network_cap, sizeof(LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT));
  nas_ctx->m_sec_ctx.ms_network_cap_present = attach_req.ms_network_cap_present;
  if (attach_req.ms_network_cap_present) {
    memcpy(&nas_ctx->m_sec_ctx.ms_network_cap,
           &attach_req.ms_network_cap,
           sizeof(LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT));
  }

  uint8_t eps_bearer_id                       = pdn_con_req.eps_bearer_id; // TODO: Unused
  nas_ctx->m_emm_ctx.procedure_transaction_id = pdn_con_req.proc_transaction_id;

  // Initialize NAS count
  nas_ctx->m_sec_ctx.ul_nas_count = 0;
  nas_ctx->m_sec_ctx.dl_nas_count = 0;

  // Set eNB information
  memcpy(&nas_ctx->m_ecm_ctx.enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));

  // Save whether secure ESM information transfer is necessary
  nas_ctx->m_ecm_ctx.eit = pdn_con_req.esm_info_transfer_flag_present;

  // Initialize E-RABs
  for (uint i = 0; i < MAX_ERABS_PER_UE; i++) {
    nas_ctx->m_esm_ctx[i].state   = ERAB_DEACTIVATED;
    nas_ctx->m_esm_ctx[i].erab_id = i;
  }

  // Save attach request type
  nas_ctx->m_emm_ctx.attach_type = attach_req.eps_attach_type;

  // Get Authentication Vectors from HSS
  if (!hss->gen_auth_info_answer(nas_ctx->m_emm_ctx.imsi,
                                 nas_ctx->m_sec_ctx.k_asme,
                                 nas_ctx->m_sec_ctx.autn,
                                 nas_ctx->m_sec_ctx.rand,
                                 nas_ctx->m_sec_ctx.xres)) {
    srsran::console("User not found. IMSI %015" PRIu64 "\n", nas_ctx->m_emm_ctx.imsi);
    nas_logger.info("User not found. IMSI %015" PRIu64 "", nas_ctx->m_emm_ctx.imsi);
    return false;
  }

  // Allocate eKSI for this authentication vector
  // Here we assume a new security context thus a new eKSI
  nas_ctx->m_sec_ctx.eksi = 0;

  // Save the UE context
  s1ap->add_nas_ctx_to_imsi_map(nas_ctx);
  s1ap->add_nas_ctx_to_mme_ue_s1ap_id_map(nas_ctx);
  s1ap->add_ue_to_enb_set(enb_sri->sinfo_assoc_id, nas_ctx->m_ecm_ctx.mme_ue_s1ap_id);

  // Pack NAS Authentication Request in Downlink NAS Transport msg
  nas_tx = srsran::make_byte_buffer();
  if (nas_tx == nullptr) {
    nas_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return false;
  }
  nas_ctx->pack_authentication_request(nas_tx.get());

  // Send reply to eNB
  s1ap->send_downlink_nas_transport(
      nas_ctx->m_ecm_ctx.enb_ue_s1ap_id, nas_ctx->m_ecm_ctx.mme_ue_s1ap_id, nas_tx.get(), nas_ctx->m_ecm_ctx.enb_sri);

  nas_logger.info("Downlink NAS: Sending Authentication Request");
  srsran::console("Downlink NAS: Sending Authentication Request\n");
  return true;
}

bool nas::handle_imsi_attach_request_known_ue(nas*                                                  nas_ctx,
                                              uint32_t                                              enb_ue_s1ap_id,
                                              struct sctp_sndrcvinfo*                               enb_sri,
                                              const LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT&           attach_req,
                                              const LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT& pdn_con_req,
                                              srsran::byte_buffer_t*                                nas_rx,
                                              const nas_init_t&                                     args,
                                              const nas_if_t&                                       itf)
{
  bool  err;
  auto& nas_logger = srslog::fetch_basic_logger("NAS");

  // Interfaces
  s1ap_interface_nas* s1ap = itf.s1ap;
  hss_interface_nas*  hss  = itf.hss;
  gtpc_interface_nas* gtpc = itf.gtpc;

  // Delete previous GTP-U session
  gtpc->send_delete_session_request(nas_ctx->m_emm_ctx.imsi);

  // Release previous context in the eNB, if present
  if (nas_ctx->m_ecm_ctx.mme_ue_s1ap_id != 0) {
    s1ap->send_ue_context_release_command(nas_ctx->m_ecm_ctx.mme_ue_s1ap_id);
  }
  // Delete previous NAS context
  s1ap->delete_ue_ctx(nas_ctx->m_emm_ctx.imsi);

  // Handle new attach
  err = nas::handle_imsi_attach_request_unknown_ue(enb_ue_s1ap_id, enb_sri, attach_req, pdn_con_req, args, itf);
  return err;
}

bool nas::handle_guti_attach_request_unknown_ue(uint32_t                                              enb_ue_s1ap_id,
                                                struct sctp_sndrcvinfo*                               enb_sri,
                                                const LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT&           attach_req,
                                                const LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT& pdn_con_req,
                                                const nas_init_t&                                     args,
                                                const nas_if_t&                                       itf)

{
  nas*                         nas_ctx;
  srsran::unique_byte_buffer_t nas_tx;

  // Interfaces
  s1ap_interface_nas* s1ap = itf.s1ap;
  hss_interface_nas*  hss  = itf.hss;
  gtpc_interface_nas* gtpc = itf.gtpc;

  // Create new NAS context.
  nas_ctx = new nas(args, itf);

  // Could not find IMSI from M-TMSI, send Id request
  // The IMSI will be set when the identity response is received
  // Set EMM ctx
  nas_ctx->m_emm_ctx.imsi  = 0;
  nas_ctx->m_emm_ctx.state = EMM_STATE_DEREGISTERED;

  // Save UE network capabilities
  memcpy(
      &nas_ctx->m_sec_ctx.ue_network_cap, &attach_req.ue_network_cap, sizeof(LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT));
  nas_ctx->m_sec_ctx.ms_network_cap_present = attach_req.ms_network_cap_present;
  if (attach_req.ms_network_cap_present) {
    memcpy(&nas_ctx->m_sec_ctx.ms_network_cap,
           &attach_req.ms_network_cap,
           sizeof(LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT));
  }
  // Initialize NAS count
  nas_ctx->m_sec_ctx.ul_nas_count             = 0;
  nas_ctx->m_sec_ctx.dl_nas_count             = 0;
  nas_ctx->m_emm_ctx.procedure_transaction_id = pdn_con_req.proc_transaction_id;

  // Set ECM context
  nas_ctx->m_ecm_ctx.enb_ue_s1ap_id = enb_ue_s1ap_id;
  nas_ctx->m_ecm_ctx.mme_ue_s1ap_id = s1ap->get_next_mme_ue_s1ap_id();

  uint8_t eps_bearer_id = pdn_con_req.eps_bearer_id;

  // Save attach request type
  nas_ctx->m_emm_ctx.attach_type = attach_req.eps_attach_type;

  // Save whether ESM information transfer is necessary
  nas_ctx->m_ecm_ctx.eit = pdn_con_req.esm_info_transfer_flag_present;

  // Add eNB info to UE ctxt
  memcpy(&nas_ctx->m_ecm_ctx.enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));

  // Initialize E-RABs
  for (uint i = 0; i < MAX_ERABS_PER_UE; i++) {
    nas_ctx->m_esm_ctx[i].state   = ERAB_DEACTIVATED;
    nas_ctx->m_esm_ctx[i].erab_id = i;
  }

  // Store temporary ue context
  s1ap->add_nas_ctx_to_mme_ue_s1ap_id_map(nas_ctx);
  s1ap->add_ue_to_enb_set(enb_sri->sinfo_assoc_id, nas_ctx->m_ecm_ctx.mme_ue_s1ap_id);

  // Send Identity Request
  nas_tx = srsran::make_byte_buffer();
  if (nas_tx == nullptr) {
    srslog::fetch_basic_logger("NAS").error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return false;
  }
  nas_ctx->pack_identity_request(nas_tx.get());
  s1ap->send_downlink_nas_transport(
      nas_ctx->m_ecm_ctx.enb_ue_s1ap_id, nas_ctx->m_ecm_ctx.mme_ue_s1ap_id, nas_tx.get(), nas_ctx->m_ecm_ctx.enb_sri);

  return true;
}

bool nas::handle_guti_attach_request_known_ue(nas*                                                  nas_ctx,
                                              uint32_t                                              enb_ue_s1ap_id,
                                              struct sctp_sndrcvinfo*                               enb_sri,
                                              const LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT&           attach_req,
                                              const LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT& pdn_con_req,
                                              srsran::byte_buffer_t*                                nas_rx,
                                              const nas_init_t&                                     args,
                                              const nas_if_t&                                       itf)
{
  bool                         msg_valid = false;
  srsran::unique_byte_buffer_t nas_tx;
  auto&                        nas_logger = srslog::fetch_basic_logger("NAS");

  emm_ctx_t* emm_ctx = &nas_ctx->m_emm_ctx;
  ecm_ctx_t* ecm_ctx = &nas_ctx->m_ecm_ctx;
  sec_ctx_t* sec_ctx = &nas_ctx->m_sec_ctx;

  // Interfaces
  s1ap_interface_nas* s1ap = itf.s1ap;
  hss_interface_nas*  hss  = itf.hss;
  gtpc_interface_nas* gtpc = itf.gtpc;

  srsran::console("Found UE context. IMSI: %015" PRIu64 ", old eNB UE S1ap Id %d, old MME UE S1AP Id %d\n",
                  emm_ctx->imsi,
                  ecm_ctx->enb_ue_s1ap_id,
                  ecm_ctx->mme_ue_s1ap_id);

  // Check NAS integrity
  msg_valid = nas_ctx->integrity_check(nas_rx);
  if (msg_valid == true && emm_ctx->state == EMM_STATE_DEREGISTERED) {
    srsran::console(
        "GUTI Attach -- NAS Integrity OK. UL count %d, DL count %d\n", sec_ctx->ul_nas_count, sec_ctx->dl_nas_count);
    nas_logger.info(
        "GUTI Attach -- NAS Integrity OK. UL count %d, DL count %d", sec_ctx->ul_nas_count, sec_ctx->dl_nas_count);

    // Create new MME UE S1AP Identity
    ecm_ctx->mme_ue_s1ap_id = s1ap->get_next_mme_ue_s1ap_id();
    ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;

    emm_ctx->procedure_transaction_id = pdn_con_req.proc_transaction_id;

    // Save Attach type
    emm_ctx->attach_type = attach_req.eps_attach_type;

    // Set eNB information
    ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
    memcpy(&ecm_ctx->enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));

    // Save whether secure ESM information transfer is necessary
    ecm_ctx->eit = pdn_con_req.esm_info_transfer_flag_present;

    // Initialize E-RABs
    for (uint i = 0; i < MAX_ERABS_PER_UE; i++) {
      nas_ctx->m_esm_ctx[i].state   = ERAB_DEACTIVATED;
      nas_ctx->m_esm_ctx[i].erab_id = i;
    }

    // Store context based on MME UE S1AP id
    s1ap->add_nas_ctx_to_mme_ue_s1ap_id_map(nas_ctx);
    s1ap->add_ue_to_enb_set(enb_sri->sinfo_assoc_id, ecm_ctx->mme_ue_s1ap_id);

    // Re-generate K_eNB
    srsran::security_generate_k_enb(sec_ctx->k_asme, sec_ctx->ul_nas_count, sec_ctx->k_enb);
    nas_logger.info("Generating KeNB with UL NAS COUNT: %d", sec_ctx->ul_nas_count);
    srsran::console("Generating KeNB with UL NAS COUNT: %d\n", sec_ctx->ul_nas_count);
    nas_logger.info(sec_ctx->k_enb, 32, "Key eNodeB (k_enb)");

    // Send reply
    nas_tx = srsran::make_byte_buffer();
    if (nas_tx == nullptr) {
      nas_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return false;
    }
    if (ecm_ctx->eit) {
      srsran::console("Secure ESM information transfer requested.\n");
      nas_logger.info("Secure ESM information transfer requested.");
      nas_ctx->pack_esm_information_request(nas_tx.get());
      s1ap->send_downlink_nas_transport(ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id, nas_tx.get(), *enb_sri);
    } else {
      // Get subscriber info from HSS
      uint8_t default_bearer = 5;
      hss->gen_update_loc_answer(emm_ctx->imsi, &nas_ctx->m_esm_ctx[default_bearer].qci);
      nas_logger.debug("Getting subscription information -- QCI %d", nas_ctx->m_esm_ctx[default_bearer].qci);
      srsran::console("Getting subscription information -- QCI %d\n", nas_ctx->m_esm_ctx[default_bearer].qci);
      gtpc->send_create_session_request(emm_ctx->imsi);
    }
    sec_ctx->ul_nas_count++;
    return true;
  } else {
    if (emm_ctx->state != EMM_STATE_DEREGISTERED) {
      nas_logger.error("Received GUTI-Attach Request from attached user.");
      srsran::console("Received GUTI-Attach Request from attached user.\n");

      // Delete previous Ctx, restart authentication
      // Detaching previoulsy attached UE.
      gtpc->send_delete_session_request(emm_ctx->imsi);
      if (ecm_ctx->mme_ue_s1ap_id != 0) {
        s1ap->send_ue_context_release_command(ecm_ctx->mme_ue_s1ap_id);
      }
    }
    sec_ctx->ul_nas_count = 0;
    sec_ctx->dl_nas_count = 0;

    // Create new MME UE S1AP Identity
    uint32_t new_mme_ue_s1ap_id = s1ap->get_next_mme_ue_s1ap_id();

    // Make sure context from previous NAS connections is not present
    if (ecm_ctx->mme_ue_s1ap_id != 0) {
      s1ap->release_ue_ecm_ctx(ecm_ctx->mme_ue_s1ap_id);
    }
    ecm_ctx->mme_ue_s1ap_id = s1ap->get_next_mme_ue_s1ap_id();

    // Set EMM as de-registered
    emm_ctx->state = EMM_STATE_DEREGISTERED;
    // Save Attach type
    emm_ctx->attach_type = attach_req.eps_attach_type;

    // Set eNB information
    ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
    memcpy(&ecm_ctx->enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));
    // Save whether secure ESM information transfer is necessary
    ecm_ctx->eit = pdn_con_req.esm_info_transfer_flag_present;

    // Initialize E-RABs
    for (uint i = 0; i < MAX_ERABS_PER_UE; i++) {
      nas_ctx->m_esm_ctx[i].state   = ERAB_DEACTIVATED;
      nas_ctx->m_esm_ctx[i].erab_id = i;
    }
    // Store context based on MME UE S1AP id
    s1ap->add_nas_ctx_to_mme_ue_s1ap_id_map(nas_ctx);
    s1ap->add_ue_to_enb_set(enb_sri->sinfo_assoc_id, ecm_ctx->mme_ue_s1ap_id);

    // NAS integrity failed. Re-start authentication process.
    srsran::console("GUTI Attach request NAS integrity failed.\n");
    srsran::console("RE-starting authentication procedure.\n");

    // Get Authentication Vectors from HSS
    if (!hss->gen_auth_info_answer(emm_ctx->imsi, sec_ctx->k_asme, sec_ctx->autn, sec_ctx->rand, sec_ctx->xres)) {
      srsran::console("User not found. IMSI %015" PRIu64 "\n", emm_ctx->imsi);
      nas_logger.info("User not found. IMSI %015" PRIu64 "", emm_ctx->imsi);
      return false;
    }

    // Restarting security context. Reseting eKSI to 0.
    sec_ctx->eksi = 0;
    nas_tx        = srsran::make_byte_buffer();
    if (nas_tx == nullptr) {
      nas_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return false;
    }
    nas_ctx->pack_authentication_request(nas_tx.get());

    // Send reply to eNB
    s1ap->send_downlink_nas_transport(ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id, nas_tx.get(), *enb_sri);
    nas_logger.info("Downlink NAS: Sent Authentication Request");
    srsran::console("Downlink NAS: Sent Authentication Request\n");
    return true;
  }
}

// Service Requests
bool nas::handle_service_request(uint32_t                m_tmsi,
                                 uint32_t                enb_ue_s1ap_id,
                                 struct sctp_sndrcvinfo* enb_sri,
                                 srsran::byte_buffer_t*  nas_rx,
                                 const nas_init_t&       args,
                                 const nas_if_t&         itf)
{
  auto& nas_logger = srslog::fetch_basic_logger("NAS");

  nas_logger.info("Service request -- S-TMSI 0x%x", m_tmsi);
  srsran::console("Service request -- S-TMSI 0x%x\n", m_tmsi);
  nas_logger.info("Service request -- eNB UE S1AP Id %d", enb_ue_s1ap_id);
  srsran::console("Service request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id);

  bool                                  mac_valid = false;
  LIBLTE_MME_SERVICE_REQUEST_MSG_STRUCT service_req;

  // Interfaces
  s1ap_interface_nas* s1ap = itf.s1ap;
  hss_interface_nas*  hss  = itf.hss;
  gtpc_interface_nas* gtpc = itf.gtpc;
  mme_interface_nas*  mme  = itf.mme;

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_service_request_msg((LIBLTE_BYTE_MSG_STRUCT*)nas_rx, &service_req);
  if (err != LIBLTE_SUCCESS) {
    nas_logger.error("Could not unpack service request");
    return false;
  }

  uint64_t imsi = s1ap->find_imsi_from_m_tmsi(m_tmsi);
  if (imsi == 0) {
    srsran::console("Could not find IMSI from M-TMSI. M-TMSI 0x%x\n", m_tmsi);
    nas_logger.error("Could not find IMSI from M-TMSI. M-TMSI 0x%x", m_tmsi);
    nas nas_tmp(args, itf);
    nas_tmp.m_ecm_ctx.enb_ue_s1ap_id = enb_ue_s1ap_id;
    nas_tmp.m_ecm_ctx.mme_ue_s1ap_id = s1ap->get_next_mme_ue_s1ap_id();

    srsran::unique_byte_buffer_t nas_tx = srsran::make_byte_buffer();
    if (nas_tx == nullptr) {
      nas_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return false;
    }
    nas_tmp.pack_service_reject(nas_tx.get(), LIBLTE_MME_EMM_CAUSE_IMPLICITLY_DETACHED);
    s1ap->send_downlink_nas_transport(enb_ue_s1ap_id, nas_tmp.m_ecm_ctx.mme_ue_s1ap_id, nas_tx.get(), *enb_sri);
    return true;
  }

  nas* nas_ctx = s1ap->find_nas_ctx_from_imsi(imsi);
  if (nas_ctx == NULL || nas_ctx->m_emm_ctx.state != EMM_STATE_REGISTERED) {
    srsran::console("UE is not EMM-Registered.\n");
    nas_logger.error("UE is not EMM-Registered.");
    nas nas_tmp(args, itf);
    nas_tmp.m_ecm_ctx.enb_ue_s1ap_id = enb_ue_s1ap_id;
    nas_tmp.m_ecm_ctx.mme_ue_s1ap_id = s1ap->get_next_mme_ue_s1ap_id();

    srsran::unique_byte_buffer_t nas_tx = srsran::make_byte_buffer();
    if (nas_tx == nullptr) {
      nas_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return false;
    }
    nas_tmp.pack_service_reject(nas_tx.get(), LIBLTE_MME_EMM_CAUSE_IMPLICITLY_DETACHED);
    s1ap->send_downlink_nas_transport(enb_ue_s1ap_id, nas_tmp.m_ecm_ctx.mme_ue_s1ap_id, nas_tx.get(), *enb_sri);
    return true;
  }
  emm_ctx_t* emm_ctx = &nas_ctx->m_emm_ctx;
  ecm_ctx_t* ecm_ctx = &nas_ctx->m_ecm_ctx;
  sec_ctx_t* sec_ctx = &nas_ctx->m_sec_ctx;

  mac_valid = nas_ctx->short_integrity_check(nas_rx);
  if (mac_valid) {
    srsran::console("Service Request -- Short MAC valid\n");
    nas_logger.info("Service Request -- Short MAC valid");
    if (ecm_ctx->state == ECM_STATE_CONNECTED) {
      // Release previous context
      nas_logger.info("Service Request -- Releasing previouse ECM context. eNB S1AP Id %d, MME UE S1AP Id %d",
                      ecm_ctx->enb_ue_s1ap_id,
                      ecm_ctx->mme_ue_s1ap_id);
      s1ap->send_ue_context_release_command(ecm_ctx->mme_ue_s1ap_id);
      s1ap->release_ue_ecm_ctx(ecm_ctx->mme_ue_s1ap_id);
    }

    ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;

    // UE not connect. Connect normally.
    srsran::console("Service Request -- User is ECM DISCONNECTED\n");
    nas_logger.info("Service Request -- User is ECM DISCONNECTED");

    // Create ECM context
    ecm_ctx->mme_ue_s1ap_id = s1ap->get_next_mme_ue_s1ap_id();

    // Set eNB information
    ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
    memcpy(&ecm_ctx->enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));

    // Save whether secure ESM information transfer is necessary
    ecm_ctx->eit = false;

    // Get UE IP, and uplink F-TEID
    if (emm_ctx->ue_ip.s_addr == 0) {
      nas_logger.error("UE has no valid IP assigned upon reception of service request");
    }

    srsran::console("UE previously assigned IP: %s\n", inet_ntoa(emm_ctx->ue_ip));

    // Re-generate K_eNB
    srsran::security_generate_k_enb(sec_ctx->k_asme, sec_ctx->ul_nas_count, sec_ctx->k_enb);
    nas_logger.info("Generating KeNB with UL NAS COUNT: %d", sec_ctx->ul_nas_count);
    srsran::console("Generating KeNB with UL NAS COUNT: %d\n", sec_ctx->ul_nas_count);
    nas_logger.info(sec_ctx->k_enb, 32, "Key eNodeB (k_enb)");
    srsran::console("UE Ctr TEID %d\n", emm_ctx->sgw_ctrl_fteid.teid);

    // Stop T3413 if running
    if (mme->is_nas_timer_running(T_3413, emm_ctx->imsi)) {
      mme->remove_nas_timer(T_3413, emm_ctx->imsi);
    }

    // Save UE ctx to MME UE S1AP id
    s1ap->add_nas_ctx_to_mme_ue_s1ap_id_map(nas_ctx);
    s1ap->send_initial_context_setup_request(imsi, 5);
    sec_ctx->ul_nas_count++;
  } else {
    srsran::console("Service Request -- Short MAC invalid\n");
    nas_logger.info("Service Request -- Short MAC invalid");
    if (ecm_ctx->state == ECM_STATE_CONNECTED) {
      // Release previous context
      nas_logger.info("Service Request -- Releasing previouse ECM context. eNB S1AP Id %d, MME UE S1AP Id %d",
                      ecm_ctx->enb_ue_s1ap_id,
                      ecm_ctx->mme_ue_s1ap_id);
      s1ap->send_ue_context_release_command(ecm_ctx->mme_ue_s1ap_id);
      s1ap->release_ue_ecm_ctx(ecm_ctx->mme_ue_s1ap_id);
    }

    // Reset and store context with new mme s1ap id
    nas_ctx->reset();
    memcpy(&ecm_ctx->enb_sri, enb_sri, sizeof(struct sctp_sndrcvinfo));
    ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
    ecm_ctx->mme_ue_s1ap_id = s1ap->get_next_mme_ue_s1ap_id();
    s1ap->add_nas_ctx_to_mme_ue_s1ap_id_map(nas_ctx);
    s1ap->add_ue_to_enb_set(enb_sri->sinfo_assoc_id, nas_ctx->m_ecm_ctx.mme_ue_s1ap_id);
    srsran::unique_byte_buffer_t nas_tx = srsran::make_byte_buffer();
    if (nas_tx == nullptr) {
      nas_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return false;
    }
    nas_ctx->pack_service_reject(nas_tx.get(), LIBLTE_MME_EMM_CAUSE_UE_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK);
    s1ap->send_downlink_nas_transport(ecm_ctx->enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id, nas_tx.get(), *enb_sri);

    srsran::console("Service Request -- Short MAC invalid. Sending service reject.\n");
    nas_logger.warning("Service Request -- Short MAC invalid. Sending service reject.");
    nas_logger.info("Service Reject -- eNB_UE_S1AP_ID %d MME_UE_S1AP_ID %d.", enb_ue_s1ap_id, ecm_ctx->mme_ue_s1ap_id);
  }
  return true;
}

bool nas::handle_detach_request(uint32_t                m_tmsi,
                                uint32_t                enb_ue_s1ap_id,
                                struct sctp_sndrcvinfo* enb_sri,
                                srsran::byte_buffer_t*  nas_rx,
                                const nas_init_t&       args,
                                const nas_if_t&         itf)
{
  auto& nas_logger = srslog::fetch_basic_logger("NAS");

  nas_logger.info("Detach Request -- S-TMSI 0x%x", m_tmsi);
  srsran::console("Detach Request -- S-TMSI 0x%x\n", m_tmsi);
  nas_logger.info("Detach Request -- eNB UE S1AP Id %d", enb_ue_s1ap_id);
  srsran::console("Detach Request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id);

  bool                                 mac_valid = false;
  LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT detach_req;

  // Interfaces
  s1ap_interface_nas* s1ap = itf.s1ap;
  hss_interface_nas*  hss  = itf.hss;
  gtpc_interface_nas* gtpc = itf.gtpc;

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_detach_request_msg((LIBLTE_BYTE_MSG_STRUCT*)nas_rx, &detach_req);
  if (err != LIBLTE_SUCCESS) {
    nas_logger.error("Could not unpack detach request");
    return false;
  }

  uint64_t imsi = s1ap->find_imsi_from_m_tmsi(m_tmsi);
  if (imsi == 0) {
    srsran::console("Could not find IMSI from M-TMSI. M-TMSI 0x%x\n", m_tmsi);
    nas_logger.error("Could not find IMSI from M-TMSI. M-TMSI 0x%x", m_tmsi);
    return true;
  }

  nas* nas_ctx = s1ap->find_nas_ctx_from_imsi(imsi);
  if (nas_ctx == NULL) {
    srsran::console("Could not find UE context from IMSI\n");
    nas_logger.error("Could not find UE context from IMSI");
    return true;
  }

  emm_ctx_t* emm_ctx = &nas_ctx->m_emm_ctx;
  ecm_ctx_t* ecm_ctx = &nas_ctx->m_ecm_ctx;
  sec_ctx_t* sec_ctx = &nas_ctx->m_sec_ctx;

  // TS 24.301, Sec 5.5.2.2.1, UE initiated detach request
  if (detach_req.detach_type.switch_off == 0) {
    // UE expects detach accept
    srsran::unique_byte_buffer_t nas_tx = srsran::make_byte_buffer();
    if (nas_tx == nullptr) {
      nas_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return false;
    }

    LIBLTE_MME_DETACH_ACCEPT_MSG_STRUCT detach_accept = {};
    err                                               = liblte_mme_pack_detach_accept_msg(&detach_accept,
                                            LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS,
                                            sec_ctx->dl_nas_count,
                                            (LIBLTE_BYTE_MSG_STRUCT*)nas_tx.get());
    if (err != LIBLTE_SUCCESS) {
      nas_logger.error("Error packing Detach Accept\n");
    }

    nas_logger.info("Sending detach accept.\n");
    sec_ctx->dl_nas_count++;
    s1ap->send_downlink_nas_transport(enb_ue_s1ap_id, s1ap->get_next_mme_ue_s1ap_id(), nas_tx.get(), *enb_sri);
  } else {
    nas_logger.info("UE is switched off\n");
  }

  gtpc->send_delete_session_request(emm_ctx->imsi);
  emm_ctx->state = EMM_STATE_DEREGISTERED;
  sec_ctx->ul_nas_count++;

  // Mark E-RABs as de-activated
  for (esm_ctx_t& esm_ctx : nas_ctx->m_esm_ctx) {
    esm_ctx.state = ERAB_DEACTIVATED;
  }

  srsran::console("Received. M-TMSI 0x%x\n", m_tmsi);
  // Received detach request as an initial UE message
  // eNB created new ECM context to send the detach request; this needs to be cleared.
  ecm_ctx->mme_ue_s1ap_id = s1ap->get_next_mme_ue_s1ap_id();
  ecm_ctx->enb_ue_s1ap_id = enb_ue_s1ap_id;
  s1ap->send_ue_context_release_command(ecm_ctx->mme_ue_s1ap_id);
  return true;
}

bool nas::handle_tracking_area_update_request(uint32_t                m_tmsi,
                                              uint32_t                enb_ue_s1ap_id,
                                              struct sctp_sndrcvinfo* enb_sri,
                                              srsran::byte_buffer_t*  nas_rx,
                                              const nas_init_t&       args,
                                              const nas_if_t&         itf)
{
  auto& nas_logger = srslog::fetch_basic_logger("NAS");

  nas_logger.info("Tracking Area Update Request -- S-TMSI 0x%x", m_tmsi);
  srsran::console("Tracking Area Update Request -- S-TMSI 0x%x\n", m_tmsi);
  nas_logger.info("Tracking Area Update Request -- eNB UE S1AP Id %d", enb_ue_s1ap_id);
  srsran::console("Tracking Area Update Request -- eNB UE S1AP Id %d\n", enb_ue_s1ap_id);

  srsran::console("Warning: Tracking area update requests are not handled yet.\n");
  nas_logger.warning("Tracking area update requests are not handled yet.");

  // Interfaces
  s1ap_interface_nas* s1ap = itf.s1ap;
  hss_interface_nas*  hss  = itf.hss;
  gtpc_interface_nas* gtpc = itf.gtpc;

  // TODO don't search for NAS ctxt, just send that reject
  // with context we could enable integrity protection

  nas nas_tmp(args, itf);
  nas_tmp.m_ecm_ctx.enb_ue_s1ap_id = enb_ue_s1ap_id;
  nas_tmp.m_ecm_ctx.mme_ue_s1ap_id = s1ap->get_next_mme_ue_s1ap_id();

  srsran::unique_byte_buffer_t nas_tx = srsran::make_byte_buffer();
  if (nas_tx == nullptr) {
    nas_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return false;
  }
  nas_tmp.pack_tracking_area_update_reject(nas_tx.get(), LIBLTE_MME_EMM_CAUSE_IMPLICITLY_DETACHED);
  s1ap->send_downlink_nas_transport(enb_ue_s1ap_id, nas_tmp.m_ecm_ctx.mme_ue_s1ap_id, nas_tx.get(), *enb_sri);
  return true;
}

/***************************************
 *
 * Handle Uplink NAS Transport messages
 *
 ***************************************/
bool nas::handle_attach_request(srsran::byte_buffer_t* nas_rx)
{
  uint32_t                                       m_tmsi      = 0;
  uint64_t                                       imsi        = 0;
  LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT           attach_req  = {};
  LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT pdn_con_req = {};

  // Get NAS Attach Request and PDN connectivity request messages
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_attach_request_msg((LIBLTE_BYTE_MSG_STRUCT*)nas_rx, &attach_req);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error unpacking NAS attach request. Error: %s", liblte_error_text[err]);
    return false;
  }
  // Get PDN Connectivity Request*/
  err = liblte_mme_unpack_pdn_connectivity_request_msg(&attach_req.esm_msg, &pdn_con_req);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error unpacking NAS PDN Connectivity Request. Error: %s", liblte_error_text[err]);
    return false;
  }

  // Get UE IMSI
  if (attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI) {
    for (int i = 0; i <= 14; i++) {
      imsi += attach_req.eps_mobile_id.imsi[i] * std::pow(10, 14 - i);
    }
    srsran::console("Attach request -- IMSI: %015" PRIu64 "\n", imsi);
    m_logger.info("Attach request -- IMSI: %015" PRIu64 "", imsi);
  } else if (attach_req.eps_mobile_id.type_of_id == LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI) {
    m_tmsi = attach_req.eps_mobile_id.guti.m_tmsi;
    imsi   = m_s1ap->find_imsi_from_m_tmsi(m_tmsi);
    srsran::console("Attach request -- M-TMSI: 0x%x\n", m_tmsi);
    m_logger.info("Attach request -- M-TMSI: 0x%x", m_tmsi);
  } else {
    m_logger.error("Unhandled Mobile Id type in attach request");
    return false;
  }

  // Is UE known?
  if (m_emm_ctx.imsi == 0) {
    m_logger.info("Attach request from Unkonwn UE");
    // Get IMSI
    uint64_t imsi = 0;
    for (int i = 0; i <= 14; i++) {
      imsi += attach_req.eps_mobile_id.imsi[i] * std::pow(10, 14 - i);
    }

    // Save IMSI, eNB UE S1AP Id, MME UE S1AP Id and make sure UE is EMM_DEREGISTERED
    m_emm_ctx.imsi  = imsi;
    m_emm_ctx.state = EMM_STATE_DEREGISTERED;

    // Save UE network capabilities
    memcpy(&m_sec_ctx.ue_network_cap, &attach_req.ue_network_cap, sizeof(LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT));
    m_sec_ctx.ms_network_cap_present = attach_req.ms_network_cap_present;
    if (attach_req.ms_network_cap_present) {
      memcpy(&m_sec_ctx.ms_network_cap, &attach_req.ms_network_cap, sizeof(LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT));
    }

    uint8_t eps_bearer_id              = pdn_con_req.eps_bearer_id; // TODO: Unused
    m_emm_ctx.procedure_transaction_id = pdn_con_req.proc_transaction_id;

    // Initialize NAS count
    m_sec_ctx.ul_nas_count = 0;
    m_sec_ctx.dl_nas_count = 0;

    // Save whether secure ESM information transfer is necessary
    m_ecm_ctx.eit = pdn_con_req.esm_info_transfer_flag_present;

    // Initialize E-RABs
    for (uint i = 0; i < MAX_ERABS_PER_UE; i++) {
      m_esm_ctx[i].state   = ERAB_DEACTIVATED;
      m_esm_ctx[i].erab_id = i;
    }

    // Save attach request type
    m_emm_ctx.attach_type = attach_req.eps_attach_type;

    // Get Authentication Vectors from HSS
    if (!m_hss->gen_auth_info_answer(
            m_emm_ctx.imsi, m_sec_ctx.k_asme, m_sec_ctx.autn, m_sec_ctx.rand, m_sec_ctx.xres)) {
      srsran::console("User not found. IMSI %015" PRIu64 "\n", m_emm_ctx.imsi);
      m_logger.info("User not found. IMSI %015" PRIu64 "", m_emm_ctx.imsi);
      return false;
    }

    // Allocate eKSI for this authentication vector
    // Here we assume a new security context thus a new eKSI
    m_sec_ctx.eksi = 0;

    // Save the UE context
    m_s1ap->add_nas_ctx_to_imsi_map(this);

    // Pack NAS Authentication Request in Downlink NAS Transport msg
    srsran::unique_byte_buffer_t nas_tx = srsran::make_byte_buffer();
    if (nas_tx == nullptr) {
      m_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return false;
    }
    pack_authentication_request(nas_tx.get());

    // Send reply to eNB
    m_s1ap->send_downlink_nas_transport(
        m_ecm_ctx.enb_ue_s1ap_id, m_ecm_ctx.mme_ue_s1ap_id, nas_tx.get(), m_ecm_ctx.enb_sri);

    m_logger.info("DL NAS: Sending Authentication Request");
    srsran::console("DL NAS: Sending Authentication Request\n");
    return true;
  } else {
    m_logger.error("Attach request from known UE");
  }
  return true;
}

bool nas::handle_pdn_connectivity_request(srsran::byte_buffer_t* nas_rx)
{
  LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT pdn_con_req = {};

  // Get PDN connectivity request messages
  LIBLTE_ERROR_ENUM err =
      liblte_mme_unpack_pdn_connectivity_request_msg((LIBLTE_BYTE_MSG_STRUCT*)nas_rx->msg, &pdn_con_req);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error unpacking NAS PDN Connectivity Request. Error: %s", liblte_error_text[err]);
    return false;
  }

  // Send PDN connectivity reject
  srsran::unique_byte_buffer_t nas_tx = srsran::make_byte_buffer();
  if (nas_tx == nullptr) {
    m_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return false;
  }

  LIBLTE_MME_PDN_CONNECTIVITY_REJECT_MSG_STRUCT pdn_con_reject = {};
  pdn_con_reject.eps_bearer_id                                 = pdn_con_req.eps_bearer_id;
  pdn_con_reject.proc_transaction_id                           = pdn_con_req.proc_transaction_id;
  pdn_con_reject.esm_cause                                     = LIBLTE_MME_ESM_CAUSE_SERVICE_OPTION_NOT_SUPPORTED;

  err = liblte_mme_pack_pdn_connectivity_reject_msg(&pdn_con_reject, (LIBLTE_BYTE_MSG_STRUCT*)nas_tx.get());
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error packing PDN connectivity reject");
    srsran::console("Error packing PDN connectivity reject\n");
    return false;
  }

  // Send reply to eNB
  m_s1ap->send_downlink_nas_transport(
      m_ecm_ctx.enb_ue_s1ap_id, m_ecm_ctx.mme_ue_s1ap_id, nas_tx.get(), m_ecm_ctx.enb_sri);

  m_logger.info("DL NAS: Sending PDN Connectivity Reject");
  srsran::console("DL NAS: Sending PDN Connectivity Reject\n");

  return true;
}

bool nas::handle_authentication_response(srsran::byte_buffer_t* nas_rx)
{
  LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT auth_resp = {};
  bool                                          ue_valid  = true;

  // Get NAS authentication response
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_authentication_response_msg((LIBLTE_BYTE_MSG_STRUCT*)nas_rx, &auth_resp);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error unpacking NAS authentication response. Error: %s", liblte_error_text[err]);
    return false;
  }

  // Log received authentication response
  srsran::console("Authentication Response -- IMSI %015" PRIu64 "\n", m_emm_ctx.imsi);
  m_logger.info("Authentication Response -- IMSI %015" PRIu64 "", m_emm_ctx.imsi);
  m_logger.info(auth_resp.res, 8, "Authentication response -- RES");
  m_logger.info(m_sec_ctx.xres, 8, "Authentication response -- XRES");

  // Check UE authentication
  for (int i = 0; i < 8; i++) {
    if (auth_resp.res[i] != m_sec_ctx.xres[i]) {
      ue_valid = false;
    }
  }

  srsran::unique_byte_buffer_t nas_tx = srsran::make_byte_buffer();
  if (nas_tx == nullptr) {
    m_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return false;
  }
  if (!ue_valid) {
    // Authentication rejected
    srsran::console("UE Authentication Rejected.\n");
    m_logger.warning("UE Authentication Rejected.");

    // Send back Athentication Reject
    pack_authentication_reject(nas_tx.get());
    m_logger.info("Downlink NAS: Sending Authentication Reject.");
  } else {
    // Authentication accepted
    srsran::console("UE Authentication Accepted.\n");
    m_logger.info("UE Authentication Accepted.");

    // Send Security Mode Command
    m_sec_ctx.ul_nas_count = 0; // Reset the NAS uplink counter for the right key k_enb derivation
    pack_security_mode_command(nas_tx.get());
    srsran::console("Downlink NAS: Sending NAS Security Mode Command.\n");
  }

  // Send reply
  m_s1ap->send_downlink_nas_transport(
      m_ecm_ctx.enb_ue_s1ap_id, m_ecm_ctx.mme_ue_s1ap_id, nas_tx.get(), m_ecm_ctx.enb_sri);
  return true;
}

bool nas::handle_security_mode_complete(srsran::byte_buffer_t* nas_rx)
{
  LIBLTE_MME_SECURITY_MODE_COMPLETE_MSG_STRUCT sm_comp = {};

  // Get NAS security mode complete
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_security_mode_complete_msg((LIBLTE_BYTE_MSG_STRUCT*)nas_rx, &sm_comp);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error unpacking NAS authentication response. Error: %s", liblte_error_text[err]);
    return false;
  }

  // Log security mode complete
  m_logger.info("Security Mode Command Complete -- IMSI: %015" PRIu64 "", m_emm_ctx.imsi);
  srsran::console("Security Mode Command Complete -- IMSI: %015" PRIu64 "\n", m_emm_ctx.imsi);

  // Check wether secure ESM information transfer is required
  srsran::unique_byte_buffer_t nas_tx = srsran::make_byte_buffer();
  if (nas_tx == nullptr) {
    m_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return false;
  }
  if (m_ecm_ctx.eit == true) {
    // Secure ESM information transfer is required
    srsran::console("Sending ESM information request\n");
    m_logger.info("Sending ESM information request");

    // Packing ESM information request
    pack_esm_information_request(nas_tx.get());
    m_s1ap->send_downlink_nas_transport(
        m_ecm_ctx.enb_ue_s1ap_id, m_ecm_ctx.mme_ue_s1ap_id, nas_tx.get(), m_ecm_ctx.enb_sri);
  } else {
    // Secure ESM information transfer not necessary
    // Sending create session request to SP-GW.
    uint8_t default_bearer = 5;
    m_hss->gen_update_loc_answer(m_emm_ctx.imsi, &m_esm_ctx[default_bearer].qci);
    m_logger.debug("Getting subscription information -- QCI %d", m_esm_ctx[default_bearer].qci);
    srsran::console("Getting subscription information -- QCI %d\n", m_esm_ctx[default_bearer].qci);
    m_gtpc->send_create_session_request(m_emm_ctx.imsi);
  }
  return true;
}

bool nas::handle_attach_complete(srsran::byte_buffer_t* nas_rx)
{
  LIBLTE_MME_ATTACH_COMPLETE_MSG_STRUCT                            attach_comp;
  uint8_t                                                          pd, msg_type;
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT act_bearer;

  // Get NAS authentication response
  std::memset(&attach_comp, 0, sizeof(attach_comp));
  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_attach_complete_msg((LIBLTE_BYTE_MSG_STRUCT*)nas_rx, &attach_comp);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error unpacking NAS authentication response. Error: %s", liblte_error_text[err]);
    return false;
  }

  err = liblte_mme_unpack_activate_default_eps_bearer_context_accept_msg((LIBLTE_BYTE_MSG_STRUCT*)&attach_comp.esm_msg,
                                                                         &act_bearer);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error unpacking Activate EPS Bearer Context Accept Msg. Error: %s", liblte_error_text[err]);
    return false;
  }

  srsran::console("Unpacked Attached Complete Message. IMSI %" PRIu64 "\n", m_emm_ctx.imsi);
  srsran::console("Unpacked Activate Default EPS Bearer message. EPS Bearer id %d\n", act_bearer.eps_bearer_id);

  if (act_bearer.eps_bearer_id < 5 || act_bearer.eps_bearer_id > 15) {
    m_logger.error("EPS Bearer ID out of range");
    return false;
  }
  if (m_emm_ctx.state == EMM_STATE_DEREGISTERED) {
    // Attach requested from attach request
    m_gtpc->send_modify_bearer_request(
        m_emm_ctx.imsi, act_bearer.eps_bearer_id, &m_esm_ctx[act_bearer.eps_bearer_id].enb_fteid);

    // Send reply to EMM Info to UE
    srsran::unique_byte_buffer_t nas_tx = srsran::make_byte_buffer();
    if (nas_tx == nullptr) {
      m_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return false;
    }
    pack_emm_information(nas_tx.get());

    m_s1ap->send_downlink_nas_transport(
        m_ecm_ctx.enb_ue_s1ap_id, m_ecm_ctx.mme_ue_s1ap_id, nas_tx.get(), m_ecm_ctx.enb_sri);

    srsran::console("Sending EMM Information\n");
    m_logger.info("Sending EMM Information");
  }
  m_emm_ctx.state = EMM_STATE_REGISTERED;
  return true;
}

bool nas::handle_esm_information_response(srsran::byte_buffer_t* nas_rx)
{
  LIBLTE_MME_ESM_INFORMATION_RESPONSE_MSG_STRUCT esm_info_resp;

  // Get NAS authentication response
  LIBLTE_ERROR_ENUM err =
      srsran_mme_unpack_esm_information_response_msg((LIBLTE_BYTE_MSG_STRUCT*)nas_rx, &esm_info_resp);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error unpacking NAS authentication response. Error: %s", liblte_error_text[err]);
    return false;
  }

  m_logger.info("ESM Info: EPS bearer id %d", esm_info_resp.eps_bearer_id);
  if (esm_info_resp.apn_present) {
    m_logger.info("ESM Info: APN %s", esm_info_resp.apn.apn);
    srsran::console("ESM Info: APN %s\n", esm_info_resp.apn.apn);
  }
  if (esm_info_resp.protocol_cnfg_opts_present) {
    m_logger.info("ESM Info: %d Protocol Configuration Options", esm_info_resp.protocol_cnfg_opts.N_opts);
    srsran::console("ESM Info: %d Protocol Configuration Options\n", esm_info_resp.protocol_cnfg_opts.N_opts);
  }

  // Get subscriber info from HSS
  uint8_t default_bearer = 5;
  m_hss->gen_update_loc_answer(m_emm_ctx.imsi, &m_esm_ctx[default_bearer].qci);
  m_logger.debug("Getting subscription information -- QCI %d", m_esm_ctx[default_bearer].qci);
  srsran::console("Getting subscription information -- QCI %d\n", m_esm_ctx[default_bearer].qci);

  // TODO The packging of GTP-C messages is not ready.
  // This means that GTP-U tunnels are created with function calls, as opposed to GTP-C.
  m_gtpc->send_create_session_request(m_emm_ctx.imsi);
  return true;
}

bool nas::handle_identity_response(srsran::byte_buffer_t* nas_rx)
{
  srsran::unique_byte_buffer_t      nas_tx;
  LIBLTE_MME_ID_RESPONSE_MSG_STRUCT id_resp;

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_identity_response_msg((LIBLTE_BYTE_MSG_STRUCT*)nas_rx, &id_resp);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error unpacking NAS identity response. Error: %s", liblte_error_text[err]);
    return false;
  }

  uint64_t imsi = 0;
  for (int i = 0; i <= 14; i++) {
    imsi += id_resp.mobile_id.imsi[i] * std::pow(10, 14 - i);
  }

  m_logger.info("ID response -- IMSI: %015" PRIu64 "", imsi);
  srsran::console("ID Response -- IMSI: %015" PRIu64 "\n", imsi);

  // Set UE's IMSI
  m_emm_ctx.imsi = imsi;

  // Get Authentication Vectors from HSS
  if (!m_hss->gen_auth_info_answer(imsi, m_sec_ctx.k_asme, m_sec_ctx.autn, m_sec_ctx.rand, m_sec_ctx.xres)) {
    srsran::console("User not found. IMSI %015" PRIu64 "\n", imsi);
    m_logger.info("User not found. IMSI %015" PRIu64 "", imsi);
    return false;
  }
  // Identity reponse from unknown GUTI atach. Assigning new eKSI.
  m_sec_ctx.eksi = 0;

  // Make sure UE context was not previously stored in IMSI map
  nas* nas_ctx = m_s1ap->find_nas_ctx_from_imsi(imsi);
  if (nas_ctx != nullptr) {
    m_logger.warning("UE context already exists.");
    m_s1ap->delete_ue_ctx(imsi);
  }

  // Store UE context im IMSI map
  m_s1ap->add_nas_ctx_to_imsi_map(this);

  // Pack NAS Authentication Request in Downlink NAS Transport msg
  nas_tx = srsran::make_byte_buffer();
  if (nas_tx == nullptr) {
    m_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return false;
  }
  pack_authentication_request(nas_tx.get());

  // Send reply to eNB
  m_s1ap->send_downlink_nas_transport(
      m_ecm_ctx.enb_ue_s1ap_id, m_ecm_ctx.mme_ue_s1ap_id, nas_tx.get(), m_ecm_ctx.enb_sri);

  m_logger.info("Downlink NAS: Sent Authentication Request");
  srsran::console("Downlink NAS: Sent Authentication Request\n");
  return true;
}

bool nas::handle_tracking_area_update_request(srsran::byte_buffer_t* nas_rx)
{
  srsran::console("Warning: Tracking Area Update Request messages not handled yet.\n");
  m_logger.warning("Warning: Tracking Area Update Request messages not handled yet.");

  srsran::unique_byte_buffer_t nas_tx;

  /* TAU handling unsupported, therefore send TAU reject with cause IMPLICITLY DETACHED.
   * this will trigger full re-attach by the UE, instead of going to a TAU request loop */
  nas_tx = srsran::make_byte_buffer();
  if (nas_tx == nullptr) {
    m_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return false;
  }
  // TODO we could enable integrity protection in some cases, but UE should comply anyway
  pack_tracking_area_update_reject(nas_tx.get(), LIBLTE_MME_EMM_CAUSE_IMPLICITLY_DETACHED);
  // Send reply
  m_s1ap->send_downlink_nas_transport(
      m_ecm_ctx.enb_ue_s1ap_id, m_ecm_ctx.mme_ue_s1ap_id, nas_tx.get(), m_ecm_ctx.enb_sri);

  return true;
}

bool nas::handle_authentication_failure(srsran::byte_buffer_t* nas_rx)
{
  m_logger.info("Received Authentication Failure");

  srsran::unique_byte_buffer_t                 nas_tx;
  LIBLTE_MME_AUTHENTICATION_FAILURE_MSG_STRUCT auth_fail;
  LIBLTE_ERROR_ENUM                            err;

  err = liblte_mme_unpack_authentication_failure_msg((LIBLTE_BYTE_MSG_STRUCT*)nas_rx, &auth_fail);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error unpacking NAS authentication failure. Error: %s", liblte_error_text[err]);
    return false;
  }

  switch (auth_fail.emm_cause) {
    case 20:
      srsran::console("MAC code failure\n");
      m_logger.info("MAC code failure");
      break;
    case 26:
      srsran::console("Non-EPS authentication unacceptable\n");
      m_logger.info("Non-EPS authentication unacceptable");
      break;
    case 21:
      srsran::console("Authentication Failure -- Synchronization Failure\n");
      m_logger.info("Authentication Failure -- Synchronization Failure");
      if (auth_fail.auth_fail_param_present == false) {
        m_logger.error("Missing fail parameter");
        return false;
      }
      if (!m_hss->resync_sqn(m_emm_ctx.imsi, auth_fail.auth_fail_param)) {
        srsran::console("Resynchronization failed. IMSI %015" PRIu64 "\n", m_emm_ctx.imsi);
        m_logger.info("Resynchronization failed. IMSI %015" PRIu64 "", m_emm_ctx.imsi);
        return false;
      }
      // Get Authentication Vectors from HSS
      if (!m_hss->gen_auth_info_answer(
              m_emm_ctx.imsi, m_sec_ctx.k_asme, m_sec_ctx.autn, m_sec_ctx.rand, m_sec_ctx.xres)) {
        srsran::console("User not found. IMSI %015" PRIu64 "\n", m_emm_ctx.imsi);
        m_logger.info("User not found. IMSI %015" PRIu64 "", m_emm_ctx.imsi);
        return false;
      }

      // Making sure eKSI is different from previous eKSI.
      m_sec_ctx.eksi = (m_sec_ctx.eksi + 1) % 6;

      // Pack NAS Authentication Request in Downlink NAS Transport msg
      nas_tx = srsran::make_byte_buffer();
      if (nas_tx == nullptr) {
        m_logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
        return false;
      }
      pack_authentication_request(nas_tx.get());

      // Send reply to eNB
      m_s1ap->send_downlink_nas_transport(
          m_ecm_ctx.enb_ue_s1ap_id, m_ecm_ctx.mme_ue_s1ap_id, nas_tx.get(), m_ecm_ctx.enb_sri);

      m_logger.info("Downlink NAS: Sent Authentication Request");
      srsran::console("Downlink NAS: Sent Authentication Request\n");
      // TODO Start T3460 Timer!
      break;
  }
  return true;
}

bool nas::handle_detach_request(srsran::byte_buffer_t* nas_msg)
{
  srsran::console("Detach request -- IMSI %015" PRIu64 "\n", m_emm_ctx.imsi);
  m_logger.info("Detach request -- IMSI %015" PRIu64 "", m_emm_ctx.imsi);
  LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT detach_req;

  LIBLTE_ERROR_ENUM err = liblte_mme_unpack_detach_request_msg((LIBLTE_BYTE_MSG_STRUCT*)nas_msg, &detach_req);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Could not unpack detach request");
    return false;
  }

  m_gtpc->send_delete_session_request(m_emm_ctx.imsi);
  m_emm_ctx.state = EMM_STATE_DEREGISTERED;

  // Mark E-RABs as de-activated
  for (esm_ctx_t& esm_ctx : m_esm_ctx) {
    esm_ctx.state = ERAB_DEACTIVATED;
  }

  if (m_ecm_ctx.mme_ue_s1ap_id != 0) {
    m_s1ap->send_ue_context_release_command(m_ecm_ctx.mme_ue_s1ap_id);
  }
  return true;
}

/*Packing/Unpacking helper functions*/
bool nas::pack_authentication_request(srsran::byte_buffer_t* nas_buffer)
{
  m_logger.info("Packing Authentication Request");

  // Pack NAS msg
  LIBLTE_MME_AUTHENTICATION_REQUEST_MSG_STRUCT auth_req;
  memcpy(auth_req.autn, m_sec_ctx.autn, 16);
  memcpy(auth_req.rand, m_sec_ctx.rand, 16);
  auth_req.nas_ksi.tsc_flag = LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
  auth_req.nas_ksi.nas_ksi  = m_sec_ctx.eksi;

  LIBLTE_ERROR_ENUM err = liblte_mme_pack_authentication_request_msg(&auth_req, (LIBLTE_BYTE_MSG_STRUCT*)nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error packing Authentication Request");
    srsran::console("Error packing Authentication Request\n");
    return false;
  }
  return true;
}

bool nas::pack_authentication_reject(srsran::byte_buffer_t* nas_buffer)
{
  m_logger.info("Packing Authentication Reject");

  LIBLTE_MME_AUTHENTICATION_REJECT_MSG_STRUCT auth_rej;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_authentication_reject_msg(&auth_rej, (LIBLTE_BYTE_MSG_STRUCT*)nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error packing Authentication Reject");
    srsran::console("Error packing Authentication Reject\n");
    return false;
  }
  return true;
}

bool nas::pack_security_mode_command(srsran::byte_buffer_t* nas_buffer)
{
  m_logger.info("Packing Security Mode Command");

  // Pack NAS PDU
  LIBLTE_MME_SECURITY_MODE_COMMAND_MSG_STRUCT sm_cmd;

  sm_cmd.selected_nas_sec_algs.type_of_eea = (LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_ENUM)m_sec_ctx.cipher_algo;
  sm_cmd.selected_nas_sec_algs.type_of_eia = (LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_ENUM)m_sec_ctx.integ_algo;

  sm_cmd.nas_ksi.tsc_flag = LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
  sm_cmd.nas_ksi.nas_ksi  = m_sec_ctx.eksi;

  // Replay UE security cap
  memcpy(sm_cmd.ue_security_cap.eea, m_sec_ctx.ue_network_cap.eea, 8 * sizeof(bool));
  memcpy(sm_cmd.ue_security_cap.eia, m_sec_ctx.ue_network_cap.eia, 8 * sizeof(bool));

  sm_cmd.ue_security_cap.uea_present = m_sec_ctx.ue_network_cap.uea_present;
  memcpy(sm_cmd.ue_security_cap.uea, m_sec_ctx.ue_network_cap.uea, 8 * sizeof(bool));

  sm_cmd.ue_security_cap.uia_present = m_sec_ctx.ue_network_cap.uia_present;
  memcpy(sm_cmd.ue_security_cap.uia, m_sec_ctx.ue_network_cap.uia, 8 * sizeof(bool));

  sm_cmd.ue_security_cap.gea_present = m_sec_ctx.ms_network_cap_present;
  memcpy(sm_cmd.ue_security_cap.gea, m_sec_ctx.ms_network_cap.gea, 8 * sizeof(bool));

  sm_cmd.imeisv_req_present = m_request_imeisv;
  if (m_request_imeisv) {
    sm_cmd.imeisv_req = LIBLTE_MME_IMEISV_REQUESTED;
  }

  sm_cmd.nonce_ue_present  = false;
  sm_cmd.nonce_mme_present = false;

  uint8_t           sec_hdr_type = 3;
  LIBLTE_ERROR_ENUM err          = liblte_mme_pack_security_mode_command_msg(
      &sm_cmd, sec_hdr_type, m_sec_ctx.dl_nas_count, (LIBLTE_BYTE_MSG_STRUCT*)nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    srsran::console("Error packing Authentication Request\n");
    return false;
  }

  // Generate EPS security context
  srsran::security_generate_k_nas(
      m_sec_ctx.k_asme, m_sec_ctx.cipher_algo, m_sec_ctx.integ_algo, m_sec_ctx.k_nas_enc, m_sec_ctx.k_nas_int);

  m_logger.info(m_sec_ctx.k_nas_enc, 32, "Key NAS Encryption (k_nas_enc)");
  m_logger.info(m_sec_ctx.k_nas_int, 32, "Key NAS Integrity (k_nas_int)");

  uint8_t key_enb[32];
  srsran::security_generate_k_enb(m_sec_ctx.k_asme, m_sec_ctx.ul_nas_count, m_sec_ctx.k_enb);
  m_logger.info("Generating KeNB with UL NAS COUNT: %d", m_sec_ctx.ul_nas_count);
  srsran::console("Generating KeNB with UL NAS COUNT: %d\n", m_sec_ctx.ul_nas_count);
  m_logger.info(m_sec_ctx.k_enb, 32, "Key eNodeB (k_enb)");

  // Generate MAC for integrity protection
  uint8_t mac[4];
  integrity_generate(nas_buffer, mac);
  memcpy(&nas_buffer->msg[1], mac, 4);
  return true;
}

bool nas::pack_esm_information_request(srsran::byte_buffer_t* nas_buffer)
{
  m_logger.info("Packing ESM Information request");

  LIBLTE_MME_ESM_INFORMATION_REQUEST_MSG_STRUCT esm_info_req;
  esm_info_req.eps_bearer_id       = 0;
  esm_info_req.proc_transaction_id = m_emm_ctx.procedure_transaction_id;

  uint8_t sec_hdr_type = LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED;

  m_sec_ctx.dl_nas_count++;
  LIBLTE_ERROR_ENUM err = srsran_mme_pack_esm_information_request_msg(
      &esm_info_req, sec_hdr_type, m_sec_ctx.dl_nas_count, (LIBLTE_BYTE_MSG_STRUCT*)nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error packing ESM information request");
    srsran::console("Error packing ESM information request\n");
    return false;
  }

  cipher_encrypt(nas_buffer);
  uint8_t mac[4];
  integrity_generate(nas_buffer, mac);
  memcpy(&nas_buffer->msg[1], mac, 4);

  return true;
}

bool nas::pack_attach_accept(srsran::byte_buffer_t* nas_buffer)
{
  m_logger.info("Packing Attach Accept");

  LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT                               attach_accept;
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT act_def_eps_bearer_context_req;

  // Get decimal MCC and MNC
  uint32_t mcc = 0;
  mcc += 0x000F & m_mcc;
  mcc += 10 * ((0x00F0 & m_mcc) >> 4);
  mcc += 100 * ((0x0F00 & m_mcc) >> 8);

  uint32_t mnc = 0;
  if (0xFF00 == (m_mnc & 0xFF00)) {
    // Two digit MNC
    mnc += 0x000F & m_mnc;
    mnc += 10 * ((0x00F0 & m_mnc) >> 4);
  } else {
    // Three digit MNC
    mnc += 0x000F & m_mnc;
    mnc += 10 * ((0x00F0 & m_mnc) >> 4);
    mnc += 100 * ((0x0F00 & m_mnc) >> 8);
  }

  // Attach accept
  attach_accept.eps_attach_result = m_emm_ctx.attach_type;

  // TODO: Set t3412 from config
  attach_accept.t3412.unit  = LIBLTE_MME_GPRS_TIMER_UNIT_1_MINUTE; // GPRS 1 minute unit
  attach_accept.t3412.value = 30;                                  // 30 minute periodic timer

  attach_accept.tai_list.N_tais     = 1;
  attach_accept.tai_list.tai[0].mcc = mcc;
  attach_accept.tai_list.tai[0].mnc = mnc;
  attach_accept.tai_list.tai[0].tac = m_tac;

  m_logger.info("Attach Accept -- MCC 0x%x, MNC 0x%x", m_mcc, m_mnc);

  // Allocate a GUTI ot the UE
  attach_accept.guti_present           = true;
  attach_accept.guti.type_of_id        = 6; // 110 -> GUTI
  attach_accept.guti.guti.mcc          = mcc;
  attach_accept.guti.guti.mnc          = mnc;
  attach_accept.guti.guti.mme_group_id = m_mme_group;
  attach_accept.guti.guti.mme_code     = m_mme_code;
  attach_accept.guti.guti.m_tmsi       = m_s1ap->allocate_m_tmsi(m_emm_ctx.imsi);
  m_logger.debug("Allocated GUTI: MCC %d, MNC %d, MME Group Id %d, MME Code 0x%x, M-TMSI 0x%x",
                 attach_accept.guti.guti.mcc,
                 attach_accept.guti.guti.mnc,
                 attach_accept.guti.guti.mme_group_id,
                 attach_accept.guti.guti.mme_code,
                 attach_accept.guti.guti.m_tmsi);

  memcpy(&m_sec_ctx.guti, &attach_accept.guti, sizeof(LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT));

  // Set up LAI for combined EPS/IMSI attach
  attach_accept.lai_present = true;
  attach_accept.lai.mcc     = mcc;
  attach_accept.lai.mnc     = mnc;
  attach_accept.lai.lac     = m_lac;

  attach_accept.ms_id_present    = true;
  attach_accept.ms_id.type_of_id = LIBLTE_MME_MOBILE_ID_TYPE_TMSI;
  attach_accept.ms_id.tmsi       = attach_accept.guti.guti.m_tmsi;

  // Make sure all unused options are set to false
  attach_accept.emm_cause_present                   = false;
  attach_accept.t3402_present                       = false;
  attach_accept.t3423_present                       = false;
  attach_accept.equivalent_plmns_present            = false;
  attach_accept.emerg_num_list_present              = false;
  attach_accept.eps_network_feature_support_present = false;
  attach_accept.additional_update_result_present    = false;
  attach_accept.t3412_ext_present                   = false;

  // Set activate default eps bearer (esm_ms)
  // Set pdn_addr
  act_def_eps_bearer_context_req.pdn_addr.pdn_type = LIBLTE_MME_PDN_TYPE_IPV4;
  memcpy(act_def_eps_bearer_context_req.pdn_addr.addr, &m_emm_ctx.ue_ip.s_addr, 4);
  // Set eps bearer id
  act_def_eps_bearer_context_req.eps_bearer_id          = 5;
  act_def_eps_bearer_context_req.transaction_id_present = false;
  // set eps_qos
  act_def_eps_bearer_context_req.eps_qos.qci            = m_esm_ctx[5].qci;
  act_def_eps_bearer_context_req.eps_qos.br_present     = false;
  act_def_eps_bearer_context_req.eps_qos.br_ext_present = false;

  // set apn
  strncpy(act_def_eps_bearer_context_req.apn.apn, m_apn.c_str(), LIBLTE_STRING_LEN - 1);
  act_def_eps_bearer_context_req.proc_transaction_id = m_emm_ctx.procedure_transaction_id; // TODO

  // Set DNS server
  act_def_eps_bearer_context_req.protocol_cnfg_opts_present    = true;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.N_opts     = 1;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].id  = 0x0d;
  act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].len = 4;

  struct sockaddr_in dns_addr;
  if (inet_pton(AF_INET, m_dns.c_str(), &(dns_addr.sin_addr)) != 1) {
    m_logger.error("Invalid m_dns: %s", m_dns.c_str());
    srsran::console("Invalid m_dns: %s\n", m_dns.c_str());
    perror("inet_pton");
    return false;
  }
  memcpy(act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[0].contents, &dns_addr.sin_addr.s_addr, 4);

  // Make sure all unused options are set to false
  act_def_eps_bearer_context_req.negotiated_qos_present    = false;
  act_def_eps_bearer_context_req.llc_sapi_present          = false;
  act_def_eps_bearer_context_req.radio_prio_present        = false;
  act_def_eps_bearer_context_req.packet_flow_id_present    = false;
  act_def_eps_bearer_context_req.apn_ambr_present          = false;
  act_def_eps_bearer_context_req.esm_cause_present         = false;
  act_def_eps_bearer_context_req.connectivity_type_present = false;

  uint8_t sec_hdr_type = 2;
  m_sec_ctx.dl_nas_count++;
  liblte_mme_pack_activate_default_eps_bearer_context_request_msg(&act_def_eps_bearer_context_req,
                                                                  &attach_accept.esm_msg);
  liblte_mme_pack_attach_accept_msg(
      &attach_accept, sec_hdr_type, m_sec_ctx.dl_nas_count, (LIBLTE_BYTE_MSG_STRUCT*)nas_buffer);

  // Encrypt NAS message
  cipher_encrypt(nas_buffer);

  // Integrity protect NAS message
  uint8_t mac[4];
  integrity_generate(nas_buffer, mac);
  memcpy(&nas_buffer->msg[1], mac, 4);

  // Log attach accept info
  m_logger.info("Packed Attach Accept");
  return true;
}

bool nas::pack_identity_request(srsran::byte_buffer_t* nas_buffer)
{
  m_logger.info("Packing Identity Request");

  LIBLTE_MME_ID_REQUEST_MSG_STRUCT id_req;
  id_req.id_type        = LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_identity_request_msg(&id_req, (LIBLTE_BYTE_MSG_STRUCT*)nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error packing Identity Request");
    srsran::console("Error packing Identity Request\n");
    return false;
  }
  return true;
}

bool nas::pack_emm_information(srsran::byte_buffer_t* nas_buffer)
{
  m_logger.info("Packing EMM Information");

  LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT emm_info;
  emm_info.full_net_name_present = true;
  memccpy(emm_info.full_net_name.name, m_full_net_name.c_str(), 0, LIBLTE_STRING_LEN);
  emm_info.full_net_name.add_ci   = LIBLTE_MME_ADD_CI_DONT_ADD;
  emm_info.short_net_name_present = true;
  memccpy(emm_info.short_net_name.name, m_short_net_name.c_str(), 0, LIBLTE_STRING_LEN);
  emm_info.short_net_name.add_ci = LIBLTE_MME_ADD_CI_DONT_ADD;

  emm_info.local_time_zone_present         = false;
  emm_info.utc_and_local_time_zone_present = false;
  emm_info.net_dst_present                 = false;

  time_t    now;
  struct tm broken_down_time;
  if ((time(&now) != -1) && (gmtime_r(&now, &broken_down_time) != NULL)) {
    emm_info.utc_and_local_time_zone.year    = broken_down_time.tm_year + 1900;
    emm_info.utc_and_local_time_zone.month   = broken_down_time.tm_mon + 1;
    emm_info.utc_and_local_time_zone.day     = broken_down_time.tm_mday;
    emm_info.utc_and_local_time_zone.hour    = broken_down_time.tm_hour;
    emm_info.utc_and_local_time_zone.minute  = broken_down_time.tm_min;
    emm_info.utc_and_local_time_zone.second  = broken_down_time.tm_sec;
    emm_info.utc_and_local_time_zone.tz      = 0;
    emm_info.utc_and_local_time_zone_present = true;
  } else {
    m_logger.error("Error getting current time: %s", strerror(errno));
  }

  uint8_t sec_hdr_type = LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED;
  m_sec_ctx.dl_nas_count++;
  LIBLTE_ERROR_ENUM err = liblte_mme_pack_emm_information_msg(
      &emm_info, sec_hdr_type, m_sec_ctx.dl_nas_count, (LIBLTE_BYTE_MSG_STRUCT*)nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error packing EMM Information");
    srsran::console("Error packing EMM Information\n");
    return false;
  }

  // Encrypt NAS message
  cipher_encrypt(nas_buffer);

  // Integrity protect NAS message
  uint8_t mac[4];
  integrity_generate(nas_buffer, mac);
  memcpy(&nas_buffer->msg[1], mac, 4);

  m_logger.info("Packed UE EMM information");
  return true;
}

bool nas::pack_service_reject(srsran::byte_buffer_t* nas_buffer, uint8_t emm_cause)
{
  LIBLTE_MME_SERVICE_REJECT_MSG_STRUCT service_rej;
  service_rej.t3442_present = true;
  service_rej.t3442.unit    = LIBLTE_MME_GPRS_TIMER_DEACTIVATED;
  service_rej.t3442.value   = 0;
  service_rej.t3446_present = true;
  service_rej.t3446         = 0;
  service_rej.emm_cause     = emm_cause;

  LIBLTE_ERROR_ENUM err = liblte_mme_pack_service_reject_msg(
      &service_rej, LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS, 0, (LIBLTE_BYTE_MSG_STRUCT*)nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error packing Service Reject");
    srsran::console("Error packing Service Reject\n");
    return false;
  }
  return true;
}

bool nas::pack_tracking_area_update_reject(srsran::byte_buffer_t* nas_buffer, uint8_t emm_cause)
{
  LIBLTE_MME_TRACKING_AREA_UPDATE_REJECT_MSG_STRUCT tau_rej;
  tau_rej.t3446_present = false;
  tau_rej.t3446         = 0;
  tau_rej.emm_cause     = emm_cause;

  if (emm_cause == LIBLTE_MME_EMM_CAUSE_CONGESTION) {
    // Standard would want T3446 set in this case
    m_logger.error("Tracking Area Update Reject EMM Cause set to \"CONGESTION\", but back-off timer not set.");
  }

  LIBLTE_ERROR_ENUM err = liblte_mme_pack_tracking_area_update_reject_msg(
      &tau_rej, LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS, 0, (LIBLTE_BYTE_MSG_STRUCT*)nas_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_logger.error("Error packing Tracking Area Update Reject");
    srsran::console("Error packing Tracking Area Update Reject\n");
    return false;
  }
  return true;
}

/************************
 *
 * Security Functions
 *
 ************************/
bool nas::short_integrity_check(srsran::byte_buffer_t* pdu)
{
  uint8_t  exp_mac[4] = {0x00, 0x00, 0x00, 0x00};
  uint8_t* mac        = &pdu->msg[2];
  int      i;

  if (pdu->N_bytes < 4) {
    m_logger.warning("NAS message to short for short integrity check (pdu len: %d)", pdu->N_bytes);
    return false;
  }

  uint32_t estimated_count = (m_sec_ctx.ul_nas_count & 0xffffffe0) | (pdu->msg[1] & 0x1f);

  switch (m_sec_ctx.integ_algo) {
    case srsran::INTEGRITY_ALGORITHM_ID_EIA0:
      break;
    case srsran::INTEGRITY_ALGORITHM_ID_128_EIA1:
      srsran::security_128_eia1(&m_sec_ctx.k_nas_int[16],
                                estimated_count,
                                0,
                                srsran::SECURITY_DIRECTION_UPLINK,
                                &pdu->msg[0],
                                2,
                                &exp_mac[0]);
      break;
    case srsran::INTEGRITY_ALGORITHM_ID_128_EIA2:
      srsran::security_128_eia2(&m_sec_ctx.k_nas_int[16],
                                estimated_count,
                                0,
                                srsran::SECURITY_DIRECTION_UPLINK,
                                &pdu->msg[0],
                                2,
                                &exp_mac[0]);
      break;
    case srsran::INTEGRITY_ALGORITHM_ID_128_EIA3:
      srsran::security_128_eia3(&m_sec_ctx.k_nas_int[16],
                                estimated_count,
                                0,
                                srsran::SECURITY_DIRECTION_UPLINK,
                                &pdu->msg[0],
                                2,
                                &exp_mac[0]);
      break;
    default:
      break;
  }

  // Check if expected mac equals the sent mac
  for (i = 0; i < 2; i++) {
    if (exp_mac[i + 2] != mac[i]) {
      m_logger.warning("Short integrity check failure. Local: count=%d, [%02x %02x %02x %02x], "
                       "Received: count=%d, [%02x %02x]",
                       estimated_count,
                       exp_mac[0],
                       exp_mac[1],
                       exp_mac[2],
                       exp_mac[3],
                       pdu->msg[1] & 0x1F,
                       mac[0],
                       mac[1]);
      return false;
    }
  }

  m_logger.info("Integrity check ok. Local: count=%d, Received: count=%d", m_sec_ctx.ul_nas_count, pdu->msg[1] & 0x1F);
  m_sec_ctx.ul_nas_count = estimated_count;
  return true;
}

bool nas::integrity_check(srsran::byte_buffer_t* pdu, bool warn_failure)
{
  uint8_t        exp_mac[4] = {};
  const uint8_t* mac        = &pdu->msg[1];

  uint32_t estimated_count = (m_sec_ctx.ul_nas_count & 0xffffff00) | (pdu->msg[5] & 0xff);

  switch (m_sec_ctx.integ_algo) {
    case srsran::INTEGRITY_ALGORITHM_ID_EIA0:
      break;
    case srsran::INTEGRITY_ALGORITHM_ID_128_EIA1:
      srsran::security_128_eia1(&m_sec_ctx.k_nas_int[16],
                                estimated_count,
                                0,
                                srsran::SECURITY_DIRECTION_UPLINK,
                                &pdu->msg[5],
                                pdu->N_bytes - 5,
                                &exp_mac[0]);
      break;
    case srsran::INTEGRITY_ALGORITHM_ID_128_EIA2:
      srsran::security_128_eia2(&m_sec_ctx.k_nas_int[16],
                                estimated_count,
                                0,
                                srsran::SECURITY_DIRECTION_UPLINK,
                                &pdu->msg[5],
                                pdu->N_bytes - 5,
                                &exp_mac[0]);
      break;
    case srsran::INTEGRITY_ALGORITHM_ID_128_EIA3:
      srsran::security_128_eia3(&m_sec_ctx.k_nas_int[16],
                                estimated_count,
                                0,
                                srsran::SECURITY_DIRECTION_UPLINK,
                                &pdu->msg[5],
                                pdu->N_bytes - 5,
                                &exp_mac[0]);
      break;
    default:
      break;
  }
  // Check if expected mac equals the sent mac
  for (int i = 0; i < 4; i++) {
    if (exp_mac[i] != mac[i]) {
      srslog::log_channel& channel = warn_failure ? m_logger.warning : m_logger.info;
      channel("Integrity check failure. Algorithm=EIA%d", (int)m_sec_ctx.integ_algo);
      channel("UL Local: est_count=%d, old_count=%d, MAC=[%02x %02x %02x %02x], "
              "Received: UL count=%d, MAC=[%02x %02x %02x %02x]",
              estimated_count,
              m_sec_ctx.ul_nas_count,
              exp_mac[0],
              exp_mac[1],
              exp_mac[2],
              exp_mac[3],
              pdu->msg[5],
              mac[0],
              mac[1],
              mac[2],
              mac[3]);
      return false;
    }
  }
  m_logger.info("Integrity check ok. Local: count=%d, Received: count=%d", estimated_count, pdu->msg[5]);
  m_sec_ctx.ul_nas_count = estimated_count;

  return true;
}

void nas::integrity_generate(srsran::byte_buffer_t* pdu, uint8_t* mac)
{
  switch (m_sec_ctx.integ_algo) {
    case srsran::INTEGRITY_ALGORITHM_ID_EIA0:
      break;
    case srsran::INTEGRITY_ALGORITHM_ID_128_EIA1:
      srsran::security_128_eia1(&m_sec_ctx.k_nas_int[16],
                                m_sec_ctx.dl_nas_count,
                                0, // Bearer always 0 for NAS
                                srsran::SECURITY_DIRECTION_DOWNLINK,
                                &pdu->msg[5],
                                pdu->N_bytes - 5,
                                mac);
      break;
    case srsran::INTEGRITY_ALGORITHM_ID_128_EIA2:
      srsran::security_128_eia2(&m_sec_ctx.k_nas_int[16],
                                m_sec_ctx.dl_nas_count,
                                0, // Bearer always 0 for NAS
                                srsran::SECURITY_DIRECTION_DOWNLINK,
                                &pdu->msg[5],
                                pdu->N_bytes - 5,
                                mac);
      break;
    case srsran::INTEGRITY_ALGORITHM_ID_128_EIA3:
      srsran::security_128_eia3(&m_sec_ctx.k_nas_int[16],
                                m_sec_ctx.dl_nas_count,
                                0, // Bearer always 0 for NAS
                                srsran::SECURITY_DIRECTION_DOWNLINK,
                                &pdu->msg[5],
                                pdu->N_bytes - 5,
                                mac);
      break;
    default:
      break;
  }
  m_logger.debug("Generating MAC with inputs: Algorithm %s, DL COUNT %d",
                 srsran::integrity_algorithm_id_text[m_sec_ctx.integ_algo],
                 m_sec_ctx.dl_nas_count);
}

void nas::cipher_decrypt(srsran::byte_buffer_t* pdu)
{
  srsran::byte_buffer_t tmp_pdu;
  switch (m_sec_ctx.cipher_algo) {
    case srsran::CIPHERING_ALGORITHM_ID_EEA0:
      break;
    case srsran::CIPHERING_ALGORITHM_ID_128_EEA1:
      srsran::security_128_eea1(&m_sec_ctx.k_nas_enc[16],
                                pdu->msg[5],
                                0, // Bearer always 0 for NAS
                                srsran::SECURITY_DIRECTION_UPLINK,
                                &pdu->msg[6],
                                pdu->N_bytes - 6,
                                &tmp_pdu.msg[6]);
      memcpy(&pdu->msg[6], &tmp_pdu.msg[6], pdu->N_bytes - 6);
      m_logger.debug(tmp_pdu.msg, pdu->N_bytes, "Decrypted");
      break;
    case srsran::CIPHERING_ALGORITHM_ID_128_EEA2:
      srsran::security_128_eea2(&m_sec_ctx.k_nas_enc[16],
                                pdu->msg[5],
                                0, // Bearer always 0 for NAS
                                srsran::SECURITY_DIRECTION_UPLINK,
                                &pdu->msg[6],
                                pdu->N_bytes - 6,
                                &tmp_pdu.msg[6]);
      m_logger.debug(tmp_pdu.msg, pdu->N_bytes, "Decrypted");
      memcpy(&pdu->msg[6], &tmp_pdu.msg[6], pdu->N_bytes - 6);
      break;
    case srsran::CIPHERING_ALGORITHM_ID_128_EEA3:
      srsran::security_128_eea3(&m_sec_ctx.k_nas_enc[16],
                                pdu->msg[5],
                                0, // Bearer always 0 for NAS
                                srsran::SECURITY_DIRECTION_UPLINK,
                                &pdu->msg[6],
                                pdu->N_bytes - 6,
                                &tmp_pdu.msg[6]);
      m_logger.debug(tmp_pdu.msg, pdu->N_bytes, "Decrypted");
      memcpy(&pdu->msg[6], &tmp_pdu.msg[6], pdu->N_bytes - 6);
      break;
    default:
      m_logger.error("Ciphering algorithms not known");
      break;
  }
}

void nas::cipher_encrypt(srsran::byte_buffer_t* pdu)
{
  srsran::byte_buffer_t pdu_tmp;
  switch (m_sec_ctx.cipher_algo) {
    case srsran::CIPHERING_ALGORITHM_ID_EEA0:
      break;
    case srsran::CIPHERING_ALGORITHM_ID_128_EEA1:
      srsran::security_128_eea1(&m_sec_ctx.k_nas_enc[16],
                                pdu->msg[5],
                                0, // Bearer always 0 for NAS
                                srsran::SECURITY_DIRECTION_DOWNLINK,
                                &pdu->msg[6],
                                pdu->N_bytes - 6,
                                &pdu_tmp.msg[6]);
      memcpy(&pdu->msg[6], &pdu_tmp.msg[6], pdu->N_bytes - 6);
      m_logger.debug(pdu_tmp.msg, pdu->N_bytes, "Encrypted");
      break;
    case srsran::CIPHERING_ALGORITHM_ID_128_EEA2:
      srsran::security_128_eea2(&m_sec_ctx.k_nas_enc[16],
                                pdu->msg[5],
                                0, // Bearer always 0 for NAS
                                srsran::SECURITY_DIRECTION_DOWNLINK,
                                &pdu->msg[6],
                                pdu->N_bytes - 6,
                                &pdu_tmp.msg[6]);
      memcpy(&pdu->msg[6], &pdu_tmp.msg[6], pdu->N_bytes - 6);
      m_logger.debug(pdu_tmp.msg, pdu->N_bytes, "Encrypted");
      break;
    case srsran::CIPHERING_ALGORITHM_ID_128_EEA3:
      srsran::security_128_eea3(&m_sec_ctx.k_nas_enc[16],
                                pdu->msg[5],
                                0, // Bearer always 0 for NAS
                                srsran::SECURITY_DIRECTION_DOWNLINK,
                                &pdu->msg[6],
                                pdu->N_bytes - 6,
                                &pdu_tmp.msg[6]);
      memcpy(&pdu->msg[6], &pdu_tmp.msg[6], pdu->N_bytes - 6);
      m_logger.debug(pdu_tmp.msg, pdu->N_bytes, "Encrypted");
      break;
    default:
      m_logger.error("Ciphering algorithm not known");
      break;
  }
}

/**************************
 *
 * Timer related functions
 *
 **************************/
bool nas::start_timer(enum nas_timer_type type)
{
  m_logger.debug("Starting NAS timer");
  bool err = false;
  switch (type) {
    case T_3413:
      err = start_t3413();
      break;
    default:
      m_logger.error("Invalid timer type");
  }
  return err;
}

bool nas::expire_timer(enum nas_timer_type type)
{
  m_logger.debug("NAS timer expired");
  bool err = false;
  switch (type) {
    case T_3413:
      err = expire_t3413();
      break;
    default:
      m_logger.error("Invalid timer type");
  }
  return err;
}

// T3413 -> Paging timer
bool nas::start_t3413()
{
  m_logger.info("Starting T3413 Timer: Timeout value %d", m_t3413);
  if (m_emm_ctx.state != EMM_STATE_REGISTERED) {
    m_logger.error("EMM invalid status to start T3413");
    return false;
  }

  int fdt = timerfd_create(CLOCK_MONOTONIC, 0);
  if (fdt < 0) {
    m_logger.error("Error creating timer. %s", strerror(errno));
    return false;
  }
  struct itimerspec t_value;
  t_value.it_value.tv_sec     = m_t3413;
  t_value.it_value.tv_nsec    = 0;
  t_value.it_interval.tv_sec  = 0;
  t_value.it_interval.tv_nsec = 0;

  if (timerfd_settime(fdt, 0, &t_value, NULL) == -1) {
    m_logger.error("Could not set timer");
    close(fdt);
    return false;
  }

  m_mme->add_nas_timer(fdt, T_3413, m_emm_ctx.imsi); // TODO timers without IMSI?
  return true;
}

bool nas::expire_t3413()
{
  m_logger.info("T3413 expired -- Could not page the ue.");
  srsran::console("T3413 expired -- Could not page the ue.\n");
  if (m_emm_ctx.state != EMM_STATE_REGISTERED) {
    m_logger.error("EMM invalid status upon T3413 expiration");
    return false;
  }
  // Send Paging Failure to the SPGW
  m_gtpc->send_downlink_data_notification_failure_indication(m_emm_ctx.imsi,
                                                             srsran::GTPC_CAUSE_VALUE_UE_NOT_RESPONDING);
  return true;
}

} // namespace srsepc
