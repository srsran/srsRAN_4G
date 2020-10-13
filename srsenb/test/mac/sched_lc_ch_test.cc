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

#include "scheduler_test_common.h"
#include "scheduler_test_utils.h"
#include "srsenb/hdr/stack/mac/scheduler_ue.h"
#include "srslte/common/test_common.h"

using namespace srsenb;
const uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();

/// Tests if a PDU was allocated with lcid and pdu_size bytes
int test_pdu_alloc_successful(srsenb::lch_manager&             lch_handler,
                              sched_interface::dl_sched_pdu_t& pdu,
                              int                              lcid,
                              uint32_t                         pdu_size)
{
  TESTASSERT(lch_handler.get_max_prio_lcid() == lcid);
  TESTASSERT(lch_handler.alloc_rlc_pdu(&pdu, pdu_size) == (int)pdu_size);
  TESTASSERT(pdu.lcid == (uint32_t)lcid);
  TESTASSERT(pdu.nbytes == pdu_size);
  return SRSLTE_SUCCESS;
}

int test_retx_until_empty(srsenb::lch_manager& lch_handler, int lcid, uint32_t pdu_size)
{
  uint32_t                        nof_pdus = lch_handler.get_dl_retx(lcid) / pdu_size;
  sched_interface::dl_sched_pdu_t pdu;

  for (uint32_t i = 0; i < nof_pdus; ++i) {
    TESTASSERT(test_pdu_alloc_successful(lch_handler, pdu, lcid, pdu_size) == SRSLTE_SUCCESS);
    TESTASSERT(lch_handler.get_dl_retx(lcid) == (int)((nof_pdus - i - 1) * pdu_size));
  }
  return nof_pdus * pdu_size;
}

int test_newtx_until_empty(srsenb::lch_manager& lch_handler, int lcid, uint32_t pdu_size)
{
  uint32_t                        nof_pdus = lch_handler.get_dl_tx(lcid) / pdu_size;
  sched_interface::dl_sched_pdu_t pdu;

  for (uint32_t i = 0; i < nof_pdus; ++i) {
    TESTASSERT(test_pdu_alloc_successful(lch_handler, pdu, lcid, pdu_size) == SRSLTE_SUCCESS);
    TESTASSERT(lch_handler.get_dl_tx(lcid) == (int)((nof_pdus - i - 1) * pdu_size));
  }
  return nof_pdus * pdu_size;
}

int test_lc_ch_pbr_infinity()
{
  srsenb::lch_manager lch_handler;

  srsenb::sched_interface::ue_cfg_t ue_cfg        = generate_default_ue_cfg();
  ue_cfg                                          = generate_setup_ue_cfg(ue_cfg);
  ue_cfg.ue_bearers[srsenb::RB_ID_SRB1]           = {};
  ue_cfg.ue_bearers[srsenb::RB_ID_SRB1].direction = sched_interface::ue_bearer_cfg_t::BOTH;
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB1]           = {};
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB1].direction = sched_interface::ue_bearer_cfg_t::BOTH;
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB1].priority  = 5;
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB2]           = {};
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB2].direction = sched_interface::ue_bearer_cfg_t::BOTH;
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB2].priority  = 3;

  lch_handler.set_cfg(ue_cfg);
  lch_handler.new_tti();

  lch_handler.dl_buffer_state(srsenb::RB_ID_SRB1, 50000, 10000);
  lch_handler.dl_buffer_state(srsenb::RB_ID_DRB1, 5000, 10000);
  lch_handler.dl_buffer_state(srsenb::RB_ID_DRB2, 5000, 10000);

  // TEST1 - retx of SRB1 is prioritized. Do not transmit other bearers until there are no SRB1 retxs
  int nof_pending_bytes = lch_handler.get_dl_retx(srsenb::RB_ID_SRB1);
  TESTASSERT(test_retx_until_empty(lch_handler, srsenb::RB_ID_SRB1, 500) == nof_pending_bytes);

  // TEST2 - the DRB2 has lower prio level than SRB1, but has retxs
  nof_pending_bytes = lch_handler.get_dl_retx(srsenb::RB_ID_DRB2);
  TESTASSERT(test_retx_until_empty(lch_handler, srsenb::RB_ID_DRB2, 500) == nof_pending_bytes);

  // TEST3 - the DRB1 has lower prio level, but has retxs
  nof_pending_bytes = lch_handler.get_dl_retx(srsenb::RB_ID_DRB1);
  TESTASSERT(test_retx_until_empty(lch_handler, srsenb::RB_ID_DRB1, 500) == nof_pending_bytes);

  // TEST4 - The SRB1 newtx buffer is emptied before other bearers newtxs
  nof_pending_bytes = lch_handler.get_dl_tx(srsenb::RB_ID_SRB1);
  TESTASSERT(test_newtx_until_empty(lch_handler, srsenb::RB_ID_SRB1, 500) == nof_pending_bytes);

  // TEST5 - The DRB2 newtx buffer is emptied before DRB1 newtxs
  nof_pending_bytes = lch_handler.get_dl_tx(srsenb::RB_ID_DRB2);
  TESTASSERT(test_newtx_until_empty(lch_handler, srsenb::RB_ID_DRB2, 500) == nof_pending_bytes);

  // TEST6 - The DRB1 buffer is emptied
  nof_pending_bytes = lch_handler.get_dl_tx(srsenb::RB_ID_DRB1);
  TESTASSERT(test_newtx_until_empty(lch_handler, srsenb::RB_ID_DRB1, 500) == nof_pending_bytes);

  return SRSLTE_SUCCESS;
}

