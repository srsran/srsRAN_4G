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

#include "srslte/common/config_file.h"
#include "srslte/common/crash_handler.h"
#include "srslte/common/logmap.h"
#include "srslte/common/metrics_hub.h"
#include "srslte/common/signal_handler.h"
#include "srslte/srslte.h"
#include "srslte/version.h"
#include "srsue/hdr/metrics_csv.h"
#include "srsue/hdr/metrics_stdout.h"
#include "srsue/hdr/ue.h"
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

extern bool simulate_rlf;

using namespace std;
using namespace srsue;
namespace bpo = boost::program_options;

/**********************************************************************
 *  Local static variables
 ***********************************************************************/

static bool            do_metrics     = false;
static metrics_stdout* metrics_screen = nullptr;

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
string config_file;

static int parse_args(all_args_t* args, int argc, char* argv[])
{
  // Command line only options
  bpo::options_description general("General options");

  general.add_options()("help,h", "Produce help message")("version,v", "Print version information and exit");

  // Command line or config file options
  bpo::options_description common("Configuration options");
  // clang-format off
  common.add_options()
    ("ue.radio", bpo::value<string>(&args->rf.type)->default_value("multi"), "Type of the radio [multi]")
    ("ue.phy", bpo::value<string>(&args->phy.type)->default_value("lte"), "Type of the PHY [lte]")
    ("ue.stack", bpo::value<string>(&args->stack.type)->default_value("lte"), "Type of the upper stack [lte]")

    ("rf.dl_earfcn",    bpo::value<string>(&args->phy.dl_earfcn)->default_value("3400"), "Downlink EARFCN list")
    ("rf.ul_earfcn",    bpo::value<string>(&args->phy.ul_earfcn), "Uplink EARFCN list. Optional.")
    ("rf.freq_offset",  bpo::value<float>(&args->rf.freq_offset)->default_value(0), "(optional) Frequency offset")
    ("rf.dl_freq",      bpo::value<float>(&args->phy.dl_freq)->default_value(-1),      "Downlink Frequency (if positive overrides EARFCN)")
    ("rf.ul_freq",      bpo::value<float>(&args->phy.ul_freq)->default_value(-1),      "Uplink Frequency (if positive overrides EARFCN)")
    ("rf.rx_gain",      bpo::value<float>(&args->rf.rx_gain)->default_value(-1), "Front-end receiver gain")
    ("rf.tx_gain",      bpo::value<float>(&args->rf.tx_gain)->default_value(-1), "Front-end transmitter gain")
    ("rf.nof_carriers", bpo::value<uint32_t>(&args->rf.nof_carriers)->default_value(1), "Number of carriers")
    ("rf.nof_antennas", bpo::value<uint32_t>(&args->rf.nof_antennas)->default_value(1), "Number of antennas per carrier")

    ("rf.device_name", bpo::value<string>(&args->rf.device_name)->default_value("auto"), "Front-end device name")
    ("rf.device_args", bpo::value<string>(&args->rf.device_args)->default_value("auto"), "Front-end device arguments")
    ("rf.time_adv_nsamples", bpo::value<string>(&args->rf.time_adv_nsamples)->default_value("auto"), "Transmission time advance")
    ("rf.continuous_tx", bpo::value<string>(&args->rf.continuous_tx)->default_value("auto"), "Transmit samples continuously to the radio or on bursts (auto/yes/no). Default is auto (yes for UHD, no for rest)")

    ("rf.bands.rx[0].min", bpo::value<float>(&args->rf.ch_rx_bands[0].min)->default_value(0), "Lower frequency boundary for CH0-RX")
    ("rf.bands.rx[0].max", bpo::value<float>(&args->rf.ch_rx_bands[0].max)->default_value(0), "Higher frequency boundary for CH0-RX")
    ("rf.bands.rx[1].min", bpo::value<float>(&args->rf.ch_rx_bands[1].min)->default_value(0), "Lower frequency boundary for CH1-RX")
    ("rf.bands.rx[1].max", bpo::value<float>(&args->rf.ch_rx_bands[1].max)->default_value(0), "Higher frequency boundary for CH1-RX")
    ("rf.bands.rx[2].min", bpo::value<float>(&args->rf.ch_rx_bands[2].min)->default_value(0), "Lower frequency boundary for CH2-RX")
    ("rf.bands.rx[2].max", bpo::value<float>(&args->rf.ch_rx_bands[2].max)->default_value(0), "Higher frequency boundary for CH2-RX")
    ("rf.bands.rx[3].min", bpo::value<float>(&args->rf.ch_rx_bands[3].min)->default_value(0), "Lower frequency boundary for CH3-RX")
    ("rf.bands.rx[3].max", bpo::value<float>(&args->rf.ch_rx_bands[3].max)->default_value(0), "Higher frequency boundary for CH3-RX")
    ("rf.bands.rx[4].min", bpo::value<float>(&args->rf.ch_rx_bands[4].min)->default_value(0), "Lower frequency boundary for CH4-RX")
    ("rf.bands.rx[4].max", bpo::value<float>(&args->rf.ch_rx_bands[4].max)->default_value(0), "Higher frequency boundary for CH4-RX")

    ("rf.bands.tx[0].min", bpo::value<float>(&args->rf.ch_tx_bands[0].min)->default_value(0), "Lower frequency boundary for CH1-TX")
    ("rf.bands.tx[0].max", bpo::value<float>(&args->rf.ch_tx_bands[0].max)->default_value(0), "Higher frequency boundary for CH1-TX")
    ("rf.bands.tx[1].min", bpo::value<float>(&args->rf.ch_tx_bands[1].min)->default_value(0), "Lower frequency boundary for CH1-TX")
    ("rf.bands.tx[1].max", bpo::value<float>(&args->rf.ch_tx_bands[1].max)->default_value(0), "Higher frequency boundary for CH1-TX")
    ("rf.bands.tx[2].min", bpo::value<float>(&args->rf.ch_tx_bands[2].min)->default_value(0), "Lower frequency boundary for CH2-TX")
    ("rf.bands.tx[2].max", bpo::value<float>(&args->rf.ch_tx_bands[2].max)->default_value(0), "Higher frequency boundary for CH2-TX")
    ("rf.bands.tx[3].min", bpo::value<float>(&args->rf.ch_tx_bands[3].min)->default_value(0), "Lower frequency boundary for CH3-TX")
    ("rf.bands.tx[3].max", bpo::value<float>(&args->rf.ch_tx_bands[3].max)->default_value(0), "Higher frequency boundary for CH3-TX")
    ("rf.bands.tx[4].min", bpo::value<float>(&args->rf.ch_tx_bands[4].min)->default_value(0), "Lower frequency boundary for CH4-TX")
    ("rf.bands.tx[4].max", bpo::value<float>(&args->rf.ch_tx_bands[4].max)->default_value(0), "Higher frequency boundary for CH4-TX")

    ("rrc.feature_group", bpo::value<uint32_t>(&args->stack.rrc.feature_group)->default_value(0xe6041000), "Hex value of the featureGroupIndicators field in the"
                                                                                           "UECapabilityInformation message. Default 0xe6041000")
    ("rrc.ue_category",       bpo::value<string>(&args->stack.rrc.ue_category_str)->default_value(SRSLTE_UE_CATEGORY_DEFAULT),  "UE Category (1 to 10)")
    ("rrc.ue_category_dl",       bpo::value<int>(&args->stack.rrc.ue_category_dl)->default_value(-1),  "UE Category DL v12 (valid values: 0, 4, 6, 7, 9 to 16)")
    ("rrc.ue_category_ul",       bpo::value<int>(&args->stack.rrc.ue_category_ul)->default_value(-1),  "UE Category UL v12 (valid values: 0, 3, 5, 7, 8 and 13)")
    ("rrc.release",           bpo::value<uint32_t>(&args->stack.rrc.release)->default_value(SRSLTE_RELEASE_DEFAULT),  "UE Release (8 to 12)")
    ("rrc.mbms_service_id",   bpo::value<int32_t>(&args->stack.rrc.mbms_service_id)->default_value(-1),  "MBMS service id for autostart (-1 means disabled)")
    ("rrc.mbms_service_port", bpo::value<uint32_t>(&args->stack.rrc.mbms_service_port)->default_value(4321),  "Port of the MBMS service")

    ("nas.apn",               bpo::value<string>(&args->stack.nas.apn_name)->default_value(""),          "Set Access Point Name (APN) for data services")
    ("nas.apn_protocol",      bpo::value<string>(&args->stack.nas.apn_protocol)->default_value(""),  "Set Access Point Name (APN) protocol for data services")
    ("nas.user",              bpo::value<string>(&args->stack.nas.apn_user)->default_value(""),  "Username for CHAP authentication")
    ("nas.pass",              bpo::value<string>(&args->stack.nas.apn_pass)->default_value(""),  "Password for CHAP authentication")
    ("nas.force_imsi_attach", bpo::value<bool>(&args->stack.nas.force_imsi_attach)->default_value(false),  "Whether to always perform an IMSI attach")
    ("nas.eia",               bpo::value<string>(&args->stack.nas.eia)->default_value("1,2,3"),  "List of integrity algorithms included in UE capabilities")
    ("nas.eea",               bpo::value<string>(&args->stack.nas.eea)->default_value("0,1,2,3"),  "List of ciphering algorithms included in UE capabilities")

    ("pcap.enable", bpo::value<bool>(&args->stack.pcap.enable)->default_value(false), "Enable MAC packet captures for wireshark")
    ("pcap.filename", bpo::value<string>(&args->stack.pcap.filename)->default_value("ue.pcap"), "MAC layer capture filename")
    ("pcap.nas_enable",   bpo::value<bool>(&args->stack.pcap.nas_enable)->default_value(false), "Enable NAS packet captures for wireshark")
    ("pcap.nas_filename", bpo::value<string>(&args->stack.pcap.nas_filename)->default_value("ue_nas.pcap"), "NAS layer capture filename (useful when NAS encryption is enabled)")

    ("gui.enable", bpo::value<bool>(&args->gui.enable)->default_value(false), "Enable GUI plots")

    ("log.rf_level", bpo::value<string>(&args->rf.log_level), "RF log level")
    ("log.phy_level", bpo::value<string>(&args->phy.log.phy_level), "PHY log level")
    ("log.phy_lib_level", bpo::value<string>(&args->phy.log.phy_lib_level), "PHY lib log level")
    ("log.phy_hex_limit", bpo::value<int>(&args->phy.log.phy_hex_limit), "PHY log hex dump limit")
    ("log.mac_level", bpo::value<string>(&args->stack.log.mac_level), "MAC log level")
    ("log.mac_hex_limit", bpo::value<int>(&args->stack.log.mac_hex_limit), "MAC log hex dump limit")
    ("log.rlc_level", bpo::value<string>(&args->stack.log.rlc_level), "RLC log level")
    ("log.rlc_hex_limit", bpo::value<int>(&args->stack.log.rlc_hex_limit), "RLC log hex dump limit")
    ("log.pdcp_level", bpo::value<string>(&args->stack.log.pdcp_level), "PDCP log level")
    ("log.pdcp_hex_limit", bpo::value<int>(&args->stack.log.pdcp_hex_limit), "PDCP log hex dump limit")
    ("log.rrc_level", bpo::value<string>(&args->stack.log.rrc_level), "RRC log level")
    ("log.rrc_hex_limit", bpo::value<int>(&args->stack.log.rrc_hex_limit), "RRC log hex dump limit")
    ("log.gw_level", bpo::value<string>(&args->gw.log.gw_level), "GW log level")
    ("log.gw_hex_limit", bpo::value<int>(&args->gw.log.gw_hex_limit), "GW log hex dump limit")
    ("log.nas_level", bpo::value<string>(&args->stack.log.nas_level), "NAS log level")
    ("log.nas_hex_limit", bpo::value<int>(&args->stack.log.nas_hex_limit), "NAS log hex dump limit")
    ("log.usim_level", bpo::value<string>(&args->stack.log.usim_level), "USIM log level")
    ("log.usim_hex_limit", bpo::value<int>(&args->stack.log.usim_hex_limit), "USIM log hex dump limit")
    ("log.stack_level", bpo::value<string>(&args->stack.log.stack_level), "Stack log level")
    ("log.stack_hex_limit", bpo::value<int>(&args->stack.log.stack_hex_limit), "Stack log hex dump limit")

    ("log.all_level", bpo::value<string>(&args->log.all_level)->default_value("info"), "ALL log level")
    ("log.all_hex_limit", bpo::value<int>(&args->log.all_hex_limit)->default_value(32), "ALL log hex dump limit")

    ("log.filename", bpo::value<string>(&args->log.filename)->default_value("/tmp/ue.log"), "Log filename")
    ("log.file_max_size", bpo::value<int>(&args->log.file_max_size)->default_value(-1), "Maximum file size (in kilobytes). When passed, multiple files are created. Default -1 (single file)")

    ("usim.mode", bpo::value<string>(&args->stack.usim.mode)->default_value("soft"), "USIM mode (soft or pcsc)")
    ("usim.algo", bpo::value<string>(&args->stack.usim.algo), "USIM authentication algorithm")
    ("usim.op", bpo::value<string>(&args->stack.usim.op), "USIM operator code")
    ("usim.opc", bpo::value<string>(&args->stack.usim.opc), "USIM operator code (ciphered variant)")
    ("usim.imsi", bpo::value<string>(&args->stack.usim.imsi), "USIM IMSI")
    ("usim.imei", bpo::value<string>(&args->stack.usim.imei), "USIM IMEI")
    ("usim.k", bpo::value<string>(&args->stack.usim.k), "USIM K")
    ("usim.pin", bpo::value<string>(&args->stack.usim.pin), "PIN in case real SIM card is used")
    ("usim.reader", bpo::value<string>(&args->stack.usim.reader)->default_value(""), "Force specific PCSC reader. Default: Try all available readers.")

    ("gw.netns", bpo::value<string>(&args->gw.netns)->default_value(""), "Network namespace to for TUN device (empty for default netns)")
    ("gw.ip_devname", bpo::value<string>(&args->gw.tun_dev_name)->default_value("tun_srsue"), "Name of the tun_srsue device")
    ("gw.ip_netmask", bpo::value<string>(&args->gw.tun_dev_netmask)->default_value("255.255.255.0"), "Netmask of the tun_srsue device")

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

    /* PHY section */
    ("phy.worker_cpu_mask",
     bpo::value<int>(&args->phy.worker_cpu_mask)->default_value(-1),
     "cpu bit mask (eg 255 = 1111 1111)")

    ("phy.sync_cpu_affinity",
     bpo::value<int>(&args->phy.sync_cpu_affinity)->default_value(-1),
     "index of the core used by the sync thread")

    ("phy.rx_gain_offset",
     bpo::value<float>(&args->phy.rx_gain_offset)->default_value(62),
     "RX Gain offset to add to rx_gain to correct RSRP value")

    ("phy.prach_gain",
     bpo::value<float>(&args->phy.prach_gain)->default_value(-1.0),
     "Disable PRACH power control")

    ("phy.cqi_max",
     bpo::value<int>(&args->phy.cqi_max)->default_value(15),
     "Upper bound on the maximum CQI to be reported. Default 15.")

    ("phy.cqi_fixed",
     bpo::value<int>(&args->phy.cqi_fixed)->default_value(-1),
     "Fixes the reported CQI to a constant value. Default disabled.")

    ("phy.sfo_correct_period",
     bpo::value<uint32_t>(&args->phy.sfo_correct_period)->default_value(DEFAULT_SAMPLE_OFFSET_CORRECT_PERIOD),
     "Period in ms to correct sample time")

    ("phy.sfo_emma",
     bpo::value<float>(&args->phy.sfo_ema)->default_value(DEFAULT_SFO_EMA_COEFF),
     "EMA coefficient to average sample offsets used to compute SFO")

    ("phy.snr_ema_coeff",
     bpo::value<float>(&args->phy.snr_ema_coeff)->default_value(0.1),
     "Sets the SNR exponential moving average coefficient (Default 0.1)")

    ("phy.snr_estim_alg",
     bpo::value<string>(&args->phy.snr_estim_alg)->default_value("refs"),
     "Sets the noise estimation algorithm. (Default refs)")

    ("phy.pdsch_max_its",
     bpo::value<uint32_t>(&args->phy.pdsch_max_its)->default_value(8),
     "Maximum number of turbo decoder iterations")

    ("phy.meas_evm",
     bpo::value<bool>(&args->phy.meas_evm)->default_value(false),
     "Measure PDSCH EVM, increases CPU load (default false)")

    ("phy.nof_phy_threads",
     bpo::value<int>(&args->phy.nof_phy_threads)->default_value(3),
     "Number of PHY threads")

    ("phy.equalizer_mode",
     bpo::value<string>(&args->phy.equalizer_mode)->default_value("mmse"),
     "Equalizer mode")

    ("phy.intra_freq_meas_len_ms",
       bpo::value<uint32_t>(&args->phy.intra_freq_meas_len_ms)->default_value(20),
       "Duration of the intra-frequency neighbour cell measurement in ms.")

    ("phy.intra_freq_meas_period_ms",
       bpo::value<uint32_t>(&args->phy.intra_freq_meas_period_ms)->default_value(200),
       "Period of intra-frequency neighbour cell measurement in ms. Maximum as per 3GPP is 200 ms.")

    ("phy.correct_sync_error",
       bpo::value<bool>(&args->phy.correct_sync_error)->default_value(false),
       "Channel estimator measures and pre-compensates time synchronization error. Increases CPU usage, improves PDSCH "
       "decoding in high SFO and high speed UE scenarios.")

    ("phy.cfo_is_doppler",
       bpo::value<bool>(&args->phy.cfo_is_doppler)->default_value(false),
       "Assume detected CFO is doppler and correct the UL in the same direction. If disabled, the CFO is assumed"
       "to be caused by the local oscillator and the UL correction is in the opposite direction. Default assumes oscillator.")

    ("phy.cfo_integer_enabled",
     bpo::value<bool>(&args->phy.cfo_integer_enabled)->default_value(false),
     "Enables integer CFO estimation and correction.")

    ("phy.cfo_correct_tol_hz",
     bpo::value<float>(&args->phy.cfo_correct_tol_hz)->default_value(1.0),
     "Tolerance (in Hz) for digital CFO compensation (needs to be low if interpolate_subframe_enabled=true.")

    ("phy.cfo_pss_ema",
     bpo::value<float>(&args->phy.cfo_pss_ema)->default_value(DEFAULT_CFO_EMA_TRACK),
     "CFO Exponential Moving Average coefficient for PSS estimation during TRACK.")

    ("phy.cfo_ref_mask",
     bpo::value<uint32_t>(&args->phy.cfo_ref_mask)->default_value(1023),
     "Bitmask for subframes on which to run RS estimation (set to 0 to disable, default all sf)")

    ("phy.cfo_loop_bw_pss",
     bpo::value<float>(&args->phy.cfo_loop_bw_pss)->default_value(DEFAULT_CFO_BW_PSS),
     "CFO feedback loop bandwidth for samples from PSS")

    ("phy.cfo_loop_bw_ref",
     bpo::value<float>(&args->phy.cfo_loop_bw_ref)->default_value(DEFAULT_CFO_BW_REF),
     "CFO feedback loop bandwidth for samples from RS")

    ("phy.cfo_loop_pss_tol",
     bpo::value<float>(&args->phy.cfo_loop_pss_tol)->default_value(DEFAULT_CFO_PSS_MIN),
     "Tolerance (in Hz) of the PSS estimation method. Below this value, PSS estimation does not feeds back the loop"
     "and RS estimations are used instead (when available)")

    ("phy.cfo_loop_ref_min",
     bpo::value<float>(&args->phy.cfo_loop_ref_min)->default_value(DEFAULT_CFO_REF_MIN),
     "Tolerance (in Hz) of the RS estimation method. Below this value, RS estimation does not feeds back the loop")

    ("phy.cfo_loop_pss_conv",
     bpo::value<uint32_t>(&args->phy.cfo_loop_pss_conv)->default_value(DEFAULT_PSS_STABLE_TIMEOUT),
     "After the PSS estimation is below cfo_loop_pss_tol for cfo_loop_pss_timeout times consecutively, RS adjustments are allowed.")

    ("phy.interpolate_subframe_enabled",
     bpo::value<bool>(&args->phy.interpolate_subframe_enabled)->default_value(false),
     "Interpolates in the time domain the channel estimates within 1 subframe.")

    ("phy.estimator_fil_auto",
     bpo::value<bool>(&args->phy.estimator_fil_auto)->default_value(false),
     "The channel estimator smooths the channel estimate with an adaptative filter.")

    ("phy.estimator_fil_stddev",
     bpo::value<float>(&args->phy.estimator_fil_stddev)->default_value(1.0f),
     "Sets the channel estimator smooth gaussian filter standard deviation.")

    ("phy.estimator_fil_order",
     bpo::value<uint32_t>(&args->phy.estimator_fil_order)->default_value(4),
     "Sets the channel estimator smooth gaussian filter order (even values perform better).")

    ("phy.snr_to_cqi_offset",
     bpo::value<float>(&args->phy.snr_to_cqi_offset)->default_value(0),
     "Sets an offset in the SNR to CQI table. This is used to adjust the reported CQI.")

    ("phy.sss_algorithm",
     bpo::value<string>(&args->phy.sss_algorithm)->default_value("full"),
     "Selects the SSS estimation algorithm.")

    ("phy.pdsch_csi_enabled",
     bpo::value<bool>(&args->phy.pdsch_csi_enabled)->default_value(true),
     "Stores the Channel State Information and uses it for weightening the softbits. It is only used in TM1.")

    ("phy.pdsch_8bit_decoder",
       bpo::value<bool>(&args->phy.pdsch_8bit_decoder)->default_value(false),
       "Use 8-bit for LLR representation and turbo decoder trellis computation (Experimental)")

    ("phy.force_ul_amplitude",
       bpo::value<float>(&args->phy.force_ul_amplitude)->default_value(0.0),
       "Forces the peak amplitude in the PUCCH, PUSCH and SRS (set 0.0 to 1.0, set to 0 or negative for disabling)")

    ("phy.in_sync_rsrp_dbm_th",
     bpo::value<float>(&args->phy.in_sync_rsrp_dbm_th)->default_value(-130.0f),
     "RSRP threshold (in dBm) above which the UE considers to be in-sync")

    ("phy.in_sync_snr_db_th",
     bpo::value<float>(&args->phy.in_sync_snr_db_th)->default_value(3.0f),
     "SNR threshold (in dB) above which the UE considers to be in-sync")

    ("phy.nof_in_sync_events",
     bpo::value<uint32_t>(&args->phy.nof_in_sync_events)->default_value(10),
     "Number of PHY in-sync events before sending an in-sync event to RRC")

    ("phy.nof_out_of_sync_events",
     bpo::value<uint32_t>(&args->phy.nof_out_of_sync_events)->default_value(20),
     "Number of PHY out-sync events before sending an out-sync event to RRC")

    // UE simulation args
    ("sim.airplane_t_on_ms",
     bpo::value<int>(&args->stack.nas.sim.airplane_t_on_ms)->default_value(-1),
     "On-time for airplane mode (in ms)")

    ("sim.airplane_t_off_ms",
     bpo::value<int>(&args->stack.nas.sim.airplane_t_off_ms)->default_value(-1),
     "Off-time for airplane mode (in ms)")

     /* general options */
    ("general.metrics_period_secs",
       bpo::value<float>(&args->general.metrics_period_secs)->default_value(1.0),
      "Periodicity for metrics in seconds")

    ("general.metrics_csv_enable",
       bpo::value<bool>(&args->general.metrics_csv_enable)->default_value(false),
       "Write UE metrics to CSV file")

    ("general.metrics_csv_filename",
       bpo::value<string>(&args->general.metrics_csv_filename)->default_value("/tmp/ue_metrics.csv"),
       "Metrics CSV filename")

    ("general.metrics_csv_append",
           bpo::value<bool>(&args->general.metrics_csv_append)->default_value(false),
           "Set to true to append new output to existing CSV file")

    ("general.metrics_csv_flush_period_sec",
           bpo::value<int>(&args->general.metrics_csv_flush_period_sec)->default_value(-1),
           "Periodicity in s to flush CSV file to disk (-1 for auto)")

    ("stack.have_tti_time_stats",
        bpo::value<bool>(&args->stack.have_tti_time_stats)->default_value(true),
        "Calculate TTI execution statistics");

  // Positional options - config file location
  bpo::options_description position("Positional options");
  position.add_options()
    ("config_file", bpo::value<string>(&config_file), "UE configuration file");
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
    return SRSLTE_ERROR;
  }
  // help option was given - print usage and exit
  if (vm.count("help")) {
    cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << endl << endl;
    cout << common << endl << general << endl;
    exit(SRSLTE_SUCCESS);
  }

  // print version number and exit
  if (vm.count("version")) {
    cout << "Version " << srslte_get_version_major() << "." << srslte_get_version_minor() << "."
         << srslte_get_version_patch() << endl;
    exit(SRSLTE_SUCCESS);
  }

  // if no config file given, check users home path
  if (!vm.count("config_file")) {

    if (!config_exists(config_file, "ue.conf")) {
      cout << "Failed to read UE configuration file " << config_file << " - exiting" << endl;
      return SRSLTE_ERROR;
    }
  }

  cout << "Reading configuration file " << config_file << "..." << endl;
  ifstream conf(config_file.c_str(), ios::in);
  if (conf.fail()) {
    cout << "Failed to read configuration file " << config_file << " - exiting" << endl;
    return SRSLTE_ERROR;
  }

  // parse config file and handle errors gracefully
  try {
    bpo::store(bpo::parse_config_file(conf, common), vm);
    bpo::notify(vm);
  } catch (const boost::program_options::error& e) {
    cerr << e.what() << endl;
    return SRSLTE_ERROR;
  }

  // Check conflicting OP/OPc options and which is being used
  if (vm.count("usim.op") && !vm["usim.op"].defaulted() && vm.count("usim.opc") && !vm["usim.opc"].defaulted()) {
    cout << "Conflicting options OP and OPc. Please configure either one or the other." << endl;
    return SRSLTE_ERROR;
  } else {
    args->stack.usim.using_op = vm.count("usim.op");
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
    if (!vm.count("log.nas_level")) {
      args->stack.log.nas_level = args->log.all_level;
    }
    if (!vm.count("log.gw_level")) {
      args->gw.log.gw_level = args->log.all_level;
    }
    if (!vm.count("log.usim_level")) {
      args->stack.log.usim_level = args->log.all_level;
    }
    if (!vm.count("log.stack_level")) {
      args->stack.log.stack_level = args->log.all_level;
    }
  }

  // Apply all_hex_limit to any unset layers
  if (vm.count("log.all_hex_limit")) {
    if (!vm.count("log.phy_hex_limit")) {
      args->phy.log.phy_hex_limit = args->log.all_hex_limit;
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
    if (!vm.count("log.nas_hex_limit")) {
      args->stack.log.nas_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.gw_hex_limit")) {
      args->gw.log.gw_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.usim_hex_limit")) {
      args->stack.log.usim_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.stack_hex_limit")) {
      args->stack.log.stack_hex_limit = args->log.all_hex_limit;
    }
  }

  return SRSLTE_SUCCESS;
}

