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

#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include "srslte/common/log_filter.h"
#include "srslte/common/logger_stdout.h"
#include "srslte/common/threads.h"
#include "srslte/upper/rlc.h"
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <assert.h>

using namespace std;
using namespace srsue;
using namespace srslte;
namespace bpo = boost::program_options;

typedef struct {
  uint32_t test_duration_sec;
  float    error_rate;
  uint32_t sdu_gen_delay_usec;
  uint32_t pdu_tx_delay_usec;
  bool     reestablish;
  uint32_t log_level;
} stress_test_args_t;

void parse_args(stress_test_args_t *args, int argc, char *argv[]) {

  // Command line only options
  bpo::options_description general("General options");

  general.add_options()
  ("help,h", "Produce help message")
  ("version,v", "Print version information and exit");

  // Command line or config file options
  bpo::options_description common("Configuration options");
  common.add_options()
  ("duration",      bpo::value<uint32_t>(&args->test_duration_sec)->default_value(10), "Duration (sec)")
  ("sdu_gen_delay", bpo::value<uint32_t>(&args->sdu_gen_delay_usec)->default_value(10), "SDU generation delay (usec)")
  ("pdu_tx_delay",  bpo::value<uint32_t>(&args->pdu_tx_delay_usec)->default_value(10), "Delay in MAC for transfering PDU from tx'ing RLC to rx'ing RLC (usec)")
  ("error_rate",    bpo::value<float>(&args->error_rate)->default_value(0.1), "Rate at which RLC PDUs are dropped")
  ("reestablish",   bpo::value<bool>(&args->reestablish)->default_value(false), "Mimic RLC reestablish during execution")
  ("loglevel",      bpo::value<uint32_t>(&args->log_level)->default_value(srslte::LOG_LEVEL_DEBUG), "Log level (1=Error,2=Warning,3=Info,4=Debug");

  // these options are allowed on the command line
  bpo::options_description cmdline_options;
  cmdline_options.add(common).add(general);

  // parse the command line and store result in vm
  bpo::variables_map vm;
  bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).run(), vm);
  bpo::notify(vm);

  // help option was given - print usage and exit
  if (vm.count("help")) {
    cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << endl << endl;
    cout << common << endl << general << endl;
    exit(0);
  }

  if (args->log_level > 4) {
    args->log_level = 4;
    printf("Set log level to %d (%s)\n", args->log_level, srslte::log_level_text[args->log_level]);
  }
}

class mac_reader
    :public thread
{
public:
  mac_reader(rlc_interface_mac *rlc1_, rlc_interface_mac *rlc2_, float fail_rate_, uint32_t pdu_tx_delay_usec_)
  {
    rlc1 = rlc1_;
    rlc2 = rlc2_;
    fail_rate = fail_rate_;
    run_enable = true;
    running = false;
    pdu_tx_delay_usec = pdu_tx_delay_usec_;
  }

  void stop()
  {
    run_enable = false;
    int cnt=0;
    while(running && cnt<100) {
      usleep(10000);
      cnt++;
    }
    if(running) {
      thread_cancel();
    }
    wait_thread_finish();
  }

private:
  void run_thread()
  {
    running = true;
    byte_buffer_t *pdu = byte_buffer_pool::get_instance()->allocate("mac_reader::run_thread");
    if (!pdu) {
      printf("Fatal Error: Could not allocate PDU in mac_reader::run_thread\n");
      exit(-1);
    }

    while(run_enable) {
      float r = (float)rand()/RAND_MAX;
      int opp_size = r*1500;
      rlc1->get_buffer_state(1);
      int read = rlc1->read_pdu(1, pdu->msg, opp_size);
      if(((float)rand()/RAND_MAX > fail_rate) && read>0) {
        rlc2->write_pdu(1, pdu->msg, opp_size);
      }
      usleep(pdu_tx_delay_usec);
    }
    running = false;
    byte_buffer_pool::get_instance()->deallocate(pdu);
  }

  rlc_interface_mac *rlc1;
  rlc_interface_mac *rlc2;
  float fail_rate;
  uint32_t pdu_tx_delay_usec;

  bool run_enable;
  bool running;
};

class mac_dummy
    :public srslte::mac_interface_timers
{
public:
  mac_dummy(rlc_interface_mac *rlc1_, rlc_interface_mac *rlc2_, float fail_rate_, uint32_t pdu_tx_delay)
    :r1(rlc1_, rlc2_, fail_rate_, pdu_tx_delay)
    ,r2(rlc2_, rlc1_, fail_rate_, pdu_tx_delay)
  {
  }

  void start()
  {
    r1.start(7);
    r2.start(7);
  }

  void stop()
  {
    r1.stop();
    r2.stop();
  }

  srslte::timers::timer* timer_get(uint32_t timer_id)
  {
    return &t;
  }
  uint32_t timer_get_unique_id(){return 0;}
  void timer_release_id(uint32_t id){}

private:
  srslte::timers::timer t;

  mac_reader r1;
  mac_reader r2;
};



