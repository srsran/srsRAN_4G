/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_RF_UHD_GENERIC_H
#define SRSRAN_RF_UHD_GENERIC_H

#include "rf_uhd_safe.h"

class rf_uhd_generic : public rf_uhd_safe_interface
{
private:
  uhd::usrp::multi_usrp::sptr     usrp                         = nullptr;
  const uhd::fs_path              TREE_DBOARD_RX_FRONTEND_NAME = "/mboards/0/dboards/A/rx_frontends/A/name";
  const std::chrono::milliseconds FE_RX_RESET_SLEEP_TIME_MS    = std::chrono::milliseconds(2000UL);
  uhd::stream_args_t              stream_args                  = {};
  double                          lo_freq_tx_hz                = 0.0;
  double                          lo_freq_rx_hz                = 0.0;
  double                          lo_freq_offset_hz            = 0.0;
  double                          forced_freq_tx_hz[4]         = {0.0};
  double                          forced_freq_rx_hz[4]         = {0.0};
  double                          dsp_freq_tx_hz[4]            = {0.0};
  double                          dsp_freq_rx_hz[4]            = {0.0};

  uhd_error usrp_make_internal(const uhd::device_addr_t& dev_addr) override
  {
    // Destroy any previous USRP instance
    usrp = nullptr;

    Debug("Making USRP object with args '" << dev_addr.to_string() << "'");

    SRSRAN_UHD_SAFE_C_LOG_ERROR(usrp = uhd::usrp::multi_usrp::make(dev_addr);)
  }

  uhd_error set_tx_subdev(const std::string& string)
  {
    Info("Setting tx_subdev_spec to '" << string << "'");
    SRSRAN_UHD_SAFE_C_LOG_ERROR(usrp->set_tx_subdev_spec(string);)
  }
  uhd_error set_rx_subdev(const std::string& string)
  {
    Info("Setting rx_subdev_spec to '" << string << "'");
    SRSRAN_UHD_SAFE_C_LOG_ERROR(usrp->set_rx_subdev_spec(string);)
  }

  uhd_error test_ad936x_device(uint32_t nof_channels)
  {
    uhd_error err = set_rx_rate(1.92e6);
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    size_t max_samp = 0;
    err             = get_rx_stream(max_samp);
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    // Allocate buffers
    std::vector<float> data(max_samp * 2);
    std::vector<void*> buf(nof_channels);
    for (auto& b : buf) {
      b = data.data();
    }

    uhd::rx_metadata_t md              = {};
    size_t             nof_rxd_samples = 0;

    // If no error getting RX stream, try to receive once
    err = start_rx_stream(0.1);
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    // Flush Stream
    do {
      err = receive(buf.data(), max_samp, md, 0.0f, false, nof_rxd_samples);
      if (err != UHD_ERROR_NONE) {
        return err;
      }
    } while (md.error_code != uhd::rx_metadata_t::ERROR_CODE_TIMEOUT);

    // Receive
    err = receive(buf.data(), max_samp, md, 2.0f, false, nof_rxd_samples);

    if (err != UHD_ERROR_NONE) {
      return err;
    }

    if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT) {
      Error(md.strerror());
      return UHD_ERROR_IO;
    }

    // Stop stream
    err = stop_rx_stream();
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    // Flush Stream
    do {
      err = receive(buf.data(), max_samp, md, 0.0f, false, nof_rxd_samples);
      if (err != UHD_ERROR_NONE) {
        return err;
      }
    } while (md.error_code != uhd::rx_metadata_t::ERROR_CODE_TIMEOUT);

    return err;
  }

