// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#include "srsran/rrc/rrc_common.h"

namespace srsran {

uint8_t rrc_value_to_range(quant_s quant, const float value)
{
  uint8_t range = 0;
  if (quant == quant_rsrp) {
    if (value < -140) {
      range = 0;
    } else if (value < -44) {
      range = 1u + (uint8_t)(value + 140);
    } else {
      range = 97;
    }
  } else {
    if (value < -19.5) {
      range = 0;
    } else if (value < -3) {
      range = 1u + (uint8_t)(2 * (value + 19.5));
    } else {
      range = 34;
    }
  }
  return range;
}

float rrc_range_to_value(quant_s quant, const uint8_t range)
{
  float val = 0;
  if (quant == quant_rsrp) {
    val = -140 + (float)range;
  } else {
    val = -19.5f + (float)range / 2;
  }
  return val;
}

} // namespace srsran