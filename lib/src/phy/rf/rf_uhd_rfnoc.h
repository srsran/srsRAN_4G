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

#include <uhd/version.hpp>

// RF-NOC is only available for UHD 3.15 LTS
#if UHD_VERSION == 3150000

#ifndef SRSLTE_RF_UHD_RFNOC_H
#define SRSLTE_RF_UHD_RFNOC_H

#include <chrono>
#include <complex>
#include <csignal>
#include <fstream>
#include <iostream>
#include <thread>

#include <uhd/device3.hpp>
#include <uhd/error.h>
#include <uhd/rfnoc/block_ctrl.hpp>
#include <uhd/rfnoc/ddc_block_ctrl.hpp>
#include <uhd/rfnoc/dma_fifo_block_ctrl.hpp>
#include <uhd/rfnoc/duc_block_ctrl.hpp>
#include <uhd/rfnoc/graph.hpp>
#include <uhd/rfnoc/radio_ctrl.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/utils/safe_main.hpp>

#include <uhd/utils/log.hpp>
#define Debug(message) UHD_LOG_DEBUG("UHD RF", message)

#include "rf_uhd_safe.h"

class rf_uhd_rfnoc : public rf_uhd_safe_interface
{
private:
  uhd::device3::sptr      device3;
  uhd::rfnoc::graph::sptr tx_graph;
  uhd::rfnoc::graph::sptr rx_graph;

  // Constant parameters
  const std::string RADIO_BLOCK_NAME    = "Radio";
  const std::string DDC_BLOCK_NAME      = "DDC";
  const std::string DUC_BLOCK_NAME      = "DUC";
  const std::string DMA_FIFO_BLOCK_NAME = "DmaFIFO";
  const double      SETUP_TIME_S        = 1;

  // Primary parameters
  double              master_clock_rate = 184.32e6;
  double              srate_hz          = 0.0;
  double              bw_hz             = 0.0;
  size_t              nof_radios        = 1;
  size_t              nof_channels      = 1;
  double              rx_gain_db        = 10.0;
  double              tx_gain_db        = 5.0;
  std::vector<double> rx_freq_hz;
  std::vector<double> tx_freq_hz;
  std::string         sync_source = "internal";

  // Radio control
  std::vector<uhd::rfnoc::radio_ctrl::sptr> radio_ctrl    = {};
  std::vector<uhd::rfnoc::block_id_t>       radio_ctrl_id = {};

  // DDC Control
  std::vector<uhd::rfnoc::ddc_block_ctrl::sptr> ddc_ctrl    = {};
  std::vector<uhd::rfnoc::block_id_t>           ddc_ctrl_id = {};

  // DUC Control
  std::vector<uhd::rfnoc::duc_block_ctrl::sptr> duc_ctrl    = {};
  std::vector<uhd::rfnoc::block_id_t>           duc_ctrl_id = {};

  // DMA FIFO Control
  uhd::rfnoc::dma_fifo_block_ctrl::sptr dma_fifo_ctrl    = {};
  uhd::rfnoc::block_id_t                dma_fifo_ctrl_id = {};

  template <class T>
  uhd_error parse_param(uhd::device_addr_t& args, const std::string& param, T& value)
  {
    UHD_SAFE_C_SAVE_ERROR(this,
                          // Check if parameter exists
                          if (not args.has_key(param)) {
                            last_error = "RF-NOC requires " + param + " parameter";
                            return UHD_ERROR_KEY;
                          }

                          // Parse parameter
                          value = args.cast(param, value);

                          // Remove parameter from list
                          args.pop(param);)
  }

