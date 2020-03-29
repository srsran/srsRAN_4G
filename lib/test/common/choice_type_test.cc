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

#include "srslte/common/choice_type.h"
#include "srslte/common/test_common.h"

namespace srslte {
namespace choice_details {

static_assert(static_max<1, 2>::value == 2, "StaticMax not working");
static_assert(static_max<2, 1>::value == 2, "StaticMax not working");
static_assert(type_indexer<double, char, float, int, long, double>::index == 0, "Type indexer not working");
static_assert(type_indexer<double, double, float, int, long, char>::index == 4, "Type indexer not working");
static_assert(type_indexer<double, char, float, int>::index == invalid_idx, "Type Indexer not working");
static_assert(sizeof(choice_storage_t<5, 4>) == 8, "Size of storage wrongly computed");
static_assert(alignof(choice_storage_t<5, 4>) == 4, "Alignment of storage wrongly computed");
static_assert(std::is_same<typename type_get<0, char, float, int, long, double>::type, double>::value,
              "type index-based search not working");
static_assert(std::is_same<typename type_get<3, char, float, int, long, double>::type, float>::value,
              "type index-based search not working");
static_assert(std::is_same<tagged_union_t<char, int, double>::default_type, char>::value,
              "Default type is incorrect\n");
static_assert(tagged_union_t<char, int, double>::can_hold_type<int>(), "Can hold type implementation is incorrect\n");
static_assert(not tagged_union_t<char, int, double>::can_hold_type<uint8_t>(),
              "Can hold type implementation is incorrect\n");

struct C {
  static int counter;
  C() { counter++; }
  C(C&&) { counter++; }
  C(const C&) { counter++; }
  C& operator=(C&& other)
  {
    counter++;
    return *this;
  }
  C& operator=(const C& other)
  {
    if (this != &other) {
      counter++;
    }
    return *this;
  }
  ~C() { counter--; }
};
int C::counter = 0;

struct D {
  static int counter;
  D() { counter++; }
  D(D&&) { counter++; }
  D(const D&) = delete;
  D& operator=(const D&) = delete;
  D& operator==(D&&)
  {
    counter++;
    return *this;
  }
  ~D() { counter--; }
};
int D::counter = 0;

int test_tagged_union()
{
  tagged_union_t<char, int, double, C> u;
  u.construct_unsafe(5);
  TESTASSERT(u.is<int>());
  TESTASSERT(u.get_unsafe<int>() == 5);
  TESTASSERT(u.get_unsafe<1>() == 5);
  u.destroy_unsafe<int>();

  TESTASSERT(C::counter == 0);
  u.construct_unsafe<C>(C{});
  TESTASSERT(C::counter == 1);
  u.destroy_unsafe<C>();
  TESTASSERT(C::counter == 0);

  return SRSLTE_SUCCESS;
}

int test_choice()
{
  TESTASSERT(C::counter == 0);
  TESTASSERT(D::counter == 0);
  {
    int i = 6;
    C   c0{};

    // TEST: correct construction, holding the right type and value
    choice_t<char, int, double, C> c, c2{i}, c3{c0};
    TESTASSERT(c.is<char>());
    TESTASSERT(c2.is<int>() and c2.get<int>() == i and *c2.get_if<int>() == i);
    TESTASSERT(c3.is<C>());
    TESTASSERT(C::counter == 2);

    // TEST: Invalid member access. get<>() should throw
    TESTASSERT(c2.get_if<char>() == nullptr);
    bool catched = false;
    try {
      char n = '1';
      n      = c2.get<char>();
      TESTASSERT(n == '1');
    } catch (choice_details::bad_choice_access& e) {
      catched = true;
    }
    TESTASSERT(catched);

    // TEST: simple emplace after construction
    c2 = 'c';
    TESTASSERT(c2.is<char>() and c2.get<char>() == 'c');

    // TEST: copy ctor test.
    choice_t<char, int, double, C> c5{c3};
    TESTASSERT(C::counter == 3);
    TESTASSERT(c5.is<C>());
    TESTASSERT(c5.get_if<C>() == &c5.get<C>());

    // TEST: copy assignment
    c = c5;
    TESTASSERT(C::counter == 4);
    TESTASSERT(c.is<C>() and c.get_if<C>() != c5.get_if<C>());
    c = c2;
    TESTASSERT(C::counter == 3);
    TESTASSERT(c2.is<char>() and c.get<char>() == 'c');
  }
  TESTASSERT(C::counter == 0);
  TESTASSERT(D::counter == 0);
  {
    choice_t<char, int, double, C, D> c, c2{5.0}, c3{C{}}, c4{D{}};
    TESTASSERT(c.is<char>());
    TESTASSERT(c2.is<double>() and c2.get<double>() == 5.0 and *c2.get_if<double>() == 5.0);
    TESTASSERT(c3.is<C>());
    TESTASSERT(c4.is<D>());
    TESTASSERT(C::counter == 1);
    TESTASSERT(D::counter == 1);

    choice_t<char, int, double, C, D> c5{std::move(c3)};
    TESTASSERT(C::counter == 2);
    choice_t<char, int, double, C, D> c6{std::move(c4)};
    TESTASSERT(D::counter == 2);
  }
  TESTASSERT(C::counter == 0);
  TESTASSERT(D::counter == 0);

  return SRSLTE_SUCCESS;
}

} // namespace choice_details
} // namespace srslte

int main()
{
  TESTASSERT(srslte::choice_details::test_tagged_union() == SRSLTE_SUCCESS);
  TESTASSERT(srslte::choice_details::test_choice() == SRSLTE_SUCCESS);
}
