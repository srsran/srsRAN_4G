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

#include "srsue/hdr/stack/upper/nas_5g.h"
#include "srsran/asn1/nas_5g_ies.h"
#include "srsran/asn1/nas_5g_msg.h"
#include "srsran/common/bcd_helpers.h"
#include "srsran/common/security.h"
#include "srsran/common/ssl.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"
#include "srsran/interfaces/ue_gw_interfaces.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/interfaces/ue_usim_interfaces.h"
#include "srsue/hdr/stack/upper/nas_5g_procedures.h"

#include <fstream>
#include <iomanip>
#include <unistd.h>

#define MAC_5G_OFFSET 2
#define SEQ_5G_OFFSET 6
#define NAS_5G_BEARER 1

using namespace srsran;
using namespace srsran::nas_5g;

namespace srsue {

/*********************************************************************
 *   NAS 5G (NR)
 ********************************************************************/

nas_5g::nas_5g(srslog::basic_logger& logger_, srsran::task_sched_handle task_sched_) :
  nas_base(logger_, MAC_5G_OFFSET, SEQ_5G_OFFSET, NAS_5G_BEARER),
  task_sched(task_sched_),
  t3502(task_sched_.get_unique_timer()),
  t3510(task_sched_.get_unique_timer()),
  t3511(task_sched_.get_unique_timer()),
  t3521(task_sched_.get_unique_timer()),
  reregistration_timer(task_sched_.get_unique_timer()),
  registration_proc(this),
  state(logger_),
  pdu_session_establishment_proc(this, logger_)
{
  // Configure timers
  t3502.set(t3502_duration_ms, [this](uint32_t tid) { timer_expired(tid); });
  t3510.set(t3510_duration_ms, [this](uint32_t tid) { timer_expired(tid); });
  t3511.set(t3511_duration_ms, [this](uint32_t tid) { timer_expired(tid); });
  t3521.set(t3521_duration_ms, [this](uint32_t tid) { timer_expired(tid); });
  reregistration_timer.set(reregistration_timer_duration_ms, [this](uint32_t tid) { timer_expired(tid); });
}

nas_5g::~nas_5g() {}

void nas_5g::stop()
{
  running = false;
}

int nas_5g::init(usim_interface_nas*      usim_,
                 rrc_nr_interface_nas_5g* rrc_nr_,
                 gw_interface_nas*        gw_,
                 const nas_5g_args_t&     cfg_)
{
  usim   = usim_;
  rrc_nr = rrc_nr_;
  gw     = gw_;
  cfg    = cfg_;

  // parse and sanity check EIA list
  if (parse_security_algorithm_list(cfg_.ia5g, ia5g_caps) != SRSRAN_SUCCESS) {
    logger.warning("Failed to parse integrity algorithm list: Defaulting to 5G-EI1-128, 5G-EI2-128, 5G-EI3-128");
    ia5g_caps[0] = false;
    ia5g_caps[1] = true;
    ia5g_caps[2] = true;
    ia5g_caps[3] = true;
  }

  // parse and sanity check EEA list
  if (parse_security_algorithm_list(cfg_.ea5g, ea5g_caps) != SRSRAN_SUCCESS) {
    logger.warning(
        "Failed to parse encryption algorithm list: Defaulting to 5G-EA0, 5G-EA1-128, 5G-EA2-128, 5G-EA3-128");
    ea5g_caps[0] = true;
    ea5g_caps[1] = true;
    ea5g_caps[2] = true;
    ea5g_caps[3] = true;
  }

  if (init_pdu_sessions(cfg.pdu_session_cfgs) != SRSRAN_SUCCESS) {
    logger.warning("Failure while configuring pdu sessions");
  }

  running = true;
  return SRSRAN_SUCCESS;
}

void nas_5g::run_tti()
{
  // Process PLMN selection ongoing procedures
  callbacks.run();

  // Transmit initiating messages if necessary
  switch (state.get_state()) {
    case mm5g_state_t::state_t::deregistered:
      // TODO Make sure cell selection is finished after transitioning from another state (if required)
      // Make sure the RRC is finished transitioning to RRC Idle
      if (reregistration_timer.is_running()) {
        logger.debug("Waiting for re-attach timer to expire to attach again.");
        return;
      }
      switch (state.get_deregistered_substate()) {
        case mm5g_state_t::deregistered_substate_t::plmn_search:
        case mm5g_state_t::deregistered_substate_t::normal_service:
        case mm5g_state_t::deregistered_substate_t::initial_registration_needed:
          registration_proc.launch();
          break;
        case mm5g_state_t::deregistered_substate_t::attempting_to_registration:
        case mm5g_state_t::deregistered_substate_t::no_supi:
        case mm5g_state_t::deregistered_substate_t::no_cell_available:
        case mm5g_state_t::deregistered_substate_t::e_call_inactive:
          logger.debug("Attempting to registration (not implemented) %s", state.get_full_state_text().c_str());
        default:
          break;
      }
    case mm5g_state_t::state_t::registered:
      break;
    case mm5g_state_t::state_t::deregistered_initiated:
      break;
    default:
      break;
  }
}

int nas_5g::write_pdu(srsran::unique_byte_buffer_t pdu)
{
  logger.info(pdu->msg, pdu->N_bytes, "DL PDU (length %d)", pdu->N_bytes);

  nas_5gs_msg nas_msg;

  if (nas_msg.unpack_outer_hdr(pdu) != SRSRAN_SUCCESS) {
    logger.error("Unable to unpack outer NAS header");
    return SRSRAN_ERROR;
  }

  switch (nas_msg.hdr.security_header_type) {
    case nas_5gs_hdr::security_header_type_opts::plain_5gs_nas_message:
      break;
    case nas_5gs_hdr::security_header_type_opts::integrity_protected:
      if (integrity_check(pdu.get()) == false) {
        logger.error("Not handling NAS message with integrity check error");
        return SRSRAN_ERROR;
      }
      break;
    case nas_5gs_hdr::security_header_type_opts::integrity_protected_and_ciphered:
      if (integrity_check(pdu.get()) == false) {
        logger.error("Not handling NAS message with integrity check error");
        return SRSRAN_ERROR;
      } else {
        cipher_decrypt(pdu.get());
      }
      break;
    case nas_5gs_hdr::security_header_type_opts::integrity_protected_with_new_5G_nas_context:
      break;
    case nas_5gs_hdr::security_header_type_opts::integrity_protected_and_ciphered_with_new_5G_nas_context:
      return SRSRAN_ERROR;
    default:
      logger.error("Not handling NAS message with unkown security header");
      break;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  logger.info(pdu->msg, pdu->N_bytes, "Decrypted DL PDU (length %d)", pdu->N_bytes);

  // Parse the message header
  if (nas_msg.unpack(pdu) != SRSRAN_SUCCESS) {
    logger.error("Unable to unpack complete NAS pdu");
    return SRSRAN_ERROR;
  }

  switch (nas_msg.hdr.message_type) {
    case msg_opts::options::registration_accept:
      handle_registration_accept(nas_msg.registration_accept());
      break;
    case msg_opts::options::registration_reject:
      handle_registration_reject(nas_msg.registration_reject());
      break;
    case msg_opts::options::authentication_reject:
      handle_authentication_reject(nas_msg.authentication_reject());
      break;
    case msg_opts::options::authentication_request:
      handle_authentication_request(nas_msg.authentication_request());
      break;
    case msg_opts::options::identity_request:
      handle_identity_request(nas_msg.identity_request());
      break;
    case msg_opts::options::security_mode_command:
      handle_security_mode_command(nas_msg.security_mode_command(), std::move(pdu));
      break;
    case msg_opts::options::service_accept:
      handle_service_accept(nas_msg.service_accept());
      break;
    case msg_opts::options::service_reject:
      handle_service_reject(nas_msg.service_reject());
      break;
    case msg_opts::options::deregistration_accept_ue_terminated:
      handle_deregistration_accept_ue_terminated(nas_msg.deregistration_accept_ue_terminated());
      break;
    case msg_opts::options::deregistration_request_ue_terminated:
      handle_deregistration_request_ue_terminated(nas_msg.deregistration_request_ue_terminated());
      break;
    case msg_opts::options::dl_nas_transport:
      handle_dl_nas_transport(nas_msg.dl_nas_transport());
      break;
    case msg_opts::options::deregistration_accept_ue_originating:
      handle_deregistration_accept_ue_originating(nas_msg.deregistration_accept_ue_originating());
      break;
    case msg_opts::options::configuration_update_command:
      handle_configuration_update_command(nas_msg.configuration_update_command());
      break;
    default:
      logger.error(
          "Not handling NAS message type: %s (0x%02x)", nas_msg.hdr.message_type.to_string(), nas_msg.hdr.message_type);
      break;
  }
  return SRSRAN_SUCCESS;
}

/*******************************************************************************
 * Senders
 ******************************************************************************/

int nas_5g::send_registration_request()
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  initial_registration_request_stored.hdr.extended_protocol_discriminator =
      nas_5gs_hdr::extended_protocol_discriminator_opts::extended_protocol_discriminator_5gmm;
  registration_request_t& reg_req = initial_registration_request_stored.set_registration_request();

  reg_req.registration_type_5gs.follow_on_request_bit =
      registration_type_5gs_t::follow_on_request_bit_type_::options::follow_on_request_pending;
  reg_req.registration_type_5gs.registration_type =
      registration_type_5gs_t::registration_type_type_::options::initial_registration;
  mobile_identity_5gs_t::suci_s& suci = reg_req.mobile_identity_5gs.set_suci();
  suci.supi_format                    = mobile_identity_5gs_t::suci_s::supi_format_type_::options::imsi;
  usim->get_home_mcc_bytes(suci.mcc.data(), suci.mcc.size());
  usim->get_home_mnc_bytes(suci.mnc.data(), suci.mnc.size());

  suci.scheme_output.resize(5);
  usim->get_home_msin_bcd(suci.scheme_output.data(), 5);
  logger.info("Requesting IMSI attach (IMSI=%s)", usim->get_imsi_str().c_str());

  reg_req.ue_security_capability_present = true;
  fill_security_caps(reg_req.ue_security_capability);

  if (initial_registration_request_stored.pack(pdu) != SRSASN_SUCCESS) {
    logger.error("Failed to pack registration request");
    return SRSRAN_ERROR;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu.get()->msg, pdu.get()->N_bytes);
  }

