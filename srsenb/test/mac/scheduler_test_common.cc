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
#include "srsenb/hdr/stack/mac/scheduler.h"
#include "srslte/mac/pdu.h"

#include "srslte/common/test_common.h"

#include <set>

using namespace srsenb;

/***************************
 *     Random Utils
 **************************/

std::default_random_engine rand_gen;

float ::srsenb::randf()
{
  static std::uniform_real_distribution<float> unif_dist(0, 1.0);
  return unif_dist(rand_gen);
}

void ::srsenb::set_randseed(uint64_t seed)
{
  rand_gen = std::default_random_engine(seed);
}

std::default_random_engine& ::srsenb::get_rand_gen()
{
  return rand_gen;
}

/***************************
 *     Sched Testers
 **************************/

int output_sched_tester::test_pusch_collisions(const tti_params_t&                    tti_params,
                                               const sched_interface::ul_sched_res_t& ul_result,
                                               prbmask_t&                             ul_allocs) const
{
  uint32_t nof_prb = cell_params.nof_prb();
  ul_allocs.resize(nof_prb);
  ul_allocs.reset();

  auto try_ul_fill = [&](srsenb::ul_harq_proc::ul_alloc_t alloc, const char* ch_str, bool strict = true) {
    CONDERROR((alloc.RB_start + alloc.L) > nof_prb,
              "Allocated RBs (%d,%d) out-of-bounds\n",
              alloc.RB_start,
              alloc.RB_start + alloc.L);
    CONDERROR(alloc.L == 0, "Allocations must have at least one PRB\n");
    if (strict and ul_allocs.any(alloc.RB_start, alloc.RB_start + alloc.L)) {
      TESTERROR("Collision Detected of %s alloc=(%d,%d) and cumulative_mask=0x%s\n",
                ch_str,
                alloc.RB_start,
                alloc.RB_start + alloc.L,
                ul_allocs.to_hex().c_str());
    }
    ul_allocs.fill(alloc.RB_start, alloc.RB_start + alloc.L, true);
    return SRSLTE_SUCCESS;
  };

  /* TEST: Check if there is space for PRACH */
  bool is_prach_tti_tx_ul =
      srslte_prach_tti_opportunity_config_fdd(cell_params.cfg.prach_config, tti_params.tti_tx_ul, -1);
  if (is_prach_tti_tx_ul) {
    try_ul_fill({cell_params.cfg.prach_freq_offset, 6}, "PRACH");
  }

  /* TEST: check collisions in PUCCH */
  bool strict = nof_prb != 6 or (not is_prach_tti_tx_ul); // and not tti_data.ul_pending_msg3_present);
  try_ul_fill({0, (uint32_t)cell_params.cfg.nrb_pucch}, "PUCCH", strict);
  try_ul_fill(
      {cell_params.cfg.cell.nof_prb - cell_params.cfg.nrb_pucch, (uint32_t)cell_params.cfg.nrb_pucch}, "PUCCH", strict);

  /* TEST: check collisions in the UL PUSCH */
  for (uint32_t i = 0; i < ul_result.nof_dci_elems; ++i) {
    uint32_t L, RBstart;
    srslte_ra_type2_from_riv(ul_result.pusch[i].dci.type2_alloc.riv, &L, &RBstart, nof_prb, nof_prb);
    strict = ul_result.pusch[i].needs_pdcch or nof_prb != 6; // Msg3 may collide with PUCCH at PRB==6
    try_ul_fill({RBstart, L}, "PUSCH", strict);
    //    ue_stats[ul_result.pusch[i].dci.rnti].nof_ul_rbs += L;
  }

  return SRSLTE_SUCCESS;
}

int output_sched_tester::test_pdsch_collisions(const tti_params_t&                    tti_params,
                                               const sched_interface::dl_sched_res_t& dl_result,
                                               rbgmask_t&                             rbgmask) const
{
  srslte::bounded_bitset<100, true> dl_allocs(cell_params.cfg.cell.nof_prb), alloc_mask(cell_params.cfg.cell.nof_prb);

  auto try_dl_mask_fill = [&](const srslte_dci_dl_t& dci, const char* channel) {
    if (extract_dl_prbmask(cell_params.cfg.cell, dci, &alloc_mask) != SRSLTE_SUCCESS) {
      return SRSLTE_ERROR;
    }
    if ((dl_allocs & alloc_mask).any()) {
      TESTERROR("Detected collision in the DL %s allocation (%s intersects %s)\n",
                channel,
                dl_allocs.to_string().c_str(),
                alloc_mask.to_string().c_str());
    }
    dl_allocs |= alloc_mask;
    return SRSLTE_SUCCESS;
  };

  // Decode BC allocations, check collisions, and fill cumulative mask
  for (uint32_t i = 0; i < dl_result.nof_bc_elems; ++i) {
    TESTASSERT(try_dl_mask_fill(dl_result.bc[i].dci, "BC") == SRSLTE_SUCCESS);
  }

  // Decode RAR allocations, check collisions, and fill cumulative mask
  for (uint32_t i = 0; i < dl_result.nof_rar_elems; ++i) {
    TESTASSERT(try_dl_mask_fill(dl_result.rar[i].dci, "RAR") == SRSLTE_SUCCESS);
  }

  // forbid Data in DL if it conflicts with PRACH for PRB==6
  if (cell_params.cfg.cell.nof_prb == 6) {
    uint32_t tti_rx_ack = tti_params.tti_rx_ack_dl();
    if (srslte_prach_tti_opportunity_config_fdd(cell_params.cfg.prach_config, tti_rx_ack, -1)) {
      dl_allocs.fill(0, dl_allocs.size());
    }
  }

  // Decode Data allocations, check collisions and fill cumulative mask
  for (uint32_t i = 0; i < dl_result.nof_data_elems; ++i) {
    TESTASSERT(try_dl_mask_fill(dl_result.data[i].dci, "data") == SRSLTE_SUCCESS);
  }

  // TEST: check for holes in the PRB mask (RBGs not fully filled)
  rbgmask.resize(cell_params.nof_rbgs);
  rbgmask.reset();
  srslte::bounded_bitset<100, true> rev_alloc = ~dl_allocs;
  for (uint32_t i = 0; i < cell_params.nof_rbgs; ++i) {
    uint32_t lim = SRSLTE_MIN((i + 1) * cell_params.P, dl_allocs.size());
    bool     val = dl_allocs.any(i * cell_params.P, lim);
    CONDERROR(rev_alloc.any(i * cell_params.P, lim) and val, "No holes can be left in an RBG\n");
    if (val) {
      rbgmask.set(i);
    }
  }

  return SRSLTE_SUCCESS;
}

