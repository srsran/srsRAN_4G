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

#ifndef SRSUE_RRC_NR_METRICS_H
#define SRSUE_RRC_NR_METRICS_H

namespace srsue {

/// RRC states (3GPP 38.331 v15.5.1 Sec 4.2.1)
enum rrc_nr_state_t {
  RRC_NR_STATE_IDLE = 0,
  RRC_NR_STATE_CONNECTED,
  RRC_NR_STATE_CONNECTED_INACTIVE,
  RRC_NR_STATE_N_ITEMS,
};

struct rrc_nr_metrics_t {
  rrc_nr_state_t state;
};

} // namespace srsue

#endif // SRSUE_RRC_NR_METRICS_H
