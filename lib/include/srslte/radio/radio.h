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

#include "channel_mapping.h"
#include "radio_metrics.h"
#include "rf_buffer.h"
#include "rf_timestamp.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/log_filter.h"
#include "srslte/interfaces/radio_interfaces.h"
#include "srslte/phy/resampling/resampler.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/radio/radio_base.h"
#include "srslte/srslte.h"
#include <list>
#include <string>

#ifndef SRSLTE_RADIO_H
#define SRSLTE_RADIO_H

namespace srslte {

/**
 * Implementation of the radio interface for the PHY
 *
 * It uses the rf C library object to access the underlying radio. This implementation uses a flat array to
 * transmit/receive samples for all RF channels. The N carriers and P antennas are mapped into M=NP RF channels (M <=
 * SRSLTE_MAX_CHANNELS). Note that all carriers must have the same number of antennas.
 *
 * The underlying radio receives and transmits M RF channels synchronously from possibly multiple radios using the same
 * rf driver object. In the current implementation, the mapping between N carriers and P antennas is sequentially, eg:
 * [carrier_0_port_0, carrier_0_port_1, carrier_1_port_0, carrier_1_port_1, ..., carrier_N_port_N]
 */
class radio : public radio_interface_phy, public srslte::radio_base
{
public:
  radio(srslte::log_filter* log_h);
  radio(srslte::logger* logger_h);
  ~radio();

  int         init(const rf_args_t& args_, phy_interface_radio* phy_) final;
  void        stop() final;
  std::string get_type() override { return "radio"; }

  // ==== PHY interface ===

  // trx functions
  void tx_end() override;
  bool tx(rf_buffer_interface& buffer, const rf_timestamp_interface& tx_time) override;
  bool rx_now(rf_buffer_interface& buffer, rf_timestamp_interface& rxd_time) override;

  // setter
  void set_tx_freq(const uint32_t& carrier_idx, const double& freq) override;
  void set_rx_freq(const uint32_t& carrier_idx, const double& freq) override;
  void release_freq(const uint32_t& carrier_idx) override;

  void set_tx_gain(const float& gain) override;
  void set_rx_gain_th(const float& gain) override;
  void set_rx_gain(const float& gain) override;
  void set_tx_srate(const double& srate) override;
  void set_rx_srate(const double& srate) override;
  void set_channel_rx_offset(uint32_t ch, int32_t offset_samples) override;

  // getter
  double            get_freq_offset() override;
  float             get_rx_gain() override;
  bool              is_continuous_tx() override;
  bool              get_is_start_of_burst() override;
  bool              is_init() override;
  void              reset() override;
  srslte_rf_info_t* get_info() override;

  // Other functions
  bool get_metrics(rf_metrics_t* metrics) final;

  void        handle_rf_msg(srslte_rf_error_t error);
  static void rf_msg_callback(void* arg, srslte_rf_error_t error);

private:
  std::vector<srslte_rf_t>                                rf_devices  = {};
  std::vector<srslte_rf_info_t>                           rf_info     = {};
  std::vector<int32_t>                                    rx_offset_n = {};
  rf_metrics_t                                            rf_metrics  = {};
  log_filter                                              log_local   = {};
  log_filter*                                             log_h       = nullptr;
  srslte::logger*                                         logger      = nullptr;
  phy_interface_radio*                                    phy         = nullptr;
  cf_t*                                                   zeros       = nullptr;
  std::array<cf_t*, SRSLTE_MAX_CHANNELS>                  dummy_buffers;
  std::mutex                                              tx_mutex;
  std::mutex                                              rx_mutex;
  std::array<std::vector<cf_t>, SRSLTE_MAX_CHANNELS>      tx_buffer;
  std::array<std::vector<cf_t>, SRSLTE_MAX_CHANNELS>      rx_buffer;
  std::array<srslte_resampler_fft_t, SRSLTE_MAX_CHANNELS> interpolators = {};
  std::array<srslte_resampler_fft_t, SRSLTE_MAX_CHANNELS> decimators    = {};

