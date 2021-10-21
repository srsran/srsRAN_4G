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

#include "dummy_gnb_stack.h"
#include "dummy_ue_stack.h"
#include "srsran/common/phy_cfg_nr_default.h"
#include "srsran/common/test_common.h"
#include "test_bench.h"
#include <boost/program_options.hpp>
#include <iostream>

// shorten boost program options namespace
namespace bpo = boost::program_options;

static double assert_sr_detection_min    = 1.000;
static double assert_cqi_detection_min   = 1.000;
static double assert_pusch_bler_max      = 0.000;
static double assert_pdsch_bler_max      = 0.000;
static double assert_prach_detection_min = 1.000;
static double assert_prach_ta_min        = 0.000;
static double assert_prach_ta_max        = 0.000;
static double assert_pucch_snr_min       = 0.000;

test_bench::args_t::args_t(int argc, char** argv)
{
  std::string              reference_cfg_str = "";
  bpo::options_description options("Test bench options");
  bpo::options_description options_gnb_stack("gNb stack and scheduling related options");
  bpo::options_description options_gnb_phy("gNb PHY related options");
  bpo::options_description options_ue_stack("UE stack options");
  bpo::options_description options_ue_phy("UE stack options");
  bpo::options_description options_assertion("Test assertions");

  uint16_t rnti = 17921;

  gnb_stack.pdsch.slots = "0,1,2,3,4,5";
  gnb_stack.pusch.slots = "6,7,8,9";

  // clang-format off
  options.add_options()
        ("rnti",                         bpo::value<uint16_t>(&rnti)->default_value(rnti),                                                        "UE RNTI")
        ("duration",                     bpo::value<uint64_t>(&durations_slots)->default_value(durations_slots),                                  "Test duration in slots")
        ("lib.log.level",                bpo::value<std::string>(&phy_lib_log_level)->default_value(phy_lib_log_level),                           "PHY librray log level")
        ("reference",                    bpo::value<std::string>(&reference_cfg_str)->default_value(reference_cfg_str),                           "Reference PHY configuration arguments")
        ("dl_channel.awgn_enable",       bpo::value<bool>(&dl_channel.awgn_enable)->default_value(dl_channel.awgn_enable),                        "DL Channel AWGN enable / disable")
        ("dl_channel.awgn_snr",          bpo::value<float>(&dl_channel.awgn_snr_dB)->default_value(dl_channel.awgn_snr_dB),                       "DL Channel AWGN SNR in dB")
        ("ul_channel.awgn_enable",       bpo::value<bool>(&ul_channel.awgn_enable)->default_value(ul_channel.awgn_enable),                        "UL Channel AWGN enable / disable")
        ("ul_channel.awgn_snr",          bpo::value<float>(&ul_channel.awgn_snr_dB)->default_value(ul_channel.awgn_snr_dB),                       "UL Channel AWGN SNR in dB")
        ("ul_channel.signal_power_dBfs", bpo::value<float>(&ul_channel.awgn_signal_power_dBfs)->default_value(ul_channel.awgn_signal_power_dBfs), "UL Channel expected signal power")
        ("channel.cfo",                  bpo::value<float>(&ul_channel.hst_fd_hz)->default_value(0),                                              "Channel HST Doppler frequency")
;

  options_gnb_stack.add_options()
        ("gnb.stack.pdcch.aggregation_level", bpo::value<uint32_t>(&gnb_stack.pdcch_aggregation_level)->default_value(gnb_stack.pdcch_aggregation_level), "PDCCH aggregation level")
        ("gnb.stack.pdsch.candidate",         bpo::value<uint32_t>(&gnb_stack.pdcch_dl_candidate)->default_value(gnb_stack.pdcch_dl_candidate),           "PDCCH candidate index for PDSCH")
        ("gnb.stack.pdsch.start",             bpo::value<uint32_t>(&gnb_stack.pdsch.rb_start)->default_value(0),                                          "PDSCH scheduling frequency allocation start")
        ("gnb.stack.pdsch.length",            bpo::value<uint32_t>(&gnb_stack.pdsch.rb_length)->default_value(gnb_stack.pdsch.rb_length),                 "PDSCH scheduling frequency allocation length")
        ("gnb.stack.pdsch.slots",             bpo::value<std::string>(&gnb_stack.pdsch.slots)->default_value(gnb_stack.pdsch.slots),                      "Slots enabled for PDSCH")
        ("gnb.stack.pdsch.mcs",               bpo::value<uint32_t>(&gnb_stack.pdsch.mcs)->default_value(gnb_stack.pdsch.mcs),                             "PDSCH scheduling modulation code scheme")
        ("gnb.stack.pusch.candidate",         bpo::value<uint32_t>(&gnb_stack.pdcch_ul_candidate)->default_value(gnb_stack.pdcch_ul_candidate),           "PDCCH candidate index for PUSCH")
        ("gnb.stack.pusch.start",             bpo::value<uint32_t>(&gnb_stack.pusch.rb_start)->default_value(0),                                          "PUSCH scheduling frequency allocation start")
        ("gnb.stack.pusch.length",            bpo::value<uint32_t>(&gnb_stack.pusch.rb_length)->default_value(gnb_stack.pusch.rb_length),                 "PUSCH scheduling frequency allocation length")
        ("gnb.stack.pusch.slots",             bpo::value<std::string>(&gnb_stack.pusch.slots)->default_value(gnb_stack.pusch.slots),                      "Slots enabled for PUSCH")
        ("gnb.stack.pusch.mcs",               bpo::value<uint32_t>(&gnb_stack.pusch.mcs)->default_value(gnb_stack.pusch.mcs),                             "PUSCH scheduling modulation code scheme")
        ("gnb.stack.log.level",               bpo::value<std::string>(&gnb_stack.log_level)->default_value(gnb_stack.log_level),                          "Stack log level")
        ("gnb.stack.use_dummy_mac",         bpo::value<std::string>(&gnb_stack.use_dummy_mac)->default_value("dummymac"),                                          "Use dummy or real NR scheduler (dummymac or realmac)")
        ;

  options_gnb_phy.add_options()
        ("gnb.phy.nof_threads",     bpo::value<uint32_t>(&gnb_phy.nof_phy_threads)->default_value(1),          "Number of threads")
        ("gnb.phy.log.level",       bpo::value<std::string>(&gnb_phy.log.phy_level)->default_value("warning"), "gNb PHY log level")
        ("gnb.phy.log.hex_limit",   bpo::value<int>(&gnb_phy.log.phy_hex_limit)->default_value(0),             "gNb PHY log hex limit")
        ("gnb.phy.log.id_preamble", bpo::value<std::string>(&gnb_phy.log.id_preamble)->default_value("GNB/"),  "gNb PHY log ID preamble")
        ("gnb.phy.pusch.max_iter",  bpo::value<uint32_t>(&gnb_phy.pusch_max_its)->default_value(10),      "PUSCH LDPC max number of iterations")
        ;

  options_ue_phy.add_options()
        ("ue.phy.nof_threads",     bpo::value<uint32_t>(&ue_phy.nof_phy_threads)->default_value(1),          "Number of threads")
        ("ue.phy.log.level",       bpo::value<std::string>(&ue_phy.log.phy_level)->default_value("warning"), "UE PHY log level")
        ("ue.phy.log.hex_limit",   bpo::value<int>(&ue_phy.log.phy_hex_limit)->default_value(0),             "UE PHY log hex limit")
        ("ue.phy.log.id_preamble", bpo::value<std::string>(&ue_phy.log.id_preamble)->default_value(" UE/"),  "UE PHY log ID preamble")
        ;

  options_ue_stack.add_options()
        ("ue.stack.sr.period",      bpo::value<uint32_t>(&ue_stack.sr_period)->default_value(ue_stack.sr_period),           "SR period in number of opportunities. Set 0 to disable and 1 for all.")
        ("ue.stack.prach.period",   bpo::value<uint32_t>(&ue_stack.prach_period)->default_value(ue_stack.prach_period),     "PRACH period in SFN. Set 0 to disable and 1 for all.")
        ;

  options_assertion.add_options()
      ("assert.sr.detection.min",  bpo::value<double>(&assert_sr_detection_min)->default_value(assert_sr_detection_min),   "Scheduling request minimum detection threshold")
      ("assert.cqi.detection.min", bpo::value<double>(&assert_cqi_detection_min)->default_value(assert_cqi_detection_min), "CQI report minimum detection threshold")
      ("assert.pusch.bler.max",    bpo::value<double>(&assert_pusch_bler_max)->default_value(assert_pusch_bler_max),       "PUSCH maximum BLER threshold")
      ("assert.pdsch.bler.max",    bpo::value<double>(&assert_pdsch_bler_max)->default_value(assert_pdsch_bler_max),       "PDSCH maximum BLER threshold")
      ("assert.prach.ta.min",      bpo::value<double>(&assert_prach_ta_min)->default_value(assert_prach_ta_min),           "PRACH estimated TA minimum value threshold")
      ("assert.prach.ta.max",      bpo::value<double>(&assert_prach_ta_max)->default_value(assert_prach_ta_max),           "PRACH estimated TA maximum value threshold")
      ("assert.pucch.snr.min",     bpo::value<double>(&assert_pucch_snr_min)->default_value(assert_pucch_snr_min),         "PUCCH DMRS minimum SNR allowed threshold")
      ;

  options.add(options_gnb_stack).add(options_gnb_phy).add(options_ue_stack).add(options_ue_phy).add_options()
        ("help",                      "Show this message")
        ;
  // clang-format on

  bpo::variables_map vm;
  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(options).run(), vm);
    bpo::notify(vm);

    // Apply the High Speed Train args to the DL channel as well
    ul_channel.hst_enable = std::isnormal(ul_channel.hst_fd_hz);
    dl_channel.hst_enable = ul_channel.hst_enable;
    dl_channel.hst_fd_hz  = ul_channel.hst_fd_hz;
  } catch (bpo::error& e) {
    std::cerr << e.what() << std::endl;
    return;
  }

  // help option was given or error - print usage and exit
  if (vm.count("help")) {
    std::cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << std::endl << std::endl;
    std::cout << options << std::endl << std::endl;
    return;
  }

  // Load default reference configuration
  phy_cfg = srsran::phy_cfg_nr_default_t(srsran::phy_cfg_nr_default_t::reference_cfg_t(reference_cfg_str));

  // Calulate the DL signal power from the number of PRBs
  dl_channel.awgn_signal_power_dBfs = srsran_gnb_dl_get_maximum_signal_power_dBfs(phy_cfg.carrier.nof_prb);

  // Reverses the Doppler shift for the UL
  ul_channel.hst_init_time_s = 0.5 * dl_channel.hst_period_s;

  // Calculate sampling rate in Hz
  srate_hz = (double)(srsran_min_symbol_sz_rb(phy_cfg.carrier.nof_prb) * SRSRAN_SUBC_SPACING_NR(phy_cfg.carrier.scs));

  cell_list.resize(1);
  cell_list[0].carrier = phy_cfg.carrier;
  cell_list[0].rf_port = 0;
  cell_list[0].cell_id = 0;
  cell_list[0].pdcch   = phy_cfg.pdcch;

  ue_stack.rnti = rnti;

  gnb_stack.rnti          = rnti;
  gnb_stack.phy_cfg       = phy_cfg;
  gnb_stack.wait_preamble = ue_stack.prach_period > 0;

  if (gnb_stack.pdsch.rb_length == 0) {
    gnb_stack.pdsch.rb_length = phy_cfg.carrier.nof_prb;
    gnb_stack.pdsch.rb_start  = 0;
  }

  if (gnb_stack.pusch.rb_length == 0) {
    gnb_stack.pusch.rb_length = phy_cfg.carrier.nof_prb;
    gnb_stack.pdsch.rb_start  = 0;
  }

  // Flag configuration as valid
  valid = true;
}

