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

int test_retx_prb_inf(srsenb::lch_manager& lch_handler, int lcid, uint32_t pdu_size)
{
  uint32_t                        nof_pdus = lch_handler.get_dl_retx(lcid) / pdu_size;
  sched_interface::dl_sched_pdu_t pdu;

  for (uint32_t i = 0; i < nof_pdus; ++i) {
    TESTASSERT(lch_handler.get_max_prio_lcid() == lcid);
    TESTASSERT(lch_handler.alloc_rlc_pdu(&pdu, pdu_size) == (int)pdu_size);
    TESTASSERT(pdu.lcid == (uint32_t)lcid);
    TESTASSERT(pdu.nbytes == pdu_size);
    TESTASSERT(lch_handler.get_dl_retx(lcid) == (int)((nof_pdus - i - 1) * pdu_size));
  }
  return nof_pdus * pdu_size;
}

int test_newtx_prb_inf(srsenb::lch_manager& lch_handler, int lcid, uint32_t pdu_size)
{
  uint32_t                        nof_pdus = lch_handler.get_dl_tx(lcid) / pdu_size;
  sched_interface::dl_sched_pdu_t pdu;

  for (uint32_t i = 0; i < nof_pdus; ++i) {
    TESTASSERT(lch_handler.get_max_prio_lcid() == lcid);
    TESTASSERT(lch_handler.alloc_rlc_pdu(&pdu, pdu_size) == (int)pdu_size);
    TESTASSERT(pdu.lcid == (uint32_t)lcid);
    TESTASSERT(pdu.nbytes == pdu_size);
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
  TESTASSERT(test_retx_prb_inf(lch_handler, srsenb::RB_ID_SRB1, 500) == nof_pending_bytes);

  // TEST2 - the DRB2 has lower prio level than SRB1, but has retxs
  nof_pending_bytes = lch_handler.get_dl_retx(srsenb::RB_ID_DRB2);
  TESTASSERT(test_retx_prb_inf(lch_handler, srsenb::RB_ID_DRB2, 500) == nof_pending_bytes);

  // TEST3 - the DRB1 has lower prio level, but has retxs
  nof_pending_bytes = lch_handler.get_dl_retx(srsenb::RB_ID_DRB1);
  TESTASSERT(test_retx_prb_inf(lch_handler, srsenb::RB_ID_DRB1, 500) == nof_pending_bytes);

  // TEST4 - The SRB1 newtx buffer is emptied before other bearers newtxs
  nof_pending_bytes = lch_handler.get_dl_tx(srsenb::RB_ID_SRB1);
  TESTASSERT(test_newtx_prb_inf(lch_handler, srsenb::RB_ID_SRB1, 500) == nof_pending_bytes);

  // TEST5 - The DRB2 newtx buffer is emptied before DRB1 newtxs
  nof_pending_bytes = lch_handler.get_dl_tx(srsenb::RB_ID_DRB2);
  TESTASSERT(test_newtx_prb_inf(lch_handler, srsenb::RB_ID_DRB2, 500) == nof_pending_bytes);

  // TEST6 - The DRB1 buffer is emptied
  nof_pending_bytes = lch_handler.get_dl_tx(srsenb::RB_ID_DRB1);
  TESTASSERT(test_newtx_prb_inf(lch_handler, srsenb::RB_ID_DRB1, 500) == nof_pending_bytes);

  return SRSLTE_SUCCESS;
}

int main()
{
  srsenb::set_randseed(seed);
  srslte::console("This is the chosen seed: %u\n", seed);
  srslte::logmap::get("TEST")->set_level(srslte::LOG_LEVEL_INFO);

  TESTASSERT(test_lc_ch_pbr_infinity() == SRSLTE_SUCCESS);
  srslte::console("Success\n");
}
