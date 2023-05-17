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

#include "srsgnb/hdr/stack/rrc/rrc_nr_ue.h"
#include "srsgnb/hdr/stack/rrc/cell_asn1_config.h"
#include "srsgnb/hdr/stack/rrc/rrc_nr_config_utils.h"
#include "srsran/asn1/rrc_nr_utils.h"
#include "srsran/common/bearer_manager.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"

using namespace asn1::rrc_nr;

namespace srsenb {

/*******************************************************************************
  UE class

Every function in UE class is called from a mutex environment thus does not
    need extra protection.
    *******************************************************************************/
rrc_nr::ue::ue(rrc_nr* parent_, uint16_t rnti_, uint32_t pcell_cc_idx, bool start_msg3_timer) :
  parent(parent_), logger(parent_->logger), rnti(rnti_), uecfg(), sec_ctx(parent->cfg)
{
  // Set default MAC UE config
  uecfg.carriers.resize(1);
  uecfg.carriers[0].active = true;
  uecfg.carriers[0].cc     = pcell_cc_idx;
  uecfg.phy_cfg            = parent->cell_ctxt->default_phy_ue_cfg_nr;

  if (not parent->cfg.is_standalone) {
    // Add the final PDCCH config in case of NSA
    srsran::fill_phy_pdcch_cfg(
        parent->cell_ctxt->master_cell_group->sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.setup(),
        &uecfg.phy_cfg.pdcch);
  } else {
    cell_group_cfg      = *parent->cell_ctxt->master_cell_group;
    next_cell_group_cfg = cell_group_cfg;
  }

  // Set timer for MSG3_RX_TIMEOUT or UE_INACTIVITY_TIMEOUT
  activity_timer = parent->task_sched.get_unique_timer();
  start_msg3_timer ? set_activity_timeout(MSG3_RX_TIMEOUT) : set_activity_timeout(MSG5_RX_TIMEOUT);
}

rrc_nr::ue::~ue() {}

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
      deadline_ms = parent->cfg.inactivity_timeout_ms;
      break;
    default:
      logger.error("Unknown timeout type %d", type);
      return;
  }

  activity_timer.set(deadline_ms, [this, type](uint32_t tid) { activity_timer_expired(type); });
  logger.debug("Setting timer for %s for rnti=0x%x to %dms", to_string(type).c_str(), rnti, deadline_ms);

  set_activity();
}

void rrc_nr::ue::set_activity(bool enabled)
{
  if (not enabled) {
    if (activity_timer.is_running()) {
      logger.debug("Inactivity timer interrupted for rnti=0x%x", rnti);
    }
    activity_timer.stop();
    return;
  }

  // re-start activity timer with current timeout value
  activity_timer.run();
  logger.debug("Activity registered for rnti=0x%x (timeout_value=%dms)", rnti, activity_timer.duration());
}

void rrc_nr::ue::activity_timer_expired(const activity_timeout_type_t type)
{
  logger.info("Activity timer for rnti=0x%x expired after %d ms", rnti, activity_timer.time_elapsed());

  switch (type) {
    case MSG5_RX_TIMEOUT:
    case UE_INACTIVITY_TIMEOUT: {
      state = rrc_nr_state_t::RRC_INACTIVE;
      if (parent->cfg.is_standalone) {
        // Start NGAP Release UE context
        parent->ngap->user_release_request(rnti, asn1::ngap::cause_radio_network_opts::user_inactivity);
      } else {
        parent->rrc_eutra->sgnb_inactivity_timeout(eutra_rnti);
      }
      break;
    }
    case MSG3_RX_TIMEOUT: {
      // MSG3 timeout, no need to notify NGAP or LTE stack. Just remove UE
      state                = rrc_nr_state_t::RRC_IDLE;
      uint32_t rnti_to_rem = rnti;
      parent->task_sched.defer_task([this, rnti_to_rem]() { parent->rem_user(rnti_to_rem); });
      break;
    }
    default:
      // Unhandled activity timeout, just remove UE and log an error
      parent->rem_user(rnti);
      logger.error(
          "Unhandled reason for activity timer expiration. rnti=0x%x, cause %d", rnti, static_cast<unsigned>(type));
  }
}

std::string rrc_nr::ue::to_string(const activity_timeout_type_t& type)
{
  constexpr static const char* options[] = {"Msg3 reception", "UE inactivity", "Msg5 reception"};
  return srsran::enum_to_text(options, (uint32_t)activity_timeout_type_t::nulltype, (uint32_t)type);
}

