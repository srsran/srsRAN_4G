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

#ifndef SRSUE_PHY_METRICS_H
#define SRSUE_PHY_METRICS_H

#include "srslte/srslte.h"

namespace srsue {

struct info_metrics_t {
  uint32_t pci;
  uint32_t dl_earfcn;
};

struct sync_metrics_t {
  float ta_us;
  float distance_km;
  float speed_kmph;
  float cfo;
  float sfo;
};

struct ch_metrics_t {
  float n;
  float sinr;
  float rsrp;
  float rsrq;
  float rssi;
  float ri;
  float pathloss;
  float sync_err;
};

struct dl_metrics_t {
  float turbo_iters;
  float mcs;
};

struct ul_metrics_t {
  float mcs;
  float power;
};

struct phy_metrics_t {
  info_metrics_t info[SRSLTE_MAX_CARRIERS];
  sync_metrics_t sync[SRSLTE_MAX_CARRIERS];
  ch_metrics_t   ch[SRSLTE_MAX_CARRIERS];
  dl_metrics_t   dl[SRSLTE_MAX_CARRIERS];
  ul_metrics_t   ul[SRSLTE_MAX_CARRIERS];
  uint32_t       nof_active_cc;
};

} // namespace srsue

#endif // SRSUE_PHY_METRICS_H
