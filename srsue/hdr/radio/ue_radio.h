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
 * File:        ue_radio_multi.h
 * Description: UE radio module using the srslte_radio_multi() object.
 *****************************************************************************/

#ifndef SRSUE_UE_RADIO_MULTI_H
#define SRSUE_UE_RADIO_MULTI_H

#include "srslte/common/log_filter.h"
#include "srslte/radio/radio.h"
#include "srsue/hdr/radio/ue_radio_base.h"
#include "srsue/hdr/ue_metrics_interface.h"

namespace srsue {

/*******************************************************************************
  Main UE stack class
*******************************************************************************/

class ue_radio : public ue_radio_base, public radio_interface_phy
{
public:
  ue_radio();
  ~ue_radio();

  std::string get_type();

  int init(const rf_args_t& args_, srslte::logger* logger_);
  int init(const rf_args_t& args_, srslte::logger* logger_, phy_interface_radio* phy_);

  void stop();

  static void rf_msg(srslte_rf_error_t error);
  void        handle_rf_msg(srslte_rf_error_t error);

  bool get_metrics(rf_metrics_t* metrics);

  // radio_interface_phy
  bool is_init() { return radios.at(0)->is_init(); }
  void reset() { return radios.at(0)->reset(); }
  bool is_continuous_tx() { return radios.at(0)->is_continuous_tx(); }
  bool tx(cf_t* buffer[SRSLTE_MAX_PORTS], const uint32_t& nof_samples, const srslte_timestamp_t& tx_time)
  {
    for (auto& radio : radios) {
      radio->tx(buffer, nof_samples, tx_time);
    }
    return true;
  }
  void tx_end() { return radios.at(0)->tx_end(); }

  bool rx_now(cf_t* buffer[SRSLTE_MAX_PORTS], const uint32_t& nof_samples, srslte_timestamp_t* rxd_time)
  {
    return radios.at(0)->rx_now(buffer, nof_samples, rxd_time);
  }
  void   set_rx_gain(const uint32_t& radio_idx, const float& gain) { radios.at(radio_idx)->set_rx_gain(gain); }
  double set_rx_gain_th(const float& gain) { return radios.at(0)->set_rx_gain_th(gain); }
  float  get_rx_gain(const uint32_t& radio_idx) { return radios.at(radio_idx)->get_rx_gain(); }
  void   set_tx_freq(const uint32_t& radio_idx, const double& freq) { radios.at(radio_idx)->set_tx_freq(0, freq); }
  void   set_rx_freq(const uint32_t& radio_idx, const double& freq) { radios.at(radio_idx)->set_rx_freq(0, freq); }
  double get_freq_offset() { return radios.at(0)->get_freq_offset(); }
  double get_tx_freq(const uint32_t& radio_idx) { return radios.at(radio_idx)->get_tx_freq(); }
  double get_rx_freq(const uint32_t& radio_idx) { return radios.at(radio_idx)->get_rx_freq(); }
  float  get_max_tx_power() { return radios.at(0)->get_max_tx_power(); }
  void   set_master_clock_rate(const double& rate) { radios.at(0)->set_master_clock_rate(rate); }
  void   set_tx_srate(const double& srate) { radios.at(0)->set_tx_srate(srate); }
  void   set_rx_srate(const double& srate) { radios.at(0)->set_rx_srate(srate); }
  float  set_tx_power(const float& power) { return radios.at(0)->set_tx_power(power); }
  srslte_rf_info_t* get_info(const uint32_t& radio_idx) { return radios.at(radio_idx)->get_info(); }

private:
  srsue::rf_args_t args;

  std::vector<std::unique_ptr<srslte::radio> > radios;

  srslte::logger*    logger;
  srslte::log_filter log;
  bool               running;

  rf_metrics_t rf_metrics;
  phy_interface_radio* phy;
};

} // namespace srsue

#endif // SRSUE_UE_RADIO_MULTI_H
