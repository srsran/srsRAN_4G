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
#include <mutex>
#include <srsenb/hdr/phy/phy.h>
#include <srslte/common/test_common.h>
#include <srslte/common/threads.h>
#include <srslte/common/tti_sync_cv.h>
#include <srslte/phy/phch/pusch_cfg.h>
#include <srslte/phy/utils/random.h>
#include <srslte/srslte.h>

#define CALLBACK(NAME)                                                                                                 \
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
      log_h.warning("Expired " #NAME " waiting\n");                                                                    \
    }                                                                                                                  \
    return received_##NAME;                                                                                            \
  }                                                                                                                    \
                                                                                                                       \
  bool get_received_##NAME() { return received_##NAME; }                                                               \
                                                                                                                       \
private:                                                                                                               \
  void notify_##NAME()                                                                                                 \
  {                                                                                                                    \
    std::unique_lock<std::mutex> lock(mutex);                                                                          \
    cvar.notify_all();                                                                                                 \
    log_h.info(#NAME " received\n");                                                                                   \
    received_##NAME = true;                                                                                            \
  }

class dummy_radio : public srslte::radio_interface_phy
{
private:
  std::mutex                        mutex;
  std::condition_variable           cvar;
  srslte::log_filter                log_h;
  std::vector<srslte_ringbuffer_t*> ringbuffers_tx;
  std::vector<srslte_ringbuffer_t*> ringbuffers_rx;
  srslte_timestamp_t                ts_tx    = {};
  srslte_timestamp_t                ts_rx    = {};
  double                            rx_srate = 0.0;

  CALLBACK(tx);
  CALLBACK(tx_end);
  CALLBACK(rx_now);
  CALLBACK(set_tx_freq);
  CALLBACK(set_rx_freq);
  CALLBACK(set_rx_gain_th);
  CALLBACK(set_rx_gain);
  CALLBACK(set_tx_gain);
  CALLBACK(set_tx_srate);
  CALLBACK(set_rx_srate);
  CALLBACK(get_rx_gain);
  CALLBACK(get_freq_offset);
  CALLBACK(get_tx_freq);
  CALLBACK(get_rx_freq);
  CALLBACK(get_max_tx_power);
  CALLBACK(get_tx_gain_offset);
  CALLBACK(get_rx_gain_offset);
  CALLBACK(is_continuous_tx);
  CALLBACK(get_is_start_of_burst);
  CALLBACK(is_init);
  CALLBACK(reset);
  CALLBACK(get_info);

public:
  dummy_radio(uint32_t nof_channels) : log_h("RADIO")
  {
    log_h.set_level("info");

    // Allocate receive ring buffer
    for (uint32_t i = 0; i < nof_channels; i++) {
      srslte_ringbuffer_t* rb = (srslte_ringbuffer_t*)srslte_vec_malloc(sizeof(srslte_ringbuffer_t));
      if (!rb) {
        ERROR("Allocating ring buffer\n");
      }

      if (srslte_ringbuffer_init(rb, SRSLTE_SF_LEN_MAX * (uint32_t)sizeof(cf_t))) {
        ERROR("Initiating ring buffer\n");
      }

      ringbuffers_tx.push_back(rb);
    }

    // Allocate transmit ring buffer
    for (uint32_t i = 0; i < nof_channels; i++) {
      srslte_ringbuffer_t* rb = (srslte_ringbuffer_t*)srslte_vec_malloc(sizeof(srslte_ringbuffer_t));
      if (!rb) {
        ERROR("Allocating ring buffer\n");
      }

      if (srslte_ringbuffer_init(rb, SRSLTE_SF_LEN_MAX * (uint32_t)sizeof(cf_t))) {
        ERROR("Initiating ring buffer\n");
      }

      ringbuffers_rx.push_back(rb);
    }
  }

  ~dummy_radio()
  {
    for (auto& rb : ringbuffers_tx) {
      if (rb) {
        srslte_ringbuffer_free(rb);
        free(rb);
      }
    }
    for (auto& rb : ringbuffers_rx) {
      if (rb) {
        srslte_ringbuffer_free(rb);
        free(rb);
      }
    }
  }

  void read_tx(std::vector<cf_t*>& buffers, uint32_t nof_samples)
  {
    uint32_t nbytes = static_cast<uint32_t>(sizeof(cf_t)) * nof_samples;

    log_h.debug("read_tx %d\n", nof_samples);

    for (uint32_t i = 0; i < ringbuffers_tx.size() && i < buffers.size(); i++) {
      srslte_ringbuffer_read(ringbuffers_tx[i], buffers[i], nbytes);
    }
  }

  void write_rx(std::vector<cf_t*>& buffers, uint32_t nof_samples)
  {
    uint32_t nbytes = static_cast<uint32_t>(sizeof(cf_t)) * nof_samples;

    log_h.debug("write_rx %d\n", nof_samples);

    for (uint32_t i = 0; i < ringbuffers_rx.size() && i < buffers.size(); i++) {
      srslte_ringbuffer_write(ringbuffers_rx[i], buffers[i], nbytes);
    }
  }

  bool
  tx(const uint32_t& radio_idx, cf_t** buffer, const uint32_t& nof_samples, const srslte_timestamp_t& tx_time) override
  {
    int err = SRSLTE_SUCCESS;

    // Get number of bytes to write
    uint32_t nbytes = static_cast<uint32_t>(sizeof(cf_t)) * nof_samples;

    log_h.debug("tx %d\n", nof_samples);

    // Write ring buffer
    for (uint32_t i = 0; i < ringbuffers_tx.size() && err >= SRSLTE_SUCCESS; i++) {
      err = srslte_ringbuffer_write(ringbuffers_tx[i], buffer[i], nbytes);
    }

    // Copy new timestamp
    ts_tx = tx_time;

    // Notify call
    notify_tx();

    // Return True if err >= SRSLTE_SUCCESS
    return err >= SRSLTE_SUCCESS;
  }
  void tx_end() override {}
  bool
  rx_now(const uint32_t& radio_idx, cf_t** buffer, const uint32_t& nof_samples, srslte_timestamp_t* rxd_time) override
  {
    int err = SRSLTE_SUCCESS;

    log_h.info("rx_now %d\n", nof_samples);

    // Get number of bytes to read
    uint32_t nbytes = static_cast<uint32_t>(sizeof(cf_t)) * nof_samples;

    // Write ring buffer
    for (uint32_t i = 0; i < ringbuffers_rx.size() && err >= SRSLTE_SUCCESS; i++) {
      err = srslte_ringbuffer_read(ringbuffers_rx[i], buffer[i], nbytes);
    }

    // Copy new timestamp
    if (rxd_time) {
      *rxd_time = ts_rx;
    }

    // Copy new timestamp
    if (std::isnormal(rx_srate)) {
      srslte_timestamp_add(&ts_rx, 0, static_cast<double>(nof_samples) / rx_srate);
    }

    // Notify Rx
    notify_rx_now();

    // Return True if err >= SRSLTE_SUCCESS
    return err >= SRSLTE_SUCCESS;
  }
  void              set_tx_freq(const uint32_t& radio_idx, const uint32_t& channel_idx, const double& freq) override {}
  void              set_rx_freq(const uint32_t& radio_idx, const uint32_t& channel_idx, const double& freq) override {}
  void              set_rx_gain_th(const float& gain) override {}
  void              set_rx_gain(const uint32_t& radio_idx, const float& gain) override {}
  void              set_tx_srate(const uint32_t& radio_idx, const double& srate) override {}
  void              set_rx_srate(const uint32_t& radio_idx, const double& srate) override { rx_srate = srate; }
  float             get_rx_gain(const uint32_t& radio_idx) override { return 0; }
  double            get_freq_offset() override { return 0; }
  double            get_tx_freq(const uint32_t& radio_idx) override { return 0; }
  double            get_rx_freq(const uint32_t& radio_idx) override { return 0; }
  float             get_max_tx_power() override { return 0; }
  float             get_tx_gain_offset() override { return 0; }
  float             get_rx_gain_offset() override { return 0; }
  bool              is_continuous_tx() override { return false; }
  bool              get_is_start_of_burst(const uint32_t& radio_idx) override { return false; }
  bool              is_init() override { return false; }
  void              reset() override {}
  srslte_rf_info_t* get_info(const uint32_t& radio_idx) override { return nullptr; }
};

class dummy_stack : public srsenb::stack_interface_phy_lte
{
private:
  std::mutex              mutex;
  std::condition_variable cvar;
  srslte::log_filter      log_h;
  srslte::tti_sync_cv     tti_sync;
  srslte_softbuffer_tx_t  softbuffer_tx = {};
  srslte_softbuffer_rx_t  softbuffer_rx = {};
  uint8_t*                data          = nullptr;
  uint16_t                ue_rnti       = 0;

  CALLBACK(sr_detected);
  CALLBACK(rach_detected);
  CALLBACK(ri_info);
  CALLBACK(pmi_info);
  CALLBACK(cqi_info);
  CALLBACK(snr_info);
  CALLBACK(ack_info);
  CALLBACK(crc_info);
  CALLBACK(get_dl_sched);
  CALLBACK(get_mch_sched);
  CALLBACK(get_ul_sched);
  CALLBACK(set_sched_dl_tti_mask);
  CALLBACK(rl_failure);
  CALLBACK(rl_ok);
  CALLBACK(tti_clock);

public:
  dummy_stack(uint16_t rnti_) : log_h("STACK"), ue_rnti(rnti_)
  {
    log_h.set_level("info");
    srslte_softbuffer_tx_init(&softbuffer_tx, SRSLTE_MAX_PRB);
    srslte_softbuffer_rx_init(&softbuffer_rx, SRSLTE_MAX_PRB);
    data = srslte_vec_u8_malloc(150000);
  }

  ~dummy_stack()
  {
    srslte_softbuffer_tx_free(&softbuffer_tx);
    srslte_softbuffer_rx_free(&softbuffer_rx);

    if (data) {
      free(data);
    }
  }

  int sr_detected(uint32_t tti, uint16_t rnti) override
  {
    notify_sr_detected();
    return 0;
  }
  int rach_detected(uint32_t tti, uint32_t primary_cc_idx, uint32_t preamble_idx, uint32_t time_adv) override
  {
    notify_rach_detected();
    return 0;
  }
  int ri_info(uint32_t tti, uint16_t rnti, uint32_t ri_value) override
  {
    notify_ri_info();
    return 0;
  }
  int pmi_info(uint32_t tti, uint16_t rnti, uint32_t pmi_value) override
  {
    notify_pmi_info();
    return 0;
  }
  int cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi_value) override
  {
    notify_cqi_info();
    return 0;
  }
  int snr_info(uint32_t tti, uint16_t rnti, float snr_db) override
  {
    notify_snr_info();
    return 0;
  }
  int ack_info(uint32_t tti, uint16_t rnti, uint32_t tb_idx, bool ack) override
  {
    notify_ack_info();
    return 0;
  }
  int crc_info(uint32_t tti, uint16_t rnti, uint32_t nof_bytes, bool crc_res) override
  {
    notify_crc_info();
    return 0;
  }
  int get_dl_sched(uint32_t tti, dl_sched_list_t& dl_sched_res) override
  {
    // Wait for UE
    tti_sync.wait();

    for (auto& dl_sched : dl_sched_res) {
      dl_sched.cfi                                  = 1;
      dl_sched.nof_grants                           = 1;
      dl_sched.pdsch[0].softbuffer_tx[0]            = &softbuffer_tx;
      dl_sched.pdsch[0].softbuffer_tx[1]            = &softbuffer_tx;
      dl_sched.pdsch[0].dci.location.ncce           = 0;
      dl_sched.pdsch[0].dci.location.L              = 1;
      dl_sched.pdsch[0].dci.type0_alloc.rbg_bitmask = 0xffffffff;
      dl_sched.pdsch[0].dci.rnti                    = ue_rnti;
      dl_sched.pdsch[0].dci.alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
      dl_sched.pdsch[0].dci.tb[0].cw_idx            = 0;
      dl_sched.pdsch[0].dci.tb[0].mcs_idx           = 27;
      dl_sched.pdsch[0].dci.tb[0].rv                = 0;
      dl_sched.pdsch[0].dci.tb[0].ndi               = 0;
      dl_sched.pdsch[0].data[0]                     = data;
      dl_sched.pdsch[0].data[1]                     = data;
      dl_sched.pdsch[0].dci.format                  = SRSLTE_DCI_FORMAT1;
    }

    // Notify test engine
    notify_get_dl_sched();

    return 0;
  }
  int get_mch_sched(uint32_t tti, bool is_mcch, dl_sched_list_t& dl_sched_res) override
  {
    notify_get_mch_sched();
    return 0;
  }
  int get_ul_sched(uint32_t tti, ul_sched_list_t& ul_sched_res) override
  {
    notify_get_ul_sched();
    return 0;
  }
  void set_sched_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs) override { notify_set_sched_dl_tti_mask(); }
  void rl_failure(uint16_t rnti) override { notify_rl_failure(); }
  void rl_ok(uint16_t rnti) override { notify_rl_ok(); }
  void tti_clock() override
  {
    notify_tti_clock();
    tti_sync.increase();
  }
};

