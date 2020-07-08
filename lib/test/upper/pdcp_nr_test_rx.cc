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
#include "pdcp_nr_test.h"
#include <numeric>

/*
 * Genric function to test reception of in-sequence packets
 */
int test_rx(std::vector<pdcp_test_event_t>      events,
            const pdcp_initial_state&           init_state,
            uint8_t                             pdcp_sn_len,
            uint32_t                            n_sdus_exp,
            const srslte::unique_byte_buffer_t& sdu_exp,
            srslte::byte_buffer_pool*           pool,
            srslte::log_ref                     log)

{

  srslte::pdcp_config_t cfg_rx = {1,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  pdcp_sn_len,
                                  srslte::pdcp_t_reordering_t::ms500,
                                  srslte::pdcp_discard_timer_t::infinity};

  pdcp_nr_test_helper      pdcp_hlp_rx(cfg_rx, sec_cfg, log);
  srslte::pdcp_entity_nr*  pdcp_rx = &pdcp_hlp_rx.pdcp;
  gw_dummy*                gw_rx   = &pdcp_hlp_rx.gw;
  srsue::stack_test_dummy* stack   = &pdcp_hlp_rx.stack;
  pdcp_hlp_rx.set_pdcp_initial_state(init_state);

  // Generate test message and encript/decript SDU.
  for (pdcp_test_event_t& event : events) {

    // Decript and integrity check the PDU
    pdcp_rx->write_pdu(std::move(event.pkt));
    for (uint32_t i = 0; i < event.ticks; ++i) {
      stack->run_tti();
    }
  }

  // Test if the number of RX packets
  TESTASSERT(gw_rx->rx_count == n_sdus_exp);
  srslte::unique_byte_buffer_t sdu_act = allocate_unique_buffer(*pool);
  gw_rx->get_last_pdu(sdu_act);
  TESTASSERT(compare_two_packets(sdu_exp, sdu_act) == 0);
  return 0;
}

/*
 * RX Test: PDCP Entity with SN LEN = 12 and 18.
 * PDCP entity configured with EIA2 and EEA2
 */
