/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "srslte/asn1/liblte_rrc.h"
#include "upper/nas.h"
#include "srslte/common/security.h"
#include "srslte/common/bcd_helpers.h"

using namespace srslte;

namespace srsue {


/*********************************************************************
 *   NAS
 ********************************************************************/

nas::nas()
  : state(EMM_STATE_DEREGISTERED), plmn_selection(PLMN_SELECTED), have_guti(false), have_ctxt(false), ip_addr(0), eps_bearer_id(0)
{
  ctxt.rx_count = 0;
  ctxt.tx_count = 0;
}

void nas::init(usim_interface_nas *usim_,
               rrc_interface_nas *rrc_,
               gw_interface_nas *gw_,
               srslte::log *nas_log_,
               srslte::srslte_nas_config_t cfg_)
{
  pool = byte_buffer_pool::get_instance();
  usim = usim_;
  rrc = rrc_;
  gw = gw_;
  nas_log = nas_log_;
  state = EMM_STATE_DEREGISTERED;
  plmn_selection = PLMN_NOT_SELECTED;

  if (!usim->get_home_plmn_id(&home_plmn)) {
    nas_log->error("Getting Home PLMN Id from USIM. Defaulting to 001-01\n");
    home_plmn.mcc = 61441; // This is 001
    home_plmn.mnc = 65281; // This is 01
  }
  cfg     = cfg_;

  if((read_ctxt_file(&ctxt))) {
    usim->generate_nas_keys(ctxt.k_asme, k_nas_enc, k_nas_int,
                            ctxt.cipher_algo, ctxt.integ_algo);
    nas_log->debug_hex(k_nas_enc, 32, "NAS encryption key - k_nas_enc");
    nas_log->debug_hex(k_nas_int, 32, "NAS integrity key - k_nas_int");
    have_guti = true;
    have_ctxt = true;
  }
}

void nas::stop() {
  write_ctxt_file(ctxt);
}

emm_state_t nas::get_state() {
  return state;
}

/*******************************************************************************
 * UE interface
 ******************************************************************************/

void nas::attach_request() {
  nas_log->info("Attach Request\n");
  if (state == EMM_STATE_DEREGISTERED) {
    state = EMM_STATE_REGISTERED_INITIATED;
    if (plmn_selection == PLMN_NOT_SELECTED) {
      nas_log->info("Starting PLMN Search...\n");
      rrc->plmn_search();
    } else if (plmn_selection == PLMN_SELECTED) {
      nas_log->info("Selecting PLMN %s\n", plmn_id_to_string(current_plmn).c_str());
      rrc->plmn_select(current_plmn);
      selecting_plmn = current_plmn;
    }
  } else if (state == EMM_STATE_REGISTERED) {
    nas_log->info("NAS state is registered, connecting to same PLMN\n");
    rrc->plmn_select(current_plmn);
  } else {
    nas_log->info("Attach request ignored. State = %s\n", emm_state_text[state]);
  }
}

void nas::deattach_request() {
  state = EMM_STATE_DEREGISTERED_INITIATED;
  nas_log->info("Dettach request not supported\n");
}

/*******************************************************************************
 * RRC interface
 ******************************************************************************/

void nas::plmn_found(LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id, uint16_t tracking_area_code) {

  // Do not process new PLMN if already selected
  if (plmn_selection == PLMN_SELECTED) {
    return;
  }

  // Check if already registered
  for (uint32_t i=0;i<known_plmns.size();i++) {
    if (plmn_id.mcc == known_plmns[i].mcc && plmn_id.mnc == known_plmns[i].mnc) {
      nas_log->info("Found known PLMN Id=%s\n", plmn_id_to_string(plmn_id).c_str());
      if (plmn_id.mcc == home_plmn.mcc && plmn_id.mnc == home_plmn.mnc) {
        nas_log->info("Connecting Home PLMN Id=%s\n", plmn_id_to_string(plmn_id).c_str());
        rrc->plmn_select(plmn_id);
        selecting_plmn = plmn_id;
      }
      return;
    }
  }

  // Save if new PLMN
  known_plmns.push_back(plmn_id);

  nas_log->info("Found PLMN:  Id=%s, TAC=%d\n", plmn_id_to_string(plmn_id).c_str(),
                tracking_area_code);
  nas_log->console("Found PLMN:  Id=%s, TAC=%d\n", plmn_id_to_string(plmn_id).c_str(),
                tracking_area_code);

  if (plmn_id.mcc == home_plmn.mcc && plmn_id.mnc == home_plmn.mnc) {
    rrc->plmn_select(plmn_id);
    selecting_plmn = plmn_id;
  }

}

// RRC indicates that the UE has gone through all EARFCN and finished PLMN selection
void nas::plmn_search_end() {
  if (known_plmns.size() > 0) {
    nas_log->info("Could not find Home PLMN Id=%s, trying to connect to PLMN Id=%s\n",
                  plmn_id_to_string(home_plmn).c_str(),
                  plmn_id_to_string(known_plmns[0]).c_str());

    nas_log->console("Could not find Home PLMN Id=%s, trying to connect to PLMN Id=%s\n",
                     plmn_id_to_string(home_plmn).c_str(),
                     plmn_id_to_string(known_plmns[0]).c_str());

    rrc->plmn_select(known_plmns[0]);
  } else {
    nas_log->debug("Finished searching PLMN in current EARFCN set but no networks were found.\n");
  }
}

bool nas::is_attached() {
  return state == EMM_STATE_REGISTERED;
}

bool nas::is_attaching() {
  return state == EMM_STATE_REGISTERED_INITIATED;
}

void nas::notify_connection_setup() {
  nas_log->debug("State = %s\n", emm_state_text[state]);
  if (EMM_STATE_REGISTERED_INITIATED == state) {
    send_attach_request();
  } else {
    send_service_request();
  }
}

void nas::write_pdu(uint32_t lcid, byte_buffer_t *pdu) {
  uint8 pd = 0;
  uint8 msg_type = 0;
  uint8 sec_hdr_type = 0;
  bool  mac_valid = false;

  nas_log->info_hex(pdu->msg, pdu->N_bytes, "DL %s PDU", rrc->get_rb_name(lcid).c_str());

  // Parse the message security header
  liblte_mme_parse_msg_sec_header((LIBLTE_BYTE_MSG_STRUCT*)pdu, &pd, &sec_hdr_type);
  switch(sec_hdr_type)
  {
    case LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS:
    case LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_WITH_NEW_EPS_SECURITY_CONTEXT:
    case LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST:
    case LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY:
        break;
    case LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED:
        mac_valid = integrity_check(pdu);
        cipher_decrypt(pdu);
        break;
    case LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED_WITH_NEW_EPS_SECURITY_CONTEXT:
        break;
    default:
      nas_log->error("Not handling NAS message with SEC_HDR_TYPE=%02X\n", sec_hdr_type);
      pool->deallocate(pdu);
      break;
  }

  // Write NAS pcap
  if(pcap != NULL) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  // Parse the message header
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT *) pdu, &pd, &msg_type);
  nas_log->info_hex(pdu->msg, pdu->N_bytes, "DL %s Decrypted PDU", rrc->get_rb_name(lcid).c_str());
  // TODO: Check if message type requieres specical security header type and if it isvalid

  switch (msg_type) {
    case LIBLTE_MME_MSG_TYPE_ATTACH_ACCEPT:
      parse_attach_accept(lcid, pdu);
      break;
    case LIBLTE_MME_MSG_TYPE_ATTACH_REJECT:
      parse_attach_reject(lcid, pdu);
      break;
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_REQUEST:
      parse_authentication_request(lcid, pdu);
      break;
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_REJECT:
      parse_authentication_reject(lcid, pdu);
      break;
    case LIBLTE_MME_MSG_TYPE_IDENTITY_REQUEST:
      parse_identity_request(lcid, pdu);
      break;
    case LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMMAND:
      parse_security_mode_command(lcid, pdu);
      break;
    case LIBLTE_MME_MSG_TYPE_SERVICE_REJECT:
      parse_service_reject(lcid, pdu);
      break;
    case LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_REQUEST:
      parse_esm_information_request(lcid, pdu);
      break;
    case LIBLTE_MME_MSG_TYPE_EMM_INFORMATION:
      parse_emm_information(lcid, pdu);
      break;
    default:
      nas_log->error("Not handling NAS message with MSG_TYPE=%02X\n", msg_type);
      pool->deallocate(pdu);
      break;
  }
}

