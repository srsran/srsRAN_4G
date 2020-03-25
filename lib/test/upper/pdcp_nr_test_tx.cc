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
 * Genric function to test transmission of in-sequence packets
 */
int test_tx(uint32_t                     n_packets,
            const pdcp_initial_state&    init_state,
            uint8_t                      pdcp_sn_len,
            uint64_t                     n_pdus_exp,
            srslte::unique_byte_buffer_t pdu_exp,
            srslte::byte_buffer_pool*    pool,
            srslte::log_ref              log)
{
  srslte::pdcp_config_t cfg = {1,
                               srslte::PDCP_RB_IS_DRB,
                               srslte::SECURITY_DIRECTION_UPLINK,
                               srslte::SECURITY_DIRECTION_DOWNLINK,
                               pdcp_sn_len,
                               srslte::pdcp_t_reordering_t::ms500,
                               srslte::pdcp_discard_timer_t::infinity};

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
 * TX Test: PDCP Entity with SN LEN = 12 and 18.
 * PDCP entity configured with EIA2 and EEA2
 */
int test_tx_all(srslte::byte_buffer_pool* pool, srslte::log_ref log)
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

// Setup all tests
int run_all_tests(srslte::byte_buffer_pool* pool)
{
  // Setup log
  srslte::log_ref log("PDCP NR Test TX");
  log->set_level(srslte::LOG_LEVEL_DEBUG);
  log->set_hex_limit(128);

  TESTASSERT(test_tx_all(pool, log) == 0);
  return 0;
}

int main()
{
  if (run_all_tests(srslte::byte_buffer_pool::get_instance()) != SRSLTE_SUCCESS) {
    fprintf(stderr, "pdcp_nr_tests_tx() failed\n");
    return SRSLTE_ERROR;
  }
  srslte::byte_buffer_pool::cleanup();

  return SRSLTE_SUCCESS;
}
