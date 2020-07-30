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
#include "srslte/common/rlc_pcap.h"
#include "srslte/common/test_common.h"
#include "srslte/common/threads.h"
#include "srslte/upper/rlc_am_lte.h"
#include <assert.h>
#include <iostream>
#define NBUFS 5
#define HAVE_PCAP 0
#define SDU_SIZE 500

using namespace srsue;
using namespace srslte;

srslte::log_ref rrc_log1("RLC_AM_1");
srslte::log_ref rrc_log2("RLC_AM_2");

bool rx_is_tx(const rlc_bearer_metrics_t& rlc1_metrics, const rlc_bearer_metrics_t& rlc2_metrics)
{
  if (rlc1_metrics.num_tx_pdu_bytes != rlc2_metrics.num_rx_pdu_bytes) {
    return false;
  }

  if (rlc2_metrics.num_tx_pdu_bytes != rlc1_metrics.num_rx_pdu_bytes) {
    return false;
  }
  return true;
}

class rlc_am_tester : public pdcp_interface_rlc, public rrc_interface_rlc
{
public:
  rlc_am_tester(rlc_pcap* pcap_ = NULL)
  {
    n_sdus = 0;
    pcap   = pcap_;
  }

  // PDCP interface
  void write_pdu(uint32_t lcid, unique_byte_buffer_t sdu)
  {
    assert(lcid == 1);
    sdus[n_sdus++] = std::move(sdu);
  }
  void write_pdu_bcch_bch(unique_byte_buffer_t sdu) {}
  void write_pdu_bcch_dlsch(unique_byte_buffer_t sdu) {}
  void write_pdu_pcch(unique_byte_buffer_t sdu) {}
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}

  // RRC interface
  void        max_retx_attempted() {}
  std::string get_rb_name(uint32_t lcid) { return std::string(""); }

  unique_byte_buffer_t sdus[10];
  int                  n_sdus;
  rlc_pcap*            pcap;
};

class ul_writer : public thread
{
public:
  ul_writer(rlc_am_lte* rlc_) : rlc(rlc_), running(false), thread("UL_WRITER") {}
  ~ul_writer() { stop(); }
  void stop()
  {
    running = false;
    int cnt = 0;
    while (running && cnt < 100) {
      usleep(10000);
      cnt++;
    }
    wait_thread_finish();
  }

private:
  void run_thread()
  {
    int sn  = 0;
    running = true;
    while (running) {
      byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
      unique_byte_buffer_t pdu  = srslte::allocate_unique_buffer(*pool, "rlc_tester::run_thread", true);
      if (!pdu) {
        printf("Error: Could not allocate PDU in rlc_tester::run_thread\n\n\n");
        // backoff for a bit
        usleep(1000);
        continue;
      }
      for (uint32_t i = 0; i < SDU_SIZE; i++) {
        pdu->msg[i] = sn;
      }
      sn++;
      pdu->N_bytes = SDU_SIZE;
      rlc->write_sdu(std::move(pdu));
    }
    running = false;
  }

  rlc_am_lte* rlc;
  bool        running;
};

void basic_test_tx(rlc_am_lte* rlc, byte_buffer_t pdu_bufs[NBUFS])
{

  // Push 5 SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i]          = srslte::allocate_unique_buffer(*pool, true);
    sdu_bufs[i]->msg[0]  = i; // Write the index into the buffer
    sdu_bufs[i]->N_bytes = 1; // Give each buffer a size of 1 byte
    rlc->write_sdu(std::move(sdu_bufs[i]));
  }

  assert(14 == rlc->get_buffer_state());

  // Read 5 PDUs from RLC1 (1 byte each)
  for (int i = 0; i < NBUFS; i++) {
    uint32_t len        = rlc->read_pdu(pdu_bufs[i].msg, 4); // 3 bytes for header + payload
    pdu_bufs[i].N_bytes = len;
  }

  assert(0 == rlc->get_buffer_state());
}

bool meas_obj_test()
{
  rlc_am_tester tester;
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);
  rlc_am_lte rlc2(rrc_log2, 1, &tester, &tester, &timers);

  // before configuring entity
  assert(0 == rlc1.get_buffer_state());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  basic_test_tx(&rlc1, pdu_bufs);

  // Write 5 PDUs into RLC2
  for (int i = 0; i < NBUFS; i++) {
    rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  assert(2 == rlc2.get_buffer_state());

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  int           len  = rlc2.read_pdu(status_buf.msg, 2);
  status_buf.N_bytes = len;

  assert(0 == rlc2.get_buffer_state());

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  for (int i = 0; i < tester.n_sdus; i++) {
    assert(tester.sdus[i]->N_bytes == 1);
    assert(*(tester.sdus[i]->msg) == i);
  }

  // Check statistics
  TESTASSERT(rx_is_tx(rlc1.get_metrics(), rlc2.get_metrics()));

  return SRSLTE_SUCCESS;
}