int output_sched_tester::test_sib_scheduling(const tti_params_t&                    tti_params,
                                             const sched_interface::dl_sched_res_t& dl_result) const
{
  uint32_t sfn          = tti_params.sfn_tx_dl;
  uint32_t sf_idx       = tti_params.sf_idx_tx_dl;
  bool     sib1_present = ((sfn % 2) == 0) and sf_idx == 5;

  using bc_elem     = const sched_interface::dl_sched_bc_t;
  bc_elem* bc_begin = &dl_result.bc[0];
  bc_elem* bc_end   = &dl_result.bc[dl_result.nof_bc_elems];

  /* Test if SIB1 was correctly scheduled */
  if (sib1_present) {
    auto it = std::find_if(bc_begin, bc_end, [](bc_elem& elem) { return elem.index == 0; });
    CONDERROR(it == bc_end, "Failed to allocate SIB1 in even sfn, sf_idx==5\n");
  }

  /* Test if any SIB was scheduled with wrong index, tbs, or outside of its window */
  for (bc_elem* bc = bc_begin; bc != bc_end; ++bc) {
    if (bc->index == 0) {
      continue;
    }
    CONDERROR(bc->index >= sched_interface::MAX_SIBS, "Invalid SIB idx=%d\n", bc->index + 1);
    CONDERROR(bc->tbs < cell_params.cfg.sibs[bc->index].len,
              "Allocated BC process with TBS=%d < sib_len=%d\n",
              bc->tbs,
              cell_params.cfg.sibs[bc->index].len);
    uint32_t x         = (bc->index - 1) * cell_params.cfg.si_window_ms;
    uint32_t sf        = x % 10;
    uint32_t sfn_start = sfn;
    while ((sfn_start % cell_params.cfg.sibs[bc->index].period_rf) != x / 10) {
      sfn_start--;
    }
    uint32_t win_start = sfn_start * 10 + sf;
    uint32_t win_end   = win_start + cell_params.cfg.si_window_ms;
    CONDERROR(tti_params.tti_tx_dl < win_start or tti_params.tti_tx_dl > win_end,
              "Scheduled SIB is outside of its SIB window\n");
  }
  return SRSLTE_SUCCESS;
}

int output_sched_tester::test_pdcch_collisions(const sched_interface::dl_sched_res_t& dl_result,
                                               const sched_interface::ul_sched_res_t& ul_result,
                                               srslte::bounded_bitset<128, true>*     used_cce) const
{
  used_cce->resize(srslte_regs_pdcch_ncce(cell_params.regs.get(), dl_result.cfi));
  used_cce->reset();

  // Helper Function: checks if there is any collision. If not, fills the PDCCH mask
  auto try_cce_fill = [&](const srslte_dci_location_t& dci_loc, const char* ch) {
    uint32_t cce_start = dci_loc.ncce, cce_stop = dci_loc.ncce + (1u << dci_loc.L);
    if (used_cce->any(cce_start, cce_stop)) {
      TESTERROR("%s DCI collision between CCE positions (%u, %u)\n", ch, cce_start, cce_stop);
    }
    used_cce->fill(cce_start, cce_stop);
    return SRSLTE_SUCCESS;
  };

  /* TEST: verify there are no dci collisions for UL, DL data, BC, RAR */
  for (uint32_t i = 0; i < ul_result.nof_dci_elems; ++i) {
    const auto& pusch = ul_result.pusch[i];
    if (not pusch.needs_pdcch) {
      // In case of non-adaptive retx or Msg3
      continue;
    }
    try_cce_fill(pusch.dci.location, "UL");
  }
  for (uint32_t i = 0; i < dl_result.nof_data_elems; ++i) {
    try_cce_fill(dl_result.data[i].dci.location, "DL data");
  }
  for (uint32_t i = 0; i < dl_result.nof_bc_elems; ++i) {
    try_cce_fill(dl_result.bc[i].dci.location, "DL BC");
  }
  for (uint32_t i = 0; i < dl_result.nof_rar_elems; ++i) {
    try_cce_fill(dl_result.rar[i].dci.location, "DL RAR");
  }

  return SRSLTE_SUCCESS;
}

int output_sched_tester::test_dci_values_consistency(const sched_interface::dl_sched_res_t& dl_result,
                                                     const sched_interface::ul_sched_res_t& ul_result) const
{
  for (uint32_t i = 0; i < ul_result.nof_dci_elems; ++i) {
    const auto& pusch = ul_result.pusch[i];
    CONDERROR(pusch.tbs == 0, "Allocated RAR process with invalid TBS=%d\n", pusch.tbs);
    //    CONDERROR(ue_db.count(pusch.dci.rnti) == 0, "The allocated rnti=0x%x does not exist\n", pusch.dci.rnti);
    if (not pusch.needs_pdcch) {
      // In case of non-adaptive retx or Msg3
      continue;
    }
    CONDERROR(pusch.dci.location.L == 0,
              "Invalid aggregation level %d\n",
              pusch.dci.location.L); // TODO: Extend this test
  }
  for (uint32_t i = 0; i < dl_result.nof_data_elems; ++i) {
    auto& data = dl_result.data[i];
    CONDERROR(data.tbs[0] == 0, "Allocated DL data has empty TBS\n");
  }
  for (uint32_t i = 0; i < dl_result.nof_bc_elems; ++i) {
    auto& bc = dl_result.bc[i];
    if (bc.type == sched_interface::dl_sched_bc_t::BCCH) {
      CONDERROR(bc.tbs < cell_params.cfg.sibs[bc.index].len,
                "Allocated BC process with TBS=%d < sib_len=%d\n",
                bc.tbs,
                cell_params.cfg.sibs[bc.index].len);
    } else if (bc.type == sched_interface::dl_sched_bc_t::PCCH) {
      CONDERROR(bc.tbs == 0, "Allocated paging process with invalid TBS=%d\n", bc.tbs);
    } else {
      TESTERROR("Invalid broadcast process id=%d\n", (int)bc.type);
    }
  }
  for (uint32_t i = 0; i < dl_result.nof_rar_elems; ++i) {
    const auto& rar = dl_result.rar[i];
    CONDERROR(rar.tbs == 0, "Allocated RAR process with invalid TBS=%d\n", rar.tbs);
  }

  return SRSLTE_SUCCESS;
}

