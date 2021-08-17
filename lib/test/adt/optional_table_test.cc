/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/optional_table.h"
#include "srsran/common/test_common.h"

namespace srsran {

void test_slot_table()
{
  optional_table<int, 5> table1;
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

} // namespace srsran

int main(int argc, char** argv)
{
  auto& test_log = srslog::fetch_basic_logger("TEST");
  test_log.set_level(srslog::basic_levels::info);

  srsran::test_init(argc, argv);

  srsran::test_slot_table();

  printf("Success\n");
  return SRSRAN_SUCCESS;
}
