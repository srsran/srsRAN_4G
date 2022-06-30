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

#include "srsran/common/threads.h"
#include "srsran/phy/utils/random.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsran.h"
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>
#include <mutex>
#include <srsenb/hdr/phy/phy.h>
#include <srsran/common/string_helpers.h>
#include <srsran/common/test_common.h>

static inline bool dl_ack_value(uint32_t ue_cc_idx, uint32_t tti)
{
  return (tti % SRSRAN_MAX_CARRIERS) != ue_cc_idx;
}

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
    while (not received_##NAME and not expired) {                                                                      \
      expired = (cvar.wait_until(lock, expire_time) == std::cv_status::timeout);                                       \
    }                                                                                                                  \
    if (expired) {                                                                                                     \
      logger.warning("Expired " #NAME " waiting");                                                                     \
    }                                                                                                                  \
    return received_##NAME;                                                                                            \
  }                                                                                                                    \
                                                                                                                       \
  bool get_received_##NAME() { return received_##NAME; }                                                               \
  void clear_##NAME() { received_##NAME = false; }                                                                     \
                                                                                                                       \
private:                                                                                                               \
  void notify_##NAME()                                                                                                 \
  {                                                                                                                    \
    std::unique_lock<std::mutex> lock(mutex);                                                                          \
    cvar.notify_all();                                                                                                 \
    logger.debug(#NAME " received");                                                                                   \
    received_##NAME = true;                                                                                            \
  }

class dummy_radio final : public srsran::radio_interface_phy
{
private:
  std::mutex                        mutex;
  std::condition_variable           cvar;
  srslog::basic_logger&             logger;
  std::vector<srsran_ringbuffer_t*> ringbuffers_tx;
  std::vector<srsran_ringbuffer_t*> ringbuffers_rx;
  srsran::rf_timestamp_t            ts_rx    = {};
  double                            rx_srate = 0.0;
  std::atomic<bool>                 running  = {true};

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
  explicit dummy_radio(uint32_t nof_channels, uint32_t nof_prb, const std::string& log_level) :
    logger(srslog::fetch_basic_logger("RADIO", false))
  {
    logger.set_level(srslog::str_to_basic_level(log_level));

    // Allocate receive ring buffer
    for (uint32_t i = 0; i < nof_channels; i++) {
      auto* rb = (srsran_ringbuffer_t*)srsran_vec_malloc(sizeof(srsran_ringbuffer_t));
      if (not rb) {
        ERROR("Allocating ring buffer");
      }

      if (srsran_ringbuffer_init(rb, SRSRAN_SF_LEN_PRB(nof_prb) * SRSRAN_NOF_SF_X_FRAME * (uint32_t)sizeof(cf_t))) {
        ERROR("Initiating ring buffer");
      }

      ringbuffers_tx.push_back(rb);
    }

    // Allocate transmit ring buffer
    for (uint32_t i = 0; i < nof_channels; i++) {
      auto* rb = (srsran_ringbuffer_t*)srsran_vec_malloc(sizeof(srsran_ringbuffer_t));
      if (not rb) {
        ERROR("Allocating ring buffer");
      }

      if (srsran_ringbuffer_init(rb, SRSRAN_SF_LEN_PRB(nof_prb) * SRSRAN_NOF_SF_X_FRAME * (uint32_t)sizeof(cf_t))) {
        ERROR("Initiating ring buffer");
      }

      ringbuffers_rx.push_back(rb);
    }
  }

  ~dummy_radio()
  {
    for (auto& rb : ringbuffers_tx) {
      if (rb) {
        srsran_ringbuffer_free(rb);
        free(rb);
      }
    }
    for (auto& rb : ringbuffers_rx) {
      if (rb) {
        srsran_ringbuffer_free(rb);
        free(rb);
      }
    }
  }

  void stop() { running = false; }

  int read_tx(std::vector<cf_t*>& buffers, uint32_t nof_samples)
  {
    int      err    = SRSRAN_SUCCESS;
    uint32_t nbytes = static_cast<uint32_t>(sizeof(cf_t)) * nof_samples;

    logger.debug("read_tx %d", nof_samples);

    for (uint32_t i = 0; i < ringbuffers_tx.size() and i < buffers.size(); i++) {
      do {
        err = srsran_ringbuffer_read_timed(ringbuffers_tx[i], buffers[i], nbytes, 1000);
      } while (err < SRSRAN_SUCCESS and running);
    }

    return err;
  }

  void write_rx(std::vector<cf_t*>& buffers, uint32_t nof_samples)
  {
    uint32_t nbytes = static_cast<uint32_t>(sizeof(cf_t)) * nof_samples;

    logger.debug("write_rx %d", nof_samples);

    for (uint32_t i = 0; i < ringbuffers_rx.size() and i < buffers.size(); i++) {
      srsran_ringbuffer_write(ringbuffers_rx[i], buffers[i], nbytes);
    }
  }

  bool tx(srsran::rf_buffer_interface& buffer, const srsran::rf_timestamp_interface& tx_time) override
  {
    int err = SRSRAN_SUCCESS;

    if (not running) {
      return true;
    }

    // Get number of bytes to write
    uint32_t nbytes = static_cast<uint32_t>(sizeof(cf_t)) * buffer.get_nof_samples();

    logger.debug("tx %d", buffer.get_nof_samples());

    // Write ring buffer
    for (uint32_t i = 0; i < ringbuffers_tx.size() and err >= SRSRAN_SUCCESS; i++) {
      err = srsran_ringbuffer_write(ringbuffers_tx[i], buffer.get(i), nbytes);
    }

    // Notify call
    notify_tx();

    // Return True if err >= SRSRAN_SUCCESS
    return err >= SRSRAN_SUCCESS;
  }
  void tx_end() override {}
  bool rx_now(srsran::rf_buffer_interface& buffer, srsran::rf_timestamp_interface& rxd_time) override
  {
    int err = SRSRAN_SUCCESS;

    if (not running) {
      for (uint32_t i = 0; i < buffer.size(); i++) {
        srsran_vec_cf_zero(buffer.get(i), buffer.get_nof_samples());
      }
      return true;
    }

    logger.info("rx_now %d", buffer.get_nof_samples());

    // Get number of bytes to read
    uint32_t nbytes = static_cast<uint32_t>(sizeof(cf_t)) * buffer.get_nof_samples();

    // Write ring buffer
    for (uint32_t i = 0; i < ringbuffers_rx.size() and err >= SRSRAN_SUCCESS; i++) {
      do {
        err = srsran_ringbuffer_read_timed(ringbuffers_rx[i], buffer.get(i), nbytes, 1000);
      } while (err < SRSRAN_SUCCESS and running);
    }

    // Copy new timestamp
    rxd_time = ts_rx;

    // Copy new timestamp
    if (std::isnormal(rx_srate)) {
      ts_rx.add(static_cast<double>(buffer.get_nof_samples()) / rx_srate);
    }

    // Notify Rx
    notify_rx_now();

    // Return True if err >= SRSRAN_SUCCESS
    return err >= SRSRAN_SUCCESS;
  }
  void              release_freq(const uint32_t& carrier_idx) override{};
  void              set_tx_freq(const uint32_t& channel_idx, const double& freq) override {}
  void              set_rx_freq(const uint32_t& channel_idx, const double& freq) override {}
  void              set_rx_gain_th(const float& gain) override {}
  void              set_rx_gain(const float& gain) override {}
  void              set_tx_srate(const double& srate) override {}
  void              set_rx_srate(const double& srate) override { rx_srate = srate; }
  void              set_channel_rx_offset(uint32_t ch, int32_t offset_samples) override{};
  void              set_tx_gain(const float& gain) override {}
  float             get_rx_gain() override { return 0; }
  double            get_freq_offset() override { return 0; }
  bool              is_continuous_tx() override { return false; }
  bool              get_is_start_of_burst() override { return false; }
  bool              is_init() override { return false; }
  void              reset() override {}
  srsran_rf_info_t* get_info() override { return nullptr; }
};

typedef std::unique_ptr<dummy_radio> unique_dummy_radio_t;

class dummy_stack final : public srsenb::stack_interface_phy_lte
{
private:
  static constexpr float    prob_dl_grant = 0.50f;
  static constexpr float    prob_ul_grant = 0.10f;
  static constexpr uint32_t cfi           = 2;

  srsenb::phy_cell_cfg_list_t                       phy_cell_cfg;
  srsenb::phy_interface_rrc_lte::phy_rrc_cfg_list_t phy_rrc;
  std::mutex                                        mutex;
  std::condition_variable                           cvar;
  srslog::basic_logger&                             logger;
  srsran_softbuffer_tx_t                            softbuffer_tx                                           = {};
  srsran_softbuffer_rx_t                            softbuffer_rx[SRSRAN_MAX_CARRIERS][SRSRAN_FDD_NOF_HARQ] = {};
  uint8_t*                                          data                                                    = nullptr;
  uint16_t                                          ue_rnti                                                 = 0;
  srsran_random_t                                   random_gen                                              = nullptr;

  CALLBACK(sr_detected);
  CALLBACK(rach_detected);
  CALLBACK(ri_info);
  CALLBACK(pmi_info);
  CALLBACK(cqi_info);
  CALLBACK(sb_cqi_info);
  CALLBACK(snr_info);
  CALLBACK(ta_info);
  CALLBACK(ack_info);
  CALLBACK(crc_info);
  CALLBACK(push_pdu);
  CALLBACK(get_dl_sched);
  CALLBACK(get_mch_sched);
  CALLBACK(get_ul_sched);
  CALLBACK(set_sched_dl_tti_mask);
  CALLBACK(tti_clock);

  typedef struct {
    uint32_t tti;
    uint32_t cc_idx;
    uint32_t tb_idx;
    bool     ack;
  } tti_dl_info_t;

  typedef struct {
    uint32_t tti;
    uint32_t cc_idx;
    bool     crc;
  } tti_ul_info_t;

  typedef struct {
    uint32_t tti;
  } tti_sr_info_t;

  typedef struct {
    uint32_t tti;
    uint32_t cc_idx;
    uint32_t cqi;
  } tti_cqi_info_t;

  std::mutex                 phy_mac_mutex;
  std::queue<tti_dl_info_t>  tti_dl_info_sched_queue;
  std::queue<tti_dl_info_t>  tti_dl_info_ack_queue;
  std::queue<tti_ul_info_t>  tti_ul_info_sched_queue;
  std::queue<tti_ul_info_t>  tti_ul_info_ack_queue;
  std::queue<tti_sr_info_t>  tti_sr_info_queue;
  std::queue<tti_cqi_info_t> tti_cqi_info_queue;
  std::vector<uint32_t>      active_cell_list;

  uint32_t              nof_locations[SRSRAN_NOF_SF_X_FRAME]                           = {};
  srsran_dci_location_t dci_locations[SRSRAN_NOF_SF_X_FRAME][SRSRAN_MAX_CANDIDATES_UE] = {};
  uint32_t              ul_riv                                                         = 0;

public:
  explicit dummy_stack(const srsenb::phy_cfg_t&                                 phy_cfg_,
                       const srsenb::phy_interface_rrc_lte::phy_rrc_cfg_list_t& phy_rrc_,
                       const std::string&                                       log_level,
                       uint16_t                                                 rnti_) :
    logger(srslog::fetch_basic_logger("STACK", false)),
    ue_rnti(rnti_),
    random_gen(srsran_random_init(rnti_)),
    phy_cell_cfg(phy_cfg_.phy_cell_cfg),
    phy_rrc(phy_rrc_)
  {
    logger.set_level(srslog::str_to_basic_level(log_level));
    srsran_softbuffer_tx_init(&softbuffer_tx, SRSRAN_MAX_PRB);
    for (uint32_t i = 0; i < phy_rrc.size(); i++) {
      for (auto& sb : softbuffer_rx[i]) {
        srsran_softbuffer_rx_init(&sb, SRSRAN_MAX_PRB);
      }
    }

    srsran_pdcch_t pdcch = {};
    srsran_regs_t  regs  = {};
    srsran_regs_init(&regs, phy_cell_cfg[0].cell);
    srsran_pdcch_init_enb(&pdcch, phy_cell_cfg[0].cell.nof_prb);
    srsran_pdcch_set_cell(&pdcch, &regs, phy_cell_cfg[0].cell);
    for (uint32_t i = 0; i < SRSRAN_NOF_SF_X_FRAME; i++) {
      srsran_dl_sf_cfg_t sf_cfg_dl;
      ZERO_OBJECT(sf_cfg_dl);
      sf_cfg_dl.tti     = i;
      sf_cfg_dl.cfi     = cfi;
      sf_cfg_dl.sf_type = SRSRAN_SF_NORM;

      uint32_t              _nof_locations                           = {};
      srsran_dci_location_t _dci_locations[SRSRAN_MAX_CANDIDATES_UE] = {};
      _nof_locations = srsran_pdcch_ue_locations(&pdcch, &sf_cfg_dl, _dci_locations, SRSRAN_MAX_CANDIDATES_UE, ue_rnti);

      // Take L == 0 aggregation levels
      for (uint32_t j = 0; j < _nof_locations && nof_locations[i] < SRSRAN_MAX_CANDIDATES_UE; j++) {
        if (_dci_locations[j].L == 0) {
          dci_locations[i][nof_locations[i]] = _dci_locations[j];
          nof_locations[i]++;
        }
      }
    }
    srsran_pdcch_free(&pdcch);
    srsran_regs_free(&regs);

    // Find a valid UL DCI RIV
    uint32_t L_prb = phy_cell_cfg[0].cell.nof_prb - 2;
    do {
      if (srsran_dft_precoding_valid_prb(L_prb)) {
        ul_riv = srsran_ra_type2_to_riv(L_prb, 1, phy_cell_cfg[0].cell.nof_prb);
      } else {
        L_prb--;
      }
    } while (ul_riv == 0);

    data = srsran_vec_u8_malloc(150000);
    memset(data, 0, 150000);
  }

  ~dummy_stack()
  {
    srsran_softbuffer_tx_free(&softbuffer_tx);
    for (auto& v : softbuffer_rx) {
      for (auto& sb : v) {
        srsran_softbuffer_rx_free(&sb);
      }
    }
    if (data) {
      free(data);
    }

    srsran_random_free(random_gen);
  }

  void set_active_cell_list(std::vector<uint32_t>& active_cell_list_)
  {
    std::lock_guard<std::mutex> lock(phy_mac_mutex);
    active_cell_list = active_cell_list_;
  }

  int sr_detected(uint32_t tti, uint16_t rnti) override
  {
    std::lock_guard<std::mutex> lock(phy_mac_mutex);

    tti_sr_info_t tti_sr_info = {};
    tti_sr_info.tti           = tti;
    tti_sr_info_queue.push(tti_sr_info);

    notify_sr_detected();

    logger.info("Received SR tti=%d; rnti=0x%x", tti, rnti);

    return SRSRAN_SUCCESS;
  }
  void rach_detected(uint32_t tti, uint32_t primary_cc_idx, uint32_t preamble_idx, uint32_t time_adv) override
  {
    notify_rach_detected();
  }
  int ri_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t ri_value) override
  {
    notify_ri_info();

    logger.info("Received RI tti=%d; rnti=0x%x; cc_idx=%d; ri=%d;", tti, rnti, cc_idx, ri_value);

    return 0;
  }
  int pmi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t pmi_value) override
  {
    notify_pmi_info();

    logger.info("Received PMI tti=%d; rnti=0x%x; cc_idx=%d; pmi=%d;", tti, rnti, cc_idx, pmi_value);

    return 0;
  }
  int cqi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t cqi_value) override
  {
    std::lock_guard<std::mutex> lock(phy_mac_mutex);

    tti_cqi_info_t tti_cqi_info = {};
    tti_cqi_info.tti            = tti;
    tti_cqi_info.cc_idx         = cc_idx;
    tti_cqi_info.cqi            = cqi_value;
    tti_cqi_info_queue.push(tti_cqi_info);

    notify_cqi_info();

    logger.info("Received CQI tti=%d; rnti=0x%x; cc_idx=%d; cqi=%d;", tti, rnti, cc_idx, cqi_value);

    return SRSRAN_SUCCESS;
  }
  int sb_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t sb_idx, uint32_t cqi_value) override
  {
    notify_sb_cqi_info();
    logger.info("Received CQI tti=%d; rnti=0x%x; cc_idx=%d; sb_idx=%d cqi=%d;", tti, rnti, cc_idx, sb_idx, cqi_value);

    return SRSRAN_SUCCESS;
  }
  int snr_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, float snr_db, ul_channel_t ch) override
  {
    notify_snr_info();
    return 0;
  }
  int ta_info(uint32_t tti, uint16_t rnti, float ta_us) override
  {
    logger.info("Received TA INFO tti=%d; rnti=0x%x; ta=%.1f us", tti, rnti, ta_us);
    notify_ta_info();
    return 0;
  }
  int ack_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t tb_idx, bool ack) override
  {
    std::lock_guard<std::mutex> lock(phy_mac_mutex);

    // Push grant info in queue
    tti_dl_info_t tti_dl_info = {};
    tti_dl_info.tti           = tti;
    tti_dl_info.cc_idx        = cc_idx;
    tti_dl_info.tb_idx        = tb_idx;
    tti_dl_info.ack           = ack;
    tti_dl_info_ack_queue.push(tti_dl_info);

    logger.info("Received DL ACK tti=%d; rnti=0x%x; cc=%d; tb=%d; ack=%d;", tti, rnti, cc_idx, tb_idx, ack);
    notify_ack_info();
    return 0;
  }
  int crc_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t nof_bytes, bool crc_res) override
  {
    std::lock_guard<std::mutex> lock(phy_mac_mutex);

    // Push grant info in queue
    tti_ul_info_t tti_ul_info = {};
    tti_ul_info.tti           = tti;
    tti_ul_info.cc_idx        = cc_idx;
    tti_ul_info.crc           = crc_res;
    tti_ul_info_ack_queue.push(tti_ul_info);

    logger.info("Received UL ACK tti=%d; rnti=0x%x; cc=%d; ack=%d;", tti, rnti, cc_idx, crc_res);
    notify_crc_info();

    return 0;
  }
  int push_pdu(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t nof_bytes, bool crc_res, uint32_t grant_nof_prbs)
      override
  {
    logger.info("Received push_pdu tti=%d; rnti=0x%x; ack=%d;", tti, rnti, crc_res);
    notify_push_pdu();

    return 0;
  }
  int get_dl_sched(uint32_t tti, dl_sched_list_t& dl_sched_res) override
  {
    std::lock_guard<std::mutex> lock(phy_mac_mutex);

    // Notify test engine
    notify_get_dl_sched();

    // Make sure it writes the CFI in all cells
    for (dl_sched_t& dl_sched : dl_sched_res) {
      dl_sched.cfi = cfi;
    }

    // Iterate for each carrier
    uint32_t ue_cc_idx = 0;
    for (uint32_t& cc_idx : active_cell_list) {
      auto& dl_sched = dl_sched_res[cc_idx];

      // Default TB scheduling
      bool sched_tb[SRSRAN_MAX_TB] = {};

      sched_tb[0] = srsran_random_bool(random_gen, prob_dl_grant);

      // Schedule second TB for TM3 or TM4
      if (phy_rrc[ue_cc_idx].phy_cfg.dl_cfg.tm == SRSRAN_TM3 or phy_rrc[ue_cc_idx].phy_cfg.dl_cfg.tm == SRSRAN_TM4) {
        sched_tb[1] = srsran_random_bool(random_gen, prob_dl_grant);
      }

      // Random decision on whether transmit or not
      bool sched = sched_tb[0] | sched_tb[1];

      // RNTI needs to be valid
      sched &= (ue_rnti != 0);

      // Number of locations needs to be more than 2
      sched &= (nof_locations[tti % SRSRAN_NOF_SF_X_FRAME] > 1);

      // Schedule grant
      if (sched) {
        uint32_t              location_idx = tti % nof_locations[tti % SRSRAN_NOF_SF_X_FRAME];
        srsran_dci_location_t location     = dci_locations[tti % SRSRAN_NOF_SF_X_FRAME][location_idx];

        dl_sched.nof_grants                           = 1;
        dl_sched.pdsch[0].softbuffer_tx[0]            = &softbuffer_tx;
        dl_sched.pdsch[0].softbuffer_tx[1]            = &softbuffer_tx;
        dl_sched.pdsch[0].dci.location                = location;
        dl_sched.pdsch[0].dci.type0_alloc.rbg_bitmask = 0xffffffff;
        dl_sched.pdsch[0].dci.rnti                    = ue_rnti;
        dl_sched.pdsch[0].dci.alloc_type              = SRSRAN_RA_ALLOC_TYPE0;
        dl_sched.pdsch[0].data[0]                     = data;
        dl_sched.pdsch[0].data[1]                     = data;
        dl_sched.pdsch[0].dci.tpc_pucch               = (location.ncce) % SRSRAN_PUCCH_SIZE_AN_CS;

        // Set DCI format depending on the transmission mode
        switch (phy_rrc[0].phy_cfg.dl_cfg.tm) {
          default:
          case SRSRAN_TM1:
          case SRSRAN_TM2:
            dl_sched.pdsch[0].dci.format = SRSRAN_DCI_FORMAT1;
            break;
          case SRSRAN_TM3:
            dl_sched.pdsch[0].dci.format = SRSRAN_DCI_FORMAT2A;
            break;
          case SRSRAN_TM4:
            dl_sched.pdsch[0].dci.format = SRSRAN_DCI_FORMAT2;
            break;
        }

        // Push grant info in queue
        tti_dl_info_t tti_dl_info = {};
        tti_dl_info.tti           = tti;
        tti_dl_info.cc_idx        = cc_idx;
        tti_dl_info.tb_idx        = 0;
        tti_dl_info.ack           = dl_ack_value(ue_cc_idx, tti);

        // Schedule TB
        uint32_t cw_count = 0;
        for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
          if (sched_tb[tb]) {
            logger.debug("Transmitted DL grant tti=%d; rnti=0x%x; cc=%d; tb=%d;", tti, ue_rnti, cc_idx, tb);

            // Create Grant with maximum safe MCS
            dl_sched.pdsch[0].dci.tb[tb].cw_idx  = cw_count++;
            dl_sched.pdsch[0].dci.tb[tb].mcs_idx = 27;
            dl_sched.pdsch[0].dci.tb[tb].rv      = 0;
            dl_sched.pdsch[0].dci.tb[tb].ndi     = false;

            // Push to queue
            tti_dl_info.tb_idx = tb;
            tti_dl_info_sched_queue.push(tti_dl_info);
          } else {
            // Create Grant with no TB
            dl_sched.pdsch[0].dci.tb[tb].cw_idx  = 0;
            dl_sched.pdsch[0].dci.tb[tb].mcs_idx = 0;
            dl_sched.pdsch[0].dci.tb[tb].rv      = 1;
            dl_sched.pdsch[0].dci.tb[tb].ndi     = false;
          }
        }
      } else {
        dl_sched.nof_grants = 0;
      }

      ue_cc_idx++;
    }

    return 0;
  }
  int get_mch_sched(uint32_t tti, bool is_mcch, dl_sched_list_t& dl_sched_res) override
  {
    notify_get_mch_sched();
    return 0;
  }
  int get_ul_sched(uint32_t tti, ul_sched_list_t& ul_sched_res) override
  {
    // Notify test engine
    notify_get_ul_sched();

    std::lock_guard<std::mutex> lock(phy_mac_mutex);

    // Iterate for each carrier following the eNb/Cell order
    for (uint32_t cc_idx = 0; cc_idx < ul_sched_res.size(); cc_idx++) {
      auto  scell_idx = active_cell_list.size();
      auto& ul_sched  = ul_sched_res[cc_idx];

      // Checks if the eNb cell/carrier is enabled
      for (uint32_t i = 0; i < active_cell_list.size() and scell_idx == active_cell_list.size(); i++) {
        if (cc_idx == active_cell_list[i]) {
          scell_idx = i;
        }
      }

      // Random decision on whether transmit or not
      bool sched = srsran_random_bool(random_gen, prob_ul_grant);

      sched &= (scell_idx < active_cell_list.size());

      // RNTI needs to be valid
      sched &= (ue_rnti != 0);

      // Number of locations needs to be more than 2
      sched &= (nof_locations[tti % SRSRAN_NOF_SF_X_FRAME] > 1);

      // Avoid giving grants when SR is expected
      sched &= (tti % 20 != 0);

      // Schedule grant
      if (sched) {
        uint32_t              tti_pdcch    = TTI_SUB(tti, FDD_HARQ_DELAY_DL_MS);
        uint32_t              location_idx = (tti_pdcch + 1) % nof_locations[tti_pdcch % SRSRAN_NOF_SF_X_FRAME];
        srsran_dci_location_t location     = dci_locations[tti_pdcch % SRSRAN_NOF_SF_X_FRAME][location_idx];

        ul_sched.nof_grants                       = 1;
        ul_sched.pusch[0]                         = {};
        ul_sched.pusch[0].dci.rnti                = ue_rnti;
        ul_sched.pusch[0].dci.format              = SRSRAN_DCI_FORMAT0;
        ul_sched.pusch[0].dci.location            = location;
        ul_sched.pusch[0].dci.type2_alloc.riv     = ul_riv;
        ul_sched.pusch[0].dci.type2_alloc.n_prb1a = srsran_ra_type2_t::SRSRAN_RA_TYPE2_NPRB1A_2;
        ul_sched.pusch[0].dci.type2_alloc.n_gap   = srsran_ra_type2_t::SRSRAN_RA_TYPE2_NG1;
        ul_sched.pusch[0].dci.type2_alloc.mode    = srsran_ra_type2_t::SRSRAN_RA_TYPE2_LOC;
        ul_sched.pusch[0].dci.freq_hop_fl         = srsran_dci_ul_t::SRSRAN_RA_PUSCH_HOP_DISABLED;
        ul_sched.pusch[0].dci.tb.mcs_idx          = 20; // Can't set it too high for grants with CQI and long ACK/NACK
        ul_sched.pusch[0].dci.tb.rv               = 0;
        ul_sched.pusch[0].dci.tb.ndi              = false;
        ul_sched.pusch[0].dci.tb.cw_idx           = 0;
        ul_sched.pusch[0].dci.n_dmrs              = 0;
        ul_sched.pusch[0].dci.cqi_request         = false;
        ul_sched.pusch[0].data                    = data;

        ul_sched.pusch[0].needs_pdcch   = true;
        ul_sched.pusch[0].softbuffer_rx = &softbuffer_rx[scell_idx][tti % SRSRAN_FDD_NOF_HARQ];

        // Reset Rx softbuffer
        srsran_softbuffer_rx_reset(ul_sched.pusch[0].softbuffer_rx);

        // Push grant info in queue
        tti_ul_info_t tti_ul_info = {};
        tti_ul_info.tti           = tti;
        tti_ul_info.cc_idx        = cc_idx;
        tti_ul_info.crc           = true;

        // Push to queue
        tti_ul_info_sched_queue.push(tti_ul_info);
      } else {
        ul_sched.nof_grants = 0;
      }
    }

    return SRSRAN_SUCCESS;
  }
  void set_sched_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs) override { notify_set_sched_dl_tti_mask(); }
  void tti_clock() { notify_tti_clock(); }
  int  run_tti(bool enable_assert)
  {
    std::lock_guard<std::mutex> lock(phy_mac_mutex);

    // Check DL ACKs match with grants
    while (not tti_dl_info_ack_queue.empty()) {
      // Get both Info
      tti_dl_info_t& tti_dl_sched = tti_dl_info_sched_queue.front();
      tti_dl_info_t& tti_dl_ack   = tti_dl_info_ack_queue.front();

      // Calculate ACK TTI
      tti_dl_sched.tti = TTI_ADD(tti_dl_sched.tti, FDD_HARQ_DELAY_DL_MS);

      // Assert that ACKs have been received
      if (enable_assert) {
        TESTASSERT(tti_dl_sched.tti == tti_dl_ack.tti);
        TESTASSERT(tti_dl_sched.cc_idx == tti_dl_ack.cc_idx);
        TESTASSERT(tti_dl_sched.tb_idx == tti_dl_ack.tb_idx);
        TESTASSERT(tti_dl_sched.ack == tti_dl_ack.ack);
      }
      tti_dl_info_sched_queue.pop();
      tti_dl_info_ack_queue.pop();
    }

    // Check UL ACKs match with grants
    while (not tti_ul_info_ack_queue.empty()) {
      // Get both Info
      tti_ul_info_t& tti_ul_sched = tti_ul_info_sched_queue.front();
      tti_ul_info_t& tti_ul_ack   = tti_ul_info_ack_queue.front();

      // Assert that ACKs have been received
      if (enable_assert) {
        TESTASSERT(tti_ul_sched.tti == tti_ul_ack.tti);
        TESTASSERT(tti_ul_sched.cc_idx == tti_ul_ack.cc_idx);
        TESTASSERT(tti_ul_sched.crc == tti_ul_ack.crc);
      }

      tti_ul_info_sched_queue.pop();
      tti_ul_info_ack_queue.pop();
    }

    //  Check SR match with TTI
    size_t req_queue_size = (enable_assert) ? 1 : 0;
    while (tti_sr_info_queue.size() > req_queue_size) {
      tti_sr_info_t tti_sr_info1 = tti_sr_info_queue.front();

      // POP first from queue
      tti_sr_info_queue.pop();

      if (enable_assert) {
        // Get second, do not pop
        tti_sr_info_t& tti_sr_info2 = tti_sr_info_queue.front();

        uint32_t elapsed_tti = TTI_SUB(tti_sr_info2.tti, tti_sr_info1.tti);

        // Log SR info
        logger.info("SR: tti1=%d; tti2=%d; elapsed %d;", tti_sr_info1.tti, tti_sr_info2.tti, elapsed_tti);

        // Check first TTI
        TESTASSERT(tti_sr_info1.tti % 20 == 0);

        // Make sure the TTI difference is 20
        TESTASSERT(elapsed_tti == 20);
      }
    }

    return SRSRAN_SUCCESS;
  }
};

