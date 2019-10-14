/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/common/crash_handler.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/logger_stdout.h"
#include "srslte/common/rlc_pcap.h"
#include "srslte/common/threads.h"
#include "srslte/upper/rlc.h"
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <random>

#define LOG_HEX_LIMIT (-1)

using namespace std;
using namespace srsue;
using namespace srslte;
namespace bpo = boost::program_options;

typedef struct {
  std::string mode;
  uint32_t    sdu_size;
  uint32_t    test_duration_sec;
  float       pdu_drop_rate;
  float       pdu_cut_rate;
  uint32_t    sdu_gen_delay_usec;
  uint32_t    pdu_tx_delay_usec;
  bool        reestablish;
  uint32_t    log_level;
  bool        single_tx;
  bool        write_pcap;
  uint32_t    avg_opp_size;
  bool        random_opp;
  bool        zero_seed;
  bool        pedantic_sdu_check;
} stress_test_args_t;

void parse_args(stress_test_args_t *args, int argc, char *argv[]) {

  // Command line only options
  bpo::options_description general("General options");

  general.add_options()
  ("help,h", "Produce help message")
  ("version,v", "Print version information and exit");

  // clang-format off

  // Command line or config file options
  bpo::options_description common("Configuration options");
  common.add_options()
  ("mode",          bpo::value<std::string>(&args->mode)->default_value("AM"), "Whether to test RLC acknowledged or unacknowledged mode (AM/UM)")
  ("duration",      bpo::value<uint32_t>(&args->test_duration_sec)->default_value(5), "Duration (sec)")
  ("sdu_size",      bpo::value<uint32_t>(&args->sdu_size)->default_value(1500), "Size of SDUs")
  ("avg_opp_size",  bpo::value<uint32_t>(&args->avg_opp_size)->default_value(1505), "Size of the MAC opportunity (if not random)")
  ("random_opp",    bpo::value<bool>(&args->random_opp)->default_value(true), "Whether to generate random MAC opportunities")
  ("sdu_gen_delay", bpo::value<uint32_t>(&args->sdu_gen_delay_usec)->default_value(0), "SDU generation delay (usec)")
  ("pdu_tx_delay",  bpo::value<uint32_t>(&args->pdu_tx_delay_usec)->default_value(0), "Delay in MAC for transfering PDU from tx'ing RLC to rx'ing RLC (usec)")
  ("pdu_drop_rate", bpo::value<float>(&args->pdu_drop_rate)->default_value(0.1), "Rate at which RLC PDUs are dropped")
  ("pdu_cut_rate",  bpo::value<float>(&args->pdu_cut_rate)->default_value(0.0), "Rate at which RLC PDUs are chopped in length")
  ("reestablish",   bpo::value<bool>(&args->reestablish)->default_value(false), "Mimic RLC reestablish during execution")
  ("loglevel",      bpo::value<uint32_t>(&args->log_level)->default_value(srslte::LOG_LEVEL_DEBUG), "Log level (1=Error,2=Warning,3=Info,4=Debug)")
  ("singletx",      bpo::value<bool>(&args->single_tx)->default_value(false), "If set to true, only one node is generating data")
  ("pcap",          bpo::value<bool>(&args->write_pcap)->default_value(false), "Whether to write all RLC PDU to PCAP file")
  ("zeroseed",      bpo::value<bool>(&args->zero_seed)->default_value(false), "Whether to initialize random seed to zero")
  ("pedantic",      bpo::value<bool>(&args->pedantic_sdu_check)->default_value(false), "Whether to check SDU length and exit on error");
  // clang-format on

  // these options are allowed on the command line
  bpo::options_description cmdline_options;
  cmdline_options.add(common).add(general);

  // parse the command line and store result in vm
  bpo::variables_map vm;
  bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).run(), vm);
  bpo::notify(vm);

  // help option was given - print usage and exit
  if (vm.count("help") > 0) {
    cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << endl << endl;
    cout << common << endl << general << endl;
    exit(0);
  }

  if (args->log_level > 4) {
    args->log_level = 4;
    printf("Set log level to %d (%s)\n", args->log_level, srslte::log_level_text[args->log_level]);
  }
}

class mac_dummy : public thread
{
public:
  mac_dummy(rlc_interface_mac* rlc1_,
            rlc_interface_mac* rlc2_,
            stress_test_args_t args_,
            uint32_t           lcid_,
            timers*            timers_,
            rlc_pcap*          pcap_ = NULL) :
    run_enable(true),
    rlc1(rlc1_),
    rlc2(rlc2_),
    args(args_),
    pcap(pcap_),
    lcid(lcid_),
    timers(timers_),
    log("MAC  "),
    thread("MAC_DUMMY"),
    real_dist(0.0, 1.0)
  {
    log.set_level(static_cast<LOG_LEVEL_ENUM>(args.log_level));
    log.set_hex_limit(LOG_HEX_LIMIT);
  }

