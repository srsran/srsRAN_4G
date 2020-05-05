/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "srslte/common/config_file.h"
#include "srslte/common/crash_handler.h"
#include "srslte/common/signal_handler.h"

#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>
#include <memory>
#include <string>

#include "srsenb/hdr/enb.h"
#include "srsenb/hdr/metrics_csv.h"
#include "srsenb/hdr/metrics_stdout.h"

using namespace std;
using namespace srsenb;
namespace bpo = boost::program_options;

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
string config_file;

void parse_args(all_args_t* args, int argc, char* argv[])
{
  string mcc;
  string mnc;
  string enb_id;

  // Command line only options
  bpo::options_description general("General options");
  // clang-format off
  general.add_options()
      ("help,h", "Produce help message")
      ("version,v", "Print version information and exit")
      ;

  // Command line or config file options
  bpo::options_description common("Configuration options");
  common.add_options()

    ("enb.enb_id",        bpo::value<string>(&enb_id)->default_value("0x0"),                       "eNodeB ID")
    ("enb.name",          bpo::value<string>(&args->stack.s1ap.enb_name)->default_value("srsenb01"), "eNodeB Name")
    ("enb.mcc",           bpo::value<string>(&mcc)->default_value("001"),                          "Mobile Country Code")
    ("enb.mnc",           bpo::value<string>(&mnc)->default_value("01"),                           "Mobile Network Code")
    ("enb.mme_addr",      bpo::value<string>(&args->stack.s1ap.mme_addr)->default_value("127.0.0.1"),"IP address of MME for S1 connection")
    ("enb.gtp_bind_addr", bpo::value<string>(&args->stack.s1ap.gtp_bind_addr)->default_value("192.168.3.1"), "Local IP address to bind for GTP connection")
    ("enb.s1c_bind_addr", bpo::value<string>(&args->stack.s1ap.s1c_bind_addr)->default_value("192.168.3.1"), "Local IP address to bind for S1AP connection")
    ("enb.n_prb",         bpo::value<uint32_t>(&args->enb.n_prb)->default_value(25),               "Number of PRB")
    ("enb.nof_ports",     bpo::value<uint32_t>(&args->enb.nof_ports)->default_value(1),            "Number of ports")
    ("enb.tm",            bpo::value<uint32_t>(&args->enb.transmission_mode)->default_value(1),    "Transmission mode (1-8)")
    ("enb.p_a",           bpo::value<float>(&args->enb.p_a)->default_value(0.0f),                  "Power allocation rho_a (-6, -4.77, -3, -1.77, 0, 1, 2, 3)")

    ("enb_files.sib_config", bpo::value<string>(&args->enb_files.sib_config)->default_value("sib.conf"), "SIB configuration files")
    ("enb_files.rr_config",  bpo::value<string>(&args->enb_files.rr_config)->default_value("rr.conf"),   "RR configuration files")
    ("enb_files.drb_config", bpo::value<string>(&args->enb_files.drb_config)->default_value("drb.conf"), "DRB configuration files")

    ("rf.dl_earfcn",      bpo::value<uint32_t>(&args->enb.dl_earfcn)->default_value(0), "Force Downlink EARFCN for single cell")
    ("rf.rx_gain",        bpo::value<float>(&args->rf.rx_gain)->default_value(50),        "Front-end receiver gain")
    ("rf.tx_gain",        bpo::value<float>(&args->rf.tx_gain)->default_value(70),        "Front-end transmitter gain")
    ("rf.dl_freq",        bpo::value<float>(&args->rf.dl_freq)->default_value(-1),        "Downlink Frequency (if positive overrides EARFCN)")
    ("rf.ul_freq",        bpo::value<float>(&args->rf.ul_freq)->default_value(-1),        "Uplink Frequency (if positive overrides EARFCN)")

    ("rf.device_name",       bpo::value<string>(&args->rf.device_name)->default_value("auto"),       "Front-end device name")
    ("rf.device_args",       bpo::value<string>(&args->rf.device_args)->default_value("auto"),       "Front-end device arguments")
    ("rf.time_adv_nsamples", bpo::value<string>(&args->rf.time_adv_nsamples)->default_value("auto"), "Transmission time advance")

    ("gui.enable",        bpo::value<bool>(&args->gui.enable)->default_value(false),          "Enable GUI plots")

    /* Log section */
    ("log.rf_level",     bpo::value<string>(&args->rf.log_level),         "RF log level")
    ("log.phy_level",     bpo::value<string>(&args->phy.log.phy_level),   "PHY log level")
    ("log.phy_hex_limit", bpo::value<int>(&args->phy.log.phy_hex_limit),  "PHY log hex dump limit")
    ("log.phy_lib_level", bpo::value<string>(&args->phy.log.phy_lib_level)->default_value("none"), "PHY lib log level")
    ("log.mac_level",     bpo::value<string>(&args->stack.log.mac_level),   "MAC log level")
    ("log.mac_hex_limit", bpo::value<int>(&args->stack.log.mac_hex_limit),  "MAC log hex dump limit")
    ("log.rlc_level",     bpo::value<string>(&args->stack.log.rlc_level),   "RLC log level")
    ("log.rlc_hex_limit", bpo::value<int>(&args->stack.log.rlc_hex_limit),  "RLC log hex dump limit")
    ("log.pdcp_level",    bpo::value<string>(&args->stack.log.pdcp_level),  "PDCP log level")
    ("log.pdcp_hex_limit",bpo::value<int>(&args->stack.log.pdcp_hex_limit), "PDCP log hex dump limit")
    ("log.rrc_level",     bpo::value<string>(&args->stack.log.rrc_level),   "RRC log level")
    ("log.rrc_hex_limit", bpo::value<int>(&args->stack.log.rrc_hex_limit),  "RRC log hex dump limit")
    ("log.gtpu_level",    bpo::value<string>(&args->stack.log.gtpu_level),  "GTPU log level")
    ("log.gtpu_hex_limit",bpo::value<int>(&args->stack.log.gtpu_hex_limit), "GTPU log hex dump limit")
    ("log.s1ap_level",    bpo::value<string>(&args->stack.log.s1ap_level),  "S1AP log level")
    ("log.s1ap_hex_limit",bpo::value<int>(&args->stack.log.s1ap_hex_limit), "S1AP log hex dump limit")
    ("log.stack_level",    bpo::value<string>(&args->stack.log.stack_level),  "Stack log level")
    ("log.stack_hex_limit",bpo::value<int>(&args->stack.log.stack_hex_limit), "Stack log hex dump limit")

    ("log.all_level",     bpo::value<string>(&args->log.all_level)->default_value("info"),   "ALL log level")
    ("log.all_hex_limit", bpo::value<int>(&args->log.all_hex_limit)->default_value(32),  "ALL log hex dump limit")

    ("log.filename",      bpo::value<string>(&args->log.filename)->default_value("/tmp/ue.log"),"Log filename")
    ("log.file_max_size", bpo::value<int>(&args->log.file_max_size)->default_value(-1), "Maximum file size (in kilobytes). When passed, multiple files are created. Default -1 (single file)")

    /* PCAP */
    ("pcap.enable",    bpo::value<bool>(&args->stack.mac_pcap.enable)->default_value(false),         "Enable MAC packet captures for wireshark")
    ("pcap.filename",  bpo::value<string>(&args->stack.mac_pcap.filename)->default_value("enb_mac.pcap"), "MAC layer capture filename")
    ("pcap.s1ap_enable",   bpo::value<bool>(&args->stack.s1ap_pcap.enable)->default_value(false),         "Enable S1AP packet captures for wireshark")
    ("pcap.s1ap_filename", bpo::value<string>(&args->stack.s1ap_pcap.filename)->default_value("enb_s1ap.pcap"), "S1AP layer capture filename")
   
    /* MCS section */
    ("scheduler.pdsch_mcs", bpo::value<int>(&args->stack.mac.sched.pdsch_mcs)->default_value(-1), "Optional fixed PDSCH MCS (ignores reported CQIs if specified)")
    ("scheduler.pdsch_max_mcs", bpo::value<int>(&args->stack.mac.sched.pdsch_max_mcs)->default_value(-1), "Optional PDSCH MCS limit")
    ("scheduler.pusch_mcs", bpo::value<int>(&args->stack.mac.sched.pusch_mcs)->default_value(-1), "Optional fixed PUSCH MCS (ignores reported CQIs if specified)")
    ("scheduler.pusch_max_mcs", bpo::value<int>(&args->stack.mac.sched.pusch_max_mcs)->default_value(-1), "Optional PUSCH MCS limit")
    ("scheduler.max_aggr_level", bpo::value<int>(&args->stack.mac.sched.max_aggr_level)->default_value(-1), "Optional maximum aggregation level index (l=log2(L)) ")
    ("scheduler.max_nof_ctrl_symbols", bpo::value<uint32_t>(&args->stack.mac.sched.max_nof_ctrl_symbols)->default_value(3), "Number of control symbols")
    ("scheduler.min_nof_ctrl_symbols", bpo::value<uint32_t>(&args->stack.mac.sched.min_nof_ctrl_symbols)->default_value(1), "Minimum number of control symbols")

    /* Downlink Channel emulator section */
    ("channel.dl.enable", bpo::value<bool>(&args->phy.dl_channel_args.enable)->default_value(false), "Enable/Disable internal Downlink channel emulator")
    ("channel.dl.awgn.enable", bpo::value<bool>(&args->phy.dl_channel_args.awgn_enable)->default_value(false), "Enable/Disable AWGN simulator")
    ("channel.dl.awgn.n0", bpo::value<float>(&args->phy.dl_channel_args.awgn_n0_dBfs)->default_value(-30.0f), "Noise level in decibels full scale (dBfs)")
    ("channel.dl.fading.enable", bpo::value<bool>(&args->phy.dl_channel_args.fading_enable)->default_value(false), "Enable/Disable Fading model")
    ("channel.dl.fading.model", bpo::value<std::string>(&args->phy.dl_channel_args.fading_model)->default_value("none"), "Fading model + maximum doppler (E.g. none, epa5, eva70, etu300, etc)")
    ("channel.dl.delay.enable", bpo::value<bool>(&args->phy.dl_channel_args.delay_enable)->default_value(false), "Enable/Disable Delay simulator")
    ("channel.dl.delay.period_s", bpo::value<float>(&args->phy.dl_channel_args.delay_period_s)->default_value(3600), "Delay period in seconds (integer)")
    ("channel.dl.delay.init_time_s", bpo::value<float>(&args->phy.dl_channel_args.delay_init_time_s)->default_value(0), "Initial time in seconds")
    ("channel.dl.delay.maximum_us", bpo::value<float>(&args->phy.dl_channel_args.delay_max_us)->default_value(100.0f), "Maximum delay in microseconds")
    ("channel.dl.delay.minimum_us", bpo::value<float>(&args->phy.dl_channel_args.delay_min_us)->default_value(10.0f), "Minimum delay in microseconds")
    ("channel.dl.rlf.enable", bpo::value<bool>(&args->phy.dl_channel_args.rlf_enable)->default_value(false), "Enable/Disable Radio-Link Failure simulator")
    ("channel.dl.rlf.t_on_ms", bpo::value<uint32_t >(&args->phy.dl_channel_args.rlf_t_on_ms)->default_value(10000), "Time for On state of the channel (ms)")
    ("channel.dl.rlf.t_off_ms", bpo::value<uint32_t >(&args->phy.dl_channel_args.rlf_t_off_ms)->default_value(2000), "Time for Off state of the channel (ms)")
    ("channel.dl.hst.enable", bpo::value<bool>(&args->phy.dl_channel_args.hst_enable)->default_value(false), "Enable/Disable HST simulator")
    ("channel.dl.hst.period_s", bpo::value<float>(&args->phy.dl_channel_args.hst_period_s)->default_value(7.2f), "HST simulation period in seconds")
    ("channel.dl.hst.fd_hz", bpo::value<float>(&args->phy.dl_channel_args.hst_fd_hz)->default_value(+750.0f), "Doppler frequency in Hz")
    ("channel.dl.hst.init_time_s", bpo::value<float>(&args->phy.dl_channel_args.hst_init_time_s)->default_value(0), "Initial time in seconds")

    /* Uplink Channel emulator section */
    ("channel.ul.enable", bpo::value<bool>(&args->phy.ul_channel_args.enable)->default_value(false), "Enable/Disable internal Uplink channel emulator")
    ("channel.ul.awgn.enable", bpo::value<bool>(&args->phy.ul_channel_args.awgn_enable)->default_value(false), "Enable/Disable AWGN simulator")
    ("channel.ul.awgn.n0", bpo::value<float>(&args->phy.ul_channel_args.awgn_n0_dBfs)->default_value(-30.0f), "Noise level in decibels full scale (dBfs)")
    ("channel.ul.fading.enable", bpo::value<bool>(&args->phy.ul_channel_args.fading_enable)->default_value(false), "Enable/Disable Fading model")
    ("channel.ul.fading.model", bpo::value<std::string>(&args->phy.ul_channel_args.fading_model)->default_value("none"), "Fading model + maximum doppler (E.g. none, epa5, eva70, etu300, etc)")
    ("channel.ul.delay.enable", bpo::value<bool>(&args->phy.ul_channel_args.delay_enable)->default_value(false), "Enable/Disable Delay simulator")
    ("channel.ul.delay.period_s", bpo::value<float>(&args->phy.ul_channel_args.delay_period_s)->default_value(3600), "Delay period in seconds (integer)")
    ("channel.ul.delay.init_time_s", bpo::value<float>(&args->phy.ul_channel_args.delay_init_time_s)->default_value(0), "Initial time in seconds")
    ("channel.ul.delay.maximum_us", bpo::value<float>(&args->phy.ul_channel_args.delay_max_us)->default_value(100.0f), "Maximum delay in microseconds")
    ("channel.ul.delay.minimum_us", bpo::value<float>(&args->phy.ul_channel_args.delay_min_us)->default_value(10.0f), "Minimum delay in microseconds")
    ("channel.ul.rlf.enable", bpo::value<bool>(&args->phy.ul_channel_args.rlf_enable)->default_value(false), "Enable/Disable Radio-Link Failure simulator")
    ("channel.ul.rlf.t_on_ms", bpo::value<uint32_t >(&args->phy.ul_channel_args.rlf_t_on_ms)->default_value(10000), "Time for On state of the channel (ms)")
    ("channel.ul.rlf.t_off_ms", bpo::value<uint32_t >(&args->phy.ul_channel_args.rlf_t_off_ms)->default_value(2000), "Time for Off state of the channel (ms)")
    ("channel.ul.hst.enable", bpo::value<bool>(&args->phy.ul_channel_args.hst_enable)->default_value(false), "Enable/Disable HST simulator")
    ("channel.ul.hst.period_s", bpo::value<float>(&args->phy.ul_channel_args.hst_period_s)->default_value(7.2f), "HST simulation period in seconds")
    ("channel.ul.hst.fd_hz", bpo::value<float>(&args->phy.ul_channel_args.hst_fd_hz)->default_value(-750.0f), "Doppler frequency in Hz")
    ("channel.ul.hst.init_time_s", bpo::value<float>(&args->phy.ul_channel_args.hst_init_time_s)->default_value(0), "Initial time in seconds")

      /* Expert section */
    ("expert.metrics_period_secs", bpo::value<float>(&args->general.metrics_period_secs)->default_value(1.0), "Periodicity for metrics in seconds")
    ("expert.metrics_csv_enable",  bpo::value<bool>(&args->general.metrics_csv_enable)->default_value(false), "Write metrics to CSV file")
    ("expert.metrics_csv_filename", bpo::value<string>(&args->general.metrics_csv_filename)->default_value("/tmp/enb_metrics.csv"), "Metrics CSV filename")
    ("expert.pusch_max_its", bpo::value<int>(&args->phy.pusch_max_its)->default_value(8), "Maximum number of turbo decoder iterations")
    ("expert.pusch_8bit_decoder", bpo::value<bool>(&args->phy.pusch_8bit_decoder)->default_value(false), "Use 8-bit for LLR representation and turbo decoder trellis computation (Experimental)")
    ("expert.pusch_meas_evm", bpo::value<bool>(&args->phy.pusch_meas_evm)->default_value(false), "Enable/Disable PUSCH EVM measure")
    ("expert.tx_amplitude", bpo::value<float>(&args->phy.tx_amplitude)->default_value(0.6), "Transmit amplitude factor")
    ("expert.nof_phy_threads", bpo::value<int>(&args->phy.nof_phy_threads)->default_value(3), "Number of PHY threads")
    ("expert.link_failure_nof_err", bpo::value<int>(&args->stack.mac.link_failure_nof_err)->default_value(100), "Number of PUSCH failures after which a radio-link failure is triggered")
    ("expert.max_prach_offset_us", bpo::value<float>(&args->phy.max_prach_offset_us)->default_value(30), "Maximum allowed RACH offset (in us)")
    ("expert.equalizer_mode", bpo::value<string>(&args->phy.equalizer_mode)->default_value("mmse"), "Equalizer mode")
    ("expert.estimator_fil_w", bpo::value<float>(&args->phy.estimator_fil_w)->default_value(0.1), "Chooses the coefficients for the 3-tap channel estimator centered filter.")
    ("expert.rrc_inactivity_timer", bpo::value<uint32_t>(&args->general.rrc_inactivity_timer)->default_value(60000), "Inactivity timer in ms")
    ("expert.print_buffer_state", bpo::value<bool>(&args->general.print_buffer_state)->default_value(false), "Prints on the console the buffer state every 10 seconds")
    ("expert.eea_pref_list", bpo::value<string>(&args->general.eea_pref_list)->default_value("EEA0, EEA2, EEA1"), "Ordered preference list for the selection of encryption algorithm (EEA) (default: EEA0, EEA2, EEA1).")
    ("expert.eia_pref_list", bpo::value<string>(&args->general.eia_pref_list)->default_value("EIA2, EIA1, EIA0"), "Ordered preference list for the selection of integrity algorithm (EIA) (default: EIA2, EIA1, EIA0).")

    // eMBMS section
    ("embms.enable", bpo::value<bool>(&args->stack.embms.enable)->default_value(false), "Enables MBMS in the eNB")
    ("embms.m1u_multiaddr", bpo::value<string>(&args->stack.embms.m1u_multiaddr)->default_value("239.255.0.1"), "M1-U Multicast address the eNB joins.")
    ("embms.m1u_if_addr", bpo::value<string>(&args->stack.embms.m1u_if_addr)->default_value("127.0.1.201"), "IP address of the interface the eNB will listen for M1-U traffic.")
    ;

  // Positional options - config file location
  bpo::options_description position("Positional options");
  position.add_options()
    ("config_file", bpo::value< string >(&config_file), "eNodeB configuration file")
  ;

  // clang-format on
  bpo::positional_options_description p;
  p.add("config_file", -1);

  // these options are allowed on the command line
  bpo::options_description cmdline_options;
  cmdline_options.add(common).add(position).add(general);

  // parse the command line and store result in vm
  bpo::variables_map vm;
  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error& e) {
    cerr << e.what() << endl;
    exit(1);
  }
  // help option was given - print usage and exit
  if (vm.count("help")) {
    cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << endl << endl;
    cout << common << endl << general << endl;
    exit(0);
  }

  // print version number and exit
  if (vm.count("version")) {
    cout << "Version " << srslte_get_version_major() << "." << srslte_get_version_minor() << "."
         << srslte_get_version_patch() << endl;
    exit(0);
  }

  // if no config file given, check users home path
  if (!vm.count("config_file")) {
    if (!config_exists(config_file, "enb.conf")) {
      cout << "Failed to read eNB configuration file " << config_file << " - exiting" << endl;
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

  // Convert MCC/MNC strings
  if (!srslte::string_to_mcc(mcc, &args->stack.s1ap.mcc)) {
    cout << "Error parsing enb.mcc:" << mcc << " - must be a 3-digit string." << endl;
  }
  if (!srslte::string_to_mnc(mnc, &args->stack.s1ap.mnc)) {
    cout << "Error parsing enb.mnc:" << mnc << " - must be a 2 or 3-digit string." << endl;
  }

  if (args->stack.embms.enable) {
    if (args->stack.mac.sched.max_nof_ctrl_symbols == 3) {
      fprintf(stderr,
              "nof_ctrl_symbols = %d, While using MBMS, please set number of control symbols to either 1 or 2, "
              "depending on the length of the non-mbsfn region\n",
              args->stack.mac.sched.max_nof_ctrl_symbols);
      exit(1);
    }
  }

  // Covert eNB Id
  std::size_t pos = {};
  try {
    args->enb.enb_id = std::stoi(enb_id, &pos, 0);
  } catch (...) {
    cout << "Error parsing enb.enb_id: " << enb_id << "." << endl;
    exit(1);
  }
  if (pos != enb_id.size()) {
    cout << "Error parsing enb.enb_id: " << enb_id << "." << endl;
    exit(1);
  }

  // Apply all_level to any unset layers
  if (vm.count("log.all_level")) {
    if (!vm.count("log.rf_level")) {
      args->rf.log_level = args->log.all_level;
    }
    if (!vm.count("log.phy_level")) {
      args->phy.log.phy_level = args->log.all_level;
    }
    if (!vm.count("log.phy_lib_level")) {
      args->phy.log.phy_lib_level = args->log.all_level;
    }
    if (!vm.count("log.mac_level")) {
      args->stack.log.mac_level = args->log.all_level;
    }
    if (!vm.count("log.rlc_level")) {
      args->stack.log.rlc_level = args->log.all_level;
    }
    if (!vm.count("log.pdcp_level")) {
      args->stack.log.pdcp_level = args->log.all_level;
    }
    if (!vm.count("log.rrc_level")) {
      args->stack.log.rrc_level = args->log.all_level;
    }
    if (!vm.count("log.gtpu_level")) {
      args->stack.log.gtpu_level = args->log.all_level;
    }
    if (!vm.count("log.s1ap_level")) {
      args->stack.log.s1ap_level = args->log.all_level;
    }
    if (!vm.count("log.stack_level")) {
      args->stack.log.stack_level = args->log.all_level;
    }
  }

  // Apply all_hex_limit to any unset layers
  if (vm.count("log.all_hex_limit")) {
    if (!vm.count("log.phy_hex_limit")) {
      args->log.phy_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.mac_hex_limit")) {
      args->stack.log.mac_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.rlc_hex_limit")) {
      args->stack.log.rlc_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.pdcp_hex_limit")) {
      args->stack.log.pdcp_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.rrc_hex_limit")) {
      args->stack.log.rrc_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.gtpu_hex_limit")) {
      args->stack.log.gtpu_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.s1ap_hex_limit")) {
      args->stack.log.s1ap_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.stack_hex_limit")) {
      args->stack.log.stack_hex_limit = args->log.all_hex_limit;
    }
  }

  // Check remaining eNB config files
  if (!config_exists(args->enb_files.sib_config, "sib.conf")) {
    cout << "Failed to read SIB configuration file " << args->enb_files.sib_config << " - exiting" << endl;
    exit(1);
  }

  if (!config_exists(args->enb_files.rr_config, "rr.conf")) {
    cout << "Failed to read RR configuration file " << args->enb_files.rr_config << " - exiting" << endl;
    exit(1);
  }

  if (!config_exists(args->enb_files.drb_config, "drb.conf")) {
    cout << "Failed to read DRB configuration file " << args->enb_files.drb_config << " - exiting" << endl;
    exit(1);
  }
}

static bool do_metrics = false;

void* input_loop(void* m)
{
  metrics_stdout* metrics = (metrics_stdout*)m;
  char            key;
  while (running) {
    cin >> key;
    if (cin.eof() || cin.bad()) {
      cout << "Closing stdin thread." << endl;
      break;
    } else {
      if ('t' == key) {
        do_metrics = !do_metrics;
        if (do_metrics) {
          cout << "Enter t to stop trace." << endl;
        } else {
          cout << "Enter t to restart trace." << endl;
        }
        metrics->toggle_print(do_metrics);
      } else if ('q' == key) {
        raise(SIGTERM);
      }
    }
  }
  return nullptr;
}

int main(int argc, char* argv[])
{
  srslte_register_signal_handler();
  all_args_t                         args = {};
  srslte::metrics_hub<enb_metrics_t> metricshub;
  metrics_stdout                     metrics_screen;

  cout << "---  Software Radio Systems LTE eNodeB  ---" << endl << endl;

  srslte_debug_handle_crash(argc, argv);
  parse_args(&args, argc, argv);

  srslte::logger_stdout logger_stdout;

  // Set logger
  srslte::logger* logger = nullptr;
  if (args.log.filename == "stdout") {
    logger = &logger_stdout;
  } else {
    logger_file.init(args.log.filename, args.log.file_max_size);
    logger = &logger_file;
  }
  srslte::logmap::set_default_logger(logger);
  srslte::logmap::get("COMMON")->set_level(srslte::LOG_LEVEL_INFO);

  // Create eNB
  unique_ptr<srsenb::enb> enb{new srsenb::enb};
  if (enb->init(args, logger) != SRSLTE_SUCCESS) {
    enb->stop();
    return SRSLTE_ERROR;
  }

  // Set metrics
  metricshub.init(enb.get(), args.general.metrics_period_secs);
  metricshub.add_listener(&metrics_screen);
  metrics_screen.set_handle(enb.get());

  srsenb::metrics_csv metrics_file(args.general.metrics_csv_filename);
  if (args.general.metrics_csv_enable) {
    metricshub.add_listener(&metrics_file);
    metrics_file.set_handle(enb.get());
  }

  // create input thread
  pthread_t input;
  pthread_create(&input, nullptr, &input_loop, &metrics_screen);

  bool signals_pregenerated = false;
  if (running) {
    if (args.gui.enable) {
      enb->start_plot();
    }
  }
  int cnt = 0;
  while (running) {
    if (args.general.print_buffer_state) {
      cnt++;
      if (cnt == 1000) {
        cnt = 0;
        enb->print_pool();
      }
    }
    usleep(10000);
  }
  pthread_cancel(input);
  pthread_join(input, NULL);
  metricshub.stop();
  enb->stop();
  cout << "---  exiting  ---" << endl;

  return SRSLTE_SUCCESS;
}
