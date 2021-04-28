/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/rrc/rrc_ue.h"
#include "srsenb/hdr/stack/rrc/mac_controller.h"
#include "srsenb/hdr/stack/rrc/rrc_mobility.h"
#include "srsenb/hdr/stack/rrc/ue_rr_cfg.h"
#include "srsran/adt/pool/mem_pool.h"
#include "srsran/asn1/rrc_utils.h"
#include "srsran/common/enb_events.h"
#include "srsran/common/int_helpers.h"
#include "srsran/common/standard_streams.h"
#include "srsran/interfaces/enb_pdcp_interfaces.h"
#include "srsran/interfaces/enb_rlc_interfaces.h"
#include "srsran/interfaces/enb_s1ap_interfaces.h"

using namespace asn1::rrc;

namespace srsenb {

/*******************************************************************************
  UE class

  Every function in UE class is called from a mutex environment thus does not
  need extra protection.
*******************************************************************************/

rrc::ue::ue(rrc* outer_rrc, uint16_t rnti_, const sched_interface::ue_cfg_t& sched_ue_cfg) :
  parent(outer_rrc),
  rnti(rnti_),
  phy_rrc_dedicated_list(sched_ue_cfg.supported_cc_list.size()),
  ue_cell_list(parent->cfg, *outer_rrc->cell_res_list, *outer_rrc->cell_common_list),
  bearer_list(rnti_, parent->cfg, outer_rrc->gtpu),
  ue_security_cfg(parent->cfg),
  mac_ctrl(rnti, ue_cell_list, bearer_list, parent->cfg, parent->mac, *parent->cell_common_list, sched_ue_cfg)
{}

rrc::ue::~ue() {}

int rrc::ue::init()
{
  // Allocate cell and PUCCH resources
  if (ue_cell_list.add_cell(mac_ctrl.get_ue_sched_cfg().supported_cc_list[0].enb_cc_idx) == nullptr) {
    return SRSRAN_ERROR;
  }

  // Configure
  apply_setup_phy_common(parent->cfg.sibs[1].sib2().rr_cfg_common, true);

  rlf_timer      = parent->task_sched.get_unique_timer();
  activity_timer = parent->task_sched.get_unique_timer();
  set_activity_timeout(MSG3_RX_TIMEOUT); // next UE response is Msg3
  set_rlf_timeout();

  mobility_handler = make_rnti_obj<rrc_mobility>(rnti, this);
  return SRSRAN_SUCCESS;
}

rrc_state_t rrc::ue::get_state()
{
  return state;
}

void rrc::ue::get_metrics(rrc_ue_metrics_t& ue_metrics) const
{
  ue_metrics.state      = state;
  const auto& drb_list  = bearer_list.get_established_drbs();
  const auto& erab_list = bearer_list.get_erabs();
  ue_metrics.drb_qci_map.reserve(drb_list.size());
  for (size_t i = 0; i < drb_list.size(); ++i) {
    auto erab_it = erab_list.find(drb_list[i].eps_bearer_id);
    if (erab_it != erab_list.end()) {
      ue_metrics.drb_qci_map.push_back(std::make_pair(drb_list[i].lc_ch_id, erab_it->second.qos_params.qci));
    }
  }
}

void rrc::ue::set_activity()
{
  // re-start activity timer with current timeout value
  activity_timer.run();
  if (parent) {
    parent->logger.debug("Activity registered for rnti=0x%x (timeout_value=%dms)", rnti, activity_timer.duration());
  }
}

void rrc::ue::set_radiolink_dl_state(bool crc_res)
{
  parent->logger.debug(
      "Radio-Link downlink state for rnti=0x%x: crc_res=%d, consecutive_ko=%d", rnti, crc_res, consecutive_kos_dl);

  // If received OK, restart counter and stop RLF timer
  if (crc_res) {
    consecutive_kos_dl = 0;
    consecutive_kos_ul = 0;
    rlf_timer.stop();
    return;
  }

  // Count KOs in MAC and trigger release if it goes above a certain value.
  // This is done to detect out-of-coverage UEs
  if (rlf_timer.is_running()) {
    // RLF timer already running, no need to count KOs
    return;
  }

  consecutive_kos_dl++;
  if (consecutive_kos_dl > parent->cfg.max_mac_dl_kos) {
    parent->logger.info("Max KOs in DL reached, triggering release rnti=0x%x", rnti);
    max_retx_reached();
  }
}

void rrc::ue::set_radiolink_ul_state(bool crc_res)
{
  parent->logger.debug(
      "Radio-Link uplink state for rnti=0x%x: crc_res=%d, consecutive_ko=%d", rnti, crc_res, consecutive_kos_ul);

  // If received OK, restart counter and stop RLF timer
  if (crc_res) {
    consecutive_kos_dl = 0;
    consecutive_kos_ul = 0;
    rlf_timer.stop();
    return;
  }

  // Count KOs in MAC and trigger release if it goes above a certain value.
  // This is done to detect out-of-coverage UEs
  if (rlf_timer.is_running()) {
    // RLF timer already running, no need to count KOs
    return;
  }

  consecutive_kos_ul++;
  if (consecutive_kos_ul > parent->cfg.max_mac_ul_kos) {
    parent->logger.info("Max KOs in UL reached, triggering release rnti=0x%x", rnti);
    max_retx_reached();
  }
}

void rrc::ue::activity_timer_expired(const activity_timeout_type_t type)
{
  rlf_timer.stop();
  if (parent) {
    parent->logger.info("Activity timer for rnti=0x%x expired after %d ms", rnti, activity_timer.time_elapsed());

    if (parent->s1ap->user_exists(rnti)) {
      if (type == UE_INACTIVITY_TIMEOUT) {
        parent->s1ap->user_release(rnti, asn1::s1ap::cause_radio_network_opts::user_inactivity);
        con_release_result = procedure_result_code::activity_timeout;
      } else if (type == MSG3_RX_TIMEOUT) {
        // MSG3 timeout, no need to notify S1AP, just remove UE
        parent->rem_user_thread(rnti);
        con_release_result = procedure_result_code::msg3_timeout;
      } else {
        // Unhandled activity timeout, just remove UE and log an error
        parent->rem_user_thread(rnti);
        con_release_result = procedure_result_code::activity_timeout;
        parent->logger.error(
            "Unhandled reason for activity timer expiration. rnti=0x%x, cause %d", rnti, static_cast<unsigned>(type));
      }
    } else {
      if (rnti != SRSRAN_MRNTI) {
        parent->rem_user_thread(rnti);
      }
    }
  }

  state = RRC_STATE_RELEASE_REQUEST;
}

void rrc::ue::rlf_timer_expired()
{
  activity_timer.stop();
  if (parent) {
    parent->logger.info("RLF timer for rnti=0x%x expired after %d ms", rnti, rlf_timer.time_elapsed());

    if (parent->s1ap->user_exists(rnti)) {
      parent->s1ap->user_release(rnti, asn1::s1ap::cause_radio_network_opts::radio_conn_with_ue_lost);
      con_release_result = procedure_result_code::radio_conn_with_ue_lost;
    } else {
      if (rnti != SRSRAN_MRNTI) {
        parent->rem_user(rnti);
      }
    }
  }

  state = RRC_STATE_RELEASE_REQUEST;
}

void rrc::ue::max_retx_reached()
{
  if (parent) {
    parent->logger.info("Max retx reached for rnti=0x%x", rnti);

    // Give UE time to start re-establishment
    rlf_timer.run();

    mac_ctrl.handle_max_retx();
  }
}

void rrc::ue::set_rlf_timeout()
{
  uint32_t deadline_s  = 0;
  uint32_t deadline_ms = 0;

  deadline_ms = static_cast<uint32_t>((get_ue_cc_cfg(UE_PCELL_CC_IDX)->sib2.ue_timers_and_consts.t310.to_number()) +
                                      (get_ue_cc_cfg(UE_PCELL_CC_IDX)->sib2.ue_timers_and_consts.t311.to_number()) +
                                      (get_ue_cc_cfg(UE_PCELL_CC_IDX)->sib2.ue_timers_and_consts.n310.to_number()));
  deadline_s  = deadline_ms / 1000;
  deadline_ms = deadline_ms % 1000;

  uint32_t deadline = deadline_s * 1e3 + deadline_ms;
  rlf_timer.set(deadline, [this](uint32_t tid) { rlf_timer_expired(); });
  parent->logger.info("Setting RLF timer for rnti=0x%x to %dms", rnti, deadline);
}

void rrc::ue::set_activity_timeout(const activity_timeout_type_t type)
{
  uint32_t deadline_s  = 0;
  uint32_t deadline_ms = 0;

  switch (type) {
    case MSG3_RX_TIMEOUT:
      deadline_s  = 0;
      deadline_ms = static_cast<uint32_t>(
          (get_ue_cc_cfg(UE_PCELL_CC_IDX)->sib2.rr_cfg_common.rach_cfg_common.max_harq_msg3_tx + 1) * 16);
      break;
    case UE_INACTIVITY_TIMEOUT:
      deadline_s  = parent->cfg.inactivity_timeout_ms / 1000;
      deadline_ms = parent->cfg.inactivity_timeout_ms % 1000;
      break;
    default:
      parent->logger.error("Unknown timeout type %d", type);
  }

  uint32_t deadline = deadline_s * 1e3 + deadline_ms;
  activity_timer.set(deadline, [this, type](uint32_t tid) { activity_timer_expired(type); });
  parent->logger.debug("Setting timer for %s for rnti=0x%x to %dms", to_string(type).c_str(), rnti, deadline);

  set_activity();
}

bool rrc::ue::is_connected()
{
  return state == RRC_STATE_REGISTERED;
}

bool rrc::ue::is_idle()
{
  return state == RRC_STATE_IDLE;
}

void rrc::ue::parse_ul_dcch(uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  ul_dcch_msg_s  ul_dcch_msg;
  asn1::cbit_ref bref(pdu->msg, pdu->N_bytes);
  if (ul_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
      ul_dcch_msg.msg.type().value != ul_dcch_msg_type_c::types_opts::c1) {
    parent->logger.error("Failed to unpack UL-DCCH message");
    return;
  }

  parent->log_rrc_message(get_rb_name(lcid), Rx, pdu.get(), ul_dcch_msg, ul_dcch_msg.msg.c1().type().to_string());

  srsran::unique_byte_buffer_t original_pdu = std::move(pdu);
  pdu                                       = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    parent->logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  transaction_id = 0;

  switch (ul_dcch_msg.msg.c1().type()) {
    case ul_dcch_msg_type_c::c1_c_::types::rrc_conn_setup_complete:
      save_ul_message(std::move(original_pdu));
      handle_rrc_con_setup_complete(&ul_dcch_msg.msg.c1().rrc_conn_setup_complete(), std::move(pdu));
      set_activity();
      break;
    case ul_dcch_msg_type_c::c1_c_::types::rrc_conn_reest_complete:
      save_ul_message(std::move(original_pdu));
      handle_rrc_con_reest_complete(&ul_dcch_msg.msg.c1().rrc_conn_reest_complete(), std::move(pdu));
      set_activity();
      break;
    case ul_dcch_msg_type_c::c1_c_::types::ul_info_transfer:
      pdu->N_bytes = ul_dcch_msg.msg.c1()
                         .ul_info_transfer()
                         .crit_exts.c1()
                         .ul_info_transfer_r8()
                         .ded_info_type.ded_info_nas()
                         .size();
      memcpy(pdu->msg,
             ul_dcch_msg.msg.c1()
                 .ul_info_transfer()
                 .crit_exts.c1()
                 .ul_info_transfer_r8()
                 .ded_info_type.ded_info_nas()
                 .data(),
             pdu->N_bytes);
      parent->s1ap->write_pdu(rnti, std::move(pdu));
      break;
    case ul_dcch_msg_type_c::c1_c_::types::rrc_conn_recfg_complete:
      save_ul_message(std::move(original_pdu));
      handle_rrc_reconf_complete(&ul_dcch_msg.msg.c1().rrc_conn_recfg_complete(), std::move(pdu));
      srsran::console("User 0x%x connected\n", rnti);
      state = RRC_STATE_REGISTERED;
      set_activity_timeout(UE_INACTIVITY_TIMEOUT);
      break;
    case ul_dcch_msg_type_c::c1_c_::types::security_mode_complete:
      handle_security_mode_complete(&ul_dcch_msg.msg.c1().security_mode_complete());
      send_ue_cap_enquiry();
      state = RRC_STATE_WAIT_FOR_UE_CAP_INFO;
      break;
    case ul_dcch_msg_type_c::c1_c_::types::security_mode_fail:
      handle_security_mode_failure(&ul_dcch_msg.msg.c1().security_mode_fail());
      break;
    case ul_dcch_msg_type_c::c1_c_::types::ue_cap_info:
      if (handle_ue_cap_info(&ul_dcch_msg.msg.c1().ue_cap_info())) {
        parent->s1ap->ue_ctxt_setup_complete(rnti);
        send_connection_reconf(std::move(pdu));
        state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
      } else {
        send_connection_reject(procedure_result_code::none);
        state = RRC_STATE_IDLE;
      }
      break;
    case ul_dcch_msg_type_c::c1_c_::types::meas_report:
      if (mobility_handler != nullptr) {
        mobility_handler->handle_ue_meas_report(ul_dcch_msg.msg.c1().meas_report(), std::move(original_pdu));
      } else {
        parent->logger.warning("Received MeasReport but no mobility configuration is available");
      }
      break;
    case ul_dcch_msg_type_c::c1_c_::types::ue_info_resp_r9:
      handle_ue_info_resp(ul_dcch_msg.msg.c1().ue_info_resp_r9(), std::move(original_pdu));
      break;
    default:
      parent->logger.error("Msg: %s not supported", ul_dcch_msg.msg.c1().type().to_string());
      break;
  }
}

std::string rrc::ue::to_string(const activity_timeout_type_t& type)
{
  constexpr static const char* options[] = {"Msg3 reception", "UE inactivity", "UE reestablishment"};
  return srsran::enum_to_text(options, (uint32_t)activity_timeout_type_t::nulltype, (uint32_t)type);
}

/*
 *  Connection Setup
 */
void rrc::ue::handle_rrc_con_req(rrc_conn_request_s* msg)
{
  // Log event.
  event_logger::get().log_rrc_event(ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
                                    asn1::octstring_to_string(last_ul_msg->msg, last_ul_msg->N_bytes),
                                    static_cast<unsigned>(rrc_event_type::con_request),
                                    static_cast<unsigned>(procedure_result_code::none),
                                    rnti);

  if (not parent->s1ap->is_mme_connected()) {
    parent->logger.error("MME isn't connected. Sending Connection Reject");
    send_connection_reject(procedure_result_code::error_mme_not_connected);
    return;
  }

  rrc_conn_request_r8_ies_s* msg_r8 = &msg->crit_exts.rrc_conn_request_r8();

  if (msg_r8->ue_id.type() == init_ue_id_c::types::s_tmsi) {
    mmec     = (uint8_t)msg_r8->ue_id.s_tmsi().mmec.to_number();
    m_tmsi   = (uint32_t)msg_r8->ue_id.s_tmsi().m_tmsi.to_number();
    has_tmsi = true;

    // Make sure that the context does not already exist
    for (auto& user : parent->users) {
      if (user.first != rnti && user.second->has_tmsi && user.second->mmec == mmec && user.second->m_tmsi == m_tmsi) {
        parent->logger.info("RRC connection request: UE context already exists. M-TMSI=%d", m_tmsi);
        if (parent->s1ap->user_release(rnti, asn1::s1ap::cause_radio_network_opts::radio_conn_with_ue_lost)) {
          // Do not wait for MME response
          parent->rem_user_thread(user.first);
        }
        break;
      }
    }
  }

  establishment_cause = msg_r8->establishment_cause;
  send_connection_setup();
  state = RRC_STATE_WAIT_FOR_CON_SETUP_COMPLETE;

  set_activity_timeout(UE_INACTIVITY_TIMEOUT);
}

void rrc::ue::send_connection_setup()
{
  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set_c1();

  rrc_conn_setup_s& rrc_setup       = dl_ccch_msg.msg.c1().set_rrc_conn_setup();
  rrc_setup.rrc_transaction_id      = (uint8_t)((transaction_id++) % 4);
  rrc_conn_setup_r8_ies_s& setup_r8 = rrc_setup.crit_exts.set_c1().set_rrc_conn_setup_r8();
  rr_cfg_ded_s&            rr_cfg   = setup_r8.rr_cfg_ded;

  // Fill RR config dedicated
  fill_rr_cfg_ded_setup(rr_cfg, parent->cfg, ue_cell_list);

  // Apply ConnectionSetup Configuration to MAC scheduler
  mac_ctrl.handle_con_setup(setup_r8);

  // Add SRBs/DRBs, and configure RLC+PDCP
  apply_pdcp_srb_updates(setup_r8.rr_cfg_ded);
  apply_pdcp_drb_updates(setup_r8.rr_cfg_ded);
  apply_rlc_rb_updates(setup_r8.rr_cfg_ded);

  // Configure PHY layer
  apply_setup_phy_config_dedicated(rr_cfg.phys_cfg_ded); // It assumes SCell has not been set before

  std::string octet_str;
  send_dl_ccch(&dl_ccch_msg, &octet_str);

  // Log event.
  event_logger::get().log_rrc_event(ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
                                    octet_str,
                                    static_cast<unsigned>(rrc_event_type::con_setup),
                                    static_cast<unsigned>(procedure_result_code::none),
                                    rnti);

  apply_rr_cfg_ded_diff(current_ue_cfg.rr_cfg, rr_cfg);
}

void rrc::ue::handle_rrc_con_setup_complete(rrc_conn_setup_complete_s* msg, srsran::unique_byte_buffer_t pdu)
{
  // Log event.
  event_logger::get().log_rrc_event(ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
                                    asn1::octstring_to_string(last_ul_msg->msg, last_ul_msg->N_bytes),
                                    static_cast<unsigned>(rrc_event_type::con_setup_complete),
                                    static_cast<unsigned>(procedure_result_code::none),
                                    rnti);

  // Inform PHY about the configuration completion
  parent->phy->complete_config(rnti);

  parent->logger.info("RRCConnectionSetupComplete transaction ID: %d", msg->rrc_transaction_id);
  rrc_conn_setup_complete_r8_ies_s* msg_r8 = &msg->crit_exts.c1().rrc_conn_setup_complete_r8();

  // TODO: msg->selected_plmn_id - used to select PLMN from SIB1 list
  // TODO: if(msg->registered_mme_present) - the indicated MME should be used from a pool

  pdu->N_bytes = msg_r8->ded_info_nas.size();
  memcpy(pdu->msg, msg_r8->ded_info_nas.data(), pdu->N_bytes);

  // Signal MAC scheduler that configuration was successful
  mac_ctrl.handle_con_setup_complete();

  asn1::s1ap::rrc_establishment_cause_e s1ap_cause;
  s1ap_cause.value = (asn1::s1ap::rrc_establishment_cause_opts::options)establishment_cause.value;

  uint32_t enb_cc_idx = ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx;
  if (has_tmsi) {
    parent->s1ap->initial_ue(rnti, enb_cc_idx, s1ap_cause, std::move(pdu), m_tmsi, mmec);
  } else {
    parent->s1ap->initial_ue(rnti, enb_cc_idx, s1ap_cause, std::move(pdu));
  }
  state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;

  // 2> if the UE has radio link failure or handover failure information available
  if (msg->crit_exts.type().value == c1_or_crit_ext_opts::c1 and
      msg->crit_exts.c1().type().value ==
          rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::types_opts::rrc_conn_setup_complete_r8) {
    const auto& complete_r8 = msg->crit_exts.c1().rrc_conn_setup_complete_r8();
    if (complete_r8.non_crit_ext.non_crit_ext.rlf_info_available_r10_present) {
      rlf_info_pending = true;
    }
  }
}

void rrc::ue::send_connection_reject(procedure_result_code cause)
{
  mac_ctrl.handle_con_reject();

  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set_c1().set_rrc_conn_reject().crit_exts.set_c1().set_rrc_conn_reject_r8().wait_time = 10;

  std::string octet_str;
  send_dl_ccch(&dl_ccch_msg, &octet_str);

  // Log event.
  event_logger::get().log_rrc_event(ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
                                    octet_str,
                                    static_cast<unsigned>(rrc_event_type::con_reject),
                                    static_cast<unsigned>(cause),
                                    rnti);
}

/*
 * Connection Reestablishment
 */
void rrc::ue::handle_rrc_con_reest_req(rrc_conn_reest_request_s* msg)
{
  // Log event.
  event_logger::get().log_rrc_event(ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
                                    asn1::octstring_to_string(last_ul_msg->msg, last_ul_msg->N_bytes),
                                    static_cast<unsigned>(rrc_event_type::con_reest_req),
                                    static_cast<unsigned>(procedure_result_code::none),
                                    rnti);

  if (not parent->s1ap->is_mme_connected()) {
    parent->logger.error("MME isn't connected. Sending Connection Reject");
    send_connection_reject(procedure_result_code::error_mme_not_connected);
    return;
  }
  parent->logger.debug("rnti=0x%x, phyid=0x%x, smac=0x%x, cause=%s",
                       (uint32_t)msg->crit_exts.rrc_conn_reest_request_r8().ue_id.c_rnti.to_number(),
                       msg->crit_exts.rrc_conn_reest_request_r8().ue_id.pci,
                       (uint32_t)msg->crit_exts.rrc_conn_reest_request_r8().ue_id.short_mac_i.to_number(),
                       msg->crit_exts.rrc_conn_reest_request_r8().reest_cause.to_string());
  if (is_idle()) {
    uint16_t               old_rnti = msg->crit_exts.rrc_conn_reest_request_r8().ue_id.c_rnti.to_number();
    uint16_t               old_pci  = msg->crit_exts.rrc_conn_reest_request_r8().ue_id.pci;
    const enb_cell_common* old_cell = parent->cell_common_list->get_pci(old_pci);
    auto                   ue_it    = parent->users.find(old_rnti);
    // Reject unrecognized rntis, and PCIs that do not belong to eNB
    if (ue_it != parent->users.end() and old_cell != nullptr and
        ue_it->second->ue_cell_list.get_enb_cc_idx(old_cell->enb_cc_idx) != nullptr) {
      parent->logger.info("ConnectionReestablishmentRequest for rnti=0x%x. Sending Connection Reestablishment",
                          old_rnti);

      // Cancel Handover in Target eNB if on-going
      asn1::s1ap::cause_c cause;
      cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::interaction_with_other_proc;
      parent->users.at(old_rnti)->mobility_handler->trigger(rrc_mobility::ho_cancel_ev{cause});

      // Recover security setup
      const enb_cell_common* pcell_cfg = get_ue_cc_cfg(UE_PCELL_CC_IDX);
      ue_security_cfg                  = parent->users.at(old_rnti)->ue_security_cfg;
      ue_security_cfg.regenerate_keys_handover(pcell_cfg->cell_cfg.pci, pcell_cfg->cell_cfg.dl_earfcn);

      // send reestablishment and restore bearer configuration
      send_connection_reest(parent->users.at(old_rnti)->ue_security_cfg.get_ncc());

      // Get PDCP entity state (required when using RLC AM)
      for (const auto& erab_pair : parent->users.at(old_rnti)->bearer_list.get_erabs()) {
        uint16_t lcid              = erab_pair.second.id - 2;
        old_reest_pdcp_state[lcid] = {};
        parent->pdcp->get_bearer_state(old_rnti, lcid, &old_reest_pdcp_state[lcid]);

        parent->logger.debug("Getting PDCP state for E-RAB with LCID %d", lcid);
        parent->logger.debug("Got PDCP state: TX HFN %d, NEXT_PDCP_TX_SN %d, RX_HFN %d, NEXT_PDCP_RX_SN %d, "
                             "LAST_SUBMITTED_PDCP_RX_SN %d",
                             old_reest_pdcp_state[lcid].tx_hfn,
                             old_reest_pdcp_state[lcid].next_pdcp_tx_sn,
                             old_reest_pdcp_state[lcid].rx_hfn,
                             old_reest_pdcp_state[lcid].next_pdcp_rx_sn,
                             old_reest_pdcp_state[lcid].last_submitted_pdcp_rx_sn);
      }

      // Make sure UE capabilities are copied over to new RNTI
      eutra_capabilities          = parent->users.at(old_rnti)->eutra_capabilities;
      eutra_capabilities_unpacked = parent->users.at(old_rnti)->eutra_capabilities_unpacked;
      ue_capabilities             = parent->users.at(old_rnti)->ue_capabilities;
      if (parent->logger.debug.enabled()) {
        asn1::json_writer js{};
        eutra_capabilities.to_json(js);
        parent->logger.debug("rnti=0x%x EUTRA capabilities: %s", rnti, js.to_string().c_str());
      }

      old_reest_rnti = old_rnti;
      state          = RRC_STATE_WAIT_FOR_CON_REEST_COMPLETE;
      set_activity_timeout(UE_INACTIVITY_TIMEOUT);
    } else {
      parent->logger.error("Received ConnectionReestablishment for rnti=0x%x without context", old_rnti);
      send_connection_reest_rej(procedure_result_code::error_unknown_rnti);
    }
  } else {
    parent->logger.error("Received ReestablishmentRequest from an rnti=0x%x not in IDLE", rnti);
  }
}

void rrc::ue::send_connection_reest(uint8_t ncc)
{
  dl_ccch_msg_s dl_ccch_msg;
  auto&         reest               = dl_ccch_msg.msg.set_c1().set_rrc_conn_reest();
  reest.rrc_transaction_id          = (uint8_t)((transaction_id++) % 4);
  rrc_conn_reest_r8_ies_s& reest_r8 = reest.crit_exts.set_c1().set_rrc_conn_reest_r8();
  rr_cfg_ded_s&            rr_cfg   = reest_r8.rr_cfg_ded;

  // Fill RR config dedicated
  fill_rr_cfg_ded_setup(rr_cfg, parent->cfg, ue_cell_list);

  // Set NCC
  reest_r8.next_hop_chaining_count = ncc;

  // Apply ConnectionReest Configuration to MAC scheduler
  mac_ctrl.handle_con_reest(reest_r8);

  // Add SRBs/DRBs, and configure RLC+PDCP
  apply_pdcp_srb_updates(rr_cfg);
  apply_pdcp_drb_updates(rr_cfg);
  apply_rlc_rb_updates(rr_cfg);

  // Configure PHY layer
  apply_setup_phy_config_dedicated(rr_cfg.phys_cfg_ded); // It assumes SCell has not been set before

  std::string octet_str;
  send_dl_ccch(&dl_ccch_msg, &octet_str);

  apply_rr_cfg_ded_diff(current_ue_cfg.rr_cfg, rr_cfg);

  // Log event.
  event_logger::get().log_rrc_event(ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
                                    octet_str,
                                    static_cast<unsigned>(rrc_event_type::con_reest),
                                    static_cast<unsigned>(procedure_result_code::none),
                                    rnti);
}

void rrc::ue::handle_rrc_con_reest_complete(rrc_conn_reest_complete_s* msg, srsran::unique_byte_buffer_t pdu)
{
  // Log event.
  event_logger::get().log_rrc_event(ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
                                    asn1::octstring_to_string(last_ul_msg->msg, last_ul_msg->N_bytes),
                                    static_cast<unsigned>(rrc_event_type::con_reest_complete),
                                    static_cast<unsigned>(procedure_result_code::none),
                                    rnti);

  // Inform PHY about the configuration completion
  parent->phy->complete_config(rnti);

  parent->logger.info("RRCConnectionReestablishComplete transaction ID: %d", msg->rrc_transaction_id);

  // TODO: msg->selected_plmn_id - used to select PLMN from SIB1 list
  // TODO: if(msg->registered_mme_present) - the indicated MME should be used from a pool

  // Modify GTP-U tunnel and S1AP context
  parent->gtpu->mod_bearer_rnti(old_reest_rnti, rnti);
  parent->s1ap->user_mod(old_reest_rnti, rnti);

  // Signal MAC scheduler that configuration was successful
  mac_ctrl.handle_con_reest_complete();

  // Activate security for SRB1
  parent->pdcp->config_security(rnti, srb_to_lcid(lte_srb::srb1), ue_security_cfg.get_as_sec_cfg());
  parent->pdcp->enable_integrity(rnti, srb_to_lcid(lte_srb::srb1));
  parent->pdcp->enable_encryption(rnti, srb_to_lcid(lte_srb::srb1));

  // Reestablish current DRBs during ConnectionReconfiguration
  bearer_list = std::move(parent->users.at(old_reest_rnti)->bearer_list);

  // remove old RNTI
  parent->rem_user_thread(old_reest_rnti);

  state = RRC_STATE_REESTABLISHMENT_COMPLETE;

  // 2> if the UE has radio link failure or handover failure information available
  if (msg->crit_exts.type().value == rrc_conn_reest_complete_s::crit_exts_c_::types_opts::rrc_conn_reest_complete_r8) {
    const auto& complete_r8 = msg->crit_exts.rrc_conn_reest_complete_r8();
    if (complete_r8.non_crit_ext.rlf_info_available_r9_present) {
      rlf_info_pending = true;
    }
  }

  send_connection_reconf(std::move(pdu));
}

void rrc::ue::send_connection_reest_rej(procedure_result_code cause)
{
  mac_ctrl.handle_con_reject();

  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set_c1().set_rrc_conn_reest_reject().crit_exts.set_rrc_conn_reest_reject_r8();

  std::string octet_str;
  send_dl_ccch(&dl_ccch_msg, &octet_str);

  // Log event.
  event_logger::get().log_rrc_event(ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
                                    octet_str,
                                    static_cast<unsigned>(rrc_event_type::con_reest_reject),
                                    static_cast<unsigned>(cause),
                                    rnti);
}

/*
 * Connection Reconfiguration
 */
void rrc::ue::send_connection_reconf(srsran::unique_byte_buffer_t pdu,
                                     bool                         phy_cfg_updated,
                                     srsran::const_byte_span      nas_pdu)
{
  parent->logger.debug("RRC state %d", state);

  update_scells();

  /* Create RRCConnectionReconfiguration ASN1 message */
  dl_dcch_msg_s     dl_dcch_msg;
  rrc_conn_recfg_s& rrc_conn_recfg  = dl_dcch_msg.msg.set_c1().set_rrc_conn_recfg();
  rrc_conn_recfg.rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
  rrc_conn_recfg_r8_ies_s& recfg_r8 = rrc_conn_recfg.crit_exts.set_c1().set_rrc_conn_recfg_r8();

  // Fill RR Config Ded and SCells
  apply_reconf_updates(
      recfg_r8, current_ue_cfg, parent->cfg, ue_cell_list, bearer_list, ue_capabilities, phy_cfg_updated);

  // Add measConfig
  if (mobility_handler != nullptr) {
    mobility_handler->fill_conn_recfg_no_ho_cmd(&recfg_r8);
  }

  // if no updates were detected, skip rrc reconfiguration
  if (not(recfg_r8.rr_cfg_ded_present or recfg_r8.meas_cfg_present or recfg_r8.mob_ctrl_info_present or
          recfg_r8.ded_info_nas_list_present or recfg_r8.security_cfg_ho_present or recfg_r8.non_crit_ext_present)) {
    return;
  }

  /* Apply updates present in RRCConnectionReconfiguration to lower layers */
  // apply PHY config
  apply_reconf_phy_config(recfg_r8, true);

  // setup SRB2/DRBs in PDCP and RLC
  apply_rlc_rb_updates(recfg_r8.rr_cfg_ded);
  apply_pdcp_srb_updates(recfg_r8.rr_cfg_ded);
  apply_pdcp_drb_updates(recfg_r8.rr_cfg_ded);

  // UE MAC scheduler updates
  mac_ctrl.handle_con_reconf(recfg_r8, ue_capabilities);

  // Fill in NAS PDU - Only for RRC Connection Reconfiguration during E-RAB Release Command
  if (nas_pdu.size() > 0 and !recfg_r8.ded_info_nas_list_present) {
    recfg_r8.ded_info_nas_list_present = true;
    recfg_r8.ded_info_nas_list.resize(recfg_r8.rr_cfg_ded.drb_to_release_list.size());
    // Add NAS PDU
    for (uint32_t idx = 0; idx < recfg_r8.rr_cfg_ded.drb_to_release_list.size(); idx++) {
      recfg_r8.ded_info_nas_list[idx].resize(nas_pdu.size());
      memcpy(recfg_r8.ded_info_nas_list[idx].data(), nas_pdu.data(), nas_pdu.size());
    }
  }

  // Reuse same PDU
  if (pdu != nullptr) {
    pdu->clear();
  }

  // send DL-DCCH message to lower layers
  std::string octet_str;
  send_dl_dcch(&dl_dcch_msg, std::move(pdu), &octet_str);

  // Log event.
  event_logger::get().log_rrc_event(ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
                                    octet_str,
                                    static_cast<unsigned>(rrc_event_type::con_reconf),
                                    static_cast<unsigned>(procedure_result_code::none),
                                    rnti);

  state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
}

void rrc::ue::handle_rrc_reconf_complete(rrc_conn_recfg_complete_s* msg, srsran::unique_byte_buffer_t pdu)
{
  // Inform PHY about the configuration completion
  parent->phy->complete_config(rnti);

  if (transaction_id != msg->rrc_transaction_id) {
    parent->logger.error(
        "Expected RRCReconfigurationComplete with transaction ID: %d, got %d", transaction_id, msg->rrc_transaction_id);
    return;
  }

  // Log event.
  event_logger::get().log_rrc_event(ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
                                    asn1::octstring_to_string(last_ul_msg->msg, last_ul_msg->N_bytes),
                                    static_cast<unsigned>(rrc_event_type::con_reconf_complete),
                                    static_cast<unsigned>(procedure_result_code::none),
                                    rnti);

  // Activate SCells and bearers in the MAC scheduler that were advertised in the RRC Reconf message
  mac_ctrl.handle_con_reconf_complete();

  // If performing handover, signal its completion
  mobility_handler->trigger(*msg);

  // 2> if the UE has radio link failure or handover failure information available
  const auto& complete_r8 = msg->crit_exts.rrc_conn_recfg_complete_r8();
  if (complete_r8.non_crit_ext.non_crit_ext.rlf_info_available_r10_present or rlf_info_pending) {
    rlf_info_pending = false;
    send_ue_info_req();
  }
}

void rrc::ue::send_ue_info_req()
{
  dl_dcch_msg_s msg;
  auto&         req_r9      = msg.msg.set_c1().set_ue_info_request_r9();
  req_r9.rrc_transaction_id = (uint8_t)((transaction_id++) % 4);

  auto& req              = req_r9.crit_exts.set_c1().set_ue_info_request_r9();
  req.rlf_report_req_r9  = true;
  req.rach_report_req_r9 = true;

  send_dl_dcch(&msg);
}

void rrc::ue::handle_ue_info_resp(const asn1::rrc::ue_info_resp_r9_s& msg, srsran::unique_byte_buffer_t pdu)
{
  auto& resp_r9 = msg.crit_exts.c1().ue_info_resp_r9();
  if (resp_r9.rlf_report_r9_present) {
    std::string msg_str = asn1::octstring_to_string(pdu->msg, pdu->N_bytes);
    event_logger::get().log_rlf(ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx, msg_str, rnti);
  }
  if (resp_r9.rach_report_r9_present) {
    // TODO: Handle RACH-Report
  }
}

/*
 * Security Mode command
 */
void rrc::ue::send_security_mode_command()
{
  // Setup SRB1 security/integrity. Encryption is set on completion
  parent->pdcp->config_security(rnti, srb_to_lcid(lte_srb::srb1), ue_security_cfg.get_as_sec_cfg());
  parent->pdcp->enable_integrity(rnti, srb_to_lcid(lte_srb::srb1));

  dl_dcch_msg_s        dl_dcch_msg;
  security_mode_cmd_s* comm = &dl_dcch_msg.msg.set_c1().set_security_mode_cmd();
  comm->rrc_transaction_id  = (uint8_t)((transaction_id++) % 4);

  comm->crit_exts.set_c1().set_security_mode_cmd_r8().security_cfg_smc.security_algorithm_cfg =
      ue_security_cfg.get_security_algorithm_cfg();

  send_dl_dcch(&dl_dcch_msg);
}

void rrc::ue::handle_security_mode_complete(security_mode_complete_s* msg)
{
  parent->logger.info("SecurityModeComplete transaction ID: %d", msg->rrc_transaction_id);

  parent->pdcp->enable_encryption(rnti, srb_to_lcid(lte_srb::srb1));
}

void rrc::ue::handle_security_mode_failure(security_mode_fail_s* msg)
{
  parent->logger.info("SecurityModeFailure transaction ID: %d", msg->rrc_transaction_id);
}

/*
 * UE capabilities info
 */
void rrc::ue::send_ue_cap_enquiry()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_ue_cap_enquiry().crit_exts.set_c1().set_ue_cap_enquiry_r8();

