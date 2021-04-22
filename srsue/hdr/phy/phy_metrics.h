/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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

#include "srsran/srsran.h"
#include <array>

namespace srsue {

struct info_metrics_t {
  typedef std::array<info_metrics_t, SRSRAN_MAX_CARRIERS> array_t;

  uint32_t pci;
  uint32_t dl_earfcn;
};

#define PHY_METRICS_SET(PARAM)                                                                                         \
  do {                                                                                                                 \
    PARAM = PARAM + (other.PARAM - PARAM) / count;                                                                     \
  } while (false)

struct sync_metrics_t {
  typedef std::array<sync_metrics_t, SRSRAN_MAX_CARRIERS> array_t;

  float ta_us;
  float distance_km;
  float speed_kmph;
  float cfo;
  float sfo;

  void set(const sync_metrics_t& other)
  {
    count++;
    ta_us       = other.ta_us;
    distance_km = other.distance_km;
    speed_kmph  = other.speed_kmph;
    PHY_METRICS_SET(cfo);
    PHY_METRICS_SET(sfo);
  }

  void reset()
  {
    count       = 0;
    ta_us       = 0.0f;
    distance_km = 0.0f;
    speed_kmph  = 0.0f;
    cfo         = 0.0f;
    sfo         = 0.0f;
  }

private:
  uint32_t count = 0;
};

struct ch_metrics_t {
  typedef std::array<ch_metrics_t, SRSRAN_MAX_CARRIERS> array_t;

  float n;
  float sinr;
  float rsrp;
  float rsrq;
  float rssi;
  float ri;
  float pathloss;
  float sync_err;

  void set(const ch_metrics_t& other)
  {
    count++;
    PHY_METRICS_SET(n);
    PHY_METRICS_SET(sinr);
    PHY_METRICS_SET(rsrp);
    PHY_METRICS_SET(rsrq);
    PHY_METRICS_SET(rssi);
    PHY_METRICS_SET(ri);
    PHY_METRICS_SET(pathloss);
    PHY_METRICS_SET(sync_err);
  }

  void reset()
  {
    count    = 0;
    n        = 0.0;
    sinr     = 0.0;
    rsrp     = 0.0;
    rsrq     = 0.0;
    rssi     = 0.0;
    ri       = 0.0;
    pathloss = 0.0;
    sync_err = 0.0;
  }

private:
  uint32_t count = 0;
};

struct dl_metrics_t {
  typedef std::array<dl_metrics_t, SRSRAN_MAX_CARRIERS> array_t;

  float fec_iters;
  float mcs;
  float evm;

  void set(const dl_metrics_t& other)
  {
    count++;
    PHY_METRICS_SET(fec_iters);
    PHY_METRICS_SET(mcs);
    PHY_METRICS_SET(evm);
  }

  void reset()
  {
    count     = 0;
    fec_iters = 0.0f;
    mcs       = 0.0f;
    evm       = 0.0f;
  }

private:
  uint32_t count = 0;
};

struct ul_metrics_t {
  typedef std::array<ul_metrics_t, SRSRAN_MAX_CARRIERS> array_t;

  float mcs;
  float power;

  void set(const ul_metrics_t& other)
  {
    count++;
    PHY_METRICS_SET(mcs);
    PHY_METRICS_SET(power);
  }

  void reset()
  {
    count = 0;
    mcs   = 0.0f;
    power = 0.0f;
  }

private:
  uint32_t count = 0;
};

#undef PHY_METRICS_SET

struct phy_metrics_t {
  info_metrics_t::array_t info          = {};
  sync_metrics_t::array_t sync          = {};
  ch_metrics_t::array_t   ch            = {};
  dl_metrics_t::array_t   dl            = {};
  ul_metrics_t::array_t   ul            = {};
  uint32_t                nof_active_cc = 0;
};

} // namespace srsue

#endif // SRSUE_PHY_METRICS_H