typedef std::unique_ptr<dummy_stack> unique_dummy_stack_t;

class dummy_ue
{
private:
  std::vector<srsran_ue_dl_t*>                      ue_dl_v       = {};
  std::vector<srsran_ue_ul_t*>                      ue_ul_v       = {};
  std::vector<cf_t*>                                buffers       = {};
  dummy_radio*                                      radio         = nullptr;
  uint32_t                                          sf_len        = 0;
  uint32_t                                          nof_ports     = 0;
  uint16_t                                          rnti          = 0;
  srsran_dl_sf_cfg_t                                sf_dl_cfg     = {};
  srsran_ul_sf_cfg_t                                sf_ul_cfg     = {};
  srsran_softbuffer_tx_t                            softbuffer_tx = {};
  uint8_t*                                          tx_data       = nullptr;
  srsenb::phy_interface_rrc_lte::phy_rrc_cfg_list_t phy_rrc_cfg   = {};
  srslog::basic_logger&                             logger;
  std::map<uint32_t, uint32_t>                      last_ri = {};

public:
  dummy_ue(dummy_radio* _radio, const srsenb::phy_cell_cfg_list_t& cell_list, std::string log_level, uint16_t rnti_) :
    radio(_radio), logger(srslog::fetch_basic_logger("UPHY"))
  {
    // Calculate subframe length
    nof_ports = cell_list[0].cell.nof_ports;
    sf_len    = static_cast<uint32_t>(SRSRAN_SF_LEN_PRB(cell_list[0].cell.nof_prb));
    rnti      = rnti_;

    logger.set_level(srslog::str_to_basic_level(log_level));

    // Initialise one buffer per eNb
    for (uint32_t i = 0; i < cell_list.size() * nof_ports; i++) {
      // Allocate buffers
      cf_t* buffer = srsran_vec_cf_malloc(sf_len);
      if (not buffer) {
        ERROR("Allocating UE DL buffer");
      }
      buffers.push_back(buffer);

      // Set buffer to zero
      srsran_vec_cf_zero(buffer, sf_len);
    }

    // Iterate over all cells
    for (uint32_t cc_idx = 0; cc_idx < (uint32_t)cell_list.size(); cc_idx++) {
      const srsran_cell_t& cell = cell_list[cc_idx].cell;

      // Allocate UE DL
      auto* ue_dl = (srsran_ue_dl_t*)srsran_vec_malloc(sizeof(srsran_ue_dl_t));
      if (not ue_dl) {
        ERROR("Allocatin UE DL");
      }
      ue_dl_v.push_back(ue_dl);

      // Initialise UE DL
      if (srsran_ue_dl_init(ue_dl, &buffers[cc_idx * nof_ports], cell.nof_prb, cell.nof_ports)) {
        ERROR("Initiating UE DL");
      }

      // Set Cell
      if (srsran_ue_dl_set_cell(ue_dl, cell)) {
        ERROR("Setting UE DL cell");
      }

      // Allocate UE UL
      auto* ue_ul = (srsran_ue_ul_t*)srsran_vec_malloc(sizeof(srsran_ue_ul_t));
      if (not ue_ul) {
        ERROR("Allocatin UE UL");
      }
      ue_ul_v.push_back(ue_ul);

      // Initialise UE UL
      if (srsran_ue_ul_init(ue_ul, buffers[cc_idx * nof_ports], cell.nof_prb)) {
        ERROR("Setting UE UL cell");
      }

      // Set cell
      if (srsran_ue_ul_set_cell(ue_ul, cell)) {
        ERROR("Setting UE DL cell");
      }
    }

    // Initialise softbuffer
    if (srsran_softbuffer_tx_init(&softbuffer_tx, cell_list[0].cell.nof_prb)) {
      ERROR("Initialising Tx softbuffer");
    }

    // Initialise dummy tx data
    tx_data = srsran_vec_u8_malloc(SRSENB_MAX_BUFFER_SIZE_BYTES);
    if (not tx_data) {
      ERROR("Allocating Tx data");
    }

    for (uint32_t i = 0; i < SRSENB_MAX_BUFFER_SIZE_BYTES; i++) {
      tx_data[i] = static_cast<uint8_t>(((i + 257) * (i + 373)) % 255); ///< Creative random data generator
    }

    // Push HARQ delay to radio
    for (uint32_t i = 0; i < FDD_HARQ_DELAY_DL_MS; i++) {
      radio->write_rx(buffers, sf_len);
      sf_ul_cfg.tti = TTI_ADD(sf_ul_cfg.tti, 1); // Advance UL TTI too
    }
    for (uint32_t i = 0; i < FDD_HARQ_DELAY_UL_MS; i++) {
      radio->write_rx(buffers, sf_len);
    }
  }

