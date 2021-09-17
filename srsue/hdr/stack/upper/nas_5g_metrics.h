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
