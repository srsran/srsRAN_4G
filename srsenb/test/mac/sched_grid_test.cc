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
#include "srsenb/hdr/stack/mac/scheduler_grid.h"
#include "srslte/common/test_common.h"

using namespace srsenb;
const uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();

const uint32_t                PCell_IDX = 0;
const std::array<uint32_t, 6> prb_list  = {6, 15, 25, 50, 75, 100};

uint32_t get_aggr_level(sched_ue& sched_ue, uint32_t ue_cc_idx, const std::vector<sched_cell_params_t>& cell_params)
{
  srslte_dci_format_t dci_format = sched_ue.get_dci_format();
  uint32_t nof_dci_bits = srslte_dci_format_sizeof(&cell_params[ue_cc_idx].cfg.cell, nullptr, nullptr, dci_format);
  uint32_t aggr_level   = sched_ue.get_aggr_level(ue_cc_idx, nof_dci_bits);
  return aggr_level;
}

int test_pdcch_one_ue()
{
  using rand_uint           = std::uniform_int_distribution<uint32_t>;
  const uint32_t ENB_CC_IDX = 0;
  // Params
  uint32_t          nof_prb = prb_list[rand_uint{0, 5}(get_rand_gen())];
  uint16_t          rnti    = rand_uint{70, 120}(get_rand_gen());
  srslte::tti_point start_tti{rand_uint{0, 10240}(get_rand_gen())};
  uint32_t          nof_ttis = 100;

  // Derived
  std::vector<sched_cell_params_t> cell_params(1);
  sched_interface::ue_cfg_t        ue_cfg   = generate_default_ue_cfg();
  sched_interface::cell_cfg_t      cell_cfg = generate_default_cell_cfg(nof_prb);
  sched_interface::sched_args_t    sched_args{};
  TESTASSERT(cell_params[ENB_CC_IDX].set_cfg(ENB_CC_IDX, cell_cfg, sched_args));

  pdcch_grid_t pdcch;
  sched_ue     sched_ue{};
  sched_ue.init(rnti, cell_params);
  sched_ue.set_cfg(ue_cfg);

  pdcch.init(cell_params[PCell_IDX]);
  TESTASSERT(pdcch.nof_alloc_combinations() == 0);
  TESTASSERT(pdcch.nof_allocs() == 0);

  uint32_t tti_counter = 0;
  for (; tti_counter < nof_ttis; ++tti_counter) {
    tti_params_t tti_params{(start_tti + tti_counter).to_uint()};
    pdcch.new_tti(tti_params);
    TESTASSERT(pdcch.nof_cces() == cell_params[ENB_CC_IDX].nof_cce_table[0]);
    TESTASSERT(pdcch.get_cfi() == 1); // Start at CFI=1

    // Set DL CQI - it should affect aggregation level
    uint32_t dl_cqi = std::uniform_int_distribution<uint32_t>{1, 25}(srsenb::get_rand_gen());
    sched_ue.set_dl_cqi(tti_params.tti_tx_dl, ENB_CC_IDX, dl_cqi);
    uint32_t aggr_idx = get_aggr_level(sched_ue, PCell_IDX, cell_params);
    uint32_t max_nof_cce_locs =
        sched_ue.get_locations(ENB_CC_IDX, pdcch_grid_t::MAX_CFI, tti_params.sf_idx_tx_dl)->nof_loc[aggr_idx];

    // allocate DL user
    uint32_t                 prev_cfi          = pdcch.get_cfi();
    srsenb::sched_dci_cce_t* dci_cce           = sched_ue.get_locations(ENB_CC_IDX, prev_cfi, tti_params.sf_idx_tx_dl);
    uint32_t                 prev_nof_cce_locs = dci_cce->nof_loc[aggr_idx];

    TESTASSERT(pdcch.alloc_dci(alloc_type_t::DL_DATA, aggr_idx, &sched_ue));
    TESTASSERT(pdcch.nof_allocs() == 1);
    if (prev_nof_cce_locs == pdcch.nof_allocs() - 1) {
      // CFI must be increased
      TESTASSERT(pdcch.get_cfi() > prev_cfi);
    } else {
      // Previous CFI should be fine
      TESTASSERT(pdcch.get_cfi() == prev_cfi);
    }

    dci_cce                      = sched_ue.get_locations(ENB_CC_IDX, pdcch.get_cfi(), tti_params.sf_idx_tx_dl);
    uint32_t        nof_dci_locs = dci_cce->nof_loc[aggr_idx];
    const uint32_t* dci_locs     = dci_cce->cce_start[aggr_idx];

    // TEST: Check the first alloc of the pdcch result (e.g. rnti, valid cce mask, etc.)
    pdcch_grid_t::alloc_result_t pdcch_result;
    pdcch_mask_t                 pdcch_mask;
    pdcch.get_allocs(&pdcch_result, &pdcch_mask, 0);
    TESTASSERT(pdcch_result.size() == 1);
    TESTASSERT(pdcch_result[0]->rnti == sched_ue.get_rnti());
    TESTASSERT(pdcch_result[0]->total_mask.size() == cell_params[ENB_CC_IDX].nof_cce_table[pdcch.get_cfi() - 1]);
    TESTASSERT(pdcch_result[0]->current_mask == pdcch_result[0]->total_mask);
    TESTASSERT(pdcch_result[0]->current_mask.count() == 1u << aggr_idx);
    TESTASSERT(std::count(dci_locs, dci_locs + nof_dci_locs, pdcch_result[0]->dci_pos.ncce) > 0);

    // allocate UL user
    if (max_nof_cce_locs == pdcch.nof_allocs()) {
      // no more space
      continue;
    }
    prev_nof_cce_locs = nof_dci_locs;
    prev_cfi          = pdcch.get_cfi();
    TESTASSERT(pdcch.alloc_dci(alloc_type_t::UL_DATA, aggr_idx, &sched_ue));
    TESTASSERT(pdcch.nof_allocs() == 2);
    if (prev_nof_cce_locs == pdcch.nof_allocs() - 1) {
      // CFI must be increased
      TESTASSERT(pdcch.get_cfi() > prev_cfi);
    } else {
      // Previous CFI should be fine
      TESTASSERT(pdcch.get_cfi() == prev_cfi);
    }

    dci_cce      = sched_ue.get_locations(ENB_CC_IDX, pdcch.get_cfi(), tti_params.sf_idx_tx_dl);
    nof_dci_locs = dci_cce->nof_loc[aggr_idx];
    dci_locs     = dci_cce->cce_start[aggr_idx];

    pdcch.get_allocs(&pdcch_result, &pdcch_mask, 0);
    TESTASSERT(pdcch_result.size() == pdcch.nof_allocs());
    TESTASSERT(pdcch_result[1]->rnti == sched_ue.get_rnti());
    TESTASSERT(pdcch_result[1]->total_mask.size() == cell_params[ENB_CC_IDX].nof_cce_table[pdcch.get_cfi() - 1]);
    TESTASSERT((pdcch_result[1]->current_mask & pdcch_result[0]->current_mask).none());
    TESTASSERT(pdcch_result[1]->current_mask.count() == 1u << aggr_idx);
    TESTASSERT(pdcch_result[1]->total_mask == (pdcch_result[0]->current_mask | pdcch_result[1]->current_mask));
    TESTASSERT(std::count(dci_locs, dci_locs + nof_dci_locs, pdcch_result[0]->dci_pos.ncce) > 0);

    srslte::logmap::get("TEST")->info("PDCCH alloc result: %s\n", pdcch.result_to_string(true).c_str());
  }
  TESTASSERT(tti_counter == nof_ttis);

  return SRSLTE_SUCCESS;
}

int main()
{
  srsenb::set_randseed(seed);
  printf("This is the chosen seed: %u\n", seed);
  srslte::logmap::get("TEST")->set_level(srslte::LOG_LEVEL_INFO);

  TESTASSERT(test_pdcch_one_ue() == SRSLTE_SUCCESS);
  printf("Success\n");
}
