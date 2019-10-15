/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

// Encryption and Integrity Keys
uint8_t k_int[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                   0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};
uint8_t k_enc[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                   0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};

// Security Configuration, common to all tests.
pdcp_security_cfg sec_cfg = {
    k_int,
    k_enc,
    k_int,
    k_enc,
    srslte::INTEGRITY_ALGORITHM_ID_128_EIA2,
    srslte::CIPHERING_ALGORITHM_ID_128_EEA2,
};

// Test SDUs for tx
uint8_t sdu1[] = {0x18, 0xe2};
uint8_t sdu2[] = {0xde, 0xad};

// Test PDUs for rx (generated from SDU1)
uint8_t pdu1_count0_snlen12[]          = {0x80, 0x00, 0x8f, 0xe3, 0xe0, 0xdf, 0x82, 0x92};
uint8_t pdu1_count2048_snlen12[]       = {0x88, 0x00, 0x8d, 0x2c, 0x47, 0x5e, 0xb1, 0x5b};
uint8_t pdu1_count4096_snlen12[]       = {0x80, 0x00, 0x97, 0xbe, 0xa3, 0x32, 0xfa, 0x61};
uint8_t pdu1_count4294967295_snlen12[] = {0x8f, 0xff, 0x1e, 0x47, 0xe6, 0x86, 0x28, 0x6c};
uint8_t pdu1_count0_snlen18[]          = {0x80, 0x00, 0x00, 0x8f, 0xe3, 0xe0, 0xdf, 0x82, 0x92};
uint8_t pdu1_count131072_snlen18[]     = {0x82, 0x00, 0x00, 0x15, 0x01, 0xf4, 0xb0, 0xfc, 0xc5};
uint8_t pdu1_count262144_snlen18[]     = {0x80, 0x00, 0x00, 0xc2, 0x47, 0xa8, 0xdd, 0xc0, 0x73};
uint8_t pdu1_count4294967295_snlen18[] = {0x83, 0xff, 0xff, 0x1e, 0x47, 0xe6, 0x86, 0x28, 0x6c};

// Test PDUs for rx (generated from SDU2)
uint8_t pdu2_count1_snlen12[] = {0x80, 0x01, 0x5e, 0x3d, 0x64, 0xaf, 0xac, 0x7c};
uint8_t pdu2_count1_snlen18[] = {0x80, 0x00, 0x01, 0x5e, 0x3d, 0x64, 0xaf, 0xac, 0x7c};

// This is the normal initial state. All state variables are set to zero
pdcp_initial_state normal_init_state = {};

// Some tests regarding COUNT wraparound take really long.
// This puts the PCDC state closer to wraparound quickly.
pdcp_initial_state near_wraparound_init_state = {
    .tx_next = 4294967295, .rx_next = 4294967295, .rx_deliv = 4294967295, .rx_reord = 0};

/*
 * Genric function to test transmission of in-sequence packets
 */
int test_tx(uint32_t                     n_packets,
            const pdcp_initial_state&    init_state,
            uint8_t                      pdcp_sn_len,
            uint64_t                     n_pdus_exp,
            srslte::unique_byte_buffer_t pdu_exp,
            srslte::byte_buffer_pool*    pool,
            srslte::log*                 log)
{
  srslte::pdcp_config_t cfg = {1,
                               srslte::PDCP_RB_IS_DRB,
                               srslte::SECURITY_DIRECTION_UPLINK,
                               srslte::SECURITY_DIRECTION_DOWNLINK,
                               pdcp_sn_len,
                               srslte::pdcp_t_reordering_t::ms500};

  pdcp_nr_test_helper     pdcp_hlp(cfg, sec_cfg, log);
  srslte::pdcp_entity_nr* pdcp = &pdcp_hlp.pdcp;
  rlc_dummy*              rlc  = &pdcp_hlp.rlc;

  pdcp_hlp.set_pdcp_initial_state(init_state);

  // Run test
  for (uint32_t i = 0; i < n_packets; ++i) {
    // Test SDU
    srslte::unique_byte_buffer_t sdu = allocate_unique_buffer(*pool);
    sdu->append_bytes(sdu1, sizeof(sdu1));
    pdcp->write_sdu(std::move(sdu), true);
  }

  srslte::unique_byte_buffer_t pdu_act = allocate_unique_buffer(*pool);
  rlc->get_last_sdu(pdu_act);

  TESTASSERT(rlc->rx_count == n_pdus_exp);
  TESTASSERT(compare_two_packets(pdu_act, pdu_exp) == 0);
  return 0;
}