  ue_cap_enquiry_s* enq   = &dl_dcch_msg.msg.c1().ue_cap_enquiry();
  enq->rrc_transaction_id = (uint8_t)((transaction_id++) % 4);

  enq->crit_exts.c1().ue_cap_enquiry_r8().ue_cap_request.resize(1);
  enq->crit_exts.c1().ue_cap_enquiry_r8().ue_cap_request[0].value = rat_type_e::eutra;

  send_dl_dcch(&dl_dcch_msg);
}

bool rrc::ue::handle_ue_cap_info(ue_cap_info_s* msg)
{
  parent->logger.info("UECapabilityInformation transaction ID: %d", msg->rrc_transaction_id);
  ue_cap_info_r8_ies_s* msg_r8 = &msg->crit_exts.c1().ue_cap_info_r8();

  for (uint32_t i = 0; i < msg_r8->ue_cap_rat_container_list.size(); i++) {
    if (msg_r8->ue_cap_rat_container_list[i].rat_type != rat_type_e::eutra) {
      parent->logger.warning("Not handling UE capability information for RAT type %s",
                             msg_r8->ue_cap_rat_container_list[i].rat_type.to_string());
      continue;
    }
    asn1::cbit_ref bref(msg_r8->ue_cap_rat_container_list[i].ue_cap_rat_container.data(),
                        msg_r8->ue_cap_rat_container_list[i].ue_cap_rat_container.size());
    if (eutra_capabilities.unpack(bref) != asn1::SRSASN_SUCCESS) {
      parent->logger.error("Failed to unpack EUTRA capabilities message");
      return false;
    }
    if (parent->logger.debug.enabled()) {
      asn1::json_writer js{};
      eutra_capabilities.to_json(js);
      parent->logger.debug("rnti=0x%x EUTRA capabilities: %s", rnti, js.to_string().c_str());
    }
    eutra_capabilities_unpacked = true;
    ue_capabilities             = srsran::make_rrc_ue_capabilities(eutra_capabilities);

    parent->logger.info("UE rnti: 0x%x category: %d", rnti, eutra_capabilities.ue_category);
  }

  if (eutra_capabilities_unpacked) {
    srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    if (pdu == nullptr) {
      parent->logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return false;
    }
    asn1::bit_ref bref2{pdu->msg, pdu->get_tailroom()};
    msg->pack(bref2);
    asn1::rrc::ue_radio_access_cap_info_s ue_rat_caps;
    auto& dest = ue_rat_caps.crit_exts.set_c1().set_ue_radio_access_cap_info_r8().ue_radio_access_cap_info;
    dest.resize(bref2.distance_bytes());
    memcpy(dest.data(), pdu->msg, bref2.distance_bytes());
    bref2 = asn1::bit_ref{pdu->msg, pdu->get_tailroom()};
    ue_rat_caps.pack(bref2);
    pdu->N_bytes = bref2.distance_bytes();
    parent->s1ap->send_ue_cap_info_indication(rnti, std::move(pdu));
  }

  return true;
}

