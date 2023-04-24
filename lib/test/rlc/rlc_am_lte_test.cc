/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "rlc_test_common.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/rlc_pcap.h"
#include "srsran/common/test_common.h"
#include "srsran/common/threads.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/rlc/rlc_am_lte.h"

#define NBUFS 5
#define HAVE_PCAP 0
#define SDU_SIZE 500

using namespace srsue;
using namespace srsran;

class ul_writer : public thread
{
public:
  ul_writer(rlc_am* rlc_) : rlc(rlc_), thread("UL_WRITER") {}
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
      unique_byte_buffer_t pdu = srsran::make_byte_buffer();
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

  rlc_am*           rlc     = nullptr;
  std::atomic<bool> running = {false};
};

int basic_test_tx(rlc_am* rlc, byte_buffer_t pdu_bufs[NBUFS])
{
  // Push 5 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i; // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = 1; // Give each buffer a size of 1 byte
    sdu_bufs[i]->md.pdcp_sn = i; // PDCP SN for notifications
    rlc->write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT(13 == rlc->get_buffer_state()); // 2 Bytes for fixed header + 6 for LIs + 5 for payload

  // Read 5 PDUs from RLC1 (1 byte each)
  for (int i = 0; i < NBUFS; i++) {
    uint32_t len        = rlc->read_pdu(pdu_bufs[i].msg, 3); // 2 bytes for header + 1 byte payload
    pdu_bufs[i].N_bytes = len;
    TESTASSERT(3 == len);
  }

  TESTASSERT(0 == rlc->get_buffer_state());
  return SRSRAN_SUCCESS;
}

int basic_test()
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  // before configuring entity
  TESTASSERT(0 == rlc1.get_buffer_state());

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

  TESTASSERT(2 == rlc2.get_buffer_state());

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  int           len  = rlc2.read_pdu(status_buf.msg, 2);
  status_buf.N_bytes = len;

  TESTASSERT(0 == rlc2.get_buffer_state());

  // Assert status is correct
  rlc_status_pdu_t status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.ack_sn == 5); // 5 is the last SN that was not received.
  TESTASSERT(rlc_am_is_valid_status_pdu(status_check));

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Check PDCP notifications
  TESTASSERT(tester.notified_counts.size() == 5);
  for (uint16_t i = 0; i < tester.sdus.size(); i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == 1);
    TESTASSERT(*(tester.sdus[i]->msg) == i);
    TESTASSERT(tester.notified_counts[i] == 1);
  }

  // Check statistics
  TESTASSERT(rx_is_tx(rlc1.get_metrics(), rlc2.get_metrics()));

  return SRSRAN_SUCCESS;
}

int concat_test()
{
  rlc_am_tester         tester(true, nullptr);
  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i; // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = 1; // Give each buffer a size of 1 byte
    sdu_bufs[i]->md.pdcp_sn = i; // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT(13 == rlc1.get_buffer_state()); // 2 Bytes for fixed header + 6 for LIs + 5 for payload

  // Read 1 PDUs from RLC1 containing all 5 SDUs
  byte_buffer_t pdu_buf;
  int           len = rlc1.read_pdu(pdu_buf.msg, 13); // 8 bytes for header + payload
  pdu_buf.N_bytes   = len;

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Write PDU into RLC2
  rlc2.write_pdu(pdu_buf.msg, pdu_buf.N_bytes);

  // Check status report
  TESTASSERT(2 == rlc2.get_buffer_state());
  byte_buffer_t status_buf;
  len                = rlc2.read_pdu(status_buf.msg, 2);
  status_buf.N_bytes = len;

  TESTASSERT(0 == rlc2.get_buffer_state());

  // Assert status is correct
  rlc_status_pdu_t status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.ack_sn == 1); // 1 is the last SN that was not received.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  TESTASSERT(tester.sdus.size() == 5);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == 1);
    TESTASSERT(*(tester.sdus[i]->msg) == i);
  }

  // Check PDCP notifications
  TESTASSERT(tester.notified_counts.size() == 5);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == 1);
    TESTASSERT(*(tester.sdus[i]->msg) == i);
    TESTASSERT(tester.notified_counts[i] == 1);
  }

  // Check statistics
  TESTASSERT(rx_is_tx(rlc1.get_metrics(), rlc2.get_metrics()));

  return SRSRAN_SUCCESS;
}

int segment_test(bool in_seq_rx)
{
  rlc_am_tester         tester(true, nullptr);
  srsran::timer_handler timers(8);
  int                   len = 0;

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i] = srsran::make_byte_buffer();
    for (int j = 0; j < 10; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes    = 10; // Give each buffer a size of 10 bytes
    sdu_bufs[i]->md.pdcp_sn = i;  // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT(58 == rlc1.get_buffer_state()); // 2 bytes for header + 6 bytes for LI + 50 bytes for payload

  // Read PDUs from RLC1 (force segmentation)
  byte_buffer_t pdu_bufs[20];
  int           n_pdus = 0;
  while (rlc1.get_buffer_state() > 0) {
    len                        = rlc1.read_pdu(pdu_bufs[n_pdus].msg, 10); // 2 header + payload
    pdu_bufs[n_pdus++].N_bytes = len;
  }

  TESTASSERT(0 == rlc1.get_buffer_state());

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
    TESTASSERT(2 == rlc2.get_buffer_state());

    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    len                = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status
    status_buf.N_bytes = len;

    // Assert status is correct
    rlc_status_pdu_t status_check = {};
    rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
    TESTASSERT(status_check.ack_sn == n_pdus); // n_pdus (8) is the last SN that was not received.

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check all notification of ack'ed PDUs
    TESTASSERT(tester.notified_counts.size() == 5);
    for (int i = 0; i < NBUFS; i++) {
      auto not_it = tester.notified_counts.find(i);
      TESTASSERT(not_it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
    }
  }

  TESTASSERT(0 == rlc2.get_buffer_state());

  TESTASSERT(tester.sdus.size() == 5);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == 10);
    for (int j = 0; j < 10; j++) {
      TESTASSERT(tester.sdus[i]->msg[j] == j);
    }
  }

  // Check statistics
  TESTASSERT(rx_is_tx(rlc1.get_metrics(), rlc2.get_metrics()));

  return SRSRAN_SUCCESS;
}

int retx_test()
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  int           len = 0;

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i; // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = 1; // Give each buffer a size of 1 byte
    sdu_bufs[i]->md.pdcp_sn = i; // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT(13 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (1 byte each)
  byte_buffer_t pdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    len                 = rlc1.read_pdu(pdu_bufs[i].msg, 3); // 2 byte header + 1 byte payload
    pdu_bufs[i].N_bytes = len;
  }

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 1)
  for (int i = 0; i < NBUFS; i++) {
    if (i != 1) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
    }
  }

  // check buffered bytes at receiver, 3 PDUs with one 1 B each (SN=0 has been delivered already)
  rlc_bearer_metrics_t metrics = rlc2.get_metrics();
  TESTASSERT(metrics.rx_buffered_bytes == 3);

  // Step timers until reordering timeout expires
  for (int cnt = 0; cnt < 5; cnt++) {
    timers.step_all();
  }

  uint32_t buffer_state = rlc2.get_buffer_state();
  TESTASSERT(4 == buffer_state);

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  len                = rlc2.read_pdu(status_buf.msg, buffer_state); // provide exactly the reported buffer state
  status_buf.N_bytes = len;

  // Assert all bytes for status PDU were read
  buffer_state = rlc2.get_buffer_state();
  TESTASSERT(0 == buffer_state);

  // Assert status is correct
  rlc_status_pdu_t status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 1);           // 1 packet was lost.
  TESTASSERT(status_check.nacks[0].nack_sn == 1); // SN 1 was lost.
  TESTASSERT(status_check.ack_sn == 5);           // Delivered up to SN 4.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  TESTASSERT(3 == rlc1.get_buffer_state()); // 2 byte header + 1 byte payload

  // Check notifications of ack'ed PDUs
  TESTASSERT(tester.notified_counts.size() == 4);
  for (int i = 0; i < NBUFS; i++) {
    auto not_it = tester.notified_counts.find(i);
    if (i != 1) {
      TESTASSERT(not_it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
    } else {
      TESTASSERT(not_it == tester.notified_counts.end());
    }
  }

  // Read the retx PDU from RLC1
  byte_buffer_t retx;
  len          = rlc1.read_pdu(retx.msg, 3); // 2 byte header + 1 byte payload
  retx.N_bytes = len;

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx.msg, retx.N_bytes);

  TESTASSERT(tester.sdus.size() == 5);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    if (tester.sdus[i]->N_bytes != 1)
      return -1;
    if (*(tester.sdus[i]->msg) != i)
      return -1;
  }

  // Step timers until poll Retx timeout expires
  for (int cnt = 0; cnt < 5; cnt++) {
    timers.step_all();
  }

  // Get status report of RETX PDU
  buffer_state = rlc2.get_buffer_state();
  TESTASSERT(2 == buffer_state);
  len                = rlc2.read_pdu(status_buf.msg, buffer_state); // provide exactly the reported buffer state
  status_buf.N_bytes = len;

  // Assert status is correct
  status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 0); // No packet was lost.
  TESTASSERT(status_check.ack_sn == 5); // Delivered up to SN 4.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Check all notification of ack'ed PDUs
  TESTASSERT(tester.notified_counts.size() == 5);
  for (int i = 0; i < NBUFS; i++) {
    auto not_it = tester.notified_counts.find(i);
    TESTASSERT(not_it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
  }

  return 0;
}

// Test correct upper layer signaling when maxRetx (default 4) have been reached
int max_retx_test()
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  int           len = 0;

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);

  const rlc_config_t rlc_cfg = rlc_config_t::default_rlc_am_config();
  if (not rlc1.configure(rlc_cfg)) {
    return -1;
  }

  // Push 2 SDUs into RLC1
  const uint32_t       n_sdus = 2;
  unique_byte_buffer_t sdu_bufs[n_sdus];
  for (uint32_t i = 0; i < n_sdus; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i; // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = 1; // Give each buffer a size of 1 byte
    sdu_bufs[i]->md.pdcp_sn = i; // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  // Read 2 PDUs from RLC1 (1 byte each)
  const uint32_t n_pdus = 2;
  byte_buffer_t  pdu_bufs[n_pdus];
  for (uint32_t i = 0; i < n_pdus; i++) {
    len                 = rlc1.read_pdu(pdu_bufs[i].msg, 3); // 2 byte header + 1 byte payload
    pdu_bufs[i].N_bytes = len;
  }

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Fake status PDU that ack SN=1
  rlc_status_pdu_t fake_status = {};
  fake_status.ack_sn           = 2; // delivered up to SN=1
  fake_status.N_nack           = 1; // one SN was lost
  fake_status.nacks[0].nack_sn = 0; // it was SN=0 that was lost

  // pack into PDU
  byte_buffer_t status_pdu;
  rlc_am_write_status_pdu(&fake_status, &status_pdu);

  // We've Tx'ed once already, loop until the max is reached
  for (uint32_t retx_count = 0; retx_count < rlc_cfg.am.max_retx_thresh; ++retx_count) {
    // we've not yet reached max attempts
    TESTASSERT(tester.max_retx_triggered == false);

    // Write status PDU to RLC1
    rlc1.write_pdu(status_pdu.msg, status_pdu.N_bytes);

    byte_buffer_t pdu_buf;
    len = rlc1.read_pdu(pdu_buf.msg, 3);
  }

  // Now maxRetx should have been triggered
  TESTASSERT(tester.max_retx_triggered == true);

  return SRSRAN_SUCCESS;
}

// Purpose: test correct retx of lost segment and pollRetx timer expiration
int segment_retx_test()
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  int           len = 0;

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push SDU(s) into RLC1
  const uint32_t       nof_sdus = 1; // just one SDU to make sure the transmitter sets polling bit
  unique_byte_buffer_t sdu_bufs[nof_sdus];

  for (uint32_t i = 0; i < nof_sdus; i++) {
    sdu_bufs[i]          = srsran::make_byte_buffer();
    sdu_bufs[i]->N_bytes = 10; // Give each buffer a size of 10 bytes
    std::fill(sdu_bufs[i]->msg, sdu_bufs[i]->msg + sdu_bufs[i]->N_bytes, 0);
    sdu_bufs[i]->msg[0] = i; // Write the index into the buffer
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  // Read 2 PDUs from RLC1
  const uint32_t nof_pdus = 2;
  byte_buffer_t  pdu_bufs[nof_pdus];
  for (uint32_t i = 0; i < nof_pdus; i++) {
    len                 = rlc1.read_pdu(pdu_bufs[i].msg, 7); // 2 byte header
    pdu_bufs[i].N_bytes = len;
  }

  TESTASSERT(rlc1.get_buffer_state() == 0);

  // Step timers until poll Retx timeout expires
  for (int cnt = 0; cnt < 5; cnt++) {
    timers.step_all();
  }

  uint32_t buffer_state = rlc1.get_buffer_state();
  TESTASSERT(buffer_state == 7);

  // Read retx PDU from RLC1
  byte_buffer_t retx_pdu;
  len              = rlc1.read_pdu(retx_pdu.msg, buffer_state); // provide exactly the reported buffer state
  retx_pdu.N_bytes = len;

  // Write retx segment to RLC2
  rlc2.write_pdu(retx_pdu.msg, retx_pdu.N_bytes);

  buffer_state = rlc2.get_buffer_state(); // Status PDU
  TESTASSERT(buffer_state == 2);

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  len                = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status
  status_buf.N_bytes = len;

  // Assert status is correct
  rlc_status_pdu_t status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 0); // No packet was lost.
  TESTASSERT(status_check.ack_sn == 1); // Delivered up to SN 0.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Make sure no notifications yet
  TESTASSERT(tester.notified_counts.size() == 0);

  // Step timers again until poll Retx timeout expires
  for (int cnt = 0; cnt < 5; cnt++) {
    TESTASSERT(rlc1.get_buffer_state() == 0); // No status transmissions until pollRetx expires
    timers.step_all();
  }

  // read buffer state from RLC1 again to see if it has rescheduled SN=1 for retx
  buffer_state = rlc1.get_buffer_state(); // Status PDU
  TESTASSERT(buffer_state == 7);

  // Read 2nd retx PDU from RLC1
  byte_buffer_t retx_pdu2;
  len               = rlc1.read_pdu(retx_pdu2.msg, buffer_state); // provide exactly the reported buffer state
  retx_pdu2.N_bytes = len;

  // Write retx segment to RLC2
  rlc2.write_pdu(retx_pdu2.msg, retx_pdu2.N_bytes);

  // read Status PDU from RLC2 again
  len                = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status
  status_buf.N_bytes = len;
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Assert status is correct
  status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 0); // No packet was lost.
  TESTASSERT(status_check.ack_sn == 2); // Delivered up to SN 0.

  // Make sure SDU was notified
  TESTASSERT(tester.notified_counts.size() == 1);
  TESTASSERT(tester.notified_counts.find(0) != tester.notified_counts.end() && tester.notified_counts[0] == 1);

  TESTASSERT(tester.sdus.size() == nof_sdus);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    if (tester.sdus[i]->N_bytes != 10) {
      return SRSRAN_ERROR;
    }
    if (*(tester.sdus[i]->msg) != i) {
      return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

int resegment_test_1()
{
  // SDUs:                |  10  |  10  |  10  |  10  |  10  |
  // PDUs:                |  10  |  10  |  10  |  10  |  10  |
  // Retx PDU segments:          | 5 | 5|

  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  int           len = 0;

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i] = srsran::make_byte_buffer();
    for (int j = 0; j < 10; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes    = 10; // Give each buffer a size of 10 bytes
    sdu_bufs[i]->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT(58 == rlc1.get_buffer_state()); // 2 bytes for fixed header, 6 bytes for LIs, 50 bytes for data

  // Read 5 PDUs from RLC1 (10 bytes each)
  byte_buffer_t pdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    len                 = rlc1.read_pdu(pdu_bufs[i].msg, 12); // 12 bytes for header + payload
    pdu_bufs[i].N_bytes = len;
  }

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 1)
  for (int i = 0; i < NBUFS; i++) {
    if (i != 1)
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  // Step timers until reordering timeout expires
  for (int cnt = 0; cnt < 5; cnt++) {
    timers.step_all();
  }

  TESTASSERT(4 == rlc2.get_buffer_state());

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  len                = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status
  status_buf.N_bytes = len;

  // Assert status is correct
  rlc_status_pdu_t status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 1);           // 1 packet was lost.
  TESTASSERT(status_check.nacks[0].nack_sn == 1); // SN 1 was lost.
  TESTASSERT(status_check.ack_sn == 5);           // Delivered up to SN 5.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  TESTASSERT(12 == rlc1.get_buffer_state()); // 2 byte header + 10 data

  // Check notifications
  srslog::fetch_basic_logger("RLC_AM_1").debug("%ld", tester.notified_counts.size());
  TESTASSERT(tester.notified_counts.size() == 4);
  for (int i = 0; i < 5; i++) {
    auto it = tester.notified_counts.find(i);
    if (i != 1) {
      TESTASSERT(it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
    } else {
      TESTASSERT(it == tester.notified_counts.end());
    }
  }

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  len           = rlc1.read_pdu(retx1.msg, 9); // 4 byte header + 5 data
  retx1.N_bytes = len;

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);

  TESTASSERT(9 == rlc1.get_buffer_state());

  // Step timers to get status report
  for (int cnt = 0; cnt < 5; cnt++) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  status_buf         = {};
  len                = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status
  status_buf.N_bytes = len;

  // Assert status is correct
  status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 1);           // 1 packet was lost.
  TESTASSERT(status_check.nacks[0].nack_sn == 1); // SN 1 was lost.
  TESTASSERT(status_check.ack_sn == 5);           // Delivered up to SN 5.

  // Read the remaining segment
  byte_buffer_t retx2;
  len           = rlc1.read_pdu(retx2.msg, 9); // 4 byte header + 5 data
  retx2.N_bytes = len;

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);

  TESTASSERT(tester.sdus.size() == 5);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    if (tester.sdus[i]->N_bytes != 10)
      return -1;
    for (int j = 0; j < 10; j++)
      if (tester.sdus[i]->msg[j] != j)
        return -1;
  }

  // Step timers to get status report
  for (int cnt = 0; cnt < 5; cnt++) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  status_buf         = {};
  len                = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status
  status_buf.N_bytes = len;

  // Assert status is correct
  status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 0); // all packets delivered.
  TESTASSERT(status_check.ack_sn == 5); // Delivered up to SN 5.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Check notifications
  TESTASSERT(tester.notified_counts.size() == 5);
  for (int i = 0; i < 5; i++) {
    auto it = tester.notified_counts.find(i);
    TESTASSERT(it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
  }

  return 0;
}

