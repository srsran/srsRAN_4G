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

#include <srsran/common/test_common.h>
#include <srsran/common/threads.h>
#include <srsran/common/tsan_options.h>
#include <srsran/interfaces/ue_interfaces.h>
#include <srsran/phy/utils/random.h>
#include <srsran/srslog/srslog.h>
#include <srsue/hdr/phy/phy.h>

#define CALLBACK(NAME, ...)                                                                                            \
private:                                                                                                               \
  bool received_##NAME = false;                                                                                        \
                                                                                                                       \
public:                                                                                                                \
  bool wait_##NAME(uint32_t timeout_ms, bool reset_flag = false)                                                       \
  {                                                                                                                    \
    std::unique_lock<std::mutex>          lock(mutex);                                                                 \
    std::chrono::system_clock::time_point expire_time = std::chrono::system_clock::now();                              \
    expire_time += std::chrono::milliseconds(timeout_ms);                                                              \
    bool expired = false;                                                                                              \
    if (reset_flag) {                                                                                                  \
      received_##NAME = false;                                                                                         \
    }                                                                                                                  \
    while (!received_##NAME && !expired) {                                                                             \
      expired = (cvar.wait_until(lock, expire_time) == std::cv_status::timeout);                                       \
    }                                                                                                                  \
    if (expired) {                                                                                                     \
      logger.debug("Expired " #NAME " waiting");                                                                       \
    }                                                                                                                  \
    return received_##NAME;                                                                                            \
  }                                                                                                                    \
                                                                                                                       \
  bool get_received_##NAME() { return received_##NAME; }                                                               \
                                                                                                                       \
private:                                                                                                               \
  void notify_##NAME(__VA_ARGS__)                                                                                      \
  {                                                                                                                    \
    std::unique_lock<std::mutex> lock(mutex);                                                                          \
    cvar.notify_all();                                                                                                 \
    logger.debug(#NAME " received");                                                                                   \
    received_##NAME = true;                                                                                            \
  }

class phy_test_bench : public srsran::thread
{
private:
  // Dummy classes
  class dummy_stack final : public srsue::stack_interface_phy_lte
  {
  private:
    srslog::basic_logger&   logger;
    uint16_t                rnti = 0x3c;
    std::mutex              mutex;
    std::condition_variable cvar;

    CALLBACK(in_sync)
    CALLBACK(out_of_sync)
    CALLBACK(new_phy_meas)
    CALLBACK(new_grant_ul)
    CALLBACK(new_grant_dl)
    CALLBACK(run_tti)
    CALLBACK(cell_search)
    CALLBACK(cell_select)
    CALLBACK(config)
    CALLBACK(scell)

  public:
    // Local test access methods
    dummy_stack() : logger(srslog::fetch_basic_logger("stack", false)) {}
    void set_rnti(uint16_t rnti_) { rnti = rnti_; }
    void set_loglevel(std::string& str) { logger.set_level(srslog::str_to_basic_level(str)); }

    void in_sync() override { notify_in_sync(); }
    void out_of_sync() override { notify_out_of_sync(); }
    void new_cell_meas(const std::vector<srsue::phy_meas_t>& meas) override
    {
      for (auto& m : meas) {
        notify_new_phy_meas();
        logger.info("New measurement earfcn=%d; pci=%d; rsrp=%+.1fdBm; rsrq=%+.1fdB;", m.earfcn, m.pci, m.rsrp, m.rsrq);
      }
    }
    uint16_t get_dl_sched_rnti(uint32_t tti) override { return rnti; }
    uint16_t get_ul_sched_rnti(uint32_t tti) override { return rnti; }
    void new_grant_ul(uint32_t cc_idx, mac_grant_ul_t grant, tb_action_ul_t* action) override { notify_new_grant_ul(); }
    void new_grant_dl(uint32_t cc_idx, mac_grant_dl_t grant, tb_action_dl_t* action) override
    {
      notify_new_grant_dl();
      for (auto& i : action->tb) {
        i.enabled = true;
      }
    }
    void tb_decoded(uint32_t cc_idx, mac_grant_dl_t grant, bool* ack) override {}
    void bch_decoded_ok(uint32_t cc_idx, uint8_t* payload, uint32_t len) override {}
    void mch_decoded(uint32_t len, bool crc, uint8_t* payload) override {}
    void set_mbsfn_config(uint32_t nof_mbsfn_services) override {}
    void run_tti(const uint32_t tti, const uint32_t tti_jump) override
    {
      notify_run_tti();
      logger.debug("Run TTI %d", tti);
    }

    void cell_search_complete(cell_search_ret_t ret, srsue::phy_cell_t found_cell) override
    {
      cell_search_ret = ret;
      last_found_cell = found_cell;
      notify_cell_search();
    }
    void cell_select_complete(bool status) override
    {
      notify_cell_select();
      last_cell_select = status;
    }
    void set_config_complete(bool status) override
    {
      notify_config();
      last_config = status;
    }
    void set_scell_complete(bool status) override
    {
      notify_scell();
      last_scell = status;
    }

    cell_search_ret_t cell_search_ret  = {};
    srsue::phy_cell_t last_found_cell  = {};
    bool              last_cell_select = false, last_config = false, last_scell = false;
  };

  class dummy_radio : public srsran::radio_interface_phy
  {
  private:
    srslog::basic_logger&            logger;
    std::vector<srsran_ringbuffer_t> ring_buffers;
    float                            base_srate   = 0.0f;
    float                            tx_srate     = 0.0f;
    float                            rx_srate     = 0.0f;
    float                            rx_gain      = 0.0f;
    float                            tx_freq      = 0.0f;
    float                            rx_freq      = 0.0f;
    cf_t*                            temp_buffer  = nullptr;
    uint64_t                         rx_timestamp = 0;
    std::mutex                       mutex;
    std::condition_variable          cvar;
    srsran_rf_info_t                 rf_info    = {};
    srsran::rf_timestamp_t           tx_last_tx = {};
    uint32_t                         count_late = 0;

    CALLBACK(rx_now)
    CALLBACK(tx)
    CALLBACK(late)

  public:
    dummy_radio(uint32_t nof_channels, float base_srate_) :
      logger(srslog::fetch_basic_logger("radio", false)), ring_buffers(nof_channels), base_srate(base_srate_)
    {
      // Create Ring buffers
      for (auto& rb : ring_buffers) {
        if (srsran_ringbuffer_init(&rb, (uint32_t)sizeof(cf_t) * SRSRAN_SF_LEN_MAX * SRSRAN_NOF_SF_X_FRAME)) {
          perror("init softbuffer");
        }
      }

      // Create temporal buffer
      temp_buffer = srsran_vec_cf_malloc(SRSRAN_SF_LEN_MAX * SRSRAN_NOF_SF_X_FRAME);
      if (!temp_buffer) {
        perror("malloc");
      }

      // Set RF Info (in dB)
      rf_info.min_rx_gain = 0.0f;
      rf_info.max_rx_gain = 90.0f;
      rf_info.min_tx_gain = 0.0f;
      rf_info.max_tx_gain = 90.0f;
    }

    ~dummy_radio()
    {
      for (auto& rb : ring_buffers) {
        srsran_ringbuffer_free(&rb);
      }
      if (temp_buffer) {
        free(temp_buffer);
      }
    }

    void set_loglevel(std::string& str) { logger.set_level(srslog::str_to_basic_level(str)); }

    void write_ring_buffers(cf_t** buffer, uint32_t nsamples)
    {
      for (uint32_t i = 0; i < ring_buffers.size(); i++) {
        int ret = SRSRAN_SUCCESS;
        do {
          if (ret != SRSRAN_SUCCESS) {
            logger.error("Ring buffer write failed (full). Trying again.");
          }
          ret = srsran_ringbuffer_write_timed(&ring_buffers[i], buffer[i], (uint32_t)sizeof(cf_t) * nsamples, 1000);
        } while (ret == SRSRAN_ERROR_TIMEOUT);
      }
    }

    uint32_t get_count_late() { return count_late; }

    bool tx(srsran::rf_buffer_interface& buffer, const srsran::rf_timestamp_interface& tx_time) override
    {
      bool ret = true;
      notify_tx();
      std::lock_guard<std::mutex> lock(mutex);

      if (!std::isnormal(tx_srate)) {
        count_late++;
      }
      if (srsran_timestamp_compare(&tx_time.get(0), tx_last_tx.get_ptr(0)) < 0) {
        ret = false;
      }

      tx_last_tx.copy(tx_time);
      if (std::isnormal(tx_srate)) {
        tx_last_tx.add((double)buffer.get_nof_samples() / (double)tx_srate);
      }

      return ret;
    }
    void release_freq(const uint32_t& carrier_idx) override{};
    void tx_end() override {}
    bool rx_now(srsran::rf_buffer_interface& buffer, srsran::rf_timestamp_interface& rxd_time) override
    {
      notify_rx_now();

      std::lock_guard<std::mutex> lock(mutex);
      uint32_t base_nsamples = (uint32_t)floorf(((float)buffer.get_nof_samples() * base_srate) / rx_srate);

      for (uint32_t i = 0; i < ring_buffers.size(); i++) {
        cf_t* buf_ptr = ((buffer.get(i) != nullptr) && (base_srate == rx_srate)) ? buffer.get(i) : temp_buffer;

        // Read base srate samples
        int ret = srsran_ringbuffer_read(&ring_buffers[i], buf_ptr, (uint32_t)sizeof(cf_t) * base_nsamples);
        if (ret < 0) {
          logger.error("Reading ring buffer");
        } else {
          logger.debug("-- %d samples read from ring buffer", base_nsamples);
        }

        // Only if baseband buffer is provided
        if (buffer.get(i)) {
          if (base_srate > rx_srate) {
            // Decimate
            auto decimation = (uint32_t)roundf(base_srate / rx_srate);

            // Perform decimation
            for (uint32_t j = 0, k = 0; j < buffer.get_nof_samples(); j++, k += decimation) {
              buffer.get(i)[j] = buf_ptr[k];
            }
          } else if (base_srate < rx_srate) {
            // Interpolate
            auto interpolation = (uint32_t)roundf(rx_srate / base_srate);

            // Perform zero order hold interpolation
            for (uint32_t j = 0, k = 0; j < buffer.get_nof_samples(); k++) {
              for (uint32_t c = 0; c < interpolation; c++, j++) {
                buffer.get(i)[j] = buf_ptr[k];
              }
            }
          }
        }
      }

      // Set Rx timestamp
      srsran_timestamp_init_uint64(rxd_time.get_ptr(0), rx_timestamp, (double)base_srate);

      // Update timestamp
      rx_timestamp += base_nsamples;

      return true;
    }
    void set_tx_freq(const uint32_t& channel_idx, const double& freq) override
    {
      std::unique_lock<std::mutex> lock(mutex);
      tx_freq = (float)freq;
      logger.info("Set Tx freq to %+.0f MHz.", freq * 1.0e-6);
    }
    void set_rx_freq(const uint32_t& channel_idx, const double& freq) override
    {
      std::unique_lock<std::mutex> lock(mutex);
      rx_freq = (float)freq;
      logger.info("Set Rx freq to %+.0f MHz.", freq * 1.0e-6);
    }
    void set_rx_gain_th(const float& gain) override
    {
      std::unique_lock<std::mutex> lock(mutex);
      rx_gain = srsran_convert_dB_to_amplitude(gain);
      logger.info("Set Rx gain-th to %+.1f dB (%.6f).", gain, rx_gain);
    }
    void set_tx_gain(const float& gain) override
    {
      std::unique_lock<std::mutex> lock(mutex);
      rx_gain = srsran_convert_dB_to_amplitude(gain);
      logger.info("Set Tx gain to %+.1f dB (%.6f).", gain, rx_gain);
    }
    void set_rx_gain(const float& gain) override
    {
      std::unique_lock<std::mutex> lock(mutex);
      rx_gain = srsran_convert_dB_to_amplitude(gain);
      logger.info("Set Rx gain to %+.1f dB (%.6f).", gain, rx_gain);
    }
    void set_tx_srate(const double& srate) override
    {
      std::unique_lock<std::mutex> lock(mutex);
      tx_srate = (float)srate;
      logger.info("Set Tx sampling rate to %+.3f MHz.", srate * 1.0e-6);
    }
    void set_rx_srate(const double& srate) override
    {
      std::unique_lock<std::mutex> lock(mutex);
      rx_srate = (float)srate;
      logger.info("Set Rx sampling rate to %+.3f MHz.", srate * 1.0e-6);
    }
    void  set_channel_rx_offset(uint32_t ch, int32_t offset_samples) override{};
    float get_rx_gain() override
    {
      std::unique_lock<std::mutex> lock(mutex);
      return srsran_convert_amplitude_to_dB(rx_gain);
    }
    double            get_freq_offset() override { return 0; }
    bool              is_continuous_tx() override { return false; }
    bool              get_is_start_of_burst() override { return false; }
    bool              is_init() override { return false; }
    void              reset() override {}
    srsran_rf_info_t* get_info() override { return &rf_info; }
  };

  // Common instances
  srslog::basic_logger& logger;

  // Dummy instances
  dummy_stack stack;
  dummy_radio radio;

  // Phy Instances
  std::unique_ptr<srsue::phy> phy;

  // eNb
  srsran_enb_dl_t    enb_dl                          = {};
  cf_t*              enb_dl_buffer[SRSRAN_MAX_PORTS] = {};
  srsran_dl_sf_cfg_t dl_sf_cfg                       = {};
  uint64_t           sfn                             = 0; // System Frame Number
  uint32_t           sf_len                          = 0;

  // Control atributes
  bool                    running = false;
  std::mutex              mutex;
  std::condition_variable cvar;

public:
  phy_test_bench(const srsue::phy_args_t& phy_args, const srsran_cell_t& cell) :
    radio(cell.nof_ports, srsran_sampling_freq_hz(cell.nof_prb)),
    thread("phy_test_bench"),
    logger(srslog::fetch_basic_logger("test bench", false))
  {
    // Deduce physical attributes
    sf_len = SRSRAN_SF_LEN_PRB(cell.nof_prb);

    // Initialise UE
    phy = std::unique_ptr<srsue::phy>(new srsue::phy);
    phy->init(phy_args, &stack, &radio);

    // Wait PHY init to end
    phy->wait_initialize();

    // Initialise DL baseband buffers
    for (uint32_t i = 0; i < cell.nof_ports; i++) {
      enb_dl_buffer[i] = srsran_vec_cf_malloc(sf_len);
      if (!enb_dl_buffer[i]) {
        perror("malloc");
      }
    }

    // Initialise eNb DL
    srsran_enb_dl_init(&enb_dl, enb_dl_buffer, SRSRAN_MAX_PRB);
    srsran_enb_dl_set_cell(&enb_dl, cell);
  }

  ~phy_test_bench()
  {
    // Free eNb DL object
    srsran_enb_dl_free(&enb_dl);

    // Free buffers
    for (auto& buf : enb_dl_buffer) {
      if (buf) {
        free(buf);
        buf = nullptr;
      }
    }
  }

  dummy_stack*                  get_stack() { return &stack; }
  dummy_radio*                  get_radio() { return &radio; }
  srsue::phy_interface_rrc_lte* get_phy_interface_rrc() { return phy.get(); }
  srsue::phy_interface_mac_lte* get_phy_interface_mac() { return phy.get(); }

  void configure_dedicated(uint16_t rnti, srsran::phy_cfg_t& phy_cfg)
  {
    // Set PHY configuration
    phy->set_config(phy_cfg, 0);
  }

  void run_thread() override
  {
    bool _running;

    // Free run DL
    do {
      logger.debug("-- generating DL baseband SFN=%" PRId64 " TTI=%d;", sfn, dl_sf_cfg.tti);

      // Create empty resource grid with basic signals
      srsran_enb_dl_put_base(&enb_dl, &dl_sf_cfg);

      // Generate signal and transmit
      srsran_enb_dl_gen_signal(&enb_dl);

      // Write baseband to radio
      radio.write_ring_buffers(enb_dl_buffer, sf_len);

      // Increase TTI
      dl_sf_cfg.tti++;

      // Increase System Frame number
      if (dl_sf_cfg.tti >= 10240U) {
        dl_sf_cfg.tti = 0;
        sfn++;
      }

      // Update local running state
      mutex.lock();
      _running = running;
      mutex.unlock();
    } while (_running);

    // Stop PHY now!
    phy->stop();

    // Finish thread
  }

  void start()
  {
    std::lock_guard<std::mutex> lock(mutex);
    running = true;
    thread::start();
  }

  void stop()
  {
    cvar.notify_all();
    mutex.lock();
    running = false;
    mutex.unlock();
    wait_thread_finish();
  }

  void set_loglevel(std::string str)
  {
    logger.set_level(srslog::str_to_basic_level(str));
    radio.set_loglevel(str);
    stack.set_loglevel(str);
  }
};

int main(int argc, char** argv)
{
  int            ret             = SRSRAN_SUCCESS;
  const uint32_t default_timeout = 60000; // 1 minute

  // Define Cell
  srsran_cell_t cell = {.nof_prb         = 6,
                        .nof_ports       = 4,
                        .id              = 1,
                        .cp              = SRSRAN_CP_NORM,
                        .phich_length    = SRSRAN_PHICH_NORM,
                        .phich_resources = SRSRAN_PHICH_R_1,
                        .frame_type      = SRSRAN_FDD};
  // Define PHY arguments
  srsue::phy_args_t phy_args = {};

  // Set custom test cell and arguments here
  phy_args.log.phy_level = "info";

  // Start the log backend.
  srslog::init();

  // Create test bench
  std::unique_ptr<phy_test_bench> phy_test = std::unique_ptr<phy_test_bench>(new phy_test_bench(phy_args, cell));
  phy_test->set_loglevel("info");

  // Start test bench
  phy_test->start();

  // 1. Cell search
  TESTASSERT(phy_test->get_phy_interface_rrc()->cell_search(-1));
  TESTASSERT(phy_test->get_stack()->wait_cell_search(default_timeout));
  TESTASSERT(phy_test->get_stack()->cell_search_ret.found ==
             srsue::rrc_interface_phy_lte::cell_search_ret_t::CELL_FOUND);

  // 2. Cell select
  srsue::phy_cell_t phy_cell = phy_test->get_stack()->last_found_cell;
  TESTASSERT(phy_test->get_phy_interface_rrc()->cell_select(phy_cell));
  TESTASSERT(phy_test->get_stack()->wait_cell_select(default_timeout));
  TESTASSERT(phy_test->get_stack()->wait_in_sync(default_timeout));
  TESTASSERT(phy_test->get_stack()->wait_new_phy_meas(default_timeout));

  // 3. Transmit PRACH
  srsran::phy_cfg_t phy_cfg = {};
  phy_cfg.set_defaults();
  phy_test->get_phy_interface_rrc()->set_config(phy_cfg, 0);
  TESTASSERT(phy_test->get_stack()->wait_config(default_timeout));
  //  phy_test->get_phy_interface_mac()->configure_prach_params();
  phy_test->get_phy_interface_mac()->prach_send(0, -1, 0.0f);
  TESTASSERT(phy_test->get_radio()->wait_tx(default_timeout, false));

  // 4. Configure RNTI with PUCCH and check transmission
  uint16_t rnti = 0x3c;
  phy_cfg       = {};
  phy_cfg.set_defaults();
  phy_cfg.dl_cfg.cqi_report.periodic_mode       = SRSRAN_CQI_MODE_12;
  phy_cfg.dl_cfg.cqi_report.periodic_configured = true;
  phy_cfg.dl_cfg.cqi_report.pmi_idx             = 0;
  phy_cfg.ul_cfg.pucch.n_pucch_2                = 0;
  phy_test->configure_dedicated(rnti, phy_cfg);
  TESTASSERT(phy_test->get_radio()->wait_tx(default_timeout));

  // Wait to finish test
  phy_test->stop();

  // Final test checks...
  TESTASSERT(!phy_test->get_radio()->get_count_late()); // No Late allowed

  return ret;
}