uint32_t nas::get_ul_count() {
  return ctxt.tx_count;
}

bool nas::get_s_tmsi(LIBLTE_RRC_S_TMSI_STRUCT *s_tmsi) {
  if (have_guti) {
    s_tmsi->mmec   = ctxt.guti.mme_code;
    s_tmsi->m_tmsi = ctxt.guti.m_tmsi;
    return true;
  } else {
    return false;
  }
}

bool nas::get_k_asme(uint8_t *k_asme_, uint32_t n) {
  if(!have_ctxt) {
    nas_log->error("K_asme requested before security context established\n");
    return false;
  }
  if(NULL == k_asme_ || n < 32) {
    nas_log->error("Invalid parameters to get_k_asme");
    return false;
  }

  memcpy(k_asme_, ctxt.k_asme, 32);
  return true;
}

/*******************************************************************************
  PCAP
*******************************************************************************/

void nas::start_pcap(srslte::nas_pcap *pcap_)
{
  pcap = pcap_;
}

/*******************************************************************************
 * Security
 ******************************************************************************/

void nas::integrity_generate(uint8_t *key_128,
                             uint32_t count,
                             uint8_t direction,
                             uint8_t *msg,
                             uint32_t msg_len,
                             uint8_t *mac) {
  switch (ctxt.integ_algo) {
    case INTEGRITY_ALGORITHM_ID_EIA0:
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA1:
      security_128_eia1(key_128,
                        count,
                        0,            // Bearer always 0 for NAS
                        direction,
                        msg,
                        msg_len,
                        mac);
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA2:
      security_128_eia2(key_128,
                        count,
                        0,            // Bearer always 0 for NAS
                        direction,
                        msg,
                        msg_len,
                        mac);
      break;
    default:
      break;
  }
}

// This function depends to a valid k_nas_int.
// This key is generated in the security mode command.

bool nas::integrity_check(byte_buffer_t *pdu)
{
  uint8_t exp_mac[4];
  uint8_t *mac = &pdu->msg[1];
  int i;

  integrity_generate(&k_nas_int[16],
                     ctxt.rx_count,
                     SECURITY_DIRECTION_DOWNLINK,
                     &pdu->msg[5],
                     pdu->N_bytes-5,
                     &exp_mac[0]);

  // Check if expected mac equals the sent mac
  for(i=0; i<4; i++){
    if(exp_mac[i] != mac[i]){
     nas_log->warning("Integrity check failure. Local: count=%d, [%02x %02x %02x %02x], "
                      "Received: count=%d, [%02x %02x %02x %02x]\n",
                      ctxt.rx_count, exp_mac[0], exp_mac[1], exp_mac[2], exp_mac[3],
                      pdu->msg[5], mac[0], mac[1], mac[2], mac[3]);
     return false;
    }
  }
  nas_log->info("Integrity check ok. Local: count=%d, Received: count=%d\n",
                ctxt.rx_count, pdu->msg[5]);
  return true;
}