bool concat_test()
{
  rlc_am_tester         tester;
  srslte::timer_handler timers(8);

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);
  rlc_am_lte rlc2(rrc_log2, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i]          = srslte::allocate_unique_buffer(*pool, true);
    sdu_bufs[i]->msg[0]  = i; // Write the index into the buffer
    sdu_bufs[i]->N_bytes = 1; // Give each buffer a size of 1 byte
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  assert(14 == rlc1.get_buffer_state());

  // Read 1 PDUs from RLC1 containing all 5 SDUs
  byte_buffer_t pdu_buf;
  int           len = rlc1.read_pdu(pdu_buf.msg, 13); // 8 bytes for header + payload
  pdu_buf.N_bytes   = len;

  assert(0 == rlc1.get_buffer_state());

  // Write PDU into RLC2
  rlc2.write_pdu(pdu_buf.msg, pdu_buf.N_bytes);

  // No status report as we haven't crossed polling thresholds

  assert(tester.n_sdus == 5);
  for (int i = 0; i < tester.n_sdus; i++) {
    assert(tester.sdus[i]->N_bytes == 1);
    assert(*(tester.sdus[i]->msg) == i);
  }

  // Check statistics
  TESTASSERT(rx_is_tx(rlc1.get_metrics(), rlc2.get_metrics()));

  return SRSLTE_SUCCESS;
}

bool segment_test(bool in_seq_rx)
{
  rlc_am_tester         tester;
  srslte::timer_handler timers(8);
  int                   len = 0;

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);
  rlc_am_lte rlc2(rrc_log2, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i] = srslte::allocate_unique_buffer(*pool, true);
    for (int j = 0; j < 10; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes = 10; // Give each buffer a size of 10 bytes
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  assert(59 == rlc1.get_buffer_state());

  // Read PDUs from RLC1 (force segmentation)
  byte_buffer_t pdu_bufs[20];
  int           n_pdus = 0;
  while (rlc1.get_buffer_state() > 0) {
    len                        = rlc1.read_pdu(pdu_bufs[n_pdus].msg, 10); // 2 header + payload
    pdu_bufs[n_pdus++].N_bytes = len;
  }

  assert(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2
  if (in_seq_rx) {
    // deliver PDUs in order
    for (int i = 0; i < n_pdus; ++i) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
    }
  } else {
    // deliver PDUs in reverse order
    for (int i = n_pdus - 1; i >= 0; --i) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
    }
  }

  // Receiver will only generate status PDU if they arrive in order
  // If SN=7 arrives first, but the Rx expects SN=0, status reporting will be delayed, see TS 36.322 v10 Section 5.2.3
  if (in_seq_rx) {
    assert(2 == rlc2.get_buffer_state());

    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    len                = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status
    status_buf.N_bytes = len;

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
  }

  assert(0 == rlc2.get_buffer_state());

  assert(tester.n_sdus == 5);
  for (int i = 0; i < tester.n_sdus; i++) {
    assert(tester.sdus[i]->N_bytes == 10);
    for (int j = 0; j < 10; j++)
      assert(tester.sdus[i]->msg[j] == j);
  }

  // Check statistics
  TESTASSERT(rx_is_tx(rlc1.get_metrics(), rlc2.get_metrics()));

  return SRSLTE_SUCCESS;
}

bool retx_test()
{
  rlc_am_tester tester;
  timer_handler timers(8);
  int           len = 0;

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);
  rlc_am_lte rlc2(rrc_log2, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i]          = srslte::allocate_unique_buffer(*pool, true);
    sdu_bufs[i]->msg[0]  = i; // Write the index into the buffer
    sdu_bufs[i]->N_bytes = 1; // Give each buffer a size of 1 byte
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  assert(14 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (1 byte each)
  byte_buffer_t pdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    len                 = rlc1.read_pdu(pdu_bufs[i].msg, 4); // 2 byte header + 1 byte payload
    pdu_bufs[i].N_bytes = len;
  }

  assert(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 1)
  for (int i = 0; i < NBUFS; i++) {
    if (i != 1)
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  uint32_t buffer_state = rlc2.get_buffer_state();
  assert(4 == buffer_state);

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  len                = rlc2.read_pdu(status_buf.msg, buffer_state); // provide exactly the reported buffer state
  status_buf.N_bytes = len;

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  assert(3 == rlc1.get_buffer_state()); // 2 byte header + 1 byte payload

  // Read the retx PDU from RLC1
  byte_buffer_t retx;
  len          = rlc1.read_pdu(retx.msg, 3); // 2 byte header + 1 byte payload
  retx.N_bytes = len;

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx.msg, retx.N_bytes);

  assert(tester.n_sdus == 5);
  for (int i = 0; i < tester.n_sdus; i++) {
    if (tester.sdus[i]->N_bytes != 1)
      return -1;
    if (*(tester.sdus[i]->msg) != i)
      return -1;
  }

  return 0;
}

