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
#include "srsran/common/test_common.h"
#include "srsran/radio/radio.h"
#include <boost/program_options.hpp>
#include <iostream>

// shorten boost program options namespace
namespace bpo = boost::program_options;

// Test arguments
struct test_args_s {
  bool        valid            = false;
  double      srate_hz         = 3.84e6;
  double      freq_hz          = 3.5e9;
  float       rx_gain_db       = 20.0f;
  float       tx_gain_db_begin = 0.0f;
  float       tx_gain_db_end   = 30.0f;
  float       tx_gain_db_step  = 1.0f;
  uint32_t    tx_delay_ms      = 4;
  uint32_t    step_period_ms   = 1;
  uint32_t    nof_repetitions  = 3;
  uint32_t    power_ramping_ms = 50;
  uint32_t    pre_tx_ms        = 50; // After main loop acquire a few more ms
  uint32_t    post_tx_ms       = 50; // After main loop acquire a few more ms
  std::string filename         = "/tmp/baseband.iq.dat";
  std::string device_name      = "zmq";
  std::string device_args      = "tx_port=tcp://*:5555,rx_port=tcp://localhost:5555,base_srate=3.84e6";

  test_args_s(int argc, char** argv)
  {
    bpo::options_description options;

    // clang-format off
    options.add_options()
        ("srate",         bpo::value<double>(&srate_hz)->default_value(srate_hz),                   "Sampling rate in Hz")
        ("freq",          bpo::value<double>(&freq_hz)->default_value(freq_hz),                     "Center frequency in Hz")
        ("rx_gain",       bpo::value<float>(&rx_gain_db)->default_value(rx_gain_db),                "Receiver gain in dB")
        ("tx_gain_begin", bpo::value<float>(&tx_gain_db_begin)->default_value(tx_gain_db_begin),    "Initial transmitter gain in dB")
        ("tx_gain_end",   bpo::value<float>(&tx_gain_db_end)->default_value(tx_gain_db_end),        "Final transmitter gain in dB")
        ("tx_gain_step",  bpo::value<float>(&tx_gain_db_step)->default_value(tx_gain_db_step),      "Step transmitter gain in dB")
        ("tx_delay",      bpo::value<uint32_t>(&tx_delay_ms)->default_value(tx_delay_ms),           "Delay between Rx and Tx in milliseconds")
        ("step_period",   bpo::value<uint32_t>(&step_period_ms)->default_value(step_period_ms),     "Transmitter gain step period in milliseconds")
        ("repetitions",   bpo::value<uint32_t>(&nof_repetitions)->default_value(nof_repetitions),   "Number of transmit gain steering repetitions")
        ("power_ramping", bpo::value<uint32_t>(&power_ramping_ms)->default_value(power_ramping_ms), "Transmitter initial power ramping in milliseconds")
        ("pre_tx",        bpo::value<uint32_t>(&pre_tx_ms)->default_value(pre_tx_ms),               "Initial acquisition time before start transmission in milliseconds")
        ("post_tx",       bpo::value<uint32_t>(&pre_tx_ms)->default_value(pre_tx_ms),               "Initial acquisition time after ending transmission in milliseconds")
        ("filename",      bpo::value<std::string>(&filename)->default_value(filename),              "File sink filename")
        ("dev_name",      bpo::value<std::string>(&device_name)->default_value(device_name),        "RF Device name")
        ("dev_args",      bpo::value<std::string>(&device_args)->default_value(device_args),        "RF Device arguments")
        ("help",                                                                                    "Show this message")
        ;
    // clang-format on

    bpo::variables_map vm;
    try {
      bpo::store(bpo::command_line_parser(argc, argv).options(options).run(), vm);
      bpo::notify(vm);
      valid = true;
    } catch (bpo::error& e) {
      std::cerr << e.what() << std::endl;
    }

    // help option was given or error - print usage and exit
    if (vm.count("help") > 0 or not valid) {
      std::cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << std::endl << std::endl;
      std::cout << options << std::endl << std::endl;
      valid = false;
    }
  }
};

class phy_radio_listener : public srsran::phy_interface_radio
{
private:
  srslog::basic_logger& logger         = srslog::fetch_basic_logger("TEST", false);
  uint32_t              overflow_count = 0;
  uint32_t              failure_count  = 0;

public:
  phy_radio_listener() {}
  void radio_overflow() override
  {
    overflow_count++;
    logger.error("Overflow");
  }
  void radio_failure() override
  {
    failure_count++;
    logger.error("Failure");
  }
};

class test_sink
{
private:
  srslog::basic_logger& logger   = srslog::fetch_basic_logger("SINK", false);
  srsran_filesink_t     filesink = {};
  std::vector<float>    meas_power_dB;

public:
  test_sink(const std::string& filename)
  {
    if (srsran_filesink_init(&filesink, filename.c_str(), SRSRAN_COMPLEX_FLOAT_BIN) < SRSRAN_SUCCESS) {
      ERROR("Error initiating filesink");
      filesink = {};
    }
  }

  ~test_sink()
  {
    // Free filesink
    srsran_filesink_free(&filesink);

    // Print measure power
    printf("Measured power: ");
    srsran_vec_fprint_f(stdout, meas_power_dB.data(), (int)meas_power_dB.size());
  }

