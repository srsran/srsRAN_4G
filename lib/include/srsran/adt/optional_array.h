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

#ifndef SRSRAN_OPTIONAL_ARRAY_H
#define SRSRAN_OPTIONAL_ARRAY_H

#include "optional.h"
#include "srsran/common/srsran_assert.h"
#include <array>

namespace srsran {

/**
 * Array of optional items. The iteration is in order of indexes and correctly skips non-present items
 * Pointer/References/Iterators remain valid throughout the object lifetime
 * NOTE: The sorted iteration and pointer validation guarantees add some overhead if the array is very fragmented
 * @tparam T type of objects
 * @tparam N static size of max nof items
 */
template <typename T, size_t N>
class optional_array
{
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
    iterator_impl(optional_array<T, N>* parent_, size_t idx_) : parent(parent_), idx(idx_)
    {
      if (idx < parent->capacity() and not parent->contains(idx)) {
        ++(*this);
      }
    }

    It& operator++()
    {
      while (++idx < parent->capacity() and not parent->contains(idx)) {
      }
      return *this;
    }
    It& operator--()
    {
      while (--idx < parent->capacity() and not parent->contains(idx)) {
      }
      return *this;
    }

    reference operator*() { return parent->operator[](idx); }
    pointer   operator->() { return &parent->operator[](idx); }

    bool operator==(const It& other) const { return idx == other.idx and parent == other.parent; }
    bool operator!=(const It& other) const { return not(*this == other); }

  protected:
    friend class optional_array<T, N>;

    optional_array<T, N>* parent = nullptr;
    size_t                idx    = N;
  };

public:
  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  optional_array()                      = default;
  optional_array(const optional_array&) = default;
  optional_array(optional_array&& other) noexcept : vec(std::move(other.vec)), nof_elems(other.nof_elems)
  {
    other.nof_elems = 0;
  }
  optional_array& operator=(const optional_array&) = default;
  optional_array& operator                         =(optional_array&& other) noexcept
  {
    vec       = std::move(other.vec);
    nof_elems = other.nof_elems;
    nof_elems = 0;
    return *this;
  }

  // Find first position that is empty
  size_t find_first_empty(size_t start_guess = 0)
  {
    if (nof_elems == capacity()) {
      return N;
    }
    for (size_t i = start_guess; i < N; ++i) {
      if (not vec[i].has_value()) {
        return i;
      }
    }
    return N;
  }
  template <typename U>
  void insert(size_t idx, U&& u)
  {
    nof_elems += contains(idx) ? 0 : 1;
    vec[idx] = std::forward<U>(u);
  }
  void erase(size_t idx)
  {
    if (contains(idx)) {
      nof_elems--;
      vec[idx].reset();
    }
  }
  void erase(iterator it) { erase(it.idx); }
  void clear()
  {
    nof_elems = 0;
    for (auto& e : *this) {
      e.reset();
    }
  }

  bool contains(size_t idx) const { return idx < N and vec[idx].has_value(); }

  T&       operator[](size_t idx) { return *vec[idx]; }
  const T& operator[](size_t idx) const { return *vec[idx]; }

  bool          empty() const { return nof_elems == 0; }
  size_t        size() const { return nof_elems; }
  static size_t capacity() { return N; }

  iterator       begin() { return iterator{this, 0}; }
  iterator       end() { return iterator{this, N}; }
  const_iterator begin() const { return const_iterator{this, 0}; }
  const_iterator end() const { return const_iterator{this, N}; }

private:
  size_t                     nof_elems = 0;
  std::array<optional<T>, N> vec;
};

} // namespace srsran

#endif // SRSRAN_OPTIONAL_ARRAY_H