int resegment_test_2()
{
  // SDUs:              |  10  |  10  |  10  |  10  |  10  |
  // PDUs:              | 5 |  10  |     20     |  10  | 5 |
  // Retx PDU segments:            |  10  |  10 |

  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i] = srsran::make_byte_buffer();
    for (int j = 0; j < 10; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes    = 10; // Give each buffer a size of 10 bytes
    sdu_bufs[i]->md.pdcp_sn = i;  // Give each buffer a size of 10 bytes
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT(58 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (5 bytes, 10 bytes, 20 bytes, 10 bytes, 5 bytes)
  byte_buffer_t pdu_bufs[NBUFS];
  pdu_bufs[0].N_bytes = rlc1.read_pdu(pdu_bufs[0].msg, 7);  // 2 byte header +  5 byte payload
  pdu_bufs[1].N_bytes = rlc1.read_pdu(pdu_bufs[1].msg, 14); // 4 byte header + 10 byte payload
  pdu_bufs[2].N_bytes = rlc1.read_pdu(pdu_bufs[2].msg, 25); // 5 byte header + 20 byte payload
  pdu_bufs[3].N_bytes = rlc1.read_pdu(pdu_bufs[3].msg, 14); // 4 byte header + 10 byte payload
  pdu_bufs[4].N_bytes = rlc1.read_pdu(pdu_bufs[4].msg, 7);  // 2 byte header +  5 byte payload

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 2)
  for (int i = 0; i < NBUFS; i++) {
    if (i != 2)
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  // Step timers until reordering timeout expires
  for (int cnt = 0; cnt < 5; cnt++) {
    timers.step_all();
  }

  TESTASSERT(4 == rlc2.get_buffer_state());

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Assert status is correct
  rlc_status_pdu_t status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 1); // One packet was lost.
  TESTASSERT(status_check.ack_sn == 5); // Delivered up to SN 5.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  TESTASSERT(25 == rlc1.get_buffer_state()); // 4 byte header + 20 data

  // Check notifications
  TESTASSERT(tester.notified_counts.size() == 2);
  for (int i = 0; i < 5; i++) {
    auto it = tester.notified_counts.find(i);
    if (i == 0 || i == 4) {
      TESTASSERT(it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
    } else {
      TESTASSERT(it == tester.notified_counts.end());
    }
  }

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  retx1.N_bytes = rlc1.read_pdu(retx1.msg, 16); // 6 byte header + 10 data

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);

  TESTASSERT(16 == rlc1.get_buffer_state());

  // Read the remaining segment
  byte_buffer_t retx2;
  retx2.N_bytes = rlc1.read_pdu(retx2.msg, 18); // 6 byte header + 12 data

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);

  TESTASSERT(tester.sdus.size() == 5);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    if (tester.sdus[i]->N_bytes != 10)
      return -1;
    for (int j = 0; j < 10; j++)
      if (tester.sdus[i]->msg[j] != j)
        return -1;
  }

  // Step timers until reordering timeout expires
  for (int cnt = 0; cnt < 5; cnt++) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Assert status is correct
  status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 0); // all packets delivered.
  TESTASSERT(status_check.ack_sn == 5); // Delivered up to SN 5.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
  TESTASSERT(tester.notified_counts.size() == 5);

  for (int i = 0; i < 5; i++) {
    auto it = tester.notified_counts.find(i);
    TESTASSERT(it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
  }
  return 0;
}

int resegment_test_3()
{
  // SDUs:              |  10  |  10  |  10  |  10  |  10  |
  // PDUs:              | 5 | 5|      20     |  10  |  10  |
  // Retx PDU segments:        |  10  |  10  |

  rlc_am_tester         tester(true, nullptr);
  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i] = srsran::make_byte_buffer();
    for (int j = 0; j < 10; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes    = 10; // Give each buffer a size of 10 bytes
    sdu_bufs[i]->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT(58 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (5 bytes, 5 bytes, 20 bytes, 10 bytes, 10 bytes)
  byte_buffer_t pdu_bufs[NBUFS];
  pdu_bufs[0].N_bytes = rlc1.read_pdu(pdu_bufs[0].msg, 7);  // 2 byte header +  5 byte payload
  pdu_bufs[1].N_bytes = rlc1.read_pdu(pdu_bufs[1].msg, 7);  // 2 byte header +  5 byte payload
  pdu_bufs[2].N_bytes = rlc1.read_pdu(pdu_bufs[2].msg, 24); // 4 byte header + 20 byte payload
  pdu_bufs[3].N_bytes = rlc1.read_pdu(pdu_bufs[3].msg, 12); // 2 byte header + 10 byte payload
  pdu_bufs[4].N_bytes = rlc1.read_pdu(pdu_bufs[4].msg, 12); // 2 byte header + 10 byte payload

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 2)
  for (int i = 0; i < NBUFS; i++) {
    if (i != 2)
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  // Step timers until reordering timeout expires
  for (int cnt = 0; cnt < 5; cnt++) {
    timers.step_all();
  }

  TESTASSERT(4 == rlc2.get_buffer_state());

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Assert status is correct
  rlc_status_pdu_t status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 1);           // One packet was lost.
  TESTASSERT(status_check.nacks[0].nack_sn == 2); // SN 2 was lost.
  TESTASSERT(status_check.ack_sn == 5);           // Delivered up to SN 5.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Check notifications
  TESTASSERT(tester.notified_counts.size() == 3);
  for (int i = 0; i < 5; i++) {
    auto it = tester.notified_counts.find(i);
    if (i == 0 || i == 3 || i == 4) {
      TESTASSERT(it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
    } else {
      TESTASSERT(it == tester.notified_counts.end());
    }
  }

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

  TESTASSERT(tester.sdus.size() == 5);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    if (tester.sdus[i]->N_bytes != 10)
      return -1;
    for (int j = 0; j < 10; j++)
      if (tester.sdus[i]->msg[j] != j)
        return -1;
  }

  // Get status from RLC 2
  for (int cnt = 0; cnt < 5; cnt++) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Assert status is correct
  status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 0); // all packets delivered.
  TESTASSERT(status_check.ack_sn == 5); // Delivered up to SN 5.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Check final notifications
  TESTASSERT(tester.notified_counts.size() == 5);
  for (int i = 0; i < 5; i++) {
    auto it = tester.notified_counts.find(i);
    TESTASSERT(it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
  }
  return 0;
}

int resegment_test_4()
{
  // SDUs:              |  10  |  10  |  10  |  10  |  10  |
  // PDUs:              | 5 | 5|         30         | 5 | 5|
  // Retx PDU segments:        |    15    |    15   |

  rlc_am_tester         tester(true, nullptr);
  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i] = srsran::make_byte_buffer();
    for (int j = 0; j < 10; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes    = 10; // Give each buffer a size of 10 bytes
    sdu_bufs[i]->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT(58 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (5 bytes, 5 bytes, 30 bytes, 5 bytes, 5 bytes)
  byte_buffer_t pdu_bufs[NBUFS];
  pdu_bufs[0].N_bytes = rlc1.read_pdu(pdu_bufs[0].msg, 7);  // 2 byte header +  5 byte payload
  pdu_bufs[1].N_bytes = rlc1.read_pdu(pdu_bufs[1].msg, 7);  // 2 byte header +  5 byte payload
  pdu_bufs[2].N_bytes = rlc1.read_pdu(pdu_bufs[2].msg, 35); // 5 byte header + 30 byte payload
  pdu_bufs[3].N_bytes = rlc1.read_pdu(pdu_bufs[3].msg, 7);  // 2 byte header +  5 byte payload
  pdu_bufs[4].N_bytes = rlc1.read_pdu(pdu_bufs[4].msg, 7);  // 2 byte header +  5 byte payload

  TESTASSERT(0 == rlc1.get_buffer_state());

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

  TESTASSERT(4 == rlc2.get_buffer_state());

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Assert status is correct
  rlc_status_pdu_t status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 1);           // one packet lost.
  TESTASSERT(status_check.nacks[0].nack_sn == 2); // SN 2 was lost.
  TESTASSERT(status_check.ack_sn == 5);           // Delivered up to SN 5.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  TESTASSERT(tester.notified_counts.size() == 2);
  for (int i = 0; i < 5; i++) {
    auto it = tester.notified_counts.find(i);
    if (i == 0 || i == 4) {
      TESTASSERT(it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
    } else {
      TESTASSERT(it == tester.notified_counts.end());
    }
  }

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  retx1.N_bytes = rlc1.read_pdu(retx1.msg, 21); // 6 byte header + 15 data

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);

  TESTASSERT(21 == rlc1.get_buffer_state());

  // Read the remaining segment
  byte_buffer_t retx2;
  retx2.N_bytes = rlc1.read_pdu(retx2.msg, 21);

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);

  TESTASSERT(tester.sdus.size() == 5);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    if (tester.sdus[i]->N_bytes != 10)
      return -1;
    for (int j = 0; j < 10; j++)
      if (tester.sdus[i]->msg[j] != j)
        return -1;
  }

  // Get status from RLC 2
  for (int i = 0; i < 5; i++) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Assert status is correct
  status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 0); // all packets delivered.
  TESTASSERT(status_check.ack_sn == 5); // Delivered up to SN 5.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Check final notifications
  TESTASSERT(tester.notified_counts.size() == 5);
  for (int i = 0; i < 5; i++) {
    auto it = tester.notified_counts.find(i);
    TESTASSERT(it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
  }
  return 0;
}

int resegment_test_5()
{
  // SDUs:              |  10  |  10  |  10  |  10  |  10  |
  // PDUs:              |2|3|            40            |3|2|
  // Retx PDU segments:     |     20      |     20     |

  rlc_am_tester         tester(true, nullptr);
  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i] = srsran::make_byte_buffer();
    for (int j = 0; j < 10; j++) {
      sdu_bufs[i]->msg[j] = i;
    }
    sdu_bufs[i]->N_bytes    = 10; // Give each buffer a size of 10 bytes
    sdu_bufs[i]->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT(58 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (2 bytes, 3 bytes, 40 bytes, 3 bytes, 2 bytes)
  byte_buffer_t pdu_bufs[NBUFS];
  pdu_bufs[0].N_bytes = rlc1.read_pdu(pdu_bufs[0].msg, 4);  // 2 byte header +  2 byte payload
  pdu_bufs[1].N_bytes = rlc1.read_pdu(pdu_bufs[1].msg, 5);  // 2 byte header +  3 byte payload
  pdu_bufs[2].N_bytes = rlc1.read_pdu(pdu_bufs[2].msg, 48); // 8 byte header + 40 byte payload
  pdu_bufs[3].N_bytes = rlc1.read_pdu(pdu_bufs[3].msg, 5);  // 2 byte header +  3 byte payload
  pdu_bufs[4].N_bytes = rlc1.read_pdu(pdu_bufs[4].msg, 4);  // 2 byte header +  2 byte payload

  TESTASSERT(0 == rlc1.get_buffer_state());

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

  TESTASSERT(4 == rlc2.get_buffer_state());

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Assert status is correct
  rlc_status_pdu_t status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 1);           // one packet was lost.
  TESTASSERT(status_check.nacks[0].nack_sn == 2); // SN 2 was lost.
  TESTASSERT(status_check.ack_sn == 5);           // Delivered up to SN 5.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Check notifications
  TESTASSERT(tester.notified_counts.size() == 0);

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  retx1.N_bytes = rlc1.read_pdu(retx1.msg, 27); // 7 byte header + 20 data

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);

  TESTASSERT(32 == rlc1.get_buffer_state());

  // Read the remaining segment
  byte_buffer_t retx2;
  retx2.N_bytes = rlc1.read_pdu(retx2.msg, 40);

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);

  TESTASSERT(tester.sdus.size() == 5);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == 10);
    for (int j = 0; j < 10; j++) {
      TESTASSERT(tester.sdus[i]->msg[j] == i);
    }
  }

  // Get status from RLC 2
  for (int i = 0; i < 5; i++) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Assert status is correct
  status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 0); // all packets delivered.
  TESTASSERT(status_check.ack_sn == 5); // Delivered up to SN 5.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Check final notifications
  TESTASSERT(tester.notified_counts.size() == 5);
  for (int i = 0; i < 5; i++) {
    auto it = tester.notified_counts.find(i);
    TESTASSERT(it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
  }

  return 0;
}

int resegment_test_6()
{
  // SDUs:                |10|10|10|  54  |  54  |  54  |  54  |  54  | 54 |
  // PDUs:                |10|10|10|                270               | 54 |
  // Retx PDU segments:            |  120           |      150        |

  rlc_am_tester         tester(true, nullptr);
  srsran::timer_handler timers(8);
  int                   len = 0;

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[9];
  for (int i = 0; i < 3; i++) {
    sdu_bufs[i] = srsran::make_byte_buffer();
    for (int j = 0; j < 10; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes    = 10; // Give each buffer a size of 10 bytes
    sdu_bufs[i]->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }
  for (int i = 3; i < 9; i++) {
    sdu_bufs[i] = srsran::make_byte_buffer();
    for (int j = 0; j < 54; j++)
      sdu_bufs[i]->msg[j] = j;
    sdu_bufs[i]->N_bytes    = 54;
    sdu_bufs[i]->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT(368 == rlc1.get_buffer_state());

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

  TESTASSERT(0 == rlc1.get_buffer_state());

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

  TESTASSERT(4 == rlc2.get_buffer_state());

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  len                = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status
  status_buf.N_bytes = len;

  // Assert status is correct
  rlc_status_pdu_t status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 1);           // One packet was lost.
  TESTASSERT(status_check.nacks[0].nack_sn == 3); // SN 3 was lost.
  TESTASSERT(status_check.ack_sn == 5);

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  TESTASSERT(278 == rlc1.get_buffer_state());

  // Check notifications
  TESTASSERT(tester.notified_counts.size() == 4);
  for (int i = 0; i < 5; i++) {
    auto it = tester.notified_counts.find(i);
    if (i == 0 || i == 1 || i == 2 || i == 8) {
      TESTASSERT(it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
    } else {
      TESTASSERT(it == tester.notified_counts.end());
    }
  }

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  len           = rlc1.read_pdu(retx1.msg, 129);
  retx1.N_bytes = len;

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);

  TESTASSERT(169 == rlc1.get_buffer_state());

  // Read the remaining segment
  byte_buffer_t retx2;
  len           = rlc1.read_pdu(retx2.msg, 169);
  retx2.N_bytes = len;

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);

  TESTASSERT(tester.sdus.size() == 9);
  for (int i = 0; i < 3; i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == 10);
    for (int j = 0; j < 10; j++)
      TESTASSERT(tester.sdus[i]->msg[j] == j);
  }
  for (uint32_t i = 3; i < 9; i++) {
    if (i >= tester.sdus.size()) {
      return SRSRAN_ERROR;
    }
    TESTASSERT(tester.sdus[i]->N_bytes == 54);
    for (int j = 0; j < 54; j++) {
      TESTASSERT(tester.sdus[i]->msg[j] == j);
    }
  }

  // Get status from RLC 2
  for (int i = 0; i < 5; i++) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Assert status is correct
  status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 0); // all packets delivered.
  TESTASSERT(status_check.ack_sn == 5); // Delivered up to SN 5.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Check final notifications
  TESTASSERT(tester.notified_counts.size() == 9);
  for (int i = 0; i < 9; i++) {
    auto it = tester.notified_counts.find(i);
    TESTASSERT(it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
  }

  return 0;
}

