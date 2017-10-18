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


#include "srslte/asn1/liblte_rrc.h"
#include "upper/nas.h"
#include "srslte/common/bcd_helpers.h"

using namespace srslte;

namespace srsue {

nas::nas()
  : state(EMM_STATE_DEREGISTERED), plmn_selection(PLMN_SELECTED), is_guti_set(false), ip_addr(0), eps_bearer_id(0),
    count_ul(0), count_dl(0) {}

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

  if (usim->get_home_plmn_id(&home_plmn)) {
    nas_log->error("Getting Home PLMN Id from USIM. Defaulting to 001-01\n");
    home_plmn.mcc = 61441; // This is 001
    home_plmn.mnc = 65281; // This is 01
  }
  cfg     = cfg_;
}

void nas::stop() {}

emm_state_t nas::get_state() {
  return state;
}

/*******************************************************************************
UE interface
*******************************************************************************/

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
    selecting_plmn = current_plmn;
  } else {
    nas_log->info("Attach request ignored. State = %s\n", emm_state_text[state]);
  }
}

void nas::deattach_request() {
  state = EMM_STATE_DEREGISTERED_INITIATED;
  nas_log->info("Dettach request not supported\n");
}

/*******************************************************************************
RRC interface
*******************************************************************************/

void nas::plmn_found(LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id, uint16_t tracking_area_code) {

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
  uint8 pd;
  uint8 msg_type;

  nas_log->info_hex(pdu->msg, pdu->N_bytes, "DL %s PDU", rrc->get_rb_name(lcid).c_str());

  // Parse the message
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT *) pdu, &pd, &msg_type);
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
  return count_ul;
}

bool nas::get_s_tmsi(LIBLTE_RRC_S_TMSI_STRUCT *s_tmsi) {
  if (is_guti_set) {
    s_tmsi->mmec   = guti.mme_code;
    s_tmsi->m_tmsi = guti.m_tmsi;
    return true;
  } else {
    return false;
  }
}

/*******************************************************************************
Security
*******************************************************************************/

void nas::integrity_generate(uint8_t *key_128,
                             uint32_t count,
                             uint8_t rb_id,
                             uint8_t direction,
                             uint8_t *msg,
                             uint32_t msg_len,
                             uint8_t *mac) {
  switch (integ_algo) {
    case INTEGRITY_ALGORITHM_ID_EIA0:
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA1:
      security_128_eia1(key_128,
                        count,
                        rb_id,
                        direction,
                        msg,
                        msg_len,
                        mac);
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA2:
      security_128_eia2(key_128,
                        count,
                        rb_id,
                        direction,
                        msg,
                        msg_len,
                        mac);
      break;
    default:
      break;
  }
}

void nas::integrity_check() {

}

void nas::cipher_encrypt() {

}

void nas::cipher_decrypt() {

}


/*******************************************************************************
Parsers
*******************************************************************************/