void nas::cipher_encrypt(byte_buffer_t *pdu)
{
  byte_buffer_t pdu_tmp;
  switch(ctxt.cipher_algo)
  {
  case CIPHERING_ALGORITHM_ID_EEA0:
      break;
  case CIPHERING_ALGORITHM_ID_128_EEA1:
      security_128_eea1(&k_nas_enc[16],
                        pdu->msg[5],
                        0,            // Bearer always 0 for NAS
                        SECURITY_DIRECTION_UPLINK,
                        &pdu->msg[6],
                        pdu->N_bytes-6,
                        &pdu_tmp.msg[6]);
      memcpy(&pdu->msg[6], &pdu_tmp.msg[6], pdu->N_bytes-6);
      break;
  case CIPHERING_ALGORITHM_ID_128_EEA2:
      security_128_eea2(&k_nas_enc[16],
                        pdu->msg[5],
                        0,            // Bearer always 0 for NAS
                        SECURITY_DIRECTION_UPLINK,
                        &pdu->msg[6],
                        pdu->N_bytes-6,
                        &pdu_tmp.msg[6]);
      memcpy(&pdu->msg[6], &pdu_tmp.msg[6], pdu->N_bytes-6);
      break;
  default:
      nas_log->error("Ciphering algorithmus not known");
      break;
  }
}

void nas::cipher_decrypt(byte_buffer_t *pdu)
{
  byte_buffer_t tmp_pdu;
  switch(ctxt.cipher_algo)
  {
  case CIPHERING_ALGORITHM_ID_EEA0:
      break;
  case CIPHERING_ALGORITHM_ID_128_EEA1:
      security_128_eea1(&k_nas_enc[16],
                        pdu->msg[5],
                        0,            // Bearer always 0 for NAS
                        SECURITY_DIRECTION_DOWNLINK,
                        &pdu->msg[6],
                        pdu->N_bytes-6,
                        &tmp_pdu.msg[6]);
      memcpy(&pdu->msg[6], &tmp_pdu.msg[6], pdu->N_bytes-6);
      break;
  case CIPHERING_ALGORITHM_ID_128_EEA2:
      security_128_eea2(&k_nas_enc[16],
                        pdu->msg[5],
                        0,            // Bearer always 0 for NAS
                        SECURITY_DIRECTION_DOWNLINK,
                        &pdu->msg[6],
                        pdu->N_bytes-6,
                        &tmp_pdu.msg[6]);
      nas_log->debug_hex(tmp_pdu.msg, pdu->N_bytes, "Decrypted");
      memcpy(&pdu->msg[6], &tmp_pdu.msg[6], pdu->N_bytes-6);
      break;
    default:
      nas_log->error("Ciphering algorithmus not known");
      break;
  }
}

bool nas::check_cap_replay(LIBLTE_MME_UE_SECURITY_CAPABILITIES_STRUCT *caps)
{
  for(uint32_t i=0; i<8; i++) {
    if(caps->eea[i] != eea_caps[i] || caps->eia[i] != eia_caps[i]) {
      return false;
    }
  }
  return true;
}


/*******************************************************************************
 * Parsers
 ******************************************************************************/

