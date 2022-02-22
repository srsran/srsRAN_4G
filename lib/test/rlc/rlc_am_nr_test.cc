/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  TESTASSERT_EQ(15, rlc->get_buffer_state()); // 2 Bytes * NBUFFS (header size) + NBUFFS (data) = 15

  // Read 5 PDUs from RLC1 (1 byte each)
  for (int i = 0; i < NBUFS; i++) {
    uint32_t len        = rlc->read_pdu(pdu_bufs[i].msg, 3); // 2 bytes for header + 1 byte payload
    pdu_bufs[i].N_bytes = len;
    TESTASSERT_EQ(3, len);
  }

  TESTASSERT_EQ(0, rlc->get_buffer_state());
  return SRSRAN_SUCCESS;
}

/*
 * Test the limits of the TX/RX window checkers
 *
 */
int window_checker_test()
{
  rlc_am_tester tester;
  timer_handler timers(8);

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("window checkers");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);

  rlc_am_nr_tx* tx = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config())) {
    return SRSRAN_ERROR;
  }

  {
    // RLC1 RX_NEXT == 0 and RLC2 TX_NEXT_ACK == 0
    uint32_t sn_inside_below  = 0;
    uint32_t sn_inside_above  = 2047;
    uint32_t sn_outside_below = 4095;
    uint32_t sn_outside_above = 2048;
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
  rx_st.rx_next              = 4095;
  rlc_am_nr_tx_state_t tx_st = {};
  tx_st.tx_next_ack          = 4095;

  rx->set_rx_state(rx_st);
  tx->set_tx_state(tx_st);

  {
    // RX_NEXT == 4095 TX_NEXT_ACK == 4095
    uint32_t sn_inside_below  = 0;
    uint32_t sn_inside_above  = 2046;
    uint32_t sn_outside_below = 4094;
    uint32_t sn_outside_above = 2048;
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
int retx_segmentation_required_checker_test()
{
  rlc_am_tester tester;
  timer_handler timers(8);

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("retx segmentation required checkers");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);

  rlc_am_nr_tx* tx = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config())) {
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
    uint32_t       nof_bytes = 8;
    rlc_amd_retx_t retx      = {};
    retx.sn                  = 0;
    retx.is_segment          = false;

    tx->is_retx_segmentation_required(retx, nof_bytes);
    TESTASSERT_EQ(false, tx->is_retx_segmentation_required(retx, nof_bytes));
  }

  // Test SDU retx segmentation required
  {
    uint32_t       nof_bytes = 4;
    rlc_amd_retx_t retx;
    retx.sn         = 0;
    retx.is_segment = false;

    tx->is_retx_segmentation_required(retx, nof_bytes);
    TESTASSERT_EQ(true, tx->is_retx_segmentation_required(retx, nof_bytes));
  }

  // Test full SDU segment retx
  {
    uint32_t       nof_bytes = 40;
    rlc_amd_retx_t retx      = {};
    retx.sn                  = 0;
    retx.is_segment          = true;
    retx.so_start            = 4;
    retx.so_end              = 6;

    tx->is_retx_segmentation_required(retx, nof_bytes);
    TESTASSERT_EQ(false, tx->is_retx_segmentation_required(retx, nof_bytes));
  }

  // Test SDU segment retx segmentation required
  {
    uint32_t       nof_bytes = 4;
    rlc_amd_retx_t retx      = {};
    retx.sn                  = 0;
    retx.is_segment          = true;
    retx.so_start            = 4;
    retx.so_end              = 6;

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
int basic_test()
{
  rlc_am_tester tester;
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("basic tx/rx");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  // before configuring entity
  TESTASSERT_EQ(0, rlc1.get_buffer_state());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_nr_config())) {
    return -1;
  }

  basic_test_tx(&rlc1, pdu_bufs);

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
  rlc_am_nr_status_pdu_t status_check = {};
  rlc_am_nr_read_status_pdu(&status_buf, rlc_am_nr_sn_size_t::size12bits, &status_check);
  TESTASSERT_EQ(5, status_check.ack_sn); // 5 is the last SN that was not received.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Check TX_NEXT_ACK
  rlc_am_nr_tx_state_t st = tx1->get_tx_state();
  TESTASSERT_EQ(5, st.tx_next_ack);
  TESTASSERT_EQ(0, tx1->get_tx_window_size());

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

  // RLC1 PDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_sdus);
  TESTASSERT_EQ(0, metrics1.num_rx_sdus);
  TESTASSERT_EQ(5, metrics1.num_tx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);
  // RLC1 SDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_pdus);
  TESTASSERT_EQ(1, metrics1.num_rx_pdus);       // One status PDU
  TESTASSERT_EQ(15, metrics1.num_tx_pdu_bytes); // 2 Bytes * NBUFFS (header size) + NBUFFS (data) = 15
  TESTASSERT_EQ(3, metrics1.num_rx_pdu_bytes);  // One status PDU
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);     // No lost SDUs

  // RLC2 PDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_sdus);
  TESTASSERT_EQ(5, metrics2.num_rx_sdus);
  TESTASSERT_EQ(0, metrics2.num_tx_sdu_bytes);
  TESTASSERT_EQ(5, metrics2.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);
  // RLC2 SDU metrics
  TESTASSERT_EQ(1, metrics2.num_tx_pdus);       // One status PDU
  TESTASSERT_EQ(5, metrics2.num_rx_pdus);       // 5 SDUs
  TESTASSERT_EQ(3, metrics2.num_tx_pdu_bytes);  // One status PDU
  TESTASSERT_EQ(15, metrics2.num_rx_pdu_bytes); // 2 Bytes * NBUFFS (header size) + NBUFFS (data) = 15
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);     // No lost SDUs
  return SRSRAN_SUCCESS;
}

