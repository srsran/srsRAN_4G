/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
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


namespace srsue {

struct sync_metrics_t
{
  float cfo;
  float sfo;
};

struct dl_metrics_t
{
  float n;
  float sinr;
  float rsrp;
  float rsrq;
  float rssi;
  float turbo_iters;
  float mcs;
  float pathloss;
  float mabr_mbps;
};

struct ul_metrics_t
{
  float mcs;
  float power;
  float mabr_mbps;
};

struct phy_metrics_t
{
  sync_metrics_t sync;
  dl_metrics_t   dl;
  ul_metrics_t   ul;
};

} // namespace srsue

#endif // SRSUE_PHY_METRICS_H
