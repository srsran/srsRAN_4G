/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sched_test_utils.h"
#include "srsenb/hdr/stack/mac/sched_common.h"
#include "srsenb/hdr/stack/mac/sched_phy_ch/sched_dci.h"
#include "srslte/common/test_common.h"

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
      return srslte_cqi_to_coderate(std::min(cqi + 1u, 15u), false);
    }
    return srslte_cqi_to_coderate(std::min(cqi + 1u, 15u), use_tbs_index_alt);
  }
};

bool lower_coderate(tbs_info tb, uint32_t nof_re, const tbs_test_args& args)
{
  float max_coderate = srslte_cqi_to_coderate(std::min(args.cqi + 1u, 15u), args.use_tbs_index_alt);
  float coderate     = srslte_coderate(tb.tbs_bytes * 8, nof_re);
  if (coderate > max_coderate) {
    return false;
  }
  srslte_mod_t mod =
      (args.is_ul) ? srslte_ra_ul_mod_from_mcs(tb.mcs) : srslte_ra_dl_mod_from_mcs(tb.mcs, args.use_tbs_index_alt);
  float Qm = std::min(args.get_max_Qm(), srslte_mod_bits_x_symbol(mod));
  return coderate <= 0.930f * Qm;
}

int test_mcs_tbs_dl_helper(const sched_cell_params_t& cell_params, const tbs_test_args& args, tbs_info* result)
{
  uint32_t nof_re       = cell_params.get_dl_lb_nof_re(args.tti_tx_dl, args.prb_grant_size);
  float    max_coderate = args.get_max_coderate();

  if (srslte_coderate(16, nof_re) > max_coderate) {
    // no solution is possible
    return SRSLTE_SUCCESS;
  }

  // Verify MCS, TBS
  tbs_info ret = compute_mcs_and_tbs(
      args.prb_grant_size, nof_re, args.cqi, args.max_mcs, args.is_ul, args.ul64qam_enabled, args.use_tbs_index_alt);
  if (ret.tbs_bytes < 0) {
    return SRSLTE_SUCCESS;
  }
  if (result != nullptr) {
    *result = ret;
  }
  CONDERROR(ret.mcs > (int)args.max_mcs, "Result mcs=%d is higher than stipulated max_mcs=%d", ret.mcs, args.max_mcs);

  // Verify TBS is a valid value in TS tables
  uint32_t tbs_idx      = srslte_ra_tbs_idx_from_mcs(ret.mcs, args.use_tbs_index_alt, args.is_ul);
  int      expected_tbs = srslte_ra_tbs_from_idx(tbs_idx, args.prb_grant_size);
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
    int tbs_idx2  = srslte_ra_tbs_idx_from_mcs(tb2.mcs, args.use_tbs_index_alt, args.is_ul);
    tb2.tbs_bytes = srslte_ra_tbs_from_idx(tbs_idx2, args.prb_grant_size) / 8u;
    TESTASSERT(not lower_coderate(tb2, nof_re, args));
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

  return SRSLTE_SUCCESS;
}

int test_mcs_lookup_specific()
{
  sched_cell_params_t           cell_params = {};
  sched_interface::cell_cfg_t   cell_cfg    = generate_default_cell_cfg(6);
  sched_interface::sched_args_t sched_args  = {};
  cell_params.set_cfg(0, cell_cfg, sched_args);
  tbs_test_args args;
  args.verbose = true;
  tbs_info expected_result;

  /* TEST CASE: DL, no 256-QAM */
  // mcs=1 -> {tbs_idx=1, Nprb=1} -> tbs=24
  TESTASSERT(test_mcs_tbs_dl_helper(cell_params, args, &expected_result) == SRSLTE_SUCCESS);
  CONDERROR(expected_result != tbs_info(24 / 8, 1),
            "TBS computation failure. {%d, %d}!={24, 1}",
            expected_result.tbs_bytes * 8,
            expected_result.mcs);

  // mcs=10 -> {tbs_idx=9, Nprb=1} -> tbs=136
  args.cqi = 15;
  TESTASSERT(test_mcs_tbs_dl_helper(cell_params, args, &expected_result) == SRSLTE_SUCCESS);
  CONDERROR(expected_result != tbs_info(136 / 8, 10),
            "TBS computation failure. {%d, %d}!={136, 10}",
            expected_result.tbs_bytes * 8,
            expected_result.mcs);

  // mcs=5 -> {tbs_idx=5, Nprb=1} -> tbs=72
  cell_params = {};
  cell_cfg    = generate_default_cell_cfg(100);
  cell_params.set_cfg(0, cell_cfg, sched_args);
  args.cqi = 9;
  TESTASSERT(test_mcs_tbs_dl_helper(cell_params, args, &expected_result) == SRSLTE_SUCCESS);
  CONDERROR(expected_result != tbs_info(72 / 8, 5),
            "TBS computation failure. {%d, %d}!={72, 5}",
            expected_result.tbs_bytes * 8,
            expected_result.mcs);

  // mcs=14 -> {tbs_idx=13, Nprb=10} -> tbs=317
  args.prb_grant_size = 10;
  args.cqi            = 10;
  TESTASSERT(test_mcs_tbs_dl_helper(cell_params, args, &expected_result) == SRSLTE_SUCCESS);
  CONDERROR(expected_result != tbs_info(2536 / 8, 14),
            "TBS computation failure. {%d, %d}!={317, 14}",
            expected_result.tbs_bytes * 8,
            expected_result.mcs);

  return SRSLTE_SUCCESS;
}

