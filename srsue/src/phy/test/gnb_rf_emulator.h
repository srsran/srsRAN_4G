/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_GNB_RF_EMULATOR_H
#define SRSRAN_GNB_RF_EMULATOR_H

#include "gnb_emulator.h"
#include <set>
#include <srsran/interfaces/radio_interfaces.h>
#include <srsran/srslog/srslog.h>
#include <srsran/srsran.h>
#include <srsran/support/srsran_assert.h>

class gnb_rf_emulator final : public srsran::radio_interface_phy
{
private:
  const uint32_t                              BUFFER_SIZE_SF = 10;
  const std::string                           LOGNAME        = "RF";
  uint32_t                                    sf_len         = 0;
  srsran_ringbuffer_t                         ringbuffer     = {};
  uint32_t                                    slot_idx       = 0;
  std::atomic<bool>                           running        = {true};
  srsran::rf_timestamp_t                      ts_write       = {};
  std::vector<cf_t>                           buffer;
  std::vector<std::shared_ptr<gnb_emulator> > gnb_vector;

  void run_async_slot()
  {
    // Early return if not running
    if (not running) {
      return;
    }

    // Zero slot buffer
    srsran_vec_cf_zero(buffer.data(), sf_len);

    for (std::shared_ptr<gnb_emulator>& gnb : gnb_vector) {
      srsran_assert(gnb->work(slot_idx, buffer.data(), ts_write) == SRSRAN_SUCCESS, "Failed to run gNb emulator");
    }

    // Write slot samples in ringbuffer
    srsran_assert(srsran_ringbuffer_write(&ringbuffer, buffer.data(), (int)sizeof(cf_t) * sf_len) > SRSRAN_SUCCESS,
                  "Error writing in ringbuffer");

    // Increment time
    ts_write.add(0.001f);
  }

public:
  struct args_t {
    double                      srate_hz;
    srsran_carrier_nr_t         base_carrier;
    srsran_subcarrier_spacing_t ssb_scs;
    srsran_ssb_pattern_t        ssb_pattern;
    uint32_t                    ssb_periodicity_ms;
    srsran_duplex_mode_t        duplex_mode;
    std::set<uint32_t>          pci_list;
    float                       channel_hst_fd_hz    = 0.0f;
    float                       channel_hst_period_s = 7.2f;
  };

  gnb_rf_emulator(const args_t& args)
  {
    srsran_assert(
        std::isnormal(args.srate_hz) and args.srate_hz > 0, "Invalid sampling rate (%.2f MHz)", args.srate_hz);

    sf_len = args.srate_hz / 1000;

    for (uint32_t pci : args.pci_list) {
      gnb_emulator::args_t gnb_args = {};
      gnb_args.srate_hz             = args.srate_hz;
      gnb_args.carrier              = args.base_carrier;
      gnb_args.carrier.pci          = pci;
      gnb_args.ssb_scs              = args.ssb_scs;
      gnb_args.ssb_pattern          = args.ssb_pattern;
      gnb_args.ssb_periodicity_ms   = args.ssb_periodicity_ms;
      gnb_args.duplex_mode          = args.duplex_mode;

      gnb_args.channel.hst_enable = std::isnormal(args.channel_hst_fd_hz) and std::isnormal(args.channel_hst_period_s);
      gnb_args.channel.hst_fd_hz  = args.channel_hst_fd_hz;
      gnb_args.channel.hst_period_s = args.channel_hst_period_s;
      gnb_args.channel.enable       = gnb_args.channel.hst_enable;

      gnb_vector.emplace_back(std::make_shared<gnb_emulator>(gnb_args));
    }

    srsran_assert(srsran_ringbuffer_init(&ringbuffer, sizeof(cf_t) * BUFFER_SIZE_SF * sf_len) >= SRSRAN_SUCCESS,
                  "Ringbuffer initialisation failed");

    buffer.resize(BUFFER_SIZE_SF * sf_len);
  }
  ~gnb_rf_emulator() = default;
  void tx_end() override {}
  bool tx(srsran::rf_buffer_interface& tx_buffer, const srsran::rf_timestamp_interface& tx_time) override
  {
    return false;
  }
  bool rx_now(srsran::rf_buffer_interface& rx_buffer, srsran::rf_timestamp_interface& rxd_time) override
  {
    int   nbytes      = (int)(sizeof(cf_t) * rx_buffer.get_nof_samples());
    cf_t* temp_buffer = rx_buffer.get(0);

    // If the buffer is invalid, use internal temporal buffer
    if (temp_buffer == nullptr) {
      temp_buffer = buffer.data();
    }

    // As long as there are not enough samples
    while (srsran_ringbuffer_status(&ringbuffer) < nbytes and running) {
      run_async_slot();
    }

    if (not running) {
      return true;
    }

    srsran_assert(srsran_ringbuffer_read(&ringbuffer, temp_buffer, nbytes) >= SRSRAN_SUCCESS,
                  "Error reading from ringbuffer");

    return true;
  }
  void              set_tx_freq(const uint32_t& carrier_idx, const double& freq) override {}
  void              set_rx_freq(const uint32_t& carrier_idx, const double& freq) override {}
  void              release_freq(const uint32_t& carrier_idx) override {}
  void              set_tx_gain(const float& gain) override {}
  void              set_rx_gain_th(const float& gain) override {}
  void              set_rx_gain(const float& gain) override {}
  void              set_tx_srate(const double& srate) override {}
  void              set_rx_srate(const double& srate) override {}
  void              set_channel_rx_offset(uint32_t ch, int32_t offset_samples) override {}
  double            get_freq_offset() override { return 0; }
  float             get_rx_gain() override { return 0; }
  bool              is_continuous_tx() override { return false; }
  bool              get_is_start_of_burst() override { return false; }
  bool              is_init() override { return false; }
  void              reset() override { running = false; }
  srsran_rf_info_t* get_info() override { return nullptr; }
};

#endif // SRSRAN_GNB_RF_EMULATOR_H
