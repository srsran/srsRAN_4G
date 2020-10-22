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

#include "srsenb/hdr/stack/rrc/rrc_ue.h"
#include "srsenb/hdr/stack/rrc/mac_controller.h"
#include "srsenb/hdr/stack/rrc/rrc_mobility.h"
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/common/int_helpers.h"

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
  pool(srslte::byte_buffer_pool::get_instance()),
  phy_rrc_dedicated_list(sched_ue_cfg.supported_cc_list.size()),
  cell_ded_list(parent->cfg, *outer_rrc->pucch_res_list, *outer_rrc->cell_common_list),
  bearer_list(rnti_, parent->cfg),
  ue_security_cfg(parent->cfg)
{
  mac_ctrl.reset(new mac_controller{this, sched_ue_cfg});

  // Allocate cell and PUCCH resources
  if (cell_ded_list.add_cell(sched_ue_cfg.supported_cc_list[0].enb_cc_idx) == nullptr) {
    return;
  }

  // Configure
  apply_setup_phy_common(parent->cfg.sibs[1].sib2().rr_cfg_common, true);

  activity_timer = outer_rrc->task_sched.get_unique_timer();
  set_activity_timeout(MSG3_RX_TIMEOUT); // next UE response is Msg3

  mobility_handler.reset(new rrc_mobility(this));
}

rrc::ue::~ue() {}

rrc_state_t rrc::ue::get_state()
{
  return state;
}

void rrc::ue::set_activity()
{
  // re-start activity timer with current timeout value
  activity_timer.run();

  if (parent && parent->rrc_log) {
    parent->rrc_log->debug("Activity registered for rnti=0x%x (timeout_value=%dms)\n", rnti, activity_timer.duration());
  }
}

void rrc::ue::activity_timer_expired()
{
  if (parent) {
    if (parent->rrc_log) {
      parent->rrc_log->warning(
          "Activity timer for rnti=0x%x expired after %d ms\n", rnti, activity_timer.time_elapsed());
    }

    if (parent->s1ap->user_exists(rnti)) {
      parent->s1ap->user_release(rnti, asn1::s1ap::cause_radio_network_opts::user_inactivity);
    } else {
      if (rnti != SRSLTE_MRNTI) {
        parent->rem_user_thread(rnti);
      }
    }
  }

  state = RRC_STATE_RELEASE_REQUEST;
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
      parent->rrc_log->error("Unknown timeout type %d", type);
  }

  uint32_t deadline = deadline_s * 1e3 + deadline_ms;
  activity_timer.set(deadline, [this](uint32_t tid) { activity_timer_expired(); });
  parent->rrc_log->debug("Setting timer for %s for rnti=0x%x to %dms\n", to_string(type).c_str(), rnti, deadline);

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

void rrc::ue::parse_ul_dcch(uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  set_activity();

  ul_dcch_msg_s  ul_dcch_msg;
  asn1::cbit_ref bref(pdu->msg, pdu->N_bytes);
  if (ul_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
      ul_dcch_msg.msg.type().value != ul_dcch_msg_type_c::types_opts::c1) {
    parent->rrc_log->error("Failed to unpack UL-DCCH message\n");
    return;
  }

  parent->log_rrc_message(
      srsenb::to_string((rb_id_t)lcid), Rx, pdu.get(), ul_dcch_msg, ul_dcch_msg.msg.c1().type().to_string());

  // reuse PDU
  pdu->clear(); // TODO: name collision with byte_buffer reset

  transaction_id = 0;

  switch (ul_dcch_msg.msg.c1().type()) {
    case ul_dcch_msg_type_c::c1_c_::types::rrc_conn_setup_complete:
      handle_rrc_con_setup_complete(&ul_dcch_msg.msg.c1().rrc_conn_setup_complete(), std::move(pdu));
      break;
    case ul_dcch_msg_type_c::c1_c_::types::rrc_conn_reest_complete:
      handle_rrc_con_reest_complete(&ul_dcch_msg.msg.c1().rrc_conn_reest_complete(), std::move(pdu));
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
      handle_rrc_reconf_complete(&ul_dcch_msg.msg.c1().rrc_conn_recfg_complete(), std::move(pdu));
      srslte::console("User 0x%x connected\n", rnti);
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
        notify_s1ap_ue_ctxt_setup_complete();
        send_connection_reconf(std::move(pdu));
        state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
      } else {
        send_connection_reject();
        state = RRC_STATE_IDLE;
      }
      break;
    case ul_dcch_msg_type_c::c1_c_::types::meas_report:
      if (mobility_handler != nullptr) {
        mobility_handler->handle_ue_meas_report(ul_dcch_msg.msg.c1().meas_report());
      } else {
        parent->rrc_log->warning("Received MeasReport but no mobility configuration is available\n");
      }
      break;
    default:
      parent->rrc_log->error("Msg: %s not supported\n", ul_dcch_msg.msg.c1().type().to_string().c_str());
      break;
  }
}

std::string rrc::ue::to_string(const activity_timeout_type_t& type)
{
  constexpr static const char* options[] = {"Msg3 reception", "UE response reception", "UE inactivity"};
  return srslte::enum_to_text(options, (uint32_t)activity_timeout_type_t::nulltype, (uint32_t)type);
}

/*
 *  Connection Setup
 */
void rrc::ue::handle_rrc_con_req(rrc_conn_request_s* msg)
{
  if (not parent->s1ap->is_mme_connected()) {
    parent->rrc_log->error("MME isn't connected. Sending Connection Reject\n");
    send_connection_reject();
    return;
  }

  rrc_conn_request_r8_ies_s* msg_r8 = &msg->crit_exts.rrc_conn_request_r8();

  if (msg_r8->ue_id.type() == init_ue_id_c::types::s_tmsi) {
    mmec     = (uint8_t)msg_r8->ue_id.s_tmsi().mmec.to_number();
    m_tmsi   = (uint32_t)msg_r8->ue_id.s_tmsi().m_tmsi.to_number();
    has_tmsi = true;
  }
  establishment_cause = msg_r8->establishment_cause;
  send_connection_setup();
  state = RRC_STATE_WAIT_FOR_CON_SETUP_COMPLETE;

  set_activity_timeout(UE_INACTIVITY_TIMEOUT);
}

void rrc::ue::send_connection_setup()
{
  // (Re-)Establish SRB1
  bearer_list.add_srb(1);

  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set_c1();

  dl_ccch_msg.msg.c1().set_rrc_conn_setup();
  dl_ccch_msg.msg.c1().rrc_conn_setup().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
  rrc_conn_setup_r8_ies_s& setup  = dl_ccch_msg.msg.c1().rrc_conn_setup().crit_exts.set_c1().set_rrc_conn_setup_r8();
  rr_cfg_ded_s*            rr_cfg = &setup.rr_cfg_ded;

  // Fill RR config dedicated
  fill_rrc_setup_rr_config_dedicated(rr_cfg);
  phys_cfg_ded_s* phy_cfg = &rr_cfg->phys_cfg_ded;

  // Apply ConnectionSetup Configuration to MAC scheduler
  mac_ctrl->handle_con_setup(setup);

  // Add SRBs/DRBs, and configure RLC+PDCP
  apply_pdcp_srb_updates();
  apply_pdcp_drb_updates();
  apply_rlc_rb_updates();

  // Configure PHY layer
  apply_setup_phy_config_dedicated(*phy_cfg); // It assumes SCell has not been set before

  send_dl_ccch(&dl_ccch_msg);
}

