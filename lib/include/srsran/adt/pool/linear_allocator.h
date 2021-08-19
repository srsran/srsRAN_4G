/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_LINEAR_ALLOCATOR_H
#define SRSRAN_LINEAR_ALLOCATOR_H

#include "pool_utils.h"
#include "support/srsran_assert.h"

namespace srsran {

class linear_allocator
{
public:
  linear_allocator() = default;
  linear_allocator(void* start_, void* end_) :
    start(static_cast<uint8_t*>(start_)), end(static_cast<uint8_t*>(end_)), cur(start)
  {}
  linear_allocator(void* start_, size_t sz) : start(static_cast<uint8_t*>(start_)), end(start + sz), cur(start) {}
  linear_allocator(const linear_allocator& other) = delete;
  linear_allocator(linear_allocator&& other) noexcept : start(other.start), end(other.end), cur(other.cur)
  {
    other.clear();
  }
  linear_allocator& operator=(const linear_allocator& other) = delete;
  linear_allocator& operator                                 =(linear_allocator&& other) noexcept
  {
    start = other.start;
    end   = other.end;
    cur   = other.cur;
    other.clear();
    return *this;
  }

  void* allocate(size_t sz, size_t alignment)
  {
    void*    alloc_start = align_to(cur, alignment);
    uint8_t* new_cur     = static_cast<uint8_t*>(alloc_start) + sz;
    if (new_cur > end) {
      // Cannot fit allocation in memory block
      return nullptr;
    }
    cur = new_cur;
    return alloc_start;
  }

  void deallocate(void* p) { srsran_assert(p >= start and p < end, "pointer does not belong to pool"); }

  size_t nof_bytes_allocated() const { return cur - start; }
  size_t nof_bytes_left() const { return end - cur; }
  size_t size() const { return end - start; }
  bool   is_init() const { return start != end; }
  void*  memblock_ptr() { return static_cast<void*>(start); }

  void clear()
  {
    start = nullptr;
    cur   = nullptr;
    end   = nullptr;
  }

protected:
  uint8_t* start = nullptr;
  uint8_t* end   = nullptr;
  uint8_t* cur   = nullptr;
};

} // namespace srsran

#endif // SRSRAN_LINEAR_ALLOCATOR_H
