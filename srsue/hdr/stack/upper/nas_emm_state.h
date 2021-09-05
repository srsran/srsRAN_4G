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

#ifndef SRSUE_NAS_EMM_STATE_H
#define SRSUE_NAS_EMM_STATE_H

#include "srsran/srslog/srslog.h"
#include <string>

namespace srsue {

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
  void set_null();
  void set_deregistered(deregistered_substate_t substate);
  void set_deregistered_initiated();
  void set_registered(registered_substate_t substate);
  void set_registered_initiated();
  void set_tau_initiated();
  void set_service_request_initiated();

  // FSM getters
  state_t                 get_state() { return state; }
  deregistered_substate_t get_deregistered_substate() { return deregistered_substate; }
  registered_substate_t   get_registered_substate() { return registered_substate; }

  // Text Helpers
  const std::string get_full_state_text();

private:
  state_t                 state                 = state_t::null;
  deregistered_substate_t deregistered_substate = deregistered_substate_t::null;
  registered_substate_t   registered_substate   = registered_substate_t::null;
  srslog::basic_logger&   logger                = srslog::fetch_basic_logger("NAS");
};

const char* emm_state_text(emm_state_t::state_t type);
const char* emm_deregistered_substate_text(emm_state_t::deregistered_substate_t type);
const char* emm_registered_substate_text(emm_state_t::registered_substate_t type);

} // namespace srsue

#endif