int rrc_nr::ue::send_dl_ccch(const dl_ccch_msg_s& dl_ccch_msg)
{
  // Allocate a new PDU buffer, pack the message and send to PDCP
  srsran::unique_byte_buffer_t pdu = parent->pack_into_pdu(dl_ccch_msg, __FUNCTION__);
  if (pdu == nullptr) {
    logger.error("Failed to send DL-CCCH");
    return SRSRAN_ERROR;
  }
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, "DL-CCCH.{}", dl_ccch_msg.msg.c1().type().to_string());
  log_rrc_message(srsran::nr_srb::srb0, Tx, *pdu.get(), dl_ccch_msg, srsran::to_c_str(fmtbuf));
  parent->rlc->write_sdu(rnti, srsran::srb_to_lcid(srsran::nr_srb::srb0), std::move(pdu));
  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::send_dl_dcch(srsran::nr_srb srb, const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg)
{
  // Allocate a new PDU buffer, pack the message and send to PDCP
  srsran::unique_byte_buffer_t pdu = parent->pack_into_pdu(dl_dcch_msg, __FUNCTION__);
  if (pdu == nullptr) {
    return SRSRAN_ERROR;
  }
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, "DL-DCCH.{}", dl_dcch_msg.msg.c1().type().to_string());
  log_rrc_message(srb, Tx, *pdu.get(), dl_dcch_msg, srsran::to_c_str(fmtbuf));
  parent->pdcp->write_sdu(rnti, srsran::srb_to_lcid(srb), std::move(pdu));

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_secondary_cell_group_mac_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  // mac-CellGroup-Config for BSR and SR
  cell_group_cfg_pack.mac_cell_group_cfg_present = true;
  auto& mac_cell_group                           = cell_group_cfg_pack.mac_cell_group_cfg;
  mac_cell_group.sched_request_cfg_present       = true;
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list.resize(1);
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list[0].sched_request_id = 0;
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list[0].sr_trans_max =
      asn1::rrc_nr::sched_request_to_add_mod_s::sr_trans_max_opts::n64;
  mac_cell_group.bsr_cfg_present            = true;
  mac_cell_group.bsr_cfg.periodic_bsr_timer = asn1::rrc_nr::bsr_cfg_s::periodic_bsr_timer_opts::sf20;
  mac_cell_group.bsr_cfg.retx_bsr_timer     = asn1::rrc_nr::bsr_cfg_s::retx_bsr_timer_opts::sf320;

  // Skip TAG and PHR config
  mac_cell_group.tag_cfg_present = false;
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

  pack_sp_cell_cfg_ded_init_dl_bwp_radio_link_monitoring(cell_group_cfg_pack);

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::pack_sp_cell_cfg_ded_init_dl_bwp_radio_link_monitoring(
    asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.radio_link_monitoring_cfg_present = true;
  auto& radio_link_monitoring = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.radio_link_monitoring_cfg;

  // add resource to detect RLF
  radio_link_monitoring.set_setup().fail_detection_res_to_add_mod_list.resize(1);
  auto& fail_detec_res_elem = radio_link_monitoring.set_setup().fail_detection_res_to_add_mod_list[0];
  fail_detec_res_elem.radio_link_monitoring_rs_id = 0;
  fail_detec_res_elem.purpose                     = asn1::rrc_nr::radio_link_monitoring_rs_s::purpose_opts::rlf;
  fail_detec_res_elem.detection_res.set_ssb_idx() = 0;

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
  pucch_cfg.setup().sched_request_res_to_add_mod_list.resize(1);
  auto& sr_res1                             = pucch_cfg.setup().sched_request_res_to_add_mod_list[0];
  sr_res1.sched_request_res_id              = 1;
  sr_res1.sched_request_id                  = 0;
  sr_res1.periodicity_and_offset_present    = true;
  sr_res1.periodicity_and_offset.set_sl40() = 8;
  sr_res1.res_present                       = true;
  sr_res1.res                               = 2; // PUCCH resource for SR

  // DL data
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
  pucch_cfg.setup().res_to_add_mod_list.resize(3);
  if (not srsran::make_phy_res_config(resource_small, pucch_cfg.setup().res_to_add_mod_list[0], 0)) {
    logger.warning("Failed to create 1-2 bit NR PUCCH resource");
  }
  if (not srsran::make_phy_res_config(resource_big, pucch_cfg.setup().res_to_add_mod_list[1], 1)) {
    logger.warning("Failed to create >2 bit NR PUCCH resource");
  }
  if (not srsran::make_phy_res_config(resource_sr, pucch_cfg.setup().res_to_add_mod_list[2], 2)) {
    logger.warning("Failed to create SR NR PUCCH resource");
  }

  // Make 2 PUCCH resource sets
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
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded_present = true;

  pack_sp_cell_cfg_ded_ul_cfg(cell_group_cfg_pack);
  pack_sp_cell_cfg_ded_init_dl_bwp(cell_group_cfg_pack);

  // Serving cell config (only to setup)
  pack_sp_cell_cfg_ded_pdcch_serving_cell_cfg(cell_group_cfg_pack);

  // spCellConfig
  if (fill_sp_cell_cfg_from_enb_cfg(parent->cfg, UE_PSCELL_CC_IDX, cell_group_cfg_pack.sp_cell_cfg) != SRSRAN_SUCCESS) {
    logger.error("Failed to pack spCellConfig for rnti=0x%x", rnti);
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

  pack_secondary_cell_group_mac_cfg(cell_group_cfg_pack);
  pack_secondary_cell_group_sp_cell_cfg(cell_group_cfg_pack);

  // make sufficiant space
  packed_secondary_cell_config.resize(256);
  asn1::bit_ref bref_pack(packed_secondary_cell_config.data(), packed_secondary_cell_config.size());
  if (cell_group_cfg_pack.pack(bref_pack) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack NR secondary cell config");
    return SRSRAN_ERROR;
  }
  packed_secondary_cell_config.resize(bref_pack.distance_bytes());

  log_rrc_container(Tx, packed_secondary_cell_config, cell_group_cfg_pack, "nr-SecondaryCellGroupConfig-r15");

  return SRSRAN_SUCCESS;
}

// Packs a hard-coded RRC Reconfiguration with fixed params for all layers (for now)
int rrc_nr::ue::pack_rrc_reconfiguration(asn1::dyn_octstring& packed_rrc_reconfig)
{
  rrc_recfg_s reconfig;
  reconfig.rrc_transaction_id = ((transaction_id++) % 4u);
  rrc_recfg_ies_s& recfg_ies  = reconfig.crit_exts.set_rrc_recfg();

  // add secondary cell group config
  if (pack_secondary_cell_group_cfg(recfg_ies.secondary_cell_group) == SRSRAN_ERROR) {
    logger.error("Failed to pack secondary cell group");
    return SRSRAN_ERROR;
  }

  // now pack ..
  packed_rrc_reconfig.resize(512);
  asn1::bit_ref bref_pack(packed_rrc_reconfig.data(), packed_rrc_reconfig.size());
  if (reconfig.pack(bref_pack) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack RRC Reconfiguration");
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
    logger.error("Failed to pack NR radio bearer config");
    return SRSRAN_ERROR;
  }

  // resize to packed length
  packed_nr_bearer_config.resize(bref_pack.distance_bytes());

  log_rrc_container(Tx, packed_nr_bearer_config, radio_bearer_cfg_pack, "nr-RadioBearerConfig1-r15");

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::handle_sgnb_addition_request(uint16_t eutra_rnti_, const sgnb_addition_req_params_t& req_params)
{
  // Add DRB1 to RLC and PDCP
  if (add_drb(req_params.five_qi) != SRSRAN_SUCCESS) {
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
      uecfg.lc_ch_to_add.emplace_back();
      uecfg.lc_ch_to_add.back().lcid          = drb.lc_ch_id;
      uecfg.lc_ch_to_add.back().cfg.direction = mac_lc_ch_cfg_t::BOTH;
      uecfg.lc_ch_to_add.back().cfg.group     = drb.mac_lc_ch_cfg.ul_specific_params.lc_ch_group;
    }

    // Update UE phy params
    srsran::make_phy_ssb_cfg(parent->cfg.cell_list[0].phy_cell.carrier,
                             cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common,
                             &uecfg.phy_cfg.ssb);
    srsran::make_duplex_cfg_from_serv_cell(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common,
                                           &uecfg.phy_cfg.duplex);

    srsran_assert(check_nr_pdcch_cfg_valid(uecfg.phy_cfg.pdcch) == SRSRAN_SUCCESS, "Invalid PhyCell Config");

    uecfg.sp_cell_cfg.reset(new sp_cell_cfg_s{cell_group_cfg.sp_cell_cfg});
    uecfg.mac_cell_group_cfg.reset(new mac_cell_group_cfg_s{cell_group_cfg.mac_cell_group_cfg});
    uecfg.phy_cell_group_cfg.reset(new phys_cell_group_cfg_s{cell_group_cfg.phys_cell_group_cfg});
    parent->mac->ue_cfg(rnti, uecfg);
  }
}

/**
 * @brief Set DRB configuration
 *
 * The function sets and configures all relavant fields for the DRB configuration (MAC, RLC, PDCP) in the
 * cellGroupConfig and also adds the bearer to the local RLC and PDCP entities.
 *
 * @param  int 5QI of the DRB to be added
 * @return int SRSRAN_SUCCESS on success
 */
int rrc_nr::ue::add_drb(uint32_t five_qi)
{
  if (parent->cfg.five_qi_cfg.find(five_qi) == parent->cfg.five_qi_cfg.end()) {
    parent->logger.error("No bearer config for 5QI %d present. Aborting DRB addition.", five_qi);
    return SRSRAN_ERROR;
  }

  // RLC for DRB1 (with fixed LCID) inside cell_group_cfg
  auto& cell_group_cfg_pack = cell_group_cfg;

  cell_group_cfg_pack.rlc_bearer_to_add_mod_list.resize(1);
  auto& rlc_bearer                       = cell_group_cfg_pack.rlc_bearer_to_add_mod_list[0];
  rlc_bearer.lc_ch_id                    = drb1_lcid;
  rlc_bearer.served_radio_bearer_present = true;
  rlc_bearer.served_radio_bearer.set_drb_id();
  rlc_bearer.served_radio_bearer.drb_id() = 1;
  rlc_bearer.rlc_cfg_present              = true;
  rlc_bearer.rlc_cfg                      = parent->cfg.five_qi_cfg[five_qi].rlc_cfg;

  // add RLC bearer
  srsran::rlc_config_t rlc_cfg;
  /// NOTE, we need to pass the radio-bearer to the rlc_config
  if (srsran::make_rlc_config_t(cell_group_cfg.rlc_bearer_to_add_mod_list[0].rlc_cfg,
                                rlc_bearer.served_radio_bearer.drb_id(),
                                &rlc_cfg) != SRSRAN_SUCCESS) {
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
  auto& radio_bearer_cfg_pack = radio_bearer_cfg;
  radio_bearer_cfg_pack.drb_to_add_mod_list.resize(1);

  // configure fixed DRB1
  auto& drb_item                        = radio_bearer_cfg_pack.drb_to_add_mod_list[0];
  drb_item.drb_id                       = 1;
  drb_item.cn_assoc_present             = true;
  drb_item.cn_assoc.set_eps_bearer_id() = 5;
  drb_item.pdcp_cfg_present             = true;
  drb_item.pdcp_cfg                     = parent->cfg.five_qi_cfg[five_qi].pdcp_cfg;

  // Add DRB1 to PDCP
  srsran::pdcp_config_t pdcp_cnfg = srsran::make_drb_pdcp_config_t(drb_item.drb_id, false, drb_item.pdcp_cfg);
  parent->pdcp->add_bearer(rnti, rlc_bearer.lc_ch_id, pdcp_cnfg);

  // Note: DRB1 is only activated in the MAC when the C-RNTI CE is received

  return SRSRAN_SUCCESS;
}

void rrc_nr::ue::handle_rrc_setup_request(const asn1::rrc_nr::rrc_setup_request_s& msg)
{
  const uint8_t max_wait_time_secs = 16;
  if (not parent->ngap->is_amf_connected()) {
    logger.error("MME isn't connected. Sending Connection Reject");
    send_rrc_reject(max_wait_time_secs);
    return;
  }

  // Allocate PUCCH resources and reject if not available
  if (not init_pucch()) {
    logger.warning("Could not allocate PUCCH resources for rnti=0x%x. Sending Connection Reject", rnti);
    send_rrc_reject(max_wait_time_secs);
    return;
  }

  const rrc_setup_request_ies_s& ies = msg.rrc_setup_request;

  switch (ies.ue_id.type().value) {
    case init_ue_id_c::types_opts::ng_minus5_g_s_tmsi_part1:
      ctxt.setup_ue_id = ies.ue_id.ng_minus5_g_s_tmsi_part1().to_number();
      break;
    case asn1::rrc_nr::init_ue_id_c::types_opts::random_value:
      ctxt.setup_ue_id = ies.ue_id.random_value().to_number();
      // TODO: communicate with NGAP
      break;
    default:
      logger.error("Unsupported RRCSetupRequest");
      send_rrc_reject(max_wait_time_secs);
      return;
  }
  ctxt.connection_cause.value = ies.establishment_cause.value;

  send_rrc_setup();
  set_activity_timeout(UE_INACTIVITY_TIMEOUT);
}

void rrc_nr::ue::handle_rrc_reestablishment_request(const asn1::rrc_nr::rrc_reest_request_s& msg)
{
  uint32_t old_rnti = msg.rrc_reest_request.ue_id.c_rnti;
  uint16_t pci      = msg.rrc_reest_request.ue_id.pci;

  // Log event
  parent->logger.debug("rnti=0x%x, phyid=0x%x, smac=0x%x, cause=%s",
                       old_rnti,
                       pci,
                       (uint32_t)msg.rrc_reest_request.ue_id.short_mac_i.to_number(),
                       msg.rrc_reest_request.reest_cause.to_string());

  // Check AMF connection
  const uint8_t max_wait_time_secs = 16;
  if (not parent->ngap->is_amf_connected()) {
    logger.error("AMF not connected. Sending Connection Reject.");
    send_rrc_reject(max_wait_time_secs);
    return;
  }

  // Allocate PUCCH resources and reject if not available
  if (not init_pucch()) {
    logger.warning("Could not allocate PUCCH resources for rnti=0x%x. Sending RRC Reject.", rnti);
    send_rrc_reject(max_wait_time_secs);
    return;
  }

  if (not is_idle()) {
    // The created RNTI has to receive ReestablishmentRequest as first message
    parent->logger.error(
        "Could not reestablish connection for rnti=0x%x. Cause: old rnti=0x%x is not in RRC_IDLE.", rnti, old_rnti);
    send_rrc_reject(max_wait_time_secs);
    return;
  }

  auto old_ue_it = parent->users.find(old_rnti);

  // Fallback to connection establishment for unrecognized rntis, and PCIs that do not belong to eNB
  if (old_ue_it == parent->users.end()) {
    parent->logger.info(
        "Fallback to connection establishment for rnti=0x%x. Cause: no rnti=0x%x context available", rnti, old_rnti);
    srsran::console("Fallback to connection establishment for rnti=0x%x. Cause: no context available\n", rnti);

    // send RRC Setup
    send_rrc_setup();
    set_activity_timeout(UE_INACTIVITY_TIMEOUT);

    return;
  }

  // Reestablishment procedure going forward
  parent->logger.info("ConnectionReestablishmentRequest for rnti=0x%x. Sending Connection Reestablishment.", old_rnti);
  srsran::console("User 0x%x requesting RRC Reestablishment as 0x%x. Cause: %s\n",
                  rnti,
                  old_rnti,
                  msg.rrc_reest_request.reest_cause.to_string());

  ue* old_ue = old_ue_it->second.get();

  // Recover security setup
  sec_ctx          = old_ue->sec_ctx;
  auto& pscell_cfg = parent->cfg.cell_list.at(UE_PSCELL_CC_IDX);
  sec_ctx.regenerate_keys_handover(pscell_cfg.phy_cell.carrier.pci, pscell_cfg.ssb_absolute_freq_point);

  // For the reestablishment, only add SRB1 to new UE context
  next_radio_bearer_cfg.srb_to_add_mod_list.resize(1);
  srb_to_add_mod_s& srb1 = next_radio_bearer_cfg.srb_to_add_mod_list[0];
  srb1.srb_id            = 1;

  // compute config and create SRB1 for new user
  asn1::rrc_nr::radio_bearer_cfg_s dummy_radio_bearer_cfg; // just to compute difference, it's never sent to UE
  compute_diff_radio_bearer_cfg(parent->cfg, radio_bearer_cfg, next_radio_bearer_cfg, dummy_radio_bearer_cfg);
  if (fill_cellgroup_with_radio_bearer_cfg(
          parent->cfg, old_rnti, *parent->bearer_mapper, dummy_radio_bearer_cfg, next_cell_group_cfg) !=
      SRSRAN_SUCCESS) {
    logger.error("Couldn't fill cellGroupCfg during RRC Reestablishment");
    send_rrc_reject(max_wait_time_secs);
    return;
  }

  // send RRC Reestablishment message and restore bearer configuration
  send_connection_reest(old_ue->sec_ctx.get_ncc());

  // store current bearer/cell config with configured SRB1
  radio_bearer_cfg = next_radio_bearer_cfg;
  cell_group_cfg   = next_cell_group_cfg;

  // recover all previously created bearers from old UE object for (later) reconfiguration
  next_radio_bearer_cfg = old_ue->radio_bearer_cfg;
  next_cell_group_cfg   = old_ue->cell_group_cfg;

  // Recover GTP-U tunnels and NGAP context
  parent->gtpu->mod_bearer_rnti(old_rnti, rnti);
  parent->ngap->user_mod(old_rnti, rnti);

  // Reestablish E-RABs of old rnti later, during ConnectionReconfiguration
  // bearer_list.reestablish_bearers(std::move(old_ue->bearer_list));
  drb1_five_qi = old_ue->drb1_five_qi;

  // remove old RNTI
  old_ue->deactivate_bearers();
  parent->bearer_mapper->rem_user(old_rnti);
  parent->task_sched.defer_task([this, old_rnti]() { parent->rem_user(old_rnti); });

  set_activity_timeout(MSG5_RX_TIMEOUT);
}

void rrc_nr::ue::send_connection_reest(uint8_t ncc)
{
  dl_dcch_msg_s    msg;
  rrc_reest_ies_s& reest = msg.msg.set_c1().set_rrc_reest().crit_exts.set_rrc_reest();

  msg.msg.c1().rrc_reest().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);

  // set NCC
  reest.next_hop_chaining_count = ncc;

  // add RLC bearers
  update_rlc_bearers(next_cell_group_cfg);

  // add PDCP bearers
  // this is done after updating the RLC bearers,
  // so the PDCP can query the RLC mode
  update_pdcp_bearers(next_radio_bearer_cfg, next_cell_group_cfg);

  // add MAC bearers
  update_mac(next_cell_group_cfg, false);

  if (send_dl_dcch(srsran::nr_srb::srb1, msg) != SRSRAN_SUCCESS) {
    // TODO: Handle
  }
}

/// TS 38.331, RRCReject message
void rrc_nr::ue::send_rrc_reject(uint8_t reject_wait_time_secs)
{
  dl_ccch_msg_s     msg;
  rrc_reject_ies_s& reject = msg.msg.set_c1().set_rrc_reject().crit_exts.set_rrc_reject();

  // See TS 38.331, RejectWaitTime
  if (reject_wait_time_secs > 0) {
    reject.wait_time_present = true;
    reject.wait_time         = reject_wait_time_secs;
  }
  if (send_dl_ccch(msg) != SRSRAN_SUCCESS) {
    // TODO: Handle
  }

  // TODO: remove user
}

/// TS 38.331, RRCSetup
void rrc_nr::ue::send_rrc_setup()
{
  const uint8_t max_wait_time_secs = 16;

  // Add SRB1 to UE context
  // Note: See 5.3.5.6.3 - SRB addition/modification
  next_radio_bearer_cfg.srb_to_add_mod_list.resize(1);
  srb_to_add_mod_s& srb1 = next_radio_bearer_cfg.srb_to_add_mod_list[0];
  srb1.srb_id            = 1;

  // Generate RRC setup message
  dl_ccch_msg_s msg;
  rrc_setup_s&  setup        = msg.msg.set_c1().set_rrc_setup();
  setup.rrc_transaction_id   = (uint8_t)((transaction_id++) % 4);
  rrc_setup_ies_s& setup_ies = setup.crit_exts.set_rrc_setup();

  // Fill RRC Setup
  // - Setup SRB1
  compute_diff_radio_bearer_cfg(parent->cfg, radio_bearer_cfg, next_radio_bearer_cfg, setup_ies.radio_bearer_cfg);

  // - Setup masterCellGroup
  // - Derive master cell group config bearers
  if (fill_cellgroup_with_radio_bearer_cfg(
          parent->cfg, rnti, *parent->bearer_mapper, setup_ies.radio_bearer_cfg, next_cell_group_cfg) !=
      SRSRAN_SUCCESS) {
    logger.error("Couldn't fill cellGroupCfg during RRC Setup");
    send_rrc_reject(max_wait_time_secs);
    return;
  }

  // - Pack masterCellGroup into container
  srsran::unique_byte_buffer_t pdu = parent->pack_into_pdu(next_cell_group_cfg, __FUNCTION__);
  if (pdu == nullptr) {
    send_rrc_reject(max_wait_time_secs);
    return;
  }
  setup_ies.master_cell_group.resize(pdu->N_bytes);
  memcpy(setup_ies.master_cell_group.data(), pdu->data(), pdu->N_bytes);
  if (logger.debug.enabled()) {
    asn1::json_writer js;
    next_cell_group_cfg.to_json(js);
    logger.debug("Containerized MasterCellGroup: %s", js.to_string().c_str());
  }

  // add RLC bearers
  update_rlc_bearers(next_cell_group_cfg);

  // add PDCP bearers
  // this is done after updating the RLC bearers,
  // so the PDCP can query the RLC mode
  update_pdcp_bearers(next_radio_bearer_cfg, next_cell_group_cfg);

  // add MAC bearers
  update_mac(next_cell_group_cfg, false);

  // Send RRC Setup message to UE
  if (send_dl_ccch(msg) != SRSRAN_SUCCESS) {
    send_rrc_reject(max_wait_time_secs);
  }
}

/// TS 38.331, RRCSetupComplete
void rrc_nr::ue::handle_rrc_setup_complete(const asn1::rrc_nr::rrc_setup_complete_s& msg)
{
  update_mac(next_cell_group_cfg, true);

  // Update current radio bearer cfg
  radio_bearer_cfg = next_radio_bearer_cfg;
  cell_group_cfg   = next_cell_group_cfg;

  // Create UE context in NGAP
  using ngap_cause_t = asn1::ngap::rrcestablishment_cause_opts::options;
  auto ngap_cause    = (ngap_cause_t)ctxt.connection_cause.value; // NGAP and RRC causes seem to have a 1-1 mapping
  parent->ngap->initial_ue(
      rnti, uecfg.carriers[0].cc, ngap_cause, msg.crit_exts.rrc_setup_complete().ded_nas_msg, ctxt.setup_ue_id);
}

/// TS 38.331, SecurityModeCommand
void rrc_nr::ue::send_security_mode_command(srsran::unique_byte_buffer_t nas_pdu)
{
  // apply selected security config and enable integrity on SRB1 before generating security mode command
  update_as_security(srb_to_lcid(srsran::nr_srb::srb1), true, false);

  if (nas_pdu != nullptr) {
    nas_pdu_queue.push_back(std::move(nas_pdu));
  }

  asn1::rrc_nr::dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_security_mode_cmd().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
  security_mode_cmd_ies_s& ies = dl_dcch_msg.msg.c1().security_mode_cmd().crit_exts.set_security_mode_cmd();

  ies.security_cfg_smc.security_algorithm_cfg.integrity_prot_algorithm_present = true;
  ies.security_cfg_smc.security_algorithm_cfg                                  = sec_ctx.get_security_algorithm_cfg();

  if (send_dl_dcch(srsran::nr_srb::srb1, dl_dcch_msg) != SRSRAN_SUCCESS) {
    parent->ngap->user_release_request(rnti, asn1::ngap::cause_radio_network_opts::radio_res_not_available);
  }
}

/**
 * @brief  Internal helper to update the security configuration of a PDCP bearer
 *
 * If no valid AS security config is present (yet) the method doesn't modify the
 * PDCP config and returns SRSRAN_ERROR. In some cases, however,
 * for example during RRC Setup, this is in fact the expected behaviour as
 * AS security isn't established yet.
 *
 * @param lcid Logical channel ID of the bearer
 * @param enable_integrity Whether to enable integrity protection for the bearer
 * @param enable_ciphering Whether to enable ciphering for the bearer
 * @return int SRSRAN_SUCCESS if a valid AS security config was found and the security was configured
 */
int rrc_nr::ue::update_as_security(uint32_t lcid, bool enable_integrity = true, bool enable_ciphering = true)
{
  if (not sec_ctx.is_as_sec_cfg_valid()) {
    parent->logger.error("Invalid AS security configuration. Skipping configuration for lcid=%d", lcid);
    return SRSRAN_ERROR;
  }

  // TODO: Currently we are using the PDCP-LTE, so we need to convert from nr_as_security_cfg to as_security_config.
  // When we start using PDCP-NR we can avoid this step.
  srsran::nr_as_security_config_t tmp_cnfg  = sec_ctx.get_as_sec_cfg();
  srsran::as_security_config_t    pdcp_cnfg = {};
  pdcp_cnfg.k_rrc_int                       = tmp_cnfg.k_nr_rrc_int;
  pdcp_cnfg.k_rrc_enc                       = tmp_cnfg.k_nr_rrc_enc;
  pdcp_cnfg.k_up_int                        = tmp_cnfg.k_nr_up_int;
  pdcp_cnfg.k_up_enc                        = tmp_cnfg.k_nr_up_enc;
  pdcp_cnfg.integ_algo                      = (srsran::INTEGRITY_ALGORITHM_ID_ENUM)tmp_cnfg.integ_algo;
  pdcp_cnfg.cipher_algo                     = (srsran::CIPHERING_ALGORITHM_ID_ENUM)tmp_cnfg.cipher_algo;

  // configure algorithm and keys
  parent->pdcp->config_security(rnti, lcid, pdcp_cnfg);

  if (enable_integrity) {
    parent->pdcp->enable_integrity(rnti, lcid);
  }

  if (enable_ciphering) {
    parent->pdcp->enable_encryption(rnti, lcid);
  }

  return SRSRAN_SUCCESS;
}

/// TS 38.331, SecurityModeComplete
void rrc_nr::ue::handle_security_mode_complete(const asn1::rrc_nr::security_mode_complete_s& msg)
{
  parent->logger.info("SecurityModeComplete transaction ID: %d", msg.rrc_transaction_id);

  // finally, also enable ciphering on SRB1
  update_as_security(srb_to_lcid(srsran::nr_srb::srb1), false, true);

  send_ue_capability_enquiry();
}

/// TS 38.331, RRCReconfiguration
void rrc_nr::ue::send_rrc_reconfiguration()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_recfg().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
  rrc_recfg_ies_s& ies = dl_dcch_msg.msg.c1().rrc_recfg().crit_exts.set_rrc_recfg();

  // Add new SRBs/DRBs
  ies.radio_bearer_cfg_present =
      compute_diff_radio_bearer_cfg(parent->cfg, radio_bearer_cfg, next_radio_bearer_cfg, ies.radio_bearer_cfg);

  // If no bearer to add/mod/remove, do not include master_cell_group
  // Set ies.non_crit_ext_present (a few lines below) only if
  // master_cell_group_present == true or ies.non_crit_ext.ded_nas_msg_list_present == true
  if (ies.radio_bearer_cfg_present) {
    // Fill masterCellGroup
    cell_group_cfg_s master_cell_group;
    master_cell_group.cell_group_id = 0;
    if (fill_cellgroup_with_radio_bearer_cfg(
            parent->cfg, rnti, *parent->bearer_mapper, ies.radio_bearer_cfg, master_cell_group) != SRSRAN_SUCCESS) {
      logger.error("Couldn't fill cellGroupCfg during RRC Reconfiguration");
      parent->ngap->user_release_request(rnti, asn1::ngap::cause_radio_network_opts::radio_res_not_available);
      return;
    }

    // Pack masterCellGroup into container
    srsran::unique_byte_buffer_t pdu = parent->pack_into_pdu(master_cell_group, __FUNCTION__);
    if (pdu == nullptr) {
      parent->ngap->user_release_request(rnti, asn1::ngap::cause_radio_network_opts::radio_res_not_available);
      return;
    }
    ies.non_crit_ext.master_cell_group.resize(pdu->N_bytes);
    memcpy(ies.non_crit_ext.master_cell_group.data(), pdu->data(), pdu->N_bytes);
    if (logger.debug.enabled()) {
      asn1::json_writer js;
      master_cell_group.to_json(js);
      logger.debug("Containerized MasterCellGroup: %s", js.to_string().c_str());
    }

    // Update lower layers
    // add MAC bearers
    update_mac(master_cell_group, false);

    // add RLC bearers
    update_rlc_bearers(master_cell_group);

    // add PDCP bearers
    // this is done after updating the RLC bearers,
    // so the PDCP can query the RLC mode
    update_pdcp_bearers(ies.radio_bearer_cfg, master_cell_group);
  }

  if (nas_pdu_queue.size() > 0) {
    // Pass stored NAS PDUs
    ies.non_crit_ext.ded_nas_msg_list.resize(nas_pdu_queue.size());
    for (uint32_t i = 0; i < nas_pdu_queue.size(); ++i) {
      ies.non_crit_ext.ded_nas_msg_list[i].resize(nas_pdu_queue[i]->size());
      memcpy(ies.non_crit_ext.ded_nas_msg_list[i].data(), nas_pdu_queue[i]->data(), nas_pdu_queue[i]->size());
    }
    nas_pdu_queue.clear();
  }

  ies.non_crit_ext_present =
      ies.non_crit_ext.master_cell_group.size() > 0 or ies.non_crit_ext.ded_nas_msg_list.size() > 0;

  if (send_dl_dcch(srsran::nr_srb::srb1, dl_dcch_msg) != SRSRAN_SUCCESS) {
    parent->ngap->user_release_request(rnti, asn1::ngap::cause_radio_network_opts::radio_res_not_available);
  }
}

