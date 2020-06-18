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

/**
 * @file radio_null.h
 * @brief Dummy radio class
 */

#ifndef SRSLTE_RADIO_NULL_H
#define SRSLTE_RADIO_NULL_H

#include "radio_base.h"
#include "srslte/common/logger.h"
#include "srslte/common/logmap.h"
#include "srslte/interfaces/radio_interfaces.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/radio/radio.h"
#include "srslte/radio/radio_metrics.h"

namespace srslte {

class radio_null final : public radio_base, public radio_interface_phy
{
public:
  explicit radio_null(srslte::logger* logger_);
  ~radio_null() final = default;

  std::string get_type() override { return "null"; }

  int init(const rf_args_t& args_, phy_interface_radio* phy_) override
  {
    log->set_level(args.log_level);
    running = true;

    return SRSLTE_SUCCESS;
  }

  void stop() override { running = false; }

  bool get_metrics(rf_metrics_t* metrics) override
  {
    // do nothing
    return true;
  }

  // radio_interface_phy
  bool is_init() override { return running; }
  void reset() override {}
  bool is_continuous_tx() override { return false; }
  bool tx(rf_buffer_interface& buffer, const rf_timestamp_interface& tx_time) override
  {
    log->info("%s\n", __PRETTY_FUNCTION__);
    return true;
  }

  void tx_end() override { log->info("%s\n", __PRETTY_FUNCTION__); }

  bool rx_now(rf_buffer_interface& buffer, rf_timestamp_interface& rxd_time) override
  {
    log->info("%s\n", __PRETTY_FUNCTION__);
    return true;
  }

  void set_rx_gain(const float& gain) override { log->info("%s\n", __PRETTY_FUNCTION__); }

  void set_rx_gain_th(const float& gain) override { log->info("%s\n", __PRETTY_FUNCTION__); }

  float get_rx_gain() override
  {
    log->info("%s\n", __PRETTY_FUNCTION__);
    return 0.0;
  }

  void set_tx_gain(const float& gain) override { log->info("%s\n", __PRETTY_FUNCTION__); }

  void set_tx_freq(const uint32_t& channel_idx, const double& freq) override { log->info("%s\n", __PRETTY_FUNCTION__); }

  void set_rx_freq(const uint32_t& channel_idx, const double& freq) override { log->info("%s\n", __PRETTY_FUNCTION__); }

  double get_freq_offset() override
  {
    log->info("%s\n", __PRETTY_FUNCTION__);
    return 0.0;
  }

  void set_tx_srate(const double& srate) override { log->info("%s\n", __PRETTY_FUNCTION__); }

  void set_rx_srate(const double& srate) override { log->info("%s\n", __PRETTY_FUNCTION__); }

  void set_channel_rx_offset(uint32_t ch, int32_t offset_samples) override { log->info("%s\n", __PRETTY_FUNCTION__); }

  srslte_rf_info_t* get_info() override
  {
    log->info("%s\n", __PRETTY_FUNCTION__);
    return nullptr;
  }

  bool get_is_start_of_burst() override { return true; }

  void release_freq(const uint32_t& carrier_idx) override { log->info("%s\n", __PRETTY_FUNCTION__); }

protected:
  rf_args_t args = {};

  srslte::log_ref log;
  bool            running = false;

  srslte::rf_metrics_t rf_metrics = {};
  phy_interface_radio* phy        = nullptr;
};
} // namespace srslte

#endif // SRSLTE_RADIO_NULL_H