  uhd_error parse_args(uhd::device_addr_t& args)
  {
    uhd_error err;

    err = parse_param(args, "rfnoc_srate", srate_hz);
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    parse_param(args, "rfnoc_nof_radios", nof_radios);
    if (nof_radios == 0) {
      last_error = "RF-NOC Number of radios cannot be zero";
      return UHD_ERROR_KEY;
    }

    parse_param(args, "rfnoc_nof_channels", nof_channels);
    if (nof_channels == 0) {
      last_error = "RF-NOC Number of channels cannot be zero";
      return UHD_ERROR_KEY;
    }

    // Optional parameters, ignore error return
    parse_param(args, "rfnoc_tx_gain", tx_gain_db);
    parse_param(args, "rfnoc_rx_gain", tx_gain_db);

    // Parse Rx frequencies
    rx_freq_hz.resize(nof_radios * nof_channels);
    for (size_t radio_idx = 0; radio_idx < nof_radios; radio_idx++) {
      for (size_t channel_idx = 0; channel_idx < nof_channels; channel_idx++) {
        std::stringstream ss;
        ss << "rfnoc_rx_" << radio_idx << "_" << channel_idx << "_freq";
        err = parse_param(args, ss.str(), rx_freq_hz[radio_idx * nof_channels + channel_idx]);
        if (err != UHD_ERROR_NONE) {
          return err;
        }
      }
    }

    // Parse Rx frequencies
    tx_freq_hz.resize(nof_radios * nof_channels);
    for (size_t radio_idx = 0; radio_idx < nof_radios; radio_idx++) {
      for (size_t channel_idx = 0; channel_idx < nof_channels; channel_idx++) {
        std::stringstream ss;
        ss << "rfnoc_tx_" << radio_idx << "_" << channel_idx << "_freq";
        err = parse_param(args, ss.str(), tx_freq_hz[radio_idx * nof_channels + channel_idx]);
        if (err != UHD_ERROR_NONE) {
          return err;
        }
      }
    }

    // Secondary parameters
    bw_hz = srate_hz * (1200.0 / 1536.0);

    return UHD_ERROR_NONE;
  }

  uhd_error create_control_interfaces()
  {
    UHD_SAFE_C_SAVE_ERROR(
        this,
        // Create Radio control
        radio_ctrl.resize(nof_radios);
        radio_ctrl_id.resize(nof_radios);
        for (size_t i = 0; i < nof_radios; i++) {
          // Create handle for radio object
          radio_ctrl_id[i] = uhd::rfnoc::block_id_t(0, RADIO_BLOCK_NAME, i);
          // This next line will fail if the radio is not actually available
          radio_ctrl[i] = device3->get_block_ctrl<uhd::rfnoc::radio_ctrl>(radio_ctrl_id[i]);
          Debug("Using radio " << i);
        }

        // Create DDC control
        ddc_ctrl.resize(nof_radios);
        ddc_ctrl_id.resize(nof_radios);
        for (size_t i = 0; i < nof_radios; i++) {
          ddc_ctrl_id[i] = uhd::rfnoc::block_id_t(0, DDC_BLOCK_NAME, i);
          ddc_ctrl[i]    = device3->get_block_ctrl<uhd::rfnoc::ddc_block_ctrl>(ddc_ctrl_id[i]);
        }

        // Create DUC control
        duc_ctrl.resize(nof_radios);
        duc_ctrl_id.resize(nof_radios);
        for (size_t i = 0; i < nof_radios; i++) {
          duc_ctrl_id[i] = uhd::rfnoc::block_id_t(0, DUC_BLOCK_NAME, i);
          duc_ctrl[i]    = device3->get_block_ctrl<uhd::rfnoc::duc_block_ctrl>(duc_ctrl_id[i]);
        }

        // Create DMA FIFO control
        dma_fifo_ctrl_id = uhd::rfnoc::block_id_t(0, DMA_FIFO_BLOCK_NAME);
        dma_fifo_ctrl    = device3->get_block_ctrl<uhd::rfnoc::dma_fifo_block_ctrl>(dma_fifo_ctrl_id);)
  }

