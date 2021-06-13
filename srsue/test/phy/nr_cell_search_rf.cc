/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/common/band_helper.h"
#include "srsran/common/string_helpers.h"
#include "srsran/common/test_common.h"
#include "srsran/interfaces/phy_interface_types.h"
#include "srsran/radio/radio.h"
#include "srsran/srslog/srslog.h"
#include "srsue/hdr/phy/scell/intra_measure_nr.h"
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

struct args_t {
  // General
  std::string log_level = "warning";
  double      srate_hz  = 23.04e6;

  // Measurement parameters
  uint32_t meas_len_ms    = 20;
  uint32_t meas_period_ms = 20;
  float    thr_snr_db     = -5.0f;

  // Radio parameters
  std::string radio_device_name = "auto";
  std::string radio_device_args = "auto";
  std::string radio_log_level   = "info";
  float       rx_gain           = 80.0f;
  double      freq_offset_hz    = 0;
  std::string bands             = "78";
};

class meas_itf_listener : public srsue::scell::intra_measure_base::meas_itf
{
public:
  typedef struct {
    float    rsrp_avg;
    float    rsrp_min;
    float    rsrp_max;
    float    rsrq_avg;
    float    rsrq_min;
    float    rsrq_max;
    uint32_t arfcn;
    uint32_t count;
  } cell_meas_t;

  std::map<uint32_t, cell_meas_t> cells;

  void cell_meas_reset(uint32_t cc_idx) override {}
  void new_cell_meas(uint32_t cc_idx, const std::vector<srsue::phy_meas_t>& meas) override
  {
    for (const srsue::phy_meas_t& m : meas) {
      uint32_t pci = m.pci;
      if (!cells.count(pci)) {
        cells[pci].rsrp_min = m.rsrp;
        cells[pci].rsrp_max = m.rsrp;
        cells[pci].rsrp_avg = m.rsrp;
        cells[pci].rsrq_min = m.rsrq;
        cells[pci].rsrq_max = m.rsrq;
        cells[pci].rsrq_avg = m.rsrq;
        cells[pci].count    = 1;
      } else {
        cells[pci].rsrp_min = SRSRAN_MIN(cells[pci].rsrp_min, m.rsrp);
        cells[pci].rsrp_max = SRSRAN_MAX(cells[pci].rsrp_max, m.rsrp);
        cells[pci].rsrp_avg = (m.rsrp + cells[pci].rsrp_avg * cells[pci].count) / (cells[pci].count + 1);

        cells[pci].rsrq_min = SRSRAN_MIN(cells[pci].rsrq_min, m.rsrq);
        cells[pci].rsrq_max = SRSRAN_MAX(cells[pci].rsrq_max, m.rsrq);
        cells[pci].rsrq_avg = (m.rsrq + cells[pci].rsrq_avg * cells[pci].count) / (cells[pci].count + 1);
        cells[pci].count++;
      }
      cells[pci].arfcn = m.earfcn;
    }
  }

  void print_stats()
  {
    printf("\n-- Statistics:\n");
    for (auto& e : cells) {
      printf("  pci=%03d; arfcn=%d; count=%3d; rsrp=%+.1f|%+.1f|%+.1fdBfs;  rsrq=%+.1f|%+.1f|%+.1fdB;\n",
             e.first,
             e.second.arfcn,
             e.second.count,
             e.second.rsrp_min,
             e.second.rsrp_avg,
             e.second.rsrp_max,
             e.second.rsrq_min,
             e.second.rsrq_avg,
             e.second.rsrq_max);
    }
  }
};

// shorten boost program options namespace
namespace bpo = boost::program_options;

int parse_args(int argc, char** argv, args_t& args)
{
  int ret = SRSRAN_SUCCESS;

  bpo::options_description options("General options");
  bpo::options_description measure("Measurement options");
  bpo::options_description over_the_air("Mode 1: Over the air options (Default)");

  // clang-format off
  measure.add_options()
      ("meas_len_ms",      bpo::value<uint32_t>(&args.meas_len_ms)->default_value(args.meas_len_ms),       "Measurement length")
      ("meas_period_ms",   bpo::value<uint32_t>(&args.meas_period_ms)->default_value(args.meas_period_ms), "Measurement period")
      ("thr_snr_db",       bpo::value<float>(&args.thr_snr_db)->default_value(args.thr_snr_db),            "Detection threshold for SNR in dB")
      ("bands",       bpo::value<std::string>(&args.bands)->default_value(args.bands),            "band list to measure, comma separated")
      ;

  over_the_air.add_options()
      ("rf.device_name", bpo::value<std::string>(&args.radio_device_name)->default_value(args.radio_device_name), "RF Device Name")
      ("rf.device_args", bpo::value<std::string>(&args.radio_device_args)->default_value(args.radio_device_args), "RF Device arguments")
      ("rf.log_level",   bpo::value<std::string>(&args.radio_log_level)->default_value(args.radio_log_level),     "RF Log level (none, warning, info, debug)")
      ("rf.rx_gain",     bpo::value<float>(&args.rx_gain)->default_value(args.rx_gain),                           "RF Receiver gain in dB")
      ("rf.freq_offset",     bpo::value<double>(&args.freq_offset_hz)->default_value(args.freq_offset_hz),                           "RF frequency offset in Hz")
      ;

  options.add(measure).add(over_the_air).add_options()
      ("help,h",        "Show this message")
      ("log_level",     bpo::value<std::string>(&args.log_level)->default_value(args.log_level),    "Intra measurement log level (none, warning, info, debug)")
      ("srate",         bpo::value<double>(&args.srate_hz)->default_value(args.srate_hz),           "Sampling Rate in Hz")
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

  return ret;
}

