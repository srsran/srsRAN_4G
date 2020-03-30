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
int test_tx_sdu_discard(const pdcp_initial_state&    init_state,
                        srslte::pdcp_discard_timer_t discard_timeout,
                        bool                         imediate_notify,
                        srslte::byte_buffer_pool*    pool,
                        srslte::log_ref              log)
{
  srslte::pdcp_config_t cfg = {1,
                               srslte::PDCP_RB_IS_DRB,
                               srslte::SECURITY_DIRECTION_UPLINK,
                               srslte::SECURITY_DIRECTION_DOWNLINK,
                               srslte::PDCP_SN_LEN_12,
                               srslte::pdcp_t_reordering_t::ms500,
                               discard_timeout};

  pdcp_nr_test_helper     pdcp_hlp(cfg, sec_cfg, log);
  srslte::pdcp_entity_nr* pdcp   = &pdcp_hlp.pdcp;
  rlc_dummy*              rlc    = &pdcp_hlp.rlc;
  srslte::timer_handler*  timers = &pdcp_hlp.stack.timers;

  pdcp_hlp.set_pdcp_initial_state(init_state);

  // Test SDU
  srslte::unique_byte_buffer_t sdu = allocate_unique_buffer(*pool);
  sdu->append_bytes(sdu1, sizeof(sdu1));
  pdcp->write_sdu(std::move(sdu), true);

  for (uint32_t i = 0; i < static_cast<uint32_t>(cfg.discard_timer) - 1; ++i) {
    timers->step_all();
  }
  TESTASSERT(rlc->discard_count == 0);

  // Check if timers were started
  if (imediate_notify) {
    TESTASSERT(pdcp->nof_discard_timers() == 0); // RLC notified PDCP already, timer should have been disarmed
  } else {
    TESTASSERT(pdcp->nof_discard_timers() == 1); // One timer should be running
  }

  // Last timer step
  timers->step_all();

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
int test_tx_discard_all(srslte::byte_buffer_pool* pool, srslte::log_ref log)
{

  /*
   * TX Test 1: PDCP Entity with SN LEN = 12
   * Test TX PDU discard.
   */
  TESTASSERT(test_tx_sdu_discard(normal_init_state, srslte::pdcp_discard_timer_t::ms50, false, pool, log) == 0);

  /*
   * TX Test 2: PDCP Entity with SN LEN = 12
   * Test TX PDU discard.
   */
  // TESTASSERT(test_tx_sdu_discard(normal_init_state, srslte::pdcp_discard_timer_t::ms50, true, pool, log) == 0);
  return 0;
}

// Setup all tests
int run_all_tests(srslte::byte_buffer_pool* pool)
{
  // Setup log
  srslte::log_ref log("PDCP NR Test");
  log->set_level(srslte::LOG_LEVEL_DEBUG);
  log->set_hex_limit(128);

  TESTASSERT(test_tx_discard_all(pool, log) == 0);
  return 0;
}

int main()
{
  if (run_all_tests(srslte::byte_buffer_pool::get_instance()) != SRSLTE_SUCCESS) {
    fprintf(stderr, "pdcp_nr_tests() failed\n");
    return SRSLTE_ERROR;
  }
  srslte::byte_buffer_pool::cleanup();

  return SRSLTE_SUCCESS;
}
