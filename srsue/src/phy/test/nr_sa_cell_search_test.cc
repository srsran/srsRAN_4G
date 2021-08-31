/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsue/hdr/phy/phy_nr_sa.h"

struct test_args_t {};

class gnb_emulator : public srsran::radio_interface_phy
{
private:
  // srsran_ssb_t ssb = {};

public:
  struct args_t {
    double srate_hz;
  };
  gnb_emulator() {}
  void tx_end() override {}
  bool tx(srsran::rf_buffer_interface& buffer, const srsran::rf_timestamp_interface& tx_time) override { return false; }
  bool rx_now(srsran::rf_buffer_interface& buffer, srsran::rf_timestamp_interface& rxd_time) override { return false; }
  void set_tx_freq(const uint32_t& carrier_idx, const double& freq) override {}
  void set_rx_freq(const uint32_t& carrier_idx, const double& freq) override {}
  void release_freq(const uint32_t& carrier_idx) override {}
  void set_tx_gain(const float& gain) override {}
  void set_rx_gain_th(const float& gain) override {}
  void set_rx_gain(const float& gain) override {}
  void set_tx_srate(const double& srate) override {}
  void set_rx_srate(const double& srate) override {}
  void set_channel_rx_offset(uint32_t ch, int32_t offset_samples) override {}
  double            get_freq_offset() override { return 0; }
  float             get_rx_gain() override { return 0; }
  bool              is_continuous_tx() override { return false; }
  bool              get_is_start_of_burst() override { return false; }
  bool              is_init() override { return false; }
  void              reset() override {}
  srsran_rf_info_t* get_info() override { return nullptr; }
};

int main(int argc, char** argv)
{
  return 0;
}