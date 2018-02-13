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
#include <assert.h>
#define NBUFS 5

using namespace srsue;
using namespace srslte;

class mac_reader
    :public thread
{
public:
  mac_reader(rlc_interface_mac *rlc1_, rlc_interface_mac *rlc2_, float fail_rate_)
  {
    rlc1 = rlc1_;
    rlc2 = rlc2_;
    fail_rate = fail_rate_;
    run_enable = true;
    running = false;
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
      usleep(1000);
    }
    running = false;
  }

  rlc_interface_mac *rlc1;
  rlc_interface_mac *rlc2;
  float fail_rate;

  bool run_enable;
  bool running;
};

class mac_dummy
    :public srslte::mac_interface_timers
{
public:
  mac_dummy(rlc_interface_mac *rlc1_, rlc_interface_mac *rlc2_, float fail_rate_)
    :r1(rlc1_, rlc2_, fail_rate_)
    ,r2(rlc2_, rlc1_, fail_rate_)
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
  rlc_am_tester(rlc_interface_pdcp *rlc_){
    rlc = rlc_;
    run_enable = true;
    running = false;
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
      usleep(1000);
    }
    running = false;
  }

  bool run_enable;
  bool running;

  rlc_interface_pdcp *rlc;
};

void stress_test()
{
  srslte::log_filter log1("RLC_AM_1");
  srslte::log_filter log2("RLC_AM_2");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log2.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(-1);
  log2.set_hex_limit(-1);

  float fail_rate = 0.1;

  rlc rlc1;
  rlc rlc2;

  rlc_am_tester tester1(&rlc1);
  rlc_am_tester tester2(&rlc2);
  mac_dummy     mac(&rlc1, &rlc2, fail_rate);
  ue_interface  ue;

  rlc1.init(&tester1, &tester1, &ue, &log1, &mac, 0);
  rlc2.init(&tester1, &tester1, &ue, &log2, &mac, 0);

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
  //tester2.start(7);
  mac.start();

  usleep(100e6);

  tester1.stop();
  tester2.stop();
  mac.stop();
}


int main(int argc, char **argv) {
  stress_test();
  byte_buffer_pool::get_instance()->cleanup();
}
