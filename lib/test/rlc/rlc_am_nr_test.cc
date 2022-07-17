/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
#include "srsran/rlc/rlc_am_nr.h"

#define NBUFS 5
#define HAVE_PCAP 0
#define SDU_SIZE 500

using namespace srsue;
using namespace srsran;

int basic_test_tx(rlc_am* rlc, byte_buffer_t pdu_bufs[NBUFS], rlc_am_nr_sn_size_t sn_size)
{
  // Push 5 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[NBUFS];
  constexpr uint32_t   payload_size = 1; // Give each buffer a size of 1 byte
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i;            // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = payload_size; // Give each buffer a size of 1 byte
    sdu_bufs[i]->md.pdcp_sn = i;            // PDCP SN for notifications
    rlc->write_sdu(std::move(sdu_bufs[i]));
  }

  uint32_t header_size         = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  uint32_t data_pdu_size       = header_size + payload_size;
  uint32_t expect_buffer_state = NBUFS * data_pdu_size;
  TESTASSERT_EQ(expect_buffer_state, rlc->get_buffer_state());

  // Read 5 PDUs from RLC1 (1 byte each)
  for (int i = 0; i < NBUFS; i++) {
    uint32_t len        = rlc->read_pdu(pdu_bufs[i].msg, data_pdu_size);
    pdu_bufs[i].N_bytes = len;
    TESTASSERT_EQ(data_pdu_size, len);
  }

  TESTASSERT_EQ(0, rlc->get_buffer_state());
  return SRSRAN_SUCCESS;
}

/*
 * Test the limits of the TX/RX window checkers
 */
int window_checker_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("window checkers ({} bit SN)", to_number(sn_size));
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);

  rlc_am_nr_tx* tx = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return SRSRAN_ERROR;
  }

  {
    // RLC1 RX_NEXT == 0 and RLC2 TX_NEXT_ACK == 0
    uint32_t sn_inside_below  = 0;
    uint32_t sn_inside_above  = cardinality(sn_size) / 2 - 1;
    uint32_t sn_outside_below = cardinality(sn_size) - 1;
    uint32_t sn_outside_above = cardinality(sn_size) / 2;
    TESTASSERT_EQ(true, rx->inside_rx_window(sn_inside_below));
    TESTASSERT_EQ(true, rx->inside_rx_window(sn_inside_above));
    TESTASSERT_EQ(false, rx->inside_rx_window(sn_outside_below));
    TESTASSERT_EQ(false, rx->inside_rx_window(sn_outside_above));
    TESTASSERT_EQ(true, tx->inside_tx_window(sn_inside_below));
    TESTASSERT_EQ(true, tx->inside_tx_window(sn_inside_above));
    TESTASSERT_EQ(false, tx->inside_tx_window(sn_outside_below));
    TESTASSERT_EQ(false, tx->inside_tx_window(sn_outside_above));
  }

  rlc_am_nr_rx_state_t rx_st = {};
  rx_st.rx_next              = cardinality(sn_size) - 1;
  ;
  rlc_am_nr_tx_state_t tx_st = {};
  tx_st.tx_next_ack          = cardinality(sn_size) - 1;
  ;

  rx->set_rx_state(rx_st);
  tx->set_tx_state(tx_st);

  {
    // RX_NEXT == 4095 TX_NEXT_ACK == 4095
    uint32_t sn_inside_below  = 0;
    uint32_t sn_inside_above  = cardinality(sn_size) / 2 - 2;
    uint32_t sn_outside_below = cardinality(sn_size) - 2;
    uint32_t sn_outside_above = cardinality(sn_size) / 2;
    TESTASSERT_EQ(true, rx->inside_rx_window(sn_inside_below));
    TESTASSERT_EQ(true, rx->inside_rx_window(sn_inside_above));
    TESTASSERT_EQ(false, rx->inside_rx_window(sn_outside_below));
    TESTASSERT_EQ(false, rx->inside_rx_window(sn_outside_above));
    TESTASSERT_EQ(true, tx->inside_tx_window(sn_inside_below));
    TESTASSERT_EQ(true, tx->inside_tx_window(sn_inside_above));
    TESTASSERT_EQ(false, tx->inside_tx_window(sn_outside_below));
    TESTASSERT_EQ(false, tx->inside_tx_window(sn_outside_above));
  }
  return SRSRAN_SUCCESS;
}

/*
 * Test is retx_segmentation required
 *
 */
int retx_segmentation_required_checker_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("retx segmentation required checkers ({} bit SN)", to_number(sn_size));
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);

  rlc_am_nr_tx* tx = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return SRSRAN_ERROR;
  }

  unique_byte_buffer_t sdu_bufs[NBUFS];
  unique_byte_buffer_t pdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    pdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i; // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = 5; // Give each buffer a size of 1 byte
    sdu_bufs[i]->md.pdcp_sn = i; // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
    rlc1.read_pdu(pdu_bufs[i]->msg, 8);
  }

  // Test full SDU retx
  {
    uint32_t          nof_bytes = 8;
    rlc_amd_retx_nr_t retx      = {};
    retx.sn                     = 0;
    retx.is_segment             = false;

    tx->is_retx_segmentation_required(retx, nof_bytes);
    TESTASSERT_EQ(false, tx->is_retx_segmentation_required(retx, nof_bytes));
  }

  // Test SDU retx segmentation required
  {
    uint32_t          nof_bytes = 4;
    rlc_amd_retx_nr_t retx;
    retx.sn         = 0;
    retx.is_segment = false;

    tx->is_retx_segmentation_required(retx, nof_bytes);
    TESTASSERT_EQ(true, tx->is_retx_segmentation_required(retx, nof_bytes));
  }

  // Test full SDU segment retx
  {
    uint32_t          nof_bytes = 40;
    rlc_amd_retx_nr_t retx      = {};
    retx.sn                     = 0;
    retx.is_segment             = true;
    retx.so_start               = 4;
    retx.segment_length         = 2;

    tx->is_retx_segmentation_required(retx, nof_bytes);
    TESTASSERT_EQ(false, tx->is_retx_segmentation_required(retx, nof_bytes));
  }

  // Test SDU segment retx segmentation required
  {
    uint32_t          nof_bytes = 4;
    rlc_amd_retx_nr_t retx      = {};
    retx.sn                     = 0;
    retx.is_segment             = true;
    retx.so_start               = 4;
    retx.segment_length         = 2;

    tx->is_retx_segmentation_required(retx, nof_bytes);
    TESTASSERT_EQ(true, tx->is_retx_segmentation_required(retx, nof_bytes));
  }
  return SRSRAN_SUCCESS;
}

/*
 * Test the transmission and acknowledgement of 5 SDUs.
 *
 * Each SDU is transmitted as a single PDU.
 * There are no lost PDUs, and the byte size is small, so the Poll_PDU configuration
 * will trigger the status report.
 * Poll PDU is configured to 4, so the 5th PDU should set the polling bit.
 */
int basic_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("basic tx/rx ({} bit SN)", to_number(sn_size));
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return -1;
  }

  // after configuring entity
  TESTASSERT_EQ(0, rlc1.get_buffer_state());

  basic_test_tx(&rlc1, pdu_bufs, sn_size);

  // Write 5 PDUs into RLC2
  for (int i = 0; i < NBUFS; i++) {
    rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  TESTASSERT_EQ(3, rlc2.get_buffer_state());
  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  int           len  = rlc2.read_pdu(status_buf.msg, 3);
  status_buf.N_bytes = len;

  TESTASSERT_EQ(0, rlc2.get_buffer_state());

  // Assert status is correct
  rlc_am_nr_status_pdu_t status_check(sn_size);
  rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
  TESTASSERT_EQ(5, status_check.ack_sn); // 5 is the last SN that was not received.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Check TX_NEXT_ACK
  rlc_am_nr_tx_state_t st = tx1->get_tx_state();
  TESTASSERT_EQ(5, st.tx_next_ack);
  TESTASSERT_EQ(0, tx1->get_tx_window_utilization());

  // Check PDCP notifications
  TESTASSERT_EQ(5, tester.notified_counts.size());
  for (uint16_t i = 0; i < tester.sdus.size(); i++) {
    TESTASSERT_EQ(1, tester.sdus[i]->N_bytes);
    TESTASSERT_EQ(i, *(tester.sdus[i]->msg));
    TESTASSERT_EQ(1, tester.notified_counts[i]);
  }

  // Check statistics
  rlc_bearer_metrics_t metrics1 = rlc1.get_metrics();
  rlc_bearer_metrics_t metrics2 = rlc2.get_metrics();

  constexpr uint32_t payload_size       = 1;
  uint32_t           header_size        = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  uint32_t           data_pdu_size      = header_size + payload_size;
  constexpr uint32_t status_pdu_size    = 3;
  uint32_t           total_tx_pdu_bytes = NBUFS * data_pdu_size; // NBUFS * PDU size
  uint32_t           total_rx_pdu_bytes = status_pdu_size;       // One status PDU

  // RLC1 PDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_sdus);
  TESTASSERT_EQ(0, metrics1.num_rx_sdus);
  TESTASSERT_EQ(5, metrics1.num_tx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);
  // RLC1 SDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_pdus);
  TESTASSERT_EQ(1, metrics1.num_rx_pdus);                       // One status PDU
  TESTASSERT_EQ(total_tx_pdu_bytes, metrics1.num_tx_pdu_bytes); // NBUFS * PDU size
  TESTASSERT_EQ(total_rx_pdu_bytes, metrics1.num_rx_pdu_bytes); // One status PDU
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);                     // No lost SDUs

  // RLC2 PDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_sdus);
  TESTASSERT_EQ(5, metrics2.num_rx_sdus);
  TESTASSERT_EQ(0, metrics2.num_tx_sdu_bytes);
  TESTASSERT_EQ(5, metrics2.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);
  // RLC2 SDU metrics
  TESTASSERT_EQ(1, metrics2.num_tx_pdus);                       // One status PDU
  TESTASSERT_EQ(5, metrics2.num_rx_pdus);                       // 5 SDUs
  TESTASSERT_EQ(total_rx_pdu_bytes, metrics2.num_tx_pdu_bytes); // One status PDU
  TESTASSERT_EQ(total_tx_pdu_bytes, metrics2.num_rx_pdu_bytes); // NBUFS * PDU size
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);                     // No lost SDUs
  return SRSRAN_SUCCESS;
}

/*
 * Test the loss of a single PDU.
 * NACK should be visible in the status report.
 * Retx after NACK should be present too.
 * No further status reports shall be issued.
 */
int lost_pdu_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  test_delimit_logger delimiter("lost PDU ({} bit SN)", to_number(sn_size));

  constexpr uint32_t payload_size        = 1;
  uint32_t           header_size         = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  uint32_t           data_pdu_size       = header_size + payload_size;
  uint32_t           expect_buffer_state = NBUFS * data_pdu_size;

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return -1;
  }

  rlc_config_t rlc2_config = rlc_config_t::default_rlc_am_nr_config(to_number(sn_size));
  if (not rlc2.configure(rlc2_config)) {
    return -1;
  }

  // after configuring entity
  TESTASSERT(0 == rlc1.get_buffer_state());

  basic_test_tx(&rlc1, pdu_bufs, sn_size);

  // Write 5 PDUs into RLC2
  for (int i = 0; i < NBUFS; i++) {
    if (i != 3) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes); // Don't write RLC_SN=3.
    }
  }

  // Only after t-reassembly has expired, will the status report include NACKs.
  TESTASSERT_EQ(3, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, 5);
    status_buf.N_bytes = len;

    TESTASSERT(0 == rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(3, status_check.ack_sn); // 3 is the next expected SN (i.e. the lost packet.)

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // t-reassembly has expired. There should be a NACK in the status report.
  constexpr uint32_t status_pdu_ack_size  = 3;
  uint32_t           status_pdu_nack_size = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  TESTASSERT_EQ(status_pdu_ack_size + status_pdu_nack_size, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    uint32_t      len  = rlc2.read_pdu(status_buf.msg, status_pdu_ack_size + status_pdu_nack_size);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);           // 5 is the next expected SN.
    TESTASSERT_EQ(1, status_check.nacks.size());     // We lost one PDU.
    TESTASSERT_EQ(3, status_check.nacks[0].nack_sn); // Lost PDU SN=3.

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there is an Retx of SN=3
    TESTASSERT_EQ(data_pdu_size, rlc1.get_buffer_state());
  }

  {
    // Check correct re-transmission
    byte_buffer_t retx_buf;
    uint32_t      len = rlc1.read_pdu(retx_buf.msg, data_pdu_size);
    retx_buf.N_bytes  = len;
    TESTASSERT_EQ(data_pdu_size, len);

    // Polling bit on the RETX should be required, as the buffers are not empty.
    rlc2.write_pdu(retx_buf.msg, retx_buf.N_bytes);

    TESTASSERT_EQ(0, rlc2.get_buffer_state()); // t-StatusProhibit is still running
  }

  // Step timers until t-StatusProhibit expires
  for (int cnt = 0; cnt < 8; cnt++) {
    timers.step_all();
  }
  TESTASSERT_EQ(3, rlc2.get_buffer_state()); // t-StatusProhibit no longer running

  {
    // Double check status report
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, 3);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);       // 5 is the next expected SN.
    TESTASSERT_EQ(0, status_check.nacks.size()); // All PDUs are acked now
  }

  {
    // rlc2 should not issue further status PDUs as time passes (even after expiry of t_status_prohibit)
    int32_t checktime = 2 * rlc2_config.am_nr.t_status_prohibit;
    for (int cnt = 0; cnt < checktime; cnt++) {
      timers.step_all();
      TESTASSERT_EQ(0, rlc2.get_buffer_state());
    }
  }

  // Check statistics
  rlc_bearer_metrics_t metrics1 = rlc1.get_metrics();
  rlc_bearer_metrics_t metrics2 = rlc2.get_metrics();

  uint32_t total_tx_pdu_bytes1 = (NBUFS + 1) * data_pdu_size;                    // (NBUFS + 1 RETX) * PDU size
  uint32_t total_rx_pdu_bytes1 = 2 * status_pdu_ack_size + status_pdu_nack_size; // Two status PDU (one with a NACK)
  uint32_t total_tx_pdu_bytes2 =
      3 * status_pdu_ack_size + status_pdu_nack_size;   // Three status PDU (one with a NACK, two without)
  uint32_t total_rx_pdu_bytes2 = (NBUFS)*data_pdu_size; // (NBUFS - 1 Lost + 1 RETX) * PDU size

  // SDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_sdus);
  TESTASSERT_EQ(0, metrics1.num_rx_sdus);
  TESTASSERT_EQ(5, metrics1.num_tx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);
  // PDU metrics
  TESTASSERT_EQ(5 + 1, metrics1.num_tx_pdus);                    // One re-transmission
  TESTASSERT_EQ(2, metrics1.num_rx_pdus);                        // One status PDU
  TESTASSERT_EQ(total_tx_pdu_bytes1, metrics1.num_tx_pdu_bytes); // (NBUFS + 1 RETX) * PDU size
  TESTASSERT_EQ(total_rx_pdu_bytes1, metrics1.num_rx_pdu_bytes); // Two status PDU (one with a NACK)
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);                      // No lost SDUs

  // PDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_sdus);
  TESTASSERT_EQ(5, metrics2.num_rx_sdus);
  TESTASSERT_EQ(0, metrics2.num_tx_sdu_bytes);
  TESTASSERT_EQ(5, metrics2.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);
  // SDU metrics
  TESTASSERT_EQ(3, metrics2.num_tx_pdus);                        // Three status PDUs
  TESTASSERT_EQ(5, metrics2.num_rx_pdus);                        // 5 PDUs (6 tx'ed, but one was lost)
  TESTASSERT_EQ(total_tx_pdu_bytes2, metrics2.num_tx_pdu_bytes); // Three status PDU (one with a NACK, two without)
  TESTASSERT_EQ(total_rx_pdu_bytes2, metrics2.num_rx_pdu_bytes); // (NBUFS - 1 Lost + 1 RETX) * PDU size
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);                      // No lost SDUs
  return SRSRAN_SUCCESS;
}

/*
 * Test the loss of a single PDU with NACK duplicate
 * NACK should be visible in the status report.
 *
 * Retx after NACK should be present too.
 * No further status reports shall be issued.
 */
