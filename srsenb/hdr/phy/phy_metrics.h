// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSENB_PHY_METRICS_H
#define SRSENB_PHY_METRICS_H

#include <limits>

namespace srsenb {

// PHY metrics per user

struct ul_metrics_t {
  float   n;
  float   pusch_sinr;
  float   pusch_rssi;
  int64_t pusch_tpc;
  float   pucch_sinr;
  float   pucch_rssi;
  float   pucch_ni;
  float   turbo_iters;
  float   mcs;
  int     n_samples;
  int     n_samples_pucch;
};

struct dl_metrics_t {
  float mcs;
  int64_t pucch_tpc;
  int     n_samples;
};

struct phy_metrics_t {
  dl_metrics_t dl;
  ul_metrics_t ul;
};

} // namespace srsenb

#endif // SRSENB_PHY_METRICS_H
