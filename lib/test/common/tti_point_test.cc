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

#include "srslte/common/test_common.h"
#include "srslte/common/tti_point.h"

using srslte::tti_point;

int test_tti_type()
{
  // TEST: constructors
  tti_point tti1;
  TESTASSERT(not tti1.is_valid());
  tti1 = tti_point{5};
  TESTASSERT(tti1.is_valid());

  // TEST: operators
  TESTASSERT(tti1 != tti_point{2});
  TESTASSERT(tti1 == tti_point{5});
  TESTASSERT(tti1 <= tti_point{5});
  TESTASSERT(tti1 >= tti_point{5});
  TESTASSERT(tti1 < tti_point{6});
  TESTASSERT(tti1 > tti_point{4});
  TESTASSERT(tti1 - tti_point{2} == 3);
  TESTASSERT(tti1++ == tti_point{5});
  TESTASSERT(tti1 == tti_point{6});
  TESTASSERT(++tti1 == tti_point{7});

  // TEST: wrap arounds
  TESTASSERT(tti_point{10239} - tti_point{1} == -2);
  TESTASSERT(tti_point{10239} < tti_point{1});
  TESTASSERT(tti_point{10238} - tti_point{10239} == -1);
  TESTASSERT(tti_point{10238} < tti_point{10239});
  TESTASSERT(tti_point{10239} - tti_point{10238} == 1);
  TESTASSERT(tti_point{10239} > tti_point{10238});
  TESTASSERT(tti_point{1} - tti_point{10239} == 2);
  TESTASSERT(tti_point{1} > tti_point{10239});

  tti1 = tti_point{10239};
  TESTASSERT(tti1++ == tti_point{10239});
  TESTASSERT(tti1 == tti_point{0});

  // TEST: tti skips
  tti1 = tti_point{10239};
  tti1 += 10;
  TESTASSERT(tti1 == tti_point{9});
  tti1 -= 11;
  TESTASSERT(tti1 == tti_point{10238});
  tti1 -= 10237;
  TESTASSERT(tti1 == (tti_point)1);

  // TEST: wrap initializations
  TESTASSERT(tti_point{1u - 2u} == tti_point{10239});
  TESTASSERT(tti_point{1u - 100u} == tti_point{10141});
  TESTASSERT(tti_point{10239u + 3u} == tti_point{2});

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(test_tti_type() == SRSLTE_SUCCESS);
  return 0;
}