  ~dummy_ue()
  {
    for (auto& ue_dl : ue_dl_v) {
      if (ue_dl) {
        srsran_ue_dl_free(ue_dl);
        free(ue_dl);
      }
    }
    for (auto& ue_ul : ue_ul_v) {
      if (ue_ul) {
        srsran_ue_ul_free(ue_ul);
        free(ue_ul);
      }
    }
    for (auto& b : buffers) {
      if (b) {
        free(b);
      }
    }
    if (tx_data) {
      free(tx_data);
    }
    srsran_softbuffer_tx_free(&softbuffer_tx);
  }

  void reconfigure(const srsenb::phy_interface_rrc_lte::phy_rrc_cfg_list_t& phy_rrc_cfg_)
  {
    // Copy new configuration
    phy_rrc_cfg = phy_rrc_cfg_;

    // Enable Extended CSI request bits in DCI format 0 according to 3GPP 36.212 R10 5.3.3.1.1
    for (auto& e : phy_rrc_cfg) {
      e.phy_cfg.dl_cfg.dci.multiple_csi_request_enabled = (phy_rrc_cfg.size() > 1);
    }
  }

  int work_dl(srsran_pdsch_ack_t& pdsch_ack, srsran_uci_data_t& uci_data)
  {
    // Read DL
    TESTASSERT(radio->read_tx(buffers, sf_len) >= SRSRAN_SUCCESS);

    // Get grants DL/UL, we do not care about Decoding PDSCH
    for (uint32_t ue_cc_idx = 0; ue_cc_idx < phy_rrc_cfg.size(); ue_cc_idx++) {
      uint32_t           cc_idx    = phy_rrc_cfg[ue_cc_idx].enb_cc_idx;
      srsran::phy_cfg_t& dedicated = phy_rrc_cfg[ue_cc_idx].phy_cfg;

      /// Set UCI configuration from PCell only
      if (ue_cc_idx == 0) {
        uci_data.cfg = dedicated.ul_cfg.pucch.uci_cfg;

        pdsch_ack.ack_nack_feedback_mode = dedicated.ul_cfg.pucch.ack_nack_feedback_mode;
        pdsch_ack.nof_cc                 = static_cast<uint32_t>(phy_rrc_cfg.size());
        pdsch_ack.transmission_mode      = dedicated.dl_cfg.tm;
        pdsch_ack.simul_cqi_ack          = dedicated.ul_cfg.pucch.simul_cqi_ack;
      }

      srsran_dci_dl_t    dci_dl[SRSRAN_MAX_DCI_MSG] = {};
      srsran_ue_dl_cfg_t ue_dl_cfg                  = {};
      ue_dl_cfg.cfg                                 = dedicated.dl_cfg;
      ue_dl_cfg.cfg.cqi_report.periodic_mode        = SRSRAN_CQI_MODE_12;
      ue_dl_cfg.cfg.pdsch.rnti                      = rnti;

      int report_ri_cc_idx = -1;
      if (last_ri.count(ue_cc_idx)) {
        ue_dl_cfg.last_ri = last_ri[ue_cc_idx];
      }

      srsran_ue_dl_decode_fft_estimate(ue_dl_v[cc_idx], &sf_dl_cfg, &ue_dl_cfg);

      // Get DL Grants
      int nof_dl_grants = srsran_ue_dl_find_dl_dci(ue_dl_v[cc_idx], &sf_dl_cfg, &ue_dl_cfg, rnti, dci_dl);
      TESTASSERT(nof_dl_grants >= SRSRAN_SUCCESS);

      // Generate ACKs
      if (nof_dl_grants) {
        char str[256] = {};
        srsran_dci_dl_info(dci_dl, str, sizeof(str));
        logger.info("[DL DCI] %s", str);

        if (srsran_ue_dl_dci_to_pdsch_grant(
                ue_dl_v[cc_idx], &sf_dl_cfg, &ue_dl_cfg, dci_dl, &ue_dl_cfg.cfg.pdsch.grant)) {
          logger.error("Converting DCI message to DL dci");
          return SRSRAN_ERROR;
        }

        srsran_pdsch_tx_info(&ue_dl_cfg.cfg.pdsch, str, 512);

        logger.info("[DL PDSCH %d] cc=%d, %s", sf_dl_cfg.tti, cc_idx, str);

        pdsch_ack.cc[ue_cc_idx].M                           = 1;
        pdsch_ack.cc[ue_cc_idx].m[0].present                = true;
        pdsch_ack.cc[ue_cc_idx].m[0].resource.v_dai_dl      = dci_dl->dai;
        pdsch_ack.cc[ue_cc_idx].m[0].resource.n_cce         = dci_dl->location.ncce;
        pdsch_ack.cc[ue_cc_idx].m[0].resource.grant_cc_idx  = ue_cc_idx;
        pdsch_ack.cc[ue_cc_idx].m[0].resource.tpc_for_pucch = dci_dl->tpc_pucch;

        for (uint32_t tb_idx = 0; tb_idx < SRSRAN_MAX_TB; tb_idx++) {
          if (ue_dl_cfg.cfg.pdsch.grant.tb[tb_idx].enabled) {
            pdsch_ack.cc[ue_cc_idx].m[0].value[tb_idx] = dl_ack_value(ue_cc_idx, sf_dl_cfg.tti);
          } else {
            pdsch_ack.cc[ue_cc_idx].m[0].value[tb_idx] = 2;
          }
        }
      } else {
        pdsch_ack.cc[ue_cc_idx].M            = 1;
        pdsch_ack.cc[ue_cc_idx].m[0].present = false;
      }

      // Generate CQI periodic if required
      srsran_ue_dl_gen_cqi_periodic(ue_dl_v[cc_idx], &ue_dl_cfg, 0x0f, sf_ul_cfg.tti, &uci_data);

      if (srsran_cqi_periodic_ri_send(&ue_dl_cfg.cfg.cqi_report, sf_ul_cfg.tti, ue_dl_v[cc_idx]->cell.frame_type) &&
          uci_data.cfg.cqi.ri_len) {
        uci_data.cfg.cqi.scell_index = ue_cc_idx;
      }
    }

    return SRSRAN_SUCCESS;
  }

