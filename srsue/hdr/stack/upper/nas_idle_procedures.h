/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
/*
 * NAS idle mode Procedures
 * As specified in TS 23.122 version 14.5.0
 */

#ifndef SRSUE_NAS_IDLE_PROCEDURES_H
#define SRSUE_NAS_IDLE_PROCEDURES_H

#include "srsran/common/stack_procedure.h"
#include "srsue/hdr/stack/upper/nas.h"

namespace srsue {

class nas::plmn_search_proc
{
public:
  struct plmn_search_complete_t {
    nas_interface_rrc::found_plmn_t found_plmns[nas_interface_rrc::MAX_FOUND_PLMNS];
    int                             nof_plmns;
    plmn_search_complete_t(const nas_interface_rrc::found_plmn_t* plmns_, int nof_plmns_) : nof_plmns(nof_plmns_)
    {
      if (nof_plmns > 0) {
        std::copy(&plmns_[0], &plmns_[nof_plmns], found_plmns);
      }
    }
  };

  explicit plmn_search_proc(nas* nas_ptr_) : nas_ptr(nas_ptr_) {}
  srsran::proc_outcome_t init();
  srsran::proc_outcome_t step();
  void                   then(const srsran::proc_state_t& result);
  srsran::proc_outcome_t react(const plmn_search_complete_t& t);
  static const char*     name() { return "NAS PLMN Search"; }

private:
  nas* nas_ptr;
};

} // namespace srsue

#endif // SRSUE_NAS_PLMN_SELECT_H