  void write(std::vector<cf_t>& buffer, uint32_t nsamp)
  {
    // Measure average power
    float avg_pwr = srsran_vec_avg_power_cf(buffer.data(), nsamp);

    // Push measurement in dB
    meas_power_dB.push_back(srsran_convert_power_to_dB(avg_pwr));

    // Write signal in file
    srsran_filesink_write(&filesink, (void*)buffer.data(), (int)nsamp);
  }
};

class test_source
{
private:
  //  cf_t  phase     = 1.0f;
  cf_t  phase     = 0.01;
  float freq_norm = 0.125;

public:
  test_source() {}

  ~test_source() {}

  void generate(std::vector<cf_t>& buffer, uint32_t nsamp)
  {
    phase *= srsran_vec_gen_sine(phase, freq_norm, buffer.data(), (int)nsamp);
  }
};

int main(int argc, char** argv)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);
  srslog::init();
  srsran::radio      radio;
  phy_radio_listener radio_listener;

  enum { BEFORE_RAMPING, RAMPING, GAIN_STEERING, POST_GAIN_STEERING, END } state = BEFORE_RAMPING;

  test_args_s args(argc, argv);
  TESTASSERT(args.valid);

  // Calculate number of steps
  TESTASSERT(std::isnormal(args.tx_gain_db_step));
  uint32_t nof_steps = (args.tx_gain_db_end - args.tx_gain_db_begin) / args.tx_gain_db_step;

  // Calculate subframe size in 1ms
  TESTASSERT(std::isnormal(args.srate_hz));
  uint32_t sf_sz = (uint32_t)std::round(1e-3 * args.srate_hz);

  // Allocate baseband buffer
  std::vector<cf_t> buffer(sf_sz);

  // Prepare radio arguments
  srsran::rf_args_t rf_args = {};
  rf_args.log_level         = "info";
  rf_args.srate_hz          = args.srate_hz;
  rf_args.dl_freq           = args.freq_hz;
  rf_args.ul_freq           = args.freq_hz;
  rf_args.rx_gain           = args.rx_gain_db;
  rf_args.tx_gain           = args.tx_gain_db_begin;
  rf_args.nof_carriers      = 1;
  rf_args.nof_antennas      = 1;
  rf_args.device_name       = args.device_name;
  rf_args.device_args       = args.device_args;

  // Initialise radio
  TESTASSERT(radio.init(rf_args, &radio_listener) == SRSRAN_SUCCESS);

  // Setup LO frequencies
  radio.set_tx_freq(0, args.freq_hz);
  radio.set_rx_freq(0, args.freq_hz);

  // Setup sampling rate
  radio.set_tx_srate(args.srate_hz);
  radio.set_rx_srate(args.srate_hz);

  // Setup initial gains
  radio.set_tx_gain(args.tx_gain_db_begin);
  radio.set_rx_gain(args.rx_gain_db);

  // Create signal sink
  test_sink sink(args.filename);

  // Create signal source
  test_source source;

  // Perform Tx/Rx
  uint32_t sf_count   = 0;
  uint32_t repetition = 0;
  while (state != END) {
    switch (state) {
      case BEFORE_RAMPING:
        if (sf_count >= args.pre_tx_ms) {
          logger.info("-- Starting power ramping stage");
          state    = RAMPING;
          sf_count = 0;
          continue;
        }
        break;
      case RAMPING:
        if (sf_count >= args.power_ramping_ms) {
          logger.info("-- Starting gain steering stage");
          state    = GAIN_STEERING;
          sf_count = 0;
          continue;
        }
        break;
      case GAIN_STEERING:
        if (sf_count >= nof_steps * args.step_period_ms) {
          repetition++;
          sf_count = 0;
          logger.info("-- Finished repetition %d of %d", repetition, args.nof_repetitions);
        }

        if (repetition >= args.nof_repetitions) {
          logger.info("-- Starting post gain steering stage");
          state = POST_GAIN_STEERING;
          radio.tx_end();
          continue;
        }
        if (sf_count % args.step_period_ms == 0 and sf_count != 0) {
          float tx_gain_db = args.tx_gain_db_begin + args.tx_gain_db_step * (sf_count / args.step_period_ms);
          logger.info("-- New Tx gain %+.2f dB", tx_gain_db);
          radio.set_tx_gain(tx_gain_db);
        }
        break;
      case POST_GAIN_STEERING:
        if (sf_count >= args.post_tx_ms) {
          logger.info("-- Ending...");
          state    = END;
          sf_count = 0;
          continue;
        }
        break;
      default:
        break;
    }

    // Prepare reception buffers
    srsran::rf_buffer_t rf_buffer = {};
    rf_buffer.set(0, buffer.data());
    rf_buffer.set_nof_samples(sf_sz);

    // Receive
    srsran::rf_timestamp_t ts = {};
    TESTASSERT(radio.rx_now(rf_buffer, ts));

    // Save signal, including the time before transmission
    sink.write(buffer, sf_sz);

    if (state == RAMPING or state == GAIN_STEERING) {
      // Generate transmit signal
      source.generate(buffer, sf_sz);

      // Update timestamp for Tx
      ts.add(1e-3 * (double)args.tx_delay_ms);

      // Transmit
      radio.tx(rf_buffer, ts);
    }

    // Increase SF counting
    sf_count++;
  }

  // Tear down radio
  radio.stop();

  return SRSRAN_SUCCESS;
}
