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
 * File:        radio_multi.h
 * Description: Class for using multiple srslte::radio's for both eNB/UE
 *****************************************************************************/

#ifndef SRSLTE_RADIO_MULTI_H
#define SRSLTE_RADIO_MULTI_H

#include "srslte/common/logger.h"
#include "srslte/interfaces/common_interfaces.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/radio/radio.h"
#include "srslte/radio/radio_base.h"
#include "srslte/radio/radio_metrics.h"

namespace srslte {

class radio_multi : public radio_base, public radio_interface_phy
{
public:
  radio_multi(srslte::logger* logger_);
  ~radio_multi() override;

  std::string get_type() override;

  int init(const rf_args_t& args_, phy_interface_radio* phy_);

  void stop() override;

  static void rf_msg(srslte_rf_error_t error);
  void        handle_rf_msg(srslte_rf_error_t error);

  bool get_metrics(rf_metrics_t* metrics) override;

  // radio_interface_phy
  bool is_init() override { return radios.at(0)->is_init(); }
  void reset() override { return radios.at(0)->reset(); }
  bool is_continuous_tx() override { return radios.at(0)->is_continuous_tx(); }
  bool get_is_start_of_burst(const uint32_t& radio_idx) override
  {
    return radios.at(radio_idx)->get_is_start_of_burst();
  }
  bool tx(const uint32_t&           radio_idx,
          cf_t*                     buffer[SRSLTE_MAX_PORTS],
          const uint32_t&           nof_samples,
          const srslte_timestamp_t& tx_time) override
  {
    return radios.at(radio_idx)->tx(buffer, nof_samples, tx_time);
  }
  void tx_end() override
  {
    // Send Tx exd to all radios
    for (auto& r : radios) {
      r->tx_end();
    }
  }

  bool rx_now(const uint32_t&     radio_idx,
              cf_t*               buffer[SRSLTE_MAX_PORTS],
              const uint32_t&     nof_samples,
              srslte_timestamp_t* rxd_time) override
  {
    return radios.at(radio_idx)->rx_now(buffer, nof_samples, rxd_time);
  }
  void   set_rx_gain(const uint32_t& radio_idx, const float& gain) override { radios.at(radio_idx)->set_rx_gain(gain); }
  double set_rx_gain_th(const float& gain) override { return radios.at(0)->set_rx_gain_th(gain); }
  float  get_rx_gain(const uint32_t& radio_idx) override { return radios.at(radio_idx)->get_rx_gain(); }
  void   set_tx_freq(const uint32_t& radio_idx, const uint32_t& channel_idx, const double& freq) override
  {
    radios.at(radio_idx)->set_tx_freq(channel_idx, freq);
  }
  void set_rx_freq(const uint32_t& radio_idx, const uint32_t& channel_idx, const double& freq) override
  {
    radios.at(radio_idx)->set_rx_freq(channel_idx, freq);
  }
  double get_freq_offset() override { return radios.at(0)->get_freq_offset(); }
  double get_tx_freq(const uint32_t& radio_idx) override { return radios.at(radio_idx)->get_tx_freq(); }
  double get_rx_freq(const uint32_t& radio_idx) override { return radios.at(radio_idx)->get_rx_freq(); }
  float  get_max_tx_power() override { return args.tx_max_power; }
  float  get_tx_gain_offset() override { return args.tx_gain_offset; }
  float  get_rx_gain_offset() override { return args.rx_gain_offset; }
  void   set_tx_srate(const uint32_t& radio_idx, const double& srate) override
  {
    radios.at(radio_idx)->set_tx_srate(srate);
  }
  void set_rx_srate(const uint32_t& radio_idx, const double& srate) override
  {
    radios.at(radio_idx)->set_rx_srate(srate);
  }
  srslte_rf_info_t* get_info(const uint32_t& radio_idx) override { return radios.at(radio_idx)->get_info(); }

protected:
  rf_args_t args = {};

  std::vector<std::unique_ptr<radio> > radios;

  srslte::logger*    logger = nullptr;
  srslte::log_filter log;
  bool               running = false;

  srslte::rf_metrics_t rf_metrics = {};
  phy_interface_radio* phy        = nullptr;
};
} // namespace srslte

#endif // SRSLTE_RADIO_MULTI_H
