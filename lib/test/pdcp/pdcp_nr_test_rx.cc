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
#include "pdcp_nr_test.h"
#include <numeric>

/*
 * Generic class to test reception of in-sequence packets
 */
class test_rx_helper
{
public:
  pdcp_nr_test_helper      pdcp_hlp_rx;
  srsran::pdcp_entity_nr&  pdcp_rx;
  gw_dummy&                gw_rx;
  srsue::stack_test_dummy& stack;
  srslog::basic_logger&    logger;

  test_rx_helper(uint8_t pdcp_sn_len, srslog::basic_logger& logger) :
    pdcp_hlp_rx({1,
                 srsran::PDCP_RB_IS_DRB,
                 srsran::SECURITY_DIRECTION_DOWNLINK,
                 srsran::SECURITY_DIRECTION_UPLINK,
                 pdcp_sn_len,
                 srsran::pdcp_t_reordering_t::ms500,
                 srsran::pdcp_discard_timer_t::infinity,
                 false,
                 srsran::srsran_rat_t::nr},
                sec_cfg,
                logger),
    pdcp_rx(pdcp_hlp_rx.pdcp),
    gw_rx(pdcp_hlp_rx.gw),
    stack(pdcp_hlp_rx.stack),
    logger(logger)
  {}

  int test_rx(std::vector<pdcp_test_event_t>      events,
              const pdcp_initial_state&           init_state,
              uint32_t                            n_sdus_exp,
              const srsran::unique_byte_buffer_t& sdu_exp)

  {
    pdcp_hlp_rx.set_pdcp_initial_state(init_state);

    // Generate test message and encrypt/decrypt SDU.
    for (pdcp_test_event_t& event : events) {
      // Decrypt and integrity check the PDU
      pdcp_rx.write_pdu(std::move(event.pkt));
      for (uint32_t i = 0; i < event.ticks; ++i) {
        stack.run_tti();
      }
    }

    // Test if the number of RX packets
    TESTASSERT_EQ(gw_rx.rx_count, n_sdus_exp);
    srsran::unique_byte_buffer_t sdu_act = srsran::make_byte_buffer();
    gw_rx.get_last_pdu(sdu_act);
    TESTASSERT(compare_two_packets(sdu_exp, sdu_act) == 0);
    return 0;
  }
};

/*
 * RX Test: PDCP Entity with SN LEN = 12 and 18.
 * PDCP entity configured with EIA2 and EEA2
 */