int lost_pdu_duplicated_nack_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  test_delimit_logger delimiter("lost PDU with NACK duplicate ({} bit SN)", to_number(sn_size));

  constexpr uint32_t payload_size        = 1;
  uint32_t           header_size         = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  uint32_t           data_pdu_size       = header_size + payload_size;
  uint32_t           expect_buffer_state = NBUFS * data_pdu_size;

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return -1;
  }

  rlc_config_t rlc2_config = rlc_config_t::default_rlc_am_nr_config(to_number(sn_size));
  if (not rlc2.configure(rlc2_config)) {
    return -1;
  }

  // after configuring entity
  TESTASSERT(0 == rlc1.get_buffer_state());

  basic_test_tx(&rlc1, pdu_bufs, sn_size);

  // Write 5 PDUs into RLC2
  for (int i = 0; i < NBUFS; i++) {
    if (i != 3) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes); // Don't write RLC_SN=3.
    }
  }

  // Only after t-reassembly has expired, will the status report include NACKs.
  TESTASSERT_EQ(3, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, 5);
    status_buf.N_bytes = len;

    TESTASSERT(0 == rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(3, status_check.ack_sn); // 3 is the next expected SN (i.e. the lost packet.)

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Write duplicated status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there is nothing pending in RLC1
    TESTASSERT_EQ(0, rlc1.get_buffer_state());
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // t-reassembly has expired. There should be a NACK in the status report.
  constexpr uint32_t status_pdu_ack_size  = 3;
  uint32_t           status_pdu_nack_size = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  TESTASSERT_EQ(status_pdu_ack_size + status_pdu_nack_size, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    uint32_t      len  = rlc2.read_pdu(status_buf.msg, status_pdu_ack_size + status_pdu_nack_size);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);           // 5 is the next expected SN.
    TESTASSERT_EQ(1, status_check.nacks.size());     // We lost one PDU.
    TESTASSERT_EQ(3, status_check.nacks[0].nack_sn); // Lost PDU SN=3.

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Write duplicated status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there is only one Retx of SN=3
    TESTASSERT_EQ(data_pdu_size, rlc1.get_buffer_state());
  }

  {
    // Check correct re-transmission
    byte_buffer_t retx_buf;
    uint32_t      len = rlc1.read_pdu(retx_buf.msg, data_pdu_size);
    retx_buf.N_bytes  = len;
    TESTASSERT_EQ(data_pdu_size, len);

    rlc2.write_pdu(retx_buf.msg, retx_buf.N_bytes);

    TESTASSERT_EQ(0, rlc2.get_buffer_state()); // Status report shoud be required, as the TX buffers are now empty.
  }

  // Step timers until t-StatusProhibit expires
  for (int cnt = 0; cnt < 8; cnt++) {
    timers.step_all();
  }
  TESTASSERT_EQ(3, rlc2.get_buffer_state()); // t-StatusProhibit no longer running

  {
    // Double check status report
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, 3);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);       // 5 is the next expected SN.
    TESTASSERT_EQ(0, status_check.nacks.size()); // All PDUs are acked now
  }

  {
    // rlc2 should not issue further status PDUs as time passes (even after expiry of t_status_prohibit)
    int32_t checktime = 2 * rlc2_config.am_nr.t_status_prohibit;
    for (int cnt = 0; cnt < checktime; cnt++) {
      timers.step_all();
      TESTASSERT_EQ(0, rlc2.get_buffer_state());
    }
  }

  // Check statistics
  rlc_bearer_metrics_t metrics1 = rlc1.get_metrics();
  rlc_bearer_metrics_t metrics2 = rlc2.get_metrics();

  uint32_t total_tx_pdu_bytes1 = (NBUFS + 1) * data_pdu_size;                        // (NBUFS + 1 RETX) * PDU size
  uint32_t total_rx_pdu_bytes1 = 4 * status_pdu_ack_size + 2 * status_pdu_nack_size; // 4 status PDU (2 with a NACK)
  uint32_t total_tx_pdu_bytes2 =
      3 * status_pdu_ack_size + status_pdu_nack_size;   // Three status PDU (one with a NACK, two without)
  uint32_t total_rx_pdu_bytes2 = (NBUFS)*data_pdu_size; // (NBUFS - 1 Lost + 1 RETX) * PDU size

  // SDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_sdus);
  TESTASSERT_EQ(0, metrics1.num_rx_sdus);
  TESTASSERT_EQ(5, metrics1.num_tx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);
  // PDU metrics
  TESTASSERT_EQ(5 + 1, metrics1.num_tx_pdus);                    // One re-transmission
  TESTASSERT_EQ(4, metrics1.num_rx_pdus);                        // 4 status PDUs
  TESTASSERT_EQ(total_tx_pdu_bytes1, metrics1.num_tx_pdu_bytes); // (NBUFS + 1 RETX) * PDU size
  TESTASSERT_EQ(total_rx_pdu_bytes1, metrics1.num_rx_pdu_bytes); // Two status PDU (one with a NACK)
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);                      // No lost SDUs

  // PDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_sdus);
  TESTASSERT_EQ(5, metrics2.num_rx_sdus);
  TESTASSERT_EQ(0, metrics2.num_tx_sdu_bytes);
  TESTASSERT_EQ(5, metrics2.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);
  // SDU metrics
  TESTASSERT_EQ(3, metrics2.num_tx_pdus);                        // Three status PDUs
  TESTASSERT_EQ(5, metrics2.num_rx_pdus);                        // 5 PDUs (6 tx'ed, but one was lost)
  TESTASSERT_EQ(total_tx_pdu_bytes2, metrics2.num_tx_pdu_bytes); // Three status PDU (one with a NACK, two without)
  TESTASSERT_EQ(total_rx_pdu_bytes2, metrics2.num_rx_pdu_bytes); // (NBUFS - 1 Lost + 1 RETX) * PDU size
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);                      // No lost SDUs
  return SRSRAN_SUCCESS;
}

/*
 * Test the loss of multiple PDUs.
 * NACKs for all missing PDUs should be visible in buffer state -- but we enforce
 * a trimmed status PDU by providing little space for the whole status PDU.
 * Retx after NACK should be present too.
 * Further status report shall contain the trimmed NACK.
 * Another Retx after NACK should be present.
 * No further status reports shall be issued.
 */
int lost_pdus_trimmed_nack_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  test_delimit_logger delimiter("lost PDUs and trimmed NACKs ({} bit SN)", to_number(sn_size));

  constexpr uint32_t payload_size        = 1;
  uint32_t           header_size         = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  uint32_t           data_pdu_size       = header_size + payload_size;
  uint32_t           expect_buffer_state = NBUFS * data_pdu_size;

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return -1;
  }

  rlc_config_t rlc2_config = rlc_config_t::default_rlc_am_nr_config(to_number(sn_size));
  if (not rlc2.configure(rlc2_config)) {
    return -1;
  }

  // after configuring entity
  TESTASSERT(0 == rlc1.get_buffer_state());

  basic_test_tx(&rlc1, pdu_bufs, sn_size);

  // Write 5 PDUs into RLC2
  for (int i = 0; i < NBUFS; i++) {
    if (i != 1 && i != 3) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes); // Don't write RLC_SN=1 and 3.
    }
  }

  // Only after t-reassembly has expired, will the status report include NACKs.
  TESTASSERT_EQ(3, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, 5);
    status_buf.N_bytes = len;

    TESTASSERT(0 == rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(1, status_check.ack_sn); // 1 is the next expected SN (i.e. the first lost packet.)

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // t-reassembly has expired. There should be two NACKs in the status report.
  constexpr uint32_t status_pdu_ack_size  = 3;
  uint32_t           status_pdu_nack_size = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  uint32_t           expected_size        = status_pdu_ack_size + 2 * status_pdu_nack_size;
  TESTASSERT_EQ(expected_size, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2, enforce to trimming by providing little space (expected_size - 1)
    // to drop the second NACK.
    byte_buffer_t status_buf;
    uint32_t      len  = rlc2.read_pdu(status_buf.msg, expected_size - 1);
    status_buf.N_bytes = len;
    expected_size      = status_pdu_ack_size + 1 * status_pdu_nack_size; // only one NACK left
    TESTASSERT_EQ(len, expected_size);

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(3, status_check.ack_sn);           // 3 is the next expected SN (after trimming)
    TESTASSERT_EQ(1, status_check.nacks.size());     // Expect only one NACK left
    TESTASSERT_EQ(1, status_check.nacks[0].nack_sn); // The NACK'ed SN is 1.

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there is an Retx of SN=1
    TESTASSERT_EQ(data_pdu_size, rlc1.get_buffer_state());
  }

  {
    // Check correct re-transmission
    byte_buffer_t retx_buf;
    uint32_t      len = rlc1.read_pdu(retx_buf.msg, data_pdu_size);
    retx_buf.N_bytes  = len;
    TESTASSERT_EQ(data_pdu_size, len);

    rlc2.write_pdu(retx_buf.msg, retx_buf.N_bytes);

    expected_size = status_pdu_ack_size + 1 * status_pdu_nack_size;
    TESTASSERT_EQ(0, rlc2.get_buffer_state()); // Status report should now include the chopped NACK
  }

  // Step timers until t-StatusProhibit expires
  for (int cnt = 0; cnt < 8; cnt++) {
    timers.step_all();
  }
  TESTASSERT_EQ(expected_size, rlc2.get_buffer_state()); // t-StatusProhibit no longer running

  {
    // Double check status report
    byte_buffer_t status_buf;
    uint32_t      len  = rlc2.read_pdu(status_buf.msg, expected_size);
    status_buf.N_bytes = len;
    expected_size      = status_pdu_ack_size + 1 * status_pdu_nack_size; // the remaining NACK
    TESTASSERT_EQ(len, expected_size);

    // Nothing else pending
    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);           // 5 is the next expected SN.
    TESTASSERT_EQ(1, status_check.nacks.size());     // Expect only the second NACK
    TESTASSERT_EQ(3, status_check.nacks[0].nack_sn); // The NACK'ed SN is 3.

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there is an Retx of SN=3
    TESTASSERT_EQ(data_pdu_size, rlc1.get_buffer_state());
  }
  {
    // Check correct re-transmission
    byte_buffer_t retx_buf;
    uint32_t      len = rlc1.read_pdu(retx_buf.msg, data_pdu_size);
    retx_buf.N_bytes  = len;
    TESTASSERT_EQ(data_pdu_size, len);

    rlc2.write_pdu(retx_buf.msg, retx_buf.N_bytes);

    expected_size = status_pdu_ack_size;
    TESTASSERT_EQ(0, rlc2.get_buffer_state()); // Status report should have no NACKs
  }
  // Step timers until t-StatusProhibit expires
  for (int cnt = 0; cnt < 8; cnt++) {
    timers.step_all();
  }
  TESTASSERT_EQ(expected_size, rlc2.get_buffer_state()); // t-StatusProhibit no longer running
  {
    // Double check status report
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, expected_size);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);       // 5 is the next expected SN.
    TESTASSERT_EQ(0, status_check.nacks.size()); // All PDUs are acked now
  }

  {
    // rlc2 should not issue further status PDUs as time passes (even after expiry of t_status_prohibit)
    int32_t checktime = 2 * rlc2_config.am_nr.t_status_prohibit;
    for (int cnt = 0; cnt < checktime; cnt++) {
      timers.step_all();
      TESTASSERT_EQ(0, rlc2.get_buffer_state());
    }
  }

  // Check statistics
  rlc_bearer_metrics_t metrics1 = rlc1.get_metrics();
  rlc_bearer_metrics_t metrics2 = rlc2.get_metrics();

  uint32_t total_tx_pdu_bytes1 = (NBUFS + 2) * data_pdu_size;                        // (NBUFS + 2 RETX) * PDU size
  uint32_t total_rx_pdu_bytes1 = 3 * status_pdu_ack_size + 2 * status_pdu_nack_size; // 3 status PDUs (2 with one NACK)
  uint32_t total_tx_pdu_bytes2 =
      4 * status_pdu_ack_size + 2 * status_pdu_nack_size; // 4 status PDUs (2 with one NACK, two without)
  uint32_t total_rx_pdu_bytes2 = (NBUFS)*data_pdu_size;   // (NBUFS - 2 Lost + 2 RETX) * PDU size

  // SDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_sdus);
  TESTASSERT_EQ(0, metrics1.num_rx_sdus);
  TESTASSERT_EQ(5, metrics1.num_tx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);
  // PDU metrics
  TESTASSERT_EQ(5 + 2, metrics1.num_tx_pdus);                    // One re-transmission
  TESTASSERT_EQ(3, metrics1.num_rx_pdus);                        // 3 status PDUs
  TESTASSERT_EQ(total_tx_pdu_bytes1, metrics1.num_tx_pdu_bytes); // (NBUFS + 2 RETX) * PDU size
  TESTASSERT_EQ(total_rx_pdu_bytes1, metrics1.num_rx_pdu_bytes); // Two status PDU (one with a NACK)
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);                      // No lost SDUs

  // PDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_sdus);
  TESTASSERT_EQ(5, metrics2.num_rx_sdus);
  TESTASSERT_EQ(0, metrics2.num_tx_sdu_bytes);
  TESTASSERT_EQ(5, metrics2.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);
  // SDU metrics
  TESTASSERT_EQ(4, metrics2.num_tx_pdus);                        // 4 status PDUs
  TESTASSERT_EQ(5, metrics2.num_rx_pdus);                        // 5 PDUs (7 tx'ed, but 2 were lost)
  TESTASSERT_EQ(total_tx_pdu_bytes2, metrics2.num_tx_pdu_bytes); // Three status PDU (one with a NACK, two without)
  TESTASSERT_EQ(total_rx_pdu_bytes2, metrics2.num_rx_pdu_bytes); // (NBUFS - 2 Lost + 2 RETX) * PDU size
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);                      // No lost SDUs
  return SRSRAN_SUCCESS;
}

/*
 * Test if retx queue is cleared of SDUs that are ACK'ed by a late/delayed ACK.
 */
