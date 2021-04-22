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

#ifndef SRSRAN_POOL_UTILS_H
#define SRSRAN_POOL_UTILS_H

#include "../move_callback.h"
#include <memory>

namespace srsran {

namespace detail {

template <typename T>
struct inplace_default_ctor_operator {
  void operator()(void* ptr) { new (ptr) T(); }
};

struct noop_operator {
  template <typename T>
  void operator()(T&& t) const
  {
    // do nothing
  }
};

} // namespace detail

/// check if alignment is power of 2
constexpr bool is_valid_alignment(std::size_t alignment)
{
  return alignment && (alignment & (alignment - 1)) == 0u;
}

inline bool is_aligned(void* ptr, std::size_t alignment)
{
  return (reinterpret_cast<std::uintptr_t>(ptr) & (alignment - 1)) == 0;
}

constexpr std::uintptr_t align_next(std::uintptr_t pos, size_t alignment)
{
  return (pos + (alignment - 1)) & ~(alignment - 1);
}
inline void* align_to(void* pos, size_t alignment)
{
  return reinterpret_cast<void*>(align_next(reinterpret_cast<std::uintptr_t>(pos), alignment));
}
inline void* offset_byte_ptr(void* pos, size_t offset)
{
  return static_cast<void*>(static_cast<uint8_t*>(pos) + offset);
}

} // namespace srsran

#endif // SRSRAN_POOL_UTILS_H
