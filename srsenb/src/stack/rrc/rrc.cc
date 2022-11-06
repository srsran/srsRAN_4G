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

#include "srsenb/hdr/stack/rrc/rrc.h"
#include "srsenb/hdr/stack/mac/sched_interface.h"
#include "srsenb/hdr/stack/rrc/rrc_cell_cfg.h"
#include "srsenb/hdr/stack/rrc/rrc_endc.h"
#include "srsenb/hdr/stack/rrc/rrc_mobility.h"
#include "srsenb/hdr/stack/rrc/rrc_paging.h"
#include "srsenb/hdr/stack/s1ap/s1ap.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/rrc_utils.h"
#include "srsran/common/bcd_helpers.h"
#include "srsran/common/enb_events.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"
#include "srsran/interfaces/enb_mac_interfaces.h"
#include "srsran/interfaces/enb_pdcp_interfaces.h"
#include "srsran/interfaces/enb_rlc_interfaces.h"

using srsran::byte_buffer_t;

using namespace asn1::rrc;

namespace srsenb {

rrc::rrc(srsran::task_sched_handle task_sched_, enb_bearer_manager& manager_) :
  logger(srslog::fetch_basic_logger("RRC")), bearer_manager(manager_), task_sched(task_sched_), rx_pdu_queue(128)
{
}

rrc::~rrc() {}

int32_t rrc::init(const rrc_cfg_t&       cfg_,
                  phy_interface_rrc_lte* phy_,
                  mac_interface_rrc*     mac_,
                  rlc_interface_rrc*     rlc_,
                  pdcp_interface_rrc*    pdcp_,
                  s1ap_interface_rrc*    s1ap_,
                  gtpu_interface_rrc*    gtpu_)
{
  return init(cfg_, phy_, mac_, rlc_, pdcp_, s1ap_, gtpu_, nullptr);
}

int32_t rrc::init(const rrc_cfg_t&       cfg_,
                  phy_interface_rrc_lte* phy_,
                  mac_interface_rrc*     mac_,
                  rlc_interface_rrc*     rlc_,
                  pdcp_interface_rrc*    pdcp_,
                  s1ap_interface_rrc*    s1ap_,
                  gtpu_interface_rrc*    gtpu_,
                  rrc_nr_interface_rrc*  rrc_nr_)
{
  phy    = phy_;
  mac    = mac_;
  rlc    = rlc_;
  pdcp   = pdcp_;
  gtpu   = gtpu_;
  s1ap   = s1ap_;
  rrc_nr = rrc_nr_;

  cfg = cfg_;

  if (cfg.sibs[12].type() == asn1::rrc::sys_info_r8_ies_s::sib_type_and_info_item_c_::types::sib13_v920 &&
      cfg.enable_mbsfn) {
    configure_mbsfn_sibs();
  }

  cell_res_list.reset(new freq_res_common_list{cfg});

  // Loads the PRACH root sequence
  cfg.sibs[1].sib2().rr_cfg_common.prach_cfg.root_seq_idx = cfg.cell_list[0].root_seq_idx;

  if (cfg.num_nr_cells > 0) {
    cfg.sibs[1].sib2().ext = true;
    cfg.sibs[1].sib2().plmn_info_list_r15.set_present();
    cfg.sibs[1].sib2().plmn_info_list_r15.get()->resize(1);
    auto& plmn                       = cfg.sibs[1].sib2().plmn_info_list_r15.get()->back();
    plmn.upper_layer_ind_r15_present = true;
  }

  if (generate_sibs() != SRSRAN_SUCCESS) {
    logger.error("Couldn't generate SIBs.");
    return false;
  }
  config_mac();

  // Check valid inactivity timeout config
  uint32_t t310 = cfg.sibs[1].sib2().ue_timers_and_consts.t310.to_number();
  uint32_t t311 = cfg.sibs[1].sib2().ue_timers_and_consts.t311.to_number();
  uint32_t n310 = cfg.sibs[1].sib2().ue_timers_and_consts.n310.to_number();
  logger.info("T310 %d, T311 %d, N310 %d", t310, t311, n310);
  if (cfg.inactivity_timeout_ms < t310 + t311 + n310) {
    srsran::console("\nWarning: Inactivity timeout is smaller than the sum of t310, t311 and n310.\n"
                    "This may break the UE's re-establishment procedure.\n");
    logger.warning("Inactivity timeout is smaller than the sum of t310, t311 and n310. This may break the UE's "
                   "re-establishment procedure.");
  }
  logger.info("Inactivity timeout: %d ms", cfg.inactivity_timeout_ms);
  logger.info("Max consecutive MAC KOs: %d", cfg.max_mac_dl_kos);

  pending_paging.reset(new paging_manager(cfg.sibs[1].sib2().rr_cfg_common.pcch_cfg.default_paging_cycle.to_number(),
                                          cfg.sibs[1].sib2().rr_cfg_common.pcch_cfg.nb.to_number()));

  running = true;

  if (logger.debug.enabled()) {
    asn1::json_writer js{};
    cfg.srb1_cfg.rlc_cfg.to_json(js);
    logger.debug("SRB1 configuration: %s", js.to_string().c_str());
    js = {};
    cfg.srb2_cfg.rlc_cfg.to_json(js);
    logger.debug("SRB2 configuration: %s", js.to_string().c_str());
  }
  return SRSRAN_SUCCESS;
}

void rrc::stop()
{
  if (running) {
    running   = false;
    rrc_pdu p = {0, LCID_EXIT, false, nullptr};
    rx_pdu_queue.push_blocking(std::move(p));
  }
  users.clear();
}

/*******************************************************************************
  Public functions
*******************************************************************************/

void rrc::get_metrics(rrc_metrics_t& m)
{
  if (running) {
    m.ues.resize(users.size());
    size_t count = 0;
    for (auto& ue : users) {
      ue.second->get_metrics(m.ues[count++]);
    }
  }
}

/*******************************************************************************
  MAC interface

  Those functions that shall be called from a phch_worker should push the command
  to the queue and process later
*******************************************************************************/

uint8_t* rrc::read_pdu_bcch_dlsch(const uint8_t cc_idx, const uint32_t sib_index)
{
  if (sib_index < ASN1_RRC_MAX_SIB && cc_idx < cell_common_list->nof_cells()) {
    return cell_common_list->get_cc_idx(cc_idx)->sib_buffer.at(sib_index)->msg;
  }
  return nullptr;
}

void rrc::set_radiolink_dl_state(uint16_t rnti, bool crc_res)
{
  // embed parameters in arg value
  rrc_pdu p = {rnti, LCID_RADLINK_DL, crc_res, nullptr};

  if (not rx_pdu_queue.try_push(std::move(p))) {
    logger.error("Failed to push radio link DL state");
  }
}

void rrc::set_radiolink_ul_state(uint16_t rnti, bool crc_res)
{
  // embed parameters in arg value
  rrc_pdu p = {rnti, LCID_RADLINK_UL, crc_res, nullptr};

  if (not rx_pdu_queue.try_push(std::move(p))) {
    logger.error("Failed to push radio link UL state");
  }
}

void rrc::set_activity_user(uint16_t rnti)
{
  rrc_pdu p = {rnti, LCID_ACT_USER, false, nullptr};

  if (not rx_pdu_queue.try_push(std::move(p))) {
    logger.error("Failed to push UE activity command to RRC queue");
  }
}

void rrc::rem_user_thread(uint16_t rnti)
{
  rrc_pdu p = {rnti, LCID_REM_USER, false, nullptr};
  if (not rx_pdu_queue.try_push(std::move(p))) {
    logger.error("Failed to push UE remove command to RRC queue");
  }
}

uint32_t rrc::get_nof_users()
{
  return users.size();
}

void rrc::max_retx_attempted(uint16_t rnti)
{
  rrc_pdu p = {rnti, LCID_RLC_RTX, false, nullptr};
  if (not rx_pdu_queue.try_push(std::move(p))) {
    logger.error("Failed to push max Retx event to RRC queue");
  }
}

void rrc::protocol_failure(uint16_t rnti)
{
  rrc_pdu p = {rnti, LCID_PROT_FAIL, false, nullptr};
  if (not rx_pdu_queue.try_push(std::move(p))) {
    logger.error("Failed to push protocol failure to RRC queue");
  }
}

// This function is called from PRACH worker (can wait)
int rrc::add_user(uint16_t rnti, const sched_interface::ue_cfg_t& sched_ue_cfg)
{
  auto user_it = users.find(rnti);
  if (user_it == users.end()) {
    if (rnti != SRSRAN_MRNTI) {
      // only non-eMBMS RNTIs are present in user map
      unique_rnti_ptr<ue> u = make_rnti_obj<ue>(rnti, this, rnti, sched_ue_cfg);
      if (u->init() != SRSRAN_SUCCESS) {
        logger.error("Adding user rnti=0x%x - Failed to allocate user resources", rnti);
        return SRSRAN_ERROR;
      }
      users.insert(std::make_pair(rnti, std::move(u)));
    }
    rlc->add_user(rnti);
    pdcp->add_user(rnti);
    logger.info("Added new user rnti=0x%x", rnti);
  } else {
    logger.error("Adding user rnti=0x%x (already exists)", rnti);
  }

  if (rnti == SRSRAN_MRNTI) {
    for (auto& mbms_item : mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[0].mbms_session_info_list_r9) {
      uint32_t lcid = mbms_item.lc_ch_id_r9;
      uint32_t addr_in;
      // adding UE object to MAC for MRNTI without scheduling configuration (broadcast not part of regular scheduling)
      rlc->add_bearer_mrb(SRSRAN_MRNTI, lcid);
      bearer_manager.add_eps_bearer(SRSRAN_MRNTI, 1, srsran::srsran_rat_t::lte, lcid);
      pdcp->add_bearer(SRSRAN_MRNTI, lcid, srsran::make_drb_pdcp_config_t(1, false));
      gtpu->add_bearer(SRSRAN_MRNTI, lcid, 1, 1, addr_in);
    }
  }
  return SRSRAN_SUCCESS;
}

/* Function called by MAC after the reception of a C-RNTI CE indicating that the UE still has a
 * valid RNTI.
 */
void rrc::upd_user(uint16_t new_rnti, uint16_t old_rnti)
{
  // Remove new_rnti
  auto new_ue_it = users.find(new_rnti);
  if (new_ue_it != users.end()) {
    new_ue_it->second->deactivate_bearers();
    rem_user_thread(new_rnti);
  }

  // Send Reconfiguration to old_rnti if is RRC_CONNECT or RRC Release if already released here
  auto old_it = users.find(old_rnti);
  if (old_it == users.end()) {
    logger.info("rnti=0x%x received MAC CRNTI CE: 0x%x, but old context is unavailable", new_rnti, old_rnti);
    return;
  }
  ue* ue_ptr = old_it->second.get();

  if (ue_ptr->mobility_handler->is_ho_running()) {
    ue_ptr->mobility_handler->trigger(ue::rrc_mobility::user_crnti_upd_ev{old_rnti, new_rnti});
  } else {
    logger.info("Resuming rnti=0x%x RRC connection due to received C-RNTI CE from rnti=0x%x.", old_rnti, new_rnti);
    if (ue_ptr->is_connected()) {
      // Send a new RRC Reconfiguration to overlay previous
      old_it->second->send_connection_reconf();
    }
  }

  // Log event.
  event_logger::get().log_connection_resume(
      ue_ptr->get_cell_list().get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx, old_rnti, new_rnti);
}

// Note: this method is not part of UE methods, because the UE context may not exist anymore when reject is sent
void rrc::send_rrc_connection_reject(uint16_t rnti)
{
  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set_c1().set_rrc_conn_reject().crit_exts.set_c1().set_rrc_conn_reject_r8().wait_time = 10;

  // Allocate a new PDU buffer, pack the message and send to PDCP
  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Allocating pdu");
    return;
  }
  asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
  if (dl_ccch_msg.pack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error(pdu->msg, bref.distance_bytes(), "Failed to pack DL-CCCH-Msg:");
    return;
  }
  pdu->N_bytes = bref.distance_bytes();

