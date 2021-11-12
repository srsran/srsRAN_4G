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

#ifndef SRSENB_PHY_METRICS_H
#define SRSENB_PHY_METRICS_H

#include <limits>

namespace srsenb {

// PHY metrics per user

struct ul_metrics_t {
  float n;
  float pusch_sinr;
  // Initialize this member with an invalid value as this field is optional.
  float pusch_rssi = std::numeric_limits<float>::quiet_NaN();
  // Initialize this member with an invalid value as this field is optional.
  int64_t pusch_tpc = 0;
  float   pucch_sinr;
  // Initialize this member with an invalid value as this field is optional.
  float pucch_rssi = std::numeric_limits<float>::quiet_NaN();
  // Initialize this member with an invalid value as this field is optional.
  float pucch_ni = std::numeric_limits<float>::quiet_NaN();
  float rssi;
  float turbo_iters;
  float mcs;
  int   n_samples;
  int   n_samples_pucch;
};

struct dl_metrics_t {
  float mcs;
  // Initialize this member with an invalid value as this field is optional.
  int64_t pucch_tpc = 0;
  int     n_samples;
};

struct phy_metrics_t {
  dl_metrics_t dl;
  ul_metrics_t ul;
};

} // namespace srsenb

#endif // SRSENB_PHY_METRICS_H
