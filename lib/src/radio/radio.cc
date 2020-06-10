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

#include "srslte/srslte.h"
extern "C" {
#include "srslte/phy/rf/rf.h"
}
#include "srslte/common/log_filter.h"
#include "srslte/radio/radio.h"
#include <list>
#include <string.h>
#include <unistd.h>

namespace srslte {

radio::radio(srslte::log_filter* log_h_) : logger(nullptr), log_h(log_h_), zeros(NULL)
{
  zeros = srslte_vec_cf_malloc(SRSLTE_SF_LEN_MAX);
  srslte_vec_cf_zero(zeros, SRSLTE_SF_LEN_MAX);
}

radio::radio(srslte::logger* logger_) : logger(logger_), log_h(nullptr), zeros(NULL)
{
  zeros = srslte_vec_cf_malloc(SRSLTE_SF_LEN_MAX);
  srslte_vec_cf_zero(zeros, SRSLTE_SF_LEN_MAX);
}

radio::~radio()
{
  if (zeros) {
    free(zeros);
    zeros = nullptr;
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
    log_h->console("Error configuring RF channels\n");
    return SRSLTE_ERROR;
  }

  nof_channels = args.nof_antennas * args.nof_carriers;
  nof_antennas = args.nof_antennas;
  nof_carriers = args.nof_carriers;

  cur_tx_freqs.resize(nof_carriers);
  cur_rx_freqs.resize(nof_carriers);

  // Init and start Radio
  char* device_args = nullptr;
  if (args.device_args != "auto") {
    device_args = (char*)args.device_args.c_str();
  }
  char* dev_name = nullptr;
  if (args.device_name != "auto") {
    dev_name = (char*)args.device_name.c_str();
  }
  log_h->console("Opening %d channels in RF device=%s with args=%s\n",
                 nof_channels,
                 dev_name ? dev_name : "default",
                 device_args ? device_args : "default");
  if (srslte_rf_open_devname(&rf_device, dev_name, device_args, nof_channels)) {
    log_h->error("Error opening RF device\n");
    return SRSLTE_ERROR;
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
  if (args.tx_gain > 0) {
    set_tx_gain(args.tx_gain);
  } else {
    // Set same gain than for RX until power control sets a gain
    set_tx_gain(args.rx_gain);
    log_h->console("\nWarning: TX gain was not set. Using open-loop power control (not working properly)\n\n");
  }

  // Frequency offset
  freq_offset = args.freq_offset;

  // Get device info
  rf_info = *get_info();

  // Suppress radio stdout
  srslte_rf_suppress_stdout(&rf_device);

  // Register handler for processing O/U/L
  srslte_rf_register_error_handler(&rf_device, rf_msg_callback, this);

  return SRSLTE_SUCCESS;
}

bool radio::is_init()
{
  return is_initialized;
}

void radio::stop()
{
  if (zeros) {
    free(zeros);
    zeros = NULL;
  }
  if (is_initialized) {
    srslte_rf_close(&rf_device);
  }
}

void radio::reset()
{
  srslte_rf_stop_rx_stream(&rf_device);
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
  if (srslte_rf_start_gain_thread(&rf_device, tx_gain_same_rx)) {
    ERROR("Error starting AGC Thread RF device\n");
    return false;
  }

  return true;
}

bool radio::rx_now(rf_buffer_interface& buffer, const uint32_t& nof_samples, srslte_timestamp_t* rxd_time)
{
  if (!is_initialized) {
    return false;
  }
  bool ret = true;

  if (!radio_is_streaming) {
    srslte_rf_start_rx_stream(&rf_device, false);
    radio_is_streaming = true;
  }

  time_t* full_secs = rxd_time ? &rxd_time->full_secs : NULL;
  double* frac_secs = rxd_time ? &rxd_time->frac_secs : NULL;

  void* radio_buffers[SRSLTE_MAX_CHANNELS] = {};
  if (!map_channels(rx_channel_mapping, 0, buffer, radio_buffers)) {
    log_h->error("Mapping logical channels to physical channels for transmission\n");
    return false;
  }

  if (srslte_rf_recv_with_time_multi(&rf_device, radio_buffers, nof_samples, true, full_secs, frac_secs) > 0) {
    ret = true;
  } else {
    ret = false;
  }

  return ret;
}

void radio::get_time(srslte_timestamp_t* now)
{
  if (!is_initialized) {
    return;
  }
  srslte_rf_get_time(&rf_device, &now->full_secs, &now->frac_secs);
}

float radio::get_rssi()
{
  if (!is_initialized) {
    return 0.0f;
  }
  return srslte_rf_get_rssi(&rf_device);
}

bool radio::has_rssi()
{
  if (!is_initialized) {
    return false;
  }
  return srslte_rf_has_rssi(&rf_device);
}

bool radio::tx(rf_buffer_interface& buffer, const uint32_t& nof_samples_, const srslte_timestamp_t& tx_time_)
{
  uint32_t nof_samples   = nof_samples_;
  uint32_t sample_offset = 0;

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
    srslte_timestamp_t ts_overlap = end_of_burst_time;
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
      sample_offset = (uint32_t)past_nsamples;     // Sets an offset for moving first samples offset
      tx_time       = end_of_burst_time;           // Keeps same transmission time
      nof_samples   = nof_samples - past_nsamples; // Subtracts the number of trimmed samples

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
          int ret = srslte_rf_send_timed2(
              &rf_device, zeros, nzeros, end_of_burst_time.full_secs, end_of_burst_time.frac_secs, false, false);
          if (ret < SRSLTE_SUCCESS) {
            return false;
          }

          // Substract gap samples
          gap_nsamples -= nzeros;

          // Increase timestamp
          srslte_timestamp_add(&end_of_burst_time, 0, (double)nzeros / cur_tx_srate);
        }
      }
    }
  }

  // Save possible end of burst time
  srslte_timestamp_copy(&end_of_burst_time, &tx_time);
  srslte_timestamp_add(&end_of_burst_time, 0, (double)nof_samples / cur_tx_srate);

  void* radio_buffers[SRSLTE_MAX_CHANNELS] = {};
  if (!map_channels(rx_channel_mapping, sample_offset, buffer, radio_buffers)) {
    log_h->error("Mapping logical channels to physical channels for transmission\n");
    return false;
  }

  int ret = srslte_rf_send_timed_multi(
      &rf_device, radio_buffers, nof_samples, tx_time.full_secs, tx_time.frac_secs, true, is_start_of_burst, false);
  is_start_of_burst = false;
  return ret > SRSLTE_SUCCESS;
}