void rrc::ue::handle_rrc_con_setup_complete(rrc_conn_setup_complete_s* msg, srslte::unique_byte_buffer_t pdu)
{
  // Inform PHY about the configuration completion
  parent->phy->complete_config(rnti);

  parent->rrc_log->info("RRCConnectionSetupComplete transaction ID: %d\n", msg->rrc_transaction_id);
  rrc_conn_setup_complete_r8_ies_s* msg_r8 = &msg->crit_exts.c1().rrc_conn_setup_complete_r8();

  // TODO: msg->selected_plmn_id - used to select PLMN from SIB1 list
  // TODO: if(msg->registered_mme_present) - the indicated MME should be used from a pool

  pdu->N_bytes = msg_r8->ded_info_nas.size();
  memcpy(pdu->msg, msg_r8->ded_info_nas.data(), pdu->N_bytes);

  // Flag completion of RadioResource Configuration
  bearer_list.rr_ded_cfg_complete();

  // Signal MAC scheduler that configuration was successful
  mac_ctrl->handle_con_setup_complete();

  asn1::s1ap::rrc_establishment_cause_e s1ap_cause;
  s1ap_cause.value = (asn1::s1ap::rrc_establishment_cause_opts::options)establishment_cause.value;
  if (has_tmsi) {
    parent->s1ap->initial_ue(rnti, s1ap_cause, std::move(pdu), m_tmsi, mmec);
  } else {
    parent->s1ap->initial_ue(rnti, s1ap_cause, std::move(pdu));
  }
  state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
}

void rrc::ue::send_connection_reject()
{
  mac_ctrl->handle_con_reject();

  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set_c1().set_rrc_conn_reject().crit_exts.set_c1().set_rrc_conn_reject_r8().wait_time = 10;
  send_dl_ccch(&dl_ccch_msg);
}

/*
 * Connection Reestablishment
 */
void rrc::ue::handle_rrc_con_reest_req(rrc_conn_reest_request_s* msg)
{
  if (not parent->s1ap->is_mme_connected()) {
    parent->rrc_log->error("MME isn't connected. Sending Connection Reject\n");
    send_connection_reject();
    return;
  }
  parent->rrc_log->debug("rnti=0x%x, phyid=0x%x, smac=0x%x, cause=%s\n",
                         (uint32_t)msg->crit_exts.rrc_conn_reest_request_r8().ue_id.c_rnti.to_number(),
                         msg->crit_exts.rrc_conn_reest_request_r8().ue_id.pci,
                         (uint32_t)msg->crit_exts.rrc_conn_reest_request_r8().ue_id.short_mac_i.to_number(),
                         msg->crit_exts.rrc_conn_reest_request_r8().reest_cause.to_string().c_str());
  if (is_idle()) {
    uint16_t                old_rnti = msg->crit_exts.rrc_conn_reest_request_r8().ue_id.c_rnti.to_number();
    uint16_t                old_pci  = msg->crit_exts.rrc_conn_reest_request_r8().ue_id.pci;
    const cell_info_common* old_cell = parent->cell_common_list->get_pci(old_pci);
    auto                    ue_it    = parent->users.find(old_rnti);
    // Reject unrecognized rntis, and PCIs that do not belong to eNB
    if (ue_it != parent->users.end() and old_cell != nullptr and
        ue_it->second->cell_ded_list.get_enb_cc_idx(old_cell->enb_cc_idx) != nullptr) {
      parent->rrc_log->info("ConnectionReestablishmentRequest for rnti=0x%x. Sending Connection Reestablishment\n",
                            old_rnti);
      send_connection_reest(parent->users[old_rnti]->ue_security_cfg.get_ncc());

      // Cancel Handover in Target eNB if on-going
      parent->users[old_rnti]->mobility_handler->trigger(rrc_mobility::ho_cancel_ev{});

      // Setup security
      const cell_info_common* pcell_cfg = get_ue_cc_cfg(UE_PCELL_CC_IDX);
      ue_security_cfg                   = parent->users[old_rnti]->ue_security_cfg;
      ue_security_cfg.regenerate_keys_handover(pcell_cfg->cell_cfg.pci, pcell_cfg->cell_cfg.dl_earfcn);

      // Get PDCP entity state (required when using RLC AM)
      for (const auto& erab_pair : parent->users[old_rnti]->bearer_list.get_erabs()) {
        uint16_t lcid              = erab_pair.second.id - 2;
        old_reest_pdcp_state[lcid] = {};
        parent->pdcp->get_bearer_state(old_rnti, lcid, &old_reest_pdcp_state[lcid]);

        parent->rrc_log->debug("Getting PDCP state for E-RAB with LCID %d\n", lcid);
        parent->rrc_log->debug("Got PDCP state: TX HFN %d, NEXT_PDCP_TX_SN %d, RX_HFN %d, NEXT_PDCP_RX_SN %d, "
                               "LAST_SUBMITTED_PDCP_RX_SN %d\n",
                               old_reest_pdcp_state[lcid].tx_hfn,
                               old_reest_pdcp_state[lcid].next_pdcp_tx_sn,
                               old_reest_pdcp_state[lcid].rx_hfn,
                               old_reest_pdcp_state[lcid].next_pdcp_rx_sn,
                               old_reest_pdcp_state[lcid].last_submitted_pdcp_rx_sn);
      }
      // Apply PDCP configuration to SRB1
      apply_pdcp_srb_updates();

      // Make sure UE capabilities are copied over to new RNTI
      eutra_capabilities          = parent->users[old_rnti]->eutra_capabilities;
      eutra_capabilities_unpacked = parent->users[old_rnti]->eutra_capabilities_unpacked;
      ue_capabilities             = parent->users[old_rnti]->ue_capabilities;
      if (parent->rrc_log->get_level() == srslte::LOG_LEVEL_DEBUG) {
        asn1::json_writer js{};
        eutra_capabilities.to_json(js);
        parent->rrc_log->debug_long("rnti=0x%x EUTRA capabilities: %s\n", rnti, js.to_string().c_str());
      }

      old_reest_rnti = old_rnti;
      state          = RRC_STATE_WAIT_FOR_CON_REEST_COMPLETE;
      set_activity_timeout(UE_INACTIVITY_TIMEOUT);
    } else {
      parent->rrc_log->error("Received ConnectionReestablishment for rnti=0x%x without context\n", old_rnti);
      send_connection_reest_rej();
    }
  } else {
    parent->rrc_log->error("Received ReestablishmentRequest from an rnti=0x%x not in IDLE\n", rnti);
  }
}

void rrc::ue::send_connection_reest(uint8_t ncc)
{
  // Re-Establish SRB1
  bearer_list.add_srb(1);

  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set_c1();

  dl_ccch_msg.msg.c1().set_rrc_conn_reest();
  dl_ccch_msg.msg.c1().rrc_conn_reest().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
  rrc_conn_reest_r8_ies_s& reest  = dl_ccch_msg.msg.c1().rrc_conn_reest().crit_exts.set_c1().set_rrc_conn_reest_r8();
  rr_cfg_ded_s*            rr_cfg = &reest.rr_cfg_ded;

  // Fill RR config dedicated
  fill_rrc_setup_rr_config_dedicated(rr_cfg);
  phys_cfg_ded_s* phy_cfg = &rr_cfg->phys_cfg_ded;

  // Set NCC
  reest.next_hop_chaining_count = ncc;

  // Apply ConnectionReest Configuration to MAC scheduler
  mac_ctrl->handle_con_reest(reest);

  // Add SRBs/DRBs, and configure RLC+PDCP
  apply_pdcp_srb_updates();
  apply_pdcp_drb_updates();
  apply_rlc_rb_updates();

  // Configure PHY layer
  apply_setup_phy_config_dedicated(*phy_cfg); // It assumes SCell has not been set before

  send_dl_ccch(&dl_ccch_msg);
}