public:
  rf_uhd_generic() { Info("RF UHD Generic instance constructed"); }
  virtual ~rf_uhd_generic()
  {
    rx_stream = nullptr;
    tx_stream = nullptr;
    usrp      = nullptr;
    Debug("RF UHD closed Ok");
  }
  uhd_error usrp_make(const uhd::device_addr_t& dev_addr_, uint32_t nof_channels) override
  {
    uhd::device_addr_t dev_addr = dev_addr_;

    // Set transmitter subdevice spec string
    std::string tx_subdev;
    if (dev_addr.has_key("tx_subdev_spec")) {
      tx_subdev = dev_addr.pop("tx_subdev_spec");
    }

    // Set receiver subdevice spec string
    std::string rx_subdev;
    if (dev_addr.has_key("rx_subdev_spec")) {
      rx_subdev = dev_addr.pop("rx_subdev_spec");
    }

    // Set over the wire format
    std::string otw_format = "sc16";
    if (dev_addr.has_key("otw_format")) {
      otw_format = dev_addr.pop("otw_format");
    }

    // Samples-Per-Packet option, 0 means automatic
    std::string spp;
    if (dev_addr.has_key("spp")) {
      spp = dev_addr.pop("spp");
    }

    // Tx LO frequency
    if (dev_addr.has_key("lo_freq_tx_hz")) {
      lo_freq_tx_hz = dev_addr.cast("lo_freq_tx_hz", lo_freq_tx_hz);
      dev_addr.pop("lo_freq_tx_hz");
    }

    // Rx LO frequency
    if (dev_addr.has_key("lo_freq_rx_hz")) {
      lo_freq_rx_hz = dev_addr.cast("lo_freq_rx_hz", lo_freq_rx_hz);
      dev_addr.pop("lo_freq_rx_hz");
    }

    // LO Frequency offset automatic
    if (dev_addr.has_key("lo_freq_offset_hz")) {
      lo_freq_offset_hz = dev_addr.cast("lo_freq_offset_hz", lo_freq_offset_hz);
      dev_addr.pop("lo_freq_offset_hz");

      if (std::isnormal(lo_freq_tx_hz)) {
        Warning("'lo_freq_offset_hz' overrides 'lo_freq_tx_hz' (" << lo_freq_tx_hz / 1e6 << " MHz)");
      }

      if (std::isnormal(lo_freq_rx_hz)) {
        Warning("'lo_freq_offset_hz' overrides 'lo_freq_rx_hz' (" << lo_freq_rx_hz / 1e6 << " MHz)");
      }
    }

    // Tx CH0 overwrite frequency
    if (dev_addr.has_key("force_freq_tx0_hz")) {
      forced_freq_tx_hz[0] = dev_addr.cast("force_freq_tx0_hz", forced_freq_tx_hz[0]);
      dev_addr.pop("force_freq_tx0_hz");
      if (std::isnormal(forced_freq_tx_hz[0])) {
        Warning("'force_freq_tx0_hz' used to overwrite TX for frequency channel 0: " << forced_freq_tx_hz[0] / 1e6
                                                                                     << " MHz");
      }
    }
    // Tx CH1 overwrite frequency
    if (dev_addr.has_key("force_freq_tx1_hz")) {
      forced_freq_tx_hz[1] = dev_addr.cast("force_freq_tx1_hz", forced_freq_tx_hz[1]);
      dev_addr.pop("force_freq_tx1_hz");
      if (std::isnormal(forced_freq_tx_hz[1])) {
        Warning("'force_freq_tx1_hz' used to overwrite TX for frequency channel 1: " << forced_freq_tx_hz[1] / 1e6
                                                                                     << " MHz");
      }
    }
    // Rx CH0 overwrite frequency
    if (dev_addr.has_key("force_freq_rx0_hz")) {
      forced_freq_rx_hz[0] = dev_addr.cast("force_freq_rx0_hz", forced_freq_rx_hz[0]);
      dev_addr.pop("force_freq_rx0_hz");
      if (std::isnormal(forced_freq_rx_hz[0])) {
        Warning("'force_freq_rx0_hz' used to overwrite RX for frequency channel 0: " << forced_freq_rx_hz[0] / 1e6
                                                                                     << " MHz");
      }
    }
    // Rx CH1 overwrite frequency
    if (dev_addr.has_key("force_freq_rx1_hz")) {
      forced_freq_rx_hz[1] = dev_addr.cast("force_freq_rx1_hz", forced_freq_rx_hz[1]);
      dev_addr.pop("force_freq_rx1_hz");
      if (std::isnormal(forced_freq_rx_hz[1])) {
        Warning("'force_freq_rx1_hz' used to overwrite RX for frequency channel 1: " << forced_freq_rx_hz[1] / 1e6
                                                                                     << " MHz");
      }
    }

    // Tx CH0 DSP frequency
    if (dev_addr.has_key("dsp_freq_tx0_hz")) {
      dsp_freq_tx_hz[0] = dev_addr.cast("dsp_freq_tx0_hz", dsp_freq_tx_hz[0]);
      dev_addr.pop("dsp_freq_tx0_hz");
      if (std::isnormal(dsp_freq_tx_hz[0])) {
        Warning("'dsp_freq_tx0_hz' used to set dsp_freq for TX channel 0: " << dsp_freq_tx_hz[0] / 1e6 << " MHz");
      }
    }
    // Tx CH1 DSP frequency
    if (dev_addr.has_key("dsp_freq_tx1_hz")) {
      dsp_freq_tx_hz[1] = dev_addr.cast("dsp_freq_tx1_hz", dsp_freq_tx_hz[1]);
      dev_addr.pop("dsp_freq_tx1_hz");
      if (std::isnormal(dsp_freq_tx_hz[1])) {
        Warning("'dsp_freq_tx1_hz' used to set dsp_freq for TX channel 1: " << dsp_freq_tx_hz[1] / 1e6 << " MHz");
      }
    }
    // Rx CH0 DSP frequency
    if (dev_addr.has_key("dsp_freq_rx0_hz")) {
      dsp_freq_rx_hz[0] = dev_addr.cast("dsp_freq_rx0_hz", dsp_freq_rx_hz[0]);
      dev_addr.pop("dsp_freq_rx0_hz");
      if (std::isnormal(dsp_freq_rx_hz[0])) {
        Warning("'dsp_freq_rx0_hz' used to set dsp_freq for RX channel 0: " << dsp_freq_rx_hz[0] / 1e6 << " MHz");
      }
    }
    // Rx CH1 DSP frequency
    if (dev_addr.has_key("dsp_freq_rx1_hz")) {
      dsp_freq_rx_hz[1] = dev_addr.cast("dsp_freq_rx1_hz", dsp_freq_rx_hz[1]);
      dev_addr.pop("dsp_freq_rx1_hz");
      if (std::isnormal(dsp_freq_rx_hz[1])) {
        Warning("'dsp_freq_rx1_hz' used to set dsp_freq for RX channel 1: " << dsp_freq_rx_hz[1] / 1e6 << " MHz");
      }
    }

    // Make USRP
    uhd_error err = usrp_multi_make(dev_addr);
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    // Set transmitter subdev spec if specified
    if (not tx_subdev.empty()) {
      err = set_tx_subdev(tx_subdev);
      if (err != UHD_ERROR_NONE) {
        return err;
      }
    }

    // Set receiver subdev spec if specified
    if (not rx_subdev.empty()) {
      err = set_rx_subdev(rx_subdev);
      if (err != UHD_ERROR_NONE) {
        return err;
      }
    }

    // Initialize TX/RX stream args
    stream_args.cpu_format = "fc32";
    stream_args.otw_format = otw_format;
    if (not spp.empty()) {
      if (spp == "0") {
        Warning(
            "The parameter spp is 0, some UHD versions do not handle it as default and receive method will overflow.");
      }
      stream_args.args.set("spp", spp);
    }
    stream_args.channels.resize(nof_channels);
    for (size_t i = 0; i < (size_t)nof_channels; i++) {
      stream_args.channels[i] = i;
    }

    // Try to get dboard name from property tree
    uhd::property_tree::sptr tree = usrp->get_device()->get_tree();
    if (tree == nullptr || not tree->exists(TREE_DBOARD_RX_FRONTEND_NAME)) {
      // Couldn't find dboard name in property tree
      return err;
    }

    std::string dboard_name = usrp->get_device()->get_tree()->access<std::string>(TREE_DBOARD_RX_FRONTEND_NAME).get();

    // Detect if it a AD9361 based device
    if (dboard_name.find("FE-RX") != std::string::npos and false) {
      Info("The device is based on AD9361, get RX stream for checking LIBUSB_TRANSFER_ERROR");
      uint32_t ntrials = 10;
      do {
        // If no error getting RX stream, return
        err = test_ad936x_device(nof_channels);
        if (err == UHD_ERROR_NONE) {
          return err;
        }

        // Otherwise, close USRP and open again
        usrp = nullptr;

        Warning("Failed to open Rx stream, trying to open device again. "
                << ntrials << " trials left. Waiting for " << FE_RX_RESET_SLEEP_TIME_MS.count() << " ms");

        // Sleep
        std::this_thread::sleep_for(FE_RX_RESET_SLEEP_TIME_MS);

        // Try once more making the device
        err = usrp_multi_make(dev_addr);

      } while (err == UHD_ERROR_NONE and --ntrials != 0);
    }

    return err;
  }

  uhd_error get_mboard_name(std::string& mboard_name) override
  {
    SRSRAN_UHD_SAFE_C_LOG_ERROR(mboard_name = usrp->get_mboard_name();)
  }
  uhd_error get_mboard_sensor_names(std::vector<std::string>& sensors) override
  {
    SRSRAN_UHD_SAFE_C_LOG_ERROR(sensors = usrp->get_mboard_sensor_names();)
  }
  uhd_error get_rx_sensor_names(std::vector<std::string>& sensors) override
  {
    SRSRAN_UHD_SAFE_C_LOG_ERROR(sensors = usrp->get_rx_sensor_names();)
  }
  uhd_error get_sensor(const std::string& sensor_name, double& sensor_value) override
  {
    SRSRAN_UHD_SAFE_C_LOG_ERROR(sensor_value = usrp->get_mboard_sensor(sensor_name).to_real();)
  }
  uhd_error get_sensor(const std::string& sensor_name, bool& sensor_value) override
  {
    SRSRAN_UHD_SAFE_C_LOG_ERROR(sensor_value = usrp->get_mboard_sensor(sensor_name).to_bool();)
  }
  uhd_error get_rx_sensor(const std::string& sensor_name, bool& sensor_value) override
  {
    SRSRAN_UHD_SAFE_C_LOG_ERROR(sensor_value = usrp->get_rx_sensor(sensor_name).to_bool();)
  }
  uhd_error set_time_unknown_pps(const uhd::time_spec_t& timespec) override
  {
    Debug("Setting Time at next PPS...");
    SRSRAN_UHD_SAFE_C_LOG_ERROR(usrp->set_time_unknown_pps(timespec);)
  }
  uhd_error get_time_now(uhd::time_spec_t& timespec) override
  {
    SRSRAN_UHD_SAFE_C_LOG_ERROR(timespec = usrp->get_time_now();)
  }
  uhd_error set_sync_source(const std::string& sync_source, const std::string& clock_source) override
  {
    Debug("Setting PPS source to '" << sync_source << "' and clock source to '" << clock_source << "'");
#if UHD_VERSION < 3140099
    SRSRAN_UHD_SAFE_C_LOG_ERROR(usrp->set_clock_source(clock_source); usrp->set_time_source(sync_source);)
#else
    SRSRAN_UHD_SAFE_C_LOG_ERROR(usrp->set_sync_source(clock_source, sync_source);)
#endif
  }
  uhd_error get_gain_range(uhd::gain_range_t& tx_gain_range, uhd::gain_range_t& rx_gain_range) override
  {
    SRSRAN_UHD_SAFE_C_LOG_ERROR(tx_gain_range = usrp->get_tx_gain_range(); rx_gain_range = usrp->get_rx_gain_range();)
  }
  uhd_error set_master_clock_rate(double rate) override
  {
    Debug("Setting master clock rate to " << rate / 1e6 << " MHz");
    SRSRAN_UHD_SAFE_C_LOG_ERROR(usrp->set_master_clock_rate(rate);)
  }
  uhd_error set_rx_rate(double rate) override
  {
    Debug("Setting Rx Rate to " << rate / 1e6 << "MHz");
    SRSRAN_UHD_SAFE_C_LOG_ERROR(usrp->set_rx_rate(rate);)
  }
  uhd_error set_tx_rate(double rate) override
  {
    Debug("Setting Tx Rate to " << rate / 1e6 << "MHz");
    SRSRAN_UHD_SAFE_C_LOG_ERROR(usrp->set_tx_rate(rate);)
  }
  uhd_error set_command_time(const uhd::time_spec_t& timespec) override
  {
    SRSRAN_UHD_SAFE_C_LOG_ERROR(usrp->set_command_time(timespec);)
  }
  uhd_error get_rx_stream(size_t& max_num_samps) override
  {
    Debug("Creating Rx stream");
    SRSRAN_UHD_SAFE_C_LOG_ERROR(rx_stream = nullptr; rx_stream = usrp->get_rx_stream(stream_args);
                                max_num_samps                  = rx_stream->get_max_num_samps();
                                if (max_num_samps == 0UL) {
                                  Error("The maximum number of receive samples is zero.");
                                  return UHD_ERROR_VALUE;
                                })
  }
  uhd_error get_tx_stream(size_t& max_num_samps) override
  {
    Debug("Creating Tx stream");
    SRSRAN_UHD_SAFE_C_LOG_ERROR(tx_stream = nullptr; tx_stream = usrp->get_tx_stream(stream_args);
                                max_num_samps                  = tx_stream->get_max_num_samps();
                                if (max_num_samps == 0UL) {
                                  Error("The maximum number of transmit samples is zero.");
                                  return UHD_ERROR_VALUE;
                                })
  }
  uhd_error set_tx_gain(size_t ch, double gain) override
  {
    Debug("Setting channel " << ch << " Tx gain to " << gain << " dB");
    SRSRAN_UHD_SAFE_C_LOG_ERROR(usrp->set_tx_gain(gain, ch);)
  }
  uhd_error set_rx_gain(size_t ch, double gain) override
  {
    Debug("Setting channel " << ch << " Rx gain to " << gain << " dB");
    SRSRAN_UHD_SAFE_C_LOG_ERROR(usrp->set_rx_gain(gain, ch);)
  }
  uhd_error get_rx_gain(double& gain) override { SRSRAN_UHD_SAFE_C_LOG_ERROR(gain = usrp->get_rx_gain();) }
  uhd_error get_tx_gain(double& gain) override { SRSRAN_UHD_SAFE_C_LOG_ERROR(gain = usrp->get_tx_gain();) }
  uhd_error set_tx_freq(uint32_t ch, double target_freq, double& actual_freq) override
  {
    Debug("Setting channel " << ch << " Tx frequency to " << target_freq / 1e6 << " MHz");

    // Create Tune request
    uhd::tune_request_t tune_request(target_freq);

    if (std::isnormal(lo_freq_offset_hz)) {
      lo_freq_tx_hz = target_freq + lo_freq_offset_hz;
    }

    // If the LO frequency is defined, force a LO frequency and use the
    if (std::isnormal(lo_freq_tx_hz)) {
      tune_request.rf_freq         = lo_freq_tx_hz;
      tune_request.rf_freq_policy  = uhd::tune_request_t::POLICY_MANUAL;
      tune_request.dsp_freq_policy = uhd::tune_request_t::POLICY_AUTO;
    }

    if (ch < 4 and std::isnormal(forced_freq_tx_hz[ch])) {
      Warning("Overwriting channel " << ch << " Tx frequency: " << target_freq / 1e6 << " MHz -> "
                                     << forced_freq_tx_hz[ch] / 1e6 << " MHz");
      target_freq = forced_freq_tx_hz[ch];
    }

    if (ch < 4 and std::isnormal(dsp_freq_tx_hz[ch])) {
      // target_freq = rf_freq - dsp_freq
      tune_request.target_freq     = 0;
      tune_request.rf_freq         = target_freq;
      tune_request.rf_freq_policy  = uhd::tune_request_t::POLICY_MANUAL;
      tune_request.dsp_freq        = dsp_freq_tx_hz[ch];
      tune_request.dsp_freq_policy = uhd::tune_request_t::POLICY_MANUAL;
      Warning("Setting channel " << ch << " Tx rf_freq=" << target_freq / 1e6
                                 << " dsp_freq=" << dsp_freq_tx_hz[ch] / 1e6
                                 << " target_freq=" << (tune_request.rf_freq + tune_request.dsp_freq) / 1e6 << " MHz");
    }

    SRSRAN_UHD_SAFE_C_LOG_ERROR(uhd::tune_result_t tune_result = usrp->set_tx_freq(tune_request, ch);
                                actual_freq                    = tune_result.target_rf_freq;)
  }
  uhd_error set_rx_freq(uint32_t ch, double target_freq, double& actual_freq) override
  {
    Debug("Setting channel " << ch << " Rx frequency to " << target_freq / 1e6 << " MHz");

    // Create Tune request
    uhd::tune_request_t tune_request(target_freq);

    if (std::isnormal(lo_freq_offset_hz)) {
      lo_freq_rx_hz = target_freq + lo_freq_offset_hz;
    }

    // If the LO frequency is defined, force a LO frequency and use the
    if (std::isnormal(lo_freq_rx_hz)) {
      tune_request.rf_freq         = lo_freq_rx_hz;
      tune_request.rf_freq_policy  = uhd::tune_request_t::POLICY_MANUAL;
      tune_request.dsp_freq_policy = uhd::tune_request_t::POLICY_AUTO;
    }

    if (ch < 4 and std::isnormal(forced_freq_rx_hz[ch])) {
      Warning("Overwriting channel " << ch << " Rx frequency: " << target_freq / 1e6 << " MHz -> "
                                     << forced_freq_rx_hz[ch] / 1e6 << " MHz");
      target_freq = forced_freq_rx_hz[ch];
    }

    if (ch < 4 and std::isnormal(dsp_freq_rx_hz[ch])) {
      // target_freq = rf_freq + dsp_freq
      tune_request.target_freq     = 0;
      tune_request.rf_freq         = target_freq;
      tune_request.rf_freq_policy  = uhd::tune_request_t::POLICY_MANUAL;
      tune_request.dsp_freq        = dsp_freq_rx_hz[ch];
      tune_request.dsp_freq_policy = uhd::tune_request_t::POLICY_MANUAL;
      Warning("Setting channel " << ch << " Rx rf_freq=" << target_freq / 1e6
                                 << " dsp_freq=" << dsp_freq_rx_hz[ch] / 1e6
                                 << " target_freq=" << (tune_request.rf_freq - tune_request.dsp_freq) / 1e6 << " MHz");
    }

    SRSRAN_UHD_SAFE_C_LOG_ERROR(uhd::tune_result_t tune_result = usrp->set_rx_freq(tune_request, ch);
                                actual_freq                    = tune_result.target_rf_freq;)
  }
};

#endif // SRSRAN_RF_UHD_GENERIC_H