  // start T3510
  logger.debug("Starting T3410. Timeout in %d ms.", t3510.duration());
  t3510.run();

  logger.info("Sending Registration Request");
  if (rrc_nr->is_connected() == true) {
    rrc_nr->write_sdu(std::move(pdu));
  } else {
    logger.debug("Initiating RRC NR Connection");
    if (rrc_nr->connection_request(nr_establishment_cause_t::mo_Signalling, std::move(pdu)) != SRSRAN_SUCCESS) {
      logger.warning("Error starting RRC NR connection");
      return SRSRAN_ERROR;
    }
  }

  if (has_sec_ctxt) {
    set_k_gnb_count(ctxt_base.tx_count);
    ctxt_base.tx_count++;
  }

  state.set_registered_initiated();

  return SRSRAN_SUCCESS;
}

int nas_5g::send_registration_complete()
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  nas_5gs_msg              nas_msg;
  registration_complete_t& reg_comp = nas_msg.set_registration_complete();
  nas_msg.hdr.security_header_type  = nas_5gs_hdr::security_header_type_opts::integrity_protected_and_ciphered;
  nas_msg.hdr.sequence_number       = ctxt_base.tx_count;

  if (nas_msg.pack(pdu) != SRSASN_SUCCESS) {
    logger.error("Failed to pack registration complete.");
    return SRSRAN_ERROR;
  }

  cipher_encrypt(pdu.get());
  integrity_generate(&ctxt_base.k_nas_int[16],
                     ctxt_base.tx_count,
                     SECURITY_DIRECTION_UPLINK,
                     &pdu->msg[SEQ_5G_OFFSET],
                     pdu->N_bytes - SEQ_5G_OFFSET,
                     &pdu->msg[MAC_5G_OFFSET]);

  if (pcap != nullptr) {
    pcap->write_nas(pdu.get()->msg, pdu.get()->N_bytes);
  }
  logger.info("Sending Registration Complete");
  rrc_nr->write_sdu(std::move(pdu));
  ctxt_base.tx_count++;
  return SRSRAN_SUCCESS;
}

int nas_5g::send_authentication_response(const uint8_t res[16])
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  nas_5gs_msg                nas_msg;
  authentication_response_t& auth_resp                = nas_msg.set_authentication_response();
  auth_resp.authentication_response_parameter_present = true;
  auth_resp.authentication_response_parameter.res.resize(16);
  memcpy(auth_resp.authentication_response_parameter.res.data(), res, 16);

  if (nas_msg.pack(pdu) != SRSASN_SUCCESS) {
    logger.error("Failed to pack authentication response");
    return SRSRAN_ERROR;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu.get()->msg, pdu.get()->N_bytes);
  }

  logger.info("Sending Authentication Response");
  rrc_nr->write_sdu(std::move(pdu));
  ctxt_base.tx_count++;

  return SRSRAN_SUCCESS;
}

int nas_5g::send_authentication_response_eap(const std::vector<uint8_t>& eap_response)
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  nas_5gs_msg                nas_msg;
  authentication_response_t& auth_resp = nas_msg.set_authentication_response();
  auth_resp.eap_message_present        = true;
  auth_resp.eap_message.eap_message    = eap_response;

  if (nas_msg.pack(pdu) != SRSASN_SUCCESS) {
    logger.error("Failed to pack EAP authentication response");
    return SRSRAN_ERROR;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu.get()->msg, pdu.get()->N_bytes);
  }

  logger.info("Sending Authentication Response (EAP-Response/AKA'-Challenge, %zu B)", eap_response.size());
  rrc_nr->write_sdu(std::move(pdu));
  ctxt_base.tx_count++;

  return SRSRAN_SUCCESS;
}

