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
#include "pdcp_nr_test.h"
#include <numeric>

/*
 * Genric function to test transmission of in-sequence packets
 */
int test_tx(uint32_t                     n_packets,
            const pdcp_initial_state&    init_state,
            uint8_t                      pdcp_sn_len,
            uint64_t                     n_pdus_exp,
            srsran::unique_byte_buffer_t pdu_exp,
            srslog::basic_logger&        logger)
{
  srsran::pdcp_config_t cfg = {1,
                               srsran::PDCP_RB_IS_DRB,
                               srsran::SECURITY_DIRECTION_UPLINK,
                               srsran::SECURITY_DIRECTION_DOWNLINK,
                               pdcp_sn_len,
                               srsran::pdcp_t_reordering_t::ms500,
                               srsran::pdcp_discard_timer_t::infinity,
                               false,
                               srsran::srsran_rat_t::nr};

  pdcp_nr_test_helper     pdcp_hlp(cfg, sec_cfg, logger);
  srsran::pdcp_entity_nr* pdcp = &pdcp_hlp.pdcp;
  rlc_dummy*              rlc  = &pdcp_hlp.rlc;

  pdcp_hlp.set_pdcp_initial_state(init_state);

  // Run test
  for (uint32_t i = 0; i < n_packets; ++i) {
    // Test SDU
    srsran::unique_byte_buffer_t sdu = srsran::make_byte_buffer();
    sdu->append_bytes(sdu1, sizeof(sdu1));
    pdcp->write_sdu(std::move(sdu));
  }

  srsran::unique_byte_buffer_t pdu_act = srsran::make_byte_buffer();
  rlc->get_last_sdu(pdu_act);

  TESTASSERT(rlc->rx_count == n_pdus_exp);
  TESTASSERT(compare_two_packets(pdu_act, pdu_exp) == 0);
  return 0;
}

/*
 * TX Test: PDCP Entity with SN LEN = 12 and 18.
 * PDCP entity configured with EIA2 and EEA2
 */
