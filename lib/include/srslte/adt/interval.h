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

#ifndef SRSLTE_INTERVAL_H
#define SRSLTE_INTERVAL_H

#include "adt_utils.h"
#include <cassert>
#include <string>
#include <type_traits>

namespace srslte {

/// Representation of an interval between two numeric-types in the math representation [start, stop)
template <typename T>
class interval
{
  // TODO: older compilers may not have defined this C++11 trait.
#if (defined(__clang__) && (__clang_major__ >= 5)) || (defined(__GNUG__) && (__GNUC__ >= 5))
  static_assert(std::is_trivially_copyable<T>::value, "Expected to be trivially copyable");
#endif

public:
  interval() : start_(T{}), stop_(T{}) {}
  interval(T start_point, T stop_point) : start_(start_point), stop_(stop_point) { assert(start_ <= stop_); }

  T start() const { return start_; }
  T stop() const { return stop_; }

  bool empty() const { return stop_ == start_; }

  T length() const { return stop_ - start_; }

  void set(T start_point, T stop_point)
  {
    assert(stop_point >= start_point);
    start_ = start_point;
    stop_  = stop_point;
  }

  void resize_by(T len)
  {
    // Detect length overflows
    assert(std::is_unsigned<T>::value or (len >= 0 or length() >= -len));
    stop_ += len;
  }

  void resize_to(T len)
  {
    assert(std::is_unsigned<T>::value or len >= 0);
    stop_ = start_ + len;
  }

  void displace_by(int offset)
  {
    start_ += offset;
    stop_ += offset;
  }

  void displace_to(T start_point)
  {
    stop_  = start_point + length();
    start_ = start_point;
  }

  bool overlaps(interval other) const { return start_ < other.stop_ and other.start_ < stop_; }

  bool contains(T point) const { return start_ <= point and point < stop_; }

  std::string to_string() const
  {
    std::string s = "[" + std::to_string(start_) + "," + std::to_string(stop_) + ")";
    return s;
  }

private:
  T start_;
  T stop_;
};

template <typename T>
bool operator==(const interval<T>& lhs, const interval<T>& rhs)
{
  return lhs.start() == rhs.start() and lhs.stop() == rhs.stop();
}

template <typename T>
bool operator!=(const interval<T>& lhs, const interval<T>& rhs)
{
  return not(lhs == rhs);
}

template <typename T>
bool operator<(const interval<T>& lhs, const interval<T>& rhs)
{
  return lhs.start() < rhs.start() or (lhs.start() == rhs.start() and lhs.stop() < rhs.stop());
}

//! Union of intervals
template <typename T>
interval<T> operator|(const interval<T>& lhs, const interval<T>& rhs)
{
  if (not lhs.overlaps(rhs)) {
    return interval<T>{};
  }
  return {std::min(lhs.start(), rhs.start()), std::max(lhs.stop(), rhs.stop())};
}

template <typename T>
interval<T> make_union(const interval<T>& lhs, const interval<T>& rhs)
{
  return lhs | rhs;
}

//! Intersection of intervals
template <typename T>
interval<T> operator&(const interval<T>& lhs, const interval<T>& rhs)
{
  if (not lhs.overlaps(rhs)) {
    return interval<T>{};
  }
  return interval<T>{std::max(lhs.start(), rhs.start()), std::min(lhs.stop(), rhs.stop())};
}

template <typename T>
interval<T> make_intersection(const interval<T>& lhs, const interval<T>& rhs)
{
  return lhs & rhs;
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const interval<T>& interv)
{
  out << interv.to_string();
  return out;
}

} // namespace srslte

#endif // SRSLTE_INTERVAL_H
