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

#ifndef SRSLTE_RADIO_METRICS_H
#define SRSLTE_RADIO_METRICS_H

namespace srslte {

typedef struct {
  uint32_t rf_o;
  uint32_t rf_u;
  uint32_t rf_l;
  bool     rf_error;
} rf_metrics_t;

} // namespace srslte

#endif // SRSLTE_RADIO_METRICS_H
