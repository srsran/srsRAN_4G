/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "sched_test_common.h"
#include "sched_test_utils.h"
#include "srsenb/hdr/stack/mac/sched_ue.h"
#include "srsran/common/test_common.h"

using namespace srsenb;
const uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();

uint32_t rlc_overhead(uint32_t lcid)
{
  return lcid == 0 ? 0 : 3;
}

uint32_t add_rlc_overhead(uint32_t lcid, uint32_t rlc_payload_size)
{
  return rlc_payload_size + (rlc_payload_size == 0 ? 0 : rlc_overhead(lcid));
}

/// Tests if a PDU was allocated with lcid and pdu_size bytes
int test_pdu_alloc_successful(srsenb::lch_ue_manager&          lch_handler,
                              sched_interface::dl_sched_pdu_t& pdu,
                              int                              lcid,
                              uint32_t                         rlc_payload_size)
{
  uint32_t mac_sdu_size = add_rlc_overhead(lcid, rlc_payload_size);
  TESTASSERT(lch_handler.get_max_prio_lcid() == lcid);
  TESTASSERT(lch_handler.alloc_rlc_pdu(&pdu, mac_sdu_size) == (int)mac_sdu_size);
  TESTASSERT(pdu.lcid == (uint32_t)lcid);
  TESTASSERT(pdu.nbytes == mac_sdu_size);
  return SRSRAN_SUCCESS;
}

int test_retx_until_empty(srsenb::lch_ue_manager& lch_handler, int lcid, uint32_t rlc_payload_size)
{
  int start_rlc_bytes = lch_handler.get_dl_prio_tx(lcid);
  int nof_pdus        = ceil(static_cast<float>(start_rlc_bytes) / static_cast<float>(rlc_payload_size));
  int rem_rlc_bytes   = start_rlc_bytes;

  sched_interface::dl_sched_pdu_t pdu;
  for (int i = 0; i < nof_pdus; ++i) {
    uint32_t expected_payload_size = std::min(rlc_payload_size, (uint32_t)rem_rlc_bytes);
    TESTASSERT(test_pdu_alloc_successful(lch_handler, pdu, lcid, expected_payload_size) == SRSRAN_SUCCESS);
    rem_rlc_bytes -= expected_payload_size;
    TESTASSERT(lch_handler.get_dl_prio_tx(lcid) == rem_rlc_bytes);
  }
  return start_rlc_bytes;
}

int test_newtx_until_empty(srsenb::lch_ue_manager& lch_handler, int lcid, uint32_t rlc_payload_size)
{
  int start_rlc_bytes = lch_handler.get_dl_tx(lcid);
  int nof_pdus        = ceil(static_cast<float>(start_rlc_bytes) / (float)rlc_payload_size);
  int rem_rlc_bytes   = start_rlc_bytes;

  sched_interface::dl_sched_pdu_t pdu;
  for (int i = 0; i < nof_pdus; ++i) {
    uint32_t expected_payload_size = std::min(rlc_payload_size, (uint32_t)rem_rlc_bytes);
    TESTASSERT(test_pdu_alloc_successful(lch_handler, pdu, lcid, expected_payload_size) == SRSRAN_SUCCESS);
    rem_rlc_bytes -= expected_payload_size;
    TESTASSERT(lch_handler.get_dl_tx(lcid) == (int)rem_rlc_bytes);
  }
  return start_rlc_bytes;
}

int test_lc_ch_pbr_infinity()
{
  srsenb::lch_ue_manager lch_handler{0x46};

  srsenb::sched_interface::ue_cfg_t ue_cfg                  = generate_default_ue_cfg();
  ue_cfg                                                    = generate_setup_ue_cfg(ue_cfg);
  ue_cfg.ue_bearers[srb_to_lcid((lte_srb::srb1))]           = {};
  ue_cfg.ue_bearers[srb_to_lcid((lte_srb::srb1))].direction = mac_lc_ch_cfg_t::BOTH;
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb1))]           = {};
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb1))].direction = mac_lc_ch_cfg_t::BOTH;
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb1))].priority  = 5;
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb2))]           = {};
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb2))].direction = mac_lc_ch_cfg_t::BOTH;
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb2))].priority  = 3;

  lch_handler.set_cfg(ue_cfg);
  lch_handler.new_tti();

  lch_handler.dl_buffer_state(srb_to_lcid(lte_srb::srb1), 50000, 10000);
  lch_handler.dl_buffer_state(drb_to_lcid(lte_drb::drb1), 5000, 10000);
  lch_handler.dl_buffer_state(drb_to_lcid(lte_drb::drb2), 5000, 10000);

  // TEST1 - retx of SRB1 is prioritized. Do not transmit other bearers until there are no SRB1 retxs
  int nof_pending_bytes = lch_handler.get_dl_prio_tx(srb_to_lcid(lte_srb::srb1));
  TESTASSERT(test_retx_until_empty(lch_handler, srb_to_lcid(lte_srb::srb1), 500) == nof_pending_bytes);

  // TEST2 - the DRB2 has lower prio level than SRB1, but has retxs
  nof_pending_bytes = lch_handler.get_dl_prio_tx(drb_to_lcid(lte_drb::drb2));
  TESTASSERT(test_retx_until_empty(lch_handler, drb_to_lcid(lte_drb::drb2), 500) == nof_pending_bytes);

  // TEST3 - the DRB1 has lower prio level, but has retxs
  nof_pending_bytes = lch_handler.get_dl_prio_tx(drb_to_lcid(lte_drb::drb1));
  TESTASSERT(test_retx_until_empty(lch_handler, drb_to_lcid(lte_drb::drb1), 500) == nof_pending_bytes);

  // TEST4 - The SRB1 newtx buffer is emptied before other bearers newtxs
  nof_pending_bytes = lch_handler.get_dl_tx(srb_to_lcid(lte_srb::srb1));
  TESTASSERT(test_newtx_until_empty(lch_handler, srb_to_lcid(lte_srb::srb1), 500) == nof_pending_bytes);

  // TEST5 - The DRB2 newtx buffer is emptied before DRB1 newtxs
  nof_pending_bytes = lch_handler.get_dl_tx(drb_to_lcid(lte_drb::drb2));
  TESTASSERT(test_newtx_until_empty(lch_handler, drb_to_lcid(lte_drb::drb2), 500) == nof_pending_bytes);

  // TEST6 - The DRB1 buffer is emptied
  nof_pending_bytes = lch_handler.get_dl_tx(drb_to_lcid(lte_drb::drb1));
  TESTASSERT(test_newtx_until_empty(lch_handler, drb_to_lcid(lte_drb::drb1), 500) == nof_pending_bytes);

  return SRSRAN_SUCCESS;
}

