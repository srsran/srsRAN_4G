// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSUE_RRC_COMMON_H
#define SRSUE_RRC_COMMON_H

#include <stdint.h>

namespace srsran {

enum quant_s { quant_rsrp, quant_rsrq };

uint8_t rrc_value_to_range(quant_s quant, const float value);
float   rrc_range_to_value(quant_s quant, const uint8_t range);

} // namespace srsran

#endif // SRSUE_RRC_COMMON_H