int output_sched_tester::test_all(const tti_params_t&                    tti_params,
                                  const sched_interface::dl_sched_res_t& dl_result,
                                  const sched_interface::ul_sched_res_t& ul_result) const
{
  prbmask_t ul_allocs;
  TESTASSERT(test_pusch_collisions(tti_params, ul_result, ul_allocs) == SRSLTE_SUCCESS);
  rbgmask_t dl_mask;
  TESTASSERT(test_pdsch_collisions(tti_params, dl_result, dl_mask) == SRSLTE_SUCCESS);
  TESTASSERT(test_sib_scheduling(tti_params, dl_result) == SRSLTE_SUCCESS);
  srslte::bounded_bitset<128, true> used_cce;
  TESTASSERT(test_pdcch_collisions(dl_result, ul_result, &used_cce) == SRSLTE_SUCCESS);
  return SRSLTE_SUCCESS;
}

int srsenb::extract_dl_prbmask(const srslte_cell_t&               cell,
                               const srslte_dci_dl_t&             dci,
                               srslte::bounded_bitset<100, true>* alloc_mask)
{
  srslte_pdsch_grant_t grant;
  srslte_dl_sf_cfg_t   dl_sf = {};

  alloc_mask->resize(cell.nof_prb);
  alloc_mask->reset();

  CONDERROR(srslte_ra_dl_dci_to_grant(&cell, &dl_sf, SRSLTE_TM1, false, &dci, &grant) == SRSLTE_ERROR,
            "Failed to decode PDSCH grant\n");
  for (uint32_t j = 0; j < alloc_mask->size(); ++j) {
    if (grant.prb_idx[0][j]) {
      alloc_mask->set(j);
    }
  }
  return SRSLTE_SUCCESS;
}

void user_state_sched_tester::new_tti(uint32_t tti_rx)
{
  tic++;
}

int user_state_sched_tester::add_user(uint16_t                                 rnti,
                                      uint32_t                                 preamble_idx,
                                      const srsenb::sched_interface::ue_cfg_t& ue_cfg)
{
  CONDERROR(!srslte_prach_tti_opportunity_config_fdd(
                cell_params[ue_cfg.supported_cc_list[0].enb_cc_idx].prach_config, tic.tti_rx(), -1),
            "New user added in a non-PRACH TTI\n");
  TESTASSERT(users.count(rnti) == 0);
  ue_state ue;
  ue.user_cfg     = ue_cfg;
  ue.prach_tic    = tic;
  ue.preamble_idx = preamble_idx;
  users.insert(std::make_pair(rnti, ue));
  return SRSLTE_SUCCESS;
}

int user_state_sched_tester::user_reconf(uint16_t rnti, const srsenb::sched_interface::ue_cfg_t& ue_cfg)
{
  TESTASSERT(users.count(rnti) > 0);
  users[rnti].user_cfg = ue_cfg;
  return SRSLTE_SUCCESS;
}

int user_state_sched_tester::bearer_cfg(uint16_t                                        rnti,
                                        uint32_t                                        lcid,
                                        const srsenb::sched_interface::ue_bearer_cfg_t& bearer_cfg)
{
  auto it = users.find(rnti);
  TESTASSERT(it != users.end());
  it->second.user_cfg.ue_bearers[lcid] = bearer_cfg;
  users[rnti].drb_cfg_flag             = false;
  for (uint32_t i = 2; i < it->second.user_cfg.ue_bearers.size(); ++i) {
    if (it->second.user_cfg.ue_bearers[i].direction != sched_interface::ue_bearer_cfg_t::IDLE) {
      users[rnti].drb_cfg_flag = true;
    }
  }
  return SRSLTE_SUCCESS;
}

void user_state_sched_tester::rem_user(uint16_t rnti)
{
  users.erase(rnti);
}

/**
 * Tests whether the RAR and Msg3 were scheduled within the expected windows. Individual tests:
 * - No UL allocs before Msg3
 * - No DL data allocs before Msg3 is correctly ACKed
 * - RAR alloc falls within RAR window and is unique per user
 * - Msg3 is allocated in expected TTI, without PDCCH, and correct rnti
 * - First Data allocation happens after Msg3, and contains a ConRes
 * - No RARs are allocated with wrong enb_cc_idx, preamble_idx or wrong user
 * TODO:
 * - check Msg3 PRBs match the ones advertised in the RAR
 */