  log_rrc_message(Tx, rnti, srb_to_lcid(lte_srb::srb0), *pdu, dl_ccch_msg, dl_ccch_msg.msg.c1().type().to_string());

  rlc->write_sdu(rnti, srb_to_lcid(lte_srb::srb0), std::move(pdu));
}

/*******************************************************************************
  PDCP interface
*******************************************************************************/
void rrc::write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  rrc_pdu p = {rnti, lcid, false, std::move(pdu)};
  if (not rx_pdu_queue.try_push(std::move(p))) {
    logger.error("Failed to push Release command to RRC queue");
  }
}

void rrc::notify_pdcp_integrity_error(uint16_t rnti, uint32_t lcid)
{
  logger.warning("Received integrity protection failure indication, rnti=0x%x, lcid=%u", rnti, lcid);
  s1ap->user_release(rnti, asn1::s1ap::cause_radio_network_opts::unspecified);
}

/*******************************************************************************
  S1AP interface
*******************************************************************************/
void rrc::write_dl_info(uint16_t rnti, srsran::unique_byte_buffer_t sdu)
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1();
  dl_dcch_msg_type_c::c1_c_* msg_c1 = &dl_dcch_msg.msg.c1();

  auto user_it = users.find(rnti);
  if (user_it != users.end()) {
    dl_info_transfer_r8_ies_s* dl_info_r8 =
        &msg_c1->set_dl_info_transfer().crit_exts.set_c1().set_dl_info_transfer_r8();
    //    msg_c1->dl_info_transfer().rrc_transaction_id = ;
    dl_info_r8->non_crit_ext_present = false;
    dl_info_r8->ded_info_type.set_ded_info_nas();
    dl_info_r8->ded_info_type.ded_info_nas().resize(sdu->N_bytes);
    memcpy(msg_c1->dl_info_transfer().crit_exts.c1().dl_info_transfer_r8().ded_info_type.ded_info_nas().data(),
           sdu->msg,
           sdu->N_bytes);

    sdu->clear();

    user_it->second->send_dl_dcch(&dl_dcch_msg, std::move(sdu));
  } else {
    logger.error("Rx SDU for unknown rnti=0x%x", rnti);
  }
}

