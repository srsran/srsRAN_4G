// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef SRSRAN_RADIO_METRICS_H
#define SRSRAN_RADIO_METRICS_H

namespace srsran {

typedef struct {
  uint32_t rf_o;
  uint32_t rf_u;
  uint32_t rf_l;
  bool     rf_error;
} rf_metrics_t;

} // namespace srsran

#endif // SRSRAN_RADIO_METRICS_H
