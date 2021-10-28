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
#include "srsue/hdr/stack/upper/nas_5gmm_state.h"

namespace srsue {

// FSM setters
void mm5g_state_t::set_null()
{
  state                 = state_t::null;
  deregistered_substate = deregistered_substate_t::null;
  registered_substate   = registered_substate_t::null;
  logger.debug("Changed to mm5g state: %s", get_full_state_text().c_str());
}

void mm5g_state_t::set_deregistered(deregistered_substate_t substate)
{
  state                 = state_t::deregistered;
  deregistered_substate = substate;
  registered_substate   = registered_substate_t::null;
  logger.debug("Changed to mm5g state: %s", get_full_state_text().c_str());
}

void mm5g_state_t::set_deregistered_initiated()
{
  state                 = state_t::deregistered_initiated;
  deregistered_substate = deregistered_substate_t::null;
  registered_substate   = registered_substate_t::null;
  logger.debug("Changed to mm5g state: %s", get_full_state_text().c_str());
}

void mm5g_state_t::set_registered(registered_substate_t substate)
{
  state                 = state_t::registered;
  deregistered_substate = deregistered_substate_t::null;
  registered_substate   = substate;
  logger.debug("Changed to mm5g state: %s", get_full_state_text().c_str());
}

void mm5g_state_t::set_registered_initiated()
{
  state                 = state_t::registered_initiated;
  deregistered_substate = deregistered_substate_t::null;
  registered_substate   = registered_substate_t::null;
  logger.debug("Changed to mm5g state: %s", get_full_state_text().c_str());
}

void mm5g_state_t::set_service_request_initiated()
{
  state                 = state_t::service_request_initiated;
  deregistered_substate = deregistered_substate_t::null;
  registered_substate   = registered_substate_t::null;
  logger.debug("Changed to mm5g state: %s", get_full_state_text().c_str());
}

const std::string mm5g_state_t::get_full_state_text()
{
  if (state == state_t::deregistered) {
    return mm5g_state_text(state) + std::string(", with substate ") +
           mm5g_deregistered_substate_text(deregistered_substate);
  } else if (state == state_t::registered) {
    return mm5g_state_text(state) + std::string(", with substate ") +
           mm5g_registered_substate_text(registered_substate);
  }

  return mm5g_state_text(state);
}

/*
 * Logging helper functions
 */
const char* mm5g_state_text(mm5g_state_t::state_t type)
{
  switch (type) {
    case mm5g_state_t::state_t::null:
      return "NULL";
    case mm5g_state_t::state_t::deregistered:
      return "DEREGISTERED";
    case mm5g_state_t::state_t::registered_initiated:
      return "REGISTERED-INITIATED";
    case mm5g_state_t::state_t::registered:
      return "REGISTERED";
    case mm5g_state_t::state_t::deregistered_initiated:
      return "DEREGISTERED-INITIATED";
    case mm5g_state_t::state_t::service_request_initiated:
      return "SERVICE-REQUEST-INITIATED";
  }
  return "INVALID";
}

const char* mm5g_deregistered_substate_text(mm5g_state_t::deregistered_substate_t type)
{
  switch (type) {
    case mm5g_state_t::deregistered_substate_t::null:
      return "NULL";
    case mm5g_state_t::deregistered_substate_t::normal_service:
      return "NORMAL-SERVICE";
    case mm5g_state_t::deregistered_substate_t::limited_service:
      return "LIMITED-SERVICE";
    case mm5g_state_t::deregistered_substate_t::attempting_to_registration:
      return "ATTEMPTING-TO-REGISTRATION";
    case mm5g_state_t::deregistered_substate_t::plmn_search:
      return "PLMN-SEARCH";
    case mm5g_state_t::deregistered_substate_t::no_supi:
      return "NO-SUPI";
    case mm5g_state_t::deregistered_substate_t::no_cell_available:
      return "NO-CELL-AVAILABLE";
    case mm5g_state_t::deregistered_substate_t::e_call_inactive:
      return "eCALL-INACTIVE";
    case mm5g_state_t::deregistered_substate_t::initial_registration_needed:
      return "ATTACH-NEEDED";
  }
  return "INVALID";
}

const char* mm5g_registered_substate_text(mm5g_state_t::registered_substate_t type)
{
  switch (type) {
    case mm5g_state_t::registered_substate_t::null:
      return "NULL";
    case mm5g_state_t::registered_substate_t::normal_service:
      return "NORMAL-SERVICE";
    case mm5g_state_t::registered_substate_t::non_allowed_service:
      return "NON-ALLOWED-SERVICE";
    case mm5g_state_t::registered_substate_t::attempting_registration_update:
      return "ATTEMPTING-REGISTRATION-UPDATE";
    case mm5g_state_t::registered_substate_t::limited_service:
      return "LIMITED-SERVICE";
    case mm5g_state_t::registered_substate_t::plmn_search:
      return "PLMN-SEARCH";
    case mm5g_state_t::registered_substate_t::no_cell_available:
      return "NO-CELL-AVAILABLE";
    case mm5g_state_t::registered_substate_t::update_needed:
      return "UPDATE-NEEDED";
  }
  return "INVALID";
}

} // namespace srsue