void rrc::ue::handle_rrc_con_reest_complete(rrc_conn_reest_complete_s* msg, srslte::unique_byte_buffer_t pdu)
{
  // Inform PHY about the configuration completion
  parent->phy->complete_config(rnti);

  parent->rrc_log->info("RRCConnectionReestablishComplete transaction ID: %d\n", msg->rrc_transaction_id);

  // TODO: msg->selected_plmn_id - used to select PLMN from SIB1 list
  // TODO: if(msg->registered_mme_present) - the indicated MME should be used from a pool

  // Modify GTP-U tunnel and S1AP context
  parent->gtpu->mod_bearer_rnti(old_reest_rnti, rnti);
  parent->s1ap->user_mod(old_reest_rnti, rnti);

  // Flag completion of RadioResource Configuration
  bearer_list.rr_ded_cfg_complete();

  // Signal MAC scheduler that configuration was successful
  mac_ctrl->handle_con_reest_complete();

  // Activate security for SRB1
  parent->pdcp->config_security(rnti, RB_ID_SRB1, ue_security_cfg.get_as_sec_cfg());
  parent->pdcp->enable_integrity(rnti, RB_ID_SRB1);
  parent->pdcp->enable_encryption(rnti, RB_ID_SRB1);

  // Reestablish current DRBs during ConnectionReconfiguration
  for (const auto& erab_pair : parent->users[old_reest_rnti]->bearer_list.get_erabs()) {
    const bearer_cfg_handler::erab_t& erab = erab_pair.second;
    bearer_list.add_erab(erab.id, erab.qos_params, erab.address, erab.teid_out, nullptr);
  }

  // remove old RNTI
  parent->rem_user_thread(old_reest_rnti);

  state = RRC_STATE_REESTABLISHMENT_COMPLETE;
  send_connection_reconf(std::move(pdu));
}

void rrc::ue::send_connection_reest_rej()
{
  mac_ctrl->handle_con_reject();

  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set_c1().set_rrc_conn_reest_reject().crit_exts.set_rrc_conn_reest_reject_r8();
  send_dl_ccch(&dl_ccch_msg);
}

/*
 * Connection Reconfiguration
 */
void rrc::ue::send_connection_reconf(srslte::unique_byte_buffer_t pdu)
{
  // Setup SRB2
  bearer_list.add_srb(2);

  // Add re-establish DRBs
  parent->rrc_log->debug("RRC state %d\n", state);
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_conn_recfg().crit_exts.set_c1().set_rrc_conn_recfg_r8();
  dl_dcch_msg.msg.c1().rrc_conn_recfg().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);

  rrc_conn_recfg_r8_ies_s* conn_reconf = &dl_dcch_msg.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();

  // Add DRBs/SRBs
  conn_reconf->rr_cfg_ded_present = bearer_list.fill_rr_cfg_ded(conn_reconf->rr_cfg_ded);

  conn_reconf->rr_cfg_ded.phys_cfg_ded_present = true;
  phys_cfg_ded_s* phy_cfg                      = &conn_reconf->rr_cfg_ded.phys_cfg_ded;

  // Configure PHY layer
  phy_cfg->ant_info_present              = true;
  phy_cfg->ant_info.set_explicit_value() = parent->cfg.antenna_info;
  phy_cfg->cqi_report_cfg_present        = true;
  if (parent->cfg.cqi_cfg.mode == RRC_CFG_CQI_MODE_APERIODIC) {
    phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic_present = true;
    if (phy_cfg->ant_info_present and
        phy_cfg->ant_info.explicit_value().tx_mode.value == ant_info_ded_s::tx_mode_e_::tm4) {
      phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic = cqi_report_mode_aperiodic_e::rm31;
    } else {
      phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic = cqi_report_mode_aperiodic_e::rm30;
    }
  } else {
    phy_cfg->cqi_report_cfg.cqi_report_periodic_present = true;
    auto& cqi_rep                                       = phy_cfg->cqi_report_cfg.cqi_report_periodic.set_setup();
    get_cqi(&cqi_rep.cqi_pmi_cfg_idx, &cqi_rep.cqi_pucch_res_idx, UE_PCELL_CC_IDX);
    cqi_rep.cqi_format_ind_periodic.set(
        cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types::wideband_cqi);
    cqi_rep.simul_ack_nack_and_cqi = parent->cfg.cqi_cfg.simultaneousAckCQI;
    if (phy_cfg->ant_info_present and
        ((phy_cfg->ant_info.explicit_value().tx_mode == ant_info_ded_s::tx_mode_e_::tm3) ||
         (phy_cfg->ant_info.explicit_value().tx_mode == ant_info_ded_s::tx_mode_e_::tm4))) {
      uint16_t ri_idx = 0;
      if (get_ri(parent->cfg.cqi_cfg.m_ri, &ri_idx) == SRSLTE_SUCCESS) {
        phy_cfg->cqi_report_cfg.cqi_report_periodic.set_setup();
        phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx_present = true;
        phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx         = ri_idx;
      } else {
        srslte::console("\nWarning: Configured wrong M_ri parameter.\n\n");
      }
    } else {
      phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx_present = false;
    }
  }
  phy_cfg->cqi_report_cfg.nom_pdsch_rs_epre_offset = 0;
  // PDSCH
  phy_cfg->pdsch_cfg_ded_present = true;
  phy_cfg->pdsch_cfg_ded.p_a     = parent->cfg.pdsch_cfg;
  // 256-QAM
  if (ue_capabilities.support_dl_256qam) {
    phy_cfg->ext = true;
    phy_cfg->cqi_report_cfg_pcell_v1250.set_present(true);
    phy_cfg->cqi_report_cfg_pcell_v1250->alt_cqi_table_r12_present = true;
    phy_cfg->cqi_report_cfg_pcell_v1250->alt_cqi_table_r12.value =
        cqi_report_cfg_v1250_s::alt_cqi_table_r12_opts::all_sfs;
  }

  // Add SCells
  if (fill_scell_to_addmod_list(conn_reconf) != SRSLTE_SUCCESS) {
    parent->rrc_log->warning("Could not create configuration for Scell\n");
    return;
  }

  apply_reconf_phy_config(*conn_reconf, true);

  // setup SRB2/DRBs in PDCP and RLC
  apply_pdcp_srb_updates();
  apply_pdcp_drb_updates();
  apply_rlc_rb_updates();

  // Add pending NAS info
  bearer_list.fill_pending_nas_info(conn_reconf);

  if (mobility_handler != nullptr) {
    mobility_handler->fill_conn_recfg_no_ho_cmd(conn_reconf);
  }
  last_rrc_conn_recfg = dl_dcch_msg.msg.c1().rrc_conn_recfg();

  // Apply Reconf Msg configuration to MAC scheduler
  mac_ctrl->handle_con_reconf(*conn_reconf);

  // If reconf due to reestablishment, recover PDCP state
  if (state == RRC_STATE_REESTABLISHMENT_COMPLETE) {
    for (const auto& erab_pair : bearer_list.get_erabs()) {
      uint16_t lcid  = erab_pair.second.id - 2;
      bool     is_am = parent->cfg.qci_cfg[erab_pair.second.qos_params.qci].rlc_cfg.type().value ==
                   asn1::rrc::rlc_cfg_c::types_opts::am;
      if (is_am) {
        parent->rrc_log->debug("Set PDCP state: TX HFN %d, NEXT_PDCP_TX_SN %d, RX_HFN %d, NEXT_PDCP_RX_SN %d, "
                               "LAST_SUBMITTED_PDCP_RX_SN %d\n",
                               old_reest_pdcp_state[lcid].tx_hfn,
                               old_reest_pdcp_state[lcid].next_pdcp_tx_sn,
                               old_reest_pdcp_state[lcid].rx_hfn,
                               old_reest_pdcp_state[lcid].next_pdcp_rx_sn,
                               old_reest_pdcp_state[lcid].last_submitted_pdcp_rx_sn);
        parent->pdcp->set_bearer_state(rnti, lcid, old_reest_pdcp_state[lcid]);
      }
    }
  }

  // Reuse same PDU
  pdu->clear();

  send_dl_dcch(&dl_dcch_msg, std::move(pdu));

  state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
}

