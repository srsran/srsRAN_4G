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

#include "srsran/srslog/srslog.h"
#include "srsue/hdr/stack/rrc_nr/rrc_nr.h"

#ifndef SRSRAN_RRC_NR_PROCEDURES_H
#define SRSRAN_RRC_NR_PROCEDURES_H

namespace srsue {

/********************************
 *         Procedures
 *******************************/

class rrc_nr::connection_reconf_no_ho_proc
{
public:
  explicit connection_reconf_no_ho_proc(rrc_nr* parent_);
  srsran::proc_outcome_t init(const reconf_initiator_t  initiator_,
                              const bool                endc_release_and_add_r15,
                              const bool                nr_secondary_cell_group_cfg_r15_present,
                              const asn1::dyn_octstring nr_secondary_cell_group_cfg_r15,
                              const bool                sk_counter_r15_present,
                              const uint32_t            sk_counter_r15,
                              const bool                nr_radio_bearer_cfg1_r15_present,
                              const asn1::dyn_octstring nr_radio_bearer_cfg1_r15);
  srsran::proc_outcome_t step() { return srsran::proc_outcome_t::yield; }
  static const char*     name() { return "NR Connection Reconfiguration"; }
  srsran::proc_outcome_t react(const bool& config_complete);
  void                   then(const srsran::proc_state_t& result);

private:
  // const
  rrc_nr*                        rrc_ptr = nullptr;
  reconf_initiator_t             initiator;
  asn1::rrc_nr::rrc_recfg_s      rrc_recfg;
  asn1::rrc_nr::cell_group_cfg_s cell_group_cfg;
};

} // namespace srsue

#endif // SRSRAN_RRC_NR_PROCEDURES_H
