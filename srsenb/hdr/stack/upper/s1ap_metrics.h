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

#ifndef SRSENB_S1AP_METRICS_H
#define SRSENB_S1AP_METRICS_H

namespace srsenb {

typedef enum {
  S1AP_ATTACHING = 0, // Attempting to create S1 connection
  S1AP_READY,         // S1 connected
  S1AP_ERROR          // Failure
} S1AP_STATUS_ENUM;

struct s1ap_metrics_t {
  S1AP_STATUS_ENUM status;
};

} // namespace srsenb

#endif // SRSENB_S1AP_METRICS_H