int test_lc_ch_pbr_finite()
{
  srsenb::lch_manager             lch_handler;
  sched_interface::dl_sched_pdu_t pdu;

  srsenb::sched_interface::ue_cfg_t ue_cfg        = generate_default_ue_cfg();
  ue_cfg                                          = generate_setup_ue_cfg(ue_cfg);
  ue_cfg.ue_bearers[srsenb::RB_ID_SRB1]           = {};
  ue_cfg.ue_bearers[srsenb::RB_ID_SRB1].direction = sched_interface::ue_bearer_cfg_t::BOTH;
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB1]           = {};
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB1].direction = sched_interface::ue_bearer_cfg_t::BOTH;
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB1].pbr       = 256; // kBps
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB1].bsd       = 50;  // msec
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB1].priority  = 5;
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB2]           = {};
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB2].direction = sched_interface::ue_bearer_cfg_t::BOTH;
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB2].pbr       = 8;  // kBps
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB2].bsd       = 50; // msec
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB2].priority  = 3;

  lch_handler.set_cfg(ue_cfg);
  for (uint32_t i = 0; i < 50; ++i) {
    lch_handler.new_tti();
  }
  // Bj={0, infinity, 0, 12800, 400}

  lch_handler.dl_buffer_state(srsenb::RB_ID_SRB1, 50000, 1000);
  lch_handler.dl_buffer_state(srsenb::RB_ID_DRB1, 50000, 1000);
  lch_handler.dl_buffer_state(srsenb::RB_ID_DRB2, 50000, 0);

  // TEST1 - SRB1 retxs are emptied first
  int nof_pending_bytes = lch_handler.get_dl_retx(srsenb::RB_ID_SRB1);
  TESTASSERT(test_retx_until_empty(lch_handler, srsenb::RB_ID_SRB1, 500) == nof_pending_bytes);

  // TEST2 - DRB1 retxs are emptied
  nof_pending_bytes = lch_handler.get_dl_retx(srsenb::RB_ID_DRB1);
  TESTASSERT(test_retx_until_empty(lch_handler, srsenb::RB_ID_DRB1, 500) == nof_pending_bytes);

  // TEST3 - SRB1 newtxs are emptied (PBR==infinity)
  nof_pending_bytes = lch_handler.get_dl_tx(srsenb::RB_ID_SRB1);
  TESTASSERT(test_newtx_until_empty(lch_handler, srsenb::RB_ID_SRB1, 500) == nof_pending_bytes);

  // TEST4 - DRB2 has higher priority so it gets allocated until Bj <= 0
  TESTASSERT(test_pdu_alloc_successful(lch_handler, pdu, srsenb::RB_ID_DRB2, 200) == SRSLTE_SUCCESS);
  // Bj={0, infinity, 0, 12800, 200}
  TESTASSERT(test_pdu_alloc_successful(lch_handler, pdu, srsenb::RB_ID_DRB2, 600) == SRSLTE_SUCCESS);
  // Bj={0, infinity, 0, 256000, -400}

  // TEST5 - DRB1 has lower prio, but DRB2 Bj <= 0.
  for (uint32_t i = 0; i < 50; ++i) {
    lch_handler.new_tti();
    TESTASSERT(test_pdu_alloc_successful(lch_handler, pdu, srsenb::RB_ID_DRB1, 50) == SRSLTE_SUCCESS);
  }

  // TEST6 - new tti restores DRB2 Bj>=0, and DRB2 gets allocated
  lch_handler.new_tti();
  // Bj={0, infinity, 0, 256000, 8}
  TESTASSERT(test_pdu_alloc_successful(lch_handler, pdu, srsenb::RB_ID_DRB2, 50) == SRSLTE_SUCCESS);
  // Bj={0, infinity, 0, 256000, -42}
  lch_handler.new_tti();
  TESTASSERT(test_pdu_alloc_successful(lch_handler, pdu, srsenb::RB_ID_DRB1, 50) == SRSLTE_SUCCESS);

  return SRSLTE_SUCCESS;
}

int main()
{
  srsenb::set_randseed(seed);
  srslte::console("This is the chosen seed: %u\n", seed);
  srslte::logmap::get("TEST")->set_level(srslte::LOG_LEVEL_INFO);

  TESTASSERT(test_lc_ch_pbr_infinity() == SRSLTE_SUCCESS);
  TESTASSERT(test_lc_ch_pbr_finite() == SRSLTE_SUCCESS);
  srslte::console("Success\n");
}
