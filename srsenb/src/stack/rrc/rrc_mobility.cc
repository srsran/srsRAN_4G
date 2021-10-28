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

#include "srsenb/hdr/stack/rrc/rrc_mobility.h"
#include "srsenb/hdr/stack/rrc/mac_controller.h"
#include "srsenb/hdr/stack/rrc/rrc_cell_cfg.h"
#include "srsenb/hdr/stack/rrc/ue_meas_cfg.h"
#include "srsenb/hdr/stack/rrc/ue_rr_cfg.h"
#include "srsran/asn1/rrc_utils.h"
#include "srsran/common/bcd_helpers.h"
#include "srsran/common/common.h"
#include "srsran/common/enb_events.h"
#include "srsran/common/int_helpers.h"
#include "srsran/common/standard_streams.h"
#include "srsran/interfaces/enb_mac_interfaces.h"
#include "srsran/interfaces/enb_pdcp_interfaces.h"
#include "srsran/interfaces/enb_rlc_interfaces.h"
#include "srsran/interfaces/enb_s1ap_interfaces.h"
#include "srsran/rrc/rrc_cfg_utils.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace srsenb {

#define Info(fmt, ...) logger.info("Mobility: " fmt, ##__VA_ARGS__)
#define Error(fmt, ...) logger.error("Mobility: " fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) logger.warning("Mobility: " fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) logger.debug("Mobility: " fmt, ##__VA_ARGS__)

#define procInfo(fmt, ...) parent->logger.info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procWarning(fmt, ...) parent->logger.warning("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procError(fmt, ...) parent->logger.error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

using namespace asn1::rrc;

/*************************************************************************************************
 *         Convenience Functions to handle ASN1 MeasObjs/MeasId/ReportCfg/Cells/etc.
 ************************************************************************************************/

namespace rrc_details {

//! extract cell id from ECI
uint32_t eci_to_cellid(uint32_t eci)
{
  return eci & 0xFFu;
}
//! extract enb id from ECI
uint32_t eci_to_enbid(uint32_t eci)
{
  return (eci - eci_to_cellid(eci)) >> 8u;
}
uint16_t compute_mac_i(uint16_t                            crnti,
                       uint32_t                            cellid,
                       uint16_t                            pci,
                       srsran::INTEGRITY_ALGORITHM_ID_ENUM integ_algo,
                       const uint8_t*                      k_rrc_int)
{
  static srslog::basic_logger& logger = srslog::fetch_basic_logger("RRC");
  // Compute shortMAC-I
  uint8_t varShortMAC_packed[16] = {};
  uint8_t mac_key[4]             = {};

  // ASN.1 encode VarShortMAC-Input
  asn1::rrc::var_short_mac_input_s var_short_mac;
  var_short_mac.cell_id.from_number(cellid);
  var_short_mac.pci = pci;
  var_short_mac.c_rnti.from_number(crnti);

  asn1::bit_ref bref(varShortMAC_packed, sizeof(varShortMAC_packed));
  if (var_short_mac.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) { // already zeroed, so no need to align
    printf("Error packing varShortMAC");
  }
  uint32_t N_bytes = bref.distance_bytes();

  logger.info("Encoded varShortMAC: cellId=0x%x, PCI=%d, rnti=0x%x (%d bytes)", cellid, pci, crnti, N_bytes);

  // Compute MAC-I
  switch (integ_algo) {
    case srsran::INTEGRITY_ALGORITHM_ID_EIA0:
      return 0;
    case srsran::INTEGRITY_ALGORITHM_ID_128_EIA1:
      srsran::security_128_eia1(&k_rrc_int[16],
                                0xffffffff, // 32-bit all to ones
                                0x1f,       // 5-bit all to ones
                                1,          // 1-bit to one
                                varShortMAC_packed,
                                N_bytes,
                                mac_key);
      break;
    case srsran::INTEGRITY_ALGORITHM_ID_128_EIA2:
      srsran::security_128_eia2(&k_rrc_int[16],
                                0xffffffff, // 32-bit all to ones
                                0x1f,       // 5-bit all to ones
                                1,          // 1-bit to one
                                varShortMAC_packed,
                                N_bytes,
                                mac_key);
      break;
    default:
      srsran::console_stderr("ERROR: Unsupported integrity algorithm %d.\n", integ_algo);
  }

  uint16_t short_mac_i = (((uint16_t)mac_key[2] << 8u) | (uint16_t)mac_key[3]);
  return short_mac_i;
}

//! convenience function overload to print MeasObj/MeasId/etc. fields
std::string to_string(const cells_to_add_mod_s& obj)
{
  char buf[128];
  std::snprintf(
      buf, 128, "{cell_idx: %d, pci: %d, offset: %d}", obj.cell_idx, obj.pci, obj.cell_individual_offset.to_number());
  return {buf};
}

} // namespace rrc_details

/*************************************************************************************************
 *                                  mobility_cfg class
 ************************************************************************************************/

/**
 * Description: Handover Request Handling
 *             - Allocation of RNTI
 *             - Apply HandoverPreparation container to created UE state
 *             - Apply target cell config to UE state
 *             - Preparation of HandoverCommand that goes inside the transparent container of HandoverRequestAck
 *             - Response from TeNB on whether it was able to allocate resources for user doing handover
 * @return rnti of created ue
 */
uint16_t rrc::start_ho_ue_resource_alloc(const asn1::s1ap::ho_request_s&                                   msg,
                                         const asn1::s1ap::sourceenb_to_targetenb_transparent_container_s& container,
                                         asn1::s1ap::cause_c&                                              cause)
{
  // TODO: Decision Making on whether the same QoS of the source eNB can be provided by target eNB

  /* Evaluate if cell exists */
  uint32_t               target_eci  = container.target_cell_id.cell_id.to_number();
  const enb_cell_common* target_cell = cell_common_list->get_cell_id(rrc_details::eci_to_cellid(target_eci));
  if (target_cell == nullptr) {
    logger.error("The S1-handover target cell_id=0x%x does not exist", rrc_details::eci_to_cellid(target_eci));
    cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::cell_not_available;
    return SRSRAN_INVALID_RNTI;
  }

  /* Create new User */

  // Allocate C-RNTI in MAC
  sched_interface::ue_cfg_t ue_cfg = {};
  ue_cfg.supported_cc_list.resize(1);
  ue_cfg.supported_cc_list[0].active     = true;
  ue_cfg.supported_cc_list[0].enb_cc_idx = target_cell->enb_cc_idx;
  ue_cfg.ue_bearers[0].direction         = mac_lc_ch_cfg_t::BOTH;
  ue_cfg.supported_cc_list[0].dl_cfg.tm  = SRSRAN_TM1;
  uint16_t rnti                          = mac->reserve_new_crnti(ue_cfg);
  if (rnti == SRSRAN_INVALID_RNTI) {
    logger.error("Failed to allocate C-RNTI resources");
    cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::no_radio_res_available_in_target_cell;
    return SRSRAN_INVALID_RNTI;
  }

  // Register new user in RRC
  if (add_user(rnti, ue_cfg) != SRSRAN_SUCCESS) {
    logger.error("Failed to create user");
    cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::no_radio_res_available_in_target_cell;
    return SRSRAN_INVALID_RNTI;
  }
  auto it     = users.find(rnti);
  ue*  ue_ptr = it->second.get();
  if (not ue_ptr->init_pucch()) {
    rem_user(rnti);
    logger.warning("Failed to allocate PUCCH resources for rnti=0x%x", rnti);
    cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::no_radio_res_available_in_target_cell;
    return SRSRAN_INVALID_RNTI;
  }

  // Reset activity timer (Response is not expected)
  ue_ptr->set_activity_timeout(ue::UE_INACTIVITY_TIMEOUT);
  ue_ptr->set_activity(false);

  //  /* Setup e-RABs & DRBs / establish an UL/DL S1 bearer to the S-GW */
  //  if (not setup_ue_erabs(rnti, msg)) {
  //    rrc_ptr->logger.error("Failed to setup e-RABs for rnti=0x%x", );
  //  }

  if (not ue_ptr->mobility_handler->start_s1_tenb_ho(msg, container, cause)) {
    rem_user(rnti);
    return SRSRAN_INVALID_RNTI;
  }
  return rnti;
}

