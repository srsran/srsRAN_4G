/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
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
int test_tx_sdu_discard(const pdcp_initial_state&    init_state,
                        srslte::pdcp_discard_timer_t discard_timeout,
                        bool                         imediate_notify,
                        srslte::log_ref              log)
{
  srslte::pdcp_config_t cfg = {1,
                               srslte::PDCP_RB_IS_DRB,
                               srslte::SECURITY_DIRECTION_UPLINK,
                               srslte::SECURITY_DIRECTION_DOWNLINK,
                               srslte::PDCP_SN_LEN_12,
                               srslte::pdcp_t_reordering_t::ms500,
                               discard_timeout};

  pdcp_nr_test_helper      pdcp_hlp(cfg, sec_cfg, log);
  srslte::pdcp_entity_nr*  pdcp  = &pdcp_hlp.pdcp;
  rlc_dummy*               rlc   = &pdcp_hlp.rlc;
  srsue::stack_test_dummy* stack = &pdcp_hlp.stack;

  pdcp_hlp.set_pdcp_initial_state(init_state);

  // Test SDU
  srslte::unique_byte_buffer_t sdu = srslte::make_byte_buffer();
  sdu->append_bytes(sdu1, sizeof(sdu1));
  pdcp->write_sdu(std::move(sdu));

  for (uint32_t i = 0; i < static_cast<uint32_t>(cfg.discard_timer) - 1; ++i) {
    stack->run_tti();
  }
  TESTASSERT(rlc->discard_count == 0);

  // Check if timers were started
  if (imediate_notify) {
    TESTASSERT(pdcp->nof_discard_timers() == 0); // RLC notified PDCP already, timer should have been disarmed
  } else {
    TESTASSERT(pdcp->nof_discard_timers() == 1); // One timer should be running
  }

  // Last timer step
  stack->run_tti();

  // Check if RLC was notified of SDU discard
  if (imediate_notify) {
    TESTASSERT(rlc->discard_count == 0); // RLC imediatly notified the PDCP of tx, there should be no timeouts
  } else {
    TESTASSERT(rlc->discard_count == 1); // RLC does not notify the the PDCP of tx, there should be a timeout
  }

  // Make sure there are no timers still left on the map
  TESTASSERT(pdcp->nof_discard_timers() == 0);

  return 0;
}

/*
 * TX Test: PDCP Entity with SN LEN = 12 and 18.
 * PDCP entity configured with EIA2 and EEA2
 */
int test_tx_discard_all(srslte::log_ref log)
{
  /*
   * TX Test 1: PDCP Entity with SN LEN = 12
   * Test TX PDU discard.
   */
  TESTASSERT(test_tx_sdu_discard(normal_init_state, srslte::pdcp_discard_timer_t::ms50, false, log) == 0);

  /*
   * TX Test 2: PDCP Entity with SN LEN = 12
   * Test TX PDU discard.
   */
  // TESTASSERT(test_tx_sdu_discard(normal_init_state, srslte::pdcp_discard_timer_t::ms50, true, log) == 0);
  return 0;
}

// Setup all tests
int run_all_tests()
{
  // Setup log
  srslte::log_ref log("PDCP NR Test");
  log->set_level(srslte::LOG_LEVEL_DEBUG);
  log->set_hex_limit(128);

  TESTASSERT(test_tx_discard_all(log) == 0);
  return 0;
}

int main()
{
  if (run_all_tests() != SRSLTE_SUCCESS) {
    fprintf(stderr, "pdcp_nr_tests() failed\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}
