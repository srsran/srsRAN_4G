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

#ifndef SRSUE_RRC_COMMON_H
#define SRSUE_RRC_COMMON_H

namespace srsue {

#include <stdint.h>

// RRC states (3GPP 36.331 v10.0.0)
typedef enum {
  RRC_STATE_IDLE = 0,
  RRC_STATE_CONNECTED,
  RRC_STATE_N_ITEMS,
} rrc_state_t;
static const char rrc_state_text[RRC_STATE_N_ITEMS][100] = {"IDLE", "CONNECTED"};

enum quant_s { quant_rsrp, quant_rsrq };

uint8_t rrc_value_to_range(quant_s quant, const float value);
float   rrc_range_to_value(quant_s quant, const uint8_t range);

} // namespace srsue

#endif // SRSUE_RRC_COMMON_H