/*************************************************************************************************
 *                                  rrc_mobility class
 ************************************************************************************************/

rrc::ue::rrc_mobility::rrc_mobility(rrc::ue* outer_ue) :
  base_t(outer_ue->parent->logger), rrc_ue(outer_ue), rrc_enb(outer_ue->parent), logger(outer_ue->parent->logger)
{}

//! Method to add Mobility Info to a RRC Connection Reconfiguration Message
bool rrc::ue::rrc_mobility::fill_conn_recfg_no_ho_cmd(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_recfg)
{
  // only reconfigure meas_cfg if no handover is occurring.
  // NOTE: We basically freeze ue_var_meas for the whole duration of the handover procedure
  if (is_ho_running()) {
    return false;
  }

  // Check if there has been any update in ue_var_meas based on UE current cell list
  conn_recfg->meas_cfg_present =
      apply_meascfg_updates(conn_recfg->meas_cfg, rrc_ue->current_ue_cfg.meas_cfg, rrc_ue->ue_cell_list);
  return conn_recfg->meas_cfg_present;
}

//! Method called whenever the eNB receives a MeasReport from the UE. In normal situations, an HO procedure is started
void rrc::ue::rrc_mobility::handle_ue_meas_report(const meas_report_s& msg, srsran::unique_byte_buffer_t pdu)
{
  asn1::json_writer json_writer;
  msg.to_json(json_writer);
  event_logger::get().log_measurement_report(
      rrc_ue->ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
      asn1::octstring_to_string(pdu->msg, pdu->N_bytes),
      json_writer.to_string(),
      rrc_ue->rnti);

  if (not is_in_state<idle_st>()) {
    Info("Received a MeasReport while UE is performing Handover. Ignoring...");
    return;
  }
  // Check if meas_id is valid
  const meas_results_s& meas_res = msg.crit_exts.c1().meas_report_r8().meas_results;
  if (not meas_res.meas_result_neigh_cells_present) {
    Debug("Received a MeasReport, but the UE did not detect any cell.");
    return;
  }
  if (meas_res.meas_result_neigh_cells.type().value !=
      meas_results_s::meas_result_neigh_cells_c_::types::meas_result_list_eutra) {
    Debug("Skipping non-EUTRA MeasReport.");
    return;
  }
  const meas_id_list&  measid_list  = rrc_ue->current_ue_cfg.meas_cfg.meas_id_to_add_mod_list;
  const meas_obj_list& measobj_list = rrc_ue->current_ue_cfg.meas_cfg.meas_obj_to_add_mod_list;
  auto                 measid_it    = srsran::find_rrc_obj_id(measid_list, meas_res.meas_id);
  if (measid_it == measid_list.end()) {
    Warning("The measurement ID %d provided by the UE does not exist.", meas_res.meas_id);
    return;
  }
  const meas_result_list_eutra_l& eutra_report_list = meas_res.meas_result_neigh_cells.meas_result_list_eutra();

  // Find respective ReportCfg and MeasObj
  ho_meas_report_ev meas_ev{};
  auto              obj_it = srsran::find_rrc_obj_id(measobj_list, measid_it->meas_obj_id);
  meas_ev.meas_obj         = &(*obj_it);

  // iterate from strongest to weakest cell
  const ue_cell_ded* pcell         = rrc_ue->ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
  const auto&        meas_list_cfg = pcell->cell_common->cell_cfg.meas_cfg.meas_cells;
  for (const meas_result_eutra_s& e : eutra_report_list) {
    auto                   same_pci = [&e](const meas_cell_cfg_t& c) { return c.pci == e.pci; };
    auto                   meas_it  = std::find_if(meas_list_cfg.begin(), meas_list_cfg.end(), same_pci);
    const enb_cell_common* c        = rrc_enb->cell_common_list->get_pci(e.pci);
    if (meas_it != meas_list_cfg.end()) {
      meas_ev.target_eci      = meas_it->eci;
      meas_ev.direct_fwd_path = meas_it->direct_forward_path_available;
    } else if (c != nullptr) {
      meas_ev.target_eci = (rrc_enb->cfg.enb_id << 8u) + c->cell_cfg.cell_id;
    } else {
      logger.warning("The PCI=%d inside the MeasReport is not recognized.", e.pci);
      continue;
    }

    // eNB found the respective cell. eNB takes "HO Decision"
    // NOTE: From now we just choose the strongest.
    if (trigger(meas_ev)) {
      break;
    }
  }
}

/**
 * Description: Send "HO Required" message from source eNB to MME
 *              - 1st Message of the handover preparation phase
 *              - The RRC stores info regarding the source eNB configuration in a HO Preparation Info struct
 *              - This struct goes in a transparent container to the S1AP
 */