/*
 * Test the loss of a single PDU.
 * NACK should be visible in the status report.
 * Retx after NACK should be present too.
 */
int lost_pdu_test()
{
  rlc_am_tester tester;
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  test_delimit_logger delimiter("lost PDU");

  // before configuring entity
  TESTASSERT(0 == rlc1.get_buffer_state());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_nr_config())) {
    return -1;
  }

  basic_test_tx(&rlc1, pdu_bufs);

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
    rlc_am_nr_status_pdu_t status_check = {};
    rlc_am_nr_read_status_pdu(&status_buf, rlc_am_nr_sn_size_t::size12bits, &status_check);
    TESTASSERT_EQ(3, status_check.ack_sn); // 3 is the next expected SN (i.e. the lost packet.)

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // t-reassembly has expired. There should be a NACK in the status report.
  TESTASSERT_EQ(5, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, 5);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check = {};
    rlc_am_nr_read_status_pdu(&status_buf, rlc_am_nr_sn_size_t::size12bits, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);           // 5 is the next expected SN.
    TESTASSERT_EQ(1, status_check.N_nack);           // We lost one PDU.
    TESTASSERT_EQ(3, status_check.nacks[0].nack_sn); // Lost PDU SN=3.

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there is an Retx of SN=3
    TESTASSERT_EQ(3, rlc1.get_buffer_state());
  }

  {
    // Check correct re-transmission
    byte_buffer_t retx_buf;
    int           len = rlc1.read_pdu(retx_buf.msg, 3);
    retx_buf.N_bytes  = len;
    TESTASSERT_EQ(3, len);

    rlc2.write_pdu(retx_buf.msg, retx_buf.N_bytes);

    TESTASSERT_EQ(0, rlc2.get_buffer_state());
  }

  // Check statistics
  rlc_bearer_metrics_t metrics1 = rlc1.get_metrics();
  rlc_bearer_metrics_t metrics2 = rlc2.get_metrics();

  // SDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_sdus);
  TESTASSERT_EQ(0, metrics1.num_rx_sdus);
  TESTASSERT_EQ(5, metrics1.num_tx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);
  // PDU metrics
  TESTASSERT_EQ(5 + 1, metrics1.num_tx_pdus);      // One re-transmission
  TESTASSERT_EQ(2, metrics1.num_rx_pdus);          // One status PDU
  TESTASSERT_EQ(18, metrics1.num_tx_pdu_bytes);    // 2 Bytes * NBUFFS (header size) + NBUFFS (data) + 1 rext (3) = 18
  TESTASSERT_EQ(3 + 5, metrics1.num_rx_pdu_bytes); // Two status PDU (one with a NACK)
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);        // No lost SDUs

  // PDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_sdus);
  TESTASSERT_EQ(5, metrics2.num_rx_sdus);
  TESTASSERT_EQ(0, metrics2.num_tx_sdu_bytes);
  TESTASSERT_EQ(5, metrics2.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);
  // SDU metrics
  TESTASSERT_EQ(2, metrics2.num_tx_pdus);          // Two status PDUs
  TESTASSERT_EQ(5, metrics2.num_rx_pdus);          // 5 PDUs (6 tx'ed, but one was lost)
  TESTASSERT_EQ(5 + 3, metrics2.num_tx_pdu_bytes); // Two status PDU (one with a NACK)
  TESTASSERT_EQ(15, metrics2.num_rx_pdu_bytes);    // 2 Bytes * NBUFFS (header size) + NBUFFS (data) = 15
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);        // No lost SDUs
  return SRSRAN_SUCCESS;
}

