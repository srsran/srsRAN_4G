/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/mac/sched_ue_ctrl/sched_dl_cqi.h"
#include "srsran/common/test_common.h"

namespace srsenb {

void test_sched_cqi_one_subband_cqi()
{
  // 50 PRBs, K=4
  sched_dl_cqi ue_cqi(50, 4, 0);

  // J == 3, N == 9
  TESTASSERT(ue_cqi.nof_bandwidth_parts() == 3);
  TESTASSERT(ue_cqi.nof_subbands() == 9);

  // Ni = 0 -> cqi=5
  ue_cqi.cqi_sb_info(tti_point(0), 0, 5);

  // TEST: updated part has positive cqi. Non-updated cqi didn't change
  TESTASSERT(ue_cqi.get_grant_avg_cqi(rbg_interval(0, 1)) == 5);
  for (uint32_t i = 1; i < 5; ++i) {
    TESTASSERT(ue_cqi.get_grant_avg_cqi(rbg_interval(i, i + 1)) > 0);
  }
  TESTASSERT(ue_cqi.get_grant_avg_cqi(rbg_interval(6, cell_nof_prb_to_rbg(50))) == 0);

  // TEST: Check average cqi over a mask of RBGs
  rbgmask_t mask(cell_nof_prb_to_rbg(50));
  mask.fill(10, mask.size());
  TESTASSERT(ue_cqi.get_grant_avg_cqi(mask) == 0);
  mask.reset();
  mask.set(1);
  TESTASSERT(ue_cqi.get_grant_avg_cqi(mask) == 5);
  mask.fill(0, mask.size());
  TESTASSERT(ue_cqi.get_grant_avg_cqi(mask) > 0 and ue_cqi.get_grant_avg_cqi(mask) < 5);

  // TEST: Get optimal RBG mask in terms of CQI
  mask = ue_cqi.get_optim_rbgmask(5);
  TESTASSERT(mask.count() == 5);
  for (uint32_t i = 0; i < 5; ++i) {
    TESTASSERT(mask.test(i) > 0);
  }
}

void test_sched_cqi_wideband_cqi()
{
  uint32_t nof_prb  = 50;
  uint32_t nof_rbgs = cell_nof_prb_to_rbg(nof_prb);

  sched_dl_cqi ue_cqi(nof_prb, 0, 0);

  ue_cqi.cqi_wb_info(tti_point(0), 5);

  // TEST: all bandwidth has positive cqi.
  for (uint32_t i = 0; i < nof_rbgs; ++i) {
    TESTASSERT(ue_cqi.get_grant_avg_cqi(rbg_interval(i, i + 1)) == 5);
  }
  TESTASSERT(ue_cqi.get_grant_avg_cqi(rbg_interval(0, nof_rbgs)) == 5);

  // TEST: Check average cqi over a mask of RBGs
  rbgmask_t mask(cell_nof_prb_to_rbg(50));
  mask.fill(10, mask.size());
  TESTASSERT(ue_cqi.get_grant_avg_cqi(mask) == 5);

  // TEST: Get optimal RBG mask in terms of CQI
  mask = ue_cqi.get_optim_rbgmask(5);
  TESTASSERT(mask.count() == 5);
  for (uint32_t i = 0; i < 5; ++i) {
    TESTASSERT(mask.test(i) > 0);
  }
}

} // namespace srsenb

int main(int argc, char** argv)
{
  srsran::test_init(argc, argv);

  srsenb::test_sched_cqi_one_subband_cqi();
  srsenb::test_sched_cqi_wideband_cqi();

  return SRSRAN_SUCCESS;
}