class dummy_ue
{
private:
  std::vector<srslte_ue_dl_t*> ue_dl_v       = {};
  std::vector<srslte_ue_ul_t*> ue_ul_v       = {};
  std::vector<cf_t*>           buffers       = {};
  dummy_radio*                 radio         = nullptr;
  uint32_t                     sf_len        = 0;
  uint16_t                     rnti          = 0;
  srslte_dl_sf_cfg_t           sf_dl_cfg     = {};
  srslte_ul_sf_cfg_t           sf_ul_cfg     = {};
  srslte_softbuffer_tx_t       softbuffer_tx = {};
  uint8_t*                     tx_data       = nullptr;
  srslte::phy_cfg_t            dedicated     = {};
  srslte::log_filter           log_h;

public:
  dummy_ue(dummy_radio&                       _radio,
           const srsenb::phy_cell_cfg_list_t& cell_list,
           uint16_t                           rnti_,
           const srslte::phy_cfg_t&           dedicated_) :
    radio(&_radio),
    log_h("UE PHY"),
    dedicated(dedicated_)
  {
    // Calculate subframe length
    sf_len = SRSLTE_SF_LEN_PRB(cell_list[0].cell.nof_prb);
    rnti   = rnti_;

    log_h.set_level("info");

    // Initialise each cell
    for (auto& cell : cell_list) {
      // Allocate buffers
      cf_t* buffer = srslte_vec_cf_malloc(sf_len);
      if (!buffer) {
        ERROR("Allocatin UE DL buffer\n");
      }
      buffers.push_back(buffer);

      // Set buffer to zero
      srslte_vec_cf_zero(buffer, sf_len);

      // Allocate UE DL
      auto* ue_dl = (srslte_ue_dl_t*)srslte_vec_malloc(sizeof(srslte_ue_dl_t));
      if (!ue_dl) {
        ERROR("Allocatin UE DL\n");
      }
      ue_dl_v.push_back(ue_dl);

      // Initialise UE DL
      if (srslte_ue_dl_init(ue_dl, &buffer, cell.cell.nof_prb, 1)) {
        ERROR("Initiating UE DL\n");
      }

      // Set Cell
      if (srslte_ue_dl_set_cell(ue_dl, cell.cell)) {
        ERROR("Setting UE DL cell\n");
      }

      // Set RNTI
      srslte_ue_dl_set_rnti(ue_dl, rnti);

      // Allocate UE UL
      auto* ue_ul = (srslte_ue_ul_t*)srslte_vec_malloc(sizeof(srslte_ue_ul_t));
      if (!ue_ul) {
        ERROR("Allocatin UE UL\n");
      }
      ue_ul_v.push_back(ue_ul);

      // Initialise UE UL
      if (srslte_ue_ul_init(ue_ul, buffer, cell.cell.nof_prb)) {
        ERROR("Setting UE UL cell\n");
      }

      // Set cell
      if (srslte_ue_ul_set_cell(ue_ul, cell.cell)) {
        ERROR("Setting UE DL cell\n");
      }

      // Set RNTI
      srslte_ue_ul_set_rnti(ue_ul, rnti);
    }

    // Initialise softbuffer
    if (srslte_softbuffer_tx_init(&softbuffer_tx, cell_list[0].cell.nof_prb)) {
      ERROR("Initialising Tx softbuffer\n");
    }

    // Initialise dummy tx data
    tx_data = srslte_vec_u8_malloc(150000);
    if (!tx_data) {
      ERROR("Allocating Tx data\n");
    }
    memset(tx_data, 0, 150000);

    // Push HARQ delay to radio
    for (uint32_t i = 0; i < FDD_HARQ_DELAY_MS; i++) {
      radio->write_rx(buffers, sf_len);
      sf_ul_cfg.tti = (sf_ul_cfg.tti + 1) % 10240; // Advance UL TTI too
    }
  }