int user_state_sched_tester::test_ra(uint32_t                               enb_cc_idx,
                                     const sched_interface::dl_sched_res_t& dl_result,
                                     const sched_interface::ul_sched_res_t& ul_result)
{
  uint32_t msg3_count = 0;

  for (auto& iter : users) {
    uint16_t  rnti     = iter.first;
    ue_state& userinfo = iter.second;

    uint32_t primary_cc_idx = userinfo.user_cfg.supported_cc_list[0].enb_cc_idx;
    if (enb_cc_idx != primary_cc_idx) {
      // only check for RAR/Msg3 presence for a UE's PCell
      continue;
    }

    /* TEST: RAR allocation */
    std::array<tti_counter, 2> rar_window = {
        userinfo.prach_tic + 3, userinfo.prach_tic + 3 + (int)cell_params[primary_cc_idx].prach_rar_window};
    tti_counter tic_tx_dl        = tic.tic_tx_dl();
    tti_counter tic_tx_ul        = tic.tic_tx_ul();
    bool        is_in_rar_window = tic_tx_dl >= rar_window[0] and tic_tx_dl <= rar_window[1];

    if (not is_in_rar_window) {
      CONDERROR(not userinfo.rar_tic.is_valid() and tic_tx_dl > rar_window[1],
                "RAR not scheduled within the RAR Window\n");
      for (uint32_t i = 0; i < dl_result.nof_rar_elems; ++i) {
        CONDERROR(dl_result.rar[i].dci.rnti == rnti, "No RAR allocations allowed outside of user RAR window\n");
      }
    } else {
      // Inside RAR window
      for (uint32_t i = 0; i < dl_result.nof_rar_elems; ++i) {
        for (uint32_t j = 0; j < dl_result.rar[i].nof_grants; ++j) {
          auto& data = dl_result.rar[i].msg3_grant[j].data;
          if (data.prach_tti == (uint32_t)userinfo.prach_tic.tti_rx() and data.preamble_idx == userinfo.preamble_idx) {
            CONDERROR(userinfo.rar_tic.is_valid(), "There was more than one RAR for the same user\n");
            CONDERROR(rnti != data.temp_crnti, "RAR grant C-RNTI does not match the expected.\n");
            userinfo.msg3_riv = dl_result.rar[i].msg3_grant[j].grant.rba;
            userinfo.rar_tic  = tic_tx_dl;
          }
        }
      }
    }

    /* TEST: Check Msg3 */
    if (userinfo.rar_tic.is_valid() and not userinfo.msg3_tic.is_valid()) {
      // RAR scheduled, Msg3 not yet scheduled
      tti_counter expected_msg3_tti = userinfo.rar_tic + FDD_HARQ_DELAY_DL_MS + MSG3_DELAY_MS;
      CONDERROR(expected_msg3_tti < tic_tx_ul and not userinfo.msg3_tic.is_valid(), "No UL msg3 alloc was made\n");

      if (expected_msg3_tti == tic_tx_ul) {
        // Msg3 should exist
        for (uint32_t i = 0; i < ul_result.nof_dci_elems; ++i) {
          if (ul_result.pusch[i].dci.rnti == rnti) {
            CONDERROR(userinfo.msg3_tic.is_valid(), "Only one Msg3 allowed per user\n");
            CONDERROR(ul_result.pusch[i].needs_pdcch, "Msg3 allocations do not require PDCCH\n");
            CONDERROR(userinfo.msg3_riv != ul_result.pusch[i].dci.type2_alloc.riv,
                      "The Msg3 was not allocated in the expected PRBs.\n");
            userinfo.msg3_tic = tic_tx_ul;
            msg3_count++;
          }
        }
      }
    }

    /* TEST: Check Msg4 */
    if (userinfo.msg3_tic.is_valid() and not userinfo.msg4_tic.is_valid()) {
      // Msg3 scheduled, but Msg4 not yet scheduled
      for (uint32_t i = 0; i < dl_result.nof_data_elems; ++i) {
        if (dl_result.data[i].dci.rnti == rnti) {
          CONDERROR(tic < userinfo.msg3_tic, "Msg4 cannot be scheduled without Msg3 being tx\n");
          for (uint32_t j = 0; j < dl_result.data[i].nof_pdu_elems[0]; ++j) {
            if (dl_result.data[i].pdu[0][j].lcid == (uint32_t)srslte::dl_sch_lcid::CON_RES_ID) {
              // ConRes found
              CONDERROR(dl_result.data[i].dci.format != SRSLTE_DCI_FORMAT1, "ConRes must be format1\n");
              CONDERROR(userinfo.msg4_tic.is_valid(), "Duplicate ConRes CE for the same rnti\n");
              userinfo.msg4_tic = tic_tx_dl;
            }
          }
        }
      }
    }

    /* TEST: Txs out of place */
    if (not userinfo.msg4_tic.is_valid()) {
      // Msg4 not yet received by user
      for (uint32_t i = 0; i < dl_result.nof_data_elems; ++i) {
        CONDERROR(dl_result.data[i].dci.rnti == rnti, "No DL data allocs allowed before Msg4 is scheduled\n");
      }
      if (userinfo.msg3_tic.is_valid() and userinfo.msg3_tic != tic_tx_ul) {
        // Msg3 scheduled. No UL alloc allowed unless it is a newtx (the Msg3 itself)
        for (uint32_t i = 0; i < ul_result.nof_dci_elems; ++i) {
          // Needs PDCCH - filters out UL retxs
          bool msg3_retx = ((tic_tx_ul - userinfo.msg3_tic) % (FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS)) == 0;
          CONDERROR(ul_result.pusch[i].dci.rnti == rnti and not msg3_retx,
                    "No UL txs allowed except for Msg3 before user received Msg4\n");
        }
      } else if (not userinfo.msg3_tic.is_valid()) {
        // Not Msg3 sched TTI
        for (uint32_t i = 0; i < ul_result.nof_dci_elems; ++i) {
          CONDERROR(ul_result.pusch[i].dci.rnti == rnti, "No UL newtxs allowed before user received Msg4\n");
        }
      }
    }
  }

  for (uint32_t i = 0; i < ul_result.nof_dci_elems; ++i) {
    auto& pusch_alloc = ul_result.pusch[i];
    if (not pusch_alloc.needs_pdcch) {
      // can be adaptive retx or msg3
      auto& ue = users[pusch_alloc.dci.rnti];
      if (tic.tic_tx_ul() == ue.msg3_tic) {
        msg3_count--;
      }
    }
  }
  CONDERROR(msg3_count > 0, "There are pending msg3 that do not belong to any active UE\n");

  return SRSLTE_SUCCESS;
}

