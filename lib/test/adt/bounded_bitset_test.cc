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

#include "srslte/adt/bounded_bitset.h"
#include "srslte/common/test_common.h"

int test_zero_bitset()
{
  srslte::bounded_bitset<25> mask;
  srslte::bounded_bitset<25> mask2{23};

  TESTASSERT(mask.max_size() == 25);
  TESTASSERT(mask.size() == 0);
  TESTASSERT(mask.count() == 0);
  TESTASSERT(mask.none());
  TESTASSERT(not mask.any());
  TESTASSERT(mask.all());
  TESTASSERT(mask != mask2);

  TESTASSERT(mask2.max_size() == 25);
  TESTASSERT(mask2.size() == 23);
  TESTASSERT(mask2.size() == 23);
  TESTASSERT(mask2.count() == 0);
  TESTASSERT(mask2.none());
  TESTASSERT(not mask2.any());
  TESTASSERT(not mask2.all());

  mask = mask2;
  TESTASSERT(mask == mask2);

  return SRSLTE_SUCCESS;
}

int test_ones_bitset()
{
  srslte::bounded_bitset<25> mask;

  // Flipping empty bitset is noop
  TESTASSERT(mask.none() and mask.all() and not mask.any());
  mask.flip();
  TESTASSERT(mask.none() and mask.all() and not mask.any());

  // Flipping zeros bitset with size>0 will set all bits to one
  mask.resize(23);
  TESTASSERT(mask.none() and not mask.all() and not mask.any());
  mask.flip();
  TESTASSERT(not mask.none() and mask.all() and mask.any());

  return SRSLTE_SUCCESS;
}

int test_bitset_set()
{
  srslte::bounded_bitset<25> mask{23};
  mask.set(10);

  TESTASSERT(mask.any());
  TESTASSERT(not mask.all());
  TESTASSERT(not mask.test(0));
  TESTASSERT(mask.test(10));
  mask.flip();
  TESTASSERT(not mask.test(10));
  TESTASSERT(mask.test(0));

  return SRSLTE_SUCCESS;
}

int test_bitset_bitwise_oper()
{
  srslte::bounded_bitset<25> mask{23};
  srslte::bounded_bitset<25> mask2{23};

  mask.set(10);
  TESTASSERT(mask != mask2);
  mask2 |= mask;
  TESTASSERT(mask == mask2);

  mask.set(11);
  mask2 &= mask;
  TESTASSERT(mask != mask2);
  TESTASSERT(mask2.test(10) and not mask2.test(11));

#if EXCEPTIONS_ENABLED
  bool caught = false;
  mask2.resize(24);
  try {
    mask2 |= mask;
  } catch (srslte::bad_type_access& c) {
    printf("Received exception \"%s\"\n", c.what());
    caught = true;
  }
  TESTASSERT(caught);
#endif

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(test_zero_bitset() == SRSLTE_SUCCESS);
  TESTASSERT(test_ones_bitset() == SRSLTE_SUCCESS);
  TESTASSERT(test_bitset_set() == SRSLTE_SUCCESS);
  TESTASSERT(test_bitset_bitwise_oper() == SRSLTE_SUCCESS);
  return 0;
}