// Retransmission of PDU segments of the same size
int resegment_test_7()
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
  pcap.open("rlc_am_test7.pcap", rlc_config_t::default_rlc_am_config());
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, nullptr);
#endif
  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 2 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[N_SDU_BUFS];
  for (uint32_t i = 0; i < N_SDU_BUFS; i++) {
    sdu_bufs[i] = srsran::make_byte_buffer();
    for (uint32_t j = 0; j < sdu_size; j++) {
      sdu_bufs[i]->msg[j] = i;
    }
    sdu_bufs[i]->N_bytes    = sdu_size; // Give each buffer a size of 15 bytes
    sdu_bufs[i]->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT(64 == rlc1.get_buffer_state());

  // Read PDUs from RLC1 (15 bytes each)
  byte_buffer_t pdu_bufs[N_PDU_BUFS];
  for (uint32_t i = 0; i < N_PDU_BUFS; i++) {
    pdu_bufs[i].N_bytes = rlc1.read_pdu(pdu_bufs[i].msg, 15); // 2 bytes for header + 12 B payload
    TESTASSERT(pdu_bufs[i].N_bytes);
  }

  // Step timers until poll_retx timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  // RLC should try to retx a random PDU because it needs to request a status from the receiver
  TESTASSERT(0 != rlc1.get_buffer_state());

  // Skip PDU with SN 2
  for (uint32_t i = 0; i < N_PDU_BUFS; i++) {
    if (i != 2) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
#if HAVE_PCAP
      pcap.write_dl_ccch(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
#endif
    }
  }

  // Step timers until reordering timeout expires
  cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  // RLC should try to retransmit a random PDU because it needs to re-request a status PDU from the receiver
  TESTASSERT(0 != rlc1.get_buffer_state());

  // first round of retx, forcing resegmentation
  byte_buffer_t retx[4];
  for (uint32_t i = 0; i < 4; i++) {
    TESTASSERT(0 != rlc1.get_buffer_state());
    retx[i].N_bytes = rlc1.read_pdu(retx[i].msg, 7);
    TESTASSERT(retx[i].N_bytes);

    // Write the last two segments to RLC2
    if (i > 1) {
      rlc2.write_pdu(retx[i].msg, retx[i].N_bytes);
#if HAVE_PCAP
      pcap.write_dl_ccch(retx[i].msg, retx[i].N_bytes);
#endif
    }
  }

  // Read status PDU from RLC2
  TESTASSERT(rlc2.get_buffer_state());
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Assert status is correct
  rlc_status_pdu_t status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 1);           // one packet dropped.
  TESTASSERT(status_check.nacks[0].nack_sn == 2); // SN 2 dropped.
  TESTASSERT(status_check.ack_sn == 5);           // Delivered up to SN 5.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
#if HAVE_PCAP
  pcap.write_ul_ccch(status_buf.msg, status_buf.N_bytes);
#endif

  TESTASSERT(15 == rlc1.get_buffer_state());

  // Check notifications
  TESTASSERT(tester.notified_counts.size() == 0);

  // second round of retx, forcing resegmentation
  byte_buffer_t retx2[4];
  for (uint32_t i = 0; i < 4; i++) {
    TESTASSERT(rlc1.get_buffer_state() != 0);
    retx2[i].N_bytes = rlc1.read_pdu(retx2[i].msg, 9);
    TESTASSERT(retx2[i].N_bytes != 0);

    rlc2.write_pdu(retx2[i].msg, retx2[i].N_bytes);
#if HAVE_PCAP
    pcap.write_dl_ccch(retx[i].msg, retx[i].N_bytes);
#endif
  }

  // check buffer states
  TESTASSERT(0 == rlc1.get_buffer_state());

  // Step timers until poll_retx timeout expires
  cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  TESTASSERT(rlc2.get_buffer_state());
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Assert status is correct
  status_check = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_check);
  TESTASSERT(status_check.N_nack == 0); // all packets delivered.
  TESTASSERT(status_check.ack_sn == 5); // Delivered up to SN 5.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
#if HAVE_PCAP
  pcap.write_ul_ccch(status_buf.msg, status_buf.N_bytes);
#endif

  // check status again
  TESTASSERT(0 == rlc1.get_buffer_state());
  TESTASSERT(0 == rlc2.get_buffer_state());

  // Check number of SDUs and their content
  TESTASSERT(tester.sdus.size() == N_SDU_BUFS);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == sdu_size);
    for (uint32_t j = 0; j < N_SDU_BUFS; j++) {
      TESTASSERT(tester.sdus[i]->msg[j] == i);
    }
  }

  // Check final notifications
  TESTASSERT(tester.notified_counts.size() == 2);
  for (int i = 0; i < 2; i++) {
    auto it = tester.notified_counts.find(i);
    TESTASSERT(it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
  }

#if HAVE_PCAP
  pcap.close();
#endif

  return 0;
}

// Retransmission of PDU segments with different size
int resegment_test_8()
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
  pcap.open("rlc_am_test8.pcap", rlc_config_t::default_rlc_am_config());
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, nullptr);
#endif
  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 2 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[N_SDU_BUFS];
  for (uint32_t i = 0; i < N_SDU_BUFS; i++) {
    sdu_bufs[i] = srsran::make_byte_buffer();
    for (uint32_t j = 0; j < sdu_size; j++) {
      sdu_bufs[i]->msg[j] = i;
    }
    sdu_bufs[i]->N_bytes    = sdu_size; // Give each buffer a size of 30 bytes
    sdu_bufs[i]->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT(64 == rlc1.get_buffer_state());

  // Read PDUs from RLC1 (15 bytes each)
  byte_buffer_t pdu_bufs[N_PDU_BUFS];
  for (uint32_t i = 0; i < N_PDU_BUFS; i++) {
    pdu_bufs[i].N_bytes = rlc1.read_pdu(pdu_bufs[i].msg, 15); // 12 bytes for header + payload
    TESTASSERT(pdu_bufs[i].N_bytes);
  }

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Skip PDU one and two
  for (uint32_t i = 0; i < N_PDU_BUFS; i++) {
    if (i < 1 || i > 2) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
#if HAVE_PCAP
      pcap.write_dl_ccch(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
#endif
    }
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  // what PDU to retransmit is random but it must not be zero
  TESTASSERT(0 != rlc1.get_buffer_state());

  // first round of retx, forcing resegmentation
  byte_buffer_t retx[4];
  for (uint32_t i = 0; i < 3; i++) {
    TESTASSERT(rlc1.get_buffer_state());
    retx[i].N_bytes = rlc1.read_pdu(retx[i].msg, 8);
    TESTASSERT(retx[i].N_bytes);

    // Write the last two segments to RLC2
    if (i > 1) {
      rlc2.write_pdu(retx[i].msg, retx[i].N_bytes);
#if HAVE_PCAP
      pcap.write_dl_ccch(retx[i].msg, retx[i].N_bytes);
#endif
    }
  }

  // Step timers until reordering timeout expires
  cnt = 7;
  while (cnt--) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  TESTASSERT(rlc2.get_buffer_state());
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
#if HAVE_PCAP
  pcap.write_ul_ccch(status_buf.msg, status_buf.N_bytes);
#endif

  TESTASSERT(15 == rlc1.get_buffer_state());

  // second round of retx, reduce grant size to force different segment sizes
  byte_buffer_t retx2[20];
  for (uint32_t i = 0; i < 7; i++) {
    TESTASSERT(rlc1.get_buffer_state() != 0);
    retx2[i].N_bytes = rlc1.read_pdu(retx2[i].msg, 9);
    TESTASSERT(retx2[i].N_bytes != 0);
    rlc2.write_pdu(retx2[i].msg, retx2[i].N_bytes);
#if HAVE_PCAP
    pcap.write_dl_ccch(retx[i].msg, retx[i].N_bytes);
#endif
  }

  // get BSR from RLC2
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
#if HAVE_PCAP
  pcap.write_ul_ccch(status_buf.msg, status_buf.N_bytes);
#endif

  // check buffer states
  if (rlc1.get_buffer_state() != 0) {
    return -1;
  };
  if (rlc2.get_buffer_state() != 0) {
    return -1;
  };

  // Check number of SDUs and their content
  TESTASSERT(tester.sdus.size() == N_SDU_BUFS);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
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

// Resegmentation with 1 B segments
int resegment_test_9()
{
  // SDUs:              |  10  |  10  |  10  |
  // PDUs:              |   9 |    x  |
  // Retx PDU segments:       |2|  9  |

  const rlc_config_t config = rlc_config_t::default_rlc_am_config();
#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_resegment_test_9.pcap", config);
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, nullptr);
#endif
  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(config)) {
    return SRSRAN_ERROR;
  }

  if (not rlc2.configure(config)) {
    return SRSRAN_ERROR;
  }

  // Push 3 SDUs into RLC1
  const uint32_t       n_bufs = 3;
  unique_byte_buffer_t sdu_bufs[n_bufs];
  for (uint32_t i = 0; i < n_bufs; i++) {
    sdu_bufs[i] = srsran::make_byte_buffer();
    for (uint32_t j = 0; j < 10; j++) {
      sdu_bufs[i]->msg[j] = i;
    }
    sdu_bufs[i]->N_bytes    = 10; // Give each buffer a size of 10 bytes
    sdu_bufs[i]->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  // Read 5 PDUs from RLC1 (2 bytes, 3 bytes, 40 bytes, 3 bytes, 2 bytes)
  byte_buffer_t pdu_bufs[n_bufs];
  pdu_bufs[0].N_bytes = rlc1.read_pdu(pdu_bufs[0].msg, 11); // 2 byte header +  9 byte payload
  pdu_bufs[1].N_bytes = rlc1.read_pdu(pdu_bufs[1].msg, 15); // 4 byte header +  11 byte payload
  pdu_bufs[2].N_bytes = rlc1.read_pdu(pdu_bufs[2].msg, 12); // 2 byte header +  10 byte payload

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 0)
  for (uint32_t i = 0; i < n_bufs; i++) {
    if (i != 1) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
#if HAVE_PCAP
      pcap.write_dl_ccch(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
#endif
    }
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
#if HAVE_PCAP
  pcap.write_ul_ccch(status_buf.msg, status_buf.N_bytes);
#endif

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  byte_buffer_t retx2;
  retx1.N_bytes = rlc1.read_pdu(retx1.msg, 8); // 6 byte header + 2 data

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);
#if HAVE_PCAP
  pcap.write_dl_ccch(retx1.msg, retx1.N_bytes);
#endif

  // Read 2nd with a big enough grant to fit remaining content
  retx2.N_bytes = rlc1.read_pdu(retx2.msg, 40);

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);
#if HAVE_PCAP
  pcap.write_dl_ccch(retx2.msg, retx2.N_bytes);
#endif
  // goto exit;

  TESTASSERT(tester.sdus.size() == n_bufs);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == 10);
    for (int j = 0; j < 10; j++) {
      TESTASSERT(tester.sdus[i]->msg[j] == i);
    }
  }

  // Get status from RLC 2
  for (int i = 0; i < 5; i++) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Check final notifications
  TESTASSERT(tester.notified_counts.size() == n_bufs);
  for (uint32_t i = 0; i < n_bufs; i++) {
    auto it = tester.notified_counts.find(i);
    TESTASSERT(it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
  }

  // exit:

#if HAVE_PCAP
  pcap.close();
#endif

  return 0;
}

// Retransmission of segment Resegmentation with 1 B segments
int resegment_test_10()
{
  /// 21:35:17.369012 [RLC_1] [I] DRB1 Tx PDU SN=520 (20 B)
  ///    0000: 9e 08 80 40 0a 34 34 34 34 35 35 35 35 35 35 35
  ///    0010: 35 35 35 36
  /// 21:35:17.369016 [RLC_1] [D] [Data PDU, RF=0, P=0, FI=1, SN=520, LSF=0, SO=0, N_li=2 (4, 10, )]

  /// 21:35:17.369703 [RLC_1] [I] DRB1 Retx PDU segment SN=520 [so=0] (10 B) (attempt 2/16)
  ///    0000: fe 08 00 00 00 40 34 34 34 34
  /// 21:35:17.369712 [RLC_2] [I] DRB1 Rx data PDU segment of SN=520 (4 B), SO=0, N_li=1
  ///    0000: 34 34 34 34
  /// 21:35:17.369718 [RLC_2] [D] [Data PDU, RF=1, P=1, FI=1, SN=520, LSF=0, SO=0, N_li=1 (4, )]

  // SDUs:              |  10  |  10  |  10  |  10  |
  // PDUs:              |  6 |   25(x)        |  9  |
  // Retx PDU segments:      |4|  50                |

  const rlc_config_t config = rlc_config_t::default_rlc_am_config();
#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_resegment_test_10.pcap", config);
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif
  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(config)) {
    return SRSRAN_ERROR;
  }

  if (not rlc2.configure(config)) {
    return SRSRAN_ERROR;
  }

  // Push 3 SDUs into RLC1
  const uint32_t       n_sdus = 4;
  unique_byte_buffer_t sdu_bufs[n_sdus];
  for (uint32_t i = 0; i < n_sdus; i++) {
    sdu_bufs[i] = srsran::make_byte_buffer();
    for (uint32_t j = 0; j < 10; j++) {
      sdu_bufs[i]->msg[j] = i;
    }
    sdu_bufs[i]->N_bytes    = 10; // Give each buffer a size of 10 bytes
    sdu_bufs[i]->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  // Read 5 PDUs from RLC1 (2 bytes, 3 bytes, 40 bytes, 3 bytes, 2 bytes)
  const uint32_t n_pdus = 3;
  byte_buffer_t  pdu_bufs[n_pdus];
  pdu_bufs[0].N_bytes = rlc1.read_pdu(pdu_bufs[0].msg, 8);  // 2 byte header +  6 byte payload
  pdu_bufs[1].N_bytes = rlc1.read_pdu(pdu_bufs[1].msg, 32); // 4 byte header +  25 byte payload
  pdu_bufs[2].N_bytes = rlc1.read_pdu(pdu_bufs[2].msg, 11); // 2 byte header +  9 byte payload

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 0)
  for (uint32_t i = 0; i < n_pdus; i++) {
    if (i != 1) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
    }
#if HAVE_PCAP
    // write to PCAP even if its lost in the TC
    pcap.write_dl_ccch(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
#endif
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
#if HAVE_PCAP
  pcap.write_ul_ccch(status_buf.msg, status_buf.N_bytes);
#endif

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  byte_buffer_t retx2;
  retx1.N_bytes = rlc1.read_pdu(retx1.msg, 13); // 6 byte header + 4 data ( +2 B MAC)

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);
#if HAVE_PCAP
  pcap.write_dl_ccch(retx1.msg, retx1.N_bytes);
#endif

  // Read 2nd with a big enough grant to fit remaining content
  retx2.N_bytes = rlc1.read_pdu(retx2.msg, 32);
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);
#if HAVE_PCAP
  pcap.write_dl_ccch(retx2.msg, retx2.N_bytes);
#endif

  TESTASSERT(tester.sdus.size() == n_sdus);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == 10);
    for (int j = 0; j < 10; j++) {
      TESTASSERT(tester.sdus[i]->msg[j] == i);
    }
  }

  // Get status from RLC 2
  for (int i = 0; i < 5; i++) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Check final notifications
  TESTASSERT(tester.notified_counts.size() == n_sdus);
  for (uint32_t i = 0; i < n_sdus; i++) {
    auto it = tester.notified_counts.find(i);
    TESTASSERT(it != tester.notified_counts.end() && tester.notified_counts[i] == 1);
  }

  return SRSRAN_SUCCESS;
}

