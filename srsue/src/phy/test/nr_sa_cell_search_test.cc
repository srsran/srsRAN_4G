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

#include "gnb_rf_emulator.h"
#include "srsran/asn1/rrc_nr.h"
#include "srsran/common/band_helper.h"
#include "srsran/common/crash_handler.h"
#include "srsran/common/string_helpers.h"
#include "srsue/hdr/phy/phy_nr_sa.h"
#include "test/phy/dummy_ue_stack.h"
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>

class throttled_dummy_stack : public ue_dummy_stack
{
private:
  bool                    pending_tti = false;
  std::mutex              pending_tti_mutex;
  std::condition_variable pending_tti_cvar;
  std::atomic<bool>       running = {true};

public:
  throttled_dummy_stack(const ue_dummy_stack::args_t& args, srsue::phy_interface_stack_nr& phy) :
    ue_dummy_stack(args, phy)
  {}
  void wait_tti() override
  {
    // Wait for tick
    std::unique_lock<std::mutex> lock(pending_tti_mutex);
    while (not pending_tti and running) {
      pending_tti_cvar.wait(lock);
    }

    // Let the tick proceed
    pending_tti = false;
    pending_tti_cvar.notify_all();
  }

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
  // Generic parameters
  double                      srate_hz        = 11.52e6;
  srsran_carrier_nr_t         base_carrier    = SRSRAN_DEFAULT_CARRIER_NR;
  srsran_ssb_patern_t         ssb_pattern     = SRSRAN_SSB_PATTERN_A;
  srsran_subcarrier_spacing_t ssb_scs         = srsran_subcarrier_spacing_15kHz;
  srsran_duplex_mode_t        duplex_mode     = SRSRAN_DUPLEX_MODE_FDD;
  uint32_t                    duration_ms     = 1000;
  std::string                 phy_log_level   = "warning";
  std::string                 stack_log_level = "warning";

  // Simulation parameters
  uint32_t           sim_ssb_periodicity_ms = 10;
  std::set<uint32_t> sim_cell_pci;

  // RF parameters
  std::string rf_device_name    = "auto";
  std::string rf_device_args    = "auto";
  std::string rf_log_level      = "info";
  float       rf_rx_gain_dB     = 20.0f;
  float       rf_freq_offset_Hz = 0.0f;

  void set_ssb_from_band()
  {
    srsran::srsran_band_helper bands;

    // Deduce band number
    uint16_t band = bands.get_band_from_dl_freq_Hz(base_carrier.dl_center_frequency_hz);
    srsran_assert(band != UINT16_MAX, "Invalid band");

    // Deduce point A in Hz
    double pointA_Hz =
        bands.get_abs_freq_point_a_from_center_freq(base_carrier.nof_prb, base_carrier.dl_center_frequency_hz);

    // Deduce DL center frequency ARFCN
    uint32_t pointA_arfcn = bands.freq_to_nr_arfcn(pointA_Hz);
    srsran_assert(pointA_arfcn != 0, "Invalid frequency");

    // Select a valid SSB subcarrier spacing
    ssb_scs = bands.get_ssb_scs(band);

    // Deduce SSB center frequency ARFCN
    uint32_t ssb_arfcn = bands.get_abs_freq_ssb_arfcn(band, ssb_scs, pointA_arfcn);
    srsran_assert(ssb_arfcn, "Invalid SSB center frequency");

    duplex_mode                     = bands.get_duplex_mode(band);
    ssb_pattern                     = bands.get_ssb_pattern(band, ssb_scs);
    base_carrier.ssb_center_freq_hz = bands.nr_arfcn_to_freq(ssb_arfcn);
  }
};

// shorten boost program options namespace
namespace bpo = boost::program_options;

static void pci_list_parse_helper(std::string& list_str, std::set<uint32_t>& list)
{
  if (list_str == "all") {
    // Add all possible cells
    for (int i = 0; i < 504; i++) {
      list.insert(i);
    }
  } else if (list_str == "none") {
    // Do nothing
  } else if (not list_str.empty()) {
    // Remove spaces from neightbour cell list
    list_str = srsran::string_remove_char(list_str, ' ');

    // Add cell to known cells
    srsran::string_parse_list(list_str, ',', list);
  }
}

