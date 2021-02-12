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
  srslte::pdcp_config_t cfg = {1,
                               srslte::PDCP_RB_IS_DRB,
                               srslte::SECURITY_DIRECTION_UPLINK,
                               srslte::SECURITY_DIRECTION_DOWNLINK,
                               srslte::PDCP_SN_LEN_12,
                               srslte::pdcp_t_reordering_t::ms500,
                               srslte::pdcp_discard_timer_t::ms500,
                               false};

  pdcp_lte_test_helper     pdcp_hlp(cfg, sec_cfg, logger);
  srslte::pdcp_entity_lte* pdcp  = &pdcp_hlp.pdcp;
  rlc_dummy*               rlc   = &pdcp_hlp.rlc;
  srsue::stack_test_dummy* stack = &pdcp_hlp.stack;

  pdcp_hlp.set_pdcp_initial_state(init_state);
  srslte::unique_byte_buffer_t out_pdu = srslte::make_byte_buffer();

  // Write 256 SDUs and notify imediatly -> FMS 0001 0000 0001
  for (uint32_t i = 0; i < 257; i++) {
    srslte::unique_byte_buffer_t sdu = srslte::make_byte_buffer();
    sdu->append_bytes(sdu1, sizeof(sdu1));
    pdcp->write_sdu(std::move(sdu));
    pdcp->notify_delivery({i});
  }

  // Check undelivered SDUs queue size
  TESTASSERT(pdcp->nof_discard_timers() == 0); // 0 timers should be running

  // Generate the status report
  pdcp->send_status_report();
  rlc->get_last_sdu(out_pdu);
  logger.debug(out_pdu->msg, out_pdu->N_bytes, "Status PDU:");

  // Check status PDU
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
    sdu->append_bytes(sdu1, sizeof(sdu1));
    pdcp->write_sdu(std::move(sdu));
    if (i != 257 && i != 258 && i != 271 && i != 272) {
      pdcp->notify_delivery({i});
    }
  }

  // Check undelivered SDUs queue size
  TESTASSERT(pdcp->nof_discard_timers() == 4);

  // Generate the status report
  pdcp->send_status_report();
  rlc->get_last_sdu(out_pdu);
  logger.debug(out_pdu->msg, out_pdu->N_bytes, "Status PDU:");

  // Check status PDU
  /*
   *  | D/C | TYPE | FMS | -> | 0 | 0 | 0001 |
   *  |       FMS        | -> | 00000001     |
   *  |     bitmap       | -> | 11000000     |
   *  |  bitmap (cont.)  | -> | 00000011     |
   */
  TESTASSERT(out_pdu->N_bytes == 4);
  TESTASSERT(out_pdu->msg[0] == 0b00000001);
  TESTASSERT(out_pdu->msg[1] == 0b00000001);
  TESTASSERT(out_pdu->msg[2] == 0b11000000);
  TESTASSERT(out_pdu->msg[3] == 0b00000011);
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
                               false};

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

  // Check if the SDUs were correctly discarded with the status report
  pdcp->send_status_report();
  rlc->get_last_sdu(out_pdu);
  logger.debug(out_pdu->msg, out_pdu->N_bytes, "Status PDU:");

  // Check status PDU, only 271 is missing => FMS = 271
  /*
   *  | D/C | TYPE | FMS | -> | 0 | 0 | 0001 |
   *  |       FMS        | -> | 00001111     |
   */
  TESTASSERT(out_pdu->N_bytes == 3);
  TESTASSERT(out_pdu->msg[0] == 0b00000001);
  TESTASSERT(out_pdu->msg[1] == 0b00001111);
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
  srslte::pdcp_lte_state_t normal_init_state = {};

  TESTASSERT(test_tx_status_report(normal_init_state, logger) == 0);

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
