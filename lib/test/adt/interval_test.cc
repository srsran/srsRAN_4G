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

#include "srsran/adt/interval.h"
#include "srsran/common/test_common.h"

int test_interval_init()
{
  srsran::interval<int> I{}, I2{12, 15}, I3{12, 12};

  TESTASSERT(I.empty() and I.start() == 0 and I.stop() == 0);
  TESTASSERT(not I2.empty() and I2.start() == 12 and I2.stop() == 15);
  TESTASSERT(I3.empty() and I3.start() == 12 and I3.stop() == 12);

  return SRSRAN_SUCCESS;
}

int test_interval_overlaps()
{
  srsran::interval<int> I{10, 15}, I2{9, 11}, I3{11, 14}, I4{9, 16}, I5{14, 16}, I6{4, 10}, I7{15, 17};

  TESTASSERT(I.overlaps(I2));
  TESTASSERT(I.overlaps(I3));
  TESTASSERT(I.overlaps(I4));
  TESTASSERT(I.overlaps(I5));
  TESTASSERT(not I.overlaps(I6));
  TESTASSERT(not I.overlaps(I7));

  return SRSRAN_SUCCESS;
}

int test_interval_contains()
{
  srsran::interval<int> I{5, 10};

  TESTASSERT(I.contains(5));
  TESTASSERT(I.contains(6));
  TESTASSERT(I.contains(9));
  TESTASSERT(not I.contains(10));
  TESTASSERT(not I.contains(11));
  TESTASSERT(not I.contains(4));

  return SRSRAN_SUCCESS;
}

int test_interval_intersect()
{
  srsran::interval<int> I{5, 10}, I2{3, 6}, I3{9, 12}, I4{10, 13};

  TESTASSERT(srsran::make_intersection(I, I2) == (I & I2));
  TESTASSERT((I & I2) == srsran::interval<int>(5, 6));
  TESTASSERT((I & I3) == srsran::interval<int>(9, 10));
  TESTASSERT(not(I & I3).empty());
  TESTASSERT((I & I4).empty());

  return SRSRAN_SUCCESS;
}

int test_interval_expand()
{
  srsran::interval<uint32_t> I{};
  srsran::interval<int>      I2{};

  I.resize_by(5);
  TESTASSERT(I.length() == 5);
  I.resize_by(-5);
  TESTASSERT(I.length() == 0);

  I2.resize_by(3);
  TESTASSERT(I2.length() == 3);
  //  I2.expand_by(-4);

  return SRSRAN_SUCCESS;
}

int main()
{
  TESTASSERT(test_interval_init() == SRSRAN_SUCCESS);
  TESTASSERT(test_interval_overlaps() == SRSRAN_SUCCESS);
  TESTASSERT(test_interval_contains() == SRSRAN_SUCCESS);
  TESTASSERT(test_interval_intersect() == SRSRAN_SUCCESS);
  TESTASSERT(test_interval_expand() == SRSRAN_SUCCESS);
  return 0;
}
