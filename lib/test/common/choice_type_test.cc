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

namespace srslte {
namespace choice_details {

static_assert(static_max<1, 2>::value == 2, "StaticMax not working");
static_assert(static_max<2, 1>::value == 2, "StaticMax not working");
static_assert(get_type_index<double, char, float, int, long, double>() == 4, "Type indexer not working");
static_assert(get_type_index<double, double, float, int, long, char>() == 0, "Type indexer not working");
static_assert(get_type_index<double, char, float, int>() == invalid_type_index, "Type Indexer not working");
static_assert(sizeof(choice_storage_t<5, 4>) == 8, "Size of storage wrongly computed");
static_assert(alignof(choice_storage_t<5, 4>) == 4, "Alignment of storage wrongly computed");
static_assert(std::is_same<typename get_index_type<0, char, float, int, long, double>::type, char>::value,
              "type index-based search not working");
static_assert(std::is_same<typename get_index_type<3, char, float, int, long, double>::type, long>::value,
              "type index-based search not working");
static_assert(std::is_same<choice_t<char, int, double>::default_type, char>::value, "Default type is incorrect\n");
static_assert(tagged_union_t<char, int, double>::can_hold_type<int>(), "Can hold type implementation is incorrect\n");
static_assert(not tagged_union_t<char, int, double>::can_hold_type<uint8_t>(),
              "Can hold type implementation is incorrect\n");
static_assert(std::is_same<choice_t<char, int, double>::enable_if_can_hold<char>, void>::value,
              "Metafunction enable if not working\n");
static_assert(std::is_same<choice_t<char, int, double>::disable_if_can_hold<float>, void>::value,
              "Metafunction enable if not working\n");

} // namespace choice_details
} // namespace srslte

int test_tagged_union()
{
  using srslte::choice_details::tagged_union_t;
  tagged_union_t<char, int, double, C> u;
  u.construct_unsafe(5);
  TESTASSERT(u.is<int>());
  TESTASSERT(u.get_unchecked<int>() == 5);
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
  using srslte::choice_t;

  TESTASSERT(C::counter == 0);
  TESTASSERT(D::counter == 0);
  {
    int i = 6;
    C   c0{};

    // TEST: correct construction, holding the right type and value
    choice_t<char, int, double, C> c, c2{i}, c3{c0};
    TESTASSERT(c.is<char>());
    TESTASSERT(c2.is<int>() and srslte::get<int>(c2) == i and *srslte::get_if<int>(c2) == i);
    TESTASSERT(srslte::get<1>(c2) == srslte::get<int>(c2));
    TESTASSERT(c3.is<C>());
    TESTASSERT(C::counter == 2);

    // TEST: Invalid member access. get<>() should throw
    TESTASSERT(srslte::get_if<char>(c2) == nullptr);

    // TEST: simple emplace after construction
    c2 = 'c';
    TESTASSERT(c2.is<char>() and srslte::get<char>(c2) == 'c');

    // TEST: copy ctor test.
    choice_t<char, int, double, C> c5{c3};
    TESTASSERT(C::counter == 3);
    TESTASSERT(c5.is<C>());
    TESTASSERT(srslte::get_if<C>(c5) == &srslte::get<C>(c5));

    // TEST: copy assignment
    c = c5;
    TESTASSERT(C::counter == 4);
    TESTASSERT(c.is<C>() and srslte::get_if<C>(c) != srslte::get_if<C>(c5));
    c = c2;
    TESTASSERT(C::counter == 3);
    TESTASSERT(c2.is<char>() and srslte::get<char>(c) == 'c');
  }
  TESTASSERT(C::counter == 0);
  TESTASSERT(D::counter == 0);
  {
    choice_t<char, int, double, C, D> c, c2{5.0}, c3{C{}}, c4{D{}};
    TESTASSERT(c.is<char>());
    TESTASSERT(c2.is<double>() and srslte::get<double>(c2) == 5.0 and *srslte::get_if<double>(c2) == 5.0);
    TESTASSERT(c3.is<C>());
    TESTASSERT(c4.is<D>());
    TESTASSERT(C::counter == 1);
    TESTASSERT(D::counter == 1);

    // TEST: Move ctor
    choice_t<char, int, double, C, D> c5{std::move(c3)};
    TESTASSERT(C::counter == 2);
    choice_t<char, int, double, C, D> c6{std::move(c4)};
    TESTASSERT(D::counter == 2);

    // TEST: move assignment
    c = std::move(c3);
    TESTASSERT(c.is<C>());
    TESTASSERT(c3.is<C>() and c.holds_same_type(c3)); // move is not destructive
    TESTASSERT(C::counter == 3);
    c = std::move(c4);
    TESTASSERT(c.is<D>());
    TESTASSERT(c4.is<D>() and c.holds_same_type(c4));
    TESTASSERT(C::counter == 2);
    TESTASSERT(D::counter == 3);
    c = std::move(c2);
    TESTASSERT(c.is<double>() and c2.is<double>() and c.holds_same_type(c2));
    TESTASSERT(srslte::get<double>(c) == srslte::get<double>(c2));
    TESTASSERT(C::counter == 2 and D::counter == 2);
  }
  TESTASSERT(C::counter == 0);
  TESTASSERT(D::counter == 0);

  return SRSLTE_SUCCESS;
}

struct E {
  srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*srslte::byte_buffer_pool::get_instance());
};

struct EVisitor {
  template <typename T>
  void operator()(T&& t)
  {
    // do nothing
  }
  void                         operator()(E& e) { pdu = std::move(e.pdu); }
  srslte::unique_byte_buffer_t pdu;
};

int test_visit()
{
  using srslte::choice_t;

  choice_t<int, double, char, E> c{5};

  // TEST: visitor hits integer type which is noop
  EVisitor v;
  srslte::visit(v, c);
  TESTASSERT(c.is<int>() and srslte::get<int>(c) == 5);
  TESTASSERT(v.pdu == nullptr);

  // TEST: visitor hits type E and steals pdu
  E e;
  e.pdu = srslte::allocate_unique_buffer(*srslte::byte_buffer_pool::get_instance());
  c     = std::move(e);
  TESTASSERT(c.is<E>() and srslte::get<E>(c).pdu != nullptr);
  srslte::visit(v, c);
  TESTASSERT(v.pdu != nullptr);
  TESTASSERT(c.is<E>() and srslte::get<E>(c).pdu == nullptr);

  // TEST: visitor hits type E and steals pdu. Second type called there is no pdu to steal.
  v.pdu = nullptr;
  e.pdu = srslte::allocate_unique_buffer(*srslte::byte_buffer_pool::get_instance());
  c     = std::move(e);
  srslte::visit(v, c);
  TESTASSERT(v.pdu != nullptr);
  srslte::visit(v, c);
  TESTASSERT(v.pdu == nullptr);

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(test_tagged_union() == SRSLTE_SUCCESS);
  TESTASSERT(test_choice() == SRSLTE_SUCCESS);
  TESTASSERT(test_visit() == SRSLTE_SUCCESS);
}
