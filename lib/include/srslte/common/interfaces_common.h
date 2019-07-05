/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/common/timers.h"
#include "srslte/common/security.h"
#include <string>

namespace srslte {

typedef struct {
  std::string phy_level;
  std::string phy_lib_level;
  int         phy_hex_limit;
} phy_log_args_t;

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
  uint32_t    nof_radios;
  uint32_t    nof_rf_channels; // Number of RF channels per radio
  uint32_t    nof_rx_ant;      // Number of RF channels for MIMO
  uint32_t    nof_tx_ports;    // Number of Tx ports for MIMO
  std::string device_name;
  std::string device_args[SRSLTE_MAX_RADIOS];
  std::string time_adv_nsamples;
  std::string burst_preamble;
  std::string continuous_tx;
} rf_args_t;

class srslte_gw_config_t
{
public:
  srslte_gw_config_t(uint32_t lcid_ = 0)
  :lcid(lcid_)
  {}

  uint32_t lcid;
};


class srslte_pdcp_config_t
{
public:
  srslte_pdcp_config_t(uint8_t bearer_id_ = 0, bool is_control_ = false, bool is_data_ = false, uint8_t direction_ = SECURITY_DIRECTION_UPLINK)
    :bearer_id(bearer_id_)
    ,direction(direction_)
    ,is_control(is_control_)
    ,is_data(is_data_)
    ,sn_len(12) {}

  uint32_t bearer_id;
  uint8_t  direction;
  bool     is_control;
  bool     is_data;
  uint8_t  sn_len;

  // TODO: Support the following configurations
  // bool do_rohc;
};

class mac_interface_timers
{
public: 
  /* Timer services with ms resolution. 
   * timer_id must be lower than MAC_NOF_UPPER_TIMERS
   */
  virtual timers::timer* timer_get(uint32_t timer_id)  = 0;
  virtual void           timer_release_id(uint32_t timer_id) = 0;
  virtual uint32_t       timer_get_unique_id() = 0;
};

class read_pdu_interface
{
public:
  virtual int read_pdu(uint32_t lcid, uint8_t *payload, uint32_t requested_bytes) = 0; 
};

}

#endif // SRSLTE_INTERFACES_COMMON_H