  rf_timestamp_t end_of_burst_time  = {};
  bool           is_start_of_burst  = false;
  uint32_t       tx_adv_nsamples    = 0;
  double         tx_adv_sec         = 0.0; // Transmission time advance to compensate for antenna->timestamp delay
  bool           tx_adv_auto        = false;
  bool           tx_adv_negative    = false;
  bool           is_initialized     = false;
  bool           radio_is_streaming = false;
  bool           continuous_tx      = false;
  double         freq_offset        = 0.0;
  double         cur_tx_srate       = 0.0;
  double         cur_rx_srate       = 0.0;
  double         fix_srate_hz       = 0.0;
  uint32_t       nof_antennas       = 0;
  uint32_t       nof_channels       = 0;
  uint32_t       nof_channels_x_dev = 0;
  uint32_t       nof_carriers       = 0;

  std::vector<double> cur_tx_freqs = {};
  std::vector<double> cur_rx_freqs = {};

  constexpr static double tx_max_gap_zeros = 4e-3; ///< Maximum transmission gap to fill with zeros, otherwise the burst
                                                   ///< shall be stopped

  // Define default values for known radios
  constexpr static int    uhd_default_tx_adv_samples    = 98;
  constexpr static double uhd_default_tx_adv_offset_sec = 4 * 1e-6;

  constexpr static int    lime_default_tx_adv_samples    = 98;
  constexpr static double lime_default_tx_adv_offset_sec = 4 * 1e-6;

  constexpr static int    blade_default_tx_adv_samples    = 27;
  constexpr static double blade_default_tx_adv_offset_sec = 1e-6;

  /**
   * Get device calibrated transmit time in advanced seconds
   * @param device_name actual device name
   * @return transmit time in advanced in seconds
   */
  double get_dev_cal_tx_adv_sec(const std::string& device_name);

  channel_mapping rx_channel_mapping = {}, tx_channel_mapping = {};

  /**
   * Helper method for opening a RF device
   *
   * @param device_idx Device index
   * @param device_name Device name
   * @param devive_args  Device arguments
   * @return it returns true if the device was opened successful, otherwise it returns false
   */
  bool open_dev(const uint32_t& device_idx, const std::string& device_name, const std::string& devive_args);

  /**
   * Helper method for transmitting over a single RF device. This function maps automatically the logical transmit
   * buffers to the physical RF buffers for the given device.
   *
   * Also, it takes care internally of transmission gaps and overlaps. So, it applies time compensation per channel
   * basis.
   *
   * @param device_idx Device index
   * @param buffer Common transmit buffer
   * @param nof_samples_ number of samples to transmit
   * @param tx_time_ Timestamp to transmit (read only)
   * @return it returns true if the transmission was successful, otherwise it returns false
   */
  bool tx_dev(const uint32_t& device_idx, rf_buffer_interface& buffer, const srslte_timestamp_t& tx_time_);

  /**
   * Helper method for receiving over a single RF device. This function maps automatically the logical receive buffers
   * to the physical RF buffers for the given device.
   *
   * @param device_idx Device index
   * @param buffer Common receive buffers
   * @param rxd_time Points at the receive time (write only)
   * @return it returns true if the reception was successful, otherwise it returns false
   */
  bool rx_dev(const uint32_t& device_idx, const rf_buffer_interface& buffer, srslte_timestamp_t* rxd_time);

  /**
   * Helper method for mapping logical channels into physical radio buffers.
   *
   * @param map Channel mapping, it can be either Tx or Rx mapping
   * @param device_idx RF Device index for the buffer mapping
   * @param sample_offset The physical radio buffer pointer offset
   * @param buffer Logical channels buffer
   * @param radio_buffers Actual physical radio buffer
   * @return It returns true if the mapping was successful, otherwise it returns false.
   */
  bool map_channels(const channel_mapping&     map,
                    uint32_t                   device_idx,
                    uint32_t                   sample_offset,
                    const rf_buffer_interface& buffer,
                    void*                      radio_buffers[SRSLTE_MAX_CHANNELS]);
  bool start_agc(bool tx_gain_same_rx = false);
  void set_tx_adv(int nsamples);
  void set_tx_adv_neg(bool tx_adv_is_neg);
  bool config_rf_channels(const rf_args_t& args);
};

} // namespace srslte

#endif // SRSLTE_RADIO_H