int rrc_nr::ue::send_ue_capability_enquiry()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_ue_cap_enquiry().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
  ue_cap_enquiry_ies_s& ies = dl_dcch_msg.msg.c1().ue_cap_enquiry().crit_exts.set_ue_cap_enquiry();

  // ue-CapabilityRAT-RequestList
  ue_cap_rat_request_s cap_rat_request;
  cap_rat_request.rat_type.value = rat_type_opts::nr;

  // capabilityRequestFilter
  ue_cap_request_filt_nr_s request_filter;

  // frequencyBandListFilter
  freq_band_info_c     freq_band_info;
  freq_band_info_nr_s& freq_band_info_nr = freq_band_info.set_band_info_nr();

  // Iterate through cell list and assign bandInformationNR items
  for (auto& iter : parent->cfg.cell_list) {
    freq_band_info_nr.band_nr = iter.band;
    request_filter.freq_band_list_filt.push_back(freq_band_info);
  }

  // Pack capabilityRequestFilter
  cap_rat_request.cap_request_filt.resize(128);
  asn1::bit_ref bref_pack(cap_rat_request.cap_request_filt.data(), cap_rat_request.cap_request_filt.size());
  if (request_filter.pack(bref_pack) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack capabilityRequestFilter in UE Capability Enquiry");
    return SRSRAN_ERROR;
  }
  cap_rat_request.cap_request_filt.resize(bref_pack.distance_bytes());

  ies.ue_cap_rat_request_list.push_back(cap_rat_request);

  send_dl_dcch(srsran::nr_srb::srb1, dl_dcch_msg);

  return SRSRAN_SUCCESS;
}

