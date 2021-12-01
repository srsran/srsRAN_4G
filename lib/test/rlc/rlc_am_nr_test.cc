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

  TESTASSERT(15 == rlc->get_buffer_state()); // 2 Bytes * NBUFFS (header size) + NBUFFS (data) = 15

  // Read 5 PDUs from RLC1 (1 byte each)
  for (int i = 0; i < NBUFS; i++) {
    uint32_t len        = rlc->read_pdu(pdu_bufs[i].msg, 3); // 2 bytes for header + 1 byte payload
    pdu_bufs[i].N_bytes = len;
    TESTASSERT_EQ(3, len);
  }

  TESTASSERT(0 == rlc->get_buffer_state());
  return SRSRAN_SUCCESS;
}

/*
 * Test the limits of the TX/RX window checkers
 *
 * This will test
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
    rlc2.write_pdu(pdu_bufs[i].msg, pdu_bufs[i].N_bytes);
  }

  TESTASSERT(3 == rlc2.get_buffer_state());
  // Read status PDU from RLC2
  byte_buffer_t status_buf;
  int           len  = rlc2.read_pdu(status_buf.msg, 3);
  status_buf.N_bytes = len;

  TESTASSERT(0 == rlc2.get_buffer_state());

  // Assert status is correct
  rlc_am_nr_status_pdu_t status_check = {};
  rlc_am_nr_read_status_pdu(&status_buf, rlc_am_nr_sn_size_t::size12bits, &status_check);
  TESTASSERT(status_check.ack_sn == 5); // 5 is the last SN that was not received.

  // Write status PDU to RLC1
  rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

  // Check TX_NEXT_ACK
  rlc_am_nr_tx_state_t st = tx1->get_tx_state();
  TESTASSERT_EQ(5, st.tx_next_ack);
  TESTASSERT_EQ(0, tx1->get_tx_window_size());

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
  TESTASSERT(3 == rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, 5);
    status_buf.N_bytes = len;

    TESTASSERT(0 == rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check = {};
    rlc_am_nr_read_status_pdu(&status_buf, rlc_am_nr_sn_size_t::size12bits, &status_check);
    TESTASSERT(status_check.ack_sn == 3); // 3 is the next expected SN (i.e. the lost packet.)

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);
  }

  // Step timers until reassambly timeout expires
  for (int cnt = 0; cnt < 35; cnt++) {
    timers.step_all();
  }

  // t-reassembly has expired. There should be a NACK in the status report.
  TESTASSERT(5 == rlc2.get_buffer_state());
  {
    // Read status PDU from RLC2
    byte_buffer_t status_buf;
    int           len  = rlc2.read_pdu(status_buf.msg, 5);
    status_buf.N_bytes = len;

    TESTASSERT(0 == rlc2.get_buffer_state());

    // Assert status is correct
    rlc_am_nr_status_pdu_t status_check = {};
    rlc_am_nr_read_status_pdu(&status_buf, rlc_am_nr_sn_size_t::size12bits, &status_check);
    TESTASSERT(status_check.ack_sn == 5);           // 5 is the next expected SN.
    TESTASSERT(status_check.N_nack == 1);           // We lost one PDU.
    TESTASSERT(status_check.nacks[0].nack_sn == 3); // Lost PDU SN=3.

    // Write status PDU to RLC1
    rlc1.write_pdu(status_buf.msg, status_buf.N_bytes);

    // Check there is an Retx of SN=3
    TESTASSERT(3 == rlc1.get_buffer_state());
  }

  {
    // Check correct re-transmission
    byte_buffer_t retx_buf;
    int           len = rlc1.read_pdu(retx_buf.msg, 3);
    retx_buf.N_bytes  = len;
    TESTASSERT(3 == len);

    rlc2.write_pdu(retx_buf.msg, retx_buf.N_bytes);

    TESTASSERT(0 == rlc2.get_buffer_state());
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

  // before configuring entity
  TESTASSERT(0 == rlc1.get_buffer_state());

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

  // start log backend
  srslog::init();

  TESTASSERT(window_checker_test() == SRSRAN_SUCCESS);
  TESTASSERT(basic_test() == SRSRAN_SUCCESS);
  TESTASSERT(lost_pdu_test() == SRSRAN_SUCCESS);
  TESTASSERT(basic_segmentation_test() == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}
