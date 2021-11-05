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

#ifndef SRSENB_NGAP_METRICS_H
#define SRSENB_NGAP_METRICS_H

namespace srsenb {

typedef enum {
  ngap_attaching = 0, // Attempting to create NG connection
  ngap_connected,     // NG connected
  ngap_error          // Failure
} ngap_status_t;

struct ngap_metrics_t {
  ngap_status_t status;
};

} // namespace srsenb

#endif // SRSENB_NGAP_METRICS_H