void rrc::ue::send_connection_reconf_upd(srslte::unique_byte_buffer_t pdu)
{
  dl_dcch_msg_s     dl_dcch_msg;
  rrc_conn_recfg_s* rrc_conn_recfg     = &dl_dcch_msg.msg.set_c1().set_rrc_conn_recfg();
  rrc_conn_recfg->rrc_transaction_id   = (uint8_t)((transaction_id++) % 4);
  rrc_conn_recfg_r8_ies_s& reconfig_r8 = rrc_conn_recfg->crit_exts.set_c1().set_rrc_conn_recfg_r8();

  reconfig_r8.rr_cfg_ded_present = true;
  rr_cfg_ded_s* rr_cfg           = &reconfig_r8.rr_cfg_ded;

  rr_cfg->phys_cfg_ded_present       = true;
  phys_cfg_ded_s* phy_cfg            = &rr_cfg->phys_cfg_ded;
  phy_cfg->sched_request_cfg_present = true;
  phy_cfg->sched_request_cfg.set_setup();
  phy_cfg->sched_request_cfg.setup().dsr_trans_max = parent->cfg.sr_cfg.dsr_max;

  phy_cfg->cqi_report_cfg_present = true;
  if (cell_ded_list.nof_cells() > 0) {
    phy_cfg->cqi_report_cfg.cqi_report_periodic_present = true;
    phy_cfg->cqi_report_cfg.cqi_report_periodic.set_setup().cqi_format_ind_periodic.set(
        cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types::wideband_cqi);
    get_cqi(&phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pmi_cfg_idx,
            &phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pucch_res_idx,
            UE_PCELL_CC_IDX);
    phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().simul_ack_nack_and_cqi = parent->cfg.cqi_cfg.simultaneousAckCQI;
    if (parent->cfg.antenna_info.tx_mode == ant_info_ded_s::tx_mode_e_::tm3 ||
        parent->cfg.antenna_info.tx_mode == ant_info_ded_s::tx_mode_e_::tm4) {
      phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx_present = true;
      phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx = 483; /* TODO: HARDCODED! Add to UL scheduler */
    } else {
      phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx_present = false;
    }
  } else {
    phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic_present = true;
    if (phy_cfg->ant_info_present && parent->cfg.antenna_info.tx_mode == ant_info_ded_s::tx_mode_e_::tm4) {
      phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic = cqi_report_mode_aperiodic_e::rm31;
    } else {
      phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic = cqi_report_mode_aperiodic_e::rm30;
    }
  }
  apply_reconf_phy_config(reconfig_r8, true);

  phy_cfg->sched_request_cfg.setup().sr_cfg_idx       = cell_ded_list.get_sr_res()->sr_I;
  phy_cfg->sched_request_cfg.setup().sr_pucch_res_idx = cell_ded_list.get_sr_res()->sr_N_pucch;

  // Apply Reconf Msg configuration to MAC scheduler
  mac_ctrl->handle_con_reconf(reconfig_r8);

  pdu->clear();

  send_dl_dcch(&dl_dcch_msg, std::move(pdu));

  state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
}

void rrc::ue::send_connection_reconf_new_bearer()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_conn_recfg().crit_exts.set_c1().set_rrc_conn_recfg_r8();
  dl_dcch_msg.msg.c1().rrc_conn_recfg().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
  rrc_conn_recfg_r8_ies_s* conn_reconf = &dl_dcch_msg.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();

  conn_reconf->rr_cfg_ded_present = bearer_list.fill_rr_cfg_ded(conn_reconf->rr_cfg_ded);

  // Setup new bearer
  apply_pdcp_srb_updates();
  apply_pdcp_drb_updates();
  apply_rlc_rb_updates();
  // Add pending NAS info
  bearer_list.fill_pending_nas_info(conn_reconf);

  if (conn_reconf->rr_cfg_ded_present or conn_reconf->ded_info_nas_list_present) {
    send_dl_dcch(&dl_dcch_msg);
  }
}

void rrc::ue::handle_rrc_reconf_complete(rrc_conn_recfg_complete_s* msg, srslte::unique_byte_buffer_t pdu)
{
  // Inform PHY about the configuration completion
  parent->phy->complete_config(rnti);

  if (last_rrc_conn_recfg.rrc_transaction_id == msg->rrc_transaction_id) {
    // Flag completion of RadioResource Configuration
    bearer_list.rr_ded_cfg_complete();

    // Activate SCells and bearers in the MAC scheduler that were advertised in the RRC Reconf message
    mac_ctrl->handle_con_reconf_complete();

    // If performing handover, signal its completion
    mobility_handler->trigger(*msg);
  } else {
    parent->rrc_log->error("Expected RRCReconfigurationComplete with transaction ID: %d, got %d\n",
                           last_rrc_conn_recfg.rrc_transaction_id,
                           msg->rrc_transaction_id);
  }
}

/*
 * Security Mode command
 */
void rrc::ue::send_security_mode_command()
{
  // Setup SRB1 security/integrity. Encryption is set on completion
  parent->pdcp->config_security(rnti, RB_ID_SRB1, ue_security_cfg.get_as_sec_cfg());
  parent->pdcp->enable_integrity(rnti, RB_ID_SRB1);

  dl_dcch_msg_s        dl_dcch_msg;
  security_mode_cmd_s* comm = &dl_dcch_msg.msg.set_c1().set_security_mode_cmd();
  comm->rrc_transaction_id  = (uint8_t)((transaction_id++) % 4);

  comm->crit_exts.set_c1().set_security_mode_cmd_r8().security_cfg_smc.security_algorithm_cfg =
      ue_security_cfg.get_security_algorithm_cfg();

  send_dl_dcch(&dl_dcch_msg);
}

void rrc::ue::handle_security_mode_complete(security_mode_complete_s* msg)
{
  parent->rrc_log->info("SecurityModeComplete transaction ID: %d\n", msg->rrc_transaction_id);

  parent->pdcp->enable_encryption(rnti, RB_ID_SRB1);
}

void rrc::ue::handle_security_mode_failure(security_mode_fail_s* msg)
{
  parent->rrc_log->info("SecurityModeFailure transaction ID: %d\n", msg->rrc_transaction_id);
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
  parent->rrc_log->info("UECapabilityInformation transaction ID: %d\n", msg->rrc_transaction_id);
  ue_cap_info_r8_ies_s* msg_r8 = &msg->crit_exts.c1().ue_cap_info_r8();

  for (uint32_t i = 0; i < msg_r8->ue_cap_rat_container_list.size(); i++) {
    if (msg_r8->ue_cap_rat_container_list[i].rat_type != rat_type_e::eutra) {
      parent->rrc_log->warning("Not handling UE capability information for RAT type %s\n",
                               msg_r8->ue_cap_rat_container_list[i].rat_type.to_string().c_str());
    } else {
      asn1::cbit_ref bref(msg_r8->ue_cap_rat_container_list[0].ue_cap_rat_container.data(),
                          msg_r8->ue_cap_rat_container_list[0].ue_cap_rat_container.size());
      if (eutra_capabilities.unpack(bref) != asn1::SRSASN_SUCCESS) {
        parent->rrc_log->error("Failed to unpack EUTRA capabilities message\n");
        return false;
      }
      if (parent->rrc_log->get_level() == srslte::LOG_LEVEL_DEBUG) {
        asn1::json_writer js{};
        eutra_capabilities.to_json(js);
        parent->rrc_log->debug_long("rnti=0x%x EUTRA capabilities: %s\n", rnti, js.to_string().c_str());
      }
      eutra_capabilities_unpacked = true;
      ue_capabilities             = srslte::make_rrc_ue_capabilities(eutra_capabilities);

      parent->rrc_log->info("UE rnti: 0x%x category: %d\n", rnti, eutra_capabilities.ue_category);
    }
  }

  return true;

  // TODO: Add liblte_rrc support for unpacking UE cap info and repacking into
  //       inter-node UERadioAccessCapabilityInformation (36.331 v10.0.0 Section 10.2.2).
  //       This is then passed to S1AP for transfer to EPC.
  // parent->s1ap->ue_capabilities(rnti, &eutra_capabilities);
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

  send_dl_dcch(&dl_dcch_msg);
}

/*
 * UE context
 */