/*
 * Connection Release
 */
void rrc::ue::send_connection_release()
{
  dl_dcch_msg_s dl_dcch_msg;
  auto&         rrc_release          = dl_dcch_msg.msg.set_c1().set_rrc_conn_release();
  rrc_release.rrc_transaction_id     = (uint8_t)((transaction_id++) % 4);
  rrc_conn_release_r8_ies_s& rel_ies = rrc_release.crit_exts.set_c1().set_rrc_conn_release_r8();
  rel_ies.release_cause              = release_cause_e::other;
  if (is_csfb) {
    if (parent->sib7.carrier_freqs_info_list.size() > 0) {
      rel_ies.redirected_carrier_info_present = true;
      rel_ies.redirected_carrier_info.set_geran();
      rel_ies.redirected_carrier_info.geran() = parent->sib7.carrier_freqs_info_list[0].carrier_freqs;
    } else {
      rel_ies.redirected_carrier_info_present = false;
    }
  }

  std::string octet_str;
  send_dl_dcch(&dl_dcch_msg, nullptr, &octet_str);

  // Log rrc release event.
  event_logger::get().log_rrc_event(ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
                                    octet_str,
                                    static_cast<unsigned>(rrc_event_type::con_release),
                                    static_cast<unsigned>(con_release_result),
                                    rnti);
  // Restore release result.
  con_release_result = procedure_result_code::none;
}

