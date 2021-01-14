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

#include "srsenb/hdr/stack/mac/sched_ue_ctrl/sched_ue_cell.h"
#include "srsenb/hdr/stack/mac/sched_helpers.h"

namespace srsenb {

sched_ue_cell::sched_ue_cell(uint16_t rnti_, const sched_cell_params_t& cell_cfg_) :
  rnti(rnti_), cell_cfg(&cell_cfg_), dci_locations(generate_cce_location_table(rnti_, cell_cfg_))
{}

void sched_ue_cell::set_ue_cfg(const sched_interface::ue_cfg_t& ue_cfg_)
{
  ue_cfg = &ue_cfg_;
  for (size_t i = 0; i < ue_cfg_.supported_cc_list.size(); ++i) {
    if (ue_cfg_.supported_cc_list[i].enb_cc_idx == cell_cfg->enb_cc_idx) {
      ue_cc_idx = i;
    }
  }
}

} // namespace srsenb