void rrc::release_ue(uint16_t rnti)
{
  rrc_pdu p = {rnti, LCID_REL_USER, false, nullptr};
  if (not rx_pdu_queue.try_push(std::move(p))) {
    logger.error("Failed to push Release command to RRC queue");
  }
}

bool rrc::setup_ue_ctxt(uint16_t rnti, const asn1::s1ap::init_context_setup_request_s& msg)
{
  logger.info("Adding initial context for 0x%x", rnti);
  auto user_it = users.find(rnti);
  if (user_it == users.end()) {
    logger.warning("Unrecognised rnti: 0x%x", rnti);
    return false;
  }

  user_it->second->handle_ue_init_ctxt_setup_req(msg);
  return true;
}

bool rrc::modify_ue_ctxt(uint16_t rnti, const asn1::s1ap::ue_context_mod_request_s& msg)
{
  logger.info("Modifying context for 0x%x", rnti);
  auto user_it = users.find(rnti);

  if (user_it == users.end()) {
    logger.warning("Unrecognised rnti: 0x%x", rnti);
    return false;
  }

  return user_it->second->handle_ue_ctxt_mod_req(msg);
}

bool rrc::release_erabs(uint32_t rnti)
{
  logger.info("Releasing E-RABs for 0x%x", rnti);
  auto user_it = users.find(rnti);

  if (user_it == users.end()) {
    logger.warning("Unrecognised rnti: 0x%x", rnti);
    return false;
  }

  bool ret = user_it->second->release_erabs();
  return ret;
}

int rrc::release_erab(uint16_t rnti, uint16_t erab_id)
{
  logger.info("Releasing E-RAB id=%d for 0x%x", erab_id, rnti);
  auto user_it = users.find(rnti);

  if (user_it == users.end()) {
    logger.warning("Unrecognised rnti: 0x%x", rnti);
    return SRSRAN_ERROR;
  }

  return user_it->second->release_erab(erab_id);
}

int rrc::notify_ue_erab_updates(uint16_t rnti, srsran::const_byte_span nas_pdu)
{
  auto user_it = users.find(rnti);
  if (user_it == users.end()) {
    logger.warning("Unrecognised rnti: 0x%x", rnti);
    return SRSRAN_ERROR;
  }
  user_it->second->send_connection_reconf(nullptr, false, nas_pdu);
  return SRSRAN_SUCCESS;
}

bool rrc::has_erab(uint16_t rnti, uint32_t erab_id) const
{
  auto user_it = users.find(rnti);
  if (user_it == users.end()) {
    logger.warning("Unrecognised rnti: 0x%x", rnti);
    return false;
  }
  return user_it->second->has_erab(erab_id);
}

int rrc::get_erab_addr_in(uint16_t rnti, uint16_t erab_id, transp_addr_t& addr_in, uint32_t& teid_in) const
{
  auto user_it = users.find(rnti);
  if (user_it == users.end()) {
    logger.warning("Unrecognised rnti: 0x%x", rnti);
    return SRSRAN_ERROR;
  }
  return user_it->second->get_erab_addr_in(erab_id, addr_in, teid_in);
}

void rrc::set_aggregate_max_bitrate(uint16_t rnti, const asn1::s1ap::ue_aggregate_maximum_bitrate_s& bitrate)
{
  auto user_it = users.find(rnti);
  if (user_it == users.end()) {
    logger.warning("Unrecognised rnti: 0x%x", rnti);
    return;
  }
  user_it->second->set_bitrates(bitrate);
}

int rrc::setup_erab(uint16_t                                           rnti,
                    uint16_t                                           erab_id,
                    const asn1::s1ap::erab_level_qos_params_s&         qos_params,
                    srsran::const_span<uint8_t>                        nas_pdu,
                    const asn1::bounded_bitstring<1, 160, true, true>& addr,
                    uint32_t                                           gtpu_teid_out,
                    asn1::s1ap::cause_c&                               cause)
{
  logger.info("Setting up erab id=%d for 0x%x", erab_id, rnti);
  auto user_it = users.find(rnti);
  if (user_it == users.end()) {
    logger.warning("Unrecognised rnti: 0x%x", rnti);
    cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::unknown_erab_id;
    return SRSRAN_ERROR;
  }
  return user_it->second->setup_erab(erab_id, qos_params, nas_pdu, addr, gtpu_teid_out, cause);
}