/**
 * Individual tests:
 * - All RARs belong to a user that just PRACHed
 * - All DL/UL data allocs have a valid RNTI
 */
int user_state_sched_tester::test_ctrl_info(uint32_t                               enb_cc_idx,
                                            const sched_interface::dl_sched_res_t& dl_result,
                                            const sched_interface::ul_sched_res_t& ul_result)
{
  /* TEST: Ensure there are no spurious RARs that do not belong to any user */
  for (uint32_t i = 0; i < dl_result.nof_rar_elems; ++i) {
    for (uint32_t j = 0; j < dl_result.rar[i].nof_grants; ++j) {
      uint32_t prach_tti    = dl_result.rar[i].msg3_grant[j].data.prach_tti;
      uint32_t preamble_idx = dl_result.rar[i].msg3_grant[j].data.preamble_idx;
      auto     it           = std::find_if(users.begin(), users.end(), [&](const std::pair<uint16_t, ue_state>& u) {
        return u.second.preamble_idx == preamble_idx and ((uint32_t)u.second.prach_tic.tti_rx() == prach_tti);
      });
      CONDERROR(it == users.end(), "There was a RAR allocation with no associated user");
      CONDERROR(it->second.user_cfg.supported_cc_list[0].enb_cc_idx != enb_cc_idx,
                "The allocated RAR is in the wrong cc\n");
    }
  }

  /* TEST: All DL allocs have a correct rnti */
  std::set<uint16_t> alloc_rntis;
  for (uint32_t i = 0; i < dl_result.nof_data_elems; ++i) {
    uint16_t rnti = dl_result.data[i].dci.rnti;
    CONDERROR(alloc_rntis.count(rnti) > 0, "The user rnti=0x%x got allocated multiple times in DL\n", rnti);
    CONDERROR(users.count(rnti) == 0, "The user rnti=0x%x allocated in DL does not exist\n", rnti);
    alloc_rntis.insert(rnti);
  }

  /* TEST: All UL allocs have a correct rnti */
  alloc_rntis.clear();
  for (uint32_t i = 0; i < ul_result.nof_dci_elems; ++i) {
    uint16_t rnti = ul_result.pusch[i].dci.rnti;
    CONDERROR(alloc_rntis.count(rnti) > 0, "The user rnti=0x%x got allocated multiple times in UL\n", rnti);
    CONDERROR(users.count(rnti) == 0, "The user rnti=0x%x allocated in UL does not exist\n", rnti);
    alloc_rntis.insert(rnti);
  }

  return SRSLTE_SUCCESS;
}

/**
 * Tests whether the SCells are correctly activated. Individual tests:
 * - no DL and UL allocations in inactive carriers
 */
int user_state_sched_tester::test_scell_activation(uint32_t                               enb_cc_idx,
                                                   const sched_interface::dl_sched_res_t& dl_result,
                                                   const sched_interface::ul_sched_res_t& ul_result)
{
  for (auto& iter : users) {
    uint16_t  rnti     = iter.first;
    ue_state& userinfo = iter.second;

    auto it = std::find_if(userinfo.user_cfg.supported_cc_list.begin(),
                           userinfo.user_cfg.supported_cc_list.end(),
                           [enb_cc_idx](const sched::ue_cfg_t::cc_cfg_t& cc) { return cc.enb_cc_idx == enb_cc_idx; });

    if (it == userinfo.user_cfg.supported_cc_list.end() or not it->active) {
      // cell not active. Ensure data allocations are not made
      for (uint32_t i = 0; i < dl_result.nof_data_elems; ++i) {
        CONDERROR(dl_result.data[i].dci.rnti == rnti, "Allocated user in inactive carrier\n");
      }
      for (uint32_t i = 0; i < ul_result.nof_dci_elems; ++i) {
        CONDERROR(ul_result.pusch[i].needs_pdcch and ul_result.pusch[i].dci.rnti == rnti,
                  "Allocated user in inactive carrier\n");
      }
    } else {
      uint32_t ue_cc_idx = std::distance(userinfo.user_cfg.supported_cc_list.begin(), it);
      for (uint32_t i = 0; i < dl_result.nof_data_elems; ++i) {
        if (dl_result.data[i].dci.rnti == rnti) {
          CONDERROR(dl_result.data[i].dci.ue_cc_idx != ue_cc_idx, "User cell index was incorrectly set\n");
        }
      }
      for (uint32_t i = 0; i < ul_result.nof_dci_elems; ++i) {
        if (ul_result.pusch[i].needs_pdcch and ul_result.pusch[i].dci.rnti == rnti) {
          CONDERROR(ul_result.pusch[i].dci.ue_cc_idx != ue_cc_idx, "The user cell index was incorrectly set\n");
        }
      }
    }
  }

  return SRSLTE_SUCCESS;
}

int user_state_sched_tester::test_all(uint32_t                               enb_cc_idx,
                                      const sched_interface::dl_sched_res_t& dl_result,
                                      const sched_interface::ul_sched_res_t& ul_result)
{
  TESTASSERT(test_ra(enb_cc_idx, dl_result, ul_result) == SRSLTE_SUCCESS);
  TESTASSERT(test_ctrl_info(enb_cc_idx, dl_result, ul_result) == SRSLTE_SUCCESS);
  TESTASSERT(test_scell_activation(enb_cc_idx, dl_result, ul_result) == SRSLTE_SUCCESS);
  return SRSLTE_SUCCESS;
}