// Custom resegmentation test of a orignal PDU with N_li=2
// Because the provided MAC grant is relativly small, the retx segment
// can only accomodate 2 B of the original PDU.
// The test verifies the correct PDU packing, specifically the LI value
int resegment_test_11()
{
  /// Original PDU:
  /// 11:29:16.065008 [RLC_1] [I] DRB1 Tx PDU SN=419 (21 B)
  ///     0000: bd a3 80 50 0a aa aa aa aa aa ab ab ab ab ab ab
  ///     0010: ab ab ab ab ac
  /// 11:29:16.065013 [RLC_1] [D] [Data PDU, RF=0, P=1, FI=1, SN=419, LSF=0, SO=0, N_li=2 (5, 10, )]

  /// Log messages with the restoration bug:
  /// 11:29:16.065688 [RLC_1] [D] MAC opportunity - 10 bytes
  /// 11:29:16.065695 [RLC_1] [D] DRB1 build_retx_pdu - resegmentation required
  /// 11:29:16.065702 [RLC_1] [D] retx.so_start=2, retx.so_end=6
  /// 11:29:16.065703 [RLC_1] [D] new_header head_len=4
  /// 11:29:16.065706 [RLC_1] [D] old_header.li[0], head_len=6, pdu_space=4
  /// 11:29:16.065710 [RLC_1] [D] new_header head_len=6
  /// 11:29:16.065713 [RLC_1] [D] old_header.li[1], head_len=8, pdu_space=2
  /// 11:29:16.065716 [RLC_1] [D] DRB1 vt_a = 419, vt_ms = 931, vt_s = 426, poll_sn = 424
  /// 11:29:16.065718 [RLC_1] [I] DRB1 Retx PDU segment SN=419 [so=2] (8 B) (attempt 2/16)
  ///     0000: dd a3 00 02 00 30 aa aa
  /// 11:29:16.065723 [RLC_2] [I] DRB1 Rx data PDU segment of SN=419 (2 B), SO=2, N_li=1
  ///     0000: aa aa
  /// 11:29:16.065730 [RLC_2] [D] [Data PDU, RF=1, P=0, FI=1, SN=419, LSF=0, SO=2, N_li=1 (3, )]
  /// NOTE: this segment is malformed, it has 2 B data and a larger LI field of 3 B

  // SDUs:              |  10  |  10  |  10  |  10  |
  // PDUs:              |     15   |   16(x)  |  9  |
  // Retx PDU segments:      |4|  50                |

  const rlc_config_t config = rlc_config_t::default_rlc_am_config();
#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_resegment_test_11.pcap", config);
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif
  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(config)) {
    return SRSRAN_ERROR;
  }

  if (not rlc2.configure(config)) {
    return SRSRAN_ERROR;
  }

  // Push 4 SDUs into RLC1
  const uint32_t       n_sdus = 4;
  unique_byte_buffer_t sdu_bufs[n_sdus];
  for (uint32_t i = 0; i < n_sdus; i++) {
    sdu_bufs[i] = srsran::make_byte_buffer();
    for (uint32_t j = 0; j < 10; j++) {
      sdu_bufs[i]->msg[j] = i;
    }
    sdu_bufs[i]->N_bytes    = 10; // Give each buffer a size of 10 bytes
    sdu_bufs[i]->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  // Read 3 PDUs from RLC1 (MAC opportunities are taken from logs)
  const uint32_t n_pdus = 3;
  byte_buffer_t  pdu_bufs[n_pdus];
  pdu_bufs[0].N_bytes = rlc1.read_pdu(pdu_bufs[0].msg, 19);
  pdu_bufs[1].N_bytes = rlc1.read_pdu(pdu_bufs[1].msg, 21);
  pdu_bufs[2].N_bytes = rlc1.read_pdu(pdu_bufs[2].msg, 12);

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 1)
  for (uint32_t i = 0; i < n_pdus; i++) {
    if (i != 1) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
    }
#if HAVE_PCAP
    // write to PCAP even if its lost in the TC
    pcap.write_dl_ccch(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
#endif
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
#if HAVE_PCAP
  pcap.write_ul_ccch(status_buf.msg, status_buf.N_bytes);
#endif

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  retx1.N_bytes = rlc1.read_pdu(retx1.msg, 8);
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);
#if HAVE_PCAP
  pcap.write_dl_ccch(retx1.msg, retx1.N_bytes);
#endif

  // Read 2nd with a small grant to trigger the original segmentation bug
  byte_buffer_t retx2;
  retx2.N_bytes = rlc1.read_pdu(retx2.msg, 10);

  // Write the retx PDU to RLC2
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);
#if HAVE_PCAP
  pcap.write_dl_ccch(retx2.msg, retx2.N_bytes);
#endif

  // Read 3nd with a big enough grant to fit remaining content
  byte_buffer_t retx3;
  retx3.N_bytes = rlc1.read_pdu(retx3.msg, 20);
  rlc2.write_pdu(retx3.msg, retx3.N_bytes);
#if HAVE_PCAP
  pcap.write_dl_ccch(retx3.msg, retx3.N_bytes);
#endif

  TESTASSERT(tester.sdus.size() == n_sdus);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == 10);
    for (int j = 0; j < 10; j++) {
      TESTASSERT(tester.sdus[i]->msg[j] == i);
    }
  }

  // Get status from RLC 2
  for (int i = 0; i < 5; i++) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

#if HAVE_PCAP
  pcap.close();
#endif

  return SRSRAN_SUCCESS;
}

// Custom resegmentation test of a orignal PDU with N_li=2
// The test triggered a bug in the packing and was creating a too large
// PDU
int resegment_test_12()
{
  /// Original PDU:
  /// 17:19:51.296653 [RLC_1] [I] DRB1 Tx PDU SN=728 (21 B)
  ///  0000: be d8 80 10 0a d1 d2 d2 d2 d2 d2 d2 d2 d2 d2 d2
  ///  0010: d3 d3 d3 d3 d3
  /// 17:19:51.296659 [RLC_1] [D] [Data PDU, RF=0, P=1, FI=1, SN=728, LSF=0, SO=0, N_li=2 (1, 10, )]

  /// Log messages with the segmentation bug:
  /// 17:19:51.297485 [RLC_1] [D] MAC opportunity - 18 bytes
  /// 17:19:51.297487 [RLC_1] [D] tx_window size - 2 PDUs
  /// 17:19:51.297489 [RLC_1] [D] DRB1 build_retx_pdu - resegmentation required
  /// 17:19:51.297498 [RLC_1] [I] DRB1 pdu_without_poll: 4
  /// 17:19:51.297499 [RLC_1] [I] DRB1 byte_without_poll: 67
  /// 17:19:51.297501 [RLC_1] [D] retx.so_start=0, retx.so_end=12
  /// 17:19:51.297502 [RLC_1] [D] new_header head_len=4
  /// 17:19:51.297504 [RLC_1] [D] old_header.li[0], head_len=4, pdu_space=14
  /// 17:19:51.297505 [RLC_1] [D] new_header head_len=6
  /// 17:19:51.297506 [RLC_1] [D] old_header.li[1], head_len=6, pdu_space=12
  /// 17:19:51.297509 [RLC_1] [D] DRB1 vt_a = 724, vt_ms = 212, vt_s = 736, poll_sn = 733
  /// 17:19:51.297513 [RLC_1] [E] DRB1 Retx PDU segment length error. Available: 18, Used: 19
  /// 17:19:51.297522 [RLC_1] [D] DRB1 Retx PDU segment length error. Header len: 7, Payload len: 12, N_li: 2
  /// 17:19:51.297527 [RLC_1] [I] DRB1 Retx PDU segment SN=728 [so=0] (19 B) (attempt 2/16)
  ///     0000: de d8 00 00 80 10 0a d1 d2 d2 d2 d2 d2 d2 d2 d2
  ///     0010: d2 d2 d3
  /// 17:19:51.297531 [RLC_2] [I] DRB1 Rx data PDU segment of SN=728 (12 B), SO=0, N_li=2
  ///     0000: d1 d2 d2 d2 d2 d2 d2 d2 d2 d2 d2 d3
  /// 17:19:51.297538 [RLC_2] [D] [Data PDU, RF=1, P=0, FI=1, SN=728, LSF=0, SO=0, N_li=2 (1, 10, )]

  // SDUs:              |  10  |  10  |  10  |  10  |
  // PDUs:              |   9 |   16(x)  |  9  |
  // Retx PDU segments:      |4|  50                |

  const rlc_config_t config = rlc_config_t::default_rlc_am_config();
#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_resegment_test_12.pcap", config);
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif
  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(config)) {
    return SRSRAN_ERROR;
  }

  if (not rlc2.configure(config)) {
    return SRSRAN_ERROR;
  }

  // Push 4 SDUs into RLC1
  const uint32_t       n_sdus = 4;
  unique_byte_buffer_t sdu_bufs[n_sdus];
  for (uint32_t i = 0; i < n_sdus; i++) {
    sdu_bufs[i] = srsran::make_byte_buffer();
    for (uint32_t j = 0; j < 10; j++) {
      sdu_bufs[i]->msg[j] = i;
    }
    sdu_bufs[i]->N_bytes    = 10; // Give each buffer a size of 10 bytes
    sdu_bufs[i]->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  // Read 3 PDUs from RLC1 (MAC opportunities are taken from logs)
  const uint32_t n_pdus = 3;
  byte_buffer_t  pdu_bufs[n_pdus];
  pdu_bufs[0].N_bytes = rlc1.read_pdu(pdu_bufs[0].msg, 11);
  pdu_bufs[1].N_bytes = rlc1.read_pdu(pdu_bufs[1].msg, 21);
  pdu_bufs[2].N_bytes = rlc1.read_pdu(pdu_bufs[2].msg, 19);

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Write PDUs into RLC2 (skip SN 1)
  for (uint32_t i = 0; i < n_pdus; i++) {
    if (i != 1) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
    }
#if HAVE_PCAP
    // write to PCAP even if its lost in the TC
    pcap.write_dl_ccch(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
#endif
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
#if HAVE_PCAP
  pcap.write_ul_ccch(status_buf.msg, status_buf.N_bytes);
#endif

  // Read the retx PDU from RLC1 and force resegmentation
  byte_buffer_t retx1;
  retx1.N_bytes = rlc1.read_pdu(retx1.msg, 18);
  rlc2.write_pdu(retx1.msg, retx1.N_bytes);
#if HAVE_PCAP
  pcap.write_dl_ccch(retx1.msg, retx1.N_bytes);
#endif

  // Read 2nd to trigger the original segmentation bug
  byte_buffer_t retx2;
  retx2.N_bytes = rlc1.read_pdu(retx2.msg, 18);
  rlc2.write_pdu(retx2.msg, retx2.N_bytes);
#if HAVE_PCAP
  pcap.write_dl_ccch(retx2.msg, retx2.N_bytes);
#endif

  // Read 3nd with a big enough grant to fit remaining content
  byte_buffer_t retx3;
  retx3.N_bytes = rlc1.read_pdu(retx3.msg, 20);
  rlc2.write_pdu(retx3.msg, retx3.N_bytes);
#if HAVE_PCAP
  pcap.write_dl_ccch(retx3.msg, retx3.N_bytes);
#endif

  TESTASSERT(tester.sdus.size() == n_sdus);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == 10);
    for (int j = 0; j < 10; j++) {
      TESTASSERT(tester.sdus[i]->msg[j] == i);
    }
  }

  // Get status from RLC 2
  for (int i = 0; i < 5; i++) {
    timers.step_all();
  }

  // Read status PDU from RLC2
  status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10); // 10 bytes is enough to hold the status

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

#if HAVE_PCAP
  pcap.close();
#endif

  return SRSRAN_SUCCESS;
}

// Series of header reconstruction tests that all used canned TV generated with the rlc_stress_test
// In this particular case, check correct reconstruction of headers after 2 segment retx
int header_reconstruction_test(srsran::log_sink_message_spy& spy)
{
  /// Original SN=277 with 3 segments, including full SDU with 24
  /// 13:35:16.337011 [RLC_1] [I] DRB1 Tx PDU SN=277 (20 B)
  ///   0000: 9d 15 80 20 0a 23 23 24 24 24 24 24 24 24 24 24
  ///   0010: 24 25 25 25
  /// 13:35:16.337016 [RLC_1] [D] [Data PDU, RF=0, P=0, FI=1, SN=277, LSF=0, SO=0, N_li=2 (2, 10)]

  // 2nd retransmission with SO=9
  std::array<uint8_t, 12> tv2 = {0xdd, 0x15, 0x80, 0x09, 0x00, 0x30, 0x24, 0x24, 0x24, 0x25, 0x25, 0x25};

  // 3rd retransmission with S0=0
  std::array<uint8_t, 17> tv3 = {
      0xdd, 0x15, 0x00, 0x00, 0x00, 0x20, 0x23, 0x23, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24};

  byte_buffer_t pdu_tv2;
  memcpy(pdu_tv2.msg, tv2.data(), tv2.size());
  pdu_tv2.N_bytes = tv2.size();

  byte_buffer_t pdu_tv3;
  memcpy(pdu_tv3.msg, tv3.data(), tv3.size());
  pdu_tv3.N_bytes = tv3.size();

#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_header_reconstruction_test.pcap", rlc_config_t::default_rlc_am_config());
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif
  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

#if HAVE_PCAP
  pcap.write_dl_ccch(pdu_tv2.msg, pdu_tv2.N_bytes);
  pcap.write_dl_ccch(pdu_tv3.msg, pdu_tv3.N_bytes);
#endif

  rlc1.write_pdu(pdu_tv2.msg, pdu_tv2.N_bytes);
  rlc1.write_pdu(pdu_tv3.msg, pdu_tv3.N_bytes);

  // Check RLC re-assembled message header
  TESTASSERT(spy.has_message("[Data PDU, RF=0, P=0, FI=1, SN=277, LSF=0, SO=0, N_li=2 (2, 10)]"));

#if HAVE_PCAP
  pcap.close();
#endif

  return 0;
}

// Check correct reconstruction of headers after 3 segment retx
int header_reconstruction_test2(srsran::log_sink_message_spy& spy)
{
  /// Original SN=199 with 3 segments, including full SDU with d4
  /// 15:19:19.148272 [RLC_1] [I] DRB1 Tx PDU SN=199 (19 B)
  ///    0000: 9c c7 80 30 0a d3 d3 d3 d4 d4 d4 d4 d4 d4 d4 d4
  ///    0010: d4 d4 d5
  /// 15:19:19.148278 [RLC_1] [D] [Data PDU, RF=0, P=0, FI=1, SN=199, LSF=0, SO=0, N_li=2 (3, 10, )]

  // 2nd retransmission with SO=0
  std::array<uint8_t, 6> tv1 = {0xd8, 0xc7, 0x00, 0x00, 0xd3, 0xd3};

  // 3rd retransmission with S0=2
  std::array<uint8_t, 16> tv2 = {
      0xdc, 0xc7, 0x00, 0x02, 0x00, 0x10, 0xd3, 0xd4, 0xd4, 0xd4, 0xd4, 0xd4, 0xd4, 0xd4, 0xd4, 0xd4};

  std::array<uint8_t, 8> tv3 = {0xdc, 0xc7, 0x80, 0x0c, 0x00, 0x10, 0xd4, 0xd5};

  byte_buffer_t pdu_tv1;
  memcpy(pdu_tv1.msg, tv1.data(), tv1.size());
  pdu_tv1.N_bytes = tv1.size();

  byte_buffer_t pdu_tv2;
  memcpy(pdu_tv2.msg, tv2.data(), tv2.size());
  pdu_tv2.N_bytes = tv2.size();

  byte_buffer_t pdu_tv3;
  memcpy(pdu_tv3.msg, tv3.data(), tv3.size());
  pdu_tv3.N_bytes = tv3.size();

#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_header_reconstruction_test2.pcap", rlc_config_t::default_rlc_am_config());
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif
  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

#if HAVE_PCAP
  pcap.write_dl_ccch(pdu_tv1.msg, pdu_tv1.N_bytes);
  pcap.write_dl_ccch(pdu_tv2.msg, pdu_tv2.N_bytes);
  pcap.write_dl_ccch(pdu_tv3.msg, pdu_tv3.N_bytes);
#endif

  rlc1.write_pdu(pdu_tv1.msg, pdu_tv1.N_bytes);
  rlc1.write_pdu(pdu_tv2.msg, pdu_tv2.N_bytes);
  rlc1.write_pdu(pdu_tv3.msg, pdu_tv3.N_bytes);

  // Check RLC re-assembled message header
  TESTASSERT(spy.has_message("[Data PDU, RF=0, P=0, FI=1, SN=199, LSF=0, SO=0, N_li=2 (3, 10)]"));

#if HAVE_PCAP
  pcap.close();
#endif

  return SRSRAN_SUCCESS;
}