bool rrc::ue::rrc_mobility::start_ho_preparation(uint32_t target_eci,
                                                 uint8_t  measobj_id,
                                                 bool     fwd_direct_path_available)
{
  srsran::plmn_id_t target_plmn =
      srsran::make_plmn_id_t(rrc_enb->cfg.sib1.cell_access_related_info.plmn_id_list[0].plmn_id);
  const ue_cell_ded*     src_cell_ded = rrc_ue->ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
  const enb_cell_common* src_cell_cfg = src_cell_ded->cell_common;

  /*** Fill HO Preparation Info ***/
  asn1::rrc::ho_prep_info_s         hoprep;
  asn1::rrc::ho_prep_info_r8_ies_s& hoprep_r8 = hoprep.crit_exts.set_c1().set_ho_prep_info_r8();
  if (not rrc_ue->eutra_capabilities_unpacked) {
    // TODO: temporary. Made up something to please target eNB. (there must be at least one capability in this packet)
    hoprep_r8.ue_radio_access_cap_info.resize(1);
    hoprep_r8.ue_radio_access_cap_info[0].rat_type = asn1::rrc::rat_type_e::eutra;
    asn1::rrc::ue_eutra_cap_s capitem;
    capitem.access_stratum_release                            = asn1::rrc::access_stratum_release_e::rel8;
    capitem.ue_category                                       = 4;
    capitem.pdcp_params.max_num_rohc_context_sessions_present = true;
    capitem.pdcp_params.max_num_rohc_context_sessions = asn1::rrc::pdcp_params_s::max_num_rohc_context_sessions_e_::cs2;
    bzero(&capitem.pdcp_params.supported_rohc_profiles,
          sizeof(asn1::rrc::rohc_profile_support_list_r15_s)); // TODO: why is it r15?
    capitem.phy_layer_params.ue_specific_ref_sigs_supported = false;
    capitem.phy_layer_params.ue_tx_ant_sel_supported        = false;
    capitem.rf_params.supported_band_list_eutra.resize(1);
    capitem.rf_params.supported_band_list_eutra[0].band_eutra  = 7;
    capitem.rf_params.supported_band_list_eutra[0].half_duplex = false;
    capitem.meas_params.band_list_eutra.resize(1);
    capitem.meas_params.band_list_eutra[0].inter_rat_band_list_present = false;
    capitem.meas_params.band_list_eutra[0].inter_freq_band_list.resize(1);
    capitem.meas_params.band_list_eutra[0].inter_freq_band_list[0].inter_freq_need_for_gaps = false;
    capitem.feature_group_inds_present                                                      = true;
    capitem.feature_group_inds.from_number(0xe6041000); // 0x5d0ffc80); // 0xe6041c00;
    {
      uint8_t       buffer[128];
      asn1::bit_ref bref(&buffer[0], sizeof(buffer));
      if (capitem.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
        logger.error("Failed to pack UE EUTRA Capability");
      }
      hoprep_r8.ue_radio_access_cap_info[0].ue_cap_rat_container.resize((uint32_t)bref.distance_bytes());
      memcpy(&hoprep_r8.ue_radio_access_cap_info[0].ue_cap_rat_container[0], &buffer[0], bref.distance_bytes());
    }
    Debug("UE RA Category: %d", capitem.ue_category);
  } else {
    hoprep_r8.ue_radio_access_cap_info.resize(1);
    hoprep_r8.ue_radio_access_cap_info[0].rat_type = asn1::rrc::rat_type_e::eutra;

    srsran::unique_byte_buffer_t buffer = srsran::make_byte_buffer();
    if (buffer == nullptr) {
      logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return false;
    }
    asn1::bit_ref bref(buffer->msg, buffer->get_tailroom());
    if (rrc_ue->eutra_capabilities.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
      logger.error("Failed to pack UE EUTRA Capability");
      return false;
    }
    hoprep_r8.ue_radio_access_cap_info[0].ue_cap_rat_container.resize(bref.distance_bytes());
    memcpy(&hoprep_r8.ue_radio_access_cap_info[0].ue_cap_rat_container[0], buffer->msg, bref.distance_bytes());
  }
  /*** fill AS-Config ***/
  hoprep_r8.as_cfg_present       = true;
  hoprep_r8.as_cfg.source_rr_cfg = rrc_ue->current_ue_cfg.rr_cfg;
  hoprep_r8.as_cfg.source_scell_cfg_list_r10.reset(new scell_to_add_mod_list_r10_l{rrc_ue->current_ue_cfg.scells});
  hoprep_r8.as_cfg.source_meas_cfg = rrc_ue->current_ue_cfg.meas_cfg;
  // Get security cfg
  hoprep_r8.as_cfg.source_security_algorithm_cfg = rrc_ue->ue_security_cfg.get_security_algorithm_cfg();
  hoprep_r8.as_cfg.source_ue_id.from_number(rrc_ue->rnti);
  asn1::number_to_enum(hoprep_r8.as_cfg.source_mib.dl_bw, rrc_enb->cfg.cell.nof_prb);
  hoprep_r8.as_cfg.source_mib.phich_cfg.phich_dur.value =
      (asn1::rrc::phich_cfg_s::phich_dur_e_::options)rrc_enb->cfg.cell.phich_length;
  hoprep_r8.as_cfg.source_mib.phich_cfg.phich_res.value =
      (asn1::rrc::phich_cfg_s::phich_res_e_::options)rrc_enb->cfg.cell.phich_resources;
  hoprep_r8.as_cfg.source_mib.sys_frame_num.from_number(0); // NOTE: The TS says this can go empty
  hoprep_r8.as_cfg.source_sib_type1 = src_cell_cfg->sib1;
  hoprep_r8.as_cfg.source_sib_type2 = src_cell_cfg->sib2;
  asn1::number_to_enum(hoprep_r8.as_cfg.ant_info_common.ant_ports_count, rrc_enb->cfg.cell.nof_ports);
  hoprep_r8.as_cfg.source_dl_carrier_freq = src_cell_cfg->cell_cfg.dl_earfcn;
  // - fill as_context
  hoprep_r8.as_context_present               = true;
  hoprep_r8.as_context.reest_info_present    = true;
  hoprep_r8.as_context.reest_info.source_pci = src_cell_cfg->cell_cfg.pci;
  hoprep_r8.as_context.reest_info.target_cell_short_mac_i.from_number(
      rrc_details::compute_mac_i(rrc_ue->rnti,
                                 src_cell_cfg->sib1.cell_access_related_info.cell_id.to_number(),
                                 src_cell_cfg->cell_cfg.pci,
                                 rrc_ue->ue_security_cfg.get_as_sec_cfg().integ_algo,
                                 rrc_ue->ue_security_cfg.get_as_sec_cfg().k_rrc_int.data()));

  /*** pack HO Preparation Info into an RRC container buffer ***/
  srsran::unique_byte_buffer_t buffer = srsran::make_byte_buffer();
  if (buffer == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return false;
  }
  asn1::bit_ref bref(buffer->msg, buffer->get_tailroom());
  if (hoprep.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
    Error("Failed to pack HO preparation msg");
    return false;
  }
  buffer->N_bytes = bref.distance_bytes();

  // Set list of E-RABs for DL forwarding
  std::vector<uint32_t> fwd_erabs;
  fwd_erabs.reserve(rrc_ue->bearer_list.get_erabs().size());
  for (auto& erab_pair : rrc_ue->bearer_list.get_erabs()) {
    fwd_erabs.push_back(erab_pair.first);
  }

  return rrc_enb->s1ap->send_ho_required(
      rrc_ue->rnti, target_eci, target_plmn, fwd_erabs, std::move(buffer), fwd_direct_path_available);
}

/**
 * Description: Handover Preparation Complete (with success or failure)
 *             - MME --> SeNB
 *             - Response from MME on whether the HandoverRequired command is valid and the TeNB was able to allocate
 *               space for the UE
 * @param is_success flag to whether an HandoverCommand or HandoverReject was received
 * @param container RRC container with HandoverCommand to send to UE
 */
void rrc::ue::rrc_mobility::handle_ho_preparation_complete(rrc::ho_prep_result          result,
                                                           const asn1::s1ap::ho_cmd_s&  msg,
                                                           srsran::unique_byte_buffer_t container)
{
  if (result == rrc_interface_s1ap::ho_prep_result::failure) {
    logger.info("Received S1AP HandoverFailure. Aborting Handover...");
    trigger(srsran::failure_ev{});
    return;
  }
  if (result == rrc_interface_s1ap::ho_prep_result::timeout) {
    asn1::s1ap::cause_c cause;
    cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::ts1relocprep_expiry;
    trigger(ho_cancel_ev{cause});
    return;
  }

  // Check if any E-RAB that was not admitted. Cancel Handover, in such case.
  if (msg.protocol_ies.erab_to_release_list_ho_cmd_present) {
    get_logger().warning("E-RAB id=%d was not admitted in target eNB. Cancelling handover...",
                         msg.protocol_ies.erab_to_release_list_ho_cmd.value[0].value.erab_item().erab_id);
    asn1::s1ap::cause_c cause;
    cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::no_radio_res_available_in_target_cell;
    trigger(ho_cancel_ev{cause});
  }

  /* unpack RRC HOCmd struct and perform sanity checks */
  asn1::rrc::ho_cmd_s rrchocmd;
  {
    asn1::cbit_ref bref(container->msg, container->N_bytes);
    if (rrchocmd.unpack(bref) != asn1::SRSASN_SUCCESS) {
      get_logger().warning("Unpacking of RRC HOCommand was unsuccessful");
      get_logger().warning(container->msg, container->N_bytes, "Received container:");
      asn1::s1ap::cause_c cause;
      cause.set_protocol().value = asn1::s1ap::cause_protocol_opts::transfer_syntax_error;
      trigger(ho_cancel_ev{cause});
      return;
    }
  }
  if (rrchocmd.crit_exts.type().value != c1_or_crit_ext_opts::c1 or
      rrchocmd.crit_exts.c1().type().value != ho_cmd_s::crit_exts_c_::c1_c_::types_opts::ho_cmd_r8) {
    get_logger().warning("Only handling r8 Handover Commands");
    asn1::s1ap::cause_c cause;
    cause.set_protocol().value = asn1::s1ap::cause_protocol_opts::semantic_error;
    trigger(ho_cancel_ev{cause});
    return;
  }

  trigger(s1_source_ho_st::ho_cmd_msg{&msg, &rrchocmd.crit_exts.c1().ho_cmd_r8()});
}

