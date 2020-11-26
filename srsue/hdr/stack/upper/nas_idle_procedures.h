/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
/*
 * NAS idle mode Procedures
 * As specified in TS 23.122 version 14.5.0
 */

#ifndef SRSUE_NAS_IDLE_PROCEDURES_H
#define SRSUE_NAS_IDLE_PROCEDURES_H

#include "srslte/common/stack_procedure.h"
#include "srsue/hdr/stack/upper/nas.h"

namespace srsue {

class nas::plmn_search_proc
{
public:
  struct plmn_search_complete_t {
    rrc_interface_nas::found_plmn_t found_plmns[rrc_interface_nas::MAX_FOUND_PLMNS];
    int                             nof_plmns;
    plmn_search_complete_t(const rrc_interface_nas::found_plmn_t* plmns_, int nof_plmns_) : nof_plmns(nof_plmns_)
    {
      if (nof_plmns > 0) {
        std::copy(&plmns_[0], &plmns_[nof_plmns], found_plmns);
      }
    }
  };

  explicit plmn_search_proc(nas* nas_ptr_) : nas_ptr(nas_ptr_) {}
  srslte::proc_outcome_t init();
  srslte::proc_outcome_t step();
  void                   then(const srslte::proc_state_t& result);
  srslte::proc_outcome_t react(const plmn_search_complete_t& t);
  static const char*     name() { return "NAS PLMN Search"; }

private:
  nas* nas_ptr;
};

} // namespace srsue

#endif // SRSUE_NAS_PLMN_SELECT_H
