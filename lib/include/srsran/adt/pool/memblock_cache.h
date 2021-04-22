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

#ifndef SRSRAN_MEMBLOCK_CACHE_H
#define SRSRAN_MEMBLOCK_CACHE_H

#include "pool_utils.h"
#include <mutex>

namespace srsran {

namespace detail {

class intrusive_memblock_list
{
public:
  struct node {
    node* next;
    explicit node(node* prev_) : next(prev_) {}
  };
  node*  head  = nullptr;
  size_t count = 0;

  constexpr static size_t min_memblock_size() { return sizeof(node); }
  constexpr static size_t min_memblock_align() { return alignof(node); }

  void push(void* block) noexcept
  {
    srsran_assert(is_aligned(block, min_memblock_align()), "The provided memory block is not aligned");
    node* ptr = ::new (block) node(head);
    head      = ptr;
    count++;
  }

  void* pop() noexcept
  {
    srsran_assert(not empty(), "pop() called on empty list");
    node* last_head = head;
    head            = head->next;
    last_head->~node();
    count--;
    return static_cast<void*>(last_head);
  }

  void* try_pop() noexcept { return empty() ? nullptr : pop(); }

  bool empty() const noexcept { return head == nullptr; }

  size_t size() const { return count; }

  void clear() noexcept
  {
    head  = nullptr;
    count = 0;
  }
};

} // namespace detail

/**
 * List of memory blocks. It overwrites bytes of blocks passed via push(void*). Thus, it is not safe to use in any
 * pool of initialized objects
 */
class free_memblock_list : public detail::intrusive_memblock_list
{
private:
  using base_t = detail::intrusive_memblock_list;
  using base_t::count;
  using base_t::head;
};

/**
 * List of memory blocks, each memory block containing a node. Memory Structure:
 *  memory block 1     memory block
 * [ next | node ]   [ next | node ]
 *    '--------------^  '-----------> nullptr
 */
class memblock_node_list : public detail::intrusive_memblock_list
{
  using base_t = detail::intrusive_memblock_list;
  using base_t::count;
  using base_t::head;
  using base_t::try_pop;

public:
  const size_t memblock_alignment;
  const size_t header_size;
  const size_t payload_size;
  const size_t memblock_size;

  explicit memblock_node_list(size_t node_size_, size_t node_alignment_ = detail::max_alignment) :
    memblock_alignment(std::max(free_memblock_list::min_memblock_align(), node_alignment_)),
    header_size(align_next(base_t::min_memblock_size(), memblock_alignment)),
    payload_size(align_next(node_size_, memblock_alignment)),
    memblock_size(header_size + payload_size)
  {
    srsran_assert(node_size_ > 0 and is_valid_alignment(node_alignment_),
                  "Invalid arguments node size=%zd,alignment=%zd",
                  node_size_,
                  node_alignment_);
  }

  void* get_node_header(void* payload_addr)
  {
    srsran_assert(is_aligned(payload_addr, memblock_alignment), "Provided address is not valid");
    return static_cast<void*>(static_cast<uint8_t*>(payload_addr) - header_size);
  }

  /// returns address of memblock payload (skips memblock header)
  void* top() noexcept { return static_cast<void*>(reinterpret_cast<uint8_t*>(this->head) + header_size); }

  void steal_top(intrusive_memblock_list& other) noexcept
  {
    srsran_assert(not other.empty(), "Trying to steal from empty memblock list");
    node* other_head = other.head;
    other.head       = other.head->next;
    other_head->next = head;
    head             = other_head;
    other.count--;
    count++;
  }
};

/// Similar to node_memblock_list, but manages the allocation/deallocation of memory blocks
class memblock_stack
{
public:
  explicit memblock_stack(size_t node_size_, size_t node_alignment_ = detail::max_alignment) :
    node_list(node_size_, node_alignment_)
  {}
  memblock_stack(const memblock_stack&)           = delete;
  memblock_stack(memblock_stack&& other) noexcept = delete;
  memblock_stack& operator=(const memblock_stack&) = delete;
  memblock_stack& operator=(memblock_stack&&) = delete;
  ~memblock_stack() { clear(); }

  void clear()
  {
    while (not empty()) {
      deallocate_block();
    }
  }

  size_t get_memblock_size() const { return node_list.memblock_size; }
  size_t get_node_max_size() const { return node_list.payload_size; }

  void* allocate_block()
  {
    node_list.push(new uint8_t[node_list.memblock_size]);
    return current_node();
  }

  void deallocate_block() noexcept
  {
    uint8_t* block = static_cast<uint8_t*>(node_list.pop());
    delete[] block;
  }

  bool   empty() const noexcept { return node_list.empty(); }
  size_t size() const noexcept { return node_list.size(); }
  void*  current_node() noexcept { return node_list.top(); }
  void   steal_top(memblock_stack& other) noexcept { return node_list.steal_top(other.node_list); }

private:
  static size_t get_memblock_start_offset(size_t node_alignment)
  {
    return align_next(detail::intrusive_memblock_list::min_memblock_size(), node_alignment);
  }
  static size_t get_memblock_size(size_t node_size, size_t node_alignment)
  {
    return align_next(get_memblock_start_offset(node_alignment) + node_size, detail::max_alignment);
  }

  memblock_node_list node_list;
};

/// memblock stack that mutexes pushing/popping
class concurrent_free_memblock_list
{
public:
  concurrent_free_memblock_list()                                     = default;
  concurrent_free_memblock_list(const concurrent_free_memblock_list&) = delete;
  concurrent_free_memblock_list(concurrent_free_memblock_list&& other) noexcept
  {
    std::unique_lock<std::mutex> lk1(other.mutex, std::defer_lock);
    std::unique_lock<std::mutex> lk2(mutex, std::defer_lock);
    std::lock(lk1, lk2);
    stack = other.stack;
  }
  concurrent_free_memblock_list& operator=(const concurrent_free_memblock_list&) = delete;
  concurrent_free_memblock_list& operator=(concurrent_free_memblock_list&& other) noexcept
  {
    std::unique_lock<std::mutex> lk1(other.mutex, std::defer_lock);
    std::unique_lock<std::mutex> lk2(mutex, std::defer_lock);
    std::lock(lk1, lk2);
    stack = other.stack;
    return *this;
  }

  void push(void* block) noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    stack.push(block);
  }

  void steal_blocks(free_memblock_list& other, size_t max_n) noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    for (size_t i = 0; i < max_n and not other.empty(); ++i) {
      stack.push(other.try_pop());
    }
  }

  void* try_pop() noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    void*                       block = stack.try_pop();
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

  bool empty() const noexcept { return stack.empty(); }

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
  free_memblock_list stack;
  mutable std::mutex mutex;
};

/**
 * Manages the allocation, caching and deallocation of memory blocks.
 * On alloc, a memory block is stolen from cache. If cache is empty, malloc/new is called.
 * Only the last allocated memory block can be deallocated.
 */
class cached_memblock_stack
{
public:
  explicit cached_memblock_stack(size_t block_size_) : used(block_size_), cache(block_size_) {}

  void* allocate_block()
  {
    if (cache.empty()) {
      used.allocate_block();
    } else {
      used.steal_top(cache);
    }
    return used.current_node();
  }

  void*  current_node() noexcept { return used.current_node(); }
  void   deallocate_block() noexcept { cache.steal_top(used); }
  size_t cache_size() const noexcept { return cache.size(); }

private:
  memblock_stack used;
  memblock_stack cache;
};

} // namespace srsran

#endif // SRSRAN_MEMBLOCK_CACHE_H
