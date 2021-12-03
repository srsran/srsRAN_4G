/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

proc_outcome_t rrc_nr::connection_reconf_no_ho_proc::init(const reconf_initiator_t initiator_,
                                                          const bool               endc_release_and_add_r15,
                                                          const bool nr_secondary_cell_group_cfg_r15_present,
                                                          const asn1::dyn_octstring nr_secondary_cell_group_cfg_r15,
                                                          const bool                sk_counter_r15_present,
                                                          const uint32_t            sk_counter_r15,
                                                          const bool                nr_radio_bearer_cfg1_r15_present,
                                                          const asn1::dyn_octstring nr_radio_bearer_cfg1_r15)
{
  Info("Starting...");
  initiator = initiator_;

  rrc_recfg_s        rrc_recfg;
  cell_group_cfg_s   cell_group_cfg;
  radio_bearer_cfg_s radio_bearer_cfg;
  asn1::SRSASN_CODE  err;

  if (nr_secondary_cell_group_cfg_r15_present) {
    cbit_ref bref(nr_secondary_cell_group_cfg_r15.data(), nr_secondary_cell_group_cfg_r15.size());
    err = rrc_recfg.unpack(bref);
    if (err != asn1::SRSASN_SUCCESS) {
      Error("Could not unpack NR reconfiguration message.");
      return proc_outcome_t::error;
    }

#if 0
    rrc_ptr->log_rrc_message(
        "RRC NR Reconfiguration", Rx, nr_secondary_cell_group_cfg_r15, rrc_recfg, "NR Secondary Cell Group Cfg R15");
#endif

    if (rrc_recfg.crit_exts.type() != asn1::rrc_nr::rrc_recfg_s::crit_exts_c_::types::rrc_recfg) {
      Error("Reconfiguration does not contain Secondary Cell Group Config");
      return proc_outcome_t::error;
    }

    if (not rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group_present) {
      Error("Reconfiguration does not contain Secondary Cell Group Config");
      return proc_outcome_t::error;
    }

    cbit_ref bref0(rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group.data(),
                   rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group.size());

    err = cell_group_cfg.unpack(bref0);
    if (err != asn1::SRSASN_SUCCESS) {
      Error("Could not unpack cell group message message.");
      return proc_outcome_t::error;
    }

#if 0
    rrc_ptr->log_rrc_message("RRC NR Reconfiguration",
                             Rx,
                             rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group,
                             cell_group_cfg,
                             "Secondary Cell Group Config");
#endif

    Info("Applying Cell Group Cfg");
    if (!rrc_handle.apply_cell_group_cfg(cell_group_cfg)) {
      return proc_outcome_t::error;
    }
  }

  if (sk_counter_r15_present) {
    Info("Applying Cell Group Cfg");
    if (!rrc_handle.configure_sk_counter((uint16_t)sk_counter_r15)) {
      return proc_outcome_t::error;
    }
  }

  if (nr_radio_bearer_cfg1_r15_present) {
    cbit_ref bref1(nr_radio_bearer_cfg1_r15.data(), nr_radio_bearer_cfg1_r15.size());

    err = radio_bearer_cfg.unpack(bref1);
    if (err != asn1::SRSASN_SUCCESS) {
      Error("Could not unpack radio bearer config.");
      return proc_outcome_t::error;
    }

#if 0
    rrc_ptr->log_rrc_message(
        "RRC NR Reconfiguration", Rx, nr_radio_bearer_cfg1_r15, radio_bearer_cfg, "Radio Bearer Config R15");
#endif

    Info("Applying Radio Bearer Cfg");
    if (!rrc_handle.apply_radio_bearer_cfg(radio_bearer_cfg)) {
      return proc_outcome_t::error;
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
    rrc_handle.rrc_eutra->nr_rrc_con_reconfig_complete(true);
  } else {
    // 5.3.5.8.2 Inability to comply with RRCReconfiguration
    switch (initiator) {
      case reconf_initiator_t::mcg_srb1:
        rrc_handle.rrc_eutra->nr_notify_reconfiguration_failure();
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
  rrc_handle(parent_), logger(srslog::fetch_basic_logger("RRC"))
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

  Info("Initiation of Connection establishment procedure");

  cell_search_ret = cell_search_result_t::no_cell;

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

    rrc_handle.phy_cfg_state = PHY_CFG_STATE_APPLY_SP_CELL;
    rrc_handle.phy->set_config(rrc_handle.phy_cfg);

    // Send setup request message to lower layers
    rrc_handle.send_setup_request(cause);

    Info("Waiting for RRCSetup/Reject or expiry");
    state = state_t::wait_t300;
    return step();

  } else if (state == state_t::wait_t300) {
    // TODO: add T300 waiting
  }

  return proc_outcome_t::error;
}

void rrc_nr::setup_request_proc::then(const srsran::proc_state_t& result)
{
  if (result.is_error()) {
    logger.warning("Could not establish connection. Deallocating dedicatedInfoNAS PDU");
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
  } else {
    switch (cell_search_ret) {
      case cell_search_result_t::same_cell:
        logger.warning("Did not reselect cell but serving cell is out-of-sync.");
        break;
      case cell_search_result_t::changed_cell:
        logger.warning("Selected a new cell but could not camp on. Setting out-of-sync.");
        break;
      default:
        logger.warning("Could not find any suitable cell to connect");
    }
    return proc_outcome_t::error;
  }
}

/**************************************
 *       Basic Cell Selection Procedure
 *************************************/

rrc_nr::cell_selection_proc::cell_selection_proc(rrc_nr& parent_) :
  rrc_handle(parent_), meas_cells(rrc_handle.meas_cells)
{}

/// Verifies if serving cell passes selection criteria, UE is camping, and required SIBs were obtained
bool rrc_nr::cell_selection_proc::is_serv_cell_suitable() const
{
  // TODO: add selection criteria
  return true;
}

/// Called on procedure exit to set result
proc_outcome_t rrc_nr::cell_selection_proc::set_proc_complete()
{
  if (is_serv_cell_suitable()) {
    cell_search_ret = is_same_cell(init_serv_cell, meas_cells.serving_cell()) ? cell_search_result_t::same_cell
                                                                              : cell_search_result_t::changed_cell;
    return proc_outcome_t::success;
  }
  cell_search_ret = cell_search_result_t::no_cell;
  return proc_outcome_t::error;
}

proc_outcome_t rrc_nr::cell_selection_proc::init()
{
  init_serv_cell = meas_cells.serving_cell().phy_cell;

  // TODO: add full cell selection
  phy_interface_rrc_nr::cell_select_args_t cell_cfg = {};
  cell_cfg.carrier                                  = rrc_handle.phy_cfg.carrier;
  cell_cfg.ssb_cfg                                  = rrc_handle.phy_cfg.get_ssb_cfg();
  rrc_handle.phy->start_cell_select(cell_cfg);

  // Skip cell selection if serving cell is suitable and there are no stronger neighbours in same earfcn
  if (is_serv_cell_suitable()) {
    Debug("Skipping cell selection procedure as there are no stronger neighbours in same EARFCN.");
    return set_proc_complete();
  }

  return set_proc_complete();
}

proc_outcome_t rrc_nr::cell_selection_proc::step()
{
  switch (state) {
    case search_state_t::cell_selection:
      // this state waits for phy event
      return proc_outcome_t::yield;
    case search_state_t::serv_cell_camp:
      // this state waits for phy event
      return proc_outcome_t::yield;
    case search_state_t::cell_config:
      // return step_cell_config();
      return proc_outcome_t::yield;
    case search_state_t::cell_search:
      // return step_cell_search();
      return proc_outcome_t::yield;
  }
  return proc_outcome_t::error;
}

void rrc_nr::cell_selection_proc::then(const srsran::proc_result_t<cell_search_result_t>& proc_result) const
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
