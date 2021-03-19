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

namespace srsenb {

// PHY metrics per user

struct ul_metrics_t {
  float n;
  float pusch_sinr;
  float pucch_sinr;
  float rssi;
  float turbo_iters;
  float mcs;
  int   n_samples;
  int   n_samples_pucch;
};

struct dl_metrics_t {
  float mcs;
  int   n_samples;
};

struct phy_metrics_t {
  dl_metrics_t dl;
  ul_metrics_t ul;
};

} // namespace srsenb

#endif // SRSENB_PHY_METRICS_H
