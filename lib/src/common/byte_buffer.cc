// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#include "srsran/common/byte_buffer.h"
#include "srsran/common/buffer_pool.h"

namespace srsran {

void* byte_buffer_t::operator new(size_t sz, const std::nothrow_t& nothrow_value) noexcept
{
  assert(sz == sizeof(byte_buffer_t));
  return byte_buffer_pool::get_instance()->allocate_node(sz);
}

void* byte_buffer_t::operator new(size_t sz)
{
  assert(sz == sizeof(byte_buffer_t));
  void* ptr = byte_buffer_pool::get_instance()->allocate_node(sz);
  if (ptr == nullptr) {
    throw std::bad_alloc();
  }
  return ptr;
}

void byte_buffer_t::operator delete(void* ptr)
{
  byte_buffer_pool::get_instance()->deallocate_node(ptr);
}

} // namespace srsran
