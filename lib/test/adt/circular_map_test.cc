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

#include "srsran/common/test_common.h"
#include "srsran/adt/circular_map.h"

namespace srsran {

int test_id_map()
{
  static_circular_map<uint32_t, std::string, 16> myobj;
  TESTASSERT(myobj.size() == 0 and myobj.empty() and not myobj.full());
  TESTASSERT(myobj.begin() == myobj.end());

  TESTASSERT(not myobj.contains(0));
  TESTASSERT(myobj.insert(0, "obj0"));
  TESTASSERT(myobj.contains(0) and myobj[0] == "obj0");
  TESTASSERT(myobj.size() == 1 and not myobj.empty() and not myobj.full());
  TESTASSERT(myobj.begin() != myobj.end());

  TESTASSERT(not myobj.insert(0, "obj0"));
  TESTASSERT(myobj.insert(1, "obj1"));
  TESTASSERT(myobj.contains(0) and myobj.contains(1) and myobj[1] == "obj1");
  TESTASSERT(myobj.size() == 2 and not myobj.empty() and not myobj.full());

  TESTASSERT(myobj.find(1) != myobj.end());
  TESTASSERT(myobj.find(1)->first == 1);
  TESTASSERT(myobj.find(1)->second == "obj1");

  // TEST: iteration
  uint32_t count = 0;
  for (std::pair<uint32_t, std::string>& obj : myobj) {
    TESTASSERT(obj.second == "obj" + std::to_string(count++));
  }

  // TEST: const iteration
  count = 0;
  for (const std::pair<uint32_t, std::string>& obj : myobj) {
    TESTASSERT(obj.second == "obj" + std::to_string(count++));
  }

  TESTASSERT(myobj.erase(0));
  TESTASSERT(myobj.erase(1));
  TESTASSERT(myobj.size() == 0 and myobj.empty());

  TESTASSERT(myobj.insert(0, "obj0"));
  TESTASSERT(myobj.insert(1, "obj1"));
  TESTASSERT(myobj.size() == 2 and not myobj.empty() and not myobj.full());
  myobj.clear();
  TESTASSERT(myobj.size() == 0 and myobj.empty());

  return SRSRAN_SUCCESS;
}

int test_id_map_wraparound()
{
  static_circular_map<uint32_t, std::string, 4> mymap;

  // Fill map
  TESTASSERT(mymap.insert(0, "0"));
  TESTASSERT(mymap.insert(1, "1"));
  TESTASSERT(mymap.insert(2, "2"));
  TESTASSERT(mymap.insert(3, "3"));
  TESTASSERT(mymap.full());

  // TEST: Ensure that insertion fails when map is full
  TESTASSERT(not mymap.insert(4, "4"));
  TESTASSERT(not mymap.erase(4));

  // TEST: Ensure that insertion works once the element with matching map index is removed
  TESTASSERT(mymap.erase(0));
  TESTASSERT(not mymap.full());
  TESTASSERT(mymap.insert(4, "4"));
  TESTASSERT(mymap.full());

  return SRSRAN_SUCCESS;
}

} // namespace srsran

int main()
{
  auto& test_log = srslog::fetch_basic_logger("TEST");
  test_log.set_level(srslog::basic_levels::info);

  // Start the log backend.
  srslog::init();

  TESTASSERT(srsran::test_id_map() == SRSRAN_SUCCESS);
  TESTASSERT(srsran::test_id_map_wraparound() == SRSRAN_SUCCESS);
  return SRSRAN_SUCCESS;
}