bool rrc::ue::rrc_mobility::start_s1_tenb_ho(
    const asn1::s1ap::ho_request_s&                                   msg,
    const asn1::s1ap::sourceenb_to_targetenb_transparent_container_s& container,
    asn1::s1ap::cause_c&                                              cause)
{
  trigger(ho_req_rx_ev{&msg, &container});
  if (not is_in_state<s1_target_ho_st>()) {
    cause = failure_cause;
    return false;
  }
  return true;
}

/**
 * @brief Fills RRCConnectionReconfigurationMessage with Handover Command fields that are common to
 *        all types of handover (e.g. S1, intra-enb, X2), namely:
 *        - mobilityControlInformation
 *        - SecurityConfigHandover
 *        - RadioReconfiguration.PhyConfig
 *          - Scheduling Request setup
 *          - CQI report cfg
 *          - AntennaConfig
 * @param msg
 * @param target_cell
 */
void rrc::ue::rrc_mobility::fill_mobility_reconf_common(asn1::rrc::dl_dcch_msg_s& msg,
                                                        const enb_cell_common&    target_cell,
                                                        uint32_t                  src_dl_earfcn,
                                                        uint32_t                  src_pci)
{
  auto& recfg              = msg.msg.set_c1().set_rrc_conn_recfg();
  recfg.rrc_transaction_id = rrc_ue->transaction_id;
  rrc_ue->transaction_id   = (rrc_ue->transaction_id + 1) % 4;
  auto& recfg_r8           = recfg.crit_exts.set_c1().set_rrc_conn_recfg_r8();

  // Pack MobilityControlInfo message with params of target Cell
  recfg_r8.mob_ctrl_info_present = true;
  auto& mob_info                 = recfg_r8.mob_ctrl_info;
  mob_info.target_pci            = target_cell.cell_cfg.pci;
  mob_info.t304                  = target_cell.cell_cfg.t304;
  mob_info.new_ue_id.from_number(rrc_ue->rnti);

  mob_info.rr_cfg_common.rach_cfg_common_present    = true;
  mob_info.rr_cfg_common.rach_cfg_common            = target_cell.sib2.rr_cfg_common.rach_cfg_common;
  mob_info.rr_cfg_common.prach_cfg.root_seq_idx     = target_cell.sib2.rr_cfg_common.prach_cfg.root_seq_idx;
  mob_info.rr_cfg_common.pdsch_cfg_common_present   = true;
  mob_info.rr_cfg_common.pdsch_cfg_common           = target_cell.sib2.rr_cfg_common.pdsch_cfg_common;
  mob_info.rr_cfg_common.pusch_cfg_common           = target_cell.sib2.rr_cfg_common.pusch_cfg_common;
  mob_info.rr_cfg_common.pucch_cfg_common_present   = true;
  mob_info.rr_cfg_common.pucch_cfg_common           = target_cell.sib2.rr_cfg_common.pucch_cfg_common;
  mob_info.rr_cfg_common.srs_ul_cfg_common_present  = true;
  mob_info.rr_cfg_common.srs_ul_cfg_common          = target_cell.sib2.rr_cfg_common.srs_ul_cfg_common;
  mob_info.rr_cfg_common.ul_pwr_ctrl_common_present = true;
  mob_info.rr_cfg_common.ul_pwr_ctrl_common         = target_cell.sib2.rr_cfg_common.ul_pwr_ctrl_common;
  mob_info.rr_cfg_common.p_max_present              = true;
  mob_info.rr_cfg_common.p_max                      = rrc_enb->cfg.sib1.p_max;
  mob_info.rr_cfg_common.ul_cp_len                  = target_cell.sib2.rr_cfg_common.ul_cp_len;

  mob_info.carrier_freq_present = false; // same frequency handover for now
  asn1::number_to_enum(mob_info.carrier_bw.dl_bw, target_cell.mib.dl_bw.to_number());
  if (target_cell.cell_cfg.dl_earfcn != src_dl_earfcn) {
    mob_info.carrier_freq_present         = true;
    mob_info.carrier_freq.dl_carrier_freq = target_cell.cell_cfg.dl_earfcn;
  }

  // Set security cfg
  recfg_r8.security_cfg_ho_present        = true;
  auto& intralte                          = recfg_r8.security_cfg_ho.handov_type.set_intra_lte();
  intralte.security_algorithm_cfg_present = false;
  intralte.key_change_ind                 = false;
  intralte.next_hop_chaining_count        = rrc_ue->ue_security_cfg.get_ncc();

  // Add MeasConfig of target cell
  recfg_r8.meas_cfg_present = apply_meascfg_updates(
      recfg_r8.meas_cfg, rrc_ue->current_ue_cfg.meas_cfg, rrc_ue->ue_cell_list, src_dl_earfcn, src_pci);

  apply_reconf_updates(recfg_r8,
                       rrc_ue->current_ue_cfg,
                       rrc_enb->cfg,
                       rrc_ue->ue_cell_list,
                       rrc_ue->bearer_list,
                       rrc_ue->ue_capabilities,
                       true);
}

/*************************************
 *     rrc_mobility FSM methods
 *************************************/

bool rrc::ue::rrc_mobility::needs_s1_ho(idle_st& s, const ho_meas_report_ev& meas_result)
{
  if (rrc_ue->get_state() != RRC_STATE_REGISTERED) {
    return false;
  }
  return rrc_details::eci_to_enbid(meas_result.target_eci) != rrc_enb->cfg.enb_id;
}

bool rrc::ue::rrc_mobility::needs_intraenb_ho(idle_st& s, const ho_meas_report_ev& meas_result)
{
  if (rrc_ue->get_state() != RRC_STATE_REGISTERED) {
    return false;
  }
  if (rrc_details::eci_to_enbid(meas_result.target_eci) != rrc_enb->cfg.enb_id) {
    return false;
  }
  uint32_t cell_id = rrc_details::eci_to_cellid(meas_result.target_eci);
  return rrc_ue->get_ue_cc_cfg(UE_PCELL_CC_IDX)->cell_cfg.cell_id != cell_id;
}

/*************************************
 *   s1_source_ho subFSM methods
 *************************************/

rrc::ue::rrc_mobility::s1_source_ho_st::s1_source_ho_st(rrc_mobility* parent_) :
  base_t(parent_), rrc_enb(parent_->rrc_enb), rrc_ue(parent_->rrc_ue), logger(parent_->logger)
{}

/**
 * TS 36.413, Section 8.4.6 - eNB Status Transfer
 * @brief: Send "eNBStatusTransfer" message from source eNB to MME, and setup Forwarding GTPU tunnel
 *         - PDCP provides the bearers' DL/UL HFN and COUNT to be put inside a transparent container
 *         - The eNB sends eNBStatusTransfer to MME
 *         - A GTPU forwarding tunnel is opened to forward buffered PDCP PDUs and incoming GTPU PDUs
 */