void rrc_nr::ue::handle_ue_capability_information(const asn1::rrc_nr::ue_cap_info_s& msg)
{
  logger.info("UECapabilityInformation transaction ID: %d", msg.rrc_transaction_id);

  send_rrc_reconfiguration();

  // Send RRCReconfiguration if necessary
  if (not nas_pdu_queue.empty()) {
    send_rrc_reconfiguration();
  }
}

void rrc_nr::ue::handle_rrc_reconfiguration_complete(const asn1::rrc_nr::rrc_recfg_complete_s& msg)
{
  update_mac(next_cell_group_cfg, true);

  radio_bearer_cfg = next_radio_bearer_cfg;
  cell_group_cfg   = next_cell_group_cfg;
  parent->ngap->ue_notify_rrc_reconf_complete(rnti, true);
}

void rrc_nr::ue::handle_rrc_reestablishment_complete(const asn1::rrc_nr::rrc_reest_complete_s& msg)
{
  // Register DRB again, TODO: combine/move to establish_eps_bearer()
  for (const auto& drb : next_radio_bearer_cfg.drb_to_add_mod_list) {
    uint16_t lcid = drb1_lcid;
    parent->bearer_mapper->add_eps_bearer(rnti, lcid - 3, srsran::srsran_rat_t::nr, lcid);
    parent->bearer_mapper->set_five_qi(rnti, lcid - 3, drb1_five_qi);

    logger.info("Established EPS bearer for LCID %u and RNTI 0x%x", lcid, rnti);
  }

  // send reconfiguration to reestablish SRB2 and all previously established DRBs
  send_rrc_reconfiguration();
}

