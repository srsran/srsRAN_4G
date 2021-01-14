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

#ifndef SRSLTE_SCHED_UE_CELL_H
#define SRSLTE_SCHED_UE_CELL_H

#include "../sched_common.h"

namespace srsenb {

struct sched_ue_cell {
  using ue_cc_cfg = sched_interface::ue_cfg_t::cc_cfg_t;

  sched_ue_cell(uint16_t rnti_, const sched_cell_params_t& cell_cfg_);
  void set_ue_cfg(const sched_interface::ue_cfg_t& ue_cfg_);

  bool             configured() const { return ue_cc_idx >= 0; }
  int              get_ue_cc_idx() const { return ue_cc_idx; }
  const ue_cc_cfg* get_ue_cc_cfg() const { return configured() ? &ue_cfg->supported_cc_list[ue_cc_idx] : nullptr; }

  /// Cell const configuration
  const sched_cell_params_t* cell_cfg = nullptr;

  /// Allowed DCI locations per per CFI and per subframe
  const ue_cce_locations_table dci_locations;

private:
  uint16_t                         rnti      = SRSLTE_INVALID_RNTI;
  const sched_interface::ue_cfg_t* ue_cfg    = nullptr;
  int                              ue_cc_idx = -1;
};

} // namespace srsenb

#endif // SRSLTE_SCHED_UE_CELL_H
