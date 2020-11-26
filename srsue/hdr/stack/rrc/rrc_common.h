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

#ifndef SRSUE_RRC_COMMON_H
#define SRSUE_RRC_COMMON_H

namespace srsue {

// RRC states (3GPP 36.331 v10.0.0)
typedef enum {
  RRC_STATE_IDLE = 0,
  RRC_STATE_CONNECTED,
  RRC_STATE_N_ITEMS,
} rrc_state_t;
static const char rrc_state_text[RRC_STATE_N_ITEMS][100] = {"IDLE", "CONNECTED"};

} // namespace srsue

#endif // SRSUE_RRC_COMMON_H