  void fill_uci(uint32_t             cc_idx,
                srsran_ue_ul_cfg_t&  ue_ul_cfg,
                srsran_uci_data_t&   uci_data,
                srsran_pdsch_ack_t&  pdsch_ack,
                srsran_pusch_data_t& pusch_data)
  {
    // Generate scheduling request
    srsran_ue_ul_gen_sr(&ue_ul_cfg, &sf_ul_cfg, &uci_data, (bool)(sf_ul_cfg.tti % 20 == 0));

    // Generate Acknowledgements
    srsran_ue_dl_gen_ack(&ue_dl_v[cc_idx]->cell, &sf_dl_cfg, &pdsch_ack, &uci_data);

    if (uci_data.cfg.cqi.ri_len) {
      last_ri[uci_data.cfg.cqi.scell_index] = uci_data.value.ri;
    }

    // Set UCI only for lowest serving cell index
    pusch_data.uci                 = uci_data.value;
    ue_ul_cfg.ul_cfg.pusch.uci_cfg = uci_data.cfg;
    ue_ul_cfg.ul_cfg.pucch.uci_cfg = uci_data.cfg;
  }

  int work_ul(srsran_pdsch_ack_t& pdsch_ack, srsran_uci_data_t& uci_data)
  {
    bool first_pusch = true;

    // Zero all IQ UL buffers
    for (auto& buffer : buffers) {
      srsran_vec_cf_zero(buffer, SRSRAN_SF_LEN_PRB(ue_ul_v[0]->cell.nof_prb));
    }

    for (uint32_t i = 0; i < phy_rrc_cfg.size(); i++) {
      srsran_dci_ul_t    dci_ul[SRSRAN_MAX_DCI_MSG] = {};
      srsran::phy_cfg_t& dedicated                  = phy_rrc_cfg[i].phy_cfg;
      uint32_t           cc_idx                     = phy_rrc_cfg[i].enb_cc_idx;

      srsran_ue_ul_cfg_t ue_ul_cfg          = {};
      ue_ul_cfg.ul_cfg                      = dedicated.ul_cfg;
      ue_ul_cfg.ul_cfg.pusch.softbuffers.tx = &softbuffer_tx;
      ue_ul_cfg.ul_cfg.pusch.rnti           = rnti;
      ue_ul_cfg.ul_cfg.pucch.rnti           = rnti;
      ue_ul_cfg.cc_idx                      = i; // SCell index

      srsran_ue_dl_cfg_t ue_dl_cfg           = {};
      ue_dl_cfg.cfg                          = dedicated.dl_cfg;
      ue_dl_cfg.cfg.cqi_report.periodic_mode = SRSRAN_CQI_MODE_12;
      ue_dl_cfg.cfg.pdsch.rnti               = rnti;

      // Get UL grants
      int nof_ul_grants = srsran_ue_dl_find_ul_dci(ue_dl_v[cc_idx], &sf_dl_cfg, &ue_dl_cfg, rnti, dci_ul);
      TESTASSERT(nof_ul_grants >= SRSRAN_SUCCESS);

      srsran_pusch_data_t pusch_data = {};
      pusch_data.ptr                 = tx_data;

      if (nof_ul_grants > SRSRAN_SUCCESS) {
        TESTASSERT(srsran_ue_ul_dci_to_pusch_grant(
                       ue_ul_v[cc_idx], &sf_ul_cfg, &ue_ul_cfg, dci_ul, &ue_ul_cfg.ul_cfg.pusch.grant) >=
                   SRSRAN_SUCCESS);

        srsran_softbuffer_tx_reset(&softbuffer_tx);

        ue_ul_cfg.ul_cfg.pusch.softbuffers.tx = &softbuffer_tx;
        ue_ul_cfg.grant_available             = true;
        pdsch_ack.is_pusch_available          = true;

        // Generate UCI data
        if (first_pusch) {
          fill_uci(cc_idx, ue_ul_cfg, uci_data, pdsch_ack, pusch_data);

          first_pusch = false;
        }
      }

      // Work UL
      TESTASSERT(srsran_ue_ul_encode(ue_ul_v[cc_idx], &sf_ul_cfg, &ue_ul_cfg, &pusch_data) >= SRSRAN_SUCCESS);

      char str[256] = {};
      srsran_ue_ul_info(&ue_ul_cfg, &sf_ul_cfg, &pusch_data.uci, str, sizeof(str));
      if (str[0]) {
        logger.info("[UL INFO %d] %s", i, str);
      }
    }

    // If no PUSCH, send PUCCH
    if (first_pusch) {
      uint32_t           cc_idx    = phy_rrc_cfg[0].enb_cc_idx;
      srsran::phy_cfg_t& dedicated = phy_rrc_cfg[0].phy_cfg;

      srsran_ue_ul_cfg_t ue_ul_cfg          = {};
      ue_ul_cfg.ul_cfg                      = dedicated.ul_cfg;
      ue_ul_cfg.ul_cfg.pusch.softbuffers.tx = &softbuffer_tx;
      ue_ul_cfg.ul_cfg.pucch.rnti           = rnti;
      ue_ul_cfg.cc_idx                      = 0; // SCell index

      srsran_pusch_data_t pusch_data = {};

      fill_uci(cc_idx, ue_ul_cfg, uci_data, pdsch_ack, pusch_data);

      // Work UL PUCCH
      TESTASSERT(srsran_ue_ul_encode(ue_ul_v[cc_idx], &sf_ul_cfg, &ue_ul_cfg, &pusch_data) >= SRSRAN_SUCCESS);

      char str[256] = {};
      srsran_ue_ul_info(&ue_ul_cfg, &sf_ul_cfg, &pusch_data.uci, str, sizeof(str));
      if (str[0]) {
        logger.info("[UL INFO %d] %s", 0, str);
      }
    }

    // Write eNb Rx
    radio->write_rx(buffers, sf_len);

    return SRSRAN_SUCCESS;
  }

