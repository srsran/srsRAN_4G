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

#include "srsran/common/band_helper.h"
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
  uint32_t              slot_idx       = 0;
  srsran_carrier_nr_t   carrier        = {};
  std::atomic<bool>     running        = {true};
  srslog::basic_logger& logger;

  void run_async_slot()
  {
    // Early return if not running
    if (not running) {
      return;
    }

    // Zero slot buffer
    srsran_vec_cf_zero(buffer, sf_len);

    if (srsran_ssb_send(&ssb, slot_idx)) {
      // Create MIB for this slot
      srsran_mib_nr_t mib = {};

      // Create PBCH message from packed MIB
      srsran_pbch_msg_nr_t pbch_msg = {};
      srsran_assert(srsran_pbch_msg_nr_mib_pack(&mib, &pbch_msg) >= SRSRAN_SUCCESS,
                    "Error packing MIB into PBCH message");

      // Encode SSB signal and add it to the baseband
      srsran_assert(srsran_ssb_add(&ssb, carrier.pci, &pbch_msg, buffer, buffer) >= SRSRAN_SUCCESS,
                    "Error adding SSB signal");
    }
    slot_idx++;

    // Write slot samples in ringbuffer
    srsran_assert(srsran_ringbuffer_write(&ringbuffer, buffer, (int)sizeof(cf_t) * sf_len) > SRSRAN_SUCCESS,
                  "Error writing in ringbuffer");
  }

public:
  struct args_t {
    double                      srate_hz;
    srsran_carrier_nr_t         carrier;
    srsran_subcarrier_spacing_t ssb_scs;
    srsran_ssb_patern_t         ssb_pattern;
    uint32_t                    ssb_periodicity_ms;
    srsran_duplex_mode_t        duplex_mode;
  };
  gnb_emulator(const args_t& args) : logger(srslog::fetch_basic_logger(LOGNAME))
  {
    srsran_assert(
        std::isnormal(args.srate_hz) and args.srate_hz > 0, "Invalid sampling rate (%.2f MHz)", args.srate_hz);

    sf_len  = args.srate_hz / 1000;
    carrier = args.carrier;

    srsran_ssb_args_t ssb_args = {};
    ssb_args.enable_encode     = true;
    srsran_assert(srsran_ssb_init(&ssb, &ssb_args) == SRSRAN_SUCCESS, "SSB initialisation failed");

    srsran_ssb_cfg_t ssb_cfg = {};
    ssb_cfg.srate_hz         = args.srate_hz;
    ssb_cfg.center_freq_hz   = args.carrier.dl_center_frequency_hz;
    ssb_cfg.ssb_freq_hz      = args.carrier.ssb_center_freq_hz;
    ssb_cfg.scs              = args.ssb_scs;
    ssb_cfg.pattern          = args.ssb_pattern;
    ssb_cfg.duplex_mode      = args.duplex_mode;
    ssb_cfg.periodicity_ms   = args.ssb_periodicity_ms;
    srsran_assert(srsran_ssb_set_cfg(&ssb, &ssb_cfg) == SRSRAN_SUCCESS, "SSB set config failed");

    srsran_assert(srsran_ringbuffer_init(&ringbuffer, sizeof(cf_t) * BUFFER_SIZE_SF * sf_len) >= SRSRAN_SUCCESS,
                  "Ringbuffer initialisation failed");

    buffer = srsran_vec_cf_malloc(BUFFER_SIZE_SF * sf_len);
  }
  ~gnb_emulator()
  {
    srsran_ssb_free(&ssb);
    srsran_ringbuffer_free(&ringbuffer);
    if (buffer != nullptr) {
      free(buffer);
    }
  }
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
      temp_buffer = buffer;
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