asn1::s1ap::cause_c
rrc::ue::rrc_mobility::s1_source_ho_st::start_enb_status_transfer(const asn1::s1ap::ho_cmd_s& s1ap_ho_cmd)
{
  asn1::s1ap::cause_c                                 cause;
  std::vector<s1ap_interface_rrc::bearer_status_info> s1ap_bearers;
  s1ap_bearers.reserve(rrc_ue->bearer_list.get_erabs().size());

  for (const auto& erab_pair : rrc_ue->bearer_list.get_erabs()) {
    s1ap_interface_rrc::bearer_status_info b    = {};
    uint8_t                                lcid = erab_pair.second.lcid;
    b.erab_id                                   = erab_pair.second.id;
    srsran::pdcp_lte_state_t pdcp_state         = {};
    if (not rrc_enb->pdcp->get_bearer_state(rrc_ue->rnti, lcid, &pdcp_state)) {
      Error("PDCP bearer lcid=%d for rnti=0x%x was not found", lcid, rrc_ue->rnti);
      cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::unknown_erab_id;
      return cause;
    }
    b.dl_hfn     = pdcp_state.tx_hfn;
    b.pdcp_dl_sn = pdcp_state.next_pdcp_tx_sn;
    b.ul_hfn     = pdcp_state.rx_hfn;
    b.pdcp_ul_sn = pdcp_state.next_pdcp_rx_sn;
    s1ap_bearers.push_back(b);
  }

  Info("PDCP Bearer list sent to S1AP to initiate the eNB Status Transfer");
  if (not rrc_enb->s1ap->send_enb_status_transfer_proc(rrc_ue->rnti, s1ap_bearers)) {
    cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::unknown_erab_id;
    return cause;
  }

  // Setup GTPU forwarding tunnel
  if (s1ap_ho_cmd.protocol_ies.erab_subjectto_data_forwarding_list_present) {
    const auto& fwd_erab_list = s1ap_ho_cmd.protocol_ies.erab_subjectto_data_forwarding_list.value;
    const auto& erab_list     = rrc_ue->bearer_list.get_erabs();
    for (const auto& e : fwd_erab_list) {
      const auto& fwd_erab = e.value.erab_data_forwarding_item();
      auto        it       = erab_list.find(fwd_erab.erab_id);
      if (it == erab_list.end()) {
        Warning("E-RAB id=%d subject to forwarding not found\n", fwd_erab.erab_id);
        continue;
      }
      const bearer_cfg_handler::erab_t& erab = it->second;
      if (fwd_erab.dl_g_tp_teid_present and fwd_erab.dl_transport_layer_address_present) {
        gtpu_interface_rrc::bearer_props props;
        props.forward_from_teidin_present = true;
        props.forward_from_teidin         = erab.teid_in;
        rrc_ue->bearer_list.add_gtpu_bearer(fwd_erab.erab_id,
                                            fwd_erab.dl_g_tp_teid.to_number(),
                                            fwd_erab.dl_transport_layer_address.to_number(),
                                            &props);
      }
    }
  }

  return cause;
}

void rrc::ue::rrc_mobility::s1_source_ho_st::enter(rrc_mobility* f, const ho_meas_report_ev& ev)
{
  srsran::console("Starting S1 Handover of rnti=0x%x to cellid=0x%x.\n", rrc_ue->rnti, ev.target_eci);
  logger.info("Starting S1 Handover of rnti=0x%x to cellid=0x%x.", rrc_ue->rnti, ev.target_eci);
  report = ev;

  if (not parent_fsm()->start_ho_preparation(report.target_eci, report.meas_obj->meas_obj_id, ev.direct_fwd_path)) {
    trigger(srsran::failure_ev{});
  }
}

/**
 * TS 36.413, Section 8.4.2 - Handover Resource Allocation
 * @brief: Called in SeNB when "Handover Command" is received
 *         Send "eNBStatusTransfer" message from source eNB to MME, and setup Forwarding GTPU tunnel
 *         - PDCP provides the bearers' DL/UL HFN and COUNT to be put inside a transparent container
 *         - The eNB sends eNBStatusTransfer to MME
 *         - A GTPU forwarding tunnel is opened to forward buffered PDCP PDUs and incoming GTPU PDUs
 */
void rrc::ue::rrc_mobility::s1_source_ho_st::handle_ho_cmd(wait_ho_cmd& s, const ho_cmd_msg& ho_cmd)
{
  /* unpack DL-DCCH message containing the RRCRonnectionReconf (with MobilityInfo) to be sent to the UE */
  asn1::rrc::dl_dcch_msg_s dl_dcch_msg;
  {
    asn1::cbit_ref bref(&ho_cmd.ho_cmd->ho_cmd_msg[0], ho_cmd.ho_cmd->ho_cmd_msg.size());
    if (dl_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS) {
      Warning("Unpacking of RRC DL-DCCH message with HO Command was unsuccessful.");
      asn1::s1ap::cause_c cause;
      cause.set_protocol().value = asn1::s1ap::cause_protocol_opts::transfer_syntax_error;
      trigger(ho_cancel_ev{cause});
      return;
    }
  }
  if (dl_dcch_msg.msg.type().value != dl_dcch_msg_type_c::types_opts::c1 or
      dl_dcch_msg.msg.c1().type().value != dl_dcch_msg_type_c::c1_c_::types_opts::rrc_conn_recfg) {
    Warning("HandoverCommand is expected to contain an RRC Connection Reconf message inside");
    asn1::s1ap::cause_c cause;
    cause.set_protocol().value = asn1::s1ap::cause_protocol_opts::semantic_error;
    trigger(ho_cancel_ev{cause});
    return;
  }
  asn1::rrc::rrc_conn_recfg_s& reconf = dl_dcch_msg.msg.c1().rrc_conn_recfg();
  if (not reconf.crit_exts.c1().rrc_conn_recfg_r8().mob_ctrl_info_present) {
    Warning("HandoverCommand is expected to have mobility control subfield");
    asn1::s1ap::cause_c cause;
    cause.set_protocol().value = asn1::s1ap::cause_protocol_opts::semantic_error;
    trigger(ho_cancel_ev{cause});
    return;
  }

  /* Enter Handover Execution */

  // Disable DRBs in the MAC and PDCP, while Reconfiguration is taking place.
  for (const drb_to_add_mod_s& drb : rrc_ue->bearer_list.get_established_drbs()) {
    rrc_ue->parent->pdcp->set_enabled(rrc_ue->rnti, drb_to_lcid((lte_drb)drb.drb_id), false);
  }
  rrc_ue->mac_ctrl.set_drb_activation(false);
  rrc_ue->mac_ctrl.update_mac();

  // Send HO Command to UE
  std::string octet_str;
  if (not rrc_ue->send_dl_dcch(&dl_dcch_msg, nullptr, &octet_str)) {
    asn1::s1ap::cause_c cause;
    cause.set_protocol().value = asn1::s1ap::cause_protocol_opts::unspecified;
    trigger(ho_cancel_ev{cause});
    return;
  }

  // Log rrc release event.
  asn1::json_writer json_writer;
  dl_dcch_msg.to_json(json_writer);
  event_logger::get().log_rrc_event(rrc_ue->ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
                                    octet_str,
                                    json_writer.to_string(),
                                    static_cast<unsigned>(rrc_event_type::con_reconf),
                                    static_cast<unsigned>(procedure_result_code::none),
                                    rrc_ue->rnti);

  // Log HO command.
  event_logger::get().log_handover_command(
      rrc_ue->ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
      reconf.crit_exts.c1().rrc_conn_recfg_r8().mob_ctrl_info.target_pci,
      reconf.crit_exts.c1().rrc_conn_recfg_r8().mob_ctrl_info.carrier_freq.dl_carrier_freq,
      reconf.crit_exts.c1().rrc_conn_recfg_r8().mob_ctrl_info.new_ue_id.to_number(),
      rrc_ue->rnti);

  /* Start S1AP eNBStatusTransfer Procedure */
  asn1::s1ap::cause_c cause = start_enb_status_transfer(*ho_cmd.s1ap_ho_cmd);
  if (cause.type().value != asn1::s1ap::cause_c::types_opts::nulltype) {
    trigger(ho_cancel_ev{cause});
  }
}

//! Called in Source ENB during S1-Handover when there was a Reestablishment Request
void rrc::ue::rrc_mobility::s1_source_ho_st::handle_ho_cancel(const ho_cancel_ev& ev)
{
  rrc_enb->s1ap->send_ho_cancel(rrc_ue->rnti, ev.cause);
}