  int run_tti()
  {
    srsran_uci_data_t  uci_data  = {};
    srsran_pdsch_ack_t pdsch_ack = {};

    // Set logging TTI
    logger.set_context(sf_dl_cfg.tti);

    // Work DL
    TESTASSERT(work_dl(pdsch_ack, uci_data) == SRSRAN_SUCCESS);

    // Work UL
    TESTASSERT(work_ul(pdsch_ack, uci_data) == SRSRAN_SUCCESS);

    // Increment TTI
    sf_dl_cfg.tti = TTI_ADD(sf_dl_cfg.tti, 1);
    sf_ul_cfg.tti = TTI_ADD(sf_ul_cfg.tti, 1);

    return SRSRAN_SUCCESS;
  }
};

typedef std::unique_ptr<dummy_ue> unique_dummy_ue_phy_t;

typedef std::unique_ptr<srsenb::phy> unique_srsenb_phy_t;

class phy_test_bench : public srsenb::enb_time_interface
{
public:
  struct args_t {
    uint16_t              rnti                = 0x1234;
    uint32_t              duration            = 10240;
    uint32_t              nof_enb_cells       = 1;
    srsran_cell_t         cell                = {};
    std::string           ue_cell_list_str    = "0"; ///< First indicates PCell
    std::vector<uint32_t> ue_cell_list        = {0};
    std::string           ack_mode            = "normal";
    std::string           log_level           = "none";
    uint32_t              tm_u32              = 1;
    uint32_t              period_pcell_rotate = 0;
    srsran_tm_t           tm                  = SRSRAN_TM1;
    bool                  extended_cp         = false;
    args_t()
    {
      cell.nof_prb   = 6;
      cell.nof_ports = 1;
    }

