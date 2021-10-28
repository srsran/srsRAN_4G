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

#include "srsenb/hdr/stack/rrc/rrc_nr.h"
#include "srsenb/hdr/common/common_enb.h"
#include "srsenb/hdr/stack/rrc/nr/cell_asn1_config.h"
#include "srsenb/test/mac/nr/sched_nr_cfg_generators.h"
#include "srsran/asn1/rrc_nr_utils.h"
#include "srsran/common/common_nr.h"
#include "srsran/common/phy_cfg_nr_default.h"
#include "srsran/common/standard_streams.h"

using namespace asn1::rrc_nr;

namespace srsenb {

rrc_nr::rrc_nr(srsran::task_sched_handle task_sched_) :
  logger(srslog::fetch_basic_logger("RRC-NR")), task_sched(task_sched_)
{}

int rrc_nr::init(const rrc_nr_cfg_t&         cfg_,
                 phy_interface_stack_nr*     phy_,
                 mac_interface_rrc_nr*       mac_,
                 rlc_interface_rrc*          rlc_,
                 pdcp_interface_rrc*         pdcp_,
                 ngap_interface_rrc_nr*      ngap_,
                 gtpu_interface_rrc_nr*      gtpu_,
                 rrc_eutra_interface_rrc_nr* rrc_eutra_)
{
  phy       = phy_;
  mac       = mac_;
  rlc       = rlc_;
  pdcp      = pdcp_;
  ngap      = ngap_;
  gtpu      = gtpu_;
  rrc_eutra = rrc_eutra_;

  // TODO: overwriting because we are not passing config right now
  cfg = update_default_cfg(cfg_);

  // derived
  slot_dur_ms = 1;

  if (generate_sibs() != SRSRAN_SUCCESS) {
    logger.error("Couldn't generate SIB messages.");
    return SRSRAN_ERROR;
  }

  // Fill base ASN1 cell config.
  int ret = fill_sp_cell_cfg_from_enb_cfg(cfg, UE_PSCELL_CC_IDX, base_sp_cell_cfg);
  srsran_assert(ret == SRSRAN_SUCCESS, "Failed to configure cell");

  // Fill rrc_nr_cfg with UE-specific search spaces and coresets
  bool ret2 = srsran::fill_phy_pdcch_cfg_common(
      base_sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup(),
      &cfg.cell_list[0].phy_cell.pdcch);
  srsran_assert(ret2, "Invalid NR cell configuration.");
  ret2 = srsran::fill_phy_pdcch_cfg(base_sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.setup(),
                                    &cfg.cell_list[0].phy_cell.pdcch);
  srsran_assert(ret2, "Invalid NR cell configuration.");

  config_phy(); // if PHY is not yet initialized, config will be stored and applied on initialization
  config_mac();

  logger.info("Started");

  running = true;

  return SRSRAN_SUCCESS;
}

void rrc_nr::stop()
{
  if (running) {
    running = false;
  }
  users.clear();
}

template <class T>
void rrc_nr::log_rrc_message(const std::string&           source,
                             const direction_t            dir,
                             const srsran::byte_buffer_t* pdu,
                             const T&                     msg)
{
  if (logger.debug.enabled()) {
    asn1::json_writer json_writer;
    msg.to_json(json_writer);
    logger.debug(pdu->msg,
                 pdu->N_bytes,
                 "%s - %s %s (%d B)",
                 source.c_str(),
                 dir == Tx ? "Tx" : "Rx",
                 msg.msg.c1().type().to_string(),
                 pdu->N_bytes);
    logger.debug("Content:\n%s", json_writer.to_string().c_str());
  } else if (logger.info.enabled()) {
    logger.info(
        "%s - %s %s (%d B)", source.c_str(), dir == Tx ? "Tx" : "Rx", msg.msg.c1().type().to_string(), pdu->N_bytes);
  }
}

rrc_nr_cfg_t rrc_nr::update_default_cfg(const rrc_nr_cfg_t& current)
{
  // NOTE: This function is temporary.
  rrc_nr_cfg_t cfg_default = current;

  // Fill MIB
  cfg_default.mib.sub_carrier_spacing_common.value = mib_s::sub_carrier_spacing_common_opts::scs15or60;
  cfg_default.mib.ssb_subcarrier_offset            = 0;
  cfg_default.mib.intra_freq_resel.value           = mib_s::intra_freq_resel_opts::allowed;
  cfg_default.mib.cell_barred.value                = mib_s::cell_barred_opts::not_barred;
  cfg_default.mib.pdcch_cfg_sib1.search_space_zero = 0;
  cfg_default.mib.pdcch_cfg_sib1.ctrl_res_set_zero = 0;
  cfg_default.mib.dmrs_type_a_position.value       = mib_s::dmrs_type_a_position_opts::pos2;
  cfg_default.mib.sys_frame_num.from_number(0);

  // Fill SIB1
  cfg_default.sib1.cell_access_related_info.plmn_id_list.resize(1);
  cfg_default.sib1.cell_access_related_info.plmn_id_list[0].plmn_id_list.resize(1);
  srsran::plmn_id_t plmn;
  plmn.from_string("90170");
  srsran::to_asn1(&cfg_default.sib1.cell_access_related_info.plmn_id_list[0].plmn_id_list[0], plmn);
  cfg_default.sib1.cell_access_related_info.plmn_id_list[0].cell_id.from_number(1);
  cfg_default.sib1.cell_access_related_info.plmn_id_list[0].cell_reserved_for_oper.value =
      plmn_id_info_s::cell_reserved_for_oper_opts::not_reserved;
  cfg_default.sib1.si_sched_info_present                                  = true;
  cfg_default.sib1.si_sched_info.si_request_cfg.rach_occasions_si_present = true;
  cfg_default.sib1.si_sched_info.si_request_cfg.rach_occasions_si.rach_cfg_si.ra_resp_win.value =
      rach_cfg_generic_s::ra_resp_win_opts::sl8;
  cfg_default.sib1.si_sched_info.si_win_len.value = si_sched_info_s::si_win_len_opts::s20;
  cfg_default.sib1.si_sched_info.sched_info_list.resize(1);
  cfg_default.sib1.si_sched_info.sched_info_list[0].si_broadcast_status.value =
      sched_info_s::si_broadcast_status_opts::broadcasting;
  cfg_default.sib1.si_sched_info.sched_info_list[0].si_periodicity.value = sched_info_s::si_periodicity_opts::rf16;
  cfg_default.sib1.si_sched_info.sched_info_list[0].sib_map_info.resize(1);
  // scheduling of SI messages
  cfg_default.sib1.si_sched_info.sched_info_list[0].sib_map_info[0].type.value = sib_type_info_s::type_opts::sib_type2;
  cfg_default.sib1.si_sched_info.sched_info_list[0].sib_map_info[0].value_tag_present = true;
  cfg_default.sib1.si_sched_info.sched_info_list[0].sib_map_info[0].value_tag         = 0;

  // Fill SIB2+
  cfg_default.nof_sibs                     = 1;
  sib2_s& sib2                             = cfg_default.sibs[0].set_sib2();
  sib2.cell_resel_info_common.q_hyst.value = sib2_s::cell_resel_info_common_s_::q_hyst_opts::db5;
  // TODO: Fill SIB2 values

  return cfg_default;
}

/* @brief PRIVATE function, gets called by sgnb_addition_request
 *
 * This function WILL NOT TRIGGER the RX MSG3 activity timer
 */
int rrc_nr::add_user(uint16_t rnti, const sched_nr_ue_cfg_t& uecfg, bool start_msg3_timer)
{
  if (users.count(rnti) == 0) {
    // If in the ue ctor, "start_msg3_timer" is set to true, this will start the MSG3 RX TIMEOUT at ue creation
    users.insert(std::make_pair(rnti, std::unique_ptr<ue>(new ue(this, rnti, uecfg, start_msg3_timer))));
    rlc->add_user(rnti);
    pdcp->add_user(rnti);
    logger.info("Added new user rnti=0x%x", rnti);
    return SRSRAN_SUCCESS;
  } else {
    logger.error("Adding user rnti=0x%x (already exists)", rnti);
    return SRSRAN_ERROR;
  }
}

/* @brief PUBLIC function, gets called by mac_nr::rach_detected
 *
 * This function is called from PRACH worker (can wait) and WILL TRIGGER the RX MSG3 activity timer
 */
int rrc_nr::add_user(uint16_t rnti, const sched_nr_ue_cfg_t& uecfg)
{
  // Set "triggered_by_rach" to true to start the MSG3 RX TIMEOUT
  return add_user(rnti, uecfg, true);
}

void rrc_nr::rem_user(uint16_t rnti)
{
  auto user_it = users.find(rnti);
  if (user_it != users.end()) {
    // First remove MAC and GTPU to stop processing DL/UL traffic for this user
    mac->remove_ue(rnti); // MAC handles PHY
    rlc->rem_user(rnti);
    pdcp->rem_user(rnti);
    users.erase(rnti);

    srsran::console("Disconnecting rnti=0x%x.\n", rnti);
    logger.info("Removed user rnti=0x%x", rnti);
  } else {
    logger.error("Removing user rnti=0x%x (does not exist)", rnti);
  }
}

/* Function called by MAC after the reception of a C-RNTI CE indicating that the UE still has a
 * valid RNTI.
 */
int rrc_nr::update_user(uint16_t new_rnti, uint16_t old_rnti)
{
  if (new_rnti == old_rnti) {
    logger.warning("rnti=0x%x received MAC CRNTI CE with same rnti", new_rnti);
    return SRSRAN_ERROR;
  }

  // Remove new_rnti
  auto new_ue_it = users.find(new_rnti);
  if (new_ue_it != users.end()) {
    new_ue_it->second->deactivate_bearers();
    task_sched.defer_task([this, new_rnti]() { rem_user(new_rnti); });
  }

  // Send Reconfiguration to old_rnti if is RRC_CONNECT or RRC Release if already released here
  auto old_it = users.find(old_rnti);
  if (old_it == users.end()) {
    logger.info("rnti=0x%x received MAC CRNTI CE: 0x%x, but old context is unavailable", new_rnti, old_rnti);
    return SRSRAN_ERROR;
  }
  ue* ue_ptr = old_it->second.get();

  logger.info("Resuming rnti=0x%x RRC connection due to received C-RNTI CE from rnti=0x%x.", old_rnti, new_rnti);
  ue_ptr->crnti_ce_received();

  return SRSRAN_SUCCESS;
}

void rrc_nr::set_activity_user(uint16_t rnti)
{
  auto it = users.find(rnti);
  if (it == users.end()) {
    logger.info("rnti=0x%x not found. Can't set activity", rnti);
    return;
  }
  ue* ue_ptr = it->second.get();

  // inform EUTRA RRC about user activity
  if (ue_ptr->is_endc()) {
    // Restart inactivity timer for RRC-NR
    ue_ptr->set_activity();
    // inform EUTRA RRC about user activity
    rrc_eutra->set_activity_user(ue_ptr->get_eutra_rnti());
  }
}

void rrc_nr::config_phy()
{
  srsenb::phy_interface_rrc_nr::common_cfg_t common_cfg = {};
  common_cfg.carrier                                    = cfg.cell_list[0].phy_cell.carrier;
  common_cfg.pdcch                                      = cfg.cell_list[0].phy_cell.pdcch;
  common_cfg.prach                                      = cfg.cell_list[0].phy_cell.prach;
  common_cfg.duplex_mode                                = cfg.cell_list[0].duplex_mode;
  common_cfg.ssb                                        = cfg.cell_list[0].ssb_cfg;
  if (phy->set_common_cfg(common_cfg) < SRSRAN_SUCCESS) {
    logger.error("Couldn't set common PHY config");
    return;
  }
}

void rrc_nr::config_mac()
{
  // Fill MAC scheduler configuration for SIBs
  // TODO: use parsed cell NR cfg configuration
  std::vector<srsenb::sched_nr_interface::cell_cfg_t> sched_cells_cfg = {srsenb::get_default_cells_cfg(1)};
  sched_nr_interface::cell_cfg_t&                     cell            = sched_cells_cfg[0];

  // Derive cell config from rrc_nr_cfg_t
  cell.bwps[0].pdcch = cfg.cell_list[0].phy_cell.pdcch;
  // Derive cell config from ASN1
  bool ret2 = srsran::make_pdsch_cfg_from_serv_cell(base_sp_cell_cfg.sp_cell_cfg_ded, &cell.bwps[0].pdsch);
  srsran_assert(ret2, "Invalid NR cell configuration.");
  ret2 = srsran::make_phy_ssb_cfg(
      cfg.cell_list[0].phy_cell.carrier, base_sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common, &cell.ssb);
  srsran_assert(ret2, "Invalid NR cell configuration.");
  ret2 = srsran::make_duplex_cfg_from_serv_cell(base_sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common, &cell.duplex);
  srsran_assert(ret2, "Invalid NR cell configuration.");

  // FIXME: entire SI configuration, etc needs to be ported to NR
  sched_interface::cell_cfg_t cell_cfg;
  set_sched_cell_cfg_sib1(&cell_cfg, cfg.sib1);

  // set SIB length
  for (uint32_t i = 0; i < nof_si_messages + 1; i++) {
    cell_cfg.sibs[i].len = sib_buffer[i]->N_bytes;
  }

  // PUCCH width
  cell_cfg.nrb_pucch = SRSRAN_MAX(cfg.sr_cfg.nof_prb, /* TODO: where is n_rb2 in NR? */ 0);
  logger.info("Allocating %d PRBs for PUCCH", cell_cfg.nrb_pucch);

  // Copy Cell configuration
  // cell_cfg.cell = cfg.cell;

  // Configure MAC/scheduler
  mac->cell_cfg(sched_cells_cfg);
}

int32_t rrc_nr::generate_sibs()
{
  // MIB packing
  bcch_bch_msg_s mib_msg;
  mib_s&         mib = mib_msg.msg.set_mib();
  mib                = cfg.mib;
  {
    srsran::unique_byte_buffer_t mib_buf = srsran::make_byte_buffer();
    if (mib_buf == nullptr) {
      logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return SRSRAN_ERROR;
    }
    asn1::bit_ref bref(mib_buf->msg, mib_buf->get_tailroom());
    if (mib_msg.pack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Couldn't pack mib msg");
      return SRSRAN_ERROR;
    }
    mib_buf->N_bytes = bref.distance_bytes();
    logger.debug(mib_buf->msg, mib_buf->N_bytes, "MIB payload (%d B)", mib_buf->N_bytes);
    mib_buffer = std::move(mib_buf);
  }

  si_sched_info_s::sched_info_list_l_& sched_info = cfg.sib1.si_sched_info.sched_info_list;
  uint32_t nof_messages = cfg.sib1.si_sched_info_present ? cfg.sib1.si_sched_info.sched_info_list.size() : 0;

  // msg is array of SI messages, each SI message msg[i] may contain multiple SIBs
  // all SIBs in a SI message msg[i] share the same periodicity
  sib_buffer.reserve(nof_messages + 1);
  asn1::dyn_array<bcch_dl_sch_msg_s> msg(nof_messages + 1);

  // Copy SIB1 to first SI message
  msg[0].msg.set_c1().set_sib_type1() = cfg.sib1;

  // Copy rest of SIBs
  for (uint32_t sched_info_elem = 0; sched_info_elem < nof_messages; sched_info_elem++) {
    uint32_t msg_index = sched_info_elem + 1; // first msg is SIB1, therefore start with second

    msg[msg_index].msg.set_c1().set_sys_info().crit_exts.set_sys_info();
    auto& sib_list = msg[msg_index].msg.c1().sys_info().crit_exts.sys_info().sib_type_and_info;

    for (uint32_t mapping = 0; mapping < sched_info[sched_info_elem].sib_map_info.size(); ++mapping) {
      uint32_t sibidx = sched_info[sched_info_elem].sib_map_info[mapping].type; // SIB2 == 0
      sib_list.push_back(cfg.sibs[sibidx]);
    }
  }

  // Pack payload for all messages
  for (uint32_t msg_index = 0; msg_index < nof_messages + 1; msg_index++) {
    srsran::unique_byte_buffer_t sib = srsran::make_byte_buffer();
    if (sib == nullptr) {
      logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return SRSRAN_ERROR;
    }
    asn1::bit_ref bref(sib->msg, sib->get_tailroom());
    if (msg[msg_index].pack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Failed to pack SIB message %d", msg_index);
      return SRSRAN_ERROR;
    }
    sib->N_bytes = bref.distance_bytes();
    sib_buffer.push_back(std::move(sib));

    // Log SIBs in JSON format
    fmt::memory_buffer strbuf;
    fmt::format_to(strbuf, "SI message={} payload", msg_index);
    log_rrc_message(fmt::to_string(strbuf), Tx, sib_buffer.back().get(), msg[msg_index]);
  }

  nof_si_messages = sib_buffer.size() - 1;

  return SRSRAN_SUCCESS;
}

/*******************************************************************************
  MAC interface
*******************************************************************************/

int rrc_nr::read_pdu_bcch_bch(const uint32_t tti, srsran::unique_byte_buffer_t& buffer)
{
  if (mib_buffer == nullptr || buffer->get_tailroom() < mib_buffer->N_bytes) {
    return SRSRAN_ERROR;
  }
  memcpy(buffer->msg, mib_buffer->msg, mib_buffer->N_bytes);
  buffer->N_bytes = mib_buffer->N_bytes;
  return SRSRAN_SUCCESS;
}

int rrc_nr::read_pdu_bcch_dlsch(uint32_t sib_index, srsran::unique_byte_buffer_t& buffer)
{
  if (sib_index >= sib_buffer.size()) {
    logger.error("SIB %d is not a configured SIB.", sib_index);
    return SRSRAN_ERROR;
  }

  if (buffer->get_tailroom() < sib_buffer[sib_index]->N_bytes) {
    logger.error("Not enough space to fit SIB %d into buffer (%d < %d)",
                 sib_index,
                 buffer->get_tailroom(),
                 sib_buffer[sib_index]->N_bytes);
    return SRSRAN_ERROR;
  }

  memcpy(buffer->msg, sib_buffer[sib_index]->msg, sib_buffer[sib_index]->N_bytes);
  buffer->N_bytes = sib_buffer[sib_index]->N_bytes;

  return SRSRAN_SUCCESS;
}

void rrc_nr::get_metrics(srsenb::rrc_metrics_t& m)
{
  if (running) {
    for (auto& ue : users) {
      rrc_ue_metrics_t ue_metrics;
      ue.second->get_metrics(ue_metrics);
      m.ues.push_back(ue_metrics);
    }
  }
}

void rrc_nr::handle_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  if (pdu) {
    logger.info(pdu->msg, pdu->N_bytes, "Rx %s PDU", get_rb_name(lcid));
  }

