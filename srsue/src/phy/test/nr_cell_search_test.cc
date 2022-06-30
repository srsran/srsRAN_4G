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

// Test gNb class
class test_gnb
{
private:
  uint32_t              pci;
  uint32_t              sf_len        = 0;
  srsran_ssb_t          ssb           = {};
  std::vector<cf_t>     signal_buffer = {};
  srslog::basic_logger& logger;
  srsran::channel       channel;
  std::vector<cf_t>     buffer;

public:
  struct args_t {
    uint32_t                    pci            = 500;
    double                      srate_hz       = 11.52e6;
    double                      center_freq_hz = 3.5e9;
    double                      ssb_freq_hz    = 3.5e9 - 960e3;
    srsran_subcarrier_spacing_t ssb_scs        = srsran_subcarrier_spacing_30kHz;
    uint32_t                    ssb_period_ms  = 20;
    uint16_t                    band;
    srsran::channel::args_t     channel;
    std::string                 log_level = "error";

    srsran_ssb_pattern_t get_ssb_pattern() const { return srsran::srsran_band_helper().get_ssb_pattern(band, ssb_scs); }
    srsran_duplex_mode_t get_duplex_mode() const { return srsran::srsran_band_helper().get_duplex_mode(band); }
  };

  test_gnb(const args_t& args) :
    logger(srslog::fetch_basic_logger("PCI=" + std::to_string(args.pci))), channel(args.channel, 1, logger)
  {
    logger.set_level(srslog::str_to_basic_level(args.log_level));

    // Initialise internals
    pci    = args.pci;
    sf_len = (uint32_t)round(args.srate_hz / 1000);

    // Allocate buffer
    buffer.resize(sf_len);

    // Initialise SSB
    srsran_ssb_args_t ssb_args = {};
    ssb_args.max_srate_hz      = args.srate_hz;
    ssb_args.min_scs           = args.ssb_scs;
    ssb_args.enable_encode     = true;
    if (srsran_ssb_init(&ssb, &ssb_args) < SRSRAN_SUCCESS) {
      logger.error("Error initialising SSB");
      return;
    }

    // Configure SSB
    srsran_ssb_cfg_t ssb_cfg = {};
    ssb_cfg.srate_hz         = args.srate_hz;
    ssb_cfg.center_freq_hz   = args.center_freq_hz;
    ssb_cfg.ssb_freq_hz      = args.ssb_freq_hz;
    ssb_cfg.scs              = args.ssb_scs;
    ssb_cfg.pattern          = args.get_ssb_pattern();
    ssb_cfg.duplex_mode      = args.get_duplex_mode();
    ssb_cfg.periodicity_ms   = args.ssb_period_ms;
    if (srsran_ssb_set_cfg(&ssb, &ssb_cfg) < SRSRAN_SUCCESS) {
      logger.error("Error configuring SSB");
      return;
    }

    // Configure channel
    channel.set_srate((uint32_t)args.srate_hz);
  }

  int work(uint32_t sf_idx, std::vector<cf_t>& baseband_buffer, const srsran::rf_timestamp_t& ts)
  {
    logger.set_context(sf_idx);

    // Zero buffer
    srsran_vec_cf_zero(buffer.data(), (uint32_t)buffer.size());

    // Check if SSB needs to be sent
    if (srsran_ssb_send(&ssb, sf_idx)) {
      // Prepare PBCH message
      srsran_pbch_msg_nr_t msg = {};

      // Add SSB
      if (srsran_ssb_add(&ssb, pci, &msg, buffer.data(), buffer.data()) < SRSRAN_SUCCESS) {
        logger.error("Error adding SSB");
        return SRSRAN_ERROR;
      }
    }

    // Run channel
    cf_t* in[SRSRAN_MAX_CHANNELS]  = {};
    cf_t* out[SRSRAN_MAX_CHANNELS] = {};
    in[0]                          = buffer.data();
    out[0]                         = buffer.data();
    channel.run(in, out, (uint32_t)buffer.size(), ts.get(0));

    // Add buffer to baseband buffer
    srsran_vec_sum_ccc(baseband_buffer.data(), buffer.data(), baseband_buffer.data(), (uint32_t)buffer.size());

    return SRSRAN_SUCCESS;
  }

