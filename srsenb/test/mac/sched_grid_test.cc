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
#include "srsenb/hdr/stack/mac/sched_grid.h"
#include "srsran/common/common_lte.h"
#include "srsran/common/test_common.h"

using namespace srsenb;
const uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();

const uint32_t PCell_IDX = 0;

uint32_t get_aggr_level(sched_ue& sched_ue, uint32_t enb_cc_idx, const std::vector<sched_cell_params_t>& cell_params)
{
  srsran_dci_format_t dci_format = sched_ue.get_dci_format();
  uint32_t nof_dci_bits = srsran_dci_format_sizeof(&cell_params[enb_cc_idx].cfg.cell, nullptr, nullptr, dci_format);
  uint32_t aggr_level   = sched_ue.get_aggr_level(enb_cc_idx, nof_dci_bits);
  return aggr_level;
}

int test_pdcch_one_ue()
{
  using rand_uint           = std::uniform_int_distribution<uint32_t>;
  const uint32_t ENB_CC_IDX = 0;
  // Params
  uint32_t          nof_prb = srsran::lte_cell_nof_prbs[rand_uint{0, 5}(get_rand_gen())];
  uint16_t          rnti    = rand_uint{70, 120}(get_rand_gen());
  srsran::tti_point start_tti{rand_uint{0, 10240}(get_rand_gen())};
  uint32_t          nof_ttis = 100;

  // Derived
  std::vector<sched_cell_params_t> cell_params(1);
  sched_interface::ue_cfg_t        ue_cfg   = generate_default_ue_cfg();
  sched_interface::cell_cfg_t      cell_cfg = generate_default_cell_cfg(nof_prb);
  sched_interface::sched_args_t    sched_args{};
  TESTASSERT(cell_params[ENB_CC_IDX].set_cfg(ENB_CC_IDX, cell_cfg, sched_args));

  sf_cch_allocator pdcch;
  sched_ue         sched_ue{rnti, cell_params, ue_cfg};

  pdcch.init(cell_params[PCell_IDX]);
  TESTASSERT(pdcch.nof_allocs() == 0);

  uint32_t tti_counter = 0;
  for (; tti_counter < nof_ttis; ++tti_counter) {
    tti_point tti_rx = start_tti + tti_counter;
    pdcch.new_tti(tti_rx);
    TESTASSERT(pdcch.nof_cces() == cell_params[ENB_CC_IDX].nof_cce_table[0]);
    TESTASSERT(pdcch.get_cfi() == 1); // Start at CFI=1

    // Set DL CQI - it should affect aggregation level
    uint32_t dl_cqi = std::uniform_int_distribution<uint32_t>{1, 25}(srsenb::get_rand_gen());
    sched_ue.set_dl_cqi(to_tx_dl(tti_rx), ENB_CC_IDX, dl_cqi);
    uint32_t aggr_idx = get_aggr_level(sched_ue, PCell_IDX, cell_params);
    uint32_t max_nof_cce_locs =
        (*sched_ue.get_locations(ENB_CC_IDX, sf_cch_allocator::MAX_CFI, to_tx_dl(tti_rx).sf_idx()))[aggr_idx].size();

    // allocate DL user
    uint32_t                      prev_cfi = pdcch.get_cfi();
    const cce_cfi_position_table* dci_cce  = sched_ue.get_locations(ENB_CC_IDX, prev_cfi, to_tx_dl(tti_rx).sf_idx());
    uint32_t                      prev_nof_cce_locs = (*dci_cce)[aggr_idx].size();

    TESTASSERT(pdcch.alloc_dci(alloc_type_t::DL_DATA, aggr_idx, &sched_ue, true));
    TESTASSERT(pdcch.nof_allocs() == 1);
    if (prev_nof_cce_locs == pdcch.nof_allocs() - 1) {
      // CFI must be increased
      TESTASSERT(pdcch.get_cfi() > prev_cfi);
    } else {
      // Previous CFI should be fine
      TESTASSERT(pdcch.get_cfi() == prev_cfi);
    }

    dci_cce                           = sched_ue.get_locations(ENB_CC_IDX, pdcch.get_cfi(), to_tx_dl(tti_rx).sf_idx());
    const cce_position_list& dci_locs = (*dci_cce)[aggr_idx];

    // TEST: Check the first alloc of the pdcch result (e.g. rnti, valid cce mask, etc.)
    sf_cch_allocator::alloc_result_t pdcch_result;
    pdcch_mask_t                     pdcch_mask;
    pdcch.get_allocs(&pdcch_result, &pdcch_mask, 0);
    TESTASSERT(pdcch_result.size() == 1);
    TESTASSERT(pdcch_result[0]->rnti == sched_ue.get_rnti());
    TESTASSERT(pdcch_result[0]->total_mask.size() == cell_params[ENB_CC_IDX].nof_cce_table[pdcch.get_cfi() - 1]);
    TESTASSERT(pdcch_result[0]->current_mask == pdcch_result[0]->total_mask);
    TESTASSERT(pdcch_result[0]->current_mask.count() == 1U << aggr_idx);
    TESTASSERT(std::count(dci_locs.begin(), dci_locs.end(), pdcch_result[0]->dci_pos.ncce) > 0);

    // allocate UL user
    if (max_nof_cce_locs == pdcch.nof_allocs()) {
      // no more space
      continue;
    }
    prev_nof_cce_locs = dci_locs.size();
    prev_cfi          = pdcch.get_cfi();
    TESTASSERT(pdcch.alloc_dci(alloc_type_t::UL_DATA, aggr_idx, &sched_ue, true));
    TESTASSERT(pdcch.nof_allocs() == 2);
    if (prev_nof_cce_locs == pdcch.nof_allocs() - 1) {
      // CFI must be increased
      TESTASSERT(pdcch.get_cfi() > prev_cfi);
    } else {
      // Previous CFI should be fine
      TESTASSERT(pdcch.get_cfi() == prev_cfi);
    }

    dci_cce                            = sched_ue.get_locations(ENB_CC_IDX, pdcch.get_cfi(), to_tx_dl(tti_rx).sf_idx());
    const cce_position_list& dci_locs2 = (*dci_cce)[aggr_idx];

    pdcch.get_allocs(&pdcch_result, &pdcch_mask, 0);
    TESTASSERT(pdcch_result.size() == pdcch.nof_allocs());
    TESTASSERT(pdcch_result[1]->rnti == sched_ue.get_rnti());
    TESTASSERT(pdcch_result[1]->total_mask.size() == cell_params[ENB_CC_IDX].nof_cce_table[pdcch.get_cfi() - 1]);
    TESTASSERT((pdcch_result[1]->current_mask & pdcch_result[0]->current_mask).none());
    TESTASSERT(pdcch_result[1]->current_mask.count() == 1U << aggr_idx);
    TESTASSERT(pdcch_result[1]->total_mask == (pdcch_result[0]->current_mask | pdcch_result[1]->current_mask));
    TESTASSERT(std::count(dci_locs2.begin(), dci_locs2.end(), pdcch_result[0]->dci_pos.ncce) > 0);

    srslog::fetch_basic_logger("TEST").info("PDCCH alloc result: %s", pdcch.result_to_string(true).c_str());
  }
  TESTASSERT(tti_counter == nof_ttis);

  return SRSRAN_SUCCESS;
}