// TC with 3 segment retx
int header_reconstruction_test3(srsran::log_sink_message_spy& spy)
{
  // Original PDU
  // 11:13:25.994566 [RLC_1] [I] DRB1 Tx PDU SN=206 (18 B)
  //  0000: 8c ce 00 a0 db db db db db db db db db db dc dc
  //  0010: dc dc
  // 11:13:25.994571 [RLC_1] [D] [Data PDU, RF=0, P=0, FI=1, SN=206, LSF=0, SO=0, N_li=1 (10)]

  // 11:13:25.995744 [RLC_1] [I] DRB1 Retx PDU segment SN=206 [so=8] (12 B) (attempt 2/16)
  //   0000: dc ce 80 08 00 20 db db dc dc dc dc
  // 11:13:25.995752 [RLC_2] [I] DRB1 Rx data PDU segment of SN=206 (6 B), SO=8, N_li=1
  //   0000: db db dc dc dc dc
  std::array<uint8_t, 12> tv0 = {0xdc, 0xce, 0x80, 0x08, 0x00, 0x20, 0xdb, 0xdb, 0xdc, 0xdc, 0xdc, 0xdc};

  // 11:13:25.996267 [RLC_1] [I] DRB1 Retx PDU segment SN=206 [so=0] (14 B) (attempt 3/16)
  //  0000: c0 ce 00 00 db db db db db db db db db db
  // 11:13:25.996272 [RLC_2] [I] DRB1 Rx data PDU segment of SN=206 (10 B), SO=0, N_li=0
  //  0000: db db db db db db db db db db
  std::array<uint8_t, 14> tv1 = {0xc0, 0xce, 0x00, 0x00, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb};

  byte_buffer_t pdu_tv0;
  memcpy(pdu_tv0.msg, tv0.data(), tv0.size());
  pdu_tv0.N_bytes = tv0.size();

  byte_buffer_t pdu_tv1;
  memcpy(pdu_tv1.msg, tv1.data(), tv1.size());
  pdu_tv1.N_bytes = tv1.size();

#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_header_reconstruction_test3.pcap", rlc_config_t::default_rlc_am_config());
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif
  srsran::timer_handler timers(8);

  // configure RLC
  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

#if HAVE_PCAP
  pcap.write_dl_ccch(pdu_tv0.msg, pdu_tv0.N_bytes);
  pcap.write_dl_ccch(pdu_tv1.msg, pdu_tv1.N_bytes);
#endif

  rlc1.write_pdu(pdu_tv0.msg, pdu_tv0.N_bytes);
  rlc1.write_pdu(pdu_tv1.msg, pdu_tv1.N_bytes);

  // Check RLC re-assembled message header
  TESTASSERT(spy.has_message("[Data PDU, RF=0, P=0, FI=1, SN=206, LSF=0, SO=0, N_li=1 (10)]"));

#if HAVE_PCAP
  pcap.close();
#endif

  return SRSRAN_SUCCESS;
}

int header_reconstruction_test4(srsran::log_sink_message_spy& spy)
{
  // Original PDU
  // 15:32:20.667043 [RLC_1] [I] DRB1 Tx PDU SN=172 (22 B)
  //   0000: 9c ac 80 10 0a af b0 b0 b0 b0 b0 b0 b0 b0 b0 b0
  //   0010: b1 b1 b1 b1 b1 b1
  // 15:32:20.667048 [RLC_1] [D] [Data PDU, RF=0, P=0, FI=1, SN=172, LSF=0, SO=0, N_li=2 (1, 10)]

  // 15:32:20.668094 [RLC_1] [I] DRB1 Retx PDU segment SN=172 [so=0] (14 B) (attempt 2/16)
  //  0000: dc ac 00 00 00 10 af b0 b0 b0 b0 b0 b0 b0
  // 15:32:20.668100 [RLC_2] [I] DRB1 Rx data PDU segment of SN=172 (8 B), SO=0, N_li=1
  //  0000: af b0 b0 b0 b0 b0 b0 b0
  // 15:32:20.668105 [RLC_2] [D] [Data PDU, RF=1, P=0, FI=1, SN=172, LSF=0, SO=0, N_li=1 (1)]
  std::array<uint8_t, 14> tv1 = {0xdc, 0xac, 0x00, 0x00, 0x00, 0x10, 0xaf, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0};

  // 15:32:20.668497 [RLC_1] [I] DRB1 Retx PDU segment SN=172 [so=0] (12 B) (attempt 3/16)
  //  0000: fc ac 00 00 00 10 af b0 b0 b0 b0 b0
  // 15:32:20.668502 [RLC_2] [I] DRB1 Rx data PDU segment of SN=172 (6 B), SO=0, N_li=1
  //  0000: af b0 b0 b0 b0 b0
  // 15:32:20.668507 [RLC_2] [D] [Data PDU, RF=1, P=1, FI=1, SN=172, LSF=0, SO=0, N_li=1 (1)]
  std::array<uint8_t, 12> tv2 = {0xfc, 0xac, 0x00, 0x00, 0x00, 0x10, 0xaf, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0};

  // 15:32:20.668575 [RLC_1] [I] DRB1 Retx PDU segment SN=172 [so=6] (7 B) (attempt 3/16)
  //  0000: d8 ac 00 06 b0 b0 b0
  // 15:32:20.668581 [RLC_1] [I] DRB1 Tx SDU (10 B, tx_sdu_queue_len=33)
  //  0000: d8 d8 d8 d8 d8 d8 d8 d8 d8 d8
  // 15:32:20.668582 [RLC_2] [I] DRB1 Rx data PDU segment of SN=172 (3 B), SO=6, N_li=0
  //  0000: b0 b0 b0
  std::array<uint8_t, 7> tv3 = {0xd8, 0xac, 0x00, 0x06, 0xb0, 0xb0, 0xb0};

  // 15:32:20.668665 [RLC_1] [I] DRB1 Retx PDU segment SN=172 [so=9] (14 B) (attempt 3/16)
  //  0000: dc ac 80 09 00 20 b0 b0 b1 b1 b1 b1 b1 b1
  // 15:32:20.668671 [RLC_2] [I] DRB1 Rx data PDU segment of SN=172 (8 B), SO=9, N_li=1
  //  0000: b0 b0 b1 b1 b1 b1 b1 b1
  // 15:32:20.668675 [RLC_2] [D] [Data PDU, RF=1, P=0, FI=1, SN=172, LSF=1, SO=9, N_li=1 (2)]
  std::array<uint8_t, 14> tv4 = {0xdc, 0xac, 0x80, 0x09, 0x00, 0x20, 0xb0, 0xb0, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1};

  byte_buffer_t pdu_tv1;
  memcpy(pdu_tv1.msg, tv1.data(), tv1.size());
  pdu_tv1.N_bytes = tv1.size();

  byte_buffer_t pdu_tv2;
  memcpy(pdu_tv2.msg, tv2.data(), tv2.size());
  pdu_tv2.N_bytes = tv2.size();

  byte_buffer_t pdu_tv3;
  memcpy(pdu_tv3.msg, tv3.data(), tv3.size());
  pdu_tv3.N_bytes = tv3.size();

  byte_buffer_t pdu_tv4;
  memcpy(pdu_tv4.msg, tv4.data(), tv4.size());
  pdu_tv4.N_bytes = tv4.size();

#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_header_reconstruction_test4.pcap", rlc_config_t::default_rlc_am_config());
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif
  srsran::timer_handler timers(8);

  // configure RLC
  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

#if HAVE_PCAP
  pcap.write_dl_ccch(pdu_tv1.msg, pdu_tv1.N_bytes);
  pcap.write_dl_ccch(pdu_tv2.msg, pdu_tv2.N_bytes);
  pcap.write_dl_ccch(pdu_tv3.msg, pdu_tv3.N_bytes);
  pcap.write_dl_ccch(pdu_tv4.msg, pdu_tv4.N_bytes);
#endif

  rlc1.write_pdu(pdu_tv1.msg, pdu_tv1.N_bytes);
  rlc1.write_pdu(pdu_tv2.msg, pdu_tv2.N_bytes);
  rlc1.write_pdu(pdu_tv3.msg, pdu_tv3.N_bytes);
  rlc1.write_pdu(pdu_tv4.msg, pdu_tv4.N_bytes);

  // Check RLC re-assembled message header
  TESTASSERT(spy.has_message("[Data PDU, RF=0, P=0, FI=1, SN=172, LSF=0, SO=0, N_li=2 (1, 10)]"));

#if HAVE_PCAP
  pcap.close();
#endif

  return SRSRAN_SUCCESS;
}

int header_reconstruction_test5(srsran::log_sink_message_spy& spy)
{
  // Original PDU:
  // 18:46:22.372858 [RLC_1] [I] DRB1 Tx PDU SN=222 (22 B)
  //   0000: bc de 80 30 0a ee ee ee ef ef ef ef ef ef ef ef
  //   0010: ef ef f0 f0 f0 f0
  // 18:46:22.372863 [RLC_1] [D] [Data PDU, RF=0, P=1, FI=1, SN=222, LSF=0, SO=0, N_li=2 (3, 10)]

  // 18:46:22.373623 [RLC_1] [I] DRB1 Retx PDU segment SN=222 [so=0] (7 B) (attempt 2/16)
  //  0000: d0 de 00 00 ee ee ee
  // 18:46:22.373629 [RLC_2] [I] DRB1 Rx data PDU segment of SN=222 (3 B), SO=0, N_li=0
  //  0000: ee ee ee
  std::array<uint8_t, 7> tv0 = {0xd0, 0xde, 0x00, 0x00, 0xee, 0xee, 0xee};

  // 18:46:22.373707 [RLC_1] [I] DRB1 Retx PDU segment SN=222 [so=3] (19 B) (attempt 2/16)
  //   0000: cc de 00 03 00 a0 ef ef ef ef ef ef ef ef ef ef
  //   0010: f0 f0 f0
  // 18:46:22.373714 [RLC_2] [I] DRB1 Rx data PDU segment of SN=222 (13 B), SO=3, N_li=1
  //   0000: ef ef ef ef ef ef ef ef ef ef f0 f0 f0
  std::array<uint8_t, 19> tv1 = {
      0xcc, 0xde, 0x00, 0x03, 0x00, 0xa0, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xf0, 0xf0, 0xf0};

  // 18:46:22.373793 [RLC_1] [I] DRB1 Retx PDU segment SN=222 [so=16] (5 B) (attempt 2/16)
  //  0000: d8 de 80 10 f0
  // 18:46:22.373798 [RLC_2] [I] DRB1 Rx data PDU segment of SN=222 (1 B), SO=16, N_li=0
  //  0000: f0
  std::array<uint8_t, 5> tv2 = {0xd8, 0xde, 0x80, 0x10, 0xf0};

  byte_buffer_t pdu_tv0;
  memcpy(pdu_tv0.msg, tv0.data(), tv0.size());
  pdu_tv0.N_bytes = tv0.size();

  byte_buffer_t pdu_tv1;
  memcpy(pdu_tv1.msg, tv1.data(), tv1.size());
  pdu_tv1.N_bytes = tv1.size();

  byte_buffer_t pdu_tv2;
  memcpy(pdu_tv2.msg, tv2.data(), tv2.size());
  pdu_tv2.N_bytes = tv2.size();

#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_header_reconstruction_test5.pcap", rlc_config_t::default_rlc_am_config());
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif
  srsran::timer_handler timers(8);

  // configure RLC
  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

#if HAVE_PCAP
  pcap.write_dl_ccch(pdu_tv0.msg, pdu_tv0.N_bytes);
  pcap.write_dl_ccch(pdu_tv1.msg, pdu_tv1.N_bytes);
  pcap.write_dl_ccch(pdu_tv2.msg, pdu_tv2.N_bytes);
#endif

  // don't write original PDU
  rlc1.write_pdu(pdu_tv0.msg, pdu_tv0.N_bytes);
  rlc1.write_pdu(pdu_tv1.msg, pdu_tv1.N_bytes);
  rlc1.write_pdu(pdu_tv2.msg, pdu_tv2.N_bytes);

  // Check RLC re-assembled message header
  TESTASSERT(spy.has_message("[Data PDU, RF=0, P=0, FI=1, SN=222, LSF=0, SO=0, N_li=2 (3, 10)]"));

#if HAVE_PCAP
  pcap.close();
#endif

  return SRSRAN_SUCCESS;
}

int header_reconstruction_test6(srsran::log_sink_message_spy& spy)
{
  // Original PDU:
  // 21:50:12.709646 [RLC_1] [I] DRB1 Tx PDU SN=509 (20 B)
  //  0000: 9d fd 80 40 0a b1 b1 b1 b1 b2 b2 b2 b2 b2 b2 b2
  //  0010: b2 b2 b2 b3
  // 21:50:12.709653 [RLC_1] [D] [Data PDU, RF=0, P=0, FI=1, SN=509, LSF=0, SO=0, N_li=2 (4, 10)]]

  // 21:50:12.711022 [RLC_1] [I] DRB1 Retx PDU segment SN=509 [so=0] (5 B) (attempt 3/16)
  //   0000: d9 fd 00 00 b1
  // 21:50:12.711029 [RLC_2] [I] DRB1 Rx data PDU segment of SN=509 (1 B), SO=0, N_li=0
  //   0000: b1
  // 21:50:12.711034 [RLC_2] [D] [Data PDU, RF=1, P=0, FI=1, SN=509, LSF=0, SO=0, N_li=0]
  std::array<uint8_t, 5> tv0 = {0xd9, 0xfd, 0x00, 0x00, 0xb1};

  // 21:50:12.711104 [RLC_1] [I] DRB1 Retx PDU segment SN=509 [so=1] (7 B) (attempt 3/16)
  //  0000: d1 fd 00 01 b1 b1 b1
  // 21:50:12.711110 [RLC_2] [I] DRB1 Rx data PDU segment of SN=509 (3 B), SO=1, N_li=0
  //  0000: b1 b1 b1
  // 21:50:12.711115 [RLC_2] [D] [Data PDU, RF=1, P=0, FI=1, SN=509, LSF=0, SO=1, N_li=0]
  std::array<uint8_t, 7> tv1 = {0xd1, 0xfd, 0x00, 0x01, 0xb1, 0xb1, 0xb1};

  // 21:50:12.711201 [RLC_1] [I] DRB1 Retx PDU segment SN=509 [so=4] (17 B) (attempt 3/16)
  //  0000: ed fd 80 04 00 a0 b2 b2 b2 b2 b2 b2 b2 b2 b2 b2
  //  0010: b3
  // 21:50:12.711210 [RLC_2] [I] DRB1 Rx data PDU segment of SN=509 (11 B), SO=4, N_li=1
  //  0000: b2 b2 b2 b2 b2 b2 b2 b2 b2 b2 b3
  // 21:50:12.711216 [RLC_2] [D] [Data PDU, RF=1, P=1, FI=1, SN=509, LSF=1, SO=4, N_li=1 (10)]
  std::array<uint8_t, 17> tv2 = {
      0xed, 0xfd, 0x80, 0x04, 0x00, 0xa0, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb3};

  byte_buffer_t pdu_tv0;
  memcpy(pdu_tv0.msg, tv0.data(), tv0.size());
  pdu_tv0.N_bytes = tv0.size();

  byte_buffer_t pdu_tv1;
  memcpy(pdu_tv1.msg, tv1.data(), tv1.size());
  pdu_tv1.N_bytes = tv1.size();

  byte_buffer_t pdu_tv2;
  memcpy(pdu_tv2.msg, tv2.data(), tv2.size());
  pdu_tv2.N_bytes = tv2.size();

#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_header_reconstruction_test6.pcap", rlc_config_t::default_rlc_am_config());
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif
  srsran::timer_handler timers(8);

  // configure RLC
  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

#if HAVE_PCAP
  pcap.write_dl_ccch(pdu_tv0.msg, pdu_tv0.N_bytes);
  pcap.write_dl_ccch(pdu_tv1.msg, pdu_tv1.N_bytes);
  pcap.write_dl_ccch(pdu_tv2.msg, pdu_tv2.N_bytes);
#endif

  // don't write original PDU
  rlc1.write_pdu(pdu_tv0.msg, pdu_tv0.N_bytes);
  rlc1.write_pdu(pdu_tv1.msg, pdu_tv1.N_bytes);
  rlc1.write_pdu(pdu_tv2.msg, pdu_tv2.N_bytes);

  // Check RLC re-assembled message header
  TESTASSERT(spy.has_message("[Data PDU, RF=0, P=1, FI=1, SN=509, LSF=0, SO=0, N_li=2 (4, 10)]"));

#if HAVE_PCAP
  pcap.close();
#endif

  return SRSRAN_SUCCESS;
}