void sched_result_stats::process_results(const tti_params_t&                                 tti_params,
                                         const std::vector<sched_interface::dl_sched_res_t>& dl_result,
                                         const std::vector<sched_interface::ul_sched_res_t>& ul_result)
{
  for (uint32_t ccidx = 0; ccidx < dl_result.size(); ++ccidx) {
    for (uint32_t i = 0; i < dl_result[ccidx].nof_data_elems; ++i) {
      user_stats* user = get_user(dl_result[ccidx].data[i].dci.rnti);
      user->tot_dl_sched_data[ccidx] += dl_result[ccidx].data[i].tbs[0];
      user->tot_dl_sched_data[ccidx] += dl_result[ccidx].data[i].tbs[1];
    }
    for (uint32_t i = 0; i < ul_result[ccidx].nof_dci_elems; ++i) {
      user_stats* user = get_user(ul_result[ccidx].pusch[i].dci.rnti);
      user->tot_ul_sched_data[ccidx] += ul_result[ccidx].pusch[i].tbs;
    }
  }
}

sched_result_stats::user_stats* sched_result_stats::get_user(uint16_t rnti)
{
  if (users.count(rnti) != 0) {
    return &users[rnti];
  }
  users[rnti].rnti = rnti;
  users[rnti].tot_dl_sched_data.resize(cell_params.size(), 0);
  users[rnti].tot_ul_sched_data.resize(cell_params.size(), 0);
  return &users[rnti];
}

/***********************
 * Common Sched Tester
 **********************/

const sched::ue_cfg_t* common_sched_tester::get_current_ue_cfg(uint16_t rnti) const
{
  auto it = ue_db.find(rnti);
  if (it == ue_db.end()) {
    return nullptr;
  }
  return &it->second.get_ue_cfg();
}

int common_sched_tester::sim_cfg(sim_sched_args args)
{
  sim_args0 = std::move(args);

  sched::cell_cfg(sim_args0.cell_cfg); // call parent cfg

  ue_tester.reset(new user_state_sched_tester{sim_args0.cell_cfg});
  output_tester.clear();
  output_tester.reserve(sim_args0.cell_cfg.size());
  for (uint32_t i = 0; i < sim_args0.cell_cfg.size(); ++i) {
    output_tester.emplace_back(sched_cell_params[i]);
  }
  sched_stats.reset(new sched_result_stats{sim_args0.cell_cfg});

  tester_log = sim_args0.sim_log;

  return SRSLTE_SUCCESS;
}

int common_sched_tester::add_user(uint16_t rnti, const ue_cfg_t& ue_cfg_)
{
  CONDERROR(ue_cfg(rnti, ue_cfg_) != SRSLTE_SUCCESS, "Configuring new user rnti=0x%x to sched\n", rnti);
  //        CONDERROR(!srslte_prach_tti_opportunity_config_fdd(
  //            sched_cell_params[CARRIER_IDX].cfg.prach_config, tti_info.tti_params.tti_rx, -1),
  //                  "New user added in a non-PRACH TTI\n");

  dl_sched_rar_info_t rar_info = {};
  rar_info.prach_tti           = tti_info.tti_params.tti_rx;
  rar_info.temp_crnti          = rnti;
  rar_info.msg3_size           = 7;
  rar_info.preamble_idx        = tti_info.nof_prachs++;
  uint32_t pcell_idx           = ue_cfg_.supported_cc_list[0].enb_cc_idx;
  dl_rach_info(pcell_idx, rar_info);

  ue_tester->add_user(rnti, rar_info.preamble_idx, ue_cfg_);

  tester_log->info("Adding user rnti=0x%x\n", rnti);
  return SRSLTE_SUCCESS;
}

void common_sched_tester::rem_user(uint16_t rnti)
{
  tester_log->info("Removing user rnti=0x%x\n", rnti);
  sched::ue_rem(rnti);
  ue_tester->rem_user(rnti);
}

void common_sched_tester::new_test_tti()
{
  if (not tic.is_valid()) {
    tic.set_start_tti(sim_args0.start_tti);
  } else {
    tic++;
  }

  tti_info.tti_params = tti_params_t{tic.tti_rx()};
  tti_info.nof_prachs = 0;
  tti_info.dl_sched_result.clear();
  tti_info.ul_sched_result.clear();
  tti_info.dl_sched_result.resize(sched_cell_params.size());
  tti_info.ul_sched_result.resize(sched_cell_params.size());

  tester_log->step(tti_info.tti_params.tti_rx);

  ue_tester->new_tti(tti_info.tti_params.tti_rx);
}