int rrc::modify_erab(uint16_t                                   rnti,
                     uint16_t                                   erab_id,
                     const asn1::s1ap::erab_level_qos_params_s& qos_params,
                     srsran::const_span<uint8_t>                nas_pdu,
                     asn1::s1ap::cause_c&                       cause)
{
  logger.info("Modifying E-RAB for 0x%x. E-RAB Id %d", rnti, erab_id);
  auto user_it = users.find(rnti);
  if (user_it == users.end()) {
    logger.warning("Unrecognised rnti: 0x%x", rnti);
    cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::unknown_erab_id;
    return SRSRAN_ERROR;
  }

  return user_it->second->modify_erab(erab_id, qos_params, nas_pdu, cause);
}

/*******************************************************************************
  Paging functions
  These functions use a different mutex because access different shared variables
  than user map
*******************************************************************************/

void rrc::add_paging_id(uint32_t ueid, const asn1::s1ap::ue_paging_id_c& ue_paging_id)
{
  if (ue_paging_id.type().value == asn1::s1ap::ue_paging_id_c::types_opts::imsi) {
    pending_paging->add_imsi_paging(ueid, ue_paging_id.imsi());
  } else {
    pending_paging->add_tmsi_paging(ueid, ue_paging_id.s_tmsi().mmec[0], ue_paging_id.s_tmsi().m_tmsi);
  }
}

bool rrc::is_paging_opportunity(uint32_t tti, uint32_t* payload_len)
{
  *payload_len = pending_paging->pending_pcch_bytes(tti_point(tti));
  return *payload_len > 0;
}

void rrc::read_pdu_pcch(uint32_t tti_tx_dl, uint8_t* payload, uint32_t buffer_size)
{
  auto read_func = [this, payload, buffer_size](srsran::const_byte_span pdu, const pcch_msg_s& msg, bool first_tx) {
    // copy PCCH pdu to buffer
    if (pdu.size() > buffer_size) {
      logger.warning("byte buffer with size=%zd is too small to fit pcch msg with size=%zd", buffer_size, pdu.size());
      return false;
    }
    std::copy(pdu.begin(), pdu.end(), payload);

    if (first_tx) {
      logger.info("Assembling PCCH payload with %d UE identities, payload_len=%d bytes",
                  msg.msg.c1().paging().paging_record_list.size(),
                  pdu.size());
      log_broadcast_rrc_message(SRSRAN_PRNTI, pdu, msg, msg.msg.c1().type().to_string());
    }
    return true;
  };

  pending_paging->read_pdu_pcch(tti_point(tti_tx_dl), read_func);
}

/*******************************************************************************
  Handover functions
*******************************************************************************/

void rrc::ho_preparation_complete(uint16_t                     rnti,
                                  ho_prep_result               result,
                                  const asn1::s1ap::ho_cmd_s&  msg,
                                  srsran::unique_byte_buffer_t rrc_container)
{
  users.at(rnti)->mobility_handler->handle_ho_preparation_complete(result, msg, std::move(rrc_container));
}

void rrc::set_erab_status(uint16_t rnti, const asn1::s1ap::bearers_subject_to_status_transfer_list_l& erabs)
{
  auto ue_it = users.find(rnti);
  if (ue_it == users.end()) {
    logger.warning("rnti=0x%x does not exist", rnti);
    return;
  }
  ue_it->second->mobility_handler->trigger(erabs);
}

/*******************************************************************************
  EN-DC/NSA helper functions
*******************************************************************************/

void rrc::sgnb_addition_ack(uint16_t eutra_rnti, sgnb_addition_ack_params_t params)
{
  logger.info("Received SgNB addition acknowledgement for rnti=0x%x", eutra_rnti);
  auto ue_it = users.find(eutra_rnti);
  if (ue_it == users.end()) {
    logger.warning("rnti=0x%x does not exist", eutra_rnti);
    return;
  }
  ue_it->second->endc_handler->trigger(ue::rrc_endc::sgnb_add_req_ack_ev{params});

  // trigger RRC Reconfiguration to send NR config to UE
  ue_it->second->send_connection_reconf();
}

void rrc::sgnb_addition_reject(uint16_t eutra_rnti)
{
  logger.error("Received SgNB addition reject for rnti=%d", eutra_rnti);
  auto ue_it = users.find(eutra_rnti);
  if (ue_it == users.end()) {
    logger.warning("rnti=0x%x does not exist", eutra_rnti);
    return;
  }
  ue_it->second->endc_handler->trigger(ue::rrc_endc::sgnb_add_req_reject_ev{});
}

void rrc::sgnb_addition_complete(uint16_t eutra_rnti, uint16_t nr_rnti)
{
  logger.info("User rnti=0x%x successfully enabled EN-DC", eutra_rnti);
  auto ue_it = users.find(eutra_rnti);
  if (ue_it == users.end()) {
    logger.warning("rnti=0x%x does not exist", eutra_rnti);
    return;
  }
  ue_it->second->endc_handler->trigger(ue::rrc_endc::sgnb_add_complete_ev{nr_rnti});
}

void rrc::sgnb_inactivity_timeout(uint16_t eutra_rnti)
{
  logger.info("Received NR inactivity timeout for rnti=0x%x - releasing UE", eutra_rnti);
  auto ue_it = users.find(eutra_rnti);
  if (ue_it == users.end()) {
    logger.warning("rnti=0x%x does not exist", eutra_rnti);
    return;
  }
  s1ap->user_release(eutra_rnti, asn1::s1ap::cause_radio_network_opts::user_inactivity);
}

void rrc::sgnb_release_ack(uint16_t eutra_rnti)
{
  auto ue_it = users.find(eutra_rnti);
  if (ue_it != users.end()) {
    logger.info("Received SgNB release acknowledgement for rnti=0x%x", eutra_rnti);
    ue_it->second->endc_handler->trigger(ue::rrc_endc::sgnb_rel_req_ack_ev{});
  } else {
    // The EUTRA does not need to wait for Release Ack in case it wants to destroy the EUTRA UE
    logger.info("Received SgNB release acknowledgement for already released rnti=0x%x", eutra_rnti);
  }
}

/*******************************************************************************
  Private functions
  All private functions are not mutexed and must be called from a mutexed environment
  from either a public function or the internal thread
*******************************************************************************/