int parse_args(int argc, char** argv, args_t& args)
{
  int ret = SRSRAN_SUCCESS;

  std::string simulation_cell_list = "";

  bpo::options_description options("General options");
  bpo::options_description phy("Physical layer options");
  bpo::options_description stack("Stack options");
  bpo::options_description over_the_air("Mode 1: Over the air options (Default)");
  bpo::options_description simulation("Mode 2: Simulation options (enabled if simulation_cell_list is not empty)");

  // clang-format off
  over_the_air.add_options()
      ("rf.device_name", bpo::value<std::string>(&args.rf_device_name)->default_value(args.rf_device_name), "RF Device Name")
      ("rf.device_args", bpo::value<std::string>(&args.rf_device_args)->default_value(args.rf_device_args), "RF Device arguments")
      ("rf.log_level",   bpo::value<std::string>(&args.rf_log_level)->default_value(args.rf_log_level),     "RF Log level (none, warning, info, debug)")
      ("rf.rx_gain",     bpo::value<float>(&args.rf_rx_gain_dB)->default_value(args.rf_rx_gain_dB),                           "RF Receiver gain in dB")
      ("rf.freq_offset",     bpo::value<float>(&args.rf_freq_offset_Hz)->default_value(args.rf_freq_offset_Hz),                           "RF Frequency offset")
      ;

  simulation.add_options()
      ("sim.pci_list", bpo::value<std::string>(&simulation_cell_list)->default_value(simulation_cell_list), "Comma separated PCI cell list to simulate")
      ("sim.bw", bpo::value<uint32_t>(&args.base_carrier.nof_prb)->default_value(args.base_carrier.nof_prb), "Carrier bandwidth in RB")
      ("sim.ssb_period",           bpo::value<uint32_t>(&args.sim_ssb_periodicity_ms)->default_value(args.sim_ssb_periodicity_ms),        "SSB period in ms")
      ;

  phy.add_options()
      ("phy.srate", bpo::value<double>(&args.srate_hz)->default_value(args.srate_hz), "Sampling Rate in Hz")
      ("phy.log.level", bpo::value<std::string>(&args.phy_log_level)->default_value(args.phy_log_level), "Physical layer logging level")
      ;

  stack.add_options()
      ("stack.log.level", bpo::value<std::string>(&args.stack_log_level)->default_value(args.stack_log_level), "Stack logging level")
      ;

  options.add(over_the_air).add(simulation).add(phy).add(stack).add_options()
      ("help,h",        "Show this message")
      ("duration",      bpo::value<uint32_t>(&args.duration_ms)->default_value(args.duration_ms),     "Duration of the test in milli-seconds")
      ("freq_dl", bpo::value<double>(&args.base_carrier.dl_center_frequency_hz)->default_value(args.base_carrier.dl_center_frequency_hz), "Carrier center frequency in Hz")
      ;
  // clang-format on

  bpo::variables_map vm;
  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(options).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error& e) {
    std::cerr << e.what() << std::endl;
    ret = SRSRAN_ERROR;
  }

  // help option was given or error - print usage and exit
  if (vm.count("help") || ret) {
    std::cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << std::endl << std::endl;
    std::cout << options << std::endl << std::endl;
    ret = SRSRAN_ERROR;
  }

  // Parse PCI lists
  pci_list_parse_helper(simulation_cell_list, args.sim_cell_pci);

  args.set_ssb_from_band();

  return ret;
}

class dummy_ue
{
private:
  throttled_dummy_stack stack;
  srsue::phy_nr_sa      phy;

public:
  struct args_t {
    srsue::phy_args_nr_t     phy;
    ue_dummy_stack::args_t   stack;
  };
  dummy_ue(const args_t& args, srsran::radio_interface_phy* radio) : stack(args.stack, phy), phy("PHY")
  {
    srsran_assert(phy.init(args.phy, &stack, radio), "Failed to initialise PHY");
  }

