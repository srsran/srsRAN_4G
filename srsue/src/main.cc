/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#include <iostream>
#include <fstream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

#include "srsue/hdr/ue.h"
#include "srslte/common/config_file.h"
#include "srslte/srslte.h"
#include "srsue/hdr/metrics_stdout.h"
#include "srsue/hdr/metrics_csv.h"
#include "srslte/common/metrics_hub.h"
#include "srslte/version.h"

using namespace std;
using namespace srsue;
namespace bpo = boost::program_options;

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
string config_file;

void parse_args(all_args_t *args, int argc, char *argv[]) {

  // Command line only options
  bpo::options_description general("General options");

  general.add_options()
    ("help,h", "Produce help message")
    ("version,v", "Print version information and exit");

  // Command line or config file options
  bpo::options_description common("Configuration options");
  common.add_options()
    ("rf.dl_earfcn", bpo::value<uint32_t>(&args->rf.dl_earfcn)->default_value(3400), "Downlink EARFCN")
    ("rf.freq_offset", bpo::value<float>(&args->rf.freq_offset)->default_value(0), "(optional) Frequency offset")
    ("rf.dl_freq",     bpo::value<float>(&args->rf.dl_freq)->default_value(-1),      "Downlink Frequency (if positive overrides EARFCN)")
    ("rf.ul_freq",     bpo::value<float>(&args->rf.ul_freq)->default_value(-1),      "Uplink Frequency (if positive overrides EARFCN)")
    ("rf.rx_gain", bpo::value<float>(&args->rf.rx_gain)->default_value(-1), "Front-end receiver gain")
    ("rf.tx_gain", bpo::value<float>(&args->rf.tx_gain)->default_value(-1), "Front-end transmitter gain")
    ("rf.nof_rx_ant", bpo::value<uint32_t>(&args->rf.nof_rx_ant)->default_value(1), "Number of RX antennas")

    ("rf.device_name", bpo::value<string>(&args->rf.device_name)->default_value("auto"), "Front-end device name")
    ("rf.device_args", bpo::value<string>(&args->rf.device_args)->default_value("auto"), "Front-end device arguments")
    ("rf.time_adv_nsamples", bpo::value<string>(&args->rf.time_adv_nsamples)->default_value("auto"),
     "Transmission time advance")
    ("rf.burst_preamble_us", bpo::value<string>(&args->rf.burst_preamble)->default_value("auto"), "Transmission time advance")
    ("rf.continuous_tx", bpo::value<string>(&args->rf.continuous_tx)->default_value("auto"), "Transmit samples continuously to the radio or on bursts (auto/yes/no). Default is auto (yes for UHD, no for rest)")

    ("rrc.feature_group", bpo::value<uint32_t>(&args->rrc.feature_group)->default_value(0xe6041000), "Hex value of the featureGroupIndicators field in the"
                                                                                           "UECapabilityInformation message. Default 0xe6041000")
    ("rrc.ue_category",   bpo::value<string>(&args->ue_category_str)->default_value("4"),  "UE Category (1 to 5)")

    ("nas.apn",               bpo::value<string>(&args->nas.apn_name)->default_value(""),  "Set Access Point Name (APN) for data services")
    ("nas.user",              bpo::value<string>(&args->nas.apn_user)->default_value(""),  "Username for CHAP authentication")
    ("nas.pass",              bpo::value<string>(&args->nas.apn_pass)->default_value(""),  "Password for CHAP authentication")
    ("nas.force_imsi_attach", bpo::value<bool>(&args->nas.force_imsi_attach)->default_value(false),  "Whether to always perform an IMSI attach")


    ("pcap.enable", bpo::value<bool>(&args->pcap.enable)->default_value(false), "Enable MAC packet captures for wireshark")
    ("pcap.filename", bpo::value<string>(&args->pcap.filename)->default_value("ue.pcap"), "MAC layer capture filename")
    ("pcap.nas_enable",   bpo::value<bool>(&args->pcap.nas_enable)->default_value(false), "Enable NAS packet captures for wireshark")
    ("pcap.nas_filename", bpo::value<string>(&args->pcap.nas_filename)->default_value("ue_nas.pcap"), "NAS layer capture filename (useful when NAS encryption is enabled)")


    ("trace.enable", bpo::value<bool>(&args->trace.enable)->default_value(false), "Enable PHY and radio timing traces")
    ("trace.phy_filename", bpo::value<string>(&args->trace.phy_filename)->default_value("ue.phy_trace"),
     "PHY timing traces filename")
    ("trace.radio_filename", bpo::value<string>(&args->trace.radio_filename)->default_value("ue.radio_trace"),
     "Radio timing traces filename")

    ("gui.enable", bpo::value<bool>(&args->gui.enable)->default_value(false), "Enable GUI plots")

    ("log.phy_level", bpo::value<string>(&args->log.phy_level), "PHY log level")
    ("log.phy_lib_level", bpo::value<string>(&args->log.phy_lib_level), "PHY lib log level")
    ("log.phy_hex_limit", bpo::value<int>(&args->log.phy_hex_limit), "PHY log hex dump limit")
    ("log.mac_level", bpo::value<string>(&args->log.mac_level), "MAC log level")
    ("log.mac_hex_limit", bpo::value<int>(&args->log.mac_hex_limit), "MAC log hex dump limit")
    ("log.rlc_level", bpo::value<string>(&args->log.rlc_level), "RLC log level")
    ("log.rlc_hex_limit", bpo::value<int>(&args->log.rlc_hex_limit), "RLC log hex dump limit")
    ("log.pdcp_level", bpo::value<string>(&args->log.pdcp_level), "PDCP log level")
    ("log.pdcp_hex_limit", bpo::value<int>(&args->log.pdcp_hex_limit), "PDCP log hex dump limit")
    ("log.rrc_level", bpo::value<string>(&args->log.rrc_level), "RRC log level")
    ("log.rrc_hex_limit", bpo::value<int>(&args->log.rrc_hex_limit), "RRC log hex dump limit")
    ("log.gw_level", bpo::value<string>(&args->log.gw_level), "GW log level")
    ("log.gw_hex_limit", bpo::value<int>(&args->log.gw_hex_limit), "GW log hex dump limit")
    ("log.nas_level", bpo::value<string>(&args->log.nas_level), "NAS log level")
    ("log.nas_hex_limit", bpo::value<int>(&args->log.nas_hex_limit), "NAS log hex dump limit")
    ("log.usim_level", bpo::value<string>(&args->log.usim_level), "USIM log level")
    ("log.usim_hex_limit", bpo::value<int>(&args->log.usim_hex_limit), "USIM log hex dump limit")


    ("log.all_level", bpo::value<string>(&args->log.all_level)->default_value("info"), "ALL log level")
    ("log.all_hex_limit", bpo::value<int>(&args->log.all_hex_limit)->default_value(32), "ALL log hex dump limit")

    ("log.filename", bpo::value<string>(&args->log.filename)->default_value("/tmp/ue.log"), "Log filename")
    ("log.file_max_size", bpo::value<int>(&args->log.file_max_size)->default_value(-1), "Maximum file size (in kilobytes). When passed, multiple files are created. Default -1 (single file)")

    ("usim.mode", bpo::value<string>(&args->usim.mode)->default_value("soft"), "USIM mode (soft or pcsc)")
    ("usim.algo", bpo::value<string>(&args->usim.algo), "USIM authentication algorithm")
    ("usim.op", bpo::value<string>(&args->usim.op), "USIM operator code")
    ("usim.opc", bpo::value<string>(&args->usim.opc), "USIM operator code (ciphered variant)")
    ("usim.imsi", bpo::value<string>(&args->usim.imsi), "USIM IMSI")
    ("usim.imei", bpo::value<string>(&args->usim.imei), "USIM IMEI")
    ("usim.k", bpo::value<string>(&args->usim.k), "USIM K")
    ("usim.pin", bpo::value<string>(&args->usim.pin), "PIN in case real SIM card is used")
    ("usim.reader", bpo::value<string>(&args->usim.reader)->default_value(""), "Force specifiy PCSC reader. Default: Try all available readers.")

    /* Expert section */
    ("expert.ip_netmask",
     bpo::value<string>(&args->expert.ip_netmask)->default_value("255.255.255.0"),
     "Netmask of the tun_srsue device")

     ("expert.mbms_service",
     bpo::value<int>(&args->expert.mbms_service)->default_value(-1),
     "automatically starts an mbms service of the number given")

    ("expert.phy.worker_cpu_mask",
     bpo::value<int>(&args->expert.phy.worker_cpu_mask)->default_value(-1),
     "cpu bit mask (eg 255 = 1111 1111)")

    ("expert.phy.sync_cpu_affinity",
     bpo::value<int>(&args->expert.phy.sync_cpu_affinity)->default_value(-1),
     "index of the core used by the sync thread")

    ("expert.metrics_period_secs",
     bpo::value<float>(&args->expert.metrics_period_secs)->default_value(1.0),
     "Periodicity for metrics in seconds")

    ("expert.metrics_csv_enable",
     bpo::value<bool>(&args->expert.metrics_csv_enable)->default_value(false),
     "Write UE metrics to CSV file")

    ("expert.metrics_csv_filename",
     bpo::value<string>(&args->expert.metrics_csv_filename)->default_value("/tmp/ue_metrics.csv"),
     "Metrics CSV filename")

    ("expert.pregenerate_signals",
     bpo::value<bool>(&args->expert.pregenerate_signals)->default_value(false),
     "Pregenerate uplink signals after attach. Improves CPU performance.")

    ("expert.print_buffer_state",
     bpo::value<bool>(&args->expert.print_buffer_state)->default_value(false),
     "Prints on the console the buffer state every 10 seconds")

    ("expert.rssi_sensor_enabled",
     bpo::value<bool>(&args->expert.phy.rssi_sensor_enabled)->default_value(false),
     "Enable or disable RF frontend RSSI sensor. In some USRP devices can cause segmentation fault")

    ("expert.rx_gain_offset",
     bpo::value<float>(&args->expert.phy.rx_gain_offset)->default_value(62),
     "RX Gain offset to add to rx_gain to correct RSRP value")

      ("expert.prach_gain",
     bpo::value<float>(&args->expert.phy.prach_gain)->default_value(-1.0),
     "Disable PRACH power control")

    ("expert.cqi_max",
     bpo::value<int>(&args->expert.phy.cqi_max)->default_value(15),
     "Upper bound on the maximum CQI to be reported. Default 15.")

    ("expert.cqi_fixed",
     bpo::value<int>(&args->expert.phy.cqi_fixed)->default_value(-1),
     "Fixes the reported CQI to a constant value. Default disabled.")

    ("expert.sfo_correct_period",
     bpo::value<uint32_t>(&args->expert.phy.sfo_correct_period)->default_value(DEFAULT_SAMPLE_OFFSET_CORRECT_PERIOD),
     "Period in ms to correct sample time")

    ("expert.sfo_emma",
     bpo::value<float>(&args->expert.phy.sfo_ema)->default_value(DEFAULT_SFO_EMA_COEFF),
     "EMA coefficient to average sample offsets used to compute SFO")

    ("expert.snr_ema_coeff",
     bpo::value<float>(&args->expert.phy.snr_ema_coeff)->default_value(0.1),
     "Sets the SNR exponential moving average coefficient (Default 0.1)")

    ("expert.snr_estim_alg",
     bpo::value<string>(&args->expert.phy.snr_estim_alg)->default_value("refs"),
     "Sets the noise estimation algorithm. (Default refs)")

    ("expert.pdsch_max_its",
     bpo::value<int>(&args->expert.phy.pdsch_max_its)->default_value(4),
     "Maximum number of turbo decoder iterations")

    ("expert.attach_enable_64qam",
     bpo::value<bool>(&args->expert.phy.attach_enable_64qam)->default_value(false),
     "PUSCH 64QAM modulation before attachment")

    ("expert.nof_phy_threads",
     bpo::value<int>(&args->expert.phy.nof_phy_threads)->default_value(2),
     "Number of PHY threads")

    ("expert.equalizer_mode",
     bpo::value<string>(&args->expert.phy.equalizer_mode)->default_value("mmse"),
     "Equalizer mode")

    ("expert.intra_freq_meas_len_ms",
       bpo::value<uint32_t>(&args->expert.phy.intra_freq_meas_len_ms)->default_value(20),
       "Duration of the intra-frequency neighbour cell measurement in ms.")

    ("expert.intra_freq_meas_period_ms",
       bpo::value<uint32_t>(&args->expert.phy.intra_freq_meas_period_ms)->default_value(200),
       "Period of intra-frequency neighbour cell measurement in ms. Maximum as per 3GPP is 200 ms.")

    ("expert.cfo_is_doppler",
       bpo::value<bool>(&args->expert.phy.cfo_is_doppler)->default_value(false),
       "Assume detected CFO is doppler and correct the UL in the same direction. If disabled, the CFO is assumed"
        "to be caused by the local oscillator and the UL correction is in the opposite direction. Default assumes oscillator.")

    ("expert.cfo_integer_enabled",
     bpo::value<bool>(&args->expert.phy.cfo_integer_enabled)->default_value(false),
     "Enables integer CFO estimation and correction.")

    ("expert.cfo_correct_tol_hz",
     bpo::value<float>(&args->expert.phy.cfo_correct_tol_hz)->default_value(1.0),
     "Tolerance (in Hz) for digital CFO compensation (needs to be low if average_subframe_enabled=true.")

    ("expert.cfo_pss_ema",
     bpo::value<float>(&args->expert.phy.cfo_pss_ema)->default_value(DEFAULT_CFO_EMA_TRACK),
     "CFO Exponential Moving Average coefficient for PSS estimation during TRACK.")

    ("expert.cfo_ref_mask",
     bpo::value<uint32_t>(&args->expert.phy.cfo_ref_mask)->default_value(1023),
     "Bitmask for subframes on which to run RS estimation (set to 0 to disable, default all sf)")

    ("expert.cfo_loop_bw_pss",
     bpo::value<float>(&args->expert.phy.cfo_loop_bw_pss)->default_value(DEFAULT_CFO_BW_PSS),
     "CFO feedback loop bandwidth for samples from PSS")

    ("expert.cfo_loop_bw_ref",
     bpo::value<float>(&args->expert.phy.cfo_loop_bw_ref)->default_value(DEFAULT_CFO_BW_REF),
     "CFO feedback loop bandwidth for samples from RS")

    ("expert.cfo_loop_pss_tol",
     bpo::value<float>(&args->expert.phy.cfo_loop_pss_tol)->default_value(DEFAULT_CFO_PSS_MIN),
     "Tolerance (in Hz) of the PSS estimation method. Below this value, PSS estimation does not feeds back the loop"
       "and RS estimations are used instead (when available)")

    ("expert.cfo_loop_ref_min",
     bpo::value<float>(&args->expert.phy.cfo_loop_ref_min)->default_value(DEFAULT_CFO_REF_MIN),
     "Tolerance (in Hz) of the RS estimation method. Below this value, RS estimation does not feeds back the loop")

    ("expert.cfo_loop_pss_conv",
     bpo::value<uint32_t>(&args->expert.phy.cfo_loop_pss_conv)->default_value(DEFAULT_PSS_STABLE_TIMEOUT),
     "After the PSS estimation is below cfo_loop_pss_tol for cfo_loop_pss_timeout times consecutively, RS adjustments are allowed.")

    ("expert.sic_pss_enabled",
     bpo::value<bool>(&args->expert.phy.sic_pss_enabled)->default_value(false),
     "Applies Successive Interference Cancellation to PSS signals when searching for neighbour cells. Must be disabled if cells have identical channel and timing.")

    ("expert.average_subframe_enabled",
     bpo::value<bool>(&args->expert.phy.average_subframe_enabled)->default_value(true),
     "Averages in the time domain the channel estimates within 1 subframe. Needs accurate CFO correction.")

    ("expert.estimator_fil_auto",
     bpo::value<bool>(&args->expert.phy.estimator_fil_auto)->default_value(false),
     "The channel estimator smooths the channel estimate with an adaptative filter.")

    ("expert.estimator_fil_stddev",
     bpo::value<float>(&args->expert.phy.estimator_fil_stddev)->default_value(1.0f),
     "Sets the channel estimator smooth gaussian filter standard deviation.")

    ("expert.estimator_fil_order",
     bpo::value<uint32_t>(&args->expert.phy.estimator_fil_order)->default_value(4),
     "Sets the channel estimator smooth gaussian filter order (even values perform better).")

    ("expert.sss_algorithm",
     bpo::value<string>(&args->expert.phy.sss_algorithm)->default_value("full"),
     "Selects the SSS estimation algorithm.")

    ("expert.pdsch_csi_enabled",
     bpo::value<bool>(&args->expert.phy.pdsch_csi_enabled)->default_value(true),
     "Stores the Channel State Information and uses it for weightening the softbits. It is only used in TM1.")

    ("rf_calibration.tx_corr_dc_gain", bpo::value<float>(&args->rf_cal.tx_corr_dc_gain)->default_value(0.0),
     "TX DC offset gain correction")
    ("rf_calibration.tx_corr_dc_phase", bpo::value<float>(&args->rf_cal.tx_corr_dc_phase)->default_value(0.0),
     "TX DC offset phase correction")
    ("rf_calibration.tx_corr_iq_i", bpo::value<float>(&args->rf_cal.tx_corr_iq_i)->default_value(0.0),
     "TX IQ imbalance inphase correction")
    ("rf_calibration.tx_corr_iq_q", bpo::value<float>(&args->rf_cal.tx_corr_iq_q)->default_value(0.0),
     "TX IQ imbalance quadrature correction");

  // Positional options - config file location
  bpo::options_description position("Positional options");
  position.add_options()
    ("config_file", bpo::value<string>(&config_file), "UE configuration file");
  bpo::positional_options_description p;
  p.add("config_file", -1);

  // these options are allowed on the command line
  bpo::options_description cmdline_options;
  cmdline_options.add(common).add(position).add(general);

  // parse the command line and store result in vm
  bpo::variables_map vm;
  bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
  bpo::notify(vm);

  // help option was given - print usage and exit
  if (vm.count("help")) {
    cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << endl << endl;
    cout << common << endl << general << endl;
    exit(0);
  }

  // print version number and exit
  if (vm.count("version")) {
    cout << "Version " <<
         srslte_get_version_major() << "." <<
         srslte_get_version_minor() << "." <<
         srslte_get_version_patch() << endl;
    exit(0);
  }

  // if no config file given, check users home path
  if (!vm.count("config_file")) {

    if (!config_exists(config_file, "ue.conf")) {
      cout << "Failed to read UE configuration file " << config_file << " - exiting" << endl;
      exit(1);
    }
  }

  cout << "Reading configuration file " << config_file << "..." << endl;
  ifstream conf(config_file.c_str(), ios::in);
  if (conf.fail()) {
    cout << "Failed to read configuration file " << config_file << " - exiting" << endl;
    exit(1);
  }

  // parse config file and handle errors gracefully
  try {
    bpo::store(bpo::parse_config_file(conf, common), vm);
    bpo::notify(vm);
  } catch (const boost::program_options::error& e) {
    cerr << e.what() << endl;
    exit(1);
  }

  //Check conflicting OP/OPc options and which is being used
  if (vm.count("usim.op") && !vm["usim.op"].defaulted() &&
      vm.count("usim.opc") && !vm["usim.opc"].defaulted())
  {
    cout << "Conflicting options OP and OPc. Please configure either one or the other." << endl;
    exit(1);
  }
  else {
    args->usim.using_op = vm.count("usim.op");
  }

  // Apply all_level to any unset layers
  if (vm.count("log.all_level")) {
    if (!vm.count("log.phy_level")) {
      args->log.phy_level = args->log.all_level;
    }
    if (!vm.count("log.phy_lib_level")) {
      args->log.phy_lib_level = args->log.all_level;
    }
    if (!vm.count("log.mac_level")) {
      args->log.mac_level = args->log.all_level;
    }
    if (!vm.count("log.rlc_level")) {
      args->log.rlc_level = args->log.all_level;
    }
    if (!vm.count("log.pdcp_level")) {
      args->log.pdcp_level = args->log.all_level;
    }
    if (!vm.count("log.rrc_level")) {
      args->log.rrc_level = args->log.all_level;
    }
    if (!vm.count("log.nas_level")) {
      args->log.nas_level = args->log.all_level;
    }
    if (!vm.count("log.gw_level")) {
      args->log.gw_level = args->log.all_level;
    }
    if (!vm.count("log.usim_level")) {
      args->log.usim_level = args->log.all_level;
    }
  }


  // Apply all_hex_limit to any unset layers
  if (vm.count("log.all_hex_limit")) {
    if (!vm.count("log.phy_hex_limit")) {
      args->log.phy_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.mac_hex_limit")) {
      args->log.mac_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.rlc_hex_limit")) {
      args->log.rlc_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.pdcp_hex_limit")) {
      args->log.pdcp_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.rrc_hex_limit")) {
      args->log.rrc_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.nas_hex_limit")) {
      args->log.nas_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.gw_hex_limit")) {
      args->log.gw_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.usim_hex_limit")) {
      args->log.usim_hex_limit = args->log.all_hex_limit;
    }
  }
}

