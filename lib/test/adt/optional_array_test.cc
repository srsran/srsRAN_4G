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

#include "srsran/adt/optional_array.h"
#include "srsran/common/test_common.h"

namespace srsran {

void test_optional_array()
{
  optional_array<int, 5> table1;
  TESTASSERT(table1.size() == 0 and table1.empty());

  TESTASSERT(not table1.contains(0));
  table1.insert(0, 5);
  TESTASSERT(table1.size() == 1 and not table1.empty());
  table1.erase(0);
  TESTASSERT(table1.size() == 0 and table1.empty());
  table1.insert(1, 3);
  table1.insert(4, 2);
  TESTASSERT(table1.size() == 2);
  TESTASSERT(table1[4] == 2 and table1[1] == 3);

  size_t count   = 0;
  int    array[] = {3, 2};
  for (int e : table1) {
    TESTASSERT(array[count++] == e);
  }

  auto it = table1.begin();
  TESTASSERT(*it == 3);
  table1.erase(it);
  TESTASSERT(table1.size() == 1);
}

void test_optional_vector()
{
  optional_vector<int> table1;
  TESTASSERT(table1.size() == 0 and table1.empty());

  TESTASSERT(not table1.contains(0));
  table1.insert(0, 5);
  TESTASSERT(table1.size() == 1 and not table1.empty());
  table1.erase(0);
  TESTASSERT(table1.size() == 0 and table1.empty());
  table1.insert(1, 3);
  table1.insert(4, 2);
  TESTASSERT(table1.size() == 2);
  TESTASSERT(table1[4] == 2 and table1[1] == 3);

  size_t count   = 0;
  int    array[] = {3, 2};
  for (int e : table1) {
    TESTASSERT(array[count++] == e);
  }

  auto it = table1.begin();
  TESTASSERT(*it == 3);
  table1.erase(it);
  TESTASSERT(table1.size() == 1);
}

void test_split_optional_span()
{
  constexpr size_t         L                     = 7;
  int                      some_list[L]          = {};
  bool                     some_list_presence[L] = {};
  split_optional_span<int> view(some_list, some_list_presence, L);

  TESTASSERT(view.size() == 0 and view.empty());
  TESTASSERT(view.begin() == view.end());
  TESTASSERT(not view.contains(0));
  TESTASSERT(view.find_first_empty() == L);

  view.insert(1, 1);
  TESTASSERT(view.size() == 1 and not view.empty());
  TESTASSERT(view.begin() != view.end() and *view.begin() == 1);
  TESTASSERT(view.contains(1));
  TESTASSERT(view[1] == 1);
  TESTASSERT(view.find_first_empty() == 1);

  view.insert(3, 3);
  TESTASSERT(view[3] == 3);
  size_t c = 0;
  for (auto& e : view) {
    TESTASSERT(c == 0 ? e == 1 : e == 3);
    c++;
  }
  TESTASSERT(view.size() == 2);

  view.erase(view.begin());
  TESTASSERT(view.size() == 1);
  TESTASSERT(not view.contains(1) and view.contains(3));

  view.clear();
  TESTASSERT(view.empty());
}

} // namespace srsran

int main(int argc, char** argv)
{
  auto& test_log = srslog::fetch_basic_logger("TEST");
  test_log.set_level(srslog::basic_levels::info);

  srsran::test_init(argc, argv);

  srsran::test_optional_array();
  srsran::test_optional_vector();
  srsran::test_split_optional_span();

  printf("Success\n");
  return SRSRAN_SUCCESS;
}