// EAP-AKA' (RFC 5448 / TS 33.501). The network carries the EAP-Request/AKA'-Challenge
// inside the Authentication Request EAP-Message IE. This parses the challenge, runs
// MILENAGE, derives CK'/IK' -> MK -> K_aut/EMSK -> KAUSF -> KSEAF -> KAMF and replies
// with an EAP-Response/AKA'-Challenge carrying AT_RES and AT_MAC.
int nas_5g::handle_eap_aka_prime_challenge(authentication_request_t& authentication_request)
{
  // EAP attribute types (RFC 4187 / RFC 5448)
  enum { AT_RAND = 1, AT_AUTN = 2, AT_RES = 3, AT_MAC = 11, AT_KDF_INPUT = 23 };
  // EAP-AKA' subtypes (RFC 4187 section 11)
  enum { AKA_CHALLENGE = 1, AKA_AUTHENTICATION_REJECT = 10, AKA_CLIENT_ERROR = 14 };
  const uint8_t EAP_CODE_SUCCESS = 3;
  const uint8_t EAP_TYPE_AKA_PRIME = 50;

  std::vector<uint8_t>& eap = authentication_request.eap_message.eap_message;
  if (eap.size() < 5) {
    logger.error("[EAP-AKA'] EAP packet too short (%zu B)", eap.size());
    return SRSRAN_ERROR;
  }
  if (eap[0] == EAP_CODE_SUCCESS) {
    logger.info("[EAP-AKA'] EAP-Success received, authentication complete");
    return SRSRAN_SUCCESS;
  }
  // EAP header: code(1) id(1) length(2) type(1) subtype(1) reserved(2) attributes...
  if (eap.size() < 8 || eap[4] != EAP_TYPE_AKA_PRIME) {
    logger.error("[EAP-AKA'] Not an EAP-AKA' request (type=%d)", eap.size() >= 5 ? eap[4] : -1);
    return SRSRAN_ERROR;
  }
  uint8_t eap_id = eap[1];
  logger.info("[EAP-AKA'] Handling EAP-Request/AKA'-Challenge (id=%d, %zu B)", eap_id, eap.size());

  // Parse the required attributes
  uint8_t              rand[16] = {}, autn[16] = {}, recv_mac[16] = {};
  bool                 rand_present = false, autn_present = false;
  std::vector<uint8_t> network_name;
  int                  mac_offset = -1;
  size_t               pos        = 8;
  while (pos + 2 <= eap.size()) {
    uint8_t at     = eap[pos];
    size_t  a_len  = static_cast<size_t>(eap[pos + 1]) * 4; // attribute length in 4-byte units
    if (a_len == 0 || pos + a_len > eap.size()) {
      break;
    }
    switch (at) {
      case AT_RAND:
        if (a_len >= 20) {
          memcpy(rand, &eap[pos + 4], 16); // 2 reserved bytes then 16-byte RAND
          rand_present = true;
        }
        break;
      case AT_AUTN:
        if (a_len >= 20) {
          memcpy(autn, &eap[pos + 4], 16);
          autn_present = true;
        }
        break;
      case AT_KDF_INPUT: {
        uint16_t name_len = (eap[pos + 2] << 8) | eap[pos + 3];
        if (pos + 4 + name_len <= eap.size()) {
          network_name.assign(&eap[pos + 4], &eap[pos + 4] + name_len);
        }
        break;
      }
      case AT_MAC:
        if (a_len >= 20) {
          memcpy(recv_mac, &eap[pos + 4], 16);
          mac_offset = pos + 4;
        }
        break;
      default:
        break;
    }
    pos += a_len;
  }

  if (!rand_present || !autn_present || network_name.empty()) {
    logger.error("[EAP-AKA'] Challenge is missing AT_RAND/AT_AUTN/AT_KDF_INPUT");
    return send_eap_aka_prime_reject(eap_id, AKA_CLIENT_ERROR);
  }

  // Run the AKA algorithm inside the USIM: it verifies the AUTN MAC and, on success,
  // returns CK, IK, RES and SQN^AK, keeping the long-term key K inside the USIM.
  uint8_t       ck[16], ik[16], res[16], sqn_xor_ak_buf[6];
  int           res_len     = 0;
  auth_result_t auth_result = usim->generate_authentication_response_5g_eap_aka_prime(
      rand, autn, ck, ik, res, &res_len, sqn_xor_ak_buf);
  if (auth_result != AUTH_OK) {
    // AUTN failure: reject the network per RFC 4187 (a soft USIM never reports a
    // synchronization failure, so AT_AUTS resynchronization is not applicable here).
    logger.warning("[EAP-AKA'] AUTN verification failed, sending EAP-Response/AKA'-Authentication-Reject");
    return send_eap_aka_prime_reject(eap_id, AKA_AUTHENTICATION_REJECT);
  }

  // CK'||IK' = KDF(CK||IK, FC=0x20, network_name, SQN xor AK)  (TS 33.402 Annex A.2)
  std::array<uint8_t, 32> ck_ik;
  memcpy(ck_ik.data(), ck, 16);
  memcpy(ck_ik.data() + 16, ik, 16);
  std::vector<uint8_t> sqn_xor_ak(sqn_xor_ak_buf, sqn_xor_ak_buf + 6);
  uint8_t              ck_ik_prime[32];
  kdf_common(0x20, ck_ik, network_name, sqn_xor_ak, ck_ik_prime);
  const uint8_t* ck_prime = ck_ik_prime;
  const uint8_t* ik_prime = ck_ik_prime + 16;

  // MK = PRF'(IK'||CK', "EAP-AKA'"||Identity)  (RFC 5448 section 3.3)
  // PRF': T1 = HMAC-SHA256(K, S|1); Tn = HMAC-SHA256(K, T(n-1)|S|n)
  uint8_t prf_key[32];
  memcpy(prf_key, ik_prime, 16);
  memcpy(prf_key + 16, ck_prime, 16);
  std::string          identity = usim->get_imsi_str();
  std::vector<uint8_t> s        = {'E', 'A', 'P', '-', 'A', 'K', 'A', '\''};
  s.insert(s.end(), identity.begin(), identity.end());

  const int MK_ROUNDS = 7; // 7*32 = 224 B >= K_encr+K_aut+K_re+MSK+EMSK (208 B)
  uint8_t   mk[MK_ROUNDS * 32];
  uint8_t   t_prev[32];
  int       t_prev_len = 0;
  for (int i = 0; i < MK_ROUNDS; i++) {
    std::vector<uint8_t> in;
    if (t_prev_len) {
      in.insert(in.end(), t_prev, t_prev + 32);
    }
    in.insert(in.end(), s.begin(), s.end());
    in.push_back(static_cast<uint8_t>(i + 1));
    sha256(prf_key, 32, in.data(), in.size(), mk + i * 32, 0);
    memcpy(t_prev, mk + i * 32, 32);
    t_prev_len = 32;
  }
  const uint8_t* k_aut = mk + 16;  // MK[128..383]
  const uint8_t* emsk  = mk + 144; // MK[1152..1663]

  // Verify AT_MAC (HMAC-SHA256 over the EAP packet with the MAC field zeroed, truncated to 16 B).
  // A missing or invalid AT_MAC means the challenge integrity cannot be trusted, so we abort
  // with an EAP-Response/AKA'-Client-Error instead of installing a security context.
  if (mac_offset < 0) {
    logger.error("[EAP-AKA'] Challenge is missing AT_MAC, sending EAP-Response/AKA'-Client-Error");
    return send_eap_aka_prime_reject(eap_id, AKA_CLIENT_ERROR);
  }
  {
    std::vector<uint8_t> eap_zeroed(eap.begin(), eap.end());
    memset(&eap_zeroed[mac_offset], 0, 16);
    uint8_t computed_mac[32];
    sha256(k_aut, 32, eap_zeroed.data(), eap_zeroed.size(), computed_mac, 0);
    if (memcmp(computed_mac, recv_mac, 16) != 0) {
      logger.error("[EAP-AKA'] AT_MAC verification failed, sending EAP-Response/AKA'-Client-Error");
      return send_eap_aka_prime_reject(eap_id, AKA_CLIENT_ERROR);
    }
    logger.info("[EAP-AKA'] AT_MAC verification OK");
  }

  // The challenge is fully verified; let the following Security Mode Command install the context.
  initial_sec_command = true;

  // KAUSF = EMSK[0..255] -> KSEAF -> KAMF  (TS 33.501)
  uint8_t     k_ausf[32];
  memcpy(k_ausf, emsk, 32);
  std::string snn = std::string(network_name.begin(), network_name.end());
  uint8_t     k_seaf[32];
  security_generate_k_seaf(k_ausf, snn.c_str(), k_seaf);

  uint8_t  abba[8]  = {0x00, 0x00};
  uint32_t abba_len = 2;
  if (authentication_request.abba.abba_contents.size() > 0 &&
      authentication_request.abba.abba_contents.size() <= sizeof(abba)) {
    abba_len = authentication_request.abba.abba_contents.size();
    memcpy(abba, authentication_request.abba.abba_contents.data(), abba_len);
  }
  // The SUPI passed to the KAMF KDF is the plain IMSI digits, matching the 5G-AKA path.
  security_generate_k_amf(k_seaf, identity.c_str(), abba, abba_len, ctxt_5g.k_amf);
  logger.debug(ctxt_5g.k_amf, 32, "[EAP-AKA'] Derived K_AMF:");

  // Build EAP-Response/AKA'-Challenge: header + AT_RES + AT_MAC
  std::vector<uint8_t> resp = {2 /*code=Response*/, eap_id, 0, 0, EAP_TYPE_AKA_PRIME, AKA_CHALLENGE, 0, 0};
  // AT_RES: type(1) len(1=3 units) res-bit-length(2) RES(8)
  resp.push_back(AT_RES);
  resp.push_back(3);
  resp.push_back(0);
  resp.push_back(64); // 64-bit RES
  resp.insert(resp.end(), res, res + 8);
  // AT_MAC: type(1) len(1=5 units) reserved(2) MAC(16)
  size_t mac_field = resp.size() + 4;
  resp.push_back(AT_MAC);
  resp.push_back(5);
  resp.push_back(0);
  resp.push_back(0);
  resp.insert(resp.end(), 16, 0);
  // Patch EAP length then compute AT_MAC over the whole packet (MAC field zeroed)
  uint16_t eap_len = resp.size();
  resp[2]          = eap_len >> 8;
  resp[3]          = eap_len & 0xff;
  uint8_t resp_mac[32];
  sha256(k_aut, 32, resp.data(), resp.size(), resp_mac, 0);
  memcpy(&resp[mac_field], resp_mac, 16);

  return send_authentication_response_eap(resp);
}