  bool start_cell_search(const srsue::phy_interface_stack_nr::cell_search_args_t& args)
  {
    return phy.start_cell_search(args);
  }

  void run_tti() { stack.tick(); }
  void stop()
  {
    // First transition PHY to IDLE
    phy.reset_nr();

    // Make sure PHY transitioned to IDLE
    // ...

    // Stop stack, it will let PHY free run
    stack.stop();

    // Stop PHY
    phy.stop();
  }
};

int main(int argc, char** argv)
{
  srsran_debug_handle_crash(argc, argv);

  // Parse Test arguments
  args_t args;
  srsran_assert(parse_args(argc, argv, args) == SRSRAN_SUCCESS, "Failed to parse arguments");

  // Initialise logging infrastructure
  srslog::init();

  // Radio can be constructed from different options
  std::shared_ptr<srsran::radio_interface_phy> radio = nullptr;

  // Build radio
  if (not args.sim_cell_pci.empty()) {
    // Create Radio as gNb emulator if the device RF name is not defined
    gnb_rf_emulator::args_t gnb_args = {};
    gnb_args.srate_hz                = args.srate_hz;
    gnb_args.base_carrier            = args.base_carrier;
    gnb_args.ssb_pattern             = args.ssb_pattern;
    gnb_args.ssb_periodicity_ms      = args.sim_ssb_periodicity_ms;
    gnb_args.ssb_scs                 = args.ssb_scs;
    gnb_args.duplex_mode             = args.duplex_mode;
    gnb_args.pci_list                = args.sim_cell_pci;

    radio = std::make_shared<gnb_rf_emulator>(gnb_args);
  } else {
    // Create an actual radio based on RF
    srsran::rf_args_t rf_args = {};
    rf_args.type              = "multi";
    rf_args.log_level         = args.rf_log_level;
    rf_args.srate_hz          = args.srate_hz;
    rf_args.rx_gain           = args.rf_rx_gain_dB;
    rf_args.nof_carriers      = 1;
    rf_args.nof_antennas      = 1;
    rf_args.device_args       = args.rf_device_args;
    rf_args.device_name       = args.rf_device_name;
    rf_args.freq_offset       = args.rf_freq_offset_Hz;

    // Instantiate
    std::shared_ptr<srsran::radio> r = std::make_shared<srsran::radio>();
    srsran_assert(r->init(rf_args, nullptr) == SRSRAN_SUCCESS, "Failed Radio initialisation");

    // Move to base pointer
    radio = std::move(r);

    // Set sampling rate
    radio->set_rx_srate(args.srate_hz);

    // Set DL center frequency
    radio->set_rx_freq(0, args.base_carrier.dl_center_frequency_hz);

    // Set Rx gain
    radio->set_rx_gain(args.rf_rx_gain_dB);
  }

  // Create dummy UE
  dummy_ue::args_t ue_args = {};
  ue_args.phy.log.phy_level = args.phy_log_level;
  ue_args.stack.log_level  = args.stack_log_level;
  dummy_ue ue(ue_args, radio.get());

  // Transition PHY to cell search
  srsue::phy_nr_sa::cell_search_args_t cell_search_req = {};
  cell_search_req.srate_hz                             = args.srate_hz;
  cell_search_req.center_freq_hz                       = args.base_carrier.dl_center_frequency_hz;
  cell_search_req.ssb_freq_hz                          = args.base_carrier.ssb_center_freq_hz;
  cell_search_req.ssb_scs                              = args.ssb_scs;
  cell_search_req.ssb_pattern                          = args.ssb_pattern;
  cell_search_req.duplex_mode                          = args.duplex_mode;
  srsran_assert(ue.start_cell_search(cell_search_req), "Failed cell search start");

  for (uint32_t i = 0; i < args.duration_ms; i++) {
    ue.run_tti();
  }

  // Tear down UE
  ue.stop();

  // Stop Radio
  radio->reset();

  // Erase radio
  radio = nullptr;

  return 0;
}