void rrc::parse_ul_ccch(ue& ue, srsran::unique_byte_buffer_t pdu)
{
  srsran_assert(pdu != nullptr, "handle_ul_ccch called for empty message");

  ul_ccch_msg_s  ul_ccch_msg;
  asn1::cbit_ref bref(pdu->msg, pdu->N_bytes);
  if (ul_ccch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
      ul_ccch_msg.msg.type().value != ul_ccch_msg_type_c::types_opts::c1) {
    log_rx_pdu_fail(ue.rnti, srb_to_lcid(lte_srb::srb0), *pdu, "Failed to unpack UL-CCCH message");
    return;
  }

  // Log Rx message
  log_rrc_message(
      Rx, ue.rnti, srsran::srb_to_lcid(lte_srb::srb0), *pdu, ul_ccch_msg, ul_ccch_msg.msg.c1().type().to_string());

  switch (ul_ccch_msg.msg.c1().type().value) {
    case ul_ccch_msg_type_c::c1_c_::types::rrc_conn_request:
      ue.save_ul_message(std::move(pdu));
      ue.handle_rrc_con_req(&ul_ccch_msg.msg.c1().rrc_conn_request());
      break;
    case ul_ccch_msg_type_c::c1_c_::types::rrc_conn_reest_request:
      ue.save_ul_message(std::move(pdu));
      ue.handle_rrc_con_reest_req(&ul_ccch_msg.msg.c1().rrc_conn_reest_request());
      break;
    default:
      logger.error("Processing UL-CCCH for rnti=0x%x - Unsupported message type %s",
                   ul_ccch_msg.msg.c1().type().to_string());
      break;
  }
}

///< User mutex must be hold by caller
void rrc::parse_ul_dcch(ue& ue, uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  srsran_assert(pdu != nullptr, "handle_ul_dcch called for empty message");

  ue.parse_ul_dcch(lcid, std::move(pdu));
}

///< User mutex must be hold by caller
void rrc::process_release_complete(uint16_t rnti)
{
  logger.info("Received Release Complete rnti=0x%x", rnti);
  auto user_it = users.find(rnti);
  if (user_it == users.end()) {
    logger.error("Received ReleaseComplete for unknown rnti=0x%x", rnti);
    return;
  }
  ue* u = user_it->second.get();

  if (u->is_idle() or u->mobility_handler->is_ho_running()) {
    rem_user_thread(rnti);
  } else if (not u->is_idle()) {
    rlc->clear_buffer(rnti);
    user_it->second->send_connection_release();
    // delay user deletion for ~50 TTI (until RRC release is sent)
    task_sched.defer_callback(50, [this, rnti]() { rem_user_thread(rnti); });
  }
}

void rrc::rem_user(uint16_t rnti)
{
  auto user_it = users.find(rnti);
  if (user_it != users.end()) {
    // First remove MAC and GTPU to stop processing DL/UL traffic for this user
    mac->ue_rem(rnti); // MAC handles PHY
    gtpu->rem_user(rnti);

    // Now remove RLC and PDCP
    bearer_manager.rem_user(rnti);
    rlc->rem_user(rnti);
    pdcp->rem_user(rnti);

    users.erase(rnti);

    srsran::console("Disconnecting rnti=0x%x.\n", rnti);
    logger.info("Removed user rnti=0x%x", rnti);
  } else {
    logger.error("Removing user rnti=0x%x (does not exist)", rnti);
  }
}

void rrc::config_mac()
{
  using sched_cell_t = sched_interface::cell_cfg_t;

  // Fill MAC scheduler configuration for SIBs
  std::vector<sched_cell_t> sched_cfg;
  sched_cfg.resize(cfg.cell_list.size());

  for (uint32_t ccidx = 0; ccidx < cfg.cell_list.size(); ++ccidx) {
    sched_interface::cell_cfg_t& item = sched_cfg[ccidx];

    // set sib/prach cfg
    for (uint32_t i = 0; i < nof_si_messages; i++) {
      item.sibs[i].len = cell_common_list->get_cc_idx(ccidx)->sib_buffer.at(i)->N_bytes;
      if (i == 0) {
        item.sibs[i].period_rf = 8; // SIB1 is always 8 rf
      } else {
        item.sibs[i].period_rf = cfg.sib1.sched_info_list[i - 1].si_periodicity.to_number();
      }
    }
    item.prach_config        = cfg.sibs[1].sib2().rr_cfg_common.prach_cfg.prach_cfg_info.prach_cfg_idx;
    item.prach_nof_preambles = cfg.sibs[1].sib2().rr_cfg_common.rach_cfg_common.preamb_info.nof_ra_preambs.to_number();
    item.si_window_ms        = cfg.sib1.si_win_len.to_number();
    item.prach_rar_window =
        cfg.sibs[1].sib2().rr_cfg_common.rach_cfg_common.ra_supervision_info.ra_resp_win_size.to_number();
    item.prach_freq_offset    = cfg.sibs[1].sib2().rr_cfg_common.prach_cfg.prach_cfg_info.prach_freq_offset;
    item.maxharq_msg3tx       = cfg.sibs[1].sib2().rr_cfg_common.rach_cfg_common.max_harq_msg3_tx;
    item.enable_64qam         = cfg.sibs[1].sib2().rr_cfg_common.pusch_cfg_common.pusch_cfg_basic.enable64_qam;
    item.target_pucch_ul_sinr = cfg.cell_list[ccidx].target_pucch_sinr_db;
    item.target_pusch_ul_sinr = cfg.cell_list[ccidx].target_pusch_sinr_db;
    item.enable_phr_handling  = cfg.cell_list[ccidx].enable_phr_handling;
    item.min_phr_thres        = cfg.cell_list[ccidx].min_phr_thres;
    item.delta_pucch_shift    = cfg.sibs[1].sib2().rr_cfg_common.pucch_cfg_common.delta_pucch_shift.to_number();
    item.ncs_an               = cfg.sibs[1].sib2().rr_cfg_common.pucch_cfg_common.ncs_an;
    item.n1pucch_an           = cfg.sibs[1].sib2().rr_cfg_common.pucch_cfg_common.n1_pucch_an;
    item.nrb_cqi              = cfg.sibs[1].sib2().rr_cfg_common.pucch_cfg_common.nrb_cqi;

    item.nrb_pucch = SRSRAN_MAX(cfg.sr_cfg.nof_prb, item.nrb_cqi);
    logger.info("Allocating %d PRBs for PUCCH", item.nrb_pucch);

    // Copy base cell configuration
    item.cell    = cfg.cell;
    item.cell.id = cfg.cell_list[ccidx].pci;

    // copy secondary cell list info
    sched_cfg[ccidx].scell_list.reserve(cfg.cell_list[ccidx].scell_list.size());
    for (uint32_t scidx = 0; scidx < cfg.cell_list[ccidx].scell_list.size(); ++scidx) {
      const auto& scellitem = cfg.cell_list[ccidx].scell_list[scidx];
      // search enb_cc_idx specific to cell_id
      auto it = std::find_if(cfg.cell_list.begin(), cfg.cell_list.end(), [&scellitem](const cell_cfg_t& e) {
        return e.cell_id == scellitem.cell_id;
      });
      if (it == cfg.cell_list.end()) {
        logger.warning("Secondary cell 0x%x not configured", scellitem.cell_id);
        continue;
      }
      sched_interface::cell_cfg_t::scell_cfg_t scellcfg;
      scellcfg.enb_cc_idx               = it - cfg.cell_list.begin();
      scellcfg.ul_allowed               = scellitem.ul_allowed;
      scellcfg.cross_carrier_scheduling = scellitem.cross_carrier_sched;
      sched_cfg[ccidx].scell_list.push_back(scellcfg);
    }
  }

  // Configure MAC scheduler
  mac->cell_cfg(sched_cfg);
}