int header_reconstruction_test7(srsran::log_sink_message_spy& spy)
{
  // Original PDU:
  // 22:14:54.646530 [RLC_1] [I] DRB1 Tx PDU SN=282 (19 B)
  //  0000: 9d 1a 80 10 0a 28 29 29 29 29 29 29 29 29 29 29
  //  0010: 2a 2a 2a
  // 22:14:54.646535 [RLC_1] [D] [Data PDU, RF=0, P=0, FI=1, SN=282, LSF=0, SO=0, N_li=2 (1, 10)]

  // 22:14:54.648484 [RLC_1] [I] DRB1 Retx PDU segment SN=282 [so=2] (6 B) (attempt 2/16)
  //  0000: f9 1a 00 02 29 29
  // 22:14:54.648490 [RLC_2] [I] DRB1 Rx data PDU segment of SN=282 (2 B), SO=2, N_li=0
  //  0000: 29 29
  // 22:14:54.648495 [RLC_2] [D] [Data PDU, RF=1, P=1, FI=1, SN=282, LSF=0, SO=2, N_li=0]
  std::array<uint8_t, 6> tv0 = {0xf9, 0x1a, 0x00, 0x02, 0x29, 0x29};

  // 22:14:54.648576 [RLC_1] [I] DRB1 Retx PDU segment SN=282 [so=4] (11 B) (attempt 2/16)
  //  0000: d1 1a 00 04 29 29 29 29 29 29 29
  // 22:14:54.648583 [RLC_2] [I] DRB1 Rx data PDU segment of SN=282 (7 B), SO=4, N_li=0
  //  0000: 29 29 29 29 29 29 29
  // 22:14:54.648588 [RLC_2] [D] [Data PDU, RF=1, P=0, FI=1, SN=282, LSF=0, SO=4, N_li=0]
  std::array<uint8_t, 11> tv1 = {0xd1, 0x1a, 0x00, 0x04, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29};

  // 22:14:54.648701 [RLC_1] [I] DRB1 Retx PDU segment SN=282 [so=11] (7 B) (attempt 2/16)
  //  0000: d9 1a 80 0b 2a 2a 2a
  // 22:14:54.648707 [RLC_2] [I] DRB1 Rx data PDU segment of SN=282 (3 B), SO=11, N_li=0
  //  0000: 2a 2a 2a
  // 22:14:54.648713 [RLC_2] [D] [Data PDU, RF=1, P=0, FI=1, SN=282, LSF=1, SO=11, N_li=0]
  std::array<uint8_t, 7> tv2 = {0xd9, 0x1a, 0x80, 0x0b, 0x2a, 0x2a, 0x2a};

  // 22:14:54.648860 [RLC_1] [I] DRB1 Retx PDU segment SN=282 [so=0] (5 B) (attempt 3/16)
  //  0000: d1 1a 00 00 28
  // 22:14:54.648866 [RLC_2] [I] DRB1 Rx data PDU segment of SN=282 (1 B), SO=0, N_li=0
  //  0000: 28
  // 22:14:54.648871 [RLC_2] [D] [Data PDU, RF=1, P=0, FI=1, SN=282, LSF=0, SO=0, N_li=0]
  std::array<uint8_t, 5> tv3 = {0xd1, 0x1a, 0x00, 0x00, 0x28};

  // 22:14:54.648948 [RLC_1] [I] DRB1 Retx PDU segment SN=282 [so=1] (8 B) (attempt 3/16)
  //  0000: c9 1a 00 01 29 29 29 29
  // 22:14:54.648957 [RLC_2] [I] DRB1 Rx data PDU segment of SN=282 (4 B), SO=1, N_li=0
  //  0000: 29 29 29 29
  // 22:14:54.648962 [RLC_2] [D] [Data PDU, RF=1, P=0, FI=1, SN=282, LSF=0, SO=1, N_li=0]
  std::array<uint8_t, 8> tv4 = {0xc9, 0x1a, 0x00, 0x01, 0x29, 0x29, 0x29, 0x29};

  byte_buffer_t pdu_tv0;
  memcpy(pdu_tv0.msg, tv0.data(), tv0.size());
  pdu_tv0.N_bytes = tv0.size();

  byte_buffer_t pdu_tv1;
  memcpy(pdu_tv1.msg, tv1.data(), tv1.size());
  pdu_tv1.N_bytes = tv1.size();

  byte_buffer_t pdu_tv2;
  memcpy(pdu_tv2.msg, tv2.data(), tv2.size());
  pdu_tv2.N_bytes = tv2.size();

  byte_buffer_t pdu_tv3;
  memcpy(pdu_tv3.msg, tv3.data(), tv3.size());
  pdu_tv3.N_bytes = tv3.size();

  byte_buffer_t pdu_tv4;
  memcpy(pdu_tv4.msg, tv4.data(), tv4.size());
  pdu_tv4.N_bytes = tv4.size();

#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_header_reconstruction_test7.pcap", rlc_config_t::default_rlc_am_config());
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif
  srsran::timer_handler timers(8);

  // configure RLC
  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

#if HAVE_PCAP
  pcap.write_dl_ccch(pdu_tv0.msg, pdu_tv0.N_bytes);
  pcap.write_dl_ccch(pdu_tv1.msg, pdu_tv1.N_bytes);
  pcap.write_dl_ccch(pdu_tv2.msg, pdu_tv2.N_bytes);
  pcap.write_dl_ccch(pdu_tv3.msg, pdu_tv3.N_bytes);
  pcap.write_dl_ccch(pdu_tv4.msg, pdu_tv4.N_bytes);
#endif

  // don't write original PDU
  rlc1.write_pdu(pdu_tv0.msg, pdu_tv0.N_bytes);
  rlc1.write_pdu(pdu_tv1.msg, pdu_tv1.N_bytes);
  rlc1.write_pdu(pdu_tv2.msg, pdu_tv2.N_bytes);
  rlc1.write_pdu(pdu_tv3.msg, pdu_tv3.N_bytes);
  rlc1.write_pdu(pdu_tv4.msg, pdu_tv4.N_bytes);

  // Check RLC re-assembled message header
  TESTASSERT(spy.has_message("[Data PDU, RF=0, P=0, FI=1, SN=282, LSF=0, SO=0, N_li=2 (1, 10)]"));

#if HAVE_PCAP
  pcap.close();
#endif

  return SRSRAN_SUCCESS;
}

int header_reconstruction_test8(srsran::log_sink_message_spy& spy)
{
  // Original PDU:
  // 21:23:34.407718 [RLC_1] [I] DRB1 Tx PDU SN=423 (40 B)
  //  0000: b5 a7 80 38 0a 00 a0 77 77 77 78 78 78 78 78 78
  //  0010: 78 78 78 78 79 79 79 79 79 79 79 79 79 79 7a 7a
  //  0020: 7a 7a 7a 7a 7a 7a 7a 7a
  // 21:23:34.407724 [RLC_1] [D] [Data PDU, RF=0, P=1, FI=1, SN=423, LSF=0, SO=0, N_li=3 (3, 10, 10)]

  // 21:23:34.408815 [RLC_1] [I] DRB1 Retx PDU segment SN=423 [so=0] (18 B) (attempt 2/8)
  //  0000: fd a7 00 00 00 30 77 77 77 78 78 78 78 78 78 78
  //  0010: 78 78
  // 21:23:34.408822 [RLC_2] [I] DRB1 Rx data PDU segment of SN=423 (12 B), SO=0, N_li=1
  //  0000: 77 77 77 78 78 78 78 78 78 78 78 78
  // 21:23:34.408828 [RLC_2] [D] [Data PDU, RF=1, P=1, FI=1, SN=423, LSF=0, SO=0, N_li=1 (3)]
  std::array<uint8_t, 18> tv0 = {
      0xfd, 0xa7, 0x00, 0x00, 0x00, 0x30, 0x77, 0x77, 0x77, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78};

  // 21:23:34.408913 [RLC_1] [I] DRB1 Retx PDU segment SN=423 [so=12] (17 B) (attempt 2/8)
  //  0000: f5 a7 00 0c 00 10 78 79 79 79 79 79 79 79 79 79
  //  0010: 79
  // 21:23:34.408919 [RLC_2] [I] DRB1 Rx data PDU segment of SN=423 (11 B), SO=12, N_li=1
  //  0000: 78 79 79 79 79 79 79 79 79 79 79
  // 21:23:34.408925 [RLC_2] [D] [Data PDU, RF=1, P=1, FI=1, SN=423, LSF=0, SO=12, N_li=1 (1)]
  std::array<uint8_t, 17> tv1 = {
      0xf5, 0xa7, 0x00, 0x0c, 0x00, 0x10, 0x78, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79};

  // 21:23:34.409421 [RLC_1] [I] DRB1 Retx PDU segment SN=423 [so=0] (19 B) (attempt 3/8)
  //  0000: f5 a7 00 00 00 30 77 77 77 78 78 78 78 78 78 78
  //  0010: 78 78 78
  // 21:23:34.409433 [RLC_2] [I] DRB1 Rx data PDU segment of SN=423 (13 B), SO=0, N_li=1
  //  0000: 77 77 77 78 78 78 78 78 78 78 78 78 78
  // 21:23:34.409440 [RLC_2] [D] [Data PDU, RF=1, P=1, FI=1, SN=423, LSF=0, SO=0, N_li=1 (3)]
  std::array<uint8_t, 19> tv2 = {
      0xf5, 0xa7, 0x00, 0x00, 0x00, 0x30, 0x77, 0x77, 0x77, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78};

  // 21:23:34.409524 [RLC_1] [I] DRB1 Retx PDU segment SN=423 [so=13] (26 B) (attempt 3/8)
  //  0000: e5 a7 80 0d 00 a0 79 79 79 79 79 79 79 79 79 79
  //  0010: 7a 7a 7a 7a 7a 7a 7a 7a 7a 7a
  // 21:23:34.409531 [RLC_2] [I] DRB1 Rx data PDU segment of SN=423 (20 B), SO=13, N_li=1
  //  0000: 79 79 79 79 79 79 79 79 79 79 7a 7a 7a 7a 7a 7a
  //  0010: 7a 7a 7a 7a
  // 21:23:34.409537 [RLC_2] [D] [Data PDU, RF=1, P=1, FI=0, SN=423, LSF=1, SO=13, N_li=1 (10)]
  std::array<uint8_t, 26> tv3 = {0xe5, 0xa7, 0x80, 0x0d, 0x00, 0xa0, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79,
                                 0x79, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a};

  byte_buffer_t pdu_tv0;
  memcpy(pdu_tv0.msg, tv0.data(), tv0.size());
  pdu_tv0.N_bytes = tv0.size();

  byte_buffer_t pdu_tv1;
  memcpy(pdu_tv1.msg, tv1.data(), tv1.size());
  pdu_tv1.N_bytes = tv1.size();

  byte_buffer_t pdu_tv2;
  memcpy(pdu_tv2.msg, tv2.data(), tv2.size());
  pdu_tv2.N_bytes = tv2.size();

  byte_buffer_t pdu_tv3;
  memcpy(pdu_tv3.msg, tv3.data(), tv3.size());
  pdu_tv3.N_bytes = tv3.size();

#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_header_reconstruction_test8.pcap", rlc_config_t::default_rlc_am_config());
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif
  srsran::timer_handler timers(8);

  // configure RLC
  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

#if HAVE_PCAP
  pcap.write_dl_ccch(pdu_tv0.msg, pdu_tv0.N_bytes);
  pcap.write_dl_ccch(pdu_tv1.msg, pdu_tv1.N_bytes);
  pcap.write_dl_ccch(pdu_tv2.msg, pdu_tv2.N_bytes);
  pcap.write_dl_ccch(pdu_tv3.msg, pdu_tv3.N_bytes);
  pcap.close();
#endif

  // don't write original PDU
  rlc1.write_pdu(pdu_tv0.msg, pdu_tv0.N_bytes);
  rlc1.write_pdu(pdu_tv1.msg, pdu_tv1.N_bytes);
  rlc1.write_pdu(pdu_tv2.msg, pdu_tv2.N_bytes);
  rlc1.write_pdu(pdu_tv3.msg, pdu_tv3.N_bytes);

  // Check RLC re-assembled message header
  TESTASSERT(spy.has_message("[Data PDU, RF=0, P=1, FI=1, SN=423, LSF=0, SO=0, N_li=3 (3, 10, 10)]"));

  return SRSRAN_SUCCESS;
}

bool reset_test()
{
  rlc_am_tester         tester(true, nullptr);
  srsran::timer_handler timers(8);
  int                   len = 0;

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 1 SDU of size 10 into RLC1
  unique_byte_buffer_t sdu_buf = srsran::make_byte_buffer();
  sdu_buf->N_bytes             = 100;
  std::fill(sdu_buf->msg, sdu_buf->msg + sdu_buf->N_bytes, 0);
  sdu_buf->msg[0] = 1; // Write the index into the buffer
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
  rlc_am_tester         tester(true, nullptr);
  srsran::timer_handler timers(8);
  int                   len = 0;

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 1 SDU of size 10 into RLC1
  unique_byte_buffer_t sdu_buf = srsran::make_byte_buffer();
  sdu_buf->N_bytes             = 100;
  std::fill(sdu_buf->msg, sdu_buf->msg + sdu_buf->N_bytes, 0);
  sdu_buf->msg[0] = 1; // Write the index into the buffer
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
  rlc_am_tester         tester(true, nullptr);
  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);

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
  rlc_am_tester         tester(true, nullptr);
  srsran::timer_handler timers(8);
  int                   len = 0;

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  const uint32_t       n_sdus = 10;
  unique_byte_buffer_t sdu_bufs[n_sdus];
  for (uint32_t i = 0; i < n_sdus; i++) {
    sdu_bufs[i]          = srsran::make_byte_buffer();
    sdu_bufs[i]->N_bytes = 1; // Give each buffer a size of 1 byte
    sdu_bufs[i]->msg[0]  = i; // Write the index into the buffer
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  // Read 5 PDUs from RLC1 (1 byte each)
  const uint32_t n_pdus = n_sdus;
  byte_buffer_t  pdu_bufs[n_pdus];
  for (uint32_t i = 0; i < n_pdus; i++) {
    len                 = rlc1.read_pdu(pdu_bufs[i].msg, 3); // 2 byte header + 1 byte payload
    pdu_bufs[i].N_bytes = len;
  }

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Only pass 1st and last PDUs to RLC2
  for (uint32_t i = 0; i < n_pdus; ++i) {
    if (i == 0 || i == n_pdus - 1) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
    }
  }

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  uint32_t buffer_state = rlc2.get_buffer_state();

  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  len                = rlc2.read_pdu(status_buf.msg, 5); // provide only small grant
  status_buf.N_bytes = len;

  // check status PDU doesn't contain ACK_SN in NACK list
  rlc_status_pdu_t status_pdu = {};
  rlc_am_read_status_pdu(status_buf.msg, status_buf.N_bytes, &status_pdu);
  TESTASSERT(rlc_am_is_valid_status_pdu(status_pdu));

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

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
  len                = rlc2.read_pdu(status_buf.msg, 20); // big enough grant to fit full status PDU
  status_buf.N_bytes = len;
  TESTASSERT(status_buf.N_bytes != 0);

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // retransmission of remaining PDUs
  for (int i = 0; i < 10; i++) {
    retx.clear();
    len          = rlc1.read_pdu(retx.msg, 3);
    retx.N_bytes = len;

    // Write the retx PDU to RLC2
    rlc2.write_pdu(retx.msg, retx.N_bytes);
  }

  TESTASSERT(tester.sdus.size() == n_sdus);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == 1);
  }

  return SRSRAN_SUCCESS;
}

