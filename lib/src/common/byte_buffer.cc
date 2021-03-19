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

#include "srsran/common/byte_buffer.h"
#include "srsran/common/buffer_pool.h"

namespace srsran {

void* byte_buffer_t::operator new(size_t sz, const std::nothrow_t& nothrow_value) noexcept
{
  assert(sz == sizeof(byte_buffer_t));
  return byte_buffer_pool::get_instance()->allocate(nullptr, false);
}

void* byte_buffer_t::operator new(size_t sz)
{
  assert(sz == sizeof(byte_buffer_t));
  void* ptr = byte_buffer_pool::get_instance()->allocate(nullptr, false);
  if (ptr == nullptr) {
    throw std::bad_alloc();
  }
  return ptr;
}

void byte_buffer_t::operator delete(void* ptr)
{
  byte_buffer_pool::get_instance()->deallocate(ptr);
}

} // namespace srsran
