/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
 * Genric function to test transmission of in-sequence packets
 */
int test_tx_sdu_discard(const pdcp_initial_state&    init_state,
                        srsran::pdcp_discard_timer_t discard_timeout,
                        bool                         imediate_notify,
                        srslog::basic_logger&        logger)
{
  srsran::pdcp_config_t cfg = {1,
                               srsran::PDCP_RB_IS_DRB,
                               srsran::SECURITY_DIRECTION_UPLINK,
                               srsran::SECURITY_DIRECTION_DOWNLINK,
                               srsran::PDCP_SN_LEN_12,
                               srsran::pdcp_t_reordering_t::ms500,
                               discard_timeout,
                               false,
                               srsran::srsran_rat_t::nr};

  pdcp_nr_test_helper      pdcp_hlp(cfg, sec_cfg, logger);
  srsran::pdcp_entity_nr*  pdcp  = &pdcp_hlp.pdcp;
  rlc_dummy*               rlc   = &pdcp_hlp.rlc;
  srsue::stack_test_dummy* stack = &pdcp_hlp.stack;

  pdcp_hlp.set_pdcp_initial_state(init_state);

  // Test SDU
  srsran::unique_byte_buffer_t sdu = srsran::make_byte_buffer();
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
int test_tx_discard_all(srslog::basic_logger& logger)
{
  /*
   * TX Test 1: PDCP Entity with SN LEN = 12
   * Test TX PDU discard.
   */
  TESTASSERT(test_tx_sdu_discard(normal_init_state, srsran::pdcp_discard_timer_t::ms50, false, logger) == 0);

  /*
   * TX Test 2: PDCP Entity with SN LEN = 12
   * Test TX PDU discard.
   */
  // TESTASSERT(test_tx_sdu_discard(normal_init_state, srsran::pdcp_discard_timer_t::ms50, true, logger) == 0);
  return 0;
}

// Setup all tests
int run_all_tests()
{
  // Setup log
  auto& logger = srslog::fetch_basic_logger("PDCP NR Test", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(128);

  TESTASSERT(test_tx_discard_all(logger) == 0);
  return 0;
}

int main()
{
  srslog::init();

  if (run_all_tests() != SRSRAN_SUCCESS) {
    fprintf(stderr, "pdcp_nr_tests() failed\n");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}