  uhd_error configure()
  {
    UHD_SAFE_C_SAVE_ERROR(
        this, std::vector<double> rx_center_freq_hz(nof_radios); std::vector<double> tx_center_freq_hz(nof_radios);

        // Calculate center frequencies from averages
        for (size_t radio_idx = 0; radio_idx < nof_radios; radio_idx++) {
          double sum_tx = 0.0;
          double sum_rx = 0.0;

          for (size_t channel_idx = 0; channel_idx < nof_channels; channel_idx++) {
            sum_tx += tx_freq_hz[radio_idx * nof_channels + channel_idx];
            sum_rx += rx_freq_hz[radio_idx * nof_channels + channel_idx];
          }

          tx_center_freq_hz[radio_idx] = sum_tx / nof_channels;
          rx_center_freq_hz[radio_idx] = sum_rx / nof_channels;
        }

        // Configure radios
        for (size_t i = 0; i < nof_radios; ++i) {
          Debug(boost::format("Setting radio %i...") % i);
          // Lock mboard clocks
          Debug("Setting sync source to " << sync_source) radio_ctrl[i]->set_clock_source(sync_source);
          radio_ctrl[i]->set_time_source(sync_source);

          // Set sample rate
          Debug(boost::format("Setting TX/RX Rate: %f Msps...") % (master_clock_rate / 1e6));
          radio_ctrl[i]->set_rate(master_clock_rate);
          Debug(boost::format("Actual TX/RX Rate: %f Msps...") % (radio_ctrl[i]->get_rate() / 1e6));

          // Set tx freq
          Debug(boost::format("Setting TX Freq: %f MHz...") % (tx_center_freq_hz[i] / 1e6));
          radio_ctrl[i]->set_tx_frequency(tx_center_freq_hz[i], 0);
          tx_center_freq_hz[i] = radio_ctrl[i]->get_tx_frequency(0);
          Debug(boost::format("Actual TX Freq: %f MHz...") % (tx_center_freq_hz[i] / 1e6));

          // Set rx freq
          Debug(boost::format("Setting RX Freq: %f MHz...") % (rx_center_freq_hz[i] / 1e6));
          radio_ctrl[i]->set_rx_frequency(rx_center_freq_hz[i], 0);
          rx_center_freq_hz[i] = radio_ctrl[i]->get_rx_frequency(0);
          Debug(boost::format("Actual RX Freq: %f MHz...") % (rx_center_freq_hz[i] / 1e6));

          // set the IF filter bandwidth
          Debug(boost::format("Setting RX Bandwidth: %f MHz...") % (bw_hz / 1e6));
          radio_ctrl[i]->set_rx_bandwidth(bw_hz, 0);
          Debug(boost::format("Actual RX Bandwidth: %f MHz...") % (radio_ctrl[i]->get_rx_bandwidth(0) / 1e6));

          // set the rf gain
          Debug(boost::format("Setting RX Gain: %f dB...") % (rx_gain_db));
          radio_ctrl[i]->set_rx_gain(rx_gain_db, 0);
          Debug(boost::format("Actual RX Gain: %f dB...") % (radio_ctrl[i]->get_rx_gain(0)));

          // set the antenna
          radio_ctrl[i]->set_rx_antenna("TX/RX", 0);
        }

        // Sleep for some time
        std::this_thread::sleep_for(std::chrono::milliseconds(int64_t(1000 * SETUP_TIME_S)));

        // Setup DDCs and DUCs
        for (size_t radio_idx = 0; radio_idx < nof_radios; radio_idx++) {
          for (size_t channel_idx = 0; channel_idx < nof_channels; channel_idx++) {
            uhd::device_addr_t ddc_args;
            ddc_args.set("input_rate", std::to_string(master_clock_rate));
            ddc_args.set(
                "freq",
                std::to_string(rx_freq_hz[nof_channels * radio_idx + channel_idx] - rx_center_freq_hz[radio_idx]));
            ddc_args.set("fullscale", "1.0");
            ddc_args.set("output_rate", std::to_string(srate_hz));
            Debug("Configure " << ddc_ctrl_id[radio_idx] << ":" << channel_idx << "with args " << ddc_args.to_string());

            ddc_ctrl[radio_idx]->set_args(ddc_args, channel_idx);

            uhd::device_addr_t duc_args;
            duc_args.set("input_rate", std::to_string(srate_hz));
            duc_args.set(
                "freq",
                std::to_string(tx_freq_hz[nof_channels * radio_idx + channel_idx] - tx_center_freq_hz[radio_idx]));
            duc_args.set("fullscale", "1.0");
            duc_args.set("output_rate", std::to_string(master_clock_rate));

            Debug("Configure " << duc_ctrl_id[radio_idx] << ":" << channel_idx << "with args " << duc_args.to_string());

            duc_ctrl[radio_idx]->set_args(duc_args, channel_idx);

            // Setup DMA FIFO
            uhd::device_addr_t dma_fifo_args;
            Debug("Configure " << dma_fifo_ctrl_id << ":" << nof_channels * radio_idx + channel_idx << " with args "
                               << dma_fifo_args.to_string());

            dma_fifo_ctrl->set_args(dma_fifo_args, nof_channels * radio_idx + channel_idx);
          }
        })
  }