// Build and send a minimal EAP-Response for a rejected EAP-AKA' challenge. AKA'-Authentication-Reject
// (subtype 10) carries no attributes; AKA'-Client-Error (subtype 14) carries AT_CLIENT_ERROR_CODE = 0
// ("unable to process packet"). Both return SRSRAN_ERROR so the caller aborts the procedure.
int nas_5g::send_eap_aka_prime_reject(uint8_t eap_id, uint8_t subtype)
{
  const uint8_t EAP_TYPE_AKA_PRIME   = 50;
  const uint8_t AKA_CLIENT_ERROR     = 14;
  const uint8_t AT_CLIENT_ERROR_CODE = 22;

  std::vector<uint8_t> resp = {2 /*code=Response*/, eap_id, 0, 0, EAP_TYPE_AKA_PRIME, subtype, 0, 0};
  if (subtype == AKA_CLIENT_ERROR) {
    // AT_CLIENT_ERROR_CODE: type(1) len(1=1 unit) error-code(2)
    resp.push_back(AT_CLIENT_ERROR_CODE);
    resp.push_back(1);
    resp.push_back(0);
    resp.push_back(0);
  }
  uint16_t eap_len = resp.size();
  resp[2]          = eap_len >> 8;
  resp[3]          = eap_len & 0xff;

  send_authentication_response_eap(resp);
  return SRSRAN_ERROR;
}

int nas_5g::send_security_mode_reject(const cause_5gmm_t::cause_5gmm_type_::options cause)
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  nas_5gs_msg             nas_msg;
  security_mode_reject_t& security_mode_reject = nas_msg.set_security_mode_reject();
  security_mode_reject.cause_5gmm.cause_5gmm   = cause;

  if (nas_msg.pack(pdu) != SRSASN_SUCCESS) {
    logger.error("Failed to pack authentication response");
    return SRSRAN_ERROR;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu.get()->msg, pdu.get()->N_bytes);
  }

  logger.info("Sending Authentication Response");
  rrc_nr->write_sdu(std::move(pdu));

  return SRSRAN_SUCCESS;
}

int nas_5g::send_security_mode_complete(const srsran::nas_5g::security_mode_command_t& security_mode_command)
{
  uint8_t current_sec_hdr  = LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED_WITH_NEW_EPS_SECURITY_CONTEXT;
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  nas_5gs_msg               nas_msg;
  security_mode_complete_t& security_mode_complete = nas_msg.set_security_mode_complete();

  if (security_mode_command.imeisv_request_present) {
    security_mode_complete.imeisv_present   = true;
    mobile_identity_5gs_t::imeisv_s& imeisv = security_mode_complete.imeisv.set_imeisv();
    usim->get_imei_vec(imeisv.imeisv.data(), 15);
    imeisv.imeisv[14] = ue_svn_oct1;
    imeisv.imeisv[15] = ue_svn_oct2;
  }
  // TODO: Save TMSI
  registration_request_t& modified_registration_request = initial_registration_request_stored.registration_request();
  modified_registration_request.capability_5gmm_present = true;
  modified_registration_request.update_type_5gs_present = true;

  if (cfg.enable_slicing) {
    s_nssai_t s_nssai{};
    modified_registration_request.requested_nssai_present      = true;
    set_nssai(s_nssai);
    modified_registration_request.requested_nssai.s_nssai_list = {s_nssai};
  }
  modified_registration_request.capability_5gmm.lpp       = 0;
  modified_registration_request.capability_5gmm.ho_attach = 0;
  modified_registration_request.capability_5gmm.s1_mode   = 0;

  modified_registration_request.update_type_5gs.ng_ran_rcu.value =
      update_type_5gs_t::NG_RAN_RCU_type::options::ue_radio_capability_update_not_needed;
  modified_registration_request.update_type_5gs.sms_requested.value =
      update_type_5gs_t::SMS_requested_type::options::sms_over_nas_not_supported;

  security_mode_complete.nas_message_container_present = true;
  initial_registration_request_stored.pack(security_mode_complete.nas_message_container.nas_message_container);

  nas_msg.hdr.security_header_type =
      nas_5gs_hdr::security_header_type_opts::integrity_protected_and_ciphered_with_new_5G_nas_context;
  nas_msg.hdr.sequence_number = ctxt_base.tx_count;

  if (nas_msg.pack(pdu) != SRSASN_SUCCESS) {
    logger.error("Failed to pack security mode complete");
    return SRSRAN_ERROR;
  }

  cipher_encrypt(pdu.get());
  integrity_generate(&ctxt_base.k_nas_int[16],
                     ctxt_base.tx_count,
                     SECURITY_DIRECTION_UPLINK,
                     &pdu->msg[SEQ_5G_OFFSET],
                     pdu->N_bytes - SEQ_5G_OFFSET,
                     &pdu->msg[MAC_5G_OFFSET]);

  if (pcap != nullptr) {
    pcap->write_nas(pdu.get()->msg, pdu.get()->N_bytes);
  }

  has_sec_ctxt = true;
  logger.info("Sending Security Mode Complete");
  rrc_nr->write_sdu(std::move(pdu));
  ctxt_base.tx_count++;

  return SRSRAN_SUCCESS;
}

int nas_5g::send_authentication_failure(const cause_5gmm_t::cause_5gmm_type_::options cause, const uint8_t res[16])
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  nas_5gs_msg               nas_msg;
  authentication_failure_t& auth_fail = nas_msg.set_authentication_failure();
  auth_fail.cause_5gmm.cause_5gmm     = cause;

  if (cause == cause_5gmm_t::cause_5gmm_type::synch_failure) {
    auth_fail.authentication_failure_parameter_present = true;
    auth_fail.authentication_failure_parameter.auth_failure.resize(14);
    memcpy(auth_fail.authentication_failure_parameter.auth_failure.data(), res, 14);
  }

  if (nas_msg.pack(pdu) != SRSASN_SUCCESS) {
    logger.error("Failed to pack authentication failure.");
    return SRSRAN_ERROR;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu.get()->msg, pdu.get()->N_bytes);
  }
  logger.info("Sending Authentication Failure");
  rrc_nr->write_sdu(std::move(pdu));

  return SRSRAN_SUCCESS;
}

uint32_t nas_5g::allocate_next_proc_trans_id()
{
  uint32_t i = 0;
  for (auto pdu_trans_id : pdu_trans_ids) {
    i++;
    if (pdu_trans_id == false) {
      pdu_trans_id = true;
      break;
    }
  }
  // TODO if Trans ID exhausted
  return i;
}

void nas_5g::release_proc_trans_id(uint32_t proc_id)
{
  if (proc_id < MAX_TRANS_ID) {
    pdu_trans_ids[proc_id] = false;
  }
  return;
}

void nas_5g::set_nssai(srsran::nas_5g::s_nssai_t& s_nssai)
{
  if (cfg.nssai_sd == 0) {
    s_nssai.type = s_nssai_t::SST_type_::options::sst;
  } else {
    s_nssai.type = s_nssai_t::SST_type_::options::sst_and_sd;
  }
  s_nssai.sst = cfg.nssai_sst;
  s_nssai.sd  = cfg.nssai_sd;
}

