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

#include "srsgnb/hdr/stack/rrc/rrc_nr.h"
#include "srsenb/hdr/common/common_enb.h"
#include "srsgnb/hdr/stack/rrc/cell_asn1_config.h"
#include "srsgnb/hdr/stack/rrc/rrc_nr_config_utils.h"
#include "srsgnb/hdr/stack/rrc/rrc_nr_du_manager.h"
#include "srsgnb/hdr/stack/rrc/rrc_nr_ue.h"
#include "srsgnb/src/stack/mac/test/sched_nr_cfg_generators.h"
#include "srsran/asn1/rrc_nr_utils.h"
#include "srsran/common/bearer_manager.h"
#include "srsran/common/common_nr.h"
#include "srsran/common/phy_cfg_nr_default.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"

using namespace asn1::rrc_nr;

namespace srsenb {

rrc_nr::rrc_nr(srsran::task_sched_handle task_sched_) :
  logger(srslog::fetch_basic_logger("RRC-NR")), task_sched(task_sched_)
{}

rrc_nr::~rrc_nr() {}

int rrc_nr::init(const rrc_nr_cfg_t&         cfg_,
                 phy_interface_stack_nr*     phy_,
                 mac_interface_rrc_nr*       mac_,
                 rlc_interface_rrc*          rlc_,
                 pdcp_interface_rrc*         pdcp_,
                 ngap_interface_rrc_nr*      ngap_,
                 gtpu_interface_rrc*         gtpu_,
                 enb_bearer_manager&         bearer_mapper_,
                 rrc_eutra_interface_rrc_nr* rrc_eutra_)
{
  phy           = phy_;
  mac           = mac_;
  rlc           = rlc_;
  pdcp          = pdcp_;
  ngap          = ngap_;
  gtpu          = gtpu_;
  bearer_mapper = &bearer_mapper_;
  rrc_eutra     = rrc_eutra_;

  cfg = cfg_;

  // log cell configs
  for (uint32_t i = 0; i < cfg.cell_list.size(); ++i) {
    const auto& cell = cfg.cell_list.at(i);
    logger.info("Cell idx=%d, pci=%d, nr_dl_arfcn=%d, nr_ul_arfcn=%d, band=%d, duplex=%s, n_rb_dl=%d, ssb_arfcn=%d",
                i,
                cell.phy_cell.carrier.pci,
                cell.dl_arfcn,
                cell.ul_arfcn,
                cell.band,
                cell.duplex_mode == SRSRAN_DUPLEX_MODE_FDD ? "FDD" : "TDD",
                cell.phy_cell.carrier.nof_prb,
                cell.ssb_absolute_freq_point);
  }

  du_cfg = std::make_unique<du_config_manager>(cfg);
  for (uint32_t i = 0; i < cfg.cell_list.size(); ++i) {
    int ret = du_cfg->add_cell();
    srsran_assert(ret == SRSRAN_SUCCESS, "Failed to configure NR cell %d", i);
  }

  // Generate cell config structs
  cell_ctxt                                           = std::make_unique<cell_ctxt_t>();
  std::unique_ptr<cell_group_cfg_s> master_cell_group = std::make_unique<cell_group_cfg_s>();
  int                               ret               = fill_master_cell_cfg_from_enb_cfg(cfg, 0, *master_cell_group);
  srsran_assert(ret == SRSRAN_SUCCESS, "Failed to configure MasterCellGroup");
  cell_ctxt->master_cell_group = std::move(master_cell_group);

  // derived
  slot_dur_ms = 1;

  if (generate_sibs() != SRSRAN_SUCCESS) {
    logger.error("Couldn't generate SIB messages.");
    return SRSRAN_ERROR;
  }

  config_phy(); // if PHY is not yet initialized, config will be stored and applied on initialization
  config_mac();

  logger.info("Number of 5QI %d", cfg.five_qi_cfg.size());
  for (const std::pair<const uint32_t, rrc_nr_cfg_five_qi_t>& five_qi_cfg : cfg.five_qi_cfg) {
    logger.info("5QI configuration. 5QI=%d", five_qi_cfg.first);
    if (logger.info.enabled()) {
      asn1::json_writer js{};
      five_qi_cfg.second.pdcp_cfg.to_json(js);
      logger.info("PDCP NR configuration: %s", js.to_string().c_str());
      js = {};
      five_qi_cfg.second.rlc_cfg.to_json(js);
      logger.info("RLC NR configuration: %s", js.to_string().c_str());
    }
  }
  logger.info("NIA preference list: NIA%d, NIA%d, NIA%d",
              cfg.nia_preference_list[0],
              cfg.nia_preference_list[1],
              cfg.nia_preference_list[2]);
  logger.info("NEA preference list: NEA%d, NEA%d, NEA%d",
              cfg.nea_preference_list[0],
              cfg.nea_preference_list[1],
              cfg.nea_preference_list[2]);
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
void rrc_nr::log_rrc_message(const char*             source,
                             const direction_t       dir,
                             srsran::const_byte_span pdu,
                             const T&                msg,
                             const char*             msg_type)
{
  if (logger.debug.enabled()) {
    asn1::json_writer json_writer;
    msg.to_json(json_writer);
    logger.debug(pdu.data(), pdu.size(), "%s - %s %s (%d B)", source, (dir == Rx) ? "Rx" : "Tx", msg_type, pdu.size());
    logger.debug("Content:%s", json_writer.to_string().c_str());
  } else if (logger.info.enabled()) {
    logger.info(pdu.data(), pdu.size(), "%s - %s %s (%d B)", source, (dir == Rx) ? "Rx" : "Tx", msg_type, pdu.size());
  }
}
template void rrc_nr::log_rrc_message<dl_ccch_msg_s>(const char*             source,
                                                     const direction_t       dir,
                                                     srsran::const_byte_span pdu,
                                                     const dl_ccch_msg_s&    msg,
                                                     const char*             msg_type);
template void rrc_nr::log_rrc_message<dl_dcch_msg_s>(const char*             source,
                                                     const direction_t       dir,
                                                     srsran::const_byte_span pdu,
                                                     const dl_dcch_msg_s&    msg,
                                                     const char*             msg_type);
template void rrc_nr::log_rrc_message<cell_group_cfg_s>(const char*             source,
                                                        const direction_t       dir,
                                                        srsran::const_byte_span pdu,
                                                        const cell_group_cfg_s& msg,
                                                        const char*             msg_type);
template void rrc_nr::log_rrc_message<radio_bearer_cfg_s>(const char*               source,
                                                          const direction_t         dir,
                                                          srsran::const_byte_span   pdu,
                                                          const radio_bearer_cfg_s& msg,
                                                          const char*               msg_type);

void rrc_nr::log_rx_pdu_fail(uint16_t                rnti,
                             uint32_t                lcid,
                             srsran::const_byte_span pdu,
                             const char*             cause_str,
                             bool                    log_hex)
{
  if (log_hex) {
    logger.error(
        pdu.data(), pdu.size(), "Rx %s PDU, rnti=0x%x - Discarding. Cause: %s", get_rb_name(lcid), rnti, cause_str);
  } else {
    logger.error("Rx %s PDU, rnti=0x%x - Discarding. Cause: %s", get_rb_name(lcid), rnti, cause_str);
  }
}

/* @brief PRIVATE function, gets called by sgnb_addition_request
 *
 * This function WILL NOT TRIGGER the RX MSG3 activity timer
 */
int rrc_nr::add_user(uint16_t rnti, uint32_t pcell_cc_idx, bool start_msg3_timer)
{
  if (users.contains(rnti) == 0) {
    // If in the ue ctor, "start_msg3_timer" is set to true, this will start the MSG3 RX TIMEOUT at ue creation
    users.insert(rnti, std::make_unique<ue>(this, rnti, pcell_cc_idx, start_msg3_timer));
    rlc->add_user(rnti);
    pdcp->add_user(rnti);
    logger.info("Added new user rnti=0x%x", rnti);
    return SRSRAN_SUCCESS;
  }
  logger.error("Adding user rnti=0x%x (already exists)", rnti);
  return SRSRAN_ERROR;
}

/* @brief PUBLIC function, gets called by mac_nr::rach_detected
 *
 * This function is called from PRACH worker (can wait) and WILL TRIGGER the RX MSG3 activity timer
 */
int rrc_nr::add_user(uint16_t rnti, uint32_t pcell_cc_idx)
{
  // Set "triggered_by_rach" to true to start the MSG3 RX TIMEOUT
  return add_user(rnti, pcell_cc_idx, true);
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

/// This function is called when the INACTIVITY TIMER FOR
int rrc_nr::rrc_release(uint16_t rnti)
{
  // TODO: we do not have yet a defined procedure to handle this
  return SRSRAN_SUCCESS;
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

  // Restart inactivity timer for RRC-NR
  ue_ptr->set_activity();

  // inform EUTRA RRC about user activity
  if (ue_ptr->is_endc()) {
    // inform EUTRA RRC about user activity
    rrc_eutra->set_activity_user(ue_ptr->get_eutra_rnti());
  }
}

void rrc_nr::config_phy()
{
  srsenb::phy_interface_rrc_nr::common_cfg_t common_cfg = {};
  common_cfg.carrier                                    = cfg.cell_list[0].phy_cell.carrier;
  fill_phy_pdcch_cfg_common(du_cfg->cell(0), &common_cfg.pdcch);
  bool ret = srsran::fill_phy_pdcch_cfg(
      cell_ctxt->master_cell_group->sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.setup(), &common_cfg.pdcch);
  srsran_assert(ret, "Failed to generate Dedicated PDCCH config");
  srsran::make_phy_rach_cfg(du_cfg->cell(0).serv_cell_cfg_common().ul_cfg_common.init_ul_bwp.rach_cfg_common.setup(),
                            cfg.cell_list[0].duplex_mode,
                            &common_cfg.prach);
  common_cfg.duplex_mode = cfg.cell_list[0].duplex_mode;
  ret                    = srsran::fill_phy_ssb_cfg(
      cfg.cell_list[0].phy_cell.carrier, du_cfg->cell(0).serv_cell_cfg_common(), &common_cfg.ssb);
  srsran_assert(ret, "Failed to generate PHY config");
  if (phy->set_common_cfg(common_cfg) < SRSRAN_SUCCESS) {
    logger.error("Couldn't set common PHY config");
    return;
  }
}

void rrc_nr::config_mac()
{
  uint32_t cc = 0;
  // Fill MAC scheduler configuration for SIBs
  // TODO: use parsed cell NR cfg configuration
  srsran::phy_cfg_nr_default_t::reference_cfg_t ref_args{};
  ref_args.duplex = cfg.cell_list[cc].duplex_mode == SRSRAN_DUPLEX_MODE_TDD
                        ? srsran::phy_cfg_nr_default_t::reference_cfg_t::R_DUPLEX_TDD_CUSTOM_6_4
                        : srsran::phy_cfg_nr_default_t::reference_cfg_t::R_DUPLEX_FDD;
  std::vector<sched_nr_cell_cfg_t> sched_cells_cfg(1, get_default_cell_cfg(srsran::phy_cfg_nr_default_t{ref_args}));
  sched_nr_cell_cfg_t&             cell = sched_cells_cfg[cc];

  // Derive cell config from rrc_nr_cfg_t
  fill_phy_pdcch_cfg_common(du_cfg->cell(cc), &cell.bwps[0].pdcch);
  bool ret = srsran::fill_phy_pdcch_cfg(
      cell_ctxt->master_cell_group->sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.setup(), &cell.bwps[0].pdcch);
  srsran_assert(ret, "Failed to generate Dedicated PDCCH config");
  cell.pci                    = cfg.cell_list[cc].phy_cell.carrier.pci;
  cell.nof_layers             = cfg.cell_list[cc].phy_cell.carrier.max_mimo_layers;
  cell.dl_cell_nof_prb        = cfg.cell_list[cc].phy_cell.carrier.nof_prb;
  cell.ul_cell_nof_prb        = cfg.cell_list[cc].phy_cell.carrier.nof_prb;
  cell.dl_center_frequency_hz = cfg.cell_list[cc].phy_cell.carrier.dl_center_frequency_hz;
  cell.ul_center_frequency_hz = cfg.cell_list[cc].phy_cell.carrier.ul_center_frequency_hz;
  cell.ssb_center_freq_hz     = cfg.cell_list[cc].phy_cell.carrier.ssb_center_freq_hz;
  cell.dmrs_type_a_position   = du_cfg->cell(cc).mib.dmrs_type_a_position;
  cell.pdcch_cfg_sib1         = du_cfg->cell(cc).mib.pdcch_cfg_sib1;
  if (du_cfg->cell(cc).serv_cell_cfg_common().tdd_ul_dl_cfg_common_present) {
    cell.tdd_ul_dl_cfg_common.emplace(du_cfg->cell(cc).serv_cell_cfg_common().tdd_ul_dl_cfg_common);
  }
  cell.dl_cfg_common       = du_cfg->cell(cc).serv_cell_cfg_common().dl_cfg_common;
  cell.ul_cfg_common       = du_cfg->cell(cc).serv_cell_cfg_common().ul_cfg_common;
  cell.ss_pbch_block_power = du_cfg->cell(cc).serv_cell_cfg_common().ss_pbch_block_pwr;
  bool valid_cfg = srsran::make_pdsch_cfg_from_serv_cell(cell_ctxt->master_cell_group->sp_cell_cfg.sp_cell_cfg_ded,
                                                         &cell.bwps[0].pdsch);
  srsran_assert(valid_cfg, "Invalid NR cell configuration.");
  cell.ssb_positions_in_burst = du_cfg->cell(cc).serv_cell_cfg_common().ssb_positions_in_burst;
  cell.ssb_periodicity_ms     = du_cfg->cell(cc).serv_cell_cfg_common().ssb_periodicity_serving_cell.to_number();
  cell.ssb_scs.value          = (subcarrier_spacing_e::options)cfg.cell_list[0].phy_cell.carrier.scs;
  cell.ssb_offset             = du_cfg->cell(cc).mib.ssb_subcarrier_offset;
  if (not cfg.is_standalone) {
    const serving_cell_cfg_common_s& serv_cell =
        cell_ctxt->master_cell_group->sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common;
    // Derive cell config from ASN1
    cell.ssb_scs = serv_cell.ssb_subcarrier_spacing;
  }

  // Set SIB1 and SI messages
  cell.sibs.resize(cell_ctxt->sib_buffer.size());
  for (uint32_t i = 0; i < cell_ctxt->sib_buffer.size(); i++) {
    cell.sibs[i].len = cell_ctxt->sib_buffer[i]->N_bytes;
    if (i == 0) {
      cell.sibs[i].period_rf       = 16; // SIB1 is always 16 rf
      cell.sibs[i].si_window_slots = 160;
    } else {
      cell.sibs[i].period_rf = du_cfg->cell(0).sib1.si_sched_info.sched_info_list[i - 1].si_periodicity.to_number();
      cell.sibs[i].si_window_slots = du_cfg->cell(0).sib1.si_sched_info.si_win_len.to_number();
    }
  }

  // Configure MAC/scheduler
  mac->cell_cfg(sched_cells_cfg);

  // Make default UE PHY config object
  cell_ctxt->default_phy_ue_cfg_nr = get_common_ue_phy_cfg(cell);
}

int32_t rrc_nr::generate_sibs()
{
  if (not cfg.is_standalone) {
    return SRSRAN_SUCCESS;
  }

  // SIB1 packing
  const si_sched_info_s::sched_info_list_l_& sched_info = du_cfg->cell(0).sib1.si_sched_info.sched_info_list;

  // SI messages packing
  cell_ctxt->sibs.resize(1);
  sib2_s& sib2                             = cell_ctxt->sibs[0].set_sib2();
  sib2.cell_resel_info_common.q_hyst.value = sib2_s::cell_resel_info_common_s_::q_hyst_opts::db5;

  // msg is array of SI messages, each SI message msg[i] may contain multiple SIBs
  // all SIBs in a SI message msg[i] share the same periodicity
  const uint32_t nof_messages =
      du_cfg->cell(0).sib1.si_sched_info_present ? du_cfg->cell(0).sib1.si_sched_info.sched_info_list.size() : 0;
  cell_ctxt->sib_buffer.reserve(nof_messages + 1);
  asn1::dyn_array<bcch_dl_sch_msg_s> msg(nof_messages + 1);

  // Copy SIB1 to first SI message
  msg[0].msg.set_c1().set_sib_type1() = du_cfg->cell(0).sib1;

  // Copy rest of SIBs
  for (uint32_t sched_info_elem = 0; sched_info_elem < nof_messages; sched_info_elem++) {
    uint32_t msg_index = sched_info_elem + 1; // first msg is SIB1, therefore start with second

    msg[msg_index].msg.set_c1().set_sys_info().crit_exts.set_sys_info();
    auto& sib_list = msg[msg_index].msg.c1().sys_info().crit_exts.sys_info().sib_type_and_info;

    for (uint32_t mapping = 0; mapping < sched_info[sched_info_elem].sib_map_info.size(); ++mapping) {
      uint32_t sibidx = sched_info[sched_info_elem].sib_map_info[mapping].type; // SIB2 == 0
      sib_list.push_back(cell_ctxt->sibs[sibidx]);
    }
  }

  // Pack payload for all messages
  for (uint32_t msg_index = 0; msg_index < nof_messages + 1; msg_index++) {
    srsran::unique_byte_buffer_t sib_pdu = pack_into_pdu(msg[msg_index]);
    if (sib_pdu == nullptr) {
      logger.error("Failed to pack SIB");
      return SRSRAN_ERROR;
    }
    cell_ctxt->sib_buffer.push_back(std::move(sib_pdu));

    // Log SIBs in JSON format
    fmt::memory_buffer strbuf;
    if (msg_index == 0) {
      fmt::format_to(strbuf, "SIB1 payload");
    } else {
      fmt::format_to(strbuf, "SI message={} payload", msg_index + 1);
    }
    log_rrc_message("BCCH", Tx, *cell_ctxt->sib_buffer.back(), msg[msg_index], srsran::to_c_str(strbuf));
  }

  return SRSRAN_SUCCESS;
}

/*******************************************************************************
  MAC interface
*******************************************************************************/

int rrc_nr::read_pdu_bcch_bch(const uint32_t tti, srsran::byte_buffer_t& buffer)
{
  if (du_cfg->cell(0).packed_mib == nullptr || buffer.get_tailroom() < du_cfg->cell(0).packed_mib->N_bytes) {
    return SRSRAN_ERROR;
  }
  buffer = *du_cfg->cell(0).packed_mib;
  return SRSRAN_SUCCESS;
}

int rrc_nr::read_pdu_bcch_dlsch(uint32_t sib_index, srsran::byte_buffer_t& buffer)
{
  if (sib_index >= cell_ctxt->sib_buffer.size()) {
    logger.error("SI%s%d is not a configured SIB.", sib_index == 0 ? "B" : "", sib_index + 1);
    return SRSRAN_ERROR;
  }

  buffer = *cell_ctxt->sib_buffer[sib_index];

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

void rrc_nr::handle_pdu(uint16_t rnti, uint32_t lcid, srsran::const_byte_span pdu)
{
  switch (static_cast<srsran::nr_srb>(lcid)) {
    case srsran::nr_srb::srb0:
      handle_ul_ccch(rnti, pdu);
      break;
    case srsran::nr_srb::srb1:
    case srsran::nr_srb::srb2:
    case srsran::nr_srb::srb3:
      handle_ul_dcch(rnti, lcid, std::move(pdu));
      break;
    default:
      std::string errcause = fmt::format("Invalid LCID=%d", lcid);
      log_rx_pdu_fail(rnti, lcid, pdu, errcause.c_str());
      break;
  }
}

void rrc_nr::handle_ul_ccch(uint16_t rnti, srsran::const_byte_span pdu)
{
  // Parse UL-CCCH
  ul_ccch_msg_s ul_ccch_msg;
  {
    asn1::cbit_ref bref(pdu.data(), pdu.size());
    if (ul_ccch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
        ul_ccch_msg.msg.type().value != ul_ccch_msg_type_c::types_opts::c1) {
      log_rx_pdu_fail(rnti, srb_to_lcid(lte_srb::srb0), pdu, "Failed to unpack UL-CCCH message", true);
      return;
    }
  }

  // Log Rx message
  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "rnti=0x{:x}, SRB0", rnti);
  fmt::format_to(fmtbuf2, "UL-CCCH.{}", ul_ccch_msg.msg.c1().type().to_string());
  log_rrc_message(srsran::to_c_str(fmtbuf), Rx, pdu, ul_ccch_msg, srsran::to_c_str(fmtbuf2));

  // Handle message
  switch (ul_ccch_msg.msg.c1().type().value) {
    case ul_ccch_msg_type_c::c1_c_::types_opts::rrc_setup_request:
      handle_rrc_setup_request(rnti, ul_ccch_msg.msg.c1().rrc_setup_request());
      break;
    case ul_ccch_msg_type_c::c1_c_::types_opts::rrc_reest_request:
      handle_rrc_reest_request(rnti, ul_ccch_msg.msg.c1().rrc_reest_request());
      break;
    default:
      log_rx_pdu_fail(rnti, srb_to_lcid(lte_srb::srb0), pdu, "Unsupported UL-CCCH message type");
      // TODO Remove user
  }
}

void rrc_nr::handle_ul_dcch(uint16_t rnti, uint32_t lcid, srsran::const_byte_span pdu)
{
  // Parse UL-DCCH
  ul_dcch_msg_s ul_dcch_msg;
  {
    asn1::cbit_ref bref(pdu.data(), pdu.size());
    if (ul_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
        ul_dcch_msg.msg.type().value != ul_dcch_msg_type_c::types_opts::c1) {
      log_rx_pdu_fail(rnti, lcid, pdu, "Failed to unpack UL-DCCH message");
      return;
    }
  }

  // Verify UE exists
  auto ue_it = users.find(rnti);
  if (ue_it == users.end()) {
    log_rx_pdu_fail(rnti, lcid, pdu, "Inexistent rnti");
  }
  ue& u = *ue_it->second;

  // Log Rx message
  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "rnti=0x{:x}, {}", rnti, srsran::get_srb_name(srsran::nr_lcid_to_srb(lcid)));
  fmt::format_to(fmtbuf2, "UL-DCCH.{}", ul_dcch_msg.msg.c1().type().to_string());
  log_rrc_message(srsran::to_c_str(fmtbuf), Rx, pdu, ul_dcch_msg, srsran::to_c_str(fmtbuf2));

  // Handle message
  switch (ul_dcch_msg.msg.c1().type().value) {
    case ul_dcch_msg_type_c::c1_c_::types_opts::rrc_setup_complete:
      u.handle_rrc_setup_complete(ul_dcch_msg.msg.c1().rrc_setup_complete());
      break;
    case ul_dcch_msg_type_c::c1_c_::types_opts::security_mode_complete:
      u.handle_security_mode_complete(ul_dcch_msg.msg.c1().security_mode_complete());
      break;
    case ul_dcch_msg_type_c::c1_c_::types_opts::rrc_recfg_complete:
      u.handle_rrc_reconfiguration_complete(ul_dcch_msg.msg.c1().rrc_recfg_complete());
      break;
    case ul_dcch_msg_type_c::c1_c_::types_opts::ul_info_transfer:
      u.handle_ul_information_transfer(ul_dcch_msg.msg.c1().ul_info_transfer());
      break;
    case ul_dcch_msg_type_c::c1_c_::types_opts::rrc_reest_complete:
      u.handle_rrc_reestablishment_complete(ul_dcch_msg.msg.c1().rrc_reest_complete());
      break;
    case ul_dcch_msg_type_c::c1_c_::types_opts::ue_cap_info:
      u.handle_ue_capability_information(ul_dcch_msg.msg.c1().ue_cap_info());
      break;
    default:
      log_rx_pdu_fail(rnti, srb_to_lcid(lte_srb::srb0), pdu, "Unsupported UL-CCCH message type", false);
      // TODO Remove user
  }
}

void rrc_nr::handle_rrc_setup_request(uint16_t rnti, const asn1::rrc_nr::rrc_setup_request_s& msg)
{
  auto ue_it = users.find(rnti);

  // TODO: Defer creation of ue to this point
  if (ue_it == users.end()) {
    logger.error("%s received for inexistent rnti=0x%x", "UL-CCCH", rnti);
    return;
  }
  ue& u = *ue_it->second;
  u.handle_rrc_setup_request(msg);
}

void rrc_nr::handle_rrc_reest_request(uint16_t rnti, const asn1::rrc_nr::rrc_reest_request_s& msg)
{
  auto ue_it = users.find(rnti);

  // TODO: Defer creation of ue to this point
  if (ue_it == users.end()) {
    logger.error("%s received for inexistent rnti=0x%x", "UL-CCCH", rnti);
    return;
  }
  ue& u = *ue_it->second;
  u.handle_rrc_reestablishment_request(msg);
}

/*******************************************************************************
  PDCP interface
*******************************************************************************/
void rrc_nr::write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  if (pdu == nullptr or pdu->N_bytes == 0) {
    logger.error("Rx %s PDU, rnti=0x%x - Discarding. Cause: PDU is empty", srsenb::get_rb_name(lcid), rnti);
    return;
  }
  handle_pdu(rnti, lcid, *pdu);
}

void rrc_nr::notify_pdcp_integrity_error(uint16_t rnti, uint32_t lcid) {}

/*******************************************************************************
  NGAP interface
*******************************************************************************/

int rrc_nr::ue_set_security_cfg_key(uint16_t rnti, const asn1::fixed_bitstring<256, false, true>& key)
{
  logger.debug("Setting securtiy key for rnti=0x%x", rnti);
  auto ue_it = users.find(rnti);

  if (ue_it == users.end()) {
    logger.error("Trying to set key for non-existing rnti=0x%x", rnti);
    return SRSRAN_ERROR;
  }
  ue& u = *ue_it->second;
  u.set_security_key(key);
  return SRSRAN_SUCCESS;
}
int rrc_nr::ue_set_bitrates(uint16_t rnti, const asn1::ngap::ue_aggregate_maximum_bit_rate_s& rates)
{
  return SRSRAN_SUCCESS;
}
int rrc_nr::set_aggregate_max_bitrate(uint16_t rnti, const asn1::ngap::ue_aggregate_maximum_bit_rate_s& rates)
{
  return SRSRAN_SUCCESS;
}
int rrc_nr::ue_set_security_cfg_capabilities(uint16_t rnti, const asn1::ngap::ue_security_cap_s& caps)
{
  logger.debug("Setting securtiy capabilites for rnti=0x%x", rnti);
  auto ue_it = users.find(rnti);

  if (ue_it == users.end()) {
    logger.error("Trying to set security capabilities for non-existing rnti=0x%x", rnti);
    return SRSRAN_ERROR;
  }
  ue& u = *ue_it->second;
  u.set_security_capabilities(caps);
  return SRSRAN_SUCCESS;
}
int rrc_nr::start_security_mode_procedure(uint16_t rnti, srsran::unique_byte_buffer_t nas_pdu)
{
  auto user_it = users.find(rnti);
  if (user_it == users.end()) {
    logger.error("Starting SecurityModeCommand procedure failed - rnti=0x%x not found", rnti);
    return SRSRAN_ERROR;
  }
  user_it->second->send_security_mode_command(std::move(nas_pdu));
  return SRSRAN_SUCCESS;
}
int rrc_nr::establish_rrc_bearer(uint16_t                rnti,
                                 uint16_t                pdu_session_id,
                                 srsran::const_byte_span nas_pdu,
                                 uint32_t                lcid,
                                 uint32_t                five_qi)
{
  if (not users.contains(rnti)) {
    logger.error("Establishing RRC bearers for inexistent rnti=0x%x", rnti);
    return SRSRAN_ERROR;
  }

  users[rnti]->establish_eps_bearer(pdu_session_id, nas_pdu, lcid, five_qi);

  // TODO: verify whether this is the best place where to call the RRCReconfig
  users[rnti]->send_rrc_reconfiguration();
  return SRSRAN_SUCCESS;
}

int rrc_nr::release_bearers(uint16_t rnti)
{
  return SRSRAN_SUCCESS;
}

void rrc_nr::release_user(uint16_t rnti)
{
  if (not users.contains(rnti)) {
    logger.warning("User rnti=0x%x has already been released", rnti);
    return;
  }

  users[rnti]->send_rrc_release();
}

int rrc_nr::allocate_lcid(uint16_t rnti)
{
  return SRSRAN_SUCCESS;
}

void rrc_nr::write_dl_info(uint16_t rnti, srsran::unique_byte_buffer_t sdu)
{
  if (not users.contains(rnti)) {
    logger.error("Received DL information transfer for inexistent rnti=0x%x", rnti);
    return;
  }
  if (sdu == nullptr or sdu->size() == 0) {
    logger.error("Received empty DL information transfer for rnti=0x%x", rnti);
    return;
  }
  users[rnti]->send_dl_information_transfer(std::move(sdu));
}

/*******************************************************************************
  Interface for EUTRA RRC
*******************************************************************************/

void rrc_nr::sgnb_addition_request(uint16_t eutra_rnti, const sgnb_addition_req_params_t& params)
{
  // try to allocate new user
  sched_nr_ue_cfg_t uecfg{};
  uecfg.carriers.resize(1);
  uecfg.carriers[0].active = true;
  uecfg.carriers[0].cc     = 0;
  uecfg.phy_cfg            = cell_ctxt->default_phy_ue_cfg_nr;

  uint16_t nr_rnti = mac->reserve_rnti(0, uecfg);
  if (nr_rnti == SRSRAN_INVALID_RNTI) {
    logger.error("Failed to allocate RNTI at MAC");
    rrc_eutra->sgnb_addition_reject(eutra_rnti);
    return;
  }

  if (add_user(nr_rnti, 0, false) != SRSRAN_SUCCESS) {
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

} // namespace srsenb
