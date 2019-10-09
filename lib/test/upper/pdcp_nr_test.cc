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
uint8_t pdu1_count0_snlen12[] = {0x80, 0x00, 0x8f, 0xe3, 0xe0, 0xdf, 0x82, 0x92};
uint8_t pdu1_count2048_snlen12[] = {0x88, 0x00, 0x8d, 0x2c, 0x47, 0x5e, 0xb1, 0x5b};
uint8_t pdu1_count4096_snlen12[] = {0x80, 0x00, 0x97, 0xbe, 0xa3, 0x32, 0xfa, 0x61};
uint8_t pdu1_count0_snlen18[] = {0x80, 0x00, 0x00, 0x8f, 0xe3, 0xe0, 0xdf, 0x82, 0x92};
uint8_t pdu1_count131072_snlen18[] = {0x82, 0x00, 0x00, 0x15, 0x01, 0xf4, 0xb0, 0xfc, 0xc5};
uint8_t pdu1_count262144_snlen18[] = {0x80, 0x00, 0x00, 0xc2, 0x47, 0xa8, 0xdd, 0xc0, 0x73};

// Test PDUs for rx (generated from SDU2)
uint8_t pdu7[] = {0x80, 0x01, 0x5e, 0x3d, 0x64, 0xaf, 0xac, 0x7c};

// Struct to help initialize pdcp_helper.
struct pdcp_nr_initial_state_cfg {
  uint32_t tx_next  = 0;
  uint32_t rx_next  = 0;
  uint32_t rx_deliv = 0;
  uint32_t rx_reord = 0;
};

/*
 * Genric function to test transmission of in-sequence packets
 */
int test_tx(uint32_t                     n_packets,
            uint8_t                      pdcp_sn_len,
            srslte::unique_byte_buffer_t pdu_exp,
            srslte::byte_buffer_pool*    pool,
            srslte::log*                 log)
{
  srslte::pdcp_config_t  cfg = {1,
                               srslte::PDCP_RB_IS_DRB,
                               srslte::SECURITY_DIRECTION_UPLINK,
                               srslte::SECURITY_DIRECTION_DOWNLINK,
                               pdcp_sn_len,
                               srslte::pdcp_t_reordering_t::ms500};

  pdcp_nr_test_helper     pdcp_hlp(cfg, sec_cfg, log);
  srslte::pdcp_entity_nr* pdcp = &pdcp_hlp.pdcp;
  rlc_dummy*              rlc  = &pdcp_hlp.rlc;


  // Run test
  for (uint32_t i = 0; i < n_packets; ++i) {
    // Test SDU
    srslte::unique_byte_buffer_t sdu = allocate_unique_buffer(*pool);
    sdu->append_bytes(sdu1, sizeof(sdu1));
    pdcp->write_sdu(std::move(sdu), true);
  }

  srslte::unique_byte_buffer_t pdu_act = allocate_unique_buffer(*pool);
  rlc->get_last_sdu(pdu_act);

  TESTASSERT(compare_two_packets(pdu_act, pdu_exp) == 0);
  return 0;
}

/*
 * Genric function to test reception of in-sequence packets
 */
int test_rx_in_sequence(uint64_t tx_next_max, const pdcp_initial_state &init_state, uint8_t pdcp_sn_len, srslte::byte_buffer_pool* pool, srslte::log* log)
{

  srslte::pdcp_config_t  cfg_tx = {1,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  pdcp_sn_len,
                                  srslte::pdcp_t_reordering_t::ms500};

  srslte::pdcp_config_t  cfg_rx = {1,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  pdcp_sn_len,
                                  srslte::pdcp_t_reordering_t::ms500};

  pdcp_nr_test_helper     pdcp_hlp_tx(cfg_tx, sec_cfg, log);
  srslte::pdcp_entity_nr* pdcp_tx = &pdcp_hlp_tx.pdcp;
  rlc_dummy*              rlc_tx  = &pdcp_hlp_tx.rlc;

  pdcp_nr_test_helper     pdcp_hlp_rx(cfg_rx, sec_cfg, log);
  srslte::pdcp_entity_nr* pdcp_rx = &pdcp_hlp_rx.pdcp;
  gw_dummy*               gw_rx   = &pdcp_hlp_rx.gw;

  pdcp_hlp_tx.set_pdcp_initial_state(init_state);
  pdcp_hlp_rx.set_pdcp_initial_state(init_state);

  srslte::unique_byte_buffer_t sdu_act = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sdu_exp = allocate_unique_buffer(*pool);
  sdu_exp->append_bytes(sdu1, sizeof(sdu1));

  // Generate test message and encript/decript SDU. Check match with original SDU
  for (uint64_t i = init_state.tx_next; i <= tx_next_max; ++i) {
    srslte::unique_byte_buffer_t sdu = allocate_unique_buffer(*pool);
    srslte::unique_byte_buffer_t pdu = allocate_unique_buffer(*pool);
    sdu->append_bytes(sdu_exp->msg, sdu_exp->N_bytes);

    // Generate encripted and integrity protected PDU
    pdcp_tx->write_sdu(std::move(sdu), true);
    rlc_tx->get_last_sdu(pdu);
    
    // Decript and integrity check the PDU
    pdcp_rx->write_pdu(std::move(pdu));
    gw_rx->get_last_pdu(sdu_act);

    // Check if resulting SDU matches original SDU
    TESTASSERT(compare_two_packets(sdu_exp,sdu_act) == 0);
  }
  return 0;
}

