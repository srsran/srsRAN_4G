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

namespace detail {

template <typename Vec>
class base_optional_vector
{
  using base_t = base_optional_vector<Vec>;
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

  protected:
    friend base_t;

    base_t* parent = nullptr;
    size_t  idx    = std::numeric_limits<size_t>::max();
  };

  size_t nof_elems = 0;
  Vec    vec;

public:
  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  base_optional_vector()                            = default;
  base_optional_vector(const base_optional_vector&) = default;
  base_optional_vector(base_optional_vector&& other) noexcept : vec(std::move(other.vec)), nof_elems(other.nof_elems)
  {
    other.nof_elems = 0;
  }
  base_optional_vector& operator=(const base_optional_vector&) = default;
  base_optional_vector& operator                               =(base_optional_vector&& other) noexcept
  {
    vec       = std::move(other.vec);
    nof_elems = other.nof_elems;
    nof_elems = 0;
    return *this;
  }

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

  T&       operator[](size_t idx) { return *vec[idx]; }
  const T& operator[](size_t idx) const { return *vec[idx]; }

  bool   empty() const { return nof_elems == 0; }
  size_t size() const { return nof_elems; }

  iterator       begin() { return iterator{this, 0}; }
  iterator       end() { return iterator{this, vec.size()}; }
  const_iterator begin() const { return const_iterator{this, 0}; }
  const_iterator end() const { return const_iterator{this, vec.size()}; }
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
{
  using base_t = detail::base_optional_vector<std::array<optional<T>, N> >;

public:
  template <typename U>
  void insert(size_t idx, U&& u)
  {
    this->nof_elems += this->contains(idx) ? 0 : 1;
    this->vec[idx] = std::forward<U>(u);
  }
};

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
    if (not this->contains(idx)) {
      this->nof_elems++;
      this->vec.resize(std::max(idx + 1, this->vec.size()));
    }
    this->vec[idx] = std::forward<U>(u);
  }
};

} // namespace srsran

#endif // SRSRAN_OPTIONAL_ARRAY_H