void nas::parse_attach_accept(uint32_t lcid, byte_buffer_t *pdu) {
  LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT attach_accept;
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT act_def_eps_bearer_context_req;
  LIBLTE_MME_ATTACH_COMPLETE_MSG_STRUCT attach_complete;
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT act_def_eps_bearer_context_accept;

  nas_log->info("Received Attach Accept\n");

  liblte_mme_unpack_attach_accept_msg((LIBLTE_BYTE_MSG_STRUCT *) pdu, &attach_accept);

  if (attach_accept.eps_attach_result == LIBLTE_MME_EPS_ATTACH_RESULT_EPS_ONLY) {
    //FIXME: Handle t3412.unit
    //FIXME: Handle tai_list
    if (attach_accept.guti_present) {
      memcpy(&ctxt.guti, &attach_accept.guti.guti, sizeof(LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT));
      have_guti = true;
    }
    if (attach_accept.lai_present) {}
    if (attach_accept.ms_id_present) {}
    if (attach_accept.emm_cause_present) {}
    if (attach_accept.t3402_present) {}
    if (attach_accept.t3412_ext_present) {}
    if (attach_accept.t3423_present) {}
    if (attach_accept.equivalent_plmns_present) {}
    if (attach_accept.emerg_num_list_present) {}
    if (attach_accept.eps_network_feature_support_present) {}
    if (attach_accept.additional_update_result_present) {}

    liblte_mme_unpack_activate_default_eps_bearer_context_request_msg(&attach_accept.esm_msg,
                                                                      &act_def_eps_bearer_context_req);

    if (LIBLTE_MME_PDN_TYPE_IPV4 == act_def_eps_bearer_context_req.pdn_addr.pdn_type) {
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[0] << 24;
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[1] << 16;
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[2] << 8;
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[3];

      nas_log->info("Network attach successful. APN: %s, IP: %u.%u.%u.%u\n",
                    act_def_eps_bearer_context_req.apn.apn.c_str(),
                    act_def_eps_bearer_context_req.pdn_addr.addr[0],
                    act_def_eps_bearer_context_req.pdn_addr.addr[1],
                    act_def_eps_bearer_context_req.pdn_addr.addr[2],
                    act_def_eps_bearer_context_req.pdn_addr.addr[3]);

      nas_log->console("Network attach successful. IP: %u.%u.%u.%u\n",
                       act_def_eps_bearer_context_req.pdn_addr.addr[0],
                       act_def_eps_bearer_context_req.pdn_addr.addr[1],
                       act_def_eps_bearer_context_req.pdn_addr.addr[2],
                       act_def_eps_bearer_context_req.pdn_addr.addr[3]);

      // Setup GW
      char *err_str = NULL;
      if (gw->setup_if_addr(ip_addr, err_str)) {
        nas_log->error("Failed to set gateway address - %s\n", err_str);
      }
    } else {
      nas_log->error("Not handling IPV6 or IPV4V6\n");
      pool->deallocate(pdu);
      return;
    }
    eps_bearer_id = act_def_eps_bearer_context_req.eps_bearer_id;
    if (act_def_eps_bearer_context_req.transaction_id_present) {
      transaction_id = act_def_eps_bearer_context_req.proc_transaction_id;
    }

    //FIXME: Handle the following parameters
//    act_def_eps_bearer_context_req.eps_qos.qci
//    act_def_eps_bearer_context_req.eps_qos.br_present
//    act_def_eps_bearer_context_req.eps_qos.br_ext_present
//    act_def_eps_bearer_context_req.apn.apn
//    act_def_eps_bearer_context_req.negotiated_qos_present
//    act_def_eps_bearer_context_req.llc_sapi_present
//    act_def_eps_bearer_context_req.radio_prio_present
//    act_def_eps_bearer_context_req.packet_flow_id_present
//    act_def_eps_bearer_context_req.apn_ambr_present
//    act_def_eps_bearer_context_req.protocol_cnfg_opts_present
//    act_def_eps_bearer_context_req.connectivity_type_present

    // FIXME: Setup the default EPS bearer context

    state = EMM_STATE_REGISTERED;
    current_plmn = selecting_plmn;

    ctxt.rx_count++;

    // Send EPS bearer context accept and attach complete
    act_def_eps_bearer_context_accept.eps_bearer_id = eps_bearer_id;
    act_def_eps_bearer_context_accept.proc_transaction_id = transaction_id;
    act_def_eps_bearer_context_accept.protocol_cnfg_opts_present = false;
    liblte_mme_pack_activate_default_eps_bearer_context_accept_msg(&act_def_eps_bearer_context_accept,
                                                                   &attach_complete.esm_msg);
    liblte_mme_pack_attach_complete_msg(&attach_complete,
                                        LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED,
                                        ctxt.tx_count,
                                        (LIBLTE_BYTE_MSG_STRUCT *) pdu);
    // Write NAS pcap
    if (pcap != NULL) {
      pcap->write_nas(pdu->msg, pdu->N_bytes);
    }

    cipher_encrypt(pdu);
    integrity_generate(&k_nas_int[16],
                       ctxt.tx_count,
                       SECURITY_DIRECTION_UPLINK,
                       &pdu->msg[5],
                       pdu->N_bytes - 5,
                       &pdu->msg[1]);

    // Instruct RRC to enable capabilities
    rrc->enable_capabilities();

    nas_log->info("Sending Attach Complete\n");
    rrc->write_sdu(lcid, pdu);
    ctxt.tx_count++;

  } else {
    nas_log->info("Not handling attach type %u\n", attach_accept.eps_attach_result);
    state = EMM_STATE_DEREGISTERED;
    pool->deallocate(pdu);
  }
}

void nas::parse_attach_reject(uint32_t lcid, byte_buffer_t *pdu) {
  LIBLTE_MME_ATTACH_REJECT_MSG_STRUCT attach_rej;

  liblte_mme_unpack_attach_reject_msg((LIBLTE_BYTE_MSG_STRUCT *) pdu, &attach_rej);
  nas_log->warning("Received Attach Reject. Cause= %02X\n", attach_rej.emm_cause);
  nas_log->console("Received Attach Reject. Cause= %02X\n", attach_rej.emm_cause);
  state = EMM_STATE_DEREGISTERED;
  pool->deallocate(pdu);
  // FIXME: Command RRC to release?
}

void nas::parse_authentication_request(uint32_t lcid, byte_buffer_t *pdu) {
  LIBLTE_MME_AUTHENTICATION_REQUEST_MSG_STRUCT auth_req;
  LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT auth_res;

  nas_log->info("Received Authentication Request\n");
  liblte_mme_unpack_authentication_request_msg((LIBLTE_BYTE_MSG_STRUCT *) pdu, &auth_req);

  // Reuse the pdu for the response message
  pdu->reset();

  // Generate authentication response using RAND, AUTN & KSI-ASME
  uint16 mcc, mnc;
  mcc = rrc->get_mcc();
  mnc = rrc->get_mnc();

  nas_log->info("MCC=%d, MNC=%d\n", mcc, mnc);

  bool net_valid;
  uint8_t res[16];
  usim->generate_authentication_response(auth_req.rand, auth_req.autn, mcc, mnc,
                                         &net_valid, res, ctxt.k_asme);
  nas_log->info("Generated k_asme=%s\n", hex_to_string(ctxt.k_asme, 32).c_str());
  if(LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE == auth_req.nas_ksi.tsc_flag) {
    ctxt.ksi = auth_req.nas_ksi.nas_ksi;
  } else {
    nas_log->error("NAS mapped security context not currently supported\n");
    nas_log->console("Warning: NAS mapped security context not currently supported\n");
  }

  if (net_valid) {
    nas_log->info("Network authentication successful\n");
    for (int i = 0; i < 8; i++) {
      auth_res.res[i] = res[i];
    }
    liblte_mme_pack_authentication_response_msg(&auth_res, (LIBLTE_BYTE_MSG_STRUCT *) pdu);

    nas_log->info("Sending Authentication Response\n");
    // Write NAS pcap
    if (pcap != NULL) {
      pcap->write_nas(pdu->msg, pdu->N_bytes);
    }
    rrc->write_sdu(lcid, pdu);
  } else {
    nas_log->warning("Network authentication failure\n");
    nas_log->console("Warning: Network authentication failure\n");
    pool->deallocate(pdu);
  }
}

