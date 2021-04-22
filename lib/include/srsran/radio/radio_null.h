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

/**
 * @file radio_null.h
 * @brief Dummy radio class
 */

#ifndef SRSRAN_RADIO_NULL_H
#define SRSRAN_RADIO_NULL_H

#include "radio_base.h"
#include "srsran/interfaces/radio_interfaces.h"
#include "srsran/phy/rf/rf.h"
#include "srsran/radio/radio.h"
#include "srsran/radio/radio_metrics.h"

namespace srsran {

class radio_null final : public radio_base, public radio_interface_phy
{
public:
  ~radio_null() final = default;

  std::string get_type() override { return "null"; }

  int init(const rf_args_t& args_, phy_interface_radio* phy_) override
  {
    logger.set_level(srslog::str_to_basic_level(args.log_level));
    running = true;

    return SRSRAN_SUCCESS;
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
    logger.info("%s", __PRETTY_FUNCTION__);
    return true;
  }

  void tx_end() override { logger.info("%s", __PRETTY_FUNCTION__); }

  bool rx_now(rf_buffer_interface& buffer, rf_timestamp_interface& rxd_time) override
  {
    logger.info("%s", __PRETTY_FUNCTION__);
    return true;
  }

  void set_rx_gain(const float& gain) override { logger.info("%s", __PRETTY_FUNCTION__); }

  void set_rx_gain_th(const float& gain) override { logger.info("%s", __PRETTY_FUNCTION__); }

  float get_rx_gain() override
  {
    logger.info("%s", __PRETTY_FUNCTION__);
    return 0.0;
  }

  void set_tx_gain(const float& gain) override { logger.info("%s", __PRETTY_FUNCTION__); }

  void set_tx_freq(const uint32_t& channel_idx, const double& freq) override { logger.info("%s", __PRETTY_FUNCTION__); }

  void set_rx_freq(const uint32_t& channel_idx, const double& freq) override { logger.info("%s", __PRETTY_FUNCTION__); }

  double get_freq_offset() override
  {
    logger.info("%s", __PRETTY_FUNCTION__);
    return 0.0;
  }

  void set_tx_srate(const double& srate) override { logger.info("%s", __PRETTY_FUNCTION__); }

  void set_rx_srate(const double& srate) override { logger.info("%s", __PRETTY_FUNCTION__); }

  void set_channel_rx_offset(uint32_t ch, int32_t offset_samples) override { logger.info("%s", __PRETTY_FUNCTION__); }

  srsran_rf_info_t* get_info() override
  {
    logger.info("%s", __PRETTY_FUNCTION__);
    return nullptr;
  }

  bool get_is_start_of_burst() override { return true; }

  void release_freq(const uint32_t& carrier_idx) override { logger.info("%s", __PRETTY_FUNCTION__); }

protected:
  rf_args_t args = {};

  srslog::basic_logger& logger  = srslog::fetch_basic_logger("RF", false);
  bool                  running = false;

  srsran::rf_metrics_t rf_metrics = {};
  phy_interface_radio* phy        = nullptr;
};
} // namespace srsran

#endif // SRSRAN_RADIO_NULL_H
