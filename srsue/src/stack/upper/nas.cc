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

#include "srsran/common/bcd_helpers.h"
#include "srsran/common/security.h"
#include "srsran/common/string_helpers.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unistd.h>

#include "srsran/asn1/liblte_mme.h"
#include "srsran/common/standard_streams.h"
#include "srsran/interfaces/ue_gw_interfaces.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/interfaces/ue_usim_interfaces.h"
#include "srsue/hdr/stack/upper/nas.h"
#include "srsue/hdr/stack/upper/nas_idle_procedures.h"

#define LTE_MAC_OFFSET 1
#define LTE_SEQ_OFFSET 5
#define LTE_NAS_BEARER 0

using namespace srsran;

namespace srsue {

/*********************************************************************
 *   NAS
 ********************************************************************/

nas::nas(srslog::basic_logger& logger_, srsran::task_sched_handle task_sched_) :
  nas_base(logger_, LTE_MAC_OFFSET, LTE_SEQ_OFFSET, LTE_NAS_BEARER),
  plmn_searcher(this),
  task_sched(task_sched_),
  t3402(task_sched_.get_unique_timer()),
  t3410(task_sched_.get_unique_timer()),
  t3411(task_sched_.get_unique_timer()),
  t3421(task_sched_.get_unique_timer()),
  reattach_timer(task_sched_.get_unique_timer()),
  airplane_mode_sim_timer(task_sched_.get_unique_timer())
{}

int nas::init(usim_interface_nas* usim_, rrc_interface_nas* rrc_, gw_interface_nas* gw_, const nas_args_t& cfg_)
{
  usim = usim_;
  rrc  = rrc_;
  gw   = gw_;

  if (!usim->get_home_plmn_id(&home_plmn)) {
    logger.error("Getting Home PLMN Id from USIM. Defaulting to 001-01");
    home_plmn.from_number(61441, 65281); // This is 001 01
  }

  // parse and sanity check EIA list
  if (parse_security_algorithm_list(cfg_.eia, eia_caps) != SRSRAN_SUCCESS) {
    logger.warning("Failed to parse integrity protection algorithm list: Defaulting to EIA1-128, EIA2-128, EIA3-128");
    eia_caps[0] = false;
    eia_caps[1] = true;
    eia_caps[2] = true;
    eia_caps[3] = true;
  }

  // parse and sanity check EEA list
  if (parse_security_algorithm_list(cfg_.eea, eea_caps) != SRSRAN_SUCCESS) {
    logger.warning("Failed to parse encryption algorithm list: Defaulting to EEA0, EEA1-128, EEA2-128, EEA3-128");
    eea_caps[0] = true;
    eea_caps[1] = true;
    eea_caps[2] = true;
    eea_caps[3] = true;
  }

  cfg = cfg_;

  if ((read_ctxt_file(&ctxt, &ctxt_base))) {
    usim->generate_nas_keys(
        ctxt.k_asme, ctxt_base.k_nas_enc, ctxt_base.k_nas_int, ctxt_base.cipher_algo, ctxt_base.integ_algo);
    logger.debug(ctxt_base.k_nas_enc, 32, "NAS encryption key - k_nas_enc");
    logger.debug(ctxt_base.k_nas_int, 32, "NAS integrity key - k_nas_int");
  }

  // Configure timers
  t3402.set(t3402_duration_ms, [this](uint32_t tid) { timer_expired(tid); });
  t3410.set(t3410_duration_ms, [this](uint32_t tid) { timer_expired(tid); });
  t3411.set(t3411_duration_ms, [this](uint32_t tid) { timer_expired(tid); });
  t3421.set(t3421_duration_ms, [this](uint32_t tid) { timer_expired(tid); });
  reattach_timer.set(reattach_timer_duration_ms, [this](uint32_t tid) { timer_expired(tid); });

  if (cfg.sim.airplane_t_on_ms > 0) {
    airplane_mode_sim_timer.set(cfg.sim.airplane_t_on_ms, [this](uint32_t tid) { timer_expired(tid); });
    airplane_mode_sim_timer.run();
  }

  running = true;
  return SRSRAN_SUCCESS;
}

nas::~nas() {}

void nas::stop()
{
  running = false;
  write_ctxt_file(ctxt, ctxt_base);
}

void nas::get_metrics(nas_metrics_t* m)
{
  nas_metrics_t metrics         = {};
  metrics.state                 = state.get_state();
  metrics.nof_active_eps_bearer = eps_bearer.size();
  *m                            = metrics;
}

void nas::run_tti()
{
  // Process PLMN selection ongoing procedures
  callbacks.run();

  // Transmit initiating messages if necessary
  switch (state.get_state()) {
    case emm_state_t::state_t::deregistered:
      // TODO Make sure cell selection is finished after transitioning from another state (if required)
      // Make sure the RRC is finished transitioning to RRC Idle
      if (reattach_timer.is_running()) {
        logger.debug("Waiting for re-attach timer to expire to attach again.");
        return;
      }
      switch (state.get_deregistered_substate()) {
        case emm_state_t::deregistered_substate_t::plmn_search:
          start_plmn_selection_proc();
          break;
        case emm_state_t::deregistered_substate_t::normal_service:
        case emm_state_t::deregistered_substate_t::attach_needed:
          start_attach_request(srsran::establishment_cause_t::mo_data);
          break;
        case emm_state_t::deregistered_substate_t::attempting_to_attach:
          logger.debug("Attempting to attach");
        default:
          break;
      }
    case emm_state_t::state_t::registered:
      break;
    case emm_state_t::state_t::deregistered_initiated:
      logger.debug("UE detaching...");
      break;
    default:
      break;
  }
}

// Helper method to inform GW about remove default EPS bearer
void nas::clear_eps_bearer()
{
  // Deactivate EPS bearer according to Sec. 5.5.2.2.2
  logger.debug("Clearing EPS bearer context");
  for (const auto& bearer : eps_bearer) {
    gw->deactivate_eps_bearer(bearer.second.eps_bearer_id);
  }
  eps_bearer.clear();
}

/*******************************************************************************
 * FSM Helpers
 ******************************************************************************/
void nas::enter_emm_null()
{
  clear_eps_bearer();
  state.set_null();
}

void nas::enter_emm_deregistered_initiated()
{
  clear_eps_bearer();
  state.set_deregistered_initiated();
}

void nas::enter_emm_deregistered(emm_state_t::deregistered_substate_t substate)
{
  // TODO Start cell selection.
  clear_eps_bearer();
  state.set_deregistered(substate);
}

/*******************************************************************************
 * NAS Timers
 ******************************************************************************/
void nas::timer_expired(uint32_t timeout_id)
{
  if (timeout_id == t3402.id()) {
    logger.warning("Timer T3402 expired: trying to attach again");
    attach_attempt_counter = 0; // Sec. 5.5.1.1
    enter_emm_deregistered(emm_state_t::deregistered_substate_t::plmn_search);
  } else if (timeout_id == t3410.id()) {
    // Section 5.5.1.2.6 case c)
    attach_attempt_counter++;

    srsran::console("Attach failed (attempt %d/%d)\n", attach_attempt_counter, max_attach_attempts);
    if (attach_attempt_counter < max_attach_attempts) {
      logger.warning("Timer T3410 expired after attach attempt %d/%d: starting T3411",
                     attach_attempt_counter,
                     max_attach_attempts);

      // start T3411
      t3411.run();
      enter_emm_deregistered(emm_state_t::deregistered_substate_t::attempting_to_attach);
    } else {
      // maximum attach attempts reached
      logger.warning("Timer T3410 expired. Maximum attempts reached. Starting T3402");
      t3402.run();
      reset_security_context();
    }
  } else if (timeout_id == t3411.id()) {
    // In order to allow reattaching the UE, we switch into EMM_STATE_DEREGISTERED straight
    enter_emm_deregistered(emm_state_t::deregistered_substate_t::plmn_search);
  } else if (timeout_id == reattach_timer.id()) {
    logger.warning("Reattach timer expired: trying to attach again");
    start_attach_request(srsran::establishment_cause_t::mo_sig);
  } else if (timeout_id == airplane_mode_sim_timer.id()) {
    logger.debug("Airplane mode simulation timer expired after %dms, airplane mode is currently %s.",
                 airplane_mode_sim_timer.time_elapsed(),
                 airplane_mode_state == DISABLED ? "disabled" : "enabled");
    if (airplane_mode_state == DISABLED) {
      // Enabling air-plane mode
      send_detach_request(true);
      airplane_mode_state = ENABLED;

      if (cfg.sim.airplane_t_on_ms > 0) {
        airplane_mode_sim_timer.set(cfg.sim.airplane_t_on_ms, [this](uint32_t tid) { timer_expired(tid); });
        airplane_mode_sim_timer.run();
      }
    } else if (airplane_mode_state == ENABLED) {
      // Disabling airplane mode again
      start_attach_request(srsran::establishment_cause_t::mo_sig);
      airplane_mode_state = DISABLED;

      if (cfg.sim.airplane_t_off_ms > 0) {
        airplane_mode_sim_timer.set(cfg.sim.airplane_t_off_ms, [this](uint32_t tid) { timer_expired(tid); });
        airplane_mode_sim_timer.run();
      }
    }
  } else {
    logger.error("Timeout from unknown timer id %d", timeout_id);
  }
}
/*******************************************************************************
 * UE Stack and RRC common Interface
 ******************************************************************************/
bool nas::is_registered()
{
  return state.get_state() == emm_state_t::state_t::registered;
}

/*******************************************************************************
 * UE Stack Interface
 ******************************************************************************/
bool nas::switch_on()
{
  logger.info("Switching on");
  state.set_deregistered(emm_state_t::deregistered_substate_t::plmn_search);
  return true;
}

bool nas::switch_off()
{
  logger.info("Switching off");
  detach_request(true);
  return true;
}

bool nas::enable_data()
{
  logger.info("Enabling data services");
  return switch_on();
}

bool nas::disable_data()
{
  logger.info("Disabling data services");
  detach_request(false);
  return true;
}

/**
 * Non-blocking function to Attach to the network and establish RRC connection if not established.
 * The function returns true if the UE could attach correctly or false in case of error or timeout during attachment.
 *
 */
void nas::start_attach_request(srsran::establishment_cause_t cause_)
{
  logger.info("Attach Request with cause %s.", to_string(cause_).c_str());

  if (state.get_state() != emm_state_t::state_t::deregistered) {
    logger.info("NAS in invalid state for Attach Request");
    logger.info("Attach request ignored. State = %s", state.get_full_state_text().c_str());
    return;
  }

  // start T3410
  logger.debug("Starting T3410");
  t3410.run();

  // stop T3411
  if (t3411.is_running()) {
    t3411.stop();
  }

  // Todo: stop T3402
  if (t3402.is_running()) {
    t3402.stop();
  }

  // Start attach request
  unique_byte_buffer_t msg = srsran::make_byte_buffer();
  if (msg == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  gen_attach_request(msg);
  if (rrc->is_connected()) {
    rrc->write_sdu(std::move(msg));
  } else {
    logger.debug("Initiating RRC Connection");
    if (not rrc->connection_request(cause_, std::move(msg))) {
      logger.error("Error starting RRC connection");
      return;
    }
  }

  state.set_registered_initiated();
}

/**
 * Non-blocking function to start the Service to the network .
 * The function returns true if the UE could attach correctly or false in case of error or timeout during attachment.
 *
 */
void nas::start_service_request(srsran::establishment_cause_t cause_)
{
  logger.info("Service Request with cause %s.", to_string(cause_).c_str());
  srsran::console("Service Request with cause %s.\n", to_string(cause_).c_str());
  if (state.get_state() != emm_state_t::state_t::registered) {
    logger.info("NAS in invalid state for Service Request");
    logger.info("Service request ignored. State = %s", state.get_full_state_text().c_str());
    return;
  }

  if (rrc->is_connected()) {
    logger.info("NAS is already registered and RRC connected");
    logger.info("Service request ignored. State = %s", state.get_full_state_text().c_str());
    return;
  }
  logger.info("NAS is already registered but RRC disconnected. Connecting now...");

  // Start service request
  unique_byte_buffer_t msg = srsran::make_byte_buffer();
  if (msg == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }
  gen_service_request(msg);
  if (not rrc->connection_request(cause_, std::move(msg))) {
    logger.error("Error starting RRC connection");
    return;
  }
  state.set_service_request_initiated();
}

/**
 * Non-blocking function to send the detach request to the network .
 * The function returns true if the UE could attach correctly or false in case of error or timeout during attachment.
 *
 */
bool nas::detach_request(const bool switch_off)
{
  switch (state.get_state()) {
    case emm_state_t::state_t::deregistered:
    case emm_state_t::state_t::deregistered_initiated: // Fall-through
      // do nothing ..
      break;
    case emm_state_t::state_t::service_request_initiated:
    case emm_state_t::state_t::registered_initiated:
    case emm_state_t::state_t::registered: // Fall-through
      // send detach request
      send_detach_request(switch_off);
      break;
    default:
      logger.debug("Received request to detach in state %s", state.get_full_state_text().c_str());
      break;
  }
  if (switch_off) {
    enter_emm_null();
  }
  return false;
}

/*******************************************************************************
 * RRC Interface
 ******************************************************************************/
// Signal from RRC that connection request proc completed
bool nas::connection_request_completed(bool outcome)
{
  if (outcome == true) {
    logger.debug("RRC connection request completed. NAS State %s.", state.get_full_state_text().c_str());
    if (state.get_state() == emm_state_t::state_t::service_request_initiated) {
      srsran::console("Service Request successful.\n");
      logger.info("Service Request successful.");
      rrc->paging_completed(true);
      state.set_registered(emm_state_t::registered_substate_t::normal_service);
    }
  } else {
    logger.debug("RRC connection request failed. NAS State %s.", state.get_full_state_text().c_str());
    if (state.get_state() == emm_state_t::state_t::service_request_initiated) {
      srsran::console("RRC connection for Service Request failed.\n");
      logger.info("RRC connection for Service Request failed.");
      rrc->paging_completed(false);
      state.set_registered(emm_state_t::registered_substate_t::normal_service);
    }
  }
  return true;
}

void nas::plmn_search_completed(const found_plmn_t found_plmns[MAX_FOUND_PLMNS], int nof_plmns)
{
  plmn_searcher.trigger(plmn_search_proc::plmn_search_complete_t(found_plmns, nof_plmns));
}

void nas::left_rrc_connected()
{
  logger.debug("RRC no longer connected. NAS State %s.", state.get_full_state_text().c_str());
}

bool nas::paging(s_tmsi_t* ue_identity)
{
  if (state.get_state() == emm_state_t::state_t::registered) {
    logger.info("Received paging: requesting RRC connection establishment");
    start_service_request(srsran::establishment_cause_t::mt_access);
  } else {
    logger.warning("Received paging while in state %s", state.get_full_state_text().c_str());
    return false;
  }
  return true;
}

void nas::set_barring(barring_t barring)
{
  current_barring = barring;
}

void nas::write_pdu(uint32_t lcid, unique_byte_buffer_t pdu)
{
  uint8 pd           = 0;
  uint8 msg_type     = 0;
  uint8 sec_hdr_type = 0;

  logger.info(pdu->msg, pdu->N_bytes, "DL %s PDU", rrc->get_rb_name(lcid));

  // Parse the message security header
  liblte_mme_parse_msg_sec_header((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &pd, &sec_hdr_type);
  switch (sec_hdr_type) {
    case LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS:
    case LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_WITH_NEW_EPS_SECURITY_CONTEXT:
    case LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST:
      break;
    case LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY:
    case LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED:
      if ((integrity_check(pdu.get()))) {
        if (sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED) {
          cipher_decrypt(pdu.get());
        }
        break;
      } else {
        logger.error("Not handling NAS message with integrity check error");
        return;
      }
    default:
      logger.error("Not handling NAS message with SEC_HDR_TYPE=%02X", sec_hdr_type);
      return;
  }

  // Write NAS pcap
  if (pcap != nullptr) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  // Parse the message header
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &pd, &msg_type);
  logger.info(pdu->msg, pdu->N_bytes, "DL %s Decrypted PDU", rrc->get_rb_name(lcid));

  // drop messages if integrity protection isn't applied (see TS 24.301 Sec. 4.4.4.2)
  if (sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS) {
    switch (msg_type) {
      case LIBLTE_MME_MSG_TYPE_IDENTITY_REQUEST: // special case for IMSI is checked in parse_identity_request()
      case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_REQUEST:
      case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_REJECT:
      case LIBLTE_MME_MSG_TYPE_ATTACH_REJECT:
      case LIBLTE_MME_MSG_TYPE_DETACH_ACCEPT:
      case LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_REJECT:
      case LIBLTE_MME_MSG_TYPE_SERVICE_REJECT:
        break;
      default:
        logger.error("Not handling NAS message MSG_TYPE=%02X with SEC_HDR_TYPE=%02X without integrity protection!",
                     msg_type,
                     sec_hdr_type);
        return;
    }
  }

  // Reserved for Security Mode Command (Sec 9.3.1)
  if (sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_WITH_NEW_EPS_SECURITY_CONTEXT &&
      msg_type != LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMMAND) {
    logger.error("Not handling NAS message MSG_TYPE=%02X with SEC_HDR_TYPE=%02X. Security header type reserved!",
                 msg_type,
                 sec_hdr_type);
    return;
  }

  switch (msg_type) {
    case LIBLTE_MME_MSG_TYPE_ATTACH_ACCEPT:
      parse_attach_accept(lcid, std::move(pdu));
      break;
    case LIBLTE_MME_MSG_TYPE_ATTACH_REJECT:
      parse_attach_reject(lcid, std::move(pdu), sec_hdr_type);
      break;
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_REQUEST:
      parse_authentication_request(lcid, std::move(pdu), sec_hdr_type);
      break;
    case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_REJECT:
      parse_authentication_reject(lcid, std::move(pdu));
      break;
    case LIBLTE_MME_MSG_TYPE_IDENTITY_REQUEST:
      parse_identity_request(std::move(pdu), sec_hdr_type);
      break;
    case LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMMAND:
      parse_security_mode_command(lcid, std::move(pdu));
      break;
    case LIBLTE_MME_MSG_TYPE_SERVICE_REJECT:
      parse_service_reject(lcid, std::move(pdu), sec_hdr_type);
      break;
    case LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_REQUEST:
      parse_esm_information_request(lcid, std::move(pdu));
      break;
    case LIBLTE_MME_MSG_TYPE_EMM_INFORMATION:
      parse_emm_information(lcid, std::move(pdu));
      break;
    case LIBLTE_MME_MSG_TYPE_EMM_STATUS:
      parse_emm_status(lcid, std::move(pdu));
      break;
    case LIBLTE_MME_MSG_TYPE_DETACH_REQUEST:
      parse_detach_request(lcid, std::move(pdu));
      break;
    case LIBLTE_MME_MSG_TYPE_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST:
      parse_activate_dedicated_eps_bearer_context_request(lcid, std::move(pdu));
      break;
    case LIBLTE_MME_MSG_TYPE_DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST:
      parse_deactivate_eps_bearer_context_request(std::move(pdu));
      break;
    case LIBLTE_MME_MSG_TYPE_MODIFY_EPS_BEARER_CONTEXT_REQUEST:
      parse_modify_eps_bearer_context_request(std::move(pdu));
      break;
    case LIBLTE_MME_MSG_TYPE_ACTIVATE_TEST_MODE:
      parse_activate_test_mode(lcid, std::move(pdu));
      break;
    case LIBLTE_MME_MSG_TYPE_CLOSE_UE_TEST_LOOP:
      parse_close_ue_test_loop(lcid, std::move(pdu));
      break;
    // TODO: Handle deactivate test mode and ue open test loop
    case LIBLTE_MME_MSG_TYPE_OPEN_UE_TEST_LOOP:
    case LIBLTE_MME_MSG_TYPE_DEACTIVATE_TEST_MODE:
      gw->set_test_loop_mode(gw_interface_nas::TEST_LOOP_INACTIVE);
      break;
    default:
      logger.error("Not handling NAS message with MSG_TYPE=%02X", msg_type);
      return;
  }
}

void nas::set_k_enb_count(uint32_t count)
{
  // UL count for RRC key derivation depends on UL Count of the Attach Request or Service Request.
  // On the case of an Authentication Request, the UL count used to generate K_enb must be reset to zero.
  ctxt.k_enb_count = count;
}

uint32_t nas::get_k_enb_count()
{
  return ctxt.k_enb_count;
}

bool nas::get_k_asme(uint8_t* k_asme_, uint32_t n)
{
  if (!have_ctxt) {
    logger.error("K_asme requested before security context established");
    return false;
  }
  if (nullptr == k_asme_ || n < 32) {
    logger.error("Invalid parameters to get_k_asme");
    return false;
  }

  memcpy(k_asme_, ctxt.k_asme, 32);
  return true;
}

uint32_t nas::get_ipv4_addr()
{
  return ip_addr;
}

bool nas::get_ipv6_addr(uint8_t* ipv6_addr)
{
  uint8_t null_addr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  if (memcmp(ipv6_addr, null_addr, 8) != 0) {
    memcpy(ipv6_addr, ipv6_if_id, 8);
    return true;
  }
  return false;
}

/*******************************************************************************
 * PLMN selection helpers
 ******************************************************************************/
void nas::start_plmn_selection_proc()
{
  logger.debug("Attempting to select PLMN");
  if (plmn_searcher.is_idle()) {
    logger.info("No PLMN selected. Starting PLMN Selection...");
    if (not plmn_searcher.launch()) {
      logger.error("Error starting PLMN selection");
      return;
    }
  }
}

// Select PLMN from list of known PLMNs.
// TODO check whether PLMN or Tracking Area of the selected cell is in forbiden list.
void nas::select_plmn()
{
  logger.debug("Selecting PLMN from list of known PLMNs.");

  // check whether the state hasn't changed
  if (state.get_state() != emm_state_t::state_t::deregistered and
      state.get_deregistered_substate() != emm_state_t::deregistered_substate_t::plmn_search) {
    logger.error("Selecting PLMN when in incorrect EMM State");
    return;
  }

  // First find if Home PLMN is available
  for (const srsran::plmn_id_t& known_plmn : known_plmns) {
    if (known_plmn == home_plmn) {
      logger.info("Selecting Home PLMN Id=%s", known_plmn.to_string().c_str());
      current_plmn = known_plmn;
      state.set_deregistered(emm_state_t::deregistered_substate_t::normal_service);
      return;
    }
  }

  // If not, select the first available PLMN
  if (not known_plmns.empty()) {
    std::string debug_str = "Could not find Home PLMN Id=" + home_plmn.to_string() +
                            ", trying to connect to PLMN Id=" + known_plmns[0].to_string();
    logger.info("%s", debug_str.c_str());
    srsran::console("%s\n", debug_str.c_str());
    current_plmn = known_plmns[0];
    state.set_deregistered(emm_state_t::deregistered_substate_t::normal_service);
  }

  // reset attach attempt counter (Sec. 5.2.2.3.4)
  if (state.get_deregistered_substate() == emm_state_t::deregistered_substate_t::normal_service) {
    attach_attempt_counter = 0;
  }
}


bool nas::check_cap_replay(LIBLTE_MME_UE_SECURITY_CAPABILITIES_STRUCT* caps)
{
  for (uint32_t i = 0; i < 8; i++) {
    if (caps->eea[i] != eea_caps[i] || caps->eia[i] != eia_caps[i]) {
      return false;
    }
  }
  return true;
}

/**
 * Applies the current security config on a packed NAS PDU
 *
 * The PDU is ciphered and integrity protected if NAS security is configured,
 * the header type requires security protection and the current security context is valid.
 * The function also performs a length check.
 *
 * @param pdu The NAS PDU already packed inside a byte_buffer
 * @param sec_hdr_type Security header type of the message
 * @return True if successful, false otherwise
 */
int nas::apply_security_config(srsran::unique_byte_buffer_t& pdu, uint8_t sec_hdr_type)
{
  if (have_ctxt) {
    if (pdu->N_bytes > 5) {
      if (sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED ||
          sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED_WITH_NEW_EPS_SECURITY_CONTEXT) {
        cipher_encrypt(pdu.get());
      }
      if (sec_hdr_type >= LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY) {
        integrity_generate(&ctxt_base.k_nas_int[16],
                           ctxt_base.tx_count,
                           SECURITY_DIRECTION_UPLINK,
                           &pdu->msg[5],
                           pdu->N_bytes - 5,
                           &pdu->msg[1]);
      }
    } else {
      logger.error("Invalid PDU size %d", pdu->N_bytes);
      return SRSRAN_ERROR;
    }
  } else {
    logger.debug("Not applying security for PDU. No context configured.");
  }
  return SRSRAN_SUCCESS;
}

/**
 * Reset and delete any GUTI, TAI list, last visited registered TAI, list of equivalent PLMNs and KSI
 */
void nas::reset_security_context()
{
  have_guti       = false;
  have_ctxt       = false;
  current_sec_hdr = LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS;
  ctxt            = {};
  ctxt.ksi        = LIBLTE_MME_NAS_KEY_SET_IDENTIFIER_NO_KEY_AVAILABLE;
}

/*******************************************************************************
 * Parsers
 ******************************************************************************/

void nas::parse_attach_accept(uint32_t lcid, unique_byte_buffer_t pdu)
{
  if (!pdu) {
    logger.error("Invalid PDU");
    return;
  }

  if (pdu->N_bytes <= 5) {
    logger.error("Invalid attach accept PDU size (%d)", pdu->N_bytes);
    return;
  }

  logger.info("Received Attach Accept");

  // stop T3410
  if (t3410.is_running()) {
    logger.debug("Stopping T3410");
    t3410.stop();
  }

  LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT attach_accept = {};
  liblte_mme_unpack_attach_accept_msg((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &attach_accept);

  if (attach_accept.eps_attach_result == LIBLTE_MME_EPS_ATTACH_RESULT_EPS_ONLY) {
    // TODO: Handle t3412.unit
    // TODO: Handle tai_list
    if (attach_accept.guti_present) {
      memcpy(&ctxt.guti, &attach_accept.guti.guti, sizeof(LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT));
      have_guti = true;
      // Update RRC UE-Idenity
      s_tmsi_t s_tmsi;
      s_tmsi.mmec   = ctxt.guti.mme_code;
      s_tmsi.m_tmsi = ctxt.guti.m_tmsi;
      rrc->set_ue_identity(s_tmsi);
    }
    if (attach_accept.lai_present) {
      ; // Do nothing;
    }
    if (attach_accept.ms_id_present) {
      ; // Do nothing;
    }
    if (attach_accept.emm_cause_present) {
      ; // Do nothing;
    }
    if (attach_accept.t3402_present) {
      ; // Do nothing;
    }
    if (attach_accept.t3412_ext_present) {
      ; // Do nothing;
    }
    if (attach_accept.t3423_present) {
      ; // Do nothing;
    }
    if (attach_accept.equivalent_plmns_present) {
      ; // Do nothing;
    }
    if (attach_accept.emerg_num_list_present) {
      ; // Do nothing;
    }
    if (attach_accept.eps_network_feature_support_present) {
      ; // Do nothing;
    }
    if (attach_accept.additional_update_result_present) {
      ; // Do nothing;
    }

    LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT act_def_eps_bearer_context_req = {};
    liblte_mme_unpack_activate_default_eps_bearer_context_request_msg(&attach_accept.esm_msg,
                                                                      &act_def_eps_bearer_context_req);

    // make sure we don't already have a default EPS bearer activated
    for (const auto& bearer : eps_bearer) {
      if (bearer.second.type == DEFAULT_EPS_BEARER) {
        logger.error("Only one default EPS bearer supported.");
        return;
      }
    }

    if ((cfg.apn_protocol == "ipv4" && LIBLTE_MME_PDN_TYPE_IPV6 == act_def_eps_bearer_context_req.pdn_addr.pdn_type) ||
        (cfg.apn_protocol == "ipv6" && LIBLTE_MME_PDN_TYPE_IPV4 == act_def_eps_bearer_context_req.pdn_addr.pdn_type)) {
      logger.error("Failed to attach -- Mismatch between PDN protocol and PDN type in attach accept.");
      return;
    }
    if (("ipv4v6" == cfg.apn_protocol &&
         LIBLTE_MME_PDN_TYPE_IPV4 == act_def_eps_bearer_context_req.pdn_addr.pdn_type) ||
        ("ipv4v6" == cfg.apn_protocol &&
         LIBLTE_MME_PDN_TYPE_IPV6 == act_def_eps_bearer_context_req.pdn_addr.pdn_type)) {
      logger.warning("Requested IPv4v6, but only received a single PDN address.");
      logger.warning("EMM Cause: %d", attach_accept.emm_cause);
    }
    if (LIBLTE_MME_PDN_TYPE_IPV4 == act_def_eps_bearer_context_req.pdn_addr.pdn_type) {
      ip_addr = 0;
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[0] << 24u;
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[1] << 16u;
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[2] << 8u;
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[3];

      logger.info("Network attach successful. APN: %s, IP: %u.%u.%u.%u",
                  act_def_eps_bearer_context_req.apn.apn,
                  act_def_eps_bearer_context_req.pdn_addr.addr[0],
                  act_def_eps_bearer_context_req.pdn_addr.addr[1],
                  act_def_eps_bearer_context_req.pdn_addr.addr[2],
                  act_def_eps_bearer_context_req.pdn_addr.addr[3]);

      srsran::console("Network attach successful. IP: %u.%u.%u.%u\n",
                      act_def_eps_bearer_context_req.pdn_addr.addr[0],
                      act_def_eps_bearer_context_req.pdn_addr.addr[1],
                      act_def_eps_bearer_context_req.pdn_addr.addr[2],
                      act_def_eps_bearer_context_req.pdn_addr.addr[3]);

      // Setup GW
      char* err_str = nullptr;
      if (gw->setup_if_addr(act_def_eps_bearer_context_req.eps_bearer_id,
                            LIBLTE_MME_PDN_TYPE_IPV4,
                            ip_addr,
                            nullptr,
                            err_str)) {
        logger.error("%s - %s", gw_setup_failure_str.c_str(), err_str ? err_str : "");
        srsran::console("%s\n", gw_setup_failure_str.c_str());
      }
    } else if (LIBLTE_MME_PDN_TYPE_IPV6 == act_def_eps_bearer_context_req.pdn_addr.pdn_type) {
      memcpy(ipv6_if_id, act_def_eps_bearer_context_req.pdn_addr.addr, 8);
      logger.info("Network attach successful. APN: %s, IPv6 interface id: %02x%02x:%02x%02x:%02x%02x:%02x%02x",
                  act_def_eps_bearer_context_req.apn.apn,
                  act_def_eps_bearer_context_req.pdn_addr.addr[0],
                  act_def_eps_bearer_context_req.pdn_addr.addr[1],
                  act_def_eps_bearer_context_req.pdn_addr.addr[2],
                  act_def_eps_bearer_context_req.pdn_addr.addr[3],
                  act_def_eps_bearer_context_req.pdn_addr.addr[4],
                  act_def_eps_bearer_context_req.pdn_addr.addr[5],
                  act_def_eps_bearer_context_req.pdn_addr.addr[6],
                  act_def_eps_bearer_context_req.pdn_addr.addr[7]);

      srsran::console("Network attach successful. IPv6 interface Id: %02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
                      act_def_eps_bearer_context_req.pdn_addr.addr[0],
                      act_def_eps_bearer_context_req.pdn_addr.addr[1],
                      act_def_eps_bearer_context_req.pdn_addr.addr[2],
                      act_def_eps_bearer_context_req.pdn_addr.addr[3],
                      act_def_eps_bearer_context_req.pdn_addr.addr[4],
                      act_def_eps_bearer_context_req.pdn_addr.addr[5],
                      act_def_eps_bearer_context_req.pdn_addr.addr[6],
                      act_def_eps_bearer_context_req.pdn_addr.addr[7]);
      // Setup GW
      char* err_str = nullptr;
      if (gw->setup_if_addr(act_def_eps_bearer_context_req.eps_bearer_id,
                            LIBLTE_MME_PDN_TYPE_IPV6,
                            0,
                            ipv6_if_id,
                            err_str)) {
        logger.error("%s - %s", gw_setup_failure_str.c_str(), err_str);
        srsran::console("%s\n", gw_setup_failure_str.c_str());
      }
    } else if (LIBLTE_MME_PDN_TYPE_IPV4V6 == act_def_eps_bearer_context_req.pdn_addr.pdn_type) {
      memcpy(ipv6_if_id, act_def_eps_bearer_context_req.pdn_addr.addr, 8);
      // IPv6
      logger.info("Network attach successful. APN: %s, IPv6 interface id: %02x%02x:%02x%02x:%02x%02x:%02x%02x",
                  act_def_eps_bearer_context_req.apn.apn,
                  act_def_eps_bearer_context_req.pdn_addr.addr[0],
                  act_def_eps_bearer_context_req.pdn_addr.addr[1],
                  act_def_eps_bearer_context_req.pdn_addr.addr[2],
                  act_def_eps_bearer_context_req.pdn_addr.addr[3],
                  act_def_eps_bearer_context_req.pdn_addr.addr[4],
                  act_def_eps_bearer_context_req.pdn_addr.addr[5],
                  act_def_eps_bearer_context_req.pdn_addr.addr[6],
                  act_def_eps_bearer_context_req.pdn_addr.addr[7]);
      srsran::console("Network attach successful. IPv6 interface Id: %02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
                      act_def_eps_bearer_context_req.pdn_addr.addr[0],
                      act_def_eps_bearer_context_req.pdn_addr.addr[1],
                      act_def_eps_bearer_context_req.pdn_addr.addr[2],
                      act_def_eps_bearer_context_req.pdn_addr.addr[3],
                      act_def_eps_bearer_context_req.pdn_addr.addr[4],
                      act_def_eps_bearer_context_req.pdn_addr.addr[5],
                      act_def_eps_bearer_context_req.pdn_addr.addr[6],
                      act_def_eps_bearer_context_req.pdn_addr.addr[7]);
      // IPv4
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[8] << 24u;
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[9] << 16u;
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[10] << 8u;
      ip_addr |= act_def_eps_bearer_context_req.pdn_addr.addr[11];

      logger.info("Network attach successful. APN: %s, IP: %u.%u.%u.%u",
                  act_def_eps_bearer_context_req.apn.apn,
                  act_def_eps_bearer_context_req.pdn_addr.addr[8],
                  act_def_eps_bearer_context_req.pdn_addr.addr[9],
                  act_def_eps_bearer_context_req.pdn_addr.addr[10],
                  act_def_eps_bearer_context_req.pdn_addr.addr[11]);

      srsran::console("Network attach successful. IP: %u.%u.%u.%u\n",
                      act_def_eps_bearer_context_req.pdn_addr.addr[8],
                      act_def_eps_bearer_context_req.pdn_addr.addr[9],
                      act_def_eps_bearer_context_req.pdn_addr.addr[10],
                      act_def_eps_bearer_context_req.pdn_addr.addr[11]);

      char* err_str = nullptr;
      if (gw->setup_if_addr(act_def_eps_bearer_context_req.eps_bearer_id,
                            LIBLTE_MME_PDN_TYPE_IPV4V6,
                            ip_addr,
                            ipv6_if_id,
                            err_str)) {
        logger.error("%s - %s", gw_setup_failure_str.c_str(), err_str);
        srsran::console("%s\n", gw_setup_failure_str.c_str());
      }
    } else {
      logger.error("PDN type not IPv4, IPv6 nor IPv4v6");
      return;
    }

    if (act_def_eps_bearer_context_req.transaction_id_present) {
      transaction_id = act_def_eps_bearer_context_req.proc_transaction_id;
    }

    // Search for DNS entry in protocol config options
    if (act_def_eps_bearer_context_req.protocol_cnfg_opts_present) {
      for (uint32_t i = 0; i < act_def_eps_bearer_context_req.protocol_cnfg_opts.N_opts; i++) {
        if (act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[i].id ==
            LIBLTE_MME_ADDITIONAL_PARAMETERS_DL_DNS_SERVER_IPV4_ADDRESS) {
          uint32_t dns_addr = 0;
          dns_addr |= act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[i].contents[0] << 24u;
          dns_addr |= act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[i].contents[1] << 16u;
          dns_addr |= act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[i].contents[2] << 8u;
          dns_addr |= act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[i].contents[3];
          logger.info("DNS: %u.%u.%u.%u",
                      act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[i].contents[0],
                      act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[i].contents[1],
                      act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[i].contents[2],
                      act_def_eps_bearer_context_req.protocol_cnfg_opts.opt[i].contents[3]);
        }
      }
    }

    // TODO: Handle the following parameters
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

    // TODO: Setup the default EPS bearer context

    eps_bearer_t bearer  = {};
    bearer.type          = DEFAULT_EPS_BEARER;
    bearer.eps_bearer_id = act_def_eps_bearer_context_req.eps_bearer_id;
    if (eps_bearer.insert(eps_bearer_map_pair_t(bearer.eps_bearer_id, bearer)).second) {
      // bearer added successfully
      attach_attempt_counter = 0; // reset according to 5.5.1.1
      state.set_registered(emm_state_t::registered_substate_t::normal_service);

      // send attach complete
      send_attach_complete(transaction_id, bearer.eps_bearer_id);
    } else {
      // bearer already exists (perhaps the attach complete got lost and this is a retx?)
      // TODO: what are we supposed to do in this case?
      logger.error("Error adding EPS bearer.");
    }

  } else {
    logger.info("Not handling attach type %u", attach_accept.eps_attach_result);
    enter_emm_deregistered(emm_state_t::deregistered_substate_t::plmn_search);
  }

  ctxt_base.rx_count++;
}

void nas::parse_attach_reject(uint32_t lcid, unique_byte_buffer_t pdu, const uint8_t sec_hdr_type)
{
  LIBLTE_MME_ATTACH_REJECT_MSG_STRUCT attach_rej;
  ZERO_OBJECT(attach_rej);

  liblte_mme_unpack_attach_reject_msg((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &attach_rej);
  logger.warning("Received Attach Reject. Cause= %02X", attach_rej.emm_cause);
  srsran::console("Received Attach Reject. Cause= %02X\n", attach_rej.emm_cause);

  // do not accept if the message is not protected when the EMM cause is #25 (TS 24.301 Sec. 4.4.4.2)
  if (sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS &&
      attach_rej.emm_cause == LIBLTE_MME_EMM_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG) {
    logger.error("Not handling NAS Attach Reject message with EMM cause #25 without integrity protection!");
    return;
  }

  // stop T3410
  if (t3410.is_running()) {
    logger.debug("Stopping T3410");
    t3410.stop();
  }

  // Reset attach attempt counter according to 5.5.1.1
  if (attach_rej.emm_cause == LIBLTE_MME_EMM_CAUSE_PLMN_NOT_ALLOWED ||
      attach_rej.emm_cause == LIBLTE_MME_EMM_CAUSE_TRACKING_AREA_NOT_ALLOWED ||
      attach_rej.emm_cause == LIBLTE_MME_EMM_CAUSE_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA ||
      attach_rej.emm_cause == LIBLTE_MME_EMM_CAUSE_EPS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN ||
      attach_rej.emm_cause == LIBLTE_MME_EMM_CAUSE_NO_SUITABLE_CELLS_IN_TRACKING_AREA ||
      attach_rej.emm_cause == LIBLTE_MME_EMM_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG) {
    attach_attempt_counter = 0;
    enter_emm_deregistered(emm_state_t::deregistered_substate_t::attempting_to_attach);
  }

  // 5.5.1.2.5
  if (attach_rej.emm_cause == LIBLTE_MME_EMM_CAUSE_ILLEGAL_UE ||
      attach_rej.emm_cause == LIBLTE_MME_EMM_CAUSE_ILLEGAL_ME ||
      attach_rej.emm_cause == LIBLTE_MME_EMM_CAUSE_REQUESTED_SERVICE_OPTION_NOT_AUTHORIZED) {
    // delete security context
    reset_security_context();
    enter_emm_deregistered(emm_state_t::deregistered_substate_t::plmn_search);
    reattach_timer.run();
  }

  // TODO: handle other relevant reject causes

  if (attach_rej.emm_cause == LIBLTE_MME_EMM_CAUSE_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA ||
      attach_rej.emm_cause == LIBLTE_MME_EMM_CAUSE_EPS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN ||
      attach_rej.emm_cause == LIBLTE_MME_EMM_CAUSE_NETWORK_FAILURE) {
    enter_emm_deregistered(emm_state_t::deregistered_substate_t::plmn_search);
  }
}

void nas::parse_authentication_request(uint32_t lcid, unique_byte_buffer_t pdu, const uint8_t sec_hdr_type)
{
  LIBLTE_MME_AUTHENTICATION_REQUEST_MSG_STRUCT auth_req = {};

  logger.info("Received Authentication Request");
  liblte_mme_unpack_authentication_request_msg((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &auth_req);

  ctxt_base.rx_count++;

  // Generate authentication response using RAND, AUTN & KSI-ASME
  uint16 mcc, mnc;
  mcc = rrc->get_mcc();
  mnc = rrc->get_mnc();

  logger.info("MCC=%d, MNC=%d", mcc, mnc);

  uint8_t res[16];
  int     res_len = 0;
  logger.debug(auth_req.rand, 16, "Authentication request RAND");
  logger.debug(auth_req.autn, 16, "Authentication request AUTN");
  auth_result_t auth_result =
      usim->generate_authentication_response(auth_req.rand, auth_req.autn, mcc, mnc, res, &res_len, ctxt.k_asme);
  logger.debug(res, res_len, "Authentication request RES");
  if (LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE == auth_req.nas_ksi.tsc_flag) {
    ctxt.ksi = auth_req.nas_ksi.nas_ksi;
  } else {
    logger.error("NAS mapped security context not currently supported");
    srsran::console("Warning: NAS mapped security context not currently supported\n");
  }

  if (auth_result == AUTH_OK) {
    logger.info("Network authentication successful");
    // MME wants to re-establish security context, use provided protection level until security (re-)activation
    current_sec_hdr = sec_hdr_type;

    send_authentication_response(res, res_len);
    logger.info(ctxt.k_asme, 32, "Generated k_asme:");
    set_k_enb_count(0);
    auth_request = true;
  } else if (auth_result == AUTH_SYNCH_FAILURE) {
    logger.error("Network authentication synchronization failure.");
    send_authentication_failure(LIBLTE_MME_EMM_CAUSE_SYNCH_FAILURE, res);
  } else {
    logger.warning("Network authentication failure");
    srsran::console("Warning: Network authentication failure\n");
    send_authentication_failure(LIBLTE_MME_EMM_CAUSE_MAC_FAILURE, nullptr);
  }
}

void nas::parse_authentication_reject(uint32_t lcid, unique_byte_buffer_t pdu)
{
  logger.warning("Received Authentication Reject");
  reset_security_context();
  enter_emm_deregistered(emm_state_t::deregistered_substate_t::plmn_search);
  // TODO: Command RRC to release?
}

void nas::parse_identity_request(unique_byte_buffer_t pdu, const uint8_t sec_hdr_type)
{
  LIBLTE_MME_ID_REQUEST_MSG_STRUCT id_req = {};
  liblte_mme_unpack_identity_request_msg((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &id_req);

  logger.info("Received Identity Request. ID type: %d", id_req.id_type);
  ctxt_base.rx_count++;

  // do not respond if request is not protected (TS 24.301 Sec. 4.4.4.2)
  if (sec_hdr_type >= LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY ||
      (sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS && id_req.id_type == LIBLTE_MME_MOBILE_ID_TYPE_IMSI)) {
    current_sec_hdr = sec_hdr_type; // use MME protection level until security (re-)activation
    send_identity_response(id_req.id_type);
  } else {
    logger.info("Not sending identity response due to missing integrity protection.");
  }
}

void nas::parse_security_mode_command(uint32_t lcid, unique_byte_buffer_t pdu)
{
  if (!pdu) {
    logger.error("Invalid PDU");
    return;
  }

  if (pdu->N_bytes <= 5) {
    logger.error("Invalid security mode command PDU size (%d)", pdu->N_bytes);
    return;
  }

  LIBLTE_MME_SECURITY_MODE_COMMAND_MSG_STRUCT sec_mode_cmd = {};
  liblte_mme_unpack_security_mode_command_msg((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &sec_mode_cmd);
  logger.info("Received Security Mode Command ksi: %d, eea: %s, eia: %s",
              sec_mode_cmd.nas_ksi.nas_ksi,
              ciphering_algorithm_id_text[sec_mode_cmd.selected_nas_sec_algs.type_of_eea],
              integrity_algorithm_id_text[sec_mode_cmd.selected_nas_sec_algs.type_of_eia]);

  if (sec_mode_cmd.nas_ksi.tsc_flag != LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE) {
    logger.error("Mapped security context not supported");
    return;
  }

  if (have_ctxt) {
    if (sec_mode_cmd.nas_ksi.nas_ksi != ctxt.ksi) {
      logger.warning("Sending Security Mode Reject due to key set ID mismatch");
      send_security_mode_reject(LIBLTE_MME_EMM_CAUSE_SECURITY_MODE_REJECTED_UNSPECIFIED);
      return;
    }
  }

  // MME is setting up security context

  // TODO: check nonce (not sent by Amari)

  // Check capabilities replay
  if (!check_cap_replay(&sec_mode_cmd.ue_security_cap)) {
    logger.warning("Sending Security Mode Reject due to security capabilities replay mismatch");
    send_security_mode_reject(LIBLTE_MME_EMM_CAUSE_UE_SECURITY_CAPABILITIES_MISMATCH);
    return;
  }

  // Reset counters (as per 24.301 5.4.3.2), only needed for initial security mode command
  if (auth_request) {
    ctxt_base.rx_count = 0;
    ctxt_base.tx_count = 0;
    auth_request  = false;
  }

  ctxt_base.cipher_algo = (CIPHERING_ALGORITHM_ID_ENUM)sec_mode_cmd.selected_nas_sec_algs.type_of_eea;
  ctxt_base.integ_algo  = (INTEGRITY_ALGORITHM_ID_ENUM)sec_mode_cmd.selected_nas_sec_algs.type_of_eia;

  // Check capabilities
  if (!eea_caps[ctxt_base.cipher_algo] || !eia_caps[ctxt_base.integ_algo]) {
    logger.warning("Sending Security Mode Reject due to security capabilities mismatch");
    send_security_mode_reject(LIBLTE_MME_EMM_CAUSE_UE_SECURITY_CAPABILITIES_MISMATCH);
    return;
  }

  // Generate NAS keys
  usim->generate_nas_keys(
      ctxt.k_asme, ctxt_base.k_nas_enc, ctxt_base.k_nas_int, ctxt_base.cipher_algo, ctxt_base.integ_algo);
  logger.info(ctxt_base.k_nas_enc, 32, "NAS encryption key - k_nas_enc");
  logger.info(ctxt_base.k_nas_int, 32, "NAS integrity key - k_nas_int");

  logger.debug("Generating integrity check. integ_algo:%d, count_dl:%d, lcid:%d",
               ctxt_base.integ_algo,
               ctxt_base.rx_count,
               lcid);

  if (not integrity_check(pdu.get())) {
    logger.warning("Sending Security Mode Reject due to integrity check failure");
    send_security_mode_reject(LIBLTE_MME_EMM_CAUSE_MAC_FAILURE);
    return;
  }

  ctxt_base.rx_count++;

  LIBLTE_MME_SECURITY_MODE_COMPLETE_MSG_STRUCT sec_mode_comp = {};
  if (sec_mode_cmd.imeisv_req_present && LIBLTE_MME_IMEISV_REQUESTED == sec_mode_cmd.imeisv_req) {
    sec_mode_comp.imeisv_present    = true;
    sec_mode_comp.imeisv.type_of_id = LIBLTE_MME_MOBILE_ID_TYPE_IMEISV;
    usim->get_imei_vec(sec_mode_comp.imeisv.imeisv, 15);
    sec_mode_comp.imeisv.imeisv[14] = ue_svn_oct1;
    sec_mode_comp.imeisv.imeisv[15] = ue_svn_oct2;
  } else {
    sec_mode_comp.imeisv_present = false;
  }

  // Take security context into use and use new security header
  have_ctxt       = true;
  current_sec_hdr = LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED_WITH_NEW_EPS_SECURITY_CONTEXT;

  // Pack and send response
  pdu->clear();
  liblte_mme_pack_security_mode_complete_msg(
      &sec_mode_comp, current_sec_hdr, ctxt_base.tx_count, (LIBLTE_BYTE_MSG_STRUCT*)pdu.get());
  if (pcap != nullptr) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  if (apply_security_config(pdu, current_sec_hdr)) {
    logger.error("Error applying NAS security.");
    return;
  }

  logger.info(
      "Sending Security Mode Complete ctxt_base.tx_count=%d, RB=%s", ctxt_base.tx_count, rrc->get_rb_name(lcid));
  rrc->write_sdu(std::move(pdu));
  ctxt_base.tx_count++;

  // switch security header for all following messages
  current_sec_hdr = LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED;
}

void nas::parse_service_reject(uint32_t lcid, unique_byte_buffer_t pdu, const uint8_t sec_hdr_type)
{
  LIBLTE_MME_SERVICE_REJECT_MSG_STRUCT service_reject;
  if (liblte_mme_unpack_service_reject_msg((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &service_reject)) {
    logger.error("Error unpacking service reject.");
    return;
  }

  srsran::console("Received service reject with EMM cause=0x%x.\n", service_reject.emm_cause);

  // do not accept if the message is not protected when the EMM cause is #25 (TS 24.301 Sec. 4.4.4.2)
  if (sec_hdr_type == LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS &&
      service_reject.emm_cause == LIBLTE_MME_EMM_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG) {
    logger.error("Not handling NAS Service Reject message with EMM cause #25 without integrity protection!");
    return;
  }

  if (service_reject.t3446_present) {
    logger.info(
        "Received service reject with EMM cause=0x%x and t3446=%d", service_reject.emm_cause, service_reject.t3446);
  }

  // TODO: handle NAS backoff-timers correctly

  enter_emm_deregistered(emm_state_t::deregistered_substate_t::plmn_search);
  reset_security_context();

  // Send attach request after receiving service reject
  pdu->clear();
  gen_attach_request(pdu);
  rrc->write_sdu(std::move(pdu));
}

void nas::parse_esm_information_request(uint32_t lcid, unique_byte_buffer_t pdu)
{
  LIBLTE_MME_ESM_INFORMATION_REQUEST_MSG_STRUCT esm_info_req;
  liblte_mme_unpack_esm_information_request_msg((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &esm_info_req);

  logger.info("ESM information request received for beaser=%d, transaction_id=%d",
              esm_info_req.eps_bearer_id,
              esm_info_req.proc_transaction_id);
  ctxt_base.rx_count++;

  // send response
  send_esm_information_response(esm_info_req.proc_transaction_id);
}

void nas::parse_emm_information(uint32_t lcid, unique_byte_buffer_t pdu)
{
  LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT emm_info = {};
  liblte_mme_unpack_emm_information_msg((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &emm_info);
  std::string str = emm_info_str(&emm_info);
  logger.info("Received EMM Information: %s", str.c_str());
  srsran::console("%s\n", str.c_str());
  ctxt_base.rx_count++;
}

void nas::parse_detach_request(uint32_t lcid, unique_byte_buffer_t pdu)
{
  LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT detach_request;
  liblte_mme_unpack_detach_request_msg((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &detach_request);
  ctxt_base.rx_count++;

  logger.info("Received detach request (type=%d). NAS State: %s",
              detach_request.detach_type.type_of_detach,
              state.get_full_state_text().c_str());

  switch (state.get_state()) {
    case emm_state_t::state_t::service_request_initiated: // intentional fall-through to complete detach procedure
    case emm_state_t::state_t::deregistered_initiated:    // intentional fall-through to complete detach procedure
    case emm_state_t::state_t::registered:
      // send accept and leave state
      send_detach_accept();

      // TODO: add parsing and correct handling of EMM cause for detach (Sec. 5.5.2.3.2)
      enter_emm_deregistered(emm_state_t::deregistered_substate_t::null);

      // schedule reattach if required
      if (detach_request.detach_type.type_of_detach == LIBLTE_MME_TOD_DL_REATTACH_REQUIRED) {
        // Section 5.5.2.3.2
        // delay re-attach to allow RRC to release
        logger.debug("Starting reattach timer");
        reattach_timer.run();
      }
      break;
    default:
      logger.warning("Received detach request in invalid state (%s)", state.get_full_state_text().c_str());
      break;
  }
}

void nas::parse_activate_dedicated_eps_bearer_context_request(uint32_t lcid, unique_byte_buffer_t pdu)
{
  LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT request;
  liblte_mme_unpack_activate_dedicated_eps_bearer_context_request_msg((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &request);

  logger.info(
      "Received Activate Dedicated EPS bearer context request (eps_bearer_id=%d, linked_bearer_id=%d, proc_id=%d)",
      request.eps_bearer_id,
      request.linked_eps_bearer_id,
      request.proc_transaction_id);

  ctxt_base.rx_count++;
  LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft = &request.tft;
  logger.info("Traffic Flow Template: TFT OP code 0x%x, Filter list size %d, Parameter list size %d",
              tft->tft_op_code,
              tft->packet_filter_list_size,
              tft->parameter_list_size);

  // check the a linked default bearer exists
  if (eps_bearer.find(request.linked_eps_bearer_id) == eps_bearer.end()) {
    logger.error("No linked default EPS bearer found (%d).", request.linked_eps_bearer_id);
    // TODO: send reject according to 24.301 Sec 6.4.2.5 paragraph c
    return;
  }

  // check if the dedicated EPS bearer already exists
  if (eps_bearer.find(request.eps_bearer_id) != eps_bearer.end()) {
    // according to 24.301 Sec 6.4.2.5 paragraph b) the existing bearer shall be deactived before proceeding
    logger.error("EPS bearer already exists (%d). Removing it.", request.eps_bearer_id);

    // remove bearer
    eps_bearer_map_t::iterator it = eps_bearer.find(request.eps_bearer_id);
    eps_bearer.erase(it);
  }

  // create new bearer
  eps_bearer_t bearer         = {};
  bearer.type                 = DEDICATED_EPS_BEARER;
  bearer.eps_bearer_id        = request.eps_bearer_id;
  bearer.linked_eps_bearer_id = request.linked_eps_bearer_id;
  if (not eps_bearer.insert(eps_bearer_map_pair_t(bearer.eps_bearer_id, bearer)).second) {
    logger.error("Error adding EPS bearer.");
    return;
  }

  // apply packet filters to GW
  gw->apply_traffic_flow_template(request.eps_bearer_id, tft);

  send_activate_dedicated_eps_bearer_context_accept(request.proc_transaction_id, request.eps_bearer_id);
}

void nas::parse_deactivate_eps_bearer_context_request(unique_byte_buffer_t pdu)
{
  LIBLTE_MME_DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT request;

  liblte_mme_unpack_deactivate_eps_bearer_context_request_msg((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &request);

  logger.info("Received Deactivate EPS bearer context request (eps_bearer_id=%d, proc_id=%d, cause=0x%X)",
              request.eps_bearer_id,
              request.proc_transaction_id,
              request.esm_cause);

  ctxt_base.rx_count++;

  // check if bearer exists
  if (eps_bearer.find(request.eps_bearer_id) == eps_bearer.end()) {
    logger.error("EPS bearer doesn't exist (eps_bearer_id=%d)", request.eps_bearer_id);
    // fixme: send proper response
    return;
  }

  // remove bearer
  eps_bearer_map_t::iterator it = eps_bearer.find(request.eps_bearer_id);
  eps_bearer.erase(it);

  // inform GW about removed EPS bearer
  gw->deactivate_eps_bearer(request.eps_bearer_id);

  logger.info("Removed EPS bearer context (eps_bearer_id=%d)", request.eps_bearer_id);

  send_deactivate_eps_bearer_context_accept(request.proc_transaction_id, request.eps_bearer_id);
}

void nas::parse_modify_eps_bearer_context_request(srsran::unique_byte_buffer_t pdu)
{
  LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT request;

  liblte_mme_unpack_modify_eps_bearer_context_request_msg((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &request);

  logger.info("Received Modify EPS bearer context request (eps_bearer_id=%d, proc_id=%d)",
              request.eps_bearer_id,
              request.proc_transaction_id);

  ctxt_base.rx_count++;

  // check if bearer exists
  const auto it = eps_bearer.find(request.eps_bearer_id);
  if (it == eps_bearer.end()) {
    logger.error("EPS bearer doesn't exist (eps_bearer_id=%d)", request.eps_bearer_id);
    // fixme: send proper response
    return;
  }

  LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft = &request.tft;
  logger.info("Traffic Flow Template: TFT OP code 0x%x, Filter list size %d, Parameter list size %d",
              tft->tft_op_code,
              tft->packet_filter_list_size,
              tft->parameter_list_size);

  // modify/apply packet filters to GW
  if (gw->apply_traffic_flow_template(request.eps_bearer_id, tft) != SRSRAN_SUCCESS) {
    logger.error("Couldn't modify TFT");
    return;
  }

  logger.info("Modified EPS bearer context (eps_bearer_id=%d)", request.eps_bearer_id);

  send_modify_eps_bearer_context_accept(request.proc_transaction_id, request.eps_bearer_id);
}

void nas::parse_activate_test_mode(uint32_t lcid, unique_byte_buffer_t pdu)
{
  logger.info("Received Activate test mode");

  ctxt_base.rx_count++;

  send_activate_test_mode_complete();
}

void nas::parse_close_ue_test_loop(uint32_t lcid, unique_byte_buffer_t pdu)
{
  LIBLTE_MME_UE_TEST_LOOP_MODE_ENUM mode = static_cast<LIBLTE_MME_UE_TEST_LOOP_MODE_ENUM>(pdu->msg[8]);
  logger.info("Received Close UE test loop for %s", liblte_ue_test_loop_mode_text[mode]);
  switch (mode) {
    case LIBLTE_MME_UE_TEST_LOOP_MODE_A:
      gw->set_test_loop_mode(gw_interface_nas::TEST_LOOP_MODE_A_ACTIVE);
      break;
    case LIBLTE_MME_UE_TEST_LOOP_MODE_B:
      gw->set_test_loop_mode(gw_interface_nas::TEST_LOOP_MODE_B_ACTIVE, pdu->msg[9] * 1000);
      break;
    case LIBLTE_MME_UE_TEST_LOOP_MODE_C:
      gw->set_test_loop_mode(gw_interface_nas::TEST_LOOP_MODE_C_ACTIVE);
      break;
    default:
      break;
  }

  ctxt_base.rx_count++;

  send_close_ue_test_loop_complete();
}

void nas::parse_emm_status(uint32_t lcid, unique_byte_buffer_t pdu)
{
  LIBLTE_MME_EMM_STATUS_MSG_STRUCT emm_status;
  liblte_mme_unpack_emm_status_msg((LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), &emm_status);
  ctxt_base.rx_count++;

  switch (emm_status.emm_cause) {
    case LIBLTE_MME_ESM_CAUSE_INVALID_EPS_BEARER_IDENTITY:
      logger.info("Received EMM status: Invalid EPS bearer identity");
      // TODO: abort any ongoing procedure (see Sec. 6.7 in TS 24.301)
      break;
    case LIBLTE_MME_ESM_CAUSE_INVALID_PTI_VALUE:
      logger.info("Received EMM status: Invalid PTI value");
      // TODO: abort any ongoing procedure (see Sec. 6.7 in TS 24.301)
      break;
    case LIBLTE_MME_ESM_CAUSE_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED:
      logger.info("Received EMM status: Invalid PTI value");
      // TODO: see Sec. 6.7 in TS 24.301
      break;
    default:
      logger.info("Received unknown EMM status (cause=%d)", emm_status.emm_cause);
      break;
  }
}

/*******************************************************************************
 * Senders
 ******************************************************************************/

void nas::gen_attach_request(srsran::unique_byte_buffer_t& msg)
{
  if (msg == nullptr) {
    logger.error("Fatal Error: Couldn't allocate PDU in gen_attach_request().");
    return;
  }
  LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT attach_req;
  bzero(&attach_req, sizeof(LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT));

  logger.info("Generating attach request");

  attach_req.eps_attach_type = LIBLTE_MME_EPS_ATTACH_TYPE_EPS_ATTACH;

  for (u_int32_t i = 0; i < 8; i++) {
    attach_req.ue_network_cap.eea[i] = eea_caps[i];
    attach_req.ue_network_cap.eia[i] = eia_caps[i];
  }

  attach_req.ue_network_cap.uea_present                     = false; // UMTS encryption algos
  attach_req.ue_network_cap.uia_present                     = false; // UMTS integrity algos
  attach_req.ue_network_cap.ucs2_present                    = false;
  attach_req.ms_network_cap_present                         = false; // A/Gb mode (2G) or Iu mode (3G)
  attach_req.ue_network_cap.lpp_present                     = false;
  attach_req.ue_network_cap.lcs_present                     = false;
  attach_req.ue_network_cap.onexsrvcc_present               = false;
  attach_req.ue_network_cap.nf_present                      = false;
  attach_req.old_p_tmsi_signature_present                   = false;
  attach_req.additional_guti_present                        = false;
  attach_req.last_visited_registered_tai_present            = false;
  attach_req.drx_param_present                              = false;
  attach_req.old_lai_present                                = false;
  attach_req.tmsi_status_present                            = false;
  attach_req.ms_cm2_present                                 = false;
  attach_req.ms_cm3_present                                 = false;
  attach_req.supported_codecs_present                       = false;
  attach_req.additional_update_type_present                 = false;
  attach_req.voice_domain_pref_and_ue_usage_setting_present = false;
  attach_req.device_properties_present                      = false;
  attach_req.old_guti_type_present                          = false;

  if (rrc->has_nr_dc()) {
    attach_req.ue_network_cap.dc_nr_present    = true;
    attach_req.ue_network_cap.dc_nr            = true;
    attach_req.additional_security_cap_present = true;
  }

  // ESM message (PDN connectivity request) for first default bearer
  gen_pdn_connectivity_request(&attach_req.esm_msg);

  // GUTI or IMSI attach
  if (have_guti && have_ctxt) {
    attach_req.tmsi_status_present      = true;
    attach_req.tmsi_status              = LIBLTE_MME_TMSI_STATUS_VALID_TMSI;
    attach_req.eps_mobile_id.type_of_id = LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI;
    memcpy(&attach_req.eps_mobile_id.guti, &ctxt.guti, sizeof(LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT));
    attach_req.old_guti_type         = LIBLTE_MME_GUTI_TYPE_NATIVE;
    attach_req.old_guti_type_present = true;
    attach_req.nas_ksi.tsc_flag      = LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
    attach_req.nas_ksi.nas_ksi       = ctxt.ksi;
    logger.info("Requesting GUTI attach. "
                "m_tmsi: %x, mcc: %x, mnc: %x, mme_group_id: %x, mme_code: %x",
                ctxt.guti.m_tmsi,
                ctxt.guti.mcc,
                ctxt.guti.mnc,
                ctxt.guti.mme_group_id,
                ctxt.guti.mme_code);

    // According to Sec 4.4.5, the attach request is always unciphered, even if a context exists
    liblte_mme_pack_attach_request_msg(
        &attach_req, LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY, ctxt_base.tx_count, (LIBLTE_BYTE_MSG_STRUCT*)msg.get());

    if (apply_security_config(msg, LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY)) {
      logger.error("Error applying NAS security.");
      return;
    }
  } else {
    attach_req.eps_mobile_id.type_of_id = LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI;
    attach_req.nas_ksi.tsc_flag         = LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
    attach_req.nas_ksi.nas_ksi          = LIBLTE_MME_NAS_KEY_SET_IDENTIFIER_NO_KEY_AVAILABLE;
    usim->get_imsi_vec(attach_req.eps_mobile_id.imsi, 15);
    logger.info("Requesting IMSI attach (IMSI=%s)", usim->get_imsi_str().c_str());
    liblte_mme_pack_attach_request_msg(&attach_req, (LIBLTE_BYTE_MSG_STRUCT*)msg.get());
  }

  if (pcap != nullptr) {
    pcap->write_nas(msg->msg, msg->N_bytes);
  }

  if (have_ctxt) {
    set_k_enb_count(ctxt_base.tx_count);
    ctxt_base.tx_count++;
  }

  // stop T3411 and T3402
  if (t3411.is_running()) {
    logger.debug("Stopping T3411");
    t3411.stop();
  }

  if (t3402.is_running()) {
    logger.debug("Stopping T3402");
    t3402.stop();
  }

  // start T3410
  logger.debug("Starting T3410. Timeout in %d ms.", t3410.duration());
  t3410.run();
}

void nas::gen_service_request(srsran::unique_byte_buffer_t& msg)
{
  if (msg == nullptr) {
    logger.error("Fatal Error: Couldn't allocate PDU in gen_service_request().");
    return;
  }

  logger.info("Generating service request");

  // Pack the service request message directly
  msg->msg[0] = (LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST << 4u) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
  msg->N_bytes++;
  msg->msg[1] = (ctxt.ksi & 0x07u) << 5u;
  msg->msg[1] |= ctxt_base.tx_count & 0x1Fu;
  msg->N_bytes++;

  uint8_t mac[4];
  integrity_generate(&ctxt_base.k_nas_int[16], ctxt_base.tx_count, SECURITY_DIRECTION_UPLINK, &msg->msg[0], 2, &mac[0]);
  // Set the short MAC
  msg->msg[2] = mac[2];
  msg->N_bytes++;
  msg->msg[3] = mac[3];
  msg->N_bytes++;

  if (pcap != nullptr) {
    pcap->write_nas(msg->msg, msg->N_bytes);
  }
  set_k_enb_count(ctxt_base.tx_count);
  ctxt_base.tx_count++;
}

void nas::gen_pdn_connectivity_request(LIBLTE_BYTE_MSG_STRUCT* msg)
{
  LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT pdn_con_req = {};

  logger.info("Generating PDN Connectivity Request");

  // Set the PDN con req parameters
  pdn_con_req.eps_bearer_id       = 0x00; // Unassigned bearer ID
  pdn_con_req.proc_transaction_id = 0x01; // First transaction ID
  pdn_con_req.request_type        = LIBLTE_MME_REQUEST_TYPE_INITIAL_REQUEST;
  pdn_con_req.apn_present         = false;

  // Set PDN protocol type
  if (cfg.apn_protocol == "ipv4" || cfg.apn_protocol.empty()) {
    logger.debug("Requesting IPv4 PDN protocol");
    pdn_con_req.pdn_type = LIBLTE_MME_PDN_TYPE_IPV4;
  } else if (cfg.apn_protocol == "ipv6") {
    logger.debug("Requesting IPv6 PDN protocol");
    pdn_con_req.pdn_type = LIBLTE_MME_PDN_TYPE_IPV6;
  } else if (cfg.apn_protocol == "ipv4v6") {
    logger.debug("Requesting IPv4v6 PDN protocol");
    pdn_con_req.pdn_type = LIBLTE_MME_PDN_TYPE_IPV4V6;
  } else {
    logger.warning("Unsupported PDN prtocol. Defaulting to IPv4");
    srsran::console("Unsupported PDN prtocol: %s. Defaulting to IPv4\n", cfg.apn_protocol.c_str());
    pdn_con_req.pdn_type = LIBLTE_MME_PDN_TYPE_IPV4;
  }

  // Set the optional flags
  if (cfg.apn_name.empty()) {
    pdn_con_req.esm_info_transfer_flag_present = false;
  } else {
    // request ESM info transfer is APN is specified
    pdn_con_req.esm_info_transfer_flag_present = true;
    pdn_con_req.esm_info_transfer_flag         = LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_REQUIRED;
  }

  pdn_con_req.protocol_cnfg_opts_present = false;
  pdn_con_req.device_properties_present  = false;

  // Pack the message
  liblte_mme_pack_pdn_connectivity_request_msg(&pdn_con_req, msg);
}

void nas::send_security_mode_reject(uint8_t cause)
{
  unique_byte_buffer_t msg = srsran::make_byte_buffer();
  if (!msg) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  LIBLTE_MME_SECURITY_MODE_REJECT_MSG_STRUCT sec_mode_rej = {0};
  sec_mode_rej.emm_cause                                  = cause;
  liblte_mme_pack_security_mode_reject_msg(&sec_mode_rej, (LIBLTE_BYTE_MSG_STRUCT*)msg.get());
  if (pcap != nullptr) {
    pcap->write_nas(msg->msg, msg->N_bytes);
  }
  logger.info("Sending security mode reject");
  rrc->write_sdu(std::move(msg));
}

/**
 * Pack attach request message and send to RRC for transmission.
 */
void nas::send_attach_request()
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  gen_attach_request(pdu);
  rrc->write_sdu(std::move(pdu));
}

void nas::send_detach_request(bool switch_off)
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT detach_request = {};
  if (switch_off) {
    detach_request.detach_type.switch_off     = 1;
    detach_request.detach_type.type_of_detach = LIBLTE_MME_SO_FLAG_SWITCH_OFF;
  } else {
    detach_request.detach_type.switch_off     = 0;
    detach_request.detach_type.type_of_detach = LIBLTE_MME_TOD_UL_EPS_DETACH;
  }

  // GUTI or IMSI detach
  if (have_guti && have_ctxt) {
    detach_request.eps_mobile_id.type_of_id = LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI;
    memcpy(&detach_request.eps_mobile_id.guti, &ctxt.guti, sizeof(LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT));
    detach_request.nas_ksi.tsc_flag = LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
    detach_request.nas_ksi.nas_ksi  = ctxt.ksi;
    logger.info("Sending detach request with GUTI"); // If sent as an Initial UE message, it cannot be ciphered
    liblte_mme_pack_detach_request_msg(&detach_request,
                                       LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY,
                                       ctxt_base.tx_count,
                                       (LIBLTE_BYTE_MSG_STRUCT*)pdu.get());

    if (pcap != nullptr) {
      pcap->write_nas(pdu->msg, pdu->N_bytes);
    }

    if (apply_security_config(pdu, LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY)) {
      logger.error("Error applying NAS security.");
      return;
    }
  } else {
    detach_request.eps_mobile_id.type_of_id = LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI;
    detach_request.nas_ksi.tsc_flag         = LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
    detach_request.nas_ksi.nas_ksi          = 0;
    usim->get_imsi_vec(detach_request.eps_mobile_id.imsi, 15);
    logger.info("Sending detach request with IMSI");
    liblte_mme_pack_detach_request_msg(
        &detach_request, current_sec_hdr, ctxt_base.tx_count, (LIBLTE_BYTE_MSG_STRUCT*)pdu.get());

    if (pcap != nullptr) {
      pcap->write_nas(pdu->msg, pdu->N_bytes);
    }
  }

  if (switch_off) {
    enter_emm_deregistered(emm_state_t::deregistered_substate_t::null);
  } else {
    // we are expecting a response from the core
    state.set_deregistered_initiated();

    // start T3421
    logger.info("Starting T3421");
    t3421.run();
  }

  if (rrc->is_connected()) {
    rrc->write_sdu(std::move(pdu));
  } else {
    if (not rrc->connection_request(srsran::establishment_cause_t::mo_sig, std::move(pdu))) {
      logger.error("Error starting RRC connection");
    }
  }

  ctxt_base.tx_count++;
}

void nas::send_attach_complete(const uint8_t& transaction_id_, const uint8_t& eps_bearer_id)
{
  // Send EPS bearer context accept and attach complete
  LIBLTE_MME_ATTACH_COMPLETE_MSG_STRUCT                            attach_complete                   = {};
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT act_def_eps_bearer_context_accept = {};
  act_def_eps_bearer_context_accept.eps_bearer_id                                                    = eps_bearer_id;
  act_def_eps_bearer_context_accept.proc_transaction_id                                              = transaction_id_;
  act_def_eps_bearer_context_accept.protocol_cnfg_opts_present                                       = false;
  liblte_mme_pack_activate_default_eps_bearer_context_accept_msg(&act_def_eps_bearer_context_accept,
                                                                 &attach_complete.esm_msg);

  // Pack entire message
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }
  liblte_mme_pack_attach_complete_msg(
      &attach_complete, current_sec_hdr, ctxt_base.tx_count, (LIBLTE_BYTE_MSG_STRUCT*)pdu.get());
  // Write NAS pcap
  if (pcap != nullptr) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  if (apply_security_config(pdu, current_sec_hdr)) {
    logger.error("Error applying NAS security.");
    return;
  }

  // Instruct RRC to enable capabilities
  rrc->enable_capabilities();

  logger.info("Sending Attach Complete");
  rrc->write_sdu(std::move(pdu));
  ctxt_base.tx_count++;
}

void nas::send_detach_accept()
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  LIBLTE_MME_DETACH_ACCEPT_MSG_STRUCT detach_accept;
  bzero(&detach_accept, sizeof(detach_accept));
  liblte_mme_pack_detach_accept_msg(
      &detach_accept, current_sec_hdr, ctxt_base.tx_count, (LIBLTE_BYTE_MSG_STRUCT*)pdu.get());

  if (pcap != nullptr) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  if (apply_security_config(pdu, current_sec_hdr)) {
    logger.error("Error applying NAS security.");
    return;
  }

  logger.info("Sending detach accept");
  rrc->write_sdu(std::move(pdu));
}

void nas::send_authentication_response(const uint8_t* res, const size_t res_len)
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT auth_res = {};

  for (uint32_t i = 0; i < res_len; i++) {
    auth_res.res[i] = res[i];
  }
  auth_res.res_len = res_len;
  liblte_mme_pack_authentication_response_msg(
      &auth_res, current_sec_hdr, ctxt_base.tx_count, (LIBLTE_BYTE_MSG_STRUCT*)pdu.get());

  if (pcap != nullptr) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  if (apply_security_config(pdu, current_sec_hdr)) {
    logger.error("Error applying NAS security.");
    return;
  }

  logger.info("Sending Authentication Response");
  rrc->write_sdu(std::move(pdu));
  ctxt_base.tx_count++;
}

void nas::send_authentication_failure(const uint8_t cause, const uint8_t* auth_fail_param)
{
  unique_byte_buffer_t msg = srsran::make_byte_buffer();
  if (!msg) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  LIBLTE_MME_AUTHENTICATION_FAILURE_MSG_STRUCT auth_failure;
  auth_failure.emm_cause = cause;
  if (auth_fail_param != nullptr) {
    memcpy(auth_failure.auth_fail_param, auth_fail_param, 14);
    logger.debug(auth_failure.auth_fail_param, 14, "auth_failure.auth_fail_param");
    auth_failure.auth_fail_param_present = true;
  } else {
    auth_failure.auth_fail_param_present = false;
  }

  liblte_mme_pack_authentication_failure_msg(&auth_failure, (LIBLTE_BYTE_MSG_STRUCT*)msg.get());
  if (pcap != nullptr) {
    pcap->write_nas(msg->msg, msg->N_bytes);
  }
  logger.info("Sending authentication failure.");
  rrc->write_sdu(std::move(msg));
}

void nas::send_identity_response(const uint8 id_type)
{
  LIBLTE_MME_ID_RESPONSE_MSG_STRUCT id_resp = {};

  switch (id_type) {
    case LIBLTE_MME_MOBILE_ID_TYPE_IMSI:
      id_resp.mobile_id.type_of_id = LIBLTE_MME_MOBILE_ID_TYPE_IMSI;
      usim->get_imsi_vec(id_resp.mobile_id.imsi, 15);
      break;
    case LIBLTE_MME_MOBILE_ID_TYPE_IMEI:
      id_resp.mobile_id.type_of_id = LIBLTE_MME_MOBILE_ID_TYPE_IMEI;
      usim->get_imei_vec(id_resp.mobile_id.imei, 15);
      break;
    case LIBLTE_MME_MOBILE_ID_TYPE_IMEISV:
      id_resp.mobile_id.type_of_id = LIBLTE_MME_MOBILE_ID_TYPE_IMEISV;
      usim->get_imei_vec(id_resp.mobile_id.imeisv, 15);
      id_resp.mobile_id.imeisv[14] = ue_svn_oct1;
      id_resp.mobile_id.imeisv[15] = ue_svn_oct2;
      break;
    default:
      logger.error("Unhandled ID type: %d", id_type);
      return;
  }

  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  liblte_mme_pack_identity_response_msg(
      &id_resp, current_sec_hdr, ctxt_base.tx_count, (LIBLTE_BYTE_MSG_STRUCT*)pdu.get());

  // add security if needed
  if (apply_security_config(pdu, current_sec_hdr)) {
    logger.error("Error applying NAS security.");
    return;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  rrc->write_sdu(std::move(pdu));
  ctxt_base.tx_count++;
}

void nas::send_service_request()
{
  unique_byte_buffer_t msg = srsran::make_byte_buffer();
  if (!msg) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  // Pack the service request message directly
  msg->msg[0] = (LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST << 4u) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
  msg->N_bytes++;
  msg->msg[1] = (ctxt.ksi & 0x07u) << 5u;
  msg->msg[1] |= ctxt_base.tx_count & 0x1Fu;
  msg->N_bytes++;

  uint8_t mac[4];
  integrity_generate(&ctxt_base.k_nas_int[16], ctxt_base.tx_count, SECURITY_DIRECTION_UPLINK, &msg->msg[0], 2, &mac[0]);
  // Set the short MAC
  msg->msg[2] = mac[2];
  msg->N_bytes++;
  msg->msg[3] = mac[3];
  msg->N_bytes++;

  if (pcap != nullptr) {
    pcap->write_nas(msg->msg, msg->N_bytes);
  }

  logger.info("Sending service request");
  rrc->write_sdu(std::move(msg));
  ctxt_base.tx_count++;
}

void nas::send_esm_information_response(const uint8 proc_transaction_id)
{
  LIBLTE_MME_ESM_INFORMATION_RESPONSE_MSG_STRUCT esm_info_resp;
  esm_info_resp.proc_transaction_id = proc_transaction_id;
  esm_info_resp.eps_bearer_id       = 0; // respone shall always have no bearer assigned

  if (cfg.apn_name == "") {
    esm_info_resp.apn_present = false;
  } else {
    logger.debug("Including APN %s in ESM info response", cfg.apn_name.c_str());
    esm_info_resp.apn_present = true;
    int len                   = std::min((int)cfg.apn_name.length(), LIBLTE_STRING_LEN - 1);
    strncpy(esm_info_resp.apn.apn, cfg.apn_name.c_str(), len);
    esm_info_resp.apn.apn[len] = '\0';
  }

  if (cfg.apn_user != "" && cfg.apn_user.length() < LIBLTE_STRING_LEN && cfg.apn_pass != "" &&
      cfg.apn_pass.length() < LIBLTE_STRING_LEN) {
    logger.debug("Including CHAP authentication for user %s in ESM info response", cfg.apn_user.c_str());

    // Generate CHAP challenge
    uint16_t len = 1 /* CHAP code */ + 1 /* ID */ + 2 /* complete length */ + 1 /* data value size */ +
                   16 /* data value */ + cfg.apn_user.length();

    uint8_t challenge[len];
    bzero(challenge, len * sizeof(uint8_t));
    challenge[0] = 0x01;    // challenge code
    challenge[1] = chap_id; // ID
    challenge[2] = (len >> 8u) & 0xffu;
    challenge[3] = len & 0xffu;
    challenge[4] = 16;

    // Append random challenge value
    for (int i = 0; i < 16; i++) {
      challenge[5 + i] = rand() & 0xFFu;
    }

    // add user as name field
    for (size_t i = 0; i < cfg.apn_user.length(); i++) {
      const char* name  = cfg.apn_user.c_str();
      challenge[21 + i] = name[i];
    }

    // Generate response
    uint8_t response[len];
    bzero(response, len * sizeof(uint8_t));
    response[0] = 0x02; // response code
    response[1] = chap_id;
    response[2] = (len >> 8u) & 0xffu;
    response[3] = len & 0xffu;
    response[4] = 16;

    // Generate response value
    uint16_t resp_val_len = 16 /* MD5 len */ + 1 /* ID */ + cfg.apn_pass.length();
    uint8_t  resp_val[resp_val_len];
    resp_val[0] = chap_id;

    // add secret
    for (size_t i = 0; i < cfg.apn_pass.length(); i++) {
      const char* pass = cfg.apn_pass.c_str();
      resp_val[1 + i]  = pass[i];
    }

    // copy original challenge behind secret
    uint8_t* chal_val = &challenge[5];
    memcpy(&resp_val[1 + cfg.apn_pass.length()], chal_val, 16);

    // Compute MD5 of resp_val and add to response
    security_md5(resp_val, resp_val_len, &response[5]);

    // add user as name field again
    for (size_t i = 0; i < cfg.apn_user.length(); i++) {
      const char* name = cfg.apn_user.c_str();
      response[21 + i] = name[i];
    }

    // Add challenge and response to ESM info response
    esm_info_resp.protocol_cnfg_opts_present   = true;
    esm_info_resp.protocol_cnfg_opts.opt[0].id = LIBLTE_MME_CONFIGURATION_PROTOCOL_OPTIONS_CHAP;
    memcpy(esm_info_resp.protocol_cnfg_opts.opt[0].contents, challenge, sizeof(challenge));
    esm_info_resp.protocol_cnfg_opts.opt[0].len = sizeof(challenge);

    esm_info_resp.protocol_cnfg_opts.opt[1].id = LIBLTE_MME_CONFIGURATION_PROTOCOL_OPTIONS_CHAP;
    memcpy(esm_info_resp.protocol_cnfg_opts.opt[1].contents, response, sizeof(response));
    esm_info_resp.protocol_cnfg_opts.opt[1].len = sizeof(response);
    esm_info_resp.protocol_cnfg_opts.N_opts     = 2;
  } else {
    esm_info_resp.protocol_cnfg_opts_present = false;
  }

  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  if (liblte_mme_pack_esm_information_response_msg(
          &esm_info_resp, current_sec_hdr, ctxt_base.tx_count, (LIBLTE_BYTE_MSG_STRUCT*)pdu.get()) != LIBLTE_SUCCESS) {
    logger.error("Error packing ESM information response.");
    return;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  if (apply_security_config(pdu, current_sec_hdr)) {
    logger.error("Error applying NAS security.");
    return;
  }

  logger.info(pdu->msg, pdu->N_bytes, "Sending ESM information response");
  rrc->write_sdu(std::move(pdu));

  ctxt_base.tx_count++;
  chap_id++;
}

void nas::send_activate_dedicated_eps_bearer_context_accept(const uint8_t& proc_transaction_id,
                                                            const uint8_t& eps_bearer_id)
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT accept = {};

  accept.eps_bearer_id       = eps_bearer_id;
  accept.proc_transaction_id = proc_transaction_id;

  if (liblte_mme_pack_activate_dedicated_eps_bearer_context_accept_msg(
          &accept, current_sec_hdr, ctxt_base.tx_count, (LIBLTE_BYTE_MSG_STRUCT*)pdu.get()) != LIBLTE_SUCCESS) {
    logger.error("Error packing Activate Dedicated EPS Bearer context accept.");
    return;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  if (apply_security_config(pdu, current_sec_hdr)) {
    logger.error("Error applying NAS security.");
    return;
  }

  logger.info(pdu->msg,
              pdu->N_bytes,
              "Sending Activate Dedicated EPS Bearer context accept (eps_bearer_id=%d, proc_id=%d)",
              accept.eps_bearer_id,
              accept.proc_transaction_id);
  rrc->write_sdu(std::move(pdu));

  ctxt_base.tx_count++;
}

void nas::send_deactivate_eps_bearer_context_accept(const uint8_t& proc_transaction_id, const uint8_t& eps_bearer_id)
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  LIBLTE_MME_DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT accept = {};

  accept.eps_bearer_id       = eps_bearer_id;
  accept.proc_transaction_id = proc_transaction_id;

  if (liblte_mme_pack_deactivate_eps_bearer_context_accept_msg(
          &accept, current_sec_hdr, ctxt_base.tx_count, (LIBLTE_BYTE_MSG_STRUCT*)pdu.get()) != LIBLTE_SUCCESS) {
    logger.error("Error packing Activate EPS Bearer context accept.");
    return;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  if (apply_security_config(pdu, current_sec_hdr)) {
    logger.error("Error applying NAS security.");
    return;
  }

  logger.info(pdu->msg,
              pdu->N_bytes,
              "Sending Deactivate EPS Bearer context accept (eps_bearer_id=%d, proc_id=%d)",
              accept.eps_bearer_id,
              accept.proc_transaction_id);
  rrc->write_sdu(std::move(pdu));

  ctxt_base.tx_count++;
}

void nas::send_modify_eps_bearer_context_accept(const uint8_t& proc_transaction_id, const uint8_t& eps_bearer_id)
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT accept = {};

  accept.eps_bearer_id       = eps_bearer_id;
  accept.proc_transaction_id = proc_transaction_id;

  if (liblte_mme_pack_modify_eps_bearer_context_accept_msg(
          &accept, current_sec_hdr, ctxt_base.tx_count, (LIBLTE_BYTE_MSG_STRUCT*)pdu.get()) != LIBLTE_SUCCESS) {
    logger.error("Error packing Modify EPS Bearer context accept.");
    return;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  if (apply_security_config(pdu, current_sec_hdr)) {
    logger.error("Error applying NAS security.");
    return;
  }

  logger.info(pdu->msg,
              pdu->N_bytes,
              "Sending Modify EPS Bearer context accept (eps_bearer_id=%d, proc_id=%d)",
              accept.eps_bearer_id,
              accept.proc_transaction_id);
  rrc->write_sdu(std::move(pdu));

  ctxt_base.tx_count++;
}

void nas::send_activate_test_mode_complete()
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  if (liblte_mme_pack_activate_test_mode_complete_msg(
          (LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), current_sec_hdr, ctxt_base.tx_count)) {
    logger.error("Error packing activate test mode complete.");
    return;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  if (apply_security_config(pdu, current_sec_hdr)) {
    logger.error("Error applying NAS security.");
    return;
  }

  logger.info(pdu->msg, pdu->N_bytes, "Sending Activate test mode complete");
  rrc->write_sdu(std::move(pdu));

  ctxt_base.tx_count++;
}

void nas::send_close_ue_test_loop_complete()
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  if (liblte_mme_pack_close_ue_test_loop_complete_msg(
          (LIBLTE_BYTE_MSG_STRUCT*)pdu.get(), current_sec_hdr, ctxt_base.tx_count)) {
    logger.error("Error packing close UE test loop complete.");
    return;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu->msg, pdu->N_bytes);
  }

  if (apply_security_config(pdu, current_sec_hdr)) {
    logger.error("Error applying NAS security.");
    return;
  }

  logger.info(pdu->msg, pdu->N_bytes, "Sending Close UE test loop complete");
  rrc->write_sdu(std::move(pdu));

  ctxt_base.tx_count++;
}

/*******************************************************************************
 * Security context persistence file
 ******************************************************************************/

bool nas::read_ctxt_file(nas_sec_ctxt* ctxt_, nas_sec_base_ctxt* ctxt_base_)
{
  std::ifstream file;
  if (ctxt_ == nullptr || ctxt_base_ == nullptr) {
    return false;
  }

  if (cfg.force_imsi_attach) {
    logger.info("Skip reading context from file.");
    return false;
  }

  file.open(".ctxt", std::ios::in);
  if (file.is_open()) {
    if (!readvar(file, "m_tmsi=", &ctxt_->guti.m_tmsi)) {
      return false;
    }
    if (!readvar(file, "mcc=", &ctxt_->guti.mcc)) {
      return false;
    }
    if (!readvar(file, "mnc=", &ctxt_->guti.mnc)) {
      return false;
    }
    if (!readvar(file, "mme_group_id=", &ctxt_->guti.mme_group_id)) {
      return false;
    }
    if (!readvar(file, "mme_code=", &ctxt_->guti.mme_code)) {
      return false;
    }
    if (!readvar(file, "tx_count=", &ctxt_base_->tx_count)) {
      return false;
    }
    if (!readvar(file, "rx_count=", &ctxt_base_->rx_count)) {
      return false;
    }
    if (!readvar(file, "int_alg=", &ctxt_base_->integ_algo)) {
      return false;
    }
    if (!readvar(file, "enc_alg=", &ctxt_base_->cipher_algo)) {
      return false;
    }
    if (!readvar(file, "ksi=", &ctxt_->ksi)) {
      return false;
    }

    if (!readvar(file, "k_asme=", ctxt_->k_asme, 32)) {
      return false;
    }

    file.close();
    logger.info("Read GUTI from file "
                "m_tmsi: %x, mcc: %x, mnc: %x, mme_group_id: %x, mme_code: %x",
                ctxt_->guti.m_tmsi,
                ctxt_->guti.mcc,
                ctxt_->guti.mnc,
                ctxt_->guti.mme_group_id,
                ctxt_->guti.mme_code);
    logger.info("Read security ctxt from file .ctxt. "
                "ksi: %x, k_asme: %s, tx_count: %x, rx_count: %x, int_alg: %d, enc_alg: %d",
                ctxt_->ksi,
                hex_to_string(ctxt_->k_asme, 32).c_str(),
                ctxt_base_->tx_count,
                ctxt_base_->rx_count,
                ctxt_base_->integ_algo,
                ctxt_base_->cipher_algo);

    have_guti       = true;
    have_ctxt       = true;
    current_sec_hdr = LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED;

    // Set UE identity in RRC
    s_tmsi_t s_tmsi;
    s_tmsi.mmec   = ctxt.guti.mme_code;
    s_tmsi.m_tmsi = ctxt.guti.m_tmsi;
    rrc->set_ue_identity(s_tmsi);
    return true;
  }
  return false;
}

bool nas::write_ctxt_file(nas_sec_ctxt ctxt_, nas_sec_base_ctxt ctxt_base_)
{
  if (!have_guti || !have_ctxt) {
    return false;
  }
  std::ofstream file;
  file.open(".ctxt", std::ios::out | std::ios::trunc);
  if (file.is_open()) {
    file << "m_tmsi=" << (int)ctxt_.guti.m_tmsi << std::endl;
    file << "mcc=" << (int)ctxt_.guti.mcc << std::endl;
    file << "mnc=" << (int)ctxt_.guti.mnc << std::endl;
    file << "mme_group_id=" << (int)ctxt_.guti.mme_group_id << std::endl;
    file << "mme_code=" << (int)ctxt_.guti.mme_code << std::endl;
    file << "tx_count=" << (int)ctxt_base_.tx_count << std::endl;
    file << "rx_count=" << (int)ctxt_base_.rx_count << std::endl;
    file << "int_alg=" << (int)ctxt_base_.integ_algo << std::endl;
    file << "enc_alg=" << (int)ctxt_base_.cipher_algo << std::endl;
    file << "ksi=" << (int)ctxt_.ksi << std::endl;

    file << "k_asme=" << hex_to_string(ctxt_.k_asme, 32) << std::endl;

    logger.info("Saved GUTI to file "
                "m_tmsi: %x, mcc: %x, mnc: %x, mme_group_id: %x, mme_code: %x",
                ctxt_.guti.m_tmsi,
                ctxt_.guti.mcc,
                ctxt_.guti.mnc,
                ctxt_.guti.mme_group_id,
                ctxt_.guti.mme_code);
    logger.info("Saved security ctxt to file .ctxt. "
                "ksi: %x, k_asme: %s, tx_count: %x, rx_count: %x, int_alg: %d, enc_alg: %d",
                ctxt_.ksi,
                hex_to_string(ctxt_.k_asme, 32).c_str(),
                ctxt_base_.tx_count,
                ctxt_base_.rx_count,
                ctxt_base_.integ_algo,
                ctxt_base_.cipher_algo);
    file.close();
    return true;
  }
  return false;
}

/*********************************************************************
 *   Conversion helpers
 ********************************************************************/
std::string nas::hex_to_string(uint8_t* hex, int size)
{
  std::stringstream ss;

  ss << std::hex << std::setfill('0');
  for (int i = 0; i < size; i++) {
    ss << std::setw(2) << static_cast<unsigned>(hex[i]);
  }
  return ss.str();
}

bool nas::string_to_hex(std::string hex_str, uint8_t* hex, uint32_t len)
{
  static const char* const lut     = "0123456789abcdef";
  uint32_t                 str_len = hex_str.length();
  if (str_len & 1) {
    return false; // uneven hex_str length
  }
  if (str_len > len * 2) {
    return false; // not enough space in hex buffer
  }
  for (uint32_t i = 0; i < str_len; i += 2) {
    char        a = hex_str[i];
    const char* p = std::lower_bound(lut, lut + 16, a);
    if (*p != a) {
      return false; // invalid char
    }
    char        b = hex_str[i + 1];
    const char* q = std::lower_bound(lut, lut + 16, b);
    if (*q != b) {
      return false; // invalid char
    }
    hex[i / 2] = ((p - lut) << 4u) | (q - lut);
  }
  return true;
}

std::string nas::emm_info_str(LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT* info)
{
  std::stringstream ss;
  if (info->full_net_name_present) {
    ss << info->full_net_name.name;
  }
  if (info->short_net_name_present) {
    ss << " (" << info->short_net_name.name << ")";
  }
  if (info->utc_and_local_time_zone_present) {
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