  if (users.count(rnti) == 1) {
    switch (static_cast<srsran::nr_srb>(lcid)) {
      case srsran::nr_srb::srb0:
        //        parse_ul_ccch(rnti, std::move(pdu));
        break;
      case srsran::nr_srb::srb1:
      case srsran::nr_srb::srb2:
        //        parse_ul_dcch(p.rnti, p.lcid, std::move(p.pdu));
        break;
      default:
        logger.error("Rx PDU with invalid bearer id: %d", lcid);
        break;
    }
  } else {
    logger.warning("Discarding PDU for removed rnti=0x%x", rnti);
  }
}

/*******************************************************************************
  PDCP interface
*******************************************************************************/
void rrc_nr::write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  handle_pdu(rnti, lcid, std::move(pdu));
}

void rrc_nr::notify_pdcp_integrity_error(uint16_t rnti, uint32_t lcid) {}

/*******************************************************************************
  NGAP interface
*******************************************************************************/

int rrc_nr::ue_set_security_cfg_key(uint16_t rnti, const asn1::fixed_bitstring<256, false, true>& key)
{
  return SRSRAN_SUCCESS;
}
int rrc_nr::ue_set_bitrates(uint16_t rnti, const asn1::ngap_nr::ue_aggregate_maximum_bit_rate_s& rates)
{
  return SRSRAN_SUCCESS;
}
int rrc_nr::set_aggregate_max_bitrate(uint16_t rnti, const asn1::ngap_nr::ue_aggregate_maximum_bit_rate_s& rates)
{
  return SRSRAN_SUCCESS;
}
int rrc_nr::ue_set_security_cfg_capabilities(uint16_t rnti, const asn1::ngap_nr::ue_security_cap_s& caps)
{
  return SRSRAN_SUCCESS;
}
int rrc_nr::start_security_mode_procedure(uint16_t rnti)
{
  return SRSRAN_SUCCESS;
}
int rrc_nr::establish_rrc_bearer(uint16_t rnti, uint16_t pdu_session_id, srsran::const_byte_span nas_pdu, uint32_t lcid)
{
  return SRSRAN_SUCCESS;
}