  ~dummy_ue()
  {
    for (auto& ue_dl : ue_dl_v) {
      if (ue_dl) {
        srslte_ue_dl_free(ue_dl);
        free(ue_dl);
      }
    }
    for (auto& ue_ul : ue_ul_v) {
      if (ue_ul) {
        srslte_ue_ul_free(ue_ul);
        free(ue_ul);
      }
    }
    for (auto& b : buffers) {
      if (b) {
        free(b);
      }
    }
    srslte_softbuffer_tx_free(&softbuffer_tx);
  }

  int run_tti()
  {
    srslte_dci_ul_t   dci_ul[SRSLTE_MAX_DCI_MSG] = {};
    int               ret                        = SRSLTE_SUCCESS;
    srslte_uci_data_t uci_data                   = {};

    uci_data.cfg = dedicated.ul_cfg.pucch.uci_cfg;

    srslte_pdsch_ack_t pdsch_ack     = {};
    pdsch_ack.ack_nack_feedback_mode = dedicated.ul_cfg.pucch.ack_nack_feedback_mode;
    pdsch_ack.nof_cc                 = (uint32_t)buffers.size();

    // Read DL
    radio->read_tx(buffers, sf_len);

    // Get grants DL/UL, we do not care about Decoding PDSCH
    for (uint32_t i = 0; i < buffers.size(); i++) {
      srslte_dci_dl_t    dci_dl[SRSLTE_MAX_DCI_MSG] = {};
      srslte_ue_dl_cfg_t ue_dl_cfg                  = {};
      ue_dl_cfg.cfg.cqi_report.periodic_configured  = true;
      ue_dl_cfg.cfg.cqi_report.periodic_mode        = SRSLTE_CQI_MODE_12;
      ue_dl_cfg.cfg.cqi_report.pmi_idx              = 16 + i;
      ue_dl_cfg.cfg.pdsch.rnti                      = rnti;

      srslte_ue_dl_decode_fft_estimate(ue_dl_v[i], &sf_dl_cfg, &ue_dl_cfg);

      // Get DL Grants
      int nof_dl_grants = srslte_ue_dl_find_dl_dci(ue_dl_v[i], &sf_dl_cfg, &ue_dl_cfg, rnti, dci_dl);
      TESTASSERT(nof_dl_grants >= SRSLTE_SUCCESS);

      // Generate ACKs
      if (nof_dl_grants) {
        char str[256] = {};
        srslte_dci_dl_info(dci_dl, str, sizeof(str));
        log_h.info("[DL DCI] %s\n", str);

        if (srslte_ue_dl_dci_to_pdsch_grant(ue_dl_v[i], &sf_dl_cfg, &ue_dl_cfg, dci_dl, &ue_dl_cfg.cfg.pdsch.grant)) {
          log_h.error("Converting DCI message to DL dci\n");
          return -1;
        }

        srslte_pdsch_tx_info(&ue_dl_cfg.cfg.pdsch, str, 512);

        log_h.info("[DL PDSCH %d] %s\n", i, str);

        pdsch_ack.cc[i].M                           = 1;
        pdsch_ack.cc[i].m[0].present                = true;
        pdsch_ack.cc[i].m[0].resource.v_dai_dl      = dci_dl->dai;
        pdsch_ack.cc[i].m[0].resource.n_cce         = dci_dl->location.ncce;
        pdsch_ack.cc[i].m[0].resource.grant_cc_idx  = i;
        pdsch_ack.cc[i].m[0].resource.tpc_for_pucch = dci_dl->tpc_pucch;
        pdsch_ack.cc[i].m[0].value[0]               = 1;
        pdsch_ack.cc[i].m[0].value[1]               = 1;
      } else {
        pdsch_ack.cc[i].M            = 1;
        pdsch_ack.cc[i].m[0].present = false;
      }

      // Get UL grants
      int nof_ul_grants = srslte_ue_dl_find_ul_dci(ue_dl_v[i], &sf_dl_cfg, &ue_dl_cfg, rnti, &dci_ul[i]);
      TESTASSERT(nof_ul_grants >= SRSLTE_SUCCESS);

      // Generate CQI periodic if required
      srslte_ue_dl_gen_cqi_periodic(ue_dl_v[i], &ue_dl_cfg, 0x0f, sf_dl_cfg.tti, &uci_data);
    }

    // Generate Acknowledgements
    srslte_ue_dl_gen_ack(ue_dl_v[0], &sf_dl_cfg, &pdsch_ack, &uci_data);

    // Work UL
    for (uint32_t i = 0; i < buffers.size(); i++) {
      srslte_ue_ul_cfg_t ue_ul_cfg          = {};
      ue_ul_cfg.ul_cfg                      = dedicated.ul_cfg;
      ue_ul_cfg.ul_cfg.pusch.softbuffers.tx = &softbuffer_tx;
      ue_ul_cfg.ul_cfg.pucch.rnti           = rnti;

      srslte_pusch_data_t pusch_data = {};
      pusch_data.ptr                 = tx_data;

      // Set UCI only for PCel
      if (i == 0) {
        pusch_data.uci                 = uci_data.value;
        ue_ul_cfg.ul_cfg.pusch.uci_cfg = uci_data.cfg;
        ue_ul_cfg.ul_cfg.pucch.uci_cfg = uci_data.cfg;
      }

      // Reset subframe
      srslte_vec_cf_zero(buffers[i], sf_len);

      // Work UL
      TESTASSERT(srslte_ue_ul_encode(ue_ul_v[i], &sf_ul_cfg, &ue_ul_cfg, &pusch_data) >= SRSLTE_SUCCESS);

      char str[256] = {};
      srslte_ue_ul_info(&ue_ul_cfg, &sf_ul_cfg, &pusch_data.uci, str, sizeof(str));
      log_h.info("[UL INFO] %s\n", str);
    }

    // Write eNb Rx
    radio->write_rx(buffers, sf_len);

    // Increment TTI
    sf_dl_cfg.tti = (sf_dl_cfg.tti + 1) % 10240;
    sf_ul_cfg.tti = (sf_ul_cfg.tti + 1) % 10240;

    return ret;
  }
};