void rrc_nr::ue::send_rrc_release()
{
  static const uint32_t release_delay = 60; // Taken from TS 38.331, 5.3.8.3

  dl_dcch_msg_s  dl_dcch_msg;
  rrc_release_s& release = dl_dcch_msg.msg.set_c1().set_rrc_release();

  release.rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
  rrc_release_ies_s& ies     = release.crit_exts.set_rrc_release();

  ies.suspend_cfg_present = false; // goes to RRC_IDLE

  send_dl_dcch(srsran::nr_srb::srb1, dl_dcch_msg);
  state = rrc_nr_state_t::RRC_IDLE;

  // TODO: Obtain acknowledgment from lower layers that RRC Release was received
  parent->task_sched.defer_callback(release_delay, [this]() { parent->rem_user(rnti); });
}

void rrc_nr::ue::send_dl_information_transfer(srsran::unique_byte_buffer_t sdu)
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_dl_info_transfer().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
  dl_info_transfer_ies_s& ies = dl_dcch_msg.msg.c1().dl_info_transfer().crit_exts.set_dl_info_transfer();

  ies.ded_nas_msg.resize(sdu->N_bytes);
  memcpy(ies.ded_nas_msg.data(), sdu->data(), ies.ded_nas_msg.size());

  if (send_dl_dcch(srsran::nr_srb::srb1, dl_dcch_msg) != SRSRAN_SUCCESS) {
    parent->ngap->user_release_request(rnti, asn1::ngap::cause_radio_network_opts::radio_res_not_available);
  }
}

