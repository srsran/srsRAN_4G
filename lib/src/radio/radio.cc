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

#include "srslte/radio/radio.h"
#include "srslte/common/string_helpers.h"
#include "srslte/config.h"
#include <list>
#include <string>
#include <unistd.h>

namespace srslte {

radio::radio(srslte::log_filter* log_h_) : logger(nullptr), log_h(log_h_), zeros(nullptr)
{
  zeros = srslte_vec_cf_malloc(SRSLTE_SF_LEN_MAX);
  srslte_vec_cf_zero(zeros, SRSLTE_SF_LEN_MAX);
  for (uint32_t i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
    dummy_buffers[i] = srslte_vec_cf_malloc(SRSLTE_SF_LEN_MAX * SRSLTE_NOF_SF_X_FRAME);
    srslte_vec_cf_zero(dummy_buffers[i], SRSLTE_SF_LEN_MAX * SRSLTE_NOF_SF_X_FRAME);
  }
}

radio::radio(srslte::logger* logger_) : logger(logger_), log_h(nullptr), zeros(nullptr)
{
  zeros = srslte_vec_cf_malloc(SRSLTE_SF_LEN_MAX);
  srslte_vec_cf_zero(zeros, SRSLTE_SF_LEN_MAX);
  for (uint32_t i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
    dummy_buffers[i] = srslte_vec_cf_malloc(SRSLTE_SF_LEN_MAX);
  }
}

radio::~radio()
{
  if (zeros) {
    free(zeros);
    zeros = nullptr;
  }

  for (uint32_t i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
    if (dummy_buffers[i]) {
      free(dummy_buffers[i]);
    }
  }

  for (srslte_resampler_fft_t& q : interpolators) {
    srslte_resampler_fft_free(&q);
  }

  for (srslte_resampler_fft_t& q : decimators) {
    srslte_resampler_fft_free(&q);
  }
}

int radio::init(const rf_args_t& args, phy_interface_radio* phy_)
{
  phy = phy_;

  // Init log
  if (log_h == nullptr) {
    if (logger != nullptr) {
      log_local.init("RF  ", logger);
      log_local.set_level(args.log_level);
      log_h = &log_local;
    } else {
      fprintf(stderr, "Must all radio constructor with either logger or log_filter\n");
      return SRSLTE_ERROR;
    }
  }

  if (args.nof_antennas > SRSLTE_MAX_PORTS) {
    log_h->error("Maximum number of antennas exceeded (%d > %d)\n", args.nof_antennas, SRSLTE_MAX_PORTS);
    return SRSLTE_ERROR;
  }

  if (args.nof_carriers > SRSLTE_MAX_CARRIERS) {
    log_h->error("Maximum number of carriers exceeded (%d > %d)\n", args.nof_carriers, SRSLTE_MAX_CARRIERS);
    return SRSLTE_ERROR;
  }

  if (!config_rf_channels(args)) {
    srslte::console("Error configuring RF channels\n");
    return SRSLTE_ERROR;
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
    srslte::console(
        "Error: The number of required RF channels (%d) is not divisible between the number of RF devices (%zd).\n",
        nof_channels,
        device_args_list.size());
    return SRSLTE_ERROR;
  }
  nof_channels_x_dev = nof_channels / device_args_list.size();

  // Allocate RF devices
  rf_devices.resize(device_args_list.size());
  rf_info.resize(device_args_list.size());
  rx_offset_n.resize(device_args_list.size());

  tx_channel_mapping.set_config(nof_channels_x_dev, nof_antennas);
  rx_channel_mapping.set_config(nof_channels_x_dev, nof_antennas);

  // Init and start Radios
  for (uint32_t device_idx = 0; device_idx < (uint32_t)device_args_list.size(); device_idx++) {
    if (not open_dev(device_idx, args.device_name, device_args_list[device_idx])) {
      log_h->error("Error opening RF device %d\n", device_idx);
      return SRSLTE_ERROR;
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
    srslte::console("\nWarning: TX gain was not set. Using open-loop power control (not working properly)\n\n");
  }

  // Set individual gains
  for (uint32_t i = 0; i < args.nof_carriers; i++) {
    if (args.tx_gain_ch[i] > 0) {
      for (uint32_t j = 0; j < nof_antennas; j++) {
        uint32_t phys_antenna_idx = i * nof_antennas + j;

        // From channel number deduce RF device index and channel
        uint32_t rf_device_idx  = phys_antenna_idx / nof_channels_x_dev;
        uint32_t rf_channel_idx = phys_antenna_idx % nof_channels_x_dev;

        log_h->info(
            "Setting individual tx_gain=%.1f on dev=%d ch=%d\n", args.tx_gain_ch[i], rf_device_idx, rf_channel_idx);
        if (srslte_rf_set_tx_gain_ch(&rf_devices[rf_device_idx], rf_channel_idx, args.tx_gain_ch[i]) < 0) {
          log_h->error(
              "Setting channel tx_gain=%.1f on dev=%d ch=%d\n", args.tx_gain_ch[i], rf_device_idx, rf_channel_idx);
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

        log_h->info(
            "Setting individual rx_gain=%.1f on dev=%d ch=%d\n", args.rx_gain_ch[i], rf_device_idx, rf_channel_idx);
        if (srslte_rf_set_rx_gain_ch(&rf_devices[rf_device_idx], rf_channel_idx, args.rx_gain_ch[i]) < 0) {
          log_h->error(
              "Setting channel rx_gain=%.1f on dev=%d ch=%d\n", args.rx_gain_ch[i], rf_device_idx, rf_channel_idx);
        }
      }
    }
  }

  // Set resampler buffers to 5 ms
  if (std::isnormal(fix_srate_hz)) {
    for (auto& buf : rx_buffer) {
      buf.resize(size_t(fix_srate_hz / 200));
    }
    for (auto& buf : tx_buffer) {
      buf.resize(size_t(fix_srate_hz / 200));
    }
  }

  // Frequency offset
  freq_offset = args.freq_offset;

  return SRSLTE_SUCCESS;
}

bool radio::is_init()
{
  return is_initialized;
}

void radio::stop()
{
  // Stop Rx streams as soon as possible to avoid Overflows
  if (radio_is_streaming) {
    for (srslte_rf_t& rf_device : rf_devices) {
      srslte_rf_stop_rx_stream(&rf_device);
    }
  }
  if (zeros) {
    free(zeros);
    zeros = NULL;
  }
  if (is_initialized) {
    for (srslte_rf_t& rf_device : rf_devices) {
      srslte_rf_close(&rf_device);
    }
  }
}

void radio::reset()
{
  for (srslte_rf_t& rf_device : rf_devices) {
    srslte_rf_stop_rx_stream(&rf_device);
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
  for (srslte_rf_t& rf_device : rf_devices) {
    if (srslte_rf_start_gain_thread(&rf_device, tx_gain_same_rx)) {
      ERROR("Error starting AGC Thread RF device\n");
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
  uint32_t                     ratio = SRSLTE_MAX(1, decimators[0].ratio);

  // If the interpolator have been set, interpolate
  for (uint32_t ch = 0; ch < nof_channels; ch++) {
    // Use rx buffer if decimator is required
    buffer_rx.set(ch, ratio > 1 ? rx_buffer[ch].data() : buffer.get(ch));
  }

  // Set new buffer size
  buffer_rx.set_nof_samples(buffer.get_nof_samples() * ratio);

  if (not radio_is_streaming) {
    for (srslte_rf_t& rf_device : rf_devices) {
      srslte_rf_start_rx_stream(&rf_device, false);
    }
    radio_is_streaming = true;

    // Flush buffers to compensate settling time
    if (rf_devices.size() > 1) {
      for (srslte_rf_t& rf_device : rf_devices) {
        srslte_rf_flush_buffer(&rf_device);
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
        srslte_resampler_fft_run(&decimators[ch], buffer_rx.get(ch), buffer.get(ch), buffer_rx.get_nof_samples());
      }
    }
  }

  return ret;
}

bool radio::rx_dev(const uint32_t& device_idx, const rf_buffer_interface& buffer, srslte_timestamp_t* rxd_time)
{
  if (!is_initialized) {
    return false;
  }

  time_t* full_secs = rxd_time ? &rxd_time->full_secs : nullptr;
  double* frac_secs = rxd_time ? &rxd_time->frac_secs : nullptr;

  void* radio_buffers[SRSLTE_MAX_CHANNELS] = {};

  // Discard channels not allocated, need to point to valid buffer
  for (uint32_t i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
    radio_buffers[i] = dummy_buffers[i];
  }

  if (not map_channels(rx_channel_mapping, device_idx, 0, buffer, radio_buffers)) {
    log_h->error("Mapping logical channels to physical channels for transmission\n");
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
    nof_samples = SRSLTE_MIN(nof_samples + nof_samples_offset, 2 * nof_samples);
  }

  // Subtract number of offset samples
  rx_offset_n.at(device_idx) = nof_samples_offset - ((int)nof_samples - (int)buffer.get_nof_samples());

  int ret =
      srslte_rf_recv_with_time_multi(&rf_devices[device_idx], radio_buffers, nof_samples, true, full_secs, frac_secs);

  // If the number of received samples filled the buffer, there is nothing else to do
  if (buffer.get_nof_samples() <= nof_samples) {
    return ret > 0;
  }

  // Otherwise, set rest of buffer to zero
  uint32_t nof_zeros = buffer.get_nof_samples() - nof_samples;
  for (auto& b : radio_buffers) {
    if (b != nullptr) {
      cf_t* ptr = (cf_t*)b;
      srslte_vec_cf_zero(&ptr[nof_samples], nof_zeros);
    }
  }

  return ret > 0;
}

bool radio::tx(rf_buffer_interface& buffer, const rf_timestamp_interface& tx_time)
{
  bool                         ret = true;
  std::unique_lock<std::mutex> lock(tx_mutex);

  // If the interpolator have been set, interpolate
  if (interpolators[0].ratio > 1) {
    for (uint32_t ch = 0; ch < nof_channels; ch++) {
      // Perform actual interpolation
      srslte_resampler_fft_run(&interpolators[ch], buffer.get(ch), tx_buffer[ch].data(), buffer.get_nof_samples());

      // Set the buffer pointer
      buffer.set(ch, tx_buffer[ch].data());
    }

    // Set new buffer size
    buffer.set_nof_samples(buffer.get_nof_samples() * interpolators[0].ratio);
  }

  for (uint32_t device_idx = 0; device_idx < (uint32_t)rf_devices.size(); device_idx++) {
    ret &= tx_dev(device_idx, buffer, tx_time.get(device_idx));
  }

  is_start_of_burst = false;

  return ret;
}

bool radio::open_dev(const uint32_t& device_idx, const std::string& device_name, const std::string& devive_args)
{
  srslte_rf_t* rf_device = &rf_devices[device_idx];

  char* dev_args = nullptr;
  if (devive_args != "auto") {
    dev_args = (char*)devive_args.c_str();
  }

  char* dev_name = nullptr;
  if (device_name != "auto") {
    dev_name = (char*)device_name.c_str();
  }

  srslte::console("Opening %d channels in RF device=%s with args=%s\n",
                     nof_channels_x_dev,
                     dev_name ? dev_name : "default",
                     dev_args ? dev_args : "default");

  if (srslte_rf_open_devname(rf_device, dev_name, dev_args, nof_channels_x_dev)) {
    log_h->error("Error opening RF device\n");
    return false;
  }

  // Suppress radio stdout
  srslte_rf_suppress_stdout(rf_device);

  // Register handler for processing O/U/L
  srslte_rf_register_error_handler(rf_device, rf_msg_callback, this);

  // Get device info
  rf_info[device_idx] = *srslte_rf_get_info(rf_device);

  return true;
}

bool radio::tx_dev(const uint32_t& device_idx, rf_buffer_interface& buffer, const srslte_timestamp_t& tx_time_)
{
  uint32_t     nof_samples   = buffer.get_nof_samples();
  uint32_t     sample_offset = 0;
  srslte_rf_t* rf_device     = &rf_devices[device_idx];

  // Return instantly if the radio module is not initialised
  if (!is_initialized) {
    return false;
  }

  // Copy timestamp and add Tx time offset calibration
  srslte_timestamp_t tx_time = tx_time_;
  if (!tx_adv_negative) {
    srslte_timestamp_sub(&tx_time, 0, tx_adv_sec);
  } else {
    srslte_timestamp_add(&tx_time, 0, tx_adv_sec);
  }

  // Calculate transmission overlap/gap if it is not start of the burst
  if (not is_start_of_burst) {
    // Calculates transmission time overlap with previous transmission
    srslte_timestamp_t ts_overlap = end_of_burst_time[device_idx];
    srslte_timestamp_sub(&ts_overlap, tx_time.full_secs, tx_time.frac_secs);

    // Calculates number of overlap samples with previous transmission
    int32_t past_nsamples = (int32_t)round(cur_tx_srate * srslte_timestamp_real(&ts_overlap));

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

    } else if (past_nsamples < 0) {
      // if the gap is bigger than TX_MAX_GAP_ZEROS, stop burst
      if (fabs(srslte_timestamp_real(&ts_overlap)) > tx_max_gap_zeros) {
        tx_end();
      } else {
        // Otherwise, transmit zeros
        uint32_t gap_nsamples = abs(past_nsamples);
        while (gap_nsamples > 0) {
          // Transmission cannot exceed SRSLTE_SF_LEN_MAX (zeros buffer size limitation)
          uint32_t nzeros = SRSLTE_MIN(gap_nsamples, SRSLTE_SF_LEN_MAX);

          // Zeros transmission
          int ret = srslte_rf_send_timed2(rf_device,
                                          zeros,
                                          nzeros,
                                          end_of_burst_time[device_idx].full_secs,
                                          end_of_burst_time[device_idx].frac_secs,
                                          false,
                                          false);
          if (ret < SRSLTE_SUCCESS) {
            return false;
          }

          // Substract gap samples
          gap_nsamples -= nzeros;

          // Increase timestamp
          srslte_timestamp_add(&end_of_burst_time[device_idx], 0, (double)nzeros / cur_tx_srate);
        }
      }
    }
  }

  // Save possible end of burst time
  srslte_timestamp_copy(&end_of_burst_time[device_idx], &tx_time);
  srslte_timestamp_add(&end_of_burst_time[device_idx], 0, (double)nof_samples / cur_tx_srate);

  void* radio_buffers[SRSLTE_MAX_CHANNELS] = {};

  // Discard channels not allocated, need to point to valid buffer
  for (uint32_t i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
    radio_buffers[i] = zeros;
  }

  if (not map_channels(tx_channel_mapping, device_idx, sample_offset, buffer, radio_buffers)) {
    log_h->error("Mapping logical channels to physical channels for transmission\n");
    return false;
  }

  int ret = srslte_rf_send_timed_multi(
      rf_device, radio_buffers, nof_samples, tx_time.full_secs, tx_time.frac_secs, true, is_start_of_burst, false);

  return ret > SRSLTE_SUCCESS;
}

void radio::tx_end()
{
  if (!is_initialized) {
    return;
  }
  if (!is_start_of_burst) {
    for (uint32_t i = 0; i < (uint32_t)rf_devices.size(); i++) {
      srslte_rf_send_timed2(
          &rf_devices[i], zeros, 0, end_of_burst_time[i].full_secs, end_of_burst_time[i].frac_secs, false, true);
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

  // First release mapping
  rx_channel_mapping.release_freq(carrier_idx);

  // Map carrier index to physical channel
  if (rx_channel_mapping.allocate_freq(carrier_idx, freq)) {
    channel_mapping::device_mapping_t device_mapping = rx_channel_mapping.get_device_mapping(carrier_idx);
    log_h->info("Mapping RF channel %d (device=%d, channel=%d) to logical carrier %d on f_rx=%.1f MHz\n",
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
          srslte_rf_set_rx_freq(&rf_devices[dm.device_idx], dm.channel_idx, freq + freq_offset);
        }
      } else {
        log_h->error("set_rx_freq: physical_channel_idx=%d for %d antennas exceeds maximum channels (%d)\n",
                     device_mapping.carrier_idx,
                     nof_antennas,
                     nof_channels);
      }
    } else {
      log_h->info("RF Rx channel %d already on freq\n", device_mapping.carrier_idx);
    }
  } else {
    log_h->error("set_rx_freq: Could not allocate frequency %.1f MHz to carrier %d\n", freq / 1e6, carrier_idx);
  }
}

void radio::set_rx_gain(const float& gain)
{
  if (!is_initialized) {
    return;
  }
  for (srslte_rf_t& rf_device : rf_devices) {
    srslte_rf_set_rx_gain(&rf_device, gain);
  }
}

void radio::set_rx_gain_th(const float& gain)
{
  if (!is_initialized) {
    return;
  }
  for (srslte_rf_t& rf_device : rf_devices) {
    srslte_rf_set_rx_gain_th(&rf_device, gain);
  }
}

void radio::set_rx_srate(const double& srate)
{

  if (!is_initialized) {
    return;
  }
  // If fix sampling rate...
  if (std::isnormal(fix_srate_hz)) {
    std::unique_lock<std::mutex> lock(rx_mutex);

    // If the sampling rate was not set, set it
    if (not std::isnormal(cur_rx_srate)) {
      for (srslte_rf_t& rf_device : rf_devices) {
        cur_rx_srate = srslte_rf_set_rx_srate(&rf_device, fix_srate_hz);
      }
    }

    // Update decimators
    uint32_t ratio = (uint32_t)ceil(cur_rx_srate / srate);
    for (uint32_t ch = 0; ch < nof_channels; ch++) {
      srslte_resampler_fft_init(&decimators[ch], SRSLTE_RESAMPLER_MODE_DECIMATE, ratio);
    }

  } else {
    for (srslte_rf_t& rf_device : rf_devices) {
      cur_rx_srate = srslte_rf_set_rx_srate(&rf_device, srate);
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

  // First release mapping
  tx_channel_mapping.release_freq(carrier_idx);

  // Map carrier index to physical channel
  if (tx_channel_mapping.allocate_freq(carrier_idx, freq)) {
    channel_mapping::device_mapping_t device_mapping = tx_channel_mapping.get_device_mapping(carrier_idx);
    log_h->info("Mapping RF channel %d (device=%d, channel=%d) to logical carrier %d on f_tx=%.1f MHz\n",
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

          srslte_rf_set_tx_freq(&rf_devices[device_mapping.device_idx], device_mapping.channel_idx, freq + freq_offset);
        }
      } else {
        log_h->error("set_tx_freq: physical_channel_idx=%d for %d antennas exceeds maximum channels (%d)\n",
                     device_mapping.carrier_idx,
                     nof_antennas,
                     nof_channels);
      }
    } else {
      log_h->info("RF Tx channel %d already on freq\n", device_mapping.carrier_idx);
    }
  } else {
    log_h->error("set_tx_freq: Could not allocate frequency %.1f MHz to carrier %d\n", freq / 1e6, carrier_idx);
  }
}

void radio::set_tx_gain(const float& gain)
{
  if (!is_initialized) {
    return;
  }
  for (srslte_rf_t& rf_device : rf_devices) {
    srslte_rf_set_tx_gain(&rf_device, gain);
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
  return (float)srslte_rf_get_rx_gain(&rf_devices[0]);
}

double radio::get_dev_cal_tx_adv_sec(const std::string& device_name)
{
  int nsamples = 0;
  /* Set time advance for each known device if in auto mode */
  if (tx_adv_auto) {

    /* This values have been calibrated using the prach_test_usrp tool in srsLTE */

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
        srslte::console(
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
        srslte::console(
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
        srslte::console(
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
        srslte::console(
            "\nWarning TX/RX time offset for sampling rate %.0f KHz not calibrated. Using interpolated value\n\n",
            cur_tx_srate);
        nsamples = blade_default_tx_adv_samples + (int)(blade_default_tx_adv_offset_sec * cur_tx_srate);
      }
    } else if (device_name == "zmq") {
      nsamples = 0;
    }
  } else {
    nsamples = tx_adv_nsamples;
    srslte::console("Setting manual TX/RX offset to %d samples\n", nsamples);
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
      for (srslte_rf_t& rf_device : rf_devices) {
        cur_tx_srate = srslte_rf_set_tx_srate(&rf_device, fix_srate_hz);
      }
    }

    // Update interpolators
    uint32_t ratio = (uint32_t)ceil(cur_tx_srate / srate);
    for (uint32_t ch = 0; ch < nof_channels; ch++) {
      srslte_resampler_fft_init(&interpolators[ch], SRSLTE_RESAMPLER_MODE_INTERPOLATE, ratio);
    }
  } else {
    for (srslte_rf_t& rf_device : rf_devices) {
      cur_tx_srate = srslte_rf_set_tx_srate(&rf_device, srate);
    }
  }

  // Get calibrated advanced
  tx_adv_sec = get_dev_cal_tx_adv_sec(std::string(srslte_rf_name(&rf_devices[0])));

  if (tx_adv_sec < 0) {
    tx_adv_sec *= -1;
    tx_adv_negative = true;
  }
}

srslte_rf_info_t* radio::get_info()
{
  if (!is_initialized) {
    return nullptr;
  }
  return srslte_rf_get_info(&rf_devices[0]);
}

bool radio::get_metrics(rf_metrics_t* metrics)
{
  *metrics   = rf_metrics;
  rf_metrics = {};
  return true;
}

void radio::handle_rf_msg(srslte_rf_error_t error)
{
  if (!is_initialized) {
    return;
  }
  if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_OVERFLOW) {
    rf_metrics.rf_o++;
    rf_metrics.rf_error = true;
    log_h->info("Overflow\n");

    // inform PHY about overflow
    phy->radio_overflow();
  } else if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_UNDERFLOW) {
    rf_metrics.rf_u++;
    rf_metrics.rf_error = true;
    log_h->info("Underflow\n");
  } else if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_LATE) {
    rf_metrics.rf_l++;
    rf_metrics.rf_error = true;
    log_h->info("Late (detected in %s)\n", error.opt ? "rx call" : "asynchronous thread");
  } else if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_RX) {
    log_h->error("Fatal radio error occured.\n");
    phy->radio_failure();
  } else if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_OTHER) {
    std::string str(error.msg);
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    str.push_back('\n');
    log_h->info("%s\n", str.c_str());
  }
}

void radio::rf_msg_callback(void* arg, srslte_rf_error_t error)
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
                         void*                      radio_buffers[SRSLTE_MAX_CHANNELS])
{
  // Conversion from safe C++ std::array to the unsafe C interface. We must ensure that the RF driver implementation
  // accepts up to SRSLTE_MAX_CHANNELS buffers
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
    log_h->info("Configuring physical DL channel %d with band-pass filter (%.1f, %.1f)\n",
                i,
                c.band.get_low(),
                c.band.get_high());

    // Parse UL band for this channel
    c.band.set(args.ch_tx_bands[i].min, args.ch_tx_bands[i].max);
    ul_rf_channels.push_back(c);
    log_h->info("Configuring physical UL channel %d with band-pass filter (%.1f, %.1f)\n",
                i,
                c.band.get_low(),
                c.band.get_high());
  }

  rx_channel_mapping.set_channels(dl_rf_channels);
  tx_channel_mapping.set_channels(ul_rf_channels);
  return true;
}

} // namespace srslte
