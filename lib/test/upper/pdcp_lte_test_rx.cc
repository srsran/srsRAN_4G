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
#include "pdcp_lte_test.h"
#include <numeric>

/*
 * Genric function to test reception of in-sequence packets
 */
int test_rx(std::vector<pdcp_test_event_t>      events,
            const srslte::pdcp_lte_state_t&     init_state,
            uint8_t                             pdcp_sn_len,
            srslte::pdcp_rb_type_t              rb_type,
            uint32_t                            n_sdus_exp,
            const srslte::unique_byte_buffer_t& sdu_exp,
            srslte::byte_buffer_pool*           pool,
            srslte::log_ref                     log)

{
  srslte::pdcp_config_t cfg_rx = {1,
                                  rb_type,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  pdcp_sn_len,
                                  srslte::pdcp_t_reordering_t::ms500,
                                  srslte::pdcp_discard_timer_t::infinity};

  pdcp_lte_test_helper     pdcp_hlp_rx(cfg_rx, sec_cfg, log);
  srslte::pdcp_entity_lte* pdcp_rx = &pdcp_hlp_rx.pdcp;
  gw_dummy*                gw_rx   = &pdcp_hlp_rx.gw;
  rrc_dummy*               rrc_rx  = &pdcp_hlp_rx.rrc;
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
  if (rb_type == srslte::PDCP_RB_IS_DRB) {
    TESTASSERT(gw_rx->rx_count == n_sdus_exp);
    if (n_sdus_exp > 0) {
      srslte::unique_byte_buffer_t sdu_act = allocate_unique_buffer(*pool);
      gw_rx->get_last_pdu(sdu_act);
      TESTASSERT(compare_two_packets(sdu_exp, sdu_act) == 0);
    }

  } else {
    TESTASSERT(rrc_rx->rx_count == n_sdus_exp);
    if (n_sdus_exp > 0) {
      srslte::unique_byte_buffer_t sdu_act = allocate_unique_buffer(*pool);
      rrc_rx->get_last_pdu(sdu_act);
      TESTASSERT(compare_two_packets(sdu_exp, sdu_act) == 0);
    }
  }
  return 0;
}