/*
 * UE Init Context Setup Request
 */
void rrc::ue::handle_ue_init_ctxt_setup_req(const asn1::s1ap::init_context_setup_request_s& msg)
{
  set_bitrates(msg.protocol_ies.ueaggregate_maximum_bitrate.value);
  ue_security_cfg.set_security_capabilities(msg.protocol_ies.ue_security_cap.value);
  ue_security_cfg.set_security_key(msg.protocol_ies.security_key.value);

  // CSFB
  if (msg.protocol_ies.cs_fallback_ind_present) {
    if (msg.protocol_ies.cs_fallback_ind.value.value == asn1::s1ap::cs_fallback_ind_opts::cs_fallback_required or
        msg.protocol_ies.cs_fallback_ind.value.value == asn1::s1ap::cs_fallback_ind_opts::cs_fallback_high_prio) {
      is_csfb = true;
    }
  }

  // Send RRC security mode command
  send_security_mode_command();
}

bool rrc::ue::handle_ue_ctxt_mod_req(const asn1::s1ap::ue_context_mod_request_s& msg)
{
  if (msg.protocol_ies.cs_fallback_ind_present) {
    if (msg.protocol_ies.cs_fallback_ind.value.value == asn1::s1ap::cs_fallback_ind_opts::cs_fallback_required ||
        msg.protocol_ies.cs_fallback_ind.value.value == asn1::s1ap::cs_fallback_ind_opts::cs_fallback_high_prio) {
      /* Remember that we are in a CSFB right now */
      is_csfb = true;
    }
  }

  // UEAggregateMaximumBitrate
  if (msg.protocol_ies.ueaggregate_maximum_bitrate_present) {
    set_bitrates(msg.protocol_ies.ueaggregate_maximum_bitrate.value);
  }

  if (msg.protocol_ies.ue_security_cap_present) {
    ue_security_cfg.set_security_capabilities(msg.protocol_ies.ue_security_cap.value);
  }

  if (msg.protocol_ies.security_key_present) {
    ue_security_cfg.set_security_key(msg.protocol_ies.security_key.value);

    send_security_mode_command();
  }

  return true;
}