int rrc_nr::release_bearers(uint16_t rnti)
{
  return SRSRAN_SUCCESS;
}

int rrc_nr::allocate_lcid(uint16_t rnti)
{
  return SRSRAN_SUCCESS;
}

void rrc_nr::write_dl_info(uint16_t rnti, srsran::unique_byte_buffer_t sdu) {}

/*******************************************************************************
  Interface for EUTRA RRC
*******************************************************************************/

void rrc_nr::sgnb_addition_request(uint16_t eutra_rnti, const sgnb_addition_req_params_t& params)
{
  // try to allocate new user
  sched_nr_ue_cfg_t uecfg{};
  uecfg.carriers.resize(1);
  uecfg.carriers[0].active      = true;
  uecfg.carriers[0].cc          = 0;
  uecfg.ue_bearers[0].direction = mac_lc_ch_cfg_t::BOTH;
  srsran::phy_cfg_nr_default_t::reference_cfg_t ref_args{};
  ref_args.duplex   = cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_TDD
                          ? srsran::phy_cfg_nr_default_t::reference_cfg_t::R_DUPLEX_TDD_CUSTOM_6_4
                          : srsran::phy_cfg_nr_default_t::reference_cfg_t::R_DUPLEX_FDD;
  uecfg.phy_cfg     = srsran::phy_cfg_nr_default_t{ref_args};
  uecfg.phy_cfg.csi = {}; // disable CSI until RA is complete

  uint16_t nr_rnti = mac->reserve_rnti(0, uecfg);
  if (nr_rnti == SRSRAN_INVALID_RNTI) {
    logger.error("Failed to allocate RNTI at MAC");
    rrc_eutra->sgnb_addition_reject(eutra_rnti);
    return;
  }

  if (add_user(nr_rnti, uecfg, false) != SRSRAN_SUCCESS) {
    logger.error("Failed to allocate RNTI at RRC");
    rrc_eutra->sgnb_addition_reject(eutra_rnti);
    return;
  }

  // new RNTI is now registered at MAC and RRC
  auto user_it = users.find(nr_rnti);
  if (user_it == users.end()) {
    logger.warning("Unrecognised rnti: 0x%x", nr_rnti);
    return;
  }
  user_it->second->handle_sgnb_addition_request(eutra_rnti, params);
}

void rrc_nr::sgnb_reconfiguration_complete(uint16_t eutra_rnti, const asn1::dyn_octstring& reconfig_response)
{
  // user has completeted the reconfiguration and has acked on 4G side, wait until RA on NR
  logger.info("Received Reconfiguration complete for RNTI=0x%x", eutra_rnti);
}

void rrc_nr::sgnb_release_request(uint16_t nr_rnti)
{
  // remove user
  auto     it         = users.find(nr_rnti);
  uint16_t eutra_rnti = it != users.end() ? it->second->get_eutra_rnti() : SRSRAN_INVALID_RNTI;
  rem_user(nr_rnti);
  if (eutra_rnti != SRSRAN_INVALID_RNTI) {
    rrc_eutra->sgnb_release_ack(eutra_rnti);
  }
}