    // Initialises secondary parameters
    void init()
    {
      switch (tm_u32) {
        case 2:
          cell.nof_ports = 2;
          tm             = SRSRAN_TM2;
          break;
        case 3:
          cell.nof_ports = 2;
          tm             = SRSRAN_TM3;
          break;
        case 4:
          cell.nof_ports = 2;
          tm             = SRSRAN_TM4;
          break;
        case 1:
        default:
          cell.nof_ports = 1;
          tm             = SRSRAN_TM1;
      }
    }
  };

private:
  // Test constants
  static const uint32_t delta_pucch = 1;
  static const uint32_t N_pucch_1   = 12;

  // Private classes
  unique_dummy_radio_t  radio;
  unique_dummy_stack_t  stack;
  unique_srsenb_phy_t   enb_phy;
  unique_dummy_ue_phy_t ue_phy;
  srslog::basic_logger& logger;

  args_t                                            args = {};   ///< Test arguments
  srsenb::phy_args_t                                phy_args = {};   ///< PHY arguments
  srsenb::phy_cfg_t                                 phy_cfg  = {};   ///< eNb Cell/Carrier configuration
  srsenb::phy_interface_rrc_lte::phy_rrc_cfg_list_t phy_rrc_cfg; ///< UE PHY configuration

  uint64_t tti_counter = 0;
  typedef enum {
    change_state_assert = 0,
    change_state_flush,
    change_state_wait_steady,
  } change_state_t;
  change_state_t change_state = change_state_assert;

public:
  phy_test_bench(args_t& args_, srslog::sink& log_sink) :
    logger(srslog::fetch_basic_logger("TEST BENCH", log_sink, false))
  {
    // Copy test arguments
    args = args_;

    // Configure logger
    logger.set_level(srslog::str_to_basic_level(args.log_level));

    // PHY arguments
    phy_args.log.phy_level   = args.log_level;
    phy_args.nof_phy_threads = 1; ///< Set number of phy threads to 1 for avoiding concurrency issues

    // Create cell configuration
    phy_cfg.phy_cell_cfg.resize(args.nof_enb_cells);
    for (uint32_t i = 0; i < args.nof_enb_cells; i++) {
      auto& q        = phy_cfg.phy_cell_cfg[i];
      q.cell         = args.cell;
      q.cell.id      = i;
      q.cell_id      = i;
      q.cell.cp      = args.extended_cp ? SRSRAN_CP_EXT : SRSRAN_CP_NORM;
      q.dl_freq_hz   = 0.0f; ///< Frequencies are irrelevant in this test
      q.ul_freq_hz   = 0.0f;
      q.root_seq_idx = 25 + i; ///< Different PRACH root sequences
      q.rf_port      = i;
    }

    phy_cfg.pucch_cnfg.delta_pucch_shift                = asn1::rrc::pucch_cfg_common_s::delta_pucch_shift_e_::ds3;
    phy_cfg.prach_cnfg.root_seq_idx                     = 0;
    phy_cfg.prach_cnfg.prach_cfg_info.high_speed_flag   = false;
    phy_cfg.prach_cnfg.prach_cfg_info.prach_cfg_idx     = 3;
    phy_cfg.prach_cnfg.prach_cfg_info.prach_freq_offset = 2;
    phy_cfg.prach_cnfg.prach_cfg_info.zero_correlation_zone_cfg = 5;

    /// eNb PHY initialisation instance
    enb_phy = unique_srsenb_phy_t(new srsenb::phy(log_sink));
  }