void rrc::ue::set_bitrates(const asn1::s1ap::ue_aggregate_maximum_bitrate_s& rates)
{
  bitrates = rates;
}

bool rrc::ue::setup_erabs(const asn1::s1ap::erab_to_be_setup_list_ctxt_su_req_l& e)
{
  for (const auto& item : e) {
    const auto& erab = item.value.erab_to_be_setup_item_ctxt_su_req();
    if (erab.ext) {
      parent->logger.warning("Not handling E-RABToBeSetupListCtxtSURequest extensions");
    }
    if (erab.ie_exts_present) {
      parent->logger.warning("Not handling E-RABToBeSetupListCtxtSURequest extensions");
    }
    if (erab.transport_layer_address.length() > 32) {
      parent->logger.error("IPv6 addresses not currently supported");
      return false;
    }

    uint32_t teid_out = 0;
    srsran::uint8_to_uint32(erab.gtp_teid.data(), &teid_out);
    srsran::const_span<uint8_t> nas_pdu;
    if (erab.nas_pdu_present) {
      nas_pdu = erab.nas_pdu;
    }
    asn1::s1ap::cause_c cause;
    bearer_list.add_erab(
        erab.erab_id, erab.erab_level_qos_params, erab.transport_layer_address, teid_out, nas_pdu, cause);
    bearer_list.add_gtpu_bearer(erab.erab_id);
  }
  return true;
}