int test_pdcch_ue_and_sibs()
{
  // Params
  uint32_t nof_prb = 100;

  std::vector<sched_cell_params_t> cell_params(1);
  sched_interface::ue_cfg_t        ue_cfg   = generate_default_ue_cfg();
  sched_interface::cell_cfg_t      cell_cfg = generate_default_cell_cfg(nof_prb);
  sched_interface::sched_args_t    sched_args{};
  TESTASSERT(cell_params[0].set_cfg(0, cell_cfg, sched_args));

  sf_cch_allocator pdcch;
  sched_ue         sched_ue{0x46, cell_params, ue_cfg};

  pdcch.init(cell_params[PCell_IDX]);
  TESTASSERT(pdcch.nof_allocs() == 0);

  tti_point tti_rx{std::uniform_int_distribution<uint32_t>(0, 9)(get_rand_gen())};

  pdcch.new_tti(tti_rx);
  TESTASSERT(pdcch.nof_cces() == cell_params[0].nof_cce_table[0]);
  TESTASSERT(pdcch.get_cfi() == 1); // Start at CFI=1
  TESTASSERT(pdcch.nof_allocs() == 0);

  TESTASSERT(pdcch.alloc_dci(alloc_type_t::DL_BC, 2));
  TESTASSERT(pdcch.nof_allocs() == 1);
  TESTASSERT(pdcch.alloc_dci(alloc_type_t::DL_RAR, 2));
  TESTASSERT(pdcch.nof_allocs() == 2);
  TESTASSERT(pdcch.alloc_dci(alloc_type_t::DL_DATA, 2, &sched_ue, false));
  TESTASSERT(pdcch.nof_allocs() == 3);

  // TEST: Ability to revert last allocation
  pdcch.rem_last_dci();
  TESTASSERT(pdcch.nof_allocs() == 2);

  // TEST: DCI positions
  uint32_t                         cfi = pdcch.get_cfi();
  sf_cch_allocator::alloc_result_t dci_result;
  pdcch_mask_t                     result_pdcch_mask;
  pdcch.get_allocs(&dci_result, &result_pdcch_mask);
  TESTASSERT(dci_result.size() == 2);
  const cce_position_list& bc_dci_locs = cell_params[0].common_locations[cfi - 1][2];
  TESTASSERT(bc_dci_locs[0] == dci_result[0]->dci_pos.ncce);
  const cce_position_list& rar_dci_locs = cell_params[0].rar_locations[to_tx_dl(tti_rx).sf_idx()][cfi - 1][2];
  TESTASSERT(std::any_of(rar_dci_locs.begin(), rar_dci_locs.end(), [&dci_result](uint32_t val) {
    return dci_result[1]->dci_pos.ncce == val;
  }));

  return SRSRAN_SUCCESS;
}

