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

#ifndef SRSRAN_TYPE_STORAGE_H
#define SRSRAN_TYPE_STORAGE_H

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace srsran {

namespace detail {

// NOTE: gcc 4.8.5 is missing std::max_align_t. Need to create a struct
union max_alignment_t {
  char        c;
  float       f;
  uint32_t    i;
  uint64_t    i2;
  double      d;
  long double d2;
  uint32_t*   ptr;
};
const static size_t max_alignment = alignof(max_alignment_t);

template <typename T, size_t MinSize = 0, size_t AlignSize = 0>
struct type_storage {
  using value_type = T;

  template <typename... Args>
  void emplace(Args&&... args)
  {
    new (&buffer) T(std::forward<Args>(args)...);
  }
  void destroy() { get().~T(); }
  void copy_ctor(const type_storage& other) { emplace(other.get()); }
  void move_ctor(type_storage&& other) { emplace(std::move(other.get())); }
  void copy_assign(const type_storage& other) { get() = other.get(); }
  void move_assign(type_storage&& other) { get() = std::move(other.get()); }

  T&       get() { return reinterpret_cast<T&>(buffer); }
  const T& get() const { return reinterpret_cast<const T&>(buffer); }

  void*       addr() { return static_cast<void*>(&buffer); }
  const void* addr() const { return static_cast<void*>(&buffer); }
  explicit    operator void*() { return addr(); }

  const static size_t obj_size   = sizeof(T) > MinSize ? sizeof(T) : MinSize;
  const static size_t align_size = alignof(T) > AlignSize ? alignof(T) : AlignSize;

  typename std::aligned_storage<obj_size, align_size>::type buffer;
};

template <typename T, size_t MinSize, size_t AlignSize>
void copy_if_present_helper(type_storage<T, MinSize, AlignSize>&       lhs,
                            const type_storage<T, MinSize, AlignSize>& rhs,
                            bool                                       lhs_present,
                            bool                                       rhs_present)
{
  if (lhs_present and rhs_present) {
    lhs.get() = rhs.get();
  } else if (lhs_present) {
    lhs.destroy();
  } else if (rhs_present) {
    lhs.copy_ctor(rhs);
  }
}

template <typename T, size_t MinSize, size_t AlignSize>
void move_if_present_helper(type_storage<T, MinSize, AlignSize>& lhs,
                            type_storage<T, MinSize, AlignSize>& rhs,
                            bool                                 lhs_present,
                            bool                                 rhs_present)
{
  if (lhs_present and rhs_present) {
    lhs.move_assign(std::move(rhs));
  } else if (lhs_present) {
    lhs.destroy();
  } else if (rhs_present) {
    lhs.move_ctor(std::move(rhs));
  }
}

} // namespace detail

} // namespace srsran

#endif // SRSRAN_TYPE_STORAGE_H