/*
 * Test the basic segmentation of a single SDU.
 * A single SDU of 3 bytes is segmented into 3 PDUs
 */
int basic_segmentation_test()
{
  rlc_am_tester       tester;
  timer_handler       timers(8);
  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  test_delimit_logger delimiter("basic segmentation");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  // before configuring entity
  TESTASSERT_EQ(0, rlc1.get_buffer_state());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_nr_config())) {
    return -1;
  }

  // Push 1 SDU into RLC1
  unique_byte_buffer_t sdu;
  sdu = srsran::make_byte_buffer();
  TESTASSERT(nullptr != sdu);
  sdu->msg[0]     = 0; // Write the index into the buffer
  sdu->N_bytes    = 3; // Give the SDU the size of 3 bytes
  sdu->md.pdcp_sn = 0; // PDCP SN for notifications
  rlc1.write_sdu(std::move(sdu));

  // Read 3 PDUs
  constexpr uint16_t   n_pdus = 3;
  unique_byte_buffer_t pdu_bufs[n_pdus];
  for (int i = 0; i < 3; i++) {
    pdu_bufs[i] = srsran::make_byte_buffer();
    TESTASSERT(nullptr != pdu_bufs[i]);
    if (i == 0) {
      pdu_bufs[i]->N_bytes = rlc1.read_pdu(pdu_bufs[i]->msg, 3);
      TESTASSERT_EQ(3, pdu_bufs[i]->N_bytes);
    } else {
      pdu_bufs[i]->N_bytes = rlc1.read_pdu(pdu_bufs[i]->msg, 5);
      TESTASSERT_EQ(5, pdu_bufs[i]->N_bytes);
    }
  }

  // Write 5 PDUs into RLC2
  for (int i = 0; i < n_pdus; i++) {
    rlc2.write_pdu(pdu_bufs[i]->msg, pdu_bufs[i]->N_bytes); // Don't write RLC_SN=3.
  }

  // Check statistics
  rlc_bearer_metrics_t metrics1 = rlc1.get_metrics();
  rlc_bearer_metrics_t metrics2 = rlc2.get_metrics();

  // SDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_sdus);
  TESTASSERT_EQ(1, metrics2.num_rx_sdus);
  TESTASSERT_EQ(0, metrics2.num_tx_sdu_bytes);
  TESTASSERT_EQ(3, metrics2.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);
  // PDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_pdus);
  TESTASSERT_EQ(3, metrics2.num_rx_pdus);       // 5 PDUs (6 tx'ed, but one was lost)
  TESTASSERT_EQ(0, metrics2.num_tx_pdu_bytes);  // Two status PDU (one with a NACK)
  TESTASSERT_EQ(13, metrics2.num_rx_pdu_bytes); // 1 PDU (No SO) + 2 PDUs (with SO) = 3 + 2*5
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);     // No lost SDUs

  // Check state
  rlc_am_nr_tx_state_t state1_tx = tx1->get_tx_state();
  TESTASSERT_EQ(1, state1_tx.tx_next);

  return SRSRAN_SUCCESS;
}

