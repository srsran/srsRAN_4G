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
#include "pdcp_lte_test.h"
#include <numeric>

/*
 * Test correct transmission of FMS and Bitmap in status report
 */
int test_tx_status_report(const srslte::pdcp_lte_state_t& init_state, srslog::basic_logger& logger)
{
  srslte::pdcp_config_t cfg_tx = {1,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  srslte::PDCP_SN_LEN_12,
                                  srslte::pdcp_t_reordering_t::ms500,
                                  srslte::pdcp_discard_timer_t::ms500,
                                  true};

  srslte::pdcp_config_t cfg_rx = {1,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  srslte::PDCP_SN_LEN_12,
                                  srslte::pdcp_t_reordering_t::ms500,
                                  srslte::pdcp_discard_timer_t::ms500,
                                  true};

  // Setup TX
  pdcp_lte_test_helper     pdcp_hlp_tx(cfg_tx, sec_cfg, logger);
  srslte::pdcp_entity_lte* pdcp_tx  = &pdcp_hlp_tx.pdcp;
  rlc_dummy*               rlc_tx   = &pdcp_hlp_tx.rlc;
  srsue::stack_test_dummy* stack_tx = &pdcp_hlp_tx.stack;
  pdcp_hlp_tx.set_pdcp_initial_state(init_state);

  // Setup RX
  pdcp_lte_test_helper     pdcp_hlp_rx(cfg_tx, sec_cfg, logger);
  srslte::pdcp_entity_lte* pdcp_rx  = &pdcp_hlp_tx.pdcp;
  rlc_dummy*               rlc_rx   = &pdcp_hlp_tx.rlc;
  srsue::stack_test_dummy* stack_rx = &pdcp_hlp_tx.stack;
  pdcp_hlp_rx.set_pdcp_initial_state(init_state);

  // Tmp variable to hold the PDCP PDU
  srslte::unique_byte_buffer_t out_pdu = srslte::make_byte_buffer();

  // Write 256 SDUs and notify imediatly -> FMS 0001 0000 0001
  for (uint32_t i = 0; i < 257; i++) {
    srslte::unique_byte_buffer_t sdu = srslte::make_byte_buffer();
    srslte::unique_byte_buffer_t pdu = srslte::make_byte_buffer();
    sdu->append_bytes(sdu1, sizeof(sdu1));
    pdcp_tx->write_sdu(std::move(sdu));
    pdcp_tx->notify_delivery({i});
    rlc_tx->get_last_sdu(pdu);
    pdcp_rx->write_pdu(std::move(pdu));
  }

  // Check undelivered SDUs queue size
  TESTASSERT(pdcp_tx->nof_discard_timers() == 0); // 0 timers should be running on TX

  // Generate the status report
  pdcp_rx->send_status_report();
  rlc_rx->get_last_sdu(out_pdu);
  logger.debug(out_pdu->msg, out_pdu->N_bytes, "Status PDU:");

  // Check status PDU. We received up to
  /*
   *  | D/C | TYPE | FMS | -> | 0 | 0 | 0001 |
   *  |       FMS        | -> | 00000001     |
   */
  TESTASSERT(out_pdu->msg[0] == 1);
  TESTASSERT(out_pdu->msg[1] == 1);
  TESTASSERT(out_pdu->N_bytes == 2);

  // Write another 16 SDUs but don't notify SN=257, SN=258, SN=271 and SN=272
  for (uint32_t i = 257; i < 273; i++) {
    srslte::unique_byte_buffer_t sdu = srslte::make_byte_buffer();
    srslte::unique_byte_buffer_t pdu = srslte::make_byte_buffer();
    sdu->append_bytes(sdu1, sizeof(sdu1));
    pdcp_tx->write_sdu(std::move(sdu));
    if (i != 257 && i != 258 && i != 271 && i != 272) {
      pdcp_tx->notify_delivery({i});
      rlc_tx->get_last_sdu(pdu);
      pdcp_rx->write_pdu(std::move(pdu));
    }
  }

  // Check undelivered SDUs queue size
  TESTASSERT(pdcp_tx->nof_discard_timers() == 4);

  // Generate the status report
  pdcp_tx->send_status_report();
  rlc_tx->get_last_sdu(out_pdu);
  logger.debug(out_pdu->msg, out_pdu->N_bytes, "Status PDU:");

  // Check status PDU
  /*
   *  | D/C | TYPE | FMS | -> | 0 | 0 | 0001 |
   *  |       FMS        | -> | 00000001     |
   *  |     bitmap       | -> | 01111111     |
   *  |  bitmap (cont.)  | -> | 00000011     |
   */
  TESTASSERT(out_pdu->N_bytes == 4);
  TESTASSERT(out_pdu->msg[0] == 0b00000001); // FMS = 257
  TESTASSERT(out_pdu->msg[1] == 0b00000001);
  TESTASSERT(out_pdu->msg[2] == 0b01111111); // FMS + 1 is missing
  TESTASSERT(out_pdu->msg[3] == 0b11111000); //
  return 0;
}

