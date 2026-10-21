// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#include "srsran/adt/optional.h"
#include "srsran/common/test_common.h"

using namespace srsran;

void test_optional_int()
{
  optional<int> opt, opt2(5);
  TESTASSERT(not opt.has_value() and opt2.has_value());
  TESTASSERT(not static_cast<bool>(opt) and static_cast<bool>(opt2));
  TESTASSERT(opt2.value() == 5 and *opt2 == 5);

  opt = 4;
  TESTASSERT(opt.has_value());
  TESTASSERT(opt != opt2);
  opt2 = 4;
  TESTASSERT(opt == opt2);
}

struct C {
  std::unique_ptr<int> val;

  C(int val = 0) : val(std::make_unique<int>(val)) {}
};

void test_optional_move_only()
{
  optional<C> a, b;
  a.emplace(C{});
  TESTASSERT(a.has_value());
  TESTASSERT_EQ(0, *a.value().val);
  TESTASSERT(not b.has_value());
  b.emplace(C{5});
  a = std::move(b);
  TESTASSERT_EQ(5, *a.value().val);
}

int main()
{
  test_optional_int();
  test_optional_move_only();
}