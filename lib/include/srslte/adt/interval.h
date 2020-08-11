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

#include <string>

namespace srslte {

template <typename T>
class interval
{
public:
  T start;
  T stop;

  interval() : start(T{}), stop(T{}) {}
  interval(const T& start_, const T& stop_) : start(start_), stop(stop_) {}

  bool is_empty() const { return stop <= start; }

  T length() const { return stop - start; }

  void set_length(const T& len) { stop = start + len; }

  void add_offset(int offset)
  {
    start += offset;
    stop += offset;
  }

  void shift_to(int new_start)
  {
    stop  = new_start + length();
    start = new_start;
  }

  bool overlaps(const interval& other) const { return start < other.stop and other.start < stop; }

  bool contains(const T& point) const { return start <= point and point < stop; }

  std::string to_string() const
  {
    std::string s = "[" + std::to_string(start) + "," + std::to_string(stop) + ")";
    return s;
  }
};

template <typename T>
bool operator==(const interval<T>& lhs, const interval<T>& rhs)
{
  return lhs.start == rhs.start and lhs.stop == rhs.stop;
}

template <typename T>
bool operator!=(const interval<T>& lhs, const interval<T>& rhs)
{
  return not(lhs == rhs);
}

template <typename T>
bool operator<(const interval<T>& lhs, const interval<T>& rhs)
{
  return lhs.start < rhs.start or (lhs.start == rhs.start and lhs.stop < rhs.stop);
}

//! Union of intervals
template <typename T>
interval<T> operator|(const interval<T>& lhs, const interval<T>& rhs)
{
  if (not lhs.overlaps(rhs)) {
    return interval<T>{};
  }
  return {std::min(lhs.start, rhs.start), std::max(lhs.stop, rhs.stop)};
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
  return interval<T>{std::max(lhs.start, rhs.start), std::min(lhs.stop, rhs.stop)};
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