void nas::parse_authentication_reject(uint32_t lcid, byte_buffer_t *pdu) {
  nas_log->warning("Received Authentication Reject\n");
  pool->deallocate(pdu);
  state = EMM_STATE_DEREGISTERED;
  // FIXME: Command RRC to release?
}

void nas::parse_identity_request(uint32_t lcid, byte_buffer_t *pdu) {
  LIBLTE_MME_ID_REQUEST_MSG_STRUCT  id_req;
  LIBLTE_MME_ID_RESPONSE_MSG_STRUCT id_resp;

  liblte_mme_unpack_identity_request_msg((LIBLTE_BYTE_MSG_STRUCT *) pdu, &id_req);
  nas_log->info("Received Identity Request. ID type: %d\n", id_req.id_type);

  switch(id_req.id_type) {
  case LIBLTE_MME_MOBILE_ID_TYPE_IMSI:
    id_resp.mobile_id.type_of_id = LIBLTE_MME_MOBILE_ID_TYPE_IMSI;
    usim->get_imsi_vec(id_resp.mobile_id.imsi, 15);
    break;
  case LIBLTE_MME_MOBILE_ID_TYPE_IMEI:
    id_resp.mobile_id.type_of_id = LIBLTE_MME_MOBILE_ID_TYPE_IMEI;
    usim->get_imei_vec(id_resp.mobile_id.imei, 15);
    break;
  default:
    nas_log->error("Unhandled ID type: %d\n");
    pool->deallocate(pdu);
    return;
  }

  pdu->reset();
  liblte_mme_pack_identity_response_msg(&id_resp, (LIBLTE_BYTE_MSG_STRUCT *) pdu);

  if(pcap != NULL) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  rrc->write_sdu(lcid, pdu);
}