int test_rx_all(srslog::basic_logger& logger)
{
  // Test SDUs
  srsran::unique_byte_buffer_t tst_sdu1 = srsran::make_byte_buffer(); // SDU 1
  tst_sdu1->append_bytes(sdu1, sizeof(sdu1));
  srsran::unique_byte_buffer_t tst_sdu2 = srsran::make_byte_buffer(); // SDU 2
  tst_sdu2->append_bytes(sdu2, sizeof(sdu2));

  /*
   * RX Test 1: PDCP Entity with SN LEN = 12
   * Test in-sequence reception of 4097 packets.
   * This tests correct handling of HFN in the case of SN wraparound (SN LEN 12)
   */
  {
    auto&                       test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger delimiter("RX COUNT [4095,4096], 12 bit SN");
    test_rx_helper              rx_helper(srsran::PDCP_SN_LEN_12, logger);
    std::vector<uint32_t>       test1_counts(2);               // Test two packets
    std::iota(test1_counts.begin(), test1_counts.end(), 4095); // Starting at COUNT 4095
    std::vector<pdcp_test_event_t> test1_pdus =
        gen_expected_pdus_vector(tst_sdu1, test1_counts, srsran::PDCP_SN_LEN_12, sec_cfg, logger);
    pdcp_initial_state test1_init_state = {.tx_next = 4095, .rx_next = 4095, .rx_deliv = 4095, .rx_reord = 0};
    TESTASSERT(rx_helper.test_rx(std::move(test1_pdus), test1_init_state, 2, tst_sdu1) == 0);
  }
  /*
   * RX Test 2: PDCP Entity with SN LEN = 12
   * Test in-sequence reception of 4294967297 packets.
   * This tests correct handling of COUNT in the case of [HFN|SN] wraparound
   * Packet that wraparound should be dropped, so only one packet should be received at the GW.
   */
  {
    auto&                       test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger delimiter("RX COUNT [4294967295,0], 12 bit SN");
    test_rx_helper              rx_helper(srsran::PDCP_SN_LEN_12, logger);
    std::vector<uint32_t>       test2_counts(2);                     // Test two packets
    std::iota(test2_counts.begin(), test2_counts.end(), 4294967295); // Starting at COUNT 4294967295
    std::vector<pdcp_test_event_t> test2_pdus =
        gen_expected_pdus_vector(tst_sdu1, test2_counts, srsran::PDCP_SN_LEN_12, sec_cfg, logger);
    pdcp_initial_state test2_init_state = {
        .tx_next = 4294967295, .rx_next = 4294967295, .rx_deliv = 4294967295, .rx_reord = 0};
    TESTASSERT(rx_helper.test_rx(std::move(test2_pdus), test2_init_state, 1, tst_sdu1) == 0);
  }
  /*
   * RX Test 3: PDCP Entity with SN LEN = 18
   * Test In-sequence reception of 262145 packets.
   * This tests correct handling of HFN in the case of SN wraparound (SN LEN 18)
   */
  {
    auto&                       test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger delimiter("RX COUNT [262144,262145], 12 bit SN");
    test_rx_helper              rx_helper(srsran::PDCP_SN_LEN_18, logger);
    std::vector<uint32_t>       test3_counts(2);                 // Test two packets
    std::iota(test3_counts.begin(), test3_counts.end(), 262144); // Starting at COUNT 262144
    std::vector<pdcp_test_event_t> test3_pdus =
        gen_expected_pdus_vector(tst_sdu1, test3_counts, srsran::PDCP_SN_LEN_18, sec_cfg, logger);
    pdcp_initial_state test3_init_state = {.tx_next = 262144, .rx_next = 262144, .rx_deliv = 262144, .rx_reord = 0};
    TESTASSERT(rx_helper.test_rx(std::move(test3_pdus), test3_init_state, 2, tst_sdu1) == 0);
  }

  /*
   * RX Test 4: PDCP Entity with SN LEN = 18
   * Test in-sequence reception of 4294967297 packets.
   * This tests correct handling of COUNT in the case of [HFN|SN] wraparound
   */
  {
    auto&                       test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger delimiter("RX COUNT [4294967295,4294967296], 18 bit SN");
    test_rx_helper              rx_helper(srsran::PDCP_SN_LEN_18, logger);
    std::vector<uint32_t>       test4_counts(2);                     // Test two packets
    std::iota(test4_counts.begin(), test4_counts.end(), 4294967295); // Starting at COUNT 4294967295
    std::vector<pdcp_test_event_t> test4_pdus =
        gen_expected_pdus_vector(tst_sdu1, test4_counts, srsran::PDCP_SN_LEN_18, sec_cfg, logger);
    pdcp_initial_state test4_init_state = {
        .tx_next = 4294967295, .rx_next = 4294967295, .rx_deliv = 4294967295, .rx_reord = 0};
    TESTASSERT(rx_helper.test_rx(std::move(test4_pdus), test4_init_state, 1, tst_sdu1) == 0);
  }

  /*
   * RX Test 5: PDCP Entity with SN LEN = 12
   * Test reception of two out-of-order packets, starting at COUNT 0.
   * Both packets are received and delivered before t-Reordering expires.
   */
  {
    auto&                          test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger    delimiter("RX out-of-order COUNT [1,0], 12 bit SN");
    test_rx_helper                 rx_helper(srsran::PDCP_SN_LEN_12, logger);
    std::vector<pdcp_test_event_t> test5_pdus;
    pdcp_initial_state             test5_init_state = {};

    // First PDU
    pdcp_test_event_t event_pdu1;
    event_pdu1.pkt = srsran::make_byte_buffer();
    event_pdu1.pkt->append_bytes(pdu1_count0_snlen12, sizeof(pdu1_count0_snlen12));

    // Second PDU
    pdcp_test_event_t event_pdu2;
    event_pdu2.pkt = srsran::make_byte_buffer();
    event_pdu2.pkt->append_bytes(pdu2_count1_snlen12, sizeof(pdu2_count1_snlen12));

    // Write PDUs out of order (first the pdu with COUNT 1 and COUNT 0 after)
    test5_pdus.push_back(std::move(event_pdu2));
    test5_pdus.push_back(std::move(event_pdu1));
    TESTASSERT(rx_helper.test_rx(std::move(test5_pdus), test5_init_state, 2, tst_sdu2) == 0);
    TESTASSERT(rx_helper.pdcp_rx.is_reordering_timer_running() == false);
    TESTASSERT(rx_helper.pdcp_rx.get_rx_deliv() == 2);
    TESTASSERT(rx_helper.pdcp_rx.get_rx_reord() == 2);
  }

  /*
   * RX Test 6: PDCP Entity with SN LEN = 18
   * Test reception of two out-of-order packets, starting at COUNT 0.
   * Both packets are received and delivered before t-Reordering expires.
   */
  {
    auto&                          test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger    delimiter("RX out-of-order COUNT [1,0], 18 bit SN");
    test_rx_helper                 rx_helper(srsran::PDCP_SN_LEN_18, logger);
    std::vector<pdcp_test_event_t> test6_pdus;
    pdcp_initial_state             test6_init_state = {};

    // First PDU
    pdcp_test_event_t event_pdu1;
    event_pdu1.pkt = srsran::make_byte_buffer();
    event_pdu1.pkt->append_bytes(pdu1_count0_snlen18, sizeof(pdu1_count0_snlen18));

    // Second PDU
    pdcp_test_event_t event_pdu2;
    event_pdu2.pkt = srsran::make_byte_buffer();
    event_pdu2.pkt->append_bytes(pdu2_count1_snlen18, sizeof(pdu2_count1_snlen18));

    // Write PDUs out of order (first the pdu with COUNT 1 and COUNT 0 after)
    test6_pdus.push_back(std::move(event_pdu2));
    test6_pdus.push_back(std::move(event_pdu1));
    TESTASSERT(rx_helper.test_rx(std::move(test6_pdus), test6_init_state, 2, tst_sdu2) == 0);
    TESTASSERT(rx_helper.pdcp_rx.is_reordering_timer_running() == false);
    TESTASSERT(rx_helper.pdcp_rx.get_rx_deliv() == 2);
    TESTASSERT(rx_helper.pdcp_rx.get_rx_reord() == 2);
  }

  /*
   * RX Test 7: PDCP Entity with SN LEN = 12
   * Test Reception of one out-of-order packet.
   */
  {
    auto&                          test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger    delimiter("RX out-of-order COUNT [1,0] t_reordering expired, 12 bit SN");
    test_rx_helper                 rx_helper(srsran::PDCP_SN_LEN_12, logger);
    std::vector<pdcp_test_event_t> test7_pdus;
    pdcp_initial_state             test7_init_state = {};

    // First PDU
    pdcp_test_event_t event_pdu1;
    event_pdu1.pkt = srsran::make_byte_buffer();
    event_pdu1.pkt->append_bytes(pdu2_count1_snlen12, sizeof(pdu2_count1_snlen12));
    event_pdu1.ticks = 500;

    // Write PDUs out of order (first the pdu with COUNT 1 and COUNT 0 after)
    test7_pdus.push_back(std::move(event_pdu1));
    TESTASSERT(rx_helper.test_rx(std::move(test7_pdus), test7_init_state, 1, tst_sdu2) == 0);
    TESTASSERT(rx_helper.pdcp_rx.is_reordering_timer_running() == false);
    TESTASSERT(rx_helper.pdcp_rx.get_rx_deliv() == 2);
    TESTASSERT(rx_helper.pdcp_rx.get_rx_reord() == 2);
  }

  /*
   * RX Test 8: PDCP Entity with SN LEN = 12
   * Test reception of two duplicate PDUs, with COUNT 0.
   */
  {
    srsran::test_delimit_logger    delimiter("RX duplicate COUNTs [0,0], 12 bit SN");
    test_rx_helper                 rx_helper(srsran::PDCP_SN_LEN_12, logger);
    std::vector<pdcp_test_event_t> test8_pdus;
    pdcp_initial_state             test8_init_state = {};

    // First PDU
    pdcp_test_event_t event_pdu1;
    event_pdu1.pkt = srsran::make_byte_buffer();
    event_pdu1.pkt->append_bytes(pdu1_count0_snlen12, sizeof(pdu1_count0_snlen12));

    // Second PDU
    pdcp_test_event_t event_pdu2;
    event_pdu2.pkt = srsran::make_byte_buffer();
    event_pdu2.pkt->append_bytes(pdu1_count0_snlen12, sizeof(pdu1_count0_snlen12));

    // Write PDUs out of order (first the pdu with COUNT 1 and COUNT 0 after)
    test8_pdus.push_back(std::move(event_pdu1));
    test8_pdus.push_back(std::move(event_pdu2));
    TESTASSERT(rx_helper.test_rx(std::move(test8_pdus), test8_init_state, 1, tst_sdu1) == 0);
  }
  return 0;
}

// Setup all tests
int run_all_tests()
{
  // Setup log
  auto& logger = srslog::fetch_basic_logger("PDCP NR Test RX", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(128);

  TESTASSERT(test_rx_all(logger) == 0);
  return 0;
}

int main()
{
  srslog::init();

  if (run_all_tests() != SRSRAN_SUCCESS) {
    fprintf(stderr, "pdcp_nr_tests_rx() failed\n");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}