int common_sched_tester::process_ack_txs()
{
  /* check if user was removed. If so, clean respective acks */
  erase_if(to_ack,
           [this](std::pair<const uint32_t, ack_info_t>& elem) { return this->ue_db.count(elem.second.rnti) == 0; });
  erase_if(to_ul_ack,
           [this](std::pair<const uint32_t, ul_ack_info_t>& elem) { return this->ue_db.count(elem.second.rnti) == 0; });

  /* Ack DL HARQs */
  for (const auto& ack_it : to_ack) {
    if (ack_it.second.tti != tti_info.tti_params.tti_rx) {
      continue;
    }
    const ack_info_t& dl_ack = ack_it.second;

    const srsenb::dl_harq_proc& h    = ue_db[dl_ack.rnti].get_dl_harq(ack_it.second.dl_harq.get_id(), dl_ack.ue_cc_idx);
    const srsenb::dl_harq_proc& hack = dl_ack.dl_harq;
    CONDERROR(hack.is_empty(), "The acked DL harq was not active\n");

    bool ret = false;
    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; ++tb) {
      if (dl_ack.dl_harq.is_empty(tb)) {
        continue;
      }
      ret |= dl_ack_info(tti_info.tti_params.tti_rx, dl_ack.rnti, dl_ack.enb_cc_idx, tb, dl_ack.ack) > 0;
    }
    CONDERROR(not ret, "The dl harq proc that was ACKed does not exist\n");

    if (dl_ack.ack) {
      CONDERROR(!h.is_empty(), "ACKed dl harq was not emptied\n");
      CONDERROR(h.has_pending_retx(0, tti_info.tti_params.tti_tx_dl), "ACKed dl harq still has pending retx\n");
      tester_log->info(
          "DL ACK tti=%u rnti=0x%x pid=%d\n", tti_info.tti_params.tti_rx, dl_ack.rnti, dl_ack.dl_harq.get_id());
    } else {
      tester_log->info(
          "DL NACK tti=%u rnti=0x%x pid=%d\n", tti_info.tti_params.tti_rx, dl_ack.rnti, dl_ack.dl_harq.get_id());
      CONDERROR(h.is_empty() and hack.nof_retx(0) + 1 < hack.max_nof_retx(), "NACKed DL harq got emptied\n");
    }
  }

  /* Ack UL HARQs */
  for (const auto& ack_it : to_ul_ack) {
    if (ack_it.first != tti_info.tti_params.tti_rx) {
      continue;
    }
    const ul_ack_info_t& ul_ack = ack_it.second;

    srsenb::ul_harq_proc*       h    = ue_db[ul_ack.rnti].get_ul_harq(tti_info.tti_params.tti_rx, ul_ack.ue_cc_idx);
    const srsenb::ul_harq_proc& hack = ul_ack.ul_harq;
    CONDERROR(h == nullptr or h->get_tti() != hack.get_tti(), "UL Harq TTI does not match the ACK TTI\n");
    CONDERROR(h->is_empty(0), "The acked UL harq is not active\n");
    CONDERROR(hack.is_empty(0), "The acked UL harq was not active\n");

    ul_crc_info(tti_info.tti_params.tti_rx, ul_ack.rnti, ul_ack.enb_cc_idx, ul_ack.ack);

    CONDERROR(!h->get_pending_data(), "UL harq lost its pending data\n");
    CONDERROR(!h->has_pending_ack(), "ACK/NACKed UL harq should have a pending ACK\n");

    if (ul_ack.ack) {
      CONDERROR(!h->is_empty(), "ACKed UL harq did not get emptied\n");
      CONDERROR(h->has_pending_retx(), "ACKed UL harq still has pending retx\n");
      tester_log->info("UL ACK tti=%u rnti=0x%x pid=%d\n", tti_info.tti_params.tti_rx, ul_ack.rnti, hack.get_id());
    } else {
      // NACK
      tester_log->info("UL NACK tti=%u rnti=0x%x pid=%d\n", tti_info.tti_params.tti_rx, ul_ack.rnti, hack.get_id());
      CONDERROR(!h->is_empty() and !h->has_pending_retx(), "If NACKed, UL harq has to have pending retx\n");
      CONDERROR(h->is_empty() and hack.nof_retx(0) + 1 < hack.max_nof_retx(), "Nacked UL harq did get emptied\n");
    }
  }

  // erase processed acks
  to_ack.erase(tti_info.tti_params.tti_rx);
  to_ul_ack.erase(tti_info.tti_params.tti_rx);

  return SRSLTE_SUCCESS;
}

int common_sched_tester::schedule_acks()
{
  for (uint32_t ccidx = 0; ccidx < sched_cell_params.size(); ++ccidx) {
    // schedule future acks
    for (uint32_t i = 0; i < tti_info.dl_sched_result[ccidx].nof_data_elems; ++i) {
      ack_info_t ack_data;
      ack_data.rnti       = tti_info.dl_sched_result[ccidx].data[i].dci.rnti;
      ack_data.tti        = FDD_HARQ_DELAY_DL_MS + tti_info.tti_params.tti_tx_dl;
      ack_data.enb_cc_idx = ccidx;
      ack_data.ue_cc_idx  = ue_db[ack_data.rnti].get_cell_index(ccidx).second;
      const srsenb::dl_harq_proc& dl_h =
          ue_db[ack_data.rnti].get_dl_harq(tti_info.dl_sched_result[ccidx].data[i].dci.pid, ack_data.ue_cc_idx);
      ack_data.dl_harq = dl_h;
      if (ack_data.dl_harq.nof_retx(0) == 0) {
        ack_data.ack = randf() > sim_args0.P_retx;
      } else { // always ack after three retxs
        ack_data.ack = ack_data.dl_harq.nof_retx(0) == 3;
      }

      // Remove harq from the ack list if there was a harq rewrite
      auto it = to_ack.begin();
      while (it != to_ack.end() and it->first < ack_data.tti) {
        if (it->second.rnti == ack_data.rnti and it->second.dl_harq.get_id() == ack_data.dl_harq.get_id() and
            it->second.ue_cc_idx == ack_data.ue_cc_idx) {
          CONDERROR(it->second.tti + FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS > ack_data.tti,
                    "The retx dl harq id=%d was transmitted too soon\n",
                    ack_data.dl_harq.get_id());
          auto toerase_it = it++;
          to_ack.erase(toerase_it);
          continue;
        }
        ++it;
      }
      // add new ack to the list
      to_ack.insert(std::make_pair(ack_data.tti, ack_data));
    }

    /* Schedule UL ACKs */
    for (uint32_t i = 0; i < tti_info.ul_sched_result[ccidx].nof_dci_elems; ++i) {
      const auto&   pusch = tti_info.ul_sched_result[ccidx].pusch[i];
      ul_ack_info_t ack_data;
      ack_data.rnti       = pusch.dci.rnti;
      ack_data.enb_cc_idx = ccidx;
      ack_data.ue_cc_idx  = ue_db[ack_data.rnti].get_cell_index(ccidx).second;
      ack_data.ul_harq    = *ue_db[ack_data.rnti].get_ul_harq(tti_info.tti_params.tti_tx_ul, ack_data.ue_cc_idx);
      ack_data.tti_tx_ul  = tti_info.tti_params.tti_tx_ul;
      ack_data.tti_ack    = tti_info.tti_params.tti_tx_ul + FDD_HARQ_DELAY_UL_MS;
      if (ack_data.ul_harq.nof_retx(0) == 0) {
        ack_data.ack = randf() > sim_args0.P_retx;
      } else {
        ack_data.ack = ack_data.ul_harq.nof_retx(0) == 2;
      }
      to_ul_ack.insert(std::make_pair(ack_data.tti_tx_ul, ack_data));
    }
  }
  return SRSLTE_SUCCESS;
}

