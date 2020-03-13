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

#ifndef SRSUE_NAS_COMMON_H
#define SRSUE_NAS_COMMON_H

#include <string>
#include <atomic>
#include "srslte/common/logmap.h"

namespace srsue {

typedef struct {
  int airplane_t_on_ms  = -1;
  int airplane_t_off_ms = -1;
} nas_sim_args_t;

class nas_args_t
{
public:
  nas_args_t() : force_imsi_attach(false) {}

  std::string    apn_name;
  std::string    apn_protocol;
  std::string    apn_user;
  std::string    apn_pass;
  bool           force_imsi_attach;
  std::string    eia;
  std::string    eea;
  nas_sim_args_t sim;
};

// EMM states (3GPP 24.302 v14.0.0)
class emm_state_t
{
public:
  enum class state_t {
    null = 0,
    deregistered,
    deregistered_initiated,
    registered,
    registered_initiated,
    tau_initiated,
    service_request_initiated,
  };

  // EMM-DEREGISTERED sub-states (3GPP 24.302 v14.0.0)
  enum class deregistered_substate_t {
    null = 0, // This should be used when not in EMM-DEREGISTERED
    normal_service,
    limited_service,
    attempting_to_attach,
    plmn_search,
    no_imsi,
    attach_needed,
    no_cell_available,
    e_call_inactive,
  };

  // EMM-REGISTERED sub-states (3GPP 24.302 v14.0.0)
  enum class registered_substate_t {
    null = 0, // This should be used when not in EMM-REGISTERED
    normal_service,
    attempting_to_update,
    limited_service,
    plmn_search,
    update_needed,
    no_cell_available,
    attempting_to_update_mm,
    imsi_dettach_initiated,
  };

  // FSM setters
  void set_null()
  {
    state                 = state_t::null;
    deregistered_substate = deregistered_substate_t::null;
    registered_substate   = registered_substate_t::null;
    nas_log->debug("Changed to EMM state: %s\n", get_full_state_text().c_str());
  }
  void set_deregistered(deregistered_substate_t substate)
  {
    state                 = state_t::deregistered;
    deregistered_substate = substate;
    registered_substate   = registered_substate_t::null;
    nas_log->debug("Changed to EMM state: %s\n", get_full_state_text().c_str());
  }
  void set_deregistered_initiated()
  {
    state                 = state_t::deregistered_initiated;
    deregistered_substate = deregistered_substate_t::null;
    registered_substate   = registered_substate_t::null;
    nas_log->debug("Changed to EMM state: %s\n", get_full_state_text().c_str());
  }
  void set_registered(registered_substate_t substate)
  {
    state                 = state_t::registered;
    deregistered_substate = deregistered_substate_t::null;
    registered_substate   = substate;
    nas_log->debug("Changed to EMM state: %s\n", get_full_state_text().c_str());
  }
  void set_registered_initiated()
  {
    state                 = state_t::registered_initiated;
    deregistered_substate = deregistered_substate_t::null;
    registered_substate   = registered_substate_t::null;
    nas_log->debug("Changed to EMM state: %s\n", get_full_state_text().c_str());
  }
  void set_tau_initiated()
  {
    state                 = state_t::tau_initiated;
    deregistered_substate = deregistered_substate_t::null;
    registered_substate   = registered_substate_t::null;
    nas_log->debug("Changed to EMM state: %s\n", get_full_state_text().c_str());
  }
  void set_service_request_initiated()
  {
    state                 = state_t::service_request_initiated;
    deregistered_substate = deregistered_substate_t::null;
    registered_substate   = registered_substate_t::null;
    nas_log->debug("Changed to EMM state: %s\n", get_full_state_text().c_str());
  }

  // FSM getters
  state_t                 get_state() { return state; }
  deregistered_substate_t get_deregistered_substate() { return deregistered_substate; }
  registered_substate_t   get_registered_substate() { return registered_substate; }

  // Text Helpers
  const std::string get_full_state_text();

private:
  std::atomic<state_t> state{state_t::null}; // The GW might require to know the NAS state from another thread
  deregistered_substate_t deregistered_substate = deregistered_substate_t::null;
  registered_substate_t   registered_substate   = registered_substate_t::null;
  srslte::log_ref nas_log{"NAS"};
};

inline const char* emm_state_text(emm_state_t::state_t type)
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

inline const char* emm_deregistered_substate_text(emm_state_t::deregistered_substate_t type)
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


inline const char* emm_registered_substate_text(emm_state_t::registered_substate_t type)
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

inline const std::string emm_state_t::get_full_state_text()
{
  if (state == state_t::deregistered) {
    return emm_state_text(state) + std::string(", with substate ") +
           emm_deregistered_substate_text(deregistered_substate);
  } else if (state == state_t::registered) {
    return emm_state_text(state) + std::string(", with substate ") +
           emm_registered_substate_text(registered_substate);
  } else {
    return emm_state_text(state); 
  }
  return std::string("Invalid State");
};

} // namespace srsue
#endif // SRSUE_NAS_COMMON_H
