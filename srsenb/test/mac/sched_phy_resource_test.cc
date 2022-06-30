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

#include "srsenb/hdr/stack/mac/sched_phy_ch/sched_phy_resource.h"
#include "srsran/common/common_lte.h"
#include "srsran/common/test_common.h"
#include <random>

namespace srsenb {

std::random_device rd;
std::mt19937       rand_gen(rd());

uint32_t get_rand_Nrbg()
{
  return cell_nof_prb_to_rbg(srsran::lte_cell_nof_prbs[std::uniform_int_distribution<uint32_t>{
      0, srsran::lte_cell_nof_prbs.size() - 1}(rand_gen)]);
}

void test_rbg_mask_helpers()
{
  rbgmask_t rbgs(MAX_NOF_RBGS);

  // TEST: Find contiguous range of zero RBGs in RBG mask
  rbgs.set(0);
  rbgs.set(2);
  rbg_interval interv = find_empty_rbg_interval(1, rbgs);
  TESTASSERT(not interv.empty() and interv.length() == 1 and interv.start() == 1);
  interv = find_empty_rbg_interval(2, rbgs);
  TESTASSERT(not interv.empty() and interv.length() == 2 and interv.start() == 3);
  interv = find_empty_rbg_interval(rbgs.size(), rbgs);
  TESTASSERT(interv.length() + 3 == rbgs.size() and interv.start() == 3);

  // TEST: find mask of zero RBGs in RBG mask
  rbgmask_t empty_rbgs = find_available_rbgmask(1, false, rbgs);
  TESTASSERT(empty_rbgs.count() == 1 and empty_rbgs.test(1));
  empty_rbgs = find_available_rbgmask(5, false, rbgs);
  TESTASSERT(empty_rbgs.count() == 5 and empty_rbgs.test(1) and empty_rbgs.test(3) and not empty_rbgs.test(2));

  // TEST: find mask of zero RBGs in random RBG mask
  std::bernoulli_distribution dist{0.5};
  rbgs = rbgmask_t(get_rand_Nrbg());
  for (size_t i = 0; i < rbgs.size(); ++i) {
    rbgs.set(i, dist(rand_gen));
  }
  empty_rbgs = find_available_rbgmask(rbgs.size(), false, rbgs);
  TESTASSERT(empty_rbgs == ~rbgs);
  uint32_t L = std::uniform_int_distribution<uint32_t>{1, (uint32_t)rbgs.size() - 1}(rand_gen);
  empty_rbgs = find_available_rbgmask(L, false, rbgs);
  TESTASSERT(empty_rbgs.count() <= L and (empty_rbgs & rbgs).none());
  uint32_t nprb = count_prb_per_tb(rbgs);
  TESTASSERT(nprb <= MAX_NOF_PRBS);
}

} // namespace srsenb

int main()
{
  srsenb::test_rbg_mask_helpers();
}