void nas::parse_attach_accept(uint32_t lcid, byte_buffer_t *pdu) {
  LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT attach_accept;
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT act_def_eps_bearer_context_req;
  LIBLTE_MME_ATTACH_COMPLETE_MSG_STRUCT attach_complete;
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT act_def_eps_bearer_context_accept;

  nas_log->info("Received Attach Accept\n");
  count_dl++;

  liblte_mme_unpack_attach_accept_msg((LIBLTE_BYTE_MSG_STRUCT *) pdu, &attach_accept);

  if (attach_accept.eps_attach_result == LIBLTE_MME_EPS_ATTACH_RESULT_EPS_ONLY) {
    //FIXME: Handle t3412.unit
    //FIXME: Handle tai_list
    if (attach_accept.guti_present) {
      memcpy(&guti, &attach_accept.guti.guti, sizeof(LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT));
      is_guti_set = true;
      // TODO: log message to console
    }
    if (attach_accept.lai_present) {
    }
    if (attach_accept.ms_id_present) {}
    if (attach_accept.emm_cause_present) {}
    if (attach_accept.t3402_present) {}
    if (attach_accept.t3423_present) {}
    if (attach_accept.equivalent_plmns_present) {}
    if (attach_accept.emerg_num_list_present) {}
    if (attach_accept.eps_network_feature_support_present) {}
    if (attach_accept.additional_update_result_present) {}
    if (attach_accept.t3412_ext_present) {}

    liblte_mme_unpack_activate_default_eps_bearer_context_request_msg(&attach_accept.esm_msg,
                                                                      &act_def_eps_bearer_context_req);

    if (LIBLTE_MME_PDN_TYPE_IPV4 == act_def_eps_bearer_context_req.pdn_addr.pdn_type) {
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[0] << 24;
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[1] << 16;
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[2] << 8;
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[3];

      nas_log->info("IP allocated by network %u.%u.%u.%u\n",
                    act_def_eps_bearer_context_req.pdn_addr.addr[0],
                    act_def_eps_bearer_context_req.pdn_addr.addr[1],
                    act_def_eps_bearer_context_req.pdn_addr.addr[2],
                    act_def_eps_bearer_context_req.pdn_addr.addr[3]);

      nas_log->console("Network attach successful.   IP: %u.%u.%u.%u\n",
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

    // Send EPS bearer context accept and attach complete
    count_ul++;
    act_def_eps_bearer_context_accept.eps_bearer_id = eps_bearer_id;
    act_def_eps_bearer_context_accept.proc_transaction_id = transaction_id;
    act_def_eps_bearer_context_accept.protocol_cnfg_opts_present = false;
    liblte_mme_pack_activate_default_eps_bearer_context_accept_msg(&act_def_eps_bearer_context_accept,
                                                                   &attach_complete.esm_msg);
    liblte_mme_pack_attach_complete_msg(&attach_complete,
                                        LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED,
                                        count_ul,
                                        (LIBLTE_BYTE_MSG_STRUCT *) pdu);
    integrity_generate(&k_nas_int[16],
                       count_ul,
                       lcid - 1,
                       SECURITY_DIRECTION_UPLINK,
                       &pdu->msg[5],
                       pdu->N_bytes - 5,
                       &pdu->msg[1]);

    // Instruct RRC to enable capabilities
    rrc->enable_capabilities();

    nas_log->info("Sending Attach Complete\n");
    rrc->write_sdu(lcid, pdu);

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

  nas_log->info("Received Authentication Request\n");;
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
  usim->generate_authentication_response(auth_req.rand, auth_req.autn, mcc, mnc, &net_valid, res);

  if (net_valid) {
    nas_log->info("Network authentication successful\n");
    for (int i = 0; i < 8; i++) {
      auth_res.res[i] = res[i];
    }
    liblte_mme_pack_authentication_response_msg(&auth_res, (LIBLTE_BYTE_MSG_STRUCT *) pdu);

    nas_log->info("Sending Authentication Response\n");
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
  nas_log->error("TODO:parse_identity_request\n");
}

void nas::parse_security_mode_command(uint32_t lcid, byte_buffer_t *pdu) {
  bool success;
  LIBLTE_MME_SECURITY_MODE_COMMAND_MSG_STRUCT sec_mode_cmd;
  LIBLTE_MME_SECURITY_MODE_COMPLETE_MSG_STRUCT sec_mode_comp;
  LIBLTE_MME_SECURITY_MODE_REJECT_MSG_STRUCT sec_mode_rej;

  nas_log->info("Received Security Mode Command\n");
  liblte_mme_unpack_security_mode_command_msg((LIBLTE_BYTE_MSG_STRUCT *) pdu, &sec_mode_cmd);

  ksi = sec_mode_cmd.nas_ksi.nas_ksi;
  cipher_algo = (CIPHERING_ALGORITHM_ID_ENUM) sec_mode_cmd.selected_nas_sec_algs.type_of_eea;
  integ_algo = (INTEGRITY_ALGORITHM_ID_ENUM) sec_mode_cmd.selected_nas_sec_algs.type_of_eia;
  // FIXME: Handle nonce_ue, nonce_mme
  // FIXME: Currently only handling ciphering EEA0 (null) and integrity EIA1,EIA2
  // FIXME: Use selected_nas_sec_algs to choose correct algos

  nas_log->debug("Security details: ksi=%d, eea=%s, eia=%s\n",
                 ksi, ciphering_algorithm_id_text[cipher_algo], integrity_algorithm_id_text[integ_algo]);


  if (CIPHERING_ALGORITHM_ID_EEA0 != cipher_algo ||
      (INTEGRITY_ALGORITHM_ID_128_EIA2 != integ_algo &&
       INTEGRITY_ALGORITHM_ID_128_EIA1 != integ_algo) ||
      sec_mode_cmd.nas_ksi.tsc_flag != LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE) {
    sec_mode_rej.emm_cause = LIBLTE_MME_EMM_CAUSE_UE_SECURITY_CAPABILITIES_MISMATCH;
    nas_log->warning("Sending Security Mode Reject due to security capabilities mismatch\n");
    success = false;
  } else {
    // Generate NAS encryption key and integrity protection key
    usim->generate_nas_keys(k_nas_enc, k_nas_int, cipher_algo, integ_algo);
    nas_log->debug_hex(k_nas_enc, 32, "NAS encryption key - k_nas_enc");
    nas_log->debug_hex(k_nas_int, 32, "NAS integrity key - k_nas_int");

    // Check incoming MAC
    uint8_t *inMAC = &pdu->msg[1];
    uint8_t genMAC[4];
    integrity_generate(&k_nas_int[16],
                       count_dl,
                       lcid - 1,
                       SECURITY_DIRECTION_DOWNLINK,
                       &pdu->msg[5],
                       pdu->N_bytes - 5,
                       genMAC);

    nas_log->info_hex(inMAC, 4, "Incoming PDU MAC:");
    nas_log->info_hex(genMAC, 4, "Generated PDU MAC:");

    bool match = true;
    for (int i = 0; i < 4; i++) {
      if (inMAC[i] != genMAC[i]) {
        match = false;
      }
    }
    if (!match) {
      sec_mode_rej.emm_cause = LIBLTE_MME_EMM_CAUSE_SECURITY_MODE_REJECTED_UNSPECIFIED;
      nas_log->warning("Sending Security Mode Reject due to integrity check failure\n");
      success = false;
    } else {

      if (sec_mode_cmd.imeisv_req_present && LIBLTE_MME_IMEISV_REQUESTED == sec_mode_cmd.imeisv_req) {
        sec_mode_comp.imeisv_present = true;
        sec_mode_comp.imeisv.type_of_id = LIBLTE_MME_MOBILE_ID_TYPE_IMEISV;
        usim->get_imei_vec(sec_mode_comp.imeisv.imeisv, 15);
        sec_mode_comp.imeisv.imeisv[14] = 5;
        sec_mode_comp.imeisv.imeisv[15] = 3;
      } else {
        sec_mode_comp.imeisv_present = false;
      }

      // Reuse pdu for response
      pdu->reset();
      liblte_mme_pack_security_mode_complete_msg(&sec_mode_comp,
                                                 LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED,
                                                 count_ul,
                                                 (LIBLTE_BYTE_MSG_STRUCT *) pdu);
      integrity_generate(&k_nas_int[16],
                         count_ul,
                         lcid - 1,
                         SECURITY_DIRECTION_UPLINK,
                         &pdu->msg[5],
                         pdu->N_bytes - 5,
                         &pdu->msg[1]);
      nas_log->info("Sending Security Mode Complete nas_count_ul=%d, RB=%s\n",
                    count_ul,
                    rrc->get_rb_name(lcid).c_str());
      success = true;
    }
  }

  if (!success) {
    // Reuse pdu for response
    pdu->reset();
    liblte_mme_pack_security_mode_reject_msg(&sec_mode_rej, (LIBLTE_BYTE_MSG_STRUCT *) pdu);
  }

  rrc->write_sdu(lcid, pdu);
}

void nas::parse_service_reject(uint32_t lcid, byte_buffer_t *pdu) {
  nas_log->error("TODO:parse_service_reject\n");
}

void nas::parse_esm_information_request(uint32_t lcid, byte_buffer_t *pdu) {
  nas_log->error("TODO:parse_esm_information_request\n");
}

void nas::parse_emm_information(uint32_t lcid, byte_buffer_t *pdu) {
  nas_log->error("TODO:parse_emm_information\n");
}

/*******************************************************************************
Senders
*******************************************************************************/

void nas::send_attach_request() {
  LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT attach_req;
  byte_buffer_t *msg = pool_allocate;
  u_int32_t i;

  attach_req.eps_attach_type = LIBLTE_MME_EPS_ATTACH_TYPE_EPS_ATTACH;

  for (i = 0; i < 8; i++) {
    attach_req.ue_network_cap.eea[i] = false;
    attach_req.ue_network_cap.eia[i] = false;
  }
  attach_req.ue_network_cap.eea[0] = true; // EEA0 supported
  attach_req.ue_network_cap.eia[0] = true; // EIA0 supported
  attach_req.ue_network_cap.eia[1] = true; // EIA1 supported
  attach_req.ue_network_cap.eia[2] = true; // EIA2 supported

  attach_req.ue_network_cap.uea_present = false; // UMTS encryption algos
  attach_req.ue_network_cap.uia_present = false; // UMTS integrity algos

  attach_req.ms_network_cap_present = false; // A/Gb mode (2G) or Iu mode (3G)

  attach_req.eps_mobile_id.type_of_id = LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI;
  usim->get_imsi_vec(attach_req.eps_mobile_id.imsi, 15);

  // ESM message (PDN connectivity request) for first default bearer
  gen_pdn_connectivity_request(&attach_req.esm_msg);

  attach_req.old_p_tmsi_signature_present = false;
  attach_req.additional_guti_present = false;
  attach_req.last_visited_registered_tai_present = false;
  attach_req.drx_param_present = false;
  attach_req.ms_network_cap_present = false;
  attach_req.old_lai_present = false;
  attach_req.tmsi_status_present = false;
  attach_req.ms_cm2_present = false;
  attach_req.ms_cm3_present = false;
  attach_req.supported_codecs_present = false;
  attach_req.additional_update_type_present = false;
  attach_req.voice_domain_pref_and_ue_usage_setting_present = false;
  attach_req.device_properties_present = false;
  attach_req.old_guti_type_present = false;

  // Pack the message
  liblte_mme_pack_attach_request_msg(&attach_req, (LIBLTE_BYTE_MSG_STRUCT *) msg);

  nas_log->info("Sending attach request\n");
  rrc->write_sdu(cfg.lcid, msg);
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
  pdn_con_req.apn_present = false;
  pdn_con_req.protocol_cnfg_opts_present = false;
  pdn_con_req.device_properties_present = false;

  // Pack the message
  liblte_mme_pack_pdn_connectivity_request_msg(&pdn_con_req, msg);
}

void nas::send_identity_response() {}

void nas::send_service_request() {
  byte_buffer_t *msg = pool_allocate;
  count_ul++;

  // Pack the service request message directly
  msg->msg[0] = (LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
  msg->N_bytes++;
  msg->msg[1] = (ksi & 0x07) << 5;
  msg->msg[1] |= count_ul & 0x1F;
  msg->N_bytes++;

  uint8_t mac[4];
  integrity_generate(&k_nas_int[16],
                      count_ul,
                      cfg.lcid-1,
                      SECURITY_DIRECTION_UPLINK,
                      &msg->msg[0],
                      2,
                      &mac[0]);
  // Set the short MAC
  msg->msg[2] = mac[2];
  msg->N_bytes++;
  msg->msg[3] = mac[3];
  msg->N_bytes++;
  nas_log->info("Sending service request\n");
  rrc->write_sdu(cfg.lcid, msg);
}

void nas::send_esm_information_response() {}

} // namespace srsue
