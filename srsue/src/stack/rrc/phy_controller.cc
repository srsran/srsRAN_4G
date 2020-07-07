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

#include "srsue/hdr/stack/rrc/phy_controller.h"

namespace srsue {

std::string to_string(const phy_interface_rrc_lte::phy_cell_t& cell)
{
  char buffer[128];
  snprintf(buffer, sizeof(buffer), "{pci=%d, dl_earfcn=%d}", cell.pci, cell.earfcn);
  return buffer;
}

phy_controller::phy_controller(srsue::phy_interface_rrc_lte* phy_, srsue::stack_interface_rrc* stack_) :
  base_t(srslte::log_ref{"RRC"}),
  phy(phy_),
  stack(stack_)
{}

/**************************************
 *    PHY Cell Select Procedure
 *************************************/

bool phy_controller::start_cell_select(const phy_cell_t&                           phy_cell,
                                       const srslte::event_callback<cell_sel_res>& on_complete)
{
  if (not trigger(cell_sel_cmd{phy_cell, on_complete})) {
    log_h->warning("Failed to launch cell selection\n");
    return false;
  }
  return true;
}

phy_controller::selecting_cell::selecting_cell(phy_controller* parent_) : nested_fsm_t(parent_)
{
  wait_in_sync_timer = parent_fsm()->stack->get_unique_timer();
}

void phy_controller::selecting_cell::enter(phy_controller* f, const cell_sel_cmd& ev)
{
  target_cell   = ev.phy_cell;
  csel_callback = ev.callback;
  result        = false;

  f->log_h->info("Starting \"%s\" for pci=%d, earfcn=%d\n",
                 srslte::get_type_name(*this).c_str(),
                 target_cell.pci,
                 target_cell.earfcn);
  f->stack->start_cell_select(&target_cell);
}

void phy_controller::selecting_cell::exit(phy_controller* f)
{
  wait_in_sync_timer.stop();

  if (result) {
    log_h->info("Cell %s successfully selected\n", to_string(target_cell).c_str());
  } else {
    log_h->warning("Failed to select cell %s\n", to_string(target_cell).c_str());
  }

  // Signal result back to FSM that called cell selection
  csel_callback(result);
}

void phy_controller::selecting_cell::wait_in_sync::enter(selecting_cell* f, const cell_sel_res& ev)
{
  f->wait_in_sync_timer.set(wait_sync_timeout_ms, [f](uint32_t tid) { f->trigger(timeout_ev{}); });
  f->wait_in_sync_timer.run();
}

/**************************************
 *    PHY Cell Search Procedure
 *************************************/

//! Searches for a cell in the current frequency and retrieves SIB1 if not retrieved yet
bool phy_controller::start_cell_search(const srslte::event_callback<cell_srch_res>& on_complete)
{
  if (not trigger(on_complete)) {
    log_h->warning("Failed to launch cell search\n");
    return false;
  }
  return true;
}

bool phy_controller::cell_search_completed(cell_search_ret_t cs_ret, phy_cell_t found_cell)
{
  return trigger(cell_srch_res{cs_ret, found_cell});
}

bool phy_controller::cell_selection_completed(bool outcome)
{
  return trigger(outcome);
}

void phy_controller::searching_cell::enter(phy_controller* f, const cell_search_cmd& cmd)
{
  f->log_h->info("Initiated Cell search\n");
  csearch_callbacks.emplace_back(cmd);
  f->stack->start_cell_search();
}

void phy_controller::handle_cell_search_res(searching_cell& s, const cell_srch_res& result)
{
  switch (result.cs_ret.found) {
    case cell_search_ret_t::CELL_FOUND:
      log_h->info("PHY cell search completed. Found cell %s\n", to_string(result.found_cell).c_str());
      break;
    case cell_search_ret_t::CELL_NOT_FOUND:
      log_h->warning("PHY cell search completed. No cells found\n");
      break;
    default:
      log_h->error("Invalid cell search result\n");
      // TODO: check what errors can happen (currently not handled in our code)
  }

  // Signal back completion
  for (auto& f : s.csearch_callbacks) {
    f(result);
  }
  s.csearch_callbacks.clear();
}

void phy_controller::share_cell_search_res(searching_cell& s, const cell_search_cmd& callback)
{
  log_h->info("Cell Search already running. Re-utilizing result.\n");
  s.csearch_callbacks.emplace_back(callback);
}

} // namespace srsue