int test_tx_all(srslog::basic_logger& logger)
{
  uint64_t n_packets;
  /*
   * TX Test 1: PDCP Entity with SN LEN = 12
   * TX_NEXT = 0.
   * Input: {0x18, 0xE2}
   * Output:  {0x80, 0x00, 0x8f, 0xe3, 0xc7, 0x1b, 0xad, 0x14}
   */
  {
    auto&                       test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger delimiter("TX COUNT 0, 12 bit SN");
    n_packets                                         = 1;
    srsran::unique_byte_buffer_t pdu_exp_count0_len12 = srsran::make_byte_buffer();
    pdu_exp_count0_len12->append_bytes(pdu1_count0_snlen12, sizeof(pdu1_count0_snlen12));
    TESTASSERT(
        test_tx(
            n_packets, normal_init_state, srsran::PDCP_SN_LEN_12, n_packets, std::move(pdu_exp_count0_len12), logger) ==
        0);
  }
  /*
   * TX Test 2: PDCP Entity with SN LEN = 12
   * TX_NEXT = 2048.
   * Input: {0x18, 0xE2}
   * Output: {0x88, 0x00, 0x8d, 0x2c, 0xe5, 0x38, 0xc0, 0x42}
   */
  {
    auto&                       test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger delimiter("TX COUNT 2048, 12 bit SN");
    n_packets                                            = 2049;
    srsran::unique_byte_buffer_t pdu_exp_count2048_len12 = srsran::make_byte_buffer();
    pdu_exp_count2048_len12->append_bytes(pdu1_count2048_snlen12, sizeof(pdu1_count2048_snlen12));
    TESTASSERT(test_tx(n_packets,
                       normal_init_state,
                       srsran::PDCP_SN_LEN_12,
                       n_packets,
                       std::move(pdu_exp_count2048_len12),
                       logger) == 0);
  }
  /*
   * TX Test 3: PDCP Entity with SN LEN = 12
   * TX_NEXT = 4096.
   * Input: {0x18, 0xE2}
   * Output: {0x80, 0x00, 0x97, 0xbe, 0xee, 0x62, 0xf5, 0xe0}
   */
  {
    auto&                       test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger delimiter("TX COUNT 4096, 12 bit SN");
    n_packets                                            = 4097;
    srsran::unique_byte_buffer_t pdu_exp_count4096_len12 = srsran::make_byte_buffer();
    pdu_exp_count4096_len12->append_bytes(pdu1_count4096_snlen12, sizeof(pdu1_count4096_snlen12));
    TESTASSERT(test_tx(n_packets,
                       normal_init_state,
                       srsran::PDCP_SN_LEN_12,
                       n_packets,
                       std::move(pdu_exp_count4096_len12),
                       logger) == 0);
  }
  /*
   * TX Test 4: PDCP Entity with SN LEN = 18
   * TX_NEXT = 0.
   * Input: {0x18, 0xE2}
   * Output: {0x80, 0x00, 0x00, 0x8f, 0xe3, 0x37, 0x33, 0xd5, 0x64}
   */
  {
    auto&                       test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger delimiter("TX COUNT 0, 18 bit SN");
    n_packets                                         = 1;
    srsran::unique_byte_buffer_t pdu_exp_count0_len18 = srsran::make_byte_buffer();
    pdu_exp_count0_len18->append_bytes(pdu1_count0_snlen18, sizeof(pdu1_count0_snlen18));
    TESTASSERT(
        test_tx(
            n_packets, normal_init_state, srsran::PDCP_SN_LEN_18, n_packets, std::move(pdu_exp_count0_len18), logger) ==
        0);
  }

  /*
   * TX Test 5: PDCP Entity with SN LEN = 18
   * TX_NEXT = 131072.
   * Input: {0x18, 0xE2}
   * Output: {0x82, 0x00, 0x00, 0x15, 0x01, 0x99, 0x97, 0xe0, 0x4e}
   */
  {
    auto&                       test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger delimiter("TX COUNT 131072, 18 bit SN");
    n_packets                                           = 131073;
    srsran::unique_byte_buffer_t pdu_exp_sn131072_len18 = srsran::make_byte_buffer();
    pdu_exp_sn131072_len18->append_bytes(pdu1_count131072_snlen18, sizeof(pdu1_count131072_snlen18));
    TESTASSERT(test_tx(n_packets,
                       normal_init_state,
                       srsran::PDCP_SN_LEN_18,
                       n_packets,
                       std::move(pdu_exp_sn131072_len18),
                       logger) == 0);
  }

  /*
   * TX Test 6: PDCP Entity with SN LEN = 18
   * TX_NEXT = 262144.
   * Input: {0x18, 0xE2}
   * Output: {0x80, 0x00, 0x00, 0xc2, 0x47, 0xc2, 0xee, 0x46, 0xd9}
   */
  {
    auto&                       test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger delimiter("TX COUNT 262144, 18 bit SN");
    n_packets                                              = 262145;
    srsran::unique_byte_buffer_t pdu_exp_count262144_len18 = srsran::make_byte_buffer();
    pdu_exp_count262144_len18->append_bytes(pdu1_count262144_snlen18, sizeof(pdu1_count262144_snlen18));
    TESTASSERT(test_tx(n_packets,
                       normal_init_state,
                       srsran::PDCP_SN_LEN_18,
                       n_packets,
                       std::move(pdu_exp_count262144_len18),
                       logger) == 0);
  }
  /*
   * TX Test 7: PDCP Entity with SN LEN = 12
   * Test TX at COUNT wraparound.
   * Should print a warning and drop all packets after wrap around.
   */
  {
    auto&                       test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger delimiter("TX COUNT wrap arround, 12 bit SN");
    n_packets                                                  = 5;
    srsran::unique_byte_buffer_t pdu_exp_count4294967295_len12 = srsran::make_byte_buffer();
    pdu_exp_count4294967295_len12->append_bytes(pdu1_count4294967295_snlen12, sizeof(pdu1_count4294967295_snlen12));
    TESTASSERT(test_tx(n_packets,
                       near_wraparound_init_state,
                       srsran::PDCP_SN_LEN_12,
                       1,
                       std::move(pdu_exp_count4294967295_len12),
                       logger) == 0);
  }

  /*
   * TX Test 8: PDCP Entity with SN LEN = 18
   * Test TX at COUNT wraparound.
   * Should print a warning and drop all packets after wraparound.
   */
  {
    auto&                       test_logger = srslog::fetch_basic_logger("TESTER  ");
    srsran::test_delimit_logger delimiter("TX COUNT wrap arround, 12 bit SN");
    n_packets                                                  = 5;
    srsran::unique_byte_buffer_t pdu_exp_count4294967295_len18 = srsran::make_byte_buffer();
    pdu_exp_count4294967295_len18->append_bytes(pdu1_count4294967295_snlen18, sizeof(pdu1_count4294967295_snlen18));
    TESTASSERT(test_tx(n_packets,
                       near_wraparound_init_state,
                       srsran::PDCP_SN_LEN_18,
                       1,
                       std::move(pdu_exp_count4294967295_len18),
                       logger) == 0);
  }
  return SRSRAN_SUCCESS;
}

// Setup all tests
int run_all_tests()
{
  // Setup log
  auto& logger = srslog::fetch_basic_logger("PDCP NR Test TX", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(128);

  TESTASSERT(test_tx_all(logger) == 0);
  return 0;
}

int main()
{
  srslog::init();

  if (run_all_tests() != SRSRAN_SUCCESS) {
    fprintf(stderr, "pdcp_nr_tests_tx() failed\n");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}
