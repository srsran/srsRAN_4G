/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#ifndef SRSLTE_SPAN_H
#define SRSLTE_SPAN_H

#include "srslte/common/common.h"
#include <algorithm>
#include <array>
#include <cassert>

namespace srslte {

template <typename T>
class span
{
public:
  using element_type     = T;
  using value_type       = typename std::remove_cv<T>::type;
  using size_type        = std::size_t;
  using difference_type  = std::ptrdiff_t;
  using pointer          = element_type*;
  using const_pointer    = const element_type*;
  using reference        = element_type&;
  using const_reference  = const element_type&;
  using iterator         = pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;

  constexpr span() noexcept = default;
  constexpr span(pointer ptr_, size_type N_) noexcept : ptr(ptr_), len(N_) {}
  template <std::size_t N>
  constexpr span(element_type (&arr)[N]) noexcept : ptr(arr), len(N)
  {}
  template <std::size_t N>
  constexpr span(std::array<value_type, N>& arr) noexcept : ptr(arr.data()), len(N)
  {}
  template <std::size_t N>
  constexpr span(const std::array<value_type, N>& arr) noexcept : ptr(arr.data()), len(N)
  {}
  constexpr span(const std::initializer_list<T>& lst) :
    ptr(lst.begin() == lst.end() ? (T*)nullptr : lst.begin()),
    len(lst.size())
  {}
  constexpr span(byte_buffer_t& buffer) : ptr(buffer.msg), len(buffer.N_bytes) {}
  constexpr span(const byte_buffer_t& buffer) : ptr(buffer.msg), len(buffer.N_bytes) {}
  constexpr span(unique_byte_buffer_t& buffer) : ptr(buffer->msg), len(buffer->N_bytes) {}
  constexpr span(const unique_byte_buffer_t& buffer) : ptr(buffer->msg), len(buffer->N_bytes) {}
  template <typename Container>
  constexpr span(Container& cont) : ptr(cont.data()), len(cont.size())
  {}

  template <typename OtherElementType>
  constexpr span(const span<OtherElementType>& other) noexcept : ptr(other.ptr), len(other.size())
  {}
  ~span() noexcept = default;
  span& operator=(const span& other) noexcept = default;

  constexpr size_type size() const noexcept { return len; }
  reference           operator[](size_type idx) const
  {
    assert(idx < len && "index out of bounds!");
    return ptr[idx];
  }
  constexpr bool empty() const noexcept { return size() == 0; }
  constexpr reference front() const { return *data(); }
  constexpr reference back() const { return *(data() + size() - 1); }

  constexpr pointer data() const noexcept { return ptr; }

  constexpr iterator begin() const noexcept { return data(); }
  constexpr iterator end() const noexcept { return data() + size(); }
  constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
  constexpr reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

  bool equals(span rhs) const { return (len == rhs.len) ? std::equal(begin(), end(), rhs.begin()) : false; }

  // slicing operations
  span<element_type> subspan(size_type offset, size_type count) const
  {
    assert(count <= len && "size out of bounds!");
    return {data() + offset, count};
  }
  constexpr span<element_type> first(size_type count) const { return subspan(0, count); }
  constexpr span<element_type> last(size_type count) const { return subspan(size() - count, count); }

private:
  pointer   ptr = nullptr;
  size_type len = 0;
};

template <typename T>
inline bool operator==(span<T> lhs, span<T> rhs)
{
  return lhs.equals(rhs);
}

template <typename T>
inline bool operator!=(span<T> lhs, span<T> rhs)
{
  return not lhs.equals(rhs);
}

using byte_span = span<uint8_t>;

} // namespace srslte

#endif // SRSLTE_SPAN_H
