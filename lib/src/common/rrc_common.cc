/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

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