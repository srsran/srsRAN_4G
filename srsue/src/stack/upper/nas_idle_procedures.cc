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

#include "srsue/hdr/stack/upper/nas_idle_procedures.h"
#include "srsran/common/standard_streams.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"

using namespace srsran;

#define ProcError(fmt, ...) nas_ptr->logger.error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define ProcWarning(fmt, ...) nas_ptr->logger.warning("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define ProcInfo(fmt, ...) nas_ptr->logger.info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

namespace srsue {

using srsran::proc_outcome_t;

/*
 * PLMN Selection Procedures
 * As specified in TS 23.122 version 14.5.0
 */
proc_outcome_t nas::plmn_search_proc::init()
{
  // start RRC PLMN selection
  if (not nas_ptr->rrc->plmn_search()) {
    ProcError("ProcError while searching for PLMNs");
    return proc_outcome_t::error;
  }

  ProcInfo("Starting...");
  return proc_outcome_t::yield;
}

proc_outcome_t nas::plmn_search_proc::step()
{
  return proc_outcome_t::yield;
}

void nas::plmn_search_proc::then(const srsran::proc_state_t& result)
{
  ProcInfo("Completed with %s", result.is_success() ? "success" : "failure");

  if (result.is_error()) {
    nas_ptr->enter_emm_deregistered(emm_state_t::deregistered_substate_t::plmn_search);
  }
}

proc_outcome_t nas::plmn_search_proc::react(const plmn_search_complete_t& t)
{
  // check whether the NAS state hasn't changed
  if (nas_ptr->state.get_state() != emm_state_t::state_t::deregistered and
      nas_ptr->state.get_deregistered_substate() != emm_state_t::deregistered_substate_t::plmn_search) {
    ProcError("ProcError while searching for PLMNs");
    return proc_outcome_t::error;
  }

  if (t.nof_plmns < 0) {
    ProcError("Error while searching for PLMNs");
    return proc_outcome_t::error;
  }
  if (t.nof_plmns == 0) {
    ProcWarning("Did not find any PLMN in the set of frequencies.");
    return proc_outcome_t::error;
  }

  // Save PLMNs
  nas_ptr->known_plmns.clear();
  for (int i = 0; i < t.nof_plmns; i++) {
    nas_ptr->known_plmns.push_back(t.found_plmns[i].plmn_id);
    ProcInfo("Found PLMN:  Id=%s, TAC=%d", t.found_plmns[i].plmn_id.to_string().c_str(), t.found_plmns[i].tac);
    srsran::console("Found PLMN:  Id=%s, TAC=%d\n", t.found_plmns[i].plmn_id.to_string().c_str(), t.found_plmns[i].tac);
  }
  nas_ptr->select_plmn();

  // Select PLMN in request establishment of RRC connection
  if (nas_ptr->state.get_state() != emm_state_t::state_t::deregistered and
      nas_ptr->state.get_deregistered_substate() != emm_state_t::deregistered_substate_t::normal_service) {
    ProcError("PLMN is not selected because no suitable PLMN was found");
    return proc_outcome_t::error;
  }

  nas_ptr->rrc->plmn_select(nas_ptr->current_plmn);

  return proc_outcome_t::success;
}

} // namespace srsue