/*
 * Genric function to test reception of a single out-of-order packet
 */
int test_rx_out_of_order(pdcp_initial_state init_state, uint8_t pdcp_sn_len, srslte::byte_buffer_pool* pool, srslte::log* log)
{

  srslte::pdcp_config_t  cfg_tx = {1,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  pdcp_sn_len,
                                  srslte::pdcp_t_reordering_t::ms500};

  srslte::pdcp_config_t  cfg_rx = {1,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  pdcp_sn_len,
                                  srslte::pdcp_t_reordering_t::ms500};

  pdcp_nr_test_helper     pdcp_hlp_tx(cfg_tx, sec_cfg, log);
  srslte::pdcp_entity_nr* pdcp_tx = &pdcp_hlp_tx.pdcp;
  rlc_dummy*              rlc_tx  = &pdcp_hlp_tx.rlc;

  pdcp_nr_test_helper     pdcp_hlp_rx(cfg_rx, sec_cfg, log);
  srslte::pdcp_entity_nr* pdcp_rx = &pdcp_hlp_rx.pdcp;
  gw_dummy*               gw_rx   = &pdcp_hlp_rx.gw;

  pdcp_hlp_tx.set_pdcp_initial_state(init_state);
  pdcp_hlp_rx.set_pdcp_initial_state(init_state);
 
  // Allocate buffers for later comparison.
  srslte::unique_byte_buffer_t sdu_act = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sdu_exp = allocate_unique_buffer(*pool);

  // Write PDUs into tx entity to get expected PDUs
  srslte::unique_byte_buffer_t tx_sdu_out1 = allocate_unique_buffer(*pool);
  tx_sdu_out1->append_bytes(sdu1, sizeof(sdu1));
  pdcp_tx->write_sdu(std::move(tx_sdu_out1), true);

  srslte::unique_byte_buffer_t tx_pdu_out1 = allocate_unique_buffer(*pool);
  rlc_tx->get_last_sdu(tx_pdu_out1);

  srslte::unique_byte_buffer_t tx_sdu_out2 = allocate_unique_buffer(*pool);
  tx_sdu_out2->append_bytes(sdu2, sizeof(sdu2));
  *sdu_exp = *tx_sdu_out2; // save expected SDU
  pdcp_tx->write_sdu(std::move(tx_sdu_out2), true);

  srslte::unique_byte_buffer_t tx_pdu_out2 = allocate_unique_buffer(*pool);
  rlc_tx->get_last_sdu(tx_pdu_out2);
    
  // Write PDUs out-of-order into rx entity to see if re-ordering is OK.
  pdcp_rx->write_pdu(std::move(tx_pdu_out2));
  pdcp_rx->write_pdu(std::move(tx_pdu_out1));

  // Test actual reception
  TESTASSERT(gw_rx->rx_count == 2);
  gw_rx->get_last_pdu(sdu_act);

  log->info_hex(sdu_act->msg, sdu_act->N_bytes, "SDU act");
  TESTASSERT(compare_two_packets(sdu_act, sdu_exp) == 0);
  return 0;
}

/*
 * RX Test: PDCP Entity with packtes received out of order
 * PDCP entity configured with EIA2 and EEA2
 */
