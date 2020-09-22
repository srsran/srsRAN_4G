/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
