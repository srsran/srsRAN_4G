/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsenb/hdr/stack/mac/sched_ue_ctrl/sched_cqi.h"
#include "srsran/common/test_common.h"

namespace srsenb {

void test_sched_cqi_one_subband_cqi()
{
  sched_cqi ue_cqi(50, 4);

  // J == 3, N == 9
  TESTASSERT(ue_cqi.nof_bandwidth_parts() == 3);
  TESTASSERT(ue_cqi.nof_subbands() == 9);

  // Ni = 0 -> cqi=5
  ue_cqi.cqi_sb_info(tti_point(0), 0, 5);

  // TEST: updated part has positive cqi. Non-updated cqi didn't change
  TESTASSERT(ue_cqi.get_rbg_grant_avg_cqi(rbg_interval(0, 1)) == 5);
  for (uint32_t i = 1; i < 5; ++i) {
    TESTASSERT(ue_cqi.get_rbg_grant_avg_cqi(rbg_interval(i, i + 1)) > 0);
  }
  TESTASSERT(ue_cqi.get_rbg_grant_avg_cqi(rbg_interval(6, cell_nof_prb_to_rbg(50))) == 0);

  // TEST: Check average cqi over a mask of RBGs
  rbgmask_t mask(cell_nof_prb_to_rbg(50));
  mask.fill(10, mask.size());
  TESTASSERT(ue_cqi.get_rbg_grant_avg_cqi(mask) == 0);
  mask.reset();
  mask.set(1);
  TESTASSERT(ue_cqi.get_rbg_grant_avg_cqi(mask) == 5);
  mask.fill(0, mask.size());
  TESTASSERT(ue_cqi.get_rbg_grant_avg_cqi(mask) > 0 and ue_cqi.get_rbg_grant_avg_cqi(mask) < 5);
}

} // namespace srsenb

int main(int argc, char** argv)
{
  srsran::test_init(argc, argv);

  srsenb::test_sched_cqi_one_subband_cqi();

  return SRSRAN_SUCCESS;
}
