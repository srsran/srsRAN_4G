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

#include "srsran/radio/radio.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"
#include "srsran/config.h"
#include "srsran/support/srsran_assert.h"
#include <list>
#include <string>
#include <unistd.h>

namespace srsran {

radio::radio()
{
  zeros.resize(SRSRAN_SF_LEN_MAX, 0);
  for (uint32_t i = 0; i < SRSRAN_MAX_CHANNELS; i++) {
    dummy_buffers[i].resize(SRSRAN_SF_LEN_MAX * SRSRAN_NOF_SF_X_FRAME, 0);
  }
}

radio::~radio()
{
  for (srsran_resampler_fft_t& q : interpolators) {
    srsran_resampler_fft_free(&q);
  }

  for (srsran_resampler_fft_t& q : decimators) {
    srsran_resampler_fft_free(&q);
  }
}

int radio::init(const rf_args_t& args, phy_interface_radio* phy_)
{
  phy = phy_;

  // Init log
  logger.set_level(srslog::str_to_basic_level(args.log_level));

  if (args.nof_antennas > SRSRAN_MAX_PORTS) {
    logger.error("Maximum number of antennas exceeded (%d > %d)", args.nof_antennas, SRSRAN_MAX_PORTS);
    return SRSRAN_ERROR;
  }

  if (args.nof_carriers > SRSRAN_MAX_CARRIERS) {
    logger.error("Maximum number of carriers exceeded (%d > %d)", args.nof_carriers, SRSRAN_MAX_CARRIERS);
    return SRSRAN_ERROR;
  }

  if (!config_rf_channels(args)) {
    srsran::console("Error configuring RF channels\n");
    return SRSRAN_ERROR;
  }

  nof_channels = args.nof_antennas * args.nof_carriers;
  nof_antennas = args.nof_antennas;
  nof_carriers = args.nof_carriers;
  fix_srate_hz = args.srate_hz;

  cur_tx_freqs.resize(nof_carriers);
  cur_rx_freqs.resize(nof_carriers);

  // Split multiple RF channels using `;` delimiter
  std::vector<std::string> device_args_list;
  string_parse_list(args.device_args, ';', device_args_list);

  // Add auto if list is empty
  if (device_args_list.empty()) {
    device_args_list.emplace_back("auto");
  }

  // Avoid opening more RF devices than necessary
  if (nof_channels < device_args_list.size()) {
    device_args_list.resize(nof_channels);
  }

  // Makes sure it is possible to have the same number of RF channels in each RF device
  if (nof_channels % device_args_list.size() != 0) {
    srsran::console(
        "Error: The number of required RF channels (%d) is not divisible between the number of RF devices (%zd).\n",
        nof_channels,
        device_args_list.size());
    return SRSRAN_ERROR;
  }
  nof_channels_x_dev = nof_channels / device_args_list.size();

  // Allocate RF devices
  rf_devices.resize(device_args_list.size());
  rf_info.resize(device_args_list.size());
  rx_offset_n.resize(device_args_list.size());

  tx_channel_mapping.set_config(nof_channels_x_dev, nof_antennas);
  rx_channel_mapping.set_config(nof_channels_x_dev, nof_antennas);

  // Init and start Radios
  if (args.device_name != "file" || device_args_list[0] != "auto") {
    // regular RF device
    for (uint32_t device_idx = 0; device_idx < (uint32_t)device_args_list.size(); device_idx++) {
      if (not open_dev(device_idx, args.device_name, device_args_list[device_idx])) {
        logger.error("Error opening RF device %d", device_idx);
        return SRSRAN_ERROR;
      }
    }
  } else {
    // file-based RF device abstraction using pre-opened FILE* objects
    if (args.rx_files == nullptr && args.tx_files == nullptr) {
      logger.error("File-based RF device abstraction requested, but no files provided");
      return SRSRAN_ERROR;
    }
    for (uint32_t device_idx = 0; device_idx < (uint32_t)device_args_list.size(); device_idx++) {
      if (not open_dev(device_idx,
                       &args.rx_files[device_idx * nof_channels_x_dev],
                       &args.tx_files[device_idx * nof_channels_x_dev],
                       nof_channels_x_dev,
                       args.srate_hz)) {
        logger.error("Error opening RF device %d", device_idx);
        return SRSRAN_ERROR;
      }
    }
  }

  is_start_of_burst = true;
  is_initialized    = true;

  // Set RF options
  tx_adv_auto = true;
  if (args.time_adv_nsamples != "auto") {
    int t = (int)strtol(args.time_adv_nsamples.c_str(), nullptr, 10);
    set_tx_adv(abs(t));
    set_tx_adv_neg(t < 0);
  }
  continuous_tx = true;
  if (args.continuous_tx != "auto") {
    continuous_tx = (args.continuous_tx == "yes");
  }

  // Set fixed gain options
  if (args.rx_gain < 0) {
    start_agc(false);
  } else {
    set_rx_gain(args.rx_gain);
  }
  // Set gain for all channels
  if (args.tx_gain > 0) {
    set_tx_gain(args.tx_gain);
  } else {
    // Set same gain than for RX until power control sets a gain
    set_tx_gain(args.rx_gain);
    srsran::console("\nWarning: TX gain was not set. Using open-loop power control (not working properly)\n\n");
  }

  // Set individual gains
  for (uint32_t i = 0; i < args.nof_carriers; i++) {
    if (args.tx_gain_ch[i] > 0) {
      for (uint32_t j = 0; j < nof_antennas; j++) {
        uint32_t phys_antenna_idx = i * nof_antennas + j;

        // From channel number deduce RF device index and channel
        uint32_t rf_device_idx  = phys_antenna_idx / nof_channels_x_dev;
        uint32_t rf_channel_idx = phys_antenna_idx % nof_channels_x_dev;

        logger.info(
            "Setting individual tx_gain=%.1f on dev=%d ch=%d", args.tx_gain_ch[i], rf_device_idx, rf_channel_idx);
        if (srsran_rf_set_tx_gain_ch(&rf_devices[rf_device_idx], rf_channel_idx, args.tx_gain_ch[i]) < 0) {
          logger.error(
              "Setting channel tx_gain=%.1f on dev=%d ch=%d", args.tx_gain_ch[i], rf_device_idx, rf_channel_idx);
        }
      }
    }
  }

  // Set individual gains
  for (uint32_t i = 0; i < args.nof_carriers; i++) {
    if (args.rx_gain_ch[i] > 0) {
      for (uint32_t j = 0; j < nof_antennas; j++) {
        uint32_t phys_antenna_idx = i * nof_antennas + j;

        // From channel number deduce RF device index and channel
        uint32_t rf_device_idx  = phys_antenna_idx / nof_channels_x_dev;
        uint32_t rf_channel_idx = phys_antenna_idx % nof_channels_x_dev;

        logger.info(
            "Setting individual rx_gain=%.1f on dev=%d ch=%d", args.rx_gain_ch[i], rf_device_idx, rf_channel_idx);
        if (srsran_rf_set_rx_gain_ch(&rf_devices[rf_device_idx], rf_channel_idx, args.rx_gain_ch[i]) < 0) {
          logger.error(
              "Setting channel rx_gain=%.1f on dev=%d ch=%d", args.rx_gain_ch[i], rf_device_idx, rf_channel_idx);
        }
      }
    }
  }

  // It is not expected that any application tries to receive more than max_resamp_buf_sz_ms
  if (std::isnormal(fix_srate_hz)) {
    size_t resamp_buf_sz = (max_resamp_buf_sz_ms * fix_srate_hz) / 1000;
    for (auto& buf : rx_buffer) {
      buf.resize(resamp_buf_sz);
    }
    for (auto& buf : tx_buffer) {
      buf.resize(resamp_buf_sz);
    }
  }

  // Frequency offset
  freq_offset = args.freq_offset;

  return SRSRAN_SUCCESS;
}

bool radio::is_init()
{
  return is_initialized;
}

void radio::stop()
{
  // Stop Rx streams as soon as possible to avoid Overflows
  if (radio_is_streaming) {
    for (srsran_rf_t& rf_device : rf_devices) {
      srsran_rf_stop_rx_stream(&rf_device);
    }
  }
  if (is_initialized) {
    for (srsran_rf_t& rf_device : rf_devices) {
      srsran_rf_close(&rf_device);
    }
  }
}

void radio::reset()
{
  for (srsran_rf_t& rf_device : rf_devices) {
    srsran_rf_stop_rx_stream(&rf_device);
  }
  radio_is_streaming = false;
  usleep(100000);
}

bool radio::is_continuous_tx()
{
  return continuous_tx;
}

void radio::set_tx_adv(int nsamples)
{
  tx_adv_auto     = false;
  tx_adv_nsamples = nsamples;
  if (!nsamples) {
    tx_adv_sec = 0;
  }
}

void radio::set_tx_adv_neg(bool tx_adv_is_neg)
{
  tx_adv_negative = tx_adv_is_neg;
}

bool radio::start_agc(bool tx_gain_same_rx)
{
  if (!is_initialized) {
    return false;
  }
  for (srsran_rf_t& rf_device : rf_devices) {
    if (srsran_rf_start_gain_thread(&rf_device, tx_gain_same_rx)) {
      ERROR("Error starting AGC Thread RF device");
      return false;
    }
  }
  return true;
}

bool radio::rx_now(rf_buffer_interface& buffer, rf_timestamp_interface& rxd_time)
{
  std::unique_lock<std::mutex> lock(rx_mutex);
  bool                         ret = true;
  rf_buffer_t                  buffer_rx;

  // Extract decimation ratio. As the decimation may take some time to set a new ratio, deactivate the decimation and
  // keep receiving samples to avoid stalling the RX stream
  uint32_t ratio = 1; // No decimation by default
  if (decimator_busy) {
    lock.unlock();
  } else if (decimators[0].ratio > 1) {
    ratio = decimators[0].ratio;
  }

  // Calculate number of samples, considering the decimation ratio
  uint32_t nof_samples = buffer.get_nof_samples() * ratio;

  // Check decimation buffer protection
  if (ratio > 1 && nof_samples > rx_buffer[0].size()) {
    // This is a corner case that could happen during sample rate change transitions, as it does not have a negative
    // impact, log it as info.
    fmt::memory_buffer buff;
    fmt::format_to(buff,
                   "Rx number of samples ({}/{}) exceeds buffer size ({})",
                   buffer.get_nof_samples(),
                   buffer.get_nof_samples() * ratio,
                   rx_buffer[0].size());
    logger.info("%s", to_c_str(buff));

    // Limit number of samples to receive
    nof_samples = rx_buffer[0].size();
  }

  // Set new buffer size
  buffer_rx.set_nof_samples(nof_samples);

  // If the interpolator have been set, interpolate
  for (uint32_t ch = 0; ch < nof_channels; ch++) {
    // Use rx buffer if decimator is required
    buffer_rx.set(ch, ratio > 1 ? rx_buffer[ch].data() : buffer.get(ch));
  }

  if (not radio_is_streaming) {
    for (srsran_rf_t& rf_device : rf_devices) {
      srsran_rf_start_rx_stream(&rf_device, false);
    }
    radio_is_streaming = true;

    // Flush buffers to compensate settling time
    if (rf_devices.size() > 1) {
      for (srsran_rf_t& rf_device : rf_devices) {
        srsran_rf_flush_buffer(&rf_device);
      }
    }
  }

  for (uint32_t device_idx = 0; device_idx < (uint32_t)rf_devices.size(); device_idx++) {
    ret &= rx_dev(device_idx, buffer_rx, rxd_time.get_ptr(device_idx));
  }

  // Perform decimation
  if (ratio > 1) {
    for (uint32_t ch = 0; ch < nof_channels; ch++) {
      if (buffer.get(ch) and buffer_rx.get(ch)) {
        srsran_resampler_fft_run(&decimators[ch], buffer_rx.get(ch), buffer.get(ch), buffer_rx.get_nof_samples());
      }
    }
  }

  return ret;
}

bool radio::rx_dev(const uint32_t& device_idx, const rf_buffer_interface& buffer, srsran_timestamp_t* rxd_time)
{
  if (!is_initialized) {
    return false;
  }

  time_t* full_secs = rxd_time ? &rxd_time->full_secs : nullptr;
  double* frac_secs = rxd_time ? &rxd_time->frac_secs : nullptr;

  void* radio_buffers[SRSRAN_MAX_CHANNELS] = {};

  // Discard channels not allocated, need to point to valid buffer
  for (uint32_t i = 0; i < SRSRAN_MAX_CHANNELS; i++) {
    radio_buffers[i] = dummy_buffers[i].data();
  }

  if (not map_channels(rx_channel_mapping, device_idx, 0, buffer, radio_buffers)) {
    logger.error("Mapping logical channels to physical channels for transmission");
    return false;
  }

  // Apply Rx offset into the number of samples and reset value
  int      nof_samples_offset = rx_offset_n.at(device_idx);
  uint32_t nof_samples        = buffer.get_nof_samples();

  // Number of samples adjust from device time offset
  if (nof_samples_offset < 0 and (uint32_t)(-nof_samples_offset) > nof_samples) {
    // Avoid overflow subtraction
    nof_samples = 0;
  } else {
    // Limit the number of samples to a maximum of 2 times the requested number of samples
    nof_samples = SRSRAN_MIN(nof_samples + nof_samples_offset, 2 * nof_samples);
  }

  // Subtract number of offset samples
  rx_offset_n.at(device_idx) = nof_samples_offset - ((int)nof_samples - (int)buffer.get_nof_samples());

  int ret =
      srsran_rf_recv_with_time_multi(&rf_devices[device_idx], radio_buffers, nof_samples, true, full_secs, frac_secs);

  // If the number of received samples filled the buffer, there is nothing else to do
  if (buffer.get_nof_samples() <= nof_samples) {
    return ret > 0;
  }

  // Otherwise, set rest of buffer to zero
  uint32_t nof_zeros = buffer.get_nof_samples() - nof_samples;
  for (auto& b : radio_buffers) {
    if (b != nullptr) {
      cf_t* ptr = (cf_t*)b;
      srsran_vec_cf_zero(&ptr[nof_samples], nof_zeros);
    }
  }

  return ret > 0;
}

bool radio::tx(rf_buffer_interface& buffer, const rf_timestamp_interface& tx_time)
{
  bool                         ret = true;
  std::unique_lock<std::mutex> lock(tx_mutex);
  uint32_t                     ratio = interpolators[0].ratio;

  // Get number of samples at the low rate
  uint32_t nof_samples = buffer.get_nof_samples();

  // Check that number of the interpolated samples does not exceed the buffer size
  if (ratio > 1 && (size_t)nof_samples * (size_t)ratio > tx_buffer[0].size()) {
    // This is a corner case that could happen during sample rate change transitions, as it does not have a negative
    // impact, log it as info.
    fmt::memory_buffer buff;
    fmt::format_to(buff,
                   "Tx number of samples ({}/{}) exceeds buffer size ({})\n",
                   buffer.get_nof_samples(),
                   buffer.get_nof_samples() * ratio,
                   tx_buffer[0].size());
    logger.info("%s", to_c_str(buff));

    // Limit number of samples to transmit
    nof_samples = tx_buffer[0].size() / ratio;
  }

  // If the interpolator have been set, interpolate
  if (interpolators[0].ratio > 1) {
    for (uint32_t ch = 0; ch < nof_channels; ch++) {
      // Perform actual interpolation
      srsran_resampler_fft_run(&interpolators[ch], buffer.get(ch), tx_buffer[ch].data(), nof_samples);

      // Set the buffer pointer
      buffer.set(ch, tx_buffer[ch].data());
    }

    // Set buffer size after applying the interpolation
    buffer.set_nof_samples(nof_samples * ratio);
  }

  for (uint32_t device_idx = 0; device_idx < (uint32_t)rf_devices.size(); device_idx++) {
    ret &= tx_dev(device_idx, buffer, tx_time.get(device_idx));
  }

  is_start_of_burst = false;

  return ret;
}

bool radio::open_dev(const uint32_t& device_idx, const std::string& device_name, const std::string& devive_args)
{
  srsran_rf_t* rf_device = &rf_devices[device_idx];

  char* dev_args = nullptr;
  if (devive_args != "auto") {
    dev_args = (char*)devive_args.c_str();
  }

  char* dev_name = nullptr;
  if (device_name != "auto") {
    dev_name = (char*)device_name.c_str();
  }

  srsran::console("Opening %d channels in RF device=%s with args=%s\n",
                  nof_channels_x_dev,
                  dev_name ? dev_name : "default",
                  dev_args ? dev_args : "default");

  if (srsran_rf_open_devname(rf_device, dev_name, dev_args, nof_channels_x_dev)) {
    logger.error("Error opening RF device");
    return false;
  }

  // Suppress radio stdout
  srsran_rf_suppress_stdout(rf_device);

  // Register handler for processing O/U/L
  srsran_rf_register_error_handler(rf_device, rf_msg_callback, this);

  // Get device info
  rf_info[device_idx] = *srsran_rf_get_info(rf_device);

  return true;
}

bool radio::open_dev(const uint32_t &device_idx, FILE** rx_files, FILE** tx_files, uint32_t nof_channels, uint32_t base_srate)
{
  srsran_rf_t* rf_device = &rf_devices[device_idx];

  srsran::console("Opening channels idx %d in RF device abstraction\n", device_idx);

  if (srsran_rf_open_file(rf_device, rx_files, tx_files, nof_channels, base_srate)) {
    logger.error("Error opening RF device abstraction");
    return false;
  }

  // Suppress radio stdout
  srsran_rf_suppress_stdout(rf_device);

  // Register handler for processing O/U/L
  srsran_rf_register_error_handler(rf_device, rf_msg_callback, this);

  // Get device info
  rf_info[device_idx] = *srsran_rf_get_info(rf_device);

  return true;
}

bool radio::tx_dev(const uint32_t& device_idx, rf_buffer_interface& buffer, const srsran_timestamp_t& tx_time_)
{
  uint32_t     nof_samples   = buffer.get_nof_samples();
  uint32_t     sample_offset = 0;
  srsran_rf_t* rf_device     = &rf_devices[device_idx];

  // Return instantly if the radio module is not initialised
  if (!is_initialized) {
    return false;
  }

  // Copy timestamp and add Tx time offset calibration
  srsran_timestamp_t tx_time = tx_time_;
  if (!tx_adv_negative) {
    srsran_timestamp_sub(&tx_time, 0, tx_adv_sec);
  } else {
    srsran_timestamp_add(&tx_time, 0, tx_adv_sec);
  }

  // Calculates transmission time overlap with previous transmission
  srsran_timestamp_t ts_overlap = end_of_burst_time[device_idx];
  srsran_timestamp_sub(&ts_overlap, tx_time.full_secs, tx_time.frac_secs);

  // Calculates number of overlap samples with previous transmission
  int32_t past_nsamples = (int32_t)round(cur_tx_srate * srsran_timestamp_real(&ts_overlap));

  // if past_nsamples is positive, the current transmission overlaps with the previous transmission. If it is negative
  // there is a gap between the previous transmission and the current transmission.
  if (past_nsamples > 0) {
    // If the overlap length is greater than the current transmission length, it means the whole transmission is in
    // the past and it shall be ignored
    if ((int32_t)nof_samples < past_nsamples) {
      return true;
    }

    // Trim the first past_nsamples
    sample_offset = (uint32_t)past_nsamples;       // Sets an offset for moving first samples offset
    tx_time       = end_of_burst_time[device_idx]; // Keeps same transmission time
    nof_samples   = nof_samples - past_nsamples;   // Subtracts the number of trimmed samples

    // Prints discarded samples
    logger.debug("Detected RF overlap of %.1f us. Discarding %d samples.",
                 srsran_timestamp_real(&ts_overlap) * 1.0e6,
                 past_nsamples);

  } else if (past_nsamples < 0 and not is_start_of_burst) {
    // if the gap is bigger than TX_MAX_GAP_ZEROS, stop burst
    if (fabs(srsran_timestamp_real(&ts_overlap)) > tx_max_gap_zeros) {
      logger.info("Detected RF gap of %.1f us. Sending end-of-burst.", srsran_timestamp_real(&ts_overlap) * 1.0e6);
      tx_end_nolock();
    } else {
      logger.debug("Detected RF gap of %.1f us. Tx'ing zeroes.", srsran_timestamp_real(&ts_overlap) * 1.0e6);
      // Otherwise, transmit zeros
      uint32_t gap_nsamples = abs(past_nsamples);
      while (gap_nsamples > 0) {
        // Transmission cannot exceed SRSRAN_SF_LEN_MAX (zeros buffer size limitation)
        uint32_t nzeros = SRSRAN_MIN(gap_nsamples, SRSRAN_SF_LEN_MAX);

        // Zeros transmission
        int ret = srsran_rf_send_timed2(rf_device,
                                        zeros.data(),
                                        nzeros,
                                        end_of_burst_time[device_idx].full_secs,
                                        end_of_burst_time[device_idx].frac_secs,
                                        false,
                                        false);
        if (ret < SRSRAN_SUCCESS) {
          return false;
        }

        // Substract gap samples
        gap_nsamples -= nzeros;

        // Increase timestamp
        srsran_timestamp_add(&end_of_burst_time[device_idx], 0, (double)nzeros / cur_tx_srate);
      }
    }
  }

  // Save possible end of burst time
  srsran_timestamp_copy(&end_of_burst_time[device_idx], &tx_time);
  srsran_timestamp_add(&end_of_burst_time[device_idx], 0, (double)nof_samples / cur_tx_srate);

  void* radio_buffers[SRSRAN_MAX_CHANNELS] = {};

  // Discard channels not allocated, need to point to valid buffer
  for (uint32_t i = 0; i < SRSRAN_MAX_CHANNELS; i++) {
    radio_buffers[i] = zeros.data();
  }

  if (not map_channels(tx_channel_mapping, device_idx, sample_offset, buffer, radio_buffers)) {
    logger.error("Mapping logical channels to physical channels for transmission");
    return false;
  }

  int ret = srsran_rf_send_timed_multi(
      rf_device, radio_buffers, nof_samples, tx_time.full_secs, tx_time.frac_secs, true, is_start_of_burst, false);

  return ret > SRSRAN_SUCCESS;
}

void radio::tx_end()
{
  std::unique_lock<std::mutex> lock(tx_mutex);
  tx_end_nolock();
}

void radio::tx_end_nolock()
{
  if (!is_initialized) {
    return;
  }
  if (!is_start_of_burst) {
    for (uint32_t i = 0; i < (uint32_t)rf_devices.size(); i++) {
      srsran_rf_send_timed2(
          &rf_devices[i], zeros.data(), 0, end_of_burst_time[i].full_secs, end_of_burst_time[i].frac_secs, false, true);
    }
    is_start_of_burst = true;
  }
}

bool radio::get_is_start_of_burst()
{
  return is_start_of_burst;
}

void radio::release_freq(const uint32_t& carrier_idx)
{
  rx_channel_mapping.release_freq(carrier_idx);
  tx_channel_mapping.release_freq(carrier_idx);
}

void radio::set_rx_freq(const uint32_t& carrier_idx, const double& freq)
{
  if (!is_initialized) {
    return;
  }

  // Map carrier index to physical channel
  if (rx_channel_mapping.allocate_freq(carrier_idx, freq)) {
    channel_mapping::device_mapping_t device_mapping = rx_channel_mapping.get_device_mapping(carrier_idx);
    if (device_mapping.channel_idx >= nof_channels_x_dev) {
      logger.error("Invalid mapping physical channel %d to logical carrier %d on f_rx=%.1f MHz (nof_channels_x_dev=%d, device_idx=%d)",
                   device_mapping.channel_idx,
                   carrier_idx,
                   freq / 1e6, nof_channels_x_dev, device_mapping.device_idx);
      return;
    }

    logger.info("Mapping RF channel %d (device=%d, channel=%d) to logical carrier %d on f_rx=%.1f MHz",
                device_mapping.carrier_idx,
                device_mapping.device_idx,
                device_mapping.channel_idx,
                carrier_idx,
                freq / 1e6);
    if (cur_rx_freqs[device_mapping.carrier_idx] != freq) {
      if ((device_mapping.carrier_idx + 1) * nof_antennas <= nof_channels) {
        cur_rx_freqs[device_mapping.carrier_idx] = freq;
        for (uint32_t i = 0; i < nof_antennas; i++) {
          channel_mapping::device_mapping_t dm = rx_channel_mapping.get_device_mapping(carrier_idx, i);
          if (dm.device_idx >= rf_devices.size() or dm.channel_idx >= nof_channels_x_dev) {
            logger.error("Invalid port mapping %d:%d to logical carrier %d on f_rx=%.1f MHz",
                         dm.device_idx,
                         dm.channel_idx,
                         carrier_idx,
                         freq / 1e6);
            return;
          }

          srsran_rf_set_rx_freq(&rf_devices[dm.device_idx], dm.channel_idx, freq + freq_offset);
        }
      } else {
        logger.error("set_rx_freq: physical_channel_idx=%d for %d antennas exceeds maximum channels (%d)",
                     device_mapping.carrier_idx,
                     nof_antennas,
                     nof_channels);
      }
    } else {
      logger.info("RF Rx channel %d already on freq", device_mapping.carrier_idx);
    }
  } else {
    logger.error("set_rx_freq: Could not allocate frequency %.1f MHz to carrier %d", freq / 1e6, carrier_idx);
  }
}

void radio::set_rx_gain(const float& gain)
{
  if (!is_initialized) {
    return;
  }
  for (srsran_rf_t& rf_device : rf_devices) {
    srsran_rf_set_rx_gain(&rf_device, gain);
  }
}

void radio::set_rx_gain_th(const float& gain)
{
  if (!is_initialized) {
    return;
  }
  for (srsran_rf_t& rf_device : rf_devices) {
    srsran_rf_set_rx_gain_th(&rf_device, gain);
  }
}

void radio::set_rx_srate(const double& srate)
{
  if (!is_initialized) {
    return;
  }
  // If fix sampling rate...
  if (std::isnormal(fix_srate_hz)) {
    decimator_busy = true;
    std::unique_lock<std::mutex> lock(rx_mutex);

    // If the sampling rate was not set, set it
    if (not std::isnormal(cur_rx_srate)) {
      for (srsran_rf_t& rf_device : rf_devices) {
        cur_rx_srate = srsran_rf_set_rx_srate(&rf_device, fix_srate_hz);
      }
    }

    // Assert ratio is integer
    srsran_assert(((uint32_t)cur_rx_srate % (uint32_t)srate) == 0,
                  "The sampling rate ratio is not integer (%.2f MHz / %.2f MHz = %.3f)",
                  cur_rx_srate / 1e6,
                  srate / 1e6,
                  cur_rx_srate / srate);

    // Update decimators
    uint32_t ratio = (uint32_t)ceil(cur_rx_srate / srate);
    for (uint32_t ch = 0; ch < nof_channels; ch++) {
      srsran_resampler_fft_init(&decimators[ch], SRSRAN_RESAMPLER_MODE_DECIMATE, ratio);
    }

    decimator_busy = false;
  } else {
    for (srsran_rf_t& rf_device : rf_devices) {
      cur_rx_srate = srsran_rf_set_rx_srate(&rf_device, srate);
    }
  }
}

void radio::set_channel_rx_offset(uint32_t ch, int32_t offset_samples)
{
  uint32_t device_idx = rx_channel_mapping.get_device_mapping(ch, 0).device_idx;

  // Return if invalid index
  if (device_idx >= rf_devices.size()) {
    return;
  }

  // Skip correction if device matches the first logical channel
  uint32_t main_device_idx = rx_channel_mapping.get_device_mapping(0, 0).device_idx;
  if (device_idx == main_device_idx) {
    return;
  }

  // Bound device index
  if (device_idx >= rx_offset_n.size()) {
    return;
  }

  // Skip correction if it has already been set
  if (rx_offset_n[device_idx] != 0) {
    return;
  }

  rx_offset_n[device_idx] = offset_samples;
}

void radio::set_tx_freq(const uint32_t& carrier_idx, const double& freq)
{
  if (!is_initialized) {
    return;
  }

  // Map carrier index to physical channel
  if (tx_channel_mapping.allocate_freq(carrier_idx, freq)) {
    channel_mapping::device_mapping_t device_mapping = tx_channel_mapping.get_device_mapping(carrier_idx);
    if (device_mapping.channel_idx >= nof_channels_x_dev) {
      logger.error("Invalid mapping physical channel %d to logical carrier %d on f_tx=%.1f MHz",
                   device_mapping.channel_idx,
                   carrier_idx,
                   freq / 1e6);
      return;
    }

    logger.info("Mapping RF channel %d (device=%d, channel=%d) to logical carrier %d on f_tx=%.1f MHz",
                device_mapping.carrier_idx,
                device_mapping.device_idx,
                device_mapping.channel_idx,
                carrier_idx,
                freq / 1e6);
    if (cur_tx_freqs[device_mapping.carrier_idx] != freq) {
      if ((device_mapping.carrier_idx + 1) * nof_antennas <= nof_channels) {
        cur_tx_freqs[device_mapping.carrier_idx] = freq;
        for (uint32_t i = 0; i < nof_antennas; i++) {
          device_mapping = tx_channel_mapping.get_device_mapping(carrier_idx, i);
          if (device_mapping.device_idx >= rf_devices.size() or device_mapping.channel_idx >= nof_channels_x_dev) {
            logger.error("Invalid port mapping %d:%d to logical carrier %d on f_rx=%.1f MHz",
                         device_mapping.device_idx,
                         device_mapping.channel_idx,
                         carrier_idx,
                         freq / 1e6);
            return;
          }

          srsran_rf_set_tx_freq(&rf_devices[device_mapping.device_idx], device_mapping.channel_idx, freq + freq_offset);
        }
      } else {
        logger.error("set_tx_freq: physical_channel_idx=%d for %d antennas exceeds maximum channels (%d)",
                     device_mapping.carrier_idx,
                     nof_antennas,
                     nof_channels);
      }
    } else {
      logger.info("RF Tx channel %d already on freq", device_mapping.carrier_idx);
    }
  } else {
    logger.error("set_tx_freq: Could not allocate frequency %.1f MHz to carrier %d", freq / 1e6, carrier_idx);
  }
}

void radio::set_tx_gain(const float& gain)
{
  if (!is_initialized) {
    return;
  }
  for (srsran_rf_t& rf_device : rf_devices) {
    srsran_rf_set_tx_gain(&rf_device, gain);
  }
}

double radio::get_freq_offset()
{
  return freq_offset;
}

float radio::get_rx_gain()
{
  if (!is_initialized) {
    return 0.0f;
  }
  return (float)srsran_rf_get_rx_gain(&rf_devices[0]);
}

double radio::get_dev_cal_tx_adv_sec(const std::string& device_name)
{
  int nsamples = 0;
  /* Set time advance for each known device if in auto mode */
  if (tx_adv_auto) {
    /* This values have been calibrated using the prach_test_usrp tool in srsRAN */

    if (device_name == "uhd_b200") {
      double srate_khz = round(cur_tx_srate / 1e3);
      if (srate_khz == 1.92e3) {
        // 6 PRB
        nsamples = 57;
      } else if (srate_khz == 3.84e3) {
        // 15 PRB
        nsamples = 60;
      } else if (srate_khz == 5.76e3) {
        // 25 PRB
        nsamples = 92;
      } else if (srate_khz == 11.52e3) {
        // 50 PRB
        nsamples = 120;
      } else if (srate_khz == 15.36e3) {
        // 75 PRB
        nsamples = 80;
      } else if (srate_khz == 23.04e3) {
        // 100 PRB
        nsamples = 160;
      } else {
        /* Interpolate from known values */
        srsran::console(
            "\nWarning TX/RX time offset for sampling rate %.0f KHz not calibrated. Using interpolated value\n\n",
            cur_tx_srate);
        nsamples = uhd_default_tx_adv_samples + (int)(cur_tx_srate * uhd_default_tx_adv_offset_sec);
      }

    } else if (device_name == "uhd_usrp2") {
      double srate_khz = round(cur_tx_srate / 1e3);
      if (srate_khz == 1.92e3) {
        nsamples = 14; // estimated
      } else if (srate_khz == 3.84e3) {
        nsamples = 32;
      } else if (srate_khz == 5.76e3) {
        nsamples = 43;
      } else if (srate_khz == 11.52e3) {
        nsamples = 54;
      } else if (srate_khz == 15.36e3) {
        nsamples = 65; // to calc
      } else if (srate_khz == 23.04e3) {
        nsamples = 80; // to calc
      } else {
        /* Interpolate from known values */
        srsran::console(
            "\nWarning TX/RX time offset for sampling rate %.0f KHz not calibrated. Using interpolated value\n\n",
            cur_tx_srate);
        nsamples = uhd_default_tx_adv_samples + (int)(cur_tx_srate * uhd_default_tx_adv_offset_sec);
      }

    } else if (device_name == "lime") {
      double srate_khz = round(cur_tx_srate / 1e3);
      if (srate_khz == 1.92e3) {
        nsamples = 28;
      } else if (srate_khz == 3.84e3) {
        nsamples = 51;
      } else if (srate_khz == 5.76e3) {
        nsamples = 74;
      } else if (srate_khz == 11.52e3) {
        nsamples = 78;
      } else if (srate_khz == 15.36e3) {
        nsamples = 86;
      } else if (srate_khz == 23.04e3) {
        nsamples = 102;
      } else {
        /* Interpolate from known values */
        srsran::console(
            "\nWarning TX/RX time offset for sampling rate %.0f KHz not calibrated. Using interpolated value\n\n",
            cur_tx_srate);
        nsamples = lime_default_tx_adv_samples + (int)(cur_tx_srate * lime_default_tx_adv_offset_sec);
      }

    } else if (device_name == "uhd_x300") {
      // In X300 TX/RX offset is independent of sampling rate
      nsamples = 45;
    } else if (device_name == "bladerf") {
      double srate_khz = round(cur_tx_srate / 1e3);
      if (srate_khz == 1.92e3) {
        nsamples = 16;
      } else if (srate_khz == 3.84e3) {
        nsamples = 18;
      } else if (srate_khz == 5.76e3) {
        nsamples = 16;
      } else if (srate_khz == 11.52e3) {
        nsamples = 21;
      } else if (srate_khz == 15.36e3) {
        nsamples = 14;
      } else if (srate_khz == 23.04e3) {
        nsamples = 21;
      } else {
        /* Interpolate from known values */
        srsran::console(
            "\nWarning TX/RX time offset for sampling rate %.0f KHz not calibrated. Using interpolated value\n\n",
            cur_tx_srate);
        nsamples = blade_default_tx_adv_samples + (int)(blade_default_tx_adv_offset_sec * cur_tx_srate);
      }
    } else if (device_name == "zmq") {
      nsamples = 0;
    }
  } else {
    nsamples = tx_adv_nsamples;
    srsran::console("Setting manual TX/RX offset to %d samples\n", nsamples);
  }

  // Calculate TX advance in seconds from samples and sampling rate
  return (double)nsamples / cur_tx_srate;
}

void radio::set_tx_srate(const double& srate)
{
  std::unique_lock<std::mutex> lock(tx_mutex);
  if (!is_initialized) {
    return;
  }

  // If fix sampling rate...
  if (std::isnormal(fix_srate_hz)) {
    // If the sampling rate was not set, set it
    if (not std::isnormal(cur_tx_srate)) {
      for (srsran_rf_t& rf_device : rf_devices) {
        cur_tx_srate = srsran_rf_set_tx_srate(&rf_device, fix_srate_hz);
      }
    }

    // Assert ratio is integer
    srsran_assert(((uint32_t)cur_tx_srate % (uint32_t)srate) == 0,
                  "The sampling rate ratio is not integer (%.2f MHz / %.2f MHz = %.3f)",
                  cur_rx_srate / 1e6,
                  srate / 1e6,
                  cur_rx_srate / srate);

    // Update interpolators
    uint32_t ratio = (uint32_t)ceil(cur_tx_srate / srate);
    for (uint32_t ch = 0; ch < nof_channels; ch++) {
      srsran_resampler_fft_init(&interpolators[ch], SRSRAN_RESAMPLER_MODE_INTERPOLATE, ratio);
    }
  } else {
    for (srsran_rf_t& rf_device : rf_devices) {
      cur_tx_srate = srsran_rf_set_tx_srate(&rf_device, srate);
    }
  }

  // Get calibrated advanced
  tx_adv_sec = get_dev_cal_tx_adv_sec(std::string(srsran_rf_name(&rf_devices[0])));

  if (tx_adv_sec < 0) {
    tx_adv_sec *= -1;
    tx_adv_negative = true;
  }
}

srsran_rf_info_t* radio::get_info()
{
  if (!is_initialized) {
    return nullptr;
  }
  return srsran_rf_get_info(&rf_devices[0]);
}

bool radio::get_metrics(rf_metrics_t* metrics)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  *metrics   = rf_metrics;
  rf_metrics = {};
  return true;
}

