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

int output_sched_tester::test_ul_rb_collisions(const tti_params_t&                    tti_params,
                                               const sched_interface::ul_sched_res_t& ul_result,
                                               prbmask_t&                             ul_allocs) const
{
  uint32_t nof_prb = params.cfg->cell.nof_prb;
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
  bool is_prach_tti_tx_ul = srslte_prach_tti_opportunity_config_fdd(params.cfg->prach_config, tti_params.tti_tx_ul, -1);
  if (is_prach_tti_tx_ul) {
    try_ul_fill({params.cfg->prach_freq_offset, 6}, "PRACH");
  }

  /* TEST: check collisions in PUCCH */
  bool strict = nof_prb != 6 or (not is_prach_tti_tx_ul); // and not tti_data.ul_pending_msg3_present);
  try_ul_fill({0, (uint32_t)params.cfg->nrb_pucch}, "PUCCH", strict);
  try_ul_fill({params.cfg->cell.nof_prb - params.cfg->nrb_pucch, (uint32_t)params.cfg->nrb_pucch}, "PUCCH", strict);

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

int output_sched_tester::test_dl_rb_collisions(const tti_params_t&                    tti_params,
                                               const sched_interface::dl_sched_res_t& dl_result,
                                               rbgmask_t&                             rbgmask) const
{
  srslte::bounded_bitset<100, true> dl_allocs(params.cfg->cell.nof_prb), alloc_mask(params.cfg->cell.nof_prb);

  auto try_dl_mask_fill = [&](const srslte_dci_dl_t& dci, const char* channel) {
    if (extract_dl_prbmask(params.cfg->cell, dci, &alloc_mask) != SRSLTE_SUCCESS) {
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
  if (params.cfg->cell.nof_prb == 6) {
    uint32_t tti_rx_ack = TTI_RX_ACK(tti_params.tti_rx);
    if (srslte_prach_tti_opportunity_config_fdd(params.cfg->prach_config, tti_rx_ack, -1)) {
      dl_allocs.fill(0, dl_allocs.size());
    }
  }

  // Decode Data allocations, check collisions and fill cumulative mask
  for (uint32_t i = 0; i < dl_result.nof_data_elems; ++i) {
    TESTASSERT(try_dl_mask_fill(dl_result.data[i].dci, "data") == SRSLTE_SUCCESS);
  }

  // TEST: check for holes in the PRB mask (RBGs not fully filled)
  rbgmask.resize(params.nof_rbgs);
  rbgmask.reset();
  srslte::bounded_bitset<100, true> rev_alloc = ~dl_allocs;
  for (uint32_t i = 0; i < params.nof_rbgs; ++i) {
    uint32_t lim = SRSLTE_MIN((i + 1) * params.P, dl_allocs.size());
    bool     val = dl_allocs.any(i * params.P, lim);
    CONDERROR(rev_alloc.any(i * params.P, lim) and val, "[TESTER] No holes can be left in an RBG\n");
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

  /* Test if any SIB was scheduled outside of its window */
  for (bc_elem* bc = bc_begin; bc != bc_end; ++bc) {
    if (bc->index == 0) {
      continue;
    }
    uint32_t x         = (bc->index - 1) * params.cfg->si_window_ms;
    uint32_t sf        = x % 10;
    uint32_t sfn_start = sfn;
    while ((sfn_start % params.cfg->sibs[bc->index].period_rf) != x / 10) {
      sfn_start--;
    }
    uint32_t win_start = sfn_start * 10 + sf;
    uint32_t win_end   = win_start + params.cfg->si_window_ms;
    CONDERROR(tti_params.tti_tx_dl < win_start or tti_params.tti_tx_dl > win_end,
              "Scheduled SIB is outside of its SIB window\n");
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