int test_rx_all(srslte::byte_buffer_pool* pool, srslte::log_ref log)
{
  // Test SDUs
  srslte::unique_byte_buffer_t tst_sdu1 = allocate_unique_buffer(*pool); // SDU 1
  tst_sdu1->append_bytes(sdu1, sizeof(sdu1));
  srslte::unique_byte_buffer_t tst_sdu2 = allocate_unique_buffer(*pool); // SDU 2
  tst_sdu2->append_bytes(sdu2, sizeof(sdu2));

  /*
   * RX Test 1: PDCP Entity with SN LEN = 12
   * Test in-sequence reception of 4097 packets.
   * This tests correct handling of HFN in the case of SN wraparound (SN LEN 12)
   */
  {
    std::vector<uint32_t> test1_counts(2);                     // Test two packets
    std::iota(test1_counts.begin(), test1_counts.end(), 4095); // Starting at COUNT 4095
    std::vector<pdcp_test_event_t> test1_pdus =
        gen_expected_pdus_vector(tst_sdu1, test1_counts, srslte::PDCP_SN_LEN_12, sec_cfg, pool, log);
    pdcp_initial_state test1_init_state = {.tx_next = 4095, .rx_next = 4095, .rx_deliv = 4095, .rx_reord = 0};
    TESTASSERT(test_rx(std::move(test1_pdus), test1_init_state, srslte::PDCP_SN_LEN_12, 2, tst_sdu1, pool, log) == 0);
  }
  /*
   * RX Test 2: PDCP Entity with SN LEN = 12
   * Test in-sequence reception of 4294967297 packets.
   * This tests correct handling of COUNT in the case of [HFN|SN] wraparound
   * Packet that wraparound should be dropped, so only one packet should be received at the GW.
   */
  {
    std::vector<uint32_t> test2_counts(2);                           // Test two packets
    std::iota(test2_counts.begin(), test2_counts.end(), 4294967295); // Starting at COUNT 4294967295
    std::vector<pdcp_test_event_t> test2_pdus =
        gen_expected_pdus_vector(tst_sdu1, test2_counts, srslte::PDCP_SN_LEN_12, sec_cfg, pool, log);
    pdcp_initial_state test2_init_state = {
        .tx_next = 4294967295, .rx_next = 4294967295, .rx_deliv = 4294967295, .rx_reord = 0};
    TESTASSERT(test_rx(std::move(test2_pdus), test2_init_state, srslte::PDCP_SN_LEN_12, 1, tst_sdu1, pool, log) == 0);
  }
  /*
   * RX Test 3: PDCP Entity with SN LEN = 18
   * Test In-sequence reception of 262145 packets.
   * This tests correct handling of HFN in the case of SN wraparound (SN LEN 18)
   */
  {
    std::vector<uint32_t> test3_counts(2);                       // Test two packets
    std::iota(test3_counts.begin(), test3_counts.end(), 262144); // Starting at COUNT 262144
    std::vector<pdcp_test_event_t> test3_pdus =
        gen_expected_pdus_vector(tst_sdu1, test3_counts, srslte::PDCP_SN_LEN_18, sec_cfg, pool, log);
    pdcp_initial_state test3_init_state = {.tx_next = 262144, .rx_next = 262144, .rx_deliv = 262144, .rx_reord = 0};
    TESTASSERT(test_rx(std::move(test3_pdus), test3_init_state, srslte::PDCP_SN_LEN_18, 2, tst_sdu1, pool, log) == 0);
  }

  /*
   * RX Test 4: PDCP Entity with SN LEN = 18
   * Test in-sequence reception of 4294967297 packets.
   * This tests correct handling of COUNT in the case of [HFN|SN] wraparound
   */
  {
    std::vector<uint32_t> test4_counts(2);                           // Test two packets
    std::iota(test4_counts.begin(), test4_counts.end(), 4294967295); // Starting at COUNT 4294967295
    std::vector<pdcp_test_event_t> test4_pdus =
        gen_expected_pdus_vector(tst_sdu1, test4_counts, srslte::PDCP_SN_LEN_18, sec_cfg, pool, log);
    pdcp_initial_state test4_init_state = {
        .tx_next = 4294967295, .rx_next = 4294967295, .rx_deliv = 4294967295, .rx_reord = 0};
    TESTASSERT(test_rx(std::move(test4_pdus), test4_init_state, srslte::PDCP_SN_LEN_18, 1, tst_sdu1, pool, log) == 0);
  }

  /*
   * RX Test 5: PDCP Entity with SN LEN = 12
   * Test reception of two out-of-order packets, starting at COUNT 0.
   */
  {
    std::vector<pdcp_test_event_t> test5_pdus;
    pdcp_initial_state             test5_init_state = {};

    // First PDU
    pdcp_test_event_t event_pdu1;
    event_pdu1.pkt = srslte::allocate_unique_buffer(*pool);
    event_pdu1.pkt->append_bytes(pdu1_count0_snlen12, sizeof(pdu1_count0_snlen12));

    // Second PDU
    pdcp_test_event_t event_pdu2;
    event_pdu2.pkt = srslte::allocate_unique_buffer(*pool);
    event_pdu2.pkt->append_bytes(pdu2_count1_snlen12, sizeof(pdu2_count1_snlen12));

    // Write PDUs out of order (first the pdu with COUNT 1 and COUNT 0 after)
    test5_pdus.push_back(std::move(event_pdu2));
    test5_pdus.push_back(std::move(event_pdu1));
    TESTASSERT(test_rx(std::move(test5_pdus), test5_init_state, srslte::PDCP_SN_LEN_12, 2, tst_sdu2, pool, log) == 0);
  }

  /*
   * RX Test 6: PDCP Entity with SN LEN = 18
   * Test reception of two out-of-order packets, starting at COUNT 0.
   */
  {
    std::vector<pdcp_test_event_t> test6_pdus;
    pdcp_initial_state             test6_init_state = {};

    // First PDU
    pdcp_test_event_t event_pdu1;
    event_pdu1.pkt = srslte::allocate_unique_buffer(*pool);
    event_pdu1.pkt->append_bytes(pdu1_count0_snlen18, sizeof(pdu1_count0_snlen18));

    // Second PDU
    pdcp_test_event_t event_pdu2;
    event_pdu2.pkt = srslte::allocate_unique_buffer(*pool);
    event_pdu2.pkt->append_bytes(pdu2_count1_snlen18, sizeof(pdu2_count1_snlen18));

    // Write PDUs out of order (first the pdu with COUNT 1 and COUNT 0 after)
    test6_pdus.push_back(std::move(event_pdu2));
    test6_pdus.push_back(std::move(event_pdu1));
    TESTASSERT(test_rx(std::move(test6_pdus), test6_init_state, srslte::PDCP_SN_LEN_18, 2, tst_sdu2, pool, log) == 0);
  }

  /*
   * RX Test 7: PDCP Entity with SN LEN = 12
   * Test Reception of one out-of-order packet.
   */
  {
    std::vector<pdcp_test_event_t> test7_pdus;
    pdcp_initial_state             test7_init_state = {};

    // First PDU
    pdcp_test_event_t event_pdu1;
    event_pdu1.pkt = srslte::allocate_unique_buffer(*pool);
    event_pdu1.pkt->append_bytes(pdu2_count1_snlen12, sizeof(pdu2_count1_snlen12));
    event_pdu1.ticks = 500;

    // Write PDUs out of order (first the pdu with COUNT 1 and COUNT 0 after)
    test7_pdus.push_back(std::move(event_pdu1));
    TESTASSERT(test_rx(std::move(test7_pdus), test7_init_state, srslte::PDCP_SN_LEN_12, 1, tst_sdu2, pool, log) == 0);
  }

  /*
   * RX Test 8: PDCP Entity with SN LEN = 12
   * Test reception of two duplicate PDUs, with COUNT 0.
   */
  {
    std::vector<pdcp_test_event_t> test8_pdus;
    pdcp_initial_state             test8_init_state = {};

    // First PDU
    pdcp_test_event_t event_pdu1;
    event_pdu1.pkt = srslte::allocate_unique_buffer(*pool);
    event_pdu1.pkt->append_bytes(pdu1_count0_snlen12, sizeof(pdu1_count0_snlen12));

    // Second PDU
    pdcp_test_event_t event_pdu2;
    event_pdu2.pkt = srslte::allocate_unique_buffer(*pool);
    event_pdu2.pkt->append_bytes(pdu1_count0_snlen12, sizeof(pdu1_count0_snlen12));

    // Write PDUs out of order (first the pdu with COUNT 1 and COUNT 0 after)
    test8_pdus.push_back(std::move(event_pdu1));
    test8_pdus.push_back(std::move(event_pdu2));
    TESTASSERT(test_rx(std::move(test8_pdus), test8_init_state, srslte::PDCP_SN_LEN_12, 1, tst_sdu1, pool, log) == 0);
  }
  return 0;
}

// Setup all tests
int run_all_tests(srslte::byte_buffer_pool* pool)
{
  // Setup log
  srslte::log_ref log("PDCP NR Test RX");
  log->set_level(srslte::LOG_LEVEL_DEBUG);
  log->set_hex_limit(128);

  TESTASSERT(test_rx_all(pool, log) == 0);
  return 0;
}

int main()
{
  if (run_all_tests(srslte::byte_buffer_pool::get_instance()) != SRSLTE_SUCCESS) {
    fprintf(stderr, "pdcp_nr_tests_rx() failed\n");
    return SRSLTE_ERROR;
  }
  srslte::byte_buffer_pool::cleanup();

  return SRSLTE_SUCCESS;
}