int segment_retx_test()
{
  rlc_am_tester tester;
  timer_handler timers(8);
  byte_buffer_t pdu_bufs[NBUFS];

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  test_delimit_logger delimiter("segment retx PDU");

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  // before configuring entity
  TESTASSERT_EQ(0, rlc1.get_buffer_state());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_nr_config())) {
    return -1;
  }

  // Push 5 SDUs into RLC1
  unique_byte_buffer_t sdu_bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i; // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = 3; // Give each buffer a size of 3 bytes
    sdu_bufs[i]->md.pdcp_sn = i; // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT_EQ(25, rlc1.get_buffer_state()); // 2 Bytes * NBUFFS (header size) + NBUFFS * 3 (data) = 25

  // Read 5 PDUs from RLC1 (1 byte each)
  for (int i = 0; i < NBUFS; i++) {
    uint32_t len        = rlc1.read_pdu(pdu_bufs[i].msg, 5); // 2 bytes for header + 3 byte payload
    pdu_bufs[i].N_bytes = len;
    TESTASSERT_EQ(5, len);
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
    rlc_am_nr_status_pdu_t status_check = {};
    rlc_am_nr_read_status_pdu(&status_buf, rlc_am_nr_sn_size_t::size12bits, &status_check);
    TESTASSERT_EQ(3, status_check.ack_sn); // 3 is the next expected SN (i.e. the lost packet.)

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // t-reassembly has expired. There should be a NACK in the status report.
  TESTASSERT_EQ(5, rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, 5);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check = {};
    rlc_am_nr_read_status_pdu(&status_buf, rlc_am_nr_sn_size_t::size12bits, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);           // 5 is the next expected SN.
    TESTASSERT_EQ(1, status_check.N_nack);           // We lost one PDU.
    TESTASSERT_EQ(3, status_check.nacks[0].nack_sn); // Lost PDU SN=3.

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there is an Retx of SN=3
    TESTASSERT_EQ(5, rlc1.get_buffer_state());
  }

  {
    // Re-transmit PDU in 3 segments
    for (int i = 0; i < 3; i++) {
      byte_buffer_t retx_buf;
      uint32_t      len = 0;
      if (i == 0) {
        len = rlc1.read_pdu(retx_buf.msg, 3);
        TESTASSERT_EQ(3, len);
      } else {
        len = rlc1.read_pdu(retx_buf.msg, 5);
        TESTASSERT_EQ(5, len);
      }
      retx_buf.N_bytes = len;

      rlc_am_nr_pdu_header_t header_check = {};
      uint32_t hdr_len = rlc_am_nr_read_data_pdu_header(&retx_buf, rlc_am_nr_sn_size_t::size12bits, &header_check);
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

  // SDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_sdus);
  TESTASSERT_EQ(0, metrics1.num_rx_sdus);
  TESTASSERT_EQ(15, metrics1.num_tx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);
  // PDU metrics
  TESTASSERT_EQ(5 + 3, metrics1.num_tx_pdus);      // 3 re-transmissions
  TESTASSERT_EQ(2, metrics1.num_rx_pdus);          // Two status PDU
  TESTASSERT_EQ(38, metrics1.num_tx_pdu_bytes);    // 2 Bytes * NBUFFS (header size) + NBUFFS * 3 (data) +
                                                   // 3 (1 retx no SO) + 2 * 5 (2 retx with SO) = 38
  TESTASSERT_EQ(3 + 5, metrics1.num_rx_pdu_bytes); // Two status PDU (one with a NACK)
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);        // No lost SDUs

  // PDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_sdus);
  TESTASSERT_EQ(5, metrics2.num_rx_sdus);
  TESTASSERT_EQ(0, metrics2.num_tx_sdu_bytes);
  TESTASSERT_EQ(15, metrics2.num_rx_sdu_bytes); // 5 SDUs, 3 bytes each
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);
  // SDU metrics
  TESTASSERT_EQ(2, metrics2.num_tx_pdus);          // Two status PDUs
  TESTASSERT_EQ(7, metrics2.num_rx_pdus);          // 7 PDUs (8 tx'ed, but one was lost)
  TESTASSERT_EQ(5 + 3, metrics2.num_tx_pdu_bytes); // Two status PDU (one with a NACK)
  TESTASSERT_EQ(33, metrics2.num_rx_pdu_bytes);    // 2 Bytes * (NBUFFS-1) (header size) + (NBUFFS-1) * 3 (data)
                                                   // 3 (1 retx no SO) + 2 * 5 (2 retx with SO) = 33
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);        // No lost SDUs

  // Check state
  rlc_am_nr_rx_state_t state2_rx = rx2->get_rx_state();
  TESTASSERT_EQ(5, state2_rx.rx_next);
  return SRSRAN_SUCCESS;
}

