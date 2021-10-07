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
  const uint32_t        BUFFER_SIZE_SF = 10;
  const std::string     LOGNAME        = "RF";
  uint32_t              sf_len         = 0;
  srsran_ssb_t          ssb            = {};
  srsran_ringbuffer_t   ringbuffer     = {};
  cf_t*                 buffer         = nullptr;
  srslog::basic_logger& logger;

public:
  struct args_t {
    double                      srate_hz;
    srsran_carrier_nr_t         carrier;
    srsran_subcarrier_spacing_t ssb_scs;
    srsran_ssb_patern_t         ssb_pattern;
    srsran_ssb_patern_t         ssb_periodicity_ms;
    srsran_duplex_config_nr_t   duplex;
  };
  gnb_emulator(const args_t& args) : logger(srslog::fetch_basic_logger(LOGNAME))
  {
    sf_len = args.srate_hz / 1000;

    srsran_ssb_args_t ssb_args = {};
    ssb_args.enable_encode     = true;
    srsran_assert(srsran_ssb_init(&ssb, &ssb_args) == SRSRAN_SUCCESS, "SSB initialisation failed");

    srsran_ssb_cfg_t ssb_cfg = {};
    ssb_cfg.srate_hz         = args.srate_hz;
    ssb_cfg.center_freq_hz   = args.carrier.dl_center_frequency_hz;
    ssb_cfg.ssb_freq_hz      = args.carrier.ssb_center_freq_hz;
    ssb_cfg.scs              = args.ssb_scs;
    ssb_cfg.pattern          = args.ssb_pattern;
    ssb_cfg.duplex_mode      = args.duplex.mode;
    ssb_cfg.periodicity_ms   = args.ssb_periodicity_ms;
    srsran_assert(srsran_ssb_set_cfg(&ssb, &ssb_cfg) == SRSRAN_SUCCESS, "SSB set config failed");

    srsran_assert(srsran_ringbuffer_init(&ringbuffer, sizeof(cf_t) * BUFFER_SIZE_SF * sf_len),
                  "Ringbuffer initialisation failed");

    buffer = srsran_vec_cf_malloc(sf_len);
  }
  ~gnb_emulator()
  {
    srsran_ssb_free(&ssb);
    srsran_ringbuffer_free(&ringbuffer);
  }
  void tx_end() override {}
  bool tx(srsran::rf_buffer_interface& buffer, const srsran::rf_timestamp_interface& tx_time) override { return false; }
  bool rx_now(srsran::rf_buffer_interface& buffer, srsran::rf_timestamp_interface& rxd_time) override
  {
    uint32_t nbytes = sizeof(cf_t) * buffer.get_nof_samples();

    // As long as there are not enough samples
    while (srsran_ringbuffer_status(&ringbuffer) < nbytes) {
      if (srsran_ringbuffer_write(&ringbuffer, ) < SRSRAN_SUCCESS) {
        logger return false;
      }
    }

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
  void              reset() override {}
  srsran_rf_info_t* get_info() override { return nullptr; }
};

int main(int argc, char** argv)
{
  return 0;
}