bool rrc::ue::release_erabs()
{
  bearer_list.release_erabs();
  return true;
}

int rrc::ue::release_erab(uint32_t erab_id)
{
  return bearer_list.release_erab(erab_id);
}

int rrc::ue::get_erab_addr_in(uint16_t erab_id, transp_addr_t& addr_in, uint32_t& teid_in) const
{
  auto it = bearer_list.get_erabs().find(erab_id);
  if (it == bearer_list.get_erabs().end()) {
    parent->logger.error("E-RAB id=%d for rnti=0x%x not found", erab_id, rnti);
    return SRSRAN_ERROR;
  }
  addr_in = it->second.address;
  teid_in = it->second.teid_in;
  return SRSRAN_SUCCESS;
}

int rrc::ue::setup_erab(uint16_t                                           erab_id,
                        const asn1::s1ap::erab_level_qos_params_s&         qos_params,
                        srsran::const_span<uint8_t>                        nas_pdu,
                        const asn1::bounded_bitstring<1, 160, true, true>& addr,
                        uint32_t                                           gtpu_teid_out,
                        asn1::s1ap::cause_c&                               cause)
{
  if (bearer_list.get_erabs().count(erab_id) > 0) {
    cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::multiple_erab_id_instances;
    return SRSRAN_ERROR;
  }
  if (bearer_list.add_erab(erab_id, qos_params, addr, gtpu_teid_out, nas_pdu, cause) != SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }
  if (bearer_list.add_gtpu_bearer(erab_id) != SRSRAN_SUCCESS) {
    cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::radio_res_not_available;
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

int rrc::ue::modify_erab(uint16_t                                   erab_id,
                         const asn1::s1ap::erab_level_qos_params_s& qos_params,
                         srsran::const_span<uint8_t>                nas_pdu,
                         asn1::s1ap::cause_c&                       cause)
{
  return bearer_list.modify_erab(erab_id, qos_params, nas_pdu, cause);
}

//! Helper method to access Cell configuration based on UE Carrier Index
enb_cell_common* rrc::ue::get_ue_cc_cfg(uint32_t ue_cc_idx)
{
  if (ue_cc_idx >= ue_cell_list.nof_cells()) {
    return nullptr;
  }
  uint32_t enb_cc_idx = ue_cell_list.get_ue_cc_idx(ue_cc_idx)->cell_common->enb_cc_idx;
  return parent->cell_common_list->get_cc_idx(enb_cc_idx);
}

void rrc::ue::update_scells()
{
  const ue_cell_ded*     pcell     = ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
  const enb_cell_common* pcell_cfg = pcell->cell_common;

  if (ue_cell_list.nof_cells() == pcell_cfg->scells.size() + 1) {
    // SCells already added
    return;
  }

  // Check whether UE supports CA
  if (not eutra_capabilities.non_crit_ext_present or not eutra_capabilities.non_crit_ext.non_crit_ext_present or
      not eutra_capabilities.non_crit_ext.non_crit_ext.non_crit_ext_present or
      not eutra_capabilities.non_crit_ext.non_crit_ext.non_crit_ext.rf_params_v1020_present or
      eutra_capabilities.non_crit_ext.non_crit_ext.non_crit_ext.rf_params_v1020.supported_band_combination_r10.size() ==
          0) {
    parent->logger.info("UE doesn't support CA. Skipping SCell activation");
    return;
  }

  for (const enb_cell_common* scell : pcell_cfg->scells) {
    ue_cell_list.add_cell(scell->enb_cc_idx);
  }

  parent->logger.info("SCells activated for rnti=0x%x", rnti);
}

/********************** HELPERS ***************************/

void rrc::ue::send_dl_ccch(dl_ccch_msg_s* dl_ccch_msg, std::string* octet_str)
{
  // Allocate a new PDU buffer, pack the message and send to PDCP
  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu) {
    asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
    if (dl_ccch_msg->pack(bref) != asn1::SRSASN_SUCCESS) {
      parent->logger.error(pdu->msg, pdu->N_bytes, "Failed to pack DL-CCCH-Msg:");
      return;
    }
    pdu->N_bytes = (uint32_t)bref.distance_bytes();

    char buf[32] = {};
    sprintf(buf, "SRB0 - rnti=0x%x", rnti);
    parent->log_rrc_message(buf, Tx, pdu.get(), *dl_ccch_msg, dl_ccch_msg->msg.c1().type().to_string());

    // Encode the pdu as an octet string if the user passed a valid pointer.
    if (octet_str) {
      *octet_str = asn1::octstring_to_string(pdu->msg, pdu->N_bytes);
    }

    parent->rlc->write_sdu(rnti, srb_to_lcid(lte_srb::srb0), std::move(pdu));
  } else {
    parent->logger.error("Allocating pdu");
  }
}

bool rrc::ue::send_dl_dcch(const dl_dcch_msg_s* dl_dcch_msg, srsran::unique_byte_buffer_t pdu, std::string* octet_str)
{
  if (!pdu) {
    pdu = srsran::make_byte_buffer();
  }
  if (pdu) {
    asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
    if (dl_dcch_msg->pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
      parent->logger.error("Failed to encode DL-DCCH-Msg");
      return false;
    }
    pdu->N_bytes = (uint32_t)bref.distance_bytes();

    lte_srb rb = lte_srb::srb1;
    if (dl_dcch_msg->msg.c1().type() == dl_dcch_msg_type_c::c1_c_::types_opts::dl_info_transfer) {
      // send messages with NAS on SRB2 if user is fully registered (after RRC reconfig complete)
      rb = (parent->rlc->has_bearer(rnti, srb_to_lcid(lte_srb::srb2)) && state == RRC_STATE_REGISTERED) ? lte_srb::srb2
                                                                                                        : lte_srb::srb1;
    }

    char buf[32] = {};
    sprintf(buf, "%s - rnti=0x%x", srsran::get_srb_name(rb), rnti);
    parent->log_rrc_message(buf, Tx, pdu.get(), *dl_dcch_msg, dl_dcch_msg->msg.c1().type().to_string());

    // Encode the pdu as an octet string if the user passed a valid pointer.
    if (octet_str) {
      *octet_str = asn1::octstring_to_string(pdu->msg, pdu->N_bytes);
    }

    parent->pdcp->write_sdu(rnti, srb_to_lcid(rb), std::move(pdu));
  } else {
    parent->logger.error("Allocating pdu");
    return false;
  }
  return true;
}

void rrc::ue::apply_setup_phy_common(const asn1::rrc::rr_cfg_common_sib_s& config, bool update_phy)
{
  // Return if no cell is supported
  if (phy_rrc_dedicated_list.empty()) {
    return;
  }

  // Flatten common configuration
  auto& current_phy_cfg = phy_rrc_dedicated_list[0].phy_cfg;
  set_phy_cfg_t_common_prach(&current_phy_cfg, &config.prach_cfg.prach_cfg_info, config.prach_cfg.root_seq_idx);
  set_phy_cfg_t_common_pdsch(&current_phy_cfg, config.pdsch_cfg_common);
  set_phy_cfg_t_common_pusch(&current_phy_cfg, config.pusch_cfg_common);
  set_phy_cfg_t_common_pucch(&current_phy_cfg, config.pucch_cfg_common);
  set_phy_cfg_t_common_srs(&current_phy_cfg, config.srs_ul_cfg_common);
  set_phy_cfg_t_common_pwr_ctrl(&current_phy_cfg, config.ul_pwr_ctrl_common);

  // Set PCell index
  phy_rrc_dedicated_list[0].configured = true;
  phy_rrc_dedicated_list[0].enb_cc_idx = get_ue_cc_cfg(UE_PCELL_CC_IDX)->enb_cc_idx;

  // Send configuration to physical layer
  if (parent->phy != nullptr and update_phy) {
    parent->phy->set_config(rnti, phy_rrc_dedicated_list);
  }
}

void rrc::ue::apply_setup_phy_config_dedicated(const asn1::rrc::phys_cfg_ded_s& phys_cfg_ded)
{
  // Return if no cell is supported
  if (phy_rrc_dedicated_list.empty()) {
    return;
  }

  // Load PCell dedicated configuration
  srsran::set_phy_cfg_t_dedicated_cfg(&phy_rrc_dedicated_list[0].phy_cfg, phys_cfg_ded);

  // Deactivates eNb/Cells for this UE
  for (uint32_t cc = 1; cc < phy_rrc_dedicated_list.size(); cc++) {
    phy_rrc_dedicated_list[cc].configured = false;
  }

  // Send configuration to physical layer
  if (parent->phy != nullptr) {
    parent->phy->set_config(rnti, phy_rrc_dedicated_list);
  }
}

void rrc::ue::apply_reconf_phy_config(const rrc_conn_recfg_r8_ies_s& reconfig_r8, bool update_phy)
{
  // Return if no cell is supported
  if (phy_rrc_dedicated_list.empty()) {
    return;
  }

  // Configure PCell if available configuration
  if (reconfig_r8.rr_cfg_ded_present) {
    auto& rr_cfg_ded = reconfig_r8.rr_cfg_ded;
    if (rr_cfg_ded.phys_cfg_ded_present) {
      auto& phys_cfg_ded = rr_cfg_ded.phys_cfg_ded;
      srsran::set_phy_cfg_t_dedicated_cfg(&phy_rrc_dedicated_list[0].phy_cfg, phys_cfg_ded);
      srsran::set_phy_cfg_t_enable_64qam(
          &phy_rrc_dedicated_list[0].phy_cfg,
          ue_capabilities.support_ul_64qam and
              parent->cfg.sibs[1].sib2().rr_cfg_common.pusch_cfg_common.pusch_cfg_basic.enable64_qam);
    }
  }

  // Parse extensions
  if (reconfig_r8.non_crit_ext_present) {
    auto& reconfig_r890 = reconfig_r8.non_crit_ext;
    if (reconfig_r890.non_crit_ext_present) {
      auto& reconfig_r920 = reconfig_r890.non_crit_ext;
      if (reconfig_r920.non_crit_ext_present) {
        auto& reconfig_r1020 = reconfig_r920.non_crit_ext;

        // Handle Add/Modify SCell list
        if (reconfig_r1020.scell_to_add_mod_list_r10_present) {
          auto& list = reconfig_r1020.scell_to_add_mod_list_r10;
          phy_rrc_dedicated_list.resize(ue_cell_list.nof_cells());
          for (const scell_to_add_mod_r10_s& scell : list) {
            ue_cell_ded* ue_cc = ue_cell_list.get_ue_cc_idx(scell.scell_idx_r10);
            // Create new PHY configuration structure for this SCell
            phy_interface_rrc_lte::phy_rrc_cfg_t scell_phy_rrc_ded = {};
            srsran::set_phy_cfg_t_scell_config(&scell_phy_rrc_ded.phy_cfg, scell);
            scell_phy_rrc_ded.configured = true;

            // Set PUSCH dedicated configuration following 3GPP TS 36.331 R 10 Section 6.3.2 Radio resource control
            // information elements - PUSCH-Config
            //   One value applies for all serving cells with an uplink (the associated functionality is common i.e. not
            //   performed independently for each cell).
            scell_phy_rrc_ded.phy_cfg.ul_cfg.pusch.uci_offset =
                phy_rrc_dedicated_list[0].phy_cfg.ul_cfg.pusch.uci_offset;

            // Get corresponding eNB CC index
            scell_phy_rrc_ded.enb_cc_idx = ue_cc->cell_common->enb_cc_idx;

            // Append to PHY RRC config dedicated which will be applied further down
            phy_rrc_dedicated_list[scell.scell_idx_r10] = scell_phy_rrc_ded;
            srsran::set_phy_cfg_t_enable_64qam(
                &phy_rrc_dedicated_list[scell.scell_idx_r10].phy_cfg,
                ue_capabilities.support_ul_64qam and
                    parent->cfg.sibs[1].sib2().rr_cfg_common.pusch_cfg_common.pusch_cfg_basic.enable64_qam);
          }
        }
      }
    }
  }

  // Send configuration to physical layer
  if (parent->phy != nullptr and update_phy) {
    parent->phy->set_config(rnti, phy_rrc_dedicated_list);
  }
}

void rrc::ue::apply_pdcp_srb_updates(const rr_cfg_ded_s& pending_rr_cfg)
{
  for (const srb_to_add_mod_s& srb : pending_rr_cfg.srb_to_add_mod_list) {
    parent->pdcp->add_bearer(rnti, srb.srb_id, srsran::make_srb_pdcp_config_t(srb.srb_id, false));

    // enable security config
    if (ue_security_cfg.is_as_sec_cfg_valid()) {
      parent->pdcp->config_security(rnti, srb.srb_id, ue_security_cfg.get_as_sec_cfg());
      parent->pdcp->enable_integrity(rnti, srb.srb_id);
      parent->pdcp->enable_encryption(rnti, srb.srb_id);
    }
  }
}

void rrc::ue::apply_pdcp_drb_updates(const rr_cfg_ded_s& pending_rr_cfg)
{
  for (uint8_t drb_id : pending_rr_cfg.drb_to_release_list) {
    parent->pdcp->del_bearer(rnti, drb_id + 2);
  }
  for (const drb_to_add_mod_s& drb : pending_rr_cfg.drb_to_add_mod_list) {
    // Configure DRB1 in PDCP
    if (drb.pdcp_cfg_present) {
      srsran::pdcp_config_t pdcp_cnfg_drb = srsran::make_drb_pdcp_config_t(drb.drb_id, false, drb.pdcp_cfg);
      parent->pdcp->add_bearer(rnti, drb.lc_ch_id, pdcp_cnfg_drb);
    } else {
      srsran::pdcp_config_t pdcp_cnfg_drb = srsran::make_drb_pdcp_config_t(drb.drb_id, false);
      parent->pdcp->add_bearer(rnti, drb.lc_ch_id, pdcp_cnfg_drb);
    }

    if (ue_security_cfg.is_as_sec_cfg_valid()) {
      parent->pdcp->config_security(rnti, drb.lc_ch_id, ue_security_cfg.get_as_sec_cfg());
      parent->pdcp->enable_integrity(rnti, drb.lc_ch_id);
      parent->pdcp->enable_encryption(rnti, drb.lc_ch_id);
    }
  }

  // If reconf due to reestablishment, recover PDCP state
  if (state == RRC_STATE_REESTABLISHMENT_COMPLETE) {
    for (const auto& erab_pair : bearer_list.get_erabs()) {
      uint16_t lcid  = erab_pair.second.id - 2;
      bool     is_am = parent->cfg.qci_cfg[erab_pair.second.qos_params.qci].rlc_cfg.type().value ==
                   asn1::rrc::rlc_cfg_c::types_opts::am;
      if (is_am) {
        parent->logger.debug("Set PDCP state: TX HFN %d, NEXT_PDCP_TX_SN %d, RX_HFN %d, NEXT_PDCP_RX_SN %d, "
                             "LAST_SUBMITTED_PDCP_RX_SN %d",
                             old_reest_pdcp_state[lcid].tx_hfn,
                             old_reest_pdcp_state[lcid].next_pdcp_tx_sn,
                             old_reest_pdcp_state[lcid].rx_hfn,
                             old_reest_pdcp_state[lcid].next_pdcp_rx_sn,
                             old_reest_pdcp_state[lcid].last_submitted_pdcp_rx_sn);
        parent->pdcp->set_bearer_state(rnti, lcid, old_reest_pdcp_state[lcid]);
        parent->pdcp->set_bearer_state(rnti, lcid, old_reest_pdcp_state[lcid]);
        if (parent->cfg.qci_cfg[erab_pair.second.qos_params.qci].pdcp_cfg.rlc_am.status_report_required) {
          parent->pdcp->send_status_report(rnti, lcid);
        }
      }
    }
  }
}

void rrc::ue::apply_rlc_rb_updates(const rr_cfg_ded_s& pending_rr_cfg)
{
  for (const srb_to_add_mod_s& srb : pending_rr_cfg.srb_to_add_mod_list) {
    parent->rlc->add_bearer(rnti, srb.srb_id, srsran::rlc_config_t::srb_config(srb.srb_id));
  }
  if (pending_rr_cfg.drb_to_release_list.size() > 0) {
    for (uint8_t drb_id : pending_rr_cfg.drb_to_release_list) {
      parent->rlc->del_bearer(rnti, drb_id + 2);
    }
  }
  for (const drb_to_add_mod_s& drb : pending_rr_cfg.drb_to_add_mod_list) {
    if (not drb.rlc_cfg_present) {
      parent->logger.warning("Default RLC DRB config not supported");
    }
    parent->rlc->add_bearer(rnti, drb.lc_ch_id, srsran::make_rlc_config_t(drb.rlc_cfg));
  }
}

int rrc::ue::get_cqi(uint16_t* pmi_idx, uint16_t* n_pucch, uint32_t ue_cc_idx)
{
  ue_cell_ded* c = ue_cell_list.get_ue_cc_idx(ue_cc_idx);
  if (c != nullptr and c->cqi_res_present) {
    *pmi_idx = c->cqi_res.pmi_idx;
    *n_pucch = c->cqi_res.pucch_res;
    return SRSRAN_SUCCESS;
  } else {
    parent->logger.error("CQI resources for ue_cc_idx=%d have not been allocated", ue_cc_idx);
    return SRSRAN_ERROR;
  }
}

bool rrc::ue::is_allocated() const
{
  return ue_cell_list.is_allocated();
}

int rrc::ue::get_ri(uint32_t m_ri, uint16_t* ri_idx)
{
  int32_t ret = SRSRAN_SUCCESS;

  uint32_t I_ri        = 0;
  int32_t  N_offset_ri = 0; // Naivest approach: overlap RI with PMI
  switch (m_ri) {
    case 0:
      // Disabled
      break;
    case 1:
      I_ri = -N_offset_ri;
      break;
    case 2:
      I_ri = 161 - N_offset_ri;
      break;
    case 4:
      I_ri = 322 - N_offset_ri;
      break;
    case 8:
      I_ri = 483 - N_offset_ri;
      break;
    case 16:
      I_ri = 644 - N_offset_ri;
      break;
    case 32:
      I_ri = 805 - N_offset_ri;
      break;
    default:
      parent->logger.error("Allocating RI: invalid m_ri=%d", m_ri);
  }

  // If ri_dix is available, copy
  if (ri_idx) {
    *ri_idx = I_ri;
  }

  return ret;
}

} // namespace srsenb