int clean_retx_queue_of_acked_sdus_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  test_delimit_logger delimiter("Clean retx_queue of SDUs that are ACK'ed by a late/delayed ACK ({} bit SN)",
                                to_number(sn_size));

  constexpr uint32_t payload_size        = 1;
  uint32_t           header_size         = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  uint32_t           data_pdu_size       = header_size + payload_size;
  uint32_t           expect_buffer_state = NBUFS * data_pdu_size;

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return -1;
  }

  rlc_config_t rlc2_config = rlc_config_t::default_rlc_am_nr_config(to_number(sn_size));
  if (not rlc2.configure(rlc2_config)) {
    return -1;
  }

  rlc_am_nr_tx* rlc1_tx = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());

  // after configuring entity
  TESTASSERT(0 == rlc1.get_buffer_state());

  basic_test_tx(&rlc1, pdu_bufs, sn_size);

  // Write 5 PDUs into RLC2
  for (int i = 0; i < NBUFS; i++) {
    if (i != 3) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes); // Don't write RLC_SN=3.
    }
  }

  // Only after t-reassembly has expired, will the status report include NACKs.
  TESTASSERT_EQ(3, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, 5);
    status_buf.N_bytes = len;

    TESTASSERT(0 == rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(3, status_check.ack_sn); // 3 is the next expected SN (i.e. the lost packet.)

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there is nothing pending in RLC1
    TESTASSERT_EQ(0, rlc1.get_buffer_state());
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // t-reassembly has expired. There should be a NACK in the status report.
  constexpr uint32_t status_pdu_ack_size  = 3;
  uint32_t           status_pdu_nack_size = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  TESTASSERT_EQ(status_pdu_ack_size + status_pdu_nack_size, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    uint32_t      len  = rlc2.read_pdu(status_buf.msg, status_pdu_ack_size + status_pdu_nack_size);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);           // 5 is the next expected SN.
    TESTASSERT_EQ(1, status_check.nacks.size());     // We lost one PDU.
    TESTASSERT_EQ(3, status_check.nacks[0].nack_sn); // Lost PDU SN=3.

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there is only one Retx of SN=3
    TESTASSERT_EQ(data_pdu_size, rlc1.get_buffer_state());
  }

  // now we deliver the late PDU SN=3 to rlc2
  rlc2.write_pdu(pdu_bufs[3].msg, pdu_bufs[3].N_bytes);

  // Check there is only one Retx of SN=3
  TESTASSERT_EQ(data_pdu_size, rlc1.get_buffer_state());
  TESTASSERT_EQ(1, rlc1_tx->get_retx_queue_size());

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // t-reassembly has expired. There should be an ACK in the status report.
  TESTASSERT_EQ(status_pdu_ack_size, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    uint32_t      len  = rlc2.read_pdu(status_buf.msg, status_pdu_ack_size);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);       // 5 is the next expected SN.
    TESTASSERT_EQ(0, status_check.nacks.size()); // Nothing else lost

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check the Retx of SN=3 has been removed
    TESTASSERT_EQ(0, rlc1.get_buffer_state());
    TESTASSERT_EQ(0, rlc1_tx->get_retx_queue_size());
  }

  {
    // Attempt to read from rlc1 to verify there nothing to read from it
    byte_buffer_t retx_buf;
    uint32_t      len = rlc1.read_pdu(retx_buf.msg, data_pdu_size);
    retx_buf.N_bytes  = len;
    TESTASSERT_EQ(0, len);
  }

  {
    // rlc2 should not issue further status PDUs as time passes (even after expiry of t_status_prohibit)
    int32_t checktime = 2 * rlc2_config.am_nr.t_status_prohibit;
    for (int cnt = 0; cnt < checktime; cnt++) {
      timers.step_all();
      TESTASSERT_EQ(0, rlc2.get_buffer_state());
    }
  }

  // Check statistics
  rlc_bearer_metrics_t metrics1 = rlc1.get_metrics();
  rlc_bearer_metrics_t metrics2 = rlc2.get_metrics();

  uint32_t total_tx_pdu_bytes1 = (NBUFS)*data_pdu_size;                              // (NBUFS) * PDU size
  uint32_t total_rx_pdu_bytes1 = 3 * status_pdu_ack_size + 1 * status_pdu_nack_size; // 3 status PDU (1 with a NACK)
  uint32_t total_tx_pdu_bytes2 =
      3 * status_pdu_ack_size + status_pdu_nack_size;   // Three status PDU (one with a NACK, two without)
  uint32_t total_rx_pdu_bytes2 = (NBUFS)*data_pdu_size; // (NBUFS - 1 Lost + 1 Late) * PDU size

  // SDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_sdus);
  TESTASSERT_EQ(0, metrics1.num_rx_sdus);
  TESTASSERT_EQ(5, metrics1.num_tx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);
  // PDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_pdus);                        // 5 transmissions, no re-transmission
  TESTASSERT_EQ(3, metrics1.num_rx_pdus);                        // 3 status PDUs
  TESTASSERT_EQ(total_tx_pdu_bytes1, metrics1.num_tx_pdu_bytes); // (NBUFS) * PDU size
  TESTASSERT_EQ(total_rx_pdu_bytes1, metrics1.num_rx_pdu_bytes); // 3 status PDU (1 with a NACK)
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);                      // No lost SDUs

  // SDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_sdus);
  TESTASSERT_EQ(5, metrics2.num_rx_sdus);
  TESTASSERT_EQ(0, metrics2.num_tx_sdu_bytes);
  TESTASSERT_EQ(5, metrics2.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);
  // PDU metrics
  TESTASSERT_EQ(3, metrics2.num_tx_pdus);                        // 3 status PDUs
  TESTASSERT_EQ(5, metrics2.num_rx_pdus);                        // 5 transmissions, no re-transmission
  TESTASSERT_EQ(total_tx_pdu_bytes2, metrics2.num_tx_pdu_bytes); // Three status PDU (one with a NACK, two without)
  TESTASSERT_EQ(total_rx_pdu_bytes2, metrics2.num_rx_pdu_bytes); // (NBUFS - 1 Lost + 1 Late) * PDU size
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);                      // No lost SDUs
  return SRSRAN_SUCCESS;
}

/*
 * Test the basic segmentation of a single SDU.
 * A single SDU of 3 bytes is segmented into 3 PDUs
 */
int basic_segmentation_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester       tester(true, nullptr);
  timer_handler       timers(8);
  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("basic segmentation ({} bit SN)", to_number(sn_size));
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return -1;
  }

  // after configuring entity
  TESTASSERT_EQ(0, rlc1.get_buffer_state());

  // Push 1 SDU into RLC1
  unique_byte_buffer_t sdu;
  constexpr uint32_t   payload_size = 3; // Give the SDU the size of 3 bytes
  sdu                               = srsran::make_byte_buffer();
  TESTASSERT(nullptr != sdu);
  sdu->msg[0]     = 0;            // Write the index into the buffer
  sdu->N_bytes    = payload_size; // Give the SDU the size of 3 bytes
  sdu->md.pdcp_sn = 0;            // PDCP SN for notifications
  rlc1.write_sdu(std::move(sdu));

  // Read 3 PDUs
  constexpr uint16_t   n_pdus = 3;
  unique_byte_buffer_t pdu_bufs[n_pdus];
  uint32_t             header_size        = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  constexpr uint32_t   so_size            = 2;
  constexpr uint32_t   segment_size       = 1;
  uint32_t             pdu_size_first     = header_size + segment_size;
  uint32_t             pdu_size_continued = header_size + so_size + segment_size;
  for (int i = 0; i < n_pdus; i++) {
    pdu_bufs[i] = srsran::make_byte_buffer();
    TESTASSERT(nullptr != pdu_bufs[i]);
    if (i == 0) {
      pdu_bufs[i]->N_bytes = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_first);
      TESTASSERT_EQ(pdu_size_first, pdu_bufs[i]->N_bytes);
    } else {
      pdu_bufs[i]->N_bytes = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_continued);
      TESTASSERT_EQ(pdu_size_continued, pdu_bufs[i]->N_bytes);
    }
  }

  // Write 3 PDUs into RLC2
  for (int i = 0; i < n_pdus; i++) {
    rlc2.write_pdu(pdu_bufs[i]->msg, pdu_bufs[i]->N_bytes);
  }

  // Check statistics
  rlc_bearer_metrics_t metrics1 = rlc1.get_metrics();
  rlc_bearer_metrics_t metrics2 = rlc2.get_metrics();

  uint32_t total_rx_pdu_bytes = pdu_size_first + (n_pdus - 1) * pdu_size_continued; // 1 PDU (No SO) + 2 PDUs (with SO)

  // SDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_sdus);
  TESTASSERT_EQ(1, metrics2.num_rx_sdus);
  TESTASSERT_EQ(0, metrics2.num_tx_sdu_bytes);
  TESTASSERT_EQ(payload_size, metrics2.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);
  // PDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_pdus);
  TESTASSERT_EQ(n_pdus, metrics2.num_rx_pdus); // 3 PDUs
  TESTASSERT_EQ(0, metrics2.num_tx_pdu_bytes);
  TESTASSERT_EQ(total_rx_pdu_bytes, metrics2.num_rx_pdu_bytes); // 1 PDU (No SO) + 2 PDUs (with SO)
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);                     // No lost SDUs

  // Check state
  rlc_am_nr_tx_state_t state1_tx = tx1->get_tx_state();
  TESTASSERT_EQ(1, state1_tx.tx_next);

  return SRSRAN_SUCCESS;
}

// This tests correct behaviour of the following flow:
// - Transmit 5 SDUs as whole PDUs
// - Loose 3rd PDU
// - Receive NACK for missing PDU
// - Retransmit lost PDU in 3 segments
// - Check metrics and state
int segment_retx_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  test_delimit_logger delimiter("segment retx PDU ({} bit SN)", to_number(sn_size));

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  auto rlc_cnfg              = rlc_config_t::default_rlc_am_nr_config(to_number(sn_size));
  rlc_cnfg.am_nr.t_poll_retx = -1;
  if (not rlc1.configure(rlc_cnfg)) {
    return -1;
  }

  if (not rlc2.configure(rlc_cnfg)) {
    return -1;
  }

  // after configuring entity
  TESTASSERT_EQ(0, rlc1.get_buffer_state());

  // Push 5 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[NBUFS];
  constexpr uint32_t   payload_size = 3; // Give the SDU the size of 3 bytes
  uint32_t             header_size  = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i;            // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = payload_size; // Give each buffer a size of 3 bytes
    sdu_bufs[i]->md.pdcp_sn = i;            // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  uint32_t expected_buffer_state = (header_size + payload_size) * NBUFS;
  TESTASSERT_EQ(expected_buffer_state, rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (1 byte each)
  for (int i = 0; i < NBUFS; i++) {
    uint32_t len        = rlc1.read_pdu(pdu_bufs[i].msg, header_size + payload_size);
    pdu_bufs[i].N_bytes = len;
    TESTASSERT_EQ(header_size + payload_size, len);
  }

  TESTASSERT_EQ(0, rlc1.get_buffer_state());

  // Write 5 PDUs into RLC2
  for (int i = 0; i < NBUFS; i++) {
    if (i != 3) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes); // Don't write RLC_SN=3.
    }
  }

  // Only after t-reassembly has expired, will the status report include NACKs.
  TESTASSERT_EQ(3, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, 5);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(3, status_check.ack_sn); // 3 is the next expected SN (i.e. the lost packet.)

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // t-reassembly has expired. There should be a NACK in the status report.
  constexpr uint32_t status_pdu_ack_size  = 3;
  uint32_t           status_pdu_nack_size = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  TESTASSERT_EQ(status_pdu_ack_size + status_pdu_nack_size, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, status_pdu_ack_size + status_pdu_nack_size);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);           // 5 is the next expected SN.
    TESTASSERT_EQ(1, status_check.nacks.size());     // We lost one PDU.
    TESTASSERT_EQ(3, status_check.nacks[0].nack_sn); // Lost PDU SN=3.

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there is an Retx of SN=3
    TESTASSERT_EQ(header_size + payload_size, rlc1.get_buffer_state());
  }

  constexpr uint32_t so_size            = 2;
  constexpr uint32_t segment_size       = 1;
  uint32_t           pdu_size_first     = header_size + segment_size;
  uint32_t           pdu_size_continued = header_size + so_size + segment_size;
  {
    // Re-transmit PDU in 3 segments
    for (int i = 0; i < 3; i++) {
      byte_buffer_t retx_buf;
      uint32_t      len = 0;
      if (i == 0) {
        len = rlc1.read_pdu(retx_buf.msg, pdu_size_first);
        TESTASSERT_EQ(pdu_size_first, len);
      } else {
        len = rlc1.read_pdu(retx_buf.msg, pdu_size_continued);
        TESTASSERT_EQ(pdu_size_continued, len);
      }
      retx_buf.N_bytes = len;

      rlc_am_nr_pdu_header_t header_check = {};
      uint32_t               hdr_len      = rlc_am_nr_read_data_pdu_header(&retx_buf, sn_size, &header_check);
      // Double check header.
      TESTASSERT_EQ(3, header_check.sn); // Double check RETX SN
      if (i == 0) {
        TESTASSERT_EQ(rlc_nr_si_field_t::first_segment, header_check.si);
      } else if (i == 1) {
        TESTASSERT_EQ(rlc_nr_si_field_t::neither_first_nor_last_segment, header_check.si);
      } else {
        TESTASSERT_EQ(rlc_nr_si_field_t::last_segment, header_check.si);
      }

      rlc2.write_pdu(retx_buf.msg, retx_buf.N_bytes);
    }
    TESTASSERT(0 == rlc1.get_buffer_state());
  }

  // Check statistics
  rlc_bearer_metrics_t metrics1 = rlc1.get_metrics();
  rlc_bearer_metrics_t metrics2 = rlc2.get_metrics();

  uint32_t data_pdu_size       = header_size + payload_size;
  uint32_t total_tx_pdu_bytes1 = NBUFS * data_pdu_size + pdu_size_first + 2 * pdu_size_continued;
  uint32_t total_rx_pdu_bytes1 = 2 * status_pdu_ack_size + status_pdu_nack_size; // Two status PDU (one with a NACK)
  uint32_t total_tx_pdu_bytes2 = total_rx_pdu_bytes1;
  uint32_t total_rx_pdu_bytes2 = (NBUFS - 1) * data_pdu_size + pdu_size_first + 2 * pdu_size_continued;

  // SDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_sdus);
  TESTASSERT_EQ(0, metrics1.num_rx_sdus);
  TESTASSERT_EQ(15, metrics1.num_tx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);
  // PDU metrics
  TESTASSERT_EQ(5 + 3, metrics1.num_tx_pdus); // 3 re-transmissions
  TESTASSERT_EQ(2, metrics1.num_rx_pdus);     // Two status PDU
  TESTASSERT_EQ(total_tx_pdu_bytes1, metrics1.num_tx_pdu_bytes);
  TESTASSERT_EQ(total_rx_pdu_bytes1, metrics1.num_rx_pdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_lost_sdus); // No lost SDUs

  // PDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_sdus);
  TESTASSERT_EQ(5, metrics2.num_rx_sdus);
  TESTASSERT_EQ(0, metrics2.num_tx_sdu_bytes);
  TESTASSERT_EQ(15, metrics2.num_rx_sdu_bytes); // 5 SDUs, 3 bytes each
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);
  // SDU metrics
  TESTASSERT_EQ(2, metrics2.num_tx_pdus); // Two status PDUs
  TESTASSERT_EQ(7, metrics2.num_rx_pdus); // 7 PDUs (8 tx'ed, but one was lost)
  TESTASSERT_EQ(total_tx_pdu_bytes2, metrics2.num_tx_pdu_bytes);
  TESTASSERT_EQ(total_rx_pdu_bytes2,
                metrics2.num_rx_pdu_bytes); // 2 Bytes * (NBUFFS-1) (header size) + (NBUFFS-1) * 3 (data)
                                            // 3 (1 retx no SO) + 2 * 5 (2 retx with SO) = 33
  TESTASSERT_EQ(0, metrics2.num_lost_sdus); // No lost SDUs

  // Check state
  rlc_am_nr_rx_state_t state2_rx = rx2->get_rx_state();
  TESTASSERT_EQ(5, state2_rx.rx_next);
  return SRSRAN_SUCCESS;
}

