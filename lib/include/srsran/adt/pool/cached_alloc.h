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

#ifndef SRSRAN_CACHED_ALLOC_H
#define SRSRAN_CACHED_ALLOC_H

#include "../intrusive_list.h"
#include "memblock_cache.h"
#include <deque>
#include <queue>

namespace srsran {

/**
 * Custom Allocator that caches deallocated memory blocks in a stack to be reused in future allocations.
 * This minimizes the number of new/delete calls, when the rate of insertions/removals match (e.g. a queue)
 * This allocator is not thread-safe. It assumes the container is being used in a single-threaded environment,
 * or being mutexed when altered, which is a reasonable assumption
 * @tparam T object type
 */
template <typename T>
class cached_alloc : public std::allocator<T>
{
  struct memblock_t : public intrusive_double_linked_list_element<> {
    explicit memblock_t(size_t sz) : block_size(sz) {}
    size_t block_size;
  };
  const size_t min_n = (sizeof(memblock_t) + sizeof(T) - 1) / sizeof(T);

public:
  using value_type = T;

  ~cached_alloc()
  {
    while (not free_list.empty()) {
      memblock_t& b = free_list.front();
      free_list.pop_front();
      size_t n = b.block_size;
      b.~memblock_t();
      std::allocator<T>::deallocate(reinterpret_cast<T*>(&b), n);
    }
  }
  cached_alloc()                                 = default;
  cached_alloc(cached_alloc<T>&& other) noexcept = default;
  cached_alloc(const cached_alloc<T>& other) noexcept : cached_alloc() {}
  template <typename U>
  explicit cached_alloc(const cached_alloc<U>& other) noexcept : cached_alloc()
  {
    // start empty, as cached blocks cannot be copied
  }
  cached_alloc& operator=(const cached_alloc<T>& other) noexcept { return *this; }
  cached_alloc& operator=(cached_alloc&& other) noexcept = default;

  T* allocate(size_t n, const void* ptr = nullptr)
  {
    size_t req_n = std::max(n, min_n);
    for (memblock_t& b : free_list) {
      if (b.block_size == req_n) {
        free_list.pop(&b);
        b.~memblock_t();
        return reinterpret_cast<T*>(&b);
      }
    }
    return std::allocator<T>::allocate(req_n, ptr);
  }
  void deallocate(T* p, size_t n) noexcept
  {
    size_t req_n = std::max(n, min_n);
    auto*  block = reinterpret_cast<memblock_t*>(p);
    new (block) memblock_t(req_n);
    free_list.push_front(block);
  }

  template <typename U>
  struct rebind {
    using other = cached_alloc<U>;
  };

private:
  intrusive_double_linked_list<memblock_t> free_list;
};

} // namespace srsran

template <typename T1, typename T2>
bool operator==(const srsran::cached_alloc<T1>& lhs, const srsran::cached_alloc<T2>& rhs) noexcept
{
  return &lhs == &rhs;
}

template <typename T1, typename T2>
bool operator!=(const srsran::cached_alloc<T1>& lhs, const srsran::cached_alloc<T2>& rhs) noexcept
{
  return not(lhs == rhs);
}

namespace srsran {

template <typename T>
using deque = std::deque<T, cached_alloc<T> >;

template <typename T>
using queue = std::queue<T, srsran::deque<T> >;

} // namespace srsran

#endif // SRSRAN_CACHED_ALLOC_H