/*************************************
 *   s1_target_ho state methods
 *************************************/

/**
 * @brief handle S1AP "HO Requested" message from the MME
 *        - MME --> TeNB
 * @param s initial state
 * @param ho_req event with received message
 */
void rrc::ue::rrc_mobility::handle_ho_requested(idle_st& s, const ho_req_rx_ev& ho_req)
{
  asn1::s1ap::cause_c                  cause; // in case of failure
  const auto&                          rrc_container = ho_req.transparent_container->rrc_container;
  std::vector<asn1::s1ap::erab_item_s> not_admitted_erabs;
  auto&                                fwd_tunnels = get_state<s1_target_ho_st>()->pending_tunnels;
  fwd_tunnels.clear();

  /* TS 36.331 10.2.2. - Decode HandoverPreparationInformation */
  asn1::cbit_ref            bref{rrc_container.data(), rrc_container.size()};
  asn1::rrc::ho_prep_info_s hoprep;
  if (hoprep.unpack(bref) != asn1::SRSASN_SUCCESS) {
    rrc_enb->logger.error("Failed to decode HandoverPreparationinformation in S1AP SourceENBToTargetENBContainer");
    cause.set_protocol().value = asn1::s1ap::cause_protocol_opts::transfer_syntax_error;
    trigger(ho_failure_ev{cause});
    return;
  }
  if (hoprep.crit_exts.type().value != c1_or_crit_ext_opts::c1 or
      hoprep.crit_exts.c1().type().value != ho_prep_info_s::crit_exts_c_::c1_c_::types_opts::ho_prep_info_r8) {
    rrc_enb->logger.error("Only release 8 supported");
    cause.set_protocol().value = asn1::s1ap::cause_protocol_opts::semantic_error;
    trigger(ho_failure_ev{cause});
    return;
  }
  rrc_enb->log_rrc_message(direction_t::fromS1AP, rrc_ue->rnti, -1, rrc_container, hoprep, "HandoverPreparation");

  /* Setup UE current state in TeNB based on HandoverPreparation message */
  const ho_prep_info_r8_ies_s& hoprep_r8 = hoprep.crit_exts.c1().ho_prep_info_r8();
  if (not apply_ho_prep_cfg(hoprep_r8, *ho_req.ho_req_msg, not_admitted_erabs, cause)) {
    trigger(ho_failure_ev{cause});
    return;
  }

  /* Prepare Handover Request Acknowledgment - Handover Command */
  dl_dcch_msg_s      dl_dcch_msg;
  const ue_cell_ded* target_cell = rrc_ue->ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX);

  // Fill fields common to all types of handover (e.g. new CQI/SR configuration, mobControlInfo)
  fill_mobility_reconf_common(dl_dcch_msg,
                              *target_cell->cell_common,
                              hoprep_r8.as_cfg.source_dl_carrier_freq,
                              hoprep_r8.as_context.reest_info.source_pci);
  rrc_conn_recfg_r8_ies_s& recfg_r8 = dl_dcch_msg.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();

  // Apply new Security Config based on HandoverRequest
  // See TS 33.401, Sec. 7.2.8.4.3
  recfg_r8.security_cfg_ho_present = true;
  recfg_r8.security_cfg_ho.handov_type.set(security_cfg_ho_s::handov_type_c_::types_opts::intra_lte);
  recfg_r8.security_cfg_ho.handov_type.intra_lte().security_algorithm_cfg_present = true;
  recfg_r8.security_cfg_ho.handov_type.intra_lte().security_algorithm_cfg =
      rrc_ue->ue_security_cfg.get_security_algorithm_cfg();
  recfg_r8.security_cfg_ho.handov_type.intra_lte().key_change_ind = false;
  recfg_r8.security_cfg_ho.handov_type.intra_lte().next_hop_chaining_count =
      ho_req.ho_req_msg->protocol_ies.security_context.value.next_hop_chaining_count;

  /* Prepare Handover Command to be sent via S1AP */
  srsran::unique_byte_buffer_t ho_cmd_pdu = srsran::make_byte_buffer();
  if (ho_cmd_pdu == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::unspecified;
    trigger(ho_failure_ev{cause});
    return;
  }
  asn1::bit_ref bref2{ho_cmd_pdu->msg, ho_cmd_pdu->get_tailroom()};
  if (dl_dcch_msg.pack(bref2) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack HandoverCommand");
    cause.set_protocol().value = asn1::s1ap::cause_protocol_opts::transfer_syntax_error;
    trigger(ho_failure_ev{cause});
    return;
  }
  ho_cmd_pdu->N_bytes = bref2.distance_bytes();
  rrc_enb->log_rrc_message(direction_t::toS1AP, rrc_ue->rnti, -1, *ho_cmd_pdu, dl_dcch_msg, "HandoverCommand");

  asn1::rrc::ho_cmd_s         ho_cmd;
  asn1::rrc::ho_cmd_r8_ies_s& ho_cmd_r8 = ho_cmd.crit_exts.set_c1().set_ho_cmd_r8();
  ho_cmd_r8.ho_cmd_msg.resize(bref2.distance_bytes());
  memcpy(ho_cmd_r8.ho_cmd_msg.data(), ho_cmd_pdu->msg, bref2.distance_bytes());
  bref2 = {ho_cmd_pdu->msg, ho_cmd_pdu->get_tailroom()};
  if (ho_cmd.pack(bref2) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack HandoverCommand");
    cause.set_protocol().value = asn1::s1ap::cause_protocol_opts::transfer_syntax_error;
    trigger(ho_failure_ev{cause});
    return;
  }
  ho_cmd_pdu->N_bytes = bref2.distance_bytes();

  /* Configure remaining layers based on pending changes */
  // Update RLC + PDCP SRBs (no DRBs until MME Status Transfer)
  rrc_ue->apply_pdcp_srb_updates(rrc_ue->current_ue_cfg.rr_cfg);
  rrc_ue->apply_rlc_rb_updates(rrc_ue->current_ue_cfg.rr_cfg);
  // Update MAC
  rrc_ue->mac_ctrl.handle_target_enb_ho_cmd(recfg_r8, rrc_ue->ue_capabilities);
  // Apply PHY updates
  rrc_ue->apply_reconf_phy_config(recfg_r8, true);

  // Set admitted E-RABs
  std::vector<asn1::s1ap::erab_admitted_item_s> admitted_erabs;
  for (const auto& erab : rrc_ue->bearer_list.get_erabs()) {
    admitted_erabs.emplace_back();
    asn1::s1ap::erab_admitted_item_s& admitted_erab = admitted_erabs.back();
    admitted_erab.erab_id                           = erab.second.id;
    srsran::uint32_to_uint8(erab.second.teid_in, admitted_erab.gtp_teid.data());

    // Establish GTPU Forwarding Paths
    if (ho_req.transparent_container->erab_info_list_present) {
      const auto& lst = ho_req.transparent_container->erab_info_list;
      const auto* it  = std::find_if(
          lst.begin(),
          lst.end(),
          [&erab](const asn1::s1ap::protocol_ie_single_container_s<asn1::s1ap::erab_info_list_ies_o>& fwd_erab) {
            return fwd_erab.value.erab_info_list_item().erab_id == erab.second.id;
          });
      if (it == lst.end()) {
        continue;
      }
      const auto& fwd_erab = it->value.erab_info_list_item();

      if (fwd_erab.dl_forwarding_present and
          fwd_erab.dl_forwarding.value == asn1::s1ap::dl_forwarding_opts::dl_forwarding_proposed) {
        admitted_erab.dl_g_tp_teid_present = true;
        gtpu_interface_rrc::bearer_props props;
        props.flush_before_teidin_present     = true;
        props.flush_before_teidin             = erab.second.teid_in;
        srsran::expected<uint32_t> dl_teid_in = rrc_ue->bearer_list.add_gtpu_bearer(
            erab.second.id, erab.second.teid_out, erab.second.address.to_number(), &props);
        if (not dl_teid_in.has_value()) {
          logger.error("Failed to allocate GTPU TEID for E-RAB id=%d", fwd_erab.erab_id);
          not_admitted_erabs.emplace_back();
          not_admitted_erabs.back().erab_id = erab.second.id;
          not_admitted_erabs.back().cause.set_transport().value =
              asn1::s1ap::cause_transport_opts::transport_res_unavailable;
          admitted_erabs.pop_back();
          continue;
        }
        fwd_tunnels.push_back(dl_teid_in.value());
        srsran::uint32_to_uint8(dl_teid_in.value(), admitted_erabs.back().dl_g_tp_teid.data());
      }
    }
  }

  /// If the target eNB does not admit at least one non-GBR E-RAB, ..., it shall send the HANDOVER FAILURE message ...
  if (admitted_erabs.empty()) {
    cause.set_radio_network().value = asn1::s1ap::cause_radio_network_opts::unspecified;
    if (not not_admitted_erabs.empty()) {
      cause = not_admitted_erabs[0].cause;
    }
    trigger(ho_failure_ev{cause});
    return;
  }

  // send S1AP HandoverRequestAcknowledge
  if (not rrc_enb->s1ap->send_ho_req_ack(*ho_req.ho_req_msg,
                                         rrc_ue->rnti,
                                         rrc_ue->ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
                                         std::move(ho_cmd_pdu),
                                         admitted_erabs,
                                         not_admitted_erabs)) {
    cause.set_protocol().value = asn1::s1ap::cause_protocol_opts::transfer_syntax_error;
    trigger(ho_failure_ev{cause});
    return;
  }
}