  void stop()
  {
    run_enable = false;
    wait_thread_finish();
  }

private:
  void run_tti(rlc_interface_mac *tx_rlc, rlc_interface_mac *rx_rlc, bool is_dl)
  {
    byte_buffer_pool*  pool = byte_buffer_pool::get_instance();
    unique_byte_buffer_t pdu  = srslte::allocate_unique_buffer(*pool, __PRETTY_FUNCTION__, true);
    if (!pdu) {
      printf("Fatal Error: Could not allocate PDU in mac_reader::run_thread\n");
      exit(-1);
    }

    float factor = 1.0;
    if (args.random_opp) {
      factor = 0.5 + real_dist(mt19937);
    }
    int opp_size = args.avg_opp_size * factor;
    uint32_t buf_state = tx_rlc->get_buffer_state(lcid);
    if (buf_state > 0) {
      int read = tx_rlc->read_pdu(lcid, pdu->msg, opp_size);
      pdu->N_bytes = read;
      if (args.pdu_tx_delay_usec > 0) {
        usleep(args.pdu_tx_delay_usec);
      }
      if ((real_dist(mt19937) > args.pdu_drop_rate) && read > 0) {
        uint32_t pdu_len = pdu->N_bytes;
        if ((real_dist(mt19937) > 0.1) && args.pdu_cut_rate) {
          int cut_pdu_len = pdu_len * real_dist(mt19937);
          log.info("Cutting MAC PDU len (%d B > %d B)\n", pdu_len, cut_pdu_len);
          pdu_len = cut_pdu_len;
        }
        rx_rlc->write_pdu(lcid, pdu->msg, pdu_len);
        if (is_dl) {
          pcap->write_dl_am_ccch(pdu->msg, pdu_len);
        } else {
          pcap->write_ul_am_ccch(pdu->msg, pdu_len);
        }
      } else {
        log.warning_hex(pdu->msg, pdu->N_bytes, "Dropping RLC PDU (%d B)\n", pdu->N_bytes);
      }
    }
  }

  void run_thread()
  {
    while (run_enable) {
      // Downlink direction first (RLC1->RLC2)
      run_tti(rlc1, rlc2, true);

      // UL direction (RLC2->RLC1)
      run_tti(rlc2, rlc1, false);

      // step timer
      timers->step_all();
    }
  }

  rlc_interface_mac *rlc1;
  rlc_interface_mac *rlc2;

  bool run_enable;
  stress_test_args_t args;
  rlc_pcap*          pcap;
  uint32_t           lcid;
  srslte::log_filter log;
  srslte::timers*    timers = nullptr;

  std::mt19937                          mt19937;
  std::uniform_real_distribution<float> real_dist;
};


class rlc_tester
    :public pdcp_interface_rlc
    ,public rrc_interface_rlc
    ,public thread
{
public:
  rlc_tester(rlc_interface_pdcp* rlc_, std::string name_, stress_test_args_t args_, uint32_t lcid_) :
    log("Testr"),
    rlc(rlc_),
    run_enable(true),
    rx_pdus(),
    name(name_),
    args(args_),
    lcid(lcid_),
    thread("RLC_TESTER")
  {
    log.set_level(srslte::LOG_LEVEL_ERROR);
    log.set_hex_limit(LOG_HEX_LIMIT);
  }

  void stop()
  {
    run_enable = false;
    wait_thread_finish();
  }

  // PDCP interface
  void write_pdu(uint32_t rx_lcid, unique_byte_buffer_t sdu)
  {
    assert(rx_lcid == lcid);
    if (sdu->N_bytes != args.sdu_size) {
      log.error_hex(sdu->msg, sdu->N_bytes, "Received SDU with size %d, expected %d.\n", sdu->N_bytes, args.sdu_size);
      if (args.pedantic_sdu_check) {
        exit(-1);
      }
    }

    rx_pdus++;
  }
  void write_pdu_bcch_bch(unique_byte_buffer_t sdu) {}
  void write_pdu_bcch_dlsch(unique_byte_buffer_t sdu) {}
  void write_pdu_pcch(unique_byte_buffer_t sdu) {}
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t sdu) {}

  // RRC interface
  void max_retx_attempted(){}
  std::string get_rb_name(uint32_t rx_lcid) { return std::string("DRB1"); }

  int get_nof_rx_pdus() { return rx_pdus; }

private:
  void run_thread() {
    uint8_t sn = 0;
    byte_buffer_pool* pool = byte_buffer_pool::get_instance();
    while(run_enable) {
      unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool, "rlc_tester::run_thread", true);
      if (pdu == NULL) {
        printf("Error: Could not allocate PDU in rlc_tester::run_thread\n\n\n");
        // backoff for a bit
        usleep(1000);
        continue;
      }
      for (uint32_t i = 0; i < args.sdu_size; i++) {
        pdu->msg[i] = sn;
      }
      sn++;
      pdu->N_bytes = args.sdu_size;
      rlc->write_sdu(lcid, std::move(pdu));
      if (args.sdu_gen_delay_usec > 0) {
        usleep(args.sdu_gen_delay_usec);
      }
    }
  }

  bool run_enable;
  uint64_t rx_pdus;
  uint32_t lcid;
  srslte::log_filter log;

  std::string name;

  stress_test_args_t args;

  rlc_interface_pdcp *rlc;
};