class phy_test_bench
{
private:
  // Private classes
  dummy_radio           radio;
  dummy_stack           stack;
  srsenb::phy           enb_phy;
  dummy_ue              ue_phy;
  srslte::log_filter    log_h;
  srslte::logger_stdout logger_stdout;
  uint32_t              nof_carriers = 0;

public:
  phy_test_bench(srsenb::phy_args_t&      phy_args,
                 srsenb::phy_cfg_t&       phy_cfg,
                 uint16_t                 rnti,
                 const srslte::phy_cfg_t& dedicated) :
    log_h("TEST BENCH"),
    stack(rnti),
    radio(phy_cfg.phy_cell_cfg.size()),
    enb_phy(&logger_stdout),
    ue_phy(radio, phy_cfg.phy_cell_cfg, rnti, dedicated)
  {

    nof_carriers = phy_cfg.phy_cell_cfg.size();

    // Always info
    log_h.set_level("info");

    // Initiate
    enb_phy.init(phy_args, phy_cfg, &radio, &stack);

    // Add rnti to enb
    enb_phy.add_rnti(rnti, false);

    // Configure UE PHY
    enb_phy.set_config_dedicated(rnti, dedicated);
  }

  ~phy_test_bench() { enb_phy.stop(); }

  int run_tti()
  {
    int ret = SRSLTE_SUCCESS;

    stack.tti_clock();

    TESTASSERT(ue_phy.run_tti() >= SRSLTE_SUCCESS);

    return ret;
  }
};