int common_sched_tester::process_results()
{
  for (uint32_t i = 0; i < sched_cell_params.size(); ++i) {
    TESTASSERT(ue_tester->test_all(i, tti_info.dl_sched_result[i], tti_info.ul_sched_result[i]) == SRSLTE_SUCCESS);
    TESTASSERT(output_tester[i].test_all(
                   tti_info.tti_params, tti_info.dl_sched_result[i], tti_info.ul_sched_result[i]) == SRSLTE_SUCCESS);
  }
  sched_stats->process_results(tti_info.tti_params, tti_info.dl_sched_result, tti_info.ul_sched_result);

  return SRSLTE_SUCCESS;
}

int common_sched_tester::process_tti_events(const tti_ev& tti_ev)
{
  for (const tti_ev::user_cfg_ev& ue_ev : tti_ev.user_updates) {
    // There is a new configuration
    if (ue_ev.ue_cfg != nullptr) {
      if (not ue_tester->user_exists(ue_ev.rnti)) {
        // new user
        TESTASSERT(add_user(ue_ev.rnti, *ue_ev.ue_cfg) == SRSLTE_SUCCESS);
      } else {
        // reconfiguration
        TESTASSERT(ue_cfg(ue_ev.rnti, *ue_ev.ue_cfg) == SRSLTE_SUCCESS);
        ue_tester->user_reconf(ue_ev.rnti, *ue_ev.ue_cfg);
      }
    }

    // There is a user to remove
    if (ue_ev.rem_user) {
      rem_user(ue_ev.rnti);
    }

    // configure bearers
    if (ue_ev.bearer_cfg != nullptr) {
      CONDERROR(not ue_tester->user_exists(ue_ev.rnti), "User rnti=0x%x does not exist\n", ue_ev.rnti);
      // TODO: Instantiate more bearers
      bearer_ue_cfg(ue_ev.rnti, 0, ue_ev.bearer_cfg.get());
    }

    auto* user = ue_tester->get_user_state(ue_ev.rnti);

    if (user != nullptr and not user->msg4_tic.is_valid() and user->msg3_tic.is_valid() and user->msg3_tic <= tic) {
      // Msg3 has been received but Msg4 has not been yet transmitted
      uint32_t pending_dl_new_data = ue_db[ue_ev.rnti].get_pending_dl_new_data();
      if (pending_dl_new_data == 0) {
        uint32_t lcid = 0; // Use SRB0 to schedule Msg4
        dl_rlc_buffer_state(ue_ev.rnti, lcid, 50, 0);
        dl_mac_buffer_state(ue_ev.rnti, (uint32_t)srslte::dl_sch_lcid::CON_RES_ID);
      } else {
        // Let SRB0 Msg4 get fully transmitted
      }
    }

    // push UL SRs and DL packets
    if (ue_ev.buffer_ev != nullptr) {
      CONDERROR(user == nullptr, "TESTER ERROR: Trying to schedule data for user that does not exist\n");
      if (ue_ev.buffer_ev->dl_data > 0 and user->msg4_tic.is_valid()) {
        // If Msg4 has already been tx and there DL data to transmit
        uint32_t lcid                = 2;
        uint32_t pending_dl_new_data = ue_db[ue_ev.rnti].get_pending_dl_new_data();
        if (user->drb_cfg_flag or pending_dl_new_data == 0) {
          // If RRCSetup finished
          if (not user->drb_cfg_flag) {
            // setup lcid==2 bearer
            sched::ue_bearer_cfg_t cfg = {};
            cfg.direction              = ue_bearer_cfg_t::BOTH;
            ue_tester->bearer_cfg(ue_ev.rnti, 2, cfg);
            bearer_ue_cfg(ue_ev.rnti, 2, &cfg);
          }
          // DRB is set. Update DL buffer
          uint32_t tot_dl_data = pending_dl_new_data + ue_ev.buffer_ev->dl_data; // TODO: derive pending based on rx
          dl_rlc_buffer_state(ue_ev.rnti, lcid, tot_dl_data, 0);                 // TODO: Check retx_queue
        } else {
          // Let SRB0 get emptied
        }
      }

      if (ue_ev.buffer_ev->sr_data > 0 and user->drb_cfg_flag) {
        uint32_t tot_ul_data =
            ue_db[ue_ev.rnti].get_pending_ul_new_data(tti_info.tti_params.tti_tx_ul) + ue_ev.buffer_ev->sr_data;
        uint32_t lcid = 2;
        ul_bsr(ue_ev.rnti, lcid, tot_ul_data, true);
      }
    }
  }
  return SRSLTE_SUCCESS;
}

int common_sched_tester::run_tti(const tti_ev& tti_events)
{
  new_test_tti();
  tester_log->info("---- tti=%u | nof_ues=%zd ----\n", tic.tti_rx(), ue_db.size());

  process_tti_events(tti_events);
  process_ack_txs();
  before_sched();

  // Call scheduler for all carriers
  tti_info.dl_sched_result.resize(sched_cell_params.size());
  for (uint32_t i = 0; i < sched_cell_params.size(); ++i) {
    dl_sched(tti_info.tti_params.tti_tx_dl, i, tti_info.dl_sched_result[i]);
  }
  tti_info.ul_sched_result.resize(sched_cell_params.size());
  for (uint32_t i = 0; i < sched_cell_params.size(); ++i) {
    ul_sched(tti_info.tti_params.tti_tx_ul, i, tti_info.ul_sched_result[i]);
  }

  process_results();
  TESTASSERT(schedule_acks() == SRSLTE_SUCCESS);
  return SRSLTE_SUCCESS;
}

int common_sched_tester::test_next_ttis(const std::vector<tti_ev>& tti_events)
{
  uint32_t next_idx = tic.is_valid() ? tic.total_count() - sim_args0.start_tti + 1 : 0;

  for (; next_idx < tti_events.size(); ++next_idx) {
    TESTASSERT(run_tti(tti_events[next_idx]) == SRSLTE_SUCCESS);
  }
  return SRSLTE_SUCCESS;
}
