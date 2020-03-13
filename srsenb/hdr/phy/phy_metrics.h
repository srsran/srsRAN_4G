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

#ifndef SRSENB_PHY_METRICS_H
#define SRSENB_PHY_METRICS_H

namespace srsenb {

// PHY metrics per user

struct ul_metrics_t {
  float n;
  float sinr;
  float rssi;
  float turbo_iters;
  float mcs;
  int   n_samples;
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