int retx_segment_test()
{
  rlc_am_tester tester;
  timer_handler timers(8);

  auto&               test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am              rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am              rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);
  test_delimit_logger delimiter("retx segment PDU");

  rlc_am_nr_tx* tx1 = dynamic_cast<rlc_am_nr_tx*>(rlc1.get_tx());
  rlc_am_nr_rx* rx1 = dynamic_cast<rlc_am_nr_rx*>(rlc1.get_rx());
  rlc_am_nr_tx* tx2 = dynamic_cast<rlc_am_nr_tx*>(rlc2.get_tx());
  rlc_am_nr_rx* rx2 = dynamic_cast<rlc_am_nr_rx*>(rlc2.get_rx());

  // before configuring entity
  TESTASSERT(0 == rlc1.get_buffer_state());

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config())) {
    return -1;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_nr_config())) {
    return -1;
  }

  int n_sdu_bufs = 5;
  int n_pdu_bufs = 15;

  // Push 5 SDUs into RLC1
  std::vector<unique_byte_buffer_t> sdu_bufs(n_sdu_bufs);
  for (int i = 0; i < n_sdu_bufs; i++) {
    sdu_bufs[i]             = srsran::make_byte_buffer();
    sdu_bufs[i]->msg[0]     = i; // Write the index into the buffer
    sdu_bufs[i]->N_bytes    = 3; // Give each buffer a size of 3 bytes
    sdu_bufs[i]->md.pdcp_sn = i; // PDCP SN for notifications
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT(25 == rlc1.get_buffer_state()); // 2 Bytes * NBUFFS (header size) + NBUFFS * 3 (data) = 25

  // Read 15 PDUs from RLC1
  std::vector<unique_byte_buffer_t> pdu_bufs(n_pdu_bufs);
  for (int i = 0; i < n_pdu_bufs; i++) {
    pdu_bufs[i] = srsran::make_byte_buffer();
    if (i == 0 || i == 3 || i == 6 || i == 9 || i == 12) {
      // First segment, no SO
      uint32_t len         = rlc1.read_pdu(pdu_bufs[i]->msg, 3); // 2 bytes for header + 1 byte payload
      pdu_bufs[i]->N_bytes = len;
      TESTASSERT_EQ(3, len);
    } else {
      // Middle or last segment, SO present
      uint32_t len         = rlc1.read_pdu(pdu_bufs[i]->msg, 5); // 4 bytes for header + 1 byte payload
      pdu_bufs[i]->N_bytes = len;
      TESTASSERT_EQ(5, len);
    }
  }

  TESTASSERT_EQ(0, rlc1.get_buffer_state());

  // Write 15 - 3 PDUs into RLC2
  for (int i = 0; i < n_pdu_bufs; i++) {
    if (i != 3 && i != 7 && i != 11) {
      rlc2.write_pdu(pdu_bufs[i]->msg, pdu_bufs[i]->N_bytes); // Lose first segment of RLC_SN=1.
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
    rlc_am_nr_status_pdu_t status_check = {};
    rlc_am_nr_read_status_pdu(&status_buf, rlc_am_nr_sn_size_t::size12bits, &status_check);
    TESTASSERT_EQ(1, status_check.ack_sn); // 1 is the next expected SN (i.e. the first lost packet.)

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // t-reassembly has expired. There should be a NACK in the status report.
  // There should be 3 NACKs with SO_start and SO_end
  TESTASSERT_EQ(21, rlc2.get_buffer_state()); // 3 bytes for fixed header (ACK+E1) + 3 * 6 for NACK with SO = 21.
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, 21);
    status_buf.N_bytes = len;

    TESTASSERT_EQ(0, rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check = {};
    rlc_am_nr_read_status_pdu(&status_buf, rlc_am_nr_sn_size_t::size12bits, &status_check);
    TESTASSERT_EQ(5, status_check.ack_sn);               // 5 is the next expected SN.
    TESTASSERT_EQ(3, status_check.N_nack);               // We lost one PDU.
    TESTASSERT_EQ(1, status_check.nacks[0].nack_sn);     // Lost SDU on SN=1.
    TESTASSERT_EQ(true, status_check.nacks[0].has_so);   // Lost SDU on SN=1.
    TESTASSERT_EQ(0, status_check.nacks[0].so_start);    // Lost SDU on SN=1.
    TESTASSERT_EQ(1, status_check.nacks[0].so_end);      // Lost SDU on SN=1.
    TESTASSERT_EQ(2, status_check.nacks[1].nack_sn);     // Lost SDU on SN=1.
    TESTASSERT_EQ(true, status_check.nacks[1].has_so);   // Lost SDU on SN=1.
    TESTASSERT_EQ(1, status_check.nacks[1].so_start);    // Lost SDU on SN=1.
    TESTASSERT_EQ(2, status_check.nacks[1].so_end);      // Lost SDU on SN=1.
    TESTASSERT_EQ(3, status_check.nacks[2].nack_sn);     // Lost SDU on SN=1.
    TESTASSERT_EQ(true, status_check.nacks[2].has_so);   // Lost SDU on SN=1.
    TESTASSERT_EQ(2, status_check.nacks[2].so_start);    // Lost SDU on SN=1.
    TESTASSERT_EQ(0xFFFF, status_check.nacks[2].so_end); // Lost SDU on SN=1.

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there is an Retx of SN=3
    TESTASSERT_EQ(5, rlc1.get_buffer_state());
  }

  {
    // Re-transmit the 3 lost segments
    for (int i = 0; i < 3; i++) {
      byte_buffer_t retx_buf;
      uint32_t      len = 0;
      if (i == 0) {
        len = rlc1.read_pdu(retx_buf.msg, 3);
        TESTASSERT_EQ(3, len);
      } else {
        len = rlc1.read_pdu(retx_buf.msg, 5);
        TESTASSERT_EQ(5, len);
      }
      retx_buf.N_bytes = len;

      rlc_am_nr_pdu_header_t header_check = {};
      uint32_t hdr_len = rlc_am_nr_read_data_pdu_header(&retx_buf, rlc_am_nr_sn_size_t::size12bits, &header_check);
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

  // SDU metrics
  TESTASSERT_EQ(5, metrics1.num_tx_sdus);
  TESTASSERT_EQ(0, metrics1.num_rx_sdus);
  TESTASSERT_EQ(15, metrics1.num_tx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_rx_sdu_bytes);
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);

  // PDU metrics
  TESTASSERT_EQ(15 + 3, metrics1.num_tx_pdus);  // 15 PDUs + 3 re-transmissions
  TESTASSERT_EQ(2, metrics1.num_rx_pdus);       // Two status PDU
  TESTASSERT_EQ(78, metrics1.num_tx_pdu_bytes); // 3 Bytes * 5 (5 PDUs without SO) + 10 * 5 (10 PDUs with SO)
                                                // 3 (1 retx no SO) + 2 * 5 (2 retx with SO) = 78
  TESTASSERT_EQ(24, metrics1.num_rx_pdu_bytes); // Two status PDU. One with just an ack (3 bytes)
                                                // Another with 3 NACKs all with SO. (3 + 3*6 bytes)
  TESTASSERT_EQ(0, metrics1.num_lost_sdus);     // No lost SDUs

  // PDU metrics
  TESTASSERT_EQ(0, metrics2.num_tx_sdus);
  TESTASSERT_EQ(5, metrics2.num_rx_sdus);
  TESTASSERT_EQ(0, metrics2.num_tx_sdu_bytes);
  TESTASSERT_EQ(15, metrics2.num_rx_sdu_bytes); // 5 SDUs, 3 bytes each
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);
  // SDU metrics
  TESTASSERT_EQ(2, metrics2.num_tx_pdus);       // Two status PDUs
  TESTASSERT_EQ(15, metrics2.num_rx_pdus);      // 15 PDUs (18 tx'ed, but three were lost)
  TESTASSERT_EQ(24, metrics2.num_tx_pdu_bytes); // Two status PDU. One with just an ack (3 bytes)
                                                // Another with 3 NACKs all with SO. (3 + 3*6 bytes)
  TESTASSERT_EQ(65, metrics2.num_rx_pdu_bytes); // 3 Bytes (header + data size, without SO) * 5 (N PDUs without SO)
                                                // 5 bytes (header + data size, with SO) * 10 (N PDUs with SO)
                                                // = 81 bytes
  TESTASSERT_EQ(0, metrics2.num_lost_sdus);     // No lost SDUs

  // Check state
  rlc_am_nr_rx_state_t state2_rx = rx2->get_rx_state();
  TESTASSERT_EQ(5, state2_rx.rx_next);

  return SRSRAN_SUCCESS;
}

// This test checks the correct functioning of RLC discard functionality
int discard_test()
{
  rlc_am_tester tester;
  timer_handler timers(8);

  auto&  test_logger = srslog::fetch_basic_logger("TESTER  ");
  rlc_am rlc1(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_1"), 1, &tester, &tester, &timers);
  rlc_am rlc2(srsran_rat_t::nr, srslog::fetch_basic_logger("RLC_AM_2"), 1, &tester, &tester, &timers);

  srslog::fetch_basic_logger("RLC_AM_1").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC_AM_2").set_hex_dump_max_size(100);
  srslog::fetch_basic_logger("RLC").set_hex_dump_max_size(100);

  if (not rlc1.configure(rlc_config_t::default_rlc_am_nr_config())) {
    return SRSRAN_ERROR;
  }

  if (not rlc2.configure(rlc_config_t::default_rlc_am_nr_config())) {
    return SRSRAN_ERROR;
  }

  // Test discarding the single SDU from the queue
  {
    uint32_t num_tx_sdus = 1;
    for (uint32_t i = 0; i < num_tx_sdus; ++i) {
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
  rlc1.discard_sdu(0); // Try to discard PDCP_SN=0
  TESTASSERT(rlc1.has_data() == false);

  // Test discarding two SDUs in the middle (SN=3) and end (SN=9) of the queue and read PDUs after
  {
    uint32_t num_tx_sdus = 10;
    for (uint32_t i = 0; i < num_tx_sdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes = 7;
      for (uint32_t k = 0; k < sdu->N_bytes; ++k) {
        sdu->msg[k] = i; // Write the index into the buffer
      }
      sdu->md.pdcp_sn = i;
      rlc1.write_sdu(std::move(sdu));
    }
  }
  TESTASSERT(rlc1.get_buffer_state() == 90); // 10 * (2B Header + 7B Payload)
  rlc1.discard_sdu(3);                       // Try to discard PDCP_SN=3
  TESTASSERT(rlc1.has_data() == true);
  TESTASSERT(rlc1.get_buffer_state() == 81);
  rlc1.discard_sdu(9); // Try to discard PDCP_SN=9
  TESTASSERT(rlc1.has_data() == true);
  TESTASSERT(rlc1.get_buffer_state() == 72);

  {
    uint32_t num_tx_sdus = 8;
    for (uint32_t i = 0; i < num_tx_sdus; ++i) {
      unique_byte_buffer_t pdu = srsran::make_byte_buffer();
      uint32_t             len = rlc1.read_pdu(pdu->msg, 50); // sufficient space to read without segmentation
      pdu->N_bytes             = len;
      TESTASSERT((2 + 7) == len);
      // Check that we don't have any SN gaps
      rlc_am_nr_pdu_header_t header = {};
      rlc_am_nr_read_data_pdu_header(pdu.get(), rlc_am_nr_sn_size_t::size12bits, &header);
      TESTASSERT(header.sn == i);
    }
  }
  TESTASSERT(rlc1.has_data() == false);

  srslog::fetch_basic_logger("TEST").info("Received %zd SDUs", tester.sdus.size());

  // Test discarding non-existing SDU from the queue
  {
    uint32_t num_tx_sdus = 3;
    for (uint32_t i = 0; i < num_tx_sdus; ++i) {
      // Write SDU
      unique_byte_buffer_t sdu = srsran::make_byte_buffer();
      TESTASSERT(sdu != nullptr);
      sdu->N_bytes = 7;
      for (uint32_t k = 0; k < sdu->N_bytes; ++k) {
        sdu->msg[k] = i; // Write the index into the buffer
      }
      sdu->md.pdcp_sn = i;
      rlc1.write_sdu(std::move(sdu));
    }
  }
  TESTASSERT(rlc1.get_buffer_state() == 27); // 3 * (2B Header + 7B Payload)
  rlc1.discard_sdu(8);                       // Try to discard PDCP_SN=8, which doesn't exist
  TESTASSERT(rlc1.get_buffer_state() == 27); // 3 * (2B Header + 7B Payload)

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
  TESTASSERT(window_checker_test() == SRSRAN_SUCCESS);
  TESTASSERT(retx_segmentation_required_checker_test() == SRSRAN_SUCCESS);
  TESTASSERT(basic_test() == SRSRAN_SUCCESS);
  TESTASSERT(lost_pdu_test() == SRSRAN_SUCCESS);
  TESTASSERT(basic_segmentation_test() == SRSRAN_SUCCESS);
  TESTASSERT(segment_retx_test() == SRSRAN_SUCCESS);
  TESTASSERT(retx_segment_test() == SRSRAN_SUCCESS);
  TESTASSERT(discard_test() == SRSRAN_SUCCESS);
  return SRSRAN_SUCCESS;
}