  uhd_error connect()
  {
    UHD_SAFE_C_SAVE_ERROR(this, for (size_t radio_idx = 0; radio_idx < nof_radios; radio_idx++) {
      // Radio -> DDC
      Debug("Connecting " << radio_ctrl_id[radio_idx] << " -> " << ddc_ctrl_id[radio_idx]);
      rx_graph->connect(radio_ctrl_id[radio_idx], 0, ddc_ctrl_id[radio_idx], 0);

      // DUC -> Radio
      Debug("Connecting " << duc_ctrl_id[radio_idx] << " -> " << radio_ctrl_id[radio_idx]);
      tx_graph->connect(duc_ctrl_id[radio_idx], 0, radio_ctrl_id[radio_idx], 0);

      // DMA FIFO -> DUC
      for (size_t channel_idx = 0; channel_idx < nof_channels; channel_idx++) {
        Debug("Connecting " << dma_fifo_ctrl_id << ":" << radio_idx * nof_channels + channel_idx << " -> "
                            << radio_ctrl_id[radio_idx] << ":" << channel_idx);
        tx_graph->connect(
            dma_fifo_ctrl_id, radio_idx * nof_channels + channel_idx, duc_ctrl_id[radio_idx], channel_idx);
      }
    })
  }

  uhd_error get_streams()
  {
    UHD_SAFE_C_SAVE_ERROR(
        this, uhd::stream_args_t tx_stream_args("fc32", "sc16"); uhd::stream_args_t rx_stream_args("fc32", "sc16");

        tx_stream_args.channels.resize(nof_radios * nof_channels);
        rx_stream_args.channels.resize(nof_radios * nof_channels);

        // Populate stream arguments with RF-NOC blocks interfaces
        size_t channel_count = 0;
        for (size_t radio_idx = 0; radio_idx < nof_radios; radio_idx++) {
          for (size_t channel_idx = 0; channel_idx < nof_channels; channel_idx++, channel_count++) {
            std::string channel_str = std::to_string(channel_count);

            tx_stream_args.args["block_id" + channel_str]   = DMA_FIFO_BLOCK_NAME + "_0";
            tx_stream_args.args["block_port" + channel_str] = std::to_string(radio_idx * nof_channels + channel_idx);
            tx_stream_args.channels[channel_count]          = channel_count;

            rx_stream_args.args["block_id" + channel_str]   = DDC_BLOCK_NAME + "_" + std::to_string(radio_idx);
            rx_stream_args.args["block_port" + channel_str] = std::to_string(channel_idx);
            rx_stream_args.channels[channel_count]          = channel_count;
          }
        }

        tx_stream = device3->get_tx_stream(tx_stream_args);
        rx_stream = device3->get_rx_stream(rx_stream_args);)
  }