void rrc::ue::rrc_mobility::handle_ho_failure(const ho_failure_ev& ev)
{
  // Store Handover failure cause
  failure_cause = ev.cause;
}

bool rrc::ue::rrc_mobility::apply_ho_prep_cfg(const ho_prep_info_r8_ies_s&          ho_prep,
                                              const asn1::s1ap::ho_request_s&       ho_req_msg,
                                              std::vector<asn1::s1ap::erab_item_s>& erabs_failed_to_setup,
                                              asn1::s1ap::cause_c&                  cause)
{
  const ue_cell_ded* target_cell     = rrc_ue->ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
  const cell_cfg_t&  target_cell_cfg = target_cell->cell_common->cell_cfg;

  // Establish ERABs/DRBs
  for (const auto& erab_item : ho_req_msg.protocol_ies.erab_to_be_setup_list_ho_req.value) {
    const auto& erab = erab_item.value.erab_to_be_setup_item_ho_req();
    if (erab.ext) {
      get_logger().warning("Not handling E-RABToBeSetupList extensions");
    }
    if (erab.transport_layer_address.length() > 32) {
      get_logger().error("IPv6 addresses not currently supported");
      erabs_failed_to_setup.emplace_back();
      erabs_failed_to_setup.back().erab_id                     = erab.erab_id;
      erabs_failed_to_setup.back().cause.set_transport().value = asn1::s1ap::cause_transport_opts::unspecified;
      continue;
    }

    // Create E-RAB and associated main GTPU tunnel
    uint32_t teid_out = 0;
    srsran::uint8_to_uint32(erab.gtp_teid.data(), &teid_out);
    asn1::s1ap::cause_c erab_cause;
    if (rrc_ue->bearer_list.add_erab(
            erab.erab_id, erab.erab_level_qos_params, erab.transport_layer_address, teid_out, {}, erab_cause) !=
        SRSRAN_SUCCESS) {
      erabs_failed_to_setup.emplace_back();
      erabs_failed_to_setup.back().erab_id = erab.erab_id;
      erabs_failed_to_setup.back().cause   = erab_cause;
      continue;
    }
    if (rrc_ue->bearer_list.add_gtpu_bearer(erab.erab_id) != SRSRAN_SUCCESS) {
      erabs_failed_to_setup.emplace_back();
      erabs_failed_to_setup.back().erab_id = erab.erab_id;
      erabs_failed_to_setup.back().cause.set_transport().value =
          asn1::s1ap::cause_transport_opts::transport_res_unavailable;
      rrc_ue->bearer_list.release_erab(erab.erab_id);
      continue;
    }
  }

  // Regenerate AS Keys
  // See TS 33.401, Sec. 7.2.8.4.3
  if (not rrc_ue->ue_security_cfg.set_security_capabilities(ho_req_msg.protocol_ies.ue_security_cap.value)) {
    cause.set_radio_network().value =
        asn1::s1ap::cause_radio_network_opts::encryption_and_or_integrity_protection_algorithms_not_supported;
    return false;
  }
  rrc_ue->ue_security_cfg.set_security_key(ho_req_msg.protocol_ies.security_context.value.next_hop_param);
  rrc_ue->ue_security_cfg.set_ncc(ho_req_msg.protocol_ies.security_context.value.next_hop_chaining_count);
  rrc_ue->ue_security_cfg.regenerate_keys_handover(target_cell_cfg.pci, target_cell_cfg.dl_earfcn);

  // Save UE Capabilities
  for (const auto& cap : ho_prep.ue_radio_access_cap_info) {
    if (cap.rat_type.value == rat_type_opts::eutra) {
      asn1::cbit_ref bref(cap.ue_cap_rat_container.data(), cap.ue_cap_rat_container.size());
      if (rrc_ue->eutra_capabilities.unpack(bref) != asn1::SRSASN_SUCCESS) {
        logger.warning("Failed to unpack UE EUTRA Capability");
        continue;
      }
      if (logger.debug.enabled()) {
        asn1::json_writer js{};
        rrc_ue->eutra_capabilities.to_json(js);
        logger.debug("New rnti=0x%x EUTRA capabilities: %s", rrc_ue->rnti, js.to_string().c_str());
      }
      rrc_ue->ue_capabilities             = srsran::make_rrc_ue_capabilities(rrc_ue->eutra_capabilities);
      rrc_ue->eutra_capabilities_unpacked = true;
    }
  }

  // Update SCells list
  rrc_ue->update_scells();

  // Save source eNB UE RR cfg as a starting point
  apply_rr_cfg_ded_diff(rrc_ue->current_ue_cfg.rr_cfg, ho_prep.as_cfg.source_rr_cfg);

  // Save measConfig
  rrc_ue->current_ue_cfg.meas_cfg = ho_prep.as_cfg.source_meas_cfg;

  // Save source UE MAC configuration as a base
  rrc_ue->mac_ctrl.handle_ho_prep(ho_prep);

  return true;
}

void rrc::ue::rrc_mobility::handle_recfg_complete(wait_recfg_comp& s, const recfg_complete_ev& ev)
{
  ue_cell_ded* target_cell = rrc_ue->ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
  logger.info("User rnti=0x%x successfully handovered to cell_id=0x%x",
              rrc_ue->rnti,
              target_cell->cell_common->cell_cfg.cell_id);
  uint64_t target_eci = (rrc_enb->cfg.enb_id << 8u) + target_cell->cell_common->cell_cfg.cell_id;

  rrc_enb->s1ap->send_ho_notify(rrc_ue->rnti, target_eci);

  // Enable forwarding of GTPU SDUs coming from Source eNB Tunnel to PDCP
  auto& fwd_tunnels = get_state<s1_target_ho_st>()->pending_tunnels;
  for (uint32_t teid : fwd_tunnels) {
    rrc_enb->gtpu->set_tunnel_status(teid, true);
  }
}