void radio::handle_rf_msg(srsran_rf_error_t error)
{
  if (!is_initialized) {
    return;
  }
  if (error.type == srsran_rf_error_t::SRSRAN_RF_ERROR_OVERFLOW) {
    {
      std::lock_guard<std::mutex> lock(metrics_mutex);
      rf_metrics.rf_o++;
      rf_metrics.rf_error = true;
    }
    logger.info("Overflow");

    // inform PHY about overflow
    if (phy != nullptr) {
      phy->radio_overflow();
    }
  } else if (error.type == srsran_rf_error_t::SRSRAN_RF_ERROR_UNDERFLOW) {
    logger.info("Underflow");
    std::lock_guard<std::mutex> lock(metrics_mutex);
    rf_metrics.rf_u++;
    rf_metrics.rf_error = true;
  } else if (error.type == srsran_rf_error_t::SRSRAN_RF_ERROR_LATE) {
    logger.info("Late (detected in %s)", error.opt ? "rx call" : "asynchronous thread");
    std::lock_guard<std::mutex> lock(metrics_mutex);
    rf_metrics.rf_l++;
    rf_metrics.rf_error = true;
  } else if (error.type == srsran_rf_error_t::SRSRAN_RF_ERROR_RX) {
    logger.error("Fatal radio error occured.");
    phy->radio_failure();
  } else if (error.type == srsran_rf_error_t::SRSRAN_RF_ERROR_OTHER) {
    std::string str(error.msg);
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    logger.info("%s", str.c_str());
  }
}