class rlc_am_tester
    :public pdcp_interface_rlc
    ,public rrc_interface_rlc
    ,public thread
{
public:
  rlc_am_tester(rlc_interface_pdcp *rlc_, std::string name_, uint32_t sdu_gen_delay_usec_){
    rlc = rlc_;
    run_enable = true;
    running = false;
    rx_pdus = 0;
    name = name_;
    sdu_gen_delay_usec = sdu_gen_delay_usec_;
  }

  void stop()
  {
    run_enable = false;
    int cnt=0;
    while(running && cnt<100) {
      usleep(10000);
      cnt++;
    }
    if(running) {
      thread_cancel();
    }
    wait_thread_finish();
  }

  // PDCP interface
  void write_pdu(uint32_t lcid, byte_buffer_t *sdu)
  {
    assert(lcid == 1);
    byte_buffer_pool::get_instance()->deallocate(sdu);
    std::cout << "rlc_am_tester " << name << " received " << rx_pdus++ << " PDUs" << std::endl;
  }
  void write_pdu_bcch_bch(byte_buffer_t *sdu) {}
  void write_pdu_bcch_dlsch(byte_buffer_t *sdu) {}
  void write_pdu_pcch(byte_buffer_t *sdu) {}
  
  // RRC interface
  void max_retx_attempted(){}
  std::string get_rb_name(uint32_t lcid) { return std::string(""); }

private:
  void run_thread()
  {
    uint8_t sn = 0;
    running = true;
    while(run_enable) {
      byte_buffer_t *pdu = byte_buffer_pool::get_instance()->allocate("rlc_am_tester::run_thread");
      if (!pdu) {
        printf("Fatal Error: Could not allocate PDU in rlc_am_tester::run_thread\n");
        exit(-1);
      }
      pdu->N_bytes = 1500;
      pdu->msg[0]   = sn++;
      rlc->write_sdu(1, pdu);
      usleep(sdu_gen_delay_usec);
    }
    running = false;
  }

  bool run_enable;
  bool running;
  long rx_pdus;

  std::string name;

  uint32_t sdu_gen_delay_usec;

  rlc_interface_pdcp *rlc;
};

void stress_test(stress_test_args_t args)
{
  srslte::log_filter log1("RLC_AM_1");
  srslte::log_filter log2("RLC_AM_2");
  log1.set_level((LOG_LEVEL_ENUM)args.log_level);
  log2.set_level((LOG_LEVEL_ENUM)args.log_level);
  log1.set_hex_limit(-1);
  log2.set_hex_limit(-1);

  rlc rlc1;
  rlc rlc2;

  rlc_am_tester tester1(&rlc1, "tester1", args.sdu_gen_delay_usec);
  rlc_am_tester tester2(&rlc2, "tester2", args.sdu_gen_delay_usec);
  mac_dummy     mac(&rlc1, &rlc2, args.error_rate, args.pdu_tx_delay_usec);
  ue_interface  ue;

  rlc1.init(&tester1, &tester1, &ue, &log1, &mac, 0);
  rlc2.init(&tester2, &tester2, &ue, &log2, &mac, 0);

  LIBLTE_RRC_RLC_CONFIG_STRUCT cnfg;
  cnfg.rlc_mode = LIBLTE_RRC_RLC_MODE_AM;
  cnfg.dl_am_rlc.t_reordering = LIBLTE_RRC_T_REORDERING_MS5;
  cnfg.dl_am_rlc.t_status_prohibit = LIBLTE_RRC_T_STATUS_PROHIBIT_MS5;
  cnfg.ul_am_rlc.max_retx_thresh = LIBLTE_RRC_MAX_RETX_THRESHOLD_T4;
  cnfg.ul_am_rlc.poll_byte = LIBLTE_RRC_POLL_BYTE_KB25;
  cnfg.ul_am_rlc.poll_pdu = LIBLTE_RRC_POLL_PDU_P4;
  cnfg.ul_am_rlc.t_poll_retx = LIBLTE_RRC_T_POLL_RETRANSMIT_MS5;

  srslte_rlc_config_t cnfg_(&cnfg);

  rlc1.add_bearer(1, cnfg_);
  rlc2.add_bearer(1, cnfg_);

  tester1.start(7);
  tester2.start(7);
  mac.start();

  for (uint32_t i = 0; i < args.test_duration_sec; i++) {
    // if enabled, mimic reestablishment every second
    if (args.reestablish) {
      rlc1.reestablish();
      rlc2.reestablish();
    }
    usleep(1e6);
  }

  tester1.stop();
  tester2.stop();
  mac.stop();
}


int main(int argc, char **argv) {
  stress_test_args_t args;
  parse_args(&args, argc, argv);

  stress_test(args);
  byte_buffer_pool::get_instance()->cleanup();
}
