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

#ifndef SRSRAN_OPTIONAL_ARRAY_H
#define SRSRAN_OPTIONAL_ARRAY_H

#include "optional.h"
#include "span.h"
#include "srsran/support/srsran_assert.h"
#include <array>

namespace srsran {

namespace detail {

template <typename Vec>
class base_optional_span
{
  using base_t = base_optional_span<Vec>;
  using T      = typename Vec::value_type::value_type;

protected:
  template <typename Obj>
  class iterator_impl
  {
    using It = iterator_impl<Obj>;

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Obj;
    using difference_type   = std::ptrdiff_t;
    using pointer           = Obj*;
    using reference         = Obj&;

    iterator_impl() = default;
    iterator_impl(base_t* parent_, size_t idx_) : parent(parent_), idx(idx_)
    {
      if (idx < parent->vec.size() and not parent->contains(idx)) {
        ++(*this);
      }
    }

    It& operator++()
    {
      while (++idx < parent->vec.size() and not parent->contains(idx)) {
      }
      return *this;
    }
    It& operator--()
    {
      while (--idx < parent->vec.size() and not parent->contains(idx)) {
      }
      return *this;
    }

    reference operator*() { return parent->operator[](idx); }
    pointer   operator->() { return &parent->operator[](idx); }

    bool operator==(const It& other) const { return idx == other.idx and parent == other.parent; }
    bool operator!=(const It& other) const { return not(*this == other); }

  private:
    friend base_t;

    base_t* parent = nullptr;
    size_t  idx    = std::numeric_limits<size_t>::max();
  };

  size_t nof_elems = 0;
  Vec    vec;

public:
  using value_type     = T;
  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  // Find first position that is empty
  size_t find_first_empty(size_t start_guess = 0)
  {
    if (nof_elems == vec.size()) {
      return vec.size();
    }
    for (size_t i = start_guess; i < vec.size(); ++i) {
      if (not vec[i].has_value()) {
        return i;
      }
    }
    return vec.size();
  }

  bool contains(size_t idx) const { return idx < vec.size() and vec[idx].has_value(); }

  T&       operator[](size_t idx) { return *vec[idx]; }
  const T& operator[](size_t idx) const { return *vec[idx]; }

  bool   empty() const { return nof_elems == 0; }
  size_t size() const { return nof_elems; }

  iterator       begin() { return iterator{this, 0}; }
  iterator       end() { return iterator{this, vec.size()}; }
  const_iterator begin() const { return const_iterator{this, 0}; }
  const_iterator end() const { return const_iterator{this, vec.size()}; }

  void clear()
  {
    this->nof_elems = 0;
    for (auto& e : *this) {
      e.reset();
    }
  }
  void erase(size_t idx)
  {
    srsran_assert(idx < this->vec.size(), "Out-of-bounds access to array: %zd>=%zd", idx, this->vec.size());
    if (this->contains(idx)) {
      this->nof_elems--;
      this->vec[idx].reset();
    }
  }
  void erase(iterator it) { erase(it.idx); }

  template <typename U>
  void insert(size_t idx, U&& u)
  {
    srsran_assert(idx < this->vec.size(), "Out-of-bounds access to array: %zd>=%zd", idx, this->vec.size());
    this->nof_elems += this->contains(idx) ? 0 : 1;
    this->vec[idx] = std::forward<U>(u);
  }
};

template <typename Vec>
class base_optional_vector : public base_optional_span<Vec>
{
  using base_t = base_optional_span<Vec>;

public:
  using value_type     = typename base_optional_span<Vec>::value_type;
  using iterator       = typename base_optional_span<Vec>::iterator;
  using const_iterator = typename base_optional_span<Vec>::const_iterator;

  base_optional_vector()                            = default;
  base_optional_vector(const base_optional_vector&) = default;
  base_optional_vector(base_optional_vector&& other) noexcept : base_t::vec(std::move(other.vec)),
                                                                base_t::nof_elems(other.nof_elems)
  {
    other.nof_elems = 0;
  }
  base_optional_vector& operator=(const base_optional_vector&) = default;
  base_optional_vector& operator                               =(base_optional_vector&& other) noexcept
  {
    this->vec       = std::move(other.vec);
    this->nof_elems = other.nof_elems;
    this->nof_elems = 0;
    return *this;
  }
};

} // namespace detail

/**
 * Array of optional items. The iteration is in order of indexes and correctly skips non-present items
 * Pointer/References/Iterators remain valid throughout the object lifetime
 * NOTE: The sorted iteration and pointer validation guarantees add some overhead if the array is very fragmented
 * @tparam T type of objects
 * @tparam N static size of max nof items
 */
template <typename T, size_t N>
class optional_array : public detail::base_optional_vector<std::array<optional<T>, N> >
{};

/**
 * Contrarily to optional_array, this class may allocate and cause pointer/reference/iterator invalidation.
 * However, the indexes will remain valid.
 * @tparam T
 */
template <typename T>
class optional_vector : public detail::base_optional_vector<std::vector<optional<T> > >
{
  using base_t = detail::base_optional_vector<std::vector<optional<T> > >;

public:
  /// May allocate and cause pointer invalidation
  template <typename U>
  void insert(size_t idx, U&& u)
  {
    if (idx >= this->vec.size()) {
      this->vec.resize(idx + 1);
    }
    base_t::insert(idx, std::forward<U>(u));
  }
};

template <typename T>
class optional_span : public detail::base_optional_span<srsran::span<optional<T> > >
{
  using base_t = detail::base_optional_span<srsran::span<optional<T> > >;

public:
  template <size_t N>
  optional_span(const optional_array<T, N>& ar) : base_t::vec(ar)
  {}
  optional_span(const optional_vector<T>& ar) : base_t::vec(ar) {}
};

namespace detail {

template <typename T>
class base_split_optional_span
{
protected:
  using presence_type = typename std::conditional<std::is_const<T>::value, const bool, bool>::type;

