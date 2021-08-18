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

#include "srsue/hdr/stack/rrc/rrc_common.h"

namespace srsue {

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

} // namespace srsue