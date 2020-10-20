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

std::string to_string(const phy_cell_t& cell)
{
  char buffer[128];
  snprintf(buffer, sizeof(buffer), "{pci=%d, dl_earfcn=%d}", cell.pci, cell.earfcn);
  return buffer;
}

phy_controller::phy_controller(srsue::phy_interface_rrc_lte*                 phy_,
                               srslte::task_sched_handle                     task_sched_,
                               std::function<void(uint32_t, uint32_t, bool)> on_cell_selection) :
  base_t(srslte::log_ref{"RRC"}),
  phy(phy_),
  task_sched(task_sched_),
  cell_selection_always_observer(std::move(on_cell_selection))
{}

void phy_controller::in_sync()
{
  trigger(in_sync_ev{});
}

bool phy_controller::set_cell_config(const srslte::phy_cfg_t& config, uint32_t cc_idx)
{
  log_h->info("Setting PHY config for cc_idx=%d\n", cc_idx);
  return set_cell_config(config, cc_idx, true);
}

void phy_controller::set_phy_to_default()
{
  log_h->info("Setting default PHY config (common and dedicated)\n");

  srslte::phy_cfg_t& default_cfg = current_cells_cfg[0];
  default_cfg.set_defaults();
  for (uint32_t i = 0; i < SRSLTE_MAX_CARRIERS; ++i) {
    set_cell_config(default_cfg, i, false);
  }
}

/// Apply default PHY config for all SCells as specified in TS 36.331 9.2.4
void phy_controller::set_phy_to_default_dedicated()
{
  log_h->info("Setting default dedicated PHY config\n");

  srslte::phy_cfg_t& default_cfg = current_cells_cfg[0];
  default_cfg.set_defaults_dedicated();
  for (uint32_t i = 0; i < SRSLTE_MAX_CARRIERS; ++i) {
    set_cell_config(default_cfg, i, false);
  }
}

void phy_controller::set_phy_to_default_pucch_srs()
{
  log_h->info("Setting default PHY config dedicated\n");

  srslte::phy_cfg_t& default_cfg_ded = current_cells_cfg[0];
  default_cfg_ded.set_defaults_pucch_sr();
  for (uint32_t i = 0; i < SRSLTE_MAX_CARRIERS; ++i) {
    set_cell_config(default_cfg_ded, i, false);
  }
}

bool phy_controller::set_cell_config(const srslte::phy_cfg_t& cfg, uint32_t cc_idx, bool is_set)
{
  if ((is_set or cc_idx == 0 or configured_scell_mask[cc_idx]) and phy->set_config(cfg, cc_idx)) {
    current_cells_cfg[cc_idx] = cfg;
    if (cc_idx > 0) {
      configured_scell_mask[cc_idx] = is_set;
    }
    nof_pending_configs++;
    return true;
  }
  return false;
}

void phy_controller::set_config_complete()
{
  if (nof_pending_configs == 0) {
    log_h->warning("Received more phy config complete signals than the ones scheduled\n");
    return;
  }
  nof_pending_configs--;
}

/**************************************
 *    PHY Cell Select Procedure
 *************************************/

bool phy_controller::start_cell_select(const phy_cell_t& phy_cell, srslte::event_observer<bool> observer)
{
  if (is_in_state<selecting_cell>()) {
    log_h->warning("Failed to launch cell selection as it is already running\n");
    return false;
  }
  trigger(cell_sel_cmd{phy_cell});
  if (not is_in_state<selecting_cell>()) {
    log_h->warning("Failed to launch cell selection. Current state: %s\n", current_state_name().c_str());
    return false;
  }
  cell_selection_notifier = std::move(observer);
  return true;
}

void phy_controller::cell_selection_completed(bool outcome)
{
  trigger(cell_sel_res{outcome});
}

phy_controller::selecting_cell::selecting_cell(phy_controller* parent_) : composite_fsm_t(parent_)
{
  wait_in_sync_timer = parent_fsm()->task_sched.get_unique_timer();
}

void phy_controller::selecting_cell::enter(phy_controller* f, const cell_sel_cmd& ev)
{
  target_cell     = ev.phy_cell;
  csel_res.result = false;

  fsmInfo("Starting for pci=%d, earfcn=%d\n", target_cell.pci, target_cell.earfcn);
  if (not f->phy->cell_select(target_cell)) {
    trigger(srslte::failure_ev{});
  }
}

void phy_controller::selecting_cell::exit(phy_controller* f)
{
  wait_in_sync_timer.stop();

  if (csel_res.result) {
    fsmInfo("Cell %s successfully selected\n", to_string(target_cell).c_str());
  } else {
    fsmWarning("Failed to select cell %s\n", to_string(target_cell).c_str());
  }

  // Signal result back to FSM that called cell selection
  bool result = csel_res.result;
  if (f->cell_selection_always_observer) {
    f->cell_selection_always_observer(target_cell.earfcn, target_cell.pci, result);
  }
  f->task_sched.defer_task([f, result]() { f->cell_selection_notifier(result); });
}

void phy_controller::selecting_cell::wait_in_sync::enter(selecting_cell* f)
{
  f->wait_in_sync_timer.set(wait_sync_timeout_ms, [f](uint32_t tid) { f->parent_fsm()->trigger(timeout_ev{}); });
  f->wait_in_sync_timer.run();
}

/**************************************
 *    PHY Cell Search Procedure
 *************************************/

//! Searches for a cell in the current frequency and retrieves SIB1 if not retrieved yet
bool phy_controller::start_cell_search(srslte::event_observer<cell_srch_res> observer)
{
  if (is_in_state<searching_cell>()) {
    fsmInfo("Cell search already launched.\n");
    return true;
  }
  trigger(cell_search_cmd{});
  if (not is_in_state<searching_cell>()) {
    fsmWarning("Failed to launch cell search\n");
    return false;
  }
  cell_search_observers.subscribe(observer);
  return true;
}

void phy_controller::cell_search_completed(cell_search_ret_t cs_ret, phy_cell_t found_cell)
{
  trigger(cell_srch_res{cs_ret, found_cell});
}

void phy_controller::searching_cell::enter(phy_controller* f)
{
  otherfsmInfo(f, "Initiating Cell search\n");
  f->phy->cell_search();
}

void phy_controller::handle_cell_search_res(searching_cell& s, const cell_srch_res& result)
{
  switch (result.cs_ret.found) {
    case cell_search_ret_t::CELL_FOUND:
      fsmInfo("PHY cell search completed. Found cell %s\n", to_string(result.found_cell).c_str());
      break;
    case cell_search_ret_t::CELL_NOT_FOUND:
      fsmWarning("PHY cell search completed. No cells found.\n");
      break;
    default:
      fsmError("Invalid cell search result\n");
      // TODO: check what errors can happen (currently not handled in our code)
  }

  // Signal result back to FSM that called cell search
  auto copy = result;
  task_sched.defer_task([this, copy]() {
    cell_search_observers.dispatch(copy);
    cell_search_observers.unsubscribe_all();
  });
}

} // namespace srsue