/* This methods packs the SIBs for each component carrier and stores them
 * inside the sib_buffer, a vector of SIBs for each CC.
 *
 * Before packing the message, it patches the cell specific params of
 * the SIB, including the cellId and the PRACH config index.
 *
 * The number of generates SIB messages is stored in the class member nof_si_messages
 *
 * @return SRSRAN_SUCCESS on success, SRSRAN_ERROR on failure
 */
uint32_t rrc::generate_sibs()
{
  // nof_messages includes SIB2 by default, plus all configured SIBs
  uint32_t           nof_messages = 1 + cfg.sib1.sched_info_list.size();
  sched_info_list_l& sched_info   = cfg.sib1.sched_info_list;

  // Store configs,SIBs in common cell ctxt list
  cell_common_list.reset(new enb_cell_common_list{cfg});

  // generate and pack into SIB buffers
  for (uint32_t cc_idx = 0; cc_idx < cfg.cell_list.size(); cc_idx++) {
    enb_cell_common* cell_ctxt = cell_common_list->get_cc_idx(cc_idx);
    // msg is array of SI messages, each SI message msg[i] may contain multiple SIBs
    // all SIBs in a SI message msg[i] share the same periodicity
    asn1::dyn_array<bcch_dl_sch_msg_s> msg(nof_messages + 1);

    // Copy SIB1 to first SI message
    msg[0].msg.set_c1().set_sib_type1() = cell_ctxt->sib1;

    // Copy rest of SIBs
    for (uint32_t sched_info_elem = 0; sched_info_elem < nof_messages - 1; sched_info_elem++) {
      uint32_t msg_index = sched_info_elem + 1; // first msg is SIB1, therefore start with second

      msg[msg_index].msg.set_c1().set_sys_info().crit_exts.set_sys_info_r8();
      sys_info_r8_ies_s::sib_type_and_info_l_& sib_list =
          msg[msg_index].msg.c1().sys_info().crit_exts.sys_info_r8().sib_type_and_info;

      // SIB2 always in second SI message
      if (msg_index == 1) {
        sib_info_item_c sibitem;
        sibitem.set_sib2() = cell_ctxt->sib2;
        sib_list.push_back(sibitem);
      }

      // Add other SIBs to this message, if any
      for (auto& mapping_enum : sched_info[sched_info_elem].sib_map_info) {
        sib_list.push_back(cfg.sibs[(int)mapping_enum + 2]);
      }
    }

    // Pack payload for all messages
    for (uint32_t msg_index = 0; msg_index < nof_messages; msg_index++) {
      srsran::unique_byte_buffer_t sib_buffer = srsran::make_byte_buffer();
      if (sib_buffer == nullptr) {
        logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
        return SRSRAN_ERROR;
      }
      asn1::bit_ref bref(sib_buffer->msg, sib_buffer->get_tailroom());
      if (msg[msg_index].pack(bref) != asn1::SRSASN_SUCCESS) {
        logger.error("Failed to pack SIB message %d", msg_index);
        return SRSRAN_ERROR;
      }
      sib_buffer->N_bytes = bref.distance_bytes();
      cell_ctxt->sib_buffer.push_back(std::move(sib_buffer));

      // Log SIBs in JSON format
      fmt::memory_buffer membuf;
      const char*        msg_str = msg[msg_index].msg.c1().type().to_string();
      if (msg[msg_index].msg.c1().type().value != asn1::rrc::bcch_dl_sch_msg_type_c::c1_c_::types_opts::sib_type1) {
        msg_str = msg[msg_index].msg.c1().sys_info().crit_exts.type().to_string();
      }
      fmt::format_to(membuf, "{}, cc={}, idx={}", msg_str, cc_idx, msg_index);
      log_broadcast_rrc_message(SRSRAN_SIRNTI, *cell_ctxt->sib_buffer.back(), msg[msg_index], srsran::to_c_str(membuf));
    }

    if (cfg.sibs[6].type() == asn1::rrc::sys_info_r8_ies_s::sib_type_and_info_item_c_::types::sib7) {
      sib7 = cfg.sibs[6].sib7();
    }
  }

  nof_si_messages = nof_messages;

  return SRSRAN_SUCCESS;
}

