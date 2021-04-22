/**
 * Copyright 2013-2021 Software Radio Systems Limited
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
#include "pdcp_lte_test.h"
#include <numeric>

/*
 * Test correct transmission of FMS and Bitmap in status report
 */
int test_tx_status_report(const srsran::pdcp_lte_state_t& init_state, srslog::basic_logger& logger)
{
  srsran::pdcp_config_t cfg_tx = {1,
                                  srsran::PDCP_RB_IS_DRB,
                                  srsran::SECURITY_DIRECTION_UPLINK,
                                  srsran::SECURITY_DIRECTION_DOWNLINK,
                                  srsran::PDCP_SN_LEN_12,
                                  srsran::pdcp_t_reordering_t::ms500,
                                  srsran::pdcp_discard_timer_t::ms500,
                                  true,
                                  srsran::srsran_rat_t::lte};

  srsran::pdcp_config_t cfg_rx = {1,
                                  srsran::PDCP_RB_IS_DRB,
                                  srsran::SECURITY_DIRECTION_DOWNLINK,
                                  srsran::SECURITY_DIRECTION_UPLINK,
                                  srsran::PDCP_SN_LEN_12,
                                  srsran::pdcp_t_reordering_t::ms500,
                                  srsran::pdcp_discard_timer_t::ms500,
                                  true,
                                  srsran::srsran_rat_t::lte};

  // Setup TX
  pdcp_lte_test_helper     pdcp_hlp_tx(cfg_tx, sec_cfg, logger);
  srsran::pdcp_entity_lte* pdcp_tx  = &pdcp_hlp_tx.pdcp;
  rlc_dummy*               rlc_tx   = &pdcp_hlp_tx.rlc;
  srsue::stack_test_dummy* stack_tx = &pdcp_hlp_tx.stack;
  pdcp_hlp_tx.set_pdcp_initial_state(init_state);

  // Setup RX
  pdcp_lte_test_helper     pdcp_hlp_rx(cfg_tx, sec_cfg, logger);
  srsran::pdcp_entity_lte* pdcp_rx  = &pdcp_hlp_tx.pdcp;
  rlc_dummy*               rlc_rx   = &pdcp_hlp_tx.rlc;
  srsue::stack_test_dummy* stack_rx = &pdcp_hlp_tx.stack;
  pdcp_hlp_rx.set_pdcp_initial_state(init_state);

  // Tmp variable to hold the PDCP PDU
  srsran::unique_byte_buffer_t out_pdu = srsran::make_byte_buffer();

  // Write 256 SDUs and notify imediatly -> FMS 0001 0000 0001
  for (uint32_t i = 0; i < 257; i++) {
    srsran::unique_byte_buffer_t sdu = srsran::make_byte_buffer();
    srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
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
    srsran::unique_byte_buffer_t sdu = srsran::make_byte_buffer();
    srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
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
int test_tx_wraparound_status_report(const srsran::pdcp_lte_state_t& init_state, srslog::basic_logger& logger)
{
  srsran::pdcp_config_t cfg_tx = {1,
                                  srsran::PDCP_RB_IS_DRB,
                                  srsran::SECURITY_DIRECTION_UPLINK,
                                  srsran::SECURITY_DIRECTION_DOWNLINK,
                                  srsran::PDCP_SN_LEN_12,
                                  srsran::pdcp_t_reordering_t::ms500,
                                  srsran::pdcp_discard_timer_t::ms500,
                                  true,
                                  srsran::srsran_rat_t::lte};

  srsran::pdcp_config_t cfg_rx = {1,
                                  srsran::PDCP_RB_IS_DRB,
                                  srsran::SECURITY_DIRECTION_DOWNLINK,
                                  srsran::SECURITY_DIRECTION_UPLINK,
                                  srsran::PDCP_SN_LEN_12,
                                  srsran::pdcp_t_reordering_t::ms500,
                                  srsran::pdcp_discard_timer_t::ms500,
                                  true,
                                  srsran::srsran_rat_t::lte};

  // Setup TX
  pdcp_lte_test_helper     pdcp_hlp_tx(cfg_tx, sec_cfg, logger);
  srsran::pdcp_entity_lte* pdcp_tx  = &pdcp_hlp_tx.pdcp;
  rlc_dummy*               rlc_tx   = &pdcp_hlp_tx.rlc;
  srsue::stack_test_dummy* stack_tx = &pdcp_hlp_tx.stack;
  pdcp_hlp_tx.set_pdcp_initial_state(init_state);

  // Setup RX
  pdcp_lte_test_helper     pdcp_hlp_rx(cfg_tx, sec_cfg, logger);
  srsran::pdcp_entity_lte* pdcp_rx  = &pdcp_hlp_tx.pdcp;
  rlc_dummy*               rlc_rx   = &pdcp_hlp_tx.rlc;
  srsue::stack_test_dummy* stack_rx = &pdcp_hlp_tx.stack;
  pdcp_hlp_rx.set_pdcp_initial_state(init_state);

  srsran::unique_byte_buffer_t out_pdu = srsran::make_byte_buffer();

  // Write 256 SDUs and notify imediatly -> FMS 1111 1111 0000
  for (uint32_t i = 0; i < 4080; i++) {
    srsran::unique_byte_buffer_t sdu = srsran::make_byte_buffer();
    srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    if (sdu == nullptr or pdu == nullptr) {
      logger.error("Could not allocate byte buffer");
      return SRSRAN_ERROR;
    }
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
    srsran::unique_byte_buffer_t sdu = srsran::make_byte_buffer();
    srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    if (sdu == nullptr or pdu == nullptr) {
      logger.error("Could not allocate byte buffer");
      return SRSRAN_ERROR;
    }
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
int test_rx_status_report(const srsran::pdcp_lte_state_t& init_state, srslog::basic_logger& logger)
{
  srsran::pdcp_config_t cfg = {1,
                               srsran::PDCP_RB_IS_DRB,
                               srsran::SECURITY_DIRECTION_UPLINK,
                               srsran::SECURITY_DIRECTION_DOWNLINK,
                               srsran::PDCP_SN_LEN_12,
                               srsran::pdcp_t_reordering_t::ms500,
                               srsran::pdcp_discard_timer_t::ms500,
                               true,
                               srsran::srsran_rat_t::lte};

  pdcp_lte_test_helper     pdcp_hlp(cfg, sec_cfg, logger);
  srsran::pdcp_entity_lte* pdcp  = &pdcp_hlp.pdcp;
  rlc_dummy*               rlc   = &pdcp_hlp.rlc;
  srsue::stack_test_dummy* stack = &pdcp_hlp.stack;

  pdcp_hlp.set_pdcp_initial_state(init_state);
  srsran::unique_byte_buffer_t status_pdu = srsran::make_byte_buffer();
  srsran::unique_byte_buffer_t out_pdu    = srsran::make_byte_buffer();

  // Write 256 SDUs and notify imediatly -> FMS 0001 0000 0001
  for (uint32_t i = 0; i < 257; i++) {
    srsran::unique_byte_buffer_t sdu = srsran::make_byte_buffer();
    sdu->append_bytes(sdu1, sizeof(sdu1));
    pdcp->write_sdu(std::move(sdu));
    pdcp->notify_delivery({i});
  }

  // Write another 16 SDUs but don't notify SN=257, SN=258, SN=271 and SN=272
  for (uint32_t i = 257; i < 273; i++) {
    srsran::unique_byte_buffer_t sdu = srsran::make_byte_buffer();
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
  srsran::pdcp_lte_state_t normal_init_state  = {};
  normal_init_state.last_submitted_pdcp_rx_sn = 4095;

  TESTASSERT(test_tx_status_report(normal_init_state, logger) == 0);
  TESTASSERT(test_tx_wraparound_status_report(normal_init_state, logger) == 0);
  TESTASSERT(test_rx_status_report(normal_init_state, logger) == 0);
  return 0;
}

int main()
{
  srslog::init();

  if (run_all_tests() != SRSRAN_SUCCESS) {
    fprintf(stderr, "pdcp_lte_tests() failed\n");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}