  T*             ptr         = nullptr;
  presence_type* present_ptr = nullptr;
  size_t         len         = 0;

  template <typename Obj>
  class iterator_impl
  {
    using It     = iterator_impl<Obj>;
    using Parent = typename std::
        conditional<std::is_const<Obj>::value, const base_split_optional_span<T>, base_split_optional_span<T> >::type;

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Obj;
    using difference_type   = std::ptrdiff_t;
    using pointer           = Obj*;
    using reference         = Obj&;

    iterator_impl() = default;
    iterator_impl(Parent* parent_, size_t idx_) : parent(parent_), idx(idx_)
    {
      if (idx < parent->len and not parent->contains(idx)) {
        ++(*this);
      }
    }

    It& operator++()
    {
      while (++idx < parent->len and not parent->contains(idx)) {
      }
      return *this;
    }
    It& operator--()
    {
      while (--idx < parent->len and not parent->contains(idx)) {
      }
      return *this;
    }

    reference operator*() { return parent->operator[](idx); }
    pointer   operator->() { return &parent->operator[](idx); }

    bool operator==(const It& other) const { return idx == other.idx and parent == other.parent; }
    bool operator!=(const It& other) const { return not(*this == other); }

    size_t get_idx() const { return idx; }

  private:
    Parent* parent = nullptr;
    size_t  idx    = std::numeric_limits<size_t>::max();
  };

public:
  using value_type     = T;
  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  constexpr base_split_optional_span() = default;
  template <std::size_t N>
  constexpr base_split_optional_span(value_type (&arr)[N], presence_type (&present)[N]) noexcept : ptr(arr),
                                                                                                   present_ptr(present),
                                                                                                   len(N)
  {}
  constexpr base_split_optional_span(value_type* arr, presence_type* present, size_t N) :
    ptr(arr), present_ptr(present), len(N)
  {}

  bool contains(size_t idx) const { return idx < len and present_ptr[idx]; }
  bool empty() const
  {
    for (size_t i = 0; i < len; ++i) {
      if (present_ptr[i]) {
        return false;
      }
    }
    return true;
  }
  size_t size() const
  {
    size_t c = 0;
    for (size_t i = 0; i < len; ++i) {
      c += present_ptr[i] ? 1 : 0;
    }
    return c;
  }
  size_t capacity() const { return len; }

  const T& operator[](size_t idx) const { return ptr[idx]; }
  T&       operator[](size_t idx) { return ptr[idx]; }
  const T& at(size_t idx) const
  {
    srsran_assert(contains(idx), "Access to inexistent element of index=%zd", idx);
    return ptr[idx];
  }
  T& at(size_t idx)
  {
    srsran_assert(this->contains(idx), "Access to inexistent element of index=%zd", idx);
    return this->ptr[idx];
  }

  const_iterator begin() const { return const_iterator(this, 0); }
  const_iterator end() const { return const_iterator(this, len); }
  iterator       begin() { return iterator(this, 0); }
  iterator       end() { return iterator(this, this->len); }

  // Find first position that is empty
  size_t find_first_empty(size_t start_guess = 0) { return begin().get_idx(); }
};

} // namespace detail

template <typename T>
class split_optional_span : public detail::base_split_optional_span<T>
{
  using base_t = detail::base_split_optional_span<T>;

public:
  using value_type     = T;
  using const_iterator = typename base_t::const_iterator;
  using iterator       = typename base_t::iterator;

  using base_t::base_t;

  template <typename U>
  void insert(size_t idx, U&& u)
  {
    srsran_assert(idx < this->len, "Out-of-bounds access to array: %zd>=%zd", idx, this->len);
    this->present_ptr[idx] = true;
    this->ptr[idx]         = std::forward<U>(u);
  }
  void erase(size_t idx)
  {
    srsran_assert(idx < this->len, "Out-of-bounds access to array: %zd>=%zd", idx, this->len);
    this->present_ptr[idx] = false;
  }
  void erase(iterator it) { erase(it.get_idx()); }
  void clear()
  {
    for (size_t i = 0; i < this->len; ++i) {
      this->present_ptr[i] = false;
    }
  }
};

template <typename U>
class split_optional_span<const U> : public detail::base_split_optional_span<const U>
{
  using base_t        = detail::base_split_optional_span<const U>;
  using presence_type = typename base_t::presence_type;

public:
  using value_type     = const U;
  using const_iterator = typename base_t::const_iterator;

  using base_t::base_t;
};

template <typename T>
split_optional_span<T>
make_optional_span(T*                                                                          array,
                   typename std::conditional<std::is_const<T>::value, const bool, bool>::type* present,
                   size_t                                                                      N)
{
  return split_optional_span<T>(array, present, N);
}
template <typename T, size_t N>
split_optional_span<T>
    make_optional_span(T (&array)[N],
                       typename std::conditional<std::is_const<T>::value, const bool, bool>::type (&present)[N])
{
  return split_optional_span<T>(array, present);
}

} // namespace srsran

#endif // SRSRAN_OPTIONAL_ARRAY_H