void rrc::configure_mbsfn_sibs()
{
  // populate struct with sib2 values needed in PHY/MAC
  srsran::sib2_mbms_t sibs2;
  sibs2.mbsfn_sf_cfg_list_present = cfg.sibs[1].sib2().mbsfn_sf_cfg_list_present;
  sibs2.nof_mbsfn_sf_cfg          = cfg.sibs[1].sib2().mbsfn_sf_cfg_list.size();
  for (int i = 0; i < sibs2.nof_mbsfn_sf_cfg; i++) {
    sibs2.mbsfn_sf_cfg_list[i].nof_alloc_subfrs = srsran::mbsfn_sf_cfg_t::sf_alloc_type_t::one_frame;
    sibs2.mbsfn_sf_cfg_list[i].radioframe_alloc_offset =
        cfg.sibs[1].sib2().mbsfn_sf_cfg_list[i].radioframe_alloc_offset;
    sibs2.mbsfn_sf_cfg_list[i].radioframe_alloc_period =
        (srsran::mbsfn_sf_cfg_t::alloc_period_t)cfg.sibs[1].sib2().mbsfn_sf_cfg_list[i].radioframe_alloc_period.value;
    sibs2.mbsfn_sf_cfg_list[i].sf_alloc =
        (uint32_t)cfg.sibs[1].sib2().mbsfn_sf_cfg_list[i].sf_alloc.one_frame().to_number();
  }
  // populate struct with sib13 values needed for PHY/MAC
  srsran::sib13_t sibs13;
  sibs13.notif_cfg.notif_offset = cfg.sibs[12].sib13_v920().notif_cfg_r9.notif_offset_r9;
  sibs13.notif_cfg.notif_repeat_coeff =
      (srsran::mbms_notif_cfg_t::coeff_t)cfg.sibs[12].sib13_v920().notif_cfg_r9.notif_repeat_coeff_r9.value;
  sibs13.notif_cfg.notif_sf_idx = cfg.sibs[12].sib13_v920().notif_cfg_r9.notif_sf_idx_r9;
  sibs13.nof_mbsfn_area_info    = cfg.sibs[12].sib13_v920().mbsfn_area_info_list_r9.size();
  for (uint32_t i = 0; i < sibs13.nof_mbsfn_area_info; i++) {
    sibs13.mbsfn_area_info_list[i].mbsfn_area_id =
        cfg.sibs[12].sib13_v920().mbsfn_area_info_list_r9[i].mbsfn_area_id_r9;
    sibs13.mbsfn_area_info_list[i].notif_ind        = cfg.sibs[12].sib13_v920().mbsfn_area_info_list_r9[i].notif_ind_r9;
    sibs13.mbsfn_area_info_list[i].mcch_cfg.sig_mcs = (srsran::mbsfn_area_info_t::mcch_cfg_t::sig_mcs_t)cfg.sibs[12]
                                                          .sib13_v920()
                                                          .mbsfn_area_info_list_r9[i]
                                                          .mcch_cfg_r9.sig_mcs_r9.value;
    sibs13.mbsfn_area_info_list[i].mcch_cfg.sf_alloc_info =
        cfg.sibs[12].sib13_v920().mbsfn_area_info_list_r9[i].mcch_cfg_r9.sf_alloc_info_r9.to_number();
    sibs13.mbsfn_area_info_list[i].mcch_cfg.mcch_repeat_period =
        (srsran::mbsfn_area_info_t::mcch_cfg_t::repeat_period_t)cfg.sibs[12]
            .sib13_v920()
            .mbsfn_area_info_list_r9[i]
            .mcch_cfg_r9.mcch_repeat_period_r9.value;
    sibs13.mbsfn_area_info_list[i].mcch_cfg.mcch_offset =
        cfg.sibs[12].sib13_v920().mbsfn_area_info_list_r9[i].mcch_cfg_r9.mcch_offset_r9;
    sibs13.mbsfn_area_info_list[i].mcch_cfg.mcch_mod_period =
        (srsran::mbsfn_area_info_t::mcch_cfg_t::mod_period_t)cfg.sibs[12]
            .sib13_v920()
            .mbsfn_area_info_list_r9[i]
            .mcch_cfg_r9.mcch_mod_period_r9.value;
    sibs13.mbsfn_area_info_list[i].non_mbsfn_region_len = (srsran::mbsfn_area_info_t::region_len_t)cfg.sibs[12]
                                                              .sib13_v920()
                                                              .mbsfn_area_info_list_r9[i]
                                                              .non_mbsfn_region_len.value;
    sibs13.mbsfn_area_info_list[i].notif_ind = cfg.sibs[12].sib13_v920().mbsfn_area_info_list_r9[i].notif_ind_r9;
  }

  // pack MCCH for transmission and pass relevant MCCH values to PHY/MAC
  pack_mcch();
  srsran::mcch_msg_t mcch_t;
  mcch_t.common_sf_alloc_period         = srsran::mcch_msg_t::common_sf_alloc_period_t::rf64;
  mcch_t.nof_common_sf_alloc            = 1;
  srsran::mbsfn_sf_cfg_t sf_alloc_item  = mcch_t.common_sf_alloc[0];
  sf_alloc_item.radioframe_alloc_offset = 0;
  sf_alloc_item.radioframe_alloc_period = srsran::mbsfn_sf_cfg_t::alloc_period_t::n1;
  sf_alloc_item.sf_alloc                = 63;
  mcch_t.nof_pmch_info                  = 1;
  srsran::pmch_info_t* pmch_item        = &mcch_t.pmch_info_list[0];

  pmch_item->nof_mbms_session_info              = 1;
  pmch_item->mbms_session_info_list[0].lc_ch_id = 1;
  if (pmch_item->nof_mbms_session_info > 1) {
    pmch_item->mbms_session_info_list[1].lc_ch_id = 2;
  }
  uint16_t mbms_mcs = cfg.mbms_mcs;
  if (mbms_mcs > 28) {
    mbms_mcs = 28; // TS 36.213, Table 8.6.1-1
    logger.warning("PMCH data MCS too high, setting it to 28");
  }
  logger.debug("PMCH data MCS=%d", mbms_mcs);
  pmch_item->data_mcs         = mbms_mcs;
  pmch_item->mch_sched_period = srsran::pmch_info_t::mch_sched_period_t::rf64;
  pmch_item->sf_alloc_end     = 64 * 6;

  // Configure PHY when PHY is done being initialized
  task_sched.defer_task([this, sibs2, sibs13, mcch_t]() mutable {
    phy->configure_mbsfn(&sibs2, &sibs13, mcch_t);
    mac->write_mcch(&sibs2, &sibs13, &mcch_t, mcch_payload_buffer, current_mcch_length);
  });
}