void rrc::ue::handle_ue_init_ctxt_setup_req(const asn1::s1ap::init_context_setup_request_s& msg)
{
  if (msg.protocol_ies.add_cs_fallback_ind_present) {
    parent->rrc_log->warning("Not handling AdditionalCSFallbackIndicator\n");
  }
  if (msg.protocol_ies.csg_membership_status_present) {
    parent->rrc_log->warning("Not handling CSGMembershipStatus\n");
  }
  if (msg.protocol_ies.gummei_id_present) {
    parent->rrc_log->warning("Not handling GUMMEI_ID\n");
  }
  if (msg.protocol_ies.ho_restrict_list_present) {
    parent->rrc_log->warning("Not handling HandoverRestrictionList\n");
  }
  if (msg.protocol_ies.management_based_mdt_allowed_present) {
    parent->rrc_log->warning("Not handling ManagementBasedMDTAllowed\n");
  }
  if (msg.protocol_ies.management_based_mdtplmn_list_present) {
    parent->rrc_log->warning("Not handling ManagementBasedMDTPLMNList\n");
  }
  if (msg.protocol_ies.mme_ue_s1ap_id_minus2_present) {
    parent->rrc_log->warning("Not handling MME_UE_S1AP_ID_2\n");
  }
  if (msg.protocol_ies.registered_lai_present) {
    parent->rrc_log->warning("Not handling RegisteredLAI\n");
  }
  if (msg.protocol_ies.srvcc_operation_possible_present) {
    parent->rrc_log->warning("Not handling SRVCCOperationPossible\n");
  }
  if (msg.protocol_ies.subscriber_profile_idfor_rfp_present) {
    parent->rrc_log->warning("Not handling SubscriberProfileIDforRFP\n");
  }
  if (msg.protocol_ies.trace_activation_present) {
    parent->rrc_log->warning("Not handling TraceActivation\n");
  }
  if (msg.protocol_ies.ue_radio_cap_present) {
    parent->rrc_log->warning("Not handling UERadioCapability\n");
  }

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

  // Setup E-RABs
  setup_erabs(msg.protocol_ies.erab_to_be_setup_list_ctxt_su_req.value);
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

  if (msg.protocol_ies.add_cs_fallback_ind_present) {
    parent->rrc_log->warning("Not handling AdditionalCSFallbackIndicator\n");
  }
  if (msg.protocol_ies.csg_membership_status_present) {
    parent->rrc_log->warning("Not handling CSGMembershipStatus\n");
  }
  if (msg.protocol_ies.registered_lai_present) {
    parent->rrc_log->warning("Not handling RegisteredLAI\n");
  }
  if (msg.protocol_ies.subscriber_profile_idfor_rfp_present) {
    parent->rrc_log->warning("Not handling SubscriberProfileIDforRFP\n");
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

void rrc::ue::notify_s1ap_ue_ctxt_setup_complete()
{
  asn1::s1ap::init_context_setup_resp_s res;

  res.protocol_ies.erab_setup_list_ctxt_su_res.value.resize(bearer_list.get_erabs().size());
  uint32_t i = 0;
  for (const auto& erab : bearer_list.get_erabs()) {
    res.protocol_ies.erab_setup_list_ctxt_su_res.value[i].load_info_obj(ASN1_S1AP_ID_ERAB_SETUP_ITEM_CTXT_SU_RES);
    auto& item   = res.protocol_ies.erab_setup_list_ctxt_su_res.value[i].value.erab_setup_item_ctxt_su_res();
    item.erab_id = erab.second.id;
    srslte::uint32_to_uint8(erab.second.teid_in, item.gtp_teid.data());
    i++;
  }

  parent->s1ap->ue_ctxt_setup_complete(rnti, res);
}

void rrc::ue::set_bitrates(const asn1::s1ap::ue_aggregate_maximum_bitrate_s& rates)
{
  bitrates = rates;
}

bool rrc::ue::setup_erabs(const asn1::s1ap::erab_to_be_setup_list_ctxt_su_req_l& e)
{
  for (const auto& item : e) {
    auto& erab = item.value.erab_to_be_setup_item_ctxt_su_req();
    if (erab.ext) {
      parent->rrc_log->warning("Not handling E-RABToBeSetupListCtxtSURequest extensions\n");
    }
    if (erab.ie_exts_present) {
      parent->rrc_log->warning("Not handling E-RABToBeSetupListCtxtSURequest extensions\n");
    }
    if (erab.transport_layer_address.length() > 32) {
      parent->rrc_log->error("IPv6 addresses not currently supported\n");
      return false;
    }

    uint32_t teid_out;
    srslte::uint8_to_uint32(erab.gtp_teid.data(), &teid_out);
    const asn1::unbounded_octstring<true>* nas_pdu = erab.nas_pdu_present ? &erab.nas_pdu : nullptr;
    bearer_list.add_erab(erab.erab_id, erab.erab_level_qos_params, erab.transport_layer_address, teid_out, nas_pdu);
    bearer_list.add_gtpu_bearer(parent->gtpu, erab.erab_id);
  }
  return true;
}

bool rrc::ue::setup_erabs(const asn1::s1ap::erab_to_be_setup_list_bearer_su_req_l& e)
{
  for (const auto& item : e) {
    auto& erab = item.value.erab_to_be_setup_item_bearer_su_req();
    if (erab.ext) {
      parent->rrc_log->warning("Not handling E-RABToBeSetupListBearerSUReq extensions\n");
    }
    if (erab.ie_exts_present) {
      parent->rrc_log->warning("Not handling E-RABToBeSetupListBearerSUReq extensions\n");
    }
    if (erab.transport_layer_address.length() > 32) {
      parent->rrc_log->error("IPv6 addresses not currently supported\n");
      return false;
    }

    uint32_t teid_out;
    srslte::uint8_to_uint32(erab.gtp_teid.data(), &teid_out);
    bearer_list.add_erab(
        erab.erab_id, erab.erab_level_qos_params, erab.transport_layer_address, teid_out, &erab.nas_pdu);
    bearer_list.add_gtpu_bearer(parent->gtpu, erab.erab_id);
  }

  // Work in progress
  notify_s1ap_ue_erab_setup_response(e);
  send_connection_reconf_new_bearer();
  return true;
}

bool rrc::ue::release_erabs()
{
  bearer_list.release_erabs();
  return true;
}

void rrc::ue::notify_s1ap_ue_erab_setup_response(const asn1::s1ap::erab_to_be_setup_list_bearer_su_req_l& e)
{
  asn1::s1ap::erab_setup_resp_s res;

  const auto& erabs = bearer_list.get_erabs();
  for (const auto& erab : e) {
    uint8_t id = erab.value.erab_to_be_setup_item_bearer_su_req().erab_id;
    if (erabs.count(id)) {
      res.protocol_ies.erab_setup_list_bearer_su_res_present = true;
      res.protocol_ies.erab_setup_list_bearer_su_res.value.push_back({});
      auto& item = res.protocol_ies.erab_setup_list_bearer_su_res.value.back();
      item.load_info_obj(ASN1_S1AP_ID_ERAB_SETUP_ITEM_BEARER_SU_RES);
      item.value.erab_setup_item_bearer_su_res().erab_id = id;
      srslte::uint32_to_uint8(bearer_list.get_erabs().at(id).teid_in,
                              &item.value.erab_setup_item_bearer_su_res().gtp_teid[0]);
    } else {
      res.protocol_ies.erab_failed_to_setup_list_bearer_su_res_present = true;
      res.protocol_ies.erab_failed_to_setup_list_bearer_su_res.value.push_back({});
      auto& item                     = res.protocol_ies.erab_failed_to_setup_list_bearer_su_res.value.back();
      item.value.erab_item().erab_id = id;
      item.value.erab_item().cause.set_radio_network().value =
          asn1::s1ap::cause_radio_network_opts::invalid_qos_combination;
    }
  }

  parent->s1ap->ue_erab_setup_complete(rnti, res);
}

//! Helper method to access Cell configuration based on UE Carrier Index
cell_info_common* rrc::ue::get_ue_cc_cfg(uint32_t ue_cc_idx)
{
  if (ue_cc_idx >= cell_ded_list.nof_cells()) {
    return nullptr;
  }
  uint32_t enb_cc_idx = cell_ded_list.get_ue_cc_idx(ue_cc_idx)->cell_common->enb_cc_idx;
  return parent->cell_common_list->get_cc_idx(enb_cc_idx);
}

//! Method to fill SCellToAddModList for SCell info
int rrc::ue::fill_scell_to_addmod_list(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_reconf)
{
  // check whether we have SCells configured
  const cell_info_common* pcell_cfg = get_ue_cc_cfg(UE_PCELL_CC_IDX);
  if (pcell_cfg->cell_cfg.scell_list.empty()) {
    return SRSLTE_SUCCESS;
  }

  // Check whether UE supports CA
  if (not eutra_capabilities.non_crit_ext_present or not eutra_capabilities.non_crit_ext.non_crit_ext_present or
      not eutra_capabilities.non_crit_ext.non_crit_ext.non_crit_ext_present or
      not eutra_capabilities.non_crit_ext.non_crit_ext.non_crit_ext.rf_params_v1020_present or
      eutra_capabilities.non_crit_ext.non_crit_ext.non_crit_ext.rf_params_v1020.supported_band_combination_r10.size() ==
          0) {
    parent->rrc_log->info("UE doesn't support CA. Skipping SCell activation\n");
    return SRSLTE_SUCCESS;
  }

  // Allocate CQI + PUCCH for SCells.
  for (const scell_cfg_t& scell_cfg : pcell_cfg->cell_cfg.scell_list) {
    uint32_t cell_id = scell_cfg.cell_id;
    cell_ded_list.add_cell(parent->cell_common_list->get_cell_id(cell_id)->enb_cc_idx);
  }
  if (cell_ded_list.nof_cells() == 1) {
    // No SCell could be allocated. Fallback to single cell mode.
    return SRSLTE_SUCCESS;
  }

  parent->rrc_log->info("SCells activated for rnti=0x%x\n", rnti);

  conn_reconf->non_crit_ext_present                                                     = true;
  conn_reconf->non_crit_ext.non_crit_ext_present                                        = true;
  conn_reconf->non_crit_ext.non_crit_ext.non_crit_ext_present                           = true;
  conn_reconf->non_crit_ext.non_crit_ext.non_crit_ext.scell_to_add_mod_list_r10_present = true;
  auto& list = conn_reconf->non_crit_ext.non_crit_ext.non_crit_ext.scell_to_add_mod_list_r10;

  // Add all SCells configured+allocated for the current PCell
  phy_rrc_dedicated_list.resize(cell_ded_list.nof_cells());
  for (auto& p : cell_ded_list) {
    if (p.ue_cc_idx == UE_PCELL_CC_IDX) {
      continue;
    }
    uint32_t                scell_idx = p.ue_cc_idx;
    const cell_info_common* cc_cfg    = p.cell_common;
    const sib_type1_s&      cell_sib1 = cc_cfg->sib1;
    const sib_type2_s&      cell_sib2 = cc_cfg->sib2;

    scell_to_add_mod_r10_s cell;
    cell.scell_idx_r10                        = scell_idx;
    cell.cell_identif_r10_present             = true;
    cell.cell_identif_r10.pci_r10             = cc_cfg->cell_cfg.pci;
    cell.cell_identif_r10.dl_carrier_freq_r10 = cc_cfg->cell_cfg.dl_earfcn;
    cell.rr_cfg_common_scell_r10_present      = true;
    // RadioResourceConfigCommon
    const rr_cfg_common_sib_s& cc_cfg_sib = cell_sib2.rr_cfg_common;
    auto&                      nonul_cfg  = cell.rr_cfg_common_scell_r10.non_ul_cfg_r10;
    asn1::number_to_enum(nonul_cfg.dl_bw_r10, parent->cfg.cell.nof_prb);
    nonul_cfg.ant_info_common_r10.ant_ports_count.value = ant_info_common_s::ant_ports_count_opts::an1;
    nonul_cfg.phich_cfg_r10                             = cc_cfg->mib.phich_cfg;
    nonul_cfg.pdsch_cfg_common_r10                      = cc_cfg_sib.pdsch_cfg_common;
    // RadioResourceConfigCommonSCell-r10::ul-Configuration-r10
    cell.rr_cfg_common_scell_r10.ul_cfg_r10_present          = true;
    auto& ul_cfg                                             = cell.rr_cfg_common_scell_r10.ul_cfg_r10;
    ul_cfg.ul_freq_info_r10.ul_carrier_freq_r10_present      = true;
    ul_cfg.ul_freq_info_r10.ul_carrier_freq_r10              = cc_cfg->cell_cfg.ul_earfcn;
    ul_cfg.p_max_r10_present                                 = cell_sib1.p_max_present;
    ul_cfg.p_max_r10                                         = cell_sib1.p_max;
    ul_cfg.ul_freq_info_r10.add_spec_emission_scell_r10      = 1;
    ul_cfg.ul_pwr_ctrl_common_scell_r10.p0_nominal_pusch_r10 = cc_cfg_sib.ul_pwr_ctrl_common.p0_nominal_pusch;
    ul_cfg.ul_pwr_ctrl_common_scell_r10.alpha_r10.value      = cc_cfg_sib.ul_pwr_ctrl_common.alpha;
    ul_cfg.srs_ul_cfg_common_r10                             = cc_cfg_sib.srs_ul_cfg_common;
    ul_cfg.ul_cp_len_r10.value                               = cc_cfg_sib.ul_cp_len.value;
    ul_cfg.pusch_cfg_common_r10                              = cc_cfg_sib.pusch_cfg_common;
    // RadioResourceConfigDedicatedSCell-r10
    cell.rr_cfg_ded_scell_r10_present                                       = true;
    cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10_present                = true;
    cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.non_ul_cfg_r10_present = true;
    auto& nonul_cfg_ded                = cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.non_ul_cfg_r10;
    nonul_cfg_ded.ant_info_r10_present = true;
    asn1::number_to_enum(nonul_cfg_ded.ant_info_r10.tx_mode_r10, parent->cfg.cell.nof_ports);
    nonul_cfg_ded.ant_info_r10.ue_tx_ant_sel.set(setup_opts::release);
    nonul_cfg_ded.cross_carrier_sched_cfg_r10_present                                            = true;
    nonul_cfg_ded.cross_carrier_sched_cfg_r10.sched_cell_info_r10.set_own_r10().cif_presence_r10 = false;
    nonul_cfg_ded.pdsch_cfg_ded_r10_present                                                      = true;
    nonul_cfg_ded.pdsch_cfg_ded_r10.p_a.value                           = parent->cfg.pdsch_cfg.value;
    cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.ul_cfg_r10_present = true;
    auto& ul_cfg_ded                                  = cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.ul_cfg_r10;
    ul_cfg_ded.ant_info_ul_r10_present                = true;
    ul_cfg_ded.ant_info_ul_r10.tx_mode_ul_r10_present = true;
    asn1::number_to_enum(ul_cfg_ded.ant_info_ul_r10.tx_mode_ul_r10, parent->cfg.cell.nof_ports);
    ul_cfg_ded.pusch_cfg_ded_scell_r10_present           = true;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10_present         = true;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.p0_ue_pusch_r10 = 0;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.delta_mcs_enabled_r10.value =
        ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_opts::en0;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.accumulation_enabled_r10   = true;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.psrs_offset_ap_r10_present = true;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.psrs_offset_ap_r10         = 3;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.pathloss_ref_linking_r10.value =
        ul_pwr_ctrl_ded_scell_r10_s::pathloss_ref_linking_r10_opts::scell;
    ul_cfg_ded.cqi_report_cfg_scell_r10_present                               = true;
    ul_cfg_ded.cqi_report_cfg_scell_r10.nom_pdsch_rs_epre_offset_r10          = 0;
    ul_cfg_ded.cqi_report_cfg_scell_r10.cqi_report_periodic_scell_r10_present = true;
    if (ue_capabilities.support_dl_256qam) {
      cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.ext = true;
      cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.cqi_report_cfg_scell_v1250.set_present(true);
      cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.cqi_report_cfg_scell_v1250->alt_cqi_table_r12_present = true;
      cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.cqi_report_cfg_scell_v1250->alt_cqi_table_r12.value =
          cqi_report_cfg_v1250_s::alt_cqi_table_r12_opts::all_sfs;
    }

    // Get CQI allocation for secondary cell
    auto& cqi_setup = ul_cfg_ded.cqi_report_cfg_scell_r10.cqi_report_periodic_scell_r10.set_setup();
    get_cqi(&cqi_setup.cqi_pmi_cfg_idx, &cqi_setup.cqi_pucch_res_idx_r10, scell_idx);

    cqi_setup.cqi_format_ind_periodic_r10.set_wideband_cqi_r10();
    cqi_setup.simul_ack_nack_and_cqi = parent->cfg.cqi_cfg.simultaneousAckCQI;
#if SRS_ENABLED
    ul_cfg_ded.srs_ul_cfg_ded_r10_present                  = true;
    auto& srs_setup                                        = ul_cfg_ded.srs_ul_cfg_ded_r10.set_setup();
    srs_setup.srs_bw.value                                 = srs_ul_cfg_ded_c::setup_s_::srs_bw_opts::bw0;
    srs_setup.srs_hop_bw.value                             = srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_opts::hbw0;
    srs_setup.freq_domain_position                         = 0;
    srs_setup.dur                                          = true;
    srs_setup.srs_cfg_idx                                  = 167;
    srs_setup.tx_comb                                      = 0;
    srs_setup.cyclic_shift.value                           = srs_ul_cfg_ded_c::setup_s_::cyclic_shift_opts::cs0;
    ul_cfg_ded.srs_ul_cfg_ded_v1020_present                = true;
    ul_cfg_ded.srs_ul_cfg_ded_v1020.srs_ant_port_r10.value = srs_ant_port_opts::an1;
    ul_cfg_ded.srs_ul_cfg_ded_aperiodic_r10_present        = true;
    ul_cfg_ded.srs_ul_cfg_ded_aperiodic_r10.set(setup_opts::release);
#endif // SRS_ENABLED
    list.push_back(cell);

    // Create new PHY configuration structure for this SCell
    phy_interface_rrc_lte::phy_rrc_cfg_t scell_phy_rrc_ded = {};
    srslte::set_phy_cfg_t_scell_config(&scell_phy_rrc_ded.phy_cfg, cell);
    scell_phy_rrc_ded.configured = true;

    // Set PUSCH dedicated configuration following 3GPP TS 36.331 R 10 Section 6.3.2 Radio resource control information
    // elements - PUSCH-Config
    //   One value applies for all serving cells with an uplink (the associated functionality is common i.e. not
    //   performed independently for each cell).
    scell_phy_rrc_ded.phy_cfg.ul_cfg.pusch.uci_offset = phy_rrc_dedicated_list[0].phy_cfg.ul_cfg.pusch.uci_offset;

    // Get corresponding eNB CC index
    scell_phy_rrc_ded.enb_cc_idx = cc_cfg->enb_cc_idx;

    // Append to PHY RRC config dedicated which will be applied further down
    phy_rrc_dedicated_list[scell_idx] = scell_phy_rrc_ded;
  }

  // Set DL HARQ Feedback mode
  conn_reconf->rr_cfg_ded.phys_cfg_ded.pucch_cfg_ded_v1020.set_present(true);
  conn_reconf->rr_cfg_ded.phys_cfg_ded.pucch_cfg_ded_v1020->pucch_format_r10_present = true;
  conn_reconf->rr_cfg_ded.phys_cfg_ded.ext                                           = true;
  auto pucch_format_r10                      = conn_reconf->rr_cfg_ded.phys_cfg_ded.pucch_cfg_ded_v1020.get();
  pucch_format_r10->pucch_format_r10_present = true;
  if (cell_ded_list.nof_cells() <= 2) {
    // Use PUCCH format 1b with channel selection for 2 serving cells
    auto& ch_sel_r10                      = pucch_format_r10->pucch_format_r10.set_ch_sel_r10();
    ch_sel_r10.n1_pucch_an_cs_r10_present = true;
    ch_sel_r10.n1_pucch_an_cs_r10.set_setup();
    n1_pucch_an_cs_r10_l item0(4);
    // TODO: should we use a different n1PUCCH-AN-CS-List configuration?
    for (auto& it : item0) {
      it = cell_ded_list.is_pucch_cs_allocated() ? *cell_ded_list.get_n_pucch_cs() : 0;
    }
    ch_sel_r10.n1_pucch_an_cs_r10.setup().n1_pucch_an_cs_list_r10.push_back(item0);
  } else {
    // Use PUCCH format 3 for more than 2 serving cells
    auto& format3_r10                        = pucch_format_r10->pucch_format_r10.set_format3_r10();
    format3_r10.n3_pucch_an_list_r13_present = true;
    format3_r10.n3_pucch_an_list_r13.resize(4);
    for (auto& it : format3_r10.n3_pucch_an_list_r13) {
      // Hard-coded resource, only one user is supported
      it = 0;
    }
  }
  return SRSLTE_SUCCESS;
}

/********************** Handover **************************/

void rrc::ue::handle_ho_preparation_complete(bool is_success, srslte::unique_byte_buffer_t container)
{
  mobility_handler->handle_ho_preparation_complete(is_success, std::move(container));
}

/********************** HELPERS ***************************/

// Helper method to fill in rr_config_dedicated
void rrc::ue::fill_rrc_setup_rr_config_dedicated(asn1::rrc::rr_cfg_ded_s* rr_cfg)
{
  // Fill drbsToAddModList/srbsToAddModList/drbsToReleaseList
  bearer_list.fill_rr_cfg_ded(*rr_cfg);

  // Fill mac-MainConfig
  rr_cfg->mac_main_cfg_present  = true;
  mac_main_cfg_s* mac_cfg       = &rr_cfg->mac_main_cfg.set_explicit_value();
  mac_cfg->ul_sch_cfg_present   = true;
  mac_cfg->ul_sch_cfg           = parent->cfg.mac_cnfg.ul_sch_cfg;
  mac_cfg->phr_cfg_present      = true;
  mac_cfg->phr_cfg              = parent->cfg.mac_cnfg.phr_cfg;
  mac_cfg->time_align_timer_ded = parent->cfg.mac_cnfg.time_align_timer_ded;

  // Fill physicalConfigDedicated
  rr_cfg->phys_cfg_ded_present       = true;
  phys_cfg_ded_s* phy_cfg            = &rr_cfg->phys_cfg_ded;
  phy_cfg->pusch_cfg_ded_present     = true;
  phy_cfg->pusch_cfg_ded             = parent->cfg.pusch_cfg;
  phy_cfg->sched_request_cfg_present = true;
  phy_cfg->sched_request_cfg.set_setup();
  phy_cfg->sched_request_cfg.setup().dsr_trans_max = parent->cfg.sr_cfg.dsr_max;

  // set default antenna config
  phy_cfg->ant_info_present = true;
  phy_cfg->ant_info.set_explicit_value();
  if (parent->cfg.cell.nof_ports == 1) {
    phy_cfg->ant_info.explicit_value().tx_mode.value = ant_info_ded_s::tx_mode_e_::tm1;
  } else {
    phy_cfg->ant_info.explicit_value().tx_mode.value = ant_info_ded_s::tx_mode_e_::tm2;
  }
  phy_cfg->ant_info.explicit_value().ue_tx_ant_sel.set(setup_e::release);

  phy_cfg->sched_request_cfg.setup().sr_cfg_idx       = (uint8_t)cell_ded_list.get_sr_res()->sr_I;
  phy_cfg->sched_request_cfg.setup().sr_pucch_res_idx = (uint16_t)cell_ded_list.get_sr_res()->sr_N_pucch;

  // Power control
  phy_cfg->ul_pwr_ctrl_ded_present              = true;
  phy_cfg->ul_pwr_ctrl_ded.p0_ue_pusch          = 0;
  phy_cfg->ul_pwr_ctrl_ded.delta_mcs_enabled    = ul_pwr_ctrl_ded_s::delta_mcs_enabled_e_::en0;
  phy_cfg->ul_pwr_ctrl_ded.accumulation_enabled = true;
  phy_cfg->ul_pwr_ctrl_ded.p0_ue_pucch          = 0;
  phy_cfg->ul_pwr_ctrl_ded.psrs_offset          = 3;

  // PDSCH
  phy_cfg->pdsch_cfg_ded_present = true;
  phy_cfg->pdsch_cfg_ded.p_a     = parent->cfg.pdsch_cfg;

  // PUCCH
  phy_cfg->pucch_cfg_ded_present = true;
  phy_cfg->pucch_cfg_ded.ack_nack_repeat.set(pucch_cfg_ded_s::ack_nack_repeat_c_::types::release);

  phy_cfg->cqi_report_cfg_present = true;
  if (parent->cfg.cqi_cfg.mode == RRC_CFG_CQI_MODE_APERIODIC) {
    phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic_present = true;
    phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic         = cqi_report_mode_aperiodic_e::rm30;
  } else {
    phy_cfg->cqi_report_cfg.cqi_report_periodic_present = true;
    phy_cfg->cqi_report_cfg.cqi_report_periodic.set_setup();
    phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_format_ind_periodic.set(
        cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types::wideband_cqi);
    phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().simul_ack_nack_and_cqi = parent->cfg.cqi_cfg.simultaneousAckCQI;
    if (get_cqi(&phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pmi_cfg_idx,
                &phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pucch_res_idx,
                UE_PCELL_CC_IDX)) {
      parent->rrc_log->error("Allocating CQI resources for rnti=%d\n", rnti);
      return;
    }
  }
  phy_cfg->cqi_report_cfg.nom_pdsch_rs_epre_offset = 0;

  rr_cfg->rlf_timers_and_consts_r9.set_present(false);
  rr_cfg->sps_cfg_present = false;
}

void rrc::ue::send_dl_ccch(dl_ccch_msg_s* dl_ccch_msg)
{
  // Allocate a new PDU buffer, pack the message and send to PDCP
  srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool);
  if (pdu) {
    asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
    if (dl_ccch_msg->pack(bref) != asn1::SRSASN_SUCCESS) {
      parent->rrc_log->error_hex(pdu->msg, pdu->N_bytes, "Failed to pack DL-CCCH-Msg:\n");
      return;
    }
    pdu->N_bytes = (uint32_t)bref.distance_bytes();

    char buf[32] = {};
    sprintf(buf, "SRB0 - rnti=0x%x", rnti);
    parent->log_rrc_message(buf, Tx, pdu.get(), *dl_ccch_msg, dl_ccch_msg->msg.c1().type().to_string());
    parent->rlc->write_sdu(rnti, RB_ID_SRB0, std::move(pdu));
  } else {
    parent->rrc_log->error("Allocating pdu\n");
  }
}

bool rrc::ue::send_dl_dcch(const dl_dcch_msg_s* dl_dcch_msg, srslte::unique_byte_buffer_t pdu)
{
  if (!pdu) {
    pdu = srslte::allocate_unique_buffer(*pool);
  }
  if (pdu) {
    asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
    if (dl_dcch_msg->pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
      parent->rrc_log->error("Failed to encode DL-DCCH-Msg\n");
      return false;
    }
    pdu->N_bytes = (uint32_t)bref.distance_bytes();

    // send on SRB2 if user is fully registered (after RRC reconfig complete)
    uint32_t lcid =
        parent->rlc->has_bearer(rnti, RB_ID_SRB2) && state == RRC_STATE_REGISTERED ? RB_ID_SRB2 : RB_ID_SRB1;

    char buf[32] = {};
    sprintf(buf, "SRB%d - rnti=0x%x", lcid, rnti);
    parent->log_rrc_message(buf, Tx, pdu.get(), *dl_dcch_msg, dl_dcch_msg->msg.c1().type().to_string());

    parent->pdcp->write_sdu(rnti, lcid, std::move(pdu));
  } else {
    parent->rrc_log->error("Allocating pdu\n");
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
  srslte::set_phy_cfg_t_dedicated_cfg(&phy_rrc_dedicated_list[0].phy_cfg, phys_cfg_ded);

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
      srslte::set_phy_cfg_t_dedicated_cfg(&phy_rrc_dedicated_list[0].phy_cfg, phys_cfg_ded);
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
          // This is already applied when packing the SCell list
        }
      }
    }
  }

  // Send configuration to physical layer
  if (parent->phy != nullptr and update_phy) {
    parent->phy->set_config(rnti, phy_rrc_dedicated_list);
  }
}