// This tests correct behaviour of the following flow:
// - Transmit 5 SDUs as whole PDUs
// - Loose 3rd PDU
// - Receive NACK for missing PDU
// - Retransmit lost PDU in 3 segments
// - Loose first and last segment
// - Receive NACKs for missing segments
// - Receive duplicate of previous NACKs
// - Retransmit missing segments again, but only once!
// - Check metrics and state
int segment_retx_and_loose_segments_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  test_delimit_logger delimiter("segment retx PDU and loose some segments ({} bit SN)", to_number(sn_size));

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return -1;
  }

  // after configuring entity
  TESTASSERT_EQ(0, rlc1.get_buffer_state());

  // Push 5 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[NBUFS];
  constexpr uint32_t   payload_size = 3; // Give the SDU the size of 3 bytes
  uint32_t             header_size  = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i;            // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = payload_size; // Give each buffer a size of 3 bytes
    sdu_bufs[i]->md.pdcp_sn = i;            // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  uint32_t expected_buffer_state = (header_size + payload_size) * NBUFS;
  TESTASSERT_EQ(expected_buffer_state, rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (each with a full SDU)
  for (int i = 0; i < NBUFS; i++) {
    uint32_t len        = rlc1.read_pdu(pdu_bufs[i].msg, header_size + payload_size);
    pdu_bufs[i].N_bytes = len;
    TESTASSERT_EQ(header_size + payload_size, len);
  }

  TESTASSERT_EQ(0, rlc1.get_buffer_state());

  // Write 5 - 1 PDUs into RLC2
  for (int i = 0; i < NBUFS; i++) {
    if (i != 3) {
      rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes); // Don't write RLC_SN=3.
    }
  }

  // Only after t-reassembly has expired, will the status report include NACKs.
  TESTASSERT_EQ(3, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, 5);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(3, status_check.ack_sn); // 3 is the next expected SN (i.e. the lost packet.)

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // t-reassembly has expired. There should be a NACK in the status report.
  constexpr uint32_t status_pdu_ack_size  = 3;
  uint32_t           status_pdu_nack_size = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  TESTASSERT_EQ(status_pdu_ack_size + status_pdu_nack_size, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, status_pdu_ack_size + status_pdu_nack_size);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);           // 5 is the next expected SN.
    TESTASSERT_EQ(1, status_check.nacks.size());     // We lost one PDU.
    TESTASSERT_EQ(3, status_check.nacks[0].nack_sn); // Lost PDU SN=3.

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there is an Retx of SN=3
    TESTASSERT_EQ(header_size + payload_size, rlc1.get_buffer_state());
  }

  constexpr uint32_t so_size            = 2;
  constexpr uint32_t segment_size       = 1;
  uint32_t           pdu_size_first     = header_size + segment_size;
  uint32_t           pdu_size_continued = header_size + so_size + segment_size;
  {
    // Re-transmit PDU in 3 segments
    for (int i = 0; i < 3; i++) {
      byte_buffer_t retx_buf;
      uint32_t      len = 0;
      if (i == 0) {
        len = rlc1.read_pdu(retx_buf.msg, pdu_size_first);
        TESTASSERT_EQ(pdu_size_first, len);
      } else {
        len = rlc1.read_pdu(retx_buf.msg, pdu_size_continued);
        TESTASSERT_EQ(pdu_size_continued, len);
      }
      retx_buf.N_bytes = len;

      rlc_am_nr_pdu_header_t header_check = {};
      uint32_t               hdr_len      = rlc_am_nr_read_data_pdu_header(&retx_buf, sn_size, &header_check);
      // Double check header.
      TESTASSERT_EQ(3, header_check.sn); // Double check RETX SN
      if (i == 0) {
        TESTASSERT_EQ(rlc_nr_si_field_t::first_segment, header_check.si);
      } else if (i == 1) {
        TESTASSERT_EQ(rlc_nr_si_field_t::neither_first_nor_last_segment, header_check.si);
      } else {
        TESTASSERT_EQ(rlc_nr_si_field_t::last_segment, header_check.si);
      }

      // We loose the first and the last segment
      if (i != 0 && i != 2) {
        rlc2.write_pdu(retx_buf.msg, retx_buf.N_bytes);
      }
    }
    TESTASSERT(0 == rlc1.get_buffer_state());
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // t-reassembly has expired. There should be another NACK in the status report.
  constexpr uint32_t status_pdu_so_size = 4;
  TESTASSERT_EQ(status_pdu_ack_size + 2 * status_pdu_nack_size + 2 * status_pdu_so_size, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int len = rlc2.read_pdu(status_buf.msg, status_pdu_ack_size + 2 * status_pdu_nack_size + 2 * status_pdu_so_size);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);               // 5 is the next expected SN.
    TESTASSERT_EQ(2, status_check.nacks.size());         // We lost two PDU segments.
    TESTASSERT_EQ(3, status_check.nacks[0].nack_sn);     // Lost PDU SN=3.
    TESTASSERT_EQ(true, status_check.nacks[0].has_so);   // This is a segment missing.
    TESTASSERT_EQ(0, status_check.nacks[0].so_start);    // Segment offset should be 0 here
    TESTASSERT_EQ(0, status_check.nacks[0].so_end);      // Segment end should be 0 here
    TESTASSERT_EQ(true, status_check.nacks[1].has_so);   // This is a segment missing.
    TESTASSERT_EQ(2, status_check.nacks[1].so_start);    // Segment offset should be 2 here
    TESTASSERT_EQ(0xFFFF, status_check.nacks[1].so_end); // Segment end should be 0xFFFF here

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Write status PDU duplicate to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there are two Retx segments (a first one and a continued one)
    TESTASSERT_EQ(pdu_size_first + pdu_size_continued, rlc1.get_buffer_state());
  }

  {
    // Re-transmit the lost 2 segments
    for (int i = 0; i < 2; i++) {
      byte_buffer_t retx_buf;
      uint32_t      len = 0;
      if (i == 0) {
        len = rlc1.read_pdu(retx_buf.msg, pdu_size_first);
        TESTASSERT_EQ(pdu_size_first, len);
      } else {
        len = rlc1.read_pdu(retx_buf.msg, pdu_size_continued);
        TESTASSERT_EQ(pdu_size_continued, len);
      }
      retx_buf.N_bytes = len;

      rlc_am_nr_pdu_header_t header_check = {};
      uint32_t               hdr_len      = rlc_am_nr_read_data_pdu_header(&retx_buf, sn_size, &header_check);
      // Double check header.
      TESTASSERT_EQ(3, header_check.sn); // Double check RETX SN
      if (i == 0) {
        TESTASSERT_EQ(rlc_nr_si_field_t::first_segment, header_check.si);
      } else {
        TESTASSERT_EQ(rlc_nr_si_field_t::last_segment, header_check.si);
      }

      rlc2.write_pdu(retx_buf.msg, retx_buf.N_bytes);
    }
    TESTASSERT(0 == rlc1.get_buffer_state());
  }

  // Check statistics
  rlc_bearer_metrics_t metrics1 = rlc1.get_metrics();
  rlc_bearer_metrics_t metrics2 = rlc2.get_metrics();

  uint32_t data_pdu_size       = header_size + payload_size;
  uint32_t total_tx_pdu_bytes1 = NBUFS * data_pdu_size + 2 * pdu_size_first + 3 * pdu_size_continued;
  uint32_t total_rx_pdu_bytes1 = status_pdu_ack_size +                                 // ACK, no NACK
                                 (status_pdu_ack_size + status_pdu_nack_size) +        // ACK + NACK full SDU
                                 2 * (status_pdu_ack_size + 2 * status_pdu_nack_size + // 2 * (ACK + NACK two segments)
                                      2 * status_pdu_so_size);
  uint32_t total_tx_pdu_bytes2 = status_pdu_ack_size +                                 // ACK, no NACK
                                 (status_pdu_ack_size + status_pdu_nack_size) +        // ACK + NACK full SDU
                                 1 * (status_pdu_ack_size + 2 * status_pdu_nack_size + // 1 * (ACK + NACK two segments)
                                      2 * status_pdu_so_size);
  uint32_t total_rx_pdu_bytes2 = (NBUFS - 1) * data_pdu_size + pdu_size_first + 2 * pdu_size_continued;

  // SDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_sdus);
  TESTASSERT_EQ(0, metrics1.num_rx_sdus);
  TESTASSERT_EQ(15, metrics1.num_tx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);
  // PDU metrics
  TESTASSERT_EQ(5 + 3 + 2, metrics1.num_tx_pdus); // 5 + (3 + 2) re-transmissions
  TESTASSERT_EQ(4, metrics1.num_rx_pdus);         // 4 status PDU
  TESTASSERT_EQ(total_tx_pdu_bytes1, metrics1.num_tx_pdu_bytes);
  TESTASSERT_EQ(total_rx_pdu_bytes1, metrics1.num_rx_pdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_lost_sdus); // No lost SDUs

  // PDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_sdus);
  TESTASSERT_EQ(5, metrics2.num_rx_sdus);
  TESTASSERT_EQ(0, metrics2.num_tx_sdu_bytes);
  TESTASSERT_EQ(15, metrics2.num_rx_sdu_bytes); // 5 SDUs, 3 bytes each
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);
  // SDU metrics
  TESTASSERT_EQ(3, metrics2.num_tx_pdus); // 3 status PDUs
  TESTASSERT_EQ(7, metrics2.num_rx_pdus); // 7 PDUs (10 tx'ed, but 3 were lost)
  TESTASSERT_EQ(total_tx_pdu_bytes2, metrics2.num_tx_pdu_bytes);
  TESTASSERT_EQ(total_rx_pdu_bytes2,
                metrics2.num_rx_pdu_bytes); // 2 Bytes * (NBUFFS-1) (header size) + (NBUFFS-1) * 3 (data)
                                            // 3 (1 retx no SO) + 2 * 5 (2 retx with SO) = 33
  TESTASSERT_EQ(0, metrics2.num_lost_sdus); // No lost SDUs

  // Check state
  rlc_am_nr_rx_state_t state2_rx = rx2->get_rx_state();
  TESTASSERT_EQ(5, state2_rx.rx_next);
  return SRSRAN_SUCCESS;
}

int retx_segment_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  std::string         str = "retx segment PDU (" + std::to_string(to_number(sn_size)) + " bit SN)";
  test_delimit_logger delimiter(str.c_str());

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  auto rlc_cnfg              = rlc_config_t::default_rlc_am_nr_config(to_number(sn_size));
  rlc_cnfg.am_nr.t_poll_retx = -1;
  if (not rlc1.configure(rlc_cnfg)) {
    return -1;
  }

  if (not rlc2.configure(rlc_cnfg)) {
    return -1;
  }

  // after configuring entity
  TESTASSERT(0 == rlc1.get_buffer_state());

  int n_sdu_bufs = 5;
  int n_pdu_bufs = 15;

  // Push 5 SDUs into RLC1
  std::vector<unique_byte_buffer_t> sdu_bufs(n_sdu_bufs);
  constexpr uint32_t                payload_size = 3; // Give the SDU the size of 3 bytes
  uint32_t                          header_size  = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  for (int i = 0; i < n_sdu_bufs; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i;            // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = payload_size; // Give each buffer a size of 3 bytes
    sdu_bufs[i]->md.pdcp_sn = i;            // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  uint32_t expected_buffer_state = (header_size + payload_size) * n_sdu_bufs;
  TESTASSERT_EQ(expected_buffer_state, rlc1.get_buffer_state());

  constexpr uint32_t so_size            = 2;
  constexpr uint32_t segment_size       = 1;
  uint32_t           pdu_size_first     = header_size + segment_size;
  uint32_t           pdu_size_continued = header_size + so_size + segment_size;

  // Read 15 PDUs from RLC1
  std::vector<unique_byte_buffer_t> pdu_bufs(n_pdu_bufs);
  for (int i = 0; i < n_pdu_bufs; i++) {
    // First also test buffer state
    uint32_t remaining_total_bytes = (payload_size * n_sdu_bufs) - (i * segment_size);
    uint32_t remaining_full_sdus   = remaining_total_bytes / payload_size;
    uint32_t remaining_seg_bytes   = remaining_total_bytes % payload_size;

    uint32_t buffer_state_full_sdus = (header_size + payload_size) * remaining_full_sdus;
    uint32_t buffer_state_seg_sdu   = remaining_seg_bytes == 0 ? 0 : (header_size + so_size + remaining_seg_bytes);
    expected_buffer_state           = buffer_state_full_sdus + buffer_state_seg_sdu;
    TESTASSERT_EQ(expected_buffer_state, rlc1.get_buffer_state());

    pdu_bufs[i] = srsran::make_byte_buffer();
    if (i == 0 || i == 3 || i == 6 || i == 9 || i == 12) {
      // First segment, no SO
      uint32_t len         = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_first); // 2 bytes for header + 1 byte payload
      pdu_bufs[i]->N_bytes = len;
      TESTASSERT_EQ(pdu_size_first, len);
    } else {
      // Middle or last segment, SO present
      uint32_t len         = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_continued); // 4 bytes for header + 1 byte payload
      pdu_bufs[i]->N_bytes = len;
      TESTASSERT_EQ(pdu_size_continued, len);
    }
  }

  TESTASSERT_EQ(0, rlc1.get_buffer_state());

  // Write 15 - 3 PDUs into RLC2
  for (int i = 0; i < n_pdu_bufs; i++) {
    if (i != 3 && i != 7 && i != 11) {
      // Lose first segment of RLC_SN=1.
      // Lose middle segment of RLC_SN=2.
      // Lose last segment of RLC_SN=3.
      rlc2.write_pdu(pdu_bufs[i]->msg, pdu_bufs[i]->N_bytes);
    }
  }

  {
    // Double check rx state
    rlc_am_nr_rx_state_t st = rx2->get_rx_state();
    TESTASSERT_EQ(1, st.rx_next);
    TESTASSERT_EQ(1, st.rx_highest_status);
    TESTASSERT_EQ(2, st.rx_next_status_trigger); // Rx_Next_Highest + 1, when the t-Reordering was started
    TESTASSERT_EQ(5, st.rx_next_highest);        // Highest SN received + 1
  }

  // Only after t-reassembly has expired, will the status report include NACKs.
  // RX_Highest_Status will be updated to to the SN
  // of the first RLC SDU with SN >= RX_Next_Status_Trigger
  TESTASSERT_EQ(3, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, 5);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(1, status_check.ack_sn); // 1 is the next expected SN (i.e. the first lost packet.)

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // After the t-Reassembly expires:
  // - RX_Highest_Status is updated to the SN of the first RLC SDU with SN >= RX_Next_Status_Trigger, i.e., SN=2
  // - Because RX_Next_Highest> RX_Highest_Status +1:
  //   - t-Reassembly is restarted, and
  //   - RX_Next_Status_Trigger is set to RX_Next_Highest.
  {
    // Double check rx state
    rlc_am_nr_rx_state_t st = rx2->get_rx_state();
    TESTASSERT_EQ(1, st.rx_next);
    TESTASSERT_EQ(2, st.rx_highest_status);
    TESTASSERT_EQ(5, st.rx_next_status_trigger); // Rx_Next_Highest + 1, when the t-Reassembly was started
    TESTASSERT_EQ(5, st.rx_next_highest);        // Highest SN received + 1
  }

  // t-reassembly has expired. Becuse RX_Highest_Status is 2
  // There should be an ACK of SN=2 and a NACK of SN=1
  constexpr uint32_t status_pdu_ack_size  = 3;
  uint32_t           status_pdu_nack_size = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  constexpr uint32_t status_pdu_so_size   = 4;
  TESTASSERT_EQ(status_pdu_ack_size + status_pdu_nack_size + status_pdu_so_size,
                rlc2.get_buffer_state()); // 3 bytes for fixed header (ACK+E1) + 6 for NACK with SO = 9.
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, status_pdu_ack_size + status_pdu_nack_size + status_pdu_so_size);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(2, status_check.ack_sn);             // 5 is the next expected SN.
    TESTASSERT_EQ(1, status_check.nacks.size());       // We lost one PDU.
    TESTASSERT_EQ(1, status_check.nacks[0].nack_sn);   // Lost SDU on SN=1.
    TESTASSERT_EQ(true, status_check.nacks[0].has_so); // It's a segment.
    TESTASSERT_EQ(0, status_check.nacks[0].so_start);  // First byte missing is 0.
    TESTASSERT_EQ(0, status_check.nacks[0].so_end);    // Last byte of the segment.
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // After the t-Reassembly expires:
  // - RX_Highest_Status is updated to the SN of the first RLC SDU with SN >= RX_Next_Status_Trigger, i.e., SN=2
  // - Because RX_Next_Highest> RX_Highest_Status +1:
  //   - t-Reassembly is restarted, and
  //   - RX_Next_Status_Trigger is set to RX_Next_Highest.
  {
    // Double check rx state
    rlc_am_nr_rx_state_t st = rx2->get_rx_state();
    TESTASSERT_EQ(1, st.rx_next);
    TESTASSERT_EQ(5, st.rx_highest_status);
    TESTASSERT_EQ(5, st.rx_next_status_trigger); // Rx_Next_Highest + 1, when the t-Reordering was started
    TESTASSERT_EQ(5, st.rx_next_highest);        // Highest SN received + 1
  }

  // t-reassembly has expired. There should be a NACK in the status report.
  // There should be 3 NACKs with SO_start and SO_end
  TESTASSERT_EQ(status_pdu_ack_size + 3 * (status_pdu_nack_size + status_pdu_so_size), rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int len = rlc2.read_pdu(status_buf.msg, status_pdu_ack_size + 3 * (status_pdu_nack_size + status_pdu_so_size));
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check(sn_size);
    rlc_am_nr_read_status_pdu(&status_buf, sn_size, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);               // 5 is the next expected SN.
    TESTASSERT_EQ(3, status_check.nacks.size());         // We lost one PDU.
    TESTASSERT_EQ(1, status_check.nacks[0].nack_sn);     // Lost SDU on SN=1.
    TESTASSERT_EQ(true, status_check.nacks[0].has_so);   // Lost SDU on SN=1.
    TESTASSERT_EQ(0, status_check.nacks[0].so_start);    // Lost SDU on SN=1.
    TESTASSERT_EQ(0, status_check.nacks[0].so_end);      // Lost SDU on SN=1.
    TESTASSERT_EQ(2, status_check.nacks[1].nack_sn);     // Lost SDU on SN=1.
    TESTASSERT_EQ(true, status_check.nacks[1].has_so);   // Lost SDU on SN=1.
    TESTASSERT_EQ(1, status_check.nacks[1].so_start);    // Lost SDU on SN=1.
    TESTASSERT_EQ(1, status_check.nacks[1].so_end);      // Lost SDU on SN=1.
    TESTASSERT_EQ(3, status_check.nacks[2].nack_sn);     // Lost SDU on SN=1.
    TESTASSERT_EQ(true, status_check.nacks[2].has_so);   // Lost SDU on SN=1.
    TESTASSERT_EQ(2, status_check.nacks[2].so_start);    // Lost SDU on SN=1.
    TESTASSERT_EQ(0xFFFF, status_check.nacks[2].so_end); // Lost SDU on SN=1.

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there are 3 Retx segments (a first one and two continued ones)
    TESTASSERT_EQ(pdu_size_first + 2 * pdu_size_continued, rlc1.get_buffer_state());
  }

  {
    // Re-transmit the 3 lost segments
    for (int i = 0; i < 3; i++) {
      // First also test buffer state
      uint32_t remaining_segments = 3 - i;
      expected_buffer_state       = remaining_segments * (header_size + so_size + segment_size);
      if (i == 0) { // subtract so_size, because in this setup the first retx is a "first_segment" without SO.
        expected_buffer_state -= so_size;
      }
      TESTASSERT_EQ(expected_buffer_state, rlc1.get_buffer_state());

      byte_buffer_t retx_buf;
      uint32_t      len = 0;
      if (i == 0) {
        len = rlc1.read_pdu(retx_buf.msg, pdu_size_first);
        TESTASSERT_EQ(pdu_size_first, len);
      } else {
        len = rlc1.read_pdu(retx_buf.msg, pdu_size_continued);
        TESTASSERT_EQ(pdu_size_continued, len);
      }
      retx_buf.N_bytes = len;

      rlc_am_nr_pdu_header_t header_check = {};
      uint32_t               hdr_len      = rlc_am_nr_read_data_pdu_header(&retx_buf, sn_size, &header_check);
      // Double check header.
      if (i == 0) {
        TESTASSERT_EQ(1, header_check.sn); // Double check RETX SN
        TESTASSERT_EQ(rlc_nr_si_field_t::first_segment, header_check.si);
      } else if (i == 1) {
        TESTASSERT_EQ(2, header_check.sn); // Double check RETX SN
        TESTASSERT_EQ(rlc_nr_si_field_t::neither_first_nor_last_segment, header_check.si);
      } else {
        TESTASSERT_EQ(3, header_check.sn); // Double check RETX SN
        TESTASSERT_EQ(rlc_nr_si_field_t::last_segment, header_check.si);
      }

      rlc2.write_pdu(retx_buf.msg, retx_buf.N_bytes);
    }
    TESTASSERT_EQ(0, rlc1.get_buffer_state());
  }

  // Check statistics
  rlc_bearer_metrics_t metrics1 = rlc1.get_metrics();
  rlc_bearer_metrics_t metrics2 = rlc2.get_metrics();

  uint32_t data_pdu_size       = header_size + payload_size;
  uint32_t total_tx_pdu_bytes1 = 5 * pdu_size_first + 10 * pdu_size_continued + pdu_size_first + 2 * pdu_size_continued;
  uint32_t total_rx_pdu_bytes1 = 2 * status_pdu_ack_size + 3 * (status_pdu_nack_size + status_pdu_so_size);
  uint32_t total_tx_pdu_bytes2 = 3 * status_pdu_ack_size + 4 * (status_pdu_nack_size + status_pdu_so_size);
  uint32_t total_rx_pdu_bytes2 = 4 * pdu_size_first + 8 * pdu_size_continued + pdu_size_first + 2 * pdu_size_continued;

  // SDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_sdus);
  TESTASSERT_EQ(0, metrics1.num_rx_sdus);
  TESTASSERT_EQ(15, metrics1.num_tx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);

  // PDU metrics
  TESTASSERT_EQ(15 + 3, metrics1.num_tx_pdus); // 15 PDUs + 3 re-transmissions
  TESTASSERT_EQ(2, metrics1.num_rx_pdus);      // Two status PDU
  TESTASSERT_EQ(total_tx_pdu_bytes1,
                metrics1.num_tx_pdu_bytes); // 3 Bytes * 5 (5 PDUs without SO) + 10 * 5 (10 PDUs with SO)
                                            // 3 (1 retx no SO) + 2 * 5 (2 retx with SO) = 78
  TESTASSERT_EQ(total_rx_pdu_bytes1,
                metrics1.num_rx_pdu_bytes); // Two status PDU. One with just an ack (3 bytes)
                                            // Another with 3 NACKs all with SO. (3 + 3*6 bytes) = 24
  TESTASSERT_EQ(0, metrics1.num_lost_sdus); // No lost SDUs

  // PDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_sdus);
  TESTASSERT_EQ(5, metrics2.num_rx_sdus);
  TESTASSERT_EQ(0, metrics2.num_tx_sdu_bytes);
  TESTASSERT_EQ(15, metrics2.num_rx_sdu_bytes); // 5 SDUs, 3 bytes each
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);
  // SDU metrics
  TESTASSERT_EQ(3, metrics2.num_tx_pdus);   // 3 status PDUs
  TESTASSERT_EQ(15, metrics2.num_rx_pdus);  // 15 PDUs (18 tx'ed, but three were lost)
  TESTASSERT_EQ(total_tx_pdu_bytes2,        // Three status PDU. One with just an ack
                metrics2.num_tx_pdu_bytes); // Another with 1 NACK with SO.
                                            // Another with 3 NACKs all with SO.
  TESTASSERT_EQ(total_rx_pdu_bytes2,        // 3 Bytes (header + data size, without SO) * 5 (N PDUs without SO)
                metrics2.num_rx_pdu_bytes); // 5 bytes (header + data size, with SO) * 10 (N PDUs with SO)
                                            // = 81 bytes
  TESTASSERT_EQ(0, metrics2.num_lost_sdus); // No lost SDUs

  // Check state
  rlc_am_nr_rx_state_t state2_rx = rx2->get_rx_state();
  TESTASSERT_EQ(5, state2_rx.rx_next);

  return SRSRAN_SUCCESS;
}

