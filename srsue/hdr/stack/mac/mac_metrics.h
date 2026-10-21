// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSUE_MAC_METRICS_H
#define SRSUE_MAC_METRICS_H

namespace srsue {

struct mac_metrics_t {
  uint32_t nof_tti;
  int      tx_pkts;
  int      tx_errors;
  int      tx_brate;
  int      rx_pkts;
  int      rx_errors;
  int      rx_brate;
  int      ul_buffer;
  float    dl_retx_avg;
  float    ul_retx_avg;
};

} // namespace srsue

#endif // SRSUE_MAC_METRICS_H
