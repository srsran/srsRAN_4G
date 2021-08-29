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

#include "sched_test_utils.h"
#include "srsenb/hdr/stack/mac/sched_lte_common.h"
#include "srsenb/hdr/stack/mac/sched_phy_ch/sched_dci.h"
#include "srsran/common/common_lte.h"
#include "srsran/support/srsran_test.h"

namespace srsenb {

struct tbs_test_args {
  bool      verbose           = false;
  bool      is_ul             = false;
  bool      use_tbs_index_alt = false;
  bool      ul64qam_enabled   = false;
  uint32_t  cqi               = 5;
  uint32_t  max_mcs           = 28;
  uint32_t  prb_grant_size    = 1;
  int       req_bytes         = std::numeric_limits<int>::max();
  tti_point tti_tx_dl{0};

  uint32_t get_max_Qm() const
  {
    if (is_ul) {
      return ul64qam_enabled ? 6 : 4;
    }
    return use_tbs_index_alt ? 8 : 6;
  }
  float get_max_coderate() const
  {
    if (is_ul) {
      return srsran_cqi_to_coderate(std::min(cqi + 1u, 15u), false);
    }
    return srsran_cqi_to_coderate(std::min(cqi + 1u, 15u), use_tbs_index_alt);
  }
};

bool lower_coderate(tbs_info tb, uint32_t nof_re, const tbs_test_args& args)
{
  float max_coderate = srsran_cqi_to_coderate(std::min(args.cqi + 1u, 15u), args.use_tbs_index_alt);
  float coderate     = srsran_coderate(tb.tbs_bytes * 8, nof_re);
  if (coderate > max_coderate) {
    return false;
  }
  srsran_mod_t mod =
      (args.is_ul) ? srsran_ra_ul_mod_from_mcs(tb.mcs) : srsran_ra_dl_mod_from_mcs(tb.mcs, args.use_tbs_index_alt);
  float Qm = std::min(args.get_max_Qm(), srsran_mod_bits_x_symbol(mod));
  return coderate <= 0.932f * Qm;
}

int test_mcs_tbs_dl_helper(const sched_cell_params_t& cell_params, const tbs_test_args& args, tbs_info* result)
{
  srsran_dci_dl_t dci;
  dci.format     = SRSRAN_DCI_FORMAT1;
  dci.alloc_type = SRSRAN_RA_ALLOC_TYPE0;
  rbgmask_t rbgmask(cell_params.nof_rbgs);
  rbgmask.fill(0, cell_params.nof_prbs_to_rbgs(args.prb_grant_size));
  dci.type0_alloc.rbg_bitmask = (uint32_t)rbgmask.to_uint64();
  uint32_t nof_re             = cell_params.get_dl_nof_res(args.tti_tx_dl, dci, 1);
  float    max_coderate       = args.get_max_coderate();

  if (srsran_coderate(16, nof_re) > max_coderate) {
    // no solution is possible
    return SRSRAN_SUCCESS;
  }

  // Verify MCS, TBS
  tbs_info ret = compute_mcs_and_tbs(
      args.prb_grant_size, nof_re, args.cqi, args.max_mcs, args.is_ul, args.ul64qam_enabled, args.use_tbs_index_alt);
  if (ret.tbs_bytes < 0) {
    return SRSRAN_SUCCESS;
  }
  if (result != nullptr) {
    *result = ret;
  }
  CONDERROR(ret.mcs > (int)args.max_mcs, "Result mcs=%d is higher than stipulated max_mcs=%d", ret.mcs, args.max_mcs);

  // Verify TBS is a valid value in TS tables
  uint32_t tbs_idx      = srsran_ra_tbs_idx_from_mcs(ret.mcs, args.use_tbs_index_alt, args.is_ul);
  int      expected_tbs = srsran_ra_tbs_from_idx(tbs_idx, args.prb_grant_size);
  CONDERROR(expected_tbs != ret.tbs_bytes * 8,
            "The tbs=%d is not valid. For {mcs=%d,tbs_idx=%d,nof_re=%d,nof_prb=%d}, it should have been tbs=%d",
            ret.tbs_bytes * 8,
            ret.mcs,
            tbs_idx,
            nof_re,
            args.prb_grant_size,
            expected_tbs);

  // Verify coderate doesn't surpass maximum
  CONDERROR(not lower_coderate(ret, nof_re, args), "Coderate is higher than maximum");

  // Verify there were no better {mcs,tbs} solutions
  tbs_info tb2;
  for (tb2.mcs = ret.mcs + 1; tb2.mcs <= (int)args.max_mcs; ++tb2.mcs) {
    int tbs_idx2  = srsran_ra_tbs_idx_from_mcs(tb2.mcs, args.use_tbs_index_alt, args.is_ul);
    tb2.tbs_bytes = srsran_ra_tbs_from_idx(tbs_idx2, args.prb_grant_size) / 8U;
    TESTASSERT(not lower_coderate(tb2, nof_re, args) or (args.prb_grant_size == 1 and tb2.mcs == 6));
  }

  // log results
  if (args.verbose) {
    printf("input={max_mcs=%d,cqi=%d,nof_prb=%d,nof_re=%d} -> output={mcs=%d, tbs=%d, tbs_index=%d}\n",
           args.max_mcs,
           args.cqi,
           args.prb_grant_size,
           nof_re,
           ret.mcs,
           ret.tbs_bytes * 8,
           tbs_idx);
  }

  return SRSRAN_SUCCESS;
}

int assert_mcs_tbs_result(uint32_t cell_nof_prb,
                          uint32_t cqi,
                          uint32_t prb_grant_size,
                          uint32_t tbs,
                          uint32_t mcs,
                          bool     alt_cqi_table = false)
{
  sched_cell_params_t           cell_params = {};
  sched_interface::cell_cfg_t   cell_cfg    = generate_default_cell_cfg(cell_nof_prb);
  sched_interface::sched_args_t sched_args  = {};
  cell_params.set_cfg(0, cell_cfg, sched_args);
  tbs_test_args args;
  args.verbose           = true;
  args.cqi               = cqi;
  args.prb_grant_size    = prb_grant_size;
  args.use_tbs_index_alt = alt_cqi_table;
  if (alt_cqi_table) {
    args.max_mcs = std::min(args.max_mcs, 27U); // limited to 27 for 256-QAM
  }

  tbs_info expected_result;
  TESTASSERT(test_mcs_tbs_dl_helper(cell_params, args, &expected_result) == SRSRAN_SUCCESS);
  CONDERROR(expected_result != tbs_info(tbs / 8, mcs),
            "TBS computation failure. {%d, %d}!={%d, %d}",
            expected_result.tbs_bytes * 8,
            expected_result.mcs,
            tbs,
            mcs);

  return SRSRAN_SUCCESS;
}

int test_mcs_lookup_specific()
{
  /* TEST CASE: DL, no 256-QAM */
  // cqi=5,Nprb=1 -> {mcs=3, tbs_idx=3, tbs=40}
  TESTASSERT(assert_mcs_tbs_result(6, 5, 1, 40, 3) == SRSRAN_SUCCESS);
  TESTASSERT(assert_mcs_tbs_result(6, 5, 4, 256, 4) == SRSRAN_SUCCESS);

  TESTASSERT(assert_mcs_tbs_result(100, 9, 1, 712, 28) == SRSRAN_SUCCESS);
  TESTASSERT(assert_mcs_tbs_result(100, 10, 10, 5736, 25) == SRSRAN_SUCCESS);

  // cqi=15
  TESTASSERT(assert_mcs_tbs_result(6, 15, 1, 336, 19) == SRSRAN_SUCCESS);     // I_tbs=17
  TESTASSERT(assert_mcs_tbs_result(6, 15, 6, 2152, 19) == SRSRAN_SUCCESS);    // I_tbs=17
  TESTASSERT(assert_mcs_tbs_result(100, 15, 1, 712, 28) == SRSRAN_SUCCESS);   // I_tbs=26
  TESTASSERT(assert_mcs_tbs_result(100, 15, 2, 1480, 28) == SRSRAN_SUCCESS);  // I_tbs=26
  TESTASSERT(assert_mcs_tbs_result(100, 15, 10, 7480, 28) == SRSRAN_SUCCESS); // I_tbs=26
  TESTASSERT(assert_mcs_tbs_result(100, 15, 1, 968, 27, true) == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}

/// Verify consistency of MCS,TBS computation for different permutations of banwidths, grant sizes, cqi, max_mcs
int test_mcs_tbs_consistency_all()
{
  sched_interface::sched_args_t sched_args = {};

  for (auto& nof_prb_cell : srsran::lte_cell_nof_prbs) {
    sched_interface::cell_cfg_t cell_cfg    = generate_default_cell_cfg(nof_prb_cell);
    sched_cell_params_t         cell_params = {};
    cell_params.set_cfg(0, cell_cfg, sched_args);
    for (uint32_t prb_grant = 1; prb_grant < nof_prb_cell; ++prb_grant) {
      for (uint32_t cqi = 1; cqi < 15; ++cqi) {
        for (uint32_t max_mcs = 1; max_mcs <= 28; ++max_mcs) {
          tbs_test_args args;
          args.tti_tx_dl      = tti_point{1};
          args.prb_grant_size = prb_grant;
          args.cqi            = cqi;
          args.max_mcs        = max_mcs;
          TESTASSERT(test_mcs_tbs_dl_helper(cell_params, args, nullptr) == SRSRAN_SUCCESS);
        }
      }
    }
  }
  return SRSRAN_SUCCESS;
}

/**
 * Note: assumes lowest bound for nof of REs
 */
int test_min_mcs_tbs_dl_helper(const sched_cell_params_t& cell_params, const tbs_test_args& args, tbs_info* result)
{
  uint32_t nof_re = cell_params.get_dl_lb_nof_re(args.tti_tx_dl, args.prb_grant_size);
  *result         = compute_min_mcs_and_tbs_from_required_bytes(args.prb_grant_size,
                                                        nof_re,
                                                        args.cqi,
                                                        args.max_mcs,
                                                        args.req_bytes,
                                                        args.is_ul,
                                                        args.ul64qam_enabled,
                                                        args.use_tbs_index_alt);
  tbs_info tb_max;
  TESTASSERT(test_mcs_tbs_dl_helper(cell_params, args, &tb_max) == SRSRAN_SUCCESS);
  CONDERROR(tb_max.mcs < result->mcs or tb_max.tbs_bytes < result->tbs_bytes, "Invalid min MCS calculation");

  if (args.verbose) {
    printf("Min: {tbs=%d, mcs=%d}. Max: {tbs=%d, mcs=%d}. Required tbs was %d\n",
           result->tbs_bytes * 8,
           result->mcs,
           tb_max.tbs_bytes * 8,
           tb_max.mcs,
           args.req_bytes * 8);
  }

  return SRSRAN_SUCCESS;
}

/// Test search for minimum MCS/TBS in TS 36.213 table 7.1.7.2.1-1 that fulfills a TBS >= required bytes
int test_min_mcs_tbs_specific()
{
  printf("--- Min MCS test ---\n");
  sched_cell_params_t           cell_params = {};
  sched_interface::cell_cfg_t   cell_cfg    = generate_default_cell_cfg(100);
  sched_interface::sched_args_t sched_args  = {};
  cell_params.set_cfg(0, cell_cfg, sched_args);
  tbs_test_args args;
  args.verbose = true;
  tbs_info result;

  args.cqi            = 10;
  args.prb_grant_size = 5;
  args.req_bytes      = 10;
  TESTASSERT(test_min_mcs_tbs_dl_helper(cell_params, args, &result) == SRSRAN_SUCCESS);
  CONDERROR(result.tbs_bytes < (int)args.req_bytes, "Invalid MCS calculation");
  CONDERROR(result.tbs_bytes * 8 != 120, "Invalid min TBS calculation");

  args.req_bytes = 50;
  TESTASSERT_SUCCESS(test_min_mcs_tbs_dl_helper(cell_params, args, &result));
  CONDERROR(result.tbs_bytes < (int)args.req_bytes, "Invalid MCS calculation");
  CONDERROR(result.tbs_bytes * 8 != 424, "Invalid min TBS calculation");

  args.cqi            = 15;
  args.prb_grant_size = 10;
  args.req_bytes      = 100;
  TESTASSERT(test_min_mcs_tbs_dl_helper(cell_params, args, &result) == SRSRAN_SUCCESS);
  CONDERROR(result.tbs_bytes < (int)args.req_bytes, "Invalid MCS calculation");
  CONDERROR(result.tbs_bytes * 8 != 872, "Invalid min TBS calculation");

  // Check equality case
  args.req_bytes = 109;
  TESTASSERT_SUCCESS(test_min_mcs_tbs_dl_helper(cell_params, args, &result));
  CONDERROR(result.tbs_bytes < (int)args.req_bytes, "Invalid MCS calculation");
  CONDERROR(result.tbs_bytes * 8 != 872, "Invalid min TBS calculation");

  return SRSRAN_SUCCESS;
}

void test_ul_mcs_tbs_derivation()
{
  uint32_t cqi     = 15;
  uint32_t max_mcs = 28;

  sched_cell_params_t cell_params;
  prbmask_t           prbs;

  auto compute_tbs_mcs = [&prbs, &cell_params, &max_mcs, &cqi](uint32_t Nprb, uint32_t prb_grant_size) {
    sched_interface::cell_cfg_t   cell_cfg   = generate_default_cell_cfg(Nprb);
    sched_interface::sched_args_t sched_args = {};
    cell_params.set_cfg(0, cell_cfg, sched_args);
    prbs.resize(Nprb);
    prbs.fill(2, prb_grant_size);
    uint32_t req_bytes = 1000000;
    uint32_t N_srs     = 0;
    uint32_t nof_symb  = 2 * (SRSRAN_CP_NSYMB(cell_params.cfg.cell.cp) - 1) - N_srs;
    uint32_t nof_re    = nof_symb * prbs.count() * SRSRAN_NRE;
    return compute_min_mcs_and_tbs_from_required_bytes(
        prbs.count(), nof_re, cqi, max_mcs, req_bytes, true, false, false);
  };

  cqi = 0;
  TESTASSERT_EQ(0, compute_tbs_mcs(25, 25 - 4).mcs);
  TESTASSERT_EQ(0, compute_tbs_mcs(50, 50 - 5).mcs);

  cqi = 5;
  TESTASSERT_EQ(9, compute_tbs_mcs(25, 25 - 4).mcs);
  TESTASSERT_EQ(9, compute_tbs_mcs(50, 50 - 5).mcs);

  cqi = 15;
  TESTASSERT_EQ(23, compute_tbs_mcs(25, 25 - 4).mcs);
  TESTASSERT_EQ(23, compute_tbs_mcs(50, 50 - 5).mcs);
  TESTASSERT_EQ(24, compute_tbs_mcs(75, 75 - 5).mcs);
  TESTASSERT_EQ(23, compute_tbs_mcs(100, 100 - 5).mcs);
}

} // namespace srsenb

int main()
{
  auto& mac_log = srslog::fetch_basic_logger("MAC");
  mac_log.set_level(srslog::basic_levels::info);
  auto& test_log = srslog::fetch_basic_logger("TEST");
  test_log.set_level(srslog::basic_levels::info);

  // Start the log backend.
  srslog::init();

  TESTASSERT(srsenb::test_mcs_lookup_specific() == SRSRAN_SUCCESS);
  TESTASSERT(srsenb::test_mcs_tbs_consistency_all() == SRSRAN_SUCCESS);
  TESTASSERT(srsenb::test_min_mcs_tbs_specific() == SRSRAN_SUCCESS);
  srsenb::test_ul_mcs_tbs_derivation();

  printf("Success\n");
  return 0;
}
