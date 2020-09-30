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
#include "srsenb/hdr/stack/upper/common_enb.h"
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

  auto try_ul_fill = [&](prb_interval alloc, const char* ch_str, bool strict = true) {
    CONDERROR(alloc.stop() > nof_prb, "Allocated RBs %s out-of-bounds\n", alloc.to_string().c_str());
    CONDERROR(alloc.empty(), "Allocations must have at least one PRB\n");
    if (strict and ul_allocs.any(alloc.start(), alloc.stop())) {
      TESTERROR("Collision Detected of %s alloc=%s and cumulative_mask=0x%s\n",
                ch_str,
                alloc.to_string().c_str(),
                ul_allocs.to_hex().c_str());
    }
    ul_allocs.fill(alloc.start(), alloc.stop(), true);
    return SRSLTE_SUCCESS;
  };

  /* TEST: Check if there is space for PRACH */
  bool is_prach_tti_tx_ul =
      srslte_prach_tti_opportunity_config_fdd(cell_params.cfg.prach_config, tti_params.tti_tx_ul, -1);
  if (is_prach_tti_tx_ul) {
    try_ul_fill({cell_params.cfg.prach_freq_offset, cell_params.cfg.prach_freq_offset + 6}, "PRACH");
  }

  /* TEST: check collisions in PUCCH */
  bool strict = nof_prb != 6 or (not is_prach_tti_tx_ul); // and not tti_data.ul_pending_msg3_present);
  try_ul_fill({0, (uint32_t)cell_params.cfg.nrb_pucch}, "PUCCH", strict);
  try_ul_fill({cell_params.cfg.cell.nof_prb - cell_params.cfg.nrb_pucch, (uint32_t)cell_params.cfg.cell.nof_prb},
              "PUCCH",
              strict);

  /* TEST: check collisions in the UL PUSCH */
  for (uint32_t i = 0; i < ul_result.nof_dci_elems; ++i) {
    uint32_t L, RBstart;
    srslte_ra_type2_from_riv(ul_result.pusch[i].dci.type2_alloc.riv, &L, &RBstart, nof_prb, nof_prb);
    strict = ul_result.pusch[i].needs_pdcch or nof_prb != 6; // Msg3 may collide with PUCCH at PRB==6
    try_ul_fill({RBstart, RBstart + L}, "PUSCH", strict);
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
    CONDERROR(pusch.tbs == 0, "Allocated PUSCH with invalid TBS=%d\n", pusch.tbs);
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

/***********************
 *  User State Tester
 ***********************/

ue_ctxt_test::ue_ctxt_test(uint16_t                                      rnti_,
                           uint32_t                                      preamble_idx_,
                           srslte::tti_point                             prach_tti_,
                           const ue_ctxt_test_cfg&                       cfg_,
                           const std::vector<srsenb::sched::cell_cfg_t>& cell_params_) :
  sim_cfg(cfg_),
  rnti(rnti_),
  prach_tti(prach_tti_),
  preamble_idx(preamble_idx_),
  cell_params(cell_params_),
  current_tti_rx(prach_tti_)
{
  set_cfg(cfg_.ue_cfg);
}

int ue_ctxt_test::set_cfg(const sched::ue_cfg_t& ue_cfg_)
{
  for (uint32_t ue_cc_idx = 0; ue_cc_idx < ue_cfg_.supported_cc_list.size(); ++ue_cc_idx) {
    const auto& cc = ue_cfg_.supported_cc_list[ue_cc_idx];
    if (ue_cc_idx >= active_ccs.size()) {
      // new cell
      active_ccs.emplace_back();
      active_ccs.back().ue_cc_idx  = active_ccs.size() - 1;
      active_ccs.back().enb_cc_idx = cc.enb_cc_idx;
      for (size_t i = 0; i < active_ccs.back().dl_harqs.size(); ++i) {
        active_ccs.back().dl_harqs[i].pid = i;
        active_ccs.back().ul_harqs[i].pid = i;
      }
    } else {
      CONDERROR(cc.enb_cc_idx != active_ccs[ue_cc_idx].enb_cc_idx, "changing ccs not supported\n");
    }
  }

  user_cfg = ue_cfg_;
  return SRSLTE_SUCCESS;
}

ue_ctxt_test::cc_ue_ctxt_test* ue_ctxt_test::get_cc_state(uint32_t enb_cc_idx)
{
  auto it = std::find_if(active_ccs.begin(), active_ccs.end(), [enb_cc_idx](const cc_ue_ctxt_test& c) {
    return c.enb_cc_idx == enb_cc_idx;
  });
  return it == active_ccs.end() ? nullptr : &(*it);
}

int ue_ctxt_test::new_tti(sched* sched_ptr, srslte::tti_point tti_rx)
{
  current_tti_rx = tti_rx;

  TESTASSERT(fwd_pending_acks(sched_ptr) == SRSLTE_SUCCESS);
  if (sim_cfg.periodic_cqi and (tti_rx.to_uint() % sim_cfg.cqi_Npd) == sim_cfg.cqi_Noffset) {
    for (auto& cc : active_ccs) {
      sched_ptr->dl_cqi_info(
          tti_rx.to_uint(), rnti, cc.enb_cc_idx, std::uniform_int_distribution<uint32_t>{5, 24}(get_rand_gen()));
      sched_ptr->ul_cqi_info(
          tti_rx.to_uint(), rnti, cc.enb_cc_idx, std::uniform_int_distribution<uint32_t>{5, 24}(get_rand_gen()), 0);
    }
  }

  return SRSLTE_SUCCESS;
}

int ue_ctxt_test::fwd_pending_acks(sched* sched_ptr)
{
  /* Ack DL HARQs */
  // Checks:
  // - Pending DL ACK {cc_idx,rnti,tb} exist in scheduler harqs
  // - Pending DL ACK tti_ack correspond to the expected based on tti_tx_dl
  while (not pending_dl_acks.empty()) {
    auto& p = pending_dl_acks.top();
    if (p.tti_ack > current_tti_rx) {
      break;
    }
    auto& h = active_ccs[p.ue_cc_idx].dl_harqs[p.pid];
    CONDERROR(not h.active, "The ACKed DL Harq pid=%d is not active\n", h.pid);
    CONDERROR(h.tti_tx + FDD_HARQ_DELAY_DL_MS != p.tti_ack, "dl ack hasn't arrived when expected\n");
    CONDERROR(sched_ptr->dl_ack_info(current_tti_rx.to_uint(), rnti, p.cc_idx, p.tb, p.ack) <= 0,
              "The ACKed DL Harq pid=%d does not exist.\n",
              p.pid);

    if (p.ack) {
      h.active = false;
      log_h->info("DL ACK tti=%u rnti=0x%x pid=%d\n", current_tti_rx.to_uint(), rnti, p.pid);
    }
    pending_dl_acks.pop();
  }

  /* Ack UL HARQs */
  while (not pending_ul_acks.empty()) {
    auto& p = pending_ul_acks.top();
    if (p.tti_ack > current_tti_rx) {
      break;
    }
    auto& h = active_ccs[p.ue_cc_idx].ul_harqs[p.pid];
    CONDERROR(not h.active, "The ACKed UL Harq pid=%d is not active\n", h.pid);
    CONDERROR(h.tti_tx != p.tti_ack, "UL CRC wasn't set when expected\n");
    CONDERROR(sched_ptr->ul_crc_info(current_tti_rx.to_uint(), rnti, p.cc_idx, p.ack) != SRSLTE_SUCCESS,
              "Failed UL ACK\n");

    if (p.ack) {
      h.active = false;
      log_h->info("UL ACK tti=%u rnti=0x%x pid=%d\n", current_tti_rx.to_uint(), rnti, p.pid);
    }
    pending_ul_acks.pop();
  }

  return SRSLTE_SUCCESS;
}

int ue_ctxt_test::test_sched_result(uint32_t                     enb_cc_idx,
                                    const sched::dl_sched_res_t& dl_result,
                                    const sched::ul_sched_res_t& ul_result)
{
  cc_result result{enb_cc_idx, &dl_result, &ul_result};
  TESTASSERT(test_harqs(result) == SRSLTE_SUCCESS);
  TESTASSERT(test_ra(result) == SRSLTE_SUCCESS);
  TESTASSERT(test_scell_activation(result) == SRSLTE_SUCCESS);
  TESTASSERT(schedule_acks(result) == SRSLTE_SUCCESS);
  return SRSLTE_SUCCESS;
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
int ue_ctxt_test::test_ra(cc_result result)
{
  if (result.enb_cc_idx != active_ccs[0].enb_cc_idx) {
    // only check for RAR/Msg3 presence for a UE's PCell
    return SRSLTE_SUCCESS;
  }

  /* TEST: RAR allocation */
  uint32_t                         rar_win_size     = cell_params[result.enb_cc_idx].prach_rar_window;
  std::array<srslte::tti_point, 2> rar_window       = {prach_tti + 3, prach_tti + 3 + rar_win_size};
  srslte::tti_point                tti_tx_dl        = srslte::to_tx_dl(current_tti_rx);
  srslte::tti_point                tti_tx_ul        = srslte::to_tx_ul(current_tti_rx);
  bool                             is_in_rar_window = tti_tx_dl >= rar_window[0] and tti_tx_dl <= rar_window[1];

  if (not is_in_rar_window) {
    CONDERROR(not rar_tti.is_valid() and tti_tx_dl > rar_window[1],
              "rnti=0x%x RAR not scheduled within the RAR Window\n",
              rnti);
    for (uint32_t i = 0; i < result.dl_result->nof_rar_elems; ++i) {
      CONDERROR(result.dl_result->rar[i].dci.rnti == rnti, "No RAR allocations allowed outside of user RAR window\n");
    }
  } else {
    // Inside RAR window
    for (uint32_t i = 0; i < result.dl_result->nof_rar_elems; ++i) {
      for (uint32_t j = 0; j < result.dl_result->rar[i].nof_grants; ++j) {
        const auto& data = result.dl_result->rar[i].msg3_grant[j].data;
        if (data.prach_tti == (uint32_t)prach_tti.to_uint() and data.preamble_idx == preamble_idx) {
          CONDERROR(rar_tti.is_valid(), "There was more than one RAR for the same user\n");
          CONDERROR(rnti != data.temp_crnti, "RAR grant C-RNTI does not match the expected.\n");
          msg3_riv = result.dl_result->rar[i].msg3_grant[j].grant.rba;
          rar_tti  = tti_tx_dl;
        }
      }
    }
  }

  /* TEST: Check Msg3 */
  if (rar_tti.is_valid() and not msg3_tti.is_valid()) {
    // RAR scheduled, Msg3 not yet scheduled
    srslte::tti_point expected_msg3_tti = rar_tti + FDD_HARQ_DELAY_DL_MS + MSG3_DELAY_MS;
    CONDERROR(expected_msg3_tti < tti_tx_ul and not msg3_tti.is_valid(), "No UL msg3 alloc was made\n");

    if (expected_msg3_tti == tti_tx_ul) {
      // Msg3 should exist
      for (uint32_t i = 0; i < result.ul_result->nof_dci_elems; ++i) {
        if (result.ul_result->pusch[i].dci.rnti == rnti) {
          CONDERROR(msg3_tti.is_valid(), "Only one Msg3 allowed per user\n");
          CONDERROR(result.ul_result->pusch[i].needs_pdcch, "Msg3 allocations do not require PDCCH\n");
          CONDERROR(msg3_riv != result.ul_result->pusch[i].dci.type2_alloc.riv,
                    "The Msg3 was not allocated in the expected PRBs.\n");
          msg3_tti = tti_tx_ul;
        }
      }
    }
  }

  /* TEST: Check Msg4 */
  if (msg3_tti.is_valid() and not msg4_tti.is_valid()) {
    // Msg3 scheduled, but Msg4 not yet scheduled
    for (uint32_t i = 0; i < result.dl_result->nof_data_elems; ++i) {
      if (result.dl_result->data[i].dci.rnti == rnti) {
        CONDERROR(current_tti_rx < msg3_tti, "Msg4 cannot be scheduled without Msg3 being tx\n");
        for (uint32_t j = 0; j < result.dl_result->data[i].nof_pdu_elems[0]; ++j) {
          if (result.dl_result->data[i].pdu[0][j].lcid == (uint32_t)srslte::dl_sch_lcid::CON_RES_ID) {
            // ConRes found
            CONDERROR(result.dl_result->data[i].dci.format != SRSLTE_DCI_FORMAT1, "ConRes must be format1\n");
            CONDERROR(msg4_tti.is_valid(), "Duplicate ConRes CE for the same rnti\n");
            msg4_tti = tti_tx_dl;
          }
        }
      }
    }
  }

  /* TEST: Txs out of place */
  if (not msg4_tti.is_valid()) {
    // Msg4 not yet received by user
    for (uint32_t i = 0; i < result.dl_result->nof_data_elems; ++i) {
      CONDERROR(result.dl_result->data[i].dci.rnti == rnti, "No DL data allocs allowed before Msg4 is scheduled\n");
    }
    if (msg3_tti.is_valid() and msg3_tti != tti_tx_ul) {
      // Msg3 scheduled. No UL alloc allowed unless it is a newtx (the Msg3 itself)
      for (uint32_t i = 0; i < result.ul_result->nof_dci_elems; ++i) {
        // Needs PDCCH - filters out UL retxs
        bool msg3_retx = ((tti_tx_ul - msg3_tti) % (FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS)) == 0;
        CONDERROR(result.ul_result->pusch[i].dci.rnti == rnti and not msg3_retx,
                  "No UL txs allowed except for Msg3 before user received Msg4\n");
      }
    } else if (not msg3_tti.is_valid()) {
      // No Msg3 sched TTI
      for (uint32_t i = 0; i < result.ul_result->nof_dci_elems; ++i) {
        CONDERROR(result.ul_result->pusch[i].dci.rnti == rnti, "No UL newtxs allowed before user received Msg4\n");
      }
    }
  }

  return SRSLTE_SUCCESS;
}

/**
 * Tests whether the SCells are correctly activated. Individual tests:
 * - no DL and UL allocations in inactive carriers
 */
int ue_ctxt_test::test_scell_activation(cc_result result)
{
  auto cc_it =
      std::find_if(user_cfg.supported_cc_list.begin(),
                   user_cfg.supported_cc_list.end(),
                   [&result](const sched::ue_cfg_t::cc_cfg_t& cc) { return cc.enb_cc_idx == result.enb_cc_idx; });

  if (cc_it == user_cfg.supported_cc_list.end() or not cc_it->active) {
    // cell not active. Ensure data allocations are not made
    for (uint32_t i = 0; i < result.dl_result->nof_data_elems; ++i) {
      CONDERROR(result.dl_result->data[i].dci.rnti == rnti, "Allocated user in inactive carrier\n");
    }
    for (uint32_t i = 0; i < result.ul_result->nof_dci_elems; ++i) {
      CONDERROR(result.ul_result->pusch[i].dci.rnti == rnti, "Allocated user in inactive carrier\n");
    }
  } else {
    uint32_t ue_cc_idx = std::distance(user_cfg.supported_cc_list.begin(), cc_it);
    for (uint32_t i = 0; i < result.dl_result->nof_data_elems; ++i) {
      if (result.dl_result->data[i].dci.rnti == rnti) {
        CONDERROR(result.dl_result->data[i].dci.ue_cc_idx != ue_cc_idx, "User cell index was incorrectly set\n");
      }
    }
    for (uint32_t i = 0; i < result.ul_result->nof_dci_elems; ++i) {
      if (result.ul_result->pusch[i].dci.rnti == rnti) {
        CONDERROR(result.ul_result->pusch[i].dci.ue_cc_idx != ue_cc_idx, "The user cell index was incorrectly set\n");
      }
    }
  }

  return SRSLTE_SUCCESS;
}

/**
 * Sanity checks of the DCI values in the scheduling result for a given user. Current checks:
 * - invalid ue_cc_idx<->enb_cc_idx matching in dl_result
 * - reusing same pid too early (ACK still didn't arrive yet)
 * - invalid rv value (nof retxs is incorrect) and ndi value
 */
int ue_ctxt_test::test_harqs(cc_result result)
{
  cc_ue_ctxt_test* cc = get_cc_state(result.enb_cc_idx);
  if (cc == nullptr) {
    // unsupported carrier
    return SRSLTE_SUCCESS;
  }

  /* Test DL Harqs */
  for (uint32_t i = 0; i < result.dl_result->nof_data_elems; ++i) {
    const auto& data = result.dl_result->data[i];
    if (data.dci.rnti != rnti) {
      continue;
    }

    CONDERROR(data.dci.ue_cc_idx != cc->ue_cc_idx, "invalid ue_cc_idx=%d in sched result\n", data.dci.ue_cc_idx);
    CONDERROR(data.dci.pid >= cc->dl_harqs.size(), "invalid pid\n");
    auto&    h        = cc->dl_harqs[data.dci.pid];
    uint32_t nof_retx = sched_utils::get_nof_retx(data.dci.tb[0].rv); // 0..3

    if (h.nof_txs == 0 or h.ndi != data.dci.tb[0].ndi) {
      // It is newtx
      CONDERROR(nof_retx != 0, "Invalid rv index for new tx\n");

      h.active    = true;
      h.nof_retxs = 0;
      h.ndi       = data.dci.tb[0].ndi;
      h.tti_tx    = srslte::to_tx_dl(current_tti_rx);
    } else {
      // it is retx
      CONDERROR(sched_utils::get_rvidx(h.nof_retxs + 1) != (uint32_t)data.dci.tb[0].rv, "Invalid rv index for retx\n");
      CONDERROR(h.ndi != data.dci.tb[0].ndi, "Invalid ndi for retx\n");
      CONDERROR(not h.active, "retx for inactive dl harq pid=%d\n", h.pid);
      CONDERROR(h.tti_tx > current_tti_rx, "harq pid=%d reused too soon\n", h.pid);
      CONDERROR(h.nof_retxs + 1 > sim_cfg.ue_cfg.maxharq_tx,
                "The number of retx=%d exceeded its max=%d\n",
                h.nof_retxs + 1,
                sim_cfg.ue_cfg.maxharq_tx);

      h.nof_retxs++;
      h.tti_tx = srslte::to_tx_dl(current_tti_rx);
    }
    h.nof_txs++;
  }

  /* Test UL Harqs */
  for (uint32_t i = 0; i < result.ul_result->nof_dci_elems; ++i) {
    const auto& pusch = result.ul_result->pusch[i];
    if (pusch.dci.rnti != rnti) {
      continue;
    }

    CONDERROR(pusch.dci.ue_cc_idx != cc->ue_cc_idx, "invalid ue_cc_idx=%d in sched result\n", pusch.dci.ue_cc_idx);
    auto&    h        = cc->ul_harqs[srslte::to_tx_ul(current_tti_rx).to_uint() % cc->ul_harqs.size()];
    uint32_t nof_retx = sched_utils::get_nof_retx(pusch.dci.tb.rv); // 0..3

    if (h.nof_txs == 0 or h.ndi != pusch.dci.tb.ndi) {
      // newtx
      CONDERROR(nof_retx != 0, "Invalid rv index for new tx\n");

      h.active    = true;
      h.nof_retxs = 0;
      h.ndi       = pusch.dci.tb.ndi;
    } else {
      if (pusch.needs_pdcch) {
        // adaptive retx
      } else {
        // non-adaptive retx
        CONDERROR(pusch.dci.type2_alloc.riv != h.riv, "Non-adaptive retx must keep the same riv\n");
      }
      if (pusch.tbs > 0) {
        CONDERROR(sched_utils::get_rvidx(h.nof_retxs + 1) != (uint32_t)pusch.dci.tb.rv, "Invalid rv index for retx\n");
      }
      CONDERROR(h.ndi != pusch.dci.tb.ndi, "Invalid ndi for retx\n");
      CONDERROR(not h.active, "retx for inactive UL harq pid=%d\n", h.pid);
      CONDERROR(h.tti_tx > current_tti_rx, "UL harq pid=%d was reused too soon\n", h.pid);

      h.nof_retxs++;
    }
    h.tti_tx = srslte::to_tx_ul(current_tti_rx);
    h.riv    = pusch.dci.type2_alloc.riv;
    h.nof_txs++;
  }

  return SRSLTE_SUCCESS;
}

int ue_ctxt_test::schedule_acks(cc_result result)
{
  auto* cc = get_cc_state(result.enb_cc_idx);
  if (cc == nullptr) {
    return SRSLTE_SUCCESS;
  }
  /* Schedule DL ACKs */
  for (uint32_t i = 0; i < result.dl_result->nof_data_elems; ++i) {
    const auto& data = result.dl_result->data[i];
    if (data.dci.rnti != rnti) {
      continue;
    }
    pending_ack_t ack_data;
    ack_data.tti_ack   = srslte::to_tx_dl_ack(current_tti_rx);
    ack_data.cc_idx    = result.enb_cc_idx;
    ack_data.tb        = 0;
    ack_data.pid       = data.dci.pid;
    ack_data.ue_cc_idx = data.dci.ue_cc_idx;
    uint32_t nof_retx  = sched_utils::get_nof_retx(data.dci.tb[0].rv); // 0..3
    ack_data.ack       = randf() < sim_cfg.prob_dl_ack_mask[nof_retx % sim_cfg.prob_dl_ack_mask.size()];

    pending_dl_acks.push(ack_data);
  }

  /* Schedule UL ACKs */
  for (uint32_t i = 0; i < result.ul_result->nof_dci_elems; ++i) {
    const auto& pusch = result.ul_result->pusch[i];
    if (pusch.dci.rnti != rnti) {
      continue;
    }

    pending_ack_t ack_data;
    ack_data.tti_ack   = srslte::to_tx_ul(current_tti_rx);
    ack_data.cc_idx    = result.enb_cc_idx;
    ack_data.ue_cc_idx = pusch.dci.ue_cc_idx;
    ack_data.tb        = 0;
    ack_data.pid       = srslte::to_tx_ul(current_tti_rx).to_uint() % cc->ul_harqs.size();
    uint32_t nof_retx  = sched_utils::get_nof_retx(pusch.dci.tb.rv); // 0..3
    ack_data.ack       = randf() < sim_cfg.prob_ul_ack_mask[nof_retx % sim_cfg.prob_ul_ack_mask.size()];

    pending_ul_acks.push(ack_data);
  }
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

void user_state_sched_tester::new_tti(sched* sched_ptr, uint32_t tti_rx)
{
  tic++;
  for (auto& u : users) {
    u.second.new_tti(sched_ptr, srslte::tti_point{tti_rx});
  }
}

int user_state_sched_tester::add_user(uint16_t rnti, uint32_t preamble_idx, const ue_ctxt_test_cfg& cfg_)
{
  CONDERROR(!srslte_prach_tti_opportunity_config_fdd(
                cell_params[cfg_.ue_cfg.supported_cc_list[0].enb_cc_idx].prach_config, tic.to_uint(), -1),
            "New user added in a non-PRACH TTI\n");
  TESTASSERT(users.count(rnti) == 0);
  ue_ctxt_test ue{rnti, preamble_idx, srslte::tti_point{tic.to_uint()}, cfg_, cell_params};
  users.insert(std::make_pair(rnti, ue));
  return SRSLTE_SUCCESS;
}

int user_state_sched_tester::user_reconf(uint16_t rnti, const srsenb::sched_interface::ue_cfg_t& ue_cfg)
{
  TESTASSERT(users.count(rnti) > 0);
  users.at(rnti).set_cfg(ue_cfg);
  return SRSLTE_SUCCESS;
}

int user_state_sched_tester::bearer_cfg(uint16_t                                        rnti,
                                        uint32_t                                        lcid,
                                        const srsenb::sched_interface::ue_bearer_cfg_t& bearer_cfg)
{
  auto it = users.find(rnti);
  TESTASSERT(it != users.end());
  it->second.user_cfg.ue_bearers[lcid] = bearer_cfg;
  users.at(rnti).drb_cfg_flag          = false;
  for (uint32_t i = 2; i < it->second.user_cfg.ue_bearers.size(); ++i) {
    if (it->second.user_cfg.ue_bearers[i].direction != sched_interface::ue_bearer_cfg_t::IDLE) {
      users.at(rnti).drb_cfg_flag = true;
    }
  }
  return SRSLTE_SUCCESS;
}

void user_state_sched_tester::rem_user(uint16_t rnti)
{
  users.erase(rnti);
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
      auto     it           = std::find_if(users.begin(), users.end(), [&](const std::pair<uint16_t, ue_ctxt_test>& u) {
        return u.second.preamble_idx == preamble_idx and ((uint32_t)u.second.prach_tti.to_uint() == prach_tti);
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

int user_state_sched_tester::test_all(uint32_t                               enb_cc_idx,
                                      const sched_interface::dl_sched_res_t& dl_result,
                                      const sched_interface::ul_sched_res_t& ul_result)
{
  TESTASSERT(test_ctrl_info(enb_cc_idx, dl_result, ul_result) == SRSLTE_SUCCESS);

  for (auto& u : users) {
    TESTASSERT(u.second.test_sched_result(enb_cc_idx, dl_result, ul_result) == SRSLTE_SUCCESS);
  }

  return SRSLTE_SUCCESS;
}

/***********************
 *  Sim Stats Storage
 **********************/

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
  return ue_tester->get_user_cfg(rnti);
}

int common_sched_tester::sim_cfg(sim_sched_args args)
{
  sim_args0 = std::move(args);

  sched::cell_cfg(sim_args0.cell_cfg); // call parent cfg
  sched::set_sched_cfg(&sim_args0.sched_args);

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

int common_sched_tester::add_user(uint16_t rnti, const ue_ctxt_test_cfg& ue_cfg_)
{
  CONDERROR(ue_cfg(rnti, generate_rach_ue_cfg(ue_cfg_.ue_cfg)) != SRSLTE_SUCCESS,
            "Configuring new user rnti=0x%x to sched\n",
            rnti);
  //        CONDERROR(!srslte_prach_tti_opportunity_config_fdd(
  //            sched_cell_params[CARRIER_IDX].cfg.prach_config, tti_info.tti_params.tti_rx, -1),
  //                  "New user added in a non-PRACH TTI\n");

  dl_sched_rar_info_t rar_info = {};
  rar_info.prach_tti           = tti_info.tti_params.tti_rx;
  rar_info.temp_crnti          = rnti;
  rar_info.msg3_size           = 7;
  rar_info.preamble_idx        = tti_info.nof_prachs++;
  uint32_t pcell_idx           = ue_cfg_.ue_cfg.supported_cc_list[0].enb_cc_idx;
  dl_rach_info(pcell_idx, rar_info);

  ue_tester->add_user(rnti, rar_info.preamble_idx, ue_cfg_);

  tester_log->info("Adding user rnti=0x%x\n", rnti);
  return SRSLTE_SUCCESS;
}

int common_sched_tester::reconf_user(uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg_)
{
  CONDERROR(not ue_tester->user_exists(rnti), "User must already exist to be configured\n");
  CONDERROR(ue_cfg(rnti, ue_cfg_) != SRSLTE_SUCCESS, "Configuring new user rnti=0x%x to sched\n", rnti);
  ue_tester->user_reconf(rnti, ue_cfg_);
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
    tic = srslte::tti_point{sim_args0.start_tti};
  } else {
    tic++;
  }

  tti_info.tti_params = tti_params_t{tic.to_uint()};
  tti_info.nof_prachs = 0;
  tti_info.dl_sched_result.clear();
  tti_info.ul_sched_result.clear();
  tti_info.dl_sched_result.resize(sched_cell_params.size());
  tti_info.ul_sched_result.resize(sched_cell_params.size());

  tester_log->step(tti_info.tti_params.tti_rx);
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
    if (ue_ev.ue_sim_cfg != nullptr) {
      if (not ue_tester->user_exists(ue_ev.rnti)) {
        // new user
        TESTASSERT(add_user(ue_ev.rnti, *ue_ev.ue_sim_cfg) == SRSLTE_SUCCESS);
      } else {
        // reconfiguration
        TESTASSERT(reconf_user(ue_ev.rnti, ue_ev.ue_sim_cfg->ue_cfg) == SRSLTE_SUCCESS);
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

    const ue_ctxt_test* user = ue_tester->get_user_ctxt(ue_ev.rnti);

    if (user != nullptr and not user->msg4_tti.is_valid() and user->msg3_tti.is_valid() and user->msg3_tti <= tic) {
      // Msg3 has been received but Msg4 has not been yet transmitted
      // Setup default UE config
      reconf_user(user->rnti, generate_setup_ue_cfg(sim_args0.default_ue_sim_cfg.ue_cfg));

      // Schedule RRC Setup and ConRes CE
      uint32_t pending_dl_new_data = ue_db[ue_ev.rnti].get_pending_dl_new_data();
      if (pending_dl_new_data == 0) {
        uint32_t lcid = RB_ID_SRB0; // Use SRB0 to schedule Msg4
        dl_rlc_buffer_state(ue_ev.rnti, lcid, 50, 0);
        dl_mac_buffer_state(ue_ev.rnti, (uint32_t)srslte::dl_sch_lcid::CON_RES_ID);
      } else {
        // Let SRB0 Msg4 get fully transmitted
      }
    }

    // push UL SRs and DL packets
    if (ue_ev.buffer_ev != nullptr) {
      CONDERROR(user == nullptr, "TESTER ERROR: Trying to schedule data for user that does not exist\n");
      if (ue_ev.buffer_ev->dl_data > 0 and user->msg4_tti.is_valid()) {
        // If Msg4 has already been tx and there DL data to transmit
        uint32_t lcid                = RB_ID_DRB1;
        uint32_t pending_dl_new_data = ue_db[ue_ev.rnti].get_pending_dl_new_data();
        if (user->drb_cfg_flag or pending_dl_new_data == 0) {
          // If RRCSetup finished
          if (not user->drb_cfg_flag) {
            reconf_user(user->rnti, sim_args0.default_ue_sim_cfg.ue_cfg);
            // setup lcid==drb1 bearer
            sched::ue_bearer_cfg_t cfg = {};
            cfg.direction              = ue_bearer_cfg_t::BOTH;
            cfg.group                  = 1;
            ue_tester->bearer_cfg(ue_ev.rnti, lcid, cfg);
            bearer_ue_cfg(ue_ev.rnti, lcid, &cfg);
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
            ue_db[ue_ev.rnti].get_pending_ul_new_data(tti_info.tti_params.tti_tx_ul, -1) + ue_ev.buffer_ev->sr_data;
        uint32_t lcg = 1;
        ul_bsr(ue_ev.rnti, lcg, tot_ul_data);
      }
    }
  }
  return SRSLTE_SUCCESS;
}

int common_sched_tester::run_tti(const tti_ev& tti_events)
{
  new_test_tti();
  tester_log->info("---- tti=%u | nof_ues=%zd ----\n", tic.to_uint(), ue_db.size());

  ue_tester->new_tti(this, tti_info.tti_params.tti_rx);
  process_tti_events(tti_events);
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
  tti_count++;
  return SRSLTE_SUCCESS;
}

int common_sched_tester::test_next_ttis(const std::vector<tti_ev>& tti_events)
{
  while (tti_count < tti_events.size()) {
    TESTASSERT(run_tti(tti_events[tti_count]) == SRSLTE_SUCCESS);
  }
  return SRSLTE_SUCCESS;
}