  uhd_error usrp_get_device3()
  {
    UHD_SAFE_C_SAVE_ERROR(this,
                          // Check if device is RF-NOC compatible
                          if (not usrp->is_device3()) {
                            last_error = "The selected device is not compatible with RF-NOC";
                            return UHD_ERROR_INVALID_DEVICE;
                          }

                          // Get device 3 shared pointer
                          device3 = usrp->get_device3();)
  }

public:
  static bool is_required(const uhd::device_addr_t& device_addr)
  {
    const std::vector<std::string> keys = device_addr.keys();

    for (const std::string& k : keys) {
      if (k.find("rfnoc") != std::string::npos) {
        return true;
      }
    }

    return false;
  }

  uhd_error usrp_make(const uhd::device_addr_t& _device_addr) override
  {
    // Copy device address
    uhd::device_addr_t device_addr = _device_addr;

    // Set log level to debug
    uhd::log::set_console_level(uhd::log::severity_level::trace);

    // Parse arguments, removes
    uhd_error err = parse_args(device_addr);
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    // Make USRP
    err = usrp_multi_make(device_addr);
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    err = usrp_get_device3();
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    // Reset blocks after a stream
    device3->clear();

    // Create control interfaces
    err = create_control_interfaces();
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    return UHD_ERROR_NONE;
  }

  uhd_error set_tx_subdev(const std::string& string) override { return UHD_ERROR_NONE; }
  uhd_error set_rx_subdev(const std::string& string) override { return UHD_ERROR_NONE; }
  uhd_error set_time_unknown_pps(const uhd::time_spec_t& timespec) override { return UHD_ERROR_NONE; }
  uhd_error set_time_now(const uhd::time_spec_t& timespec) override { return UHD_ERROR_NONE; }
  uhd_error get_time_now(uhd::time_spec_t& timespec) override { return UHD_ERROR_NONE; }
  uhd_error set_sync_source(const std::string& source) override
  {
    sync_source = source;
    return UHD_ERROR_NONE;
  }
  uhd_error set_master_clock_rate(double rate) override { return UHD_ERROR_NONE; }
  uhd_error set_rx_rate(double rate) override { return UHD_ERROR_NONE; }
  uhd_error set_tx_rate(double rate) override { return UHD_ERROR_NONE; }
  uhd_error set_command_time(const uhd::time_spec_t& timespec) override { return UHD_ERROR_NONE; }
  uhd_error get_rx_stream(const uhd::stream_args_t& args, size_t& max_num_samps) override
  {
    uhd_error err;

    // Get Tx and Rx Graph
    tx_graph = device3->create_graph("tx_graph");
    rx_graph = device3->create_graph("rx_graph");

    // Configure components
    err = configure();
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    // Connect components
    err = connect();
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    // Get streams
    err = get_streams();
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    max_num_samps = rx_stream->get_max_num_samps();
    return UHD_ERROR_NONE;
  }
  uhd_error get_tx_stream(const uhd::stream_args_t& args, size_t& max_num_samps) override
  {
    max_num_samps = tx_stream->get_max_num_samps();
    return UHD_ERROR_NONE;
  }
  uhd_error set_tx_gain(size_t ch, double gain) override { return UHD_ERROR_NONE; }
  uhd_error set_rx_gain(size_t ch, double gain) override { return UHD_ERROR_NONE; }
  uhd_error get_rx_gain(double& gain) override { return UHD_ERROR_NONE; }
  uhd_error get_tx_gain(double& gain) override { return UHD_ERROR_NONE; }
  uhd_error set_tx_freq(uint32_t ch, double target_freq, double& actual_freq) override
  {
    if (ch < tx_freq_hz.size()) {
      tx_freq_hz[ch] = target_freq;
      actual_freq    = tx_freq_hz[ch];
    }

    return UHD_ERROR_NONE;
  }
  uhd_error set_rx_freq(uint32_t ch, double target_freq, double& actual_freq) override
  {
    if (ch < rx_freq_hz.size()) {
      rx_freq_hz[ch] = target_freq;
      actual_freq    = rx_freq_hz[ch];
    }

    return UHD_ERROR_NONE;
  }
};

#endif // SRSLTE_RF_UHD_RFNOC_H
#endif // UHD_VERSION == 3150000