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

#ifndef SRSUE_NR_NAS_MM5G_STATE_H
#define SRSUE_NR_NAS_MM5G_STATE_H

#include "srsran/srslog/srslog.h"
#include <atomic>
#include <string>

namespace srsue {

// 5GMM states (3GPP 24.501 v16.07.0)
class mm5g_state_t
{
public:
  enum class state_t {
    null = 0,
    deregistered,
    registered_initiated,
    registered,
    deregistered_initiated,
    service_request_initiated,
  };

  // 5GMM-DEREGISTERED sub-states (3GPP 24.501 v16.07.0)
  enum class deregistered_substate_t {
    null = 0, // This should be used when not in mm5g-DEREGISTERED
    normal_service,
    limited_service,
    attempting_to_registration,
    plmn_search,
    no_supi,
    no_cell_available,
    e_call_inactive,
    initial_registration_needed,
  };

  // 5GMM-DEREGISTERED sub-states (3GPP 24.501 v16.07.0)
  enum class registered_substate_t {
    null = 0, // This should be used when not in mm5g-REGISTERED
    normal_service,
    non_allowed_service,
    attempting_registration_update,
    limited_service,
    plmn_search,
    no_cell_available,
    update_needed,
  };

  // FSM setters
  void set_null();
  void set_deregistered(deregistered_substate_t substate);
  void set_deregistered_initiated();
  void set_registered(registered_substate_t substate);
  void set_registered_initiated();
  void set_service_request_initiated();

  // FSM getters
  state_t                 get_state() { return state; }
  deregistered_substate_t get_deregistered_substate() { return deregistered_substate; }
  registered_substate_t   get_registered_substate() { return registered_substate; }

  // Text Helpers
  const std::string get_full_state_text();

private:
  std::atomic<state_t>    state{state_t::null};
  deregistered_substate_t deregistered_substate = deregistered_substate_t::null;
  registered_substate_t   registered_substate   = registered_substate_t::null;
  srslog::basic_logger&   logger                = srslog::fetch_basic_logger("NAS-5G");
};

const char* mm5g_state_text(mm5g_state_t::state_t type);
const char* mm5g_deregistered_substate_text(mm5g_state_t::deregistered_substate_t type);
const char* mm5g_registered_substate_text(mm5g_state_t::registered_substate_t type);

} // namespace srsue
#endif