int nas_5g::send_pdu_session_establishment_request(uint32_t                 transaction_identity,
                                                   uint16_t                 pdu_session_id,
                                                   const pdu_session_cfg_t& pdu_session_cfg)
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  nas_5gs_msg nas_msg;
  nas_msg.hdr.pdu_session_identity           = pdu_session_id;
  nas_msg.hdr.procedure_transaction_identity = transaction_identity;
  nas_msg.hdr.sequence_number                = ctxt_base.tx_count;

  pdu_session_establishment_request_t& pdu_ses_est_req = nas_msg.set_pdu_session_establishment_request();
  pdu_ses_est_req.integrity_protection_maximum_data_rate.max_data_rate_upip_downlink.value =
      integrity_protection_maximum_data_rate_t::max_data_rate_UPIP_downlink_type_::options::full_data_rate;
  pdu_ses_est_req.integrity_protection_maximum_data_rate.max_data_rate_upip_uplink.value =
      integrity_protection_maximum_data_rate_t::max_data_rate_UPIP_uplink_type_::options::full_data_rate;

  pdu_ses_est_req.pdu_session_type_present = true;
  pdu_ses_est_req.pdu_session_type.pdu_session_type_value =
      static_cast<srsran::nas_5g::pdu_session_type_t::PDU_session_type_value_type_::options>(pdu_session_cfg.apn_type);

  pdu_ses_est_req.ssc_mode_present        = true;
  pdu_ses_est_req.ssc_mode.ssc_mode_value = ssc_mode_t::SSC_mode_value_type_::options::ssc_mode_1;

  // TODO set the capability and extended protocol configuration
  pdu_ses_est_req.capability_5gsm_present                         = false;
  pdu_ses_est_req.extended_protocol_configuration_options_present = false;

  // Build up the Envelope for the PDU session request
  nas_5gs_msg env_nas_msg;
  env_nas_msg.hdr.security_header_type = nas_5gs_hdr::security_header_type_opts::integrity_protected_and_ciphered;

  // TODO move that seq number setting to the security part
  env_nas_msg.hdr.sequence_number = ctxt_base.tx_count;

  ul_nas_transport_t& ul_nas_msg = env_nas_msg.set_ul_nas_transport();
  ul_nas_msg.payload_container_type.payload_container_type.value =
      payload_container_type_t::Payload_container_type_type_::options::n1_sm_information;

  // Pack the pdu session est request into the envelope
  if (nas_msg.pack(ul_nas_msg.payload_container.payload_container_contents) != SRSASN_SUCCESS) {
    logger.error("Failed to pack PDU Session Establishment Request.");
    return SRSRAN_ERROR;
  }

  ul_nas_msg.pdu_session_id_present                      = true;
  ul_nas_msg.pdu_session_id.pdu_session_identity_2_value = pdu_session_id;

  ul_nas_msg.request_type_present            = true;
  ul_nas_msg.request_type.request_type_value = request_type_t::Request_type_value_type_::options::initial_request;

  if (cfg.enable_slicing) {
    ul_nas_msg.s_nssai_present = true;
    set_nssai(ul_nas_msg.s_nssai);
  }
  ul_nas_msg.dnn_present = true;
  ul_nas_msg.dnn.dnn_value.resize(pdu_session_cfg.apn_name.size() + 1);
  ul_nas_msg.dnn.dnn_value.data()[0] = static_cast<uint8_t>(pdu_session_cfg.apn_name.size());

  memcpy(ul_nas_msg.dnn.dnn_value.data() + 1, pdu_session_cfg.apn_name.data(), pdu_session_cfg.apn_name.size());

  if (env_nas_msg.pack(pdu) != SRSASN_SUCCESS) {
    logger.error("Failed to pack UL NAS transport.");
    return SRSRAN_ERROR;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu.get()->msg, pdu.get()->N_bytes);
  }

  cipher_encrypt(pdu.get());
  integrity_generate(&ctxt_base.k_nas_int[16],
                     ctxt_base.tx_count,
                     SECURITY_DIRECTION_UPLINK,
                     &pdu->msg[SEQ_5G_OFFSET],
                     pdu->N_bytes - SEQ_5G_OFFSET,
                     &pdu->msg[MAC_5G_OFFSET]);

  logger.info("Sending PDU Session Establishment Request in UL NAS transport.");
  rrc_nr->write_sdu(std::move(pdu));
  ctxt_base.tx_count++;

  return SRSRAN_SUCCESS;
}

int nas_5g::send_deregistration_request_ue_originating(bool switch_off)
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  nas_5gs_msg                              nas_msg;
  deregistration_request_ue_originating_t& deregistration_request = nas_msg.set_deregistration_request_ue_originating();
  nas_msg.hdr.security_header_type = nas_5gs_hdr::security_header_type_opts::integrity_protected_and_ciphered;
  nas_msg.hdr.sequence_number      = ctxt_base.tx_count;

  // Note 5.5.2.2.2 : AMF does not send a Deregistration Accept NAS message if De-registration type IE indicates "switch
  // off"
  if (switch_off) {
    deregistration_request.de_registration_type.switch_off.value =
        de_registration_type_t::switch_off_type_::options::switch_off;
    state.set_deregistered(mm5g_state_t::deregistered_substate_t::null);
  } else {
    deregistration_request.de_registration_type.switch_off.value =
        de_registration_type_t::switch_off_type_::options::normal_de_registration;
    // In this case we need to wait for the response by the core
    state.set_deregistered_initiated();
  }

  mobile_identity_5gs_t::suci_s& suci = deregistration_request.mobile_identity_5gs.set_suci();
  suci.supi_format                    = mobile_identity_5gs_t::suci_s::supi_format_type_::options::imsi;
  usim->get_home_mcc_bytes(suci.mcc.data(), suci.mcc.size());
  usim->get_home_mnc_bytes(suci.mnc.data(), suci.mnc.size());
  suci.scheme_output.resize(5);

  deregistration_request.ng_ksi.nas_key_set_identifier.value =
      key_set_identifier_t::nas_key_set_identifier_type_::options::no_key_is_available_or_reserved;

  if (nas_msg.pack(pdu) != SRSASN_SUCCESS) {
    logger.error("Failed to pack Deregistration Request (UE Originating).");
    return SRSRAN_ERROR;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu.get()->msg, pdu.get()->N_bytes);
  }

  logger.info("Sending Deregistration Request (UE Originating)");
  cipher_encrypt(pdu.get());
  integrity_generate(&ctxt_base.k_nas_int[16],
                     ctxt_base.tx_count,
                     SECURITY_DIRECTION_UPLINK,
                     &pdu->msg[SEQ_5G_OFFSET],
                     pdu->N_bytes - SEQ_5G_OFFSET,
                     &pdu->msg[MAC_5G_OFFSET]);

  rrc_nr->write_sdu(std::move(pdu));
  ctxt_base.tx_count++;
  reset_pdu_sessions();
  // TODO: Consider reworking ctxt / 5G ctxt release

  return SRSASN_SUCCESS;
}

int nas_5g::send_identity_response(srsran::nas_5g::identity_type_5gs_t::identity_types_::options identity_type)
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  nas_5gs_msg          nas_msg;
  identity_response_t& identity_response = nas_msg.set_identity_response();
  nas_msg.hdr.security_header_type       = nas_5gs_hdr::security_header_type_opts::integrity_protected_and_ciphered;
  nas_msg.hdr.sequence_number            = ctxt_base.tx_count;

  switch (identity_type) {
    case (identity_type_5gs_t::identity_types_::suci): {
      srsran::nas_5g::mobile_identity_5gs_t::suci_s& suci = identity_response.mobile_identity.set_suci();
      suci.supi_format = mobile_identity_5gs_t::suci_s::supi_format_type_::options::imsi;
      usim->get_home_mcc_bytes(suci.mcc.data(), suci.mcc.size());
      usim->get_home_mnc_bytes(suci.mnc.data(), suci.mnc.size());
      suci.scheme_output.resize(5);
      usim->get_home_msin_bcd(suci.scheme_output.data(), 5);
    } break;
    case (identity_type_5gs_t::identity_types_::guti_5g): {
      srsran::nas_5g::mobile_identity_5gs_t::guti_5g_s& guti = identity_response.mobile_identity.set_guti_5g();
      guti                                                   = guti_5g;
    } break;
    case (identity_type_5gs_t::identity_types_::imei): {
      srsran::nas_5g::mobile_identity_5gs_t::imei_s& imei = identity_response.mobile_identity.set_imei();
      usim->get_imei_vec(imei.imei.data(), 15);
    } break;
    case (identity_type_5gs_t::identity_types_::imeisv): {
      srsran::nas_5g::mobile_identity_5gs_t::imeisv_s& imeisv = identity_response.mobile_identity.set_imeisv();
      usim->get_imei_vec(imeisv.imeisv.data(), 15);
      imeisv.imeisv[14] = ue_svn_oct1;
      imeisv.imeisv[15] = ue_svn_oct2;
    } break;
    default:
      logger.warning("Unhandled identity type for identity response");
      return SRSRAN_ERROR;
  }

  if (nas_msg.pack(pdu) != SRSASN_SUCCESS) {
    logger.error("Failed to pack Identity Response.");
    return SRSRAN_ERROR;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu.get()->msg, pdu.get()->N_bytes);
  }

  cipher_encrypt(pdu.get());
  integrity_generate(&ctxt_base.k_nas_int[16],
                     ctxt_base.tx_count,
                     SECURITY_DIRECTION_UPLINK,
                     &pdu->msg[SEQ_5G_OFFSET],
                     pdu->N_bytes - SEQ_5G_OFFSET,
                     &pdu->msg[MAC_5G_OFFSET]);

  logger.info("Sending Identity Response");
  rrc_nr->write_sdu(std::move(pdu));
  ctxt_base.tx_count++;

  return SRSRAN_SUCCESS;
}