class dummy_stack : public srsue::stack_interface_phy_sa_nr
{
private:
  srslog::basic_logger&   logger      = srslog::fetch_basic_logger("STACK");
  bool                    pending_tti = false;
  std::mutex              pending_tti_mutex;
  std::condition_variable pending_tti_cvar;
  std::atomic<bool>       running = {true};

public:
  dummy_stack() { logger.set_level(srslog::str_to_basic_level("info")); }
  void in_sync() override {}
  void out_of_sync() override {}
  void run_tti(const uint32_t tti) override
  {
    logger.debug("Run TTI %d", tti);

    // Wait for tick
    std::unique_lock<std::mutex> lock(pending_tti_mutex);
    while (not pending_tti and running) {
      pending_tti_cvar.wait(lock);
    }

    // Let the tick proceed
    pending_tti = false;
    pending_tti_cvar.notify_all();
  }
  void cell_search_found_cell(const cell_search_result_t& result) override
  {
    std::array<char, 512> csi_info = {};

    srsran_csi_meas_info(&result.measurements, csi_info.data(), (uint32_t)csi_info.size());

    logger.info("Cell found pci=%d barred=%c intra_freq=%c %s",
                result.pci,
                result.barred ? 'y' : 'n',
                result.intra_freq_meas ? 'y' : 'n',
                csi_info.data());
  }
  int sf_indication(const uint32_t tti) override
  {
    logger.info("SF %d indication", tti);
    return 0;
  }
  sched_rnti_t get_dl_sched_rnti_nr(const uint32_t tti) override { return sched_rnti_t(); }
  sched_rnti_t get_ul_sched_rnti_nr(const uint32_t tti) override { return sched_rnti_t(); }
  void         new_grant_dl(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_t* action) override {}
  void tb_decoded(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_result_t result) override {}
  void new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant, tb_action_ul_t* action) override {}
  void prach_sent(uint32_t tti, uint32_t s_id, uint32_t t_id, uint32_t f_id, uint32_t ul_carrier_id) override {}
  bool sr_opportunity(uint32_t tti, uint32_t sr_id, bool meas_gap, bool ul_sch_tx) override { return false; }

  void tick()
  {
    // Wait for TTI to get processed
    std::unique_lock<std::mutex> lock(pending_tti_mutex);
    while (pending_tti) {
      pending_tti_cvar.wait(lock);
    }

    // Let the TTI proceed
    pending_tti = true;
    pending_tti_cvar.notify_all();
  }

  void stop()
  {
    running = false;
    pending_tti_cvar.notify_all();
  }
};

struct args_t {
  double                      srate_hz           = 11.52e6;
  srsran_carrier_nr_t         carrier            = SRSRAN_DEFAULT_CARRIER_NR;
  srsran_ssb_patern_t         ssb_pattern        = SRSRAN_SSB_PATTERN_A;
  uint32_t                    ssb_periodicity_ms = 10;
  srsran_subcarrier_spacing_t ssb_scs            = srsran_subcarrier_spacing_15kHz;
  srsran_duplex_mode_t        duplex_mode        = SRSRAN_DUPLEX_MODE_FDD;
  uint32_t                    duration_ms        = 1000;

  void set_ssb_from_band(uint16_t band)
  {
    srsran::srsran_band_helper bands;
    duplex_mode                = bands.get_duplex_mode(band);
    ssb_scs                    = bands.get_ssb_scs(band);
    ssb_pattern                = bands.get_ssb_pattern(band, ssb_scs);
    carrier.ssb_center_freq_hz = bands.get_ssb_center_freq(carrier);
  }
};

int main(int argc, char** argv)
{
  // Parse Test arguments
  args_t args;

  // Initialise logging infrastructure
  srslog::init();

  // Radio can be constructed from different options
  std::shared_ptr<srsran::radio_interface_phy> radio = nullptr;

  // Create Radio as gNb emulator
  gnb_emulator::args_t gnb_args = {};
  gnb_args.srate_hz             = args.srate_hz;
  gnb_args.carrier              = args.carrier;
  gnb_args.ssb_pattern          = args.ssb_pattern;
  gnb_args.ssb_periodicity_ms   = args.ssb_periodicity_ms;
  gnb_args.duplex_mode          = args.duplex_mode;
  radio                         = std::make_shared<gnb_emulator>(gnb_args);

  // Create stack
  dummy_stack stack;

  // Create UE PHY
  srsue::phy_nr_sa phy(stack, *radio);

  // Initialise PHY, it will instantly start free running
  srsue::phy_nr_sa::args_t phy_args = {};
  phy_args.srate_hz                 = args.srate_hz;
  phy.init(phy_args);

  // Transition PHY to cell search
  srsue::phy_nr_sa::cell_search_args_t cell_search_req = {};
  cell_search_req.center_freq_hz                       = args.carrier.dl_center_frequency_hz;
  cell_search_req.ssb_freq_hz                          = args.carrier.ssb_center_freq_hz;
  cell_search_req.ssb_scs                              = args.ssb_scs;
  cell_search_req.ssb_pattern                          = args.ssb_pattern;
  cell_search_req.duplex_mode                          = args.duplex_mode;
  phy.start_cell_search(cell_search_req);

  for (uint32_t i = 0; i < args.duration_ms; i++) {
    stack.tick();
  }

  // First transition PHY to IDLE
  phy.reset();

  // Make sure PHY transitioned to IDLE
  // ...

  // Stop stack, it will let PHY free run
  stack.stop();

  // Stop PHY
  phy.stop();

  // Stop Radio
  radio->reset();

  return 0;
}