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

/******************************************************************************
 * File:        common_interfaces.h
 * Description: Common interface for eNB/UE for PHY and radio
 *****************************************************************************/

#ifndef SRSLTE_COMMON_INTERFACES_H
#define SRSLTE_COMMON_INTERFACES_H

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/common/timestamp.h"
#include "srslte/phy/rf/rf.h"

namespace srslte {

class radio_interface_phy
{
public:
  // trx functions
  virtual bool tx(const uint32_t&           radio_idx,
                  cf_t*                     buffer[SRSLTE_MAX_PORTS],
                  const uint32_t&           nof_samples,
                  const srslte_timestamp_t& tx_time) = 0;
  virtual void tx_end()                              = 0;
  virtual bool rx_now(const uint32_t&     radio_idx,
                      cf_t*               buffer[SRSLTE_MAX_PORTS],
                      const uint32_t&     nof_samples,
                      srslte_timestamp_t* rxd_time)  = 0;

  // setter
  virtual void set_tx_freq(const uint32_t& radio_idx, const uint32_t& channel_idx, const double& freq) = 0;
  virtual void set_rx_freq(const uint32_t& radio_idx, const uint32_t& channel_idx, const double& freq) = 0;

  virtual double set_rx_gain_th(const float& gain)                            = 0;
  virtual void   set_rx_gain(const uint32_t& radio_idx, const float& gain)    = 0;
  virtual void   set_tx_srate(const uint32_t& radio_idx, const double& srate) = 0;
  virtual void   set_rx_srate(const uint32_t& radio_idx, const double& srate) = 0;

  // getter
  virtual float             get_rx_gain(const uint32_t& radio_idx)           = 0;
  virtual double            get_freq_offset()                                = 0;
  virtual double            get_tx_freq(const uint32_t& radio_idx)           = 0;
  virtual double            get_rx_freq(const uint32_t& radio_idx)           = 0;
  virtual float             get_max_tx_power()                               = 0;
  virtual float             get_tx_gain_offset()                             = 0;
  virtual float             get_rx_gain_offset()                             = 0;
  virtual bool              is_continuous_tx()                               = 0;
  virtual bool              get_is_start_of_burst(const uint32_t& radio_idx) = 0;
  virtual bool              is_init()                                        = 0;
  virtual void              reset()                                          = 0;
  virtual srslte_rf_info_t* get_info(const uint32_t& radio_idx)              = 0;
};

class phy_interface_radio
{
public:
  virtual void radio_overflow() = 0;
  virtual void radio_failure()  = 0;
};

} // namespace srslte

#endif // SRSLTE_COMMON_INTERFACES_H