int test_lc_ch_pbr_finite()
{
  srsenb::lch_ue_manager          lch_handler{0x46};
  sched_interface::dl_sched_pdu_t pdu;

  srsenb::sched_interface::ue_cfg_t ue_cfg                  = generate_default_ue_cfg();
  ue_cfg                                                    = generate_setup_ue_cfg(ue_cfg);
  ue_cfg.ue_bearers[srb_to_lcid((lte_srb::srb1))]           = {};
  ue_cfg.ue_bearers[srb_to_lcid((lte_srb::srb1))].direction = mac_lc_ch_cfg_t::BOTH;
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb1))]           = {};
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb1))].direction = mac_lc_ch_cfg_t::BOTH;
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb1))].pbr       = 256; // kBps
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb1))].bsd       = 50;  // msec
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb1))].priority  = 5;
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb2))]           = {};
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb2))].direction = mac_lc_ch_cfg_t::BOTH;
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb2))].pbr       = 8;  // kBps
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb2))].bsd       = 50; // msec
  ue_cfg.ue_bearers[drb_to_lcid((lte_drb::drb2))].priority  = 3;

  lch_handler.set_cfg(ue_cfg);
  for (uint32_t i = 0; i < 50; ++i) {
    lch_handler.new_tti();
  }
  // Bj={0, infinity, 0, 12800, 400}

  lch_handler.dl_buffer_state(srb_to_lcid(lte_srb::srb1), 50000, 1000);
  lch_handler.dl_buffer_state(drb_to_lcid(lte_drb::drb1), 50000, 1000);
  lch_handler.dl_buffer_state(drb_to_lcid(lte_drb::drb2), 50000, 0);

  // TEST1 - SRB1 retxs are emptied first
  int nof_pending_bytes = lch_handler.get_dl_prio_tx(srb_to_lcid(lte_srb::srb1));
  TESTASSERT(test_retx_until_empty(lch_handler, srb_to_lcid(lte_srb::srb1), 500) == nof_pending_bytes);

  // TEST2 - DRB1 retxs are emptied
  nof_pending_bytes = lch_handler.get_dl_prio_tx(drb_to_lcid(lte_drb::drb1));
  TESTASSERT(test_retx_until_empty(lch_handler, drb_to_lcid(lte_drb::drb1), 500) == nof_pending_bytes);

  // TEST3 - SRB1 newtxs are emptied (PBR==infinity)
  nof_pending_bytes = lch_handler.get_dl_tx(srb_to_lcid(lte_srb::srb1));
  TESTASSERT(test_newtx_until_empty(lch_handler, srb_to_lcid(lte_srb::srb1), 500) == nof_pending_bytes);

  // TEST4 - DRB2 has higher priority so it gets allocated until Bj <= 0
  TESTASSERT(test_pdu_alloc_successful(lch_handler, pdu, drb_to_lcid(lte_drb::drb2), 200) == SRSRAN_SUCCESS);
  // Bj={0, infinity, 0, 12800, 200}
  TESTASSERT(test_pdu_alloc_successful(lch_handler, pdu, drb_to_lcid(lte_drb::drb2), 600) == SRSRAN_SUCCESS);
  // Bj={0, infinity, 0, 256000, -400}

  // TEST5 - DRB1 has lower prio, but DRB2 Bj <= 0.
  for (uint32_t i = 0; i < 50; ++i) {
    lch_handler.new_tti();
    TESTASSERT(test_pdu_alloc_successful(lch_handler, pdu, drb_to_lcid(lte_drb::drb1), 50) == SRSRAN_SUCCESS);
  }

  // TEST6 - new tti restores DRB2 Bj>=0, and DRB2 gets allocated
  lch_handler.new_tti();
  // Bj={0, infinity, 0, 256000, 8}
  TESTASSERT(test_pdu_alloc_successful(lch_handler, pdu, drb_to_lcid(lte_drb::drb2), 50) == SRSRAN_SUCCESS);
  // Bj={0, infinity, 0, 256000, -42}
  lch_handler.new_tti();
  TESTASSERT(test_pdu_alloc_successful(lch_handler, pdu, drb_to_lcid(lte_drb::drb1), 50) == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}

int main()
{
  srsenb::set_randseed(seed);
  srsran::console("This is the chosen seed: %u\n", seed);

  auto& test_log = srslog::fetch_basic_logger("TEST", false);
  test_log.set_level(srslog::basic_levels::info);

  // Start the log backend.
  srslog::init();

  TESTASSERT(test_lc_ch_pbr_infinity() == SRSRAN_SUCCESS);
  TESTASSERT(test_lc_ch_pbr_finite() == SRSRAN_SUCCESS);

  srslog::flush();

  srsran::console("Success\n");
}