void radio::tx_end()
{
  if (!is_initialized) {
    return;
  }
  if (!is_start_of_burst) {
    srslte_rf_send_timed2(&rf_device, zeros, 0, end_of_burst_time.full_secs, end_of_burst_time.frac_secs, false, true);
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
    uint32_t physical_channel_idx = rx_channel_mapping.get_carrier_idx(carrier_idx);
    log_h->info("Mapping RF channel %d to logical carrier %d on f_rx=%.1f MHz\n",
                physical_channel_idx * nof_antennas,
                carrier_idx,
                freq / 1e6);
    if (cur_rx_freqs[physical_channel_idx] != freq) {
      if ((physical_channel_idx + 1) * nof_antennas <= nof_channels) {
        cur_rx_freqs[physical_channel_idx] = freq;
        for (uint32_t i = 0; i < nof_antennas; i++) {
          srslte_rf_set_rx_freq(&rf_device, physical_channel_idx * nof_antennas + i, freq + freq_offset);
        }
      } else {
        log_h->error("set_rx_freq: physical_channel_idx=%d for %d antennas exceeds maximum channels (%d)\n",
                     physical_channel_idx,
                     nof_antennas,
                     nof_channels);
      }
    } else {
      log_h->info("RF channel %d already on freq\n", physical_channel_idx * nof_antennas);
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
  srslte_rf_set_rx_gain(&rf_device, gain);
}

void radio::set_rx_gain_th(const float& gain)
{
  if (!is_initialized) {
    return;
  }
  srslte_rf_set_rx_gain_th(&rf_device, gain);
}

void radio::set_rx_srate(const double& srate)
{
  if (!is_initialized) {
    return;
  }
  srslte_rf_set_rx_srate(&rf_device, srate);
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
    uint32_t physical_channel_idx = tx_channel_mapping.get_carrier_idx(carrier_idx);
    log_h->info("Mapping RF channel %d to logical carrier %d on f_tx=%.1f MHz\n",
                physical_channel_idx * nof_antennas,
                carrier_idx,
                freq / 1e6);
    if (cur_tx_freqs[physical_channel_idx] != freq) {
      if ((physical_channel_idx + 1) * nof_antennas <= nof_channels) {
        cur_tx_freqs[physical_channel_idx] = freq;
        for (uint32_t i = 0; i < nof_antennas; i++) {
          srslte_rf_set_tx_freq(&rf_device, physical_channel_idx * nof_antennas + i, freq + freq_offset);
        }
      } else {
        log_h->error("set_tx_freq: physical_channel_idx=%d for %d antennas exceeds maximum channels (%d)\n",
                     physical_channel_idx,
                     nof_antennas,
                     nof_channels);
      }
    } else {
      log_h->info("RF channel %d already on freq\n", physical_channel_idx * nof_antennas);
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
  srslte_rf_set_tx_gain(&rf_device, gain);
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
  return srslte_rf_get_rx_gain(&rf_device);
}

void radio::set_tx_srate(const double& srate)
{
  if (!is_initialized) {
    return;
  }
  cur_tx_srate = srslte_rf_set_tx_srate(&rf_device, srate);

  int nsamples = 0;
  /* Set time advance for each known device if in auto mode */
  if (tx_adv_auto) {

    /* This values have been calibrated using the prach_test_usrp tool in srsLTE */

    if (!strcmp(srslte_rf_name(&rf_device), "uhd_b200")) {

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
        log_h->console(
            "\nWarning TX/RX time offset for sampling rate %.0f KHz not calibrated. Using interpolated value\n\n",
            cur_tx_srate);
        nsamples = cur_tx_srate * (uhd_default_tx_adv_samples * (1 / cur_tx_srate) + uhd_default_tx_adv_offset_sec);
      }

    } else if (!strcmp(srslte_rf_name(&rf_device), "uhd_usrp2")) {
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
        log_h->console(
            "\nWarning TX/RX time offset for sampling rate %.0f KHz not calibrated. Using interpolated value\n\n",
            cur_tx_srate);
        nsamples = cur_tx_srate * (uhd_default_tx_adv_samples * (1 / cur_tx_srate) + uhd_default_tx_adv_offset_sec);
      }

    } else if (!strcmp(srslte_rf_name(&rf_device), "lime")) {
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
        log_h->console(
            "\nWarning TX/RX time offset for sampling rate %.0f KHz not calibrated. Using interpolated value\n\n",
            cur_tx_srate);
        nsamples = cur_tx_srate * (uhd_default_tx_adv_samples * (1 / cur_tx_srate) + uhd_default_tx_adv_offset_sec);
      }

    } else if (!strcmp(srslte_rf_name(&rf_device), "uhd_x300")) {

      // In X300 TX/RX offset is independent of sampling rate
      nsamples = 45;
    } else if (!strcmp(srslte_rf_name(&rf_device), "bladerf")) {

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
        log_h->console(
            "\nWarning TX/RX time offset for sampling rate %.0f KHz not calibrated. Using interpolated value\n\n",
            cur_tx_srate);
        nsamples = blade_default_tx_adv_samples + blade_default_tx_adv_offset_sec * cur_tx_srate;
      }
    } else if (!strcmp(srslte_rf_name(&rf_device), "zmq")) {
      nsamples = 0;
    }
  } else {
    nsamples = tx_adv_nsamples;
    log_h->console("Setting manual TX/RX offset to %d samples\n", nsamples);
  }

  // Calculate TX advance in seconds from samples and sampling rate
  tx_adv_sec = nsamples / cur_tx_srate;
  if (tx_adv_sec < 0) {
    tx_adv_sec *= -1;
    tx_adv_negative = true;
  }
}