void rrc_nr::ue::handle_ul_information_transfer(const asn1::rrc_nr::ul_info_transfer_s& msg)
{
  // Forward dedicatedNAS-Message to NGAP
  parent->ngap->write_pdu(rnti, msg.crit_exts.ul_info_transfer().ded_nas_msg);
}

void rrc_nr::ue::establish_eps_bearer(uint32_t                pdu_session_id,
                                      srsran::const_byte_span nas_pdu,
                                      uint32_t                lcid,
                                      uint32_t                five_qi)
{
  if (parent->cfg.five_qi_cfg.find(five_qi) == parent->cfg.five_qi_cfg.end()) {
    parent->logger.error("No bearer config for 5QI %d present. Aborting DRB addition.", five_qi);
    return;
  }

  // Enqueue NAS PDU
  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Couldn't allocate NAS PDU in %s().", __FUNCTION__);
    return;
  }
  pdu->resize(nas_pdu.size());
  memcpy(pdu->data(), nas_pdu.data(), nas_pdu.size());
  nas_pdu_queue.push_back(std::move(pdu));

  // Add SRB2, if not yet added
  asn1::rrc_nr::srb_to_add_mod_s* srb_it =
      std::find_if(radio_bearer_cfg.srb_to_add_mod_list.begin(),
                   radio_bearer_cfg.srb_to_add_mod_list.end(),
                   [](const asn1::rrc_nr::srb_to_add_mod_s& srb) { return srb.srb_id == 2; });

  if (srb_it == radio_bearer_cfg.srb_to_add_mod_list.end()) {
    next_radio_bearer_cfg.srb_to_add_mod_list.push_back(srb_to_add_mod_s{});
    next_radio_bearer_cfg.srb_to_add_mod_list.back().srb_id = 2;
  }

  drb_to_add_mod_s drb;
  drb.cn_assoc_present                      = true;
  drb.cn_assoc.set_sdap_cfg().pdu_session   = pdu_session_id;
  drb.cn_assoc.sdap_cfg().sdap_hdr_dl.value = asn1::rrc_nr::sdap_cfg_s::sdap_hdr_dl_opts::absent;
  drb.cn_assoc.sdap_cfg().sdap_hdr_ul.value = asn1::rrc_nr::sdap_cfg_s::sdap_hdr_ul_opts::absent;
  drb.cn_assoc.sdap_cfg().default_drb       = true;
  drb.cn_assoc.sdap_cfg().mapped_qos_flows_to_add.resize(1);
  drb.cn_assoc.sdap_cfg().mapped_qos_flows_to_add[0] = 1;

  drb.drb_id           = lcid - srsran::MAX_NR_SRB_ID;
  drb.pdcp_cfg_present = true;
  drb.pdcp_cfg         = parent->cfg.five_qi_cfg[five_qi].pdcp_cfg;

  next_radio_bearer_cfg.drb_to_add_mod_list.push_back(drb);

  parent->bearer_mapper->add_eps_bearer(rnti,
                                        pdu_session_id,
                                        srsran::srsran_rat_t::nr,
                                        lcid); // TODO: configurable bearer id <-> lcid mapping
  parent->bearer_mapper->set_five_qi(rnti, pdu_session_id, five_qi);

  // store 5QI for possible reestablishment of DRB
  drb1_five_qi = five_qi;

  logger.info("Established EPS bearer for LCID %u and RNTI 0x%x", lcid, rnti);
}