  ~test_gnb() { srsran_ssb_free(&ssb); }
};

struct args_t {
  // General
  std::string log_level    = "warning";
  uint32_t    duration_s   = 1;
  double      srate_hz     = 11.52e6;
  uint32_t    carier_arfcn = 634240;
  uint32_t    ssb_arfcn    = 634176;
  std::string ssb_scs_str  = "30";

  // Measurement parameters
  std::set<uint32_t>          pcis_to_meas;
  uint32_t                    meas_len_ms    = 1;
  uint32_t                    meas_period_ms = 20;
  float                       thr_snr_db     = 5.0f;
  srsran_subcarrier_spacing_t ssb_scs        = srsran_subcarrier_spacing_30kHz;

  // Simulation parameters
  std::set<uint32_t> pcis_to_simulate;
  uint32_t           ssb_period_ms = 20;
  float channel_delay_min          = 0.0f; // Set to non-zero value to stir the delay from zero to this value in usec
  float channel_delay_max          = 0.0f; // Set to non-zero value to stir the delay from zero to this value in usec

  // On the Fly parameters
  std::string radio_device_name = "auto";
  std::string radio_device_args = "auto";
  std::string radio_log_level   = "info";
  float       rx_gain           = 60.0f;

  // File parameters
  std::string filename            = "";
  double      file_freq_offset_hz = 0.0;
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
    }
  }

  bool print_stats(args_t args)
  {
    printf("\n-- Statistics:\n");
    uint32_t true_counts        = 0;
    uint32_t false_counts       = 0;
    uint32_t tti_count          = (1000 * args.duration_s) / args.meas_period_ms;
    uint32_t ideal_true_counts  = args.pcis_to_simulate.size() * tti_count;
    uint32_t ideal_false_counts = tti_count * cells.size() - ideal_true_counts;

    for (auto& e : cells) {
      bool false_alarm = args.pcis_to_simulate.find(e.first) == args.pcis_to_simulate.end();

      if (args.pcis_to_simulate.empty()) {
        false_alarm       = (args.pcis_to_meas.count(e.first) == 0);
        ideal_true_counts = args.pcis_to_meas.size() * tti_count;
      }

      if (false_alarm) {
        false_counts += e.second.count;
      } else {
        true_counts += e.second.count;
      }

      printf("  pci=%03d; count=%3d; false=%s; rsrp=%+.1f|%+.1f|%+.1fdBfs;  rsrq=%+.1f|%+.1f|%+.1fdB;\n",
             e.first,
             e.second.count,
             false_alarm ? "y" : "n",
             e.second.rsrp_min,
             e.second.rsrp_avg,
             e.second.rsrp_max,
             e.second.rsrq_min,
             e.second.rsrq_avg,
             e.second.rsrq_max);
    }

    float prob_detection   = (ideal_true_counts) ? (float)true_counts / (float)ideal_true_counts : 0.0f;
    float prob_false_alarm = (ideal_false_counts) ? (float)false_counts / (float)ideal_false_counts : 0.0f;
    printf("\n");
    printf("    Probability of detection: %.6f\n", prob_detection);
    printf("  Probability of false alarm: %.6f\n", prob_false_alarm);

    return (prob_detection >= 0.9f && prob_false_alarm <= 0.1f);
  }
};

static void pci_list_parse_helper(std::string& list_str, std::set<uint32_t>& list)
{
  if (list_str == "all") {
    // Add all possible cells
    for (int i = 0; i < SRSRAN_NOF_NID_NR; i++) {
      list.insert(i);
    }
  } else if (list_str == "none") {
    list.clear();
  } else if (not list_str.empty()) {
    // Remove spaces from neightbour cell list
    list_str = srsran::string_remove_char(list_str, ' ');

    // Add cell to known cells
    srsran::string_parse_list(list_str, ',', list);
  }
}