void nas::parse_security_mode_command(uint32_t lcid, byte_buffer_t *pdu)
{
  LIBLTE_MME_SECURITY_MODE_COMMAND_MSG_STRUCT sec_mode_cmd;
  LIBLTE_MME_SECURITY_MODE_COMPLETE_MSG_STRUCT sec_mode_comp;

  liblte_mme_unpack_security_mode_command_msg((LIBLTE_BYTE_MSG_STRUCT *) pdu, &sec_mode_cmd);
  nas_log->info("Received Security Mode Command ksi: %d, eea: %s, eia: %s\n",
                sec_mode_cmd.nas_ksi.nas_ksi,
                ciphering_algorithm_id_text[sec_mode_cmd.selected_nas_sec_algs.type_of_eea],
                integrity_algorithm_id_text[sec_mode_cmd.selected_nas_sec_algs.type_of_eia]);

  if(sec_mode_cmd.nas_ksi.tsc_flag != LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE) {
    nas_log->error("Mapped security context not supported\n");
    pool->deallocate(pdu);
    return;
  }

  if (have_ctxt) {
    if(sec_mode_cmd.nas_ksi.nas_ksi != ctxt.ksi) {
      nas_log->warning("Sending Security Mode Reject due to key set ID mismatch\n");
      send_security_mode_reject(LIBLTE_MME_EMM_CAUSE_SECURITY_MODE_REJECTED_UNSPECIFIED);
      pool->deallocate(pdu);
      return;
    }
  }

  // MME is setting up security context

  // TODO: check nonce (not sent by Amari)

  // Check capabilities replay
  if(!check_cap_replay(&sec_mode_cmd.ue_security_cap)) {
    nas_log->warning("Sending Security Mode Reject due to security capabilities mismatch\n");
    send_security_mode_reject(LIBLTE_MME_EMM_CAUSE_UE_SECURITY_CAPABILITIES_MISMATCH);
    pool->deallocate(pdu);
    return;
  }

  // Reset counters (as per 24.301 5.4.3.2)
  ctxt.rx_count = 0;
  ctxt.tx_count = 0;

  ctxt.cipher_algo = (CIPHERING_ALGORITHM_ID_ENUM) sec_mode_cmd.selected_nas_sec_algs.type_of_eea;
  ctxt.integ_algo  = (INTEGRITY_ALGORITHM_ID_ENUM) sec_mode_cmd.selected_nas_sec_algs.type_of_eia;

  // Check capabilities
  if(!eea_caps[ctxt.cipher_algo] || !eia_caps[ctxt.integ_algo]) {
    nas_log->warning("Sending Security Mode Reject due to security capabilities mismatch\n");
    send_security_mode_reject(LIBLTE_MME_EMM_CAUSE_UE_SECURITY_CAPABILITIES_MISMATCH);
    pool->deallocate(pdu);
    return;
  }

  // Generate NAS keys
  usim->generate_nas_keys(ctxt.k_asme, k_nas_enc, k_nas_int,
                          ctxt.cipher_algo, ctxt.integ_algo);
  nas_log->debug_hex(k_nas_enc, 32, "NAS encryption key - k_nas_enc");
  nas_log->debug_hex(k_nas_int, 32, "NAS integrity key - k_nas_int");

  nas_log->debug("Generating integrity check. integ_algo:%d, count_dl:%d, lcid:%d\n",
                 ctxt.integ_algo, ctxt.rx_count, lcid);

  if (integrity_check(pdu) != true) {
    nas_log->warning("Sending Security Mode Reject due to integrity check failure\n");
    send_security_mode_reject(LIBLTE_MME_EMM_CAUSE_MAC_FAILURE);
    pool->deallocate(pdu);
    return;
  }

  ctxt.rx_count++;

  // Take security context into use
  have_ctxt = true;

  if (sec_mode_cmd.imeisv_req_present && LIBLTE_MME_IMEISV_REQUESTED == sec_mode_cmd.imeisv_req) {
    sec_mode_comp.imeisv_present = true;
    sec_mode_comp.imeisv.type_of_id = LIBLTE_MME_MOBILE_ID_TYPE_IMEISV;
    usim->get_imei_vec(sec_mode_comp.imeisv.imeisv, 15);
    sec_mode_comp.imeisv.imeisv[14] = 5;
    sec_mode_comp.imeisv.imeisv[15] = 3;
  } else {
    sec_mode_comp.imeisv_present = false;
  }

  // Send response
  byte_buffer_t *sdu = pool_allocate;
  liblte_mme_pack_security_mode_complete_msg(&sec_mode_comp,
                                             LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED_WITH_NEW_EPS_SECURITY_CONTEXT,
                                             ctxt.tx_count,
                                             (LIBLTE_BYTE_MSG_STRUCT *) sdu);
  if(pcap != NULL) {
    pcap->write_nas(sdu->msg, sdu->N_bytes);
  }
  cipher_encrypt(sdu);
  integrity_generate(&k_nas_int[16],
                     ctxt.tx_count,
                     SECURITY_DIRECTION_UPLINK,
                     &sdu->msg[5],
                     sdu->N_bytes - 5,
                     &sdu->msg[1]);
  nas_log->info("Sending Security Mode Complete nas_current_ctxt.tx_count=%d, RB=%s\n",
                ctxt.tx_count,
                rrc->get_rb_name(lcid).c_str());
  rrc->write_sdu(lcid, sdu);
  ctxt.tx_count++;
  pool->deallocate(pdu);
}

void nas::parse_service_reject(uint32_t lcid, byte_buffer_t *pdu) {
  nas_log->error("TODO:parse_service_reject\n");
}

void nas::parse_esm_information_request(uint32_t lcid, byte_buffer_t *pdu) {
  nas_log->error("TODO:parse_esm_information_request\n");

}

void nas::parse_emm_information(uint32_t lcid, byte_buffer_t *pdu) {
  liblte_mme_unpack_emm_information_msg((LIBLTE_BYTE_MSG_STRUCT *) pdu, &emm_info);
  std::string str = emm_info_str(&emm_info);
  nas_log->info("Received EMM Information: %s\n", str.c_str());
  nas_log->console("%s\n", str.c_str());
  ctxt.rx_count++;
}

/*******************************************************************************
 * Senders
 ******************************************************************************/

void nas::send_attach_request() {
  LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT attach_req;
  byte_buffer_t *msg = pool_allocate;
  u_int32_t i;

  attach_req.eps_attach_type = LIBLTE_MME_EPS_ATTACH_TYPE_EPS_ATTACH;

  for (i = 0; i < 8; i++) {
    attach_req.ue_network_cap.eea[i] = eea_caps[i];
    attach_req.ue_network_cap.eia[i] = eia_caps[i];
  }

  attach_req.ue_network_cap.uea_present = false;  // UMTS encryption algos
  attach_req.ue_network_cap.uia_present = false;  // UMTS integrity algos
  attach_req.ms_network_cap_present = false;      // A/Gb mode (2G) or Iu mode (3G)
  attach_req.old_p_tmsi_signature_present = false;
  attach_req.additional_guti_present = false;
  attach_req.last_visited_registered_tai_present = false;
  attach_req.drx_param_present = false;
  attach_req.old_lai_present = false;
  attach_req.tmsi_status_present = false;
  attach_req.ms_cm2_present = false;
  attach_req.ms_cm3_present = false;
  attach_req.supported_codecs_present = false;
  attach_req.additional_update_type_present = false;
  attach_req.voice_domain_pref_and_ue_usage_setting_present = false;
  attach_req.device_properties_present = false;
  attach_req.old_guti_type_present = false;

  // ESM message (PDN connectivity request) for first default bearer
  gen_pdn_connectivity_request(&attach_req.esm_msg);

  // GUTI or IMSI attach
  if(have_guti && have_ctxt) {
    attach_req.eps_mobile_id.type_of_id = LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI;
    memcpy(&attach_req.eps_mobile_id.guti, &ctxt.guti, sizeof(LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT));
    attach_req.old_guti_type         = LIBLTE_MME_GUTI_TYPE_NATIVE;
    attach_req.old_guti_type_present = true;
    attach_req.nas_ksi.tsc_flag      = LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
    attach_req.nas_ksi.nas_ksi       = ctxt.ksi;
    nas_log->info("Requesting GUTI attach. "
                  "m_tmsi: %x, mcc: %x, mnc: %x, mme_group_id: %x, mme_code: %x\n",
                  ctxt.guti.m_tmsi, ctxt.guti.mcc, ctxt.guti.mnc, ctxt.guti.mme_group_id, ctxt.guti.mme_code);
    liblte_mme_pack_attach_request_msg(&attach_req,
                                       LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY,
                                       ctxt.tx_count,
                                       (LIBLTE_BYTE_MSG_STRUCT *) msg);

    // Add MAC
    integrity_generate(&k_nas_int[16],
                       ctxt.tx_count,
                       SECURITY_DIRECTION_UPLINK,
                       &msg->msg[5],
                       msg->N_bytes - 5,
                       &msg->msg[1]);
  } else {
    attach_req.eps_mobile_id.type_of_id = LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI;
    usim->get_imsi_vec(attach_req.eps_mobile_id.imsi, 15);
    nas_log->info("Requesting IMSI attach (IMSI=%s)\n", usim->get_imsi_str().c_str());
    liblte_mme_pack_attach_request_msg(&attach_req, (LIBLTE_BYTE_MSG_STRUCT *) msg);
  }

  if(pcap != NULL) {
    pcap->write_nas(msg->msg, msg->N_bytes);
  }

  nas_log->info("Sending attach request\n");
  rrc->write_sdu(cfg.lcid, msg);

  if (have_ctxt) {
    ctxt.tx_count++;
  }
}

