/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "sched_nr_cfg_generators.h"
#include "sched_nr_common_test.h"
#include "srsgnb/hdr/stack/mac/sched_nr_interface_utils.h"
#include "srsgnb/hdr/stack/mac/sched_nr_pdcch.h"
#include "srsran/common/test_common.h"

namespace srsenb {

using namespace sched_nr_impl;

/**
 * Test for the case CORESET#0 is active.
 * Given only one PDCCH candidate position is supported, only one PDCCH allocation should take place per slot
 * The test additionally verifies that the DCI context content is correct for each PDCCH allocation
 */
void test_coreset0_cfg()
{
  const uint32_t aggr_idx = 2;

  srsran::test_delimit_logger delimiter{"Test PDCCH Allocation in CORESET#0"};

  sched_nr_interface::sched_args_t   sched_args;
  sched_nr_impl::cell_config_manager cell_cfg{0, get_default_sa_cell_cfg_common(), sched_args};
  bwp_params_t&                      bwp_params = cell_cfg.bwps[0];

  // UE config
  ue_cfg_manager uecfg{get_rach_ue_cfg(0)};
  uecfg.phy_cfg.pdcch = cell_cfg.bwps[0].cfg.pdcch; // Starts without UE-specific PDCCH
  ue_carrier_params_t ue_cc{0x46, bwp_params, uecfg};

  pdcch_dl_list_t       dl_pdcchs;
  pdcch_ul_list_t       ul_pdcchs;
  pdcch_dl_alloc_result dl_pdcch_result;
  pdcch_ul_alloc_result ul_pdcch_result;
  pdcch_dl_t*           dl_pdcch = nullptr;
  pdcch_ul_t*           ul_pdcch = nullptr;

  bwp_pdcch_allocator pdcch_sched(bwp_params, 0, dl_pdcchs, ul_pdcchs);
  for (const srsran_coreset_t& cs : view_active_coresets(cell_cfg.bwps[0].cfg.pdcch)) {
    // Verify nof CCEs is correctly computed
    TESTASSERT_EQ(coreset_nof_cces(cs), pdcch_sched.nof_cces(cs.id));
  }

  // Slot with SIB1
  TESTASSERT_EQ(0, pdcch_sched.nof_allocations());

  // SIB1 allocation should be successful
  dl_pdcch_result = pdcch_sched.alloc_si_pdcch(0, aggr_idx);
  TESTASSERT(dl_pdcch_result.has_value());
  dl_pdcch = dl_pdcch_result.value();
  TESTASSERT_EQ(1, pdcch_sched.nof_allocations());
  TESTASSERT_EQ(srsran_rnti_type_si, dl_pdcch->dci.ctx.rnti_type);
  TESTASSERT_EQ(0, dl_pdcch->dci.ctx.coreset_id);
  test_dci_ctx_consistency(bwp_params.cfg.pdcch, dl_pdcch->dci.ctx);

  // No space for RAR, UE PDSCH/PUSCH
  TESTASSERT(pdcch_sched.alloc_rar_pdcch(0x2, aggr_idx).error() == alloc_result::no_cch_space);
  TESTASSERT(pdcch_sched.alloc_dl_pdcch(srsran_rnti_type_c, 1, aggr_idx, ue_cc).error() == alloc_result::no_cch_space);
  TESTASSERT(pdcch_sched.alloc_ul_pdcch(1, aggr_idx, ue_cc).error() == alloc_result::no_cch_space);

  srslog::fetch_basic_logger("TEST").info("%s", pdcch_sched.print_allocations());

  // Slot with RAR
  pdcch_sched.reset();

  // RAR allocation should be successful
  dl_pdcch_result = pdcch_sched.alloc_rar_pdcch(0x2, aggr_idx);
  TESTASSERT(dl_pdcch_result.has_value());
  dl_pdcch = dl_pdcch_result.value();
  TESTASSERT_EQ(1, pdcch_sched.nof_allocations());
  TESTASSERT_EQ(srsran_rnti_type_ra, dl_pdcch->dci.ctx.rnti_type);
  TESTASSERT_EQ(0, dl_pdcch->dci.ctx.coreset_id);
  TESTASSERT_EQ(srsran_search_space_type_common_1, dl_pdcch->dci.ctx.ss_type);
  test_dci_ctx_consistency(bwp_params.cfg.pdcch, dl_pdcch->dci.ctx);

  // No space for RAR, UE PDSCH/PUSCH
  TESTASSERT(pdcch_sched.alloc_dl_pdcch(srsran_rnti_type_c, 1, aggr_idx, ue_cc).error() == alloc_result::no_cch_space);
  TESTASSERT(pdcch_sched.alloc_ul_pdcch(1, aggr_idx, ue_cc).error() == alloc_result::no_cch_space);

  srslog::fetch_basic_logger("TEST").info("%s", pdcch_sched.print_allocations());

  // Slot with DL PDSCH
  pdcch_sched.reset();

  // 1st PDCCH allocation for DL should be successful
  dl_pdcch_result = pdcch_sched.alloc_dl_pdcch(srsran_rnti_type_c, 1, aggr_idx, ue_cc);
  TESTASSERT(dl_pdcch_result.has_value());
  dl_pdcch = dl_pdcch_result.value();
  TESTASSERT_EQ(1, pdcch_sched.nof_allocations());
  TESTASSERT_EQ(srsran_rnti_type_c, dl_pdcch->dci.ctx.rnti_type);
  TESTASSERT_EQ(0u, dl_pdcch->dci.ctx.coreset_id);
  TESTASSERT_EQ(srsran_search_space_type_common_1, dl_pdcch->dci.ctx.ss_type);
  test_dci_ctx_consistency(bwp_params.cfg.pdcch, dl_pdcch->dci.ctx);

  // No space for 2nd PDCCH allocation
  TESTASSERT(pdcch_sched.alloc_ul_pdcch(1, aggr_idx, ue_cc).error() == alloc_result::no_cch_space);

  srslog::fetch_basic_logger("TEST").info("%s", pdcch_sched.print_allocations());

  // Slot with UL PDSCH
  pdcch_sched.reset();

  // 1st PDCCH allocation for UL should be successful
  ul_pdcch_result = pdcch_sched.alloc_ul_pdcch(1, aggr_idx, ue_cc);
  TESTASSERT(ul_pdcch_result.has_value());
  ul_pdcch = ul_pdcch_result.value();
  TESTASSERT_EQ(1, pdcch_sched.nof_allocations());
  TESTASSERT_EQ(srsran_rnti_type_c, ul_pdcch->dci.ctx.rnti_type);
  TESTASSERT_EQ(0u, ul_pdcch->dci.ctx.coreset_id);
  TESTASSERT_EQ(srsran_search_space_type_common_1, ul_pdcch->dci.ctx.ss_type);
  test_dci_ctx_consistency(bwp_params.cfg.pdcch, ul_pdcch->dci.ctx);

  // No space for 2nd PDCCH allocation
  TESTASSERT(pdcch_sched.alloc_dl_pdcch(srsran_rnti_type_c, 1, aggr_idx, ue_cc).error() == alloc_result::no_cch_space);

  srslog::fetch_basic_logger("TEST").info("%s", pdcch_sched.print_allocations());
}

/**
 * Test for the case CORESET#2 is active.
 * The PDCCH allocator should find enough space to fit SIB1/RAR (in CORESET#0) and UE-dedicated PDCCHs in (CORESET#2)
 * The test additionally verifies that the DCI context content is correct for each PDCCH allocation and there are no
 * collisions between PDCCH CCE allocations
 */
void test_coreset2_cfg()
{
  const uint32_t aggr_idx = 2;

  srsran::test_delimit_logger delimiter{"Test PDCCH Allocation in CORESET#0 and CORESET#2"};

  sched_nr_interface::sched_args_t sched_args;
  sched_nr_cell_cfg_t              cell_cfg      = get_default_sa_cell_cfg_common();
  cell_cfg.bwps[0].pdcch.search_space_present[2] = true;
  cell_cfg.bwps[0].pdcch.search_space[2]         = get_default_ue_specific_search_space(2, 2);
  cell_cfg.bwps[0].pdcch.coreset_present[2]      = true;
  cell_cfg.bwps[0].pdcch.coreset[2]              = get_default_ue_specific_coreset(2, cell_cfg.pci);
  sched_nr_impl::cell_config_manager cellparams{0, cell_cfg, sched_args};
  bwp_params_t&                      bwp_params = cellparams.bwps[0];

  // UE config
  ue_cfg_manager uecfg{get_rach_ue_cfg(0)};
  uecfg.phy_cfg       = get_common_ue_phy_cfg(cell_cfg);
  uecfg.phy_cfg.pdcch = cell_cfg.bwps[0].pdcch; // Starts with UE-specific PDCCH
  ue_carrier_params_t ue_cc{0x46, bwp_params, uecfg};

  pdcch_dl_list_t dl_pdcchs;
  pdcch_ul_list_t ul_pdcchs;
  pdcch_dl_t*     dl_pdcch = nullptr;
  pdcch_ul_t*     ul_pdcch = nullptr;

  bwp_pdcch_allocator pdcch_sched(bwp_params, 0, dl_pdcchs, ul_pdcchs);
  for (const srsran_coreset_t& cs : view_active_coresets(cell_cfg.bwps[0].pdcch)) {
    // Verify nof CCEs is correctly computed
    TESTASSERT_EQ(coreset_nof_cces(cs), pdcch_sched.nof_cces(cs.id));
  }

  // Slot with SIB1 + DL PDCCH and UL PDCCH
  TESTASSERT_EQ(0, pdcch_sched.nof_allocations());

  // SIB1 allocation should be successful
  dl_pdcch = pdcch_sched.alloc_si_pdcch(0, aggr_idx).value();
  TESTASSERT(dl_pdcch != nullptr);
  TESTASSERT_EQ(1, pdcch_sched.nof_allocations());
  TESTASSERT_EQ(srsran_rnti_type_si, dl_pdcch->dci.ctx.rnti_type);
  TESTASSERT_EQ(0, dl_pdcch->dci.ctx.coreset_id);
  srsran_dci_location_t expected_loc{aggr_idx, 0};
  TESTASSERT(dl_pdcch->dci.ctx.location == expected_loc);
  test_dci_ctx_consistency(bwp_params.cfg.pdcch, dl_pdcch->dci.ctx);

  // No space for RAR or PDSCH in SS#1
  TESTASSERT(pdcch_sched.alloc_rar_pdcch(0x2, aggr_idx).error() == alloc_result::no_cch_space);
  TESTASSERT(pdcch_sched.alloc_dl_pdcch(srsran_rnti_type_c, 1, aggr_idx, ue_cc).error() == alloc_result::no_cch_space);
  TESTASSERT(pdcch_sched.alloc_ul_pdcch(1, aggr_idx, ue_cc).error() == alloc_result::no_cch_space);

  // there is space for UE DL PDCCH in SS#2
  dl_pdcch = pdcch_sched.alloc_dl_pdcch(srsran_rnti_type_c, 2, aggr_idx, ue_cc).value();
  TESTASSERT(dl_pdcch != nullptr);
  TESTASSERT_EQ(2, pdcch_sched.nof_allocations());
  TESTASSERT_EQ(srsran_rnti_type_c, dl_pdcch->dci.ctx.rnti_type);
  TESTASSERT_EQ(2u, dl_pdcch->dci.ctx.coreset_id);
  TESTASSERT_EQ(srsran_search_space_type_ue, dl_pdcch->dci.ctx.ss_type);
  expected_loc = srsran_dci_location_t{aggr_idx, 0};
  TESTASSERT(dl_pdcch->dci.ctx.location == expected_loc);
  test_dci_ctx_consistency(bwp_params.cfg.pdcch, dl_pdcch->dci.ctx);

  // there is space for UE UL PDCCH in SS#2
  ul_pdcch = pdcch_sched.alloc_ul_pdcch(2, aggr_idx, ue_cc).value();
  TESTASSERT(ul_pdcch != nullptr);
  TESTASSERT_EQ(3, pdcch_sched.nof_allocations());
  TESTASSERT_EQ(srsran_rnti_type_c, ul_pdcch->dci.ctx.rnti_type);
  TESTASSERT_EQ(srsran_dci_format_nr_0_0, ul_pdcch->dci.ctx.format);
  TESTASSERT_EQ(2u, ul_pdcch->dci.ctx.coreset_id);
  TESTASSERT_EQ(srsran_search_space_type_ue, ul_pdcch->dci.ctx.ss_type);
  expected_loc = srsran_dci_location_t{aggr_idx, 4};
  TESTASSERT(ul_pdcch->dci.ctx.location == expected_loc);
  test_dci_ctx_consistency(bwp_params.cfg.pdcch, ul_pdcch->dci.ctx);

  // No space for 3rd PDCCH allocation in SS#2
  TESTASSERT(pdcch_sched.alloc_dl_pdcch(srsran_rnti_type_c, 2, aggr_idx, ue_cc).error() == alloc_result::no_cch_space);

  // Verify there are no PDCCH collisions
  TESTASSERT_EQ(3, pdcch_sched.nof_allocations());
  TESTASSERT_EQ(2, dl_pdcchs.size());
  TESTASSERT_EQ(1, ul_pdcchs.size());
  test_pdcch_collisions(bwp_params.cfg.pdcch, dl_pdcchs, ul_pdcchs);

  srslog::fetch_basic_logger("TEST").info("%s", pdcch_sched.print_allocations());

  // Verify all coresets are correctly cleaned up
  pdcch_sched.reset();
  TESTASSERT_EQ(0, pdcch_sched.nof_allocations());
  TESTASSERT_EQ(0, dl_pdcchs.size());
  TESTASSERT_EQ(0, ul_pdcchs.size());
}

void test_invalid_params()
{
  const uint32_t aggr_idx = 2;

  srsran::test_delimit_logger delimiter{"Test PDCCH Allocation with Invalid Arguments"};

  sched_nr_cell_cfg_t cell_cfg                       = get_default_sa_cell_cfg_common();
  cell_cfg.bwps[0].pdcch.search_space_present[2]     = true;
  cell_cfg.bwps[0].pdcch.search_space[2]             = get_default_ue_specific_search_space(2, 2);
  cell_cfg.bwps[0].pdcch.coreset_present[2]          = true;
  cell_cfg.bwps[0].pdcch.coreset[2]                  = get_default_ue_specific_coreset(2, cell_cfg.pci);
  cell_cfg.bwps[0].pdcch.search_space_present[3]     = true;
  cell_cfg.bwps[0].pdcch.search_space[3]             = get_default_ue_specific_search_space(3, 2);
  cell_cfg.bwps[0].pdcch.search_space[3].nof_formats = 1; // only DL
  cell_cfg.bwps[0].pdcch.search_space[3].formats[0]  = srsran_dci_format_nr_1_0;
  sched_nr_interface::sched_args_t   sched_args;
  sched_nr_impl::cell_config_manager cellparams{0, cell_cfg, sched_args};
  bwp_params_t&                      bwp_params = cellparams.bwps[0];

  // UE config
  ue_cfg_manager uecfg{get_rach_ue_cfg(0)};
  uecfg.phy_cfg       = get_common_ue_phy_cfg(cell_cfg);
  uecfg.phy_cfg.pdcch = cell_cfg.bwps[0].pdcch; // Starts with UE-specific PDCCH
  ue_carrier_params_t ue_cc{0x46, bwp_params, uecfg};

  pdcch_dl_list_t       dl_pdcchs;
  pdcch_ul_list_t       ul_pdcchs;
  pdcch_dl_alloc_result dl_res;
  pdcch_ul_alloc_result ul_res;

  bwp_pdcch_allocator pdcch_sched(bwp_params, 0, dl_pdcchs, ul_pdcchs);

  // Slot with SIB1 + DL PDCCH and UL PDCCH
  TESTASSERT_EQ(0, pdcch_sched.nof_allocations());

  // Pass UE search space for SI alloc
  dl_res = pdcch_sched.alloc_si_pdcch(2, aggr_idx);
  TESTASSERT(dl_res.is_error() and dl_res.error() == alloc_result::invalid_grant_params);

  // Pass aggregation index for which there are no candidates
  dl_res = pdcch_sched.alloc_si_pdcch(2, 4);
  TESTASSERT(dl_res.is_error() and dl_res.error() == alloc_result::invalid_grant_params);

  // SearchSpace must exist
  dl_res = pdcch_sched.alloc_dl_pdcch(srsran_rnti_type_c, 4, aggr_idx, ue_cc);
  TESTASSERT(dl_res.is_error() and dl_res.error() == alloc_result::invalid_grant_params);

  // TC-RNTI cannot be allocated in Common SearchSpace Type1
  dl_res = pdcch_sched.alloc_dl_pdcch(srsran_rnti_type_tc, 2, aggr_idx, ue_cc);
  TESTASSERT(dl_res.is_error() and dl_res.error() == alloc_result::invalid_grant_params);

  // C-RNTI cannot be allocated in Common SearchSpace Type0
  dl_res = pdcch_sched.alloc_dl_pdcch(srsran_rnti_type_c, 0, aggr_idx, ue_cc);
  TESTASSERT(dl_res.is_error() and dl_res.error() == alloc_result::invalid_grant_params);

  // UL allocation cannot be made in SearchSpace without DCI format 0_0
  ul_res = pdcch_sched.alloc_ul_pdcch(3, aggr_idx, ue_cc);
  TESTASSERT(ul_res.is_error() and ul_res.error() == alloc_result::invalid_grant_params);

  // Success case
  TESTASSERT(pdcch_sched.nof_allocations() == 0);
  ul_res = pdcch_sched.alloc_ul_pdcch(2, aggr_idx, ue_cc);
  TESTASSERT(ul_res.has_value() and ul_res.value()->dci.ctx.format == srsran_dci_format_nr_0_0);
  TESTASSERT(pdcch_sched.nof_allocations() == 1);
}

} // namespace srsenb

int main()
{
  auto& test_logger = srslog::fetch_basic_logger("TEST");
  test_logger.set_level(srslog::basic_levels::info);
  auto& mac_nr_logger = srslog::fetch_basic_logger("MAC-NR");
  mac_nr_logger.set_level(srslog::basic_levels::debug);
  auto& pool_logger = srslog::fetch_basic_logger("POOL");
  pool_logger.set_level(srslog::basic_levels::debug);

  // Start the log backend.
  srslog::init();

  srsenb::test_coreset0_cfg();
  srsenb::test_coreset2_cfg();
  srsenb::test_invalid_params();
}