/*******************************************************************************
  UE class

  Every function in UE class is called from a mutex environment thus does not
  need extra protection.
*******************************************************************************/
rrc_nr::ue::ue(rrc_nr* parent_, uint16_t rnti_, const sched_nr_ue_cfg_t& uecfg_, bool start_msg3_timer) :
  parent(parent_), rnti(rnti_), uecfg(uecfg_)
{
  // Derive UE cfg from rrc_cfg_nr_t
  uecfg.phy_cfg.pdcch = parent->cfg.cell_list[0].phy_cell.pdcch;

  // Set timer for MSG3_RX_TIMEOUT or UE_INACTIVITY_TIMEOUT
  activity_timer = parent->task_sched.get_unique_timer();
  start_msg3_timer ? set_activity_timeout(MSG3_RX_TIMEOUT) : set_activity_timeout(MSG5_RX_TIMEOUT);
}

void rrc_nr::ue::set_activity_timeout(activity_timeout_type_t type)
{
  uint32_t deadline_ms = 0;

  switch (type) {
    case MSG3_RX_TIMEOUT:
      // TODO: Retrieve the parameters from somewhere(RRC?) - Currently hardcoded to 100ms
      deadline_ms = 100;
      break;
    case MSG5_RX_TIMEOUT:
      // TODO: Retrieve the parameters from somewhere(RRC?) - Currently hardcoded to 1s
      deadline_ms = 5000;
      break;
    case UE_INACTIVITY_TIMEOUT:
      // TODO: Retrieve the parameters from somewhere(RRC?) - Currently hardcoded to 5s
      deadline_ms = 10000;
      break;
    default:
      parent->logger.error("Unknown timeout type %d", type);
      return;
  }

  activity_timer.set(deadline_ms, [this, type](uint32_t tid) { activity_timer_expired(type); });
  parent->logger.debug("Setting timer for %s for rnti=0x%x to %dms", to_string(type).c_str(), rnti, deadline_ms);

  set_activity();
}

void rrc_nr::ue::set_activity(bool enabled)
{
  if (not enabled) {
    if (activity_timer.is_running()) {
      parent->logger.debug("Inactivity timer interrupted for rnti=0x%x", rnti);
    }
    activity_timer.stop();
    return;
  }

  // re-start activity timer with current timeout value
  activity_timer.run();
  parent->logger.debug("Activity registered for rnti=0x%x (timeout_value=%dms)", rnti, activity_timer.duration());
}

void rrc_nr::ue::activity_timer_expired(const activity_timeout_type_t type)
{
  parent->logger.info("Activity timer for rnti=0x%x expired after %d ms", rnti, activity_timer.time_elapsed());

  switch (type) {
    case MSG5_RX_TIMEOUT:
    case UE_INACTIVITY_TIMEOUT:
      state = rrc_nr_state_t::RRC_INACTIVE;
      parent->rrc_eutra->sgnb_inactivity_timeout(eutra_rnti);
      break;
    case MSG3_RX_TIMEOUT: {
      // MSG3 timeout, no need to notify NGAP or LTE stack. Just remove UE
      state = rrc_nr_state_t::RRC_IDLE;
      uint32_t rnti_to_rem = rnti;
      parent->task_sched.defer_task([this, rnti_to_rem]() { parent->rem_user(rnti_to_rem); });
      break;
    }
    default:
      // Unhandled activity timeout, just remove UE and log an error
      parent->rem_user(rnti);
      parent->logger.error(
          "Unhandled reason for activity timer expiration. rnti=0x%x, cause %d", rnti, static_cast<unsigned>(type));
  }
}

std::string rrc_nr::ue::to_string(const activity_timeout_type_t& type)
{
  constexpr static const char* options[] = {"Msg3 reception", "UE inactivity", "Msg5 reception"};
  return srsran::enum_to_text(options, (uint32_t)activity_timeout_type_t::nulltype, (uint32_t)type);
}

void rrc_nr::ue::send_connection_setup()
{
  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set_c1().set_rrc_setup().rrc_transaction_id = ((transaction_id++) % 4u);
  rrc_setup_ies_s&    setup  = dl_ccch_msg.msg.c1().rrc_setup().crit_exts.set_rrc_setup();
  radio_bearer_cfg_s& rr_cfg = setup.radio_bearer_cfg;

  // Add DRB1 to cfg
  rr_cfg.drb_to_add_mod_list_present = true;
  rr_cfg.drb_to_add_mod_list.resize(1);
  auto& drb_item                               = rr_cfg.drb_to_add_mod_list[0];
  drb_item.drb_id                              = 1;
  drb_item.pdcp_cfg_present                    = true;
  drb_item.pdcp_cfg.ciphering_disabled_present = true;
  //  drb_item.cn_assoc_present = true;
  //  drb_item.cn_assoc.set_eps_bearer_id() = ;
  drb_item.recover_pdcp_present = false;

  // TODO: send config to RLC/PDCP

  send_dl_ccch(&dl_ccch_msg);
}

void rrc_nr::ue::send_dl_ccch(dl_ccch_msg_s* dl_ccch_msg)
{
  // Allocate a new PDU buffer, pack the message and send to PDCP
  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    parent->logger.error("Allocating pdu");
  }
  asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
  if (dl_ccch_msg->pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
    parent->logger.error("Failed to pack DL-CCCH message. Discarding msg.");
  }
  pdu->N_bytes = bref.distance_bytes();

  char buf[32] = {};
  sprintf(buf, "SRB0 - rnti=0x%x", rnti);
  parent->log_rrc_message(buf, Tx, pdu.get(), *dl_ccch_msg);
  parent->rlc->write_sdu(rnti, (uint32_t)srsran::nr_srb::srb0, std::move(pdu));
}

