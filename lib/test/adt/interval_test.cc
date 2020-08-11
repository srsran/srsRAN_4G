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

#include "srslte/adt/interval.h"
#include "srslte/common/test_common.h"

int test_interval_overlaps()
{
  srslte::interval<int> I{10, 15}, I2{9, 11}, I3{11, 14}, I4{9, 16}, I5{14, 16}, I6{4, 10}, I7{15, 17};

  TESTASSERT(I.overlaps(I2));
  TESTASSERT(I.overlaps(I3));
  TESTASSERT(I.overlaps(I4));
  TESTASSERT(I.overlaps(I5));
  TESTASSERT(not I.overlaps(I6));
  TESTASSERT(not I.overlaps(I7));

  return SRSLTE_SUCCESS;
}

int test_interval_contains()
{
  srslte::interval<int> I{5, 10};

  TESTASSERT(I.contains(5));
  TESTASSERT(I.contains(6));
  TESTASSERT(I.contains(9));
  TESTASSERT(not I.contains(10));
  TESTASSERT(not I.contains(11));
  TESTASSERT(not I.contains(4));

  return SRSLTE_SUCCESS;
}

int test_interval_intersect()
{
  srslte::interval<int> I{5, 10}, I2{3, 6}, I3{9, 12}, I4{10, 13};

  TESTASSERT(srslte::make_intersection(I, I2) == (I & I2));
  TESTASSERT((I & I2) == srslte::interval<int>(5, 6));
  TESTASSERT((I & I3) == srslte::interval<int>(9, 10));
  TESTASSERT(not(I & I3).is_empty());
  TESTASSERT((I & I4).is_empty());

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(test_interval_overlaps() == SRSLTE_SUCCESS);
  TESTASSERT(test_interval_contains() == SRSLTE_SUCCESS);
  TESTASSERT(test_interval_intersect() == SRSLTE_SUCCESS);
  return 0;
}
