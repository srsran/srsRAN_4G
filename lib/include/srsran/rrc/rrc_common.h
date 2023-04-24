/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSUE_RRC_COMMON_H
#define SRSUE_RRC_COMMON_H

#include <stdint.h>

namespace srsran {

enum quant_s { quant_rsrp, quant_rsrq };

uint8_t rrc_value_to_range(quant_s quant, const float value);
float   rrc_range_to_value(quant_s quant, const uint8_t range);

} // namespace srsran

#endif // SRSUE_RRC_COMMON_H