int rrc_nr::ue::pack_secondary_cell_group_rlc_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  // RLC for DRB1 (with fixed LCID)
  cell_group_cfg_pack.rlc_bearer_to_add_mod_list_present = true;
  cell_group_cfg_pack.rlc_bearer_to_add_mod_list.resize(1);
  auto& rlc_bearer                       = cell_group_cfg_pack.rlc_bearer_to_add_mod_list[0];
  rlc_bearer.lc_ch_id                    = drb1_lcid;
  rlc_bearer.served_radio_bearer_present = true;
  rlc_bearer.served_radio_bearer.set_drb_id();
  rlc_bearer.served_radio_bearer.drb_id() = 1;
  rlc_bearer.rlc_cfg_present              = true;
  rlc_bearer.rlc_cfg.set_um_bi_dir();
  rlc_bearer.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len_present = true;
  rlc_bearer.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len         = sn_field_len_um_opts::size12;
  rlc_bearer.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len_present = true;
  rlc_bearer.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len         = sn_field_len_um_opts::size12;
  rlc_bearer.rlc_cfg.um_bi_dir().dl_um_rlc.t_reassembly         = t_reassembly_opts::ms50;

  // MAC logical channel config
  rlc_bearer.mac_lc_ch_cfg_present                    = true;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params_present = true;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params.prio    = 11;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params.prioritised_bit_rate =
      asn1::rrc_nr::lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::kbps0;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params.bucket_size_dur =
      asn1::rrc_nr::lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::ms100;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params.lc_ch_group_present      = true;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params.lc_ch_group              = 6;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params.sched_request_id_present = true;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params.sched_request_id         = 0;

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_secondary_cell_group_mac_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  // mac-CellGroup-Config for BSR and SR
  cell_group_cfg_pack.mac_cell_group_cfg_present                         = true;
  auto& mac_cell_group                                                   = cell_group_cfg_pack.mac_cell_group_cfg;
  mac_cell_group.sched_request_cfg_present                               = true;
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list_present = true;
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list.resize(1);
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list[0].sched_request_id = 0;
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list[0].sr_trans_max =
      asn1::rrc_nr::sched_request_to_add_mod_s::sr_trans_max_opts::n64;
  mac_cell_group.bsr_cfg_present            = true;
  mac_cell_group.bsr_cfg.periodic_bsr_timer = asn1::rrc_nr::bsr_cfg_s::periodic_bsr_timer_opts::sf20;
  mac_cell_group.bsr_cfg.retx_bsr_timer     = asn1::rrc_nr::bsr_cfg_s::retx_bsr_timer_opts::sf320;

  // Skip TAG and PHR config
  mac_cell_group.tag_cfg_present                     = false;
  mac_cell_group.tag_cfg.tag_to_add_mod_list_present = true;
  mac_cell_group.tag_cfg.tag_to_add_mod_list.resize(1);
  mac_cell_group.tag_cfg.tag_to_add_mod_list[0].tag_id           = 0;
  mac_cell_group.tag_cfg.tag_to_add_mod_list[0].time_align_timer = time_align_timer_opts::infinity;

  mac_cell_group.phr_cfg_present = false;
  mac_cell_group.phr_cfg.set_setup();
  mac_cell_group.phr_cfg.setup().phr_periodic_timer       = asn1::rrc_nr::phr_cfg_s::phr_periodic_timer_opts::sf500;
  mac_cell_group.phr_cfg.setup().phr_prohibit_timer       = asn1::rrc_nr::phr_cfg_s::phr_prohibit_timer_opts::sf200;
  mac_cell_group.phr_cfg.setup().phr_tx_pwr_factor_change = asn1::rrc_nr::phr_cfg_s::phr_tx_pwr_factor_change_opts::db3;
  mac_cell_group.phr_cfg.setup().multiple_phr             = true;
  mac_cell_group.phr_cfg.setup().dummy                    = false;
  mac_cell_group.phr_cfg.setup().phr_type2_other_cell     = false;
  mac_cell_group.phr_cfg.setup().phr_mode_other_cg        = asn1::rrc_nr::phr_cfg_s::phr_mode_other_cg_opts::real;

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_sp_cell_cfg_ded_init_dl_bwp(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp_present = true;

  pack_sp_cell_cfg_ded_init_dl_bwp_pdsch_cfg(cell_group_cfg_pack);
  pack_sp_cell_cfg_ded_init_dl_bwp_radio_link_monitoring(cell_group_cfg_pack);

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_sp_cell_cfg_ded_init_dl_bwp_radio_link_monitoring(
    asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.radio_link_monitoring_cfg_present = true;
  auto& radio_link_monitoring = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.radio_link_monitoring_cfg;
  radio_link_monitoring.set_setup().fail_detection_res_to_add_mod_list_present = true;

  // add resource to detect RLF
  radio_link_monitoring.set_setup().fail_detection_res_to_add_mod_list.resize(1);
  auto& fail_detec_res_elem = radio_link_monitoring.set_setup().fail_detection_res_to_add_mod_list[0];
  fail_detec_res_elem.radio_link_monitoring_rs_id = 0;
  fail_detec_res_elem.purpose                     = asn1::rrc_nr::radio_link_monitoring_rs_s::purpose_opts::rlf;
  fail_detec_res_elem.detection_res.set_ssb_idx() = 0;

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_sp_cell_cfg_ded_init_dl_bwp_pdsch_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg_present = true;
  auto& pdsch_cfg_dedicated = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg;

  pdsch_cfg_dedicated.set_setup();
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a_present = true;
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a.set_setup();
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a.setup().dmrs_add_position_present = true;
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a.setup().dmrs_add_position =
      asn1::rrc_nr::dmrs_dl_cfg_s::dmrs_add_position_opts::pos1;
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list_present = true;
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list.resize(1);
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].tci_state_id = 0;
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].qcl_type1.ref_sig.set_ssb();
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].qcl_type1.ref_sig.ssb() = 0;
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].qcl_type1.qcl_type =
      asn1::rrc_nr::qcl_info_s::qcl_type_opts::type_d;
  pdsch_cfg_dedicated.setup().res_alloc = pdsch_cfg_s::res_alloc_opts::res_alloc_type1;
  pdsch_cfg_dedicated.setup().rbg_size  = asn1::rrc_nr::pdsch_cfg_s::rbg_size_opts::cfg1;
  pdsch_cfg_dedicated.setup().prb_bundling_type.set_static_bundling();
  pdsch_cfg_dedicated.setup().prb_bundling_type.static_bundling().bundle_size_present = true;
  pdsch_cfg_dedicated.setup().prb_bundling_type.static_bundling().bundle_size =
      asn1::rrc_nr::pdsch_cfg_s::prb_bundling_type_c_::static_bundling_s_::bundle_size_opts::wideband;

  // ZP-CSI
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list_present = false;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list.resize(1);
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].zp_csi_rs_res_id = 0;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_domain_alloc.set_row4();
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_domain_alloc.row4().from_number(0b100);
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.nrof_ports =
      asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p4;

  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.first_ofdm_symbol_in_time_domain = 8;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.cdm_type =
      asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::fd_cdm2;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.density.set_one();

  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_band.start_rb     = 0;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_band.nrof_rbs     = 52;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset_present = true;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset.set_slots80();
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset.slots80() = 1;
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set_present                                       = false;
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set.set_setup();
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set.setup().zp_csi_rs_res_set_id = 0;
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set.setup().zp_csi_rs_res_id_list.resize(1);

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_sp_cell_cfg_ded_ul_cfg_init_ul_bwp_pucch_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  // PUCCH
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg_present = true;
  auto& pucch_cfg = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg;

  pucch_cfg.set_setup();
  pucch_cfg.setup().format2_present = true;
  pucch_cfg.setup().format2.set_setup();
  pucch_cfg.setup().format2.setup().max_code_rate_present = true;
  pucch_cfg.setup().format2.setup().max_code_rate         = pucch_max_code_rate_opts::zero_dot25;

  // SR resources
  pucch_cfg.setup().sched_request_res_to_add_mod_list_present = true;
  pucch_cfg.setup().sched_request_res_to_add_mod_list.resize(1);
  auto& sr_res1                             = pucch_cfg.setup().sched_request_res_to_add_mod_list[0];
  sr_res1.sched_request_res_id              = 1;
  sr_res1.sched_request_id                  = 0;
  sr_res1.periodicity_and_offset_present    = true;
  sr_res1.periodicity_and_offset.set_sl40() = 8;
  sr_res1.res_present                       = true;
  sr_res1.res                               = 2; // PUCCH resource for SR

  // DL data
  pucch_cfg.setup().dl_data_to_ul_ack_present = true;

  if (parent->cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
    pucch_cfg.setup().dl_data_to_ul_ack.resize(1);
    pucch_cfg.setup().dl_data_to_ul_ack[0] = 4;
  } else {
    pucch_cfg.setup().dl_data_to_ul_ack.resize(6);
    pucch_cfg.setup().dl_data_to_ul_ack[0] = 6;
    pucch_cfg.setup().dl_data_to_ul_ack[1] = 5;
    pucch_cfg.setup().dl_data_to_ul_ack[2] = 4;
    pucch_cfg.setup().dl_data_to_ul_ack[3] = 4;
    pucch_cfg.setup().dl_data_to_ul_ack[4] = 4;
    pucch_cfg.setup().dl_data_to_ul_ack[5] = 4;
  }

  // PUCCH Resource for format 1
  srsran_pucch_nr_resource_t resource_small = {};
  resource_small.starting_prb               = 0;
  resource_small.format                     = SRSRAN_PUCCH_NR_FORMAT_1;
  resource_small.initial_cyclic_shift       = 0;
  resource_small.nof_symbols                = 14;
  resource_small.start_symbol_idx           = 0;
  resource_small.time_domain_occ            = 0;

  // PUCCH Resource for format 2
  srsran_pucch_nr_resource_t resource_big = {};
  resource_big.starting_prb               = 51;
  resource_big.format                     = SRSRAN_PUCCH_NR_FORMAT_2;
  resource_big.nof_prb                    = 1;
  resource_big.nof_symbols                = 2;
  resource_big.start_symbol_idx           = 12;

  // Resource for SR
  srsran_pucch_nr_resource_t resource_sr = {};
  resource_sr.starting_prb               = 51;
  resource_sr.format                     = SRSRAN_PUCCH_NR_FORMAT_1;
  resource_sr.initial_cyclic_shift       = 0;
  resource_sr.nof_symbols                = 14;
  resource_sr.start_symbol_idx           = 0;
  resource_sr.time_domain_occ            = 0;

  // Make 3 possible resources
  pucch_cfg.setup().res_to_add_mod_list_present = true;
  pucch_cfg.setup().res_to_add_mod_list.resize(3);
  if (not srsran::make_phy_res_config(resource_small, pucch_cfg.setup().res_to_add_mod_list[0], 0)) {
    parent->logger.warning("Failed to create 1-2 bit NR PUCCH resource");
  }
  if (not srsran::make_phy_res_config(resource_big, pucch_cfg.setup().res_to_add_mod_list[1], 1)) {
    parent->logger.warning("Failed to create >2 bit NR PUCCH resource");
  }
  if (not srsran::make_phy_res_config(resource_sr, pucch_cfg.setup().res_to_add_mod_list[2], 2)) {
    parent->logger.warning("Failed to create SR NR PUCCH resource");
  }

  // Make 2 PUCCH resource sets
  pucch_cfg.setup().res_set_to_add_mod_list_present = true;
  pucch_cfg.setup().res_set_to_add_mod_list.resize(2);

  // Make PUCCH resource set for 1-2 bit
  pucch_cfg.setup().res_set_to_add_mod_list[0].pucch_res_set_id = 0;
  pucch_cfg.setup().res_set_to_add_mod_list[0].res_list.resize(8);
  for (auto& e : pucch_cfg.setup().res_set_to_add_mod_list[0].res_list) {
    e = 0;
  }

  // Make PUCCH resource set for >2 bit
  pucch_cfg.setup().res_set_to_add_mod_list[1].pucch_res_set_id = 1;
  pucch_cfg.setup().res_set_to_add_mod_list[1].res_list.resize(8);
  for (auto& e : pucch_cfg.setup().res_set_to_add_mod_list[1].res_list) {
    e = 1;
  }

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_sp_cell_cfg_ded_ul_cfg_init_ul_bwp_pusch_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  // PUSCH config
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.set_setup();
  auto& pusch_cfg_ded = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup();

  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a_present = true;
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a.set_setup();
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a.setup().dmrs_add_position_present = true;
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a.setup().dmrs_add_position = dmrs_ul_cfg_s::dmrs_add_position_opts::pos1;
  // PUSH power control skipped
  pusch_cfg_ded.res_alloc = pusch_cfg_s::res_alloc_opts::res_alloc_type1;

  // UCI
  pusch_cfg_ded.uci_on_pusch_present = true;
  pusch_cfg_ded.uci_on_pusch.set_setup();
  pusch_cfg_ded.uci_on_pusch.setup().beta_offsets_present = true;
  pusch_cfg_ded.uci_on_pusch.setup().beta_offsets.set_semi_static();
  auto& beta_offset_semi_static                        = pusch_cfg_ded.uci_on_pusch.setup().beta_offsets.semi_static();
  beta_offset_semi_static.beta_offset_ack_idx1_present = true;
  beta_offset_semi_static.beta_offset_ack_idx1         = 9;
  beta_offset_semi_static.beta_offset_ack_idx2_present = true;
  beta_offset_semi_static.beta_offset_ack_idx2         = 9;
  beta_offset_semi_static.beta_offset_ack_idx3_present = true;
  beta_offset_semi_static.beta_offset_ack_idx3         = 9;
  beta_offset_semi_static.beta_offset_csi_part1_idx1_present = true;
  beta_offset_semi_static.beta_offset_csi_part1_idx1         = 6;
  beta_offset_semi_static.beta_offset_csi_part1_idx2_present = true;
  beta_offset_semi_static.beta_offset_csi_part1_idx2         = 6;
  beta_offset_semi_static.beta_offset_csi_part2_idx1_present = true;
  beta_offset_semi_static.beta_offset_csi_part2_idx1         = 6;
  beta_offset_semi_static.beta_offset_csi_part2_idx2_present = true;
  beta_offset_semi_static.beta_offset_csi_part2_idx2         = 6;
  pusch_cfg_ded.uci_on_pusch.setup().scaling                 = uci_on_pusch_s::scaling_opts::f1;

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_sp_cell_cfg_ded_ul_cfg_init_ul_bwp(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp_present = true;

  pack_sp_cell_cfg_ded_ul_cfg_init_ul_bwp_pucch_cfg(cell_group_cfg_pack);
  pack_sp_cell_cfg_ded_ul_cfg_init_ul_bwp_pusch_cfg(cell_group_cfg_pack);

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_sp_cell_cfg_ded_ul_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  // UL config dedicated
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg_present = true;

  pack_sp_cell_cfg_ded_ul_cfg_init_ul_bwp(cell_group_cfg_pack);

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.first_active_ul_bwp_id_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.first_active_ul_bwp_id         = 0;

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_sp_cell_cfg_ded_pdcch_serving_cell_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdcch_serving_cell_cfg_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdcch_serving_cell_cfg.set_setup();

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg.set_setup();
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().nrof_harq_processes_for_pdsch_present =
      true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().nrof_harq_processes_for_pdsch =
      pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::n16;

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_sp_cell_cfg_ded(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  // SP Cell Dedicated config
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded_present                        = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.first_active_dl_bwp_id_present = true;

  if (parent->cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
    cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.first_active_dl_bwp_id = 0;
  } else {
    cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.first_active_dl_bwp_id = 1;
  }

  pack_sp_cell_cfg_ded_ul_cfg(cell_group_cfg_pack);
  pack_sp_cell_cfg_ded_init_dl_bwp(cell_group_cfg_pack);

  // Serving cell config (only to setup)
  pack_sp_cell_cfg_ded_pdcch_serving_cell_cfg(cell_group_cfg_pack);

  // spCellConfig
  if (fill_sp_cell_cfg_from_enb_cfg(parent->cfg, UE_PSCELL_CC_IDX, cell_group_cfg_pack.sp_cell_cfg) != SRSRAN_SUCCESS) {
    parent->logger.error("Failed to pack spCellConfig for rnti=0x%x", rnti);
  }

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_common_phy_cell_group_cfg(
    asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  cell_group_cfg_pack.phys_cell_group_cfg_present = true;
  cell_group_cfg_pack.phys_cell_group_cfg.pdsch_harq_ack_codebook =
      phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::dynamic_value;

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_init_dl_bwp_pdsch_cfg_common(
    asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  // PDSCH config common
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp
      .pdsch_cfg_common_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdsch_cfg_common
      .set_setup();

  auto& pdsch_cfg_common = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp
                               .pdsch_cfg_common.setup();
  pdsch_cfg_common.pdsch_time_domain_alloc_list_present = true;
  pdsch_cfg_common.pdsch_time_domain_alloc_list.resize(1);
  pdsch_cfg_common.pdsch_time_domain_alloc_list[0].map_type = pdsch_time_domain_res_alloc_s::map_type_opts::type_a;
  pdsch_cfg_common.pdsch_time_domain_alloc_list[0].start_symbol_and_len = 40;

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_init_dl_bwp(
    asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp_present = true;
  auto& init_dl_bwp = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp;

  init_dl_bwp.generic_params.location_and_bw    = 14025;
  init_dl_bwp.generic_params.subcarrier_spacing = subcarrier_spacing_opts::khz15;

  pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_init_dl_bwp_pdsch_cfg_common(cell_group_cfg_pack);

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_common(
    asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  // DL config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common_present = true;

  pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_common_phy_cell_group_cfg(cell_group_cfg_pack);
  pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_init_dl_bwp(cell_group_cfg_pack);

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_recfg_with_sync_sp_cell_cfg_common_ul_cfg_common_init_ul_bwp_pusch_cfg_common(
    asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  // PUSCH config common
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp
      .pusch_cfg_common_present = true;
  auto& pusch_cfg_common_pack =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.pusch_cfg_common;
  pusch_cfg_common_pack.set_setup();
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list_present = true;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list.resize(2);
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].k2_present = true;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].k2         = 4;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].map_type =
      asn1::rrc_nr::pusch_time_domain_res_alloc_s::map_type_opts::type_a;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].start_symbol_and_len = 27;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].k2_present           = true;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].k2                   = 3;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].map_type =
      asn1::rrc_nr::pusch_time_domain_res_alloc_s::map_type_opts::type_a;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].start_symbol_and_len = 27;
  pusch_cfg_common_pack.setup().p0_nominal_with_grant_present                        = true;
  pusch_cfg_common_pack.setup().p0_nominal_with_grant                                = -60;

  // PUCCH config common
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp
      .pucch_cfg_common_present = true;
  auto& pucch_cfg_common_pack =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.pucch_cfg_common;
  pucch_cfg_common_pack.set_setup();
  pucch_cfg_common_pack.setup().pucch_group_hop    = asn1::rrc_nr::pucch_cfg_common_s::pucch_group_hop_opts::neither;
  pucch_cfg_common_pack.setup().p0_nominal_present = true;
  pucch_cfg_common_pack.setup().p0_nominal         = -60;

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_recfg_with_sync_sp_cell_cfg_common_ul_cfg_common_init_ul_bwp(
    asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params
      .location_and_bw = 14025;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params
      .subcarrier_spacing = subcarrier_spacing_opts::khz15;

  pack_recfg_with_sync_sp_cell_cfg_common_ul_cfg_common_init_ul_bwp_pusch_cfg_common(cell_group_cfg_pack);

  return SRSRAN_ERROR;
}

