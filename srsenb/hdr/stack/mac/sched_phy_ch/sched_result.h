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

#ifndef SRSRAN_SCHED_RESULT_H
#define SRSRAN_SCHED_RESULT_H

#include "../sched_lte_common.h"
#include "srsenb/hdr/stack/mac/sched_phy_ch/sched_phy_resource.h"

namespace srsenb {

/// Result of a Subframe sched computation
struct cc_sched_result {
  bool      generated = false;
  tti_point tti_rx{};

  /// Accumulation of all DL RBG allocations
  rbgmask_t dl_mask = {};

  /// Accumulation of all UL PRB allocations
  prbmask_t ul_mask = {};

  /// Accumulation of all CCE allocations
  pdcch_mask_t pdcch_mask = {};

  /// Individual allocations information
  sched_interface::dl_sched_res_t dl_sched_result = {};
  sched_interface::ul_sched_res_t ul_sched_result = {};
};

} // namespace srsenb

#endif // SRSRAN_SCHED_RESULT_H