int test_rx_out_of_order_timeout(uint8_t pdcp_sn_len, srslte::byte_buffer_pool* pool, srslte::log* log)
{
  srslte::pdcp_config_t  cfg_rx = {1,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  pdcp_sn_len,
                                  srslte::pdcp_t_reordering_t::ms500};

  pdcp_nr_test_helper     pdcp_rx_hlp(cfg_rx, sec_cfg, log);
  srslte::pdcp_entity_nr* pdcp_rx = &pdcp_rx_hlp.pdcp;
  gw_dummy*               gw_rx   = &pdcp_rx_hlp.gw;
  srslte::timers*         timers  = &pdcp_rx_hlp.timers;

  srslte::unique_byte_buffer_t sdu_act = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sdu_exp = allocate_unique_buffer(*pool);
  sdu_exp->append_bytes(sdu2, sizeof(sdu2));

  // Generate encripted and integrity protected PDUs
  srslte::unique_byte_buffer_t rx_pdu7 = allocate_unique_buffer(*pool);
  rx_pdu7->append_bytes(pdu7, sizeof(pdu7));

  // decript and check matching SDUs (out of order)
  pdcp_rx->write_pdu(std::move(rx_pdu7));

  // Make sure out of order is not received until time out
  TESTASSERT(gw_rx->rx_count == 0);

  // Trigger timer
  for (uint16_t i = 0; i < 500; ++i){
    timers->step_all();
  }

  // Make sure timout delivered PDU to GW
  TESTASSERT(gw_rx->rx_count == 1);
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
  /*
   * TX Test 1: PDCP Entity with SN LEN = 12
   * TX_NEXT = 0.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x80, 0x00}, Ciphered Text {0x8f, 0xe3}, MAC-I {0xe0, 0xdf, 0x82, 0x92}
   */
  srslte::unique_byte_buffer_t pdu_exp_count0_len12 = allocate_unique_buffer(*pool);
  pdu_exp_count0_len12->append_bytes(pdu1_count0_snlen12, sizeof(pdu1_count0_snlen12));
  TESTASSERT(test_tx(1, srslte::PDCP_SN_LEN_12, std::move(pdu_exp_count0_len12), pool, log) == 0);

  /*
   * TX Test 2: PDCP Entity with SN LEN = 12
   * TX_NEXT = 2048.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x88, 0x00}, Ciphered Text {0x8d, 0x2c}, MAC-I {0x47, 0x5e, 0xb1, 0x5b}
   */
  srslte::unique_byte_buffer_t pdu_exp_count2048_len12 = allocate_unique_buffer(*pool);
  pdu_exp_count2048_len12->append_bytes(pdu1_count2048_snlen12, sizeof(pdu1_count2048_snlen12));
  TESTASSERT(test_tx(2049, srslte::PDCP_SN_LEN_12, std::move(pdu_exp_count2048_len12), pool, log) == 0);

  /*
   * TX Test 3: PDCP Entity with SN LEN = 12
   * TX_NEXT = 4096.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x80,0x00}, Ciphered Text {0x97, 0xbe}, MAC-I {0xa3, 0x32, 0xfa, 0x61}
   */
  srslte::unique_byte_buffer_t pdu_exp_count4096_len12 = allocate_unique_buffer(*pool);
  pdu_exp_count4096_len12->append_bytes(pdu1_count4096_snlen12, sizeof(pdu1_count4096_snlen12));
  TESTASSERT(test_tx(4097, srslte::PDCP_SN_LEN_12, std::move(pdu_exp_count4096_len12), pool, log) == 0);

  /*
   * TX Test 4: PDCP Entity with SN LEN = 18
   * TX_NEXT = 0.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x80, 0x80, 0x00}, Ciphered Text {0x8f, 0xe3}, MAC-I {0xe0, 0xdf, 0x82, 0x92}
   */
  srslte::unique_byte_buffer_t pdu_exp_count0_len18 = allocate_unique_buffer(*pool);
  pdu_exp_count0_len18->append_bytes(pdu1_count0_snlen18, sizeof(pdu1_count0_snlen18));
  TESTASSERT(test_tx(1, srslte::PDCP_SN_LEN_18, std::move(pdu_exp_count0_len18), pool, log) == 0);

  /*
   * TX Test 5: PDCP Entity with SN LEN = 18
   * TX_NEXT = 131072.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x82, 0x00, 0x00}, Ciphered Text {0x15, 0x01}, MAC-I {0xf4, 0xb0, 0xfc, 0xc5}
   */
  srslte::unique_byte_buffer_t pdu_exp_sn131072_len18 = allocate_unique_buffer(*pool);
  pdu_exp_sn131072_len18->append_bytes(pdu1_count131072_snlen18, sizeof(pdu1_count131072_snlen18));
  TESTASSERT(test_tx(131073, srslte::PDCP_SN_LEN_18, std::move(pdu_exp_sn131072_len18), pool, log) == 0);

  /*
   * TX Test 6: PDCP Entity with SN LEN = 18
   * TX_NEXT = 262144.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x80, 0x00, 0x00}, Ciphered Text {0xc2, 0x47}, MAC-I {0xa8, 0xdd, 0xc0, 0x73}
   */
  srslte::unique_byte_buffer_t pdu_exp_count262144_len18 = allocate_unique_buffer(*pool);
  pdu_exp_count262144_len18->append_bytes(pdu1_count262144_snlen18, sizeof(pdu1_count262144_snlen18));
  TESTASSERT(test_tx(262145, srslte::PDCP_SN_LEN_18, std::move(pdu_exp_count262144_len18), pool, log) == 0);
  return 0;
}

