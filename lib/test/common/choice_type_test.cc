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

#include "srsran/adt/choice_type.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/test_common.h"

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

namespace srsran {
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
} // namespace srsran

int test_tagged_union()
{
  using srsran::choice_details::tagged_union_t;
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

  return SRSRAN_SUCCESS;
}

int test_choice()
{
  using srsran::choice_t;

  TESTASSERT(C::counter == 0);
  TESTASSERT(D::counter == 0);
  {
    int i = 6;
    C   c0{};

    // TEST: correct construction, holding the right type and value
    choice_t<char, int, double, C> c, c2{i}, c3{c0};
    TESTASSERT(c.is<char>());
    TESTASSERT(c2.is<int>() and srsran::get<int>(c2) == i and *srsran::get_if<int>(c2) == i);
    TESTASSERT(srsran::get<1>(c2) == srsran::get<int>(c2));
    TESTASSERT(c3.is<C>());
    TESTASSERT(C::counter == 2);

    // TEST: Invalid member access. get<>() should throw
    TESTASSERT(srsran::get_if<char>(c2) == nullptr);

    // TEST: simple emplace after construction
    c2 = 'c';
    TESTASSERT(c2.is<char>() and srsran::get<char>(c2) == 'c');

    // TEST: copy ctor test.
    choice_t<char, int, double, C> c5{c3};
    TESTASSERT(C::counter == 3);
    TESTASSERT(c5.is<C>());
    TESTASSERT(srsran::get_if<C>(c5) == &srsran::get<C>(c5));

    // TEST: copy assignment
    c = c5;
    TESTASSERT(C::counter == 4);
    TESTASSERT(c.is<C>() and srsran::get_if<C>(c) != srsran::get_if<C>(c5));
    c = c2;
    TESTASSERT(C::counter == 3);
    TESTASSERT(c2.is<char>() and srsran::get<char>(c) == 'c');
  }
  TESTASSERT(C::counter == 0);
  TESTASSERT(D::counter == 0);
  {
    choice_t<char, int, double, C, D> c, c2{5.0}, c3{C{}}, c4{D{}};
    TESTASSERT(c.is<char>());
    TESTASSERT(c2.is<double>() and srsran::get<double>(c2) == 5.0 and *srsran::get_if<double>(c2) == 5.0);
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
    TESTASSERT(srsran::get<double>(c) == srsran::get<double>(c2));
    TESTASSERT(C::counter == 2 and D::counter == 2);
  }
  TESTASSERT(C::counter == 0);
  TESTASSERT(D::counter == 0);

  return SRSRAN_SUCCESS;
}

struct E {
  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
};

struct EVisitor {
  template <typename T>
  void operator()(T&& t)
  {
    // do nothing
  }
  void                         operator()(E& e) { pdu = std::move(e.pdu); }
  srsran::unique_byte_buffer_t pdu;
};

int test_visit()
{
  using srsran::choice_t;

  choice_t<int, double, char, E> c{5};

  // TEST: visitor hits integer type which is noop
  EVisitor v;
  srsran::visit(v, c);
  TESTASSERT(c.is<int>() and srsran::get<int>(c) == 5);
  TESTASSERT(v.pdu == nullptr);

  // TEST: visitor hits type E and steals pdu
  E e;
  e.pdu = srsran::make_byte_buffer();
  TESTASSERT(e.pdu != nullptr);
  c     = std::move(e);
  TESTASSERT(c.is<E>() and srsran::get<E>(c).pdu != nullptr);
  srsran::visit(v, c);
  TESTASSERT(v.pdu != nullptr);
  TESTASSERT(c.is<E>() and srsran::get<E>(c).pdu == nullptr);

  // TEST: visitor hits type E and steals pdu. Second type called there is no pdu to steal.
  v.pdu = nullptr;
  e.pdu = srsran::make_byte_buffer();
  TESTASSERT(e.pdu != nullptr);
  c     = std::move(e);
  srsran::visit(v, c);
  TESTASSERT(v.pdu != nullptr);
  srsran::visit(v, c);
  TESTASSERT(v.pdu == nullptr);

  return SRSRAN_SUCCESS;
}

int main()
{
  TESTASSERT(test_tagged_union() == SRSRAN_SUCCESS);
  TESTASSERT(test_choice() == SRSRAN_SUCCESS);
  TESTASSERT(test_visit() == SRSRAN_SUCCESS);
}
