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
#include <uhd/version.hpp>

// Load custom RFNOC blocks if available
#ifdef UHD_ENABLE_CUSTOM_RFNOC
#include <uhd/rfnoc/ddc_ch2_block_ctrl.hpp>
#include <uhd/rfnoc/duc_ch2_block_ctrl.hpp>
#endif // UHD_ENABLE_CUSTOM_RFNOC

#include "rf_uhd_safe.h"

class rf_uhd_rfnoc : public rf_uhd_safe_interface
{
private:
  uhd::device3::sptr      device3;
  uhd::rfnoc::graph::sptr graph;

  // Constant parameters
  const std::string RADIO_BLOCK_NAME = "Radio";
  const std::string DDC_BLOCK_NAME   = "DDC";
  const std::string DUC_BLOCK_NAME   = "DUC";
#ifdef UHD_ENABLE_CUSTOM_RFNOC
  const std::string DDC_CUSTOM_BLOCK_NAME = "DDCch2";
  const std::string DUC_CUSTOM_BLOCK_NAME = "DUCch2";
#endif // UHD_ENABLE_CUSTOM_RFNOC

  const std::string               DMA_FIFO_BLOCK_NAME = "DmaFIFO";
  const std::string               FIFO_BLOCK_NAME     = "FIFO";
  const std::chrono::milliseconds SETUP_TIME_MS       = std::chrono::milliseconds(1000UL);
  const uhd::fs_path              TREE_MBOARD_SENSORS = "/mboards/0/sensors";
  const uhd::fs_path              TREE_RX_SENSORS     = "/mboards/0/dboards/A/rx_frontends/0/sensors";
  const uhd::fs_path              TREE_TIME_NOW       = "/mboards/0/time/now";
  const std::string               TX_ANTENNA_PORT     = "TX/RX";
  const std::string               RX_ANTENNA_PORT     = "RX2";
  const size_t                    spp                 = 1920 / 8;

  // Primary parameters
  bool                loopback          = false;
  double              master_clock_rate = 184.32e6;
  double              bw_hz             = 100.0;
  size_t              nof_radios        = 1; ///< Number of RF devices in the device
  size_t              nof_channels      = 1; ///< Number of Channels per Radio
  std::vector<double> rx_freq_hz;
  std::vector<double> rx_center_freq_hz;
  std::vector<double> tx_freq_hz;
  std::vector<double> tx_center_freq_hz;
  size_t              dma_fifo_depth = 8192UL * 4096UL;

  // Radio control
  std::vector<uhd::rfnoc::radio_ctrl::sptr> radio_ctrl = {};
  std::vector<uhd::rfnoc::block_id_t>       radio_id   = {};

  // DDC Control
  std::vector<uhd::rfnoc::block_ctrl_base::sptr> ddc_ctrl = {};
  std::vector<uhd::rfnoc::block_id_t>            ddc_id   = {};

  // DUC Control
  std::vector<uhd::rfnoc::block_ctrl_base::sptr> duc_ctrl = {};
  std::vector<uhd::rfnoc::block_id_t>            duc_id   = {};

  // DMA FIFO Control
  bool                                           use_dma   = false;
  std::vector<uhd::rfnoc::block_ctrl_base::sptr> fifo_ctrl = {};
  std::vector<uhd::rfnoc::block_id_t>            fifo_id   = {};

  uhd_error usrp_make_internal(const uhd::device_addr_t& dev_addr) override
  {
    // Destroy any previous USRP instance
    device3 = nullptr;

    UHD_SAFE_C_SAVE_ERROR(this, device3 = uhd::device3::make(dev_addr);)
  }