// This test checks the correct handling of a sending RLC entity when an incorrect status PDU is injected.
// In this test, the receiver requests the retransmission of a SN that he has acknowledeged before.
// The incidence is reported to the upper layers.
bool incorrect_status_pdu_test()
{
  rlc_am_tester         tester(true, nullptr);
  srsran::timer_handler timers(8);
  int                   len = 0;

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  const uint32_t       n_sdus = 10;
  unique_byte_buffer_t sdu_bufs[n_sdus];
  for (uint32_t i = 0; i < n_sdus; i++) {
    sdu_bufs[i]          = srsran::make_byte_buffer();
    sdu_bufs[i]->N_bytes = 1; // Give each buffer a size of 1 byte
    sdu_bufs[i]->msg[0]  = i; // Write the index into the buffer
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  // Read 5 PDUs from RLC1 (1 byte each)
  const uint32_t n_pdus = n_sdus;
  byte_buffer_t  pdu_bufs[n_pdus];
  for (uint32_t i = 0; i < n_pdus; i++) {
    len                 = rlc1.read_pdu(pdu_bufs[i].msg, 3); // 2 byte header + 1 byte payload
    pdu_bufs[i].N_bytes = len;
  }

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Construct a status PDU that ACKs SN 1
  rlc_status_pdu_t status_pdu = {};
  status_pdu.ack_sn           = 4;
  status_pdu.N_nack           = 3;
  status_pdu.nacks[0].nack_sn = 0;
  status_pdu.nacks[1].nack_sn = 2;
  TESTASSERT(rlc_am_is_valid_status_pdu(status_pdu));

  // pack PDU and write to RLC
  byte_buffer_t status_buf;
  rlc_am_write_status_pdu(&status_pdu, &status_buf);
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // This will remove SN=1 from the Tx window

  TESTASSERT(tester.protocol_failure_triggered == false);

  // construct a valid but conflicting status PDU that request SN=1 for retx
  status_pdu.N_nack           = 1;
  status_pdu.nacks[0].nack_sn = 1;
  TESTASSERT(rlc_am_is_valid_status_pdu(status_pdu));

  // pack and write to RLC again
  rlc_am_write_status_pdu(&status_pdu, &status_buf);
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  TESTASSERT(tester.protocol_failure_triggered == true);
  return SRSRAN_SUCCESS;
}

/// The test checks the correct detection of an out-of-order status PDUs
/// In contrast to the without explicitly NACK-ing specific SNs
bool incorrect_status_pdu_test2()
{
  rlc_am_tester         tester(true, nullptr);
  srsran::timer_handler timers(8);
  int                   len = 0;

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }
  if (not rlc2.configure(rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Push 10 SDUs into RLC1
  const uint32_t       n_sdus = 10;
  unique_byte_buffer_t sdu_bufs[n_sdus];
  for (uint32_t i = 0; i < n_sdus; i++) {
    sdu_bufs[i]          = srsran::make_byte_buffer();
    sdu_bufs[i]->N_bytes = 1; // Give each buffer a size of 1 byte
    sdu_bufs[i]->msg[0]  = i; // Write the index into the buffer
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  // Read 10 PDUs from RLC1 (1 byte each) and push half of them to RLC2
  const uint32_t n_pdus = n_sdus;
  byte_buffer_t  pdu_bufs[n_pdus];
  for (uint32_t i = 0; i < n_pdus; i++) {
    len                 = rlc1.read_pdu(pdu_bufs[i].msg, 3); // 2 byte header + 1 byte payload
    pdu_bufs[i].N_bytes = len;
    if (i < 5) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
    }
  }

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Construct a status PDU that ACKs all SNs
  rlc_status_pdu_t status_pdu = {};
  status_pdu.ack_sn           = 5;
  status_pdu.N_nack           = 0;
  TESTASSERT(rlc_am_is_valid_status_pdu(status_pdu));

  // pack PDU and write to RLC
  byte_buffer_t status_buf;
  rlc_am_write_status_pdu(&status_pdu, &status_buf);
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  TESTASSERT(tester.protocol_failure_triggered == false);

  // construct a valid but conflicting status PDU that acks a lower SN and requests SN=1 for retx
  status_pdu.ack_sn           = 3;
  status_pdu.N_nack           = 1;
  status_pdu.nacks[0].nack_sn = 1;
  TESTASSERT(rlc_am_is_valid_status_pdu(status_pdu));

  // pack and write to RLC again
  rlc_am_write_status_pdu(&status_pdu, &status_buf);
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // the PDU should be dropped

  // resend first Status PDU again
  status_pdu.ack_sn = 5;
  status_pdu.N_nack = 0;
  TESTASSERT(rlc_am_is_valid_status_pdu(status_pdu));

  // pack and write to RLC again
  rlc_am_write_status_pdu(&status_pdu, &status_buf);
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Step timers until reordering timeout expires
  int cnt = 5;
  while (cnt--) {
    timers.step_all();
  }

  // retransmit all outstanding PDUs
  for (int i = 0; i < 5; i++) {
    byte_buffer_t retx;
    retx.N_bytes = rlc1.read_pdu(retx.msg, 3);
    rlc2.write_pdu(retx.msg, retx.N_bytes);

    // Step timers until reordering timeout expires
    int cnt = 5;
    while (cnt--) {
      timers.step_all();
    }

    // read status
    byte_buffer_t status_buf;
    status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 10);
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
  }

  TESTASSERT(tester.sdus.size() == n_sdus);
  for (uint32_t i = 0; i < tester.sdus.size(); i++) {
    TESTASSERT(tester.sdus[i]->N_bytes == 1);
  }

  return SRSRAN_SUCCESS;
}

// This test checks the correct functioning of RLC reestablishment
// after maxRetx attempt.
bool reestablish_test()
{
  const rlc_config_t config = rlc_config_t::default_rlc_am_config();
#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_reestablish_test.pcap", config);
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, nullptr);
#endif

  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  srslog::fetch_basic_logger("RLC_AM_1").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC_AM_2").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC").set_hex_dump_max_size(100);

  if (not rlc1.configure(config)) {
    return -1;
  }

  if (not rlc2.configure(config)) {
    return -1;
  }

  bool reetablished_once = false;

  // Generate 40 SDUs/PDUs
  const uint32_t total_num_tx_pdus = config.am.max_retx_thresh * 10;
  uint32_t       num_tx_pdus       = 0;

  // Create a few SDUs and write to RLC1 to make sure buffers aren't empty after tx one PDU
  for (uint32_t i = num_tx_pdus; i < 5; ++i) {
    // Write SDU
    unique_byte_buffer_t sdu = srsran::make_byte_buffer();
    TESTASSERT(sdu != nullptr);
    sdu->N_bytes = 5; // Give each buffer a size of 1 byte
    for (uint32_t k = 0; k < sdu->N_bytes; ++k) {
      sdu->msg[k] = i; // Write the index into the buffer
    }
    sdu->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu));
  }

  for (uint32_t i = num_tx_pdus; i < total_num_tx_pdus; i++) {
    // Write SDU
    unique_byte_buffer_t sdu = srsran::make_byte_buffer();
    TESTASSERT(sdu != nullptr);
    sdu->N_bytes = 5; // Give each buffer a size of 1 byte
    for (uint32_t k = 0; k < sdu->N_bytes; ++k) {
      sdu->msg[k] = i; // Write the index into the buffer
    }
    sdu->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu));

    // Read PDU
    unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    pdu->N_bytes             = rlc1.read_pdu(pdu->msg, 7); // 2 byte header + 5 byte payload;

    // Find SN=0 PDU
    bool is_data_pdu_sn0 = false;
    if (not rlc_am_is_control_pdu(pdu->msg)) {
      // After reestablishment after maxretx, also SN=0 is delivered
      if (not reetablished_once) {
        rlc_amd_pdu_header_t header = {};
        rlc_am_read_data_pdu_header(pdu.get(), &header);
        if (header.sn == 0) {
          is_data_pdu_sn0 = true;
        }
      }
    }

    // Deliver all PDUs but SN=0 to RLC2
    if (not is_data_pdu_sn0) {
      rlc2.write_pdu(pdu->msg, pdu->N_bytes);
#if HAVE_PCAP
      pcap.write_dl_ccch(pdu->msg, pdu->N_bytes);
#endif
    }

    // Check if RLC2 has something to send
    if (rlc2.get_buffer_state() > 0) {
      byte_buffer_t status_buf;
      status_buf.N_bytes = rlc2.read_pdu(status_buf.msg, 5); // provide only small grant
      TESTASSERT(status_buf.N_bytes != 0);

      // Write status PDU to RLC1
      rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
#if HAVE_PCAP
      pcap.write_ul_ccch(status_buf.msg, status_buf.N_bytes);
#endif
    }

    // each interation is one TTI
    timers.step_all();

    // Reestablish if max retx have been reached
    if (tester.max_retx_triggered and !reetablished_once) {
      rlc1.reestablish();
      rlc2.reestablish();
      // make sure we only reesablish once
      reetablished_once = true;
    }
  }

  TESTASSERT(tester.sdus.size() == 18);

  srslog::fetch_basic_logger("TEST").info("Received %zd SDUs", tester.sdus.size());

#if HAVE_PCAP
  pcap.close();
#endif

  return SRSRAN_SUCCESS;
}

// This test checks the correct functioning of RLC discard functionality
bool discard_test()
{
  const rlc_config_t config = rlc_config_t::default_rlc_am_config();
#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_reestablish_test.pcap", config);
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif

  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  srslog::fetch_basic_logger("RLC_AM_1").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC_AM_2").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC").set_hex_dump_max_size(100);

  if (not rlc1.configure(config)) {
    return -1;
  }

  if (not rlc2.configure(config)) {
    return -1;
  }

  // Check has_data() after a SDU discard
  {
    uint32_t num_tx_pdus = 1;
    for (uint32_t i = 0; i < num_tx_pdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes = 5;
      for (uint32_t k = 0; k < sdu->N_bytes; ++k) {
        sdu->msg[k] = i; // Write the index into the buffer
      }
      sdu->md.pdcp_sn = i;
      rlc1.write_sdu(std::move(sdu));
    }
  }
  rlc1.discard_sdu(0); // Try to discard PDCP_SN=1
  TESTASSERT(rlc1.has_data() == false);

  // Discard an SDU in the midle of the queue and read PDUs after
  {
    uint32_t num_tx_pdus = 10;
    for (uint32_t i = 0; i < num_tx_pdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes = 1;
      for (uint32_t k = 0; k < sdu->N_bytes; ++k) {
        sdu->msg[k] = i; // Write the index into the buffer
      }
      sdu->md.pdcp_sn = i;
      rlc1.write_sdu(std::move(sdu));
    }
  }
  rlc1.discard_sdu(3); // Try to discard PDCP_SN=1
  TESTASSERT(rlc1.has_data() == true);
  TESTASSERT(rlc1.get_buffer_state() == 23); // 2 bytes fixed header, 12 , 9 bytes of data,

  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  uint32_t             len = rlc1.read_pdu(pdu->msg, 50); // enough for all PDUs
  pdu->N_bytes             = len;
  TESTASSERT(23 == len);

  srslog::fetch_basic_logger("TEST").info("Received %zd SDUs", tester.sdus.size());

#if HAVE_PCAP
  pcap.close();
#endif

  return SRSRAN_SUCCESS;
}

// This test checks wether re-transmissions are triggered correctly in case the t-PollRetranmission expires.
// It checks if the poll retx timer is re-armed upon receiving an ACK for POLL_SN
bool poll_retx_expiry_test()
{
  rlc_config_t config = rlc_config_t::default_rlc_am_config();
  // [I] SRB1 configured: t_poll_retx=65, poll_pdu=-1, poll_byte=-1, max_retx_thresh=6, t_reordering=55,
  // t_status_prohibit=0
  config.am.t_poll_retx       = 65;
  config.am.poll_pdu          = -1;
  config.am.poll_byte         = -1;
  config.am.max_retx_thresh   = 6;
  config.am.t_reordering      = 55;
  config.am.t_status_prohibit = 55;

#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_poll_rext_expiry_test.pcap", config);
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif

  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  srslog::fetch_basic_logger("RLC_AM_1").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC_AM_2").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC").set_hex_dump_max_size(100);

  if (not rlc1.configure(config)) {
    return -1;
  }

  if (not rlc2.configure(config)) {
    return -1;
  }

  // [I] SRB1 Tx SDU (135 B, tx_sdu_queue_len=1)
  // [I] SRB1 Tx PDU SN=3 (91 B)
  // [I] SRB1 Tx PDU SN=4 (48 B)
  {
    // Initial Tx
    uint32_t num_tx_pdus = 1;
    for (uint32_t i = 0; i < num_tx_pdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes = 135;
      for (uint32_t k = 0; k < sdu->N_bytes; ++k) {
        sdu->msg[k] = i; // Write the index into the buffer
      }
      sdu->md.pdcp_sn = i;
      rlc1.write_sdu(std::move(sdu));
    }
    unique_byte_buffer_t pdu1 = srsran::make_byte_buffer();
    TESTASSERT(pdu1 != nullptr);
    pdu1->N_bytes = rlc1.read_pdu(pdu1->msg, 91);

    unique_byte_buffer_t pdu2 = srsran::make_byte_buffer();
    TESTASSERT(pdu2 != nullptr);
    pdu2->N_bytes = rlc1.read_pdu(pdu2->msg, 48);

    // Deliver PDU2 to RLC2. PDU1 is lost
    rlc2.write_pdu(pdu2->msg, pdu2->N_bytes);
  }

  // Step timers until t-PollRetransmission timer expires on RLC1
  // t-Reordering timer also will expire on RLC2, so we can get an status report.
  // [I] SRB1 Schedule SN=3 for reTx
  for (int cnt = 0; cnt < 65; cnt++) {
    timers.step_all();
  }

  uint32_t status_size = rlc2.get_buffer_state();
  srslog::flush();
  TESTASSERT(4 == status_size);

  // Read status PDU from RLC2
  unique_byte_buffer_t status_buf = srsran::make_byte_buffer();
  TESTASSERT(status_buf != nullptr);
  int len             = rlc2.read_pdu(status_buf->msg, status_size);
  status_buf->N_bytes = len;

  TESTASSERT(0 == rlc2.get_buffer_state());

  // Assert status is correct
  rlc_status_pdu_t status_check = {};
  rlc_am_read_status_pdu(status_buf->msg, status_buf->N_bytes, &status_check);
  TESTASSERT(status_check.ack_sn == 2); // 2 is the SN after the largest SN received.
  TESTASSERT(status_check.N_nack == 1); // 1 PDU lost
  TESTASSERT(rlc_am_is_valid_status_pdu(status_check));

  // [I] SRB1 Retx PDU segment SN=3 [so=0] (83 B) (attempt 2/6)
  {
    unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    TESTASSERT(pdu != nullptr);
    pdu->N_bytes = rlc1.read_pdu(pdu->msg, 83);
  }

  // [I] SRB1 Retx PDU segment SN=3 [so=79] (14 B) (attempt 2/6)
  {
    unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    TESTASSERT(pdu != nullptr);
    pdu->N_bytes = rlc1.read_pdu(pdu->msg, 79);
  }

  // Deliver status PDU after ReTX to RLC1. This should restart t-PollRetransmission
  TESTASSERT_EQ(false, rlc1.has_data());
  rlc1.write_pdu(status_buf->msg, status_buf->N_bytes);
  TESTASSERT_EQ(true, rlc1.has_data());

  // [I] SRB1 Retx PDU segment SN=3 [so=0] (83 B) (attempt 3/6) (received a NACK and retx...)
  // [I] SRB1 Retx PDU segment SN=3 [so=79] (14 B) (attempt 3/6)
  {
    unique_byte_buffer_t pdu1 = srsran::make_byte_buffer();
    TESTASSERT(pdu1 != nullptr);
    pdu1->N_bytes = rlc1.read_pdu(pdu1->msg, 83);

    unique_byte_buffer_t pdu2 = srsran::make_byte_buffer();
    TESTASSERT(pdu2 != nullptr);
    pdu2->N_bytes = rlc1.read_pdu(pdu2->msg, 14);
  }

  TESTASSERT_EQ(false, rlc1.has_data());

  // Step timers until t-PollRetransmission timer expires on RLC1
  // [I] SRB1 Schedule SN=3 for reTx

  for (int cnt = 0; cnt < 66; cnt++) {
    timers.step_all();
  }
  TESTASSERT_EQ(true, rlc1.has_data());
  srslog::fetch_basic_logger("TEST").info("t-Poll Retransmssion successfully restarted.");

#if HAVE_PCAP
  pcap.close();