// shorten boost program options namespace
namespace bpo = boost::program_options;

int parse_args(int argc, char** argv, args_t& args)
{
  int ret = SRSRAN_SUCCESS;

  std::string active_cell_list     = "500";
  std::string simulation_cell_list = "";
  std::string ssb_scs              = "30";

  bpo::options_description options("General options");
  bpo::options_description measure("Measurement options");
  bpo::options_description over_the_air("Mode 1: Over the air options (Default)");
  bpo::options_description simulation("Mode 2: Simulation options (enabled if simulation_cell_list is not empty)");
  bpo::options_description file("Mode 3: File (enabled if filename is provided)");

  // clang-format off
  measure.add_options()
      ("meas_len_ms",      bpo::value<uint32_t>(&args.meas_len_ms)->default_value(args.meas_len_ms),       "Measurement length")
      ("meas_period_ms",   bpo::value<uint32_t>(&args.meas_period_ms)->default_value(args.meas_period_ms), "Measurement period")
      ("active_cell_list", bpo::value<std::string>(&active_cell_list)->default_value(active_cell_list),    "Comma separated PCI cell list to measure")
      ("thr_snr_db",       bpo::value<float>(&args.thr_snr_db)->default_value(args.thr_snr_db),            "Detection threshold for SNR in dB")
      ;

  over_the_air.add_options()
      ("rf.device_name", bpo::value<std::string>(&args.radio_device_name)->default_value(args.radio_device_name), "RF Device Name")
      ("rf.device_args", bpo::value<std::string>(&args.radio_device_args)->default_value(args.radio_device_args), "RF Device arguments")
      ("rf.log_level",   bpo::value<std::string>(&args.radio_log_level)->default_value(args.radio_log_level),     "RF Log level (none, warning, info, debug)")
      ("rf.rx_gain",     bpo::value<float>(&args.rx_gain)->default_value(args.rx_gain),                           "RF Receiver gain in dB")
      ;

  simulation.add_options()
      ("simulation_cell_list", bpo::value<std::string>(&simulation_cell_list)->default_value(simulation_cell_list), "Comma separated PCI cell list to simulate")
      ("ssb_period",           bpo::value<uint32_t>(&args.ssb_period_ms)->default_value(args.ssb_period_ms),        "SSB period in ms")
      ("channel.delay_min",    bpo::value<float>(&args.channel_delay_min)->default_value(args.channel_delay_min),   "Channel delay minimum in usec.")
      ("channel.delay_max",    bpo::value<float>(&args.channel_delay_max)->default_value(args.channel_delay_max),   "Channel delay maximum in usec. Set to 0 to disable, otherwise it will steer the delay for the duration of the simulation")
      ;

  file.add_options()
      ("file.name", bpo::value<std::string>(&args.filename)->default_value(args.filename), "File name providing baseband")
      ("file.freq_offset", bpo::value<double>(&args.file_freq_offset_hz)->default_value(args.file_freq_offset_hz), "File name providing baseband")
      ;

  options.add(measure).add(over_the_air).add(simulation).add(file).add_options()
      ("help,h",        "Show this message")
      ("log_level",     bpo::value<std::string>(&args.log_level)->default_value(args.log_level),    "Intra measurement log level (none, warning, info, debug)")
      ("duration",      bpo::value<uint32_t>(&args.duration_s)->default_value(args.duration_s),     "Duration of the test in seconds")
      ("srate",         bpo::value<double>(&args.srate_hz)->default_value(args.srate_hz),           "Sampling Rate in Hz")
      ("carrier_arfcn", bpo::value<uint32_t>(&args.carier_arfcn)->default_value(args.carier_arfcn), "Carrier center frequency ARFCN")
      ("ssb_arfcn",     bpo::value<uint32_t>(&args.ssb_arfcn)->default_value(args.ssb_arfcn),       "SSB center frequency in ARFCN")
      ("ssb_scs",       bpo::value<std::string>(&ssb_scs)->default_value(ssb_scs),                  "SSB subcarrier spacing in kHz")
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
  pci_list_parse_helper(active_cell_list, args.pcis_to_meas);
  pci_list_parse_helper(simulation_cell_list, args.pcis_to_simulate);

  // Parse SSB SCS
  args.ssb_scs = srsran_subcarrier_spacing_from_str(ssb_scs.c_str());
  if (args.ssb_scs == srsran_subcarrier_spacing_invalid) {
    ret = SRSRAN_ERROR;
  }

  return ret;
}

