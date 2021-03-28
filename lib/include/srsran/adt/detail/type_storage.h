/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_TYPE_STORAGE_H
#define SRSRAN_TYPE_STORAGE_H

#include <type_traits>
#include <utility>

namespace srsran {

namespace detail {

template <typename T>
struct type_storage {
  using value_type = T;

  template <typename... Args>
  void emplace(Args&&... args)
  {
    new (&buffer) T(std::forward<Args>(args)...);
  }
  void destroy() { get().~T(); }
  void copy_ctor(const type_storage<T>& other) { emplace(other.get()); }
  void move_ctor(type_storage<T>&& other) { emplace(std::move(other.get())); }
  void copy_assign(const type_storage<T>& other) { get() = other.get(); }
  void move_assign(type_storage<T>&& other) { get() = std::move(other.get()); }

  T&       get() { return reinterpret_cast<T&>(buffer); }
  const T& get() const { return reinterpret_cast<const T&>(buffer); }

  typename std::aligned_storage<sizeof(T), alignof(T)>::type buffer;
};

template <typename T>
void copy_if_present_helper(type_storage<T>& lhs, const type_storage<T>& rhs, bool lhs_present, bool rhs_present)
{
  if (lhs_present and rhs_present) {
    lhs.get() = rhs.get();
  }
  if (lhs_present) {
    lhs.destroy();
  }
  if (rhs_present) {
    lhs.copy_ctor(rhs);
  }
}

template <typename T>
void move_if_present_helper(type_storage<T>& lhs, type_storage<T>& rhs, bool lhs_present, bool rhs_present)
{
  if (lhs_present and rhs_present) {
    lhs.move_assign(std::move(rhs));
  }
  if (lhs_present) {
    lhs.destroy();
  }
  if (rhs_present) {
    lhs.move_ctor(std::move(rhs));
  }
}

} // namespace detail

} // namespace srsran

#endif // SRSRAN_TYPE_STORAGE_H