bool rrc_nr::ue::init_pucch()
{
  // TODO: Allocate PUCCH resources

  return true;
}

int rrc_nr::ue::update_pdcp_bearers(const asn1::rrc_nr::radio_bearer_cfg_s& radio_bearer_diff,
                                    const asn1::rrc_nr::cell_group_cfg_s&   cell_group_diff)
{
  // release DRBs
  // TODO

  // add SRBs
  for (const srb_to_add_mod_s& srb : radio_bearer_diff.srb_to_add_mod_list) {
    srsran::pdcp_config_t   pdcp_cnfg  = srsran::make_nr_srb_pdcp_config_t(srb.srb_id, false);
    const rlc_bearer_cfg_s* rlc_bearer = nullptr;
    for (const rlc_bearer_cfg_s& item : cell_group_diff.rlc_bearer_to_add_mod_list) {
      if (item.served_radio_bearer.type().value == rlc_bearer_cfg_s::served_radio_bearer_c_::types_opts::srb_id and
          item.served_radio_bearer.srb_id() == srb.srb_id) {
        rlc_bearer = &item;
        break;
      }
    }
    if (rlc_bearer == nullptr) {
      logger.error("Inconsistency between cellGroupConfig and radioBearerConfig in ASN1 message");
      return SRSRAN_ERROR;
    }
    parent->pdcp->add_bearer(rnti, rlc_bearer->lc_ch_id, pdcp_cnfg);

    if (sec_ctx.is_as_sec_cfg_valid()) {
      update_as_security(rlc_bearer->lc_ch_id);
    }
  }

  // Add DRBs
  for (const drb_to_add_mod_s& drb : radio_bearer_diff.drb_to_add_mod_list) {
    srsran::pdcp_config_t   pdcp_cnfg  = srsran::make_drb_pdcp_config_t(drb.drb_id, false, drb.pdcp_cfg);
    const rlc_bearer_cfg_s* rlc_bearer = nullptr;
    for (const rlc_bearer_cfg_s& item : cell_group_diff.rlc_bearer_to_add_mod_list) {
      if (item.served_radio_bearer.type().value == rlc_bearer_cfg_s::served_radio_bearer_c_::types_opts::drb_id and
          item.served_radio_bearer.drb_id() == drb.drb_id) {
        rlc_bearer = &item;
        break;
      }
    }
    if (rlc_bearer == nullptr) {
      logger.error("Inconsistency between cellGroupConfig and radioBearerConfig in ASN1 message");
      return SRSRAN_ERROR;
    }
    parent->pdcp->add_bearer(rnti, rlc_bearer->lc_ch_id, pdcp_cnfg);

    if (sec_ctx.is_as_sec_cfg_valid()) {
      update_as_security(rlc_bearer->lc_ch_id, drb.pdcp_cfg.drb.integrity_protection_present, true);
    }
  }

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::update_rlc_bearers(const asn1::rrc_nr::cell_group_cfg_s& cell_group_diff)
{
  // Release RLC radio bearers
  for (uint8_t lcid : cell_group_diff.rlc_bearer_to_release_list) {
    parent->rlc->del_bearer(rnti, lcid);
  }

  // Add/Mod RLC radio bearers
  for (const rlc_bearer_cfg_s& rb : cell_group_diff.rlc_bearer_to_add_mod_list) {
    srsran::rlc_config_t rlc_cfg;
    uint8_t              rb_id = 0;
    if (rb.served_radio_bearer.type().value == rlc_bearer_cfg_s::served_radio_bearer_c_::types_opts::srb_id) {
      rb_id = rb.served_radio_bearer.srb_id();
      if (not rb.rlc_cfg_present) {
        rlc_cfg = srsran::rlc_config_t::default_rlc_am_nr_config();
      } else {
        if (srsran::make_rlc_config_t(rb.rlc_cfg, rb_id, &rlc_cfg) != SRSRAN_SUCCESS) {
          logger.error("Failed to build RLC config");
          // TODO: HANDLE
          return SRSRAN_ERROR;
        }
      }
    } else {
      rb_id = rb.served_radio_bearer.drb_id();
      if (not rb.rlc_cfg_present) {
        logger.error("No RLC config for DRB");
        // TODO: HANDLE
        return SRSRAN_ERROR;
      }
      if (srsran::make_rlc_config_t(rb.rlc_cfg, rb_id, &rlc_cfg) != SRSRAN_SUCCESS) {
        logger.error("Failed to build RLC config");
        // TODO: HANDLE
        return SRSRAN_ERROR;
      }
    }
    parent->rlc->add_bearer(rnti, rb.lc_ch_id, rlc_cfg);
  }

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::update_mac(const cell_group_cfg_s& cell_group_config, bool is_config_complete)
{
  if (not is_config_complete) {
    // Release bearers
    for (uint8_t lcid : cell_group_config.rlc_bearer_to_release_list) {
      uecfg.lc_ch_to_rem.push_back(lcid);
    }

    for (const rlc_bearer_cfg_s& bearer : cell_group_config.rlc_bearer_to_add_mod_list) {
      uecfg.lc_ch_to_add.emplace_back();
      uecfg.lc_ch_to_add.back().lcid = bearer.lc_ch_id;
      auto& lch                      = uecfg.lc_ch_to_add.back().cfg;
      lch.direction                  = mac_lc_ch_cfg_t::BOTH;
      if (bearer.mac_lc_ch_cfg.ul_specific_params_present) {
        lch.priority = bearer.mac_lc_ch_cfg.ul_specific_params.prio;
        lch.pbr      = bearer.mac_lc_ch_cfg.ul_specific_params.prioritised_bit_rate.to_number();
        lch.bsd      = bearer.mac_lc_ch_cfg.ul_specific_params.bucket_size_dur.to_number();
        lch.group    = bearer.mac_lc_ch_cfg.ul_specific_params.lc_ch_group;
        // TODO: remaining fields
      }
    }

    if (cell_group_config.sp_cell_cfg_present and cell_group_config.sp_cell_cfg.sp_cell_cfg_ded_present and
        cell_group_config.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg_present and
        cell_group_config.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp_present and
        cell_group_config.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg_present) {
      auto& pucch_cfg = cell_group_config.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup();
      srsran::fill_phy_pucch_cfg(pucch_cfg, &uecfg.phy_cfg.pucch);
    }
  } else {
    auto& pdcch = cell_group_config.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.setup();
    for (auto& ss : pdcch.search_spaces_to_add_mod_list) {
      uecfg.phy_cfg.pdcch.search_space_present[ss.search_space_id] = true;
      srsran::make_phy_search_space_cfg(ss, &uecfg.phy_cfg.pdcch.search_space[ss.search_space_id]);
    }
    for (auto& cs : pdcch.ctrl_res_set_to_add_mod_list) {
      uecfg.phy_cfg.pdcch.coreset_present[cs.ctrl_res_set_id] = true;
      srsran::make_phy_coreset_cfg(cs, &uecfg.phy_cfg.pdcch.coreset[cs.ctrl_res_set_id]);
    }
  }

  uecfg.sp_cell_cfg.reset(new sp_cell_cfg_s{cell_group_cfg.sp_cell_cfg});
  uecfg.mac_cell_group_cfg.reset(new mac_cell_group_cfg_s{cell_group_cfg.mac_cell_group_cfg});
  uecfg.phy_cell_group_cfg.reset(new phys_cell_group_cfg_s{cell_group_cfg.phys_cell_group_cfg});
  srsran::make_csi_cfg_from_serv_cell(cell_group_config.sp_cell_cfg.sp_cell_cfg_ded, &uecfg.phy_cfg.csi);
  parent->mac->ue_cfg(rnti, uecfg);

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
  for (uint32_t lcid = 1; lcid < SCHED_NR_MAX_LCID; ++lcid) {
    uecfg.lc_ch_to_rem.push_back(lcid);
  }

  // No need to check the returned value, as the function ue_cfg will return SRSRAN_SUCCESS (it asserts if it fails)
  uecfg.phy_cell_group_cfg = {};
  uecfg.mac_cell_group_cfg = {};
  uecfg.sp_cell_cfg        = {};
  parent->mac->ue_cfg(rnti, uecfg);
}

template <class M>
void rrc_nr::ue::log_rrc_message(srsran::nr_srb          srb,
                                 const direction_t       dir,
                                 srsran::const_byte_span pdu,
                                 const M&                msg,
                                 const char*             msg_type)
{
  fmt::memory_buffer strbuf;
  fmt::format_to(strbuf, "rnti=0x{:x}, {}", rnti, srsran::get_srb_name(srb));
  parent->log_rrc_message(srsran::to_c_str(strbuf), Tx, pdu, msg, msg_type);
}

template <class M>
void rrc_nr::ue::log_rrc_container(const direction_t       dir,
                                   srsran::const_byte_span pdu,
                                   const M&                msg,
                                   const char*             msg_type)
{
  fmt::memory_buffer strbuf;
  fmt::format_to(strbuf, "rnti=0x{:x}, container", rnti);
  parent->log_rrc_message(srsran::to_c_str(strbuf), Tx, pdu, msg, msg_type);
}

} // namespace srsenb
