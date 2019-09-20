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

#include "srslte/radio/radio_multi.h"
#include <mutex>

namespace srslte {

std::mutex          radio_instance_mutex;
static radio_multi* instance;

radio_multi::radio_multi(srslte::logger* logger_) : logger(logger_), radio_base(logger_)
{
  std::lock_guard<std::mutex> lock(radio_instance_mutex);
  instance = this;
}

radio_multi::~radio_multi()
{
  stop();
}

std::string radio_multi::get_type()
{
  return "radio_multi";
}

int radio_multi::init(const rf_args_t& args_, phy_interface_radio* phy_)
{
  args   = args_;
  phy    = phy_;

  // Init log
  log.init("RF  ", logger);
  log.set_level(args.log_level);

  if (args.nof_radios > SRSLTE_MAX_RADIOS) {
    log.error("Maximum supported number of radios exceeded (%d > %d)\n", args.nof_radios, SRSLTE_MAX_RADIOS);
    return SRSLTE_ERROR;
  }

  // Init and start Radio
  char* dev_name = nullptr;
  if (args.device_name != "auto") {
    dev_name = (char*)args.device_name.c_str();
  }

  char* dev_args[SRSLTE_MAX_RADIOS] = {nullptr};
  for (int i = 0; i < SRSLTE_MAX_RADIOS; i++) {
    if (args.device_args[i] != "auto") {
      dev_args[i] = (char*)args.device_args[i].c_str();
    }
  }

  log.console(
      "Opening %d RF devices with %d RF channels...\n", args.nof_radios, args.nof_rf_channels * args.nof_rx_ant);
  for (uint32_t r = 0; r < args.nof_radios; r++) {
    std::unique_ptr<srslte::radio> radio = std::unique_ptr<srslte::radio>(new srslte::radio());
    if (!radio->init(&log, dev_args[r], dev_name, args.nof_rf_channels * args.nof_rx_ant)) {
      log.console("Failed to find device %s with args %s\n", args.device_name.c_str(), args.device_args[0].c_str());
      return SRSLTE_ERROR;
    }

    // Set RF options
    if (args.time_adv_nsamples != "auto") {
      int t = (int)strtol(args.time_adv_nsamples.c_str(), nullptr, 10);
      radio->set_tx_adv(abs(t));
      radio->set_tx_adv_neg(t < 0);
    }
    if (args.burst_preamble != "auto") {
      radio->set_burst_preamble(strtof(args.burst_preamble.c_str(), nullptr));
    }
    if (args.continuous_tx != "auto") {
      radio->set_continuous_tx(args.continuous_tx == "yes");
    }

    // Set PHY options
    if (args.rx_gain < 0) {
      radio->start_agc(false);
    } else {
      radio->set_rx_gain(args.rx_gain);
    }
    if (args.tx_gain > 0) {
      radio->set_tx_gain(args.tx_gain);
    } else {
      radio->set_tx_gain(args.rx_gain);
      log.console("\nWarning: TX gain was not set. Using open-loop power control (not working properly)\n\n");
    }

    radio->register_error_handler(rf_msg);
    radio->set_freq_offset(args.freq_offset);

    // append to radios
    radios.push_back(std::move(radio));
  }

  running = true;

  return SRSLTE_SUCCESS;
}

void radio_multi::stop()
{
  if (running) {
    for (auto& radio : radios) {
      radio->stop();
    }

    std::lock_guard<std::mutex> lock(radio_instance_mutex);
    instance = nullptr;

    running = false;
  }
}

bool radio_multi::get_metrics(rf_metrics_t* metrics)
{
  *metrics = rf_metrics;
  rf_metrics = {};
  return true;
}

void radio_multi::rf_msg(srslte_rf_error_t error)
{
  std::lock_guard<std::mutex> lock(radio_instance_mutex);
  if (instance) {
    instance->handle_rf_msg(error);
  }
}

void radio_multi::handle_rf_msg(srslte_rf_error_t error)
{
  if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_OVERFLOW) {
    rf_metrics.rf_o++;
    rf_metrics.rf_error = true;
    log.info("Overflow\n");

    // inform PHY about overflow
    phy->radio_overflow();
  } else if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_UNDERFLOW) {
    rf_metrics.rf_u++;
    rf_metrics.rf_error = true;
    log.info("Underflow\n");
  } else if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_LATE) {
    rf_metrics.rf_l++;
    rf_metrics.rf_error = true;
    log.info("Late (detected in %s)\n", error.opt ? "rx call" : "asynchronous thread");
  } else if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_RX) {
    log.error("Fatal radio error occured.\n");
    phy->radio_failure();
  } else if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_OTHER) {
    std::string str(error.msg);
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    str.push_back('\n');
    log.info("%s\n", str.c_str());
  }
}

} // namespace srslte