/*
 * RX Test: PDCP Entity with SN LEN = 12 and 18.
 * PDCP entity configured with EIA2 and EEA2
 */
int test_rx_all(srslte::byte_buffer_pool* pool, srslte::log* log)
{
  // This is the normal initial state. All state variables are set to zero
  pdcp_initial_state normal_init_state = {}; 

  // Some tests regarding COUNT wraparound take really long.
  // This puts the PCDC state closer to wraparound quickly.
  pdcp_initial_state near_wraparound_init_state = {
      .tx_next = 4294967295, .rx_next = 4294967295, .rx_deliv = 4294967295, .rx_reord = 0};

  /*
   * RX Test 1: PDCP Entity with SN LEN = 12
   * Test in-sequence reception of 4097 packets.
   * This tests correct handling of HFN in the case of SN wraparound (SN LEN 12)
   */
  //TESTASSERT(test_rx_in_sequence(4096, normal_init_state, srslte::PDCP_SN_LEN_12, pool, log) == 0);

  /*
   * RX Test 2: PDCP Entity with SN LEN = 12
   * Test in-sequence reception of 4294967297 packets.
   * This tests correct handling of COUNT in the case of [HFN|SN] wraparound
   */
  //pdcp_initial_state test2_init_state = {};
  //TESTASSERT(test_rx_in_sequence(4294967296, near_wraparound_init_state, srslte::PDCP_SN_LEN_12, pool, log) == 0);

  /*
   * RX Test 3: PDCP Entity with SN LEN = 18
   * Test In-sequence reception of 262145 packets.
   * This tests correct handling of HFN in the case of SN wraparound (SN LEN 18)
   */
  pdcp_initial_state test3_init_state = {};
  //TESTASSERT(test_rx_in_sequence(262144, normal_init_state, srslte::PDCP_SN_LEN_18, pool, log) == 0);

  /*
   * RX Test 4: PDCP Entity with SN LEN = 18
   * Test in-sequence reception of 4294967297 packets.
   * This tests correct handling of COUNT in the case of [HFN|SN] wraparound
   */
  //TESTASSERT(test_rx_in_sequence(4294967296, near_wraparound_init_state, srslte::PDCP_SN_LEN_18, pool, log) == 0);

  /*
   * RX Test 5: PDCP Entity with SN LEN = 12
   * Test Reception of one out-of-order packet.
   */
  TESTASSERT(test_rx_out_of_order(normal_init_state, srslte::PDCP_SN_LEN_12, pool, log) == 0);

  /*
   * RX Test 6: PDCP Entity with SN LEN = 12
   * Test Reception of one out-of-order packet at COUNT wraparound.
   */
  TESTASSERT(test_rx_out_of_order(near_wraparound_init_state, srslte::PDCP_SN_LEN_12, pool, log) == 0);
  /*
   * RX Test 5: PDCP Entity with SN LEN = 12
   * Test timeout of t-Reordering when one packet is lost.
   */
  //TESTASSERT(test_rx_out_of_order_timeout(srslte::PDCP_SN_LEN_12, pool, log) == 0);

  /*
   * RX Test 5: PDCP Entity with SN LEN = 12
   * Test timeout of t-Reordering when one packet is lost.
   */
  //TESTASSERT(test_rx_out_of_order(4294967297, srslte::PDCP_SN_LEN_12, pool, log) == 0);
  return 0;
}
// Setup all tests
int run_all_tests(srslte::byte_buffer_pool* pool)
{
  // Setup log
  srslte::log_filter log("PDCP NR Test");
  log.set_level(srslte::LOG_LEVEL_DEBUG);
  log.set_hex_limit(128);

  // Helpers for generating expected PDUs
  // srslte::unique_byte_buffer_t sdu = srslte::allocate_unique_buffer(*pool);
  // sdu->append_bytes(sdu1, sizeof(sdu1));
  // srslte::pdcp_config_t cfg_tx = {1,
  //                                 srslte::PDCP_RB_IS_DRB,
  //                                 srslte::SECURITY_DIRECTION_UPLINK,
  //                                 srslte::SECURITY_DIRECTION_DOWNLINK,
  //                                 srslte::PDCP_SN_LEN_18,
  //                                 srslte::pdcp_t_reordering_t::ms500};
  //gen_expected_pdu(std::move(sdu), 0, cfg_tx, sec_cfg, &log, pool);
  //TESTASSERT(test_tx_all(pool, &log) == 0);
  TESTASSERT(test_rx_all(pool, &log) == 0);
  return 0;
}

int main(int argc, char** argv)
{
  run_all_tests(srslte::byte_buffer_pool::get_instance());
  srslte::byte_buffer_pool::cleanup();
}