  int init()
  {
    // Create base UE dedicated configuration
    srsran::phy_cfg_t dedicated = {};

    // Configure DL
    dedicated.dl_cfg.tm = args.tm;

    // Configure reporting
    dedicated.dl_cfg.cqi_report.periodic_configured = true;
    dedicated.dl_cfg.cqi_report.pmi_idx             = 25;
    dedicated.dl_cfg.cqi_report.periodic_mode       = SRSRAN_CQI_MODE_20;

    if (args.tm == SRSRAN_TM3 or args.tm == SRSRAN_TM4) {
      dedicated.dl_cfg.cqi_report.ri_idx_present = true;
      dedicated.dl_cfg.cqi_report.ri_idx         = 483; // Every 8 CQI/PMI report, schedule an RI report
    }

    // Configure UL Resources
    dedicated.ul_cfg.pucch.ack_nack_feedback_mode  = srsran_string_ack_nack_feedback_mode(args.ack_mode.c_str());
    dedicated.ul_cfg.pucch.delta_pucch_shift       = delta_pucch;
    dedicated.ul_cfg.pucch.n_rb_2                  = 2;
    dedicated.ul_cfg.pucch.N_cs                    = 0;
    dedicated.ul_cfg.pucch.n_pucch_sr              = 0;
    dedicated.ul_cfg.pucch.N_pucch_1               = N_pucch_1;
    dedicated.ul_cfg.pucch.n_pucch_2               = 5;
    dedicated.ul_cfg.pucch.simul_cqi_ack           = true;
    dedicated.ul_cfg.pucch.sr_configured           = true;
    dedicated.ul_cfg.pucch.I_sr                    = 5;
    dedicated.ul_cfg.pucch.n1_pucch_an_cs[0][0]    = N_pucch_1 + 2;
    dedicated.ul_cfg.pucch.n1_pucch_an_cs[1][0]    = N_pucch_1 + 3;
    dedicated.ul_cfg.pucch.n1_pucch_an_cs[2][0]    = N_pucch_1 + 4;
    dedicated.ul_cfg.pucch.n1_pucch_an_cs[3][0]    = N_pucch_1 + 5;
    dedicated.ul_cfg.pucch.n1_pucch_an_cs[0][1]    = N_pucch_1 + 3;
    dedicated.ul_cfg.pucch.n1_pucch_an_cs[1][1]    = N_pucch_1 + 4;
    dedicated.ul_cfg.pucch.n1_pucch_an_cs[2][1]    = N_pucch_1 + 5;
    dedicated.ul_cfg.pucch.n1_pucch_an_cs[3][1]    = N_pucch_1 + 6;
    dedicated.ul_cfg.pusch.uci_offset.I_offset_ack = 7;
    dedicated.ul_cfg.pusch.uci_offset.I_offset_ri  = 7;
    dedicated.ul_cfg.pusch.uci_offset.I_offset_cqi = 7;

    // Configure UE PHY
    std::array<bool, SRSRAN_MAX_CARRIERS> activation = {}; ///< Activation/Deactivation vector
    phy_rrc_cfg.resize(args.ue_cell_list.size());
    for (uint32_t i = 0; i < args.ue_cell_list.size(); i++) {
      phy_rrc_cfg[i].enb_cc_idx = args.ue_cell_list[i];      ///< First element is PCell
      phy_rrc_cfg[i].configured = true;                      ///< All configured by default
      phy_rrc_cfg[i].phy_cfg    = dedicated;                 ///< Load the same in all by default
      phy_rrc_cfg[i].phy_cfg.dl_cfg.cqi_report.pmi_idx += i; ///< CQI report depend on SCell index

      // Disable SCell stuff
      if (i != 0) {
        phy_rrc_cfg[i].phy_cfg.ul_cfg.pucch.sr_configured = false;
      }

      /// All the cell/carriers are activated from the beggining
      activation[i] = true;
    }

    /// Create Radio instance
    radio = unique_dummy_radio_t(
        new dummy_radio(args.nof_enb_cells * args.cell.nof_ports, args.cell.nof_prb, args.log_level));

    /// Create Dummy Stack instance
    stack = unique_dummy_stack_t(new dummy_stack(phy_cfg, phy_rrc_cfg, args.log_level, args.rnti));
    stack->set_active_cell_list(args.ue_cell_list);

    /// Initiate eNb PHY with the given RNTI
    if (enb_phy->init(phy_args, phy_cfg, radio.get(), stack.get(), this) < 0) {
      return SRSRAN_ERROR;
    }
    enb_phy->set_config(args.rnti, phy_rrc_cfg);
    enb_phy->complete_config(args.rnti);
    enb_phy->set_activation_deactivation_scell(args.rnti, activation);

    /// Create dummy UE instance
    ue_phy = unique_dummy_ue_phy_t(new dummy_ue(radio.get(), phy_cfg.phy_cell_cfg, args.log_level, args.rnti));

    /// Configure UE with initial configuration
    ue_phy->reconfigure(phy_rrc_cfg);

    return SRSRAN_SUCCESS;
  }

