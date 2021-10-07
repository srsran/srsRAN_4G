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

#include <stdint.h>

namespace srsran {

enum quant_s { quant_rsrp, quant_rsrq };

uint8_t rrc_value_to_range(quant_s quant, const float value);
float   rrc_range_to_value(quant_s quant, const uint8_t range);

} // namespace srsran

#endif // SRSUE_RRC_COMMON_H
