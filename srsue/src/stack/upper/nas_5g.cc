/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
  modified_registration_request.requested_nssai_present = true;
  modified_registration_request.update_type_5gs_present = true;

  s_nssai_t s_nssai{};
  s_nssai.type                                               = s_nssai_t::SST_type_::options::sst;
  s_nssai.sst                                                = 1;
  modified_registration_request.requested_nssai.s_nssai_list = {s_nssai};

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

  ul_nas_msg.s_nssai_present = true;
  ul_nas_msg.s_nssai.type    = s_nssai_t::SST_type_::options::sst;
  ul_nas_msg.s_nssai.sst     = 1;

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