static void* input_loop(void*)
{
  string key;
  while (running) {
    getline(cin, key);
    if (cin.eof() || cin.bad()) {
      cout << "Closing stdin thread." << endl;
      break;
    } else {
      if (key == "t") {
        do_metrics = !do_metrics;
        if (do_metrics) {
          cout << "Enter t to stop trace." << endl;
        } else {
          cout << "Enter t to restart trace." << endl;
        }
        if (metrics_screen) {
          metrics_screen->toggle_print(do_metrics);
        }
      } else if (key == "rlf") {
        simulate_rlf = true;
        cout << "Sending Radio Link Failure" << endl;
      } else if (key == "q") {
        // let the signal handler do the job
        raise(SIGTERM);
      }
    }
  }
  return nullptr;
}

int main(int argc, char* argv[])
{
  srslte_register_signal_handler();
  srslte_debug_handle_crash(argc, argv);

  all_args_t args = {};
  if (parse_args(&args, argc, argv) != SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  };

  // Setup logging
  srslte::logger_stdout logger_stdout;
  srslte::logger*       logger = nullptr;
  if (args.log.filename == "stdout") {
    logger = &logger_stdout;
  } else {
    logger_file.init(args.log.filename, args.log.file_max_size);
    logger = &logger_file;
  }
  srslte::logmap::set_default_logger(logger);

  // Create UE instance
  srsue::ue ue;
  if (ue.init(args, logger)) {
    ue.stop();
    return SRSLTE_SUCCESS;
  }

  srslte::metrics_hub<ue_metrics_t> metricshub;
  metrics_stdout                    _metrics_screen;

  metrics_screen = &_metrics_screen;
  metricshub.init(&ue, args.general.metrics_period_secs);
  metricshub.add_listener(metrics_screen);
  metrics_screen->set_ue_handle(&ue);

  metrics_csv metrics_file(args.general.metrics_csv_filename, args.general.metrics_csv_append);
  if (args.general.metrics_csv_enable) {
    metricshub.add_listener(&metrics_file);
    metrics_file.set_ue_handle(&ue);
    if (args.general.metrics_csv_flush_period_sec > 0) {
      metrics_file.set_flush_period((uint32_t)args.general.metrics_csv_flush_period_sec);
    }
  }

  pthread_t input;
  pthread_create(&input, nullptr, &input_loop, &args);

  cout << "Attaching UE..." << endl;
  ue.switch_on();

  if (args.gui.enable) {
    ue.start_plot();
  }

  while (running) {
    sleep(1);
  }

  ue.switch_off();
  pthread_cancel(input);
  pthread_join(input, nullptr);
  metricshub.stop();
  metrics_file.stop();
  ue.stop();
  cout << "---  exiting  ---" << endl;

  return SRSLTE_SUCCESS;
}