  template <class T>
  uhd_error parse_param(uhd::device_addr_t& args, const std::string& param, T& value, bool pop = true)
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
                          if (pop) args.pop(param);)
  }

  uhd_error parse_args(uhd::device_addr_t& args)
  {
    // Parse master clock rate
    parse_param(args, "master_clock_rate", master_clock_rate, false);

    // Parser loopback (optional)
    loopback = args.has_key("loopback");
    if (loopback) {
      args.pop("loopback");
      Warning("RFNOC Loopback activated!")
    }

    // Parse number of radios
    parse_param(args, "rfnoc_nof_radios", nof_radios);
    if (nof_radios == 0) {
      last_error = "RF-NOC Number of radios cannot be zero";
      return UHD_ERROR_KEY;
    }

    // Parse number of channels per radio
    parse_param(args, "rfnoc_nof_channels", nof_channels);
    if (nof_channels == 0) {
      last_error = "RF-NOC Number of channels cannot be zero";
      return UHD_ERROR_KEY;
    }

    // Set secondary parameters
    bw_hz = master_clock_rate / 2.0;
    tx_freq_hz.resize(nof_radios * nof_channels);
    tx_center_freq_hz.resize(nof_radios);
    rx_freq_hz.resize(nof_radios * nof_channels);
    rx_center_freq_hz.resize(nof_radios);

    return UHD_ERROR_NONE;
  }

  uhd_error create_control_interfaces()
  {
    UHD_SAFE_C_SAVE_ERROR(
        this,
        // Create Radio control
        if (not loopback) {
          radio_ctrl.resize(nof_radios);
          radio_id.resize(nof_radios);
        }

        for (size_t i = 0; i < radio_ctrl.size(); i++) {
          // Create handle for radio object
          radio_id[i] = uhd::rfnoc::block_id_t(0, RADIO_BLOCK_NAME, i);
          // This next line will fail if the radio is not actually available
          radio_ctrl[i] = device3->get_block_ctrl<uhd::rfnoc::radio_ctrl>(radio_id[i]);

          // Set sample rate
          UHD_LOG_DEBUG(radio_id[i], "Setting TX/RX Rate: " << master_clock_rate / 1e6 << " Msps...");
          radio_ctrl[i]->set_rate(master_clock_rate);
          UHD_LOG_DEBUG(radio_id[i], "Actual TX/RX Rate:" << radio_ctrl[i]->get_rate() / 1e6 << " Msps...");

          // set the IF filter bandwidth
          UHD_LOG_DEBUG(radio_id[i], "Setting RX Bandwidth: " << bw_hz / 1e6 << " MHz...");
          radio_ctrl[i]->set_rx_bandwidth(bw_hz, 0);
          UHD_LOG_DEBUG(radio_id[i], "Actual RX Bandwidth: " << radio_ctrl[i]->get_rx_bandwidth(0) / 1e6 << " MHz...");

          // set Rx antenna
          UHD_LOG_DEBUG(radio_id[i], "Setting RX antenna: " << RX_ANTENNA_PORT);
          radio_ctrl[i]->set_rx_antenna(RX_ANTENNA_PORT, 0);

          // set Tx antenna
          UHD_LOG_DEBUG(radio_id[i], "Setting TX antenna: " << TX_ANTENNA_PORT);
          radio_ctrl[i]->set_tx_antenna(TX_ANTENNA_PORT, 0);

#if UHD_VERSION >= 3150000
          // API call introduced in UHD 3.15 (commit 67dbaa41)
          radio_ctrl[i]->enable_rx_timestamps(true, 0);
#endif

          if (spp) {
            radio_ctrl[i]->set_arg("spp", spp);
          }
        }

        // Sleep for some time
        std::this_thread::sleep_for(SETUP_TIME_MS);

    // Detect custom DDC
#ifdef UHD_ENABLE_CUSTOM_RFNOC
        std::vector<uhd::rfnoc::block_id_t> custom_ddc_list      = device3->find_blocks(DDC_CUSTOM_BLOCK_NAME);
        bool                                available_custom_ddc = not custom_ddc_list.empty();
#endif // UHD_ENABLE_CUSTOM_RFNOC

        // Create DDC control
        ddc_ctrl.resize(nof_radios);
        ddc_id.resize(nof_radios);
        for (size_t i = 0; i < nof_radios; i++) {

#ifdef UHD_ENABLE_CUSTOM_RFNOC
          if (available_custom_ddc) {
            ddc_id[i]   = uhd::rfnoc::block_id_t(0, DDC_CUSTOM_BLOCK_NAME, i);
            ddc_ctrl[i] = device3->get_block_ctrl<uhd::rfnoc::ddc_ch2_block_ctrl>(ddc_id[i]);
          }
#endif // UHD_ENABLE_CUSTOM_RFNOC

          if (ddc_ctrl[i] == nullptr) {
            ddc_id[i]   = uhd::rfnoc::block_id_t(0, DDC_BLOCK_NAME, i);
            ddc_ctrl[i] = device3->get_block_ctrl<uhd::rfnoc::ddc_block_ctrl>(ddc_id[i]);
            ddc_ctrl[i]->clear();
          }

          for (size_t j = 0; j < nof_channels; j++) {

            uhd::device_addr_t args;
            args.set("input_rate", std::to_string(master_clock_rate));
            args.set("fullscale", "1.0");

            UHD_LOG_DEBUG(ddc_id[i], "Configure channel " << j << " with args " << args.to_string());

            ddc_ctrl[i]->set_args(args, j);
          }
        }

    // Detect custom DUC
#ifdef UHD_ENABLE_CUSTOM_RFNOC
        std::vector<uhd::rfnoc::block_id_t> custom_duc_list      = device3->find_blocks(DUC_CUSTOM_BLOCK_NAME);
        bool                                available_custom_duc = not custom_duc_list.empty();
#endif // UHD_ENABLE_CUSTOM_RFNOC

        // Create DUC control
        duc_ctrl.resize(nof_radios);
        duc_id.resize(nof_radios);
        for (size_t i = 0; i < nof_radios; i++) {
#ifdef UHD_ENABLE_CUSTOM_RFNOC
          if (available_custom_duc) {
            duc_id[i]   = uhd::rfnoc::block_id_t(0, DUC_CUSTOM_BLOCK_NAME, i);
            duc_ctrl[i] = device3->get_block_ctrl<uhd::rfnoc::duc_ch2_block_ctrl>(duc_id[i]);
          }
#endif // UHD_ENABLE_CUSTOM_RFNOC

          if (duc_ctrl[i] == nullptr) {
            duc_id[i]   = uhd::rfnoc::block_id_t(0, DUC_BLOCK_NAME, i);
            duc_ctrl[i] = device3->get_block_ctrl<uhd::rfnoc::duc_block_ctrl>(duc_id[i]);
            duc_ctrl[i]->clear();
          }

          for (size_t j = 0; j < nof_channels; j++) {

            uhd::device_addr_t args;
            args.set("output_rate", std::to_string(master_clock_rate));
            args.set("fullscale", "1.0");

            UHD_LOG_DEBUG(duc_id[i], "Configure channel " << j << " with args " << args.to_string());

            duc_ctrl[i]->set_args(args, j);
          }
        }

        // Check whether using DMA or FIFO
        use_dma = device3->find_blocks(FIFO_BLOCK_NAME).size() < (nof_radios * nof_channels);

        // Create DMA FIFO control
        if (use_dma) {
          // One FIFO for all channels
          fifo_id.resize(1);
          fifo_ctrl.resize(1);

          // Get block
          fifo_id[0]   = uhd::rfnoc::block_id_t(0, DMA_FIFO_BLOCK_NAME);
          fifo_ctrl[0] = device3->get_block_ctrl<uhd::rfnoc::block_ctrl_base>(fifo_id[0]);

          // Configure DMA Channels
          for (size_t i = 0; i < nof_radios * nof_channels; i++) {
            uhd::device_addr_t dma_fifo_args;
            dma_fifo_args.set("base_addr", std::to_string(dma_fifo_depth * i));
            dma_fifo_args.set("depth", std::to_string(dma_fifo_depth));

            UHD_LOG_DEBUG(fifo_id[0], "Setting channel " << i << " args: " << dma_fifo_args.to_string());

            fifo_ctrl[0]->set_args(dma_fifo_args, i);
          }
        } else {
          // One FIFO for every channel
          fifo_id.resize(nof_radios * nof_channels);
          fifo_ctrl.resize(nof_radios * nof_channels);

          for (size_t i = 0; i < nof_radios * nof_channels; i++) {
            // Get block
            fifo_id[i]   = uhd::rfnoc::block_id_t(0, FIFO_BLOCK_NAME, i);
            fifo_ctrl[i] = device3->get_block_ctrl<uhd::rfnoc::block_ctrl_base>(fifo_id[i]);
          }
        })
  }

  uhd_error connect()
  {
    size_t nof_samples_per_packet = 0;

    if (spp != 0) {
      nof_samples_per_packet = spp * 4 + 2 * sizeof(uint64_t);
    }

    UHD_SAFE_C_SAVE_ERROR(
        this,

        // Get Tx and Rx Graph
        graph = device3->create_graph("graph");

        for (size_t i = 0; i < nof_radios; i++) {
          // Radio -> DDC
          UHD_LOG_DEBUG(graph->get_name(), "Connecting " << radio_id[i] << " -> " << ddc_id[i]);
          graph->connect(radio_id[i], 0, ddc_id[i], 0, nof_samples_per_packet);

          // DUC -> Radio
          UHD_LOG_DEBUG(graph->get_name(), "Connecting " << duc_id[i] << " -> " << radio_id[i]);
          graph->connect(duc_id[i], 0, radio_id[i], 0, nof_samples_per_packet);

          if (use_dma) {
            // DMA FIFO -> DUC
            for (size_t j = 0; j < nof_channels; j++) {
              size_t dma_port = i * nof_channels + j;
              UHD_LOG_DEBUG(graph->get_name(),
                            "Connecting " << fifo_id[0] << ":" << dma_port << " -> " << duc_id[i] << ":" << j);
              graph->connect(fifo_id[0], dma_port, duc_id[i], j, nof_samples_per_packet);
            }
          } else {
            // FIFO -> DUC
            for (size_t j = 0; j < nof_channels; j++) {
              size_t fifo_idx = i * nof_channels + j;
              UHD_LOG_DEBUG(graph->get_name(), "Connecting " << fifo_id[fifo_idx] << ":0 -> " << duc_id[i] << ":" << j);
              graph->connect(fifo_id[fifo_idx], 0, duc_id[i], j, nof_samples_per_packet);
            }
          }
        })
  }

  uhd_error connect_loopback()
  {
    size_t nof_samples_per_packet = 0;

    if (spp != 0) {
      nof_samples_per_packet = spp * 4 + 2 * sizeof(uint64_t);
    }

    UHD_SAFE_C_SAVE_ERROR(
        this,

        // Get Tx and Rx Graph
        graph = device3->create_graph("graph");

        for (size_t i = 0; i < nof_radios; i++) {
          // DUC -> DDC
          UHD_LOG_DEBUG(graph->get_name(), "Connecting " << duc_id[i] << " -> " << ddc_id[i]);
          graph->connect(duc_id[i], 0, ddc_id[i], 0, nof_samples_per_packet);

          if (use_dma) {
            for (size_t j = 0; j < nof_channels; j++) {
              // DMA FIFO -> DUC
              size_t dma_port = i * nof_channels + j;
              UHD_LOG_DEBUG(graph->get_name(),
                            "Connecting " << fifo_id[0] << ":" << dma_port << " -> " << duc_id[i] << ":" << j);
              graph->connect(fifo_id[0], dma_port, duc_id[i], j, nof_samples_per_packet);
            }
          } else {
            for (size_t j = 0; j < nof_channels; j++) {
              // DMA FIFO -> DUC
              size_t fifo_idx = i * nof_channels + j;
              UHD_LOG_DEBUG(graph->get_name(), "Connecting " << fifo_id[fifo_idx] << ":0 -> " << duc_id[i] << ":" << j);
              graph->connect(fifo_id[fifo_idx], 0, duc_id[i], j, nof_samples_per_packet);
            }
          }
        })
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

  uhd_error usrp_make(const uhd::device_addr_t& _device_addr, uint32_t nof_channels_) override
  {
    // Copy device address
    uhd::device_addr_t device_addr = _device_addr;

    // Parse arguments, removes
    uhd_error err = parse_args(device_addr);
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    // Check number of channels
    if (nof_channels_ % nof_channels != 0 or nof_channels_ / nof_channels > nof_radios) {
      last_error = "Number of requested channels (" + std::to_string(nof_channels_) +
                   ") is different than the RFNOC "
                   "available channels (" +
                   std::to_string(nof_radios * nof_channels) + ")";
      return UHD_ERROR_VALUE;
    }

    // Re-calculate number of radios
    nof_radios = nof_channels_ / nof_channels;

    // Make USRP
    err = usrp_multi_make(device_addr);
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

    // Connect components
    if (loopback) {
      err = connect_loopback();
    } else {
      err = connect();
    }
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    return UHD_ERROR_NONE;
  }

  uhd_error get_mboard_name(std::string& mboard_name) override
  {
    mboard_name = "X300";
    return UHD_ERROR_NONE;
  };
  uhd_error get_mboard_sensor_names(std::vector<std::string>& sensors) override
  {
    UHD_SAFE_C_SAVE_ERROR(this, if (device3->get_tree()->exists(TREE_MBOARD_SENSORS)) {
      sensors = device3->get_tree()->list(TREE_MBOARD_SENSORS);
    })
  }
  uhd_error get_rx_sensor_names(std::vector<std::string>& sensors) override
  {
    UHD_SAFE_C_SAVE_ERROR(this, if (device3->get_tree()->exists(TREE_RX_SENSORS)) {
      sensors = device3->get_tree()->list(TREE_RX_SENSORS);
    })
  }
  uhd_error get_sensor(const std::string& sensor_name, double& sensor_value) override
  {
    UHD_SAFE_C_SAVE_ERROR(
        this,
        sensor_value =
            device3->get_tree()->access<uhd::sensor_value_t>(TREE_MBOARD_SENSORS / sensor_name).get().to_real();)
  }
  uhd_error get_sensor(const std::string& sensor_name, bool& sensor_value) override
  {
    UHD_SAFE_C_SAVE_ERROR(
        this,
        sensor_value =
            device3->get_tree()->access<uhd::sensor_value_t>(TREE_MBOARD_SENSORS / sensor_name).get().to_bool();)
  }
  uhd_error get_rx_sensor(const std::string& sensor_name, bool& sensor_value) override
  {
    UHD_SAFE_C_SAVE_ERROR(
        this,
        sensor_value = device3->get_tree()->access<uhd::sensor_value_t>(TREE_RX_SENSORS / sensor_name).get().to_bool();)
  }
  uhd_error set_time_unknown_pps(const uhd::time_spec_t& timespec) override
  {
    Info("Setting time " << timespec.get_real_secs() << " at next PPS...");
    UHD_SAFE_C_SAVE_ERROR(this, for (auto& r : radio_ctrl) { r->set_time_next_pps(timespec); });
  }
  uhd_error get_time_now(uhd::time_spec_t& timespec) override
  {
    UHD_SAFE_C_SAVE_ERROR(this, timespec = device3->get_tree()->access<uhd::time_spec_t>(TREE_TIME_NOW).get();
                          Info("-- " << timespec.get_real_secs());)
  }
  uhd_error set_sync_source(const std::string& source) override
  {
    if (loopback) {
      return UHD_ERROR_NONE;
    }

    UHD_SAFE_C_SAVE_ERROR(this, for (size_t radio_idx = 0; radio_idx < nof_radios; radio_idx++) {
      UHD_LOG_DEBUG(radio_id[radio_idx], "Setting sync source to " << source);
      radio_ctrl[radio_idx]->set_clock_source(source);
      radio_ctrl[radio_idx]->set_time_source(source);
    })
  }
  uhd_error get_gain_range(uhd::gain_range_t& tx_gain_range, uhd::gain_range_t& rx_gain_range) override
  {
    // Hard coded for X300
    tx_gain_range = uhd::gain_range_t(.0, 30.0, .5);
    rx_gain_range = uhd::gain_range_t(.0, 30.0, .5);
    return UHD_ERROR_NONE;
  }
  uhd_error set_master_clock_rate(double rate) override { return UHD_ERROR_NONE; }
  uhd_error set_rx_rate(double rate) override
  {
    UHD_SAFE_C_SAVE_ERROR(this, for (size_t i = 0; i < nof_radios; i++) {
      for (size_t j = 0; j < nof_channels; j++) {
        UHD_LOG_DEBUG(ddc_id[i], "Setting channel " << j << " output rate to " << rate / 1e6 << " MHz");
        ddc_ctrl[i]->set_arg("output_rate", std::to_string(rate), j);
      }
    })
  }
  uhd_error set_tx_rate(double rate) override
  {
    UHD_SAFE_C_SAVE_ERROR(this, for (size_t i = 0; i < nof_radios; i++) {
      for (size_t j = 0; j < nof_channels; j++) {
        UHD_LOG_DEBUG(duc_id[i], "Setting channel " << j << " input rate to " << rate / 1e6 << " MHz");
        duc_ctrl[i]->set_arg("input_rate", std::to_string(rate), j);
      }
    })
  }
  uhd_error set_command_time(const uhd::time_spec_t& timespec) override { return UHD_ERROR_NONE; }
  uhd_error get_rx_stream(size_t& max_num_samps) override
  {
    UHD_SAFE_C_SAVE_ERROR(
        this, uhd::stream_args_t stream_args("fc32", "sc16");

        stream_args.channels.resize(nof_radios * nof_channels);

        if (spp != 0) { stream_args.args["spp"] = std::to_string(spp); }

        // Populate stream arguments with RF-NOC blocks interfaces
        size_t channel_count = 0;
        for (size_t i = 0; i < nof_radios; i++) {
          for (size_t j = 0; j < nof_channels; j++, channel_count++) {
            std::string channel_str = std::to_string(channel_count);

            stream_args.args["block_id" + channel_str]   = ddc_id[i];
            stream_args.args["block_port" + channel_str] = std::to_string(j);
            stream_args.channels[channel_count]          = channel_count;
          }
        }

        Debug("Getting Rx Stream with arguments " << stream_args.args.to_pp_string());
        rx_stream     = device3->get_rx_stream(stream_args);
        max_num_samps = rx_stream->get_max_num_samps();)
  }
  uhd_error get_tx_stream(size_t& max_num_samps) override
  {
    UHD_SAFE_C_SAVE_ERROR(
        this, uhd::stream_args_t stream_args("fc32", "sc16");

        stream_args.channels.resize(nof_radios * nof_channels);
        if (spp != 0) { stream_args.args["spp"] = std::to_string(spp); }

        // Populate stream arguments with RF-NOC blocks interfaces
        size_t channel_count = 0;
        for (size_t i = 0; i < nof_radios; i++) {
          for (size_t j = 0; j < nof_channels; j++, channel_count++) {
            std::string channel_str = std::to_string(channel_count);
            if (use_dma) {
              stream_args.args["block_id" + channel_str]   = fifo_id[0];
              stream_args.args["block_port" + channel_str] = channel_str;
              stream_args.channels[channel_count]          = channel_count;
            } else {
              stream_args.args["block_id" + channel_str]   = fifo_id[channel_count];
              stream_args.args["block_port" + channel_str] = "0";
              stream_args.channels[channel_count]          = channel_count;
            }
          }
        }

        Debug("Getting Tx Stream with arguments " << stream_args.args.to_pp_string());
        tx_stream     = device3->get_tx_stream(stream_args);
        max_num_samps = tx_stream->get_max_num_samps();)
  }
  uhd_error set_tx_gain(size_t ch, double gain) override
  {
    if (ch >= nof_channels * nof_radios) {
      last_error = "Invalid channel index " + std::to_string(ch);
      return UHD_ERROR_INDEX;
    }

    if (radio_ctrl.size() == 0) {
      return UHD_ERROR_NONE;
    }

    size_t radio_idx   = ch / nof_channels;
    size_t channel_idx = ch % nof_channels;

    // Set the gain for the channel zero only
    if (channel_idx != 0) {
      last_error = "None";
      return UHD_ERROR_NONE;
    }

    UHD_SAFE_C_SAVE_ERROR(
        this, UHD_LOG_DEBUG(radio_id[radio_idx], "Setting TX Gain: " << gain << " dB...");
        radio_ctrl[radio_idx]->set_tx_gain(gain, 0);
        UHD_LOG_DEBUG(radio_id[radio_idx], "Actual TX Gain: " << radio_ctrl[radio_idx]->get_rx_gain(0) << " dB...");)
  }

  uhd_error set_rx_gain(size_t ch, double gain) override
  {
    if (ch >= nof_channels * nof_radios) {
      last_error = "Invalid channel index " + std::to_string(ch);
      return UHD_ERROR_INDEX;
    }

    if (radio_ctrl.size() == 0) {
      return UHD_ERROR_NONE;
    }

    size_t radio_idx   = ch / nof_channels;
    size_t channel_idx = ch % nof_channels;

    // Set the gain for the channel zero only
    if (channel_idx != 0) {
      last_error = "None";
      return UHD_ERROR_NONE;
    }

    UHD_SAFE_C_SAVE_ERROR(
        this, UHD_LOG_DEBUG(radio_id[radio_idx], "Setting RX Gain: " << gain << " dB...");
        radio_ctrl[radio_idx]->set_rx_gain(gain, 0);
        UHD_LOG_DEBUG(radio_id[radio_idx], "Actual RX Gain: " << radio_ctrl[radio_idx]->get_rx_gain(0) << " dB...");)
  }
  uhd_error get_tx_gain(double& gain) override
  {
    if (radio_ctrl.size() == 0) {
      return UHD_ERROR_NONE;
    }

    UHD_SAFE_C_SAVE_ERROR(this, gain = radio_ctrl[0]->get_tx_gain(0);)
  }
  uhd_error get_rx_gain(double& gain) override
  {

    if (radio_ctrl.size() == 0) {
      return UHD_ERROR_NONE;
    }

    UHD_SAFE_C_SAVE_ERROR(this, gain = radio_ctrl[0]->get_rx_gain(0);)
  }
  uhd_error set_tx_freq(uint32_t ch, double target_freq, double& actual_freq) override
  {
    if (ch >= tx_freq_hz.size()) {
      last_error = "Invalid channel index " + std::to_string(ch);
      return UHD_ERROR_INDEX;
    }

    if (not std::isnormal(target_freq)) {
      last_error = "Invalid TX frequency value " + std::to_string(target_freq) + " for channel " + std::to_string(ch);
      return UHD_ERROR_VALUE;
    }

    // Nothing to update
    if (std::round(tx_freq_hz[ch]) == std::round(target_freq)) {
      last_error = "None";
      return UHD_ERROR_NONE;
    }

    // Update frequency
    tx_freq_hz[ch] = target_freq;
    actual_freq    = tx_freq_hz[ch];

    Debug("Tuning Tx " << ch << " to " << actual_freq / 1e6 << " MHz...");
    size_t i = ch / nof_channels;
    size_t j = ch % nof_channels;

    UHD_SAFE_C_SAVE_ERROR(
        this,

        // Set Radio Tx freq
        if (not std::isnormal(tx_center_freq_hz[i]) and not loopback) {
          UHD_LOG_DEBUG(radio_id[i],
                        "Setting TX Freq: " << target_freq / 1e6 << " (" << uint64_t(target_freq) << ") MHz...");
          radio_ctrl[i]->set_tx_frequency(target_freq, 0);
          tx_center_freq_hz[i] = radio_ctrl[i]->get_tx_frequency(0);
          UHD_LOG_DEBUG(radio_id[i],
                        "Actual TX Freq: " << tx_center_freq_hz[i] / 1e6 << "(" << uint64_t(tx_center_freq_hz[i])
                                           << ") MHz...");
        }

        // Setup DUC
        double freq_hz = tx_freq_hz[nof_channels * i + j] - tx_center_freq_hz[i];
        if (std::round(duc_ctrl[i]->get_arg<double>("freq") != freq_hz)) {
          UHD_LOG_DEBUG(duc_id[i],
                        "Setting " << j << " freq: " << freq_hz / 1e6 << "(" << uint64_t(freq_hz) << ") MHz ...");
          duc_ctrl[i]->set_arg("freq", std::to_string(freq_hz), j);
          freq_hz = duc_ctrl[i]->get_arg<double>("freq");
          Debug("Actual Tx " << ch << " freq error: " << tx_center_freq_hz[i] + freq_hz - target_freq << " Hz...");
        })
  }
  uhd_error set_rx_freq(uint32_t ch, double target_freq, double& actual_freq) override
  {
    if (ch >= rx_freq_hz.size()) {
      last_error = "Invalid channel index " + std::to_string(ch);
      return UHD_ERROR_INDEX;
    }

    if (not std::isnormal(target_freq)) {
      last_error = "Invalid TX frequency value " + std::to_string(target_freq) + " for channel " + std::to_string(ch);
      return UHD_ERROR_VALUE;
    }

    // Nothing to update
    if (std::round(rx_freq_hz[ch]) == std::round(target_freq)) {
      last_error = "None";
      return UHD_ERROR_NONE;
    }

    // Update frequency
    rx_freq_hz[ch] = target_freq;
    actual_freq    = rx_freq_hz[ch];

    Debug("Tuning Rx " << ch << " to " << actual_freq / 1e6 << " MHz...");
    size_t i = ch / nof_channels;
    size_t j = ch % nof_channels;

    UHD_SAFE_C_SAVE_ERROR(
        this,

        // Set Radio Tx freq
        if (not std::isnormal(rx_center_freq_hz[i]) and not loopback) {
          UHD_LOG_DEBUG(radio_id[i], "Setting RX Freq: " << target_freq / 1e6 << " MHz...");
          radio_ctrl[i]->set_rx_frequency(target_freq, 0);
          rx_center_freq_hz[i] = radio_ctrl[i]->get_rx_frequency(0);
          UHD_LOG_DEBUG(radio_id[i], "Actual RX Freq: " << rx_center_freq_hz[i] / 1e6 << " MHz...");
        }

        // Setup DDC
        double freq_hz = rx_center_freq_hz[i] - rx_freq_hz[nof_channels * i + j];
        if (std::round(ddc_ctrl[i]->get_arg<double>("freq") != freq_hz)) {
          UHD_LOG_DEBUG(ddc_id[i], "Setting " << j << " freq: " << freq_hz / 1e6 << " MHz ...");
          ddc_ctrl[i]->set_arg("freq", freq_hz, j);
          freq_hz = ddc_ctrl[i]->get_arg<double>("freq");
          Debug("Actual Rx " << ch << " freq error: " << rx_center_freq_hz[i] - freq_hz - target_freq << " Hz...");
        })
  }
};

#endif // SRSLTE_RF_UHD_RFNOC_H
