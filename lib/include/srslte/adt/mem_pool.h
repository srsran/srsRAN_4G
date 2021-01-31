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

#ifndef SRSLTE_MEM_POOL_H
#define SRSLTE_MEM_POOL_H

#include <cassert>
#include <cstdint>
#include <memory>
#include <mutex>

namespace srslte {

/// Stores provided mem blocks in a stack in an non-owning manner. Not thread-safe
class memblock_stack
{
  struct node {
    node* prev;
    explicit node(node* prev_) : prev(prev_) {}
  };

public:
  constexpr static size_t min_memblock_size() { return sizeof(node); }

  memblock_stack() = default;

  memblock_stack(const memblock_stack&) = delete;

  memblock_stack(memblock_stack&& other) noexcept : head(other.head) { other.head = nullptr; }

  memblock_stack& operator=(const memblock_stack&) = delete;

  memblock_stack& operator=(memblock_stack&& other) noexcept
  {
    head       = other.head;
    other.head = nullptr;
    return *this;
  }

  void push(uint8_t* block) noexcept
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
class mutexed_memblock_stack
{
public:
  mutexed_memblock_stack() = default;

  mutexed_memblock_stack(const mutexed_memblock_stack&) = delete;

  mutexed_memblock_stack(mutexed_memblock_stack&& other) noexcept
  {
    std::unique_lock<std::mutex> lk1(other.mutex, std::defer_lock);
    std::unique_lock<std::mutex> lk2(mutex, std::defer_lock);
    std::lock(lk1, lk2);
    stack = std::move(other.stack);
  }

  mutexed_memblock_stack& operator=(const mutexed_memblock_stack&) = delete;

  mutexed_memblock_stack& operator=(mutexed_memblock_stack&& other) noexcept
  {
    std::unique_lock<std::mutex> lk1(other.mutex, std::defer_lock);
    std::unique_lock<std::mutex> lk2(mutex, std::defer_lock);
    std::lock(lk1, lk2);
    stack = std::move(other.stack);
    return *this;
  }

  void push(uint8_t* block) noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    stack.push(block);
  }

  uint8_t* try_pop() noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    uint8_t*                    block = stack.try_pop();
    return block;
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
  memblock_stack     stack;
  mutable std::mutex mutex;
};

/**
 * Pool specialized for big objects. Created objects are not contiguous in memory.
 * Relevant methods:
 * - ::allocate_node(sz) - allocate memory of sizeof(T), or reuse memory already present in cache
 * - ::deallocate_node(void* p) - return memory addressed by p back to the pool to be cached.
 * - ::reserve(N) - prereserve memory slots for faster object creation
 * @tparam ObjSize object memory size
 * @tparam ThreadSafe if object pool is thread-safe or not
 */
template <typename T, bool ThreadSafe = false>
class big_obj_pool
{
  // memory stack type derivation (thread safe or not)
  using stack_type = typename std::conditional<ThreadSafe, mutexed_memblock_stack, memblock_stack>::type;

  // memory stack to cache allocate memory chunks
  stack_type stack;

public:
  ~big_obj_pool() { clear(); }

  /// alloc new object space. If no memory is pre-reserved in the pool, malloc is called.
  void* allocate_node(size_t sz)
  {
    assert(sz == sizeof(T));
    static const size_t blocksize = std::max(sizeof(T), memblock_stack::min_memblock_size());
    uint8_t*            block     = stack.try_pop();
    if (block == nullptr) {
      block = new uint8_t[blocksize];
    }
    return block;
  }

  void deallocate_node(void* p)
  {
    if (p != nullptr) {
      stack.push(static_cast<uint8_t*>(p));
    }
  }

  /// Pre-reserve N memory chunks for future object allocations
  void reserve(size_t N)
  {
    static const size_t blocksize = std::max(sizeof(T), memblock_stack::min_memblock_size());
    for (size_t i = 0; i < N; ++i) {
      stack.push(new uint8_t[blocksize]);
    }
  }

  size_t capacity() const { return stack.size(); }

  void clear()
  {
    uint8_t* block = stack.try_pop();
    while (block != nullptr) {
      delete[] block;
      block = stack.try_pop();
    }
  }
};

} // namespace srslte

#endif // SRSLTE_MEM_POOL_H
