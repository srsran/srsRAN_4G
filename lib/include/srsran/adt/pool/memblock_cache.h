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

#ifndef SRSRAN_MEMBLOCK_CACHE_H
#define SRSRAN_MEMBLOCK_CACHE_H

#include <mutex>

namespace srsran {

/// Stores provided mem blocks in a stack in an non-owning manner. Not thread-safe
class memblock_cache
{
  struct node {
    node* prev;
    explicit node(node* prev_) : prev(prev_) {}
  };

public:
  constexpr static size_t min_memblock_size() { return sizeof(node); }

  memblock_cache() = default;

  memblock_cache(const memblock_cache&) = delete;

  memblock_cache(memblock_cache&& other) noexcept : head(other.head) { other.head = nullptr; }

  memblock_cache& operator=(const memblock_cache&) = delete;

  memblock_cache& operator=(memblock_cache&& other) noexcept
  {
    head       = other.head;
    other.head = nullptr;
    return *this;
  }

  void push(void* block) noexcept
  {
    // printf("head: %ld\n", (long)head);
    node* next = ::new (block) node(head);
    head       = next;
    count++;
  }

  uint8_t* try_pop() noexcept
  {
    if (is_empty()) {
      return nullptr;
    }
    node* last_head = head;
    head            = head->prev;
    count--;
    return (uint8_t*)last_head;
  }

  bool is_empty() const { return head == nullptr; }

  size_t size() const { return count; }

  void clear() { head = nullptr; }

private:
  node*  head  = nullptr;
  size_t count = 0;
};

/// memblock stack that mutexes pushing/popping
class mutexed_memblock_cache
{
public:
  mutexed_memblock_cache() = default;

  mutexed_memblock_cache(const mutexed_memblock_cache&) = delete;

  mutexed_memblock_cache(mutexed_memblock_cache&& other) noexcept
  {
    std::unique_lock<std::mutex> lk1(other.mutex, std::defer_lock);
    std::unique_lock<std::mutex> lk2(mutex, std::defer_lock);
    std::lock(lk1, lk2);
    stack = std::move(other.stack);
  }

  mutexed_memblock_cache& operator=(const mutexed_memblock_cache&) = delete;

  mutexed_memblock_cache& operator=(mutexed_memblock_cache&& other) noexcept
  {
    std::unique_lock<std::mutex> lk1(other.mutex, std::defer_lock);
    std::unique_lock<std::mutex> lk2(mutex, std::defer_lock);
    std::lock(lk1, lk2);
    stack = std::move(other.stack);
    return *this;
  }

  void push(void* block) noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    stack.push(block);
  }

  void steal_blocks(memblock_cache& other, size_t max_n) noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    for (size_t i = 0; i < max_n and not other.is_empty(); ++i) {
      stack.push(other.try_pop());
    }
  }

  uint8_t* try_pop() noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    uint8_t*                    block = stack.try_pop();
    return block;
  }

  template <size_t N>
  size_t try_pop(std::array<void*, N>& result) noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    size_t                      i = 0;
    for (; i < N; ++i) {
      result[i] = stack.try_pop();
      if (result[i] == nullptr) {
        break;
      }
    }
    return i;
  }

  bool is_empty() const noexcept { return stack.is_empty(); }

  size_t size() const noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    return stack.size();
  }

  void clear()
  {
    std::lock_guard<std::mutex> lock(mutex);
    stack.clear();
  }

private:
  memblock_cache     stack;
  mutable std::mutex mutex;
};

} // namespace srsran

#endif // SRSRAN_MEMBLOCK_CACHE_H
