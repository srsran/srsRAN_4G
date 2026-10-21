// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSUE_RRC_METRICS_H
#define SRSUE_RRC_METRICS_H

#include "srsran/interfaces/phy_interface_types.h"

namespace srsue {

// RRC states (3GPP 36.331 v10.0.0)
typedef enum {
  RRC_STATE_IDLE = 0,
  RRC_STATE_CONNECTED,
  RRC_STATE_N_ITEMS,
} rrc_state_t;
static const char rrc_state_text[RRC_STATE_N_ITEMS][100] = {"IDLE", "CONNECTED"};

struct rrc_metrics_t {
  rrc_state_t             state;
  std::vector<phy_meas_t> neighbour_cells;
};

} // namespace srsue

#endif // SRSUE_RRC_METRICS_H