bool resegment_test_1()
{
  // SDUs:                |  10  |  10  |  10  |  10  |  10  |
  // PDUs:                |  10  |  10  |  10  |  10  |  10  |
  // Retx PDU segments:                 | 5 | 5|

  rlc_am_tester tester;
  timer_handler timers(8);
  int           len = 0;

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);
  rlc_am_lte rlc2(rrc_log2, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i] = srslte::allocate_unique_buffer(*pool, true);
    for (int j = 0; j < 10; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes = 10; // Give each buffer a size of 10 bytes
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  assert(59 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (10 bytes each)
  byte_buffer_t pdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    len                 = rlc1.read_pdu(pdu_bufs[i].msg, 12); // 12 bytes for header + payload
    pdu_bufs[i].N_bytes = len;
  }

  assert(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 1)
  for (int i = 0; i < NBUFS; i++) {
    if (i != 1)
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  assert(4 == rlc2.get_buffer_state());

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  len                = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status
  status_buf.N_bytes = len;

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  assert(12 == rlc1.get_buffer_state()); // 2 byte header + 10 data

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  len           = rlc1.read_pdu(retx1.msg, 9); // 4 byte header + 5 data
  retx1.N_bytes = len;

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);

  assert(9 == rlc1.get_buffer_state());

  // Read the remaining segment
  byte_buffer_t retx2;
  len           = rlc1.read_pdu(retx2.msg, 9); // 4 byte header + 5 data
  retx2.N_bytes = len;

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);

  assert(tester.n_sdus == 5);
  for (int i = 0; i < tester.n_sdus; i++) {
    if (tester.sdus[i]->N_bytes != 10)
      return -1;
    for (int j = 0; j < 10; j++)
      if (tester.sdus[i]->msg[j] != j)
        return -1;
  }

  return 0;
}

bool resegment_test_2()
{

  // SDUs:              |  10  |  10  |  10  |  10  |  10  |
  // PDUs:              | 5 |  10  |     20     |  10  | 5 |
  // Retx PDU segments:            |  10  |  10 |

  rlc_am_tester tester;
  timer_handler timers(8);
  int           len = 0;

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);
  rlc_am_lte rlc2(rrc_log2, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i] = srslte::allocate_unique_buffer(*pool, true);
    for (int j = 0; j < 10; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes = 10; // Give each buffer a size of 10 bytes
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  assert(59 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (5 bytes, 10 bytes, 20 bytes, 10 bytes, 5 bytes)
  byte_buffer_t pdu_bufs[NBUFS];
  pdu_bufs[0].N_bytes = rlc1.read_pdu(pdu_bufs[0].msg, 7);  // 2 byte header +  5 byte payload
  pdu_bufs[1].N_bytes = rlc1.read_pdu(pdu_bufs[1].msg, 14); // 4 byte header + 10 byte payload
  pdu_bufs[2].N_bytes = rlc1.read_pdu(pdu_bufs[2].msg, 25); // 5 byte header + 20 byte payload
  pdu_bufs[3].N_bytes = rlc1.read_pdu(pdu_bufs[3].msg, 14); // 4 byte header + 10 byte payload
  pdu_bufs[4].N_bytes = rlc1.read_pdu(pdu_bufs[4].msg, 7);  // 2 byte header +  5 byte payload

  assert(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 2)
  for (int i = 0; i < NBUFS; i++) {
    if (i != 2)
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  assert(4 == rlc2.get_buffer_state());

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  assert(25 == rlc1.get_buffer_state()); // 4 byte header + 20 data

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  retx1.N_bytes = rlc1.read_pdu(retx1.msg, 16); // 6 byte header + 10 data

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);

  assert(18 == rlc1.get_buffer_state());

  // Read the remaining segment
  byte_buffer_t retx2;
  retx2.N_bytes = rlc1.read_pdu(retx2.msg, 18); // 6 byte header + 12 data

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);

  assert(tester.n_sdus == 5);
  for (int i = 0; i < tester.n_sdus; i++) {
    if (tester.sdus[i]->N_bytes != 10)
      return -1;
    for (int j = 0; j < 10; j++)
      if (tester.sdus[i]->msg[j] != j)
        return -1;
  }

  return 0;
}

