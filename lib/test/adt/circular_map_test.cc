/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsran/adt/circular_map.h"
#include "srsran/common/test_common.h"

namespace srsran {

void test_id_map()
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
}

void test_id_map_wraparound()
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
}

struct C {
  C() { count++; }
  ~C() { count--; }
  C(C&&) { count++; }
  C(const C&) = delete;
  C& operator=(C&&) = default;

  static size_t count;
};
size_t C::count = 0;

void test_correct_destruction()
{
  TESTASSERT(C::count == 0);
  {
    static_circular_map<uint32_t, C, 4> circ_buffer;
    TESTASSERT(C::count == 0);
    TESTASSERT(circ_buffer.insert(0, C{}));
    TESTASSERT(C::count == 1);
    TESTASSERT(circ_buffer.insert(1, C{}));
    TESTASSERT(circ_buffer.insert(2, C{}));
    TESTASSERT(circ_buffer.insert(3, C{}));
    TESTASSERT(C::count == 4);
    TESTASSERT(not circ_buffer.insert(4, C{}));
    TESTASSERT(C::count == 4);
    TESTASSERT(circ_buffer.erase(1));
    TESTASSERT(C::count == 3);
    TESTASSERT(not circ_buffer.contains(1));

    std::array<uint32_t, 3> content{0, 2, 3};
    size_t                  i = 0;
    for (auto& e : circ_buffer) {
      TESTASSERT(content[i] == e.first);
      i++;
    }

    TESTASSERT(C::count == 3);
    static_circular_map<uint32_t, C, 4> circ_buffer2;
    circ_buffer2 = std::move(circ_buffer);
    TESTASSERT(C::count == 3);

    static_circular_map<uint32_t, C, 4> circ_buffer3;
    TESTASSERT(circ_buffer3.insert(1, C{}));
    TESTASSERT(C::count == 4);
    circ_buffer2 = std::move(circ_buffer3);
    TESTASSERT(C::count == 1);
  }
  TESTASSERT(C::count == 0);
}

} // namespace srsran

int main(int argc, char** argv)
{
  auto& test_log = srslog::fetch_basic_logger("TEST");
  test_log.set_level(srslog::basic_levels::info);

  srsran::test_init(argc, argv);

  srsran::test_id_map();
  srsran::test_id_map_wraparound();
  srsran::test_correct_destruction();

  printf("Success\n");
  return SRSRAN_SUCCESS;
}