int rrc::pack_mcch()
{
  mcch.msg.set_c1();
  mbsfn_area_cfg_r9_s& area_cfg_r9      = mcch.msg.c1().mbsfn_area_cfg_r9();
  area_cfg_r9.common_sf_alloc_period_r9 = mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_e_::rf64;
  area_cfg_r9.common_sf_alloc_r9.resize(1);
  mbsfn_sf_cfg_s* sf_alloc_item          = &area_cfg_r9.common_sf_alloc_r9[0];
  sf_alloc_item->radioframe_alloc_offset = 0;
  sf_alloc_item->radioframe_alloc_period = mbsfn_sf_cfg_s::radioframe_alloc_period_e_::n1;
  sf_alloc_item->sf_alloc.set_one_frame().from_number(32 + 31);

  area_cfg_r9.pmch_info_list_r9.resize(1);
  pmch_info_r9_s* pmch_item = &area_cfg_r9.pmch_info_list_r9[0];
  pmch_item->mbms_session_info_list_r9.resize(1);

  pmch_item->mbms_session_info_list_r9[0].lc_ch_id_r9           = 1;
  pmch_item->mbms_session_info_list_r9[0].session_id_r9_present = true;
  pmch_item->mbms_session_info_list_r9[0].session_id_r9[0]      = 0;
  pmch_item->mbms_session_info_list_r9[0].tmgi_r9.plmn_id_r9.set_explicit_value_r9();
  srsran::plmn_id_t plmn_obj;
  plmn_obj.from_string("00003");
  srsran::to_asn1(&pmch_item->mbms_session_info_list_r9[0].tmgi_r9.plmn_id_r9.explicit_value_r9(), plmn_obj);
  uint8_t byte[] = {0x0, 0x0, 0x0};
  memcpy(&pmch_item->mbms_session_info_list_r9[0].tmgi_r9.service_id_r9[0], &byte[0], 3);

  if (pmch_item->mbms_session_info_list_r9.size() > 1) {
    pmch_item->mbms_session_info_list_r9[1].lc_ch_id_r9           = 2;
    pmch_item->mbms_session_info_list_r9[1].session_id_r9_present = true;
    pmch_item->mbms_session_info_list_r9[1].session_id_r9[0]      = 1;
    pmch_item->mbms_session_info_list_r9[1].tmgi_r9.plmn_id_r9.set_explicit_value_r9() =
        pmch_item->mbms_session_info_list_r9[0].tmgi_r9.plmn_id_r9.explicit_value_r9();
    byte[2] = 1;
    memcpy(&pmch_item->mbms_session_info_list_r9[1].tmgi_r9.service_id_r9[0],
           &byte[0],
           3); // TODO: Check if service is set to 1
  }

  uint16_t mbms_mcs = cfg.mbms_mcs;
  if (mbms_mcs > 28) {
    mbms_mcs = 28; // TS 36.213, Table 8.6.1-1
    logger.warning("PMCH data MCS too high, setting it to 28");
  }

  logger.debug("PMCH data MCS=%d", mbms_mcs);
  pmch_item->pmch_cfg_r9.data_mcs_r9         = mbms_mcs;
  pmch_item->pmch_cfg_r9.mch_sched_period_r9 = pmch_cfg_r9_s::mch_sched_period_r9_e_::rf64;
  pmch_item->pmch_cfg_r9.sf_alloc_end_r9     = 64 * 6;

  const int     rlc_header_len = 1;
  asn1::bit_ref bref(&mcch_payload_buffer[rlc_header_len], sizeof(mcch_payload_buffer) - rlc_header_len);
  if (mcch.pack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack MCCH message");
  }

  current_mcch_length = bref.distance_bytes(&mcch_payload_buffer[1]);
  current_mcch_length = current_mcch_length + rlc_header_len;
  return current_mcch_length;
}

/*******************************************************************************
  RRC run tti method
*******************************************************************************/

void rrc::tti_clock()
{
  // pop cmds from queue
  rrc_pdu p;
  while (rx_pdu_queue.try_pop(p)) {
    // check if user exists
    auto user_it = users.find(p.rnti);
    if (user_it == users.end()) {
      if (p.pdu != nullptr) {
        log_rx_pdu_fail(p.rnti, p.lcid, *p.pdu, "unknown rnti");
      } else {
        logger.warning("Ignoring rnti=0x%x command %d arg %d. Cause: unknown rnti", p.rnti, p.lcid, p.arg);
      }
      continue;
    }
    ue& ue = *user_it->second;

    // handle queue cmd
    switch (p.lcid) {
      case srb_to_lcid(lte_srb::srb0):
        parse_ul_ccch(ue, std::move(p.pdu));
        break;
      case srb_to_lcid(lte_srb::srb1):
      case srb_to_lcid(lte_srb::srb2):
        parse_ul_dcch(ue, p.lcid, std::move(p.pdu));
        break;
      case LCID_REM_USER:
        rem_user(p.rnti);
        break;
      case LCID_REL_USER:
        process_release_complete(p.rnti);
        break;
      case LCID_ACT_USER:
        user_it->second->set_activity();
        break;
      case LCID_RADLINK_DL:
        user_it->second->set_radiolink_dl_state(p.arg);
        break;
      case LCID_RADLINK_UL:
        user_it->second->set_radiolink_ul_state(p.arg);
        break;
      case LCID_RLC_RTX:
        user_it->second->max_rlc_retx_reached();
        break;
      case LCID_PROT_FAIL:
        user_it->second->protocol_failure();
        break;
      case LCID_EXIT:
        logger.info("Exiting thread");
        break;
      default:
        logger.error("Rx PDU with invalid bearer id: %d", p.lcid);
        break;
    }
  }
}

void rrc::log_rx_pdu_fail(uint16_t rnti, uint32_t lcid, srsran::const_byte_span pdu, const char* cause_str)
{
  logger.error(
      pdu.data(), pdu.size(), "Rx %s PDU, rnti=0x%x - Discarding. Cause: %s", get_rb_name(lcid), rnti, cause_str);
}

void rrc::log_rxtx_pdu_impl(direction_t             dir,
                            uint16_t                rnti,
                            uint32_t                lcid,
                            srsran::const_byte_span pdu,
                            const char*             msg_type)
{
  static const char* dir_str[] = {"Rx", "Tx", "Tx S1AP", "Rx S1AP"};
  fmt::memory_buffer membuf;
  fmt::format_to(membuf, "{} ", dir_str[dir]);
  if (rnti != SRSRAN_PRNTI and rnti != SRSRAN_SIRNTI) {
    if (dir == Tx or dir == Rx) {
      fmt::format_to(membuf, "{} ", srsran::get_srb_name(srsran::lte_lcid_to_srb(lcid)));
    }
    fmt::format_to(membuf, "PDU, rnti=0x{:x} ", rnti);
  } else {
    fmt::format_to(membuf, "Broadcast PDU ");
  }
  fmt::format_to(membuf, "- {} ({} B)", msg_type, pdu.size());

  logger.info(pdu.data(), pdu.size(), "%s", srsran::to_c_str(membuf));
}

} // namespace srsenb