void rrc::ue::rrc_mobility::handle_status_transfer(s1_target_ho_st& s, const status_transfer_ev& erabs)
{
  // Establish DRBs
  rrc_ue->apply_pdcp_drb_updates(rrc_ue->current_ue_cfg.rr_cfg);

  // Set DRBs SNs
  for (const auto& erab : erabs) {
    const auto& erab_item = erab.value.bearers_subject_to_status_transfer_item();
    auto        erab_it   = rrc_ue->bearer_list.get_erabs().find(erab_item.erab_id);
    if (erab_it == rrc_ue->bearer_list.get_erabs().end()) {
      logger.warning("The E-RAB Id=%d is not recognized", erab_item.erab_id);
      continue;
    }
    const auto& drbs   = rrc_ue->bearer_list.get_established_drbs();
    lte_drb     drbid  = lte_lcid_to_drb(erab_it->second.lcid);
    auto        drb_it = std::find_if(
        drbs.begin(), drbs.end(), [drbid](const drb_to_add_mod_s& drb) { return (lte_drb)drb.drb_id == drbid; });
    if (drb_it == drbs.end()) {
      logger.warning("The DRB id=%d does not exist", drbid);
    }

    srsran::pdcp_lte_state_t drb_state{};
    drb_state.tx_hfn                    = erab_item.dl_coun_tvalue.hfn;
    drb_state.next_pdcp_tx_sn           = erab_item.dl_coun_tvalue.pdcp_sn;
    drb_state.rx_hfn                    = erab_item.ul_coun_tvalue.hfn;
    drb_state.next_pdcp_rx_sn           = erab_item.ul_coun_tvalue.pdcp_sn;
    uint8_t  sn_len                     = srsran::get_pdcp_drb_sn_len(drb_it->pdcp_cfg);
    uint32_t maximum_pdcp_sn            = (1u << sn_len) - 1u;
    drb_state.last_submitted_pdcp_rx_sn = std::min(erab_item.ul_coun_tvalue.pdcp_sn - 1u, maximum_pdcp_sn);
    logger.info("Setting lcid=%d PDCP state to {Tx SN: %d, Rx SN: %d}",
                drb_it->lc_ch_id,
                drb_state.next_pdcp_tx_sn,
                drb_state.next_pdcp_rx_sn);
    rrc_enb->pdcp->set_bearer_state(rrc_ue->rnti, drb_it->lc_ch_id, drb_state);
  }

  // Check if there is any pending Reconfiguration Complete. If there is, self-trigger
  if (pending_recfg_complete.crit_exts.type().value != rrc_conn_recfg_complete_s::crit_exts_c_::types_opts::nulltype) {
    trigger(pending_recfg_complete);
    pending_recfg_complete.crit_exts.set(rrc_conn_recfg_complete_s::crit_exts_c_::types_opts::nulltype);
  }
}

void rrc::ue::rrc_mobility::defer_recfg_complete(s1_target_ho_st& s, const recfg_complete_ev& ev)
{
  pending_recfg_complete = ev;
}

/*************************************************************************************************
 *                                  intraENB Handover sub-FSM
 ************************************************************************************************/

void rrc::ue::rrc_mobility::intraenb_ho_st::enter(rrc_mobility* f, const ho_meas_report_ev& meas_report)
{
  uint32_t cell_id = rrc_details::eci_to_cellid(meas_report.target_eci);
  target_cell      = f->rrc_enb->cell_common_list->get_cell_id(cell_id);
  source_cell      = f->rrc_ue->ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common;
  if (target_cell == nullptr) {
    f->logger.error("The target cell_id=0x%x was not found in the list of eNB cells", cell_id);
    f->trigger(srsran::failure_ev{});
    return;
  }

  f->logger.info("Starting intraeNB Handover of rnti=0x%x to 0x%x.", f->rrc_ue->rnti, meas_report.target_eci);

  if (target_cell == nullptr) {
    f->trigger(srsran::failure_ev{});
    return;
  }
  last_temp_crnti = SRSRAN_INVALID_RNTI;

  /* Allocate Resources in Target Cell */
  if (not f->rrc_ue->ue_cell_list.set_cells({target_cell->enb_cc_idx})) {
    f->trigger(srsran::failure_ev{});
    return;
  }
  f->rrc_ue->update_scells();

  /* Prepare RRC Reconf Message with mobility info */
  dl_dcch_msg_s dl_dcch_msg;
  f->fill_mobility_reconf_common(dl_dcch_msg, *target_cell, source_cell->cell_cfg.dl_earfcn, source_cell->cell_cfg.pci);
  rrc_conn_recfg_r8_ies_s& reconf_r8 = dl_dcch_msg.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();

  // Apply changes to the MAC scheduler
  f->rrc_ue->mac_ctrl.handle_intraenb_ho_cmd(reconf_r8, f->rrc_ue->ue_capabilities);

  f->rrc_ue->apply_setup_phy_common(f->rrc_enb->cfg.sibs[1].sib2().rr_cfg_common, false);
  f->rrc_ue->apply_reconf_phy_config(reconf_r8, false);

  // Send DL-DCCH Message via current PCell
  if (not f->rrc_ue->send_dl_dcch(&dl_dcch_msg)) {
    f->trigger(srsran::failure_ev{});
    return;
  }

  // Log HO command.
  event_logger::get().log_handover_command(
      f->rrc_ue->get_cell_list().get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common->enb_cc_idx,
      reconf_r8.mob_ctrl_info.target_pci,
      reconf_r8.mob_ctrl_info.carrier_freq.dl_carrier_freq,
      reconf_r8.mob_ctrl_info.new_ue_id.to_number(),
      f->rrc_ue->rnti);
}

void rrc::ue::rrc_mobility::handle_crnti_ce(intraenb_ho_st& s, const user_crnti_upd_ev& ev)
{
  logger.info("UE performing handover updated its temp-crnti=0x%x to rnti=0x%x", ev.temp_crnti, ev.crnti);
  bool is_first_crnti_ce = s.last_temp_crnti == SRSRAN_INVALID_RNTI;
  s.last_temp_crnti      = ev.temp_crnti;

  if (is_first_crnti_ce) {
    // Stop all running RLF timers
    // Note: The RLF timer can be triggered during Handover because the UE did not RLC-ACK the Handover Command
    //       Once the Handover is complete, to avoid releasing the UE, the RLF timer should stop.
    rrc_ue->rlc_rlf_timer.stop();
    rrc_ue->phy_dl_rlf_timer.stop();
    rrc_ue->phy_ul_rlf_timer.stop();

    // Need to reset SNs of bearers.
    rrc_enb->rlc->reestablish(rrc_ue->rnti);
    rrc_enb->pdcp->reestablish(rrc_ue->rnti);

    // Change PCell in MAC/Scheduler
    rrc_ue->mac_ctrl.handle_crnti_ce(ev.temp_crnti);

    // finally apply new phy changes
    rrc_enb->phy->set_config(rrc_ue->rnti, rrc_ue->phy_rrc_dedicated_list);

    rrc_ue->ue_security_cfg.regenerate_keys_handover(s.target_cell->cell_cfg.pci, s.target_cell->cell_cfg.dl_earfcn);
    rrc_ue->apply_pdcp_srb_updates(rrc_ue->current_ue_cfg.rr_cfg);
    rrc_ue->apply_pdcp_drb_updates(rrc_ue->current_ue_cfg.rr_cfg);

    // Send PDCP status report if necessary
    rrc_enb->pdcp->send_status_report(rrc_ue->rnti);
  } else {
    logger.info("Received duplicate C-RNTI CE during rnti=0x%x handover.", rrc_ue->rnti);
  }
}

void rrc::ue::rrc_mobility::handle_recfg_complete(intraenb_ho_st& s, const recfg_complete_ev& ev)
{
  logger.info("User rnti=0x%x successfully handovered to cell_id=0x%x", rrc_ue->rnti, s.target_cell->cell_cfg.cell_id);
}

} // namespace srsenb