/*
 * Genric function to test reception of in-sequence packets
 */
int test_rx(std::vector<pdcp_test_event_t>      events,
            const pdcp_initial_state&           init_state,
            uint8_t                             pdcp_sn_len,
            uint32_t                            n_sdus_exp,
            const srslte::unique_byte_buffer_t& sdu_exp,
            srslte::byte_buffer_pool*           pool,
            srslte::log*                        log)

{

  srslte::pdcp_config_t cfg_rx = {1,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  pdcp_sn_len,
                                  srslte::pdcp_t_reordering_t::ms500};

  pdcp_nr_test_helper     pdcp_hlp_rx(cfg_rx, sec_cfg, log);
  srslte::pdcp_entity_nr* pdcp_rx   = &pdcp_hlp_rx.pdcp;
  gw_dummy*               gw_rx     = &pdcp_hlp_rx.gw;
  srslte::timers*         timers_rx = &pdcp_hlp_rx.timers;
  pdcp_hlp_rx.set_pdcp_initial_state(init_state);

  // Generate test message and encript/decript SDU.
  for (pdcp_test_event_t& event : events) {

    // Decript and integrity check the PDU
    pdcp_rx->write_pdu(std::move(event.pkt));
    for (uint32_t i = 0; i < event.ticks; ++i) {
      timers_rx->step_all();
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
 * TX Test: PDCP Entity with SN LEN = 12 and 18.
 * PDCP entity configured with EIA2 and EEA2
 */
int test_tx_all(srslte::byte_buffer_pool* pool, srslte::log* log)
{
  uint64_t n_packets;
  /*
   * TX Test 1: PDCP Entity with SN LEN = 12
   * TX_NEXT = 0.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x80, 0x00}, Ciphered Text {0x8f, 0xe3}, MAC-I {0xe0, 0xdf, 0x82, 0x92}
   */
  n_packets                                         = 1;
  srslte::unique_byte_buffer_t pdu_exp_count0_len12 = allocate_unique_buffer(*pool);
  pdu_exp_count0_len12->append_bytes(pdu1_count0_snlen12, sizeof(pdu1_count0_snlen12));
  TESTASSERT(test_tx(n_packets,
                     normal_init_state,
                     srslte::PDCP_SN_LEN_12,
                     n_packets,
                     std::move(pdu_exp_count0_len12),
                     pool,
                     log) == 0);

  /*
   * TX Test 2: PDCP Entity with SN LEN = 12
   * TX_NEXT = 2048.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x88, 0x00}, Ciphered Text {0x8d, 0x2c}, MAC-I {0x47, 0x5e, 0xb1, 0x5b}
   */
  n_packets                                            = 2049;
  srslte::unique_byte_buffer_t pdu_exp_count2048_len12 = allocate_unique_buffer(*pool);
  pdu_exp_count2048_len12->append_bytes(pdu1_count2048_snlen12, sizeof(pdu1_count2048_snlen12));
  TESTASSERT(test_tx(n_packets,
                     normal_init_state,
                     srslte::PDCP_SN_LEN_12,
                     n_packets,
                     std::move(pdu_exp_count2048_len12),
                     pool,
                     log) == 0);

  /*
   * TX Test 3: PDCP Entity with SN LEN = 12
   * TX_NEXT = 4096.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x80,0x00}, Ciphered Text {0x97, 0xbe}, MAC-I {0xa3, 0x32, 0xfa, 0x61}
   */
  n_packets                                            = 4097;
  srslte::unique_byte_buffer_t pdu_exp_count4096_len12 = allocate_unique_buffer(*pool);
  pdu_exp_count4096_len12->append_bytes(pdu1_count4096_snlen12, sizeof(pdu1_count4096_snlen12));
  TESTASSERT(test_tx(n_packets,
                     normal_init_state,
                     srslte::PDCP_SN_LEN_12,
                     n_packets,
                     std::move(pdu_exp_count4096_len12),
                     pool,
                     log) == 0);

  /*
   * TX Test 4: PDCP Entity with SN LEN = 18
   * TX_NEXT = 0.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x80, 0x80, 0x00}, Ciphered Text {0x8f, 0xe3}, MAC-I {0xe0, 0xdf, 0x82, 0x92}
   */
  n_packets                                         = 1;
  srslte::unique_byte_buffer_t pdu_exp_count0_len18 = allocate_unique_buffer(*pool);
  pdu_exp_count0_len18->append_bytes(pdu1_count0_snlen18, sizeof(pdu1_count0_snlen18));
  TESTASSERT(test_tx(n_packets,
                     normal_init_state,
                     srslte::PDCP_SN_LEN_18,
                     n_packets,
                     std::move(pdu_exp_count0_len18),
                     pool,
                     log) == 0);

  /*
   * TX Test 5: PDCP Entity with SN LEN = 18
   * TX_NEXT = 131072.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x82, 0x00, 0x00}, Ciphered Text {0x15, 0x01}, MAC-I {0xf4, 0xb0, 0xfc, 0xc5}
   */
  n_packets                                           = 131073;
  srslte::unique_byte_buffer_t pdu_exp_sn131072_len18 = allocate_unique_buffer(*pool);
  pdu_exp_sn131072_len18->append_bytes(pdu1_count131072_snlen18, sizeof(pdu1_count131072_snlen18));
  TESTASSERT(test_tx(n_packets,
                     normal_init_state,
                     srslte::PDCP_SN_LEN_18,
                     n_packets,
                     std::move(pdu_exp_sn131072_len18),
                     pool,
                     log) == 0);

  /*
   * TX Test 6: PDCP Entity with SN LEN = 18
   * TX_NEXT = 262144.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x80, 0x00, 0x00}, Ciphered Text {0xc2, 0x47}, MAC-I {0xa8, 0xdd, 0xc0, 0x73}
   */
  n_packets                                              = 262145;
  srslte::unique_byte_buffer_t pdu_exp_count262144_len18 = allocate_unique_buffer(*pool);
  pdu_exp_count262144_len18->append_bytes(pdu1_count262144_snlen18, sizeof(pdu1_count262144_snlen18));
  TESTASSERT(test_tx(n_packets,
                     normal_init_state,
                     srslte::PDCP_SN_LEN_18,
                     n_packets,
                     std::move(pdu_exp_count262144_len18),
                     pool,
                     log) == 0);

  /*
   * TX Test 7: PDCP Entity with SN LEN = 12
   * Test TX at COUNT wraparound.
   * Should print a warning and drop all packets after wraparound.
   */
  n_packets                                                  = 5;
  srslte::unique_byte_buffer_t pdu_exp_count4294967295_len12 = allocate_unique_buffer(*pool);
  pdu_exp_count4294967295_len12->append_bytes(pdu1_count4294967295_snlen12, sizeof(pdu1_count4294967295_snlen12));
  TESTASSERT(test_tx(n_packets,
                     near_wraparound_init_state,
                     srslte::PDCP_SN_LEN_12,
                     1,
                     std::move(pdu_exp_count4294967295_len12),
                     pool,
                     log) == 0);

  /*
   * TX Test 8: PDCP Entity with SN LEN = 18
   * Test TX at COUNT wraparound.
   * Should print a warning and drop all packets after wraparound.
   */
  n_packets                                                  = 5;
  srslte::unique_byte_buffer_t pdu_exp_count4294967295_len18 = allocate_unique_buffer(*pool);
  pdu_exp_count4294967295_len18->append_bytes(pdu1_count4294967295_snlen18, sizeof(pdu1_count4294967295_snlen18));
  TESTASSERT(test_tx(n_packets,
                     near_wraparound_init_state,
                     srslte::PDCP_SN_LEN_18,
                     1,
                     std::move(pdu_exp_count4294967295_len18),
                     pool,
                     log) == 0);
  return 0;
}

/*
 * RX Test: PDCP Entity with SN LEN = 12 and 18.
 * PDCP entity configured with EIA2 and EEA2
 */
int test_rx_all(srslte::byte_buffer_pool* pool, srslte::log* log)
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
  srslte::log_filter log("PDCP NR Test");
  log.set_level(srslte::LOG_LEVEL_DEBUG);
  log.set_hex_limit(128);

  TESTASSERT(test_tx_all(pool, &log) == 0);
  TESTASSERT(test_rx_all(pool, &log) == 0);

  // Helpers for generating expected PDUs
  // srslte::unique_byte_buffer_t sdu = srslte::allocate_unique_buffer(*pool);
  // sdu->append_bytes(sdu2, sizeof(sdu2));
  // uint32_t tx_next = 1;
  // srslte::unique_byte_buffer_t pdu = gen_expected_pdu(std::move(sdu), tx_next, srslte::PDCP_SN_LEN_18, sec_cfg, pool,
  // &log); print_packet_array(pdu);
  return 0;
}

int main(int argc, char** argv)
{
  run_all_tests(srslte::byte_buffer_pool::get_instance());
  srslte::byte_buffer_pool::cleanup();
}
