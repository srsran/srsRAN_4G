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
#include "srslte/common/log_filter.h"
#include "srslte/upper/rlc_um.h"
#include <assert.h>

#define NBUFS 5

using namespace srslte;
using namespace srsue;

class mac_dummy_timers
    :public srslte::mac_interface_timers
{
public:
  srslte::timers::timer* timer_get(uint32_t timer_id)
  {
    return &t;
  }
  uint32_t timer_get_unique_id(){return 0;}
  void step()
  {
    t.step();
  }
  void timer_release_id(uint32_t timer_id) {}
private:
  srslte::timers::timer t;
};

class rlc_um_tester
    :public pdcp_interface_rlc
    ,public rrc_interface_rlc
{
public:
  rlc_um_tester(){
    bzero(sdus, sizeof(sdus));
    n_sdus = 0;
  }

  ~rlc_um_tester(){
    for (uint32_t i = 0; i < NBUFS; i++) {
      if (sdus[i] != NULL) {
        byte_buffer_pool::get_instance()->deallocate(sdus[i]);
      }
    }
  }

  // PDCP interface
  void write_pdu(uint32_t lcid, byte_buffer_t *sdu)
  {
    assert(lcid == 3);
    sdus[n_sdus++] = sdu;
  }
  void write_pdu_bcch_bch(byte_buffer_t *sdu) {}
  void write_pdu_bcch_dlsch(byte_buffer_t *sdu) {}
  void write_pdu_pcch(byte_buffer_t *sdu) {}
  
  // RRC interface
  void max_retx_attempted(){}
  std::string get_rb_name(uint32_t lcid) { return std::string(""); }

  byte_buffer_t *sdus[5];
  int n_sdus;
};

void basic_test()
{
  srslte::log_filter log1("RLC_UM_1");
  srslte::log_filter log2("RLC_UM_2");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log2.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(-1);
  log2.set_hex_limit(-1);
  rlc_um_tester    tester;
  mac_dummy_timers timers;

  rlc_um rlc1;
  rlc_um rlc2;

  int len;

  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log2.set_level(srslte::LOG_LEVEL_DEBUG);

  rlc1.init(&log1, 3, &tester, &tester, &timers);
  rlc2.init(&log2, 3, &tester, &tester, &timers);

  LIBLTE_RRC_RLC_CONFIG_STRUCT cnfg;
  cnfg.rlc_mode = LIBLTE_RRC_RLC_MODE_UM_BI;
  cnfg.dl_um_bi_rlc.t_reordering = LIBLTE_RRC_T_REORDERING_MS5;
  cnfg.dl_um_bi_rlc.sn_field_len = LIBLTE_RRC_SN_FIELD_LENGTH_SIZE10;
  cnfg.ul_um_bi_rlc.sn_field_len = LIBLTE_RRC_SN_FIELD_LENGTH_SIZE10;

  rlc1.configure(&cnfg);
  rlc2.configure(&cnfg);

  // Push 5 SDUs into RLC1
  byte_buffer_t sdu_bufs[NBUFS];
  for(int i=0;i<NBUFS;i++)
  {
    *sdu_bufs[i].msg    = i; // Write the index into the buffer
    sdu_bufs[i].N_bytes = 1; // Give each buffer a size of 1 byte
    rlc1.write_sdu(&sdu_bufs[i]);
  }

  assert(14 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (1 byte each)
  byte_buffer_t pdu_bufs[NBUFS];
  for(int i=0;i<NBUFS;i++)
  {
    len = rlc1.read_pdu(pdu_bufs[i].msg, 4); // 3 bytes for header + payload
    pdu_bufs[i].N_bytes = len;
  }

  assert(0 == rlc1.get_buffer_state());

  // Write 5 PDUs into RLC2
  for(int i=0;i<NBUFS;i++)
  {
    rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  assert(0 == rlc2.get_buffer_state());

  assert(NBUFS == tester.n_sdus);
  for(int i=0; i<tester.n_sdus; i++)
  {
    assert(tester.sdus[i]->N_bytes == 1);
    assert(*(tester.sdus[i]->msg)  == i);
  }
}

void loss_test()
{
  srslte::log_filter log1("RLC_UM_1");
  srslte::log_filter log2("RLC_UM_2");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log2.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(-1);
  log2.set_hex_limit(-1);
  rlc_um_tester    tester;
  mac_dummy_timers timers;

  rlc_um rlc1;
  rlc_um rlc2;

  int len;

  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log2.set_level(srslte::LOG_LEVEL_DEBUG);

  rlc1.init(&log1, 3, &tester, &tester, &timers);
  rlc2.init(&log2, 3, &tester, &tester, &timers);

  LIBLTE_RRC_RLC_CONFIG_STRUCT cnfg;
  cnfg.rlc_mode = LIBLTE_RRC_RLC_MODE_UM_BI;
  cnfg.dl_um_bi_rlc.t_reordering = LIBLTE_RRC_T_REORDERING_MS5;
  cnfg.dl_um_bi_rlc.sn_field_len = LIBLTE_RRC_SN_FIELD_LENGTH_SIZE10;
  cnfg.ul_um_bi_rlc.sn_field_len = LIBLTE_RRC_SN_FIELD_LENGTH_SIZE10;

  rlc1.configure(&cnfg);
  rlc2.configure(&cnfg);

  // Push 5 SDUs into RLC1
  byte_buffer_t sdu_bufs[NBUFS];
  for(int i=0;i<NBUFS;i++)
  {
    *sdu_bufs[i].msg    = i; // Write the index into the buffer
    sdu_bufs[i].N_bytes = 1; // Give each buffer a size of 1 byte
    rlc1.write_sdu(&sdu_bufs[i]);
  }

  assert(14 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (1 byte each)
  byte_buffer_t pdu_bufs[NBUFS];
  for(int i=0;i<NBUFS;i++)
  {
    len = rlc1.read_pdu(pdu_bufs[i].msg, 4); // 3 bytes for header + payload
    pdu_bufs[i].N_bytes = len;
  }

  assert(0 == rlc1.get_buffer_state());

  // Write 5 PDUs into RLC2 (skip SN 1)
  for(int i=0;i<NBUFS;i++)
  {
    if(i != 1)
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  // Step the reordering timer until expiry
  while(!timers.timer_get(1)->is_expired())
    timers.timer_get(1)->step();

  assert(NBUFS-1 == tester.n_sdus);
}

int main(int argc, char **argv) {
  basic_test();
  byte_buffer_pool::get_instance()->cleanup();
  loss_test();
  byte_buffer_pool::get_instance()->cleanup();
}