void rrc::ue::apply_pdcp_srb_updates()
{
  for (const srb_to_add_mod_s& srb : bearer_list.get_pending_addmod_srbs()) {
    parent->pdcp->add_bearer(rnti, srb.srb_id, srslte::make_srb_pdcp_config_t(srb.srb_id, false));

    // For SRB2, enable security/encryption/integrity
    if (ue_security_cfg.is_as_sec_cfg_valid()) {
      parent->pdcp->config_security(rnti, srb.srb_id, ue_security_cfg.get_as_sec_cfg());
      parent->pdcp->enable_integrity(rnti, srb.srb_id);
      parent->pdcp->enable_encryption(rnti, srb.srb_id);
    }
  }
}

void rrc::ue::apply_pdcp_drb_updates()
{
  for (uint8_t drb_id : bearer_list.get_pending_rem_drbs()) {
    parent->pdcp->del_bearer(rnti, drb_id + 2);
  }
  for (const drb_to_add_mod_s& drb : bearer_list.get_pending_addmod_drbs()) {
    // Configure DRB1 in PDCP
    if (drb.pdcp_cfg_present) {
      srslte::pdcp_config_t pdcp_cnfg_drb = srslte::make_drb_pdcp_config_t(drb.drb_id, false, drb.pdcp_cfg);
      parent->pdcp->add_bearer(rnti, drb.lc_ch_id, pdcp_cnfg_drb);
    } else {
      srslte::pdcp_config_t pdcp_cnfg_drb = srslte::make_drb_pdcp_config_t(drb.drb_id, false);
      parent->pdcp->add_bearer(rnti, drb.lc_ch_id, pdcp_cnfg_drb);
    }

    if (ue_security_cfg.is_as_sec_cfg_valid()) {
      parent->pdcp->config_security(rnti, drb.lc_ch_id, ue_security_cfg.get_as_sec_cfg());
      parent->pdcp->enable_integrity(rnti, drb.lc_ch_id);
      parent->pdcp->enable_encryption(rnti, drb.lc_ch_id);
    }
  }
}

