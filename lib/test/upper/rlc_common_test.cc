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

#include "srslte/common/log_filter.h"
#include "srslte/upper/rlc.h"
#include <iostream>

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

#define MAX_NBUFS 100
#define NBUFS 5

using namespace srslte;

class rlc_tester : public srsue::pdcp_interface_rlc, public srsue::rrc_interface_rlc
{
public:
  rlc_tester()
  {
    bzero(sdus, sizeof(sdus));
    n_sdus           = 0;
    expected_sdu_len = 0;
  }

  // PDCP interface
  void write_pdu(uint32_t lcid, unique_byte_buffer_t sdu)
  {
    if (lcid != 3 && sdu->N_bytes != expected_sdu_len) {
      printf("Received PDU with size %d, expected %d. Exiting.\n", sdu->N_bytes, expected_sdu_len);
      exit(-1);
    }
    sdus[n_sdus++] = std::move(sdu);
  }
  void write_pdu_bcch_bch(unique_byte_buffer_t sdu) {}
  void write_pdu_bcch_dlsch(unique_byte_buffer_t sdu) {}
  void write_pdu_pcch(unique_byte_buffer_t sdu) {}
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t sdu) { sdus[n_sdus++] = std::move(sdu); }

  // RRC interface
  void        max_retx_attempted() {}
  std::string get_rb_name(uint32_t lcid) { return std::string("TestRB"); }
  void        set_expected_sdu_len(uint32_t len) { expected_sdu_len = len; }

  unique_byte_buffer_t sdus[MAX_NBUFS];
  int                  n_sdus;
  uint32_t             expected_sdu_len;
};

int meas_obj_test()
{
  srslte::log_ref log1("RLC_1");
  srslte::log_ref log2("RLC_2");
  log1->set_level(srslte::LOG_LEVEL_DEBUG);
  log2->set_level(srslte::LOG_LEVEL_DEBUG);
  log1->set_hex_limit(-1);
  log2->set_hex_limit(-1);
  rlc_tester            tester;
  srslte::timer_handler timers(1);

  int len = 0;

  rlc rlc1(log1->get_service_name().c_str());
  rlc rlc2(log2->get_service_name().c_str());

  rlc1.init(&tester, &tester, &timers, 0);
  rlc2.init(&tester, &tester, &timers, 0);

  rlc_config_t cnfg          = rlc_config_t::default_rlc_um_config(10);
  cnfg.rlc_mode              = rlc_mode_t::um;
  cnfg.um.t_reordering       = 5;
  cnfg.um.rx_sn_field_length = rlc_umd_sn_size_t::size10bits;
  cnfg.um.rx_window_size     = 512;
  cnfg.um.rx_mod             = 1024;
  cnfg.um.tx_sn_field_length = rlc_umd_sn_size_t::size10bits;
  cnfg.um.tx_mod             = 1024;

  uint32_t lcid = 1;
  rlc1.add_bearer(lcid, cnfg);
  rlc2.add_bearer(lcid, cnfg);

  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[NBUFS];

  tester.set_expected_sdu_len(1);

  // Push 5 SDUs into RLC1
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i]          = srslte::allocate_unique_buffer(*pool, true);
    *sdu_bufs[i]->msg    = i; // Write the index into the buffer
    sdu_bufs[i]->N_bytes = 1; // Give each buffer a size of 1 byte
    rlc1.write_sdu(lcid, std::move(sdu_bufs[i]));
  }

  TESTASSERT(14 == rlc1.get_buffer_state(lcid));

  // Reestablish
  rlc1.reestablish(1);

  TESTASSERT(0 == rlc1.get_buffer_state(lcid));

  // Push again 5 SDUs, SN should start from 0
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i]          = srslte::allocate_unique_buffer(*pool, true);
    *sdu_bufs[i]->msg    = i; // Write the index into the buffer
    sdu_bufs[i]->N_bytes = 1; // Give each buffer a size of 1 byte
    rlc1.write_sdu(lcid, std::move(sdu_bufs[i]));
  }

  TESTASSERT(14 == rlc1.get_buffer_state(lcid));

  // Read 5 PDUs from RLC1 (1 byte each)
  byte_buffer_t pdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    len                 = rlc1.read_pdu(lcid, pdu_bufs[i].msg, 4); // 3 bytes for header + payload
    pdu_bufs[i].N_bytes = len;
  }

  TESTASSERT(0 == rlc1.get_buffer_state(lcid));

  // Write 5 PDUs into RLC2
  for (int i = 0; i < NBUFS; i++) {
    rlc2.write_pdu(lcid, pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  // Check they have been passed to PDCP
  TESTASSERT(NBUFS == tester.n_sdus);

  rlc2.reestablish(lcid);

  tester.n_sdus = 0;

  // Push again
  for (int i = 0; i < NBUFS; i++) {
    rlc2.write_pdu(lcid, pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  // Check the are again in the buffer
  TESTASSERT(NBUFS == tester.n_sdus);

  for (int i = 0; i < NBUFS; i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == 1);
    TESTASSERT(*(tester.sdus[i]->msg) == i);
  }

  // Resume unexisting and unsuspended bearer
  rlc2.resume_bearer(lcid + 1);
  rlc2.resume_bearer(lcid);

  // Suspend unexisting bearer and twice
  rlc2.suspend_bearer(lcid + 1);
  rlc2.suspend_bearer(lcid);
  rlc2.suspend_bearer(lcid);

  // Reestablish and push again while in suspended
  rlc2.reestablish(lcid);

  tester.n_sdus = 0;

  // Push again
  for (int i = 0; i < NBUFS; i++) {
    rlc2.write_pdu(lcid, pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  // Check they are not being passed to PDCP
  TESTASSERT(0 == tester.n_sdus);

  rlc2.resume_bearer(lcid);

  // Check now they are being passed
  TESTASSERT(NBUFS == tester.n_sdus);

  for (int i = 0; i < NBUFS; i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == 1);
    TESTASSERT(*(tester.sdus[i]->msg) == i);
  }

  return 0;
}

int main(int argc, char** argv)
{
  if (meas_obj_test()) {
    return -1;
  }
  byte_buffer_pool::get_instance()->cleanup();
}
