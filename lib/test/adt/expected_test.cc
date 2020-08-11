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

#include "srslte/adt/expected.h"
#include "srslte/common/test_common.h"

int test_expected_trivial()
{
  srslte::expected<int> exp;
  TESTASSERT(exp.has_value());
  TESTASSERT(exp);

  exp = 5;
  TESTASSERT(exp.has_value());
  TESTASSERT(exp.value() == 5);
  TESTASSERT(exp);

  exp.set_error(srslte::default_error_t{});
  TESTASSERT(not exp.has_value());
  TESTASSERT(not exp);

  int i = 2;
  exp   = i;
  TESTASSERT(exp);
  TESTASSERT(exp.value() == 2);

  exp.set_error();
  TESTASSERT(not exp);

  exp = 3;
  {
    srslte::expected<int> exp2 = exp;
    TESTASSERT(exp2 and exp2.value() == 3);
    srslte::expected<int> exp3;
    exp3 = exp2;
    TESTASSERT(exp3 and exp3.value() == 3);
  }
  TESTASSERT(exp and exp.value() == 3);

  exp.set_error();
  {
    srslte::expected<int> exp2{exp};
    TESTASSERT(not exp2);
    srslte::expected<int> exp3;
    exp3 = exp;
    TESTASSERT(not exp3);
  }

  return SRSLTE_SUCCESS;
}

struct C {
  C() : val(0) { count++; }
  C(int v) : val(v) { count++; }
  C(const C& other)
  {
    val = other.val;
    count++;
  }
  C(C&& other)
  {
    val       = other.val;
    other.val = 0;
    count++;
  }
  ~C() { count--; }
  C& operator=(const C& other)
  {
    val = other.val;
    return *this;
  }
  C& operator=(C&& other)
  {
    val       = other.val;
    other.val = 0;
    return *this;
  }
  int             val;
  static uint32_t count;
};
uint32_t C::count = 0;

int test_expected_struct()
{
  srslte::expected<C, int> exp;
  exp = C{5};
  TESTASSERT(exp and exp.value().val == 5);
  TESTASSERT(C::count == 1);

  {
    auto exp2 = exp;
    TESTASSERT(exp2 and exp2.value().val == 5);
    TESTASSERT(C::count == 2);
  }
  TESTASSERT(exp and exp.value().val == 5);
  TESTASSERT(C::count == 1);

  {
    auto exp2 = std::move(exp);
    TESTASSERT(exp2 and exp2.value().val == 5);
    TESTASSERT(exp and exp.value().val == 0);
  }

  exp.set_error(2);
  TESTASSERT(not exp and exp.error() == 2);

  return SRSLTE_SUCCESS;
}

int test_unique_ptr()
{
  srslte::expected<std::unique_ptr<C> > exp;
  TESTASSERT(exp);
  exp.value().reset(new C{2});
  TESTASSERT(exp.value()->val == 2);

  {
    auto exp2 = std::move(exp);
    TESTASSERT(exp.value() == nullptr);
    TESTASSERT(exp2 and exp2.value()->val == 2);
  }

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(test_expected_trivial() == SRSLTE_SUCCESS);
  TESTASSERT(test_expected_struct() == SRSLTE_SUCCESS);
  TESTASSERT(test_unique_ptr() == SRSLTE_SUCCESS);
}
