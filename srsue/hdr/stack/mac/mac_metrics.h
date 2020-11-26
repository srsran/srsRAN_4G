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
