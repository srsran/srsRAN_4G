/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/common/test_common.h"

using namespace srsenb;

int output_sched_tester::test_pusch_collisions(const tti_params_t&                    tti_params,
                                               const sched_interface::ul_sched_res_t& ul_result,
                                               prbmask_t&                             ul_allocs) const
{
  uint32_t nof_prb = cell_params.nof_prb();
  ul_allocs.resize(nof_prb);
  ul_allocs.reset();

  auto try_ul_fill = [&](srsenb::ul_harq_proc::ul_alloc_t alloc, const char* ch_str, bool strict = true) {
    CONDERROR((alloc.RB_start + alloc.L) > nof_prb,
              "[TESTER] Allocated RBs (%d,%d) out-of-bounds\n",
              alloc.RB_start,
              alloc.RB_start + alloc.L);
    CONDERROR(alloc.L == 0, "[TESTER] Allocations must have at least one PRB\n");
    if (strict and ul_allocs.any(alloc.RB_start, alloc.RB_start + alloc.L)) {
      TESTERROR("[TESTER] Collision Detected of %s alloc=(%d,%d) and cumulative_mask=0x%s\n",
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
      TESTERROR("[TESTER] Detected collision in the DL %s allocation (%s intersects %s)\n",
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
    uint32_t tti_rx_ack = TTI_RX_ACK(tti_params.tti_rx);
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
    CONDERROR(rev_alloc.any(i * cell_params.P, lim) and val, "[TESTER] No holes can be left in an RBG\n");
    if (val) {
      rbgmask.set(i);
    }
  }

  return SRSLTE_SUCCESS;
}

int output_sched_tester::test_sib_scheduling(const tti_params_t&                    tti_params,
                                             const sched_interface::dl_sched_res_t& dl_result) const
{
  uint32_t sfn          = tti_params.sfn;
  uint32_t sf_idx       = tti_params.sf_idx;
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
      TESTERROR("[TESTER] %s DCI collision between CCE positions (%u, %u)\n", ch, cce_start, cce_stop);
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
              "[TESTER] Invalid aggregation level %d\n",
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

int srsenb::extract_dl_prbmask(const srslte_cell_t&               cell,
                               const srslte_dci_dl_t&             dci,
                               srslte::bounded_bitset<100, true>* alloc_mask)
{
  srslte_pdsch_grant_t grant;
  srslte_dl_sf_cfg_t   dl_sf    = {};
  srslte_dci_dl_t*     dci_dyn  = const_cast<srslte_dci_dl_t*>(&dci); // TODO
  srslte_cell_t*       cell_dyn = const_cast<srslte_cell_t*>(&cell);

  alloc_mask->resize(cell.nof_prb);
  alloc_mask->reset();

  CONDERROR(srslte_ra_dl_dci_to_grant(cell_dyn, &dl_sf, SRSLTE_TM1, false, dci_dyn, &grant) == SRSLTE_ERROR,
            "Failed to decode PDSCH grant\n");
  for (uint32_t j = 0; j < alloc_mask->size(); ++j) {
    if (grant.prb_idx[0][j]) {
      alloc_mask->set(j);
    }
  }
  return SRSLTE_SUCCESS;
}