// We only increment TX_NEXT after transmitting the last segment of a SDU
// This means that we need to handle status reports where ACK_SN may be larger
// than TX_NEXT, as it may contain a NACK for the partially transmitted PDU with
// SN==TX_NEXT.
int handle_status_of_non_tx_last_segment(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester       tester(true, nullptr);
  timer_handler       timers(8);
  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("basic segmentation ({} bit SN)", to_number(sn_size));
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return -1;
  }

  // after configuring entity
  TESTASSERT_EQ(0, rlc1.get_buffer_state());

  // Push 1 SDU into RLC1
  unique_byte_buffer_t sdu;
  constexpr uint32_t   payload_size = 3; // Give the SDU the size of 3 bytes
  sdu                               = srsran::make_byte_buffer();
  TESTASSERT(nullptr != sdu);
  sdu->msg[0]     = 0;            // Write the index into the buffer
  sdu->N_bytes    = payload_size; // Give the SDU the size of 3 bytes
  sdu->md.pdcp_sn = 0;            // PDCP SN for notifications
  rlc1.write_sdu(std::move(sdu));

  // Read 2 PDUs. Leave last one in the tx_window.
  constexpr uint16_t   n_pdus = 2;
  unique_byte_buffer_t pdu_bufs[n_pdus];
  uint32_t             header_size        = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  constexpr uint32_t   so_size            = 2;
  constexpr uint32_t   segment_size       = 1;
  uint32_t             pdu_size_first     = header_size + segment_size;
  uint32_t             pdu_size_continued = header_size + so_size + segment_size;
  for (int i = 0; i < n_pdus; i++) {
    pdu_bufs[i] = srsran::make_byte_buffer();
    TESTASSERT(nullptr != pdu_bufs[i]);
    if (i == 0) {
      pdu_bufs[i]->N_bytes = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_first);
      TESTASSERT_EQ(pdu_size_first, pdu_bufs[i]->N_bytes);
    } else {
      pdu_bufs[i]->N_bytes = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_continued);
      TESTASSERT_EQ(pdu_size_continued, pdu_bufs[i]->N_bytes);
    }
  }

  // Only middle PDU into RLC2
  // First PDU is lost to trigger status report
  for (int i = 0; i < n_pdus; i++) {
    if (i == 1) {
      rlc2.write_pdu(pdu_bufs[i]->msg, pdu_bufs[i]->N_bytes);
    }
  }

  // Advance timer to trigger status report
  for (uint8_t t = 0; t < 35; t++) {
    timers.step_all();
  }
  TESTASSERT_NEQ(0, rlc2.get_buffer_state());

  // Make sure RLC 1 has only the last segment to TX before getting the status report
  TESTASSERT_EQ(pdu_size_continued, rlc1.get_buffer_state());

  // Get status report from RLC 2
  // and write it to RLC 1
  {
    unique_byte_buffer_t status_buf = srsran::make_byte_buffer();
    status_buf->N_bytes             = rlc2.read_pdu(status_buf->msg, 100);
    rlc1.write_pdu(status_buf->msg, status_buf->N_bytes);
  }

  // Make sure RLC 1 now has the last segment to TX and the RETX of the first segment
  TESTASSERT_EQ(pdu_size_continued + pdu_size_first, rlc1.get_buffer_state());
  return SRSRAN_SUCCESS;
}

// This test checks whether RLC informs upper layer when max retransmission has been reached
// due to lost SDUs as a whole
int max_retx_lost_sdu_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  int           len = 0;

  auto&  test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  srslog::fetch_basic_logger("RLC_AM_1").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC").set_hex_dump_max_size(100);
  test_delimit_logger delimiter("max retx lost SDU ({} bit SN)", to_number(sn_size));

  const rlc_config_t rlc_cfg = rlc_config_t::default_rlc_am_nr_config(to_number(sn_size));
  if (not rlc1.configure(rlc_cfg)) {
    return SRSRAN_ERROR;
  }

  // Push 2 SDUs into RLC1
  const uint32_t       n_sdus = 2;
  unique_byte_buffer_t sdu_bufs[n_sdus];
  constexpr uint32_t   payload_size = 1; // Give each buffer a size of 1 byte
  uint32_t             header_size  = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  for (uint32_t i = 0; i < n_sdus; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i;            // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = payload_size; // Give each buffer a size of 1 byte
    sdu_bufs[i]->md.pdcp_sn = i;            // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  uint32_t pdu_size = header_size + payload_size;

  // Read 2 PDUs from RLC1 (1 byte each)
  const uint32_t n_pdus = 2;
  byte_buffer_t  pdu_bufs[n_pdus];
  for (uint32_t i = 0; i < n_pdus; i++) {
    len                 = rlc1.read_pdu(pdu_bufs[i].msg, pdu_size); // 2 byte header + 1 byte payload
    pdu_bufs[i].N_bytes = len;
  }

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Fake status PDU that ack SN=1 and nack SN=0
  rlc_am_nr_status_pdu_t fake_status(sn_size);
  fake_status.ack_sn = 2; // delivered up to SN=1
  rlc_status_nack_t nack; // one SN was lost
  nack.nack_sn = 0;       // it was SN=0 that was lost
  fake_status.push_nack(nack);

  // pack into PDU
  byte_buffer_t status_pdu;
  rlc_am_nr_write_status_pdu(fake_status, rlc_cfg.am_nr.tx_sn_field_length, &status_pdu);

  // Exceed the number of tolerated retransmissions by one additional retransmission
  // to trigger notification of the higher protocol layers. Note that the initial transmission
  // (before starting retransmissions) does not count. See TS 38.322 Sec. 5.3.2
  for (uint32_t retx_count = 0; retx_count < rlc_cfg.am_nr.max_retx_thresh + 1; ++retx_count) {
    // we've not yet reached max attempts
    TESTASSERT(tester.max_retx_triggered == false);

    // Write status PDU to RLC1
    rlc1.write_pdu(status_pdu.msg, status_pdu.N_bytes);

    byte_buffer_t pdu_buf;
    len = rlc1.read_pdu(pdu_buf.msg, pdu_size); // 2 byte header + 1 byte payload
  }

  // Now maxRetx should have been triggered
  TESTASSERT(tester.max_retx_triggered == true);

  return SRSRAN_SUCCESS;
}

// This test checks whether RLC informs upper layer when max retransmission has been reached
// due to lost SDU segments
int max_retx_lost_segments_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  int           len = 0;

  auto&  test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  srslog::fetch_basic_logger("RLC_AM_1").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC").set_hex_dump_max_size(100);
  test_delimit_logger delimiter("max retx lost SDU segment ({} bit SN)", to_number(sn_size));

  const rlc_config_t rlc_cfg = rlc_config_t::default_rlc_am_nr_config(to_number(sn_size));
  if (not rlc1.configure(rlc_cfg)) {
    return SRSRAN_ERROR;
  }

  // Push 2 SDUs into RLC1
  const uint32_t       n_sdus = 2;
  unique_byte_buffer_t sdu_bufs[n_sdus];
  constexpr uint32_t   payload_size = 20; // Give each buffer a size of 20 bytes
  uint32_t             header_size  = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  for (uint32_t i = 0; i < n_sdus; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i;            // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = payload_size; // Give each buffer a size of 20 bytes
    sdu_bufs[i]->md.pdcp_sn = i;            // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  constexpr uint32_t so_size                = 2;
  constexpr uint32_t segment_size_first     = 13;
  constexpr uint32_t segment_size_continued = 7;
  uint32_t           pdu_size_first         = header_size + segment_size_first;
  uint32_t           pdu_size_continued     = header_size + so_size + segment_size_continued;

  // Read 2*2=4 PDUs from RLC1 and limit to 15 byte to force segmentation in two parts:
  // Segment 1: 2 byte header + 13 byte payload; space fully used
  // Segment 2: 4 byte header + 7 byte payload; space not fully used, 4 bytes left over
  const uint32_t n_pdus = 4;
  byte_buffer_t  pdu_bufs[n_pdus];
  for (uint32_t i = 0; i < n_pdus; i++) {
    len                 = rlc1.read_pdu(pdu_bufs[i].msg, pdu_size_first);
    pdu_bufs[i].N_bytes = len;
  }

  TESTASSERT(0 == rlc1.get_buffer_state());

  // Fake status PDU that ack SN=1 and nack {SN=0 segment 0, SN=0 segment 1}
  rlc_am_nr_status_pdu_t status_lost_both_segments(sn_size);
  status_lost_both_segments.ack_sn = 2; // delivered up to SN=1

  // two segments lost
  {
    rlc_status_nack_t nack;
    nack.nack_sn  = 0;    // it was SN=0 that was lost
    nack.has_so   = true; // this NACKs a segment
    nack.so_start = 0;    // segment starts at (and includes) byte 0
    nack.so_end   = 12;   // segment ends at (and includes) byte 12
    status_lost_both_segments.push_nack(nack);
  }

  {
    rlc_status_nack_t nack;
    nack.nack_sn  = 0;    // it was SN=0 that was lost
    nack.has_so   = true; // this NACKs a segment
    nack.so_start = 13;   // segment starts at (and includes) byte 13
    nack.so_end   = 19;   // segment ends at (and includes) byte 19
    status_lost_both_segments.push_nack(nack);
  }

  // pack into PDU
  byte_buffer_t status_pdu_lost_both_segments;
  rlc_am_nr_write_status_pdu(
      status_lost_both_segments, rlc_cfg.am_nr.tx_sn_field_length, &status_pdu_lost_both_segments);

  // Fake status PDU that ack SN=1 and nack {SN=0 segment 1}
  rlc_am_nr_status_pdu_t status_lost_second_segment(sn_size);
  status_lost_second_segment.ack_sn = 2; // delivered up to SN=1

  // one SN was lost
  {
    rlc_status_nack_t nack;
    nack.nack_sn  = 0;    // it was SN=0 that was lost
    nack.has_so   = true; // this NACKs a segment
    nack.so_start = 13;   // segment starts at (and includes) byte 13
    nack.so_end   = 19;   // segment ends at (and includes) byte 19
    status_lost_second_segment.push_nack(nack);
  }

  // pack into PDU
  byte_buffer_t status_pdu_lost_second_segment;
  rlc_am_nr_write_status_pdu(
      status_lost_second_segment, rlc_cfg.am_nr.tx_sn_field_length, &status_pdu_lost_second_segment);

  // Exceed the number of tolerated retransmissions by one additional retransmission
  // to trigger notification of the higher protocol layers. Note that the initial transmission
  // (before starting retransmissions) does not count. See TS 38.322 Sec. 5.3.2
  for (uint32_t retx_count = 0; retx_count < rlc_cfg.am_nr.max_retx_thresh + 1; ++retx_count) {
    byte_buffer_t pdu_buf;

    // we've not yet reached max attempts
    TESTASSERT(tester.max_retx_triggered == false);

    if (retx_count < rlc_cfg.am_nr.max_retx_thresh / 2) {
      // Send NACK for segment 1 and segment 2
      // Although two segments, this must count as one retransmission,
      // because both segments NACK the same SDU in the same status message.
      rlc1.write_pdu(status_pdu_lost_both_segments.msg, status_pdu_lost_both_segments.N_bytes);

      // read the retransmitted PDUs
      len = rlc1.read_pdu(pdu_buf.msg, pdu_size_first); // 2 byte header + 13 byte payload
      len = rlc1.read_pdu(pdu_buf.msg, pdu_size_first); // 4 byte header + 7 byte payload
    } else {
      // Send NACK for segment 2 (assume at least segment 1 was finally received)
      rlc1.write_pdu(status_pdu_lost_second_segment.msg, status_pdu_lost_second_segment.N_bytes);

      // read the retransmitted PDUs
      len = rlc1.read_pdu(pdu_buf.msg, pdu_size_first); // 4 byte header + 7 byte payload
    }
  }

  // Now maxRetx should have been triggered
  TESTASSERT(tester.max_retx_triggered == true);

  return SRSRAN_SUCCESS;
}