int rrc_nr::ue::pack_recfg_with_sync_sp_cell_cfg_common_ul_cfg_common(
    asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  // UL config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.dummy = time_align_timer_opts::ms500;

  pack_recfg_with_sync_sp_cell_cfg_common_ul_cfg_common_init_ul_bwp(cell_group_cfg_pack);

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_recfg_with_sync_sp_cell_cfg_common(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  auto& pscell_cfg = parent->cfg.cell_list.at(UE_PSCELL_CC_IDX);

  if (pscell_cfg.duplex_mode == SRSRAN_DUPLEX_MODE_TDD) {
    cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.smtc.release();
  }

  // DL config
  pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_common(cell_group_cfg_pack);

  // UL config
  pack_recfg_with_sync_sp_cell_cfg_common_ul_cfg_common(cell_group_cfg_pack);

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_recfg_with_sync(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  // Reconfig with Sync
  cell_group_cfg_pack.cell_group_id = 1; // 0 identifies the MCG. Other values identify SCGs.

  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync_present   = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.new_ue_id = rnti;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.t304      = recfg_with_sync_s::t304_opts::ms1000;

  pack_recfg_with_sync_sp_cell_cfg_common(cell_group_cfg_pack);

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_secondary_cell_group_sp_cell_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  cell_group_cfg_pack.sp_cell_cfg_present               = true;
  cell_group_cfg_pack.sp_cell_cfg.serv_cell_idx_present = true;
  cell_group_cfg_pack.sp_cell_cfg.serv_cell_idx = 1; // Serving cell ID of a PSCell. The PCell of the MCG uses ID 0.

  pack_sp_cell_cfg_ded(cell_group_cfg_pack);
  pack_recfg_with_sync(cell_group_cfg_pack);

  return SRSRAN_SUCCESS;
}

// Helper for the RRC Reconfiguration sender to pack hard-coded config
int rrc_nr::ue::pack_secondary_cell_group_cfg(asn1::dyn_octstring& packed_secondary_cell_config)
{
  auto& cell_group_cfg_pack = cell_group_cfg;

  pack_secondary_cell_group_rlc_cfg(cell_group_cfg_pack);
  pack_secondary_cell_group_mac_cfg(cell_group_cfg_pack);
  pack_secondary_cell_group_sp_cell_cfg(cell_group_cfg_pack);

  // make sufficiant space
  packed_secondary_cell_config.resize(256);
  asn1::bit_ref bref_pack(packed_secondary_cell_config.data(), packed_secondary_cell_config.size());
  if (cell_group_cfg_pack.pack(bref_pack) != asn1::SRSASN_SUCCESS) {
    parent->logger.error("Failed to pack NR secondary cell config");
    return SRSRAN_ERROR;
  }
  packed_secondary_cell_config.resize(bref_pack.distance_bytes());

  return SRSRAN_SUCCESS;
}

// Packs a hard-coded RRC Reconfiguration with fixed params for all layers (for now)
int rrc_nr::ue::pack_rrc_reconfiguration(asn1::dyn_octstring& packed_rrc_reconfig)
{
  rrc_recfg_s reconfig;
  reconfig.rrc_transaction_id = ((transaction_id++) % 4u);
  rrc_recfg_ies_s& recfg_ies  = reconfig.crit_exts.set_rrc_recfg();

  // add secondary cell group config
  recfg_ies.secondary_cell_group_present = true;

  if (pack_secondary_cell_group_cfg(recfg_ies.secondary_cell_group) == SRSRAN_ERROR) {
    parent->logger.error("Failed to pack secondary cell group");
    return SRSRAN_ERROR;
  }

  // now pack ..
  packed_rrc_reconfig.resize(512);
  asn1::bit_ref bref_pack(packed_rrc_reconfig.data(), packed_rrc_reconfig.size());
  if (reconfig.pack(bref_pack) != asn1::SRSASN_SUCCESS) {
    parent->logger.error("Failed to pack RRC Reconfiguration");
    return SRSRAN_ERROR;
  }
  packed_rrc_reconfig.resize(bref_pack.distance_bytes());

  return SRSRAN_SUCCESS;
}

// Packs a hard-coded NR radio bearer config with fixed params for RLC/PDCP (for now)
int rrc_nr::ue::pack_nr_radio_bearer_config(asn1::dyn_octstring& packed_nr_bearer_config)
{
  // set security config
  auto& radio_bearer_cfg_pack                        = radio_bearer_cfg;
  radio_bearer_cfg_pack.security_cfg_present         = true;
  auto& sec_cfg                                      = radio_bearer_cfg_pack.security_cfg;
  sec_cfg.key_to_use_present                         = true;
  sec_cfg.key_to_use                                 = asn1::rrc_nr::security_cfg_s::key_to_use_opts::secondary;
  sec_cfg.security_algorithm_cfg_present             = true;
  sec_cfg.security_algorithm_cfg.ciphering_algorithm = ciphering_algorithm_opts::nea0;
  sec_cfg.security_algorithm_cfg.integrity_prot_algorithm_present = true;
  sec_cfg.security_algorithm_cfg.integrity_prot_algorithm         = integrity_prot_algorithm_opts::nia0;

  // pack it
  packed_nr_bearer_config.resize(128);
  asn1::bit_ref bref_pack(packed_nr_bearer_config.data(), packed_nr_bearer_config.size());
  if (radio_bearer_cfg_pack.pack(bref_pack) != asn1::SRSASN_SUCCESS) {
    parent->logger.error("Failed to pack NR radio bearer config");
    return SRSRAN_ERROR;
  }

  // resize to packed length
  packed_nr_bearer_config.resize(bref_pack.distance_bytes());

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::handle_sgnb_addition_request(uint16_t eutra_rnti_, const sgnb_addition_req_params_t& req_params)
{
  // Add DRB1 to RLC and PDCP
  if (add_drb() != SRSRAN_SUCCESS) {
    parent->logger.error("Failed to configure DRB");
    parent->rrc_eutra->sgnb_addition_reject(eutra_rnti_);
    return SRSRAN_ERROR;
  }

  // provide hard-coded NR configs
  rrc_eutra_interface_rrc_nr::sgnb_addition_ack_params_t ack_params = {};
  if (pack_rrc_reconfiguration(ack_params.nr_secondary_cell_group_cfg_r15) == SRSRAN_ERROR) {
    parent->logger.error("Failed to pack RRC Reconfiguration. Sending SgNB addition reject.");
    parent->rrc_eutra->sgnb_addition_reject(eutra_rnti_);
    return SRSRAN_ERROR;
  }

  if (pack_nr_radio_bearer_config(ack_params.nr_radio_bearer_cfg1_r15) == SRSRAN_ERROR) {
    parent->logger.error("Failed to pack NR radio bearer config. Sending SgNB addition reject.");
    parent->rrc_eutra->sgnb_addition_reject(eutra_rnti_);
    return SRSRAN_ERROR;
  }

  // send response to EUTRA
  ack_params.nr_rnti       = rnti;
  ack_params.eps_bearer_id = req_params.eps_bearer_id;
  parent->rrc_eutra->sgnb_addition_ack(eutra_rnti_, ack_params);

  // recognize RNTI as ENDC user
  endc       = true;
  eutra_rnti = eutra_rnti_;

  return SRSRAN_SUCCESS;
}

void rrc_nr::ue::crnti_ce_received()
{
  // Assume NSA mode active
  if (endc) {
    // send SgNB addition complete for ENDC users
    parent->rrc_eutra->sgnb_addition_complete(eutra_rnti, rnti);

    // stop RX MSG3/MSG5 activity timer on MAC CE RNTI reception
    set_activity_timeout(UE_INACTIVITY_TIMEOUT);
    parent->logger.debug("Received MAC CE-RNTI for 0x%x - stopping MSG3/MSG5 timer, starting inactivity timer", rnti);

    // Add DRB1 to MAC
    for (auto& drb : cell_group_cfg.rlc_bearer_to_add_mod_list) {
      uecfg.ue_bearers[drb.lc_ch_id].direction = mac_lc_ch_cfg_t::BOTH;
      uecfg.ue_bearers[drb.lc_ch_id].group     = drb.mac_lc_ch_cfg.ul_specific_params.lc_ch_group;
    }

    // Update UE phy params
    srsran::make_pdsch_cfg_from_serv_cell(cell_group_cfg.sp_cell_cfg.sp_cell_cfg_ded, &uecfg.phy_cfg.pdsch);
    srsran::make_csi_cfg_from_serv_cell(cell_group_cfg.sp_cell_cfg.sp_cell_cfg_ded, &uecfg.phy_cfg.csi);
    srsran::make_phy_ssb_cfg(parent->cfg.cell_list[0].phy_cell.carrier,
                             cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common,
                             &uecfg.phy_cfg.ssb);
    srsran::make_duplex_cfg_from_serv_cell(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common,
                                           &uecfg.phy_cfg.duplex);

    parent->mac->ue_cfg(rnti, uecfg);
  }
}

/**
 * @brief Set DRB configuration
 *
 * The function sets and configures all relavant fields for the DRB configuration (MAC, RLC, PDCP) in the
 * cellGroupConfig and also adds the bearer to the local RLC and PDCP entities.
 *
 * @return int SRSRAN_SUCCESS on success
 */
int rrc_nr::ue::add_drb()
{
  // RLC for DRB1 (with fixed LCID) inside cell_group_cfg
  auto& cell_group_cfg_pack = cell_group_cfg;

  cell_group_cfg_pack.rlc_bearer_to_add_mod_list_present = true;
  cell_group_cfg_pack.rlc_bearer_to_add_mod_list.resize(1);
  auto& rlc_bearer                       = cell_group_cfg_pack.rlc_bearer_to_add_mod_list[0];
  rlc_bearer.lc_ch_id                    = drb1_lcid;
  rlc_bearer.served_radio_bearer_present = true;
  rlc_bearer.served_radio_bearer.set_drb_id();
  rlc_bearer.served_radio_bearer.drb_id() = 1;
  rlc_bearer.rlc_cfg_present              = true;
  rlc_bearer.rlc_cfg.set_um_bi_dir();
  rlc_bearer.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len_present = true;
  rlc_bearer.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len         = sn_field_len_um_opts::size12;
  rlc_bearer.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len_present = true;
  rlc_bearer.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len         = sn_field_len_um_opts::size12;
  rlc_bearer.rlc_cfg.um_bi_dir().dl_um_rlc.t_reassembly         = t_reassembly_opts::ms50;

  // add RLC bearer
  srsran::rlc_config_t rlc_cfg;
  if (srsran::make_rlc_config_t(cell_group_cfg.rlc_bearer_to_add_mod_list[0].rlc_cfg, &rlc_cfg) != SRSRAN_SUCCESS) {
    parent->logger.error("Failed to build RLC config");
    return SRSRAN_ERROR;
  }
  parent->rlc->add_bearer(rnti, drb1_lcid, rlc_cfg);

  // MAC logical channel config
  rlc_bearer.mac_lc_ch_cfg_present                    = true;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params_present = true;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params.prio    = 11;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params.prioritised_bit_rate =
      asn1::rrc_nr::lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::kbps0;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params.bucket_size_dur =
      asn1::rrc_nr::lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::ms100;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params.lc_ch_group_present      = true;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params.lc_ch_group              = 3;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params.sched_request_id_present = true;
  rlc_bearer.mac_lc_ch_cfg.ul_specific_params.sched_request_id         = 0;
  // TODO: add LC config to MAC

  // PDCP config goes into radio_bearer_cfg
  auto& radio_bearer_cfg_pack                       = radio_bearer_cfg;
  radio_bearer_cfg_pack.drb_to_add_mod_list_present = true;
  radio_bearer_cfg_pack.drb_to_add_mod_list.resize(1);

  // configure fixed DRB1
  auto& drb_item                                = radio_bearer_cfg_pack.drb_to_add_mod_list[0];
  drb_item.drb_id                               = 1;
  drb_item.cn_assoc_present                     = true;
  drb_item.cn_assoc.set_eps_bearer_id()         = 5;
  drb_item.pdcp_cfg_present                     = true;
  drb_item.pdcp_cfg.ciphering_disabled_present  = true;
  drb_item.pdcp_cfg.drb_present                 = true;
  drb_item.pdcp_cfg.drb.pdcp_sn_size_dl_present = true;
  drb_item.pdcp_cfg.drb.pdcp_sn_size_dl         = asn1::rrc_nr::pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::len18bits;
  drb_item.pdcp_cfg.drb.pdcp_sn_size_ul_present = true;
  drb_item.pdcp_cfg.drb.pdcp_sn_size_ul         = asn1::rrc_nr::pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_opts::len18bits;
  drb_item.pdcp_cfg.drb.discard_timer_present   = true;
  drb_item.pdcp_cfg.drb.discard_timer           = asn1::rrc_nr::pdcp_cfg_s::drb_s_::discard_timer_opts::ms100;
  drb_item.pdcp_cfg.drb.hdr_compress.set_not_used();
  drb_item.pdcp_cfg.t_reordering_present = true;
  drb_item.pdcp_cfg.t_reordering         = asn1::rrc_nr::pdcp_cfg_s::t_reordering_opts::ms0;

  // Add DRB1 to PDCP
  srsran::pdcp_config_t pdcp_cnfg = srsran::make_drb_pdcp_config_t(drb_item.drb_id, false, drb_item.pdcp_cfg);
  parent->pdcp->add_bearer(rnti, rlc_bearer.lc_ch_id, pdcp_cnfg);

  // Note: DRB1 is only activated in the MAC when the C-RNTI CE is received

  return SRSRAN_SUCCESS;
}

/**
 * @brief Deactivate all Bearers (MAC logical channel) for this specific RNTI
 *
 * The function iterates over the bearers or MAC logical channels and deactivates them by setting each one to IDLE
 */
void rrc_nr::ue::deactivate_bearers()
{
  // Iterate over the bearers (MAC LC CH) and set each of them to IDLE
  for (auto& ue_bearer : uecfg.ue_bearers) {
    ue_bearer.direction = mac_lc_ch_cfg_t::IDLE;
  }

  // No need to check the returned value, as the function ue_cfg will return SRSRAN_SUCCESS (it asserts if it fails)
  parent->mac->ue_cfg(rnti, uecfg);
}

} // namespace srsenb