void radio::rf_msg_callback(void* arg, srsran_rf_error_t error)
{
  radio* h = (radio*)arg;
  if (arg != nullptr) {
    h->handle_rf_msg(error);
  }
}

bool radio::map_channels(const channel_mapping&     map,
                         uint32_t                   device_idx,
                         uint32_t                   sample_offset,
                         const rf_buffer_interface& buffer,
                         void*                      radio_buffers[SRSRAN_MAX_CHANNELS])
{
  // Conversion from safe C++ std::array to the unsafe C interface. We must ensure that the RF driver implementation
  // accepts up to SRSRAN_MAX_CHANNELS buffers
  for (uint32_t i = 0; i < nof_carriers; i++) {
    // Skip if not allocated
    if (not map.is_allocated(i)) {
      continue;
    }

    // Map each antenna
    for (uint32_t j = 0; j < nof_antennas; j++) {
      channel_mapping::device_mapping_t physical_idx = map.get_device_mapping(i, j);

      // Detect mapping out-of-bounds
      if (physical_idx.channel_idx >= nof_channels_x_dev) {
        return false;
      }

      // Set pointer if device index matches
      if (physical_idx.device_idx == device_idx) {
        cf_t* ptr = buffer.get(i, j, nof_antennas);

        // Add sample offset only if it is a valid pointer
        if (ptr != nullptr) {
          ptr += sample_offset;
        }

        radio_buffers[physical_idx.channel_idx] = ptr;
      }
    }
  }

  return true;
}

bool radio::config_rf_channels(const rf_args_t& args)
{
  // Generate RF-Channel to Carrier map
  std::list<channel_mapping::channel_cfg_t> dl_rf_channels = {};
  std::list<channel_mapping::channel_cfg_t> ul_rf_channels = {};

  for (uint32_t i = 0; i < args.nof_carriers; i++) {
    channel_mapping::channel_cfg_t c = {};
    c.carrier_idx                    = i;

    // Parse DL band for this channel
    c.band.set(args.ch_rx_bands[i].min, args.ch_rx_bands[i].max);
    dl_rf_channels.push_back(c);
    logger.info("Configuring physical DL channel %d with band-pass filter (%.1f, %.1f)",
                i,
                c.band.get_low(),
                c.band.get_high());

    // Parse UL band for this channel
    c.band.set(args.ch_tx_bands[i].min, args.ch_tx_bands[i].max);
    ul_rf_channels.push_back(c);
    logger.info("Configuring physical UL channel %d with band-pass filter (%.1f, %.1f)",
                i,
                c.band.get_low(),
                c.band.get_high());
  }

  rx_channel_mapping.set_channels(dl_rf_channels);
  tx_channel_mapping.set_channels(ul_rf_channels);
  return true;
}

} // namespace srsran
