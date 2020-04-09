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

#ifndef SRSLTE_INTERFACES_COMMON_H
#define SRSLTE_INTERFACES_COMMON_H

#include "srslte/common/multiqueue.h"
#include "srslte/common/security.h"
#include "srslte/common/timers.h"
#include <string>

namespace srslte {

typedef struct {
  std::string phy_level     = "none";
  std::string phy_lib_level = "none";
  int         phy_hex_limit = -1;
} phy_log_args_t;

typedef struct {
  float min;
  float max;
} rf_args_band_t;

// RF/radio args
typedef struct {
  std::string type;
  std::string log_level;
  float       dl_freq;
  float       ul_freq;
  float       freq_offset;
  float       rx_gain;
  float       tx_gain;
  float       tx_max_power;
  float       tx_gain_offset;
  float       rx_gain_offset;
  uint32_t    nof_carriers; // Number of RF channels
  uint32_t    nof_antennas; // Number of antennas per RF channel
  std::string device_name;
  std::string device_args;
  std::string time_adv_nsamples;
  std::string continuous_tx;

  std::array<rf_args_band_t, SRSLTE_MAX_CARRIERS> ch_rx_bands;
  std::array<rf_args_band_t, SRSLTE_MAX_CARRIERS> ch_tx_bands;

} rf_args_t;

class srslte_gw_config_t
{
public:
  explicit srslte_gw_config_t(uint32_t lcid_ = 0) : lcid(lcid_) {}

  uint32_t lcid;
};

class read_pdu_interface
{
public:
  virtual int read_pdu(uint32_t lcid, uint8_t* payload, uint32_t requested_bytes) = 0;
};

// Generic Task Management + Timer interface for upper stack
class task_handler_interface
{
public:
  virtual srslte::timer_handler::unique_timer    get_unique_timer()                                               = 0;
  virtual srslte::task_multiqueue::queue_handler make_task_queue()                                                = 0;
  virtual void                                   defer_callback(uint32_t duration_ms, std::function<void()> func) = 0;
  virtual void                                   defer_task(srslte::move_task_t func)                             = 0;
  virtual void                                   enqueue_background_task(std::function<void(uint32_t)> task)      = 0;
  virtual void                                   notify_background_task_result(srslte::move_task_t task)          = 0;
};

} // namespace srslte

#endif // SRSLTE_INTERFACES_COMMON_H