int test_6prbs()
{
  std::vector<sched_cell_params_t> cell_params(1);
  sched_interface::ue_cfg_t        ue_cfg   = generate_default_ue_cfg();
  sched_interface::cell_cfg_t      cell_cfg = generate_default_cell_cfg(6);
  sched_interface::sched_args_t    sched_args{};
  TESTASSERT(cell_params[0].set_cfg(0, cell_cfg, sched_args));

  sf_cch_allocator                 pdcch;
  sched_ue                         sched_ue{0x46, cell_params, ue_cfg}, sched_ue2{0x47, cell_params, ue_cfg};
  sf_cch_allocator::alloc_result_t dci_result;
  pdcch_mask_t                     result_pdcch_mask;

  pdcch.init(cell_params[PCell_IDX]);
  TESTASSERT(pdcch.nof_allocs() == 0);

  uint32_t opt_cfi     = 3;
  uint32_t bc_aggr_idx = 2, ue_aggr_idx = 1;

  // TEST: The first rnti will pick a DCI position of its 3 possible ones that avoids clash with SIB. The second rnti
  // wont find space
  tti_point tti_rx{0};
  pdcch.new_tti(tti_rx);
  const cce_position_list& bc_dci_locs = cell_params[0].common_locations[opt_cfi - 1][bc_aggr_idx];
  const cce_position_list& rnti_dci_locs =
      (*sched_ue.get_locations(0, opt_cfi, to_tx_dl(tti_rx).sf_idx()))[ue_aggr_idx];
  const cce_position_list& rnti2_dci_locs =
      (*sched_ue2.get_locations(0, opt_cfi, to_tx_dl(tti_rx).sf_idx()))[ue_aggr_idx];

  TESTASSERT(pdcch.alloc_dci(alloc_type_t::DL_BC, bc_aggr_idx));
  TESTASSERT(pdcch.alloc_dci(alloc_type_t::DL_DATA, ue_aggr_idx, &sched_ue, false));
  TESTASSERT(not pdcch.alloc_dci(alloc_type_t::DL_DATA, ue_aggr_idx, &sched_ue2, false));
  TESTASSERT(pdcch.nof_allocs() == 2);

  pdcch.get_allocs(&dci_result, &result_pdcch_mask);
  TESTASSERT(dci_result.size() == 2);
  TESTASSERT(dci_result[0]->dci_pos.ncce == bc_dci_locs[0]);
  TESTASSERT(dci_result[1]->dci_pos.ncce == rnti_dci_locs[2]);

  // TEST: Two RNTIs can be allocated if one doesnt use the PUCCH
  opt_cfi = 2;
  tti_rx  = tti_point{1};
  pdcch.new_tti(tti_rx);
  const cce_position_list& rnti_dci_locs3 =
      (*sched_ue.get_locations(0, opt_cfi, to_tx_dl(tti_rx).sf_idx()))[ue_aggr_idx];
  const cce_position_list& rnti_dci_locs4 =
      (*sched_ue2.get_locations(0, opt_cfi, to_tx_dl(tti_rx).sf_idx()))[ue_aggr_idx];

  TESTASSERT(pdcch.alloc_dci(alloc_type_t::DL_DATA, ue_aggr_idx, &sched_ue, false));
  TESTASSERT(not pdcch.alloc_dci(alloc_type_t::DL_DATA, ue_aggr_idx, &sched_ue2, false));
  TESTASSERT(pdcch.alloc_dci(alloc_type_t::DL_DATA, ue_aggr_idx, &sched_ue2, true));
  TESTASSERT(pdcch.nof_allocs() == 2 and pdcch.get_cfi() == opt_cfi);

  pdcch.get_allocs(&dci_result, &result_pdcch_mask);
  TESTASSERT(dci_result.size() == 2);
  TESTASSERT(dci_result[0]->dci_pos.ncce == rnti_dci_locs3[0]);
  TESTASSERT(dci_result[1]->dci_pos.ncce == rnti_dci_locs4[0]);

  return SRSRAN_SUCCESS;
}

int main()
{
  srsenb::set_randseed(seed);
  printf("This is the chosen seed: %u\n", seed);

  auto& test_log = srslog::fetch_basic_logger("TEST", false);
  test_log.set_level(srslog::basic_levels::info);

  // Start the log backend.
  srslog::init();

  TESTASSERT(test_pdcch_one_ue() == SRSRAN_SUCCESS);
  TESTASSERT(test_pdcch_ue_and_sibs() == SRSRAN_SUCCESS);
  TESTASSERT(test_6prbs() == SRSRAN_SUCCESS);

  srslog::flush();

  printf("Success\n");
}
