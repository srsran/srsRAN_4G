/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

int main()
{
  test_optional_int();
}