int nas_5g::send_configuration_update_complete()
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  nas_5gs_msg                      nas_msg;
  configuration_update_complete_t& config_update_complete = nas_msg.set_configuration_update_complete();
  nas_msg.hdr.security_header_type = nas_5gs_hdr::security_header_type_opts::integrity_protected_and_ciphered;
  nas_msg.hdr.sequence_number      = ctxt_base.tx_count;

  if (nas_msg.pack(pdu) != SRSASN_SUCCESS) {
    logger.error("Failed to pack Identity Response.");
    return SRSRAN_ERROR;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu.get()->msg, pdu.get()->N_bytes);
  }

  cipher_encrypt(pdu.get());
  integrity_generate(&ctxt_base.k_nas_int[16],
                     ctxt_base.tx_count,
                     SECURITY_DIRECTION_UPLINK,
                     &pdu->msg[SEQ_5G_OFFSET],
                     pdu->N_bytes - SEQ_5G_OFFSET,
                     &pdu->msg[MAC_5G_OFFSET]);

  logger.info("Sending Configuration Update Complete");
  rrc_nr->write_sdu(std::move(pdu));
  ctxt_base.tx_count++;
  return SRSRAN_SUCCESS;
}

// Message handler
int nas_5g::handle_registration_accept(registration_accept_t& registration_accept)
{
  ctxt_base.rx_count++;
  if (state.get_state() != mm5g_state_t::state_t::registered_initiated) {
    logger.warning("Not compatibale with current state %s", state.get_full_state_text());
    return SRSRAN_ERROR;
  }

  bool send_reg_complete = false;
  logger.info("Handling Registration Accept");
  if (registration_accept.guti_5g_present) {
    guti_5g           = registration_accept.guti_5g.guti_5g();
    send_reg_complete = true;
  }

  // TODO: reset counters and everything what is needed by the specification
  t3521.set(registration_accept.t3512_value.timer_value);
  registration_proc.run();
  state.set_registered(mm5g_state_t::registered_substate_t::normal_service);

  if (send_reg_complete == true) {
    send_registration_complete();
  }
  // TODO: use the state machine to trigger that transition
  trigger_pdu_session_est();
  return SRSRAN_SUCCESS;
}

int nas_5g::handle_registration_reject(registration_reject_t& registration_reject)
{
  logger.info("Handling Registration Reject");
  has_sec_ctxt = false;
  ctxt_base.rx_count++;
  state.set_deregistered(mm5g_state_t::deregistered_substate_t::plmn_search);

  switch (registration_reject.cause_5gmm.cause_5gmm.value) {
    case (cause_5gmm_t::cause_5gmm_type_::options::illegal_ue):
      logger.error("Registration Reject: Illegal UE");
      break;
    case (cause_5gmm_t::cause_5gmm_type_::options::plmn_not_allowed):
      logger.error("Registration Reject: PLMN not allowed");
      break;
    case (cause_5gmm_t::cause_5gmm_type_::options::ue_security_capabilities_mismatch):
      logger.error("Registration Reject: UE security capabilities mismatch");
      break;
    case (cause_5gmm_t::cause_5gmm_type_::options::mac_failure):
      logger.error("Registration Reject: MAC Failure");
      break;
    case (cause_5gmm_t::cause_5gmm_type_::options::maximum_number_of_pdu_sessions_reached_):
      logger.error("Registration Reject: Maximum number of pdu sessions reached");
      break;
    default:
      logger.error("Unhandled Registration Reject cause");
  }

  return SRSRAN_SUCCESS;
}

int nas_5g::handle_authentication_request(authentication_request_t& authentication_request)
{
  logger.info("Handling Authentication Request");
  ctxt_base.rx_count++;

  // The home network (UDM/ARPF) selects the primary authentication method per TS 33.501; the
  // UE follows whatever the Authentication Request carries. An EAP-Message IE means EAP-AKA'
  // (RFC 5448), otherwise the RAND/AUTN parameters below drive 5G-AKA.
  if (authentication_request.eap_message_present) {
    return handle_eap_aka_prime_challenge(authentication_request);
  }

  // Generate authentication response using RAND, AUTN & KSI-ASME
  plmn_id_t plmn_id;
  usim->get_home_plmn_id(&plmn_id);

  if (authentication_request.authentication_parameter_rand_present == false) {
    logger.error("authentication_parameter_rand_present is not present");
    return SRSRAN_ERROR;
  }

  if (authentication_request.authentication_parameter_autn_present == false) {
    logger.error("authentication_parameter_autn_present is not present");
    return SRSRAN_ERROR;
  }

  initial_sec_command = true;
  uint8_t res_star[16];

  logger.info(authentication_request.authentication_parameter_rand.rand.data(),
              authentication_request.authentication_parameter_rand.rand.size(),
              "Authentication request RAND");

  logger.info(authentication_request.authentication_parameter_autn.autn.data(),
              authentication_request.authentication_parameter_rand.rand.size(),
              "Authentication request AUTN");

  logger.info("Serving network name %s", plmn_id.to_serving_network_name_string().c_str());
  auth_result_t auth_result =
      usim->generate_authentication_response_5g(authentication_request.authentication_parameter_rand.rand.data(),
                                                authentication_request.authentication_parameter_autn.autn.data(),
                                                plmn_id.to_serving_network_name_string().c_str(),
                                                authentication_request.abba.abba_contents.data(),
                                                authentication_request.abba.abba_contents.size(),
                                                res_star,
                                                ctxt_5g.k_amf);

  logger.info(ctxt_5g.k_amf, 32, "Generated k_amf:");

  if (auth_result == AUTH_OK) {
    logger.info("Network authentication successful");
    send_authentication_response(res_star);
    logger.info(res_star, 16, "Generated res_star (%d):", 16);

  } else if (auth_result == AUTH_FAILED) {
    logger.error("Network authentication failure");
    send_authentication_failure(cause_5gmm_t::cause_5gmm_type::mac_failure, res_star);
  } else if (auth_result == AUTH_SYNCH_FAILURE) {
    logger.error("Network authentication synchronization failure");
    send_authentication_failure(cause_5gmm_t::cause_5gmm_type::synch_failure, res_star);
  } else {
    logger.error("Unhandled authentication failure cause");
  }

  return SRSRAN_SUCCESS;
}

int nas_5g::handle_authentication_reject(srsran::nas_5g::authentication_reject_t& authentication_reject)
{
  logger.info("Handling Authentication Reject");
  has_sec_ctxt = false;
  ctxt_base.rx_count++;
  state.set_deregistered(mm5g_state_t::deregistered_substate_t::plmn_search);
  return SRSRAN_SUCCESS;
}

int nas_5g::handle_identity_request(identity_request_t& identity_request)
{
  logger.info("Handling Identity Request");
  ctxt_base.rx_count++;
  send_identity_response(identity_request.identity_type.type_of_identity.value);
  return SRSRAN_SUCCESS;
}

int nas_5g::handle_service_accept(srsran::nas_5g::service_accept_t& service_accept)
{
  logger.info("Handling Service Accept");
  ctxt_base.rx_count++;
  return SRSRAN_SUCCESS;
}

int nas_5g::handle_service_reject(srsran::nas_5g::service_reject_t& service_reject)
{
  logger.info("Handling Service Reject");
  has_sec_ctxt = false;
  ctxt_base.rx_count++;
  return SRSRAN_SUCCESS;
}

