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

#include "srsue/hdr/stack/rrc_nr/rrc_nr_procedures.h"
#include "srsran/common/standard_streams.h"

#define Error(fmt, ...) rrc_handle.logger.error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Warning(fmt, ...) rrc_handle.logger.warning("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Info(fmt, ...) rrc_handle.logger.info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Debug(fmt, ...) rrc_handle.logger.debug("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

using namespace asn1::rrc_nr;
using namespace asn1;
using namespace srsran;

namespace srsue {

rrc_nr::connection_reconf_no_ho_proc::connection_reconf_no_ho_proc(rrc_nr& parent_) : rrc_handle(parent_), initiator(nr)
{}

proc_outcome_t rrc_nr::connection_reconf_no_ho_proc::init(const reconf_initiator_t         initiator_,
                                                          const bool                       endc_release_and_add_r15,
                                                          const asn1::rrc_nr::rrc_recfg_s& rrc_nr_reconf)
{
  Info("Starting...");
  initiator = initiator_;

  asn1::json_writer js;
  rrc_nr_reconf.to_json(js);
  Debug("RRC NR Reconfiguration: %s", js.to_string().c_str());

  if (rrc_nr_reconf.crit_exts.rrc_recfg().secondary_cell_group.size() > 0) {
    if (rrc_nr_reconf.crit_exts.type() != asn1::rrc_nr::rrc_recfg_s::crit_exts_c_::types::rrc_recfg) {
      Error("Reconfiguration does not contain Secondary Cell Group Config.");
      return proc_outcome_t::error;
    }

    cbit_ref bref0(rrc_nr_reconf.crit_exts.rrc_recfg().secondary_cell_group.data(),
                   rrc_nr_reconf.crit_exts.rrc_recfg().secondary_cell_group.size());

    cell_group_cfg_s secondary_cell_group_cfg;
    if (secondary_cell_group_cfg.unpack(bref0) != asn1::SRSASN_SUCCESS) {
      Error("Could not unpack Secondary Cell Group Config.");
      return proc_outcome_t::error;
    }

    asn1::json_writer js1;
    secondary_cell_group_cfg.to_json(js1);
    Debug("Secondary Cell Group: %s", js1.to_string().c_str());

    Info("Applying Secondary Cell Group Cfg.");
    if (!rrc_handle.apply_cell_group_cfg(secondary_cell_group_cfg)) {
      return proc_outcome_t::error;
    }
  }

  if (rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.non_crit_ext.non_crit_ext.sk_counter_present) {
    Info("Applying SK Counter");
    if (!rrc_handle.configure_sk_counter(
            (uint16_t)rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.non_crit_ext.non_crit_ext.sk_counter)) {
      return proc_outcome_t::error;
    }
  }

  if (rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.master_cell_group.size() > 0) {
    cbit_ref bref1(rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.master_cell_group.data(),
                   rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.master_cell_group.size());

    cell_group_cfg_s master_cell_group_cfg;
    if (master_cell_group_cfg.unpack(bref1) != asn1::SRSASN_SUCCESS) {
      Error("Could not unpack Master Cell Group Config.");
      return proc_outcome_t::error;
    }

    asn1::json_writer js2;
    master_cell_group_cfg.to_json(js2);
    Debug("Master Cell Group: %s", js2.to_string().c_str());

    Info("Applying Master Cell Group Cfg.");
    if (!rrc_handle.apply_cell_group_cfg(master_cell_group_cfg)) {
      return proc_outcome_t::error;
    }
  }

  if (rrc_nr_reconf.crit_exts.rrc_recfg().radio_bearer_cfg_present) {
    Info("Applying Radio Bearer Cfg.");
    if (!rrc_handle.apply_radio_bearer_cfg(rrc_nr_reconf.crit_exts.rrc_recfg().radio_bearer_cfg)) {
      return proc_outcome_t::error;
    }
  }

  // only send reconfig complete in SA mode
  if (rrc_handle.rrc_eutra == nullptr) {
    rrc_handle.send_rrc_reconfig_complete();
  }

  // Handle NAS messages
  if (rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.ded_nas_msg_list.size() > 0) {
    for (uint32_t i = 0; i < rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.ded_nas_msg_list.size(); ++i) {
      srsran::unique_byte_buffer_t nas_pdu = srsran::make_byte_buffer();
      if (nas_pdu != nullptr) {
        memcpy(nas_pdu->msg,
               rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.ded_nas_msg_list[i].data(),
               rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.ded_nas_msg_list[i].size());
        nas_pdu->N_bytes = rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.ded_nas_msg_list[i].size();
        rrc_handle.nas->write_pdu(std::move(nas_pdu));
      } else {
        rrc_handle.logger.error("Couldn't allocate SDU in %s.", __FUNCTION__);
        return proc_outcome_t::error;
      }
    }
  }

  return proc_outcome_t::success;
}

proc_outcome_t rrc_nr::connection_reconf_no_ho_proc::react(const bool& config_complete)
{
  if (not config_complete) {
    Error("NR reconfiguration failed");
    return proc_outcome_t::error;
  }

  // TODO phy ctrl
  // in case there are scell to configure, wait for second phy configuration
  // if (not rrc_ptr->phy_ctrl->is_config_pending()) {
  //   return proc_outcome_t::yield;
  // }

  Info("Reconfig NR return successful");
  return proc_outcome_t::success;
}

void rrc_nr::connection_reconf_no_ho_proc::then(const srsran::proc_state_t& result)
{
  if (result.is_success()) {
    Info("Finished %s successfully", name());
    srsran::console("RRC NR reconfiguration successful.\n");
    if (rrc_handle.rrc_eutra) {
      rrc_handle.rrc_eutra->nr_rrc_con_reconfig_complete(true);
    }
  } else {
    // 5.3.5.8.2 Inability to comply with RRCReconfiguration
    switch (initiator) {
      case reconf_initiator_t::mcg_srb1:
        if (rrc_handle.rrc_eutra) {
          rrc_handle.rrc_eutra->nr_notify_reconfiguration_failure();
        }
        break;
      default:
        Warning("Reconfiguration failure not implemented for initiator %d", initiator);
        break;
    }
    srsran::console("RRC NR reconfiguration failed.\n");
    Warning("Finished %s with failure", name());
  }
  return;
}

/**************************************
 *     RRC Setup Request Procedure
 *************************************/

rrc_nr::setup_request_proc::setup_request_proc(rrc_nr& parent_) :
  rrc_handle(parent_), logger(srslog::fetch_basic_logger("RRC-NR"))
{}

proc_outcome_t rrc_nr::setup_request_proc::init(srsran::nr_establishment_cause_t cause_,
                                                srsran::unique_byte_buffer_t     dedicated_info_nas_)
{
  cause              = cause_;
  dedicated_info_nas = std::move(dedicated_info_nas_);

  if (!rrc_handle.plmn_is_selected) {
    Error("Trying to connect but PLMN not selected.");
    return proc_outcome_t::error;
  }

  if (rrc_handle.state != RRC_NR_STATE_IDLE) {
    logger.warning("Requested RRC connection establishment while not in IDLE");
    return proc_outcome_t::error;
  }

  // TODO: add T302 handling

  Info("Initiation of Setup request procedure");

  cell_search_ret = rrc_cell_search_result_t::no_cell;

  state = state_t::cell_selection;
  if (rrc_handle.cell_selector.is_idle()) {
    // No one is running cell selection
    if (not rrc_handle.cell_selector.launch()) {
      Error("Failed to initiate cell selection procedure...");
      return proc_outcome_t::error;
    }
    rrc_handle.callback_list.add_proc(rrc_handle.cell_selector);
  } else {
    Info("Cell selection proc already on-going. Wait for its result");
  }
  return proc_outcome_t::yield;
}

proc_outcome_t rrc_nr::setup_request_proc::step()
{
  if (state == state_t::cell_selection) {
    // NOTE: cell selection will signal back with an event trigger
    return proc_outcome_t::yield;
  }

  if (state == state_t::config_serving_cell) {
    // TODO: start serving cell config and start T300

    // start T300
    rrc_handle.t300.run();

    // Send setup request message to lower layers
    rrc_handle.send_setup_request(cause);

    // Save dedicatedInfoNAS SDU, if needed (TODO: this should be passed to procedure without temp storage)
    if (dedicated_info_nas.get()) {
      if (rrc_handle.dedicated_info_nas.get()) {
        Warning("Received a new dedicatedInfoNAS SDU but there was one still in queue. Removing it.");
        rrc_handle.dedicated_info_nas.reset();
      }

      Debug("Updating dedicatedInfoNAS in RRC");
      rrc_handle.dedicated_info_nas = std::move(dedicated_info_nas);
    } else {
      Debug("dedicatedInfoNAS has already been provided to RRC.");
    }

    Info("Waiting for RRCSetup/Reject or expiry");
    state = state_t::wait_t300;
    return step();

  } else if (state == state_t::wait_t300) {
    // Wait until t300 stops due to RRCConnectionSetup/Reject or expiry
    if (rrc_handle.t300.is_running()) {
      return proc_outcome_t::yield;
    }

    if (rrc_handle.state == RRC_NR_STATE_CONNECTED) {
      // Received ConnectionSetup
      return proc_outcome_t::success;
    }
  }

  return proc_outcome_t::error;
}

void rrc_nr::setup_request_proc::then(const srsran::proc_state_t& result)
{
  if (result.is_error()) {
    logger.warning("Could not finish setup request. Deallocating dedicatedInfoNAS PDU");
    dedicated_info_nas.reset();
    rrc_handle.dedicated_info_nas.reset();
  } else {
    Info("Finished connection request procedure successfully.");
  }
  // TODO: signal back to NAS
  // rrc_handle.nas->connection_request_completed(result.is_success());
}

srsran::proc_outcome_t rrc_nr::setup_request_proc::react(const cell_selection_proc::cell_selection_complete_ev& e)
{
  if (state != state_t::cell_selection) {
    // ignore if we are not expecting an cell selection result
    return proc_outcome_t::yield;
  }
  if (e.is_error()) {
    return proc_outcome_t::error;
  }
  cell_search_ret = *e.value();
  // .. and SI acquisition
  // TODO @ismagom use appropiate PHY interface
  if (true /*rrc_handle.phy->cell_is_camping()*/) {
    // TODO: Set default configurations
    // rrc_handle.set_phy_default();
    // rrc_handle.set_mac_default();

    // CCCH configuration applied already at start
    // timeAlignmentCommon applied in configure_serving_cell

    Info("Configuring serving cell...");
    state = state_t::config_serving_cell;

    // Skip SI acquisition
    return step();
  }
}

/******************************************
 *  Connection Setup Procedure
 *****************************************/

// Simple procedure mainly do defer the transmission of the SetupComplete until all PHY reconfiguration are done
rrc_nr::connection_setup_proc::connection_setup_proc(srsue::rrc_nr& parent_) :
  rrc_handle(parent_), logger(srslog::fetch_basic_logger("RRC-NR"))
{}

srsran::proc_outcome_t rrc_nr::connection_setup_proc::init(const asn1::rrc_nr::radio_bearer_cfg_s& radio_bearer_cfg_,
                                                           const asn1::rrc_nr::cell_group_cfg_s&   cell_group_,
                                                           srsran::unique_byte_buffer_t            dedicated_info_nas_)
{
  Info("Starting...");

  if (dedicated_info_nas_ == nullptr) {
    logger.error("Connection Setup Failed, no dedicatedInfoNAS available");
    return proc_outcome_t::error;
  }

  dedicated_info_nas = std::move(dedicated_info_nas_);

  // Stop T300
  rrc_handle.t300.stop();

  // Apply the Cell Group configuration
  if (!rrc_handle.update_cell_group_cfg(cell_group_)) {
    return proc_outcome_t::error;
  }

  // Apply the Radio Bearer configuration
  if (!rrc_handle.apply_radio_bearer_cfg(radio_bearer_cfg_)) {
    return proc_outcome_t::error;
  }

  return proc_outcome_t::yield;
}

srsran::proc_outcome_t rrc_nr::connection_setup_proc::react(const bool& config_complete)
{
  if (not config_complete) {
    logger.error("Connection Setup Failed");
    return proc_outcome_t::error;
  }

  rrc_handle.send_con_setup_complete(std::move(dedicated_info_nas));
  return proc_outcome_t::success;
}

void rrc_nr::connection_setup_proc::then(const srsran::proc_state_t& result)
{
  if (result.is_success()) {
    logger.info("Finished %s successfully", name());
    return;
  }
}

/**************************************
 * Combined Cell Search/Selection Procedure
 *************************************/

rrc_nr::cell_selection_proc::cell_selection_proc(rrc_nr& parent_) : rrc_handle(parent_) {}

// Starts PHY's cell search in the current ARFCN
proc_outcome_t rrc_nr::cell_selection_proc::init()
{
  Info("Starting...");
  state = state_t::phy_cell_search;

  // TODO: add full cell selection
  // Start cell search
  phy_interface_rrc_nr::cell_search_args_t cs_args = {};
  cs_args.center_freq_hz                           = rrc_handle.phy_cfg.carrier.dl_center_frequency_hz;
  cs_args.ssb_freq_hz                              = rrc_handle.phy_cfg.carrier.ssb_center_freq_hz;
  cs_args.ssb_scs                                  = rrc_handle.phy_cfg.ssb.scs;
  cs_args.ssb_pattern                              = rrc_handle.phy_cfg.ssb.pattern;
  cs_args.duplex_mode                              = rrc_handle.phy_cfg.duplex.mode;
  if (not rrc_handle.phy->start_cell_search(cs_args)) {
    Error("Failed to initiate Cell Search.");
    return proc_outcome_t::error;
  }

  return proc_outcome_t::yield;
}

proc_outcome_t rrc_nr::cell_selection_proc::step()
{
  switch (state) {
    case state_t::phy_cell_search:
    case state_t::phy_cell_select:
    case state_t::sib_acquire:
      // Waits for cell select/search to complete
      return proc_outcome_t::yield;
  }
  return proc_outcome_t::yield;
}

// Handles result of PHY's cell search and triggers PHY cell select when new cell was found
proc_outcome_t
rrc_nr::cell_selection_proc::handle_cell_search_result(const rrc_interface_phy_nr::cell_search_result_t& result)
{
  if (!result.cell_found) {
    Info("Cell search did not find any cell.");
    return proc_outcome_t::error;
  }

  // Convert Cell measurement in Text
  std::array<char, 512> csi_info_str = {};
  srsran_csi_meas_info_short(&result.measurements, csi_info_str.data(), (uint32_t)csi_info_str.size());

  // Unpack MIB and convert to text
  srsran_mib_nr_t       mib          = {};
  std::array<char, 512> mib_info_str = {};
  if (srsran_pbch_msg_nr_mib_unpack(&result.pbch_msg, &mib) == SRSASN_SUCCESS) {
    // Convert to text
    srsran_pbch_msg_nr_mib_info(&mib, mib_info_str.data(), (uint32_t)mib_info_str.size());
  } else {
    // It could be the PBCH does not carry MIB
    strcpy(mib_info_str.data(), "No MIB found");
    Error("No MIB found\n");
    return proc_outcome_t::error;
  }

  // Check unsupported settings
  if (mib.cell_barred) {
    Error("Cell barred");
    return proc_outcome_t::error;
  }
  if (mib.scs_common != srsran_subcarrier_spacing_15kHz) {
    Error("Unsupported SCS %s", srsran_subcarrier_spacing_to_str(mib.scs_common));
    return proc_outcome_t::error;
  }

  // Logs the PCI, cell measurements and decoded MIB
  Info("Cell search found ARFCN=%d PCI=%d %s %s",
       result.ssb_arfcn,
       result.pci,
       csi_info_str.data(),
       mib_info_str.data());

  // Apply MIB settings
  srsran::phy_cfg_nr_t& phy_cfg = rrc_handle.phy_cfg;
  phy_cfg.pdsch.typeA_pos       = mib.dmrs_typeA_pos;
  phy_cfg.pdsch.scs_cfg         = mib.scs_common;
  phy_cfg.carrier.pci           = result.pci;

  // Get pointA and SSB absolute frequencies
  double pointA_abs_freq_Hz = phy_cfg.carrier.dl_center_frequency_hz -
                              phy_cfg.carrier.nof_prb * SRSRAN_NRE * SRSRAN_SUBC_SPACING_NR(phy_cfg.carrier.scs) / 2;
  double ssb_abs_freq_Hz = phy_cfg.carrier.ssb_center_freq_hz;
  // Calculate integer SSB to pointA frequency offset in Hz
  uint32_t ssb_pointA_freq_offset_Hz =
      (ssb_abs_freq_Hz > pointA_abs_freq_Hz) ? (uint32_t)(ssb_abs_freq_Hz - pointA_abs_freq_Hz) : 0;

  // Create coreset0
  if (srsran_coreset_zero(phy_cfg.carrier.pci,
                          ssb_pointA_freq_offset_Hz,
                          phy_cfg.ssb.scs,
                          phy_cfg.carrier.scs,
                          mib.coreset0_idx,
                          &phy_cfg.pdcch.coreset[0])) {
    Error("Error generating coreset0");
    return proc_outcome_t::error;
  }
  phy_cfg.pdcch.coreset_present[0] = true;

  // Create SearchSpace0
  make_phy_search_space0_cfg(&phy_cfg.pdcch.search_space[0]);
  phy_cfg.pdcch.search_space_present[0] = true;

  // Set dummy offset to pass PRACH config check, real value is provided in SIB1
  phy_cfg.prach.freq_offset = 1;

  // Update PHY configuration
  rrc_handle.phy_cfg_state = PHY_CFG_STATE_SA_MIB_CFG;
  if (not rrc_handle.phy->set_config(phy_cfg)) {
    Error("Setting PHY configuration");
    return proc_outcome_t::error;
  }

  phy_interface_rrc_nr::cell_select_args_t cs_args = {};
  cs_args.carrier                                  = rrc_handle.phy_cfg.carrier;
  cs_args.ssb_cfg                                  = rrc_handle.phy_cfg.get_ssb_cfg();

  // Transition to cell selection ignoring the cell search result
  state = state_t::phy_cell_select;
  if (not rrc_handle.phy->start_cell_select(cs_args)) {
    Error("Could not set start cell search.");
    return proc_outcome_t::error;
  }
  return proc_outcome_t::yield;
}

proc_outcome_t rrc_nr::cell_selection_proc::react(const rrc_interface_phy_nr::cell_select_result_t& event)
{
  if (state != state_t::phy_cell_select) {
    Warning("Received unexpected cell select result");
    return proc_outcome_t::yield;
  }

  if (event.status != rrc_interface_phy_nr::cell_select_result_t::SUCCESSFUL) {
    Error("Couldn't select new serving cell");
    phy_search_result.cell_found = false;
    rrc_search_result            = rrc_nr::rrc_cell_search_result_t::no_cell;
    return proc_outcome_t::error;
  }

  rrc_search_result = rrc_nr::rrc_cell_search_result_t::same_cell;

  // PHY is now camping on serving cell
  Info("Cell selection completed. Starting SIB1 acquisition");

  // Transition to cell selection ignoring the cell search result
  state = state_t::sib_acquire;
  rrc_handle.mac->bcch_search(true);
  return proc_outcome_t::yield;
}

proc_outcome_t rrc_nr::cell_selection_proc::react(const bool sib1_found)
{
  if (state != state_t::sib_acquire) {
    Warning("Received unexpected cell select result");
    return proc_outcome_t::yield;
  }

  Info("SIB1 acquired successfully");
  rrc_handle.mac->bcch_search(false);

  return proc_outcome_t::success;
}

proc_outcome_t rrc_nr::cell_selection_proc::react(const rrc_interface_phy_nr::cell_search_result_t& event)
{
  if (state != state_t::phy_cell_search) {
    Error("Received unexpected cell search result");
    return proc_outcome_t::error;
  }
  phy_search_result = event;

  if (phy_search_result.cell_found) {
    return handle_cell_search_result(phy_search_result);
  }
  return proc_outcome_t::error;
}

void rrc_nr::cell_selection_proc::then(const cell_selection_complete_ev& proc_result) const
{
  Info("Completed with %s.", proc_result.is_success() ? "success" : "failure");
  // Inform Connection Request Procedure
  rrc_handle.task_sched.defer_task([this, proc_result]() {
    if (rrc_handle.setup_req_proc.is_busy()) {
      rrc_handle.setup_req_proc.trigger(proc_result);
    }
  });
}

} // namespace srsue