#endif

  return SRSRAN_SUCCESS;
}

bool full_window_check_test()
{
  rlc_config_t config = rlc_config_t::default_rlc_am_config();
  // [I] SRB1 configured: t_poll_retx=65, poll_pdu=-1, poll_byte=-1, max_retx_thresh=6, t_reordering=55,
  // t_status_prohibit=0
  config.am.t_poll_retx       = 65;
  config.am.poll_pdu          = -1;
  config.am.poll_byte         = -1;
  config.am.max_retx_thresh   = 6;
  config.am.t_reordering      = 55;
  config.am.t_status_prohibit = 55;

#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_poll_rext_expiry_test.pcap", config);
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif

  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  srslog::fetch_basic_logger("RLC_AM_1").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC_AM_2").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC").set_hex_dump_max_size(100);

  if (not rlc1.configure(config)) {
    return -1;
  }

  if (not rlc2.configure(config)) {
    return -1;
  }

  {
    // Initial Tx
    uint32_t num_tx_pdus = 512;
    for (uint32_t i = 0; i < num_tx_pdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes    = 1;
      sdu->msg[0]     = i;
      sdu->md.pdcp_sn = i;
      rlc1.write_sdu(std::move(sdu));

      unique_byte_buffer_t pdu = srsran::make_byte_buffer();
      TESTASSERT(pdu != nullptr);
      pdu->N_bytes    = 1;
      pdu->msg[0]     = i;
      pdu->md.pdcp_sn = i;
      pdu->N_bytes    = rlc1.read_pdu(pdu->msg, 3);
      TESTASSERT(pdu->N_bytes == 3);
    }
  }
  {
    // Tx one more to check the window is full
    unique_byte_buffer_t sdu = srsran::make_byte_buffer();
    TESTASSERT(sdu != nullptr);
    sdu->N_bytes    = 1;
    sdu->msg[0]     = 0;
    sdu->md.pdcp_sn = 512;
    rlc1.write_sdu(std::move(sdu));

    unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    TESTASSERT(pdu != nullptr);
    pdu->N_bytes = rlc1.read_pdu(pdu->msg, 3);
    TESTASSERT(pdu->N_bytes == 3);

    // If the TX window is full, we should RETX SN=0
    rlc_amd_pdu_header_t header = {};
    rlc_am_read_data_pdu_header(&pdu->msg, &pdu->N_bytes, &header);
    TESTASSERT_EQ(header.sn, 0);
    TESTASSERT_EQ(header.N_li, 0);
    TESTASSERT_EQ(header.fi, 0);
  }

  // Ack one SN in the middle of the TX window.
  // This is done to make sure the full window check is correct
  // even if PDUs in the middle of the window are ACKed.
  // ACK_SN=3, NACK_SN=0
  {
    rlc_status_pdu_t status = {};
    status.ack_sn           = 3;
    status.N_nack           = 1;
    status.nacks[0].nack_sn = 0;

    unique_byte_buffer_t status_buf = srsran::make_byte_buffer();
    TESTASSERT(status_buf != nullptr);
    rlc_am_write_status_pdu(&status, status_buf.get());
    rlc1.write_pdu(status_buf->msg, status_buf->N_bytes);

    // Read RETX for SN=0 from NACK
    unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    TESTASSERT(pdu != nullptr);
    pdu->N_bytes = rlc1.read_pdu(pdu->msg, 3);
    TESTASSERT(pdu->N_bytes == 3);

    // Check RETX SN=0
    rlc_amd_pdu_header_t header = {};
    rlc_am_read_data_pdu_header(&pdu->msg, &pdu->N_bytes, &header);
    TESTASSERT_EQ(header.sn, 0);
    TESTASSERT_EQ(header.N_li, 0);
    TESTASSERT_EQ(header.fi, 0);
    TESTASSERT_EQ(0, rlc1.get_buffer_state());
  }
  {
    // Tx more PDUs to check the window is still full
    uint32_t num_tx_pdus = 2;
    for (uint32_t i = 0; i < num_tx_pdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes    = 1;
      sdu->msg[0]     = i;
      sdu->md.pdcp_sn = i;
      rlc1.write_sdu(std::move(sdu));

      unique_byte_buffer_t pdu = srsran::make_byte_buffer();
      TESTASSERT(pdu != nullptr);
      pdu->N_bytes    = 1;
      pdu->msg[0]     = i;
      pdu->md.pdcp_sn = i;
      pdu->N_bytes    = rlc1.read_pdu(pdu->msg, 3);
      TESTASSERT(pdu->N_bytes == 3);

      // If the TX window is full, we should RETX SN=0
      rlc_amd_pdu_header_t header = {};
      rlc_am_read_data_pdu_header(&pdu->msg, &pdu->N_bytes, &header);
      TESTASSERT_EQ(header.sn, 0);
      TESTASSERT_EQ(header.N_li, 0);
      TESTASSERT_EQ(header.fi, 0);
    }
  }
  // ACK more PDUs and advance VT(A).
  // New PDUs should be available to read now.
  {
    rlc_status_pdu_t status = {};
    status.ack_sn           = 5;
    status.N_nack           = 0;

    unique_byte_buffer_t status_buf = srsran::make_byte_buffer();
    TESTASSERT(status_buf != nullptr);
    rlc_am_write_status_pdu(&status, status_buf.get());
    rlc1.write_pdu(status_buf->msg, status_buf->N_bytes);

    // Read new PDU
    unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    TESTASSERT(pdu != nullptr);
    pdu->N_bytes = rlc1.read_pdu(pdu->msg, 3);
    TESTASSERT(pdu->N_bytes == 3);

    // If the TX window is no longer full, we should TX a new SN (SN=512)
    rlc_amd_pdu_header_t header = {};
    rlc_am_read_data_pdu_header(&pdu->msg, &pdu->N_bytes, &header);
    TESTASSERT_EQ(header.sn, 512);
    TESTASSERT_EQ(header.N_li, 0);
    TESTASSERT_EQ(header.fi, 0);
  }

#if HAVE_PCAP
  pcap.close();
#endif

  return SRSRAN_SUCCESS;
}

bool full_window_check_wraparound_test()
{
  rlc_config_t config = rlc_config_t::default_rlc_am_config();
  // [I] SRB1 configured: t_poll_retx=65, poll_pdu=-1, poll_byte=-1, max_retx_thresh=6, t_reordering=55,
  // t_status_prohibit=0
  config.am.t_poll_retx       = 65;
  config.am.poll_pdu          = -1;
  config.am.poll_byte         = -1;
  config.am.max_retx_thresh   = 6;
  config.am.t_reordering      = 55;
  config.am.t_status_prohibit = 55;

#if HAVE_PCAP
  rlc_pcap pcap;
  pcap.open("rlc_am_poll_rext_expiry_test.pcap", config);
  rlc_am_tester tester(true, &pcap);
#else
  rlc_am_tester tester(true, NULL);
#endif

  uint32_t pdcp_count = 0;

  srsran::timer_handler timers(8);

  rlc_am rlc1(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::lte, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  srslog::fetch_basic_logger("RLC_AM_1").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC_AM_2").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC").set_hex_dump_max_size(100);

  if (not rlc1.configure(config)) {
    return -1;
  }

  if (not rlc2.configure(config)) {
    return -1;
  }

  // Advance vt_a to 512 and vt_s to 512 as well.
  {
    // Initial Tx
    uint32_t num_tx_pdus = 512;
    for (uint32_t i = 0; i < num_tx_pdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes    = 1;
      sdu->msg[0]     = i;
      sdu->md.pdcp_sn = pdcp_count++;
      rlc1.write_sdu(std::move(sdu));

      unique_byte_buffer_t pdu = srsran::make_byte_buffer();
      TESTASSERT(pdu != nullptr);
      pdu->N_bytes = rlc1.read_pdu(pdu->msg, 3);
      TESTASSERT(pdu->N_bytes == 3);
    }

    // ACK all SNs to advance the TX window.
    rlc_status_pdu_t status = {};
    status.ack_sn           = num_tx_pdus;
    status.N_nack           = 0;

    unique_byte_buffer_t status_buf = srsran::make_byte_buffer();
    TESTASSERT(status_buf != nullptr);
    rlc_am_write_status_pdu(&status, status_buf.get());
    rlc1.write_pdu(status_buf->msg, status_buf->N_bytes);
  }

  // Advance vt_a and vt_s to 1023
  {
    // Initial Tx
    uint32_t num_tx_pdus = 511;
    for (uint32_t i = 0; i < num_tx_pdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes    = 1;
      sdu->msg[0]     = i;
      sdu->md.pdcp_sn = pdcp_count++;
      rlc1.write_sdu(std::move(sdu));

      unique_byte_buffer_t pdu = srsran::make_byte_buffer();
      TESTASSERT(pdu != nullptr);
      pdu->N_bytes = rlc1.read_pdu(pdu->msg, 3);
      TESTASSERT(pdu->N_bytes == 3);
    }

    // ACK all SNs to advance the TX window.
    rlc_status_pdu_t status = {};
    status.ack_sn           = 512 + num_tx_pdus;
    status.N_nack           = 0;

    unique_byte_buffer_t status_buf = srsran::make_byte_buffer();
    TESTASSERT(status_buf != nullptr);
    rlc_am_write_status_pdu(&status, status_buf.get());
    rlc1.write_pdu(status_buf->msg, status_buf->N_bytes);
  }

  // Now, fill up the window
  {
    // Initial Tx
    uint32_t num_tx_pdus = 512;
    for (uint32_t i = 0; i < num_tx_pdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes    = 1;
      sdu->msg[0]     = i;
      sdu->md.pdcp_sn = pdcp_count++;
      rlc1.write_sdu(std::move(sdu));

      unique_byte_buffer_t pdu = srsran::make_byte_buffer();
      TESTASSERT(pdu != nullptr);
      pdu->N_bytes = rlc1.read_pdu(pdu->msg, 3);
      TESTASSERT(pdu->N_bytes == 3);
    }
  }
  {
    // Tx one more to check the window is full
    unique_byte_buffer_t sdu = srsran::make_byte_buffer();
    TESTASSERT(sdu != nullptr);
    sdu->N_bytes    = 1;
    sdu->msg[0]     = 0;
    sdu->md.pdcp_sn = pdcp_count++;
    rlc1.write_sdu(std::move(sdu));

    unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    TESTASSERT(pdu != nullptr);
    pdu->N_bytes = rlc1.read_pdu(pdu->msg, 3);
    TESTASSERT(pdu->N_bytes == 3);

    // If the TX window is full, we should RETX SN=1023
    rlc_amd_pdu_header_t header = {};
    rlc_am_read_data_pdu_header(&pdu->msg, &pdu->N_bytes, &header);
    TESTASSERT_EQ(header.sn, 1023);
    TESTASSERT_EQ(header.N_li, 0);
    TESTASSERT_EQ(header.fi, 0);
  }

#if HAVE_PCAP
  pcap.close();
#endif

  return SRSRAN_SUCCESS;
}
int main(int argc, char** argv)
{
  // Setup the log message spy to intercept error and warning log entries from RLC
  if (!srslog::install_custom_sink(srsran::log_sink_message_spy::name(),
                                   std::unique_ptr<srsran::log_sink_message_spy>(
                                       new srsran::log_sink_message_spy(srslog::get_default_log_formatter())))) {
    return SRSRAN_ERROR;
  }

  auto* spy = static_cast<srsran::log_sink_message_spy*>(srslog::find_sink(srsran::log_sink_message_spy::name()));
  if (!spy) {
    return SRSRAN_ERROR;
  }
  srslog::set_default_sink(*spy);

  auto& logger_rrc1 = srslog::fetch_basic_logger("RLC_AM_1", *spy, false);
  auto& logger_rrc2 = srslog::fetch_basic_logger("RLC_AM_2", *spy, false);
  logger_rrc1.set_hex_dump_max_size(100);
  logger_rrc2.set_hex_dump_max_size(100);
  logger_rrc1.set_level(srslog::basic_levels::debug);
  logger_rrc2.set_level(srslog::basic_levels::debug);

  // start log backend
  srslog::init();

  if (basic_test()) {
    printf("basic_test failed\n");
    exit(-1);
  };

  if (concat_test()) {
    printf("concat_test failed\n");
    exit(-1);
  };

  if (segment_test(true)) {
    printf("segment_test with in-order PDU reception failed\n");
    exit(-1);
  };

  if (segment_test(false)) {
    printf("segment_test with out-of-order PDU reception failed\n");
    exit(-1);
  };

  if (retx_test()) {
    printf("retx_test failed\n");
    exit(-1);
  };

  if (max_retx_test()) {
    printf("max_retx_test failed\n");
    exit(-1);
  };

  if (reestablish_test()) {
    printf("reestablish_test failed\n");
    exit(-1);
  };

  if (segment_retx_test()) {
    printf("segment_retx_test failed\n");
    exit(-1);
  };

  if (resegment_test_1()) {
    printf("resegment_test_1 failed\n");
    exit(-1);
  };

  if (resegment_test_2()) {
    printf("resegment_test_2 failed\n");
    exit(-1);
  };

  if (resegment_test_3()) {
    printf("resegment_test_3 failed\n");
    exit(-1);
  };

  if (resegment_test_4()) {
    printf("resegment_test_4 failed\n");
    exit(-1);
  };

  if (resegment_test_5()) {
    printf("resegment_test_5 failed\n");
    exit(-1);
  };

  if (resegment_test_6()) {
    printf("resegment_test_6 failed\n");
    exit(-1);
  };

  logger_rrc1.set_hex_dump_max_size(100);
  logger_rrc2.set_hex_dump_max_size(100);
  if (resegment_test_7()) {
    printf("resegment_test_7 failed\n");
    exit(-1);
  }

  if (resegment_test_8()) {
    printf("resegment_test_8 failed\n");
    exit(-1);
  };
  logger_rrc1.set_hex_dump_max_size(-1);
  logger_rrc2.set_hex_dump_max_size(-1);

  if (resegment_test_9()) {
    printf("resegment_test_9 failed\n");
    exit(-1);
  };

  if (resegment_test_10()) {
    printf("resegment_test_10 failed\n");
    exit(-1);
  };

  if (resegment_test_11()) {
    printf("resegment_test_11 failed\n");
    exit(-1);
  };

  if (resegment_test_12()) {
    printf("resegment_test_12 failed\n");
    exit(-1);
  };

  // Set of unique header reconstruction tests using the logspy
  if (header_reconstruction_test(*spy)) {
    printf("header_reconstruction_test failed\n");
    exit(-1);
  }

  if (header_reconstruction_test2(*spy)) {
    printf("header_reconstruction_test2 failed\n");
    exit(-1);
  }

  if (header_reconstruction_test3(*spy)) {
    printf("header_reconstruction_test3 failed\n");
    exit(-1);
  }

  if (header_reconstruction_test4(*spy)) {
    printf("header_reconstruction_test4 failed\n");
    exit(-1);
  }

  if (header_reconstruction_test5(*spy)) {
    printf("header_reconstruction_test5 failed\n");
    exit(-1);
  }

  if (header_reconstruction_test6(*spy)) {
    printf("header_reconstruction_test6 failed\n");
    exit(-1);
  }

  if (header_reconstruction_test7(*spy)) {
    printf("header_reconstruction_test7 failed\n");
    exit(-1);
  }

  if (header_reconstruction_test8(*spy)) {
    printf("header_reconstruction_test8 failed\n");
    exit(-1);
  }

  if (reset_test()) {
    printf("reset_test failed\n");
    exit(-1);
  };

  if (stop_test()) {
    printf("stop_test failed\n");
    exit(-1);
  };

  if (resume_test()) {
    printf("resume_test failed\n");
    exit(-1);
  };

  if (status_pdu_test()) {
    printf("status_pdu_test failed\n");
    exit(-1);
  };

  if (incorrect_status_pdu_test()) {
    printf("incorrect_status_pdu_test failed\n");
    exit(-1);
  };

  if (incorrect_status_pdu_test2()) {
    printf("incorrect_status_pdu_test2 failed\n");
    exit(-1);
  };

  if (discard_test()) {
    printf("discard_test failed\n");
    exit(-1);
  };

  if (poll_retx_expiry_test()) {
    printf("poll_retx_expiry_test failed\n");
    exit(-1);
  };

  if (full_window_check_test()) {
    printf("full_window_check_test failed\n");
    exit(-1);
  };

  if (full_window_check_wraparound_test()) {
    printf("full_window_check_wraparound_test failed\n");
    exit(-1);
  };
  return SRSRAN_SUCCESS;
}