/*
 * Test correct transmission of FMS and Bitmap in status report
 */
int test_tx_wraparound_status_report(const srslte::pdcp_lte_state_t& init_state, srslog::basic_logger& logger)
{
  srslte::pdcp_config_t cfg_tx = {1,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  srslte::PDCP_SN_LEN_12,
                                  srslte::pdcp_t_reordering_t::ms500,
                                  srslte::pdcp_discard_timer_t::ms500,
                                  true};

  srslte::pdcp_config_t cfg_rx = {1,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  srslte::PDCP_SN_LEN_12,
                                  srslte::pdcp_t_reordering_t::ms500,
                                  srslte::pdcp_discard_timer_t::ms500,
                                  true};

  // Setup TX
  pdcp_lte_test_helper     pdcp_hlp_tx(cfg_tx, sec_cfg, logger);
  srslte::pdcp_entity_lte* pdcp_tx  = &pdcp_hlp_tx.pdcp;
  rlc_dummy*               rlc_tx   = &pdcp_hlp_tx.rlc;
  srsue::stack_test_dummy* stack_tx = &pdcp_hlp_tx.stack;
  pdcp_hlp_tx.set_pdcp_initial_state(init_state);

  // Setup RX
  pdcp_lte_test_helper     pdcp_hlp_rx(cfg_tx, sec_cfg, logger);
  srslte::pdcp_entity_lte* pdcp_rx  = &pdcp_hlp_tx.pdcp;
  rlc_dummy*               rlc_rx   = &pdcp_hlp_tx.rlc;
  srsue::stack_test_dummy* stack_rx = &pdcp_hlp_tx.stack;
  pdcp_hlp_rx.set_pdcp_initial_state(init_state);

  srslte::unique_byte_buffer_t out_pdu = srslte::make_byte_buffer();

  // Write 256 SDUs and notify imediatly -> FMS 1111 1111 0000
  for (uint32_t i = 0; i < 4080; i++) {
    srslte::unique_byte_buffer_t sdu = srslte::make_byte_buffer();
    srslte::unique_byte_buffer_t pdu = srslte::make_byte_buffer();
    sdu->append_bytes(sdu1, sizeof(sdu1));
    pdcp_tx->write_sdu(std::move(sdu));
    pdcp_tx->notify_delivery({i});
    rlc_tx->get_last_sdu(pdu);
    pdcp_rx->write_pdu(std::move(pdu));
  }

  // Check undelivered SDUs queue size
  TESTASSERT(pdcp_tx->nof_discard_timers() == 0); // 0 timers should be running

  // Generate the status report
  pdcp_rx->send_status_report();
  rlc_rx->get_last_sdu(out_pdu);
  logger.debug(out_pdu->msg, out_pdu->N_bytes, "Status PDU:");

  // Check status PDU
  /*
   *  | D/C | TYPE | FMS | -> | 0 | 000 | 1111 |
   *  |       FMS        | -> |   11110000     |
   */
  TESTASSERT(out_pdu->msg[0] == 15);
  TESTASSERT(out_pdu->msg[1] == 240);
  TESTASSERT(out_pdu->N_bytes == 2);

  // Write another 32 SDUs but don't notify SN=4080, SN=4081, SN=14 and SN=15
  for (uint32_t i = 4080; i < 4112; i++) {
    srslte::unique_byte_buffer_t sdu = srslte::make_byte_buffer();
    srslte::unique_byte_buffer_t pdu = srslte::make_byte_buffer();
    sdu->append_bytes(sdu1, sizeof(sdu1));
    pdcp_tx->write_sdu(std::move(sdu));
    if (i != 4080 && i != 4081 && i != 4110 && i != 4111) {
      pdcp_tx->notify_delivery({i % 4096});
      rlc_tx->get_last_sdu(pdu);
      pdcp_rx->write_pdu(std::move(pdu));
    }
  }

  // Check undelivered SDUs queue size
  TESTASSERT(pdcp_tx->nof_discard_timers() == 4);

  // Generate the status report
  pdcp_rx->send_status_report();
  rlc_rx->get_last_sdu(out_pdu);
  logger.debug(out_pdu->msg, out_pdu->N_bytes, "Status PDU:");

  // Check status PDU
  /*
   *  | D/C | TYPE | FMS | -> | 0 | 0 | 0001 |
   *  |       FMS        | -> | 00000001     |
   *  |     bitmap       | -> | 11000000     |
   *  |  bitmap (cont.)  | -> | 00000011     |
   */
  TESTASSERT(out_pdu->N_bytes == 6);
  TESTASSERT(out_pdu->msg[0] == 0b00001111);
  TESTASSERT(out_pdu->msg[1] == 0b11110000);
  TESTASSERT(out_pdu->msg[2] == 0b01111111);
  TESTASSERT(out_pdu->msg[3] == 0b11111111);
  TESTASSERT(out_pdu->msg[4] == 0b11111111);
  TESTASSERT(out_pdu->msg[5] == 0b11111000);
  return 0;
}
/*
 * Test reception of status report
 */
