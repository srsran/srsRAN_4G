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

#ifndef SRSUE_RRC_METRICS_H
#define SRSUE_RRC_METRICS_H

#include "rrc_common.h"
#include "srsran/interfaces/phy_interface_types.h"

namespace srsue {

struct rrc_metrics_t {
  rrc_state_t             state;
  std::vector<phy_meas_t> neighbour_cells;
};

} // namespace srsue

#endif // SRSUE_RRC_METRICS_H