srslte_rf_info_t* radio::get_info()
{
  if (!is_initialized) {
    return NULL;
  }
  return srslte_rf_get_info(&rf_device);
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

bool radio::map_channels(channel_mapping&           map,
                         uint32_t                   sample_offset,
                         const rf_buffer_interface& buffer,
                         void*                      radio_buffers[SRSLTE_MAX_CHANNELS])
{
  // Discard channels not allocated, need to point to valid buffer
  for (uint32_t i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
    radio_buffers[i] = zeros;
  }
  // Conversion from safe C++ std::array to the unsafe C interface. We must ensure that the RF driver implementation
  // accepts up to SRSLTE_MAX_CHANNELS buffers
  for (uint32_t i = 0; i < nof_carriers; i++) {
    if (map.is_allocated(i)) {
      uint32_t physical_idx = map.get_carrier_idx(i);
      for (uint32_t j = 0; j < nof_antennas; j++) {
        if (physical_idx * nof_antennas + j < SRSLTE_MAX_CHANNELS) {
          cf_t* ptr = buffer.get(i, j, nof_antennas);

          // Add sample offset only if it is a valid pointer
          if (ptr != nullptr) {
            ptr += sample_offset;
          }

          radio_buffers[physical_idx * nof_antennas + j] = ptr;
        } else {
          return false;
        }
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

/***
 * Carrier mapping class
 */
bool radio::channel_mapping::allocate_freq(const uint32_t& logical_ch, const float& freq)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (allocated_channels.count(logical_ch)) {
    ERROR("allocate_freq: Carrier logical_ch=%d already allocated to channel=%d\n",
          logical_ch,
          allocated_channels[logical_ch].carrier_idx);
    return false;
  }

  // Find first available channel that supports this frequency and allocated it
  for (auto c = available_channels.begin(); c != available_channels.end(); ++c) {
    if (c->band.contains(freq)) {
      allocated_channels[logical_ch] = *c;
      available_channels.erase(c);
      return true;
    }
  }
  ERROR("allocate_freq: No channels available for frequency=%.1f\n", freq);
  return false;
}

bool radio::channel_mapping::release_freq(const uint32_t& logical_ch)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (allocated_channels.count(logical_ch)) {
    available_channels.push_back(allocated_channels[logical_ch]);
    allocated_channels.erase(logical_ch);
    return true;
  }
  return false;
}

int radio::channel_mapping::get_carrier_idx(const uint32_t& logical_ch)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (allocated_channels.count(logical_ch)) {
    return allocated_channels[logical_ch].carrier_idx;
  }
  return -1;
}

bool radio::channel_mapping::is_allocated(const uint32_t& logical_ch)
{
  std::lock_guard<std::mutex> lock(mutex);
  return allocated_channels.count(logical_ch) > 0;
}

} // namespace srslte
