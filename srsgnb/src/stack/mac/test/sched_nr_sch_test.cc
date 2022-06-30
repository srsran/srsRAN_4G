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
#include "srsgnb/hdr/stack/mac/sched_nr_interface_utils.h"
#include "srsgnb/hdr/stack/mac/sched_nr_sch.h"
#include "srsran/common/test_common.h"
extern "C" {
#include "srsran/phy/common/sliv.h"
}

namespace srsenb {

using namespace sched_nr_impl;

sched_nr_cell_cfg_t get_cell_cfg()
{
  sched_nr_cell_cfg_t cell_cfg                   = get_default_sa_cell_cfg_common();
  cell_cfg.bwps[0].pdcch.search_space_present[2] = true;
  cell_cfg.bwps[0].pdcch.search_space[2]         = get_default_ue_specific_search_space(2, 2);
  cell_cfg.bwps[0].pdcch.coreset_present[2]      = true;
  cell_cfg.bwps[0].pdcch.coreset[2]              = get_default_ue_specific_coreset(2, cell_cfg.pci);
  return cell_cfg;
}

sched_nr_interface::ue_cfg_t get_ue_cfg(const sched_nr_cell_cfg_t& cell_cfg)
{
  sched_nr_ue_cfg_t uecfg = get_rach_ue_cfg(0);
  uecfg.phy_cfg           = get_common_ue_phy_cfg(cell_cfg);
  uecfg.phy_cfg.pdcch     = cell_cfg.bwps[0].pdcch; // Starts with UE-specific PDCCH
  return uecfg;
}

srsran_dci_ctx_t generate_dci_ctx(const srsran_pdcch_cfg_nr_t& pdcch,
                                  uint32_t                     ss_id,
                                  srsran_rnti_type_t           rnti_type,
                                  uint16_t                     rnti,
                                  srsran_dci_format_nr_t       dci_fmt = srsran_dci_format_nr_1_0)
{
  const srsran_search_space_t& ss = pdcch.search_space[ss_id];
  const srsran_coreset_t&      cs = pdcch.coreset[ss.coreset_id];

  srsran_dci_ctx_t ctx;
  ctx.location         = {2, 4};
  ctx.ss_type          = ss.type;
  ctx.coreset_id       = ss.coreset_id;
  ctx.coreset_start_rb = srsran_coreset_start_rb(&cs);
  ctx.rnti_type        = rnti_type;
  ctx.format           = dci_fmt;
  ctx.rnti             = rnti;
  return ctx;
}

void test_dci_freq_assignment(const bwp_params_t& bwp_params, prb_interval grant, const pdcch_dl_t& pdcch)
{
  // Compute BWP PRB limits
  prb_interval lims{0, bwp_params.nof_prb};
  if (SRSRAN_SEARCH_SPACE_IS_COMMON(pdcch.dci.ctx.ss_type) and pdcch.dci.ctx.format == srsran_dci_format_nr_1_0) {
    lims = bwp_params.dci_fmt_1_0_prb_lims(pdcch.dci.ctx.coreset_id);
  }

  // RB indexing should start from the first PRB of CORESET
  uint32_t expected_freq_assignment =
      srsran_ra_nr_type1_riv(lims.length(), grant.start() - lims.start(), grant.length());
  TESTASSERT_EQ(expected_freq_assignment, pdcch.dci.freq_domain_assigment);

  uint32_t st, len;
  srsran_sliv_to_s_and_l(lims.length(), pdcch.dci.freq_domain_assigment, &st, &len);
  prb_interval allocated_prbs{st + lims.start(), st + lims.start() + len};
  TESTASSERT(allocated_prbs == grant);
}

void test_si()
{
  srsran::test_delimit_logger delimiter{"Test PDSCH SI Allocation"};

  static const uint32_t ss_id = 0;

  // Create Cell and UE configs
  sched_nr_interface::sched_args_t   sched_args;
  sched_nr_cell_cfg_t                cellcfg = get_cell_cfg();
  sched_nr_impl::cell_config_manager cell_params{0, cellcfg, sched_args};
  const bwp_params_t&                bwp_params = cell_params.bwps[0];

  pdsch_list_t       pdschs;
  pdsch_alloc_result alloc_res;

  pdsch_allocator pdsch_sched(bwp_params, 0, pdschs);

  pdcch_dl_t pdcch;
  pdcch.dci.ctx = generate_dci_ctx(bwp_params.cfg.pdcch, ss_id, srsran_rnti_type_si, SRSRAN_SIRNTI);

  uint32_t min_prb = bwp_params.dci_fmt_1_0_prb_lims(pdcch.dci.ctx.coreset_id).start();
  uint32_t max_prb = bwp_params.dci_fmt_1_0_prb_lims(pdcch.dci.ctx.coreset_id).stop();

  std::array<prb_interval, 3> grant_list = {
      prb_interval{2, 4}, prb_interval{min_prb, max_prb}, prb_interval{0, bwp_params.nof_prb}};

  for (uint32_t i = 0; i < grant_list.size(); ++i) {
    pdsch_sched.reset();
    TESTASSERT_EQ(0, pdschs.size());

    prb_interval grant = grant_list[i];

    bool success_expected = grant.start() >= min_prb and grant.stop() <= max_prb;

    alloc_result check_ret  = pdsch_sched.is_si_grant_valid(ss_id, grant);
    prb_bitmap   avail_prbs = pdsch_sched.occupied_prbs(ss_id, srsran_dci_format_nr_1_0);
    TESTASSERT_EQ((int)min_prb, avail_prbs.find_lowest(0, avail_prbs.size(), false));
    TESTASSERT_EQ((int)max_prb, avail_prbs.find_lowest(min_prb, avail_prbs.size(), true));

    printf("Attempt %d should be %ssuccessful\n", i, success_expected ? "" : "un");
    alloc_res = pdsch_sched.alloc_si_pdsch(ss_id, grant, pdcch.dci);
    if (success_expected) {
      // SIB1 allocation doesnt go outside CORESET#0 BW
      TESTASSERT(alloc_res.has_value());
      TESTASSERT_EQ(1, pdschs.size());
      TESTASSERT(&pdschs.back() == alloc_res.value());
      TESTASSERT_EQ(0, pdcch.dci.time_domain_assigment);

      TESTASSERT(not avail_prbs.any(grant.start(), grant.stop()));

      test_dci_freq_assignment(bwp_params, grant, pdcch);
    } else {
      TESTASSERT(alloc_res.is_error());
      TESTASSERT(check_ret == alloc_res.error());
      TESTASSERT_EQ(0, pdschs.size());
      TESTASSERT(avail_prbs.any(grant.start(), grant.stop()));
    }
  }
}

void test_rar()
{
  srsran::test_delimit_logger delimiter{"Test PDSCH RAR Allocation"};
  static const uint32_t       ss_id = 1;

  // Create Cell and UE configs
  sched_nr_interface::sched_args_t   sched_args;
  sched_nr_impl::cell_config_manager cell_cfg{0, get_cell_cfg(), sched_args};
  const bwp_params_t&                bwp_params = cell_cfg.bwps[0];

  pdsch_list_t       pdschs;
  pdsch_alloc_result alloc_res;

  pdsch_allocator pdsch_sched(bwp_params, 0, pdschs);

  pdcch_dl_t pdcch;
  pdcch.dci.ctx = generate_dci_ctx(bwp_params.cfg.pdcch, ss_id, srsran_rnti_type_ra, 0x2);

  uint32_t min_prb = bwp_params.dci_fmt_1_0_prb_lims(pdcch.dci.ctx.coreset_id).start();
  uint32_t max_prb = bwp_params.dci_fmt_1_0_prb_lims(pdcch.dci.ctx.coreset_id).stop();

  std::array<prb_interval, 3> grant_list = {
      prb_interval{2, 4}, prb_interval{min_prb, max_prb}, prb_interval{0, bwp_params.nof_prb}};

  for (uint32_t i = 0; i < grant_list.size(); ++i) {
    pdsch_sched.reset();
    TESTASSERT_EQ(0, pdschs.size());

    prb_interval grant = grant_list[i];

    bool success_expected = grant.start() >= min_prb and grant.stop() <= max_prb;

    alloc_result check_ret  = pdsch_sched.is_rar_grant_valid(grant);
    prb_bitmap   avail_prbs = pdsch_sched.occupied_prbs(ss_id, srsran_dci_format_nr_1_0);
    TESTASSERT_EQ((int)min_prb, avail_prbs.find_lowest(0, avail_prbs.size(), false));
    TESTASSERT_EQ((int)max_prb, avail_prbs.find_lowest(min_prb, avail_prbs.size(), true));

    printf("Attempt %d should be %ssuccessful\n", i, success_expected ? "" : "un");
    alloc_res = pdsch_sched.alloc_rar_pdsch(grant, pdcch.dci);
    if (success_expected) {
      // SIB1 allocation doesnt go outside CORESET#0 BW
      TESTASSERT(alloc_res.has_value());
      TESTASSERT_EQ(1, pdschs.size());
      TESTASSERT(&pdschs.back() == alloc_res.value());
      TESTASSERT_EQ(0, pdcch.dci.time_domain_assigment);

      TESTASSERT(not avail_prbs.any(grant.start(), grant.stop()));

      test_dci_freq_assignment(bwp_params, grant, pdcch);
    } else {
      TESTASSERT(alloc_res.is_error());
      TESTASSERT(check_ret == alloc_res.error());
      TESTASSERT_EQ(0, pdschs.size());
      TESTASSERT(avail_prbs.any(grant.start(), grant.stop()));
    }
  }
}

void test_ue_pdsch()
{
  srsran::test_delimit_logger delimiter{"Test PDSCH UE Allocation"};

  // Create Cell and UE configs
  sched_nr_interface::sched_args_t   sched_args;
  sched_nr_cell_cfg_t                cellcfg = get_cell_cfg();
  sched_nr_impl::cell_config_manager cell_params{0, get_cell_cfg(), sched_args};
  sched_nr_impl::ue_cfg_manager      uecfg{get_ue_cfg(cellcfg)};
  const bwp_params_t&                bwp_params = cell_params.bwps[0];
  ue_carrier_params_t                ue_cc{0x4601, bwp_params, uecfg};

  pdsch_list_t       pdschs;
  pdsch_alloc_result alloc_res;

  pdsch_allocator pdsch_sched(bwp_params, 0, pdschs);

  pdcch_dl_t pdcch_common, pdcch_ue;
  pdcch_common.dci.ctx = generate_dci_ctx(bwp_params.cfg.pdcch, 1, srsran_rnti_type_c, 0x4601);
  pdcch_ue.dci.ctx     = generate_dci_ctx(bwp_params.cfg.pdcch, 2, srsran_rnti_type_c, 0x4601);

  prb_interval lims_common = bwp_params.dci_fmt_1_0_prb_lims(pdcch_common.dci.ctx.coreset_id);
  prb_interval lims_ue{0, bwp_params.nof_prb};

  std::array<std::pair<uint32_t, prb_interval>, 4> grant_list = {std::make_pair(1, prb_interval{2, 4}),
                                                                 std::make_pair(1, lims_common),
                                                                 std::make_pair(1, lims_ue),
                                                                 std::make_pair(2, lims_common)};

  for (uint32_t i = 0; i < grant_list.size(); ++i) {
    pdsch_sched.reset();
    TESTASSERT_EQ(0, pdschs.size());

    auto         g     = grant_list[i];
    uint32_t     ss_id = g.first;
    prb_interval grant = g.second;
    prb_interval lims  = ss_id == 1 ? lims_common : lims_ue;
    pdcch_dl_t&  pdcch = ss_id == 1 ? pdcch_common : pdcch_ue;

    bool success_expected = grant.start() >= lims.start() and grant.stop() <= lims.stop();

    alloc_result check_ret  = pdsch_sched.is_ue_grant_valid(ue_cc, ss_id, srsran_dci_format_nr_1_0, grant);
    prb_bitmap   avail_prbs = pdsch_sched.occupied_prbs(ss_id, srsran_dci_format_nr_1_0);
    int          pos        = avail_prbs.find_lowest(0, avail_prbs.size(), false);
    TESTASSERT_EQ((int)lims.start(), pos);
    pos = avail_prbs.find_lowest(lims.start(), avail_prbs.size(), true);
    TESTASSERT_EQ((int)lims.stop(), (pos < 0 ? (int)avail_prbs.size() : pos));

    printf("Attempt %d should be %ssuccessful\n", i, success_expected ? "" : "un");
    alloc_res = pdsch_sched.alloc_ue_pdsch(ss_id, srsran_dci_format_nr_1_0, grant, ue_cc, pdcch.dci);
    TESTASSERT(success_expected == alloc_res.has_value());
    if (success_expected) {
      // SIB1 allocation doesnt go outside CORESET#0 BW
      TESTASSERT_EQ(1, pdschs.size());
      TESTASSERT(&pdschs.back() == alloc_res.value());
      TESTASSERT_EQ(0, pdcch.dci.time_domain_assigment);

      TESTASSERT(not avail_prbs.any(grant.start(), grant.stop()));

      test_dci_freq_assignment(bwp_params, grant, pdcch);
    } else {
      TESTASSERT(check_ret == alloc_res.error());
      TESTASSERT_EQ(0, pdschs.size());
      TESTASSERT(avail_prbs.any(grant.start(), grant.stop()));
    }
  }
}

void test_pdsch_fail()
{
  srsran::test_delimit_logger delimiter{"Test PDSCH Allocation Failure"};

  // Create Cell and UE configs
  sched_nr_interface::sched_args_t   sched_args;
  sched_nr_cell_cfg_t                cellcfg = get_cell_cfg();
  sched_nr_impl::cell_config_manager cell_params{0, cellcfg, sched_args};
  sched_nr_impl::ue_cfg_manager      uecfg{get_ue_cfg(cellcfg)};
  const bwp_params_t&                bwp_params = cell_params.bwps[0];
  ue_carrier_params_t                ue_cc{0x4601, bwp_params, uecfg};

  pdsch_list_t       pdschs;
  pdsch_alloc_result alloc_res;

  pdsch_allocator pdsch_sched(bwp_params, 0, pdschs);

  pdcch_dl_t pdcch_common, pdcch_ue, pdcch_rar, pdcch_si, pdcch;
  pdcch_si.dci.ctx     = generate_dci_ctx(bwp_params.cfg.pdcch, 0, srsran_rnti_type_si, SRSRAN_SIRNTI);
  pdcch_rar.dci.ctx    = generate_dci_ctx(bwp_params.cfg.pdcch, 1, srsran_rnti_type_ra, 0x2);
  pdcch_common.dci.ctx = generate_dci_ctx(bwp_params.cfg.pdcch, 1, srsran_rnti_type_c, 0x4601);
  pdcch_ue.dci.ctx     = generate_dci_ctx(bwp_params.cfg.pdcch, 2, srsran_rnti_type_c, 0x4601);

  // Allocations of type 0 are not compatible with DCI format 1_0
  rbg_bitmap rbgs(bwp_params.N_rbg);
  rbgs.set(1);
  rbgs.set(3);
  prb_grant grant_type0 = rbgs;
  TESTASSERT_EQ(alloc_result::invalid_grant_params, pdsch_sched.alloc_si_pdsch(0, grant_type0, pdcch_si.dci).error());
  TESTASSERT_EQ(alloc_result::invalid_grant_params, pdsch_sched.alloc_rar_pdsch(grant_type0, pdcch_rar.dci).error());
  TESTASSERT_EQ(alloc_result::invalid_grant_params,
                pdsch_sched.alloc_ue_pdsch(1, srsran_dci_format_nr_1_0, grant_type0, ue_cc, pdcch.dci).error());
  TESTASSERT_EQ(alloc_result::invalid_grant_params,
                pdsch_sched.alloc_ue_pdsch(2, srsran_dci_format_nr_1_0, grant_type0, ue_cc, pdcch.dci).error());

  // Resource Allocation type must be compatible with UE PDSCH configuration
  TESTASSERT_EQ(alloc_result::invalid_grant_params,
                pdsch_sched.alloc_ue_pdsch(2, srsran_dci_format_nr_1_1, grant_type0, ue_cc, pdcch.dci).error());

  // Allocations of DCI format 1_0 should start from CORESET first RB and their BW should be limited by CORESET#0 BW
  prb_grant grant_type1 = prb_interval{0, bwp_params.coreset_prb_range(0).stop()};
  TESTASSERT(pdsch_sched.alloc_ue_pdsch(1, srsran_dci_format_nr_1_0, grant_type1, ue_cc, pdcch.dci).is_error());
  grant_type1 = prb_interval{bwp_params.coreset_prb_range(0).start(), bwp_params.nof_prb};
  TESTASSERT(pdsch_sched.alloc_ue_pdsch(1, srsran_dci_format_nr_1_0, grant_type1, ue_cc, pdcch.dci).is_error());
  TESTASSERT(pdsch_sched.alloc_ue_pdsch(2, srsran_dci_format_nr_1_0, grant_type1, ue_cc, pdcch.dci).has_value());

  // PRB collisions are detected
  TESTASSERT(pdsch_sched.alloc_ue_pdsch(2, srsran_dci_format_nr_1_0, prb_interval{5, 6}, ue_cc, pdcch.dci).is_error());
}

void test_multi_pdsch()
{
  srsran::test_delimit_logger delimiter{"Test Multiple PDSCH Allocations"};

  // Create Cell and UE configs
  sched_nr_interface::sched_args_t   sched_args;
  sched_nr_cell_cfg_t                cellcfg = get_cell_cfg();
  sched_nr_impl::cell_config_manager cell_params{0, cellcfg, sched_args};
  sched_nr_impl::ue_cfg_manager      uecfg{get_ue_cfg(cellcfg)};
  const bwp_params_t&                bwp_params = cell_params.bwps[0];
  ue_carrier_params_t                ue_cc{0x4601, bwp_params, uecfg};
  ue_carrier_params_t                ue_cc2{0x4602, bwp_params, uecfg};

  pdsch_list_t       pdschs;
  pdsch_alloc_result alloc_res;

  pdsch_allocator pdsch_sched(bwp_params, 0, pdschs);

  pdcch_dl_t pdcch_sib, pdcch_common, pdcch_ue;
  pdcch_sib.dci.ctx    = generate_dci_ctx(bwp_params.cfg.pdcch, 0, srsran_rnti_type_si, SRSRAN_SIRNTI);
  pdcch_common.dci.ctx = generate_dci_ctx(bwp_params.cfg.pdcch, 1, srsran_rnti_type_c, 0x4601);
  pdcch_ue.dci.ctx     = generate_dci_ctx(bwp_params.cfg.pdcch, 2, srsran_rnti_type_c, 0x4602);

  // Allocate SIB1
  uint32_t    ss_id     = 0;
  pdcch_dl_t* pdcch     = &pdcch_sib;
  prb_bitmap  used_prbs = pdsch_sched.occupied_prbs(ss_id, srsran_dci_format_nr_1_0);
  fmt::print("No allocations yet. Occupied PRBs for common SearchSpace: {:b}\n", used_prbs);
  uint32_t     sib1_grant_size = 4;
  prb_bitmap   sib_prbs        = ~used_prbs;
  int          first_prb       = sib_prbs.find_lowest(0, sib_prbs.size(), true);
  prb_interval sib_grant{(uint32_t)first_prb, sib1_grant_size};
  TESTASSERT_EQ(alloc_result::success, pdsch_sched.is_si_grant_valid(ss_id, sib_grant));
  alloc_res = pdsch_sched.alloc_si_pdsch(ss_id, sib_grant, pdcch->dci);
  TESTASSERT(alloc_res.has_value());
  test_dci_freq_assignment(bwp_params, sib_grant, *pdcch);
  prb_bitmap used_prbs_sib1 = pdsch_sched.occupied_prbs(ss_id, srsran_dci_format_nr_1_0);
  TESTASSERT_EQ(used_prbs_sib1.count(), used_prbs.count() + sib_grant.length());
  TESTASSERT_EQ(alloc_result::sch_collision, pdsch_sched.is_si_grant_valid(ss_id, sib_grant));

  prb_bitmap last_prb_bitmap(used_prbs.size());
  last_prb_bitmap.fill(sib_grant.start(), sib_grant.stop());
  fmt::print("SIB1 allocated. Occupied PRBs:\n{:b} -> {:b}\n", last_prb_bitmap, used_prbs_sib1);

  // Allocate UE in common SearchSpace
  ss_id                      = 1;
  pdcch                      = &pdcch_common;
  prb_bitmap ue_prbs         = ~used_prbs_sib1;
  first_prb                  = ue_prbs.find_lowest(0, ue_prbs.size(), true);
  uint32_t     ue_grant_size = 10;
  prb_interval ue_grant{(uint32_t)first_prb, ue_grant_size};
  TESTASSERT_EQ(alloc_result::success, pdsch_sched.is_ue_grant_valid(ue_cc, ss_id, srsran_dci_format_nr_1_0, ue_grant));
  alloc_res = pdsch_sched.alloc_ue_pdsch(ss_id, srsran_dci_format_nr_1_0, ue_grant, ue_cc, pdcch->dci);
  TESTASSERT(alloc_res.has_value());
  test_dci_freq_assignment(bwp_params, ue_grant, *pdcch);
  prb_bitmap used_prbs_ue = pdsch_sched.occupied_prbs(ss_id, srsran_dci_format_nr_1_0);
  TESTASSERT_EQ(used_prbs_ue.count(), used_prbs_sib1.count() + ue_grant.length());
  TESTASSERT_EQ(alloc_result::sch_collision,
                pdsch_sched.is_ue_grant_valid(ue_cc, ss_id, srsran_dci_format_nr_1_0, ue_grant));

  last_prb_bitmap.reset();
  last_prb_bitmap.fill(ue_grant.start(), ue_grant.stop());
  fmt::print("C-RNTI allocated in Common SearchSpace. Occupied PRBs:\n{:b} -> {:b}\n", last_prb_bitmap, used_prbs_ue);

  // Allocate UE in UE SearchSpace
  ss_id                  = 2;
  pdcch                  = &pdcch_ue;
  used_prbs              = pdsch_sched.occupied_prbs(ss_id, srsran_dci_format_nr_1_0);
  prb_interval ue_grant2 = find_empty_interval_of_length(used_prbs, used_prbs_ue.size(), 0);
  TESTASSERT_EQ(bwp_params.nof_prb, ue_grant2.stop());
  TESTASSERT_EQ(alloc_result::success,
                pdsch_sched.is_ue_grant_valid(ue_cc, ss_id, srsran_dci_format_nr_1_0, ue_grant2));
  alloc_res = pdsch_sched.alloc_ue_pdsch(ss_id, srsran_dci_format_nr_1_0, ue_grant2, ue_cc, pdcch->dci);
  TESTASSERT(alloc_res.has_value());
  test_dci_freq_assignment(bwp_params, ue_grant2, *pdcch);
  prb_bitmap used_prbs_ue2 = pdsch_sched.occupied_prbs(ss_id, srsran_dci_format_nr_1_0);
  TESTASSERT_EQ(used_prbs_ue2.count(), used_prbs.count() + ue_grant2.length());
  TESTASSERT_EQ(alloc_result::sch_collision,
                pdsch_sched.is_ue_grant_valid(ue_cc, ss_id, srsran_dci_format_nr_1_0, ue_grant2));

  last_prb_bitmap.reset();
  last_prb_bitmap.fill(ue_grant2.start(), ue_grant2.stop());
  fmt::print("C-RNTI allocated in UE-dedicated common SearchSpace. Occupied PRBs:\n{:b} -> {:b}\n",
             last_prb_bitmap,
             used_prbs_ue2);

  TESTASSERT_EQ(3, pdschs.size());
  pdsch_sched.reset();
  TESTASSERT_EQ(0, pdschs.size());
  TESTASSERT_EQ(0, pdsch_sched.occupied_prbs(2, srsran_dci_format_nr_1_0).count());
}

void test_multi_pusch()
{
  srsran::test_delimit_logger delimiter{"Test Multiple PUSCH Allocations"};

  // Create Cell and UE configs
  sched_nr_interface::sched_args_t   sched_args;
  sched_nr_cell_cfg_t                cellcfg = get_cell_cfg();
  sched_nr_impl::cell_config_manager cell_params{0, cellcfg, sched_args};
  sched_nr_impl::ue_cfg_manager      uecfg{get_ue_cfg(cellcfg)};
  const bwp_params_t&                bwp_params = cell_params.bwps[0];
  ue_carrier_params_t                ue_cc{0x4601, bwp_params, uecfg};
  ue_carrier_params_t                ue_cc2{0x4602, bwp_params, uecfg};

  pusch_list_t       puschs;
  pusch_alloc_result alloc_res;

  pusch_allocator pusch_sched(bwp_params, 0, puschs);

  pdcch_ul_t pdcch_ue1, pdcch_ue2;
  pdcch_ue1.dci.ctx = generate_dci_ctx(bwp_params.cfg.pdcch, 1, srsran_rnti_type_c, 0x4601);
  pdcch_ue2.dci.ctx = generate_dci_ctx(bwp_params.cfg.pdcch, 2, srsran_rnti_type_c, 0x4602);

  // Allocate UE in common SearchSpace
  uint32_t     ss_id         = 1;
  pdcch_ul_t*  pdcch         = &pdcch_ue1;
  prb_bitmap   used_prbs     = pusch_sched.occupied_prbs();
  uint32_t     ue_grant_size = 10;
  prb_interval ue_grant      = find_empty_interval_of_length(used_prbs, ue_grant_size);
  TESTASSERT_EQ(alloc_result::success, pusch_sched.is_grant_valid(srsran_search_space_type_common_1, ue_grant));
  alloc_res = pusch_sched.alloc_pusch(pdcch->dci.ctx.ss_type, ue_grant, pdcch->dci);
  TESTASSERT(alloc_res.has_value());
  prb_bitmap used_prbs_ue1 = pusch_sched.occupied_prbs();
  TESTASSERT_EQ(used_prbs_ue1.count(), used_prbs.count() + ue_grant.length());
  TESTASSERT_EQ(alloc_result::sch_collision,
                pusch_sched.is_grant_valid(srsran_search_space_type_common_1, ue_grant, false));

  prb_bitmap last_prb_bitmap(used_prbs.size());
  last_prb_bitmap.fill(ue_grant.start(), ue_grant.stop());
  fmt::print("C-RNTI allocated in Common SearchSpace. Occupied PRBs:\n{:b} -> {:b}\n", last_prb_bitmap, used_prbs_ue1);

  // Allocate UE in dedicated SearchSpace
  ss_id                  = 2;
  pdcch                  = &pdcch_ue2;
  used_prbs              = pusch_sched.occupied_prbs();
  prb_interval ue2_grant = find_empty_interval_of_length(used_prbs, used_prbs.size());
  TESTASSERT_EQ(alloc_result::success, pusch_sched.is_grant_valid(srsran_search_space_type_ue, ue2_grant));
  alloc_res = pusch_sched.alloc_pusch(pdcch->dci.ctx.ss_type, ue2_grant, pdcch->dci);
  TESTASSERT(alloc_res.has_value());
  prb_bitmap used_prbs_ue2 = pusch_sched.occupied_prbs();
  TESTASSERT_EQ(used_prbs_ue2.count(), used_prbs.count() + ue2_grant.length());
  TESTASSERT_EQ(alloc_result::sch_collision, pusch_sched.is_grant_valid(srsran_search_space_type_ue, ue2_grant, false));

  last_prb_bitmap.reset();
  last_prb_bitmap.fill(ue2_grant.start(), ue2_grant.stop());
  fmt::print("C-RNTI allocated in Common SearchSpace. Occupied PRBs:\n{:b} -> {:b}\n", last_prb_bitmap, used_prbs_ue2);
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

  srsenb::test_si();
  srsenb::test_rar();
  srsenb::test_ue_pdsch();
  srsenb::test_pdsch_fail();
  srsenb::test_multi_pdsch();
  srsenb::test_multi_pusch();
}