int main(int argc, char** argv)
{
  int ret;

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
  double   srate_hz       = args.srate_hz;
  uint32_t sf_len         = (uint32_t)round(srate_hz / 1000.0);
  double   center_freq_hz = srsran::srsran_band_helper().nr_arfcn_to_freq(args.carier_arfcn);
  double   ssb_freq_hz    = srsran::srsran_band_helper().nr_arfcn_to_freq(args.ssb_arfcn);
  uint16_t band           = srsran::srsran_band_helper().get_band_from_dl_freq_Hz(center_freq_hz);
  logger.debug("Band: %d; srate: %.2f MHz; center_freq: %.1f MHz; ssb_freq: %.1f MHz;",
               band,
               srate_hz / 1e6,
               center_freq_hz / 1e6,
               ssb_freq_hz / 1e6);

  // Allocate buffer
  std::vector<cf_t> baseband_buffer(sf_len);

  // Create measurement callback
  meas_itf_listener rrc;

  // Create measurement instance
  srsue::scell::intra_measure_nr intra_measure(logger, rrc);

  // Initialise measurement instance
  srsue::scell::intra_measure_nr::args_t meas_args = {};
  meas_args.rx_gain_offset_dB                      = 0.0f;
  meas_args.max_len_ms                             = args.meas_len_ms;
  meas_args.max_srate_hz                           = srate_hz;
  meas_args.min_scs                                = args.ssb_scs;
  meas_args.thr_snr_db                             = args.thr_snr_db;
  TESTASSERT(intra_measure.init(0, meas_args));

  // Setup measurement
  srsue::scell::intra_measure_nr::config_t meas_cfg = {};
  meas_cfg.arfcn                                    = args.carier_arfcn;
  meas_cfg.srate_hz                                 = srate_hz;
  meas_cfg.len_ms                                   = args.meas_len_ms;
  meas_cfg.period_ms                                = args.meas_period_ms;
  meas_cfg.tti_period                               = args.meas_period_ms;
  meas_cfg.tti_offset                               = 0;
  meas_cfg.rx_gain_offset_db                        = 0;
  meas_cfg.center_freq_hz                           = center_freq_hz;
  meas_cfg.ssb_freq_hz                              = ssb_freq_hz;
  meas_cfg.scs                                      = srsran_subcarrier_spacing_30kHz;
  meas_cfg.serving_cell_pci                         = -1;
  TESTASSERT(intra_measure.set_config(meas_cfg));

  // Simulation only
  std::vector<std::unique_ptr<test_gnb> > test_gnb_v;

  // Over-the-air only
  std::unique_ptr<srsran::radio> radio = nullptr;

  // File read only
  srsran_filesource_t filesource = {};

  // Setup raio if the list of PCIs to simulate is empty
  if (not args.filename.empty()) {
    if (srsran_filesource_init(&filesource, args.filename.c_str(), SRSRAN_COMPLEX_FLOAT) < SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  } else if (args.pcis_to_simulate.empty()) {
    // Create radio log
    auto& radio_logger = srslog::fetch_basic_logger("RF", false);
    radio_logger.set_level(srslog::str_to_basic_level(args.radio_log_level));

    // Create radio
    radio = std::unique_ptr<srsran::radio>(new srsran::radio);

    // Init radio
    srsran::rf_args_t radio_args = {};
    radio_args.device_args       = args.radio_device_args;
    radio_args.device_name       = args.radio_device_name;
    radio_args.nof_carriers      = 1;
    radio_args.nof_antennas      = 1;
    radio->init(radio_args, nullptr);

    // Set sampling rate
    radio->set_rx_srate(srate_hz);

    // Set frequency
    radio->set_rx_freq(0, center_freq_hz);

  } else {
    // Create test eNb's if radio is not available
    for (const uint32_t& pci : args.pcis_to_simulate) {
      // Initialise channel and push back
      test_gnb::args_t gnb_args          = {};
      gnb_args.pci                       = pci;
      gnb_args.srate_hz                  = srate_hz;
      gnb_args.center_freq_hz            = center_freq_hz;
      gnb_args.ssb_freq_hz               = ssb_freq_hz;
      gnb_args.ssb_scs                   = args.ssb_scs;
      gnb_args.ssb_period_ms             = args.ssb_period_ms;
      gnb_args.band                      = band;
      gnb_args.log_level                 = args.log_level;
      gnb_args.channel.delay_enable      = std::isnormal(args.channel_delay_max);
      gnb_args.channel.delay_min_us      = args.channel_delay_min;
      gnb_args.channel.delay_max_us      = args.channel_delay_max;
      gnb_args.channel.delay_period_s    = args.duration_s;
      gnb_args.channel.delay_init_time_s = 0.0f;
      gnb_args.channel.enable            = (gnb_args.channel.delay_enable || gnb_args.channel.awgn_enable ||
                                 gnb_args.channel.fading_enable || gnb_args.channel.hst_enable);
      test_gnb_v.push_back(std::unique_ptr<test_gnb>(new test_gnb(gnb_args)));
    }
  }

  // pass cells to measure to intra_measure object
  intra_measure.set_cells_to_meas(args.pcis_to_meas);

  // Run loop
  srsran::rf_timestamp_t ts = {};
  for (uint32_t sf_idx = 0; sf_idx < args.duration_s * 1000; sf_idx++) {
    logger.set_context(sf_idx);

    // Clean buffer
    srsran_vec_cf_zero(baseband_buffer.data(), sf_len);

    if (not args.filename.empty()) {
      if (srsran_filesource_read(&filesource, baseband_buffer.data(), (int)sf_len) < SRSRAN_SUCCESS) {
        ERROR("Error reading from file");
        srsran_filesource_free(&filesource);
        return SRSRAN_ERROR;
      }

      srsran_vec_apply_cfo(
          baseband_buffer.data(), args.file_freq_offset_hz / args.srate_hz, baseband_buffer.data(), (int)sf_len);
    } else if (radio) {
      // Receive radio
      srsran::rf_buffer_t radio_buffer(baseband_buffer.data(), sf_len);
      radio->rx_now(radio_buffer, ts);
    } else {
      // Run gNb simulator
      for (auto& gnb : test_gnb_v) {
        gnb->work(sf_idx, baseband_buffer, ts);
      }

      // if it measuring, wait for avoiding overflowing
      intra_measure.wait_meas();

      // Increase Time counter
      ts.add(0.001);
    }

    // Give data to intra measure component
    intra_measure.run_tti(sf_idx % 10240, baseband_buffer.data(), sf_len);
    if (sf_idx % 1000 == 0) {
      logger.info("Done %.1f%%", (double)sf_idx * 100.0 / ((double)args.duration_s * 1000.0));
    }
  }

  // make sure last measurement has been received before stopping
  if (not radio) {
    intra_measure.wait_meas();
  }

  // Stop, it will block until the asynchronous thread quits
  intra_measure.stop();

  logger.warning("NR intra frequency performance %d Msps\n", intra_measure.get_perf());

  ret = rrc.print_stats(args) ? SRSRAN_SUCCESS : SRSRAN_ERROR;

  if (radio) {
    radio->stop();
  }

  if (not args.filename.empty()) {
    srsran_filesource_free(&filesource);
  }

  srslog::flush();

  if (ret == SRSRAN_SUCCESS) {
    printf("Ok\n");
  } else {
    printf("Error\n");
  }

  return ret;
}