// This test checks the correct functioning of RLC discard functionality
int discard_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  test_delimit_logger delimiter("discard test ({} bit SN)", to_number(sn_size));

  srslog::fetch_basic_logger("RLC_AM_1").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC_AM_2").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC").set_hex_dump_max_size(100);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return SRSRAN_ERROR;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return SRSRAN_ERROR;
  }

  uint32_t num_tx_sdus  = 1;
  uint32_t header_size  = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  uint32_t payload_size = 5; // Give each buffer a size of 5 bytes
  // Test discarding the single SDU from the queue
  {
    for (uint32_t i = 0; i < num_tx_sdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes = payload_size;
      for (uint32_t k = 0; k < sdu->N_bytes; ++k) {
        sdu->msg[k] = i; // Write the index into the buffer
      }
      sdu->md.pdcp_sn = i;
      rlc1.write_sdu(std::move(sdu));
    }
  }
  rlc1.discard_sdu(0); // Try to discard PDCP_SN=0
  TESTASSERT(rlc1.has_data() == false);

  num_tx_sdus  = 10;
  payload_size = 7; // Give each buffer a size of 7 bytes
  // Test discarding two SDUs in the middle (SN=3) and end (SN=9) of the queue and read PDUs after
  {
    for (uint32_t i = 0; i < num_tx_sdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes = payload_size;
      for (uint32_t k = 0; k < sdu->N_bytes; ++k) {
        sdu->msg[k] = i; // Write the index into the buffer
      }
      sdu->md.pdcp_sn = i;
      rlc1.write_sdu(std::move(sdu));
    }
  }
  TESTASSERT(rlc1.get_buffer_state() == num_tx_sdus * (header_size + payload_size)); // 10 * (2B Header + 7B Payload)
  rlc1.discard_sdu(3);                                                               // Try to discard PDCP_SN=3
  TESTASSERT(rlc1.has_data() == true);
  TESTASSERT(rlc1.get_buffer_state() == (num_tx_sdus - 1) * (header_size + payload_size));
  rlc1.discard_sdu(9); // Try to discard PDCP_SN=9
  TESTASSERT(rlc1.has_data() == true);
  TESTASSERT(rlc1.get_buffer_state() == (num_tx_sdus - 2) * (header_size + payload_size));

  num_tx_sdus = 8;
  {
    for (uint32_t i = 0; i < num_tx_sdus; ++i) {
      unique_byte_buffer_t pdu = srsran::make_byte_buffer();
      uint32_t             len = rlc1.read_pdu(pdu->msg, 50); // sufficient space to read without segmentation
      pdu->N_bytes             = len;
      TESTASSERT((header_size + payload_size) == len);
      // Check that we don't have any SN gaps
      rlc_am_nr_pdu_header_t header = {};
      rlc_am_nr_read_data_pdu_header(pdu.get(), sn_size, &header);
      TESTASSERT(header.sn == i);
    }
  }
  TESTASSERT(rlc1.has_data() == false);
  srslog::fetch_basic_logger("TEST").info("Received %zd SDUs", tester.sdus.size());

  num_tx_sdus  = 3;
  payload_size = 7; // Give each buffer a size of 7 bytes
  // Test discarding non-existing SDU from the queue
  {
    for (uint32_t i = 0; i < num_tx_sdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes = payload_size;
      for (uint32_t k = 0; k < sdu->N_bytes; ++k) {
        sdu->msg[k] = i; // Write the index into the buffer
      }
      sdu->md.pdcp_sn = i;
      rlc1.write_sdu(std::move(sdu));
    }
  }
  TESTASSERT(rlc1.get_buffer_state() == num_tx_sdus * (header_size + payload_size)); // 3 * (2B Header + 7B Payload)
  rlc1.discard_sdu(8); // Try to discard PDCP_SN=8, which doesn't exist
  TESTASSERT(rlc1.get_buffer_state() == num_tx_sdus * (header_size + payload_size)); // 3 * (2B Header + 7B Payload)

  return SRSRAN_SUCCESS;
}

// Test p bit set on new TX with PollPDU
int poll_pdu(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("pollPDU test ({} bit SN)", to_number(sn_size));

  srslog::fetch_basic_logger("RLC_AM_1").set_hex_dump_max_size(100);

  rlc_config_t rlc_cnfg             = {};
  rlc_cnfg.rat                      = srsran_rat_t::nr;
  rlc_cnfg.rlc_mode                 = rlc_mode_t::am;
  rlc_cnfg.am_nr.tx_sn_field_length = sn_size; // Number of bits used for tx (UL) sequence number
  rlc_cnfg.am_nr.rx_sn_field_length = sn_size; // Number of bits used for rx (DL) sequence number
  rlc_cnfg.am_nr.poll_pdu           = 4;
  rlc_cnfg.am_nr.poll_byte          = 3000;
  rlc_cnfg.am_nr.t_status_prohibit  = 8;
  rlc_cnfg.am_nr.max_retx_thresh    = 8;
  rlc_cnfg.am_nr.t_reassembly       = 35;

  // Test p bit set on new TX with PollPDU
  {
    rlc_am rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
    if (not rlc1.configure(rlc_cnfg)) {
      return SRSRAN_ERROR;
    }
    // pollPDU == 4
    uint32_t num_tx_sdus = 6;
    for (uint32_t i = 0; i < num_tx_sdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes    = 1;
      sdu->md.pdcp_sn = i;
      rlc1.write_sdu(std::move(sdu));
    }
    uint32_t num_tx_pdus = 6;
    uint32_t pdu_size    = sn_size == rlc_am_nr_sn_size_t::size12bits ? 3 : 4;
    for (uint32_t i = 0; i < num_tx_pdus; ++i) {
      unique_byte_buffer_t pdu = srsran::make_byte_buffer();
      TESTASSERT(pdu != nullptr);
      pdu->N_bytes = rlc1.read_pdu(pdu->msg, pdu_size);
      rlc_am_nr_pdu_header_t hdr;
      rlc_am_nr_read_data_pdu_header(pdu.get(), sn_size, &hdr);
      if (i != 3 && i != 5) { // P bit set for PollPDU and for empty TX queue
        TESTASSERT_EQ(0, hdr.p);
      } else {
        TESTASSERT_EQ(1, hdr.p);
      }
    }
  }
  return SRSRAN_SUCCESS;
}

// Test p bit set on new TX with PollBYTE
int poll_byte(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("pollBYTE test ({} bit SN)", to_number(sn_size));

  srslog::fetch_basic_logger("RLC_AM_1").set_hex_dump_max_size(100);

  rlc_config_t rlc_cnfg             = {};
  rlc_cnfg.rat                      = srsran_rat_t::nr;
  rlc_cnfg.rlc_mode                 = rlc_mode_t::am;
  rlc_cnfg.am_nr.tx_sn_field_length = sn_size; // Number of bits used for tx (UL) sequence number
  rlc_cnfg.am_nr.rx_sn_field_length = sn_size; // Number of bits used for rx (DL) sequence number
  rlc_cnfg.am_nr.poll_pdu           = 4;
  rlc_cnfg.am_nr.poll_byte          = 3000;
  rlc_cnfg.am_nr.t_status_prohibit  = 8;
  rlc_cnfg.am_nr.max_retx_thresh    = 8;
  rlc_cnfg.am_nr.t_reassembly       = 35;

  rlc_am rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  if (not rlc1.configure(rlc_cnfg)) {
    return SRSRAN_ERROR;
  }
  // pollByte == 3000
  uint32_t num_tx_sdus = 4;
  for (uint32_t i = 0; i < num_tx_sdus; ++i) {
    // Write SDU
    unique_byte_buffer_t sdu = srsran::make_byte_buffer();
    TESTASSERT(sdu != nullptr);
    sdu->N_bytes    = i == 0 ? 2999 : 1;
    sdu->md.pdcp_sn = i;
    rlc1.write_sdu(std::move(sdu));
  }
  uint32_t num_tx_pdus    = num_tx_sdus;
  uint32_t small_pdu_size = sn_size == rlc_am_nr_sn_size_t::size12bits ? 3 : 4;
  uint32_t large_pdu_size = sn_size == rlc_am_nr_sn_size_t::size12bits ? 3001 : 3002;
  for (uint32_t i = 0; i < num_tx_pdus; ++i) {
    unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    TESTASSERT(pdu != nullptr);
    uint32_t nof_bytes = i == 0 ? large_pdu_size : small_pdu_size;
    pdu->N_bytes       = rlc1.read_pdu(pdu->msg, nof_bytes);
    TESTASSERT_EQ(nof_bytes, pdu->N_bytes);
    rlc_am_nr_pdu_header_t hdr;
    rlc_am_nr_read_data_pdu_header(pdu.get(), rlc_am_nr_sn_size_t::size18bits, &hdr);
    if (i != 1 && i != 3) {
      TESTASSERT_EQ(0, hdr.p);
    } else {
      TESTASSERT_EQ(1, hdr.p);
    }
  }
  return SRSRAN_SUCCESS;
}

// Test p bit set on RETXes that cause an empty retx queue.
int poll_retx(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("poll retx test ({} bit SN)", to_number(sn_size));

  srslog::fetch_basic_logger("RLC_AM_1").set_hex_dump_max_size(100);

  rlc_config_t rlc_cnfg             = {};
  rlc_cnfg.rat                      = srsran_rat_t::nr;
  rlc_cnfg.rlc_mode                 = rlc_mode_t::am;
  rlc_cnfg.am_nr.tx_sn_field_length = sn_size; // Number of bits used for tx (UL) sequence number
  rlc_cnfg.am_nr.rx_sn_field_length = sn_size; // Number of bits used for rx (DL) sequence number
  rlc_cnfg.am_nr.poll_pdu           = 4;
  rlc_cnfg.am_nr.poll_byte          = 3000;
  rlc_cnfg.am_nr.t_status_prohibit  = 8;
  rlc_cnfg.am_nr.max_retx_thresh    = 8;
  rlc_cnfg.am_nr.t_reassembly       = 35;

  rlc_am rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  if (not rlc1.configure(rlc_cnfg)) {
    return SRSRAN_ERROR;
  }

  // pollPDU == 4
  {
    uint32_t num_tx_sdus = 5;
    for (uint32_t i = 0; i < num_tx_sdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes    = 1;
      sdu->md.pdcp_sn = i;
      rlc1.write_sdu(std::move(sdu));
    }
  }
  {
    // Read 3 PDUs and NACK the second one
    uint32_t num_tx_pdus = 3;
    uint32_t pdu_size    = sn_size == rlc_am_nr_sn_size_t::size12bits ? 3 : 4;
    for (uint32_t i = 0; i < num_tx_pdus; ++i) {
      unique_byte_buffer_t pdu = srsran::make_byte_buffer();
      TESTASSERT(pdu != nullptr);
      pdu->N_bytes = rlc1.read_pdu(pdu->msg, pdu_size);
      rlc_am_nr_pdu_header_t hdr;
      rlc_am_nr_read_data_pdu_header(pdu.get(), sn_size, &hdr);
      TESTASSERT_EQ(0, hdr.p);
    }
  }
  {
    unique_byte_buffer_t status_pdu = srsran::make_byte_buffer();
    TESTASSERT(status_pdu != nullptr);
    rlc_am_nr_status_pdu_t status(rlc_am_nr_sn_size_t::size12bits);
    status.ack_sn = 2;
    {
      rlc_status_nack_t nack;
      nack.nack_sn = 1; // SN=1 needs RETX
      status.push_nack(nack);
    }
    rlc_am_nr_write_status_pdu(status, rlc_cnfg.am_nr.tx_sn_field_length, status_pdu.get());
    rlc1.write_pdu(status_pdu->msg, status_pdu->N_bytes);
  }
  {
    // Read 2 PDUs,
    uint32_t num_tx_pdus = 3;
    uint32_t pdu_size    = sn_size == rlc_am_nr_sn_size_t::size12bits ? 3 : 4;
    for (uint32_t i = 0; i < num_tx_pdus; ++i) {
      unique_byte_buffer_t pdu = srsran::make_byte_buffer();
      TESTASSERT(pdu != nullptr);
      pdu->N_bytes = rlc1.read_pdu(pdu->msg, pdu_size);
      TESTASSERT_EQ(pdu_size, pdu->N_bytes);
      rlc_am_nr_pdu_header_t hdr;
      rlc_am_nr_read_data_pdu_header(pdu.get(), sn_size, &hdr);
      if (i == 0) {
        TESTASSERT_EQ(0, hdr.p); // No poll since pollPDU is not incremented for RETX
        TESTASSERT_EQ(1, hdr.sn);
      } else {
        TESTASSERT_EQ(1, hdr.p); // poll set because of pollPDU for SN=3 and empty buffer on SN=4
      }
    }
  }
  {
    unique_byte_buffer_t status_pdu = srsran::make_byte_buffer();
    TESTASSERT(status_pdu != nullptr);
    rlc_am_nr_status_pdu_t status(rlc_am_nr_sn_size_t::size12bits);
    status.ack_sn = 4;
    {
      rlc_status_nack_t nack;
      nack.nack_sn = 1; // SN=1 needs RETX
      status.push_nack(nack);
    }
    rlc_am_nr_write_status_pdu(status, rlc_cnfg.am_nr.tx_sn_field_length, status_pdu.get());
    rlc1.write_pdu(status_pdu->msg, status_pdu->N_bytes);
  }
  {
    // Read 1 RETX PDU. Empty retx buffer, so poll should be set
    uint32_t num_tx_pdus = 1;
    uint32_t pdu_size    = sn_size == rlc_am_nr_sn_size_t::size12bits ? 3 : 4;
    for (uint32_t i = 0; i < num_tx_pdus; ++i) {
      unique_byte_buffer_t pdu = srsran::make_byte_buffer();
      TESTASSERT(pdu != nullptr);
      pdu->N_bytes = rlc1.read_pdu(pdu->msg, pdu_size);
      TESTASSERT_EQ(pdu_size, pdu->N_bytes);
      rlc_am_nr_pdu_header_t hdr;
      rlc_am_nr_read_data_pdu_header(pdu.get(), sn_size, &hdr);
      if (i == 0) {
        TESTASSERT_EQ(1, hdr.p); // Poll set because of empty retx buffer
        TESTASSERT_EQ(1, hdr.sn);
      }
    }
  }
  return SRSRAN_SUCCESS;
}

