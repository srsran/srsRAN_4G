/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
#include <srslte/srslte.h>
#include <srsue/hdr/phy/phy.h>

class phy_test_bench
{
private:
  // Dummy classes
  class dummy_stack : public srsue::stack_interface_phy_lte
  {
  private:
    srslte::log_filter log_h;
    uint16_t           rnti = 0x3c;

  public:
    // Local test access methods
    explicit dummy_stack(srslte::logger& logger) : log_h("stack", &logger) {}
    void set_rnti(uint16_t rnti_) { rnti = rnti_; }

    // Implemented methods
    void in_sync() override { log_h.info("in-sync received\n"); }
    void out_of_sync() override { log_h.info("out-of-sync received\n"); }
    void new_phy_meas(float rsrp, float rsrq, uint32_t tti, int earfcn, int pci) override
    {
      log_h.info("New measurement earfcn=%d; pci=%d; rsrp=%+.1fdBm; rsrq=%+.1fdB;\n", earfcn, pci, rsrp, rsrq);
    }
    uint16_t get_dl_sched_rnti(uint32_t tti) override { return rnti; }
    uint16_t get_ul_sched_rnti(uint32_t tti) override { return rnti; }
    void     new_grant_ul(uint32_t cc_idx, mac_grant_ul_t grant, tb_action_ul_t* action) override
    {
      action->tb.enabled = true;
    }
    void new_grant_dl(uint32_t cc_idx, mac_grant_dl_t grant, tb_action_dl_t* action) override
    {
      for (auto& i : action->tb) {
        i.enabled = true;
      }
    }
    void tb_decoded(uint32_t cc_idx, mac_grant_dl_t grant, bool* ack) override {}
    void bch_decoded_ok(uint8_t* payload, uint32_t len) override {}
    void mch_decoded(uint32_t len, bool crc) override {}
    void new_mch_dl(srslte_pdsch_grant_t phy_grant, tb_action_dl_t* action) override {}
    void set_mbsfn_config(uint32_t nof_mbsfn_services) override {}
    void run_tti(const uint32_t tti) override { log_h.info("Run TTI %d\n", tti); }
  };

  class dummy_radio : public srslte::radio_interface_phy
  {
  private:
    srslte::log_filter               log_h;
    std::vector<srslte_ringbuffer_t> ring_buffers;
    float                            base_srate   = 0.0f;
    float                            tx_srate     = 0.0f;
    float                            rx_srate     = 0.0f;
    float                            rx_gain      = 0.0f;
    float                            tx_freq      = 0.0f;
    float                            rx_freq      = 0.0f;
    cf_t*                            temp_buffer  = nullptr;
    uint64_t                         rx_timestamp = 0;

  public:
    dummy_radio(srslte::logger& logger, uint32_t nof_channels, float base_srate_) :
      log_h("radio", &logger),
      ring_buffers(nof_channels),
      base_srate(base_srate_)
    {
      // Create Ring buffers
      for (auto& rb : ring_buffers) {
        if (srslte_ringbuffer_init(&rb, (uint32_t)sizeof(cf_t) * SRSLTE_SF_LEN_MAX)) {
          perror("init softbuffer");
        }
      }

      // Create temporal buffer
    }

    void write_ring_buffers(cf_t** buffer, uint32_t nsamples)
    {
      for (uint32_t i = 0; i < ring_buffers.size(); i++) {
        if (srslte_ringbuffer_write_timed(&ring_buffers[i], buffer[i], sizeof(cf_t) * nsamples, 1000) < 0) {
          perror("writing ring buffer");
        }
      }
    }

