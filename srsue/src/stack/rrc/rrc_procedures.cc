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

#include "srsue/hdr/stack/rrc/rrc_procedures.h"
#include "srslte/common/tti_point.h"
#include <inttypes.h> // for printing uint64_t

#define Error(fmt, ...) rrc_ptr->rrc_log->error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Warning(fmt, ...) rrc_ptr->rrc_log->warning("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Info(fmt, ...) rrc_ptr->rrc_log->info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Debug(fmt, ...) rrc_ptr->rrc_log->debug("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

namespace srsue {

using srslte::proc_outcome_t;
using srslte::tti_point;

/**************************************
 *       Cell Search Procedure
 *************************************/

rrc::cell_search_proc::cell_search_proc(rrc* parent_) : rrc_ptr(parent_) {}

/* Searches for a cell in the current frequency and retrieves SIB1 if not retrieved yet */
proc_outcome_t rrc::cell_search_proc::init()
{
  Info("Starting...\n");
  state = state_t::phy_cell_search;
  rrc_ptr->stack->start_cell_search();
  return proc_outcome_t::yield;
}

/* Implements the SI acquisition procedure. Configures MAC/PHY scheduling to retrieve SI messages.*/
proc_outcome_t rrc::cell_search_proc::step()
{
  switch (state) {
    case state_t::phy_cell_search:
    case state_t::phy_cell_select:
      // Waits for cell select/search to complete
      return proc_outcome_t::yield;
    case state_t::si_acquire:
      return step_si_acquire();
    case state_t::wait_measurement:
      return step_wait_measurement();
  }
  return proc_outcome_t::yield;
}

proc_outcome_t rrc::cell_search_proc::step_si_acquire()
{
  if (not si_acquire_fut.is_complete()) {
    return proc_outcome_t::yield;
  }
  // SI Acquire has completed
  if (si_acquire_fut.is_error()) {
    Error("Failed to trigger SI acquire for SIB0\n");
    return proc_outcome_t::error;
  }
  Info("Completed successfully\n");
  return proc_outcome_t::success;
}

proc_outcome_t rrc::cell_search_proc::handle_cell_found(const phy_interface_rrc_lte::phy_cell_t& new_cell)
{
  Info("Cell found in this frequency. Setting new serving cell EARFCN=%d PCI=%d ...\n", new_cell.earfcn, new_cell.pci);

  // Create a cell with NaN RSRP. Will be updated by new_phy_meas() during SIB search.
  if (not rrc_ptr->add_neighbour_cell(unique_cell_t(new cell_t(new_cell)))) {
    Error("Could not add new found cell\n");
    return proc_outcome_t::error;
  }

  rrc_ptr->set_serving_cell(new_cell, false);

  // set new serving cell in PHY
  state = state_t::phy_cell_select;
  rrc_ptr->stack->start_cell_select(&rrc_ptr->serving_cell->phy_cell);
  return proc_outcome_t::yield;
}

proc_outcome_t rrc::cell_search_proc::step_wait_measurement()
{
  if (not std::isnormal(rrc_ptr->serving_cell->get_rsrp())) {
    return proc_outcome_t::yield;
  }

  if (rrc_ptr->serving_cell->has_sib1()) {
    Info("Cell has SIB1\n");
    // What do we do????
    return proc_outcome_t::success;
  }

  Info("Cell has no SIB1. Obtaining SIB1...\n");
  if (not rrc_ptr->si_acquirer.launch(&si_acquire_fut, 0)) {
    // disallow concurrent si_acquire
    Error("SI Acquire is already running...\n");
    return proc_outcome_t::error;
  }
  state = state_t::si_acquire;
  return step();
}

proc_outcome_t rrc::cell_search_proc::react(const cell_select_event_t& event)
{
  if (state != state_t::phy_cell_select) {
    Warning("Received unexpected cell search result\n");
    return proc_outcome_t::yield;
  }

  if (not event.cs_ret) {
    Error("Couldn't select new serving cell\n");
    return proc_outcome_t::error;
  }

  if (not rrc_ptr->phy->cell_is_camping()) {
    Warning("Could not camp on found cell.\n");
    return proc_outcome_t::error;
  }

  if (not std::isnormal(rrc_ptr->serving_cell->get_rsrp())) {
    Info("No valid measurement found for the serving cell. Wait for valid measurement...\n");
  }
  state = state_t::wait_measurement;
  return proc_outcome_t::yield;
}

proc_outcome_t rrc::cell_search_proc::react(const cell_search_event_t& event)
{
  if (state != state_t::phy_cell_search) {
    Error("Received unexpected cell search result\n");
    return proc_outcome_t::error;
  }
  search_result = event;

  Info("PHY cell search completed.\n");
  // Transition to SI Acquire or finish
  switch (search_result.cs_ret.found) {
    case phy_interface_rrc_lte::cell_search_ret_t::CELL_FOUND:
      return handle_cell_found(search_result.found_cell);
    case phy_interface_rrc_lte::cell_search_ret_t::CELL_NOT_FOUND:
      rrc_ptr->phy_sync_state = phy_unknown_sync;
      Info("No cells found.\n");
      // do nothing
      return proc_outcome_t::success;
    case phy_interface_rrc_lte::cell_search_ret_t::ERROR:
      Error("Error while performing cell search\n");
      // TODO: check what errors can happen (currently not handled in our code)
      return proc_outcome_t::error;
  }
  return proc_outcome_t::yield;
}

/****************************************************************
 * TS 36.331 Sec 5.2.3 - Acquisition of an SI message procedure
 ***************************************************************/

// Helper functions
const uint32_t sib1_periodicity = 20;

/**
 * compute "T" (aka si-Periodicity) and "n" (order of entry in schedulingInfoList).
 * @param sib_index SI index of interest
 * @param sib1 configuration of SIB1
 * @return {T, n} if successful, {0, -1} if sib_index was not found
 */
std::pair<uint32_t, int32_t> compute_si_periodicity_and_idx(uint32_t sib_index, const asn1::rrc::sib_type1_s* sib1)
{
  if (sib_index == 0) {
    return {sib1_periodicity, 0};
  }
  if (sib_index == 1) {
    // SIB2 scheduling
    return {sib1->sched_info_list[0].si_periodicity.to_number(), 0};
  }
  // SIB3+ scheduling Section 5.2.3
  for (uint32_t i = 0; i < sib1->sched_info_list.size(); ++i) {
    for (uint32_t j = 0; j < sib1->sched_info_list[i].sib_map_info.size(); ++j) {
      if (sib1->sched_info_list[i].sib_map_info[j].to_number() == sib_index + 1) {
        return {sib1->sched_info_list[i].si_periodicity.to_number(), i};
      }
    }
  }
  return {0, -1};
}

/**
 * Determine the start TTI of SI-window (see TS 36.331 Sec 5.2.3)
 * @param tti    current TTI
 * @param T      Parameter "T" representing a SIB si-Periodicity
 * @param offset frame offset for the start of SI-window
 * @param a      subframe when SI-window starts
 * @return       next TTI when SI-window starts
 */
uint32_t sib_start_tti(uint32_t tti, uint32_t T, uint32_t offset, uint32_t a)
{
  return (T * 10 * (1 + tti / (T * 10)) + (offset * 10) + a) % 10240; // the 1 means next opportunity
}

/**
 * Determine SI-window [start, length] (see TS 36.331 Sec 5.2.3)
 * @param sib_index index of SI-message of interest
 * @return
 */
std::pair<uint32_t, uint32_t>
compute_si_window(uint32_t tti, uint32_t sib_index, uint32_t n, uint32_t T, const asn1::rrc::sib_type1_s* sib1)
{
  uint32_t si_win_start;
  uint32_t si_win_len; // si-WindowLength or "w"
  if (sib_index == 0) {
    si_win_len   = 1;
    si_win_start = sib_start_tti(tti, 2, 0, 5);
  } else {
    si_win_len      = sib1->si_win_len.to_number();
    uint32_t x      = n * si_win_len;
    uint32_t a      = x % 10; // subframe #a when the SI-window starts
    uint32_t offset = x / 10; // frame offset
    si_win_start    = sib_start_tti(tti, T, offset, a);
  }
  return {si_win_start, si_win_len};
}

// SI Acquire class

rrc::si_acquire_proc::si_acquire_proc(rrc* parent_) :
  rrc_ptr(parent_),
  log_h(srslte::logmap::get("RRC")),
  si_acq_timeout(rrc_ptr->stack->get_unique_timer()),
  si_acq_retry_timer(rrc_ptr->stack->get_unique_timer())
{
  // SIB acquisition procedure timeout.
  // NOTE: The standard does not specify this timeout
  si_acq_timeout.set(SIB_SEARCH_TIMEOUT_MS,
                     [this](uint32_t tid) { rrc_ptr->si_acquirer.trigger(si_acq_timer_expired{tid}); });
  // Sets the callback. The retry period will change for every run
  si_acq_retry_timer.set(1, [this](uint32_t tid) { rrc_ptr->si_acquirer.trigger(si_acq_timer_expired{tid}); });
}

proc_outcome_t rrc::si_acquire_proc::init(uint32_t sib_index_)
{
  // make sure we dont already have the SIB of interest
  if (rrc_ptr->serving_cell->has_sib(sib_index_)) {
    Info("The UE has already acquired SIB%d\n", sib_index + 1);
    return proc_outcome_t::success;
  }
  Info("Starting SI Acquisition procedure for SIB%d\n", sib_index_ + 1);

  // make sure SIB1 is captured before other SIBs
  sib_index = sib_index_;
  if (sib_index > 0 and not rrc_ptr->serving_cell->has_sib1()) {
    Error("Trying to acquire SIB%d but SIB1 not received yet\n", sib_index + 1);
    return proc_outcome_t::error;
  }

  // compute the si-Periodicity and schedInfoList index
  auto ret = compute_si_periodicity_and_idx(sib_index, rrc_ptr->serving_cell->sib1ptr());
  if (ret.second < 0) {
    Info("Could not find SIB%d scheduling in SIB1\n", sib_index + 1);
    return proc_outcome_t::error;
  }
  period      = ret.first;  // si-Periodicity
  sched_index = ret.second; // order index of SI in schedInfoList

  // trigger new SI acquisition procedure in MAC
  start_si_acquire();

  // start timeout timer
  si_acq_timeout.run();

  return proc_outcome_t::yield;
}

void rrc::si_acquire_proc::then(const srslte::proc_state_t& result)
{
  // make sure timers are stopped
  si_acq_retry_timer.stop();
  si_acq_timeout.stop();

  if (result.is_success()) {
    Info("SIB%d acquired successfully\n", sib_index + 1);
  } else {
    Error("Failed to acquire SIB%d\n", sib_index + 1);
  }
}

void rrc::si_acquire_proc::start_si_acquire()
{
  const uint32_t nof_sib_harq_retxs = 5;

  // Instruct MAC to decode SIB (non-blocking)
  tti_point tti = rrc_ptr->stack->get_current_tti();
  auto      ret = compute_si_window(tti.to_uint(), sib_index, sched_index, period, rrc_ptr->serving_cell->sib1ptr());
  tti_point si_win_start = tti_point{ret.first};
  if (si_win_start < tti) {
    Error("The SI Window start was incorrectly calculated. si_win_start=%d, tti=%d\n",
          si_win_start.to_uint(),
          tti.to_uint());
    return;
  }
  uint32_t si_win_len = ret.second;
  rrc_ptr->mac->bcch_start_rx(si_win_start.to_uint(), si_win_len);

  // start window retry timer
  uint32_t retry_period            = (sib_index == 0) ? sib1_periodicity : period * nof_sib_harq_retxs;
  int      tics_until_si_win_start = si_win_start - tti;
  uint32_t tics_until_si_retry     = retry_period + tics_until_si_win_start;
  si_acq_retry_timer.set(tics_until_si_retry);
  si_acq_retry_timer.run();

  Info("Instructed MAC to search for SIB%d, win_start=%d, win_len=%d, period=%d, sched_index=%d\n",
       sib_index + 1,
       si_win_start.to_uint(),
       si_win_len,
       period,
       sched_index);
}

proc_outcome_t rrc::si_acquire_proc::react(sib_received_ev ev)
{
  return rrc_ptr->serving_cell->has_sib(sib_index) ? proc_outcome_t::success : proc_outcome_t::yield;
}

proc_outcome_t rrc::si_acquire_proc::react(si_acq_timer_expired ev)
{
  if (rrc_ptr->serving_cell->has_sib(sib_index)) {
    return proc_outcome_t::success;
  }

  // retry si acquire
  if (ev.timer_id == si_acq_retry_timer.id()) {
    Info("SI Acquire Retry Timeout for SIB%d\n", sib_index + 1);
    start_si_acquire();
    return proc_outcome_t::yield;
  }

  // timeout. SI acquire failed
  if (ev.timer_id == si_acq_timeout.id()) {
    Error("Timeout while acquiring SIB%d\n", sib_index + 1);
  } else {
    Error("Unrecognized timer id\n");
  }
  return proc_outcome_t::error;
}

/**************************************
 *    Serving Cell Config Procedure
 *************************************/

rrc::serving_cell_config_proc::serving_cell_config_proc(rrc* parent_) :
  rrc_ptr(parent_),
  log_h(srslte::logmap::get("RRC"))
{
}

/*
 * Retrieves all required SIB or configures them if already retrieved before
 */
proc_outcome_t rrc::serving_cell_config_proc::init(const std::vector<uint32_t>& required_sibs_)
{
  required_sibs = required_sibs_;

  Info("Starting a Serving Cell Configuration Procedure\n");

  if (not rrc_ptr->phy->cell_is_camping()) {
    Error("Trying to configure Cell while not camping on it\n");
    return proc_outcome_t::error;
  }

  rrc_ptr->serving_cell->has_mcch = false;

  req_idx = 0;
  return launch_sib_acquire();
}

srslte::proc_outcome_t rrc::serving_cell_config_proc::launch_sib_acquire()
{
  // Obtain the SIBs if not available or apply the configuration if available
  for (; req_idx < required_sibs.size(); req_idx++) {
    uint32_t required_sib = required_sibs[req_idx];
    if (not rrc_ptr->serving_cell->has_sib(required_sib)) {
      if (required_sib < 2 or rrc_ptr->serving_cell->is_sib_scheduled(required_sib)) {
        Info("Cell has no SIB%d. Obtaining SIB%d\n", required_sib + 1, required_sib + 1);
        if (not rrc_ptr->si_acquirer.launch(&si_acquire_fut, required_sib)) {
          Error("SI Acquire is already running...\n");
          return proc_outcome_t::error;
        }
        // wait for si acquire to finish
        return proc_outcome_t::yield;
      }
      // SIB is not scheduled in SchedInfoList. Skipping it...
    } else {
      // UE had SIB already. Handle its SIB
      Info("Cell has SIB%d\n", required_sib + 1);
      switch (required_sib) {
        case 1:
          rrc_ptr->handle_sib2();
          break;
        case 12:
          rrc_ptr->handle_sib13();
          break;
        default:
          break;
      }
    }
  }

  Info("Serving Cell Configuration Procedure has finished successfully\n");
  return proc_outcome_t::success;
}

proc_outcome_t rrc::serving_cell_config_proc::step()
{
  if (not si_acquire_fut.is_complete()) {
    return proc_outcome_t::yield;
  }
  uint32_t required_sib = required_sibs[req_idx];
  if (si_acquire_fut.is_error() or not rrc_ptr->serving_cell->has_sib(required_sib)) {
    if (required_sib < 2) {
      log_h->warning("Serving Cell Configuration has failed\n");
      return proc_outcome_t::error;
    }
  }
  // continue with remaining SIBs
  req_idx++;
  return launch_sib_acquire();
}

/**************************************
 *       Cell Selection Procedure
 *************************************/

rrc::cell_selection_proc::cell_selection_proc(rrc* parent_) : rrc_ptr(parent_) {}

/*
 * Cell selection procedure 36.304 5.2.3
 * Select the best cell to camp on among the list of known cells
 */
proc_outcome_t rrc::cell_selection_proc::init()
{
  if (rrc_ptr->neighbour_cells.empty() and rrc_ptr->phy_sync_state == phy_in_sync and rrc_ptr->phy->cell_is_camping()) {
    // don't bother with cell selection if there are no neighbours and we are already camping
    Debug("Skipping Cell Selection Procedure as there are no neighbour and cell is camping.\n");
    cs_result = cs_result_t::same_cell;
    return proc_outcome_t::success;
  }

  Info("Starting...\n");
  Info("Current neighbor cells: [%s]\n", rrc_ptr->print_neighbour_cells().c_str());
  neigh_index     = 0;
  cs_result       = cs_result_t::no_cell;
  state           = search_state_t::cell_selection;
  discard_serving = false;
  return start_cell_selection();
}

proc_outcome_t rrc::cell_selection_proc::react(const cell_select_event_t& event)
{
  switch (state) {
    case search_state_t::cell_selection: {
      return step_cell_selection(event);
    }
    case search_state_t::serv_cell_camp: {
      return step_serv_cell_camp(event);
    }
    case search_state_t::cell_search:
      // cell search may call cell_select
      break;
    default:
      Warning("Unexpected cell selection event received\n");
  }
  return proc_outcome_t::yield;
}

proc_outcome_t rrc::cell_selection_proc::start_cell_selection()
{
  Info("Current serving cell: %s\n", rrc_ptr->serving_cell->to_string().c_str());

  // Neighbour cells are sorted in descending order of RSRP
  for (; neigh_index < rrc_ptr->neighbour_cells.size(); ++neigh_index) {
    /*TODO: CHECK that PLMN matches. Currently we don't receive SIB1 of neighbour cells
     * neighbour_cells[i]->plmn_equals(selected_plmn_id) && */
    // Matches S criteria
    float rsrp = rrc_ptr->neighbour_cells.at(neigh_index)->get_rsrp();

    if (rrc_ptr->phy_sync_state != phy_in_sync or
        (rrc_ptr->cell_selection_criteria(rsrp) and rsrp > rrc_ptr->serving_cell->get_rsrp() + 5)) {
      // currently connected and verifies cell selection criteria
      // Try to select Cell
      rrc_ptr->set_serving_cell(rrc_ptr->neighbour_cells.at(neigh_index)->phy_cell, discard_serving);
      discard_serving = false;
      Info("Selected cell: %s\n", rrc_ptr->serving_cell->to_string().c_str());

      state = search_state_t::cell_selection;
      rrc_ptr->stack->start_cell_select(&rrc_ptr->serving_cell->phy_cell);
      return proc_outcome_t::yield;
    }
  }
  // Iteration over neighbor cells is over.

  if (rrc_ptr->phy_sync_state == phy_in_sync && rrc_ptr->cell_selection_criteria(rrc_ptr->serving_cell->get_rsrp())) {
    if (not rrc_ptr->phy->cell_is_camping()) {
      Info("Serving cell %s is in-sync but not camping. Selecting it...\n", rrc_ptr->serving_cell->to_string().c_str());

      state = search_state_t::serv_cell_camp;
      rrc_ptr->stack->start_cell_select(&rrc_ptr->serving_cell->phy_cell);
      return proc_outcome_t::yield;
    }
    cs_result = cs_result_t::same_cell;
    return proc_outcome_t::success;
  }

  // If can not find any suitable cell, search again
  Info("Cell selection and reselection in IDLE did not find any suitable cell. Searching again\n");
  if (not rrc_ptr->cell_searcher.launch(&cell_search_fut)) {
    return proc_outcome_t::error;
  }
  state = search_state_t::cell_search;
  return step();
}

proc_outcome_t rrc::cell_selection_proc::step_cell_selection(const cell_select_event_t& event)
{
  rrc_ptr->phy_sync_state = phy_unknown_sync;

  if (event.cs_ret) {
    // successful selection
    Info("Wait PHY to be in-synch\n");
    state = search_state_t::wait_in_sync;
    return step();
  }

  Error("Could not camp on serving cell.\n");
  discard_serving = true;
  // Continue to next neighbour cell
  ++neigh_index;
  return start_cell_selection();
}

srslte::proc_outcome_t rrc::cell_selection_proc::step_serv_cell_camp(const cell_select_event_t& event)
{
  // if we are now camping, the proc was successful
  if (event.cs_ret) {
    Info("Selected serving cell OK.\n");
    cs_result = cs_result_t::same_cell;
  } else {
    rrc_ptr->phy_sync_state = phy_unknown_sync;
    rrc_ptr->serving_cell->set_rsrp(-INFINITY);
    Error("Could not camp on serving cell.\n");
  }

  return event.cs_ret ? proc_outcome_t::success : proc_outcome_t::error;
}

proc_outcome_t rrc::cell_selection_proc::step_wait_in_sync()
{
  if (rrc_ptr->phy_sync_state == phy_in_sync) {
    if (rrc_ptr->cell_selection_criteria(rrc_ptr->serving_cell->get_rsrp())) {
      Info("PHY is in SYNC and cell selection passed\n");
      if (not rrc_ptr->serv_cell_cfg.launch(&serv_cell_cfg_fut, rrc_ptr->ue_required_sibs)) {
        return proc_outcome_t::error;
      }
      state = search_state_t::cell_config;
    } else {
      Info("PHY is in SYNC but cell selection did not pass. Go back to select step.\n");
      cs_result       = cs_result_t::no_cell;
      neigh_index     = 0;    // TODO: go back to the start?
      discard_serving = true; // Discard this cell
      return start_cell_selection();
    }
  }
  return proc_outcome_t::yield;
}

proc_outcome_t rrc::cell_selection_proc::step_cell_search()
{
  if (rrc_ptr->cell_searcher.run()) {
    return proc_outcome_t::yield;
  }
  if (cell_search_fut.is_error()) {
    cs_result = cs_result_t::no_cell;
    return proc_outcome_t::error;
  }
  cs_result = (cell_search_fut.value()->found == phy_interface_rrc_lte::cell_search_ret_t::CELL_FOUND)
                  ? cs_result_t::changed_cell
                  : cs_result_t::no_cell;
  Info("Cell Search of cell selection run successfully\n");
  return proc_outcome_t::success;
}

proc_outcome_t rrc::cell_selection_proc::step_cell_config()
{
  if (rrc_ptr->serv_cell_cfg.run()) {
    return proc_outcome_t::yield;
  }
  if (serv_cell_cfg_fut.is_success()) {
    rrc_ptr->rrc_log->console("Selected cell: %s\n", rrc_ptr->serving_cell->to_string().c_str());
    Info("All SIBs of serving cell obtained successfully\n");
    cs_result = cs_result_t::changed_cell;
    return proc_outcome_t::success;
  }
  Error("While configuring serving cell\n");
  // resume cell selection
  ++neigh_index;
  return start_cell_selection();
}

proc_outcome_t rrc::cell_selection_proc::step()
{
  switch (state) {
    case search_state_t::cell_selection:
      // this state waits for phy event
      return proc_outcome_t::yield;
    case search_state_t::serv_cell_camp:
      // this state waits for phy event
      return proc_outcome_t::yield;
    case search_state_t::wait_in_sync:
      return step_wait_in_sync();
    case search_state_t::cell_config:
      return step_cell_config();
    case search_state_t::cell_search:
      return step_cell_search();
  }
  return proc_outcome_t::error;
}

void rrc::cell_selection_proc::then(const srslte::proc_result_t<cs_result_t>& proc_result) const
{
  // Inform Connection Request Procedure
  Info("Completed with %s. Informing proc %s\n",
       proc_result.is_success() ? "success" : "failure",
       rrc_ptr->conn_req_proc.get()->name());
  rrc_ptr->conn_req_proc.trigger(proc_result);
}

/**************************************
 *       PLMN search Procedure
 *************************************/

rrc::plmn_search_proc::plmn_search_proc(rrc* parent_) : rrc_ptr(parent_), log_h(srslte::logmap::get("RRC")) {}

proc_outcome_t rrc::plmn_search_proc::init()
{
  Info("Starting PLMN search\n");
  nof_plmns       = 0;
  cell_search_fut = rrc_ptr->cell_searcher.get_future();
  if (not rrc_ptr->cell_searcher.launch(&cell_search_fut)) {
    Error("Failed due to fail to init cell search...\n");
    return proc_outcome_t::error;
  }
  return step();
}

/* NAS interface to search for available PLMNs.
 * It goes through all known frequencies, synchronizes and receives SIB1 for each to extract PLMN.
 * The function is blocking and waits until all frequencies have been
 * searched and PLMNs are obtained.
 */
proc_outcome_t rrc::plmn_search_proc::step()
{
  if (rrc_ptr->cell_searcher.run()) {
    // wait for new TTI
    return proc_outcome_t::yield;
  }
  if (cell_search_fut.is_error() or cell_search_fut.value()->found == phy_interface_rrc_lte::cell_search_ret_t::ERROR) {
    // stop search
    nof_plmns = -1;
    Error("Failed due to failed cell search sub-procedure\n");
    return proc_outcome_t::error;
  }

  if (cell_search_fut.value()->found == phy_interface_rrc_lte::cell_search_ret_t::CELL_FOUND) {
    if (rrc_ptr->serving_cell->has_sib1()) {
      // Save PLMN and TAC to NAS
      for (uint32_t i = 0; i < rrc_ptr->serving_cell->nof_plmns(); i++) {
        if (nof_plmns < MAX_FOUND_PLMNS) {
          found_plmns[nof_plmns].plmn_id = rrc_ptr->serving_cell->get_plmn(i);
          found_plmns[nof_plmns].tac     = rrc_ptr->serving_cell->get_tac();
          nof_plmns++;
        } else {
          Error("No more space for plmns (%d)\n", nof_plmns);
        }
      }
    } else {
      Error("SIB1 not acquired\n");
    }
  }

  if (cell_search_fut.value()->last_freq == phy_interface_rrc_lte::cell_search_ret_t::NO_MORE_FREQS) {
    Info("completed PLMN search\n");
    return proc_outcome_t::success;
  }

  if (not rrc_ptr->cell_searcher.launch(&cell_search_fut)) {
    Error("Failed due to fail to init cell search...\n");
    return proc_outcome_t::error;
  }

  // run again
  return step();
}

void rrc::plmn_search_proc::then(const srslte::proc_state_t& result) const
{
  // on cleanup, call plmn_search_completed
  if (result.is_success()) {
    Info("completed with success\n");
    rrc_ptr->nas->plmn_search_completed(found_plmns, nof_plmns);
  } else {
    Error("PLMN Search completed with an error\n");
    rrc_ptr->nas->plmn_search_completed(nullptr, -1);
  }
}

/**************************************
 *     Connection Request Procedure
 *************************************/

rrc::connection_request_proc::connection_request_proc(rrc* parent_) :
  rrc_ptr(parent_),
  log_h(srslte::logmap::get("RRC"))
{
}

proc_outcome_t rrc::connection_request_proc::init(srslte::establishment_cause_t cause_,
                                                  srslte::unique_byte_buffer_t  dedicated_info_nas_)
{
  cause              = cause_;
  dedicated_info_nas = std::move(dedicated_info_nas_);

  if (!rrc_ptr->plmn_is_selected) {
    Error("Trying to connect but PLMN not selected.\n");
    return proc_outcome_t::error;
  }

  if (rrc_ptr->state != RRC_STATE_IDLE) {
    log_h->warning("Requested RRC connection establishment while not in IDLE\n");
    return proc_outcome_t::error;
  }

  if (rrc_ptr->t302.is_running()) {
    Info("Requested RRC connection establishment while T302 is running\n");
    rrc_ptr->nas->set_barring(srslte::barring_t::mo_data);
    return proc_outcome_t::error;
  }

  Info("Initiation of Connection establishment procedure\n");

  cs_ret = cs_result_t::no_cell;

  state = state_t::cell_selection;
  if (rrc_ptr->cell_selector.is_idle()) {
    // No one is running cell selection
    if (not rrc_ptr->cell_selector.launch()) {
      Error("Failed to initiate cell selection procedure...\n");
      return proc_outcome_t::error;
    }
    rrc_ptr->callback_list.add_proc(rrc_ptr->cell_selector);
  } else {
    Info("Cell selection proc already on-going. Wait for its result\n");
  }
  return proc_outcome_t::yield;
}

proc_outcome_t rrc::connection_request_proc::step()
{
  if (state == state_t::cell_selection) {
    // NOTE: cell selection will signal back with an event trigger
    return proc_outcome_t::yield;
  } else if (state == state_t::config_serving_cell) {
    if (rrc_ptr->serv_cell_cfg.run()) {
      return proc_outcome_t::yield;
    }
    if (serv_cfg_fut.is_error()) {
      Error("Configuring serving cell\n");
      return proc_outcome_t::error;
    }

    rrc_ptr->t300.run();

    // Send connectionRequest message to lower layers
    rrc_ptr->send_con_request(cause);

    // Save dedicatedInfoNAS SDU, if needed
    if (dedicated_info_nas.get()) {
      if (rrc_ptr->dedicated_info_nas.get()) {
        Warning("Received a new dedicatedInfoNAS SDU but there was one still in queue. Removing it.\n");
        rrc_ptr->dedicated_info_nas.reset();
      }

      Debug("Updating dedicatedInfoNAS in RRC\n");
      rrc_ptr->dedicated_info_nas = std::move(dedicated_info_nas);
    } else {
      Debug("dedicatedInfoNAS has already been provided to RRC.\n");
    }

    Info("Waiting for RRCConnectionSetup/Reject or expiry\n");
    state = state_t::wait_t300;
    return step();

  } else if (state == state_t::wait_t300) {
    // Wait until t300 stops due to RRCConnectionSetup/Reject or expiry
    if (rrc_ptr->t300.is_running()) {
      return proc_outcome_t::yield;
    }

    if (rrc_ptr->state == RRC_STATE_CONNECTED) {
      // Received ConnectionSetup
      return proc_outcome_t::success;
    } else if (rrc_ptr->t300.is_expired()) {
      // T300 is expired: 5.3.3.6
      Warning("Timer T300 expired: ConnectionRequest timed out\n");
      rrc_ptr->mac->reset();
      rrc_ptr->set_mac_default();
      rrc_ptr->rlc->reestablish();
    } else {
      // T300 is stopped but RRC not Connected is because received Reject: Section 5.3.3.8
      Warning("Timer T300 stopped: Received ConnectionReject\n");
      rrc_ptr->mac->reset();
      rrc_ptr->set_mac_default();
    }
  }

  return proc_outcome_t::error;
}

void rrc::connection_request_proc::then(const srslte::proc_state_t& result)
{
  if (result.is_error()) {
    log_h->warning("Could not establish connection. Deallocating dedicatedInfoNAS PDU\n");
    dedicated_info_nas.reset();
    rrc_ptr->dedicated_info_nas.reset();
  } else {
    Info("Finished connection request procedure successfully.\n");
  }
  rrc_ptr->nas->connection_request_completed(result.is_success());
}

srslte::proc_outcome_t rrc::connection_request_proc::react(const cell_selection_proc::cell_selection_complete_ev& e)
{
  if (state != state_t::cell_selection) {
    // ignore if we are not expecting an cell selection result
    return proc_outcome_t::yield;
  }
  if (e.is_error()) {
    return proc_outcome_t::error;
  }
  cs_ret = *e.value();
  // .. and SI acquisition
  if (rrc_ptr->phy->cell_is_camping()) {

    // Set default configurations
    rrc_ptr->set_phy_default();
    rrc_ptr->set_mac_default();

    // CCCH configuration applied already at start
    // timeAlignmentCommon applied in configure_serving_cell

    Info("Configuring serving cell...\n");
    if (not rrc_ptr->serv_cell_cfg.launch(&serv_cfg_fut, rrc_ptr->ue_required_sibs)) {
      Error("Attach request failed to configure serving cell...\n");
      return proc_outcome_t::error;
    }
    state = state_t::config_serving_cell;
    return step();
  } else {
    switch (cs_ret) {
      case cs_result_t::same_cell:
        log_h->warning("Did not reselect cell but serving cell is out-of-sync.\n");
        rrc_ptr->phy_sync_state = phy_unknown_sync;
        break;
      case cs_result_t::changed_cell:
        log_h->warning("Selected a new cell but could not camp on. Setting out-of-sync.\n");
        rrc_ptr->phy_sync_state = phy_unknown_sync;
        break;
      default:
        log_h->warning("Could not find any suitable cell to connect\n");
    }
    return proc_outcome_t::error;
  }
}

/**************************************
 *     Process PCCH procedure
 *************************************/

rrc::process_pcch_proc::process_pcch_proc(srsue::rrc* parent_) : rrc_ptr(parent_), log_h(srslte::logmap::get("RRC")) {}

proc_outcome_t rrc::process_pcch_proc::init(const asn1::rrc::paging_s& paging_)
{
  paging = paging_;

  paging_idx = 0;
  state      = state_t::next_record;
  Info("starting...\n");
  return step();
}

proc_outcome_t rrc::process_pcch_proc::step()
{
  if (state == state_t::next_record) {
    for (; paging_idx < paging.paging_record_list.size(); ++paging_idx) {
      srslte::s_tmsi_t s_tmsi_paged = srslte::make_s_tmsi_t(paging.paging_record_list[paging_idx].ue_id.s_tmsi());
      Info("Received paging (%d/%d) for UE %" PRIu64 ":%" PRIu64 "\n",
           paging_idx + 1,
           paging.paging_record_list.size(),
           paging.paging_record_list[paging_idx].ue_id.s_tmsi().mmec.to_number(),
           paging.paging_record_list[paging_idx].ue_id.s_tmsi().m_tmsi.to_number());
      if (rrc_ptr->ue_identity == s_tmsi_paged) {
        if (RRC_STATE_IDLE == rrc_ptr->state) {
          Info("S-TMSI match in paging message\n");
          log_h->console("S-TMSI match in paging message\n");
          if (not rrc_ptr->nas->paging(&s_tmsi_paged)) {
            Error("Unable to start NAS paging proc\n");
            return proc_outcome_t::error;
          }
          state = state_t::nas_paging;
          return step();
        } else {
          Warning("Received paging while in CONNECT\n");
        }
      } else {
        Info("Received paging for unknown identity\n");
      }
    }
    if (paging.sys_info_mod_present) {
      Info("Received System Information notification update request.\n");
      // invalidate and then update all SIBs of serving cell
      rrc_ptr->serving_cell->reset_sibs();

      // create a serving cell config procedure and push it to callback list
      if (not rrc_ptr->serv_cell_cfg.launch(&serv_cfg_fut, rrc_ptr->ue_required_sibs)) {
        Error("Failed to initiate a serving cell configuration procedure\n");
        return proc_outcome_t::error;
      }
    } else {
      Info("Completed successfully\n");
      return proc_outcome_t::success;
    }
    state = state_t::serv_cell_cfg;
    return step();
  } else if (state == state_t::nas_paging) {
    // wait for trigger
    return proc_outcome_t::yield;
  } else if (state == state_t::serv_cell_cfg) {
    if (rrc_ptr->serv_cell_cfg.run()) {
      return proc_outcome_t::yield;
    }
    if (serv_cfg_fut.is_success()) {
      Info("All SIBs of serving cell obtained successfully\n");
      return proc_outcome_t::success;
    } else {
      Error("While obtaining SIBs of serving cell\n");
      return proc_outcome_t::error;
    }
  }
  return proc_outcome_t::yield;
}

proc_outcome_t rrc::process_pcch_proc::react(paging_complete e)
{
  if (state != state_t::nas_paging) {
    Warning("Received an unexpected paging complete\n");
    return proc_outcome_t::yield;
  }
  if (not e.outcome) {
    Info("NAS Paging has failed\n");
    return proc_outcome_t::error;
  }
  paging_idx++;
  state = state_t::next_record;
  Info("Received paging complete event\n");
  return step();
}

/**************************************
 *        Go Idle procedure
 *************************************/

rrc::go_idle_proc::go_idle_proc(srsue::rrc* rrc_) : rrc_ptr(rrc_)
{
  rlc_flush_timer = rrc_ptr->stack->get_unique_timer();
  rlc_flush_timer.set(rlc_flush_timeout, [this](uint32_t tid) { rrc_ptr->idle_setter.trigger(true); });
}

proc_outcome_t rrc::go_idle_proc::init()
{
  Info("Starting...\n");
  rlc_flush_timer.run();
  // Do not call step() directly. Instead we defer for one TTI to avoid double-locking the RLC mutex
  return proc_outcome_t::yield;
}

srslte::proc_outcome_t rrc::go_idle_proc::react(bool timeout)
{
  rrc_ptr->leave_connected();
  return proc_outcome_t::success;
}

proc_outcome_t rrc::go_idle_proc::step()
{
  if (rrc_ptr->state == RRC_STATE_IDLE) {
    Info("Interrupting as RRC has already became IDLE\n");
    return proc_outcome_t::success;
  }

  // If the RLC SRB1 is not suspended
  // wait for max. 2s for RLC on SRB1 to be flushed
  if (rrc_ptr->rlc->is_suspended(RB_ID_SRB1) || not rrc_ptr->rlc->has_data(RB_ID_SRB1)) {
    rrc_ptr->leave_connected();
    return proc_outcome_t::success;
  } else {
    Debug("Postponing transition to RRC IDLE (%d ms < %d ms)\n", rlc_flush_timer.time_elapsed(), rlc_flush_timeout);
  }
  return proc_outcome_t::yield;
}

/**************************************
 *    Cell Reselection procedure
 *************************************/

rrc::cell_reselection_proc::cell_reselection_proc(srsue::rrc* rrc_) : rrc_ptr(rrc_) {}

proc_outcome_t rrc::cell_reselection_proc::init()
{
  Info("Starting...\n");
  if (not rrc_ptr->cell_selector.launch(&cell_selection_fut)) {
    Error("Failed to initiate a Cell Selection procedure...\n");
    return proc_outcome_t::error;
  }

  return proc_outcome_t::yield;
}

proc_outcome_t rrc::cell_reselection_proc::step()
{
  if (rrc_ptr->cell_selector.run()) {
    return srslte::proc_outcome_t::yield;
  }
  if (cell_selection_fut.is_error()) {
    Error("Error while selecting a cell\n");
    return srslte::proc_outcome_t::error;
  }

  Info("Cell Selection completed. Handling its result...\n");
  switch (*cell_selection_fut.value()) {
    case cs_result_t::changed_cell:
      if (rrc_ptr->state == rrc_state_t::RRC_STATE_IDLE) {
        Info("New cell has been selected, start receiving PCCH\n");
        rrc_ptr->mac->pcch_start_rx();
      }
      break;
    case cs_result_t::no_cell:
      Warning("Could not find any cell to camp on\n");
      break;
    case cs_result_t::same_cell:
      if (!rrc_ptr->phy->cell_is_camping()) {
        Warning("Did not reselect cell but serving cell is out-of-sync.\n");
        rrc_ptr->phy_sync_state = phy_unknown_sync;
      }
      break;
  }
  Info("Finished successfully\n");
  return srslte::proc_outcome_t::success;
}

/**************************************
 *    RRC Connection Re-establishment procedure
 *************************************/

rrc::connection_reest_proc::connection_reest_proc(srsue::rrc* rrc_) : rrc_ptr(rrc_), state(state_t::cell_reselection) {}

proc_outcome_t rrc::connection_reest_proc::init(asn1::rrc::reest_cause_e cause)
{
  Info("Starting... Cause: %s\n",
       cause == asn1::rrc::reest_cause_opts::recfg_fail
           ? "Reconfiguration failure"
           : cause == asn1::rrc::reest_cause_opts::ho_fail ? "Handover failure" : "Other failure");

  // Save Current RNTI before MAC Reset
  mac_interface_rrc::ue_rnti_t uernti;
  rrc_ptr->mac->get_rntis(&uernti);

  // If security is activated, RRC connected and C-RNTI available
  if (rrc_ptr->security_is_activated && rrc_ptr->state == RRC_STATE_CONNECTED && uernti.crnti != 0) {
    // Save reestablishment cause and current C-RNTI
    reest_rnti        = uernti.crnti;
    reest_cause       = cause;
    reest_source_pci  = rrc_ptr->serving_cell->get_pci(); // needed for reestablishment with another cell
    reest_source_freq = rrc_ptr->serving_cell->get_earfcn();

    // the initiation of reestablishment procedure as indicates in 3GPP 36.331 Section 5.3.7.2
    // Cannot be called from here because it has PHY-MAC re-configuration that should be performed in a different thread
    Info("Conditions are met. Initiating RRC Connection Reestablishment Procedure\n");

    // stop timer T310, if running;
    rrc_ptr->t310.stop();

    // start timer T311;
    rrc_ptr->t311.run();

    // Suspend all RB except SRB0
    for (int i = 1; i < SRSLTE_N_RADIO_BEARERS; i++) {
      if (rrc_ptr->rlc->has_bearer(i)) {
        rrc_ptr->rlc->suspend_bearer(i);
      }
    }

    // reset MAC;
    rrc_ptr->mac->reset();

    // apply the default physical channel configuration as specified in 9.2.4;
    rrc_ptr->set_phy_default_pucch_srs();

    // apply the default semi-persistent scheduling configuration as specified in 9.2.3;
    // N.A.

    // apply the default MAC main configuration as specified in 9.2.2;
    rrc_ptr->apply_mac_config_dedicated_default();

    // Launch cell reselection
    if (not rrc_ptr->cell_reselector.launch()) {
      Error("Failed to initiate a Cell re-selection procedure...\n");
      return proc_outcome_t::error;
    }
    state = state_t::cell_reselection;

  } else {
    // 3GPP 36.331 Section 5.3.7.1
    // If AS security has not been activated, the UE does not initiate the procedure but instead
    // moves to RRC_IDLE directly
    Info("Conditions are NOT met\n");
    rrc_ptr->start_go_idle();
    return proc_outcome_t::success;
  }

  return proc_outcome_t::yield;
}

srslte::proc_outcome_t rrc::connection_reest_proc::step_cell_reselection()
{

  // Run cell reselection
  if (not rrc_ptr->cell_reselector.run()) {
    // Check T311
    if (not rrc_ptr->t311.is_running()) {
      // Abort procedure if T311 expires
      Info("T311 expired during cell reselection. Going to IDLE.\n");
      rrc_ptr->start_go_idle();
      return proc_outcome_t::success;
    }

    // Cell reselection finished or not started
    if (rrc_ptr->phy_sync_state == phy_in_sync) {
      // In-sync, check SIBs
      if (rrc_ptr->serving_cell->has_sib1() && rrc_ptr->serving_cell->has_sib2() && rrc_ptr->serving_cell->has_sib3()) {
        Info("In-sync, SIBs available. Going to cell criteria\n");
        return cell_criteria();
      } else {
        Info("SIBs missing (%d, %d, %d), launching serving cell configuration procedure\n",
             rrc_ptr->serving_cell->has_sib1(),
             rrc_ptr->serving_cell->has_sib2(),
             rrc_ptr->serving_cell->has_sib3());
        std::vector<uint32_t> required_sibs = {0, 1, 2};
        if (!rrc_ptr->serv_cell_cfg.launch(required_sibs)) {
          Error("Failed to initiate configure serving cell\n");
          return proc_outcome_t::error;
        }
        state = state_t::cell_configuration;
      }
    } else {
      // Out-of-sync, relaunch reselection
      Info("Serving cell is out-of-sync, re-launching re-selection procedure. T311: %d/%d ms\n",
           rrc_ptr->t311.time_elapsed(),
           rrc_ptr->t311.duration());
      if (!rrc_ptr->cell_reselector.launch()) {
        return proc_outcome_t::error;
      }
    }
  }

  return proc_outcome_t::yield;
}

proc_outcome_t rrc::connection_reest_proc::step_cell_configuration()
{
  if (not rrc_ptr->serv_cell_cfg.run()) {
    // Check T311
    if (!rrc_ptr->t311.is_running()) {
      // Abort procedure if T311 expires
      Info("T311 expired during cell configuration. Going to IDLE.\n");
      rrc_ptr->start_go_idle();
      return proc_outcome_t::success;
    }

    // SIBs adquisition not started or finished
    if (rrc_ptr->phy_sync_state == phy_in_sync) {
      // In-sync
      if (rrc_ptr->serving_cell->has_sib1() && rrc_ptr->serving_cell->has_sib2() && rrc_ptr->serving_cell->has_sib3()) {
        // All SIBs are available
        return cell_criteria();
      } else {
        // Required SIBs are not available
        Error("Failed to configure serving cell\n");
        return proc_outcome_t::error;
      }
    } else {
      // Out-of-sync, relaunch reselection
      Info("Serving cell is out-of-sync, re-launching re-selection procedure\n");
      if (!rrc_ptr->cell_reselector.launch()) {
        Error("Failed to initiate a Cell re-selection procedure...\n");
        return proc_outcome_t::error;
      }
      state = state_t::cell_reselection;
    }
  }

  return proc_outcome_t::yield;
}

srslte::proc_outcome_t rrc::connection_reest_proc::cell_criteria()
{
  // Perform cell selection in accordance to 36.304
  if (rrc_ptr->cell_selection_criteria(rrc_ptr->serving_cell->get_rsrp())) {
    // Actions following cell reselection while T311 is running 5.3.7.3
    // Upon selecting a suitable E-UTRA cell, the UE shall:
    Info("Cell Selection criteria passed after %dms. Sending RRC Connection Reestablishment Request\n",
         rrc_ptr->t311.time_elapsed());

    // stop timer T311;
    rrc_ptr->t311.stop();

    // start timer T301;
    rrc_ptr->t301.run();

    // apply the timeAlignmentTimerCommon included in SystemInformationBlockType2;
    // Not implemented yet.

    // initiate transmission of the RRCConnectionReestablishmentRequest message in accordance with 5.3.7.4;
    rrc_ptr->send_con_restablish_request(reest_cause, reest_rnti, reest_source_pci);
  } else if (rrc_ptr->t311.is_running()) {
    // Upon selecting an inter-RAT cell
    Info("Reestablishment Cell Selection criteria failed.\n");

    // Launch cell reselection
    if (not rrc_ptr->cell_reselector.launch()) {
      Error("Failed to initiate a Cell re-selection procedure...\n");
      return proc_outcome_t::error;
    }
    state = state_t::cell_reselection;
    return proc_outcome_t::yield;
  }
  return proc_outcome_t::success;
}

proc_outcome_t rrc::connection_reest_proc::step()
{
  proc_outcome_t ret = proc_outcome_t::yield;

  /*
   * Implementation of procedure in 3GPP 36.331 Section 5.3.7.3: Actions following cell selection while T311 is
   * running
   */
  switch (state) {
    case state_t::cell_reselection:
      ret = step_cell_reselection();
      break;

    case state_t::cell_configuration:
      ret = step_cell_configuration();
      break;
  }

  return ret;
}

} // namespace srsue