bool resegment_test_3()
{

  // SDUs:              |  10  |  10  |  10  |  10  |  10  |
  // PDUs:              | 5 | 5|      20     |  10  |  10  |
  // Retx PDU segments:        |  10  |  10  |

  rlc_am_tester         tester;
  srslte::timer_handler timers(8);

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);
  rlc_am_lte rlc2(rrc_log2, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i] = srslte::allocate_unique_buffer(*pool, true);
    for (int j = 0; j < 10; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes = 10; // Give each buffer a size of 10 bytes
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  assert(59 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (5 bytes, 5 bytes, 20 bytes, 10 bytes, 10 bytes)
  byte_buffer_t pdu_bufs[NBUFS];
  pdu_bufs[0].N_bytes = rlc1.read_pdu(pdu_bufs[0].msg, 7);  // 2 byte header +  5 byte payload
  pdu_bufs[1].N_bytes = rlc1.read_pdu(pdu_bufs[1].msg, 7);  // 2 byte header +  5 byte payload
  pdu_bufs[2].N_bytes = rlc1.read_pdu(pdu_bufs[2].msg, 24); // 4 byte header + 20 byte payload
  pdu_bufs[3].N_bytes = rlc1.read_pdu(pdu_bufs[3].msg, 12); // 2 byte header + 10 byte payload
  pdu_bufs[4].N_bytes = rlc1.read_pdu(pdu_bufs[4].msg, 12); // 2 byte header + 10 byte payload

  assert(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 2)
  for (int i = 0; i < NBUFS; i++) {
    if (i != 2)
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  assert(4 == rlc2.get_buffer_state());

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  retx1.N_bytes = rlc1.read_pdu(retx1.msg, 16); // 6 byte header + 10 data

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);

  // Read the remaining segment
  byte_buffer_t retx2;
  retx2.N_bytes = rlc1.read_pdu(retx2.msg, 16); // 6 byte header + 10 data

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);

  assert(tester.n_sdus == 5);
  for (int i = 0; i < tester.n_sdus; i++) {
    if (tester.sdus[i]->N_bytes != 10)
      return -1;
    for (int j = 0; j < 10; j++)
      if (tester.sdus[i]->msg[j] != j)
        return -1;
  }

  return 0;
}

bool resegment_test_4()
{
  // SDUs:              |  10  |  10  |  10  |  10  |  10  |
  // PDUs:              | 5 | 5|         30         | 5 | 5|
  // Retx PDU segments:        |    15    |    15   |

  rlc_am_tester         tester;
  srslte::timer_handler timers(8);

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);
  rlc_am_lte rlc2(rrc_log2, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i] = srslte::allocate_unique_buffer(*pool, true);
    for (int j = 0; j < 10; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes = 10; // Give each buffer a size of 10 bytes
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  assert(59 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (5 bytes, 5 bytes, 30 bytes, 5 bytes, 5 bytes)
  byte_buffer_t pdu_bufs[NBUFS];
  pdu_bufs[0].N_bytes = rlc1.read_pdu(pdu_bufs[0].msg, 7);  // 2 byte header +  5 byte payload
  pdu_bufs[1].N_bytes = rlc1.read_pdu(pdu_bufs[1].msg, 7);  // 2 byte header +  5 byte payload
  pdu_bufs[2].N_bytes = rlc1.read_pdu(pdu_bufs[2].msg, 35); // 5 byte header + 30 byte payload
  pdu_bufs[3].N_bytes = rlc1.read_pdu(pdu_bufs[3].msg, 7);  // 2 byte header +  5 byte payload
  pdu_bufs[4].N_bytes = rlc1.read_pdu(pdu_bufs[4].msg, 7);  // 2 byte header +  5 byte payload

  assert(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 2)
  for (int i = 0; i < NBUFS; i++) {
    if (i != 2)
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  assert(4 == rlc2.get_buffer_state());

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  retx1.N_bytes = rlc1.read_pdu(retx1.msg, 21); // 6 byte header + 15 data

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);

  assert(23 == rlc1.get_buffer_state());

  // Read the remaining segment
  byte_buffer_t retx2;
  retx2.N_bytes = rlc1.read_pdu(retx2.msg, 23); // 6 byte header + 18 data

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);

  assert(tester.n_sdus == 5);
  for (int i = 0; i < tester.n_sdus; i++) {
    if (tester.sdus[i]->N_bytes != 10)
      return -1;
    for (int j = 0; j < 10; j++)
      if (tester.sdus[i]->msg[j] != j)
        return -1;
  }

  return 0;
}

bool resegment_test_5()
{
  // SDUs:              |  10  |  10  |  10  |  10  |  10  |
  // PDUs:              |2|3|            40            |3|2|
  // Retx PDU segments:     |     20      |     20     |

  rlc_am_tester         tester;
  srslte::timer_handler timers(8);

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);
  rlc_am_lte rlc2(rrc_log2, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i] = srslte::allocate_unique_buffer(*pool, true);
    for (int j = 0; j < 10; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes = 10; // Give each buffer a size of 10 bytes
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  assert(59 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (2 bytes, 3 bytes, 40 bytes, 3 bytes, 2 bytes)
  byte_buffer_t pdu_bufs[NBUFS];
  pdu_bufs[0].N_bytes = rlc1.read_pdu(pdu_bufs[0].msg, 4);  // 2 byte header +  2 byte payload
  pdu_bufs[1].N_bytes = rlc1.read_pdu(pdu_bufs[1].msg, 5);  // 2 byte header +  3 byte payload
  pdu_bufs[2].N_bytes = rlc1.read_pdu(pdu_bufs[2].msg, 48); // 8 byte header + 40 byte payload
  pdu_bufs[3].N_bytes = rlc1.read_pdu(pdu_bufs[3].msg, 5);  // 2 byte header +  3 byte payload
  pdu_bufs[4].N_bytes = rlc1.read_pdu(pdu_bufs[4].msg, 4);  // 2 byte header +  2 byte payload

  assert(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 2)
  for (int i = 0; i < NBUFS; i++) {
    if (i != 2)
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  assert(4 == rlc2.get_buffer_state());

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  retx1.N_bytes = rlc1.read_pdu(retx1.msg, 27); // 7 byte header + 20 data

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);

  assert(31 == rlc1.get_buffer_state());

  // Read the remaining segment
  byte_buffer_t retx2;
  retx2.N_bytes = rlc1.read_pdu(retx2.msg, 34); // 7 byte header + 24 data

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);

  assert(tester.n_sdus == 5);
  for (int i = 0; i < tester.n_sdus; i++) {
    if (tester.sdus[i]->N_bytes != 10)
      return -1;
    for (int j = 0; j < 10; j++)
      if (tester.sdus[i]->msg[j] != j)
        return -1;
  }

  return 0;
}

