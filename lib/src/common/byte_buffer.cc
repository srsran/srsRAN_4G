/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