int main(int argc, char** argv)
{
  // Parse args
  args_t args = {};
  if (parse_args(argc, argv, args) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Initiate logging
  srslog::init();
  srslog::basic_logger& logger = srslog::fetch_basic_logger("PHY");
  logger.set_level(srslog::str_to_basic_level(args.log_level));

  // Deduce base-band parameters
  uint32_t sf_len = (uint32_t)round(args.srate_hz / 1000.0);

  // Allocate buffer
  std::vector<cf_t> baseband_buffer(sf_len);

  // Create measurement callback
  meas_itf_listener rrc;

  // Create measurement object
  srsue::scell::intra_measure_nr intra_measure(logger, rrc);

  // Initialise measurement instance
  srsue::scell::intra_measure_nr::args_t meas_args = {};
  meas_args.rx_gain_offset_dB                      = 0.0f;
  meas_args.max_len_ms                             = args.meas_len_ms;
  meas_args.max_srate_hz                           = args.srate_hz;
  meas_args.min_scs                                = srsran_subcarrier_spacing_15kHz;
  meas_args.thr_snr_db                             = args.thr_snr_db;
  TESTASSERT(intra_measure.init(0, meas_args));

  std::set<srsran_subcarrier_spacing_t> scs_set  = {srsran_subcarrier_spacing_15kHz, srsran_subcarrier_spacing_30kHz};
  std::set<uint16_t>                    band_set = {};

  srsran::string_parse_list(args.bands, ',', band_set);

  // Create Radio
  srsran::radio radio;

  auto& radio_logger = srslog::fetch_basic_logger("RF", false);
  radio_logger.set_level(srslog::str_to_basic_level(args.radio_log_level));

  // Init radio
  srsran::rf_args_t radio_args = {};
  radio_args.device_args       = args.radio_device_args;
  radio_args.device_name       = args.radio_device_name;
  radio_args.nof_carriers      = 1;
  radio_args.nof_antennas      = 1;
  radio.init(radio_args, nullptr);

  // Set sampling rate
  radio.set_rx_srate(args.srate_hz);
  radio.set_rx_gain(args.rx_gain);

  double   center_freq_hz = 0.0;
  uint32_t tti_count      = 0;

  // Iterate
  for (const uint16_t& band : band_set) {
    for (const srsran_subcarrier_spacing_t& scs : scs_set) {
      srsran::srsran_band_helper::sync_raster_t sync_raster = srsran::srsran_band_helper().get_sync_raster(band, scs);

      // Iterate over all GSCN
      for (; not sync_raster.end(); sync_raster.next()) {
        double ssb_freq_hz = sync_raster.get_frequency();

        // Set frequency if the deviation from the current frequency is too high
        if (std::abs(center_freq_hz - ssb_freq_hz) > (args.srate_hz / 2.0)) {
          center_freq_hz = ssb_freq_hz + args.srate_hz / 2.0;

          // Update Rx frequency
          radio.set_rx_freq(0, center_freq_hz + args.freq_offset_hz);
        }

        logger.info("Measuring SSB frequency %.2f MHz, center %.2f MHz", ssb_freq_hz / 1e6, center_freq_hz / 1e6);

        // Setup measurement
        srsue::scell::intra_measure_nr::config_t meas_cfg = {};
        meas_cfg.arfcn                                    = (uint32_t)(ssb_freq_hz / 1e3);
        meas_cfg.srate_hz                                 = args.srate_hz;
        meas_cfg.len_ms                                   = args.meas_len_ms;
        meas_cfg.period_ms                                = args.meas_period_ms;
        meas_cfg.center_freq_hz                           = center_freq_hz;
        meas_cfg.ssb_freq_hz                              = ssb_freq_hz;
        meas_cfg.scs                                      = scs;
        meas_cfg.serving_cell_pci                         = -1;
        TESTASSERT(intra_measure.set_config(meas_cfg));

        srsran::rf_buffer_t    radio_buffer(baseband_buffer.data(), sf_len);
        srsran::rf_timestamp_t ts = {};

        // Start measurements
        intra_measure.set_cells_to_meas({});

        for (uint32_t i = 0; i < args.meas_period_ms * 5; i++) {
          radio.rx_now(radio_buffer, ts);

          intra_measure.run_tti(tti_count, baseband_buffer.data(), sf_len);

          tti_count = TTI_ADD(tti_count, 1);
        }

        // Stop measurements
        intra_measure.meas_stop();
      }
    }
  }

  // Stop radio before it overflows
  radio.stop();

  // make sure last measurement has been received before stopping
  intra_measure.wait_meas();

  // Stop, it will block until the asynchronous thread quits
  intra_measure.stop();

  logger.warning("NR intra frequency performance %d Msps\n", intra_measure.get_perf());
  srslog::flush();

  rrc.print_stats();

  return EXIT_SUCCESS;
}