bool resegment_test_6()
{
  // SDUs:                |10|10|10|  54  |  54  |  54  |  54  |  54  | 54 |
  // PDUs:                |10|10|10|                270               | 54 |
  // Retx PDU segments:            |  120           |      150        |

  rlc_am_tester         tester;
  srslte::timer_handler timers(8);
  int                   len = 0;

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);
  rlc_am_lte rlc2(rrc_log2, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[9];
  for (int i = 0; i < 3; i++) {
    sdu_bufs[i] = srslte::allocate_unique_buffer(*pool, true);
    for (int j = 0; j < 10; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes = 10; // Give each buffer a size of 10 bytes
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }
  for (int i = 3; i < 9; i++) {
    sdu_bufs[i] = srslte::allocate_unique_buffer(*pool, true);
    for (int j = 0; j < 54; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes = 54;
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  assert(369 == rlc1.get_buffer_state());

  // Read PDUs from RLC1 (10, 10, 10, 270, 54)
  byte_buffer_t pdu_bufs[5];
  for (int i = 0; i < 3; i++) {
    len                 = rlc1.read_pdu(pdu_bufs[i].msg, 12);
    pdu_bufs[i].N_bytes = len;
  }
  len                 = rlc1.read_pdu(pdu_bufs[3].msg, 278);
  pdu_bufs[3].N_bytes = len;
  len                 = rlc1.read_pdu(pdu_bufs[4].msg, 56);
  pdu_bufs[4].N_bytes = len;

  assert(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 3)
  for (int i = 0; i < 5; i++) {
    if (i != 3)
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  assert(4 == rlc2.get_buffer_state());

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  len                = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status
  status_buf.N_bytes = len;

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  assert(278 == rlc1.get_buffer_state());

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  len           = rlc1.read_pdu(retx1.msg, 129);
  retx1.N_bytes = len;

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);

  assert(159 == rlc1.get_buffer_state());

  // Read the remaining segment
  byte_buffer_t retx2;
  len           = rlc1.read_pdu(retx2.msg, 162);
  retx2.N_bytes = len;

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);

  assert(tester.n_sdus == 9);
  for (int i = 0; i < 3; i++) {
    assert(tester.sdus[i]->N_bytes == 10);
    for (int j = 0; j < 10; j++)
      assert(tester.sdus[i]->msg[j] == j);
  }
  for (int i = 3; i < 9; i++) {
    if (i >= tester.n_sdus)
      return -1;
    if (tester.sdus[i]->N_bytes != 54)
      return -1;
    for (int j = 0; j < 54; j++) {
      if (tester.sdus[i]->msg[j] != j)
        return -1;
    }
  }

  return 0;
}

// Retransmission of PDU segments of the same size
bool resegment_test_7()
{
  // SDUs:                |         30         |         30         |
  // PDUs:                |    13  |   13  |  11   |   13   |   10  |
  // Rxed PDUs            |    13  |   13  |       |   13   |   10  |
  // Retx PDU segments:                    | 4 | 7 |
  // Retx PDU segments:                    |3|3]3|2|
  const uint32_t N_SDU_BUFS = 2;
  const uint32_t N_PDU_BUFS = 5;
  const uint32_t sdu_size   = 30;

#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_test7.pcap", 0);
  rlc_am_tester tester(&pcap);
#else
  rlc_am_tester tester(NULL);
#endif
  srslte::timer_handler timers(8);

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);
  rlc_am_lte rlc2(rrc_log2, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 2 SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[N_SDU_BUFS];
  for (uint32_t i = 0; i < N_SDU_BUFS; i++) {
    sdu_bufs[i] = srslte::allocate_unique_buffer(*pool, true);
    for (uint32_t j = 0; j < sdu_size; j++) {
      sdu_bufs[i]->msg[j] = i;
    }
    sdu_bufs[i]->N_bytes = sdu_size; // Give each buffer a size of 15 bytes
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  assert(65 == rlc1.get_buffer_state());

  // Read PDUs from RLC1 (15 bytes each)
  byte_buffer_t pdu_bufs[N_PDU_BUFS];
  for (uint32_t i = 0; i < N_PDU_BUFS; i++) {
    pdu_bufs[i].N_bytes = rlc1.read_pdu(pdu_bufs[i].msg, 15); // 2 bytes for header + 12 B payload
    assert(pdu_bufs[i].N_bytes);
  }

  // Step timers until poll_retx timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  // RLC should try to retx a random PDU because it needs to request a status from the receiver
  assert(0 != rlc1.get_buffer_state());

  // Skip PDU with SN 2
  for (uint32_t i = 0; i < N_PDU_BUFS; i++) {
    if (i != 2) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
#if HAVE_PCAP
      pcap.write_dl_am_ccch(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
#endif
    }
  }

  // Step timers until reordering timeout expires
  cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  // RLC should try to retransmit a random PDU because it needs to re-request a status PDU from the receiver
  assert(0 != rlc1.get_buffer_state());

  // first round of retx, forcing resegmentation
  byte_buffer_t retx[4];
  for (uint32_t i = 0; i < 4; i++) {
    assert(0 != rlc1.get_buffer_state());
    retx[i].N_bytes = rlc1.read_pdu(retx[i].msg, 7);
    assert(retx[i].N_bytes);

    // Write the last two segments to RLC2
    if (i > 1) {
      rlc2.write_pdu(retx[i].msg, retx[i].N_bytes);
#if HAVE_PCAP
      pcap.write_dl_am_ccch(retx[i].msg, retx[i].N_bytes);
#endif
    }
  }

  // Read status PDU from RLC2
  assert(rlc2.get_buffer_state());
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
#if HAVE_PCAP
  pcap.write_ul_am_ccch(status_buf.msg, status_buf.N_bytes);
#endif

  assert(15 == rlc1.get_buffer_state());

  // second round of retx, forcing resegmentation
  byte_buffer_t retx2[4];
  for (uint32_t i = 0; i < 4; i++) {
    assert(rlc1.get_buffer_state() != 0);
    retx2[i].N_bytes = rlc1.read_pdu(retx2[i].msg, 9);
    assert(retx2[i].N_bytes != 0);

    rlc2.write_pdu(retx2[i].msg, retx2[i].N_bytes);
#if HAVE_PCAP
    pcap.write_dl_am_ccch(retx[i].msg, retx[i].N_bytes);
#endif
  }

  // check buffer states
  assert(0 == rlc1.get_buffer_state());

  // Step timers until poll_retx timeout expires
  cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  assert(rlc2.get_buffer_state());
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
#if HAVE_PCAP
  pcap.write_ul_am_ccch(status_buf.msg, status_buf.N_bytes);
#endif

  // check status again
  assert(0 == rlc1.get_buffer_state());
  assert(0 == rlc2.get_buffer_state());

  // Check number of SDUs and their content
  assert(tester.n_sdus == N_SDU_BUFS);
  for (int i = 0; i < tester.n_sdus; i++) {
    if (tester.sdus[i]->N_bytes != sdu_size)
      return -1;
    for (uint32_t j = 0; j < N_SDU_BUFS; j++) {
      if (tester.sdus[i]->msg[j] != i)
        return -1;
    }
  }

#if HAVE_PCAP
  pcap.close();
#endif

  return 0;
}

// Retransmission of PDU segments with different size
bool resegment_test_8()
{
  // SDUs:                |         30         |         30         |
  // PDUs:                |    15   |   15  |   15   |   15   |   15   |
  // Rxed PDUs            |    15   |                |   15   |   15   |
  // Retx PDU segments:                 | 7  | 7  | 7  | 7  |
  // Retx PDU segments:             | 6 | 6 ] 6 | 6 | 6 | 6 | 6 | 6 |
  const uint32_t N_SDU_BUFS = 2;
  const uint32_t N_PDU_BUFS = 5;
  const uint32_t sdu_size   = 30;

#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_test8.pcap", 0);
  rlc_am_tester tester(&pcap);
#else
  rlc_am_tester tester(NULL);
#endif
  srslte::timer_handler timers(8);

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);
  rlc_am_lte rlc2(rrc_log2, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 2 SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[N_SDU_BUFS];
  for (uint32_t i = 0; i < N_SDU_BUFS; i++) {
    sdu_bufs[i] = srslte::allocate_unique_buffer(*pool, true);
    for (uint32_t j = 0; j < sdu_size; j++) {
      sdu_bufs[i]->msg[j] = i;
    }
    sdu_bufs[i]->N_bytes = sdu_size; // Give each buffer a size of 15 bytes
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  assert(65 == rlc1.get_buffer_state());

  // Read PDUs from RLC1 (15 bytes each)
  byte_buffer_t pdu_bufs[N_PDU_BUFS];
  for (uint32_t i = 0; i < N_PDU_BUFS; i++) {
    pdu_bufs[i].N_bytes = rlc1.read_pdu(pdu_bufs[i].msg, 15); // 12 bytes for header + payload
    assert(pdu_bufs[i].N_bytes);
  }

  assert(0 == rlc1.get_buffer_state());

  // Skip PDU one and two
  for (uint32_t i = 0; i < N_PDU_BUFS; i++) {
    if (i < 1 || i > 2) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
#if HAVE_PCAP
      pcap.write_dl_am_ccch(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
#endif
    }
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  // what PDU to retransmit is random but it must not be zero
  assert(0 != rlc1.get_buffer_state());

  // first round of retx, forcing resegmentation
  byte_buffer_t retx[4];
  for (uint32_t i = 0; i < 3; i++) {
    assert(rlc1.get_buffer_state());
    retx[i].N_bytes = rlc1.read_pdu(retx[i].msg, 8);
    assert(retx[i].N_bytes);

    // Write the last two segments to RLC2
    if (i > 1) {
      rlc2.write_pdu(retx[i].msg, retx[i].N_bytes);
#if HAVE_PCAP
      pcap.write_dl_am_ccch(retx[i].msg, retx[i].N_bytes);
#endif
    }
  }

  // Step timers until reordering timeout expires
  cnt = 7;
  while (cnt--) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  assert(rlc2.get_buffer_state());
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
#if HAVE_PCAP
  pcap.write_ul_am_ccch(status_buf.msg, status_buf.N_bytes);
#endif

  assert(15 == rlc1.get_buffer_state());

  // second round of retx, reduce grant size to force different segment sizes
  byte_buffer_t retx2[20];
  for (uint32_t i = 0; i < 7; i++) {
    assert(rlc1.get_buffer_state() != 0);
    retx2[i].N_bytes = rlc1.read_pdu(retx2[i].msg, 9);
    assert(retx2[i].N_bytes != 0);
    rlc2.write_pdu(retx2[i].msg, retx2[i].N_bytes);
#if HAVE_PCAP
    pcap.write_dl_am_ccch(retx[i].msg, retx[i].N_bytes);
#endif
  }

  // get BSR from RLC2
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
#if HAVE_PCAP
  pcap.write_ul_am_ccch(status_buf.msg, status_buf.N_bytes);
#endif

  // check buffer states
  if (rlc1.get_buffer_state() != 0) {
    return -1;
  };
  if (rlc2.get_buffer_state() != 0) {
    return -1;
  };

  // Check number of SDUs and their content
  assert(tester.n_sdus == N_SDU_BUFS);
  for (int i = 0; i < tester.n_sdus; i++) {
    if (tester.sdus[i]->N_bytes != sdu_size)
      return -1;
    for (uint32_t j = 0; j < N_SDU_BUFS; j++) {
      if (tester.sdus[i]->msg[j] != i)
        return -1;
    }
  }

#if HAVE_PCAP
  pcap.close();
#endif

  return 0;
}

bool reset_test()
{
  rlc_am_tester         tester;
  srslte::timer_handler timers(8);
  int                   len = 0;

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 1 SDU of size 10 into RLC1
  byte_buffer_pool*    pool    = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_buf = srslte::allocate_unique_buffer(*pool, true);
  sdu_buf->msg[0]              = 1; // Write the index into the buffer
  sdu_buf->N_bytes             = 100;
  rlc1.write_sdu(std::move(sdu_buf));

  // read 1 PDU from RLC1 and force segmentation
  byte_buffer_t pdu_bufs;
  len              = rlc1.read_pdu(pdu_bufs.msg, 4);
  pdu_bufs.N_bytes = len;

  // reset RLC1
  rlc1.stop();

  // read another PDU segment from RLC1
  len              = rlc1.read_pdu(pdu_bufs.msg, 4);
  pdu_bufs.N_bytes = len;

  // now empty RLC buffer
  len              = rlc1.read_pdu(pdu_bufs.msg, 100);
  pdu_bufs.N_bytes = len;

  if (0 != rlc1.get_buffer_state()) {
    return -1;
  }

  return 0;
}

bool resume_test()
{
  rlc_am_tester         tester;
  srslte::timer_handler timers(8);
  int                   len = 0;

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 1 SDU of size 10 into RLC1
  byte_buffer_pool*    pool    = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_buf = srslte::allocate_unique_buffer(*pool, true);
  sdu_buf->msg[0]              = 1; // Write the index into the buffer
  sdu_buf->N_bytes             = 100;
  rlc1.write_sdu(std::move(sdu_buf));

  // read 1 PDU from RLC1 and force segmentation
  byte_buffer_t pdu_bufs;
  len              = rlc1.read_pdu(pdu_bufs.msg, 4);
  pdu_bufs.N_bytes = len;

  // reestablish RLC1
  rlc1.reestablish();

  // resume RLC1
  rlc1.resume();

  // Buffer should be zero
  if (0 != rlc1.get_buffer_state()) {
    return -1;
  }

  // Do basic test
  byte_buffer_t pdu_bufs_tx[NBUFS];
  basic_test_tx(&rlc1, pdu_bufs_tx);

  return 0;
}

bool stop_test()
{
  rlc_am_tester         tester;
  srslte::timer_handler timers(8);

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // start thread reading
  ul_writer writer(&rlc1);
  writer.start(-2);

  // let writer thread block on tx_queue
  usleep(1e6);

  // stop RLC1
  rlc1.stop();

  return 0;
}

// This test checks if status PDUs are generated even though the grant size may not
// be enough to fit all SNs that would need to be NACKed
bool status_pdu_test()
{
  rlc_am_tester         tester;
  srslte::timer_handler timers(8);
  int                   len = 0;

  rlc_am_lte rlc1(rrc_log1, 1, &tester, &tester, &timers);
  rlc_am_lte rlc2(rrc_log2, 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i]          = srslte::allocate_unique_buffer(*pool, true);
    sdu_bufs[i]->msg[0]  = i; // Write the index into the buffer
    sdu_bufs[i]->N_bytes = 1; // Give each buffer a size of 1 byte
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  assert(14 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (1 byte each)
  byte_buffer_t pdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    len                 = rlc1.read_pdu(pdu_bufs[i].msg, 4); // 2 byte header + 1 byte payload
    pdu_bufs[i].N_bytes = len;
  }

  assert(0 == rlc1.get_buffer_state());

  // Only pass last PDUs to RLC2
  for (int i = 0; i < NBUFS; i++) {
    if (i == 4) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
    }
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  uint32_t buffer_state = rlc2.get_buffer_state();
  assert(8 == buffer_state);

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  len                = rlc2.read_pdu(status_buf.msg, 5); // provide only small grant
  status_buf.N_bytes = len;

  assert(status_buf.N_bytes != 0);

  // check status PDU doesn't contain ACK_SN in NACK list
  rlc_status_pdu_t status_pdu = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_pdu);
  if (rlc_am_is_valid_status_pdu(status_pdu) == false) {
    return -1;
  }

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  assert(3 == rlc1.get_buffer_state()); // 2 byte header + 1 byte payload

  // Read the retx PDU from RLC1
  byte_buffer_t retx;
  len          = rlc1.read_pdu(retx.msg, 10);
  retx.N_bytes = len;

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx.msg, retx.N_bytes);

  // Step timers until reordering timeout expires
  cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  // get buffer state and status PDU again
  status_buf.clear();
  len                = rlc2.read_pdu(status_buf.msg, 10); // big enough grant to fit full status PDU
  status_buf.N_bytes = len;
  assert(status_buf.N_bytes != 0);

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // retransmission of remaining PDUs
  for (int i = 0; i < 3; i++) {
    retx.clear();
    len          = rlc1.read_pdu(retx.msg, 3);
    retx.N_bytes = len;

    // Write the retx PDU to RLC2
    rlc2.write_pdu(retx.msg, retx.N_bytes);
  }

  assert(tester.n_sdus == NBUFS);
  for (int i = 0; i < tester.n_sdus; i++) {
    if (tester.sdus[i]->N_bytes != 1)
      return -1;
    if (*(tester.sdus[i]->msg) != i)
      return -1;
  }

  return 0;
}

int main(int argc, char** argv)
{
  rrc_log1->set_level(srslte::LOG_LEVEL_DEBUG);
  rrc_log2->set_level(srslte::LOG_LEVEL_DEBUG);
  rrc_log1->set_hex_limit(-1);
  rrc_log2->set_hex_limit(-1);

  if (meas_obj_test()) {
    printf("basic_test failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  if (concat_test()) {
    printf("concat_test failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  if (segment_test(true)) {
    printf("segment_test with in-order PDU reception failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  if (segment_test(false)) {
    printf("segment_test with out-of-order PDU reception failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  if (retx_test()) {
    printf("retx_test failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  if (resegment_test_1()) {
    printf("resegment_test_1 failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  if (resegment_test_2()) {
    printf("resegment_test_2 failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  if (resegment_test_3()) {
    printf("resegment_test_3 failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  if (resegment_test_4()) {
    printf("resegment_test_4 failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  if (resegment_test_5()) {
    printf("resegment_test_5 failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  if (resegment_test_6()) {
    printf("resegment_test_6 failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  rrc_log1->set_hex_limit(100);
  rrc_log2->set_hex_limit(100);
  if (resegment_test_7()) {
    printf("resegment_test_7 failed\n");
    exit(-1);
  }
  byte_buffer_pool::get_instance()->cleanup();

  if (resegment_test_8()) {
    printf("resegment_test_8 failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();
  rrc_log1->set_hex_limit(-1);
  rrc_log2->set_hex_limit(-1);

  if (reset_test()) {
    printf("reset_test failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  if (stop_test()) {
    printf("stop_test failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  if (resume_test()) {
    printf("resume_test failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  if (status_pdu_test()) {
    printf("status_pdu_test failed\n");
    exit(-1);
  };
  byte_buffer_pool::get_instance()->cleanup();

  return 0;
}
