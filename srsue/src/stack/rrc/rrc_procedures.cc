/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
#include <inttypes.h> // for printing uint64_t

#define Error(fmt, ...) rrc_ptr->rrc_log->error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Warning(fmt, ...) rrc_ptr->rrc_log->warning("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Info(fmt, ...) rrc_ptr->rrc_log->info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Debug(fmt, ...) rrc_ptr->rrc_log->debug("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

namespace srsue {

using srslte::proc_outcome_t;

/**************************************
 *       Cell Search Procedure
 *************************************/

/* Searches for a cell in the current frequency and retrieves SIB1 if not retrieved yet */
proc_outcome_t rrc::cell_search_proc::init(srsue::rrc* parent_)
{
  rrc_ptr = parent_;
  log_h   = parent_->rrc_log;

  Info("Starting...\n");
  state = state_t::phy_cell_search;
  rrc_ptr->stack->start_cell_search();
  return proc_outcome_t::repeat;
}

/* Implements the SI acquisition procedure. Configures MAC/PHY scheduling to retrieve SI messages.*/
proc_outcome_t rrc::cell_search_proc::step()
{
  if (state == state_t::phy_cell_search) {
    // Waits for cell search to complete
    return proc_outcome_t::yield;
  } else if (state == state_t::si_acquire) {
    if (not rrc_ptr->si_acquirer.run()) {
      // SI Acquire has completed
      si_acquire_proc ret = rrc_ptr->si_acquirer.pop();
      if (ret.is_error()) {
        Error("Failed to trigger SI acquire for SIB0\n");
        return proc_outcome_t::error;
      } else {
        //      if(parent->serving_cell->has_sib(0)) {
        //      }
        return proc_outcome_t::success;
      }
    }
  }
  return proc_outcome_t::yield;
}

proc_outcome_t rrc::cell_search_proc::handle_cell_found(const phy_interface_rrc_lte::phy_cell_t& new_cell)
{
  Info("Cell found in this frequency. Setting new serving cell...\n");

  // Create a cell with NaN RSRP. Will be updated by new_phy_meas() during SIB search.
  if (not rrc_ptr->add_neighbour_cell(new_cell, NAN)) {
    Info("No more space for neighbour cells\n");
    return proc_outcome_t::success;
  }

  rrc_ptr->set_serving_cell(new_cell);

  if (not rrc_ptr->phy->cell_is_camping()) {
    log_h->warning("Could not camp on found cell.\n");
    return proc_outcome_t::error;
  }

  if (rrc_ptr->serving_cell->has_sib1()) {
    Info("Cell has SIB1\n");
    // What do we do????
    return proc_outcome_t::success;
  }

  if (not rrc_ptr->si_acquirer.launch(rrc_ptr, 0)) {
    // disallow concurrent si_acquire
    Error("SI Acquire is already running...\n");
    return proc_outcome_t::error;
  }

  // instruct MAC to look for SIB1
  Info("Cell has no SIB1. Obtaining SIB1...\n");
  state = state_t::si_acquire;
  return proc_outcome_t::repeat;
}

proc_outcome_t rrc::cell_search_proc::trigger_event(const cell_search_event_t& event)
{
  if (state != state_t::phy_cell_search) {
    Error("Received unexpected cell search result\n");
    return proc_outcome_t::error;
  }
  search_result = event;

  Info("PHY cell search completed.\n");
  // Transition to SI Acquire or finish
  switch (search_result.cs_ret.found) {
    case phy_interface_rrc_lte::cell_search_ret_t::CELL_FOUND: {
      return handle_cell_found(search_result.found_cell);
    }
    case phy_interface_rrc_lte::cell_search_ret_t::CELL_NOT_FOUND:
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

/**************************************
 *       SI Acquire Procedure
 *************************************/

proc_outcome_t rrc::si_acquire_proc::init(rrc* parent_, uint32_t sib_index_)
{
  rrc_ptr = parent_;
  log_h   = parent_->rrc_log;

  Info("Starting SI Acquire procedure for SIB%d\n", sib_index_ + 1);
  sib_index = sib_index_;
  start_tti = rrc_ptr->mac->get_current_tti();

  // set period/sched_index
  if (sib_index == 0) {
    period      = 20;
    sched_index = 0;
  } else {
    // Instruct MAC to look for SIB2..13
    if (not rrc_ptr->serving_cell->has_sib1()) {
      Error("Trying to acquire SIB%d but SIB1 not received yet\n", sib_index + 1);
      return proc_outcome_t::error;
    }
    asn1::rrc::sib_type1_s* sib1 = rrc_ptr->serving_cell->sib1ptr();

    if (sib_index == 1) {
      // SIB2 scheduling
      period      = sib1->sched_info_list[0].si_periodicity.to_number();
      sched_index = 0;
    } else {
      // SIB3+ scheduling Section 5.2.3
      bool found = false;
      for (uint32_t i = 0; i < sib1->sched_info_list.size() && not found; ++i) {
        for (uint32_t j = 0; j < sib1->sched_info_list[i].sib_map_info.size() && not found; ++j) {
          if (sib1->sched_info_list[i].sib_map_info[j].to_number() == sib_index + 1) {
            period      = sib1->sched_info_list[i].si_periodicity.to_number();
            sched_index = i;
            found       = true;
          }
        }
      }
      if (not found) {
        Info("Could not find SIB%d scheduling in SIB1\n", sib_index + 1);
        return proc_outcome_t::success;
      }
    }
  }

  return proc_outcome_t::repeat;
}

proc_outcome_t rrc::si_acquire_proc::step()
{
  uint32_t tti         = rrc_ptr->mac->get_current_tti();
  uint32_t tti_diff1   = srslte_tti_interval(tti, start_tti);
  bool     has_timeout = tti_diff1 >= SIB_SEARCH_TIMEOUT_MS and tti_diff1 < 10240 / 2;
  if (has_timeout or rrc_ptr->serving_cell->has_sib(sib_index)) {
    if (rrc_ptr->serving_cell->has_sib(sib_index)) {
      Info("SIB%d acquired successfully\n", sib_index + 1);
      return proc_outcome_t::success;
    } else {
      Error("Timeout while acquiring SIB1\n");
      return proc_outcome_t::error;
    }
  } else {

    uint32_t tti_diff2 = srslte_tti_interval(tti, last_win_start);

    // If first time or inside retry window, trigger MAC to decode SIB
    uint32_t retry_period = (sib_index == 0) ? 20 : period * 5;
    if (last_win_start == 0 or (tti_diff2 > retry_period and tti_diff2 < 1000)) {

      // compute win start and len
      uint32_t si_win_start, si_win_len;
      if (sib_index == 0) {
        si_win_len   = 1;
        si_win_start = sib_start_tti(tti, 2, 0, 5);
      } else {
        asn1::rrc::sib_type1_s* sib1 = rrc_ptr->serving_cell->sib1ptr();

        si_win_len  = sib1->si_win_len.to_number();
        uint32_t x  = sched_index * si_win_len;
        uint32_t sf = x % 10, offset = x / 10;
        si_win_start = sib_start_tti(tti, period, offset, sf);
      }
      last_win_start = si_win_start;

      // Instruct MAC to decode SIB
      rrc_ptr->mac->bcch_start_rx(si_win_start, si_win_len);
      Info("Instructed MAC to search for SIB%d, win_start=%d, win_len=%d, period=%d, sched_index=%d\n",
           sib_index + 1,
           si_win_start,
           si_win_len,
           period,
           sched_index);
    }
  }

  return proc_outcome_t::yield;
}

// Determine SI messages scheduling as in 36.331 5.2.3 Acquisition of an SI message
uint32_t rrc::si_acquire_proc::sib_start_tti(uint32_t tti, uint32_t period, uint32_t offset, uint32_t sf)
{
  return (period * 10 * (1 + tti / (period * 10)) + (offset * 10) + sf) % 10240; // the 1 means next opportunity
}

/**************************************
 *    Serving Cell Config Procedure
 *************************************/

/*
 * Retrieves all required SIB or configures them if already retrieved before
 */
proc_outcome_t rrc::serving_cell_config_proc::init(srsue::rrc* parent_, const std::vector<uint32_t>& required_sibs_)
{
  rrc_ptr       = parent_;
  log_h         = parent_->rrc_log;
  required_sibs = required_sibs_;

  Info("Starting a Serving Cell Configuration Procedure\n");

  if (not rrc_ptr->phy->cell_is_camping()) {
    Error("Trying to configure Cell while not camping on it\n");
    return proc_outcome_t::error;
  }

  rrc_ptr->serving_cell->has_mcch = false;

  req_idx      = 0;
  search_state = search_state_t::next_sib;
  return proc_outcome_t::repeat;
}

proc_outcome_t rrc::serving_cell_config_proc::step()
{
  if (search_state == search_state_t::next_sib) {
    // Obtain the SIBs if not available or apply the configuration if available
    for (; req_idx < required_sibs.size(); req_idx++) {
      uint32_t required_sib = required_sibs[req_idx];

      if (not rrc_ptr->serving_cell->has_sib(required_sib)) {
        Info("Cell has no SIB%d. Obtaining SIB%d\n", required_sib + 1, required_sib + 1);
        if (not rrc_ptr->si_acquirer.launch(rrc_ptr, required_sib)) {
          Error("SI Acquire is already running...\n");
          return proc_outcome_t::error;
        }
        search_state = search_state_t::si_acquire;
        return proc_outcome_t::repeat;
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
    if (req_idx == required_sibs.size()) {
      Info("Serving Cell Configuration Procedure has finished successfully\n");
      return proc_outcome_t::success;
    }
  } else if (search_state == search_state_t::si_acquire) {
    uint32_t required_sib = required_sibs[req_idx];
    if (not rrc_ptr->si_acquirer.run()) {
      si_acquire_proc ret = rrc_ptr->si_acquirer.pop();
      if (ret.is_error() or not rrc_ptr->serving_cell->has_sib(required_sib)) {
        if (required_sib < 2) {
          log_h->warning("Serving Cell Configuration has failed\n");
          return proc_outcome_t::error;
        }
      }
      // continue with remaining SIBs
      search_state = search_state_t::next_sib;
      req_idx++;
      return proc_outcome_t::repeat;
    }
  }
  return proc_outcome_t::yield;
}

/**************************************
 *       Cell Selection Procedure
 *************************************/

/*
 * Cell selection procedure 36.304 5.2.3
 * Select the best cell to camp on among the list of known cells
 */
proc_outcome_t rrc::cell_selection_proc::init(srsue::rrc* parent_)
{
  rrc_ptr = parent_;
  log_h   = parent_->rrc_log;

  if (rrc_ptr->neighbour_cells.empty() and rrc_ptr->serving_cell->in_sync and rrc_ptr->phy->cell_is_camping()) {
    // don't bother with cell selection if there are no neighbours and we are already camping
    Debug("Skipping Cell Selection Procedure ..\n");
    return proc_outcome_t::success;
  }

  Info("Starting a Cell Selection Procedure...\n");
  neigh_index = 0;
  cs_result   = cs_result_t::no_cell;
  state       = search_state_t::cell_selection;
  return proc_outcome_t::repeat;
}

proc_outcome_t rrc::cell_selection_proc::step_cell_selection()
{
  // Neighbour cells are sorted in descending order of RSRP
  for (; neigh_index < rrc_ptr->neighbour_cells.size(); ++neigh_index) {
    /*TODO: CHECK that PLMN matches. Currently we don't receive SIB1 of neighbour cells
     * neighbour_cells[i]->plmn_equals(selected_plmn_id) && */
    if (rrc_ptr->neighbour_cells.at(neigh_index)->in_sync) {
      // Matches S criteria
      float rsrp = rrc_ptr->neighbour_cells.at(neigh_index)->get_rsrp();

      if (not rrc_ptr->serving_cell->in_sync or
          (rrc_ptr->cell_selection_criteria(rsrp) and rsrp > rrc_ptr->serving_cell->get_rsrp() + 5)) {
        // currently connected and verifies cell selection criteria
        // Try to select Cell
        rrc_ptr->set_serving_cell(rrc_ptr->neighbour_cells.at(neigh_index)->phy_cell);
        Info(
            "Selected cell PCI=%d, EARFCN=%d\n", rrc_ptr->serving_cell->get_pci(), rrc_ptr->serving_cell->get_earfcn());
        log_h->console(
            "Selected cell PCI=%d, EARFCN=%d\n", rrc_ptr->serving_cell->get_pci(), rrc_ptr->serving_cell->get_earfcn());

        /* BLOCKING CALL */
        if (rrc_ptr->phy->cell_select(&rrc_ptr->serving_cell->phy_cell)) {
          if (not rrc_ptr->serv_cell_cfg.launch(rrc_ptr, rrc_ptr->ue_required_sibs)) {
            return proc_outcome_t::error;
          }
          state = search_state_t::cell_config;
          return proc_outcome_t::repeat;
        } else {
          rrc_ptr->serving_cell->in_sync = false;
          Error("Could not camp on serving cell.\n");
          // Continue to next neighbour cell
        }
      }
    }
  }
  if (rrc_ptr->serving_cell->in_sync) {
    if (not rrc_ptr->phy->cell_is_camping()) {
      Info("Serving cell is in-sync but not camping. Selecting it...\n");

      /* BLOCKING CALL */
      if (rrc_ptr->phy->cell_select(&rrc_ptr->serving_cell->phy_cell)) {
        Info("Selected serving cell OK.\n");
      } else {
        rrc_ptr->serving_cell->in_sync = false;
        Error("Could not camp on serving cell.\n");
      }
    }
    cs_result = cs_result_t::same_cell;
    return proc_outcome_t::success;
  }

  // If can not find any suitable cell, search again
  Info("Cell selection and reselection in IDLE did not find any suitable cell. Searching again\n");
  if (not rrc_ptr->cell_searcher.launch(rrc_ptr)) {
    return proc_outcome_t::error;
  }
  state = search_state_t::cell_search;
  return proc_outcome_t::repeat;
}

proc_outcome_t rrc::cell_selection_proc::step_cell_search()
{

  if (rrc_ptr->cell_searcher.run()) {
    return proc_outcome_t::yield;
  }
  cell_search_proc ret = rrc_ptr->cell_searcher.pop();
  if (ret.is_error()) {
    cs_result = cs_result_t::no_cell;
    return proc_outcome_t::error;
  } else {
    cs_result = (ret.get_cs_ret().found == phy_interface_rrc_lte::cell_search_ret_t::CELL_FOUND)
                    ? cs_result_t::changed_cell
                    : cs_result_t::no_cell;
    Info("Cell Search of cell selection run successfully\n");
    return proc_outcome_t::success;
  }
}

proc_outcome_t rrc::cell_selection_proc::step_cell_config()
{
  if (rrc_ptr->serv_cell_cfg.run()) {
    return proc_outcome_t::yield;
  }
  serving_cell_config_proc ret = rrc_ptr->serv_cell_cfg.pop();
  if (ret.is_success()) {
    Info("All SIBs of serving cell obtained successfully\n");
    cs_result = cs_result_t::changed_cell;
    return proc_outcome_t::success;
  } else {
    Error("While configuring serving cell\n");
    // resume cell selection
    state = search_state_t::cell_selection;
    ++neigh_index;
    return proc_outcome_t::repeat;
  }
}

proc_outcome_t rrc::cell_selection_proc::step()
{
  switch (state) {
    case search_state_t::cell_selection:
      return step_cell_selection();
    case search_state_t::cell_config:
      return step_cell_config();
    case search_state_t::cell_search:
      return step_cell_search();
  }
  return proc_outcome_t::error;
}

void rrc::cell_selection_proc::stop()
{
  // Inform Connection Request Procedure
  Info("Completed with %s. Informing proc %s\n",
       is_success() ? "success" : "failure",
       rrc_ptr->conn_req_proc.get()->name());
  rrc_ptr->conn_req_proc.trigger_event(connection_request_proc::cell_selection_complete{is_success(), cs_result});
}

/**************************************
 *       PLMN search Procedure
 *************************************/

proc_outcome_t rrc::plmn_search_proc::init(srsue::rrc* parent_)
{
  rrc_ptr = parent_;
  log_h   = parent_->rrc_log;

  Info("Starting PLMN search\n");
  nof_plmns = 0;
  if (not rrc_ptr->cell_searcher.launch(rrc_ptr)) {
    Error("Failed due to fail to init cell search...\n");
    return proc_outcome_t::error;
  }
  return proc_outcome_t::repeat;
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
  cell_search_proc                         ret    = rrc_ptr->cell_searcher.pop();
  phy_interface_rrc_lte::cell_search_ret_t cs_ret = ret.get_cs_ret();
  if (ret.is_error() or cs_ret.found == phy_interface_rrc_lte::cell_search_ret_t::ERROR) {
    // stop search
    nof_plmns = -1;
    Error("Failed due to failed cell search sub-procedure\n");
    return proc_outcome_t::error;
  }

  if (cs_ret.found == phy_interface_rrc_lte::cell_search_ret_t::CELL_FOUND) {
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

  if (cs_ret.last_freq == phy_interface_rrc_lte::cell_search_ret_t::NO_MORE_FREQS) {
    Info("completed PLMN search\n");
    return proc_outcome_t::success;
  }

  if (not rrc_ptr->cell_searcher.launch(rrc_ptr)) {
    Error("Failed due to fail to init cell search...\n");
    return proc_outcome_t::error;
  }

  // run again
  return proc_outcome_t::repeat;
}

void rrc::plmn_search_proc::stop()
{
  // on cleanup, call plmn_search_completed
  if (is_success()) {
    Info("completed with success\n");
    rrc_ptr->nas->plmn_search_completed(found_plmns, nof_plmns);
  } else if (is_error()) {
    Error("PLMN Search completed with an error\n");
    rrc_ptr->nas->plmn_search_completed(nullptr, -1);
  }
}

/**************************************
 *     Connection Request Procedure
 *************************************/

proc_outcome_t rrc::connection_request_proc::init(rrc*                          parent_,
                                                  srslte::establishment_cause_t cause_,
                                                  srslte::unique_byte_buffer_t  dedicated_info_nas_)
{
  rrc_ptr            = parent_;
  log_h              = parent_->rrc_log;
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

  if (rrc_ptr->timers->get(rrc_ptr->t302)->is_running()) {
    Info("Requested RRC connection establishment while T302 is running\n");
    rrc_ptr->nas->set_barring(nas_interface_rrc::BARRING_MO_DATA);
    return proc_outcome_t::error;
  }

  Info("Initiation of Connection establishment procedure\n");

  cs_ret = cs_result_t::no_cell;

  state = state_t::cell_selection;
  if (not rrc_ptr->cell_selector.launch(rrc_ptr)) {
    if (not rrc_ptr->cell_selector.is_active()) {
      // Launch failed but cell selection was not already running
      Error("Failed to initiate cell selection procedure...\n");
      return proc_outcome_t::error;
    }
    // In case it was already running, just wait for an cell_selection_complete event trigger
    Info("Cell selection proc already on-going. Wait for its result\n");
  } else {
    // In case we were able to launch it, let the callback list handle it
    rrc_ptr->callback_list.defer_proc(rrc_ptr->cell_selector);
  }
  return proc_outcome_t::repeat;
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
    serving_cell_config_proc ret = rrc_ptr->serv_cell_cfg.pop();

    if (ret.is_error()) {
      Error("Configuring serving cell\n");
      return proc_outcome_t::error;
    }

    rrc_ptr->timers->get(rrc_ptr->t300)->reset();
    rrc_ptr->timers->get(rrc_ptr->t300)->run();

    // Send connectionRequest message to lower layers
    rrc_ptr->send_con_request(cause);

    // Save dedicatedInfoNAS SDU
    if (rrc_ptr->dedicated_info_nas.get()) {
      log_h->warning("Received a new dedicatedInfoNAS SDU but there was one still in queue. Removing it\n");
    }

    Info("Waiting for RRCConnectionSetup/Reject or expiry\n");
    rrc_ptr->dedicated_info_nas = std::move(dedicated_info_nas);
    state                       = state_t::wait_t300;
    return proc_outcome_t::repeat;

  } else if (state == state_t::wait_t300) {
    // Wait until t300 stops due to RRCConnectionSetup/Reject or expiry
    if (rrc_ptr->timers->get(rrc_ptr->t300)->is_running()) {
      return proc_outcome_t::yield;
    }

    if (rrc_ptr->state == RRC_STATE_CONNECTED) {
      // Received ConnectionSetup
      return proc_outcome_t::success;
    } else if (rrc_ptr->timers->get(rrc_ptr->t300)->is_expired()) {
      // T300 is expired: 5.3.3.6
      Info("Timer T300 expired: ConnectionRequest timed out\n");
      rrc_ptr->mac->reset();
      rrc_ptr->set_mac_default();
      rrc_ptr->rlc->reestablish();
    } else {
      // T300 is stopped but RRC not Connected is because received Reject: Section 5.3.3.8
      Info("Timer T300 stopped: Received ConnectionReject\n");
      rrc_ptr->mac->reset();
      rrc_ptr->set_mac_default();
    }
  }

  return proc_outcome_t::error;
}

void rrc::connection_request_proc::stop()
{
  if (is_error()) {
    log_h->warning("Could not establish connection. Deallocating dedicatedInfoNAS PDU\n");
    this->dedicated_info_nas.reset();
    rrc_ptr->nas->connection_request_completed(false);
  } else if (is_success()) {
    Info("Finished connection request procedure successfully.\n");
    rrc_ptr->nas->connection_request_completed(true);
  }
}

srslte::proc_outcome_t rrc::connection_request_proc::trigger_event(const cell_selection_complete& e)
{
  if (state != state_t::cell_selection) {
    // ignore if we are not expecting an cell selection result
    return proc_outcome_t::yield;
  }
  if (not e.is_success) {
    return proc_outcome_t::error;
  }
  cs_ret = e.cs_result;
  // .. and SI acquisition
  if (rrc_ptr->phy->cell_is_camping()) {

    // Set default configurations
    rrc_ptr->set_phy_default();
    rrc_ptr->set_mac_default();

    // CCCH configuration applied already at start
    // timeAlignmentCommon applied in configure_serving_cell

    Info("Configuring serving cell...\n");
    if (not rrc_ptr->serv_cell_cfg.launch(rrc_ptr, rrc_ptr->ue_required_sibs)) {
      Error("Attach request failed to configure serving cell...\n");
      return proc_outcome_t::error;
    }
    state = state_t::config_serving_cell;
    return proc_outcome_t::repeat;
  } else {
    switch (cs_ret) {
      case cs_result_t::same_cell:
        log_h->warning("Did not reselect cell but serving cell is out-of-sync.\n");
        rrc_ptr->serving_cell->in_sync = false;
        break;
      case cs_result_t::changed_cell:
        log_h->warning("Selected a new cell but could not camp on. Setting out-of-sync.\n");
        rrc_ptr->serving_cell->in_sync = false;
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

proc_outcome_t rrc::process_pcch_proc::init(rrc* parent_, const asn1::rrc::paging_s& paging_)
{
  rrc_ptr = parent_;
  log_h   = parent_->rrc_log;
  paging  = paging_;

  paging_idx = 0;
  state      = state_t::next_record;
  Info("starting...\n");
  return proc_outcome_t::repeat;
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
          rrc_ptr->nas->paging(&s_tmsi_paged);
          state = state_t::nas_paging;
          return proc_outcome_t::repeat;
        } else {
          log_h->warning("Received paging while in CONNECT\n");
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
      if (not rrc_ptr->serv_cell_cfg.launch(rrc_ptr, rrc_ptr->ue_required_sibs)) {
        Error("Failed to initiate a serving cell configuration procedure\n");
        return proc_outcome_t::error;
      }
    } else {
      Info("Completed successfully\n");
      return proc_outcome_t::success;
    }
    state = state_t::serv_cell_cfg;
    return proc_outcome_t::repeat;
  } else if (state == state_t::nas_paging) {
    // wait for trigger
    return proc_outcome_t::yield;
  } else if (state == state_t::serv_cell_cfg) {
    if (rrc_ptr->serv_cell_cfg.run()) {
      return proc_outcome_t::yield;
    }
    serving_cell_config_proc ret = rrc_ptr->serv_cell_cfg.pop();
    if (ret.is_success()) {
      Info("All SIBs of serving cell obtained successfully\n");
      return proc_outcome_t::success;
    } else {
      Error("While obtaining SIBs of serving cell\n");
      return proc_outcome_t::error;
    }
  }
  return proc_outcome_t::yield;
}

proc_outcome_t rrc::process_pcch_proc::trigger_event(paging_complete e)
{
  if (not e.outcome) {
    Info("NAS Paging has failed\n");
    return proc_outcome_t::error;
  }
  paging_idx++;
  state = state_t::next_record;
  Info("Received paging complete event\n");
  return proc_outcome_t::repeat;
}

/**************************************
 *        Go Idle procedure
 *************************************/

proc_outcome_t rrc::go_idle_proc::init(rrc* rrc_)
{
  rrc_ptr = rrc_;

  rlc_flush_counter = 0;
  Info("Starting...\n");
  return proc_outcome_t::yield;
}

proc_outcome_t rrc::go_idle_proc::step()
{
  if (rrc_ptr->state == RRC_STATE_IDLE) {
    Info("Interrupting as RRC has already became IDLE\n");
    return proc_outcome_t::success;
  }

  // wait for max. 2s for RLC on SRB1 to be flushed
  if (not rrc_ptr->rlc->has_data(RB_ID_SRB1) || ++rlc_flush_counter > rlc_flush_timeout) {
    rrc_ptr->leave_connected();
    return proc_outcome_t::success;
  } else {
    rrc_ptr->rrc_log->debug(
        "Postponing transition to RRC IDLE (%d ms < %d ms)\n", rlc_flush_counter, rlc_flush_timeout);
  }
  return proc_outcome_t::yield;
}

} // namespace srsue