/// Verify consistency of MCS,TBS computation for different permutations of banwidths, grant sizes, cqi, max_mcs
int test_mcs_tbs_consistency_all()
{
  uint32_t                      prb_list[] = {6, 15, 25, 50, 75, 100};
  sched_interface::sched_args_t sched_args = {};

  for (auto& nof_prb_cell : prb_list) {
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
          TESTASSERT(test_mcs_tbs_dl_helper(cell_params, args, nullptr) == SRSLTE_SUCCESS);
        }
      }
    }
  }
  return SRSLTE_SUCCESS;
}

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
  TESTASSERT(test_mcs_tbs_dl_helper(cell_params, args, &tb_max) == SRSLTE_SUCCESS);
  CONDERROR(tb_max.mcs < result->mcs or tb_max.tbs_bytes < result->tbs_bytes, "Invalid min MCS calculation");

  if (args.verbose) {
    printf("Min: {tbs=%d, mcs=%d}. Max: {tbs=%d, mcs=%d}. Required tbs was %d\n",
           result->tbs_bytes * 8,
           result->mcs,
           tb_max.tbs_bytes * 8,
           tb_max.mcs,
           args.req_bytes * 8);
  }

  return SRSLTE_SUCCESS;
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
  TESTASSERT(test_min_mcs_tbs_dl_helper(cell_params, args, &result) == SRSLTE_SUCCESS);
  CONDERROR(result.tbs_bytes < (int)args.req_bytes, "Invalid MCS calculation");
  CONDERROR(result.tbs_bytes * 8 != 120, "Invalid min TBS calculation");

  args.req_bytes = 50;
  TESTASSERT(test_min_mcs_tbs_dl_helper(cell_params, args, &result) == SRSLTE_SUCCESS);
  CONDERROR(result.tbs_bytes < (int)args.req_bytes, "Invalid MCS calculation");
  CONDERROR(result.tbs_bytes * 8 != 424, "Invalid min TBS calculation");

  args.cqi            = 15;
  args.prb_grant_size = 10;
  args.req_bytes      = 100;
  TESTASSERT(test_min_mcs_tbs_dl_helper(cell_params, args, &result) == SRSLTE_SUCCESS);
  CONDERROR(result.tbs_bytes < (int)args.req_bytes, "Invalid MCS calculation");
  CONDERROR(result.tbs_bytes * 8 != 872, "Invalid min TBS calculation");

  // Check equality case
  args.req_bytes = 109;
  TESTASSERT(test_min_mcs_tbs_dl_helper(cell_params, args, &result) == SRSLTE_SUCCESS);
  CONDERROR(result.tbs_bytes < (int)args.req_bytes, "Invalid MCS calculation");
  CONDERROR(result.tbs_bytes * 8 != 872, "Invalid min TBS calculation");

  return SRSLTE_SUCCESS;
}

} // namespace srsenb

int main()
{
  TESTASSERT(srsenb::test_mcs_lookup_specific() == SRSLTE_SUCCESS);
  TESTASSERT(srsenb::test_mcs_tbs_consistency_all() == SRSLTE_SUCCESS);
  TESTASSERT(srsenb::test_min_mcs_tbs_specific() == SRSLTE_SUCCESS);

  printf("Success\n");
  return 0;
}