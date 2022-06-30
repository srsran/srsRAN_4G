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

#include "gnb_rf_emulator.h"
#include "srsran/common/band_helper.h"
#include "srsran/common/crash_handler.h"
#include "srsran/common/string_helpers.h"
#include "srsue/hdr/phy/phy_nr_sa.h"
#include "test/phy/dummy_ue_stack.h"
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>

struct args_t {
  // Generic parameters
  double                      srate_hz        = 11.52e6;
  srsran_carrier_nr_t         base_carrier    = SRSRAN_DEFAULT_CARRIER_NR;
  srsran_ssb_pattern_t        ssb_pattern     = SRSRAN_SSB_PATTERN_A;
  srsran_subcarrier_spacing_t ssb_scs         = srsran_subcarrier_spacing_15kHz;
  srsran_duplex_mode_t        duplex_mode     = SRSRAN_DUPLEX_MODE_FDD;
  uint32_t                    duration_ms     = 1000;
  std::string                 phy_log_level   = "warning";
  std::string                 stack_log_level = "warning";

  // Simulation parameters
  uint32_t           sim_ssb_periodicity_ms   = 10;
  float              sim_channel_hst_fd_hz    = 0.0f;
  float              sim_channel_hst_period_s = 7.2f;
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
     ("sim.channel.hst.fd",           bpo::value<float>(&args.sim_channel_hst_fd_hz)->default_value(args.sim_channel_hst_fd_hz),        "Channel emulator HST maximum frequency")
     ("sim.channel.hst.period",           bpo::value<float>(&args.sim_channel_hst_period_s)->default_value(args.sim_channel_hst_period_s),        "Channel emulator HST period")
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
  ue_dummy_stack   stack;
  srsue::phy_nr_sa phy;

public:
  struct args_t {
    srsue::phy_args_nr_t   phy;
    ue_dummy_stack::args_t stack;
  };
  dummy_ue(const args_t& args, srsran::radio_interface_phy* radio) : stack(args.stack, phy), phy("PHY")
  {
    srsran_assert(phy.init(args.phy, &stack, radio) == SRSRAN_SUCCESS, "Failed to initialise PHY");
    phy.wait_initialize();
  }

  bool cell_search_read_and_clear() { return stack.get_cell_search_finished(); }

  bool start_cell_search(const srsue::phy_interface_stack_nr::cell_search_args_t& args)
  {
    return phy.start_cell_search(args);
  }