int test_rx_status_report(const srslte::pdcp_lte_state_t& init_state, srslog::basic_logger& logger)
{
  srslte::pdcp_config_t cfg = {1,
                               srslte::PDCP_RB_IS_DRB,
                               srslte::SECURITY_DIRECTION_UPLINK,
                               srslte::SECURITY_DIRECTION_DOWNLINK,
                               srslte::PDCP_SN_LEN_12,
                               srslte::pdcp_t_reordering_t::ms500,
                               srslte::pdcp_discard_timer_t::ms500,
                               true};

  pdcp_lte_test_helper     pdcp_hlp(cfg, sec_cfg, logger);
  srslte::pdcp_entity_lte* pdcp  = &pdcp_hlp.pdcp;
  rlc_dummy*               rlc   = &pdcp_hlp.rlc;
  srsue::stack_test_dummy* stack = &pdcp_hlp.stack;

  pdcp_hlp.set_pdcp_initial_state(init_state);
  srslte::unique_byte_buffer_t status_pdu = srslte::make_byte_buffer();
  srslte::unique_byte_buffer_t out_pdu    = srslte::make_byte_buffer();

  // Write 256 SDUs and notify imediatly -> FMS 0001 0000 0001
  for (uint32_t i = 0; i < 257; i++) {
    srslte::unique_byte_buffer_t sdu = srslte::make_byte_buffer();
    sdu->append_bytes(sdu1, sizeof(sdu1));
    pdcp->write_sdu(std::move(sdu));
    pdcp->notify_delivery({i});
  }

  // Write another 16 SDUs but don't notify SN=257, SN=258, SN=271 and SN=272
  for (uint32_t i = 257; i < 273; i++) {
    srslte::unique_byte_buffer_t sdu = srslte::make_byte_buffer();
    sdu->append_bytes(sdu1, sizeof(sdu1));
    pdcp->write_sdu(std::move(sdu));
    if (i != 257 && i != 258 && i != 271 && i != 272) {
      pdcp->notify_delivery({i});
    }
  }

  // Check undelivered SDUs queue size
  TESTASSERT(pdcp->nof_discard_timers() == 4);

  // Set status PDU
  // Notify up to 270 wit FMS
  // Notify 272 with bitmap
  /*
   *  | D/C | TYPE | FMS | -> | 0 | 0 | 0001 |
   *  |       FMS        | -> | 00001110     |
   *  |     bitmap       | -> | 01000000     |
   */
  status_pdu->N_bytes = 3;
  status_pdu->msg[0]  = 0b00000001;
  status_pdu->msg[1]  = 0b00001110;
  status_pdu->msg[2]  = 0b01000000;

  pdcp->write_pdu(std::move(status_pdu));

  TESTASSERT(pdcp->nof_discard_timers() == 1);
  return 0;
}

// Setup all tests
int run_all_tests()
{
  // Setup log
  srslog::basic_logger& logger = srslog::fetch_basic_logger("PDCP", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(128);

  // This is the normal initial state. All state variables are set to zero
  srslte::pdcp_lte_state_t normal_init_state = {
      .next_pdcp_tx_sn = 0, .tx_hfn = 0, .rx_hfn = 0, .next_pdcp_rx_sn = 0, .last_submitted_pdcp_rx_sn = 4095};

  TESTASSERT(test_tx_status_report(normal_init_state, logger) == 0);
  TESTASSERT(test_tx_wraparound_status_report(normal_init_state, logger) == 0);
  TESTASSERT(test_rx_status_report(normal_init_state, logger) == 0);
  return 0;
}

int main()
{
  srslog::init();

  if (run_all_tests() != SRSLTE_SUCCESS) {
    fprintf(stderr, "pdcp_lte_tests() failed\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}