// This test checks whether re-transmissions are triggered correctly in case the t-PollRetranmission expires.
// It checks if the poll retx timer is re-armed upon receiving an ACK for POLL_SN
bool poll_retx_expiry(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("poll retx expiry test ({} bit SN)", to_number(sn_size));

  srslog::fetch_basic_logger("RLC_AM_1").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC_AM_2").set_hex_dump_max_size(100);

  rlc_config_t rlc_cnfg = rlc_config_t::default_rlc_am_nr_config();

  rlc_cnfg.am_nr.tx_sn_field_length = sn_size; // Number of bits used for tx (UL) sequence number
  rlc_cnfg.am_nr.rx_sn_field_length = sn_size; // Number of bits used for rx (DL) sequence number
  rlc_cnfg.am_nr.t_poll_retx        = 65;
  rlc_cnfg.am_nr.poll_pdu           = -1;
  rlc_cnfg.am_nr.poll_byte          = -1;
  rlc_cnfg.am_nr.max_retx_thresh    = 6;
  rlc_cnfg.am_nr.t_status_prohibit  = 55;

  rlc_am rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  if (not rlc1.configure(rlc_cnfg)) {
    return SRSRAN_ERROR;
  }

  rlc_am rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  if (not rlc2.configure(rlc_cnfg)) {
    return SRSRAN_ERROR;
  }

  unsigned hdr_no_so         = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  unsigned hdr_with_so       = sn_size == rlc_am_nr_sn_size_t::size12bits ? 4 : 5;
  unsigned ack_size          = 3;
  unsigned nack_size_no_so   = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  unsigned nack_size_with_so = sn_size == rlc_am_nr_sn_size_t::size12bits ? (2 + 4) : (3 + 4);
  // Tx SDU with 135 B of data
  // Read it in two PDU segments, so=0 (89B of data)
  // and so=89 (46B of data)
  {
    // TX a single SDU
    unique_byte_buffer_t sdu = srsran::make_byte_buffer();
    TESTASSERT(sdu != nullptr);
    sdu->N_bytes = 135;
    for (uint32_t k = 0; k < sdu->N_bytes; ++k) {
      sdu->msg[k] = 0; // Write the index into the buffer
    }
    sdu->md.pdcp_sn = 0;
    rlc1.write_sdu(std::move(sdu));

    // Read two PDUs. The last PDU should trigger polling, as it
    // is the last SDU segment in the buffer.
    uint32_t             pdu1_size = 89 + hdr_no_so;
    unique_byte_buffer_t pdu1      = srsran::make_byte_buffer();
    TESTASSERT(pdu1 != nullptr);
    pdu1->N_bytes = rlc1.read_pdu(pdu1->msg, pdu1_size); // 89 bytes payload

    uint32_t             pdu2_size = 46 + hdr_with_so;
    unique_byte_buffer_t pdu2      = srsran::make_byte_buffer();
    TESTASSERT(pdu2 != nullptr);
    pdu2->N_bytes = rlc1.read_pdu(pdu2->msg, pdu2_size); // 46 bytes payload

    // Deliver PDU2 to RLC2. PDU1 is lost
    rlc2.write_pdu(pdu2->msg, pdu2->N_bytes);

    // Double-check polling status in PDUs
    rlc_am_nr_pdu_header_t hdr1 = {};
    rlc_am_nr_read_data_pdu_header(pdu1.get(), sn_size, &hdr1);
    rlc_am_nr_pdu_header_t hdr2 = {};
    rlc_am_nr_read_data_pdu_header(pdu2.get(), sn_size, &hdr2);
    TESTASSERT_EQ(0, hdr1.p);
    TESTASSERT_EQ(1, hdr2.p);
  }

  // Step timers until t-PollRetransmit timer expires on RLC1
  // t-PollRetransmit will schedule SN=0, so=0, payload_len=89 for RETX
  // t-Reordering timer also will expire on RLC2, meaning we will also get a status report.
  TESTASSERT_EQ(false, rlc1.has_data());
  for (int cnt = 0; cnt < 65; cnt++) {
    timers.step_all();
  }

  // Make sure that the SDU segment was scheduled for RETX
  TESTASSERT_EQ(89 + hdr_no_so, rlc1.get_buffer_state());

  // Further segment RETX segment
  // First SDU segment (81B of data)
  {
    unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    TESTASSERT(pdu != nullptr);
    pdu->N_bytes = rlc1.read_pdu(pdu->msg, 81 + hdr_no_so);
  }
  // Second SDU segment (8B of data)
  {
    unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    TESTASSERT(pdu != nullptr);
    pdu->N_bytes = rlc1.read_pdu(pdu->msg, 8 + hdr_with_so);
  }
  TESTASSERT_EQ(0, rlc1.get_buffer_state());

  // Read status PDU from RLC2 (triggered previously from t-Reordering)
  // ACK=1, NACKs=1
  // NACK_SN[0].sn=0, NACK_SN[0].so_start=0, NACK_SN[0].so_end=89
  uint32_t status_size = rlc2.get_buffer_state();
  TESTASSERT_EQ(ack_size + nack_size_with_so, status_size);

  // Read status PDU from RLC2
  unique_byte_buffer_t status_buf = srsran::make_byte_buffer();
  TESTASSERT(status_buf != nullptr);
  int len             = rlc2.read_pdu(status_buf->msg, status_size);
  status_buf->N_bytes = len;

  TESTASSERT(0 == rlc2.get_buffer_state());

  // Assert status is correct
  rlc_am_nr_status_pdu_t status_check(sn_size);
  rlc_am_nr_read_status_pdu(status_buf.get(), sn_size, &status_check);
  TESTASSERT(status_check.ack_sn == 1);            // SN=1 is first SN missing without a NACK
  TESTASSERT(status_check.nacks.size() == 1);      // 1 PDU lost
  TESTASSERT(status_check.nacks[0].nack_sn == 0);  // SN=0
  TESTASSERT(status_check.nacks[0].so_start == 0); // SN=0
  TESTASSERT_EQ(88, status_check.nacks[0].so_end); // SN=0

  TESTASSERT_EQ(0, rlc1.get_buffer_state());
  // Deliver status PDU after ReTX to RLC1. This should restart t-PollRetransmission
  // It NACKs SDU segment 0:81 and 81:89
  TESTASSERT_EQ(false, rlc1.has_data());
  rlc1.write_pdu(status_buf->msg, status_buf->N_bytes);
  TESTASSERT_EQ(true, rlc1.has_data());

  // [I] SRB1 Retx SDU segment (81 B of data)
  // [I] SRB1 Retx PDU segment (8 B of data)
  {
    unique_byte_buffer_t pdu1 = srsran::make_byte_buffer();
    TESTASSERT(pdu1 != nullptr);
    pdu1->N_bytes = rlc1.read_pdu(pdu1->msg, 81 + hdr_no_so);

    unique_byte_buffer_t pdu2 = srsran::make_byte_buffer();
    TESTASSERT(pdu2 != nullptr);
    pdu2->N_bytes = rlc1.read_pdu(pdu2->msg, 8 + hdr_with_so);
  }

  TESTASSERT_EQ(false, rlc1.has_data()); // We don't have any more data

  // Step timers until t-PollRetransmission timer expires on RLC1
  // [I] SRB1 Schedule SN=3 for reTx
  for (int cnt = 0; cnt < 66; cnt++) {
    timers.step_all();
  }
  TESTASSERT_EQ(81 + hdr_no_so, rlc1.get_buffer_state());
  srslog::fetch_basic_logger("TEST").info("t-PollRetransmssion successfully restarted.");

  return SRSRAN_SUCCESS;
}

int rx_nack_range_no_so_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  std::string         str = "Rx NACK range test (" + std::to_string(to_number(sn_size)) + " bit SN)";
  test_delimit_logger delimiter(str.c_str());

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  auto rlc_cnfg              = rlc_config_t::default_rlc_am_nr_config(to_number(sn_size));
  rlc_cnfg.am_nr.t_poll_retx = -1;
  if (not rlc1.configure(rlc_cnfg)) {
    return -1;
  }

  // after configuring entity
  TESTASSERT(0 == rlc1.get_buffer_state());

  int n_sdu_bufs = 5;
  int n_pdu_bufs = 15;

  // Push 5 SDUs into RLC1
  std::vector<unique_byte_buffer_t> sdu_bufs(n_sdu_bufs);
  constexpr uint32_t                payload_size = 3; // Give the SDU the size of 3 bytes
  uint32_t                          header_size  = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  for (int i = 0; i < n_sdu_bufs; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i;            // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = payload_size; // Give each buffer a size of 3 bytes
    sdu_bufs[i]->md.pdcp_sn = i;            // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  uint32_t expected_buffer_state = (header_size + payload_size) * n_sdu_bufs;
  TESTASSERT_EQ(expected_buffer_state, rlc1.get_buffer_state());

  constexpr uint32_t so_size            = 2;
  constexpr uint32_t segment_size       = 1;
  uint32_t           pdu_size_first     = header_size + segment_size;
  uint32_t           pdu_size_continued = header_size + so_size + segment_size;

  // Read 15 PDUs from RLC1
  std::vector<unique_byte_buffer_t> pdu_bufs(n_pdu_bufs);
  for (int i = 0; i < n_pdu_bufs; i++) {
    // First also test buffer state
    uint32_t remaining_total_bytes = (payload_size * n_sdu_bufs) - (i * segment_size);
    uint32_t remaining_full_sdus   = remaining_total_bytes / payload_size;
    uint32_t remaining_seg_bytes   = remaining_total_bytes % payload_size;

    uint32_t buffer_state_full_sdus = (header_size + payload_size) * remaining_full_sdus;
    uint32_t buffer_state_seg_sdu   = remaining_seg_bytes == 0 ? 0 : (header_size + so_size + remaining_seg_bytes);
    expected_buffer_state           = buffer_state_full_sdus + buffer_state_seg_sdu;
    TESTASSERT_EQ(expected_buffer_state, rlc1.get_buffer_state());

    pdu_bufs[i] = srsran::make_byte_buffer();
    if (i == 0 || i == 3 || i == 6 || i == 9 || i == 12) {
      // First segment, no SO
      uint32_t len         = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_first); // 2 bytes for header + 1 byte payload
      pdu_bufs[i]->N_bytes = len;
      TESTASSERT_EQ(pdu_size_first, len);
    } else {
      // Middle or last segment, SO present
      uint32_t len         = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_continued); // 4 bytes for header + 1 byte payload
      pdu_bufs[i]->N_bytes = len;
      TESTASSERT_EQ(pdu_size_continued, len);
    }
  }

  // Deliver dummy status report with nack range betwen PDU 6 and 10.
  rlc_am_nr_status_pdu_t status(sn_size);
  status.ack_sn          = 5;
  rlc_status_nack_t nack = {};
  nack.nack_sn           = 1;
  nack.has_nack_range    = true;
  nack.nack_range        = 3;
  status.push_nack(nack);
  byte_buffer_t status_pdu;
  rlc_am_nr_write_status_pdu(status, sn_size, &status_pdu);

  rlc1.write_pdu(status_pdu.msg, status_pdu.N_bytes);

  TESTASSERT_EQ(3 * pdu_size_first + 6 * pdu_size_continued, rlc1.get_buffer_state());
  return SRSRAN_SUCCESS;
}

int rx_nack_range_with_so_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  std::string         str = "Rx NACK range test (" + std::to_string(to_number(sn_size)) + " bit SN)";
  test_delimit_logger delimiter(str.c_str());

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  auto rlc_cnfg              = rlc_config_t::default_rlc_am_nr_config(to_number(sn_size));
  rlc_cnfg.am_nr.t_poll_retx = -1;
  if (not rlc1.configure(rlc_cnfg)) {
    return -1;
  }

  // after configuring entity
  TESTASSERT(0 == rlc1.get_buffer_state());

  int n_sdu_bufs = 5;
  int n_pdu_bufs = 15;

  // Push 5 SDUs into RLC1
  std::vector<unique_byte_buffer_t> sdu_bufs(n_sdu_bufs);
  constexpr uint32_t                payload_size = 3; // Give the SDU the size of 3 bytes
  uint32_t                          header_size  = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  for (int i = 0; i < n_sdu_bufs; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i;            // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = payload_size; // Give each buffer a size of 3 bytes
    sdu_bufs[i]->md.pdcp_sn = i;            // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  uint32_t expected_buffer_state = (header_size + payload_size) * n_sdu_bufs;
  TESTASSERT_EQ(expected_buffer_state, rlc1.get_buffer_state());

  constexpr uint32_t so_size            = 2;
  constexpr uint32_t segment_size       = 1;
  uint32_t           pdu_size_first     = header_size + segment_size;
  uint32_t           pdu_size_continued = header_size + so_size + segment_size;

  // Read 15 PDUs from RLC1
  std::vector<unique_byte_buffer_t> pdu_bufs(n_pdu_bufs);
  for (int i = 0; i < n_pdu_bufs; i++) {
    // First also test buffer state
    uint32_t remaining_total_bytes = (payload_size * n_sdu_bufs) - (i * segment_size);
    uint32_t remaining_full_sdus   = remaining_total_bytes / payload_size;
    uint32_t remaining_seg_bytes   = remaining_total_bytes % payload_size;

    uint32_t buffer_state_full_sdus = (header_size + payload_size) * remaining_full_sdus;
    uint32_t buffer_state_seg_sdu   = remaining_seg_bytes == 0 ? 0 : (header_size + so_size + remaining_seg_bytes);
    expected_buffer_state           = buffer_state_full_sdus + buffer_state_seg_sdu;
    TESTASSERT_EQ(expected_buffer_state, rlc1.get_buffer_state());

    pdu_bufs[i] = srsran::make_byte_buffer();
    if (i == 0 || i == 3 || i == 6 || i == 9 || i == 12) {
      // First segment, no SO
      uint32_t len         = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_first); // 2 bytes for header + 1 byte payload
      pdu_bufs[i]->N_bytes = len;
      TESTASSERT_EQ(pdu_size_first, len);
    } else {
      // Middle or last segment, SO present
      uint32_t len         = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_continued); // 4 bytes for header + 1 byte payload
      pdu_bufs[i]->N_bytes = len;
      TESTASSERT_EQ(pdu_size_continued, len);
    }
  }

  // Deliver dummy status report with nack range betwen PDU 6 and 10.
  rlc_am_nr_status_pdu_t status(sn_size);
  status.ack_sn = 5;

  rlc_status_nack_t nack = {};
  nack.nack_sn           = 1;
  nack.has_nack_range    = true;
  nack.nack_range        = 3;
  nack.has_so            = true;
  nack.so_start          = 2;
  nack.so_end            = 0;
  status.push_nack(nack);
  byte_buffer_t status_pdu;
  rlc_am_nr_write_status_pdu(status, sn_size, &status_pdu);

  rlc1.write_pdu(status_pdu.msg, status_pdu.N_bytes);

  TESTASSERT_EQ(2 * pdu_size_first + 3 * pdu_size_continued, rlc1.get_buffer_state());
  return SRSRAN_SUCCESS;
}