int main(int argc, char** argv)
{
  int                ret = SRSLTE_SUCCESS;
  srsenb::phy_args_t phy_args;

  phy_args.log.phy_level = "info";

  srsenb::phy_cfg_t phy_cfg = {};
  for (uint32_t i = 0; i < 4; i++) {
    srsenb::phy_cell_cfg_t phy_cell_cfg = {};

    phy_cell_cfg.cell.nof_prb   = 6;
    phy_cell_cfg.cell.id        = i;
    phy_cell_cfg.cell.nof_ports = 1;
    phy_cell_cfg.cell_id        = i;
    phy_cell_cfg.dl_freq_hz     = 2.6e9 + 10e6 * i;
    phy_cell_cfg.ul_freq_hz     = 2.6e9 + 10e6 * i - 100e6;
    phy_cell_cfg.rf_port        = i;
    phy_cell_cfg.root_seq_idx   = 150 + i;

    phy_cfg.phy_cell_cfg.push_back(phy_cell_cfg);
  }

  phy_cfg.pucch_cnfg.delta_pucch_shift                = asn1::rrc::pucch_cfg_common_s::delta_pucch_shift_e_::ds1;
  phy_cfg.prach_cnfg.root_seq_idx                     = 0;
  phy_cfg.prach_cnfg.prach_cfg_info.high_speed_flag   = false;
  phy_cfg.prach_cnfg.prach_cfg_info.prach_cfg_idx     = 3;
  phy_cfg.prach_cnfg.prach_cfg_info.prach_freq_offset = 2;
  phy_cfg.prach_cnfg.prach_cfg_info.zero_correlation_zone_cfg = 5;

  // Set UE dedicated configuration
  srslte::phy_cfg_t dedicated                   = {};
  dedicated.ul_cfg.pucch.ack_nack_feedback_mode = SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3;
  dedicated.ul_cfg.pucch.delta_pucch_shift      = 1;
  dedicated.ul_cfg.pucch.n_rb_2                 = 0;
  dedicated.ul_cfg.pucch.N_cs                   = 0;
  dedicated.ul_cfg.pucch.N_pucch_1              = 0;
  dedicated.ul_cfg.pucch.n_pucch_2              = 0;
  dedicated.ul_cfg.pucch.n_pucch_sr             = 0;
  dedicated.ul_cfg.pucch.simul_cqi_ack          = true;

  std::unique_ptr<phy_test_bench> test_bench =
      std::unique_ptr<phy_test_bench>(new phy_test_bench(phy_args, phy_cfg, 0x1234, dedicated));

  for (uint32_t i = 0; i < 32; i++) {
    TESTASSERT(test_bench->run_tti() >= SRSLTE_SUCCESS);
  }

  return ret;
}
