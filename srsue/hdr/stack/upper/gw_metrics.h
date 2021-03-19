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

#ifndef SRSUE_GW_METRICS_H
#define SRSUE_GW_METRICS_H

namespace srsue {

struct gw_metrics_t {
  double dl_tput_mbps;
  double ul_tput_mbps;
};

} // namespace srsue

#endif // SRSUE_GW_METRICS_H
