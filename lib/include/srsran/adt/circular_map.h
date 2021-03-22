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

#ifndef SRSRAN_ID_MAP_H
#define SRSRAN_ID_MAP_H

#include "expected.h"
#include "srsran/common/srsran_assert.h"
#include <array>

namespace srsran {

template <typename K, typename T, size_t N>
class static_circular_map
{
  static_assert(std::is_integral<K>::value and std::is_unsigned<K>::value, "Map key must be an unsigned integer");

  using obj_t         = std::pair<K, T>;
  using obj_storage_t = typename std::aligned_storage<sizeof(obj_t), alignof(obj_t)>::type;

public:
  class iterator
  {
  public:
    iterator() = default;
    iterator(static_circular_map<K, T, N>* map, size_t idx_) : ptr(map), idx(idx_)
    {
      if (idx < ptr->capacity() and not ptr->present[idx]) {
        ++(*this);
      }
    }

    iterator& operator++()
    {
      while (++idx < ptr->capacity() and not ptr->present[idx]) {
      }
      return *this;
    }

    obj_t& operator*()
    {
      srsran_assert(idx < ptr->buffer.size(), "Iterator out-of-bounds (%zd >= %zd)", idx, ptr->buffer.size());
      return ptr->get_obj_(idx);
    }
    obj_t* operator->()
    {
      srsran_assert(idx < ptr->buffer.size(), "Iterator out-of-bounds (%zd >= %zd)", idx, ptr->buffer.size());
      return &ptr->get_obj_(idx);
    }
    const obj_t* operator*() const
    {
      srsran_assert(idx < ptr->buffer.size(), "Iterator out-of-bounds (%zd >= %zd)", idx, ptr->buffer.size());
      return ptr->buffer[idx];
    }
    const obj_t* operator->() const
    {
      srsran_assert(idx < ptr->buffer.size(), "Iterator out-of-bounds (%zd >= %zd)", idx, ptr->buffer.size());
      return ptr->buffer[idx];
    }

    bool operator==(const iterator& other) const { return ptr == other.ptr and idx == other.idx; }
    bool operator!=(const iterator& other) const { return not(*this == other); }

  private:
    friend class static_circular_map<K, T, N>;
    static_circular_map<K, T, N>* ptr = nullptr;
    size_t                        idx = 0;
  };
  class const_iterator
  {
  public:
    const_iterator() = default;
    const_iterator(static_circular_map<K, T, N>* map, size_t idx_) : ptr(map), idx(idx_) {}

    const_iterator& operator++()
    {
      while (++idx < ptr->capacity() and not ptr->present[idx]) {
      }
      return *this;
    }

    const obj_t* operator*() const { return ptr->buffer[idx]; }
    const obj_t* operator->() const { return ptr->buffer[idx]; }

    bool operator==(const const_iterator& other) const { return ptr == other.ptr and idx == other.idx; }
    bool operator!=(const const_iterator& other) const { return not(*this == other); }

  private:
    friend class static_circular_map<K, T, N>;
    const static_circular_map<K, T, N>* ptr = nullptr;
    size_t                              idx = 0;
  };

  static_circular_map() { std::fill(present.begin(), present.end(), false); }
  static_circular_map(const static_circular_map<K, T, N>& other) : present(other.present), count(other.count)
  {
    for (size_t idx = 0; idx < other.size(); ++idx) {
      if (present[idx]) {
        new (&buffer[idx]) obj_t(other.get_obj_(idx));
      }
    }
  }
  static_circular_map(static_circular_map<K, T, N>&& other) noexcept : present(other.present), count(other.count)
  {
    for (size_t idx = 0; idx < other.size(); ++idx) {
      if (present[idx]) {
        new (&buffer[idx]) obj_t(std::move(other.get_obj_(idx)));
      }
    }
    other.clear();
  }
  ~static_circular_map() { clear(); }
  static_circular_map& operator=(const static_circular_map<K, T, N>& other)
  {
    if (this == &other) {
      return *this;
    }
    clear();
    count   = other.count;
    present = other.present;
    for (size_t idx = 0; idx < other.size(); ++idx) {
      if (present[idx]) {
        new (&buffer[idx]) obj_t(other.get_obj_(idx));
      }
    }
  }
  static_circular_map& operator=(static_circular_map<K, T, N>&& other) noexcept
  {
    clear();
    count   = other.count;
    present = other.present;
    for (size_t idx = 0; idx < other.size(); ++idx) {
      if (present[idx]) {
        new (&buffer[idx]) obj_t(std::move(other.get_obj_(idx)));
      }
    }
    clear();
  }

  bool contains(K id)
  {
    size_t idx = id % N;
    return present[idx] and get_obj_(idx).first == id;
  }

  bool insert(K id, const T& obj)
  {
    size_t idx = id % N;
    if (present[idx]) {
      return false;
    }
    new (&buffer[idx]) obj_t(id, obj);
    present[idx] = true;
    count++;
    return true;
  }
  srsran::expected<iterator, T> insert(K id, T&& obj)
  {
    size_t idx = id % N;
    if (present[idx]) {
      return srsran::expected<iterator, T>(std::move(obj));
    }
    new (&buffer[idx]) obj_t(id, std::move(obj));
    present[idx] = true;
    count++;
    return iterator(this, idx);
  }

  bool erase(K id)
  {
    if (not contains(id)) {
      return false;
    }
    size_t idx = id % N;
    get_obj_(idx).~obj_t();
    present[idx] = false;
    --count;
    return true;
  }

  iterator erase(iterator it)
  {
    srsran_assert(it.idx < N, "Iterator out-of-bounds (%zd >= %zd)", it.idx, N);
    iterator next = it;
    ++next;
    it->~obj_t();
    present[it->first] = false;
    --count;
    return next;
  }

  void clear()
  {
    for (auto it = begin(); it != end();) {
      it = erase(it);
    }
  }

  T& operator[](K id)
  {
    srsran_assert(contains(id), "Accessing non-existent ID=%zd", (size_t)id);
    return get_obj_(id % N).second;
  }
  const T& operator[](K id) const
  {
    srsran_assert(contains(id), "Accessing non-existent ID=%zd", (size_t)id);
    return get_obj_(id % N).second;
  }

  size_t size() const { return count; }
  bool   empty() const { return count == 0; }
  bool   full() const { return count == N; }
  size_t capacity() const { return N; }

  iterator       begin() { return iterator(this, 0); }
  iterator       end() { return iterator(this, N); }
  const_iterator begin() const { return iterator(this, 0); }
  const_iterator end() const { return iterator(this, N); }

  iterator find(K id)
  {
    if (contains(id)) {
      return iterator(this, id % N);
    }
    return end();
  }
  const_iterator find(K id) const
  {
    if (contains(id)) {
      return iterator(this, id % N);
    }
    return end();
  }

private:
  obj_t&       get_obj_(size_t idx) { return reinterpret_cast<obj_t&>(buffer[idx]); }
  const obj_t& get_obj_(size_t idx) const { return reinterpret_cast<obj_t&>(buffer[idx]); }

  std::array<obj_storage_t, N> buffer;
  std::array<bool, N>          present;
  size_t                       count = 0;
};

} // namespace srsran

#endif // SRSRAN_ID_MAP_H