void stress_test(stress_test_args_t args)
{
  srslte::log_filter log1("RLC_1");
  srslte::log_filter log2("RLC_2");
  log1.set_level(static_cast<LOG_LEVEL_ENUM>(args.log_level));
  log2.set_level(static_cast<LOG_LEVEL_ENUM>(args.log_level));
  log1.set_hex_limit(LOG_HEX_LIMIT);
  log2.set_hex_limit(LOG_HEX_LIMIT);
  rlc_pcap pcap;
  uint32_t lcid = 1;

  if (args.write_pcap) {
    pcap.open("rlc_stress_test.pcap", 0);
  }

  rlc_config_t cnfg_;
  if (args.mode == "AM") {
    // config RLC AM bearer
    cnfg_.rlc_mode             = rlc_mode_t::am;
    cnfg_.am.max_retx_thresh = 4;
    cnfg_.am.poll_byte = 25*1000;
    cnfg_.am.poll_pdu = 4;
    cnfg_.am.t_poll_retx = 5;
    cnfg_.am.t_reordering = 5;
    cnfg_.am.t_status_prohibit = 5;
  } else if (args.mode == "UM") {
    // config UM bearer
    cnfg_.rlc_mode              = rlc_mode_t::um;
    cnfg_.um.t_reordering = 5;
    cnfg_.um.rx_mod = 32;
    cnfg_.um.rx_sn_field_length = rlc_umd_sn_size_t::size5bits;
    cnfg_.um.rx_window_size = 16;
    cnfg_.um.tx_sn_field_length = rlc_umd_sn_size_t::size5bits;
    cnfg_.um.tx_mod = 32;
  } else if (args.mode == "TM") {
    // use default LCID in TM
    lcid = 0;
  } else {
    cout << "Unsupported RLC mode " << args.mode << ", exiting." << endl;
    exit(-1);
  }

  srslte::timers timers(8);

  rlc rlc1(&log1);
  rlc rlc2(&log2);

  rlc_tester tester1(&rlc1, "tester1", args, lcid);
  rlc_tester tester2(&rlc2, "tester2", args, lcid);
  mac_dummy  mac(&rlc1, &rlc2, args, lcid, &timers, &pcap);

  rlc1.init(&tester1, &tester1, &timers, 0);
  rlc2.init(&tester2, &tester2, &timers, 0);

  // only add AM and UM bearers
  if (args.mode != "TM") {
    rlc1.add_bearer(lcid, cnfg_);
    rlc2.add_bearer(lcid, cnfg_);
  }

  tester1.start(7);
  if (!args.single_tx) {
    tester2.start(7);
  }
  mac.start();

  if (args.test_duration_sec < 1) {
    args.test_duration_sec = 1;
  }

  for (uint32_t i = 0; i < args.test_duration_sec; i++) {
    // if enabled, mimic reestablishment every second
    if (args.reestablish) {
      rlc1.reestablish();
      rlc2.reestablish();
    }
    usleep(1e6);
  }

  printf("Test finished, tearing down ..\n");

  // Stop RLC instances first to release blocking writers
  rlc1.stop();
  rlc2.stop();

  printf("RLC entities stopped.\n");

  // Stop upper layer writers
  tester1.stop();
  tester2.stop();

  printf("Writers stopped.\n");

  mac.stop();
  if (args.write_pcap) {
    pcap.close();
  }

  rlc_metrics_t metrics = {};
  rlc1.get_metrics(metrics);

  printf("RLC1 received %d SDUs in %ds (%.2f/s), Throughput: DL=%4.2f Mbps, UL=%4.2f Mbps\n",
         tester1.get_nof_rx_pdus(),
         args.test_duration_sec,
         static_cast<double>(tester1.get_nof_rx_pdus()/args.test_duration_sec),
         metrics.dl_tput_mbps[lcid],
         metrics.ul_tput_mbps[lcid]);

  rlc2.get_metrics(metrics);
  printf("RLC2 received %d SDUs in %ds (%.2f/s), Throughput: DL=%4.2f Mbps, UL=%4.2f Mbps\n",
         tester2.get_nof_rx_pdus(),
         args.test_duration_sec,
         static_cast<double>(tester2.get_nof_rx_pdus()/args.test_duration_sec),
         metrics.dl_tput_mbps[lcid],
         metrics.ul_tput_mbps[lcid]);
}


int main(int argc, char **argv)
{
  srslte_debug_handle_crash(argc, argv);

  stress_test_args_t args = {};
  parse_args(&args, argc, argv);

  if (args.zero_seed) {
    srand(0);
  } else {
    srand(time(NULL));
  }

  stress_test(args);
  byte_buffer_pool::get_instance()->cleanup();

  exit(0);
}