/*
 * RX Test: PDCP Entity with SN LEN = 5 and 12.
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
   * RX Test 1: PDCP LTE Entity with SN LEN = 5
   * Test in-sequence reception of 32 packets.
   * This tests correct handling of HFN in the case of SN wraparound (SN LEN 5)
   */
  {
    std::vector<uint32_t> test1_counts(2);                   // Test two packets
    std::iota(test1_counts.begin(), test1_counts.end(), 31); // Starting at COUNT 31
    std::vector<pdcp_test_event_t> test1_pdus = gen_expected_pdus_vector(
        tst_sdu1, test1_counts, srslte::PDCP_SN_LEN_5, srslte::PDCP_RB_IS_SRB, sec_cfg, pool, log);
    srslte::pdcp_lte_state_t test1_init_state = {
        .next_pdcp_tx_sn = 0, .tx_hfn = 0, .rx_hfn = 0, .next_pdcp_rx_sn = 31, .last_submitted_pdcp_rx_sn = 30};
    TESTASSERT(test_rx(std::move(test1_pdus),
                       test1_init_state,
                       srslte::PDCP_SN_LEN_5,
                       srslte::PDCP_RB_IS_SRB,
                       2,
                       tst_sdu1,
                       pool,
                       log) == 0);
  }

  /*
   * RX Test 2: PDCP LTE Entity with SN LEN = 12
   * Test in-sequence reception of 4096 packets.
   * This tests correct handling of HFN in the case of SN wraparound (SN LEN 12)
   */
  {
    std::vector<uint32_t> test_counts(2);                    // Test two packets
    std::iota(test_counts.begin(), test_counts.end(), 4095); // Starting at COUNT 4095
    std::vector<pdcp_test_event_t> test_pdus = gen_expected_pdus_vector(
        tst_sdu1, test_counts, srslte::PDCP_SN_LEN_12, srslte::PDCP_RB_IS_DRB, sec_cfg, pool, log);
    srslte::pdcp_lte_state_t test_init_state = {
        .next_pdcp_tx_sn = 0, .tx_hfn = 0, .rx_hfn = 0, .next_pdcp_rx_sn = 4095, .last_submitted_pdcp_rx_sn = 4094};
    TESTASSERT(test_rx(std::move(test_pdus),
                       test_init_state,
                       srslte::PDCP_SN_LEN_12,
                       srslte::PDCP_RB_IS_DRB,
                       2,
                       tst_sdu1,
                       pool,
                       log) == 0);
  }

  /*
   * RX Test 3: PDCP LTE Entity with SN LEN = 12
   * Test reception of a dublicate SN, the duplicate should just be dropped.
   */
  {
    std::vector<uint32_t> test_counts(2);                  // Test two packets
    std::iota(test_counts.begin(), test_counts.end(), 31); // Starting at COUNT 31
    std::vector<pdcp_test_event_t> test_pdus = gen_expected_pdus_vector(
        tst_sdu1, test_counts, srslte::PDCP_SN_LEN_12, srslte::PDCP_RB_IS_DRB, sec_cfg, pool, log);
    srslte::pdcp_lte_state_t test_init_state = {
        .next_pdcp_tx_sn = 0, .tx_hfn = 0, .rx_hfn = 0, .next_pdcp_rx_sn = 32, .last_submitted_pdcp_rx_sn = 31};
    TESTASSERT(test_rx(std::move(test_pdus),
                       test_init_state,
                       srslte::PDCP_SN_LEN_12,
                       srslte::PDCP_RB_IS_DRB,
                       test_counts.size() - 1,
                       tst_sdu1,
                       pool,
                       log) == 0);
  }

  return SRSLTE_SUCCESS;
}

// Basic test to verify the correct handling of PDCP status PDUs on DRBs
// As long as we don't implement status reporting, the PDU shall be dropped
int test_rx_control_pdu(srslte::byte_buffer_pool* pool, srslte::log_ref log)
{
  const uint8_t pdcp_status_report_long[] = {0x0a, 0xc9, 0x3c};

  std::vector<pdcp_test_event_t> pdu_vec;

  pdcp_test_event_t event;
  event.pkt = allocate_unique_buffer(*pool);
  memcpy(event.pkt->msg, pdcp_status_report_long, sizeof(pdcp_status_report_long));
  event.pkt->N_bytes = sizeof(pdcp_status_report_long);
  pdu_vec.push_back(std::move(event));

  srslte::unique_byte_buffer_t tst_sdu1 = allocate_unique_buffer(*pool);

  srslte::pdcp_lte_state_t test_init_state = {
      .next_pdcp_tx_sn = 0, .tx_hfn = 0, .rx_hfn = 0, .next_pdcp_rx_sn = 32, .last_submitted_pdcp_rx_sn = 31};
  TESTASSERT(test_rx(std::move(pdu_vec),
                     test_init_state,
                     srslte::PDCP_SN_LEN_12,
                     srslte::PDCP_RB_IS_DRB,
                     0,
                     tst_sdu1,
                     pool,
                     log) == 0);

  return SRSLTE_SUCCESS;
}

// Setup all tests
int run_all_tests(srslte::byte_buffer_pool* pool)
{
  // Setup log
  srslte::log_ref log("PDCP LTE Test RX");
  log->set_level(srslte::LOG_LEVEL_DEBUG);
  log->set_hex_limit(128);

  TESTASSERT(test_rx_all(pool, log) == 0);
  TESTASSERT(test_rx_control_pdu(pool, log) == 0);

  return 0;
}

int main()
{
  if (run_all_tests(srslte::byte_buffer_pool::get_instance()) != SRSLTE_SUCCESS) {
    fprintf(stderr, "pdcp_nr_tests_rx() failed\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}