  bool start_cell_select(const srsue::phy_interface_stack_nr::cell_select_args_t& args)
  {
    return phy.start_cell_select(args);
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

  const ue_dummy_stack::metrics_t& get_metrics() const { return stack.get_metrics(); }
  void                             reset_metrics() { stack.reset_metrics(); }
};

struct cell_search_result_t {
  bool                        found           = false;
  double                      ssb_abs_freq_hz = 0.0f;
  srsran_subcarrier_spacing_t ssb_scs         = srsran_subcarrier_spacing_15kHz;
  srsran_ssb_pattern_t        ssb_pattern     = SRSRAN_SSB_PATTERN_A;
  srsran_duplex_mode_t        duplex_mode     = SRSRAN_DUPLEX_MODE_FDD;
  srsran_mib_nr_t             mib             = {};
  uint32_t                    pci             = 0;
};

/*
 * The following function searches for cells in all possible SSB absolute frequencies within the baseband range. It
 * returns the first found cell.
 */
static cell_search_result_t cell_search(const args_t& args, dummy_ue& ue)
{
  cell_search_result_t ret = {};

  // Base cell search arguments
  srsue::phy_nr_sa::cell_search_args_t cs_args = {};
  cs_args.center_freq_hz                       = args.base_carrier.dl_center_frequency_hz;
  cs_args.ssb_scs                              = args.ssb_scs;
  cs_args.ssb_pattern                          = args.ssb_pattern;
  cs_args.duplex_mode                          = args.duplex_mode;

  // Deduce band number
  srsran::srsran_band_helper bands;
  uint16_t                   band = bands.get_band_from_dl_freq_Hz(args.base_carrier.dl_center_frequency_hz);
  srsran_assert(band != UINT16_MAX, "Invalid band");

  // Calculate SSB center frequency boundaries
  double   ssb_bw_hz              = SRSRAN_SSB_BW_SUBC * bands.get_ssb_scs(band);
  double   ssb_center_freq_min_hz = args.base_carrier.dl_center_frequency_hz - (args.srate_hz * 0.7 - ssb_bw_hz) / 2.0;
  double   ssb_center_freq_max_hz = args.base_carrier.dl_center_frequency_hz + (args.srate_hz * 0.7 - ssb_bw_hz) / 2.0;
  uint32_t ssb_scs_hz             = SRSRAN_SUBC_SPACING_NR(args.ssb_scs);

  // Get sync raster
  srsran::srsran_band_helper::sync_raster_t ss = bands.get_sync_raster(band, args.ssb_scs);
  srsran_assert(ss.valid(), "Invalid synchronization raster");

  // Iterate every possible frequency in the synchronization raster
  while (not ss.end()) {
    // Get SSB center frequency
    cs_args.ssb_freq_hz = ss.get_frequency();

    // Advance SSB frequency raster
    ss.next();

    // Calculate frequency offset between the base-band center frequency and the SSB absolute frequency
    uint32_t offset_hz = (uint32_t)std::abs(std::round(cs_args.ssb_freq_hz - args.base_carrier.dl_center_frequency_hz));

    // The SSB absolute frequency is invalid if it is outside the range and the offset is NOT multiple of the subcarrier
    // spacing
    if ((cs_args.ssb_freq_hz < ssb_center_freq_min_hz) or (cs_args.ssb_freq_hz > ssb_center_freq_max_hz) or
        (offset_hz % ssb_scs_hz != 0)) {
      // Skip this frequency
      continue;
    }

    // Transition PHY to cell search
    srsran_assert(ue.start_cell_search(cs_args), "Failed cell search start");

    // Run slot until the PHY reported to the stack
    while (not ue.cell_search_read_and_clear()) {
      ue.run_tti();
    }

    const ue_dummy_stack::metrics_t& metrics = ue.get_metrics();

    // Skip printing cell search findings if no SSB is found
    if (metrics.cell_search.empty()) {
      continue;
    }

    // Print found cells
    printf("Cells found at SSB center frequency %.2f MHz:\n", cs_args.ssb_freq_hz / 1e6);
    printf("| %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s |\n",
           "PCI",
           "SSB",
           "Count",
           "RSRP min",
           "RSRP avg",
           "RSRP max",
           "SNR min",
           "SNR avg",
           "SNR max",
           "CFO min",
           "CFO avg",
           "CFO max");

    // For each found PCI...
    for (auto& pci : metrics.cell_search) {
      // For each found beam...
      for (auto& ssb : pci.second) {
        // Print stats
        printf("| %10d | %10d | %10d | %+10.1f | %+10.1f | %+10.1f | %+10.1f | %+10.1f | %+10.1f | %+10.1f | %+10.1f | "
               "%+10.1f |\n",
               pci.first,
               ssb.first,
               (uint32_t)ssb.second.count,
               ssb.second.rsrp_db_min,
               ssb.second.rsrp_db_avg,
               ssb.second.rsrp_db_max,
               ssb.second.snr_db_min,
               ssb.second.snr_db_avg,
               ssb.second.snr_db_max,
               ssb.second.cfo_hz_min,
               ssb.second.cfo_hz_avg,
               ssb.second.cfo_hz_max);

        // If this is the first found cell, then set return value
        if (not ret.found) {
          ret.found           = true;
          ret.ssb_abs_freq_hz = cs_args.ssb_freq_hz;
          ret.ssb_scs         = cs_args.ssb_scs;
          ret.ssb_pattern     = cs_args.ssb_pattern;
          ret.duplex_mode     = cs_args.duplex_mode;
          ret.pci             = pci.first;
          srsran_assert(srsran_pbch_msg_nr_mib_unpack(&ssb.second.last_result.pbch_msg, &ret.mib) == SRSRAN_SUCCESS,
                        "Error unpacking MIB");
        }
      }
    }

    // Reset stack metrics
    ue.reset_metrics();
  }

  return ret;
}

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
    gnb_args.channel_hst_fd_hz       = args.sim_channel_hst_fd_hz;
    gnb_args.channel_hst_period_s    = args.sim_channel_hst_period_s;

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
  dummy_ue::args_t ue_args  = {};
  ue_args.phy.srate_hz      = args.srate_hz;
  ue_args.phy.log.phy_level = args.phy_log_level;
  ue_args.stack.log_level   = args.stack_log_level;
  dummy_ue ue(ue_args, radio.get());

  // Perform cell search
  cell_search_result_t found_cell = cell_search(args, ue);

  // Perform cell select
  if (found_cell.found) {
    srsue::phy_interface_stack_nr::cell_select_args_t cs_args = {};
    cs_args.ssb_cfg.srate_hz                                  = args.srate_hz;
    cs_args.ssb_cfg.center_freq_hz                            = args.base_carrier.dl_center_frequency_hz;
    cs_args.ssb_cfg.ssb_freq_hz                               = found_cell.ssb_abs_freq_hz;
    cs_args.ssb_cfg.scs                                       = found_cell.ssb_scs;
    cs_args.ssb_cfg.pattern                                   = found_cell.ssb_pattern;
    cs_args.ssb_cfg.duplex_mode                               = found_cell.duplex_mode;
    cs_args.ssb_cfg.periodicity_ms                            = 10;
    cs_args.carrier                                           = args.base_carrier;
    cs_args.carrier.pci                                       = found_cell.pci;

    srsran_assert(ue.start_cell_select(cs_args), "Failed to start cell selection\n");

    for (uint32_t i = 0; i < 1000; i++) {
      ue.run_tti();
    }
  }

  // Tear down UE
  ue.stop();

  for (uint32_t i = 0; i < 1000; i++) {
    ue.run_tti();
  }

  // Stop Radio
  radio->reset();

  return 0;
}