void rrc::ue::apply_rlc_rb_updates()
{
  for (const srb_to_add_mod_s& srb : bearer_list.get_pending_addmod_srbs()) {
    parent->rlc->add_bearer(rnti, srb.srb_id, srslte::rlc_config_t::srb_config(srb.srb_id));
  }
  if (bearer_list.get_pending_rem_drbs().size() > 0) {
    parent->rrc_log->error("Removing DRBs not currently supported\n");
  }
  for (const drb_to_add_mod_s& drb : bearer_list.get_pending_addmod_drbs()) {
    if (not drb.rlc_cfg_present) {
      parent->rrc_log->warning("Default RLC DRB config not supported\n");
    }
    parent->rlc->add_bearer(rnti, drb.lc_ch_id, srslte::make_rlc_config_t(drb.rlc_cfg));
  }
}

int rrc::ue::get_cqi(uint16_t* pmi_idx, uint16_t* n_pucch, uint32_t ue_cc_idx)
{
  cell_ctxt_dedicated* c = cell_ded_list.get_ue_cc_idx(ue_cc_idx);
  if (c != nullptr and c->cqi_res_present) {
    *pmi_idx = c->cqi_res.pmi_idx;
    *n_pucch = c->cqi_res.pucch_res;
    return SRSLTE_SUCCESS;
  } else {
    parent->rrc_log->error("CQI resources for ue_cc_idx=%d have not been allocated\n", ue_cc_idx);
    return SRSLTE_ERROR;
  }
}

bool rrc::ue::is_allocated() const
{
  return cell_ded_list.is_allocated();
}

int rrc::ue::get_ri(uint32_t m_ri, uint16_t* ri_idx)
{
  int32_t ret = SRSLTE_SUCCESS;

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
      parent->rrc_log->error("Allocating RI: invalid m_ri=%d\n", m_ri);
  }

  // If ri_dix is available, copy
  if (ri_idx) {
    *ri_idx = I_ri;
  }

  return ret;
}

} // namespace srsenb
