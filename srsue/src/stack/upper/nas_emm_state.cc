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
#include "srsue/hdr/stack/upper/nas_emm_state.h"

namespace srsue {

// FSM setters
void emm_state_t::set_null()
{
  state                 = state_t::null;
  deregistered_substate = deregistered_substate_t::null;
  registered_substate   = registered_substate_t::null;
  logger.debug("Changed to EMM state: %s", get_full_state_text().c_str());
}

void emm_state_t::set_deregistered(deregistered_substate_t substate)
{
  state                 = state_t::deregistered;
  deregistered_substate = substate;
  registered_substate   = registered_substate_t::null;
  logger.debug("Changed to EMM state: %s", get_full_state_text().c_str());
}

void emm_state_t::set_deregistered_initiated()
{
  state                 = state_t::deregistered_initiated;
  deregistered_substate = deregistered_substate_t::null;
  registered_substate   = registered_substate_t::null;
  logger.debug("Changed to EMM state: %s", get_full_state_text().c_str());
}

void emm_state_t::set_registered(registered_substate_t substate)
{
  state                 = state_t::registered;
  deregistered_substate = deregistered_substate_t::null;
  registered_substate   = substate;
  logger.debug("Changed to EMM state: %s", get_full_state_text().c_str());
}

void emm_state_t::set_registered_initiated()
{
  state                 = state_t::registered_initiated;
  deregistered_substate = deregistered_substate_t::null;
  registered_substate   = registered_substate_t::null;
  logger.debug("Changed to EMM state: %s", get_full_state_text().c_str());
}

void emm_state_t::set_tau_initiated()
{
  state                 = state_t::tau_initiated;
  deregistered_substate = deregistered_substate_t::null;
  registered_substate   = registered_substate_t::null;
  logger.debug("Changed to EMM state: %s", get_full_state_text().c_str());
}

void emm_state_t::set_service_request_initiated()
{
  state                 = state_t::service_request_initiated;
  deregistered_substate = deregistered_substate_t::null;
  registered_substate   = registered_substate_t::null;
  logger.debug("Changed to EMM state: %s", get_full_state_text().c_str());
}

const std::string emm_state_t::get_full_state_text()
{
  if (state == state_t::deregistered) {
    return emm_state_text(state) + std::string(", with substate ") +
           emm_deregistered_substate_text(deregistered_substate);
  } else if (state == state_t::registered) {
    return emm_state_text(state) + std::string(", with substate ") + emm_registered_substate_text(registered_substate);
  } else {
    return emm_state_text(state);
  }
  return std::string("Invalid State");
}

/*
 * Logging helper functions
 */
const char* emm_state_text(emm_state_t::state_t type)
{
  switch (type) {
    case emm_state_t::state_t::null:
      return "NULL";
    case emm_state_t::state_t::deregistered:
      return "DEREGISTERED";
    case emm_state_t::state_t::deregistered_initiated:
      return "DEREGISTERED-INITIATED";
    case emm_state_t::state_t::registered:
      return "REGISTERED";
    case emm_state_t::state_t::registered_initiated:
      return "REGISTERED-INITIATED";
    case emm_state_t::state_t::tau_initiated:
      return "TAU-INITIATED";
    case emm_state_t::state_t::service_request_initiated:
      return "SERVICE-REQUEST-INITIATED";
  }
  return "INVALID";
}

const char* emm_deregistered_substate_text(emm_state_t::deregistered_substate_t type)
{
  switch (type) {
    case emm_state_t::deregistered_substate_t::null:
      return "NULL";
    case emm_state_t::deregistered_substate_t::normal_service:
      return "NORMAL-SERVICE";
    case emm_state_t::deregistered_substate_t::limited_service:
      return "LIMITED-SERVICE";
    case emm_state_t::deregistered_substate_t::attempting_to_attach:
      return "ATTEMPTING-TO-ATTACH";
    case emm_state_t::deregistered_substate_t::plmn_search:
      return "PLMN-SEARCH";
    case emm_state_t::deregistered_substate_t::no_imsi:
      return "NO-IMSI";
    case emm_state_t::deregistered_substate_t::attach_needed:
      return "ATTACH-NEEDED";
    case emm_state_t::deregistered_substate_t::no_cell_available:
      return "NO-CELL-AVAILABLE";
    case emm_state_t::deregistered_substate_t::e_call_inactive:
      return "eCALL-INACTIVE";
  }
  return "INVALID";
}

const char* emm_registered_substate_text(emm_state_t::registered_substate_t type)
{
  switch (type) {
    case emm_state_t::registered_substate_t::null:
      return "NULL";
    case emm_state_t::registered_substate_t::normal_service:
      return "NORMAL-SERVICE";
    case emm_state_t::registered_substate_t::attempting_to_update:
      return "ATTEMPTING-TO-UPDATE";
    case emm_state_t::registered_substate_t::limited_service:
      return "LIMITED-SERVICE";
    case emm_state_t::registered_substate_t::plmn_search:
      return "PLMN-SEARCH";
    case emm_state_t::registered_substate_t::update_needed:
      return "UPDATE-NEEDED";
    case emm_state_t::registered_substate_t::no_cell_available:
      return "NO-CELL-AVAILABLE";
    case emm_state_t::registered_substate_t::attempting_to_update_mm:
      return "ATTEMPTING-TO-UPDATE-MM";
    case emm_state_t::registered_substate_t::imsi_dettach_initiated:
      return "IMSI-DETTACHED-INITIATED";
  }
  return "INVALID";
}

} // namespace srsue