void nas::gen_pdn_connectivity_request(LIBLTE_BYTE_MSG_STRUCT *msg) {
  LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT pdn_con_req;

  nas_log->info("Generating PDN Connectivity Request\n");

  // Set the PDN con req parameters
  pdn_con_req.eps_bearer_id = 0x00; // Unassigned bearer ID
  pdn_con_req.proc_transaction_id = 0x01; // First transaction ID
  pdn_con_req.pdn_type = LIBLTE_MME_PDN_TYPE_IPV4;
  pdn_con_req.request_type = LIBLTE_MME_REQUEST_TYPE_INITIAL_REQUEST;

  // Set the optional flags
  pdn_con_req.esm_info_transfer_flag_present = false; //FIXME: Check if this is needed
  if (cfg.apn == "") {
    pdn_con_req.apn_present = false;
  } else {
    pdn_con_req.apn_present = true;
    LIBLTE_MME_ACCESS_POINT_NAME_STRUCT apn;
    apn.apn = cfg.apn;
    pdn_con_req.apn = apn;
  }
  pdn_con_req.protocol_cnfg_opts_present = false;
  pdn_con_req.device_properties_present = false;

  // Pack the message
  liblte_mme_pack_pdn_connectivity_request_msg(&pdn_con_req, msg);
}

void nas::send_security_mode_reject(uint8_t cause) {
  byte_buffer_t *msg = pool_allocate;

  LIBLTE_MME_SECURITY_MODE_REJECT_MSG_STRUCT sec_mode_rej;
  sec_mode_rej.emm_cause = cause;
  liblte_mme_pack_security_mode_reject_msg(&sec_mode_rej, (LIBLTE_BYTE_MSG_STRUCT *) msg);
  if(pcap != NULL) {
    pcap->write_nas(msg->msg, msg->N_bytes);
  }
  nas_log->info("Sending security mode reject\n");
  rrc->write_sdu(cfg.lcid, msg);
}

void nas::send_identity_response() {}