int main(int argc, char** argv)
{
  srslog::init();

  // Parse test bench arguments
  test_bench::args_t args(argc, argv);

  // Parse arguments
  TESTASSERT(args.valid);

  // Create test bench
  test_bench tb(args);

  // Assert bench is initialised correctly
  TESTASSERT(tb.is_initialised());

  // Run per TTI basis
  while (tb.run_tti()) {
    ; // Do nothing
  }

  // Stop test bench
  tb.stop();

  // Flush log
  srslog::flush();

  // Retrieve MAC metrics
  test_bench::metrics_t metrics = tb.get_metrics();

  // Print PRACH
  double   prach_detection = 0.0;
  double   prach_ta        = 0.0;
  uint32_t prach_tx_count  = 0;
  uint32_t prach_rx_count  = 0;
  if (metrics.ue_stack.prach.size() > 0) {
    srsran::console("PRACH:\n");
    srsran::console(
        "   +------------+------------+------------+------------+------------+------------+------------+\n");
    srsran::console("   | %10s | %10s | %10s | %10s | %10s | %10s | %10s |\n",
                    "Preamble",
                    "Transmit'd",
                    "Received",
                    "Detection",
                    "Avg TA",
                    "Min TA",
                    "Max TA");
    srsran::console(
        "   +------------+------------+------------+------------+------------+------------+------------+\n");

    for (const auto& p : metrics.ue_stack.prach) {
      // Ensure the detected count matches with transmission
      //      TESTASSERT(metrics.gnb_stack.prach.count(p.first));
      //      TESTASSERT(metrics.gnb_stack.prach[p.first].count == p.second.count);
      TESTASSERT(p.second.count != 0);
      prach_tx_count += p.second.count;

      gnb_dummy_stack::prach_metrics_t gnb_prach = {};
      if (metrics.gnb_stack.prach.count(p.first) > 0) {
        gnb_prach = metrics.gnb_stack.prach[p.first];
        prach_ta  = SRSRAN_VEC_SAFE_CMA(gnb_prach.avg_ta, prach_ta, prach_rx_count);
      } else {
        gnb_prach.avg_ta = NAN;
        gnb_prach.min_ta = NAN;
        gnb_prach.max_ta = NAN;
      }
      prach_rx_count += gnb_prach.count;

      double detection = (double)gnb_prach.count / (double)p.second.count;

      srsran::console("   | %10d | %10d | %10d | %10.3f | %10.1f | %10.1f | %10.1f |\n",
                      p.first,
                      p.second.count,
                      gnb_prach.count,
                      detection,
                      gnb_prach.avg_ta,
                      gnb_prach.min_ta,
                      gnb_prach.max_ta);
    }
    srsran::console(
        "   +------------+------------+------------+------------+------------+------------+------------+\n");
  }
  if (prach_tx_count > 0) {
    prach_detection = (double)prach_rx_count / (double)prach_tx_count;
  }

  // Print PUCCH
  if (metrics.gnb_stack.pucch.count > 0) {
    srsran::console("PUCCH DMRS Receiver metrics:\n");
    srsran::console("   +------------+------------+------------+------------+\n");
    srsran::console("   | %10s | %10s | %10s | %10s |\n", "Measure", "Average", "Min", "Max");
    srsran::console("   +------------+------------+------------+------------+\n");
    srsran::console("   | %10s | %+10.2f | %+10.2f | %+10.2f |\n",
                    "EPRE (dB)",
                    metrics.gnb_stack.pucch.epre_db_avg,
                    metrics.gnb_stack.pucch.epre_db_min,
                    metrics.gnb_stack.pucch.epre_db_min);
    srsran::console("   | %10s | %+10.2f | %+10.2f | %+10.2f |\n",
                    "RSRP (dB)",
                    metrics.gnb_stack.pucch.rsrp_db_avg,
                    metrics.gnb_stack.pucch.rsrp_db_min,
                    metrics.gnb_stack.pucch.rsrp_db_max);
    srsran::console("   | %10s | %+10.2f | %+10.2f | %+10.2f |\n",
                    "SINR (dB)",
                    metrics.gnb_stack.pucch.snr_db_avg,
                    metrics.gnb_stack.pucch.snr_db_min,
                    metrics.gnb_stack.pucch.snr_db_max);
    srsran::console("   | %10s | %+10.2f | %+10.2f | %+10.2f |\n",
                    "TA (us)",
                    metrics.gnb_stack.pucch.ta_us_avg,
                    metrics.gnb_stack.pucch.ta_us_min,
                    metrics.gnb_stack.pucch.ta_us_max);
    srsran::console("   +------------+------------+------------+------------+\n");
  }

  // Print PDSCH metrics if scheduled
  double pdsch_bler = 0.0;
  if (metrics.gnb_stack.mac.tx_pkts > 0) {
    pdsch_bler = (double)metrics.gnb_stack.mac.tx_errors / (double)metrics.gnb_stack.mac.tx_pkts;

    float pdsch_shed_rate = 0.0f;
    pdsch_shed_rate       = (float)metrics.gnb_stack.mac.tx_brate / (float)metrics.gnb_stack.mac.tx_pkts / 1000.0f;

    srsran::console("PDSCH:\n");
    srsran::console("       Count: %d\n", metrics.gnb_stack.mac.tx_pkts);
    srsran::console("        BLER: %f\n", pdsch_bler);
    srsran::console("  Sched Rate: %f Mbps\n", pdsch_shed_rate);
    srsran::console("    Net Rate: %f Mbps\n", (1.0f - pdsch_bler) * pdsch_shed_rate);
    srsran::console("   Retx Rate: %f Mbps\n", pdsch_bler * pdsch_shed_rate);
    srsran::console("\n");
  }

  // Print PUSCH metrics if scheduled
  double pusch_bler = 0.0;
  if (metrics.gnb_stack.mac.rx_pkts > 0) {
    if (metrics.gnb_stack.mac.rx_pkts != 0) {
      pusch_bler = (double)metrics.gnb_stack.mac.rx_errors / (double)metrics.gnb_stack.mac.rx_pkts;
    }

    float pusch_shed_rate = 0.0f;
    if (metrics.gnb_stack.mac.rx_pkts != 0) {
      pusch_shed_rate = (float)metrics.gnb_stack.mac.rx_brate / (float)metrics.gnb_stack.mac.rx_pkts / 1000.0f;
    }

    srsran::console("PUSCH:\n");
    srsran::console("       Count: %d\n", metrics.gnb_stack.mac.rx_pkts);
    srsran::console("        BLER: %f\n", pusch_bler);
    srsran::console("  Sched Rate: %f Mbps\n", pusch_shed_rate);
    srsran::console("    Net Rate: %f Mbps\n", (1.0f - pusch_bler) * pusch_shed_rate);
    srsran::console("   Retx Rate: %f Mbps\n", pusch_bler * pusch_shed_rate);
    srsran::console("\n");
  }

  // Print PUSCH
  if (metrics.gnb_stack.pusch.count > 0) {
    srsran::console("PUSCH DMRS Receiver metrics:\n");
    srsran::console("   +------------+------------+------------+------------+\n");
    srsran::console("   | %10s | %10s | %10s | %10s |\n", "Measure", "Average", "Min", "Max");
    srsran::console("   +------------+------------+------------+------------+\n");
    srsran::console("   | %10s | %+10.2f | %+10.2f | %+10.2f |\n",
                    "EPRE (dB)",
                    metrics.gnb_stack.pusch.epre_db_avg,
                    metrics.gnb_stack.pusch.epre_db_min,
                    metrics.gnb_stack.pusch.epre_db_min);
    srsran::console("   | %10s | %+10.2f | %+10.2f | %+10.2f |\n",
                    "RSRP (dB)",
                    metrics.gnb_stack.pusch.rsrp_db_avg,
                    metrics.gnb_stack.pusch.rsrp_db_min,
                    metrics.gnb_stack.pusch.rsrp_db_max);
    srsran::console("   | %10s | %+10.2f | %+10.2f | %+10.2f |\n",
                    "SINR (dB)",
                    metrics.gnb_stack.pusch.snr_db_avg,
                    metrics.gnb_stack.pusch.snr_db_min,
                    metrics.gnb_stack.pusch.snr_db_max);
    srsran::console("   | %10s | %+10.2f | %+10.2f | %+10.2f |\n",
                    "TA (us)",
                    metrics.gnb_stack.pusch.ta_us_avg,
                    metrics.gnb_stack.pusch.ta_us_min,
                    metrics.gnb_stack.pusch.ta_us_max);
    srsran::console("   +------------+------------+------------+------------+\n");
  }

  srsran::console("UCI stats:\n");
  srsran::console("   +------------+------------+------------+------------+------------+\n");
  srsran::console(
      "   | %10s | %10s | %10s | %10s | %10s |\n", "Field", "Transmit'd", "Received", "Detection", "Avg. Val.");
  srsran::console("   +------------+------------+------------+------------+------------+\n");

  // Print SR
  double sr_detection = 0.0;
  if (metrics.ue_stack.sr_count > 0) {
    sr_detection = (double)metrics.gnb_stack.sr_count / (double)metrics.ue_stack.sr_count;
    srsran::console("   | %10s | %10d | %10d | %10.5f | %10s |\n",
                    "SR",
                    metrics.ue_stack.sr_count,
                    metrics.gnb_stack.sr_count,
                    sr_detection,
                    "-");
  }

  // Print SR
  double cqi_detection = 0.0;
  if (metrics.gnb_stack.cqi_count > 0) {
    cqi_detection = (double)metrics.gnb_stack.cqi_valid_count / (double)metrics.gnb_stack.cqi_count;
    srsran::console("   | %10s | %10d | %10d | %10.5f | %10.5f |\n",
                    "CQI",
                    metrics.gnb_stack.cqi_count,
                    metrics.gnb_stack.cqi_valid_count,
                    cqi_detection,
                    metrics.gnb_stack.mac.dl_cqi);
  }
  srsran::console("   +------------+------------+------------+------------+------------+\n");

  // Assert metrics
  srsran_assert(metrics.gnb_stack.mac.tx_pkts == 0 or pdsch_bler <= assert_pdsch_bler_max,
                "PDSCH BLER (%f) exceeds the assertion maximum (%f)",
                pdsch_bler,
                assert_pusch_bler_max);
  srsran_assert(metrics.gnb_stack.mac.rx_pkts == 0 or pusch_bler <= assert_pusch_bler_max,
                "PUSCH BLER (%f) exceeds the assertion maximum (%f)",
                pusch_bler,
                assert_pusch_bler_max);
  srsran_assert(metrics.ue_stack.sr_count == 0 or sr_detection >= assert_sr_detection_min,
                "SR detection probability (%f) did not reach the assertion minimum (%f)",
                sr_detection,
                assert_sr_detection_min);
  srsran_assert(metrics.gnb_stack.cqi_count == 0 or cqi_detection >= assert_cqi_detection_min,
                "CQI report detection probability (%f) did not reach the assertion minimum (%f)",
                cqi_detection,
                assert_sr_detection_min);
  srsran_assert(prach_tx_count == 0 or prach_detection >= assert_prach_detection_min,
                "PRACH detection probability (%f) did not reach the assertion minimum (%f)",
                prach_detection,
                assert_prach_detection_min);
  srsran_assert(prach_tx_count == 0 or (prach_ta >= assert_prach_ta_min and prach_ta <= assert_prach_ta_max),
                "PRACH TA average measurement %f is higher than minimum (%d) or above maximum (%f)",
                prach_ta,
                assert_prach_ta_min,
                assert_prach_ta_max);
  srsran_assert(metrics.gnb_stack.pucch.count == 0 or (metrics.gnb_stack.pucch.snr_db_min >= assert_pucch_snr_min),
                "Minimum PUCCH DMRS SNR %f is below the minimum (%d)",
                metrics.gnb_stack.pucch.snr_db_min,
                assert_pucch_snr_min);

  // If reached here, the test is successful
  return SRSRAN_SUCCESS;
}