  void stop()
  {
    radio->stop();
    enb_phy->stop();
  }

  virtual ~phy_test_bench() = default;

  int run_tti()
  {
    int ret = SRSRAN_SUCCESS;

    TESTASSERT(ue_phy->run_tti() >= SRSRAN_SUCCESS);
    TESTASSERT(stack->run_tti(change_state == change_state_assert) >= SRSRAN_SUCCESS);

    // Change state FSM
    switch (change_state) {
      case change_state_assert:
        if (args.period_pcell_rotate > 0 and tti_counter >= args.period_pcell_rotate) {
          logger.warning("******* Cell rotation: Disable scheduling *******");
          // Disable all cells
          std::vector<uint32_t> active_cells;
          stack->set_active_cell_list(active_cells);

          change_state = change_state_flush;
          tti_counter  = 0;
        }
        break;
      case change_state_flush:
        if (tti_counter >= 2 * (FDD_HARQ_DELAY_DL_MS + FDD_HARQ_DELAY_UL_MS)) {
          logger.warning("******* Cell rotation: Reconfigure *******");

          std::array<bool, SRSRAN_MAX_CARRIERS> activation = {}; ///< Activation/Deactivation vector

          // Rotate primary cells
          for (auto& q : phy_rrc_cfg) {
            q.enb_cc_idx = (q.enb_cc_idx + 1) % args.nof_enb_cells;
          }

          for (uint32_t i = 0; i < args.ue_cell_list.size(); i++) {
            activation[i] = true;
          }

          // Reconfigure eNb PHY
          enb_phy->set_config(args.rnti, phy_rrc_cfg);
          enb_phy->complete_config(args.rnti);
          enb_phy->set_activation_deactivation_scell(args.rnti, activation);

          // Reconfigure UE PHY
          ue_phy->reconfigure(phy_rrc_cfg);

          change_state = change_state_wait_steady;
          tti_counter  = 0;
        }
        break;
      case change_state_wait_steady:
        if (tti_counter >= FDD_HARQ_DELAY_DL_MS + FDD_HARQ_DELAY_UL_MS) {
          logger.warning("******* Cell rotation: Enable scheduling *******");

          std::vector<uint32_t> active_cell_list;

          // Rotate primary cells
          for (auto& q : phy_rrc_cfg) {
            active_cell_list.push_back(q.enb_cc_idx);
          }

          stack->set_active_cell_list(active_cell_list);

          change_state = change_state_assert;
          tti_counter  = 0;
        }
        break;
    }
    // Increment counter
    tti_counter++;

    return ret;
  }

  void tti_clock() final
  {
    // nothing to do
  }
};

typedef std::unique_ptr<phy_test_bench> unique_phy_test_bench;

namespace bpo = boost::program_options;

int parse_args(int argc, char** argv, phy_test_bench::args_t& args)
{
  int ret = SRSRAN_SUCCESS;

  bpo::options_description options;
  bpo::options_description common("Common execution options");

  // clang-format off
  common.add_options()
      ("duration",       bpo::value<uint32_t>(&args.duration),                                           "Duration of the execution in subframes")
      ("rnti",           bpo::value<uint16_t>(&args.rnti),                                               "UE RNTI, used for random seed")
      ("log_level",           bpo::value<std::string>(&args.log_level),                                  "General logging level")
      ("nof_enb_cells",  bpo::value<uint32_t>(&args.nof_enb_cells),                                      "Cell Number of PRB")
      ("ue_cell_list",   bpo::value<std::string>(&args.ue_cell_list_str),                                 "UE active cell list, the first is used as PCell")
      ("ack_mode",       bpo::value<std::string>(&args.ack_mode),                                        "HARQ ACK/NACK mode: normal, pucch3, cs")
      ("cell.nof_prb",   bpo::value<uint32_t>(&args.cell.nof_prb)->default_value(args.cell.nof_prb),     "eNb Cell/Carrier bandwidth")
      ("cell.nof_ports", bpo::value<uint32_t>(&args.cell.nof_ports)->default_value(args.cell.nof_ports), "eNb Cell/Carrier number of ports")
      ("cell.cp",        bpo::value<bool>(&args.extended_cp)->default_value(false),                      "use extended CP")
      ("tm", bpo::value<uint32_t>(&args.tm_u32)->default_value(args.tm_u32),                             "Transmission mode")
      ("rotation", bpo::value<uint32_t>(&args.period_pcell_rotate),                      "Serving cells rotation period in ms, set to zero to disable")
      ;
  options.add(common).add_options()("help", "Show this message");
  // clang-format on

  bpo::variables_map vm;
  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(options).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error& e) {
    std::cerr << e.what() << std::endl;
    ret = SRSRAN_ERROR;
  }

  // populate UE Active cell list
  if (not args.ue_cell_list_str.empty()) {
    srsran::string_parse_list(args.ue_cell_list_str, ',', args.ue_cell_list);
  } else {
    return SRSRAN_ERROR;
  }

  // help option was given or error - print usage and exit
  if (vm.count("help") || ret) {
    std::cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << std::endl << std::endl;
    std::cout << options << std::endl << std::endl;
    ret = SRSRAN_ERROR;
  }

  return ret;
}

int main(int argc, char** argv)
{
  // First of all, initialise crash handler - Crash files from ctest executions will be stored in
  // srsenb/test/phy/srsRAN.backtrace.crash
  srsran_debug_handle_crash(argc, argv);

  phy_test_bench::args_t test_args;

  // Parse arguments
  TESTASSERT(parse_args(argc, argv, test_args) == SRSRAN_SUCCESS);

  // Initialize secondary parameters
  test_args.init();

  // Setup logging.
  srslog::init();

  // Create Test Bench
  unique_phy_test_bench test_bench = unique_phy_test_bench(new phy_test_bench(test_args, srslog::get_default_sink()));
  int                   err_code   = test_bench->init();
  bool                  valid_cfg  = test_args.nof_enb_cells <= SRSRAN_MAX_CARRIERS;
  if (not valid_cfg) {
    // Verify that phy returns with an error if provided an invalid configuration
    TESTASSERT(err_code != SRSRAN_SUCCESS);
    return SRSRAN_SUCCESS;
  }

  // Run Simulation
  for (uint32_t i = 0; i < test_args.duration and err_code >= SRSRAN_SUCCESS; i++) {
    err_code = test_bench->run_tti();
  }

  test_bench->stop();

  srslog::flush();

  if (err_code >= SRSRAN_SUCCESS) {
    std::cout << "Ok" << std::endl;
  } else {
    std::cout << "Error" << std::endl;
  }

  return err_code;
}