int rx_nack_range_with_so_starting_with_full_sdu_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);

  auto&       test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am      rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am      rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  std::string str =
      "Rx NACK range test with SO starting with full SDU (" + std::to_string(to_number(sn_size)) + " bit SN)";
  test_delimit_logger delimiter(str.c_str());

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  auto rlc_cnfg              = rlc_config_t::default_rlc_am_nr_config(to_number(sn_size));
  rlc_cnfg.am_nr.t_poll_retx = -1;
  if (not rlc1.configure(rlc_cnfg)) {
    return -1;
  }

  // after configuring entity
  TESTASSERT(0 == rlc1.get_buffer_state());

  int n_sdu_bufs = 5;
  int n_pdu_bufs = 15;

  // Push 5 SDUs into RLC1
  std::vector<unique_byte_buffer_t> sdu_bufs(n_sdu_bufs);
  constexpr uint32_t                payload_size = 3; // Give the SDU the size of 3 bytes
  uint32_t                          header_size  = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  for (int i = 0; i < n_sdu_bufs; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i;            // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = payload_size; // Give each buffer a size of 3 bytes
    sdu_bufs[i]->md.pdcp_sn = i;            // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  uint32_t expected_buffer_state = (header_size + payload_size) * n_sdu_bufs;
  TESTASSERT_EQ(expected_buffer_state, rlc1.get_buffer_state());

  constexpr uint32_t so_size            = 2;
  constexpr uint32_t segment_size       = 1;
  uint32_t           pdu_size_whole     = header_size + payload_size;
  uint32_t           pdu_size_first     = header_size + segment_size;
  uint32_t           pdu_size_continued = header_size + so_size + segment_size;

  // Read 15 PDUs from RLC1
  std::vector<unique_byte_buffer_t> pdu_bufs(n_pdu_bufs);
  for (int i = 0; i < n_pdu_bufs; i++) {
    // First also test buffer state
    uint32_t remaining_total_bytes = (payload_size * n_sdu_bufs) - (i * segment_size);
    uint32_t remaining_full_sdus   = remaining_total_bytes / payload_size;
    uint32_t remaining_seg_bytes   = remaining_total_bytes % payload_size;

    uint32_t buffer_state_full_sdus = (header_size + payload_size) * remaining_full_sdus;
    uint32_t buffer_state_seg_sdu   = remaining_seg_bytes == 0 ? 0 : (header_size + so_size + remaining_seg_bytes);
    expected_buffer_state           = buffer_state_full_sdus + buffer_state_seg_sdu;
    TESTASSERT_EQ(expected_buffer_state, rlc1.get_buffer_state());

    pdu_bufs[i] = srsran::make_byte_buffer();
    if (i == 3) {
      // Special handling for SDU SN=1 (i==3): send as a whole, not segmented
      uint32_t len         = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_whole); // 2 bytes for header + 3 byte payload
      pdu_bufs[i]->N_bytes = len;
      TESTASSERT_EQ(pdu_size_whole, len);
      // update i to skip 2 segments
      i += 2;
    } else {
      if (i == 0 || i == 6 || i == 9 || i == 12) {
        // First segment, no SO
        uint32_t len         = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_first); // 2 bytes for header + 1 byte payload
        pdu_bufs[i]->N_bytes = len;
        TESTASSERT_EQ(pdu_size_first, len);
      } else {
        // Middle or last segment, SO present
        uint32_t len = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_continued); // 4 bytes for header + 1 byte payload
        pdu_bufs[i]->N_bytes = len;
        TESTASSERT_EQ(pdu_size_continued, len);
      }
    }
  }

  // Deliver dummy status report with nack range betwen PDU 4 and 10.
  rlc_am_nr_status_pdu_t status(sn_size);
  status.ack_sn = 5;

  rlc_status_nack_t nack = {};
  nack.nack_sn           = 1;
  nack.has_nack_range    = true;
  nack.nack_range        = 3;
  nack.has_so            = true;
  nack.so_start          = 0;
  nack.so_end            = 0;
  status.push_nack(nack);
  byte_buffer_t status_pdu;
  rlc_am_nr_write_status_pdu(status, sn_size, &status_pdu);

  rlc1.write_pdu(status_pdu.msg, status_pdu.N_bytes);

  TESTASSERT_EQ(pdu_size_whole + 2 * pdu_size_first + 2 * pdu_size_continued, rlc1.get_buffer_state());
  return SRSRAN_SUCCESS;
}

int rx_nack_range_with_so_ending_with_full_sdu_test(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);

  auto&       test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am      rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am      rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  std::string str =
      "Rx NACK range test with SO starting with full SDU (" + std::to_string(to_number(sn_size)) + " bit SN)";
  test_delimit_logger delimiter(str.c_str());

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  auto rlc_cnfg              = rlc_config_t::default_rlc_am_nr_config(to_number(sn_size));
  rlc_cnfg.am_nr.t_poll_retx = -1;
  if (not rlc1.configure(rlc_cnfg)) {
    return -1;
  }

  // after configuring entity
  TESTASSERT(0 == rlc1.get_buffer_state());

  int n_sdu_bufs = 5;
  int n_pdu_bufs = 15;

  // Push 5 SDUs into RLC1
  std::vector<unique_byte_buffer_t> sdu_bufs(n_sdu_bufs);
  constexpr uint32_t                payload_size = 3; // Give the SDU the size of 3 bytes
  uint32_t                          header_size  = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  for (int i = 0; i < n_sdu_bufs; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i;            // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = payload_size; // Give each buffer a size of 3 bytes
    sdu_bufs[i]->md.pdcp_sn = i;            // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  uint32_t expected_buffer_state = (header_size + payload_size) * n_sdu_bufs;
  TESTASSERT_EQ(expected_buffer_state, rlc1.get_buffer_state());

  constexpr uint32_t so_size            = 2;
  constexpr uint32_t segment_size       = 1;
  uint32_t           pdu_size_whole     = header_size + payload_size;
  uint32_t           pdu_size_first     = header_size + segment_size;
  uint32_t           pdu_size_continued = header_size + so_size + segment_size;

  // Read 15 PDUs from RLC1
  std::vector<unique_byte_buffer_t> pdu_bufs(n_pdu_bufs);
  for (int i = 0; i < n_pdu_bufs; i++) {
    // First also test buffer state
    uint32_t remaining_total_bytes = (payload_size * n_sdu_bufs) - (i * segment_size);
    uint32_t remaining_full_sdus   = remaining_total_bytes / payload_size;
    uint32_t remaining_seg_bytes   = remaining_total_bytes % payload_size;

    uint32_t buffer_state_full_sdus = (header_size + payload_size) * remaining_full_sdus;
    uint32_t buffer_state_seg_sdu   = remaining_seg_bytes == 0 ? 0 : (header_size + so_size + remaining_seg_bytes);
    expected_buffer_state           = buffer_state_full_sdus + buffer_state_seg_sdu;
    TESTASSERT_EQ(expected_buffer_state, rlc1.get_buffer_state());

    pdu_bufs[i] = srsran::make_byte_buffer();
    if (i == 9) {
      // Special handling for SDU SN=3 (i==9): send as a whole, not segmented
      uint32_t len         = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_whole); // 2 bytes for header + 3 byte payload
      pdu_bufs[i]->N_bytes = len;
      TESTASSERT_EQ(pdu_size_whole, len);
      // update i to skip 2 segments
      i += 2;
    } else {
      if (i == 0 || i == 3 || i == 6 || i == 12) {
        // First segment, no SO
        uint32_t len         = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_first); // 2 bytes for header + 1 byte payload
        pdu_bufs[i]->N_bytes = len;
        TESTASSERT_EQ(pdu_size_first, len);
      } else {
        // Middle or last segment, SO present
        uint32_t len = rlc1.read_pdu(pdu_bufs[i]->msg, pdu_size_continued); // 4 bytes for header + 1 byte payload
        pdu_bufs[i]->N_bytes = len;
        TESTASSERT_EQ(pdu_size_continued, len);
      }
    }
  }

  // Deliver dummy status report with nack range betwen PDU 6 and 12.
  rlc_am_nr_status_pdu_t status(sn_size);
  status.ack_sn = 5;

  rlc_status_nack_t nack = {};
  nack.nack_sn           = 1;
  nack.has_nack_range    = true;
  nack.nack_range        = 3;
  nack.has_so            = true;
  nack.so_start          = 2;
  nack.so_end            = rlc_status_nack_t::so_end_of_sdu;
  status.push_nack(nack);
  byte_buffer_t status_pdu;
  rlc_am_nr_write_status_pdu(status, sn_size, &status_pdu);

  rlc1.write_pdu(status_pdu.msg, status_pdu.N_bytes);

  TESTASSERT_EQ(1 * pdu_size_first + 3 * pdu_size_continued + pdu_size_whole, rlc1.get_buffer_state());
  return SRSRAN_SUCCESS;
}

int out_of_order_status(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("out of order status report ({} bit SN)", to_number(sn_size));
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config(to_number(sn_size)))) {
    return -1;
  }
  TESTASSERT_EQ(0, rlc1.get_buffer_state());

  basic_test_tx(&rlc1, pdu_bufs, sn_size);

  // Status 1, ACK SN=2, NACK_SN = 1
  rlc_am_nr_status_pdu_t status1(sn_size);
  status1.ack_sn = 2;
  {
    rlc_status_nack_t nack = {};
    nack.nack_sn           = 1;
    status1.push_nack(nack);
  }

  // Status 2, ACK SN=5, NACK SN = 3
  rlc_am_nr_status_pdu_t status2(sn_size);
  status2.ack_sn = 5;
  {
    rlc_status_nack_t nack = {};
    nack.nack_sn           = 3;
    status2.push_nack(nack);
  }

  // pack into PDU
  byte_buffer_t status1_pdu;
  rlc_am_nr_write_status_pdu(status1, sn_size, &status1_pdu);

  // pack into PDU
  byte_buffer_t status2_pdu;
  rlc_am_nr_write_status_pdu(status2, sn_size, &status2_pdu);

  // Write status 2 to RLC1
  rlc1.write_pdu(status2_pdu.msg, status2_pdu.N_bytes);

  // Check TX_NEXT_ACK
  {
    rlc_am_nr_tx_state_t st = tx1->get_tx_state();
    TESTASSERT_EQ(3, st.tx_next_ack);                   // SN=3 was nacked on status report 2
    TESTASSERT_EQ(2, tx1->get_tx_window_utilization()); // 2 PDUs still in TX_WINDOW
  }
  // Write status 1 to RLC1
  rlc1.write_pdu(status1_pdu.msg, status1_pdu.N_bytes);

  // Check TX_NEXT_ACK
  {
    rlc_am_nr_tx_state_t st = tx1->get_tx_state();
    TESTASSERT_EQ(3, st.tx_next_ack);
    TESTASSERT_EQ(2, tx1->get_tx_window_utilization());
  }
  // Check statistics
  rlc_bearer_metrics_t metrics1 = rlc1.get_metrics();

  return SRSRAN_SUCCESS;
}

// If we lose the status report
int lost_status_and_advanced_rx_window(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(true, nullptr);
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("Lost status report and advance RX window ({} bit SN)", to_number(sn_size));
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  auto cfg = rlc_config_t::default_rlc_am_nr_config(to_number(sn_size));
  if (not rlc1.configure(cfg)) {
    return -1;
  }
  if (not rlc2.configure(cfg)) {
    return -1;
  }
  uint32_t mod_nr = cardinality(cfg.am_nr.tx_sn_field_length);

  // Fill up the RX window
  constexpr uint32_t payload_size = 3; // Give the SDU the size of 3 bytes
  uint32_t           header_size  = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  for (uint32_t sn = 0; sn < 10; ++sn) {
    // Write SDU
    unique_byte_buffer_t sdu_buf = srsran::make_byte_buffer();
    sdu_buf->msg[0]              = sn;           // Write the index into the buffer
    sdu_buf->N_bytes             = payload_size; // Give each buffer a size of 3 bytes
    sdu_buf->md.pdcp_sn          = sn;           // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_buf));

    // Read PDU
    unique_byte_buffer_t pdu_buf = srsran::make_byte_buffer();
    pdu_buf->N_bytes             = rlc1.read_pdu(pdu_buf->msg, 100);

    // Write PDU into RLC 2
    // We receive all PDUs
    rlc2.write_pdu(pdu_buf->msg, pdu_buf->N_bytes);
  }

  // We got the polling bit, so we generate the status report.
  TESTASSERT_EQ(3, rlc2.get_buffer_state());

  // Read status PDU
  {
    unique_byte_buffer_t status_buf = srsran::make_byte_buffer();
    status_buf->N_bytes             = rlc2.read_pdu(status_buf->msg, 3);
  }
  TESTASSERT_EQ(0, rlc2.get_buffer_state());

  // We do not write the status report into RLC 1
  // We step trought the timers to let t-PollRetransmission expire
  TESTASSERT_EQ(0, rlc1.get_buffer_state());
  for (int t = 0; t < 45; t++) {
    timers.step_all();
  }
  TESTASSERT_EQ(header_size + payload_size, rlc1.get_buffer_state());

  // Read RETX of POLL_SN and check if it triggered the
  // Status report
  {
    unique_byte_buffer_t pdu_buf = srsran::make_byte_buffer();
    pdu_buf->N_bytes             = rlc1.read_pdu(pdu_buf->msg, 100);
    TESTASSERT_EQ(0, rlc2.get_buffer_state());
    rlc2.write_pdu(pdu_buf->msg, pdu_buf->N_bytes);
    TESTASSERT_EQ(3, rlc2.get_buffer_state());
  }

  return SRSRAN_SUCCESS;
}

int full_rx_window_t_reassembly_expiry(rlc_am_nr_sn_size_t sn_size)
{
  rlc_am_tester tester(false, nullptr);
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("Full RX window and t-Reassmbly expiry test ({} bit SN)", to_number(sn_size));
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  auto cfg = rlc_config_t::default_rlc_am_nr_config(to_number(sn_size));
  if (not rlc1.configure(cfg)) {
    return -1;
  }
  if (not rlc2.configure(cfg)) {
    return -1;
  }
  uint32_t mod_nr = cardinality(cfg.am_nr.tx_sn_field_length);

  // Fill up the RX window
  constexpr uint32_t payload_size = 3; // Give the SDU the size of 3 bytes
  uint32_t           header_size  = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  for (uint32_t sn = 0; sn < am_window_size(sn_size); ++sn) {
    // Write SDU
    unique_byte_buffer_t sdu_buf = srsran::make_byte_buffer();
    sdu_buf->msg[0]              = sn;           // Write the index into the buffer
    sdu_buf->N_bytes             = payload_size; // Give each buffer a size of 3 bytes
    sdu_buf->md.pdcp_sn          = sn;           // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_buf));

    // Read PDU
    unique_byte_buffer_t pdu_buf = srsran::make_byte_buffer();
    pdu_buf->N_bytes             = rlc1.read_pdu(pdu_buf->msg, 100);

    // Write PDUs into RLC 2
    // Do not write SN=0 to fill up the RX window
    if (sn != 0) {
      rlc2.write_pdu(pdu_buf->msg, pdu_buf->N_bytes);
    }
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // Read status PDU
  {
    TESTASSERT_EQ(0, rlc1.get_buffer_state());
    unique_byte_buffer_t status_buf = srsran::make_byte_buffer();
    status_buf->N_bytes             = rlc2.read_pdu(status_buf->msg, 1000);
    rlc1.write_pdu(status_buf->msg, status_buf->N_bytes);
    TESTASSERT_EQ(header_size + payload_size, rlc1.get_buffer_state());
  }
  // Check Rx_Status_Highest
  {
    rlc_am_nr_rx_state_t st = rx2->get_rx_state();
    TESTASSERT_EQ(2048, st.rx_highest_status);
  }

  return SRSRAN_SUCCESS;
}

int main()
{
  // Setup the log message spy to intercept error and warning log entries from RLC
  if (!srslog::install_custom_sink(srsran::log_sink_message_spy::name(),
                                   std::unique_ptr<srsran::log_sink_message_spy>(
                                       new srsran::log_sink_message_spy(srslog::get_default_log_formatter())))) {
    return SRSRAN_ERROR;
  }

  auto* spy = static_cast<srsran::log_sink_message_spy*>(srslog::find_sink(srsran::log_sink_message_spy::name()));
  if (spy == nullptr) {
    return SRSRAN_ERROR;
  }
  srslog::set_default_sink(*spy);

  auto& logger_rlc1 = srslog::fetch_basic_logger("RLC_AM_1", *spy, false);
  auto& logger_rlc2 = srslog::fetch_basic_logger("RLC_AM_2", *spy, false);
  logger_rlc1.set_hex_dump_max_size(100);
  logger_rlc2.set_hex_dump_max_size(100);
  logger_rlc1.set_level(srslog::basic_levels::debug);
  logger_rlc2.set_level(srslog::basic_levels::debug);

  // start log back-end
  srslog::init();
  std::initializer_list<rlc_am_nr_sn_size_t> sn_sizes = {rlc_am_nr_sn_size_t::size12bits,
                                                         rlc_am_nr_sn_size_t::size18bits};
  for (auto sn_size : sn_sizes) {
    TESTASSERT(window_checker_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(retx_segmentation_required_checker_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(basic_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(lost_pdu_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(lost_pdu_duplicated_nack_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(lost_pdus_trimmed_nack_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(clean_retx_queue_of_acked_sdus_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(basic_segmentation_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(segment_retx_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(segment_retx_and_loose_segments_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(retx_segment_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(handle_status_of_non_tx_last_segment(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(max_retx_lost_sdu_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(max_retx_lost_segments_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(discard_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(poll_pdu(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(poll_byte(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(poll_retx(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(poll_retx_expiry(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(rx_nack_range_no_so_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(rx_nack_range_with_so_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(rx_nack_range_with_so_starting_with_full_sdu_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(rx_nack_range_with_so_ending_with_full_sdu_test(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(out_of_order_status(sn_size) == SRSRAN_SUCCESS);
    TESTASSERT(lost_status_and_advanced_rx_window(sn_size) == SRSRAN_SUCCESS);
  }
  TESTASSERT(full_rx_window_t_reassembly_expiry(rlc_am_nr_sn_size_t::size12bits) == SRSRAN_SUCCESS);
  return SRSRAN_SUCCESS;
}
