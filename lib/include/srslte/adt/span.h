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

#include <algorithm>
#include <array>
#include <cassert>
#include <iterator>
#include <type_traits>
#include <vector>

namespace srslte {

template <typename T>
class span;

namespace detail {

/// Helper traits used by SFINAE expressions in constructors.

template <typename... Ts>
struct make_void {
  typedef void type;
};

template <typename... Ts>
using void_t = typename make_void<Ts...>::type;

template <typename U>
struct is_span : std::false_type {};
template <typename U>
struct is_span<span<U> > : std::true_type {};

template <typename U>
struct is_std_array : std::false_type {};
template <typename U, std::size_t N>
struct is_std_array<std::array<U, N> > : std::true_type {};

template <typename U>
using remove_cvref_t = typename std::remove_cv<typename std::remove_reference<U>::type>::type;

template <class Container, class U, class = void>
struct is_container_compatible : public std::false_type {};
template <class Container, class U>
struct is_container_compatible<
    Container,
    U,
    void_t<
        // Check if the container type has data and size members.
        decltype(std::declval<Container>().data()),
        decltype(std::declval<Container>().size()),
        // Container should not be a span.
        typename std::enable_if<!is_span<remove_cvref_t<Container> >::value, int>::type,
        // Container should not be a std::array.
        typename std::enable_if<!is_std_array<remove_cvref_t<Container> >::value, int>::type,
        // Container should not be an array.
        typename std::enable_if<!std::is_array<remove_cvref_t<Container> >::value, int>::type,
        // Check type compatibility between the contained type and the span type.
        typename std::enable_if<
            std::is_convertible<typename std::remove_pointer<decltype(std::declval<Container>().data())>::type (*)[],
                                U (*)[]>::value,
            int>::type> > : public std::true_type {};

} // namespace detail

/// The class template span describes an object that can refer to a contiguous sequence of objects with the first
/// element of the sequence at position zero.
template <typename T>
class span
{
public:
  /// Member types.
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

  /// Constructs an empty span with data() == nullptr and size() == 0.
  constexpr span() noexcept = default;

  /// Constructs a span that is a view over the range [ptr, ptr + len).
  constexpr span(pointer ptr, size_type len) noexcept : ptr(ptr), len(len) {}

  /// Constructs a span that is a view over the range [first, last).
  constexpr span(pointer first, pointer last) noexcept : ptr(first), len(last - first) {}

  /// Constructs a span that is a view over the array arr.
  template <std::size_t N>
  constexpr span(element_type (&arr)[N]) noexcept : ptr(arr), len(N)
  {}

  /// Constructs a span that is a view over the array arr.
  template <typename U,
            std::size_t N,
            typename std::enable_if<std::is_convertible<U (*)[], element_type (*)[]>::value, int>::type = 0>
  constexpr span(std::array<U, N>& arr) noexcept : ptr(arr.data()), len(N)
  {}

  /// Constructs a span that is a view over the array arr.
  template <typename U,
            std::size_t N,
            typename std::enable_if<std::is_convertible<const U (*)[], element_type (*)[]>::value, int>::type = 0>
  constexpr span(const std::array<U, N>& arr) noexcept : ptr(arr.data()), len(N)
  {}

  /// Constructs a span that is a view over the container c.
  template <typename Container,
            typename std::enable_if<detail::is_container_compatible<Container, element_type>::value, int>::type = 0>
  constexpr span(Container& c) noexcept : ptr(c.data()), len(c.size())
  {}

  /// Constructs a span that is a view over the container c.
  template <
      typename Container,
      typename std::enable_if<detail::is_container_compatible<const Container, element_type>::value, int>::type = 0>
  constexpr span(const Container& c) noexcept : ptr(c.data()), len(c.size())
  {}

  template <typename U, typename std::enable_if<std::is_convertible<U (*)[], element_type (*)[]>::value, int>::type = 0>
  constexpr span(const span<U>& other) noexcept : ptr(other.data()), len(other.size())
  {}

  span& operator=(const span& other) noexcept = default;

  ~span() noexcept = default;

  /// Returns the number of elements in the span.
  constexpr size_type size() const noexcept { return len; }

  /// Returns the size of the sequence in bytes.
  constexpr size_type size_bytes() const noexcept { return len * sizeof(element_type); }

  /// Checks if the span is empty.
  constexpr bool empty() const noexcept { return size() == 0; }

  /// Returns a reference to the first element in the span.
  /// NOTE: Calling front on an empty span results in undefined behavior.
  reference front() const
  {
    assert(!empty() && "called front with empty span");
    return *data();
  }

  /// Returns a reference to the last element in the span.
  /// NOTE: Calling back on an empty span results in undefined behavior.
  reference back() const
  {
    assert(!empty() && "called back with empty span");
    return *(data() + size() - 1);
  }

  /// Returns a reference to the idx-th element of the sequence.
  /// NOTE: The behavior is undefined if idx is out of range.
  reference operator[](size_type idx) const
  {
    assert(idx < len && "index out of bounds!");
    return ptr[idx];
  }

  /// Returns a pointer to the beginning of the sequence.
  constexpr pointer data() const noexcept { return ptr; }

  /// Returns an iterator to the first element of the span.
  constexpr iterator begin() const noexcept { return data(); }

  /// Returns an iterator to the element following the last element of the span.
  constexpr iterator end() const noexcept { return data() + size(); }

  /// Returns a reverse iterator to the first element of the reversed span.
  constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }

  /// Returns a reverse iterator to the element following the last element of the reversed span.
  constexpr reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

  /// Obtains a span that is a view over the first count elements of this span.
  /// NOTE: The behavior is undefined if count > size().
  span<element_type> first(size_type count) const
  {
    assert(count <= size() && "count is out of range");
    return subspan(0, count);
  }

  /// Obtains a span that is a view over the last count elements of this span.
  /// NOTE: The behavior is undefined if count > size().
  span<element_type> last(size_type count) const
  {
    assert(count <= size() && "count is out of range");
    return subspan(size() - count, count);
  }

  /// Obtains a span that is a view over the count elements of this span starting at offset offset.
  span<element_type> subspan(size_type offset, size_type count) const
  {
    assert(count <= size() - offset && "size out of bounds!");
    return {data() + offset, count};
  }

  /// Returns true if the input span has the same elements as this.
  bool equals(span rhs) const { return (len == rhs.len) ? std::equal(begin(), end(), rhs.begin()) : false; }

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

} // namespace srslte

#endif // SRSLTE_SPAN_H
