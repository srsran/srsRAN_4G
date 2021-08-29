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

#include "srsran/adt/bounded_vector.h"
#include "srsran/support/srsran_test.h"

namespace srsran {

struct C {
  static int nof_copy_ctor;
  static int nof_value_ctor;
  static int nof_move_ctor;
  static int nof_dtor;

  C(int val_ = 0) : val(val_) { nof_value_ctor++; }
  C(const C& v) : val(v.val) { nof_copy_ctor++; }
  C(C&& v) : val(v.val)
  {
    v.val = 0;
    nof_move_ctor++;
  }
  ~C() { nof_dtor++; }
  C& operator=(const C&) = default;
  C& operator            =(C&& other)
  {
    val       = other.val;
    other.val = 0;
    return *this;
  }
  bool operator==(const C& other) const { return val == other.val; }
  bool operator!=(const C& other) const { return not(*this == other); }

  int val = 0;
};
int C::nof_copy_ctor  = 0;
int C::nof_value_ctor = 0;
int C::nof_move_ctor  = 0;
int C::nof_dtor       = 0;

struct moveonly {
  moveonly()                    = default;
  moveonly(moveonly&&) noexcept = default;
  moveonly& operator=(moveonly&&) noexcept = default;
};

void test_ctor()
{
  // TEST: default ctor
  bounded_vector<int, 10> a;
  TESTASSERT(a.size() == 0);
  TESTASSERT(a.capacity() == 10);
  TESTASSERT(a.empty());

  // TEST: copy ctor
  a.push_back(1);
  bounded_vector<int, 10> a2(a);
  TESTASSERT(a2.size() == a.size());
  TESTASSERT(std::equal(a.begin(), a.end(), a2.begin()));

  // TEST: size ctor
  bounded_vector<int, 5> a3(2);
  TESTASSERT(a3.size() == 2);

  // TEST: size+value ctor
  bounded_vector<int, 15> a4(10, 5);
  TESTASSERT(a4.size() == 10);
  for (auto& v : a4) {
    TESTASSERT(v == 5);
  }

  // TEST: initializer_list ctor
  bounded_vector<int, 20> a5({0, 2, 4, 6, 8, 10, 12});
  TESTASSERT(a5.size() == 7);
  for (size_t i = 0; i < a5.size(); ++i) {
    TESTASSERT(a5[i] == (int)i * 2);
  }

  // TEST: move ctor
  bounded_vector<int, 20> a6(std::move(a5));
  TESTASSERT(a6.size() == 7);
  TESTASSERT(a5.size() == 0);
}

void test_obj_add_rem()
{
  // TEST: push_back / emplace_back
  bounded_vector<C, 10> a;
  TESTASSERT(a.size() == 0);
  TESTASSERT(a.empty());
  a.push_back(1);
  a.emplace_back(2);
  TESTASSERT(a.size() == 2);
  TESTASSERT(not a.empty());

  // TEST: resize with size growth
  a.resize(10, 3);
  TESTASSERT(a.size() == 10);
  TESTASSERT(a[0] == 1);
  TESTASSERT(a[1] == 2);
  TESTASSERT(a[2] == 3 and a.back() == 3);

  // TEST: copy ctor correct insertion
  bounded_vector<C, 10> a2(a);
  TESTASSERT(a2.size() == a.size());
  TESTASSERT(std::equal(a.begin(), a.end(), a2.begin()));

  // TEST: back() access
  a.back() = 4;
  TESTASSERT(not std::equal(a.begin(), a.end(), a2.begin()));
  a2 = a;
  TESTASSERT(a == a2);

  // TEST: assign
  a.resize(5);
  a2.assign(a.begin(), a.end());
  TESTASSERT(a2.size() == 5);
  TESTASSERT(a == a2);

  // TEST: pop_back
  int last_nof_dtor = C::nof_dtor;
  a.pop_back();
  TESTASSERT(a.size() == 4 and last_nof_dtor == C::nof_dtor - 1);
  TESTASSERT(a != a2);

  // TEST: erase
  a.erase(a.begin() + 1);
  srsran::bounded_vector<C, 10> test = {1, 3, 3};
  TESTASSERT(a == test);

  // TEST: clear
  last_nof_dtor = C::nof_dtor;
  a.clear();
  TESTASSERT(a.size() == 0 and a.empty() and last_nof_dtor == C::nof_dtor - 3);

  // TEST: move assignment
  TESTASSERT(a2.size() == 5);
  a = std::move(a2);
  TESTASSERT(a.size() == 5 and a2.empty());
  test = {1, 2, 3, 3, 3};
  TESTASSERT(a == test);

  // TEST: move assignment from empty array
  a2.clear();
  a = std::move(a2);
  TESTASSERT(a.empty() and a2.empty());
}

void test_move_only_type()
{
  bounded_vector<moveonly, 10> a(5);
  TESTASSERT(a.size() == 5);

  bounded_vector<moveonly, 10> a2(std::move(a));
  TESTASSERT(a2.size() == 5 and a.empty());

  a2[0] = moveonly();
  moveonly c;
  a2[1] = std::move(c);

  a2.emplace_back();
  TESTASSERT(a2.size() == 6);

  a2.push_back(moveonly());
  TESTASSERT(a2.size() == 7);
}

void assert_dtor_consistency()
{
  TESTASSERT(C::nof_dtor == C::nof_copy_ctor + C::nof_value_ctor + C::nof_move_ctor);
}

} // namespace srsran

int main()
{
  srsran::test_ctor();
  srsran::test_obj_add_rem();
  srsran::test_move_only_type();
  srsran::assert_dtor_consistency();
  printf("Success\n");
  return 0;
}