int nas_5g::handle_security_mode_command(security_mode_command_t&     security_mode_command,
                                         srsran::unique_byte_buffer_t pdu)
{
  logger.info("Handling Security Mode Command");
  ctxt_base.cipher_algo =
      (CIPHERING_ALGORITHM_ID_ENUM)security_mode_command.selected_nas_security_algorithms.ciphering_algorithm.value;
  ctxt_base.integ_algo =
      (INTEGRITY_ALGORITHM_ID_ENUM)
          security_mode_command.selected_nas_security_algorithms.integrity_protection_algorithm.value;

  // Check replayed ue security capabilities
  if (!check_replayed_ue_security_capabilities(security_mode_command.replayed_ue_security_capabilities)) {
    logger.warning("Sending Security Mode Reject due to security capabilities mismatch");
    send_security_mode_reject(cause_5gmm_t::cause_5gmm_type_::ue_security_capabilities_mismatch);
    return SRSRAN_ERROR;
  }

  if (initial_sec_command) {
    set_k_gnb_count(0);
    ctxt_base.tx_count  = 0;
    initial_sec_command = false;
  }

  // Generate NAS keys
  logger.debug(ctxt_5g.k_amf, 32, "K AMF");
  logger.debug("cipher_algo %d, integ_algo %d", ctxt_base.cipher_algo, ctxt_base.integ_algo);

  usim->generate_nas_keys_5g(
      ctxt_5g.k_amf, ctxt_base.k_nas_enc, ctxt_base.k_nas_int, ctxt_base.cipher_algo, ctxt_base.integ_algo);
  logger.info(ctxt_base.k_nas_enc, 32, "NAS encryption key - k_nas_enc");
  logger.info(ctxt_base.k_nas_int, 32, "NAS integrity key - k_nas_int");

  logger.debug("Generating integrity check. integ_algo:%d, count_dl:%d", ctxt_base.integ_algo, ctxt_base.rx_count);

  if (not integrity_check(pdu.get())) {
    logger.warning("Sending Security Mode Reject due to integrity check failure");
    send_security_mode_reject(cause_5gmm_t::cause_5gmm_type_::options::mac_failure);
    return SRSRAN_ERROR;
  }

  send_security_mode_complete(security_mode_command);
  ctxt_base.rx_count++;
  return SRSRAN_SUCCESS;
}

int nas_5g::handle_deregistration_accept_ue_terminated(
    deregistration_accept_ue_terminated_t& deregistration_accept_ue_terminated)
{
  logger.info("Handling Deregistration Accept UE Terminated");
  ctxt_base.rx_count++;
  return SRSRAN_SUCCESS;
}

int nas_5g::handle_deregistration_request_ue_terminated(
    deregistration_request_ue_terminated_t& deregistration_request_ue_terminated)
{
  logger.info("Handling Deregistration Request UE Terminated");
  ctxt_base.rx_count++;
  return SRSRAN_SUCCESS;
}

int nas_5g::handle_dl_nas_transport(srsran::nas_5g::dl_nas_transport_t& dl_nas_transport)
{
  logger.info("Handling DL NAS transport");
  ctxt_base.rx_count++;
  switch (dl_nas_transport.payload_container_type.payload_container_type) {
    case payload_container_type_t::Payload_container_type_type_::options::n1_sm_information:
      return handle_n1_sm_information(dl_nas_transport.payload_container.payload_container_contents);
      break;
    default:
      logger.warning("Not handling payload container %x",
                     dl_nas_transport.payload_container_type.payload_container_type.value);
      break;
  }
  return SRSRAN_SUCCESS;
}

int nas_5g::handle_n1_sm_information(std::vector<uint8_t> payload_container_contents)
{
  logger.info(payload_container_contents.data(),
              payload_container_contents.size(),
              "Payload contents (length %d)",
              payload_container_contents.size());

  nas_5gs_msg nas_msg;
  nas_msg.unpack(payload_container_contents);

  switch (nas_msg.hdr.message_type) {
    case msg_opts::options::pdu_session_establishment_accept:
      pdu_session_establishment_proc.trigger(nas_msg.pdu_session_establishment_accept());
      break;
    case msg_opts::options::pdu_session_establishment_reject:
      pdu_session_establishment_proc.trigger(nas_msg.pdu_session_establishment_reject());
      break;
    default:
      logger.error(
          "Not handling NAS message type: %s (0x%02x)", nas_msg.hdr.message_type.to_string(), nas_msg.hdr.message_type);
      break;
  }
  return SRSRAN_SUCCESS;
}

int nas_5g::handle_deregistration_accept_ue_originating(
    srsran::nas_5g::deregistration_accept_ue_originating_t& deregistration_accept_ue_originating)
{
  logger.info("Received Deregistration Accept (UE Originating)");
  ctxt_base.rx_count++;
  if (state.get_state() != mm5g_state_t::state_t::deregistered_initiated) {
    logger.warning("Received deregistration accept while not in deregistered initiated state");
  }

  state.set_deregistered(mm5g_state_t::deregistered_substate_t::null);
  return SRSASN_SUCCESS;
}

int nas_5g::handle_configuration_update_command(
    srsran::nas_5g::configuration_update_command_t& configuration_update_command)
{
  logger.info("Handling Configuration Update Command");
  ctxt_base.rx_count++;
  send_configuration_update_complete();
  return SRSRAN_SUCCESS;
}

/*******************************************************************************
 * NAS Timers
 ******************************************************************************/
void nas_5g::timer_expired(uint32_t timeout_id)
{
  // TODO
}

/*******************************************************************************
 * UE Stack & RRC Interface
 ******************************************************************************/
bool nas_5g::is_registered()
{
  return state.get_state() == mm5g_state_t::state_t::registered;
}

int nas_5g::switch_on()
{
  logger.info("Switching on");
  state.set_deregistered(mm5g_state_t::deregistered_substate_t::plmn_search);
  return SRSRAN_SUCCESS;
}

int nas_5g::switch_off()
{
  logger.info("Switching off");
  send_deregistration_request_ue_originating(true);
  return SRSRAN_SUCCESS;
}

int nas_5g::enable_data()
{
  logger.info("Enabling data services");
  return switch_on();
}

int nas_5g::disable_data()
{
  logger.info("Disabling data services");
  // TODO
  return SRSRAN_SUCCESS;
}

int nas_5g::start_service_request()
{
  logger.info("Service Request");
  // TODO
  return SRSRAN_SUCCESS;
}

int nas_5g::reset_pdu_sessions()
{
  for (auto pdu_session : pdu_sessions) {
    pdu_session.established    = false;
    pdu_session.pdu_session_id = 0;
  }
  return SRSRAN_SUCCESS;
}

void nas_5g::get_metrics(nas_5g_metrics_t& metrics)
{
  metrics.nof_active_pdu_sessions = num_of_est_pdu_sessions();
  metrics.state                   = state.get_state();
}

int nas_5g::get_k_amf(as_key_t& k_amf)
{
  if (not has_sec_ctxt) {
    logger.error("K_amf requested before a valid NAS security context was established");
    return SRSRAN_ERROR;
  }

  std::copy(std::begin(ctxt_5g.k_amf), std::end(ctxt_5g.k_amf), k_amf.begin());
  return SRSRAN_SUCCESS;
}

uint32_t nas_5g::get_ul_nas_count()
{
  return ctxt_5g.k_gnb_count;
}

void nas_5g::set_k_gnb_count(uint32_t count)
{
  ctxt_5g.k_gnb_count = count;
}

/*******************************************************************************
 * Helpers
 ******************************************************************************/

