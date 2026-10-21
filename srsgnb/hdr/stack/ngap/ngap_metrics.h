// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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
