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

#include "srslte/srslog/detail/support/any.h"
#include "testing_helpers.h"

using namespace srslog;

static bool when_default_constructed_then_any_is_empty()
{
  detail::any a;

  ASSERT_EQ(a.has_value(), false);
  ASSERT_EQ(detail::any_cast<int>(&a), nullptr);

  const detail::any b;

  ASSERT_EQ(b.has_value(), false);
  ASSERT_EQ(detail::any_cast<int>(&b), nullptr);

  return true;
}

namespace {

struct copyable_object {
  explicit copyable_object(int x) : x(x) {}
  copyable_object(const copyable_object&) = default;
  int x;
};

struct movable_object {
  explicit movable_object(int x) : x(x) {}
  movable_object(movable_object&&) = default;
  int x;
};

} // namespace

static bool when_constructed_with_lvalue_then_any_has_value()
{
  copyable_object value(5);
  detail::any a(value);

  ASSERT_EQ(a.has_value(), true);

  const copyable_object* p = detail::any_cast<copyable_object>(&a);
  ASSERT_NE(p, nullptr);
  ASSERT_EQ(p->x, 5);

  ASSERT_EQ(detail::any_cast<int>(&a), nullptr);

  return true;
}

static bool when_constructed_with_rvalue_then_any_has_value()
{
  movable_object value(5);
  detail::any a(std::move(value));

  ASSERT_EQ(a.has_value(), true);

  const movable_object* p = detail::any_cast<movable_object>(&a);
  ASSERT_NE(p, nullptr);
  ASSERT_EQ(p->x, 5);

  ASSERT_EQ(detail::any_cast<int>(&a), nullptr);

  return true;
}

static bool when_constructed_with_make_any_then_any_has_int()
{
  int value = 5;
  auto a = detail::make_any<int>(value);

  ASSERT_EQ(a.has_value(), true);

  const int* p = detail::any_cast<int>(&a);
  ASSERT_NE(p, nullptr);
  ASSERT_EQ(*p, value);

  ASSERT_EQ(detail::any_cast<char>(&a), nullptr);

  return true;
}

static bool when_move_constructing_from_any_then_contents_are_transferred()
{
  int value = 5;
  detail::any a(value);

  detail::any b(std::move(a));
  ASSERT_EQ(a.has_value(), false);
  ASSERT_EQ(b.has_value(), true);

  const int* p = detail::any_cast<int>(&b);
  ASSERT_NE(p, nullptr);
  ASSERT_EQ(*p, value);

  ASSERT_EQ(detail::any_cast<char>(&b), nullptr);

  return true;
}

static bool when_move_assigning_from_any_then_contents_are_transferred()
{
  int value = 5;
  detail::any a(value);

  detail::any b(3.0);
  b = std::move(a);
  ASSERT_EQ(a.has_value(), false);
  ASSERT_EQ(b.has_value(), true);

  const int* p = detail::any_cast<int>(&b);
  ASSERT_NE(p, nullptr);
  ASSERT_EQ(*p, value);

  ASSERT_EQ(detail::any_cast<char>(&b), nullptr);

  return true;
}

static bool when_any_is_reset_then_value_is_lost()
{
  detail::any a(1);

  a.reset();
  ASSERT_EQ(a.has_value(), false);

  return true;
}

static bool when_swapping_any_then_values_are_exchanged()
{
  int i = 5;
  detail::any a(i);
  double d = 3.14;
  detail::any b(d);

  swap(a, b);

  ASSERT_EQ(*detail::any_cast<double>(&a), d);
  ASSERT_EQ(*detail::any_cast<int>(&b), i);

  return true;
}

static bool when_null_is_passed_to_any_cast_then_null_is_returned()
{
  detail::any* p = nullptr;
  const detail::any* cp = nullptr;
  ASSERT_EQ(detail::any_cast<int>(p), nullptr);
  ASSERT_EQ(detail::any_cast<int>(cp), nullptr);

  return true;
}

static bool when_empty_any_is_passed_to_any_cast_then_null_is_returned()
{
  detail::any a;
  const detail::any b;
  ASSERT_EQ(detail::any_cast<int>(&a), nullptr);
  ASSERT_EQ(detail::any_cast<int>(&b), nullptr);

  return true;
}

int main()
{
  TEST_FUNCTION(when_default_constructed_then_any_is_empty);
  TEST_FUNCTION(when_constructed_with_lvalue_then_any_has_value);
  TEST_FUNCTION(when_constructed_with_rvalue_then_any_has_value);
  TEST_FUNCTION(when_constructed_with_make_any_then_any_has_int);
  TEST_FUNCTION(when_move_constructing_from_any_then_contents_are_transferred);
  TEST_FUNCTION(when_move_assigning_from_any_then_contents_are_transferred);
  TEST_FUNCTION(when_any_is_reset_then_value_is_lost);
  TEST_FUNCTION(when_swapping_any_then_values_are_exchanged);
  TEST_FUNCTION(when_null_is_passed_to_any_cast_then_null_is_returned);
  TEST_FUNCTION(when_empty_any_is_passed_to_any_cast_then_null_is_returned);

  return 0;
}