void nas::send_service_request() {
  byte_buffer_t *msg = pool_allocate;

  // Pack the service request message directly
  msg->msg[0] = (LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
  msg->N_bytes++;
  msg->msg[1] = (ctxt.ksi & 0x07) << 5;
  msg->msg[1] |= ctxt.tx_count & 0x1F;
  msg->N_bytes++;

  uint8_t mac[4];
  integrity_generate(&k_nas_int[16],
                     ctxt.tx_count,
                     SECURITY_DIRECTION_UPLINK,
                     &msg->msg[0],
                     2,
                     &mac[0]);
  // Set the short MAC
  msg->msg[2] = mac[2];
  msg->N_bytes++;
  msg->msg[3] = mac[3];
  msg->N_bytes++;

  if(pcap != NULL) {
    pcap->write_nas(msg->msg, msg->N_bytes);
  }

  nas_log->info("Sending service request\n");
  rrc->write_sdu(cfg.lcid, msg);
  ctxt.tx_count++;
}

void nas::send_esm_information_response() {}


/*******************************************************************************
 * Security context persistence file
 ******************************************************************************/

bool nas::read_ctxt_file(nas_sec_ctxt *ctxt)
{
  std::ifstream file;
  if(!ctxt) {
    return false;
  }

  file.open(".ctxt", std::ios::in);
  if(file.is_open()) {
    if(!readvar(file, "m_tmsi=",        &ctxt->guti.m_tmsi))        {return false;}
    if(!readvar(file, "mcc=",           &ctxt->guti.mcc))           {return false;}
    if(!readvar(file, "mnc=",           &ctxt->guti.mnc))           {return false;}
    if(!readvar(file, "mme_group_id=",  &ctxt->guti.mme_group_id))  {return false;}
    if(!readvar(file, "mme_code=",      &ctxt->guti.mme_code))      {return false;}
    if(!readvar(file, "tx_count=",      &ctxt->tx_count))           {return false;}
    if(!readvar(file, "rx_count=",      &ctxt->rx_count))           {return false;}
    if(!readvar(file, "int_alg=",       &ctxt->integ_algo))         {return false;}
    if(!readvar(file, "enc_alg=",       &ctxt->cipher_algo))        {return false;}
    if(!readvar(file, "ksi=",           &ctxt->ksi))                {return false;}

    if(!readvar(file, "k_asme=",        ctxt->k_asme, 32))          {return false;}

    file.close();
    have_guti = true;
    nas_log->info("Read GUTI from file "
                  "m_tmsi: %x, mcc: %x, mnc: %x, mme_group_id: %x, mme_code: %x\n",
                  ctxt->guti.m_tmsi,
                  ctxt->guti.mcc,
                  ctxt->guti.mnc,
                  ctxt->guti.mme_group_id,
                  ctxt->guti.mme_code);
    have_ctxt = true;
    nas_log->info("Read security ctxt from file .ctxt. "
                  "ksi: %x, k_asme: %s, tx_count: %x, rx_count: %x, int_alg: %d, enc_alg: %d\n",
                  ctxt->ksi,
                  hex_to_string(ctxt->k_asme,32).c_str(),
                  ctxt->tx_count,
                  ctxt->rx_count,
                  ctxt->integ_algo,
                  ctxt->cipher_algo);
    return true;

  } else {
    return false;
  }
}

bool nas::write_ctxt_file(nas_sec_ctxt ctxt)
{
  if (!have_guti || !have_ctxt) {
    return false;
  }
  std::ofstream file;
  file.open(".ctxt", std::ios::out | std::ios::trunc);
  if (file.is_open()) {
    file << "m_tmsi="       << (int) ctxt.guti.m_tmsi         << std::endl;
    file << "mcc="          << (int) ctxt.guti.mcc            << std::endl;
    file << "mnc="          << (int) ctxt.guti.mnc            << std::endl;
    file << "mme_group_id=" << (int) ctxt.guti.mme_group_id   << std::endl;
    file << "mme_code="     << (int) ctxt.guti.mme_code       << std::endl;
    file << "tx_count="     << (int) ctxt.tx_count            << std::endl;
    file << "rx_count="     << (int) ctxt.rx_count            << std::endl;
    file << "int_alg="      << (int) ctxt.integ_algo          << std::endl;
    file << "enc_alg="      << (int) ctxt.cipher_algo         << std::endl;
    file << "ksi="          << (int) ctxt.ksi                 << std::endl;

    file << "k_asme="       << hex_to_string(ctxt.k_asme, 32) << std::endl;

    nas_log->info("Saved GUTI to file "
                  "m_tmsi: %x, mcc: %x, mnc: %x, mme_group_id: %x, mme_code: %x\n",
                  ctxt.guti.m_tmsi,
                  ctxt.guti.mcc,
                  ctxt.guti.mnc,
                  ctxt.guti.mme_group_id,
                  ctxt.guti.mme_code);
    nas_log->info("Saved security ctxt to file .ctxt. "
                  "ksi: %x, k_asme: %s, tx_count: %x, rx_count: %x, int_alg: %d, enc_alg: %d\n",
                  ctxt.ksi,
                  hex_to_string(ctxt.k_asme,32).c_str(),
                  ctxt.tx_count,
                  ctxt.rx_count,
                  ctxt.integ_algo,
                  ctxt.cipher_algo);
    file.close();
    return true;
  } else {
    return false;
  }
}

/*********************************************************************
 *   Conversion helpers
 ********************************************************************/
std::string nas::hex_to_string(uint8_t *hex, int size)
{
  std::stringstream ss;

  ss << std::hex << std::setfill('0');
  for(int i=0; i<size; i++) {
    ss << std::setw(2) << static_cast<unsigned>(hex[i]);
  }
  return ss.str();
}

bool nas::string_to_hex(std::string hex_str, uint8_t *hex, uint32_t len)
{
  static const char* const lut = "0123456789abcdef";
  uint32_t str_len = hex_str.length();
  if(str_len & 1) {
    return false; // uneven hex_str length
  }
  if(str_len > len*2) {
    return false; // not enough space in hex buffer
  }
  for(uint32_t i=0; i<str_len; i+=2)
  {
    char a = hex_str[i];
    const char* p = std::lower_bound(lut, lut + 16, a);
    if (*p != a) {
      return false; // invalid char
    }
    char b = hex_str[i+1];
    const char* q = std::lower_bound(lut, lut + 16, b);
    if (*q != b) {
      return false; // invalid char
    }
    hex[i/2] = ((p - lut) << 4) | (q - lut);
  }
  return true;
}

std::string nas::emm_info_str(LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT *info)
{
  std::stringstream ss;
  if(info->full_net_name_present) {
    ss << info->full_net_name.name;
  }
  if(info->short_net_name_present) {
    ss << " (" << info->short_net_name.name << ")";
  }
  if(info->utc_and_local_time_zone_present) {
    ss << " " << (int)info->utc_and_local_time_zone.day;
    ss << "/" << (int)info->utc_and_local_time_zone.month;
    ss << "/" << (int)info->utc_and_local_time_zone.year;
    ss << " " << (int)info->utc_and_local_time_zone.hour;
    ss << ":" << (int)info->utc_and_local_time_zone.minute;
    ss << ":" << (int)info->utc_and_local_time_zone.second;
    ss << " TZ:" << (int)info->utc_and_local_time_zone.tz;
  }
  return ss.str();
}


} // namespace srsue