    bool tx(const uint32_t&           radio_idx,
            cf_t**                    buffer,
            const uint32_t&           nof_samples,
            const srslte_timestamp_t& tx_time) override
    {
      return false;
    }
    void tx_end() override {}
    bool
    rx_now(const uint32_t& radio_idx, cf_t** buffer, const uint32_t& nof_samples, srslte_timestamp_t* rxd_time) override
    {
      if (base_srate > rx_srate) {
        // Decimate
        uint32_t decimation = base_srate / rx_srate;
        for (uint32_t i = 0; i < ring_buffers.size(); i++) {
        }
      }
      return true;
    }
    void set_tx_freq(const uint32_t& radio_idx, const uint32_t& channel_idx, const double& freq) override
    {
      tx_freq = (float)freq;
    }
    void set_rx_freq(const uint32_t& radio_idx, const uint32_t& channel_idx, const double& freq) override
    {
      rx_freq = (float)freq;
    }
    float set_rx_gain_th(const float& gain) override
    {
      rx_gain = srslte_convert_dB_to_amplitude(gain);
      return srslte_convert_amplitude_to_dB(rx_gain);
    }
    void set_rx_gain(const uint32_t& radio_idx, const float& gain) override
    {
      rx_gain = srslte_convert_dB_to_amplitude(gain);
    }
    void   set_tx_srate(const uint32_t& radio_idx, const double& srate) override { tx_srate = (float)srate; }
    void   set_rx_srate(const uint32_t& radio_idx, const double& srate) override { rx_srate = (float)srate; }
    float  get_rx_gain(const uint32_t& radio_idx) override { return srslte_convert_amplitude_to_dB(rx_gain); }
    double get_freq_offset() override { return 0; }
    double get_tx_freq(const uint32_t& radio_idx) override { return tx_freq; }
    double get_rx_freq(const uint32_t& radio_idx) override { return rx_freq; }
    float  get_max_tx_power() override { return 0; }
    float  get_tx_gain_offset() override { return 0; }
    float  get_rx_gain_offset() override { return 0; }
    bool   is_continuous_tx() override { return false; }
    bool   get_is_start_of_burst(const uint32_t& radio_idx) override { return false; }
    bool   is_init() override { return false; }
    void   reset() override {}
    srslte_rf_info_t* get_info(const uint32_t& radio_idx) override { return nullptr; }
  };

  // Common instances
  srslte::logger_stdout main_logger;

  // Dummy instances
  dummy_stack stack;
  dummy_radio radio;

  // Phy Instances
  srsue::phy phy;

  // eNb
  srslte_enb_dl_t    enb_dl                          = {};
  cf_t*              enb_dl_buffer[SRSLTE_MAX_PORTS] = {};
  srslte_dl_sf_cfg_t dl_sf_cfg                       = {};
  uint64_t           sfn                             = 0; // System Frame Number
  uint32_t           sf_len                          = 0;

public:
  explicit phy_test_bench(const srsue::phy_args_t& phy_args, const srslte_cell_t& cell) :
    stack(main_logger),
    radio(main_logger, cell.nof_ports, srslte_sampling_freq_hz(cell.nof_prb)),
    phy(&main_logger)
  {
    // Deduce physical attributes
    sf_len = SRSLTE_SF_LEN_PRB(cell.nof_prb);

    // Initialise UE
    phy.init(phy_args, &stack, &radio);

    // Initialise DL baseband buffers
    for (uint32_t i = 0; i < phy_args.nof_rx_ant; i++) {
      enb_dl_buffer[i] = srslte_vec_cf_malloc(sf_len);
      if (!enb_dl_buffer[i]) {
        perror("malloc");
      }
    }

    // Initialise eNb DL
    srslte_enb_dl_init(&enb_dl, enb_dl_buffer, cell.nof_prb);
    srslte_enb_dl_set_cell(&enb_dl, cell);
  }

  void run_dl_tti()
  {
    // Generate baseband signal
    srslte_enb_dl_put_base(&enb_dl, &dl_sf_cfg);

    // Write baseband to radio
    radio.write_ring_buffers(enb_dl_buffer, sf_len);

    // Increase TTI
    dl_sf_cfg.tti++;

    // Increase System Frame number
    if (dl_sf_cfg.tti >= 10240) {
      dl_sf_cfg.tti = 0;
      sfn++;
    }
  }
};

int main(int argc, char** argv)
{
  int               ret      = SRSLTE_SUCCESS;
  srslte_cell_t     cell     = {.nof_prb         = 6,
                        .nof_ports       = 1,
                        .id              = 1,
                        .cp              = SRSLTE_CP_NORM,
                        .phich_length    = SRSLTE_PHICH_NORM,
                        .phich_resources = SRSLTE_PHICH_R_1,
                        .frame_type      = SRSLTE_FDD};
  srsue::phy_args_t phy_args = {};

  phy_test_bench phy_test(phy_args, cell);

  return ret;
}
