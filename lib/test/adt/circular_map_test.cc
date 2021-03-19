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

#include "srslte/adt/circular_map.h"
#include "srslte/common/test_common.h"

namespace srslte {

int test_id_map()
{
  static_circular_map<uint32_t, std::string, 16> myobj;
  TESTASSERT(myobj.size() == 0 and myobj.empty() and not myobj.full());
  TESTASSERT(myobj.begin() == myobj.end());

  TESTASSERT(not myobj.has_key(0));
  TESTASSERT(myobj.insert(0, "obj0"));
  TESTASSERT(myobj.has_key(0) and myobj[0] == "obj0");
  TESTASSERT(myobj.size() == 1 and not myobj.empty() and not myobj.full());
  TESTASSERT(myobj.begin() != myobj.end());

  TESTASSERT(not myobj.insert(0, "obj0"));
  TESTASSERT(myobj.insert(1, "obj1"));
  TESTASSERT(myobj.has_key(0) and myobj.has_key(1) and myobj[1] == "obj1");
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

  return SRSLTE_SUCCESS;
}

} // namespace srslte

int main()
{
  auto& test_log = srslog::fetch_basic_logger("TEST");
  test_log.set_level(srslog::basic_levels::info);

  // Start the log backend.
  srslog::init();

  TESTASSERT(srslte::test_id_map() == SRSLTE_SUCCESS);
  return SRSLTE_SUCCESS;
}