static int sigcnt = 0;
static bool running = true;
static bool do_metrics = false;
metrics_stdout metrics_screen;
static bool show_mbms = false;
static bool mbms_service_start = false;
uint32_t serv, port;

void sig_int_handler(int signo) {
  sigcnt++;
  running = false;
  printf("Stopping srsUE... Press Ctrl+C %d more times to force stop\n", 10-sigcnt);
  if (sigcnt >= 10) {
    exit(-1);
  }
}

void *input_loop(void *m) {
  string key;
  while (running) {
    getline(cin, key);
    if (cin.eof() || cin.bad()) {
      cout << "Closing stdin thread." << endl;
      break;
    } else {
      if (0 == key.compare("t")) {
        do_metrics = !do_metrics;
        if (do_metrics) {
          cout << "Enter t to stop trace." << endl;
        } else {
          cout << "Enter t to restart trace." << endl;
        }
        metrics_screen.toggle_print(do_metrics);
      } else
      if (0 == key.compare("q")) {
        running = false;
      }  
    else if (0 == key.compare("mbms")) {
      show_mbms = true;
    } else if (key.find("mbms_service_start") != string::npos) {

      char *dup = strdup(key.c_str());
      strtok(dup, " ");
      char *s = strtok(NULL, " ");
      if(NULL == s) {
        cout << "Usage: mbms_service_start <service_id> <port_number>" << endl;
        continue;
      }
      serv = atoi(s);
      char* p = strtok(NULL, " ");
      if(NULL == p) {
        cout << "Usage: mbms_service_start <service_id> <port_number>" << endl;
        continue;
      }
      port = atoi(p);
      mbms_service_start = true;
      free(dup);
    }
   }
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  srslte::metrics_hub<ue_metrics_t> metricshub;
  signal(SIGINT, sig_int_handler);
  signal(SIGTERM, sig_int_handler);
  all_args_t args;

  srslte_debug_handle_crash(argc, argv);

  parse_args(&args, argc, argv);

  srsue_instance_type_t type = LTE;
  ue_base *ue = ue_base::get_instance(type);
  if (!ue) {
    cout << "Error creating UE instance." << endl << endl;
    exit(1);
  }

  cout << "---  Software Radio Systems " << srsue_instance_type_text[type] << " UE  ---" << endl << endl;
  if (!ue->init(&args)) {
    exit(1);
  }

  metricshub.init(ue, args.expert.metrics_period_secs);
  metricshub.add_listener(&metrics_screen);
  metrics_screen.set_ue_handle(ue);

  metrics_csv metrics_file(args.expert.metrics_csv_filename);
  if (args.expert.metrics_csv_enable) {
    metricshub.add_listener(&metrics_file);
    metrics_file.set_ue_handle(ue);
  }

  pthread_t input;
  pthread_create(&input, NULL, &input_loop, &args);

  printf("Attaching UE...\n");
  while (!ue->attach() && running) {
    sleep(1);
  }
  if (running) {
    if (args.expert.pregenerate_signals) {
      printf("Pre-generating signals...\n");
      ue->pregenerate_signals(true);
      printf("Done pregenerating signals.\n");
    }
    if (args.gui.enable) {
      ue->start_plot();
    }
    // Auto-start MBMS service by default
    if(args.expert.mbms_service > -1){
      serv = args.expert.mbms_service;
      port = 4321;
      mbms_service_start = true;
    }
  }
  int cnt=0;
  while (running) {
    //
    if(mbms_service_start) {
      if(ue->mbms_service_start(serv, port)){
        mbms_service_start = false;
      }
    }
    if(show_mbms) {
      show_mbms = false;
      ue->print_mbms();
    }
    if (args.expert.print_buffer_state) {
      cnt++;
      if (cnt==10) {
        cnt=0;
        ue->print_pool();
      }
    }
    sleep(1);
  }
  pthread_cancel(input);
  metricshub.stop();
  ue->stop();
  ue->cleanup();
  cout << "---  exiting  ---" << endl;
  exit(0);
}