void nas_5g::fill_security_caps(srsran::nas_5g::ue_security_capability_t& sec_caps)
{
  if (ia5g_caps[0] == true) {
    sec_caps.ia0_5g_supported = true;
  }
  if (ia5g_caps[1] == true) {
    sec_caps.ia1_128_5g_supported = true;
  }
  if (ia5g_caps[2] == true) {
    sec_caps.ia2_128_5g_supported = true;
  }
  if (ia5g_caps[3] == true) {
    sec_caps.ia3_128_5g_supported = true;
  }
  if (ia5g_caps[4] == true) {
    sec_caps.ia4_5g_supported = true;
  }
  if (ia5g_caps[5] == true) {
    sec_caps.ia5_5g_supported = true;
  }
  if (ia5g_caps[6] == true) {
    sec_caps.ia6_5g_supported = true;
  }
  if (ia5g_caps[7] == true) {
    sec_caps.ia7_5g_supported = true;
  }

  if (ea5g_caps[0] == true) {
    sec_caps.ea0_5g_supported = true;
  }
  if (ea5g_caps[1] == true) {
    sec_caps.ea1_128_5g_supported = true;
  }
  if (ea5g_caps[2] == true) {
    sec_caps.ea2_128_5g_supported = true;
  }
  if (ea5g_caps[3] == true) {
    sec_caps.ea3_128_5g_supported = true;
  }
  if (ea5g_caps[4] == true) {
    sec_caps.ea4_5g_supported = true;
  }
  if (ea5g_caps[5] == true) {
    sec_caps.ea5_5g_supported = true;
  }
  if (ea5g_caps[6] == true) {
    sec_caps.ea6_5g_supported = true;
  }
  if (ea5g_caps[7] == true) {
    sec_caps.ea7_5g_supported = true;
  }
}

bool nas_5g::check_replayed_ue_security_capabilities(srsran::nas_5g::ue_security_capability_t& caps)
{
  if (caps.ia0_5g_supported != ia5g_caps[0] || caps.ea0_5g_supported != ea5g_caps[0]) {
    return false;
  }
  if (caps.ia1_128_5g_supported != ia5g_caps[1] || caps.ea1_128_5g_supported != ea5g_caps[1]) {
    return false;
  }
  if (caps.ia2_128_5g_supported != ia5g_caps[2] || caps.ea2_128_5g_supported != ea5g_caps[2]) {
    return false;
  }
  if (caps.ia3_128_5g_supported != ia5g_caps[3] || caps.ea3_128_5g_supported != ea5g_caps[3]) {
    return false;
  }
  if (caps.ia4_5g_supported != ia5g_caps[4] || caps.ea4_5g_supported != ea5g_caps[4]) {
    return false;
  }
  if (caps.ia5_5g_supported != ia5g_caps[5] || caps.ea5_5g_supported != ea5g_caps[5]) {
    return false;
  }
  if (caps.ia6_5g_supported != ia5g_caps[6] || caps.ea6_5g_supported != ea5g_caps[6]) {
    return false;
  }
  if (caps.ia7_5g_supported != ia5g_caps[7] || caps.ea7_5g_supported != ea5g_caps[7]) {
    return false;
  }

  return true;
}

/*******************************************************************************
 * Helpers for Session Management
 ******************************************************************************/

int nas_5g::trigger_pdu_session_est()
{
  if (unestablished_pdu_sessions() == true) {
    pdu_session_cfg_t pdu_session_cfg;
    uint16_t          pdu_session_id;
    get_unestablished_pdu_session(pdu_session_id, pdu_session_cfg);
    pdu_session_establishment_proc.launch(pdu_session_id, pdu_session_cfg);
  }
  return SRSRAN_SUCCESS;
}

int nas_5g::init_pdu_sessions(std::vector<pdu_session_cfg_t> pdu_session_cfgs)
{
  uint16_t i = 0;
  for (auto pdu_session_cfg : pdu_session_cfgs) {
    pdu_sessions[i].configured      = true;
    pdu_sessions[i].pdu_session_id  = i + 1;
    pdu_sessions[i].pdu_session_cfg = pdu_session_cfg;
  }
  return SRSRAN_SUCCESS;
}

uint32_t nas_5g::num_of_est_pdu_sessions()
{
  uint32_t i = 0;
  for (auto pdu_session : pdu_sessions) {
    if (pdu_session.established == true) {
      i++;
    }
  }
  return i;
}

int nas_5g::configure_pdu_session(uint16_t pdu_session_id)
{
  for (auto pdu_session : pdu_sessions) {
    if (pdu_session.pdu_session_id == pdu_session_id) {
      pdu_session.established = true;
    }
  }
  return SRSRAN_SUCCESS;
}

bool nas_5g::unestablished_pdu_sessions()
{
  for (auto pdu_session : pdu_sessions) {
    if (pdu_session.configured == true && pdu_session.established == false) {
      return true;
    }
  }
  return false;
}

int nas_5g::get_unestablished_pdu_session(uint16_t& pdu_session_id, pdu_session_cfg_t& pdu_session_cfg)
{
  for (auto pdu_session : pdu_sessions) {
    if (pdu_session.configured == true && pdu_session.established == false) {
      pdu_session_id  = pdu_session.pdu_session_id;
      pdu_session_cfg = pdu_session.pdu_session_cfg;
    }
  }
  return SRSRAN_SUCCESS;
}

int nas_5g::add_pdu_session(uint16_t                      pdu_session_id,
                            uint16_t                      pdu_session_type,
                            srsran::nas_5g::pdu_address_t pdu_address)
{
  char* err_str = nullptr;

  // Copy IPv4
  uint32_t ip_addr = 0;

  ip_addr |= pdu_address.ipv4.data()[0] << 24u;
  ip_addr |= pdu_address.ipv4.data()[1] << 16u;
  ip_addr |= pdu_address.ipv4.data()[2] << 8u;
  ip_addr |= pdu_address.ipv4.data()[3];

  // Copy IPv6
  uint8_t ipv6_if_id[8] = {};
  memcpy(ipv6_if_id, pdu_address.ipv6.data(), 8);

  if (!(pdu_session_type == LIBLTE_MME_PDN_TYPE_IPV4V6 || pdu_session_type == LIBLTE_MME_PDN_TYPE_IPV4 ||
        pdu_session_type == LIBLTE_MME_PDN_TYPE_IPV6)) {
    logger.warning("PDU session typed expected to be of IPV4 or IPV6 or IPV4V6");
    return SRSRAN_ERROR;
  }

  if (gw->setup_if_addr(pdu_session_id, pdu_session_type, ip_addr, ipv6_if_id, err_str)) {
    logger.error("%s - %s", gw_setup_failure_str.c_str(), err_str ? err_str : "");
    srsran::console("%s\n", gw_setup_failure_str.c_str());
    return SRSRAN_ERROR;
  }

  if (pdu_session_type == LIBLTE_MME_PDN_TYPE_IPV4V6 || pdu_session_type == LIBLTE_MME_PDN_TYPE_IPV4) {
    logger.info("PDU Session Establishment successful. IP: %u.%u.%u.%u",
                pdu_address.ipv4.data()[0],
                pdu_address.ipv4.data()[1],
                pdu_address.ipv4.data()[2],
                pdu_address.ipv4.data()[3]);

    srsran::console("PDU Session Establishment successful. IP: %u.%u.%u.%u\n",
                    pdu_address.ipv4.data()[0],
                    pdu_address.ipv4.data()[1],
                    pdu_address.ipv4.data()[2],
                    pdu_address.ipv4.data()[3]);
  }

  if (pdu_session_type == LIBLTE_MME_PDN_TYPE_IPV4V6 || pdu_session_type == LIBLTE_MME_PDN_TYPE_IPV6) {
    logger.info("PDU Session Establishment successful. IPv6 interface id: %02x%02x:%02x%02x:%02x%02x:%02x%02x",
                pdu_address.ipv6.data()[0],
                pdu_address.ipv6.data()[1],
                pdu_address.ipv6.data()[2],
                pdu_address.ipv6.data()[3],
                pdu_address.ipv6.data()[4],
                pdu_address.ipv6.data()[5],
                pdu_address.ipv6.data()[6],
                pdu_address.ipv6.data()[7]);

    srsran::console("PDU Session Establishment successful. IPv6 interface id: %02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
                    pdu_address.ipv6.data()[0],
                    pdu_address.ipv6.data()[1],
                    pdu_address.ipv6.data()[2],
                    pdu_address.ipv6.data()[3],
                    pdu_address.ipv6.data()[4],
                    pdu_address.ipv6.data()[5],
                    pdu_address.ipv6.data()[6],
                    pdu_address.ipv6.data()[7]);
  }

  return SRSRAN_SUCCESS;
}

} // namespace srsue
