// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSUE_NAS_5G_METRICS_H
#define SRSUE_NAS_5G_METRICS_H

#include "nas_5gmm_state.h"

namespace srsue {

struct nas_5g_metrics_t {
  uint32_t              nof_active_pdu_sessions;
  mm5g_state_t::state_t state;
};

} // namespace srsue

#endif // SRSUE_NAS_5G_METRICS_H
